
#ifdef WIN32
#include <windows.h>
#include <process.h>
#if CYGWIN
#include <ncurses.h>
#else
#include <conio.h>
#endif
#include <io.h>
#endif

#ifdef _LINUX_
#include <pthread.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <termios.h>
#include <assert.h>
#include <errno.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "icli.h"

extern VOID vty_printf(ULONG vtyId, CHAR *format, ...);
extern int cmd_init();

#ifndef CMD_NULL_DWORD
#undef CMD_NULL_DWORD
#define CMD_NULL_DWORD 0xFFFFFFFF
#endif

/* os version */
#define OS_VERSION_MAJOR 0
#define OS_VERSION_MINOR 0
#define OS_VERSION_PATCH 2

#define MID_TEST 1

enum CMO_TBLID_EM
{
	CMO_TBL_SHOW = 0,
	CMO_TBL_CFG,
};

enum CMO_SHOW_ID_EM
{
	CMO_SHOW_SHOW_DEVICE = CMD_ELEMID_DEF(MID_TEST, CMO_TBL_SHOW, 0),
	CMO_SHOW_SHOW_SLOTID,
	CMO_SHOW_SHOW_NAME,
	CMO_SHOW_CUR_CONFIG,
};

enum CMO_CFG_ID_EM
{
	CMO_CFG_QUIT = CMD_ELEMID_DEF(MID_TEST, CMO_TBL_CFG, 0),
	CMO_CFG_SYSVIEW,
	CMO_CFG_DIAGVIEW,
	CMO_CFG_SYSNAME,
};

extern CHAR g_cmd_sysname[];

ULONG show_callback(VOID *pRcvMsg)
{
	ULONG iLoop = 0;
	ULONG iElemNum = 0;
	ULONG iElemID = 0;
	VOID *pElem = NULL;
	ULONG vtyId = 0;
	ULONG SlotId = 0xFFFFFFFF;
	ULONG isDevice = 0;
	ULONG isUsername = 0;
	CHAR szName[32] = {0};

	vtyId = cmd_get_vty_id(pRcvMsg);
	iElemNum = cmd_get_elem_num(pRcvMsg);

	for (iLoop = 0; iLoop < iElemNum; iLoop++)
	{
		pElem = cmd_get_elem_by_index(pRcvMsg, iLoop);
		iElemID = cmd_get_elemid(pElem);

		switch (iElemID)
		{
		case CMO_SHOW_SHOW_DEVICE:
			isDevice = 1;
			break;
		case CMO_SHOW_SHOW_SLOTID:
			SlotId = cmd_get_ulong_param(pElem);
			break;
		case CMO_SHOW_SHOW_NAME:
			isUsername = 1;
			cmd_copy_string_param(pElem, szName);
			break;
		case CMO_SHOW_CUR_CONFIG:
			cli_bdn_show(vtyId, 0);
			break;

		default:
			break;
		}
	}

	if (SlotId != 0xFFFFFFFF)
	{
		vty_printf(vtyId, "Run command display device slot %u.\r\n", SlotId);
		return 0;
	}

	if (isDevice)
	{
		vty_printf(vtyId, "Run command display device.\r\n");
		return 0;
	}

	if (isUsername)
	{
		vty_printf(vtyId, "Run command display user-name %s.\r\n", szName);
		return 0;
	}

	return 0;
}

ULONG cfg_callback(VOID *pRcvMsg)
{
	ULONG iLoop = 0;
	ULONG iElemNum = 0;
	ULONG iElemID = 0;
	VOID *pElem = NULL;
	ULONG vtyId = 0;

	vtyId = cmd_get_vty_id(pRcvMsg);
	iElemNum = cmd_get_elem_num(pRcvMsg);

	for (iLoop = 0; iLoop < iElemNum; iLoop++)
	{
		pElem = cmd_get_elem_by_index(pRcvMsg, iLoop);
		iElemID = cmd_get_elemid(pElem);

		switch (iElemID)
		{
		case CMO_CFG_QUIT:
			vty_view_quit(vtyId);
			break;

		case CMO_CFG_SYSVIEW:
			vty_view_set(vtyId, VIEW_SYSTEM);
			break;

		case CMO_CFG_DIAGVIEW:
			vty_view_set(vtyId, VIEW_DIAGNOSE);
			break;

		case CMO_CFG_SYSNAME:
			cmd_copy_string_param(pElem, g_cmd_sysname);
			break;

		default:
			break;
		}
	}

	return 0;
}

ULONG cmd_callback(VOID *pRcvMsg)
{
	ULONG iRet = 0;
	ULONG iTBLID = 0;

	iTBLID = cmd_get_first_elem_tblid(pRcvMsg);

	switch (iTBLID)
	{
	case CMO_TBL_SHOW:
		iRet = show_callback(pRcvMsg);
		break;
	case CMO_TBL_CFG:
		iRet = cfg_callback(pRcvMsg);
		break;

	default:
		break;
	}

	return iRet;
}

