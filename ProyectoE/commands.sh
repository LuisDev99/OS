#!/bin/bash
echo "Building bootload.."
nasm bootload.asm
echo "Copying dev/zero to floppya.img.."
dd if=/dev/zero of=floppya.img bs=512 count=2880
echo "Copying bootload to floppya.img.."
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc

#Check if every command ran correctly, if not, echo and stopped the execution
echo "Compiling kernels.."
bcc -ansi -c -o kernel.o kernel.c

if [ $? -eq 0 ]; then
    echo COMPILED KERNELS GOOD

    echo "Assembling kernels.."
    as86 kernel.asm -o kernel_asm.o

    if [ $? -eq 0 ]; then
        echo ASSEMBLING KERNELS GOOD

        echo "Linking files.."
        ld86 -o kernel -d kernel.o kernel_asm.o
        echo "Putting kernel to sector 3.."
        dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
        echo "Copying Message.txt to floppy ..."
        dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc
        echo "Copying the test program into the sector 11 of the floppy..."
        dd if=syscall-test of=floppya.img bs=512 count=1 seek=11 conv=notrunc
        echo "Executing bochs..."
        echo c | bochs -f ./bochsrc-2.6.9.bxrc
        echo "Boch finished, deleting object files now... :D"
        rm *.o

    else
        echo ASSEMBLING KERNELS FAIL, BOCHS STOPPED
    fi

else
    echo COMPILED KERNEL FAIL, BOCHS STOPPED
fi