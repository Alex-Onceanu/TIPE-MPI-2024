#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <GLES3/gl3.h>
#include <emscripten/html5.h>

#include "../tools/reader.h"
#include "../tools/constantes.h"
#include "model_3D.h"

unsigned int compile_shader(unsigned int type, const char *source)
{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    return id;
}

int create_program(const char *vertex_shader, const char *fragment_shader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    // Penser au moment de free à gl Detach shader ?

    return program;
}

char *read_shader(const char *filename)
{
    FILE *shader_file = fopen(filename, "r");
    if (shader_file == NULL)
    {
        printf("Cannot load file %s\n", filename);
    }

    int maxbuf = 200;
    char *res = malloc((1 + maxbuf) * sizeof(char));
    int i = 0;

    while (!feof(shader_file))
    {
        if (i >= maxbuf)
        {
            maxbuf *= 2;
            res = realloc(res, (1 + maxbuf) * sizeof(char));
        }
        res[i++] = fgetc(shader_file);
    }
    res[i - 1] = '\0';

    fclose(shader_file);

    return res;
}

void init_buffers()
{
    void* vertex_buffer;
    void* index_buffer;

    // __________________________________________SOL________________________________________________________

    // Vertex buffer, on envoie à OpenGL les données des triangles
    vertex_buffer = init_vertex_buffer_pave_data(NB_VERTEX_PER_BUFFER + GROUND_BUF, 1200.0, 2.0, 1200.0, 0.44, 0.401, 0.3313);
    glGenBuffers(1, VERTEX_BUFFER_ID + GROUND_BUF);
    glBindBuffer(GL_ARRAY_BUFFER, VERTEX_BUFFER_ID[GROUND_BUF]);
    glBufferData(GL_ARRAY_BUFFER, 2 * NB_VERTEX_PER_BUFFER[GROUND_BUF] * NB_ATTRIBUTES_VERTEX * sizeof(float), vertex_buffer, GL_DYNAMIC_DRAW);

    // Pas de index buffer pour les pavés
    NB_INDEX_PER_BUFFER[GROUND_BUF] = 0;
    INDEX_BUFFER_ID[GROUND_BUF] = 0;

    free(vertex_buffer);

    // _________________________________________CUBE________________________________________________________

    vertex_buffer = init_vertex_buffer_pave_data(NB_VERTEX_PER_BUFFER + CUBE_TEST_BUF, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
    glGenBuffers(1, VERTEX_BUFFER_ID + CUBE_TEST_BUF);
    glBindBuffer(GL_ARRAY_BUFFER, VERTEX_BUFFER_ID[CUBE_TEST_BUF]);
    glBufferData(GL_ARRAY_BUFFER, 2 * NB_VERTEX_PER_BUFFER[CUBE_TEST_BUF] * NB_ATTRIBUTES_VERTEX * sizeof(float), vertex_buffer, GL_DYNAMIC_DRAW);

    NB_INDEX_PER_BUFFER[CUBE_TEST_BUF] = 0;
    INDEX_BUFFER_ID[CUBE_TEST_BUF] = 0;

    free(vertex_buffer);

    // ________________________________________boule de pétanque________________________________________

    vertex_buffer = init_vertex_buffer_sphere_data(NB_VERTEX_PER_BUFFER + SPHERE_BIG_BUF, BIG_SPHERE_RADIUS, 0.5, 0.5, 0.5);
    glGenBuffers(1, VERTEX_BUFFER_ID + SPHERE_BIG_BUF);
    glBindBuffer(GL_ARRAY_BUFFER, VERTEX_BUFFER_ID[SPHERE_BIG_BUF]);
    glBufferData(GL_ARRAY_BUFFER, 2 * NB_VERTEX_PER_BUFFER[SPHERE_BIG_BUF] * NB_ATTRIBUTES_VERTEX * sizeof(float), vertex_buffer, GL_DYNAMIC_DRAW);

    // Bind et interprétation du index_buffer
    index_buffer = init_index_buffer_sphere(NB_INDEX_PER_BUFFER + SPHERE_BIG_BUF);
    glGenBuffers(1, INDEX_BUFFER_ID + SPHERE_BIG_BUF);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, INDEX_BUFFER_ID[SPHERE_BIG_BUF]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NB_INDEX_PER_BUFFER[SPHERE_BIG_BUF] * sizeof(unsigned int), index_buffer, GL_DYNAMIC_DRAW);

    free(vertex_buffer);

    // ________________________________________Cochonnet_____________________________________________

    vertex_buffer = init_vertex_buffer_sphere_data(NB_VERTEX_PER_BUFFER + SPHERE_SMALL_BUF, SMALL_SPHERE_RADIUS, 1.0, 0.0, 0.0);
    glGenBuffers(1, VERTEX_BUFFER_ID + SPHERE_SMALL_BUF);
    glBindBuffer(GL_ARRAY_BUFFER, VERTEX_BUFFER_ID[SPHERE_SMALL_BUF]);
    glBufferData(GL_ARRAY_BUFFER, 2 * NB_VERTEX_PER_BUFFER[SPHERE_SMALL_BUF] * NB_ATTRIBUTES_VERTEX * sizeof(float), vertex_buffer, GL_DYNAMIC_DRAW);

    index_buffer = init_index_buffer_sphere(NB_INDEX_PER_BUFFER + SPHERE_SMALL_BUF);
    glGenBuffers(1, INDEX_BUFFER_ID + SPHERE_SMALL_BUF);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, INDEX_BUFFER_ID[SPHERE_SMALL_BUF]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NB_INDEX_PER_BUFFER[SPHERE_SMALL_BUF] * sizeof(unsigned int), index_buffer, GL_DYNAMIC_DRAW);

    free(vertex_buffer);

    // ________________________________________Ombre_____________________________________________________

    vertex_buffer = init_vertex_buffer_rect_data(NB_VERTEX_PER_BUFFER + SHADOW_BUF, 2.0 * BIG_SPHERE_RADIUS, 2.0 * BIG_SPHERE_RADIUS, 0.01, 0.01, 0.01);
    glGenBuffers(1, VERTEX_BUFFER_ID + SHADOW_BUF);
    glBindBuffer(GL_ARRAY_BUFFER, VERTEX_BUFFER_ID[SHADOW_BUF]);
    glBufferData(GL_ARRAY_BUFFER, 2 * NB_VERTEX_PER_BUFFER[SHADOW_BUF] * NB_ATTRIBUTES_VERTEX * sizeof(float), vertex_buffer, GL_DYNAMIC_DRAW);

    NB_INDEX_PER_BUFFER[SHADOW_BUF] = 0;
    INDEX_BUFFER_ID[SHADOW_BUF] = 0;

    free(vertex_buffer);

    //  _____________________________________Axe de rotation _____________________________________________

    vertex_buffer = init_vertex_buffer_pave_data(NB_VERTEX_PER_BUFFER + AXIS_BUF, 4.0, 0.2, 0.2, 0.3, 1.0, 0.3);
    glGenBuffers(1, VERTEX_BUFFER_ID + AXIS_BUF);
    glBindBuffer(GL_ARRAY_BUFFER, VERTEX_BUFFER_ID[AXIS_BUF]);
    glBufferData(GL_ARRAY_BUFFER, 2 * NB_VERTEX_PER_BUFFER[AXIS_BUF] * NB_ATTRIBUTES_VERTEX * sizeof(float), vertex_buffer, GL_DYNAMIC_DRAW);

    NB_INDEX_PER_BUFFER[AXIS_BUF] = 0;
    INDEX_BUFFER_ID[AXIS_BUF] = 0;

    free(vertex_buffer);

    // _____________________________________Crosshair______________________________________________________

    vertex_buffer = init_vertex_buffer_pave_data(NB_VERTEX_PER_BUFFER + CROSSHAIR_BUF, 0.4, 0.05, 0.05, 0.1, 0.5, 1.0);
    glGenBuffers(1, VERTEX_BUFFER_ID + CROSSHAIR_BUF);
    glBindBuffer(GL_ARRAY_BUFFER, VERTEX_BUFFER_ID[CROSSHAIR_BUF]);
    glBufferData(GL_ARRAY_BUFFER, 2 * NB_VERTEX_PER_BUFFER[CROSSHAIR_BUF] * NB_ATTRIBUTES_VERTEX * sizeof(float), vertex_buffer, GL_DYNAMIC_DRAW);

    NB_INDEX_PER_BUFFER[CROSSHAIR_BUF] = 0;
    INDEX_BUFFER_ID[CROSSHAIR_BUF] = 0;

    free(vertex_buffer);
}

