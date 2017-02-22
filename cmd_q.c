#include "cmd_q.h"
#include <stdlib.h> 

/* Jump the tag time by dist, adjusting the cur_tag_idx. Negative dist for
 * undos, positive dist for redos. */
static void cmd_q_jump_tag_time(vvvv_cmd_q_t *cmdq, long int dist)
{
    if ((-1*dist) > cmdq->cur_tag_time) {
        cmdq->cur_tag_time = 0;
    } else {
        cmdq->cur_tag_time += dist;
    }
    while ((cmdq->cur_tag_idx > -1)
            && (cmdq->tags[cmdq->cur_tag_idx] > cmdq->cur_tag_time)) {
        cmdq->cur_tag_idx--;
    }
}

/* Removes the first command in the queue if it is not NULL.
 * The event is not freed (must be freed by the caller if it wants).
 */
static vvvv_cmd_t *
cmd_q_rm_frst_cmd(vvvv_cmd_q_t *cmdq)
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
cmd_q_rm_lst_cmd(vvvv_cmd_q_t *cmdq)
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
vvvv_cmd_q_push_cmd(vvvv_cmd_q_t *cmdq, vvvv_cmd_t *cmd)
{
    vvvv_cmd_t *tmp_cmd;
    /* Remove all commands that were "undone" as they will never be able to be
     * redone now */
    while (cmdq->cur_end_idx != cmdq->lst_dn_idx) {
        tmp_cmd = cmd_q_rm_lst_cmd(cmdq);
        if (!tmp_cmd) {
            break;
        }
        vvvv_cmd_free(tmp_cmd);
    }
    /* Remove all tags ahead of the current time, because now they can never be
     * jumped to. If cur_tag_idx is -1, cur_tags_len becomes 0. */
    cmdq->cur_tags_len = cmdq->cur_tag_idx + 1;
    tmp_cmd = cmd_q_rm_frst_cmd(cmdq);
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
        /* Increment tag time */
        cmd_q_jump_tag_time(cmdq,1);
        return ret;
    }
    /* Otherwise there are no more commands to do */
    return vvvv_err_DONE;
}

/* Redoes n commands until error. n will contain the number of redos that weren't
 * done */
vvvv_err_t
vvvv_cmd_q_redo_n_cmds(vvvv_cmd_q_t *cmdq, size_t *n)
{
    vvvv_err_t err;
    while (*n > 0) {
        if ((err = vvvv_cmd_q_redo_next_cmd(cmdq)) != vvvv_err_NONE) {
            return err;
        }
        n--;
    }
    return err;
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
    /* Decrement tag time */
    cmd_q_jump_tag_time(cmdq,-1);
    return ret;
}

/* Undoes commands until error. n will contain the number of undos that weren't
 * done */
vvvv_err_t
vvvv_cmd_q_undo_n_cmds(vvvv_cmd_q_t *cmdq, size_t *n)
{
    vvvv_err_t err;
    while (*n > 0) {
        if ((err = vvvv_cmd_q_undo_cur_cmd(cmdq)) != vvvv_err_NONE) {
            return err;
        }
        n--;
    }
    return err;
}

/* Jump to the time in history at the tag index idx.
 * If the time in history is greater than the current time, commands will be
 * redone until it is attained, otherwise they will be undone. If the time in
 * history and the current time are equal, no commands are undone or redone and
 * vvvv_err_NONE is returned.
 */
vvvv_err_t
vvvv_cmd_q_jump_tag_idx(vvvv_cmd_q_t *cmdq, long int idx)
{
    long int dist;
    if ((idx < 0) || (idx >= cmdq->cur_tags_len)) {
        return vvvv_err_EBNDS;
    }
    if ((dist = cmdq->tags[idx] - cmdq->cur_tag_time) < 0) {
        return vvvv_cmd_q_undo_n_cmds(cmdq,-1*dist);
    } else if ((dist = cmdq->tags[idx] - cmdq->cur_tag_time) > 0) {
        return vvvv_cmd_q_redo_n_cmds(cmdq,dist);
    }
    return vvvv_err_NONE;
}

/* If the current time is equal to the current time, jump to the previous tag,
 * otherwise jump to the current tag (it will be the first tag less than the
 * current time).
 */
vvvv_err_t
vvvv_cmd_q_jump_prev_tag(vvvv_cmd_q_t *cmdq)
{
    if (cmdq->cur_tag_idx < 0) {
        vvvv_err_DONE; /* No more tags previous to jump to. */
    }
    if (cmdq->tags[cmdq->cur_tag_idx] == cmdq->cur_tag_time) {
        return vvvv_cmd_q_jump_tag_idx(cmdq,cmdq->cur_tag_idx - 1);
    } 
    return vvvv_cmd_q_jump_tag_idx(cmdq,cmdq->cur_tag_idx);
}

/* Jump to the next tag. */
vvvv_err_t
vvvv_cmd_q_jump_next_tag(vvvv_cmd_q_t *cmdq)
{
    return vvvv_cmd_q_jump_tag_idx(cmdq,cmdq->cur_tag_idx+1);
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
        /* In the worst case, each command in the queue would be tagged, so the
         * maximum number of tags is the same as the length of the queue. */
        size_t *tags = (size_t*)malloc(sizeof(size_t) * (len + 1));
        if (!tags) {
            free(ret);
            return NULL;
        }
        ret->len = len + 1;
        ret->cur_end_idx = 0;
        ret->lst_dn_idx = 1;
        ret->cur_tag_time = 0;
        ret->cur_tag_idx = -1;
        ret->cur_tags_len = 0;
    }
    return ret;
}
