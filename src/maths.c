#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "maths.h"

matrix_p mat_id(int n)
{
    matrix_p res = malloc(sizeof(matrix_t));
    res->dim = n;
    res->coefs = malloc(n * n * sizeof(float));
    for (int i = 0; i < n * n; ++i)
    {
        res->coefs[i] = (int)(i % n == i / n);
    }
    return res;
}

void mat_free(matrix_p m)
{
    free(m->coefs);
    free(m);
}

float *mat_get(matrix_p m)
{
    return m->coefs;
}

void mat_affiche(matrix_p m)
{
    for (int i = 0; i < m->dim * m->dim; ++i)
    {
        if (i % m->dim == 0 && i > 0)
            printf("\n");
        printf("%f ", m->coefs[i]);
    }
    printf("\n");
}

void mat_ajoute(matrix_p m1, const matrix_t m2)
{
    assert(m1->dim == m2.dim);

    for (int i = 0; i < m1->dim * m1->dim; ++i)
    {
        m1->coefs[i] += m2.coefs[i];
    }
}

void mat_scalaire(matrix_p m, float k)
{
    for (int i = 0; i < m->dim * m->dim; ++i)
    {
        m->coefs[i] *= k;
    }
}

// Voir algo de Strassen
matrix_p mat_produit(const matrix_t m1, const matrix_t m2)
{
    const int n = m1.dim;
    assert(n == m2.dim);

    int x, y;
    matrix_p res = mat_id(n);
    for (int i = 0; i < n * n; ++i)
    {
        // i = y * n + x
        x = i % n;
        y = i / n;
        res->coefs[i] = 0;
        for (int k = 0; k < n; ++k)
        {
            res->coefs[i] += (m1.coefs[k * n + x] * m2.coefs[y * n + k]);
        }
    }
    return res;
}

// A optimiser
void mat_transpose(matrix_p m)
{
    int y, x;
    float tmp;
    for (int i = 0; i < m->dim * m->dim; ++i)
    {
        y = i / m->dim;
        x = i % m->dim;
        if (y >= x)
            continue;

        tmp = m->coefs[y * m->dim + x];
        m->coefs[y * m->dim + x] = m->coefs[x * m->dim + y];
        m->coefs[x * m->dim + y] = tmp;
    }
}

matrix_p translation(int dim, float *k)
{
    matrix_p res = mat_id(dim + 1);

    for (int i = 0; i < dim; ++i)
    {
        res->coefs[i * (dim + 1) + dim] = k[i];
    }

    return res;
}

matrix_p translation_3(float kx, float ky, float kz)
{
    float vect[3] = {kx, ky, kz};
    return translation(3, vect);
}

matrix_p rotation_x_4(float theta)
{
    matrix_p res = mat_id(4);

    res->coefs[5] = cos(theta);
    res->coefs[6] = -sin(theta);
    res->coefs[9] = sin(theta);
    res->coefs[10] = cos(theta);

    return res;
}

matrix_p rotation_y_4(float theta)
{
    matrix_p res = mat_id(4);

    res->coefs[0] = cos(theta);
    res->coefs[2] = sin(theta);
    res->coefs[8] = -sin(theta);
    res->coefs[10] = cos(theta);

    return res;
}

matrix_p rotation_z_4(float theta)
{
    matrix_p res = mat_id(4);

    res->coefs[0] = cos(theta);
    res->coefs[1] = -sin(theta);
    res->coefs[4] = sin(theta);
    res->coefs[5] = cos(theta);

    return res;
}

// Change le theta d'une matrice de rotation_z_4 inplace
void mat_set_theta_x_4(matrix_p m, float theta)
{
    m->coefs[5] = cos(theta);
    m->coefs[6] = -sin(theta);
    m->coefs[9] = sin(theta);
    m->coefs[10] = cos(theta);
}

// Change le theta d'une matrice de rotation_z_4 inplace
void mat_set_theta_y_4(matrix_p m, float theta)
{
    m->coefs[0] = cos(theta);
    m->coefs[2] = sin(theta);
    m->coefs[8] = -sin(theta);
    m->coefs[10] = cos(theta);
}

// Change le theta d'une matrice de rotation_z_4 inplace
void mat_set_theta_z_4(matrix_p m, float theta)
{
    m->coefs[0] = cos(theta);
    m->coefs[1] = -sin(theta);
    m->coefs[4] = sin(theta);
    m->coefs[5] = cos(theta);
}