//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdLut.c - LUT Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_LUT)
//**********************************************************************************
//	LUT情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lut (void)
{
	int status;
	
	status = cmdLut (NULL);
	return (status);	
}


//**********************************************************************************
//	LUT情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLut (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int mode, select;

	//--------------------------------------------------
	// LUT情報
	//--------------------------------------------------
	DEBUG_PRINT_FORCE ("\n[LUT informations]\n");

	if ((status = lutGetEnable (&select, &mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Mode                      ");
	if (mode & FPGA_LUT_CTRL_ENABLE_BIT)
		DEBUG_PRINT_FORCE ("%d(Enable)\n", mode);
	else
		DEBUG_PRINT_FORCE ("%d(Disable)\n", mode);

	DEBUG_PRINT_FORCE ("   select                    ");
	if (select == LUT_SELECT1)
		DEBUG_PRINT_FORCE ("1\n");
	else
		DEBUG_PRINT_FORCE ("2\n");

_DONE:
	return (status);
}


//**********************************************************************************
//	LUT設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdLutMode (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int lutMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdLutModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// LUTモード取得
		if ((status = lutGetMode (&lutMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", lutMode);
	}
	else if (argc == 2)
	{
		// Mode
		if (sscanf (gCmdArg[1], "%d", &lutMode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// LUTモード設定
		if ((status = lutSetMode (lutMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	LUT設定 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLutModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT Enable Mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : lut-mode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : LUT Mode(Min:%d / Max:%d)\n", LUT_MODE_MIN, LUT_MODE_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT Enable Mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : lut-mode [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : LUT Mode(Min:%d / Max:%d)\n", LUT_MODE_MIN, LUT_MODE_MAX);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[LUT Mode]\n");
	DEBUG_PRINT_FORCE ("  %d = LUT Disable\n", LUT_MODE_DISABLE);
	DEBUG_PRINT_FORCE ("  %d = LUT1 Enable\n", LUT_MODE1);
	DEBUG_PRINT_FORCE ("  %d = LUT2 Enable\n", LUT_MODE2);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	LUTフォーマット設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdLutFormat (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int select, format;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdLutFormatHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// select
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// LUTフォーマット取得
		if ((status = lutGetFormat (select, &format)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", format);
	}
	else if (argc == 3)
	{
		// select
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// format
		if (sscanf (gCmdArg[2], "%d", &format) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// LUT設定
		if ((status = lutSetFormat (select, format)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	LUT設定 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLutFormatHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT Data Format is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : lut-format [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : LUT Select Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : LUT Format\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT Data Format is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : lut-format [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : LUT Select Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : LUT Format\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Select]\n");
	DEBUG_PRINT_FORCE ("  %d = LUT1\n", LUT_SELECT1);
	DEBUG_PRINT_FORCE ("  %d = LUT2\n", LUT_SELECT2);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Format]\n");
	DEBUG_PRINT_FORCE ("  %d : The reading from the Flash Memory\n", LUT_FORMAT_FLASH);
	DEBUG_PRINT_FORCE ("  %d : Increment Data\n", LUT_FORMAT_INC);
	DEBUG_PRINT_FORCE ("  %d : Reverse Data\n", LUT_FORMAT_REV);
	DEBUG_PRINT_FORCE ("  %d : Binarization Data\n", LUT_FORMAT_BIN);
	DEBUG_PRINT_FORCE ("  %d : Gamma\n", LUT_FORMAT_GAMMA);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	LUT 2値化しきい値設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdLutBinThreshold (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int select, threthold;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdLutBinThresholdHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// select
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// LUTしきい値取得
		if ((status = lutGetBinThreshold (select, &threthold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", threthold);
	}
	else if (argc == 3)
	{
		// select
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// threthold
		if (sscanf (gCmdArg[2], "%d", &threthold) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// LUT設定
		if ((status = lutSetBinThreshold (select, threthold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	LUT 2値化しきい値設定Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLutBinThresholdHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT Binarization Threshold is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : binarization/lut-bin [Param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : LUT Select Number\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : LUT Binarization Threshold\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT Binarization Threshold is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : binarization/lut-bin [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : LUT Select Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : LUT Binarization Threshold\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Select]\n");
	DEBUG_PRINT_FORCE ("  %d = LUT1\n", LUT_SELECT1);
	DEBUG_PRINT_FORCE ("  %d = LUT2\n", LUT_SELECT2);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Threshold]\n");

#if (MODE_CAMERA_BIT >= PIXEL_8BIT)
	DEBUG_PRINT_FORCE ("  8bit  : Min:0 - Max:%6d\n", PIXEL_8_SIZE);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_10BIT)
	DEBUG_PRINT_FORCE ("  10bit : Min:0 - Max:%6d\n", PIXEL_10_SIZE);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_12BIT)
	DEBUG_PRINT_FORCE ("  12bit : Min:0 - Max:%6d\n", PIXEL_12_SIZE);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_14BIT)
	DEBUG_PRINT_FORCE ("  14bit : Min:0 - Max:%6d\n", PIXEL_14_SIZE);
#endif

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	LUT ガンマ値設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdLutGamma (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int select;
	float gamma;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdLutGammaHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// select
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// ガンマ取得
		if ((status = lutGetGamma(select, &gamma)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.1f ", gamma);
	}
	else if (argc == 3)
	{
		// select
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// gamma
		if (sscanf (gCmdArg[2], "%f", &gamma) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// ガンマ設定
		if ((status = lutSetGamma (select, gamma)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	LUT ガンマ値設定Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLutGammaHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT gamma value is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : lut-gmma [Param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : LUT Select Number\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : LUT Gamma Value\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT gamma value is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : lut-gamma [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : LUT Select Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : LUT Gamma Value\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Select]\n");
	DEBUG_PRINT_FORCE ("  %d  : LUT1\n", LUT_SELECT1);
	DEBUG_PRINT_FORCE ("  %d  : LUT2\n", LUT_SELECT2);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Gannma]\n");
	DEBUG_PRINT_FORCE ("  Min : %.1f\n", LUT_GAMMA_MIN);
	DEBUG_PRINT_FORCE ("  Max : %.1f\n", LUT_GAMMA_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	LUTテーブルデータ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdLutTableData (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int select;
	unsigned int offset, data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdLutTableDataHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// select
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// offset
		if (sscanf (gCmdArg[2], "%d", &offset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// LUTテーブルデータ取得
		if ((status = lutGetTableData (select, offset, &data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", data);
	}
	else if (argc == 4)
	{
		// select
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// offset
		if (sscanf (gCmdArg[2], "%d", &offset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// data
		if (sscanf (gCmdArg[3], "%d", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// LUTテーブルデータ設定
		if ((status = lutSetTableData (select, offset, data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	LUTテーブルデータ設定Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLutTableDataHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT Table Read Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : lut [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : LUT Select Number\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : LUT Table Address\n");
	DEBUG_PRINT_FORCE ("  Output Param2     : LUT Table Read Data\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT Table Write Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : lut [Param0] [Param1] [Param2]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : LUT Select Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : LUT Table Address\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : LUT Table Write Data\n");
	DEBUG_PRINT_FORCE ("  Output Param        none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Select]\n");
	DEBUG_PRINT_FORCE ("  %d = LUT1\n", LUT_SELECT1);
	DEBUG_PRINT_FORCE ("  %d = LUT2\n", LUT_SELECT2);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[LUT Table Address/Data]\n");

#if (MODE_CAMERA_BIT >= PIXEL_8BIT)
	DEBUG_PRINT_FORCE ("  8bit  : Min:0 - Max:%6d\n", PIXEL_8_SIZE-1);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_10BIT)
	DEBUG_PRINT_FORCE ("  10bit : Min:0 - Max:%6d\n", PIXEL_10_SIZE-1);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_12BIT)
	DEBUG_PRINT_FORCE ("  12bit : Min:0 - Max:%6d\n", PIXEL_12_SIZE-1);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_14BIT)
	DEBUG_PRINT_FORCE ("  14bit : Min:0 - Max:%6d\n", PIXEL_14_SIZE-1);
#endif

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	LUTテーブルデータ保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdLutSave (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int select;
	unsigned int adrs;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdLutSaveHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// select
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check select Parameter
		if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "LUT Save select(%d) Parameter Error. (Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// LUTテーブルアドレス取得
		if ((status = lutGetMemAdrs (select, &adrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// LUTテーブルデータ保存
		if ((status = lutSetFlash (select, (unsigned int *)adrs, LUT_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	LUTテーブルデータ保存Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLutSaveHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT Data is saved for the Flash Memory.\n");
	DEBUG_PRINT_FORCE ("  Command           : lut-save [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : LUT Select Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Select]\n");
	DEBUG_PRINT_FORCE ("  %d = LUT1\n", LUT_SELECT1);
	DEBUG_PRINT_FORCE ("  %d = LUT2\n", LUT_SELECT2);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	LUT Default
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdLutDefault (void *str)
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
			cmdLutDefaultHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// LUT Default
		if ((status = lutDefault ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	LUT Default Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLutDefaultHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The LUT Data is default.\n");
	DEBUG_PRINT_FORCE ("  Command           : lut-default\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if defined (MODE_LUT)

// eof
