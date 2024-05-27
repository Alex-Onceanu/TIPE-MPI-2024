#ifndef CONTROLLER_CAMERA_H
#define CONTROLLER_CAMERA_H

#include <stdbool.h>
#include "controller.h"
#include "../tools/maths.h"

typedef struct controller_camera
{
    // Inheritance, toute instance contient une instance pere (super-ieure)
    controller_t super;

    force3_t pos;
    float v;            // Vitesse de référence (pixels/frame)
    force3_t speed;     // Les composantes valent v, -v ou 0
    float theta_x, theta_y;
    float w; // Vitesse angulaire de référence
    float w_x, w_y;
    bool clicks;
    float mouse_x, mouse_y;
    float old_mouse_x, old_mouse_y;
    double old_time;
    force3_t direction;
} controller_camera_t, *controller_camera_p;

// Constructeur
controller_camera_p Controller_camera(force3_t pos0, force3_t dir0);

#endif