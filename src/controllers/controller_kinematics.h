#ifndef CONTROLLER_KINEMATICS_H
#define CONTROLLER_KINEMATICS_H

#include "controller.h"
#include "../tools/maths.h"

// On part du principe que personne ne va jamais subir simultanément 10 forces
#define NB_MAX_FORCES 10

// Bientot l'acceleration et les forces...

typedef struct controller_kinematics
{
    // Inheritance, toute instance contient une instance pere (super-ieure)
    controller_t super;

    // Masse
    float mass;

    // Position
    float x, y, z;
    // Vitesse
    float vx, vy, vz;

    // Angle de rotation (EN RADIANS)
    float theta_x, theta_y, theta_z;
    // Vitesse angulaire (rd/s)
    float wx, wy, wz;

    double old_time;

    // Liste de forces (force3_t), qui seront converties en dv/dt et dw/dt par PFD et TMC
    // On utilise un tableau statique et pas dynamique pour pas avoir à malloc/free à chaque frame
    // Et on est certains de ne jamais appliquer plus de 10 forces instantanées au même moment à un même objet
    int nb_forces;
    force3_t forces[NB_MAX_FORCES];
} controller_kinematics_t, *controller_kinematics_p;


#include "physics_manager.h"

// Constructeur
controller_kinematics_p Controller_kinematics(float m, float x0, float y0, float z0, float theta_x0, float theta_y0, float theta_z0, physics_manager_p manager);

// Destructeur
void controller_kinematics_free(controller_kinematics_p this);

// Ajoute la force de vecteur (fx, fy, fz) à la liste de forces s'appliquant à cet objet
void controller_kinematics_add_force(controller_kinematics_p this, force3_t f);

#endif