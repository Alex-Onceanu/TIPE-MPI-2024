#ifndef MODEL_3D_H
#define MODEL_3D_H

#include "vertex.h"

typedef struct model_3D
{
    unsigned int nb_vertex;
    vertex_t *vertex_buffer;

    unsigned int nb_index;
    unsigned int *index_buffer;
} model_3D_t, *model_3D_p;

// Vertex buffer et Index buffer doivent etre alloués aupréalablement sur le tas
model_3D_p Model_3D(unsigned int __nb_vertex, vertex_t *__vertex_buffer, unsigned int __nb_index, unsigned int *__index_buffer);

// Lance les glDrawElements, glbindbuffer et tout ce qu'il faut
// Techniquement dessine au centre 0, 0, 0, donc il FAUT faire une translation depuis O vers x, y, z)
void model_3D_draw(model_3D_p this);

#endif