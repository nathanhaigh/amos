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

#ifndef _DR_EVENTS_H_
#define _DR_EVENTS_H_ 1


/**************************************************
 * ROUTINES TO REGISTER EVENT CALLBACKS
 */
/**
 * @file dr_events.h
 * @brief Event callback registration routines.
 */


/**
 * Registers a callback function for the process exit event.  DR calls
 * \p func when the process exits.  Note that in release build it is
 * possible for other threads to still be executing, so
 * synchronization should be used for global variables; alternatively,
 * the -synch_at_exit option can be set, or the
 * dr_request_synchronized_exit() routine invoked, to guarantee that
 * only one thread is active at exit time (at a potential performance
 * cost).
 *
 * On Linux, SYS_execve does NOT result in an exit event, but it WILL
 * result in the client library being reloaded and its dr_init()
 * routine being called.
 */
void
dr_register_exit_event(void (*func)(void));

/**
 * Unregister a callback function for the process exit event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_exit_event(void (*func)(void));
/**
 * Flags controlling the behavior of basic blocks and traces when emitted
 * into the code cache.  For multiple clients, the flags returned by each
 * client are or-ed together.
 */
typedef enum {
    /** Emit as normal. */
    DR_EMIT_DEFAULT              =    0,
    /** 
     * Store translation information at emit time rather than calling
     * the basic block or trace event later to recreate the
     * information.  Note that even if a standalone basic block has
     * stored translations, if when it is added to a trace it does not
     * request storage (and the trace callback also does not request
     * storage) then the basic block callback may still be called to
     * translate for the trace.
     *
     * \sa #dr_register_bb_event()
     */
    DR_EMIT_STORE_TRANSLATIONS   = 0x01,
} dr_emit_flags_t;


