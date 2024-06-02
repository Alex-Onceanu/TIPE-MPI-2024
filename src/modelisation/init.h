#ifndef INIT_H
#define INIT_H

#include <stdbool.h>

// Renvoie un shader en c_str à partir de son fichier source
char *read_shader(const char *filename);
// Compile un shader (donc l'envoie à OpenGL) à partir du code source en c_str
unsigned int compile_shader(unsigned int type, const char *source);
// Compile deux shaders envoyés en param et renvoie un programme OpenGL
int create_program(const char *vertex_shader, const char *fragment_shader);

// Charge une texture, renvoie son identifiant OpenGL
unsigned int init_texture(const char* path);

// Charge un cubemap, renvoie son identifiant OpenGL
unsigned int init_cubemap(const char* paths[6], bool skybox);

// Initialise le contexte Emscripten, lit les shaders et innitialise les programmes
void init();

// Libère la mémoire de tous les PROGRAM_ID, les VERTEX_BUFFER_ID et INDEX_BUFFER_ID
void free_gl();

// Libère la mémoire d'une texture ou d'un cubemap d'identifiant id généré par init_texture ou init_cubemap
void free_texture(unsigned int id);

#endif