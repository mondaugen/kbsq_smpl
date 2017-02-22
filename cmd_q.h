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
    /* The current "time" in history. */
    size_t cur_tag_time;
    /* The index of the first tag <= cur_tag_time. This value signed because the
     * current time could be less than the time of the first tag. In this case
     * this index will be -1. */
    long int cur_tag_idx;
    /* The current length of the tag list. Its maximum length is len, the same
     * as the command queue */
    size_t cur_tags_len;
    /* An array of tagged undo points in history */
    size_t *tags;
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
