#ifndef CMD_H
#define CMD_H 

#include <stddef.h>
#include "err.h" 

typedef struct vvvv_cmd_t vvvv_cmd_t;

typedef struct vvvv_cmd_vtab_t {
    err_t (*redo) (vvvv_cmd_t *);
    err_t (*undo) (vvvv_cmd_t *);
    void  (*free) (vvvv_cmd_t *);
} vvvv_cmd_vtab_t;

typedef enum {
    vvvv_cmd_done_FALSE,
    vvvv_cmd_done_TRUE
} vvvv_cmd_dn_t;

struct vvvv_cmd_t {
    vvvv_cmd_vtab_t *v;
    vvvv_cmd_dn_t dn;
} vvvv_cmd_t;

#define vvvv_cmd_redo(cmd) ((vvvv_cmd_t*)(cmd))->v->redo((vvvv_cmd_t*)(cmd))
#define vvvv_cmd_undo(cmd) ((vvvv_cmd_t*)(cmd))->v->undo((vvvv_cmd_t*)(cmd))
#define vvvv_cmd_free(cmd) ((vvvv_cmd_t*)(cmd))->v->free((vvvv_cmd_t*)(cmd))
//#define vvvv_cmd_set_redo(cmd,f) do { ((vvvv_cmd_t*)(cmd))->v->redo = f; } while (0)
//#define vvvv_cmd_set_undo(cmd,f) do { ((vvvv_cmd_t*)(cmd))->v->undo = f; } while (0)
//#define vvvv_cmd_set_free(cmd,f) do { ((vvvv_cmd_t*)(cmd))->v->free = f; } while (0)

#endif /* CMD_H */
