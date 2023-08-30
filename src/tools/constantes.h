#ifndef CONSTANTES_H
#define CONSTANTES_H

// Index des identifiants opengl des "programmes" (= 2 shaders) stockés dans PROGRAM_ID
#define NB_PROGRAMS 3
#define NO_PROGRAM -1
#define COLOR_PROGRAM 0
#define TEXTURE_PROGRAM 1
#define SKYBOX_PROGRAM 2
extern unsigned int PROGRAM_ID[NB_PROGRAMS];

#define NO_TEXTURE -1

#define PI 3.141592653589793

#define NB_ATTRIBUTES_VERTEX 9
#define SUN_X -8.0
#define SUN_Y 10.0
#define SUN_Z -30.0

#endif