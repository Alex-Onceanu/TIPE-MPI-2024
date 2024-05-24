#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <stdbool.h>

#define FPS 60

extern double dt;

// __________________________________Rendu 3D_____________________________________________

// Index des identifiants opengl des "programmes" (= 2 shaders) stockés dans PROGRAM_ID
#define NB_PROGRAMS 5
#define NO_PROGRAM -1    // Certaines entités n'ont pas de programme associé
#define COLOR_PROGRAM 0  // Rendu d'objets de couleur unie
#define SKYBOX_PROGRAM 1 // Rendu du ciel
#define GROUND_PROGRAM 2 // Rendu du sol (sablonneux)
#define SHADOW_PROGRAM 3 // Rendu des ombres
#define AXIS_PROGRAM 4   // Rendu de l'axe de rotation des boules

// Les constantes ci-dessus sont des indices de PROGRAM_ID
extern unsigned int PROGRAM_ID[NB_PROGRAMS];

// Convention pour les objets sans texture
#define NO_TEXTURE -1

// 9 attributs / vertex : x, y, z, r, g, b, normale_x, normale_y, normale_z
#define NB_ATTRIBUTES_VERTEX 9

#define NB_BUFFERS 6

// Index des identifiants opengl des vertex/index buffer
// Ils sont générés au lancement puis à chaque draw call on choisit quels buffers on veut bind
typedef enum MODEL_TYPE
{
    CUBE_TEST_BUF,    // Un certain cube, pour tester..
    SPHERE_BIG_BUF,   // Boule de pétanque
    SPHERE_SMALL_BUF, // Cochonnet
    GROUND_BUF,       // Sol
    SHADOW_BUF,       // Ombre de boule
    AXIS_BUF          // Axe de rotation des boules
} MODEL_TYPE_t;

// L'avantage, quand on code en C, c'est qu'un enum peut être trivialement converti en unsigned int
// Les constantes ci-dessus sont en fait des indices de VERTEX_BUFFER_ID et INDEX_BUFFER_ID
extern unsigned int NB_VERTEX_PER_BUFFER[NB_BUFFERS];
extern unsigned int VERTEX_BUFFER_ID[NB_BUFFERS];
extern unsigned int NB_INDEX_PER_BUFFER[NB_BUFFERS];
extern unsigned int INDEX_BUFFER_ID[NB_BUFFERS];

// __________________________________________Monde_______________________________________

// Rayon d'une boule de pétanque / du cochonnet
#define BIG_SPHERE_RADIUS 1.0
#define SMALL_SPHERE_RADIUS 0.4

// Position (de monde) de la source de lumière
extern float SUN_X;
extern float SUN_Y;
extern float SUN_Z;

#define SUN_X_0 -8.0
#define SUN_Y_0 20.0
#define SUN_Z_0 -30.0

extern float LIGHT_COLOR[3];
extern float AMBIENT_INTENSITY;

extern float BALL_REFLECTIVITY;
#define BALL_REFLECTIVITY_0 0.6

extern bool SHOULD_RENDER_AXIS;

// _______________________________________Physique________________________________________

#define PI 3.141592653589793

extern float GRAVITY;
extern float FLUID_MU;
extern float SOLID_MU;
extern float ROTATION_MU;
extern float THROW_SPEED;
extern float THROW_ANGLE;
extern float BALL_MASS;

#define GRAVITY_0 0.03
#define FLUID_MU_0 0.006
#define SOLID_MU_0 0.9833
#define ROTATION_MU_0 0.02
#define THROW_SPEED_0 1.5
#define THROW_ANGLE_0 0.0
#define BALL_MASS_0 0.8 // masse réelle d'une boule de petanque : 0,730 kg

#endif