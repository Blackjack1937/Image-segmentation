#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include <string.h>

// Function to get a character from the file (handling comments and whitespaces)
char pm_getc(FILE *file)
{
    int ich;
    char ch;

    ich = getc(file);
    if (ich == EOF)
        pm_erreur("EOF / read error");
    ch = (char)ich;

    if (ch == '#')
    {
        do
        {
            ich = getc(file);
            if (ich == EOF)
                pm_erreur("EOF / read error");
            ch = (char)ich;
        } while (ch != '\n' && ch != '\r'); // Ignore comments till end of line
    }

    return ch;
}

// Function to get a raw byte (used for reading headers)
unsigned char pm_getrawbyte(FILE *file)
{
    int iby;

    iby = getc(file);
    if (iby == EOF)
        pm_erreur("EOF / read error ");
    return (unsigned char)iby;
}

// Function to get an integer from the file (e.g., width, height, max color value)
int pm_getint(FILE *file)
{
    char ch;
    int i;

    do
    {
        ch = pm_getc(file);
    } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

    if (ch < '0' || ch > '9')
        pm_erreur("read error: != [0-9]");

    i = 0;
    do
    {
        i = i * 10 + ch - '0';
        ch = pm_getc(file);
    } while (ch >= '0' && ch <= '9');

    return i;
}

// Error handling function
void pm_erreur(char *texte)
{
    fprintf(stderr, "\n%s \n\n", texte);
    exit(1);
}

