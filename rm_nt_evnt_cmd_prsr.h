#ifndef RM_NT_EVNT_CMD_PRSR_H
#define RM_NT_EVNT_CMD_PRSR_H 

/* Parses commands that delete note events. */

#include "cmd_q.h" 
#include "nt_evnt_sq.h" 
#include "cmd_prsr.h"
#include "err.h" 
#include "tmstmp.h"
#include "rm_nt_evnt_cmd.h"

#define RM_NT_EVNT_CMD_PRSR_TOK "rm"
#define RM_NT_EVNT_CMD_PRSR_DFLT_PCH 60.
#define RM_NT_EVNT_CMD_PRSR_DFLT_TS  0
#define RM_NT_EVNT_CMD_PRSR_DFLT_TRK 0

typedef struct vvvv_rm_nt_evnt_cmd_prsr_t {
    vvvv_cmd_prsr_t super;
    /* The sequence the note events are removed from */
    vvvv_nt_evnt_sq_t **nes;
    /* The command queue the note event commands are placed in */
    vvvv_cmd_q_t **cq;
} vvvv_rm_nt_evnt_cmd_prsr_t;

typedef struct vvvv_rm_nt_evnt_cmd_prsr_init_t {
    vvvv_nt_evnt_sq_t **nes;
    vvvv_cmd_q_t **cq;
} vvvv_rm_nt_evnt_cmd_prsr_init_t;

void vvvv_rm_nt_evnt_cmd_prsr_init(vvvv_rm_nt_evnt_cmd_prsr_t *necp,
                                vvvv_rm_nt_evnt_cmd_prsr_init_t *necpi);

#endif /* RM_NT_EVNT_CMD_PRSR_H */
