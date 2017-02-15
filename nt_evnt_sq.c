#include "nt_evnt_sq.h" 
#include "emalloc.h" 

/* Find a note event in the sequence by comparing with fields of cnev.
 * Returns a pointer to the event if found or NULL if not.
 */
vvvv_nt_evnt_t *
vvvv_nt_evnt_sq_fnd(vvvv_nt_evnt_sq_t *nes,
                     size_t trk,
                     vvvv_nt_evnt_t *cnev,
                     vvvv_nt_evnt_sq_fnd_flgs_t flgs)
{
    if (trk >= nes->n_trks) {
        return NULL;
    }
    vvvv_nt_evnt_t *ret;
    if (flgs == vvvv_nt_evnt_sq_fnd_flgs_TS_PCH) {
        /* Find based on timestamp and pitch */
        size_t i = 0;
        while (i < nes->n_tcks) {
            /* Halt on slot holding events with cnev's timestamp */
            if (((i+1) * nes->tick_dur) > cnev->ts) {
                /* Check if pitch present and timestamp matches */
                ret = vvvv_nt_evnt_lst_fnd(vvvv_nt_evnt_sq_get_evnt_lst(nes,trk,i),
                                           cnev,vvvv_nt_evnt_lst_fnd_flgs_PCH_TS);
                return ret;
            }
            i++;
        }
    }
    /* No other flags supported at the moment */
    return NULL;
}

vvvv_nt_evnt_sq_t *vvvv_nt_evnt_sq_new(const vvvv_nt_evnt_sq_init_t *init)
{
    vvvv_nt_evnt_sq_t *ret;
    ret = (vvvv_nt_evnt_sq_t*)emalloc(sizeof(vvvv_nt_evnt_sq_t)
                                        + (sizeof(vvvv_nt_evnt_lst_t)
                                            * init->n_tcks * init->n_trks));
    ret->tick_dur = init->tick_dur;
    ret->n_tcks = init->n_tcks;
    ret->n_trks = init->n_trks;
    size_t i, j;
    for (i = 0; i < init->n_trks; i++) {
        for (j = 0; j < init->n_tcks; j++) {
            vvvv_nt_evnt_lst_init(vvvv_nt_evnt_sq_get_evnt_lst(ret,i,j));
        }
    }
    return ret;
}

/* Insert an event on track trk */
vvvv_err_t vvvv_nt_evnt_sq_insert(vvvv_nt_evnt_sq_t *nes,
                                  size_t trk,
                                  vvvv_nt_evnt_t *nev)
{
    size_t i = 0;
    if (trk >= nes->n_trks) {
        return vvvv_err_EBNDS;
    }
    while (i < nes->n_tcks) {
        if (((i+1) * nes->tick_dur) > nev->ts) {
            vvvv_nt_evnt_lst_insert(vvvv_nt_evnt_sq_get_evnt_lst(nes,trk,i),
                                    nev);
            return vvvv_err_NONE;
        }
        i++;
    }
    return vvvv_err_EBNDS;
}

