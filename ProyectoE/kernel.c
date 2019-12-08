#define TRUE 1
#define FALSE 0
#define PROCESS_TABLE_LENGTH 8

typedef struct
{
    char isActive;
    int stackPointer;
} Process;

void putInMemory(int segment, int address, char character);
void printChar(char ch);
void printString(char *str);
void readFile(char *fileName, char *buffer);
void readString(char *buffer);
void readSector(char *buffer, int sector);
char readChar();
int compareFileNameStrings(char str1_file_from_disk[6], char str2[6]);
void handleInterrupt21(int AX, int BX, int CX, int DX);
void loadFileIntoBuffer(char *fileSectorPointers, char *buffer);
int binToDec(char *fileSectorPointer);
int power(int base, int exp);
void executeProgram(char *name);
void moveByteToMemory(char byte, int segment);
void launchProgram(int segment);
void terminate();
void clearScreen();
void writeSector(char *buffer, int sector);
void deleteFile(char *name);

void clearBuffer(char *buffer, int size);

void writeFile(char *name, char *buffer, int numberOfSectors);
void copyFileName_toDirectory(char *name, char *directory);
int get_and_Mark_FreeSector(char *diskMap);

void makeTimerInterrupt();
void handleTimerInterrupt(int segment, int sp);
void returnFromTimer(int segment, int sp);

void initializeProcessTable();
int getFreeEntry_index();
int getProcess_Segment(int process_index);
void initializeProgram(int segment);
void setKernelDataSegment();
void restoreDataSegment();

Process processTable[PROCESS_TABLE_LENGTH];
int currentProcess = 0;

int main()
{

    initializeProcessTable();
    makeInterrupt21();
    makeTimerInterrupt();
    executeProgram("shell");

    while (1)
    {
    }
    return 0;
}

int getProcess_Segment(int process_index)
{
    return (process_index + 2) * 0x1000;
}

int getFreeEntry_index()
{
    int i = 0;

    for (i = 0; i < PROCESS_TABLE_LENGTH; i++)
    {
        setKernelDataSegment();
        if (processTable[i].isActive == FALSE)
        {
            restoreDataSegment();
            return i;
        }
        restoreDataSegment();
    }

    return i;
}

void initializeProcessTable()
{
    int i = 0;

    /* Go through the table and set active to 0 on all entries, 
       and set the stack pointer values to 0xff00 */

    /* You must call setKernelDataSegment before accessing the process table */
    //setKernelDataSegment();

    for (i = 0; i < PROCESS_TABLE_LENGTH; i++)
    {
        setKernelDataSegment();
        processTable[i].isActive = FALSE;
        processTable[i].stackPointer = 0xff00;
        restoreDataSegment();
    }

    /* Set currentProcess to 0 too. */
    setKernelDataSegment();
    currentProcess = 0;
    restoreDataSegment();
    return;
}

void handleTimerInterrupt(int segment, int sp)
{
    int iterator = 0;
    int was_A_ProcessActive_Found = FALSE;
    int current_process_index = (segment / 0x1000) - 2;

    /* The scheduler should first save the stack pointer (sp) in the current process table entry. */

    processTable[current_process_index].stackPointer = sp;

    /* Then it should look through the process table starting
       at the next entry after currentProcess and choose another
       process to run (it should loop around if it has to). */

    for (iterator = current_process_index + 1; iterator < PROCESS_TABLE_LENGTH; iterator++)
    {
        if (processTable[iterator].isActive == TRUE)
        {
            was_A_ProcessActive_Found = TRUE;

            /* Finally it should set currentProcess to that entry, and
               call returnFromTimer with that entry's segment and stack pointer. */

            currentProcess = iterator;
            segment = getProcess_Segment(iterator);
            sp = processTable[iterator].stackPointer;
        }
    }

    if (was_A_ProcessActive_Found == FALSE)
    {
        for (iterator = 0; iterator < currentProcess; iterator++)
        {
            if (processTable[iterator].isActive == TRUE)
            {
                /* Finally it should set currentProcess to that entry, and
               call returnFromTimer with that entry's segment and stack pointer. */

                currentProcess = iterator;
                segment = getProcess_Segment(iterator);
                sp = processTable[iterator].stackPointer;
            }
        }
    }

    /* If there are no processes at all that are active, 
       the scheduler should just call returnFromTimer with the
       segment and stack pointer it was called with. */
    returnFromTimer(segment, sp);
}

void terminate()
{
    /*Also in your kernel, you should change terminate to no longer hang up. Instead it should reload and execute the shell at segment*/
    //executeProgram("shell");

    /* Project E - Additionally, terminate needs to be changed. Previously you had it reload the shell, but now the shell
       never stops running. terminate's new task is to set the process that called it (currentProcess) to inactive
       (0) and start an infinite while loop. Eventually the timer will go off and the scheduler will choose a new
       process. 
    */

    /* You must call setKernelDataSegment before accessing the process table */
    setKernelDataSegment();
    processTable[currentProcess].isActive = FALSE;

    while (TRUE)
        ;
}

