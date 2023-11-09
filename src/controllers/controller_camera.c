#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "controller_camera.h"
#include "../tools/maths.h"
#include "../tools/constantes.h"
#include "../user_event.h"
#include "../tools/vector.h"
#include <GLES2/gl2.h>

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
        case UP_ARROW_UP:
            this2->w_x = 0.0;
            break;
        case UP_ARROW_DOWN:
            this2->w_x = this2->w;
            break;
        case DOWN_ARROW_UP:
            this2->w_x = 0.0;
            break;
        case DOWN_ARROW_DOWN:
            this2->w_x = -this2->w;
            break;
        case LEFT_ARROW_UP:
            this2->w_y = 0.0;
            break;
        case LEFT_ARROW_DOWN:
            this2->w_y = this2->w;
            break;
        case RIGHT_ARROW_UP:
            this2->w_y = 0.0;
            break;
        case RIGHT_ARROW_DOWN:
            this2->w_y = -this2->w;
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

    double current_time = (double)clock() / (double)CLOCKS_PER_SEC;
    double time_between_frames = current_time - this2->old_time;
    double target_time = 1.0 / FPS;
    this2->old_time = current_time;
    double dt = 1.0;
    if (time_between_frames > target_time)
    {
        dt = time_between_frames / target_time;
    }

    this2->direction_x = this2->v_x * cos(this2->theta_x) + this2->v_z * sin(-this2->theta_x);
    this2->direction_y = this2->v_y;
    this2->direction_z = this2->v_z * cos(this2->theta_x) - this2->v_x * sin(-this2->theta_x);

    this2->x += this2->direction_x * dt;
    this2->y += this2->direction_y * dt;
    this2->z += this2->direction_z * dt;

    normalize(&(this2->direction_x), &(this2->direction_y), &(this2->direction_z));

    for(int i = 0; i < NB_PROGRAMS; i++)
    {
        int u_CameraPos = glGetUniformLocation(PROGRAM_ID[i], "u_CameraPos");
        glUniform3f(u_CameraPos, this2->x, this2->y, this2->z);
    }

    Clamp(&(this2->y), -0.0, 40.0);

    if (!this2->clicks || (this2->old_mouse_x == 0.0 && this2->old_mouse_y == 0.0) || (this2->mouse_x - this2->old_mouse_x <= 0.0001 && this2->mouse_x - this2->old_mouse_x >= -0.0001))
        return;

    this2->theta_x += 0.2 * PI * (this2->mouse_x - this2->old_mouse_x) * dt;
    this2->theta_y += 0.2 * PI * (this2->mouse_y - this2->old_mouse_y) * dt;

    Clamp(&(this2->theta_y), -PI / 2.0, PI / 2.0);
}

void controller_camera_draw(controller_p this)
{
    controller_camera_p this2 = (controller_camera_p)this;
    // inv_view est la matrice de transform de la caméra, pour avoir la vraie matrice view,
    // On utilisera son inverse (car on veut qu'en la multipliant par la transform de caméra, on obtienne l'identité)
    mat4_t inv_view = translation(this2->x, this2->y, this2->z);
    mat4_t rotations = mat4_produit(rotation_y_4(-this2->theta_x), rotation_x_4(this2->theta_y));
    mat4_ajoute_inplace(&inv_view, mat4_ajoute(rotations, mat4_scalaire(mat4_id_t(), -1.0)));

    mat4_t view = mat4_inverse(inv_view);

    for(int i = 0; i < NB_PROGRAMS; i++)
    {
        glUseProgram(PROGRAM_ID[i]);
        // Matrice view : déplacements et rotations de la caméra dans le sens inverse
        int u_View = glGetUniformLocation(PROGRAM_ID[i], "u_View");
        glUniformMatrix4fv(u_View, 1, GL_FALSE, view.coefs);
    }

    this2->old_mouse_x = this2->mouse_x;
    this2->old_mouse_y = this2->mouse_y;
}

// Fait pointer direction_ptr vers l'attribut "direction" de cette instance
controller_camera_p Controller_camera(float x0, float y0, float z0)
{
    controller_camera_p this = malloc(sizeof(controller_camera_t));
    this->super.process_input = controller_camera_process_input;
    this->super.update = controller_camera_update;
    this->super.draw = controller_camera_draw;

    this->x = x0;
    this->y = y0;
    this->z = z0;
    this->v = 0.05;
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

    this->direction_x = 0.0;
    this->direction_y = 0.0;
    this->direction_z = 0.0;

    return this;
}