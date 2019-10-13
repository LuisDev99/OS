#!/bin/bash
echo "Building bootload.."
nasm bootload.asm
echo "Copying dev/zero to floppya.img.."
dd if=/dev/zero of=floppya.img bs=512 count=2880
echo "Copying bootload to floppya.img.."
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
echo "Compiling kernels.."
bcc -ansi -c -o kernel.o kernel.c
echo "Assembling kernels.."
as86 kernel.asm -o kernel_asm.o
echo "Linking files.."
ld86 -o kernel -d kernel.o kernel_asm.o
echo "Putting kernel to sector 3.."
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3