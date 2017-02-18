#ifndef CMD_PRSR_H
#define CMD_PRSR_H 

#include "err.h"

typedef struct vvvv_cmd_prsr_t vvvv_cmd_prsr_t;

typedef struct vvvv_cmd_prsr_vtab_t {
    vvvv_err_t (*prs_args)(vvvv_cmd_prsr_t *, char *);
} vvvv_cmd_prsr_vtab_t;

struct vvvv_cmd_prsr_t {
    vvvv_cmd_prsr_vtab_t *v;
    vvvv_cmd_prsr_t *next;
    char *cmd_tkn;
};

#define vvvv_cmd_prsr_prs_args(c,s) ((vvvv_cmd_prsr_t*)(c))->v->prs_args((vvvv_cmd_prsr_t*)(c),s) 

void vvvv_cmd_prsr_add(vvvv_cmd_prsr_t *prsr);
vvvv_err_t vvvv_cmd_prsr_prs(char *str);

#endif /* CMD_PRSR_H */
