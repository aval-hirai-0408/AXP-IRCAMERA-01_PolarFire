//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014-2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdFfcAdjust.c - FFC Adjust Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_FFC)
//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int ffcAdjustAllFlag;			// FFC Adjust All Flag


//**********************************************************************************
//	FFC Adjust Info
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustInfo (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustInfoHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFC Number取得
		if (sscanf (gCmdArg[1], "%d", &ffcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}
		
		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if (corMode == FFC_CORRECTION_MODE_FIRST)
		{
			// Check ffcNum Parameter
			if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "FFC Adjust Information ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// FFC Adjust情報表示
			if ((status = ffcAdjustInfo (ffcNum, FFC_USER)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		//--------------------------------------------------------------------------------
		// Shading Line補正
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
		{
			// Check ffcNum Parameter
			if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "FFC Adjust Information ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// FFC Adjust情報表示
			if ((status = ffcAdjustInfoShadingLine (ffcNum, FFC_USER)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Info Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustInfoHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustinfo\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Number (Min:%d - Max:%d)\n", FFC_NUMBER_MIN, FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Adjust Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Adjust Info(Admin)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustInfoAdmin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustInfoAdminHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFC Number取得
		if (sscanf (gCmdArg[1], "%d", &ffcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if (corMode == FFC_CORRECTION_MODE_FIRST)
		{
			// Check ffcNum Parameter
			if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "FFC Adjust Information ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// FFC Adjust情報表示
			if ((status = ffcAdjustInfo (ffcNum, FFC_USER)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		//--------------------------------------------------------------------------------
		// Shading Line補正
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
		{
			// Check ffcNum Parameter
			if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "FFC Adjust Information ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// FFC Adjust情報表示
			if ((status = ffcAdjustInfoShadingLine (ffcNum, FFC_USER)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Adjust Info Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustInfoAdminHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustinfo-admin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Adjust Information\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n", FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Adjust Info Rate取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustRate (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	float rate;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustRateHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整Rate取得
		if ((status = ffcGetAdjustRate (mode, &rate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", rate);
	}
	else if (argc == 3)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// rate取得
		if (sscanf (gCmdArg[2], "%f", &rate) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整Rate設定
		if ((status = ffcSetAdjustRate (mode, rate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Info Rate取得 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustRateHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Rate is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustrate [Param] \n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Adjust Rate\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Rate is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustrate [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : FFC Adjust Rate\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Black Level\n", FFC_ADJUST_BLACK);
	DEBUG_PRINT_FORCE ("  %d : White Level\n", FFC_ADJUST_WHITE);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Rate]\n");
	DEBUG_PRINT_FORCE ("  Min : %.2f\n", FRAMERATE_MIN);
	DEBUG_PRINT_FORCE ("  Max : %.2f\n", FRAMERATE_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Adjust Info Exposure Time取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustExp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	int exp;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustExpHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整Exposure Time取得
		if ((status = ffcGetAdjustExposure (mode, &exp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", exp);
	}
	else if (argc == 3)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// exp取得
		if (sscanf (gCmdArg[2], "%d", &exp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整Exposure Time設定
		if ((status = ffcSetAdjustExposure (mode, exp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Info Exposure Time取得 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustExpHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Exposure Time is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustexp [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Adjust Exposure Time\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Exposure Time is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustexp [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : FFC Adjust Exposure Time\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Black Level\n", FFC_ADJUST_BLACK);
	DEBUG_PRINT_FORCE ("  %d : White Level\n", FFC_ADJUST_WHITE);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Exposure Time]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", MIN_EXPOSURE_TIME);
	DEBUG_PRINT_FORCE ("  Max : %d\n", MAX_EXPOSURE_TIME);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Adjust Info Target
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustTarget (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	int target;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustTargetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整Target 取得
		if ((status = ffcGetAdjustTarget (mode, &target)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", target);
	}
	else if (argc == 3)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Target取得
		if (sscanf (gCmdArg[2], "%d", &target) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整Target 設定
		if ((status = ffcSetAdjustTarget (mode, target)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Info Target Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustTargetHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Target is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjusttarget [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Adjust Target\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Target is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjusttarget [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : FFC Adjust Target\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Black Level\n", FFC_ADJUST_BLACK);
	DEBUG_PRINT_FORCE ("  %d : White Level\n", FFC_ADJUST_WHITE);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Target]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", PIXEL_14_MASK);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Adjust Info 温度
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustTemp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	float temp;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustTempHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整温度取得
		if ((status = ffcGetAdjustTemp (mode, &temp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", temp);
	}
	else if (argc == 3)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Temp取得
		if (sscanf (gCmdArg[2], "%f", &temp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整温度設定
		if ((status = ffcSetAdjustTemp (mode, temp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Info 温度 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustTempHelp (void *str)
{
	//double tempMax, tempMin;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Temperature is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjusttemp [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Adjust Temperature\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Temperature is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjusttemp [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : FFC Adjust Temperature\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Black Level\n", FFC_ADJUST_BLACK);
	DEBUG_PRINT_FORCE ("  %d : White Level\n", FFC_ADJUST_WHITE);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Temp]\n");
	DEBUG_PRINT_FORCE ("  Min : %.2f\n", PELTIER_SENSOR_ASJUST_TEMP_MIN);
	DEBUG_PRINT_FORCE ("  Max : %.2f\n", PELTIER_SENSOR_ASJUST_TEMP_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Adjust Info Gain
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustGainX (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	float gain;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustGainXHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整Gain取得
		if ((status = ffcGetAdjustGainX (mode, &gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", gain);
	}
	else if (argc == 3)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Gain取得
		if (sscanf (gCmdArg[2], "%f", &gain) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整Gain設定
		if ((status = ffcSetAdjustGainX (mode, gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Info Gain Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustGainXHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Gain is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustgain [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Adjust Gain\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Gain is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustgain [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : FFC Adjust Gain\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Black Level\n", FFC_ADJUST_BLACK);
	DEBUG_PRINT_FORCE ("  %d : White Level\n", FFC_ADJUST_WHITE);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("FFC Gain (magnification unit)]\n");
	DEBUG_PRINT_FORCE ("  Min : %.2f\n", DOG_GAIN_MIN);
	DEBUG_PRINT_FORCE ("  Max : %.2f\n", DOG_GAIN_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Adjust Info Bit
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBit (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	int bit;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustBitHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整Bit取得
		if ((status = ffcGetAdjustBit (mode, &bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", bit);
	}
	else if (argc == 3)
	{
		// FFCモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Bit取得
		if (sscanf (gCmdArg[2], "%d", &bit) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC調整Bit設定
		if ((status = ffcSetAdjustBit (mode, bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Info Bit Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBitHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Bit is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustbit [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Adjust Bit\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Adjust Bit is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustbit [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : FFC Mode\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : FFC Adjust Bit\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Black Level\n", FFC_ADJUST_BLACK);
	DEBUG_PRINT_FORCE ("  %d : White Level\n", FFC_ADJUST_WHITE);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[bit]\n");
#if (MODE_CAMERA_BIT >= PIXEL_8BIT)
	DEBUG_PRINT_FORCE ("   8 : 8bit\n");
#endif
#if (MODE_CAMERA_BIT >= PIXEL_10BIT)
	DEBUG_PRINT_FORCE ("  10 : 10bit\n");
#endif
#if (MODE_CAMERA_BIT >= PIXEL_12BIT)
	DEBUG_PRINT_FORCE ("  12 : 12bit\n");
#endif
#if (MODE_CAMERA_BIT >= PIXEL_14BIT)
	DEBUG_PRINT_FORCE ("  14 : 14bit\n");
#endif
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Blackコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchBlack (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int expTime;
	unsigned int blackTarget, bit, ffcNo;
	int saveLed0 = -1;
	FFC_PARAM ffcParam;
	double sensorTemp;
	double frameRate;
	int factoryMode;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustBatchBlackHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 6)
	{
		// FFC設定ステート
		saveLed0 = ledSettingState ();

		// FFC番号
		if (sscanf (gCmdArg[1], "%d", &ffcNo) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// フレームレート取得
		if (sscanf (gCmdArg[2], "%lf", &frameRate) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 露光時間取得
		if (sscanf (gCmdArg[3], "%d", &expTime) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 黒レベル目標値取得
		if (sscanf (gCmdArg[4], "%d", &blackTarget) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 画素ビット数取得
//		bit = atoi (gCmdArg[4]);
		bit = 14;

		// センサ温度
		if (sscanf (gCmdArg[5], "%lf", &sensorTemp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check ffcNo Parameter
		if ((ffcNo < FFC_NUMBER_MIN) || (ffcNo > FFC_NUMBER_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Black ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		//--------------------------------------------------------------------------------
		// Black Image
		//--------------------------------------------------------------------------------

		// クリア
		memset ((void *)&ffcParam, 0, sizeof(ffcParam));

		// パラメータ設定
		ffcParam.ffcNo = ffcNo;
		//@@@1ffcParam.mode = MODE_BATCH;
		ffcParam.frameRate = frameRate;
		ffcParam.exposure = expTime;
		ffcParam.target = blackTarget;
		ffcParam.bit = bit;
		ffcParam.sensorTemp = sensorTemp;
		ffcParam.memType = FFC_MEMORY_EXT;
		ffcParam.userMode = FFC_USER;

		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC Black

		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if (corMode == FFC_CORRECTION_MODE_FIRST)
		{
			if ((status = ffcBlack (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		//--------------------------------------------------------------------------------
		// Shading Line補正
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
		{
			if ((status = ffcBlack (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}


//**********************************************************************************
//	FFC Blackコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchBlackHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int targetMax;
	double rMin, rMax;
	int bit;
	int readOutTime;
#ifdef MODE_PELTIER
	double tempMin, tempMax;

	// Peltier Temp Min/Max取得
	if ((status = peltierGetTargetMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC offset is adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustblackbatch [param0] [param1] [param2] [param3] [param4]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : FFC Number (Min:%d - Max:%d)\n", FFC_NUMBER_MIN+1, FFC_NUMBER_MAX);

	// 最小レート時間取得
	if ((status = rateMin (&rMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大レート時間取得
	if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("  Input  Param1     : FrameRate[fps](Min:%.2f - Max:%.2f)\n", rMin, rMax);

	// Read Out時間取得
	if ((status = sensoreGetReadOut (&readOutTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("  Input  Param2     : Exposure Time[us] (Min:%d - Max:(FrameRate Time(us) - %d(us)))\n", MIN_EXPOSURE_TIME, readOutTime);


	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (bit == PIXEL_8BIT)
		targetMax = FFC_BLACK_LEVEL_BIT8_MAX;
	else if (bit == PIXEL_10BIT)
		targetMax = FFC_BLACK_LEVEL_BIT10_MAX;
	else if (bit == PIXEL_12BIT)
		targetMax = FFC_BLACK_LEVEL_BIT12_MAX;
	else
		targetMax = FFC_BLACK_LEVEL_BIT14_MAX;

	DEBUG_PRINT_FORCE ("  Input  Param3     : Black Level Target (Min:%d - Max:%d)\n", 0, targetMax);

#ifdef MODE_PELTIER
	DEBUG_PRINT_FORCE ("  Input  Param4     : Sensor Temp (Min:%.2f - Max:%.2f)\n", 0, tempMin, tempMax);
#else
	DEBUG_PRINT_FORCE ("  Input  Param4     : Reserved\n" );
#endif
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Whiteコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchWhite (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int expTime;
	unsigned int whiteTarget, bit, ffcNo;
	int saveLed0 = -1;
	FFC_PARAM ffcParam;
	double sensorTemp;
	unsigned int mark;
	int dpcNum;
	int factoryMode;
	double frameRate;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustBatchWhiteHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 6)
	{
		// FFC設定ステート
		saveLed0 = ledSettingState ();

		// FFC番号
		if (sscanf (gCmdArg[1], "%d", &ffcNo) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// フレームレート取得
		if (sscanf (gCmdArg[2], "%lf", &frameRate) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 露光時間取得
		if (sscanf (gCmdArg[3], "%d", &expTime) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 白レベル目標値取得
		if (sscanf (gCmdArg[4], "%d", &whiteTarget) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 画素ビット数取得
		//bit = atoi (gCmdArg[4]);
		bit = 14;

		// センサ温度
		if (sscanf (gCmdArg[5], "%lf", &sensorTemp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check ffcNo Parameter
		if ((ffcNo < FFC_NUMBER_MIN) || (ffcNo > FFC_NUMBER_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Black ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		//--------------------------------------------------------------------------------
		// White Image
		//--------------------------------------------------------------------------------

		// クリア
		memset ((void *)&ffcParam, 0, sizeof(ffcParam));

		// パラメータ設定
		ffcParam.ffcNo = ffcNo;
		//@@@1ffcParam.mode = MODE_BATCH;
		ffcParam.frameRate = frameRate;
		ffcParam.exposure = expTime;
		ffcParam.target = whiteTarget;
		ffcParam.bit = bit;
		ffcParam.sensorTemp = sensorTemp;
		ffcParam.memType = FFC_MEMORY_EXT;
		ffcParam.userMode = FFC_USER;

		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// FFC White

		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if (corMode == FFC_CORRECTION_MODE_FIRST)
		{
			if ((status = ffcWhite (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Get DPC Number
			if ((status = dpcGetLoadNum (&dpcNum)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// DPCマークCheck
			if ((status = dpcGetMarkFlash (dpcNum, &mark)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// DPCマークは有効?
			if ((mark != DPC_NO_DATA) && (mark != 0))
			{
				dpcSetEnableMode (MODE_ENABLE);
			}
		}
		//--------------------------------------------------------------------------------
		// Shading Line補正
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
		{
			if ((status = ffcWhite (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Get DPC Number
			if ((status = dpcGetLoadNum (&dpcNum)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// DPCマークCheck
			if ((status = dpcGetMarkFlash (dpcNum, &mark)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// DPCマークは有効?
			if ((mark != DPC_NO_DATA) && (mark != 0))
			{
				dpcSetEnableMode (MODE_ENABLE);
			}
		}
#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}


//**********************************************************************************
//	FFC Whiteコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchWhiteHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int targetMax;
	double rMin, rMax;
	int bit;
	int readOutTime;
#ifdef MODE_PELTIER
	double tempMin, tempMax;

	// Peltier Temp Min/Max取得
	if ((status = peltierGetTargetMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC Gain is adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustwhitebatch [param0] [param1] [param2] [param3] [param4]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : FFC Number (Min:%d - Max:%d)\n", FFC_NUMBER_MIN+1, FFC_NUMBER_MAX);

	// 最小レート時間取得
	if ((status = rateMin (&rMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大レート時間取得
	if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Read Out時間取得
	if ((status = sensoreGetReadOut (&readOutTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("  Input  Param1     : FrameRate[fps](Min:%.2f - Max:%.2f)\n", rMin, rMax);
	DEBUG_PRINT_FORCE ("  Input  Param2     : Exposure Time[us] (Min:%d - Max:(FrameRate Time(us) - %d(us)))\n", MIN_EXPOSURE_TIME, readOutTime);

	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (bit == PIXEL_8BIT)
		targetMax = FFC_WHITE_LEVEL_BIT8_MAX;
	else if (bit == PIXEL_10BIT)
		targetMax = FFC_WHITE_LEVEL_BIT10_MAX;
	else if (bit == PIXEL_12BIT)
		targetMax = FFC_WHITE_LEVEL_BIT12_MAX;
	else
		targetMax = FFC_WHITE_LEVEL_BIT14_MAX;

	DEBUG_PRINT_FORCE ("  Input  Param3     : White Level Target (Min:%d - Max:%d)\n", 0, targetMax);

#ifdef MODE_PELTIER
	DEBUG_PRINT_FORCE ("  Input  Param4     : Sensor Temp (Min:%.2f - Max:%.2f)\n", 0, tempMin, tempMax);
#else
	DEBUG_PRINT_FORCE ("  Input  Param4     : Reserved\n" );
#endif
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

_DONE:
	return (status);
}


#if defined (MODE_FFC_SHADING_LINE)
//**********************************************************************************
//	FFC Shadingコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchShading (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int expTime;
	unsigned int whiteTarget, bit, ffcNo;
	int saveLed0 = -1;
	FFC_PARAM ffcParam;
	double sensorTemp;
	double frameRate;
	unsigned int mark;
	int dpcNum;
	int factoryMode;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustBatchShadingHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 6)
	{
		// FFC設定ステート
		saveLed0 = ledSettingState ();

		// FFC番号
		if (sscanf (gCmdArg[1], "%d", &ffcNo) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// フレームレート取得
		if (sscanf (gCmdArg[2], "%lf", &frameRate) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 露光時間取得
		if (sscanf (gCmdArg[3], "%d", &expTime) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 白レベル目標値取得
		if (sscanf (gCmdArg[4], "%d", &whiteTarget) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 画素ビット数取得
		//bit = atoi (gCmdArg[4]);
		bit = 14;

		// センサ温度
		if (sscanf (gCmdArg[5], "%lf", &sensorTemp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check ffcNo Parameter
		if ((ffcNo < FFC_NUMBER_MIN) || (ffcNo > FFC_NUMBER_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Black ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		//--------------------------------------------------------------------------------
		// White Image
		//--------------------------------------------------------------------------------

		// クリア
		memset ((void *)&ffcParam, 0, sizeof(ffcParam));

		// パラメータ設定
		ffcParam.ffcNo = ffcNo;
		//@@@1ffcParam.mode = MODE_BATCH;
		ffcParam.frameRate = frameRate;
		ffcParam.exposure = expTime;
		ffcParam.target = 0;
		ffcParam.target = whiteTarget;
		ffcParam.bit = bit;
		ffcParam.sensorTemp = sensorTemp;
		ffcParam.memType = FFC_MEMORY_EXT;
		ffcParam.userMode = FFC_USER;

		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//--------------------------------------------------------------------------------
		// Shading Line
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
		{
			if ((status = ffcShadingLine (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Get DPC Number
			if ((status = dpcGetLoadNum (&dpcNum)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// DPCマークCheck
			if ((status = dpcGetMarkFlash (dpcNum, &mark)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// DPCマークは有効?
			if ((mark != DPC_NO_DATA) && (mark != 0))
			{
				dpcSetEnableMode (MODE_ENABLE);
			}
		}
	}
	else
#endif // #if defined (FFC_CORRECTION_MODE_SHADING_LINE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}


//**********************************************************************************
//	FFC Shadingコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchShadingHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int targetMax;
	double rMin, rMax;
	int readOutTime;
#ifdef MODE_PELTIER
	double tempMin, tempMax;

	// Peltier Temp Min/Max取得
	if ((status = peltierGetTargetMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC Gain is adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustshadingbatch [param0] [param1] [param2] [param3] [param4]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : FFC Number (Min:%d - Max:%d)\n", FFC_NUMBER_MIN+1, FFC_NUMBER_MAX);

	// 最小レート時間取得
	if ((status = rateMin (&rMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大レート時間取得
	if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Read Out時間取得
	if ((status = sensoreGetReadOut (&readOutTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("  Input  Param1     : FrameRate[fps](Min:%.2f - Max:%.2f)\n", rMin, rMax);
	DEBUG_PRINT_FORCE ("  Input  Param2     : Exposure Time[us] (Min:%d - Max:(FrameRate Time(us) - %d(us)))\n", MIN_EXPOSURE_TIME, readOutTime);

	if (MODE_FFC_BIT == PIXEL_8BIT)
		targetMax = FFC_WHITE_LEVEL_BIT8_MAX;
	else if (MODE_FFC_BIT == PIXEL_10BIT)
		targetMax = FFC_WHITE_LEVEL_BIT10_MAX;
	else if (MODE_FFC_BIT == PIXEL_12BIT)
		targetMax = FFC_WHITE_LEVEL_BIT12_MAX;
	else
		targetMax = FFC_WHITE_LEVEL_BIT14_MAX;

	DEBUG_PRINT_FORCE ("  Input  Param3     : White Level Target (Min:%d - Max:%d)\n", 0, targetMax);

#ifdef MODE_PELTIER
	DEBUG_PRINT_FORCE ("  Input  Param4     : Sensor Temp (Min:%.2f - Max:%.2f)\n", 0, tempMin, tempMax);
#else
	DEBUG_PRINT_FORCE ("  Input  Param4     : Reserved\n" );
#endif
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

_DONE:
	return (status);
}
#endif // #if defined (FFC_CORRECTION_MODE_SHADING_LINE)


#if defined (MODE_FFC_SHADING_LINE)
//**********************************************************************************
//	FFC White Gainxコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcWhiteGainX (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double gain;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcWhiteGainXHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// FFC White Gainx取得
		if ((status = ffcGetWhiteGainX (&gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.1f ", gain);
	}
	else if (argc == 2)
	{
		// FFC番号
		if (sscanf (gCmdArg[1], "%lf", &gain) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC White Gainx設定
		if ((status = ffcSetWhiteGainX (gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Shadingコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcWhiteGainXHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC white gain is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcwhitegainx [param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC White Gain (Min:%.1lf - Max:%.1lf)\n", FFC_WHITE_GAIN_MIN, FFC_WHITE_GAIN_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC white gain is adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcwhitegainx\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC White Gain (Min:%.1lf - Max:%.1lf)\n", FFC_WHITE_GAIN_MIN, FFC_WHITE_GAIN_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}
#endif // #if defined (FFC_CORRECTION_MODE_SHADING_LINE)


//**********************************************************************************
//	FFC Blackコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchBlackAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int expTime;
	unsigned int blackTarget, bit, ffcNo;
	int saveLed0 = -1;
	FFC_PARAM ffcParam;
	double sensorTemp;
	double frameRate;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustBatchBlackAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 6)
	{
		// FFC設定ステート
		saveLed0 = ledSettingState ();

		// FFC番号
		if (sscanf (gCmdArg[1], "%d", &ffcNo) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// フレームレート取得
		if (sscanf (gCmdArg[2], "%lf", &frameRate) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 露光時間取得
		if (sscanf (gCmdArg[3], "%d", &expTime) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 黒レベル目標値取得
		if (sscanf (gCmdArg[4], "%d", &blackTarget) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 画素ビット数取得
		bit = MODE_FFC_BIT;

		// センサ温度
		if (sscanf (gCmdArg[5], "%lf", &sensorTemp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check ffcNo Parameter
		if ((ffcNo < FFC_NUMBER_MIN) || (ffcNo > FFC_NUMBER_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Black ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		//--------------------------------------------------
		// All Mode
		//--------------------------------------------------
		ffcAdjustAllFlag = 1;

		//--------------------------------------------------------------------------------
		// Black Image
		//--------------------------------------------------------------------------------

		// クリア
		memset ((void *)&ffcParam, 0, sizeof(ffcParam));

		// パラメータ設定
		ffcParam.ffcNo = ffcNo;
		//@@@1ffcParam.mode = MODE_BATCH;
		ffcParam.frameRate = frameRate;
		ffcParam.exposure = expTime;
		ffcParam.target = blackTarget;
		ffcParam.bit = bit;
		ffcParam.sensorTemp = sensorTemp;
		ffcParam.memType = FFC_MEMORY_INT;
		ffcParam.userMode = FFC_ADMIN;

		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC Black

		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if (corMode == FFC_CORRECTION_MODE_FIRST)
		{
			if ((status = ffcBlack (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		//--------------------------------------------------------------------------------
		// Shading Line補正
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
		{
			if ((status = ffcBlack (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

	// FFC無効
	ffcSetMode (0, 0);

_DONE:
	// All Mode Clear
	ffcAdjustAllFlag = 0;

	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


//**********************************************************************************
//	FFC Blackコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchBlackAllHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int targetMax;
#ifdef MODE_PELTIER
	double tempMin, tempMax;

	// Peltier Temp Min/Max取得
	if ((status = peltierGetTargetMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC offset is all adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustblackbatchall [param0] [param1] [param2] [param3] [param4]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Reserved\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : FrameRate[fps](Min:%.2f - Max:%.2f)\n", FRAMERATE_MIN, FRAMERATE_MAX);
	DEBUG_PRINT_FORCE ("  Input  Param2     : Reserved\n");

	if (MODE_FFC_BIT == PIXEL_8BIT)
		targetMax = FFC_BLACK_LEVEL_BIT8_MAX;
	else if (MODE_FFC_BIT == PIXEL_10BIT)
		targetMax = FFC_BLACK_LEVEL_BIT10_MAX;
	else if (MODE_FFC_BIT == PIXEL_12BIT)
		targetMax = FFC_BLACK_LEVEL_BIT12_MAX;
	else
		targetMax = FFC_BLACK_LEVEL_BIT14_MAX;

	DEBUG_PRINT_FORCE ("  Input  Param3     : Black Level Pixel (Min:%d - Max:%d)\n", 0, targetMax);
#ifdef MODE_PELTIER
	DEBUG_PRINT_FORCE ("  Input  Param4     : Sensor Temp (Min:%.2f - Max:%.2f)\n", 0, tempMin, tempMax);
#else
	DEBUG_PRINT_FORCE ("  Input  Param4     : Reserved\n" );
#endif
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

#ifdef MODE_PELTIER
_DONE:
#endif
	return (status);
}


//**********************************************************************************
//	FFC Whiteコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchWhiteAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int expTime;
	unsigned int whiteTarget, bit, ffcNo;
	int saveLed0 = -1;
	FFC_PARAM ffcParam;
	double sensorTemp;
	double frameRate;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustBatchWhiteAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 6)
	{
		// FFC設定ステート
		saveLed0 = ledSettingState ();

		// FFC番号
		if (sscanf (gCmdArg[1], "%d", &ffcNo) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// フレームレート取得
		if (sscanf (gCmdArg[2], "%lf", &frameRate) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 露光時間取得
		if (sscanf (gCmdArg[3], "%d", &expTime) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 白レベル目標値取得
		if (sscanf (gCmdArg[4], "%d", &whiteTarget) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 画素ビット数取得
		bit = MODE_FFC_BIT;

		// センサ温度
		if (sscanf (gCmdArg[5], "%lf", &sensorTemp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check ffcNo Parameter
		if ((ffcNo < FFC_NUMBER_MIN) || (ffcNo > FFC_NUMBER_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC White ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		//--------------------------------------------------
		// All Mode
		//--------------------------------------------------
		ffcAdjustAllFlag = 1;

		//--------------------------------------------------------------------------------
		// White Image
		//--------------------------------------------------------------------------------

		// クリア
		memset ((void *)&ffcParam, 0, sizeof(ffcParam));

		// パラメータ設定
		ffcParam.ffcNo = ffcNo;
		//@@@1ffcParam.mode = MODE_BATCH;
		ffcParam.frameRate = frameRate;
		ffcParam.exposure = expTime;
		ffcParam.target = whiteTarget;
		ffcParam.bit = bit;
		ffcParam.sensorTemp = sensorTemp;
		ffcParam.memType = FFC_MEMORY_INT;
		ffcParam.userMode = FFC_ADMIN;

		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if (corMode == FFC_CORRECTION_MODE_FIRST)
		{
			// FFC White
			if ((status = ffcWhite (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// FFC有効
			ffcSetMode (FFC_ENABLE, FFC_ENABLE);
		}
		//--------------------------------------------------------------------------------
		// Shading Line補正
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
		{
			// FFC White
			if ((status = ffcWhite (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// FFC有効
			ffcSetMode (FFC_ENABLE, FFC_ENABLE);
		}
#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// All Mode Clear
	ffcAdjustAllFlag = 0;

	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}


//**********************************************************************************
//	FFC Whiteコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchWhiteAllHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int min, max;
	int targetMax;
#ifdef MODE_PELTIER
	double tempMin, tempMax;

	// Peltier Temp Min/Max取得
	if ((status = peltierGetTargetMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC Gain is all adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustwhitebatch [param0] [param1] [param2] [param3] [param4]\n");

	min = FFC_NUMBER_MIN;
	max = FFC_NUMBER_MAX;
	DEBUG_PRINT_FORCE ("  Input  Param0     : FFC Number (Min:%d - Max:%d)\n", min, max);
	DEBUG_PRINT_FORCE ("  Input  Param1     : FrameRate[fps](Min:%.2f - Max:%.2f)\n", FRAMERATE_MIN, FRAMERATE_MAX);
	DEBUG_PRINT_FORCE ("  Input  Param2     : Reserved\n");

	if (MODE_FFC_BIT == PIXEL_8BIT)
		targetMax = FFC_WHITE_LEVEL_BIT8_MAX;
	else if (MODE_FFC_BIT == PIXEL_10BIT)
		targetMax = FFC_WHITE_LEVEL_BIT10_MAX;
	else if (MODE_FFC_BIT == PIXEL_12BIT)
		targetMax = FFC_WHITE_LEVEL_BIT12_MAX;
	else
		targetMax = FFC_WHITE_LEVEL_BIT14_MAX;

	DEBUG_PRINT_FORCE ("  Input  Param3     : White Level Pixel (Min:%d - Max:%d)\n", 0, targetMax);

#ifdef MODE_PELTIER
	DEBUG_PRINT_FORCE ("  Input  Param4     : Sensor Temp (Min:%.2f - Max:%.2f)\n", 0, tempMin, tempMax);
#else
	DEBUG_PRINT_FORCE ("  Input  Param4     : Reserved\n" );
#endif
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

_DONE:
	return (status);
}


#if defined (MODE_FFC_SHADING_LINE)
//**********************************************************************************
//	FFC Shadingコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchShadingAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int shadingTarget, bit, ffcNo;
	int saveLed0 = -1;
	FFC_PARAM ffcParam;
	double sensorTemp;
	unsigned int expTime;
	double frameRate;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcAdjustBatchShadingAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 6)
	{
		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		if (corMode != FFC_CORRECTION_MODE_SHADING_LINE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
			goto _DONE;
		}

		// FFC設定ステート
		saveLed0 = ledSettingState ();

		// FFC番号
		if (sscanf (gCmdArg[1], "%d", &ffcNo) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// フレームレート取得
		if (sscanf (gCmdArg[2], "%lf", &frameRate) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 露光時間取得
		if (sscanf (gCmdArg[3], "%d", &expTime) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Shadingレベル目標値取得
		if (sscanf (gCmdArg[4], "%d", &shadingTarget) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 画素ビット数取得
		bit = MODE_FFC_BIT;

		// センサ温度
		if (sscanf (gCmdArg[5], "%lf", &sensorTemp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		if ((ffcNo < FFC_NUMBER_MIN) || (ffcNo > FFC_NUMBER_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Shading ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
		
		//--------------------------------------------------
		// All Mode
		//--------------------------------------------------
		ffcAdjustAllFlag = 1;

		//--------------------------------------------------------------------------------
		// Shading Image
		//--------------------------------------------------------------------------------

		// クリア
		memset ((void *)&ffcParam, 0, sizeof(ffcParam));

		// パラメータ設定
		ffcParam.ffcNo = ffcNo;
		//@@@1ffcParam.mode = MODE_BATCH;
		ffcParam.frameRate = frameRate;
		ffcParam.exposure = expTime;
		ffcParam.target = shadingTarget;
		ffcParam.bit = bit;
		ffcParam.sensorTemp = sensorTemp;
		ffcParam.memType = FFC_MEMORY_INT;
		ffcParam.userMode = FFC_ADMIN;

		if ((status = ffcShadingLine (ffcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// All Mode Clear
	ffcAdjustAllFlag = 0;

	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}


//**********************************************************************************
//	FFC Shadingコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcAdjustBatchShadingAllHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
#ifdef MODE_PELTIER
	double tempMin, tempMax;

	// Peltier Temp Min/Max取得
	if ((status = peltierGetTargetMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC Shading Gain is all adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcadjustshadingbatch [param0] [param1] [param2] [param3] [param4]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : FFC Number (Min:%d - Max:%d)\n", FFC_NUMBER_MIN, FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Input  Param1     : FrameRate[fps](Min:%.2f - Max:%.2f)\n", FRAMERATE_MIN, FRAMERATE_MAX);
	DEBUG_PRINT_FORCE ("  Input  Param2     : Reserved\n");
	DEBUG_PRINT_FORCE ("  Input  Param3     : White Level Pixel (Min:%d - Max:%d)\n", 0, FFC_WHITE_LEVEL_BIT14_MAX);
#ifdef MODE_PELTIER
	DEBUG_PRINT_FORCE ("  Input  Param4     : Sensor Temp (Min:%.2f - Max:%.2f)\n", 0, tempMin, tempMax);
#else
	DEBUG_PRINT_FORCE ("  Input  Param4     : Reserved\n" );
#endif
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

#ifdef MODE_PELTIER
_DONE:
#endif
	return (status);
}
#endif // #if defined (MODE_FFC_SHADING_LINE)

#endif // #if defined (MODE_FFC)

// eof
