
void main(){
__asm__(

"lea    0x4(%esp),%ecx\n\t"


"and    $0xfffffff0,%esp\n\t"
"pushl  -0x4(%ecx)\n\t"
"push   %ebp\n\t"
"mov    %esp,%ebp\n\t"
"push   %ecx\n\t"
"sub    $0x24,%esp\n\t"

"movl   $0x80484c0,-0x10(%ebp)\n\t"
"movl   $0x80484c5,-0xc(%ebp)\n\t"
"movl   $0x0,-0x8(%ebp)\n\t"
"movl   $0x0,0x8(%esp)\n\t"
"lea    -0x10(%ebp),%eax\n\t"
"mov    %eax,0x4(%esp)\n\t"
"movl   $0x80484dd,(%esp)\n\t"
"jmp    *0x8049650\n\t"
"push   $0x10\n\t"
"jmp    0x8048288\n\t"
"add    $0x24,%esp\n\t"
"pop    %ecx\n\t"
"pop    %ebp\n\t"
"lea    -0x4(%ecx),%esp\n\t"

"ret" 
);
}
