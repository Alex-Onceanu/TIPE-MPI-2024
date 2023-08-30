#ifndef MATERIAU_H
#define MATERIAU_H

// Un materiau est défini comme 3 vecteurs caractérisant l'intensité des 3 composantes
// du modèle de lumière de Phong, ainsi qu'un exposant de "brillance" pour le reflet
typedef struct {
    float ambient[3];
    float diffuse[3];
    float specular[3];

    float shininess;
} materiau_t;

typedef enum MATERIAU_TYPE {
    FER,
    BRIQUE,
    SABLE,
    SOLEIL // La lumiere ne fait RIEN sur un soleil (à part la couleur)
} MATERIAU_TYPE_t;

// "Constructeur" sur la pile
materiau_t Materiau(MATERIAU_TYPE_t type);

#endif