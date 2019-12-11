#define TRUE 1
#define FALSE 0
#define COMMANDS_COUNT 7
#define BUFFER_CAPACITY 4096
#define NUMBER_OF_SECTORS (BUFFER_CAPACITY / 512)

typedef struct
{
    char *command;
    unsigned int size;
} Command;

typedef struct
{
    char name[6];      /* each file name has a max size of 6 bytes */
    unsigned int size; /* in sectors */
} File;

/* Declaring the api's functions to get intellisense in VsCode */
void syscall_readSector(char *buffer, int sector);
void syscall_printString(char *str);
void syscall_readString(char *str);
void syscall_readFile(char *fileName, char *buffer);
void syscall_executeProgram(char *name, int segment);
void syscall_terminate();
void syscall_clearScreen();
void syscall_deleteFile(char *name);
void syscall_writeSector(char *buffer, int sector);
void syscall_writeFile(char *name, char *buffer, int numberOfSectors);

int checkCommandExistence(char *command);
int isCommandEqual(char *userCommand, char *command, int commandSize);
void executeCommand(char *userCommand);
void clearBuffer(char *buffer, int size);

void readCommandArguments(char *arguments, char *arg1, char *arg2);
void createFileHandler(char *fileName);
int getNumberOfSectors(char *buffer);
int strlen(char *str);
char *int_toString(int integer);

void showDirHandler();

int main()
{
    int commandExists = FALSE;
    char command[200];

    do
    {
        syscall_printString("Luis-SHELL--$ ");
        syscall_readString(command);
        syscall_printString("\n\r");

        commandExists = checkCommandExistence(command);

        if (commandExists == TRUE)
        {
            executeCommand(command);
            syscall_printString("\n\rExecution succesfull\n\r");
            break;
        }

        syscall_printString("Command not found, try again\n\r");

    } while (!commandExists);

    //End the process. Dont, the program should do it
    //syscall_terminate();

    return 0;
}

char *int_toString(int integer)
{
    /* There's really no other way on doing this */

    switch (integer)
    {

    case 0:
        return "0 ";

    case 1:
        return "1 ";

    case 2:
        return "2 ";

    case 3:
        return "3 ";

    case 4:
        return "4 ";

    case 5:
        return "5 ";

    case 6:
        return "6 ";

    case 7:
        return "7 ";

    case 8:
        return "8 ";

    case 9:
        return "9 ";

    case 10:
        return "10";

    case 11:
        return "11";

    case 12:
        return "12";

    case 13:
        return "13";

    case 14:
        return "14";

    case 15:
        return "15";

    case 16:
        return "16";

    case 17:
        return "17";

    case 18:
        return "18";

    case 19:
        return "19";

    case 20:
        return "20";

    case 21:
        return "21";

    case 22:
        return "22";

    case 23:
        return "23";

    case 24:
        return "24";

    case 25:
        return "25";

    case 26:
        return "26";

    case 27:
        return "27";

    case 28:
        return "28";

    case 29:
        return "29";

    case 30:
        return "30";

    case 31:
        return "31";

    case 32:
        return "32";

    case 33:
        return "33";

    default:
        return "WTF";
    }
}

void showDirHandler()
{
    File files[16]; /* The amount of files capable to be in the directory */

    char directory[512];

    char *directory_ptr;
    char *fileSector_ptr;

    int bytesRead = 0;
    int file_index = 0;
    int iterator = 0;

    /* init the files */
    for (iterator = 0; iterator < 16; iterator++)
    {
        files[iterator].name[0] = 'N';
        files[iterator].name[1] = 'U';
        files[iterator].name[2] = 'L';
        files[iterator].name[3] = 'L';
        files[iterator].name[4] = ' ';
        files[iterator].name[5] = ' ';
        files[iterator].size = 0;
    }

    iterator = 0;

    /* Read the directory from disk */
    syscall_readSector(directory, 2);

    directory_ptr = directory;

    for (bytesRead = 0; bytesRead < 512; bytesRead += 32, file_index++)
    {
        /* if the first byte of the current file entry is 0x00, then continue */

        /* Get and save the file name */
        for (iterator = 0; iterator < 6; iterator++)
        {
            if (directory_ptr[iterator] == 0x00)
            {
                if (iterator != 0) /* If the file had a name less than six bytes, then take out the default value that the file has */
                {
                    for (; iterator < 6; iterator++)
                        files[file_index].name[iterator] = ' ';
                }

                break;
            }

            files[file_index].name[iterator] = directory_ptr[iterator];
        }

        fileSector_ptr = directory_ptr + 6;

        /* Get the amount of sectors by reading the rest of the file bytes */
        for (iterator = 0; iterator < 26; iterator++)
        {
            if (fileSector_ptr[iterator] == 0x00)
            {
                break;
            }

            files[file_index].size++;
        }

        directory_ptr += 32;
    }

    syscall_printString("\n\n\r          [Showing file system]           \n\n\r");

    /* Print out the files */
    for (iterator = 0; iterator < 16; iterator++)
    {
        syscall_printString(int_toString(iterator));
        syscall_printString(">: ");
        syscall_printString(files[iterator].name);
        syscall_printString(",     size (in sectors): ");
        syscall_printString(int_toString(files[iterator].size));
        syscall_printString("\n\r");
    }
}

