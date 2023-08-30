#include "vertex.h"

vertex_t Vertex(float x, float y, float z, float r, float g, float b, float n_x, float n_y, float n_z)
{
    vertex_t this = {{x, y, z, r, g, b, n_x, n_y, n_z}};
    return this;
}