#include "vertex.h"

vertex_t Vertex(float x, float y, float z, float r, float g, float b)
{
    vertex_t this = {{x, y, z, r, g, b}};
    return this;
}