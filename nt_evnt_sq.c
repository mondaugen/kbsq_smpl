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

/* Iterate over note events in range and call fun on the events.
 * Data is passed as the last argument to fun.
 * It is an error to have ts1 < ts0 or pch1 > pch0
 */
static void vvvv_nt_evnt_sq_mp(vvvv_nt_evnt_sq_t *nes,
                               vvvv_nt_evnt_sq_rng_t *nesr,
                               void (*func)(vvvv_nt_evnt_sq_t*,vvvv_nt_evnt_t*,void*),
                               void *data)
{
    if ((nesr->ts0 > nesr->ts1) || (nesr->pch0 > nesr->pch1)) {
        return;
    }
    if (nesr->trk >= nes->n_trks) {
        return;
    }
    vvvv_nt_evnt_t *ret;
    size_t i = 0;
    while (i < nes->n_tcks) {
        if (((i * nes->tick_dur) >= nesr->ts0) && ((i * nes->tick_dur) < nesr->ts1)) {
            vvvv_nt_evnt_lst_t *nel;
            nel = vvvv_nt_evnt_sq_get_evnt_lst(nes, nesr->trk, i);
            if (nel) {
                vvvv_nt_evnt_t *ptr = (vvvv_nt_evnt_t*)&nel->lst_hd;
                while ((ptr = (vvvv_nt_evnt_t*)MMDLList_getNext(ptr))) {
                    if ((ptr->pch >= nesr->pch0)
                            && (ptr->pch < nesr->pch1)
                            && (ptr->ts >= nesr->ts0)
                            && (ptr->ts < nesr->ts1)) {
                        /* If in range, call function on it. We have to check ts
                         * again because some events' timestamps could be slightly
                         * after this index in the sequence multiplied by the tick
                         * duration. */
                        func(nes,ptr,data);
                    }
                }
            }
        }
        i++;
    }
}

static void nt_evnt_sq_gt_nm_evnts_rng_func(vvvv_nt_evnt_sq_t *nes,
                                            vvvv_nt_evnt_t *nev,
                                            void *data)
{
    size_t *cnt = (size_t*)data;
    *cnt += 1;
}

size_t vvvv_nt_evnt_sq_gt_nm_evnts_rng(vvvv_nt_evnt_sq_t *nes,
                                       vvvv_nt_evnt_sq_rng_t *nesr)
{
    size_t cnt = 0;
    vvvv_nt_evnt_sq_mp(nes,
                       nesr,
                       nt_evnt_sq_gt_nm_evnts_rng_func,
                       (void*)&cnt);
    return cnt;
}

typedef struct nt_evnt_sq_gt_evnts_t {
    vvvv_nt_evnt_t **evnts;
    size_t evnt_cnt;
} nt_evnt_sq_gt_evnts_t;

void nt_evnt_sq_gt_evnts_rng_func(vvvv_nt_evnt_sq_t *nes,
                                  vvvv_nt_evnt_t *nev,
                                  void *data)
{
    nt_evnt_sq_gt_evnts_t *nesge = (nt_evnt_sq_gt_evnts_t*)data;
    nesge->evnts[nesge->evnt_cnt++] = nev;
}

static void vvvv_nt_evnt_sq_gt_evnts_rng_(vvvv_nt_evnt_sq_t *nes,
                                          vvvv_nt_evnt_sq_rng_t *nesr,
                                          vvvv_nt_evnt_t **evnts)
{
    nt_evnt_sq_gt_evnts_t nesge = {evnts, 0};
    vvvv_nt_evnt_sq_mp(nes,nesr,nt_evnt_sq_gt_evnts_rng_func,(void*)&nesge);
}


/* Get note events between ts0 and ts1, pch0 and pch1.
 * Returns a null terminated array of pointers to nt_evnt_t types.
 * This array should be freed when it is no longer needed.
 * */
vvvv_nt_evnt_t **vvvv_nt_evnt_sq_gt_evnts_rng(vvvv_nt_evnt_sq_t *nes,
                                              vvvv_nt_evnt_sq_rng_t *nesr)
{
    size_t num_nevs = vvvv_nt_evnt_sq_gt_nm_evnts_rng(nes,nesr);
    if (num_nevs == 0) {
        return NULL;
    }
    vvvv_nt_evnt_t **rslt = (vvvv_nt_evnt_t**) malloc(sizeof(vvvv_nt_evnt_t*)*num_nevs);
    if (!rslt) {
        return NULL;
    }
    rslt[num_nevs-1] = NULL;
    vvvv_nt_evnt_sq_gt_evnts_rng_(nes,nesr,rslt);
    return rslt;
}