/**
 * Registers a callback function for the basic block event.  DR calls
 * \p func before inserting a new basic block into the code cache.
 * When adding a basic block to a new trace, DR calls \p func again
 * with \p for_trace set to true, giving the client the opportunity to
 * keep its same instrumentation in the trace, or to change it.  The
 * original basic block's instrumentation is unchanged by whatever
 * action is taken in the \p for_trace call.
 *
 * DR may call \p func again if it needs to translate from code cache
 * addresses back to application addresses, which happens on faulting
 * instructions as well as in certain situations involving suspended
 * threads or forcibly relocated threads.  The \p translating
 * parameter distinguishes the two types of calls and is further
 * explained below.
 *
 * - \p drcontext is a pointer to the input program's machine context.
 * Clients should not inspect or modify the context; it is provided as
 * an opaque pointer (i.e., <tt>void *</tt>) to be passed to API
 * routines that require access to this internal data.
 * drcontext is specific to the current thread, but in normal
 * configurations the basic block being created is thread-shared: thus,
 * when allocating data structures with the same lifetime as the
 * basic block, usually global heap (#dr_global_alloc()) is a better
 * choice than heap tied to the thread that happened to first create
 * the basic block (#dr_thread_alloc()).  Thread-private heap is fine
 * for temporary structures such as instr_t and instrlist_t.
 *
 * - \p tag is a unique identifier for the basic block fragment.
 * Use dr_fragment_app_pc() to translate it to an application address.
 * - \p bb is a pointer to the list of instructions that comprise the
 * basic block.  Clients can examine, manipulate, or completely
 * replace the instructions in the list.
 *
 * - \p translating indicates whether this callback is for basic block
 * creation (false) or is for address translation (true).  This is
 * further explained below.
 *
 * \return a #dr_emit_flags_t flag.
 *
 * The user is free to inspect and modify the block before it
 * executes, but must adhere to the following restrictions:
 * - If there is more than one non-meta branch, only the last can be
 * conditional.
 * - A non-meta conditional branch or direct call must be the final
 * instruction in the block.
 * - There can only be one indirect branch (call, jump, or return) in
 * a basic block, and it must be the final instruction in the
 * block.
 * - The exit control-flow of a block ending in a system call or
 * int instruction cannot be changed, nor can instructions be inserted
 * after the system call or int instruction itself, unless
 * the system call or int instruction is removed entirely.
 * - The number of an interrupt cannot be changed.  (Note that the
 * parameter to a system call, normally kept in the eax register, can
 * be freely changed in a basic block: but not in a trace.)
 * - A system call or interrupt instruction can only be added
 * if it satisfies the above constraints: i.e., if it is the final
 * instruction in the block and the only system call or interrupt.
 * - The block's application source code (as indicated by the
 * translation targets, set by #instr_set_translation()) must remain
 * within the original bounds of the block (the one exception to this
 * is that a jump can translate to its target).  Otherwise, DR's cache
 * consistency algorithms cannot guarantee to properly invalidate the
 * block if the source application code is modified.  To send control
 * to other application code regions, truncate the block and use a
 * direct jump to target the desired address, which will then
 * materialize in the subsequent block, rather than embedding the
 * desired instructions in this block.
 * - There is a limit on the size of a basic block in the code cache.
 * DR performs its own modifications, especially on memory writes for
 * cache consistency of self-modifying (or false sharing) code
 * regions.  If an assert fires in debug build indicating a limit was
 * reached, either truncate blocks or use the -max_bb_instrs runtime
 * option to ask DR to make them smaller.
 *
 * To support transparent fault handling, DR must translate a fault in the
 * code cache into a fault at the corresponding application address.  DR
 * must also be able to translate when a suspended thread is examined by
 * the application or by DR itself for internal synchronization purposes.
 * If the client is only adding observational instrumentation (i.e., meta
 * instructions: see #instr_set_ok_to_mangle()) (which should not fault) and
 * is not modifying, reordering, or removing application instructions,
 * these details can be ignored.  In that case the client should return
 * #DR_EMIT_DEFAULT and set up its basic block callback to be deterministic
 * and idempotent.  If the client is performing modifications, then in
 * order for DR to properly translate a code cache address the client must
 * use #instr_set_translation() in the basic block creation callback to set
 * the corresponding application address (the address that should be
 * presented to the application as the faulting address, or the address
 * that should be restarted after a suspend) for each modified instruction
 * and each added non-meta instruction (see #instr_set_ok_to_mangle()).
 *
 * There are two methods for using the translated addresses:
 *
 * -# Return #DR_EMIT_STORE_TRANSLATIONS from the basic block creation
 *    callback.  DR will then store the translation addresses and use
 *    the stored information on a fault.  The basic block callback for
 *    \p tag will not be called with \p translating set to true.  Note
 *    that unless #DR_EMIT_STORE_TRANSLATIONS is also returned for \p
 *    for_trace calls (or #DR_EMIT_STORE_TRANSLATIONS is returned in
 *    the trace callback), each constituent block comprising the trace
 *    will need to be re-created with both \p for_trace and \p
 *    translating set to true.  Storing translations uses additional
 *    memory that can be significant: up to 20% in some cases, as it
 *    prevents DR from using its simple data structures and forces it
 *    to fall back to its complex, corner-case design.  This is why DR
 *    does not store all translations by default.
 * -# Return #DR_EMIT_DEFAULT from the basic block creation callback.
 *    DR will then call the callback again during fault translation
 *    with \p translating set to true.  All modifications to \p bb
 *    that were performed on the creation callback must be repeated on
 *    the translating callback.  This option is only possible when
 *    basic block modifications are deterministic and idempotent, but
 *    it saves memory.  Naturally, global state changes triggered by
 *    block creation should be wrapped in checks for \p translating
 *    being false.  Even in this case, #instr_set_translation() should
 *    be called for non-meta instructions even when \p translating is
 *    false, as DR may decide to store the translations at creation
 *    time for reasons of its own.
 *
 * Furthermore, if the client's modifications change any part of the
 * machine state besides the program counter, the client should use
 * #dr_register_restore_state_event() or
 * #dr_register_restore_state_ex_event() to restore the registers and
 * application memory to their original application values.
 *
 * For meta instructions that do not reference application memory
 * (i.e., they should not fault), leave the translation field as NULL.
 * A NULL value instructs DR to use the subsequent non-meta
 * instruction's translation as the application address, and to fail
 * when translating the full state.  Since the full state will only be
 * needed when relocating a thread (as stated, there will not be a
 * fault here), failure indicates that this is not a valid relocation
 * point, and DR's thread synchronization scheme will use another
 * spot.  If the translation field is set to a non-NULL value, the
 * client should be willing to also restore the rest of the machine
 * state at that point (restore spilled registers, etc.) via
 * #dr_register_restore_state_event() or
 * #dr_register_restore_state_ex_event().  This is necessary for meta
 * instructions that reference application memory.  DR takes care of
 * such potentially-faulting instructions added by its own API
 * routines (#dr_insert_clean_call() arguments that reference
 * application data, #dr_insert_mbr_instrumentation()'s read of
 * application indirect branch data, etc.)
 *
 * \note In order to present a more straightforward code stream to clients,
 * this release of DR disables several internal optimizations.  As a result,
 * some applications may see a performance degradation.  Applications making
 * heavy use of system calls are the most likely to be affected.
 * Future releases may allow clients some control over performance versus
 * visibility.
 *
 * \note If multiple clients are present, the instruction list for a
 * basic block passed to earlier-registered clients will contain the
 * instrumentation and modifications put in place by later-registered
 * clients.
 * 
 * \note Basic blocks can be deleted due to hitting capacity limits or
 * cache consistency events (when the source application code of a
 * basic block is modified).  In that case, the client will see a new
 * basic block callback if the block is then executed again after
 * deletion.  The deletion event (#dr_register_delete_event()) will be
 * raised at deletion time.
 *
 * \note If the -thread_private runtime option is specified, clients
 * should expect to see duplicate tags for separate threads, albeit
 * with different dcrcontext values.  Additionally, DR employs a
 * cache-sizing algorithm for thread private operation that
 * proactively deletes fragments.  Even with thread-shared caches
 * enabled, however, certain situations cause DR to emit
 * thread-private basic blocks (e.g., self-modifying code).  In this
 * case, clients should be prepared to see duplicate tags without an
 * intermediate deletion.
 */
