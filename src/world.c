#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tools/maths.h"
#include "world.h"

#include "tools/vector.h"
#include "entity.h"
#include "controllers/controller.h"
#include "controllers/controller_kinematics.h"
#include "controllers/controller_solid.h"
#include "controllers/controller_camera.h"
#include "modelisation/model_3D.h"
#include "user_event.h"

struct world
{
    vector_p entities;
    vector_p events;
};

typedef struct world world_t, *world_p;

world_p world_init()
{
    world_p this = malloc(sizeof(world_t));
    this->entities = vector_init();
    this->events = vector_init();

    // Instancier la camera
    entity_p cam = Entity();
    controller_camera_p ccam = Controller_camera(0.0, 1.0, 0.0);
    entity_add_controller(cam, (controller_p)ccam);
    vector_append(this->entities, (void *)cam);

    // for (float z = -20.0; z <= 20.1; z += 20.0)
    // {
    //     for (float y = -20.0; y <= 20.1; y += 20.0)
    //     {
    //         for (float x = -20.0; x <= 20.1; x += 20.0)
    //         {
    //             if (x >= -10.0 && x <= 10.0 && y <= -10.0 && y >= 10.0 && z >= -10.0 && z <= 10.0)
    //                 continue;
    //             entity_p e = Entity();
    //             controller_kinematics_p ck = Controller_kinematics(x, y, z, 0.0, 0.0, 0.0);

    //             model_3D_p cube3d = Cube(2.0, x / 41.0, y / 41.0, z / 41.0);
    //             controller_solid_p cs = Controller_solid(cube3d);

    //             entity_add_controller(e, (controller_p)ck);
    //             entity_add_controller(e, (controller_p)cs);
    //             vector_append(this->entities, (void *)e);
    //         }
    //     }
    // }

    {
        entity_p e = Entity();
        controller_kinematics_p ck = Controller_kinematics(0.0, 0.0, -35.0, 0.0, 0.0, 0.0);

        model_3D_p cube3d = Sphere(1.0);
        controller_solid_p cs = Controller_solid(cube3d);

        entity_add_controller(e, (controller_p)ck);
        entity_add_controller(e, (controller_p)cs);
        vector_append(this->entities, (void *)e);
    }

    {
        entity_p e = Entity();
        controller_kinematics_p ck = Controller_kinematics(-5.0, 0.5, -35.0, 0.0, 0.0, 0.0);
        ck->wy = PI / 60.0;
        ck->wz = PI / 120.0;

        model_3D_p cube3d = Cube(2.0, 0.3, 0.3, 0.6);
        controller_solid_p cs = Controller_solid(cube3d);

        entity_add_controller(e, (controller_p)ck);
        entity_add_controller(e, (controller_p)cs);
        vector_append(this->entities, (void *)e);
    }

    {
        entity_p e = Entity();
        controller_kinematics_p ck = Controller_kinematics(5.0, 0.0, -35.0, 0.0, 0.0, 0.0);

        model_3D_p cube3d = Pave(2.0, 5.0, 2.0, 0.6, 0.6, 0.3);
        controller_solid_p cs = Controller_solid(cube3d);

        entity_add_controller(e, (controller_p)ck);
        entity_add_controller(e, (controller_p)cs);
        vector_append(this->entities, (void *)e);
    }

    // Le sol est un pavé
    entity_p sol = Entity();
    controller_kinematics_p c1 = Controller_kinematics(0.0, -2.0, 0.0, 0., 0., 0.);

    model_3D_p pav3d = Pave(100.0, 2.0, 100.0, 0.88, 0.803, 0.6627);
    assert(pav3d != NULL);
    controller_solid_p c2 = Controller_solid(pav3d);

    entity_add_controller(sol, (controller_p)c1);
    entity_add_controller(sol, (controller_p)c2);
    vector_append(this->entities, (void *)sol);

    return this;
}

void world_add_event(world_p this, user_event_t e_t)
{
    // Il faut allouer l'event sur le tas pour pouvoir l'ajouter au vecteur..
    user_event_p e_p = malloc(sizeof(user_event_t));
    e_p->type = e_t.type;
    e_p->data = e_t.data;

    vector_append(this->events, (void *)e_p);
}

void world_process_input(world_p this)
{
    const int n = vector_len(this->entities);
    for (int i = 0; i < n; ++i)
    {
        entity_p e = (entity_p)(vector_get_at(this->entities, i));
        e->process_input(e, (void *)(this->events));
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