ULONG reg_cmd()
{
	CMD_VECTOR_S *vec = NULL;

	/* 命令行注册四部曲1: 申请命令行向量 */
	CMD_VECTOR_NEW(vec);

	/* 命令行注册四部曲2: 定义命令字 */
	/* 1 display */
	cmd_regelement_new(CMD_ELEMID_NULL, CMD_ELEM_TYPE_KEY, "display", "Display", vec);
	/* 2 device */
	cmd_regelement_new(CMO_SHOW_SHOW_DEVICE, CMD_ELEM_TYPE_KEY, "device", "Device", vec);
	/* 3 slot */
	cmd_regelement_new(CMD_ELEMID_NULL, CMD_ELEM_TYPE_KEY, "slot", "Slot", vec);
	/* 4 slot-id */
	cmd_regelement_new(CMO_SHOW_SHOW_SLOTID, CMD_ELEM_TYPE_INTEGER, "INTEGER<0-8>", "Slot ID", vec);
	/* 5 user-name */
	cmd_regelement_new(CMD_ELEMID_NULL, CMD_ELEM_TYPE_KEY, "user-name", "User Name", vec);
	/* 6 user-name-value */
	cmd_regelement_new(CMO_SHOW_SHOW_NAME, CMD_ELEM_TYPE_STRING, "STRING<1-32>", "User Name, length range from 1 to 32.", vec);
	/* 7 user-name-value */
	cmd_regelement_new(CMO_SHOW_CUR_CONFIG, CMD_ELEM_TYPE_KEY, "current-configuration", "Current configuration", vec);

	/* 命令行注册四部曲3: 注册命令行 */
	cmd_install_command(MID_TEST, VIEW_GLOBAL, " 1 2 ", vec);	 /* display device */
	cmd_install_command(MID_TEST, VIEW_GLOBAL, " 1 2 3 4 ", vec); /* display device slot <slot-id> */
	cmd_install_command(MID_TEST, VIEW_GLOBAL, " 1 5 6 ", vec);   /* display user-name <user-name-value> */
	cmd_install_command(MID_TEST, VIEW_GLOBAL, " 1 7 ", vec);	 /* display current-configuration */

	/* 命令行注册四部曲4: 释放命令行向量 */
	CMD_VECTOR_FREE(vec);

	return 0;
}

ULONG reg_cmd2()
{
	CMD_VECTOR_S *vec = NULL;

	/* 命令行注册四部曲1: 申请命令行向量 */
	CMD_VECTOR_NEW(vec);

	/* 命令行注册四部曲2: 定义命令字 */
	/* 1 quit */
	cmd_regelement_new(CMO_CFG_QUIT, CMD_ELEM_TYPE_KEY, "quit", "Quit from current view", vec);
	/* 2 system-view */
	cmd_regelement_new(CMO_CFG_SYSVIEW, CMD_ELEM_TYPE_KEY, "system-view", "Enter system-view", vec);
	/* 3 diagnose-view */
	cmd_regelement_new(CMO_CFG_DIAGVIEW, CMD_ELEM_TYPE_KEY, "diagnose-view", "Enter diagnose-view", vec);
	/* 4 sysname */
	cmd_regelement_new(CMD_ELEMID_NULL, CMD_ELEM_TYPE_KEY, "sysname", "Set system name", vec);
	/* 5 <sysname> */
	cmd_regelement_new(CMO_CFG_SYSNAME, CMD_ELEM_TYPE_STRING, "STRING<1-24>", "System Name", vec);

	/* 命令行注册四部曲3: 注册命令行 */
	cmd_install_command(MID_TEST, VIEW_GLOBAL, " 1 ", vec);
	cmd_install_command(MID_TEST, VIEW_USER, " 2 ", vec);
	cmd_install_command(MID_TEST, VIEW_SYSTEM, " 3 ", vec);
	cmd_install_command(MID_TEST, VIEW_SYSTEM, " 4 5 ", vec);

	/* 命令行注册四部曲4: 释放命令行向量 */
	CMD_VECTOR_FREE(vec);

	return 0;
}

ULONG cmd_buildrun(CHAR **ppBuildrun, ULONG ulIncludeDefault)
{
	CHAR *pBuildrun = NULL;

	*ppBuildrun = (CHAR *)malloc(BDN_MAX_BUILDRUN_SIZE);
	if (NULL == *ppBuildrun)
	{
		return 1;
	}
	memset(*ppBuildrun, 0, BDN_MAX_BUILDRUN_SIZE);

	pBuildrun = *ppBuildrun;

	pBuildrun += sprintf(pBuildrun, "#version %u.%u.%u", OS_VERSION_MAJOR, OS_VERSION_MINOR, OS_VERSION_PATCH);
	pBuildrun += sprintf(pBuildrun, BDN_BUILDRUN "sysname %s", g_cmd_sysname);

	if (0 == strlen(*ppBuildrun))
	{
		free(*ppBuildrun);
		*ppBuildrun = NULL;
	}

	return 0;
}

int main()
{
	/* 初始化 */
	cmd_init();

	/* 注册命令行 */
	reg_cmd();
	reg_cmd2();

	/* 注册buildrun处理回调 */
	(void)cli_bdn_regist(MID_TEST, VIEW_SYSTEM, BDN_PRIORITY_NORMAL, cmd_buildrun);

	/* 注册命令行处理回调 */
	(void)cmd_regcallback(MID_TEST, cmd_callback);

	/* 创建主线程 */
#ifdef WIN32
	extern unsigned _stdcall cmd_main_entry(void *pEntry);
	_beginthreadex(NULL, 0, cmd_main_entry, NULL, NULL, NULL);
#else
	pthread_t tidp;
	extern void* cmd_main_entry(void *pEntry);
	(void)pthread_create(&tidp, 0, cmd_main_entry, 0);
#endif

	for (;;)
	{
#ifdef WIN32
		Sleep(1);
#else
		usleep(1000);
#endif
	}

	return 0;
}
