#include "user_event.h"

user_event_t UserEvent(EVENT_TYPE_t type0, void *data0)
{
    user_event_t res;

    res.type = type0;
    res.data = data0;

    return res;
}