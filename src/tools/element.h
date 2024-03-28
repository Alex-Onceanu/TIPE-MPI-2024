/*** Fonctions sur les clés / valeurs ***/
// ces fonctions seront à modifier pour changer le
// type des clés ou des valeurs
#include <stdbool.h>

typedef char *KEY;
typedef int VAL;

/* Renvoie true si k1 et k2 sont égales, false sinon */
bool egal(KEY k1, KEY k2);

/* Calcule le hash de la clé k, pour une table de hachage
   à m cases. La valeur renvoie sera entre 0 et m-1 inclus */
int hash(KEY k, int m);

/* Affiche la clé k sans retour à la ligne */
void print_key(KEY k);

/* Affiche la valeur v sans retour à la ligne */
void print_val(VAL v);