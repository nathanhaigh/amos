/* This file auto-generated from insns.dat by insns.pl - don't edit it */

#include "nasm.h"
#include "insns.h"

static const struct itemplate instrux[] = {
    /*    0 */ {I_RESB, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+18790, IF_8086},
    /*    1 */ {I_AAA, 0, {0,0,0,0,0}, nasm_bytecodes+19680, IF_8086|IF_NOLONG},
    /*    2 */ {I_AAD, 0, {0,0,0,0,0}, nasm_bytecodes+18632, IF_8086|IF_NOLONG},
    /*    3 */ {I_AAD, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+18636, IF_8086|IF_SB|IF_NOLONG},
    /*    4 */ {I_AAM, 0, {0,0,0,0,0}, nasm_bytecodes+18640, IF_8086|IF_NOLONG},
    /*    5 */ {I_AAM, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+18644, IF_8086|IF_SB|IF_NOLONG},
    /*    6 */ {I_AAS, 0, {0,0,0,0,0}, nasm_bytecodes+19683, IF_8086|IF_NOLONG},
    /*    7 */ {I_ADC, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+18648, IF_8086|IF_SM},
    /*    8 */ {I_ADC, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+18648, IF_8086},
    /*    9 */ {I_ADC, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16772, IF_8086|IF_SM},
    /*   10 */ {I_ADC, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16772, IF_8086},
    /*   11 */ {I_ADC, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+16777, IF_386|IF_SM},
    /*   12 */ {I_ADC, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+16777, IF_386},
    /*   13 */ {I_ADC, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+16782, IF_X64|IF_SM},
    /*   14 */ {I_ADC, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+16782, IF_X64},
    /*   15 */ {I_ADC, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+9884, IF_8086|IF_SM},
    /*   16 */ {I_ADC, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+9884, IF_8086},
    /*   17 */ {I_ADC, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+16787, IF_8086|IF_SM},
    /*   18 */ {I_ADC, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16787, IF_8086},
    /*   19 */ {I_ADC, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+16792, IF_386|IF_SM},
    /*   20 */ {I_ADC, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+16792, IF_386},
    /*   21 */ {I_ADC, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+16797, IF_X64|IF_SM},
    /*   22 */ {I_ADC, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+16797, IF_X64},
    /*   23 */ {I_ADC, 2, {RM_GPR|BITS16,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13220, IF_8086},
    /*   24 */ {I_ADC, 2, {RM_GPR|BITS32,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13226, IF_386},
    /*   25 */ {I_ADC, 2, {RM_GPR|BITS64,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13232, IF_X64},
    /*   26 */ {I_ADC, 2, {REG_AL,IMMEDIATE,0,0,0}, nasm_bytecodes+18652, IF_8086|IF_SM},
    /*   27 */ {I_ADC, 2, {REG_AX,SBYTE16,0,0,0}, nasm_bytecodes+13220, IF_8086|IF_SM},
    /*   28 */ {I_ADC, 2, {REG_AX,IMMEDIATE,0,0,0}, nasm_bytecodes+16802, IF_8086|IF_SM},
    /*   29 */ {I_ADC, 2, {REG_EAX,SBYTE32,0,0,0}, nasm_bytecodes+13226, IF_386|IF_SM},
    /*   30 */ {I_ADC, 2, {REG_EAX,IMMEDIATE,0,0,0}, nasm_bytecodes+16807, IF_386|IF_SM},
    /*   31 */ {I_ADC, 2, {REG_RAX,SBYTE64,0,0,0}, nasm_bytecodes+13232, IF_X64|IF_SM},
    /*   32 */ {I_ADC, 2, {REG_RAX,IMMEDIATE,0,0,0}, nasm_bytecodes+16812, IF_X64|IF_SM},
    /*   33 */ {I_ADC, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+16817, IF_8086|IF_SM},
    /*   34 */ {I_ADC, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+13238, IF_8086|IF_SM},
    /*   35 */ {I_ADC, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+13244, IF_386|IF_SM},
    /*   36 */ {I_ADC, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+13250, IF_X64|IF_SM},
    /*   37 */ {I_ADC, 2, {MEMORY,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+16817, IF_8086|IF_SM},
    /*   38 */ {I_ADC, 2, {MEMORY,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+13238, IF_8086|IF_SM},
    /*   39 */ {I_ADC, 2, {MEMORY,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+13244, IF_386|IF_SM},
    /*   40 */ {I_ADD, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+18656, IF_8086|IF_SM},
    /*   41 */ {I_ADD, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+18656, IF_8086},
    /*   42 */ {I_ADD, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16822, IF_8086|IF_SM},
    /*   43 */ {I_ADD, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16822, IF_8086},
    /*   44 */ {I_ADD, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+16827, IF_386|IF_SM},
    /*   45 */ {I_ADD, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+16827, IF_386},
    /*   46 */ {I_ADD, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+16832, IF_X64|IF_SM},
    /*   47 */ {I_ADD, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+16832, IF_X64},
    /*   48 */ {I_ADD, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+10535, IF_8086|IF_SM},
    /*   49 */ {I_ADD, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+10535, IF_8086},
    /*   50 */ {I_ADD, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+16837, IF_8086|IF_SM},
    /*   51 */ {I_ADD, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16837, IF_8086},
    /*   52 */ {I_ADD, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+16842, IF_386|IF_SM},
    /*   53 */ {I_ADD, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+16842, IF_386},
    /*   54 */ {I_ADD, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+16847, IF_X64|IF_SM},
    /*   55 */ {I_ADD, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+16847, IF_X64},
    /*   56 */ {I_ADD, 2, {RM_GPR|BITS16,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13256, IF_8086},
    /*   57 */ {I_ADD, 2, {RM_GPR|BITS32,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13262, IF_386},
    /*   58 */ {I_ADD, 2, {RM_GPR|BITS64,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13268, IF_X64},
    /*   59 */ {I_ADD, 2, {REG_AL,IMMEDIATE,0,0,0}, nasm_bytecodes+18660, IF_8086|IF_SM},
    /*   60 */ {I_ADD, 2, {REG_AX,SBYTE16,0,0,0}, nasm_bytecodes+13256, IF_8086|IF_SM},
    /*   61 */ {I_ADD, 2, {REG_AX,IMMEDIATE,0,0,0}, nasm_bytecodes+16852, IF_8086|IF_SM},
    /*   62 */ {I_ADD, 2, {REG_EAX,SBYTE32,0,0,0}, nasm_bytecodes+13262, IF_386|IF_SM},
    /*   63 */ {I_ADD, 2, {REG_EAX,IMMEDIATE,0,0,0}, nasm_bytecodes+16857, IF_386|IF_SM},
    /*   64 */ {I_ADD, 2, {REG_RAX,SBYTE64,0,0,0}, nasm_bytecodes+13268, IF_X64|IF_SM},
    /*   65 */ {I_ADD, 2, {REG_RAX,IMMEDIATE,0,0,0}, nasm_bytecodes+16862, IF_X64|IF_SM},
    /*   66 */ {I_ADD, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+16867, IF_8086|IF_SM},
    /*   67 */ {I_ADD, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+13274, IF_8086|IF_SM},
    /*   68 */ {I_ADD, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+13280, IF_386|IF_SM},
    /*   69 */ {I_ADD, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+13286, IF_X64|IF_SM},
    /*   70 */ {I_ADD, 2, {MEMORY,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+16867, IF_8086|IF_SM},
    /*   71 */ {I_ADD, 2, {MEMORY,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+13274, IF_8086|IF_SM},
    /*   72 */ {I_ADD, 2, {MEMORY,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+13280, IF_386|IF_SM},
    /*   73 */ {I_AND, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+18664, IF_8086|IF_SM},
    /*   74 */ {I_AND, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+18664, IF_8086},
    /*   75 */ {I_AND, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16872, IF_8086|IF_SM},
    /*   76 */ {I_AND, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16872, IF_8086},
    /*   77 */ {I_AND, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+16877, IF_386|IF_SM},
    /*   78 */ {I_AND, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+16877, IF_386},
    /*   79 */ {I_AND, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+16882, IF_X64|IF_SM},
    /*   80 */ {I_AND, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+16882, IF_X64},
    /*   81 */ {I_AND, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+10822, IF_8086|IF_SM},
    /*   82 */ {I_AND, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+10822, IF_8086},
    /*   83 */ {I_AND, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+16887, IF_8086|IF_SM},
    /*   84 */ {I_AND, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16887, IF_8086},
    /*   85 */ {I_AND, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+16892, IF_386|IF_SM},
    /*   86 */ {I_AND, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+16892, IF_386},
    /*   87 */ {I_AND, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+16897, IF_X64|IF_SM},
    /*   88 */ {I_AND, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+16897, IF_X64},
    /*   89 */ {I_AND, 2, {RM_GPR|BITS16,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13292, IF_8086},
    /*   90 */ {I_AND, 2, {RM_GPR|BITS32,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13298, IF_386},
    /*   91 */ {I_AND, 2, {RM_GPR|BITS64,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13304, IF_X64},
    /*   92 */ {I_AND, 2, {REG_AL,IMMEDIATE,0,0,0}, nasm_bytecodes+18668, IF_8086|IF_SM},
    /*   93 */ {I_AND, 2, {REG_AX,SBYTE16,0,0,0}, nasm_bytecodes+13292, IF_8086|IF_SM},
    /*   94 */ {I_AND, 2, {REG_AX,IMMEDIATE,0,0,0}, nasm_bytecodes+16902, IF_8086|IF_SM},
    /*   95 */ {I_AND, 2, {REG_EAX,SBYTE32,0,0,0}, nasm_bytecodes+13298, IF_386|IF_SM},
    /*   96 */ {I_AND, 2, {REG_EAX,IMMEDIATE,0,0,0}, nasm_bytecodes+16907, IF_386|IF_SM},
    /*   97 */ {I_AND, 2, {REG_RAX,SBYTE64,0,0,0}, nasm_bytecodes+13304, IF_X64|IF_SM},
    /*   98 */ {I_AND, 2, {REG_RAX,IMMEDIATE,0,0,0}, nasm_bytecodes+16912, IF_X64|IF_SM},
    /*   99 */ {I_AND, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+16917, IF_8086|IF_SM},
    /*  100 */ {I_AND, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+13310, IF_8086|IF_SM},
    /*  101 */ {I_AND, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+13316, IF_386|IF_SM},
    /*  102 */ {I_AND, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+13322, IF_X64|IF_SM},
    /*  103 */ {I_AND, 2, {MEMORY,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+16917, IF_8086|IF_SM},
    /*  104 */ {I_AND, 2, {MEMORY,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+13310, IF_8086|IF_SM},
    /*  105 */ {I_AND, 2, {MEMORY,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+13316, IF_386|IF_SM},
    /*  106 */ {I_ARPL, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18672, IF_286|IF_PROT|IF_SM|IF_NOLONG},
    /*  107 */ {I_ARPL, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18672, IF_286|IF_PROT|IF_NOLONG},
    /*  108 */ {I_BOUND, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+16922, IF_186|IF_NOLONG},
    /*  109 */ {I_BOUND, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+16927, IF_386|IF_NOLONG},
    /*  110 */ {I_BSF, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+13328, IF_386|IF_SM},
    /*  111 */ {I_BSF, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13328, IF_386},
    /*  112 */ {I_BSF, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+13334, IF_386|IF_SM},
    /*  113 */ {I_BSF, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13334, IF_386},
    /*  114 */ {I_BSF, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+13340, IF_X64|IF_SM},
    /*  115 */ {I_BSF, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13340, IF_X64},
    /*  116 */ {I_BSR, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+13346, IF_386|IF_SM},
    /*  117 */ {I_BSR, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13346, IF_386},
    /*  118 */ {I_BSR, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+13352, IF_386|IF_SM},
    /*  119 */ {I_BSR, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13352, IF_386},
    /*  120 */ {I_BSR, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+13358, IF_X64|IF_SM},
    /*  121 */ {I_BSR, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13358, IF_X64},
    /*  122 */ {I_BSWAP, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+13364, IF_486},
    /*  123 */ {I_BSWAP, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+13370, IF_X64},
    /*  124 */ {I_BT, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13376, IF_386|IF_SM},
    /*  125 */ {I_BT, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13376, IF_386},
    /*  126 */ {I_BT, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13382, IF_386|IF_SM},
    /*  127 */ {I_BT, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13382, IF_386},
    /*  128 */ {I_BT, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13388, IF_X64|IF_SM},
    /*  129 */ {I_BT, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13388, IF_X64},
    /*  130 */ {I_BT, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+6920, IF_386|IF_SB},
    /*  131 */ {I_BT, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+6927, IF_386|IF_SB},
    /*  132 */ {I_BT, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+6934, IF_X64|IF_SB},
    /*  133 */ {I_BTC, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13394, IF_386|IF_SM},
    /*  134 */ {I_BTC, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13394, IF_386},
    /*  135 */ {I_BTC, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13400, IF_386|IF_SM},
    /*  136 */ {I_BTC, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13400, IF_386},
    /*  137 */ {I_BTC, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13406, IF_X64|IF_SM},
    /*  138 */ {I_BTC, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13406, IF_X64},
    /*  139 */ {I_BTC, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+6941, IF_386|IF_SB},
    /*  140 */ {I_BTC, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+6948, IF_386|IF_SB},
    /*  141 */ {I_BTC, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+6955, IF_X64|IF_SB},
    /*  142 */ {I_BTR, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13412, IF_386|IF_SM},
    /*  143 */ {I_BTR, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13412, IF_386},
    /*  144 */ {I_BTR, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13418, IF_386|IF_SM},
    /*  145 */ {I_BTR, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13418, IF_386},
    /*  146 */ {I_BTR, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13424, IF_X64|IF_SM},
    /*  147 */ {I_BTR, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13424, IF_X64},
    /*  148 */ {I_BTR, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+6962, IF_386|IF_SB},
    /*  149 */ {I_BTR, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+6969, IF_386|IF_SB},
    /*  150 */ {I_BTR, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+6976, IF_X64|IF_SB},
    /*  151 */ {I_BTS, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13430, IF_386|IF_SM},
    /*  152 */ {I_BTS, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13430, IF_386},
    /*  153 */ {I_BTS, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13436, IF_386|IF_SM},
    /*  154 */ {I_BTS, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13436, IF_386},
    /*  155 */ {I_BTS, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13442, IF_X64|IF_SM},
    /*  156 */ {I_BTS, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13442, IF_X64},
    /*  157 */ {I_BTS, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+6983, IF_386|IF_SB},
    /*  158 */ {I_BTS, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+6990, IF_386|IF_SB},
    /*  159 */ {I_BTS, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+6997, IF_X64|IF_SB},
    /*  160 */ {I_CALL, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+16932, IF_8086},
    /*  161 */ {I_CALL, 1, {IMMEDIATE|NEAR,0,0,0,0}, nasm_bytecodes+16932, IF_8086},
    /*  162 */ {I_CALL, 1, {IMMEDIATE|BITS16,0,0,0,0}, nasm_bytecodes+16937, IF_8086},
    /*  163 */ {I_CALL, 1, {IMMEDIATE|BITS16|NEAR,0,0,0,0}, nasm_bytecodes+16937, IF_8086},
    /*  164 */ {I_CALL, 1, {IMMEDIATE|BITS32,0,0,0,0}, nasm_bytecodes+16942, IF_386},
    /*  165 */ {I_CALL, 1, {IMMEDIATE|BITS32|NEAR,0,0,0,0}, nasm_bytecodes+16942, IF_386},
    /*  166 */ {I_CALL, 2, {IMMEDIATE|COLON,IMMEDIATE,0,0,0}, nasm_bytecodes+13466, IF_8086|IF_NOLONG},
    /*  167 */ {I_CALL, 2, {IMMEDIATE|BITS16|COLON,IMMEDIATE,0,0,0}, nasm_bytecodes+13472, IF_8086|IF_NOLONG},
    /*  168 */ {I_CALL, 2, {IMMEDIATE|COLON,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+13472, IF_8086|IF_NOLONG},
    /*  169 */ {I_CALL, 2, {IMMEDIATE|BITS32|COLON,IMMEDIATE,0,0,0}, nasm_bytecodes+13478, IF_386|IF_NOLONG},
    /*  170 */ {I_CALL, 2, {IMMEDIATE|COLON,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+13478, IF_386|IF_NOLONG},
    /*  171 */ {I_CALL, 1, {MEMORY|FAR,0,0,0,0}, nasm_bytecodes+16947, IF_8086|IF_NOLONG},
    /*  172 */ {I_CALL, 1, {MEMORY|FAR,0,0,0,0}, nasm_bytecodes+16952, IF_X64},
    /*  173 */ {I_CALL, 1, {MEMORY|BITS16|FAR,0,0,0,0}, nasm_bytecodes+16957, IF_8086},
    /*  174 */ {I_CALL, 1, {MEMORY|BITS32|FAR,0,0,0,0}, nasm_bytecodes+16962, IF_386},
    /*  175 */ {I_CALL, 1, {MEMORY|BITS64|FAR,0,0,0,0}, nasm_bytecodes+16952, IF_X64},
    /*  176 */ {I_CALL, 1, {MEMORY|NEAR,0,0,0,0}, nasm_bytecodes+16967, IF_8086},
    /*  177 */ {I_CALL, 1, {MEMORY|BITS16|NEAR,0,0,0,0}, nasm_bytecodes+16972, IF_8086},
    /*  178 */ {I_CALL, 1, {MEMORY|BITS32|NEAR,0,0,0,0}, nasm_bytecodes+16977, IF_386|IF_NOLONG},
    /*  179 */ {I_CALL, 1, {MEMORY|BITS64|NEAR,0,0,0,0}, nasm_bytecodes+16982, IF_X64},
    /*  180 */ {I_CALL, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16972, IF_8086},
    /*  181 */ {I_CALL, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16977, IF_386|IF_NOLONG},
    /*  182 */ {I_CALL, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16987, IF_X64},
    /*  183 */ {I_CALL, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+16967, IF_8086},
    /*  184 */ {I_CALL, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+16972, IF_8086},
    /*  185 */ {I_CALL, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+16977, IF_386|IF_NOLONG},
    /*  186 */ {I_CALL, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+16987, IF_X64},
    /*  187 */ {I_CBW, 0, {0,0,0,0,0}, nasm_bytecodes+18684, IF_8086},
    /*  188 */ {I_CDQ, 0, {0,0,0,0,0}, nasm_bytecodes+18688, IF_386},
    /*  189 */ {I_CDQE, 0, {0,0,0,0,0}, nasm_bytecodes+18692, IF_X64},
    /*  190 */ {I_CLC, 0, {0,0,0,0,0}, nasm_bytecodes+18424, IF_8086},
    /*  191 */ {I_CLD, 0, {0,0,0,0,0}, nasm_bytecodes+19686, IF_8086},
    /*  192 */ {I_CLGI, 0, {0,0,0,0,0}, nasm_bytecodes+16992, IF_X64|IF_AMD},
    /*  193 */ {I_CLI, 0, {0,0,0,0,0}, nasm_bytecodes+19689, IF_8086},
    /*  194 */ {I_CLTS, 0, {0,0,0,0,0}, nasm_bytecodes+18696, IF_286|IF_PRIV},
    /*  195 */ {I_CMC, 0, {0,0,0,0,0}, nasm_bytecodes+19692, IF_8086},
    /*  196 */ {I_CMP, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+18700, IF_8086|IF_SM},
    /*  197 */ {I_CMP, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+18700, IF_8086},
    /*  198 */ {I_CMP, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16997, IF_8086|IF_SM},
    /*  199 */ {I_CMP, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+16997, IF_8086},
    /*  200 */ {I_CMP, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+17002, IF_386|IF_SM},
    /*  201 */ {I_CMP, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+17002, IF_386},
    /*  202 */ {I_CMP, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+17007, IF_X64|IF_SM},
    /*  203 */ {I_CMP, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+17007, IF_X64},
    /*  204 */ {I_CMP, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+10780, IF_8086|IF_SM},
    /*  205 */ {I_CMP, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+10780, IF_8086},
    /*  206 */ {I_CMP, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+17012, IF_8086|IF_SM},
    /*  207 */ {I_CMP, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+17012, IF_8086},
    /*  208 */ {I_CMP, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+17017, IF_386|IF_SM},
    /*  209 */ {I_CMP, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+17017, IF_386},
    /*  210 */ {I_CMP, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+17022, IF_X64|IF_SM},
    /*  211 */ {I_CMP, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+17022, IF_X64},
    /*  212 */ {I_CMP, 2, {RM_GPR|BITS16,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13484, IF_8086},
    /*  213 */ {I_CMP, 2, {RM_GPR|BITS32,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13490, IF_386},
    /*  214 */ {I_CMP, 2, {RM_GPR|BITS64,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13496, IF_X64},
    /*  215 */ {I_CMP, 2, {REG_AL,IMMEDIATE,0,0,0}, nasm_bytecodes+18704, IF_8086|IF_SM},
    /*  216 */ {I_CMP, 2, {REG_AX,SBYTE16,0,0,0}, nasm_bytecodes+13484, IF_8086|IF_SM},
    /*  217 */ {I_CMP, 2, {REG_AX,IMMEDIATE,0,0,0}, nasm_bytecodes+17027, IF_8086|IF_SM},
    /*  218 */ {I_CMP, 2, {REG_EAX,SBYTE32,0,0,0}, nasm_bytecodes+13490, IF_386|IF_SM},
    /*  219 */ {I_CMP, 2, {REG_EAX,IMMEDIATE,0,0,0}, nasm_bytecodes+17032, IF_386|IF_SM},
    /*  220 */ {I_CMP, 2, {REG_RAX,SBYTE64,0,0,0}, nasm_bytecodes+13496, IF_X64|IF_SM},
    /*  221 */ {I_CMP, 2, {REG_RAX,IMMEDIATE,0,0,0}, nasm_bytecodes+17037, IF_X64|IF_SM},
    /*  222 */ {I_CMP, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+17042, IF_8086|IF_SM},
    /*  223 */ {I_CMP, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+13502, IF_8086|IF_SM},
    /*  224 */ {I_CMP, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+13508, IF_386|IF_SM},
    /*  225 */ {I_CMP, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+13514, IF_X64|IF_SM},
    /*  226 */ {I_CMP, 2, {MEMORY,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+17042, IF_8086|IF_SM},
    /*  227 */ {I_CMP, 2, {MEMORY,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+13502, IF_8086|IF_SM},
    /*  228 */ {I_CMP, 2, {MEMORY,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+13508, IF_386|IF_SM},
    /*  229 */ {I_CMPSB, 0, {0,0,0,0,0}, nasm_bytecodes+18708, IF_8086},
    /*  230 */ {I_CMPSD, 0, {0,0,0,0,0}, nasm_bytecodes+17047, IF_386},
    /*  231 */ {I_CMPSQ, 0, {0,0,0,0,0}, nasm_bytecodes+17052, IF_X64},
    /*  232 */ {I_CMPSW, 0, {0,0,0,0,0}, nasm_bytecodes+17057, IF_8086},
    /*  233 */ {I_CMPXCHG, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+17062, IF_PENT|IF_SM},
    /*  234 */ {I_CMPXCHG, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+17062, IF_PENT},
    /*  235 */ {I_CMPXCHG, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13520, IF_PENT|IF_SM},
    /*  236 */ {I_CMPXCHG, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13520, IF_PENT},
    /*  237 */ {I_CMPXCHG, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13526, IF_PENT|IF_SM},
    /*  238 */ {I_CMPXCHG, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13526, IF_PENT},
    /*  239 */ {I_CMPXCHG, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13532, IF_X64|IF_SM},
    /*  240 */ {I_CMPXCHG, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13532, IF_X64},
    /*  241 */ {I_CMPXCHG8B, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+13551, IF_PENT},
    /*  242 */ {I_CMPXCHG16B, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+13550, IF_X64},
    /*  243 */ {I_CPUID, 0, {0,0,0,0,0}, nasm_bytecodes+18712, IF_PENT},
    /*  244 */ {I_CPU_READ, 0, {0,0,0,0,0}, nasm_bytecodes+18716, IF_PENT|IF_CYRIX},
    /*  245 */ {I_CPU_WRITE, 0, {0,0,0,0,0}, nasm_bytecodes+18720, IF_PENT|IF_CYRIX},
    /*  246 */ {I_CQO, 0, {0,0,0,0,0}, nasm_bytecodes+18724, IF_X64},
    /*  247 */ {I_CWD, 0, {0,0,0,0,0}, nasm_bytecodes+18728, IF_8086},
    /*  248 */ {I_CWDE, 0, {0,0,0,0,0}, nasm_bytecodes+18732, IF_386},
    /*  249 */ {I_DAA, 0, {0,0,0,0,0}, nasm_bytecodes+19695, IF_8086|IF_NOLONG},
    /*  250 */ {I_DAS, 0, {0,0,0,0,0}, nasm_bytecodes+19698, IF_8086|IF_NOLONG},
    /*  251 */ {I_DEC, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+18736, IF_8086|IF_NOLONG},
    /*  252 */ {I_DEC, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+18740, IF_386|IF_NOLONG},
    /*  253 */ {I_DEC, 1, {RM_GPR|BITS8,0,0,0,0}, nasm_bytecodes+18744, IF_8086},
    /*  254 */ {I_DEC, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17072, IF_8086},
    /*  255 */ {I_DEC, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17077, IF_386},
    /*  256 */ {I_DEC, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17082, IF_X64},
    /*  257 */ {I_DIV, 1, {RM_GPR|BITS8,0,0,0,0}, nasm_bytecodes+18748, IF_8086},
    /*  258 */ {I_DIV, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17087, IF_8086},
    /*  259 */ {I_DIV, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17092, IF_386},
    /*  260 */ {I_DIV, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17097, IF_X64},
    /*  261 */ {I_DMINT, 0, {0,0,0,0,0}, nasm_bytecodes+18752, IF_P6|IF_CYRIX},
    /*  262 */ {I_EMMS, 0, {0,0,0,0,0}, nasm_bytecodes+18756, IF_PENT|IF_MMX},
    /*  263 */ {I_ENTER, 2, {IMMEDIATE,IMMEDIATE,0,0,0}, nasm_bytecodes+17102, IF_186},
    /*  264 */ {I_EQU, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+5526, IF_8086},
    /*  265 */ {I_EQU, 2, {IMMEDIATE|COLON,IMMEDIATE,0,0,0}, nasm_bytecodes+5526, IF_8086},
    /*  266 */ {I_F2XM1, 0, {0,0,0,0,0}, nasm_bytecodes+18760, IF_8086|IF_FPU},
    /*  267 */ {I_FABS, 0, {0,0,0,0,0}, nasm_bytecodes+18764, IF_8086|IF_FPU},
    /*  268 */ {I_FADD, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18768, IF_8086|IF_FPU},
    /*  269 */ {I_FADD, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+18772, IF_8086|IF_FPU},
    /*  270 */ {I_FADD, 1, {FPUREG|TO,0,0,0,0}, nasm_bytecodes+17107, IF_8086|IF_FPU},
    /*  271 */ {I_FADD, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17112, IF_8086|IF_FPU},
    /*  272 */ {I_FADD, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17107, IF_8086|IF_FPU},
    /*  273 */ {I_FADD, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17117, IF_8086|IF_FPU},
    /*  274 */ {I_FADDP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17122, IF_8086|IF_FPU},
    /*  275 */ {I_FADDP, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17122, IF_8086|IF_FPU},
    /*  276 */ {I_FBLD, 1, {MEMORY|BITS80,0,0,0,0}, nasm_bytecodes+18780, IF_8086|IF_FPU},
    /*  277 */ {I_FBLD, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+18780, IF_8086|IF_FPU},
    /*  278 */ {I_FBSTP, 1, {MEMORY|BITS80,0,0,0,0}, nasm_bytecodes+18784, IF_8086|IF_FPU},
    /*  279 */ {I_FBSTP, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+18784, IF_8086|IF_FPU},
    /*  280 */ {I_FCHS, 0, {0,0,0,0,0}, nasm_bytecodes+18788, IF_8086|IF_FPU},
    /*  281 */ {I_FCLEX, 0, {0,0,0,0,0}, nasm_bytecodes+17127, IF_8086|IF_FPU},
    /*  282 */ {I_FCMOVB, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17132, IF_P6|IF_FPU},
    /*  283 */ {I_FCMOVB, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17137, IF_P6|IF_FPU},
    /*  284 */ {I_FCMOVBE, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17142, IF_P6|IF_FPU},
    /*  285 */ {I_FCMOVBE, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17147, IF_P6|IF_FPU},
    /*  286 */ {I_FCMOVE, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17152, IF_P6|IF_FPU},
    /*  287 */ {I_FCMOVE, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17157, IF_P6|IF_FPU},
    /*  288 */ {I_FCMOVNB, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17162, IF_P6|IF_FPU},
    /*  289 */ {I_FCMOVNB, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17167, IF_P6|IF_FPU},
    /*  290 */ {I_FCMOVNBE, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17172, IF_P6|IF_FPU},
    /*  291 */ {I_FCMOVNBE, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17177, IF_P6|IF_FPU},
    /*  292 */ {I_FCMOVNE, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17182, IF_P6|IF_FPU},
    /*  293 */ {I_FCMOVNE, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17187, IF_P6|IF_FPU},
    /*  294 */ {I_FCMOVNU, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17192, IF_P6|IF_FPU},
    /*  295 */ {I_FCMOVNU, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17197, IF_P6|IF_FPU},
    /*  296 */ {I_FCMOVU, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17202, IF_P6|IF_FPU},
    /*  297 */ {I_FCMOVU, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17207, IF_P6|IF_FPU},
    /*  298 */ {I_FCOM, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18824, IF_8086|IF_FPU},
    /*  299 */ {I_FCOM, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+18828, IF_8086|IF_FPU},
    /*  300 */ {I_FCOM, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17212, IF_8086|IF_FPU},
    /*  301 */ {I_FCOM, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17217, IF_8086|IF_FPU},
    /*  302 */ {I_FCOMI, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17222, IF_P6|IF_FPU},
    /*  303 */ {I_FCOMI, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17227, IF_P6|IF_FPU},
    /*  304 */ {I_FCOMIP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17232, IF_P6|IF_FPU},
    /*  305 */ {I_FCOMIP, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17237, IF_P6|IF_FPU},
    /*  306 */ {I_FCOMP, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18844, IF_8086|IF_FPU},
    /*  307 */ {I_FCOMP, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+18848, IF_8086|IF_FPU},
    /*  308 */ {I_FCOMP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17242, IF_8086|IF_FPU},
    /*  309 */ {I_FCOMP, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17247, IF_8086|IF_FPU},
    /*  310 */ {I_FCOMPP, 0, {0,0,0,0,0}, nasm_bytecodes+18856, IF_8086|IF_FPU},
    /*  311 */ {I_FCOS, 0, {0,0,0,0,0}, nasm_bytecodes+18860, IF_386|IF_FPU},
    /*  312 */ {I_FDECSTP, 0, {0,0,0,0,0}, nasm_bytecodes+18864, IF_8086|IF_FPU},
    /*  313 */ {I_FDISI, 0, {0,0,0,0,0}, nasm_bytecodes+17252, IF_8086|IF_FPU},
    /*  314 */ {I_FDIV, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18868, IF_8086|IF_FPU},
    /*  315 */ {I_FDIV, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+18872, IF_8086|IF_FPU},
    /*  316 */ {I_FDIV, 1, {FPUREG|TO,0,0,0,0}, nasm_bytecodes+17257, IF_8086|IF_FPU},
    /*  317 */ {I_FDIV, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17262, IF_8086|IF_FPU},
    /*  318 */ {I_FDIV, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17257, IF_8086|IF_FPU},
    /*  319 */ {I_FDIV, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17267, IF_8086|IF_FPU},
    /*  320 */ {I_FDIVP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17272, IF_8086|IF_FPU},
    /*  321 */ {I_FDIVP, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17272, IF_8086|IF_FPU},
    /*  322 */ {I_FDIVR, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18880, IF_8086|IF_FPU},
    /*  323 */ {I_FDIVR, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+18884, IF_8086|IF_FPU},
    /*  324 */ {I_FDIVR, 1, {FPUREG|TO,0,0,0,0}, nasm_bytecodes+17277, IF_8086|IF_FPU},
    /*  325 */ {I_FDIVR, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17277, IF_8086|IF_FPU},
    /*  326 */ {I_FDIVR, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17282, IF_8086|IF_FPU},
    /*  327 */ {I_FDIVR, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17287, IF_8086|IF_FPU},
    /*  328 */ {I_FDIVRP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17292, IF_8086|IF_FPU},
    /*  329 */ {I_FDIVRP, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17292, IF_8086|IF_FPU},
    /*  330 */ {I_FEMMS, 0, {0,0,0,0,0}, nasm_bytecodes+18892, IF_PENT|IF_3DNOW},
    /*  331 */ {I_FENI, 0, {0,0,0,0,0}, nasm_bytecodes+17297, IF_8086|IF_FPU},
    /*  332 */ {I_FFREE, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17302, IF_8086|IF_FPU},
    /*  333 */ {I_FFREE, 0, {0,0,0,0,0}, nasm_bytecodes+18896, IF_8086|IF_FPU},
    /*  334 */ {I_FFREEP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17307, IF_286|IF_FPU|IF_UNDOC},
    /*  335 */ {I_FFREEP, 0, {0,0,0,0,0}, nasm_bytecodes+18900, IF_286|IF_FPU|IF_UNDOC},
    /*  336 */ {I_FIADD, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18904, IF_8086|IF_FPU},
    /*  337 */ {I_FIADD, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18908, IF_8086|IF_FPU},
    /*  338 */ {I_FICOM, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18912, IF_8086|IF_FPU},
    /*  339 */ {I_FICOM, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18916, IF_8086|IF_FPU},
    /*  340 */ {I_FICOMP, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18920, IF_8086|IF_FPU},
    /*  341 */ {I_FICOMP, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18924, IF_8086|IF_FPU},
    /*  342 */ {I_FIDIV, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18928, IF_8086|IF_FPU},
    /*  343 */ {I_FIDIV, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18932, IF_8086|IF_FPU},
    /*  344 */ {I_FIDIVR, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18936, IF_8086|IF_FPU},
    /*  345 */ {I_FIDIVR, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18940, IF_8086|IF_FPU},
    /*  346 */ {I_FILD, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18944, IF_8086|IF_FPU},
    /*  347 */ {I_FILD, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18948, IF_8086|IF_FPU},
    /*  348 */ {I_FILD, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+18952, IF_8086|IF_FPU},
    /*  349 */ {I_FIMUL, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18956, IF_8086|IF_FPU},
    /*  350 */ {I_FIMUL, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18960, IF_8086|IF_FPU},
    /*  351 */ {I_FINCSTP, 0, {0,0,0,0,0}, nasm_bytecodes+18964, IF_8086|IF_FPU},
    /*  352 */ {I_FINIT, 0, {0,0,0,0,0}, nasm_bytecodes+17312, IF_8086|IF_FPU},
    /*  353 */ {I_FIST, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18968, IF_8086|IF_FPU},
    /*  354 */ {I_FIST, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18972, IF_8086|IF_FPU},
    /*  355 */ {I_FISTP, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18976, IF_8086|IF_FPU},
    /*  356 */ {I_FISTP, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18980, IF_8086|IF_FPU},
    /*  357 */ {I_FISTP, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+18984, IF_8086|IF_FPU},
    /*  358 */ {I_FISTTP, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18988, IF_PRESCOTT|IF_FPU},
    /*  359 */ {I_FISTTP, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+18992, IF_PRESCOTT|IF_FPU},
    /*  360 */ {I_FISTTP, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+18996, IF_PRESCOTT|IF_FPU},
    /*  361 */ {I_FISUB, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+19000, IF_8086|IF_FPU},
    /*  362 */ {I_FISUB, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+19004, IF_8086|IF_FPU},
    /*  363 */ {I_FISUBR, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+19008, IF_8086|IF_FPU},
    /*  364 */ {I_FISUBR, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+19012, IF_8086|IF_FPU},
    /*  365 */ {I_FLD, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+19016, IF_8086|IF_FPU},
    /*  366 */ {I_FLD, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+19020, IF_8086|IF_FPU},
    /*  367 */ {I_FLD, 1, {MEMORY|BITS80,0,0,0,0}, nasm_bytecodes+19024, IF_8086|IF_FPU},
    /*  368 */ {I_FLD, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17317, IF_8086|IF_FPU},
    /*  369 */ {I_FLD1, 0, {0,0,0,0,0}, nasm_bytecodes+19032, IF_8086|IF_FPU},
    /*  370 */ {I_FLDCW, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+19036, IF_8086|IF_FPU|IF_SW},
    /*  371 */ {I_FLDENV, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+19040, IF_8086|IF_FPU},
    /*  372 */ {I_FLDL2E, 0, {0,0,0,0,0}, nasm_bytecodes+19044, IF_8086|IF_FPU},
    /*  373 */ {I_FLDL2T, 0, {0,0,0,0,0}, nasm_bytecodes+19048, IF_8086|IF_FPU},
    /*  374 */ {I_FLDLG2, 0, {0,0,0,0,0}, nasm_bytecodes+19052, IF_8086|IF_FPU},
    /*  375 */ {I_FLDLN2, 0, {0,0,0,0,0}, nasm_bytecodes+19056, IF_8086|IF_FPU},
    /*  376 */ {I_FLDPI, 0, {0,0,0,0,0}, nasm_bytecodes+19060, IF_8086|IF_FPU},
    /*  377 */ {I_FLDZ, 0, {0,0,0,0,0}, nasm_bytecodes+19064, IF_8086|IF_FPU},
    /*  378 */ {I_FMUL, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+19068, IF_8086|IF_FPU},
    /*  379 */ {I_FMUL, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+19072, IF_8086|IF_FPU},
    /*  380 */ {I_FMUL, 1, {FPUREG|TO,0,0,0,0}, nasm_bytecodes+17322, IF_8086|IF_FPU},
    /*  381 */ {I_FMUL, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17322, IF_8086|IF_FPU},
    /*  382 */ {I_FMUL, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17327, IF_8086|IF_FPU},
    /*  383 */ {I_FMUL, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17332, IF_8086|IF_FPU},
    /*  384 */ {I_FMULP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17337, IF_8086|IF_FPU},
    /*  385 */ {I_FMULP, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17337, IF_8086|IF_FPU},
    /*  386 */ {I_FNCLEX, 0, {0,0,0,0,0}, nasm_bytecodes+17128, IF_8086|IF_FPU},
    /*  387 */ {I_FNDISI, 0, {0,0,0,0,0}, nasm_bytecodes+17253, IF_8086|IF_FPU},
    /*  388 */ {I_FNENI, 0, {0,0,0,0,0}, nasm_bytecodes+17298, IF_8086|IF_FPU},
    /*  389 */ {I_FNINIT, 0, {0,0,0,0,0}, nasm_bytecodes+17313, IF_8086|IF_FPU},
    /*  390 */ {I_FNOP, 0, {0,0,0,0,0}, nasm_bytecodes+19080, IF_8086|IF_FPU},
    /*  391 */ {I_FNSAVE, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17343, IF_8086|IF_FPU},
    /*  392 */ {I_FNSTCW, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17353, IF_8086|IF_FPU|IF_SW},
    /*  393 */ {I_FNSTENV, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17358, IF_8086|IF_FPU},
    /*  394 */ {I_FNSTSW, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17368, IF_8086|IF_FPU|IF_SW},
    /*  395 */ {I_FNSTSW, 1, {REG_AX,0,0,0,0}, nasm_bytecodes+17373, IF_286|IF_FPU},
    /*  396 */ {I_FPATAN, 0, {0,0,0,0,0}, nasm_bytecodes+19084, IF_8086|IF_FPU},
    /*  397 */ {I_FPREM, 0, {0,0,0,0,0}, nasm_bytecodes+19088, IF_8086|IF_FPU},
    /*  398 */ {I_FPREM1, 0, {0,0,0,0,0}, nasm_bytecodes+19092, IF_386|IF_FPU},
    /*  399 */ {I_FPTAN, 0, {0,0,0,0,0}, nasm_bytecodes+19096, IF_8086|IF_FPU},
    /*  400 */ {I_FRNDINT, 0, {0,0,0,0,0}, nasm_bytecodes+19100, IF_8086|IF_FPU},
    /*  401 */ {I_FRSTOR, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+19104, IF_8086|IF_FPU},
    /*  402 */ {I_FSAVE, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17342, IF_8086|IF_FPU},
    /*  403 */ {I_FSCALE, 0, {0,0,0,0,0}, nasm_bytecodes+19108, IF_8086|IF_FPU},
    /*  404 */ {I_FSETPM, 0, {0,0,0,0,0}, nasm_bytecodes+19112, IF_286|IF_FPU},
    /*  405 */ {I_FSIN, 0, {0,0,0,0,0}, nasm_bytecodes+19116, IF_386|IF_FPU},
    /*  406 */ {I_FSINCOS, 0, {0,0,0,0,0}, nasm_bytecodes+19120, IF_386|IF_FPU},
    /*  407 */ {I_FSQRT, 0, {0,0,0,0,0}, nasm_bytecodes+19124, IF_8086|IF_FPU},
    /*  408 */ {I_FST, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+19128, IF_8086|IF_FPU},
    /*  409 */ {I_FST, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+19132, IF_8086|IF_FPU},
    /*  410 */ {I_FST, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17347, IF_8086|IF_FPU},
    /*  411 */ {I_FSTCW, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17352, IF_8086|IF_FPU|IF_SW},
    /*  412 */ {I_FSTENV, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17357, IF_8086|IF_FPU},
    /*  413 */ {I_FSTP, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+19140, IF_8086|IF_FPU},
    /*  414 */ {I_FSTP, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+19144, IF_8086|IF_FPU},
    /*  415 */ {I_FSTP, 1, {MEMORY|BITS80,0,0,0,0}, nasm_bytecodes+19148, IF_8086|IF_FPU},
    /*  416 */ {I_FSTP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17362, IF_8086|IF_FPU},
    /*  417 */ {I_FSTSW, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17367, IF_8086|IF_FPU|IF_SW},
    /*  418 */ {I_FSTSW, 1, {REG_AX,0,0,0,0}, nasm_bytecodes+17372, IF_286|IF_FPU},
    /*  419 */ {I_FSUB, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+19156, IF_8086|IF_FPU},
    /*  420 */ {I_FSUB, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+19160, IF_8086|IF_FPU},
    /*  421 */ {I_FSUB, 1, {FPUREG|TO,0,0,0,0}, nasm_bytecodes+17377, IF_8086|IF_FPU},
    /*  422 */ {I_FSUB, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17377, IF_8086|IF_FPU},
    /*  423 */ {I_FSUB, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17382, IF_8086|IF_FPU},
    /*  424 */ {I_FSUB, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17387, IF_8086|IF_FPU},
    /*  425 */ {I_FSUBP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17392, IF_8086|IF_FPU},
    /*  426 */ {I_FSUBP, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17392, IF_8086|IF_FPU},
    /*  427 */ {I_FSUBR, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+19168, IF_8086|IF_FPU},
    /*  428 */ {I_FSUBR, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+19172, IF_8086|IF_FPU},
    /*  429 */ {I_FSUBR, 1, {FPUREG|TO,0,0,0,0}, nasm_bytecodes+17397, IF_8086|IF_FPU},
    /*  430 */ {I_FSUBR, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17397, IF_8086|IF_FPU},
    /*  431 */ {I_FSUBR, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17402, IF_8086|IF_FPU},
    /*  432 */ {I_FSUBR, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17407, IF_8086|IF_FPU},
    /*  433 */ {I_FSUBRP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17412, IF_8086|IF_FPU},
    /*  434 */ {I_FSUBRP, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17412, IF_8086|IF_FPU},
    /*  435 */ {I_FTST, 0, {0,0,0,0,0}, nasm_bytecodes+19180, IF_8086|IF_FPU},
    /*  436 */ {I_FUCOM, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17417, IF_386|IF_FPU},
    /*  437 */ {I_FUCOM, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17422, IF_386|IF_FPU},
    /*  438 */ {I_FUCOMI, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17427, IF_P6|IF_FPU},
    /*  439 */ {I_FUCOMI, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17432, IF_P6|IF_FPU},
    /*  440 */ {I_FUCOMIP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17437, IF_P6|IF_FPU},
    /*  441 */ {I_FUCOMIP, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17442, IF_P6|IF_FPU},
    /*  442 */ {I_FUCOMP, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17447, IF_386|IF_FPU},
    /*  443 */ {I_FUCOMP, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17452, IF_386|IF_FPU},
    /*  444 */ {I_FUCOMPP, 0, {0,0,0,0,0}, nasm_bytecodes+19200, IF_386|IF_FPU},
    /*  445 */ {I_FXAM, 0, {0,0,0,0,0}, nasm_bytecodes+19204, IF_8086|IF_FPU},
    /*  446 */ {I_FXCH, 1, {FPUREG,0,0,0,0}, nasm_bytecodes+17457, IF_8086|IF_FPU},
    /*  447 */ {I_FXCH, 2, {FPUREG,FPU0,0,0,0}, nasm_bytecodes+17457, IF_8086|IF_FPU},
    /*  448 */ {I_FXCH, 2, {FPU0,FPUREG,0,0,0}, nasm_bytecodes+17462, IF_8086|IF_FPU},
    /*  449 */ {I_FXTRACT, 0, {0,0,0,0,0}, nasm_bytecodes+19212, IF_8086|IF_FPU},
    /*  450 */ {I_FYL2X, 0, {0,0,0,0,0}, nasm_bytecodes+19216, IF_8086|IF_FPU},
    /*  451 */ {I_FYL2XP1, 0, {0,0,0,0,0}, nasm_bytecodes+19220, IF_8086|IF_FPU},
    /*  452 */ {I_HLT, 0, {0,0,0,0,0}, nasm_bytecodes+19701, IF_8086|IF_PRIV},
    /*  453 */ {I_IDIV, 1, {RM_GPR|BITS8,0,0,0,0}, nasm_bytecodes+19224, IF_8086},
    /*  454 */ {I_IDIV, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17467, IF_8086},
    /*  455 */ {I_IDIV, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17472, IF_386},
    /*  456 */ {I_IDIV, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17477, IF_X64},
    /*  457 */ {I_IMUL, 1, {RM_GPR|BITS8,0,0,0,0}, nasm_bytecodes+19228, IF_8086},
    /*  458 */ {I_IMUL, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17482, IF_8086},
    /*  459 */ {I_IMUL, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17487, IF_386},
    /*  460 */ {I_IMUL, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17492, IF_X64},
    /*  461 */ {I_IMUL, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+13556, IF_386|IF_SM},
    /*  462 */ {I_IMUL, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13556, IF_386},
    /*  463 */ {I_IMUL, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+13562, IF_386|IF_SM},
    /*  464 */ {I_IMUL, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13562, IF_386},
    /*  465 */ {I_IMUL, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+13568, IF_X64|IF_SM},
    /*  466 */ {I_IMUL, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13568, IF_X64},
    /*  467 */ {I_IMUL, 3, {REG_GPR|BITS16,MEMORY,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+13574, IF_186|IF_SM},
    /*  468 */ {I_IMUL, 3, {REG_GPR|BITS16,MEMORY,IMMEDIATE|BITS16,0,0}, nasm_bytecodes+13580, IF_186|IF_SM},
    /*  469 */ {I_IMUL, 3, {REG_GPR|BITS16,REG_GPR|BITS16,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+13574, IF_186},
    /*  470 */ {I_IMUL, 3, {REG_GPR|BITS16,REG_GPR|BITS16,IMMEDIATE|BITS16,0,0}, nasm_bytecodes+13580, IF_186},
    /*  471 */ {I_IMUL, 3, {REG_GPR|BITS32,MEMORY,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+13592, IF_386|IF_SM},
    /*  472 */ {I_IMUL, 3, {REG_GPR|BITS32,MEMORY,IMMEDIATE|BITS32,0,0}, nasm_bytecodes+13598, IF_386|IF_SM},
    /*  473 */ {I_IMUL, 3, {REG_GPR|BITS32,REG_GPR|BITS32,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+13592, IF_386},
    /*  474 */ {I_IMUL, 3, {REG_GPR|BITS32,REG_GPR|BITS32,IMMEDIATE|BITS32,0,0}, nasm_bytecodes+13598, IF_386},
    /*  475 */ {I_IMUL, 3, {REG_GPR|BITS64,MEMORY,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+13610, IF_X64|IF_SM},
    /*  476 */ {I_IMUL, 3, {REG_GPR|BITS64,MEMORY,IMMEDIATE|BITS32,0,0}, nasm_bytecodes+13616, IF_X64|IF_SM},
    /*  477 */ {I_IMUL, 3, {REG_GPR|BITS64,REG_GPR|BITS64,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+13610, IF_X64},
    /*  478 */ {I_IMUL, 3, {REG_GPR|BITS64,REG_GPR|BITS64,IMMEDIATE|BITS32,0,0}, nasm_bytecodes+13616, IF_X64},
    /*  479 */ {I_IMUL, 2, {REG_GPR|BITS16,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13628, IF_186},
    /*  480 */ {I_IMUL, 2, {REG_GPR|BITS16,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+13634, IF_186},
    /*  481 */ {I_IMUL, 2, {REG_GPR|BITS32,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13646, IF_386},
    /*  482 */ {I_IMUL, 2, {REG_GPR|BITS32,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+13652, IF_386},
    /*  483 */ {I_IMUL, 2, {REG_GPR|BITS64,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13664, IF_X64},
    /*  484 */ {I_IMUL, 2, {REG_GPR|BITS64,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+13670, IF_X64},
    /*  485 */ {I_IN, 2, {REG_AL,IMMEDIATE,0,0,0}, nasm_bytecodes+19232, IF_8086|IF_SB},
    /*  486 */ {I_IN, 2, {REG_AX,IMMEDIATE,0,0,0}, nasm_bytecodes+17497, IF_8086|IF_SB},
    /*  487 */ {I_IN, 2, {REG_EAX,IMMEDIATE,0,0,0}, nasm_bytecodes+17502, IF_386|IF_SB},
    /*  488 */ {I_IN, 2, {REG_AL,REG_DX,0,0,0}, nasm_bytecodes+19707, IF_8086},
    /*  489 */ {I_IN, 2, {REG_AX,REG_DX,0,0,0}, nasm_bytecodes+19236, IF_8086},
    /*  490 */ {I_IN, 2, {REG_EAX,REG_DX,0,0,0}, nasm_bytecodes+19240, IF_386},
    /*  491 */ {I_INC, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+19244, IF_8086|IF_NOLONG},
    /*  492 */ {I_INC, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+19248, IF_386|IF_NOLONG},
    /*  493 */ {I_INC, 1, {RM_GPR|BITS8,0,0,0,0}, nasm_bytecodes+19252, IF_8086},
    /*  494 */ {I_INC, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17507, IF_8086},
    /*  495 */ {I_INC, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17512, IF_386},
    /*  496 */ {I_INC, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17517, IF_X64},
    /*  497 */ {I_INSB, 0, {0,0,0,0,0}, nasm_bytecodes+19710, IF_186},
    /*  498 */ {I_INSD, 0, {0,0,0,0,0}, nasm_bytecodes+19256, IF_386},
    /*  499 */ {I_INSW, 0, {0,0,0,0,0}, nasm_bytecodes+19260, IF_186},
    /*  500 */ {I_INT, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+19264, IF_8086|IF_SB},
    /*  501 */ {I_INT1, 0, {0,0,0,0,0}, nasm_bytecodes+19704, IF_386},
    /*  502 */ {I_INT3, 0, {0,0,0,0,0}, nasm_bytecodes+19713, IF_8086},
    /*  503 */ {I_INTO, 0, {0,0,0,0,0}, nasm_bytecodes+19716, IF_8086|IF_NOLONG},
    /*  504 */ {I_INVD, 0, {0,0,0,0,0}, nasm_bytecodes+19268, IF_486|IF_PRIV},
    /*  505 */ {I_INVLPG, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17522, IF_486|IF_PRIV},
    /*  506 */ {I_INVLPGA, 2, {REG_AX,REG_ECX,0,0,0}, nasm_bytecodes+13682, IF_X86_64|IF_AMD|IF_NOLONG},
    /*  507 */ {I_INVLPGA, 2, {REG_EAX,REG_ECX,0,0,0}, nasm_bytecodes+13688, IF_X86_64|IF_AMD},
    /*  508 */ {I_INVLPGA, 2, {REG_RAX,REG_ECX,0,0,0}, nasm_bytecodes+7004, IF_X64|IF_AMD},
    /*  509 */ {I_INVLPGA, 0, {0,0,0,0,0}, nasm_bytecodes+13689, IF_X86_64|IF_AMD},
    /*  510 */ {I_IRET, 0, {0,0,0,0,0}, nasm_bytecodes+19272, IF_8086},
    /*  511 */ {I_IRETD, 0, {0,0,0,0,0}, nasm_bytecodes+19276, IF_386},
    /*  512 */ {I_IRETQ, 0, {0,0,0,0,0}, nasm_bytecodes+19280, IF_X64},
    /*  513 */ {I_IRETW, 0, {0,0,0,0,0}, nasm_bytecodes+19284, IF_8086},
    /*  514 */ {I_JCXZ, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+17527, IF_8086|IF_NOLONG},
    /*  515 */ {I_JECXZ, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+17532, IF_386},
    /*  516 */ {I_JRCXZ, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+17537, IF_X64},
    /*  517 */ {I_JMP, 1, {IMMEDIATE|SHORT,0,0,0,0}, nasm_bytecodes+17543, IF_8086},
    /*  518 */ {I_JMP, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+17547, IF_8086},
    /*  519 */ {I_JMP, 1, {IMMEDIATE|BITS16,0,0,0,0}, nasm_bytecodes+17552, IF_8086},
    /*  520 */ {I_JMP, 1, {IMMEDIATE|BITS32,0,0,0,0}, nasm_bytecodes+17557, IF_386},
    /*  521 */ {I_JMP, 2, {IMMEDIATE|COLON,IMMEDIATE,0,0,0}, nasm_bytecodes+13712, IF_8086|IF_NOLONG},
    /*  522 */ {I_JMP, 2, {IMMEDIATE|BITS16|COLON,IMMEDIATE,0,0,0}, nasm_bytecodes+13718, IF_8086|IF_NOLONG},
    /*  523 */ {I_JMP, 2, {IMMEDIATE|COLON,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+13718, IF_8086|IF_NOLONG},
    /*  524 */ {I_JMP, 2, {IMMEDIATE|BITS32|COLON,IMMEDIATE,0,0,0}, nasm_bytecodes+13724, IF_386|IF_NOLONG},
    /*  525 */ {I_JMP, 2, {IMMEDIATE|COLON,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+13724, IF_386|IF_NOLONG},
    /*  526 */ {I_JMP, 1, {MEMORY|FAR,0,0,0,0}, nasm_bytecodes+17562, IF_8086|IF_NOLONG},
    /*  527 */ {I_JMP, 1, {MEMORY|FAR,0,0,0,0}, nasm_bytecodes+17567, IF_X64},
    /*  528 */ {I_JMP, 1, {MEMORY|BITS16|FAR,0,0,0,0}, nasm_bytecodes+17572, IF_8086},
    /*  529 */ {I_JMP, 1, {MEMORY|BITS32|FAR,0,0,0,0}, nasm_bytecodes+17577, IF_386},
    /*  530 */ {I_JMP, 1, {MEMORY|BITS64|FAR,0,0,0,0}, nasm_bytecodes+17567, IF_X64},
    /*  531 */ {I_JMP, 1, {MEMORY|NEAR,0,0,0,0}, nasm_bytecodes+17582, IF_8086},
    /*  532 */ {I_JMP, 1, {MEMORY|BITS16|NEAR,0,0,0,0}, nasm_bytecodes+17587, IF_8086},
    /*  533 */ {I_JMP, 1, {MEMORY|BITS32|NEAR,0,0,0,0}, nasm_bytecodes+17592, IF_386|IF_NOLONG},
    /*  534 */ {I_JMP, 1, {MEMORY|BITS64|NEAR,0,0,0,0}, nasm_bytecodes+17597, IF_X64},
    /*  535 */ {I_JMP, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17587, IF_8086},
    /*  536 */ {I_JMP, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17592, IF_386|IF_NOLONG},
    /*  537 */ {I_JMP, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17597, IF_X64},
    /*  538 */ {I_JMP, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17582, IF_8086},
    /*  539 */ {I_JMP, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+17587, IF_8086},
    /*  540 */ {I_JMP, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+17592, IF_386|IF_NOLONG},
    /*  541 */ {I_JMP, 1, {MEMORY|BITS64,0,0,0,0}, nasm_bytecodes+17597, IF_X64},
    /*  542 */ {I_JMPE, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+13730, IF_IA64},
    /*  543 */ {I_JMPE, 1, {IMMEDIATE|BITS16,0,0,0,0}, nasm_bytecodes+13736, IF_IA64},
    /*  544 */ {I_JMPE, 1, {IMMEDIATE|BITS32,0,0,0,0}, nasm_bytecodes+13742, IF_IA64},
    /*  545 */ {I_JMPE, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+13748, IF_IA64},
    /*  546 */ {I_JMPE, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+13754, IF_IA64},
    /*  547 */ {I_LAHF, 0, {0,0,0,0,0}, nasm_bytecodes+19719, IF_8086},
    /*  548 */ {I_LAR, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+13760, IF_286|IF_PROT|IF_SW},
    /*  549 */ {I_LAR, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13760, IF_286|IF_PROT},
    /*  550 */ {I_LAR, 2, {REG_GPR|BITS16,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13760, IF_386|IF_PROT},
    /*  551 */ {I_LAR, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+13766, IF_386|IF_PROT|IF_SW},
    /*  552 */ {I_LAR, 2, {REG_GPR|BITS32,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13766, IF_386|IF_PROT},
    /*  553 */ {I_LAR, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13766, IF_386|IF_PROT},
    /*  554 */ {I_LAR, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+13772, IF_X64|IF_PROT|IF_SW},
    /*  555 */ {I_LAR, 2, {REG_GPR|BITS64,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13772, IF_X64|IF_PROT},
    /*  556 */ {I_LAR, 2, {REG_GPR|BITS64,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13772, IF_X64|IF_PROT},
    /*  557 */ {I_LAR, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13772, IF_X64|IF_PROT},
    /*  558 */ {I_LDS, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+17602, IF_8086|IF_NOLONG},
    /*  559 */ {I_LDS, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+17607, IF_386|IF_NOLONG},
    /*  560 */ {I_LEA, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+17612, IF_8086},
    /*  561 */ {I_LEA, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+17617, IF_386},
    /*  562 */ {I_LEA, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+17622, IF_X64},
    /*  563 */ {I_LEAVE, 0, {0,0,0,0,0}, nasm_bytecodes+17859, IF_186},
    /*  564 */ {I_LES, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+17627, IF_8086|IF_NOLONG},
    /*  565 */ {I_LES, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+17632, IF_386|IF_NOLONG},
    /*  566 */ {I_LFENCE, 0, {0,0,0,0,0}, nasm_bytecodes+17637, IF_X64|IF_AMD},
    /*  567 */ {I_LFS, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+13778, IF_386},
    /*  568 */ {I_LFS, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+13784, IF_386},
    /*  569 */ {I_LFS, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+13790, IF_X64},
    /*  570 */ {I_LGDT, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17642, IF_286|IF_PRIV},
    /*  571 */ {I_LGS, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+13796, IF_386},
    /*  572 */ {I_LGS, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+13802, IF_386},
    /*  573 */ {I_LGS, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+13808, IF_X64},
    /*  574 */ {I_LIDT, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17647, IF_286|IF_PRIV},
    /*  575 */ {I_LLDT, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17652, IF_286|IF_PROT|IF_PRIV},
    /*  576 */ {I_LLDT, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+17652, IF_286|IF_PROT|IF_PRIV},
    /*  577 */ {I_LLDT, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17652, IF_286|IF_PROT|IF_PRIV},
    /*  578 */ {I_LMSW, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17657, IF_286|IF_PRIV},
    /*  579 */ {I_LMSW, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+17657, IF_286|IF_PRIV},
    /*  580 */ {I_LMSW, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17657, IF_286|IF_PRIV},
    /*  581 */ {I_LOADALL, 0, {0,0,0,0,0}, nasm_bytecodes+19288, IF_386|IF_UNDOC},
    /*  582 */ {I_LOADALL286, 0, {0,0,0,0,0}, nasm_bytecodes+19292, IF_286|IF_UNDOC},
    /*  583 */ {I_LODSB, 0, {0,0,0,0,0}, nasm_bytecodes+19722, IF_8086},
    /*  584 */ {I_LODSD, 0, {0,0,0,0,0}, nasm_bytecodes+19296, IF_386},
    /*  585 */ {I_LODSQ, 0, {0,0,0,0,0}, nasm_bytecodes+19300, IF_X64},
    /*  586 */ {I_LODSW, 0, {0,0,0,0,0}, nasm_bytecodes+19304, IF_8086},
    /*  587 */ {I_LOOP, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+17662, IF_8086},
    /*  588 */ {I_LOOP, 2, {IMMEDIATE,REG_CX,0,0,0}, nasm_bytecodes+17667, IF_8086|IF_NOLONG},
    /*  589 */ {I_LOOP, 2, {IMMEDIATE,REG_ECX,0,0,0}, nasm_bytecodes+17672, IF_386},
    /*  590 */ {I_LOOP, 2, {IMMEDIATE,REG_RCX,0,0,0}, nasm_bytecodes+17677, IF_X64},
    /*  591 */ {I_LOOPE, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+17682, IF_8086},
    /*  592 */ {I_LOOPE, 2, {IMMEDIATE,REG_CX,0,0,0}, nasm_bytecodes+17687, IF_8086|IF_NOLONG},
    /*  593 */ {I_LOOPE, 2, {IMMEDIATE,REG_ECX,0,0,0}, nasm_bytecodes+17692, IF_386},
    /*  594 */ {I_LOOPE, 2, {IMMEDIATE,REG_RCX,0,0,0}, nasm_bytecodes+17697, IF_X64},
    /*  595 */ {I_LOOPNE, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+17702, IF_8086},
    /*  596 */ {I_LOOPNE, 2, {IMMEDIATE,REG_CX,0,0,0}, nasm_bytecodes+17707, IF_8086|IF_NOLONG},
    /*  597 */ {I_LOOPNE, 2, {IMMEDIATE,REG_ECX,0,0,0}, nasm_bytecodes+17712, IF_386},
    /*  598 */ {I_LOOPNE, 2, {IMMEDIATE,REG_RCX,0,0,0}, nasm_bytecodes+17717, IF_X64},
    /*  599 */ {I_LOOPNZ, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+17702, IF_8086},
    /*  600 */ {I_LOOPNZ, 2, {IMMEDIATE,REG_CX,0,0,0}, nasm_bytecodes+17707, IF_8086|IF_NOLONG},
    /*  601 */ {I_LOOPNZ, 2, {IMMEDIATE,REG_ECX,0,0,0}, nasm_bytecodes+17712, IF_386},
    /*  602 */ {I_LOOPNZ, 2, {IMMEDIATE,REG_RCX,0,0,0}, nasm_bytecodes+17717, IF_X64},
    /*  603 */ {I_LOOPZ, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+17682, IF_8086},
    /*  604 */ {I_LOOPZ, 2, {IMMEDIATE,REG_CX,0,0,0}, nasm_bytecodes+17687, IF_8086|IF_NOLONG},
    /*  605 */ {I_LOOPZ, 2, {IMMEDIATE,REG_ECX,0,0,0}, nasm_bytecodes+17692, IF_386},
    /*  606 */ {I_LOOPZ, 2, {IMMEDIATE,REG_RCX,0,0,0}, nasm_bytecodes+17697, IF_X64},
    /*  607 */ {I_LSL, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+13814, IF_286|IF_PROT|IF_SW},
    /*  608 */ {I_LSL, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13814, IF_286|IF_PROT},
    /*  609 */ {I_LSL, 2, {REG_GPR|BITS16,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13814, IF_386|IF_PROT},
    /*  610 */ {I_LSL, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+13820, IF_386|IF_PROT|IF_SW},
    /*  611 */ {I_LSL, 2, {REG_GPR|BITS32,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13820, IF_386|IF_PROT},
    /*  612 */ {I_LSL, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13820, IF_386|IF_PROT},
    /*  613 */ {I_LSL, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+13826, IF_X64|IF_PROT|IF_SW},
    /*  614 */ {I_LSL, 2, {REG_GPR|BITS64,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+13826, IF_X64|IF_PROT},
    /*  615 */ {I_LSL, 2, {REG_GPR|BITS64,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13826, IF_X64|IF_PROT},
    /*  616 */ {I_LSL, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13826, IF_X64|IF_PROT},
    /*  617 */ {I_LSS, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+13832, IF_386},
    /*  618 */ {I_LSS, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+13838, IF_386},
    /*  619 */ {I_LSS, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+13844, IF_X64},
    /*  620 */ {I_LTR, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17722, IF_286|IF_PROT|IF_PRIV},
    /*  621 */ {I_LTR, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+17722, IF_286|IF_PROT|IF_PRIV},
    /*  622 */ {I_LTR, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17722, IF_286|IF_PROT|IF_PRIV},
    /*  623 */ {I_MFENCE, 0, {0,0,0,0,0}, nasm_bytecodes+17727, IF_X64|IF_AMD},
    /*  624 */ {I_MONITOR, 0, {0,0,0,0,0}, nasm_bytecodes+17732, IF_PRESCOTT},
    /*  625 */ {I_MOV, 2, {MEMORY,REG_SREG,0,0,0}, nasm_bytecodes+17743, IF_8086|IF_SM},
    /*  626 */ {I_MOV, 2, {REG_GPR|BITS16,REG_SREG,0,0,0}, nasm_bytecodes+17737, IF_8086},
    /*  627 */ {I_MOV, 2, {REG_GPR|BITS32,REG_SREG,0,0,0}, nasm_bytecodes+17742, IF_386},
    /*  628 */ {I_MOV, 2, {REG_SREG,MEMORY,0,0,0}, nasm_bytecodes+19308, IF_8086|IF_SM},
    /*  629 */ {I_MOV, 2, {REG_SREG,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+19308, IF_8086},
    /*  630 */ {I_MOV, 2, {REG_SREG,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+19308, IF_386},
    /*  631 */ {I_MOV, 2, {REG_AL,MEM_OFFS,0,0,0}, nasm_bytecodes+19312, IF_8086|IF_SM},
    /*  632 */ {I_MOV, 2, {REG_AX,MEM_OFFS,0,0,0}, nasm_bytecodes+17747, IF_8086|IF_SM},
    /*  633 */ {I_MOV, 2, {REG_EAX,MEM_OFFS,0,0,0}, nasm_bytecodes+17752, IF_386|IF_SM},
    /*  634 */ {I_MOV, 2, {REG_RAX,MEM_OFFS,0,0,0}, nasm_bytecodes+17757, IF_X64|IF_SM},
    /*  635 */ {I_MOV, 2, {MEM_OFFS,REG_AL,0,0,0}, nasm_bytecodes+19316, IF_8086|IF_SM},
    /*  636 */ {I_MOV, 2, {MEM_OFFS,REG_AX,0,0,0}, nasm_bytecodes+17762, IF_8086|IF_SM},
    /*  637 */ {I_MOV, 2, {MEM_OFFS,REG_EAX,0,0,0}, nasm_bytecodes+17767, IF_386|IF_SM},
    /*  638 */ {I_MOV, 2, {MEM_OFFS,REG_RAX,0,0,0}, nasm_bytecodes+17772, IF_X64|IF_SM},
    /*  639 */ {I_MOV, 2, {REG_GPR|BITS32,REG_CREG,0,0,0}, nasm_bytecodes+13850, IF_386|IF_PRIV|IF_NOLONG},
    /*  640 */ {I_MOV, 2, {REG_GPR|BITS64,REG_CREG,0,0,0}, nasm_bytecodes+13856, IF_X64|IF_PRIV},
    /*  641 */ {I_MOV, 2, {REG_CREG,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13862, IF_386|IF_PRIV|IF_NOLONG},
    /*  642 */ {I_MOV, 2, {REG_CREG,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13868, IF_X64|IF_PRIV},
    /*  643 */ {I_MOV, 2, {REG_GPR|BITS32,REG_DREG,0,0,0}, nasm_bytecodes+13875, IF_386|IF_PRIV|IF_NOLONG},
    /*  644 */ {I_MOV, 2, {REG_GPR|BITS64,REG_DREG,0,0,0}, nasm_bytecodes+13874, IF_X64|IF_PRIV},
    /*  645 */ {I_MOV, 2, {REG_DREG,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13881, IF_386|IF_PRIV|IF_NOLONG},
    /*  646 */ {I_MOV, 2, {REG_DREG,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+13880, IF_X64|IF_PRIV},
    /*  647 */ {I_MOV, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19320, IF_8086|IF_SM},
    /*  648 */ {I_MOV, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19320, IF_8086},
    /*  649 */ {I_MOV, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+17787, IF_8086|IF_SM},
    /*  650 */ {I_MOV, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+17787, IF_8086},
    /*  651 */ {I_MOV, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+17792, IF_386|IF_SM},
    /*  652 */ {I_MOV, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+17792, IF_386},
    /*  653 */ {I_MOV, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+17797, IF_X64|IF_SM},
    /*  654 */ {I_MOV, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+17797, IF_X64},
    /*  655 */ {I_MOV, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+19324, IF_8086|IF_SM},
    /*  656 */ {I_MOV, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19324, IF_8086},
    /*  657 */ {I_MOV, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+17802, IF_8086|IF_SM},
    /*  658 */ {I_MOV, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+17802, IF_8086},
    /*  659 */ {I_MOV, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+17807, IF_386|IF_SM},
    /*  660 */ {I_MOV, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+17807, IF_386},
    /*  661 */ {I_MOV, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+17812, IF_X64|IF_SM},
    /*  662 */ {I_MOV, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+17812, IF_X64},
    /*  663 */ {I_MOV, 2, {REG_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+19328, IF_8086|IF_SM},
    /*  664 */ {I_MOV, 2, {REG_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+17817, IF_8086|IF_SM},
    /*  665 */ {I_MOV, 2, {REG_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+17822, IF_386|IF_SM},
    /*  666 */ {I_MOV, 2, {REG_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+17827, IF_X64|IF_SM},
    /*  667 */ {I_MOV, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+17832, IF_8086|IF_SM},
    /*  668 */ {I_MOV, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+13886, IF_8086|IF_SM},
    /*  669 */ {I_MOV, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+13892, IF_386|IF_SM},
    /*  670 */ {I_MOV, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+13898, IF_X64|IF_SM},
    /*  671 */ {I_MOV, 2, {RM_GPR|BITS64,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+13898, IF_X64},
    /*  672 */ {I_MOV, 2, {MEMORY,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+17832, IF_8086|IF_SM},
    /*  673 */ {I_MOV, 2, {MEMORY,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+13886, IF_8086|IF_SM},
    /*  674 */ {I_MOV, 2, {MEMORY,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+13892, IF_386|IF_SM},
    /*  675 */ {I_MOVD, 2, {MMXREG,MEMORY,0,0,0}, nasm_bytecodes+13904, IF_PENT|IF_MMX|IF_SD},
    /*  676 */ {I_MOVD, 2, {MMXREG,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+13904, IF_PENT|IF_MMX},
    /*  677 */ {I_MOVD, 2, {MEMORY,MMXREG,0,0,0}, nasm_bytecodes+13910, IF_PENT|IF_MMX|IF_SD},
    /*  678 */ {I_MOVD, 2, {REG_GPR|BITS32,MMXREG,0,0,0}, nasm_bytecodes+13910, IF_PENT|IF_MMX},
    /*  679 */ {I_MOVD, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+7039, IF_X64|IF_SD},
    /*  680 */ {I_MOVD, 2, {XMMREG,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+7039, IF_X64},
    /*  681 */ {I_MOVD, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+7046, IF_X64|IF_SD},
    /*  682 */ {I_MOVD, 2, {REG_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+7046, IF_X64|IF_SSE},
    /*  683 */ {I_MOVQ, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7053, IF_PENT|IF_MMX|IF_SQ},
    /*  684 */ {I_MOVQ, 2, {RM_MMX,MMXREG,0,0,0}, nasm_bytecodes+7060, IF_PENT|IF_MMX|IF_SQ},
    /*  685 */ {I_MOVQ, 2, {MMXREG,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+13904, IF_X64|IF_MMX},
    /*  686 */ {I_MOVQ, 2, {RM_GPR|BITS64,MMXREG,0,0,0}, nasm_bytecodes+13910, IF_X64|IF_MMX},
    /*  687 */ {I_MOVSB, 0, {0,0,0,0,0}, nasm_bytecodes+5245, IF_8086},
    /*  688 */ {I_MOVSD, 0, {0,0,0,0,0}, nasm_bytecodes+19332, IF_386},
    /*  689 */ {I_MOVSQ, 0, {0,0,0,0,0}, nasm_bytecodes+19336, IF_X64},
    /*  690 */ {I_MOVSW, 0, {0,0,0,0,0}, nasm_bytecodes+19340, IF_8086},
    /*  691 */ {I_MOVSX, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+13916, IF_386|IF_SB},
    /*  692 */ {I_MOVSX, 2, {REG_GPR|BITS16,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+13916, IF_386},
    /*  693 */ {I_MOVSX, 2, {REG_GPR|BITS32,RM_GPR|BITS8,0,0,0}, nasm_bytecodes+13922, IF_386},
    /*  694 */ {I_MOVSX, 2, {REG_GPR|BITS32,RM_GPR|BITS16,0,0,0}, nasm_bytecodes+13928, IF_386},
    /*  695 */ {I_MOVSX, 2, {REG_GPR|BITS64,RM_GPR|BITS8,0,0,0}, nasm_bytecodes+13934, IF_X64},
    /*  696 */ {I_MOVSX, 2, {REG_GPR|BITS64,RM_GPR|BITS16,0,0,0}, nasm_bytecodes+13940, IF_X64},
    /*  697 */ {I_MOVSXD, 2, {REG_GPR|BITS64,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+17837, IF_X64},
    /*  698 */ {I_MOVZX, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+13946, IF_386|IF_SB},
    /*  699 */ {I_MOVZX, 2, {REG_GPR|BITS16,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+13946, IF_386},
    /*  700 */ {I_MOVZX, 2, {REG_GPR|BITS32,RM_GPR|BITS8,0,0,0}, nasm_bytecodes+13952, IF_386},
    /*  701 */ {I_MOVZX, 2, {REG_GPR|BITS32,RM_GPR|BITS16,0,0,0}, nasm_bytecodes+13958, IF_386},
    /*  702 */ {I_MOVZX, 2, {REG_GPR|BITS64,RM_GPR|BITS8,0,0,0}, nasm_bytecodes+13964, IF_X64},
    /*  703 */ {I_MOVZX, 2, {REG_GPR|BITS64,RM_GPR|BITS16,0,0,0}, nasm_bytecodes+13970, IF_X64},
    /*  704 */ {I_MUL, 1, {RM_GPR|BITS8,0,0,0,0}, nasm_bytecodes+19344, IF_8086},
    /*  705 */ {I_MUL, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17842, IF_8086},
    /*  706 */ {I_MUL, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17847, IF_386},
    /*  707 */ {I_MUL, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17852, IF_X64},
    /*  708 */ {I_MWAIT, 0, {0,0,0,0,0}, nasm_bytecodes+17857, IF_PRESCOTT},
    /*  709 */ {I_NEG, 1, {RM_GPR|BITS8,0,0,0,0}, nasm_bytecodes+19348, IF_8086},
    /*  710 */ {I_NEG, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17862, IF_8086},
    /*  711 */ {I_NEG, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17867, IF_386},
    /*  712 */ {I_NEG, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17872, IF_X64},
    /*  713 */ {I_NOP, 0, {0,0,0,0,0}, nasm_bytecodes+19352, IF_8086},
    /*  714 */ {I_NOP, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+13976, IF_P6},
    /*  715 */ {I_NOP, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+13982, IF_P6},
    /*  716 */ {I_NOP, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+13988, IF_X64},
    /*  717 */ {I_NOT, 1, {RM_GPR|BITS8,0,0,0,0}, nasm_bytecodes+19356, IF_8086},
    /*  718 */ {I_NOT, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17877, IF_8086},
    /*  719 */ {I_NOT, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17882, IF_386},
    /*  720 */ {I_NOT, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17887, IF_X64},
    /*  721 */ {I_OR, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19360, IF_8086|IF_SM},
    /*  722 */ {I_OR, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19360, IF_8086},
    /*  723 */ {I_OR, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+17892, IF_8086|IF_SM},
    /*  724 */ {I_OR, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+17892, IF_8086},
    /*  725 */ {I_OR, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+17897, IF_386|IF_SM},
    /*  726 */ {I_OR, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+17897, IF_386},
    /*  727 */ {I_OR, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+17902, IF_X64|IF_SM},
    /*  728 */ {I_OR, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+17902, IF_X64},
    /*  729 */ {I_OR, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+11067, IF_8086|IF_SM},
    /*  730 */ {I_OR, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+11067, IF_8086},
    /*  731 */ {I_OR, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+17907, IF_8086|IF_SM},
    /*  732 */ {I_OR, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+17907, IF_8086},
    /*  733 */ {I_OR, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+17912, IF_386|IF_SM},
    /*  734 */ {I_OR, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+17912, IF_386},
    /*  735 */ {I_OR, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+17917, IF_X64|IF_SM},
    /*  736 */ {I_OR, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+17917, IF_X64},
    /*  737 */ {I_OR, 2, {RM_GPR|BITS16,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+13994, IF_8086},
    /*  738 */ {I_OR, 2, {RM_GPR|BITS32,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14000, IF_386},
    /*  739 */ {I_OR, 2, {RM_GPR|BITS64,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14006, IF_X64},
    /*  740 */ {I_OR, 2, {REG_AL,IMMEDIATE,0,0,0}, nasm_bytecodes+19364, IF_8086|IF_SM},
    /*  741 */ {I_OR, 2, {REG_AX,SBYTE16,0,0,0}, nasm_bytecodes+13994, IF_8086|IF_SM},
    /*  742 */ {I_OR, 2, {REG_AX,IMMEDIATE,0,0,0}, nasm_bytecodes+17922, IF_8086|IF_SM},
    /*  743 */ {I_OR, 2, {REG_EAX,SBYTE32,0,0,0}, nasm_bytecodes+14000, IF_386|IF_SM},
    /*  744 */ {I_OR, 2, {REG_EAX,IMMEDIATE,0,0,0}, nasm_bytecodes+17927, IF_386|IF_SM},
    /*  745 */ {I_OR, 2, {REG_RAX,SBYTE64,0,0,0}, nasm_bytecodes+14006, IF_X64|IF_SM},
    /*  746 */ {I_OR, 2, {REG_RAX,IMMEDIATE,0,0,0}, nasm_bytecodes+17932, IF_X64|IF_SM},
    /*  747 */ {I_OR, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+17937, IF_8086|IF_SM},
    /*  748 */ {I_OR, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14012, IF_8086|IF_SM},
    /*  749 */ {I_OR, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14018, IF_386|IF_SM},
    /*  750 */ {I_OR, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14024, IF_X64|IF_SM},
    /*  751 */ {I_OR, 2, {MEMORY,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+17937, IF_8086|IF_SM},
    /*  752 */ {I_OR, 2, {MEMORY,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+14012, IF_8086|IF_SM},
    /*  753 */ {I_OR, 2, {MEMORY,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+14018, IF_386|IF_SM},
    /*  754 */ {I_OUT, 2, {IMMEDIATE,REG_AL,0,0,0}, nasm_bytecodes+19368, IF_8086|IF_SB},
    /*  755 */ {I_OUT, 2, {IMMEDIATE,REG_AX,0,0,0}, nasm_bytecodes+17942, IF_8086|IF_SB},
    /*  756 */ {I_OUT, 2, {IMMEDIATE,REG_EAX,0,0,0}, nasm_bytecodes+17947, IF_386|IF_SB},
    /*  757 */ {I_OUT, 2, {REG_DX,REG_AL,0,0,0}, nasm_bytecodes+19725, IF_8086},
    /*  758 */ {I_OUT, 2, {REG_DX,REG_AX,0,0,0}, nasm_bytecodes+19372, IF_8086},
    /*  759 */ {I_OUT, 2, {REG_DX,REG_EAX,0,0,0}, nasm_bytecodes+19376, IF_386},
    /*  760 */ {I_OUTSB, 0, {0,0,0,0,0}, nasm_bytecodes+19728, IF_186},
    /*  761 */ {I_OUTSD, 0, {0,0,0,0,0}, nasm_bytecodes+19380, IF_386},
    /*  762 */ {I_OUTSW, 0, {0,0,0,0,0}, nasm_bytecodes+19384, IF_186},
    /*  763 */ {I_PACKSSDW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7067, IF_PENT|IF_MMX|IF_SQ},
    /*  764 */ {I_PACKSSWB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7074, IF_PENT|IF_MMX|IF_SQ},
    /*  765 */ {I_PACKUSWB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7081, IF_PENT|IF_MMX|IF_SQ},
    /*  766 */ {I_PADDB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7088, IF_PENT|IF_MMX|IF_SQ},
    /*  767 */ {I_PADDD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7095, IF_PENT|IF_MMX|IF_SQ},
    /*  768 */ {I_PADDSB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7102, IF_PENT|IF_MMX|IF_SQ},
    /*  769 */ {I_PADDSIW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+14030, IF_PENT|IF_MMX|IF_SQ|IF_CYRIX},
    /*  770 */ {I_PADDSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7109, IF_PENT|IF_MMX|IF_SQ},
    /*  771 */ {I_PADDUSB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7116, IF_PENT|IF_MMX|IF_SQ},
    /*  772 */ {I_PADDUSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7123, IF_PENT|IF_MMX|IF_SQ},
    /*  773 */ {I_PADDW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7130, IF_PENT|IF_MMX|IF_SQ},
    /*  774 */ {I_PAND, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7137, IF_PENT|IF_MMX|IF_SQ},
    /*  775 */ {I_PANDN, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7144, IF_PENT|IF_MMX|IF_SQ},
    /*  776 */ {I_PAUSE, 0, {0,0,0,0,0}, nasm_bytecodes+17952, IF_8086},
    /*  777 */ {I_PAVEB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+14036, IF_PENT|IF_MMX|IF_SQ|IF_CYRIX},
    /*  778 */ {I_PAVGUSB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5184, IF_PENT|IF_3DNOW|IF_SQ},
    /*  779 */ {I_PCMPEQB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7151, IF_PENT|IF_MMX|IF_SQ},
    /*  780 */ {I_PCMPEQD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7158, IF_PENT|IF_MMX|IF_SQ},
    /*  781 */ {I_PCMPEQW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7165, IF_PENT|IF_MMX|IF_SQ},
    /*  782 */ {I_PCMPGTB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7172, IF_PENT|IF_MMX|IF_SQ},
    /*  783 */ {I_PCMPGTD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7179, IF_PENT|IF_MMX|IF_SQ},
    /*  784 */ {I_PCMPGTW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7186, IF_PENT|IF_MMX|IF_SQ},
    /*  785 */ {I_PDISTIB, 2, {MMXREG,MEMORY,0,0,0}, nasm_bytecodes+15207, IF_PENT|IF_MMX|IF_SM|IF_CYRIX},
    /*  786 */ {I_PF2ID, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5192, IF_PENT|IF_3DNOW|IF_SQ},
    /*  787 */ {I_PFACC, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5200, IF_PENT|IF_3DNOW|IF_SQ},
    /*  788 */ {I_PFADD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5208, IF_PENT|IF_3DNOW|IF_SQ},
    /*  789 */ {I_PFCMPEQ, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5216, IF_PENT|IF_3DNOW|IF_SQ},
    /*  790 */ {I_PFCMPGE, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5224, IF_PENT|IF_3DNOW|IF_SQ},
    /*  791 */ {I_PFCMPGT, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5232, IF_PENT|IF_3DNOW|IF_SQ},
    /*  792 */ {I_PFMAX, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5240, IF_PENT|IF_3DNOW|IF_SQ},
    /*  793 */ {I_PFMIN, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5248, IF_PENT|IF_3DNOW|IF_SQ},
    /*  794 */ {I_PFMUL, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5256, IF_PENT|IF_3DNOW|IF_SQ},
    /*  795 */ {I_PFRCP, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5264, IF_PENT|IF_3DNOW|IF_SQ},
    /*  796 */ {I_PFRCPIT1, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5272, IF_PENT|IF_3DNOW|IF_SQ},
    /*  797 */ {I_PFRCPIT2, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5280, IF_PENT|IF_3DNOW|IF_SQ},
    /*  798 */ {I_PFRSQIT1, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5288, IF_PENT|IF_3DNOW|IF_SQ},
    /*  799 */ {I_PFRSQRT, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5296, IF_PENT|IF_3DNOW|IF_SQ},
    /*  800 */ {I_PFSUB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5304, IF_PENT|IF_3DNOW|IF_SQ},
    /*  801 */ {I_PFSUBR, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5312, IF_PENT|IF_3DNOW|IF_SQ},
    /*  802 */ {I_PI2FD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5320, IF_PENT|IF_3DNOW|IF_SQ},
    /*  803 */ {I_PMACHRIW, 2, {MMXREG,MEMORY,0,0,0}, nasm_bytecodes+15303, IF_PENT|IF_MMX|IF_SM|IF_CYRIX},
    /*  804 */ {I_PMADDWD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7193, IF_PENT|IF_MMX|IF_SQ},
    /*  805 */ {I_PMAGW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+14042, IF_PENT|IF_MMX|IF_SQ|IF_CYRIX},
    /*  806 */ {I_PMULHRIW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+14048, IF_PENT|IF_MMX|IF_SQ|IF_CYRIX},
    /*  807 */ {I_PMULHRWA, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5328, IF_PENT|IF_3DNOW|IF_SQ},
    /*  808 */ {I_PMULHRWC, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+14054, IF_PENT|IF_MMX|IF_SQ|IF_CYRIX},
    /*  809 */ {I_PMULHW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7200, IF_PENT|IF_MMX|IF_SQ},
    /*  810 */ {I_PMULLW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7207, IF_PENT|IF_MMX|IF_SQ},
    /*  811 */ {I_PMVGEZB, 2, {MMXREG,MEMORY,0,0,0}, nasm_bytecodes+15435, IF_PENT|IF_MMX|IF_SQ|IF_CYRIX},
    /*  812 */ {I_PMVLZB, 2, {MMXREG,MEMORY,0,0,0}, nasm_bytecodes+15291, IF_PENT|IF_MMX|IF_SQ|IF_CYRIX},
    /*  813 */ {I_PMVNZB, 2, {MMXREG,MEMORY,0,0,0}, nasm_bytecodes+15273, IF_PENT|IF_MMX|IF_SQ|IF_CYRIX},
    /*  814 */ {I_PMVZB, 2, {MMXREG,MEMORY,0,0,0}, nasm_bytecodes+15195, IF_PENT|IF_MMX|IF_SQ|IF_CYRIX},
    /*  815 */ {I_POP, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+19388, IF_8086},
    /*  816 */ {I_POP, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+19392, IF_386|IF_NOLONG},
    /*  817 */ {I_POP, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+19396, IF_X64},
    /*  818 */ {I_POP, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17957, IF_8086},
    /*  819 */ {I_POP, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17962, IF_386|IF_NOLONG},
    /*  820 */ {I_POP, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17967, IF_X64},
    /*  821 */ {I_POP, 1, {REG_DESS,0,0,0,0}, nasm_bytecodes+19206, IF_8086|IF_NOLONG},
    /*  822 */ {I_POP, 1, {REG_FSGS,0,0,0,0}, nasm_bytecodes+19400, IF_386},
    /*  823 */ {I_POPA, 0, {0,0,0,0,0}, nasm_bytecodes+19404, IF_186|IF_NOLONG},
    /*  824 */ {I_POPAD, 0, {0,0,0,0,0}, nasm_bytecodes+19408, IF_386|IF_NOLONG},
    /*  825 */ {I_POPAW, 0, {0,0,0,0,0}, nasm_bytecodes+19412, IF_186|IF_NOLONG},
    /*  826 */ {I_POPF, 0, {0,0,0,0,0}, nasm_bytecodes+19416, IF_8086},
    /*  827 */ {I_POPFD, 0, {0,0,0,0,0}, nasm_bytecodes+19420, IF_386|IF_NOLONG},
    /*  828 */ {I_POPFQ, 0, {0,0,0,0,0}, nasm_bytecodes+19420, IF_X64},
    /*  829 */ {I_POPFW, 0, {0,0,0,0,0}, nasm_bytecodes+19424, IF_8086},
    /*  830 */ {I_POR, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7214, IF_PENT|IF_MMX|IF_SQ},
    /*  831 */ {I_PREFETCH, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17972, IF_PENT|IF_3DNOW|IF_SQ},
    /*  832 */ {I_PREFETCHW, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+17977, IF_PENT|IF_3DNOW|IF_SQ},
    /*  833 */ {I_PSLLD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7221, IF_PENT|IF_MMX|IF_SQ},
    /*  834 */ {I_PSLLD, 2, {MMXREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7228, IF_PENT|IF_MMX},
    /*  835 */ {I_PSLLQ, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7235, IF_PENT|IF_MMX|IF_SQ},
    /*  836 */ {I_PSLLQ, 2, {MMXREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7242, IF_PENT|IF_MMX},
    /*  837 */ {I_PSLLW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7249, IF_PENT|IF_MMX|IF_SQ},
    /*  838 */ {I_PSLLW, 2, {MMXREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7256, IF_PENT|IF_MMX},
    /*  839 */ {I_PSRAD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7263, IF_PENT|IF_MMX|IF_SQ},
    /*  840 */ {I_PSRAD, 2, {MMXREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7270, IF_PENT|IF_MMX},
    /*  841 */ {I_PSRAW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7277, IF_PENT|IF_MMX|IF_SQ},
    /*  842 */ {I_PSRAW, 2, {MMXREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7284, IF_PENT|IF_MMX},
    /*  843 */ {I_PSRLD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7291, IF_PENT|IF_MMX|IF_SQ},
    /*  844 */ {I_PSRLD, 2, {MMXREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7298, IF_PENT|IF_MMX},
    /*  845 */ {I_PSRLQ, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7305, IF_PENT|IF_MMX|IF_SQ},
    /*  846 */ {I_PSRLQ, 2, {MMXREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7312, IF_PENT|IF_MMX},
    /*  847 */ {I_PSRLW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7319, IF_PENT|IF_MMX|IF_SQ},
    /*  848 */ {I_PSRLW, 2, {MMXREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7326, IF_PENT|IF_MMX},
    /*  849 */ {I_PSUBB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7333, IF_PENT|IF_MMX|IF_SQ},
    /*  850 */ {I_PSUBD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7340, IF_PENT|IF_MMX|IF_SQ},
    /*  851 */ {I_PSUBSB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7347, IF_PENT|IF_MMX|IF_SQ},
    /*  852 */ {I_PSUBSIW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+14060, IF_PENT|IF_MMX|IF_SQ|IF_CYRIX},
    /*  853 */ {I_PSUBSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7354, IF_PENT|IF_MMX|IF_SQ},
    /*  854 */ {I_PSUBUSB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7361, IF_PENT|IF_MMX|IF_SQ},
    /*  855 */ {I_PSUBUSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7368, IF_PENT|IF_MMX|IF_SQ},
    /*  856 */ {I_PSUBW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7375, IF_PENT|IF_MMX|IF_SQ},
    /*  857 */ {I_PUNPCKHBW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7382, IF_PENT|IF_MMX|IF_SQ},
    /*  858 */ {I_PUNPCKHDQ, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7389, IF_PENT|IF_MMX|IF_SQ},
    /*  859 */ {I_PUNPCKHWD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7396, IF_PENT|IF_MMX|IF_SQ},
    /*  860 */ {I_PUNPCKLBW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7403, IF_PENT|IF_MMX|IF_SQ},
    /*  861 */ {I_PUNPCKLDQ, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7410, IF_PENT|IF_MMX|IF_SQ},
    /*  862 */ {I_PUNPCKLWD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7417, IF_PENT|IF_MMX|IF_SQ},
    /*  863 */ {I_PUSH, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+19428, IF_8086},
    /*  864 */ {I_PUSH, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+19432, IF_386|IF_NOLONG},
    /*  865 */ {I_PUSH, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+19436, IF_X64},
    /*  866 */ {I_PUSH, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+17982, IF_8086},
    /*  867 */ {I_PUSH, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+17987, IF_386|IF_NOLONG},
    /*  868 */ {I_PUSH, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+17992, IF_X64},
    /*  869 */ {I_PUSH, 1, {REG_CS,0,0,0,0}, nasm_bytecodes+19182, IF_8086|IF_NOLONG},
    /*  870 */ {I_PUSH, 1, {REG_DESS,0,0,0,0}, nasm_bytecodes+19182, IF_8086|IF_NOLONG},
    /*  871 */ {I_PUSH, 1, {REG_FSGS,0,0,0,0}, nasm_bytecodes+19440, IF_386},
    /*  872 */ {I_PUSH, 1, {IMMEDIATE|BITS8,0,0,0,0}, nasm_bytecodes+19444, IF_186},
    /*  873 */ {I_PUSH, 1, {IMMEDIATE|BITS16,0,0,0,0}, nasm_bytecodes+17997, IF_186|IF_AR0|IF_SZ},
    /*  874 */ {I_PUSH, 1, {IMMEDIATE|BITS32,0,0,0,0}, nasm_bytecodes+18002, IF_386|IF_NOLONG|IF_AR0|IF_SZ},
    /*  875 */ {I_PUSH, 1, {IMMEDIATE|BITS32,0,0,0,0}, nasm_bytecodes+18002, IF_386|IF_NOLONG|IF_SD},
    /*  876 */ {I_PUSH, 1, {IMMEDIATE|BITS32,0,0,0,0}, nasm_bytecodes+18007, IF_X64|IF_AR0|IF_SZ},
    /*  877 */ {I_PUSH, 1, {IMMEDIATE|BITS64,0,0,0,0}, nasm_bytecodes+18007, IF_X64|IF_AR0|IF_SZ},
    /*  878 */ {I_PUSHA, 0, {0,0,0,0,0}, nasm_bytecodes+19448, IF_186|IF_NOLONG},
    /*  879 */ {I_PUSHAD, 0, {0,0,0,0,0}, nasm_bytecodes+19452, IF_386|IF_NOLONG},
    /*  880 */ {I_PUSHAW, 0, {0,0,0,0,0}, nasm_bytecodes+19456, IF_186|IF_NOLONG},
    /*  881 */ {I_PUSHF, 0, {0,0,0,0,0}, nasm_bytecodes+19460, IF_8086},
    /*  882 */ {I_PUSHFD, 0, {0,0,0,0,0}, nasm_bytecodes+19464, IF_386|IF_NOLONG},
    /*  883 */ {I_PUSHFQ, 0, {0,0,0,0,0}, nasm_bytecodes+19464, IF_X64},
    /*  884 */ {I_PUSHFW, 0, {0,0,0,0,0}, nasm_bytecodes+19468, IF_8086},
    /*  885 */ {I_PXOR, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7424, IF_PENT|IF_MMX|IF_SQ},
    /*  886 */ {I_RCL, 2, {RM_GPR|BITS8,UNITY,0,0,0}, nasm_bytecodes+19472, IF_8086},
    /*  887 */ {I_RCL, 2, {RM_GPR|BITS8,REG_CL,0,0,0}, nasm_bytecodes+19476, IF_8086},
    /*  888 */ {I_RCL, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18012, IF_186|IF_SB},
    /*  889 */ {I_RCL, 2, {RM_GPR|BITS16,UNITY,0,0,0}, nasm_bytecodes+18017, IF_8086},
    /*  890 */ {I_RCL, 2, {RM_GPR|BITS16,REG_CL,0,0,0}, nasm_bytecodes+18022, IF_8086},
    /*  891 */ {I_RCL, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14066, IF_186|IF_SB},
    /*  892 */ {I_RCL, 2, {RM_GPR|BITS32,UNITY,0,0,0}, nasm_bytecodes+18027, IF_386},
    /*  893 */ {I_RCL, 2, {RM_GPR|BITS32,REG_CL,0,0,0}, nasm_bytecodes+18032, IF_386},
    /*  894 */ {I_RCL, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14072, IF_386|IF_SB},
    /*  895 */ {I_RCL, 2, {RM_GPR|BITS64,UNITY,0,0,0}, nasm_bytecodes+18037, IF_X64},
    /*  896 */ {I_RCL, 2, {RM_GPR|BITS64,REG_CL,0,0,0}, nasm_bytecodes+18042, IF_X64},
    /*  897 */ {I_RCL, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14078, IF_X64|IF_SB},
    /*  898 */ {I_RCR, 2, {RM_GPR|BITS8,UNITY,0,0,0}, nasm_bytecodes+19480, IF_8086},
    /*  899 */ {I_RCR, 2, {RM_GPR|BITS8,REG_CL,0,0,0}, nasm_bytecodes+19484, IF_8086},
    /*  900 */ {I_RCR, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18047, IF_186|IF_SB},
    /*  901 */ {I_RCR, 2, {RM_GPR|BITS16,UNITY,0,0,0}, nasm_bytecodes+18052, IF_8086},
    /*  902 */ {I_RCR, 2, {RM_GPR|BITS16,REG_CL,0,0,0}, nasm_bytecodes+18057, IF_8086},
    /*  903 */ {I_RCR, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14084, IF_186|IF_SB},
    /*  904 */ {I_RCR, 2, {RM_GPR|BITS32,UNITY,0,0,0}, nasm_bytecodes+18062, IF_386},
    /*  905 */ {I_RCR, 2, {RM_GPR|BITS32,REG_CL,0,0,0}, nasm_bytecodes+18067, IF_386},
    /*  906 */ {I_RCR, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14090, IF_386|IF_SB},
    /*  907 */ {I_RCR, 2, {RM_GPR|BITS64,UNITY,0,0,0}, nasm_bytecodes+18072, IF_X64},
    /*  908 */ {I_RCR, 2, {RM_GPR|BITS64,REG_CL,0,0,0}, nasm_bytecodes+18077, IF_X64},
    /*  909 */ {I_RCR, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14096, IF_X64|IF_SB},
    /*  910 */ {I_RDSHR, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+14102, IF_P6|IF_CYRIX|IF_SMM},
    /*  911 */ {I_RDMSR, 0, {0,0,0,0,0}, nasm_bytecodes+19488, IF_PENT|IF_PRIV},
    /*  912 */ {I_RDPMC, 0, {0,0,0,0,0}, nasm_bytecodes+19492, IF_P6},
    /*  913 */ {I_RDTSC, 0, {0,0,0,0,0}, nasm_bytecodes+19496, IF_PENT},
    /*  914 */ {I_RDTSCP, 0, {0,0,0,0,0}, nasm_bytecodes+18082, IF_X86_64},
    /*  915 */ {I_RET, 0, {0,0,0,0,0}, nasm_bytecodes+18609, IF_8086},
    /*  916 */ {I_RET, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+19500, IF_8086|IF_SW},
    /*  917 */ {I_RETF, 0, {0,0,0,0,0}, nasm_bytecodes+19731, IF_8086},
    /*  918 */ {I_RETF, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+19504, IF_8086|IF_SW},
    /*  919 */ {I_RETN, 0, {0,0,0,0,0}, nasm_bytecodes+18609, IF_8086},
    /*  920 */ {I_RETN, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+19500, IF_8086|IF_SW},
    /*  921 */ {I_ROL, 2, {RM_GPR|BITS8,UNITY,0,0,0}, nasm_bytecodes+19508, IF_8086},
    /*  922 */ {I_ROL, 2, {RM_GPR|BITS8,REG_CL,0,0,0}, nasm_bytecodes+19512, IF_8086},
    /*  923 */ {I_ROL, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18087, IF_186|IF_SB},
    /*  924 */ {I_ROL, 2, {RM_GPR|BITS16,UNITY,0,0,0}, nasm_bytecodes+18092, IF_8086},
    /*  925 */ {I_ROL, 2, {RM_GPR|BITS16,REG_CL,0,0,0}, nasm_bytecodes+18097, IF_8086},
    /*  926 */ {I_ROL, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14108, IF_186|IF_SB},
    /*  927 */ {I_ROL, 2, {RM_GPR|BITS32,UNITY,0,0,0}, nasm_bytecodes+18102, IF_386},
    /*  928 */ {I_ROL, 2, {RM_GPR|BITS32,REG_CL,0,0,0}, nasm_bytecodes+18107, IF_386},
    /*  929 */ {I_ROL, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14114, IF_386|IF_SB},
    /*  930 */ {I_ROL, 2, {RM_GPR|BITS64,UNITY,0,0,0}, nasm_bytecodes+18112, IF_X64},
    /*  931 */ {I_ROL, 2, {RM_GPR|BITS64,REG_CL,0,0,0}, nasm_bytecodes+18117, IF_X64},
    /*  932 */ {I_ROL, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14120, IF_X64|IF_SB},
    /*  933 */ {I_ROR, 2, {RM_GPR|BITS8,UNITY,0,0,0}, nasm_bytecodes+19516, IF_8086},
    /*  934 */ {I_ROR, 2, {RM_GPR|BITS8,REG_CL,0,0,0}, nasm_bytecodes+19520, IF_8086},
    /*  935 */ {I_ROR, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18122, IF_186|IF_SB},
    /*  936 */ {I_ROR, 2, {RM_GPR|BITS16,UNITY,0,0,0}, nasm_bytecodes+18127, IF_8086},
    /*  937 */ {I_ROR, 2, {RM_GPR|BITS16,REG_CL,0,0,0}, nasm_bytecodes+18132, IF_8086},
    /*  938 */ {I_ROR, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14126, IF_186|IF_SB},
    /*  939 */ {I_ROR, 2, {RM_GPR|BITS32,UNITY,0,0,0}, nasm_bytecodes+18137, IF_386},
    /*  940 */ {I_ROR, 2, {RM_GPR|BITS32,REG_CL,0,0,0}, nasm_bytecodes+18142, IF_386},
    /*  941 */ {I_ROR, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14132, IF_386|IF_SB},
    /*  942 */ {I_ROR, 2, {RM_GPR|BITS64,UNITY,0,0,0}, nasm_bytecodes+18147, IF_X64},
    /*  943 */ {I_ROR, 2, {RM_GPR|BITS64,REG_CL,0,0,0}, nasm_bytecodes+18152, IF_X64},
    /*  944 */ {I_ROR, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14138, IF_X64|IF_SB},
    /*  945 */ {I_RSDC, 2, {REG_SREG,MEMORY|BITS80,0,0,0}, nasm_bytecodes+15543, IF_486|IF_CYRIX|IF_SMM},
    /*  946 */ {I_RSLDT, 1, {MEMORY|BITS80,0,0,0,0}, nasm_bytecodes+18157, IF_486|IF_CYRIX|IF_SMM},
    /*  947 */ {I_RSM, 0, {0,0,0,0,0}, nasm_bytecodes+19524, IF_PENT|IF_SMM},
    /*  948 */ {I_RSTS, 1, {MEMORY|BITS80,0,0,0,0}, nasm_bytecodes+18162, IF_486|IF_CYRIX|IF_SMM},
    /*  949 */ {I_SAHF, 0, {0,0,0,0,0}, nasm_bytecodes+5213, IF_8086},
    /*  950 */ {I_SALC, 0, {0,0,0,0,0}, nasm_bytecodes+19734, IF_8086|IF_UNDOC},
    /*  951 */ {I_SAR, 2, {RM_GPR|BITS8,UNITY,0,0,0}, nasm_bytecodes+19536, IF_8086},
    /*  952 */ {I_SAR, 2, {RM_GPR|BITS8,REG_CL,0,0,0}, nasm_bytecodes+19540, IF_8086},
    /*  953 */ {I_SAR, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18202, IF_186|IF_SB},
    /*  954 */ {I_SAR, 2, {RM_GPR|BITS16,UNITY,0,0,0}, nasm_bytecodes+18207, IF_8086},
    /*  955 */ {I_SAR, 2, {RM_GPR|BITS16,REG_CL,0,0,0}, nasm_bytecodes+18212, IF_8086},
    /*  956 */ {I_SAR, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14162, IF_186|IF_SB},
    /*  957 */ {I_SAR, 2, {RM_GPR|BITS32,UNITY,0,0,0}, nasm_bytecodes+18217, IF_386},
    /*  958 */ {I_SAR, 2, {RM_GPR|BITS32,REG_CL,0,0,0}, nasm_bytecodes+18222, IF_386},
    /*  959 */ {I_SAR, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14168, IF_386|IF_SB},
    /*  960 */ {I_SAR, 2, {RM_GPR|BITS64,UNITY,0,0,0}, nasm_bytecodes+18227, IF_X64},
    /*  961 */ {I_SAR, 2, {RM_GPR|BITS64,REG_CL,0,0,0}, nasm_bytecodes+18232, IF_X64},
    /*  962 */ {I_SAR, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14174, IF_X64|IF_SB},
    /*  963 */ {I_SBB, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19544, IF_8086|IF_SM},
    /*  964 */ {I_SBB, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19544, IF_8086},
    /*  965 */ {I_SBB, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18237, IF_8086|IF_SM},
    /*  966 */ {I_SBB, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18237, IF_8086},
    /*  967 */ {I_SBB, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18242, IF_386|IF_SM},
    /*  968 */ {I_SBB, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18242, IF_386},
    /*  969 */ {I_SBB, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18247, IF_X64|IF_SM},
    /*  970 */ {I_SBB, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18247, IF_X64},
    /*  971 */ {I_SBB, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+8771, IF_8086|IF_SM},
    /*  972 */ {I_SBB, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+8771, IF_8086},
    /*  973 */ {I_SBB, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+18252, IF_8086|IF_SM},
    /*  974 */ {I_SBB, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18252, IF_8086},
    /*  975 */ {I_SBB, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+18257, IF_386|IF_SM},
    /*  976 */ {I_SBB, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18257, IF_386},
    /*  977 */ {I_SBB, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+18262, IF_X64|IF_SM},
    /*  978 */ {I_SBB, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18262, IF_X64},
    /*  979 */ {I_SBB, 2, {RM_GPR|BITS16,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14180, IF_8086},
    /*  980 */ {I_SBB, 2, {RM_GPR|BITS32,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14186, IF_386},
    /*  981 */ {I_SBB, 2, {RM_GPR|BITS64,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14192, IF_X64},
    /*  982 */ {I_SBB, 2, {REG_AL,IMMEDIATE,0,0,0}, nasm_bytecodes+19548, IF_8086|IF_SM},
    /*  983 */ {I_SBB, 2, {REG_AX,SBYTE16,0,0,0}, nasm_bytecodes+14180, IF_8086|IF_SM},
    /*  984 */ {I_SBB, 2, {REG_AX,IMMEDIATE,0,0,0}, nasm_bytecodes+18267, IF_8086|IF_SM},
    /*  985 */ {I_SBB, 2, {REG_EAX,SBYTE32,0,0,0}, nasm_bytecodes+14186, IF_386|IF_SM},
    /*  986 */ {I_SBB, 2, {REG_EAX,IMMEDIATE,0,0,0}, nasm_bytecodes+18272, IF_386|IF_SM},
    /*  987 */ {I_SBB, 2, {REG_RAX,SBYTE64,0,0,0}, nasm_bytecodes+14192, IF_X64|IF_SM},
    /*  988 */ {I_SBB, 2, {REG_RAX,IMMEDIATE,0,0,0}, nasm_bytecodes+18277, IF_X64|IF_SM},
    /*  989 */ {I_SBB, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18282, IF_8086|IF_SM},
    /*  990 */ {I_SBB, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14198, IF_8086|IF_SM},
    /*  991 */ {I_SBB, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14204, IF_386|IF_SM},
    /*  992 */ {I_SBB, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14210, IF_X64|IF_SM},
    /*  993 */ {I_SBB, 2, {MEMORY,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+18282, IF_8086|IF_SM},
    /*  994 */ {I_SBB, 2, {MEMORY,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+14198, IF_8086|IF_SM},
    /*  995 */ {I_SBB, 2, {MEMORY,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+14204, IF_386|IF_SM},
    /*  996 */ {I_SCASB, 0, {0,0,0,0,0}, nasm_bytecodes+19552, IF_8086},
    /*  997 */ {I_SCASD, 0, {0,0,0,0,0}, nasm_bytecodes+18287, IF_386},
    /*  998 */ {I_SCASQ, 0, {0,0,0,0,0}, nasm_bytecodes+18292, IF_X64},
    /*  999 */ {I_SCASW, 0, {0,0,0,0,0}, nasm_bytecodes+18297, IF_8086},
    /* 1000 */ {I_SFENCE, 0, {0,0,0,0,0}, nasm_bytecodes+18302, IF_X64|IF_AMD},
    /* 1001 */ {I_SGDT, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+18307, IF_286},
    /* 1002 */ {I_SHL, 2, {RM_GPR|BITS8,UNITY,0,0,0}, nasm_bytecodes+19528, IF_8086},
    /* 1003 */ {I_SHL, 2, {RM_GPR|BITS8,REG_CL,0,0,0}, nasm_bytecodes+19532, IF_8086},
    /* 1004 */ {I_SHL, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18167, IF_186|IF_SB},
    /* 1005 */ {I_SHL, 2, {RM_GPR|BITS16,UNITY,0,0,0}, nasm_bytecodes+18172, IF_8086},
    /* 1006 */ {I_SHL, 2, {RM_GPR|BITS16,REG_CL,0,0,0}, nasm_bytecodes+18177, IF_8086},
    /* 1007 */ {I_SHL, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14144, IF_186|IF_SB},
    /* 1008 */ {I_SHL, 2, {RM_GPR|BITS32,UNITY,0,0,0}, nasm_bytecodes+18182, IF_386},
    /* 1009 */ {I_SHL, 2, {RM_GPR|BITS32,REG_CL,0,0,0}, nasm_bytecodes+18187, IF_386},
    /* 1010 */ {I_SHL, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14150, IF_386|IF_SB},
    /* 1011 */ {I_SHL, 2, {RM_GPR|BITS64,UNITY,0,0,0}, nasm_bytecodes+18192, IF_X64},
    /* 1012 */ {I_SHL, 2, {RM_GPR|BITS64,REG_CL,0,0,0}, nasm_bytecodes+18197, IF_X64},
    /* 1013 */ {I_SHL, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14156, IF_X64|IF_SB},
    /* 1014 */ {I_SHLD, 3, {MEMORY,REG_GPR|BITS16,IMMEDIATE,0,0}, nasm_bytecodes+7431, IF_386|IF_SM2|IF_SB|IF_AR2},
    /* 1015 */ {I_SHLD, 3, {REG_GPR|BITS16,REG_GPR|BITS16,IMMEDIATE,0,0}, nasm_bytecodes+7431, IF_386|IF_SM2|IF_SB|IF_AR2},
    /* 1016 */ {I_SHLD, 3, {MEMORY,REG_GPR|BITS32,IMMEDIATE,0,0}, nasm_bytecodes+7438, IF_386|IF_SM2|IF_SB|IF_AR2},
    /* 1017 */ {I_SHLD, 3, {REG_GPR|BITS32,REG_GPR|BITS32,IMMEDIATE,0,0}, nasm_bytecodes+7438, IF_386|IF_SM2|IF_SB|IF_AR2},
    /* 1018 */ {I_SHLD, 3, {MEMORY,REG_GPR|BITS64,IMMEDIATE,0,0}, nasm_bytecodes+7445, IF_X64|IF_SM2|IF_SB|IF_AR2},
    /* 1019 */ {I_SHLD, 3, {REG_GPR|BITS64,REG_GPR|BITS64,IMMEDIATE,0,0}, nasm_bytecodes+7445, IF_X64|IF_SM2|IF_SB|IF_AR2},
    /* 1020 */ {I_SHLD, 3, {MEMORY,REG_GPR|BITS16,REG_CL,0,0}, nasm_bytecodes+14216, IF_386|IF_SM},
    /* 1021 */ {I_SHLD, 3, {REG_GPR|BITS16,REG_GPR|BITS16,REG_CL,0,0}, nasm_bytecodes+14216, IF_386},
    /* 1022 */ {I_SHLD, 3, {MEMORY,REG_GPR|BITS32,REG_CL,0,0}, nasm_bytecodes+14222, IF_386|IF_SM},
    /* 1023 */ {I_SHLD, 3, {REG_GPR|BITS32,REG_GPR|BITS32,REG_CL,0,0}, nasm_bytecodes+14222, IF_386},
    /* 1024 */ {I_SHLD, 3, {MEMORY,REG_GPR|BITS64,REG_CL,0,0}, nasm_bytecodes+14228, IF_X64|IF_SM},
    /* 1025 */ {I_SHLD, 3, {REG_GPR|BITS64,REG_GPR|BITS64,REG_CL,0,0}, nasm_bytecodes+14228, IF_X64},
    /* 1026 */ {I_SHR, 2, {RM_GPR|BITS8,UNITY,0,0,0}, nasm_bytecodes+19556, IF_8086},
    /* 1027 */ {I_SHR, 2, {RM_GPR|BITS8,REG_CL,0,0,0}, nasm_bytecodes+19560, IF_8086},
    /* 1028 */ {I_SHR, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18312, IF_186|IF_SB},
    /* 1029 */ {I_SHR, 2, {RM_GPR|BITS16,UNITY,0,0,0}, nasm_bytecodes+18317, IF_8086},
    /* 1030 */ {I_SHR, 2, {RM_GPR|BITS16,REG_CL,0,0,0}, nasm_bytecodes+18322, IF_8086},
    /* 1031 */ {I_SHR, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14234, IF_186|IF_SB},
    /* 1032 */ {I_SHR, 2, {RM_GPR|BITS32,UNITY,0,0,0}, nasm_bytecodes+18327, IF_386},
    /* 1033 */ {I_SHR, 2, {RM_GPR|BITS32,REG_CL,0,0,0}, nasm_bytecodes+18332, IF_386},
    /* 1034 */ {I_SHR, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14240, IF_386|IF_SB},
    /* 1035 */ {I_SHR, 2, {RM_GPR|BITS64,UNITY,0,0,0}, nasm_bytecodes+18337, IF_X64},
    /* 1036 */ {I_SHR, 2, {RM_GPR|BITS64,REG_CL,0,0,0}, nasm_bytecodes+18342, IF_X64},
    /* 1037 */ {I_SHR, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14246, IF_X64|IF_SB},
    /* 1038 */ {I_SHRD, 3, {MEMORY,REG_GPR|BITS16,IMMEDIATE,0,0}, nasm_bytecodes+7452, IF_386|IF_SM2|IF_SB|IF_AR2},
    /* 1039 */ {I_SHRD, 3, {REG_GPR|BITS16,REG_GPR|BITS16,IMMEDIATE,0,0}, nasm_bytecodes+7452, IF_386|IF_SM2|IF_SB|IF_AR2},
    /* 1040 */ {I_SHRD, 3, {MEMORY,REG_GPR|BITS32,IMMEDIATE,0,0}, nasm_bytecodes+7459, IF_386|IF_SM2|IF_SB|IF_AR2},
    /* 1041 */ {I_SHRD, 3, {REG_GPR|BITS32,REG_GPR|BITS32,IMMEDIATE,0,0}, nasm_bytecodes+7459, IF_386|IF_SM2|IF_SB|IF_AR2},
    /* 1042 */ {I_SHRD, 3, {MEMORY,REG_GPR|BITS64,IMMEDIATE,0,0}, nasm_bytecodes+7466, IF_X64|IF_SM2|IF_SB|IF_AR2},
    /* 1043 */ {I_SHRD, 3, {REG_GPR|BITS64,REG_GPR|BITS64,IMMEDIATE,0,0}, nasm_bytecodes+7466, IF_X64|IF_SM2|IF_SB|IF_AR2},
    /* 1044 */ {I_SHRD, 3, {MEMORY,REG_GPR|BITS16,REG_CL,0,0}, nasm_bytecodes+14252, IF_386|IF_SM},
    /* 1045 */ {I_SHRD, 3, {REG_GPR|BITS16,REG_GPR|BITS16,REG_CL,0,0}, nasm_bytecodes+14252, IF_386},
    /* 1046 */ {I_SHRD, 3, {MEMORY,REG_GPR|BITS32,REG_CL,0,0}, nasm_bytecodes+14258, IF_386|IF_SM},
    /* 1047 */ {I_SHRD, 3, {REG_GPR|BITS32,REG_GPR|BITS32,REG_CL,0,0}, nasm_bytecodes+14258, IF_386},
    /* 1048 */ {I_SHRD, 3, {MEMORY,REG_GPR|BITS64,REG_CL,0,0}, nasm_bytecodes+14264, IF_X64|IF_SM},
    /* 1049 */ {I_SHRD, 3, {REG_GPR|BITS64,REG_GPR|BITS64,REG_CL,0,0}, nasm_bytecodes+14264, IF_X64},
    /* 1050 */ {I_SIDT, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+18347, IF_286},
    /* 1051 */ {I_SLDT, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14289, IF_286},
    /* 1052 */ {I_SLDT, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+14289, IF_286},
    /* 1053 */ {I_SLDT, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+14270, IF_286},
    /* 1054 */ {I_SLDT, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+14276, IF_386},
    /* 1055 */ {I_SLDT, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+14288, IF_X64},
    /* 1056 */ {I_SKINIT, 0, {0,0,0,0,0}, nasm_bytecodes+18352, IF_X64},
    /* 1057 */ {I_SMI, 0, {0,0,0,0,0}, nasm_bytecodes+19704, IF_386|IF_UNDOC},
    /* 1058 */ {I_SMSW, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14301, IF_286},
    /* 1059 */ {I_SMSW, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+14301, IF_286},
    /* 1060 */ {I_SMSW, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+14294, IF_286},
    /* 1061 */ {I_SMSW, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+14300, IF_386},
    /* 1062 */ {I_STC, 0, {0,0,0,0,0}, nasm_bytecodes+18084, IF_8086},
    /* 1063 */ {I_STD, 0, {0,0,0,0,0}, nasm_bytecodes+19737, IF_8086},
    /* 1064 */ {I_STGI, 0, {0,0,0,0,0}, nasm_bytecodes+18357, IF_X64},
    /* 1065 */ {I_STI, 0, {0,0,0,0,0}, nasm_bytecodes+19740, IF_8086},
    /* 1066 */ {I_STOSB, 0, {0,0,0,0,0}, nasm_bytecodes+5317, IF_8086},
    /* 1067 */ {I_STOSD, 0, {0,0,0,0,0}, nasm_bytecodes+19572, IF_386},
    /* 1068 */ {I_STOSQ, 0, {0,0,0,0,0}, nasm_bytecodes+19576, IF_X64},
    /* 1069 */ {I_STOSW, 0, {0,0,0,0,0}, nasm_bytecodes+19580, IF_8086},
    /* 1070 */ {I_STR, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14319, IF_286|IF_PROT},
    /* 1071 */ {I_STR, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+14319, IF_286|IF_PROT},
    /* 1072 */ {I_STR, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+14306, IF_286|IF_PROT},
    /* 1073 */ {I_STR, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+14312, IF_386|IF_PROT},
    /* 1074 */ {I_STR, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+14318, IF_X64},
    /* 1075 */ {I_SUB, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19584, IF_8086|IF_SM},
    /* 1076 */ {I_SUB, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19584, IF_8086},
    /* 1077 */ {I_SUB, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18362, IF_8086|IF_SM},
    /* 1078 */ {I_SUB, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18362, IF_8086},
    /* 1079 */ {I_SUB, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18367, IF_386|IF_SM},
    /* 1080 */ {I_SUB, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18367, IF_386},
    /* 1081 */ {I_SUB, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18372, IF_X64|IF_SM},
    /* 1082 */ {I_SUB, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18372, IF_X64},
    /* 1083 */ {I_SUB, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+9786, IF_8086|IF_SM},
    /* 1084 */ {I_SUB, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+9786, IF_8086},
    /* 1085 */ {I_SUB, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+18377, IF_8086|IF_SM},
    /* 1086 */ {I_SUB, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18377, IF_8086},
    /* 1087 */ {I_SUB, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+18382, IF_386|IF_SM},
    /* 1088 */ {I_SUB, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18382, IF_386},
    /* 1089 */ {I_SUB, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+18387, IF_X64|IF_SM},
    /* 1090 */ {I_SUB, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18387, IF_X64},
    /* 1091 */ {I_SUB, 2, {RM_GPR|BITS16,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14324, IF_8086},
    /* 1092 */ {I_SUB, 2, {RM_GPR|BITS32,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14330, IF_386},
    /* 1093 */ {I_SUB, 2, {RM_GPR|BITS64,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14336, IF_X64},
    /* 1094 */ {I_SUB, 2, {REG_AL,IMMEDIATE,0,0,0}, nasm_bytecodes+19588, IF_8086|IF_SM},
    /* 1095 */ {I_SUB, 2, {REG_AX,SBYTE16,0,0,0}, nasm_bytecodes+14324, IF_8086|IF_SM},
    /* 1096 */ {I_SUB, 2, {REG_AX,IMMEDIATE,0,0,0}, nasm_bytecodes+18392, IF_8086|IF_SM},
    /* 1097 */ {I_SUB, 2, {REG_EAX,SBYTE32,0,0,0}, nasm_bytecodes+14330, IF_386|IF_SM},
    /* 1098 */ {I_SUB, 2, {REG_EAX,IMMEDIATE,0,0,0}, nasm_bytecodes+18397, IF_386|IF_SM},
    /* 1099 */ {I_SUB, 2, {REG_RAX,SBYTE64,0,0,0}, nasm_bytecodes+14336, IF_X64|IF_SM},
    /* 1100 */ {I_SUB, 2, {REG_RAX,IMMEDIATE,0,0,0}, nasm_bytecodes+18402, IF_X64|IF_SM},
    /* 1101 */ {I_SUB, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18407, IF_8086|IF_SM},
    /* 1102 */ {I_SUB, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14342, IF_8086|IF_SM},
    /* 1103 */ {I_SUB, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14348, IF_386|IF_SM},
    /* 1104 */ {I_SUB, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14354, IF_X64|IF_SM},
    /* 1105 */ {I_SUB, 2, {MEMORY,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+18407, IF_8086|IF_SM},
    /* 1106 */ {I_SUB, 2, {MEMORY,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+14342, IF_8086|IF_SM},
    /* 1107 */ {I_SUB, 2, {MEMORY,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+14348, IF_386|IF_SM},
    /* 1108 */ {I_SVDC, 2, {MEMORY|BITS80,REG_SREG,0,0,0}, nasm_bytecodes+7874, IF_486|IF_CYRIX|IF_SMM},
    /* 1109 */ {I_SVTS, 1, {MEMORY|BITS80,0,0,0,0}, nasm_bytecodes+18417, IF_486|IF_CYRIX|IF_SMM},
    /* 1110 */ {I_SWAPGS, 0, {0,0,0,0,0}, nasm_bytecodes+18422, IF_X64},
    /* 1111 */ {I_SYSCALL, 0, {0,0,0,0,0}, nasm_bytecodes+19292, IF_P6|IF_AMD},
    /* 1112 */ {I_SYSENTER, 0, {0,0,0,0,0}, nasm_bytecodes+19592, IF_P6},
    /* 1113 */ {I_SYSEXIT, 0, {0,0,0,0,0}, nasm_bytecodes+19596, IF_P6|IF_PRIV},
    /* 1114 */ {I_SYSRET, 0, {0,0,0,0,0}, nasm_bytecodes+19288, IF_P6|IF_PRIV|IF_AMD},
    /* 1115 */ {I_TEST, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19600, IF_8086|IF_SM},
    /* 1116 */ {I_TEST, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19600, IF_8086},
    /* 1117 */ {I_TEST, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18427, IF_8086|IF_SM},
    /* 1118 */ {I_TEST, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18427, IF_8086},
    /* 1119 */ {I_TEST, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18432, IF_386|IF_SM},
    /* 1120 */ {I_TEST, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18432, IF_386},
    /* 1121 */ {I_TEST, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18437, IF_X64|IF_SM},
    /* 1122 */ {I_TEST, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18437, IF_X64},
    /* 1123 */ {I_TEST, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+19604, IF_8086|IF_SM},
    /* 1124 */ {I_TEST, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+18442, IF_8086|IF_SM},
    /* 1125 */ {I_TEST, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+18447, IF_386|IF_SM},
    /* 1126 */ {I_TEST, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+18452, IF_X64|IF_SM},
    /* 1127 */ {I_TEST, 2, {REG_AL,IMMEDIATE,0,0,0}, nasm_bytecodes+19608, IF_8086|IF_SM},
    /* 1128 */ {I_TEST, 2, {REG_AX,IMMEDIATE,0,0,0}, nasm_bytecodes+18457, IF_8086|IF_SM},
    /* 1129 */ {I_TEST, 2, {REG_EAX,IMMEDIATE,0,0,0}, nasm_bytecodes+18462, IF_386|IF_SM},
    /* 1130 */ {I_TEST, 2, {REG_RAX,IMMEDIATE,0,0,0}, nasm_bytecodes+18467, IF_X64|IF_SM},
    /* 1131 */ {I_TEST, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18472, IF_8086|IF_SM},
    /* 1132 */ {I_TEST, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14360, IF_8086|IF_SM},
    /* 1133 */ {I_TEST, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14366, IF_386|IF_SM},
    /* 1134 */ {I_TEST, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14372, IF_X64|IF_SM},
    /* 1135 */ {I_TEST, 2, {MEMORY,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+18472, IF_8086|IF_SM},
    /* 1136 */ {I_TEST, 2, {MEMORY,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+14360, IF_8086|IF_SM},
    /* 1137 */ {I_TEST, 2, {MEMORY,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+14366, IF_386|IF_SM},
    /* 1138 */ {I_UD0, 0, {0,0,0,0,0}, nasm_bytecodes+19612, IF_186|IF_UNDOC},
    /* 1139 */ {I_UD1, 0, {0,0,0,0,0}, nasm_bytecodes+19616, IF_186|IF_UNDOC},
    /* 1140 */ {I_UD2, 0, {0,0,0,0,0}, nasm_bytecodes+19620, IF_186},
    /* 1141 */ {I_VERR, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+18477, IF_286|IF_PROT},
    /* 1142 */ {I_VERR, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18477, IF_286|IF_PROT},
    /* 1143 */ {I_VERR, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+18477, IF_286|IF_PROT},
    /* 1144 */ {I_VERW, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+18482, IF_286|IF_PROT},
    /* 1145 */ {I_VERW, 1, {MEMORY|BITS16,0,0,0,0}, nasm_bytecodes+18482, IF_286|IF_PROT},
    /* 1146 */ {I_VERW, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+18482, IF_286|IF_PROT},
    /* 1147 */ {I_FWAIT, 0, {0,0,0,0,0}, nasm_bytecodes+19186, IF_8086},
    /* 1148 */ {I_WBINVD, 0, {0,0,0,0,0}, nasm_bytecodes+19624, IF_486|IF_PRIV},
    /* 1149 */ {I_WRSHR, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+14390, IF_P6|IF_CYRIX|IF_SMM},
    /* 1150 */ {I_WRMSR, 0, {0,0,0,0,0}, nasm_bytecodes+19628, IF_PENT|IF_PRIV},
    /* 1151 */ {I_XADD, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+18487, IF_486|IF_SM},
    /* 1152 */ {I_XADD, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+18487, IF_486},
    /* 1153 */ {I_XADD, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+14396, IF_486|IF_SM},
    /* 1154 */ {I_XADD, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+14396, IF_486},
    /* 1155 */ {I_XADD, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+14402, IF_486|IF_SM},
    /* 1156 */ {I_XADD, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+14402, IF_486},
    /* 1157 */ {I_XADD, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+14408, IF_X64|IF_SM},
    /* 1158 */ {I_XADD, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+14408, IF_X64},
    /* 1159 */ {I_XCHG, 2, {REG_AX,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+19632, IF_8086},
    /* 1160 */ {I_XCHG, 2, {REG_EAX,REG32NA,0,0,0}, nasm_bytecodes+19636, IF_386},
    /* 1161 */ {I_XCHG, 2, {REG_RAX,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+19640, IF_X64},
    /* 1162 */ {I_XCHG, 2, {REG_GPR|BITS16,REG_AX,0,0,0}, nasm_bytecodes+19644, IF_8086},
    /* 1163 */ {I_XCHG, 2, {REG32NA,REG_EAX,0,0,0}, nasm_bytecodes+19648, IF_386},
    /* 1164 */ {I_XCHG, 2, {REG_GPR|BITS64,REG_RAX,0,0,0}, nasm_bytecodes+19652, IF_X64},
    /* 1165 */ {I_XCHG, 2, {REG_EAX,REG_EAX,0,0,0}, nasm_bytecodes+19656, IF_386|IF_NOLONG},
    /* 1166 */ {I_XCHG, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+19660, IF_8086|IF_SM},
    /* 1167 */ {I_XCHG, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19660, IF_8086},
    /* 1168 */ {I_XCHG, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+18492, IF_8086|IF_SM},
    /* 1169 */ {I_XCHG, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18492, IF_8086},
    /* 1170 */ {I_XCHG, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+18497, IF_386|IF_SM},
    /* 1171 */ {I_XCHG, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18497, IF_386},
    /* 1172 */ {I_XCHG, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+18502, IF_X64|IF_SM},
    /* 1173 */ {I_XCHG, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18502, IF_X64},
    /* 1174 */ {I_XCHG, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19664, IF_8086|IF_SM},
    /* 1175 */ {I_XCHG, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19664, IF_8086},
    /* 1176 */ {I_XCHG, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18507, IF_8086|IF_SM},
    /* 1177 */ {I_XCHG, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18507, IF_8086},
    /* 1178 */ {I_XCHG, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18512, IF_386|IF_SM},
    /* 1179 */ {I_XCHG, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18512, IF_386},
    /* 1180 */ {I_XCHG, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18517, IF_X64|IF_SM},
    /* 1181 */ {I_XCHG, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18517, IF_X64},
    /* 1182 */ {I_XLATB, 0, {0,0,0,0,0}, nasm_bytecodes+19743, IF_8086},
    /* 1183 */ {I_XLAT, 0, {0,0,0,0,0}, nasm_bytecodes+19743, IF_8086},
    /* 1184 */ {I_XOR, 2, {MEMORY,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19668, IF_8086|IF_SM},
    /* 1185 */ {I_XOR, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+19668, IF_8086},
    /* 1186 */ {I_XOR, 2, {MEMORY,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18522, IF_8086|IF_SM},
    /* 1187 */ {I_XOR, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18522, IF_8086},
    /* 1188 */ {I_XOR, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18527, IF_386|IF_SM},
    /* 1189 */ {I_XOR, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18527, IF_386},
    /* 1190 */ {I_XOR, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18532, IF_X64|IF_SM},
    /* 1191 */ {I_XOR, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18532, IF_X64},
    /* 1192 */ {I_XOR, 2, {REG_GPR|BITS8,MEMORY,0,0,0}, nasm_bytecodes+10864, IF_8086|IF_SM},
    /* 1193 */ {I_XOR, 2, {REG_GPR|BITS8,REG_GPR|BITS8,0,0,0}, nasm_bytecodes+10864, IF_8086},
    /* 1194 */ {I_XOR, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+18537, IF_8086|IF_SM},
    /* 1195 */ {I_XOR, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+18537, IF_8086},
    /* 1196 */ {I_XOR, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+18542, IF_386|IF_SM},
    /* 1197 */ {I_XOR, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+18542, IF_386},
    /* 1198 */ {I_XOR, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+18547, IF_X64|IF_SM},
    /* 1199 */ {I_XOR, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+18547, IF_X64},
    /* 1200 */ {I_XOR, 2, {RM_GPR|BITS16,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14426, IF_8086},
    /* 1201 */ {I_XOR, 2, {RM_GPR|BITS32,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14432, IF_386},
    /* 1202 */ {I_XOR, 2, {RM_GPR|BITS64,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+14438, IF_X64},
    /* 1203 */ {I_XOR, 2, {REG_AL,IMMEDIATE,0,0,0}, nasm_bytecodes+19672, IF_8086|IF_SM},
    /* 1204 */ {I_XOR, 2, {REG_AX,SBYTE16,0,0,0}, nasm_bytecodes+14426, IF_8086|IF_SM},
    /* 1205 */ {I_XOR, 2, {REG_AX,IMMEDIATE,0,0,0}, nasm_bytecodes+18552, IF_8086|IF_SM},
    /* 1206 */ {I_XOR, 2, {REG_EAX,SBYTE32,0,0,0}, nasm_bytecodes+14432, IF_386|IF_SM},
    /* 1207 */ {I_XOR, 2, {REG_EAX,IMMEDIATE,0,0,0}, nasm_bytecodes+18557, IF_386|IF_SM},
    /* 1208 */ {I_XOR, 2, {REG_RAX,SBYTE64,0,0,0}, nasm_bytecodes+14438, IF_X64|IF_SM},
    /* 1209 */ {I_XOR, 2, {REG_RAX,IMMEDIATE,0,0,0}, nasm_bytecodes+18562, IF_X64|IF_SM},
    /* 1210 */ {I_XOR, 2, {RM_GPR|BITS8,IMMEDIATE,0,0,0}, nasm_bytecodes+18567, IF_8086|IF_SM},
    /* 1211 */ {I_XOR, 2, {RM_GPR|BITS16,IMMEDIATE,0,0,0}, nasm_bytecodes+14444, IF_8086|IF_SM},
    /* 1212 */ {I_XOR, 2, {RM_GPR|BITS32,IMMEDIATE,0,0,0}, nasm_bytecodes+14450, IF_386|IF_SM},
    /* 1213 */ {I_XOR, 2, {RM_GPR|BITS64,IMMEDIATE,0,0,0}, nasm_bytecodes+14456, IF_X64|IF_SM},
    /* 1214 */ {I_XOR, 2, {MEMORY,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+18567, IF_8086|IF_SM},
    /* 1215 */ {I_XOR, 2, {MEMORY,IMMEDIATE|BITS16,0,0,0}, nasm_bytecodes+14444, IF_8086|IF_SM},
    /* 1216 */ {I_XOR, 2, {MEMORY,IMMEDIATE|BITS32,0,0,0}, nasm_bytecodes+14450, IF_386|IF_SM},
    /* 1217 */ {I_CMOVcc, 2, {REG_GPR|BITS16,MEMORY,0,0,0}, nasm_bytecodes+7501, IF_P6|IF_SM},
    /* 1218 */ {I_CMOVcc, 2, {REG_GPR|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+7501, IF_P6},
    /* 1219 */ {I_CMOVcc, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+7508, IF_P6|IF_SM},
    /* 1220 */ {I_CMOVcc, 2, {REG_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+7508, IF_P6},
    /* 1221 */ {I_CMOVcc, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+7515, IF_X64|IF_SM},
    /* 1222 */ {I_CMOVcc, 2, {REG_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+7515, IF_X64},
    /* 1223 */ {I_Jcc, 1, {IMMEDIATE|NEAR,0,0,0,0}, nasm_bytecodes+7522, IF_386},
    /* 1224 */ {I_Jcc, 1, {IMMEDIATE|BITS16|NEAR,0,0,0,0}, nasm_bytecodes+7529, IF_386},
    /* 1225 */ {I_Jcc, 1, {IMMEDIATE|BITS32|NEAR,0,0,0,0}, nasm_bytecodes+7536, IF_386},
    /* 1226 */ {I_Jcc, 1, {IMMEDIATE,0,0,0,0}, nasm_bytecodes+18573, IF_8086},
    /* 1227 */ {I_SETcc, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14462, IF_386|IF_SB},
    /* 1228 */ {I_SETcc, 1, {REG_GPR|BITS8,0,0,0,0}, nasm_bytecodes+14462, IF_386},
    /* 1229 */ {I_ADDPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14468, IF_KATMAI|IF_SSE},
    /* 1230 */ {I_ADDSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14474, IF_KATMAI|IF_SSE|IF_SD},
    /* 1231 */ {I_ANDNPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14480, IF_KATMAI|IF_SSE},
    /* 1232 */ {I_ANDPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14486, IF_KATMAI|IF_SSE},
    /* 1233 */ {I_CMPEQPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5336, IF_KATMAI|IF_SSE},
    /* 1234 */ {I_CMPEQSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5344, IF_KATMAI|IF_SSE},
    /* 1235 */ {I_CMPLEPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5352, IF_KATMAI|IF_SSE},
    /* 1236 */ {I_CMPLESS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5360, IF_KATMAI|IF_SSE},
    /* 1237 */ {I_CMPLTPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5368, IF_KATMAI|IF_SSE},
    /* 1238 */ {I_CMPLTSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5376, IF_KATMAI|IF_SSE},
    /* 1239 */ {I_CMPNEQPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5384, IF_KATMAI|IF_SSE},
    /* 1240 */ {I_CMPNEQSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5392, IF_KATMAI|IF_SSE},
    /* 1241 */ {I_CMPNLEPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5400, IF_KATMAI|IF_SSE},
    /* 1242 */ {I_CMPNLESS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5408, IF_KATMAI|IF_SSE},
    /* 1243 */ {I_CMPNLTPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5416, IF_KATMAI|IF_SSE},
    /* 1244 */ {I_CMPNLTSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5424, IF_KATMAI|IF_SSE},
    /* 1245 */ {I_CMPORDPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5432, IF_KATMAI|IF_SSE},
    /* 1246 */ {I_CMPORDSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5440, IF_KATMAI|IF_SSE},
    /* 1247 */ {I_CMPUNORDPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5448, IF_KATMAI|IF_SSE},
    /* 1248 */ {I_CMPUNORDSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5456, IF_KATMAI|IF_SSE},
    /* 1249 */ {I_CMPPS, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+7550, IF_KATMAI|IF_SSE|IF_SB|IF_AR2},
    /* 1250 */ {I_CMPPS, 3, {XMMREG,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+7550, IF_KATMAI|IF_SSE|IF_SB|IF_AR2},
    /* 1251 */ {I_CMPSS, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+7557, IF_KATMAI|IF_SSE|IF_SB|IF_AR2},
    /* 1252 */ {I_CMPSS, 3, {XMMREG,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+7557, IF_KATMAI|IF_SSE|IF_SB|IF_AR2},
    /* 1253 */ {I_COMISS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14492, IF_KATMAI|IF_SSE},
    /* 1254 */ {I_CVTPI2PS, 2, {XMMREG,RM_MMX,0,0,0}, nasm_bytecodes+14498, IF_KATMAI|IF_SSE|IF_MMX|IF_SQ},
    /* 1255 */ {I_CVTPS2PI, 2, {MMXREG,RM_XMM,0,0,0}, nasm_bytecodes+14504, IF_KATMAI|IF_SSE|IF_MMX|IF_SQ},
    /* 1256 */ {I_CVTSI2SS, 2, {XMMREG,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+7565, IF_KATMAI|IF_SSE|IF_SD|IF_AR1},
    /* 1257 */ {I_CVTSI2SS, 2, {XMMREG,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+7564, IF_X64|IF_SSE|IF_SQ|IF_AR1},
    /* 1258 */ {I_CVTSS2SI, 2, {REG_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+7572, IF_KATMAI|IF_SSE|IF_SD|IF_AR1},
    /* 1259 */ {I_CVTSS2SI, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+7572, IF_KATMAI|IF_SSE|IF_SD|IF_AR1},
    /* 1260 */ {I_CVTSS2SI, 2, {REG_GPR|BITS64,XMMREG,0,0,0}, nasm_bytecodes+7571, IF_X64|IF_SSE|IF_SD|IF_AR1},
    /* 1261 */ {I_CVTSS2SI, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+7571, IF_X64|IF_SSE|IF_SD|IF_AR1},
    /* 1262 */ {I_CVTTPS2PI, 2, {MMXREG,RM_XMM,0,0,0}, nasm_bytecodes+14510, IF_KATMAI|IF_SSE|IF_MMX|IF_SQ},
    /* 1263 */ {I_CVTTSS2SI, 2, {REG_GPR|BITS32,RM_XMM,0,0,0}, nasm_bytecodes+7579, IF_KATMAI|IF_SSE|IF_SD|IF_AR1},
    /* 1264 */ {I_CVTTSS2SI, 2, {REG_GPR|BITS64,RM_XMM,0,0,0}, nasm_bytecodes+7578, IF_X64|IF_SSE|IF_SD|IF_AR1},
    /* 1265 */ {I_DIVPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14516, IF_KATMAI|IF_SSE},
    /* 1266 */ {I_DIVSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14522, IF_KATMAI|IF_SSE},
    /* 1267 */ {I_LDMXCSR, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+12536, IF_KATMAI|IF_SSE|IF_SD},
    /* 1268 */ {I_MAXPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14528, IF_KATMAI|IF_SSE},
    /* 1269 */ {I_MAXSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14534, IF_KATMAI|IF_SSE},
    /* 1270 */ {I_MINPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14540, IF_KATMAI|IF_SSE},
    /* 1271 */ {I_MINSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14546, IF_KATMAI|IF_SSE},
    /* 1272 */ {I_MOVAPS, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+14552, IF_KATMAI|IF_SSE},
    /* 1273 */ {I_MOVAPS, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14558, IF_KATMAI|IF_SSE},
    /* 1274 */ {I_MOVAPS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14552, IF_KATMAI|IF_SSE},
    /* 1275 */ {I_MOVAPS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14558, IF_KATMAI|IF_SSE},
    /* 1276 */ {I_MOVHPS, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+14564, IF_KATMAI|IF_SSE},
    /* 1277 */ {I_MOVHPS, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14570, IF_KATMAI|IF_SSE},
    /* 1278 */ {I_MOVLHPS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14564, IF_KATMAI|IF_SSE},
    /* 1279 */ {I_MOVLPS, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+14384, IF_KATMAI|IF_SSE},
    /* 1280 */ {I_MOVLPS, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14576, IF_KATMAI|IF_SSE},
    /* 1281 */ {I_MOVHLPS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14384, IF_KATMAI|IF_SSE},
    /* 1282 */ {I_MOVMSKPS, 2, {REG_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+14582, IF_KATMAI|IF_SSE},
    /* 1283 */ {I_MOVMSKPS, 2, {REG_GPR|BITS64,XMMREG,0,0,0}, nasm_bytecodes+7585, IF_X64|IF_SSE},
    /* 1284 */ {I_MOVNTPS, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14588, IF_KATMAI|IF_SSE},
    /* 1285 */ {I_MOVSS, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+14594, IF_KATMAI|IF_SSE},
    /* 1286 */ {I_MOVSS, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14600, IF_KATMAI|IF_SSE},
    /* 1287 */ {I_MOVSS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14594, IF_KATMAI|IF_SSE},
    /* 1288 */ {I_MOVSS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14600, IF_KATMAI|IF_SSE},
    /* 1289 */ {I_MOVUPS, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+14606, IF_KATMAI|IF_SSE},
    /* 1290 */ {I_MOVUPS, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14612, IF_KATMAI|IF_SSE},
    /* 1291 */ {I_MOVUPS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14606, IF_KATMAI|IF_SSE},
    /* 1292 */ {I_MOVUPS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14612, IF_KATMAI|IF_SSE},
    /* 1293 */ {I_MULPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14618, IF_KATMAI|IF_SSE},
    /* 1294 */ {I_MULSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14624, IF_KATMAI|IF_SSE},
    /* 1295 */ {I_ORPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14630, IF_KATMAI|IF_SSE},
    /* 1296 */ {I_RCPPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14636, IF_KATMAI|IF_SSE},
    /* 1297 */ {I_RCPSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14642, IF_KATMAI|IF_SSE},
    /* 1298 */ {I_RSQRTPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14648, IF_KATMAI|IF_SSE},
    /* 1299 */ {I_RSQRTSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14654, IF_KATMAI|IF_SSE},
    /* 1300 */ {I_SHUFPS, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+7592, IF_KATMAI|IF_SSE|IF_SB|IF_AR2},
    /* 1301 */ {I_SHUFPS, 3, {XMMREG,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+7592, IF_KATMAI|IF_SSE|IF_SB|IF_AR2},
    /* 1302 */ {I_SQRTPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14660, IF_KATMAI|IF_SSE},
    /* 1303 */ {I_SQRTSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14666, IF_KATMAI|IF_SSE},
    /* 1304 */ {I_STMXCSR, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+12543, IF_KATMAI|IF_SSE|IF_SD},
    /* 1305 */ {I_SUBPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14672, IF_KATMAI|IF_SSE},
    /* 1306 */ {I_SUBSS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14678, IF_KATMAI|IF_SSE},
    /* 1307 */ {I_UCOMISS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14684, IF_KATMAI|IF_SSE},
    /* 1308 */ {I_UNPCKHPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14690, IF_KATMAI|IF_SSE},
    /* 1309 */ {I_UNPCKLPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14696, IF_KATMAI|IF_SSE},
    /* 1310 */ {I_XORPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14702, IF_KATMAI|IF_SSE},
    /* 1311 */ {I_FXRSTOR, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14709, IF_P6|IF_SSE|IF_FPU},
    /* 1312 */ {I_FXRSTOR64, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14708, IF_X64|IF_SSE|IF_FPU},
    /* 1313 */ {I_FXSAVE, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14715, IF_P6|IF_SSE|IF_FPU},
    /* 1314 */ {I_FXSAVE64, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14714, IF_X64|IF_SSE|IF_FPU},
    /* 1315 */ {I_XGETBV, 0, {0,0,0,0,0}, nasm_bytecodes+14720, IF_NEHALEM},
    /* 1316 */ {I_XSETBV, 0, {0,0,0,0,0}, nasm_bytecodes+14726, IF_NEHALEM|IF_PRIV},
    /* 1317 */ {I_XSAVE, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14733, IF_NEHALEM},
    /* 1318 */ {I_XSAVE64, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14732, IF_LONG|IF_NEHALEM},
    /* 1319 */ {I_XSAVEOPT, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14739, IF_FUTURE},
    /* 1320 */ {I_XSAVEOPT64, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14738, IF_LONG|IF_FUTURE},
    /* 1321 */ {I_XRSTOR, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14745, IF_NEHALEM},
    /* 1322 */ {I_XRSTOR64, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+14744, IF_LONG|IF_NEHALEM},
    /* 1323 */ {I_PREFETCHNTA, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+15651, IF_KATMAI},
    /* 1324 */ {I_PREFETCHT0, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+15669, IF_KATMAI},
    /* 1325 */ {I_PREFETCHT1, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+15687, IF_KATMAI},
    /* 1326 */ {I_PREFETCHT2, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+15705, IF_KATMAI},
    /* 1327 */ {I_SFENCE, 0, {0,0,0,0,0}, nasm_bytecodes+18302, IF_KATMAI},
    /* 1328 */ {I_MASKMOVQ, 2, {MMXREG,MMXREG,0,0,0}, nasm_bytecodes+14750, IF_KATMAI|IF_MMX},
    /* 1329 */ {I_MOVNTQ, 2, {MEMORY,MMXREG,0,0,0}, nasm_bytecodes+14756, IF_KATMAI|IF_MMX|IF_SQ},
    /* 1330 */ {I_PAVGB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7599, IF_KATMAI|IF_MMX|IF_SQ},
    /* 1331 */ {I_PAVGW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7606, IF_KATMAI|IF_MMX|IF_SQ},
    /* 1332 */ {I_PEXTRW, 3, {REG_GPR|BITS32,MMXREG,IMMEDIATE,0,0}, nasm_bytecodes+7613, IF_KATMAI|IF_MMX|IF_SB|IF_AR2},
    /* 1333 */ {I_PINSRW, 3, {MMXREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+7620, IF_KATMAI|IF_MMX|IF_SB|IF_AR2},
    /* 1334 */ {I_PINSRW, 3, {MMXREG,RM_GPR|BITS16,IMMEDIATE,0,0}, nasm_bytecodes+7620, IF_KATMAI|IF_MMX|IF_SB|IF_AR2},
    /* 1335 */ {I_PINSRW, 3, {MMXREG,REG_GPR|BITS32,IMMEDIATE,0,0}, nasm_bytecodes+7620, IF_KATMAI|IF_MMX|IF_SB|IF_AR2},
    /* 1336 */ {I_PMAXSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7627, IF_KATMAI|IF_MMX|IF_SQ},
    /* 1337 */ {I_PMAXUB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7634, IF_KATMAI|IF_MMX|IF_SQ},
    /* 1338 */ {I_PMINSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7641, IF_KATMAI|IF_MMX|IF_SQ},
    /* 1339 */ {I_PMINUB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7648, IF_KATMAI|IF_MMX|IF_SQ},
    /* 1340 */ {I_PMOVMSKB, 2, {REG_GPR|BITS32,MMXREG,0,0,0}, nasm_bytecodes+14762, IF_KATMAI|IF_MMX},
    /* 1341 */ {I_PMULHUW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7655, IF_KATMAI|IF_MMX|IF_SQ},
    /* 1342 */ {I_PSADBW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7662, IF_KATMAI|IF_MMX|IF_SQ},
    /* 1343 */ {I_PSHUFW, 3, {MMXREG,RM_MMX,IMMEDIATE,0,0}, nasm_bytecodes+5464, IF_KATMAI|IF_MMX|IF_SM2|IF_SB|IF_AR2},
    /* 1344 */ {I_PF2IW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5472, IF_PENT|IF_3DNOW|IF_SQ},
    /* 1345 */ {I_PFNACC, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5480, IF_PENT|IF_3DNOW|IF_SQ},
    /* 1346 */ {I_PFPNACC, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5488, IF_PENT|IF_3DNOW|IF_SQ},
    /* 1347 */ {I_PI2FW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5496, IF_PENT|IF_3DNOW|IF_SQ},
    /* 1348 */ {I_PSWAPD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5504, IF_PENT|IF_3DNOW|IF_SQ},
    /* 1349 */ {I_MASKMOVDQU, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14768, IF_WILLAMETTE|IF_SSE2},
    /* 1350 */ {I_CLFLUSH, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+18577, IF_WILLAMETTE|IF_SSE2},
    /* 1351 */ {I_MOVNTDQ, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14774, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1352 */ {I_MOVNTI, 2, {MEMORY,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+7670, IF_WILLAMETTE|IF_SD},
    /* 1353 */ {I_MOVNTI, 2, {MEMORY,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+7669, IF_X64|IF_SQ},
    /* 1354 */ {I_MOVNTPD, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14780, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1355 */ {I_LFENCE, 0, {0,0,0,0,0}, nasm_bytecodes+17637, IF_WILLAMETTE|IF_SSE2},
    /* 1356 */ {I_MFENCE, 0, {0,0,0,0,0}, nasm_bytecodes+17727, IF_WILLAMETTE|IF_SSE2},
    /* 1357 */ {I_MOVD, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+7676, IF_WILLAMETTE|IF_SSE2|IF_SD},
    /* 1358 */ {I_MOVD, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+7683, IF_WILLAMETTE|IF_SSE2|IF_SD},
    /* 1359 */ {I_MOVD, 2, {XMMREG,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+7683, IF_WILLAMETTE|IF_SSE2},
    /* 1360 */ {I_MOVD, 2, {RM_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+7676, IF_WILLAMETTE|IF_SSE2},
    /* 1361 */ {I_MOVDQA, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14786, IF_WILLAMETTE|IF_SSE2},
    /* 1362 */ {I_MOVDQA, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14792, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1363 */ {I_MOVDQA, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+14786, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1364 */ {I_MOVDQA, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14792, IF_WILLAMETTE|IF_SSE2},
    /* 1365 */ {I_MOVDQU, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14798, IF_WILLAMETTE|IF_SSE2},
    /* 1366 */ {I_MOVDQU, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14804, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1367 */ {I_MOVDQU, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+14798, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1368 */ {I_MOVDQU, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14804, IF_WILLAMETTE|IF_SSE2},
    /* 1369 */ {I_MOVDQ2Q, 2, {MMXREG,XMMREG,0,0,0}, nasm_bytecodes+14810, IF_WILLAMETTE|IF_SSE2},
    /* 1370 */ {I_MOVQ, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14816, IF_WILLAMETTE|IF_SSE2},
    /* 1371 */ {I_MOVQ, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+14822, IF_WILLAMETTE|IF_SSE2},
    /* 1372 */ {I_MOVQ, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+14822, IF_WILLAMETTE|IF_SSE2|IF_SQ},
    /* 1373 */ {I_MOVQ, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+14816, IF_WILLAMETTE|IF_SSE2|IF_SQ},
    /* 1374 */ {I_MOVQ, 2, {XMMREG,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+7690, IF_X64|IF_SSE2},
    /* 1375 */ {I_MOVQ, 2, {RM_GPR|BITS64,XMMREG,0,0,0}, nasm_bytecodes+7697, IF_X64|IF_SSE2},
    /* 1376 */ {I_MOVQ2DQ, 2, {XMMREG,MMXREG,0,0,0}, nasm_bytecodes+14828, IF_WILLAMETTE|IF_SSE2},
    /* 1377 */ {I_PACKSSWB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14834, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1378 */ {I_PACKSSDW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14840, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1379 */ {I_PACKUSWB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14846, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1380 */ {I_PADDB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14852, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1381 */ {I_PADDW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14858, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1382 */ {I_PADDD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14864, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1383 */ {I_PADDQ, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+14870, IF_WILLAMETTE|IF_MMX|IF_SQ},
    /* 1384 */ {I_PADDQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14876, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1385 */ {I_PADDSB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14882, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1386 */ {I_PADDSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14888, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1387 */ {I_PADDUSB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14894, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1388 */ {I_PADDUSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14900, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1389 */ {I_PAND, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14906, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1390 */ {I_PANDN, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14912, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1391 */ {I_PAVGB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14918, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1392 */ {I_PAVGW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14924, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1393 */ {I_PCMPEQB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14930, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1394 */ {I_PCMPEQW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14936, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1395 */ {I_PCMPEQD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14942, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1396 */ {I_PCMPGTB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14948, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1397 */ {I_PCMPGTW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14954, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1398 */ {I_PCMPGTD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14960, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1399 */ {I_PEXTRW, 3, {REG_GPR|BITS32,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+7704, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR2},
    /* 1400 */ {I_PINSRW, 3, {XMMREG,REG_GPR|BITS16,IMMEDIATE,0,0}, nasm_bytecodes+7711, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR2},
    /* 1401 */ {I_PINSRW, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+7711, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR2},
    /* 1402 */ {I_PINSRW, 3, {XMMREG,MEMORY|BITS16,IMMEDIATE,0,0}, nasm_bytecodes+7711, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR2},
    /* 1403 */ {I_PMADDWD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14966, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1404 */ {I_PMAXSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14972, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1405 */ {I_PMAXUB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14978, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1406 */ {I_PMINSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14984, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1407 */ {I_PMINUB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+14990, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1408 */ {I_PMOVMSKB, 2, {REG_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+14996, IF_WILLAMETTE|IF_SSE2},
    /* 1409 */ {I_PMULHUW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15002, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1410 */ {I_PMULHW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15008, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1411 */ {I_PMULLW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15014, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1412 */ {I_PMULUDQ, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7718, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1413 */ {I_PMULUDQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15020, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1414 */ {I_POR, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15026, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1415 */ {I_PSADBW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15032, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1416 */ {I_PSHUFD, 3, {XMMREG,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+7725, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR2},
    /* 1417 */ {I_PSHUFD, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+7725, IF_WILLAMETTE|IF_SSE2|IF_SM2|IF_SB|IF_AR2},
    /* 1418 */ {I_PSHUFHW, 3, {XMMREG,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+7732, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR2},
    /* 1419 */ {I_PSHUFHW, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+7732, IF_WILLAMETTE|IF_SSE2|IF_SM2|IF_SB|IF_AR2},
    /* 1420 */ {I_PSHUFLW, 3, {XMMREG,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+7739, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR2},
    /* 1421 */ {I_PSHUFLW, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+7739, IF_WILLAMETTE|IF_SSE2|IF_SM2|IF_SB|IF_AR2},
    /* 1422 */ {I_PSLLDQ, 2, {XMMREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7746, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR1},
    /* 1423 */ {I_PSLLW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15038, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1424 */ {I_PSLLW, 2, {XMMREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7753, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR1},
    /* 1425 */ {I_PSLLD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15044, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1426 */ {I_PSLLD, 2, {XMMREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7760, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR1},
    /* 1427 */ {I_PSLLQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15050, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1428 */ {I_PSLLQ, 2, {XMMREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7767, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR1},
    /* 1429 */ {I_PSRAW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15056, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1430 */ {I_PSRAW, 2, {XMMREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7774, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR1},
    /* 1431 */ {I_PSRAD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15062, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1432 */ {I_PSRAD, 2, {XMMREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7781, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR1},
    /* 1433 */ {I_PSRLDQ, 2, {XMMREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7788, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR1},
    /* 1434 */ {I_PSRLW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15068, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1435 */ {I_PSRLW, 2, {XMMREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7795, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR1},
    /* 1436 */ {I_PSRLD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15074, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1437 */ {I_PSRLD, 2, {XMMREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7802, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR1},
    /* 1438 */ {I_PSRLQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15080, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1439 */ {I_PSRLQ, 2, {XMMREG,IMMEDIATE,0,0,0}, nasm_bytecodes+7809, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR1},
    /* 1440 */ {I_PSUBB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15086, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1441 */ {I_PSUBW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15092, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1442 */ {I_PSUBD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15098, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1443 */ {I_PSUBQ, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7816, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1444 */ {I_PSUBQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15104, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1445 */ {I_PSUBSB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15110, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1446 */ {I_PSUBSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15116, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1447 */ {I_PSUBUSB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15122, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1448 */ {I_PSUBUSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15128, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1449 */ {I_PUNPCKHBW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15134, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1450 */ {I_PUNPCKHWD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15140, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1451 */ {I_PUNPCKHDQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15146, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1452 */ {I_PUNPCKHQDQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15152, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1453 */ {I_PUNPCKLBW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15158, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1454 */ {I_PUNPCKLWD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15164, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1455 */ {I_PUNPCKLDQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15170, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1456 */ {I_PUNPCKLQDQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15176, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1457 */ {I_PXOR, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15182, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1458 */ {I_ADDPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15188, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1459 */ {I_ADDSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15194, IF_WILLAMETTE|IF_SSE2|IF_SQ},
    /* 1460 */ {I_ANDNPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15200, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1461 */ {I_ANDPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15206, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1462 */ {I_CMPEQPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5512, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1463 */ {I_CMPEQSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5520, IF_WILLAMETTE|IF_SSE2},
    /* 1464 */ {I_CMPLEPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5528, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1465 */ {I_CMPLESD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5536, IF_WILLAMETTE|IF_SSE2},
    /* 1466 */ {I_CMPLTPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5544, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1467 */ {I_CMPLTSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5552, IF_WILLAMETTE|IF_SSE2},
    /* 1468 */ {I_CMPNEQPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5560, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1469 */ {I_CMPNEQSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5568, IF_WILLAMETTE|IF_SSE2},
    /* 1470 */ {I_CMPNLEPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5576, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1471 */ {I_CMPNLESD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5584, IF_WILLAMETTE|IF_SSE2},
    /* 1472 */ {I_CMPNLTPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5592, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1473 */ {I_CMPNLTSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5600, IF_WILLAMETTE|IF_SSE2},
    /* 1474 */ {I_CMPORDPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5608, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1475 */ {I_CMPORDSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5616, IF_WILLAMETTE|IF_SSE2},
    /* 1476 */ {I_CMPUNORDPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5624, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1477 */ {I_CMPUNORDSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+5632, IF_WILLAMETTE|IF_SSE2},
    /* 1478 */ {I_CMPPD, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+7823, IF_WILLAMETTE|IF_SSE2|IF_SM2|IF_SB|IF_AR2},
    /* 1479 */ {I_CMPSD, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+7830, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR2},
    /* 1480 */ {I_COMISD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15212, IF_WILLAMETTE|IF_SSE2},
    /* 1481 */ {I_CVTDQ2PD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15218, IF_WILLAMETTE|IF_SSE2|IF_SQ},
    /* 1482 */ {I_CVTDQ2PS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15224, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1483 */ {I_CVTPD2DQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15230, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1484 */ {I_CVTPD2PI, 2, {MMXREG,RM_XMM,0,0,0}, nasm_bytecodes+15236, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1485 */ {I_CVTPD2PS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15242, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1486 */ {I_CVTPI2PD, 2, {XMMREG,RM_MMX,0,0,0}, nasm_bytecodes+15248, IF_WILLAMETTE|IF_SSE2|IF_SQ},
    /* 1487 */ {I_CVTPS2DQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15254, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1488 */ {I_CVTPS2PD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15260, IF_WILLAMETTE|IF_SSE2|IF_SQ},
    /* 1489 */ {I_CVTSD2SI, 2, {REG_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+7838, IF_WILLAMETTE|IF_SSE2|IF_SQ|IF_AR1},
    /* 1490 */ {I_CVTSD2SI, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+7838, IF_WILLAMETTE|IF_SSE2|IF_SQ|IF_AR1},
    /* 1491 */ {I_CVTSD2SI, 2, {REG_GPR|BITS64,XMMREG,0,0,0}, nasm_bytecodes+7837, IF_X64|IF_SSE2|IF_SQ|IF_AR1},
    /* 1492 */ {I_CVTSD2SI, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+7837, IF_X64|IF_SSE2|IF_SQ|IF_AR1},
    /* 1493 */ {I_CVTSD2SS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15266, IF_WILLAMETTE|IF_SSE2|IF_SQ},
    /* 1494 */ {I_CVTSI2SD, 2, {XMMREG,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+7845, IF_WILLAMETTE|IF_SSE2|IF_SD|IF_AR1},
    /* 1495 */ {I_CVTSI2SD, 2, {XMMREG,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+7844, IF_X64|IF_SSE2|IF_SQ|IF_AR1},
    /* 1496 */ {I_CVTSS2SD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15272, IF_WILLAMETTE|IF_SSE2|IF_SD},
    /* 1497 */ {I_CVTTPD2PI, 2, {MMXREG,RM_XMM,0,0,0}, nasm_bytecodes+15278, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1498 */ {I_CVTTPD2DQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15284, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1499 */ {I_CVTTPS2DQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15290, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1500 */ {I_CVTTSD2SI, 2, {REG_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+7852, IF_WILLAMETTE|IF_SSE2|IF_SQ|IF_AR1},
    /* 1501 */ {I_CVTTSD2SI, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+7852, IF_WILLAMETTE|IF_SSE2|IF_SQ|IF_AR1},
    /* 1502 */ {I_CVTTSD2SI, 2, {REG_GPR|BITS64,XMMREG,0,0,0}, nasm_bytecodes+7851, IF_X64|IF_SSE2|IF_SQ|IF_AR1},
    /* 1503 */ {I_CVTTSD2SI, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+7851, IF_X64|IF_SSE2|IF_SQ|IF_AR1},
    /* 1504 */ {I_DIVPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15296, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1505 */ {I_DIVSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15302, IF_WILLAMETTE|IF_SSE2},
    /* 1506 */ {I_MAXPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15308, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1507 */ {I_MAXSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15314, IF_WILLAMETTE|IF_SSE2},
    /* 1508 */ {I_MINPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15320, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1509 */ {I_MINSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15326, IF_WILLAMETTE|IF_SSE2},
    /* 1510 */ {I_MOVAPD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+15332, IF_WILLAMETTE|IF_SSE2},
    /* 1511 */ {I_MOVAPD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+15338, IF_WILLAMETTE|IF_SSE2},
    /* 1512 */ {I_MOVAPD, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+15338, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1513 */ {I_MOVAPD, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+15332, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1514 */ {I_MOVHPD, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+15344, IF_WILLAMETTE|IF_SSE2},
    /* 1515 */ {I_MOVHPD, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+15350, IF_WILLAMETTE|IF_SSE2},
    /* 1516 */ {I_MOVLPD, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+15356, IF_WILLAMETTE|IF_SSE2},
    /* 1517 */ {I_MOVLPD, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+15362, IF_WILLAMETTE|IF_SSE2},
    /* 1518 */ {I_MOVMSKPD, 2, {REG_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+15368, IF_WILLAMETTE|IF_SSE2},
    /* 1519 */ {I_MOVMSKPD, 2, {REG_GPR|BITS64,XMMREG,0,0,0}, nasm_bytecodes+7858, IF_X64|IF_SSE2},
    /* 1520 */ {I_MOVSD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+15374, IF_WILLAMETTE|IF_SSE2},
    /* 1521 */ {I_MOVSD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+15380, IF_WILLAMETTE|IF_SSE2},
    /* 1522 */ {I_MOVSD, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+15380, IF_WILLAMETTE|IF_SSE2},
    /* 1523 */ {I_MOVSD, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+15374, IF_WILLAMETTE|IF_SSE2},
    /* 1524 */ {I_MOVUPD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+15386, IF_WILLAMETTE|IF_SSE2},
    /* 1525 */ {I_MOVUPD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+15392, IF_WILLAMETTE|IF_SSE2},
    /* 1526 */ {I_MOVUPD, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+15392, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1527 */ {I_MOVUPD, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+15386, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1528 */ {I_MULPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15398, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1529 */ {I_MULSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15404, IF_WILLAMETTE|IF_SSE2},
    /* 1530 */ {I_ORPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15410, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1531 */ {I_SHUFPD, 3, {XMMREG,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+7865, IF_WILLAMETTE|IF_SSE2|IF_SB|IF_AR2},
    /* 1532 */ {I_SHUFPD, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+7865, IF_WILLAMETTE|IF_SSE2|IF_SM|IF_SB|IF_AR2},
    /* 1533 */ {I_SQRTPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15416, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1534 */ {I_SQRTSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15422, IF_WILLAMETTE|IF_SSE2},
    /* 1535 */ {I_SUBPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15428, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1536 */ {I_SUBSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15434, IF_WILLAMETTE|IF_SSE2},
    /* 1537 */ {I_UCOMISD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15440, IF_WILLAMETTE|IF_SSE2},
    /* 1538 */ {I_UNPCKHPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15446, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1539 */ {I_UNPCKLPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15452, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1540 */ {I_XORPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15458, IF_WILLAMETTE|IF_SSE2|IF_SO},
    /* 1541 */ {I_ADDSUBPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15464, IF_PRESCOTT|IF_SSE3|IF_SO},
    /* 1542 */ {I_ADDSUBPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15470, IF_PRESCOTT|IF_SSE3|IF_SO},
    /* 1543 */ {I_HADDPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15476, IF_PRESCOTT|IF_SSE3|IF_SO},
    /* 1544 */ {I_HADDPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15482, IF_PRESCOTT|IF_SSE3|IF_SO},
    /* 1545 */ {I_HSUBPD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15488, IF_PRESCOTT|IF_SSE3|IF_SO},
    /* 1546 */ {I_HSUBPS, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15494, IF_PRESCOTT|IF_SSE3|IF_SO},
    /* 1547 */ {I_LDDQU, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+15500, IF_PRESCOTT|IF_SSE3|IF_SO},
    /* 1548 */ {I_MOVDDUP, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15506, IF_PRESCOTT|IF_SSE3},
    /* 1549 */ {I_MOVSHDUP, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15512, IF_PRESCOTT|IF_SSE3},
    /* 1550 */ {I_MOVSLDUP, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+15518, IF_PRESCOTT|IF_SSE3},
    /* 1551 */ {I_VMCALL, 0, {0,0,0,0,0}, nasm_bytecodes+18582, IF_VMX},
    /* 1552 */ {I_VMCLEAR, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+15524, IF_VMX},
    /* 1553 */ {I_VMLAUNCH, 0, {0,0,0,0,0}, nasm_bytecodes+18587, IF_VMX},
    /* 1554 */ {I_VMLOAD, 0, {0,0,0,0,0}, nasm_bytecodes+18592, IF_X64|IF_VMX},
    /* 1555 */ {I_VMMCALL, 0, {0,0,0,0,0}, nasm_bytecodes+18597, IF_X64|IF_VMX},
    /* 1556 */ {I_VMPTRLD, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+15585, IF_VMX},
    /* 1557 */ {I_VMPTRST, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+18602, IF_VMX},
    /* 1558 */ {I_VMREAD, 2, {RM_GPR|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+7873, IF_VMX|IF_NOLONG|IF_SD},
    /* 1559 */ {I_VMREAD, 2, {RM_GPR|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+7872, IF_X64|IF_VMX|IF_SQ},
    /* 1560 */ {I_VMRESUME, 0, {0,0,0,0,0}, nasm_bytecodes+18607, IF_VMX},
    /* 1561 */ {I_VMRUN, 0, {0,0,0,0,0}, nasm_bytecodes+18612, IF_X64|IF_VMX},
    /* 1562 */ {I_VMSAVE, 0, {0,0,0,0,0}, nasm_bytecodes+18617, IF_X64|IF_VMX},
    /* 1563 */ {I_VMWRITE, 2, {REG_GPR|BITS32,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+7880, IF_VMX|IF_NOLONG|IF_SD},
    /* 1564 */ {I_VMWRITE, 2, {REG_GPR|BITS64,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+7879, IF_X64|IF_VMX|IF_SQ},
    /* 1565 */ {I_VMXOFF, 0, {0,0,0,0,0}, nasm_bytecodes+18622, IF_VMX},
    /* 1566 */ {I_VMXON, 1, {MEMORY,0,0,0,0}, nasm_bytecodes+15530, IF_VMX},
    /* 1567 */ {I_INVEPT, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+5641, IF_VMX|IF_SO|IF_NOLONG},
    /* 1568 */ {I_INVEPT, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+5640, IF_VMX|IF_SO|IF_LONG},
    /* 1569 */ {I_INVVPID, 2, {REG_GPR|BITS32,MEMORY,0,0,0}, nasm_bytecodes+5649, IF_VMX|IF_SO|IF_NOLONG},
    /* 1570 */ {I_INVVPID, 2, {REG_GPR|BITS64,MEMORY,0,0,0}, nasm_bytecodes+5648, IF_VMX|IF_SO|IF_LONG},
    /* 1571 */ {I_PABSB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7886, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1572 */ {I_PABSB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+7893, IF_SSSE3},
    /* 1573 */ {I_PABSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7900, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1574 */ {I_PABSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+7907, IF_SSSE3},
    /* 1575 */ {I_PABSD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7914, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1576 */ {I_PABSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+7921, IF_SSSE3},
    /* 1577 */ {I_PALIGNR, 3, {MMXREG,RM_MMX,IMMEDIATE,0,0}, nasm_bytecodes+5656, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1578 */ {I_PALIGNR, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5664, IF_SSSE3},
    /* 1579 */ {I_PHADDW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7928, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1580 */ {I_PHADDW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+7935, IF_SSSE3},
    /* 1581 */ {I_PHADDD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7942, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1582 */ {I_PHADDD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+7949, IF_SSSE3},
    /* 1583 */ {I_PHADDSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7956, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1584 */ {I_PHADDSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+7963, IF_SSSE3},
    /* 1585 */ {I_PHSUBW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7970, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1586 */ {I_PHSUBW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+7977, IF_SSSE3},
    /* 1587 */ {I_PHSUBD, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7984, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1588 */ {I_PHSUBD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+7991, IF_SSSE3},
    /* 1589 */ {I_PHSUBSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+7998, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1590 */ {I_PHSUBSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8005, IF_SSSE3},
    /* 1591 */ {I_PMADDUBSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+8012, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1592 */ {I_PMADDUBSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8019, IF_SSSE3},
    /* 1593 */ {I_PMULHRSW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+8026, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1594 */ {I_PMULHRSW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8033, IF_SSSE3},
    /* 1595 */ {I_PSHUFB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+8040, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1596 */ {I_PSHUFB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8047, IF_SSSE3},
    /* 1597 */ {I_PSIGNB, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+8054, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1598 */ {I_PSIGNB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8061, IF_SSSE3},
    /* 1599 */ {I_PSIGNW, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+8068, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1600 */ {I_PSIGNW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8075, IF_SSSE3},
    /* 1601 */ {I_PSIGND, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+8082, IF_SSSE3|IF_MMX|IF_SQ},
    /* 1602 */ {I_PSIGND, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8089, IF_SSSE3},
    /* 1603 */ {I_EXTRQ, 3, {XMMREG,IMMEDIATE,IMMEDIATE,0,0}, nasm_bytecodes+5672, IF_SSE4A|IF_AMD},
    /* 1604 */ {I_EXTRQ, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+15536, IF_SSE4A|IF_AMD},
    /* 1605 */ {I_INSERTQ, 4, {XMMREG,XMMREG,IMMEDIATE,IMMEDIATE,0}, nasm_bytecodes+5680, IF_SSE4A|IF_AMD},
    /* 1606 */ {I_INSERTQ, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+15542, IF_SSE4A|IF_AMD},
    /* 1607 */ {I_MOVNTSD, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+15548, IF_SSE4A|IF_AMD|IF_SQ},
    /* 1608 */ {I_MOVNTSS, 2, {MEMORY,XMMREG,0,0,0}, nasm_bytecodes+15554, IF_SSE4A|IF_AMD|IF_SD},
    /* 1609 */ {I_LZCNT, 2, {REG_GPR|BITS16,RM_GPR|BITS16,0,0,0}, nasm_bytecodes+8096, IF_P6|IF_AMD},
    /* 1610 */ {I_LZCNT, 2, {REG_GPR|BITS32,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+8103, IF_P6|IF_AMD},
    /* 1611 */ {I_LZCNT, 2, {REG_GPR|BITS64,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+8110, IF_X64|IF_AMD},
    /* 1612 */ {I_BLENDPD, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5688, IF_SSE41},
    /* 1613 */ {I_BLENDPS, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5696, IF_SSE41},
    /* 1614 */ {I_BLENDVPD, 3, {XMMREG,RM_XMM,XMM0,0,0}, nasm_bytecodes+8117, IF_SSE41},
    /* 1615 */ {I_BLENDVPS, 3, {XMMREG,RM_XMM,XMM0,0,0}, nasm_bytecodes+8124, IF_SSE41},
    /* 1616 */ {I_DPPD, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5704, IF_SSE41},
    /* 1617 */ {I_DPPS, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5712, IF_SSE41},
    /* 1618 */ {I_EXTRACTPS, 3, {RM_GPR|BITS32,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+1, IF_SSE41},
    /* 1619 */ {I_EXTRACTPS, 3, {REG_GPR|BITS64,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+0, IF_SSE41|IF_X64},
    /* 1620 */ {I_INSERTPS, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5720, IF_SSE41|IF_SD},
    /* 1621 */ {I_MOVNTDQA, 2, {XMMREG,MEMORY,0,0,0}, nasm_bytecodes+8131, IF_SSE41},
    /* 1622 */ {I_MPSADBW, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5728, IF_SSE41},
    /* 1623 */ {I_PACKUSDW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8138, IF_SSE41},
    /* 1624 */ {I_PBLENDVB, 3, {XMMREG,RM_XMM,XMM0,0,0}, nasm_bytecodes+8145, IF_SSE41},
    /* 1625 */ {I_PBLENDW, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5736, IF_SSE41},
    /* 1626 */ {I_PCMPEQQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8152, IF_SSE41},
    /* 1627 */ {I_PEXTRB, 3, {REG_GPR|BITS32,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+10, IF_SSE41},
    /* 1628 */ {I_PEXTRB, 3, {MEMORY|BITS8,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+10, IF_SSE41},
    /* 1629 */ {I_PEXTRB, 3, {REG_GPR|BITS64,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+9, IF_SSE41|IF_X64},
    /* 1630 */ {I_PEXTRD, 3, {RM_GPR|BITS32,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+19, IF_SSE41},
    /* 1631 */ {I_PEXTRQ, 3, {RM_GPR|BITS64,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+18, IF_SSE41|IF_X64},
    /* 1632 */ {I_PEXTRW, 3, {REG_GPR|BITS32,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+28, IF_SSE41},
    /* 1633 */ {I_PEXTRW, 3, {MEMORY|BITS16,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+28, IF_SSE41},
    /* 1634 */ {I_PEXTRW, 3, {REG_GPR|BITS64,XMMREG,IMMEDIATE,0,0}, nasm_bytecodes+27, IF_SSE41|IF_X64},
    /* 1635 */ {I_PHMINPOSUW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8159, IF_SSE41},
    /* 1636 */ {I_PINSRB, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+37, IF_SSE41|IF_SB|IF_AR2},
    /* 1637 */ {I_PINSRB, 3, {XMMREG,RM_GPR|BITS8,IMMEDIATE,0,0}, nasm_bytecodes+36, IF_SSE41|IF_SB|IF_AR2},
    /* 1638 */ {I_PINSRB, 3, {XMMREG,REG_GPR|BITS32,IMMEDIATE,0,0}, nasm_bytecodes+37, IF_SSE41|IF_SB|IF_AR2},
    /* 1639 */ {I_PINSRD, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+46, IF_SSE41|IF_SB|IF_AR2},
    /* 1640 */ {I_PINSRD, 3, {XMMREG,RM_GPR|BITS32,IMMEDIATE,0,0}, nasm_bytecodes+46, IF_SSE41|IF_SB|IF_AR2},
    /* 1641 */ {I_PINSRQ, 3, {XMMREG,MEMORY,IMMEDIATE,0,0}, nasm_bytecodes+45, IF_SSE41|IF_X64|IF_SB|IF_AR2},
    /* 1642 */ {I_PINSRQ, 3, {XMMREG,RM_GPR|BITS64,IMMEDIATE,0,0}, nasm_bytecodes+45, IF_SSE41|IF_X64|IF_SB|IF_AR2},
    /* 1643 */ {I_PMAXSB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8166, IF_SSE41},
    /* 1644 */ {I_PMAXSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8173, IF_SSE41},
    /* 1645 */ {I_PMAXUD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8180, IF_SSE41},
    /* 1646 */ {I_PMAXUW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8187, IF_SSE41},
    /* 1647 */ {I_PMINSB, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8194, IF_SSE41},
    /* 1648 */ {I_PMINSD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8201, IF_SSE41},
    /* 1649 */ {I_PMINUD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8208, IF_SSE41},
    /* 1650 */ {I_PMINUW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8215, IF_SSE41},
    /* 1651 */ {I_PMOVSXBW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8222, IF_SSE41|IF_SQ},
    /* 1652 */ {I_PMOVSXBD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8229, IF_SSE41|IF_SD},
    /* 1653 */ {I_PMOVSXBQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8236, IF_SSE41|IF_SW},
    /* 1654 */ {I_PMOVSXWD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8243, IF_SSE41|IF_SQ},
    /* 1655 */ {I_PMOVSXWQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8250, IF_SSE41|IF_SD},
    /* 1656 */ {I_PMOVSXDQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8257, IF_SSE41|IF_SQ},
    /* 1657 */ {I_PMOVZXBW, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8264, IF_SSE41|IF_SQ},
    /* 1658 */ {I_PMOVZXBD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8271, IF_SSE41|IF_SD},
    /* 1659 */ {I_PMOVZXBQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8278, IF_SSE41|IF_SW},
    /* 1660 */ {I_PMOVZXWD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8285, IF_SSE41|IF_SQ},
    /* 1661 */ {I_PMOVZXWQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8292, IF_SSE41|IF_SD},
    /* 1662 */ {I_PMOVZXDQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8299, IF_SSE41|IF_SQ},
    /* 1663 */ {I_PMULDQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8306, IF_SSE41},
    /* 1664 */ {I_PMULLD, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8313, IF_SSE41},
    /* 1665 */ {I_PTEST, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8320, IF_SSE41},
    /* 1666 */ {I_ROUNDPD, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5744, IF_SSE41},
    /* 1667 */ {I_ROUNDPS, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5752, IF_SSE41},
    /* 1668 */ {I_ROUNDSD, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5760, IF_SSE41},
    /* 1669 */ {I_ROUNDSS, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5768, IF_SSE41},
    /* 1670 */ {I_CRC32, 2, {REG_GPR|BITS32,RM_GPR|BITS8,0,0,0}, nasm_bytecodes+5793, IF_SSE42},
    /* 1671 */ {I_CRC32, 2, {REG_GPR|BITS32,RM_GPR|BITS16,0,0,0}, nasm_bytecodes+5776, IF_SSE42},
    /* 1672 */ {I_CRC32, 2, {REG_GPR|BITS32,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+5784, IF_SSE42},
    /* 1673 */ {I_CRC32, 2, {REG_GPR|BITS64,RM_GPR|BITS8,0,0,0}, nasm_bytecodes+5792, IF_SSE42|IF_X64},
    /* 1674 */ {I_CRC32, 2, {REG_GPR|BITS64,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+5800, IF_SSE42|IF_X64},
    /* 1675 */ {I_PCMPESTRI, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5808, IF_SSE42},
    /* 1676 */ {I_PCMPESTRM, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5816, IF_SSE42},
    /* 1677 */ {I_PCMPISTRI, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5824, IF_SSE42},
    /* 1678 */ {I_PCMPISTRM, 3, {XMMREG,RM_XMM,IMMEDIATE,0,0}, nasm_bytecodes+5832, IF_SSE42},
    /* 1679 */ {I_PCMPGTQ, 2, {XMMREG,RM_XMM,0,0,0}, nasm_bytecodes+8327, IF_SSE42},
    /* 1680 */ {I_POPCNT, 2, {REG_GPR|BITS16,RM_GPR|BITS16,0,0,0}, nasm_bytecodes+8334, IF_NEHALEM|IF_SW},
    /* 1681 */ {I_POPCNT, 2, {REG_GPR|BITS32,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+8341, IF_NEHALEM|IF_SD},
    /* 1682 */ {I_POPCNT, 2, {REG_GPR|BITS64,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+8348, IF_NEHALEM|IF_SQ|IF_X64},
    /* 1683 */ {I_GETSEC, 0, {0,0,0,0,0}, nasm_bytecodes+19676, IF_KATMAI},
    /* 1684 */ {I_PFRCPV, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5840, IF_PENT|IF_3DNOW|IF_SQ|IF_CYRIX},
    /* 1685 */ {I_PFRSQRTV, 2, {MMXREG,RM_MMX,0,0,0}, nasm_bytecodes+5848, IF_PENT|IF_3DNOW|IF_SQ|IF_CYRIX},
    /* 1686 */ {I_MOVBE, 2, {REG_GPR|BITS16,MEMORY|BITS16,0,0,0}, nasm_bytecodes+8355, IF_NEHALEM|IF_SM},
    /* 1687 */ {I_MOVBE, 2, {REG_GPR|BITS32,MEMORY|BITS32,0,0,0}, nasm_bytecodes+8362, IF_NEHALEM|IF_SM},
    /* 1688 */ {I_MOVBE, 2, {REG_GPR|BITS64,MEMORY|BITS64,0,0,0}, nasm_bytecodes+8369, IF_NEHALEM|IF_SM},
    /* 1689 */ {I_MOVBE, 2, {MEMORY|BITS16,REG_GPR|BITS16,0,0,0}, nasm_bytecodes+8376, IF_NEHALEM|IF_SM},
    /* 1690 */ {I_MOVBE, 2, {MEMORY|BITS32,REG_GPR|BITS32,0,0,0}, nasm_bytecodes+8383, IF_NEHALEM|IF_SM},
    /* 1691 */ {I_MOVBE, 2, {MEMORY|BITS64,REG_GPR|BITS64,0,0,0}, nasm_bytecodes+8390, IF_NEHALEM|IF_SM},
    /* 1692 */ {I_AESENC, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8397, IF_SSE|IF_WESTMERE},
    /* 1693 */ {I_AESENCLAST, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8404, IF_SSE|IF_WESTMERE},
    /* 1694 */ {I_AESDEC, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8411, IF_SSE|IF_WESTMERE},
    /* 1695 */ {I_AESDECLAST, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8418, IF_SSE|IF_WESTMERE},
    /* 1696 */ {I_AESIMC, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8425, IF_SSE|IF_WESTMERE},
    /* 1697 */ {I_AESKEYGENASSIST, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+5856, IF_SSE|IF_WESTMERE},
    /* 1698 */ {I_VAESENC, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8432, IF_AVX|IF_SANDYBRIDGE},
    /* 1699 */ {I_VAESENC, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8439, IF_AVX|IF_SANDYBRIDGE},
    /* 1700 */ {I_VAESENCLAST, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8446, IF_AVX|IF_SANDYBRIDGE},
    /* 1701 */ {I_VAESENCLAST, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8453, IF_AVX|IF_SANDYBRIDGE},
    /* 1702 */ {I_VAESDEC, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8460, IF_AVX|IF_SANDYBRIDGE},
    /* 1703 */ {I_VAESDEC, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8467, IF_AVX|IF_SANDYBRIDGE},
    /* 1704 */ {I_VAESDECLAST, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8474, IF_AVX|IF_SANDYBRIDGE},
    /* 1705 */ {I_VAESDECLAST, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8481, IF_AVX|IF_SANDYBRIDGE},
    /* 1706 */ {I_VAESIMC, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8488, IF_AVX|IF_SANDYBRIDGE},
    /* 1707 */ {I_VAESKEYGENASSIST, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+5864, IF_AVX|IF_SANDYBRIDGE},
    /* 1708 */ {I_VADDPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8495, IF_AVX|IF_SANDYBRIDGE},
    /* 1709 */ {I_VADDPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8502, IF_AVX|IF_SANDYBRIDGE},
    /* 1710 */ {I_VADDPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+8509, IF_AVX|IF_SANDYBRIDGE},
    /* 1711 */ {I_VADDPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+8516, IF_AVX|IF_SANDYBRIDGE},
    /* 1712 */ {I_VADDPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8523, IF_AVX|IF_SANDYBRIDGE},
    /* 1713 */ {I_VADDPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8530, IF_AVX|IF_SANDYBRIDGE},
    /* 1714 */ {I_VADDPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+8537, IF_AVX|IF_SANDYBRIDGE},
    /* 1715 */ {I_VADDPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+8544, IF_AVX|IF_SANDYBRIDGE},
    /* 1716 */ {I_VADDSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+8551, IF_AVX|IF_SANDYBRIDGE},
    /* 1717 */ {I_VADDSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+8558, IF_AVX|IF_SANDYBRIDGE},
    /* 1718 */ {I_VADDSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+8565, IF_AVX|IF_SANDYBRIDGE},
    /* 1719 */ {I_VADDSS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+8572, IF_AVX|IF_SANDYBRIDGE},
    /* 1720 */ {I_VADDSUBPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8579, IF_AVX|IF_SANDYBRIDGE},
    /* 1721 */ {I_VADDSUBPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8586, IF_AVX|IF_SANDYBRIDGE},
    /* 1722 */ {I_VADDSUBPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+8593, IF_AVX|IF_SANDYBRIDGE},
    /* 1723 */ {I_VADDSUBPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+8600, IF_AVX|IF_SANDYBRIDGE},
    /* 1724 */ {I_VADDSUBPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8607, IF_AVX|IF_SANDYBRIDGE},
    /* 1725 */ {I_VADDSUBPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8614, IF_AVX|IF_SANDYBRIDGE},
    /* 1726 */ {I_VADDSUBPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+8621, IF_AVX|IF_SANDYBRIDGE},
    /* 1727 */ {I_VADDSUBPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+8628, IF_AVX|IF_SANDYBRIDGE},
    /* 1728 */ {I_VANDPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8635, IF_AVX|IF_SANDYBRIDGE},
    /* 1729 */ {I_VANDPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8642, IF_AVX|IF_SANDYBRIDGE},
    /* 1730 */ {I_VANDPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+8649, IF_AVX|IF_SANDYBRIDGE},
    /* 1731 */ {I_VANDPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+8656, IF_AVX|IF_SANDYBRIDGE},
    /* 1732 */ {I_VANDPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8663, IF_AVX|IF_SANDYBRIDGE},
    /* 1733 */ {I_VANDPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8670, IF_AVX|IF_SANDYBRIDGE},
    /* 1734 */ {I_VANDPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+8677, IF_AVX|IF_SANDYBRIDGE},
    /* 1735 */ {I_VANDPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+8684, IF_AVX|IF_SANDYBRIDGE},
    /* 1736 */ {I_VANDNPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8691, IF_AVX|IF_SANDYBRIDGE},
    /* 1737 */ {I_VANDNPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8698, IF_AVX|IF_SANDYBRIDGE},
    /* 1738 */ {I_VANDNPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+8705, IF_AVX|IF_SANDYBRIDGE},
    /* 1739 */ {I_VANDNPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+8712, IF_AVX|IF_SANDYBRIDGE},
    /* 1740 */ {I_VANDNPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+8719, IF_AVX|IF_SANDYBRIDGE},
    /* 1741 */ {I_VANDNPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8726, IF_AVX|IF_SANDYBRIDGE},
    /* 1742 */ {I_VANDNPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+8733, IF_AVX|IF_SANDYBRIDGE},
    /* 1743 */ {I_VANDNPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+8740, IF_AVX|IF_SANDYBRIDGE},
    /* 1744 */ {I_VBLENDPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+5872, IF_AVX|IF_SANDYBRIDGE},
    /* 1745 */ {I_VBLENDPD, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+5880, IF_AVX|IF_SANDYBRIDGE},
    /* 1746 */ {I_VBLENDPD, 4, {YMMREG,YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0}, nasm_bytecodes+5888, IF_AVX|IF_SANDYBRIDGE},
    /* 1747 */ {I_VBLENDPD, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+5896, IF_AVX|IF_SANDYBRIDGE},
    /* 1748 */ {I_VBLENDPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+5904, IF_AVX|IF_SANDYBRIDGE},
    /* 1749 */ {I_VBLENDPS, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+5912, IF_AVX|IF_SANDYBRIDGE},
    /* 1750 */ {I_VBLENDPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0}, nasm_bytecodes+5920, IF_AVX|IF_SANDYBRIDGE},
    /* 1751 */ {I_VBLENDPS, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+5928, IF_AVX|IF_SANDYBRIDGE},
    /* 1752 */ {I_VBLENDVPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+54, IF_AVX|IF_SANDYBRIDGE},
    /* 1753 */ {I_VBLENDVPD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+63, IF_AVX|IF_SANDYBRIDGE},
    /* 1754 */ {I_VBLENDVPD, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+72, IF_AVX|IF_SANDYBRIDGE},
    /* 1755 */ {I_VBLENDVPD, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+81, IF_AVX|IF_SANDYBRIDGE},
    /* 1756 */ {I_VBLENDVPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+90, IF_AVX|IF_SANDYBRIDGE},
    /* 1757 */ {I_VBLENDVPS, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+99, IF_AVX|IF_SANDYBRIDGE},
    /* 1758 */ {I_VBLENDVPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+108, IF_AVX|IF_SANDYBRIDGE},
    /* 1759 */ {I_VBLENDVPS, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+117, IF_AVX|IF_SANDYBRIDGE},
    /* 1760 */ {I_VBROADCASTSS, 2, {XMMREG,MEMORY|BITS32,0,0,0}, nasm_bytecodes+8747, IF_AVX|IF_SANDYBRIDGE},
    /* 1761 */ {I_VBROADCASTSS, 2, {YMMREG,MEMORY|BITS32,0,0,0}, nasm_bytecodes+8754, IF_AVX|IF_SANDYBRIDGE},
    /* 1762 */ {I_VBROADCASTSD, 2, {YMMREG,MEMORY|BITS64,0,0,0}, nasm_bytecodes+8761, IF_AVX|IF_SANDYBRIDGE},
    /* 1763 */ {I_VBROADCASTF128, 2, {YMMREG,MEMORY|BITS128,0,0,0}, nasm_bytecodes+8768, IF_AVX|IF_SANDYBRIDGE},
    /* 1764 */ {I_VCMPEQ_OSPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+126, IF_AVX|IF_SANDYBRIDGE},
    /* 1765 */ {I_VCMPEQ_OSPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+135, IF_AVX|IF_SANDYBRIDGE},
    /* 1766 */ {I_VCMPEQ_OSPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+144, IF_AVX|IF_SANDYBRIDGE},
    /* 1767 */ {I_VCMPEQ_OSPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+153, IF_AVX|IF_SANDYBRIDGE},
    /* 1768 */ {I_VCMPEQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+126, IF_AVX|IF_SANDYBRIDGE},
    /* 1769 */ {I_VCMPEQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+135, IF_AVX|IF_SANDYBRIDGE},
    /* 1770 */ {I_VCMPEQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+144, IF_AVX|IF_SANDYBRIDGE},
    /* 1771 */ {I_VCMPEQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+153, IF_AVX|IF_SANDYBRIDGE},
    /* 1772 */ {I_VCMPLT_OSPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+162, IF_AVX|IF_SANDYBRIDGE},
    /* 1773 */ {I_VCMPLT_OSPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+171, IF_AVX|IF_SANDYBRIDGE},
    /* 1774 */ {I_VCMPLT_OSPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+180, IF_AVX|IF_SANDYBRIDGE},
    /* 1775 */ {I_VCMPLT_OSPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+189, IF_AVX|IF_SANDYBRIDGE},
    /* 1776 */ {I_VCMPLTPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+162, IF_AVX|IF_SANDYBRIDGE},
    /* 1777 */ {I_VCMPLTPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+171, IF_AVX|IF_SANDYBRIDGE},
    /* 1778 */ {I_VCMPLTPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+180, IF_AVX|IF_SANDYBRIDGE},
    /* 1779 */ {I_VCMPLTPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+189, IF_AVX|IF_SANDYBRIDGE},
    /* 1780 */ {I_VCMPLE_OSPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+198, IF_AVX|IF_SANDYBRIDGE},
    /* 1781 */ {I_VCMPLE_OSPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+207, IF_AVX|IF_SANDYBRIDGE},
    /* 1782 */ {I_VCMPLE_OSPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+216, IF_AVX|IF_SANDYBRIDGE},
    /* 1783 */ {I_VCMPLE_OSPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+225, IF_AVX|IF_SANDYBRIDGE},
    /* 1784 */ {I_VCMPLEPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+198, IF_AVX|IF_SANDYBRIDGE},
    /* 1785 */ {I_VCMPLEPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+207, IF_AVX|IF_SANDYBRIDGE},
    /* 1786 */ {I_VCMPLEPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+216, IF_AVX|IF_SANDYBRIDGE},
    /* 1787 */ {I_VCMPLEPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+225, IF_AVX|IF_SANDYBRIDGE},
    /* 1788 */ {I_VCMPUNORD_QPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+234, IF_AVX|IF_SANDYBRIDGE},
    /* 1789 */ {I_VCMPUNORD_QPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+243, IF_AVX|IF_SANDYBRIDGE},
    /* 1790 */ {I_VCMPUNORD_QPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+252, IF_AVX|IF_SANDYBRIDGE},
    /* 1791 */ {I_VCMPUNORD_QPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+261, IF_AVX|IF_SANDYBRIDGE},
    /* 1792 */ {I_VCMPUNORDPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+234, IF_AVX|IF_SANDYBRIDGE},
    /* 1793 */ {I_VCMPUNORDPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+243, IF_AVX|IF_SANDYBRIDGE},
    /* 1794 */ {I_VCMPUNORDPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+252, IF_AVX|IF_SANDYBRIDGE},
    /* 1795 */ {I_VCMPUNORDPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+261, IF_AVX|IF_SANDYBRIDGE},
    /* 1796 */ {I_VCMPNEQ_UQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+270, IF_AVX|IF_SANDYBRIDGE},
    /* 1797 */ {I_VCMPNEQ_UQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+279, IF_AVX|IF_SANDYBRIDGE},
    /* 1798 */ {I_VCMPNEQ_UQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+288, IF_AVX|IF_SANDYBRIDGE},
    /* 1799 */ {I_VCMPNEQ_UQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+297, IF_AVX|IF_SANDYBRIDGE},
    /* 1800 */ {I_VCMPNEQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+270, IF_AVX|IF_SANDYBRIDGE},
    /* 1801 */ {I_VCMPNEQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+279, IF_AVX|IF_SANDYBRIDGE},
    /* 1802 */ {I_VCMPNEQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+288, IF_AVX|IF_SANDYBRIDGE},
    /* 1803 */ {I_VCMPNEQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+297, IF_AVX|IF_SANDYBRIDGE},
    /* 1804 */ {I_VCMPNLT_USPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+306, IF_AVX|IF_SANDYBRIDGE},
    /* 1805 */ {I_VCMPNLT_USPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+315, IF_AVX|IF_SANDYBRIDGE},
    /* 1806 */ {I_VCMPNLT_USPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+324, IF_AVX|IF_SANDYBRIDGE},
    /* 1807 */ {I_VCMPNLT_USPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+333, IF_AVX|IF_SANDYBRIDGE},
    /* 1808 */ {I_VCMPNLTPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+306, IF_AVX|IF_SANDYBRIDGE},
    /* 1809 */ {I_VCMPNLTPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+315, IF_AVX|IF_SANDYBRIDGE},
    /* 1810 */ {I_VCMPNLTPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+324, IF_AVX|IF_SANDYBRIDGE},
    /* 1811 */ {I_VCMPNLTPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+333, IF_AVX|IF_SANDYBRIDGE},
    /* 1812 */ {I_VCMPNLE_USPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+342, IF_AVX|IF_SANDYBRIDGE},
    /* 1813 */ {I_VCMPNLE_USPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+351, IF_AVX|IF_SANDYBRIDGE},
    /* 1814 */ {I_VCMPNLE_USPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+360, IF_AVX|IF_SANDYBRIDGE},
    /* 1815 */ {I_VCMPNLE_USPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+369, IF_AVX|IF_SANDYBRIDGE},
    /* 1816 */ {I_VCMPNLEPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+342, IF_AVX|IF_SANDYBRIDGE},
    /* 1817 */ {I_VCMPNLEPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+351, IF_AVX|IF_SANDYBRIDGE},
    /* 1818 */ {I_VCMPNLEPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+360, IF_AVX|IF_SANDYBRIDGE},
    /* 1819 */ {I_VCMPNLEPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+369, IF_AVX|IF_SANDYBRIDGE},
    /* 1820 */ {I_VCMPORD_QPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+378, IF_AVX|IF_SANDYBRIDGE},
    /* 1821 */ {I_VCMPORD_QPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+387, IF_AVX|IF_SANDYBRIDGE},
    /* 1822 */ {I_VCMPORD_QPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+396, IF_AVX|IF_SANDYBRIDGE},
    /* 1823 */ {I_VCMPORD_QPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+405, IF_AVX|IF_SANDYBRIDGE},
    /* 1824 */ {I_VCMPORDPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+378, IF_AVX|IF_SANDYBRIDGE},
    /* 1825 */ {I_VCMPORDPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+387, IF_AVX|IF_SANDYBRIDGE},
    /* 1826 */ {I_VCMPORDPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+396, IF_AVX|IF_SANDYBRIDGE},
    /* 1827 */ {I_VCMPORDPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+405, IF_AVX|IF_SANDYBRIDGE},
    /* 1828 */ {I_VCMPEQ_UQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+414, IF_AVX|IF_SANDYBRIDGE},
    /* 1829 */ {I_VCMPEQ_UQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+423, IF_AVX|IF_SANDYBRIDGE},
    /* 1830 */ {I_VCMPEQ_UQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+432, IF_AVX|IF_SANDYBRIDGE},
    /* 1831 */ {I_VCMPEQ_UQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+441, IF_AVX|IF_SANDYBRIDGE},
    /* 1832 */ {I_VCMPNGE_USPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+450, IF_AVX|IF_SANDYBRIDGE},
    /* 1833 */ {I_VCMPNGE_USPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+459, IF_AVX|IF_SANDYBRIDGE},
    /* 1834 */ {I_VCMPNGE_USPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+468, IF_AVX|IF_SANDYBRIDGE},
    /* 1835 */ {I_VCMPNGE_USPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+477, IF_AVX|IF_SANDYBRIDGE},
    /* 1836 */ {I_VCMPNGEPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+450, IF_AVX|IF_SANDYBRIDGE},
    /* 1837 */ {I_VCMPNGEPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+459, IF_AVX|IF_SANDYBRIDGE},
    /* 1838 */ {I_VCMPNGEPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+468, IF_AVX|IF_SANDYBRIDGE},
    /* 1839 */ {I_VCMPNGEPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+477, IF_AVX|IF_SANDYBRIDGE},
    /* 1840 */ {I_VCMPNGT_USPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+486, IF_AVX|IF_SANDYBRIDGE},
    /* 1841 */ {I_VCMPNGT_USPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+495, IF_AVX|IF_SANDYBRIDGE},
    /* 1842 */ {I_VCMPNGT_USPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+504, IF_AVX|IF_SANDYBRIDGE},
    /* 1843 */ {I_VCMPNGT_USPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+513, IF_AVX|IF_SANDYBRIDGE},
    /* 1844 */ {I_VCMPNGTPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+486, IF_AVX|IF_SANDYBRIDGE},
    /* 1845 */ {I_VCMPNGTPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+495, IF_AVX|IF_SANDYBRIDGE},
    /* 1846 */ {I_VCMPNGTPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+504, IF_AVX|IF_SANDYBRIDGE},
    /* 1847 */ {I_VCMPNGTPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+513, IF_AVX|IF_SANDYBRIDGE},
    /* 1848 */ {I_VCMPFALSE_OQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+522, IF_AVX|IF_SANDYBRIDGE},
    /* 1849 */ {I_VCMPFALSE_OQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+531, IF_AVX|IF_SANDYBRIDGE},
    /* 1850 */ {I_VCMPFALSE_OQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+540, IF_AVX|IF_SANDYBRIDGE},
    /* 1851 */ {I_VCMPFALSE_OQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+549, IF_AVX|IF_SANDYBRIDGE},
    /* 1852 */ {I_VCMPFALSEPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+522, IF_AVX|IF_SANDYBRIDGE},
    /* 1853 */ {I_VCMPFALSEPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+531, IF_AVX|IF_SANDYBRIDGE},
    /* 1854 */ {I_VCMPFALSEPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+540, IF_AVX|IF_SANDYBRIDGE},
    /* 1855 */ {I_VCMPFALSEPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+549, IF_AVX|IF_SANDYBRIDGE},
    /* 1856 */ {I_VCMPNEQ_OQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+558, IF_AVX|IF_SANDYBRIDGE},
    /* 1857 */ {I_VCMPNEQ_OQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+567, IF_AVX|IF_SANDYBRIDGE},
    /* 1858 */ {I_VCMPNEQ_OQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+576, IF_AVX|IF_SANDYBRIDGE},
    /* 1859 */ {I_VCMPNEQ_OQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+585, IF_AVX|IF_SANDYBRIDGE},
    /* 1860 */ {I_VCMPGE_OSPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+594, IF_AVX|IF_SANDYBRIDGE},
    /* 1861 */ {I_VCMPGE_OSPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+603, IF_AVX|IF_SANDYBRIDGE},
    /* 1862 */ {I_VCMPGE_OSPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+612, IF_AVX|IF_SANDYBRIDGE},
    /* 1863 */ {I_VCMPGE_OSPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+621, IF_AVX|IF_SANDYBRIDGE},
    /* 1864 */ {I_VCMPGEPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+594, IF_AVX|IF_SANDYBRIDGE},
    /* 1865 */ {I_VCMPGEPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+603, IF_AVX|IF_SANDYBRIDGE},
    /* 1866 */ {I_VCMPGEPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+612, IF_AVX|IF_SANDYBRIDGE},
    /* 1867 */ {I_VCMPGEPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+621, IF_AVX|IF_SANDYBRIDGE},
    /* 1868 */ {I_VCMPGT_OSPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+630, IF_AVX|IF_SANDYBRIDGE},
    /* 1869 */ {I_VCMPGT_OSPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+639, IF_AVX|IF_SANDYBRIDGE},
    /* 1870 */ {I_VCMPGT_OSPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+648, IF_AVX|IF_SANDYBRIDGE},
    /* 1871 */ {I_VCMPGT_OSPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+657, IF_AVX|IF_SANDYBRIDGE},
    /* 1872 */ {I_VCMPGTPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+630, IF_AVX|IF_SANDYBRIDGE},
    /* 1873 */ {I_VCMPGTPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+639, IF_AVX|IF_SANDYBRIDGE},
    /* 1874 */ {I_VCMPGTPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+648, IF_AVX|IF_SANDYBRIDGE},
    /* 1875 */ {I_VCMPGTPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+657, IF_AVX|IF_SANDYBRIDGE},
    /* 1876 */ {I_VCMPTRUE_UQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+666, IF_AVX|IF_SANDYBRIDGE},
    /* 1877 */ {I_VCMPTRUE_UQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+675, IF_AVX|IF_SANDYBRIDGE},
    /* 1878 */ {I_VCMPTRUE_UQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+684, IF_AVX|IF_SANDYBRIDGE},
    /* 1879 */ {I_VCMPTRUE_UQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+693, IF_AVX|IF_SANDYBRIDGE},
    /* 1880 */ {I_VCMPTRUEPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+666, IF_AVX|IF_SANDYBRIDGE},
    /* 1881 */ {I_VCMPTRUEPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+675, IF_AVX|IF_SANDYBRIDGE},
    /* 1882 */ {I_VCMPTRUEPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+684, IF_AVX|IF_SANDYBRIDGE},
    /* 1883 */ {I_VCMPTRUEPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+693, IF_AVX|IF_SANDYBRIDGE},
    /* 1884 */ {I_VCMPEQ_OSPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+702, IF_AVX|IF_SANDYBRIDGE},
    /* 1885 */ {I_VCMPEQ_OSPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+711, IF_AVX|IF_SANDYBRIDGE},
    /* 1886 */ {I_VCMPEQ_OSPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+720, IF_AVX|IF_SANDYBRIDGE},
    /* 1887 */ {I_VCMPEQ_OSPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+729, IF_AVX|IF_SANDYBRIDGE},
    /* 1888 */ {I_VCMPLT_OQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+738, IF_AVX|IF_SANDYBRIDGE},
    /* 1889 */ {I_VCMPLT_OQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+747, IF_AVX|IF_SANDYBRIDGE},
    /* 1890 */ {I_VCMPLT_OQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+756, IF_AVX|IF_SANDYBRIDGE},
    /* 1891 */ {I_VCMPLT_OQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+765, IF_AVX|IF_SANDYBRIDGE},
    /* 1892 */ {I_VCMPLE_OQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+774, IF_AVX|IF_SANDYBRIDGE},
    /* 1893 */ {I_VCMPLE_OQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+783, IF_AVX|IF_SANDYBRIDGE},
    /* 1894 */ {I_VCMPLE_OQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+792, IF_AVX|IF_SANDYBRIDGE},
    /* 1895 */ {I_VCMPLE_OQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+801, IF_AVX|IF_SANDYBRIDGE},
    /* 1896 */ {I_VCMPUNORD_SPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+810, IF_AVX|IF_SANDYBRIDGE},
    /* 1897 */ {I_VCMPUNORD_SPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+819, IF_AVX|IF_SANDYBRIDGE},
    /* 1898 */ {I_VCMPUNORD_SPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+828, IF_AVX|IF_SANDYBRIDGE},
    /* 1899 */ {I_VCMPUNORD_SPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+837, IF_AVX|IF_SANDYBRIDGE},
    /* 1900 */ {I_VCMPNEQ_USPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+846, IF_AVX|IF_SANDYBRIDGE},
    /* 1901 */ {I_VCMPNEQ_USPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+855, IF_AVX|IF_SANDYBRIDGE},
    /* 1902 */ {I_VCMPNEQ_USPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+864, IF_AVX|IF_SANDYBRIDGE},
    /* 1903 */ {I_VCMPNEQ_USPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+873, IF_AVX|IF_SANDYBRIDGE},
    /* 1904 */ {I_VCMPNLT_UQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+882, IF_AVX|IF_SANDYBRIDGE},
    /* 1905 */ {I_VCMPNLT_UQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+891, IF_AVX|IF_SANDYBRIDGE},
    /* 1906 */ {I_VCMPNLT_UQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+900, IF_AVX|IF_SANDYBRIDGE},
    /* 1907 */ {I_VCMPNLT_UQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+909, IF_AVX|IF_SANDYBRIDGE},
    /* 1908 */ {I_VCMPNLE_UQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+918, IF_AVX|IF_SANDYBRIDGE},
    /* 1909 */ {I_VCMPNLE_UQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+927, IF_AVX|IF_SANDYBRIDGE},
    /* 1910 */ {I_VCMPNLE_UQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+936, IF_AVX|IF_SANDYBRIDGE},
    /* 1911 */ {I_VCMPNLE_UQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+945, IF_AVX|IF_SANDYBRIDGE},
    /* 1912 */ {I_VCMPORD_SPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+954, IF_AVX|IF_SANDYBRIDGE},
    /* 1913 */ {I_VCMPORD_SPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+963, IF_AVX|IF_SANDYBRIDGE},
    /* 1914 */ {I_VCMPORD_SPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+972, IF_AVX|IF_SANDYBRIDGE},
    /* 1915 */ {I_VCMPORD_SPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+981, IF_AVX|IF_SANDYBRIDGE},
    /* 1916 */ {I_VCMPEQ_USPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+990, IF_AVX|IF_SANDYBRIDGE},
    /* 1917 */ {I_VCMPEQ_USPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+999, IF_AVX|IF_SANDYBRIDGE},
    /* 1918 */ {I_VCMPEQ_USPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1008, IF_AVX|IF_SANDYBRIDGE},
    /* 1919 */ {I_VCMPEQ_USPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1017, IF_AVX|IF_SANDYBRIDGE},
    /* 1920 */ {I_VCMPNGE_UQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1026, IF_AVX|IF_SANDYBRIDGE},
    /* 1921 */ {I_VCMPNGE_UQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1035, IF_AVX|IF_SANDYBRIDGE},
    /* 1922 */ {I_VCMPNGE_UQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1044, IF_AVX|IF_SANDYBRIDGE},
    /* 1923 */ {I_VCMPNGE_UQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1053, IF_AVX|IF_SANDYBRIDGE},
    /* 1924 */ {I_VCMPNGT_UQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1062, IF_AVX|IF_SANDYBRIDGE},
    /* 1925 */ {I_VCMPNGT_UQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1071, IF_AVX|IF_SANDYBRIDGE},
    /* 1926 */ {I_VCMPNGT_UQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1080, IF_AVX|IF_SANDYBRIDGE},
    /* 1927 */ {I_VCMPNGT_UQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1089, IF_AVX|IF_SANDYBRIDGE},
    /* 1928 */ {I_VCMPFALSE_OSPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1098, IF_AVX|IF_SANDYBRIDGE},
    /* 1929 */ {I_VCMPFALSE_OSPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1107, IF_AVX|IF_SANDYBRIDGE},
    /* 1930 */ {I_VCMPFALSE_OSPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1116, IF_AVX|IF_SANDYBRIDGE},
    /* 1931 */ {I_VCMPFALSE_OSPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1125, IF_AVX|IF_SANDYBRIDGE},
    /* 1932 */ {I_VCMPNEQ_OSPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1134, IF_AVX|IF_SANDYBRIDGE},
    /* 1933 */ {I_VCMPNEQ_OSPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1143, IF_AVX|IF_SANDYBRIDGE},
    /* 1934 */ {I_VCMPNEQ_OSPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1152, IF_AVX|IF_SANDYBRIDGE},
    /* 1935 */ {I_VCMPNEQ_OSPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1161, IF_AVX|IF_SANDYBRIDGE},
    /* 1936 */ {I_VCMPGE_OQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1170, IF_AVX|IF_SANDYBRIDGE},
    /* 1937 */ {I_VCMPGE_OQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1179, IF_AVX|IF_SANDYBRIDGE},
    /* 1938 */ {I_VCMPGE_OQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1188, IF_AVX|IF_SANDYBRIDGE},
    /* 1939 */ {I_VCMPGE_OQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1197, IF_AVX|IF_SANDYBRIDGE},
    /* 1940 */ {I_VCMPGT_OQPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1206, IF_AVX|IF_SANDYBRIDGE},
    /* 1941 */ {I_VCMPGT_OQPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1215, IF_AVX|IF_SANDYBRIDGE},
    /* 1942 */ {I_VCMPGT_OQPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1224, IF_AVX|IF_SANDYBRIDGE},
    /* 1943 */ {I_VCMPGT_OQPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1233, IF_AVX|IF_SANDYBRIDGE},
    /* 1944 */ {I_VCMPTRUE_USPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1242, IF_AVX|IF_SANDYBRIDGE},
    /* 1945 */ {I_VCMPTRUE_USPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1251, IF_AVX|IF_SANDYBRIDGE},
    /* 1946 */ {I_VCMPTRUE_USPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1260, IF_AVX|IF_SANDYBRIDGE},
    /* 1947 */ {I_VCMPTRUE_USPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1269, IF_AVX|IF_SANDYBRIDGE},
    /* 1948 */ {I_VCMPPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+5936, IF_AVX|IF_SANDYBRIDGE},
    /* 1949 */ {I_VCMPPD, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+5944, IF_AVX|IF_SANDYBRIDGE},
    /* 1950 */ {I_VCMPPD, 4, {YMMREG,YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0}, nasm_bytecodes+5952, IF_AVX|IF_SANDYBRIDGE},
    /* 1951 */ {I_VCMPPD, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+5960, IF_AVX|IF_SANDYBRIDGE},
    /* 1952 */ {I_VCMPEQ_OSPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1278, IF_AVX|IF_SANDYBRIDGE},
    /* 1953 */ {I_VCMPEQ_OSPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1287, IF_AVX|IF_SANDYBRIDGE},
    /* 1954 */ {I_VCMPEQ_OSPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1296, IF_AVX|IF_SANDYBRIDGE},
    /* 1955 */ {I_VCMPEQ_OSPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1305, IF_AVX|IF_SANDYBRIDGE},
    /* 1956 */ {I_VCMPEQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1278, IF_AVX|IF_SANDYBRIDGE},
    /* 1957 */ {I_VCMPEQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1287, IF_AVX|IF_SANDYBRIDGE},
    /* 1958 */ {I_VCMPEQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1296, IF_AVX|IF_SANDYBRIDGE},
    /* 1959 */ {I_VCMPEQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1305, IF_AVX|IF_SANDYBRIDGE},
    /* 1960 */ {I_VCMPLT_OSPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1314, IF_AVX|IF_SANDYBRIDGE},
    /* 1961 */ {I_VCMPLT_OSPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1323, IF_AVX|IF_SANDYBRIDGE},
    /* 1962 */ {I_VCMPLT_OSPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1332, IF_AVX|IF_SANDYBRIDGE},
    /* 1963 */ {I_VCMPLT_OSPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1341, IF_AVX|IF_SANDYBRIDGE},
    /* 1964 */ {I_VCMPLTPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1314, IF_AVX|IF_SANDYBRIDGE},
    /* 1965 */ {I_VCMPLTPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1323, IF_AVX|IF_SANDYBRIDGE},
    /* 1966 */ {I_VCMPLTPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1332, IF_AVX|IF_SANDYBRIDGE},
    /* 1967 */ {I_VCMPLTPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1341, IF_AVX|IF_SANDYBRIDGE},
    /* 1968 */ {I_VCMPLE_OSPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1350, IF_AVX|IF_SANDYBRIDGE},
    /* 1969 */ {I_VCMPLE_OSPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1359, IF_AVX|IF_SANDYBRIDGE},
    /* 1970 */ {I_VCMPLE_OSPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1368, IF_AVX|IF_SANDYBRIDGE},
    /* 1971 */ {I_VCMPLE_OSPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1377, IF_AVX|IF_SANDYBRIDGE},
    /* 1972 */ {I_VCMPLEPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1350, IF_AVX|IF_SANDYBRIDGE},
    /* 1973 */ {I_VCMPLEPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1359, IF_AVX|IF_SANDYBRIDGE},
    /* 1974 */ {I_VCMPLEPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1368, IF_AVX|IF_SANDYBRIDGE},
    /* 1975 */ {I_VCMPLEPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1377, IF_AVX|IF_SANDYBRIDGE},
    /* 1976 */ {I_VCMPUNORD_QPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1386, IF_AVX|IF_SANDYBRIDGE},
    /* 1977 */ {I_VCMPUNORD_QPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1395, IF_AVX|IF_SANDYBRIDGE},
    /* 1978 */ {I_VCMPUNORD_QPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1404, IF_AVX|IF_SANDYBRIDGE},
    /* 1979 */ {I_VCMPUNORD_QPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1413, IF_AVX|IF_SANDYBRIDGE},
    /* 1980 */ {I_VCMPUNORDPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1386, IF_AVX|IF_SANDYBRIDGE},
    /* 1981 */ {I_VCMPUNORDPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1395, IF_AVX|IF_SANDYBRIDGE},
    /* 1982 */ {I_VCMPUNORDPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1404, IF_AVX|IF_SANDYBRIDGE},
    /* 1983 */ {I_VCMPUNORDPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1413, IF_AVX|IF_SANDYBRIDGE},
    /* 1984 */ {I_VCMPNEQ_UQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1422, IF_AVX|IF_SANDYBRIDGE},
    /* 1985 */ {I_VCMPNEQ_UQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1431, IF_AVX|IF_SANDYBRIDGE},
    /* 1986 */ {I_VCMPNEQ_UQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1440, IF_AVX|IF_SANDYBRIDGE},
    /* 1987 */ {I_VCMPNEQ_UQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1449, IF_AVX|IF_SANDYBRIDGE},
    /* 1988 */ {I_VCMPNEQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1422, IF_AVX|IF_SANDYBRIDGE},
    /* 1989 */ {I_VCMPNEQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1431, IF_AVX|IF_SANDYBRIDGE},
    /* 1990 */ {I_VCMPNEQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1440, IF_AVX|IF_SANDYBRIDGE},
    /* 1991 */ {I_VCMPNEQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1449, IF_AVX|IF_SANDYBRIDGE},
    /* 1992 */ {I_VCMPNLT_USPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1458, IF_AVX|IF_SANDYBRIDGE},
    /* 1993 */ {I_VCMPNLT_USPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1467, IF_AVX|IF_SANDYBRIDGE},
    /* 1994 */ {I_VCMPNLT_USPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1476, IF_AVX|IF_SANDYBRIDGE},
    /* 1995 */ {I_VCMPNLT_USPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1485, IF_AVX|IF_SANDYBRIDGE},
    /* 1996 */ {I_VCMPNLTPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1458, IF_AVX|IF_SANDYBRIDGE},
    /* 1997 */ {I_VCMPNLTPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1467, IF_AVX|IF_SANDYBRIDGE},
    /* 1998 */ {I_VCMPNLTPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1476, IF_AVX|IF_SANDYBRIDGE},
    /* 1999 */ {I_VCMPNLTPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1485, IF_AVX|IF_SANDYBRIDGE},
    /* 2000 */ {I_VCMPNLE_USPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1494, IF_AVX|IF_SANDYBRIDGE},
    /* 2001 */ {I_VCMPNLE_USPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1503, IF_AVX|IF_SANDYBRIDGE},
    /* 2002 */ {I_VCMPNLE_USPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1512, IF_AVX|IF_SANDYBRIDGE},
    /* 2003 */ {I_VCMPNLE_USPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1521, IF_AVX|IF_SANDYBRIDGE},
    /* 2004 */ {I_VCMPNLEPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1494, IF_AVX|IF_SANDYBRIDGE},
    /* 2005 */ {I_VCMPNLEPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1503, IF_AVX|IF_SANDYBRIDGE},
    /* 2006 */ {I_VCMPNLEPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1512, IF_AVX|IF_SANDYBRIDGE},
    /* 2007 */ {I_VCMPNLEPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1521, IF_AVX|IF_SANDYBRIDGE},
    /* 2008 */ {I_VCMPORD_QPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1530, IF_AVX|IF_SANDYBRIDGE},
    /* 2009 */ {I_VCMPORD_QPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1539, IF_AVX|IF_SANDYBRIDGE},
    /* 2010 */ {I_VCMPORD_QPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1548, IF_AVX|IF_SANDYBRIDGE},
    /* 2011 */ {I_VCMPORD_QPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1557, IF_AVX|IF_SANDYBRIDGE},
    /* 2012 */ {I_VCMPORDPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1530, IF_AVX|IF_SANDYBRIDGE},
    /* 2013 */ {I_VCMPORDPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1539, IF_AVX|IF_SANDYBRIDGE},
    /* 2014 */ {I_VCMPORDPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1548, IF_AVX|IF_SANDYBRIDGE},
    /* 2015 */ {I_VCMPORDPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1557, IF_AVX|IF_SANDYBRIDGE},
    /* 2016 */ {I_VCMPEQ_UQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1566, IF_AVX|IF_SANDYBRIDGE},
    /* 2017 */ {I_VCMPEQ_UQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1575, IF_AVX|IF_SANDYBRIDGE},
    /* 2018 */ {I_VCMPEQ_UQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1584, IF_AVX|IF_SANDYBRIDGE},
    /* 2019 */ {I_VCMPEQ_UQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1593, IF_AVX|IF_SANDYBRIDGE},
    /* 2020 */ {I_VCMPNGE_USPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1602, IF_AVX|IF_SANDYBRIDGE},
    /* 2021 */ {I_VCMPNGE_USPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1611, IF_AVX|IF_SANDYBRIDGE},
    /* 2022 */ {I_VCMPNGE_USPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1620, IF_AVX|IF_SANDYBRIDGE},
    /* 2023 */ {I_VCMPNGE_USPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1629, IF_AVX|IF_SANDYBRIDGE},
    /* 2024 */ {I_VCMPNGEPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1602, IF_AVX|IF_SANDYBRIDGE},
    /* 2025 */ {I_VCMPNGEPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1611, IF_AVX|IF_SANDYBRIDGE},
    /* 2026 */ {I_VCMPNGEPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1620, IF_AVX|IF_SANDYBRIDGE},
    /* 2027 */ {I_VCMPNGEPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1629, IF_AVX|IF_SANDYBRIDGE},
    /* 2028 */ {I_VCMPNGT_USPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1638, IF_AVX|IF_SANDYBRIDGE},
    /* 2029 */ {I_VCMPNGT_USPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1647, IF_AVX|IF_SANDYBRIDGE},
    /* 2030 */ {I_VCMPNGT_USPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1656, IF_AVX|IF_SANDYBRIDGE},
    /* 2031 */ {I_VCMPNGT_USPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1665, IF_AVX|IF_SANDYBRIDGE},
    /* 2032 */ {I_VCMPNGTPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1638, IF_AVX|IF_SANDYBRIDGE},
    /* 2033 */ {I_VCMPNGTPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1647, IF_AVX|IF_SANDYBRIDGE},
    /* 2034 */ {I_VCMPNGTPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1656, IF_AVX|IF_SANDYBRIDGE},
    /* 2035 */ {I_VCMPNGTPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1665, IF_AVX|IF_SANDYBRIDGE},
    /* 2036 */ {I_VCMPFALSE_OQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1674, IF_AVX|IF_SANDYBRIDGE},
    /* 2037 */ {I_VCMPFALSE_OQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1683, IF_AVX|IF_SANDYBRIDGE},
    /* 2038 */ {I_VCMPFALSE_OQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1692, IF_AVX|IF_SANDYBRIDGE},
    /* 2039 */ {I_VCMPFALSE_OQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1701, IF_AVX|IF_SANDYBRIDGE},
    /* 2040 */ {I_VCMPFALSEPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1674, IF_AVX|IF_SANDYBRIDGE},
    /* 2041 */ {I_VCMPFALSEPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1683, IF_AVX|IF_SANDYBRIDGE},
    /* 2042 */ {I_VCMPFALSEPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1692, IF_AVX|IF_SANDYBRIDGE},
    /* 2043 */ {I_VCMPFALSEPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1701, IF_AVX|IF_SANDYBRIDGE},
    /* 2044 */ {I_VCMPNEQ_OQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1710, IF_AVX|IF_SANDYBRIDGE},
    /* 2045 */ {I_VCMPNEQ_OQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1719, IF_AVX|IF_SANDYBRIDGE},
    /* 2046 */ {I_VCMPNEQ_OQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1728, IF_AVX|IF_SANDYBRIDGE},
    /* 2047 */ {I_VCMPNEQ_OQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1737, IF_AVX|IF_SANDYBRIDGE},
    /* 2048 */ {I_VCMPGE_OSPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1746, IF_AVX|IF_SANDYBRIDGE},
    /* 2049 */ {I_VCMPGE_OSPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1755, IF_AVX|IF_SANDYBRIDGE},
    /* 2050 */ {I_VCMPGE_OSPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1764, IF_AVX|IF_SANDYBRIDGE},
    /* 2051 */ {I_VCMPGE_OSPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1773, IF_AVX|IF_SANDYBRIDGE},
    /* 2052 */ {I_VCMPGEPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1746, IF_AVX|IF_SANDYBRIDGE},
    /* 2053 */ {I_VCMPGEPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1755, IF_AVX|IF_SANDYBRIDGE},
    /* 2054 */ {I_VCMPGEPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1764, IF_AVX|IF_SANDYBRIDGE},
    /* 2055 */ {I_VCMPGEPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1773, IF_AVX|IF_SANDYBRIDGE},
    /* 2056 */ {I_VCMPGT_OSPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1782, IF_AVX|IF_SANDYBRIDGE},
    /* 2057 */ {I_VCMPGT_OSPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1791, IF_AVX|IF_SANDYBRIDGE},
    /* 2058 */ {I_VCMPGT_OSPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1800, IF_AVX|IF_SANDYBRIDGE},
    /* 2059 */ {I_VCMPGT_OSPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1809, IF_AVX|IF_SANDYBRIDGE},
    /* 2060 */ {I_VCMPGTPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1782, IF_AVX|IF_SANDYBRIDGE},
    /* 2061 */ {I_VCMPGTPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1791, IF_AVX|IF_SANDYBRIDGE},
    /* 2062 */ {I_VCMPGTPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1800, IF_AVX|IF_SANDYBRIDGE},
    /* 2063 */ {I_VCMPGTPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1809, IF_AVX|IF_SANDYBRIDGE},
    /* 2064 */ {I_VCMPTRUE_UQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1818, IF_AVX|IF_SANDYBRIDGE},
    /* 2065 */ {I_VCMPTRUE_UQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1827, IF_AVX|IF_SANDYBRIDGE},
    /* 2066 */ {I_VCMPTRUE_UQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1836, IF_AVX|IF_SANDYBRIDGE},
    /* 2067 */ {I_VCMPTRUE_UQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1845, IF_AVX|IF_SANDYBRIDGE},
    /* 2068 */ {I_VCMPTRUEPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1818, IF_AVX|IF_SANDYBRIDGE},
    /* 2069 */ {I_VCMPTRUEPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1827, IF_AVX|IF_SANDYBRIDGE},
    /* 2070 */ {I_VCMPTRUEPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1836, IF_AVX|IF_SANDYBRIDGE},
    /* 2071 */ {I_VCMPTRUEPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1845, IF_AVX|IF_SANDYBRIDGE},
    /* 2072 */ {I_VCMPEQ_OSPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1854, IF_AVX|IF_SANDYBRIDGE},
    /* 2073 */ {I_VCMPEQ_OSPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1863, IF_AVX|IF_SANDYBRIDGE},
    /* 2074 */ {I_VCMPEQ_OSPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1872, IF_AVX|IF_SANDYBRIDGE},
    /* 2075 */ {I_VCMPEQ_OSPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1881, IF_AVX|IF_SANDYBRIDGE},
    /* 2076 */ {I_VCMPLT_OQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1890, IF_AVX|IF_SANDYBRIDGE},
    /* 2077 */ {I_VCMPLT_OQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1899, IF_AVX|IF_SANDYBRIDGE},
    /* 2078 */ {I_VCMPLT_OQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1908, IF_AVX|IF_SANDYBRIDGE},
    /* 2079 */ {I_VCMPLT_OQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1917, IF_AVX|IF_SANDYBRIDGE},
    /* 2080 */ {I_VCMPLE_OQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1926, IF_AVX|IF_SANDYBRIDGE},
    /* 2081 */ {I_VCMPLE_OQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1935, IF_AVX|IF_SANDYBRIDGE},
    /* 2082 */ {I_VCMPLE_OQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1944, IF_AVX|IF_SANDYBRIDGE},
    /* 2083 */ {I_VCMPLE_OQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1953, IF_AVX|IF_SANDYBRIDGE},
    /* 2084 */ {I_VCMPUNORD_SPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1962, IF_AVX|IF_SANDYBRIDGE},
    /* 2085 */ {I_VCMPUNORD_SPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+1971, IF_AVX|IF_SANDYBRIDGE},
    /* 2086 */ {I_VCMPUNORD_SPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+1980, IF_AVX|IF_SANDYBRIDGE},
    /* 2087 */ {I_VCMPUNORD_SPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+1989, IF_AVX|IF_SANDYBRIDGE},
    /* 2088 */ {I_VCMPNEQ_USPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+1998, IF_AVX|IF_SANDYBRIDGE},
    /* 2089 */ {I_VCMPNEQ_USPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2007, IF_AVX|IF_SANDYBRIDGE},
    /* 2090 */ {I_VCMPNEQ_USPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2016, IF_AVX|IF_SANDYBRIDGE},
    /* 2091 */ {I_VCMPNEQ_USPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2025, IF_AVX|IF_SANDYBRIDGE},
    /* 2092 */ {I_VCMPNLT_UQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2034, IF_AVX|IF_SANDYBRIDGE},
    /* 2093 */ {I_VCMPNLT_UQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2043, IF_AVX|IF_SANDYBRIDGE},
    /* 2094 */ {I_VCMPNLT_UQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2052, IF_AVX|IF_SANDYBRIDGE},
    /* 2095 */ {I_VCMPNLT_UQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2061, IF_AVX|IF_SANDYBRIDGE},
    /* 2096 */ {I_VCMPNLE_UQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2070, IF_AVX|IF_SANDYBRIDGE},
    /* 2097 */ {I_VCMPNLE_UQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2079, IF_AVX|IF_SANDYBRIDGE},
    /* 2098 */ {I_VCMPNLE_UQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2088, IF_AVX|IF_SANDYBRIDGE},
    /* 2099 */ {I_VCMPNLE_UQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2097, IF_AVX|IF_SANDYBRIDGE},
    /* 2100 */ {I_VCMPORD_SPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2106, IF_AVX|IF_SANDYBRIDGE},
    /* 2101 */ {I_VCMPORD_SPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2115, IF_AVX|IF_SANDYBRIDGE},
    /* 2102 */ {I_VCMPORD_SPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2124, IF_AVX|IF_SANDYBRIDGE},
    /* 2103 */ {I_VCMPORD_SPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2133, IF_AVX|IF_SANDYBRIDGE},
    /* 2104 */ {I_VCMPEQ_USPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2142, IF_AVX|IF_SANDYBRIDGE},
    /* 2105 */ {I_VCMPEQ_USPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2151, IF_AVX|IF_SANDYBRIDGE},
    /* 2106 */ {I_VCMPEQ_USPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2160, IF_AVX|IF_SANDYBRIDGE},
    /* 2107 */ {I_VCMPEQ_USPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2169, IF_AVX|IF_SANDYBRIDGE},
    /* 2108 */ {I_VCMPNGE_UQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2178, IF_AVX|IF_SANDYBRIDGE},
    /* 2109 */ {I_VCMPNGE_UQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2187, IF_AVX|IF_SANDYBRIDGE},
    /* 2110 */ {I_VCMPNGE_UQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2196, IF_AVX|IF_SANDYBRIDGE},
    /* 2111 */ {I_VCMPNGE_UQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2205, IF_AVX|IF_SANDYBRIDGE},
    /* 2112 */ {I_VCMPNGT_UQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2214, IF_AVX|IF_SANDYBRIDGE},
    /* 2113 */ {I_VCMPNGT_UQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2223, IF_AVX|IF_SANDYBRIDGE},
    /* 2114 */ {I_VCMPNGT_UQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2232, IF_AVX|IF_SANDYBRIDGE},
    /* 2115 */ {I_VCMPNGT_UQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2241, IF_AVX|IF_SANDYBRIDGE},
    /* 2116 */ {I_VCMPFALSE_OSPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2250, IF_AVX|IF_SANDYBRIDGE},
    /* 2117 */ {I_VCMPFALSE_OSPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2259, IF_AVX|IF_SANDYBRIDGE},
    /* 2118 */ {I_VCMPFALSE_OSPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2268, IF_AVX|IF_SANDYBRIDGE},
    /* 2119 */ {I_VCMPFALSE_OSPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2277, IF_AVX|IF_SANDYBRIDGE},
    /* 2120 */ {I_VCMPNEQ_OSPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2286, IF_AVX|IF_SANDYBRIDGE},
    /* 2121 */ {I_VCMPNEQ_OSPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2295, IF_AVX|IF_SANDYBRIDGE},
    /* 2122 */ {I_VCMPNEQ_OSPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2304, IF_AVX|IF_SANDYBRIDGE},
    /* 2123 */ {I_VCMPNEQ_OSPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2313, IF_AVX|IF_SANDYBRIDGE},
    /* 2124 */ {I_VCMPGE_OQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2322, IF_AVX|IF_SANDYBRIDGE},
    /* 2125 */ {I_VCMPGE_OQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2331, IF_AVX|IF_SANDYBRIDGE},
    /* 2126 */ {I_VCMPGE_OQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2340, IF_AVX|IF_SANDYBRIDGE},
    /* 2127 */ {I_VCMPGE_OQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2349, IF_AVX|IF_SANDYBRIDGE},
    /* 2128 */ {I_VCMPGT_OQPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2358, IF_AVX|IF_SANDYBRIDGE},
    /* 2129 */ {I_VCMPGT_OQPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2367, IF_AVX|IF_SANDYBRIDGE},
    /* 2130 */ {I_VCMPGT_OQPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2376, IF_AVX|IF_SANDYBRIDGE},
    /* 2131 */ {I_VCMPGT_OQPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2385, IF_AVX|IF_SANDYBRIDGE},
    /* 2132 */ {I_VCMPTRUE_USPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+2394, IF_AVX|IF_SANDYBRIDGE},
    /* 2133 */ {I_VCMPTRUE_USPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+2403, IF_AVX|IF_SANDYBRIDGE},
    /* 2134 */ {I_VCMPTRUE_USPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+2412, IF_AVX|IF_SANDYBRIDGE},
    /* 2135 */ {I_VCMPTRUE_USPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+2421, IF_AVX|IF_SANDYBRIDGE},
    /* 2136 */ {I_VCMPPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+5968, IF_AVX|IF_SANDYBRIDGE},
    /* 2137 */ {I_VCMPPS, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+5976, IF_AVX|IF_SANDYBRIDGE},
    /* 2138 */ {I_VCMPPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0}, nasm_bytecodes+5984, IF_AVX|IF_SANDYBRIDGE},
    /* 2139 */ {I_VCMPPS, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+5992, IF_AVX|IF_SANDYBRIDGE},
    /* 2140 */ {I_VCMPEQ_OSSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2430, IF_AVX|IF_SANDYBRIDGE},
    /* 2141 */ {I_VCMPEQ_OSSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2439, IF_AVX|IF_SANDYBRIDGE},
    /* 2142 */ {I_VCMPEQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2430, IF_AVX|IF_SANDYBRIDGE},
    /* 2143 */ {I_VCMPEQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2439, IF_AVX|IF_SANDYBRIDGE},
    /* 2144 */ {I_VCMPLT_OSSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2448, IF_AVX|IF_SANDYBRIDGE},
    /* 2145 */ {I_VCMPLT_OSSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2457, IF_AVX|IF_SANDYBRIDGE},
    /* 2146 */ {I_VCMPLTSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2448, IF_AVX|IF_SANDYBRIDGE},
    /* 2147 */ {I_VCMPLTSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2457, IF_AVX|IF_SANDYBRIDGE},
    /* 2148 */ {I_VCMPLE_OSSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2466, IF_AVX|IF_SANDYBRIDGE},
    /* 2149 */ {I_VCMPLE_OSSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2475, IF_AVX|IF_SANDYBRIDGE},
    /* 2150 */ {I_VCMPLESD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2466, IF_AVX|IF_SANDYBRIDGE},
    /* 2151 */ {I_VCMPLESD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2475, IF_AVX|IF_SANDYBRIDGE},
    /* 2152 */ {I_VCMPUNORD_QSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2484, IF_AVX|IF_SANDYBRIDGE},
    /* 2153 */ {I_VCMPUNORD_QSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2493, IF_AVX|IF_SANDYBRIDGE},
    /* 2154 */ {I_VCMPUNORDSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2484, IF_AVX|IF_SANDYBRIDGE},
    /* 2155 */ {I_VCMPUNORDSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2493, IF_AVX|IF_SANDYBRIDGE},
    /* 2156 */ {I_VCMPNEQ_UQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2502, IF_AVX|IF_SANDYBRIDGE},
    /* 2157 */ {I_VCMPNEQ_UQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2511, IF_AVX|IF_SANDYBRIDGE},
    /* 2158 */ {I_VCMPNEQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2502, IF_AVX|IF_SANDYBRIDGE},
    /* 2159 */ {I_VCMPNEQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2511, IF_AVX|IF_SANDYBRIDGE},
    /* 2160 */ {I_VCMPNLT_USSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2520, IF_AVX|IF_SANDYBRIDGE},
    /* 2161 */ {I_VCMPNLT_USSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2529, IF_AVX|IF_SANDYBRIDGE},
    /* 2162 */ {I_VCMPNLTSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2520, IF_AVX|IF_SANDYBRIDGE},
    /* 2163 */ {I_VCMPNLTSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2529, IF_AVX|IF_SANDYBRIDGE},
    /* 2164 */ {I_VCMPNLE_USSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2538, IF_AVX|IF_SANDYBRIDGE},
    /* 2165 */ {I_VCMPNLE_USSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2547, IF_AVX|IF_SANDYBRIDGE},
    /* 2166 */ {I_VCMPNLESD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2538, IF_AVX|IF_SANDYBRIDGE},
    /* 2167 */ {I_VCMPNLESD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2547, IF_AVX|IF_SANDYBRIDGE},
    /* 2168 */ {I_VCMPORD_QSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2556, IF_AVX|IF_SANDYBRIDGE},
    /* 2169 */ {I_VCMPORD_QSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2565, IF_AVX|IF_SANDYBRIDGE},
    /* 2170 */ {I_VCMPORDSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2556, IF_AVX|IF_SANDYBRIDGE},
    /* 2171 */ {I_VCMPORDSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2565, IF_AVX|IF_SANDYBRIDGE},
    /* 2172 */ {I_VCMPEQ_UQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2574, IF_AVX|IF_SANDYBRIDGE},
    /* 2173 */ {I_VCMPEQ_UQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2583, IF_AVX|IF_SANDYBRIDGE},
    /* 2174 */ {I_VCMPNGE_USSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2592, IF_AVX|IF_SANDYBRIDGE},
    /* 2175 */ {I_VCMPNGE_USSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2601, IF_AVX|IF_SANDYBRIDGE},
    /* 2176 */ {I_VCMPNGESD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2592, IF_AVX|IF_SANDYBRIDGE},
    /* 2177 */ {I_VCMPNGESD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2601, IF_AVX|IF_SANDYBRIDGE},
    /* 2178 */ {I_VCMPNGT_USSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2610, IF_AVX|IF_SANDYBRIDGE},
    /* 2179 */ {I_VCMPNGT_USSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2619, IF_AVX|IF_SANDYBRIDGE},
    /* 2180 */ {I_VCMPNGTSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2610, IF_AVX|IF_SANDYBRIDGE},
    /* 2181 */ {I_VCMPNGTSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2619, IF_AVX|IF_SANDYBRIDGE},
    /* 2182 */ {I_VCMPFALSE_OQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2628, IF_AVX|IF_SANDYBRIDGE},
    /* 2183 */ {I_VCMPFALSE_OQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2637, IF_AVX|IF_SANDYBRIDGE},
    /* 2184 */ {I_VCMPFALSESD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2628, IF_AVX|IF_SANDYBRIDGE},
    /* 2185 */ {I_VCMPFALSESD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2637, IF_AVX|IF_SANDYBRIDGE},
    /* 2186 */ {I_VCMPNEQ_OQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2646, IF_AVX|IF_SANDYBRIDGE},
    /* 2187 */ {I_VCMPNEQ_OQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2655, IF_AVX|IF_SANDYBRIDGE},
    /* 2188 */ {I_VCMPGE_OSSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2664, IF_AVX|IF_SANDYBRIDGE},
    /* 2189 */ {I_VCMPGE_OSSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2673, IF_AVX|IF_SANDYBRIDGE},
    /* 2190 */ {I_VCMPGESD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2664, IF_AVX|IF_SANDYBRIDGE},
    /* 2191 */ {I_VCMPGESD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2673, IF_AVX|IF_SANDYBRIDGE},
    /* 2192 */ {I_VCMPGT_OSSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2682, IF_AVX|IF_SANDYBRIDGE},
    /* 2193 */ {I_VCMPGT_OSSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2691, IF_AVX|IF_SANDYBRIDGE},
    /* 2194 */ {I_VCMPGTSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2682, IF_AVX|IF_SANDYBRIDGE},
    /* 2195 */ {I_VCMPGTSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2691, IF_AVX|IF_SANDYBRIDGE},
    /* 2196 */ {I_VCMPTRUE_UQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2700, IF_AVX|IF_SANDYBRIDGE},
    /* 2197 */ {I_VCMPTRUE_UQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2709, IF_AVX|IF_SANDYBRIDGE},
    /* 2198 */ {I_VCMPTRUESD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2700, IF_AVX|IF_SANDYBRIDGE},
    /* 2199 */ {I_VCMPTRUESD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2709, IF_AVX|IF_SANDYBRIDGE},
    /* 2200 */ {I_VCMPEQ_OSSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2718, IF_AVX|IF_SANDYBRIDGE},
    /* 2201 */ {I_VCMPEQ_OSSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2727, IF_AVX|IF_SANDYBRIDGE},
    /* 2202 */ {I_VCMPLT_OQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2736, IF_AVX|IF_SANDYBRIDGE},
    /* 2203 */ {I_VCMPLT_OQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2745, IF_AVX|IF_SANDYBRIDGE},
    /* 2204 */ {I_VCMPLE_OQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2754, IF_AVX|IF_SANDYBRIDGE},
    /* 2205 */ {I_VCMPLE_OQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2763, IF_AVX|IF_SANDYBRIDGE},
    /* 2206 */ {I_VCMPUNORD_SSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2772, IF_AVX|IF_SANDYBRIDGE},
    /* 2207 */ {I_VCMPUNORD_SSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2781, IF_AVX|IF_SANDYBRIDGE},
    /* 2208 */ {I_VCMPNEQ_USSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2790, IF_AVX|IF_SANDYBRIDGE},
    /* 2209 */ {I_VCMPNEQ_USSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2799, IF_AVX|IF_SANDYBRIDGE},
    /* 2210 */ {I_VCMPNLT_UQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2808, IF_AVX|IF_SANDYBRIDGE},
    /* 2211 */ {I_VCMPNLT_UQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2817, IF_AVX|IF_SANDYBRIDGE},
    /* 2212 */ {I_VCMPNLE_UQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2826, IF_AVX|IF_SANDYBRIDGE},
    /* 2213 */ {I_VCMPNLE_UQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2835, IF_AVX|IF_SANDYBRIDGE},
    /* 2214 */ {I_VCMPORD_SSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2844, IF_AVX|IF_SANDYBRIDGE},
    /* 2215 */ {I_VCMPORD_SSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2853, IF_AVX|IF_SANDYBRIDGE},
    /* 2216 */ {I_VCMPEQ_USSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2862, IF_AVX|IF_SANDYBRIDGE},
    /* 2217 */ {I_VCMPEQ_USSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2871, IF_AVX|IF_SANDYBRIDGE},
    /* 2218 */ {I_VCMPNGE_UQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2880, IF_AVX|IF_SANDYBRIDGE},
    /* 2219 */ {I_VCMPNGE_UQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2889, IF_AVX|IF_SANDYBRIDGE},
    /* 2220 */ {I_VCMPNGT_UQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2898, IF_AVX|IF_SANDYBRIDGE},
    /* 2221 */ {I_VCMPNGT_UQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2907, IF_AVX|IF_SANDYBRIDGE},
    /* 2222 */ {I_VCMPFALSE_OSSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2916, IF_AVX|IF_SANDYBRIDGE},
    /* 2223 */ {I_VCMPFALSE_OSSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2925, IF_AVX|IF_SANDYBRIDGE},
    /* 2224 */ {I_VCMPNEQ_OSSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2934, IF_AVX|IF_SANDYBRIDGE},
    /* 2225 */ {I_VCMPNEQ_OSSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2943, IF_AVX|IF_SANDYBRIDGE},
    /* 2226 */ {I_VCMPGE_OQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2952, IF_AVX|IF_SANDYBRIDGE},
    /* 2227 */ {I_VCMPGE_OQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2961, IF_AVX|IF_SANDYBRIDGE},
    /* 2228 */ {I_VCMPGT_OQSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2970, IF_AVX|IF_SANDYBRIDGE},
    /* 2229 */ {I_VCMPGT_OQSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2979, IF_AVX|IF_SANDYBRIDGE},
    /* 2230 */ {I_VCMPTRUE_USSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+2988, IF_AVX|IF_SANDYBRIDGE},
    /* 2231 */ {I_VCMPTRUE_USSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+2997, IF_AVX|IF_SANDYBRIDGE},
    /* 2232 */ {I_VCMPSD, 4, {XMMREG,XMMREG,RM_XMM|BITS64,IMMEDIATE|BITS8,0}, nasm_bytecodes+6000, IF_AVX|IF_SANDYBRIDGE},
    /* 2233 */ {I_VCMPSD, 3, {XMMREG,RM_XMM|BITS64,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6008, IF_AVX|IF_SANDYBRIDGE},
    /* 2234 */ {I_VCMPEQ_OSSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3006, IF_AVX|IF_SANDYBRIDGE},
    /* 2235 */ {I_VCMPEQ_OSSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3015, IF_AVX|IF_SANDYBRIDGE},
    /* 2236 */ {I_VCMPEQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3006, IF_AVX|IF_SANDYBRIDGE},
    /* 2237 */ {I_VCMPEQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3015, IF_AVX|IF_SANDYBRIDGE},
    /* 2238 */ {I_VCMPLT_OSSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3024, IF_AVX|IF_SANDYBRIDGE},
    /* 2239 */ {I_VCMPLT_OSSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3033, IF_AVX|IF_SANDYBRIDGE},
    /* 2240 */ {I_VCMPLTSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3024, IF_AVX|IF_SANDYBRIDGE},
    /* 2241 */ {I_VCMPLTSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3033, IF_AVX|IF_SANDYBRIDGE},
    /* 2242 */ {I_VCMPLE_OSSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3042, IF_AVX|IF_SANDYBRIDGE},
    /* 2243 */ {I_VCMPLE_OSSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3051, IF_AVX|IF_SANDYBRIDGE},
    /* 2244 */ {I_VCMPLESS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3042, IF_AVX|IF_SANDYBRIDGE},
    /* 2245 */ {I_VCMPLESS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3051, IF_AVX|IF_SANDYBRIDGE},
    /* 2246 */ {I_VCMPUNORD_QSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3060, IF_AVX|IF_SANDYBRIDGE},
    /* 2247 */ {I_VCMPUNORD_QSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3069, IF_AVX|IF_SANDYBRIDGE},
    /* 2248 */ {I_VCMPUNORDSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3060, IF_AVX|IF_SANDYBRIDGE},
    /* 2249 */ {I_VCMPUNORDSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3069, IF_AVX|IF_SANDYBRIDGE},
    /* 2250 */ {I_VCMPNEQ_UQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3078, IF_AVX|IF_SANDYBRIDGE},
    /* 2251 */ {I_VCMPNEQ_UQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3087, IF_AVX|IF_SANDYBRIDGE},
    /* 2252 */ {I_VCMPNEQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3078, IF_AVX|IF_SANDYBRIDGE},
    /* 2253 */ {I_VCMPNEQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3087, IF_AVX|IF_SANDYBRIDGE},
    /* 2254 */ {I_VCMPNLT_USSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3096, IF_AVX|IF_SANDYBRIDGE},
    /* 2255 */ {I_VCMPNLT_USSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3105, IF_AVX|IF_SANDYBRIDGE},
    /* 2256 */ {I_VCMPNLTSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3096, IF_AVX|IF_SANDYBRIDGE},
    /* 2257 */ {I_VCMPNLTSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3105, IF_AVX|IF_SANDYBRIDGE},
    /* 2258 */ {I_VCMPNLE_USSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3114, IF_AVX|IF_SANDYBRIDGE},
    /* 2259 */ {I_VCMPNLE_USSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3123, IF_AVX|IF_SANDYBRIDGE},
    /* 2260 */ {I_VCMPNLESS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3114, IF_AVX|IF_SANDYBRIDGE},
    /* 2261 */ {I_VCMPNLESS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3123, IF_AVX|IF_SANDYBRIDGE},
    /* 2262 */ {I_VCMPORD_QSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3132, IF_AVX|IF_SANDYBRIDGE},
    /* 2263 */ {I_VCMPORD_QSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3141, IF_AVX|IF_SANDYBRIDGE},
    /* 2264 */ {I_VCMPORDSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3132, IF_AVX|IF_SANDYBRIDGE},
    /* 2265 */ {I_VCMPORDSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3141, IF_AVX|IF_SANDYBRIDGE},
    /* 2266 */ {I_VCMPEQ_UQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3150, IF_AVX|IF_SANDYBRIDGE},
    /* 2267 */ {I_VCMPEQ_UQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3159, IF_AVX|IF_SANDYBRIDGE},
    /* 2268 */ {I_VCMPNGE_USSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3168, IF_AVX|IF_SANDYBRIDGE},
    /* 2269 */ {I_VCMPNGE_USSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3177, IF_AVX|IF_SANDYBRIDGE},
    /* 2270 */ {I_VCMPNGESS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3168, IF_AVX|IF_SANDYBRIDGE},
    /* 2271 */ {I_VCMPNGESS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3177, IF_AVX|IF_SANDYBRIDGE},
    /* 2272 */ {I_VCMPNGT_USSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3186, IF_AVX|IF_SANDYBRIDGE},
    /* 2273 */ {I_VCMPNGT_USSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3195, IF_AVX|IF_SANDYBRIDGE},
    /* 2274 */ {I_VCMPNGTSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3186, IF_AVX|IF_SANDYBRIDGE},
    /* 2275 */ {I_VCMPNGTSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3195, IF_AVX|IF_SANDYBRIDGE},
    /* 2276 */ {I_VCMPFALSE_OQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3204, IF_AVX|IF_SANDYBRIDGE},
    /* 2277 */ {I_VCMPFALSE_OQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3213, IF_AVX|IF_SANDYBRIDGE},
    /* 2278 */ {I_VCMPFALSESS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3204, IF_AVX|IF_SANDYBRIDGE},
    /* 2279 */ {I_VCMPFALSESS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3213, IF_AVX|IF_SANDYBRIDGE},
    /* 2280 */ {I_VCMPNEQ_OQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3222, IF_AVX|IF_SANDYBRIDGE},
    /* 2281 */ {I_VCMPNEQ_OQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3231, IF_AVX|IF_SANDYBRIDGE},
    /* 2282 */ {I_VCMPGE_OSSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3240, IF_AVX|IF_SANDYBRIDGE},
    /* 2283 */ {I_VCMPGE_OSSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3249, IF_AVX|IF_SANDYBRIDGE},
    /* 2284 */ {I_VCMPGESS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3240, IF_AVX|IF_SANDYBRIDGE},
    /* 2285 */ {I_VCMPGESS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3249, IF_AVX|IF_SANDYBRIDGE},
    /* 2286 */ {I_VCMPGT_OSSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3258, IF_AVX|IF_SANDYBRIDGE},
    /* 2287 */ {I_VCMPGT_OSSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3267, IF_AVX|IF_SANDYBRIDGE},
    /* 2288 */ {I_VCMPGTSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3258, IF_AVX|IF_SANDYBRIDGE},
    /* 2289 */ {I_VCMPGTSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3267, IF_AVX|IF_SANDYBRIDGE},
    /* 2290 */ {I_VCMPTRUE_UQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3276, IF_AVX|IF_SANDYBRIDGE},
    /* 2291 */ {I_VCMPTRUE_UQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3285, IF_AVX|IF_SANDYBRIDGE},
    /* 2292 */ {I_VCMPTRUESS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3276, IF_AVX|IF_SANDYBRIDGE},
    /* 2293 */ {I_VCMPTRUESS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3285, IF_AVX|IF_SANDYBRIDGE},
    /* 2294 */ {I_VCMPEQ_OSSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3294, IF_AVX|IF_SANDYBRIDGE},
    /* 2295 */ {I_VCMPEQ_OSSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3303, IF_AVX|IF_SANDYBRIDGE},
    /* 2296 */ {I_VCMPLT_OQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3312, IF_AVX|IF_SANDYBRIDGE},
    /* 2297 */ {I_VCMPLT_OQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3321, IF_AVX|IF_SANDYBRIDGE},
    /* 2298 */ {I_VCMPLE_OQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3330, IF_AVX|IF_SANDYBRIDGE},
    /* 2299 */ {I_VCMPLE_OQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3339, IF_AVX|IF_SANDYBRIDGE},
    /* 2300 */ {I_VCMPUNORD_SSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3348, IF_AVX|IF_SANDYBRIDGE},
    /* 2301 */ {I_VCMPUNORD_SSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3357, IF_AVX|IF_SANDYBRIDGE},
    /* 2302 */ {I_VCMPNEQ_USSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3366, IF_AVX|IF_SANDYBRIDGE},
    /* 2303 */ {I_VCMPNEQ_USSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3375, IF_AVX|IF_SANDYBRIDGE},
    /* 2304 */ {I_VCMPNLT_UQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3384, IF_AVX|IF_SANDYBRIDGE},
    /* 2305 */ {I_VCMPNLT_UQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3393, IF_AVX|IF_SANDYBRIDGE},
    /* 2306 */ {I_VCMPNLE_UQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3402, IF_AVX|IF_SANDYBRIDGE},
    /* 2307 */ {I_VCMPNLE_UQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3411, IF_AVX|IF_SANDYBRIDGE},
    /* 2308 */ {I_VCMPORD_SSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3420, IF_AVX|IF_SANDYBRIDGE},
    /* 2309 */ {I_VCMPORD_SSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3429, IF_AVX|IF_SANDYBRIDGE},
    /* 2310 */ {I_VCMPEQ_USSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3438, IF_AVX|IF_SANDYBRIDGE},
    /* 2311 */ {I_VCMPEQ_USSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3447, IF_AVX|IF_SANDYBRIDGE},
    /* 2312 */ {I_VCMPNGE_UQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3456, IF_AVX|IF_SANDYBRIDGE},
    /* 2313 */ {I_VCMPNGE_UQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3465, IF_AVX|IF_SANDYBRIDGE},
    /* 2314 */ {I_VCMPNGT_UQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3474, IF_AVX|IF_SANDYBRIDGE},
    /* 2315 */ {I_VCMPNGT_UQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3483, IF_AVX|IF_SANDYBRIDGE},
    /* 2316 */ {I_VCMPFALSE_OSSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3492, IF_AVX|IF_SANDYBRIDGE},
    /* 2317 */ {I_VCMPFALSE_OSSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3501, IF_AVX|IF_SANDYBRIDGE},
    /* 2318 */ {I_VCMPNEQ_OSSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3510, IF_AVX|IF_SANDYBRIDGE},
    /* 2319 */ {I_VCMPNEQ_OSSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3519, IF_AVX|IF_SANDYBRIDGE},
    /* 2320 */ {I_VCMPGE_OQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3528, IF_AVX|IF_SANDYBRIDGE},
    /* 2321 */ {I_VCMPGE_OQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3537, IF_AVX|IF_SANDYBRIDGE},
    /* 2322 */ {I_VCMPGT_OQSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3546, IF_AVX|IF_SANDYBRIDGE},
    /* 2323 */ {I_VCMPGT_OQSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3555, IF_AVX|IF_SANDYBRIDGE},
    /* 2324 */ {I_VCMPTRUE_USSS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3564, IF_AVX|IF_SANDYBRIDGE},
    /* 2325 */ {I_VCMPTRUE_USSS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+3573, IF_AVX|IF_SANDYBRIDGE},
    /* 2326 */ {I_VCMPSS, 4, {XMMREG,XMMREG,RM_XMM|BITS64,IMMEDIATE|BITS8,0}, nasm_bytecodes+6016, IF_AVX|IF_SANDYBRIDGE},
    /* 2327 */ {I_VCMPSS, 3, {XMMREG,RM_XMM|BITS64,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6024, IF_AVX|IF_SANDYBRIDGE},
    /* 2328 */ {I_VCOMISD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+8775, IF_AVX|IF_SANDYBRIDGE},
    /* 2329 */ {I_VCOMISS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+8782, IF_AVX|IF_SANDYBRIDGE},
    /* 2330 */ {I_VCVTDQ2PD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+8789, IF_AVX|IF_SANDYBRIDGE},
    /* 2331 */ {I_VCVTDQ2PD, 2, {YMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8796, IF_AVX|IF_SANDYBRIDGE},
    /* 2332 */ {I_VCVTDQ2PS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8803, IF_AVX|IF_SANDYBRIDGE},
    /* 2333 */ {I_VCVTDQ2PS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+8810, IF_AVX|IF_SANDYBRIDGE},
    /* 2334 */ {I_VCVTPD2DQ, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+8817, IF_AVX|IF_SANDYBRIDGE},
    /* 2335 */ {I_VCVTPD2DQ, 2, {XMMREG,MEMORY|BITS128,0,0,0}, nasm_bytecodes+8817, IF_AVX|IF_SANDYBRIDGE|IF_SO},
    /* 2336 */ {I_VCVTPD2DQ, 2, {XMMREG,YMMREG,0,0,0}, nasm_bytecodes+8824, IF_AVX|IF_SANDYBRIDGE},
    /* 2337 */ {I_VCVTPD2DQ, 2, {XMMREG,MEMORY|BITS256,0,0,0}, nasm_bytecodes+8824, IF_AVX|IF_SANDYBRIDGE|IF_SY},
    /* 2338 */ {I_VCVTPD2PS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+8831, IF_AVX|IF_SANDYBRIDGE},
    /* 2339 */ {I_VCVTPD2PS, 2, {XMMREG,MEMORY|BITS128,0,0,0}, nasm_bytecodes+8831, IF_AVX|IF_SANDYBRIDGE|IF_SO},
    /* 2340 */ {I_VCVTPD2PS, 2, {XMMREG,YMMREG,0,0,0}, nasm_bytecodes+8838, IF_AVX|IF_SANDYBRIDGE},
    /* 2341 */ {I_VCVTPD2PS, 2, {XMMREG,MEMORY|BITS256,0,0,0}, nasm_bytecodes+8838, IF_AVX|IF_SANDYBRIDGE|IF_SY},
    /* 2342 */ {I_VCVTPS2DQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8845, IF_AVX|IF_SANDYBRIDGE},
    /* 2343 */ {I_VCVTPS2DQ, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+8852, IF_AVX|IF_SANDYBRIDGE},
    /* 2344 */ {I_VCVTPS2PD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+8859, IF_AVX|IF_SANDYBRIDGE},
    /* 2345 */ {I_VCVTPS2PD, 2, {YMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8866, IF_AVX|IF_SANDYBRIDGE},
    /* 2346 */ {I_VCVTSD2SI, 2, {REG_GPR|BITS32,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+8873, IF_AVX|IF_SANDYBRIDGE},
    /* 2347 */ {I_VCVTSD2SI, 2, {REG_GPR|BITS64,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+8880, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2348 */ {I_VCVTSD2SS, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+8887, IF_AVX|IF_SANDYBRIDGE},
    /* 2349 */ {I_VCVTSD2SS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+8894, IF_AVX|IF_SANDYBRIDGE},
    /* 2350 */ {I_VCVTSI2SD, 3, {XMMREG,XMMREG,RM_GPR|BITS32,0,0}, nasm_bytecodes+8901, IF_AVX|IF_SANDYBRIDGE|IF_SD},
    /* 2351 */ {I_VCVTSI2SD, 2, {XMMREG,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+8908, IF_AVX|IF_SANDYBRIDGE|IF_SD},
    /* 2352 */ {I_VCVTSI2SD, 3, {XMMREG,XMMREG,RM_GPR|BITS64,0,0}, nasm_bytecodes+8915, IF_AVX|IF_SANDYBRIDGE|IF_LONG|IF_SQ},
    /* 2353 */ {I_VCVTSI2SD, 2, {XMMREG,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+8922, IF_AVX|IF_SANDYBRIDGE|IF_LONG|IF_SQ},
    /* 2354 */ {I_VCVTSI2SS, 3, {XMMREG,XMMREG,RM_GPR|BITS32,0,0}, nasm_bytecodes+8929, IF_AVX|IF_SANDYBRIDGE|IF_SD},
    /* 2355 */ {I_VCVTSI2SS, 2, {XMMREG,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+8936, IF_AVX|IF_SANDYBRIDGE|IF_SD},
    /* 2356 */ {I_VCVTSI2SS, 3, {XMMREG,XMMREG,RM_GPR|BITS64,0,0}, nasm_bytecodes+8943, IF_AVX|IF_SANDYBRIDGE|IF_LONG|IF_SQ},
    /* 2357 */ {I_VCVTSI2SS, 2, {XMMREG,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+8950, IF_AVX|IF_SANDYBRIDGE|IF_LONG|IF_SQ},
    /* 2358 */ {I_VCVTSS2SD, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+8957, IF_AVX|IF_SANDYBRIDGE},
    /* 2359 */ {I_VCVTSS2SD, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+8964, IF_AVX|IF_SANDYBRIDGE},
    /* 2360 */ {I_VCVTSS2SI, 2, {REG_GPR|BITS32,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+8971, IF_AVX|IF_SANDYBRIDGE},
    /* 2361 */ {I_VCVTSS2SI, 2, {REG_GPR|BITS64,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+8978, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2362 */ {I_VCVTTPD2DQ, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+8985, IF_AVX|IF_SANDYBRIDGE},
    /* 2363 */ {I_VCVTTPD2DQ, 2, {XMMREG,MEMORY|BITS128,0,0,0}, nasm_bytecodes+8985, IF_AVX|IF_SANDYBRIDGE|IF_SO},
    /* 2364 */ {I_VCVTTPD2DQ, 2, {XMMREG,YMMREG,0,0,0}, nasm_bytecodes+8992, IF_AVX|IF_SANDYBRIDGE},
    /* 2365 */ {I_VCVTTPD2DQ, 2, {XMMREG,MEMORY|BITS256,0,0,0}, nasm_bytecodes+8992, IF_AVX|IF_SANDYBRIDGE|IF_SY},
    /* 2366 */ {I_VCVTTPS2DQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+8999, IF_AVX|IF_SANDYBRIDGE},
    /* 2367 */ {I_VCVTTPS2DQ, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9006, IF_AVX|IF_SANDYBRIDGE},
    /* 2368 */ {I_VCVTTSD2SI, 2, {REG_GPR|BITS32,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+9013, IF_AVX|IF_SANDYBRIDGE},
    /* 2369 */ {I_VCVTTSD2SI, 2, {REG_GPR|BITS64,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+9020, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2370 */ {I_VCVTTSS2SI, 2, {REG_GPR|BITS32,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+9027, IF_AVX|IF_SANDYBRIDGE},
    /* 2371 */ {I_VCVTTSS2SI, 2, {REG_GPR|BITS64,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+9034, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2372 */ {I_VDIVPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9041, IF_AVX|IF_SANDYBRIDGE},
    /* 2373 */ {I_VDIVPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9048, IF_AVX|IF_SANDYBRIDGE},
    /* 2374 */ {I_VDIVPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+9055, IF_AVX|IF_SANDYBRIDGE},
    /* 2375 */ {I_VDIVPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9062, IF_AVX|IF_SANDYBRIDGE},
    /* 2376 */ {I_VDIVPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9069, IF_AVX|IF_SANDYBRIDGE},
    /* 2377 */ {I_VDIVPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9076, IF_AVX|IF_SANDYBRIDGE},
    /* 2378 */ {I_VDIVPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+9083, IF_AVX|IF_SANDYBRIDGE},
    /* 2379 */ {I_VDIVPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9090, IF_AVX|IF_SANDYBRIDGE},
    /* 2380 */ {I_VDIVSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+9097, IF_AVX|IF_SANDYBRIDGE},
    /* 2381 */ {I_VDIVSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+9104, IF_AVX|IF_SANDYBRIDGE},
    /* 2382 */ {I_VDIVSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+9111, IF_AVX|IF_SANDYBRIDGE},
    /* 2383 */ {I_VDIVSS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+9118, IF_AVX|IF_SANDYBRIDGE},
    /* 2384 */ {I_VDPPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6032, IF_AVX|IF_SANDYBRIDGE},
    /* 2385 */ {I_VDPPD, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6040, IF_AVX|IF_SANDYBRIDGE},
    /* 2386 */ {I_VDPPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6048, IF_AVX|IF_SANDYBRIDGE},
    /* 2387 */ {I_VDPPS, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6056, IF_AVX|IF_SANDYBRIDGE},
    /* 2388 */ {I_VDPPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0}, nasm_bytecodes+6064, IF_AVX|IF_SANDYBRIDGE},
    /* 2389 */ {I_VDPPS, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6072, IF_AVX|IF_SANDYBRIDGE},
    /* 2390 */ {I_VEXTRACTF128, 3, {RM_XMM|BITS128,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6080, IF_AVX|IF_SANDYBRIDGE},
    /* 2391 */ {I_VEXTRACTPS, 3, {RM_GPR|BITS32,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6088, IF_AVX|IF_SANDYBRIDGE},
    /* 2392 */ {I_VHADDPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9125, IF_AVX|IF_SANDYBRIDGE},
    /* 2393 */ {I_VHADDPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9132, IF_AVX|IF_SANDYBRIDGE},
    /* 2394 */ {I_VHADDPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+9139, IF_AVX|IF_SANDYBRIDGE},
    /* 2395 */ {I_VHADDPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9146, IF_AVX|IF_SANDYBRIDGE},
    /* 2396 */ {I_VHADDPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9153, IF_AVX|IF_SANDYBRIDGE},
    /* 2397 */ {I_VHADDPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9160, IF_AVX|IF_SANDYBRIDGE},
    /* 2398 */ {I_VHADDPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+9167, IF_AVX|IF_SANDYBRIDGE},
    /* 2399 */ {I_VHADDPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9174, IF_AVX|IF_SANDYBRIDGE},
    /* 2400 */ {I_VHSUBPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9181, IF_AVX|IF_SANDYBRIDGE},
    /* 2401 */ {I_VHSUBPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9188, IF_AVX|IF_SANDYBRIDGE},
    /* 2402 */ {I_VHSUBPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+9195, IF_AVX|IF_SANDYBRIDGE},
    /* 2403 */ {I_VHSUBPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9202, IF_AVX|IF_SANDYBRIDGE},
    /* 2404 */ {I_VHSUBPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9209, IF_AVX|IF_SANDYBRIDGE},
    /* 2405 */ {I_VHSUBPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9216, IF_AVX|IF_SANDYBRIDGE},
    /* 2406 */ {I_VHSUBPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+9223, IF_AVX|IF_SANDYBRIDGE},
    /* 2407 */ {I_VHSUBPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9230, IF_AVX|IF_SANDYBRIDGE},
    /* 2408 */ {I_VINSERTF128, 4, {YMMREG,YMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6096, IF_AVX|IF_SANDYBRIDGE},
    /* 2409 */ {I_VINSERTPS, 4, {XMMREG,XMMREG,RM_XMM|BITS32,IMMEDIATE|BITS8,0}, nasm_bytecodes+6104, IF_AVX|IF_SANDYBRIDGE},
    /* 2410 */ {I_VINSERTPS, 3, {XMMREG,RM_XMM|BITS32,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6112, IF_AVX|IF_SANDYBRIDGE},
    /* 2411 */ {I_VLDDQU, 2, {XMMREG,MEMORY|BITS128,0,0,0}, nasm_bytecodes+9237, IF_AVX|IF_SANDYBRIDGE},
    /* 2412 */ {I_VLDQQU, 2, {YMMREG,MEMORY|BITS256,0,0,0}, nasm_bytecodes+9244, IF_AVX|IF_SANDYBRIDGE},
    /* 2413 */ {I_VLDMXCSR, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+9251, IF_AVX|IF_SANDYBRIDGE},
    /* 2414 */ {I_VMASKMOVDQU, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+9258, IF_AVX|IF_SANDYBRIDGE},
    /* 2415 */ {I_VMASKMOVPS, 3, {XMMREG,XMMREG,MEMORY|BITS128,0,0}, nasm_bytecodes+9265, IF_AVX|IF_SANDYBRIDGE},
    /* 2416 */ {I_VMASKMOVPS, 3, {YMMREG,YMMREG,MEMORY|BITS256,0,0}, nasm_bytecodes+9272, IF_AVX|IF_SANDYBRIDGE},
    /* 2417 */ {I_VMASKMOVPS, 3, {MEMORY|BITS128,XMMREG,XMMREG,0,0}, nasm_bytecodes+9279, IF_AVX|IF_SANDYBRIDGE|IF_SO},
    /* 2418 */ {I_VMASKMOVPS, 3, {MEMORY|BITS256,XMMREG,XMMREG,0,0}, nasm_bytecodes+9286, IF_AVX|IF_SANDYBRIDGE|IF_SY},
    /* 2419 */ {I_VMASKMOVPD, 3, {XMMREG,XMMREG,MEMORY|BITS128,0,0}, nasm_bytecodes+9293, IF_AVX|IF_SANDYBRIDGE},
    /* 2420 */ {I_VMASKMOVPD, 3, {YMMREG,YMMREG,MEMORY|BITS256,0,0}, nasm_bytecodes+9300, IF_AVX|IF_SANDYBRIDGE},
    /* 2421 */ {I_VMASKMOVPD, 3, {MEMORY|BITS128,XMMREG,XMMREG,0,0}, nasm_bytecodes+9307, IF_AVX|IF_SANDYBRIDGE},
    /* 2422 */ {I_VMASKMOVPD, 3, {MEMORY|BITS256,YMMREG,YMMREG,0,0}, nasm_bytecodes+9314, IF_AVX|IF_SANDYBRIDGE},
    /* 2423 */ {I_VMAXPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9321, IF_AVX|IF_SANDYBRIDGE},
    /* 2424 */ {I_VMAXPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9328, IF_AVX|IF_SANDYBRIDGE},
    /* 2425 */ {I_VMAXPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+9335, IF_AVX|IF_SANDYBRIDGE},
    /* 2426 */ {I_VMAXPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9342, IF_AVX|IF_SANDYBRIDGE},
    /* 2427 */ {I_VMAXPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9349, IF_AVX|IF_SANDYBRIDGE},
    /* 2428 */ {I_VMAXPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9356, IF_AVX|IF_SANDYBRIDGE},
    /* 2429 */ {I_VMAXPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+9363, IF_AVX|IF_SANDYBRIDGE},
    /* 2430 */ {I_VMAXPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9370, IF_AVX|IF_SANDYBRIDGE},
    /* 2431 */ {I_VMAXSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+9377, IF_AVX|IF_SANDYBRIDGE},
    /* 2432 */ {I_VMAXSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+9384, IF_AVX|IF_SANDYBRIDGE},
    /* 2433 */ {I_VMAXSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+9391, IF_AVX|IF_SANDYBRIDGE},
    /* 2434 */ {I_VMAXSS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+9398, IF_AVX|IF_SANDYBRIDGE},
    /* 2435 */ {I_VMINPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9405, IF_AVX|IF_SANDYBRIDGE},
    /* 2436 */ {I_VMINPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9412, IF_AVX|IF_SANDYBRIDGE},
    /* 2437 */ {I_VMINPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+9419, IF_AVX|IF_SANDYBRIDGE},
    /* 2438 */ {I_VMINPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9426, IF_AVX|IF_SANDYBRIDGE},
    /* 2439 */ {I_VMINPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9433, IF_AVX|IF_SANDYBRIDGE},
    /* 2440 */ {I_VMINPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9440, IF_AVX|IF_SANDYBRIDGE},
    /* 2441 */ {I_VMINPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+9447, IF_AVX|IF_SANDYBRIDGE},
    /* 2442 */ {I_VMINPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9454, IF_AVX|IF_SANDYBRIDGE},
    /* 2443 */ {I_VMINSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+9461, IF_AVX|IF_SANDYBRIDGE},
    /* 2444 */ {I_VMINSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+9468, IF_AVX|IF_SANDYBRIDGE},
    /* 2445 */ {I_VMINSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+9475, IF_AVX|IF_SANDYBRIDGE},
    /* 2446 */ {I_VMINSS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+9482, IF_AVX|IF_SANDYBRIDGE},
    /* 2447 */ {I_VMOVAPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9489, IF_AVX|IF_SANDYBRIDGE},
    /* 2448 */ {I_VMOVAPD, 2, {RM_XMM|BITS128,XMMREG,0,0,0}, nasm_bytecodes+9496, IF_AVX|IF_SANDYBRIDGE},
    /* 2449 */ {I_VMOVAPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9503, IF_AVX|IF_SANDYBRIDGE},
    /* 2450 */ {I_VMOVAPD, 2, {RM_YMM|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9510, IF_AVX|IF_SANDYBRIDGE},
    /* 2451 */ {I_VMOVAPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9517, IF_AVX|IF_SANDYBRIDGE},
    /* 2452 */ {I_VMOVAPS, 2, {RM_XMM|BITS128,XMMREG,0,0,0}, nasm_bytecodes+9524, IF_AVX|IF_SANDYBRIDGE},
    /* 2453 */ {I_VMOVAPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9531, IF_AVX|IF_SANDYBRIDGE},
    /* 2454 */ {I_VMOVAPS, 2, {RM_YMM|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9538, IF_AVX|IF_SANDYBRIDGE},
    /* 2455 */ {I_VMOVD, 2, {XMMREG,RM_GPR|BITS32,0,0,0}, nasm_bytecodes+9545, IF_AVX|IF_SANDYBRIDGE},
    /* 2456 */ {I_VMOVD, 2, {RM_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+9552, IF_AVX|IF_SANDYBRIDGE},
    /* 2457 */ {I_VMOVQ, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+9559, IF_AVX|IF_SANDYBRIDGE|IF_SQ},
    /* 2458 */ {I_VMOVQ, 2, {RM_XMM|BITS64,XMMREG,0,0,0}, nasm_bytecodes+9566, IF_AVX|IF_SANDYBRIDGE|IF_SQ},
    /* 2459 */ {I_VMOVQ, 2, {XMMREG,RM_GPR|BITS64,0,0,0}, nasm_bytecodes+9573, IF_AVX|IF_SANDYBRIDGE|IF_LONG|IF_SQ},
    /* 2460 */ {I_VMOVQ, 2, {RM_GPR|BITS64,XMMREG,0,0,0}, nasm_bytecodes+9580, IF_AVX|IF_SANDYBRIDGE|IF_LONG|IF_SQ},
    /* 2461 */ {I_VMOVDDUP, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+9587, IF_AVX|IF_SANDYBRIDGE},
    /* 2462 */ {I_VMOVDDUP, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9594, IF_AVX|IF_SANDYBRIDGE},
    /* 2463 */ {I_VMOVDQA, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9601, IF_AVX|IF_SANDYBRIDGE},
    /* 2464 */ {I_VMOVDQA, 2, {RM_XMM|BITS128,XMMREG,0,0,0}, nasm_bytecodes+9608, IF_AVX|IF_SANDYBRIDGE},
    /* 2465 */ {I_VMOVQQA, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9615, IF_AVX|IF_SANDYBRIDGE},
    /* 2466 */ {I_VMOVQQA, 2, {RM_YMM|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9622, IF_AVX|IF_SANDYBRIDGE},
    /* 2467 */ {I_VMOVDQA, 2, {YMMREG,RM_YMM,0,0,0}, nasm_bytecodes+9615, IF_AVX|IF_SANDYBRIDGE},
    /* 2468 */ {I_VMOVDQA, 2, {RM_YMM|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9622, IF_AVX|IF_SANDYBRIDGE},
    /* 2469 */ {I_VMOVDQU, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9629, IF_AVX|IF_SANDYBRIDGE},
    /* 2470 */ {I_VMOVDQU, 2, {RM_XMM|BITS128,XMMREG,0,0,0}, nasm_bytecodes+9636, IF_AVX|IF_SANDYBRIDGE},
    /* 2471 */ {I_VMOVQQU, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9643, IF_AVX|IF_SANDYBRIDGE},
    /* 2472 */ {I_VMOVQQU, 2, {RM_YMM|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9650, IF_AVX|IF_SANDYBRIDGE},
    /* 2473 */ {I_VMOVDQU, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9643, IF_AVX|IF_SANDYBRIDGE},
    /* 2474 */ {I_VMOVDQU, 2, {RM_YMM|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9650, IF_AVX|IF_SANDYBRIDGE},
    /* 2475 */ {I_VMOVHLPS, 3, {XMMREG,XMMREG,XMMREG,0,0}, nasm_bytecodes+9657, IF_AVX|IF_SANDYBRIDGE},
    /* 2476 */ {I_VMOVHLPS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+9664, IF_AVX|IF_SANDYBRIDGE},
    /* 2477 */ {I_VMOVHPD, 3, {XMMREG,XMMREG,MEMORY|BITS64,0,0}, nasm_bytecodes+9671, IF_AVX|IF_SANDYBRIDGE},
    /* 2478 */ {I_VMOVHPD, 2, {XMMREG,MEMORY|BITS64,0,0,0}, nasm_bytecodes+9678, IF_AVX|IF_SANDYBRIDGE},
    /* 2479 */ {I_VMOVHPD, 2, {MEMORY|BITS64,XMMREG,0,0,0}, nasm_bytecodes+9685, IF_AVX|IF_SANDYBRIDGE},
    /* 2480 */ {I_VMOVHPS, 3, {XMMREG,XMMREG,MEMORY|BITS64,0,0}, nasm_bytecodes+9692, IF_AVX|IF_SANDYBRIDGE},
    /* 2481 */ {I_VMOVHPS, 2, {XMMREG,MEMORY|BITS64,0,0,0}, nasm_bytecodes+9699, IF_AVX|IF_SANDYBRIDGE},
    /* 2482 */ {I_VMOVHPS, 2, {MEMORY|BITS64,XMMREG,0,0,0}, nasm_bytecodes+9706, IF_AVX|IF_SANDYBRIDGE},
    /* 2483 */ {I_VMOVLHPS, 3, {XMMREG,XMMREG,XMMREG,0,0}, nasm_bytecodes+9692, IF_AVX|IF_SANDYBRIDGE},
    /* 2484 */ {I_VMOVLHPS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+9699, IF_AVX|IF_SANDYBRIDGE},
    /* 2485 */ {I_VMOVLPD, 3, {XMMREG,XMMREG,MEMORY|BITS64,0,0}, nasm_bytecodes+9713, IF_AVX|IF_SANDYBRIDGE},
    /* 2486 */ {I_VMOVLPD, 2, {XMMREG,MEMORY|BITS64,0,0,0}, nasm_bytecodes+9720, IF_AVX|IF_SANDYBRIDGE},
    /* 2487 */ {I_VMOVLPD, 2, {MEMORY|BITS64,XMMREG,0,0,0}, nasm_bytecodes+9727, IF_AVX|IF_SANDYBRIDGE},
    /* 2488 */ {I_VMOVLPS, 3, {XMMREG,XMMREG,MEMORY|BITS64,0,0}, nasm_bytecodes+9657, IF_AVX|IF_SANDYBRIDGE},
    /* 2489 */ {I_VMOVLPS, 2, {XMMREG,MEMORY|BITS64,0,0,0}, nasm_bytecodes+9664, IF_AVX|IF_SANDYBRIDGE},
    /* 2490 */ {I_VMOVLPS, 2, {MEMORY|BITS64,XMMREG,0,0,0}, nasm_bytecodes+9734, IF_AVX|IF_SANDYBRIDGE},
    /* 2491 */ {I_VMOVMSKPD, 2, {REG_GPR|BITS64,XMMREG,0,0,0}, nasm_bytecodes+9741, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2492 */ {I_VMOVMSKPD, 2, {REG_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+9741, IF_AVX|IF_SANDYBRIDGE},
    /* 2493 */ {I_VMOVMSKPD, 2, {REG_GPR|BITS64,YMMREG,0,0,0}, nasm_bytecodes+9748, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2494 */ {I_VMOVMSKPD, 2, {REG_GPR|BITS32,YMMREG,0,0,0}, nasm_bytecodes+9748, IF_AVX|IF_SANDYBRIDGE},
    /* 2495 */ {I_VMOVMSKPS, 2, {REG_GPR|BITS64,XMMREG,0,0,0}, nasm_bytecodes+9755, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2496 */ {I_VMOVMSKPS, 2, {REG_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+9755, IF_AVX|IF_SANDYBRIDGE},
    /* 2497 */ {I_VMOVMSKPS, 2, {REG_GPR|BITS64,YMMREG,0,0,0}, nasm_bytecodes+9762, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2498 */ {I_VMOVMSKPS, 2, {REG_GPR|BITS32,YMMREG,0,0,0}, nasm_bytecodes+9762, IF_AVX|IF_SANDYBRIDGE},
    /* 2499 */ {I_VMOVNTDQ, 2, {MEMORY|BITS128,XMMREG,0,0,0}, nasm_bytecodes+9769, IF_AVX|IF_SANDYBRIDGE},
    /* 2500 */ {I_VMOVNTQQ, 2, {MEMORY|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9776, IF_AVX|IF_SANDYBRIDGE},
    /* 2501 */ {I_VMOVNTDQ, 2, {MEMORY|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9776, IF_AVX|IF_SANDYBRIDGE},
    /* 2502 */ {I_VMOVNTDQA, 2, {XMMREG,MEMORY|BITS128,0,0,0}, nasm_bytecodes+9783, IF_AVX|IF_SANDYBRIDGE},
    /* 2503 */ {I_VMOVNTPD, 2, {MEMORY|BITS128,XMMREG,0,0,0}, nasm_bytecodes+9790, IF_AVX|IF_SANDYBRIDGE},
    /* 2504 */ {I_VMOVNTPD, 2, {MEMORY|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9797, IF_AVX|IF_SANDYBRIDGE},
    /* 2505 */ {I_VMOVNTPS, 2, {MEMORY|BITS128,XMMREG,0,0,0}, nasm_bytecodes+9804, IF_AVX|IF_SANDYBRIDGE},
    /* 2506 */ {I_VMOVNTPS, 2, {MEMORY|BITS128,YMMREG,0,0,0}, nasm_bytecodes+9811, IF_AVX|IF_SANDYBRIDGE},
    /* 2507 */ {I_VMOVSD, 3, {XMMREG,XMMREG,XMMREG,0,0}, nasm_bytecodes+9818, IF_AVX|IF_SANDYBRIDGE},
    /* 2508 */ {I_VMOVSD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+9825, IF_AVX|IF_SANDYBRIDGE},
    /* 2509 */ {I_VMOVSD, 2, {XMMREG,MEMORY|BITS64,0,0,0}, nasm_bytecodes+9832, IF_AVX|IF_SANDYBRIDGE},
    /* 2510 */ {I_VMOVSD, 3, {XMMREG,XMMREG,XMMREG,0,0}, nasm_bytecodes+9839, IF_AVX|IF_SANDYBRIDGE},
    /* 2511 */ {I_VMOVSD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+9846, IF_AVX|IF_SANDYBRIDGE},
    /* 2512 */ {I_VMOVSD, 2, {MEMORY|BITS64,XMMREG,0,0,0}, nasm_bytecodes+9853, IF_AVX|IF_SANDYBRIDGE},
    /* 2513 */ {I_VMOVSHDUP, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9860, IF_AVX|IF_SANDYBRIDGE},
    /* 2514 */ {I_VMOVSHDUP, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9867, IF_AVX|IF_SANDYBRIDGE},
    /* 2515 */ {I_VMOVSLDUP, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9874, IF_AVX|IF_SANDYBRIDGE},
    /* 2516 */ {I_VMOVSLDUP, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9881, IF_AVX|IF_SANDYBRIDGE},
    /* 2517 */ {I_VMOVSS, 3, {XMMREG,XMMREG,XMMREG,0,0}, nasm_bytecodes+9888, IF_AVX|IF_SANDYBRIDGE},
    /* 2518 */ {I_VMOVSS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+9895, IF_AVX|IF_SANDYBRIDGE},
    /* 2519 */ {I_VMOVSS, 2, {XMMREG,MEMORY|BITS64,0,0,0}, nasm_bytecodes+9902, IF_AVX|IF_SANDYBRIDGE},
    /* 2520 */ {I_VMOVSS, 3, {XMMREG,XMMREG,XMMREG,0,0}, nasm_bytecodes+9909, IF_AVX|IF_SANDYBRIDGE},
    /* 2521 */ {I_VMOVSS, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+9916, IF_AVX|IF_SANDYBRIDGE},
    /* 2522 */ {I_VMOVSS, 2, {MEMORY|BITS64,XMMREG,0,0,0}, nasm_bytecodes+9923, IF_AVX|IF_SANDYBRIDGE},
    /* 2523 */ {I_VMOVUPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9930, IF_AVX|IF_SANDYBRIDGE},
    /* 2524 */ {I_VMOVUPD, 2, {RM_XMM|BITS128,XMMREG,0,0,0}, nasm_bytecodes+9937, IF_AVX|IF_SANDYBRIDGE},
    /* 2525 */ {I_VMOVUPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9944, IF_AVX|IF_SANDYBRIDGE},
    /* 2526 */ {I_VMOVUPD, 2, {RM_YMM|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9951, IF_AVX|IF_SANDYBRIDGE},
    /* 2527 */ {I_VMOVUPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9958, IF_AVX|IF_SANDYBRIDGE},
    /* 2528 */ {I_VMOVUPS, 2, {RM_XMM|BITS128,XMMREG,0,0,0}, nasm_bytecodes+9965, IF_AVX|IF_SANDYBRIDGE},
    /* 2529 */ {I_VMOVUPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+9972, IF_AVX|IF_SANDYBRIDGE},
    /* 2530 */ {I_VMOVUPS, 2, {RM_YMM|BITS256,YMMREG,0,0,0}, nasm_bytecodes+9979, IF_AVX|IF_SANDYBRIDGE},
    /* 2531 */ {I_VMPSADBW, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6120, IF_AVX|IF_SANDYBRIDGE},
    /* 2532 */ {I_VMPSADBW, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6128, IF_AVX|IF_SANDYBRIDGE},
    /* 2533 */ {I_VMULPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+9986, IF_AVX|IF_SANDYBRIDGE},
    /* 2534 */ {I_VMULPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+9993, IF_AVX|IF_SANDYBRIDGE},
    /* 2535 */ {I_VMULPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+10000, IF_AVX|IF_SANDYBRIDGE},
    /* 2536 */ {I_VMULPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+10007, IF_AVX|IF_SANDYBRIDGE},
    /* 2537 */ {I_VMULPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10014, IF_AVX|IF_SANDYBRIDGE},
    /* 2538 */ {I_VMULPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10021, IF_AVX|IF_SANDYBRIDGE},
    /* 2539 */ {I_VMULPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+10028, IF_AVX|IF_SANDYBRIDGE},
    /* 2540 */ {I_VMULPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+10035, IF_AVX|IF_SANDYBRIDGE},
    /* 2541 */ {I_VMULSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+10042, IF_AVX|IF_SANDYBRIDGE},
    /* 2542 */ {I_VMULSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+10049, IF_AVX|IF_SANDYBRIDGE},
    /* 2543 */ {I_VMULSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+10056, IF_AVX|IF_SANDYBRIDGE},
    /* 2544 */ {I_VMULSS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+10063, IF_AVX|IF_SANDYBRIDGE},
    /* 2545 */ {I_VORPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10070, IF_AVX|IF_SANDYBRIDGE},
    /* 2546 */ {I_VORPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10077, IF_AVX|IF_SANDYBRIDGE},
    /* 2547 */ {I_VORPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+10084, IF_AVX|IF_SANDYBRIDGE},
    /* 2548 */ {I_VORPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+10091, IF_AVX|IF_SANDYBRIDGE},
    /* 2549 */ {I_VORPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10098, IF_AVX|IF_SANDYBRIDGE},
    /* 2550 */ {I_VORPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10105, IF_AVX|IF_SANDYBRIDGE},
    /* 2551 */ {I_VORPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+10112, IF_AVX|IF_SANDYBRIDGE},
    /* 2552 */ {I_VORPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+10119, IF_AVX|IF_SANDYBRIDGE},
    /* 2553 */ {I_VPABSB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10126, IF_AVX|IF_SANDYBRIDGE},
    /* 2554 */ {I_VPABSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10133, IF_AVX|IF_SANDYBRIDGE},
    /* 2555 */ {I_VPABSD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10140, IF_AVX|IF_SANDYBRIDGE},
    /* 2556 */ {I_VPACKSSWB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10147, IF_AVX|IF_SANDYBRIDGE},
    /* 2557 */ {I_VPACKSSWB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10154, IF_AVX|IF_SANDYBRIDGE},
    /* 2558 */ {I_VPACKSSDW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10161, IF_AVX|IF_SANDYBRIDGE},
    /* 2559 */ {I_VPACKSSDW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10168, IF_AVX|IF_SANDYBRIDGE},
    /* 2560 */ {I_VPACKUSWB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10175, IF_AVX|IF_SANDYBRIDGE},
    /* 2561 */ {I_VPACKUSWB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10182, IF_AVX|IF_SANDYBRIDGE},
    /* 2562 */ {I_VPACKUSDW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10189, IF_AVX|IF_SANDYBRIDGE},
    /* 2563 */ {I_VPACKUSDW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10196, IF_AVX|IF_SANDYBRIDGE},
    /* 2564 */ {I_VPADDB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10203, IF_AVX|IF_SANDYBRIDGE},
    /* 2565 */ {I_VPADDB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10210, IF_AVX|IF_SANDYBRIDGE},
    /* 2566 */ {I_VPADDW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10217, IF_AVX|IF_SANDYBRIDGE},
    /* 2567 */ {I_VPADDW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10224, IF_AVX|IF_SANDYBRIDGE},
    /* 2568 */ {I_VPADDD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10231, IF_AVX|IF_SANDYBRIDGE},
    /* 2569 */ {I_VPADDD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10238, IF_AVX|IF_SANDYBRIDGE},
    /* 2570 */ {I_VPADDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10245, IF_AVX|IF_SANDYBRIDGE},
    /* 2571 */ {I_VPADDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10252, IF_AVX|IF_SANDYBRIDGE},
    /* 2572 */ {I_VPADDSB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10259, IF_AVX|IF_SANDYBRIDGE},
    /* 2573 */ {I_VPADDSB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10266, IF_AVX|IF_SANDYBRIDGE},
    /* 2574 */ {I_VPADDSW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10273, IF_AVX|IF_SANDYBRIDGE},
    /* 2575 */ {I_VPADDSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10280, IF_AVX|IF_SANDYBRIDGE},
    /* 2576 */ {I_VPADDUSB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10287, IF_AVX|IF_SANDYBRIDGE},
    /* 2577 */ {I_VPADDUSB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10294, IF_AVX|IF_SANDYBRIDGE},
    /* 2578 */ {I_VPADDUSW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10301, IF_AVX|IF_SANDYBRIDGE},
    /* 2579 */ {I_VPADDUSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10308, IF_AVX|IF_SANDYBRIDGE},
    /* 2580 */ {I_VPALIGNR, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6136, IF_AVX|IF_SANDYBRIDGE},
    /* 2581 */ {I_VPALIGNR, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6144, IF_AVX|IF_SANDYBRIDGE},
    /* 2582 */ {I_VPAND, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10315, IF_AVX|IF_SANDYBRIDGE},
    /* 2583 */ {I_VPAND, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10322, IF_AVX|IF_SANDYBRIDGE},
    /* 2584 */ {I_VPANDN, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10329, IF_AVX|IF_SANDYBRIDGE},
    /* 2585 */ {I_VPANDN, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10336, IF_AVX|IF_SANDYBRIDGE},
    /* 2586 */ {I_VPAVGB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10343, IF_AVX|IF_SANDYBRIDGE},
    /* 2587 */ {I_VPAVGB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10350, IF_AVX|IF_SANDYBRIDGE},
    /* 2588 */ {I_VPAVGW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10357, IF_AVX|IF_SANDYBRIDGE},
    /* 2589 */ {I_VPAVGW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10364, IF_AVX|IF_SANDYBRIDGE},
    /* 2590 */ {I_VPBLENDVB, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+3582, IF_AVX|IF_SANDYBRIDGE},
    /* 2591 */ {I_VPBLENDVB, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+3591, IF_AVX|IF_SANDYBRIDGE},
    /* 2592 */ {I_VPBLENDW, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6152, IF_AVX|IF_SANDYBRIDGE},
    /* 2593 */ {I_VPBLENDW, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6160, IF_AVX|IF_SANDYBRIDGE},
    /* 2594 */ {I_VPCMPESTRI, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6168, IF_AVX|IF_SANDYBRIDGE},
    /* 2595 */ {I_VPCMPESTRM, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6176, IF_AVX|IF_SANDYBRIDGE},
    /* 2596 */ {I_VPCMPISTRI, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6184, IF_AVX|IF_SANDYBRIDGE},
    /* 2597 */ {I_VPCMPISTRM, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6192, IF_AVX|IF_SANDYBRIDGE},
    /* 2598 */ {I_VPCMPEQB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10371, IF_AVX|IF_SANDYBRIDGE},
    /* 2599 */ {I_VPCMPEQB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10378, IF_AVX|IF_SANDYBRIDGE},
    /* 2600 */ {I_VPCMPEQW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10385, IF_AVX|IF_SANDYBRIDGE},
    /* 2601 */ {I_VPCMPEQW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10392, IF_AVX|IF_SANDYBRIDGE},
    /* 2602 */ {I_VPCMPEQD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10399, IF_AVX|IF_SANDYBRIDGE},
    /* 2603 */ {I_VPCMPEQD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10406, IF_AVX|IF_SANDYBRIDGE},
    /* 2604 */ {I_VPCMPEQQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10413, IF_AVX|IF_SANDYBRIDGE},
    /* 2605 */ {I_VPCMPEQQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10420, IF_AVX|IF_SANDYBRIDGE},
    /* 2606 */ {I_VPCMPGTB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10427, IF_AVX|IF_SANDYBRIDGE},
    /* 2607 */ {I_VPCMPGTB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10434, IF_AVX|IF_SANDYBRIDGE},
    /* 2608 */ {I_VPCMPGTW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10441, IF_AVX|IF_SANDYBRIDGE},
    /* 2609 */ {I_VPCMPGTW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10448, IF_AVX|IF_SANDYBRIDGE},
    /* 2610 */ {I_VPCMPGTD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10455, IF_AVX|IF_SANDYBRIDGE},
    /* 2611 */ {I_VPCMPGTD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10462, IF_AVX|IF_SANDYBRIDGE},
    /* 2612 */ {I_VPCMPGTQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10469, IF_AVX|IF_SANDYBRIDGE},
    /* 2613 */ {I_VPCMPGTQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10476, IF_AVX|IF_SANDYBRIDGE},
    /* 2614 */ {I_VPERMILPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10483, IF_AVX|IF_SANDYBRIDGE},
    /* 2615 */ {I_VPERMILPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+10490, IF_AVX|IF_SANDYBRIDGE},
    /* 2616 */ {I_VPERMILPD, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6200, IF_AVX|IF_SANDYBRIDGE},
    /* 2617 */ {I_VPERMILPD, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6208, IF_AVX|IF_SANDYBRIDGE},
    /* 2618 */ {I_VPERMILPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10497, IF_AVX|IF_SANDYBRIDGE},
    /* 2619 */ {I_VPERMILPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+10504, IF_AVX|IF_SANDYBRIDGE},
    /* 2620 */ {I_VPERMILPS, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6216, IF_AVX|IF_SANDYBRIDGE},
    /* 2621 */ {I_VPERMILPS, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6224, IF_AVX|IF_SANDYBRIDGE},
    /* 2622 */ {I_VPERM2F128, 4, {YMMREG,YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0}, nasm_bytecodes+6232, IF_AVX|IF_SANDYBRIDGE},
    /* 2623 */ {I_VPEXTRB, 3, {REG_GPR|BITS64,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6240, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2624 */ {I_VPEXTRB, 3, {REG_GPR|BITS32,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6240, IF_AVX|IF_SANDYBRIDGE},
    /* 2625 */ {I_VPEXTRB, 3, {MEMORY|BITS8,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6240, IF_AVX|IF_SANDYBRIDGE},
    /* 2626 */ {I_VPEXTRW, 3, {REG_GPR|BITS64,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6248, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2627 */ {I_VPEXTRW, 3, {REG_GPR|BITS32,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6248, IF_AVX|IF_SANDYBRIDGE},
    /* 2628 */ {I_VPEXTRW, 3, {REG_GPR|BITS64,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6256, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2629 */ {I_VPEXTRW, 3, {REG_GPR|BITS32,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6256, IF_AVX|IF_SANDYBRIDGE},
    /* 2630 */ {I_VPEXTRW, 3, {MEMORY|BITS16,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6256, IF_AVX|IF_SANDYBRIDGE},
    /* 2631 */ {I_VPEXTRD, 3, {REG_GPR|BITS64,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6264, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2632 */ {I_VPEXTRD, 3, {RM_GPR|BITS32,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6264, IF_AVX|IF_SANDYBRIDGE},
    /* 2633 */ {I_VPEXTRQ, 3, {RM_GPR|BITS64,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6272, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2634 */ {I_VPHADDW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10511, IF_AVX|IF_SANDYBRIDGE},
    /* 2635 */ {I_VPHADDW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10518, IF_AVX|IF_SANDYBRIDGE},
    /* 2636 */ {I_VPHADDD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10525, IF_AVX|IF_SANDYBRIDGE},
    /* 2637 */ {I_VPHADDD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10532, IF_AVX|IF_SANDYBRIDGE},
    /* 2638 */ {I_VPHADDSW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10539, IF_AVX|IF_SANDYBRIDGE},
    /* 2639 */ {I_VPHADDSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10546, IF_AVX|IF_SANDYBRIDGE},
    /* 2640 */ {I_VPHMINPOSUW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10553, IF_AVX|IF_SANDYBRIDGE},
    /* 2641 */ {I_VPHSUBW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10560, IF_AVX|IF_SANDYBRIDGE},
    /* 2642 */ {I_VPHSUBW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10567, IF_AVX|IF_SANDYBRIDGE},
    /* 2643 */ {I_VPHSUBD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10574, IF_AVX|IF_SANDYBRIDGE},
    /* 2644 */ {I_VPHSUBD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10581, IF_AVX|IF_SANDYBRIDGE},
    /* 2645 */ {I_VPHSUBSW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10588, IF_AVX|IF_SANDYBRIDGE},
    /* 2646 */ {I_VPHSUBSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10595, IF_AVX|IF_SANDYBRIDGE},
    /* 2647 */ {I_VPINSRB, 4, {XMMREG,XMMREG,MEMORY|BITS8,IMMEDIATE|BITS8,0}, nasm_bytecodes+6280, IF_AVX|IF_SANDYBRIDGE},
    /* 2648 */ {I_VPINSRB, 3, {XMMREG,MEMORY|BITS8,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6288, IF_AVX|IF_SANDYBRIDGE},
    /* 2649 */ {I_VPINSRB, 4, {XMMREG,XMMREG,RM_GPR|BITS8,IMMEDIATE|BITS8,0}, nasm_bytecodes+6280, IF_AVX|IF_SANDYBRIDGE},
    /* 2650 */ {I_VPINSRB, 3, {XMMREG,RM_GPR|BITS8,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6288, IF_AVX|IF_SANDYBRIDGE},
    /* 2651 */ {I_VPINSRB, 4, {XMMREG,XMMREG,REG_GPR|BITS32,IMMEDIATE|BITS8,0}, nasm_bytecodes+6280, IF_AVX|IF_SANDYBRIDGE},
    /* 2652 */ {I_VPINSRB, 3, {XMMREG,REG_GPR|BITS32,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6288, IF_AVX|IF_SANDYBRIDGE},
    /* 2653 */ {I_VPINSRW, 4, {XMMREG,XMMREG,MEMORY|BITS16,IMMEDIATE|BITS8,0}, nasm_bytecodes+6296, IF_AVX|IF_SANDYBRIDGE},
    /* 2654 */ {I_VPINSRW, 3, {XMMREG,MEMORY|BITS16,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6304, IF_AVX|IF_SANDYBRIDGE},
    /* 2655 */ {I_VPINSRW, 4, {XMMREG,XMMREG,RM_GPR|BITS16,IMMEDIATE|BITS8,0}, nasm_bytecodes+6296, IF_AVX|IF_SANDYBRIDGE},
    /* 2656 */ {I_VPINSRW, 3, {XMMREG,RM_GPR|BITS16,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6304, IF_AVX|IF_SANDYBRIDGE},
    /* 2657 */ {I_VPINSRW, 4, {XMMREG,XMMREG,REG_GPR|BITS32,IMMEDIATE|BITS8,0}, nasm_bytecodes+6296, IF_AVX|IF_SANDYBRIDGE},
    /* 2658 */ {I_VPINSRW, 3, {XMMREG,REG_GPR|BITS32,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6304, IF_AVX|IF_SANDYBRIDGE},
    /* 2659 */ {I_VPINSRD, 4, {XMMREG,XMMREG,MEMORY|BITS32,IMMEDIATE|BITS8,0}, nasm_bytecodes+6312, IF_AVX|IF_SANDYBRIDGE},
    /* 2660 */ {I_VPINSRD, 3, {XMMREG,MEMORY|BITS32,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6320, IF_AVX|IF_SANDYBRIDGE},
    /* 2661 */ {I_VPINSRD, 4, {XMMREG,XMMREG,RM_GPR|BITS32,IMMEDIATE|BITS8,0}, nasm_bytecodes+6312, IF_AVX|IF_SANDYBRIDGE},
    /* 2662 */ {I_VPINSRD, 3, {XMMREG,RM_GPR|BITS32,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6320, IF_AVX|IF_SANDYBRIDGE},
    /* 2663 */ {I_VPINSRQ, 4, {XMMREG,XMMREG,MEMORY|BITS64,IMMEDIATE|BITS8,0}, nasm_bytecodes+6328, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2664 */ {I_VPINSRQ, 3, {XMMREG,MEMORY|BITS64,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6336, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2665 */ {I_VPINSRQ, 4, {XMMREG,XMMREG,RM_GPR|BITS64,IMMEDIATE|BITS8,0}, nasm_bytecodes+6328, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2666 */ {I_VPINSRQ, 3, {XMMREG,RM_GPR|BITS64,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6336, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2667 */ {I_VPMADDWD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10602, IF_AVX|IF_SANDYBRIDGE},
    /* 2668 */ {I_VPMADDWD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10609, IF_AVX|IF_SANDYBRIDGE},
    /* 2669 */ {I_VPMADDUBSW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10616, IF_AVX|IF_SANDYBRIDGE},
    /* 2670 */ {I_VPMADDUBSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10623, IF_AVX|IF_SANDYBRIDGE},
    /* 2671 */ {I_VPMAXSB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10630, IF_AVX|IF_SANDYBRIDGE},
    /* 2672 */ {I_VPMAXSB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10637, IF_AVX|IF_SANDYBRIDGE},
    /* 2673 */ {I_VPMAXSW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10644, IF_AVX|IF_SANDYBRIDGE},
    /* 2674 */ {I_VPMAXSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10651, IF_AVX|IF_SANDYBRIDGE},
    /* 2675 */ {I_VPMAXSD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10658, IF_AVX|IF_SANDYBRIDGE},
    /* 2676 */ {I_VPMAXSD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10665, IF_AVX|IF_SANDYBRIDGE},
    /* 2677 */ {I_VPMAXUB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10672, IF_AVX|IF_SANDYBRIDGE},
    /* 2678 */ {I_VPMAXUB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10679, IF_AVX|IF_SANDYBRIDGE},
    /* 2679 */ {I_VPMAXUW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10686, IF_AVX|IF_SANDYBRIDGE},
    /* 2680 */ {I_VPMAXUW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10693, IF_AVX|IF_SANDYBRIDGE},
    /* 2681 */ {I_VPMAXUD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10700, IF_AVX|IF_SANDYBRIDGE},
    /* 2682 */ {I_VPMAXUD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10707, IF_AVX|IF_SANDYBRIDGE},
    /* 2683 */ {I_VPMINSB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10714, IF_AVX|IF_SANDYBRIDGE},
    /* 2684 */ {I_VPMINSB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10721, IF_AVX|IF_SANDYBRIDGE},
    /* 2685 */ {I_VPMINSW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10728, IF_AVX|IF_SANDYBRIDGE},
    /* 2686 */ {I_VPMINSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10735, IF_AVX|IF_SANDYBRIDGE},
    /* 2687 */ {I_VPMINSD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10742, IF_AVX|IF_SANDYBRIDGE},
    /* 2688 */ {I_VPMINSD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10749, IF_AVX|IF_SANDYBRIDGE},
    /* 2689 */ {I_VPMINUB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10756, IF_AVX|IF_SANDYBRIDGE},
    /* 2690 */ {I_VPMINUB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10763, IF_AVX|IF_SANDYBRIDGE},
    /* 2691 */ {I_VPMINUW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10770, IF_AVX|IF_SANDYBRIDGE},
    /* 2692 */ {I_VPMINUW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10777, IF_AVX|IF_SANDYBRIDGE},
    /* 2693 */ {I_VPMINUD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10784, IF_AVX|IF_SANDYBRIDGE},
    /* 2694 */ {I_VPMINUD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10791, IF_AVX|IF_SANDYBRIDGE},
    /* 2695 */ {I_VPMOVMSKB, 2, {REG_GPR|BITS64,XMMREG,0,0,0}, nasm_bytecodes+10798, IF_AVX|IF_SANDYBRIDGE|IF_LONG},
    /* 2696 */ {I_VPMOVMSKB, 2, {REG_GPR|BITS32,XMMREG,0,0,0}, nasm_bytecodes+10798, IF_AVX|IF_SANDYBRIDGE},
    /* 2697 */ {I_VPMOVSXBW, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+10805, IF_AVX|IF_SANDYBRIDGE},
    /* 2698 */ {I_VPMOVSXBD, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+10812, IF_AVX|IF_SANDYBRIDGE},
    /* 2699 */ {I_VPMOVSXBQ, 2, {XMMREG,RM_XMM|BITS16,0,0,0}, nasm_bytecodes+10819, IF_AVX|IF_SANDYBRIDGE},
    /* 2700 */ {I_VPMOVSXWD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+10826, IF_AVX|IF_SANDYBRIDGE},
    /* 2701 */ {I_VPMOVSXWQ, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+10833, IF_AVX|IF_SANDYBRIDGE},
    /* 2702 */ {I_VPMOVSXDQ, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+10840, IF_AVX|IF_SANDYBRIDGE},
    /* 2703 */ {I_VPMOVZXBW, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+10847, IF_AVX|IF_SANDYBRIDGE},
    /* 2704 */ {I_VPMOVZXBD, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+10854, IF_AVX|IF_SANDYBRIDGE},
    /* 2705 */ {I_VPMOVZXBQ, 2, {XMMREG,RM_XMM|BITS16,0,0,0}, nasm_bytecodes+10861, IF_AVX|IF_SANDYBRIDGE},
    /* 2706 */ {I_VPMOVZXWD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+10868, IF_AVX|IF_SANDYBRIDGE},
    /* 2707 */ {I_VPMOVZXWQ, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+10875, IF_AVX|IF_SANDYBRIDGE},
    /* 2708 */ {I_VPMOVZXDQ, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+10882, IF_AVX|IF_SANDYBRIDGE},
    /* 2709 */ {I_VPMULHUW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10889, IF_AVX|IF_SANDYBRIDGE},
    /* 2710 */ {I_VPMULHUW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10896, IF_AVX|IF_SANDYBRIDGE},
    /* 2711 */ {I_VPMULHRSW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10903, IF_AVX|IF_SANDYBRIDGE},
    /* 2712 */ {I_VPMULHRSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10910, IF_AVX|IF_SANDYBRIDGE},
    /* 2713 */ {I_VPMULHW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10917, IF_AVX|IF_SANDYBRIDGE},
    /* 2714 */ {I_VPMULHW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10924, IF_AVX|IF_SANDYBRIDGE},
    /* 2715 */ {I_VPMULLW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10931, IF_AVX|IF_SANDYBRIDGE},
    /* 2716 */ {I_VPMULLW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10938, IF_AVX|IF_SANDYBRIDGE},
    /* 2717 */ {I_VPMULLD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10945, IF_AVX|IF_SANDYBRIDGE},
    /* 2718 */ {I_VPMULLD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10952, IF_AVX|IF_SANDYBRIDGE},
    /* 2719 */ {I_VPMULUDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10959, IF_AVX|IF_SANDYBRIDGE},
    /* 2720 */ {I_VPMULUDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10966, IF_AVX|IF_SANDYBRIDGE},
    /* 2721 */ {I_VPMULDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10973, IF_AVX|IF_SANDYBRIDGE},
    /* 2722 */ {I_VPMULDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10980, IF_AVX|IF_SANDYBRIDGE},
    /* 2723 */ {I_VPOR, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+10987, IF_AVX|IF_SANDYBRIDGE},
    /* 2724 */ {I_VPOR, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+10994, IF_AVX|IF_SANDYBRIDGE},
    /* 2725 */ {I_VPSADBW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11001, IF_AVX|IF_SANDYBRIDGE},
    /* 2726 */ {I_VPSADBW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11008, IF_AVX|IF_SANDYBRIDGE},
    /* 2727 */ {I_VPSHUFB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11015, IF_AVX|IF_SANDYBRIDGE},
    /* 2728 */ {I_VPSHUFB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11022, IF_AVX|IF_SANDYBRIDGE},
    /* 2729 */ {I_VPSHUFD, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6344, IF_AVX|IF_SANDYBRIDGE},
    /* 2730 */ {I_VPSHUFHW, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6352, IF_AVX|IF_SANDYBRIDGE},
    /* 2731 */ {I_VPSHUFLW, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6360, IF_AVX|IF_SANDYBRIDGE},
    /* 2732 */ {I_VPSIGNB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11029, IF_AVX|IF_SANDYBRIDGE},
    /* 2733 */ {I_VPSIGNB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11036, IF_AVX|IF_SANDYBRIDGE},
    /* 2734 */ {I_VPSIGNW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11043, IF_AVX|IF_SANDYBRIDGE},
    /* 2735 */ {I_VPSIGNW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11050, IF_AVX|IF_SANDYBRIDGE},
    /* 2736 */ {I_VPSIGND, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11057, IF_AVX|IF_SANDYBRIDGE},
    /* 2737 */ {I_VPSIGND, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11064, IF_AVX|IF_SANDYBRIDGE},
    /* 2738 */ {I_VPSLLDQ, 3, {XMMREG,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6368, IF_AVX|IF_SANDYBRIDGE},
    /* 2739 */ {I_VPSLLDQ, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6376, IF_AVX|IF_SANDYBRIDGE},
    /* 2740 */ {I_VPSRLDQ, 3, {XMMREG,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6384, IF_AVX|IF_SANDYBRIDGE},
    /* 2741 */ {I_VPSRLDQ, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6392, IF_AVX|IF_SANDYBRIDGE},
    /* 2742 */ {I_VPSLLW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11071, IF_AVX|IF_SANDYBRIDGE},
    /* 2743 */ {I_VPSLLW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11078, IF_AVX|IF_SANDYBRIDGE},
    /* 2744 */ {I_VPSLLW, 3, {XMMREG,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6400, IF_AVX|IF_SANDYBRIDGE},
    /* 2745 */ {I_VPSLLW, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6408, IF_AVX|IF_SANDYBRIDGE},
    /* 2746 */ {I_VPSLLD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11085, IF_AVX|IF_SANDYBRIDGE},
    /* 2747 */ {I_VPSLLD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11092, IF_AVX|IF_SANDYBRIDGE},
    /* 2748 */ {I_VPSLLD, 3, {XMMREG,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6416, IF_AVX|IF_SANDYBRIDGE},
    /* 2749 */ {I_VPSLLD, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6424, IF_AVX|IF_SANDYBRIDGE},
    /* 2750 */ {I_VPSLLQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11099, IF_AVX|IF_SANDYBRIDGE},
    /* 2751 */ {I_VPSLLQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11106, IF_AVX|IF_SANDYBRIDGE},
    /* 2752 */ {I_VPSLLQ, 3, {XMMREG,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6432, IF_AVX|IF_SANDYBRIDGE},
    /* 2753 */ {I_VPSLLQ, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6440, IF_AVX|IF_SANDYBRIDGE},
    /* 2754 */ {I_VPSRAW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11113, IF_AVX|IF_SANDYBRIDGE},
    /* 2755 */ {I_VPSRAW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11120, IF_AVX|IF_SANDYBRIDGE},
    /* 2756 */ {I_VPSRAW, 3, {XMMREG,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6448, IF_AVX|IF_SANDYBRIDGE},
    /* 2757 */ {I_VPSRAW, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6456, IF_AVX|IF_SANDYBRIDGE},
    /* 2758 */ {I_VPSRAD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11127, IF_AVX|IF_SANDYBRIDGE},
    /* 2759 */ {I_VPSRAD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11134, IF_AVX|IF_SANDYBRIDGE},
    /* 2760 */ {I_VPSRAD, 3, {XMMREG,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6464, IF_AVX|IF_SANDYBRIDGE},
    /* 2761 */ {I_VPSRAD, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6472, IF_AVX|IF_SANDYBRIDGE},
    /* 2762 */ {I_VPSRLW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11141, IF_AVX|IF_SANDYBRIDGE},
    /* 2763 */ {I_VPSRLW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11148, IF_AVX|IF_SANDYBRIDGE},
    /* 2764 */ {I_VPSRLW, 3, {XMMREG,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6480, IF_AVX|IF_SANDYBRIDGE},
    /* 2765 */ {I_VPSRLW, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6488, IF_AVX|IF_SANDYBRIDGE},
    /* 2766 */ {I_VPSRLD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11155, IF_AVX|IF_SANDYBRIDGE},
    /* 2767 */ {I_VPSRLD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11162, IF_AVX|IF_SANDYBRIDGE},
    /* 2768 */ {I_VPSRLD, 3, {XMMREG,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6496, IF_AVX|IF_SANDYBRIDGE},
    /* 2769 */ {I_VPSRLD, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6504, IF_AVX|IF_SANDYBRIDGE},
    /* 2770 */ {I_VPSRLQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11169, IF_AVX|IF_SANDYBRIDGE},
    /* 2771 */ {I_VPSRLQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11176, IF_AVX|IF_SANDYBRIDGE},
    /* 2772 */ {I_VPSRLQ, 3, {XMMREG,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6512, IF_AVX|IF_SANDYBRIDGE},
    /* 2773 */ {I_VPSRLQ, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6520, IF_AVX|IF_SANDYBRIDGE},
    /* 2774 */ {I_VPTEST, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11183, IF_AVX|IF_SANDYBRIDGE},
    /* 2775 */ {I_VPTEST, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11190, IF_AVX|IF_SANDYBRIDGE},
    /* 2776 */ {I_VPSUBB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11197, IF_AVX|IF_SANDYBRIDGE},
    /* 2777 */ {I_VPSUBB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11204, IF_AVX|IF_SANDYBRIDGE},
    /* 2778 */ {I_VPSUBW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11211, IF_AVX|IF_SANDYBRIDGE},
    /* 2779 */ {I_VPSUBW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11218, IF_AVX|IF_SANDYBRIDGE},
    /* 2780 */ {I_VPSUBD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11225, IF_AVX|IF_SANDYBRIDGE},
    /* 2781 */ {I_VPSUBD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11232, IF_AVX|IF_SANDYBRIDGE},
    /* 2782 */ {I_VPSUBQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11239, IF_AVX|IF_SANDYBRIDGE},
    /* 2783 */ {I_VPSUBQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11246, IF_AVX|IF_SANDYBRIDGE},
    /* 2784 */ {I_VPSUBSB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11253, IF_AVX|IF_SANDYBRIDGE},
    /* 2785 */ {I_VPSUBSB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11260, IF_AVX|IF_SANDYBRIDGE},
    /* 2786 */ {I_VPSUBSW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11267, IF_AVX|IF_SANDYBRIDGE},
    /* 2787 */ {I_VPSUBSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11274, IF_AVX|IF_SANDYBRIDGE},
    /* 2788 */ {I_VPSUBUSB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11281, IF_AVX|IF_SANDYBRIDGE},
    /* 2789 */ {I_VPSUBUSB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11288, IF_AVX|IF_SANDYBRIDGE},
    /* 2790 */ {I_VPSUBUSW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11295, IF_AVX|IF_SANDYBRIDGE},
    /* 2791 */ {I_VPSUBUSW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11302, IF_AVX|IF_SANDYBRIDGE},
    /* 2792 */ {I_VPUNPCKHBW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11309, IF_AVX|IF_SANDYBRIDGE},
    /* 2793 */ {I_VPUNPCKHBW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11316, IF_AVX|IF_SANDYBRIDGE},
    /* 2794 */ {I_VPUNPCKHWD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11323, IF_AVX|IF_SANDYBRIDGE},
    /* 2795 */ {I_VPUNPCKHWD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11330, IF_AVX|IF_SANDYBRIDGE},
    /* 2796 */ {I_VPUNPCKHDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11337, IF_AVX|IF_SANDYBRIDGE},
    /* 2797 */ {I_VPUNPCKHDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11344, IF_AVX|IF_SANDYBRIDGE},
    /* 2798 */ {I_VPUNPCKHQDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11351, IF_AVX|IF_SANDYBRIDGE},
    /* 2799 */ {I_VPUNPCKHQDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11358, IF_AVX|IF_SANDYBRIDGE},
    /* 2800 */ {I_VPUNPCKLBW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11365, IF_AVX|IF_SANDYBRIDGE},
    /* 2801 */ {I_VPUNPCKLBW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11372, IF_AVX|IF_SANDYBRIDGE},
    /* 2802 */ {I_VPUNPCKLWD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11379, IF_AVX|IF_SANDYBRIDGE},
    /* 2803 */ {I_VPUNPCKLWD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11386, IF_AVX|IF_SANDYBRIDGE},
    /* 2804 */ {I_VPUNPCKLDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11393, IF_AVX|IF_SANDYBRIDGE},
    /* 2805 */ {I_VPUNPCKLDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11400, IF_AVX|IF_SANDYBRIDGE},
    /* 2806 */ {I_VPUNPCKLQDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11407, IF_AVX|IF_SANDYBRIDGE},
    /* 2807 */ {I_VPUNPCKLQDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11414, IF_AVX|IF_SANDYBRIDGE},
    /* 2808 */ {I_VPXOR, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11421, IF_AVX|IF_SANDYBRIDGE},
    /* 2809 */ {I_VPXOR, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11428, IF_AVX|IF_SANDYBRIDGE},
    /* 2810 */ {I_VRCPPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11435, IF_AVX|IF_SANDYBRIDGE},
    /* 2811 */ {I_VRCPPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11442, IF_AVX|IF_SANDYBRIDGE},
    /* 2812 */ {I_VRCPSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+11449, IF_AVX|IF_SANDYBRIDGE},
    /* 2813 */ {I_VRCPSS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+11456, IF_AVX|IF_SANDYBRIDGE},
    /* 2814 */ {I_VRSQRTPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11463, IF_AVX|IF_SANDYBRIDGE},
    /* 2815 */ {I_VRSQRTPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11470, IF_AVX|IF_SANDYBRIDGE},
    /* 2816 */ {I_VRSQRTSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+11477, IF_AVX|IF_SANDYBRIDGE},
    /* 2817 */ {I_VRSQRTSS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+11484, IF_AVX|IF_SANDYBRIDGE},
    /* 2818 */ {I_VROUNDPD, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6528, IF_AVX|IF_SANDYBRIDGE},
    /* 2819 */ {I_VROUNDPD, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6536, IF_AVX|IF_SANDYBRIDGE},
    /* 2820 */ {I_VROUNDPS, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6544, IF_AVX|IF_SANDYBRIDGE},
    /* 2821 */ {I_VROUNDPS, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6552, IF_AVX|IF_SANDYBRIDGE},
    /* 2822 */ {I_VROUNDSD, 4, {XMMREG,XMMREG,RM_XMM|BITS64,IMMEDIATE|BITS8,0}, nasm_bytecodes+6560, IF_AVX|IF_SANDYBRIDGE},
    /* 2823 */ {I_VROUNDSD, 3, {XMMREG,RM_XMM|BITS64,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6568, IF_AVX|IF_SANDYBRIDGE},
    /* 2824 */ {I_VROUNDSS, 4, {XMMREG,XMMREG,RM_XMM|BITS32,IMMEDIATE|BITS8,0}, nasm_bytecodes+6576, IF_AVX|IF_SANDYBRIDGE},
    /* 2825 */ {I_VROUNDSS, 3, {XMMREG,RM_XMM|BITS32,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6584, IF_AVX|IF_SANDYBRIDGE},
    /* 2826 */ {I_VSHUFPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6592, IF_AVX|IF_SANDYBRIDGE},
    /* 2827 */ {I_VSHUFPD, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6600, IF_AVX|IF_SANDYBRIDGE},
    /* 2828 */ {I_VSHUFPD, 4, {YMMREG,YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0}, nasm_bytecodes+6608, IF_AVX|IF_SANDYBRIDGE},
    /* 2829 */ {I_VSHUFPD, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6616, IF_AVX|IF_SANDYBRIDGE},
    /* 2830 */ {I_VSHUFPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6624, IF_AVX|IF_SANDYBRIDGE},
    /* 2831 */ {I_VSHUFPS, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6632, IF_AVX|IF_SANDYBRIDGE},
    /* 2832 */ {I_VSHUFPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0}, nasm_bytecodes+6640, IF_AVX|IF_SANDYBRIDGE},
    /* 2833 */ {I_VSHUFPS, 3, {YMMREG,RM_YMM|BITS256,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6648, IF_AVX|IF_SANDYBRIDGE},
    /* 2834 */ {I_VSQRTPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11491, IF_AVX|IF_SANDYBRIDGE},
    /* 2835 */ {I_VSQRTPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11498, IF_AVX|IF_SANDYBRIDGE},
    /* 2836 */ {I_VSQRTPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11505, IF_AVX|IF_SANDYBRIDGE},
    /* 2837 */ {I_VSQRTPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11512, IF_AVX|IF_SANDYBRIDGE},
    /* 2838 */ {I_VSQRTSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+11519, IF_AVX|IF_SANDYBRIDGE},
    /* 2839 */ {I_VSQRTSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+11526, IF_AVX|IF_SANDYBRIDGE},
    /* 2840 */ {I_VSQRTSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+11533, IF_AVX|IF_SANDYBRIDGE},
    /* 2841 */ {I_VSQRTSS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+11540, IF_AVX|IF_SANDYBRIDGE},
    /* 2842 */ {I_VSTMXCSR, 1, {MEMORY|BITS32,0,0,0,0}, nasm_bytecodes+11547, IF_AVX|IF_SANDYBRIDGE},
    /* 2843 */ {I_VSUBPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11554, IF_AVX|IF_SANDYBRIDGE},
    /* 2844 */ {I_VSUBPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11561, IF_AVX|IF_SANDYBRIDGE},
    /* 2845 */ {I_VSUBPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11568, IF_AVX|IF_SANDYBRIDGE},
    /* 2846 */ {I_VSUBPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11575, IF_AVX|IF_SANDYBRIDGE},
    /* 2847 */ {I_VSUBPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11582, IF_AVX|IF_SANDYBRIDGE},
    /* 2848 */ {I_VSUBPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11589, IF_AVX|IF_SANDYBRIDGE},
    /* 2849 */ {I_VSUBPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11596, IF_AVX|IF_SANDYBRIDGE},
    /* 2850 */ {I_VSUBPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11603, IF_AVX|IF_SANDYBRIDGE},
    /* 2851 */ {I_VSUBSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+11610, IF_AVX|IF_SANDYBRIDGE},
    /* 2852 */ {I_VSUBSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+11617, IF_AVX|IF_SANDYBRIDGE},
    /* 2853 */ {I_VSUBSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+11624, IF_AVX|IF_SANDYBRIDGE},
    /* 2854 */ {I_VSUBSS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+11631, IF_AVX|IF_SANDYBRIDGE},
    /* 2855 */ {I_VTESTPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11638, IF_AVX|IF_SANDYBRIDGE},
    /* 2856 */ {I_VTESTPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11645, IF_AVX|IF_SANDYBRIDGE},
    /* 2857 */ {I_VTESTPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11652, IF_AVX|IF_SANDYBRIDGE},
    /* 2858 */ {I_VTESTPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11659, IF_AVX|IF_SANDYBRIDGE},
    /* 2859 */ {I_VUCOMISD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+11666, IF_AVX|IF_SANDYBRIDGE},
    /* 2860 */ {I_VUCOMISS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+11673, IF_AVX|IF_SANDYBRIDGE},
    /* 2861 */ {I_VUNPCKHPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11680, IF_AVX|IF_SANDYBRIDGE},
    /* 2862 */ {I_VUNPCKHPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11687, IF_AVX|IF_SANDYBRIDGE},
    /* 2863 */ {I_VUNPCKHPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11694, IF_AVX|IF_SANDYBRIDGE},
    /* 2864 */ {I_VUNPCKHPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11701, IF_AVX|IF_SANDYBRIDGE},
    /* 2865 */ {I_VUNPCKHPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11708, IF_AVX|IF_SANDYBRIDGE},
    /* 2866 */ {I_VUNPCKHPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11715, IF_AVX|IF_SANDYBRIDGE},
    /* 2867 */ {I_VUNPCKHPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11722, IF_AVX|IF_SANDYBRIDGE},
    /* 2868 */ {I_VUNPCKHPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11729, IF_AVX|IF_SANDYBRIDGE},
    /* 2869 */ {I_VUNPCKLPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11736, IF_AVX|IF_SANDYBRIDGE},
    /* 2870 */ {I_VUNPCKLPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11743, IF_AVX|IF_SANDYBRIDGE},
    /* 2871 */ {I_VUNPCKLPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11750, IF_AVX|IF_SANDYBRIDGE},
    /* 2872 */ {I_VUNPCKLPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11757, IF_AVX|IF_SANDYBRIDGE},
    /* 2873 */ {I_VUNPCKLPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11764, IF_AVX|IF_SANDYBRIDGE},
    /* 2874 */ {I_VUNPCKLPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11771, IF_AVX|IF_SANDYBRIDGE},
    /* 2875 */ {I_VUNPCKLPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11778, IF_AVX|IF_SANDYBRIDGE},
    /* 2876 */ {I_VUNPCKLPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11785, IF_AVX|IF_SANDYBRIDGE},
    /* 2877 */ {I_VXORPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11792, IF_AVX|IF_SANDYBRIDGE},
    /* 2878 */ {I_VXORPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11799, IF_AVX|IF_SANDYBRIDGE},
    /* 2879 */ {I_VXORPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11806, IF_AVX|IF_SANDYBRIDGE},
    /* 2880 */ {I_VXORPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11813, IF_AVX|IF_SANDYBRIDGE},
    /* 2881 */ {I_VXORPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11820, IF_AVX|IF_SANDYBRIDGE},
    /* 2882 */ {I_VXORPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+11827, IF_AVX|IF_SANDYBRIDGE},
    /* 2883 */ {I_VXORPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11834, IF_AVX|IF_SANDYBRIDGE},
    /* 2884 */ {I_VXORPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+11841, IF_AVX|IF_SANDYBRIDGE},
    /* 2885 */ {I_VZEROALL, 0, {0,0,0,0,0}, nasm_bytecodes+15560, IF_AVX|IF_SANDYBRIDGE},
    /* 2886 */ {I_VZEROUPPER, 0, {0,0,0,0,0}, nasm_bytecodes+15566, IF_AVX|IF_SANDYBRIDGE},
    /* 2887 */ {I_PCLMULLQLQDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+3600, IF_SSE|IF_WESTMERE},
    /* 2888 */ {I_PCLMULHQLQDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+3609, IF_SSE|IF_WESTMERE},
    /* 2889 */ {I_PCLMULLQHQDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+3618, IF_SSE|IF_WESTMERE},
    /* 2890 */ {I_PCLMULHQHQDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+3627, IF_SSE|IF_WESTMERE},
    /* 2891 */ {I_PCLMULQDQ, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6656, IF_SSE|IF_WESTMERE},
    /* 2892 */ {I_VPCLMULLQLQDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+3636, IF_AVX|IF_SANDYBRIDGE},
    /* 2893 */ {I_VPCLMULLQLQDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+3645, IF_AVX|IF_SANDYBRIDGE},
    /* 2894 */ {I_VPCLMULHQLQDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+3654, IF_AVX|IF_SANDYBRIDGE},
    /* 2895 */ {I_VPCLMULHQLQDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+3663, IF_AVX|IF_SANDYBRIDGE},
    /* 2896 */ {I_VPCLMULLQHQDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+3672, IF_AVX|IF_SANDYBRIDGE},
    /* 2897 */ {I_VPCLMULLQHQDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+3681, IF_AVX|IF_SANDYBRIDGE},
    /* 2898 */ {I_VPCLMULHQHQDQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+3690, IF_AVX|IF_SANDYBRIDGE},
    /* 2899 */ {I_VPCLMULHQHQDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+3699, IF_AVX|IF_SANDYBRIDGE},
    /* 2900 */ {I_VPCLMULQDQ, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6664, IF_AVX|IF_SANDYBRIDGE},
    /* 2901 */ {I_VPCLMULQDQ, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6672, IF_AVX|IF_SANDYBRIDGE},
    /* 2902 */ {I_VFMADD132PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11848, IF_FMA|IF_FUTURE},
    /* 2903 */ {I_VFMADD132PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11855, IF_FMA|IF_FUTURE},
    /* 2904 */ {I_VFMADD132PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11862, IF_FMA|IF_FUTURE},
    /* 2905 */ {I_VFMADD132PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11869, IF_FMA|IF_FUTURE},
    /* 2906 */ {I_VFMADD312PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11848, IF_FMA|IF_FUTURE},
    /* 2907 */ {I_VFMADD312PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11855, IF_FMA|IF_FUTURE},
    /* 2908 */ {I_VFMADD312PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11862, IF_FMA|IF_FUTURE},
    /* 2909 */ {I_VFMADD312PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11869, IF_FMA|IF_FUTURE},
    /* 2910 */ {I_VFMADD213PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11876, IF_FMA|IF_FUTURE},
    /* 2911 */ {I_VFMADD213PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11883, IF_FMA|IF_FUTURE},
    /* 2912 */ {I_VFMADD213PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11890, IF_FMA|IF_FUTURE},
    /* 2913 */ {I_VFMADD213PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11897, IF_FMA|IF_FUTURE},
    /* 2914 */ {I_VFMADD123PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11876, IF_FMA|IF_FUTURE},
    /* 2915 */ {I_VFMADD123PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11883, IF_FMA|IF_FUTURE},
    /* 2916 */ {I_VFMADD123PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11890, IF_FMA|IF_FUTURE},
    /* 2917 */ {I_VFMADD123PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11897, IF_FMA|IF_FUTURE},
    /* 2918 */ {I_VFMADD231PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11904, IF_FMA|IF_FUTURE},
    /* 2919 */ {I_VFMADD231PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11911, IF_FMA|IF_FUTURE},
    /* 2920 */ {I_VFMADD231PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11918, IF_FMA|IF_FUTURE},
    /* 2921 */ {I_VFMADD231PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11925, IF_FMA|IF_FUTURE},
    /* 2922 */ {I_VFMADD321PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11904, IF_FMA|IF_FUTURE},
    /* 2923 */ {I_VFMADD321PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11911, IF_FMA|IF_FUTURE},
    /* 2924 */ {I_VFMADD321PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11918, IF_FMA|IF_FUTURE},
    /* 2925 */ {I_VFMADD321PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11925, IF_FMA|IF_FUTURE},
    /* 2926 */ {I_VFMADDSUB132PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11932, IF_FMA|IF_FUTURE},
    /* 2927 */ {I_VFMADDSUB132PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11939, IF_FMA|IF_FUTURE},
    /* 2928 */ {I_VFMADDSUB132PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11946, IF_FMA|IF_FUTURE},
    /* 2929 */ {I_VFMADDSUB132PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11953, IF_FMA|IF_FUTURE},
    /* 2930 */ {I_VFMADDSUB312PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11932, IF_FMA|IF_FUTURE},
    /* 2931 */ {I_VFMADDSUB312PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11939, IF_FMA|IF_FUTURE},
    /* 2932 */ {I_VFMADDSUB312PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11946, IF_FMA|IF_FUTURE},
    /* 2933 */ {I_VFMADDSUB312PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11953, IF_FMA|IF_FUTURE},
    /* 2934 */ {I_VFMADDSUB213PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11960, IF_FMA|IF_FUTURE},
    /* 2935 */ {I_VFMADDSUB213PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11967, IF_FMA|IF_FUTURE},
    /* 2936 */ {I_VFMADDSUB213PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11974, IF_FMA|IF_FUTURE},
    /* 2937 */ {I_VFMADDSUB213PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11981, IF_FMA|IF_FUTURE},
    /* 2938 */ {I_VFMADDSUB123PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11960, IF_FMA|IF_FUTURE},
    /* 2939 */ {I_VFMADDSUB123PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11967, IF_FMA|IF_FUTURE},
    /* 2940 */ {I_VFMADDSUB123PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11974, IF_FMA|IF_FUTURE},
    /* 2941 */ {I_VFMADDSUB123PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11981, IF_FMA|IF_FUTURE},
    /* 2942 */ {I_VFMADDSUB231PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11988, IF_FMA|IF_FUTURE},
    /* 2943 */ {I_VFMADDSUB231PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11995, IF_FMA|IF_FUTURE},
    /* 2944 */ {I_VFMADDSUB231PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12002, IF_FMA|IF_FUTURE},
    /* 2945 */ {I_VFMADDSUB231PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12009, IF_FMA|IF_FUTURE},
    /* 2946 */ {I_VFMADDSUB321PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+11988, IF_FMA|IF_FUTURE},
    /* 2947 */ {I_VFMADDSUB321PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+11995, IF_FMA|IF_FUTURE},
    /* 2948 */ {I_VFMADDSUB321PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12002, IF_FMA|IF_FUTURE},
    /* 2949 */ {I_VFMADDSUB321PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12009, IF_FMA|IF_FUTURE},
    /* 2950 */ {I_VFMSUB132PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12016, IF_FMA|IF_FUTURE},
    /* 2951 */ {I_VFMSUB132PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12023, IF_FMA|IF_FUTURE},
    /* 2952 */ {I_VFMSUB132PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12030, IF_FMA|IF_FUTURE},
    /* 2953 */ {I_VFMSUB132PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12037, IF_FMA|IF_FUTURE},
    /* 2954 */ {I_VFMSUB312PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12016, IF_FMA|IF_FUTURE},
    /* 2955 */ {I_VFMSUB312PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12023, IF_FMA|IF_FUTURE},
    /* 2956 */ {I_VFMSUB312PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12030, IF_FMA|IF_FUTURE},
    /* 2957 */ {I_VFMSUB312PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12037, IF_FMA|IF_FUTURE},
    /* 2958 */ {I_VFMSUB213PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12044, IF_FMA|IF_FUTURE},
    /* 2959 */ {I_VFMSUB213PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12051, IF_FMA|IF_FUTURE},
    /* 2960 */ {I_VFMSUB213PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12058, IF_FMA|IF_FUTURE},
    /* 2961 */ {I_VFMSUB213PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12065, IF_FMA|IF_FUTURE},
    /* 2962 */ {I_VFMSUB123PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12044, IF_FMA|IF_FUTURE},
    /* 2963 */ {I_VFMSUB123PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12051, IF_FMA|IF_FUTURE},
    /* 2964 */ {I_VFMSUB123PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12058, IF_FMA|IF_FUTURE},
    /* 2965 */ {I_VFMSUB123PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12065, IF_FMA|IF_FUTURE},
    /* 2966 */ {I_VFMSUB231PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12072, IF_FMA|IF_FUTURE},
    /* 2967 */ {I_VFMSUB231PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12079, IF_FMA|IF_FUTURE},
    /* 2968 */ {I_VFMSUB231PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12086, IF_FMA|IF_FUTURE},
    /* 2969 */ {I_VFMSUB231PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12093, IF_FMA|IF_FUTURE},
    /* 2970 */ {I_VFMSUB321PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12072, IF_FMA|IF_FUTURE},
    /* 2971 */ {I_VFMSUB321PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12079, IF_FMA|IF_FUTURE},
    /* 2972 */ {I_VFMSUB321PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12086, IF_FMA|IF_FUTURE},
    /* 2973 */ {I_VFMSUB321PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12093, IF_FMA|IF_FUTURE},
    /* 2974 */ {I_VFMSUBADD132PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12100, IF_FMA|IF_FUTURE},
    /* 2975 */ {I_VFMSUBADD132PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12107, IF_FMA|IF_FUTURE},
    /* 2976 */ {I_VFMSUBADD132PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12114, IF_FMA|IF_FUTURE},
    /* 2977 */ {I_VFMSUBADD132PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12121, IF_FMA|IF_FUTURE},
    /* 2978 */ {I_VFMSUBADD312PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12100, IF_FMA|IF_FUTURE},
    /* 2979 */ {I_VFMSUBADD312PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12107, IF_FMA|IF_FUTURE},
    /* 2980 */ {I_VFMSUBADD312PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12114, IF_FMA|IF_FUTURE},
    /* 2981 */ {I_VFMSUBADD312PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12121, IF_FMA|IF_FUTURE},
    /* 2982 */ {I_VFMSUBADD213PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12128, IF_FMA|IF_FUTURE},
    /* 2983 */ {I_VFMSUBADD213PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12135, IF_FMA|IF_FUTURE},
    /* 2984 */ {I_VFMSUBADD213PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12142, IF_FMA|IF_FUTURE},
    /* 2985 */ {I_VFMSUBADD213PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12149, IF_FMA|IF_FUTURE},
    /* 2986 */ {I_VFMSUBADD123PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12128, IF_FMA|IF_FUTURE},
    /* 2987 */ {I_VFMSUBADD123PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12135, IF_FMA|IF_FUTURE},
    /* 2988 */ {I_VFMSUBADD123PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12142, IF_FMA|IF_FUTURE},
    /* 2989 */ {I_VFMSUBADD123PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12149, IF_FMA|IF_FUTURE},
    /* 2990 */ {I_VFMSUBADD231PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12156, IF_FMA|IF_FUTURE},
    /* 2991 */ {I_VFMSUBADD231PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12163, IF_FMA|IF_FUTURE},
    /* 2992 */ {I_VFMSUBADD231PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12170, IF_FMA|IF_FUTURE},
    /* 2993 */ {I_VFMSUBADD231PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12177, IF_FMA|IF_FUTURE},
    /* 2994 */ {I_VFMSUBADD321PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12156, IF_FMA|IF_FUTURE},
    /* 2995 */ {I_VFMSUBADD321PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12163, IF_FMA|IF_FUTURE},
    /* 2996 */ {I_VFMSUBADD321PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12170, IF_FMA|IF_FUTURE},
    /* 2997 */ {I_VFMSUBADD321PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12177, IF_FMA|IF_FUTURE},
    /* 2998 */ {I_VFNMADD132PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12184, IF_FMA|IF_FUTURE},
    /* 2999 */ {I_VFNMADD132PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12191, IF_FMA|IF_FUTURE},
    /* 3000 */ {I_VFNMADD132PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12198, IF_FMA|IF_FUTURE},
    /* 3001 */ {I_VFNMADD132PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12205, IF_FMA|IF_FUTURE},
    /* 3002 */ {I_VFNMADD312PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12184, IF_FMA|IF_FUTURE},
    /* 3003 */ {I_VFNMADD312PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12191, IF_FMA|IF_FUTURE},
    /* 3004 */ {I_VFNMADD312PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12198, IF_FMA|IF_FUTURE},
    /* 3005 */ {I_VFNMADD312PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12205, IF_FMA|IF_FUTURE},
    /* 3006 */ {I_VFNMADD213PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12212, IF_FMA|IF_FUTURE},
    /* 3007 */ {I_VFNMADD213PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12219, IF_FMA|IF_FUTURE},
    /* 3008 */ {I_VFNMADD213PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12226, IF_FMA|IF_FUTURE},
    /* 3009 */ {I_VFNMADD213PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12233, IF_FMA|IF_FUTURE},
    /* 3010 */ {I_VFNMADD123PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12212, IF_FMA|IF_FUTURE},
    /* 3011 */ {I_VFNMADD123PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12219, IF_FMA|IF_FUTURE},
    /* 3012 */ {I_VFNMADD123PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12226, IF_FMA|IF_FUTURE},
    /* 3013 */ {I_VFNMADD123PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12233, IF_FMA|IF_FUTURE},
    /* 3014 */ {I_VFNMADD231PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12240, IF_FMA|IF_FUTURE},
    /* 3015 */ {I_VFNMADD231PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12247, IF_FMA|IF_FUTURE},
    /* 3016 */ {I_VFNMADD231PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12254, IF_FMA|IF_FUTURE},
    /* 3017 */ {I_VFNMADD231PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12261, IF_FMA|IF_FUTURE},
    /* 3018 */ {I_VFNMADD321PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12240, IF_FMA|IF_FUTURE},
    /* 3019 */ {I_VFNMADD321PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12247, IF_FMA|IF_FUTURE},
    /* 3020 */ {I_VFNMADD321PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12254, IF_FMA|IF_FUTURE},
    /* 3021 */ {I_VFNMADD321PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12261, IF_FMA|IF_FUTURE},
    /* 3022 */ {I_VFNMSUB132PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12268, IF_FMA|IF_FUTURE},
    /* 3023 */ {I_VFNMSUB132PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12275, IF_FMA|IF_FUTURE},
    /* 3024 */ {I_VFNMSUB132PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12282, IF_FMA|IF_FUTURE},
    /* 3025 */ {I_VFNMSUB132PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12289, IF_FMA|IF_FUTURE},
    /* 3026 */ {I_VFNMSUB312PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12268, IF_FMA|IF_FUTURE},
    /* 3027 */ {I_VFNMSUB312PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12275, IF_FMA|IF_FUTURE},
    /* 3028 */ {I_VFNMSUB312PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12282, IF_FMA|IF_FUTURE},
    /* 3029 */ {I_VFNMSUB312PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12289, IF_FMA|IF_FUTURE},
    /* 3030 */ {I_VFNMSUB213PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12296, IF_FMA|IF_FUTURE},
    /* 3031 */ {I_VFNMSUB213PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12303, IF_FMA|IF_FUTURE},
    /* 3032 */ {I_VFNMSUB213PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12310, IF_FMA|IF_FUTURE},
    /* 3033 */ {I_VFNMSUB213PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12317, IF_FMA|IF_FUTURE},
    /* 3034 */ {I_VFNMSUB123PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12296, IF_FMA|IF_FUTURE},
    /* 3035 */ {I_VFNMSUB123PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12303, IF_FMA|IF_FUTURE},
    /* 3036 */ {I_VFNMSUB123PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12310, IF_FMA|IF_FUTURE},
    /* 3037 */ {I_VFNMSUB123PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12317, IF_FMA|IF_FUTURE},
    /* 3038 */ {I_VFNMSUB231PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12324, IF_FMA|IF_FUTURE},
    /* 3039 */ {I_VFNMSUB231PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12331, IF_FMA|IF_FUTURE},
    /* 3040 */ {I_VFNMSUB231PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12338, IF_FMA|IF_FUTURE},
    /* 3041 */ {I_VFNMSUB231PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12345, IF_FMA|IF_FUTURE},
    /* 3042 */ {I_VFNMSUB321PS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12324, IF_FMA|IF_FUTURE},
    /* 3043 */ {I_VFNMSUB321PS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12331, IF_FMA|IF_FUTURE},
    /* 3044 */ {I_VFNMSUB321PD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12338, IF_FMA|IF_FUTURE},
    /* 3045 */ {I_VFNMSUB321PD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+12345, IF_FMA|IF_FUTURE},
    /* 3046 */ {I_VFMADD132SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12352, IF_FMA|IF_FUTURE},
    /* 3047 */ {I_VFMADD132SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12359, IF_FMA|IF_FUTURE},
    /* 3048 */ {I_VFMADD312SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12352, IF_FMA|IF_FUTURE},
    /* 3049 */ {I_VFMADD312SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12359, IF_FMA|IF_FUTURE},
    /* 3050 */ {I_VFMADD213SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12366, IF_FMA|IF_FUTURE},
    /* 3051 */ {I_VFMADD213SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12373, IF_FMA|IF_FUTURE},
    /* 3052 */ {I_VFMADD123SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12366, IF_FMA|IF_FUTURE},
    /* 3053 */ {I_VFMADD123SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12373, IF_FMA|IF_FUTURE},
    /* 3054 */ {I_VFMADD231SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12380, IF_FMA|IF_FUTURE},
    /* 3055 */ {I_VFMADD231SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12387, IF_FMA|IF_FUTURE},
    /* 3056 */ {I_VFMADD321SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12380, IF_FMA|IF_FUTURE},
    /* 3057 */ {I_VFMADD321SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12387, IF_FMA|IF_FUTURE},
    /* 3058 */ {I_VFMSUB132SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12394, IF_FMA|IF_FUTURE},
    /* 3059 */ {I_VFMSUB132SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12401, IF_FMA|IF_FUTURE},
    /* 3060 */ {I_VFMSUB312SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12394, IF_FMA|IF_FUTURE},
    /* 3061 */ {I_VFMSUB312SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12401, IF_FMA|IF_FUTURE},
    /* 3062 */ {I_VFMSUB213SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12408, IF_FMA|IF_FUTURE},
    /* 3063 */ {I_VFMSUB213SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12415, IF_FMA|IF_FUTURE},
    /* 3064 */ {I_VFMSUB123SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12408, IF_FMA|IF_FUTURE},
    /* 3065 */ {I_VFMSUB123SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12415, IF_FMA|IF_FUTURE},
    /* 3066 */ {I_VFMSUB231SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12422, IF_FMA|IF_FUTURE},
    /* 3067 */ {I_VFMSUB231SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12429, IF_FMA|IF_FUTURE},
    /* 3068 */ {I_VFMSUB321SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12422, IF_FMA|IF_FUTURE},
    /* 3069 */ {I_VFMSUB321SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12429, IF_FMA|IF_FUTURE},
    /* 3070 */ {I_VFNMADD132SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12436, IF_FMA|IF_FUTURE},
    /* 3071 */ {I_VFNMADD132SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12443, IF_FMA|IF_FUTURE},
    /* 3072 */ {I_VFNMADD312SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12436, IF_FMA|IF_FUTURE},
    /* 3073 */ {I_VFNMADD312SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12443, IF_FMA|IF_FUTURE},
    /* 3074 */ {I_VFNMADD213SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12450, IF_FMA|IF_FUTURE},
    /* 3075 */ {I_VFNMADD213SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12457, IF_FMA|IF_FUTURE},
    /* 3076 */ {I_VFNMADD123SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12450, IF_FMA|IF_FUTURE},
    /* 3077 */ {I_VFNMADD123SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12457, IF_FMA|IF_FUTURE},
    /* 3078 */ {I_VFNMADD231SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12464, IF_FMA|IF_FUTURE},
    /* 3079 */ {I_VFNMADD231SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12471, IF_FMA|IF_FUTURE},
    /* 3080 */ {I_VFNMADD321SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12464, IF_FMA|IF_FUTURE},
    /* 3081 */ {I_VFNMADD321SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12471, IF_FMA|IF_FUTURE},
    /* 3082 */ {I_VFNMSUB132SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12478, IF_FMA|IF_FUTURE},
    /* 3083 */ {I_VFNMSUB132SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12485, IF_FMA|IF_FUTURE},
    /* 3084 */ {I_VFNMSUB312SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12478, IF_FMA|IF_FUTURE},
    /* 3085 */ {I_VFNMSUB312SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12485, IF_FMA|IF_FUTURE},
    /* 3086 */ {I_VFNMSUB213SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12492, IF_FMA|IF_FUTURE},
    /* 3087 */ {I_VFNMSUB213SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12499, IF_FMA|IF_FUTURE},
    /* 3088 */ {I_VFNMSUB123SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12492, IF_FMA|IF_FUTURE},
    /* 3089 */ {I_VFNMSUB123SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12499, IF_FMA|IF_FUTURE},
    /* 3090 */ {I_VFNMSUB231SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12506, IF_FMA|IF_FUTURE},
    /* 3091 */ {I_VFNMSUB231SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12513, IF_FMA|IF_FUTURE},
    /* 3092 */ {I_VFNMSUB321SS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+12506, IF_FMA|IF_FUTURE},
    /* 3093 */ {I_VFNMSUB321SD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+12513, IF_FMA|IF_FUTURE},
    /* 3094 */ {I_RDFSBASE, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+12521, IF_LONG|IF_FUTURE},
    /* 3095 */ {I_RDFSBASE, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+12520, IF_LONG|IF_FUTURE},
    /* 3096 */ {I_RDGSBASE, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+12528, IF_LONG|IF_FUTURE},
    /* 3097 */ {I_RDGSBASE, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+12527, IF_LONG|IF_FUTURE},
    /* 3098 */ {I_RDRAND, 1, {REG_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15572, IF_FUTURE},
    /* 3099 */ {I_RDRAND, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15578, IF_FUTURE},
    /* 3100 */ {I_RDRAND, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15584, IF_LONG|IF_FUTURE},
    /* 3101 */ {I_WRFSBASE, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+12535, IF_LONG|IF_FUTURE},
    /* 3102 */ {I_WRFSBASE, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+12534, IF_LONG|IF_FUTURE},
    /* 3103 */ {I_WRGSBASE, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+12542, IF_LONG|IF_FUTURE},
    /* 3104 */ {I_WRGSBASE, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+12541, IF_LONG|IF_FUTURE},
    /* 3105 */ {I_VCVTPH2PS, 2, {YMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12548, IF_AVX|IF_FUTURE},
    /* 3106 */ {I_VCVTPH2PS, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+12555, IF_AVX|IF_FUTURE},
    /* 3107 */ {I_VCVTPS2PH, 3, {RM_XMM|BITS128,YMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6680, IF_AVX|IF_FUTURE},
    /* 3108 */ {I_VCVTPS2PH, 3, {RM_XMM|BITS64,XMMREG,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6688, IF_AVX|IF_FUTURE},
    /* 3109 */ {I_XSTORE, 0, {0,0,0,0,0}, nasm_bytecodes+18627, IF_PENT|IF_CYRIX},
    /* 3110 */ {I_XCRYPTECB, 0, {0,0,0,0,0}, nasm_bytecodes+15590, IF_PENT|IF_CYRIX},
    /* 3111 */ {I_XCRYPTCBC, 0, {0,0,0,0,0}, nasm_bytecodes+15596, IF_PENT|IF_CYRIX},
    /* 3112 */ {I_XCRYPTCTR, 0, {0,0,0,0,0}, nasm_bytecodes+15602, IF_PENT|IF_CYRIX},
    /* 3113 */ {I_XCRYPTCFB, 0, {0,0,0,0,0}, nasm_bytecodes+15608, IF_PENT|IF_CYRIX},
    /* 3114 */ {I_XCRYPTOFB, 0, {0,0,0,0,0}, nasm_bytecodes+15614, IF_PENT|IF_CYRIX},
    /* 3115 */ {I_MONTMUL, 0, {0,0,0,0,0}, nasm_bytecodes+15620, IF_PENT|IF_CYRIX},
    /* 3116 */ {I_XSHA1, 0, {0,0,0,0,0}, nasm_bytecodes+15626, IF_PENT|IF_CYRIX},
    /* 3117 */ {I_XSHA256, 0, {0,0,0,0,0}, nasm_bytecodes+15632, IF_PENT|IF_CYRIX},
    /* 3118 */ {I_LLWPCB, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+12562, IF_AMD|IF_386},
    /* 3119 */ {I_LLWPCB, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+12569, IF_AMD|IF_X64},
    /* 3120 */ {I_SLWPCB, 1, {REG_GPR|BITS32,0,0,0,0}, nasm_bytecodes+12576, IF_AMD|IF_386},
    /* 3121 */ {I_SLWPCB, 1, {REG_GPR|BITS64,0,0,0,0}, nasm_bytecodes+12583, IF_AMD|IF_X64},
    /* 3122 */ {I_LWPVAL, 3, {REG_GPR|BITS32,RM_GPR|BITS32,IMMEDIATE|BITS32,0,0}, nasm_bytecodes+6696, IF_AMD|IF_386},
    /* 3123 */ {I_LWPVAL, 3, {REG_GPR|BITS64,RM_GPR|BITS32,IMMEDIATE|BITS32,0,0}, nasm_bytecodes+6704, IF_AMD|IF_X64},
    /* 3124 */ {I_LWPINS, 3, {REG_GPR|BITS32,RM_GPR|BITS32,IMMEDIATE|BITS32,0,0}, nasm_bytecodes+6712, IF_AMD|IF_386},
    /* 3125 */ {I_LWPINS, 3, {REG_GPR|BITS64,RM_GPR|BITS32,IMMEDIATE|BITS32,0,0}, nasm_bytecodes+6720, IF_AMD|IF_X64},
    /* 3126 */ {I_VFMADDPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+3708, IF_AMD|IF_SSE5},
    /* 3127 */ {I_VFMADDPD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+3717, IF_AMD|IF_SSE5},
    /* 3128 */ {I_VFMADDPD, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+3726, IF_AMD|IF_SSE5},
    /* 3129 */ {I_VFMADDPD, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+3735, IF_AMD|IF_SSE5},
    /* 3130 */ {I_VFMADDPD, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+3744, IF_AMD|IF_SSE5},
    /* 3131 */ {I_VFMADDPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+3753, IF_AMD|IF_SSE5},
    /* 3132 */ {I_VFMADDPD, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+3762, IF_AMD|IF_SSE5},
    /* 3133 */ {I_VFMADDPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+3771, IF_AMD|IF_SSE5},
    /* 3134 */ {I_VFMADDPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+3780, IF_AMD|IF_SSE5},
    /* 3135 */ {I_VFMADDPS, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+3789, IF_AMD|IF_SSE5},
    /* 3136 */ {I_VFMADDPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+3798, IF_AMD|IF_SSE5},
    /* 3137 */ {I_VFMADDPS, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+3807, IF_AMD|IF_SSE5},
    /* 3138 */ {I_VFMADDPS, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+3816, IF_AMD|IF_SSE5},
    /* 3139 */ {I_VFMADDPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+3825, IF_AMD|IF_SSE5},
    /* 3140 */ {I_VFMADDPS, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+3834, IF_AMD|IF_SSE5},
    /* 3141 */ {I_VFMADDPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+3843, IF_AMD|IF_SSE5},
    /* 3142 */ {I_VFMADDSD, 4, {XMMREG,XMMREG,RM_XMM|BITS64,XMMREG,0}, nasm_bytecodes+3852, IF_AMD|IF_SSE5},
    /* 3143 */ {I_VFMADDSD, 3, {XMMREG,RM_XMM|BITS64,XMMREG,0,0}, nasm_bytecodes+3861, IF_AMD|IF_SSE5},
    /* 3144 */ {I_VFMADDSD, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS64,0}, nasm_bytecodes+3870, IF_AMD|IF_SSE5},
    /* 3145 */ {I_VFMADDSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+3879, IF_AMD|IF_SSE5},
    /* 3146 */ {I_VFMADDSS, 4, {XMMREG,XMMREG,RM_XMM|BITS32,XMMREG,0}, nasm_bytecodes+3888, IF_AMD|IF_SSE5},
    /* 3147 */ {I_VFMADDSS, 3, {XMMREG,RM_XMM|BITS32,XMMREG,0,0}, nasm_bytecodes+3897, IF_AMD|IF_SSE5},
    /* 3148 */ {I_VFMADDSS, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS32,0}, nasm_bytecodes+3906, IF_AMD|IF_SSE5},
    /* 3149 */ {I_VFMADDSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+3915, IF_AMD|IF_SSE5},
    /* 3150 */ {I_VFMADDSUBPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+3924, IF_AMD|IF_SSE5},
    /* 3151 */ {I_VFMADDSUBPD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+3933, IF_AMD|IF_SSE5},
    /* 3152 */ {I_VFMADDSUBPD, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+3942, IF_AMD|IF_SSE5},
    /* 3153 */ {I_VFMADDSUBPD, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+3951, IF_AMD|IF_SSE5},
    /* 3154 */ {I_VFMADDSUBPD, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+3960, IF_AMD|IF_SSE5},
    /* 3155 */ {I_VFMADDSUBPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+3969, IF_AMD|IF_SSE5},
    /* 3156 */ {I_VFMADDSUBPD, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+3978, IF_AMD|IF_SSE5},
    /* 3157 */ {I_VFMADDSUBPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+3987, IF_AMD|IF_SSE5},
    /* 3158 */ {I_VFMADDSUBPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+3996, IF_AMD|IF_SSE5},
    /* 3159 */ {I_VFMADDSUBPS, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4005, IF_AMD|IF_SSE5},
    /* 3160 */ {I_VFMADDSUBPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+4014, IF_AMD|IF_SSE5},
    /* 3161 */ {I_VFMADDSUBPS, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+4023, IF_AMD|IF_SSE5},
    /* 3162 */ {I_VFMADDSUBPS, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+4032, IF_AMD|IF_SSE5},
    /* 3163 */ {I_VFMADDSUBPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+4041, IF_AMD|IF_SSE5},
    /* 3164 */ {I_VFMADDSUBPS, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+4050, IF_AMD|IF_SSE5},
    /* 3165 */ {I_VFMADDSUBPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+4059, IF_AMD|IF_SSE5},
    /* 3166 */ {I_VFMSUBADDPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4068, IF_AMD|IF_SSE5},
    /* 3167 */ {I_VFMSUBADDPD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4077, IF_AMD|IF_SSE5},
    /* 3168 */ {I_VFMSUBADDPD, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+4086, IF_AMD|IF_SSE5},
    /* 3169 */ {I_VFMSUBADDPD, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+4095, IF_AMD|IF_SSE5},
    /* 3170 */ {I_VFMSUBADDPD, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+4104, IF_AMD|IF_SSE5},
    /* 3171 */ {I_VFMSUBADDPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+4113, IF_AMD|IF_SSE5},
    /* 3172 */ {I_VFMSUBADDPD, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+4122, IF_AMD|IF_SSE5},
    /* 3173 */ {I_VFMSUBADDPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+4131, IF_AMD|IF_SSE5},
    /* 3174 */ {I_VFMSUBADDPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4140, IF_AMD|IF_SSE5},
    /* 3175 */ {I_VFMSUBADDPS, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4149, IF_AMD|IF_SSE5},
    /* 3176 */ {I_VFMSUBADDPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+4158, IF_AMD|IF_SSE5},
    /* 3177 */ {I_VFMSUBADDPS, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+4167, IF_AMD|IF_SSE5},
    /* 3178 */ {I_VFMSUBADDPS, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+4176, IF_AMD|IF_SSE5},
    /* 3179 */ {I_VFMSUBADDPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+4185, IF_AMD|IF_SSE5},
    /* 3180 */ {I_VFMSUBADDPS, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+4194, IF_AMD|IF_SSE5},
    /* 3181 */ {I_VFMSUBADDPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+4203, IF_AMD|IF_SSE5},
    /* 3182 */ {I_VFMSUBPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4212, IF_AMD|IF_SSE5},
    /* 3183 */ {I_VFMSUBPD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4221, IF_AMD|IF_SSE5},
    /* 3184 */ {I_VFMSUBPD, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+4230, IF_AMD|IF_SSE5},
    /* 3185 */ {I_VFMSUBPD, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+4239, IF_AMD|IF_SSE5},
    /* 3186 */ {I_VFMSUBPD, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+4248, IF_AMD|IF_SSE5},
    /* 3187 */ {I_VFMSUBPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+4257, IF_AMD|IF_SSE5},
    /* 3188 */ {I_VFMSUBPD, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+4266, IF_AMD|IF_SSE5},
    /* 3189 */ {I_VFMSUBPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+4275, IF_AMD|IF_SSE5},
    /* 3190 */ {I_VFMSUBPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4284, IF_AMD|IF_SSE5},
    /* 3191 */ {I_VFMSUBPS, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4293, IF_AMD|IF_SSE5},
    /* 3192 */ {I_VFMSUBPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+4302, IF_AMD|IF_SSE5},
    /* 3193 */ {I_VFMSUBPS, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+4311, IF_AMD|IF_SSE5},
    /* 3194 */ {I_VFMSUBPS, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+4320, IF_AMD|IF_SSE5},
    /* 3195 */ {I_VFMSUBPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+4329, IF_AMD|IF_SSE5},
    /* 3196 */ {I_VFMSUBPS, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+4338, IF_AMD|IF_SSE5},
    /* 3197 */ {I_VFMSUBPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+4347, IF_AMD|IF_SSE5},
    /* 3198 */ {I_VFMSUBSD, 4, {XMMREG,XMMREG,RM_XMM|BITS64,XMMREG,0}, nasm_bytecodes+4356, IF_AMD|IF_SSE5},
    /* 3199 */ {I_VFMSUBSD, 3, {XMMREG,RM_XMM|BITS64,XMMREG,0,0}, nasm_bytecodes+4365, IF_AMD|IF_SSE5},
    /* 3200 */ {I_VFMSUBSD, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS64,0}, nasm_bytecodes+4374, IF_AMD|IF_SSE5},
    /* 3201 */ {I_VFMSUBSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+4383, IF_AMD|IF_SSE5},
    /* 3202 */ {I_VFMSUBSS, 4, {XMMREG,XMMREG,RM_XMM|BITS32,XMMREG,0}, nasm_bytecodes+4392, IF_AMD|IF_SSE5},
    /* 3203 */ {I_VFMSUBSS, 3, {XMMREG,RM_XMM|BITS32,XMMREG,0,0}, nasm_bytecodes+4401, IF_AMD|IF_SSE5},
    /* 3204 */ {I_VFMSUBSS, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS32,0}, nasm_bytecodes+4410, IF_AMD|IF_SSE5},
    /* 3205 */ {I_VFMSUBSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+4419, IF_AMD|IF_SSE5},
    /* 3206 */ {I_VFNMADDPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4428, IF_AMD|IF_SSE5},
    /* 3207 */ {I_VFNMADDPD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4437, IF_AMD|IF_SSE5},
    /* 3208 */ {I_VFNMADDPD, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+4446, IF_AMD|IF_SSE5},
    /* 3209 */ {I_VFNMADDPD, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+4455, IF_AMD|IF_SSE5},
    /* 3210 */ {I_VFNMADDPD, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+4464, IF_AMD|IF_SSE5},
    /* 3211 */ {I_VFNMADDPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+4473, IF_AMD|IF_SSE5},
    /* 3212 */ {I_VFNMADDPD, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+4482, IF_AMD|IF_SSE5},
    /* 3213 */ {I_VFNMADDPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+4491, IF_AMD|IF_SSE5},
    /* 3214 */ {I_VFNMADDPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4500, IF_AMD|IF_SSE5},
    /* 3215 */ {I_VFNMADDPS, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4509, IF_AMD|IF_SSE5},
    /* 3216 */ {I_VFNMADDPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+4518, IF_AMD|IF_SSE5},
    /* 3217 */ {I_VFNMADDPS, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+4527, IF_AMD|IF_SSE5},
    /* 3218 */ {I_VFNMADDPS, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+4536, IF_AMD|IF_SSE5},
    /* 3219 */ {I_VFNMADDPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+4545, IF_AMD|IF_SSE5},
    /* 3220 */ {I_VFNMADDPS, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+4554, IF_AMD|IF_SSE5},
    /* 3221 */ {I_VFNMADDPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+4563, IF_AMD|IF_SSE5},
    /* 3222 */ {I_VFNMADDSD, 4, {XMMREG,XMMREG,RM_XMM|BITS64,XMMREG,0}, nasm_bytecodes+4572, IF_AMD|IF_SSE5},
    /* 3223 */ {I_VFNMADDSD, 3, {XMMREG,RM_XMM|BITS64,XMMREG,0,0}, nasm_bytecodes+4581, IF_AMD|IF_SSE5},
    /* 3224 */ {I_VFNMADDSD, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS64,0}, nasm_bytecodes+4590, IF_AMD|IF_SSE5},
    /* 3225 */ {I_VFNMADDSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+4599, IF_AMD|IF_SSE5},
    /* 3226 */ {I_VFNMADDSS, 4, {XMMREG,XMMREG,RM_XMM|BITS32,XMMREG,0}, nasm_bytecodes+4608, IF_AMD|IF_SSE5},
    /* 3227 */ {I_VFNMADDSS, 3, {XMMREG,RM_XMM|BITS32,XMMREG,0,0}, nasm_bytecodes+4617, IF_AMD|IF_SSE5},
    /* 3228 */ {I_VFNMADDSS, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS32,0}, nasm_bytecodes+4626, IF_AMD|IF_SSE5},
    /* 3229 */ {I_VFNMADDSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+4635, IF_AMD|IF_SSE5},
    /* 3230 */ {I_VFNMSUBPD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4644, IF_AMD|IF_SSE5},
    /* 3231 */ {I_VFNMSUBPD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4653, IF_AMD|IF_SSE5},
    /* 3232 */ {I_VFNMSUBPD, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+4662, IF_AMD|IF_SSE5},
    /* 3233 */ {I_VFNMSUBPD, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+4671, IF_AMD|IF_SSE5},
    /* 3234 */ {I_VFNMSUBPD, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+4680, IF_AMD|IF_SSE5},
    /* 3235 */ {I_VFNMSUBPD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+4689, IF_AMD|IF_SSE5},
    /* 3236 */ {I_VFNMSUBPD, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+4698, IF_AMD|IF_SSE5},
    /* 3237 */ {I_VFNMSUBPD, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+4707, IF_AMD|IF_SSE5},
    /* 3238 */ {I_VFNMSUBPS, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4716, IF_AMD|IF_SSE5},
    /* 3239 */ {I_VFNMSUBPS, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4725, IF_AMD|IF_SSE5},
    /* 3240 */ {I_VFNMSUBPS, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+4734, IF_AMD|IF_SSE5},
    /* 3241 */ {I_VFNMSUBPS, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+4743, IF_AMD|IF_SSE5},
    /* 3242 */ {I_VFNMSUBPS, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+4752, IF_AMD|IF_SSE5},
    /* 3243 */ {I_VFNMSUBPS, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+4761, IF_AMD|IF_SSE5},
    /* 3244 */ {I_VFNMSUBPS, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+4770, IF_AMD|IF_SSE5},
    /* 3245 */ {I_VFNMSUBPS, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+4779, IF_AMD|IF_SSE5},
    /* 3246 */ {I_VFNMSUBSD, 4, {XMMREG,XMMREG,RM_XMM|BITS64,XMMREG,0}, nasm_bytecodes+4788, IF_AMD|IF_SSE5},
    /* 3247 */ {I_VFNMSUBSD, 3, {XMMREG,RM_XMM|BITS64,XMMREG,0,0}, nasm_bytecodes+4797, IF_AMD|IF_SSE5},
    /* 3248 */ {I_VFNMSUBSD, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS64,0}, nasm_bytecodes+4806, IF_AMD|IF_SSE5},
    /* 3249 */ {I_VFNMSUBSD, 3, {XMMREG,XMMREG,RM_XMM|BITS64,0,0}, nasm_bytecodes+4815, IF_AMD|IF_SSE5},
    /* 3250 */ {I_VFNMSUBSS, 4, {XMMREG,XMMREG,RM_XMM|BITS32,XMMREG,0}, nasm_bytecodes+4824, IF_AMD|IF_SSE5},
    /* 3251 */ {I_VFNMSUBSS, 3, {XMMREG,RM_XMM|BITS32,XMMREG,0,0}, nasm_bytecodes+4833, IF_AMD|IF_SSE5},
    /* 3252 */ {I_VFNMSUBSS, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS32,0}, nasm_bytecodes+4842, IF_AMD|IF_SSE5},
    /* 3253 */ {I_VFNMSUBSS, 3, {XMMREG,XMMREG,RM_XMM|BITS32,0,0}, nasm_bytecodes+4851, IF_AMD|IF_SSE5},
    /* 3254 */ {I_VFRCZPD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12590, IF_AMD|IF_SSE5},
    /* 3255 */ {I_VFRCZPD, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12597, IF_AMD|IF_SSE5},
    /* 3256 */ {I_VFRCZPD, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+12604, IF_AMD|IF_SSE5},
    /* 3257 */ {I_VFRCZPD, 1, {YMMREG,0,0,0,0}, nasm_bytecodes+12611, IF_AMD|IF_SSE5},
    /* 3258 */ {I_VFRCZPS, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12618, IF_AMD|IF_SSE5},
    /* 3259 */ {I_VFRCZPS, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12625, IF_AMD|IF_SSE5},
    /* 3260 */ {I_VFRCZPS, 2, {YMMREG,RM_YMM|BITS256,0,0,0}, nasm_bytecodes+12632, IF_AMD|IF_SSE5},
    /* 3261 */ {I_VFRCZPS, 1, {YMMREG,0,0,0,0}, nasm_bytecodes+12639, IF_AMD|IF_SSE5},
    /* 3262 */ {I_VFRCZSD, 2, {XMMREG,RM_XMM|BITS64,0,0,0}, nasm_bytecodes+12646, IF_AMD|IF_SSE5},
    /* 3263 */ {I_VFRCZSD, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12653, IF_AMD|IF_SSE5},
    /* 3264 */ {I_VFRCZSS, 2, {XMMREG,RM_XMM|BITS32,0,0,0}, nasm_bytecodes+12660, IF_AMD|IF_SSE5},
    /* 3265 */ {I_VFRCZSS, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12667, IF_AMD|IF_SSE5},
    /* 3266 */ {I_VPCMOV, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4860, IF_AMD|IF_SSE5},
    /* 3267 */ {I_VPCMOV, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4869, IF_AMD|IF_SSE5},
    /* 3268 */ {I_VPCMOV, 4, {YMMREG,YMMREG,RM_YMM|BITS256,YMMREG,0}, nasm_bytecodes+4878, IF_AMD|IF_SSE5},
    /* 3269 */ {I_VPCMOV, 3, {YMMREG,RM_YMM|BITS256,YMMREG,0,0}, nasm_bytecodes+4887, IF_AMD|IF_SSE5},
    /* 3270 */ {I_VPCMOV, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+4896, IF_AMD|IF_SSE5},
    /* 3271 */ {I_VPCMOV, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+4905, IF_AMD|IF_SSE5},
    /* 3272 */ {I_VPCMOV, 4, {YMMREG,YMMREG,YMMREG,RM_YMM|BITS256,0}, nasm_bytecodes+4914, IF_AMD|IF_SSE5},
    /* 3273 */ {I_VPCMOV, 3, {YMMREG,YMMREG,RM_YMM|BITS256,0,0}, nasm_bytecodes+4923, IF_AMD|IF_SSE5},
    /* 3274 */ {I_VPCOMB, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6728, IF_AMD|IF_SSE5},
    /* 3275 */ {I_VPCOMB, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6736, IF_AMD|IF_SSE5},
    /* 3276 */ {I_VPCOMD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6744, IF_AMD|IF_SSE5},
    /* 3277 */ {I_VPCOMD, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6752, IF_AMD|IF_SSE5},
    /* 3278 */ {I_VPCOMQ, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6760, IF_AMD|IF_SSE5},
    /* 3279 */ {I_VPCOMQ, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6768, IF_AMD|IF_SSE5},
    /* 3280 */ {I_VPCOMUB, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6776, IF_AMD|IF_SSE5},
    /* 3281 */ {I_VPCOMUB, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6784, IF_AMD|IF_SSE5},
    /* 3282 */ {I_VPCOMUD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6792, IF_AMD|IF_SSE5},
    /* 3283 */ {I_VPCOMUD, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6800, IF_AMD|IF_SSE5},
    /* 3284 */ {I_VPCOMUQ, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6808, IF_AMD|IF_SSE5},
    /* 3285 */ {I_VPCOMUQ, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6816, IF_AMD|IF_SSE5},
    /* 3286 */ {I_VPCOMUW, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6824, IF_AMD|IF_SSE5},
    /* 3287 */ {I_VPCOMUW, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6832, IF_AMD|IF_SSE5},
    /* 3288 */ {I_VPCOMW, 4, {XMMREG,XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0}, nasm_bytecodes+6840, IF_AMD|IF_SSE5},
    /* 3289 */ {I_VPCOMW, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6848, IF_AMD|IF_SSE5},
    /* 3290 */ {I_VPHADDBD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12674, IF_AMD|IF_SSE5},
    /* 3291 */ {I_VPHADDBD, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12681, IF_AMD|IF_SSE5},
    /* 3292 */ {I_VPHADDBQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12688, IF_AMD|IF_SSE5},
    /* 3293 */ {I_VPHADDBQ, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12695, IF_AMD|IF_SSE5},
    /* 3294 */ {I_VPHADDBW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12702, IF_AMD|IF_SSE5},
    /* 3295 */ {I_VPHADDBW, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12709, IF_AMD|IF_SSE5},
    /* 3296 */ {I_VPHADDDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12716, IF_AMD|IF_SSE5},
    /* 3297 */ {I_VPHADDDQ, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12723, IF_AMD|IF_SSE5},
    /* 3298 */ {I_VPHADDUBD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12730, IF_AMD|IF_SSE5},
    /* 3299 */ {I_VPHADDUBD, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12737, IF_AMD|IF_SSE5},
    /* 3300 */ {I_VPHADDUBQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12744, IF_AMD|IF_SSE5},
    /* 3301 */ {I_VPHADDUBQ, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12751, IF_AMD|IF_SSE5},
    /* 3302 */ {I_VPHADDUBW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12758, IF_AMD|IF_SSE5},
    /* 3303 */ {I_VPHADDUBW, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12765, IF_AMD|IF_SSE5},
    /* 3304 */ {I_VPHADDUDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12772, IF_AMD|IF_SSE5},
    /* 3305 */ {I_VPHADDUDQ, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12779, IF_AMD|IF_SSE5},
    /* 3306 */ {I_VPHADDUWD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12786, IF_AMD|IF_SSE5},
    /* 3307 */ {I_VPHADDUWD, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12793, IF_AMD|IF_SSE5},
    /* 3308 */ {I_VPHADDUWQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12800, IF_AMD|IF_SSE5},
    /* 3309 */ {I_VPHADDUWQ, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12807, IF_AMD|IF_SSE5},
    /* 3310 */ {I_VPHADDWD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12814, IF_AMD|IF_SSE5},
    /* 3311 */ {I_VPHADDWD, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12821, IF_AMD|IF_SSE5},
    /* 3312 */ {I_VPHADDWQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12828, IF_AMD|IF_SSE5},
    /* 3313 */ {I_VPHADDWQ, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12835, IF_AMD|IF_SSE5},
    /* 3314 */ {I_VPHSUBBW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12842, IF_AMD|IF_SSE5},
    /* 3315 */ {I_VPHSUBBW, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12849, IF_AMD|IF_SSE5},
    /* 3316 */ {I_VPHSUBDQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12856, IF_AMD|IF_SSE5},
    /* 3317 */ {I_VPHSUBDQ, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12863, IF_AMD|IF_SSE5},
    /* 3318 */ {I_VPHSUBWD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12870, IF_AMD|IF_SSE5},
    /* 3319 */ {I_VPHSUBWD, 1, {XMMREG,0,0,0,0}, nasm_bytecodes+12877, IF_AMD|IF_SSE5},
    /* 3320 */ {I_VPMACSDD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4932, IF_AMD|IF_SSE5},
    /* 3321 */ {I_VPMACSDD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4941, IF_AMD|IF_SSE5},
    /* 3322 */ {I_VPMACSDQH, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4950, IF_AMD|IF_SSE5},
    /* 3323 */ {I_VPMACSDQH, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4959, IF_AMD|IF_SSE5},
    /* 3324 */ {I_VPMACSDQL, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4968, IF_AMD|IF_SSE5},
    /* 3325 */ {I_VPMACSDQL, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4977, IF_AMD|IF_SSE5},
    /* 3326 */ {I_VPMACSSDD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+4986, IF_AMD|IF_SSE5},
    /* 3327 */ {I_VPMACSSDD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+4995, IF_AMD|IF_SSE5},
    /* 3328 */ {I_VPMACSSDQH, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+5004, IF_AMD|IF_SSE5},
    /* 3329 */ {I_VPMACSSDQH, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+5013, IF_AMD|IF_SSE5},
    /* 3330 */ {I_VPMACSSDQL, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+5022, IF_AMD|IF_SSE5},
    /* 3331 */ {I_VPMACSSDQL, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+5031, IF_AMD|IF_SSE5},
    /* 3332 */ {I_VPMACSSWD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+5040, IF_AMD|IF_SSE5},
    /* 3333 */ {I_VPMACSSWD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+5049, IF_AMD|IF_SSE5},
    /* 3334 */ {I_VPMACSSWW, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+5058, IF_AMD|IF_SSE5},
    /* 3335 */ {I_VPMACSSWW, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+5067, IF_AMD|IF_SSE5},
    /* 3336 */ {I_VPMACSWD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+5076, IF_AMD|IF_SSE5},
    /* 3337 */ {I_VPMACSWD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+5085, IF_AMD|IF_SSE5},
    /* 3338 */ {I_VPMACSWW, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+5094, IF_AMD|IF_SSE5},
    /* 3339 */ {I_VPMACSWW, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+5103, IF_AMD|IF_SSE5},
    /* 3340 */ {I_VPMADCSSWD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+5112, IF_AMD|IF_SSE5},
    /* 3341 */ {I_VPMADCSSWD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+5121, IF_AMD|IF_SSE5},
    /* 3342 */ {I_VPMADCSWD, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+5130, IF_AMD|IF_SSE5},
    /* 3343 */ {I_VPMADCSWD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+5139, IF_AMD|IF_SSE5},
    /* 3344 */ {I_VPPERM, 4, {XMMREG,XMMREG,XMMREG,RM_XMM|BITS128,0}, nasm_bytecodes+5148, IF_AMD|IF_SSE5},
    /* 3345 */ {I_VPPERM, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+5157, IF_AMD|IF_SSE5},
    /* 3346 */ {I_VPPERM, 4, {XMMREG,XMMREG,RM_XMM|BITS128,XMMREG,0}, nasm_bytecodes+5166, IF_AMD|IF_SSE5},
    /* 3347 */ {I_VPPERM, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+5175, IF_AMD|IF_SSE5},
    /* 3348 */ {I_VPROTB, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+12884, IF_AMD|IF_SSE5},
    /* 3349 */ {I_VPROTB, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+12891, IF_AMD|IF_SSE5},
    /* 3350 */ {I_VPROTB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12898, IF_AMD|IF_SSE5},
    /* 3351 */ {I_VPROTB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12905, IF_AMD|IF_SSE5},
    /* 3352 */ {I_VPROTB, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6856, IF_AMD|IF_SSE5},
    /* 3353 */ {I_VPROTB, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6864, IF_AMD|IF_SSE5},
    /* 3354 */ {I_VPROTD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+12912, IF_AMD|IF_SSE5},
    /* 3355 */ {I_VPROTD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+12919, IF_AMD|IF_SSE5},
    /* 3356 */ {I_VPROTD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12926, IF_AMD|IF_SSE5},
    /* 3357 */ {I_VPROTD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12933, IF_AMD|IF_SSE5},
    /* 3358 */ {I_VPROTD, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6872, IF_AMD|IF_SSE5},
    /* 3359 */ {I_VPROTD, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6880, IF_AMD|IF_SSE5},
    /* 3360 */ {I_VPROTQ, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+12940, IF_AMD|IF_SSE5},
    /* 3361 */ {I_VPROTQ, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+12947, IF_AMD|IF_SSE5},
    /* 3362 */ {I_VPROTQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12954, IF_AMD|IF_SSE5},
    /* 3363 */ {I_VPROTQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12961, IF_AMD|IF_SSE5},
    /* 3364 */ {I_VPROTQ, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6888, IF_AMD|IF_SSE5},
    /* 3365 */ {I_VPROTQ, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6896, IF_AMD|IF_SSE5},
    /* 3366 */ {I_VPROTW, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+12968, IF_AMD|IF_SSE5},
    /* 3367 */ {I_VPROTW, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+12975, IF_AMD|IF_SSE5},
    /* 3368 */ {I_VPROTW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+12982, IF_AMD|IF_SSE5},
    /* 3369 */ {I_VPROTW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+12989, IF_AMD|IF_SSE5},
    /* 3370 */ {I_VPROTW, 3, {XMMREG,RM_XMM|BITS128,IMMEDIATE|BITS8,0,0}, nasm_bytecodes+6904, IF_AMD|IF_SSE5},
    /* 3371 */ {I_VPROTW, 2, {XMMREG,IMMEDIATE|BITS8,0,0,0}, nasm_bytecodes+6912, IF_AMD|IF_SSE5},
    /* 3372 */ {I_VPSHAB, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+12996, IF_AMD|IF_SSE5},
    /* 3373 */ {I_VPSHAB, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+13003, IF_AMD|IF_SSE5},
    /* 3374 */ {I_VPSHAB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+13010, IF_AMD|IF_SSE5},
    /* 3375 */ {I_VPSHAB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+13017, IF_AMD|IF_SSE5},
    /* 3376 */ {I_VPSHAD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+13024, IF_AMD|IF_SSE5},
    /* 3377 */ {I_VPSHAD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+13031, IF_AMD|IF_SSE5},
    /* 3378 */ {I_VPSHAD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+13038, IF_AMD|IF_SSE5},
    /* 3379 */ {I_VPSHAD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+13045, IF_AMD|IF_SSE5},
    /* 3380 */ {I_VPSHAQ, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+13052, IF_AMD|IF_SSE5},
    /* 3381 */ {I_VPSHAQ, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+13059, IF_AMD|IF_SSE5},
    /* 3382 */ {I_VPSHAQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+13066, IF_AMD|IF_SSE5},
    /* 3383 */ {I_VPSHAQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+13073, IF_AMD|IF_SSE5},
    /* 3384 */ {I_VPSHAW, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+13080, IF_AMD|IF_SSE5},
    /* 3385 */ {I_VPSHAW, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+13087, IF_AMD|IF_SSE5},
    /* 3386 */ {I_VPSHAW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+13094, IF_AMD|IF_SSE5},
    /* 3387 */ {I_VPSHAW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+13101, IF_AMD|IF_SSE5},
    /* 3388 */ {I_VPSHLB, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+13108, IF_AMD|IF_SSE5},
    /* 3389 */ {I_VPSHLB, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+13115, IF_AMD|IF_SSE5},
    /* 3390 */ {I_VPSHLB, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+13122, IF_AMD|IF_SSE5},
    /* 3391 */ {I_VPSHLB, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+13129, IF_AMD|IF_SSE5},
    /* 3392 */ {I_VPSHLD, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+13136, IF_AMD|IF_SSE5},
    /* 3393 */ {I_VPSHLD, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+13143, IF_AMD|IF_SSE5},
    /* 3394 */ {I_VPSHLD, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+13150, IF_AMD|IF_SSE5},
    /* 3395 */ {I_VPSHLD, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+13157, IF_AMD|IF_SSE5},
    /* 3396 */ {I_VPSHLQ, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+13164, IF_AMD|IF_SSE5},
    /* 3397 */ {I_VPSHLQ, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+13171, IF_AMD|IF_SSE5},
    /* 3398 */ {I_VPSHLQ, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+13178, IF_AMD|IF_SSE5},
    /* 3399 */ {I_VPSHLQ, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+13185, IF_AMD|IF_SSE5},
    /* 3400 */ {I_VPSHLW, 3, {XMMREG,RM_XMM|BITS128,XMMREG,0,0}, nasm_bytecodes+13192, IF_AMD|IF_SSE5},
    /* 3401 */ {I_VPSHLW, 2, {XMMREG,XMMREG,0,0,0}, nasm_bytecodes+13199, IF_AMD|IF_SSE5},
    /* 3402 */ {I_VPSHLW, 3, {XMMREG,XMMREG,RM_XMM|BITS128,0,0}, nasm_bytecodes+13206, IF_AMD|IF_SSE5},
    /* 3403 */ {I_VPSHLW, 2, {XMMREG,RM_XMM|BITS128,0,0,0}, nasm_bytecodes+13213, IF_AMD|IF_SSE5},
    /* 3404 */ {I_HINT_NOP0, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15638, IF_P6|IF_UNDOC},
    /* 3405 */ {I_HINT_NOP0, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15644, IF_P6|IF_UNDOC},
    /* 3406 */ {I_HINT_NOP0, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15650, IF_X64|IF_UNDOC},
    /* 3407 */ {I_HINT_NOP1, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15656, IF_P6|IF_UNDOC},
    /* 3408 */ {I_HINT_NOP1, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15662, IF_P6|IF_UNDOC},
    /* 3409 */ {I_HINT_NOP1, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15668, IF_X64|IF_UNDOC},
    /* 3410 */ {I_HINT_NOP2, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15674, IF_P6|IF_UNDOC},
    /* 3411 */ {I_HINT_NOP2, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15680, IF_P6|IF_UNDOC},
    /* 3412 */ {I_HINT_NOP2, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15686, IF_X64|IF_UNDOC},
    /* 3413 */ {I_HINT_NOP3, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15692, IF_P6|IF_UNDOC},
    /* 3414 */ {I_HINT_NOP3, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15698, IF_P6|IF_UNDOC},
    /* 3415 */ {I_HINT_NOP3, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15704, IF_X64|IF_UNDOC},
    /* 3416 */ {I_HINT_NOP4, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15710, IF_P6|IF_UNDOC},
    /* 3417 */ {I_HINT_NOP4, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15716, IF_P6|IF_UNDOC},
    /* 3418 */ {I_HINT_NOP4, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15722, IF_X64|IF_UNDOC},
    /* 3419 */ {I_HINT_NOP5, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15728, IF_P6|IF_UNDOC},
    /* 3420 */ {I_HINT_NOP5, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15734, IF_P6|IF_UNDOC},
    /* 3421 */ {I_HINT_NOP5, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15740, IF_X64|IF_UNDOC},
    /* 3422 */ {I_HINT_NOP6, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15746, IF_P6|IF_UNDOC},
    /* 3423 */ {I_HINT_NOP6, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15752, IF_P6|IF_UNDOC},
    /* 3424 */ {I_HINT_NOP6, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15758, IF_X64|IF_UNDOC},
    /* 3425 */ {I_HINT_NOP7, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15764, IF_P6|IF_UNDOC},
    /* 3426 */ {I_HINT_NOP7, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15770, IF_P6|IF_UNDOC},
    /* 3427 */ {I_HINT_NOP7, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15776, IF_X64|IF_UNDOC},
    /* 3428 */ {I_HINT_NOP8, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15782, IF_P6|IF_UNDOC},
    /* 3429 */ {I_HINT_NOP8, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15788, IF_P6|IF_UNDOC},
    /* 3430 */ {I_HINT_NOP8, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15794, IF_X64|IF_UNDOC},
    /* 3431 */ {I_HINT_NOP9, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15800, IF_P6|IF_UNDOC},
    /* 3432 */ {I_HINT_NOP9, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15806, IF_P6|IF_UNDOC},
    /* 3433 */ {I_HINT_NOP9, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15812, IF_X64|IF_UNDOC},
    /* 3434 */ {I_HINT_NOP10, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15818, IF_P6|IF_UNDOC},
    /* 3435 */ {I_HINT_NOP10, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15824, IF_P6|IF_UNDOC},
    /* 3436 */ {I_HINT_NOP10, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15830, IF_X64|IF_UNDOC},
    /* 3437 */ {I_HINT_NOP11, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15836, IF_P6|IF_UNDOC},
    /* 3438 */ {I_HINT_NOP11, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15842, IF_P6|IF_UNDOC},
    /* 3439 */ {I_HINT_NOP11, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15848, IF_X64|IF_UNDOC},
    /* 3440 */ {I_HINT_NOP12, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15854, IF_P6|IF_UNDOC},
    /* 3441 */ {I_HINT_NOP12, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15860, IF_P6|IF_UNDOC},
    /* 3442 */ {I_HINT_NOP12, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15866, IF_X64|IF_UNDOC},
    /* 3443 */ {I_HINT_NOP13, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15872, IF_P6|IF_UNDOC},
    /* 3444 */ {I_HINT_NOP13, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15878, IF_P6|IF_UNDOC},
    /* 3445 */ {I_HINT_NOP13, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15884, IF_X64|IF_UNDOC},
    /* 3446 */ {I_HINT_NOP14, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15890, IF_P6|IF_UNDOC},
    /* 3447 */ {I_HINT_NOP14, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15896, IF_P6|IF_UNDOC},
    /* 3448 */ {I_HINT_NOP14, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15902, IF_X64|IF_UNDOC},
    /* 3449 */ {I_HINT_NOP15, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15908, IF_P6|IF_UNDOC},
    /* 3450 */ {I_HINT_NOP15, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15914, IF_P6|IF_UNDOC},
    /* 3451 */ {I_HINT_NOP15, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15920, IF_X64|IF_UNDOC},
    /* 3452 */ {I_HINT_NOP16, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15926, IF_P6|IF_UNDOC},
    /* 3453 */ {I_HINT_NOP16, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15932, IF_P6|IF_UNDOC},
    /* 3454 */ {I_HINT_NOP16, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15938, IF_X64|IF_UNDOC},
    /* 3455 */ {I_HINT_NOP17, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15944, IF_P6|IF_UNDOC},
    /* 3456 */ {I_HINT_NOP17, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15950, IF_P6|IF_UNDOC},
    /* 3457 */ {I_HINT_NOP17, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15956, IF_X64|IF_UNDOC},
    /* 3458 */ {I_HINT_NOP18, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15962, IF_P6|IF_UNDOC},
    /* 3459 */ {I_HINT_NOP18, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15968, IF_P6|IF_UNDOC},
    /* 3460 */ {I_HINT_NOP18, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15974, IF_X64|IF_UNDOC},
    /* 3461 */ {I_HINT_NOP19, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15980, IF_P6|IF_UNDOC},
    /* 3462 */ {I_HINT_NOP19, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+15986, IF_P6|IF_UNDOC},
    /* 3463 */ {I_HINT_NOP19, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+15992, IF_X64|IF_UNDOC},
    /* 3464 */ {I_HINT_NOP20, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+15998, IF_P6|IF_UNDOC},
    /* 3465 */ {I_HINT_NOP20, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16004, IF_P6|IF_UNDOC},
    /* 3466 */ {I_HINT_NOP20, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16010, IF_X64|IF_UNDOC},
    /* 3467 */ {I_HINT_NOP21, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16016, IF_P6|IF_UNDOC},
    /* 3468 */ {I_HINT_NOP21, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16022, IF_P6|IF_UNDOC},
    /* 3469 */ {I_HINT_NOP21, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16028, IF_X64|IF_UNDOC},
    /* 3470 */ {I_HINT_NOP22, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16034, IF_P6|IF_UNDOC},
    /* 3471 */ {I_HINT_NOP22, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16040, IF_P6|IF_UNDOC},
    /* 3472 */ {I_HINT_NOP22, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16046, IF_X64|IF_UNDOC},
    /* 3473 */ {I_HINT_NOP23, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16052, IF_P6|IF_UNDOC},
    /* 3474 */ {I_HINT_NOP23, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16058, IF_P6|IF_UNDOC},
    /* 3475 */ {I_HINT_NOP23, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16064, IF_X64|IF_UNDOC},
    /* 3476 */ {I_HINT_NOP24, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16070, IF_P6|IF_UNDOC},
    /* 3477 */ {I_HINT_NOP24, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16076, IF_P6|IF_UNDOC},
    /* 3478 */ {I_HINT_NOP24, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16082, IF_X64|IF_UNDOC},
    /* 3479 */ {I_HINT_NOP25, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16088, IF_P6|IF_UNDOC},
    /* 3480 */ {I_HINT_NOP25, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16094, IF_P6|IF_UNDOC},
    /* 3481 */ {I_HINT_NOP25, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16100, IF_X64|IF_UNDOC},
    /* 3482 */ {I_HINT_NOP26, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16106, IF_P6|IF_UNDOC},
    /* 3483 */ {I_HINT_NOP26, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16112, IF_P6|IF_UNDOC},
    /* 3484 */ {I_HINT_NOP26, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16118, IF_X64|IF_UNDOC},
    /* 3485 */ {I_HINT_NOP27, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16124, IF_P6|IF_UNDOC},
    /* 3486 */ {I_HINT_NOP27, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16130, IF_P6|IF_UNDOC},
    /* 3487 */ {I_HINT_NOP27, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16136, IF_X64|IF_UNDOC},
    /* 3488 */ {I_HINT_NOP28, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16142, IF_P6|IF_UNDOC},
    /* 3489 */ {I_HINT_NOP28, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16148, IF_P6|IF_UNDOC},
    /* 3490 */ {I_HINT_NOP28, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16154, IF_X64|IF_UNDOC},
    /* 3491 */ {I_HINT_NOP29, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16160, IF_P6|IF_UNDOC},
    /* 3492 */ {I_HINT_NOP29, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16166, IF_P6|IF_UNDOC},
    /* 3493 */ {I_HINT_NOP29, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16172, IF_X64|IF_UNDOC},
    /* 3494 */ {I_HINT_NOP30, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16178, IF_P6|IF_UNDOC},
    /* 3495 */ {I_HINT_NOP30, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16184, IF_P6|IF_UNDOC},
    /* 3496 */ {I_HINT_NOP30, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16190, IF_X64|IF_UNDOC},
    /* 3497 */ {I_HINT_NOP31, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16196, IF_P6|IF_UNDOC},
    /* 3498 */ {I_HINT_NOP31, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16202, IF_P6|IF_UNDOC},
    /* 3499 */ {I_HINT_NOP31, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16208, IF_X64|IF_UNDOC},
    /* 3500 */ {I_HINT_NOP32, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16214, IF_P6|IF_UNDOC},
    /* 3501 */ {I_HINT_NOP32, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16220, IF_P6|IF_UNDOC},
    /* 3502 */ {I_HINT_NOP32, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16226, IF_X64|IF_UNDOC},
    /* 3503 */ {I_HINT_NOP33, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16232, IF_P6|IF_UNDOC},
    /* 3504 */ {I_HINT_NOP33, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16238, IF_P6|IF_UNDOC},
    /* 3505 */ {I_HINT_NOP33, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16244, IF_X64|IF_UNDOC},
    /* 3506 */ {I_HINT_NOP34, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16250, IF_P6|IF_UNDOC},
    /* 3507 */ {I_HINT_NOP34, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16256, IF_P6|IF_UNDOC},
    /* 3508 */ {I_HINT_NOP34, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16262, IF_X64|IF_UNDOC},
    /* 3509 */ {I_HINT_NOP35, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16268, IF_P6|IF_UNDOC},
    /* 3510 */ {I_HINT_NOP35, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16274, IF_P6|IF_UNDOC},
    /* 3511 */ {I_HINT_NOP35, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16280, IF_X64|IF_UNDOC},
    /* 3512 */ {I_HINT_NOP36, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16286, IF_P6|IF_UNDOC},
    /* 3513 */ {I_HINT_NOP36, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16292, IF_P6|IF_UNDOC},
    /* 3514 */ {I_HINT_NOP36, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16298, IF_X64|IF_UNDOC},
    /* 3515 */ {I_HINT_NOP37, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16304, IF_P6|IF_UNDOC},
    /* 3516 */ {I_HINT_NOP37, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16310, IF_P6|IF_UNDOC},
    /* 3517 */ {I_HINT_NOP37, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16316, IF_X64|IF_UNDOC},
    /* 3518 */ {I_HINT_NOP38, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16322, IF_P6|IF_UNDOC},
    /* 3519 */ {I_HINT_NOP38, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16328, IF_P6|IF_UNDOC},
    /* 3520 */ {I_HINT_NOP38, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16334, IF_X64|IF_UNDOC},
    /* 3521 */ {I_HINT_NOP39, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16340, IF_P6|IF_UNDOC},
    /* 3522 */ {I_HINT_NOP39, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16346, IF_P6|IF_UNDOC},
    /* 3523 */ {I_HINT_NOP39, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16352, IF_X64|IF_UNDOC},
    /* 3524 */ {I_HINT_NOP40, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16358, IF_P6|IF_UNDOC},
    /* 3525 */ {I_HINT_NOP40, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16364, IF_P6|IF_UNDOC},
    /* 3526 */ {I_HINT_NOP40, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16370, IF_X64|IF_UNDOC},
    /* 3527 */ {I_HINT_NOP41, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16376, IF_P6|IF_UNDOC},
    /* 3528 */ {I_HINT_NOP41, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16382, IF_P6|IF_UNDOC},
    /* 3529 */ {I_HINT_NOP41, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16388, IF_X64|IF_UNDOC},
    /* 3530 */ {I_HINT_NOP42, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16394, IF_P6|IF_UNDOC},
    /* 3531 */ {I_HINT_NOP42, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16400, IF_P6|IF_UNDOC},
    /* 3532 */ {I_HINT_NOP42, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16406, IF_X64|IF_UNDOC},
    /* 3533 */ {I_HINT_NOP43, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16412, IF_P6|IF_UNDOC},
    /* 3534 */ {I_HINT_NOP43, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16418, IF_P6|IF_UNDOC},
    /* 3535 */ {I_HINT_NOP43, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16424, IF_X64|IF_UNDOC},
    /* 3536 */ {I_HINT_NOP44, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16430, IF_P6|IF_UNDOC},
    /* 3537 */ {I_HINT_NOP44, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16436, IF_P6|IF_UNDOC},
    /* 3538 */ {I_HINT_NOP44, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16442, IF_X64|IF_UNDOC},
    /* 3539 */ {I_HINT_NOP45, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16448, IF_P6|IF_UNDOC},
    /* 3540 */ {I_HINT_NOP45, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16454, IF_P6|IF_UNDOC},
    /* 3541 */ {I_HINT_NOP45, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16460, IF_X64|IF_UNDOC},
    /* 3542 */ {I_HINT_NOP46, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16466, IF_P6|IF_UNDOC},
    /* 3543 */ {I_HINT_NOP46, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16472, IF_P6|IF_UNDOC},
    /* 3544 */ {I_HINT_NOP46, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16478, IF_X64|IF_UNDOC},
    /* 3545 */ {I_HINT_NOP47, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16484, IF_P6|IF_UNDOC},
    /* 3546 */ {I_HINT_NOP47, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16490, IF_P6|IF_UNDOC},
    /* 3547 */ {I_HINT_NOP47, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16496, IF_X64|IF_UNDOC},
    /* 3548 */ {I_HINT_NOP48, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16502, IF_P6|IF_UNDOC},
    /* 3549 */ {I_HINT_NOP48, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16508, IF_P6|IF_UNDOC},
    /* 3550 */ {I_HINT_NOP48, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16514, IF_X64|IF_UNDOC},
    /* 3551 */ {I_HINT_NOP49, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16520, IF_P6|IF_UNDOC},
    /* 3552 */ {I_HINT_NOP49, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16526, IF_P6|IF_UNDOC},
    /* 3553 */ {I_HINT_NOP49, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16532, IF_X64|IF_UNDOC},
    /* 3554 */ {I_HINT_NOP50, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16538, IF_P6|IF_UNDOC},
    /* 3555 */ {I_HINT_NOP50, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16544, IF_P6|IF_UNDOC},
    /* 3556 */ {I_HINT_NOP50, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16550, IF_X64|IF_UNDOC},
    /* 3557 */ {I_HINT_NOP51, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16556, IF_P6|IF_UNDOC},
    /* 3558 */ {I_HINT_NOP51, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16562, IF_P6|IF_UNDOC},
    /* 3559 */ {I_HINT_NOP51, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16568, IF_X64|IF_UNDOC},
    /* 3560 */ {I_HINT_NOP52, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16574, IF_P6|IF_UNDOC},
    /* 3561 */ {I_HINT_NOP52, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16580, IF_P6|IF_UNDOC},
    /* 3562 */ {I_HINT_NOP52, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16586, IF_X64|IF_UNDOC},
    /* 3563 */ {I_HINT_NOP53, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16592, IF_P6|IF_UNDOC},
    /* 3564 */ {I_HINT_NOP53, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16598, IF_P6|IF_UNDOC},
    /* 3565 */ {I_HINT_NOP53, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16604, IF_X64|IF_UNDOC},
    /* 3566 */ {I_HINT_NOP54, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16610, IF_P6|IF_UNDOC},
    /* 3567 */ {I_HINT_NOP54, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16616, IF_P6|IF_UNDOC},
    /* 3568 */ {I_HINT_NOP54, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16622, IF_X64|IF_UNDOC},
    /* 3569 */ {I_HINT_NOP55, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16628, IF_P6|IF_UNDOC},
    /* 3570 */ {I_HINT_NOP55, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16634, IF_P6|IF_UNDOC},
    /* 3571 */ {I_HINT_NOP55, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16640, IF_X64|IF_UNDOC},
    /* 3572 */ {I_HINT_NOP56, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+13976, IF_P6|IF_UNDOC},
    /* 3573 */ {I_HINT_NOP56, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+13982, IF_P6|IF_UNDOC},
    /* 3574 */ {I_HINT_NOP56, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+13988, IF_X64|IF_UNDOC},
    /* 3575 */ {I_HINT_NOP57, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16646, IF_P6|IF_UNDOC},
    /* 3576 */ {I_HINT_NOP57, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16652, IF_P6|IF_UNDOC},
    /* 3577 */ {I_HINT_NOP57, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16658, IF_X64|IF_UNDOC},
    /* 3578 */ {I_HINT_NOP58, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16664, IF_P6|IF_UNDOC},
    /* 3579 */ {I_HINT_NOP58, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16670, IF_P6|IF_UNDOC},
    /* 3580 */ {I_HINT_NOP58, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16676, IF_X64|IF_UNDOC},
    /* 3581 */ {I_HINT_NOP59, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16682, IF_P6|IF_UNDOC},
    /* 3582 */ {I_HINT_NOP59, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16688, IF_P6|IF_UNDOC},
    /* 3583 */ {I_HINT_NOP59, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16694, IF_X64|IF_UNDOC},
    /* 3584 */ {I_HINT_NOP60, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16700, IF_P6|IF_UNDOC},
    /* 3585 */ {I_HINT_NOP60, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16706, IF_P6|IF_UNDOC},
    /* 3586 */ {I_HINT_NOP60, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16712, IF_X64|IF_UNDOC},
    /* 3587 */ {I_HINT_NOP61, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16718, IF_P6|IF_UNDOC},
    /* 3588 */ {I_HINT_NOP61, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16724, IF_P6|IF_UNDOC},
    /* 3589 */ {I_HINT_NOP61, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16730, IF_X64|IF_UNDOC},
    /* 3590 */ {I_HINT_NOP62, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16736, IF_P6|IF_UNDOC},
    /* 3591 */ {I_HINT_NOP62, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16742, IF_P6|IF_UNDOC},
    /* 3592 */ {I_HINT_NOP62, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16748, IF_X64|IF_UNDOC},
    /* 3593 */ {I_HINT_NOP63, 1, {RM_GPR|BITS16,0,0,0,0}, nasm_bytecodes+16754, IF_P6|IF_UNDOC},
    /* 3594 */ {I_HINT_NOP63, 1, {RM_GPR|BITS32,0,0,0,0}, nasm_bytecodes+16760, IF_P6|IF_UNDOC},
    /* 3595 */ {I_HINT_NOP63, 1, {RM_GPR|BITS64,0,0,0,0}, nasm_bytecodes+16766, IF_X64|IF_UNDOC},
};

static const struct itemplate * const itable_00[] = {
    instrux + 40,
    instrux + 41,
};

static const struct itemplate * const itable_01[] = {
    instrux + 42,
    instrux + 43,
    instrux + 44,
    instrux + 45,
    instrux + 46,
    instrux + 47,
};

static const struct itemplate * const itable_02[] = {
    instrux + 48,
    instrux + 49,
};

static const struct itemplate * const itable_03[] = {
    instrux + 50,
    instrux + 51,
    instrux + 52,
    instrux + 53,
    instrux + 54,
    instrux + 55,
};

static const struct itemplate * const itable_04[] = {
    instrux + 59,
};

static const struct itemplate * const itable_05[] = {
    instrux + 61,
    instrux + 63,
    instrux + 65,
};

static const struct itemplate * const itable_06[] = {
    instrux + 869,
    instrux + 870,
};

static const struct itemplate * const itable_07[] = {
    instrux + 821,
};

static const struct itemplate * const itable_08[] = {
    instrux + 721,
    instrux + 722,
};

static const struct itemplate * const itable_09[] = {
    instrux + 723,
    instrux + 724,
    instrux + 725,
    instrux + 726,
    instrux + 727,
    instrux + 728,
};

static const struct itemplate * const itable_0A[] = {
    instrux + 729,
    instrux + 730,
};

static const struct itemplate * const itable_0B[] = {
    instrux + 731,
    instrux + 732,
    instrux + 733,
    instrux + 734,
    instrux + 735,
    instrux + 736,
};

static const struct itemplate * const itable_0C[] = {
    instrux + 740,
};

static const struct itemplate * const itable_0D[] = {
    instrux + 742,
    instrux + 744,
    instrux + 746,
};

static const struct itemplate * const itable_0E[] = {
    instrux + 869,
    instrux + 870,
};

static const struct itemplate * const itable_0F00[] = {
    instrux + 545,
    instrux + 546,
    instrux + 575,
    instrux + 576,
    instrux + 577,
    instrux + 620,
    instrux + 621,
    instrux + 622,
    instrux + 1051,
    instrux + 1052,
    instrux + 1053,
    instrux + 1054,
    instrux + 1055,
    instrux + 1070,
    instrux + 1071,
    instrux + 1072,
    instrux + 1073,
    instrux + 1074,
    instrux + 1141,
    instrux + 1142,
    instrux + 1143,
    instrux + 1144,
    instrux + 1145,
    instrux + 1146,
};

static const struct itemplate * const itable_0F01[] = {
    instrux + 192,
    instrux + 505,
    instrux + 506,
    instrux + 507,
    instrux + 508,
    instrux + 509,
    instrux + 570,
    instrux + 574,
    instrux + 578,
    instrux + 579,
    instrux + 580,
    instrux + 624,
    instrux + 708,
    instrux + 914,
    instrux + 1001,
    instrux + 1050,
    instrux + 1056,
    instrux + 1058,
    instrux + 1059,
    instrux + 1060,
    instrux + 1061,
    instrux + 1064,
    instrux + 1110,
    instrux + 1315,
    instrux + 1316,
    instrux + 1551,
    instrux + 1553,
    instrux + 1554,
    instrux + 1555,
    instrux + 1560,
    instrux + 1561,
    instrux + 1562,
    instrux + 1565,
};

static const struct itemplate * const itable_0F02[] = {
    instrux + 548,
    instrux + 549,
    instrux + 550,
    instrux + 551,
    instrux + 552,
    instrux + 553,
    instrux + 554,
    instrux + 555,
    instrux + 556,
    instrux + 557,
};

static const struct itemplate * const itable_0F03[] = {
    instrux + 607,
    instrux + 608,
    instrux + 609,
    instrux + 610,
    instrux + 611,
    instrux + 612,
    instrux + 613,
    instrux + 614,
    instrux + 615,
    instrux + 616,
};

static const struct itemplate * const itable_0F05[] = {
    instrux + 582,
    instrux + 1111,
};

static const struct itemplate * const itable_0F06[] = {
    instrux + 194,
};

static const struct itemplate * const itable_0F07[] = {
    instrux + 581,
    instrux + 1114,
};

static const struct itemplate * const itable_0F08[] = {
    instrux + 504,
};

static const struct itemplate * const itable_0F09[] = {
    instrux + 1148,
};

static const struct itemplate * const itable_0F0B[] = {
    instrux + 1140,
};

static const struct itemplate * const itable_0F0D[] = {
    instrux + 831,
    instrux + 832,
};

static const struct itemplate * const itable_0F0E[] = {
    instrux + 330,
};

static const struct itemplate * const itable_0F0F[] = {
    instrux + 778,
    instrux + 786,
    instrux + 787,
    instrux + 788,
    instrux + 789,
    instrux + 790,
    instrux + 791,
    instrux + 792,
    instrux + 793,
    instrux + 794,
    instrux + 795,
    instrux + 796,
    instrux + 797,
    instrux + 798,
    instrux + 799,
    instrux + 800,
    instrux + 801,
    instrux + 802,
    instrux + 807,
    instrux + 1344,
    instrux + 1345,
    instrux + 1346,
    instrux + 1347,
    instrux + 1348,
    instrux + 1684,
    instrux + 1685,
};

static const struct itemplate * const itable_0F10[] = {
    instrux + 1285,
    instrux + 1287,
    instrux + 1289,
    instrux + 1291,
    instrux + 1520,
    instrux + 1523,
    instrux + 1524,
    instrux + 1527,
};

static const struct itemplate * const itable_0F11[] = {
    instrux + 1286,
    instrux + 1288,
    instrux + 1290,
    instrux + 1292,
    instrux + 1521,
    instrux + 1522,
    instrux + 1525,
    instrux + 1526,
};

static const struct itemplate * const itable_0F12[] = {
    instrux + 1279,
    instrux + 1281,
    instrux + 1517,
    instrux + 1548,
    instrux + 1550,
};

static const struct itemplate * const itable_0F13[] = {
    instrux + 1280,
    instrux + 1516,
};

static const struct itemplate * const itable_0F14[] = {
    instrux + 1309,
    instrux + 1539,
};

static const struct itemplate * const itable_0F15[] = {
    instrux + 1308,
    instrux + 1538,
};

static const struct itemplate * const itable_0F16[] = {
    instrux + 1276,
    instrux + 1278,
    instrux + 1515,
    instrux + 1549,
};

static const struct itemplate * const itable_0F17[] = {
    instrux + 1277,
    instrux + 1514,
};

static const struct itemplate * const itable_0F18[] = {
    instrux + 1323,
    instrux + 1324,
    instrux + 1325,
    instrux + 1326,
    instrux + 3404,
    instrux + 3405,
    instrux + 3406,
    instrux + 3407,
    instrux + 3408,
    instrux + 3409,
    instrux + 3410,
    instrux + 3411,
    instrux + 3412,
    instrux + 3413,
    instrux + 3414,
    instrux + 3415,
    instrux + 3416,
    instrux + 3417,
    instrux + 3418,
    instrux + 3419,
    instrux + 3420,
    instrux + 3421,
    instrux + 3422,
    instrux + 3423,
    instrux + 3424,
    instrux + 3425,
    instrux + 3426,
    instrux + 3427,
};

static const struct itemplate * const itable_0F19[] = {
    instrux + 3428,
    instrux + 3429,
    instrux + 3430,
    instrux + 3431,
    instrux + 3432,
    instrux + 3433,
    instrux + 3434,
    instrux + 3435,
    instrux + 3436,
    instrux + 3437,
    instrux + 3438,
    instrux + 3439,
    instrux + 3440,
    instrux + 3441,
    instrux + 3442,
    instrux + 3443,
    instrux + 3444,
    instrux + 3445,
    instrux + 3446,
    instrux + 3447,
    instrux + 3448,
    instrux + 3449,
    instrux + 3450,
    instrux + 3451,
};

static const struct itemplate * const itable_0F1A[] = {
    instrux + 3452,
    instrux + 3453,
    instrux + 3454,
    instrux + 3455,
    instrux + 3456,
    instrux + 3457,
    instrux + 3458,
    instrux + 3459,
    instrux + 3460,
    instrux + 3461,
    instrux + 3462,
    instrux + 3463,
    instrux + 3464,
    instrux + 3465,
    instrux + 3466,
    instrux + 3467,
    instrux + 3468,
    instrux + 3469,
    instrux + 3470,
    instrux + 3471,
    instrux + 3472,
    instrux + 3473,
    instrux + 3474,
    instrux + 3475,
};

static const struct itemplate * const itable_0F1B[] = {
    instrux + 3476,
    instrux + 3477,
    instrux + 3478,
    instrux + 3479,
    instrux + 3480,
    instrux + 3481,
    instrux + 3482,
    instrux + 3483,
    instrux + 3484,
    instrux + 3485,
    instrux + 3486,
    instrux + 3487,
    instrux + 3488,
    instrux + 3489,
    instrux + 3490,
    instrux + 3491,
    instrux + 3492,
    instrux + 3493,
    instrux + 3494,
    instrux + 3495,
    instrux + 3496,
    instrux + 3497,
    instrux + 3498,
    instrux + 3499,
};

static const struct itemplate * const itable_0F1C[] = {
    instrux + 3500,
    instrux + 3501,
    instrux + 3502,
    instrux + 3503,
    instrux + 3504,
    instrux + 3505,
    instrux + 3506,
    instrux + 3507,
    instrux + 3508,
    instrux + 3509,
    instrux + 3510,
    instrux + 3511,
    instrux + 3512,
    instrux + 3513,
    instrux + 3514,
    instrux + 3515,
    instrux + 3516,
    instrux + 3517,
    instrux + 3518,
    instrux + 3519,
    instrux + 3520,
    instrux + 3521,
    instrux + 3522,
    instrux + 3523,
};

static const struct itemplate * const itable_0F1D[] = {
    instrux + 3524,
    instrux + 3525,
    instrux + 3526,
    instrux + 3527,
    instrux + 3528,
    instrux + 3529,
    instrux + 3530,
    instrux + 3531,
    instrux + 3532,
    instrux + 3533,
    instrux + 3534,
    instrux + 3535,
    instrux + 3536,
    instrux + 3537,
    instrux + 3538,
    instrux + 3539,
    instrux + 3540,
    instrux + 3541,
    instrux + 3542,
    instrux + 3543,
    instrux + 3544,
    instrux + 3545,
    instrux + 3546,
    instrux + 3547,
};

static const struct itemplate * const itable_0F1E[] = {
    instrux + 3548,
    instrux + 3549,
    instrux + 3550,
    instrux + 3551,
    instrux + 3552,
    instrux + 3553,
    instrux + 3554,
    instrux + 3555,
    instrux + 3556,
    instrux + 3557,
    instrux + 3558,
    instrux + 3559,
    instrux + 3560,
    instrux + 3561,
    instrux + 3562,
    instrux + 3563,
    instrux + 3564,
    instrux + 3565,
    instrux + 3566,
    instrux + 3567,
    instrux + 3568,
    instrux + 3569,
    instrux + 3570,
    instrux + 3571,
};

static const struct itemplate * const itable_0F1F[] = {
    instrux + 714,
    instrux + 715,
    instrux + 716,
    instrux + 3572,
    instrux + 3573,
    instrux + 3574,
    instrux + 3575,
    instrux + 3576,
    instrux + 3577,
    instrux + 3578,
    instrux + 3579,
    instrux + 3580,
    instrux + 3581,
    instrux + 3582,
    instrux + 3583,
    instrux + 3584,
    instrux + 3585,
    instrux + 3586,
    instrux + 3587,
    instrux + 3588,
    instrux + 3589,
    instrux + 3590,
    instrux + 3591,
    instrux + 3592,
    instrux + 3593,
    instrux + 3594,
    instrux + 3595,
};

static const struct itemplate * const itable_0F20[] = {
    instrux + 639,
    instrux + 640,
};

static const struct itemplate * const itable_0F21[] = {
    instrux + 643,
    instrux + 644,
};

static const struct itemplate * const itable_0F22[] = {
    instrux + 641,
    instrux + 642,
};

static const struct itemplate * const itable_0F23[] = {
    instrux + 645,
    instrux + 646,
};

static const struct itemplate * const itable_0F28[] = {
    instrux + 1272,
    instrux + 1274,
    instrux + 1510,
    instrux + 1513,
};

static const struct itemplate * const itable_0F29[] = {
    instrux + 1273,
    instrux + 1275,
    instrux + 1511,
    instrux + 1512,
};

static const struct itemplate * const itable_0F2A[] = {
    instrux + 1254,
    instrux + 1256,
    instrux + 1257,
    instrux + 1486,
    instrux + 1494,
    instrux + 1495,
};

static const struct itemplate * const itable_0F2B[] = {
    instrux + 1284,
    instrux + 1354,
    instrux + 1607,
    instrux + 1608,
};

static const struct itemplate * const itable_0F2C[] = {
    instrux + 1262,
    instrux + 1263,
    instrux + 1264,
    instrux + 1497,
    instrux + 1500,
    instrux + 1501,
    instrux + 1502,
    instrux + 1503,
};

static const struct itemplate * const itable_0F2D[] = {
    instrux + 1255,
    instrux + 1258,
    instrux + 1259,
    instrux + 1260,
    instrux + 1261,
    instrux + 1484,
    instrux + 1489,
    instrux + 1490,
    instrux + 1491,
    instrux + 1492,
};

static const struct itemplate * const itable_0F2E[] = {
    instrux + 1307,
    instrux + 1537,
};

static const struct itemplate * const itable_0F2F[] = {
    instrux + 1253,
    instrux + 1480,
};

static const struct itemplate * const itable_0F30[] = {
    instrux + 1150,
};

static const struct itemplate * const itable_0F31[] = {
    instrux + 913,
};

static const struct itemplate * const itable_0F32[] = {
    instrux + 911,
};

static const struct itemplate * const itable_0F33[] = {
    instrux + 912,
};

static const struct itemplate * const itable_0F34[] = {
    instrux + 1112,
};

static const struct itemplate * const itable_0F35[] = {
    instrux + 1113,
};

static const struct itemplate * const itable_0F36[] = {
    instrux + 910,
};

static const struct itemplate * const itable_0F37[] = {
    instrux + 1149,
    instrux + 1683,
};

static const struct itemplate * const itable_0F3800[] = {
    instrux + 1595,
    instrux + 1596,
};

static const struct itemplate * const itable_0F3801[] = {
    instrux + 1579,
    instrux + 1580,
};

static const struct itemplate * const itable_0F3802[] = {
    instrux + 1581,
    instrux + 1582,
};

static const struct itemplate * const itable_0F3803[] = {
    instrux + 1583,
    instrux + 1584,
};

static const struct itemplate * const itable_0F3804[] = {
    instrux + 1591,
    instrux + 1592,
};

static const struct itemplate * const itable_0F3805[] = {
    instrux + 1585,
    instrux + 1586,
};

static const struct itemplate * const itable_0F3806[] = {
    instrux + 1587,
    instrux + 1588,
};

static const struct itemplate * const itable_0F3807[] = {
    instrux + 1589,
    instrux + 1590,
};

static const struct itemplate * const itable_0F3808[] = {
    instrux + 1597,
    instrux + 1598,
};

static const struct itemplate * const itable_0F3809[] = {
    instrux + 1599,
    instrux + 1600,
};

static const struct itemplate * const itable_0F380A[] = {
    instrux + 1601,
    instrux + 1602,
};

static const struct itemplate * const itable_0F380B[] = {
    instrux + 1593,
    instrux + 1594,
};

static const struct itemplate * const itable_0F3810[] = {
    instrux + 1624,
};

static const struct itemplate * const itable_0F3814[] = {
    instrux + 1615,
};

static const struct itemplate * const itable_0F3815[] = {
    instrux + 1614,
};

static const struct itemplate * const itable_0F3817[] = {
    instrux + 1665,
};

static const struct itemplate * const itable_0F381C[] = {
    instrux + 1571,
    instrux + 1572,
};

static const struct itemplate * const itable_0F381D[] = {
    instrux + 1573,
    instrux + 1574,
};

static const struct itemplate * const itable_0F381E[] = {
    instrux + 1575,
    instrux + 1576,
};

static const struct itemplate * const itable_0F3820[] = {
    instrux + 1651,
};

static const struct itemplate * const itable_0F3821[] = {
    instrux + 1652,
};

static const struct itemplate * const itable_0F3822[] = {
    instrux + 1653,
};

static const struct itemplate * const itable_0F3823[] = {
    instrux + 1654,
};

static const struct itemplate * const itable_0F3824[] = {
    instrux + 1655,
};

static const struct itemplate * const itable_0F3825[] = {
    instrux + 1656,
};

static const struct itemplate * const itable_0F3828[] = {
    instrux + 1663,
};

static const struct itemplate * const itable_0F3829[] = {
    instrux + 1626,
};

static const struct itemplate * const itable_0F382A[] = {
    instrux + 1621,
};

static const struct itemplate * const itable_0F382B[] = {
    instrux + 1623,
};

static const struct itemplate * const itable_0F3830[] = {
    instrux + 1657,
};

static const struct itemplate * const itable_0F3831[] = {
    instrux + 1658,
};

static const struct itemplate * const itable_0F3832[] = {
    instrux + 1659,
};

static const struct itemplate * const itable_0F3833[] = {
    instrux + 1660,
};

static const struct itemplate * const itable_0F3834[] = {
    instrux + 1661,
};

static const struct itemplate * const itable_0F3835[] = {
    instrux + 1662,
};

static const struct itemplate * const itable_0F3837[] = {
    instrux + 1679,
};

static const struct itemplate * const itable_0F3838[] = {
    instrux + 1647,
};

static const struct itemplate * const itable_0F3839[] = {
    instrux + 1648,
};

static const struct itemplate * const itable_0F383A[] = {
    instrux + 1650,
};

static const struct itemplate * const itable_0F383B[] = {
    instrux + 1649,
};

static const struct itemplate * const itable_0F383C[] = {
    instrux + 1643,
};

static const struct itemplate * const itable_0F383D[] = {
    instrux + 1644,
};

static const struct itemplate * const itable_0F383E[] = {
    instrux + 1646,
};

static const struct itemplate * const itable_0F383F[] = {
    instrux + 1645,
};

static const struct itemplate * const itable_0F3840[] = {
    instrux + 1664,
};

static const struct itemplate * const itable_0F3841[] = {
    instrux + 1635,
};

static const struct itemplate * const itable_0F3880[] = {
    instrux + 1567,
    instrux + 1568,
};

static const struct itemplate * const itable_0F3881[] = {
    instrux + 1569,
    instrux + 1570,
};

static const struct itemplate * const itable_0F38DB[] = {
    instrux + 1696,
};

static const struct itemplate * const itable_0F38DC[] = {
    instrux + 1692,
};

static const struct itemplate * const itable_0F38DD[] = {
    instrux + 1693,
};

static const struct itemplate * const itable_0F38DE[] = {
    instrux + 1694,
};

static const struct itemplate * const itable_0F38DF[] = {
    instrux + 1695,
};

static const struct itemplate * const itable_0F38F0[] = {
    instrux + 1670,
    instrux + 1673,
    instrux + 1686,
    instrux + 1687,
    instrux + 1688,
};

static const struct itemplate * const itable_0F38F1[] = {
    instrux + 1671,
    instrux + 1672,
    instrux + 1674,
    instrux + 1689,
    instrux + 1690,
    instrux + 1691,
};

static const struct itemplate * const itable_0F39[] = {
    instrux + 261,
};

static const struct itemplate * const itable_0F3A08[] = {
    instrux + 1667,
};

static const struct itemplate * const itable_0F3A09[] = {
    instrux + 1666,
};

static const struct itemplate * const itable_0F3A0A[] = {
    instrux + 1669,
};

static const struct itemplate * const itable_0F3A0B[] = {
    instrux + 1668,
};

static const struct itemplate * const itable_0F3A0C[] = {
    instrux + 1613,
};

static const struct itemplate * const itable_0F3A0D[] = {
    instrux + 1612,
};

static const struct itemplate * const itable_0F3A0E[] = {
    instrux + 1625,
};

static const struct itemplate * const itable_0F3A0F[] = {
    instrux + 1577,
    instrux + 1578,
};

static const struct itemplate * const itable_0F3A14[] = {
    instrux + 1627,
    instrux + 1628,
    instrux + 1629,
};

static const struct itemplate * const itable_0F3A15[] = {
    instrux + 1632,
    instrux + 1633,
    instrux + 1634,
};

static const struct itemplate * const itable_0F3A16[] = {
    instrux + 1630,
    instrux + 1631,
};

static const struct itemplate * const itable_0F3A17[] = {
    instrux + 1618,
    instrux + 1619,
};

static const struct itemplate * const itable_0F3A20[] = {
    instrux + 1636,
    instrux + 1637,
    instrux + 1638,
};

static const struct itemplate * const itable_0F3A21[] = {
    instrux + 1620,
};

static const struct itemplate * const itable_0F3A22[] = {
    instrux + 1639,
    instrux + 1640,
    instrux + 1641,
    instrux + 1642,
};

static const struct itemplate * const itable_0F3A40[] = {
    instrux + 1617,
};

static const struct itemplate * const itable_0F3A41[] = {
    instrux + 1616,
};

static const struct itemplate * const itable_0F3A42[] = {
    instrux + 1622,
};

static const struct itemplate * const itable_0F3A44[] = {
    instrux + 2887,
    instrux + 2888,
    instrux + 2889,
    instrux + 2890,
    instrux + 2891,
};

static const struct itemplate * const itable_0F3A60[] = {
    instrux + 1676,
};

static const struct itemplate * const itable_0F3A61[] = {
    instrux + 1675,
};

static const struct itemplate * const itable_0F3A62[] = {
    instrux + 1678,
};

static const struct itemplate * const itable_0F3A63[] = {
    instrux + 1677,
};

static const struct itemplate * const itable_0F3ADF[] = {
    instrux + 1697,
};

static const struct itemplate * const itable_0F3C[] = {
    instrux + 245,
};

static const struct itemplate * const itable_0F3D[] = {
    instrux + 244,
};

static const struct itemplate * const itable_0F40[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F41[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F42[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F43[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F44[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F45[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F46[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F47[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F48[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F49[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F4A[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F4B[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F4C[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F4D[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F4E[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F4F[] = {
    instrux + 1217,
    instrux + 1218,
    instrux + 1219,
    instrux + 1220,
    instrux + 1221,
    instrux + 1222,
};

static const struct itemplate * const itable_0F50[] = {
    instrux + 777,
    instrux + 1282,
    instrux + 1283,
    instrux + 1518,
    instrux + 1519,
};

static const struct itemplate * const itable_0F51[] = {
    instrux + 769,
    instrux + 1302,
    instrux + 1303,
    instrux + 1533,
    instrux + 1534,
};

static const struct itemplate * const itable_0F52[] = {
    instrux + 805,
    instrux + 1298,
    instrux + 1299,
};

static const struct itemplate * const itable_0F53[] = {
    instrux + 1296,
    instrux + 1297,
};

static const struct itemplate * const itable_0F54[] = {
    instrux + 785,
    instrux + 1232,
    instrux + 1461,
};

static const struct itemplate * const itable_0F55[] = {
    instrux + 852,
    instrux + 1231,
    instrux + 1460,
};

static const struct itemplate * const itable_0F56[] = {
    instrux + 1295,
    instrux + 1530,
};

static const struct itemplate * const itable_0F57[] = {
    instrux + 1310,
    instrux + 1540,
};

static const struct itemplate * const itable_0F58[] = {
    instrux + 814,
    instrux + 1229,
    instrux + 1230,
    instrux + 1458,
    instrux + 1459,
};

static const struct itemplate * const itable_0F59[] = {
    instrux + 808,
    instrux + 1293,
    instrux + 1294,
    instrux + 1528,
    instrux + 1529,
};

static const struct itemplate * const itable_0F5A[] = {
    instrux + 813,
    instrux + 1485,
    instrux + 1488,
    instrux + 1493,
    instrux + 1496,
};

static const struct itemplate * const itable_0F5B[] = {
    instrux + 812,
    instrux + 1482,
    instrux + 1487,
    instrux + 1499,
};

static const struct itemplate * const itable_0F5C[] = {
    instrux + 811,
    instrux + 1305,
    instrux + 1306,
    instrux + 1535,
    instrux + 1536,
};

static const struct itemplate * const itable_0F5D[] = {
    instrux + 806,
    instrux + 1270,
    instrux + 1271,
    instrux + 1508,
    instrux + 1509,
};

static const struct itemplate * const itable_0F5E[] = {
    instrux + 803,
    instrux + 1265,
    instrux + 1266,
    instrux + 1504,
    instrux + 1505,
};

static const struct itemplate * const itable_0F5F[] = {
    instrux + 1268,
    instrux + 1269,
    instrux + 1506,
    instrux + 1507,
};

static const struct itemplate * const itable_0F60[] = {
    instrux + 860,
    instrux + 1453,
};

static const struct itemplate * const itable_0F61[] = {
    instrux + 862,
    instrux + 1454,
};

static const struct itemplate * const itable_0F62[] = {
    instrux + 861,
    instrux + 1455,
};

static const struct itemplate * const itable_0F63[] = {
    instrux + 764,
    instrux + 1377,
};

static const struct itemplate * const itable_0F64[] = {
    instrux + 782,
    instrux + 1396,
};

static const struct itemplate * const itable_0F65[] = {
    instrux + 784,
    instrux + 1397,
};

static const struct itemplate * const itable_0F66[] = {
    instrux + 783,
    instrux + 1398,
};

static const struct itemplate * const itable_0F67[] = {
    instrux + 765,
    instrux + 1379,
};

static const struct itemplate * const itable_0F68[] = {
    instrux + 857,
    instrux + 1449,
};

static const struct itemplate * const itable_0F69[] = {
    instrux + 859,
    instrux + 1450,
};

static const struct itemplate * const itable_0F6A[] = {
    instrux + 858,
    instrux + 1451,
};

static const struct itemplate * const itable_0F6B[] = {
    instrux + 763,
    instrux + 1378,
};

static const struct itemplate * const itable_0F6C[] = {
    instrux + 1456,
};

static const struct itemplate * const itable_0F6D[] = {
    instrux + 1452,
};

static const struct itemplate * const itable_0F6E[] = {
    instrux + 675,
    instrux + 676,
    instrux + 679,
    instrux + 680,
    instrux + 685,
    instrux + 1358,
    instrux + 1359,
    instrux + 1374,
};

static const struct itemplate * const itable_0F6F[] = {
    instrux + 683,
    instrux + 1361,
    instrux + 1363,
    instrux + 1365,
    instrux + 1367,
};

static const struct itemplate * const itable_0F70[] = {
    instrux + 1343,
    instrux + 1416,
    instrux + 1417,
    instrux + 1418,
    instrux + 1419,
    instrux + 1420,
    instrux + 1421,
};

static const struct itemplate * const itable_0F71[] = {
    instrux + 838,
    instrux + 842,
    instrux + 848,
    instrux + 1424,
    instrux + 1430,
    instrux + 1435,
};

static const struct itemplate * const itable_0F72[] = {
    instrux + 834,
    instrux + 840,
    instrux + 844,
    instrux + 1426,
    instrux + 1432,
    instrux + 1437,
};

static const struct itemplate * const itable_0F73[] = {
    instrux + 836,
    instrux + 846,
    instrux + 1422,
    instrux + 1428,
    instrux + 1433,
    instrux + 1439,
};

static const struct itemplate * const itable_0F74[] = {
    instrux + 779,
    instrux + 1393,
};

static const struct itemplate * const itable_0F75[] = {
    instrux + 781,
    instrux + 1394,
};

static const struct itemplate * const itable_0F76[] = {
    instrux + 780,
    instrux + 1395,
};

static const struct itemplate * const itable_0F77[] = {
    instrux + 262,
};

static const struct itemplate * const itable_0F78[] = {
    instrux + 1108,
    instrux + 1558,
    instrux + 1559,
    instrux + 1603,
    instrux + 1605,
};

static const struct itemplate * const itable_0F79[] = {
    instrux + 945,
    instrux + 1563,
    instrux + 1564,
    instrux + 1604,
    instrux + 1606,
};

static const struct itemplate * const itable_0F7B[] = {
    instrux + 946,
};

static const struct itemplate * const itable_0F7C[] = {
    instrux + 1109,
    instrux + 1543,
    instrux + 1544,
};

static const struct itemplate * const itable_0F7D[] = {
    instrux + 948,
    instrux + 1545,
    instrux + 1546,
};

static const struct itemplate * const itable_0F7E[] = {
    instrux + 677,
    instrux + 678,
    instrux + 681,
    instrux + 682,
    instrux + 686,
    instrux + 1357,
    instrux + 1360,
    instrux + 1370,
    instrux + 1373,
    instrux + 1375,
};

static const struct itemplate * const itable_0F7F[] = {
    instrux + 684,
    instrux + 1362,
    instrux + 1364,
    instrux + 1366,
    instrux + 1368,
};

static const struct itemplate * const itable_0F80[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F81[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F82[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F83[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F84[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F85[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F86[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F87[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F88[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F89[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F8A[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F8B[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F8C[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F8D[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F8E[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F8F[] = {
    instrux + 1223,
    instrux + 1224,
    instrux + 1225,
};

static const struct itemplate * const itable_0F90[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F91[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F92[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F93[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F94[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F95[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F96[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F97[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F98[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F99[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F9A[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F9B[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F9C[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F9D[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F9E[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0F9F[] = {
    instrux + 1227,
    instrux + 1228,
};

static const struct itemplate * const itable_0FA0[] = {
    instrux + 871,
};

static const struct itemplate * const itable_0FA1[] = {
    instrux + 822,
};

static const struct itemplate * const itable_0FA2[] = {
    instrux + 243,
};

static const struct itemplate * const itable_0FA3[] = {
    instrux + 124,
    instrux + 125,
    instrux + 126,
    instrux + 127,
    instrux + 128,
    instrux + 129,
};

static const struct itemplate * const itable_0FA4[] = {
    instrux + 1014,
    instrux + 1015,
    instrux + 1016,
    instrux + 1017,
    instrux + 1018,
    instrux + 1019,
};

static const struct itemplate * const itable_0FA5[] = {
    instrux + 1020,
    instrux + 1021,
    instrux + 1022,
    instrux + 1023,
    instrux + 1024,
    instrux + 1025,
};

static const struct itemplate * const itable_0FA6C0[] = {
    instrux + 3115,
};

static const struct itemplate * const itable_0FA6C8[] = {
    instrux + 3116,
};

static const struct itemplate * const itable_0FA6D0[] = {
    instrux + 3117,
};

static const struct itemplate * const itable_0FA7C0[] = {
    instrux + 3109,
};

static const struct itemplate * const itable_0FA7C8[] = {
    instrux + 3110,
};

static const struct itemplate * const itable_0FA7D0[] = {
    instrux + 3111,
};

static const struct itemplate * const itable_0FA7D8[] = {
    instrux + 3112,
};

static const struct itemplate * const itable_0FA7E0[] = {
    instrux + 3113,
};

static const struct itemplate * const itable_0FA7E8[] = {
    instrux + 3114,
};

static const struct itemplate * const itable_0FA8[] = {
    instrux + 871,
};

static const struct itemplate * const itable_0FA9[] = {
    instrux + 822,
};

static const struct itemplate * const itable_0FAA[] = {
    instrux + 947,
};

static const struct itemplate * const itable_0FAB[] = {
    instrux + 151,
    instrux + 152,
    instrux + 153,
    instrux + 154,
    instrux + 155,
    instrux + 156,
};

static const struct itemplate * const itable_0FAC[] = {
    instrux + 1038,
    instrux + 1039,
    instrux + 1040,
    instrux + 1041,
    instrux + 1042,
    instrux + 1043,
};

static const struct itemplate * const itable_0FAD[] = {
    instrux + 1044,
    instrux + 1045,
    instrux + 1046,
    instrux + 1047,
    instrux + 1048,
    instrux + 1049,
};

static const struct itemplate * const itable_0FAE[] = {
    instrux + 566,
    instrux + 623,
    instrux + 1000,
    instrux + 1267,
    instrux + 1304,
    instrux + 1311,
    instrux + 1312,
    instrux + 1313,
    instrux + 1314,
    instrux + 1317,
    instrux + 1318,
    instrux + 1319,
    instrux + 1320,
    instrux + 1321,
    instrux + 1322,
    instrux + 1327,
    instrux + 1350,
    instrux + 1355,
    instrux + 1356,
    instrux + 3094,
    instrux + 3095,
    instrux + 3096,
    instrux + 3097,
    instrux + 3101,
    instrux + 3102,
    instrux + 3103,
    instrux + 3104,
};

static const struct itemplate * const itable_0FAF[] = {
    instrux + 461,
    instrux + 462,
    instrux + 463,
    instrux + 464,
    instrux + 465,
    instrux + 466,
};

static const struct itemplate * const itable_0FB0[] = {
    instrux + 233,
    instrux + 234,
};

static const struct itemplate * const itable_0FB1[] = {
    instrux + 235,
    instrux + 236,
    instrux + 237,
    instrux + 238,
    instrux + 239,
    instrux + 240,
};

static const struct itemplate * const itable_0FB2[] = {
    instrux + 617,
    instrux + 618,
    instrux + 619,
};

static const struct itemplate * const itable_0FB3[] = {
    instrux + 142,
    instrux + 143,
    instrux + 144,
    instrux + 145,
    instrux + 146,
    instrux + 147,
};

static const struct itemplate * const itable_0FB4[] = {
    instrux + 567,
    instrux + 568,
    instrux + 569,
};

static const struct itemplate * const itable_0FB5[] = {
    instrux + 571,
    instrux + 572,
    instrux + 573,
};

static const struct itemplate * const itable_0FB6[] = {
    instrux + 698,
    instrux + 699,
    instrux + 700,
    instrux + 702,
};

static const struct itemplate * const itable_0FB7[] = {
    instrux + 701,
    instrux + 703,
};

static const struct itemplate * const itable_0FB8[] = {
    instrux + 542,
    instrux + 543,
    instrux + 544,
    instrux + 1680,
    instrux + 1681,
    instrux + 1682,
};

static const struct itemplate * const itable_0FB9[] = {
    instrux + 1139,
};

static const struct itemplate * const itable_0FBA[] = {
    instrux + 130,
    instrux + 131,
    instrux + 132,
    instrux + 139,
    instrux + 140,
    instrux + 141,
    instrux + 148,
    instrux + 149,
    instrux + 150,
    instrux + 157,
    instrux + 158,
    instrux + 159,
};

static const struct itemplate * const itable_0FBB[] = {
    instrux + 133,
    instrux + 134,
    instrux + 135,
    instrux + 136,
    instrux + 137,
    instrux + 138,
};

static const struct itemplate * const itable_0FBC[] = {
    instrux + 110,
    instrux + 111,
    instrux + 112,
    instrux + 113,
    instrux + 114,
    instrux + 115,
};

static const struct itemplate * const itable_0FBD[] = {
    instrux + 116,
    instrux + 117,
    instrux + 118,
    instrux + 119,
    instrux + 120,
    instrux + 121,
    instrux + 1609,
    instrux + 1610,
    instrux + 1611,
};

static const struct itemplate * const itable_0FBE[] = {
    instrux + 691,
    instrux + 692,
    instrux + 693,
    instrux + 695,
};

static const struct itemplate * const itable_0FBF[] = {
    instrux + 694,
    instrux + 696,
};

static const struct itemplate * const itable_0FC0[] = {
    instrux + 1151,
    instrux + 1152,
};

static const struct itemplate * const itable_0FC1[] = {
    instrux + 1153,
    instrux + 1154,
    instrux + 1155,
    instrux + 1156,
    instrux + 1157,
    instrux + 1158,
};

static const struct itemplate * const itable_0FC2[] = {
    instrux + 1233,
    instrux + 1234,
    instrux + 1235,
    instrux + 1236,
    instrux + 1237,
    instrux + 1238,
    instrux + 1239,
    instrux + 1240,
    instrux + 1241,
    instrux + 1242,
    instrux + 1243,
    instrux + 1244,
    instrux + 1245,
    instrux + 1246,
    instrux + 1247,
    instrux + 1248,
    instrux + 1249,
    instrux + 1250,
    instrux + 1251,
    instrux + 1252,
    instrux + 1462,
    instrux + 1463,
    instrux + 1464,
    instrux + 1465,
    instrux + 1466,
    instrux + 1467,
    instrux + 1468,
    instrux + 1469,
    instrux + 1470,
    instrux + 1471,
    instrux + 1472,
    instrux + 1473,
    instrux + 1474,
    instrux + 1475,
    instrux + 1476,
    instrux + 1477,
    instrux + 1478,
    instrux + 1479,
};

static const struct itemplate * const itable_0FC3[] = {
    instrux + 1352,
    instrux + 1353,
};

static const struct itemplate * const itable_0FC4[] = {
    instrux + 1333,
    instrux + 1334,
    instrux + 1335,
    instrux + 1400,
    instrux + 1401,
    instrux + 1402,
};

static const struct itemplate * const itable_0FC5[] = {
    instrux + 1332,
    instrux + 1399,
};

static const struct itemplate * const itable_0FC6[] = {
    instrux + 1300,
    instrux + 1301,
    instrux + 1531,
    instrux + 1532,
};

static const struct itemplate * const itable_0FC7[] = {
    instrux + 241,
    instrux + 242,
    instrux + 1552,
    instrux + 1556,
    instrux + 1557,
    instrux + 1566,
    instrux + 3098,
    instrux + 3099,
    instrux + 3100,
};

static const struct itemplate * const itable_0FC8[] = {
    instrux + 122,
    instrux + 123,
};

static const struct itemplate * const itable_0FC9[] = {
    instrux + 122,
    instrux + 123,
};

static const struct itemplate * const itable_0FCA[] = {
    instrux + 122,
    instrux + 123,
};

static const struct itemplate * const itable_0FCB[] = {
    instrux + 122,
    instrux + 123,
};

static const struct itemplate * const itable_0FCC[] = {
    instrux + 122,
    instrux + 123,
};

static const struct itemplate * const itable_0FCD[] = {
    instrux + 122,
    instrux + 123,
};

static const struct itemplate * const itable_0FCE[] = {
    instrux + 122,
    instrux + 123,
};

static const struct itemplate * const itable_0FCF[] = {
    instrux + 122,
    instrux + 123,
};

static const struct itemplate * const itable_0FD0[] = {
    instrux + 1541,
    instrux + 1542,
};

static const struct itemplate * const itable_0FD1[] = {
    instrux + 847,
    instrux + 1434,
};

static const struct itemplate * const itable_0FD2[] = {
    instrux + 843,
    instrux + 1436,
};

static const struct itemplate * const itable_0FD3[] = {
    instrux + 845,
    instrux + 1438,
};

static const struct itemplate * const itable_0FD4[] = {
    instrux + 1383,
    instrux + 1384,
};

static const struct itemplate * const itable_0FD5[] = {
    instrux + 810,
    instrux + 1411,
};

static const struct itemplate * const itable_0FD6[] = {
    instrux + 1369,
    instrux + 1371,
    instrux + 1372,
    instrux + 1376,
};

static const struct itemplate * const itable_0FD7[] = {
    instrux + 1340,
    instrux + 1408,
};

static const struct itemplate * const itable_0FD8[] = {
    instrux + 854,
    instrux + 1447,
};

static const struct itemplate * const itable_0FD9[] = {
    instrux + 855,
    instrux + 1448,
};

static const struct itemplate * const itable_0FDA[] = {
    instrux + 1339,
    instrux + 1407,
};

static const struct itemplate * const itable_0FDB[] = {
    instrux + 774,
    instrux + 1389,
};

static const struct itemplate * const itable_0FDC[] = {
    instrux + 771,
    instrux + 1387,
};

static const struct itemplate * const itable_0FDD[] = {
    instrux + 772,
    instrux + 1388,
};

static const struct itemplate * const itable_0FDE[] = {
    instrux + 1337,
    instrux + 1405,
};

static const struct itemplate * const itable_0FDF[] = {
    instrux + 775,
    instrux + 1390,
};

static const struct itemplate * const itable_0FE0[] = {
    instrux + 1330,
    instrux + 1391,
};

static const struct itemplate * const itable_0FE1[] = {
    instrux + 841,
    instrux + 1429,
};

static const struct itemplate * const itable_0FE2[] = {
    instrux + 839,
    instrux + 1431,
};

static const struct itemplate * const itable_0FE3[] = {
    instrux + 1331,
    instrux + 1392,
};

static const struct itemplate * const itable_0FE4[] = {
    instrux + 1341,
    instrux + 1409,
};

static const struct itemplate * const itable_0FE5[] = {
    instrux + 809,
    instrux + 1410,
};

static const struct itemplate * const itable_0FE6[] = {
    instrux + 1481,
    instrux + 1483,
    instrux + 1498,
};

static const struct itemplate * const itable_0FE7[] = {
    instrux + 1329,
    instrux + 1351,
};

static const struct itemplate * const itable_0FE8[] = {
    instrux + 851,
    instrux + 1445,
};

static const struct itemplate * const itable_0FE9[] = {
    instrux + 853,
    instrux + 1446,
};

static const struct itemplate * const itable_0FEA[] = {
    instrux + 1338,
    instrux + 1406,
};

static const struct itemplate * const itable_0FEB[] = {
    instrux + 830,
    instrux + 1414,
};

static const struct itemplate * const itable_0FEC[] = {
    instrux + 768,
    instrux + 1385,
};

static const struct itemplate * const itable_0FED[] = {
    instrux + 770,
    instrux + 1386,
};

static const struct itemplate * const itable_0FEE[] = {
    instrux + 1336,
    instrux + 1404,
};

static const struct itemplate * const itable_0FEF[] = {
    instrux + 885,
    instrux + 1457,
};

static const struct itemplate * const itable_0FF0[] = {
    instrux + 1547,
};

static const struct itemplate * const itable_0FF1[] = {
    instrux + 837,
    instrux + 1423,
};

static const struct itemplate * const itable_0FF2[] = {
    instrux + 833,
    instrux + 1425,
};

static const struct itemplate * const itable_0FF3[] = {
    instrux + 835,
    instrux + 1427,
};

static const struct itemplate * const itable_0FF4[] = {
    instrux + 1412,
    instrux + 1413,
};

static const struct itemplate * const itable_0FF5[] = {
    instrux + 804,
    instrux + 1403,
};

static const struct itemplate * const itable_0FF6[] = {
    instrux + 1342,
    instrux + 1415,
};

static const struct itemplate * const itable_0FF7[] = {
    instrux + 1328,
    instrux + 1349,
};

static const struct itemplate * const itable_0FF8[] = {
    instrux + 849,
    instrux + 1440,
};

static const struct itemplate * const itable_0FF9[] = {
    instrux + 856,
    instrux + 1441,
};

static const struct itemplate * const itable_0FFA[] = {
    instrux + 850,
    instrux + 1442,
};

static const struct itemplate * const itable_0FFB[] = {
    instrux + 1443,
    instrux + 1444,
};

static const struct itemplate * const itable_0FFC[] = {
    instrux + 766,
    instrux + 1380,
};

static const struct itemplate * const itable_0FFD[] = {
    instrux + 773,
    instrux + 1381,
};

static const struct itemplate * const itable_0FFE[] = {
    instrux + 767,
    instrux + 1382,
};

static const struct itemplate * const itable_0FFF[] = {
    instrux + 1138,
};

static const struct itemplate * const itable_10[] = {
    instrux + 7,
    instrux + 8,
};

static const struct itemplate * const itable_11[] = {
    instrux + 9,
    instrux + 10,
    instrux + 11,
    instrux + 12,
    instrux + 13,
    instrux + 14,
};

static const struct itemplate * const itable_12[] = {
    instrux + 15,
    instrux + 16,
};

static const struct itemplate * const itable_13[] = {
    instrux + 17,
    instrux + 18,
    instrux + 19,
    instrux + 20,
    instrux + 21,
    instrux + 22,
};

static const struct itemplate * const itable_14[] = {
    instrux + 26,
};

static const struct itemplate * const itable_15[] = {
    instrux + 28,
    instrux + 30,
    instrux + 32,
};

static const struct itemplate * const itable_16[] = {
    instrux + 869,
    instrux + 870,
};

static const struct itemplate * const itable_17[] = {
    instrux + 821,
};

static const struct itemplate * const itable_18[] = {
    instrux + 963,
    instrux + 964,
};

static const struct itemplate * const itable_19[] = {
    instrux + 965,
    instrux + 966,
    instrux + 967,
    instrux + 968,
    instrux + 969,
    instrux + 970,
};

static const struct itemplate * const itable_1A[] = {
    instrux + 971,
    instrux + 972,
};

static const struct itemplate * const itable_1B[] = {
    instrux + 973,
    instrux + 974,
    instrux + 975,
    instrux + 976,
    instrux + 977,
    instrux + 978,
};

static const struct itemplate * const itable_1C[] = {
    instrux + 982,
};

static const struct itemplate * const itable_1D[] = {
    instrux + 984,
    instrux + 986,
    instrux + 988,
};

static const struct itemplate * const itable_1E[] = {
    instrux + 869,
    instrux + 870,
};

static const struct itemplate * const itable_1F[] = {
    instrux + 821,
};

static const struct itemplate * const itable_20[] = {
    instrux + 73,
    instrux + 74,
};

static const struct itemplate * const itable_21[] = {
    instrux + 75,
    instrux + 76,
    instrux + 77,
    instrux + 78,
    instrux + 79,
    instrux + 80,
};

static const struct itemplate * const itable_22[] = {
    instrux + 81,
    instrux + 82,
};

static const struct itemplate * const itable_23[] = {
    instrux + 83,
    instrux + 84,
    instrux + 85,
    instrux + 86,
    instrux + 87,
    instrux + 88,
};

static const struct itemplate * const itable_24[] = {
    instrux + 92,
};

static const struct itemplate * const itable_25[] = {
    instrux + 94,
    instrux + 96,
    instrux + 98,
};

static const struct itemplate * const itable_27[] = {
    instrux + 249,
};

static const struct itemplate * const itable_28[] = {
    instrux + 1075,
    instrux + 1076,
};

static const struct itemplate * const itable_29[] = {
    instrux + 1077,
    instrux + 1078,
    instrux + 1079,
    instrux + 1080,
    instrux + 1081,
    instrux + 1082,
};

static const struct itemplate * const itable_2A[] = {
    instrux + 1083,
    instrux + 1084,
};

static const struct itemplate * const itable_2B[] = {
    instrux + 1085,
    instrux + 1086,
    instrux + 1087,
    instrux + 1088,
    instrux + 1089,
    instrux + 1090,
};

static const struct itemplate * const itable_2C[] = {
    instrux + 1094,
};

static const struct itemplate * const itable_2D[] = {
    instrux + 1096,
    instrux + 1098,
    instrux + 1100,
};

static const struct itemplate * const itable_2F[] = {
    instrux + 250,
};

static const struct itemplate * const itable_30[] = {
    instrux + 1184,
    instrux + 1185,
};

static const struct itemplate * const itable_31[] = {
    instrux + 1186,
    instrux + 1187,
    instrux + 1188,
    instrux + 1189,
    instrux + 1190,
    instrux + 1191,
};

static const struct itemplate * const itable_32[] = {
    instrux + 1192,
    instrux + 1193,
};

static const struct itemplate * const itable_33[] = {
    instrux + 1194,
    instrux + 1195,
    instrux + 1196,
    instrux + 1197,
    instrux + 1198,
    instrux + 1199,
};

static const struct itemplate * const itable_34[] = {
    instrux + 1203,
};

static const struct itemplate * const itable_35[] = {
    instrux + 1205,
    instrux + 1207,
    instrux + 1209,
};

static const struct itemplate * const itable_37[] = {
    instrux + 1,
};

static const struct itemplate * const itable_38[] = {
    instrux + 196,
    instrux + 197,
};

static const struct itemplate * const itable_39[] = {
    instrux + 198,
    instrux + 199,
    instrux + 200,
    instrux + 201,
    instrux + 202,
    instrux + 203,
};

static const struct itemplate * const itable_3A[] = {
    instrux + 204,
    instrux + 205,
};

static const struct itemplate * const itable_3B[] = {
    instrux + 206,
    instrux + 207,
    instrux + 208,
    instrux + 209,
    instrux + 210,
    instrux + 211,
};

static const struct itemplate * const itable_3C[] = {
    instrux + 215,
};

static const struct itemplate * const itable_3D[] = {
    instrux + 217,
    instrux + 219,
    instrux + 221,
};

static const struct itemplate * const itable_3F[] = {
    instrux + 6,
};

static const struct itemplate * const itable_40[] = {
    instrux + 491,
    instrux + 492,
};

static const struct itemplate * const itable_41[] = {
    instrux + 491,
    instrux + 492,
};

static const struct itemplate * const itable_42[] = {
    instrux + 491,
    instrux + 492,
};

static const struct itemplate * const itable_43[] = {
    instrux + 491,
    instrux + 492,
};

static const struct itemplate * const itable_44[] = {
    instrux + 491,
    instrux + 492,
};

static const struct itemplate * const itable_45[] = {
    instrux + 491,
    instrux + 492,
};

static const struct itemplate * const itable_46[] = {
    instrux + 491,
    instrux + 492,
};

static const struct itemplate * const itable_47[] = {
    instrux + 491,
    instrux + 492,
};

static const struct itemplate * const itable_48[] = {
    instrux + 251,
    instrux + 252,
};

static const struct itemplate * const itable_49[] = {
    instrux + 251,
    instrux + 252,
};

static const struct itemplate * const itable_4A[] = {
    instrux + 251,
    instrux + 252,
};

static const struct itemplate * const itable_4B[] = {
    instrux + 251,
    instrux + 252,
};

static const struct itemplate * const itable_4C[] = {
    instrux + 251,
    instrux + 252,
};

static const struct itemplate * const itable_4D[] = {
    instrux + 251,
    instrux + 252,
};

static const struct itemplate * const itable_4E[] = {
    instrux + 251,
    instrux + 252,
};

static const struct itemplate * const itable_4F[] = {
    instrux + 251,
    instrux + 252,
};

static const struct itemplate * const itable_50[] = {
    instrux + 863,
    instrux + 864,
    instrux + 865,
};

static const struct itemplate * const itable_51[] = {
    instrux + 863,
    instrux + 864,
    instrux + 865,
};

static const struct itemplate * const itable_52[] = {
    instrux + 863,
    instrux + 864,
    instrux + 865,
};

static const struct itemplate * const itable_53[] = {
    instrux + 863,
    instrux + 864,
    instrux + 865,
};

static const struct itemplate * const itable_54[] = {
    instrux + 863,
    instrux + 864,
    instrux + 865,
};

static const struct itemplate * const itable_55[] = {
    instrux + 863,
    instrux + 864,
    instrux + 865,
};

static const struct itemplate * const itable_56[] = {
    instrux + 863,
    instrux + 864,
    instrux + 865,
};

static const struct itemplate * const itable_57[] = {
    instrux + 863,
    instrux + 864,
    instrux + 865,
};

static const struct itemplate * const itable_58[] = {
    instrux + 815,
    instrux + 816,
    instrux + 817,
};

static const struct itemplate * const itable_59[] = {
    instrux + 815,
    instrux + 816,
    instrux + 817,
};

static const struct itemplate * const itable_5A[] = {
    instrux + 815,
    instrux + 816,
    instrux + 817,
};

static const struct itemplate * const itable_5B[] = {
    instrux + 815,
    instrux + 816,
    instrux + 817,
};

static const struct itemplate * const itable_5C[] = {
    instrux + 815,
    instrux + 816,
    instrux + 817,
};

static const struct itemplate * const itable_5D[] = {
    instrux + 815,
    instrux + 816,
    instrux + 817,
};

static const struct itemplate * const itable_5E[] = {
    instrux + 815,
    instrux + 816,
    instrux + 817,
};

static const struct itemplate * const itable_5F[] = {
    instrux + 815,
    instrux + 816,
    instrux + 817,
};

static const struct itemplate * const itable_60[] = {
    instrux + 878,
    instrux + 879,
    instrux + 880,
};

static const struct itemplate * const itable_61[] = {
    instrux + 823,
    instrux + 824,
    instrux + 825,
};

static const struct itemplate * const itable_62[] = {
    instrux + 108,
    instrux + 109,
};

static const struct itemplate * const itable_63[] = {
    instrux + 106,
    instrux + 107,
    instrux + 697,
};

static const struct itemplate * const itable_68[] = {
    instrux + 873,
    instrux + 874,
    instrux + 875,
    instrux + 876,
    instrux + 877,
};

static const struct itemplate * const itable_69[] = {
    instrux + 468,
    instrux + 470,
    instrux + 472,
    instrux + 474,
    instrux + 476,
    instrux + 478,
    instrux + 480,
    instrux + 482,
    instrux + 484,
};

static const struct itemplate * const itable_6A[] = {
    instrux + 872,
    instrux + 873,
    instrux + 874,
    instrux + 875,
    instrux + 876,
    instrux + 877,
};

static const struct itemplate * const itable_6B[] = {
    instrux + 467,
    instrux + 469,
    instrux + 471,
    instrux + 473,
    instrux + 475,
    instrux + 477,
    instrux + 479,
    instrux + 481,
    instrux + 483,
};

static const struct itemplate * const itable_6C[] = {
    instrux + 497,
};

static const struct itemplate * const itable_6D[] = {
    instrux + 498,
    instrux + 499,
};

static const struct itemplate * const itable_6E[] = {
    instrux + 760,
};

static const struct itemplate * const itable_6F[] = {
    instrux + 761,
    instrux + 762,
};

static const struct itemplate * const itable_70[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_71[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_72[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_73[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_74[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_75[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_76[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_77[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_78[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_79[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_7A[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_7B[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_7C[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_7D[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_7E[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_7F[] = {
    instrux + 1226,
};

static const struct itemplate * const itable_80[] = {
    instrux + 33,
    instrux + 37,
    instrux + 66,
    instrux + 70,
    instrux + 99,
    instrux + 103,
    instrux + 222,
    instrux + 226,
    instrux + 747,
    instrux + 751,
    instrux + 989,
    instrux + 993,
    instrux + 1101,
    instrux + 1105,
    instrux + 1210,
    instrux + 1214,
};

static const struct itemplate * const itable_81[] = {
    instrux + 34,
    instrux + 35,
    instrux + 36,
    instrux + 38,
    instrux + 39,
    instrux + 67,
    instrux + 68,
    instrux + 69,
    instrux + 71,
    instrux + 72,
    instrux + 100,
    instrux + 101,
    instrux + 102,
    instrux + 104,
    instrux + 105,
    instrux + 223,
    instrux + 224,
    instrux + 225,
    instrux + 227,
    instrux + 228,
    instrux + 748,
    instrux + 749,
    instrux + 750,
    instrux + 752,
    instrux + 753,
    instrux + 990,
    instrux + 991,
    instrux + 992,
    instrux + 994,
    instrux + 995,
    instrux + 1102,
    instrux + 1103,
    instrux + 1104,
    instrux + 1106,
    instrux + 1107,
    instrux + 1211,
    instrux + 1212,
    instrux + 1213,
    instrux + 1215,
    instrux + 1216,
};

static const struct itemplate * const itable_83[] = {
    instrux + 23,
    instrux + 24,
    instrux + 25,
    instrux + 27,
    instrux + 29,
    instrux + 31,
    instrux + 34,
    instrux + 35,
    instrux + 36,
    instrux + 38,
    instrux + 39,
    instrux + 56,
    instrux + 57,
    instrux + 58,
    instrux + 60,
    instrux + 62,
    instrux + 64,
    instrux + 67,
    instrux + 68,
    instrux + 69,
    instrux + 71,
    instrux + 72,
    instrux + 89,
    instrux + 90,
    instrux + 91,
    instrux + 93,
    instrux + 95,
    instrux + 97,
    instrux + 100,
    instrux + 101,
    instrux + 102,
    instrux + 104,
    instrux + 105,
    instrux + 212,
    instrux + 213,
    instrux + 214,
    instrux + 216,
    instrux + 218,
    instrux + 220,
    instrux + 223,
    instrux + 224,
    instrux + 225,
    instrux + 227,
    instrux + 228,
    instrux + 737,
    instrux + 738,
    instrux + 739,
    instrux + 741,
    instrux + 743,
    instrux + 745,
    instrux + 748,
    instrux + 749,
    instrux + 750,
    instrux + 752,
    instrux + 753,
    instrux + 979,
    instrux + 980,
    instrux + 981,
    instrux + 983,
    instrux + 985,
    instrux + 987,
    instrux + 990,
    instrux + 991,
    instrux + 992,
    instrux + 994,
    instrux + 995,
    instrux + 1091,
    instrux + 1092,
    instrux + 1093,
    instrux + 1095,
    instrux + 1097,
    instrux + 1099,
    instrux + 1102,
    instrux + 1103,
    instrux + 1104,
    instrux + 1106,
    instrux + 1107,
    instrux + 1200,
    instrux + 1201,
    instrux + 1202,
    instrux + 1204,
    instrux + 1206,
    instrux + 1208,
    instrux + 1211,
    instrux + 1212,
    instrux + 1213,
    instrux + 1215,
    instrux + 1216,
};

static const struct itemplate * const itable_84[] = {
    instrux + 1115,
    instrux + 1116,
    instrux + 1123,
};

static const struct itemplate * const itable_85[] = {
    instrux + 1117,
    instrux + 1118,
    instrux + 1119,
    instrux + 1120,
    instrux + 1121,
    instrux + 1122,
    instrux + 1124,
    instrux + 1125,
    instrux + 1126,
};

static const struct itemplate * const itable_86[] = {
    instrux + 1166,
    instrux + 1167,
    instrux + 1174,
    instrux + 1175,
};

static const struct itemplate * const itable_87[] = {
    instrux + 1168,
    instrux + 1169,
    instrux + 1170,
    instrux + 1171,
    instrux + 1172,
    instrux + 1173,
    instrux + 1176,
    instrux + 1177,
    instrux + 1178,
    instrux + 1179,
    instrux + 1180,
    instrux + 1181,
};

static const struct itemplate * const itable_88[] = {
    instrux + 647,
    instrux + 648,
};

static const struct itemplate * const itable_89[] = {
    instrux + 649,
    instrux + 650,
    instrux + 651,
    instrux + 652,
    instrux + 653,
    instrux + 654,
};

static const struct itemplate * const itable_8A[] = {
    instrux + 655,
    instrux + 656,
};

static const struct itemplate * const itable_8B[] = {
    instrux + 657,
    instrux + 658,
    instrux + 659,
    instrux + 660,
    instrux + 661,
    instrux + 662,
};

static const struct itemplate * const itable_8C[] = {
    instrux + 625,
    instrux + 626,
    instrux + 627,
};

static const struct itemplate * const itable_8D[] = {
    instrux + 560,
    instrux + 561,
    instrux + 562,
};

static const struct itemplate * const itable_8E[] = {
    instrux + 628,
    instrux + 629,
    instrux + 630,
};

static const struct itemplate * const itable_8F[] = {
    instrux + 818,
    instrux + 819,
    instrux + 820,
};

static const struct itemplate * const itable_90[] = {
    instrux + 713,
    instrux + 776,
    instrux + 1159,
    instrux + 1160,
    instrux + 1161,
    instrux + 1162,
    instrux + 1163,
    instrux + 1164,
    instrux + 1165,
};

static const struct itemplate * const itable_91[] = {
    instrux + 1159,
    instrux + 1160,
    instrux + 1161,
    instrux + 1162,
    instrux + 1163,
    instrux + 1164,
};

static const struct itemplate * const itable_92[] = {
    instrux + 1159,
    instrux + 1160,
    instrux + 1161,
    instrux + 1162,
    instrux + 1163,
    instrux + 1164,
};

static const struct itemplate * const itable_93[] = {
    instrux + 1159,
    instrux + 1160,
    instrux + 1161,
    instrux + 1162,
    instrux + 1163,
    instrux + 1164,
};

static const struct itemplate * const itable_94[] = {
    instrux + 1159,
    instrux + 1160,
    instrux + 1161,
    instrux + 1162,
    instrux + 1163,
    instrux + 1164,
};

static const struct itemplate * const itable_95[] = {
    instrux + 1159,
    instrux + 1160,
    instrux + 1161,
    instrux + 1162,
    instrux + 1163,
    instrux + 1164,
};

static const struct itemplate * const itable_96[] = {
    instrux + 1159,
    instrux + 1160,
    instrux + 1161,
    instrux + 1162,
    instrux + 1163,
    instrux + 1164,
};

static const struct itemplate * const itable_97[] = {
    instrux + 1159,
    instrux + 1160,
    instrux + 1161,
    instrux + 1162,
    instrux + 1163,
    instrux + 1164,
};

static const struct itemplate * const itable_98[] = {
    instrux + 187,
    instrux + 189,
    instrux + 248,
};

static const struct itemplate * const itable_99[] = {
    instrux + 188,
    instrux + 246,
    instrux + 247,
};

static const struct itemplate * const itable_9A[] = {
    instrux + 166,
    instrux + 167,
    instrux + 168,
    instrux + 169,
    instrux + 170,
};

static const struct itemplate * const itable_9C[] = {
    instrux + 881,
    instrux + 882,
    instrux + 883,
    instrux + 884,
};

static const struct itemplate * const itable_9D[] = {
    instrux + 826,
    instrux + 827,
    instrux + 828,
    instrux + 829,
};

static const struct itemplate * const itable_9E[] = {
    instrux + 949,
};

static const struct itemplate * const itable_9F[] = {
    instrux + 547,
};

static const struct itemplate * const itable_A0[] = {
    instrux + 631,
};

static const struct itemplate * const itable_A1[] = {
    instrux + 632,
    instrux + 633,
    instrux + 634,
};

static const struct itemplate * const itable_A2[] = {
    instrux + 635,
};

static const struct itemplate * const itable_A3[] = {
    instrux + 636,
    instrux + 637,
    instrux + 638,
};

static const struct itemplate * const itable_A4[] = {
    instrux + 687,
};

static const struct itemplate * const itable_A5[] = {
    instrux + 688,
    instrux + 689,
    instrux + 690,
};

static const struct itemplate * const itable_A6[] = {
    instrux + 229,
};

static const struct itemplate * const itable_A7[] = {
    instrux + 230,
    instrux + 231,
    instrux + 232,
};

static const struct itemplate * const itable_A8[] = {
    instrux + 1127,
};

static const struct itemplate * const itable_A9[] = {
    instrux + 1128,
    instrux + 1129,
    instrux + 1130,
};

static const struct itemplate * const itable_AA[] = {
    instrux + 1066,
};

static const struct itemplate * const itable_AB[] = {
    instrux + 1067,
    instrux + 1068,
    instrux + 1069,
};

static const struct itemplate * const itable_AC[] = {
    instrux + 583,
};

static const struct itemplate * const itable_AD[] = {
    instrux + 584,
    instrux + 585,
    instrux + 586,
};

static const struct itemplate * const itable_AE[] = {
    instrux + 996,
};

static const struct itemplate * const itable_AF[] = {
    instrux + 997,
    instrux + 998,
    instrux + 999,
};

static const struct itemplate * const itable_B0[] = {
    instrux + 663,
};

static const struct itemplate * const itable_B1[] = {
    instrux + 663,
};

static const struct itemplate * const itable_B2[] = {
    instrux + 663,
};

static const struct itemplate * const itable_B3[] = {
    instrux + 663,
};

static const struct itemplate * const itable_B4[] = {
    instrux + 663,
};

static const struct itemplate * const itable_B5[] = {
    instrux + 663,
};

static const struct itemplate * const itable_B6[] = {
    instrux + 663,
};

static const struct itemplate * const itable_B7[] = {
    instrux + 663,
};

static const struct itemplate * const itable_B8[] = {
    instrux + 664,
    instrux + 665,
    instrux + 666,
};

static const struct itemplate * const itable_B9[] = {
    instrux + 664,
    instrux + 665,
    instrux + 666,
};

static const struct itemplate * const itable_BA[] = {
    instrux + 664,
    instrux + 665,
    instrux + 666,
};

static const struct itemplate * const itable_BB[] = {
    instrux + 664,
    instrux + 665,
    instrux + 666,
};

static const struct itemplate * const itable_BC[] = {
    instrux + 664,
    instrux + 665,
    instrux + 666,
};

static const struct itemplate * const itable_BD[] = {
    instrux + 664,
    instrux + 665,
    instrux + 666,
};

static const struct itemplate * const itable_BE[] = {
    instrux + 664,
    instrux + 665,
    instrux + 666,
};

static const struct itemplate * const itable_BF[] = {
    instrux + 664,
    instrux + 665,
    instrux + 666,
};

static const struct itemplate * const itable_C0[] = {
    instrux + 888,
    instrux + 900,
    instrux + 923,
    instrux + 935,
    instrux + 953,
    instrux + 1004,
    instrux + 1028,
};

static const struct itemplate * const itable_C1[] = {
    instrux + 891,
    instrux + 894,
    instrux + 897,
    instrux + 903,
    instrux + 906,
    instrux + 909,
    instrux + 926,
    instrux + 929,
    instrux + 932,
    instrux + 938,
    instrux + 941,
    instrux + 944,
    instrux + 956,
    instrux + 959,
    instrux + 962,
    instrux + 1007,
    instrux + 1010,
    instrux + 1013,
    instrux + 1031,
    instrux + 1034,
    instrux + 1037,
};

static const struct itemplate * const itable_C2[] = {
    instrux + 916,
    instrux + 920,
};

static const struct itemplate * const itable_C3[] = {
    instrux + 915,
    instrux + 919,
};

static const struct itemplate * const itable_C4[] = {
    instrux + 564,
    instrux + 565,
};

static const struct itemplate * const itable_C5[] = {
    instrux + 558,
    instrux + 559,
};

static const struct itemplate * const itable_C6[] = {
    instrux + 667,
    instrux + 672,
};

static const struct itemplate * const itable_C7[] = {
    instrux + 668,
    instrux + 669,
    instrux + 670,
    instrux + 671,
    instrux + 673,
    instrux + 674,
};

static const struct itemplate * const itable_C8[] = {
    instrux + 263,
};

static const struct itemplate * const itable_C9[] = {
    instrux + 563,
};

static const struct itemplate * const itable_CA[] = {
    instrux + 918,
};

static const struct itemplate * const itable_CB[] = {
    instrux + 917,
};

static const struct itemplate * const itable_CC[] = {
    instrux + 502,
};

static const struct itemplate * const itable_CD[] = {
    instrux + 500,
};

static const struct itemplate * const itable_CE[] = {
    instrux + 503,
};

static const struct itemplate * const itable_CF[] = {
    instrux + 510,
    instrux + 511,
    instrux + 512,
    instrux + 513,
};

static const struct itemplate * const itable_D0[] = {
    instrux + 886,
    instrux + 898,
    instrux + 921,
    instrux + 933,
    instrux + 951,
    instrux + 1002,
    instrux + 1026,
};

static const struct itemplate * const itable_D1[] = {
    instrux + 889,
    instrux + 892,
    instrux + 895,
    instrux + 901,
    instrux + 904,
    instrux + 907,
    instrux + 924,
    instrux + 927,
    instrux + 930,
    instrux + 936,
    instrux + 939,
    instrux + 942,
    instrux + 954,
    instrux + 957,
    instrux + 960,
    instrux + 1005,
    instrux + 1008,
    instrux + 1011,
    instrux + 1029,
    instrux + 1032,
    instrux + 1035,
};

static const struct itemplate * const itable_D2[] = {
    instrux + 887,
    instrux + 899,
    instrux + 922,
    instrux + 934,
    instrux + 952,
    instrux + 1003,
    instrux + 1027,
};

static const struct itemplate * const itable_D3[] = {
    instrux + 890,
    instrux + 893,
    instrux + 896,
    instrux + 902,
    instrux + 905,
    instrux + 908,
    instrux + 925,
    instrux + 928,
    instrux + 931,
    instrux + 937,
    instrux + 940,
    instrux + 943,
    instrux + 955,
    instrux + 958,
    instrux + 961,
    instrux + 1006,
    instrux + 1009,
    instrux + 1012,
    instrux + 1030,
    instrux + 1033,
    instrux + 1036,
};

static const struct itemplate * const itable_D4[] = {
    instrux + 4,
    instrux + 5,
};

static const struct itemplate * const itable_D5[] = {
    instrux + 2,
    instrux + 3,
};

static const struct itemplate * const itable_D6[] = {
    instrux + 950,
};

static const struct itemplate * const itable_D7[] = {
    instrux + 1182,
    instrux + 1183,
};

static const struct itemplate * const itable_D8[] = {
    instrux + 268,
    instrux + 271,
    instrux + 273,
    instrux + 298,
    instrux + 300,
    instrux + 301,
    instrux + 306,
    instrux + 308,
    instrux + 309,
    instrux + 314,
    instrux + 317,
    instrux + 319,
    instrux + 322,
    instrux + 326,
    instrux + 327,
    instrux + 378,
    instrux + 382,
    instrux + 383,
    instrux + 419,
    instrux + 423,
    instrux + 424,
    instrux + 427,
    instrux + 431,
    instrux + 432,
};

static const struct itemplate * const itable_D9[] = {
    instrux + 266,
    instrux + 267,
    instrux + 280,
    instrux + 311,
    instrux + 312,
    instrux + 351,
    instrux + 365,
    instrux + 368,
    instrux + 369,
    instrux + 370,
    instrux + 371,
    instrux + 372,
    instrux + 373,
    instrux + 374,
    instrux + 375,
    instrux + 376,
    instrux + 377,
    instrux + 390,
    instrux + 392,
    instrux + 393,
    instrux + 396,
    instrux + 397,
    instrux + 398,
    instrux + 399,
    instrux + 400,
    instrux + 403,
    instrux + 405,
    instrux + 406,
    instrux + 407,
    instrux + 408,
    instrux + 411,
    instrux + 412,
    instrux + 413,
    instrux + 435,
    instrux + 445,
    instrux + 446,
    instrux + 447,
    instrux + 448,
    instrux + 449,
    instrux + 450,
    instrux + 451,
};

static const struct itemplate * const itable_DA[] = {
    instrux + 282,
    instrux + 283,
    instrux + 284,
    instrux + 285,
    instrux + 286,
    instrux + 287,
    instrux + 296,
    instrux + 297,
    instrux + 336,
    instrux + 338,
    instrux + 340,
    instrux + 342,
    instrux + 344,
    instrux + 349,
    instrux + 361,
    instrux + 363,
    instrux + 444,
};

static const struct itemplate * const itable_DB[] = {
    instrux + 281,
    instrux + 288,
    instrux + 289,
    instrux + 290,
    instrux + 291,
    instrux + 292,
    instrux + 293,
    instrux + 294,
    instrux + 295,
    instrux + 302,
    instrux + 303,
    instrux + 313,
    instrux + 331,
    instrux + 346,
    instrux + 352,
    instrux + 353,
    instrux + 355,
    instrux + 359,
    instrux + 367,
    instrux + 386,
    instrux + 387,
    instrux + 388,
    instrux + 389,
    instrux + 404,
    instrux + 415,
    instrux + 438,
    instrux + 439,
};

static const struct itemplate * const itable_DC[] = {
    instrux + 269,
    instrux + 270,
    instrux + 272,
    instrux + 299,
    instrux + 307,
    instrux + 315,
    instrux + 316,
    instrux + 318,
    instrux + 323,
    instrux + 324,
    instrux + 325,
    instrux + 379,
    instrux + 380,
    instrux + 381,
    instrux + 420,
    instrux + 421,
    instrux + 422,
    instrux + 428,
    instrux + 429,
    instrux + 430,
};

static const struct itemplate * const itable_DD[] = {
    instrux + 332,
    instrux + 333,
    instrux + 360,
    instrux + 366,
    instrux + 391,
    instrux + 394,
    instrux + 401,
    instrux + 402,
    instrux + 409,
    instrux + 410,
    instrux + 414,
    instrux + 416,
    instrux + 417,
    instrux + 436,
    instrux + 437,
    instrux + 442,
    instrux + 443,
};

static const struct itemplate * const itable_DE[] = {
    instrux + 274,
    instrux + 275,
    instrux + 310,
    instrux + 320,
    instrux + 321,
    instrux + 328,
    instrux + 329,
    instrux + 337,
    instrux + 339,
    instrux + 341,
    instrux + 343,
    instrux + 345,
    instrux + 350,
    instrux + 362,
    instrux + 364,
    instrux + 384,
    instrux + 385,
    instrux + 425,
    instrux + 426,
    instrux + 433,
    instrux + 434,
};

static const struct itemplate * const itable_DF[] = {
    instrux + 276,
    instrux + 277,
    instrux + 278,
    instrux + 279,
    instrux + 304,
    instrux + 305,
    instrux + 334,
    instrux + 335,
    instrux + 347,
    instrux + 348,
    instrux + 354,
    instrux + 356,
    instrux + 357,
    instrux + 358,
    instrux + 395,
    instrux + 418,
    instrux + 440,
    instrux + 441,
};

static const struct itemplate * const itable_E0[] = {
    instrux + 595,
    instrux + 596,
    instrux + 597,
    instrux + 598,
    instrux + 599,
    instrux + 600,
    instrux + 601,
    instrux + 602,
};

static const struct itemplate * const itable_E1[] = {
    instrux + 591,
    instrux + 592,
    instrux + 593,
    instrux + 594,
    instrux + 603,
    instrux + 604,
    instrux + 605,
    instrux + 606,
};

static const struct itemplate * const itable_E2[] = {
    instrux + 587,
    instrux + 588,
    instrux + 589,
    instrux + 590,
};

static const struct itemplate * const itable_E3[] = {
    instrux + 514,
    instrux + 515,
    instrux + 516,
};

static const struct itemplate * const itable_E4[] = {
    instrux + 485,
};

static const struct itemplate * const itable_E5[] = {
    instrux + 486,
    instrux + 487,
};

static const struct itemplate * const itable_E6[] = {
    instrux + 754,
};

static const struct itemplate * const itable_E7[] = {
    instrux + 755,
    instrux + 756,
};

static const struct itemplate * const itable_E8[] = {
    instrux + 160,
    instrux + 161,
    instrux + 162,
    instrux + 163,
    instrux + 164,
    instrux + 165,
};

static const struct itemplate * const itable_E9[] = {
    instrux + 518,
    instrux + 519,
    instrux + 520,
};

static const struct itemplate * const itable_EA[] = {
    instrux + 521,
    instrux + 522,
    instrux + 523,
    instrux + 524,
    instrux + 525,
};

static const struct itemplate * const itable_EB[] = {
    instrux + 517,
};

static const struct itemplate * const itable_EC[] = {
    instrux + 488,
};

static const struct itemplate * const itable_ED[] = {
    instrux + 489,
    instrux + 490,
};

static const struct itemplate * const itable_EE[] = {
    instrux + 757,
};

static const struct itemplate * const itable_EF[] = {
    instrux + 758,
    instrux + 759,
};

static const struct itemplate * const itable_F1[] = {
    instrux + 501,
    instrux + 1057,
};

static const struct itemplate * const itable_F4[] = {
    instrux + 452,
};

static const struct itemplate * const itable_F5[] = {
    instrux + 195,
};

static const struct itemplate * const itable_F6[] = {
    instrux + 257,
    instrux + 453,
    instrux + 457,
    instrux + 704,
    instrux + 709,
    instrux + 717,
    instrux + 1131,
    instrux + 1135,
};

static const struct itemplate * const itable_F7[] = {
    instrux + 258,
    instrux + 259,
    instrux + 260,
    instrux + 454,
    instrux + 455,
    instrux + 456,
    instrux + 458,
    instrux + 459,
    instrux + 460,
    instrux + 705,
    instrux + 706,
    instrux + 707,
    instrux + 710,
    instrux + 711,
    instrux + 712,
    instrux + 718,
    instrux + 719,
    instrux + 720,
    instrux + 1132,
    instrux + 1133,
    instrux + 1134,
    instrux + 1136,
    instrux + 1137,
};

static const struct itemplate * const itable_F8[] = {
    instrux + 190,
};

static const struct itemplate * const itable_F9[] = {
    instrux + 1062,
};

static const struct itemplate * const itable_FA[] = {
    instrux + 193,
};

static const struct itemplate * const itable_FB[] = {
    instrux + 1065,
};

static const struct itemplate * const itable_FC[] = {
    instrux + 191,
};

static const struct itemplate * const itable_FD[] = {
    instrux + 1063,
};

static const struct itemplate * const itable_FE[] = {
    instrux + 253,
    instrux + 493,
};

static const struct itemplate * const itable_FF[] = {
    instrux + 171,
    instrux + 172,
    instrux + 173,
    instrux + 174,
    instrux + 175,
    instrux + 176,
    instrux + 177,
    instrux + 178,
    instrux + 179,
    instrux + 180,
    instrux + 181,
    instrux + 182,
    instrux + 183,
    instrux + 184,
    instrux + 185,
    instrux + 186,
    instrux + 254,
    instrux + 255,
    instrux + 256,
    instrux + 494,
    instrux + 495,
    instrux + 496,
    instrux + 526,
    instrux + 527,
    instrux + 528,
    instrux + 529,
    instrux + 530,
    instrux + 531,
    instrux + 532,
    instrux + 533,
    instrux + 534,
    instrux + 535,
    instrux + 536,
    instrux + 537,
    instrux + 538,
    instrux + 539,
    instrux + 540,
    instrux + 541,
    instrux + 866,
    instrux + 867,
    instrux + 868,
};

static const struct itemplate * const itable_vex01010[] = {
    instrux + 2527,
    instrux + 2529,
};

static const struct itemplate * const itable_vex01011[] = {
    instrux + 2528,
    instrux + 2530,
};

static const struct itemplate * const itable_vex01012[] = {
    instrux + 2475,
    instrux + 2476,
    instrux + 2488,
    instrux + 2489,
};

static const struct itemplate * const itable_vex01013[] = {
    instrux + 2490,
};

static const struct itemplate * const itable_vex01014[] = {
    instrux + 2873,
    instrux + 2874,
    instrux + 2875,
    instrux + 2876,
};

static const struct itemplate * const itable_vex01015[] = {
    instrux + 2865,
    instrux + 2866,
    instrux + 2867,
    instrux + 2868,
};

static const struct itemplate * const itable_vex01016[] = {
    instrux + 2480,
    instrux + 2481,
    instrux + 2483,
    instrux + 2484,
};

static const struct itemplate * const itable_vex01017[] = {
    instrux + 2482,
};

static const struct itemplate * const itable_vex01028[] = {
    instrux + 2451,
    instrux + 2453,
};

static const struct itemplate * const itable_vex01029[] = {
    instrux + 2452,
    instrux + 2454,
};

static const struct itemplate * const itable_vex0102B[] = {
    instrux + 2505,
    instrux + 2506,
};

static const struct itemplate * const itable_vex0102E[] = {
    instrux + 2860,
};

static const struct itemplate * const itable_vex0102F[] = {
    instrux + 2329,
};

static const struct itemplate * const itable_vex01050[] = {
    instrux + 2495,
    instrux + 2496,
    instrux + 2497,
    instrux + 2498,
};

static const struct itemplate * const itable_vex01051[] = {
    instrux + 2836,
    instrux + 2837,
};

static const struct itemplate * const itable_vex01052[] = {
    instrux + 2814,
    instrux + 2815,
};

static const struct itemplate * const itable_vex01053[] = {
    instrux + 2810,
    instrux + 2811,
};

static const struct itemplate * const itable_vex01054[] = {
    instrux + 1732,
    instrux + 1733,
    instrux + 1734,
    instrux + 1735,
};

static const struct itemplate * const itable_vex01055[] = {
    instrux + 1740,
    instrux + 1741,
    instrux + 1742,
    instrux + 1743,
};

static const struct itemplate * const itable_vex01056[] = {
    instrux + 2549,
    instrux + 2550,
    instrux + 2551,
    instrux + 2552,
};

static const struct itemplate * const itable_vex01057[] = {
    instrux + 2881,
    instrux + 2882,
    instrux + 2883,
    instrux + 2884,
};

static const struct itemplate * const itable_vex01058[] = {
    instrux + 1712,
    instrux + 1713,
    instrux + 1714,
    instrux + 1715,
};

static const struct itemplate * const itable_vex01059[] = {
    instrux + 2537,
    instrux + 2538,
    instrux + 2539,
    instrux + 2540,
};

static const struct itemplate * const itable_vex0105A[] = {
    instrux + 2344,
    instrux + 2345,
};

static const struct itemplate * const itable_vex0105B[] = {
    instrux + 2332,
    instrux + 2333,
};

static const struct itemplate * const itable_vex0105C[] = {
    instrux + 2847,
    instrux + 2848,
    instrux + 2849,
    instrux + 2850,
};

static const struct itemplate * const itable_vex0105D[] = {
    instrux + 2439,
    instrux + 2440,
    instrux + 2441,
    instrux + 2442,
};

static const struct itemplate * const itable_vex0105E[] = {
    instrux + 2376,
    instrux + 2377,
    instrux + 2378,
    instrux + 2379,
};

static const struct itemplate * const itable_vex0105F[] = {
    instrux + 2427,
    instrux + 2428,
    instrux + 2429,
    instrux + 2430,
};

static const struct itemplate * const itable_vex01077[] = {
    instrux + 2885,
    instrux + 2886,
};

static const struct itemplate * const itable_vex010AE[] = {
    instrux + 2413,
    instrux + 2842,
};

static const struct itemplate * const itable_vex010C2[] = {
    instrux + 1952,
    instrux + 1953,
    instrux + 1954,
    instrux + 1955,
    instrux + 1956,
    instrux + 1957,
    instrux + 1958,
    instrux + 1959,
    instrux + 1960,
    instrux + 1961,
    instrux + 1962,
    instrux + 1963,
    instrux + 1964,
    instrux + 1965,
    instrux + 1966,
    instrux + 1967,
    instrux + 1968,
    instrux + 1969,
    instrux + 1970,
    instrux + 1971,
    instrux + 1972,
    instrux + 1973,
    instrux + 1974,
    instrux + 1975,
    instrux + 1976,
    instrux + 1977,
    instrux + 1978,
    instrux + 1979,
    instrux + 1980,
    instrux + 1981,
    instrux + 1982,
    instrux + 1983,
    instrux + 1984,
    instrux + 1985,
    instrux + 1986,
    instrux + 1987,
    instrux + 1988,
    instrux + 1989,
    instrux + 1990,
    instrux + 1991,
    instrux + 1992,
    instrux + 1993,
    instrux + 1994,
    instrux + 1995,
    instrux + 1996,
    instrux + 1997,
    instrux + 1998,
    instrux + 1999,
    instrux + 2000,
    instrux + 2001,
    instrux + 2002,
    instrux + 2003,
    instrux + 2004,
    instrux + 2005,
    instrux + 2006,
    instrux + 2007,
    instrux + 2008,
    instrux + 2009,
    instrux + 2010,
    instrux + 2011,
    instrux + 2012,
    instrux + 2013,
    instrux + 2014,
    instrux + 2015,
    instrux + 2016,
    instrux + 2017,
    instrux + 2018,
    instrux + 2019,
    instrux + 2020,
    instrux + 2021,
    instrux + 2022,
    instrux + 2023,
    instrux + 2024,
    instrux + 2025,
    instrux + 2026,
    instrux + 2027,
    instrux + 2028,
    instrux + 2029,
    instrux + 2030,
    instrux + 2031,
    instrux + 2032,
    instrux + 2033,
    instrux + 2034,
    instrux + 2035,
    instrux + 2036,
    instrux + 2037,
    instrux + 2038,
    instrux + 2039,
    instrux + 2040,
    instrux + 2041,
    instrux + 2042,
    instrux + 2043,
    instrux + 2044,
    instrux + 2045,
    instrux + 2046,
    instrux + 2047,
    instrux + 2048,
    instrux + 2049,
    instrux + 2050,
    instrux + 2051,
    instrux + 2052,
    instrux + 2053,
    instrux + 2054,
    instrux + 2055,
    instrux + 2056,
    instrux + 2057,
    instrux + 2058,
    instrux + 2059,
    instrux + 2060,
    instrux + 2061,
    instrux + 2062,
    instrux + 2063,
    instrux + 2064,
    instrux + 2065,
    instrux + 2066,
    instrux + 2067,
    instrux + 2068,
    instrux + 2069,
    instrux + 2070,
    instrux + 2071,
    instrux + 2072,
    instrux + 2073,
    instrux + 2074,
    instrux + 2075,
    instrux + 2076,
    instrux + 2077,
    instrux + 2078,
    instrux + 2079,
    instrux + 2080,
    instrux + 2081,
    instrux + 2082,
    instrux + 2083,
    instrux + 2084,
    instrux + 2085,
    instrux + 2086,
    instrux + 2087,
    instrux + 2088,
    instrux + 2089,
    instrux + 2090,
    instrux + 2091,
    instrux + 2092,
    instrux + 2093,
    instrux + 2094,
    instrux + 2095,
    instrux + 2096,
    instrux + 2097,
    instrux + 2098,
    instrux + 2099,
    instrux + 2100,
    instrux + 2101,
    instrux + 2102,
    instrux + 2103,
    instrux + 2104,
    instrux + 2105,
    instrux + 2106,
    instrux + 2107,
    instrux + 2108,
    instrux + 2109,
    instrux + 2110,
    instrux + 2111,
    instrux + 2112,
    instrux + 2113,
    instrux + 2114,
    instrux + 2115,
    instrux + 2116,
    instrux + 2117,
    instrux + 2118,
    instrux + 2119,
    instrux + 2120,
    instrux + 2121,
    instrux + 2122,
    instrux + 2123,
    instrux + 2124,
    instrux + 2125,
    instrux + 2126,
    instrux + 2127,
    instrux + 2128,
    instrux + 2129,
    instrux + 2130,
    instrux + 2131,
    instrux + 2132,
    instrux + 2133,
    instrux + 2134,
    instrux + 2135,
    instrux + 2136,
    instrux + 2137,
    instrux + 2138,
    instrux + 2139,
};

static const struct itemplate * const itable_vex010C6[] = {
    instrux + 2830,
    instrux + 2831,
    instrux + 2832,
    instrux + 2833,
};

static const struct itemplate * const itable_vex01110[] = {
    instrux + 2523,
    instrux + 2525,
};

static const struct itemplate * const itable_vex01111[] = {
    instrux + 2524,
    instrux + 2526,
};

static const struct itemplate * const itable_vex01112[] = {
    instrux + 2485,
    instrux + 2486,
};

static const struct itemplate * const itable_vex01113[] = {
    instrux + 2487,
};

static const struct itemplate * const itable_vex01114[] = {
    instrux + 2869,
    instrux + 2870,
    instrux + 2871,
    instrux + 2872,
};

static const struct itemplate * const itable_vex01115[] = {
    instrux + 2861,
    instrux + 2862,
    instrux + 2863,
    instrux + 2864,
};

static const struct itemplate * const itable_vex01116[] = {
    instrux + 2477,
    instrux + 2478,
};

static const struct itemplate * const itable_vex01117[] = {
    instrux + 2479,
};

static const struct itemplate * const itable_vex01128[] = {
    instrux + 2447,
    instrux + 2449,
};

static const struct itemplate * const itable_vex01129[] = {
    instrux + 2448,
    instrux + 2450,
    instrux + 2604,
    instrux + 2605,
};

static const struct itemplate * const itable_vex0112B[] = {
    instrux + 2503,
    instrux + 2504,
};

static const struct itemplate * const itable_vex0112E[] = {
    instrux + 2859,
};

static const struct itemplate * const itable_vex0112F[] = {
    instrux + 2328,
};

static const struct itemplate * const itable_vex01137[] = {
    instrux + 2612,
    instrux + 2613,
};

static const struct itemplate * const itable_vex01150[] = {
    instrux + 2491,
    instrux + 2492,
    instrux + 2493,
    instrux + 2494,
};

static const struct itemplate * const itable_vex01151[] = {
    instrux + 2834,
    instrux + 2835,
};

static const struct itemplate * const itable_vex01154[] = {
    instrux + 1728,
    instrux + 1729,
    instrux + 1730,
    instrux + 1731,
};

static const struct itemplate * const itable_vex01155[] = {
    instrux + 1736,
    instrux + 1737,
    instrux + 1738,
    instrux + 1739,
};

static const struct itemplate * const itable_vex01156[] = {
    instrux + 2545,
    instrux + 2546,
    instrux + 2547,
    instrux + 2548,
};

static const struct itemplate * const itable_vex01157[] = {
    instrux + 2877,
    instrux + 2878,
    instrux + 2879,
    instrux + 2880,
};

static const struct itemplate * const itable_vex01158[] = {
    instrux + 1708,
    instrux + 1709,
    instrux + 1710,
    instrux + 1711,
};

static const struct itemplate * const itable_vex01159[] = {
    instrux + 2533,
    instrux + 2534,
    instrux + 2535,
    instrux + 2536,
};

static const struct itemplate * const itable_vex0115A[] = {
    instrux + 2338,
    instrux + 2339,
    instrux + 2340,
    instrux + 2341,
};

static const struct itemplate * const itable_vex0115B[] = {
    instrux + 2342,
    instrux + 2343,
};

static const struct itemplate * const itable_vex0115C[] = {
    instrux + 2843,
    instrux + 2844,
    instrux + 2845,
    instrux + 2846,
};

static const struct itemplate * const itable_vex0115D[] = {
    instrux + 2435,
    instrux + 2436,
    instrux + 2437,
    instrux + 2438,
};

static const struct itemplate * const itable_vex0115E[] = {
    instrux + 2372,
    instrux + 2373,
    instrux + 2374,
    instrux + 2375,
};

static const struct itemplate * const itable_vex0115F[] = {
    instrux + 2423,
    instrux + 2424,
    instrux + 2425,
    instrux + 2426,
};

static const struct itemplate * const itable_vex01160[] = {
    instrux + 2800,
    instrux + 2801,
};

static const struct itemplate * const itable_vex01161[] = {
    instrux + 2802,
    instrux + 2803,
};

static const struct itemplate * const itable_vex01162[] = {
    instrux + 2804,
    instrux + 2805,
};

static const struct itemplate * const itable_vex01163[] = {
    instrux + 2556,
    instrux + 2557,
};

static const struct itemplate * const itable_vex01164[] = {
    instrux + 2606,
    instrux + 2607,
};

static const struct itemplate * const itable_vex01165[] = {
    instrux + 2608,
    instrux + 2609,
};

static const struct itemplate * const itable_vex01166[] = {
    instrux + 2610,
    instrux + 2611,
};

static const struct itemplate * const itable_vex01167[] = {
    instrux + 2560,
    instrux + 2561,
};

static const struct itemplate * const itable_vex01168[] = {
    instrux + 2792,
    instrux + 2793,
};

static const struct itemplate * const itable_vex01169[] = {
    instrux + 2794,
    instrux + 2795,
};

static const struct itemplate * const itable_vex0116A[] = {
    instrux + 2796,
    instrux + 2797,
};

static const struct itemplate * const itable_vex0116B[] = {
    instrux + 2558,
    instrux + 2559,
};

static const struct itemplate * const itable_vex0116C[] = {
    instrux + 2806,
    instrux + 2807,
};

static const struct itemplate * const itable_vex0116D[] = {
    instrux + 2798,
    instrux + 2799,
};

static const struct itemplate * const itable_vex0116E[] = {
    instrux + 2455,
    instrux + 2459,
};

static const struct itemplate * const itable_vex0116F[] = {
    instrux + 2463,
    instrux + 2465,
    instrux + 2467,
};

static const struct itemplate * const itable_vex01170[] = {
    instrux + 2729,
};

static const struct itemplate * const itable_vex01171[] = {
    instrux + 2744,
    instrux + 2745,
    instrux + 2756,
    instrux + 2757,
    instrux + 2764,
    instrux + 2765,
};

static const struct itemplate * const itable_vex01172[] = {
    instrux + 2748,
    instrux + 2749,
    instrux + 2760,
    instrux + 2761,
    instrux + 2768,
    instrux + 2769,
};

static const struct itemplate * const itable_vex01173[] = {
    instrux + 2738,
    instrux + 2739,
    instrux + 2740,
    instrux + 2741,
    instrux + 2752,
    instrux + 2753,
    instrux + 2772,
    instrux + 2773,
};

static const struct itemplate * const itable_vex01174[] = {
    instrux + 2598,
    instrux + 2599,
};

static const struct itemplate * const itable_vex01175[] = {
    instrux + 2600,
    instrux + 2601,
};

static const struct itemplate * const itable_vex01176[] = {
    instrux + 2602,
    instrux + 2603,
};

static const struct itemplate * const itable_vex0117C[] = {
    instrux + 2392,
    instrux + 2393,
    instrux + 2394,
    instrux + 2395,
};

static const struct itemplate * const itable_vex0117D[] = {
    instrux + 2400,
    instrux + 2401,
    instrux + 2402,
    instrux + 2403,
};

static const struct itemplate * const itable_vex0117E[] = {
    instrux + 2456,
    instrux + 2460,
};

static const struct itemplate * const itable_vex0117F[] = {
    instrux + 2464,
    instrux + 2466,
    instrux + 2468,
};

static const struct itemplate * const itable_vex011C2[] = {
    instrux + 1764,
    instrux + 1765,
    instrux + 1766,
    instrux + 1767,
    instrux + 1768,
    instrux + 1769,
    instrux + 1770,
    instrux + 1771,
    instrux + 1772,
    instrux + 1773,
    instrux + 1774,
    instrux + 1775,
    instrux + 1776,
    instrux + 1777,
    instrux + 1778,
    instrux + 1779,
    instrux + 1780,
    instrux + 1781,
    instrux + 1782,
    instrux + 1783,
    instrux + 1784,
    instrux + 1785,
    instrux + 1786,
    instrux + 1787,
    instrux + 1788,
    instrux + 1789,
    instrux + 1790,
    instrux + 1791,
    instrux + 1792,
    instrux + 1793,
    instrux + 1794,
    instrux + 1795,
    instrux + 1796,
    instrux + 1797,
    instrux + 1798,
    instrux + 1799,
    instrux + 1800,
    instrux + 1801,
    instrux + 1802,
    instrux + 1803,
    instrux + 1804,
    instrux + 1805,
    instrux + 1806,
    instrux + 1807,
    instrux + 1808,
    instrux + 1809,
    instrux + 1810,
    instrux + 1811,
    instrux + 1812,
    instrux + 1813,
    instrux + 1814,
    instrux + 1815,
    instrux + 1816,
    instrux + 1817,
    instrux + 1818,
    instrux + 1819,
    instrux + 1820,
    instrux + 1821,
    instrux + 1822,
    instrux + 1823,
    instrux + 1824,
    instrux + 1825,
    instrux + 1826,
    instrux + 1827,
    instrux + 1828,
    instrux + 1829,
    instrux + 1830,
    instrux + 1831,
    instrux + 1832,
    instrux + 1833,
    instrux + 1834,
    instrux + 1835,
    instrux + 1836,
    instrux + 1837,
    instrux + 1838,
    instrux + 1839,
    instrux + 1840,
    instrux + 1841,
    instrux + 1842,
    instrux + 1843,
    instrux + 1844,
    instrux + 1845,
    instrux + 1846,
    instrux + 1847,
    instrux + 1848,
    instrux + 1849,
    instrux + 1850,
    instrux + 1851,
    instrux + 1852,
    instrux + 1853,
    instrux + 1854,
    instrux + 1855,
    instrux + 1856,
    instrux + 1857,
    instrux + 1858,
    instrux + 1859,
    instrux + 1860,
    instrux + 1861,
    instrux + 1862,
    instrux + 1863,
    instrux + 1864,
    instrux + 1865,
    instrux + 1866,
    instrux + 1867,
    instrux + 1868,
    instrux + 1869,
    instrux + 1870,
    instrux + 1871,
    instrux + 1872,
    instrux + 1873,
    instrux + 1874,
    instrux + 1875,
    instrux + 1876,
    instrux + 1877,
    instrux + 1878,
    instrux + 1879,
    instrux + 1880,
    instrux + 1881,
    instrux + 1882,
    instrux + 1883,
    instrux + 1884,
    instrux + 1885,
    instrux + 1886,
    instrux + 1887,
    instrux + 1888,
    instrux + 1889,
    instrux + 1890,
    instrux + 1891,
    instrux + 1892,
    instrux + 1893,
    instrux + 1894,
    instrux + 1895,
    instrux + 1896,
    instrux + 1897,
    instrux + 1898,
    instrux + 1899,
    instrux + 1900,
    instrux + 1901,
    instrux + 1902,
    instrux + 1903,
    instrux + 1904,
    instrux + 1905,
    instrux + 1906,
    instrux + 1907,
    instrux + 1908,
    instrux + 1909,
    instrux + 1910,
    instrux + 1911,
    instrux + 1912,
    instrux + 1913,
    instrux + 1914,
    instrux + 1915,
    instrux + 1916,
    instrux + 1917,
    instrux + 1918,
    instrux + 1919,
    instrux + 1920,
    instrux + 1921,
    instrux + 1922,
    instrux + 1923,
    instrux + 1924,
    instrux + 1925,
    instrux + 1926,
    instrux + 1927,
    instrux + 1928,
    instrux + 1929,
    instrux + 1930,
    instrux + 1931,
    instrux + 1932,
    instrux + 1933,
    instrux + 1934,
    instrux + 1935,
    instrux + 1936,
    instrux + 1937,
    instrux + 1938,
    instrux + 1939,
    instrux + 1940,
    instrux + 1941,
    instrux + 1942,
    instrux + 1943,
    instrux + 1944,
    instrux + 1945,
    instrux + 1946,
    instrux + 1947,
    instrux + 1948,
    instrux + 1949,
    instrux + 1950,
    instrux + 1951,
};

static const struct itemplate * const itable_vex011C4[] = {
    instrux + 2653,
    instrux + 2654,
    instrux + 2655,
    instrux + 2656,
    instrux + 2657,
    instrux + 2658,
};

static const struct itemplate * const itable_vex011C5[] = {
    instrux + 2626,
    instrux + 2627,
};

static const struct itemplate * const itable_vex011C6[] = {
    instrux + 2826,
    instrux + 2827,
    instrux + 2828,
    instrux + 2829,
};

static const struct itemplate * const itable_vex011D0[] = {
    instrux + 1720,
    instrux + 1721,
    instrux + 1722,
    instrux + 1723,
};

static const struct itemplate * const itable_vex011D1[] = {
    instrux + 2762,
    instrux + 2763,
};

static const struct itemplate * const itable_vex011D2[] = {
    instrux + 2766,
    instrux + 2767,
};

static const struct itemplate * const itable_vex011D3[] = {
    instrux + 2770,
    instrux + 2771,
};

static const struct itemplate * const itable_vex011D4[] = {
    instrux + 2570,
    instrux + 2571,
};

static const struct itemplate * const itable_vex011D5[] = {
    instrux + 2715,
    instrux + 2716,
};

static const struct itemplate * const itable_vex011D6[] = {
    instrux + 2458,
};

static const struct itemplate * const itable_vex011D7[] = {
    instrux + 2695,
    instrux + 2696,
};

static const struct itemplate * const itable_vex011D8[] = {
    instrux + 2788,
    instrux + 2789,
};

static const struct itemplate * const itable_vex011D9[] = {
    instrux + 2790,
    instrux + 2791,
};

static const struct itemplate * const itable_vex011DA[] = {
    instrux + 2689,
    instrux + 2690,
};

static const struct itemplate * const itable_vex011DB[] = {
    instrux + 2582,
    instrux + 2583,
};

static const struct itemplate * const itable_vex011DC[] = {
    instrux + 2576,
    instrux + 2577,
};

static const struct itemplate * const itable_vex011DD[] = {
    instrux + 2578,
    instrux + 2579,
};

static const struct itemplate * const itable_vex011DE[] = {
    instrux + 2677,
    instrux + 2678,
};

static const struct itemplate * const itable_vex011DF[] = {
    instrux + 2584,
    instrux + 2585,
};

static const struct itemplate * const itable_vex011E0[] = {
    instrux + 2586,
    instrux + 2587,
};

static const struct itemplate * const itable_vex011E1[] = {
    instrux + 2754,
    instrux + 2755,
};

static const struct itemplate * const itable_vex011E2[] = {
    instrux + 2758,
    instrux + 2759,
};

static const struct itemplate * const itable_vex011E3[] = {
    instrux + 2588,
    instrux + 2589,
};

static const struct itemplate * const itable_vex011E4[] = {
    instrux + 2709,
    instrux + 2710,
};

static const struct itemplate * const itable_vex011E5[] = {
    instrux + 2713,
    instrux + 2714,
};

static const struct itemplate * const itable_vex011E6[] = {
    instrux + 2362,
    instrux + 2363,
    instrux + 2364,
    instrux + 2365,
};

static const struct itemplate * const itable_vex011E7[] = {
    instrux + 2499,
    instrux + 2500,
    instrux + 2501,
};

static const struct itemplate * const itable_vex011E8[] = {
    instrux + 2784,
    instrux + 2785,
};

static const struct itemplate * const itable_vex011E9[] = {
    instrux + 2786,
    instrux + 2787,
};

static const struct itemplate * const itable_vex011EA[] = {
    instrux + 2685,
    instrux + 2686,
};

static const struct itemplate * const itable_vex011EB[] = {
    instrux + 2723,
    instrux + 2724,
};

static const struct itemplate * const itable_vex011EC[] = {
    instrux + 2572,
    instrux + 2573,
};

static const struct itemplate * const itable_vex011ED[] = {
    instrux + 2574,
    instrux + 2575,
};

static const struct itemplate * const itable_vex011EE[] = {
    instrux + 2673,
    instrux + 2674,
};

static const struct itemplate * const itable_vex011EF[] = {
    instrux + 2808,
    instrux + 2809,
};

static const struct itemplate * const itable_vex011F1[] = {
    instrux + 2742,
    instrux + 2743,
};

static const struct itemplate * const itable_vex011F2[] = {
    instrux + 2746,
    instrux + 2747,
};

static const struct itemplate * const itable_vex011F3[] = {
    instrux + 2750,
    instrux + 2751,
};

static const struct itemplate * const itable_vex011F4[] = {
    instrux + 2719,
    instrux + 2720,
};

static const struct itemplate * const itable_vex011F5[] = {
    instrux + 2667,
    instrux + 2668,
};

static const struct itemplate * const itable_vex011F6[] = {
    instrux + 2725,
    instrux + 2726,
};

static const struct itemplate * const itable_vex011F7[] = {
    instrux + 2414,
};

static const struct itemplate * const itable_vex011F8[] = {
    instrux + 2776,
    instrux + 2777,
};

static const struct itemplate * const itable_vex011F9[] = {
    instrux + 2778,
    instrux + 2779,
};

static const struct itemplate * const itable_vex011FA[] = {
    instrux + 2780,
    instrux + 2781,
};

static const struct itemplate * const itable_vex011FB[] = {
    instrux + 2782,
    instrux + 2783,
};

static const struct itemplate * const itable_vex011FC[] = {
    instrux + 2564,
    instrux + 2565,
};

static const struct itemplate * const itable_vex011FD[] = {
    instrux + 2566,
    instrux + 2567,
};

static const struct itemplate * const itable_vex011FE[] = {
    instrux + 2568,
    instrux + 2569,
};

static const struct itemplate * const itable_vex01210[] = {
    instrux + 2517,
    instrux + 2518,
    instrux + 2519,
};

static const struct itemplate * const itable_vex01211[] = {
    instrux + 2520,
    instrux + 2521,
    instrux + 2522,
};

static const struct itemplate * const itable_vex01212[] = {
    instrux + 2515,
    instrux + 2516,
};

static const struct itemplate * const itable_vex01216[] = {
    instrux + 2513,
    instrux + 2514,
};

static const struct itemplate * const itable_vex0122A[] = {
    instrux + 2354,
    instrux + 2355,
    instrux + 2356,
    instrux + 2357,
};

static const struct itemplate * const itable_vex0122C[] = {
    instrux + 2370,
    instrux + 2371,
};

static const struct itemplate * const itable_vex0122D[] = {
    instrux + 2360,
    instrux + 2361,
};

static const struct itemplate * const itable_vex01251[] = {
    instrux + 2840,
    instrux + 2841,
};

static const struct itemplate * const itable_vex01252[] = {
    instrux + 2816,
    instrux + 2817,
};

static const struct itemplate * const itable_vex01253[] = {
    instrux + 2812,
    instrux + 2813,
};

static const struct itemplate * const itable_vex01258[] = {
    instrux + 1718,
    instrux + 1719,
};

static const struct itemplate * const itable_vex01259[] = {
    instrux + 2543,
    instrux + 2544,
};

static const struct itemplate * const itable_vex0125A[] = {
    instrux + 2358,
    instrux + 2359,
};

static const struct itemplate * const itable_vex0125B[] = {
    instrux + 2366,
    instrux + 2367,
};

static const struct itemplate * const itable_vex0125C[] = {
    instrux + 2853,
    instrux + 2854,
};

static const struct itemplate * const itable_vex0125D[] = {
    instrux + 2445,
    instrux + 2446,
};

static const struct itemplate * const itable_vex0125E[] = {
    instrux + 2382,
    instrux + 2383,
};

static const struct itemplate * const itable_vex0125F[] = {
    instrux + 2433,
    instrux + 2434,
};

static const struct itemplate * const itable_vex0126F[] = {
    instrux + 2469,
    instrux + 2471,
    instrux + 2473,
};

static const struct itemplate * const itable_vex01270[] = {
    instrux + 2730,
};

static const struct itemplate * const itable_vex0127E[] = {
    instrux + 2457,
};

static const struct itemplate * const itable_vex0127F[] = {
    instrux + 2470,
    instrux + 2472,
    instrux + 2474,
};

static const struct itemplate * const itable_vex012C2[] = {
    instrux + 2234,
    instrux + 2235,
    instrux + 2236,
    instrux + 2237,
    instrux + 2238,
    instrux + 2239,
    instrux + 2240,
    instrux + 2241,
    instrux + 2242,
    instrux + 2243,
    instrux + 2244,
    instrux + 2245,
    instrux + 2246,
    instrux + 2247,
    instrux + 2248,
    instrux + 2249,
    instrux + 2250,
    instrux + 2251,
    instrux + 2252,
    instrux + 2253,
    instrux + 2254,
    instrux + 2255,
    instrux + 2256,
    instrux + 2257,
    instrux + 2258,
    instrux + 2259,
    instrux + 2260,
    instrux + 2261,
    instrux + 2262,
    instrux + 2263,
    instrux + 2264,
    instrux + 2265,
    instrux + 2266,
    instrux + 2267,
    instrux + 2268,
    instrux + 2269,
    instrux + 2270,
    instrux + 2271,
    instrux + 2272,
    instrux + 2273,
    instrux + 2274,
    instrux + 2275,
    instrux + 2276,
    instrux + 2277,
    instrux + 2278,
    instrux + 2279,
    instrux + 2280,
    instrux + 2281,
    instrux + 2282,
    instrux + 2283,
    instrux + 2284,
    instrux + 2285,
    instrux + 2286,
    instrux + 2287,
    instrux + 2288,
    instrux + 2289,
    instrux + 2290,
    instrux + 2291,
    instrux + 2292,
    instrux + 2293,
    instrux + 2294,
    instrux + 2295,
    instrux + 2296,
    instrux + 2297,
    instrux + 2298,
    instrux + 2299,
    instrux + 2300,
    instrux + 2301,
    instrux + 2302,
    instrux + 2303,
    instrux + 2304,
    instrux + 2305,
    instrux + 2306,
    instrux + 2307,
    instrux + 2308,
    instrux + 2309,
    instrux + 2310,
    instrux + 2311,
    instrux + 2312,
    instrux + 2313,
    instrux + 2314,
    instrux + 2315,
    instrux + 2316,
    instrux + 2317,
    instrux + 2318,
    instrux + 2319,
    instrux + 2320,
    instrux + 2321,
    instrux + 2322,
    instrux + 2323,
    instrux + 2324,
    instrux + 2325,
    instrux + 2326,
    instrux + 2327,
};

static const struct itemplate * const itable_vex012E6[] = {
    instrux + 2330,
    instrux + 2331,
};

static const struct itemplate * const itable_vex01310[] = {
    instrux + 2507,
    instrux + 2508,
    instrux + 2509,
};

static const struct itemplate * const itable_vex01311[] = {
    instrux + 2510,
    instrux + 2511,
    instrux + 2512,
};

static const struct itemplate * const itable_vex01312[] = {
    instrux + 2461,
    instrux + 2462,
};

static const struct itemplate * const itable_vex0132A[] = {
    instrux + 2350,
    instrux + 2351,
    instrux + 2352,
    instrux + 2353,
};

static const struct itemplate * const itable_vex0132C[] = {
    instrux + 2368,
    instrux + 2369,
};

static const struct itemplate * const itable_vex0132D[] = {
    instrux + 2346,
    instrux + 2347,
};

static const struct itemplate * const itable_vex01351[] = {
    instrux + 2838,
    instrux + 2839,
};

static const struct itemplate * const itable_vex01358[] = {
    instrux + 1716,
    instrux + 1717,
};

static const struct itemplate * const itable_vex01359[] = {
    instrux + 2541,
    instrux + 2542,
};

static const struct itemplate * const itable_vex0135A[] = {
    instrux + 2348,
    instrux + 2349,
};

static const struct itemplate * const itable_vex0135C[] = {
    instrux + 2851,
    instrux + 2852,
};

static const struct itemplate * const itable_vex0135D[] = {
    instrux + 2443,
    instrux + 2444,
};

static const struct itemplate * const itable_vex0135E[] = {
    instrux + 2380,
    instrux + 2381,
};

static const struct itemplate * const itable_vex0135F[] = {
    instrux + 2431,
    instrux + 2432,
};

static const struct itemplate * const itable_vex01370[] = {
    instrux + 2731,
};

static const struct itemplate * const itable_vex0137C[] = {
    instrux + 2396,
    instrux + 2397,
    instrux + 2398,
    instrux + 2399,
};

static const struct itemplate * const itable_vex0137D[] = {
    instrux + 2404,
    instrux + 2405,
    instrux + 2406,
    instrux + 2407,
};

static const struct itemplate * const itable_vex013C2[] = {
    instrux + 2140,
    instrux + 2141,
    instrux + 2142,
    instrux + 2143,
    instrux + 2144,
    instrux + 2145,
    instrux + 2146,
    instrux + 2147,
    instrux + 2148,
    instrux + 2149,
    instrux + 2150,
    instrux + 2151,
    instrux + 2152,
    instrux + 2153,
    instrux + 2154,
    instrux + 2155,
    instrux + 2156,
    instrux + 2157,
    instrux + 2158,
    instrux + 2159,
    instrux + 2160,
    instrux + 2161,
    instrux + 2162,
    instrux + 2163,
    instrux + 2164,
    instrux + 2165,
    instrux + 2166,
    instrux + 2167,
    instrux + 2168,
    instrux + 2169,
    instrux + 2170,
    instrux + 2171,
    instrux + 2172,
    instrux + 2173,
    instrux + 2174,
    instrux + 2175,
    instrux + 2176,
    instrux + 2177,
    instrux + 2178,
    instrux + 2179,
    instrux + 2180,
    instrux + 2181,
    instrux + 2182,
    instrux + 2183,
    instrux + 2184,
    instrux + 2185,
    instrux + 2186,
    instrux + 2187,
    instrux + 2188,
    instrux + 2189,
    instrux + 2190,
    instrux + 2191,
    instrux + 2192,
    instrux + 2193,
    instrux + 2194,
    instrux + 2195,
    instrux + 2196,
    instrux + 2197,
    instrux + 2198,
    instrux + 2199,
    instrux + 2200,
    instrux + 2201,
    instrux + 2202,
    instrux + 2203,
    instrux + 2204,
    instrux + 2205,
    instrux + 2206,
    instrux + 2207,
    instrux + 2208,
    instrux + 2209,
    instrux + 2210,
    instrux + 2211,
    instrux + 2212,
    instrux + 2213,
    instrux + 2214,
    instrux + 2215,
    instrux + 2216,
    instrux + 2217,
    instrux + 2218,
    instrux + 2219,
    instrux + 2220,
    instrux + 2221,
    instrux + 2222,
    instrux + 2223,
    instrux + 2224,
    instrux + 2225,
    instrux + 2226,
    instrux + 2227,
    instrux + 2228,
    instrux + 2229,
    instrux + 2230,
    instrux + 2231,
    instrux + 2232,
    instrux + 2233,
};

static const struct itemplate * const itable_vex013D0[] = {
    instrux + 1724,
    instrux + 1725,
    instrux + 1726,
    instrux + 1727,
};

static const struct itemplate * const itable_vex013E6[] = {
    instrux + 2334,
    instrux + 2335,
    instrux + 2336,
    instrux + 2337,
};

static const struct itemplate * const itable_vex013F0[] = {
    instrux + 2411,
    instrux + 2412,
};

static const struct itemplate * const itable_vex02100[] = {
    instrux + 2727,
    instrux + 2728,
};

static const struct itemplate * const itable_vex02101[] = {
    instrux + 2634,
    instrux + 2635,
};

static const struct itemplate * const itable_vex02102[] = {
    instrux + 2636,
    instrux + 2637,
};

static const struct itemplate * const itable_vex02103[] = {
    instrux + 2638,
    instrux + 2639,
};

static const struct itemplate * const itable_vex02104[] = {
    instrux + 2669,
    instrux + 2670,
};

static const struct itemplate * const itable_vex02105[] = {
    instrux + 2641,
    instrux + 2642,
};

static const struct itemplate * const itable_vex02106[] = {
    instrux + 2643,
    instrux + 2644,
};

static const struct itemplate * const itable_vex02107[] = {
    instrux + 2645,
    instrux + 2646,
};

static const struct itemplate * const itable_vex02108[] = {
    instrux + 2732,
    instrux + 2733,
};

static const struct itemplate * const itable_vex02109[] = {
    instrux + 2734,
    instrux + 2735,
};

static const struct itemplate * const itable_vex0210A[] = {
    instrux + 2736,
    instrux + 2737,
};

static const struct itemplate * const itable_vex0210B[] = {
    instrux + 2711,
    instrux + 2712,
};

static const struct itemplate * const itable_vex0210C[] = {
    instrux + 2618,
    instrux + 2619,
};

static const struct itemplate * const itable_vex0210D[] = {
    instrux + 2614,
    instrux + 2615,
};

static const struct itemplate * const itable_vex0210E[] = {
    instrux + 2855,
    instrux + 2856,
};

static const struct itemplate * const itable_vex0210F[] = {
    instrux + 2857,
    instrux + 2858,
};

static const struct itemplate * const itable_vex02113[] = {
    instrux + 3105,
    instrux + 3106,
};

static const struct itemplate * const itable_vex02117[] = {
    instrux + 2774,
    instrux + 2775,
};

static const struct itemplate * const itable_vex02118[] = {
    instrux + 1760,
    instrux + 1761,
};

static const struct itemplate * const itable_vex02119[] = {
    instrux + 1762,
};

static const struct itemplate * const itable_vex0211A[] = {
    instrux + 1763,
};

static const struct itemplate * const itable_vex0211C[] = {
    instrux + 2553,
};

static const struct itemplate * const itable_vex0211D[] = {
    instrux + 2554,
};

static const struct itemplate * const itable_vex0211E[] = {
    instrux + 2555,
};

static const struct itemplate * const itable_vex02120[] = {
    instrux + 2697,
};

static const struct itemplate * const itable_vex02121[] = {
    instrux + 2698,
};

static const struct itemplate * const itable_vex02122[] = {
    instrux + 2699,
};

static const struct itemplate * const itable_vex02123[] = {
    instrux + 2700,
};

static const struct itemplate * const itable_vex02124[] = {
    instrux + 2701,
};

static const struct itemplate * const itable_vex02125[] = {
    instrux + 2702,
};

static const struct itemplate * const itable_vex02128[] = {
    instrux + 2721,
    instrux + 2722,
};

static const struct itemplate * const itable_vex0212A[] = {
    instrux + 2502,
};

static const struct itemplate * const itable_vex0212B[] = {
    instrux + 2562,
    instrux + 2563,
};

static const struct itemplate * const itable_vex0212C[] = {
    instrux + 2415,
    instrux + 2416,
};

static const struct itemplate * const itable_vex0212D[] = {
    instrux + 2419,
    instrux + 2420,
};

static const struct itemplate * const itable_vex0212E[] = {
    instrux + 2417,
    instrux + 2418,
};

static const struct itemplate * const itable_vex0212F[] = {
    instrux + 2421,
    instrux + 2422,
};

static const struct itemplate * const itable_vex02130[] = {
    instrux + 2703,
};

static const struct itemplate * const itable_vex02131[] = {
    instrux + 2704,
};

static const struct itemplate * const itable_vex02132[] = {
    instrux + 2705,
};

static const struct itemplate * const itable_vex02133[] = {
    instrux + 2706,
};

static const struct itemplate * const itable_vex02134[] = {
    instrux + 2707,
};

static const struct itemplate * const itable_vex02135[] = {
    instrux + 2708,
};

static const struct itemplate * const itable_vex02138[] = {
    instrux + 2683,
    instrux + 2684,
};

static const struct itemplate * const itable_vex02139[] = {
    instrux + 2687,
    instrux + 2688,
};

static const struct itemplate * const itable_vex0213A[] = {
    instrux + 2691,
    instrux + 2692,
};

static const struct itemplate * const itable_vex0213B[] = {
    instrux + 2693,
    instrux + 2694,
};

static const struct itemplate * const itable_vex0213C[] = {
    instrux + 2671,
    instrux + 2672,
};

static const struct itemplate * const itable_vex0213D[] = {
    instrux + 2675,
    instrux + 2676,
};

static const struct itemplate * const itable_vex0213E[] = {
    instrux + 2679,
    instrux + 2680,
};

static const struct itemplate * const itable_vex0213F[] = {
    instrux + 2681,
    instrux + 2682,
};

static const struct itemplate * const itable_vex02140[] = {
    instrux + 2717,
    instrux + 2718,
};

static const struct itemplate * const itable_vex02141[] = {
    instrux + 2640,
};

static const struct itemplate * const itable_vex02196[] = {
    instrux + 2926,
    instrux + 2927,
    instrux + 2928,
    instrux + 2929,
    instrux + 2930,
    instrux + 2931,
    instrux + 2932,
    instrux + 2933,
};

static const struct itemplate * const itable_vex02197[] = {
    instrux + 2974,
    instrux + 2975,
    instrux + 2976,
    instrux + 2977,
    instrux + 2978,
    instrux + 2979,
    instrux + 2980,
    instrux + 2981,
};

static const struct itemplate * const itable_vex02198[] = {
    instrux + 2902,
    instrux + 2903,
    instrux + 2904,
    instrux + 2905,
    instrux + 2906,
    instrux + 2907,
    instrux + 2908,
    instrux + 2909,
};

static const struct itemplate * const itable_vex02199[] = {
    instrux + 3046,
    instrux + 3047,
    instrux + 3048,
    instrux + 3049,
};

static const struct itemplate * const itable_vex0219A[] = {
    instrux + 2950,
    instrux + 2951,
    instrux + 2952,
    instrux + 2953,
    instrux + 2954,
    instrux + 2955,
    instrux + 2956,
    instrux + 2957,
};

static const struct itemplate * const itable_vex0219B[] = {
    instrux + 3058,
    instrux + 3059,
    instrux + 3060,
    instrux + 3061,
};

static const struct itemplate * const itable_vex0219C[] = {
    instrux + 2998,
    instrux + 2999,
    instrux + 3000,
    instrux + 3001,
    instrux + 3002,
    instrux + 3003,
    instrux + 3004,
    instrux + 3005,
};

static const struct itemplate * const itable_vex0219D[] = {
    instrux + 3070,
    instrux + 3071,
    instrux + 3072,
    instrux + 3073,
};

static const struct itemplate * const itable_vex0219E[] = {
    instrux + 3022,
    instrux + 3023,
    instrux + 3024,
    instrux + 3025,
    instrux + 3026,
    instrux + 3027,
    instrux + 3028,
    instrux + 3029,
};

static const struct itemplate * const itable_vex0219F[] = {
    instrux + 3082,
    instrux + 3083,
    instrux + 3084,
    instrux + 3085,
};

static const struct itemplate * const itable_vex021A6[] = {
    instrux + 2934,
    instrux + 2935,
    instrux + 2936,
    instrux + 2937,
    instrux + 2938,
    instrux + 2939,
    instrux + 2940,
    instrux + 2941,
};

static const struct itemplate * const itable_vex021A7[] = {
    instrux + 2982,
    instrux + 2983,
    instrux + 2984,
    instrux + 2985,
    instrux + 2986,
    instrux + 2987,
    instrux + 2988,
    instrux + 2989,
};

static const struct itemplate * const itable_vex021A8[] = {
    instrux + 2910,
    instrux + 2911,
    instrux + 2912,
    instrux + 2913,
    instrux + 2914,
    instrux + 2915,
    instrux + 2916,
    instrux + 2917,
};

static const struct itemplate * const itable_vex021A9[] = {
    instrux + 3050,
    instrux + 3051,
    instrux + 3052,
    instrux + 3053,
};

static const struct itemplate * const itable_vex021AA[] = {
    instrux + 2958,
    instrux + 2959,
    instrux + 2960,
    instrux + 2961,
    instrux + 2962,
    instrux + 2963,
    instrux + 2964,
    instrux + 2965,
};

static const struct itemplate * const itable_vex021AB[] = {
    instrux + 3062,
    instrux + 3063,
    instrux + 3064,
    instrux + 3065,
};

static const struct itemplate * const itable_vex021AC[] = {
    instrux + 3006,
    instrux + 3007,
    instrux + 3008,
    instrux + 3009,
    instrux + 3010,
    instrux + 3011,
    instrux + 3012,
    instrux + 3013,
};

static const struct itemplate * const itable_vex021AD[] = {
    instrux + 3074,
    instrux + 3075,
    instrux + 3076,
    instrux + 3077,
};

static const struct itemplate * const itable_vex021AE[] = {
    instrux + 3030,
    instrux + 3031,
    instrux + 3032,
    instrux + 3033,
    instrux + 3034,
    instrux + 3035,
    instrux + 3036,
    instrux + 3037,
};

static const struct itemplate * const itable_vex021AF[] = {
    instrux + 3086,
    instrux + 3087,
    instrux + 3088,
    instrux + 3089,
};

static const struct itemplate * const itable_vex021B6[] = {
    instrux + 2942,
    instrux + 2943,
    instrux + 2944,
    instrux + 2945,
    instrux + 2946,
    instrux + 2947,
    instrux + 2948,
    instrux + 2949,
};

static const struct itemplate * const itable_vex021B7[] = {
    instrux + 2990,
    instrux + 2991,
    instrux + 2992,
    instrux + 2993,
    instrux + 2994,
    instrux + 2995,
    instrux + 2996,
    instrux + 2997,
};

static const struct itemplate * const itable_vex021B8[] = {
    instrux + 2918,
    instrux + 2919,
    instrux + 2920,
    instrux + 2921,
    instrux + 2922,
    instrux + 2923,
    instrux + 2924,
    instrux + 2925,
};

static const struct itemplate * const itable_vex021B9[] = {
    instrux + 3054,
    instrux + 3055,
    instrux + 3056,
    instrux + 3057,
};

static const struct itemplate * const itable_vex021BA[] = {
    instrux + 2966,
    instrux + 2967,
    instrux + 2968,
    instrux + 2969,
    instrux + 2970,
    instrux + 2971,
    instrux + 2972,
    instrux + 2973,
};

static const struct itemplate * const itable_vex021BB[] = {
    instrux + 3066,
    instrux + 3067,
    instrux + 3068,
    instrux + 3069,
};

static const struct itemplate * const itable_vex021BC[] = {
    instrux + 3014,
    instrux + 3015,
    instrux + 3016,
    instrux + 3017,
    instrux + 3018,
    instrux + 3019,
    instrux + 3020,
    instrux + 3021,
};

static const struct itemplate * const itable_vex021BD[] = {
    instrux + 3078,
    instrux + 3079,
    instrux + 3080,
    instrux + 3081,
};

static const struct itemplate * const itable_vex021BE[] = {
    instrux + 3038,
    instrux + 3039,
    instrux + 3040,
    instrux + 3041,
    instrux + 3042,
    instrux + 3043,
    instrux + 3044,
    instrux + 3045,
};

static const struct itemplate * const itable_vex021BF[] = {
    instrux + 3090,
    instrux + 3091,
    instrux + 3092,
    instrux + 3093,
};

static const struct itemplate * const itable_vex021DB[] = {
    instrux + 1706,
};

static const struct itemplate * const itable_vex021DC[] = {
    instrux + 1698,
    instrux + 1699,
};

static const struct itemplate * const itable_vex021DD[] = {
    instrux + 1700,
    instrux + 1701,
};

static const struct itemplate * const itable_vex021DE[] = {
    instrux + 1702,
    instrux + 1703,
};

static const struct itemplate * const itable_vex021DF[] = {
    instrux + 1704,
    instrux + 1705,
};

static const struct itemplate * const itable_vex03104[] = {
    instrux + 2620,
    instrux + 2621,
};

static const struct itemplate * const itable_vex03105[] = {
    instrux + 2616,
    instrux + 2617,
};

static const struct itemplate * const itable_vex03106[] = {
    instrux + 2622,
};

static const struct itemplate * const itable_vex03108[] = {
    instrux + 2820,
    instrux + 2821,
};

static const struct itemplate * const itable_vex03109[] = {
    instrux + 2818,
    instrux + 2819,
};

static const struct itemplate * const itable_vex0310A[] = {
    instrux + 2824,
    instrux + 2825,
};

static const struct itemplate * const itable_vex0310B[] = {
    instrux + 2822,
    instrux + 2823,
};

static const struct itemplate * const itable_vex0310C[] = {
    instrux + 1748,
    instrux + 1749,
    instrux + 1750,
    instrux + 1751,
};

static const struct itemplate * const itable_vex0310D[] = {
    instrux + 1744,
    instrux + 1745,
    instrux + 1746,
    instrux + 1747,
};

static const struct itemplate * const itable_vex0310E[] = {
    instrux + 2592,
    instrux + 2593,
};

static const struct itemplate * const itable_vex0310F[] = {
    instrux + 2580,
    instrux + 2581,
};

static const struct itemplate * const itable_vex03114[] = {
    instrux + 2623,
    instrux + 2624,
    instrux + 2625,
};

static const struct itemplate * const itable_vex03115[] = {
    instrux + 2628,
    instrux + 2629,
    instrux + 2630,
};

static const struct itemplate * const itable_vex03116[] = {
    instrux + 2631,
    instrux + 2632,
    instrux + 2633,
};

static const struct itemplate * const itable_vex03117[] = {
    instrux + 2391,
};

static const struct itemplate * const itable_vex03118[] = {
    instrux + 2408,
};

static const struct itemplate * const itable_vex03119[] = {
    instrux + 2390,
};

static const struct itemplate * const itable_vex0311D[] = {
    instrux + 3107,
    instrux + 3108,
};

static const struct itemplate * const itable_vex03120[] = {
    instrux + 2647,
    instrux + 2648,
    instrux + 2649,
    instrux + 2650,
    instrux + 2651,
    instrux + 2652,
};

static const struct itemplate * const itable_vex03121[] = {
    instrux + 2409,
    instrux + 2410,
};

static const struct itemplate * const itable_vex03122[] = {
    instrux + 2659,
    instrux + 2660,
    instrux + 2661,
    instrux + 2662,
    instrux + 2663,
    instrux + 2664,
    instrux + 2665,
    instrux + 2666,
};

static const struct itemplate * const itable_vex03140[] = {
    instrux + 2386,
    instrux + 2387,
    instrux + 2388,
    instrux + 2389,
};

static const struct itemplate * const itable_vex03141[] = {
    instrux + 2384,
    instrux + 2385,
};

static const struct itemplate * const itable_vex03142[] = {
    instrux + 2531,
    instrux + 2532,
};

static const struct itemplate * const itable_vex03144[] = {
    instrux + 2892,
    instrux + 2893,
    instrux + 2894,
    instrux + 2895,
    instrux + 2896,
    instrux + 2897,
    instrux + 2898,
    instrux + 2899,
    instrux + 2900,
    instrux + 2901,
};

static const struct itemplate * const itable_vex0314A[] = {
    instrux + 1756,
    instrux + 1757,
    instrux + 1758,
    instrux + 1759,
};

static const struct itemplate * const itable_vex0314B[] = {
    instrux + 1752,
    instrux + 1753,
    instrux + 1754,
    instrux + 1755,
};

static const struct itemplate * const itable_vex0314C[] = {
    instrux + 2590,
    instrux + 2591,
};

static const struct itemplate * const itable_vex0315C[] = {
    instrux + 3158,
    instrux + 3159,
    instrux + 3160,
    instrux + 3161,
    instrux + 3162,
    instrux + 3163,
    instrux + 3164,
    instrux + 3165,
};

static const struct itemplate * const itable_vex0315D[] = {
    instrux + 3150,
    instrux + 3151,
    instrux + 3152,
    instrux + 3153,
    instrux + 3154,
    instrux + 3155,
    instrux + 3156,
    instrux + 3157,
};

static const struct itemplate * const itable_vex0315E[] = {
    instrux + 3174,
    instrux + 3175,
    instrux + 3176,
    instrux + 3177,
    instrux + 3178,
    instrux + 3179,
    instrux + 3180,
    instrux + 3181,
};

static const struct itemplate * const itable_vex0315F[] = {
    instrux + 3166,
    instrux + 3167,
    instrux + 3168,
    instrux + 3169,
    instrux + 3170,
    instrux + 3171,
    instrux + 3172,
    instrux + 3173,
};

static const struct itemplate * const itable_vex03160[] = {
    instrux + 2595,
};

static const struct itemplate * const itable_vex03161[] = {
    instrux + 2594,
};

static const struct itemplate * const itable_vex03162[] = {
    instrux + 2597,
};

static const struct itemplate * const itable_vex03163[] = {
    instrux + 2596,
};

static const struct itemplate * const itable_vex03168[] = {
    instrux + 3134,
    instrux + 3135,
    instrux + 3136,
    instrux + 3137,
    instrux + 3138,
    instrux + 3139,
    instrux + 3140,
    instrux + 3141,
};

static const struct itemplate * const itable_vex03169[] = {
    instrux + 3126,
    instrux + 3127,
    instrux + 3128,
    instrux + 3129,
    instrux + 3130,
    instrux + 3131,
    instrux + 3132,
    instrux + 3133,
};

static const struct itemplate * const itable_vex0316A[] = {
    instrux + 3146,
    instrux + 3147,
    instrux + 3148,
    instrux + 3149,
};

static const struct itemplate * const itable_vex0316B[] = {
    instrux + 3142,
    instrux + 3143,
    instrux + 3144,
    instrux + 3145,
};

static const struct itemplate * const itable_vex0316C[] = {
    instrux + 3190,
    instrux + 3191,
    instrux + 3192,
    instrux + 3193,
    instrux + 3194,
    instrux + 3195,
    instrux + 3196,
    instrux + 3197,
};

static const struct itemplate * const itable_vex0316D[] = {
    instrux + 3182,
    instrux + 3183,
    instrux + 3184,
    instrux + 3185,
    instrux + 3186,
    instrux + 3187,
    instrux + 3188,
    instrux + 3189,
};

static const struct itemplate * const itable_vex0316E[] = {
    instrux + 3202,
    instrux + 3203,
    instrux + 3204,
    instrux + 3205,
};

static const struct itemplate * const itable_vex0316F[] = {
    instrux + 3198,
    instrux + 3199,
    instrux + 3200,
    instrux + 3201,
};

static const struct itemplate * const itable_vex03178[] = {
    instrux + 3214,
    instrux + 3215,
    instrux + 3216,
    instrux + 3217,
    instrux + 3218,
    instrux + 3219,
    instrux + 3220,
    instrux + 3221,
};

static const struct itemplate * const itable_vex03179[] = {
    instrux + 3206,
    instrux + 3207,
    instrux + 3208,
    instrux + 3209,
    instrux + 3210,
    instrux + 3211,
    instrux + 3212,
    instrux + 3213,
};

static const struct itemplate * const itable_vex0317A[] = {
    instrux + 3226,
    instrux + 3227,
    instrux + 3228,
    instrux + 3229,
};

static const struct itemplate * const itable_vex0317B[] = {
    instrux + 3222,
    instrux + 3223,
    instrux + 3224,
    instrux + 3225,
};

static const struct itemplate * const itable_vex0317C[] = {
    instrux + 3238,
    instrux + 3239,
    instrux + 3240,
    instrux + 3241,
    instrux + 3242,
    instrux + 3243,
    instrux + 3244,
    instrux + 3245,
};

static const struct itemplate * const itable_vex0317D[] = {
    instrux + 3230,
    instrux + 3231,
    instrux + 3232,
    instrux + 3233,
    instrux + 3234,
    instrux + 3235,
    instrux + 3236,
    instrux + 3237,
};

static const struct itemplate * const itable_vex0317E[] = {
    instrux + 3250,
    instrux + 3251,
    instrux + 3252,
    instrux + 3253,
};

static const struct itemplate * const itable_vex0317F[] = {
    instrux + 3246,
    instrux + 3247,
    instrux + 3248,
    instrux + 3249,
};

static const struct itemplate * const itable_vex031DF[] = {
    instrux + 1707,
};

static const struct itemplate * const itable_xop08085[] = {
    instrux + 3334,
    instrux + 3335,
};

static const struct itemplate * const itable_xop08086[] = {
    instrux + 3332,
    instrux + 3333,
};

static const struct itemplate * const itable_xop08087[] = {
    instrux + 3330,
    instrux + 3331,
};

static const struct itemplate * const itable_xop0808E[] = {
    instrux + 3326,
    instrux + 3327,
};

static const struct itemplate * const itable_xop0808F[] = {
    instrux + 3328,
    instrux + 3329,
};

static const struct itemplate * const itable_xop08095[] = {
    instrux + 3338,
    instrux + 3339,
};

static const struct itemplate * const itable_xop08096[] = {
    instrux + 3336,
    instrux + 3337,
};

static const struct itemplate * const itable_xop08097[] = {
    instrux + 3324,
    instrux + 3325,
};

static const struct itemplate * const itable_xop0809E[] = {
    instrux + 3320,
    instrux + 3321,
};

static const struct itemplate * const itable_xop0809F[] = {
    instrux + 3322,
    instrux + 3323,
};

static const struct itemplate * const itable_xop080A2[] = {
    instrux + 3266,
    instrux + 3267,
    instrux + 3268,
    instrux + 3269,
    instrux + 3270,
    instrux + 3271,
    instrux + 3272,
    instrux + 3273,
};

static const struct itemplate * const itable_xop080A3[] = {
    instrux + 3344,
    instrux + 3345,
    instrux + 3346,
    instrux + 3347,
};

static const struct itemplate * const itable_xop080A6[] = {
    instrux + 3340,
    instrux + 3341,
};

static const struct itemplate * const itable_xop080B6[] = {
    instrux + 3342,
    instrux + 3343,
};

static const struct itemplate * const itable_xop080C0[] = {
    instrux + 3352,
    instrux + 3353,
};

static const struct itemplate * const itable_xop080C1[] = {
    instrux + 3370,
    instrux + 3371,
};

static const struct itemplate * const itable_xop080C2[] = {
    instrux + 3358,
    instrux + 3359,
};

static const struct itemplate * const itable_xop080C3[] = {
    instrux + 3364,
    instrux + 3365,
};

static const struct itemplate * const itable_xop080CC[] = {
    instrux + 3274,
    instrux + 3275,
};

static const struct itemplate * const itable_xop080CD[] = {
    instrux + 3288,
    instrux + 3289,
};

static const struct itemplate * const itable_xop080CE[] = {
    instrux + 3276,
    instrux + 3277,
};

static const struct itemplate * const itable_xop080CF[] = {
    instrux + 3278,
    instrux + 3279,
};

static const struct itemplate * const itable_xop080EC[] = {
    instrux + 3280,
    instrux + 3281,
};

static const struct itemplate * const itable_xop080ED[] = {
    instrux + 3286,
    instrux + 3287,
};

static const struct itemplate * const itable_xop080EE[] = {
    instrux + 3282,
    instrux + 3283,
};

static const struct itemplate * const itable_xop080EF[] = {
    instrux + 3284,
    instrux + 3285,
};

static const struct itemplate * const itable_xop09012[] = {
    instrux + 3118,
    instrux + 3119,
    instrux + 3120,
    instrux + 3121,
};

static const struct itemplate * const itable_xop09080[] = {
    instrux + 3258,
    instrux + 3259,
    instrux + 3260,
    instrux + 3261,
};

static const struct itemplate * const itable_xop09081[] = {
    instrux + 3254,
    instrux + 3255,
    instrux + 3256,
    instrux + 3257,
};

static const struct itemplate * const itable_xop09082[] = {
    instrux + 3264,
    instrux + 3265,
};

static const struct itemplate * const itable_xop09083[] = {
    instrux + 3262,
    instrux + 3263,
};

static const struct itemplate * const itable_xop09090[] = {
    instrux + 3348,
    instrux + 3349,
    instrux + 3350,
    instrux + 3351,
};

static const struct itemplate * const itable_xop09091[] = {
    instrux + 3366,
    instrux + 3367,
    instrux + 3368,
    instrux + 3369,
};

static const struct itemplate * const itable_xop09092[] = {
    instrux + 3354,
    instrux + 3355,
    instrux + 3356,
    instrux + 3357,
};

static const struct itemplate * const itable_xop09093[] = {
    instrux + 3360,
    instrux + 3361,
    instrux + 3362,
    instrux + 3363,
};

static const struct itemplate * const itable_xop09094[] = {
    instrux + 3388,
    instrux + 3389,
    instrux + 3390,
    instrux + 3391,
};

static const struct itemplate * const itable_xop09095[] = {
    instrux + 3400,
    instrux + 3401,
    instrux + 3402,
    instrux + 3403,
};

static const struct itemplate * const itable_xop09096[] = {
    instrux + 3392,
    instrux + 3393,
    instrux + 3394,
    instrux + 3395,
};

static const struct itemplate * const itable_xop09097[] = {
    instrux + 3396,
    instrux + 3397,
    instrux + 3398,
    instrux + 3399,
};

static const struct itemplate * const itable_xop09098[] = {
    instrux + 3372,
    instrux + 3373,
    instrux + 3374,
    instrux + 3375,
};

static const struct itemplate * const itable_xop09099[] = {
    instrux + 3384,
    instrux + 3385,
    instrux + 3386,
    instrux + 3387,
};

static const struct itemplate * const itable_xop0909A[] = {
    instrux + 3376,
    instrux + 3377,
    instrux + 3378,
    instrux + 3379,
};

static const struct itemplate * const itable_xop0909B[] = {
    instrux + 3380,
    instrux + 3381,
    instrux + 3382,
    instrux + 3383,
};

static const struct itemplate * const itable_xop090C1[] = {
    instrux + 3294,
    instrux + 3295,
};

static const struct itemplate * const itable_xop090C2[] = {
    instrux + 3290,
    instrux + 3291,
};

static const struct itemplate * const itable_xop090C3[] = {
    instrux + 3292,
    instrux + 3293,
};

static const struct itemplate * const itable_xop090C6[] = {
    instrux + 3310,
    instrux + 3311,
};

static const struct itemplate * const itable_xop090C7[] = {
    instrux + 3312,
    instrux + 3313,
};

static const struct itemplate * const itable_xop090CB[] = {
    instrux + 3296,
    instrux + 3297,
};

static const struct itemplate * const itable_xop090D1[] = {
    instrux + 3302,
    instrux + 3303,
};

static const struct itemplate * const itable_xop090D2[] = {
    instrux + 3298,
    instrux + 3299,
};

static const struct itemplate * const itable_xop090D3[] = {
    instrux + 3300,
    instrux + 3301,
};

static const struct itemplate * const itable_xop090D6[] = {
    instrux + 3306,
    instrux + 3307,
};

static const struct itemplate * const itable_xop090D7[] = {
    instrux + 3308,
    instrux + 3309,
};

static const struct itemplate * const itable_xop090DB[] = {
    instrux + 3304,
    instrux + 3305,
};

static const struct itemplate * const itable_xop090E1[] = {
    instrux + 3314,
    instrux + 3315,
};

static const struct itemplate * const itable_xop090E2[] = {
    instrux + 3318,
    instrux + 3319,
};

static const struct itemplate * const itable_xop090E3[] = {
    instrux + 3316,
    instrux + 3317,
};

static const struct itemplate * const itable_xop0A012[] = {
    instrux + 3122,
    instrux + 3123,
    instrux + 3124,
    instrux + 3125,
};

static const struct disasm_index itable_vex010[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { NULL, 0 },
    /* 0x06 */ { NULL, 0 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { NULL, 0 },
    /* 0x09 */ { NULL, 0 },
    /* 0x0a */ { NULL, 0 },
    /* 0x0b */ { NULL, 0 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { NULL, 0 },
    /* 0x0e */ { NULL, 0 },
    /* 0x0f */ { NULL, 0 },
    /* 0x10 */ { itable_vex01010, 2 },
    /* 0x11 */ { itable_vex01011, 2 },
    /* 0x12 */ { itable_vex01012, 4 },
    /* 0x13 */ { itable_vex01013, 1 },
    /* 0x14 */ { itable_vex01014, 4 },
    /* 0x15 */ { itable_vex01015, 4 },
    /* 0x16 */ { itable_vex01016, 4 },
    /* 0x17 */ { itable_vex01017, 1 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { NULL, 0 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { NULL, 0 },
    /* 0x21 */ { NULL, 0 },
    /* 0x22 */ { NULL, 0 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { itable_vex01028, 2 },
    /* 0x29 */ { itable_vex01029, 2 },
    /* 0x2a */ { NULL, 0 },
    /* 0x2b */ { itable_vex0102B, 2 },
    /* 0x2c */ { NULL, 0 },
    /* 0x2d */ { NULL, 0 },
    /* 0x2e */ { itable_vex0102E, 1 },
    /* 0x2f */ { itable_vex0102F, 1 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { NULL, 0 },
    /* 0x41 */ { NULL, 0 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { itable_vex01050, 4 },
    /* 0x51 */ { itable_vex01051, 2 },
    /* 0x52 */ { itable_vex01052, 2 },
    /* 0x53 */ { itable_vex01053, 2 },
    /* 0x54 */ { itable_vex01054, 4 },
    /* 0x55 */ { itable_vex01055, 4 },
    /* 0x56 */ { itable_vex01056, 4 },
    /* 0x57 */ { itable_vex01057, 4 },
    /* 0x58 */ { itable_vex01058, 4 },
    /* 0x59 */ { itable_vex01059, 4 },
    /* 0x5a */ { itable_vex0105A, 2 },
    /* 0x5b */ { itable_vex0105B, 2 },
    /* 0x5c */ { itable_vex0105C, 4 },
    /* 0x5d */ { itable_vex0105D, 4 },
    /* 0x5e */ { itable_vex0105E, 4 },
    /* 0x5f */ { itable_vex0105F, 4 },
    /* 0x60 */ { NULL, 0 },
    /* 0x61 */ { NULL, 0 },
    /* 0x62 */ { NULL, 0 },
    /* 0x63 */ { NULL, 0 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { NULL, 0 },
    /* 0x70 */ { NULL, 0 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { itable_vex01077, 2 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { NULL, 0 },
    /* 0x7d */ { NULL, 0 },
    /* 0x7e */ { NULL, 0 },
    /* 0x7f */ { NULL, 0 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { NULL, 0 },
    /* 0x97 */ { NULL, 0 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { itable_vex010AE, 2 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { NULL, 0 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { itable_vex010C2, 188 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { itable_vex010C6, 4 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { NULL, 0 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { NULL, 0 },
    /* 0xdc */ { NULL, 0 },
    /* 0xdd */ { NULL, 0 },
    /* 0xde */ { NULL, 0 },
    /* 0xdf */ { NULL, 0 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { NULL, 0 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_vex011[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { NULL, 0 },
    /* 0x06 */ { NULL, 0 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { NULL, 0 },
    /* 0x09 */ { NULL, 0 },
    /* 0x0a */ { NULL, 0 },
    /* 0x0b */ { NULL, 0 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { NULL, 0 },
    /* 0x0e */ { NULL, 0 },
    /* 0x0f */ { NULL, 0 },
    /* 0x10 */ { itable_vex01110, 2 },
    /* 0x11 */ { itable_vex01111, 2 },
    /* 0x12 */ { itable_vex01112, 2 },
    /* 0x13 */ { itable_vex01113, 1 },
    /* 0x14 */ { itable_vex01114, 4 },
    /* 0x15 */ { itable_vex01115, 4 },
    /* 0x16 */ { itable_vex01116, 2 },
    /* 0x17 */ { itable_vex01117, 1 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { NULL, 0 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { NULL, 0 },
    /* 0x21 */ { NULL, 0 },
    /* 0x22 */ { NULL, 0 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { itable_vex01128, 2 },
    /* 0x29 */ { itable_vex01129, 4 },
    /* 0x2a */ { NULL, 0 },
    /* 0x2b */ { itable_vex0112B, 2 },
    /* 0x2c */ { NULL, 0 },
    /* 0x2d */ { NULL, 0 },
    /* 0x2e */ { itable_vex0112E, 1 },
    /* 0x2f */ { itable_vex0112F, 1 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { itable_vex01137, 2 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { NULL, 0 },
    /* 0x41 */ { NULL, 0 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { itable_vex01150, 4 },
    /* 0x51 */ { itable_vex01151, 2 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { itable_vex01154, 4 },
    /* 0x55 */ { itable_vex01155, 4 },
    /* 0x56 */ { itable_vex01156, 4 },
    /* 0x57 */ { itable_vex01157, 4 },
    /* 0x58 */ { itable_vex01158, 4 },
    /* 0x59 */ { itable_vex01159, 4 },
    /* 0x5a */ { itable_vex0115A, 4 },
    /* 0x5b */ { itable_vex0115B, 2 },
    /* 0x5c */ { itable_vex0115C, 4 },
    /* 0x5d */ { itable_vex0115D, 4 },
    /* 0x5e */ { itable_vex0115E, 4 },
    /* 0x5f */ { itable_vex0115F, 4 },
    /* 0x60 */ { itable_vex01160, 2 },
    /* 0x61 */ { itable_vex01161, 2 },
    /* 0x62 */ { itable_vex01162, 2 },
    /* 0x63 */ { itable_vex01163, 2 },
    /* 0x64 */ { itable_vex01164, 2 },
    /* 0x65 */ { itable_vex01165, 2 },
    /* 0x66 */ { itable_vex01166, 2 },
    /* 0x67 */ { itable_vex01167, 2 },
    /* 0x68 */ { itable_vex01168, 2 },
    /* 0x69 */ { itable_vex01169, 2 },
    /* 0x6a */ { itable_vex0116A, 2 },
    /* 0x6b */ { itable_vex0116B, 2 },
    /* 0x6c */ { itable_vex0116C, 2 },
    /* 0x6d */ { itable_vex0116D, 2 },
    /* 0x6e */ { itable_vex0116E, 2 },
    /* 0x6f */ { itable_vex0116F, 3 },
    /* 0x70 */ { itable_vex01170, 1 },
    /* 0x71 */ { itable_vex01171, 6 },
    /* 0x72 */ { itable_vex01172, 6 },
    /* 0x73 */ { itable_vex01173, 8 },
    /* 0x74 */ { itable_vex01174, 2 },
    /* 0x75 */ { itable_vex01175, 2 },
    /* 0x76 */ { itable_vex01176, 2 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { itable_vex0117C, 4 },
    /* 0x7d */ { itable_vex0117D, 4 },
    /* 0x7e */ { itable_vex0117E, 2 },
    /* 0x7f */ { itable_vex0117F, 3 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { NULL, 0 },
    /* 0x97 */ { NULL, 0 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { NULL, 0 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { itable_vex011C2, 188 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { itable_vex011C4, 6 },
    /* 0xc5 */ { itable_vex011C5, 2 },
    /* 0xc6 */ { itable_vex011C6, 4 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { itable_vex011D0, 4 },
    /* 0xd1 */ { itable_vex011D1, 2 },
    /* 0xd2 */ { itable_vex011D2, 2 },
    /* 0xd3 */ { itable_vex011D3, 2 },
    /* 0xd4 */ { itable_vex011D4, 2 },
    /* 0xd5 */ { itable_vex011D5, 2 },
    /* 0xd6 */ { itable_vex011D6, 1 },
    /* 0xd7 */ { itable_vex011D7, 2 },
    /* 0xd8 */ { itable_vex011D8, 2 },
    /* 0xd9 */ { itable_vex011D9, 2 },
    /* 0xda */ { itable_vex011DA, 2 },
    /* 0xdb */ { itable_vex011DB, 2 },
    /* 0xdc */ { itable_vex011DC, 2 },
    /* 0xdd */ { itable_vex011DD, 2 },
    /* 0xde */ { itable_vex011DE, 2 },
    /* 0xdf */ { itable_vex011DF, 2 },
    /* 0xe0 */ { itable_vex011E0, 2 },
    /* 0xe1 */ { itable_vex011E1, 2 },
    /* 0xe2 */ { itable_vex011E2, 2 },
    /* 0xe3 */ { itable_vex011E3, 2 },
    /* 0xe4 */ { itable_vex011E4, 2 },
    /* 0xe5 */ { itable_vex011E5, 2 },
    /* 0xe6 */ { itable_vex011E6, 4 },
    /* 0xe7 */ { itable_vex011E7, 3 },
    /* 0xe8 */ { itable_vex011E8, 2 },
    /* 0xe9 */ { itable_vex011E9, 2 },
    /* 0xea */ { itable_vex011EA, 2 },
    /* 0xeb */ { itable_vex011EB, 2 },
    /* 0xec */ { itable_vex011EC, 2 },
    /* 0xed */ { itable_vex011ED, 2 },
    /* 0xee */ { itable_vex011EE, 2 },
    /* 0xef */ { itable_vex011EF, 2 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { itable_vex011F1, 2 },
    /* 0xf2 */ { itable_vex011F2, 2 },
    /* 0xf3 */ { itable_vex011F3, 2 },
    /* 0xf4 */ { itable_vex011F4, 2 },
    /* 0xf5 */ { itable_vex011F5, 2 },
    /* 0xf6 */ { itable_vex011F6, 2 },
    /* 0xf7 */ { itable_vex011F7, 1 },
    /* 0xf8 */ { itable_vex011F8, 2 },
    /* 0xf9 */ { itable_vex011F9, 2 },
    /* 0xfa */ { itable_vex011FA, 2 },
    /* 0xfb */ { itable_vex011FB, 2 },
    /* 0xfc */ { itable_vex011FC, 2 },
    /* 0xfd */ { itable_vex011FD, 2 },
    /* 0xfe */ { itable_vex011FE, 2 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_vex012[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { NULL, 0 },
    /* 0x06 */ { NULL, 0 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { NULL, 0 },
    /* 0x09 */ { NULL, 0 },
    /* 0x0a */ { NULL, 0 },
    /* 0x0b */ { NULL, 0 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { NULL, 0 },
    /* 0x0e */ { NULL, 0 },
    /* 0x0f */ { NULL, 0 },
    /* 0x10 */ { itable_vex01210, 3 },
    /* 0x11 */ { itable_vex01211, 3 },
    /* 0x12 */ { itable_vex01212, 2 },
    /* 0x13 */ { NULL, 0 },
    /* 0x14 */ { NULL, 0 },
    /* 0x15 */ { NULL, 0 },
    /* 0x16 */ { itable_vex01216, 2 },
    /* 0x17 */ { NULL, 0 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { NULL, 0 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { NULL, 0 },
    /* 0x21 */ { NULL, 0 },
    /* 0x22 */ { NULL, 0 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { NULL, 0 },
    /* 0x29 */ { NULL, 0 },
    /* 0x2a */ { itable_vex0122A, 4 },
    /* 0x2b */ { NULL, 0 },
    /* 0x2c */ { itable_vex0122C, 2 },
    /* 0x2d */ { itable_vex0122D, 2 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { NULL, 0 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { NULL, 0 },
    /* 0x41 */ { NULL, 0 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { itable_vex01251, 2 },
    /* 0x52 */ { itable_vex01252, 2 },
    /* 0x53 */ { itable_vex01253, 2 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { itable_vex01258, 2 },
    /* 0x59 */ { itable_vex01259, 2 },
    /* 0x5a */ { itable_vex0125A, 2 },
    /* 0x5b */ { itable_vex0125B, 2 },
    /* 0x5c */ { itable_vex0125C, 2 },
    /* 0x5d */ { itable_vex0125D, 2 },
    /* 0x5e */ { itable_vex0125E, 2 },
    /* 0x5f */ { itable_vex0125F, 2 },
    /* 0x60 */ { NULL, 0 },
    /* 0x61 */ { NULL, 0 },
    /* 0x62 */ { NULL, 0 },
    /* 0x63 */ { NULL, 0 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { itable_vex0126F, 3 },
    /* 0x70 */ { itable_vex01270, 1 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { NULL, 0 },
    /* 0x7d */ { NULL, 0 },
    /* 0x7e */ { itable_vex0127E, 1 },
    /* 0x7f */ { itable_vex0127F, 3 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { NULL, 0 },
    /* 0x97 */ { NULL, 0 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { NULL, 0 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { itable_vex012C2, 94 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { NULL, 0 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { NULL, 0 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { NULL, 0 },
    /* 0xdc */ { NULL, 0 },
    /* 0xdd */ { NULL, 0 },
    /* 0xde */ { NULL, 0 },
    /* 0xdf */ { NULL, 0 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { itable_vex012E6, 2 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_vex013[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { NULL, 0 },
    /* 0x06 */ { NULL, 0 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { NULL, 0 },
    /* 0x09 */ { NULL, 0 },
    /* 0x0a */ { NULL, 0 },
    /* 0x0b */ { NULL, 0 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { NULL, 0 },
    /* 0x0e */ { NULL, 0 },
    /* 0x0f */ { NULL, 0 },
    /* 0x10 */ { itable_vex01310, 3 },
    /* 0x11 */ { itable_vex01311, 3 },
    /* 0x12 */ { itable_vex01312, 2 },
    /* 0x13 */ { NULL, 0 },
    /* 0x14 */ { NULL, 0 },
    /* 0x15 */ { NULL, 0 },
    /* 0x16 */ { NULL, 0 },
    /* 0x17 */ { NULL, 0 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { NULL, 0 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { NULL, 0 },
    /* 0x21 */ { NULL, 0 },
    /* 0x22 */ { NULL, 0 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { NULL, 0 },
    /* 0x29 */ { NULL, 0 },
    /* 0x2a */ { itable_vex0132A, 4 },
    /* 0x2b */ { NULL, 0 },
    /* 0x2c */ { itable_vex0132C, 2 },
    /* 0x2d */ { itable_vex0132D, 2 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { NULL, 0 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { NULL, 0 },
    /* 0x41 */ { NULL, 0 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { itable_vex01351, 2 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { itable_vex01358, 2 },
    /* 0x59 */ { itable_vex01359, 2 },
    /* 0x5a */ { itable_vex0135A, 2 },
    /* 0x5b */ { NULL, 0 },
    /* 0x5c */ { itable_vex0135C, 2 },
    /* 0x5d */ { itable_vex0135D, 2 },
    /* 0x5e */ { itable_vex0135E, 2 },
    /* 0x5f */ { itable_vex0135F, 2 },
    /* 0x60 */ { NULL, 0 },
    /* 0x61 */ { NULL, 0 },
    /* 0x62 */ { NULL, 0 },
    /* 0x63 */ { NULL, 0 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { NULL, 0 },
    /* 0x70 */ { itable_vex01370, 1 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { itable_vex0137C, 4 },
    /* 0x7d */ { itable_vex0137D, 4 },
    /* 0x7e */ { NULL, 0 },
    /* 0x7f */ { NULL, 0 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { NULL, 0 },
    /* 0x97 */ { NULL, 0 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { NULL, 0 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { itable_vex013C2, 94 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { NULL, 0 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { itable_vex013D0, 4 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { NULL, 0 },
    /* 0xdc */ { NULL, 0 },
    /* 0xdd */ { NULL, 0 },
    /* 0xde */ { NULL, 0 },
    /* 0xdf */ { NULL, 0 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { itable_vex013E6, 4 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { itable_vex013F0, 2 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_vex021[256] = {
    /* 0x00 */ { itable_vex02100, 2 },
    /* 0x01 */ { itable_vex02101, 2 },
    /* 0x02 */ { itable_vex02102, 2 },
    /* 0x03 */ { itable_vex02103, 2 },
    /* 0x04 */ { itable_vex02104, 2 },
    /* 0x05 */ { itable_vex02105, 2 },
    /* 0x06 */ { itable_vex02106, 2 },
    /* 0x07 */ { itable_vex02107, 2 },
    /* 0x08 */ { itable_vex02108, 2 },
    /* 0x09 */ { itable_vex02109, 2 },
    /* 0x0a */ { itable_vex0210A, 2 },
    /* 0x0b */ { itable_vex0210B, 2 },
    /* 0x0c */ { itable_vex0210C, 2 },
    /* 0x0d */ { itable_vex0210D, 2 },
    /* 0x0e */ { itable_vex0210E, 2 },
    /* 0x0f */ { itable_vex0210F, 2 },
    /* 0x10 */ { NULL, 0 },
    /* 0x11 */ { NULL, 0 },
    /* 0x12 */ { NULL, 0 },
    /* 0x13 */ { itable_vex02113, 2 },
    /* 0x14 */ { NULL, 0 },
    /* 0x15 */ { NULL, 0 },
    /* 0x16 */ { NULL, 0 },
    /* 0x17 */ { itable_vex02117, 2 },
    /* 0x18 */ { itable_vex02118, 2 },
    /* 0x19 */ { itable_vex02119, 1 },
    /* 0x1a */ { itable_vex0211A, 1 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { itable_vex0211C, 1 },
    /* 0x1d */ { itable_vex0211D, 1 },
    /* 0x1e */ { itable_vex0211E, 1 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { itable_vex02120, 1 },
    /* 0x21 */ { itable_vex02121, 1 },
    /* 0x22 */ { itable_vex02122, 1 },
    /* 0x23 */ { itable_vex02123, 1 },
    /* 0x24 */ { itable_vex02124, 1 },
    /* 0x25 */ { itable_vex02125, 1 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { itable_vex02128, 2 },
    /* 0x29 */ { NULL, 0 },
    /* 0x2a */ { itable_vex0212A, 1 },
    /* 0x2b */ { itable_vex0212B, 2 },
    /* 0x2c */ { itable_vex0212C, 2 },
    /* 0x2d */ { itable_vex0212D, 2 },
    /* 0x2e */ { itable_vex0212E, 2 },
    /* 0x2f */ { itable_vex0212F, 2 },
    /* 0x30 */ { itable_vex02130, 1 },
    /* 0x31 */ { itable_vex02131, 1 },
    /* 0x32 */ { itable_vex02132, 1 },
    /* 0x33 */ { itable_vex02133, 1 },
    /* 0x34 */ { itable_vex02134, 1 },
    /* 0x35 */ { itable_vex02135, 1 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { itable_vex02138, 2 },
    /* 0x39 */ { itable_vex02139, 2 },
    /* 0x3a */ { itable_vex0213A, 2 },
    /* 0x3b */ { itable_vex0213B, 2 },
    /* 0x3c */ { itable_vex0213C, 2 },
    /* 0x3d */ { itable_vex0213D, 2 },
    /* 0x3e */ { itable_vex0213E, 2 },
    /* 0x3f */ { itable_vex0213F, 2 },
    /* 0x40 */ { itable_vex02140, 2 },
    /* 0x41 */ { itable_vex02141, 1 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { NULL, 0 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { NULL, 0 },
    /* 0x59 */ { NULL, 0 },
    /* 0x5a */ { NULL, 0 },
    /* 0x5b */ { NULL, 0 },
    /* 0x5c */ { NULL, 0 },
    /* 0x5d */ { NULL, 0 },
    /* 0x5e */ { NULL, 0 },
    /* 0x5f */ { NULL, 0 },
    /* 0x60 */ { NULL, 0 },
    /* 0x61 */ { NULL, 0 },
    /* 0x62 */ { NULL, 0 },
    /* 0x63 */ { NULL, 0 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { NULL, 0 },
    /* 0x70 */ { NULL, 0 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { NULL, 0 },
    /* 0x7d */ { NULL, 0 },
    /* 0x7e */ { NULL, 0 },
    /* 0x7f */ { NULL, 0 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { itable_vex02196, 8 },
    /* 0x97 */ { itable_vex02197, 8 },
    /* 0x98 */ { itable_vex02198, 8 },
    /* 0x99 */ { itable_vex02199, 4 },
    /* 0x9a */ { itable_vex0219A, 8 },
    /* 0x9b */ { itable_vex0219B, 4 },
    /* 0x9c */ { itable_vex0219C, 8 },
    /* 0x9d */ { itable_vex0219D, 4 },
    /* 0x9e */ { itable_vex0219E, 8 },
    /* 0x9f */ { itable_vex0219F, 4 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { itable_vex021A6, 8 },
    /* 0xa7 */ { itable_vex021A7, 8 },
    /* 0xa8 */ { itable_vex021A8, 8 },
    /* 0xa9 */ { itable_vex021A9, 4 },
    /* 0xaa */ { itable_vex021AA, 8 },
    /* 0xab */ { itable_vex021AB, 4 },
    /* 0xac */ { itable_vex021AC, 8 },
    /* 0xad */ { itable_vex021AD, 4 },
    /* 0xae */ { itable_vex021AE, 8 },
    /* 0xaf */ { itable_vex021AF, 4 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { itable_vex021B6, 8 },
    /* 0xb7 */ { itable_vex021B7, 8 },
    /* 0xb8 */ { itable_vex021B8, 8 },
    /* 0xb9 */ { itable_vex021B9, 4 },
    /* 0xba */ { itable_vex021BA, 8 },
    /* 0xbb */ { itable_vex021BB, 4 },
    /* 0xbc */ { itable_vex021BC, 8 },
    /* 0xbd */ { itable_vex021BD, 4 },
    /* 0xbe */ { itable_vex021BE, 8 },
    /* 0xbf */ { itable_vex021BF, 4 },
    /* 0xc0 */ { NULL, 0 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { NULL, 0 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { NULL, 0 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { NULL, 0 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { itable_vex021DB, 1 },
    /* 0xdc */ { itable_vex021DC, 2 },
    /* 0xdd */ { itable_vex021DD, 2 },
    /* 0xde */ { itable_vex021DE, 2 },
    /* 0xdf */ { itable_vex021DF, 2 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { NULL, 0 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_vex031[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { itable_vex03104, 2 },
    /* 0x05 */ { itable_vex03105, 2 },
    /* 0x06 */ { itable_vex03106, 1 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { itable_vex03108, 2 },
    /* 0x09 */ { itable_vex03109, 2 },
    /* 0x0a */ { itable_vex0310A, 2 },
    /* 0x0b */ { itable_vex0310B, 2 },
    /* 0x0c */ { itable_vex0310C, 4 },
    /* 0x0d */ { itable_vex0310D, 4 },
    /* 0x0e */ { itable_vex0310E, 2 },
    /* 0x0f */ { itable_vex0310F, 2 },
    /* 0x10 */ { NULL, 0 },
    /* 0x11 */ { NULL, 0 },
    /* 0x12 */ { NULL, 0 },
    /* 0x13 */ { NULL, 0 },
    /* 0x14 */ { itable_vex03114, 3 },
    /* 0x15 */ { itable_vex03115, 3 },
    /* 0x16 */ { itable_vex03116, 3 },
    /* 0x17 */ { itable_vex03117, 1 },
    /* 0x18 */ { itable_vex03118, 1 },
    /* 0x19 */ { itable_vex03119, 1 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { itable_vex0311D, 2 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { itable_vex03120, 6 },
    /* 0x21 */ { itable_vex03121, 2 },
    /* 0x22 */ { itable_vex03122, 8 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { NULL, 0 },
    /* 0x29 */ { NULL, 0 },
    /* 0x2a */ { NULL, 0 },
    /* 0x2b */ { NULL, 0 },
    /* 0x2c */ { NULL, 0 },
    /* 0x2d */ { NULL, 0 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { NULL, 0 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { itable_vex03140, 4 },
    /* 0x41 */ { itable_vex03141, 2 },
    /* 0x42 */ { itable_vex03142, 2 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { itable_vex03144, 10 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { itable_vex0314A, 4 },
    /* 0x4b */ { itable_vex0314B, 4 },
    /* 0x4c */ { itable_vex0314C, 2 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { NULL, 0 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { NULL, 0 },
    /* 0x59 */ { NULL, 0 },
    /* 0x5a */ { NULL, 0 },
    /* 0x5b */ { NULL, 0 },
    /* 0x5c */ { itable_vex0315C, 8 },
    /* 0x5d */ { itable_vex0315D, 8 },
    /* 0x5e */ { itable_vex0315E, 8 },
    /* 0x5f */ { itable_vex0315F, 8 },
    /* 0x60 */ { itable_vex03160, 1 },
    /* 0x61 */ { itable_vex03161, 1 },
    /* 0x62 */ { itable_vex03162, 1 },
    /* 0x63 */ { itable_vex03163, 1 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { itable_vex03168, 8 },
    /* 0x69 */ { itable_vex03169, 8 },
    /* 0x6a */ { itable_vex0316A, 4 },
    /* 0x6b */ { itable_vex0316B, 4 },
    /* 0x6c */ { itable_vex0316C, 8 },
    /* 0x6d */ { itable_vex0316D, 8 },
    /* 0x6e */ { itable_vex0316E, 4 },
    /* 0x6f */ { itable_vex0316F, 4 },
    /* 0x70 */ { NULL, 0 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { itable_vex03178, 8 },
    /* 0x79 */ { itable_vex03179, 8 },
    /* 0x7a */ { itable_vex0317A, 4 },
    /* 0x7b */ { itable_vex0317B, 4 },
    /* 0x7c */ { itable_vex0317C, 8 },
    /* 0x7d */ { itable_vex0317D, 8 },
    /* 0x7e */ { itable_vex0317E, 4 },
    /* 0x7f */ { itable_vex0317F, 4 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { NULL, 0 },
    /* 0x97 */ { NULL, 0 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { NULL, 0 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { NULL, 0 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { NULL, 0 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { NULL, 0 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { NULL, 0 },
    /* 0xdc */ { NULL, 0 },
    /* 0xdd */ { NULL, 0 },
    /* 0xde */ { NULL, 0 },
    /* 0xdf */ { itable_vex031DF, 1 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { NULL, 0 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_xop080[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { NULL, 0 },
    /* 0x06 */ { NULL, 0 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { NULL, 0 },
    /* 0x09 */ { NULL, 0 },
    /* 0x0a */ { NULL, 0 },
    /* 0x0b */ { NULL, 0 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { NULL, 0 },
    /* 0x0e */ { NULL, 0 },
    /* 0x0f */ { NULL, 0 },
    /* 0x10 */ { NULL, 0 },
    /* 0x11 */ { NULL, 0 },
    /* 0x12 */ { NULL, 0 },
    /* 0x13 */ { NULL, 0 },
    /* 0x14 */ { NULL, 0 },
    /* 0x15 */ { NULL, 0 },
    /* 0x16 */ { NULL, 0 },
    /* 0x17 */ { NULL, 0 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { NULL, 0 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { NULL, 0 },
    /* 0x21 */ { NULL, 0 },
    /* 0x22 */ { NULL, 0 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { NULL, 0 },
    /* 0x29 */ { NULL, 0 },
    /* 0x2a */ { NULL, 0 },
    /* 0x2b */ { NULL, 0 },
    /* 0x2c */ { NULL, 0 },
    /* 0x2d */ { NULL, 0 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { NULL, 0 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { NULL, 0 },
    /* 0x41 */ { NULL, 0 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { NULL, 0 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { NULL, 0 },
    /* 0x59 */ { NULL, 0 },
    /* 0x5a */ { NULL, 0 },
    /* 0x5b */ { NULL, 0 },
    /* 0x5c */ { NULL, 0 },
    /* 0x5d */ { NULL, 0 },
    /* 0x5e */ { NULL, 0 },
    /* 0x5f */ { NULL, 0 },
    /* 0x60 */ { NULL, 0 },
    /* 0x61 */ { NULL, 0 },
    /* 0x62 */ { NULL, 0 },
    /* 0x63 */ { NULL, 0 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { NULL, 0 },
    /* 0x70 */ { NULL, 0 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { NULL, 0 },
    /* 0x7d */ { NULL, 0 },
    /* 0x7e */ { NULL, 0 },
    /* 0x7f */ { NULL, 0 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { itable_xop08085, 2 },
    /* 0x86 */ { itable_xop08086, 2 },
    /* 0x87 */ { itable_xop08087, 2 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { itable_xop0808E, 2 },
    /* 0x8f */ { itable_xop0808F, 2 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { itable_xop08095, 2 },
    /* 0x96 */ { itable_xop08096, 2 },
    /* 0x97 */ { itable_xop08097, 2 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { itable_xop0809E, 2 },
    /* 0x9f */ { itable_xop0809F, 2 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { itable_xop080A2, 8 },
    /* 0xa3 */ { itable_xop080A3, 4 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { itable_xop080A6, 2 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { itable_xop080B6, 2 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { itable_xop080C0, 2 },
    /* 0xc1 */ { itable_xop080C1, 2 },
    /* 0xc2 */ { itable_xop080C2, 2 },
    /* 0xc3 */ { itable_xop080C3, 2 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { NULL, 0 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { itable_xop080CC, 2 },
    /* 0xcd */ { itable_xop080CD, 2 },
    /* 0xce */ { itable_xop080CE, 2 },
    /* 0xcf */ { itable_xop080CF, 2 },
    /* 0xd0 */ { NULL, 0 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { NULL, 0 },
    /* 0xdc */ { NULL, 0 },
    /* 0xdd */ { NULL, 0 },
    /* 0xde */ { NULL, 0 },
    /* 0xdf */ { NULL, 0 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { NULL, 0 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { itable_xop080EC, 2 },
    /* 0xed */ { itable_xop080ED, 2 },
    /* 0xee */ { itable_xop080EE, 2 },
    /* 0xef */ { itable_xop080EF, 2 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_xop090[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { NULL, 0 },
    /* 0x06 */ { NULL, 0 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { NULL, 0 },
    /* 0x09 */ { NULL, 0 },
    /* 0x0a */ { NULL, 0 },
    /* 0x0b */ { NULL, 0 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { NULL, 0 },
    /* 0x0e */ { NULL, 0 },
    /* 0x0f */ { NULL, 0 },
    /* 0x10 */ { NULL, 0 },
    /* 0x11 */ { NULL, 0 },
    /* 0x12 */ { itable_xop09012, 4 },
    /* 0x13 */ { NULL, 0 },
    /* 0x14 */ { NULL, 0 },
    /* 0x15 */ { NULL, 0 },
    /* 0x16 */ { NULL, 0 },
    /* 0x17 */ { NULL, 0 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { NULL, 0 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { NULL, 0 },
    /* 0x21 */ { NULL, 0 },
    /* 0x22 */ { NULL, 0 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { NULL, 0 },
    /* 0x29 */ { NULL, 0 },
    /* 0x2a */ { NULL, 0 },
    /* 0x2b */ { NULL, 0 },
    /* 0x2c */ { NULL, 0 },
    /* 0x2d */ { NULL, 0 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { NULL, 0 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { NULL, 0 },
    /* 0x41 */ { NULL, 0 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { NULL, 0 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { NULL, 0 },
    /* 0x59 */ { NULL, 0 },
    /* 0x5a */ { NULL, 0 },
    /* 0x5b */ { NULL, 0 },
    /* 0x5c */ { NULL, 0 },
    /* 0x5d */ { NULL, 0 },
    /* 0x5e */ { NULL, 0 },
    /* 0x5f */ { NULL, 0 },
    /* 0x60 */ { NULL, 0 },
    /* 0x61 */ { NULL, 0 },
    /* 0x62 */ { NULL, 0 },
    /* 0x63 */ { NULL, 0 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { NULL, 0 },
    /* 0x70 */ { NULL, 0 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { NULL, 0 },
    /* 0x7d */ { NULL, 0 },
    /* 0x7e */ { NULL, 0 },
    /* 0x7f */ { NULL, 0 },
    /* 0x80 */ { itable_xop09080, 4 },
    /* 0x81 */ { itable_xop09081, 4 },
    /* 0x82 */ { itable_xop09082, 2 },
    /* 0x83 */ { itable_xop09083, 2 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { itable_xop09090, 4 },
    /* 0x91 */ { itable_xop09091, 4 },
    /* 0x92 */ { itable_xop09092, 4 },
    /* 0x93 */ { itable_xop09093, 4 },
    /* 0x94 */ { itable_xop09094, 4 },
    /* 0x95 */ { itable_xop09095, 4 },
    /* 0x96 */ { itable_xop09096, 4 },
    /* 0x97 */ { itable_xop09097, 4 },
    /* 0x98 */ { itable_xop09098, 4 },
    /* 0x99 */ { itable_xop09099, 4 },
    /* 0x9a */ { itable_xop0909A, 4 },
    /* 0x9b */ { itable_xop0909B, 4 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { NULL, 0 },
    /* 0xc1 */ { itable_xop090C1, 2 },
    /* 0xc2 */ { itable_xop090C2, 2 },
    /* 0xc3 */ { itable_xop090C3, 2 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { itable_xop090C6, 2 },
    /* 0xc7 */ { itable_xop090C7, 2 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { itable_xop090CB, 2 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { NULL, 0 },
    /* 0xd1 */ { itable_xop090D1, 2 },
    /* 0xd2 */ { itable_xop090D2, 2 },
    /* 0xd3 */ { itable_xop090D3, 2 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { itable_xop090D6, 2 },
    /* 0xd7 */ { itable_xop090D7, 2 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { itable_xop090DB, 2 },
    /* 0xdc */ { NULL, 0 },
    /* 0xdd */ { NULL, 0 },
    /* 0xde */ { NULL, 0 },
    /* 0xdf */ { NULL, 0 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { itable_xop090E1, 2 },
    /* 0xe2 */ { itable_xop090E2, 2 },
    /* 0xe3 */ { itable_xop090E3, 2 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { NULL, 0 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_xop0A0[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { NULL, 0 },
    /* 0x06 */ { NULL, 0 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { NULL, 0 },
    /* 0x09 */ { NULL, 0 },
    /* 0x0a */ { NULL, 0 },
    /* 0x0b */ { NULL, 0 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { NULL, 0 },
    /* 0x0e */ { NULL, 0 },
    /* 0x0f */ { NULL, 0 },
    /* 0x10 */ { NULL, 0 },
    /* 0x11 */ { NULL, 0 },
    /* 0x12 */ { itable_xop0A012, 4 },
    /* 0x13 */ { NULL, 0 },
    /* 0x14 */ { NULL, 0 },
    /* 0x15 */ { NULL, 0 },
    /* 0x16 */ { NULL, 0 },
    /* 0x17 */ { NULL, 0 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { NULL, 0 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { NULL, 0 },
    /* 0x21 */ { NULL, 0 },
    /* 0x22 */ { NULL, 0 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { NULL, 0 },
    /* 0x29 */ { NULL, 0 },
    /* 0x2a */ { NULL, 0 },
    /* 0x2b */ { NULL, 0 },
    /* 0x2c */ { NULL, 0 },
    /* 0x2d */ { NULL, 0 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { NULL, 0 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { NULL, 0 },
    /* 0x41 */ { NULL, 0 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { NULL, 0 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { NULL, 0 },
    /* 0x59 */ { NULL, 0 },
    /* 0x5a */ { NULL, 0 },
    /* 0x5b */ { NULL, 0 },
    /* 0x5c */ { NULL, 0 },
    /* 0x5d */ { NULL, 0 },
    /* 0x5e */ { NULL, 0 },
    /* 0x5f */ { NULL, 0 },
    /* 0x60 */ { NULL, 0 },
    /* 0x61 */ { NULL, 0 },
    /* 0x62 */ { NULL, 0 },
    /* 0x63 */ { NULL, 0 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { NULL, 0 },
    /* 0x70 */ { NULL, 0 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { NULL, 0 },
    /* 0x7d */ { NULL, 0 },
    /* 0x7e */ { NULL, 0 },
    /* 0x7f */ { NULL, 0 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { NULL, 0 },
    /* 0x97 */ { NULL, 0 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { NULL, 0 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { NULL, 0 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { NULL, 0 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { NULL, 0 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { NULL, 0 },
    /* 0xdc */ { NULL, 0 },
    /* 0xdd */ { NULL, 0 },
    /* 0xde */ { NULL, 0 },
    /* 0xdf */ { NULL, 0 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { NULL, 0 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_0F38[256] = {
    /* 0x00 */ { itable_0F3800, 2 },
    /* 0x01 */ { itable_0F3801, 2 },
    /* 0x02 */ { itable_0F3802, 2 },
    /* 0x03 */ { itable_0F3803, 2 },
    /* 0x04 */ { itable_0F3804, 2 },
    /* 0x05 */ { itable_0F3805, 2 },
    /* 0x06 */ { itable_0F3806, 2 },
    /* 0x07 */ { itable_0F3807, 2 },
    /* 0x08 */ { itable_0F3808, 2 },
    /* 0x09 */ { itable_0F3809, 2 },
    /* 0x0a */ { itable_0F380A, 2 },
    /* 0x0b */ { itable_0F380B, 2 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { NULL, 0 },
    /* 0x0e */ { NULL, 0 },
    /* 0x0f */ { NULL, 0 },
    /* 0x10 */ { itable_0F3810, 1 },
    /* 0x11 */ { NULL, 0 },
    /* 0x12 */ { NULL, 0 },
    /* 0x13 */ { NULL, 0 },
    /* 0x14 */ { itable_0F3814, 1 },
    /* 0x15 */ { itable_0F3815, 1 },
    /* 0x16 */ { NULL, 0 },
    /* 0x17 */ { itable_0F3817, 1 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { itable_0F381C, 2 },
    /* 0x1d */ { itable_0F381D, 2 },
    /* 0x1e */ { itable_0F381E, 2 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { itable_0F3820, 1 },
    /* 0x21 */ { itable_0F3821, 1 },
    /* 0x22 */ { itable_0F3822, 1 },
    /* 0x23 */ { itable_0F3823, 1 },
    /* 0x24 */ { itable_0F3824, 1 },
    /* 0x25 */ { itable_0F3825, 1 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { itable_0F3828, 1 },
    /* 0x29 */ { itable_0F3829, 1 },
    /* 0x2a */ { itable_0F382A, 1 },
    /* 0x2b */ { itable_0F382B, 1 },
    /* 0x2c */ { NULL, 0 },
    /* 0x2d */ { NULL, 0 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { NULL, 0 },
    /* 0x30 */ { itable_0F3830, 1 },
    /* 0x31 */ { itable_0F3831, 1 },
    /* 0x32 */ { itable_0F3832, 1 },
    /* 0x33 */ { itable_0F3833, 1 },
    /* 0x34 */ { itable_0F3834, 1 },
    /* 0x35 */ { itable_0F3835, 1 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { itable_0F3837, 1 },
    /* 0x38 */ { itable_0F3838, 1 },
    /* 0x39 */ { itable_0F3839, 1 },
    /* 0x3a */ { itable_0F383A, 1 },
    /* 0x3b */ { itable_0F383B, 1 },
    /* 0x3c */ { itable_0F383C, 1 },
    /* 0x3d */ { itable_0F383D, 1 },
    /* 0x3e */ { itable_0F383E, 1 },
    /* 0x3f */ { itable_0F383F, 1 },
    /* 0x40 */ { itable_0F3840, 1 },
    /* 0x41 */ { itable_0F3841, 1 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { NULL, 0 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { NULL, 0 },
    /* 0x59 */ { NULL, 0 },
    /* 0x5a */ { NULL, 0 },
    /* 0x5b */ { NULL, 0 },
    /* 0x5c */ { NULL, 0 },
    /* 0x5d */ { NULL, 0 },
    /* 0x5e */ { NULL, 0 },
    /* 0x5f */ { NULL, 0 },
    /* 0x60 */ { NULL, 0 },
    /* 0x61 */ { NULL, 0 },
    /* 0x62 */ { NULL, 0 },
    /* 0x63 */ { NULL, 0 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { NULL, 0 },
    /* 0x70 */ { NULL, 0 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { NULL, 0 },
    /* 0x7d */ { NULL, 0 },
    /* 0x7e */ { NULL, 0 },
    /* 0x7f */ { NULL, 0 },
    /* 0x80 */ { itable_0F3880, 2 },
    /* 0x81 */ { itable_0F3881, 2 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { NULL, 0 },
    /* 0x97 */ { NULL, 0 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { NULL, 0 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { NULL, 0 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { NULL, 0 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { NULL, 0 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { itable_0F38DB, 1 },
    /* 0xdc */ { itable_0F38DC, 1 },
    /* 0xdd */ { itable_0F38DD, 1 },
    /* 0xde */ { itable_0F38DE, 1 },
    /* 0xdf */ { itable_0F38DF, 1 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { NULL, 0 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { itable_0F38F0, 5 },
    /* 0xf1 */ { itable_0F38F1, 6 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_0F3A[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { NULL, 0 },
    /* 0x06 */ { NULL, 0 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { itable_0F3A08, 1 },
    /* 0x09 */ { itable_0F3A09, 1 },
    /* 0x0a */ { itable_0F3A0A, 1 },
    /* 0x0b */ { itable_0F3A0B, 1 },
    /* 0x0c */ { itable_0F3A0C, 1 },
    /* 0x0d */ { itable_0F3A0D, 1 },
    /* 0x0e */ { itable_0F3A0E, 1 },
    /* 0x0f */ { itable_0F3A0F, 2 },
    /* 0x10 */ { NULL, 0 },
    /* 0x11 */ { NULL, 0 },
    /* 0x12 */ { NULL, 0 },
    /* 0x13 */ { NULL, 0 },
    /* 0x14 */ { itable_0F3A14, 3 },
    /* 0x15 */ { itable_0F3A15, 3 },
    /* 0x16 */ { itable_0F3A16, 2 },
    /* 0x17 */ { itable_0F3A17, 2 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { NULL, 0 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { itable_0F3A20, 3 },
    /* 0x21 */ { itable_0F3A21, 1 },
    /* 0x22 */ { itable_0F3A22, 4 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { NULL, 0 },
    /* 0x29 */ { NULL, 0 },
    /* 0x2a */ { NULL, 0 },
    /* 0x2b */ { NULL, 0 },
    /* 0x2c */ { NULL, 0 },
    /* 0x2d */ { NULL, 0 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { NULL, 0 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { itable_0F3A40, 1 },
    /* 0x41 */ { itable_0F3A41, 1 },
    /* 0x42 */ { itable_0F3A42, 1 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { itable_0F3A44, 5 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { NULL, 0 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { NULL, 0 },
    /* 0x59 */ { NULL, 0 },
    /* 0x5a */ { NULL, 0 },
    /* 0x5b */ { NULL, 0 },
    /* 0x5c */ { NULL, 0 },
    /* 0x5d */ { NULL, 0 },
    /* 0x5e */ { NULL, 0 },
    /* 0x5f */ { NULL, 0 },
    /* 0x60 */ { itable_0F3A60, 1 },
    /* 0x61 */ { itable_0F3A61, 1 },
    /* 0x62 */ { itable_0F3A62, 1 },
    /* 0x63 */ { itable_0F3A63, 1 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { NULL, 0 },
    /* 0x70 */ { NULL, 0 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { NULL, 0 },
    /* 0x7d */ { NULL, 0 },
    /* 0x7e */ { NULL, 0 },
    /* 0x7f */ { NULL, 0 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { NULL, 0 },
    /* 0x97 */ { NULL, 0 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { NULL, 0 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { NULL, 0 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { NULL, 0 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { NULL, 0 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { NULL, 0 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { NULL, 0 },
    /* 0xdc */ { NULL, 0 },
    /* 0xdd */ { NULL, 0 },
    /* 0xde */ { NULL, 0 },
    /* 0xdf */ { itable_0F3ADF, 1 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { NULL, 0 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_0FA6[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { NULL, 0 },
    /* 0x06 */ { NULL, 0 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { NULL, 0 },
    /* 0x09 */ { NULL, 0 },
    /* 0x0a */ { NULL, 0 },
    /* 0x0b */ { NULL, 0 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { NULL, 0 },
    /* 0x0e */ { NULL, 0 },
    /* 0x0f */ { NULL, 0 },
    /* 0x10 */ { NULL, 0 },
    /* 0x11 */ { NULL, 0 },
    /* 0x12 */ { NULL, 0 },
    /* 0x13 */ { NULL, 0 },
    /* 0x14 */ { NULL, 0 },
    /* 0x15 */ { NULL, 0 },
    /* 0x16 */ { NULL, 0 },
    /* 0x17 */ { NULL, 0 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { NULL, 0 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { NULL, 0 },
    /* 0x21 */ { NULL, 0 },
    /* 0x22 */ { NULL, 0 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { NULL, 0 },
    /* 0x29 */ { NULL, 0 },
    /* 0x2a */ { NULL, 0 },
    /* 0x2b */ { NULL, 0 },
    /* 0x2c */ { NULL, 0 },
    /* 0x2d */ { NULL, 0 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { NULL, 0 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { NULL, 0 },
    /* 0x41 */ { NULL, 0 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { NULL, 0 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { NULL, 0 },
    /* 0x59 */ { NULL, 0 },
    /* 0x5a */ { NULL, 0 },
    /* 0x5b */ { NULL, 0 },
    /* 0x5c */ { NULL, 0 },
    /* 0x5d */ { NULL, 0 },
    /* 0x5e */ { NULL, 0 },
    /* 0x5f */ { NULL, 0 },
    /* 0x60 */ { NULL, 0 },
    /* 0x61 */ { NULL, 0 },
    /* 0x62 */ { NULL, 0 },
    /* 0x63 */ { NULL, 0 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { NULL, 0 },
    /* 0x70 */ { NULL, 0 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { NULL, 0 },
    /* 0x7d */ { NULL, 0 },
    /* 0x7e */ { NULL, 0 },
    /* 0x7f */ { NULL, 0 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { NULL, 0 },
    /* 0x97 */ { NULL, 0 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { itable_0FA6C0, 1 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { NULL, 0 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { NULL, 0 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { itable_0FA6C8, 1 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { itable_0FA6D0, 1 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { NULL, 0 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { NULL, 0 },
    /* 0xdc */ { NULL, 0 },
    /* 0xdd */ { NULL, 0 },
    /* 0xde */ { NULL, 0 },
    /* 0xdf */ { NULL, 0 },
    /* 0xe0 */ { NULL, 0 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { NULL, 0 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { NULL, 0 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_0FA7[256] = {
    /* 0x00 */ { NULL, 0 },
    /* 0x01 */ { NULL, 0 },
    /* 0x02 */ { NULL, 0 },
    /* 0x03 */ { NULL, 0 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { NULL, 0 },
    /* 0x06 */ { NULL, 0 },
    /* 0x07 */ { NULL, 0 },
    /* 0x08 */ { NULL, 0 },
    /* 0x09 */ { NULL, 0 },
    /* 0x0a */ { NULL, 0 },
    /* 0x0b */ { NULL, 0 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { NULL, 0 },
    /* 0x0e */ { NULL, 0 },
    /* 0x0f */ { NULL, 0 },
    /* 0x10 */ { NULL, 0 },
    /* 0x11 */ { NULL, 0 },
    /* 0x12 */ { NULL, 0 },
    /* 0x13 */ { NULL, 0 },
    /* 0x14 */ { NULL, 0 },
    /* 0x15 */ { NULL, 0 },
    /* 0x16 */ { NULL, 0 },
    /* 0x17 */ { NULL, 0 },
    /* 0x18 */ { NULL, 0 },
    /* 0x19 */ { NULL, 0 },
    /* 0x1a */ { NULL, 0 },
    /* 0x1b */ { NULL, 0 },
    /* 0x1c */ { NULL, 0 },
    /* 0x1d */ { NULL, 0 },
    /* 0x1e */ { NULL, 0 },
    /* 0x1f */ { NULL, 0 },
    /* 0x20 */ { NULL, 0 },
    /* 0x21 */ { NULL, 0 },
    /* 0x22 */ { NULL, 0 },
    /* 0x23 */ { NULL, 0 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { NULL, 0 },
    /* 0x29 */ { NULL, 0 },
    /* 0x2a */ { NULL, 0 },
    /* 0x2b */ { NULL, 0 },
    /* 0x2c */ { NULL, 0 },
    /* 0x2d */ { NULL, 0 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { NULL, 0 },
    /* 0x30 */ { NULL, 0 },
    /* 0x31 */ { NULL, 0 },
    /* 0x32 */ { NULL, 0 },
    /* 0x33 */ { NULL, 0 },
    /* 0x34 */ { NULL, 0 },
    /* 0x35 */ { NULL, 0 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { NULL, 0 },
    /* 0x38 */ { NULL, 0 },
    /* 0x39 */ { NULL, 0 },
    /* 0x3a */ { NULL, 0 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { NULL, 0 },
    /* 0x3d */ { NULL, 0 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { NULL, 0 },
    /* 0x41 */ { NULL, 0 },
    /* 0x42 */ { NULL, 0 },
    /* 0x43 */ { NULL, 0 },
    /* 0x44 */ { NULL, 0 },
    /* 0x45 */ { NULL, 0 },
    /* 0x46 */ { NULL, 0 },
    /* 0x47 */ { NULL, 0 },
    /* 0x48 */ { NULL, 0 },
    /* 0x49 */ { NULL, 0 },
    /* 0x4a */ { NULL, 0 },
    /* 0x4b */ { NULL, 0 },
    /* 0x4c */ { NULL, 0 },
    /* 0x4d */ { NULL, 0 },
    /* 0x4e */ { NULL, 0 },
    /* 0x4f */ { NULL, 0 },
    /* 0x50 */ { NULL, 0 },
    /* 0x51 */ { NULL, 0 },
    /* 0x52 */ { NULL, 0 },
    /* 0x53 */ { NULL, 0 },
    /* 0x54 */ { NULL, 0 },
    /* 0x55 */ { NULL, 0 },
    /* 0x56 */ { NULL, 0 },
    /* 0x57 */ { NULL, 0 },
    /* 0x58 */ { NULL, 0 },
    /* 0x59 */ { NULL, 0 },
    /* 0x5a */ { NULL, 0 },
    /* 0x5b */ { NULL, 0 },
    /* 0x5c */ { NULL, 0 },
    /* 0x5d */ { NULL, 0 },
    /* 0x5e */ { NULL, 0 },
    /* 0x5f */ { NULL, 0 },
    /* 0x60 */ { NULL, 0 },
    /* 0x61 */ { NULL, 0 },
    /* 0x62 */ { NULL, 0 },
    /* 0x63 */ { NULL, 0 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { NULL, 0 },
    /* 0x69 */ { NULL, 0 },
    /* 0x6a */ { NULL, 0 },
    /* 0x6b */ { NULL, 0 },
    /* 0x6c */ { NULL, 0 },
    /* 0x6d */ { NULL, 0 },
    /* 0x6e */ { NULL, 0 },
    /* 0x6f */ { NULL, 0 },
    /* 0x70 */ { NULL, 0 },
    /* 0x71 */ { NULL, 0 },
    /* 0x72 */ { NULL, 0 },
    /* 0x73 */ { NULL, 0 },
    /* 0x74 */ { NULL, 0 },
    /* 0x75 */ { NULL, 0 },
    /* 0x76 */ { NULL, 0 },
    /* 0x77 */ { NULL, 0 },
    /* 0x78 */ { NULL, 0 },
    /* 0x79 */ { NULL, 0 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { NULL, 0 },
    /* 0x7c */ { NULL, 0 },
    /* 0x7d */ { NULL, 0 },
    /* 0x7e */ { NULL, 0 },
    /* 0x7f */ { NULL, 0 },
    /* 0x80 */ { NULL, 0 },
    /* 0x81 */ { NULL, 0 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { NULL, 0 },
    /* 0x84 */ { NULL, 0 },
    /* 0x85 */ { NULL, 0 },
    /* 0x86 */ { NULL, 0 },
    /* 0x87 */ { NULL, 0 },
    /* 0x88 */ { NULL, 0 },
    /* 0x89 */ { NULL, 0 },
    /* 0x8a */ { NULL, 0 },
    /* 0x8b */ { NULL, 0 },
    /* 0x8c */ { NULL, 0 },
    /* 0x8d */ { NULL, 0 },
    /* 0x8e */ { NULL, 0 },
    /* 0x8f */ { NULL, 0 },
    /* 0x90 */ { NULL, 0 },
    /* 0x91 */ { NULL, 0 },
    /* 0x92 */ { NULL, 0 },
    /* 0x93 */ { NULL, 0 },
    /* 0x94 */ { NULL, 0 },
    /* 0x95 */ { NULL, 0 },
    /* 0x96 */ { NULL, 0 },
    /* 0x97 */ { NULL, 0 },
    /* 0x98 */ { NULL, 0 },
    /* 0x99 */ { NULL, 0 },
    /* 0x9a */ { NULL, 0 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { NULL, 0 },
    /* 0x9d */ { NULL, 0 },
    /* 0x9e */ { NULL, 0 },
    /* 0x9f */ { NULL, 0 },
    /* 0xa0 */ { NULL, 0 },
    /* 0xa1 */ { NULL, 0 },
    /* 0xa2 */ { NULL, 0 },
    /* 0xa3 */ { NULL, 0 },
    /* 0xa4 */ { NULL, 0 },
    /* 0xa5 */ { NULL, 0 },
    /* 0xa6 */ { NULL, 0 },
    /* 0xa7 */ { NULL, 0 },
    /* 0xa8 */ { NULL, 0 },
    /* 0xa9 */ { NULL, 0 },
    /* 0xaa */ { NULL, 0 },
    /* 0xab */ { NULL, 0 },
    /* 0xac */ { NULL, 0 },
    /* 0xad */ { NULL, 0 },
    /* 0xae */ { NULL, 0 },
    /* 0xaf */ { NULL, 0 },
    /* 0xb0 */ { NULL, 0 },
    /* 0xb1 */ { NULL, 0 },
    /* 0xb2 */ { NULL, 0 },
    /* 0xb3 */ { NULL, 0 },
    /* 0xb4 */ { NULL, 0 },
    /* 0xb5 */ { NULL, 0 },
    /* 0xb6 */ { NULL, 0 },
    /* 0xb7 */ { NULL, 0 },
    /* 0xb8 */ { NULL, 0 },
    /* 0xb9 */ { NULL, 0 },
    /* 0xba */ { NULL, 0 },
    /* 0xbb */ { NULL, 0 },
    /* 0xbc */ { NULL, 0 },
    /* 0xbd */ { NULL, 0 },
    /* 0xbe */ { NULL, 0 },
    /* 0xbf */ { NULL, 0 },
    /* 0xc0 */ { itable_0FA7C0, 1 },
    /* 0xc1 */ { NULL, 0 },
    /* 0xc2 */ { NULL, 0 },
    /* 0xc3 */ { NULL, 0 },
    /* 0xc4 */ { NULL, 0 },
    /* 0xc5 */ { NULL, 0 },
    /* 0xc6 */ { NULL, 0 },
    /* 0xc7 */ { NULL, 0 },
    /* 0xc8 */ { itable_0FA7C8, 1 },
    /* 0xc9 */ { NULL, 0 },
    /* 0xca */ { NULL, 0 },
    /* 0xcb */ { NULL, 0 },
    /* 0xcc */ { NULL, 0 },
    /* 0xcd */ { NULL, 0 },
    /* 0xce */ { NULL, 0 },
    /* 0xcf */ { NULL, 0 },
    /* 0xd0 */ { itable_0FA7D0, 1 },
    /* 0xd1 */ { NULL, 0 },
    /* 0xd2 */ { NULL, 0 },
    /* 0xd3 */ { NULL, 0 },
    /* 0xd4 */ { NULL, 0 },
    /* 0xd5 */ { NULL, 0 },
    /* 0xd6 */ { NULL, 0 },
    /* 0xd7 */ { NULL, 0 },
    /* 0xd8 */ { itable_0FA7D8, 1 },
    /* 0xd9 */ { NULL, 0 },
    /* 0xda */ { NULL, 0 },
    /* 0xdb */ { NULL, 0 },
    /* 0xdc */ { NULL, 0 },
    /* 0xdd */ { NULL, 0 },
    /* 0xde */ { NULL, 0 },
    /* 0xdf */ { NULL, 0 },
    /* 0xe0 */ { itable_0FA7E0, 1 },
    /* 0xe1 */ { NULL, 0 },
    /* 0xe2 */ { NULL, 0 },
    /* 0xe3 */ { NULL, 0 },
    /* 0xe4 */ { NULL, 0 },
    /* 0xe5 */ { NULL, 0 },
    /* 0xe6 */ { NULL, 0 },
    /* 0xe7 */ { NULL, 0 },
    /* 0xe8 */ { itable_0FA7E8, 1 },
    /* 0xe9 */ { NULL, 0 },
    /* 0xea */ { NULL, 0 },
    /* 0xeb */ { NULL, 0 },
    /* 0xec */ { NULL, 0 },
    /* 0xed */ { NULL, 0 },
    /* 0xee */ { NULL, 0 },
    /* 0xef */ { NULL, 0 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { NULL, 0 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { NULL, 0 },
    /* 0xf5 */ { NULL, 0 },
    /* 0xf6 */ { NULL, 0 },
    /* 0xf7 */ { NULL, 0 },
    /* 0xf8 */ { NULL, 0 },
    /* 0xf9 */ { NULL, 0 },
    /* 0xfa */ { NULL, 0 },
    /* 0xfb */ { NULL, 0 },
    /* 0xfc */ { NULL, 0 },
    /* 0xfd */ { NULL, 0 },
    /* 0xfe */ { NULL, 0 },
    /* 0xff */ { NULL, 0 },
};

static const struct disasm_index itable_0F[256] = {
    /* 0x00 */ { itable_0F00, 24 },
    /* 0x01 */ { itable_0F01, 33 },
    /* 0x02 */ { itable_0F02, 10 },
    /* 0x03 */ { itable_0F03, 10 },
    /* 0x04 */ { NULL, 0 },
    /* 0x05 */ { itable_0F05, 2 },
    /* 0x06 */ { itable_0F06, 1 },
    /* 0x07 */ { itable_0F07, 2 },
    /* 0x08 */ { itable_0F08, 1 },
    /* 0x09 */ { itable_0F09, 1 },
    /* 0x0a */ { NULL, 0 },
    /* 0x0b */ { itable_0F0B, 1 },
    /* 0x0c */ { NULL, 0 },
    /* 0x0d */ { itable_0F0D, 2 },
    /* 0x0e */ { itable_0F0E, 1 },
    /* 0x0f */ { itable_0F0F, 26 },
    /* 0x10 */ { itable_0F10, 8 },
    /* 0x11 */ { itable_0F11, 8 },
    /* 0x12 */ { itable_0F12, 5 },
    /* 0x13 */ { itable_0F13, 2 },
    /* 0x14 */ { itable_0F14, 2 },
    /* 0x15 */ { itable_0F15, 2 },
    /* 0x16 */ { itable_0F16, 4 },
    /* 0x17 */ { itable_0F17, 2 },
    /* 0x18 */ { itable_0F18, 28 },
    /* 0x19 */ { itable_0F19, 24 },
    /* 0x1a */ { itable_0F1A, 24 },
    /* 0x1b */ { itable_0F1B, 24 },
    /* 0x1c */ { itable_0F1C, 24 },
    /* 0x1d */ { itable_0F1D, 24 },
    /* 0x1e */ { itable_0F1E, 24 },
    /* 0x1f */ { itable_0F1F, 27 },
    /* 0x20 */ { itable_0F20, 2 },
    /* 0x21 */ { itable_0F21, 2 },
    /* 0x22 */ { itable_0F22, 2 },
    /* 0x23 */ { itable_0F23, 2 },
    /* 0x24 */ { NULL, 0 },
    /* 0x25 */ { NULL, 0 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { NULL, 0 },
    /* 0x28 */ { itable_0F28, 4 },
    /* 0x29 */ { itable_0F29, 4 },
    /* 0x2a */ { itable_0F2A, 6 },
    /* 0x2b */ { itable_0F2B, 4 },
    /* 0x2c */ { itable_0F2C, 8 },
    /* 0x2d */ { itable_0F2D, 10 },
    /* 0x2e */ { itable_0F2E, 2 },
    /* 0x2f */ { itable_0F2F, 2 },
    /* 0x30 */ { itable_0F30, 1 },
    /* 0x31 */ { itable_0F31, 1 },
    /* 0x32 */ { itable_0F32, 1 },
    /* 0x33 */ { itable_0F33, 1 },
    /* 0x34 */ { itable_0F34, 1 },
    /* 0x35 */ { itable_0F35, 1 },
    /* 0x36 */ { itable_0F36, 1 },
    /* 0x37 */ { itable_0F37, 2 },
    /* 0x38 */ { itable_0F38, -1 },
    /* 0x39 */ { itable_0F39, 1 },
    /* 0x3a */ { itable_0F3A, -1 },
    /* 0x3b */ { NULL, 0 },
    /* 0x3c */ { itable_0F3C, 1 },
    /* 0x3d */ { itable_0F3D, 1 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { NULL, 0 },
    /* 0x40 */ { itable_0F40, 6 },
    /* 0x41 */ { itable_0F41, 6 },
    /* 0x42 */ { itable_0F42, 6 },
    /* 0x43 */ { itable_0F43, 6 },
    /* 0x44 */ { itable_0F44, 6 },
    /* 0x45 */ { itable_0F45, 6 },
    /* 0x46 */ { itable_0F46, 6 },
    /* 0x47 */ { itable_0F47, 6 },
    /* 0x48 */ { itable_0F48, 6 },
    /* 0x49 */ { itable_0F49, 6 },
    /* 0x4a */ { itable_0F4A, 6 },
    /* 0x4b */ { itable_0F4B, 6 },
    /* 0x4c */ { itable_0F4C, 6 },
    /* 0x4d */ { itable_0F4D, 6 },
    /* 0x4e */ { itable_0F4E, 6 },
    /* 0x4f */ { itable_0F4F, 6 },
    /* 0x50 */ { itable_0F50, 5 },
    /* 0x51 */ { itable_0F51, 5 },
    /* 0x52 */ { itable_0F52, 3 },
    /* 0x53 */ { itable_0F53, 2 },
    /* 0x54 */ { itable_0F54, 3 },
    /* 0x55 */ { itable_0F55, 3 },
    /* 0x56 */ { itable_0F56, 2 },
    /* 0x57 */ { itable_0F57, 2 },
    /* 0x58 */ { itable_0F58, 5 },
    /* 0x59 */ { itable_0F59, 5 },
    /* 0x5a */ { itable_0F5A, 5 },
    /* 0x5b */ { itable_0F5B, 4 },
    /* 0x5c */ { itable_0F5C, 5 },
    /* 0x5d */ { itable_0F5D, 5 },
    /* 0x5e */ { itable_0F5E, 5 },
    /* 0x5f */ { itable_0F5F, 4 },
    /* 0x60 */ { itable_0F60, 2 },
    /* 0x61 */ { itable_0F61, 2 },
    /* 0x62 */ { itable_0F62, 2 },
    /* 0x63 */ { itable_0F63, 2 },
    /* 0x64 */ { itable_0F64, 2 },
    /* 0x65 */ { itable_0F65, 2 },
    /* 0x66 */ { itable_0F66, 2 },
    /* 0x67 */ { itable_0F67, 2 },
    /* 0x68 */ { itable_0F68, 2 },
    /* 0x69 */ { itable_0F69, 2 },
    /* 0x6a */ { itable_0F6A, 2 },
    /* 0x6b */ { itable_0F6B, 2 },
    /* 0x6c */ { itable_0F6C, 1 },
    /* 0x6d */ { itable_0F6D, 1 },
    /* 0x6e */ { itable_0F6E, 8 },
    /* 0x6f */ { itable_0F6F, 5 },
    /* 0x70 */ { itable_0F70, 7 },
    /* 0x71 */ { itable_0F71, 6 },
    /* 0x72 */ { itable_0F72, 6 },
    /* 0x73 */ { itable_0F73, 6 },
    /* 0x74 */ { itable_0F74, 2 },
    /* 0x75 */ { itable_0F75, 2 },
    /* 0x76 */ { itable_0F76, 2 },
    /* 0x77 */ { itable_0F77, 1 },
    /* 0x78 */ { itable_0F78, 5 },
    /* 0x79 */ { itable_0F79, 5 },
    /* 0x7a */ { NULL, 0 },
    /* 0x7b */ { itable_0F7B, 1 },
    /* 0x7c */ { itable_0F7C, 3 },
    /* 0x7d */ { itable_0F7D, 3 },
    /* 0x7e */ { itable_0F7E, 10 },
    /* 0x7f */ { itable_0F7F, 5 },
    /* 0x80 */ { itable_0F80, 3 },
    /* 0x81 */ { itable_0F81, 3 },
    /* 0x82 */ { itable_0F82, 3 },
    /* 0x83 */ { itable_0F83, 3 },
    /* 0x84 */ { itable_0F84, 3 },
    /* 0x85 */ { itable_0F85, 3 },
    /* 0x86 */ { itable_0F86, 3 },
    /* 0x87 */ { itable_0F87, 3 },
    /* 0x88 */ { itable_0F88, 3 },
    /* 0x89 */ { itable_0F89, 3 },
    /* 0x8a */ { itable_0F8A, 3 },
    /* 0x8b */ { itable_0F8B, 3 },
    /* 0x8c */ { itable_0F8C, 3 },
    /* 0x8d */ { itable_0F8D, 3 },
    /* 0x8e */ { itable_0F8E, 3 },
    /* 0x8f */ { itable_0F8F, 3 },
    /* 0x90 */ { itable_0F90, 2 },
    /* 0x91 */ { itable_0F91, 2 },
    /* 0x92 */ { itable_0F92, 2 },
    /* 0x93 */ { itable_0F93, 2 },
    /* 0x94 */ { itable_0F94, 2 },
    /* 0x95 */ { itable_0F95, 2 },
    /* 0x96 */ { itable_0F96, 2 },
    /* 0x97 */ { itable_0F97, 2 },
    /* 0x98 */ { itable_0F98, 2 },
    /* 0x99 */ { itable_0F99, 2 },
    /* 0x9a */ { itable_0F9A, 2 },
    /* 0x9b */ { itable_0F9B, 2 },
    /* 0x9c */ { itable_0F9C, 2 },
    /* 0x9d */ { itable_0F9D, 2 },
    /* 0x9e */ { itable_0F9E, 2 },
    /* 0x9f */ { itable_0F9F, 2 },
    /* 0xa0 */ { itable_0FA0, 1 },
    /* 0xa1 */ { itable_0FA1, 1 },
    /* 0xa2 */ { itable_0FA2, 1 },
    /* 0xa3 */ { itable_0FA3, 6 },
    /* 0xa4 */ { itable_0FA4, 6 },
    /* 0xa5 */ { itable_0FA5, 6 },
    /* 0xa6 */ { itable_0FA6, -1 },
    /* 0xa7 */ { itable_0FA7, -1 },
    /* 0xa8 */ { itable_0FA8, 1 },
    /* 0xa9 */ { itable_0FA9, 1 },
    /* 0xaa */ { itable_0FAA, 1 },
    /* 0xab */ { itable_0FAB, 6 },
    /* 0xac */ { itable_0FAC, 6 },
    /* 0xad */ { itable_0FAD, 6 },
    /* 0xae */ { itable_0FAE, 27 },
    /* 0xaf */ { itable_0FAF, 6 },
    /* 0xb0 */ { itable_0FB0, 2 },
    /* 0xb1 */ { itable_0FB1, 6 },
    /* 0xb2 */ { itable_0FB2, 3 },
    /* 0xb3 */ { itable_0FB3, 6 },
    /* 0xb4 */ { itable_0FB4, 3 },
    /* 0xb5 */ { itable_0FB5, 3 },
    /* 0xb6 */ { itable_0FB6, 4 },
    /* 0xb7 */ { itable_0FB7, 2 },
    /* 0xb8 */ { itable_0FB8, 6 },
    /* 0xb9 */ { itable_0FB9, 1 },
    /* 0xba */ { itable_0FBA, 12 },
    /* 0xbb */ { itable_0FBB, 6 },
    /* 0xbc */ { itable_0FBC, 6 },
    /* 0xbd */ { itable_0FBD, 9 },
    /* 0xbe */ { itable_0FBE, 4 },
    /* 0xbf */ { itable_0FBF, 2 },
    /* 0xc0 */ { itable_0FC0, 2 },
    /* 0xc1 */ { itable_0FC1, 6 },
    /* 0xc2 */ { itable_0FC2, 38 },
    /* 0xc3 */ { itable_0FC3, 2 },
    /* 0xc4 */ { itable_0FC4, 6 },
    /* 0xc5 */ { itable_0FC5, 2 },
    /* 0xc6 */ { itable_0FC6, 4 },
    /* 0xc7 */ { itable_0FC7, 9 },
    /* 0xc8 */ { itable_0FC8, 2 },
    /* 0xc9 */ { itable_0FC9, 2 },
    /* 0xca */ { itable_0FCA, 2 },
    /* 0xcb */ { itable_0FCB, 2 },
    /* 0xcc */ { itable_0FCC, 2 },
    /* 0xcd */ { itable_0FCD, 2 },
    /* 0xce */ { itable_0FCE, 2 },
    /* 0xcf */ { itable_0FCF, 2 },
    /* 0xd0 */ { itable_0FD0, 2 },
    /* 0xd1 */ { itable_0FD1, 2 },
    /* 0xd2 */ { itable_0FD2, 2 },
    /* 0xd3 */ { itable_0FD3, 2 },
    /* 0xd4 */ { itable_0FD4, 2 },
    /* 0xd5 */ { itable_0FD5, 2 },
    /* 0xd6 */ { itable_0FD6, 4 },
    /* 0xd7 */ { itable_0FD7, 2 },
    /* 0xd8 */ { itable_0FD8, 2 },
    /* 0xd9 */ { itable_0FD9, 2 },
    /* 0xda */ { itable_0FDA, 2 },
    /* 0xdb */ { itable_0FDB, 2 },
    /* 0xdc */ { itable_0FDC, 2 },
    /* 0xdd */ { itable_0FDD, 2 },
    /* 0xde */ { itable_0FDE, 2 },
    /* 0xdf */ { itable_0FDF, 2 },
    /* 0xe0 */ { itable_0FE0, 2 },
    /* 0xe1 */ { itable_0FE1, 2 },
    /* 0xe2 */ { itable_0FE2, 2 },
    /* 0xe3 */ { itable_0FE3, 2 },
    /* 0xe4 */ { itable_0FE4, 2 },
    /* 0xe5 */ { itable_0FE5, 2 },
    /* 0xe6 */ { itable_0FE6, 3 },
    /* 0xe7 */ { itable_0FE7, 2 },
    /* 0xe8 */ { itable_0FE8, 2 },
    /* 0xe9 */ { itable_0FE9, 2 },
    /* 0xea */ { itable_0FEA, 2 },
    /* 0xeb */ { itable_0FEB, 2 },
    /* 0xec */ { itable_0FEC, 2 },
    /* 0xed */ { itable_0FED, 2 },
    /* 0xee */ { itable_0FEE, 2 },
    /* 0xef */ { itable_0FEF, 2 },
    /* 0xf0 */ { itable_0FF0, 1 },
    /* 0xf1 */ { itable_0FF1, 2 },
    /* 0xf2 */ { itable_0FF2, 2 },
    /* 0xf3 */ { itable_0FF3, 2 },
    /* 0xf4 */ { itable_0FF4, 2 },
    /* 0xf5 */ { itable_0FF5, 2 },
    /* 0xf6 */ { itable_0FF6, 2 },
    /* 0xf7 */ { itable_0FF7, 2 },
    /* 0xf8 */ { itable_0FF8, 2 },
    /* 0xf9 */ { itable_0FF9, 2 },
    /* 0xfa */ { itable_0FFA, 2 },
    /* 0xfb */ { itable_0FFB, 2 },
    /* 0xfc */ { itable_0FFC, 2 },
    /* 0xfd */ { itable_0FFD, 2 },
    /* 0xfe */ { itable_0FFE, 2 },
    /* 0xff */ { itable_0FFF, 1 },
};

const struct disasm_index itable[256] = {
    /* 0x00 */ { itable_00, 2 },
    /* 0x01 */ { itable_01, 6 },
    /* 0x02 */ { itable_02, 2 },
    /* 0x03 */ { itable_03, 6 },
    /* 0x04 */ { itable_04, 1 },
    /* 0x05 */ { itable_05, 3 },
    /* 0x06 */ { itable_06, 2 },
    /* 0x07 */ { itable_07, 1 },
    /* 0x08 */ { itable_08, 2 },
    /* 0x09 */ { itable_09, 6 },
    /* 0x0a */ { itable_0A, 2 },
    /* 0x0b */ { itable_0B, 6 },
    /* 0x0c */ { itable_0C, 1 },
    /* 0x0d */ { itable_0D, 3 },
    /* 0x0e */ { itable_0E, 2 },
    /* 0x0f */ { itable_0F, -1 },
    /* 0x10 */ { itable_10, 2 },
    /* 0x11 */ { itable_11, 6 },
    /* 0x12 */ { itable_12, 2 },
    /* 0x13 */ { itable_13, 6 },
    /* 0x14 */ { itable_14, 1 },
    /* 0x15 */ { itable_15, 3 },
    /* 0x16 */ { itable_16, 2 },
    /* 0x17 */ { itable_17, 1 },
    /* 0x18 */ { itable_18, 2 },
    /* 0x19 */ { itable_19, 6 },
    /* 0x1a */ { itable_1A, 2 },
    /* 0x1b */ { itable_1B, 6 },
    /* 0x1c */ { itable_1C, 1 },
    /* 0x1d */ { itable_1D, 3 },
    /* 0x1e */ { itable_1E, 2 },
    /* 0x1f */ { itable_1F, 1 },
    /* 0x20 */ { itable_20, 2 },
    /* 0x21 */ { itable_21, 6 },
    /* 0x22 */ { itable_22, 2 },
    /* 0x23 */ { itable_23, 6 },
    /* 0x24 */ { itable_24, 1 },
    /* 0x25 */ { itable_25, 3 },
    /* 0x26 */ { NULL, 0 },
    /* 0x27 */ { itable_27, 1 },
    /* 0x28 */ { itable_28, 2 },
    /* 0x29 */ { itable_29, 6 },
    /* 0x2a */ { itable_2A, 2 },
    /* 0x2b */ { itable_2B, 6 },
    /* 0x2c */ { itable_2C, 1 },
    /* 0x2d */ { itable_2D, 3 },
    /* 0x2e */ { NULL, 0 },
    /* 0x2f */ { itable_2F, 1 },
    /* 0x30 */ { itable_30, 2 },
    /* 0x31 */ { itable_31, 6 },
    /* 0x32 */ { itable_32, 2 },
    /* 0x33 */ { itable_33, 6 },
    /* 0x34 */ { itable_34, 1 },
    /* 0x35 */ { itable_35, 3 },
    /* 0x36 */ { NULL, 0 },
    /* 0x37 */ { itable_37, 1 },
    /* 0x38 */ { itable_38, 2 },
    /* 0x39 */ { itable_39, 6 },
    /* 0x3a */ { itable_3A, 2 },
    /* 0x3b */ { itable_3B, 6 },
    /* 0x3c */ { itable_3C, 1 },
    /* 0x3d */ { itable_3D, 3 },
    /* 0x3e */ { NULL, 0 },
    /* 0x3f */ { itable_3F, 1 },
    /* 0x40 */ { itable_40, 2 },
    /* 0x41 */ { itable_41, 2 },
    /* 0x42 */ { itable_42, 2 },
    /* 0x43 */ { itable_43, 2 },
    /* 0x44 */ { itable_44, 2 },
    /* 0x45 */ { itable_45, 2 },
    /* 0x46 */ { itable_46, 2 },
    /* 0x47 */ { itable_47, 2 },
    /* 0x48 */ { itable_48, 2 },
    /* 0x49 */ { itable_49, 2 },
    /* 0x4a */ { itable_4A, 2 },
    /* 0x4b */ { itable_4B, 2 },
    /* 0x4c */ { itable_4C, 2 },
    /* 0x4d */ { itable_4D, 2 },
    /* 0x4e */ { itable_4E, 2 },
    /* 0x4f */ { itable_4F, 2 },
    /* 0x50 */ { itable_50, 3 },
    /* 0x51 */ { itable_51, 3 },
    /* 0x52 */ { itable_52, 3 },
    /* 0x53 */ { itable_53, 3 },
    /* 0x54 */ { itable_54, 3 },
    /* 0x55 */ { itable_55, 3 },
    /* 0x56 */ { itable_56, 3 },
    /* 0x57 */ { itable_57, 3 },
    /* 0x58 */ { itable_58, 3 },
    /* 0x59 */ { itable_59, 3 },
    /* 0x5a */ { itable_5A, 3 },
    /* 0x5b */ { itable_5B, 3 },
    /* 0x5c */ { itable_5C, 3 },
    /* 0x5d */ { itable_5D, 3 },
    /* 0x5e */ { itable_5E, 3 },
    /* 0x5f */ { itable_5F, 3 },
    /* 0x60 */ { itable_60, 3 },
    /* 0x61 */ { itable_61, 3 },
    /* 0x62 */ { itable_62, 2 },
    /* 0x63 */ { itable_63, 3 },
    /* 0x64 */ { NULL, 0 },
    /* 0x65 */ { NULL, 0 },
    /* 0x66 */ { NULL, 0 },
    /* 0x67 */ { NULL, 0 },
    /* 0x68 */ { itable_68, 5 },
    /* 0x69 */ { itable_69, 9 },
    /* 0x6a */ { itable_6A, 6 },
    /* 0x6b */ { itable_6B, 9 },
    /* 0x6c */ { itable_6C, 1 },
    /* 0x6d */ { itable_6D, 2 },
    /* 0x6e */ { itable_6E, 1 },
    /* 0x6f */ { itable_6F, 2 },
    /* 0x70 */ { itable_70, 1 },
    /* 0x71 */ { itable_71, 1 },
    /* 0x72 */ { itable_72, 1 },
    /* 0x73 */ { itable_73, 1 },
    /* 0x74 */ { itable_74, 1 },
    /* 0x75 */ { itable_75, 1 },
    /* 0x76 */ { itable_76, 1 },
    /* 0x77 */ { itable_77, 1 },
    /* 0x78 */ { itable_78, 1 },
    /* 0x79 */ { itable_79, 1 },
    /* 0x7a */ { itable_7A, 1 },
    /* 0x7b */ { itable_7B, 1 },
    /* 0x7c */ { itable_7C, 1 },
    /* 0x7d */ { itable_7D, 1 },
    /* 0x7e */ { itable_7E, 1 },
    /* 0x7f */ { itable_7F, 1 },
    /* 0x80 */ { itable_80, 16 },
    /* 0x81 */ { itable_81, 40 },
    /* 0x82 */ { NULL, 0 },
    /* 0x83 */ { itable_83, 88 },
    /* 0x84 */ { itable_84, 3 },
    /* 0x85 */ { itable_85, 9 },
    /* 0x86 */ { itable_86, 4 },
    /* 0x87 */ { itable_87, 12 },
    /* 0x88 */ { itable_88, 2 },
    /* 0x89 */ { itable_89, 6 },
    /* 0x8a */ { itable_8A, 2 },
    /* 0x8b */ { itable_8B, 6 },
    /* 0x8c */ { itable_8C, 3 },
    /* 0x8d */ { itable_8D, 3 },
    /* 0x8e */ { itable_8E, 3 },
    /* 0x8f */ { itable_8F, 3 },
    /* 0x90 */ { itable_90, 9 },
    /* 0x91 */ { itable_91, 6 },
    /* 0x92 */ { itable_92, 6 },
    /* 0x93 */ { itable_93, 6 },
    /* 0x94 */ { itable_94, 6 },
    /* 0x95 */ { itable_95, 6 },
    /* 0x96 */ { itable_96, 6 },
    /* 0x97 */ { itable_97, 6 },
    /* 0x98 */ { itable_98, 3 },
    /* 0x99 */ { itable_99, 3 },
    /* 0x9a */ { itable_9A, 5 },
    /* 0x9b */ { NULL, 0 },
    /* 0x9c */ { itable_9C, 4 },
    /* 0x9d */ { itable_9D, 4 },
    /* 0x9e */ { itable_9E, 1 },
    /* 0x9f */ { itable_9F, 1 },
    /* 0xa0 */ { itable_A0, 1 },
    /* 0xa1 */ { itable_A1, 3 },
    /* 0xa2 */ { itable_A2, 1 },
    /* 0xa3 */ { itable_A3, 3 },
    /* 0xa4 */ { itable_A4, 1 },
    /* 0xa5 */ { itable_A5, 3 },
    /* 0xa6 */ { itable_A6, 1 },
    /* 0xa7 */ { itable_A7, 3 },
    /* 0xa8 */ { itable_A8, 1 },
    /* 0xa9 */ { itable_A9, 3 },
    /* 0xaa */ { itable_AA, 1 },
    /* 0xab */ { itable_AB, 3 },
    /* 0xac */ { itable_AC, 1 },
    /* 0xad */ { itable_AD, 3 },
    /* 0xae */ { itable_AE, 1 },
    /* 0xaf */ { itable_AF, 3 },
    /* 0xb0 */ { itable_B0, 1 },
    /* 0xb1 */ { itable_B1, 1 },
    /* 0xb2 */ { itable_B2, 1 },
    /* 0xb3 */ { itable_B3, 1 },
    /* 0xb4 */ { itable_B4, 1 },
    /* 0xb5 */ { itable_B5, 1 },
    /* 0xb6 */ { itable_B6, 1 },
    /* 0xb7 */ { itable_B7, 1 },
    /* 0xb8 */ { itable_B8, 3 },
    /* 0xb9 */ { itable_B9, 3 },
    /* 0xba */ { itable_BA, 3 },
    /* 0xbb */ { itable_BB, 3 },
    /* 0xbc */ { itable_BC, 3 },
    /* 0xbd */ { itable_BD, 3 },
    /* 0xbe */ { itable_BE, 3 },
    /* 0xbf */ { itable_BF, 3 },
    /* 0xc0 */ { itable_C0, 7 },
    /* 0xc1 */ { itable_C1, 21 },
    /* 0xc2 */ { itable_C2, 2 },
    /* 0xc3 */ { itable_C3, 2 },
    /* 0xc4 */ { itable_C4, 2 },
    /* 0xc5 */ { itable_C5, 2 },
    /* 0xc6 */ { itable_C6, 2 },
    /* 0xc7 */ { itable_C7, 6 },
    /* 0xc8 */ { itable_C8, 1 },
    /* 0xc9 */ { itable_C9, 1 },
    /* 0xca */ { itable_CA, 1 },
    /* 0xcb */ { itable_CB, 1 },
    /* 0xcc */ { itable_CC, 1 },
    /* 0xcd */ { itable_CD, 1 },
    /* 0xce */ { itable_CE, 1 },
    /* 0xcf */ { itable_CF, 4 },
    /* 0xd0 */ { itable_D0, 7 },
    /* 0xd1 */ { itable_D1, 21 },
    /* 0xd2 */ { itable_D2, 7 },
    /* 0xd3 */ { itable_D3, 21 },
    /* 0xd4 */ { itable_D4, 2 },
    /* 0xd5 */ { itable_D5, 2 },
    /* 0xd6 */ { itable_D6, 1 },
    /* 0xd7 */ { itable_D7, 2 },
    /* 0xd8 */ { itable_D8, 24 },
    /* 0xd9 */ { itable_D9, 41 },
    /* 0xda */ { itable_DA, 17 },
    /* 0xdb */ { itable_DB, 27 },
    /* 0xdc */ { itable_DC, 20 },
    /* 0xdd */ { itable_DD, 17 },
    /* 0xde */ { itable_DE, 21 },
    /* 0xdf */ { itable_DF, 18 },
    /* 0xe0 */ { itable_E0, 8 },
    /* 0xe1 */ { itable_E1, 8 },
    /* 0xe2 */ { itable_E2, 4 },
    /* 0xe3 */ { itable_E3, 3 },
    /* 0xe4 */ { itable_E4, 1 },
    /* 0xe5 */ { itable_E5, 2 },
    /* 0xe6 */ { itable_E6, 1 },
    /* 0xe7 */ { itable_E7, 2 },
    /* 0xe8 */ { itable_E8, 6 },
    /* 0xe9 */ { itable_E9, 3 },
    /* 0xea */ { itable_EA, 5 },
    /* 0xeb */ { itable_EB, 1 },
    /* 0xec */ { itable_EC, 1 },
    /* 0xed */ { itable_ED, 2 },
    /* 0xee */ { itable_EE, 1 },
    /* 0xef */ { itable_EF, 2 },
    /* 0xf0 */ { NULL, 0 },
    /* 0xf1 */ { itable_F1, 2 },
    /* 0xf2 */ { NULL, 0 },
    /* 0xf3 */ { NULL, 0 },
    /* 0xf4 */ { itable_F4, 1 },
    /* 0xf5 */ { itable_F5, 1 },
    /* 0xf6 */ { itable_F6, 8 },
    /* 0xf7 */ { itable_F7, 23 },
    /* 0xf8 */ { itable_F8, 1 },
    /* 0xf9 */ { itable_F9, 1 },
    /* 0xfa */ { itable_FA, 1 },
    /* 0xfb */ { itable_FB, 1 },
    /* 0xfc */ { itable_FC, 1 },
    /* 0xfd */ { itable_FD, 1 },
    /* 0xfe */ { itable_FE, 2 },
    /* 0xff */ { itable_FF, 41 },
};

const struct disasm_index * const itable_vex[2][32][4] =
{
    {
        { NULL,          NULL,          NULL,          NULL,          },
        { itable_vex010, itable_vex011, itable_vex012, itable_vex013, },
        { NULL,          itable_vex021, NULL,          NULL,          },
        { NULL,          itable_vex031, NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
    },
    {
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { itable_xop080, NULL,          NULL,          NULL,          },
        { itable_xop090, NULL,          NULL,          NULL,          },
        { itable_xop0A0, NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
        { NULL,          NULL,          NULL,          NULL,          },
    },
};
