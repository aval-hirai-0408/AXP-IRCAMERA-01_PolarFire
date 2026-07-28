//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdDiagMain.c - Diag Main Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
unsigned int gAgingLoop = 0;


//----------------------------------------------------------------------------------
// Command Table(Diag)
//----------------------------------------------------------------------------------
CMD_TBL cmdDiagTbl2 [] =
{
	// 開発用
	{ (char *)"Diag(Develop)     ",	(CMDFUNC)cmdDiagDevelopMain,			(CMDFUNC)NULL,		OPT_NONE},

	// Dump/Aces
	{ (char *)"Dump              ",	(CMDFUNC)cmdDiagDumpMain,				(CMDFUNC)NULL,		OPT_NONE},

	// term
	{ (char *)NULL, 0, 0, 0, (char *)NULL }
};


//----------------------------------------------------------------------------------
// Command Table(Develop)
//----------------------------------------------------------------------------------
CMD_TBL cmdDiagDevelopTbl [] =
{
	// QSPI Flash
	{ (char *)"QSPI Erase        ", (CMDFUNC)cmdDiagQspiErase,				(CMDFUNC)NULL,			OPT_LOOP},
	{ (char *)"QSPI Erase All    ",	(CMDFUNC)cmdDiagQspiEraseAll,			(CMDFUNC)NULL,			OPT_LOOP},
	{ (char *)"QSPI RW           ", (CMDFUNC)cmdDiagQspiFlashRW,			(CMDFUNC)NULL,			OPT_LOOP},
	{ (char *)"QSPI RW All       ", (CMDFUNC)cmdDiagQspiFlashRWAll,			(CMDFUNC)NULL,			OPT_LOOP},
	{ (char *)"QSPI Random       ", (CMDFUNC)cmdDiagQspiFlashRWRandom,		(CMDFUNC)NULL,			OPT_LOOP},
	{ (char *)"QSPI Blank Check  ", (CMDFUNC)cmdDiagQspiBlankCheck,			(CMDFUNC)NULL,			OPT_LOOP},
	{ (char *)"QSPI Protect Reset", (CMDFUNC)cmdDiagQspiWriteProtectCancel,	(CMDFUNC)NULL,			OPT_LOOP},
	{ (char *)"QSPI Protect Stat ", (CMDFUNC)cmdDiagQspiWriteProtectStatus,	(CMDFUNC)NULL,			OPT_LOOP},

	// UART
	{ (char *)"UART              ", (CMDFUNC)cmdDiagUart,					(CMDFUNC)NULL,			OPT_LOOP},

	// term
	{ (char *)NULL, 0, 0, 0, (char *)NULL }
};


//----------------------------------------------------------------------------------
// Command Table(Dump/Aces)
//----------------------------------------------------------------------------------
CMD_TBL cmdDiagDumpTbl [] =
{
	// QSPI Flash Access/Dump
	{ (char *)"dumpQspi          ",	(CMDFUNC)cmdDumpQspiFlashMain,		(CMDFUNC)NULL,		OPT_NONE},
	{ (char *)"ioQspi            ",	(CMDFUNC)cmdAcesQspiFlashMain,		(CMDFUNC)NULL,		OPT_NONE},

#if defined (IF_GIGE)
	// GigE Phy
	{ (char *)"dumpGigEPhy       ", (CMDFUNC)cmdDumpGigEPhyMain,		(CMDFUNC)NULL,		OPT_NONE},
	{ (char *)"ioGigEPhy         ",	(CMDFUNC)cmdAcesGigEPhyMain,		(CMDFUNC)NULL,		OPT_NONE},
#endif
	
	// term
	{ (char *)NULL, 0, 0, 0, (char *)NULL }
};




//**********************************************************************************
//	Diag Main Program
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		loop					：テスト回数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagMain2 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDiagMain2Help (NULL);
			goto _DONE;
		}
	}

	status = cmdMain (cmdDiagTbl2, CMD_MODE_NUMBER);

_DONE:
	return (status);
}


//**********************************************************************************
//	Diag Main Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagMain2Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : HW Check Program.\n");
	DEBUG_PRINT_FORCE ("  Command           : diag2\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Diag Main Program(Develop)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDevelopMain (void *str)
{
	int status;

	status = cmdMain (cmdDiagDevelopTbl, CMD_MODE_NUMBER);
	return (status);
}


//**********************************************************************************
//	Diag Main Program(Dump/Aces)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDumpMain (void *str)
{
	int status;

	status = cmdMain (cmdDiagDumpTbl, CMD_MODE_NUMBER);
	return (status);
}


//**********************************************************************************
//	Diag Aging
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagAging (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int loop;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDiagAgingHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// ループ回数取得
		loop = atoi (gCmdArg[1]);

		// エージング
		if ((status = cmdDiagAgingMain (loop, DIAG_MODE_AGING)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Diag Aging Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagAgingHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Aging Program.\n");
	DEBUG_PRINT_FORCE ("  Command           : aging [Param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Loop Count\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Diag Aging設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		loop					：テスト回数
//		mode					：動作モード(DIAG_MODE_ONOFF/DIAG_MODE_AGING)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagAgingMain (unsigned int loop, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int tloop;
	unsigned int count;

	// 診断ステート
	setLed (LED_NUM1, LED_PATTERN10);

	// DDR Mode初期化
	diagDdrSetMode (1);

MORE:
	gAgingLoop = 0;
	for (tloop=0; tloop<loop; tloop++, gAgingLoop++)
	{
		DEBUG_PRINT("\n=====================Aging Loop = %d / %d====================\n", tloop, loop);

		// Memory
		if ((status = cmdDiagMem2 ("1")) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (mode == DIAG_MODE_AGING)
		{
			// QSPI
			if ((status = cmdDiagQspiFlashRWSector ("1")) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

	if (loop == 0xffffffff)
		goto MORE;

	_DONE:
	if (status != AVAL_STATUS_SUCCESS)
		ledSystemErrorState();
	else
		setLed (LED_NUM1, 0);
	
	// DDR Mode初期化
	diagDdrSetMode (0);

	return (status);
}

// eof
