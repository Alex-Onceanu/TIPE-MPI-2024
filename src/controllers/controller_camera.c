#include <stdlib.h>
#include <stdio.h>

#include "controller_camera.h"
#include "../tools/maths.h"
#include "../tools/constantes.h"
#include "../user_event.h"
#include "../tools/vector.h"
#include "../modelisation/model_3D.h"
#include <GLES3/gl3.h>

void controller_camera_process_input(controller_p this, void *data)
{
    controller_camera_p this2 = (controller_camera_p)this;
    vector_p events_vector = (vector_p)data;
    user_event_p actuel;
    f_tuple_t *tup;

    // !!!! CHANGER ÇA : il faut pas pop tous les event sinon les autres entités reçoivent le vecteur NULL
    while (vector_len(events_vector) > 0)
    {
        actuel = (user_event_p)vector_pop(events_vector);

        switch (actuel->type)
        {
        case CLICK_DOWN:
            this2->clicks = true;
            break;
        case CLICK_UP:
            this2->clicks = false;
            this2->mouse_x = 0.0;
            this2->mouse_y = 0.0;
            break;
        case MOUSE_MOVED:
            tup = (f_tuple_t *)actuel->data;
            this2->old_mouse_x = this2->mouse_x;
            this2->old_mouse_y = this2->mouse_y;
            this2->mouse_x = tup->x;
            this2->mouse_y = tup->y;
            free(tup);
            break;
        case W_DOWN:
            this2->v_z = -this2->v;
            break;
        case W_UP:
            this2->v_z = 0.0;
            break;
        case A_DOWN:
            this2->v_x = -this2->v;
            break;
        case A_UP:
            this2->v_x = 0.0;
            break;
        case S_DOWN:
            this2->v_z = this2->v;
            break;
        case S_UP:
            this2->v_z = 0.0;
            break;
        case D_DOWN:
            this2->v_x = this2->v;
            break;
        case D_UP:
            this2->v_x = 0.0;
            break;
        case SPACE_DOWN:
            this2->v_y = this2->v;
            break;
        case SPACE_UP:
            this2->v_y = 0.0;
            break;
        case SHIFT_DOWN:
            this2->v_y = -this2->v;
            break;
        case SHIFT_UP:
            this2->v_y = 0.0;
            break;
        default:
            break;
        }

        free(actuel);
    }
}

void controller_camera_update(controller_p this)
{
    controller_camera_p this2 = (controller_camera_p)this;

    float dvx = this2->v_x * cos(this2->theta_x) + this2->v_z * sin(-this2->theta_x);
    float dvy = this2->v_y;
    float dvz = this2->v_z * cos(this2->theta_x) - this2->v_x * sin(-this2->theta_x);

    this2->direction.fx = sin(this2->theta_x) * cos(this2->theta_y);
    this2->direction.fy = sin(this2->theta_y);
    this2->direction.fz = -cos(this2->theta_x);

    this2->pos.fx += dvx * dt;
    this2->pos.fy += dvy * dt;
    this2->pos.fz += dvz * dt;

    for (int i = 0; i < NB_PROGRAMS; i++)
    {
        glUseProgram(PROGRAM_ID[i]);
        int u_CameraPos = glGetUniformLocation(PROGRAM_ID[i], "u_CameraPos");
        glUniform3f(u_CameraPos, this2->pos.fx, this2->pos.fy, this2->pos.fz);
    }

    Clamp(&(this2->pos.fy), -0.0, 40.0);

    if (!this2->clicks || (this2->old_mouse_x == 0.0 && this2->old_mouse_y == 0.0) || (this2->mouse_x - this2->old_mouse_x <= 0.0001 && this2->mouse_x - this2->old_mouse_x >= -0.0001))
        return;

    this2->theta_x += 0.2 * PI * (this2->mouse_x - this2->old_mouse_x) * dt;
    this2->theta_y += 0.2 * PI * (this2->mouse_y - this2->old_mouse_y) * dt;

    Clamp(&(this2->theta_y), -PI / 2.0, PI / 2.0);

    // printf("Theta_x : %f, theta_y : %f\n", this2->theta_x, this2->theta_y);
}

