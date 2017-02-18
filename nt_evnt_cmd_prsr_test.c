#include <stdio.h> 
#include <assert.h> 
#include "test.h"
#include "cmd_prsr.h" 
#include "nt_evnt_cmd_prsr.h"
#include "nt_evnt_sq.h"
#include "nt_evnt.h" 
#include "cmd_q.h"
#include "nt_evnt_cmd.h"
#include "rm_nt_evnt_cmd.h" 

#define E_ALLOC(x) \
    if (!x) { \
        fprintf(stderr,"Error allocating, line: %d\n",__LINE__); \
        abort(); \
    }

int vvvv_nt_evnt_cmd_prsr_test(void)
{
    VVVV_TEST_UNIT_START("Insert notes using string command, check if correct results.");
    vvvv_nt_evnt_sq_t *nsq = NULL;
    vvvv_nt_evnt_sq_init_t nsq_init = {
        100, // Tick duration
        8,   // Length in ticks
        4,   // Number of tracks
    };
    nsq = vvvv_nt_evnt_sq_new(&nsq_init);
    E_ALLOC(nsq);
    vvvv_cmd_q_t *cmd_q = NULL;
    cmd_q = vvvv_cmd_q_new(2);
    E_ALLOC(cmd_q);
    char *strs[] = {
        "nte 1 2 3",
        "note",
        "note 1",
        "note 1 200",
        "note 1 200 67",
        "note 1 200 67 12",
        "note 1 200 67 12 50",
        "note 1 200 haha"
    };
    char strbuf[100];
    vvvv_nt_evnt_cmd_init_t necis[] = {
        {
            NT_EVNT_CMD_PRSR_DFLT_TRK,
                nsq,
                {
                    NT_EVNT_CMD_PRSR_DFLT_TS,
                    NT_EVNT_CMD_PRSR_DFLT_PCH,
                    NT_EVNT_CMD_PRSR_DFLT_VEL,
                    NT_EVNT_CMD_PRSR_DFLT_DUR
                }
        },
            {
                1,
                nsq,
                {
                    NT_EVNT_CMD_PRSR_DFLT_TS,
                    NT_EVNT_CMD_PRSR_DFLT_PCH,
                    NT_EVNT_CMD_PRSR_DFLT_VEL,
                    NT_EVNT_CMD_PRSR_DFLT_DUR
                }
            },
            {
                1,
                nsq,
                {
                    200,
                    NT_EVNT_CMD_PRSR_DFLT_PCH,
                    NT_EVNT_CMD_PRSR_DFLT_VEL,
                    NT_EVNT_CMD_PRSR_DFLT_DUR
                }
            },
            {
                1,
                nsq,
                {
                    200,
                    67,
                    NT_EVNT_CMD_PRSR_DFLT_VEL,
                    NT_EVNT_CMD_PRSR_DFLT_DUR
                }
            },
            {
                1,
                nsq,
                {
                    200,
                    67,
                    12,
                    NT_EVNT_CMD_PRSR_DFLT_DUR
                }
            },
            {
                1,
                nsq,
                {
                    200,
                    67,
                    12,
                    50
                }
            },
            {
                1,
                nsq,
                {
                    200,
                    NT_EVNT_CMD_PRSR_DFLT_PCH,
                    NT_EVNT_CMD_PRSR_DFLT_VEL,
                    NT_EVNT_CMD_PRSR_DFLT_DUR
                }
            }
    };
    vvvv_nt_evnt_cmd_prsr_init_t necpi = {
        &nsq,
        &cmd_q
    };
    vvvv_nt_evnt_cmd_prsr_t necp;
    vvvv_nt_evnt_cmd_prsr_init(&necp,&necpi);
    vvvv_cmd_prsr_add((vvvv_cmd_prsr_t*)&necp);
    /* There should be no events in any track event after executing first string
     * command */
    strcpy(strbuf,strs[0]);
    assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NMTCH);
    size_t n;
    for (n = 0; n < nsq_init.n_trks; n++)
    {
        size_t num_evnts;
        vvvv_nt_evnt_sq_rng_t nesr = {n,0,800,0,128};
        num_evnts = vvvv_nt_evnt_sq_gt_nm_evnts_rng(nsq,&nesr);
        assert(num_evnts == 0);
    }
    /* Execute rest of commands and see if the results match what is
     * suspected */
    for (n = 1; n < (sizeof(strs)/sizeof(char*)); n++) {
        strcpy(strbuf,strs[n]);
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
        /* Find event (should be only one) */
        vvvv_nt_evnt_sq_rng_t nesr = {necis[n-1].trk,
            0,
            nsq_init.tick_dur * nsq_init.n_tcks,
            0,
            128};
        vvvv_nt_evnt_t **evnts;
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        assert(evnts != NULL);
        assert(evnts[1] == NULL);
        vvvv_nt_evnt_t nev;
        vvvv_nt_evnt_init(&nev,&necis[n-1].nei);
        /* Check to see they are the same */
        assert(vvvv_nt_evnt_cmp(evnts[0],&nev) == 0);
        /* Free evnts array */
        free(evnts);
        /* Remove event */
        vvvv_rm_nt_evnt_cmd_t *rm_cmd;
        rm_cmd = vvvv_rm_nt_evnt_cmd_new(&necis[n-1]);
        vvvv_cmd_q_push_cmd(cmd_q,(vvvv_cmd_t*)rm_cmd);
        vvvv_cmd_q_redo_next_cmd(cmd_q);
    }
    VVVV_TEST_UNIT_RESULT(1);
    return 0;
}

int main (void)
{
    (void) vvvv_nt_evnt_cmd_prsr_test();
    return 0;
}
