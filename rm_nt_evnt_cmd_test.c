#include "test.h" 
#include "rm_nt_evnt_cmd.h"
#include "nt_evnt_sq.h" 
#include "nt_evnt_cmd.h"
#include "cmd_q.h"

#define E_ALLOC(x) \
    if (!x) { \
        fprintf(stderr,"Error allocating, line: %d\n",__LINE__); \
        abort(); \
    }

#define  N_CMDS 5

int vvvv_rm_nt_evnt_cmd_test(void)
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
            1,          // track
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
            1,          // track
            nsq,        // sequence
            {
                180,    // vvvv_tmstmp_t ts
                9,      // float pch
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
    vvvv_rm_nt_evnt_cmd_t *rm_cmds[N_CMDS];
    for (i = 0; i < N_CMDS; i++) {
        rm_cmds[i] = vvvv_rm_nt_evnt_cmd_new(&cmd_inits[i]);
        E_ALLOC(rm_cmds[i]);
    }
    vvvv_cmd_q_t *cmd_q = NULL;
    cmd_q = vvvv_cmd_q_new(2);
    E_ALLOC(cmd_q);
    VVVV_TEST_UNIT_START("No note should be found, but command should be done");
    assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)rm_cmds[0]) == NULL);
    vvvv_cmd_q_redo_next_cmd(cmd_q);
    assert(vvvv_cmd_get_dn(rm_cmds[0]) == vvvv_cmd_done_TRUE);
    assert(rm_cmds[0]->fnd_nev == NULL);
    VVVV_TEST_UNIT_RESULT(1);
    VVVV_TEST_UNIT_START("Insert first note, then remove");
    vvvv_cmd_q_undo_cur_cmd(cmd_q);
    assert(vvvv_cmd_get_dn(rm_cmds[0]) == vvvv_cmd_done_FALSE);
    VVVV_TEST_UNIT_RESULT(1);
    {
        VVVV_TEST_UNIT_START("Check can remove added event");
        vvvv_nt_evnt_t *tmp_nev = cmds[0]->nev;
        assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)cmds[0]) == NULL);
        vvvv_cmd_q_redo_next_cmd(cmd_q);
        /* Check inserted properly */
        assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,0,1)->lst_hd)
            == (MMDLList*)cmds[0]->nev);
        assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)rm_cmds[0]) == NULL);
        vvvv_cmd_q_redo_next_cmd(cmd_q);
        /* Check that both were done */
        assert(vvvv_cmd_get_dn(rm_cmds[0]) == vvvv_cmd_done_TRUE);
        assert(vvvv_cmd_get_dn(cmds[0]) == vvvv_cmd_done_TRUE);
        /* Check removed event is the one the first command put in */
        assert(rm_cmds[0]->fnd_nev == tmp_nev);
        /* Also check event was actually removed */
        assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,0,1)->lst_hd) == NULL);
        VVVV_TEST_UNIT_RESULT(1);
    }
    {
        VVVV_TEST_UNIT_START("Check that timestamp must match exactly");
        vvvv_nt_evnt_t *tmp_nev = cmds[1]->nev;
        assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)cmds[1]) == (vvvv_cmd_t*)cmds[0]);
        vvvv_cmd_q_redo_next_cmd(cmd_q);
        /* Check inserted properly */
        assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd)
            == (MMDLList*)cmds[1]->nev);
        /* Push removal command with same pitch and track, slightly different
         * time stamp (same index in sequence) */
        assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)rm_cmds[2]) == (vvvv_cmd_t*)rm_cmds[0]);
        vvvv_cmd_q_redo_next_cmd(cmd_q);
        /* Check that both were done */
        assert(vvvv_cmd_get_dn(rm_cmds[2]) == vvvv_cmd_done_TRUE);
        assert(vvvv_cmd_get_dn(cmds[1]) == vvvv_cmd_done_TRUE);
        /* Check removed event is NULL (not found) */
        assert(rm_cmds[2]->fnd_nev == NULL);
        /* Check event is still there */
        assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd) == (MMDLList*)tmp_nev);
        VVVV_TEST_UNIT_RESULT(1);
    }
    {
        VVVV_TEST_UNIT_START("Check that it works when there is more than 1 event at a time slot");
        vvvv_nt_evnt_t *tmp_nev = cmds[4]->nev;
        assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)cmds[4]) == (vvvv_cmd_t*)cmds[1]);
        vvvv_cmd_q_redo_next_cmd(cmd_q);
        /* Check inserted properly */
        assert(MMDLList_getNext(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd))
            == (MMDLList*)cmds[4]->nev);
        /* Push removal command with all the same paramters (should remove
         * event) */
        assert(vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)rm_cmds[4]) == (vvvv_cmd_t*)rm_cmds[2]);
        vvvv_cmd_q_redo_next_cmd(cmd_q);
        /* Check that both were done */
        assert(vvvv_cmd_get_dn(rm_cmds[4]) == vvvv_cmd_done_TRUE);
        assert(vvvv_cmd_get_dn(cmds[4]) == vvvv_cmd_done_TRUE);
        /* Check removed event is the one we want */
        assert(rm_cmds[4]->fnd_nev == tmp_nev);
        /* Check event removed */
        assert(MMDLList_getNext(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd)) == NULL);
        /* Check other event is still there */
        assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd) == (MMDLList*)cmds[1]->nev);
        /* Check that undoing the event puts it back in */
        vvvv_cmd_q_undo_cur_cmd(cmd_q);
        assert(MMDLList_getNext(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd))
            == (MMDLList*)cmds[4]->nev);
        /* Check that undoing the other event takes it out again */
        vvvv_cmd_q_undo_cur_cmd(cmd_q);
        /* Check event removed */
        assert(MMDLList_getNext(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd)) == NULL);
        /* Check other event is still there */
        assert(MMDLList_getNext(&vvvv_nt_evnt_sq_get_evnt_lst(nsq,1,1)->lst_hd) == (MMDLList*)cmds[1]->nev);
        VVVV_TEST_UNIT_RESULT(1);
    }


    return 0;
}

int main (void)
{
    (void) vvvv_rm_nt_evnt_cmd_test();
    return 0;
}
