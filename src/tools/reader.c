#include <stdlib.h>
#include <stdio.h>

#include "reader.h"

void echange(unsigned char *img, int i, int j)
{
    unsigned char tmp = img[i];
    img[i] = img[j];
    img[j] = tmp;
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

    // Tout le fichier est lu d'un seul coup avec un fread approprié

    int nb_px = (*width) * (*height); // rgb
    unsigned char *res = malloc(3 * nb_px * sizeof(unsigned char));
    fread(res, sizeof(unsigned char), 3 * nb_px, fichier);

    fclose(fichier);
    return res;
}