void executeProgram(char *name)
{
    char buffer[13312];
    int i = 0;
    int j = 0;
    int freeProcess_index = 0;
    int process_segment = 0x2000;

    /* 1. Call readFile to load the file into a buffer. */
    readFile(name, buffer);

    /* Check to see if the file exists */
    if (buffer[0] == 'F' && buffer[1] == 'I' && buffer[2] == 'L' && buffer[3] == 'E' && buffer[4] == ' ' && buffer[5] == 'N' && buffer[6] == 'O' && buffer[7] == 'T')
        return;

    /* Project E Part */

    /* In executeProgram, search through the process table for a free entry.
       Set that entry's active number to 1, and call launchProgram 
       on that entry's segment. */

    freeProcess_index = getFreeEntry_index();

    setKernelDataSegment();
    processTable[freeProcess_index].isActive = TRUE;
    restoreDataSegment();
    currentProcess = freeProcess_index;
    process_segment = getProcess_Segment(freeProcess_index);

    /*2. In a loop, transfer the file from the buffer into the bottom (0000) of memory at the segment in the parameter*/
    for (i = 0; i < 13312; i++)
    {
        putInMemory(process_segment, i, buffer[i]);
    }

    /*3. Implement the function void launchProgram(int segment) in assembly*/
    /* Project E - Change your executeProgram function to call initializeProgram instead of launchProgram. */
    //launchProgram(process_segment);
    initializeProgram(process_segment);
}

;
;
;
;
;
;
;

int get_and_Mark_FreeSector(char *diskMap)
{
    int freeSectorPos;

    for (freeSectorPos = 0; freeSectorPos < 512; freeSectorPos++)
    {
        if (diskMap[freeSectorPos] == 0x00)
        {
            diskMap[freeSectorPos] = 0xFF;
            return freeSectorPos;
        }
    }

    return -1;
}

void copyFileName_toDirectory(char *name, char *directory)
{
    int i;

    for (i = 0; i < 6; i++)
    {
        if (name[i] == 0x00 || name[i] == '\0' || name[i] == '\r' || name[i] == 0x0D)
        {
            directory[i] = 0x00;
        }
        else
        {
            directory[i] = name[i];
        }
    }
}

void writeFile(char *name, char *buffer, int numberOfSectors)
{
    char diskMap[512];
    char directory[512];

    int byteIterator;
    int freeSector;
    int i;

    char *directory_ptr;
    char *fileSectors_ptr;
    char *disMap_ptr;

    /* 1. Load the Map and Directory sectors into buffers */
    readSector(diskMap, 1);
    readSector(directory, 2);

    directory_ptr = directory;
    disMap_ptr = diskMap;

    /* 2. Find a free directory entry (one that begins with 0x00) */
    for (byteIterator = 0; byteIterator < 512; byteIterator += 32)
    {
        if (directory[byteIterator] == 0x00)
        {
            fileSectors_ptr = directory_ptr + 6; //Plus six to skip the fileName

            /* 3. Copy the name to that directory entry. If the name is less than 6 bytes, fill in the remaining bytes */
            copyFileName_toDirectory(name, directory_ptr);

            /* 4. For each sector making up the file: */
            for (i = 0; i < 26; i++)
            {
                if (i >= numberOfSectors)
                {
                    /* 9. Fill in the remaining bytes in the directory entry to 0x00 */
                    *(fileSectors_ptr++) = 0x00;
                    continue;
                }

                /* 5. Find a free sector by searching through the Map for a 0x00 */
                /* 6. Set that sector to 0xFF in the Map */
                freeSector = get_and_Mark_FreeSector(diskMap);

                if (freeSector == -1)
                {
                    //If its negative 1, it means that there are no more free sectors available
                    return;
                }

                /* 7. Add that sector number to the file's directory entry */
                *(fileSectors_ptr) = freeSector; //Check this (check to see whether i can just write an int directly)

                /* 8. Write 512 bytes from the buffer holding the file to that sector */
                writeSector(buffer, freeSector);

                buffer += 512;
                fileSectors_ptr++;
            }

            /* 10. Write the Map and Directory sectors back to the disk */
            writeSector(diskMap, 1);
            writeSector(directory, 2);
            return;
        }

        directory_ptr += 32;
    }
}

void clearBuffer(char *buffer, int size)
{
    int i = 0;

    for (i = 0; i < size; i++)
        *(buffer + i) = 0x00;
}

