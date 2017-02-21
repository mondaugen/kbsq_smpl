#include <stdio.h> 
#include "undo_cmd_prsr.h" 

static vvvv_cmd_prsr_vtab_t  undo_cmd_prsr_vtab;
static vvvv_cmd_prsr_vtab_t *undo_cmd_prsr_vtab_ptr = 0;

/* Simply calls undo. The command takes no arguments and so no arguments need
 * be parsed. */
static vvvv_err_t prs_args(vvvv_cmd_prsr_t *cp, char *str)
{
    vvvv_undo_cmd_prsr_t *necp = (vvvv_undo_cmd_prsr_t*)cp;
    vvvv_cmd_q_undo_cur_cmd(*necp->cq);
    return vvvv_err_NONE;
}

void vvvv_undo_cmd_prsr_init(vvvv_undo_cmd_prsr_t *necp,
                                vvvv_undo_cmd_prsr_init_t *necpi)
{
    if (!undo_cmd_prsr_vtab_ptr) {
        undo_cmd_prsr_vtab.prs_args = prs_args;
        undo_cmd_prsr_vtab_ptr = &undo_cmd_prsr_vtab;
    }
    vvvv_cmd_prsr_t *cp = (vvvv_cmd_prsr_t*)necp;
    cp->v = undo_cmd_prsr_vtab_ptr;
    cp->cmd_tkn = UNDO_CMD_PRSR_TOK;
    necp->cq  = necpi->cq;
}
