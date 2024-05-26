#include <GLES3/gl3.h>
#include <emscripten/html5.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#include "modelisation/init.h"
#include "tools/maths.h"
#include "tools/constantes.h"
#include "world.h"
#include "user_event.h"

// Ce tableau global stockera les ID OPENGL representant les deux "programmes" (= vertex shader + fragment shader)
unsigned int PROGRAM_ID[NB_PROGRAMS] = {0};

// Ces tableaux globaux stockeront des ID OPENGL représentant des vertex buffer et index buffer préchargés
// Ainsi il suffit de charger les buffers d'une sphère une seule fois au début, puis seulement les bind à chaque draw call
// Ces tableaux sont remplis au moment de l'appel de init_buffers() (voir fichier init.h)
unsigned int NB_VERTEX_PER_BUFFER[NB_BUFFERS] = {0};
unsigned int VERTEX_BUFFER_ID[NB_BUFFERS] = {0};
unsigned int NB_INDEX_PER_BUFFER[NB_BUFFERS] = {0};
unsigned int INDEX_BUFFER_ID[NB_BUFFERS] = {0};

float SUN_X = SUN_X_0;
float SUN_Y = SUN_Y_0;
float SUN_Z = SUN_Z_0;

float LIGHT_COLOR[3] = {1.0, 1.0, 1.0};
float AMBIENT_INTENSITY = 1.0;
float BALL_REFLECTIVITY = BALL_REFLECTIVITY_0;
bool SHOULD_RENDER_AXIS = true;

float GRAVITY = GRAVITY_0;
float FLUID_MU = FLUID_MU_0;
float SOLID_MU = SOLID_MU_0;
float ROTATION_MU = ROTATION_MU_0;
float THROW_SPEED = THROW_SPEED_0;
float THROW_ANGLE = THROW_ANGLE_0;
float BALL_MASS = BALL_MASS_0;

// Affiche les erreurs de shader et OpenGL
void debug()
{
    // 1282 -> GL_INVALID_OPERATION
    GLenum error;
    error = glGetError();

    while (error != GL_NO_ERROR)
    {
        printf("%d\n", error);
        error = glGetError();
    }
}

world_p world;

double old_time = 0.0;
double dt = 1.0; // en s ici

void update_dt(double time)
{
    float old_dt = dt;
    dt = time - old_time;
    old_time = time;

    dt *= 60;  // On accélère le temps, sinon tout est super lent

    if (dt > 5.0)
    {
        // Pour ne pas avoir d'explosion surprise..
        dt = old_dt;
    }

    // if(dt > 0.6)
        // printf("dt = %f\n", dt);
}

// Une seule itération de la boucle principale
// Cette fonction est appelée 60 fois par seconde, correspond à 1 frame
EM_BOOL mainloop(double time, void *userData)
{
    update_dt(time / 1000.0); // pour que time soit en secondes

    // Process input : gérer l'entrée utilisateur
    world_process_input(world);

    // Update : passer de l'état (t) à l'état (t + dt)
    world_update(world);

    // Draw : rendu de la scène à l'état actuel
    world_draw(world);

    debug();

    return EM_TRUE;
}

int main()
{
    srand(time(NULL));
    // init s'occupe de toutes les initialisations uniques nécessaires (compiler les shaders, charger les modèles 3D..)
    init();

    glEnable(GL_DEPTH_TEST);
    // Éviter le Z-fighting
    glDepthFunc(GL_LEQUAL);
    // Pour la transparence
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < NB_PROGRAMS; i++)
    {
        glUseProgram(PROGRAM_ID[i]);

        // Matrice projection, reste constante donc autant la faire maintenant
        mat4_t proj = projection();
        int u_Proj = glGetUniformLocation(PROGRAM_ID[i], "u_Proj");
        glUniformMatrix4fv(u_Proj, 1, GL_FALSE, proj.coefs);
    }

    // Instanciation de world (instance principale)
    world = world_init();

    // Le mainloop est ici
    emscripten_request_animation_frame_loop(mainloop, NULL);

    //  ??? Comment free sur emscripten ? Ou le placer dans le destructeur de World ?

    return 0;
}

// ___________________________________________________________________________________
// Fonctions exportées, javascript y a accès pour gérer les événements utilisateur

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

void e_up()
{
    world_add_event(world, UserEvent(E_UP, NULL));
}

void e_down()
{
    world_add_event(world, UserEvent(E_DOWN, NULL));
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

void c_gravity(float e)
{
    GRAVITY = e;
}

void c_fluid_mu(float e)
{
    FLUID_MU = e;
}

void c_solid_mu(float e)
{
    SOLID_MU = e;
}

void c_rotation_mu(float e)
{
    ROTATION_MU = e;
}

void c_ball_mass(float e)
{
    BALL_MASS = e;
}

void c_throw_speed(float e)
{
    THROW_SPEED = e;
}

void c_throw_angle(float e)
{
    THROW_ANGLE = e;
}

void c_light_color_r(float e)
{
    LIGHT_COLOR[0] = e;
}

void c_light_color_g(float e)
{
    LIGHT_COLOR[1] = e;
}

void c_light_color_b(float e)
{
    LIGHT_COLOR[2] = e;
}

void c_ambient_intensity(float e)
{
    AMBIENT_INTENSITY = e;
}

void c_ball_reflectivity(float e)
{
    BALL_REFLECTIVITY = e;
}

void toggle_should_render_axis()
{
    SHOULD_RENDER_AXIS = !SHOULD_RENDER_AXIS;
}