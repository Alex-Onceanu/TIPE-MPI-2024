#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <GLES2/gl2.h>
#include <emscripten/html5.h>

#include "modelisation/init.h"
#include "tools/maths.h"
#include "tools/constantes.h"
#include "world.h"
#include "user_event.h"

// Cette "constante" globale stockera l'ID OPENGL representant le programme
unsigned int PROGRAM_ID = 0;

// Affiche les erreurs de shader et OpenGL
void debug()
{
    // 1282 -> GL_INVALID_OPERATION
    GLenum error;
    error = glGetError();

    while (error != GL_NO_ERROR)
    {
        printf("!!\n");
        printf("%d\n", error);
        error = glGetError();
    }
}

world_p world;
double old_time = 0.0;
double time_between_frames = 0.0; // en ms
double target_time = 1.0 / 30.0;

// Une seule itération de la boucle principale
// Cette fonction est appelée 60 fois par seconde, correspond à 1 frame
EM_BOOL mainloop(double time, void *userData)
{
    time_between_frames = time - old_time;
    old_time = time;
    // printf("%f\n", time_between_frames);
    if (time_between_frames < target_time)
    {
        sleep((unsigned int)(target_time - time_between_frames));
    }

    // Process input
    world_process_input(world);

    // Update
    world_update(world);

    // Clear
    glClearColor(0.278, 0.592, 0.792, 1.0);
    // glClearColor(0.1137, 0.16, 0.31765, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw
    world_draw(world);

    return EM_TRUE;
}

int main()
{
    // init s'occupe de toutes les initialisations uniques nécessaires
    PROGRAM_ID = init();
    // init_texture s'occupe des initialisations liees aux textures...
    // init_texture(PROGRAM_ID);

    glEnable(GL_DEPTH_TEST);

    // Vecteur de lumière, qui détermine le sens d'éclairage de la scène
    float theta_incident = -PI / 2.0;
    float phi_incident = 2 * PI / 3.0;
    int u_Light = glGetUniformLocation(PROGRAM_ID, "u_Light");
    glUniform4f(u_Light, sin(phi_incident) * cos(theta_incident), sin(phi_incident) * sin(theta_incident), cos(phi_incident), 1.0);

    // Matrice projection, reste constante donc autant la faire maintenant
    mat4_t proj = projection();
    int u_Proj = glGetUniformLocation(PROGRAM_ID, "u_Proj");
    glUniformMatrix4fv(u_Proj, 1, GL_FALSE, proj.coefs);

    debug();

    // Instanciation de world (instance principale)
    // Sera parametre de mainloop sous forme de void*
    world = world_init();

    // Le mainloop est ici
    emscripten_request_animation_frame_loop(mainloop, NULL);

    //  ??? Comment free sur emscripten ? Ou le placer dans le destructeur de World ?
    // glDeleteProgram(PROGRAM_ID);

    return 0;
}

// ___________________________________________________________________________________
// Fonctions exportées, javascript y a accès pour gérer les événements utilisateur

// Pas besoin de la souris pour l'instant
void mouse_move(float x, float y)
{
    f_tuple_t *tup = malloc(sizeof(f_tuple_t));
    tup->x = x;
    tup->y = y;
    world_add_event(world, UserEvent(MOUSE_MOVED, (void *)tup));
}

void mouse_down()
{
    world_add_event(world, UserEvent(CLICK_DOWN, NULL));
}

void mouse_up()
{
    world_add_event(world, UserEvent(CLICK_UP, NULL));
}

void ar_up()
{
    world_add_event(world, UserEvent(RIGHT_ARROW_UP, NULL));
}

void ar_down()
{
    world_add_event(world, UserEvent(RIGHT_ARROW_DOWN, NULL));
}

void al_up()
{
    world_add_event(world, UserEvent(LEFT_ARROW_UP, NULL));
}

void al_down()
{
    world_add_event(world, UserEvent(LEFT_ARROW_DOWN, NULL));
}

void au_up()
{
    world_add_event(world, UserEvent(UP_ARROW_UP, NULL));
}

void au_down()
{
    world_add_event(world, UserEvent(UP_ARROW_DOWN, NULL));
}

void ad_up()
{
    world_add_event(world, UserEvent(DOWN_ARROW_UP, NULL));
}

void ad_down()
{
    world_add_event(world, UserEvent(DOWN_ARROW_DOWN, NULL));
}

void w_up()
{
    world_add_event(world, UserEvent(W_UP, NULL));
}

void w_down()
{
    world_add_event(world, UserEvent(W_DOWN, NULL));
}

void a_up()
{
    world_add_event(world, UserEvent(A_UP, NULL));
}

void a_down()
{
    world_add_event(world, UserEvent(A_DOWN, NULL));
}

void s_up()
{
    world_add_event(world, UserEvent(S_UP, NULL));
}

void s_down()
{
    world_add_event(world, UserEvent(S_DOWN, NULL));
}

void d_up()
{
    world_add_event(world, UserEvent(D_UP, NULL));
}

void d_down()
{
    world_add_event(world, UserEvent(D_DOWN, NULL));
}

void space_up()
{
    world_add_event(world, UserEvent(SPACE_UP, NULL));
}

void space_down()
{
    world_add_event(world, UserEvent(SPACE_DOWN, NULL));
}

void shift_up()
{
    world_add_event(world, UserEvent(SHIFT_UP, NULL));
}

void shift_down()
{
    world_add_event(world, UserEvent(SHIFT_DOWN, NULL));
}