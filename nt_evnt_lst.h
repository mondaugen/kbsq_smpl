#ifndef NT_EVNT_LST_H
#define NT_EVNT_LST_H 

#include "mm_dllist.h" 
#include "err.h" 
#include "nt_evnt.h" 

typedef struct vvvv_nt_evnt_lst_t {
    MMDLList lst_hd;
} vvvv_nt_evnt_lst_t;

void vvvv_nt_evnt_lst_init(vvvv_nt_evnt_lst_t *nel);
vvvv_err_t vvvv_nt_evnt_lst_insert(vvvv_nt_evnt_lst_t *nel,
                                   vvvv_nt_evnt_t *nev);

#endif /* NT_EVNT_LST_H */
