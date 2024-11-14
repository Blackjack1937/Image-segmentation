#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <time.h>

int pm_getint(FILE *file);
unsigned char pm_getrawbyte(FILE *file);

int main(int argc, char *argv[]) {
    int K;      
   printf("Enter the number of clusters (K): ");
    if (scanf("%d", &K) != 1 || K <= 0) {
        printf("Invalid number of clusters.\n");
        return 1;
    }
    srand(time(NULL));
   
    FILE *inputFile = fopen(argv[1], "rb");
    if (inputFile == NULL) {
        printf("Error opening input file.\n");
        return 1;
    }  
    char format[3];
    format[0] = pm_getrawbyte(inputFile);
    format[1] = pm_getrawbyte(inputFile);
    format[2] = '\0';

    if (format[0] != 'P' || format[1] != '6') {
        printf("Input file is not a binary PPM (P6) image.\n");
        fclose(inputFile);
        return 1;
    }
    int width = pm_getint(inputFile);
    int height = pm_getint(inputFile);
    int maxval = pm_getint(inputFile);
 
    if (maxval != 255) {
        printf("Unsupported max color value (only 255 is supported).\n");
        fclose(inputFile);
        return 1;
    }

   
    unsigned char *imageData = (unsigned char *)malloc(3 * width * height);
    if (imageData == NULL) {
        printf("Memory allocation failed.\n");
        fclose(inputFile);
        return 1;
    }

   
    fread(imageData, 3, width * height, inputFile);
    fclose(inputFile);

   
    kmeans_segmentation(imageData, width, height, K);

   
save_image(imageData, width, height);

    

    free(imageData);
    return 0;
}
