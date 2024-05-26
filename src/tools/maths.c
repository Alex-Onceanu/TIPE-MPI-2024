#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "maths.h"
#include "constantes.h"

float my_sqrtf(float x)
{
    // TODO
    return sqrtf(x);
}

int randint(int a, int b)
{
    return (rand() % (1 + b - a)) + a;
}

float f_max(float x, float y)
{
    return x > y ? x : y;
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

force3_t Force3(float __fx, float __fy, float __fz)
{
    force3_t res;

    res.fx = __fx;
    res.fy = __fy;
    res.fz = __fz;

    return res;
}

void force3_print(force3_t f)
{
    printf("%f | %f | %f", f.fx, f.fy, f.fz);
}

force3_t force3_add(force3_t f1, force3_t f2)
{
    return LINEAR_COMBINATION(f1, f2, 1.0);
}

force3_t force3_sub(force3_t f1, force3_t f2)
{
    return LINEAR_COMBINATION(f1, f2, -1.0);
}

force3_t force3_scale(force3_t f, float lambda)
{
    return LINEAR_COMBINATION(Force3(0.0, 0.0, 0.0), f, lambda);
}

force3_t force3_cross_product(force3_t f1, force3_t f2)
{
    return Force3(f1.fy * f2.fz - f1.fz * f2.fy, f1.fz * f2.fx - f1.fx * f2.fz, f1.fx * f2.fy - f1.fy * f2.fx);
}

float norme2(force3_t f)
{
    return my_sqrtf(SQ_NORME2(f));
}

void normalize(force3_p f)
{
    float n = norme2(*f);

    f->fx /= n;
    f->fy /= n;
    f->fz /= n;
}

float produit_scalaire(force3_t u, force3_t v)
{
    return u.fx * v.fx + u.fy * v.fy + u.fz * v.fz;
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

mat4_t mat4_rotation(force3_t axe, mat4_t* passage)
{
    float angle = norme2(axe);

    if(angle <= 0.000001)
    {
        if(passage != NULL) *passage = mat4_id_t();
        return mat4_id_t();
    }

    force3_t theta = axe;
    normalize(&theta);

    mat4_t P = mat4_id_t();

    // Matrice de passage vers une nouvelle base dans laquelle theta est le nouvel axe Ox
    float d = sqrtf(theta.fx * theta.fx + theta.fy * theta.fy);

    if(d <= 0.000001)
    {
        // Si on entre dans ce if la rotation est nécessairement autour de Oz
        // La matrice de passage est alors celle qui place vec{e_x} sur + ou - vec{e_z}
        if(passage != NULL) *passage = rotation_y_4(- (theta.fz / fabs(theta.fz)) * PI / 2.0);

        return rotation_z_4(angle);
    }

    P.coefs[0] = theta.fx;
    P.coefs[1] = theta.fy;
    P.coefs[2] = theta.fz;
    P.coefs[4] = -theta.fy / d;
    P.coefs[5] = theta.fx / d;
    P.coefs[6] = 0.0;
    P.coefs[8] = -theta.fz * P.coefs[5];
    P.coefs[9] = theta.fz * P.coefs[4];
    P.coefs[10] = theta.fx * P.coefs[5] - theta.fy * P.coefs[4];

    mat4_t P_T = mat4_transpose(P);

    if(passage != NULL) *passage = P_T;

    return mat4_produit(P_T, mat4_produit(rotation_x_4(angle), P));
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
    float f = 400.0;

    mat4_t res = mat4_id_t();

    res.coefs[0] = 1.0 / (aspect * tan(fov / 2.0));
    res.coefs[5] = 1.0 / tan(fov / 2.0);
    res.coefs[10] = (n + f) / (n - f);
    res.coefs[11] = (2 * n * f) / (n - f);
    res.coefs[14] = -1.0;
    res.coefs[15] = 0.0;

    // mat4_affiche(res);

    return res;
}