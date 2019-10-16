#!/bin/bash
echo "Compiling test.c..."
bcc -ansi -c -o test.o test.c
echo "Compiling os-api.asm..."
as86 -o os-api.o os-api.asm
echo "Linking the programs..."
ld86 -o syscall-test -d test.o os-api.o
echo "Copying the test program into the sector 11 of the floppy..."
dd if=syscall-test of=floppya.img bs=512 count=1 seek=11 conv=notrunc