#include <stdlib.h>
#include <string.h>
#include "tools/maths.h"
#include "world.h"

#include "tools/vector.h"
#include "entity.h"
#include "controllers/controller.h"
#include "controllers/controller_kinematics.h"
#include "controllers/controller_solid.h"
#include "modelisation/model_3D.h"

struct world
{
    vector_p entities;
};

typedef struct world world_t, *world_p;

world_p world_init()
{
    world_p this = malloc(sizeof(world_t));
    this->entities = vector_init();

    // Instancier des entity
    const int nb_cubes = 6;
    for (int i = 0; i < nb_cubes; ++i)
    {
        entity_p cube = Entity();
        controller_kinematics_p c1 = Controller_kinematics(0., 0., -10., 0., 0., 0.);
        c1->wy = PI * 0.01;
        c1->vz = (randint(0, 1) * 2. - 1.) * 0.02;
        c1->x = randint(-20, 20) * 0.1;
        c1->y = randint(-20, 20) * 0.1;
        c1->z = -randint(10, 20);

        // Ajouter + de vertex pour les faces des côtés
        vertex_t vertex[] = {
            // x, y, z, tex_x, tex_y
            Vertex(-0.5, -0.5, 0.5, 0.334, 0.333),
            Vertex(0.5, -0.5, 0.5, 0.665, 0.333),
            Vertex(0.5, -0.5, -0.5, 0.665, 0.666),
            Vertex(-0.5, -0.5, -0.5, 0.334, 0.666),

            Vertex(-0.5, 0.5, 0.5, 0.333, 0.0),
            Vertex(0.5, 0.5, 0.5, 0.666, 0.0),
            Vertex(0.5, 0.5, -0.5, 0.666, 1.0),
            Vertex(-0.5, 0.5, -0.5, 0.333, 1.0),

            Vertex(-0.5, 0.5, 0.5, 0.666, 0.0),
            Vertex(0.5, 0.5, 0.5, 1.0, 0.0),
            Vertex(0.5, 0.5, -0.5, 1.0, 0.333),
            Vertex(-0.5, 0.5, -0.5, 0.666, 0.333),

            Vertex(-0.5, 0.5, 0.5, 0.0, 0.333),
            Vertex(0.5, 0.5, 0.5, 1.0, 0.333),
            Vertex(0.5, 0.5, -0.5, 1.0, 0.666),
            Vertex(-0.5, 0.5, -0.5, 0.0, 0.666)};

        int nb_vertex = sizeof(vertex) / sizeof(vertex_t);

        vertex_t *vertex2 = malloc(sizeof(vertex));
        memcpy(vertex2, vertex, sizeof(vertex));

        unsigned int indices[] = {
            1, 2, 0, 2, 3, 0,     // 1
            12, 0, 3, 12, 3, 15,  // 2
            0, 1, 5, 0, 5, 4,     // 3
            3, 2, 6, 3, 6, 7,     // 4
            1, 2, 13, 2, 14, 13,  // 5
            8, 9, 10, 8, 10, 11}; // 6

        int nb_indices = sizeof(indices) / sizeof(unsigned int);

        unsigned int *indices2 = malloc(sizeof(indices));
        memcpy(indices2, indices, sizeof(indices));

        model_3D_p cube3d = Model_3D(nb_vertex, vertex2, nb_indices, indices2);
        controller_solid_p c2 = Controller_solid(cube3d);

        entity_add_controller(cube, (controller_p)c1);
        entity_add_controller(cube, (controller_p)c2);
        vector_append(this->entities, (void *)cube);
    }

    return this;
}

void world_process_input(world_p this)
{
    const int n = vector_len(this->entities);
    for (int i = 0; i < n; ++i)
    {
        entity_p e = (entity_p)(vector_get_at(this->entities, i));
        e->process_input(e);
    }
}

void world_update(world_p this)
{
    const int n = vector_len(this->entities);
    for (int i = 0; i < n; ++i)
    {
        entity_p e = (entity_p)(vector_get_at(this->entities, i));
        e->update(e);
    }
}

void world_draw(world_p this)
{
    const int n = vector_len(this->entities);
    for (int i = 0; i < n; ++i)
    {
        entity_p e = (entity_p)(vector_get_at(this->entities, i));
        e->draw(e);
    }
}