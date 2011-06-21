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

#ifndef _DR_IR_OPCODES_H_
#define _DR_IR_OPCODES_H_ 1


/****************************************************************************
 * OPCODES
 */
/**
 * @file dr_ir_opcodes.h
 * @brief Instruction opcode constants.
 */
/** Opcode constants for use in the instr_t data structure. */
enum {
/*   0 */     OP_INVALID, /**< Indicates an invalid instr_t. */
/*   1 */     OP_UNDECODED, /**< Indicates an undecoded instr_t. */
/*   2 */     OP_CONTD, /**< Used internally only. */
/*   3 */     OP_LABEL, /**< A label is used for instr_t branch targets. */

/*   4 */     OP_add, /**< add opcode */
/*   5 */     OP_or, /**< or opcode */
/*   6 */     OP_adc, /**< adc opcode */
/*   7 */     OP_sbb, /**< sbb opcode */
/*   8 */     OP_and, /**< and opcode */
/*   9 */     OP_daa, /**< daa opcode */
/*  10 */     OP_sub, /**< sub opcode */
/*  11 */     OP_das, /**< das opcode */
/*  12 */     OP_xor, /**< xor opcode */
/*  13 */     OP_aaa, /**< aaa opcode */
/*  14 */     OP_cmp, /**< cmp opcode */
/*  15 */     OP_aas, /**< aas opcode */
/*  16 */     OP_inc, /**< inc opcode */
/*  17 */     OP_dec, /**< dec opcode */
/*  18 */     OP_push, /**< push opcode */
/*  19 */     OP_push_imm, /**< push_imm opcode */
/*  20 */     OP_pop, /**< pop opcode */
/*  21 */     OP_pusha, /**< pusha opcode */
/*  22 */     OP_popa, /**< popa opcode */
/*  23 */     OP_bound, /**< bound opcode */
/*  24 */     OP_arpl, /**< arpl opcode */
/*  25 */     OP_imul, /**< imul opcode */

/*  26 */     OP_jo_short, /**< jo_short opcode */
/*  27 */     OP_jno_short, /**< jno_short opcode */
/*  28 */     OP_jb_short, /**< jb_short opcode */
/*  29 */     OP_jnb_short, /**< jnb_short opcode */
/*  30 */     OP_jz_short, /**< jz_short opcode */
/*  31 */     OP_jnz_short, /**< jnz_short opcode */
/*  32 */     OP_jbe_short, /**< jbe_short opcode */
/*  33 */     OP_jnbe_short, /**< jnbe_short opcode */
/*  34 */     OP_js_short, /**< js_short opcode */
/*  35 */     OP_jns_short, /**< jns_short opcode */
/*  36 */     OP_jp_short, /**< jp_short opcode */
/*  37 */     OP_jnp_short, /**< jnp_short opcode */
/*  38 */     OP_jl_short, /**< jl_short opcode */
/*  39 */     OP_jnl_short, /**< jnl_short opcode */
/*  40 */     OP_jle_short, /**< jle_short opcode */
/*  41 */     OP_jnle_short, /**< jnle_short opcode */

/*  42 */     OP_call, /**< call opcode */
/*  43 */     OP_call_ind, /**< call_ind opcode */
/*  44 */     OP_call_far, /**< call_far opcode */
/*  45 */     OP_call_far_ind, /**< call_far_ind opcode */
/*  46 */     OP_jmp, /**< jmp opcode */
/*  47 */     OP_jmp_short, /**< jmp_short opcode */
/*  48 */     OP_jmp_ind, /**< jmp_ind opcode */
/*  49 */     OP_jmp_far, /**< jmp_far opcode */
/*  50 */     OP_jmp_far_ind, /**< jmp_far_ind opcode */

/*  51 */     OP_loopne, /**< loopne opcode */
/*  52 */     OP_loope, /**< loope opcode */
/*  53 */     OP_loop, /**< loop opcode */
/*  54 */     OP_jecxz, /**< jecxz opcode */

