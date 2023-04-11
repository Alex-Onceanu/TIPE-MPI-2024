#include <stdio.h>
#include <stdlib.h>

#include <GLES2/gl2.h>
#include <emscripten/html5.h>

#include "init.h"
#include "maths.h"

#define PI 3.14156

// Une instance unique de cette structure est créée à l'initialisation de l'app
// On envoie au mainloop un pointeur vers cette instance, qui contient tout
// ce qui est nécessaire à garder en mémoire entre les frames
struct app_data
{
    unsigned int program;
    int compt;
    int step;
};

// Une seule itération de la boucle principale
// Cette fonction est appelée 60 fois par seconde, correspond à 1 frame
EM_BOOL mainloop(double time, void *userData)
{
    // Ruse de sioux, métamorphose d'un void* en struct app_data
    struct app_data *app = (struct app_data *)userData;
    if (app->compt % 100 == 0)
        app->step *= -1;

    app->compt += app->step;
    float brightness = app->compt / 100.0;

    int u_Brightness = glGetUniformLocation(app->program, "u_Brightness");
    glUniform4f(u_Brightness, brightness, brightness, brightness, 1.0);

    int u_Transform = glGetUniformLocation(app->program, "u_Transform");
    float theta = 2 * PI * (app->compt / 100.0);

    // const float rotation[] = {
    //     cos(theta), -sin(theta), 0.0, 0.0,
    //     sin(theta), cos(theta), 0.0, 0.0,
    //     0.0, 0.0, 1.0, 0.0,
    //     0.0, 0.0, 0.0, 1.0};

    const float translation[] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0};

    glUniformMatrix4fv(u_Transform, 1, GL_FALSE, translation);

    // Clear
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    return EM_TRUE;
}

// S'occupe de tous les free
void destructeur(struct app_data *app)
{
    glDeleteProgram(app->program);
    free(app);
}

#define NB_VERTEX 24
#define NB_INDEX 6

int main()
{
    // init s'occupe de toutes les initialisations uniques nécessaires
    // surtout pour tout ce qui traite des shaders
    unsigned int program = init();

    // Triangle ici, 3 sommets (vertex) 3d (1 ligne = 1 vertex)
    float vertex[NB_VERTEX] = {
        // x, y, z, r, g, b
        -0.5, -0.5, 0.0, 1.0, 0.0, 0.0,
        0.5, -0.5, 0.0, 0.0, 1.0, 0.0,
        -0.5, 0.5, 0.0, 0.0, 0.0, 1.0,
        0.5, 0.5, 0.0, 1.0, 1.0, 0.0};

    // Vertex buffer, on envoie à OpenGL les données du triangle
    unsigned int triang_buf;
    glGenBuffers(1, &triang_buf);
    glBindBuffer(GL_ARRAY_BUFFER, triang_buf);
    glBufferData(GL_ARRAY_BUFFER, NB_VERTEX * sizeof(float), vertex, GL_STATIC_DRAW);

    // Puis on lui dit comment interpréter ces données
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    // Couleur
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

    // Index buffer, pour éviter les doublons de vertex
    unsigned int indices[NB_INDEX] = {
        0, 1, 2, 1, 3, 2};

    // Bind et interprétation du index_buffer
    unsigned int ind_buf;
    glGenBuffers(1, &ind_buf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NB_INDEX * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Instanciation d'un app_data
    struct app_data *app = malloc(sizeof(struct app_data));
    app->program = program;
    app->compt = 0;
    app->step = -1;

    // Le mainloop est ici
    emscripten_request_animation_frame_loop(mainloop, (void *)app);

    return 0;
}