//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdAutoBright.c - Auto Bright Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
//**********************************************************************************
//	Auto Bright Exposure Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightExposureMode (void *str)
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
			cmdAutoBrightExposureModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// mode取得
		if ((status = autoBrightGetExposureMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// mode設定
		if ((status = autoBrightSetExposureMode (mode)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Exposure Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightExposureModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright exposure mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : exposureauto\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright exposure mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : exposureauto [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE (" 0 : Disable\n");
	DEBUG_PRINT_FORCE (" 1 : Continue\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Auto Bright Exposure Once Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightExposureStatus (void *str)
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
			cmdAutoBrightExposureStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Once Status取得
		if ((status = autoBrightGetExposureStatus (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", mode);
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
//	Auto Bright Exposure Once Status Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightExposureStatusHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright exposure status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : exposureautostatus\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : status\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Status]\n");
	DEBUG_PRINT_FORCE (" 0 : Active\n");
	DEBUG_PRINT_FORCE (" 1 : Complete\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Auto Bright Exposure Min/Max
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightExposureMax (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int expMaxTime;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAutoBrightExposureMaxHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Exposure Max取得
		if ((status = autoBrightGetExposureMax (&expMaxTime)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", expMaxTime);
	}
	else if (argc == 2)
	{
		// Exposure Max取得
		if (sscanf (gCmdArg[1], "%d", &expMaxTime) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Exposure Max設定
		if ((status = autoBrightSetExposureMax (expMaxTime)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Exposure Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightExposureMaxHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int  getExpTimeMax;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright exposure max is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : exposureautomax\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : [exposute maximum]\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright exposure max is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : exposureautomax [param0]\n");
	DEBUG_PRINT_FORCE ("  Input0 Param      : exposure maximum\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n\n");

	// 最大露光時間取得
	if ((status = exposureMax (&getExpTimeMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("[Exposure Max]\n");
	DEBUG_PRINT_FORCE (" Max : %d\n", getExpTimeMax);
	DEBUG_PRINT_FORCE ("\n");

_DONE:
	return (status);
}


//**********************************************************************************
//	Auto Bright Gain Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightGainMode (void *str)
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
			cmdAutoBrightGainModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// mode取得
		if ((status = autoBrightGetGainMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// mode設定
		if ((status = autoBrightSetGainMode (mode)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Gain Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightGainModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright gain mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gainauto\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright gain mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : gainauto [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE (" 0 : Disable\n");
	DEBUG_PRINT_FORCE (" 1 : Continue\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Auto Bright Gain Once Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightGainStatus (void *str)
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
			cmdAutoBrightGainStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Once Status取得
		if ((status = autoBrightGetGainStatus (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", mode);
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
//	Auto Bright Gain Once Status Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightGainStatusHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright gain status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gainautostatus\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : status\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Status]\n");
	DEBUG_PRINT_FORCE (" 0 : Active\n");
	DEBUG_PRINT_FORCE (" 1 : Complete\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Auto Bright Gain Min/Max
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightGainMax (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double gainMax;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAutoBrightGainMaxHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Gain Max取得
		if ((status = autoBrightGetGainMax (&gainMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%.1lf ", gainMax);
	}
	else if (argc == 2)
	{
		// Gain Max取得
		if (sscanf (gCmdArg[1], "%lf", &gainMax) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}
		
		// Gain Max設定
		if ((status = autoBrightSetGainMax (gainMax)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Gain Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightGainMaxHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright gain max is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gainautomax\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : [gain maximum]\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright gain max is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : gainautomax [param0]\n");
	DEBUG_PRINT_FORCE ("  Input0 Param      : gain maximum\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n\n");

	DEBUG_PRINT_FORCE ("[Gain Max]\n");
	DEBUG_PRINT_FORCE (" Max : %lf\n", DOG_GAIN_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}


//**********************************************************************************
//	Auto Bright Overlay
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightDetectArea (void *str)
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
			cmdAutoBrightDetectAreaHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Overlay取得
		if ((status = autoBrightGetDetectArea (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		if (mode == 3)
			mode = 2;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// Overlay取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		if (mode == 2)
			mode = 3;

		// Overlay設定
		if ((status = autoBrightSetDetectArea (mode)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Detect Area Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightDetectAreaHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright detect area is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightoverlay\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright detect area is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightoverlay [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  0 : Disable\n");
	DEBUG_PRINT_FORCE ("  1 : Enable(Black)\n");
	DEBUG_PRINT_FORCE ("  2 : Enable(White)\n\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Auto Bright Target
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightTarget (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAutoBrightTargetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Target取得
		if ((status = autoBrightGetTarget (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", data);
	}
	else if (argc == 2)
	{
		// Target取得
		if (sscanf (gCmdArg[1], "%d", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Target設定
		if ((status = autoBrightSetTarget (data)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Target Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightTargetHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit, targetMax;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright target is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrighttarget\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : target\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright target is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrighttarget [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : target\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Target Max
	targetMax = 1 << bit;

	DEBUG_PRINT_FORCE ("[Target]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n\n", targetMax);

_DONE:
	return (status);
}


//**********************************************************************************
//	Auto Bright Average
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightAverage (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAutoBrightAverageHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Average取得
		if ((status = autoBrightGetAverage (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", data);
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
//	Auto Bright Average Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightAverageHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright average is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightaverage\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : bright average\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Auto Bright Width Offset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightWidthOffset (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int offset;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAutoBrightWidthOffsetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Width Offset取得
		if ((status = autoBrightGetWidthOffset (&offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", offset);
	}
	else if (argc == 2)
	{
		// Width Offset取得
		if (sscanf (gCmdArg[1], "%d", &offset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Width Offset設定
		if ((status = autoBrightSetWidthOffset (offset)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Width Offset Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightWidthOffsetHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright width offset is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightoffsetx\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : width offset\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright width offset is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightoffsetx [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : width offset\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Width Offset]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", WidthMax()-ROI_FPGA_WIDTH_OFFSET_ALIGH);
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}


//**********************************************************************************
//	Auto Bright Height Offset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightHeightOffset (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int offset;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAutoBrightHeightOffsetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Height Offset取得
		if ((status = autoBrightGetHeightOffset (&offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", offset);
	}
	else if (argc == 2)
	{
		// Height Offset取得
		if (sscanf (gCmdArg[1], "%d", &offset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Height Offset設定
		if ((status = autoBrightSetHeightOffset (offset)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Height Offset Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightHeightOffsetHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright height offset is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightoffsety\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : width offset\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright height offset is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightoffsety [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : width offset\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Height Offset]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", HeightMax()-ROI_FPGA_HEIGHT_OFFSET_ALIGH);
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}


//**********************************************************************************
//	Auto Bright Width Size
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightWidthSize (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int size;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAutoBrightWidthSizeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Width Size取得
		if ((status = autoBrightGetWidthSize (&size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", size);
	}
	else if (argc == 2)
	{
		// Width Size取得
		if (sscanf (gCmdArg[1], "%d", &size) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Width Size設定
		if ((status = autoBrightSetWidthSize (size)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Width Size Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightWidthSizeHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright width size is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightwidth\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : width size\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright width size is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightwidth [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : width size\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Width Size]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", WidthMax());
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}


//**********************************************************************************
//	Auto Bright Height Size
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightHeightSize (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int size;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAutoBrightHeightSizeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Height Size取得
		if ((status = autoBrightGetHeightSize (&size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", size);
	}
	else if (argc == 2)
	{
		// Height Size取得
		if (sscanf (gCmdArg[1], "%d", &size) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Height Size設定
		if ((status = autoBrightSetHeightSize (size)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Height Size Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrightHeightSizeHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright height size is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightheight\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : height size\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright height size is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrightheight [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : height size\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Height Size]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", HeightMax());
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}


//**********************************************************************************
//	Auto Bright Height Size
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrigtTargetArea (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int area;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAutoBrigtTargetAreaHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Height Size取得
		if ((status = autoBrightGetTargetArea (&area)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", area);
	}
	else if (argc == 2)
	{
		// Height Size取得
		if (sscanf (gCmdArg[1], "%d", &area) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Height Size設定
		if ((status = autoBrightSetTargetArea (area)) != AVAL_STATUS_SUCCESS)
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
//	Auto Bright Height Size Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAutoBrigtTargetAreaHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright target area.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrighttargetarea\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : target area\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The auto bright target area.\n");
	DEBUG_PRINT_FORCE ("  Command           : autobrighttargetarea [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : target area number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Target Area]\n");
	DEBUG_PRINT_FORCE ("  All Range : 0\n");
	DEBUG_PRINT_FORCE ("  Center    : 1\n");
	DEBUG_PRINT_FORCE ("  Top       : 2\n");
	DEBUG_PRINT_FORCE ("  Bottom    : 3\n");
	DEBUG_PRINT_FORCE ("  Left      : 4\n");
	DEBUG_PRINT_FORCE ("  Right     : 5\n");
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}

#endif // #if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)

// eof
