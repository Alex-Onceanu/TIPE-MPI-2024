#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hashtable.h"
#include "element.h"

//________________________________CHAINE______________________________________

CHAINE *chaine_vide()
{
    CHAINE *res = malloc(sizeof(CHAINE));

    res->taille = 0;
    res->tete = NULL;

    return res;
}

MAILLON *tete(CHAINE *c)
{
    assert(c != NULL);
    return c->tete;
}

int taille(CHAINE *c)
{
    assert(c != NULL);
    return c->taille;
}

MAILLON *suivant(MAILLON *m)
{
    assert(m != NULL);
    return m->suiv;
}

void maillon_free(MAILLON *m)
{
    free(m->key);
    free(m);
}

void ajouter_debut(CHAINE *c, KEY k, VAL v)
{
    assert(c != NULL);

    MAILLON *nouv = malloc(sizeof(MAILLON));
    nouv->key = k;
    nouv->val = v;

    nouv->prec = NULL;
    nouv->suiv = c->tete;
    if (c->tete != NULL)
        c->tete->prec = nouv;

    c->tete = nouv;

    c->taille++;
}

MAILLON *recherche_chaine(CHAINE *c, KEY k)
{
    assert(c != NULL);
    MAILLON *curs = c->tete;
    for (int i = 0; i < c->taille; ++i)
    {
        if (strcmp(curs->key, k) == 0)
            return curs;
        curs = curs->suiv;
    }
    return NULL;
}

void supprimer_chaine(CHAINE *c, MAILLON *m)
{
    assert(c != NULL && m != NULL);
    if (m->prec == NULL)
    {
        c->tete = m->suiv;
    }
    else
    {
        m->prec->suiv = m->suiv;
    }
    if (m->suiv != NULL)
    {
        m->suiv->prec = m->prec;
    }
    maillon_free(m);
    c->taille--;
}

void print_chaine(CHAINE *c)
{
    assert(c != NULL);
    MAILLON *curs = c->tete;
    printf("{\n");
    for (int i = 0; i < c->taille; ++i)
    {
        printf("( ");
        print_key(curs->key);
        printf(" : ");
        print_val(curs->val);
        printf(" ),\n");
        curs = curs->suiv;
    }
    printf("}\n");
}

void free_chaine(CHAINE *c)
{
    assert(c != NULL);
    if (taille(c) > 0)
    {
        MAILLON *curs = c->tete;
        for (int i = 0; i < c->taille - 1; ++i)
        {
            curs = curs->suiv;
            maillon_free(curs->prec);
        }
        maillon_free(curs);
    }
    free(c);
}

//___________________________________HASHMAP__________________________________

DICT *dict_vide(int taille)
{
    DICT *res = malloc(sizeof(DICT));
    res->m = taille;
    res->n = 0;

    res->t = malloc(res->m * sizeof(CHAINE *));
    for (int i = 0; i < res->m; ++i)
    {
        res->t[i] = chaine_vide();
    }

    return res;
}

VAL rechercher(DICT *d, KEY k)
{
    assert(d != NULL);

    int id = hash(k, d->m);
    MAILLON *trouvaille = recherche_chaine(d->t[id], k);

    if (trouvaille == NULL)
        return -1;
    return trouvaille->val;
}

bool appartient(DICT *d, KEY k)
{
    assert(d != NULL);

    return rechercher(d, k) != -1;
}

void inserer(DICT *d, KEY k, VAL v)
{
    assert(d != NULL);
    assert(v >= 0);

    int id = hash(k, d->m);
    ajouter_debut(d->t[id], k, v);
    d->n++;
}

void supprimer(DICT *d, KEY k)
{
    assert(d != NULL);

    int id = hash(k, d->m);
    MAILLON *trouvaille = recherche_chaine(d->t[id], k);

    supprimer_chaine(d->t[id], trouvaille);
}

void print_dict(DICT *d)
{
    assert(d != NULL);

    MAILLON *curs = NULL;
    for (int i = 0; i < d->m; ++i)
    {
        curs = d->t[i]->tete;
        while (curs != NULL)
        {
            printf("%s -> %d\n", curs->key, curs->val);
            curs = curs->suiv;
        }
    }
}

void debug_dict(DICT *d)
{
    assert(d != NULL);

    MAILLON *curs = NULL;
    for (int i = 0; i < d->m; ++i)
    {
        curs = d->t[i]->tete;
        printf("Alveole %d (taille %d):\n", i, taille(d->t[i]));
        while (curs != NULL)
        {
            printf("%s -> %d\n", curs->key, curs->val);
            curs = curs->suiv;
        }
        printf("\n");
    }
}

void free_dict(DICT *d)
{
    assert(d != NULL);

    for (int i = 0; i < d->m; ++i)
    {
        free_chaine(d->t[i]);
    }
    free(d->t);
    free(d);
}
