#include "cmd_q.h"
#include <stdlib.h> 

#define CMD_TAG_Q_PREV_IDX(cq,i) \
    ((i + cq->tq.len - 1) % cq->tq.len)

#define CMD_TAG_Q_NEXT_IDX(cq,i) \
    ((i + 1) % cq->tq.len)

static vvvv_err_t
vvvv_cmd_q_undo_n_cmds(vvvv_cmd_q_t *cmdq, size_t *n);
vvvv_err_t
vvvv_cmd_q_redo_n_cmds(vvvv_cmd_q_t *cmdq, size_t *n);

/* Pops earliest tag */
static vvvv_err_t cmd_tag_q_pop(vvvv_cmd_tag_q_t *ctq, size_t *rslt)
{
    if (ctq->cur_len == 0) {
        return vvvv_err_EMPTY;
    }
    *rslt = ctq->tags[ctq->pop_index];
    ctq->pop_index = (ctq->pop_index + 1) % ctq->len;
    ctq->cur_len--;
    return vvvv_err_NONE;
}

/* Pushes new tag */
static vvvv_err_t cmd_tag_q_push(vvvv_cmd_tag_q_t *ctq, size_t item)
{
    if (ctq->cur_len == ctq->len) {
        return vvvv_err_EFULL;
    }
    ctq->tags[ctq->push_index] = item;
    ctq->push_index = (ctq->push_index + 1) % ctq->len;
    ctq->cur_len++;
    return vvvv_err_NONE;
}

/* Pops latest tag, that is, pops from same end of queue as was pushed to. */
static vvvv_err_t cmd_tag_q_pop_end(vvvv_cmd_tag_q_t *ctq, size_t *rslt)
{
    if (ctq->cur_len == 0) {
        return vvvv_err_EMPTY;
    }
    ctq->push_index = (ctq->push_index + ctq->len - 1) % ctq->len;
    *rslt = ctq->tags[ctq->push_index];
    ctq->cur_len--;
    return vvvv_err_NONE;
}

/* Creates a tag at the current time */
static vvvv_err_t cmd_tag_q_make_tag(vvvv_cmd_tag_q_t *ctq)
{
    size_t rslt;
    /* Pop off tags whose time is later than the current time */
    while (ctq->cur_len
            && (ctq->cur_tag_time 
                < ctq->tags[(ctq->push_index + ctq->len - 1)
                    % ctq->len])) {
        (void) cmd_tag_q_pop_end(ctq,&rslt);
    }
    /* Add current time as tag */
    return cmd_tag_q_push(ctq,ctq->cur_tag_time);
}

vvvv_err_t vvvv_cmq_q_undo_to_prev_tag(vvvv_cmd_q_t *cq)
{
    if (cq->tq.cur_len == 0) {
        return vvvv_err_EMPTY;
    }
    if (cq->tq.prev_tag_idx
            == CMD_TAG_Q_PREV_IDX(cq,cq->tq.pop_index)) {
        return vvvv_err_EBNDS; /* No more tags to undo to */
    }
    size_t n_undos = cq->tq.cur_tag_time
                        - cq->tq.tags[cq->tq.prev_tag_idx];
    (void) vvvv_cmd_q_undo_n_cmds(cq,&n_undos);
    if (n_undos) {
        /* If not all the undos completed successfully, return FAIL. */
        return vvvv_err_FAIL;
    }
    return vvvv_err_NONE;
}

vvvv_err_t vvvv_cmd_q_redo_to_next_tag(vvvv_cmd_q_t *cq)
{
    if (cq->tq.cur_len == 0) {
        return vvvv_err_EMPTY;
    }
    if (cq->tq.prev_tag_idx 
                == CMD_TAG_Q_PREV_IDX(cq,cq->tq.push_index)) {
        return vvvv_err_EBNDS; /* No more tags to redo to */
    }
    size_t n_redos = cq->tq.tags[cq->tq.prev_tag_idx + 1]
                        - cq->tq.cur_tag_time;
    vvvv_cmd_q_redo_n_cmds(cq,&n_redos);
    if (n_redos) {
        return vvvv_err_FAIL; /* Didn't do all the redos sucessfully */
    }
    return vvvv_err_NONE;
}

/* Increment or decrement the tag time.
 * If dir > 0, increments the tag time by 1
 * if dir < 0, decrements the tag time by 1
 * otherwise does nothing.
 * If the cur_tag_time is 0 and dir negative, does nothing.
 * If cur_tag_time is very large, the cur_tag_time might rollover but this
 * should never happen in practice. If it does a fatal error is thrown.
 */
