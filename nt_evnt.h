#ifndef NT_EVNT_H
#define NT_EVNT_H 
#include "mm_dllist.h" 

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

#endif /* NT_EVNT_H */