// Function to initialize cluster centers randomly
void initialize_centers(int K, int N, unsigned char *imageData, unsigned char *centers)
{
    for (int i = 0; i < K; i++)
    {
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

// K-means segmentation function with convergence-based stopping condition and  pixel assignment stability
// (less than 1% of pixels reassigned)
void kmeans_segmentation(unsigned char *imageData, int width, int height, int K, int stop_condition)
{
    int N = width * height;

    unsigned char *centers = (unsigned char *)malloc(K * 3 * sizeof(unsigned char));
    int *assign = (int *)malloc(N * sizeof(int));
    int *counts = (int *)malloc(K * sizeof(int));
    float *new_centers = (float *)malloc(K * 3 * sizeof(float));

    initialize_centers(K, N, imageData, centers);

    float threshold = 0.001; // center shift threshold
    int max_iterations = 50; // maximum iterations
    int i;

    for (i = 0; i < max_iterations; i++)
    {
        memset(new_centers, 0, K * 3 * sizeof(float));
        for (int j = 0; j < K; j++)
        {
            counts[j] = 0;
        }

        int changes = 0; // track the number of changed assignments

        // assignment step: associate each pixel with the closest center
        for (int idx = 0; idx < N; idx++)
        {
            int best_center = 0;
            float min_dist = 1e9;
            for (int j = 0; j < K; j++)
            {
                float r_diff = imageData[idx * 3] - centers[j * 3];
                float g_diff = imageData[idx * 3 + 1] - centers[j * 3 + 1];
                float b_diff = imageData[idx * 3 + 2] - centers[j * 3 + 2];
                float distance = r_diff * r_diff + g_diff * g_diff + b_diff * b_diff;
                if (distance < min_dist)
                {
                    min_dist = distance;
                    best_center = j;
                }
            }
            if (assign[idx] != best_center)
            {
                changes++; // increment changes if assignment differs
            }
            assign[idx] = best_center;
            new_centers[best_center * 3] += (float)imageData[idx * 3];
            new_centers[best_center * 3 + 1] += (float)imageData[idx * 3 + 1];
            new_centers[best_center * 3 + 2] += (float)imageData[idx * 3 + 2];
            counts[best_center]++;
        }

        float max_center_shift = 0.0f;
        for (int j = 0; j < K; j++)
        {
            if (counts[j] > 0)
            {
                float new_r = new_centers[j * 3] / counts[j];
                float new_g = new_centers[j * 3 + 1] / counts[j];
                float new_b = new_centers[j * 3 + 2] / counts[j];

                float shift = sqrtf(
                    (new_r - centers[j * 3]) * (new_r - centers[j * 3]) +
                    (new_g - centers[j * 3 + 1]) * (new_g - centers[j * 3 + 1]) +
                    (new_b - centers[j * 3 + 2]) * (new_b - centers[j * 3 + 2]));

                centers[j * 3] = (unsigned char)new_r;
                centers[j * 3 + 1] = (unsigned char)new_g;
                centers[j * 3 + 2] = (unsigned char)new_b;

                if (shift > max_center_shift)
                {
                    max_center_shift = shift;
                }
            }
            else
            {
                int random_index = rand() % N;
                centers[j * 3] = imageData[random_index * 3];
                centers[j * 3 + 1] = imageData[random_index * 3 + 1];
                centers[j * 3 + 2] = imageData[random_index * 3 + 2];
                printf("Reinitialized center %d\n", j);
            }
        }

        // stopping conditions
        if (stop_condition == 2)
        { // pixel assignment stability
            float fraction_changed = (float)changes / N;
            if (fraction_changed < 0.01)
            {
                printf("Converged based on assignment stability after %d iterations.\n", i + 1);
                break;
            }
        }
        else
        { // center shift
            if (max_center_shift < threshold)
            {
                printf("Converged based on center shift after %d iterations.\n", i + 1);
                break;
            }
        }
    }

    for (int idx = 0; idx < N; idx++)
    {
        int cluster = assign[idx];
        imageData[idx * 3] = centers[cluster * 3];
        imageData[idx * 3 + 1] = centers[cluster * 3 + 1];
        imageData[idx * 3 + 2] = centers[cluster * 3 + 2];
    }

    free(centers);
    free(assign);
    free(counts);
    free(new_centers);
}

// K-means segmentation that considers both RGB and location(i,j) in the image
void kmeans_segmentation_RGB_location(unsigned char *imageData, int width, int height, int K, int stop_condition, float lambda)
{
    int N = width * height;

    unsigned char *centers = (unsigned char *)malloc(K * 5 * sizeof(unsigned char)); // (R, G, B, i, j)
    int *assign = (int *)malloc(N * sizeof(int));
    int *counts = (int *)malloc(K * sizeof(int));
    float *new_centers = (float *)malloc(K * 5 * sizeof(float));

    initialize_centers(K, N, imageData, centers);

    float threshold = 0.001; // center shift threshold
    int max_iterations = 50; // maximum iterations
    int i;

    for (i = 0; i < max_iterations; i++)
    {
        memset(new_centers, 0, K * 5 * sizeof(float)); // reset new centers
        for (int j = 0; j < K; j++)
        {
            counts[j] = 0;
        }

        int changes = 0; // track the number of changed assignments

        // assignment step: associate each pixel with the closest center
        for (int idx = 0; idx < N; idx++)
        {
            int i_coord = idx / width; // row index
            int j_coord = idx % width; // column index
            int best_center = 0;
            float min_dist = 1e9;

            for (int j = 0; j < K; j++)
            {
                // normalize RGB and spatial components
                float r_diff = imageData[idx * 3] / 255.0 - centers[j * 5] / 255.0;
                float g_diff = imageData[idx * 3 + 1] / 255.0 - centers[j * 5 + 1] / 255.0;
                float b_diff = imageData[idx * 3 + 2] / 255.0 - centers[j * 5 + 2] / 255.0;
                float i_diff = (float)i_coord / height - centers[j * 5 + 3] / height;
                float j_diff = (float)j_coord / width - centers[j * 5 + 4] / width;

                // combined distance with weighted spatial influence
                float distance = r_diff * r_diff + g_diff * g_diff + b_diff * b_diff +
                                 lambda * (i_diff * i_diff + j_diff * j_diff);

                if (distance < min_dist)
                {
                    min_dist = distance;
                    best_center = j;
                }
            }

            if (assign[idx] != best_center)
            {
                changes++; // increment changes if assignment differs
            }
            assign[idx] = best_center;

            new_centers[best_center * 5] += (float)imageData[idx * 3];
            new_centers[best_center * 5 + 1] += (float)imageData[idx * 3 + 1];
            new_centers[best_center * 5 + 2] += (float)imageData[idx * 3 + 2];
            new_centers[best_center * 5 + 3] += (float)i_coord;
            new_centers[best_center * 5 + 4] += (float)j_coord;
            counts[best_center]++;
        }

        float max_center_shift = 0.0f;
        for (int j = 0; j < K; j++)
        {
            if (counts[j] > 0)
            {
                float new_r = new_centers[j * 5] / counts[j];
                float new_g = new_centers[j * 5 + 1] / counts[j];
                float new_b = new_centers[j * 5 + 2] / counts[j];
                float new_i = new_centers[j * 5 + 3] / counts[j];
                float new_j = new_centers[j * 5 + 4] / counts[j];

                float shift = sqrtf(
                    (new_r - centers[j * 5]) * (new_r - centers[j * 5]) +
                    (new_g - centers[j * 5 + 1]) * (new_g - centers[j * 5 + 1]) +
                    (new_b - centers[j * 5 + 2]) * (new_b - centers[j * 5 + 2]) +
                    lambda * ((new_i - centers[j * 5 + 3]) * (new_i - centers[j * 5 + 3]) +
                              (new_j - centers[j * 5 + 4]) * (new_j - centers[j * 5 + 4])));

                centers[j * 5] = (unsigned char)(new_r * 255);     // de-normalize
                centers[j * 5 + 1] = (unsigned char)(new_g * 255); // de-normalize
                centers[j * 5 + 2] = (unsigned char)(new_b * 255); // de-normalize
                centers[j * 5 + 3] = new_i;
                centers[j * 5 + 4] = new_j;

                if (shift > max_center_shift)
                {
                    max_center_shift = shift;
                }
            }
            else
            {
                int random_index = rand() % N;
                centers[j * 5] = imageData[random_index * 3];
                centers[j * 5 + 1] = imageData[random_index * 3 + 1];
                centers[j * 5 + 2] = imageData[random_index * 3 + 2];
                centers[j * 5 + 3] = random_index / width; // random row
                centers[j * 5 + 4] = random_index % width; // random column
                printf("Reinitialized center %d\n", j);
            }
        }

        // stopping conditions
        if (stop_condition == 2)
        { // pixel assignment stability
            float fraction_changed = (float)changes / N;
            if (fraction_changed < 0.01)
            {
                printf("Converged based on assignment stability after %d iterations.\n", i + 1);
                break;
            }
        }
        else
        { // center shift
            if (max_center_shift < threshold)
            {
                printf("Converged based on center shift after %d iterations.\n", i + 1);
                break;
            }
        }
    }

    // update pixel values based on the final cluster centers
    for (int idx = 0; idx < N; idx++)
    {
        int cluster = assign[idx];
        imageData[idx * 3] = centers[cluster * 5];
        imageData[idx * 3 + 1] = centers[cluster * 5 + 1];
        imageData[idx * 3 + 2] = centers[cluster * 5 + 2];
    }

    free(centers);
    free(assign);
    free(counts);
    free(new_centers);
}

void save_image(unsigned char *imageData, int width, int height, const char *filename)
{
    FILE *outputFile = fopen(filename, "wb");
    if (outputFile == NULL)
    {
        printf("Error saving output file.\n");
        return;
    }

    fprintf(outputFile, "P6\n%d %d\n255\n", width, height);

    fwrite(imageData, 3, width * height, outputFile);

    fclose(outputFile);

    printf("Segmentation completed. Saved to %s\n", filename);
}
