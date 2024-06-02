#ifndef CONTROLLER_KINEMATICS_H
#define CONTROLLER_KINEMATICS_H

#include "controller.h"
#include "../tools/maths.h"

// On part du principe que personne ne va jamais subir simultanément 20 forces
#define NB_MAX_FORCES 20

typedef struct controller_kinematics
{
    // Inheritance, toute instance contient une instance pere (super-ieure)
    controller_t super;

    float mass;

    // Position et position de l'objet à la frame dernière
    force3_t pos;
    force3_t old_pos;

    // Vitesse actuelle et vitesse qu'avait l'objet à la frame dernière
    force3_t speed;
    force3_t old_speed;

    // Angle de rotation (EN RADIANS) autour de chaque axe (x,y,z)
    force3_t theta;
    // Vitesse angulaire (rd/s) autour de chaque axe (x,y,z)
    force3_t omega;
    force3_t old_omega;

    // Si cet objet est une boule, cet attribut est son rayon. Sinon, vaut 0
    // En fait on va essentiellement gérer des boules, pas d'autres objets complexes
    float radius;

    // Tableau de forces (force3_t), qui seront converties en dv/dt et dw/dt par PFD et TMC
    // On utilise un tableau statique et pas dynamique pour pas avoir à malloc/free à chaque frame
    // Et on est certains de ne jamais appliquer plus de 10 forces instantanées au même moment à un même objet
    int nb_forces;
    force3_t forces[NB_MAX_FORCES];
    // OM[i] := point d'application de forces[i] (utile pour le TMC)
    force3_t contact[NB_MAX_FORCES];
} controller_kinematics_t, *controller_kinematics_p;

#include "physics_manager.h"

// Constructeur
controller_kinematics_p Controller_kinematics(float __mass, force3_t initial_pos, force3_t initial_theta, physics_manager_p manager, float radius);

// Ajoute la force de vecteur f et de point d'application om à la liste de forces s'appliquant à cet objet
void controller_kinematics_add_force(controller_kinematics_p this, force3_t f, force3_t c);

#endif