void
dr_register_bb_event(dr_emit_flags_t (*func)
                     (void *drcontext, void *tag, instrlist_t *bb,
                      bool for_trace, bool translating));

/**
 * Unregister a callback function for the basic block event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 *
 * \note We do not recommend unregistering for the basic block event
 * unless it aways returned #DR_EMIT_STORE_TRANSLATIONS (including
 * when \p for_trace is true, or if the client has a trace creation
 * callback that returns #DR_EMIT_STORE_TRANSLATIONS).  Unregistering
 * can prevent proper state translation on a later fault or other
 * translation event for this basic block or for a trace that includes
 * this basic block.  Instead of unregistering, turn the event
 * callback into a nop.
 */
bool
dr_unregister_bb_event(dr_emit_flags_t (*func)
                       (void *drcontext, void *tag, instrlist_t *bb,
                        bool for_trace, bool translating));

/**
 * Registers a callback function for the trace event.  DR calls \p func
 * before inserting a new trace into the code cache.  DR may call \p func
 * again if it needs to translate from code cache addresses back to
 * application addresses, which happens on faulting instructions as well as
 * in certain situations involving suspended threads or forcibly relocated
 * threads.  The \p translating parameter distinguishes the two types of
 * calls and behaves identically to the same parameter in the basic
 * block callback: see #dr_register_bb_event() for further details.
 *
 * Traces are not built if the -disable_traces runtime option
 * is specified.
 *
 * - \p drcontext is a pointer to the input program's machine context.
 * Clients should not inspect or modify the context; it is provided as
 * an opaque pointer (i.e., <tt>void *</tt>) to be passed to API
 * routines that require access to this internal data.
 * - \p tag is a unique identifier for the trace fragment.
 * - \p trace is a pointer to the list of instructions that comprise the
 * trace.
 * - \p translating indicates whether this callback is for trace creation
 * (false) or is for fault address recreation (true).  This is further
 * explained below.
 *
 * \return a #dr_emit_flags_t flag.
 *
 * The user is free to inspect and modify the trace before it
 * executes, with certain restrictions on introducing control-flow
 * that include those for basic blocks (see dr_register_bb_event()).
 * Additional restrictions unique to traces also apply:
 * - Only one non-meta direct branch that targets the subsequent block
 *   in the trace can be present in each block.
 * - Each block must end with a non-meta control transfer.
 * - The parameter to a system call, normally kept in the eax register, 
 *   cannot be changed.
 * - A system call or interrupt instruction cannot be added.
 *
 * If hitting a size limit due to extensive instrumentation, reduce
 * the -max_trace_bbs option to start with a smaller trace.
 *
 * The basic block restrictions on modifying application source code
 * apply to traces as well.  If the user wishes to change which basic
 * blocks comprise the trace, either the
 * #dr_register_end_trace_event() should be used or the \p for_trace
 * basic block callbacks should modify their continuation addresses
 * via direct jumps.
 *
 * All of the comments for #dr_register_bb_event() regarding
 * transparent fault handling and state translation apply to the trace
 * callback as well.  Please read those comments carefully.
 *
 * \note As each basic block is added to a new trace, the basic block
 * callback (see #dr_register_bb_event()) is called with its \p
 * for_trace parameter set to true.  In order to preserve basic block
 * instrumentation inside of traces, a client need only act
 * identically with respect to the \p for_trace parameter; it can
 * ignore the trace event if its goal is to place instrumentation
 * on all code.
 *
 * \note Certain control flow modifications applied to a basic block
 * can prevent it from becoming part of a trace: e.g., adding
 * additional non-meta control transfers.
 * 
 * \note If multiple clients are present, the instruction list for a
 * trace passed to earlier-registered clients will contain the
 * instrumentation and modifications put in place by later-registered
 * clients; similarly for each constituent basic block.
 *
 * \note Traces can be deleted due to hitting capacity limits or cache
 * consistency events (when the source application code of a trace is
 * modified).  In that case, the client will see a new trace callback
 * if a new trace containing that code is created again after
 * deletion.  The deletion event (#dr_register_delete_event()) will be
 * raised at deletion time.
 */
void
dr_register_trace_event(dr_emit_flags_t (*func)
                        (void *drcontext, void *tag, instrlist_t *trace,
                         bool translating));

/**
 * Unregister a callback function for the trace event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 *
 * \note We do not recommend unregistering for the trace event unless it
 * always returned #DR_EMIT_STORE_TRANSLATIONS, as doing so can prevent
 * proper state translation on a later fault or other translation event.
 * Instead of unregistering, turn the event callback into a nop.
 */
bool
dr_unregister_trace_event(dr_emit_flags_t (*func)
                          (void *drcontext, void *tag, instrlist_t *trace,
                           bool translating));

