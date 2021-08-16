#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>

/*
Instructions in 8080 assembly use seven types of parameters:
NO_PARAM (no parameter), example: NOP
S_REG (standalone register), example: INR A
REG_8BIT (8-bit value to register), example: MVI B,D8
REG_16BIT (16-bit value or address to register), example: LXI D,D16
S_8BIT (standalone 8-bit value), example: OUT D8
S_16BIT (standalone 16-bit value or register), example: JP adr
*/

typedef enum {
    NO_PARAM,
    S_REG,
    REG_8BIT,
    REG_16BIT,
    REG_REG,
    S_8BIT,
    S_16BIT
} InstParam;

// void fillBuffer(FILE *input, int8_t buffer, int size);
uint8_t *fillBuffer(FILE *input, int size);
int findSize(FILE *input);
int printInstruction(uint8_t *buffer, int location, int instSize, char *instName, char *reg1, char *reg2, InstParam parameter);
void readBuffer(uint8_t *buffer, int size);

int main(int argc, char *argv[])
{
    FILE *source;
    int fileSize;
    if(argv[1])
    {
        source = fopen(argv[1],"r");
        if(source == NULL)
        {
            // no such file or directory
            fprintf(stderr,"%s\n",strerror(2));
            exit(2);
        }
    }
    else
    {
        // invalid argument
        fprintf(stderr,"%s\n",strerror(22));
        exit(22);
    }

    fileSize = findSize(source);
    uint8_t *fileBuffer = fillBuffer(source, fileSize);
    readBuffer(fileBuffer, fileSize);

    fclose(source);
    return(0);
}

uint8_t *fillBuffer(FILE *input, int size)
{
    uint8_t *buffer;

    buffer = (uint8_t *)malloc(size);
    if(buffer == NULL)
    {
        fprintf(stderr,"Out of memory!\n");
        exit(99);
    }
    fread(buffer,sizeof(uint8_t),size,input);
    return buffer;
}
int findSize(FILE *input)
{
    int size;

    fseek(input, 0L, SEEK_END);
    size = ftell(input)*sizeof(int);
    rewind(input);
    return size;
}
// printInstruction takes pointer to current location in file, integer of current location in file, integer of size of instruction in bytes, string of name of instruction, strings of registers or number parameters of instruction, and the type of parameter as defined above
// printInstruction returns current value of location looping variable before next increment
int printInstruction(uint8_t *buffer, int location, int instSize, char *instName, char *reg1, char *reg2, InstParam parameter)
{
    int i = 0;
    printf("%04x ", location); // Print current location in file
    for(; i < instSize; i++) // Print bytes of instruction
    {
        printf("%02x ", buffer[i]);
    }
    for(i = 0; i < (3 - instSize)*3; i++) // Print additional padding spaces if instruction is 1 or 2 bytes
    {
        putchar(' ');
    }
    printf("%s", instName); // Print instruction name
    if(strlen(instName) < 4) // Print additional padding spaces if instruction name is less than 4 characters
    {
        for(i = 0; i < (4 - strlen(instName)); i++)
        {
            putchar(' ');
        }
    }
    if(parameter != NO_PARAM)
    {
        printf("   "); // If instruction has parameter, print minimum number of spaces between instruction name and instruction parameter
    }
    switch(parameter) // Handle all parameter cases
    {
    case NO_PARAM: // Print nothing for no parameter
        break;
    case S_REG:
        printf("%s", reg1); // Print register string
        break;
    case REG_8BIT:
        printf("%s,$%02x", reg1, buffer[1]); // Print register string and 8-bit immediate value
        location++;
        buffer++;
        break;
    case REG_16BIT:
        printf("%s,$%02x02x", reg1, buffer[2], buffer[1]); // Print register string and 16-bit little endian immediate value
        location += 2;
        buffer += 2;
        break;
    case REG_REG:
        printf("%s,%s", reg1, reg2); // Print both register strings
        break;
    case S_8BIT:
        printf("$%02x", buffer[1]); // Print 8-bit immediate value
        location++;
        buffer++;
    case S_16BIT:
        printf("$%02x%02x", buffer[2], buffer[1]); //Print little endian 16-bit immediate value
        location += 2;
        buffer += 2;
    }
    putchar('\n');
    return(location);
}
void readBuffer(uint8_t *buffer, int size)
{
    static uint8_t incIns1 = 0;
    static int isIncIns1 = 0;
    static uint8_t incIns2 [2] = {0, 0};
    static int isIncIns2 = 0;
    char inst[5];
    int i = 0;

    for(; i < size; i++, buffer++)
    {
        switch(*buffer)
        {
        case 0x00: // NOP
            i = printInstruction(buffer, i, 1, "NOP", "", "", NO_PARAM);
            break;
        default: // Undefined instructions
            i = printInstruction(buffer, i, 1, "--", "", "", NO_PARAM);
        }
    }
}
