#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "init.h"
#include "model_3D.h"
#include "vertex.h"
#include "../tools/constantes.h"
#include "materiau.h"

#include <GLES2/gl2.h>

model_3D_p Model_3D(unsigned int __nb_vertex, vertex_t *__vertex_buffer, unsigned int __nb_index, unsigned int *__index_buffer, const char* cube_images[6])
{
    model_3D_p this = malloc(sizeof(model_3D_t));

    this->nb_vertex = __nb_vertex;
    this->vertex_buffer = __vertex_buffer;
    this->nb_index = __nb_index;
    this->index_buffer = __index_buffer;

    if(cube_images == NULL) this->cubemap_id = NO_TEXTURE;
    else this->cubemap_id = init_cubemap(cube_images);

    return this;
}

model_3D_p Sphere(double rayon, double __r, double __g, double __b)
{
    // On génère une sphère comme l'ensemble des points à une distance r du point (0, 0, 0)
    // Donc on fait 2 boucles d'angles (avec des angles en degrés)
    const int pas = 10; // 2 <= pas <= 45
    int nb_vertex_par_latitude = 360.0 / pas;

    const int nb_vertex = (nb_vertex_par_latitude - 2) * nb_vertex_par_latitude;
    vertex_t *vertex_buffer = malloc(sizeof(vertex_t) * nb_vertex);

    const int nb_index = (nb_vertex_par_latitude - 2) * nb_vertex_par_latitude * 6;
    unsigned int *index_buffer = malloc(sizeof(unsigned int) * nb_index);

    int i = 0;
    double theta_rd = 0.;
    double phi_rd = 0.;
    // D'abord on génère tous les sommets de la sphère sur lesquels reposeront les triangles

    // Pôles nord et sud de la sphère
    vertex_buffer[0] = Vertex(0.0, -1.0 * rayon, 0.0, 0.5, 0.0, 0.5, 0.0, -1.0, 0.0);
    vertex_buffer[nb_vertex - 1] = Vertex(0.0, 1.0 * rayon, 0.0, 0.5, 1.0, 0.5, 0.0, 1.0, 0.0);

    // Toutes les autres possibilités de points à une distance r du centre
    for (int theta = -180 + pas; theta < 180 - pas; theta += pas)
    {
        theta_rd = (theta * 2.0 * PI) / 360.0;
        for (int phi = 0; phi < 360; phi += pas)
        {
            phi_rd = (phi * 2.0 * PI) / 360.0;
            // On a deux angles et un rayon : on déduit les coordonnées cartésiennes à partir des sphériques

            // normale = normaliser(position) parce que c'est une sphere
            vertex_buffer[i] = Vertex(
                rayon * sin(phi_rd) * cos(theta_rd),
                rayon * sin(phi_rd) * sin(theta_rd),
                rayon * cos(phi_rd),
                __r, __g, __b,
                sin(phi_rd) * cos(theta_rd),
                sin(phi_rd) * sin(theta_rd),
                cos(phi_rd));
            i++;
        }
    }
    printf("i = %d, nb_vertex = %d\n", i, nb_vertex);
    assert(i == nb_vertex);

    // Reste à "trianguler" : on prend les sommets adjacents 4 par 4 et on dessine 2 triangles par rectangle
    // (Donc on veut juste remplir le index_buffer)

    // D'abord les sommets connectés au pôle nord
    int j = 0;
    for (int k = 1; k < nb_vertex_par_latitude; ++k)
    {
        index_buffer[j++] = 0;
        index_buffer[j++] = k;
        index_buffer[j++] = k + 1;
    }
    // Dernier triangle qui lie le dernier du contour au début...
    index_buffer[j++] = 0;
    index_buffer[j++] = nb_vertex_par_latitude;
    index_buffer[j++] = 1;

    // Puis tous les autres

    int a, b, c, d;
    for (int latitude = 2; latitude < nb_vertex_par_latitude - 1; latitude++)
    {
        // a -- b
        // |   /|
        // |  / |
        // | /  |
        // |/   |
        // c -- d
        // On dessine a->c->b et c->d->b
        for (int longitude = 0; longitude < nb_vertex_par_latitude - 1; longitude++)
        {
            a = (latitude - 1) * nb_vertex_par_latitude + longitude;
            b = (latitude - 1) * nb_vertex_par_latitude + (longitude + 1);
            c = latitude * nb_vertex_par_latitude + longitude;
            d = latitude * nb_vertex_par_latitude + longitude + 1;

            index_buffer[j++] = a;
            index_buffer[j++] = c;
            index_buffer[j++] = b;
            index_buffer[j++] = c;
            index_buffer[j++] = d;
            index_buffer[j++] = b;
        }
        // Reste à connecter la fin du tour avec le début (pour bien avoir un contour)
        a = (latitude - 1) * nb_vertex_par_latitude + nb_vertex_par_latitude - 1;
        b = (latitude - 1) * nb_vertex_par_latitude;
        c = latitude * nb_vertex_par_latitude + nb_vertex_par_latitude - 1;
        d = latitude * nb_vertex_par_latitude;

        index_buffer[j++] = a;
        index_buffer[j++] = c;
        index_buffer[j++] = b;
        index_buffer[j++] = c;
        index_buffer[j++] = d;
        index_buffer[j++] = b;
    }

    // Enfin les sommets connectés au pôle sud
    int offset = nb_vertex - 1 - nb_vertex_par_latitude - 1;
    for (int k = offset + 1; k < nb_vertex_par_latitude + offset; ++k)
    {
        index_buffer[j++] = k;
        index_buffer[j++] = nb_vertex - 1;
        index_buffer[j++] = k + 1;
    }
    index_buffer[j++] = nb_vertex_par_latitude + offset;
    index_buffer[j++] = nb_vertex - 1;
    index_buffer[j++] = offset;

    printf("j = %d, nb_index = %d\n", j, nb_index);
    assert(j == nb_index);

    return Model_3D(nb_vertex, vertex_buffer, nb_index, index_buffer, NULL);
}

