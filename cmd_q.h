#ifndef CMD_Q_H
#define CMD_Q_H 

#include <stddef.h>
#include "cmd.h" 

typedef struct vvvv_cmd_tag_q_t {
    size_t len;
    /* The current length of the tag list. Its maximum length is len. */
    size_t cur_len;
    /* The index of the oldest item pushed onto the queue that has not yet been
     * popped. */
    size_t pop_index;
    /* The index where the next item to be pushed onto the queue will be pushed.
     * */
    size_t push_index;
    /* The index of the first tag <= cur_tag_time */
    long int cur_tag_idx;
    /* The current "time" in history. */
    size_t cur_tag_time;
    size_t *tags;
} vvvv_cmd_tag_q_t;

typedef struct vvvv_cmd_q_t {
    /* Physical length. The queue actually can only contain one less item than
     * this because there is one sentinel element. */
    size_t len;
    /* Index of the end of the queue, the beginning of the queue is 
     * (cur_end_idx + len + 1) % len */
    size_t cur_end_idx; 
    /* The index of the last command that was executed. */
    size_t lst_dn_idx;
    vvvv_cmd_tag_q_t tq;
    vvvv_cmd_t *cmds[];
} vvvv_cmd_q_t;

vvvv_cmd_q_t *
vvvv_cmd_q_new(size_t len);

vvvv_cmd_t *
vvvv_cmd_q_push_cmd(vvvv_cmd_q_t *cmdq, vvvv_cmd_t *cmd);

vvvv_err_t
vvvv_cmd_q_redo_next_cmd(vvvv_cmd_q_t *cmdq);

vvvv_err_t
vvvv_cmd_q_undo_cur_cmd(vvvv_cmd_q_t *cmdq);

#endif /* CMD_Q_H */
