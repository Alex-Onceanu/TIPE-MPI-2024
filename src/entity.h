#ifndef ENTITY_H
#define ENTITY_H

#include "tools/vector.h"
#include "controllers/controller.h"

typedef struct entity
{
    // Liste de ses controlleurs, classes gerant differentes choses
    // Exemples : un controlleur pour la cinematique, un pour les collisions...
    // Le vecteur en question stocke des controller_p sous forme de void*
    vector_p controllers;

    // Methodes, en tant que pointeurs vers des fonctions

    void (*process_input)(struct entity *, void *data);
    void (*update)(struct entity *);
    void (*draw)(struct entity *);
} entity_t, *entity_p;

// Constructeur
entity_p Entity();

// Destructeur
void entity_free(entity_p this);

// Litteralement vector_append(this->controllers, (void*)c)
void entity_add_controller(entity_p this, controller_p c);

#endif