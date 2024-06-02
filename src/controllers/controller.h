#ifndef CONTROLLER_H
#define CONTROLLER_H

typedef struct controller
{
    // Methodes comme pointeurs de fonctions
    void (*process_input)(struct controller *, void *);
    void (*update)(struct controller *);
    void (*draw)(struct controller *);

    // Chaque type de controlleur aura son propre destructeur
    void(*free)(struct controller *);

    unsigned int program_index;
} controller_t, *controller_p;

// Ne fait rien
void id(controller_p c);
void id_for_process_input(controller_p c, void *data);

// Destructeur par défaut qui ne fait que lancer free(this) 
void controller_free(controller_p this);

// Mutateur de l'attribut program_index
void controller_set_program_id(controller_p this, unsigned int __program_index);

// Accesseur de l'attribut program_index
unsigned int controller_get_program_id(controller_p this);

#endif