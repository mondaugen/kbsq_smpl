#include "mm_dllist.h" 
#include "nt_evnt_lst.h"

void vvvv_nt_evnt_lst_init(vvvv_nt_evnt_lst_t *nel)
{
    MMDLList_init(&nel->lst_hd);
}

vvvv_err_t vvvv_nt_evnt_lst_insert(vvvv_nt_evnt_lst_t *nel,
                                   vvvv_nt_evnt_t *nev)
{
    vvvv_nt_evnt_t *ptr = (vvvv_nt_evnt_t*)&nel->lst_hd;
    /* List is sorted by pitch */
    while (MMDLList_getNext(ptr)
            && (((vvvv_nt_evnt_t*)MMDLList_getNext(ptr))->pch
                > nev->pch)) {
        ptr = (vvvv_nt_evnt_t*)MMDLList_getNext(ptr);
    }
    MMDLList_insertAfter((MMDLList*)ptr,(MMDLList*)nev);
    return vvvv_err_NONE;
}


                    
