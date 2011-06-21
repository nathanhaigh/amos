/* **********************************************************
 * Copyright (c) 2008 VMware, Inc.  All rights reserved.
 * **********************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of VMware, Inc. nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL VMWARE, INC. OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

/* Code Manipulation API Sample:
 * cbr.c
 *
 * This sample shows how to update or replace instrumented code after
 * it executes.  We focus on cbr instructions, inserting
 * instrumentation to record the fallthrough and taken addresses when
 * they first execute.  After a particular branch first executes, we
 * re-instrument the basic block to remove the instrumentation for the
 * direction taken.  If and when we see the other direction, we remove
 * all instrumentation for that branch.  We design this sample to
 * avoid the instrumentation overhead for a particular direction until
 * it is taken.  Furthermore, we remove all overhead for that
 * direction after it triggers.
 *
 * This sample might form part of a dynamic CFG builder, where we want
 * to record each control-flow edge, but we don't want to pay the
 * execution overhead of the instrumentation after we've noted the
 * edge.
 *
 * We use the following replacement scheme:
 * 1) In the BB event, insert instrumentation for both the taken and
 *    fallthrough edges.
 * 2) When the BB executes, note the direction taken and flush the
 *    fragment from the code cache.
 * 3) When the BB event triggers again, insert new instrumentation.
 */

#include "dr_api.h"

#define MINSERT instrlist_meta_preinsert

#define ASSERT(x) \
    do {                                                        \
        if (!(x)) {                                             \
            dr_printf("ASSERT failed on line %d", __LINE__);    \
            dr_flush_file(STDOUT);                              \
            dr_abort();                                         \
        }                                                       \
    } while (0)                             

/* We need a table to store the state of each cbr (i.e., "seen taken
 * edge", "seen fallthrough edge", or "seen both").  We'll use a
 * simple hash table.
 */
#define HASH_TABLE_SIZE 7919

/* Possible cbr states */
typedef enum {
    CBR_NEITHER   = 0x00,
    CBR_TAKEN     = 0x01,
    CBR_NOT_TAKEN = 0x10
} cbr_state_t;

/* Each bucket in the hash table is a list of the following elements.
 * For each cbr, we store its address and its state.
 */
typedef struct _elem_t {
    struct _elem_t *next;
    cbr_state_t state;
    app_pc addr;
} elem_t;

typedef struct _list_t {
    elem_t *head;
    elem_t *tail;
} list_t;


/* We'll use one global hash table */
typedef list_t **hash_table_t;
//hash_table_t table = NULL;

static
elem_t *new_elem(app_pc addr, cbr_state_t state)
{
    elem_t *elem = (elem_t *)dr_global_alloc(sizeof(elem_t));
    ASSERT(elem != NULL);

    elem->next  = NULL;
    elem->addr  = addr;
    elem->state = state;
    return elem;
}

static
void delete_elem(elem_t *elem)
{
    dr_global_free(elem, sizeof(elem_t));
}

static
elem_t *find_elem(list_t *list, app_pc addr)
{
    elem_t *elem = list->head;
    while (elem != NULL) {
        if (elem->addr == addr)
            return elem;
        elem = elem->next;
    }

    return NULL;
}

static
void append_elem(list_t *list, elem_t *elem)
{
    if (list->head == NULL) {
        ASSERT(list->tail == NULL);
        list->head = elem;
        list->tail = elem;
    }
    else {
        list->tail->next = elem;
        list->tail = elem;
    }  
  
}

static
list_t *new_list()
{
    list_t *list = (list_t *)dr_global_alloc(sizeof(list_t));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

/*static void
load_to_datastructure(list_t *mylist, app_pc source, app_pc dest)
{
	static elem_t *el, *temp,*tempdest;
	temp = find_elem(mylist, source);
	tempdest =  new_elem(dest, CBR_TAKEN);
	if (temp == NULL) {
		el = new_elem(source, CBR_TAKEN);
		append_elem(mylist, el);
		el->neighbors= new_list();
		append_elem(el->neighbors, tempdest);	
	}
	else {
		append_elem(temp->neighbors, tempdest);	
	}
	//dr_fprintf(STDERR, "loaded: %x %x\n", (app_pc)source, (app_pc)dest);
}
*/
static
void delete_list(list_t *list)
{
    elem_t *iter = list->head;
    while (iter != NULL) {
        elem_t *next = iter->next;
        delete_elem(iter);
        iter = next;
    }

    dr_global_free(list, sizeof(list_t));
}
static
void print_list(list_t *list)
{
	elem_t *elem = list->head;
	while (elem != NULL) {
		dr_fprintf(STDERR, "%x-", elem->addr);
		elem = elem->next;	
	}
	
}
hash_table_t new_table()
{
    int i;
    hash_table_t table = (hash_table_t)dr_global_alloc
        (sizeof(list_t *) * HASH_TABLE_SIZE);

    for (i=0; i<HASH_TABLE_SIZE; i++) {
        table[i] = NULL;
    }

    return table;
}

void delete_table(hash_table_t table)
{
    int i;
    for (i=0; i<HASH_TABLE_SIZE; i++) {
        if (table[i] != NULL) {
            delete_list(table[i]);
        }
    }

    dr_global_free(table, sizeof(list_t *) * HASH_TABLE_SIZE);
}

static
uint hash_func(app_pc addr)
{
    return ((uint)(((ptr_uint_t)addr) % HASH_TABLE_SIZE));
}

elem_t *lookup(hash_table_t table, app_pc src, app_pc dst)
{
    list_t *list = table[hash_func(src)];
    if (list != NULL)
        return find_elem(list, dst);

    return NULL;
}

void insert(hash_table_t table, app_pc src, app_pc dst, cbr_state_t state)
{
    elem_t *elem = new_elem(src, state);

    uint index = hash_func(src);
    list_t *list = table[index];
    if (list == NULL) {
        list = new_list();
        table[index] = list;
    }
    /*if (table[index]== NULL) table[index] = new_list();
    append_elem(table[index], elem);
    elem = new_elem(dst, state);
    append_elem(table[index], elem);*/
    if (lookup(table, src,src) == NULL) append_elem(list, elem);
    elem = new_elem(dst, state);
    if (lookup(table, src,dst) == NULL)append_elem(list, elem);
}


/*
 * End hash table implementation 
 */
void print_hashtable(hash_table_t table)
{

/* Print all the cbr's seen over the life of the process, and
     * whether we saw taken, not taken, or both.
     */
    int i;
    for (i=0; i<HASH_TABLE_SIZE; i++) {
        if (table[i] != NULL) {
        dr_fprintf(STDERR, "\n%d------\n", i);    
	print_list(table[i]);
             
        }
      /*  if (table[i] == NULL) {
            dr_fprintf(STDERR, "NULL\n");
        }*/
    }
  //  if (find_elem(table[5760], (app_pc)0x4000e091)== NULL) dr_fprintf(STDERR, "oop\n");
  //  if (lookup(table, (app_pc)0x400177ae, (app_pc)0x4000e091) != NULL) dr_fprintf(STDERR, "got\n");
   // delete_table(table);
}

void dr_exit(void)
{
}


/*DR_EXPORT
void dr_init(client_id_t id)
{
    table = new_table();
    dr_register_bb_event(bb_event);
    dr_register_exit_event(dr_exit);
}*/