    /* point ld & st at eAX & al instrs, they save 1 byte (no modrm),
     * hopefully time taken considering them doesn't offset that */
/*  55 */     OP_mov_ld, /**< mov_ld opcode */
/*  56 */     OP_mov_st, /**< mov_st opcode */
    /* note that store of immed is mov_st not mov_imm, even though can be immed->reg,
     * which we address by sharing part of the mov_st template chain */
/*  57 */     OP_mov_imm, /**< mov_imm opcode */
/*  58 */     OP_mov_seg, /**< mov_seg opcode */
/*  59 */     OP_mov_priv, /**< mov_priv opcode */

/*  60 */     OP_test, /**< test opcode */
/*  61 */     OP_lea, /**< lea opcode */
/*  62 */     OP_xchg, /**< xchg opcode */
/*  63 */     OP_cwde, /**< cwde opcode */
/*  64 */     OP_cdq, /**< cdq opcode */
/*  65 */     OP_fwait, /**< fwait opcode */
/*  66 */     OP_pushf, /**< pushf opcode */
/*  67 */     OP_popf, /**< popf opcode */
/*  68 */     OP_sahf, /**< sahf opcode */
/*  69 */     OP_lahf, /**< lahf opcode */

/*  70 */     OP_ret, /**< ret opcode */
/*  71 */     OP_ret_far, /**< ret_far opcode */

/*  72 */     OP_les, /**< les opcode */
/*  73 */     OP_lds, /**< lds opcode */
/*  74 */     OP_enter, /**< enter opcode */
/*  75 */     OP_leave, /**< leave opcode */
/*  76 */     OP_int3, /**< int3 opcode */
/*  77 */     OP_int, /**< int opcode */
/*  78 */     OP_into, /**< into opcode */
/*  79 */     OP_iret, /**< iret opcode */
/*  80 */     OP_aam, /**< aam opcode */
/*  81 */     OP_aad, /**< aad opcode */
/*  82 */     OP_xlat, /**< xlat opcode */
/*  83 */     OP_in, /**< in opcode */
/*  84 */     OP_out, /**< out opcode */
/*  85 */     OP_hlt, /**< hlt opcode */
/*  86 */     OP_cmc, /**< cmc opcode */
/*  87 */     OP_clc, /**< clc opcode */
/*  88 */     OP_stc, /**< stc opcode */
/*  89 */     OP_cli, /**< cli opcode */
/*  90 */     OP_sti, /**< sti opcode */
/*  91 */     OP_cld, /**< cld opcode */
/*  92 */     OP_std, /**< std opcode */


/*  93 */     OP_lar, /**< lar opcode */
/*  94 */     OP_lsl, /**< lsl opcode */
/*  95 */     OP_syscall, /**< syscall opcode */
/*  96 */     OP_clts, /**< clts opcode */
/*  97 */     OP_sysret, /**< sysret opcode */
/*  98 */     OP_invd, /**< invd opcode */
/*  99 */     OP_wbinvd, /**< wbinvd opcode */
/* 100 */     OP_ud2a, /**< ud2a opcode */
/* 101 */     OP_nop_modrm, /**< nop_modrm opcode */
/* 102 */     OP_movntps, /**< movntps opcode */
/* 103 */     OP_movntpd, /**< movntpd opcode */
/* 104 */     OP_wrmsr, /**< wrmsr opcode */
/* 105 */     OP_rdtsc, /**< rdtsc opcode */
/* 106 */     OP_rdmsr, /**< rdmsr opcode */
/* 107 */     OP_rdpmc, /**< rdpmc opcode */
/* 108 */     OP_sysenter, /**< sysenter opcode */
/* 109 */     OP_sysexit, /**< sysexit opcode */

/* 110 */     OP_cmovo, /**< cmovo opcode */
/* 111 */     OP_cmovno, /**< cmovno opcode */
/* 112 */     OP_cmovb, /**< cmovb opcode */
/* 113 */     OP_cmovnb, /**< cmovnb opcode */
/* 114 */     OP_cmovz, /**< cmovz opcode */
/* 115 */     OP_cmovnz, /**< cmovnz opcode */
/* 116 */     OP_cmovbe, /**< cmovbe opcode */
/* 117 */     OP_cmovnbe, /**< cmovnbe opcode */
/* 118 */     OP_cmovs, /**< cmovs opcode */
/* 119 */     OP_cmovns, /**< cmovns opcode */
/* 120 */     OP_cmovp, /**< cmovp opcode */
/* 121 */     OP_cmovnp, /**< cmovnp opcode */
/* 122 */     OP_cmovl, /**< cmovl opcode */
/* 123 */     OP_cmovnl, /**< cmovnl opcode */
/* 124 */     OP_cmovle, /**< cmovle opcode */
/* 125 */     OP_cmovnle, /**< cmovnle opcode */

/* 126 */     OP_punpcklbw, /**< punpcklbw opcode */
/* 127 */     OP_punpcklwd, /**< punpcklwd opcode */
/* 128 */     OP_punpckldq, /**< punpckldq opcode */
/* 129 */     OP_packsswb, /**< packsswb opcode */
/* 130 */     OP_pcmpgtb, /**< pcmpgtb opcode */
/* 131 */     OP_pcmpgtw, /**< pcmpgtw opcode */
/* 132 */     OP_pcmpgtd, /**< pcmpgtd opcode */
/* 133 */     OP_packuswb, /**< packuswb opcode */
/* 134 */     OP_punpckhbw, /**< punpckhbw opcode */
/* 135 */     OP_punpckhwd, /**< punpckhwd opcode */
/* 136 */     OP_punpckhdq, /**< punpckhdq opcode */
/* 137 */     OP_packssdw, /**< packssdw opcode */
/* 138 */     OP_punpcklqdq, /**< punpcklqdq opcode */
/* 139 */     OP_punpckhqdq, /**< punpckhqdq opcode */
/* 140 */     OP_movd, /**< movd opcode */
/* 141 */     OP_movq, /**< movq opcode */
/* 142 */     OP_movdqu, /**< movdqu opcode */
/* 143 */     OP_movdqa, /**< movdqa opcode */
/* 144 */     OP_pshufw, /**< pshufw opcode */
/* 145 */     OP_pshufd, /**< pshufd opcode */
/* 146 */     OP_pshufhw, /**< pshufhw opcode */
/* 147 */     OP_pshuflw, /**< pshuflw opcode */
/* 148 */     OP_pcmpeqb, /**< pcmpeqb opcode */
/* 149 */     OP_pcmpeqw, /**< pcmpeqw opcode */
/* 150 */     OP_pcmpeqd, /**< pcmpeqd opcode */
/* 151 */     OP_emms, /**< emms opcode */

/* 152 */     OP_jo, /**< jo opcode */
/* 153 */     OP_jno, /**< jno opcode */
/* 154 */     OP_jb, /**< jb opcode */
/* 155 */     OP_jnb, /**< jnb opcode */
/* 156 */     OP_jz, /**< jz opcode */
/* 157 */     OP_jnz, /**< jnz opcode */
/* 158 */     OP_jbe, /**< jbe opcode */
/* 159 */     OP_jnbe, /**< jnbe opcode */
/* 160 */     OP_js, /**< js opcode */
/* 161 */     OP_jns, /**< jns opcode */
/* 162 */     OP_jp, /**< jp opcode */
/* 163 */     OP_jnp, /**< jnp opcode */
/* 164 */     OP_jl, /**< jl opcode */
/* 165 */     OP_jnl, /**< jnl opcode */
/* 166 */     OP_jle, /**< jle opcode */
/* 167 */     OP_jnle, /**< jnle opcode */

/* 168 */     OP_seto, /**< seto opcode */
/* 169 */     OP_setno, /**< setno opcode */
/* 170 */     OP_setb, /**< setb opcode */
/* 171 */     OP_setnb, /**< setnb opcode */
/* 172 */     OP_setz, /**< setz opcode */
/* 173 */     OP_setnz, /**< setnz opcode */
/* 174 */     OP_setbe, /**< setbe opcode */
/* 175 */     OP_setnbe, /**< setnbe opcode */
/* 176 */     OP_sets, /**< sets opcode */
/* 177 */     OP_setns, /**< setns opcode */
/* 178 */     OP_setp, /**< setp opcode */
/* 179 */     OP_setnp, /**< setnp opcode */
/* 180 */     OP_setl, /**< setl opcode */
/* 181 */     OP_setnl, /**< setnl opcode */
/* 182 */     OP_setle, /**< setle opcode */
/* 183 */     OP_setnle, /**< setnle opcode */

/* 184 */     OP_cpuid, /**< cpuid opcode */
/* 185 */     OP_bt, /**< bt opcode */
/* 186 */     OP_shld, /**< shld opcode */
/* 187 */     OP_rsm, /**< rsm opcode */
/* 188 */     OP_bts, /**< bts opcode */
/* 189 */     OP_shrd, /**< shrd opcode */
/* 190 */     OP_cmpxchg, /**< cmpxchg opcode */
/* 191 */     OP_lss, /**< lss opcode */
/* 192 */     OP_btr, /**< btr opcode */
/* 193 */     OP_lfs, /**< lfs opcode */
/* 194 */     OP_lgs, /**< lgs opcode */
/* 195 */     OP_movzx, /**< movzx opcode */
/* 196 */     OP_ud2b, /**< ud2b opcode */
/* 197 */     OP_btc, /**< btc opcode */
/* 198 */     OP_bsf, /**< bsf opcode */
/* 199 */     OP_bsr, /**< bsr opcode */
/* 200 */     OP_movsx, /**< movsx opcode */
/* 201 */     OP_xadd, /**< xadd opcode */
/* 202 */     OP_movnti, /**< movnti opcode */
/* 203 */     OP_pinsrw, /**< pinsrw opcode */
/* 204 */     OP_pextrw, /**< pextrw opcode */
/* 205 */     OP_bswap, /**< bswap opcode */
/* 206 */     OP_psrlw, /**< psrlw opcode */
/* 207 */     OP_psrld, /**< psrld opcode */
/* 208 */     OP_psrlq, /**< psrlq opcode */
/* 209 */     OP_paddq, /**< paddq opcode */
/* 210 */     OP_pmullw, /**< pmullw opcode */
/* 211 */     OP_pmovmskb, /**< pmovmskb opcode */
/* 212 */     OP_psubusb, /**< psubusb opcode */
/* 213 */     OP_psubusw, /**< psubusw opcode */
/* 214 */     OP_pminub, /**< pminub opcode */
/* 215 */     OP_pand, /**< pand opcode */
/* 216 */     OP_paddusb, /**< paddusb opcode */
/* 217 */     OP_paddusw, /**< paddusw opcode */
/* 218 */     OP_pmaxub, /**< pmaxub opcode */
/* 219 */     OP_pandn, /**< pandn opcode */
/* 220 */     OP_pavgb, /**< pavgb opcode */
/* 221 */     OP_psraw, /**< psraw opcode */
/* 222 */     OP_psrad, /**< psrad opcode */
/* 223 */     OP_pavgw, /**< pavgw opcode */
/* 224 */     OP_pmulhuw, /**< pmulhuw opcode */
/* 225 */     OP_pmulhw, /**< pmulhw opcode */
/* 226 */     OP_movntq, /**< movntq opcode */
/* 227 */     OP_movntdq, /**< movntdq opcode */
/* 228 */     OP_psubsb, /**< psubsb opcode */
/* 229 */     OP_psubsw, /**< psubsw opcode */
/* 230 */     OP_pminsw, /**< pminsw opcode */
/* 231 */     OP_por, /**< por opcode */
/* 232 */     OP_paddsb, /**< paddsb opcode */
/* 233 */     OP_paddsw, /**< paddsw opcode */
/* 234 */     OP_pmaxsw, /**< pmaxsw opcode */
/* 235 */     OP_pxor, /**< pxor opcode */
/* 236 */     OP_psllw, /**< psllw opcode */
/* 237 */     OP_pslld, /**< pslld opcode */
/* 238 */     OP_psllq, /**< psllq opcode */
/* 239 */     OP_pmuludq, /**< pmuludq opcode */
/* 240 */     OP_pmaddwd, /**< pmaddwd opcode */
/* 241 */     OP_psadbw, /**< psadbw opcode */
/* 242 */     OP_maskmovq, /**< maskmovq opcode */
/* 243 */     OP_maskmovdqu, /**< maskmovdqu opcode */
/* 244 */     OP_psubb, /**< psubb opcode */
/* 245 */     OP_psubw, /**< psubw opcode */
/* 246 */     OP_psubd, /**< psubd opcode */
/* 247 */     OP_psubq, /**< psubq opcode */
/* 248 */     OP_paddb, /**< paddb opcode */
/* 249 */     OP_paddw, /**< paddw opcode */
/* 250 */     OP_paddd, /**< paddd opcode */
/* 251 */     OP_psrldq, /**< psrldq opcode */
/* 252 */     OP_pslldq, /**< pslldq opcode */


/* 253 */     OP_rol, /**< rol opcode */
/* 254 */     OP_ror, /**< ror opcode */
/* 255 */     OP_rcl, /**< rcl opcode */
/* 256 */     OP_rcr, /**< rcr opcode */
/* 257 */     OP_shl, /**< shl opcode */
/* 258 */     OP_shr, /**< shr opcode */
/* 259 */     OP_sar, /**< sar opcode */
/* 260 */     OP_not, /**< not opcode */
/* 261 */     OP_neg, /**< neg opcode */
/* 262 */     OP_mul, /**< mul opcode */
/* 263 */     OP_div, /**< div opcode */
/* 264 */     OP_idiv, /**< idiv opcode */
/* 265 */     OP_sldt, /**< sldt opcode */
/* 266 */     OP_str, /**< str opcode */
/* 267 */     OP_lldt, /**< lldt opcode */
/* 268 */     OP_ltr, /**< ltr opcode */
/* 269 */     OP_verr, /**< verr opcode */
/* 270 */     OP_verw, /**< verw opcode */
/* 271 */     OP_sgdt, /**< sgdt opcode */
/* 272 */     OP_sidt, /**< sidt opcode */
/* 273 */     OP_lgdt, /**< lgdt opcode */
/* 274 */     OP_lidt, /**< lidt opcode */
/* 275 */     OP_smsw, /**< smsw opcode */
/* 276 */     OP_lmsw, /**< lmsw opcode */
/* 277 */     OP_invlpg, /**< invlpg opcode */
/* 278 */     OP_cmpxchg8b, /**< cmpxchg8b opcode */
/* 279 */     OP_fxsave, /**< fxsave opcode */
/* 280 */     OP_fxrstor, /**< fxrstor opcode */
/* 281 */     OP_ldmxcsr, /**< ldmxcsr opcode */
/* 282 */     OP_stmxcsr, /**< stmxcsr opcode */
/* 283 */     OP_lfence, /**< lfence opcode */
/* 284 */     OP_mfence, /**< mfence opcode */
/* 285 */     OP_clflush, /**< clflush opcode */
/* 286 */     OP_sfence, /**< sfence opcode */
/* 287 */     OP_prefetchnta, /**< prefetchnta opcode */
/* 288 */     OP_prefetcht0, /**< prefetcht0 opcode */
/* 289 */     OP_prefetcht1, /**< prefetcht1 opcode */
/* 290 */     OP_prefetcht2, /**< prefetcht2 opcode */
/* 291 */     OP_prefetch, /**< prefetch opcode */
/* 292 */     OP_prefetchw, /**< prefetchw opcode */


/* 293 */     OP_movups, /**< movups opcode */
/* 294 */     OP_movss, /**< movss opcode */
/* 295 */     OP_movupd, /**< movupd opcode */
/* 296 */     OP_movsd, /**< movsd opcode */
/* 297 */     OP_movlps, /**< movlps opcode */
/* 298 */     OP_movlpd, /**< movlpd opcode */
/* 299 */     OP_unpcklps, /**< unpcklps opcode */
/* 300 */     OP_unpcklpd, /**< unpcklpd opcode */
/* 301 */     OP_unpckhps, /**< unpckhps opcode */
/* 302 */     OP_unpckhpd, /**< unpckhpd opcode */
/* 303 */     OP_movhps, /**< movhps opcode */
/* 304 */     OP_movhpd, /**< movhpd opcode */
/* 305 */     OP_movaps, /**< movaps opcode */
/* 306 */     OP_movapd, /**< movapd opcode */
/* 307 */     OP_cvtpi2ps, /**< cvtpi2ps opcode */
/* 308 */     OP_cvtsi2ss, /**< cvtsi2ss opcode */
/* 309 */     OP_cvtpi2pd, /**< cvtpi2pd opcode */
/* 310 */     OP_cvtsi2sd, /**< cvtsi2sd opcode */
/* 311 */     OP_cvttps2pi, /**< cvttps2pi opcode */
/* 312 */     OP_cvttss2si, /**< cvttss2si opcode */
/* 313 */     OP_cvttpd2pi, /**< cvttpd2pi opcode */
/* 314 */     OP_cvttsd2si, /**< cvttsd2si opcode */
/* 315 */     OP_cvtps2pi, /**< cvtps2pi opcode */
/* 316 */     OP_cvtss2si, /**< cvtss2si opcode */
/* 317 */     OP_cvtpd2pi, /**< cvtpd2pi opcode */
/* 318 */     OP_cvtsd2si, /**< cvtsd2si opcode */
/* 319 */     OP_ucomiss, /**< ucomiss opcode */
/* 320 */     OP_ucomisd, /**< ucomisd opcode */
/* 321 */     OP_comiss, /**< comiss opcode */
/* 322 */     OP_comisd, /**< comisd opcode */
/* 323 */     OP_movmskps, /**< movmskps opcode */
/* 324 */     OP_movmskpd, /**< movmskpd opcode */
/* 325 */     OP_sqrtps, /**< sqrtps opcode */
/* 326 */     OP_sqrtss, /**< sqrtss opcode */
/* 327 */     OP_sqrtpd, /**< sqrtpd opcode */
/* 328 */     OP_sqrtsd, /**< sqrtsd opcode */
/* 329 */     OP_rsqrtps, /**< rsqrtps opcode */
/* 330 */     OP_rsqrtss, /**< rsqrtss opcode */
/* 331 */     OP_rcpps, /**< rcpps opcode */
/* 332 */     OP_rcpss, /**< rcpss opcode */
/* 333 */     OP_andps, /**< andps opcode */
/* 334 */     OP_andpd, /**< andpd opcode */
/* 335 */     OP_andnps, /**< andnps opcode */
/* 336 */     OP_andnpd, /**< andnpd opcode */
/* 337 */     OP_orps, /**< orps opcode */
/* 338 */     OP_orpd, /**< orpd opcode */
/* 339 */     OP_xorps, /**< xorps opcode */
/* 340 */     OP_xorpd, /**< xorpd opcode */
/* 341 */     OP_addps, /**< addps opcode */
/* 342 */     OP_addss, /**< addss opcode */
/* 343 */     OP_addpd, /**< addpd opcode */
/* 344 */     OP_addsd, /**< addsd opcode */
/* 345 */     OP_mulps, /**< mulps opcode */
/* 346 */     OP_mulss, /**< mulss opcode */
/* 347 */     OP_mulpd, /**< mulpd opcode */
/* 348 */     OP_mulsd, /**< mulsd opcode */
/* 349 */     OP_cvtps2pd, /**< cvtps2pd opcode */
/* 350 */     OP_cvtss2sd, /**< cvtss2sd opcode */
/* 351 */     OP_cvtpd2ps, /**< cvtpd2ps opcode */
/* 352 */     OP_cvtsd2ss, /**< cvtsd2ss opcode */
/* 353 */     OP_cvtdq2ps, /**< cvtdq2ps opcode */
/* 354 */     OP_cvttps2dq, /**< cvttps2dq opcode */
/* 355 */     OP_cvtps2dq, /**< cvtps2dq opcode */
/* 356 */     OP_subps, /**< subps opcode */
/* 357 */     OP_subss, /**< subss opcode */
/* 358 */     OP_subpd, /**< subpd opcode */
/* 359 */     OP_subsd, /**< subsd opcode */
/* 360 */     OP_minps, /**< minps opcode */
/* 361 */     OP_minss, /**< minss opcode */
/* 362 */     OP_minpd, /**< minpd opcode */
/* 363 */     OP_minsd, /**< minsd opcode */
/* 364 */     OP_divps, /**< divps opcode */
/* 365 */     OP_divss, /**< divss opcode */
/* 366 */     OP_divpd, /**< divpd opcode */
/* 367 */     OP_divsd, /**< divsd opcode */
/* 368 */     OP_maxps, /**< maxps opcode */
/* 369 */     OP_maxss, /**< maxss opcode */
/* 370 */     OP_maxpd, /**< maxpd opcode */
/* 371 */     OP_maxsd, /**< maxsd opcode */
/* 372 */     OP_cmpps, /**< cmpps opcode */
/* 373 */     OP_cmpss, /**< cmpss opcode */
/* 374 */     OP_cmppd, /**< cmppd opcode */
/* 375 */     OP_cmpsd, /**< cmpsd opcode */
/* 376 */     OP_shufps, /**< shufps opcode */
/* 377 */     OP_shufpd, /**< shufpd opcode */
/* 378 */     OP_cvtdq2pd, /**< cvtdq2pd opcode */
/* 379 */     OP_cvttpd2dq, /**< cvttpd2dq opcode */
/* 380 */     OP_cvtpd2dq, /**< cvtpd2dq opcode */
/* 381 */     OP_nop, /**< nop opcode */
/* 382 */     OP_pause, /**< pause opcode */

/* 383 */     OP_ins, /**< ins opcode */
/* 384 */     OP_rep_ins, /**< rep_ins opcode */
/* 385 */     OP_outs, /**< outs opcode */
/* 386 */     OP_rep_outs, /**< rep_outs opcode */
/* 387 */     OP_movs, /**< movs opcode */
/* 388 */     OP_rep_movs, /**< rep_movs opcode */
/* 389 */     OP_stos, /**< stos opcode */
/* 390 */     OP_rep_stos, /**< rep_stos opcode */
/* 391 */     OP_lods, /**< lods opcode */
/* 392 */     OP_rep_lods, /**< rep_lods opcode */
/* 393 */     OP_cmps, /**< cmps opcode */
/* 394 */     OP_rep_cmps, /**< rep_cmps opcode */
/* 395 */     OP_repne_cmps, /**< repne_cmps opcode */
/* 396 */     OP_scas, /**< scas opcode */
/* 397 */     OP_rep_scas, /**< rep_scas opcode */
/* 398 */     OP_repne_scas, /**< repne_scas opcode */


/* 399 */     OP_fadd, /**< fadd opcode */
/* 400 */     OP_fmul, /**< fmul opcode */
/* 401 */     OP_fcom, /**< fcom opcode */
/* 402 */     OP_fcomp, /**< fcomp opcode */
/* 403 */     OP_fsub, /**< fsub opcode */
/* 404 */     OP_fsubr, /**< fsubr opcode */
/* 405 */     OP_fdiv, /**< fdiv opcode */
/* 406 */     OP_fdivr, /**< fdivr opcode */
/* 407 */     OP_fld, /**< fld opcode */
/* 408 */     OP_fst, /**< fst opcode */
/* 409 */     OP_fstp, /**< fstp opcode */
/* 410 */     OP_fldenv, /**< fldenv opcode */
/* 411 */     OP_fldcw, /**< fldcw opcode */
/* 412 */     OP_fnstenv, /**< fnstenv opcode */
/* 413 */     OP_fnstcw, /**< fnstcw opcode */
/* 414 */     OP_fiadd, /**< fiadd opcode */
/* 415 */     OP_fimul, /**< fimul opcode */
/* 416 */     OP_ficom, /**< ficom opcode */
/* 417 */     OP_ficomp, /**< ficomp opcode */
/* 418 */     OP_fisub, /**< fisub opcode */
/* 419 */     OP_fisubr, /**< fisubr opcode */
/* 420 */     OP_fidiv, /**< fidiv opcode */
/* 421 */     OP_fidivr, /**< fidivr opcode */
/* 422 */     OP_fild, /**< fild opcode */
/* 423 */     OP_fist, /**< fist opcode */
/* 424 */     OP_fistp, /**< fistp opcode */
/* 425 */     OP_frstor, /**< frstor opcode */
/* 426 */     OP_fnsave, /**< fnsave opcode */
/* 427 */     OP_fnstsw, /**< fnstsw opcode */

/* 428 */     OP_fbld, /**< fbld opcode */
/* 429 */     OP_fbstp, /**< fbstp opcode */


/* 430 */     OP_fxch, /**< fxch opcode */
/* 431 */     OP_fnop, /**< fnop opcode */
/* 432 */     OP_fchs, /**< fchs opcode */
/* 433 */     OP_fabs, /**< fabs opcode */
/* 434 */     OP_ftst, /**< ftst opcode */
/* 435 */     OP_fxam, /**< fxam opcode */
/* 436 */     OP_fld1, /**< fld1 opcode */
/* 437 */     OP_fldl2t, /**< fldl2t opcode */
/* 438 */     OP_fldl2e, /**< fldl2e opcode */
/* 439 */     OP_fldpi, /**< fldpi opcode */
/* 440 */     OP_fldlg2, /**< fldlg2 opcode */
/* 441 */     OP_fldln2, /**< fldln2 opcode */
/* 442 */     OP_fldz, /**< fldz opcode */
/* 443 */     OP_f2xm1, /**< f2xm1 opcode */
/* 444 */     OP_fyl2x, /**< fyl2x opcode */
/* 445 */     OP_fptan, /**< fptan opcode */
/* 446 */     OP_fpatan, /**< fpatan opcode */
/* 447 */     OP_fxtract, /**< fxtract opcode */
/* 448 */     OP_fprem1, /**< fprem1 opcode */
/* 449 */     OP_fdecstp, /**< fdecstp opcode */
/* 450 */     OP_fincstp, /**< fincstp opcode */
/* 451 */     OP_fprem, /**< fprem opcode */
/* 452 */     OP_fyl2xp1, /**< fyl2xp1 opcode */
/* 453 */     OP_fsqrt, /**< fsqrt opcode */
/* 454 */     OP_fsincos, /**< fsincos opcode */
/* 455 */     OP_frndint, /**< frndint opcode */
/* 456 */     OP_fscale, /**< fscale opcode */
/* 457 */     OP_fsin, /**< fsin opcode */
/* 458 */     OP_fcos, /**< fcos opcode */
/* 459 */     OP_fcmovb, /**< fcmovb opcode */
/* 460 */     OP_fcmove, /**< fcmove opcode */
/* 461 */     OP_fcmovbe, /**< fcmovbe opcode */
/* 462 */     OP_fcmovu, /**< fcmovu opcode */
/* 463 */     OP_fucompp, /**< fucompp opcode */
/* 464 */     OP_fcmovnb, /**< fcmovnb opcode */
/* 465 */     OP_fcmovne, /**< fcmovne opcode */
/* 466 */     OP_fcmovnbe, /**< fcmovnbe opcode */
/* 467 */     OP_fcmovnu, /**< fcmovnu opcode */
/* 468 */     OP_fnclex, /**< fnclex opcode */
/* 469 */     OP_fninit, /**< fninit opcode */
/* 470 */     OP_fucomi, /**< fucomi opcode */
/* 471 */     OP_fcomi, /**< fcomi opcode */
/* 472 */     OP_ffree, /**< ffree opcode */
/* 473 */     OP_fucom, /**< fucom opcode */
/* 474 */     OP_fucomp, /**< fucomp opcode */
/* 475 */     OP_faddp, /**< faddp opcode */
/* 476 */     OP_fmulp, /**< fmulp opcode */
/* 477 */     OP_fcompp, /**< fcompp opcode */
/* 478 */     OP_fsubrp, /**< fsubrp opcode */
/* 479 */     OP_fsubp, /**< fsubp opcode */
/* 480 */     OP_fdivrp, /**< fdivrp opcode */
/* 481 */     OP_fdivp, /**< fdivp opcode */
/* 482 */     OP_fucomip, /**< fucomip opcode */
/* 483 */     OP_fcomip, /**< fcomip opcode */

