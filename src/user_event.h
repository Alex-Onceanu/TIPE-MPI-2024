#ifndef USER_EVENT_H
#define USER_EVENT_H

typedef enum EVENT_TYPE
{
    CLICK_DOWN,
    CLICK_UP,

    MOUSE_MOVED,

    W_UP,
    A_UP,
    S_UP,
    D_UP,
    SPACE_UP,
    SHIFT_UP,
    E_UP,

    W_DOWN,
    A_DOWN,
    S_DOWN,
    D_DOWN,
    SPACE_DOWN,
    SHIFT_DOWN,
    E_DOWN
} EVENT_TYPE_t;

typedef struct user_event
{
    EVENT_TYPE_t type;
    void *data;
} user_event_t, *user_event_p;

// Constructeur
user_event_t UserEvent(EVENT_TYPE_t type0, void *data0);

typedef struct f_tuple
{
    float x;
    float y;
} f_tuple_t;

#endif