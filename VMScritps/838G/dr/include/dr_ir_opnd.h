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

#ifndef _DR_IR_OPND_H_
#define _DR_IR_OPND_H_ 1

/****************************************************************************
 * OPERAND ROUTINES
 */
/**
 * @file dr_ir_opnd.h
 * @brief Functions and defines to create and manipulate instruction operands.
 */

enum {
    REG_NULL, /**< Sentinel value indicating no register, for address modes. */
    /* 64-bit general purpose */
    REG_RAX, /**< The "rax" register. */
    REG_RCX, /**< The "rcx" register. */
    REG_RDX, /**< The "rdx" register. */
    REG_RBX, /**< The "rbx" register. */
    REG_RSP, /**< The "rsp" register. */
    REG_RBP, /**< The "rbp" register. */
    REG_RSI, /**< The "rsi" register. */
    REG_RDI, /**< The "rdi" register. */
    
    REG_R8, /**< The "r8" register. */
    REG_R9, /**< The "r9" register. */
    REG_R10, /**< The "r10" register. */
    REG_R11, /**< The "r11" register. */
    REG_R12, /**< The "r12" register. */
    REG_R13, /**< The "r13" register. */
    REG_R14, /**< The "r14" register. */
    REG_R15, /**< The "r15" register. */
    
    /* 32-bit general purpose */
    REG_EAX, /**< The "eax" register. */
    REG_ECX, /**< The "ecx" register. */
    REG_EDX, /**< The "edx" register. */
    REG_EBX, /**< The "ebx" register. */
    REG_ESP, /**< The "esp" register. */
    REG_EBP, /**< The "ebp" register. */
    REG_ESI, /**< The "esi" register. */
    REG_EDI, /**< The "edi" register. */
    
    REG_R8D, /**< The "r8d" register. */
    REG_R9D, /**< The "r9d" register. */
    REG_R10D, /**< The "r10d" register. */
    REG_R11D, /**< The "r11d" register. */
    REG_R12D, /**< The "r12d" register. */
    REG_R13D, /**< The "r13d" register. */
    REG_R14D, /**< The "r14d" register. */
    REG_R15D, /**< The "r15d" register. */
    
    /* 16-bit general purpose */
    REG_AX, /**< The "ax" register. */
    REG_CX, /**< The "cx" register. */
    REG_DX, /**< The "dx" register. */
    REG_BX, /**< The "bx" register. */
    REG_SP, /**< The "sp" register. */
    REG_BP, /**< The "bp" register. */
    REG_SI, /**< The "si" register. */
    REG_DI, /**< The "di" register. */
    
    REG_R8W, /**< The "r8w" register. */
    REG_R9W, /**< The "r9w" register. */
    REG_R10W, /**< The "r10w" register. */
    REG_R11W, /**< The "r11w" register. */
    REG_R12W, /**< The "r12w" register. */
    REG_R13W, /**< The "r13w" register. */
    REG_R14W, /**< The "r14w" register. */
    REG_R15W, /**< The "r15w" register. */
    
    /* 8-bit general purpose */
    REG_AL, /**< The "al" register. */
    REG_CL, /**< The "cl" register. */
    REG_DL, /**< The "dl" register. */
    REG_BL, /**< The "bl" register. */
    REG_AH, /**< The "ah" register. */
    REG_CH, /**< The "ch" register. */
    REG_DH, /**< The "dh" register. */
    REG_BH, /**< The "bh" register. */
    
    REG_R8L, /**< The "r8l" register. */
    REG_R9L, /**< The "r9l" register. */
    REG_R10L, /**< The "r10l" register. */
    REG_R11L, /**< The "r11l" register. */
    REG_R12L, /**< The "r12l" register. */
    REG_R13L, /**< The "r13l" register. */
    REG_R14L, /**< The "r14l" register. */
    REG_R15L, /**< The "r15l" register. */
    
    REG_SPL, /**< The "spl" register. */
    REG_BPL, /**< The "bpl" register. */
    REG_SIL, /**< The "sil" register. */
    REG_DIL, /**< The "dil" register. */
    
    /* 64-BIT MMX */
    REG_MM0, /**< The "mm0" register. */
    REG_MM1, /**< The "mm1" register. */
    REG_MM2, /**< The "mm2" register. */
    REG_MM3, /**< The "mm3" register. */
    REG_MM4, /**< The "mm4" register. */
    REG_MM5, /**< The "mm5" register. */
    REG_MM6, /**< The "mm6" register. */
    REG_MM7, /**< The "mm7" register. */
    
    /* 128-BIT XMM */
    REG_XMM0, /**< The "xmm0" register. */
    REG_XMM1, /**< The "xmm1" register. */
    REG_XMM2, /**< The "xmm2" register. */
    REG_XMM3, /**< The "xmm3" register. */
    REG_XMM4, /**< The "xmm4" register. */
    REG_XMM5, /**< The "xmm5" register. */
    REG_XMM6, /**< The "xmm6" register. */
    REG_XMM7, /**< The "xmm7" register. */
    
    REG_XMM8, /**< The "xmm8" register. */
    REG_XMM9, /**< The "xmm9" register. */
    REG_XMM10, /**< The "xmm10" register. */
    REG_XMM11, /**< The "xmm11" register. */
    REG_XMM12, /**< The "xmm12" register. */
    REG_XMM13, /**< The "xmm13" register. */
    REG_XMM14, /**< The "xmm14" register. */
    REG_XMM15, /**< The "xmm15" register. */
    
    /* floating point registers */
    REG_ST0, /**< The "st0" register. */
    REG_ST1, /**< The "st1" register. */
    REG_ST2, /**< The "st2" register. */
    REG_ST3, /**< The "st3" register. */
    REG_ST4, /**< The "st4" register. */
    REG_ST5, /**< The "st5" register. */
    REG_ST6, /**< The "st6" register. */
    REG_ST7, /**< The "st7" register. */
    
