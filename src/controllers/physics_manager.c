#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <GLES3/gl3.h>

#include "../tools/constantes.h"
#include "physics_manager.h"
#include "../tools/maths.h"

// Nb de frames d'attente maximum entre deux collisions d'un meme couple de boules
#define MAX_COOLDOWN 60

// Renvoie 1 si la boule c1 touche la boule c2, 0 sinon
int collision(controller_kinematics_p c1, controller_kinematics_p c2)
{
    force3_t pos_diff = force3_sub(c2->pos, c1->pos);
    return SQ_NORME2(pos_diff) < SQUARED(c1->radius + c2->radius);
}

void apply_collision_force(controller_kinematics_p c1, controller_kinematics_p c2)
{
    force3_t impact_direction = force3_sub(c1->pos, c2->pos);
    force3_t v_diff = force3_sub(c1->speed, c2->speed);

    // Si les deux boules coincident
    if (SQ_NORME2(impact_direction) <= 0.00001)
    {
        if (SQ_NORME2(v_diff) <= 0.00001)
        {
            // Deux boules immobiles exactement au même endroit, on les écarte un peu
            c1->pos.fx += c2->radius;
            c2->pos.fx -= c2->radius;
        }
        else
        {
            // On téléporte une boule en arrière pour que les deux soient distantes de teleport_dist
            const float teleport_dist = 0.1;

            controller_kinematics_p c_max = SQ_NORME2(c1->speed) > SQ_NORME2(c2->speed) ? c1 : c2;
            force3_t teleport_direction = c_max->speed;
            normalize(&teleport_direction);
            c_max->pos = LINEAR_COMBINATION(c_max->pos, teleport_direction, -teleport_dist);

            impact_direction = force3_sub(c1->pos, c2->pos);
        }
    }
    normalize(&impact_direction);

    float m1 = c1->mass;
    float m2 = c2->mass;
    float F1_norm, F2_norm;

    if (SQ_NORME2(v_diff) <= 0.00001)
    {
        // Cas où les deux boules sont en collision mais quasi-immobiles (ou avancent ensemble dans la même direction)
        printf("HUH ? LES BOUULES SONT EN COLLISION MAIS SANS BOUGER ?\n");
        return;
    }
    else
    {
        // Conservation de l'énergie cinétique
        F1_norm = -2.0 * DOT_PRODUCT(v_diff, impact_direction) * m2 * m1 / (dt * (m1 + m2));
        F2_norm = 2.0 * DOT_PRODUCT(v_diff, impact_direction) * m1 * m2 / (dt * (m1 + m2)); // = -F1_norm en fait..
    }

    force3_t F1 = force3_scale(impact_direction, F1_norm);
    force3_t F2 = force3_scale(impact_direction, F2_norm);
    force3_t impact_pos = LINEAR_COMBINATION(c2->pos, impact_direction, c2->radius);

    controller_kinematics_add_force(c1, F1, impact_pos);
    controller_kinematics_add_force(c2, F2, impact_pos);

    // C'est l'heure de faire tourner les boules qui entrent en collision (modélisation du frottement solide boule contre boule)
    force3_t spin_1 = force3_cross_product(Force3(0.0, -c1->radius, 0.0), c1->speed);
    force3_t spin_2 = force3_cross_product(Force3(0.0, -c2->radius, 0.0), c2->speed);

    // On projete ces spins sur Oy, on veut pas que ce calcul ait d'influence sur le mouvement des boules
    spin_1.fx = 0.0;
    spin_1.fz = 0.0;
    spin_2.fx = 0.0;
    spin_2.fz = 0.0;

    float spin_norm_1 = (norme2(spin_1) + norme2(spin_2)) * m2 / (m1 + m2);
    float spin_norm_2 = (norme2(spin_1) + norme2(spin_2)) * m1 / (m1 + m2);

    if(SQ_NORME2(spin_1) > 0.0001)
    {
        normalize(&spin_1);

        // Pour ajouter un moment sans force : créer une 2e force sans moment pour annuler la première (sans annuler son moment)
        controller_kinematics_add_force(c1, force3_scale(spin_1, spin_norm_1), impact_pos);
        controller_kinematics_add_force(c1, force3_scale(spin_1, -spin_norm_1), c1->pos);
    }
    if(SQ_NORME2(spin_2) > 0.0001)
    {
        normalize(&spin_2);

        controller_kinematics_add_force(c2, force3_scale(spin_2, spin_norm_2), impact_pos);
        controller_kinematics_add_force(c2, force3_scale(spin_2, -spin_norm_2), c2->pos);
    }
}

