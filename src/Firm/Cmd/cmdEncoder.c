//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdEncoderControl.c - Genicam Encoder Control Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#ifdef MODE_ENCODER
//**********************************************************************************
//	Encoder Status表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int encoderStatusAll (void)
{
	cmdEncoderStatusAll (NULL);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Status表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderStatusAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int source;
	int mode;
	int divider;
	int timeout;
	int activation;
	double freqency;
	long long value;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdEncoderStatusAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		//----------------------------------------------------------------------
		// Encoder Status
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("[Encoder IO Status]\n");
		DEBUG_PRINT_FORCE ("\n");

		//----------------------------------------------------------------------
		// Encoder PhaseA Trg Source
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   PhaseA Trg  Source        : ");
		if ((status = encoderGetPhaseATrgSource (&source)) == AVAL_STATUS_SUCCESS)
		{
			if (source == GENICAM_ENCODER_PASEA_TRG_SOURCE_LINE0)
				DEBUG_PRINT_FORCE ("Line0\n");
			else if (source == GENICAM_ENCODER_PASEA_TRG_SOURCE_LINE1)
				DEBUG_PRINT_FORCE ("Line1\n");
			else if (source == GENICAM_ENCODER_PASEA_TRG_SOURCE_LINE2)
				DEBUG_PRINT_FORCE ("Line2\n");
			else if (source == GENICAM_ENCODER_PASEA_TRG_SOURCE_OFF)
				DEBUG_PRINT_FORCE ("OFF\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}

		//----------------------------------------------------------------------
		// Encoder PhaseB Trg Source
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   PhaseB Trg  Source        : ");
		if ((status = encoderGetPhaseBTrgSource (&source)) == AVAL_STATUS_SUCCESS)
		{
			if (source == GENICAM_ENCODER_PASEB_TRG_SOURCE_LINE0)
				DEBUG_PRINT_FORCE ("Line0\n");
			else if (source == GENICAM_ENCODER_PASEB_TRG_SOURCE_LINE1)
				DEBUG_PRINT_FORCE ("Line1\n");
			else if (source == GENICAM_ENCODER_PASEB_TRG_SOURCE_LINE2)
				DEBUG_PRINT_FORCE ("Line2\n");
			else if (source == GENICAM_ENCODER_PASEB_TRG_SOURCE_OFF)
				DEBUG_PRINT_FORCE ("OFF\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}

		//----------------------------------------------------------------------
		// Encoder Mode
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   Mode                      : ");
		if ((status = encoderGetMode (&mode)) == AVAL_STATUS_SUCCESS)
		{
			if (mode == GENICAM_ENCODER_MODE_HI_PRECISION)
				DEBUG_PRINT_FORCE ("High Precision\n");
			else if (mode == GENICAM_ENCODER_MODE_HI_RESOLUTION)
				DEBUG_PRINT_FORCE ("High Resolution\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}

		//----------------------------------------------------------------------
		// Encoder Divider
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   Divider                   : ");
		if ((status = encoderGetDivider (&divider)) == AVAL_STATUS_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("%d\n", divider);
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}

		//----------------------------------------------------------------------
		// Encoder Output Mode
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   Output Mode               : ");
		if ((status = encoderGetOutputMode (&mode)) == AVAL_STATUS_SUCCESS)
		{
			if (mode == GENICAM_ENCODER_OUTPUT_MODE_OFF)
				DEBUG_PRINT_FORCE ("OFF\n");
			else if (mode == GENICAM_ENCODER_OUTPUT_MODE_POSITION_UP)
				DEBUG_PRINT_FORCE ("Position Up\n");
			else if (mode == GENICAM_ENCODER_OUTPUT_MODE_POSITION_DOWN)
				DEBUG_PRINT_FORCE ("Position Down\n");
			else if (mode == GENICAM_ENCODER_OUTPUT_MODE_DIRECTION_UP)
				DEBUG_PRINT_FORCE ("Direction Up\n");
			else if (mode == GENICAM_ENCODER_OUTPUT_MODE_DIRECTION_DOWN)
				DEBUG_PRINT_FORCE ("Direction Down\n");
			else if (mode == GENICAM_ENCODER_OUTPUT_MODE_MOTION)
				DEBUG_PRINT_FORCE ("Motion\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}


		//----------------------------------------------------------------------
		// Encoder Status
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   Status                    : ");
		if ((status = encoderGetStatus (&mode)) == AVAL_STATUS_SUCCESS)
		{
			if (mode == GENICAM_ENCODER_STATUS_IDLE)
				DEBUG_PRINT_FORCE ("IDLE\n");
			else if (mode == GENICAM_ENCODER_STATUS_UP)
				DEBUG_PRINT_FORCE ("Up Count\n");
			else if (mode == GENICAM_ENCODER_OUTPUT_MODE_POSITION_DOWN)
				DEBUG_PRINT_FORCE ("Down Count\n");
			else if (mode == GENICAM_ENCODER_STATUS_STATIC)
				DEBUG_PRINT_FORCE ("Static\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}


		//----------------------------------------------------------------------
		// Encoder Timeout
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   Timeout                   : ");
		if ((status = encoderGetTimeout (&timeout)) == AVAL_STATUS_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("%d\n", timeout);
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}


		//----------------------------------------------------------------------
		// Encoder Reset Trg Source
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   Reset Source              : ");
		if ((status = encoderGetResetTrgSource (&source)) == AVAL_STATUS_SUCCESS)
		{
			if (source == GENICAM_ENCODER_RESET_TRG_ACQUISITION_START_TRG)
				DEBUG_PRINT_FORCE ("Acquisition Start Trg\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_ACQUISITION_START)
				DEBUG_PRINT_FORCE ("Acquisition Start\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_ACQUISITION_END)
				DEBUG_PRINT_FORCE ("Acquisition End\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_LINE0)
				DEBUG_PRINT_FORCE ("Line0\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_USER0_OUTPUT)
				DEBUG_PRINT_FORCE ("User0 Output\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_LINE1)
				DEBUG_PRINT_FORCE ("Line1\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_USER1_OUTPUT)
				DEBUG_PRINT_FORCE ("User1 Output\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_LINE2)
				DEBUG_PRINT_FORCE ("Line2\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_USER2_OUTPUT)
				DEBUG_PRINT_FORCE ("User2 Output\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_LINE3)
				DEBUG_PRINT_FORCE ("Line3\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_USER3_OUTPUT)
				DEBUG_PRINT_FORCE ("User3 Output\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_LINE4)
				DEBUG_PRINT_FORCE ("Line4\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_USER4_OUTPUT)
				DEBUG_PRINT_FORCE ("User4 Output\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_LINE5)
				DEBUG_PRINT_FORCE ("Line5\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_USER5_OUTPUT)
				DEBUG_PRINT_FORCE ("User5 Output\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_LINE6)
				DEBUG_PRINT_FORCE ("Line6\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_USER6_OUTPUT)
				DEBUG_PRINT_FORCE ("User6 Output\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_COUNTER0_START)
				DEBUG_PRINT_FORCE ("Counter0 Start\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_COUNTER0_END)
				DEBUG_PRINT_FORCE ("Counter0 End\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_COUNTER1_START)
				DEBUG_PRINT_FORCE ("Counter1 Start\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_COUNTER1_END)
				DEBUG_PRINT_FORCE ("Counter1 End\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_COUNTER2_START)
				DEBUG_PRINT_FORCE ("Counter2 Start\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_COUNTER2_END)
				DEBUG_PRINT_FORCE ("Counter2 End\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_COUNTER3_START)
				DEBUG_PRINT_FORCE ("Counter3 Start\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_COUNTER3_END)
				DEBUG_PRINT_FORCE ("Counter3 End\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_TIMER0_START)
				DEBUG_PRINT_FORCE ("Timer0 Start\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_TIMER0_END)
				DEBUG_PRINT_FORCE ("Timer0 End\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_TIMER1_START)
				DEBUG_PRINT_FORCE ("Timer1 Start\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_TIMER1_END)
				DEBUG_PRINT_FORCE ("Timer1 End\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_TIMER2_START)
				DEBUG_PRINT_FORCE ("Timer2 Start\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_TIMER2_END)
				DEBUG_PRINT_FORCE ("Timer2 End\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_TIMER3_START)
				DEBUG_PRINT_FORCE ("Timer3 Start\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_TIMER3_END)
				DEBUG_PRINT_FORCE ("Timer3 End\n");
			else if (source == GENICAM_ENCODER_RESET_TRG_OFF)
				DEBUG_PRINT_FORCE ("OFF\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}

		//----------------------------------------------------------------------
		// Encoder Activation
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   Activation                : ");
		if ((status = encoderGetResetActivation (&activation)) == AVAL_STATUS_SUCCESS)
		{
			if (activation == GENICAM_ENCODER_RESET_ACTIVATION_RISING_EDGE)
				DEBUG_PRINT_FORCE ("Rising Edge\n");
			else if (mode == GENICAM_ENCODER_RESET_ACTIVATION_FALLING_EDGE)
				DEBUG_PRINT_FORCE ("Falling Edge\n");
			else if (mode == GENICAM_ENCODER_RESET_ACTIVATION_ANY_EDGE)
				DEBUG_PRINT_FORCE ("Any Edge\n");
			else if (mode == GENICAM_ENCODER_RESET_ACTIVATION_HI_LEVEL)
				DEBUG_PRINT_FORCE ("High Level\n");
			else if (mode == GENICAM_ENCODER_RESET_ACTIVATION_LO_LEVEL)
				DEBUG_PRINT_FORCE ("Low Level\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}

		//----------------------------------------------------------------------
		// Encoder Value
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   Value                     : ");
		if ((status = encoderGetValue (&value)) == AVAL_STATUS_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("%lld\n", value);
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}

		//----------------------------------------------------------------------
		// Encoder Value at Reset
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   Value at Reset            : ");
		if ((status = encoderGetValueAtReset (&value)) == AVAL_STATUS_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("%lld\n", value);
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}


		//----------------------------------------------------------------------
		// Encoder Frequency
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("   Freqency                  : ");
		if ((status = encoderGetFrequency (&freqency)) == AVAL_STATUS_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("%lf\n", freqency);
		}
		else if (status == MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_TIMEOUT))
		{
			DEBUG_PRINT_FORCE ("\n");
			status = AVAL_STATUS_SUCCESS;
		}
		else
		{
			DEBUG_PRINT_FORCE ("\n");
		}
	}
	else
	{
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_ARG);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Status表示Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderStatusAllHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Encoder Status is acquired\n");
	DEBUG_PRINT_FORCE ("  Command           : enccontrol-status\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Status\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder PhaseA Trg Source
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderPhaseATrgSource (void *str)
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
			cmdEncoderPhaseATrgSourceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder PhaseA Trg Source取得
		if ((status = encoderGetPhaseATrgSource (&source)) != AVAL_STATUS_SUCCESS)
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

		// Encoder PhaseA Trg Source設定
		if ((status = encoderSetPhaseATrgSource (source)) != AVAL_STATUS_SUCCESS)
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
//	Encoder PhaseA Trg Source Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderPhaseATrgSourceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder phaseA trg source is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-phasea-source\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder PhaseA Trg Source\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder phaseA trg source is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-phasea-source [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Encoder PhaseA Trg Source\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Source]\n");
	DEBUG_PRINT_FORCE ("  %2d = Line0\n", GENICAM_ENCODER_PASEA_TRG_SOURCE_LINE0);
	DEBUG_PRINT_FORCE ("  %2d = Line1\n", GENICAM_ENCODER_PASEA_TRG_SOURCE_LINE1);
	DEBUG_PRINT_FORCE ("  %2d = OFF\n", GENICAM_ENCODER_PASEA_TRG_SOURCE_OFF);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder PhaseB Trg Source
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderPhaseBTrgSource (void *str)
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
			cmdEncoderPhaseBTrgSourceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder PhaseB Trg Source取得
		if ((status = encoderGetPhaseBTrgSource (&source)) != AVAL_STATUS_SUCCESS)
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

		// Encoder PhaseB Trg Source設定
		if ((status = encoderSetPhaseBTrgSource (source)) != AVAL_STATUS_SUCCESS)
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
//	Encoder PhaseB Trg Source Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderPhaseBTrgSourceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder phaseB trg source is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-phaseb-source\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder PhaseB Trg Source\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder phaseB trg source is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-phaseb-source [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Encoder PhaseB Trg Source\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Source]\n");
	DEBUG_PRINT_FORCE ("  %2d = Line0\n", GENICAM_ENCODER_PASEB_TRG_SOURCE_LINE0);
	DEBUG_PRINT_FORCE ("  %2d = Line1\n", GENICAM_ENCODER_PASEB_TRG_SOURCE_LINE1);
	DEBUG_PRINT_FORCE ("  %2d = OFF\n", GENICAM_ENCODER_PASEB_TRG_SOURCE_OFF);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderMode (void *str)
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
			cmdEncoderModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Mode取得
		if ((status = encoderGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// Mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Encoder Mode設定
		if ((status = encoderSetMode (mode)) != AVAL_STATUS_SUCCESS)
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
//	Encoder Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-mode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Mode\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder mode is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-mode [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Encoder Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %2d = Hi Precision\n", GENICAM_ENCODER_MODE_HI_PRECISION);
	DEBUG_PRINT_FORCE ("  %2d = Hi Resolution\n", GENICAM_ENCODER_MODE_HI_RESOLUTION);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Divider
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderDivider (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int divider;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdEncoderDividerHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Divider取得
		if ((status = encoderGetDivider (&divider)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", divider);
	}
	else if (argc == 2)
	{
		// Divider取得
		if (sscanf (gCmdArg[1], "%d", &divider) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Encoder Divider設定
		if ((status = encoderSetDivider (divider)) != AVAL_STATUS_SUCCESS)
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
//	Encoder Divider Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderDividerHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder divider is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-divider\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Divider(Min:%d / Max:%d)\n", GENICAM_ENCODER_DVIDER_MIN, GENICAM_ENCODER_DVIDER_MAX);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder divider is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-divider [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Encoder Divider(Min:%d / Max:%d)\n", GENICAM_ENCODER_DVIDER_MIN, GENICAM_ENCODER_DVIDER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Output Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderOutputMode (void *str)
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
			cmdEncoderOutputModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Output Mode取得
		if ((status = encoderGetOutputMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// Output Mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Encoder Output Mode設定
		if ((status = encoderSetOutputMode (mode)) != AVAL_STATUS_SUCCESS)
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
//	Encoder Output Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderOutputModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder output mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-outputmode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Output Mode\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder output mode is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-outputmode [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Encoder Output Mod\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Output Mode]\n");
	DEBUG_PRINT_FORCE ("  %2d = OFF\n", GENICAM_ENCODER_OUTPUT_MODE_OFF);
	DEBUG_PRINT_FORCE ("  %2d = Position Up\n", GENICAM_ENCODER_OUTPUT_MODE_POSITION_UP);
	DEBUG_PRINT_FORCE ("  %2d = Position Down\n", GENICAM_ENCODER_OUTPUT_MODE_POSITION_DOWN);
	DEBUG_PRINT_FORCE ("  %2d = Direction Up\n", GENICAM_ENCODER_OUTPUT_MODE_DIRECTION_UP);
	DEBUG_PRINT_FORCE ("  %2d = Direction Down\n", GENICAM_ENCODER_OUTPUT_MODE_DIRECTION_DOWN);
	DEBUG_PRINT_FORCE ("  %2d = Motion\n", GENICAM_ENCODER_OUTPUT_MODE_MOTION);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderStatus (void *str)
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
			cmdEncoderStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Status取得
		if ((status = encoderGetStatus (&mode)) != AVAL_STATUS_SUCCESS)
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
//	Encoder Status Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderStatusHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-status\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Status\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Status]\n");
	DEBUG_PRINT_FORCE ("  %2d = IDLE\n", GENICAM_ENCODER_STATUS_IDLE);
	DEBUG_PRINT_FORCE ("  %2d = Up Count\n", GENICAM_ENCODER_STATUS_UP);
	DEBUG_PRINT_FORCE ("  %2d = Down Count\n", GENICAM_ENCODER_STATUS_DOWN);
	DEBUG_PRINT_FORCE ("  %2d = Static\n", GENICAM_ENCODER_STATUS_STATIC);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Timeout
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderTimeout (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int timeout;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdEncoderTimeoutHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Timeout取得
		if ((status = encoderGetTimeout (&timeout)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", timeout);
	}
	else if (argc == 2)
	{
		// Timeout取得
		if (sscanf (gCmdArg[1], "%d", &timeout) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Encoder Timeout設定
		if ((status = encoderSetTimeout (timeout)) != AVAL_STATUS_SUCCESS)
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
//	Encoder Timeout Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderTimeoutHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder timeout is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-timeout\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Timeout(Min:%d / Max:%d)\n", GENICAM_ENCODER_TIMEOUT_MIN, GENICAM_ENCODER_TIMEOUT_MAX);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder timeout is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-timeout [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Encoder Divider(Min:%d / Max:%d)\n", GENICAM_ENCODER_TIMEOUT_MIN, GENICAM_ENCODER_TIMEOUT_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Reset Trg Source
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderResetTrgSource (void *str)
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
			cmdEncoderResetTrgSourceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Reset Trg Source取得
		if ((status = encoderGetResetTrgSource (&source)) != AVAL_STATUS_SUCCESS)
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

		// Encoder Reset Trg Source設定
		if ((status = encoderSetResetTrgSource (source)) != AVAL_STATUS_SUCCESS)
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
//	Encoder Reset Trg Source Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderResetTrgSourceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder reset trg source is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-reset-source\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Reset Trg Source\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder reset trg source is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-reset-source [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Encoder Reset Trg Source\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Source]\n");
	DEBUG_PRINT_FORCE ("  %2d = Acquisition Start Trigger\n", GENICAM_ENCODER_RESET_TRG_ACQUISITION_START_TRG);
	DEBUG_PRINT_FORCE ("  %2d = Acquisition Start\n", GENICAM_ENCODER_RESET_TRG_ACQUISITION_START);
	DEBUG_PRINT_FORCE ("  %2d = Acquisition End\n", GENICAM_ENCODER_RESET_TRG_ACQUISITION_END);

	DEBUG_PRINT_FORCE ("  %2d = Line0\n", GENICAM_ENCODER_RESET_TRG_LINE0);
	DEBUG_PRINT_FORCE ("  %2d = Line1\n", GENICAM_ENCODER_RESET_TRG_LINE1);
	DEBUG_PRINT_FORCE ("  %2d = Line2\n", GENICAM_ENCODER_RESET_TRG_LINE2);
	DEBUG_PRINT_FORCE ("  %2d = User Output3\n", GENICAM_ENCODER_RESET_TRG_USER3_OUTPUT);
	DEBUG_PRINT_FORCE ("  %2d = User Output4\n", GENICAM_ENCODER_RESET_TRG_USER4_OUTPUT);
	DEBUG_PRINT_FORCE ("  %2d = User Output5\n", GENICAM_ENCODER_RESET_TRG_USER5_OUTPUT);
	DEBUG_PRINT_FORCE ("  %2d = User Output6\n", GENICAM_ENCODER_RESET_TRG_USER6_OUTPUT);

#if defined (MODE_GENICAM_COUNTER)
	DEBUG_PRINT_FORCE ("  %2d = Counter Start0\n", GENICAM_ENCODER_RESET_TRG_COUNTER0_START);
	DEBUG_PRINT_FORCE ("  %2d = Counter End0\n", GENICAM_ENCODER_RESET_TRG_COUNTER0_END);
	DEBUG_PRINT_FORCE ("  %2d = Counter Start1\n", GENICAM_ENCODER_RESET_TRG_COUNTER1_START);
	DEBUG_PRINT_FORCE ("  %2d = Counter End1\n", GENICAM_ENCODER_RESET_TRG_COUNTER1_END);
	DEBUG_PRINT_FORCE ("  %2d = Counter Start2\n", GENICAM_ENCODER_RESET_TRG_COUNTER2_START);
	DEBUG_PRINT_FORCE ("  %2d = Counter End2\n", GENICAM_ENCODER_RESET_TRG_COUNTER2_END);
	DEBUG_PRINT_FORCE ("  %2d = Counter Start3\n", GENICAM_ENCODER_RESET_TRG_COUNTER3_START);
	DEBUG_PRINT_FORCE ("  %2d = Counter End3\n", GENICAM_ENCODER_RESET_TRG_COUNTER3_END);
#endif

#if defined (MODE_GENICAM_TIMER)
	DEBUG_PRINT_FORCE ("  %2d = Timer Start0\n", GENICAM_ENCODER_RESET_TRG_TIMER0_START);
	DEBUG_PRINT_FORCE ("  %2d = Timer End0\n", GENICAM_ENCODER_RESET_TRG_TIMER0_END);
	DEBUG_PRINT_FORCE ("  %2d = Timer Start1\n", GENICAM_ENCODER_RESET_TRG_TIMER1_START);
	DEBUG_PRINT_FORCE ("  %2d = Timer End1\n", GENICAM_ENCODER_RESET_TRG_TIMER1_END);
	DEBUG_PRINT_FORCE ("  %2d = Timer Start2\n", GENICAM_ENCODER_RESET_TRG_TIMER2_START);
	DEBUG_PRINT_FORCE ("  %2d = Timer End2\n", GENICAM_ENCODER_RESET_TRG_TIMER2_END);
	DEBUG_PRINT_FORCE ("  %2d = Timer Start3\n", GENICAM_ENCODER_RESET_TRG_TIMER3_START);
	DEBUG_PRINT_FORCE ("  %2d = Timer End3\n", GENICAM_ENCODER_RESET_TRG_TIMER3_END);
#endif

	DEBUG_PRINT_FORCE ("  %2d = OFF\n", GENICAM_ENCODER_RESET_TRG_OFF);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Reset Activation
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderResetActivation (void *str)
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
			cmdEncoderResetActivationHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Activation取得
		if ((status = encoderGetResetActivation (&activation)) != AVAL_STATUS_SUCCESS)
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

		// Encoder Activation設定
		if ((status = encoderSetResetActivation (activation)) != AVAL_STATUS_SUCCESS)
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
//	Encoder Reset Activation Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderResetActivationHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder reset activation is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-reset-activation\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Reset Activation\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder reset activation is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-reset-activation [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Encoder Reset Activation\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Activation]\n");
	DEBUG_PRINT_FORCE ("  %2d = Rising Edge\n", GENICAM_ENCODER_RESET_ACTIVATION_RISING_EDGE);
	DEBUG_PRINT_FORCE ("  %2d = Falling Edge\n", GENICAM_ENCODER_RESET_ACTIVATION_FALLING_EDGE);
	DEBUG_PRINT_FORCE ("  %2d = Any Edge\n", GENICAM_ENCODER_RESET_ACTIVATION_ANY_EDGE);
	DEBUG_PRINT_FORCE ("  %2d = High Level\n", GENICAM_ENCODER_RESET_ACTIVATION_HI_LEVEL);
	DEBUG_PRINT_FORCE ("  %2d = Low Level\n", GENICAM_ENCODER_RESET_ACTIVATION_LO_LEVEL);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Reset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderReset (void *str)
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
			cmdEncoderResetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Reset
		if ((status = encoderSetReset ()) != AVAL_STATUS_SUCCESS)
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
//	Encoder Reset Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderResetHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder reset is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-reset [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Value
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderValue (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	long long value;
	
	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdEncoderValueHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Value取得
		if ((status = encoderGetValue (&value)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%lld ", value);
	}
	else if (argc == 2)
	{
		// value取得
		if (sscanf (gCmdArg[1], "%lld", &value) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Encoder Value設定
		if ((status = encoderSetValue (value)) != AVAL_STATUS_SUCCESS)
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
//	Encoder Value Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderValueHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder value is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-value\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Value(Min:%lld / Max:%lld)\n", GENICAM_ENCODER_VALUE_MIN, GENICAM_ENCODER_VALUE_MAX);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder value is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-value [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Encoder Value(Min:%lld / Max:%lld)\n", GENICAM_ENCODER_VALUE_MIN, GENICAM_ENCODER_VALUE_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Value at Reset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderValueAtReset (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	long long value;
	
	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdEncoderValueAtResetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Value取得
		if ((status = encoderGetValueAtReset (&value)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%lld ", value);
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
//	Encoder Value at Reset Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderValueAtResetHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder reset value is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-reset-value\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Reset Value(Min:%lld / Max:%lld)\n", GENICAM_ENCODER_VALUE_MIN, GENICAM_ENCODER_VALUE_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Frequency
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderFrequency (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double frequency;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdEncoderFrequencyHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Encoder Frequency取得
		status = encoderGetFrequency (&frequency);

		if (status == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("%.5lf ", frequency);
		else if (status == MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_TIMEOUT))
			status = AVAL_STATUS_SUCCESS;
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
//	Encoder Frequency Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdEncoderFrequencyHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The encoder Frequency is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : enc-frequency\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Encoder Frequency[Hz](Min:%d - Max:%d)\n", GENICAM_ENCODER_FREQ_MIN, GENICAM_ENCODER_FREQ_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

#endif // MODE_ENCODER

// eof
