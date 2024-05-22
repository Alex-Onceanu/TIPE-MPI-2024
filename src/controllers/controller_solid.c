#include <stdlib.h>

#include "controller_solid.h"
#include "../modelisation/materiau.h"
#include "../modelisation/model_3D.h"

void controller_solid_draw(controller_p this)
{
    controller_solid_p this2 = (controller_solid_p)this;
    model_3D_draw(this2->model, this2->materiau, controller_get_program_id(&this2->super));
    if (this2->model.model_id == SPHERE_BIG_BUF)
    {
        model_3D_draw((model_3D_t){SHADOW_BUF, NO_TEXTURE}, Materiau(SOLEIL), SHADOW_PROGRAM);

        if(SHOULD_RENDER_AXIS)
        {
            model_3D_draw((model_3D_t){AXIS_BUF, NO_TEXTURE}, Materiau(SOLEIL), AXIS_PROGRAM);
        }
    }
}

controller_solid_p Controller_solid(model_3D_t __model, MATERIAU_TYPE_t materiau_type)
{
    controller_solid_p this = malloc(sizeof(controller_solid_t));
    this->super.process_input = id_for_process_input;
    this->super.update = id;
    this->super.draw = controller_solid_draw;

    this->model = __model;
    this->materiau = Materiau(materiau_type);

    return this;
}