    /* SSE3 instructions */
/* 484 */     OP_fisttp, /**< fisttp opcode */
/* 485 */     OP_haddpd, /**< haddpd opcode */
/* 486 */     OP_haddps, /**< haddps opcode */
/* 487 */     OP_hsubpd, /**< hsubpd opcode */
/* 488 */     OP_hsubps, /**< hsubps opcode */
/* 489 */     OP_addsubpd, /**< addsubpd opcode */
/* 490 */     OP_addsubps, /**< addsubps opcode */
/* 491 */     OP_lddqu, /**< lddqu opcode */
/* 492 */     OP_monitor, /**< monitor opcode */
/* 493 */     OP_mwait, /**< mwait opcode */
/* 494 */     OP_movsldup, /**< movsldup opcode */
/* 495 */     OP_movshdup, /**< movshdup opcode */
/* 496 */     OP_movddup, /**< movddup opcode */

    /* 3D-Now! instructions */
/* 497 */     OP_femms, /**< femms opcode */
/* 498 */     OP_unknown_3dnow, /**< unknown_3dnow opcode */
/* 499 */     OP_pavgusb, /**< pavgusb opcode */
/* 500 */     OP_pfadd, /**< pfadd opcode */
/* 501 */     OP_pfacc, /**< pfacc opcode */
/* 502 */     OP_pfcmpge, /**< pfcmpge opcode */
/* 503 */     OP_pfcmpgt, /**< pfcmpgt opcode */
/* 504 */     OP_pfcmpeq, /**< pfcmpeq opcode */
/* 505 */     OP_pfmin, /**< pfmin opcode */
/* 506 */     OP_pfmax, /**< pfmax opcode */
/* 507 */     OP_pfmul, /**< pfmul opcode */
/* 508 */     OP_pfrcp, /**< pfrcp opcode */
/* 509 */     OP_pfrcpit1, /**< pfrcpit1 opcode */
/* 510 */     OP_pfrcpit2, /**< pfrcpit2 opcode */
/* 511 */     OP_pfrsqrt, /**< pfrsqrt opcode */
/* 512 */     OP_pfrsqit1, /**< pfrsqit1 opcode */
/* 513 */     OP_pmulhrw, /**< pmulhrw opcode */
/* 514 */     OP_pfsub, /**< pfsub opcode */
/* 515 */     OP_pfsubr, /**< pfsubr opcode */
/* 516 */     OP_pi2fd, /**< pi2fd opcode */
/* 517 */     OP_pf2id, /**< pf2id opcode */
/* 518 */     OP_pi2fw, /**< pi2fw opcode */
/* 519 */     OP_pf2iw, /**< pf2iw opcode */
/* 520 */     OP_pfnacc, /**< pfnacc opcode */
/* 521 */     OP_pfpnacc, /**< pfpnacc opcode */
/* 522 */     OP_pswapd, /**< pswapd opcode */

