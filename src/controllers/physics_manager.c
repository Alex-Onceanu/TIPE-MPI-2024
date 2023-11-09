#include <stdlib.h>
#include <stdio.h>

#include "../tools/constantes.h"
#include "physics_manager.h"
#include "../tools/maths.h"


// CHANGER çA : gérer les NULL pour pas avoir 500 fonctions qui ne font rien
/// ne fait rien
void id93(controller_p c, void *data) { return; }
void id94(controller_p c) { return; }

// C'est dans cet update que physics_manager va faire subir des forces aux controlleurs qu'il gère
void physics_manager_update(controller_p this2)
{
    physics_manager_p this = (physics_manager_p)this2;
    int nb_controllers = vector_len(this->kinematic_controllers);
    controller_kinematics_p tmp_controller = NULL;
    float tmp_fx = 0.0;
    float tmp_fy = 0.0;
    float tmp_fz = 0.0;

    for(int i = 0; i < nb_controllers; i++)
    {
        // Poids et frottements fluides : ces forces s'appliquent à tout le monde
        tmp_controller = (controller_kinematics_p)vector_get_at(this->kinematic_controllers, i);

        tmp_fx = -tmp_controller->vx * FLUID_MU;
        tmp_fy = -tmp_controller->vy * FLUID_MU;
        tmp_fz = -tmp_controller->vz * FLUID_MU;

        printf("it %d : x : %f, y : %f, z : %f\n", i, tmp_controller->x, tmp_controller->y, tmp_controller->z);
        
        controller_kinematics_add_force(tmp_controller, Force3(tmp_fx, tmp_fy, tmp_fz));

        // Réaction du support : si y < 0 on ajoute une force valant -1.5 * vy (effet de rebond)
        // !! ne pas oublier d'implémenter les frottements solides !!
        if(tmp_controller->y <= 0.0)
        {
            tmp_controller->y = 0.0;
            tmp_fx = 0.0;
            tmp_fy = -tmp_controller->vy * tmp_controller->mass * 1.5;
            tmp_fz = 0.0;
            
            controller_kinematics_add_force(tmp_controller, Force3(tmp_fx, tmp_fy, tmp_fz));
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