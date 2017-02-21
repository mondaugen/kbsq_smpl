#ifndef UNDO_CMD_PRSR_H
#define UNDO_CMD_PRSR_H 

#include "cmd_q.h" 
#include "cmd_prsr.h"
#include "err.h" 

#define UNDO_CMD_PRSR_TOK "undo" 

typedef struct vvvv_undo_cmd_prsr_t {
    vvvv_cmd_prsr_t super;
    /* The command queue the undos are called on */
    vvvv_cmd_q_t **cq;
} vvvv_undo_cmd_prsr_t;

typedef struct vvvv_undo_cmd_prsr_init_t {
    vvvv_cmd_q_t **cq;
} vvvv_undo_cmd_prsr_init_t;

void vvvv_undo_cmd_prsr_init(vvvv_undo_cmd_prsr_t *necp,
                             vvvv_undo_cmd_prsr_init_t *necpi);

#endif /* UNDO_CMD_PRSR_H */