/**
 * DR will call the end trace event if it is registered prior to
 * adding each basic block to a trace being generated.  The return
 * value of the event callback should be from the
 * dr_custom_trace_action_t enum.
 *
 * \note DR treats CUSTOM_TRACE_CONTINUE as an advisement only.  Certain
 * fragments are not suitable to be included in a trace and if DR runs
 * into one it will end the trace regardless of what the client returns
 * through the event callback.
 */
typedef enum {
    CUSTOM_TRACE_DR_DECIDES,
    CUSTOM_TRACE_END_NOW,
    CUSTOM_TRACE_CONTINUE
} dr_custom_trace_action_t;



/**
 * Registers a callback function for the end-trace event.  DR calls \p
 * func before extending a trace with a new basic block.  The \p func
 * should return one of the #dr_custom_trace_action_t enum values.
 */
void
dr_register_end_trace_event(dr_custom_trace_action_t (*func)
                            (void *drcontext, void *tag, void *next_tag));

/**
 * Unregister a callback function for the end-trace event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_end_trace_event(dr_custom_trace_action_t (*func)
                              (void *drcontext, void *tag, void *next_tag));

/**
 * Registers a callback function for the fragment deletion event.  DR
 * calls \p func whenever it removes a fragment from the code cache.
 * Due to DR's high-performance non-precise flushing, a fragment
 * can be made inaccessible but not actually freed for some time.
 * A new fragment can thus be created before the deletion event
 * for the old fragment is raised.  We recommended using a counter
 * to ignore subsequent deletion events when using per-fragment
 * data structures and duplicate fragments are seen.
 *
 * \note drcontext may be NULL when thread-shared fragments are being
 * deleted during process exit.  For this reason, thread-private
 * heap should not be used for data structures intended to be freed
 * at thread-shared fragment deletion.
 */
void
dr_register_delete_event(void (*func)(void *drcontext, void *tag));

/**
 * Unregister a callback function for the fragment deletion event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_delete_event(void (*func)(void *drcontext, void *tag));

/**
 * Registers a callback function for the machine state restoration event.
 * DR calls \p func whenever it needs to translate a code cache machine
 * context from the code cache to its corresponding original application
 * context.  DR needs to translate when instructions fault in the cache as
 * well as when a suspended thread is examined or relocated for internal
 * purposes.
 *
 * If a client is only adding instrumentation (meta-code: see
 * #instr_ok_to_mangle()) that does not reference application memory,
 * and is not reordering or removing application instructions, then it
 * need not register for this event.  If, however, a client is
 * modifying application code or is adding code that can fault, the
 * client must be capable of restoring the original context.
 *
 * When DR needs to translate a code cache context, DR recreates the
 * faulting instruction's containing fragment, storing translation
 * information along the way, by calling the basic block and/or trace event
 * callbacks with the \p translating parameter set to true.  DR uses the
 * recreated code to identify the application instruction (\p mcontext.pc)
 * corresponding to the faulting code cache instruction.  If the client
 * asked to store translation information by returning
 * #DR_EMIT_STORE_TRANSLATIONS from the basic block or trace event
 * callback, then this step of re-calling the event callback is skipped and
 * the stored value is used as the application address (\p mcontext.pc).
 *
 * DR then calls the fault state restoration event to allow the client
 * to restore the registers and application memory to their proper
 * values as they would have appeared if the original application code
 * had been executed up to the \p mcontext.pc instruction.  Memory
 * should only be restored if the \p restore_memory parameter is true;
 * if it is false, DR may only be querying for the address (\p
 * mcontext.pc) or register state and may not relocate this thread.
 *
 * The \p app_code_consistent parameter indicates whether the original
 * application code containing the instruction being translated is
 * guaranteed to still be in the same state it was when the code was
 * placed in the code cache.  This guarantee varies depending on the
 * type of cache consistency being used by DR.
 *
 * The client can update \p mcontext.pc in this callback.
 *
 * \note The passed-in \p drcontext may correspond to a different thread
 * than the thread executing the callback.  Do not assume that the
 * executing thread is the target thread.
 */
void
dr_register_restore_state_event(void (*func)
                                (void *drcontext, void *tag, dr_mcontext_t *mcontext,
                                 bool restore_memory, bool app_code_consistent));

/**
 * Unregister a callback function for the machine state restoration event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_restore_state_event(void (*func)
                                  (void *drcontext, void *tag, dr_mcontext_t *mcontext,
                                   bool restore_memory, bool app_code_consistent));
/**
 * Data structure passed within dr_exception_t, dr_siginfo_t, and
 * dr_restore_state_info_t.
 * Contains information about the code fragment inside the code cache 
 * at the exception/signal/translation interruption point. 
 */
