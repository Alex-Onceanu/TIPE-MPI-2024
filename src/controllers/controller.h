#ifndef CONTROLLER_H
#define CONTROLLER_H

typedef struct controller
{
    // Methodes comme pointeurs de fonctions
    void (*process_input)(struct controller *, void *);
    void (*update)(struct controller *);
    void (*draw)(struct controller *);

    unsigned int program_index;
} controller_t, *controller_p;

// Constructeur
controller_p Controller();

// Destructeur
void controller_free(controller_p this);

// Mutateur de l'attribut program_index
void controller_set_program_id(controller_p this, unsigned int __program_index);

// Accesseur de l'attribut program_index
unsigned int controller_get_program_id(controller_p this);

#endif