#include <stdlib.h>
#include <stdio.h>

#include "controller_camera.h"
#include "../tools/maths.h"
#include "../tools/constantes.h"
#include "../user_event.h"
#include "../tools/vector.h"
#include "../modelisation/model_3D.h"
#include <GLES3/gl3.h>

void controller_camera_process_input(controller_p __this, void *data)
{
    controller_camera_p this = (controller_camera_p)__this;
    vector_p events_vector = (vector_p)data;
    user_event_p current;
    f_tuple_t *tup;

    int n_events = vector_len(events_vector);
    for (int i = 0; i < n_events; i++)
    {
        current = (user_event_p)vector_get_at(events_vector, i);

        switch (current->type)
        {
            case CLICK_DOWN:
                this->clicks = true;
                break;
            case CLICK_UP:
                this->clicks = false;
                this->mouse_x = 0.0;
                this->mouse_y = 0.0;
                break;
            // On est certain que le déplacement de la souris ne sera géré que par la caméra (principale)
            case MOUSE_MOVED:
                tup = (f_tuple_t *)current->data;
                this->old_mouse_x = this->mouse_x;
                this->old_mouse_y = this->mouse_y;
                this->mouse_x = tup->x;
                this->mouse_y = tup->y;
                free(tup);
                break;
            case W_DOWN:
                this->speed.fz = -this->v;
                break;
            case W_UP:
                this->speed.fz = 0.0;
                break;
            case A_DOWN:
                this->speed.fx = -this->v;
                break;
            case A_UP:
                this->speed.fx = 0.0;
                break;
            case S_DOWN:
                this->speed.fz = this->v;
                break;
            case S_UP:
                this->speed.fz = 0.0;
                break;
            case D_DOWN:
                this->speed.fx = this->v;
                break;
            case D_UP:
                this->speed.fx = 0.0;
                break;
            case SPACE_DOWN:
                this->speed.fy = this->v;
                break;
            case SPACE_UP:
                this->speed.fy = 0.0;
                break;
            case SHIFT_DOWN:
                this->speed.fy = -this->v;
                break;
            case SHIFT_UP:
                this->speed.fy = 0.0;
                break;
            default:
                break;
        }
    }
}

void controller_camera_update(controller_p __this)
{
    controller_camera_p this = (controller_camera_p)__this;

    float dvx = this->speed.fx * cos(this->theta_x) + this->speed.fz * sin(-this->theta_x);
    float dvy = this->speed.fy;
    float dvz = this->speed.fz * cos(this->theta_x) - this->speed.fx * sin(-this->theta_x);

    this->direction.fx = sin(this->theta_x) * cos(this->theta_y);
    this->direction.fy = sin(this->theta_y);
    this->direction.fz = -cos(this->theta_x);

    this->pos.fx += dvx * dt;
    this->pos.fy += dvy * dt;
    this->pos.fz += dvz * dt;

    for (int i = 0; i < NB_PROGRAMS; i++)
    {
        glUseProgram(PROGRAM_ID[i]);
        int u_CameraPos = glGetUniformLocation(PROGRAM_ID[i], "u_CameraPos");
        glUniform3f(u_CameraPos, this->pos.fx, this->pos.fy, this->pos.fz);
    }

    Clamp(&(this->pos.fy), -0.0, 40.0);

    if (!this->clicks || (this->old_mouse_x == 0.0 && this->old_mouse_y == 0.0) || (this->mouse_x - this->old_mouse_x <= 0.0001 && this->mouse_x - this->old_mouse_x >= -0.0001))
        return;

    this->theta_x += 0.2 * PI * (this->mouse_x - this->old_mouse_x) * dt;
    this->theta_y += 0.2 * PI * (this->mouse_y - this->old_mouse_y) * dt;

    Clamp(&(this->theta_y), -PI / 2.0, PI / 2.0);
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

void controller_camera_draw(controller_p __this)
{
    controller_camera_p this = (controller_camera_p)__this;
    // inv_view est la matrice de transform de la caméra, pour avoir la vraie matrice view,
    // On utilisera son inverse (car on veut qu'en la multipliant par la transform de caméra, on obtienne l'identité)
    mat4_t inv_view = translation(this->pos.fx, this->pos.fy, this->pos.fz);
    mat4_t rotations = mat4_produit(rotation_y_4(-this->theta_x), rotation_x_4(this->theta_y));
    mat4_ajoute_inplace(&inv_view, mat4_ajoute(rotations, mat4_scalaire(mat4_id_t(), -1.0)));

    mat4_t view = mat4_inverse(inv_view);
  
    for (int i = 0; i < NB_PROGRAMS; i++)
    {
        glUseProgram(PROGRAM_ID[i]);
        // Matrice view : déplacements et rotations de la caméra dans le sens inverse
        int u_View = glGetUniformLocation(PROGRAM_ID[i], "u_View");
        glUniformMatrix4fv(u_View, 1, GL_FALSE, view.coefs);
    }

    update_crosshair(this);
    model_3D_draw((model_3D_t){CROSSHAIR_BUF, NO_TEXTURE}, Materiau(SOLEIL), CROSSHAIR_PROGRAM);

    this->old_mouse_x = this->mouse_x;
    this->old_mouse_y = this->mouse_y;
}

controller_camera_p Controller_camera(force3_t pos0, force3_t dir0)
{
    controller_camera_p this = malloc(sizeof(controller_camera_t));
    this->super.process_input = controller_camera_process_input;
    this->super.update = controller_camera_update;
    this->super.draw = controller_camera_draw;
    this->super.free = controller_free;

    this->pos = pos0;
    this->v = 0.2;
    this->speed = Force3(0.0, 0.0, 0.0);
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