typedef struct _dr_fault_fragment_info_t {
    /**
     * The tag of the code fragment inside the code cache at the
     * exception/signal/translation interruption point. NULL for
     * interruption not in the code cache.
     */
    void *tag;
    /**
     * The start address of the code fragment inside the code cache at
     * the exception/signal/translation interruption point. NULL for interruption
     * not in the code cache.  Clients are cautioned when examining
     * code cache instructions to not rely on any details of code
     * inserted other than their own.  
     */
    byte *cache_start_pc;
    /** Indicates whether the interrupted code fragment is a trace */
    bool is_trace;
    /**
     * Indicates whether the original application code containing the
     * code corresponding to the exception/signal/translation interruption point
     * is guaranteed to still be in the same state it was when the
     * code was placed in the code cache. This guarantee varies
     * depending on the type of cache consistency being used by DR.
     */
    bool app_code_consistent;    
} dr_fault_fragment_info_t;

/**
 * Data structure passed to a restore_state_ex event handler (see
 * dr_register_restore_state_ex_event()).  Contains the machine
 * context at the translation point and other translation
 * information.
 */
typedef struct _dr_restore_state_info_t {
    /** The application machine state at the translation point. */
    dr_mcontext_t *mcontext;
    /** Whether raw_mcontext is valid. */
    bool raw_mcontext_valid;
    /** 
     * The raw pre-translated machine state at the translation
     * interruption point inside the code cache.  Clients are
     * cautioned when examining code cache instructions to not rely on
     * any details of code inserted other than their own.
     */
    dr_mcontext_t raw_mcontext;
    /**
     * Information about the code fragment inside the code cache
     * at the translation interruption point.
     */
    dr_fault_fragment_info_t fragment_info;
} dr_restore_state_info_t;


/**
 * Registers a callback function for the machine state restoration
 * event with extended information.
 *
 * This event is identical to that for dr_register_restore_state_event()
 * with the following exceptions:
 *
 * - Additional information is provided in the
 *   dr_restore_state_info_t structure, including the pre-translation
 *   context (containing the address inside the code cache of the
 *   translation point) and the starting address of the containing
 *   fragment in the code cache.  Certain registers may not contain
 *   proper application values in \p info->raw_mcontext.  Clients are
 *   cautioned against relying on any details of code cache layout or
 *   register usage beyond instrumentation inserted by the client
 *   itself when examining \p info->raw_mcontext.
 *
 * - The callback function returns a boolean indicating the success of
 *   the translation.  When DR is translating not for a fault but for
 *   thread relocation, the \p restore_memory parameter will be false.
 *   Such translation can target a meta-instruction that can fault
 *   (see instr_set_meta_may_fault()).  For that scenario, a client
 *   can choose not to translate.  Such instructions do not always
 *   require full translation for faults, and allowing translation
 *   failure removes the requirement that a client must translate at
 *   all such instructions.  Note, however, that returning false can
 *   cause performance degradation as DR must then resume the thread
 *   and attempt to re-suspend it at a safer spot.  Clients must
 *   return true for translation points in application code in order
 *   to avoid catastropic failure to suspend, and should thus identify
 *   whether translation points are inside their own instrumentation
 *   before returning false.  Translation for relocation will never
 *   occur in meta instructions, so clients only need to look for
 *   meta-may-fault instructions.  Clients should never return false
 *   when \p restore_memory is true.
 */
void
dr_register_restore_state_ex_event(bool (*func) (void *drcontext, bool restore_memory,
                                                 dr_restore_state_info_t *info));

/**
 * Unregister a callback function for the machine state restoration
 * event with extended ifnormation.  \return true if unregistration is
 * successful and false if it is not (e.g., \p func was not
 * registered).
 */
bool
dr_unregister_restore_state_ex_event(bool (*func) (void *drcontext, bool restore_memory,
                                                   dr_restore_state_info_t *info));

/**
 * Registers a callback function for the thread initialization event.
 * DR calls \p func whenever the application creates a new thread.
 */
void
dr_register_thread_init_event(void (*func)(void *drcontext));

/**
 * Unregister a callback function for the thread initialization event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_thread_init_event(void (*func)(void *drcontext));

/**
 * Registers a callback function for the thread exit event.  DR calls
 * \p func whenever an application thread exits.
 *
 * There are some potential races at process exit time with the thread
 * exit event for all remaining threads.  In debug builds, \DynamoRIO
 * synchronizes with all remaining threads at process exit time,
 * guaranteeing that no other threads are executing when the thread
 * exit events are raised.  In release build, however, for performance
 * reasons, while \DynamoRIO attempts to prevent other threads from
 * executing (and thus running instrumentation code or raising events)
 * beyond when their own thread exit events are raised, no guarantee
 * is provided.  The \p -synch_at_exit option can be turned on in
 * order to provide such a guarantee, at a potential performance hit.
 */
void
dr_register_thread_exit_event(void (*func)(void *drcontext));

/**
 * Unregister a callback function for the thread exit event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_thread_exit_event(void (*func)(void *drcontext));

/**
 * Registers a callback function for the fork event.  DR calls \p func
 * whenever the application forks a new process.
 * \note Valid on Linux only.
 */
void
dr_register_fork_init_event(void (*func)(void *drcontext));

