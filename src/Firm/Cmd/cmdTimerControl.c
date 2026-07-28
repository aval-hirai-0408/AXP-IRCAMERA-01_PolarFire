//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdTimerControl.c - Genicam Timer Control Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	Timer Status表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int timerStatusAll (void)
{
	cmdTimerStatusAll (NULL);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Status表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerStatusAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	int select;
	unsigned int count;
	unsigned int timerStatus;
	int saveSelect;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdTimerStatusAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get Timer Select(Ver.1.2)
		if ((status = timerControlGetSelect (&saveSelect)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//----------------------------------------------------------------------
		// Timer Status
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("[Timer Status]\n");

		//----------------------------------------------------------------------
		// Timer
		//----------------------------------------------------------------------
		for (select = TIMER_SELECT_MIN; select <= TIMER_SELECT_MAX; select++)
		{
			//----------------------------------------------------------------------
			// Timer Select
			//----------------------------------------------------------------------
			if ((status = timerControlSetSelect (select)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			
			DEBUG_PRINT_FORCE ("\n   Selector                 ");
			if (select == TIMER0_SELECT)
				DEBUG_PRINT_FORCE (" : Timer0\n");
			else if (select == TIMER1_SELECT)
				DEBUG_PRINT_FORCE (" : Timer1\n");
			else if (select == TIMER2_SELECT)
				DEBUG_PRINT_FORCE (" : Timer2\n");
			else if (select == TIMER3_SELECT)
				DEBUG_PRINT_FORCE (" : Timer3\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Timer Duration
			//----------------------------------------------------------------------
			if ((status = timerControlGetDuration (&count)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			DEBUG_PRINT_FORCE ("   Duration                 ");
			DEBUG_PRINT_FORCE (" : %d\n", count);

			//----------------------------------------------------------------------
			// Timer Delay
			//----------------------------------------------------------------------
			if ((status = timerControlGetDelay (&count)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			DEBUG_PRINT_FORCE ("   Delay                    ");
			DEBUG_PRINT_FORCE (" : %d\n", count);

			//----------------------------------------------------------------------
			// Timer Value
			//----------------------------------------------------------------------
			if ((status = timerControlGetValue (&count)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			DEBUG_PRINT_FORCE ("   Value                    ");
			DEBUG_PRINT_FORCE (" : %d\n", count);

			//----------------------------------------------------------------------
			// Timer Status
			//----------------------------------------------------------------------
			if ((status = timerControlGetStatus (&timerStatus)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Status                   ");
			if (timerStatus == GENICAM_TIMER_STATUS_IDLE)
				DEBUG_PRINT_FORCE (" : IDLE\n");
			else if (timerStatus == GENICAM_TIMER_STATUS_TRG_WAIT)
				DEBUG_PRINT_FORCE (" : TRG Wait\n");
			else if (timerStatus == GENICAM_TIMER_STATUS_TRG_ACTIVE)
				DEBUG_PRINT_FORCE (" : Active\n");
			else if (timerStatus == GENICAM_TIMER_STATUS_COMPLATED)
				DEBUG_PRINT_FORCE (" : Completed Edge\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Timer Trg Source
			//----------------------------------------------------------------------
			if ((status = timerControlGetSource (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Source                   ");
			if (mode == GENICAM_TIMER_SOURCE_ACQUISITION_START_TRIGGER)
				DEBUG_PRINT_FORCE (" : Acquisition Start Trg\n");
			else if (mode == GENICAM_TIMER_SOURCE_ACQUISITION_START)
				DEBUG_PRINT_FORCE (" : Acquisition Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_ACQUISITION_END)
				DEBUG_PRINT_FORCE (" : Acquisition End\n");
			else if (mode == GENICAM_TIMER_SOURCE_FRAME_START_TRIGGER)
				DEBUG_PRINT_FORCE (" : Exposure Start Trg\n");
			else if (mode == GENICAM_TIMER_SOURCE_FRAME_START)
				DEBUG_PRINT_FORCE (" : Exposure Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_FRAME_END)
				DEBUG_PRINT_FORCE (" : Exposure End\n");
			else if (mode == GENICAM_TIMER_SOURCE_EXPOSURE_START)
				DEBUG_PRINT_FORCE (" : Exposure Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_EXPOSURE_END)
				DEBUG_PRINT_FORCE (" : Exposure End\n");
			else if (mode == GENICAM_TIMER_SOURCE_LINE0)
				DEBUG_PRINT_FORCE (" : Line0\n");
			else if (mode == GENICAM_TIMER_SOURCE_USER0_OUTPUT)
				DEBUG_PRINT_FORCE (" : User0 Output\n");
			else if (mode == GENICAM_TIMER_SOURCE_LINE1)
				DEBUG_PRINT_FORCE (" : Line1\n");
			else if (mode == GENICAM_TIMER_SOURCE_USER1_OUTPUT)
				DEBUG_PRINT_FORCE (" : User1 Output\n");
			else if (mode == GENICAM_TIMER_SOURCE_LINE2)
				DEBUG_PRINT_FORCE (" : Line2\n");
			else if (mode == GENICAM_TIMER_SOURCE_USER2_OUTPUT)
				DEBUG_PRINT_FORCE (" : User2 Output\n");
			else if (mode == GENICAM_TIMER_SOURCE_LINE3)
				DEBUG_PRINT_FORCE (" : Line3\n");
			else if (mode == GENICAM_TIMER_SOURCE_USER3_OUTPUT)
				DEBUG_PRINT_FORCE (" : User3 Output\n");
			else if (mode == GENICAM_TIMER_SOURCE_LINE4)
				DEBUG_PRINT_FORCE (" : Line4\n");
			else if (mode == GENICAM_TIMER_SOURCE_USER4_OUTPUT)
				DEBUG_PRINT_FORCE (" : User4 Output\n");
			else if (mode == GENICAM_TIMER_SOURCE_LINE5)
				DEBUG_PRINT_FORCE (" : Line5\n");
			else if (mode == GENICAM_TIMER_SOURCE_USER5_OUTPUT)
				DEBUG_PRINT_FORCE (" : User5 Output\n");
			else if (mode == GENICAM_TIMER_SOURCE_LINE6)
				DEBUG_PRINT_FORCE (" : Line6\n");
			else if (mode == GENICAM_TIMER_SOURCE_USER6_OUTPUT)
				DEBUG_PRINT_FORCE (" : User6 Output\n");

			else if (mode == GENICAM_TIMER_SOURCE_COUNTER0_START)
				DEBUG_PRINT_FORCE (" : Counter0 Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER0_END)
				DEBUG_PRINT_FORCE (" : Counter0 End\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER1_START)
				DEBUG_PRINT_FORCE (" : Counter1 Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER1_END)
				DEBUG_PRINT_FORCE ("Counter1 End\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER2_START)
				DEBUG_PRINT_FORCE (" : Counter2 Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER2_END)
				DEBUG_PRINT_FORCE (" : Counter2 End\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER3_START)
				DEBUG_PRINT_FORCE (" : Counter3 Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER3_END)
				DEBUG_PRINT_FORCE (" : Counter3 End\n");
			else if (mode == GENICAM_TIMER_SOURCE_TIMER0_START)
				DEBUG_PRINT_FORCE (" : Timer0 Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER0_END)
				DEBUG_PRINT_FORCE (" : Timer0 End\n");
			else if (mode == GENICAM_TIMER_SOURCE_TIMER1_START)
				DEBUG_PRINT_FORCE (" : Timer1 Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER1_END)
				DEBUG_PRINT_FORCE (" : Timer1 End\n");
			else if (mode == GENICAM_TIMER_SOURCE_TIMER2_START)
				DEBUG_PRINT_FORCE (" : Timer2 Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER2_END)
				DEBUG_PRINT_FORCE (" : Timer2 End\n");
			else if (mode == GENICAM_TIMER_SOURCE_TIMER3_START)
				DEBUG_PRINT_FORCE (" : Timer3 Start\n");
			else if (mode == GENICAM_TIMER_SOURCE_COUNTER3_END)
				DEBUG_PRINT_FORCE (" : Timer3 End\n");
			else if (mode == GENICAM_TIMER_SOURCE_ENCODER0)
				DEBUG_PRINT_FORCE (" : Encoder0\n");
			else if (mode == GENICAM_TIMER_SOURCE_OFF)
				DEBUG_PRINT_FORCE (" : OFF\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Timer Activation
			//----------------------------------------------------------------------
			if ((status = timerControlGetTrgActive (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Activation               ");
			if (mode == TIMER_ACTIVE_RISE_EDGE)
				DEBUG_PRINT_FORCE (" : Rise Edge\n");
			else if (mode == TIMER_ACTIVE_FALL_EDGE)
				DEBUG_PRINT_FORCE (" : Fall Edge\n");
			else if (mode == TIMER_ACTIVE_ANY_EDGE)
				DEBUG_PRINT_FORCE (" : Any Edge\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}

		// Set Timer Select
		if ((status = timerControlSetSelect (saveSelect)) != AVAL_STATUS_SUCCESS)
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
//	Timer Status表示Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerStatusAllHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Timer Status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : timercontrol-status\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Timer Status\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Timer Selector
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerSelect (void *str)
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
			cmdTimerSelectHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Timerセレクト取得
		if ((status = timerControlGetSelect (&select)) != AVAL_STATUS_SUCCESS)
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

		// Timerセレクト設定
		if ((status = timerControlSetSelect (select)) != AVAL_STATUS_SUCCESS)
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
//	Timer Selector Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerSelectHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer select is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-selector\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Select\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer select is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-selector [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Timer Select\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Selector]\n");
	DEBUG_PRINT_FORCE ("  %d = Timer0\n", TIMER0_SELECT);
	DEBUG_PRINT_FORCE ("  %d = Timer1\n", TIMER1_SELECT);
	DEBUG_PRINT_FORCE ("  %d = Timer2\n", TIMER2_SELECT);
	DEBUG_PRINT_FORCE ("  %d = Timer3\n", TIMER3_SELECT);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Timer Duration
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerDuration (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int count;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdTimerDurationHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Timerカウンタ取得
		if ((status = timerControlGetDuration (&count)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", count);
	}
	else if (argc == 2)
	{
		// カウンタ取得
		if (sscanf (gCmdArg[1], "%d", &count) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Timerカウンタ設定
		if ((status = timerControlSetDuration (count)) != AVAL_STATUS_SUCCESS)
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
//	Timer Duration Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerDurationHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer duration is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-duration\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Timer Duration(Min:%d - Max:%d)\n", GENICAM_TIMER_DURATION_MIN, GENICAM_TIMER_DURATION_MAX);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer duration is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-duration [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Timer Duration(Min:%d - Max:%d)\n", GENICAM_TIMER_DURATION_MIN, GENICAM_TIMER_DURATION_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Timer Delay
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerDelay (void *str)
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
			cmdTimerDelayHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Timer Delay取得
		if ((status = timerControlGetDelay (&delay)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", delay);
	}
	else if (argc == 2)
	{
		// Delay取得
		if (sscanf (gCmdArg[1], "%d", &delay) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Timer Delay設定
		if ((status = timerControlSetDelay (delay)) != AVAL_STATUS_SUCCESS)
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
//	Timer Delay Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerDelayHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer delay is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-delay\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Timer Delay(Min:%d - Max:%d)\n", GENICAM_TIMER_DELAY_MIN, GENICAM_TIMER_DELAY_MAX);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer delay is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-delay [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Timer Delay(Min:%d - Max:%d)\n", GENICAM_TIMER_DELAY_MIN, GENICAM_TIMER_DELAY_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Timer Reset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerReset (void *str)
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
			cmdTimerResetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Timer Reset設定
		if ((status = timerControlReset ()) != AVAL_STATUS_SUCCESS)
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
//	Timer Reset Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerResetHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer reset is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-reset [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Timer Value
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerValue (void *str)
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
			cmdTimerValueHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Timer Value取得
		if ((status = timerControlGetValue (&value)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", value);
	}
	else if (argc == 2)
	{
		// Value取得
		if (sscanf (gCmdArg[1], "%d", &value) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Timer Value設定
		if ((status = timerControlSetValue (value)) != AVAL_STATUS_SUCCESS)
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
//	Timer Value Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerValueHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer value is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-value\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Timer Value(Min:%d - Max:%d)\n", GENICAM_TIMER_DELAY_MIN, GENICAM_TIMER_DELAY_MAX);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer value is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-value [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Timer Value(Min:%d - Max:%d)\n", GENICAM_TIMER_DELAY_MIN, GENICAM_TIMER_DELAY_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Timer Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerStatus (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int timerStatus;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdTimerStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Timer Status取得
		if ((status = timerControlGetStatus (&timerStatus)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", timerStatus);
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
//	Counter Status Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerStatusHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-status\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Timer Status\n");
	
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Status]\n");
	DEBUG_PRINT_FORCE ("  %d = Idle\n", GENICAM_TIMER_STATUS_IDLE);
	DEBUG_PRINT_FORCE ("  %d = Trg Wait\n", GENICAM_TIMER_STATUS_TRG_WAIT);
	DEBUG_PRINT_FORCE ("  %d = Active\n", GENICAM_TIMER_STATUS_TRG_ACTIVE);
	DEBUG_PRINT_FORCE ("  %d = Completed\n", GENICAM_TIMER_STATUS_COMPLATED);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Timer Source
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerSource (void *str)
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
			cmdTimerSourceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Timer Source取得
		if ((status = timerControlGetSource (&source)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", source);
	}
	else if (argc == 2)
	{
		// Source取得
		if (sscanf (gCmdArg[1], "%d", &source) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Timer Source設定
		if ((status = timerControlSetSource (source)) != AVAL_STATUS_SUCCESS)
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
//	Counter Reset Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerSourceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer source is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-source\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Timer Source\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer source is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-source [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Timer Source\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Source]\n");
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Start Trigger\n", GENICAM_TIMER_SOURCE_ACQUISITION_START_TRIGGER);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Start\n", GENICAM_TIMER_SOURCE_ACQUISITION_START);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition End\n", GENICAM_TIMER_SOURCE_ACQUISITION_END);
	DEBUG_PRINT_FORCE ("  %3d = Exposure Start\n", GENICAM_TIMER_SOURCE_EXPOSURE_START);
	DEBUG_PRINT_FORCE ("  %3d = Exposure End\n", GENICAM_TIMER_SOURCE_EXPOSURE_END);

	DEBUG_PRINT_FORCE ("  %3d = Line0\n", GENICAM_TIMER_SOURCE_LINE0);
	DEBUG_PRINT_FORCE ("  %3d = User Output0\n", GENICAM_TIMER_SOURCE_USER0_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line1\n", GENICAM_TIMER_SOURCE_LINE1);
	DEBUG_PRINT_FORCE ("  %3d = User Output1\n", GENICAM_TIMER_SOURCE_USER1_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line2\n", GENICAM_TIMER_SOURCE_LINE2);
	DEBUG_PRINT_FORCE ("  %3d = User Output2\n", GENICAM_TIMER_SOURCE_USER2_OUTPUT);

	DEBUG_PRINT_FORCE ("  %3d = Line3\n", GENICAM_TIMER_SOURCE_LINE3);
	DEBUG_PRINT_FORCE ("  %3d = User Output3\n", GENICAM_TIMER_SOURCE_USER3_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line4\n", GENICAM_TIMER_SOURCE_LINE4);
	DEBUG_PRINT_FORCE ("  %3d = User Output4\n", GENICAM_TIMER_SOURCE_USER4_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line5\n", GENICAM_TIMER_SOURCE_LINE5);
	DEBUG_PRINT_FORCE ("  %3d = User Output5\n", GENICAM_TIMER_SOURCE_USER5_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line6\n", GENICAM_TIMER_SOURCE_LINE6);
	DEBUG_PRINT_FORCE ("  %3d = User Output6\n", GENICAM_TIMER_SOURCE_USER6_OUTPUT);

	DEBUG_PRINT_FORCE ("  %3d = Counter0 Start\n", GENICAM_TIMER_SOURCE_COUNTER0_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter0 End\n", GENICAM_TIMER_SOURCE_COUNTER0_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter1 Start\n", GENICAM_TIMER_SOURCE_COUNTER1_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter1 End\n", GENICAM_TIMER_SOURCE_COUNTER1_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter2 Start\n", GENICAM_TIMER_SOURCE_COUNTER2_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter2 End\n", GENICAM_TIMER_SOURCE_COUNTER2_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter3 Start\n", GENICAM_TIMER_SOURCE_COUNTER3_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter3 End\n", GENICAM_TIMER_SOURCE_COUNTER3_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer0 Start\n", GENICAM_TIMER_SOURCE_TIMER0_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer0 End\n", GENICAM_TIMER_SOURCE_TIMER0_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer1 Start\n", GENICAM_TIMER_SOURCE_TIMER1_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer1 End\n", GENICAM_TIMER_SOURCE_TIMER1_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer2 Start\n", GENICAM_TIMER_SOURCE_TIMER2_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer2 End\n", GENICAM_TIMER_SOURCE_TIMER2_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer3 Start\n", GENICAM_TIMER_SOURCE_TIMER3_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer3 End\n", GENICAM_TIMER_SOURCE_TIMER3_END);
#if defined (MODE_ENCODER)
	DEBUG_PRINT_FORCE ("  %3d = Encoder0\n", GENICAM_TIMER_SOURCE_ENCODER0);
#endif
	DEBUG_PRINT_FORCE ("  %3d = OFF\n", GENICAM_TIMER_SOURCE_OFF);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Timer Activation
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerActivation (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int activation;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdTimerActivationHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Timer Activation取得
		if ((status = timerControlGetTrgActive (&activation)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", activation);
	}
	else if (argc == 2)
	{
		// Activation取得
		if (sscanf (gCmdArg[1], "%d", &activation) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Timer Activation設定
		if ((status = timerControlSetTrgActive (activation)) != AVAL_STATUS_SUCCESS)
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
//	Timer Activation Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTimerActivationHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer activation is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-activation\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Timer Activation\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Timer activation is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : timer-activation [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Timer Activation\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Activation]\n");
	DEBUG_PRINT_FORCE ("  %d = Rising Edge\n", COUNTER_EVENT_ACTIVE_RISE_EDGE);
	DEBUG_PRINT_FORCE ("  %d = Faling Edge\n", COUNTER_EVENT_ACTIVE_FALL_EDGE);
	DEBUG_PRINT_FORCE ("  %d = Any Edge\n", COUNTER_EVENT_ACTIVE_ANY_EDGE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
