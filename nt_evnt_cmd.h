#ifndef NT_EVNT_CMD_H
#define NT_EVNT_CMD_H 

#include <stddef.h>

#include "nt_evnt.h" 

typedef struct vvvv_nt_evnt_cmd_t {
    vvvv_cmd_t super;
    vvvv_nt_evnt_t *nev;
    size_t trk;
    vvvv_nt_evnt_sq_t *nes;
} vvvv_nt_evnt_cmd_t;

typedef struct vvvv_nt_evnt_cmd_init_t {
    size_t trk;
    vvvv_nt_evnt_sq_t *nes;
    vvvv_nt_evnt_init_t nei;
} vvvv_nt_evnt_cmd_init_t;

#endif /* NT_EVNT_CMD_H */
