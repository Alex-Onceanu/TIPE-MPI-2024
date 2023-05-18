#ifndef VERTEX_H
#define VERTEX_H

#include "../tools/constantes.h"

// Pour le moment les vertex sont identiques et fixes
// Cette structure est litteralement un float[] (que demander de plus ?)

// Sommet d'un polygone (d'un triangle, en fait) d'un modele 3d
// Destiné à être alloué sur la pile
typedef struct vertex
{
    // x, y, z, tex_x, tex_y
    float data[NB_ATTRIBUTES_VERTEX];
} vertex_t;

// "Constructeur" (mais sur la pile...)
vertex_t Vertex(float x, float y, float z, float tex_x, float tex_y);

#endif