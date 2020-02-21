#ifndef _CLI_CORE_H_
#define _CLI_CORE_H_

extern ULONG cmd_ip_string_to_ulong(CHAR *ip);
extern VOID cmd_ip_ulong_to_string(ULONG ip, CHAR *buf);
extern ULONG cmd_string_is_ip(CHAR *str);

extern VOID vty_offline(ULONG vtyId);	
extern VOID cmd_clear_line(CMD_VTY_S *vty);
extern CMD_VTY_S *cmd_vty_getById(ULONG vtyId);
extern VOID cmd_vty_init(CMD_VTY_S *vty);
extern VOID cmd_vty_add_history(CMD_VTY_S *vty);
extern VOID cmd_vector_insert(CMD_VECTOR_S *v, VOID *val);
extern VOID cmd_vector_insert_cr(CMD_VECTOR_S *v);

extern CMD_VECTOR_S *cmd_vector_copy(CMD_VECTOR_S *v);
extern CMD_VECTOR_S *cmd_str2vec(CHAR *string);
extern CMD_VECTOR_S *cmd_vector_init();
extern VOID cmd_vector_deinit(CMD_VECTOR_S *v, ULONG freeall);

extern ULONG cmd_match_command(CMD_VECTOR_S *icmd_vec, CMD_VTY_S *vty,
		CMD_ELMT_S **ppstMatchCmdElem, ULONG *pulMatchNum);

extern ULONG cmd_complete_command(CMD_VECTOR_S *icmd_vec, CMD_VTY_S *vty,
											CMD_ELMT_S **ppstCmdElem, ULONG *pulMatchNum, ULONG *pulNoMatchPos);

extern ULONG cmd_execute_command(CMD_VECTOR_S *icmd_vec, CMD_VTY_S *vty,
											CMD_ELMT_S **ppstCmdElem, ULONG *pulMatchNum, ULONG *pulNoMatchPos);

extern VOID cmd_output_missmatch(CMD_VTY_S *vty, ULONG ulNoMatchPos);

extern VOID cmd_insert_word(CMD_VTY_S *vty, CHAR *str);
extern VOID cmd_delete_word(CMD_VTY_S *vty);
extern VOID cmd_delete_word_ctrl_W(CMD_VTY_S *vty);
extern VOID cmd_delete_word_ctrl_W_ex(CMD_VTY_S *vty);

extern VOID cmd_vty_set_terminaldebug(VOID *vty, CHAR isEnable);

extern VOID cmd_read(CMD_VTY_S *vty);
extern ULONG cmd_run(CMD_VTY_S *vty);
extern VOID cmd_outprompt(CMD_VTY_S *vty);

extern ULONG cmd_parse(CMD_VTY_S *vty);
extern ULONG cmd_parse_vty(CMD_VTY_S *vty);
extern CMD_VECTOR_S *cmd_vector_new();
extern VOID cmd_vector_free(CMD_VECTOR_S **ppVec);
extern ULONG cmd_regelement_new(ULONG cmd_elem_id, CMD_ELEM_TYPE_E cmd_elem_type, CHAR *cmd_name, CHAR *cmd_help, CMD_VECTOR_S *pVec);
extern VOID cmd_install_command(ULONG mid, ULONG cmd_view, CHAR *cmd_string, CMD_VECTOR_S *pVec);
extern ULONG cmd_view_regist(CHAR *view_name, CHAR *view_ais, ULONG view_id, ULONG parent_view_id);

extern ULONG cmd_get_vty_id(VOID *pRunMsg);
extern VOID *cmd_get_elem_by_index(VOID *pRunMsg, ULONG index);
extern ULONG cmd_get_elem_num(VOID *pRunMsg);
extern ULONG cmd_get_elemid(VOID *pElemMsg);
extern CHAR* cmd_get_elem_param(VOID *pElemMsg);
extern ULONG cmd_get_ulong_param(VOID *pElemMsg);
extern VOID cmd_copy_string_param(VOID *pElemMsg, CHAR *param);	
extern ULONG cmd_get_first_elem_tblid(VOID *pRunMsg);
extern ULONG cmd_regcallback(ULONG ulMid, ULONG (*pfCallBackFunc)(VOID *pRcvMsg));


extern VOID cmd_handle_filter(CMD_VTY_S *vty);
extern VOID cmd_handle_tab(CMD_VTY_S *vty);
extern VOID cmd_handle_enter(CMD_VTY_S *vty);
extern VOID cmd_handle_quest(CMD_VTY_S *vty);
extern VOID cmd_handle_up(CMD_VTY_S *vty);
extern VOID cmd_handle_down(CMD_VTY_S *vty);
extern VOID cmd_handle_left(CMD_VTY_S *vty);
extern VOID cmd_handle_right(CMD_VTY_S *vty);
extern VOID cmd_handle_delete(CMD_VTY_S *vty);
extern VOID cmd_handle_backspace(CMD_VTY_S *vty);
extern VOID cmd_handle_insert(CMD_VTY_S *vty);
extern VOID cmd_hanlde_del_lastword(CMD_VTY_S *vty);

extern ULONG cmd_get_command_string(CMD_LINE_S *pstLine, CHAR *pszCmdString, int iSize);

#endif _COMMAND_CORE_H_
