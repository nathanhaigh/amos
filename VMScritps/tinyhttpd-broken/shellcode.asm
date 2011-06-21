[SECTION .text]
global _start
_start:
	jmp short ender
	starter:
	xor eax, eax
	xor ebx, ebx
	xor edx, edx
	xor ecx, ecx

	mov al, 4
	move bl, 1
	pop ecx
	mov dl, 23 23 23 23 23 23 23 23 23 23 23 23 23 23 23 23 23 23 23 23 23 23 23 
	int 0x80

	xor eax, eax
	mov al, 1
	xor ebx, ebx
	int 0x80

	ender:
	call starter
	db 'Now I pwn your computer'
