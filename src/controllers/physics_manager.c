#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../tools/constantes.h"
#include "physics_manager.h"
#include "../tools/maths.h"

// CHANGER çA : gérer les NULL pour pas avoir 500 fonctions qui ne font rien
/// ne fait rien
void id93(controller_p c, void *data) { return; }
void id94(controller_p c) { return; }

// Renvoie 1 si la boule c1 touche la boule c2, 0 sinon
int collision(controller_kinematics_p c1, controller_kinematics_p c2)
{
    float r = 2.0;
    return norme2(c1->x - c2->x, c1->y - c2->y, c1->z - c2->z) < r;
}

// C'est dans cet update que physics_manager va faire subir des forces aux controlleurs qu'il gère
void physics_manager_update(controller_p this2)
{
    double current_time = (double)clock() / (double)CLOCKS_PER_SEC;
    double time_between_frames = current_time - old_time;
    old_time = current_time;
    if (time_between_frames > TARGET_TIME)
    {
        dt = time_between_frames * FPS;
    }

    physics_manager_p this = (physics_manager_p)this2;
    int nb_controllers = vector_len(this->kinematic_controllers);
    controller_kinematics_p tmp_controller = NULL;
    float tmp_fx = 0.0;
    float tmp_fy = 0.0;
    float tmp_fz = 0.0;

    for (int i = 0; i < nb_controllers; i++)
    {
        // printf("i = %d\n", i);
        // Poids et frottements fluides : ces forces s'appliquent à tout le monde
        tmp_controller = (controller_kinematics_p)vector_get_at(this->kinematic_controllers, i);

        tmp_fx = -tmp_controller->vx * FLUID_MU;
        tmp_fy = -tmp_controller->vy * FLUID_MU;
        tmp_fz = -tmp_controller->vz * FLUID_MU;

        // printf("it %d : x : %f, y : %f, z : %f\n", i, tmp_controller->x, tmp_controller->y, tmp_controller->z);

        controller_kinematics_add_force(tmp_controller, Force3(tmp_fx, tmp_fy, tmp_fz));

        // Réaction du support : si y < 0 on ajoute une force valant -1.5 * vy (effet de rebond)
        // !! ne pas oublier d'implémenter les frottements solides !!
        if (tmp_controller->y <= 0.0)
        {
            tmp_controller->y = 0.0;
            if (f_abs(tmp_controller->vy) > 0.1) // ???
            {
                tmp_fx = -SOLID_DYNAMIC_MU * tmp_controller->vx * tmp_controller->mass / dt;
                // tmp_fx = 0.0;
                tmp_fy = -tmp_controller->vy * tmp_controller->mass * 1.4 / dt;
                // tmp_fz = 0.0;
                tmp_fz = -SOLID_DYNAMIC_MU * tmp_controller->vz * tmp_controller->mass / dt;

                controller_kinematics_add_force(tmp_controller, Force3(tmp_fx, tmp_fy, tmp_fz));
            }
        }
        else
        {
            // On applique le poids uniquement si l'objet est au-dessus du sol
            tmp_fx = 0.0;
            tmp_fy = -tmp_controller->mass * GRAVITY;
            tmp_fz = 0.0;

            controller_kinematics_add_force(tmp_controller, Force3(tmp_fx, tmp_fy, tmp_fz));
        }
    }

    // On gère les collisions
    force3_t impact_direction = {};
    float impact_coef = 0.0;
    float impact_coef_1 = 0.0;
    float impact_coef_2 = 0.0;
    controller_kinematics_p tmp_controller_2 = NULL;
    for (int i = 0; i < nb_controllers; i++)
    {
        tmp_controller = (controller_kinematics_p)vector_get_at(this->kinematic_controllers, i);
        for (int j = i + 1; j < nb_controllers; j++)
        {
            tmp_controller_2 = (controller_kinematics_p)vector_get_at(this->kinematic_controllers, j);
            if (collision(tmp_controller, tmp_controller_2))
            {
                printf("Collision !\n");
                impact_direction = Force3(tmp_controller->x - tmp_controller_2->x, tmp_controller->y - tmp_controller_2->y, tmp_controller->z - tmp_controller_2->z);
                normalize(&(impact_direction.fx), &(impact_direction.fy), &(impact_direction.fz));
                impact_coef_1 = f_abs(produit_scalaire(impact_direction, Force3(tmp_controller->vx, tmp_controller->vy, tmp_controller->vz)));
                impact_coef_2 = f_abs(produit_scalaire(impact_direction, Force3(tmp_controller_2->vx, tmp_controller_2->vy, tmp_controller_2->vz)));
                impact_coef = 0.5 * (impact_coef_1 + impact_coef_2);

                tmp_fx = impact_coef * impact_direction.fx * tmp_controller->mass / dt;
                tmp_fy = impact_coef * impact_direction.fy * tmp_controller->mass / dt;
                tmp_fz = impact_coef * impact_direction.fz * tmp_controller->mass / dt;

                controller_kinematics_add_force(tmp_controller, Force3(tmp_fx, tmp_fy, tmp_fz));

                tmp_fx = -impact_coef * impact_direction.fx * tmp_controller_2->mass / dt;
                tmp_fy = -impact_coef * impact_direction.fy * tmp_controller_2->mass / dt;
                tmp_fz = -impact_coef * impact_direction.fz * tmp_controller_2->mass / dt;

                controller_kinematics_add_force(tmp_controller_2, Force3(tmp_fx, tmp_fy, tmp_fz));
            }
        }
    }
}

physics_manager_p Physics_manager()
{
    physics_manager_p this = malloc(sizeof(physics_manager_t));
    this->super.process_input = id93;
    this->super.update = physics_manager_update;
    this->super.draw = id94;

    this->kinematic_controllers = vector_init();

    return this;
}

void physics_manager_free(physics_manager_p this)
{
    vector_free(this->kinematic_controllers);
    free(this);
}

void physics_manager_add_controller_kinematics(physics_manager_p this, controller_kinematics_p c)
{
    vector_append(this->kinematic_controllers, (void *)c);
}