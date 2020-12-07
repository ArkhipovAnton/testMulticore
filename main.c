#include <stdio.h>
#include <stdlib.h>
//#include <cstdint.h>
typedef unsigned char byte;
/*
work in systeems with 8-bit char
*/

#define FILE_ERROR -3

#define EDID_SIZE   128
#define FILE_SIZE   2048

#define ADDR_HEADER         0x00
#define VENDER_ADDR         0x08
#define EISA_MAN_CODE_ADDR  0x08
#define LSB_ADDR            0x0a
#define MSB_ADDR            0x0b
#define DISP_PARAM_ADDR     0x14    //0x80
#define SN_ADDR             0x0C
#define WEEK_OF_PROD        0x10
#define YEAR_OF_PROD        0x11
#define EDID_VERSION        0x12
#define EDID_REVISION       0x13
#define VID_ADDR            0x14
#define H_SIZE_ADDR         0x15
#define V_SIZE_ADDR         0x16
#define GAMMA_ADDR          0x17
#define ESTABLISHED_TIMING_1            0x23
#define ESTABLISHED_TIMING_2            0x24
#define MANUFACTURERS_TIMINGS           0x25



int help()
{
    puts("The program convert EDIT files to text form, arguments:");
    puts("-i - input file name");
    puts("-o - output file name");
    puts("-h - this message");
    return 0;
}

int procEDID(const byte *source, byte *out){
    unsigned int addr = 0;
    //add manufacture code
    addr += sprintf(out+addr, "EISA Manufacture ID = %x%x\r\n",source[EISA_MAN_CODE_ADDR],source[EISA_MAN_CODE_ADDR+1]);
    //add LSB and MSB
    addr += sprintf(out+addr,"\r\n");
    addr += sprintf(out+addr, "Product code = %d\r\n", source[MSB_ADDR]<<8|source[LSB_ADDR]);
//    addr += sprintf(out+addr, "Product code, MSB = %x\r\n", source[MSB_ADDR]);
    addr += sprintf(out+addr, "Serial number, SN=%d\r\n", source [SN_ADDR]<<24 | source [SN_ADDR+1]<<16 | source [SN_ADDR+2]<<8 | source [SN_ADDR+3]);
    addr += sprintf(out+addr, "EDID file ver.rev = %d.%d\r\n", source[EDID_VERSION],source[EDID_REVISION]);
    addr += sprintf(out+addr, "\r\n\r\nDisplay parameters:\r\n");
    addr += sprintf(out+addr, "Video input definations: %x\r\n",source[VID_ADDR]);

    addr += sprintf(out+addr, "max H image size (cm) = %d\r\n", source[H_SIZE_ADDR]);
    addr += sprintf(out+addr, "max V image size (cm) = %d\r\n", source[V_SIZE_ADDR]);
    addr += sprintf(out+addr, "Display gamma = %g\r\n",((100.*source[GAMMA_ADDR])/100.));
    addr += sprintf(out+addr, "Features (DPMS, Active off, RGB, timing BLK1) = %x \r\n", source [0x18]);//TODO bit parcing
    addr += sprintf(out+addr, "Red/Green low bits %x\r\n", source [0x19]);//todo
    addr += sprintf(out+addr, "Blue/White low bits %x\r\n", source [0x1A]);//todo
    addr += sprintf(out+addr, "Red X Rx = 0.%d\r\n",source [0x1B]);
    addr += sprintf(out+addr, "Red Y Ry = 0.%d\r\n",source [0x1C]);
    addr += sprintf(out+addr, "Green X Gx = 0.%d\r\n",source [0x1D]);
    addr += sprintf(out+addr, "Green y Gy = 0.%d\r\n",source [0x1E]);
    addr += sprintf(out+addr, "Blue X Bx = 0.%d\r\n",source [0x1F]);
    addr += sprintf(out+addr, "Blue Y By = 0.%d\r\n",source [0x20]);
    addr += sprintf(out+addr, "White X Wx = 0.%d\r\n",source [0x21]);
    addr += sprintf(out+addr, "White Y Wy = 0.%d\r\n",source [0x22]);

    addr += sprintf(out+addr, "\r\nEstablished timings:\r\n");
    if (source[ESTABLISHED_TIMING_1] != 0x00)
        addr += sprintf(out+addr, "Established timing I = %d\r\n",source[ESTABLISHED_TIMING_1]);
    else
        addr += sprintf(out+addr, "Established timing I not used\r\n");
    if (source[ESTABLISHED_TIMING_2] != 0x00)
        addr += sprintf(out+addr, "Established timing II = %d\r\n",source[ESTABLISHED_TIMING_2]);
    else
        addr += sprintf(out+addr, "Established timing II not used\r\n");
    if (source[MANUFACTURERS_TIMINGS] != 0x00)
        addr += sprintf(out+addr, "Manufactures timings = %d\r\n",source[MANUFACTURERS_TIMINGS]);
    else
        addr += sprintf(out+addr, "Manufactures timings not used\r\n");

    addr += sprintf(out+addr, "\r\nStandatd timing ID\r\n");
    for (int i = 1; i<=8; i++)
    {
        if (source[MANUFACTURERS_TIMINGS+i]!=0x01)
            addr += sprintf(out+addr, "Standart timing Identification %d = %d\r\n",i,source[MANUFACTURERS_TIMINGS+i]<<8|source[MANUFACTURERS_TIMINGS+i+1]);
        else
            addr += sprintf(out+addr, "Standart timing Identification %d not user \r\n",i);
    }

    addr += sprintf(out+addr, "\r\nTiming descriptor #1\r\n");
    addr += sprintf(out+addr, "Pixel clock = d\r\n");
    addr += sprintf(out+addr, "\r\n");
    addr += sprintf(out+addr, "\r\n");
    addr += sprintf(out+addr, "\r\n");
    addr += sprintf(out+addr, "\r\n");
    addr += sprintf(out+addr, "\r\n");
    addr += sprintf(out+addr, "\r\n");

    puts("procEDID");


    printf(out);
    return 0;
}