    /* segments (order from "Sreg" description in Intel manual) */
    SEG_ES, /**< The "es" register. */
    SEG_CS, /**< The "cs" register. */
    SEG_SS, /**< The "ss" register. */
    SEG_DS, /**< The "ds" register. */
    SEG_FS, /**< The "fs" register. */
    SEG_GS, /**< The "gs" register. */
    
    /* debug & control registers (privileged access only; 8-15 for future processors) */
    REG_DR0, /**< The "dr0" register. */
    REG_DR1, /**< The "dr1" register. */
    REG_DR2, /**< The "dr2" register. */
    REG_DR3, /**< The "dr3" register. */
    REG_DR4, /**< The "dr4" register. */
    REG_DR5, /**< The "dr5" register. */
    REG_DR6, /**< The "dr6" register. */
    REG_DR7, /**< The "dr7" register. */
    
    REG_DR8, /**< The "dr8" register. */
    REG_DR9, /**< The "dr9" register. */
    REG_DR10, /**< The "dr10" register. */
    REG_DR11, /**< The "dr11" register. */
    REG_DR12, /**< The "dr12" register. */
    REG_DR13, /**< The "dr13" register. */
    REG_DR14, /**< The "dr14" register. */
    REG_DR15, /**< The "dr15" register. */
    
    /* cr9-cr15 do not yet exist on current x64 hardware */
    REG_CR0, /**< The "cr0" register. */
    REG_CR1, /**< The "cr1" register. */
    REG_CR2, /**< The "cr2" register. */
    REG_CR3, /**< The "cr3" register. */
    REG_CR4, /**< The "cr4" register. */
    REG_CR5, /**< The "cr5" register. */
    REG_CR6, /**< The "cr6" register. */
    REG_CR7, /**< The "cr7" register. */
    
    REG_CR8, /**< The "cr8" register. */
    REG_CR9, /**< The "cr9" register. */
    REG_CR10, /**< The "cr10" register. */
    REG_CR11, /**< The "cr11" register. */
    REG_CR12, /**< The "cr12" register. */
    REG_CR13, /**< The "cr13" register. */
    REG_CR14, /**< The "cr14" register. */
    REG_CR15, /**< The "cr15" register. */
    
    REG_INVALID, /**< Sentinel value indicating an invalid register. */
};
/* we avoid typedef-ing the enum, as its storage size is compiler-specific */
typedef byte reg_id_t; /* contains a REG_ enum value */
typedef byte opnd_size_t; /* contains a REG_ or OPSZ_ enum value */

/* Platform-independent full-register specifiers */
#ifdef X64
# define REG_XAX REG_RAX  /**< Platform-independent way to refer to rax/eax. */
# define REG_XCX REG_RCX  /**< Platform-independent way to refer to rcx/ecx. */
# define REG_XDX REG_RDX  /**< Platform-independent way to refer to rdx/edx. */
# define REG_XBX REG_RBX  /**< Platform-independent way to refer to rbx/ebx. */
# define REG_XSP REG_RSP  /**< Platform-independent way to refer to rsp/esp. */
# define REG_XBP REG_RBP  /**< Platform-independent way to refer to rbp/ebp. */
# define REG_XSI REG_RSI  /**< Platform-independent way to refer to rsi/esi. */
# define REG_XDI REG_RDI  /**< Platform-independent way to refer to rdi/edi. */
#else
# define REG_XAX REG_EAX  /**< Platform-independent way to refer to rax/eax. */
# define REG_XCX REG_ECX  /**< Platform-independent way to refer to rcx/ecx. */
# define REG_XDX REG_EDX  /**< Platform-independent way to refer to rdx/edx. */
# define REG_XBX REG_EBX  /**< Platform-independent way to refer to rbx/ebx. */
# define REG_XSP REG_ESP  /**< Platform-independent way to refer to rsp/esp. */
# define REG_XBP REG_EBP  /**< Platform-independent way to refer to rbp/ebp. */
# define REG_XSI REG_ESI  /**< Platform-independent way to refer to rsi/esi. */
# define REG_XDI REG_EDI  /**< Platform-independent way to refer to rdi/edi. */
#endif



#define REG_START_64      REG_RAX   /**< Start of 64-bit general register enum values. */
#define REG_STOP_64       REG_R15   /**< End of 64-bit general register enum values. */  
#define REG_START_32      REG_EAX   /**< Start of 32-bit general register enum values. */
#define REG_STOP_32       REG_R15D  /**< End of 32-bit general register enum values. */  
#define REG_START_16      REG_AX    /**< Start of 16-bit general register enum values. */
#define REG_STOP_16       REG_R15W  /**< End of 16-bit general register enum values. */  
#define REG_START_8       REG_AL    /**< Start of 8-bit general register enum values. */
#define REG_STOP_8        REG_DIL   /**< End of 8-bit general register enum values. */  
#define REG_START_8HL     REG_AL    /**< Start of 8-bit high-low register enum values. */
#define REG_STOP_8HL      REG_BH    /**< End of 8-bit high-low register enum values. */  
#define REG_START_x86_8   REG_AH    /**< Start of 8-bit x86-only register enum values. */
#define REG_STOP_x86_8    REG_BH    /**< Stop of 8-bit x86-only register enum values. */
#define REG_START_x64_8   REG_SPL   /**< Start of 8-bit x64-only register enum values. */
#define REG_STOP_x64_8    REG_DIL   /**< Stop of 8-bit x64-only register enum values. */
#define REG_START_MMX     REG_MM0   /**< Start of mmx register enum values. */
#define REG_STOP_MMX      REG_MM7   /**< End of mmx register enum values. */  
#define REG_START_XMM     REG_XMM0  /**< Start of xmm register enum values. */
#define REG_STOP_XMM      REG_XMM15 /**< End of xmm register enum values. */  
#define REG_START_FLOAT   REG_ST0   /**< Start of floating-point-register enum values. */
#define REG_STOP_FLOAT    REG_ST7   /**< End of floating-point-register enum values. */  
#define REG_START_SEGMENT SEG_ES    /**< Start of segment register enum values. */
#define REG_STOP_SEGMENT  SEG_GS    /**< End of segment register enum values. */  
#define REG_START_DR      REG_DR0   /**< Start of debug register enum values. */
#define REG_STOP_DR       REG_DR15  /**< End of debug register enum values. */  
#define REG_START_CR      REG_CR0   /**< Start of control register enum values. */
#define REG_STOP_CR       REG_CR15  /**< End of control register enum values. */  
#define REG_LAST_VALID_ENUM REG_CR15  /**< Last valid register enum value. */
#define REG_LAST_ENUM     REG_INVALID /**< Last register enum value. */


