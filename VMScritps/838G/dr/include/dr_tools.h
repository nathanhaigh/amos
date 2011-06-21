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

#ifndef _DR_TOOLS_H_
#define _DR_TOOLS_H_ 1

/**************************************************
 * MEMORY INFORMATION TYPES
 */

#define DR_MEMPROT_NONE  0x00 /**< No read, write, or execute privileges. */
#define DR_MEMPROT_READ  0x01 /**< Read privileges. */
#define DR_MEMPROT_WRITE 0x02 /**< Write privileges. */
#define DR_MEMPROT_EXEC  0x04 /**< Execute privileges. */

/**
 * Flags describing memory used by dr_query_memory_ex().
 */
typedef enum {
    DR_MEMTYPE_FREE,  /**< No memory is allocated here */
    DR_MEMTYPE_IMAGE, /**< An executable file is mapped here */
    DR_MEMTYPE_DATA,  /**< Some other data is allocated here */
} dr_mem_type_t;

/**
 * Describes a memory region.  Used by dr_query_memory_ex().
 */
typedef struct _dr_mem_info_t {
    /** Starting address of memory region */
    byte          *base_pc;
    /** Size of region */
    size_t        size;
    /** Protection of region (DR_MEMPROT_* flags) */
    uint          prot;
    /** Type of region */
    dr_mem_type_t type;
} dr_mem_info_t;



/**************************************************
 * MODULE INFORMATION TYPES
 */

/**
 * Type used for dr_get_proc_address().  This can be obtained from the
 * #_module_data_t structure.  It is equivalent to the base address of
 * the module on both Windows and Linux.
 */
typedef void * module_handle_t;

#ifdef WINDOWS

#define MODULE_FILE_VERSION_INVALID ULLONG_MAX

/**
 * Used to hold .rsrc section version number information. This number is usually
 * presented as p1.p2.p3.p4 by PE parsing tools.
 */
typedef union _version_number_t {
    uint64 version;  /**< Representation as a 64-bit integer. */
    struct {
        uint ms;     /**< */
        uint ls;     /**< */
    } version_uint;  /**< Representation as 2 32-bit integers. */
    struct {
        ushort p2;   /**< */
        ushort p1;   /**< */
        ushort p4;   /**< */
        ushort p3;   /**< */
    } version_parts; /**< Representation as 4 16-bit integers. */
} version_number_t;

#endif



/**
 * Holds the names of a module.  This structure contains multiple
 * fields corresponding to different sources of a module name.  Note
 * that some of these names may not exist for certain modules.  It is
 * highly likely, however, that at least one name is available.  Use
 * dr_module_preferred_name() on the parent _module_data_t to get the
 * preferred name of the module.
 */
typedef struct _module_names_t {
    const char *module_name; /**< On windows this name comes from the PE header exports
                              * section (NULL if the module has no exports section).  On
                              * Linux the name will come from the ELF DYNAMIC program 
                              * header (NULL if the module has no SONAME entry). */
    const char *file_name; /**< The file name used to load this module. Note - on Windows
                            * this is not always available. */
#ifdef WINDOWS
    const char *exe_name; /**< If this module is the main executable of this process then
                           * this is the executable name used to launch the process (NULL
                           * for all other modules). */
    const char *rsrc_name; /**< The internal name given to the module in its resource
                            * section. Will be NULL if the module has no resource section
                            * or doesn't set this field within it. */
#else /* LINUX */
    uint64 inode; /**< The inode of the module file mapped in. */
#endif
} module_names_t;




/**************************************************
 * TOP-LEVEL ROUTINES
 */
/**
 * @file dr_tools.h
 * @brief Main API routines, including transparency support.
 */


/**
 * Creates a DR context that can be used in a standalone program.
 * \warning This context cannot be used as the drcontext for a thread 
 * running under DR control!  It is only for standalone programs that 
 * wish to use DR as a library of disassembly, etc. routines.
 */
void *
dr_standalone_init(void);

/**************************************************
 * UTILITY ROUTINES
 */

#ifdef WINDOWS
/**
 * If \p x is false, displays a message about an assertion failure
 * (appending \p msg to the message) and then calls dr_abort()
 */ 