void deleteFile(char *name)
{
    char diskMap[512];
    char directory[512];

    char *directory_ptr;      /* This will point to directory. Making this into a pointer to be able to easily move the pointer 32 bytes over the char array */
    char *fileSectors_ptr;    /* This will point to the bytes that point to the searched file location */
    int bytesRead = 0;        /* Counter variable use to read a 512 bytes sector */
    int strsAreEqual = FALSE; /* Variable that will store the comparison result of given strings */

    int i = 0;
    int sectorNumber = 0;

    /* 1. Load the Directory and Map to 512 byte character arrays */
    readSector(diskMap, 1);
    readSector(directory, 2);

    /* 2. Search through the directory and try to find the file name. */

    directory_ptr = directory;

    for (bytesRead = 0; bytesRead < 512; bytesRead += 32)
    {

        strsAreEqual = compareFileNameStrings(directory_ptr, name);

        if (strsAreEqual == TRUE)
        {
            /* 3. Set the first byte of the file name to 0x00. */
            *directory_ptr = 0x00;

            fileSectors_ptr = directory_ptr + 6; //Skip the file name, the rest 26 bytes are (1 byte) pointers that points to searched file location

            /* 4. Step through the sectors numbers listed as belonging to the file. For each sector, set the corresponding Map byte to 0x00. */
            for (sectorNumber = binToDec(fileSectors_ptr); sectorNumber != 0; sectorNumber = binToDec(++fileSectors_ptr))
            {
                //
                diskMap[sectorNumber] = 0x00;
            }

            //Fuck all the 32 bytes, not just the first byte - THIS WORKED, This prevent the file to be preserved in the stack(?), preventing the OS from reading a deleted file (even a file deleted from disk)
            clearBuffer(directory_ptr, 12);

            /* 5. Write the character arrays holding the Directory and Map back to their appropriate sectors. */
            writeSector(diskMap, 1);
            writeSector(directory, 2);

            return;
        }

        /* If the file that we're looking for is not equal to the 
           file on disk, read the next file in the directory 
           by moving the pointer 32 bytes to get the next one 
        */

        directory_ptr += 32;
    }
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

    /* Most dumb and inefficient binary to decimal converter ever, BuT iT WoRKs AnD I mAdE iT*/

    /* Created this function in order to see real integers instead of binary */

    char b = fileSectorPointer[0];
    int i = 0;
    int index = 0;
    int x = 0;
    int y = 0;

    for (i = 7; i >= 0; i--) /* Binary is big endian (its not), soo go in reverse order */
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

    sectorNumber = binToDec(fileSectorPointers);

    if (sectorNumber == 0)
    {
        //printString("It appears that you created a file but had literally nothing in it\n");
        return;
    }

    do
    {
        sectorNumber = binToDec(fileSectorPointers);
        readSector(buffer, sectorNumber);

        buffer += 512;        /* Increment the buffer pointer by 512 bytes everytime soo that we can append the data to the buffer */
        fileSectorPointers++; /* Increment (by one byte, incrementing a char pointer [pointer's arithmetics :D]) to get the next byte that points to a sector */

    } while (sectorNumber != 0);
}

int compareFileNameStrings(char str1_file_from_disk[6], char str2[6])
{

    /* Every file name is 6 bytes long (OR LESS), so iterate 6 times (OR LESS) over the character arrays */

    int i = 0;
    int isEqual = TRUE;

    for (i = 0; i < 6 && str1_file_from_disk[i] != 0x00; i++)
    {
        if (str1_file_from_disk[i] != str2[i])
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

    char directory[512];

    char *directory_ptr;   /* This will point to directory array. Making this into a pointer to be able to easily move the pointer 32 bytes over the char array */
    char *fileSectors_ptr; /* This will point to the bytes that point to the searched file location */

    int sectorNumber = 2;     /* The sector in which the directory lives */
    int bytesRead = 0;        /* Counter variable use to read a 512 bytes sector */
    int strsAreEqual = FALSE; /* Variable that will store the comparison result of given strings */

    //char *tmpBuffer = buffer; //Variable not neccesary anymore

    *buffer = 'F'; //Set the buffer to an initial state of not file not found
    *(buffer + 1) = 'I';
    *(buffer + 2) = 'L';
    *(buffer + 3) = 'E';
    *(buffer + 4) = ' ';
    *(buffer + 5) = 'N';
    *(buffer + 6) = 'O';
    *(buffer + 7) = 'T';
    *(buffer + 8) = ' ';
    *(buffer + 9) = 'F';
    *(buffer + 10) = 'O';
    *(buffer + 11) = 'U';
    *(buffer + 12) = 'N';
    *(buffer + 13) = 'D';
    *(buffer + 14) = '\0';

    readSector(directory, sectorNumber);

    directory_ptr = directory;

    for (bytesRead = 0; bytesRead < 512; bytesRead += 32)
    {

        strsAreEqual = compareFileNameStrings(directory_ptr, fileName);

        if (strsAreEqual == TRUE)
        {
            //buffer = tmpBuffer;
            fileSectors_ptr = directory_ptr + 6; //Skip the file name, the rest 26 bytes are (1 byte) pointers that points to searched file location
            loadFileIntoBuffer(fileSectors_ptr, buffer);
            return;
        }

        /* If the file that we're looking for is not equal to the 
           file on disk, read the next file in the directory 
           by moving the pointer 32 bytes to get the next one 
        */

        directory_ptr += 32;
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
        executeProgram(BX);
    }
    else if (AX == 5)
    {
        terminate();
    }
    else if (AX == 6)
    {
        writeSector(BX, CX);
    }
    else if (AX == 7)
    {
        deleteFile(BX);
    }
    else if (AX == 8)
    {
        writeFile(BX, CX, DX);
    }
    else if (AX == 9)
    {
        clearScreen();
    }
    else
    {
        printString("Error on interrupt 21, unknown command on register AX!\0");
    }
}