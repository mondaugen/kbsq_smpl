#ifndef REDO_CMD_PRSR_H
#define REDO_CMD_PRSR_H 

#include "cmd_q.h" 
#include "cmd_prsr.h"
#include "err.h" 

#define REDO_CMD_PRSR_TOK "redo" 

typedef struct vvvv_redo_cmd_prsr_t {
    vvvv_cmd_prsr_t super;
    /* The command queue the redos are called on */
    vvvv_cmd_q_t **cq;
} vvvv_redo_cmd_prsr_t;

typedef struct vvvv_redo_cmd_prsr_init_t {
    vvvv_cmd_q_t **cq;
} vvvv_redo_cmd_prsr_init_t;

void vvvv_redo_cmd_prsr_init(vvvv_redo_cmd_prsr_t *necp,
                             vvvv_redo_cmd_prsr_init_t *necpi);

#endif /* REDO_CMD_PRSR_H */
