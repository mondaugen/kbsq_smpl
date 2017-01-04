#include "nt_evnt_cmd.h"

static vvvv_cmd_vtab_t  nt_evnt_cmd_vtab;
static vvvv_cmd_vtab_t *nt_evnt_cmd_vtab_ptr = 0;

static vvvv_err_t nt_evnt_cmd_redo(vvvv_cmd_t *cmd)
{
    vvvv_nt_evnt_cmd_t *ncmd = (vvvv_nt_evnt_cmd_t*)cmd;
    vvvv_err_t ret;
    ret = vvvv_nt_evnt_sq_insert(ncmd->nes, ncmd->trk, ncmd->nev);
    cmd->dn = vvvv_cmd_done_TRUE;
    return ret;
}

static vvvv_err_t nt_evnt_cmd_undo(vvvv_cmd_t *cmd)
{
    vvvv_nt_evnt_cmd_t *ncmd = (vvvv_nt_evnt_cmd_t*)cmd;
    MMDLList_remove((MMDLList*)ncmd->nev);
    MMDLList_init((MMDLList*)ncmd->nev); // Set pointers to previous and next to NULL
    cmd->dn = vvvv_cmd_done_FALSE;
    return vvvv_err_NONE;
}

static void nt_evnt_free(vvvv_cmd_t *cmd)
{
    vvvv_nt_evnt_cmd_t *ncmd = (vvvv_nt_evnt_cmd_t*)cmd;
    if (ncmd->nev) {
        vvvv_nt_evnt_free(ncmd->nev);
    }
    free(ncmd);
}

static void vvvv_nt_evnt_cmd_init(vvvv_nt_evnt_cmd_t *ncmd, const vvvv_nt_evnt_cmd_init_t *nci)
{
    if (!nt_evnt_cmd_vtab_ptr) {
        nt_evnt_cmd_vtab.redo = nt_evnt_cmd_redo;
        nt_evnt_cmd_vtab.undo = nt_evnt_cmd_undo;
        nt_evnt_cmd_vtab.free = nt_evnt_cmd_free;
        nt_evnt_cmd_vtab_ptr = &nt_evnt_cmd_vtab;
    }
    vvvv_cmd_t *cmd = (vvvv_cmd_t*)ncmd;
    cmd->v = nt_evnt_cmd_vtab_ptr;
    cmd->done = vvvv_cmd_done_FALSE;
    ncmd->nev = vvvv_nt_evnt_new(&nci->nei);
    ncmd->trk = nci->trk;
    ncmd->nes = nci->nes;
}

/* Returns NULL upon failure to allocate */
vvvv_nt_evnt_cmd_t *
vvvv_nt_evnt_cmd_new(const vvvv_nt_evnt_cmd_init_t *nci)
{
    vvvv_nt_evnt_cmd_t *ret;
    ret = (vvvv_nt_evnt_cmd_t*)malloc(sizeof(vvvv_nt_evnt_cmd_t));
    if (ret) {
        vvvv_nt_evnt_cmd_init(ret,nci);
    }
    return ret;
}
