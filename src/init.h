#ifndef INIT_H
#define INIT_H

// Renvoie un shader en c_str à partir de son fichier source
char *read_shader(const char *filename);
// Compile un shader (donc l'envoie à OpenGL) à partir du code source en c_str
unsigned int compile_shader(unsigned int type, const char *source);
// Compile deux shaders envoyés en param et renvoie un programme OpenGL
int create_program(const char *vertex_shader, const char *fragment_shader);

// Initialise le contexte Emscripten, lit les shaders et renvoie le programme
unsigned int init();

#endif