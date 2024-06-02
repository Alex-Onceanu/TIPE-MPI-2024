/*
Implémente les dictionnaires par table de hachage
Les collisions sont gérées par chaînage: chaque
case de la table contient une liste chaînée dans
laquelle on stocke les éléments devant être placés
à cet endroit.
*/
#include <stdbool.h>

#include "element.h"

/*** Listes doublement chaînées ***/
typedef struct dict_chain CHAINE;

typedef struct maillon_
{
    KEY key;
    VAL val;
    struct maillon_ *prec;
    struct maillon_ *suiv;
} MAILLON;

struct dict_chain
{
    MAILLON *tete;
    int taille;
};

/* Renvoie une chaîne vide */
CHAINE *chaine_vide();

/* Renvoie le maillon de tête de liste, ou NULL si la liste est vide */
MAILLON *tete(CHAINE *c);

/* Renvoie la taille de la chaîne c */
int taille(CHAINE *c);

/* Renvoie le maillon suivant m, ou NULL si m n'a pas de suivant */
MAILLON *suivant(MAILLON *m);

/* Ajoute le couple (k, v) dans un nouveau maillon en tête de c */
void ajouter_debut(CHAINE *c, KEY k, VAL v);

/* Renvoie un maillon de c contenant un couple (k', v') avec k' = k */
MAILLON *recherche_chaine(CHAINE *c, KEY k);

/* Supprime le maillon m de la chaîne c */
void supprimer_chaine(CHAINE *c, MAILLON *m);

/* Affiche le contenu de la chaîne c, sous la forme:
k1 -> v1
k2 -> v2
...
kn -> vn
*/
void print_chaine(CHAINE *c);

/* Libère la mémoire allouée pour une chaîne */
void free_chaine(CHAINE *c);

/*_______________________ Tables de hachage ________________________*/
struct dict
{
    int m;      // nombre de cases
    int n;      // nombre d'entrées
    CHAINE **t; // table de dictionnaires implémentés par chaîne
};
typedef struct dict DICT;

/* Renvoie un dictionnaire vide */
DICT *dict_vide(int taille);

/* Renvoie true si k est dans d, false sinon */
bool appartient(DICT *d, KEY k);

/* Renvoie la valeur associée à k dans d. k doit être
   une clé de d */
VAL rechercher(DICT *d, KEY k);

/* Insère le couple (k, v) dans d */
void inserer(DICT *d, KEY k, VAL v);

/* Supprime la clé k de d */
void supprimer(DICT *d, KEY k);

/* Affiche le contenu du dictionnaire d, sous la forme:
k1 -> v1
k2 -> v2
...
kn -> vn
*/
void print_dict(DICT *d);

/* Affiche le contenu de d en séparant chaque alvéole. Le
résultat ressemblera à :

Alvéole 0 (taille 2):
k1 -> v1
k2 -> v2
Alvéole 1 (taille 0):
-
Avéole 2 (taille 1):
k3 -> v3
Alvévole 3 (taille 0):
-
...
Alvévole m-1 (taille 3)
k(n-3) -> v(n-3)
k(n-2) -> v(n-2)
k(n-1) -> v(n-1)
*/
void debug_dict(DICT *d);

/* Libère la mémoire allouée pour d */
void free_dict(DICT *d);
