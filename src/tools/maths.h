#ifndef MATHS_H
#define MATHS_H

#include <math.h>

// Renvoie un nombre aleatoire de [| a, b |]
int randint(int a, int b);

// Fait en sorte que x reste dans [| min_x, max_x |]
void Clamp(float *x, float min_x, float max_x);

// _______________________________Matrice________________________________

// Matrice 4x4
typedef struct
{
    float coefs[16];
} mat4_t, *mat4_p;

// Constructeur, renvoie la matrice identité 4x4 allouée dans le tas
mat4_p mat4_id_p();

// Constructeur, renvoie la matrice identité 4x4 sur la pile
mat4_t mat4_id_t();

// Juste free()
void mat4_free(mat4_p m);

// Renvoie le contenu de m en tant que tableau de floats (= m->coefs)
float *mat4_get(mat4_p m);

// dst <- src, copie tous les coefficiens de src dans dst
void mat4_set(mat4_p dst, const mat4_t src);

// Ecrit tous les coefficients de m dans stdout sous la forme
/*  1 0 0 0
    0 1 0 0
    0 0 1 0
    0 0 0 1 */
void mat4_affiche(const mat4_t m);

// Renvoie une nouvelle matrice allouée sur la pile valant m1 + m2
mat4_t mat4_ajoute(const mat4_t m1, const mat4_t m2);

// m1 <- m1 + m2
void mat4_ajoute_inplace(mat4_p m1, const mat4_t m2);

// m <- k * m
void mat4_scalaire_inplace(mat4_p m, const float k);

// Renvoie k * m sur la pile
mat4_t mat4_scalaire(const mat4_t m, const float k);

// renvoie m1 * m2 sur la pile
mat4_t mat4_produit(const mat4_t m1, const mat4_t m2);

// transposition
mat4_t mat4_transpose(const mat4_t m);

// Inversion
mat4_t mat4_inverse(const mat4_t m);

// ______________________________Applications___________________________________

// Matrice de translation de vecteur (kx, ky, kz) en dimension 3
mat4_t translation(const float kx, const float ky, const float kz);

// Matrice d'homothéthie de rapport k en dimension 3
mat4_t scaling(const float kx, const float ky, const float kz);

// Matrice de rotation autour de l'axe x d'angle theta en rd, en dimension 3
// Renvoie une matrice de taille 4 * 4
mat4_t rotation_x_4(float theta);

// Matrice de rotation autour de l'axe y d'angle theta en rd, en dimension 3
// Renvoie une matrice de taille 4 * 4
mat4_t rotation_y_4(float theta);

// Matrice de rotation autour de l'axe z d'angle theta en rd, en dimension 3
// Renvoie une matrice de taille 4 * 4
mat4_t rotation_z_4(float theta);

// Change inplace la valeur theta d'une matrice de rotation autour de l'axe x
void mat4_set_theta_x(mat4_p m, const float theta);

// Change inplace la valeur theta d'une matrice de rotation autour de l'axe y
void mat4_set_theta_y(mat4_p m, const float theta);

// Change inplace la valeur theta d'une matrice de rotation autour de l'axe z
void mat4_set_theta_z(mat4_p m, const float theta);

// Ramène les coordonnées du monde entre -1 et 1 selon les 3 axes
// Ortho inutile..? Je la garde pour l'instant, sera peut-être supprimée plus tard
mat4_t ortho(const float x_min, const float x_max, const float y_min, const float y_max, const float z_min, const float z_max);

// Renvoie la matrice de projection. Chaque vertex, une fois multiplié par cette matrice,
// passe de coordonnées du monde à des NDC avec respect à la perspective.
mat4_t projection();

#endif