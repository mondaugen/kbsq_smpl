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

static int vvvv_nt_evnt_cmd_rm_cmd_prsr_test(void)
{
    VVVV_TEST_UNIT_START("Insert and remove notes using string command, check if correct results.");
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
    char *rm_strs[] = {
        "ram 1 2 3",
        "rm",
        "rm 1",
        "rm 1 200",
        "rm 1 200 67",
        "rm 1 200 67 12",
        "rm 1 200 67 12 50",
        "rm 1 200 haha"
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
        strcpy(strbuf,rm_strs[n]);
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
    }
    vvvv_nt_evnt_sq_free(nsq);
    vvvv_cmd_q_free(cmd_q);
    VVVV_TEST_UNIT_RESULT(1);
    return 0;
}

/* Insert and remove multiple events */
static int add_rm_mult_test(void)
{
    VVVV_TEST_UNIT_START("Insert and remove multiple notes using string commands, check if correct results.");
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
    vvvv_nt_evnt_sq_rng_t nesr = {1,
        0,
        nsq_init.tick_dur * nsq_init.n_tcks,
        0,
        128};
    char *strs[] = {
        "note 1 100 35 100 24",
        "note 1 130 35 100 25",
        "rm 1 130 35 100 25",
        "note 2 200 67 90 40",
        "note 2 200 65 90 40",
        "rm 2 200 67",
    };
    /* Add first 2 notes */
    int n;
    for (n = 0; n < 2; n++) {
        strcpy(strbuf,strs[n])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
    }
    {
    /* Get events */
        vvvv_nt_evnt_t **evnts;
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        /* Ensure there are 2 */
        assert(evnts != NULL);
        assert(evnts[0] != NULL);
        assert(evnts[1] != NULL);
        assert(evnts[2] == NULL);
        free(evnts);
    }
    {
        /* Remove one, ensure there's only one and it's the proper remaining one
         * */
        strcpy(strbuf,strs[2])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
        /* Get events and ensure there's only one remaining */
        vvvv_nt_evnt_t **evnts;
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        assert(evnts != NULL);
        assert(evnts[1] == NULL);
        assert(evnts[0]->pch == 35);
        assert(evnts[0]->ts == 100);
    }
    /* Do the same, but with pitch */
    for (n = 3; n < 5; n++) {
        strcpy(strbuf,strs[n])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
    }
    {
        /* Remove one, ensure there's only one and it's the proper remaining one
         * */
        strcpy(strbuf,strs[5])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
        /* Get events and ensure there's only one remaining */
        vvvv_nt_evnt_t **evnts;
        nesr.trk = 2;
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        assert(evnts != NULL);
        assert(evnts[1] == NULL);
        assert(evnts[0]->pch == 65);
        assert(evnts[0]->ts == 200);
    }
    vvvv_nt_evnt_sq_free(nsq);
    vvvv_cmd_q_free(cmd_q);
    VVVV_TEST_UNIT_RESULT(1);
    return 0;
}

/* Insert and remove multiple events and test undo and redo */
static int add_rm_mult_undo_redo_test(void)
{
    VVVV_TEST_UNIT_START("Insert and remove multiple notes using string commands, call undo and redo, check if correct results.");
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
    vvvv_nt_evnt_sq_rng_t nesr = {1,
        0,
        nsq_init.tick_dur * nsq_init.n_tcks,
        0,
        128};
    char *strs[] = {
        "note 1 100 35 100 24",
        "note 1 130 35 100 25",
        "rm 1 130 35 100 25",
        "undo",
        "redo",
        "note 2 200 67 90 40",
        "note 2 200 65 90 40",
        "rm 2 200 67",
        "undo",
        "redo",
    };
    /* Add first 2 notes */
    int n;
    for (n = 0; n < 2; n++) {
        strcpy(strbuf,strs[n])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
    }
    {
    /* Get events */
        vvvv_nt_evnt_t **evnts;
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        /* Ensure there are 2 */
        assert(evnts != NULL);
        assert(evnts[0] != NULL);
        assert(evnts[1] != NULL);
        assert(evnts[2] == NULL);
        free(evnts);
    }
    {
        /* Remove one, ensure there's only one and it's the proper remaining one
         * */
        strcpy(strbuf,strs[2])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
        /* Get events and ensure there's only one remaining */
        vvvv_nt_evnt_t **evnts;
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        assert(evnts != NULL);
        assert(evnts[1] == NULL);
        assert(evnts[0]->pch == 35);
        assert(evnts[0]->ts == 100);
        free(evnts);
        /* Undo */
        strcpy(strbuf,strs[3])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
        /* Get events and ensure there are again 2 */
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        assert(evnts != NULL);
        assert(evnts[2] == NULL);
        assert(evnts[0]->pch == 35);
        assert(evnts[1]->pch == 35);
        /* Ensure they are both there */
        vvvv_tmstmp_t tss[] = {evnts[0]->ts,evnts[1]->ts};
        if (tss[0] > tss[1]) {
            vvvv_tmstmp_t tmp = tss[0];
            tss[0] = tss[1];
            tss[1] = tmp;
        }
        assert(tss[0] == 100);
        assert(tss[1] == 130);
        free(evnts);
        strcpy(strbuf,strs[4])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
        /* Get events and ensure there's only one remaining */
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        assert(evnts != NULL);
        assert(evnts[1] == NULL);
        assert(evnts[0]->pch == 35);
        assert(evnts[0]->ts == 100);
        free(evnts);
    }
    /* Do the same, but with pitch */
    for (n = 5; n < 7; n++) {
        strcpy(strbuf,strs[n])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
    }
    {
        /* Remove one, ensure there's only one and it's the proper remaining one
         * */
        strcpy(strbuf,strs[7])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
        /* Get events and ensure there's only one remaining */
        vvvv_nt_evnt_t **evnts;
        nesr.trk = 2;
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        assert(evnts != NULL);
        assert(evnts[1] == NULL);
        assert(evnts[0]->pch == 65);
        assert(evnts[0]->ts == 200);
        free(evnts);
        /* Undo */
        strcpy(strbuf,strs[8])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        assert(evnts != NULL);
        assert(evnts[2] == NULL);
        assert(evnts[0]->pch == 67);
        assert(evnts[0]->ts == 200);
        assert(evnts[1]->pch == 65);
        assert(evnts[1]->ts == 200);
        free(evnts);
        /* redo */
        strcpy(strbuf,strs[9])
        assert(vvvv_cmd_prsr_prs(strbuf) == vvvv_err_NONE);
        evnts = vvvv_nt_evnt_sq_gt_evnts_rng(nsq,&nesr);
        assert(evnts != NULL);
        assert(evnts[1] == NULL);
        assert(evnts[0]->pch == 65);
        assert(evnts[0]->ts == 200);
        free(evnts);
    }
    vvvv_nt_evnt_sq_free(nsq);
    vvvv_cmd_q_free(cmd_q);
    VVVV_TEST_UNIT_RESULT(1);
    return 0;
}

int main (void)
{
    (void) vvvv_nt_evnt_cmd_rm_cmd_prsr_test();
    (void) add_rm_mult_test();
    return 0;
}