void physics_manager_update_collisions(physics_manager_p this, int nb_controllers)
{
    controller_kinematics_p tmp_controller = NULL;
    force3_t impact_direction = {};
    controller_kinematics_p tmp_controller_2 = NULL;
    float m1, m2;

    for (int i = 0; i < nb_controllers; i++)
    {
        tmp_controller = (controller_kinematics_p)vector_get_at(this->kinematic_controllers, i);
        for (int j = i + 1; j < nb_controllers; j++)
        {
            tmp_controller_2 = (controller_kinematics_p)vector_get_at(this->kinematic_controllers, j);

            // string_of_pointer_couple : stocke sous forme de string le couple des deux boules pour en faire une KEY de DICT
            char sopc[21];
            snprintf(sopc, 20, "%p%p", (void *)tmp_controller, (void *)tmp_controller_2);
            if (!appartient(this->colliding, sopc))
            {
                snprintf(sopc, 20, "%p%p", (void *)tmp_controller_2, (void *)tmp_controller);
            }

            if (collision(tmp_controller, tmp_controller_2))
            {
                printf("Collision !\n");

                // Diminuer de 1 le cooldown du couple puis continue (s'il existe), sinon appliquer la force de collision et mettre le cooldown du couple a MAX_COOLDOWN
                if (appartient(this->colliding, sopc))
                {
                    int v = rechercher(this->colliding, sopc);
                    supprimer(this->colliding, sopc);
                    inserer(this->colliding, sopc, v > 0 ? (v - 1) : MAX_COOLDOWN);
                    printf("Pas le droit d'appliquer la force : v = %d\n", v);
                    continue;
                }

                printf("Tout va bien, on a le droit d'appliquer la force\n");

                // On n'appliquera plus de force à ce couple pendant "cooldown" frames
                int cooldown = MAX_COOLDOWN;


                inserer(this->colliding, sopc, cooldown);

                apply_collision_force(tmp_controller, tmp_controller_2);
            }
            else
            {
                // Mettre leur cooldown a 0
                if (appartient(this->colliding, sopc))
                {
                    int v = rechercher(this->colliding, sopc);
                    supprimer(this->colliding, sopc);
                    // if (v > 0)
                        // inserer(this->colliding, sopc, v - 1);
                }
            }
        }
    }
}

