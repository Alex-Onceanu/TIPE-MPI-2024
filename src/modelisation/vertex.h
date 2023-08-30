#ifndef VERTEX_H
#define VERTEX_H

#include "../tools/constantes.h"

// Pour le moment les vertex sont identiques et fixes
// Cette structure est litteralement un float[] (que demander de plus ?)

// Sommet d'un polygone (d'un triangle, en fait) d'un modele 3d
// Destiné à être alloué sur la pile
typedef struct vertex
{
    // x, y, z, r, g, b, normale_x, normale_y, normale_z
    float data[NB_ATTRIBUTES_VERTEX];
} vertex_t;

// "Constructeur" (mais sur la pile...)
// (x, y, z) sont des coordonnées LOCALES par rapport au "centre de l'objet" (0, 0, 0)
// (n_x, n_y, n_z) sont les coordonnees de vecteur normal en ce sommet
vertex_t Vertex(float x, float y, float z, float r, float g, float b, float n_x, float n_y, float n_z);

#endif