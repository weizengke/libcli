
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef WIN32

#if CYGWIN
#include <ncurses.h>
#else
#include <conio.h>
#endif

#include <io.h>
#include <winsock2.h>
#endif

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#ifdef _LINUX_
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <termios.h>
#include <assert.h>
#endif

#include "icli.h"
#include "cli_def.h"
#include "cli_type.h"
#include "cli_core.h"
#include "cli_view.h"
#include "cli_history.h"
#include "cli_reg.h"

CMD_NTF_NODE_S *g_cmd_reg_callback = NULL;	/* command process callback*/

CMD_NTF_NODE_S *cmd_get_regntfhead()
{
    return g_cmd_reg_callback;
}

ULONG cmd_regcallback(ULONG ulMid, ULONG (*pfCallBackFunc)(VOID *pRcvMsg))
{
	CMD_NTF_NODE_S *pstNow = NULL;
	CMD_NTF_NODE_S *pstPre = NULL;
	CMD_NTF_NODE_S *pstEvtNtfNodeNew = NULL;

	pstEvtNtfNodeNew = (CMD_NTF_NODE_S *)malloc(sizeof(CMD_NTF_NODE_S));
	if (NULL == pstEvtNtfNodeNew)
	{
		return CMD_ERR;
	}

	pstEvtNtfNodeNew->ulMid = ulMid;
	pstEvtNtfNodeNew->pfCallBackFunc = pfCallBackFunc;

	if (NULL == g_cmd_reg_callback)
	{
		g_cmd_reg_callback = pstEvtNtfNodeNew;
		pstEvtNtfNodeNew->pNext = NULL;
		return CMD_OK;
	}

	pstNow = g_cmd_reg_callback;
	pstPre = pstNow;

	while (NULL != pstNow)
	{
		if (pfCallBackFunc == pstNow->pfCallBackFunc)
		{
			free(pstEvtNtfNodeNew);
			return CMD_OK;
		}

		pstPre = pstNow;
		pstNow = pstNow->pNext;
	}

	if (NULL == pstNow)
	{
		/* INSERT TAIL*/
		pstPre->pNext = pstEvtNtfNodeNew;
		pstEvtNtfNodeNew->pNext = NULL;
	}
	else
	{
		/* INSERT HEAD */
		pstEvtNtfNodeNew->pNext = pstNow;

		if (pstNow == pstPre)
		{
			g_cmd_reg_callback = pstEvtNtfNodeNew;
		}
		else
		{
			pstPre->pNext = pstEvtNtfNodeNew;
		}
	}

	return CMD_OK;
}

ULONG cmd_regelement_new(ULONG cmd_elem_id,
						 CMD_ELEM_TYPE_E cmd_elem_type,
						 CHAR *cmd_name,
						 CHAR *cmd_help,
						 CMD_VECTOR_S *pVec)
{
	CMD_ELMT_S *pstCmdElem = NULL;

	if (CMD_MAX_CMD_ELEM_SIZE <= strlen(cmd_name))
	{
		CMD_DBGASSERT(0, "cmd_regelement_new");
		return CMD_ERR;
	}

	pstCmdElem = (CMD_ELMT_S *)malloc(sizeof(CMD_ELMT_S));
	if (NULL == pstCmdElem)
	{
		return CMD_ERR;
	}
	memset(pstCmdElem, 0, sizeof(CMD_ELMT_S));

	pstCmdElem->ulElmtId = cmd_elem_id;
	pstCmdElem->eElmtType = cmd_elem_type;

	strcpy(pstCmdElem->szElmtName, cmd_name);
	strcpy(pstCmdElem->szElmtHelp, cmd_help);

#if 0
	CMD_debug(CMD_DEBUG_INFO, "cmd_regelement_new: ulElmtId=0x%x, eElmtType=%u, szElmtName=%s,(%s), pfElmtHelpFunc=0x%x, pfElmtCheckFunc=0x%x", 
		pstCmdElem->ulElmtId, pstCmdElem->eElmtType,
		pstCmdElem->szElmtName, pstCmdElem->szElmtHelp, 
		pstCmdElem->pfElmtHelpFunc, pstCmdElem->pfElmtCheckFunc);
#endif

	cmd_vector_insert(pVec, pstCmdElem);

	return CMD_OK;
}
