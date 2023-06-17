#ifndef READER_H
#define READER_H

// Lit un fichier P6 dans un tableau de pixels
unsigned char *read_ppm(const char *filename, int *width, int *height);

#endif