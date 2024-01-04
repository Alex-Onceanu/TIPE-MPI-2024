#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "tools/maths.h"
#include "world.h"

#include "tools/vector.h"
#include "tools/constantes.h"
#include "entity.h"
#include "controllers/controller.h"
#include "controllers/controller_kinematics.h"
#include "controllers/controller_solid.h"
#include "controllers/controller_camera.h"
#include "modelisation/model_3D.h"
#include "modelisation/materiau.h"
#include "user_event.h"
#include "controllers/physics_manager.h"

struct world
{
    vector_p entities;
    vector_p events;
    controller_camera_p camera;
    physics_manager_p manager;
};

typedef struct world world_t, *world_p;
void lance_boule(world_p);

world_p world_init()
{
    world_p this = malloc(sizeof(world_t));
    this->entities = vector_init();
    this->events = vector_init();
    this->manager = Physics_manager();

    const char *SKYBOX[6] = {"../res/textures/sky/1.ppm", "../res/textures/sky/2.ppm", "../res/textures/sky/3.ppm", "../res/textures/sky/4.ppm", "../res/textures/sky/5.ppm", "../res/textures/sky/6.ppm"};

    this->camera = Controller_camera(0.0, 1.0, 0.0);
    {
        // Instancier la camera
        entity_p cam = Entity(NO_PROGRAM);
        entity_add_controller(cam, (controller_p)this->camera);
        vector_append(this->entities, (void *)cam);
    }

    {
        // On represente la source de lumiere par un "soleil" cubique
        entity_p e = Entity(COLOR_PROGRAM);
        controller_kinematics_p ck = Controller_kinematics(0.0, Force3(SUN_X, SUN_Y, SUN_Z), Force3(0.0, 0.0, 0.0), NULL);

        model_3D_p cube3d = Sphere(0.8, 1.0, 1.0, 1.0);
        controller_solid_p cs = Controller_solid(cube3d, SOLEIL);

        entity_add_controller(e, (controller_p)ck);
        entity_add_controller(e, (controller_p)cs);
        vector_append(this->entities, (void *)e);
    }
    // lance_boule(this);
    {
        // Le sol est un pavé
        entity_p sol = Entity(COLOR_PROGRAM);
        controller_kinematics_p c1 = Controller_kinematics(1000000.0, Force3(0.0, -2.0, 0.0), Force3(0.0, 0.0, 0.0), NULL);

        model_3D_p pav3d = Pave(600.0, 2.0, 600.0, 0.44, 0.401, 0.3313, NULL);
        assert(pav3d != NULL);
        controller_solid_p c2 = Controller_solid(pav3d, SABLE);

        entity_add_controller(sol, (controller_p)c1);
        entity_add_controller(sol, (controller_p)c2);
        vector_append(this->entities, (void *)sol);
    }

    {
        // Le ciel est un cube
        entity_p e = Entity(SKYBOX_PROGRAM);
        controller_kinematics_p ck = Controller_kinematics(0.0, Force3(0.0, 1.0, 0.0), Force3(0.0, 0.0, 0.0), NULL);

        model_3D_p cube3d = Cube(4.0, 1.0, 1.0, 1.0, SKYBOX);
        assert(cube3d != NULL);
        controller_solid_p cs = Controller_solid(cube3d, SOLEIL);

        entity_add_controller(e, (controller_p)ck);
        entity_add_controller(e, (controller_p)cs);
        vector_append(this->entities, (void *)e);
    }

    {
        // Physics manager : c'est une entité n'ayant que son controlleur propre
        entity_p e = Entity(NO_PROGRAM);
        entity_add_controller(e, (controller_p)this->manager);
        vector_append(this->entities, (void *)e);
    }

    return this;
}

void world_add_event(world_p this, user_event_t e_t)
{
    // Il faut allouer l'event sur le tas pour pouvoir l'ajouter au vecteur..
    // TODO : changer le vecteur pour pouvoir ajouter des éléments sans malloc
    user_event_p e_p = malloc(sizeof(user_event_t));
    e_p->type = e_t.type;
    e_p->data = e_t.data;

    vector_append(this->events, (void *)e_p);
}

void lance_boule(world_p this)
{
    entity_p e = Entity(COLOR_PROGRAM);
    // masse réelle d'une boule de petanque : 0,730 kg
    const float mass = 1.0;
    const float v0 = 2.0;

    controller_kinematics_p ck = Controller_kinematics(mass,
                                                       Force3(this->camera->x + this->camera->direction_x * 10.0,
                                                              this->camera->y + this->camera->direction_y * 10.0,
                                                              this->camera->z + this->camera->direction_z * 10.0),
                                                       Force3(0.0, 0.0, 0.0),
                                                       this->manager);

    controller_kinematics_add_force(ck, Force3(this->camera->direction_x * v0 * mass,
                                               this->camera->direction_y * v0 * mass,
                                               this->camera->direction_z * v0 * mass));

    // controller_kinematics_p ck = Controller_kinematics(mass,
    //                                 0.0,
    //                                 6.0,
    //                                 -35.0,
    //                                 0.0, 0.0, 0.0, this->manager);

    // controller_kinematics_add_force(ck, Force3 (0.15,
    //                                             0.5,
    //                                             0.2 ));

    model_3D_p model = Sphere(1.0, 0.3, 0.3, 0.3);
    controller_solid_p cs = Controller_solid(model, FER);

    entity_add_controller(e, (controller_p)ck);
    entity_add_controller(e, (controller_p)cs);
    vector_append(this->entities, (void *)e);
}

void world_process_input(world_p this)
{
    // En cas d'appui de la touche E on génère une boule et on la lance dans la bonne direction
    user_event_p actuel;

    // moche... revoir la façon de gérer les event
    int n_events = vector_len(this->events);
    for (int i = 0; i < n_events; i++)
    {
        actuel = (user_event_p)vector_get_at(this->events, i);
        switch (actuel->type)
        {
        case E_UP:
            lance_boule(this);
            i = n_events;
            break;
        default:
            break;
        }
    }

    int n_entities = vector_len(this->entities);
    for (int i = 0; i < n_entities; ++i)
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