    /* SSSE3 */
/* 523 */     OP_pshufb, /**< pshufb opcode */
/* 524 */     OP_phaddw, /**< phaddw opcode */
/* 525 */     OP_phaddd, /**< phaddd opcode */
/* 526 */     OP_phaddsw, /**< phaddsw opcode */
/* 527 */     OP_pmaddubsw, /**< pmaddubsw opcode */
/* 528 */     OP_phsubw, /**< phsubw opcode */
/* 529 */     OP_phsubd, /**< phsubd opcode */
/* 530 */     OP_phsubsw, /**< phsubsw opcode */
/* 531 */     OP_psignb, /**< psignb opcode */
/* 532 */     OP_psignw, /**< psignw opcode */
/* 533 */     OP_psignd, /**< psignd opcode */
/* 534 */     OP_pmulhrsw, /**< pmulhrsw opcode */
/* 535 */     OP_pabsb, /**< pabsb opcode */
/* 536 */     OP_pabsw, /**< pabsw opcode */
/* 537 */     OP_pabsd, /**< pabsd opcode */
/* 538 */     OP_palignr, /**< palignr opcode */

    /* SSE4 (incl AMD and Intel-specific extensions */
/* 539 */     OP_popcnt, /**< popcnt opcode */
/* 540 */     OP_movntss, /**< movntss opcode */
/* 541 */     OP_movntsd, /**< movntsd opcode */
/* 542 */     OP_extrq, /**< extrq opcode */
/* 543 */     OP_insertq, /**< insertq opcode */
/* 544 */     OP_lzcnt, /**< lzcnt opcode */
/* 545 */     OP_pblendvb, /**< pblendvb opcode */
/* 546 */     OP_blendvps, /**< blendvps opcode */
/* 547 */     OP_blendvpd, /**< blendvpd opcode */
/* 548 */     OP_ptest, /**< ptest opcode */
/* 549 */     OP_pmovsxbw, /**< pmovsxbw opcode */
/* 550 */     OP_pmovsxbd, /**< pmovsxbd opcode */
/* 551 */     OP_pmovsxbq, /**< pmovsxbq opcode */
/* 552 */     OP_pmovsxdw, /**< pmovsxdw opcode */
/* 553 */     OP_pmovsxwq, /**< pmovsxwq opcode */
/* 554 */     OP_pmovsxdq, /**< pmovsxdq opcode */
/* 555 */     OP_pmuldq, /**< pmuldq opcode */
/* 556 */     OP_pcmpeqq, /**< pcmpeqq opcode */
/* 557 */     OP_movntdqa, /**< movntdqa opcode */
/* 558 */     OP_packusdw, /**< packusdw opcode */
/* 559 */     OP_pmovzxbw, /**< pmovzxbw opcode */
/* 560 */     OP_pmovzxbd, /**< pmovzxbd opcode */
/* 561 */     OP_pmovzxbq, /**< pmovzxbq opcode */
/* 562 */     OP_pmovzxdw, /**< pmovzxdw opcode */
/* 563 */     OP_pmovzxwq, /**< pmovzxwq opcode */
/* 564 */     OP_pmovzxdq, /**< pmovzxdq opcode */
/* 565 */     OP_pcmpgtq, /**< pcmpgtq opcode */
/* 566 */     OP_pminsb, /**< pminsb opcode */
/* 567 */     OP_pminsd, /**< pminsd opcode */
/* 568 */     OP_pminuw, /**< pminuw opcode */
/* 569 */     OP_pminud, /**< pminud opcode */
/* 570 */     OP_pmaxsb, /**< pmaxsb opcode */
/* 571 */     OP_pmaxsd, /**< pmaxsd opcode */
/* 572 */     OP_pmaxuw, /**< pmaxuw opcode */
/* 573 */     OP_pmaxud, /**< pmaxud opcode */
/* 574 */     OP_pmulld, /**< pmulld opcode */
/* 575 */     OP_phminposuw, /**< phminposuw opcode */
/* 576 */     OP_crc32, /**< crc32 opcode */
/* 577 */     OP_pextrb, /**< pextrb opcode */
/* 578 */     OP_pextrd, /**< pextrd opcode */
/* 579 */     OP_extractps, /**< extractps opcode */
/* 580 */     OP_roundps, /**< roundps opcode */
/* 581 */     OP_roundpd, /**< roundpd opcode */
/* 582 */     OP_roundss, /**< roundss opcode */
/* 583 */     OP_roundsd, /**< roundsd opcode */
/* 584 */     OP_blendps, /**< blendps opcode */
/* 585 */     OP_blendpd, /**< blendpd opcode */
/* 586 */     OP_pblendw, /**< pblendw opcode */
/* 587 */     OP_pinsrb, /**< pinsrb opcode */
/* 588 */     OP_insertps, /**< insertps opcode */
/* 589 */     OP_pinsrd, /**< pinsrd opcode */
/* 590 */     OP_dpps, /**< dpps opcode */
/* 591 */     OP_dppd, /**< dppd opcode */
/* 592 */     OP_mpsadbw, /**< mpsadbw opcode */
/* 593 */     OP_pcmpestrm, /**< pcmpestrm opcode */
/* 594 */     OP_pcmpestri, /**< pcmpestri opcode */
/* 595 */     OP_pcmpistrm, /**< pcmpistrm opcode */
/* 596 */     OP_pcmpistri, /**< pcmpistri opcode */

