#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <GLES2/gl2.h>
#include <emscripten/html5.h>

#include "../tools/reader.h"
#include "../tools/constantes.h"

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

    // gl Detach shader ?

    return program;
}

char *read_shader(const char *filename)
{
    FILE *shader_file = fopen(filename, "r");

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

// paths = 6 chemins de fichiers (.ppm par pitié)
unsigned int init_cubemap(const char* paths[6])
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
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Pour chaque face
    int width, height, nrChannels;
    for(int i = 0; i < 6; i++)
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
            printf("l'image a pu être chargée : yooopi !\n");
            // On set des informations et l'image-même de la face GL_TEXTURE_CUBE_MAP_POSITIVE_X + i 
            // (c'est un sous-enum de GL_TEXTURE_CUBE_MAP représentant la i-ième face)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        }
    }
    
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture_id;
}

void init()
{
    srand(time(NULL));

    // initialisation de EmscriptenWebGLContextAttributes
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;

    // Créer un contexte emscripten et le target
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(ctx);

    char* shader_paths[NB_PROGRAMS][2] = { {"../res/shaders/color.vert","../res/shaders/color.frag" },{ "../res/shaders/texture.vert","../res/shaders/texture.frag" },{ "../res/shaders/skybox.vert","../res/shaders/skybox.frag" } };

    for(int program_it = 0; program_it < NB_PROGRAMS; program_it++)
    {
        // Vertex Shader, pour la position de chaque vertex
        char *vs_source = read_shader(shader_paths[program_it][0]);

        // Fragment Shader, pour chaque pixel (gère la couleur/texture, la lumière...)
        char *fs_source = read_shader(shader_paths[program_it][1]);
        
        PROGRAM_ID[program_it] = create_program(vs_source, fs_source);

        free(vs_source);
        free(fs_source);
    }
}
