#include <stdlib.h>

#include "controller.h"

// Ne fait rien.
void id(controller_p c) { return; }

controller_p Controller()
{
    controller_p this = malloc(sizeof(controller_t));

    this->process_input = id;
    this->update = id;
    this->draw = id;

    return this;
}

void controller_free(controller_p this)
{
    free(this);
}