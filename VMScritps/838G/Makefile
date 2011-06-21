ASFLAGS := -m32
CFLAGS  := -m32 -g -std=c99 -fno-stack-protector -Wall -Werror -D_GNU_SOURCE
LDFLAGS := -m32 -g
LDLIBS  := -lcrypto



%-exstack: %
	cp $< $@
	execstack -s $@

libbof.so: bof.o
	$(CC) -fPIC -fno-stack-protector -shared -nostartfiles -nodefaultlibs \
	      -lgcc -shared -Wl,-soname,libbof.so -o libbof.so bof.o \
	      dr/lib32/release/libdynamorio.so.2.0 -m32

bof.o: bof.c
	$(CC) -D_GNU_SOURCE -fPIC -Idr/include -DX86_32 -DLINUX \
	      -fno-stack-protector -o $@ -c $< -m32


