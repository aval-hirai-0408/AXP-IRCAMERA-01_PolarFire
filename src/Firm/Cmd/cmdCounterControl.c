//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdCounterControl.c - Genicam Counter Control Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	Counter Status表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int counterStatusAll (void)
{
	cmdCounterStatusAll (NULL);
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
int cmdCounterStatusAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	int select;
	int count;
	unsigned int counterStatus;
	int saveSelect;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCounterStatusAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get Counter Select
		if ((status = counterGetSelect (&saveSelect)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//----------------------------------------------------------------------
		// Counter Status
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("[Counter Status]\n");

		//----------------------------------------------------------------------
		// Counter
		//----------------------------------------------------------------------
		for (select = COUNTER_SELECT_MIN; select <= COUNTER_SELECT_MAX; select++)
		{
			//----------------------------------------------------------------------
			// Counter Select
			//----------------------------------------------------------------------
			if ((status = counterSetSelect (select)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			
			DEBUG_PRINT_FORCE ("\n   Selector                 ");
			if (select == COUNTER0_SELECT)
				DEBUG_PRINT_FORCE (" : Counter0\n");
			else if (select == COUNTER1_SELECT)
				DEBUG_PRINT_FORCE (" : Counter1\n");
			else if (select == COUNTER2_SELECT)
				DEBUG_PRINT_FORCE (" : Counter2\n");
			else if (select == COUNTER3_SELECT)
				DEBUG_PRINT_FORCE (" : Counter3\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Counter Event Source
			//----------------------------------------------------------------------
			if ((status = counterGetEventSource (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Event Source             ");
			if (mode == GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_START_TRIGGER)
				DEBUG_PRINT_FORCE (" : Acquisition Start Trg\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_START)
				DEBUG_PRINT_FORCE (" : Acquisition Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_END)
				DEBUG_PRINT_FORCE (" : Acquisition End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_INVALID_TRG)
				DEBUG_PRINT_FORCE (" : Acquisition Invalid Trg\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_FRAME_START_TRIGGER)
				DEBUG_PRINT_FORCE (" : Exposure Start Trg\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_FRAME_START)
				DEBUG_PRINT_FORCE (" : Exposure Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_FRAME_ACTIVE)
				DEBUG_PRINT_FORCE (" : Exposure Active\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_FRAME_END)
				DEBUG_PRINT_FORCE (" : Exposure End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_FRAME_INVALID_TRG)
				DEBUG_PRINT_FORCE (" : Frame Invalid Trg\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_EXPOSURE_START)
				DEBUG_PRINT_FORCE (" : Exposure Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_EXPOSURE_END)
				DEBUG_PRINT_FORCE (" : Exposure End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_EXPOSURE_INVALID_TRG)
				DEBUG_PRINT_FORCE (" : Exposure Invalid Trg\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_LINE0)
				DEBUG_PRINT_FORCE (" : Line0\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_LINE1)
				DEBUG_PRINT_FORCE (" : Line1\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_LINE2)
				DEBUG_PRINT_FORCE (" : Line2\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_LINE3)
				DEBUG_PRINT_FORCE (" : Line3\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_LINE4)
				DEBUG_PRINT_FORCE (" : Line4\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_LINE5)
				DEBUG_PRINT_FORCE (" : Line5\n");

			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_COUNTER0_START)
				DEBUG_PRINT_FORCE (" : Counter0 Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_COUNTER0_END)
				DEBUG_PRINT_FORCE (" : Counter0 End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_COUNTER1_START)
				DEBUG_PRINT_FORCE (" : Counter1 Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_COUNTER1_END)
				DEBUG_PRINT_FORCE (" : Counter1 End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_COUNTER2_START)
				DEBUG_PRINT_FORCE (" : Counter2 Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_COUNTER2_END)
				DEBUG_PRINT_FORCE (" : Counter2 End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_COUNTER3_START)
				DEBUG_PRINT_FORCE (" : Counter3 Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_COUNTER3_END)
				DEBUG_PRINT_FORCE (" : Counter3 End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_TIMER0_START)
				DEBUG_PRINT_FORCE (" : Timer0 Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_TIMER0_END)
				DEBUG_PRINT_FORCE (" : Timer0 End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_TIMER1_START)
				DEBUG_PRINT_FORCE (" : Timer1 Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_TIMER1_END)
				DEBUG_PRINT_FORCE (" : Timer1 End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_TIMER2_START)
				DEBUG_PRINT_FORCE (" : Timer2 Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_TIMER2_END)
				DEBUG_PRINT_FORCE (" : Timer2 End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_TIMER3_START)
				DEBUG_PRINT_FORCE (" : Timer3 Start\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_TIMER3_END)
				DEBUG_PRINT_FORCE (" : Timer3 End\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_ENCODER0)
				DEBUG_PRINT_FORCE (" : Encoder0\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_TICK)
				DEBUG_PRINT_FORCE (" : Tick\n");
			else if (mode == GENICAM_COUNTER_EVENT_SOURCE_OFF)
				DEBUG_PRINT_FORCE (" : OFF\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Counter Event Activation
			//----------------------------------------------------------------------
			if ((status = counterGetEventActive (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Event Activation         ");
			if (mode == COUNTER_EVENT_ACTIVE_RISE_EDGE)
				DEBUG_PRINT_FORCE (" : Rise Edge\n");
			else if (mode == COUNTER_EVENT_ACTIVE_FALL_EDGE)
				DEBUG_PRINT_FORCE (" : Fall Edge\n");
			else if (mode == COUNTER_EVENT_ACTIVE_ANY_EDGE)
				DEBUG_PRINT_FORCE (" : Any Edge\n");
			else
				DEBUG_PRINT_FORCE ("\n");
	
			//----------------------------------------------------------------------
			// Counter Reset Source
			//----------------------------------------------------------------------
			if ((status = counterGetResetSource (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Reset Source             ");
			if (mode == GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_START_TRIGGER)
				DEBUG_PRINT_FORCE (" : Acquisition Start Trg\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_START)
				DEBUG_PRINT_FORCE (" : Acquisition Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_END)
				DEBUG_PRINT_FORCE (" : Acquisition End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_INVALID_TRG)
				DEBUG_PRINT_FORCE (" : Acquisition Invalid Trg\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_FRAME_START_TRIGGER)
				DEBUG_PRINT_FORCE (" : Exposure Start Trg\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_FRAME_START)
				DEBUG_PRINT_FORCE (" : Exposure Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_FRAME_ACTIVE)
				DEBUG_PRINT_FORCE (" : Exposure Active\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_FRAME_END)
				DEBUG_PRINT_FORCE (" : Exposure End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_FRAME_INVALID_TRG)
				DEBUG_PRINT_FORCE (" : Frame Invalid Trg\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_EXPOSURE_START)
				DEBUG_PRINT_FORCE (" : Exposure Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_EXPOSURE_END)
				DEBUG_PRINT_FORCE (" : Exposure End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_EXPOSURE_INVALID_TRG)
				DEBUG_PRINT_FORCE (" : Exposure Invalid Trg\n");

			else if (mode == GENICAM_COUNTER_RESET_SOURCE_LINE0)
				DEBUG_PRINT_FORCE (" : Line0\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_USER0_OUTPUT)
				DEBUG_PRINT_FORCE (" : User0 Output\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_LINE1)
				DEBUG_PRINT_FORCE (" : Line1\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_USER1_OUTPUT)
				DEBUG_PRINT_FORCE (" : User1 Output\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_LINE2)
				DEBUG_PRINT_FORCE (" : Line2\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_USER2_OUTPUT)
				DEBUG_PRINT_FORCE (" : User2 Output\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_LINE3)
				DEBUG_PRINT_FORCE (" : Line3\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_USER3_OUTPUT)
				DEBUG_PRINT_FORCE (" : User3 Output\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_LINE4)
				DEBUG_PRINT_FORCE (" : Line4\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_USER4_OUTPUT)
				DEBUG_PRINT_FORCE (" : User4 Output\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_LINE5)
				DEBUG_PRINT_FORCE (" : Line5\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_USER5_OUTPUT)
				DEBUG_PRINT_FORCE (" : User6 Output\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_COUNTER0_START)
				DEBUG_PRINT_FORCE (" : Counter0 Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_COUNTER0_END)
				DEBUG_PRINT_FORCE (" : Counter0 End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_COUNTER1_START)
				DEBUG_PRINT_FORCE (" : Counter1 Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_COUNTER1_END)
				DEBUG_PRINT_FORCE (" : Counter1 End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_COUNTER2_START)
				DEBUG_PRINT_FORCE (" : Counter2 Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_COUNTER2_END)
				DEBUG_PRINT_FORCE (" : Counter2 End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_COUNTER3_START)
				DEBUG_PRINT_FORCE (" : Counter3 Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_COUNTER3_END)
				DEBUG_PRINT_FORCE (" : Counter3 End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_TIMER0_START)
				DEBUG_PRINT_FORCE (" : Timer0 Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_TIMER0_END)
				DEBUG_PRINT_FORCE (" : Timer0 End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_TIMER1_START)
				DEBUG_PRINT_FORCE (" : Timer1 Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_TIMER1_END)
				DEBUG_PRINT_FORCE (" : Timer1 End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_TIMER2_START)
				DEBUG_PRINT_FORCE (" : Timer2 Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_TIMER2_END)
				DEBUG_PRINT_FORCE (" : Timer2 End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_TIMER3_START)
				DEBUG_PRINT_FORCE (" : Timer3 Start\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_TIMER3_END)
				DEBUG_PRINT_FORCE (" : Timer3 End\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_ENCODER0)
				DEBUG_PRINT_FORCE (" : Encoder0\n");
			else if (mode == GENICAM_COUNTER_RESET_SOURCE_OFF)
				DEBUG_PRINT_FORCE (" : OFF\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Counter Reset Activation
			//----------------------------------------------------------------------
			if ((status = counterGetResetActive (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Reset Activation         ");
			if (mode == COUNTER_RESET_ACTIVE_RISE_EDGE)
				DEBUG_PRINT_FORCE (" : Rise Edge\n");
			else if (mode == COUNTER_RESET_ACTIVE_FALL_EDGE)
				DEBUG_PRINT_FORCE (" : Fall Edge\n");
			else if (mode == COUNTER_RESET_ACTIVE_ANY_EDGE)
				DEBUG_PRINT_FORCE (" : Any Edge\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Counter Value
			//----------------------------------------------------------------------
			if ((status = counterGetValue (&count)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Value                    ");
			DEBUG_PRINT_FORCE (" : %d\n", count);

			//----------------------------------------------------------------------
			// Counter Reset Value
			//----------------------------------------------------------------------
			if ((status = counterGetResetValue (&count)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Value at Reset           ");
			DEBUG_PRINT_FORCE (" : %d\n", count);

			//----------------------------------------------------------------------
			// Counter Duration
			//----------------------------------------------------------------------
			if ((status = counterGetDuration (&count)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Duration                 ");
			DEBUG_PRINT_FORCE (" : %d\n", count);

			//----------------------------------------------------------------------
			// Counter Status
			//----------------------------------------------------------------------
			if ((status = counterGetStatus (&counterStatus)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Status                   ");
			if (counterStatus == GENICAM_COUNTER_RESET_STATUS_IDLE)
				DEBUG_PRINT_FORCE (" : IDLE\n");
			else if (counterStatus == GENICAM_COUNTER_RESET_STATUS_TRG_WAIT)
				DEBUG_PRINT_FORCE (" : TRG Wait\n");
			else if (counterStatus == GENICAM_COUNTER_RESET_STATUS_ACTIVE)
				DEBUG_PRINT_FORCE (" : Active\n");
			else if (counterStatus == GENICAM_COUNTER_RESET_STATUS_COMPLATED)
				DEBUG_PRINT_FORCE (" : Completed Edge\n");
			else if (counterStatus == GENICAM_COUNTER_RESET_STATUS_OVERFLOW)
				DEBUG_PRINT_FORCE (" : Overflow\n");
			else
				DEBUG_PRINT_FORCE ("\n");
			
			//----------------------------------------------------------------------
			// Counter Trg Source
			//----------------------------------------------------------------------
			if ((status = counterGetTrgSource (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   TRG Source               ");
			if (mode == GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_START_TRIGGER)
				DEBUG_PRINT_FORCE (" : Acquisition Start Trg\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_START)
				DEBUG_PRINT_FORCE (" : Acquisition Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_END)
				DEBUG_PRINT_FORCE (" : Acquisition End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_INVALID_TRG)
				DEBUG_PRINT_FORCE (" : Acquisition Invalid Trg\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_FRAME_START_TRIGGER)
				DEBUG_PRINT_FORCE (" : Exposure Start Trg\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_FRAME_START)
				DEBUG_PRINT_FORCE (" : Exposure Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_FRAME_ACTIVE)
				DEBUG_PRINT_FORCE (" : Exposure Active\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_FRAME_END)
				DEBUG_PRINT_FORCE (" : Exposure End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_FRAME_INVALID_TRG)
				DEBUG_PRINT_FORCE (" : Frame Invalid Trg\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_EXPOSURE_START)
				DEBUG_PRINT_FORCE (" : Exposure Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_EXPOSURE_END)
				DEBUG_PRINT_FORCE (" : Exposure End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_EXPOSURE_INVALID_TRG)
				DEBUG_PRINT_FORCE (" : Exposure Invalid Trg\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_LINE0)
				DEBUG_PRINT_FORCE (" : Line0\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_USER0_OUTPUT)
				DEBUG_PRINT_FORCE (" : User0 Output\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_LINE1)
				DEBUG_PRINT_FORCE (" : Line1\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_USER1_OUTPUT)
				DEBUG_PRINT_FORCE (" : User1 Output\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_LINE2)
				DEBUG_PRINT_FORCE (" : Line2\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_USER2_OUTPUT)
				DEBUG_PRINT_FORCE (" : User2 Output\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_LINE3)
				DEBUG_PRINT_FORCE (" : Line3\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_USER3_OUTPUT)
				DEBUG_PRINT_FORCE (" : User3 Output\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_LINE4)
				DEBUG_PRINT_FORCE (" : Line4\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_USER4_OUTPUT)
				DEBUG_PRINT_FORCE (" : User4 Output\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_LINE5)
				DEBUG_PRINT_FORCE (" : Line5\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_USER5_OUTPUT)
				DEBUG_PRINT_FORCE (" : User5 Output\n");

			else if (mode == GENICAM_COUNTER_TRG_SOURCE_COUNTER0_START)
				DEBUG_PRINT_FORCE (" : Counter0 Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_COUNTER0_END)
				DEBUG_PRINT_FORCE (" : Counter0 End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_COUNTER1_START)
				DEBUG_PRINT_FORCE (" : Counter1 Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_COUNTER1_END)
				DEBUG_PRINT_FORCE (" : Counter1 End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_COUNTER2_START)
				DEBUG_PRINT_FORCE (" : Counter2 Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_COUNTER2_END)
				DEBUG_PRINT_FORCE (" : Counter2 End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_COUNTER3_START)
				DEBUG_PRINT_FORCE (" : Counter3 Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_COUNTER3_END)
				DEBUG_PRINT_FORCE (" : Counter3 End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_TIMER0_START)
				DEBUG_PRINT_FORCE (" : Timer0 Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_TIMER0_END)
				DEBUG_PRINT_FORCE (" : Timer0 End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_TIMER1_START)
				DEBUG_PRINT_FORCE (" : Timer1 Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_TIMER1_END)
				DEBUG_PRINT_FORCE (" : Timer1 End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_TIMER2_START)
				DEBUG_PRINT_FORCE (" : Timer2 Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_TIMER2_END)
				DEBUG_PRINT_FORCE (" : Timer2 End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_TIMER3_START)
				DEBUG_PRINT_FORCE (" : Timer3 Start\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_TIMER3_END)
				DEBUG_PRINT_FORCE (" : Timer3 End\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_ENCODER0)
				DEBUG_PRINT_FORCE (" : Encoder0\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_COUNTER_START_TRG)
				DEBUG_PRINT_FORCE (" : Counter Start Trg\n");
			else if (mode == GENICAM_COUNTER_TRG_SOURCE_OFF)
				DEBUG_PRINT_FORCE (" : OFF\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Counter Reset Activation
			//----------------------------------------------------------------------
			if ((status = counterGetTrgActive (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Trg Activation           ");
			if (mode == COUNTER_TRG_ACTIVE_RISE_EDGE)
				DEBUG_PRINT_FORCE (" : Rise Edge\n");
			else if (mode == COUNTER_TRG_ACTIVE_FALL_EDGE)
				DEBUG_PRINT_FORCE (" : Fall Edge\n");
			else if (mode == COUNTER_TRG_ACTIVE_ANY_EDGE)
				DEBUG_PRINT_FORCE (" : Any Edge\n");
			else if (mode == COUNTER_TRG_ACTIVE_HI_LVLEL)
				DEBUG_PRINT_FORCE (" : High Level\n");
			else if (mode == COUNTER_TRG_ACTIVE_LO_LVLEL)
				DEBUG_PRINT_FORCE (" : Low Level\n");
			else
				DEBUG_PRINT_FORCE ("\n");
		}

		// Set Counter Select(Ver.1.2)
		if ((status = counterSetSelect (saveSelect)) != AVAL_STATUS_SUCCESS)
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
//	Counter Status表示Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterStatusAllHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Counter Status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : countercontrol-status\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Status\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Selector
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterSelect (void *str)
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
			cmdCounterSelectHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counterセレクト取得
		if ((status = counterGetSelect (&select)) != AVAL_STATUS_SUCCESS)
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

		// Counterセレクト設定
		if ((status = counterSetSelect (select)) != AVAL_STATUS_SUCCESS)
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
//	Counter Selector Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterSelectHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter select is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-selector\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Select\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter select is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-selector [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Counter Select\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Selector]\n");
	DEBUG_PRINT_FORCE ("  %d = Counter0\n", COUNTER0_SELECT);
	DEBUG_PRINT_FORCE ("  %d = Counter1\n", COUNTER1_SELECT);
	DEBUG_PRINT_FORCE ("  %d = Counter2\n", COUNTER2_SELECT);
	DEBUG_PRINT_FORCE ("  %d = Counter3\n", COUNTER3_SELECT);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Event Source
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterEventSource (void *str)
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
			cmdCounterEventSourceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Event Source取得
		if ((status = counterGetEventSource (&source)) != AVAL_STATUS_SUCCESS)
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

		// Counter Event Source設定
		if ((status = counterSetEventSource (source)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Event Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterEventSourceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter event source is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-event-source\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Event Source\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter event source is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-event-source [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Counter Event Source\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Source]\n");
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Start Trigger\n", GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_START_TRIGGER);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Start\n", GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_START);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition End\n", GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_END);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Invalid Trigger\n", GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_INVALID_TRG);
	DEBUG_PRINT_FORCE ("  %3d = Frame Invalid Trigger\n", GENICAM_COUNTER_EVENT_SOURCE_FRAME_INVALID_TRG);
	DEBUG_PRINT_FORCE ("  %3d = Exposure Start\n", GENICAM_COUNTER_EVENT_SOURCE_EXPOSURE_START);
	DEBUG_PRINT_FORCE ("  %3d = Exposure End\n", GENICAM_COUNTER_EVENT_SOURCE_EXPOSURE_END);
	DEBUG_PRINT_FORCE ("  %3d = Exposure Invalid Trigger\n", GENICAM_COUNTER_EVENT_SOURCE_EXPOSURE_INVALID_TRG);

	DEBUG_PRINT_FORCE ("  %3d = Line0\n", GENICAM_COUNTER_EVENT_SOURCE_LINE0);
	DEBUG_PRINT_FORCE ("  %3d = Line1\n", GENICAM_COUNTER_EVENT_SOURCE_LINE1);
	DEBUG_PRINT_FORCE ("  %3d = Line2\n", GENICAM_COUNTER_EVENT_SOURCE_LINE2);
	DEBUG_PRINT_FORCE ("  %3d = Line3\n", GENICAM_COUNTER_EVENT_SOURCE_LINE3);
	DEBUG_PRINT_FORCE ("  %3d = Line4\n", GENICAM_COUNTER_EVENT_SOURCE_LINE4);
	DEBUG_PRINT_FORCE ("  %3d = Line5\n", GENICAM_COUNTER_EVENT_SOURCE_LINE5);
	DEBUG_PRINT_FORCE ("  %3d = Line6\n", GENICAM_COUNTER_EVENT_SOURCE_LINE6);

	DEBUG_PRINT_FORCE ("  %3d = Counter0 Start\n", GENICAM_COUNTER_EVENT_SOURCE_COUNTER0_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter0 End\n", GENICAM_COUNTER_EVENT_SOURCE_COUNTER0_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter1 Start\n", GENICAM_COUNTER_EVENT_SOURCE_COUNTER1_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter1 End\n", GENICAM_COUNTER_EVENT_SOURCE_COUNTER1_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter2 Start\n", GENICAM_COUNTER_EVENT_SOURCE_COUNTER2_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter2 End\n", GENICAM_COUNTER_EVENT_SOURCE_COUNTER2_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter3 Start\n", GENICAM_COUNTER_EVENT_SOURCE_COUNTER3_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter3 End\n", GENICAM_COUNTER_EVENT_SOURCE_COUNTER3_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer0 Start\n", GENICAM_COUNTER_EVENT_SOURCE_TIMER0_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer0 End\n", GENICAM_COUNTER_EVENT_SOURCE_TIMER0_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer1 Start\n", GENICAM_COUNTER_EVENT_SOURCE_TIMER1_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer1 End\n", GENICAM_COUNTER_EVENT_SOURCE_TIMER1_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer2 Start\n", GENICAM_COUNTER_EVENT_SOURCE_TIMER2_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer2 End\n", GENICAM_COUNTER_EVENT_SOURCE_TIMER2_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer3 Start\n", GENICAM_COUNTER_EVENT_SOURCE_TIMER3_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer3 End\n", GENICAM_COUNTER_EVENT_SOURCE_TIMER3_END);
#if defined (MODE_ENCODER)
	DEBUG_PRINT_FORCE ("  %3d = Encoder0\n", GENICAM_COUNTER_EVENT_SOURCE_ENCODER0);
#endif
	DEBUG_PRINT_FORCE ("  %3d = Tick\n", GENICAM_COUNTER_EVENT_SOURCE_TICK);
	DEBUG_PRINT_FORCE ("  %3d = OFF\n", GENICAM_COUNTER_EVENT_SOURCE_OFF);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Event Activation
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterEventActivation (void *str)
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
			cmdCounterEventActivationHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Event Activation取得
		if ((status = counterGetEventActive (&activation)) != AVAL_STATUS_SUCCESS)
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

		// Counter Event Activation設定
		if ((status = counterSetEventActive (activation)) != AVAL_STATUS_SUCCESS)
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
//	Counter Event Activation Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterEventActivationHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter event activation is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-event-activation\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Event Activation\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter event activation is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-event-activation [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Counter Event Activation\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Activation]\n");
	DEBUG_PRINT_FORCE ("  %d = Rising Edge\n", COUNTER_EVENT_ACTIVE_RISE_EDGE);
	DEBUG_PRINT_FORCE ("  %d = Faling Edge\n", COUNTER_EVENT_ACTIVE_FALL_EDGE);
	DEBUG_PRINT_FORCE ("  %d = Any Edge\n", COUNTER_EVENT_ACTIVE_ANY_EDGE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Reset Source
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterResetSource (void *str)
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
			cmdCounterResetSourceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Reset Source取得
		if ((status = counterGetResetSource (&source)) != AVAL_STATUS_SUCCESS)
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

		// Counter Reset Source設定
		if ((status = counterSetResetSource (source)) != AVAL_STATUS_SUCCESS)
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
int cmdCounterResetSourceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter reset source is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-reset-source\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Reset Source\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter reset source is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-reset-source [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Counter Reset Source\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Source]\n");
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Start Trigger\n", GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_START_TRIGGER);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Start\n", GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_START);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition End\n", GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_END);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Invalid Trigger\n", GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_INVALID_TRG);
	DEBUG_PRINT_FORCE ("  %3d = Frame Invalid Trigger\n", GENICAM_COUNTER_RESET_SOURCE_FRAME_INVALID_TRG);
	DEBUG_PRINT_FORCE ("  %3d = Exposure Start\n", GENICAM_COUNTER_RESET_SOURCE_EXPOSURE_START);
	DEBUG_PRINT_FORCE ("  %3d = Exposure End\n", GENICAM_COUNTER_RESET_SOURCE_EXPOSURE_END);
	DEBUG_PRINT_FORCE ("  %3d = Exposure Invalid Trigger\n", GENICAM_COUNTER_RESET_SOURCE_EXPOSURE_INVALID_TRG);

	DEBUG_PRINT_FORCE ("  %3d = Line0\n", GENICAM_COUNTER_RESET_SOURCE_LINE0);
	DEBUG_PRINT_FORCE ("  %3d = User Output0\n", GENICAM_COUNTER_RESET_SOURCE_USER0_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line1\n", GENICAM_COUNTER_RESET_SOURCE_LINE1);
	DEBUG_PRINT_FORCE ("  %3d = User Output1\n", GENICAM_COUNTER_RESET_SOURCE_USER1_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line2\n", GENICAM_COUNTER_RESET_SOURCE_LINE2);
	DEBUG_PRINT_FORCE ("  %3d = User Output2\n", GENICAM_COUNTER_RESET_SOURCE_USER2_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line3\n", GENICAM_COUNTER_RESET_SOURCE_LINE3);
	DEBUG_PRINT_FORCE ("  %3d = User Output3\n", GENICAM_COUNTER_RESET_SOURCE_USER3_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line4\n", GENICAM_COUNTER_RESET_SOURCE_LINE4);
	DEBUG_PRINT_FORCE ("  %3d = User Output4\n", GENICAM_COUNTER_RESET_SOURCE_USER4_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line5\n", GENICAM_COUNTER_RESET_SOURCE_LINE5);
	DEBUG_PRINT_FORCE ("  %3d = User Output5\n", GENICAM_COUNTER_RESET_SOURCE_USER5_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line6\n", GENICAM_COUNTER_RESET_SOURCE_LINE6);
	DEBUG_PRINT_FORCE ("  %3d = User Output6\n", GENICAM_COUNTER_RESET_SOURCE_USER6_OUTPUT);

	DEBUG_PRINT_FORCE ("  %3d = Counter0 Start\n", GENICAM_COUNTER_RESET_SOURCE_COUNTER0_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter0 End\n", GENICAM_COUNTER_RESET_SOURCE_COUNTER0_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter1 Start\n", GENICAM_COUNTER_RESET_SOURCE_COUNTER1_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter1 End\n", GENICAM_COUNTER_RESET_SOURCE_COUNTER1_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter2 Start\n", GENICAM_COUNTER_RESET_SOURCE_COUNTER2_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter2 End\n", GENICAM_COUNTER_RESET_SOURCE_COUNTER2_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter3 Start\n", GENICAM_COUNTER_RESET_SOURCE_COUNTER3_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter3 End\n", GENICAM_COUNTER_RESET_SOURCE_COUNTER3_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer0 Start\n", GENICAM_COUNTER_RESET_SOURCE_TIMER0_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer0 End\n", GENICAM_COUNTER_RESET_SOURCE_TIMER0_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer1 Start\n", GENICAM_COUNTER_RESET_SOURCE_TIMER1_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer1 End\n", GENICAM_COUNTER_RESET_SOURCE_TIMER1_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer2 Start\n", GENICAM_COUNTER_RESET_SOURCE_TIMER2_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer2 End\n", GENICAM_COUNTER_RESET_SOURCE_TIMER2_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer3 Start\n", GENICAM_COUNTER_RESET_SOURCE_TIMER3_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer3 End\n", GENICAM_COUNTER_EVENT_SOURCE_TIMER3_END);
#if defined (MODE_ENCODER)
	DEBUG_PRINT_FORCE ("  %3d = Encoder0\n", GENICAM_COUNTER_RESET_SOURCE_ENCODER0);
#endif
	DEBUG_PRINT_FORCE ("  %3d = OFF\n", GENICAM_COUNTER_RESET_SOURCE_OFF);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Reset Activation
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterResetActivation (void *str)
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
			cmdCounterResetActivationHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Reset Activation取得
		if ((status = counterGetResetActive (&activation)) != AVAL_STATUS_SUCCESS)
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

		// Counter Reset Activation設定
 		if ((status = counterSetResetActive (activation)) != AVAL_STATUS_SUCCESS)
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
//	Counter Reset Activation Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterResetActivationHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter reset activation is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-reset-activation\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Reset Activation\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter reset activation is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-reset-activation [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Counter Reset Activation\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Activation]\n");
	DEBUG_PRINT_FORCE ("  %d = Rising Edge\n", COUNTER_EVENT_ACTIVE_RISE_EDGE);
	DEBUG_PRINT_FORCE ("  %d = Faling Edge\n", COUNTER_EVENT_ACTIVE_FALL_EDGE);
	DEBUG_PRINT_FORCE ("  %d = Any Edge\n", COUNTER_EVENT_ACTIVE_ANY_EDGE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Reset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterReset (void *str)
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
			cmdCounterResetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Reset設定
		if ((status = counterReset ()) != AVAL_STATUS_SUCCESS)
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
int cmdCounterResetHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter reset is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-reset [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Value
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterValue (void *str)
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
			cmdCounterValueHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Value取得
		if ((status = counterGetValue (&value)) != AVAL_STATUS_SUCCESS)
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

		// Counter Reset Value設定
		if ((status = counterSetValue (value)) != AVAL_STATUS_SUCCESS)
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
//	Counter Value Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterValueHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter value is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-value\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Value(Min:%d - Max:%d)\n", GENICAM_COUNTER_RESET_VALUE_MIN, GENICAM_COUNTER_RESET_VALUE_MAX);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter value is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-value [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Counter Value(Min:%d - Max:%d)\n", GENICAM_COUNTER_RESET_VALUE_MIN, GENICAM_COUNTER_RESET_VALUE_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Reset Value
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterResetValue (void *str)
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
			cmdCounterResetValueHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Reset Value取得
		if ((status = counterGetResetValue (&value)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", value);
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
//	Counter Reset Value Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterResetValueHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter reset value is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-reset-value\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Reset Value(Min:%d - Max:%d)\n", GENICAM_COUNTER_RESET_VALUE_MIN, GENICAM_COUNTER_RESET_VALUE_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Reset Duration
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterDuratione (void *str)
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
			cmdCounterDurationeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Duration Value取得
		if ((status = counterGetDuration (&value)) != AVAL_STATUS_SUCCESS)
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

		// Counter Duration Value設定
		if ((status = counterSetDuration (value)) != AVAL_STATUS_SUCCESS)
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
//	Counter Reset Value Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterDurationeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter duration value is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-duration\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Duration Value(Min:%d - Max:%d)\n", GENICAM_COUNTER_DURATION_MIN, GENICAM_COUNTER_DURATION_MAX);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter duration value is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-duration [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Counter Duration Value(Min:%d - Max:%d)\n", GENICAM_COUNTER_DURATION_MIN, GENICAM_COUNTER_DURATION_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterStatus (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int counterStatus;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCounterStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Status取得
		if ((status = counterGetStatus (&counterStatus)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", counterStatus);
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
int cmdCounterStatusHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-status\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Status\n");
	
	DEBUG_PRINT_FORCE ("[Status]\n");
	DEBUG_PRINT_FORCE ("  %d = Idle\n", GENICAM_COUNTER_RESET_STATUS_IDLE);
	DEBUG_PRINT_FORCE ("  %d = Trg Wait\n", GENICAM_COUNTER_RESET_STATUS_TRG_WAIT);
	DEBUG_PRINT_FORCE ("  %d = Active\n", GENICAM_COUNTER_RESET_STATUS_ACTIVE);
	DEBUG_PRINT_FORCE ("  %d = Complated\n", GENICAM_COUNTER_RESET_STATUS_COMPLATED);
	DEBUG_PRINT_FORCE ("  %d = Over Flow\n", GENICAM_COUNTER_RESET_STATUS_OVERFLOW);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Trg Source
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterTrgSource (void *str)
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
			cmdCounterTrgSourceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Trg Source取得
		if ((status = counterGetTrgSource (&source)) != AVAL_STATUS_SUCCESS)
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

		// Counter Trg Source設定
		if ((status = counterSetTrgSource (source)) != AVAL_STATUS_SUCCESS)
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
//	Counter Trg Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterTrgSourceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter trg source is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-trg-source\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Trg Source\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter trg source is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-trg-source [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Counter Trg Source\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Source]\n");
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Start Trigger\n", GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_START_TRIGGER);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Start\n", GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_START);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition End\n", GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_END);
	DEBUG_PRINT_FORCE ("  %3d = Acquisition Invalid Trigger\n", GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_INVALID_TRG);
	DEBUG_PRINT_FORCE ("  %3d = Frame Invalid Trigger\n", GENICAM_COUNTER_TRG_SOURCE_FRAME_INVALID_TRG);
	DEBUG_PRINT_FORCE ("  %3d = Exposure Start\n", GENICAM_COUNTER_TRG_SOURCE_EXPOSURE_START);
	DEBUG_PRINT_FORCE ("  %3d = Exposure End\n", GENICAM_COUNTER_TRG_SOURCE_EXPOSURE_END);
	DEBUG_PRINT_FORCE ("  %3d = Exposure Invalid Trigger\n", GENICAM_COUNTER_TRG_SOURCE_EXPOSURE_INVALID_TRG);

	DEBUG_PRINT_FORCE ("  %3d = Line0\n", GENICAM_COUNTER_TRG_SOURCE_LINE0);
	DEBUG_PRINT_FORCE ("  %3d = User Output0\n", GENICAM_COUNTER_TRG_SOURCE_USER0_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line1\n", GENICAM_COUNTER_TRG_SOURCE_LINE1);
	DEBUG_PRINT_FORCE ("  %3d = User Output1\n", GENICAM_COUNTER_TRG_SOURCE_USER1_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line2\n", GENICAM_COUNTER_TRG_SOURCE_LINE2);
	DEBUG_PRINT_FORCE ("  %3d = User Output2\n", GENICAM_COUNTER_TRG_SOURCE_USER2_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line3\n", GENICAM_COUNTER_TRG_SOURCE_LINE3);
	DEBUG_PRINT_FORCE ("  %3d = User Output3\n", GENICAM_COUNTER_TRG_SOURCE_USER3_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line4\n", GENICAM_COUNTER_TRG_SOURCE_LINE4);
	DEBUG_PRINT_FORCE ("  %3d = User Output4\n", GENICAM_COUNTER_TRG_SOURCE_USER4_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line5\n", GENICAM_COUNTER_TRG_SOURCE_LINE5);
	DEBUG_PRINT_FORCE ("  %3d = User Output5\n", GENICAM_COUNTER_TRG_SOURCE_USER5_OUTPUT);
	DEBUG_PRINT_FORCE ("  %3d = Line6\n", GENICAM_COUNTER_TRG_SOURCE_LINE6);
	DEBUG_PRINT_FORCE ("  %3d = User Output6\n", GENICAM_COUNTER_TRG_SOURCE_USER6_OUTPUT);

	DEBUG_PRINT_FORCE ("  %3d = Counter0 Start\n", GENICAM_COUNTER_TRG_SOURCE_COUNTER0_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter0 End\n", GENICAM_COUNTER_TRG_SOURCE_COUNTER0_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter1 Start\n", GENICAM_COUNTER_TRG_SOURCE_COUNTER1_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter1 End\n", GENICAM_COUNTER_TRG_SOURCE_COUNTER1_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter2 Start\n", GENICAM_COUNTER_TRG_SOURCE_COUNTER2_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter2 End\n", GENICAM_COUNTER_TRG_SOURCE_COUNTER2_END);
	DEBUG_PRINT_FORCE ("  %3d = Counter3 Start\n", GENICAM_COUNTER_TRG_SOURCE_COUNTER3_START);
	DEBUG_PRINT_FORCE ("  %3d = Counter3 End\n", GENICAM_COUNTER_TRG_SOURCE_COUNTER3_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer0 Start\n", GENICAM_COUNTER_TRG_SOURCE_TIMER0_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer0 End\n", GENICAM_COUNTER_TRG_SOURCE_TIMER0_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer1 Start\n", GENICAM_COUNTER_TRG_SOURCE_TIMER1_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer1 End\n", GENICAM_COUNTER_TRG_SOURCE_TIMER1_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer2 Start\n", GENICAM_COUNTER_TRG_SOURCE_TIMER2_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer2 End\n", GENICAM_COUNTER_RESET_SOURCE_TIMER2_END);
	DEBUG_PRINT_FORCE ("  %3d = Timer3 Start\n", GENICAM_COUNTER_TRG_SOURCE_TIMER3_START);
	DEBUG_PRINT_FORCE ("  %3d = Timer3 End\n", GENICAM_COUNTER_TRG_SOURCE_TIMER3_END);
#if defined (MODE_ENCODER)
	DEBUG_PRINT_FORCE ("  %3d = Encoder0\n", GENICAM_COUNTER_TRG_SOURCE_ENCODER0);
#endif
	DEBUG_PRINT_FORCE ("  %3d = OFF\n", GENICAM_COUNTER_TRG_SOURCE_OFF);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Reset Activation
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterTrgActivation (void *str)
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
			cmdCounterTrgActivationHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Trg Activation取得
		if ((status = counterGetTrgActive (&activation)) != AVAL_STATUS_SUCCESS)
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

		// Counter Trg Activation設定
		if ((status = counterSetTrgActive (activation)) != AVAL_STATUS_SUCCESS)
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
//	Counter Reset Activation Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterTrgActivationHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter trg activation is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-trg-activation\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Counter Trg Activation\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Counter trg activation is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : counter-trg-activation [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Counter Trg Activation\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Activation]\n");
	DEBUG_PRINT_FORCE ("  %d = Rising Edge\n", COUNTER_TRG_ACTIVE_RISE_EDGE);
	DEBUG_PRINT_FORCE ("  %d = Faling Edge\n", COUNTER_TRG_ACTIVE_FALL_EDGE);
	DEBUG_PRINT_FORCE ("  %d = Any Edge\n", COUNTER_TRG_ACTIVE_ANY_EDGE);
	DEBUG_PRINT_FORCE ("  %d = High Level\n", COUNTER_TRG_ACTIVE_HI_LVLEL);
	DEBUG_PRINT_FORCE ("  %d = Low Level\n", COUNTER_TRG_ACTIVE_LO_LVLEL);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Set Trg Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterSetTrgMode (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int lineSrc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCounterSetTrgModeHelp (NULL);
			goto _DONE;
		}
	}


	if (argc == 1)
	{
		// Counter Trg Mode設定
		if ((status = counterGetTrgMode (&lineSrc)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", lineSrc);
	}
	else if (argc == 2)
	{
		// Line Source取得
		if (sscanf (gCmdArg[1], "%d", &lineSrc) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Counter Trg Mode設定
		if ((status = counterSetTrgMode (lineSrc)) != AVAL_STATUS_SUCCESS)
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
//	Counter Set Trg Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterSetTrgModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The triger count mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-countmode [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Triger Source\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Triger Source]\n");
	DEBUG_PRINT_FORCE ("  %3d = Line0\n", GENICAM_COUNTER_EVENT_SOURCE_LINE0);
	DEBUG_PRINT_FORCE ("  %3d = Line1\n", GENICAM_COUNTER_EVENT_SOURCE_LINE1);
	DEBUG_PRINT_FORCE ("  %3d = Line2\n", GENICAM_COUNTER_EVENT_SOURCE_LINE2);
	DEBUG_PRINT_FORCE ("  %3d = OFF\n", GENICAM_COUNTER_EVENT_SOURCE_OFF);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Get Trg Input Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterGetTrgInputCount (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int count;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCounterGetTrgInputCountHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Trg Input Count取得
		if ((status = counterGetTrgInputCount (&count)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", count);
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
//	Counter Get Trg Input Count Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterGetTrgInputCountHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The trigger input count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-signalcount\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Trigger input count\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Get Image Output Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterGetImageOutputCount (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int count;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCounterGetImageOutputCountHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Counter Image Output Count取得
		if ((status = counterGetImageOutputCount (&count)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", count);
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
//	Counter Get Image Output Count Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCounterGetImageOutputCountHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The image output count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-imgcount\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Image output count\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
