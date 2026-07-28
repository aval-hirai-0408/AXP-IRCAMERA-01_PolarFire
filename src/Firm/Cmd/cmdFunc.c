//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdFunc.c - Function Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "../Common/sensorVendorS.h"


//**********************************************************************************
//	情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGcp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdGcpHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		//--------------------------------------------------
		// Firm Build情報
		//--------------------------------------------------
		cmdFirmBuildInfo ("");

		//--------------------------------------------------
		// FPGA Build情報
		//--------------------------------------------------
		cmdFpgaBuildInfo ("");

		//--------------------------------------------------
		// ボード情報
		//--------------------------------------------------
		cmdBoardInfo ("");

		//--------------------------------------------------
		// カメラ情報
		//--------------------------------------------------
		cmdAcquisitionGcp ("");

		//--------------------------------------------------
		// IPU情報
		//--------------------------------------------------
		cmdIpu ("");

		//--------------------------------------------------
		// 温度情報
		//--------------------------------------------------
		cmdTempStatus ("");

		//--------------------------------------------------
		// 初期化情報
		//--------------------------------------------------
		cmdCameraStatus ("");

		//--------------------------------------------------
		// Acquisition Trg Status
		//--------------------------------------------------
		cmdAcquisitionStatusAll ("");

		//--------------------------------------------------
		// Digital IO Control Status
		//--------------------------------------------------
		cmdDigitaiIoStatusAll ("");

		//--------------------------------------------------
		// Encoder Control Status
		//--------------------------------------------------
#ifdef MODE_ENCODER
		cmdEncoderStatusAll ("");
#endif

		//--------------------------------------------------
		// Spectrum
		//--------------------------------------------------
#ifdef MODE_SPECTRUM
		// Spectrum GCP
		cmdSpectrumGcp ("");
#endif


		//--------------------------------------------------
		// Voltage
		//--------------------------------------------------
#if defined (MODE_VOLTAGE_FPGA_BOARD)
		// FPGA Board Voltage Show
		voltFpgaBoardShow();
#endif


#if defined (MODE_VOLTAGE_IF_BOARD)
		// Interface Board Voltage Show
		voltIfBoardShow();
