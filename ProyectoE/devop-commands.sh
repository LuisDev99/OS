#!/bin/bash
echo "Building bootload.."
nasm bootload.asm

echo "Copying dev/zero to floppya.img.."
dd if=/dev/zero of=floppya.img bs=512 count=2880

echo "Copying bootload to floppya.img.."
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc

# ./shellCommands
# ./testCommands

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

            echo "Putting kernel to sector 3.."
            dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

            echo "Copying Message.txt to floppy ..."
            dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc

            printf "\nSETTING UP THE INITIAL FILE SYSTEM\n"

            echo "Copying map.img to floppy ..."
            dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc

            echo "Copying dir.img to floppy ..."
            dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc

            printf "\nSetting up the filesystem on a separated terminal ...\n"
            gnome-terminal -e "./fuse-smallfs-master/bin/sfs floppya.img sfs_root/ -f -s"

            printf "\nSuccess, sleeping this bash to make sure the other terminal completes its tasks..."
            sleep 2

            printf "\nCopying message.txt to sfs_root/ ... \n"

            #TO COPY FILES INTO FLOPPY DISK (File system), DO IT IN HERE
            cp message.txt sfs_root/
            cp test.c sfs_root/
            cp syscall-test sfs_root/
            cp shell sfs_root/
            cp os-api.asm sfs_root/
            cp ok.ct sfs_root/
            cp phello sfs_root/

            printf "\nDONE WITH SETUP\n"

            echo "Unmounting the file system (sfs_root)..."
            fusermount -u sfs_root

            #echo "Copying the test program into the sector 11 of the floppy..."
            #dd if=syscall-test of=floppya.img bs=512 count=1 seek=11 conv=notrunc

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