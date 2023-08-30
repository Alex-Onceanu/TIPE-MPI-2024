#include "materiau.h"

materiau_t Materiau(MATERIAU_TYPE_t type)
{
    switch (type)
    {
    case FER:
        return (materiau_t){{0.49, 0.49, 0.49}, {0.50754, 0.50754, 0.50754}, {0.508273, 0.508273, 0.508273}, 51.2};
    case BRIQUE:
        return (materiau_t){{0.2, 0.2, 0.2}, {0.5, 0.4, 0.4}, {0.7, 0.04, 0.04}, 100};
    case SABLE:
        return (materiau_t){{0.5, 0.5, 0.5}, {0.7, 0.7, 0.7}, {0.0, 0.0, 0.0}, 8};
    case SOLEIL:
        return (materiau_t){{1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 8};
    }

    return (materiau_t){{0.2, 0.2, 0.2}, {0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, 32};
}