#endif
	}
	else if (argc == 2)
	{
		// Mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check mode Parameter
		if ((mode < GCP_MODE_MIN) || (mode > GCP_MODE_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "GCP mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, GCP_MODE_MIN, GCP_MODE_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		//--------------------------------------------------
		// ボード情報
		//--------------------------------------------------
		if (mode == GCP_MODE_BOARD)
		{
			// Board Information
			cmdBoardInfo ("");
		}
		else if (mode == GCP_MODE_CAMERA)
		{
			// カメラ情報
			cmdAcquisitionGcp ("");
		}
		else if (mode == GCP_MODE_IPU)
		{
			// IPU情報
			cmdIpu ("");
		}
		else if (mode == GCP_MODE_TEMP)
		{
			// 温度情報
			cmdTempStatus ("");
		}
		else if (mode == GCP_MODE_CAMERA_STATUS)
		{
			// 初期化情報
			cmdCameraStatus ("");
		}
		else if (mode == GCP_MODE_ACQUISITION)
		{
			// Acquisition Control Status
			cmdAcquisitionStatusAll ("");
		}
		else if (mode == GCP_MODE_DIGITALIO)
		{
			// Digital IO Control Status
			cmdDigitaiIoStatusAll ("");
		}
		else if (mode == GCP_MODE_COUNTER)
		{
			// Counter Control Status
			cmdCounterStatusAll ("");
		}
		else if (mode == GCP_MODE_TIMER)
		{
			// Timer Control Status
			cmdTimerStatusAll ("");
		}
#if defined (MODE_ENCODER)
		else if (mode == GCP_MODE_ENCODER)
		{
			// Encoder Control Status
			cmdEncoderStatusAll ("");
		}
#endif
		
#if defined (MODE_SPECTRUM)
		else if (mode == GCP_MODE_SPECTRUM)
		{
			// Spectrum GCP
			cmdSpectrumGcp ("");
		}
#endif
		else if (mode == GCP_MODE_VOLTAGE)
		{
#if defined (MODE_VOLTAGE_FPGA_BOARD)
		// FPGA Board Voltage Show
		voltFpgaBoardShow();
#endif

#if defined (MODE_VOLTAGE_IF_BOARD)
		// Interface Board Voltage Show
		voltIfBoardShow();
#endif
		}
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
//	GCP Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGcpHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Camera Information All Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : gcp\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Camera Information\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : gcp [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : information number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Camera Information\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Information Number]\n");
	DEBUG_PRINT_FORCE ("   %2d : Board Information\n", GCP_MODE_BOARD);
	DEBUG_PRINT_FORCE ("   %2d : Camera Information\n", GCP_MODE_CAMERA);
	DEBUG_PRINT_FORCE ("   %2d : IPU Information\n", GCP_MODE_IPU);
	DEBUG_PRINT_FORCE ("   %2d : Temp Information\n", GCP_MODE_TEMP);
	DEBUG_PRINT_FORCE ("   %2d : Camera Status Information\n", GCP_MODE_CAMERA_STATUS);
	DEBUG_PRINT_FORCE ("   %2d : Acquisition Control Information\n", GCP_MODE_ACQUISITION);
	DEBUG_PRINT_FORCE ("   %2d : Digital IO Control Information\n", GCP_MODE_DIGITALIO);
	DEBUG_PRINT_FORCE ("   %2d : Counter Control Information\n", GCP_MODE_COUNTER);
	DEBUG_PRINT_FORCE ("   %2d : Timer Control Information\n", GCP_MODE_TIMER);
#ifdef MODE_ENCODER
	DEBUG_PRINT_FORCE ("   %2d : Encoder Control Information\n", GCP_MODE_ENCODER);
#endif
#ifdef MODE_SPECTRUM
	DEBUG_PRINT_FORCE ("   %2d : Spectrum Information\n", GCP_MODE_SPECTRUM);
#endif
#if defined (MODE_VOLTAGE_FPGA_BOARD) || defined (MODE_VOLTAGE_IF_BOARD) || defined (MODE_VOLTAGE_SENSOR_BOARD) || defined (MODE_VOLTAGE_SUB_BOARD)
	DEBUG_PRINT_FORCE ("   %2d : Voltage Information\n", GCP_MODE_VOLTAGE);
#endif

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Delayコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdUsDelay (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int delay;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdUsDelayHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Delay Time取得
		if (sscanf (gCmdArg[1], "%d", &delay) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}
		
		// Delay
		usDelay (delay);
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
//	DelayコマンドHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdUsDelayHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Wait for time.\n");
	DEBUG_PRINT_FORCE ("  Command           : usdelay\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Delay Time[us]\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Fill
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFill (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int i;
	unsigned int adrs, size, data;
	unsigned char data8;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFillHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 4)
	{
		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// サイズ取得
		if (sscanf (gCmdArg[2], "%x", &size) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Fillデータ取得
		if (sscanf (gCmdArg[3], "%x", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		data8 = (unsigned char)data;

		// Fill
		for (i=0; i<size; i++, adrs++)
			(*(volatile unsigned char *)(unsigned long)adrs) = data8;

		// キャッシュFlash
		cacheFlush ();
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
//	Fill Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFillHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Memory Fill.\n");
	DEBUG_PRINT_FORCE ("  Command           : fill [param0] [param1] [param2]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Address(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Size(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : Fill Byte Data(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Fill Inc
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFillInc (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int i;
	unsigned int adrs, size, data;
	unsigned char data8;
	unsigned short data16;
	unsigned int data32;
	unsigned long long data64;
	unsigned char *ptr8;
	unsigned short *ptr16;
	unsigned int *ptr32;
	unsigned long long *ptr64;
	unsigned int align;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFillIncHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 5)
	{
		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// サイズ取得
		if (sscanf (gCmdArg[2], "%x", &size) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Fillデータ取得
		if (sscanf (gCmdArg[3], "%x", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 何Byte単位取得
		if (sscanf (gCmdArg[4], "%x", &align) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		if (align == 1)
		{
			// Fill Increment
			data8 = (unsigned char)data;
			ptr8  = (unsigned char *)(unsigned long)adrs;
			for (i=0; i<size; i++, ptr8++, data8++)
				*ptr8 = data8;
		}
		else if (align == 2)
		{
			// Fill Increment
			data16 = (unsigned short)data;
			ptr16  = (unsigned short *)(unsigned long)adrs;
			for (i=0; i<(size/2); i++, ptr16++, data16++)
				*ptr16 = data16;
		}
		else if (align == 4)
		{
			// Fill Increment
			data32 = (unsigned int)data;
			ptr32  = (unsigned int *)(unsigned long)adrs;
			for (i=0; i<(size/4); i++, ptr32++, data32++)
				*ptr32 = data32;
		}
		else if (align == 8)
		{
			// Fill Increment
			data64 = (unsigned long)data;
			ptr64  = (unsigned long long *)(unsigned long)adrs;
			for (i=0; i<(size/8); i++, ptr64++, data64++)
				*ptr64 = data64;
		}
		else
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
			sprintf (gLogMsgBuff, "Fill Increment Align(%d) Error.\n", align);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// キャッシュFlash
		cacheFlush ();
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
//	Fill Inc Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFillIncHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Memory Fill Increment.\n");
	DEBUG_PRINT_FORCE ("  Command           : fillInc [param0] [param1] [param2] [param3]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Address(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Size(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : Start Data(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param3     : Align(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	キャッシュフラッシュコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCacheFlash (void *str)
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
			cmdCacheFlashHelp (NULL);
			goto _DONE;
		}
	}

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
//	キャッシュフラッシュHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCacheFlashHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Cache Flush.\n");
	DEBUG_PRINT_FORCE ("  Command           : cacheflush\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Default All
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDefaultAll (void *str)
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
			cmdDefaultAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{

		// Default All
		if ((status = defaultAll ()) != AVAL_STATUS_SUCCESS)
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
//	Default All Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDefaultAllHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Camera Default.\n");
	DEBUG_PRINT_FORCE ("  Command           : defaultall\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	レジスタ表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRegMap (void *str)
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
			cmdRegMapHelp (NULL);
			goto _DONE;
		}
	}

	// Mapping表示
	fpgaRegMap ();

_DONE:
	return (status);
}


//**********************************************************************************
//	レジスタ表示Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRegMapHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Register mapping display.\n");
	DEBUG_PRINT_FORCE ("  Command           : regmap\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}



//**********************************************************************************
//	UserID設定／取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserID (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	char userId[(CAMERA_SAVE_USERID_SIZE+1)];

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCameraUserIDHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// User ID取得
		if ((status = getUserId ((char *)userId)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (userId[0] !=  '\0')
		{
			DEBUG_PRINT_FORCE ("%s ", userId);
		}
	}
	else if (argc == 2)
	{
		// User ID設定
		if ((status = setUserId (gCmdArg[1])) != AVAL_STATUS_SUCCESS)
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
//	UserID設定／取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserIDHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : A User ID of a camera is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : userid\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : camera name (String Length Min:1 - Max:%d)\n",  CAMERA_SAVE_USERID_SIZE);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A User ID of a camera is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : userid [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : camera name (String Length Min:1 - Max:%d)\n",  CAMERA_SAVE_USERID_SIZE);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	UserID Delete設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserIDDelete (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	char userId[(CAMERA_SAVE_USERID_SIZE+1)];

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCameraUserIDDeleteHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// User ID Delete
		userId[0] = '\0';
		if ((status = setUserId ((char *)userId)) != AVAL_STATUS_SUCCESS)
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
//	UserID Delete設定Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserIDDeleteHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A User ID of a camera is Delete.\n");
	DEBUG_PRINT_FORCE ("  Command           : userid-delete\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	システムタイマ時間計測
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdDiagTimer (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	char bf[CONSOLE_BUFF_SIZE];
	int hh, mm, ss;
	unsigned int count, countMs;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDiagTimerHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{

		DEBUG_PRINT ("<<< SYS Timer Test >>>\n");

		DEBUG_PRINT ("->->->-> Test Start OK ? input RTN key\n");
		memset(bf, 0, CONSOLE_BUFF_SIZE);
		cmdCharGet (bf);

		if ((bf[0] == 'q') || (bf[0] == 'Q') || (bf[0] == '.'))
		{
			DEBUG_PRINT ("Aborted.\n");
			return (AVAL_STATUS_SUCCESS);
		}

		// タイマカウンタ初期化
		timerSetSec (0);
		timerSetMs (0);
		timerStart ();

		while (1)
		{
			DEBUG_PRINT ("->->->-> Time Values Show ? input RTN key\n");
			memset(bf, 0, CONSOLE_BUFF_SIZE);
			cmdCharGet (bf);

			// タイマカウント取得
			count = timerGetSec();
			countMs = timerGetMs();

			if ((bf[0] == 'q') || (bf[0] == 'Q') || (bf[0] == '.'))
				break;

			ss  = count;
			hh = (ss / (60 * 60));
			mm = ((ss % (60 * 60)) / 60);
			ss = (ss % 60);

			DEBUG_PRINT ("SYS SEC total_sec=%d : hour=%d min=%d sec=%d \n", count, hh, mm, ss);

			ss  = countMs/1000;
			hh = (ss / (60 * 60));
			mm = ((ss % (60 * 60)) / 60);
			ss = (ss % 60);

			DEBUG_PRINT ("SYS SEC total_sec=%d : hour=%d min=%d sec=%d \n", countMs, hh, mm, ss);
		}
		
		timerStop ();
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
//	Timer Helpp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagTimerHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : timer show.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