/**
 * Unregister a callback function for the fork event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_fork_init_event(void (*func)(void *drcontext));

/**
 * Registers a callback function for the module load event.  DR calls
 * \p func whenever the application loads a module.  The \p loaded
 * parameter indicates whether the module is about to be loaded (the
 * normal case) or is already loaded (if the module was already there
 * at the time DR initialized). \note The client should be aware that
 * if the module is being loaded it may not be fully processed by the
 * loader (relocating, rebinding and on Linux segment remapping may
 * have not yet occurred). \note The module_data_t \p *info passed
 * to the callback routine is valid only for the duration of the
 * callback and should not be freed; a persistent copy can be made with
 * dr_copy_module_data().
 */
void
dr_register_module_load_event(void (*func)(void *drcontext, const module_data_t *info,
                                           bool loaded));

/**
 * Unregister a callback for the module load event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_module_load_event(void (*func)(void *drcontext, const module_data_t *info,
                                             bool loaded));

/**
 * Registers a callback function for the module unload event.  DR
 * calls \p func whenever the application unloads a module.
 * \note The module_data_t \p *info passed to
 * the callback routine is valid only for the duration of the callback
 * and should not be freed; a persistent copy can be made with
 * dr_copy_module_data().
 */
void
dr_register_module_unload_event(void (*func)(void *drcontext,
                                             const module_data_t *info));

/**
 * Unregister a callback function for the module unload event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_module_unload_event(void (*func)(void *drcontext,
                                               const module_data_t *info));
#ifdef WINDOWS

/**
 * Data structure passed with an exception event.  Contains the
 * machine context and the Win32 exception record.
 */
typedef struct _dr_exception_t {
    dr_mcontext_t mcontext;   /**< Machine context at exception point. */
    EXCEPTION_RECORD *record; /**< Win32 exception record. */
    /** 
     * The raw pre-translated machine state at the exception interruption
     * point inside the code cache.  Clients are cautioned when examining
     * code cache instructions to not rely on any details of code inserted
     * other than their own.
     */
    ALIGN_VAR(8)/*avoid differences in padding*/
    dr_mcontext_t raw_mcontext;
    /**
     * Information about the code fragment inside the code cache at
     * the exception interruption point. 
     */
    dr_fault_fragment_info_t fault_fragment_info;
} dr_exception_t;


/**
 * Registers a callback function for the exception event.  DR calls \p func
 * whenever the application throws an exception.  If \p func returns true,
 * the exception is delivered to the application's handler along with any
 * changes made to \p excpt->mcontext.  If \p func returns false, the
 * faulting instruction in the code cache is re-executed using \p
 * excpt->raw_mcontext, including any changes made to that structure.
 * Clients are expected to use \p excpt->raw_mcontext when using faults as
 * a mechanism to push rare cases out of an instrumentation fastpath that
 * need to examine instrumentation instructions rather than the translated
 * application state and should normally not examine it for application
 * instruction faults.  Certain registers may not contain proper
 * application values in \p excpt->raw_mcontext for exceptions in
 * application instructions.  Clients are cautioned against relying on any
 * details of code cache layout or register usage beyond instrumentation
 * inserted by the client itself when examining \p excpt->raw_mcontext.
 *
 * DR raises this event for exceptions outside the code cache that
 * could come from code generated by a client.  For such exceptions,
 * mcontext is not translated and is identical to raw_mcontext.
 *
 * To skip the passing of the exception to the application's exception
 * handlers and to send control elsewhere instead, a client can call
 * dr_redirect_execution() from \p func.
 *
 * \note \p excpt->fault_fragment_info data is provided with 
 * \p excpt->raw_mcontext. It is valid only if  
 * \p excpt->fault_fragment_info.cache_start_pc is not \p NULL. 
 * It provides clients information about the code fragment being
 * executed at the exception interruption point. Clients are cautioned
 * against relying on any details of code cache layout or register
 * usage beyond  instrumentation inserted by the client itself.  
 * \note Only valid on Windows.
 * \note The function is not called for RaiseException.
 */
void
dr_register_exception_event(bool (*func)(void *drcontext, dr_exception_t *excpt));

/**
 * Unregister a callback function for the exception event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_exception_event(bool (*func)(void *drcontext, dr_exception_t *excpt));
#endif /* WINDOWS */


/**
 * Registers a callback function for the syscall filter event.  DR
 * calls \p func to decide whether to invoke the syscall events for
 * each system call site encountered with a statically-determinable
 * system call number.  If \p func returns true, the pre-syscall
 * (dr_register_pre_syscall_event()) and post-syscall
 * (dr_register_post_syscall_event()) events will be invoked.
 * Otherwise, the events may or may not occur, depending on whether DR
 * itself needs to intercept them and whether the system call number
 * is statically determinable.  System call number determination can
 * depend on whether the -opt_speed option is enabled.  If a system
 * call number is not determinable, the filter event will not be
 * called, but the pre and post events will be called.
 *
 * Intercepting every system call can be detrimental to performance
 * for certain types of applications.  Filtering provides for greater
 * performance by letting uninteresting system calls execute without
 * interception overhead.
 */
