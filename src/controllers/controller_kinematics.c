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
    force3_t resultante = Force3(0.0, 0.0, 0.0);
    force3_t moment_total = Force3(0.0, 0.0, 0.0);
    force3_t OM;
    for (int i = 0; i < this->nb_forces; i++)
    {
        resultante = force3_add(resultante, this->forces[i]);

        OM = force3_sub(this->contact[i], this->pos);
        // On ajoute pas cette force au moment si son point d'application est O
        if (SQ_NORME2(OM) > 0.0001)
            // Moment = OM ^ F
            moment_total = force3_add(moment_total, force3_cross_product(OM, this->forces[i]));
    }
    this->nb_forces = 0;

    // On met à jour l'ancienne vitesse et l'ancienne position avant de changer la vitesse et position actuelle
    this->old_pos = this->pos;
    this->old_speed = this->speed;

    // PFD : on obtient une accélération à partir de la résultante des forces que subit l'objet
    // m * a = somme des forces et a = dv/dt donc dv = somme des forces * dt / m
    this->speed = LINEAR_COMBINATION(this->speed, resultante, (dt / this->mass));

    // On intègre speed : pos = somme des speed * dt (méthode d'Euler)
    this->pos = LINEAR_COMBINATION(this->pos, this->speed, dt);

    // TMC : on obtient une accélération angulaire à partir de la somme des moments des forces s'appliquant à cet objet
    // m dw/dt = somme des OM ^ F donc dw = somme des OM ^ F * dt / m

    this->old_omega = this->omega;
    this->omega = LINEAR_COMBINATION(this->omega, moment_total, (dt / this->mass));

    // On intègre omega : theta = somme des omega * dt (méthode d'Euler)
    this->theta = LINEAR_COMBINATION(this->theta, this->omega, dt);

    // float Ep = this->mass * GRAVITY * this->pos.fy;
    // float Ec = 0.5 * this->mass * SQ_NORME2(this->speed);
    // float Em = Ec + Ep;

    // if (this->radius > 0.3)
    //     printf("Ec : %f | Ep : %f | Em : %f\n", Ec, Ep, Em);

    // printf("speed = %f %f %f, old_speed = %f %f %f\n", this->speed.fx, this->speed.fy, this->speed.fz, this->old_speed.fx, this->old_speed.fy, this->old_speed.fz);
}

void controller_kinematics_draw(controller_p __this)
{
    controller_kinematics_p this = (controller_kinematics_p)__this;

    // tr est la matrice de transformation de l'ensemble des translations que va subir l'objet
    // En fait il va juste subir la translation qui l'emmene de (0,0,0) à sa position actuelle
    mat4_t tr = translation(this->pos.fx, this->pos.fy, this->pos.fz);

    mat4_t P; // Envoie ex sur theta, servira pour l'axe de rotation
    // rotation est la matrice de rotation de l'objet autour de son axe de rotation, d'angle || theta ||
    mat4_t rotation = mat4_rotation(this->theta, &P);

    unsigned int program = PROGRAM_ID[this->super.program_index];
    glUseProgram(program);

    int u_Translation = glGetUniformLocation(program, "u_Translation");
    glUniformMatrix4fv(u_Translation, 1, GL_FALSE, mat4_get(&tr));

    int u_Rotation = glGetUniformLocation(program, "u_Rotation");
    glUniformMatrix4fv(u_Rotation, 1, GL_FALSE, mat4_get(&rotation));

    program = PROGRAM_ID[SHADOW_PROGRAM];
    glUseProgram(program);

    u_Translation = glGetUniformLocation(program, "u_Translation");
    glUniformMatrix4fv(u_Translation, 1, GL_FALSE, mat4_get(&tr));

    u_Rotation = glGetUniformLocation(program, "u_Rotation");
    glUniformMatrix4fv(u_Rotation, 1, GL_FALSE, mat4_get(&rotation));

    if (this->radius != 0.0)
    {
        // Si cet objet est une boule, mettre à jour l'uniform des axes de rotation

        program = PROGRAM_ID[AXIS_PROGRAM];
        glUseProgram(program);

        u_Rotation = glGetUniformLocation(program, "u_Rotation");
        glUniformMatrix4fv(u_Rotation, 1, GL_FALSE, mat4_get(&P));

        u_Translation = glGetUniformLocation(program, "u_Translation");
        glUniformMatrix4fv(u_Translation, 1, GL_FALSE, mat4_get(&tr));

        int u_Omega = glGetUniformLocation(program, "u_Omega");
        // printf("omega = %f\n", norme2(this->omega));
        glUniform1f(u_Omega, norme2(this->omega) * FPS / (2.0 * PI));
    }
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
    this->old_omega = Force3(0.0, 0.0, 0.0);

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

void controller_kinematics_add_force(controller_kinematics_p this, force3_t f, force3_t c)
{
    this->contact[this->nb_forces] = c;
    this->forces[this->nb_forces++] = f;
}