int main()
{

    char buffer[13312];
    int i = 0;
    //syscall_readFile("messag", buffer);
    //syscall_printString(buffer);
    for (i = 0; i < 10000; i++)
        syscall_printString("Dios es amor!");

    syscall_printString("Finished");
    syscall_terminate();
}