/** Returns an empty operand. */
opnd_t 
opnd_create_null(void);

/** Returns a register operand (\p r must be a REG_ constant). */
opnd_t 
opnd_create_reg(reg_id_t r);

/** 
 * Returns an immediate integer operand with value \p i and size
 * \p data_size; \p data_size must be a OPSZ_ constant.
 */
opnd_t 
opnd_create_immed_int(ptr_int_t i, opnd_size_t data_size);

/** Returns an immediate float operand with value \p f. */
opnd_t 
opnd_create_immed_float(float f);

/** Returns a program address operand with value \p pc. */
opnd_t 
opnd_create_pc(app_pc pc);

/**
 * Returns a far program address operand with value \p seg_selector:pc.
 * \p seg_selector is a segment selector, not a SEG_ constant.
 */
opnd_t 
opnd_create_far_pc(ushort seg_selector, app_pc pc);

/** Returns an instr_t pointer address with value \p instr. */
opnd_t 
opnd_create_instr(instr_t *instr);

/**
 * Returns a far instr_t pointer address with value \p seg_selector:instr.
 * \p seg_selector is a segment selector, not a SEG_ constant.
 */
opnd_t 
opnd_create_far_instr(ushort seg_selector, instr_t *instr);

/** 
 * Returns a memory reference operand that refers to the address:
 * - disp(base_reg, index_reg, scale)
 *
 * or, in other words,
 * - base_reg + index_reg*scale + disp
 *
 * The operand has data size data_size (must be a OPSZ_ constant).
 * Both \p base_reg and \p index_reg must be REG_ constants.
 * \p scale must be either 1, 2, 4, or 8.
 */
opnd_t 
opnd_create_base_disp(reg_id_t base_reg, reg_id_t index_reg, int scale, int disp,
                      opnd_size_t data_size);

/** 
 * Returns a memory reference operand that refers to the address:
 * - disp(base_reg, index_reg, scale)
 *
 * or, in other words,
 * - base_reg + index_reg*scale + disp
 *
 * The operand has data size \p data_size (must be a OPSZ_ constant).
 * Both \p base_reg and \p index_reg must be REG_ constants.
 * \p scale must be either 1, 2, 4, or 8.
 * Gives control over encoding optimizations:
 * -# If \p encode_zero_disp, a zero value for disp will not be omitted;
 * -# If \p force_full_disp, a small value for disp will not occupy only one byte.
 * -# If \p disp_short_addr, short (16-bit for 32-bit mode, 32-bit for
 *    64-bit mode) addressing will be used (note that this normally only
 *    needs to be specified for an absolute address; otherwise, simply
 *    use the desired short registers for base and/or index).
 *
 * (Both of those are false when using opnd_create_base_disp()).
 */
opnd_t
opnd_create_base_disp_ex(reg_id_t base_reg, reg_id_t index_reg, int scale,
                         int disp, opnd_size_t size,
                         bool encode_zero_disp, bool force_full_disp,
                         bool disp_short_addr);

/** 
 * Returns a far memory reference operand that refers to the address:
 * - seg : disp(base_reg, index_reg, scale)
 *
 * or, in other words,
 * - seg : base_reg + index_reg*scale + disp
 *
 * The operand has data size \p data_size (must be a OPSZ_ constant).
 * \p seg must be a SEG_ constant.
 * Both \p base_reg and \p index_reg must be REG_ constants.
 * \p scale must be either 1, 2, 4, or 8.
 */
opnd_t 
opnd_create_far_base_disp(reg_id_t seg, reg_id_t base_reg, reg_id_t index_reg, int scale,
                          int disp, opnd_size_t data_size);

/** 
 * Returns a far memory reference operand that refers to the address:
 * - seg : disp(base_reg, index_reg, scale)
 *
 * or, in other words,
 * - seg : base_reg + index_reg*scale + disp
 *
 * The operand has data size \p data_size (must be a OPSZ_ constant).
 * \p seg must be a SEG_ constant.
 * Both \p base_reg and \p index_reg must be REG_ constants.
 * scale must be either 1, 2, 4, or 8.
 * Gives control over encoding optimizations:
 * -# If \p encode_zero_disp, a zero value for disp will not be omitted;
 * -# If \p force_full_disp, a small value for disp will not occupy only one byte.
 * -# If \p disp_short_addr, short (16-bit for 32-bit mode, 32-bit for
 *    64-bit mode) addressing will be used (note that this normally only
 *    needs to be specified for an absolute address; otherwise, simply
 *    use the desired short registers for base and/or index).
 *
 * (Both of those are false when using opnd_create_far_base_disp()).
 */
