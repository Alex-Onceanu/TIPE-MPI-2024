#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "controller.h"
#include "../tools/vector.h"

extern const double TARGET_TIME;
extern double old_time;
extern double dt;

// Va "gérer" des controller_kinematics
// Donc c'est physics_manager qui va ajouter des forces aux boules de pétanque (et autres objets)
// Exemple ajoute le poids, des frottements (fluides & solides), forces de collision...
typedef struct physics_manager
{
    // Physics_manager est en fait un controlleur, qui sera donné à une instance unique d'Entity
    // qui ne comportera que ce controlleur.
    controller_t super;

    // Liste des controller_kinematics_p que gère ce manager.
    // C'est uniquement sur les controlleurs de ce vecteur que pourra influer le manager
    // (donc leur ajouter des forces, gérer leurs collisions...)
    vector_p kinematic_controllers;
} physics_manager_t, *physics_manager_p;

// Constructeur
physics_manager_p Physics_manager();




#include "controller_kinematics.h"

// Destructeur /!\ ne free pas tous les controller_kinematics qu'il gère !
void physics_manager_free(physics_manager_p this);

void physics_manager_add_controller_kinematics(physics_manager_p this, controller_kinematics_p c);

#endif