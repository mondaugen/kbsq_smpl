#ifndef NT_EVNT_LST_H
#define NT_EVNT_LST_H 

#include "mm_dllist.h" 
#include "err.h" 
#include "nt_evnt.h" 

typedef struct vvvv_nt_evnt_lst_t {
    MMDLList lst_hd;
} vvvv_nt_evnt_lst_t;

typedef enum {
    vvvv_nt_evnt_lst_fnd_flgs_PCH_TS,
} vvvv_nt_evnt_lst_fnd_flgs_t;

void vvvv_nt_evnt_lst_init(vvvv_nt_evnt_lst_t *nel);
vvvv_err_t vvvv_nt_evnt_lst_insert(vvvv_nt_evnt_lst_t *nel,
                                   vvvv_nt_evnt_t *nev);
vvvv_nt_evnt_t *vvvv_nt_evnt_lst_fnd(vvvv_nt_evnt_lst_t *nel,
                                         vvvv_nt_evnt_t *cnev,
                                         vvvv_nt_evnt_lst_fnd_flgs_t flgs);

#endif /* NT_EVNT_LST_H */
