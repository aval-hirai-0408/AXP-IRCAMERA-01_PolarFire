//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdAcquisition.c - Genicam Acquisition Control Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionGcp (void *str)
{
	int argc;
	int status = AVAL_STATUS_SUCCESS;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAcquisitionGcpHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		//--------------------------------------------------
		// カメラ固有情報
		//--------------------------------------------------
		camerInfomation ();


		//--------------------------------------------------
		// カメラ情報
		//--------------------------------------------------
		acquisitionCamer ();

#if defined (MODE_AOI)
		//--------------------------------------------------
		// AOI
		//--------------------------------------------------
		aoi ();
#endif


#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
		//--------------------------------------------------
		// Auto Bright
		//--------------------------------------------------
		autoBrightInformation ();
#endif
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
//	Acquisition GCP Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionGcpHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Camera Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : camera\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Camera Information\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	カメラ情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int acquisitionCamer (void)
{
	cmdAcquisitionCamer (NULL);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	カメラ情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionCamer (void *str)
{
	unsigned int expTime;
	double frameRate;
	int mode;
	unsigned int frameCount;

	DEBUG_PRINT_FORCE ("[Measured values]\n");

	// フレームレート取得
	acquisitionGetFrameRate (&frameRate);
	DEBUG_PRINT_FORCE ("   Frame Rate                ");
	DEBUG_PRINT_FORCE ("%.2f fps\n", frameRate);

	// 露光時間取得
	acquisitionGetExposure (&expTime);
	DEBUG_PRINT_FORCE ("   Exposure Time             ");
	DEBUG_PRINT_FORCE ("%d us\n", expTime);

	// Acquisition Mode
	acquisitionGetMode (&mode);
	DEBUG_PRINT_FORCE ("   Acquisition Mode          ");
	if (mode == ACQUISITION_MODE_SINGLE)
		DEBUG_PRINT_FORCE ("Single\n");
	else if (mode == ACQUISITION_MODE_MULTI)
		DEBUG_PRINT_FORCE ("Multi\n");
	else if (mode == ACQUISITION_MODE_CONTIN)
		DEBUG_PRINT_FORCE ("Continuous\n");

	// Acquisition Frame Count
	if (mode == ACQUISITION_MODE_MULTI)
	{
		acquisitionGetFrameCount (&frameCount);
		DEBUG_PRINT_FORCE ("   Frame Count               ");
		DEBUG_PRINT_FORCE ("%d\n", frameCount);
	}

	// Exposure Mode
	acquisitionGetExposureMode (&mode);

	DEBUG_PRINT_FORCE ("   Exposure Mode             ");
	if (mode == ACQUISITION_EXPOSURE_TIMED)
		DEBUG_PRINT_FORCE ("Timed\n");
	else if (mode == ACQUISITION_EXPOSURE_TRG_WIDTH)
		DEBUG_PRINT_FORCE ("Trg Width\n");
	else if (mode == ACQUISITION_EXPOSURE_TRG_CTRL)
		DEBUG_PRINT_FORCE ("Trg Control\n");
	else
		DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Acquisition Status表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int acquisitionStatusAll (void)
{
	cmdAcquisitionStatusAll (NULL);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Acquisition Status表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionStatusAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	int acqStatus;
	int select;
	unsigned int delay, count;
	int saveSelect;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAcquisitionStatusAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get Selector
		if ((status = acquisitionGetSelect (&saveSelect)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//----------------------------------------------------------------------
		// Acquisition Status
		//----------------------------------------------------------------------
		DEBUG_PRINT_FORCE ("\n");
		DEBUG_PRINT_FORCE ("[Acquisition Status]\n");
		for (mode = ACQUISITION_STATUS_MIN; mode <= ACQUISITION_STATUS_MAX; mode++)
		{
			if ((status = acquisitionGetStatus (mode, &acqStatus)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			if (mode == ACQUISITION_STATUS_ACQUISITION_TRGWAIT)
				DEBUG_PRINT_FORCE ("   Acquisition Trg Wait      ");
			else if (mode == ACQUISITION_STATUS_ACQUISITION_ACTIVE)
				DEBUG_PRINT_FORCE ("   Acquisition Active        ");
			else if (mode == ACQUISITION_STATUS_ACQUISITION_TRANSFER)
				DEBUG_PRINT_FORCE ("   Acquisition Transfer      ");
			else if (mode == ACQUISITION_STATUS_FRAME_TRGWAIT)
				DEBUG_PRINT_FORCE ("   Frame Trg Wait            ");
			else if (mode == ACQUISITION_STATUS_FRAME_ACTIVE)
				DEBUG_PRINT_FORCE ("   Frame Active              ");
			else if (mode == ACQUISITION_STATUS_EXPOSURE_ACTIVE)
				DEBUG_PRINT_FORCE ("   Exposure Active           ");
			else
				DEBUG_PRINT_FORCE ("\n");

			if (acqStatus == 1)
				DEBUG_PRINT_FORCE (": TRUE\n");
			else
				DEBUG_PRINT_FORCE (": FALSE\n");
		}

		DEBUG_PRINT_FORCE ("\n");
		DEBUG_PRINT_FORCE ("[Acquisition Setting]");
		for (select = ACQUISITION_TRG_SELECT_MIN; select <= ACQUISITION_TRG_SELECT_MAX; select++)
		{
			switch (select)
			{
				case ACQUISITION_TRG_SELECT_ACQUISITION_START:
				case ACQUISITION_TRG_SELECT_ACQUISITION_END:
				case ACQUISITION_TRG_SELECT_ACQUISITION_ACTIVE:
				case ACQUISITION_TRG_SELECT_FRAME_ACTIVE:
					break;
				default:
					continue;
			}

			//----------------------------------------------------------------------
			// Acquisition Trg Select
			//----------------------------------------------------------------------
			if ((status = acquisitionSetSelect (select)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("\n   Selector                  ");
			if (select == ACQUISITION_TRG_SELECT_ACQUISITION_START)
				DEBUG_PRINT_FORCE (": Acquisition Start\n");
			else if (select == ACQUISITION_TRG_SELECT_ACQUISITION_END)
				DEBUG_PRINT_FORCE (": Acquisition End\n");
			else if (select == ACQUISITION_TRG_SELECT_ACQUISITION_ACTIVE)
				DEBUG_PRINT_FORCE (": Acquisition Active\n");
			else if (select == ACQUISITION_TRG_SELECT_FRAME_START)
				DEBUG_PRINT_FORCE (": Exposure Start\n");
			else if (select == ACQUISITION_TRG_SELECT_FRAME_END)
				DEBUG_PRINT_FORCE (": Exposure End\n");
			else if (select == ACQUISITION_TRG_SELECT_FRAME_ACTIVE)
				DEBUG_PRINT_FORCE (": Exposure Active\n");
			else if (select == ACQUISITION_TRG_SELECT_EXPOSURE_START)
				DEBUG_PRINT_FORCE (": Exposure Start\n");
			else if (select == ACQUISITION_TRG_SELECT_EXPOSURE_END)
				DEBUG_PRINT_FORCE (": Exposure End\n");
			else if (select == ACQUISITION_TRG_SELECT_EXPOSURE_ACTIVE)
				DEBUG_PRINT_FORCE (": Exposure Active\n");
			else
				DEBUG_PRINT_FORCE ("\n");
			
			//----------------------------------------------------------------------
			// Acquisition Mode
			//----------------------------------------------------------------------
			if ((status = acquisitionGetTrgMode (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Mode                     ");
			if (mode == ACQUISITION_TRG_MODE_DISABLE)
				DEBUG_PRINT_FORCE (" : Disable\n");
			else if (mode == ACQUISITION_TRG_MODE_ENABLE)
				DEBUG_PRINT_FORCE (" : Enable\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Acquisition Trg Source
			//----------------------------------------------------------------------
			if ((status = acquisitionGetTrgSource (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Source                   ");
			if (mode == GENICAM_ACQUISITION_TRG_SOURCE_SOFT)
				DEBUG_PRINT_FORCE (" : Soft Trg\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_LINE0)
				DEBUG_PRINT_FORCE (" : LINE0\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_USER0_OUTPUT)
				DEBUG_PRINT_FORCE (" : User0 Output\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_LINE1)
				DEBUG_PRINT_FORCE (" : LINE1\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_USER1_OUTPUT)
				DEBUG_PRINT_FORCE (" : User1 Output\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_LINE2)
				DEBUG_PRINT_FORCE (" : LINE2\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_USER2_OUTPUT)
				DEBUG_PRINT_FORCE (" : User2 Output\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_LINE3)
				DEBUG_PRINT_FORCE (" : LINE3\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_USER3_OUTPUT)
				DEBUG_PRINT_FORCE (" : User3 Output\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_LINE4)
				DEBUG_PRINT_FORCE (" : LINE4\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_USER4_OUTPUT)
				DEBUG_PRINT_FORCE (" : User4 Output\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_LINE5)
				DEBUG_PRINT_FORCE (" : LINE5\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_USER5_OUTPUT)
				DEBUG_PRINT_FORCE (" : User5 Output\n");
			#if 0
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_CC1)
				DEBUG_PRINT_FORCE (" : CC1\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_CC2)
				DEBUG_PRINT_FORCE (" : CC2\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_CC3)
				DEBUG_PRINT_FORCE (" : CC3\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_CC4)
				DEBUG_PRINT_FORCE (" : CC4\n");
			#endif
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_COUNTER0_START)
				DEBUG_PRINT_FORCE (" : Counter0 Start\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_COUNTER0_END)
				DEBUG_PRINT_FORCE (" : Counter0 End\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_COUNTER1_START)
				DEBUG_PRINT_FORCE (" : Counter1 Start\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_COUNTER1_END)
				DEBUG_PRINT_FORCE (" : Counter1 End\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_COUNTER2_START)
				DEBUG_PRINT_FORCE (" : Counter2 Start\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_COUNTER2_END)
				DEBUG_PRINT_FORCE (" : Counter2 End\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_COUNTER3_START)
				DEBUG_PRINT_FORCE (" : Counter3 Start\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_COUNTER3_END)
				DEBUG_PRINT_FORCE (" : Counter3 End\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_TIMER0_START)
				DEBUG_PRINT_FORCE (" : Timer0 Start\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_TIMER0_END)
				DEBUG_PRINT_FORCE (" : Timer0 End\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_TIMER1_START)
				DEBUG_PRINT_FORCE (" : Timer1 Start\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_TIMER1_END)
				DEBUG_PRINT_FORCE (" : Timer2 End\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_TIMER2_START)
				DEBUG_PRINT_FORCE (" : Timer3 Start\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_TIMER2_END)
				DEBUG_PRINT_FORCE (" : Timer3 End\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_TIMER3_START)
				DEBUG_PRINT_FORCE (" : Timer3 Start\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_TIMER3_END)
				DEBUG_PRINT_FORCE (" : Timer3 End\n");
			else if (mode == GENICAM_ACQUISITION_TRG_SOURCE_ENCODER0)
				DEBUG_PRINT_FORCE (" : Encoder0\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Acquisition Trg Activation
			//----------------------------------------------------------------------
			if ((status = acquisitionGetTrgActivation (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Activation               ");
			if (mode == ACQUISITION_TRG_ACTIVATION_RISE_EDGE)
				DEBUG_PRINT_FORCE (" : Rise Edge\n");
			else if (mode == ACQUISITION_TRG_ACTIVATION_FALL_EDGE)
				DEBUG_PRINT_FORCE (" : Fall Edge\n");
			else if (mode == ACQUISITION_TRG_ACTIVATION_ANY_EDGE)
				DEBUG_PRINT_FORCE (" : Any Edge\n");
			else if (mode == ACQUISITION_TRG_ACTIVATION_HI_LEVEL)
				DEBUG_PRINT_FORCE (" : High Level\n");
			else if (mode == ACQUISITION_TRG_ACTIVATION_LO_LEVEL)
				DEBUG_PRINT_FORCE (" : Low Level\n");
			else
				DEBUG_PRINT_FORCE ("\n");

			//----------------------------------------------------------------------
			// Acquisition Trg Delay
			//----------------------------------------------------------------------
			if ((status = acquisitionGetTrgDelay (&delay)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("   Delay                    ");
			DEBUG_PRINT_FORCE (" : %d[us]\n", delay);
		}

		DEBUG_PRINT_FORCE ("\n");

		// Set Selector
		if ((status = acquisitionSetSelect (saveSelect)) != AVAL_STATUS_SUCCESS)
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
//	Acquisition Status表示Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionStatusAllHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Acquisition Status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : acquisitioncontrol-status\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Acquisition Status\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	画像入力モード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionMode (void *str)
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
			cmdAcquisitionModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 画像モード取得
		if ((status = acquisitionGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// 画像モード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 画像モード設定
		if ((status = acquisitionSetMode (mode)) != AVAL_STATUS_SUCCESS)
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
//	画像入力モードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Acquisition mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : acquisitionmode/mode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Acquisition Mode\n");
	
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Acquisition mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : acquisitionmode/mode [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Acquisition Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %d = Single\n", GENICAM_ACQUISITION_MODE_SINGLE_BIT);
	DEBUG_PRINT_FORCE ("  %d = Multi\n", GENICAM_ACQUISITION_MODE_MULTI_BIT);
	DEBUG_PRINT_FORCE ("  %d = Continuouse\n", GENICAM_ACQUISITION_MODE_CONTIN_BIT);

	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	画像入力開始
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionStart (void *str)
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
			cmdAcquisitionStartHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 画像入力開始
		if ((status = acquisitionStart ()) != AVAL_STATUS_SUCCESS)
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
//	画像入力開始Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionStartHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Acquisition Start.\n");
	DEBUG_PRINT_FORCE ("  Command           : acquisitionstopstart/start\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	画像入力停止
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionStop (void *str)
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
			cmdAcquisitionStopHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 画像入力停止
		if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
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
//	画像入力停止Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionStopHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Acquisition Stop.\n");
	DEBUG_PRINT_FORCE ("  Command           : acquisitionstop/stop\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	フレームカウント
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionFrameCount (void *str)
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
			cmdAcquisitionFrameCountHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// フレームカウント取得
		if ((status = acquisitionGetFrameCount (&count)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%u ", count);
	}
	else if (argc == 2)
	{
		// フレームカウント取得
		if (sscanf (gCmdArg[1], "%u", &count) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// フレームカウント設定
		if ((status = acquisitionSetFrameCount (count)) != AVAL_STATUS_SUCCESS)
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
//	画像入力モードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionFrameCountHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Acquisition frame count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : acquisitionframecount/framecount\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Acquisition Frame Count(Min:%d - Max:%u)\n", GENICAM_ACQUISITION_FRAME_COUNT_MIN, GENICAM_ACQUISITION_FRAME_COUNT_MAX);

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Acquisition frame count is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : acquisitionframecount/framecount [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Acquisition Frame Count(Min:%d - Max:%u)\n", GENICAM_ACQUISITION_FRAME_COUNT_MIN, GENICAM_ACQUISITION_FRAME_COUNT_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Acquisition Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionStatus (void *str)
{
	int status;
	int argc;
	int mode, acquisitionStatus;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAcquisitionStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// ステータス選択
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// ステータス取得
		if ((status = acquisitionGetStatus (mode, &acquisitionStatus)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", acquisitionStatus);
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
//	Acquisition Status取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionStatusHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Acquisition status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : acquisitionstatus/acstatus\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Status Select\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Acquisition status(0/1)\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Selector]\n");
	DEBUG_PRINT_FORCE ("  %d : Acquisition Trigger Wait\n", ACQUISITION_STATUS_ACQUISITION_TRGWAIT);
	DEBUG_PRINT_FORCE ("  %d : Acquisition Active\n", ACQUISITION_STATUS_ACQUISITION_ACTIVE);
	DEBUG_PRINT_FORCE ("  %d : Acquisition Transfer\n", ACQUISITION_STATUS_ACQUISITION_TRANSFER);
	DEBUG_PRINT_FORCE ("  %d : Frame Trigger Wait\n", ACQUISITION_STATUS_FRAME_TRGWAIT);
	DEBUG_PRINT_FORCE ("  %d : Frame Active\n", ACQUISITION_STATUS_FRAME_ACTIVE);
	DEBUG_PRINT_FORCE ("  %d : Exposure Active\n", ACQUISITION_STATUS_EXPOSURE_ACTIVE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	レート(fps/Hz単位)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionFramerate (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double frameRate;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAcquisitionFramerateHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// フレームレート取得
		if ((status = acquisitionGetFrameRate (&frameRate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", frameRate);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%lf", &frameRate) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// フレームレート設定
		if ((status = acquisitionSetFrameRate (frameRate)) != AVAL_STATUS_SUCCESS)
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
//	フレームレート(fps単位)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionFramerateHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	double rMin, rMax;

	// 最小レート時間取得
	if ((status = rateMin (&rMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大レート時間取得
	if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The frame rate is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : framerate/frame\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : framerate[fps])\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The frame rate is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : framerate/frame [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : framerate[fps]\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Frame Rate]\n");
	DEBUG_PRINT_FORCE ("  Min : %.2f[fps]\n", rMin);
	DEBUG_PRINT_FORCE ("  Max : %.2f[fps]\n", rMax);
	DEBUG_PRINT_FORCE ("\n");

_DONE:
	return (status);
}


//**********************************************************************************
//	露光時間
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionExposure (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int expTime;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAcquisitionExposureHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 露光時間取得
		if ((status = acquisitionGetExposure (&expTime)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", expTime);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &expTime) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 露光時間設定
		if ((status = acquisitionSetExposure (expTime)) != AVAL_STATUS_SUCCESS)
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
//	露光時間Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionExposureHelp (void *str)
{
	int status;
	unsigned int expTime;
	int expMin, expMax;

	// 最小露光時間取得
	exposureMin (&expMin);

	// 最大露光時間取得
	exposureMax (&expMax);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Exposure is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : exposure/exp\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Exposure Time[us] (Min:%d / Max:%d)\n", expMin, expMax);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Exposure is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : exposure/exp [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Exposure Time[us] (Min:%d / Max:%d)\n", expMin, expMax);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	トリガ セレクト
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgSelect (void *str)
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
			cmdAcquisitionTrgSelectHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// トリガセレクト取得
		if ((status = acquisitionGetSelect (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// トリガセレクト取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// トリガセレクト設定
		if ((status = acquisitionSetSelect (mode)) != AVAL_STATUS_SUCCESS)
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
//	トリガ セレクトHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgSelectHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : A trigger choice is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-selector\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Trigger Selector\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A trigger is chosen.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-selector\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Trigger Selector\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Selector]\n");
	DEBUG_PRINT_FORCE ("  %2d : Acquisition Start\n", ACQUISITION_TRG_SELECT_ACQUISITION_START);
	DEBUG_PRINT_FORCE ("  %2d : Acquisition End\n", ACQUISITION_TRG_SELECT_ACQUISITION_END);
	DEBUG_PRINT_FORCE ("  %2d : Acquisition Active\n", ACQUISITION_TRG_SELECT_ACQUISITION_ACTIVE);
	DEBUG_PRINT_FORCE ("  %2d : Exposure Active\n", ACQUISITION_TRG_SELECT_FRAME_ACTIVE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	トリガモード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgMode (void *str)
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
			cmdAcquisitionTrgModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// トリガ有効/無効状態取得
		if ((status = acquisitionGetTrgMode (&mode)) != AVAL_STATUS_SUCCESS)
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

		// トリガ設定
		if ((status = acquisitionSetTrgMode (mode)) != AVAL_STATUS_SUCCESS)
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
//	トリガモードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Trigger Mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-mode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Trigger Mode(%d:Disable / %d:Enable)\n", ACQUISITION_TRG_MODE_DISABLE, ACQUISITION_TRG_MODE_ENABLE);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Trigger Mode is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-mode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Trigger Mode(%d:Disable / %d:Enable)\n", ACQUISITION_TRG_MODE_DISABLE, ACQUISITION_TRG_MODE_ENABLE);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ソフトトリガ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionSoftTrg (void *str)
{
	int status;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAcquisitionSoftTrgHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// ソフトトリガ設定
		if ((status = acquisitionSetSoftTrg ()) != AVAL_STATUS_SUCCESS)
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
//	ソフトトリガ Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionSoftTrgHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Soft Trigger is issued.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-soft\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	トリガソース
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgSource (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int number;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAcquisitionTrgSourceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// トリガソース取得
		if ((status = acquisitionGetTrgSource (&number)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", number);
	}
	else if (argc == 2)
	{
		// トリガソース番号取得
		if (sscanf (gCmdArg[1], "%d", &number) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// トリガソース設定
		if ((status = acquisitionSetTrgSource (number)) != AVAL_STATUS_SUCCESS)
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
//	トリガ条件Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgSourceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Trigger Source is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-source\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Trigger Source\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Trigger Source is Set.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-source\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Trigger Source\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Source]\n");
	DEBUG_PRINT_FORCE ("  %3d = Soft Trigger\n", GENICAM_ACQUISITION_TRG_SOURCE_SOFT);
	DEBUG_PRINT_FORCE ("  %3d = Line0\n", GENICAM_ACQUISITION_TRG_SOURCE_LINE0);
	DEBUG_PRINT_FORCE ("  %3d = Line1\n", GENICAM_ACQUISITION_TRG_SOURCE_LINE1);
	DEBUG_PRINT_FORCE ("  %3d = Line2\n", GENICAM_ACQUISITION_TRG_SOURCE_LINE2);
	DEBUG_PRINT_FORCE ("  %3d = CC1\n", GENICAM_ACQUISITION_TRG_SOURCE_CC1);
	DEBUG_PRINT_FORCE ("  %3d = CC2\n", GENICAM_ACQUISITION_TRG_SOURCE_CC2);
	DEBUG_PRINT_FORCE ("  %3d = CC3\n", GENICAM_ACQUISITION_TRG_SOURCE_CC3);
	DEBUG_PRINT_FORCE ("  %3d = CC4\n", GENICAM_ACQUISITION_TRG_SOURCE_CC4);
	DEBUG_PRINT_FORCE ("  %3d = Encoder0\n", GENICAM_ACQUISITION_TRG_SOURCE_ENCODER0);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	トリガActivation
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgActivation (void *str)
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
			cmdAcquisitionTrgActivationHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// トリガActivation取得
		if ((status = acquisitionGetTrgActivation (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// トリガActivation取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// トリガActivation設定
		if ((status = acquisitionSetTrgActivation (mode)) != AVAL_STATUS_SUCCESS)
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
//	トリガActivationHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgActivationHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Trigger Activation is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-activation\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Trigger Activation (Min:%d - Max:%d)\n", ACQUISITION_TRG_ACTIVATION_MIN, ACQUISITION_TRG_ACTIVATION_MAX);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Trigger Activation is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-activation [Param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : Trigger Activation (Min:%d - Max:%d)\n", ACQUISITION_TRG_ACTIVATION_MIN, ACQUISITION_TRG_ACTIVATION_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Activation]\n");
	DEBUG_PRINT_FORCE ("  %d = Rising Edge\n", ACQUISITION_TRG_ACTIVATION_RISE_EDGE);
	DEBUG_PRINT_FORCE ("  %d = Falling Edge\n", ACQUISITION_TRG_ACTIVATION_FALL_EDGE);
	DEBUG_PRINT_FORCE ("  %d = Any Edge\n", ACQUISITION_TRG_ACTIVATION_ANY_EDGE);
	DEBUG_PRINT_FORCE ("  %d = High Level\n", ACQUISITION_TRG_ACTIVATION_HI_LEVEL);
	DEBUG_PRINT_FORCE ("  %d = Low Level\n", ACQUISITION_TRG_ACTIVATION_LO_LEVEL);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	トリガDelay
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgDelay (void *str)
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
			cmdAcquisitionTrgDelayHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// トリガDelay取得
		if ((status = acquisitionGetTrgDelay (&delay)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", delay);
	}
	else if (argc == 2)
	{
		// トリガDelay取得
		if (sscanf (gCmdArg[1], "%d", &delay) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// トリガDelay設定
		if ((status = acquisitionSetTrgDelay (delay)) != AVAL_STATUS_SUCCESS)
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
//	トリガDelayHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgDelayHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Trigger Delay(us) is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-delay\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Trigger Delay(us) (Min:%d - Max:%d)\n", GENICAM_ACQUISITION_TRG_DELAY_MIN, GENICAM_ACQUISITION_TRG_DELAY_MAX);
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Trigger Delay(us) is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-delay [Param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : Trigger Delay(us) (Min:%d - Max:%d)\n", GENICAM_ACQUISITION_TRG_DELAY_MIN, GENICAM_ACQUISITION_TRG_DELAY_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Exposureモード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionExposureMode (void *str)
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
			cmdAcquisitionExposureModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 露光モード取得
		if ((status = acquisitionGetExposureMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 露光モード設定
		status = acquisitionSetExposureMode (mode);
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
//	ExposureモードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionExposureModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The exposure mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : exp-mode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Exposure Mode\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The exposure mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : exp-mode [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Exposure Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %d = Timed\n", ACQUISITION_EXPOSURE_TIMED);
	DEBUG_PRINT_FORCE ("  %d = Trigger Width\n", ACQUISITION_EXPOSURE_TRG_WIDTH);
	//DEBUG_PRINT_FORCE ("  %d = Trigger Control\n", ACQUISITION_EXPOSURE_TRG_CTRL);
	
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	無効トリガカウント
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgInvalidCount (void *str)
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
			cmdAcquisitionTrgInvalidCountHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 無効トリガカウント取得
		if ((status = acquisitionGetTrgInvalidCount (&count)) != AVAL_STATUS_SUCCESS)
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
//	Frames無効ActiveトリガカウントHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgInvalidCountHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Trigger Invalid Count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-invalidcount\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Invalid Trigger Trigger Count\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	トリガ予約モード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgReserved (void *str)
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
			cmdAcquisitionTrgReservedHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// トリガReserver Mode取得
		if ((status = acquisitionGetTrgReserve (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// Reserver Mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// トリガReserver Mode設定
		if ((status = acquisitionSetTrgReserve (mode)) != AVAL_STATUS_SUCCESS)
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
//	トリガ予約モードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgReservedHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The trigger reserved mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-reserved\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : trigger reserved mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The trigger reserved mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-reserved [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : trigger reserved mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[trigger reserved mode]\n");
	DEBUG_PRINT_FORCE ("  0 : Disable\n");
	DEBUG_PRINT_FORCE ("  1 : Enable\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Acquisition Reset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionReset (void *str)
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
			cmdAcquisitionResetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Reset
		if ((status = acquisitionReset ()) != AVAL_STATUS_SUCCESS)
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
//	Acquisition Reset Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionResetHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Acquisition Reset.\n");
	DEBUG_PRINT_FORCE ("  Command           : acquisition-reset/acreset\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Acquisition Reset.\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	最小露光時間
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdExposureMin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int time;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdExposureMinHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 最小露光時間取得
		if ((status = exposureMin (&time)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", time);
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
//	最小露光時間Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdExposureMinHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The exposure minimum time is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : exposuremin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : exposure minimum time[us]\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	最大露光時間
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdExposureMax (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int time;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdExposureMaxHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 最小大露光時間取得
		if ((status = exposureMax (&time)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", time);
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
//	最大露光時間Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdExposureMaxHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The exposure maximum time is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : exposuremax\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : exposure maximum time[us]\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	最小レート
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRateMin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double rate;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdRateMinHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 最小レート時間取得
		if ((status = rateMin (&rate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", rate);
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
//	最小レートHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRateMinHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The rate minimum time is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ratemin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : rate minimum[fps]\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	最大レート
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRateMax (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double rate;
	unsigned int rateI;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdRateMaxHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 最大レート取得
		if ((status = rateMax (&rate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		rate *= 100;
		rateI = (unsigned int)rate;
		rate = (double)((double)rateI / (double)100);
		DEBUG_PRINT_FORCE ("%.2f ", rate);
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
//	最大レートHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRateMaxHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The rate maximum time is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ratemax\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : rate maximum[fps]\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	センサRead Out時間
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensoreGetReadOut (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int time;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensoreGetReadOutHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// センサRead Out時間取得
		if ((status = sensoreGetReadOut (&time)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", time);
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
//	センサRead Out時間Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensoreGetReadOutHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The sensor read out time is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensor-read-out\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : sensor read out time[us]\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
//	Rate Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRateMode (void *str)
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
			cmdRateModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Rate Mode取得
		if ((status = acquisitionGetRateMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// Rate Mode取得
		if (sscanf (gCmdArg[1], "%u", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Rate Mode設定
		if ((status = acquisitionSetRateMode (mode)) != AVAL_STATUS_SUCCESS)
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
//	Reate Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRateModeHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The rate mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ratemode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Rate Mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The rate mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ratemode [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Rate Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Rate Mode]\n");
	DEBUG_PRINT_FORCE ("  Normal : %d\n", MODE_NORMAL);
	DEBUG_PRINT_FORCE ("  Extend : %d\n", MODE_EXTEND);

	DEBUG_PRINT_FORCE ("\n");

	return (status);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
//	Trg Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgCount (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	int argc;
	unsigned int dataH;
	int port;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAcquisitionTrgCountHelp (NULL);
			goto _DONE;
		}
	}

	if (gInterFaceID != INTERFACE_CXP)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

#if !defined (MODE_CXP_MULTI_PORT)
	if (argc == 1)
	{
		port = 0;

		// Get Trg Count
		if ((status = acquisitionGetTrgCountHigh (port, &dataH)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", dataH);
	}
#else
	if (argc == 2)
	{
		// port取得
		if (sscanf (gCmdArg[1], "%d", &port) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Get Trg Count
		if ((status = acquisitionGetTrgCountHigh (port, &dataH)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", dataH);
	}
#endif
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
//	Trg Count Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcquisitionTrgCountHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Trg count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : trg-count [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : port\n");
	DEBUG_PRINT_FORCE ("  Output0 Param     : Trg Count\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
