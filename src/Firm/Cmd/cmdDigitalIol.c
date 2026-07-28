//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdDigitalIoControl.c - Genicam Digital I/O Control Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	Digital IO Status表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int digitaiIoStatusAll (void)
{
	cmdDigitaiIoStatusAll (NULL);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Digital IO Status表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitaiIoStatusAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	int select;
	unsigned int mask;
	int saveSelect;
	int selectMax, selectMin;
	int userSelectMax;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitaiIoStatusAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get Line Select(Ver.1.2)
		if ((status = digitalIoGetLineSelect (&saveSelect)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//----------------------------------------------------------------------
		// Digital IO Status
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("[Digital IO Status]\n");

		//----------------------------------------------------------------------
		// Line
		//----------------------------------------------------------------------
		selectMin = GENICAM_DIGITAL_LINE_SELECT_LINE6;
		selectMax = GENICAM_DIGITAL_LINE_SELECT_CC1;
		
		DEBUG_PRINT_FORCE ("\n [Line]");
		for (select = GENICAM_DIGITAL_LINE_SELECT_MIN; select <= GENICAM_DIGITAL_LINE_SELECT_MAX; select++)
		{
			if ((select > selectMin) && (select < selectMax))
				continue;

			//----------------------------------------------------------------------
			// Line Select
			//----------------------------------------------------------------------
			if ((status = digitalIoSetLineSelect (select)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			
			DEBUG_PRINT_FORCE ("\n   Line Selector            ");
			if (select == GENICAM_DIGITAL_LINE_SELECT_LINE0)
				DEBUG_PRINT_FORCE (" : Line0\n");
			else if (select == GENICAM_DIGITAL_LINE_SELECT_LINE1)
				DEBUG_PRINT_FORCE (" : Line1\n");
			else if (select == GENICAM_DIGITAL_LINE_SELECT_LINE2)
				DEBUG_PRINT_FORCE (" : Line2\n");
			else if (select == GENICAM_DIGITAL_LINE_SELECT_LINE3)
				DEBUG_PRINT_FORCE (" : Line3\n");
			else if (select == GENICAM_DIGITAL_LINE_SELECT_LINE4)
				DEBUG_PRINT_FORCE (" : Line4\n");
			else if (select == GENICAM_DIGITAL_LINE_SELECT_LINE5)
				DEBUG_PRINT_FORCE (" : Line5\n");
			else if (select == GENICAM_DIGITAL_LINE_SELECT_LINE6)
				DEBUG_PRINT_FORCE (" : Line6\n");

			else if (select == GENICAM_DIGITAL_LINE_SELECT_CC1)
				DEBUG_PRINT_FORCE (" : CC1\n");
			else if (select == GENICAM_DIGITAL_LINE_SELECT_CC2)
				DEBUG_PRINT_FORCE (" : CC2\n");
			else if (select == GENICAM_DIGITAL_LINE_SELECT_CC3)
				DEBUG_PRINT_FORCE (" : CC3\n");
			else if (select == GENICAM_DIGITAL_LINE_SELECT_CC4)
				DEBUG_PRINT_FORCE (" : CC4 \n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Line Mode
			//----------------------------------------------------------------------
			if ((status = digitalIoGetMode (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Mode                     ");
			if (mode == GENICAM_DIGITAL_LINE_MODE_INPUT)
				DEBUG_PRINT_FORCE (" : Input\n");
			else if (mode == GENICAM_DIGITAL_LINE_MODE_OUTPUT)
				DEBUG_PRINT_FORCE (" : Output\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Line Inverter
			//----------------------------------------------------------------------
			if ((status = digitalIoGetInverter (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Inverter                 ");
			if (mode == DIGITAL_LINE_INVERTER_ENABLE)
				DEBUG_PRINT_FORCE (" : Enable\n");
			else if (mode == DIGITAL_LINE_INVERTER_DISABLE)
				DEBUG_PRINT_FORCE (" : Disable\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Line Status
			//----------------------------------------------------------------------
			if ((status = digitalIoGetLineStatus (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Status                   ");
			if (mode == GENICAM_DIGITAL_LINE_STATUS_NEGATE)
				DEBUG_PRINT_FORCE (" : Negate\n");
			else if (mode == GENICAM_DIGITAL_LINE_STATUS_ASSERT)
				DEBUG_PRINT_FORCE (" : Assert\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Line Source
			//----------------------------------------------------------------------
			if ((status = digitalIoGetLineSource (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Source                   ");
			if (mode == GENICAM_DIGITAL_LINE_SOURCE_ACQUISITION_TRG_WAIT)
				DEBUG_PRINT_FORCE (" : Acquisition Trg Wait\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_ACQUISITION_ACTIVE)
				DEBUG_PRINT_FORCE (" : Acquisition Active\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_FRAME_TRG_WAIT)
				DEBUG_PRINT_FORCE (" : Frame Trg Wait\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_FRAME_ACTIVE)
				DEBUG_PRINT_FORCE (" : Frame Active\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_EXPOSURE_ACTIVE)
				DEBUG_PRINT_FORCE (" : Exposure Active\n");

			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_USER0_OUTPUT)
				DEBUG_PRINT_FORCE (" : User0 Output\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_USER1_OUTPUT)
				DEBUG_PRINT_FORCE (" : User1 Output\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_USER2_OUTPUT)
				DEBUG_PRINT_FORCE (" : User2 Output\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_USER3_OUTPUT)
				DEBUG_PRINT_FORCE (" : User3 Output\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_USER4_OUTPUT)
				DEBUG_PRINT_FORCE (" : User4 Output\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_USER5_OUTPUT)
				DEBUG_PRINT_FORCE (" : User5 Output\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_USER6_OUTPUT)
				DEBUG_PRINT_FORCE (" : User6 Output\n");

			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_COUNTER0_ACTIVE)
				DEBUG_PRINT_FORCE (" : Counter0 Active\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_COUNTER1_ACTIVE)
				DEBUG_PRINT_FORCE (" : Counter1 Active\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_COUNTER2_ACTIVE)
				DEBUG_PRINT_FORCE (" : Counter2 Active\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_COUNTER3_ACTIVE)
				DEBUG_PRINT_FORCE (" : Counter3 Active\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_TIMER0_ACTIVE)
				DEBUG_PRINT_FORCE (" : Timer0 Active\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_TIMER1_ACTIVE)
				DEBUG_PRINT_FORCE (" : Timer1 Active\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_TIMER2_ACTIVE)
				DEBUG_PRINT_FORCE (" : Timer2 Active\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_TIMER3_ACTIVE)
				DEBUG_PRINT_FORCE (" : Timer3 Active\n");
			else if (mode == GENICAM_DIGITAL_LINE_SOURCE_OFF)
				DEBUG_PRINT_FORCE (" : OFF\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Line Format
			//----------------------------------------------------------------------
			if ((status = digitalIoGetLineFormat (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Format                   ");
			if (mode == DIGITAL_LINE_FORMAT_NON_CONNECT)
				DEBUG_PRINT_FORCE (" : Non Connect\n");
#if (IF_HW_TYPE == CAMERA_TYPE_ALEA)
			else if (mode == DIGITAL_LINE_FORMAT_TRI_STATE)
				DEBUG_PRINT_FORCE (" : TRI State\n");
			else if (mode == DIGITAL_LINE_FORMAT_TLL)
				DEBUG_PRINT_FORCE (" : TTL\n");
			else if (mode == DIGITAL_LINE_FORMAT_LVDS)
				DEBUG_PRINT_FORCE (" : LVDS\n");
#endif
			else if (mode == DIGITAL_LINE_FORMAT_RS422)
				DEBUG_PRINT_FORCE (" : RS422\n");
#if (IF_HW_TYPE == CAMERA_TYPE_ALEA)
			else if (mode == DIGITAL_LINE_FORMAT_OPT_COUPLED)
				DEBUG_PRINT_FORCE (" : Opt Coupled\n");
			else if (mode == DIGITAL_LINE_FORMAT_OPEN_DRAIN)
				DEBUG_PRINT_FORCE (" : Open Drain\n");
#endif

			else if (mode == DIGITAL_LINE_FORMAT_RS422_WITHOUT_TERM)
				DEBUG_PRINT_FORCE (" : RS422 Without Term\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}

		//----------------------------------------------------------------------
		// User
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("\n [User]");
		userSelectMax = DIGITAL_USER_SELECT_EXT_VERSION2_MAX;

		for (select = DIGITAL_USER_SELECT_MIN; select <= userSelectMax; select++)
		{
			//----------------------------------------------------------------------
			// User Select
			//----------------------------------------------------------------------
			if ((status = digitalIoSetUserSelect (select)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			
			DEBUG_PRINT_FORCE ("\n   User Selector            ");
			if (select == DIGITAL_USER0_SELECT)
				DEBUG_PRINT_FORCE (" : User0\n");
			else if (select == DIGITAL_USER1_SELECT)
				DEBUG_PRINT_FORCE (" : User1\n");
			else if (select == DIGITAL_USER2_SELECT)
				DEBUG_PRINT_FORCE (" : User2\n");
			else if (select == DIGITAL_USER3_SELECT)
				DEBUG_PRINT_FORCE (" : User3\n");
			else if (select == DIGITAL_USER4_SELECT)
				DEBUG_PRINT_FORCE (" : User4\n");
			else if (select == DIGITAL_USER5_SELECT)
				DEBUG_PRINT_FORCE (" : User5\n");
			else if (select == DIGITAL_USER6_SELECT)
				DEBUG_PRINT_FORCE (" : User6\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// User Value
			//----------------------------------------------------------------------
			if ((status = digitalIoGetUserValue (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Value                    ");
			if (mode == DIGITAL_USER_VALUE_NEGATE)
				DEBUG_PRINT_FORCE (" : Negate\n");
			else if (mode == DIGITAL_USER_VALUE_ASSERT)
				DEBUG_PRINT_FORCE (" : Assert\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}

		//----------------------------------------------------------------------
		// User Mask
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("\n   Mask                     ");
		if ((status = digitalIoGetUserMask (&mask)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		DEBUG_PRINT_FORCE (" : 0x%x\n", mask);

		DEBUG_PRINT_FORCE ("\n", mask);

		// Set Line Select
		if ((status = digitalIoSetLineSelect (saveSelect)) != AVAL_STATUS_SUCCESS)
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
//	Digital IO Status表示Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitaiIoStatusAllHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Line Status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : digitalcontrol-status\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Line Status\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Line Select
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineSelect (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int select;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalIoLineSelectHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// ラインセレクト取得
		if ((status = digitalIoGetLineSelect (&select)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", select);
	}
	else if (argc == 2)
	{
		// select取得
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// ラインセレクト設定
		if ((status = digitalIoSetLineSelect (select)) != AVAL_STATUS_SUCCESS)
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
//	Line Selector Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineSelectHelp (void *str)
{
	int i, end;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line select is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-selector\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Line Select\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line select is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-selector [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Line Select\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Selector]\n");

	end = GENICAM_DIGITAL_LINE_SELECT_MAX_EXTEND_VERSION2;
	for (i=GENICAM_DIGITAL_LINE_SELECT_MIN; i<=end; i++)
		DEBUG_PRINT_FORCE ("  %2d = Line%d\n", GENICAM_DIGITAL_LINE_SELECT_LINE0+i, i);

	if((gInterFaceID == INTERFACE_CAMERALINK) || (gInterFaceID == INTERFACE_CXP))
		DEBUG_PRINT_FORCE ("  %2d = CC1\n", GENICAM_DIGITAL_LINE_SELECT_CC1);

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Line Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineMode (void *str)
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
			cmdDigitalIoLineModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// ラインモード取得
		if ((status = digitalIoGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// ラインモード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// ラインモード設定
		if ((status = digitalIoSetMode (mode)) != AVAL_STATUS_SUCCESS)
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
//	Line Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-mode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Line Mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Line Mode]\n");
	DEBUG_PRINT_FORCE ("  %2d = Input\n", GENICAM_DIGITAL_LINE_MODE_INPUT);
	DEBUG_PRINT_FORCE ("  %2d = Output\n", GENICAM_DIGITAL_LINE_MODE_OUTPUT);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Line Inverter
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineInverter (void *str)
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
			cmdDigitalIoLineInverterHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// ラインインバータ取得
		if ((status = digitalIoGetInverter (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// インバータ取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// インバータ設定
		if ((status = digitalIoSetInverter (mode)) != AVAL_STATUS_SUCCESS)
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
//	Line Inverter Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineInverterHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line inverter is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-inverter\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Line Inverter(%d:Disable / %d:Enable)\n", DIGITAL_LINE_INVERTER_DISABLE, DIGITAL_LINE_INVERTER_ENABLE);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line inverter is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-inverter [Param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : Line Inverter(%d:Disable / %d:Enable)\n", DIGITAL_LINE_INVERTER_DISABLE, DIGITAL_LINE_INVERTER_ENABLE);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Line Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineStatus (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int lineState;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalIoLineStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// ラインステータス取得
		if ((status = digitalIoGetLineStatus (&lineState)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", lineState);
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
//	Line Status Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineStatusHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line Status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-status\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Line Status(%d:Negate / %d:Assert)\n", GENICAM_DIGITAL_LINE_STATUS_NEGATE, GENICAM_DIGITAL_LINE_STATUS_ASSERT);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Line Status All
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineStatusAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int lineState;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalIoLineStatusAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// ラインステータス取得
		if ((status = digitalIoGetLineAllStatus (&lineState)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("0x%08x ", lineState);
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
//	Line Status All Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineStatusAllHelp (void *str)
{
	int i, end;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line all Status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-status-all\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Line All Status\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Status]\n");
	DEBUG_PRINT_FORCE ("  %d : Negate\n", GENICAM_DIGITAL_LINE_STATUS_NEGATE);
	DEBUG_PRINT_FORCE ("  %d : Assert\n", GENICAM_DIGITAL_LINE_STATUS_ASSERT);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Bit]\n");

	DEBUG_PRINT_FORCE ("  Bit0  : Line0\n");
	DEBUG_PRINT_FORCE ("  Bit1  : Line1\n");
	DEBUG_PRINT_FORCE ("  Bit2  : Line2\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Line Source
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineSource (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int source;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalIoLineSourceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// ラインソース取得
		if ((status = digitalIoGetLineSource (&source)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", source);
	}
	else if (argc == 2)
	{
		// ラインソース取得
		if (sscanf (gCmdArg[1], "%d", &source) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// ラインソース設定
		if ((status = digitalIoSetLineSource (source)) != AVAL_STATUS_SUCCESS)
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
//	Line Source Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineSourceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line source is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-source\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Line Source\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line source is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-source [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Line Source\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Source]\n");
	DEBUG_PRINT_FORCE ("  %3d = Acquisition TRG Wait\n", GENICAM_DIGITAL_LINE_SOURCE_ACQUISITION_TRG_WAIT);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Active\n", GENICAM_DIGITAL_LINE_SOURCE_ACQUISITION_ACTIVE);

	DEBUG_PRINT_FORCE ("  %3d = Frame TRG Wait\n", GENICAM_DIGITAL_LINE_SOURCE_FRAME_TRG_WAIT);
	DEBUG_PRINT_FORCE ("  %3d = Frame Active\n", GENICAM_DIGITAL_LINE_SOURCE_FRAME_ACTIVE);

	DEBUG_PRINT_FORCE ("  %3d = Exposure Active\n", GENICAM_DIGITAL_LINE_SOURCE_EXPOSURE_ACTIVE);
	//DEBUG_PRINT_FORCE ("  %3d = User0 Output\n", GENICAM_DIGITAL_LINE_SOURCE_USER0_OUTPUT);
	//DEBUG_PRINT_FORCE ("  %3d = User1 Output\n", GENICAM_DIGITAL_LINE_SOURCE_USER1_OUTPUT);
	//DEBUG_PRINT_FORCE ("  %3d = User2 Output\n", GENICAM_DIGITAL_LINE_SOURCE_USER2_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = User3 Output\n", GENICAM_DIGITAL_LINE_SOURCE_USER3_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = User4 Output\n", GENICAM_DIGITAL_LINE_SOURCE_USER4_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = User5 Output\n", GENICAM_DIGITAL_LINE_SOURCE_USER5_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = User6 Output\n", GENICAM_DIGITAL_LINE_SOURCE_USER6_OUTPUT);

#if defined (MODE_GENICAM_COUNTER)
	#if 0
	DEBUG_PRINT_FORCE ("  %3d = Counter0 Active\n", GENICAM_DIGITAL_LINE_SOURCE_COUNTER0_ACTIVE);
	DEBUG_PRINT_FORCE ("  %3d = Counter1 Active\n", GENICAM_DIGITAL_LINE_SOURCE_COUNTER1_ACTIVE);
	DEBUG_PRINT_FORCE ("  %3d = Counter2 Active\n", GENICAM_DIGITAL_LINE_SOURCE_COUNTER2_ACTIVE);
	DEBUG_PRINT_FORCE ("  %3d = Counter3 Active\n", GENICAM_DIGITAL_LINE_SOURCE_COUNTER3_ACTIVE);
	#endif
#endif

#if defined (MODE_GENICAM_TIMER)
	#if 0
	DEBUG_PRINT_FORCE ("  %3d = Timer0 Active\n", GENICAM_DIGITAL_LINE_SOURCE_TIMER0_ACTIVE);
	DEBUG_PRINT_FORCE ("  %3d = Timer1 Active\n", GENICAM_DIGITAL_LINE_SOURCE_TIMER1_ACTIVE);
	DEBUG_PRINT_FORCE ("  %3d = Timer2 Active\n", GENICAM_DIGITAL_LINE_SOURCE_TIMER2_ACTIVE);
	DEBUG_PRINT_FORCE ("  %3d = Timer3 Active\n", GENICAM_DIGITAL_LINE_SOURCE_TIMER3_ACTIVE);
#endif
#endif

	DEBUG_PRINT_FORCE ("  %3d = OFF\n", GENICAM_DIGITAL_LINE_SOURCE_OFF);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Line Format
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineFormat (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int format;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalIoLineFormatHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// フォーマット取得
		if ((status = digitalIoGetLineFormat (&format)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", format);
	}
	else if (argc == 2)
	{
		// フォーマット設定
		if (sscanf (gCmdArg[1], "%d", &format) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// フォーマット設定
		if ((status = digitalIoSetLineFormat (format)) != AVAL_STATUS_SUCCESS)
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
//	Line Format Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoLineFormatHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line format is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-format\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Line Format\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Line format is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : line-format [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Line Format\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Format]\n");
	DEBUG_PRINT_FORCE ("  %2d : Non Connect\n", DIGITAL_LINE_FORMAT_NON_CONNECT);
	DEBUG_PRINT_FORCE ("  %2d : Non Connect\n", DIGITAL_LINE_FORMAT_NON_CONNECT);
	DEBUG_PRINT_FORCE ("  %2d : TTL\n", DIGITAL_LINE_FORMAT_TLL);
	DEBUG_PRINT_FORCE ("  %2d : RS422\n", DIGITAL_LINE_FORMAT_RS422);
	DEBUG_PRINT_FORCE ("  %2d : RS422 without term Coupled\n", DIGITAL_LINE_FORMAT_RS422_WITHOUT_TERM);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	User Selector
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoUserSelector (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int select;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalIoUserSelectorHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Userセレクタ取得
		if ((status = digitalIoGetUserSelect (&select)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", select);
	}
	else if (argc == 2)
	{
		// Userセレクタ取得
		if (sscanf (gCmdArg[1], "%d", &select) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Userセレクタ設定
		if ((status = digitalIoSetUserSelect (select)) != AVAL_STATUS_SUCCESS)
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
//	User Selector Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoUserSelectorHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The User selector is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : user-selector\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : User Selector\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The User selector is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : user-selector [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : User Selector\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Selector]\n");
	DEBUG_PRINT_FORCE ("  %2d = User Select3\n", DIGITAL_USER3_SELECT);
	DEBUG_PRINT_FORCE ("  %2d = User Select4\n", DIGITAL_USER4_SELECT);
	DEBUG_PRINT_FORCE ("  %2d = User Select5\n", DIGITAL_USER5_SELECT);
	DEBUG_PRINT_FORCE ("  %2d = User Select6\n", DIGITAL_USER6_SELECT);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	User Value
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoUserValue (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int value;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalIoUserValueHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// User Value取得
		if ((status = digitalIoGetUserValue (&value)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", value);
	}
	else if (argc == 2)
	{
		// User Value取得
		if (sscanf (gCmdArg[1], "%d", &value) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// User Value設定
		if ((status = digitalIoSetUserValue (value)) != AVAL_STATUS_SUCCESS)
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
//	User Value Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoUserValueHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The User value is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : user-value\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : User Value\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The User value is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : user-value [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : User Value\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Value]\n");
	DEBUG_PRINT_FORCE ("  %d = Negate\n", DIGITAL_USER_VALUE_NEGATE);
	DEBUG_PRINT_FORCE ("  %d = Assert\n", DIGITAL_USER_VALUE_ASSERT);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	User Value
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoUserAllValue (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int value;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalIoUserAllValueHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// User All Value取得
		if ((status = digitalIoGetAllValue (&value)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("0x%08x ", value);
	}
	else if (argc == 2)
	{
		// User Value取得
		if (sscanf (gCmdArg[1], "%d", &value) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// User All Value設定
		if ((status = digitalIoSetAllValue (value)) != AVAL_STATUS_SUCCESS)
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
//	User All Value Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoUserAllValueHelp (void *str)
{
	int i, end;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The User all value is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : user-value-all\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : User All Value\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The User all value is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : user-value-all [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : User All Value\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Value]\n");
	DEBUG_PRINT_FORCE ("  %d = Negate\n", DIGITAL_USER_VALUE_NEGATE);
	DEBUG_PRINT_FORCE ("  %d = Assert\n", DIGITAL_USER_VALUE_ASSERT);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Bit]\n");
	end = DIGITAL_USER_SELECT_EXT_VERSION2_MAX;
	for (i=DIGITAL_USER_SELECT_EXT_MIN; i<=end; i++)
		DEBUG_PRINT_FORCE ("  Bit%d = User%d\n", i, DIGITAL_USER0_SELECT+i);

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	User Mask
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoUserAllMask (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int mask;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalIoUserAllMaskHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Userマスク取得
		if ((status = digitalIoGetUserMask (&mask)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("0x%08x ", mask);
	}
	else if (argc == 2)
	{
		// Userマスク取得
		if (sscanf (gCmdArg[1], "%d", &mask) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Userマスク設定
		if ((status = digitalIoSetUserMask (mask)) != AVAL_STATUS_SUCCESS)
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
//	User Mask Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalIoUserAllMaskHelp (void *str)
{
	int i, end;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The User all mask is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : user-mask-all\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : User Mask\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The User all mask is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : user-mask-all [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : User Mask\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Value]\n");
	DEBUG_PRINT_FORCE ("  %d = UnMask\n", GENICAM_DIGITAL_USER_UNMASK);
	DEBUG_PRINT_FORCE ("  %d = Mask\n", GENICAM_DIGITAL_USER_MASK);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Bit]\n");

	end = GENICAM_DIGITAL_USER_MASK_EXT_MAX;
	end = 6;
	for (i=GENICAM_DIGITAL_USER_MASK_EXT_MIN; i<=end; i++)
		DEBUG_PRINT_FORCE ("  Bit%d = User%d\n", i, DIGITAL_USER0_SELECT+i);

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
