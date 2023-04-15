#ifndef MATHS_H
#define MATHS_H

#include <math.h>

//_______________________________________Matrice_______________________________

// Matrice carrée de floats uniquement, car c'est tout ce dont il y a besoin.
typedef struct
{
    int dim;
    float *coefs;
} matrix_t, *matrix_p;

// Renvoie l'identité de dimension n * n
matrix_p mat_id(int n);

// Destructeur
void mat_free(matrix_p m);

// Renvoie le contenu de m en tant que tableau de floats (= m->coefs)
float *mat_get(matrix_p m);

// Ecrit tous les coefficients de m dans stdout sous la forme
/*  1 0 0
    0 1 0
    0 0 1 */
void mat_affiche(matrix_p m);

// M1 <- M1 + M2
void mat_ajoute(matrix_p m1, const matrix_t m2);

// Produit de M par un scalaire k (inplace) M <- k.M
void mat_scalaire(matrix_p m, float k);

// Produit matriciel, renvoie M1 M2
matrix_p mat_produit(const matrix_t m1, const matrix_t m2);

// Inverse les lignes et les colonnes inplace
void mat_transpose(matrix_p m);

//________________________________________Applications_________________________

// Matrice de taille (dim + 1) * (dim + 1) représentant la translation de vecteur k
// k doit contenir [kx, ky, kz, kw, ...] avec len(k) = dim
matrix_p translation(int dim, float *k);

/* Cas particulier de translation en dimension 3, plus commun : renvoie
1 0 0 kx
0 1 0 ky
0 0 1 kz
0 0 0 1
*/
matrix_p translation_3(float kx, float ky, float kz);

// Matrice de rotation autour de l'axe x d'angle theta en rd, en dimension 3
// Renvoie une matrice de taille 4 * 4
matrix_p rotation_x_4(float theta);

// Matrice de rotation autour de l'axe y d'angle theta en rd, en dimension 3
// Renvoie une matrice de taille 4 * 4
matrix_p rotation_y_4(float theta);

// Matrice de rotation autour de l'axe z d'angle theta en rd, en dimension 3
// Renvoie une matrice de taille 4 * 4
matrix_p rotation_z_4(float theta);

// Change le theta d'une matrice de rotation_z_4 inplace
void mat_set_theta_x_4(matrix_p m, float theta);

// Change le theta d'une matrice de rotation_z_4 inplace
void mat_set_theta_y_4(matrix_p m, float theta);

// Change le theta d'une matrice de rotation_z_4 inplace
void mat_set_theta_z_4(matrix_p m, float theta);

#endif