void
dr_register_filter_syscall_event(bool (*func)(void *drcontext, int sysnum));

/**
 * Unregister a callback function for the syscall filter event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_filter_syscall_event(bool (*func)(void *drcontext, int sysnum));

/**
 * Registers a callback function for the pre-syscall event.  DR calls
 * \p func whenever the application is about to invoke a system call,
 * if any client asked for that system call number to be intercepted
 * via the filter event (dr_register_filter_syscall_event()).
 *
 * The application parameters to the system call can be viewed with
 * dr_syscall_get_param() and set with dr_syscall_set_param().  The
 * system call number can also be changed with
 * dr_syscall_set_sysnum().
 *
 * The application's machine state can be accessed and set with
 * dr_get_mcontext() and dr_set_mcontext().  Changing registers in
 * this way overlaps with system call parameter changes on some
 * platforms.  On Linux, for SYS_clone, client changes to the ebp/rbp
 * register will be ignored by the clone child.
 * 
 * If \p func returns true, the application's system call is invoked
 * normally; if \p func returns false, the system call is skipped.  If
 * it is skipped, the return value can be set with
 * dr_syscall_set_result().  If the system call is skipped, there will
 * not be a post-syscall event.
 */
void
dr_register_pre_syscall_event(bool (*func)(void *drcontext, int sysnum));

/**
 * Unregister a callback function for the pre-syscall event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_pre_syscall_event(bool (*func)(void *drcontext, int sysnum));

/**
 * Registers a callback function for the post-syscall event.  DR calls
 * \p func whenever the application just finished invoking a system
 * call, if any client asked for that system call number to be
 * intercepted via the filter event
 * (dr_register_filter_syscall_event()) or if DR itself needs to
 * intercept the system call.  The result of the system call can be
 * modified with dr_syscall_set_result().
 *
 * System calls that change control flow or terminate the current
 * thread or process typically do not have a post-syscall event.
 * These include SYS_exit, SYS_exit_group, SYS_execve, SYS_sigreturn,
 * and SYS_rt_sigreturn on Linux, and NtTerminateThread,
 * NtTerminateProcess (depending on the parameters), NtCallbackReturn,
 * and NtContinue on Windows.
 *
 * The application's machine state can be accessed and set with
 * dr_get_mcontext() and dr_set_mcontext().
 * 
 * Additional system calls may be invoked by calling
 * dr_syscall_invoke_another() prior to returning from the
 * post-syscall event callback.  The system call to be invoked should
 * be specified with dr_syscall_set_sysnum(), and its parameters can
 * be set with dr_syscall_set_param().
 */
void
dr_register_post_syscall_event(void (*func)(void *drcontext, int sysnum));

/**
 * Unregister a callback function for the post-syscall event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_post_syscall_event(void (*func)(void *drcontext, int sysnum));

#ifdef LINUX

/**
 * Data structure passed with a signal event.  Contains the machine
 * context at the signal interruption point and other signal
 * information.
 */
typedef struct _dr_siginfo_t {
    /** The signal number. */
    int sig;
    /** The context of the thread receiving the signal. */
    void *drcontext;
    /** The application machine state at the signal interruption point. */
    dr_mcontext_t mcontext;
    /** 
     * The raw pre-translated machine state at the signal interruption
     * point inside the code cache.  NULL for delayable signals.  Clients
     * are cautioned when examining code cache instructions to not rely on
     * any details of code inserted other than their own.
     */
    dr_mcontext_t raw_mcontext;
    /** Whether raw_mcontext is valid. */
    bool raw_mcontext_valid;
    /**
     * For SIGBUS and SIGSEGV, the address whose access caused the signal
     * to be raised (as calculated by DR).
     */
    byte *access_address;
    /**
     * Indicates this signal is blocked.  DR_SIGNAL_BYPASS is not allowed,
     * and a second event will be sent if the signal is later delivered to
     * the application.  Events are only sent for blocked non-delayable signals,
     * not for delayable signals.
     */
    bool blocked;       
    /**
     * Information about the code fragment inside the code cache
     * at the signal interruption point.
     */
    dr_fault_fragment_info_t fault_fragment_info;
} dr_siginfo_t;

/**
 * Return value of client signal event callback, determining how DR will
 * proceed with the signal.
 */
typedef enum {
    /** Deliver signal to the application as normal. */
    DR_SIGNAL_DELIVER,
    /** Suppress signal as though it never happened. */
    DR_SIGNAL_SUPPRESS,
    /**
     * Deliver signal according to the default SIG_DFL action, as would
     * happen if the application had no handler.
     */
    DR_SIGNAL_BYPASS,
    /**
     * Do not deliver the signal.  Instead, redirect control to the
     * application state specified in dr_siginfo_t.mcontext.
     */
    DR_SIGNAL_REDIRECT,
} dr_signal_action_t;


