#include <dr_api.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
//#include <cbr.h>
#include <myDS.h>

static file_t cfg_log, cfg_en;
//static list_t *mylist;
static hash_table_t table=NULL;

enum { profile = 0, enforce, stack_check };
static int mode;

static void profile_init();
static void enforce_init();
static void event_exit(void);
static void event_fork(void *drcontext);
static dr_emit_flags_t event_basic_block(void *drcontext, void *tag,
					 instrlist_t *bb, bool for_trace,
					 bool translating);

DR_EXPORT void 
dr_init(client_id_t id)
{
    dr_fprintf(STDERR, "bof: %s.%d\n", dr_get_application_name(), dr_get_process_id());

    const char *opt = dr_get_options(id);
    if (!strcmp(opt, "-p"))
        profile_init();
    else if (!strcmp(opt, "-e"))
        enforce_init();
    else
        DR_ASSERT_MSG(0, "unknown option");

    /* register events */
    dr_register_bb_event(event_basic_block);
}

static void
profile_init(void)
{
    
    mode = profile;
    event_fork(NULL);
    dr_register_exit_event(event_exit);
    dr_register_fork_init_event(event_fork);
    
    /*
    char filename[FILENAME_MAX];
    snprintf(filename, sizeof(filename), "%s.log",
             dr_get_application_name());
    cfg_log = dr_open_file(filename, DR_FILE_WRITE_APPEND | DR_FILE_ALLOW_LARGE);
    
    dr_fprintf(STDERR, "bof: Log %s\n", filename);*/
  
}

static void
enforce_init(void)
{
    char filename[FILENAME_MAX];
    char filename1[FILENAME_MAX];
    dr_register_exit_event(event_exit);
    
    
    mode = enforce;
    snprintf(filename, sizeof(filename), "%s.log",
             dr_get_application_name());
    
    //snprintf(filename1, sizeof(filename1), "enforce.log");
    //cfg_en = dr_open_file(filename1, DR_FILE_WRITE_APPEND | DR_FILE_ALLOW_LARGE);
   // DR_ASSERT_MSG(f != INVALID_FILE, "Cannot open cfg file");

	//mylist = new_list();
	table = new_table();
	char buf[18];
	FILE *fp;
	int source, dest;
	fp = fopen(filename, "r");
	while (fgets(buf, 18, fp))
	{
		buf[strcspn(buf,"\n")] = '\0';
   	if ((buf[0] != '\0' ) && (sscanf(buf, "%x %x", &source, &dest) ==2))
	
		{//load_to_datastructure(mylist, (app_pc)source, (app_pc)dest);
			insert(table, (app_pc)source, (app_pc)dest, CBR_TAKEN);
		}
	}
    //dr_close_file(f);
    //dr_fprintf(STDERR, "lines: %d\n", line");
    
	//print_hashtable(table);
      
	fclose(fp);
   // dr_close_file(cfg_en);
}

static void
event_exit(void)
{
    if (cfg_log)
        dr_close_file(cfg_log);
   /* if (cfg_en)
	dr_close_file(cfg_en);*/
}

static void
event_fork(void *drcontext)
{
    /* open log file */
    char filename[FILENAME_MAX];
    
   /* snprintf(filename, sizeof(filename), "%s.%d.log",
             dr_get_application_name(), dr_get_process_id());*/
   snprintf(filename, sizeof(filename), "%s.log",
             dr_get_application_name());
   
    if (cfg_log)
        dr_close_file(cfg_log);
    
   /* cfg_log = dr_open_file(filename, DR_FILE_WRITE_OVERWRITE |DR_FILE_ALLOW_LARGE);*/
    cfg_log = dr_open_file(filename, DR_FILE_WRITE_APPEND | DR_FILE_ALLOW_LARGE);
    
    dr_fprintf(STDERR, "bof: Log %s\n", filename);
}

static void
print_address(file_t f, app_pc addr, const char *prefix)
{
    Dl_info info;
    if (dladdr(addr, &info) && info.dli_sname)
        dr_fprintf(f, "%s%s+0x%lx\n", prefix, info.dli_sname, addr - (app_pc)info.dli_saddr);
    else
        dr_fprintf(f, "%s0x%lx\n", prefix, addr);
}

static void
profile_at_mbr(app_pc instr_addr, app_pc target_addr)
{
  /*  dr_fprintf(STDERR, "profile_at_mbr() called: 0x%lx -> 0x%lx\n",
	       instr_addr, target_addr);*/
    
    dr_fprintf(cfg_log, "%lx %lx\n",
	       instr_addr, target_addr);
}

static void
enforce_at_mbr(app_pc instr_addr, app_pc target_addr)
{
    
    /*dr_fprintf(cfg_en, "%lx %lx\n",
	       instr_addr, target_addr);*/
    int cfg_match = 1;
   
/* elem_t *elem= NULL, *elem1= NULL;
    elem = find_elem(mylist, instr_addr);
    if (elem == NULL) cfg_match= 0;
    else {
    	elem1= find_elem(elem->neighbors, target_addr);
        if (elem1 == NULL) cfg_match = 0;
    }*/
    if (lookup(table, instr_addr, target_addr) == NULL) {	
		cfg_match=0; 
	}
    if (!cfg_match) {
	dr_fprintf(STDERR, "bof: Invalid branch in %s: %p -> %p\n",
		   dr_get_application_name(), instr_addr, target_addr);
	print_address(STDERR, instr_addr,  "Branch from: ");
	print_address(STDERR, target_addr, "Branch to:   ");
    }
}


static dr_emit_flags_t
event_basic_block(void *drcontext, void *tag, instrlist_t *bb,
                  bool for_trace, bool translating)
{
    instr_t *instr, *next_instr;
    int i;

    for (instr = instrlist_first(bb); instr != NULL; instr = next_instr) {
        next_instr = instr_get_next(instr);
        if (!instr_opcode_valid(instr))
            continue;
        if (mode == profile || mode == enforce) {
	    /* instrument indirect branches */
	    if (instr_is_mbr(instr)) {
		if (mode == profile) {
		    dr_insert_mbr_instrumentation(drcontext, bb, instr,
						  (void *) profile_at_mbr,
						  SPILL_SLOT_1);
		} else {
		    dr_insert_mbr_instrumentation(drcontext, bb, instr,
						  (void *) enforce_at_mbr,
						  SPILL_SLOT_1);
		}
	    }
	}
    }
    return DR_EMIT_DEFAULT;
}
