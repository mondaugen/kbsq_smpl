#include "rm_nt_evnt_cmd.h"

static vvvv_cmd_vtab_t rm_nt_evnt_cmd_vtab;
static vvvv_cmd_vtab_t *rm_nt_evnt_cmd_vtab_ptr = 0;

/* Find nt_evnt described by rcmd->nev, remove if present and store in fnd_nev
 * */
static vvvv_err_t rm_nt_evnt_cmd_redo(vvvv_cmd_t *cmd)
{
    vvvv_rm_nt_evnt_cmd_t *rcmd = (vvvv_rm_nt_evnt_cmd_t*)cmd;
    vvvv_err_t ret;
    rcmd->fnd_nev = vvvv_nt_evnt_sq_fnd(rcmd->nes,
                        rcmd->trk,
                        &rcmd->nev,
                        vvvv_nt_evnt_sq_fnd_flgs_TS_PCH);
    if (rcmd->fnd_nev) {
        MMDLList_remove((MMDLList*)rcmd->fnd_nev);
        // Set pointers to previous and next to NULL
        MMDLList_init((MMDLList*)rcmd->fnd_nev); 
    }
    cmd->dn = vvvv_cmd_done_TRUE;
    return vvvv_err_NONE;
}

/* Put fnd_nev back into the sequence */
static vvvv_err_t rm_nt_evnt_cmd_undo(vvvv_cmd_t *cmd)
{
    vvvv_rm_nt_evnt_cmd_t *rcmd = (vvvv_rm_nt_evnt_cmd_t*)cmd;
    vvvv_err_t ret = vvvv_err_NONE;
    /* Only attempt reinsertion if an event was successfully extracted */
    if (rcmd->fnd_nev) {
        ret = vvvv_nt_evnt_sq_insert(rcmd->nes, rcmd->trk, rcmd->fnd_nev);
    }
    if (ret != vvvv_err_NONE) {
        // Problem inserting event, free it (sorry folks, that's all we can do)
        vvvv_nt_evnt_free(rcmd->fnd_nev);
    }
    rcmd->fnd_nev = NULL;
    cmd->dn = vvvv_cmd_done_FALSE;
    return ret;
}

static void rm_nt_evnt_cmd_free(vvvv_cmd_t *cmd)
{
    vvvv_rm_nt_evnt_cmd_t *rcmd = (vvvv_rm_nt_evnt_cmd_t*)cmd;
    /* Only free if event has been removed (otherwise there shouldn't be an
     * event stored in fnd_nev)*/
    if ((cmd->dn == vvvv_cmd_done_TRUE)
            && (rcmd->fnd_nev != NULL)) {
        vvvv_nt_evnt_free(rcmd->fnd_nev);
    }
    free(rcmd);
}

static void vvvv_rm_nt_evnt_cmd_init(vvvv_rm_nt_evnt_cmd_t *rcmd,
                                     const vvvv_nt_evnt_cmd_init_t *nci)
{
    if (!rm_nt_evnt_cmd_vtab_ptr) {
        rm_nt_evnt_cmd_vtab.redo = rm_nt_evnt_cmd_redo;
        rm_nt_evnt_cmd_vtab.undo = rm_nt_evnt_cmd_undo;
        rm_nt_evnt_cmd_vtab.free = rm_nt_evnt_cmd_free;
        rm_nt_evnt_cmd_vtab_ptr = &rm_nt_evnt_cmd_vtab;
    }
    vvvv_cmd_t *cmd = (vvvv_cmd_t*)rcmd;
    cmd->v = rm_nt_evnt_cmd_vtab_ptr;
    cmd->dn = vvvv_cmd_done_FALSE;
    vvvv_nt_evnt_init(&rcmd->nev, &nci->nei);
    rcmd->trk = nci->trk;
    rcmd->nes = nci->nes;
    rcmd->fnd_nev = NULL;
}

/* Returns NULL upon failure to allocate */
vvvv_rm_nt_evnt_cmd_t *
vvvv_rm_nt_evnt_cmd_new(const vvvv_nt_evnt_cmd_init_t *nci)
{
    vvvv_rm_nt_evnt_cmd_t *ret;
    ret = (vvvv_rm_nt_evnt_cmd_t*)malloc(sizeof(vvvv_rm_nt_evnt_cmd_t));
    if (ret) {
        vvvv_rm_nt_evnt_cmd_init(ret,nci);
    }
    return ret;
}
