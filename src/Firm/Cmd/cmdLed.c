//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdLed.c - LED Access Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	LEDコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLed (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int num, pattern;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdLedHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// LED番号取得
		if (sscanf (gCmdArg[1], "%d", &num) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		if ((num >= LED_NUM_MIN) && (num <= LED_NUM_MAX))
		{
			// LED取得
			if ((status = getLed (num, &pattern)) != AVAL_STATUS_SUCCESS)
				return (status);
		}
		#if defined (IF_CXP)
		else if (((num - (LED_NUM_MAX + 1)) >= CXP_PORT_MIN) && ((num - (LED_NUM_MAX + 1)) <= CXP_PORT_MAX))
		{
			if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "LED Number(%d) Parameter Error.\n", num);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// CXP LED取得
			if ((status = cxpGetLed ((num - (LED_NUM_MAX + 1)), &pattern)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		#endif // #if defined (IF_CXP)
		else
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "LED Number(%d) Parameter Error.\n", num);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		DEBUG_PRINT_FORCE ("%d ", pattern);
	}
	else if (argc == 3)
	{
		// LED番号取得
		if (sscanf (gCmdArg[1], "%d", &num) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// LEDパターン取得
		if (sscanf (gCmdArg[2], "%d", &pattern) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// LED設定
		if ((status = setLedMain (num, pattern)) != AVAL_STATUS_SUCCESS)
			return (status);

		if ((num >= LED_NUM_MIN) && (num <= LED_NUM_MAX))
		{
			// LED設定
			if ((status = setLedMain (num, pattern)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		}
		#if defined (IF_CXP)
		else if (((num - (LED_NUM_MAX + 1)) >= CXP_PORT_MIN) && ((num - (LED_NUM_MAX + 1)) <= CXP_PORT_MAX))
		{
			if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "LED Number(%d) Parameter Error.\n", num);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// CXP LED設定
			if ((status = cxpSetLed ((num - (LED_NUM_MAX + 1)), pattern)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		#endif
		else
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "LED Number(%d) Parameter Error.\n", num);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
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
//	LEDコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLedHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The led mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : led [param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : LED Number\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : LED Pattern\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The led mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : led [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : LED Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : LED Pattern\n");
	DEBUG_PRINT_FORCE ("  Output Param2     : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	LEDデバックモードコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLedDebugMode (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int mode;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdLedDebugModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// LEDデバックモード取得
		if ((status = ledGetDebugMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// LED取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// LEDデバックモード設定
		if ((status = ledSetDebugMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	LEDデバックモードコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdLedDebugModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The led debug mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : leddebugmode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Debug Mode\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The led debug mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : leddebugmode [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Debug Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %d  : Debug Mode Disable\n", LED_DEBUG_MODE_DISABLE);
	DEBUG_PRINT_FORCE ("  %d  : Debug Mode Enable\n", LED_DEBUG_MODE_ENABLE);
	DEBUG_PRINT_FORCE ("  %d  : Debug Mode Error\n", LED_DEBUG_MODE_ERROR);

	return (AVAL_STATUS_SUCCESS);
}

// eof