void update_crosshair(controller_camera_p this)
{
    unsigned int cr_program = PROGRAM_ID[CROSSHAIR_PROGRAM];
    force3_t throw_direction = LINEAR_COMBINATION(this->direction, Force3(0.0, 1.0, 0.0), sin(THROW_ANGLE));
    normalize(&throw_direction);

    mat4_t P; // Envoie vec{e_x} sur throw_direction
    mat4_rotation(throw_direction, &P);
    

    force3_t crosshair_pos = LINEAR_COMBINATION(this->pos, this->direction, 10.0);
    mat4_t crosshair_tr = translation(crosshair_pos.fx, crosshair_pos.fy, crosshair_pos.fz);

    glUseProgram(cr_program);

    int u_Rotation = glGetUniformLocation(cr_program, "u_Rotation");
    glUniformMatrix4fv(u_Rotation, 1, GL_FALSE, mat4_get(&P));

    int u_Translation = glGetUniformLocation(cr_program, "u_Translation");
    glUniformMatrix4fv(u_Translation, 1, GL_FALSE, mat4_get(&crosshair_tr));

    // Parce que pas la peine de faire encore un shader.. on utilise le même que pour axis.frag donc il faut les mêmes varying
    int u_Omega = glGetUniformLocation(cr_program, "u_Omega");
    glUniform1f(u_Omega, THROW_SPEED - 0.3);
}

void controller_camera_draw(controller_p this)
{
    controller_camera_p this2 = (controller_camera_p)this;
    // inv_view est la matrice de transform de la caméra, pour avoir la vraie matrice view,
    // On utilisera son inverse (car on veut qu'en la multipliant par la transform de caméra, on obtienne l'identité)
    mat4_t inv_view = translation(this2->pos.fx, this2->pos.fy, this2->pos.fz);
    mat4_t rotations = mat4_produit(rotation_y_4(-this2->theta_x), rotation_x_4(this2->theta_y));
    mat4_ajoute_inplace(&inv_view, mat4_ajoute(rotations, mat4_scalaire(mat4_id_t(), -1.0)));

    mat4_t view = mat4_inverse(inv_view);
  
    for (int i = 0; i < NB_PROGRAMS; i++)
    {
        glUseProgram(PROGRAM_ID[i]);
        // Matrice view : déplacements et rotations de la caméra dans le sens inverse
        int u_View = glGetUniformLocation(PROGRAM_ID[i], "u_View");
        glUniformMatrix4fv(u_View, 1, GL_FALSE, view.coefs);
    }

    update_crosshair(this2);
    model_3D_draw((model_3D_t){CROSSHAIR_BUF, NO_TEXTURE}, Materiau(SOLEIL), CROSSHAIR_PROGRAM);

    this2->old_mouse_x = this2->mouse_x;
    this2->old_mouse_y = this2->mouse_y;
}

controller_camera_p Controller_camera(force3_t pos0, force3_t dir0)
{
    controller_camera_p this = malloc(sizeof(controller_camera_t));
    this->super.process_input = controller_camera_process_input;
    this->super.update = controller_camera_update;
    this->super.draw = controller_camera_draw;

    this->pos = pos0;
    this->v = 0.2;
    this->v_x = 0.0;
    this->v_y = 0.0;
    this->v_z = 0.0;
    this->theta_x = 0.;
    this->theta_y = 0.;
    this->w = 0.003;
    this->w_x = 0.0;
    this->w_y = 0.0;
    this->clicks = false;
    this->mouse_x = 0.0;
    this->mouse_y = 0.0;
    this->old_mouse_x = 0.0;
    this->old_mouse_y = 0.0;
    this->old_time = 0.0;

    this->direction = dir0;

    return this;
}