#include <stdlib.h>
#include <stdio.h>

#include "init.h"
#include "model_3D.h"

#include <GLES2/gl2.h>

model_3D_p Model_3D(unsigned int __nb_vertex, vertex_t *__vertex_buffer, unsigned int __nb_index, unsigned int *__index_buffer)
{
    model_3D_p this = malloc(sizeof(model_3D_t));

    this->nb_vertex = __nb_vertex;
    this->vertex_buffer = __vertex_buffer;
    this->nb_index = __nb_index;
    this->index_buffer = __index_buffer;

    return this;
}

void model_3D_draw(model_3D_p this)
{
    // Vertex buffer, on envoie à OpenGL les données du triangle
    unsigned int vertex_buf_id;
    glGenBuffers(1, &vertex_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf_id);
    glBufferData(GL_ARRAY_BUFFER, this->nb_vertex * NB_ATTRIBUTES_VERTEX * sizeof(float), this->vertex_buffer->data, GL_DYNAMIC_DRAW);

    // Puis on lui dit comment interpréter ces données
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NB_ATTRIBUTES_VERTEX * sizeof(float), 0);
    // Texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, NB_ATTRIBUTES_VERTEX * sizeof(float), (void *)(3 * sizeof(float)));

    // Bind et interprétation du index_buffer
    unsigned int index_buf_id;
    glGenBuffers(1, &index_buf_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->nb_index * sizeof(unsigned int), this->index_buffer, GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLES, this->nb_index, GL_UNSIGNED_INT, NULL);
}