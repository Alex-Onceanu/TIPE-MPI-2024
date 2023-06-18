#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <GLES2/gl2.h>

#include "controller_kinematics.h"
#include "../tools/constantes.h"
#include "../tools/maths.h"

// Ne fait rien.
void id2(controller_p c, void *data) { return; }

void controller_kinematics_update(controller_p this)
{
    // ruse de sioux
    controller_kinematics_p this2 = (controller_kinematics_p)this;

    double current_time = (double)clock() / (double)CLOCKS_PER_SEC;
    double time_between_frames = current_time - this2->old_time;
    double target_time = 1.0 / 120.0;
    this2->old_time = current_time;
    double coef = 1.0;
    if (time_between_frames > target_time)
    {
        coef = time_between_frames / target_time;
    }
    printf("Current time : %f, time_bf : %f, coef : %f\n", current_time, time_between_frames, coef);

    this2->x += this2->vx * coef;
    this2->y += this2->vy * coef;
    this2->z += this2->vz * coef;
    if (this2->z <= -19. || this2->z > -9.)
        this2->vz *= -1.;

    this2->theta_x += this2->wx * coef;
    this2->theta_y += this2->wy * coef;
    this2->theta_z += this2->wz * coef;

    // printf("%f\n", this2->x);
    // printf("x, y, z : %f, %f, %f;; vz : %f, ty : %f, wy : %f\n", this2->x, this2->y, this2->z, this2->vz, this2->theta_y, this2->wy);
}

void controller_kinematics_draw(controller_p this)
{
    controller_kinematics_p this2 = (controller_kinematics_p)this;

    // tr est la matrice de transformation de l'ensemble des translations que va subir l'objet
    // En fait il va juste subir la translation qui l'emmene de (0,0,0) à sa position actuelle
    mat4_t tr = translation(this2->x, this2->y, this2->z);
    int u_Translation = glGetUniformLocation(PROGRAM_ID, "u_Translation");
    glUniformMatrix4fv(u_Translation, 1, GL_FALSE, mat4_get(&tr));

    // rotation est la matrice de transformation de l'ensemble des rotations (donc produit pour tous les axes)
    mat4_t rotation = mat4_produit(rotation_z_4(this2->theta_x), mat4_produit(rotation_x_4(this2->theta_y), rotation_z_4(this2->theta_z)));
    int u_Rotation = glGetUniformLocation(PROGRAM_ID, "u_Rotation");
    glUniformMatrix4fv(u_Rotation, 1, GL_FALSE, mat4_get(&rotation));
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

    this->old_time = 0.0;

    return this;
}

// void controller_kinematics_free(controller_kinematics_p this)
// {
//     free(this);
// }