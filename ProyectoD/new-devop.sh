#!/bin/bash
echo "Building bootload.."
nasm bootload.asm

#Check if every command ran correctly, if not, echo and stopped the execution
echo "Compiling kernels.."
bcc -ansi -c -o kernel.o kernel.c

if [ $? -eq 0 ]; then
    echo COMPILED KERNELS GOOD

    echo "Assembling kernels.."
    as86 kernel.asm -o kernel_asm.o

    if [ $? -eq 0 ]; then
        echo ASSEMBLING KERNELS GOOD

        echo "Linking kernels.."
        ld86 -o kernel -d kernel.o kernel_asm.o

        if [ $? -eq 0 ]; then

            echo LINKING KERNELS GOOD
            echo "Copying the test program into the sector 11 of the floppy..."
            dd if=syscall-test of=floppya.img bs=512 count=1 seek=11 conv=notrunc

            echo "Executing bochs..."
            echo c | bochs -f ./bochsrc-2.6.9.bxrc

            echo "Boch finished, deleting object files now... :D"
            rm *.o

            echo "Unmounting the file system (sfs_root)..."
            fusermount -u sfs_root
            
        else 
            echo LINKING KERNEL FAILED, BOCHS STOPPED
        fi

    else
        echo ASSEMBLING KERNELS FAIL, BOCHS STOPPED
    fi

else
    echo COMPILED KERNEL FAIL, BOCHS STOPPED
fi