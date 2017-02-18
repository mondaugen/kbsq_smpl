#ifndef NT_EVNT_H
#define NT_EVNT_H 

#include "mm_dllist.h" 
#include "tmstmp.h" 
#include "err.h" 
#include "midi.h" 

typedef struct vvvv_nt_evnt_t {
    MMDLList super;
    vvvv_tmstmp_t ts;
    float pch;
    float vel;
    vvvv_tmstmp_t dur;
} vvvv_nt_evnt_t;

typedef struct vvvv_nt_evnt_init_t {
    vvvv_tmstmp_t ts;
    float pch;
    float vel;
    vvvv_tmstmp_t dur;
} vvvv_nt_evnt_init_t;

vvvv_err_t vvvv_nt_evnt_get_midi_pckt_lst(vvvv_nt_evnt_t *nev,
                                          vvvv_midi_pckt_lst_t *lst,
                                          size_t sz);
vvvv_nt_evnt_t *vvvv_nt_evnt_new(const vvvv_nt_evnt_init_t *nei);
void vvvv_nt_evnt_init(vvvv_nt_evnt_t *nev,
                       const vvvv_nt_evnt_init_t *nei);
void vvvv_nt_evnt_free(vvvv_nt_evnt_t *nev);
int vvvv_nt_evnt_cmp(vvvv_nt_evnt_t *a, vvvv_nt_evnt_t *b);

#endif /* NT_EVNT_H */