unsigned int init_texture(const char* path)
{
    int width, height;
    unsigned char *data = read_ppm(path, &width, &height);

    // Chaque texture chargée est associée à un uint, qu'on stocke dans un
    // unsigned int[], ici comme il y en a 1 seul on envoie &texture
    unsigned int texture_id;
    // Génération du texture object
    glGenTextures(1, &texture_id);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    // On attache une image 2d à un texture object
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture_id;
}

// paths = 6 chemins de fichiers (.ppm par pitié)
unsigned int init_cubemap(const char *paths[6], bool skybox)
{
    // identifiant du cubemap, qu'on renverra à la fin
    unsigned int texture_id;
    // Génère un identifiant de texture (ici sera une texture de type cubemap)
    glGenTextures(1, &texture_id);
    // Associe un id de texture à GL_TEXTURE_CUBE_MAP (donc toute opération sur GL_TEXTURE_CUBE_MAP se fera sur cet id)
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    // Paramètres de rendu du cubemap
    // GL_LINEAR en MAG/MIN dit de prendre la somme des pixels environnants pour les sous-pixels (ce qui donne une image + floue au lieu de pixelisée)
    // GL_TEXTURE_WRAP_S/T/R = pour chaque fin de coordonnée (donc passage d'une face à une autre)...
    // ...on choisit le mode CLAMP_TO_EDGE donc si l'image est à peine trop petite on "étire ses bords" jusqu'à remplir ce qu'il manque
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Pour chaque face
    int width, height, nrChannels;
    for (int i = 0; i < 6; i++)
    {
        // Chargement de l'image de la i-ième face
        unsigned char *data = read_ppm(paths[i], &width, &height);

        if (!data)
        {
            printf("ERR : l'image ppm n'a pu être chargée : data = %s\n", data);
            return NO_TEXTURE;
        }
        else
        {
            // On set des informations et l'image-même de la face GL_TEXTURE_CUBE_MAP_POSITIVE_X + i
            // (c'est un sous-enum de GL_TEXTURE_CUBE_MAP représentant la i-ième face)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
    }

    if(skybox)
    {
        int wants_skybox[] = { SKYBOX_PROGRAM,COLOR_PROGRAM };
        int nb_wants_skybox = sizeof(wants_skybox) / sizeof(int);

        for(int i = 0; i < nb_wants_skybox; i++)
        {
            unsigned int program = PROGRAM_ID[wants_skybox[i]];
            glUseProgram(program); 

            // On associe le cubemap qu'on a chargé et associé à texture_id au GL_TEXTURE0 de GL_TEXTURE_CUBE_MAP
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

            // On envoie le "0" de "GL_TEXTURE0" dans l'uniform u_Skybox du program actuel
            unsigned int u_Skybox = glGetUniformLocation(program, "u_Skybox");
            glUniform1i(u_Skybox, 0);
        }
    }
    else
    {
        unsigned int program = PROGRAM_ID[COLOR_PROGRAM];
        glUseProgram(program);
        
        // On associe le cubemap qu'on a chargé et associé à texture_id au GL_TEXTURE1 de GL_TEXTURE_CUBE_MAP
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

        // On envoie le "1" de "GL_TEXTURE1" dans l'uniform u_NormalMap de color.frag
        unsigned int u_NormalMap = glGetUniformLocation(program, "u_NormalMap");
        glUniform1i(u_NormalMap, 1);
    }

    return texture_id;
}

void init()
{
    srand(time(NULL));
    SUN_POS = Force3(SUN_X_0, SUN_Y_0, SUN_Z_0);

    // initialisation de EmscriptenWebGLContextAttributes
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;

    // Créer un contexte emscripten et le target
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(ctx);

    char *shader_paths[NB_PROGRAMS][2] = {{"../res/shaders/color.vert",     "../res/shaders/color.frag"},
                                          {"../res/shaders/skybox.vert",    "../res/shaders/skybox.frag"},
                                          {"../res/shaders/ground.vert",    "../res/shaders/ground.frag"},
                                          {"../res/shaders/shadow.vert",    "../res/shaders/shadow.frag"},
                                          {"../res/shaders/axis.vert",      "../res/shaders/axis.frag"},
                                          {"../res/shaders/axis.vert",      "../res/shaders/crosshair.frag"}};

    for (int program_it = 0; program_it < NB_PROGRAMS; program_it++)
    {
        // Vertex Shader, pour la position de chaque vertex
        char *vs_source = read_shader(shader_paths[program_it][0]);

        // Fragment Shader, pour chaque pixel (gère la couleur/texture, la lumière...)
        char *fs_source = read_shader(shader_paths[program_it][1]);

        PROGRAM_ID[program_it] = create_program(vs_source, fs_source);

        free(vs_source);
        free(fs_source);
    }

    init_buffers();
}

void free_gl()
{
    for(int i = 0; i < NB_PROGRAMS; i++)
    {
        glDeleteProgram(PROGRAM_ID[i]);
    }

    glDeleteBuffers(NB_BUFFERS, VERTEX_BUFFER_ID);
    glDeleteBuffers(NB_BUFFERS, INDEX_BUFFER_ID);
}

void free_texture(unsigned int id)
{
    glDeleteTextures(1, &id);
}