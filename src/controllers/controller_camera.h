#ifndef CONTROLLER_CAMERA_H
#define CONTROLLER_CAMERA_H

#include <stdbool.h>
#include "controller.h"

typedef struct controller_camera
{
    // Inheritance, toute instance contient une instance pere (super-ieure)
    controller_t super;

    float x, y, z;
    float v;             // Vitesse de référence (pixels/frame)
    float v_x, v_y, v_z; // Valent v, -v ou 0
    float theta_x, theta_y;
    float w; // Vitesse angulaire de référence
    float w_x, w_y;
    bool clicks;
    float mouse_x, mouse_y;
    float old_mouse_x, old_mouse_y;
} controller_camera_t, *controller_camera_p;

// Constructeur
controller_camera_p Controller_camera(float x0, float y0, float z0);

#endif