# define DR_ASSERT_MSG(x, msg) \
    ((void)((!(x)) ? \
        (dr_messagebox("ASSERT FAILURE: %s:%d: %s (%s)", __FILE__,  __LINE__, #x, msg),\
         dr_abort(), 0) : 0))
#else
# define DR_ASSERT_MSG(x, msg) \
    ((void)((!(x)) ? \
        (dr_fprintf(STDERR, "ASSERT FAILURE: %s:%d: %s (%s)", \
                     __FILE__,  __LINE__, #x, msg), \
         dr_abort(), 0) : 0))
#endif

/**
 * If \p x is false, displays a message about an assertion failure and
 * then calls dr_abort()
 */ 
#define DR_ASSERT(x) DR_ASSERT_MSG(x, "")



/** Returns true if all \DynamoRIO caches are thread private. */
bool
dr_using_all_private_caches(void);

/**
 * Enables the -synch_at_exit runtime option, which guarantees that no
 * thread will executed beyond its own thread exit event at process
 * exit time.  When the -synch_at_exit option is off, which is the
 * default setting, in release builds there is no such guarantee.
 */
void
dr_request_synchronized_exit(void);

/** 
 * Returns the client-specific option string specified at client
 * registration.  \p client_id is the client ID passed to dr_init().
 */
const char *
dr_get_options(client_id_t client_id);

/**
 * Returns the client library name and path that were originally specified
 * to load the library.  If the resulting string is longer than #MAXIMUM_PATH
 * it will be truncated.  \p client_id is the client ID passed to a client's
 * dr_init() function.
 */
const char *
dr_get_client_path(client_id_t client_id);

/** Returns the image name (without path) of the current application. */
const char *
dr_get_application_name(void);

/** Returns the process id of the current process. */
process_id_t
dr_get_process_id(void);

/**
 * Returns the process id of the parent of the current process. 
 * \note Linux only.
 */
process_id_t
dr_get_parent_id(void);

/** 
 * Returns true if this process is a 32-bit process operating on a
 * 64-bit Windows kernel, known as Windows-On-Windows-64, or WOW64.
 * Returns false otherwise.
 */
bool
dr_is_wow64(void);

/** 
 * Returns a pointer to the application's Process Environment Block
 * (PEB).  DR swaps to a private PEB when running client code, in
 * order to isolate the client and its dependent libraries from the
 * application, so conventional methods of reading the PEB will obtain
 * the private PEB instead of the application PEB.
 */
void *
dr_get_app_PEB(void);

/** Retrieves the current time. */
void
dr_get_time(dr_time_t *time);

/**
 * On Linux, returns the number of milliseconds since the Epoch (Jan 1, 1970).
 * On Windows, returns the number of milliseconds since Jan 1, 1600 (this is
 * the current UTC time).
 */
uint64
dr_get_milliseconds(void);

/**
 * Returns a pseudo-random number in the range [0..max).
 * The pseudo-random sequence can be repeated by passing the seed
 * used during a run to the next run via the -prng_seed runtime option.
 */
uint
dr_get_random_value(uint max);

/** 
 * Sets the seed used for dr_get_random_value().  Generally this would
 * only be called during client initialization.
 */
void
dr_set_random_seed(uint seed);

/** Returns the seed used for dr_get_random_value(). */
uint
dr_get_random_seed(void);

/** Aborts the process immediately. */
void
dr_abort(void);

/**************************************************
 * APPLICATION-INDEPENDENT MEMORY ALLOCATION
 */


/**
 * Allocates \p size bytes of memory from DR's memory pool specific to the
 * thread associated with \p drcontext.
 */
void *
dr_thread_alloc(void *drcontext, size_t size);

/**
 * Frees thread-specific memory allocated by dr_thread_alloc().
 * \p size must be the same as that passed to dr_thread_alloc().
 */
void
dr_thread_free(void *drcontext, void *mem, size_t size);

/** Allocates \p size bytes of memory from DR's global memory pool. */
void *
dr_global_alloc(size_t size);

/**
 * Frees memory allocated by dr_global_alloc().
 * \p size must be the same as that passed to dr_global_alloc().
 */
void
dr_global_free(void *mem, size_t size);

/**
 * Allocates \p size bytes of memory as a separate allocation from DR's
 * heap, allowing for separate protection.
 * The \p prot protection should use the DR_MEMPROT_READ,
 * DR_MEMPROT_WRITE, and DR_MEMPROT_EXEC bits.
 * When creating a region to hold dynamically generated code, use
 * this routine in order to create executable memory.
 */
void *
dr_nonheap_alloc(size_t size, uint prot);

/**
 * Frees memory allocated by dr_nonheap_alloc().
 * \p size must be the same as that passed to dr_nonheap_alloc().
 */
void
dr_nonheap_free(void *mem, size_t size);

/** 
 * Allocates memory from DR's global memory pool, but mimics the
 * behavior of malloc.  Memory must be freed with __wrap_free().  The
 * __wrap routines are intended to be used with ld's -wrap option to
 * replace a client's use of malloc, realloc, and free with internal
 * versions that allocate memory from DR's private pool.  With -wrap,
 * clients can link to libraries that allocate heap memory without
 * interfering with application allocations.
 * \note Currently Linux only.
 */
void *
__wrap_malloc(size_t size);

/** 
 * Reallocates memory from DR's global memory pool, but mimics the
 * behavior of realloc.  Memory must be freed with __wrap_free().  The
 * __wrap routines are intended to be used with ld's -wrap option; see
 * __wrap_malloc() for more information.
 * \note Currently Linux only.
 */
void *
__wrap_realloc(void *mem, size_t size);

/**
 * Frees memory from DR's global memory pool.  Memory must have been
 * allocated with __wrap_malloc(). The __wrap routines are intended to
 * be used with ld's -wrap option; see __wrap_malloc() for more
 * information.
 * \note Currently Linux only.
 */
void
__wrap_free(void *mem);

/**************************************************
 * MEMORY QUERY/ACCESS ROUTINES
 */


/**
 * Checks to see that all bytes with addresses in the range [\p pc, \p pc + \p size - 1]
 * are readable and that reading from that range won't generate an exception (see also
 * dr_safe_read()).
 * \note Nothing guarantees that the memory will stay readable for any length of time.
 * \note On Linux, especially if the app is in the middle of loading a library
 * and has not properly set up the .bss yet, a page that seems readable can still
 * generate SIGBUS if beyond the end of an mmapped file.  Use dr_safe_read() to
 * avoid such problems.
 */
bool
dr_memory_is_readable(const byte *pc, size_t size);

/**
 * An os neutral method for querying a memory address. Returns true
 * iff a memory region containing \p pc is found.  If found additional
 * information about the memory region is returned in the optional out
 * arguments \p base_pc, \p size, and \p prot where \p base_pc is the
 * start address of the memory region continaing \p pc, \p size is the
 * size of said memory region and \p prot is an ORed combination of
 * DR_MEMPROT_* flags describing its current protection.
 *
 * \note To examine only application memory, skip memory for which
 * dr_memory_is_dr_internal() or dr_memory_is_in_client() returns true.
 */
bool
dr_query_memory(const byte *pc, byte **base_pc, size_t *size, uint *prot);

/**
 * Provides additional information beyond dr_query_memory().
 * Returns true if it was able to obtain information (including about
 * free regions) and sets the fields of \p info.  This routine can be
 * used to iterate over the entire address space.
 * Returns false on failure.
 *
 * \note To examine only application memory, skip memory for which
 * dr_memory_is_dr_internal() returns true.
 */
bool
dr_query_memory_ex(const byte *pc, OUT dr_mem_info_t *info);
#ifdef WINDOWS 


/**
 * Equivalent to the win32 API function VirtualQuery().
 * See that routine for a description of
 * arguments and return values.  \note Windows-only.
 */
size_t
dr_virtual_query(const byte *pc, MEMORY_BASIC_INFORMATION *mbi, size_t mbi_size);
#endif


/**
 * Safely reads \p size bytes from address \p base into buffer \p
 * out_buf.  Reading is done without the possibility of an exception
 * occurring.  Optionally returns the actual number of bytes copied
 * into \p bytes_read.  Returns true if successful.  
 */
bool
dr_safe_read(const void *base, size_t size, void *out_buf, size_t *bytes_read);

/**
 * Safely writes \p size bytes from buffer \p in_buf to address \p
 * base.  Writing is done without the possibility of an exception
 * occurring.  Optionally returns the actual number of bytes copied
 * into \p bytes_written.  Returns true if successful.
 */
bool
dr_safe_write(void *base, size_t size, const void *in_buf, size_t *bytes_written);

/**
 * Modifies the memory protections of the region from \p start through
 * \p start + \p size.  Modification of memory allocated by DR or of
 * the DR or client libraries themselves is allowed under the
 * assumption that the client knows what it is doing.  Modification of
 * the ntdll.dll library on Windows is not allowed.  Returns true if
 * successful.
 */
bool
dr_memory_protect(void *base, size_t size, uint new_prot);

/**
 * Returns true iff pc is memory allocated by DR for its own
 * purposes, and would not exist if the application were run
 * natively.
 */
bool
dr_memory_is_dr_internal(const byte *pc);

/**
 * Returns true iff pc is located inside a client library.
 */
bool
dr_memory_is_in_client(const byte *pc);

/**************************************************
 * SIMPLE MUTEX SUPPORT
 */


/** 
 * Initializes a mutex. 
 *
 * Warning: there are restrictions on when DR-provided mutexes, and
 * locks in general, can be held by a client: no lock should be held
 * while application code is executing in the code cache.  Locks can
 * be used while inside client code reached from clean calls out of
 * the code cache, but they must be released before returning to the
 * cache.  Failing to follow these restrictions can lead to deadlocks.
 */
void *
dr_mutex_create(void);

/** Deletes \p mutex. */
void
dr_mutex_destroy(void *mutex);

/** Locks \p mutex.  Waits until the mutex is successfully held. */
void
dr_mutex_lock(void *mutex);

/** Unlocks \p mutex.  Asserts that mutex is currently locked. */
void
dr_mutex_unlock(void *mutex);

/** Tries once to lock \p mutex, returns whether or not successful. */
bool
dr_mutex_trylock(void *mutex);
/**************************************************
 * MODULE INFORMATION ROUTINES
 */

/** For dr_module_iterator_* interface */
typedef void * dr_module_iterator_t;

#ifdef LINUX
/** Holds information on a segment of a loaded module. */
typedef struct _module_segment_data_t {
    app_pc start; /**< Start address of the segment, page-aligned backward. */
    app_pc end;   /**< End address of the segment, page-aligned forward. */
    uint prot;    /**< Protection attributes of the segment */
} module_segment_data_t;
#endif

/**
 * Holds information about a loaded module. \note On Linux the start address can be
 * cast to an Elf32_Ehdr or Elf64_Ehdr. \note On Windows the start address can be cast to
 * an IMAGE_DOS_HEADER for use in finding the IMAGE_NT_HEADER and its OptionalHeader.
 * The OptionalHeader can be used to walk the module sections (among other things).
 * See WINNT.H. \note When accessing any memory inside the module (including header fields)
 * user is responsible for guarding against corruption and the possibility of the module
 * being unmapped.
 */
struct _module_data_t {
    union {
        app_pc start; /**< starting address of this module */
        module_handle_t handle; /**< module_handle for use with dr_get_proc_address() */
    } ; /* anonymous union of start address and module handle */
    /**
     * Ending address of this module.  Note that on Linux the module may not
     * be contiguous: there may be gaps containing other objects between start
     * and end.  Use the segments array to examine each mapped region on Linux.
     */
    app_pc end;

    app_pc entry_point; /**< entry point for this module as specified in the headers */

    uint flags; /**< Reserved, set to 0 */

    module_names_t names; /**< struct containing name(s) for this module; use
                           * dr_module_preferred_name() to get the preferred name for
                           * this module */

    char *full_path; /**< full path to the file backing this module */

#ifdef WINDOWS
    version_number_t file_version; /**< file version number from .rsrc section */
    version_number_t product_version; /**< product version number from .rsrc section */
    uint checksum; /**< module checksum from the PE headers */
    uint timestamp; /**< module timestamp from the PE headers */
    size_t module_internal_size; /**< module internal size (from PE headers SizeOfImage) */
#else
    bool contiguous;   /**< whether there are no gaps between segments */
    uint num_segments; /**< number of segments */
    /**
     * Array of num_segments entries, one per segment.  The array is sorted
     * by the start address of each segment.
     */
    module_segment_data_t *segments;
#endif
};



/**
 * Looks up the module containing \p pc.  If a module containing \p pc is found returns
 * a module_data_t describing that module else returns NULL.  Can be used to
 * obtain a module_handle_t for dr_lookup_module_section().
 * \note Returned module_data_t must be freed with dr_free_module_data().
 */
module_data_t *
dr_lookup_module(byte *pc);

/**
 * Looks up the module with name \p name ignoring case.  If an exact name match is found
 * returns a module_data_t describing that module else returns NULL.  User must call
 * dr_free_module_data() on the returned module_data_t once finished. Can be used to
 * obtain a module_handle_t for dr_get_proc_address().
 * \note Returned module_data_t must be freed with dr_free_module_data().
 */
module_data_t *
dr_lookup_module_by_name(const char *name);

/**
 * Initialize a new module iterator.  The returned module iterator contains a snapshot
 * of the modules loaded at the time it was created.  Use dr_module_iterator_hasnext()
 * and dr_module_iterator_next() to walk the loaded modules.  Call
 * dr_module_iterator_stop() when finished to release the iterator. \note The iterator
 * does not prevent modules from being loaded or unloaded while the iterator is being
 * walked.
 */
dr_module_iterator_t *
dr_module_iterator_start(void);

/**
 * Returns true if there is another loaded module in the iterator.
 */
bool
dr_module_iterator_hasnext(dr_module_iterator_t *mi);

/**
 * Retrieves the module_data_t for the next loaded module in the iterator. User must call
 * dr_free_module_data() on the returned module_data_t once finished.
 * \note Returned module_data_t must be freed with dr_free_module_data().
 */
module_data_t *
dr_module_iterator_next(dr_module_iterator_t *mi);

/**
 * User should call this routine to free the module iterator.
 */
void
dr_module_iterator_stop(dr_module_iterator_t *mi);

/**
 * Makes a copy of \p data.  Copy must be freed with dr_free_module_data().
 * Useful for making persistent copies of module_data_t's received as part of
 * image load and unload event callbacks.
 */
module_data_t *
dr_copy_module_data(const module_data_t *data);

/**
 * Frees a module_data_t returned by dr_module_iterator_next(), dr_lookup_module(),
 * dr_lookup_module_by_name(), or dr_copy_module_data(). \note Should NOT be used with
 * a module_data_t obtained as part of a module load or unload event.
 */
void
dr_free_module_data(module_data_t *data);

/**
 * Returns the preferred name for the module described by \p data from
 * \p data->module_names.
 */
const char *
dr_module_preferred_name(const module_data_t *data);
#ifdef WINDOWS


/**
 * Returns whether \p pc is within a section within the module in \p section_found and
 * information about that section in \p section_out. \note Not yet available on Linux.
 */
bool
dr_lookup_module_section(module_handle_t lib,
                         byte *pc, IMAGE_SECTION_HEADER *section_out);
#endif /* WINDOWS */


/**
 * Returns the entry point of the exported function with the given
 * name in the module with the given base.  Returns NULL on failure.
 * \note On Linux this ignores symbol preemption by other modules and only
 * examines the specified module.
 * \note On Linux, in order to handle indirect code objects, use
 * dr_get_proc_address_ex().
 */
generic_func_t
dr_get_proc_address(module_handle_t lib, const char *name);
/**
 * Data structure used by dr_get_proc_address_ex() to retrieve information
 * about an exported symbol.
 */
typedef struct _dr_export_info_t {
    /**
     * The entry point of the export as an absolute address located
     * within the queried module.  This address is identical to what
     * dr_get_proc_address_ex() returns.
     */
    generic_func_t address;
    /**
     * Relevant for Linux only.  Set to true iff this export is an
     * indirect code object, which is a new ELF extension allowing
     * runtime selection of which implementation to use for an
     * exported symbol.  The address of such an export is a function
     * that takes no arguments and returns the address of the selected
     * implementation.
     */
    bool is_indirect_code;
} dr_export_info_t;


/**
 * Returns information in \p info about the symbol \p name exported
 * by the module \p lib.  Returns false if the symbol is not found.
 * \note On Linux this ignores symbol preemption by other modules and only
 * examines the specified module.
 */
bool
dr_get_proc_address_ex(module_handle_t lib, const char *name,
                       dr_export_info_t *info OUT, size_t info_len);
/**************************************************
 * SYSTEM CALL PROCESSING ROUTINES
 */


/**
 * Usable only from a pre-syscall (dr_register_pre_syscall_event()) 
 * event.  Returns the value of system call parameter number \p param_num.
 */
reg_t
dr_syscall_get_param(void *drcontext, int param_num);

/**
 * Usable only from a pre-syscall (dr_register_pre_syscall_event())
 * event, or from a post-syscall (dr_register_post_syscall_event())
 * event when also using dr_syscall_invoke_another().  Sets the value
 * of system call parameter number \p param_num to \p new_value.
 */
void
dr_syscall_set_param(void *drcontext, int param_num, reg_t new_value);

/**
 * Usable only from a post-syscall (dr_register_post_syscall_event())
 * event.  Returns the return value of the system call that will be
 * presented to the application.
 */
reg_t
dr_syscall_get_result(void *drcontext);

/**
 * Usable only from a pre-syscall (dr_register_pre_syscall_event()) or
 * post-syscall (dr_register_post_syscall_event()) event.
 * For pre-syscall, should only be used when skipping the system call.
 * This sets the return value of the system call that the application sees
 * to \p value.
 */
void
dr_syscall_set_result(void *drcontext, reg_t value);

/**
 * Usable only from a pre-syscall (dr_register_pre_syscall_event())
 * event, or from a post-syscall (dr_register_post_syscall_event())
 * event when also using dr_syscall_invoke_another().  Sets the system
 * call number of the system call about to be invoked to \p new_num.
 */
void
dr_syscall_set_sysnum(void *drcontext, int new_num);

/**
 * Usable only from a post-syscall (dr_register_post_syscall_event())
 * event.  An additional system call will be invoked immediately,
 * using the current values of the parameters, which can be set with
 * dr_syscall_set_param().  The system call to be invoked should be
 * specified with dr_syscall_set_sysnum().
 *
 * Use this routine with caution.  Especially on Windows, care must be
 * taken if the application is expected to continue afterward.  When
 * system call parameters are stored on the stack, modifying them can
 * result in incorrect application behavior, particularly when setting
 * more parameters than were present in the original system call,
 * which will result in corruption of the application stack.
 *
 * On Windows, when the first system call is interruptible
 * (alertable), the additional system call may be delayed.
 *
 * DR will set key registers such as r10 for 64-bit or xdx for
 * sysenter or WOW64 system calls.  However, DR will not set ecx for
 * WOW64; that is up to the client.
 */
void
dr_syscall_invoke_another(void *drcontext);

/**
 * Creates a new directory.  Fails if the directory already exists
 * or if it can't be created.
 */
bool
dr_create_dir(const char *fname);

/** Checks for the existence of a directory. */
bool
dr_directory_exists(const char *fname);

/** Checks the existence of a file. */
bool
dr_file_exists(const char *fname);

/* flags for use with dr_open_file() */
/** Open with read access. */
#define DR_FILE_READ               0x1
/** Open with write access, but do not open if the file already exists. */
#define DR_FILE_WRITE_REQUIRE_NEW  0x2
/**
 * Open with write access.  If the file already exists, set the file position to the
 * end of the file.
 */
#define DR_FILE_WRITE_APPEND       0x4
/**
 * Open with write access.  If the file already exists, truncate the
 * file to zero length.
 */
#define DR_FILE_WRITE_OVERWRITE    0x8
/**
 * Open with large (>2GB) file support.  Only applicable on 32-bit Linux.
 * \note DR's log files and tracedump files are all created with this flag.
 */
#define DR_FILE_ALLOW_LARGE       0x10


/**
 * Opens the file \p fname. If no such file exists then one is created.
 * The file access mode is set by the \p mode_flags argument which is drawn from
 * the DR_FILE_* defines ORed together.  Returns INVALID_FILE if unsuccessful.
 *
 * On Windows, \p fname must be an absolute path (when using Windows
 * system calls directly there is no such thing as a relative path.
 * On Windows the notions of current directory and relative paths are
 * limited to user space via the Win32 API.  We may add limited
 * support for using the same current directory via Issue 298.)
 *
 * \note No more then one write mode flag can be specified.
 *
 * \note DR does not hide files opened by clients from the
 * application, to allow clients to open application files and other
 * forms of interaction.  Some applications close all file
 * descriptors, and clients may want to watch for the close system
 * call and turn it into a nop (e.g., return -EBADF and not execute it
 * on Linux) if targeting a client-owned filed.
 */
file_t
dr_open_file(const char *fname, uint mode_flags);

/** Closes file \p f. */
void
dr_close_file(file_t f);

/** Flushes any buffers for file \p f. */
void
dr_flush_file(file_t f);

/**
 * Writes \p count bytes from \p buf to file \p f.  
 * Returns the actual number written.
 */
ssize_t
dr_write_file(file_t f, const void *buf, size_t count);

/**
 * Reads up to \p count bytes from file \p f into \p buf.
 * Returns the actual number read.
 */
ssize_t
dr_read_file(file_t f, void *buf, size_t count);

/* For use with dr_file_seek(), specifies the origin at which to apply the offset. */
#define DR_SEEK_SET 0  /**< start of file */
#define DR_SEEK_CUR 1  /**< current file position */
#define DR_SEEK_END 2  /**< end of file */


/**
 * Sets the current file position for file \p f to \p offset bytes
 * from the specified origin, where \p origin is one of the DR_SEEK_*
 * values.  Returns true if successful.
 */
bool
dr_file_seek(file_t f, int64 offset, int origin);

/**
 * Returns the current position for the file \p f in bytes from the start of the file.
 * Returns -1 on an error.
 */
int64
dr_file_tell(file_t f);

/**
 * Returns a new copy of the file handle \p f.
 * Returns INVALID_FILE on error.
 */
file_t
dr_dup_file_handle(file_t f);

/**************************************************
 * PRINTING
 */


/**
 * Writes to DR's log file for the thread with drcontext \p drcontext if the current
 * loglevel is >= \p level and the current \p logmask & \p mask != 0.
 * The mask constants are below.
 * Logging is disabled for the release build.
 * If \p drcontext is NULL, writes to the main log file.
 */
void
dr_log(void *drcontext, uint mask, uint level, const char *fmt, ...);

/* The log mask constants */
#define LOG_NONE           0x00000000  /**< Log no data. */                              
#define LOG_STATS          0x00000001  /**< Log per-thread and global statistics. */     
#define LOG_TOP            0x00000002  /**< Log top-level information. */                
#define LOG_THREADS        0x00000004  /**< Log data related to threads. */              
#define LOG_SYSCALLS       0x00000008  /**< Log data related to system calls. */         
#define LOG_ASYNCH         0x00000010  /**< Log data related to signals/callbacks/etc. */
#define LOG_INTERP         0x00000020  /**< Log data related to app interpretation. */   
#define LOG_EMIT           0x00000040  /**< Log data related to emitting code. */        
#define LOG_LINKS          0x00000080  /**< Log data related to linking code. */         
#define LOG_CACHE          0x00000100  /**< Log data related to code cache management. */
#define LOG_FRAGMENT       0x00000200  /**< Log data related to app code fragments. */   
#define LOG_DISPATCH       0x00000400  /**< Log data on every context switch dispatch. */
#define LOG_MONITOR        0x00000800  /**< Log data related to trace building. */       
#define LOG_HEAP           0x00001000  /**< Log data related to memory management. */     
#define LOG_VMAREAS        0x00002000  /**< Log data related to address space regions. */
#define LOG_SYNCH          0x00004000  /**< Log data related to synchronization. */      
#define LOG_MEMSTATS       0x00008000  /**< Log data related to memory statistics. */    
#define LOG_OPTS           0x00010000  /**< Log data related to optimizations. */        
#define LOG_SIDELINE       0x00020000  /**< Log data related to sideline threads. */ 
#define LOG_SYMBOLS        0x00040000  /**< Log data related to app symbols. */ 
#define LOG_RCT            0x00080000  /**< Log data related to indirect transfers. */ 
#define LOG_NT             0x00100000  /**< Log data related to Windows Native API. */ 
#define LOG_HOT_PATCHING   0x00200000  /**< Log data related to hot patching. */ 
#define LOG_HTABLE         0x00400000  /**< Log data related to hash tables. */ 
#define LOG_MODULEDB       0x00800000  /**< Log data related to the module database. */ 
#define LOG_ALL            0x00ffffff  /**< Log all data. */


/**
 * Returns the log file for the thread with drcontext \p drcontext.
 * If \p drcontext is NULL, returns the main log file.
 */
file_t
dr_get_logfile(void *drcontext);

/**
 * Returns true iff the -stderr_mask runtime option is non-zero, indicating
 * that the user wants notification messages printed to stderr.
 */
bool
dr_is_notify_on(void);

/** Returns a handle to stdout. */
file_t 
dr_get_stdout_file(void);

/** Returns a handle to stderr. */
file_t
dr_get_stderr_file(void);

/** Returns a handle to stdin. */
file_t
dr_get_stdin_file(void);
#ifdef WINDOWS 


/** 
 * Displays a message in a pop-up window. \note Windows only. \note On Windows Vista
 * most Windows services are unable to display message boxes.
 */
void
dr_messagebox(const char *fmt, ...);
#endif


/**
 * Stdout printing that won't interfere with the
 * application's own printing.  Currently non-buffered.
 * \note On Windows, this routine is not able to print to the cmd window
 * (issue 261).  The drsym_write_to_console() routine in the \p drsyms
 * Extension can be used to accomplish that.
 * \note On Windows, this routine does not support printing floating
 * point values.  Use dr_snprintf() instead.
 * \note If the data to be printed is large it will be truncated to
 * an internal buffer size.
 */
void 
dr_printf(const char *fmt, ...);

/**
 * Printing to a file that won't interfere with the
 * application's own printing.  Currently non-buffered.
 * \note On Windows, this routine is not able to print to STDOUT
 * or STDERR in the cmd window (issue 261).  The
 * drsym_write_to_console() routine in the \p drsyms Extension can be
 * used to accomplish that.
 * \note On Windows, this routine does not support printing floating
 * point values.  Use dr_snprintf() instead.
 * \note If the data to be printed is large it will be truncated to
 * an internal buffer size.  Use dr_write_file() to print large buffers.
 * \note On Linux this routine does not check for errors like EINTR.  Use
 * dr_write_file() if that is a concern.
 */
void
dr_fprintf(file_t f, const char *fmt, ...);

/**
 * Utility routine to print a formatted message to a string.  Will not
 * print more than max characters.  If successful, returns the number
 * of characters printed, not including the terminating null
 * character.  If the number of characters to write equals max, then
 * the caller is responsible for supplying a terminating null
 * character.  If the number of characters to write exceeds max, then
 * max characters are written and -1 is returned.  If an error
 * occurs, a negative value is returned.
 * \note This routine does not support printing wide characters.  On
 * Windows you can use _snprintf() instead (though _snprintf() does
 * not support printing floating point values).
 * \note If the data to be printed is large it will be truncated to
 * an internal buffer size.
 */
int
dr_snprintf(char *buf, size_t max, const char *fmt, ...);

/** Prints \p msg followed by the instruction \p instr to file \p f. */
void 
dr_print_instr(void *drcontext, file_t f, instr_t *instr, const char *msg);

/** Prints \p msg followed by the operand \p opnd to file \p f. */
void 
dr_print_opnd(void *drcontext, file_t f, opnd_t opnd, const char *msg);

/**************************************************
 * THREAD SUPPORT
 */


/**
 * Returns the DR context of the current thread. 
 */
void *
dr_get_current_drcontext(void);

/** Returns the thread id of the thread with drcontext \p drcontext. */
thread_id_t 
dr_get_thread_id(void *drcontext);

/**
 * Returns the user-controlled thread-local-storage field.  To
 * generate an instruction sequence that reads the drcontext field
 * inline in the code cache, use dr_insert_read_tls_field().
 */
void *
dr_get_tls_field(void *drcontext);

/** 
 * Sets the user-controlled thread-local-storage field.  To
 * generate an instruction sequence that reads the drcontext field
 * inline in the code cache, use dr_insert_write_tls_field().
 */
void 
dr_set_tls_field(void *drcontext, void *value);

/**
 * Allocates \p num_slots contiguous thread-local storage slots that
 * can be directly accessed via an offset from \p segment_register.
 * These slots will be initialized to 0 for each new thread.
 * The slot offsets are [\p offset .. \p offset + (num_slots - 1)].
 * These slots are disjoint from the #dr_spill_slot_t register spill slots
 * and the client tls field (dr_get_tls_field()).
 * Returns whether or not the slots were successfully obtained.
 *
 * \note These slots are useful for thread-shared code caches.  With
 * thread-private caches, DR's memory pools are guaranteed to be
 * reachable via absolute or rip-relative accesses from the code cache
 * and client libraries.
 *
 * \note These slots are a limited resource.  On Windows the slots are
 * shared with the application and reserving even one slot can result
 * in failure to initialize for certain applications.  On Linux they
 * are more plentiful and transparent but currently DR limits clients
 * to no more than 64 slots.
 */
bool
dr_raw_tls_calloc(OUT reg_id_t *segment_register,
                  OUT uint *offset,
                  IN  uint num_slots,
                  IN  uint alignment);

/**
 * Frees \p num_slots raw thread-local storage slots starting at
 * offset \p offset that were allocated with dr_raw_tls_calloc().
 * Returns whether or not the slots were successfully freed.
 */
bool
dr_raw_tls_cfree(uint offset, uint num_slots);

/**
 * Creates a new thread that is marked as a non-application thread (i.e., DR
 * will let it run natively and not execute its code from the code cache).  The
 * thread will terminate automatically simply by returning from \p func; if
 * running when the application terminates its last thread, the client thread
 * will also terminate when DR shuts the process down.
 *
 * Init and exit events will not be raised for this thread (instead simply place
 * init and exit code in \p func).
 *
 * The new client thread has a drcontext that can be used for thread-private
 * heap allocations.  It has a stack of the same size as the DR stack used by
 * application threads.
 *
 * On Linux, this thread is guaranteed to have its own private itimer
 * if dr_set_itimer() is called from it.  However this does mean it
 * will have its own process id.
 *
 * A client thread should refrain from spending most of its time in
 * calls to other libraries or making blocking or long-running system
 * calls as such actions may incur performance or correctness problems
 * with DR's synchronization engine, which needs to be able to suspend
 * client threads at safe points and cannot determine whether the
 * aforementioned actions are safe for suspension.  Calling
 * dr_sleep(), dr_thread_yield(), dr_messagebox(), or using DR's locks
 * are safe.
 *
 * \note Thread creation via this routine is not yet fully
 * transparent: on Windows, the thread will show up in the list of
 * application threads if the operating system is queried about
 * threads.  The thread will not trigger a DLL_THREAD_ATTACH message.
 */
bool
dr_create_client_thread(void (*func)(void *param), void *arg);

/** Current thread sleeps for \p time_ms milliseconds. */
void
dr_sleep(int time_ms);

/** Current thread gives up its time quantum. */
void
dr_thread_yield(void);

/**
 * Suspends all other threads in the process and returns an array of
 * contexts in \p drcontexts with one context per successfully suspended
 * threads.  The contexts can be passed to routines like dr_get_thread_id()
 * or dr_get_mcontext().  However, the contexts may not be modified:
 * dr_set_mcontext() is not supported.  dr_get_mcontext() can be called on
 * the caller of this routine, unless in a Windows nudge callback.
 *
 * The number of successfully suspended threads, which is also the length
 * of the \p drcontexts array, is returned in \p num_suspended, which is a
 * required parameter.  The number of un-successfully suspended threads, if
 * any, is returned in the optional parameter \p num_unsuspended.  The
 * calling thread is not considered in either count.  DR can fail to
 * suspend a thread for privilege reasons (e.g., on Windows in a
 * low-privilege process where another process injected a thread).  This
 * function returns true iff all threads were suspended, in which case \p
 * num_unsuspended will be 0.
 *
 * The caller must invoke dr_resume_all_other_threads() in order to resume
 * the suspended threads, free the \p drcontexts array, and release
 * coarse-grain locks that prevent new threads from being created.
 *
 * This routine may not be called from any registered event callback
 * other than the nudge event or the pre- or post-system call event.
 * It may be called from clean calls out of the cache.
 * This routine may not be called while any locks are held that
 * could block a thread processing a registered event callback or cache
 * callout.
 *
 * \note A client wishing to invoke this routine from an event callback can
 * queue up a nudge via dr_nudge_client() and invoke this routine from the
 * nudge callback.
 */
bool
dr_suspend_all_other_threads(OUT void ***drcontexts,
                             OUT uint *num_suspended,
                             OUT uint *num_unsuspended);

/**
 * May only be used after invoking dr_suspend_all_other_threads().  This
 * routine resumes the threads that were suspended by
 * dr_suspend_all_other_threads() and must be passed the same array and
 * count of suspended threads that were returned by
 * dr_suspend_all_other_threads().  It also frees the \p drcontexts array
 * and releases the locks acquired by dr_suspend_all_other_threads().  The
 * return value indicates whether all resumption attempts were successful.
 */
bool
dr_resume_all_other_threads(IN void **drcontexts,
                            IN uint num_suspended);

/**
 * Installs an interval timer in the itimer sharing group that
 * contains the calling thread.
 *
 * @param[in] which Must be one of ITIMER_REAL, ITIMER_VIRTUAL, or ITIMER_PROF
 * @param[in] millisec The frequency of the timer, in milliseconds.  Passing
 *   0 disables the timer.
 * @param[in] func The function that will be called each time the
 *   timer fires.  It will be passed the context of the thread that
 *   received the itimer signal and its machine context, which has not
 *   been translated and so may contain raw code cache values.  The function
 *   will be called from a signal handler that may have interrupted
 *   lock holder or other critical code, so it must be careful in its
 *   operations.  If a general timer that does not interrupt client code
 *   is required, the client should create a separate thread via
 *   dr_create_client_thread() (which is guaranteed to have a private
 *   itimer) and set the itimer there, where the callback function can
 *   perform more operations safely if that new thread never acquires locks
 *   in its normal operation.
 *
 * Itimer sharing varies by kernel.  Prior to 2.6.12 itimers were
 * thread-private; after 2.6.12 they are shared across a thread group,
 * though there could be multiple thread groups in one address space.
 * The dr_get_itimer() function can be used to see whether a thread
 * already has an itimer in its group to avoid re-setting an itimer
 * set by an earlier thread.
 *
 * The itimer will operate successfully in the presence of an
 * application itimer of the same type.
 *
 * The return value indicates whether the timer was successfully
 * installed (or uninstalled if 0 was passed for \p millisec).
 *
 * \note Linux-only.
 */
bool
dr_set_itimer(int which, uint millisec,
              void (*func)(void *drcontext, dr_mcontext_t *mcontext));

/**
 * If an interval timer is already installed in the itimer sharing group that
 * contains the calling thread, returns its frequency.  Else returns 0.
 *
 * @param[in] which Must be one of ITIMER_REAL, ITIMER_VIRTUAL, or ITIMER_PROF
 *
 * \note Linux-only.
 */
uint
dr_get_itimer(int which);

/****************************************************************************
 * ADAPTIVE OPTIMIZATION SUPPORT
 */


/**
 * Replaces the fragment with tag \p tag with the instructions in \p
 * ilist.  This routine is only valid with the -thread_private option;
 * it replaces the fragment for the current thread only.  After
 * replacement, the existing fragment is allowed to complete if
 * currently executing.  For example, a clean call replacing the
 * currently executing fragment will safely return to the existing
 * code.  Subsequent executions will use the new instructions.
 *
 * \note The routine takes control of \p ilist and all responsibility
 * for deleting it.  The client should not keep, use, or reference,
 * the instrlist or any of the instrs it contains after passing.
 *
 * \note This routine supports replacement for the current thread
 * only.  \p drcontext must be from the current thread and must
 * be the drcontext used to create the instruction list.
 *
 * \return false if the fragment does not exist and true otherwise.
 */
bool
dr_replace_fragment(void *drcontext, void *tag, instrlist_t *ilist);

/**
 * Deletes the fragment with tag \p tag.  This routine is only valid
 * with the -thread_private option; it deletes the fragment in the
 * current thread only.  After deletion, the existing fragment is
 * allowed to complete execution.  For example, a clean call deleting
 * the currently executing fragment will safely return to the existing
 * code.  Subsequent executions will cause \DynamoRIO to reconstruct
 * the fragment, and therefore call the appropriate fragment-creation
 * event hook, if registered.
 *
 * \note This routine supports deletion for the current thread
 * only.  \p drcontext must be from the current thread and must
 * be the drcontext used to create the instruction list.
 *
 * \return false if the fragment does not exist and true otherwise.
 */
bool
dr_delete_fragment(void *drcontext, void *tag);

/**
 * Flush all fragments containing any code from the region [\p start, \p start + \p size).
 * Once this routine returns no execution will occur out of the fragments flushed.
 * This routine may only be called during a clean call from the cache, from a nudge
 * event handler, or from a pre- or post-system call event handler.
 * It may not be called from any other event callback.  No locks can
 * held when calling this routine.  If called from a clean call, caller can NOT return
 * to the cache (the fragment that was called out of may have been flushed even if it
 * doesn't apparently overlap the flushed region). Instead the caller must call
 * dr_redirect_execution() after this routine to continue execution.  Returns true if
 * successful.
 *
 * \note This routine may not be called from any registered event callback other than
 * the nudge event or the pre- or post-system call event; clean calls
 * out of the cache may call this routine.
 * \note If called from a clean call, caller must continue execution by calling
 * dr_redirect_execution() after this routine, as the fragment containing the callout may
 * have been flushed. The context and app_errno to use can be obtained via
 * dr_get_mcontext() with the exception of the pc to continue at which must be passed as
 * an argument to the callout (see instr_get_app_pc()) or otherwise determined.
 * \note This routine may not be called while any locks are held that could block a thread
 * processing a registered event callback or cache callout.
 * \note dr_delay_flush_region() has fewer restrictions on use, but is less synchronous.
 * \note Use \p size == 1 to flush fragments containing the instruction at address 
 * \p start. A flush of \p size == 0 is not allowed.
 * \note As currently implemented, dr_delay_flush_region() with no completion callback
 * routine specified can be substantially more performant. 
 */
bool
dr_flush_region(app_pc start, size_t size);

/**
 * Flush all fragments containing any code from the region [\p start, \p start + \p size).
 * Control will not enter a fragment containing code from the region after this returns,
 * but a thread already in such a fragment will finish out the fragment.  This includes
 * the current thread if this is called from a clean call that returns to the cache.
 * This routine may only be called during a clean call from the cache, from a nudge
 * event handler, or from a pre- or post-system call event handler.
 * It may not be called from any other event callback.  No locks can be
 * held when calling this routine.  Returns true if successful.
 *
 * \note This routine may not be called from any registered event callback other than
 * the nudge event or the pre- or post-system call event; clean calls
 * out of the cache may call this routine.
 * \note This routine may not be called while any locks are held that could block a thread
 * processing a registered event callback or cache callout.
 * \note dr_delay_flush_region() has fewer restrictions on use, but is less synchronous.
 * \note Use \p size == 1 to flush fragments containing the instruction at address 
 * \p start. A flush of \p size == 0 is not allowed.
 * \note This routine is only available with either the -thread_private
 * or -enable_full_api options.  It is not available when -opt_memory is specified.
 */
bool
dr_unlink_flush_region(app_pc start, size_t size);

/**
 * Request a flush of all fragments containing code from the region 
 * [\p start, \p start + \p size).  The flush will be performed at the next safe 
 * point in time (usually before any new code is added to the cache after this 
 * routine is called). If \p flush_completion_callback is non-NULL, it will be 
 * called with the \p flush_id provided to this routine when the flush completes, 
 * after which no execution will occur out of the fragments flushed. Returns true 
 * if the flush was successfully queued.
 *
 * \note dr_flush_region() and dr_unlink_flush_region() can give stronger guarantees on
 * when the flush will occur, but have more restrictions on use.
 * \note Use \p size == 1 to flush fragments containing the instruction at address 
 * \p start.  A flush of \p size == 0 is not allowed.
 * \note As currently implemented there may be a performance penalty for requesting a
 * \p flush_completion_callback; for most performant usage set 
 * \p flush_completion_callback to NULL.
 */
bool
dr_delay_flush_region(app_pc start, size_t size, uint flush_id,
                      void (*flush_completion_callback) (int flush_id));

/** Returns whether or not there is a fragment in code cache with tag \p tag. */
bool
dr_fragment_exists_at(void *drcontext, void *tag);

/**
 * Returns true if a basic block with tag \p tag exists in the code cache.
 */
bool
dr_bb_exists_at(void *drcontext, void *tag);

/**
 * Looks up the fragment with tag \p tag.
 * If not found, returns 0.
 * If found, returns the total size occupied in the cache by the fragment.
 */
uint
dr_fragment_size(void *drcontext, void *tag);

/** Retrieves the application PC of a fragment with tag \p tag. */
app_pc
dr_fragment_app_pc(void *tag);

/**
 * Given an application PC, returns a PC that contains the application code
 * corresponding to the original PC.  In some circumstances on Windows DR
 * inserts a jump on top of the original code, which the client will not
 * see in the bb and trace hooks due to DR replacing it there with the
 * displaced original application code in order to present the client with
 * an unmodified view of the application code.  A client should use this
 * routine when attempting to decode the original application instruction
 * that caused a fault from the translated fault address, as the translated
 * address may actually point in the middle of DR's jump.
 *
 * \note Other applications on the system sometimes insert their own hooks,
 * which will not be hidden by DR and will appear to the client as jumps
 * and subsequent displaced code.
 */
app_pc
dr_app_pc_for_decoding(app_pc pc);

/**
 * Given a code cache pc, returns the corresponding application pc.
 * This involves translating the state and thus may incur calls to
 * the basic block and trace events (see dr_register_bb_event()).
 * If translation fails, returns NULL.
 */
app_pc
dr_app_pc_from_cache_pc(byte *cache_pc);

/****************************************************************************
 * CUSTOM TRACE SUPPORT
 */



/**
 * Marks the fragment associated with tag \p tag as
 * a trace head.  The fragment need not exist yet -- once it is
 * created it will be marked as a trace head.
 *
 * DR associates a counter with a trace head and once it
 * passes the -hot_threshold parameter, DR begins building
 * a trace.  Before each fragment is added to the trace, DR
 * calls the client's end_trace callback to determine whether
 * to end the trace.  (The callback will be called both for
 * standard DR traces and for client-defined traces.)
 *
 * \note Some fragments are unsuitable for trace heads. DR will 
 * ignore attempts to mark such fragments as trace heads and will return
 * false. If the client marks a fragment that doesn't exist yet as a trace
 * head and DR later determines that the fragment is unsuitable for
 * a trace head it will unmark the fragment as a trace head without 
 * notifying the client.
 *
 * \note Some fragments' notion of trace heads is dependent on
 * which previous block targets them.  For these fragments, calling
 * this routine will only mark as a trace head for targets from
 * the same memory region.
 *
 * Returns true if the target fragment is marked as a trace head.
 */
bool
dr_mark_trace_head(void *drcontext, void *tag);

/**
 * Checks to see if the fragment (or future fragment) with tag \p tag
 * is marked as a trace head.
 */
bool
dr_trace_head_at(void *drcontext, void *tag);

/** Checks to see that if there is a trace in the code cache at tag \p tag. */
bool
dr_trace_exists_at(void *drcontext, void *tag);

/****************************************************************************
 * BINARY TRACE DUMP FORMAT
 */

/**<pre>
 * Binary trace dump format:
 * the file starts with a tracedump_file_header_t
 * then, for each trace:
     struct _tracedump_trace_header
     if num_bbs > 0 # tracedump_origins
       foreach bb:
           app_pc tag;
           int bb_code_size;
           byte code[bb_code_size];
     endif
     foreach exit:
       struct _tracedump_stub_data
       if linkcount_size > 0
         linkcount_type_t count; # sizeof == linkcount_size
       endif
       if separate from body
       (i.e., exit_stub < cache_start_pc || exit_stub >= cache_start_pc+code_size):
           byte stub_code[15]; # all separate stubs are 15
       endif
     endfor
     byte code[code_size];
 * if the -tracedump_threshold option (deprecated) was specified:
     int num_below_treshold
     linkcount_type_t count_below_threshold
   endif
</pre>
 */
typedef struct _tracedump_file_header_t {
    int version;           /**< The DynamoRIO version that created the file. */
    bool x64;              /**< Whether a 64-bit DynamoRIO library created the file. */
    int linkcount_size;    /**< Size of the linkcount (linkcounts are deprecated). */
} tracedump_file_header_t;

/** Header for an individual trace in a binary trace dump file. */
typedef struct _tracedump_trace_header_t {
    int frag_id;           /**< Identifier for the trace. */
    app_pc tag;            /**< Application address for start of trace. */
    app_pc cache_start_pc; /**< Code cache address of start of trace. */
    int entry_offs;        /**< Offset into trace of normal entry. */
    int num_exits;         /**< Number of exits from the trace. */
    int code_size;         /**< Length of the trace in the code cache. */
    uint num_bbs;          /**< Number of constituent basic blocks making up the trace. */
    bool x64;              /**< Whether the trace contains 64-bit code. */
} tracedump_trace_header_t;

/** Size of tag + bb_code_size fields for each bb. */
#define BB_ORIGIN_HEADER_SIZE (sizeof(app_pc)+sizeof(int))

/**< tracedump_stub_data_t.stub_size will not exceed this value. */
#define SEPARATE_STUB_MAX_SIZE IF_X64_ELSE(23, 15)

/** The format of a stub in a trace dump file. */
typedef struct _tracedump_stub_data {
    int cti_offs;   /**< Offset from the start of the fragment. */
    /* stub_pc is an absolute address, since can be separate from body. */
    app_pc stub_pc; /**< Code cache address of the stub. */
    app_pc target;  /**< Target of the stub. */
    bool linked;    /**< Whether the stub is linked to its target. */
    int stub_size;  /**< Length of stub_code array */
    /****** the rest of the fields are optional and may not be present! ******/
    union {
        uint count32; /**< 32-bit exit execution count. */
        uint64 count64; /**< 64-bit exit execution count. */
    } count; /**< Which field is present depends on the first entry in
              * the file, which indicates the linkcount size. */
    /** Code for exit stubs.  Only present if:
     *   stub_pc < cache_start_pc ||
     *   stub_pc >= cache_start_pc+code_size). 
     * The actual size of the array varies and is indicated by the stub_size field.
     */
    byte stub_code[1/*variable-sized*/];
} tracedump_stub_data_t;

/** The last offset into tracedump_stub_data_t of always-present fields. */
#define STUB_DATA_FIXED_SIZE (offsetof(tracedump_stub_data_t, count))

/****************************************************************************/



#endif /* _DR_TOOLS_H_ */
