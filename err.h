#ifndef ERR_H
#define ERR_H 

#include <stdlib.h> 

typedef enum {
    vvvv_err_NONE = 0,
    vvvv_err_EINVAL,    /* Invalid */
    vvvv_err_NOTIMP,    /* Not implemented */
    vvvv_err_EFULL,     /* Full */
    vvvv_err_EBNDS,     /* Out of bounds */
    vvvv_err_DONE,      /* Iterator is done */
    vvvv_err_NMTCH,     /* Doesn't match */
    vvvv_err_EMPTY,     /* Data structure empty */
    vvvv_err_FAIL,      /* Command failed */
} vvvv_err_t;

void vvvv_err_fatal(const char *msg,
                    void (*cleanup)(void *),
                    void *data);

#endif /* ERR_H */
