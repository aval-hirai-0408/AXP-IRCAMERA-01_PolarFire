//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdGpio.c - GPIO Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	GPIコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGpi (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdGpiHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// GPI取得
		if ((status = getGpi (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%x ", data);
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
//	GPIコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGpiHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The GPI Signal is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gpi\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : GPI Signal\n");

	DEBUG_PRINT_FORCE ("\n[Bit Assign]\n");
	DEBUG_PRINT_FORCE ("  Bit0               : Line0\n");
	DEBUG_PRINT_FORCE ("  Bit1               : Line1\n");
	DEBUG_PRINT_FORCE ("  Bit2               : Line2\n");

	DEBUG_PRINT_FORCE ("\n[Data]\n");
	DEBUG_PRINT_FORCE ("  0 = Low Level\n");
	DEBUG_PRINT_FORCE ("  1 = High Level\n\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	GPO設定／取得コマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGpo (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdGpoHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// GPO取得
		if ((status = getGpo (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%x ", data);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%x", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// GPO設定
		if ((status = setGpo (data)) != AVAL_STATUS_SUCCESS)
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
//	GPOコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGpoHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The GPO signal setting is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gpo\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : GPO Signal[(Hex)\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The GPO signal is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : gpo\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : GPO Signal(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n[Bit Assign]\n");
	DEBUG_PRINT_FORCE ("  Bit3               : Line3\n");
	DEBUG_PRINT_FORCE ("  Bit4               : Line4\n");
	DEBUG_PRINT_FORCE ("  Bit5               : Line5\n");
	DEBUG_PRINT_FORCE ("  Bit6               : Line6\n");

	DEBUG_PRINT_FORCE ("\n[Data]\n");
	DEBUG_PRINT_FORCE ("  0 = Low Level\n");
	DEBUG_PRINT_FORCE ("  1 = High Level\n\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	GPIOデジタルノイズフィルタ設定／取得コマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGpioDnf (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int dataHigh, dataLow;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdGpioDnfHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// GPO Number
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// GPIOデジタルノイズフィルタ取得
		if ((status = gpioGetDnf (mode, &dataHigh, &dataLow)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d %d ", dataHigh, dataLow);
	}
	else if (argc == 4)
	{
		// GPO Number
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Highデータ
		if (sscanf (gCmdArg[2], "%d", &dataHigh) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Lowデータ
		if (sscanf (gCmdArg[3], "%d", &dataLow) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// GPIOデジタルノイズフィルタ設定
		if ((status = gpioSetDnf (mode, dataHigh, dataLow)) != AVAL_STATUS_SUCCESS)
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
//	GPIOデジタルノイズフィルタ設定／取得コマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGpioDnfHelp (void *str)
{
	int i;
	unsigned int dir;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The GPIO digital noise filter setting is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dnf\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : GPIO Number\n");
	DEBUG_PRINT_FORCE ("  Output Param0     : Digital Noise Filter High Time\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : Digital Noise Filter Low Time\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The GPIO digital noise filter is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : dnf [Param0] [Param1] [Param2]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : GPIO Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Digital Noise Filter High Time\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : Digital Noise Filter Low Time\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[GPIO Number]\n");
	for (i=GPIO_LINE_MIN; i<GPIO_LINE_MAX; i++)
	{
		// 入出力方法取得
		dir = IN32 ((GENICAM_DIGITAL_LINE0_MODE_ADRS + (GENICAM_DIGITAL_LINE_INTERVAL * i)));

		if (dir == GENICAM_DIGITAL_LINE_MODE_OUTPUT)
			continue;

		DEBUG_PRINT_FORCE ("  %d : Line%d\n", i, i);
	}

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[GPIO  Noise Filter Time]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %u\n", GPIO_DNF_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	GPIO Pulse Widthコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGpioPulseTime (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int high, low;
	int argc;
	int select;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdGpioPulseTimeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// ラインセレクト取得
		if ((status = digitalIoGetLineSelect (&select)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// パルス取得
		if ((status = gpioGetPulseTime (select, &high, &low)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d %d ", high, low);
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
//	GPI Pulse Widthコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGpioPulseTimeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The GPIO pulse width is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-pulse\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output0 Param     : GPI High Pulse[us]\n");
	DEBUG_PRINT_FORCE ("  Output1 Param     : GPI Low Pulse[us]\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
