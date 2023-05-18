#ifndef CONTROLLER_KINEMATICS_H
#define CONTROLLER_KINEMATICS_H

#include "controller.h"

// Bientot l'acceleration et les forces...

typedef struct controller_kinematics
{
    // Inheritance, toute instance contient une instance pere (super-ieure)
    controller_t super;
    // Position
    float x, y, z;
    // Vitesse
    float vx, vy, vz;

    // Angle de rotation (EN RADIANS)
    float theta_x, theta_y, theta_z;
    // Vitesse angulaire (rd/s)
    float wx, wy, wz;
} controller_kinematics_t, *controller_kinematics_p;

// Constructeur
controller_kinematics_p Controller_kinematics(float x0, float y0, float z0, float theta_x0, float theta_y0, float theta_z0);

// Destructeur
// void controller_kinematics_free(controller_kinematics_p this);

#endif