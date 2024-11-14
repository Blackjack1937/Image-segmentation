#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include <string.h>

// Function to get a character from the file (handling comments and whitespaces)
char pm_getc(FILE *file) {
    int ich;
    char ch;

    ich = getc(file);
    if (ich == EOF)
        pm_erreur("EOF / read error");
    ch = (char)ich;

 
    if (ch == '#') {
        do {
            ich = getc(file);
            if (ich == EOF)
                pm_erreur("EOF / read error");
            ch = (char)ich;
        } while (ch != '\n' && ch != '\r'); // Ignore comments till end of line
    }

    return ch;
}

// Function to get a raw byte (used for reading headers)
unsigned char pm_getrawbyte(FILE *file) {
    int iby;

    iby = getc(file);
    if (iby == EOF)
        pm_erreur("EOF / read error ");
    return (unsigned char)iby;
}

// Function to get an integer from the file (e.g., width, height, max color value)
int pm_getint(FILE *file) {
    char ch;
    int i;

    do {
        ch = pm_getc(file);
    } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

    if (ch < '0' || ch > '9')
        pm_erreur("read error: != [0-9]");

    i = 0;
    do {
        i = i * 10 + ch - '0';
        ch = pm_getc(file);
    } while (ch >= '0' && ch <= '9');

    return i;
}

// Error handling function
void pm_erreur(char *texte) {
    fprintf(stderr, "\n%s \n\n", texte);
    exit(1);
}


// Function to initialize cluster centers randomly
void initialize_centers(int K, int N, unsigned char *imageData, unsigned char *centers) {
    for (int i = 0; i < K; i++) {
        int random_index = rand() % N;
        centers[i * 3] = imageData[random_index * 3];
        centers[i * 3 + 1] = imageData[random_index * 3 + 1];
        centers[i * 3 + 2] = imageData[random_index * 3 + 2];
    }
}

// Function to get user-defined cluster centers
// void get_user_centers(int K, unsigned char *centers) {
//     for (int i = 0; i < K; i++) {
//         printf("Enter RGB values for center %d (R G B): ", i + 1);
//         scanf("%hhu %hhu %hhu", &centers[i * 3], &centers[i * 3 + 1], &centers[i * 3 + 2]);
//     }
// }

// K-means segmentation function with convergence-based stopping condition
void kmeans_segmentation(unsigned char *imageData, int width, int height, int K) {
    int N = width * height;

    unsigned char *centers = (unsigned char *)malloc(K * 3 * sizeof(unsigned char)); 
    int *assign = (int *)malloc(N * sizeof(int));
    int *counts = (int *)malloc(K * sizeof(int)); 
    float *new_centers = (float *)malloc(K * 3 * sizeof(float)); 

    initialize_centers(K, N, imageData, centers);  
//   get_user_centers(int K, unsigned char *centers);
    float threshold = 0.001;  
    int i;

    for (i = 0; i < 50; i++) {
        memset(new_centers, 0, K * 3 * sizeof(float));
        for (int j = 0; j < K; j++) {
            counts[j] = 0;
        }

        for (int i = 0; i < N; i++) {
            int best_center = 0;
            float min_dist = 1e9;
            for (int j = 0; j < K; j++) {
                float r_diff = imageData[i * 3] - centers[j * 3];
                float g_diff = imageData[i * 3 + 1] - centers[j * 3 + 1];
                float b_diff = imageData[i * 3 + 2] - centers[j * 3 + 2];
                float distance = r_diff * r_diff + g_diff * g_diff + b_diff * b_diff;
                if (distance < min_dist) {
                    min_dist = distance;
                    best_center = j;
                }
            }
            assign[i] = best_center;
            new_centers[best_center * 3] += (float)imageData[i * 3];
            new_centers[best_center * 3 + 1] += (float)imageData[i * 3 + 1];
            new_centers[best_center * 3 + 2] += (float)imageData[i * 3 + 2];
            counts[best_center]++;
        }

        float max_center_shift = 0.0f;  
        for (int j = 0; j < K; j++) {
            if (counts[j] > 0) {
                float new_r = new_centers[j * 3] / counts[j];
                float new_g = new_centers[j * 3 + 1] / counts[j];
                float new_b = new_centers[j * 3 + 2] / counts[j];

                float shift = sqrtf(
                    (new_r - centers[j * 3]) * (new_r - centers[j * 3]) +
                    (new_g - centers[j * 3 + 1]) * (new_g - centers[j * 3 + 1]) +
                    (new_b - centers[j * 3 + 2]) * (new_b - centers[j * 3 + 2])
                );

                centers[j * 3] = (unsigned char)new_r;
                centers[j * 3 + 1] = (unsigned char)new_g;
                centers[j * 3 + 2] = (unsigned char)new_b;

                if (shift > max_center_shift) {
                    max_center_shift = shift;
                }
            } else {
                int random_index = rand() % N;
                centers[j * 3] = imageData[random_index * 3];
                centers[j * 3 + 1] = imageData[random_index * 3 + 1];
                centers[j * 3 + 2] = imageData[random_index * 3 + 2];
                printf("Reinitialized center %d\n", j);
            }
        }

        if (max_center_shift < threshold) {
            printf("Converged after %d iterations.\n", i+ 1);
            break;
        }
    }

    for (int i = 0; i < N; i++) {
        int cluster = assign[i];
        imageData[i * 3] = centers[cluster * 3];
        imageData[i * 3 + 1] = centers[cluster * 3 + 1];
        imageData[i * 3 + 2] = centers[cluster * 3 + 2];
    }

    free(centers);
    free(assign);
    free(counts);
    free(new_centers);
}



void save_image(unsigned char *imageData, int width, int height) {
    char filename[100]; 

    printf("Enter the output file name with ppm extension");
    scanf("%99s", filename); 

    
    FILE *outputFile = fopen(filename, "wb");
    if (outputFile == NULL) {
        printf("Error saving output file.\n");
        return;
    }

    fprintf(outputFile, "P6\n%d %d\n255\n", width, height);

    fwrite(imageData, 3, width * height, outputFile);

    fclose(outputFile);

    printf("Segmentation completed. Saved to %s\n", filename);
}