static void cmd_q_advance_tag_time(vvvv_cmd_q_t *cmdq, int dir)
{
    if (dir < 0) {
        if (cmdq->tq.cur_tag_time == 0) {
            return;
        }
        cmdq->tq.cur_tag_time--;
        if ((cmdq->tq.cur_len == 0)
            || (cmdq->tq.prev_tag_idx
                == cmdq->tq.pop_index)) {
            /* If there are no tags in the tag queue or the prev_tag_idx is the
             * same as the index of the oldest tag, we cannot decrement the
             * prev_tag_idx. Don't adjust prev_tag_idx. */
            return;
        }
        if (cmdq->tq.tags[cmdq->tq.prev_tag_idx]
                == cmdq->tq.cur_tag_time) {
            /* If the cur_tag_time has been decremented to a point where it is
             * the same as the tag time at prev_tag_idx then prev_tag_idx is no
             * longer the index of the previous tag and so it must be
             * decremented. */
            cmdq->tq.prev_tag_idx = CMD_TAG_Q_PREV_IDX(cmdq,
                    cmdq->tq.prev_tag_idx);
        }
    } else if (dir > 0) {
        if (cmdq->tq.cur_tag_time == SIZE_MAX) {
            /* Incrementing the cur_tag_time would result in overflow. */
            /* TODO: Add cleanup function */
            vvvv_err_fatal("Command queue overflow.",NULL,NULL);
            return;
        }
        cmdq->tq.cur_tag_time++;
        while ((cmdq->tq.cur_len > 0) && ((cmdq->tq.cur_tag_time 
                    - cmdq->tq.tags[cmdq->tq.pop_index]) 
                > cmdq->len)) {
            /* Remove all tags so old that not enough undos can be stored to
             * undo to them */
            size_t discard;
            (void) cmd_tag_q_pop(&cmdq->tq,&discard);
        }
        if ((cmdq->tq.cur_len == 0) || (cmdq->tq.prev_tag_idx
                == CMD_TAG_Q_PREV_IDX(cmdq,cmdq->tq.push_index))) {
            /* If there are no tags in the tag queue or the previous tag index
             * is the same as the index of the last pushed tag then we cannot
             * increment the prev_tag_idx. Don't adjust prev_tag_idx. */
            return;
        }
        if ((cmdq->tq.tags[CMD_TAG_Q_NEXT_IDX(cmdq,cmdq->tq.prev_tag_idx)] + 1)
                == cmdq->tq.cur_tag_time) {
            /* If the cur_tag_time has been incremented to a point where it is
             * equal to the time of the tag after prev_tag_idx plus 1, then
             * prev_tag_idx is no longer the previous tag and so it needs to be
             * incremented. */
            cmdq->tq.prev_tag_idx = CMD_TAG_Q_NEXT_IDX(cmdq,
                    cmdq->tq.prev_tag_idx);
        }
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
 * redo_next_cmd.
 * The first command in the queue is returned, this command
 * could be NULL unless the queue was full before pushing a command at the end
 * of the queue, in which case the oldest command is returned. If this command
 * is no longer needed, it should be freed.
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
     * jumped to. If prev_tag_idx is -1, cur_tags_len becomes 0. */
    while ((cmdq->tq.cur_len > 0)
            && (cmdq->tq.tags[CMD_TAG_Q_PREV_IDX(cmdq,cmdq->tq.push_index)]
                > cmdq->tq.cur_tag_time)) {
        size_t discard;
        /* There is no need to adjust the prev_tag_idx because it should always
         * refer to a tag whose time is less than cur_tag_time */
        (void) cmd_tag_q_pop_end(&cmdq->tq,&discard);
    }
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
        cmd_q_advance_tag_time(cmdq,1);
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
    cmd_q_advance_tag_time(cmdq,-1);
    return ret;
}

/* Undoes commands until error. n will contain the number of undos that weren't
 * done */
static vvvv_err_t
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
        size_t *tags = (size_t*)malloc(sizeof(size_t) * (len + 2));
        if (!tags) {
            free(ret);
            return NULL;
        }
        ret->len = len + 1;
        ret->cur_end_idx = 0;
        ret->lst_dn_idx = 1;
        ret->tq.len = len;
        ret->tq.cur_len = 0;
        ret->tq.pop_index = 0;
        ret->tq.push_index = 0;
        ret->tq.prev_tag_idx = 0;
        ret->tq.cur_tag_time = 0;
        ret->tq.tags = tags;
    }
    return ret;
}
