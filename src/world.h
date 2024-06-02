#ifndef WORLD_H
#define WORLD_H

#include "user_event.h"

typedef struct world world_t, *world_p;

// Constructeur
world_p world_init();

// Destructeur
void world_free(world_p this);

void world_process_input(world_p this);
void world_update(world_p this);
void world_draw(world_p this);

void world_add_event(world_p this, user_event_t e_t);

#endif