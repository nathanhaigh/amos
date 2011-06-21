/* **********************************************************
 * Copyright (c) 2002-2009 VMware, Inc.  All rights reserved.
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

#ifndef _DR_IR_INSTRLIST_H_
#define _DR_IR_INSTRLIST_H_ 1

/****************************************************************************
 * INSTRLIST ROUTINES
 */
/**
 * @file dr_ir_instrlist.h
 * @brief Functions to create and manipulate lists of instructions.
 */


/** Returns an initialized instrlist_t allocated on the thread-local heap. */
instrlist_t*
instrlist_create(void *drcontext);

/** Initializes \p ilist. */
void 
instrlist_init(instrlist_t *ilist);

/** Deallocates the thread-local heap storage for \p ilist. */
void 
instrlist_destroy(void *drcontext, instrlist_t *ilist);

/** Frees the instructions in \p ilist. */
void
instrlist_clear(void *drcontext, instrlist_t *ilist);

/** Destroys the instructions in \p ilist and destroys the instrlist_t object itself. */
void
instrlist_clear_and_destroy(void *drcontext, instrlist_t *ilist);

/** 
 * All future instructions inserted into \p ilist that do not have raw bits
 * will have instr_set_translation() called with \p pc as the target.
 * This is a convenience routine to make it easy to have the same
 * code generate non-translation and translation instructions, and it does
 * not try to enforce that all instructions have translations (e.g.,
 * some could be inserted via instr_set_next()).
 */
void
instrlist_set_translation_target(instrlist_t *ilist, app_pc pc);

/** Returns the translation target, or NULL if none is set. */
app_pc
instrlist_get_translation_target(instrlist_t *ilist);

/** Returns the first instr_t in \p ilist. */
instr_t*
instrlist_first(instrlist_t *ilist);

/** Returns the last instr_t in \p ilist. */
instr_t*
instrlist_last(instrlist_t *ilist);

/** Adds \p instr to the end of \p ilist. */
void 
instrlist_append(instrlist_t *ilist, instr_t *instr);

/** Adds \p instr to the front of \p ilist. */
void 
instrlist_prepend(instrlist_t *ilist, instr_t *instr);

/** 
 * Allocates a new instrlist_t and for each instr_t in \p old allocates
 * a new instr_t using instr_clone to produce a complete copy of \p old.
 * Each operand that is opnd_is_instr() has its target updated
 * to point to the corresponding instr_t in the new instrlist_t
 * (this routine assumes that all such targets are contained within \p old,
 * and may fault otherwise).
 */
instrlist_t*
instrlist_clone(void *drcontext, instrlist_t *old);

/** Inserts \p instr into \p ilist prior to \p where. */
void   
instrlist_preinsert(instrlist_t *ilist, instr_t *where, instr_t *instr);

/** Inserts \p instr into \p ilist after \p where. */
void   
instrlist_postinsert(instrlist_t *ilist, instr_t *where, instr_t *instr);

/** Replaces \p oldinst with \p newinst in \p ilist (does not destroy \p oldinst). */
instr_t*
instrlist_replace(instrlist_t *ilist, instr_t *oldinst, instr_t *newinst);

/** Removes (does not destroy) \p instr from \p ilist. */
void   
instrlist_remove(instrlist_t *ilist, instr_t *instr);


/**
 * Prints each instruction in \p ilist in sequence to \p outfile.
 * The default is to use AT&T-style syntax, unless the \ref op_syntax_intel
 * "-syntax_intel" runtime option is specified.
 */
void 
instrlist_disassemble(void *drcontext, app_pc tag,
                      instrlist_t *ilist, file_t outfile);

/** 
 * Encodes each instruction in \p ilist in turn in contiguous memory starting
 * at \p pc.  Returns the pc after all of the encodings, or NULL if any one
 * of the encodings failed.
 * Uses the x86/x64 mode stored in each instr, not the mode of the current thread.
 * In order for instr_t operands to be encoded properly,
 * \p has_instr_jmp_targets must be true.  If \p has_instr_jmp_targets is true,
 * the note field of each instr_t in ilist will be overwritten, and if any
 * instr_t targets are not in \p ilist, they must have their note fields set with
 * their offsets relative to pc.
 */
byte *
instrlist_encode(void *drcontext, instrlist_t *ilist, byte *pc,
                 bool has_instr_jmp_targets);


#endif /* _DR_IR_INSTRLIST_H_ */
