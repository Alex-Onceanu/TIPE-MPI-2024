#include <stdlib.h>

#include "controller.h"

// Ne fait rien.
void id00(controller_p c, void *data) { return; }
void id(controller_p c) { return; }

controller_p Controller()
{
    controller_p this = malloc(sizeof(controller_t));

    this->process_input = id00;
    this->update = id;
    this->draw = id;

    return this;
}

void controller_free(controller_p this)
{
    free(this);
}

void controller_set_program_id(controller_p this, unsigned int __program_index)
{
    this->program_index = __program_index;
}

unsigned int controller_get_program_id(controller_p this)
{
    return this->program_index;
}