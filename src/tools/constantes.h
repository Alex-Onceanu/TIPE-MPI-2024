#ifndef CONSTANTES_H
#define CONSTANTES_H

#define FPS 60

// Index des identifiants opengl des "programmes" (= 2 shaders) stockés dans PROGRAM_ID
#define NB_PROGRAMS 3
#define NO_PROGRAM -1
#define COLOR_PROGRAM 0
#define TEXTURE_PROGRAM 1
#define SKYBOX_PROGRAM 2
extern unsigned int PROGRAM_ID[NB_PROGRAMS];

// Convention pour les objets sans texture
#define NO_TEXTURE -1

// 9 attributs / vertex : x, y, z, r, g, b, normale_x, normale_y, normale_z
#define NB_ATTRIBUTES_VERTEX 9
// Position (de monde) de la source de lumière
#define SUN_X -8.0
#define SUN_Y 10.0
#define SUN_Z -30.0

// _________________________________Physique___________________________________

#define PI 3.141592653589793
#define GRAVITY 0.03
#define FLUID_MU 0.01
#define SOLID_STATIC_MU 0.08
#define SOLID_DYNAMIC_MU 0.05

#endif