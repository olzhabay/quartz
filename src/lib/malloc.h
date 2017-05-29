#ifndef __MALLOC_H
#define __MALLOC_H

#include <stddef.h>

typedef struct {
    int enabled;
    int percentage;
    float rate;
} hybrid_malloc_t;

extern hybrid_malloc_t hybrid_malloc;

void *malloc(size_t size);
void free(void *ptr);

#endif // __MALLOC_H
