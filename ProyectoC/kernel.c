#define TRUE 1
#define FALSE 0

void putInMemory(int segment, int address, char character);
void printChar(char ch);
void printString(char *str);
void readFile(char *fileName, char *buffer);
void readString(char *buffer);
void readSector(char *buffer, int sector);
char readChar();
int compareFileNameStrings(char str1[6], char str2[6]);
void handleInterrupt21(int AX, int BX, int CX, int DX);
void loadFileIntoBuffer(char *fileSectorPointers, char *buffer);
int binToDec(char *fileSectorPointer);
int power(int base, int exp);
void executeProgram(char *name, int segment);
void moveByteToMemory(char byte, int segment);
void launchProgram(int segment);
void terminate();
void clearScreen();

//char line[80];
//char buffer[13312];
int main()
{

    //readFile("test.c", buffer);
    //printString(buffer);

    //asm("push 0x41 \n\t push 0x8140 \n\t 0xB000 \n\t call _putInMemory ");
    //asm("push 0x7  \n\t push 0x8141 \n\t 0xB000 \n\t call _putInMemory ");
    //asm("call _makeInterrupt21 \n\t call _loadProgram");

    //makeInterrupt21();
    //loadProgram();

    //makeInterrupt21();
    //executeProgram("syscal", 0x2000);

    //printString("We out here boys");
    //clearScreen();
    //printString("We got deleted boys");

    makeInterrupt21();
    executeProgram("shell", 0x2000);

    while (1)
    {
    }
    return 0;
}

void terminate()
{
    /*Also in your kernel, you should change terminate to no longer hang up. Instead it should reload and execute the shell at segment*/
    executeProgram("shell", 0x2000);
}

void executeProgram(char *name, int segment)
{
    char buffer[13312];
    int i = 0;
    int j = 0;

    /* 1. Call readFile to load the file into a buffer. */
    readFile(name, buffer);

    /*2. In a loop, transfer the file from the buffer into the bottom (0000) of memory at the segment in the parameter*/
    for (i = 0; i < 13312; i++)
    {
        putInMemory(segment, i, buffer[i]);
    }

    /*3. Implement the function void launchProgram(int segment) in assembly*/
    launchProgram(segment);
}

int power(int base, int exp)
{
    int i, p;

    p = 1;

    for (i = 1; i <= exp; i++)
    {
        p = p * base;
    }

    return p;
}

int binToDec(char *fileSectorPointer)
{

    /* Most dumb and inefficient binary to decimal converter ever, BuT iT WoRKs */

    /* Created this function in order to see real integers instead of binary */

    char b = fileSectorPointer[0];
    int i = 0;
    int index = 0;
    int x = 0;
    int y = 0;

    for (i = 7; i >= 0; i--) /* Binary is big endian, soo go in reverse order */
    {
        x = b & 1;
        if (x == 0)
        {
        }
        else
        {
            y += power(2, index);
        }

        b >>= 1;
        index++;
    }

    return y;
}

void loadFileIntoBuffer(char *fileSectorPointers, char *buffer)
{
    int i = 0;
    int sectorNumber = 0;
    //printString("Loading file into buffer now--\n\r");

    sectorNumber = binToDec(fileSectorPointers);

    if (sectorNumber == 0)
    {
        //printString("It appears that you created a file but had literally nothing in it\n");
        return;
    }

    //printString(" \t \t \t \t \t \t #### Dumping file's content: #####\n\r");

    do
    {
        sectorNumber = binToDec(fileSectorPointers);
        readSector(buffer, sectorNumber);

        buffer += 512;        /* Increment the buffer pointer by 512 bytes everytime soo that we can append the data to the buffer */
        fileSectorPointers++; /* Increment (by one byte, incrementing a char pointer [pointer's arithmetics :D]) to get the next byte that points to a sector */

    } while (sectorNumber != 0);

    //printString("\n\r");
}

int compareFileNameStrings(char str1[6], char str2[6])
{

    /* Every file name is 6 bytes long, so iterate 6 times over the character array */

    int i = 0;
    int isEqual = TRUE;

    for (i = 0; i < 6; i++)
    {
        if (str1[i] != str2[i])
        {
            isEqual = FALSE;
            return isEqual;
        }
    }

    return isEqual;
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
    int i = 0; /* Guard variable used to count how many characters the user enters, if the user presses backspace and no characters have been enter, this will protect the buffer variable */
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
            { /*Make sure that i doesnt go below 0 to protect buffer from substracting more than it should */
                i--;
                buffer--;
            }

            printChar(0x8); /* Move backwards */
            printChar(' '); /* Print an empty char */
            printChar(0x8); /* Move backwards again */
        }
    }

    //line[i + 1] = '\0';
    *++buffer = '\0'; /* Set the last char to zero */
}

void readFile(char *fileName, char *buffer)
{
    /* Read the directory sector (that sits in sector 2) */

    char sectorContent[512];

    char *diskFileName;       //This will point to sectorContent. Making this into a pointer to be able to easily move the pointer 32 bytes over the char array
    char *fileSectorPointers; //This will point to the bytes that point to the searched file location

    int sectorNumber = 2;     //The sector in which the directory lives
    int bytesRead = 0;        //Counter variable use to read a 512 bytes sector
    int strsAreEqual = FALSE; //Variable that will store the comparison result of given strings

    readSector(sectorContent, sectorNumber);

    diskFileName = sectorContent;

    for (bytesRead = 0; bytesRead < 512; bytesRead += 32)
    {

        strsAreEqual = compareFileNameStrings(diskFileName, fileName);

        if (strsAreEqual == TRUE)
        {
            fileSectorPointers = diskFileName + 6; //Skip the file name, the rest 26 bytes are (1 byte) pointers that points to searched file location
            loadFileIntoBuffer(fileSectorPointers, buffer);
            return;
        }

        /* If the file that we're looking for is not equal to the 
           file on disk, read the next file in the directory 
           by moving the pointer 32 bytes to get the next one 
        */

        diskFileName += 32;
    }

    //If reached here, stop, return cuz the file wasnt found
    //printString("File: '");
    //printString(fileName);
    //printString("' not found!\n");
    return;
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
    else if (AX == 3)
    {
        readFile(BX, CX);
    }
    else if (AX == 4)
    {
        executeProgram(BX, CX);
    }
    else if (AX == 5)
    {
        terminate();
    }
    else if (AX == 6)
    {
        clearScreen();
    }
    else
    {
        printString("Error on interrupt 21, unknown command on register AX!\0");
    }
}