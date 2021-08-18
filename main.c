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
    if(parameter != NO_PARAM) // Print extra spaces if instruction has parameter
    {
        if(strlen(instName) < 4) // Print additional padding spaces if instruction name is less than 4 characters
        {
            for(i = 0; i < (4 - strlen(instName)); i++)
            {
                putchar(' ');
            }
        }
        printf("   "); // Print minimum number of spaces between instruction name and instruction parameter
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
        printf("%s,$%02x%02x", reg1, buffer[2], buffer[1]); // Print register string and 16-bit little endian immediate value
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
        break;
    case S_16BIT:
        printf("$%02x%02x", buffer[2], buffer[1]); //Print little endian 16-bit immediate value
        location += 2;
        buffer += 2;
        break;
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
        case 0x01: // LXI B,D16
            i = printInstruction(buffer, i, 3, "LXI", "B", "", REG_16BIT);
            break;
        case 0x02: // STAX B
            i = printInstruction(buffer, i, 1, "STAX", "B", "", S_REG);
            break;
        case 0x03: // INX B
            i = printInstruction(buffer, i, 1, "INX", "B", "", S_REG);
            break;
        case 0x04: // INR B
            i = printInstruction(buffer, i, 1, "INR", "B", "", S_REG);
            break;
        case 0x05: // DCR B
            i = printInstruction(buffer, i, 1, "DCR", "B", "", S_REG);
            break;
        case 0x06: // MVI B,D8
            i = printInstruction(buffer, i, 2, "MVI", "B", "", REG_8BIT);
            break;
        case 0x07: // RLC
            i = printInstruction(buffer, i, 1, "RLC", "", "", NO_PARAM);
            break;
        case 0x09: // DAD B
            i = printInstruction(buffer, i, 1, "DAD", "B", "", S_REG);
            break;
        case 0x0a: // LDAX B
            i = printInstruction(buffer, i, 1, "LDAX", "B", "", S_REG);
            break;
        case 0x0b: // DCX B
            i = printInstruction(buffer, i, 1, "DCX", "B", "", S_REG);
            break;
        case 0x0c: // INR C
            i = printInstruction(buffer, i, 1, "INR", "C", "", S_REG);
            break;
        case 0x0d: // DCR C
            i = printInstruction(buffer, i, 1, "DCR", "C", "", S_REG);
            break;
        case 0x0e: // MVI C,D8
            i = printInstruction(buffer, i, 2, "MVI", "C", "", REG_8BIT);
            break;
        case 0x0f: // RRC
            i = printInstruction(buffer, i, 1, "RRC", "", "", NO_PARAM);
            break;
        case 0x11: // LXI D,D16
            i = printInstruction(buffer, i, 3, "LXI", "D", "", REG_16BIT);
            break;
        case 0x12: // STAX D
            i = printInstruction(buffer, i, 1, "STAX", "D", "", S_REG);
            break;
        case 0x13: // INX D
            i = printInstruction(buffer, i, 1, "INX", "D", "", S_REG);
            break;
        case 0x14: // INR D
            i = printInstruction(buffer, i, 1, "INR", "D", "", S_REG);
            break;
        case 0x15: // DCR D
            i = printInstruction(buffer, i, 1, "DCR", "D", "", S_REG);
            break;
        case 0x16: // MVI D,D8
            i = printInstruction(buffer, i, 2, "MVI", "D", "", REG_8BIT);
            break;
        case 0x17: // RAL
            i = printInstruction(buffer, i, 1, "RAL", "", "", NO_PARAM);
            break;
        case 0x19: // DAD D
            i = printInstruction(buffer, i, 1, "DAD", "D", "", S_REG);
            break;
        case 0x1a: // LDAX D
            i = printInstruction(buffer, i, 1, "LDAX", "D", "", S_REG);
            break;
        case 0x1b: // DCX D
            i = printInstruction(buffer, i, 1, "DCX", "D", "", S_REG);
            break;
        case 0x1c: // INR E
            i = printInstruction(buffer, i, 1, "INR", "E", "", S_REG);
            break;
        case 0x1d: // DCR E
            i = printInstruction(buffer, i, 1, "DCR", "E", "", S_REG);
            break;
        case 0x1e: // MVI E,D8
            i = printInstruction(buffer, i, 2, "MVI", "E", "", REG_8BIT);
            break;
        case 0x1f: // RAR
            i = printInstruction(buffer, i, 1, "RAR", "", "", NO_PARAM);
            break;
        case 0x21: // LXI H,D16
            i = printInstruction(buffer, i, 3, "LXI", "H", "", REG_16BIT);
            break;
        case 0x22: // SHLD adr
            i = printInstruction(buffer, i, 3, "SHLD", "", "", S_16BIT);
            break;
        case 0x23: // INX H
            i = printInstruction(buffer, i, 1, "INX", "H", "", S_REG);
            break;
        case 0x24: // INR H
            i = printInstruction(buffer, i, 1, "INX", "H", "", S_REG);
            break;
        case 0x25: // DCR H
            i = printInstruction(buffer, i, 1, "DCR", "H", "", S_REG);
            break;
        case 0x26: // MVI H,D8
            i = printInstruction(buffer, i, 2, "MVI", "H", "", REG_8BIT);
            break;
        case 0x27: // DAA
            i = printInstruction(buffer, i, 1, "DAA", "", "", NO_PARAM);
            break;
        case 0x29: // DAD H
            i = printInstruction(buffer, i, 1, "DAD", "H", "", S_REG);
            break;
        case 0x2a: // LHLD adr
            i = printInstruction(buffer, i, 3, "LHLD", "", "", S_16BIT);
            break;
        case 0x2b: // DCX H
            i = printInstruction(buffer, i, 1, "DCX", "H", "", S_REG);
            break;
        case 0x2c: // INR L
            i = printInstruction(buffer, i, 1, "INR", "L", "", S_REG);
            break;
        case 0x2d: // DCR L
            i = printInstruction(buffer, i, 1, "DCR", "L", "", S_REG);
            break;
        case 0x2e: // MVI L,D8
            i = printInstruction(buffer, i, 2, "MVI", "L", "", REG_8BIT);
            break;
        case 0x2f: // CMA
            i = printInstruction(buffer, i, 1, "CMA", "", "", NO_PARAM);
            break;
        case 0x31: // LXI SP,D16
            i = printInstruction(buffer, i, 3, "LXI", "SP", "", REG_16BIT);
            break;
        case 0x32: // STA adr
            i = printInstruction(buffer, i, 3, "STA", "", "", S_16BIT);
            break;
        case 0x33: // INX SP
            i = printInstruction(buffer, i, 1, "INX", "SP", "", S_REG);
            break;
        case 0x34: // INR M
            i = printInstruction(buffer, i, 1, "INR", "M", "", S_REG);
            break;
        case 0x35: // DCR M
            i = printInstruction(buffer, i, 1, "DCR", "M", "", S_REG);
            break;
        case 0x36: // MVI M,D8
            i = printInstruction(buffer, i, 2, "MVI", "M", "", REG_8BIT);
            break;
        case 0x37: // STC
            i = printInstruction(buffer, i, 1, "STC", "", "", NO_PARAM);
            break;
        case 0x39: // DAD SP
            i = printInstruction(buffer, i, 1, "DAD", "SP", "", S_REG);
            break;
        case 0x3a: // LDA adr
            i = printInstruction(buffer, i, 3, "LDA", "", "", S_16BIT);
            break;
        case 0x3b: // DCX SP
            i = printInstruction(buffer, i, 1, "DCX", "SP", "", S_REG);
            break;
        case 0x3c: // INR A
            i = printInstruction(buffer, i, 1, "INR", "A", "", S_REG);
            break;
        case 0x3d: // DCR A
            i = printInstruction(buffer, i, 1, "DCR", "A", "", S_REG);
            break;
        case 0x3e: // MVI A,D8
            i = printInstruction(buffer, i, 2, "MVI", "A", "", REG_8BIT);
            break;
        case 0x3f: // CMC
            i = printInstruction(buffer, i, 1, "CMC", "", "", NO_PARAM);
            break;
        case 0x40: // MOV B,B
            i = printInstruction(buffer, i, 1, "MOV", "B", "B", REG_REG);
            break;
        case 0x41: // MOV B,C
            i = printInstruction(buffer, i, 1, "MOV", "B", "C", REG_REG);
            break;
        case 0x42: // MOV B,D
            i = printInstruction(buffer, i, 1, "MOV", "B", "D", REG_REG);
            break;
        case 0x43: // MOV B,E
            i = printInstruction(buffer, i, 1, "MOV", "B", "E", REG_REG);
            break;
        case 0x44: // MOV B,H
            i = printInstruction(buffer, i, 1, "MOV", "B", "H", REG_REG);
            break;
        case 0x45: // MOV B,L
            i = printInstruction(buffer, i, 1, "MOV", "B", "L", REG_REG);
            break;
        case 0x46: // MOV B,M
            i = printInstruction(buffer, i, 1, "MOV", "B", "M", REG_REG);
            break;
        case 0x47: // MOV B,A
            i = printInstruction(buffer, i, 1, "MOV", "B", "A", REG_REG);
            break;
        case 0x48: // MOV C,B
            i = printInstruction(buffer, i, 1, "MOV", "C", "B", REG_REG);
            break;
        case 0x49: // MOV C,C
            i = printInstruction(buffer, i, 1, "MOV", "C", "C", REG_REG);
            break;
        case 0x4a: // MOV C,D
            i = printInstruction(buffer, i, 1, "MOV", "C", "D", REG_REG);
            break;
        case 0x4b: // MOV C,E
            i = printInstruction(buffer, i, 1, "MOV", "C", "E", REG_REG);
        break;
        case 0x4c: // MOV C,H
            i = printInstruction(buffer, i, 1, "MOV", "C", "H", REG_REG);
            break;
        case 0x4d: // MOV C,L
            i = printInstruction(buffer, i, 1, "MOV", "C", "L", REG_REG);
            break;
        case 0x4e: // MOV C,M
            i = printInstruction(buffer, i, 1, "MOV", "C", "M", REG_REG);
            break;
        case 0x4f: // MOV C,A
            i = printInstruction(buffer, i, 1, "MOV", "C", "A", REG_REG);
            break;
        case 0x50: // MOV D,B
            i = printInstruction(buffer, i, 1, "MOV", "D", "B", REG_REG);
            break;
        case 0x51: // MOV D,C
            i = printInstruction(buffer, i, 1, "MOV", "D", "C", REG_REG);
            break;
        case 0x52: // MOV D,D
            i = printInstruction(buffer, i, 1, "MOV", "D", "D", REG_REG);
            break;
        case 0x53: // MOV D,E
            i = printInstruction(buffer, i, 1, "MOV", "D", "E", REG_REG);
            break;
        case 0x54: // MOV D,H
            i = printInstruction(buffer, i, 1, "MOV", "D", "H", REG_REG);
            break;
        case 0x55: // MOV D,L
            i = printInstruction(buffer, i, 1, "MOV", "D", "L", REG_REG);
            break;
        case 0x56: // MOV D,M
            i = printInstruction(buffer, i, 1, "MOV", "D", "M", REG_REG);
            break;
        case 0x57: // MOV D,A
            i = printInstruction(buffer, i, 1, "MOV", "D", "A", REG_REG);
            break;
        case 0x58: // MOV E,B
            i = printInstruction(buffer, i, 1, "MOV", "E", "B", REG_REG);
            break;
        case 0x59: // MOV E,C
            i = printInstruction(buffer, i, 1, "MOV", "E", "C", REG_REG);
            break;
        case 0x5a: // MOV E,D
            i = printInstruction(buffer, i, 1, "MOV", "E", "D", REG_REG);
            break;
        case 0x5b: // MOV E,E
            i = printInstruction(buffer, i, 1, "MOV", "E", "E", REG_REG);
            break;
        case 0x5c: // MOV E,H
            i = printInstruction(buffer, i, 1, "MOV", "E", "H", REG_REG);
            break;
        case 0x5d: // MOV E,L
            i = printInstruction(buffer, i, 1, "MOV", "E", "L", REG_REG);
            break;
        case 0x5e: // MOV E,M
            i = printInstruction(buffer, i, 1, "MOV", "E", "M", REG_REG);
            break;
        case 0x5f: // MOV E,A
            i = printInstruction(buffer, i, 1, "MOV", "E", "A", REG_REG);
            break;
        case 0x60: // MOV H,B
            i = printInstruction(buffer, i, 1, "MOV", "H", "B", REG_REG);
            break;
        case 0x61: // MOV H,C
            i = printInstruction(buffer, i, 1, "MOV", "H", "C", REG_REG);
            break;
        case 0x62: // MOV H,D
            i = printInstruction(buffer, i, 1, "MOV", "H", "D", REG_REG);
            break;
        case 0x63: // MOV H,E
            i = printInstruction(buffer, i, 1, "MOV", "H", "E", REG_REG);
            break;
        case 0x64: // MOV H,H
            i = printInstruction(buffer, i, 1, "MOV", "H", "H", REG_REG);
            break;
        case 0x65: // MOV H,L
            i = printInstruction(buffer, i, 1, "MOV", "H", "L", REG_REG);
            break;
        case 0x66: // MOV H,M
            i = printInstruction(buffer, i, 1, "MOV", "H", "M", REG_REG);
            break;
        case 0x67: // MOV H,A
            i = printInstruction(buffer, i, 1, "MOV", "H", "A", REG_REG);
            break;
        case 0x68: // MOV L,B
            i = printInstruction(buffer, i, 1, "MOV", "L", "B", REG_REG);
            break;
        case 0x69: // MOV L,C
            i = printInstruction(buffer, i, 1, "MOV", "L", "C", REG_REG);
            break;
        case 0x6a: // MOV L,D
            i = printInstruction(buffer, i, 1, "MOV", "L", "D", REG_REG);
            break;
        case 0x6b: // MOV L,E
            i = printInstruction(buffer, i, 1, "MOV", "L", "E", REG_REG);
            break;
        case 0x6c: // MOV L,H
            i = printInstruction(buffer, i, 1, "MOV", "L", "H", REG_REG);
            break;
        case 0x6d: // MOV L,L
            i = printInstruction(buffer, i, 1, "MOV", "L", "L", REG_REG);
            break;
        case 0x6e: // MOV L,M
            i = printInstruction(buffer, i, 1, "MOV", "L", "M", REG_REG);
            break;
        case 0x6f: // MOV L,A
            i = printInstruction(buffer, i, 1, "MOV", "L", "A", REG_REG);
            break;
        case 0x70: // MOV M,B
            i = printInstruction(buffer, i, 1, "MOV", "M", "B", REG_REG);
            break;
        case 0x71: // MOV M,C
            i = printInstruction(buffer, i, 1, "MOV", "M", "C", REG_REG);
            break;
        case 0x72: // MOV M,D
            i = printInstruction(buffer, i, 1, "MOV", "M", "D", REG_REG);
            break;
        case 0x73: // MOV M,E
            i = printInstruction(buffer, i, 1, "MOV", "M", "E", REG_REG);
            break;
        case 0x74: // MOV M,H
            i = printInstruction(buffer, i, 1, "MOV", "M", "H", REG_REG);
            break;
        case 0x75: // MOV M,L
            i = printInstruction(buffer, i, 1, "MOV", "M", "L", REG_REG);
            break;
        case 0x76: // HLT
            i = printInstruction(buffer, i, 1, "HLT", "", "", NO_PARAM);
            break;
        case 0x77: // MOV M,A
            i = printInstruction(buffer, i, 1, "MOV", "M", "A", REG_REG);
            break;
        case 0x78: // MOV A,B
            i = printInstruction(buffer, i, 1, "MOV", "A", "B", REG_REG);
            break;
        case 0x79: // MOV A,C
            i = printInstruction(buffer, i, 1, "MOV", "A", "C", REG_REG);
            break;
        case 0x7a: // MOV A,D
            i = printInstruction(buffer, i, 1, "MOV", "A", "D", REG_REG);
            break;
        case 0x7b: // MOV A,E
            i = printInstruction(buffer, i, 1, "MOV", "A", "E", REG_REG);
            break;
        case 0x7c: // MOV A,H
            i = printInstruction(buffer, i, 1, "MOV", "A", "H", REG_REG);
            break;
        case 0x7d: // MOV A,L
            i = printInstruction(buffer, i, 1, "MOV", "A", "L", REG_REG);
            break;
        case 0x7e: // MOV A,M
            i = printInstruction(buffer, i, 1, "MOV", "A", "M", REG_REG);
            break;
        case 0x7f: // MOV A,A
            i = printInstruction(buffer, i, 1, "MOV", "A", "A", REG_REG);
            break;
        case 0x80: // ADD B
            i = printInstruction(buffer, i, 1, "ADD", "B", "", S_REG);
            break;
        case 0x81: // ADD C
            i = printInstruction(buffer, i, 1, "ADD", "C", "", S_REG);
            break;
        case 0x82: // ADD D
            i = printInstruction(buffer, i, 1, "ADD", "D", "", S_REG);
            break;
        case 0x83: // ADD E
            i = printInstruction(buffer, i, 1, "ADD", "E", "", S_REG);
            break;
        case 0x84: // ADD H
            i = printInstruction(buffer, i, 1, "ADD", "H", "", S_REG);
            break;
        case 0x85: // ADD L
            i = printInstruction(buffer, i, 1, "ADD", "L", "", S_REG);
            break;
        case 0x86: // ADD M
            i = printInstruction(buffer, i, 1, "ADD", "M", "", S_REG);
            break;
        case 0x87: // ADD A
            i = printInstruction(buffer, i, 1, "ADD", "A", "", S_REG);
            break;
        case 0x88: // ADC B
            i = printInstruction(buffer, i, 1, "ADC", "B", "", S_REG);
            break;
        case 0x89: // ADC C
            i = printInstruction(buffer, i, 1, "ADC", "C", "", S_REG);
            break;
        case 0x8a: // ADC D
            i = printInstruction(buffer, i, 1, "ADC", "D", "", S_REG);
            break;
        case 0x8b: // ADC E
            i = printInstruction(buffer, i, 1, "ADC", "E", "", S_REG);
            break;
        case 0x8c: // ADC H
            i = printInstruction(buffer, i, 1, "ADC", "H", "", S_REG);
            break;
        case 0x8d: // ADC L
            i = printInstruction(buffer, i, 1, "ADC", "L", "", S_REG);
            break;
        case 0x8e: // ADC M
            i = printInstruction(buffer, i, 1, "ADC", "M", "", S_REG);
            break;
        case 0x8f: // ADC A
            i = printInstruction(buffer, i, 1, "ADC", "A", "", S_REG);
            break;
        case 0x90: // SUB B
            i = printInstruction(buffer, i, 1, "SUB", "B", "", S_REG);
            break;
        case 0x91: // SUB C
            i = printInstruction(buffer, i, 1, "SUB", "C", "", S_REG);
            break;
        case 0x92: // SUB D
            i = printInstruction(buffer, i, 1, "SUB", "D", "", S_REG);
            break;
        case 0x93: // SUB E
            i = printInstruction(buffer, i, 1, "SUB", "E", "", S_REG);
            break;
        case 0x94: // SUB H
            i = printInstruction(buffer, i, 1, "SUB", "H", "", S_REG);
            break;
        case 0x95: // SUB L
            i = printInstruction(buffer, i, 1, "SUB", "L", "", S_REG);
            break;
        case 0x96: // SUB M
            i = printInstruction(buffer, i, 1, "SUB", "M", "", S_REG);
            break;
        case 0x97: // SUB A
            i = printInstruction(buffer, i, 1, "SUB", "A", "", S_REG);
            break;
        case 0x98: // SBB B
            i = printInstruction(buffer, i, 1, "SBB", "B", "", S_REG);
            break;
        case 0x99: // SBB C
            i = printInstruction(buffer, i, 1, "SBB", "C", "", S_REG);
            break;
        case 0x9a: // SBB D
            i = printInstruction(buffer, i, 1, "SBB", "D", "", S_REG);
            break;
        case 0x9b: // SBB E
            i = printInstruction(buffer, i, 1, "SBB", "E", "", S_REG);
            break;
        case 0x9c: // SBB H
            i = printInstruction(buffer, i, 1, "SBB", "H", "", S_REG);
            break;
        case 0x9d: // SBB L
            i = printInstruction(buffer, i, 1, "SBB", "L", "", S_REG);
            break;
        case 0x9e: // SBB M
            i = printInstruction(buffer, i, 1, "SBB", "M", "", S_REG);
            break;
        case 0x9f: // SBB A
            i = printInstruction(buffer, i, 1, "SBB", "A", "", S_REG);
            break;
        case 0xa0: // ANA B
            i = printInstruction(buffer, i, 1, "ANA", "B", "", S_REG);
            break;
        case 0xa1: // ANA C
            i = printInstruction(buffer, i, 1, "ANA", "C", "", S_REG);
            break;
        case 0xa2: // ANA D
            i = printInstruction(buffer, i, 1, "ANA", "D", "", S_REG);
            break;
        case 0xa3: // ANA E
            i = printInstruction(buffer, i, 1, "ANA", "E", "", S_REG);
            break;
        case 0xa4: // ANA H
            i = printInstruction(buffer, i, 1, "ANA", "H", "", S_REG);
            break;
        case 0xa5: // ANA L
            i = printInstruction(buffer, i, 1, "ANA", "L", "", S_REG);
            break;
        case 0xa6: // ANA M
            i = printInstruction(buffer, i, 1, "ANA", "M", "", S_REG);
            break;
        case 0xa7: // ANA A
            i = printInstruction(buffer, i, 1, "ANA", "A", "", S_REG);
            break;
        case 0xa8: // XRA B
            i = printInstruction(buffer, i, 1, "XRA", "B", "", S_REG);
            break;
        case 0xa9: // XRA C
            i = printInstruction(buffer, i, 1, "XRA", "C", "", S_REG);
            break;
        case 0xaa: // XRA D
            i = printInstruction(buffer, i, 1, "XRA", "D", "", S_REG);
            break;
        case 0xab: // XRA E
            i = printInstruction(buffer, i, 1, "XRA", "E", "", S_REG);
            break;
        case 0xac: // XRA H
            i = printInstruction(buffer, i, 1, "XRA", "H", "", S_REG);
            break;
        case 0xad: // XRA L
            i = printInstruction(buffer, i, 1, "XRA", "L", "", S_REG);
            break;
        case 0xae: // XRA M
            i = printInstruction(buffer, i, 1, "XRA", "M", "", S_REG);
            break;
        case 0xaf: // XRA A
            i = printInstruction(buffer, i, 1, "XRA", "A", "", S_REG);
            break;
        case 0xb0: // ORA B
            i = printInstruction(buffer, i, 1, "ORA", "B", "", S_REG);
            break;
        case 0xb1: // ORA C
            i = printInstruction(buffer, i, 1, "ORA", "C", "", S_REG);
            break;
        case 0xb2: // ORA D
            i = printInstruction(buffer, i, 1, "ORA", "D", "", S_REG);
            break;
        case 0xb3: // ORA E
            i = printInstruction(buffer, i, 1, "ORA", "E", "", S_REG);
            break;
        case 0xb4: // ORA H
            i = printInstruction(buffer, i, 1, "ORA", "H", "", S_REG);
            break;
        case 0xb5: // ORA L
            i = printInstruction(buffer, i, 1, "ORA", "L", "", S_REG);
            break;
        case 0xb6: // ORA M
            i = printInstruction(buffer, i, 1, "ORA", "M", "", S_REG);
            break;
        case 0xb7: // ORA A
            i = printInstruction(buffer, i, 1, "ORA", "A", "", S_REG);
            break;
        case 0xb8: // CMP B
            i = printInstruction(buffer, i, 1, "CMP", "B", "", S_REG);
            break;
        case 0xb9: // CMP C
            i = printInstruction(buffer, i, 1, "CMP", "C", "", S_REG);
            break;
        case 0xba: // CMP D
            i = printInstruction(buffer, i, 1, "CMP", "D", "", S_REG);
            break;
        case 0xbb: // CMP E
            i = printInstruction(buffer, i, 1, "CMP", "E", "", S_REG);
            break;
        case 0xbc: // CMP H
            i = printInstruction(buffer, i, 1, "CMP", "H", "", S_REG);
            break;
        case 0xbd: // CMP L
            i = printInstruction(buffer, i, 1, "CMP", "L", "", S_REG);
            break;
        case 0xbe: // CMP M
            i = printInstruction(buffer, i, 1, "CMP", "M", "", S_REG);
            break;
        case 0xbf: // CMP A
            i = printInstruction(buffer, i, 1, "CMP", "A", "", S_REG);
            break;
        case 0xc0: // RNZ
            i = printInstruction(buffer, i, 1, "RNZ", "", "", NO_PARAM);
            break;
        case 0xc1: // POP B
            i = printInstruction(buffer, i, 1, "POP", "B", "", S_REG);
            break;
        case 0xc2: // JNZ adr
            i = printInstruction(buffer, i, 3, "JNZ", "", "", S_16BIT);
            break;
        case 0xc3: // JMP adr
            i = printInstruction(buffer, i, 3, "JMP", "", "", S_16BIT);
            break;
        case 0xc4: // CNZ adr
            i = printInstruction(buffer, i, 3, "CNZ", "", "", S_16BIT);
            break;
        case 0xc5: // PUSH B
            i = printInstruction(buffer, i, 1, "PUSH", "B", "", S_REG);
            break;
        case 0xc6: // ADI D8
            i = printInstruction(buffer, i, 2, "ADI", "", "", S_8BIT);
            break;
        case 0xc7: // RST 0
            i = printInstruction(buffer, i, 1, "RST", "0", "", S_REG);
            break;
        case 0xc8: // RZ
            i = printInstruction(buffer, i, 1, "RZ", "", "", NO_PARAM);
            break;
        case 0xc9: // RET
            i = printInstruction(buffer, i, 1, "RET", "", "", NO_PARAM);
            break;
        case 0xca: // JZ adr
            i = printInstruction(buffer, i, 3, "JZ", "", "", S_16BIT);
            break;
        case 0xcc: // CZ adr
            i = printInstruction(buffer, i, 3, "CZ", "", "", S_16BIT);
            break;
        case 0xcd: // CALL adr
            i = printInstruction(buffer, i, 3, "CALL", "", "", S_16BIT);
            break;
        case 0xce: // ACI D8
            i = printInstruction(buffer, i, 2, "ACI", "", "", S_8BIT);
            break;
        case 0xcf: // RST 1
            i = printInstruction(buffer, i, 1, "RST", "", "", S_REG);
            break;
        case 0xd0: // RNC
            i = printInstruction(buffer, i, 1, "RNC", "", "", NO_PARAM);
            break;
        case 0xd1: // POP D
            i = printInstruction(buffer, i, 1, "POP", "D", "", S_REG);
            break;
        case 0xd2: // JNC adr
            i = printInstruction(buffer, i, 3, "JNC", "", "", S_16BIT);
            break;
        case 0xd3: // OUT D8
            i = printInstruction(buffer, i, 2, "OUT", "", "", S_8BIT);
            break;
        case 0xd4: // CNC adr
            i = printInstruction(buffer, i, 3, "CNC", "", "", S_16BIT);
            break;
        case 0xd5: // PUSH D
            i = printInstruction(buffer, i, 1, "PUSH", "D", "", S_REG);
            break;
        case 0xd6: // SUI D8
            i = printInstruction(buffer, i, 2, "SUI", "", "", S_8BIT);
            break;
        case 0xd7: // RST 2
            i = printInstruction(buffer, i, 1, "RST", "2", "", S_REG);
            break;
        case 0xd8: // RC
            i = printInstruction(buffer, i, 1, "RC", "", "", NO_PARAM);
            break;
        case 0xda: // JC adr
            i = printInstruction(buffer, i, 3, "JC", "", "", S_16BIT);
            break;
        case 0xdb: // IN D8
            i = printInstruction(buffer, i, 2, "IN", "", "", S_8BIT);
            break;
        case 0xdc: // CC adr
            i = printInstruction(buffer, i, 3, "CC", "", "", S_16BIT);
            break;
        case 0xde: // SBI D8
            i = printInstruction(buffer, i, 2, "SBI", "", "", S_8BIT);
            break;
        case 0xdf: // RST 3
            i = printInstruction(buffer, i, 1, "RST", "3", "", S_REG);
            break;
        case 0xe0: // RPO
            i = printInstruction(buffer, i, 1, "RPO", "", "", NO_PARAM);
            break;
        case 0xe1: // POP H
            i = printInstruction(buffer, i, 1, "POP", "H", "", S_REG);
            break;
        case 0xe2: // JPO adr
            i = printInstruction(buffer, i, 3, "JPO", "", "", S_16BIT);
            break;
        case 0xe3: // XTHL
            i = printInstruction(buffer, i, 1, "XTHL", "", "", NO_PARAM);
            break;
        case 0xe4: // CPO adr
            i = printInstruction(buffer, i, 3, "CPO", "", "", S_16BIT);
            break;
        case 0xe5: // PUSH H
            i = printInstruction(buffer, i, 1, "PUSH", "H", "", S_REG);
            break;
        case 0xe6: // ANI D8
            i = printInstruction(buffer, i, 2, "ANI", "", "", S_8BIT);
            break;
        case 0xe7: // RST 4
            i = printInstruction(buffer, i, 1, "RST", "4", "", S_REG);
            break;
        case 0xe8: // RPE
            i = printInstruction(buffer, i, 1, "RPE", "", "", NO_PARAM);
            break;
        case 0xe9: // PCHL
            i = printInstruction(buffer, i, 1, "PCHL", "", "", NO_PARAM);
            break;
        case 0xea: // JPE adr
            i = printInstruction(buffer, i, 3, "JPE", "", "", S_16BIT);
            break;
        case 0xeb: // XCHG
            i = printInstruction(buffer, i, 1, "XCHG", "", "", NO_PARAM);
            break;
        case 0xec: // CPE adr
            i = printInstruction(buffer, i, 3, "CPE", "", "", S_16BIT);
            break;
        case 0xee: // XRI D8
            i = printInstruction(buffer, i, 2, "XRI", "", "", S_8BIT);
            break;
        case 0xef: // RST 5
            i = printInstruction(buffer, i, 1, "RST", "5", "", S_REG);
            break;
        case 0xf0: // RP
            i = printInstruction(buffer, i, 1, "RP", "", "", NO_PARAM);
            break;
        case 0xf1: // POP PSW
            i = printInstruction(buffer, i, 1, "POP", "PSW", "", S_REG);
            break;
        case 0xf2: // JP adr
            i = printInstruction(buffer, i, 3, "JP", "", "", S_16BIT);
            break;
        case 0xf3: // DI
            i = printInstruction(buffer, i, 1, "DI", "", "", NO_PARAM);
            break;
        case 0xf4: // CP adr
            i = printInstruction(buffer, i, 3, "CP", "", "", S_16BIT);
            break;
        case 0xf5: // PUSH PSW
            i = printInstruction(buffer, i, 1, "PUSH", "PSW", "", S_REG);
            break;
        case 0xf6: // ORI D8
            i = printInstruction(buffer, i, 2, "ORI", "", "", S_8BIT);
            break;
        case 0xf7: // RST 6
            i = printInstruction(buffer, i, 1, "RST", "6", "", S_REG);
            break;
        case 0xf8: // RM
            i = printInstruction(buffer, i, 1, "RM", "", "", NO_PARAM);
            break;
        case 0xf9: // SPHL
            i = printInstruction(buffer, i, 1, "SPHL", "", "", NO_PARAM);
            break;
        case 0xfa: // JM adr
            i = printInstruction(buffer, i, 3, "JM", "", "", S_16BIT);
            break;
        case 0xfb: // EI
            i = printInstruction(buffer, i, 1, "EI", "", "", NO_PARAM);
            break;
        case 0xfc: // CM adr
            i = printInstruction(buffer, i, 3, "CM", "", "", S_16BIT);
            break;
        case 0xfe: // CPI D8
            i = printInstruction(buffer, i, 2, "CPI", "", "", S_8BIT);
            break;
        case 0xff: // RST 7
            i = printInstruction(buffer, i, 1, "RST", "7", "", S_REG);
            break;
        default: // Undefined instructions
            i = printInstruction(buffer, i, 1, "--", "", "", NO_PARAM);
        }
    }
}