// C'est dans cet update que physics_manager va faire subir des forces aux controlleurs qu'il gère
void physics_manager_update(controller_p this2)
{
    physics_manager_p this = (physics_manager_p)this2;
    int nb_controllers = vector_len(this->kinematic_controllers);
    controller_kinematics_p tmp_controller = NULL;
    const float boing_coef = 1.3;

    for (int i = 0; i < nb_controllers; i++)
    {
        tmp_controller = (controller_kinematics_p)vector_get_at(this->kinematic_controllers, i);
        float v = norme2(tmp_controller->speed);

        // Frottements fluides
        controller_kinematics_add_force(tmp_controller, force3_scale(tmp_controller->speed, -FLUID_MU), tmp_controller->pos);

        // La rotation doit petit a petit s'attenuer
        float w = norme2(tmp_controller->omega);
        if (w > 1.0 - ROTATION_MU)
        {
            tmp_controller->omega = force3_sub(tmp_controller->omega, force3_scale(tmp_controller->omega, (1.0 - ROTATION_MU) / norme2(tmp_controller->omega)));
        }
        else if (w > 0.001)
        {
            tmp_controller->omega = force3_scale(tmp_controller->omega, (1.0 - ROTATION_MU));
        }
        else
        {
            tmp_controller->omega = Force3(0.0, 0.0, 0.0);
        }

        if (tmp_controller->pos.fy <= 0.0)
        {
            tmp_controller->pos.fy = 0.0;
            force3_t contact_sol = force3_add(tmp_controller->pos, Force3(0.0, -tmp_controller->radius, 0.0));
            force3_t speed_proj = Force3(tmp_controller->speed.fx, 0.0, tmp_controller->speed.fz);
            float v_proj = norme2(speed_proj);

            // Rebond sur le sol (réaction du support)
            if (ABS(tmp_controller->speed.fy) > 0.1)
            {
                controller_kinematics_add_force(tmp_controller,
                                                Force3(0.0, -tmp_controller->mass * tmp_controller->speed.fy * boing_coef / dt, 0.0),
                                                contact_sol);
            }

            // Rugosité du sol (frottements aléatoires)
            if (v_proj > 0.1)
            {
                float noise_x = 0.0, noise_z = 0.0;
                if (randint(1, 3) == 1)
                {
                    float sign1 = (float)randint(0, 1) * 2.0 - 1.0;
                    float sign2 = (float)randint(0, 1) * 2.0 - 1.0;
                    noise_x = sign1 * (1.0 / 100000) * (float)randint(1, 1000);
                    noise_z = sign2 * (1.0 / 100000) * (float)randint(1, 1000);
                }

                // Vaut 1 si vx > 0, -1 sinon (idem pour z)
                float speed_direction_x = tmp_controller->speed.fx >= 0.0 ? 1.0 : -1.0;
                float speed_direction_z = tmp_controller->speed.fz >= 0.0 ? 1.0 : -1.0;

                // controller_kinematics_add_force(tmp_controller, Force3(v * noise_x, 0.0, v * noise_z),
                // tmp_controller->pos);
            }

            // Frottements solides : de norme μ * m * g tant que la vitesse et grande
            if (v_proj > SOLID_MU * tmp_controller->mass * GRAVITY)
            {
                force3_t frottements_solides = force3_scale(speed_proj,
                                                            -SOLID_MU * tmp_controller->mass * GRAVITY / v_proj);

                controller_kinematics_add_force(tmp_controller, frottements_solides, contact_sol);
            }
            // Deviennent des frottements linéaires quand la vitesse est faible
            if (v_proj > 0.001)
            {
                force3_t frottements_solides = force3_scale(speed_proj, -SOLID_MU);

                controller_kinematics_add_force(tmp_controller, frottements_solides, contact_sol);
            }
            else
            {
                tmp_controller->speed = Force3(0.0, tmp_controller->speed.fy, 0.0);
            }

            // C'est l'heure de faire avancer les boules qui roulent
            force3_t NR = Force3(0.0, tmp_controller->radius, 0.0);                 // Vecteur normal au sol de norme R
            force3_t rolling = force3_cross_product(tmp_controller->omega, NR);     // De norme R * w * sin(w, N)

            controller_kinematics_add_force(tmp_controller, rolling, contact_sol);
        }
        else if (v > 0.01 || tmp_controller->pos.fy > 0.01)
        {
            // On applique le poids uniquement si l'objet est au-dessus du sol
            controller_kinematics_add_force(tmp_controller, Force3(0.0, -tmp_controller->mass * GRAVITY, 0.0),
                                            tmp_controller->pos);
        }
    }

    // On gère les collisions
    physics_manager_update_collisions(this, nb_controllers);
}

physics_manager_p Physics_manager()
{
    physics_manager_p this = malloc(sizeof(physics_manager_t));
    this->super.process_input = id_for_process_input;
    this->super.update = physics_manager_update;
    this->super.draw = id;

    this->kinematic_controllers = vector_init();

    this->colliding = dict_vide(1000);

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