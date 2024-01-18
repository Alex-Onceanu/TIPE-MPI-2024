#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../tools/constantes.h"
#include "physics_manager.h"
#include "../tools/maths.h"

// Renvoie 1 si la boule c1 touche la boule c2, 0 sinon
int collision(controller_kinematics_p c1, controller_kinematics_p c2)
{
    force3_t pos_diff = force3_sub(c2->pos, c1->pos);
    return SQ_NORME2(pos_diff) < SQUARED(c1->radius + c2->radius);
}

#define PROPORTION(a, x, b) (a == b ? 0.0 : (x - a) / (b - a))
#define LERP(a, b, t) (a * t + b * (1.0 - t))

// Étant donnés deux points z1 et z2 avec la vitesse que possède un objet en chacun de ces deux points (v1 et v2),
// Réalise une interpolation linéaire (composante par composante) afin d'approximer la vitesse v3 qu'avait cet objet en z3
// Hypothèse : z3 appartient au segment [z1, z3]
force3_t lerp_speed_from_pos(force3_t z1, force3_t z3, force3_t z2, force3_t v1, force3_t v2)
{
    printf("Go lerp_speed_from_pos avec pos z1 : (%f; %f; %f), z3 : (%f; %f; %f), z2 : (%f; %f; %f), v1 : (%f; %f; %f), v2 : (%f; %f; %f)\n", z1.fx, z1.fy, z1.fz, z3.fx, z3.fy, z3.fz, z2.fx, z2.fy, z2.fz, v1.fx, v1.fy, v1.fz, v2.fx, v2.fy, v2.fz);
    // Pour chaque composante, donne un coefficient entre 0 et 1 correspondant à "à quel point z3 est-il proche de z2 sur cette composante ?"
    force3_t proportion = Force3(PROPORTION(z1.fx, z3.fx, z2.fx), PROPORTION(z1.fy, z3.fy, z2.fy), PROPORTION(z1.fz, z3.fz, z2.fz));
    // printf("Proportion : %f %f %f\n", proportion.fx, proportion.fy, proportion.fz);
    // force3_t ans = Force3(proportion.fx * (v2.fx - v1.fx) + v1.fx, proportion.fy * (v2.fy - v1.fy) + v1.fy, proportion.fz * (v2.fz - v1.fz) + v1.fz);
    force3_t ans = Force3(LERP(v1.fx, v2.fx, proportion.fx), LERP(v1.fy, v2.fy, proportion.fy), LERP(v1.fz, v2.fz, proportion.fz));
    // printf("Ans : %f %f %f\n", ans.fx, ans.fy, ans.fz);

    return ans;
}

