#ifndef RM_NT_EVNT_CMD_H
#define RM_NT_EVNT_CMD_H 

#include <stddef.h>

#include "nt_evnt.h" 
#include "cmd.h" 
#include "nt_evnt_sq.h" 
#include "nt_evnt_cmd.h"

typedef struct vvvv_rm_nt_evnt_cmd_t {
    vvvv_cmd_t super;
    vvvv_nt_evnt_t nev;
    size_t trk;
    vvvv_nt_evnt_sq_t *nes;
    vvvv_nt_evnt_t *fnd_nev; /* The note that was found. */
} vvvv_rm_nt_evnt_cmd_t;

vvvv_rm_nt_evnt_cmd_t *
vvvv_rm_nt_evnt_cmd_new(const vvvv_nt_evnt_cmd_init_t *nci);

#endif /* RM_NT_EVNT_CMD_H */
