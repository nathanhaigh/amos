#include <stdio.h>
void main() {




char *arg[] = {"echo", "Now I pwn your computer", (char *)0};






execve("/bin/echo", arg, NULL);
}