opnd_t
opnd_create_far_base_disp_ex(reg_id_t seg, reg_id_t base_reg, reg_id_t index_reg,
                             int scale, int disp, opnd_size_t size,
                             bool encode_zero_disp, bool force_full_disp,
                             bool disp_short_addr);

/**
 * Returns a memory reference operand that refers to the address \p addr.
 * The operand has data size \p data_size (must be a OPSZ_ constant).
 *
 * If \p addr <= 2^32 (which is always true in 32-bit mode), this routine
 * is equivalent to
 * opnd_create_base_disp(REG_NULL, REG_NULL, 0, (int)addr, data_size).
 *
 * Otherwise, this routine creates a separate operand type with an
 * absolute 64-bit memory address.  Note that such an operand can only be
 * used as a load or store from or to the rax register.
 */
opnd_t 
opnd_create_abs_addr(void *addr, opnd_size_t data_size);

/**
 * Returns a memory reference operand that refers to the address
 * \p seg: \p addr.
 * The operand has data size \p data_size (must be a OPSZ_ constant).
 *
 * If \p addr <= 2^32 (which is always true in 32-bit mode), this routine
 * is equivalent to
 * opnd_create_far_base_disp(seg, REG_NULL, REG_NULL, 0, (int)addr, data_size).
 *
 * Otherwise, this routine creates a separate operand type with an
 * absolute 64-bit memory address.  Note that such an operand can only be
 * used as a load or store from or to the rax register.
 */
opnd_t 
opnd_create_far_abs_addr(reg_id_t seg, void *addr, opnd_size_t data_size);
#ifdef X64


/**
 * Returns a memory reference operand that refers to the address \p
 * addr, but will be encoded as a pc-relative address.  At emit time,
 * if \p addr is out of reach of a 32-bit signed displacement from the
 * next instruction, encoding will fail.
 *
 * DR guarantees that all of its code caches and heap are within the
 * same 2GB memory region.  DR also loads client libraries within
 * 32-bit reachability of its code caches and heap.  This means that
 * any static data or code in a client library, or any data allocated
 * using DR's API, is guaranteed to be reachable from code cache code.
 *
 * The operand has data size data_size (must be a OPSZ_ constant).
 *
 * To represent a 32-bit address (i.e., what an address size prefix
 * indicates), simply zero out the top 32 bits of the address before
 * passing it to this routine.
 *
 * \note For 64-bit DR builds only.
 */
opnd_t 
opnd_create_rel_addr(void *addr, opnd_size_t data_size);

/**
 * Returns a memory reference operand that refers to the address \p
 * seg : \p addr, but will be encoded as a pc-relative address.  It is
 * up to the caller to ensure that the resulting address is reachable
 * via a 32-bit signed displacement from the next instruction at emit
 * time.
 *
 * DR guarantees that all of its code caches and heap are within the
 * same 2GB memory region.  DR also loads client libraries within
 * 32-bit reachability of its code caches and heap.  This means that
 * any static data or code in a client library, or any data allocated
 * using DR's API, is guaranteed to be reachable from code cache code.
 *
 * The operand has data size \p data_size (must be a OPSZ_ constant).
 *
 * To represent a 32-bit address (i.e., what an address size prefix
 * indicates), simply zero out the top 32 bits of the address before
 * passing it to this routine.
 *
 * \note For 64-bit DR builds only.
 */
opnd_t 
opnd_create_far_rel_addr(reg_id_t seg, void *addr, opnd_size_t data_size);
#endif


/** Returns true iff \p opnd is an empty operand. */
bool 
opnd_is_null(opnd_t opnd);

/** Returns true iff \p opnd is a register operand. */
bool 
opnd_is_reg(opnd_t opnd);

/** Returns true iff \p opnd is an immediate (integer or float) operand. */
bool 
opnd_is_immed(opnd_t opnd);

/** Returns true iff \p opnd is an immediate integer operand. */
bool 
opnd_is_immed_int(opnd_t opnd);

/** Returns true iff \p opnd is an immediate float operand. */
bool 
opnd_is_immed_float(opnd_t opnd);

/** Returns true iff \p opnd is a (near or far) program address operand. */
bool 
opnd_is_pc(opnd_t opnd);

/** Returns true iff \p opnd is a near (i.e., default segment) program address operand. */
bool 
opnd_is_near_pc(opnd_t opnd);

/** Returns true iff \p opnd is a far program address operand. */
bool 
opnd_is_far_pc(opnd_t opnd);

/** Returns true iff \p opnd is a (near or far) instr_t pointer address operand. */
bool 
opnd_is_instr(opnd_t opnd);

/** Returns true iff \p opnd is a near instr_t pointer address operand. */
bool 
opnd_is_near_instr(opnd_t opnd);

/** Returns true iff \p opnd is a far instr_t pointer address operand. */
bool 
opnd_is_far_instr(opnd_t opnd);

/** Returns true iff \p opnd is a (near or far) base+disp memory reference operand. */
bool 
opnd_is_base_disp(opnd_t opnd);

/**
 * Returns true iff \p opnd is a near (i.e., default segment) base+disp memory
 * reference operand.
 */
bool 
opnd_is_near_base_disp(opnd_t opnd);

/** Returns true iff \p opnd is a far base+disp memory reference operand. */
bool 
opnd_is_far_base_disp(opnd_t opnd);

/** 
 * Returns true iff \p opnd is a (near or far) absolute address operand.
 * Returns true for both base-disp operands with no base or index and
 * 64-bit non-base-disp absolute address operands. 
 */
bool 
opnd_is_abs_addr(opnd_t opnd);

