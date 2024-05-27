#ifndef MODEL_3D_H
#define MODEL_3D_H

#include "vertex.h"
#include "materiau.h"
#include "../tools/constantes.h"

// Un modèle 3D c'est la donnée d'un vertex buffer et d'un index_buffer
// Mais puisqu'ils ont déja tous été chargés en mémoire au moment de l'initialisation, il suffit de stocker une valeur d'enum
// Donc : model_3D n'est en fait quasiment qu'un alias pour MODEL_TYPE_t
// Il faut changer ça. Un model_3D devrait aussi stocker un program_index et (peut-être ?) une nouvelle matrice de transformation
typedef struct model_3D
{
    // Avec cet attribut (converti en unsigned int) on peut aller indexer VERTEX_BUFFER_ID, NB_VERTEX_PER_BUFFER, INDEX_BUFFER_ID, NB_INDEX_PER_BUFFER
    MODEL_TYPE_t model_id;

    // Si ce modèle n'est pas un cube avec des textures (cubemap), cette constante vaut NO_TEXTURE
    unsigned int cubemap_id;

    // Si ce modèle ne doit pas stocker de texture unique, cette constante vaut NO_TEXTURE
    unsigned int texture_id;
} model_3D_t;

// // Vertex buffer et Index buffer doivent etre alloués aupréalablement sur le tas
// model_3D_t Model_3D(unsigned int __nb_vertex, vertex_t *__vertex_buffer, unsigned int __nb_index, unsigned int *__index_buffer, const char* cube_images[6]);

// // Génère et renvoie un pointeur vers le modèle 3d d'une sphère
// model_3D_t Sphere(double rayon, double __r, double __g, double __b);

// // Génère et renvoie un pointeur vers le modèle 3d d'un parallelipipede rectangle de taille width x height x depth
// model_3D_t Pave(double width, double height, double depth, double r, double g, double b, const char* cube_images[6]);

// // Génère et renvoie un pointeur vers le modèle 3d d'un cube de taille c x c x c
// model_3D_t Cube(double c, double r, double g, double b, const char* cube_images[6]);

// Lance les glDrawElements/glDrawArrays, glbindbuffer et tout ce qu'il faut
// Techniquement dessine au centre 0, 0, 0, donc il FAUT faire une translation depuis O vers x, y, z)
void model_3D_draw(model_3D_t this, materiau_t materiau, unsigned int program_id);

// _______________________________________Init OpenGL buffers___________________________________________________________________

// Les prochaines fonctions ne servent qu'à l'initialisation du vertex buffer / index buffer de certains modèles 3D pour OpenGL au lancement du programme

// Renvoie un pointeur vers la zone du tas où est alloué le vertex buffer d'un Pavé droit de dimensions width * height * depth aux couleurs r, g, b
const void* init_vertex_buffer_pave_data(unsigned int *nb_vertex, double width, double height, double depth, double r, double g, double b);

// Renvoie un pointeur vers la zone du tas où est alloué le vertex buffer d'un rectangle de dimensions width * height * depth aux couleurs r, g, b (il est dans Oxz)
const void* init_vertex_buffer_rect_data(unsigned int *nb_vertex, double width, double height, double r, double g, double b);

// Renvoie un pointeur vers la zone du tas où est alloué le vertex buffer d'une sphère de rayon r aux couleurs r, g, b
const void* init_vertex_buffer_sphere_data(unsigned int *nb_vertex, double rayon, double r, double g, double b);

// Renvoie un pointeur vers la zone du tas où est alloué le index buffer d'une sphère de rayon r aux couleurs r, g, b
unsigned int* init_index_buffer_sphere(unsigned int *nb_index);

#endif