/**
 * Requests that DR call the provided callback function \p func whenever a
 * signal is received by any application thread.  The return value of \p
 * func determines whether DR delivers the signal to the application.
 * To redirect execution return DR_SIGNAL_REDIRECT (do not call
 * dr_redirect_execution() from a signal callback).  The callback function
 * will be called even if the application has no handler or has registered
 * a SIG_IGN or SIG_DFL handler.
 *
 * Modifications to the fields of \p siginfo->mcontext will be propagated
 * to the application if it has a handler for the signal, if
 * DR_SIGNAL_DELIVER is returned.
 *
 * The \p siginfo->raw_mcontext data is only provided for non-delayable
 * signals (e.g., SIGSEGV) that must be delivered immediately.  Whether it
 * is supplied is specified in \p siginfo->raw_mcontext_valid.  It is
 * intended for clients using faults as a mechanism to push rare cases out
 * of an instrumentation fastpath that need to examine instrumentation
 * instructions rather than the translated application state.  Certain
 * registers may not contain proper application values in \p
 * excpt->raw_mcontext for exceptions in application instructions.  Clients
 * are cautioned against relying on any details of code cache layout or
 * register usage beyond instrumentation inserted by the client itself.  If
 * DR_SIGNAL_SUPPRESS is returned, \p siginfo->mcontext is ignored and \p
 * siginfo->raw_mcontext is used as the resumption context.  The client's
 * changes to \p siginfo->raw_mcontext will take effect.
 * 
 * For a delayable signal, DR raises a signal event only when about to
 * deliver the signal to the application.  Thus, if the application has
 * blocked a delayable signal, the corresponding signal event will not
 * occur until the application unblocks the signal, even if such a signal
 * is delivered by the kernel.  For non-delayable signals, DR will raise a
 * signal event on initial receipt of the signal, with the \p
 * siginfo->blocked field set.  Such a blocked signal will have a second
 * event raised when it is delivered to the application (if it is not
 * suppressed by the client, and if there is not already a pending blocked
 * signal, for non-real-time signals).
 *
 * DR raises this event for faults outside the code cache that
 * could come from code generated by a client.  For such cases,
 * mcontext is not translated and is identical to raw_mcontext.
 *
 * DR will not raise a signal event for a SIGSEGV or SIGBUS
 * raised by a client code fault rather than the application.  Use
 * dr_safe_read() or dr_safe_write() to prevent such faults.
 *
 * \note \p siginfo->fault_fragment_info data is provided 
 * with \p siginfo->raw_mcontext. It is valid only if 
 * \p siginfo->fault_fragment_info.cache_start_pc is not 
 * \p NULL. It provides clients information about the code fragment
 * being executed at the signal interruption point. Clients are
 * cautioned against relying on any details of code cache layout or
 * register usage beyond instrumentation inserted by the client
 * itself. 
 * 
 * \note Only valid on Linux.
 *
 * \note DR always requests SA_SIGINFO for all signals.
 *
 * \note This version of DR does not intercept the signals SIGCONT,
 * SIGSTOP, SIGTSTP, SIGTTIN, or SIGTTOU.  Future versions should add
 * support for these signals.
 *
 * \note If the client uses signals for its own communication it should set
 * a flag to distinguish its own uses of signals from the application's
 * use.  Races where the two are re-ordered should not be problematic.
 */
void
dr_register_signal_event(dr_signal_action_t (*func)
                         (void *drcontext, dr_siginfo_t *siginfo));

/**
 * Unregister a callback function for the signal event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_signal_event(dr_signal_action_t (*func)
                           (void *drcontext, dr_siginfo_t *siginfo));
#endif /* LINUX */


/**
 * Registers a callback function for nudge events.  External entities
 * can nudge a process through the dr_nudge_process() API routine on
 * Windows or using the \p nudgeunix tool on Linux.  DR then calls \p
 * func whenever the current process receives the nudge.  On Windows,
 * the nudge event is delivered in a new non-application thread.
 * Callers must specify the target client by passing the client ID
 * that was provided in dr_init().
 */
void
dr_register_nudge_event(void (*func)(void *drcontext, uint64 argument), client_id_t id);

/**
 * Unregister a callback function for the nudge event.
 * \return true if unregistration is successful and false if it is not
 * (e.g., \p func was not registered).
 */
bool
dr_unregister_nudge_event(void (*func)(void *drcontext, uint64 argument), client_id_t id);

/**
 * Triggers an asynchronous nudge event in the current process.  The callback
 * function registered with dr_register_nudge_event() will be called with the
 * supplied \p argument (in a new non-application thread on Windows).
 *
 * \note On Linux, the nudge will not be delivered until this thread exits
 * the code cache.  Thus, if this routine is called from a clean call,
 * dr_redirect_execution() should be used to ensure cache exit.
 *
 * \note Not yet supported for 32-bit processes running on 64-bit Windows (WOW64).
 */
bool
dr_nudge_client(client_id_t id, uint64 argument);

#endif /* _DR_EVENTS_H_ */
