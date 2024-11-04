#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <math.h>

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
        } while (ch != '\n' && ch != '\r');
    }

    return ch;
}

bit pm_getbit(FILE *file)
{
    char ch;

    do
    {
        ch = pm_getc(file);
    } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

    if (ch != '0' && ch != '1')
        pm_erreur("read error : != [0|1]");

    return (ch == '1') ? 1 : 0;
}

unsigned char pm_getrawbyte(FILE *file)
{
    int iby;

    iby = getc(file);
    if (iby == EOF)
        pm_erreur("EOF / read error ");
    return (unsigned char)iby;
}

int pm_getint(FILE *file)
{
    char ch;
    int i;

    do
    {
        ch = pm_getc(file);
    } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

    if (ch < '0' || ch > '9')
        pm_erreur("read error : != [0-9]");

    i = 0;
    do
    {
        i = i * 10 + ch - '0';
        ch = pm_getc(file);
    } while (ch >= '0' && ch <= '9');

    return i;
}

void pm_erreur(char *texte)
{
    fprintf(stderr, "\n%s \n\n", texte);
    exit(1);
}

// void randomK(int K, int N, unsigned char *imageData, unsigned char *random_K_centers)
// {
//     for (int i = 0; i < K; i++)
//     {
//         int random = (float)rand() / RAND_MAX * N;
//         // int random2 = rand() % N;
//         printf("%d \n", random);
//         // printf("%d \n", random2);
//         random_K_centers[i * 3] = imageData[random * 3];
//         random_K_centers[i * 3 + 1] = imageData[random * 3 + 1];
//         random_K_centers[i * 3 + 2] = imageData[random * 3 + 2];
//     }
// }

void random_K_coords(int K, int width, int height, int *random_K_centers)
{
    for (int i = 0; i < K; i++)
    {
        int random_index = rand() % (width * height);
        int x = random_index % width;
        int y = random_index / width;

        random_K_centers[i * 2] = x;
        random_K_centers[i * 2 + 1] = y;

        printf("Center %d: (x, y) = (%d, %d)\n", i, x, y);
    }
}

float **calculateDistances(int width, int height, int K, int *random_K_centers)
{
    int N = width * height;
    float **distances = (float **)malloc(N * sizeof(float *));
    for (int i = 0; i < N; i++)
    {
        distances[i] = (float *)malloc(K * sizeof(float));
    }

    for (int i = 0; i < N; i++)
    {
        int x_pixel = i % width;
        int y_pixel = i / width;

        for (int j = 0; j < K; j++)
        {
            int x_center = random_K_centers[j * 2];
            int y_center = random_K_centers[j * 2 + 1];
            float distance = sqrt(pow(x_pixel - x_center, 2) + pow(y_pixel - y_center, 2));
            distances[i][j] = distance;
        }
    }

    return distances;
}
