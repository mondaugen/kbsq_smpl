static vvvv_cmd_prsr_t *prsrs = NULL;

/* Add a command parser. Note that if its cmd_tkn is the same as the cmd_tkn of
 * a parser already in this list, it will mask the old parser without warning.
 */
void vvvv_cmd_prsr_add(vvvv_cmd_prsr_t *prsr)
{
    prsr->next = prsrs;
    prsrs = prsr;
}

vvvv_err_t vvvv_cmd_prsr_prs(char *str)
{
    vvvv_cmd_prsr_t cp_ptr = prsrs;
    char *cmd_str, *brkt;
    const static char *sep " \t"
    cmd_str = strtok_r(str,sep,&brkt);
    while (cp_ptr) {
        if (strcmp(cmd_str,cp_ptr->cmd_tkn) == 0) {
            return vvvv_cmd_prsr_prs_args(cp_ptr,&brkt);
        }
        cp_ptr = cp_ptr->next;
    }
    return vvvv_err_NMTCH;
}

