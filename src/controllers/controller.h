#ifndef CONTROLLER_H
#define CONTROLLER_H

typedef struct controller
{
    // Methodes comme pointeurs de fonctions
    void (*process_input)(struct controller *, void *);
    void (*update)(struct controller *);
    void (*draw)(struct controller *);
} controller_t, *controller_p;

// Constructeur
controller_p Controller();

// Destructeur
void controller_free(controller_p this);

#endif