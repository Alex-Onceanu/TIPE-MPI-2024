#include <stdlib.h>

#include "controller_solid.h"
#include "../modelisation/model_3D.h"

// Ne fait rien.
void id6(controller_p c, void *data) { return; }

void id7(controller_p c) { return; }

// void controller_solid_update(controller_p this)
// {
//     controller_solid_p c = (controller_solid_p)this;
//     c->model->vertex_buffer->data
// }

void controller_solid_draw(controller_p this)
{
    model_3D_draw(((controller_solid_p)this)->model);
}

controller_solid_p Controller_solid(model_3D_p __model)
{
    controller_solid_p this = malloc(sizeof(controller_solid_t));
    this->super.process_input = id6;
    this->super.update = id7;
    this->super.draw = controller_solid_draw;

    this->model = __model;

    return this;
}