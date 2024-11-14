typedef unsigned char bit;
typedef unsigned char gray;

char pm_getc(FILE *file);
bit pm_getbit(FILE *file);
unsigned char pm_getrawbyte(FILE *file);
int pm_getint(FILE *file);
void pm_erreur(char *error_message);

void initialize_centers(int K, int N, unsigned char *imageData, unsigned char *centers);
void kmeans_segmentation(unsigned char *imageData, int width, int height, int K);
void save_image(unsigned char *imageData, int width, int height);
void get_user_centers(int K, unsigned char *centers);