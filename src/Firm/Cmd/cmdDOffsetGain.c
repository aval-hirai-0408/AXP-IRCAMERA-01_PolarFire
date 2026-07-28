//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdDOffsetGain.c - Digital Offset Gain Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	Digital Offset gain情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int dog (void)
{
	int status;

	status = cmdDog (NULL);
	return (status);
}


//**********************************************************************************
//	Digital Offset Gain表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDog (void *str)
{
	int status;
	int mode;
	int offset1, offset2;
	float gain;
	int high, low;

	//--------------------------------------------------
	// Digital Offset Gain情報
	//--------------------------------------------------
	DEBUG_PRINT_FORCE ("\n[Digital Offset Gain informations]\n");

	//--------------------------------------------------
	// Offset Gain
	//--------------------------------------------------
	if ((status = dogGetOffsetGain (&offset1, &offset2, &gain)) != AVAL_STATUS_SUCCESS)
		return (status);

	DEBUG_PRINT_FORCE ("   Offset1                   ");
	DEBUG_PRINT_FORCE ("%d\n", offset1);

	DEBUG_PRINT_FORCE ("   Offset2                   ");
	DEBUG_PRINT_FORCE ("%d\n", offset2);

	DEBUG_PRINT_FORCE ("   Gain                      ");
	DEBUG_PRINT_FORCE ("%f\n", gain);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Digital Gain
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalGain (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	float gain;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalGainHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Digital Offset Gain取得
		if ((status = digitalGetGain (&gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", gain);
	}
	else if (argc == 2)
	{
		// gain
		if (sscanf (gCmdArg[1], "%f", &gain) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Digital Gain設定
		if ((status = digitalSetGain (gain)) != AVAL_STATUS_SUCCESS)
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
//	Digital Gain Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalGainHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Digital Gain is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gain\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : gain\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Digital Gain is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : gain [Param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : gain\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Gain (dB Unit)]\n");
	DEBUG_PRINT_FORCE ("  Min : %.2f\n", DOG_DECIBEL_MIN);
	DEBUG_PRINT_FORCE ("  Max : %.2f\n", DOG_DECIBEL_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Digital Gain(倍率)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalGainX (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	float gain;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDigitalGainXHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Digital Offset Gain取得
		if ((status = digitalGetGainX (&gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", gain);
	}
	else if (argc == 2)
	{
		// gain
		if (sscanf (gCmdArg[1], "%f", &gain) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Digital Gain設定
		if ((status = digitalSetGainX (gain)) != AVAL_STATUS_SUCCESS)
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
//	Digital Gain Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalGainXHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Digital Gain is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gainx\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : gain\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Digital Gain is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : gainx [Param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : gain\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Gain (magnification unit)]\n");
	DEBUG_PRINT_FORCE ("  Min : %.2f\n", DOG_GAIN_MIN);
	DEBUG_PRINT_FORCE ("  Max : %.2f\n", DOG_GAIN_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Digital Offset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalOffset (void *str)
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
			cmdDigitalOffsetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Digital Offset Gain取得
		if ((status = digitalGetOffset (&offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", offset);
	}
	else if (argc == 2)
	{
		// offset
		if (sscanf (gCmdArg[1], "%d", &offset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Digital Offset設定
		if ((status = digitalSetOffset (offset)) != AVAL_STATUS_SUCCESS)
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
//	Digital Offset Gain Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalOffsetHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Digital Offset is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : blacklevel\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Offset\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Digital Offset is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : blacklevel [Param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : Offset\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Offset]\n");
#if (MODE_CAMERA_BIT >= PIXEL_8BIT)
	DEBUG_PRINT_FORCE ("  8bit  :Min:%6d - Max:%6d\n", DOG_OFFSET2_8BIT_MIN, DOG_OFFSET2_8BIT_MAX);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_10BIT)
	DEBUG_PRINT_FORCE ("  10bit :Min:%6d - Max:%6d\n", DOG_OFFSET2_10BIT_MIN, DOG_OFFSET2_10BIT_MAX);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_12BIT)
	DEBUG_PRINT_FORCE ("  12bit :Min:%6d - Max:%6d\n", DOG_OFFSET2_12BIT_MIN, DOG_OFFSET2_12BIT_MAX);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_14BIT)
	DEBUG_PRINT_FORCE ("  14bit :Min:%6d - Max:%6d\n", DOG_OFFSET2_14BIT_MIN, DOG_OFFSET2_14BIT_MAX);
#endif
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Digital Offset1
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalOffset1 (void *str)
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
			cmdDigitalOffset1Help (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Digital Offset取得
		if ((status = digitalGetOffset1 (&offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", offset);
	}
	else if (argc == 2)
	{
		// offset
		if (sscanf (gCmdArg[1], "%d", &offset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Digital Offset設定
		if ((status = digitalSetOffset1 (offset)) != AVAL_STATUS_SUCCESS)
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
//	Digital Offset1 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDigitalOffset1Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Digital Offset1 is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : blacklevel1\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Offset\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Digital Offset1 is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : blacklevel1 [Param]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : Offset\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Offset]\n");

#if (MODE_CAMERA_BIT >= PIXEL_8BIT)
	DEBUG_PRINT_FORCE ("  8bit  :Min:%6d - Max:%6d\n", DOG_OFFSET1_8BIT_MIN, DOG_OFFSET1_8BIT_MAX);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_10BIT)
	DEBUG_PRINT_FORCE ("  10bit :Min:%6d - Max:%6d\n", DOG_OFFSET1_10BIT_MIN, DOG_OFFSET1_10BIT_MAX);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_12BIT)
	DEBUG_PRINT_FORCE ("  12bit :Min:%6d - Max:%6d\n", DOG_OFFSET1_12BIT_MIN, DOG_OFFSET1_12BIT_MAX);
#endif

#if (MODE_CAMERA_BIT >= PIXEL_14BIT)
	DEBUG_PRINT_FORCE ("  14bit :Min:%6d - Max:%6d\n", DOG_OFFSET1_14BIT_MIN, DOG_OFFSET1_14BIT_MAX);
#endif

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
