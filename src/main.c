#include <stdio.h>
#include <stdlib.h>

#include <GLES2/gl2.h>
#include <emscripten/html5.h>

#include "init.h"
#include "maths.h"

#define PI 3.141592653589793

#define NB_VERTEX 32
#define NB_ATTRIBS_PAR_VERTEX 8
#define NB_INDEX 12
#define NB_MAT 2

// Une instance unique de cette structure est créée à l'initialisation de l'app
// On envoie au mainloop un pointeur vers cette instance, qui contient tout
// ce qui est nécessaire à garder en mémoire entre les frames
struct app_data
{
    unsigned int program;
    int compt;
    int step;
    mat4_p *transformation;
};

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

// Une seule itération de la boucle principale
// Cette fonction est appelée 60 fois par seconde, correspond à 1 frame
EM_BOOL mainloop(double time, void *userData)
{
    // Ruse de sioux, métamorphose d'un void* en struct app_data
    struct app_data *app = (struct app_data *)userData;
    if (app->compt % 100 == 0 && app->compt != 0)
    {
        app->step *= -1;
        mat4_scalaire_inplace(app->transformation[1], -1.0);
    }
    // app->compt %= 200;

    app->compt += app->step;
    float brightness = abs(app->compt) / 100.0;

    int u_Brightness = glGetUniformLocation(app->program, "u_Brightness");
    glUniform4f(u_Brightness, brightness, brightness, brightness, 1.0);

    float theta = PI * (app->compt / 100.0);
    mat4_ajoute_inplace(app->transformation[0], *app->transformation[1]);
    // mat4_affiche(*app->transformation[0]);

    mat4_set_theta_y(app->transformation[0], theta);
    int u_Model = glGetUniformLocation(app->program, "u_Model");
    glUniformMatrix4fv(u_Model, 1, GL_FALSE, mat4_get(app->transformation[0]));

    // Clear
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw
    glDrawElements(GL_TRIANGLES, NB_INDEX, GL_UNSIGNED_INT, NULL);

    return EM_TRUE;
}

// S'occupe de tous les free
void destructeur(struct app_data *app)
{
    glDeleteProgram(app->program);
    free(app);
}

int main()
{
    // init s'occupe de toutes les initialisations uniques nécessaires
    // surtout pour tout ce qui traite des shaders
    unsigned int program = init();

    // Triangle ici, 3 sommets (vertex) 3d (1 ligne = 1 vertex)
    float vertex[NB_VERTEX] = {
        // x, y, z, r, g, b, tex_x, tex_y
        -0.5, -0.433, -0.286, 1.0, 0.0, 0.0, 0.0, 0.0,
        0.5, -0.433, -0.286, 0.0, 1.0, 0.0, 0.5, 0.0,
        0.0, 0.433, 0.0, 1.0, 1.0, 1.0, 0.5, 1.0,
        0.0, -0.433, 0.577, 0.0, 0.0, 1.0, 1.0, 0.0};

    // trouver des coordonnees de monde a mettre a la pyramide puis tester proj

    // float vertex[NB_VERTEX] = {
    //     // x, y, z, r, g, b, tex_x, tex_y
    //     500.0, 0.0, 400.0, 1.0, 0.0, 0.0, 0.0, 0.0,
    //     1500.0, 0.0, 400.0, 0.0, 1.0, 0.0, 0.5, 0.0,
    //     1000.0, 1558.6, 800.0, 1.0, 1.0, 1.0, 0.5, 1.0,
    //     1000.0, 0.0, 1200.0, 0.0, 0.0, 1.0, 1.0, 0.0};

    // Vertex buffer, on envoie à OpenGL les données du triangle
    unsigned int triang_buf;
    glGenBuffers(1, &triang_buf);
    glBindBuffer(GL_ARRAY_BUFFER, triang_buf);
    glBufferData(GL_ARRAY_BUFFER, NB_VERTEX * sizeof(float), vertex, GL_STATIC_DRAW);

    // Puis on lui dit comment interpréter ces données
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NB_ATTRIBS_PAR_VERTEX * sizeof(float), 0);
    // Couleur
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, NB_ATTRIBS_PAR_VERTEX * sizeof(float), (void *)(3 * sizeof(float)));
    // Texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, NB_ATTRIBS_PAR_VERTEX * sizeof(float), (void *)(6 * sizeof(float)));

    // Index buffer, pour éviter les doublons de vertex
    unsigned int indices[NB_INDEX] = {
        0, 1, 2, 1, 3, 2, 0, 3, 2, 0, 1, 3};
    // 0, 1, 2, 1, 3, 2, 3, 0, 2};

    // Bind et interprétationß du index_buffer
    unsigned int ind_buf;
    glGenBuffers(1, &ind_buf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind_buf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NB_INDEX * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    init_texture(program);

    // Instanciation d'un app_data
    struct app_data *app = malloc(sizeof(struct app_data));
    app->program = program;
    app->compt = 0;
    app->step = -1;
    app->transformation = malloc(NB_MAT * sizeof(mat4_p));

    for (int i = 0; i < NB_MAT; ++i)
    {
        app->transformation[i] = mat4_id_p();
    }

    glEnable(GL_DEPTH_TEST);

    // Matrice view : déplacements de la caméra dans le sens inverse
    mat4_t view = mat4_id_t();
    int u_View = glGetUniformLocation(app->program, "u_View");
    glUniformMatrix4fv(u_View, 1, GL_FALSE, view.coefs);

    mat4_t proj = projection();
    int u_Proj = glGetUniformLocation(app->program, "u_Proj");
    glUniformMatrix4fv(u_Proj, 1, GL_FALSE, proj.coefs);

    // /* pour obtenir la matrice de translation 3d
    // 0 0 0 kx
    // 0 0 0 ky
    // 0 0 0 kz
    // 0 0 0 0 */
    // *app->transformation[1] = translation(-0.008, 0.005, 0.001);
    *app->transformation[0] = translation(-0.000, 0.000, -10.0);
    *app->transformation[1] = translation(0.008, 0.000, 0.040);
    mat4_ajoute_inplace(app->transformation[1], mat4_scalaire(mat4_id_t(), -1));

    debug();

    // Le mainloop est ici
    emscripten_request_animation_frame_loop(mainloop, (void *)app);

    debug();

    return 0;
}