/** 
 * Returns true iff \p opnd is a near (i.e., default segment) absolute address operand.
 * Returns true for both base-disp operands with no base or index and
 * 64-bit non-base-disp absolute address operands. 
 */
bool 
opnd_is_near_abs_addr(opnd_t opnd);

/** 
 * Returns true iff \p opnd is a far absolute address operand.
 * Returns true for both base-disp operands with no base or index and
 * 64-bit non-base-disp absolute address operands. 
 */
bool 
opnd_is_far_abs_addr(opnd_t opnd);
#ifdef X64


/**
 * Returns true iff \p opnd is a (near or far) pc-relative memory reference operand. 
 *
 * \note For 64-bit DR builds only.
 */
bool 
opnd_is_rel_addr(opnd_t opnd);

/**
 * Returns true iff \p opnd is a near (i.e., default segment) pc-relative memory
 * reference operand. 
 *
 * \note For 64-bit DR builds only.
 */
bool 
opnd_is_near_rel_addr(opnd_t opnd);

/**
 * Returns true iff \p opnd is a far pc-relative memory reference operand. 
 *
 * \note For 64-bit DR builds only.
 */
bool 
opnd_is_far_rel_addr(opnd_t opnd);
#endif


/**
 * Returns true iff \p opnd is a (near or far) memory reference operand
 * of any type: base-disp, absolute address, or pc-relative address.
 *
 * \note For 64-bit DR builds only.
 */
bool 
opnd_is_memory_reference(opnd_t opnd);

/**
 * Returns true iff \p opnd is a far memory reference operand
 * of any type: base-disp, absolute address, or pc-relative address.
 */
bool
opnd_is_far_memory_reference(opnd_t opnd);

/**
 * Returns true iff \p opnd is a near memory reference operand
 * of any type: base-disp, absolute address, or pc-relative address.
 */
bool
opnd_is_near_memory_reference(opnd_t opnd);

/** 
 * Return the data size of \p opnd as a OPSZ_ constant.
 * Assumes \p opnd is a register, immediate integer, or memory reference.
 * If \p opnd is a register returns the result of opnd_reg_get_size()
 * called on the REG_ constant.
 * Returns OPSZ_NA if \p opnd does not have a valid size.
 */
opnd_size_t
opnd_get_size(opnd_t opnd);

/** 
 * Sets the data size of \p opnd.
 * Assumes \p opnd is an immediate integer or a memory reference.
 */
void   
opnd_set_size(opnd_t *opnd, opnd_size_t newsize);

/** 
 * Assumes \p opnd is a register operand.
 * Returns the register it refers to (a REG_ constant).
 */
reg_id_t  
opnd_get_reg(opnd_t opnd);

/* Assumes opnd is an immediate integer, returns its value. */
ptr_int_t
opnd_get_immed_int(opnd_t opnd);

/** Assumes \p opnd is an immediate float, returns its value. */
float  
opnd_get_immed_float(opnd_t opnd);

/** Assumes \p opnd is a (near or far) program address, returns its value. */
app_pc 
opnd_get_pc(opnd_t opnd);

/** 
 * Assumes \p opnd is a far program address.
 * Returns \p opnd's segment, a segment selector (not a SEG_ constant).
 */
ushort    
opnd_get_segment_selector(opnd_t opnd);

/** Assumes \p opnd is an instr_t pointer, returns its value. */
instr_t*
opnd_get_instr(opnd_t opnd);

/**
 * Assumes \p opnd is a (near or far) base+disp memory reference.  Returns the base
 * register (a REG_ constant).
 */
reg_id_t
opnd_get_base(opnd_t opnd);

/**
 * Assumes \p opnd is a (near or far) base+disp memory reference.
 * Returns the displacement. 
 */
int 
opnd_get_disp(opnd_t opnd);

/** 
 * Assumes \p opnd is a (near or far) base+disp memory reference; returns whether
 * encode_zero_disp has been specified for \p opnd.
 */
bool
opnd_is_disp_encode_zero(opnd_t opnd);

/** 
 * Assumes \p opnd is a (near or far) base+disp memory reference; returns whether
 * force_full_disp has been specified for \p opnd.
 */
bool
opnd_is_disp_force_full(opnd_t opnd);

/** 
 * Assumes \p opnd is a (near or far) base+disp memory reference; returns whether
 * disp_short_addr has been specified for \p opnd.
 */
bool
opnd_is_disp_short_addr(opnd_t opnd);

/** 
 * Assumes \p opnd is a (near or far) base+disp memory reference.
 * Returns the index register (a REG_ constant).
 */
reg_id_t
opnd_get_index(opnd_t opnd);

/** Assumes \p opnd is a (near or far) base+disp memory reference.  Returns the scale. */
int 
opnd_get_scale(opnd_t opnd);

/** 
 * Assumes \p opnd is a (near or far) memory reference of any type.
 * Returns \p opnd's segment (a SEG_ constant), or REG_NULL if it is a near
 * memory reference.
 */
reg_id_t    
opnd_get_segment(opnd_t opnd);

/** 
 * Assumes \p opnd is a (near or far) absolute or pc-relative memory reference,
 * or a base+disp memory reference with no base or index register.
 * Returns \p opnd's absolute address (which will be pc-relativized on encoding
 * for pc-relative memory references).
 */
void *
opnd_get_addr(opnd_t opnd);

/** 
 * Returns the number of registers referred to by \p opnd. This will only
 * be non-zero for register operands and memory references.
 */
int 
opnd_num_regs_used(opnd_t opnd);

/** 
 * Used in conjunction with opnd_num_regs_used(), this routine can be used
 * to iterate through all registers used by \p opnd.
 * The index values begin with 0 and proceed through opnd_num_regs_used(opnd)-1.
 */
