#include <stdlib.h>
#include <stdio.h>
#include <GLES2/gl2.h>

#include "controller_kinematics.h"
#include "../tools/constantes.h"
#include "../tools/maths.h"

// Ne fait rien.
void id2(controller_p c) { return; }

void controller_kinematics_update(controller_p this)
{
    // ruse de sioux
    controller_kinematics_p this2 = (controller_kinematics_p)this;

    this2->x += this2->vx;
    this2->y += this2->vy;
    this2->z += this2->vz;
    if (this2->z <= -19. || this2->z > -9.)
        this2->vz *= -1.;

    this2->theta_x += this2->wx;
    this2->theta_y += this2->wy;
    this2->theta_z += this2->wz;

    // printf("%f\n", this2->x);
    // printf("x, y, z : %f, %f, %f;; vz : %f, ty : %f, wy : %f\n", this2->x, this2->y, this2->z, this2->vz, this2->theta_y, this2->wy);
}

void controller_kinematics_draw(controller_p this)
{
    controller_kinematics_p this2 = (controller_kinematics_p)this;
    mat4_t transform = translation(this2->x, this2->y, this2->z);

    // Pour gerer les autres axes faire des produits de matrices...
    mat4_set_theta_x(&transform, -this2->theta_y);

    int u_Model = glGetUniformLocation(PROGRAM_ID, "u_Model");
    glUniformMatrix4fv(u_Model, 1, GL_FALSE, mat4_get(&transform));
}

controller_kinematics_p Controller_kinematics(float x0, float y0, float z0, float theta_x0, float theta_y0, float theta_z0)
{
    controller_kinematics_p this = malloc(sizeof(controller_kinematics_t));
    this->super.process_input = id2;
    this->super.update = controller_kinematics_update;
    this->super.draw = controller_kinematics_draw;

    this->x = x0;
    this->y = y0;
    this->z = z0;

    this->vx = 0.;
    this->vy = 0.;
    this->vz = 0.;

    this->theta_x = theta_x0;
    this->theta_y = theta_y0;
    this->theta_z = theta_z0;

    this->wx = 0.;
    this->wy = 0.;
    this->wz = 0.;

    return this;
}

// void controller_kinematics_free(controller_kinematics_p this)
// {
//     free(this);
// }