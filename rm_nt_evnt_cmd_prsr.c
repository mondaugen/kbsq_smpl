#include <stdio.h>
#include "rm_nt_evnt_cmd_prsr.h" 

static vvvv_cmd_prsr_vtab_t  rm_nt_evnt_cmd_prsr_vtab;
static vvvv_cmd_prsr_vtab_t *rm_nt_evnt_cmd_prsr_vtab_ptr = 0;

/* Parses arguments of the form:
 * track timestamp pitch 
 */
static vvvv_err_t prs_args(vvvv_cmd_prsr_t *cp, char *str)
{
    vvvv_rm_nt_evnt_cmd_prsr_t *necp = (vvvv_rm_nt_evnt_cmd_prsr_t*)cp;
    int nargs_prsd;
    vvvv_nt_evnt_cmd_init_t neci;
    neci.nes = *necp->nes;
    if (str) {
        nargs_prsd = sscanf(str,
                            "%lu "
                                VVVV_TMSTMP_SCANF_STR
                                " %f",
                            &neci.trk,
                            &neci.nei.ts,
                            &neci.nei.pch,
                );
    } else {
        nargs_prsd = 0;
    }
    if (nargs_prsd < 3) {
        neci.nei.pch = RM_NT_EVNT_CMD_PRSR_DFLT_PCH;
    }
    if (nargs_prsd < 2) {
        neci.nei.ts = RM_NT_EVNT_CMD_PRSR_DFLT_TS;
    }
    if (nargs_prsd < 1) {
        neci.trk = RM_NT_EVNT_CMD_PRSR_DFLT_TRK;
    }
    vvvv_rm_nt_evnt_cmd_t *nevc;
    /* Assumes that it only finds note based on timestamp and pitch */
    nevc = vvvv_rm_nt_evnt_cmd_new(&neci);
    if (!nevc) {
        return vvvv_err_EFULL;
    }
    vvvv_cmd_q_push_cmd(*necp->cq,(vvvv_cmd_t*)nevc);
    /* Error ignored... */
    (void) vvvv_cmd_q_redo_next_cmd(*necp->cq);
    return vvvv_err_NONE;
}

void vvvv_rm_nt_evnt_cmd_prsr_init(vvvv_rm_nt_evnt_cmd_prsr_t *necp,
                                vvvv_rm_nt_evnt_cmd_prsr_init_t *necpi)
{
    if (!rm_nt_evnt_cmd_prsr_vtab_ptr) {
        rm_nt_evnt_cmd_prsr_vtab.prs_args = prs_args;
        rm_nt_evnt_cmd_prsr_vtab_ptr = &nt_evnt_cmd_prsr_vtab;
    }
    vvvv_cmd_prsr_t *cp = (vvvv_cmd_prsr_t*)necp;
    cp->v = rm_nt_evnt_cmd_prsr_vtab_ptr;
    cp->cmd_tkn = RM_NT_EVNT_CMD_PRSR_TOK;
    necp->nes = necpi->nes;
    necp->cq  = necpi->cq;
}
