#define TRUE 1
#define FALSE 0
#define COMMANDS_COUNT 3

typedef struct
{
    char *command;
    unsigned int size;
} Command;

/* Defining the api's functions to get intellisense in VsCode */
void syscall_readSector(char *buffer, int sector);
void syscall_printString(char *str);
void syscall_readString(char *str);
void syscall_readFile(char *fileName, char *buffer);
void syscall_executeProgram(char *name, int segment);
void syscall_terminate();
void syscall_clearScreen();

int checkCommandExistence(char *command);
int isCommandEqual(char *userCommand, char *command, int commandSize);
void executeCommand(char *userCommand);

int main()
{
    int commandExists = FALSE;
    char command[200];

    do
    {
        syscall_printString("Luis-SHELL~$ ");
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

    //End the process
    //syscall_terminate();
}

void executeCommand(char *userCommand)
{

    Command commands[COMMANDS_COUNT];
    char *fileName;
    char buffer[13312];

    commands[0].command = "clear";
    commands[0].size = 5;

    commands[1].command = "type";
    commands[1].size = 4;

    commands[2].command = "execute";
    commands[2].size = 7;

    if (isCommandEqual(userCommand, commands[0].command, commands[0].size)) //Clear
    {
        syscall_clearScreen();
        return;
    }

    if (isCommandEqual(userCommand, commands[1].command, commands[1].size)) //Type
    {
        fileName = (userCommand + commands[1].size) + 1; //To get the file name, add the userCommand pointer the size of the command plus 1 to omit the whitespace

        syscall_readFile(fileName, buffer);
        syscall_printString(buffer);
        return;
    }

    if (isCommandEqual(userCommand, commands[2].command, commands[2].size)) //Execute
    {
        fileName = (userCommand + commands[2].size) + 1; //To get the file name, add the userCommand pointer the size of the command plus 1 to omit the whitespace

        syscall_executeProgram(fileName, 0x2000);
        return;
    }

    syscall_printString("ERROR ON MATCHING THE COMMAND in shell.c function: executeCommand");
    syscall_terminate();
}

int isCommandEqual(char *userCommand, char *command, int commandSize)
{
    int i = 0;

    /* Check to see if the user input command equals to the given command */

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