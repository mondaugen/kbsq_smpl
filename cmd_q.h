#ifndef CMD_Q_H
#define CMD_Q_H 

#include <stddef.h>
#include "cmd.h" 

typedef struct vvvv_cmd_q_t {
    /* Physical length. The queue actually can only contain one less item than
     * this because there is one sentinel element. */
    size_t len;
    /* Index of the end of the queue, the beginning of the queue is 
     * (cur_end_idx + len + 1) % len */
    size_t cur_end_idx; 
    /* The index of the last command that was executed. */
    size_t lst_dn_idx;
    vvvv_cmd_t *cmds[];
} vvvv_cmd_q_t;

#endif /* CMD_Q_H */
