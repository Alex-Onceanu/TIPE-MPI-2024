#include <stdlib.h>
#include <stdio.h>

#include "maths.h"

int randint(int a, int b)
{
    return (rand() % (1 + b - a)) + a;
}

void Clamp(float *x, float min_x, float max_x)
{
    if (*x > max_x)
    {
        *x = max_x;
    }
    else if (*x < min_x)
    {
        *x = min_x;
    }
}

mat4_p mat4_id_p()
{
    mat4_p res = malloc(sizeof(mat4_t));

    for (int i = 0; i < 16; ++i)
    {
        res->coefs[i] = (int)(i % 4 == i / 4);
    }

    return res;
}

void mat4_free(mat4_p m)
{
    free(m);
}

float *mat4_get(mat4_p m)
{
    return m->coefs;
}

void mat4_set(mat4_p dst, const mat4_t src)
{
    for (int i = 0; i < 16; ++i)
    {
        dst->coefs[i] = src.coefs[i];
    }
}

mat4_t mat4_id_t()
{
    mat4_t res;

    for (int i = 0; i < 16; ++i)
    {
        res.coefs[i] = (int)(i % 4 == i / 4);
    }

    return res;
}

void mat4_affiche(const mat4_t m)
{
    for (int i = 0; i < 16; ++i)
    {
        if (i % 4 == 0 && i > 0)
            printf("\n");
        printf("%f ", m.coefs[i]);
    }
    printf("\n");
}

mat4_t mat4_ajoute(const mat4_t m1, const mat4_t m2)
{
    mat4_t res;

    for (int i = 0; i < 16; ++i)
    {
        res.coefs[i] = m1.coefs[i] + m2.coefs[i];
    }

    return res;
}

void mat4_ajoute_inplace(mat4_p m1, const mat4_t m2)
{
    for (int i = 0; i < 16; ++i)
    {
        m1->coefs[i] += m2.coefs[i];
    }
}

void mat4_scalaire_inplace(mat4_p m, const float k)
{
    for (int i = 0; i < 16; ++i)
    {
        m->coefs[i] *= k;
    }
}

mat4_t mat4_scalaire(const mat4_t m, const float k)
{
    mat4_t res;

    for (int i = 0; i < 16; ++i)
    {
        res.coefs[i] = k * m.coefs[i];
    }

    return res;
}

mat4_t mat4_produit(const mat4_t m1, const mat4_t m2)
{
    // TODO : algo de Strassen pour la multiplication

    mat4_t res;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            res.coefs[4 * i + j] = 0.0;
            for (int k = 0; k < 4; ++k)
            {
                res.coefs[4 * i + j] += m1.coefs[4 * i + k] * m2.coefs[k * 4 + j];
            }
        }
    }

    return res;
}

mat4_t mat4_transpose(const mat4_t m)
{
    mat4_t res = mat4_id_t();
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            res.coefs[4 * i + j] = m.coefs[4 * j + i];
        }
    }
    return res;
}

mat4_t translation(const float kx, const float ky, const float kz)
{
    mat4_t res = mat4_id_t();

    res.coefs[3] = kx;
    res.coefs[7] = ky;
    res.coefs[11] = kz;

    return res;
}

mat4_t scaling(const float kx, const float ky, const float kz)
{
    mat4_t res = mat4_id_t();

    res.coefs[0] = kx;
    res.coefs[5] = ky;
    res.coefs[10] = kz;

    return res;
}

// m doit etre de la forme
// 0 1 2
// 3 4 5
// 6 7 8
#define DET3(m) (m[0] * m[4] * m[8] + m[1] * m[5] * m[6] + m[2] * m[3] * m[7]) - (m[6] * m[4] * m[2] + m[7] * m[5] * m[0] + m[8] * m[3] * m[1])

