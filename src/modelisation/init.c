#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <GLES2/gl2.h>
#include <emscripten/html5.h>

#include "../tools/reader.h"

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

void init_texture(unsigned int program)
{
    // Pas la peine, GL_REPEAT est bien et est par défaut
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    int width, height, nrChannels;
    unsigned char *data = read_ppm("./res/textures/dice.ppm", &width, &height);

    if (!data)
    {
        printf("ERR : l'image png n'a pu être chargée : data = %s\n", data);
        // printf("%s\n", stbi_failure_reason());
    }
    else
    {
        printf("l'image a pu être chargée : yooopi !\n");
    }

    // Chaque texture chargée est associée à un uint, qu'on stocke dans un
    // unsigned int[], ici comme il y en a 1 seul on envoie &texture
    unsigned int texture;
    // Génération du texture object
    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0); // Activer texture unit
    glBindTexture(GL_TEXTURE_2D, texture);

    // On attache une image 2d à un texture object
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // Voir définition d'un mipmap
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glUniform1i(glGetUniformLocation(program, "u_Texture"), 0);

    free(data);
}

unsigned int init()
{
    srand(time(NULL));

    // initialisation de EmscriptenWebGLContextAttributes
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;

    // Créer un contexte emscripten et le target
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(ctx);

    // Vertex Shader, pour la position de chaque vertex
    char *vs_source = read_shader("../res/shaders/main.vert");

    // Fragment Shader, pour chaque pixel (gère la couleur en outre)
    char *fs_source = read_shader("../res/shaders/main.frag");

    unsigned int shader = create_program(vs_source, fs_source);
    glUseProgram(shader);

    free(vs_source);
    free(fs_source);

    return shader;
}
