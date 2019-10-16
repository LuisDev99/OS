void printChar(char ch);
void printString(char *);

int main()
{
    printString("Hello world\0");
    printString("Hello world\0");
    printString("Hello world\0");
    printString("Hello world\0");
    printString("Hello world\0");
    printString("Hello world\0");
    return 0;
}

void printString(char *str)
{
    while (*str != '\0')
    {
        printChar(*str);
        str++;
    }
}