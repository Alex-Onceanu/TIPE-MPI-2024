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

#include <GLES3/gl3.h>


// Précision du modèle 3D de la sphère, un petit pas donnera une sphère bien lisse mais avec beaucoup de vertex
// 2 <= SPHERE_STEP <= 45
#define SPHERE_STEP 10

// _____________________________________________Init OpenGL buffers____________________________________________________________________________

vertex_t* init_vertex_buffer_pave(unsigned int *nb_vertex, double width, double height, double depth, double r, double g, double b)
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

    unsigned int tmp_nb_vertex = sizeof(vertex) / sizeof(vertex_t);
    *nb_vertex = tmp_nb_vertex;

    vertex_t *ans = malloc(sizeof(vertex));
    memcpy(ans, vertex, sizeof(vertex));

    return ans;
}

const void* init_vertex_buffer_pave_data(unsigned int *nb_vertex, double width, double height, double depth, double r, double g, double b)
{
    vertex_t *vertex = init_vertex_buffer_pave(nb_vertex, width, height, depth, r, g, b);
    return vertex->data;
}

vertex_t* init_vertex_buffer_sphere(unsigned int *nb_vertex, double rayon, double r, double g, double b)
{
    // On génère une sphère comme l'ensemble des points à une distance rayon du point (0, 0, 0)
    // Donc on fait 2 boucles d'angles (avec des angles en degrés)
    int nb_vertex_par_latitude = 360.0 / SPHERE_STEP;

    *nb_vertex = (nb_vertex_par_latitude - 2) * nb_vertex_par_latitude;
    vertex_t *vertex_buffer = malloc(sizeof(vertex_t) * *nb_vertex);

    int i = 0;
    double theta_rd = 0.;
    double phi_rd = 0.;
    // D'abord on génère tous les sommets de la sphère sur lesquels reposeront les triangles

    // Pôles nord et sud de la sphère
    vertex_buffer[0] = Vertex(0.0, -1.0 * rayon, 0.0, 0.5, 0.0, 0.5, 0.0, -1.0, 0.0);
    vertex_buffer[*nb_vertex - 1] = Vertex(0.0, 1.0 * rayon, 0.0, 0.5, 1.0, 0.5, 0.0, 1.0, 0.0);

    // Toutes les autres possibilités de points à une distance rayon du centre
    for (int theta = -180 + SPHERE_STEP; theta < 180 - SPHERE_STEP; theta += SPHERE_STEP)
    {
        theta_rd = (theta * 2.0 * PI) / 360.0;
        for (int phi = 0; phi < 360; phi += SPHERE_STEP)
        {
            phi_rd = (phi * 2.0 * PI) / 360.0;
            // On a deux angles et un rayon : on déduit les coordonnées cartésiennes à partir des sphériques

            // normale = normaliser(position) parce que c'est une sphere
            vertex_buffer[i] = Vertex(
                rayon * sin(phi_rd) * cos(theta_rd),
                rayon * sin(phi_rd) * sin(theta_rd),
                rayon * cos(phi_rd),
                r, g, b,
                sin(phi_rd) * cos(theta_rd),
                sin(phi_rd) * sin(theta_rd),
                cos(phi_rd));
            i++;
        }
    }
    return vertex_buffer;
}

const void* init_vertex_buffer_sphere_data(unsigned int *nb_vertex, double rayon, double r, double g, double b)
{
    vertex_t *ans = init_vertex_buffer_sphere(nb_vertex, rayon, r, g, b);
    return ans->data;
}

unsigned int* init_index_buffer_sphere(unsigned int *nb_index)
{
    int nb_vertex_par_latitude = 360.0 / SPHERE_STEP;

    const int nb_vertex = (nb_vertex_par_latitude - 2) * nb_vertex_par_latitude;
    *nb_index = (nb_vertex_par_latitude - 2) * nb_vertex_par_latitude * 6;
    unsigned int *index_buffer = malloc(sizeof(unsigned int) * *nb_index);

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

    return index_buffer;
}


// _______________________________________Model_3D____________________________________

// Crise existentielle sur l'intérêt de cette structure
// à corriger ASAP

