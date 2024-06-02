#include <stdlib.h>

#include "controller_solid.h"
#include "../modelisation/materiau.h"
#include "../modelisation/model_3D.h"

void controller_solid_draw(controller_p __this)
{
    controller_solid_p this = (controller_solid_p)__this;
    model_3D_draw(this->model, this->materiau, controller_get_program_id(&this->super));
    if (this->model.model_id == SPHERE_BIG_BUF || this->model.model_id == SPHERE_SMALL_BUF)
    {
        model_3D_draw(Model3D(SHADOW_BUF, NO_TEXTURE, NO_TEXTURE), Materiau(SOLEIL), SHADOW_PROGRAM);

        if(SHOULD_RENDER_AXIS)
        {
            model_3D_draw(Model3D(AXIS_BUF, NO_TEXTURE, NO_TEXTURE), Materiau(SOLEIL), AXIS_PROGRAM);
        }
    }
}

void controller_solid_free(controller_p __this)
{
    controller_solid_p this = (controller_solid_p)__this;
    
    model_3D_free(this->model);
    free(this);
}

controller_solid_p Controller_solid(model_3D_t __model, MATERIAU_TYPE_t materiau_type)
{
    controller_solid_p this = malloc(sizeof(controller_solid_t));
    this->super.process_input = id_for_process_input;
    this->super.update = id;
    this->super.draw = controller_solid_draw;
    this->super.free = controller_solid_free;

    this->model = __model;
    this->materiau = Materiau(materiau_type);

    return this;
}