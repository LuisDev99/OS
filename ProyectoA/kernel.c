putInMemory(int segment, int address, char character);

///Function that receives a string and returns the length of the string
///@parameter
///char string[]:  the string that will return the length
int getStrLength(char string[]);

///Function that receives a string and displays it on screen in the specified address
///@parameters
///int address: the address in video memory
///char msg[]:  the string to be display on the screen
void printLine(int address, char msg[]);

int main()
{

    //printLine(0x8140, "hey whatup, will this work?", 5);
    // putInMemory(0xB000, 0x8140, 'L');
    // putInMemory(0xB000, 0x8141, 0x7);
    // putInMemory(0xB000, 0x8142, 'U');
    // putInMemory(0xB000, 0x8143, 0x7);
    // putInMemory(0xB000, 0x8144, 'I');
    // putInMemory(0xB000, 0x8145, 0x7);

    //32 -> cantidad inicial de asteriscos a la izq

    // char array[25][25];
    // int i;
    // char str;

    //array[1] = "              UNIVERSIDAD TECNOLOGICA CENTROAMERICANA (UNITEC)                 ";
    //array[2] = "                          21711234 - Luis Gerardo Ponce Rodriguez 2019         ";
    //array[3]  = "                          MI PRIMER SISTEMA OPERATIVO                          ";
    //array[4]  = "-------------------------------------------------------------------------------";
    //array[5]  = "******************************** Saluta mondu !! ******************************";
    //array[6]  = "******************************** Pozdrav svijetu !! ***************************";
    //array[7]  = "******************************** Ahoj svet !! *********************************";
    //array[8]  = "******************************** Hej verden !! ********************************";
    //array[9]  = "******************************** Hallo wereld !! ******************************";
    //array[10] = "******************************** Hola Mundo !! ********************************";
    //array[11] = "******************************** Hello World !! *******************************";
    //array[12] = "******************************** Hola Mon !! **********************************";
    //array[13] = "******************************** Kumusta kalibutan !! *************************";
    //array[14] = "******************************** Moni mdziko !! *******************************";
    //array[15] = "******************************** Saluton mondo !! *****************************";
    //array[16] = "******************************** Tere maailm !! *******************************";
    //array[17] = "******************************** Kumusta mundo !! *****************************";
    //array[18] = "******************************** Hei maailma !! *******************************";
    //array[19] = "******************************** Salut monde !! *******************************";
    //array[20] = "******************************** Hallo wrald !! *******************************";
    //array[21] = "******************************** Ola mundo !! *********************************";
    //array[22] = "******************************** Hallo Welt !! ********************************";
    //array[23] = "******************************** Bonjou mond lan !! ***************************";
    //array[24] = "******************************** Sannu duniya !! ******************************";

    printLine(0x8000, "                              SISTEMAS OPERATIVOS 1                             ");
    printLine(0x80A0, "                      21711234 - Luis Gerardo Ponce Rodriguez 2019              ");
    printLine(0x8140, "                          MI PRIMER SISTEMA OPERATIVO                           ");
    printLine(0x81E0, "                    UNIVERSIDAD TECNOLOGICA CENTROAMERICANA UNITEC              ");
    printLine(0x8280, "-------------------------------------------------------------------------------*");
    printLine(0x8320, "******************************** Saluta mondu !! *******************************");
    printLine(0x83C0, "******************************** Pozdrav svijetu !! ****************************");
    printLine(0x8460, "******************************** Ahoj svet !! **********************************");
    printLine(0x8500, "******************************** Hej verden !! *********************************");
    printLine(0x85A0, "******************************** Hallo wereld !! *******************************");
    printLine(0x8640, "******************************** Hola Mundo !! *********************************");
    printLine(0x86E0, "******************************** Hello World !! ********************************");
    printLine(0x8780, "******************************** Hola Mon !! ***********************************");
    printLine(0x8820, "******************************** Kumusta kalibutan !! **************************");
    printLine(0x88C0, "******************************** Moni mdziko !! ********************************");
    printLine(0x8960, "******************************** Saluton mondo !! ******************************");
    printLine(0x8A00, "******************************** Tere maailm !! ********************************");
    printLine(0x8AA0, "******************************** Kumusta mundo !! ******************************");
    printLine(0x8B40, "******************************** Hei maailma !! ********************************");
    printLine(0x8BE0, "******************************** Salut monde !! ********************************");
    printLine(0x8C80, "******************************** Hallo wrald !! ********************************");
    printLine(0x8D20, "******************************** Ola mundo !! **********************************");
    printLine(0x8DC0, "******************************** Hallo Welt !! *********************************");
    printLine(0x8E60, "******************************** Bonjou mond lan !! ****************************");
    printLine(0x8F00, "******************************** Sannu duniya !! *******************************");

    return 0;
}

int getStrLength(char string[])
{
    int i = 0;

    while (string[i] != '\0')
        ++i;

    return i;
}

void printLine(int address, char msg[])
{
    int i, length;
    char charity;
    static int color = 0x7;

    length = getStrLength(msg);

    for (i = 0; i < 80 && i < length; i++)
    {
        charity = msg[i];
        putInMemory(0xB000, address, charity);
        putInMemory(0xB000, address + 1, color);
        address = address + 2;
    }
    color += 6;
}

//Command to execute bochs: $ echo c | bochs -f ./bochsrc-2.6.9.bxrc