// model_3D_t Model_3D(unsigned int __nb_vertex, vertex_t *__vertex_buffer, unsigned int __nb_index, unsigned int *__index_buffer, const char* cube_images[6])
// {
//     model_3D_t this = malloc(sizeof(model_3D_t));

//     this->nb_vertex = __nb_vertex;
//     this->vertex_buffer = __vertex_buffer;
//     this->nb_index = __nb_index;
//     this->index_buffer = __index_buffer;

//     if(cube_images == NULL) this->cubemap_id = NO_TEXTURE;
//     else this->cubemap_id = init_cubemap(cube_images);

//     return this;
// }

// model_3D_t Sphere(double rayon, double __r, double __g, double __b)
// {
//     unsigned int nb_vertex;
//     vertex_t *vertex_buffer = init_vertex_buffer_sphere(&nb_vertex, rayon, r, g, b);

//     unsigned int nb_index;
//     unsigned int *index_buffer = init_index_buffer_sphere(&nb_index, rayon, r, g, b);

//     return Model_3D(nb_vertex, vertex_buffer, nb_index, index_buffer, NULL);
// }

// model_3D_t Pave(double width, double height, double depth, double r, double g, double b, const char* cube_images[6])
// {
//     unsigned int nb_vertex;
//     vertex_t* vertex = init_vertex_buffer_pave(&nb_vertex, width, height, depth, r, g, b);
    
//     return Model_3D(nb_vertex, vertex, 0, NULL, cube_images);
// }

// model_3D_t Cube(double c, double r, double g, double b, const char* cube_images[6])
// {
//     return Pave(c, c, c, r, g, b, cube_images);
// }

void model_3D_draw(model_3D_t this, materiau_t materiau, unsigned int program_index)
{
    // D'abord on set le program qui sera utilisé pour le rendu de cet objet
    const unsigned int program = PROGRAM_ID[program_index];
    glUseProgram(program);

    if(this.texture_id != NO_TEXTURE)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this.texture_id);

        unsigned int u_Texture = glGetUniformLocation(program, "u_Texture");
        glUniform1i(u_Texture, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VERTEX_BUFFER_ID[this.model_id]);
    
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


    // On set la constante du materiau de l'objet pour le rendu
    int u_ambient = glGetUniformLocation(program, "u_Material.ambient");
    glUniform3f(u_ambient, materiau.ambient[0], materiau.ambient[1], materiau.ambient[2]);
    int u_diffuse = glGetUniformLocation(program, "u_Material.diffuse");
    glUniform3f(u_diffuse, materiau.diffuse[0], materiau.diffuse[1], materiau.diffuse[2]);
    int u_specular = glGetUniformLocation(program, "u_Material.specular");
    glUniform3f(u_specular, materiau.specular[0], materiau.specular[1], materiau.specular[2]);
    int u_shininess = glGetUniformLocation(program, "u_Material.shininess");
    glUniform1f(u_shininess, materiau.shininess);

    // Lumière : c'est une position, une couleur et une intensité de lumière ambientale.
    int u_Light = glGetUniformLocation(program, "u_Light");
    glUniform3f(u_Light, SUN_X, SUN_Y, SUN_Z);
    int u_LightColor = glGetUniformLocation(program, "u_LightColor");
    glUniform3f(u_LightColor, LIGHT_COLOR[0], LIGHT_COLOR[1], LIGHT_COLOR[2]);
    int u_AmbientIntensity = glGetUniformLocation(program, "u_AmbientIntensity");
    glUniform1f(u_AmbientIntensity, AMBIENT_INTENSITY);
    
    if(program_index == COLOR_PROGRAM)
    {
        int u_Reflectivity = glGetUniformLocation(program, "u_Reflectivity");
        glUniform1f(u_Reflectivity, BALL_REFLECTIVITY);
    }

    
    if(NB_INDEX_PER_BUFFER[this.model_id] > 0)
    {
        // Bind et interprétation du index_buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, INDEX_BUFFER_ID[this.model_id]);
        glDrawElements(GL_TRIANGLES, NB_INDEX_PER_BUFFER[this.model_id], GL_UNSIGNED_INT, NULL);
        // unbind buffer ?
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, NB_VERTEX_PER_BUFFER[this.model_id]);
    }
}

