#ifndef CONSTANTES_H
#define CONSTANTES_H

#define FPS 60

extern double dt;

// __________________________________Rendu 3D_____________________________________________

// Index des identifiants opengl des "programmes" (= 2 shaders) stockés dans PROGRAM_ID
#define NB_PROGRAMS 5
#define NO_PROGRAM -1          // Certaines entités n'ont pas de programme associé
#define COLOR_PROGRAM 0        // Rendu d'objets de couleur unie
#define TEXTURE_PROGRAM 1      // Rendu avec une texture
#define SKYBOX_PROGRAM 2       // Rendu du ciel
#define CHECKERBOARD_PROGRAM 3 // Rendu du sol en damier
#define SHADOW_PROGRAM 4

// Les constantes ci-dessus sont des indices de PROGRAM_ID
extern unsigned int PROGRAM_ID[NB_PROGRAMS];

// Convention pour les objets sans texture
#define NO_TEXTURE -1

// 9 attributs / vertex : x, y, z, r, g, b, normale_x, normale_y, normale_z
#define NB_ATTRIBUTES_VERTEX 9

#define NB_BUFFERS 5

// Index des identifiants opengl des vertex/index buffer
// Ils sont générés au lancement puis à chaque draw call on choisit quels buffers on veut bind
typedef enum MODEL_TYPE
{
    CUBE_TEST_BUF,    // Un certain cube, pour tester..
    SPHERE_BIG_BUF,   // Boule de pétanque
    SPHERE_SMALL_BUF, // Cochonnet
    GROUND_BUF,       // Sol
    SHADOW_BUF        // Ombre de boule
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
#define SUN_X -8.0
#define SUN_Y 20.0
#define SUN_Z -30.0

// _______________________________________Physique________________________________________

#define PI 3.141592653589793
#define GRAVITY 0.03
#define FLUID_MU 0.01
#define SOLID_STATIC_MU 0.08
#define SOLID_DYNAMIC_MU 0.13
#define COLLISION_ENERGY_LOSS 0.0

#endif