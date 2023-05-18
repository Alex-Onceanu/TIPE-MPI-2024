#ifndef WORLD_H
#define WORLD_H

typedef struct world world_t, *world_p;

world_p world_init();

void world_process_input(world_p this);
void world_update(world_p this);
void world_draw(world_p this);

#endif