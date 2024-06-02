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

    // *_PROGRAM | NO_PROGRAM
    unsigned int program_index; 

    // Méthodes, en tant que pointeurs vers des fonctions
    void (*process_input)(struct entity *, void *data);
    void (*update)(struct entity *);
    void (*draw)(struct entity *);
} entity_t, *entity_p;

// Constructeur
entity_p Entity(unsigned int __program_index);

// Destructeur
void entity_free(entity_p this);

// Ajoute c aux controlleurs de this et set this->program_id à c 
void entity_add_controller(entity_p this, controller_p c);

#endif