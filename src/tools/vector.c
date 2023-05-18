#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "vector.h"

////__________________________________Vecteur 1D_______________________________

struct vector
{
    // On aura toujours nb_elems <= nb_allocated
    int nb_elems;
    int nb_allocated;
    T *tab;
};

vector_p vector_init()
{
    vector_p resultat = malloc(sizeof(vector_t));

    resultat->nb_elems = 0;
    resultat->nb_allocated = 1;
    resultat->tab = malloc(sizeof(T) * resultat->nb_allocated);

    return resultat;
}

void vector_free(vector_p v)
{
    if (v == NULL)
        return;
    free(v->tab);
    free(v);
}

void vector_append(vector_p v, const T elem)
{
    assert(v != NULL);
    if (v->nb_allocated == v->nb_elems)
    {
        v->nb_allocated *= 2;
        v->tab = realloc(v->tab, sizeof(T) * v->nb_allocated);
    }
    v->tab[v->nb_elems++] = elem;
}

T vector_pop(vector_p v)
{
    assert(v != NULL);
    return v->tab[--v->nb_elems];
}

T vector_get_at(const vector_p v, const int id)
{
    assert(v != NULL && id >= 0 && id < v->nb_elems);
    return v->tab[id];
}

void vector_set_at(vector_p v, const int id, const T elem)
{
    assert(v != NULL && id >= 0 && id < v->nb_elems);
    v->tab[id] = elem;
}

int vector_len(const vector_p v)
{
    assert(v != NULL);
    return v->nb_elems;
}

void swap(vector_p v, int a, int b)
{
    T tmp = v->tab[a];
    v->tab[a] = v->tab[b];
    v->tab[b] = tmp;
}

void vector_insert_front(vector_p v, const T x)
{
    vector_append(v, x);
    int n = vector_len(v);
    for (int i = n - 1; i > 0; --i)
    {
        swap(v, i, i - 1);
    }
}
