#ifndef MODEL_3D_H
#define MODEL_3D_H

#include "vertex.h"
#include "materiau.h"
#include "../tools/constantes.h"

// Un modèle 3D c'est la donnée d'un vertex buffer et d'un index_buffer
// Mais puisqu'ils ont déja tous été chargés en mémoire au moment de l'initialisation, il suffit de stocker une valeur d'enum
// Il faut aussi stocker l'identifiant d'un cubemap et/ou d'une texture pour certains objets
typedef struct model_3D
{
    // Avec cet attribut (converti en unsigned int) on peut aller indexer VERTEX_BUFFER_ID, NB_VERTEX_PER_BUFFER, INDEX_BUFFER_ID, NB_INDEX_PER_BUFFER
    MODEL_TYPE_t model_id;

    // Si ce modèle n'est pas un cube avec des textures (cubemap), cette constante vaut NO_TEXTURE
    unsigned int cubemap_id;

    // Si ce modèle ne doit pas stocker de texture unique, cette constante vaut NO_TEXTURE
    unsigned int texture_id;
} model_3D_t;

// Constructeur de model_3D_t (! cubemap et texture sont des identifiants OPENGL (éventuellement NO_TEXTURE) !)
model_3D_t Model3D(MODEL_TYPE_t type, unsigned int cubemap, unsigned int texture);

// Destructeur
void model_3D_free(model_3D_t this);

// Lance les glDrawElements/glDrawArrays, glbindbuffer et tout ce qu'il faut, démarre le rendu de cet objet
// Techniquement dessine au centre 0, 0, 0, donc il FAUT faire une translation depuis O vers x, y, z)
void model_3D_draw(model_3D_t this, materiau_t materiau, unsigned int program_id);

// _______________________________________Init OpenGL buffers___________________________________________________________________

// Les prochaines fonctions ne servent qu'à l'initialisation du vertex buffer / index buffer de certains modèles 3D pour OpenGL au lancement du programme

// Renvoie un pointeur vers la zone du tas où est alloué le vertex buffer d'un Pavé droit de dimensions width * height * depth aux couleurs r, g, b
void* init_vertex_buffer_pave_data(unsigned int *nb_vertex, double width, double height, double depth, double r, double g, double b);

// Renvoie un pointeur vers la zone du tas où est alloué le vertex buffer d'un rectangle de dimensions width * height * depth aux couleurs r, g, b (il est dans Oxz)
void* init_vertex_buffer_rect_data(unsigned int *nb_vertex, double width, double height, double r, double g, double b);

// Renvoie un pointeur vers la zone du tas où est alloué le vertex buffer d'une sphère de rayon r aux couleurs r, g, b
void* init_vertex_buffer_sphere_data(unsigned int *nb_vertex, double rayon, double r, double g, double b);

// Renvoie un pointeur vers la zone du tas où est alloué le index buffer d'une sphère de rayon r aux couleurs r, g, b
unsigned int* init_index_buffer_sphere(unsigned int *nb_index);

#endif