#include <string.h>
#include <stdio.h>

#include "element.h"

int hash(KEY k, int m)
{
    int res = 0;
    for (int i = 0; k[i] != '\0'; i++)
    {
        res = (res * 256 + (int)k[i]) % m;
    }

    return res;
}

bool egal(KEY k1, KEY k2)
{
    return strcmp(k1, k2) == 0;
}

void print_key(KEY k)
{
    printf("%s", k);
}

void print_val(VAL v)
{
    printf("%d", v);
}
