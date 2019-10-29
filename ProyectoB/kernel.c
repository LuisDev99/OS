void printChar(char ch);
void printString(char *str);
void readString(char *buffer);
void readSector(char *buffer, int sector);
char readChar();
void handleInterrupt21(int AX, int BX, int CX, int DX);

char line[80];
char buffer[512];

int main()
{
    // readString(line);
    // printString("\nYou wrote: \0");
    // printString(line);
    // readSector(buffer, 30);
    // printString("\nReadingSector:\n");
    // printString(buffer);
    makeInterrupt21();
    loadProgram();

    while (1)
    {
    }
    return 0;
}

void printString(char *str)
{
    while (*str != '\0')
    {
        printChar(*str);
        str = str + 1;
    }
}

void readString(char *buffer)
{
    char c = 'a';
    int i = 0;
    while (c != 13 && 1)
    {
        c = readChar();

        if (c != 8) /* If c is not backspace, then print and store */
        {
            //line[i] = c;
            *buffer++ = c;
            i++;
            printChar(c);
        }
        else
        {
            if (i > 0)
            { /*Make sure that i doesnt go below 0*/
                i--;
                buffer--;
            }

            printChar(0x8);
            printChar(' ');
            printChar(0x8);
        }
    }

    //line[i + 1] = '\0';
    *++buffer = '\0';
}

void handleInterrupt21(int AX, int BX, int CX, int DX)
{
    if (AX == 0)
    {
        printString(BX);
    }
    else if (AX == 1)
    {
        readString(BX);
    }
    else if (AX == 2)
    {
        readSector(BX, CX);
    }
    else
    {
        printString("Error on interrupt 21, unknown command on register AX!\0");
    }
}