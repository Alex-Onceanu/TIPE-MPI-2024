#include <stdlib.h>
#include <stdio.h>

#include "entity.h"
#include "tools/vector.h"
#include "tools/constantes.h"
#include "controllers/controller.h"

// Les 3 fonctions des entity de base
// Appellent leur equivalent pour chacun des controlleurs de l'entity

void entity_process_input(entity_p this, void *data)
{
    const int n = vector_len(this->controllers);
    for (int i = 0; i < n; ++i)
    {
        controller_p c = (controller_p)(vector_get_at(this->controllers, i));
        c->process_input(c, data);
    }
}

void entity_update(entity_p this)
{
    const int n = vector_len(this->controllers);
    for (int i = 0; i < n; ++i)
    {
        controller_p c = (controller_p)(vector_get_at(this->controllers, i));
        c->update(c);
    }
}

void entity_draw(entity_p this)
{
    const int n = vector_len(this->controllers);
    for (int i = 0; i < n; ++i)
    {
        controller_p c = (controller_p)(vector_get_at(this->controllers, i));
        c->draw(c);
    }
}

entity_p Entity(unsigned int __program_index)
{
    entity_p this = malloc(sizeof(entity_t));

    this->controllers = vector_init();
    this->process_input = entity_process_input;
    this->update = entity_update;
    this->draw = entity_draw;
    this->program_index = __program_index;

    return this;
}

void entity_free(entity_p this)
{
    for (int i = 0; i < vector_len(this->controllers); ++i)
    {
        // Transformation de void* en entity_t*
        controller_free((controller_p)vector_get_at(this->controllers, i));
    }
    vector_free(this->controllers);
    free(this);
}

void entity_add_controller(entity_p this, controller_p c)
{
    controller_set_program_id(c, this->program_index);
    vector_append(this->controllers, (void *)c);
}