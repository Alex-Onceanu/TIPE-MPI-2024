#include <stdlib.h>
#include <stdio.h>

#include "reader.h"

void echange(unsigned char *img, int i, int j)
{
    unsigned char tmp = img[i];
    img[i] = img[j];
    img[j] = tmp;
}

// Renverse une image verticalement (l'image étant un tableau de pixels (RGB sans alpha))
void vertical_flip(unsigned char *img, int width, int height)
{
    for (int y = 0; y < (height / 2); y++)
    {
        for (int x = 0; x < width; x += 3)
        {
            echange(img, width * y + x + 0, width * (height - y - 1) + x + 0);
            echange(img, width * y + x + 1, width * (height - y - 1) + x + 1);
            echange(img, width * y + x + 2, width * (height - y - 1) + x + 2);
        }
    }
}

unsigned char *read_ppm(const char *filename_ppm, int *width, int *height)
{
    FILE *fichier = fopen(filename_ppm, "r");
    if (fichier == NULL || fgetc(fichier) != 'P' || fgetc(fichier) != '6')
    {
        fclose(fichier);
        return NULL;
    }
    fgetc(fichier);
    if(fgetc(fichier) == '#')
    {
        while(fgetc(fichier) != '\n');
    }
    else
    {
        fseek(fichier, -1, SEEK_CUR);
    }

    int max_color = 255;
    if (fscanf(fichier, "%d %d %d", width, height, &max_color) != 3)
    {
        printf("Probleme de lecture du header ppm...");
        fclose(fichier);
        return NULL;
    }
    if (max_color != 255)
    {
        printf("Couleur ref doit etre 255, non ?\n");
        fclose(fichier);
        return NULL;
    }

    printf("Texture chargée : width = %d, height = %d, max_color = %d\n", *width, *height, max_color);

    // Tout le fichier est lu d'un seul coup avec un fread approprié

    int nb_px = (*width) * (*height); // rgb
    unsigned char *res = malloc(3 * nb_px * sizeof(unsigned char));
    fread(res, sizeof(unsigned char), 3 * nb_px, fichier);
    // vertical_flip(res, 3 * *width, *height); // Renverse l'image, sinon elle est lue la tête en bas...

    fclose(fichier);
    return res;
}