#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <time.h>

int pm_getint(FILE *file);
unsigned char pm_getrawbyte(FILE *file);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <input_ppm_file>\n", argv[0]);
        return 1;
    }

    // Seed the random number generator
    srand(time(NULL));

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
    int N = width * height; // Number of pixels in image
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

    // Test printing for imageData reading
    /* for (int i = 0; i < width * height; i++)
    {
        unsigned char r = imageData[i * 3];
        unsigned char g = imageData[i * 3 + 1];
        unsigned char b = imageData[i * 3 + 2];
        printf("Pixel %d: R=%d G=%d B=%d\n", i, r, g, b);
    } */

    // Getting random cluster centers
    int K = 3; // Number of random cluster centers
    unsigned char *random_K_centers = (unsigned char *)malloc(K * 3);
    randomK(K, N, imageData, random_K_centers);

    // Test printing for random centers
    /*  for (int i = 0; i < K; i++)
     {
         unsigned char r = random_K_centers[i * 3];
         unsigned char g = random_K_centers[i * 3 + 1];
         unsigned char b = random_K_centers[i * 3 + 2];
         printf("Pixel %d: R=%d G=%d B=%d\n", i, r, g, b);
     } */

    // Calculate distances from each pixel to each center
    float **distances = calculateDistances(width, height, K, random_K_centers);

    // Example: print distances
    for (int i = 0; i < width * height; i++)
    {
        for (int j = 0; j < K; j++)
        {
            printf("Distance from pixel %d to center %d: %f\n", i, j, distances[i][j]);
        }
    }
    for (int i = 0; i < width * height; i++)
    {
        free(distances[i]);
    }
    free(distances);
    free(random_K_centers);
    free(imageData);
    return 0;
}