int strlen(char *str)
{
    int length = 0;

    while (*str++ != '\0')
        length++;

    return length;
}

int getNumberOfSectors(char *buffer)
{
    int totalBytes = 0;

    while (*buffer != 0x00 && *buffer != '\0')
    {
        totalBytes++;
        buffer++;
    }

    return (totalBytes / 512) + 1; /* Plus 1 cuz division is rounded down */
}

void createFileHandler(char *fileName)
{
    char readLine[512];
    char buffer[13312];

    int lineSize = 0;
    int totalBytes = 0;
    int numberOfSectors = 1;
    int i;

    /*
        The create command should repeatedly prompt you for a line of text until you enter 
        an empty line. It should put each line in a buffer. 
        It should then write this buffer to a file. 
    */

    syscall_printString("\n\n\n\r================ Welcome to Luis Text Editor =========================== \n\r");
    syscall_printString("\n\r >>>>> Enter all the data you want, write an empty line to exit <<<<<<\n\n\r");

    syscall_printString(">: ");
    syscall_readString(readLine);

    lineSize = strlen(readLine);

    /* reading the char at index 1 to check if line is empty because readString concatenates with a '\0' at the end + 1 of any given string */

    if (lineSize == 2 || lineSize == 1)
    {
        syscall_printString("You literally enter nothing! Not creating the file, that would be a waste, sorry!\n\r");
        return;
    }

    while (lineSize != 2)
    {

        for (i = 0; i < lineSize - 1; i++)
        {
            buffer[totalBytes + i] = readLine[i];
        }

        buffer[totalBytes + i] = '\n';
        buffer[totalBytes + i + 1] = '\r';

        totalBytes += lineSize + 1;

        // clearBuffer(readLine, 512);
        syscall_printString("\n>: ");
        syscall_readString(readLine);
        lineSize = strlen(readLine);
    }

    numberOfSectors += (totalBytes / 512);
    syscall_printString("\n\r!!Dumping buffer to disk.....!!\n\r");
    syscall_writeFile(fileName, buffer, numberOfSectors);
}

void readCommandArguments(char *arguments, char *arg1, char *arg2)
{
    int iterator1 = 0;
    int iterator2 = 0;
    int doneReadingFirstArg = FALSE;

    while (*arguments != 0x00 && *arguments != '\0' && *arguments != '\r')
    {

        if (*arguments == ' ')
        {
            arguments++;
            doneReadingFirstArg = TRUE;
            continue;
        }

        if (doneReadingFirstArg == TRUE)
        {
            arg2[iterator2] = *arguments;
            iterator2++;
        }
        else
        {
            arg1[iterator1] = *arguments;
            iterator1++;
        }

        arguments++;
    }
}

void clearBuffer(char *buffer, int size)
{
    int i = 0;

    for (i = 0; i < size; i++)
        buffer[i] = 0x00;
}

