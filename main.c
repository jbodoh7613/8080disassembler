#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>

// void fillBuffer(FILE *input, int8_t buffer, int size);
uint8_t *fillBuffer(FILE *input, int size);
int findSize(FILE *input);
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
void readBuffer(uint8_t *buffer, int size)
{
    static uint8_t incIns1 = 0;
    static int isIncIns1 = 0;
    static uint8_t incIns2 [2] = {0, 0};
    static int isIncIns2 = 0;
    char inst[5];
    int i = 0;

    for(; i < size; i++)
    {
        switch(*buffer)
        {
        case 0x00:
            printf("%04x %02x       NOP\n", i, *buffer);
            break;
        case 0x01:
            printf("%04x %02x %02x %02x LXI    B,$%02x%02x\n", i, buffer[0], buffer[1], buffer[2], buffer[2], buffer[1]);
            buffer += 2;
            i += 2;
            break;
        case 0x02:
            printf("%04x %02x       STAX   B\n", i, *buffer);
            break;
        case 0x03:
            printf("%04x %02x       INX    B\n", i, *buffer);
            break;
        case 0x04:
            printf("%04x %02x       INR    B\n", i, *buffer);
            break;
        case 0x05:
            printf("%04x %02x       DCR    B\n", i, *buffer);
            break;
        case 0x06:
            printf("%04x %02x %02x    MVI    B,$%02x\n", i, buffer[0], buffer[1], buffer[1]);
            buffer += 1;
            i += 1;
            break;
        case 0x07:
            printf("%04x %02x       RLC\n", i, *buffer);
            break;
        case 0x08:
            printf("%04x %02x       --\n", i, *buffer);
            break;
        case 0x09:
            printf("%04x %02x       DAD    B\n", i, *buffer);
            break;
        case 0x0a:
            printf("%04x %02x       LDAX   B\n", i, *buffer);
            break;
        case 0x0b:
            printf("%04x %02x       DCX    B\n", i, *buffer);
            break;
        case 0x0c:
            printf("%04x %02x       INR    C\n", i, *buffer);
            break;
        case 0x0d:
            printf("%04x %02x       DCR    C\n", i, *buffer);
            break;
        case 0x0e:
            printf("%04x %02x %02x    MVI    C,$%02x\n", i, buffer[0], buffer[1], buffer[1]);
            buffer += 1;
            i += 1;
            break;
        case 0x0f:
            printf("%04x %02x       RRC\n", i, *buffer);
            break;
        case 0x10:
            printf("%04x %02x       --\n", i, *buffer);
            break;
        case 0x11:
            printf("%04x %02x %02x %02x LXI    D,$%02x%02x\n", i, buffer[0], buffer[1], buffer[2], buffer[2], buffer[1]);
            buffer += 2;
            i += 2;
            break;
        case 0x12:
            printf("%04x %02x       STAX   D\n", i, *buffer);
            break;
        case 0x13:
            printf("%04x %02x       INX    D\n", i, *buffer);
            break;
        case 0x14:
            printf("%04x %02x       INR    D\n", i, *buffer);
            break;
        case 0x15:
            printf("%04x %02x       DCR    D\n", i, *buffer);
            break;
        case 0x16:
            printf("%04x %02x %02x    MVI    D,$%02x\n", i, buffer[0], buffer[1], buffer[1]);
            buffer += 1;
            i += 1;
            break;
        case 0x17:
            printf("%04x %02x       RAL\n", i, *buffer);
            break;
        case 0x18:
            printf("%04x %02x       --\n", i, *buffer);
            break;
        case 0x19:
            printf("%04x %02x       DAD    D\n", i, *buffer);
            break;
        case 0x1a:
            printf("%04x %02x       LDAX   D\n", i, *buffer);
            break;
        case 0x1b:
            printf("%04x %02x       DCX    D\n", i, *buffer);
            break;
        case 0x1c:
            printf("%04x %02x       INR    E\n", i, *buffer);
            break;
        case 0x1d:
            printf("%04x %02x       DCR    E\n", i, *buffer);
            break;
        case 0x1e:
            printf("%04x %02x %02x    MVI    E,$%02x\n", i, buffer[0], buffer[1], buffer[1]);
            buffer += 1;
            i += 1;
            break;
        case 0x1f:
            printf("%04x %02x       RAR\n", i, *buffer);
            break;
        case 0x20:
            printf("%04x %02x       --\n", i, *buffer);
            break;
        case 0x21:
            printf("%04x %02x %02x %02x LXI    H,$%02x%02x\n", i, buffer[0], buffer[1], buffer[2], buffer[2], buffer[1]);
            buffer += 2;
            i += 2;
            break;
        case 0x32:
            printf("%04x %02x %02x %02x STA    $%02x%02x\n", i, buffer[0], buffer[1], buffer[2], buffer[2], buffer[1]);
            buffer += 2;
            i += 2;
            break;
        case 0x3e:
            printf("%04x %02x %02x    MVI    A,$%02x\n", i, buffer[0], buffer[1], buffer[1]);
            buffer += 1;
            i += 1;
            break;
        case 0xc3:
            printf("%04x %02x %02x %02x JMP    $%02x%02x\n", i, buffer[0], buffer[1], buffer[2], buffer[2], buffer[1]);
            buffer += 2;
            i += 2;
            break;
        case 0xc5:
            printf("%04x %02x       PUSH   B\n", i, *buffer);
            break;
        case 0xd5:
            printf("%04x %02x       PUSH   D\n", i, *buffer);
            break;
        case 0xe5:
            printf("%04x %02x       PUSH   H\n", i, *buffer);
            break;
        case 0xf5:
            printf("%04x %02x       PUSH   PSW\n", i, *buffer);
            break;
        }
        buffer++;
    }
}
