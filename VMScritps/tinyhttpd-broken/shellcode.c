#include <stdlib.h>
void main() {


__asm__(

"[SECTION .text]"
 
"global _start"

"_start:\n\t"
	"jmp short ender\n\t"
	"starter:\n\t"
	"xor eax, eax\n\t"
	"xor ebx, ebx\n\t"
	"xor edx, edx\n\t"
	"xor ecx, ecx\n\t"

	"move al, 4\n\t"
	"mov bl, 1\n\t"
	"pop ecx\n\t"
	"mov dl, 23\n\t"
	"int 0x80\n\t"


	"xor eax, eax\n\t"
	"mov al, 1\n\t"
	"xor ebx, ebx\n\t"
	"int 0x80\n\t"
	
	"ender:\n\t"
	"call starter"
	"db 'Now I pwn your computer'"
);
}
