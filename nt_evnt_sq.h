#ifndef NT_EVNT_SQ_H
#define NT_EVNT_SQ_H 

#include "nt_evnt_lst.h"
#include "tmstmp.h"
#include "nt_evnt.h" 

#include <stdio.h> 

typedef struct vvvv_nt_evnt_sq_t {
    /* The time between two adjacent indices in evnt_lsts */
    vvvv_tmstmp_t tick_dur; 
    size_t n_tcks; /* Length as number of ticks ("horizontal length") */
    size_t n_trks; /* Length as number of tracks ("vertical length") */
    vvvv_nt_evnt_lst_t evnt_lsts[];
} vvvv_nt_evnt_sq_t;

typedef struct vvvv_nt_evnt_sq_init_t {
    /* The time between two adjacent indices in evnt_lsts */
    vvvv_tmstmp_t tick_dur; 
    size_t n_tcks; /* Length as number of ticks ("horizontal length") */
    size_t n_trks; /* Length as number of tracks ("vertical length") */
} vvvv_nt_evnt_sq_init_t;

vvvv_nt_evnt_sq_t *vvvv_nt_evnt_sq_new(const vvvv_nt_evnt_sq_init_t *init);
vvvv_err_t vvvv_nt_evnt_sq_insert(vvvv_nt_evnt_sq_t *nes,
                                  size_t trk,
                                  vvvv_nt_evnt_t *nev);

static inline vvvv_nt_evnt_lst_t * 
vvvv_nt_evnt_sq_get_evnt_lst(vvvv_nt_evnt_sq_t *nes, size_t trk, size_t tck)
{
    if ((trk < nes->n_trks) && (tck < nes->n_tcks)) {
        return &nes->evnt_lsts[(((nes)->n_tcks) * (trk)) + (tck)];
    }
    return NULL;
}

#endif /* NT_EVNT_SQ_H */
