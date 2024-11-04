#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int pm_getint(FILE *file);
unsigned char pm_getrawbyte(FILE *file);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <input_ppm_file>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "rb");
    if (inputFile == NULL)
    {
        printf("Error opening input file.\n");
        return 1;
    }

    char format[3];
    format[0] = pm_getrawbyte(inputFile);
    format[1] = pm_getrawbyte(inputFile);
    format[2] = '\0';

    if (format[0] != 'P' || format[1] != '6')
    {
        printf("Input file is not a binary PPM (P6) image.\n");
        fclose(inputFile);
        return 1;
    }

    int width = pm_getint(inputFile);
    int height = pm_getint(inputFile);
    int maxval = pm_getint(inputFile);

    if (maxval != 255)
    {
        printf("Unsupported max color value (only 255 is supported).\n");
        fclose(inputFile);
        return 1;
    }

    unsigned char *imageData = (unsigned char *)malloc(3 * width * height);
    if (imageData == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(inputFile);
        return 1;
    }

    fread(imageData, 3, width * height, inputFile);
    fclose(inputFile);
