#ifndef NT_EVNT_CMD_PRSR_H
#define NT_EVNT_CMD_PRSR_H 

/* Parses commands that yield note events */

#include "cmd_prsr.h"

#define NT_EVNT_CMD_PRSR_CMD_TOK "note" 
#define NT_EVNT_CMD_PRSR_DFLT_DUR 24 // TODO: Update this to be the length of one beat in the sequence
#define NT_EVNT_CMD_PRSR_DFLT_VEL 1
#define NT_EVNT_CMD_PRSR_DFLT_PCH 60.
#define NT_EVNT_CMD_PRSR_DFLT_TS  0
#define NT_EVNT_CMD_PRSR_DFLT_TRK 0

typedef struct vvvv_nt_evnt_cmd_prsr_t {
    vvvv_cmd_prsr_t super;
    /* The sequence the note events are placed in */
    vvvv_nt_evnt_sq_t *nes;
    /* The command queue the note event commands are placed in */
    vvvv_cmd_q_t *cq;
} vvvv_nt_evnt_cmd_prsr_t;

typedef struct vvvv_nt_evnt_cmd_prsr_init_t {
    vvvv_nt_evnt_sq_t *nes;
    vvvv_cmd_q_t *cq;
} vvvv_nt_evnt_cmd_prsr_init_t;

void vvvv_nt_evnt_cmd_prsr_init(vvvv_nt_evnt_cmd_prsr_t *necp,
                                  vvvv_nt_evnt_cmd_prsr_init_t *necpi);

#endif /* NT_EVNT_CMD_PRSR_H */
