#include <stdio.h> 
#include "nt_evnt_cmd_prsr.h" 

static vvvv_cmd_prsr_vtab_t  nt_evnt_cmd_prsr_vtab;
static vvvv_cmd_prsr_vtab_t *nt_evnt_cmd_prsr_vtab_ptr = 0;

/* Parses arguments of the form:
 * track timestamp pitch velocity duration
 */
static vvvv_err_t prs_args(vvvv_cmd_prsr_t *cp, char *str)
{
    vvvv_nt_evnt_cmd_prsr_t *necp = (vvvv_nt_evnt_cmd_prsr_t*)cp;
    int trk;
    vvvv_tmstmp_t ts, dur;
    float pch, vel;
    int nargs_prsd, n;
    vvvv_nt_evnt_cmd_init_t neci;
    neci.nes = *necp->nes;
    if (str) {
        nargs_prsd = sscanf(str,
                            "%lu "
                                VVVV_TMSTMP_SCANF_STR
                                " %f %f "
                                VVVV_TMSTMP_SCANF_STR,
                            &neci.trk,
                            &neci.nei.ts,
                            &neci.nei.pch,
                            &neci.nei.vel,
                            &neci.nei.dur);
    } else {
        nargs_prsd = 0;
    }
    if (nargs_prsd < 5) {
        neci.nei.dur = NT_EVNT_CMD_PRSR_DFLT_DUR;
    }
    if (nargs_prsd < 4) {
        neci.nei.vel = NT_EVNT_CMD_PRSR_DFLT_VEL;
    }
    if (nargs_prsd < 3) {
        neci.nei.pch = NT_EVNT_CMD_PRSR_DFLT_PCH;
    }
    if (nargs_prsd < 2) {
        neci.nei.ts = NT_EVNT_CMD_PRSR_DFLT_TS;
    }
    if (nargs_prsd < 1) {
        neci.trk = NT_EVNT_CMD_PRSR_DFLT_TRK;
    }
    vvvv_nt_evnt_cmd_t *nevc;
    nevc = vvvv_nt_evnt_cmd_new(&neci);
    if (!nevc) {
        return vvvv_err_EFULL;
    }
    vvvv_cmd_q_push_cmd(*necp->cq,(vvvv_cmd_t*)nevc);
    /* Error ignored... */
    (void) vvvv_cmd_q_redo_next_cmd(*necp->cq);
    return vvvv_err_NONE;
}

void vvvv_nt_evnt_cmd_prsr_init(vvvv_nt_evnt_cmd_prsr_t *necp,
                                vvvv_nt_evnt_cmd_prsr_init_t *necpi)
{
    if (!nt_evnt_cmd_prsr_vtab_ptr) {
        nt_evnt_cmd_prsr_vtab.prs_args = prs_args;
        nt_evnt_cmd_prsr_vtab_ptr = &nt_evnt_cmd_prsr_vtab;
    }
    vvvv_cmd_prsr_t *cp = (vvvv_cmd_prsr_t*)necp;
    cp->v = nt_evnt_cmd_prsr_vtab_ptr;
    cp->cmd_tkn = NT_EVNT_CMD_PRSR_TOK;
    necp->nes = necpi->nes;
    necp->cq  = necpi->cq;
}