int checkFile(const byte *file)
{
    byte checksum = 0;

    if (strncmp(file,"\x00\x11\x11\x11\x11\x11\x11\x00",8)==0)//check header
    {

        puts("in if");
        for (int i = 0; i < EDID_SIZE; i++)//check summ of all elements, may == 0
        {
            checksum += file[i];
        }
        if (checksum == 0)
        {
            return 0;
        }
        else
        {
            printf("checksum value = %x \r\n",checksum);
        }
    }
    return FILE_ERROR;
    //    todo check file header and checksum
}

int procFile(const byte *in, const byte *out)
{
    FILE *inFile;
    FILE *outFile;
    byte  edidFile[EDID_SIZE];
    byte  outFileStr[FILE_SIZE];

    if ((inFile=fopen(in, "rb"))==NULL)
    {
         puts ("Cannot open EDID file");
         return FILE_ERROR;
    }

    puts("check file");
    if(fread(edidFile, sizeof(char), EDID_SIZE, inFile)!=EDID_SIZE)
    {
        puts("in IF");
        if(feof(inFile))
            printf("Premature end of file.");
        else
            printf("File read error.");
    }
    if (checkFile(edidFile)==0)
    {
        procEDID(edidFile, outFileStr);
    }


    fclose(inFile);
//    size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );

}



int main(int argc, char **argv)
{
    int i;
    char *fileIn;
    char *fileOut;

    if (argc < 3 || argc > 5) {
        if (argc > 1)
        {
            if (strcmp(argv[1],"-h")==0)
            {
                help();
            }
            else
            {
                puts("Wrong parameters, use -h for more information");
                help();
            }
        }
        else
        {
            puts("Wrong parameters, use -h for more information");
            help();
        }
        return 0;
    }
    else
    for (i = 1; i < 5; i+=2)
    {
        if (strcmp(argv[i],"-i")==0)
        {
            fileIn = argv[i+1];
            printf("input file = %s\r\n",fileIn);
        }
        else if (strcmp(argv[i],"-o")==0)
        {
            fileOut = argv[i+1];
            printf("output file = %s\r\n",fileOut);
        }
        else
        {
            puts("Wrong parameters, use -h for more information");
            help();
        }
    }

    return procFile(fileIn, fileOut);
}