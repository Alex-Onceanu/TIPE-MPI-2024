#include <stdio.h>
#include <stdlib.h>

#include <GLES2/gl2.h>
#include <emscripten/html5.h>

#include "init.h"
#include "maths.h"

#include "./lib/stb_image/stb_image.h"

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
    matrix_p *transformation;
};

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

// Une seule itération de la boucle principale
// Cette fonction est appelée 60 fois par seconde, correspond à 1 frame
EM_BOOL mainloop(double time, void *userData)
{
    // Ruse de sioux, métamorphose d'un void* en struct app_data
    struct app_data *app = (struct app_data *)userData;
    // if (app->compt % 100 == 0 && app->compt != 0)
    // {
    //     app->step *= -1;
    //     mat_scalaire(app->transformation[1], -1.0);
    // }
    app->compt %= 200;

    app->compt += app->step;
    float brightness = abs(app->compt) / 100.0;

    int u_Brightness = glGetUniformLocation(app->program, "u_Brightness");
    glUniform4f(u_Brightness, brightness, brightness, brightness, 1.0);

    float theta = PI * (app->compt / 100.0);
    mat_ajoute(app->transformation[0], *app->transformation[1]);
    // mat_affiche(app->transformation[0]);

    mat_set_theta_y_4(app->transformation[0], theta);
    int u_Transform = glGetUniformLocation(app->program, "u_Transform");
    glUniformMatrix4fv(u_Transform, 1, GL_FALSE, mat_get(app->transformation[0]));

    // mat_free(tmp);
    // mat_free(tmp2);
    // mat_free(app->transformation[0]);

    // Clear
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw
    glDrawElements(GL_TRIANGLES, NB_INDEX, GL_UNSIGNED_INT, NULL);

    return EM_TRUE;
}

void init_texture(unsigned int program)
{
    // Pas la peine, GL_REPEAT est bien et est par défaut
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    FILE *fichier = fopen("../res/textures/grass.png", "r");

    if (fichier == NULL)
    {
        printf("Pas d'ouverture du tout !\n");
    }
    else
    {
        printf("Ben oui c'est bon\n");
    }

    fclose(fichier);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load("../res/textures/grass.png", &width, &height, &nrChannels, 0);

    if (!data)
    {
        printf("ERR : l'image png n'a pu être chargée : data = %s\n", data);
        debug();
        printf("%s\n", stbi_failure_reason());
    }
    else
    {
        printf("l'image a pu être chargée !\n");
    }

    // Chaque texture chargée est associée à un uint, qu'on stocke dans un
    // unsigned int[], ici comme il y en a 1 seul on envoie &texture
    unsigned int texture;
    // Génération du texture object
    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0); // Activer texture unit
    glBindTexture(GL_TEXTURE_2D, texture);

    // On attache une image 2d à un texture object
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Voir définition d'un mipmap
    glGenerateMipmap(GL_TEXTURE_2D); // INVALID_OPERATION

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(glGetUniformLocation(program, "u_Texture"), 0);

    // équivalent à juste écrire free(data)
    stbi_image_free(data);
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
    // 0.5, 0.5, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0};

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

    // Bind et interprétation du index_buffer
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
    app->transformation = malloc(NB_MAT * sizeof(matrix_p));

    for (int i = 0; i < NB_MAT; ++i)
    {
        app->transformation[i] = mat_id(4);
    }

    glEnable(GL_DEPTH_TEST);

    // projection
    // float n = 1.1;
    // float f = 4.0;
    // matrix_p projj = mat_id(4);
    // mat_scalaire(projj, n);
    // projj->coefs[10] = n + f;
    // projj->coefs[11] = -n * f;
    // projj->coefs[14] = 1.0;
    // projj->coefs[15] = 0.0;
    // int u_Proj = glGetUniformLocation(app->program, "u_Proj");
    // glUniformMatrix4fv(u_Proj, 1, GL_FALSE, mat_get(projj));
    // mat_affiche(projj);

    // /* pour obtenir la matrice de translation 3d
    // 0 0 0 kx
    // 0 0 0 ky
    // 0 0 0 kz
    // 0 0 0 0 */
    app->transformation[1] = translation_3(0.00, 0.00, 0.00);
    mat_scalaire(app->transformation[0], -1);
    mat_ajoute(app->transformation[1], *app->transformation[0]);
    mat_scalaire(app->transformation[0], -1);

    // Le mainloop est ici
    emscripten_request_animation_frame_loop(mainloop, (void *)app);

    debug();

    return 0;
}