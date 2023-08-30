#ifndef CONTROLLER_SOLID_H
#define CONTROLLER_SOLID_H

#include "controller.h"
#include "../modelisation/materiau.h"
#include "../modelisation/model_3D.h"

typedef struct controller_solid
{
    // Inheritance, toute instance contient une instance pere (super-ieure)
    controller_t super;
    model_3D_p model;
    materiau_t materiau;
} controller_solid_t, *controller_solid_p;

// Constructeur (!! __model doit être aupréalablement alloué sur le tas)
controller_solid_p Controller_solid(model_3D_p __model, MATERIAU_TYPE_t materiau_type);

#endif