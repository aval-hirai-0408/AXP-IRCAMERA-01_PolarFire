//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdVersion.c - Version Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	基板バージョン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdBoardVersion (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	char ver[BOARD_VERSION_SIZE+1];

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdBoardVersionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		boardVersion (ver);
		DEBUG_PRINT_FORCE ("%s ", ver);
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
//	基板バージョン取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdBoardVersionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Board Version is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : boardversion\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Board Version\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	基板バージョン設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSetBoardVersion (void *str)
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
			cmdSetBoardVersionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		setboardVersion (gCmdArg[1]);
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
//	基板バージョン設定Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSetBoardVersionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Board Version is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : boardversion-set\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Board Version\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FPGAバージョン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFpgaVersion (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	char ver[FPGA_VERSION_SIZE+1];

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFpgaVersionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		fpgaVersion (ver);
		DEBUG_PRINT_FORCE ("%s ", ver);
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
//	FPGAバージョン取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFpgaVersionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FPGA Version is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : fpgaversion\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FPGA Version\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Firmwareバージョン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmVersion (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	char ver[FIRM_VERSION_SIZE+1];

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFirmVersionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		firmVersion (ver);
		DEBUG_PRINT_FORCE ("%s ", ver);
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
//	Firmwareバージョン取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmVersionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Firmware Version is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : firmversion\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Firmware Version\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Mainバージョン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdMainVersion (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	char ver[MAIN_VERSION_SIZE+1];

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdMainVersionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		mainVersion (ver);
		DEBUG_PRINT_FORCE ("%s ", ver);
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
//	Firmwareバージョン取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdMainVersionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Main Version is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : version\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Main Version\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Bootバージョン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdBootVersion (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	char ver[BOOT_VERSION_SIZE+1];

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdBootVersionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		bootVersion (ver);
		DEBUG_PRINT_FORCE ("%s ", ver);
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
//	Bootバージョン取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdBootVersionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Boot Version is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : bootversion\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Boot Version\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Firmware情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmware (void *str)
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
			cmdFirmwareHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		DEBUG_PRINT_FORCE ("%s ", FIRM_NAME);
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
//	Firmware情報取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmwareHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Firmware Name is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : firmware\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Firmware Name\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
