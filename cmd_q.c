#include "cmd_q.h"

/* Removes the first command in the queue if it is not NULL.
 * The event is not freed (must be freed by the caller if it wants).
 */
static vvvv_cmd_t *
vvvv_cmd_q_rm_frst_cmd(vvvv_cmd_q_t *cmdq)
{
    vvvv_cmd_t *ret = NULL;
    /* First element is 2 away from the last index because there is a sentinel
     * in between */
    size_t i = (cmdq->cur_end_idx + 2) % cmdq->len;
    if (cmdq->cmds[i]) {
        ret = cmdq->cmds[i];
        cmdq->cmds[i] = NULL;
    }
    return ret;
}

/* Removes the last command in the queue if it is not the index of the last
 * command executed and decrements the index
 * of the end of the queue. The queue keeps no track of the command after this
 * point and it can be freed at will by the caller.
 */
static vvvv_cmd_t *
vvvv_cmd_q_rm_lst_cmd(vvvv_cmd_q_t *cmdq)
{
    if (cmdq->cur_end_idx == cmdq->lst_dn_idx) {
        return NULL;
    }
    vvvv_cmd_t *ret;
    ret = cmdq->cmds[cmdq->cur_end_idx];
    cmdq->cmds[cmdq->cur_end_idx] = NULL;
    cmdq->cur_end_idx = (cmdq->cur_end_idx + cmdq->len - 1) % cmdq->len;
    return ret;
}

/* Places command after the command at lst_dn_idx. While there are commands at
 * indices greater than lst_dn_idx and less than the beginning of the queue,
 * these commands are freed, as they will be "inacessible" after placing in the
 * new command. The command inserted is NOT executed, that must be done using
 * redo_next_cmd. The first command in the queue is returned.
 */
vvvv_cmd_t *
vvvv_cmd_q_push_cmd(vvvv_cmd_q_t *cmdq, const vvvv_cmd_t *cmd)
{
    vvvv_cmd_t *tmp_cmd;
    while (cmdq->cur_end_idx != cmdq->lst_dn_idx) {
        tmp_cmd = vvvv_cmd_q_rm_lst_cmd(cmdq);
        if (!tmp_cmd) {
            break;
        }
        vvvv_cmd_free(tmp_cmd);
    }
    tmp_cmd = vvvv_cmd_q_rm_frst_cmd(cmdq);
    size_t i = (cmdq->lst_dn_idx + 1) % cmdq->len;
    cmdq->cmds[i] = cmd;
    cmdq->cur_end_idx = (cmdq->cur_end_idx + 1) % cmdq->len;
    return tmp_cmd;
}

/* (Re)does the next command in the queue and increments the lst_dn_idx */
vvvv_err_t
vvvv_cmd_q_redo_next_cmd(vvvv_cmd_q_t *cmdq)
{
    vvvv_err_t ret;
    size_t i = (cmdq->lst_dn_idx + 1) % cmdq->len;
    if ((cmdq->cmds[i]) && (cmdq->lst_dn_idx != cmdq->cur_end_idx)) {
        ret = vvvv_cmd_redo(cmdq->cmds[i]);
        cmdq->lst_dn_idx = i;
        return ret;
    }
    /* Otherwise there are no more commands to do */
    return vvvv_err_DONE;
}

/* Undoes the most recent command in the queue and decrements the lst_dn_idx */
vvvv_err_t
vvvv_cmd_q_undo_cur_cmd(vvvv_cmd_q_t *cmdq)
{
    vvvv_err_t ret;
    if (cmdq->lst_dn_idx == ((cmdq->cur_end_idx + 1) % cmdq->len)) {
        /* No more commands to undo */
        return vvvv_err_DONE;
    }
    ret = vvvv_cmd_undo(cmdq->cmds[cmdq->lst_dn_idx]);
    cmdq->lst_dn_idx = (cmdq->lst_dn_idx + cmdq->len - 1) % cmdq->len;
    return ret;
}

/* Returns NULL if fails to allocate. Len must be at least 2. */
vvvv_cmd_q_t *
vvvv_cmd_q_new(size_t len)
{
    if (len < 2) {
        return NULL;
    }
    vvvv_cmd_q_t *ret;
    ret = (vvvv_cmd_q_t*)calloc(1, sizeof(vvvv_cmd_q_t) + (sizeof(vvvv_cmd_t*) * (len+1)));
    if (ret) {
        ret->len = len + 1;
        ret->cur_end_idx = 0;
        ret->lst_dn_idx = 1;
    }
    return ret;
}
