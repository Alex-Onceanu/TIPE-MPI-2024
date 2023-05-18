#include "vertex.h"

vertex_t Vertex(float x, float y, float z, float tex_x, float tex_y)
{
    vertex_t this = {{x, y, z, tex_x, tex_y}};
    return this;
}