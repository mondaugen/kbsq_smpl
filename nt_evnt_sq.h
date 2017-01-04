#ifndef NT_EVNT_SQ_H
#define NT_EVNT_SQ_H 

#include "nt_evnt_lst.h"
#include "tmstmp.h"

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

#define vvvv_nt_evnt_sq_get_evnt_lst(nes,trk,tck) \
   nes->evnt_lsts[((nes)->n_tcks) * (trk) + (tck)] 

#endif /* NT_EVNT_SQ_H */
