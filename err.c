#include "err.h" 

#include <stdlib.h> 
#include <stdio.h> 

/* Call if unrecoverable error occurs.
 * cleanup will be called with data as argument to cleanup before
 * aborting program.
 */
void vvvv_err_fatal(const char *msg,
                    void (*cleanup)(void *),
                    void *data)
{
    fprintf(stderr,"FATAL ERROR: %s\n",msg);
    if (cleanup) {
        cleanup(data);
    }
    abort();
}