// "Remonte le temps" jusqu'au moment où les boules se touchent, avant qu'elles ne rentrent l'une dans l'autre
// Hypothèse : les deux boules s'intersectent grossièrement.
void teleport_and_update_speed(controller_kinematics_p tmp_controller_1, controller_kinematics_p tmp_controller_2)
{
    // On va suivre les directions de leur vitesses respectives pour trouver la position précise de leur collision
    // Cela revient à résoudre pour k l'équation :
    // || z2 + k * v2 - (z1 + k * v1) || = 2 * r
    // En élevant au carré on tombe sur une équation de degré 2 : akk + bk + c = 0 (avec a, b, c calculés ci-dessous)
    force3_t pos_diff = force3_sub(tmp_controller_2->pos, tmp_controller_2->pos);
    force3_t speed_diff = force3_sub(tmp_controller_2->speed, tmp_controller_1->speed);

    float a = SQ_NORME2(speed_diff);
    float b = 2.0 * DOT_PRODUCT(pos_diff, speed_diff);
    float c = SQ_NORME2(pos_diff) - SQUARED(tmp_controller_1->radius + tmp_controller_2->radius);

    if (a == 0 || (b * b - 4.0 * a * c) < 0)
    {
        printf("Impossible de remonter le temps.\n");
        return;
    }

    // Résolution de l'équation de degré 2 : on ne garde que la solution négative (car on veut "remonter le temps")
    float k = (-b - my_sqrtf(b * b - 4.0 * a * c)) / 2.0 * a;
    printf("k = %f\n", k);
    if (k >= -0.000000001 || k <= -1.0)
    {
        printf("!!!!! K = 0 !!!!!!\n");
        return;
    }
    // k *= 1.01;

    // Et pour pas qu'elles aient une trop grande vitesse (On ajoute de l'énergie au système quand on "téléporte" les boules)
    // On interpole linéairement la vitesse qu'elles auraient dû avoir en cette position avec lerp_speed_from_pos

    force3_t collision_pos_1 = LINEAR_COMBINATION(tmp_controller_1->pos, tmp_controller_1->speed, k);
    printf("Avant (1) : pos = %f, %f, %f | speed = %f, %f, %f\n", tmp_controller_1->pos.fx, tmp_controller_1->pos.fy, tmp_controller_1->pos.fz, tmp_controller_1->speed.fx, tmp_controller_1->speed.fy, tmp_controller_1->speed.fz);
    force3_t new_speed_1 = lerp_speed_from_pos(tmp_controller_1->old_pos, tmp_controller_1->pos, collision_pos_1, tmp_controller_1->old_speed, tmp_controller_1->speed);
    // printf("New speed : %f %f %f\n", new_speed_1.fx, new_speed_1.fy, new_speed_1.fz);
    tmp_controller_1->speed = new_speed_1;
    tmp_controller_1->pos = collision_pos_1;
    printf("Après (1) : pos = %f, %f, %f | speed = %f, %f, %f\n", tmp_controller_1->pos.fx, tmp_controller_1->pos.fy, tmp_controller_1->pos.fz, tmp_controller_1->speed.fx, tmp_controller_1->speed.fy, tmp_controller_1->speed.fz);

    printf("Avant (2) : pos = %f, %f, %f | speed = %f, %f, %f\n", tmp_controller_2->pos.fx, tmp_controller_2->pos.fy, tmp_controller_2->pos.fz, tmp_controller_2->speed.fx, tmp_controller_2->speed.fy, tmp_controller_2->speed.fz);
    force3_t collision_pos_2 = LINEAR_COMBINATION(tmp_controller_2->pos, tmp_controller_2->speed, k);
    tmp_controller_2->speed = lerp_speed_from_pos(tmp_controller_2->old_pos, tmp_controller_2->pos, collision_pos_2, tmp_controller_2->old_speed, tmp_controller_2->speed);
    tmp_controller_2->pos = collision_pos_2;
    printf("Après (2) : pos = %f, %f, %f | speed = %f, %f, %f\n", tmp_controller_2->pos.fx, tmp_controller_2->pos.fy, tmp_controller_2->pos.fz, tmp_controller_2->speed.fx, tmp_controller_2->speed.fy, tmp_controller_2->speed.fz);

    printf("\n");
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
    // dt = dt > 0 ? -dt : dt;
    printf("dt = %f\n", dt);

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

        tmp_fx = -tmp_controller->speed.fx * FLUID_MU;
        tmp_fy = -tmp_controller->speed.fy * FLUID_MU;
        tmp_fz = -tmp_controller->speed.fz * FLUID_MU;

        // printf("it %d : pos.fx : %f, pos.fy : %f, pos.fz : %f\n", i, tmp_controller->pos.fx, tmp_controller->pos.fy, tmp_controller->pos.fz);

        controller_kinematics_add_force(tmp_controller, Force3(tmp_fx, tmp_fy, tmp_fz));

        // Réaction du support : si pos.fy < 0 on ajoute une force valant -1.5 * speed.fy (effet de rebond)
        // !! ne pas oublier d'implémenter les frottements solides !!
        if (tmp_controller->pos.fy <= 0.0)
        {
            tmp_controller->pos.fy = 0.0;
            if (ABS(tmp_controller->speed.fy) > 0.1) // Pas de frottements solides dynamiques si la vitesse est trop faible (~ frottements statiques)
            {
                tmp_fx = -SOLID_DYNAMIC_MU * tmp_controller->speed.fx * tmp_controller->mass / dt;
                // tmp_fx = 0.0;
                tmp_fy = -tmp_controller->speed.fy * tmp_controller->mass * 1.2 / dt;
                // tmp_fz = 0.0;
                tmp_fz = -SOLID_DYNAMIC_MU * tmp_controller->speed.fz * tmp_controller->mass / dt;

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
                // printf("Collision ! m1 = %f, m2 = %f\nv10 = %f %f %f, v20 = %f %f %f\n", tmp_controller->mass, tmp_controller_2->mass, tmp_controller->speed.fx, tmp_controller->speed.fy, tmp_controller->speed.fz, tmp_controller_2->speed.fx, tmp_controller_2->speed.fy, tmp_controller_2->speed.fz);
                // Avant de calculer la force à appliquer, il faut "téléporter" les boules de sorte à ce qu'elles ne s'intersectent pas grossièrement
                // teleport_and_update_speed(tmp_controller, tmp_controller_2);

                impact_direction = force3_sub(tmp_controller->pos, tmp_controller_2->pos);
                normalize(&impact_direction);

                impact_coef_1 = ABS(DOT_PRODUCT(impact_direction, tmp_controller->speed));
                impact_coef_2 = ABS(DOT_PRODUCT(impact_direction, tmp_controller_2->speed));

                // Faux, refaire les collisions inélastiques
                impact_coef = (1.0 - COLLISION_ENERGY_LOSS) * 0.5 * (impact_coef_1 + impact_coef_2) * tmp_controller->mass / (tmp_controller->mass + tmp_controller_2->mass);

                tmp_fx = impact_coef * impact_direction.fx * tmp_controller->mass / dt;
                tmp_fy = impact_coef * impact_direction.fy * tmp_controller->mass / dt;
                tmp_fz = impact_coef * impact_direction.fz * tmp_controller->mass / dt;

                controller_kinematics_add_force(tmp_controller, Force3(tmp_fx, tmp_fy, tmp_fz));
                impact_coef *= tmp_controller_2->mass / tmp_controller->mass;

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
    this->super.process_input = id_for_process_input;
    this->super.update = physics_manager_update;
    this->super.draw = id;

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