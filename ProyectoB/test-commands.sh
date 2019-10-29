#!/bin/bash
echo "Compiling test.c..."
bcc -ansi -c -o test.o test.c

if [ $? -eq 0 ]; then
    echo COMPILED TEST.c GOOD
else
    echo COMPILED TEST.c FAIL
fi

echo "Compiling os-api.asm..."
as86 os-api.asm -o os-api.o
if [ $? -eq 0 ]; then
    echo COMPILED os-api.asm GOOD
else
    echo COMPILED os-api.asm FAIL
fi

echo "Linking the programs..."
ld86 -o syscall-test -d test.o os-api.o
if [ $? -eq 0 ]; then
    echo LINKING OK
    echo "Copying the test program into the sector 11 of the floppy..."
    dd if=syscall-test of=floppya.img bs=512 count=1 seek=11 conv=notrunc
else
    echo LINKING FAIL
fi