    /* x64 */
/* 597 */     OP_movsxd, /**< movsxd opcode */
/* 598 */     OP_swapgs, /**< swapgs opcode */

    /* VMX */
/* 599 */     OP_vmcall, /**< vmcall opcode */
/* 600 */     OP_vmlaunch, /**< vmlaunch opcode */
/* 601 */     OP_vmresume, /**< vmresume opcode */
/* 602 */     OP_vmxoff, /**< vmxoff opcode */
/* 603 */     OP_vmptrst, /**< vmptrst opcode */
/* 604 */     OP_vmptrld, /**< vmptrld opcode */
/* 605 */     OP_vmxon, /**< vmxon opcode */
/* 606 */     OP_vmclear, /**< vmclear opcode */
/* 607 */     OP_vmread, /**< vmread opcode */
/* 608 */     OP_vmwrite, /**< vmwrite opcode */

    /* undocumented */
/* 609 */     OP_int1, /**< int1 opcode */
/* 610 */     OP_salc, /**< salc opcode */
/* 611 */     OP_ffreep, /**< ffreep opcode */

    /* Keep these at the end so that ifdefs don't change internal enum values */

    OP_FIRST = OP_add, /**< First real opcode. */
    OP_LAST = OP_ffreep,   /**< Last real opcode. */
};