mat4_t mat4_inverse(const mat4_t m)
{
    // Soyons fous, inversons une matrice 4x4
    // Algo : comatrice
    // CET ALGO MARCHE NE MARCHE QUE SUR DES MATRICES DE LA FORME
    // a b c x
    // d e f y
    // g h i z
    // 0 0 0 1
    // Parce qu'on va développer suivant la dernière ligne

    float c1[9] = {m.coefs[0], m.coefs[1], m.coefs[2],
                   m.coefs[4], m.coefs[5], m.coefs[6],
                   m.coefs[8], m.coefs[9], m.coefs[10]};

    // d = det m
    float d = DET3(c1);
    if (d <= 0.0001 && d >= -0.0001)
    {
        printf("/!/ Matrice non inversible\n");
        return mat4_id_t();
    }

    mat4_t com;
    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            // Construction de la matrice dont le determinant est le cofacteur en (y, x) de m
            float c2[9];
            int compt = 0;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    if (i == y || j == x)
                        continue;
                    c2[compt++] = m.coefs[4 * i + j];
                }
            }
            com.coefs[4 * y + x] = (DET3(c2));
            if ((y + x) % 2 == 1)
                com.coefs[4 * y + x] *= (-1);
        }
    }

    mat4_t res = mat4_scalaire(mat4_transpose(com), 1.0 / d);

    return res;
}

mat4_t rotation_x_4(float theta)
{
    mat4_t res = mat4_id_t();

    res.coefs[5] = cos(theta);
    res.coefs[6] = -sin(theta);
    res.coefs[9] = sin(theta);
    res.coefs[10] = cos(theta);

    return res;
}

mat4_t rotation_y_4(float theta)
{
    mat4_t res = mat4_id_t();

    res.coefs[0] = cos(theta);
    res.coefs[2] = sin(theta);
    res.coefs[8] = -sin(theta);
    res.coefs[10] = cos(theta);

    return res;
}

mat4_t rotation_z_4(float theta)
{
    mat4_t res = mat4_id_t();

    res.coefs[0] = cos(theta);
    res.coefs[1] = -sin(theta);
    res.coefs[4] = sin(theta);
    res.coefs[5] = cos(theta);

    return res;
}

void mat4_set_theta_x(mat4_p m, const float theta)
{
    m->coefs[5] = cos(theta);
    m->coefs[6] = -sin(theta);
    m->coefs[9] = sin(theta);
    m->coefs[10] = cos(theta);
}

void mat4_set_theta_y(mat4_p m, const float theta)
{
    m->coefs[0] = cos(theta);
    m->coefs[2] = sin(theta);
    m->coefs[8] = -sin(theta);
    m->coefs[10] = cos(theta);
}

void mat4_set_theta_z(mat4_p m, const float theta)
{
    m->coefs[0] = cos(theta);
    m->coefs[1] = -sin(theta);
    m->coefs[4] = sin(theta);
    m->coefs[5] = cos(theta);
}

// fun fact : cette fonction ne sert littéralement à rien mais on l'aime quand meme
mat4_t ortho(const float x_min, const float x_max, const float y_min, const float y_max, const float z_min, const float z_max)
{
    mat4_t res = mat4_id_t();

    float sx = 2.0 / (x_max - x_min);
    float sy = 2.0 / (y_max - y_min);
    float sz = 2.0 / (z_max - z_min);

    float tx = -(x_min + x_max) / 2.0;
    float ty = -(y_min + y_max) / 2.0;
    float tz = -(z_min + z_max) / 2.0;

    res.coefs[0] = sx;
    res.coefs[5] = sy;
    res.coefs[10] = sz;

    res.coefs[3] = tx * sx;
    res.coefs[7] = ty * sy;
    res.coefs[11] = tz * sz;

    return res;
}

mat4_t projection()
{
    float fov = 3.14159 / 6.0;
    float n = 1.0 / tan(fov / 2.0);
    float aspect = 1.0;
    float f = 120.0;

    mat4_t res = mat4_id_t();

    res.coefs[0] = 1.0 / (aspect * tan(fov / 2.0));
    res.coefs[5] = 1.0 / tan(fov / 2.0);
    res.coefs[10] = (n + f) / (n - f);
    res.coefs[11] = (2 * n * f) / (n - f);
    res.coefs[14] = -1.0;
    res.coefs[15] = 0.0;

    mat4_affiche(res);

    return res;
}