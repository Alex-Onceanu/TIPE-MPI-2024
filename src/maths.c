#include <stdlib.h>
#include <stdio.h>

#include "maths.h"

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

    mat4_affiche(res);

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
    float f = 20.0;

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