#include <stdio.h> 
#include "redo_cmd_prsr.h" 

static vvvv_cmd_prsr_vtab_t  redo_cmd_prsr_vtab;
static vvvv_cmd_prsr_vtab_t *redo_cmd_prsr_vtab_ptr = 0;

/* Simply calls redo. The command takes no arguments and so no arguments need
 * be parsed. */
static vvvv_err_t prs_args(vvvv_cmd_prsr_t *cp, char *str)
{
    vvvv_redo_cmd_prsr_t *necp = (vvvv_redo_cmd_prsr_t*)cp;
    vvvv_cmd_q_redo_next_cmd(*necp->cq);
    return vvvv_err_NONE;
}

void vvvv_redo_cmd_prsr_init(vvvv_redo_cmd_prsr_t *necp,
                                vvvv_redo_cmd_prsr_init_t *necpi)
{
    if (!redo_cmd_prsr_vtab_ptr) {
        redo_cmd_prsr_vtab.prs_args = prs_args;
        redo_cmd_prsr_vtab_ptr = &redo_cmd_prsr_vtab;
    }
    vvvv_cmd_prsr_t *cp = (vvvv_cmd_prsr_t*)necp;
    cp->v = redo_cmd_prsr_vtab_ptr;
    cp->cmd_tkn = REDO_CMD_PRSR_TOK;
    necp->cq  = necpi->cq;
}

