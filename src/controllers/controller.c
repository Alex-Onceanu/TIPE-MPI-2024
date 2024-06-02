#include <stdlib.h>

#include "controller.h"

void id(controller_p c)
{
    return;
}

void id_for_process_input(controller_p c, void *data)
{
    return;
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