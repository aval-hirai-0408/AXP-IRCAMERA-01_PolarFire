//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdCamera.c - Camera Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern unsigned short *pImageSrc;				// 検出用入力画像の先頭を指すポインタ配列


//**********************************************************************************
//	カメラ固有情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int camerInfomation (void)
{
	double rMax;

	//--------------------------------------------------
	// カメラ情報(基本パラメータ)
	//--------------------------------------------------
	DEBUG_PRINT_FORCE ("[Camera parameter informations]\n");

	// 画素数
	DEBUG_PRINT_FORCE ("   Pixel count               ");
	DEBUG_PRINT_FORCE ("%s\n", CAMERA_PIXEL_COUNT);

	// 画素サイズ
	DEBUG_PRINT_FORCE ("   Pixel size                ");
	DEBUG_PRINT_FORCE ("%s\n", CAMERA_PIXEL_SIZE);

	// フレームレート(最大)
	rateMax (&rMax);
	DEBUG_PRINT_FORCE ("   Maximum Frame Rate        ");
	DEBUG_PRINT_FORCE ("%.2f FPS\n", (double)rMax);

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Camera Type
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraType (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int type;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCameraTypeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// type取得
		if ((status = cameraGetType (&type)) != AVAL_STATUS_SUCCESS)
			return (status);

		if (type == CAMERA_TYPE_AREA)
			DEBUG_PRINT_FORCE ("Areascan ");
		else if (type == CAMERA_TYPE_LINE)
			DEBUG_PRINT_FORCE ("Linescan ");
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
//	Camera Type Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraTypeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Camera Scan Type.\n");
	DEBUG_PRINT_FORCE ("  Command           : deviceScanType\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Camera Scan Type.(Area or Line)\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	1フレーム取り込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCapture (void *str)
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
			cmdCaptureHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// ROI Full Size
		if ((status = roiSetDmaFull ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT ("Adrs = 0x%x\n", pImageSrc);
		status = dmaStart ((unsigned char *)pImageSrc);

		// ROI Restore
		roiSetDmaResutore ();
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
//	1フレーム取り込みHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCaptureHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : A Image is taken in from a camera.\n");
	DEBUG_PRINT_FORCE ("  Command           : capture\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

//**********************************************************************************
//	ボード初期化情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraStatus (void *str)
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
			cmdCameraStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		cameraStatusShow ();
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
//	ボード初期化情報表示 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraStatusHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Camera Status Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : camerastatus\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Camera Status Information\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Console ON/OFF
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdConsoleMode (void *str)
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
			cmdConsoleModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Console Mode取得
		if ((status = consoleGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// コンソールモード
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Console Mode設定
		if ((status = consoleSetMode (mode)) != AVAL_STATUS_SUCCESS)
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
//	Console ON/OFF Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdConsoleModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Console mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : console\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : console mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Console mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : console  [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : console mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Console Mode]");
	DEBUG_PRINT_FORCE ("  0 = Message On\n");
	DEBUG_PRINT_FORCE ("  1 = Message Off\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
