#ifndef CMD_Q_H
#define CMD_Q_H 

#include <stddef.h>
#include "cmd.h" 

/* How the command queue works:
 *
 * The commmand queue is an array of pointers to commands.
 * Its length is fixed.
 * There is one sentinel pointer (always NULL) between the cur_end_idx and the
 * beginning of the queue. The beginning of the queue will only not be NULL when
 * the queue is full, otherwise it is NULL and it means a new command can be
 * pushed onto the queue by placing it after cur_end_idx and increasing
 * cur_end_idx.
 * New commands are placed after the last command that was executed i.e., the
 * command at lst_dn_idx. If there are commands ahead of this one, they are
 * freed because they can no longer be redone. After this cur_end_idx is set to
 * lst_dn_idx. Then the command is put at cur_end_idx + 1 and the cur_end_idx is
 * incremented.
 * If there is no more room in the queue to push in a command, the first command
 * in the queue (the one at cur_end_idx + 2) is freed and set to NULL (it can no
 * longer be undone) and the command is placed after cur_end_idx, and
 * cur_end_idx is incremented.
 */

typedef struct vvvv_cmd_q_t {
    /* Physical length. The queue actually can only contain one less item than
     * this because there is one sentinel element. */
    size_t len;
    /* Index of the end of the queue, the beginning of the queue is 
     * (cur_end_idx + len + 2) % len because of the one sentinel element */
    size_t cur_end_idx; 
    /* The index of the last command that was executed. */
    size_t lst_dn_idx;
    vvvv_cmd_t *cmds[];
} vvvv_cmd_q_t;

vvvv_cmd_q_t *
vvvv_cmd_q_new(size_t len);

void vvvv_cmd_q_free(vvvv_cmd_q_t *cmdq);

vvvv_cmd_t *
vvvv_cmd_q_push_cmd(vvvv_cmd_q_t *cmdq, vvvv_cmd_t *cmd);

vvvv_err_t
vvvv_cmd_q_redo_next_cmd(vvvv_cmd_q_t *cmdq);

vvvv_err_t
vvvv_cmd_q_undo_cur_cmd(vvvv_cmd_q_t *cmdq);

#endif /* CMD_Q_H */
