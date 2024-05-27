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
    unsigned int ball_normal_map;
};

typedef struct world world_t, *world_p;
void throw_ball(world_p);

#include "modelisation/init.h"

world_p world_init()
{
    world_p this = malloc(sizeof(world_t));
    this->entities = vector_init();
    this->events = vector_init();
    this->manager = Physics_manager();

    const char *SKYBOX[6] = {"../res/textures/sky/right.ppm", "../res/textures/sky/left.ppm", "../res/textures/sky/up.ppm", "../res/textures/sky/down.ppm", "../res/textures/sky/front.ppm", "../res/textures/sky/back.ppm"};
    const char *BALL_NORMAL_MAP[6] = {"../res/textures/ball/side.ppm", "../res/textures/ball/side.ppm", "../res/textures/ball/up.ppm", "../res/textures/ball/down.ppm", "../res/textures/ball/side.ppm", "../res/textures/ball/side.ppm"};

    this->camera = Controller_camera(Force3(7.0, 1.0, 0.0), Force3(0.0, 0.0, -1.0));
    {
        // Instancier la camera
        entity_p cam = Entity(NO_PROGRAM);
        entity_add_controller(cam, (controller_p)this->camera);
        vector_append(this->entities, (void *)cam);
    }

    {
        // Le sol est un pavé
        entity_p sol = Entity(GROUND_PROGRAM);
        controller_kinematics_p c1 = Controller_kinematics(1000000.0, Force3(0.0, -2.0, 0.0), Force3(0.0, 0.0, 0.0), NULL);

        // model_3D_p pav3d = Pave(600.0, 2.0, 600.0, 0.44, 0.401, 0.3313, NULL);
        model_3D_t pav3d = {GROUND_BUF, NO_TEXTURE, init_texture("../res/textures/noise.ppm")};
        controller_solid_p c2 = Controller_solid(pav3d, SABLE);

        entity_add_controller(sol, (controller_p)c1);
        entity_add_controller(sol, (controller_p)c2);
        vector_append(this->entities, (void *)sol);
    }

    {
        // Le ciel est un cube
        entity_p e = Entity(SKYBOX_PROGRAM);
        controller_kinematics_p ck = Controller_kinematics(1.0, Force3(0.0, 1.0, 0.0), Force3(0.0, 0.0, 0.0), NULL);

        // model_3D_p cube3d = Cube(4.0, 1.0, 1.0, 1.0, SKYBOX);
        model_3D_t cube3d = {CUBE_TEST_BUF, init_cubemap(SKYBOX, true), NO_TEXTURE};
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

    this->ball_normal_map = init_cubemap(BALL_NORMAL_MAP, false);

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

void throw_ball(world_p this)
{
    entity_p e = Entity(COLOR_PROGRAM);
    
    const float mass = BALL_MASS;
    const float v0 = THROW_SPEED / dt;


    force3_t throw_direction = LINEAR_COMBINATION(this->camera->direction, Force3(0.0, 1.0, 0.0), sin(THROW_ANGLE));
    normalize(&throw_direction);

    controller_kinematics_p ck = Controller_kinematics( mass,
                                                        LINEAR_COMBINATION(this->camera->pos, this->camera->direction, 10.0),
                                                        Force3(0.0, 1.0, 0.0),
                                                        this->manager);

    controller_kinematics_add_force(ck, force3_scale(throw_direction, v0 * mass), ck->pos);

    model_3D_t model = {SPHERE_BIG_BUF, this->ball_normal_map, NO_TEXTURE};
    controller_solid_p cs = Controller_solid(model, FER);

    entity_add_controller(e, (controller_p)ck);
    entity_add_controller(e, (controller_p)cs);
    vector_append(this->entities, (void *)e);
}

void world_process_input(world_p this)
{
    // En cas d'appui de la touche E on génère une boule et on la lance dans la bonne direction
    user_event_p current;

    // On va filtrer les évenements pour ne garder ici que ceux que world n'utilise pas
    vector_p events_for_entities = vector_init();
    bool already_thrown_ball = false;

    while(vector_len(this->events) > 0)
    {
        current = (user_event_p)vector_pop(this->events);

        // World gère l'appui de la touche E, et fait passer les autres évenements à ses entités
        switch(current->type)
        {
            case E_UP:
                if(!already_thrown_ball) throw_ball(this);
                already_thrown_ball = true;
                free(current);
                break;
            default:
                vector_append(events_for_entities, current);
                break;
        }
    }

    int n_entities = vector_len(this->entities);
    for (int i = 0; i < n_entities; ++i)
    {
        entity_p e = (entity_p)(vector_get_at(this->entities, i));
        e->process_input(e, (void *)events_for_entities);
    }

    // Après que toutes les entités aient utilisé le vecteur des évenements, on peut free tous ceux qui restent
    while(vector_len(events_for_entities) > 0)
    {
        current = (user_event_p)vector_pop(events_for_entities);
        free(current);
    }
    vector_free(events_for_entities);
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