#include <stdlib.h>
#include <GLES2/gl2.h>

#include "controller_kinematics.h"
#include "../tools/constantes.h"
#include "../tools/maths.h"
#include "physics_manager.h"

// Ne fait rien.
void id2(controller_p c, void *data) { return; }

void controller_kinematics_update(controller_p this)
{
    // ruse de sioux
    controller_kinematics_p this2 = (controller_kinematics_p)this;

    // Somme vectorielle des forces extérieures s'appliquant à l'objet
    force3_t resultante = { 0.0, 0.0, 0.0 };
    for(int i = 0; i < this2->nb_forces; i++)
    {
        resultante.fx += this2->forces[i].fx;
        resultante.fy += this2->forces[i].fy;
        resultante.fz += this2->forces[i].fz;

        // Puis on enleve cette force de la liste des forces que subit l'objet
        this2->forces[i] = Force3(0.0, 0.0, 0.0);
    }
    this2->nb_forces = 0;

    // PFD : on obtient une accélération à partir de la résultante des forces que subit l'objet
    // m * a = somme des forces et a = dv/dt donc dv = somme des forces * dt / m
    this2->vx += resultante.fx * dt / this2->mass;
    this2->vy += resultante.fy * dt / this2->mass;
    this2->vz += resultante.fz * dt / this2->mass;

    this2->x += this2->vx * dt;
    this2->y += this2->vy * dt;
    this2->z += this2->vz * dt;

    // Appliquer un TMC pour obtenir l'accélération angulaire ?

    this2->theta_x += this2->wx * dt;
    this2->theta_y += this2->wy * dt;
    this2->theta_z += this2->wz * dt;
}

void controller_kinematics_draw(controller_p this)
{
    controller_kinematics_p this2 = (controller_kinematics_p)this;

    // tr est la matrice de transformation de l'ensemble des translations que va subir l'objet
    // En fait il va juste subir la translation qui l'emmene de (0,0,0) à sa position actuelle
    mat4_t tr = translation(this2->x, this2->y, this2->z);
    // rotation est la matrice de transformation de l'ensemble des rotations (donc produit pour tous les axes)
    mat4_t rotation = mat4_produit(rotation_x_4(this2->theta_x), mat4_produit(rotation_y_4(this2->theta_y), rotation_z_4(this2->theta_z)));

    unsigned int program = PROGRAM_ID[this2->super.program_index];
    glUseProgram(program);

    int u_Translation = glGetUniformLocation(program, "u_Translation");
    glUniformMatrix4fv(u_Translation, 1, GL_FALSE, mat4_get(&tr));

    int u_Rotation = glGetUniformLocation(program, "u_Rotation");
    glUniformMatrix4fv(u_Rotation, 1, GL_FALSE, mat4_get(&rotation));
}

controller_kinematics_p Controller_kinematics(float m, float x0, float y0, float z0, float theta_x0, float theta_y0, float theta_z0, physics_manager_p manager)
{
    controller_kinematics_p this = malloc(sizeof(controller_kinematics_t));
    this->super.process_input = id2;
    this->super.update = controller_kinematics_update;
    this->super.draw = controller_kinematics_draw;

    this->mass = m;

    this->x = x0;
    this->y = y0;
    this->z = z0;

    this->vx = 0.0;
    this->vy = 0.0;
    this->vz = 0.0;

    this->theta_x = theta_x0;
    this->theta_y = theta_y0;
    this->theta_z = theta_z0;

    this->wx = 0.;
    this->wy = 0.;
    this->wz = 0.;

    this->nb_forces = 0;

    if(manager != NULL)
    {
        physics_manager_add_controller_kinematics(manager, this);
    }

    return this;
}

void controller_kinematics_free(controller_kinematics_p this)
{
    free(this);
}

void controller_kinematics_add_force(controller_kinematics_p this, force3_t f)
{
    this->forces[this->nb_forces++] = f;
}