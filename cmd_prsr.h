#ifndef CMD_PRSR_H
#define CMD_PRSR_H 

typedef struct vvvv_cmd_prsr_t vvvv_cmd_prsr_t;

typedef struct vvvv_cmd_prsr_vtab_t {
    vvvv_err_t (*prs_args)(vvvv_cmd_prsr_t *, char *);
} vvvv_cmd_prsr_vtab_t;

struct vvvv_cmd_prsr_t {
    vvvv_cmd_prsr_vtab_t *v;
    vvvv_cmd_prsr_t *next;
    char *cmd_tkn;
};

#define vvvv_cmd_prsr_prs_args(c,s) ((vvvv_cmd_prsr_t*)c)->((vvvv_cmd_prsr_t*)c,s) 

#endif /* CMD_PRSR_H */
