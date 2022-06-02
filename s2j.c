#include "s2j.h"
#include <stdlib.h>

S2jHook s2jHook = {
        .malloc_fn = malloc,
        .free_fn = free,
};

/**
 * struct2json library initialize
 * @note It will initialize cJSON library hooks.
 */
void s2j_init(S2jHook *hook) {
    /* initialize cJSON library */
    cJSON_InitHooks((cJSON_Hooks *)hook);
    /* initialize hooks */
    if (hook) {
        s2jHook.malloc_fn = (hook->malloc_fn) ? hook->malloc_fn : malloc;
        s2jHook.free_fn = (hook->free_fn) ? hook->free_fn : free;
    } else {
        hook->malloc_fn = malloc;
        hook->free_fn = free;
    }
}