model_3D_p Pave(double width, double height, double depth, double r, double g, double b, const char* cube_images[6])
{
    double w = width / 2.0;
    double h = height / 2.0;
    double d = depth / 2.0;

    vertex_t vertex[] = {
        // x, y, z, r, g, b, normale.x, normale.y, normale.z
        Vertex(w, -h, d, r, g, b, 0.0, -1.0, 0.0),
        Vertex(w, -h, -d, r, g, b, 0.0, -1.0, 0.0),
        Vertex(-w, -h, d, r, g, b, 0.0, -1.0, 0.0),
        Vertex(w, -h, -d, r, g, b, 0.0, -1.0, 0.0),
        Vertex(-w, -h, -d, r, g, b, 0.0, -1.0, 0.0),
        Vertex(-w, -h, d, r, g, b, 0.0, -1.0, 0.0),

        Vertex(-w, h, d, r, g, b, -1.0, 0.0, 0.0),
        Vertex(-w, -h, d, r, g, b, -1.0, 0.0, 0.0),
        Vertex(-w, -h, -d, r, g, b, -1.0, 0.0, 0.0),
        Vertex(-w, h, d, r, g, b, -1.0, 0.0, 0.0),
        Vertex(-w, -h, -d, r, g, b, -1.0, 0.0, 0.0),
        Vertex(-w, h, -d, r, g, b, -1.0, 0.0, 0.0),

        Vertex(-w, -h, d, r, g, b, 0.0, 0.0, 1.0),
        Vertex(w, -h, d, r, g, b, 0.0, 0.0, 1.0),
        Vertex(w, h, d, r, g, b, 0.0, 0.0, 1.0),
        Vertex(-w, -h, d, r, g, b, 0.0, 0.0, 1.0),
        Vertex(w, h, d, r, g, b, 0.0, 0.0, 1.0),
        Vertex(-w, h, d, r, g, b, 0.0, 0.0, 1.0),

        Vertex(-w, -h, -d, r, g, b, 0.0, 0.0, -1.0),
        Vertex(w, -h, -d, r, g, b, 0.0, 0.0, -1.0),
        Vertex(w, h, -d, r, g, b, 0.0, 0.0, -1.0),
        Vertex(-w, -h, -d, r, g, b, 0.0, 0.0, -1.0),
        Vertex(w, h, -d, r, g, b, 0.0, 0.0, -1.0),
        Vertex(-w, h, -d, r, g, b, 0.0, 0.0, -1.0),

        Vertex(w, -h, d, r, g, b, 1.0, 0.0, 0.0),
        Vertex(w, -h, -d, r, g, b, 1.0, 0.0, 0.0),
        Vertex(w, h, d, r, g, b, 1.0, 0.0, 0.0),
        Vertex(w, -h, -d, r, g, b, 1.0, 0.0, 0.0),
        Vertex(w, h, -d, r, g, b, 1.0, 0.0, 0.0),
        Vertex(w, h, d, r, g, b, 1.0, 0.0, 0.0),

        Vertex(-w, h, d, r, g, b, 0.0, 1.0, 0.0),
        Vertex(w, h, d, r, g, b, 0.0, 1.0, 0.0),
        Vertex(w, h, -d, r, g, b, 0.0, 1.0, 0.0),
        Vertex(-w, h, d, r, g, b, 0.0, 1.0, 0.0),
        Vertex(w, h, -d, r, g, b, 0.0, 1.0, 0.0),
        Vertex(-w, h, -d, r, g, b, 0.0, 1.0, 0.0)};

    int nb_vertex = sizeof(vertex) / sizeof(vertex_t);

    vertex_t *vertex2 = malloc(sizeof(vertex));
    memcpy(vertex2, vertex, sizeof(vertex));

    return Model_3D(nb_vertex, vertex2, 0, NULL, cube_images);
}