/* alternative names */
/* we do not equate the fwait+op opcodes
 *   fstsw, fstcw, fstenv, finit, fclex
 * for us that has to be a sequence of instructions: a separate fwait
 */
/* 16-bit versions that have different names */
#define OP_cbw        OP_cwde /**< Alternative opcode name for 16-bit version. */
#define OP_cwd        OP_cdq /**< Alternative opcode name for 16-bit version. */
#define OP_jcxz       OP_jecxz /**< Alternative opcode name for 16-bit version. */
/* 64-bit versions that have different names */
#define OP_jrcxz      OP_jecxz     /**< Alternative opcode name for 64-bit version. */
#define OP_cmpxchg16b OP_cmpxchg8b /**< Alternative opcode name for 64-bit version. */
#define OP_pextrq     OP_pextrd    /**< Alternative opcode name for 64-bit version. */
#define OP_pinsrq     OP_pinsrd    /**< Alternative opcode name for 64-bit version. */
/* reg-reg version has different name */
#define OP_movhlps    OP_movlps /**< Alternative opcode name for reg-reg version. */
#define OP_movlhps    OP_movhps /**< Alternative opcode name for reg-reg version. */
/* condition codes */
#define OP_jae_short  OP_jnb_short  /**< Alternative opcode name. */
#define OP_jnae_short OP_jb_short   /**< Alternative opcode name. */
#define OP_ja_short   OP_jnbe_short /**< Alternative opcode name. */
#define OP_jna_short  OP_jbe_short  /**< Alternative opcode name. */
#define OP_je_short   OP_jz_short   /**< Alternative opcode name. */
#define OP_jne_short  OP_jnz_short  /**< Alternative opcode name. */
#define OP_jge_short  OP_jnl_short  /**< Alternative opcode name. */
#define OP_jg_short   OP_jnle_short /**< Alternative opcode name. */
#define OP_jae  OP_jnb        /**< Alternative opcode name. */
#define OP_jnae OP_jb         /**< Alternative opcode name. */
#define OP_ja   OP_jnbe       /**< Alternative opcode name. */
#define OP_jna  OP_jbe        /**< Alternative opcode name. */
#define OP_je   OP_jz         /**< Alternative opcode name. */
#define OP_jne  OP_jnz        /**< Alternative opcode name. */
#define OP_jge  OP_jnl        /**< Alternative opcode name. */
#define OP_jg   OP_jnle       /**< Alternative opcode name. */
#define OP_setae  OP_setnb    /**< Alternative opcode name. */
#define OP_setnae OP_setb     /**< Alternative opcode name. */
#define OP_seta   OP_setnbe   /**< Alternative opcode name. */
#define OP_setna  OP_setbe    /**< Alternative opcode name. */
#define OP_sete   OP_setz     /**< Alternative opcode name. */
#define OP_setne  OP_setnz    /**< Alternative opcode name. */
#define OP_setge  OP_setnl    /**< Alternative opcode name. */
#define OP_setg   OP_setnle   /**< Alternative opcode name. */
#define OP_cmovae  OP_cmovnb  /**< Alternative opcode name. */
#define OP_cmovnae OP_cmovb   /**< Alternative opcode name. */
#define OP_cmova   OP_cmovnbe /**< Alternative opcode name. */
#define OP_cmovna  OP_cmovbe  /**< Alternative opcode name. */
#define OP_cmove   OP_cmovz   /**< Alternative opcode name. */
#define OP_cmovne  OP_cmovnz  /**< Alternative opcode name. */
#define OP_cmovge  OP_cmovnl  /**< Alternative opcode name. */
#define OP_cmovg   OP_cmovnle /**< Alternative opcode name. */
/* undocumented opcodes */
#define OP_icebp OP_int1
#define OP_setalc OP_salc

/****************************************************************************/




#endif /* _DR_IR_OPCODES_H_ */
