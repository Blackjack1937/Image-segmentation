#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <time.h>

int pm_getint(FILE *file);
unsigned char pm_getrawbyte(FILE *file);

int main(int argc, char *argv[])
{
    int K;
    printf("Enter the number of clusters (K): ");
    if (scanf("%d", &K) != 1 || K <= 0)
    {
        printf("Invalid number of clusters.\n");
        return 1;
    }

    int method;
    printf("Choose segmentation method:\n1. RGB-only\n2. RGB + location\nEnter your choice (1 or 2): ");
    if (scanf("%d", &method) != 1 || (method != 1 && method != 2))
    {
        printf("Invalid choice.\n");
        return 1;
    }

    float lambda = 0.0; // default value for the weighing factor
    if (method == 2)
    {
        printf("Enter the weighing factor for spatial influence (lambda): ");
        if (scanf("%f", &lambda) != 1 || lambda < 0)
        {
            printf("Invalid weighing factor.\n");
            return 1;
        }
    }

    int stop_condition;
    printf("Choose stopping condition:\n1. Center shift\n2. Pixel assignment stability\nEnter your choice (1 or 2): ");
    if (scanf("%d", &stop_condition) != 1 || (stop_condition != 1 && stop_condition != 2))
    {
        printf("Invalid choice.\n");
        return 1;
    }

    char inputFileName[100];
    printf("Enter the input file name: ");
    scanf("%99s", inputFileName);

    FILE *inputFile = fopen(inputFileName, "rb");
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

    if (method == 1)
    {
        kmeans_segmentation(imageData, width, height, K, stop_condition);
    }
    else
    {
        kmeans_segmentation_RGB_location(imageData, width, height, K, stop_condition, lambda);
    }

    char outputFileName[100];
    printf("Enter the output file name with '.ppm' extension: ");
    scanf("%99s", outputFileName);

    save_image(imageData, width, height, outputFileName);

    free(imageData);
    return 0;
}
