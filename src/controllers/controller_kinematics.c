#include <stdlib.h>
#include <GLES3/gl3.h>

#include <stdio.h>
#include "controller_kinematics.h"
#include "../tools/constantes.h"
#include "../tools/maths.h"
#include "physics_manager.h"

void controller_kinematics_update(controller_p __this)
{
    // ruse de sioux
    controller_kinematics_p this = (controller_kinematics_p)__this;
    
    // Somme vectorielle des forces extérieures s'appliquant à l'objet
    force3_t resultante = {0.0, 0.0, 0.0};
    for (int i = 0; i < this->nb_forces; i++)
    {
        resultante.fx += this->forces[i].fx;
        resultante.fy += this->forces[i].fy;
        resultante.fz += this->forces[i].fz;

        // Puis on enleve cette force de la liste des forces que subit l'objet
        this->forces[i] = Force3(0.0, 0.0, 0.0);
    }
    this->nb_forces = 0;

    // On met à jour l'ancienne vitesse et l'ancienne position avant de changer la vitesse et position actuelle
    this->old_pos = this->pos;
    this->old_speed = this->speed;

    // PFD : on obtient une accélération à partir de la résultante des forces que subit l'objet
    // m * a = somme des forces et a = dv/dt donc dv = somme des forces * dt / m
    this->speed.fx += resultante.fx * dt / this->mass;
    this->speed.fy += resultante.fy * dt / this->mass;
    this->speed.fz += resultante.fz * dt / this->mass;

    this->pos.fx += this->speed.fx * dt;
    this->pos.fy += this->speed.fy * dt;
    this->pos.fz += this->speed.fz * dt;

    // Appliquer un TMC pour obtenir l'accélération angulaire ?
    // Pas la peine, les boules sont lisses

    this->theta.fx += this->omega.fx * dt;
    this->theta.fy += this->omega.fy * dt;
    this->theta.fz += this->omega.fz * dt;
}

void controller_kinematics_draw(controller_p __this)
{
    controller_kinematics_p this = (controller_kinematics_p)__this;


    // tr est la matrice de transformation de l'ensemble des translations que va subir l'objet
    // En fait il va juste subir la translation qui l'emmene de (0,0,0) à sa position actuelle
    mat4_t tr = translation(this->pos.fx, this->pos.fy, this->pos.fz);
    // rotation est la matrice de transformation de l'ensemble des rotations (donc produit pour tous les axes)
    mat4_t rotation = mat4_produit(rotation_x_4(this->theta.fx), mat4_produit(rotation_y_4(this->theta.fy), rotation_z_4(this->theta.fz)));

    unsigned int program = PROGRAM_ID[this->super.program_index];
    glUseProgram(program);

    int u_Translation = glGetUniformLocation(program, "u_Translation");
    glUniformMatrix4fv(u_Translation, 1, GL_FALSE, mat4_get(&tr));

    int u_Rotation = glGetUniformLocation(program, "u_Rotation");
    glUniformMatrix4fv(u_Rotation, 1, GL_FALSE, mat4_get(&rotation));
}

controller_kinematics_p Controller_kinematics(float __mass, force3_t initial_pos, force3_t initial_theta, physics_manager_p manager)
{
    controller_kinematics_p this = malloc(sizeof(controller_kinematics_t));
    this->super.process_input = id_for_process_input;
    this->super.update = controller_kinematics_update;
    this->super.draw = controller_kinematics_draw;

    this->mass = __mass;

    this->pos = initial_pos;
    this->old_pos = initial_pos;

    this->speed = Force3(0.0, 0.0, 0.0);
    this->old_speed = this->speed;

    this->theta = initial_theta;
    this->omega = Force3(0.0, 0.0, 0.0);

    this->nb_forces = 0;

    if (manager != NULL)
    {
        physics_manager_add_controller_kinematics(manager, this);
        this->radius = 1.0;
    }
    else
    {
        this->radius = 0.0;
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