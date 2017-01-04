#ifndef EMALLOC_H
#define EMALLOC_H 

#include  <stddef.h>

void *emalloc(size_t size);
void *ecalloc(size_t count, size_t size);

#endif /* EMALLOC_H */
