[![Build Status](https://travis-ci.com/weizengke/libcli.svg?branch=master)](https://travis-ci.com/weizengke/libcli)

# libcli
一个command-line-interface封装库

## 什么是libcli
libcli是一个命令行接口封装库，可用于嵌入式操作系统的命令行管理，类似思科、华三交换机的命令行管理技术。<br>

## libcli支持哪些功能
1. 支持命令行联想、自动补全、命令行帮助、整形参数、字符型参数等功能
2. 支持telnet方式的多用户vty cli技术
3. 支持视图能力
4. 支持buildrun能力
```
# 使用效果
system>
system>sy
system>system-view
system]dis
system]display  ?
 device                   Device
 user-name                User Name
system]display  de
system]display  device
Run command display device.
system]display  device  ?
 slot                     Slot
 <cr>
system]display  device  s
system]display  device  slot  2
Run command display device slot 2.
system]dis
system]display  ?
 device                   Device
 user-name                User Name
system]display  us
system]display  user-name  ?
 STRING<1-32>             User Name, length range from 1 to 32.
system]display  user-name  jungle ?
 <cr>
system]display  user-name  jungle
Run command display user-name jungle.
system]dia
system-diagnose]?
 display                  Display
 quit                     Quit from current view
system-diagnose]q
system]q
system>

system]display  current-configuration
#version 0.0.1
sysname system
#
return
#
system]sys
system]sysname Cisco
Cisco]dis cu
Cisco]dis current-configuration
#version 0.0.1
sysname Cisco
#
return
#
Cisco]
```

## 使用说明
第一步编译libcli.a，根目录下一次执行如下命令编译 

```
cmake ..
cmake --build .
```

然后引用libcli.a, 可参考demo. 当然也直接拷贝cli_core.cpp、cli_bdn.cpp源码一起编译。<br>

### 引用libcli库
需要使用libcli库时，仅需包含icli.h头文件并在编译时链接libcli.a，参照demo“cli_main.cpp”。

### libcli库初始化和运行
1. 首先调用cmd_init()初始化cli库.<br>
2. 然后拉起cli库主线程cmd_main_entry().<br>
```
int main()
{	
	/* 初始化 */
	cmd_init();

	/* 注册命令行 */
	reg_cmd();

	reg_cmd2();
	
	/* 注册命令行处理回调 */
	(void)cmd_regcallback(MID_TEST, cmd_callback);
	
	/* 创建主线程 */
	_beginthreadex(NULL, 0, cmd_main_entry, NULL, NULL, NULL);

	for ( ; ; )
	{
		Sleep(1);
	}

	return 0;
}
```
3. 再来就是使用cli库注册命令行，命令注册安四部曲进行，缺一不可。<br>
举例如下：
```
#define MID_TEST 1

enum CMO_TBLID_EM {
	CMO_TBL_SHOW = 0,
	CMO_TBL_CFG,		
};

enum CMO_SHOW_ID_EM {
	CMO_SHOW_SHOW_DEVICE = CMD_ELEMID_DEF(MID_TEST, CMO_TBL_SHOW, 0),
	CMO_SHOW_SHOW_SLOTID,
	CMO_SHOW_SHOW_NAME,
};

ULONG reg_cmd()
{
	CMD_VECTOR_S * vec = NULL;
		
	/* 命令行注册四部曲1: 申请命令行向量 */
	CMD_VECTOR_NEW(vec);
	
	/* 命令行注册四部曲2: 定义命令字 */
	/* 1 display */
	cmd_regelement_new(CMD_ELEMID_NULL, 			CMD_ELEM_TYPE_KEY,    "display",  		"Display", vec);
	/* 2 device */
	cmd_regelement_new(CMO_SHOW_SHOW_DEVICE, 		CMD_ELEM_TYPE_KEY,    "device",   		"Device",  vec);
	/* 3 slot */
	cmd_regelement_new(CMD_ELEMID_NULL, 			CMD_ELEM_TYPE_KEY,    "slot",   		"Slot",  vec);
	/* 4 slot-id */
	cmd_regelement_new(CMO_SHOW_SHOW_SLOTID, 		CMD_ELEM_TYPE_INTEGER,"INTEGER<0-8>",   "Slot ID",  vec);
	/* 5 user-name */
	cmd_regelement_new(CMD_ELEMID_NULL, 			CMD_ELEM_TYPE_KEY,    "user-name",   	"User Name",  vec);
	/* 6 user-name-value */
	cmd_regelement_new(CMO_SHOW_SHOW_NAME, 			CMD_ELEM_TYPE_STRING, "STRING<1-32>",   "User Name, length range from 1 to 32.",  vec);
	
		
	/* 命令行注册四部曲3: 注册命令行 */
	cmd_install_command(MID_TEST, VIEW_GLOBAL, " 1 2 ", vec); /* display device */
	cmd_install_command(MID_TEST, VIEW_GLOBAL, " 1 2 3 4 ", vec); /* display device slot <slot-id> */
	cmd_install_command(MID_TEST, VIEW_GLOBAL, " 1 5 6 ", vec); /* display user-name <user-name-value> */
	
	/* 命令行注册四部曲4: 释放命令行向量 */
	CMD_VECTOR_FREE(vec);

	return 0;
}
```

这里说明一下cmd_install_command注册命令行时，第三个参数为线索表达式，用数字串起来代表前面cmd_regelement_new定义的命令字元素，组成一条完整的命令行。

4. 最后使用cmd_regcallback注册命令行处理回调
```

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

		switch(iElemID)
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


ULONG cmd_callback(VOID *pRcvMsg)
{
	ULONG iRet = 0;
	ULONG iTBLID = 0;

	/* 获取命令行tblid，以分别处理不同子模块的命令行 */
	iTBLID = cmd_get_first_elem_tblid(pRcvMsg);
		
	switch(iTBLID)
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

(void)cmd_regcallback(MID_TEST, cmd_callback);
```

PS: 命令行的处理过程show_callback中的步骤都是固定的。


## 使用libcli的项目
1. https://gitee.com/jungle/online-judge
   该项目使用了libcli的telnet方式的vty多用户技术。# libcli
