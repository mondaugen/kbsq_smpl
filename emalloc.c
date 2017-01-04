/* emalloc.c
 *
 * Malloc that checks for heap overflow.
 *
 * These functions are quite severe: the program is aborted if they are
 * unsuccessful. If you require different functionality you must handle heap
 * overflows yourself using the underlying routines.
 */

#include "emalloc.h" 
#include <stdlib.h> 
#include <stdio.h> 

#define EMALLOC_CHK(x)\
   if (!x) {\
      fprintf(stderr,"Out of memory.\n");\
      abort();\
   } 

void *emalloc(size_t size)
{
    void *result = malloc(size);
    EMALLOC_CHK(result);
    return result;
}

void *ecalloc(size_t count, size_t size)
{
    void *result = calloc(count, size);
    EMALLOC_CHK(result);
    return result;
}
