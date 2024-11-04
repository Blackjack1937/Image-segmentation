typedef unsigned char bit;
typedef unsigned char gray;

char pm_getc(FILE *file);
bit pm_getbit(FILE *file);
unsigned char pm_getrawbyte(FILE *file);
int pm_getint(FILE *file);

void pm_erreur(char *);

// void randomK(int K, int N, unsigned char *imageData, unsigned char *random_K_centers);
void random_K_coords(int K, int width, int height, int *random_K_centers);