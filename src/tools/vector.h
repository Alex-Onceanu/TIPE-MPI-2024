#pragma once

// Ces listes peuvent stocker absolument tout sous forme de void*
typedef void *T;

////_____________________________________Vecteur 1D____________________________

/// @brief Liste dynamique stockant des elements de type T dans un seul bloc memoire.
typedef struct vector vector_t, *vector_p;

/// @brief Constructeur de la structure vecteur
/// @return un vecteur vide
vector_p vector_init();

/// @brief Destructeur de la structure vecteur
/// @param v le vecteur qu'on veut liberer, le pointeur sera NULL a la fin de la fonction.
void vector_free(vector_p v);

/// @brief Ajoute inplace un element a la fin de v, la taille du vecteur etant dynamique
/// @param v le vecteur auquel on ajoute un element, inplace
/// @param elem l'element en question, qui sera copie
void vector_append(vector_p v, const T elem);

/// @brief Supprime du vecteur le dernier element (depile) et le renvoie
/// @param v le vecteur en question, qui sera modifie
/// @return le dernier element de v
T vector_pop(vector_p v);

/// @brief Donne une copie de l'element a la id-ieme case.
/// @param v le vecteur en question
/// @param id l'index de l'element qu'on veut obtenir, avec 0 <= id < vector_len(v)
/// @return v[id]
T vector_get_at(const vector_p v, const int id);

/// @brief La valeur a la id-ieme case de v prend la valeur elem
/// @param v le vecteur en question
/// @param id l'index de l'element qu'on veut changer, avec 0 <= id < vector_len(v)
/// @param elem l'element qu'on veut copier a la place de v[id]
void vector_set_at(vector_p v, const int id, const T elem);

/// @brief Compte le nombre d'elements de v
/// @param v le vecteur en question
/// @return le nombre d'elements contenus dans v
int vector_len(const vector_p v);

// Comme append mais au debut du vecteur
// Decale tout le vecteur : O(n)
void vector_insert_front(vector_p v, const T x);