reg_id_t
opnd_get_reg_used(opnd_t opnd, int index);

/** 
 * Assumes that \p reg is a REG_ 32-bit register constant.
 * Returns the string name for \p reg.
 */
const char *
get_register_name(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ 32-bit register constant.
 * Returns the 16-bit version of \p reg.
 */
reg_id_t
reg_32_to_16(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ 32-bit register constant.
 * Returns the 8-bit version of \p reg (the least significant byte:
 * REG_AL instead of REG_AH if passed REG_EAX, e.g.).  For 32-bit DR
 * builds, returns REG_NULL if passed REG_ESP, REG_EBP, REG_ESI, or
 * REG_EDI.
 */
reg_id_t
reg_32_to_8(reg_id_t reg);
#ifdef X64


/** 
 * Assumes that \p reg is a REG_ 32-bit register constant.
 * Returns the 64-bit version of \p reg.
 *
 * \note For 64-bit DR builds only.
 */
reg_id_t
reg_32_to_64(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ 64-bit register constant.
 * Returns the 32-bit version of \p reg.
 *
 * \note For 64-bit DR builds only.
 */
reg_id_t
reg_64_to_32(reg_id_t reg);

/** 
 * Returns true iff \p reg refers to an extended register only available
 * in 64-bit mode and not in 32-bit mode (e.g., R8-R15, XMM8-XMM15, etc.)
 *
 * \note For 64-bit DR builds only.
 */
bool 
reg_is_extended(reg_id_t reg);
#endif


/** 
 * Assumes that \p reg is a REG_ 32-bit register constant.
 * If \p sz == OPSZ_2, returns the 16-bit version of \p reg.
 * For 64-bit versions of this library, if \p sz == OPSZ_8, returns 
 * the 64-bit version of \p reg.
 */
reg_id_t
reg_32_to_opsz(reg_id_t reg, opnd_size_t sz);

/**  
 * Assumes that \p reg is a REG_ register constant.
 * If reg is used as part of the calling convention, returns which
 * parameter ordinal it matches (0-based); otherwise, returns -1.
 */
int
reg_parameter_num(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns true iff it refers to a General Purpose Register,
 * i.e., rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi, or a subset.
 */
bool
reg_is_gpr(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns true iff it refers to a segment (i.e., it's really a SEG_
 * constant).
 */
bool
reg_is_segment(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns true iff it refers to an xmm (128-bit SSE/SSE2) register.
 */
bool 
reg_is_xmm(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns true iff it refers to an mmx (64-bit) register.
 */
bool 
reg_is_mmx(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns true iff it refers to a floating-point register.
 */
bool 
reg_is_fp(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns true iff it refers to a 32-bit general-purpose register.
 */
bool 
reg_is_32bit(reg_id_t reg);

/** 
 * Returns true iff \p opnd is a register operand that refers to a 32-bit
 * general-purpose register.
 */
bool 
opnd_is_reg_32bit(opnd_t opnd);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns true iff it refers to a 64-bit general-purpose register.
 */
bool 
reg_is_64bit(reg_id_t reg);

/** 
 * Returns true iff \p opnd is a register operand that refers to a 64-bit
 * general-purpose register.
 */
bool 
opnd_is_reg_64bit(opnd_t opnd);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns true iff it refers to a pointer-sized general-purpose register.
 */
bool 
reg_is_pointer_sized(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ 32-bit register constant.
 * Returns the pointer-sized version of \p reg.
 */
reg_id_t
reg_to_pointer_sized(reg_id_t reg);

/** 
 * Returns true iff \p opnd is a register operand that refers to a
 * pointer-sized general-purpose register.
 */
bool 
opnd_is_reg_pointer_sized(opnd_t opnd);

/** 
 * Assumes that \p r1 and \p r2 are both REG_ constants.
 * Returns true iff \p r1's register overlaps \p r2's register
 * (e.g., if \p r1 == REG_AX and \p r2 == REG_EAX).
 */
bool 
reg_overlap(reg_id_t r1, reg_id_t r2);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns \p reg's representation as 3 bits in a modrm byte
 * (the 3 bits are the lower-order bits in the return value).
 */
byte
reg_get_bits(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns the OPSZ_ constant corresponding to the register size.
 * Returns OPSZ_NA if reg is not a REG_ constant.
 */
opnd_size_t 
reg_get_size(reg_id_t reg);

/** 
 * Assumes that \p reg is a REG_ constant.
 * Returns true iff \p opnd refers to reg directly or refers to a register
 * that overlaps \p reg (e.g., REG_AX overlaps REG_EAX).
 */
bool 
opnd_uses_reg(opnd_t opnd, reg_id_t reg);

/** Set the displacement of a memory reference operand \p opnd to \p disp. */
void
opnd_set_disp(opnd_t *opnd, int disp);

/** 
 * Set the displacement and encoding controls of a memory reference operand:
 * - If \p encode_zero_disp, a zero value for \p disp will not be omitted;
 * - If \p force_full_disp, a small value for \p disp will not occupy only one byte.
 * -# If \p disp_short_addr, short (16-bit for 32-bit mode, 32-bit for
 *    64-bit mode) addressing will be used (note that this normally only
 *    needs to be specified for an absolute address; otherwise, simply
 *    use the desired short registers for base and/or index).
 */
void
opnd_set_disp_ex(opnd_t *opnd, int disp, bool encode_zero_disp, bool force_full_disp,
                 bool disp_short_addr);

/** 
 * Assumes that both \p old_reg and \p new_reg are REG_ constants.
 * Replaces all occurrences of \p old_reg in \p *opnd with \p new_reg.
 */
bool 
opnd_replace_reg(opnd_t *opnd, reg_id_t old_reg, reg_id_t new_reg);

/** Returns true iff \p op1 and \p op2 are indistinguishable. 
 *  If either uses variable operand sizes, the default size is assumed.
 */
bool 
opnd_same(opnd_t op1,opnd_t op2);

/** 
 * Returns true iff \p op1 and \p op2 are both memory references and they
 * are indistinguishable, ignoring data size.
 */
bool 
opnd_same_address(opnd_t op1,opnd_t op2);

/** 
 * Returns true iff there exists some register that is referred to (directly
 * or overlapping) by both \p op1 and \p op2.
 */
bool 
opnd_share_reg(opnd_t op1, opnd_t op2);

/** 
 * Returns true iff \p def, considered as a write, affects \p use.
 * Is conservative, so if both \p def and \p use are memory references,
 * will return true unless it can disambiguate them based on their
 * registers and displacement.
 */
bool 
opnd_defines_use(opnd_t def, opnd_t use);

/** 
 * Assumes \p size is a OPSZ_ or a REG_ constant.
 * If \p size is a REG_ constant, first calls reg_get_size(\p size)
 * to get a OPSZ_ constant.
 * Returns the number of bytes the OPSZ_ constant represents.
 */
uint 
opnd_size_in_bytes(opnd_size_t size);

/** 
 * Shrinks all 32-bit registers in \p opnd to their 16-bit versions.  
 * Also shrinks the size of immediate integers and memory references from
 * OPSZ_4 to OPSZ_2.
 */
opnd_t 
opnd_shrink_to_16_bits(opnd_t opnd);
#ifdef X64


/** 
 * Shrinks all 64-bit registers in \p opnd to their 32-bit versions.  
 * Also shrinks the size of immediate integers and memory references from
 * OPSZ_8 to OPSZ_4.
 *
 * \note For 64-bit DR builds only.
 */
opnd_t 
opnd_shrink_to_32_bits(opnd_t opnd);
#endif


/** 
 * Returns the value of the register \p reg, selected from the passed-in
 * register values.
 */
reg_t
reg_get_value(reg_id_t reg, dr_mcontext_t *mc);

/**
 * Sets the register \p reg in the passed in mcontext \p mc to \p value.
 * \note Current release is limited to setting pointer-sized registers only
 * (no sub-registers).
 */
void
reg_set_value(reg_id_t reg, dr_mcontext_t *mc, reg_t value);

/** 
 * Returns the effective address of \p opnd, computed using the passed-in 
 * register values.  If \p opnd is a far address, ignores that aspect
 * except for TLS references on Windows (fs: for 32-bit, gs: for 64-bit)
 * or typical fs: or gs: references on Linux.  For far addresses the
 * calling thread's segment selector is used.
 */
app_pc
opnd_compute_address(opnd_t opnd, dr_mcontext_t *mc);

/* Memory operand sizes (with Intel's corresponding size names noted).
 * For register operands, the REG_ constants are used, which implicitly
 * state a size (e.g., REG_CX is 2 bytes).
 * Use the type opnd_size_t for these values (we avoid typedef-ing the
 * enum, as its storage size is compiler-specific).  opnd_size_t is a
 * byte, so the largest value here needs to be <= 255.
 */
enum {
    /* register enum values are used for TYPE_*REG */
    OPSZ_NA = REG_LAST_ENUM+1, /**< Sentinel value: not a valid size. */ /* = 139 */
    OPSZ_0,  /**< Intel 'm': "sizeless": used for both start addresses
              * (lea, invlpg) and implicit constants (rol, fldl2e, etc.) */
    OPSZ_1,  /**< Intel 'b': 1 byte */
    OPSZ_2,  /**< Intel 'w': 2 bytes */
    OPSZ_4,  /**< Intel 'd','si': 4 bytes */
    OPSZ_6,  /**< Intel 'p','s': 6 bytes */
    OPSZ_8,  /**< Intel 'q','pi': 8 bytes */
    OPSZ_10, /**< Intel 's' 64-bit, or double extended precision floating point
              * (latter used by fld, fstp, fbld, fbstp) */
    OPSZ_16, /**< Intel 'dq','ps','pd','ss','sd': 16 bytes */
    OPSZ_14, /**< FPU operating environment with short data size (fldenv, fnstenv) */
    OPSZ_28, /**< FPU operating environment with normal data size (fldenv, fnstenv) */
    OPSZ_94,  /**< FPU state with short data size (fnsave, frstor) */
    OPSZ_108, /**< FPU state with normal data size (fnsave, frstor) */
    OPSZ_512, /**< FPU, MMX, XMM state (fxsave, fxrstor) */
    /**
     * The following sizes (OPSZ_*_short*) vary according to the cs segment and the
     * operand size prefix.  This IR assumes that the cs segment is set to the
     * default operand size.  The operand size prefix then functions to shrink the
     * size.  The IR does not explicitly mark the prefix; rather, a shortened size is
     * requested in the operands themselves, with the IR adding the prefix at encode
     * time.  Normally the fixed sizes above should be used rather than these
     * variable sizes, which are used internally by the IR and should only be
     * externally specified when building an operand in order to be flexible and
     * allow other operands to decide the size for the instruction (the prefix
     * applies to the entire instruction).
     */
    OPSZ_2_short1, /**< Intel 'c': 2/1 bytes ("2/1" means 2 bytes normally, but if
                    * another operand requests a short size then this size can
                    * accommodate by shifting to its short size, which is 1 byte). */
    OPSZ_4_short2, /**< Intel 'z': 4/2 bytes */
    OPSZ_4_rex8_short2, /**< Intel 'v': 8/4/2 bytes */
    OPSZ_4_rex8,   /**< Intel 'd/q' (like 'v' but never 2 bytes). */
    OPSZ_6_irex10_short4, /**< Intel 'p': On Intel processors this is 10/6/4 bytes for
                           * segment selector + address.  On AMD processors this is
                           * 6/4 bytes for segment selector + address (rex is ignored). */
    OPSZ_8_short2, /**< partially resolved 4x8_short2 */
    OPSZ_8_short4, /**< Intel 'a': pair of 4_short2 (bound) */
    OPSZ_28_short14, /**< FPU operating env variable data size (fldenv, fnstenv) */
    OPSZ_108_short94, /**< FPU state with variable data size (fnsave, frstor) */
    /** Varies by 32-bit versus 64-bit processor mode. */
    OPSZ_4x8,  /**< Full register size with no variation by prefix.
                *   Used for control and debug register moves. */
    OPSZ_6x10, /**< Intel 's': 6-byte (10-byte for 64-bit mode) table base + limit */
    /** 
     * Stack operands not only vary by operand size specifications but also
     * by 32-bit versus 64-bit processor mode.
     */
    OPSZ_4x8_short2, /**< Intel 'v'/'d64' for stack operations.
                      * Also 64-bit address-size specified operands, which are
                      * short4 rather than short2 in 64-bit mode (but short2 in
                      * 32-bit mode).
                      * Note that this IR does not distinguish multiple stack
                      * operations; dispatch by opcode must be used:
                      *   X2 = far call/far ret
                      *   X3 = int/iret
                      *   X8 = pusha/popa
                      *   X* = enter (dynamically varying amount)
                      * Note that stack operations may also modify the stack
                      * pointer prior to accessing the top of the stack, so
                      * for example "(esp)" may in fact be "4(esp)" depending
                      * on the opcode.
                      */
    OPSZ_4x8_short2xi8, /**< Intel 'f64': 4_short2 for 32-bit, 8_short2 for 64-bit AMD,
                         *   always 8 for 64-bit Intel */
    OPSZ_4_short2xi4,   /**< Intel 'f64': 4_short2 for 32-bit or 64-bit AMD,
                         *   always 4 for 64-bit Intel */
    /**
     * The following sizes differ based on whether the modrm chooses a
     * register or memory.
     */
    OPSZ_1_reg4,  /**< Intel Rd/Mb: zero-extends if reg; used by pextrb */
    OPSZ_2_reg4,  /**< Intel Rd/Mw: zero-extends if reg; used by pextrw */
    OPSZ_4_reg16, /**< Intel Udq/Md: sub-xmm but we consider that whole xmm;
                   *   used by insertps. */
    OPSZ_LAST,
};

#ifdef X64
# define OPSZ_PTR OPSZ_8       /**< Operand size for pointer values. */
# define OPSZ_STACK OPSZ_8     /**< Operand size for stack push/pop operand sizes. */
#else
# define OPSZ_PTR OPSZ_4       /**< Operand size for pointer values. */
# define OPSZ_STACK OPSZ_4     /**< Operand size for stack push/pop operand sizes. */
#endif
#define OPSZ_VARSTACK OPSZ_4x8_short2 /**< Operand size for prefix-varying stack
                                       * push/pop operand sizes. */
#define OPSZ_REXVARSTACK OPSZ_4_rex8_short2 /* Operand size for prefix/rex-varying
                                             * stack push/pop like operand sizes. */

#define OPSZ_ret OPSZ_4x8_short2xi8 /**< Operand size for ret instruction. */
#define OPSZ_call OPSZ_ret         /**< Operand size for push portion of call. */

/* Convenience defines for specific opcodes */
#define OPSZ_lea OPSZ_0              /**< Operand size for lea memory reference. */
#define OPSZ_invlpg OPSZ_0           /**< Operand size for invlpg memory reference. */
#define OPSZ_xlat OPSZ_1             /**< Operand size for xlat memory reference. */
#define OPSZ_clflush OPSZ_1          /**< Operand size for clflush memory reference. */
#define OPSZ_prefetch OPSZ_1         /**< Operand size for prefetch memory references. */
#define OPSZ_lgdt OPSZ_6x10          /**< Operand size for lgdt memory reference. */
#define OPSZ_sgdt OPSZ_6x10          /**< Operand size for sgdt memory reference. */
#define OPSZ_lidt OPSZ_6x10          /**< Operand size for lidt memory reference. */
#define OPSZ_sidt OPSZ_6x10          /**< Operand size for sidt memory reference. */
#define OPSZ_bound OPSZ_8_short4     /**< Operand size for bound memory reference. */
#define OPSZ_maskmovq OPSZ_8         /**< Operand size for maskmovq memory reference. */
#define OPSZ_maskmovdqu OPSZ_16      /**< Operand size for maskmovdqu memory reference. */
#define OPSZ_fldenv OPSZ_28_short14  /**< Operand size for fldenv memory reference. */
#define OPSZ_fnstenv OPSZ_28_short14 /**< Operand size for fnstenv memory reference. */
#define OPSZ_fnsave OPSZ_108_short94 /**< Operand size for fnsave memory reference. */
#define OPSZ_frstor OPSZ_108_short94 /**< Operand size for frstor memory reference. */
#define OPSZ_fxsave OPSZ_512         /**< Operand size for fxsave memory reference. */
#define OPSZ_fxrstor OPSZ_512        /**< Operand size for fxrstor memory reference. */


/**
 * Prints the operand \p opnd to file \p outfile.
 * The default is to use AT&T-style syntax, unless the \ref op_syntax_intel
 * "-syntax_intel" runtime option is specified.
 */
void 
opnd_disassemble(void *drcontext, opnd_t opnd, file_t outfile);


#endif /* _DR_IR_OPND_H_ */