model_3D_p Cube(double c, double r, double g, double b, const char* cube_images[6])
{
    return Pave(c, c, c, r, g, b, cube_images);
}

void model_3D_draw(model_3D_p this, materiau_t materiau, unsigned int program_index)
{
    // D'abord on set le program qui sera utilisé pour le rendu de cet objet
    const unsigned int program = PROGRAM_ID[program_index];
    glUseProgram(program);
    // printf("prog : %d\n", program);

    if(this->cubemap_id != NO_TEXTURE)
    {
        // On set la texture de this comme texture active
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubemap_id);
        unsigned int u_Cubemap = glGetUniformLocation(program, "u_Cubemap");
        glUniform1i(u_Cubemap, 0);

        // Clean
        // glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }


    // On set la constante du materiau de l'objet pour le rendu
    int u_ambient = glGetUniformLocation(program, "u_Material.ambient");
    glUniform3f(u_ambient, materiau.ambient[0], materiau.ambient[1], materiau.ambient[2]);
    int u_diffuse = glGetUniformLocation(program, "u_Material.diffuse");
    glUniform3f(u_diffuse, materiau.diffuse[0], materiau.diffuse[1], materiau.diffuse[2]);
    int u_specular = glGetUniformLocation(program, "u_Material.specular");
    glUniform3f(u_specular, materiau.specular[0], materiau.specular[1], materiau.specular[2]);
    int u_shininess = glGetUniformLocation(program, "u_Material.shininess");
    glUniform1f(u_shininess, materiau.shininess);

    // Vertex buffer, on envoie à OpenGL les données du triangle
    unsigned int vertex_buf_id;
    glGenBuffers(1, &vertex_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf_id);
    glBufferData(GL_ARRAY_BUFFER, 2 * this->nb_vertex * NB_ATTRIBUTES_VERTEX * sizeof(float), this->vertex_buffer->data, GL_DYNAMIC_DRAW); // a revoir

    // Puis on lui dit comment interpréter ces données
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NB_ATTRIBUTES_VERTEX * sizeof(float), 0);
    // Couleur
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, NB_ATTRIBUTES_VERTEX * sizeof(float), (void *)(3 * sizeof(float)));
    // Normales
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, NB_ATTRIBUTES_VERTEX * sizeof(float), (void*)(6 * sizeof(float)));

    if(this->nb_index > 0)
    {
        // Bind et interprétation du index_buffer
        unsigned int index_buf_id;
        glGenBuffers(1, &index_buf_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buf_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->nb_index * sizeof(unsigned int), this->index_buffer, GL_DYNAMIC_DRAW);
        glDrawElements(GL_TRIANGLES, this->nb_index, GL_UNSIGNED_INT, NULL);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, this->nb_vertex);
    }
}