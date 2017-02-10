#include "nt_evnt_sq.h" 
#include "nt_evnt_cmd.h"
#include "cmd_q.h"

#define E_ALLOC(x) \
    if (!x) { \
        fprintf(stderr,"Error allocating, line: %d\n",__LINE__); \
        abort(); \
    }

#define  N_CMDS 5

int vvvv_nt_evnt_cmd_q_test(void)
{
    vvvv_nt_evnt_sq_t *nsq = NULL;
    vvvv_nt_evnt_sq_init_t nsq_init = {
        100, // Tick duration
        8,   // Length in ticks
        4,   // Number of tracks
    };
    nsq = vvvv_nt_evnt_sq_new(&nsq_init);
    E_ALLOC(nsq);
    vvvv_nt_evnt_cmd_init_t cmd_inits[] = {
        {
            0,          // track
            nsq,        // sequence
            {
                100,    // vvvv_tmstmp_t ts
                10,     // float pch
                20,     // float vel
                200     // vvvv_tmstmp_t dur
            }
        },
        {
            1,          // track
            nsq,        // sequence
            {
                150,    // vvvv_tmstmp_t ts
                11,     // float pch
                21,     // float vel
                100     // vvvv_tmstmp_t dur
            }
        },
        {
            0,          // track
            nsq,        // sequence
            {
                190,    // vvvv_tmstmp_t ts
                11,     // float pch
                22,     // float vel
                100     // vvvv_tmstmp_t dur
            }
        },
        {
            2,          // track
            nsq,        // sequence
            {
                400,    // vvvv_tmstmp_t ts
                11,     // float pch
                22,     // float vel
                100     // vvvv_tmstmp_t dur
            }
        },
        {
            0,          // track
            nsq,        // sequence
            {
                180,    // vvvv_tmstmp_t ts
                9,     // float pch
                22,     // float vel
                100     // vvvv_tmstmp_t dur
            }
        }
    };
    vvvv_nt_evnt_cmd_t *cmds[N_CMDS];
    size_t i;
    for (i = 0; i < N_CMDS; i++) {
        cmds[i] = vvvv_nt_evnt_cmd_new(&cmd_inits[i]);
        E_ALLOC(cmds[i]);
    }
    vvvv_cmd_q_t *cmd_q = NULL;
    cmd_q = vvvv_cmd_q_new(2);
    E_ALLOC(cmd_q);
    /* Command queue empty, should just say DONE */
    assert(vvvv_cmd_q_redo_next_cmd(cmd_q) == vvvv_err_DONE);
    assert(vvvv_cmd_q_undo_cur_cmd(cmd_q) == vvvv_err_DONE);
    assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)cmds[0]) == NULL);
    assert(vvvv_cmd_q_redo_next_cmd(cmd_q) == vvvv_err_NONE);
    /* Check inserted properly */
    assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,0,1)->lst_hd)
            == (MMDLList*)cmds[0]->nev);
    assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)cmds[1]) == NULL);
    assert(vvvv_cmd_q_redo_next_cmd(cmd_q) == vvvv_err_NONE);
    /* Check inserted properly */
    assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd)
            == (MMDLList*)cmds[1]->nev);
    /* This should be the first command as no more commands will fit */
    assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)cmds[2]) == (vvvv_cmd_t*)cmds[0]);
    assert(vvvv_cmd_q_redo_next_cmd(cmd_q) == vvvv_err_NONE);
    /* Check inserted properly */
    assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,0,1)->lst_hd)
            == (MMDLList*)cmds[2]->nev);
    assert(MMDLList_getNext(
                MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,0,1)->lst_hd))
            == (MMDLList*)cmds[0]->nev);
    /* Undo as many commands as possible */
    vvvv_err_t err;
    do {
        err = vvvv_cmd_q_undo_cur_cmd(cmd_q);
    } while (err == vvvv_err_NONE);
    /* The first command's event should be remaining */
    assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,0,1)->lst_hd)
            == (MMDLList*)cmds[0]->nev);
    /* The rest should be NULL */
    assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd)
            == NULL);
    assert(MMDLList_getNext(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,0,1)->lst_hd))
            == NULL);
    /* Redo as many commands as possible */
    do {
        err = vvvv_cmd_q_redo_next_cmd(cmd_q);
    } while (err == vvvv_err_NONE);
    /* Check inserted properly */
    assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,0,1)->lst_hd)
            == (MMDLList*)cmds[2]->nev);
    /* Check inserted properly */
    assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd)
            == (MMDLList*)cmds[1]->nev);
    /* Check inserted properly */
    assert(MMDLList_getNext(
                MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,0,1)->lst_hd))
            == (MMDLList*)cmds[0]->nev);
    /* Undo as many commands as possible */
    do {
        err = vvvv_cmd_q_undo_cur_cmd(cmd_q);
    } while (err == vvvv_err_NONE);
    assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)cmds[3]) == NULL);
    assert(vvvv_cmd_q_redo_next_cmd(cmd_q) == vvvv_err_NONE);
    /* Check inserted properly */
    assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,2,4)->lst_hd)
            == (MMDLList*)cmds[3]->nev);
    assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)cmds[4]) == NULL);
    assert(vvvv_cmd_q_redo_next_cmd(cmd_q) == vvvv_err_NONE);
    /* Check inserted properly */
    assert(MMDLList_getNext(
                MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,0,1)->lst_hd))
            == (MMDLList*)cmds[4]->nev);
    return 0;
}

int main (void)
{
    (void) vvvv_nt_evnt_cmd_q_test();
    return 0;
}