void executeCommand(char *userCommand)
{

    Command commands[COMMANDS_COUNT];
    char *fileName;
    char buffer[BUFFER_CAPACITY];
    int numberOfSectors = 1;
    char arg_1[10], arg_2[10];

    commands[0].command = "clear";
    commands[0].size = 5;

    commands[1].command = "type";
    commands[1].size = 4;

    commands[2].command = "execute";
    commands[2].size = 7;

    commands[3].command = "delete";
    commands[3].size = 6;

    commands[4].command = "copy";
    commands[4].size = 4;

    commands[5].command = "dir";
    commands[5].size = 3;

    commands[6].command = "create";
    commands[6].size = 6;

    //buffer[0] = '\0'; //This prevents the buffer from reading old garbage, LIES, this didnt work

    if (isCommandEqual(userCommand, commands[0].command, commands[0].size)) /* Clear cmd */
    {
        syscall_clearScreen();

        /* Sanitizing the stack */
        clearBuffer(buffer, BUFFER_CAPACITY);

        return;
    }

    if (isCommandEqual(userCommand, commands[1].command, commands[1].size)) /* Type cmd */
    {
        fileName = (userCommand + commands[1].size) + 1; /* To get the file name, add the userCommand pointer the size of the command plus 1 to omit the whitespace */

        /* Read and print the file */
        syscall_readFile(fileName, buffer);
        syscall_printString(buffer);

        /* Sanitizing the stack */
        clearBuffer(buffer, BUFFER_CAPACITY);
        clearBuffer(fileName, 6);

        return;
    }

    if (isCommandEqual(userCommand, commands[2].command, commands[2].size)) /* Execute cmd */
    {
        fileName = (userCommand + commands[2].size) + 1; /* To get the file name, add the userCommand pointer the size of the command plus 1 to omit the whitespace */

        syscall_executeProgram(fileName, 0x3000); /* Executing the program in segment 0x3000 (instead of 0x2000) prevents the whole OS from crashing after executing the program */

        /* Sanitizing the stack */
        clearBuffer(buffer, BUFFER_CAPACITY);
        clearBuffer(fileName, 6);

        return;
    }

    if (isCommandEqual(userCommand, commands[3].command, commands[3].size)) /* Delete file */
    {
        fileName = (userCommand + commands[3].size) + 1; /* To get the file name, add the userCommand pointer the size of the command plus 1 to omit the whitespace */

        syscall_deleteFile(fileName);

        /* Sanitizing the stack */
        clearBuffer(buffer, BUFFER_CAPACITY);
        clearBuffer(fileName, 6);

        return;
    }

    if (isCommandEqual(userCommand, commands[4].command, commands[4].size)) /* Copy file */
    {
        fileName = (userCommand + commands[4].size) + 1;

        /*fileName holds the arguments, so, split those arguments */
        readCommandArguments(fileName, arg_1, arg_2);

        /* print stuff */
        syscall_printString("Dumping contents of file: ");
        syscall_printString(arg_1);
        syscall_printString(" ----> To this new file: ");
        syscall_printString(arg_2);
        syscall_printString("\n\r");

        syscall_readFile(arg_1, buffer);

        numberOfSectors = getNumberOfSectors(buffer);

        syscall_writeFile(arg_2, buffer, numberOfSectors);

        return;
    }

    if (isCommandEqual(userCommand, commands[5].command, commands[5].size)) /* Dir */
    {
        showDirHandler();
        return;
    }

    if (isCommandEqual(userCommand, commands[6].command, commands[6].size)) /* Create */
    {
        fileName = (userCommand + commands[6].size) + 1;
        createFileHandler(fileName);
        return;
    }

    syscall_printString("ERROR ON MATCHING THE COMMAND in shell.c function: executeCommand");
    syscall_terminate();
}

int isCommandEqual(char *userCommand, char *command, int commandSize)
{
    int i = 0;

    /* Check to see if the user input command equals to an existing command */

    for (i = 0; i < commandSize; i++)
    {
        if (userCommand[i] != command[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

int checkCommandExistence(char *userCommand)
{
    /* Add here a command with its respective size (without counting the null terminator char)*/

    Command commands[COMMANDS_COUNT];

    int i = 0;
    int commandExists = FALSE;

    commands[0].command = "clear";
    commands[0].size = 5;

    commands[1].command = "type";
    commands[1].size = 4;

    commands[2].command = "execute";
    commands[2].size = 7;

    commands[3].command = "delete";
    commands[3].size = 6;

    commands[4].command = "copy";
    commands[4].size = 4;

    commands[5].command = "dir";
    commands[5].size = 3;

    commands[6].command = "create";
    commands[6].size = 6;

    /* Loop through every command in order of declaration */

    for (i = 0; i < COMMANDS_COUNT; i++)
    {
        commandExists = isCommandEqual(userCommand, commands[i].command, commands[i].size);

        if (commandExists == TRUE)
            return TRUE;
    }

    /* If reached here, it means that no command matches with the user command, so return false */
    return FALSE;
}