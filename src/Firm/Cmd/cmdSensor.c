//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdSensor.c - Sensor Command Program
//**********************************************************************************


//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"

#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
#include "../Common/sensorVendors.h"
#endif


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
//	センサタイミング
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorTmg (void *str)
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
			cmdSensorTmgHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
#if defined (MODE_SENSOR_IMX990) || defined (MODE_SENSOR_IMX991) || defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		if ((status = sensorTimingInitialize ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	センサタイミングHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorTmgHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Sensor Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensortmg\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Sensor Information\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	BlackPixel
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorBlackPixel (void *str)
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
			cmdSensorBlackPixelHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// BlackPixel取得
		if ((status = sensorGetBlackPixel (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// Mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// BlackPixel設定
		if ((status = sensorSetBlackPixel (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	BlackPixel Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorBlackPixelHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor black pixel is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : blackpixel [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Black Pixel Mode\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor black pixel is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : blackpixel [param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Black Pixel Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  0 : Disable\n");
	DEBUG_PRINT_FORCE ("  1 : Enable\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Sensor Gain(dB)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorGainDB (void *str)
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
			cmdSensorGainDBHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Sensor Offset Gain取得
		if ((status = sensorGetGainDB (&gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", gain);
	}
	else if (argc == 2)
	{
		// gain
		if (sscanf (gCmdArg[1], "%f", &gain) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Sensor Gain設定
		if ((status = sensorSetGainDB (gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Gain(dB) Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorGainDBHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Gain is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensorgain\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : gain\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Gain is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensorgain [Param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : gain(dB)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Gain (dB Unit)]\n");
	DEBUG_PRINT_FORCE ("  Min : %.02f\n", SENSOR_REG_GAIN_DB_MIN);
	DEBUG_PRINT_FORCE ("  Max : %.02f\n", SENSOR_REG_GAIN_DB_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Sensor Gain(倍率)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorGainX (void *str)
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
			cmdSensorGainXHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Sensor Gain取得
		if ((status = sensorGetGainX (&gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.1f ", gain);
	}
	else if (argc == 2)
	{
		// gain
		if (sscanf (gCmdArg[1], "%f", &gain) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Sensor Gain設定
		if ((status = sensorSetGainX (gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Gain(倍率) Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorGainXHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Gain is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensorgainx\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : gain\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Gain is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensorgainx [Param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : gain\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Gain (magnification unit)]\n");
	DEBUG_PRINT_FORCE ("  Min : %.2f\n", SENSOR_REG_GAIN_X_MIN);
	DEBUG_PRINT_FORCE ("  Max : %.2f\n", SENSOR_REG_GAIN_X_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
//**********************************************************************************
//	Sensor Conversion Gain
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorConversionGain (void *str)
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
			cmdSensorConversionGainHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Sensor Conversion Gain取得
		if ((status = sensorGetConversionGain (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// gain
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Sensor Conversion Gain設定
		if ((status = sensorSetConversionGain (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Conversion Gain Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorConversionGainHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The sensor converssion gain is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensorcg\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The sensor converssion gain is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensorcg [Param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  LGC : 0\n");
	DEBUG_PRINT_FORCE ("  HGC : 1\n\n");

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)


#if defined (MODE_SENSOR_GRADATION_COMPRESS)
//**********************************************************************************
//	Gradation Cpmpress Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorGradCompMode (void *str)
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
			cmdSensorGradCompModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Gradation Mode取得
		if ((status = sensorGradationCompGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// Mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Gradation Mode設定
		if ((status = sensorGradationCompSetMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Gradation Compress Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorGradCompModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor gradation compress mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gcmode [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Gradation Compress Mode\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor gradation compress mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : gcmode [param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Gradation Compress Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  0 : Disable\n");
	DEBUG_PRINT_FORCE ("  1 : Enable\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Gradation Cpmpress Position
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorGradCompPosition (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int first, second;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensorGradCompPositionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Gradation Position取得
		if ((status = sensorGradationCompGetPosition (&first, &second)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d %d ", first, second);
	}
	else if (argc == 3)
	{
		// First取得
		if (sscanf (gCmdArg[1], "%d", &first) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Second取得
		if (sscanf (gCmdArg[2], "%d", &second) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Gradation Position設定
		if ((status = sensorGradationCompSetPosition (first, second)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Gradation Cpmpress Position Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorGradCompPositionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor gradation compress position is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gcpos\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output0 Param     : Gradation Compress First Position\n");
	DEBUG_PRINT_FORCE ("  Output1 Param     : Gradation Compress Second Position\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor gradation compress position is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : gcpos [param0]  [param1]\n");
	DEBUG_PRINT_FORCE ("  Input0  Param     : Gradation Compress First Position\n");
	DEBUG_PRINT_FORCE ("  Input1  Param     : Gradation Compress Second Position\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[First / Second Position]\n");
	DEBUG_PRINT_FORCE ("  0 : OFF\n");
	DEBUG_PRINT_FORCE ("  1 : 0\n");
	DEBUG_PRINT_FORCE ("  2 : 16\n");
	DEBUG_PRINT_FORCE ("  3 : 32\n");
	DEBUG_PRINT_FORCE ("  4 : 64\n");
	DEBUG_PRINT_FORCE ("  5 : 128\n");
	DEBUG_PRINT_FORCE ("  6 : 256\n");
	DEBUG_PRINT_FORCE ("  7 : 512\n");
	DEBUG_PRINT_FORCE ("  8 : 1024\n");
	DEBUG_PRINT_FORCE ("  9 : 2048\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Gradation Cpmpress Gain
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorGradCompGain (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int first, second;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensorGradCompGainHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Gradation Gain取得
		if ((status = sensorGradationCompGetGain (&first, &second)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d %d ", first, second);
	}
	else if (argc == 3)
	{
		// First取得
		if (sscanf (gCmdArg[1], "%d", &first) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Second取得
		if (sscanf (gCmdArg[2], "%d", &second) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Gradation Gain設定
		if ((status = sensorGradationCompSetGain (first, second)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Gradation Cpmpress Gain Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorGradCompGainHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor gradation compress gain is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gcpos\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output0 Param     : Gradation Compress First Gain\n");
	DEBUG_PRINT_FORCE ("  Output1 Param     : Gradation Compress Second Gain\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor gradation compress gain is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : gcpos [param0]  [param1]\n");
	DEBUG_PRINT_FORCE ("  Input0  Param     : Gradation Compress First Gain\n");
	DEBUG_PRINT_FORCE ("  Input1  Param     : Gradation Compress Second Gain\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[First / Second Position]\n");
	DEBUG_PRINT_FORCE ("  0 : 1\n");
	DEBUG_PRINT_FORCE ("  1 : 1/2\n");
	DEBUG_PRINT_FORCE ("  2 : 1/4\n");
	DEBUG_PRINT_FORCE ("  3 : 1/8\n");
	DEBUG_PRINT_FORCE ("  4 : 1/16\n");
	DEBUG_PRINT_FORCE ("  5 : 1/32\n");
	DEBUG_PRINT_FORCE ("  6 : 1/64\n");
	DEBUG_PRINT_FORCE ("  7 : 1/128\n");
	DEBUG_PRINT_FORCE ("  8 : 1/256\n");
	DEBUG_PRINT_FORCE ("  9 : 1/512\n");
	DEBUG_PRINT_FORCE (" 10 : 1/1024\n");
	DEBUG_PRINT_FORCE (" 11 : 1/2048\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
// 10Bitから8Bitに変換するモード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensor8BitConvert (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	int bit;
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;
#endif

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensor8BitConvertHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// mode取得
		if ((status = sensorGet8BitConvert (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

#if defined (MODE_FRAMERATE_HIGH_SPEED)
		// Frame Rate High Speed Mode取得
		if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (hsMode == MODE_ENABLE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "ssm cannot be changed in High Speed mode.\n");
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
#endif

		// bit数取得
		if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Check bit
		if (bit != 8)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Can be used only at 8bit\n");
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// mode設定
		if ((status = sensorSet8BitConvert (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// 10Bitから8Bitに変換するモードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensor8BitConvertHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The pixel convert mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensor-sensitive-mode/ssm\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The pixel convert mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensor-sensitive-mode/ssm [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[mode]\n");
	DEBUG_PRINT_FORCE ("   0 : High Sensitivity\n");
	DEBUG_PRINT_FORCE ("   1 : Standard Mode\n");

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if defined (MODE_SENSOR_GRADATION_COMPRESS)


#if defined (MODE_FRAMERATE_HIGH_SPEED)
//**********************************************************************************
//	Frame Rate High Speed Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorFrameRateHighSpeedMode (void *str)
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
			cmdSensorFrameRateHighSpeedModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Frame Rate High Speed Mode取得
		if ((status = sensorGetFrameRateHighSpeedMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// Frame Rate High Speed Mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Frame Rate High Speed Mode設定
		if ((status = sensorSetFrameRateHighSpeedMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Frame Rate High Speed Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorFrameRateHighSpeedModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The sensor frame rate high speed mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : hsm [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Mode\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The sensor frame rate high speed mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : hsm [param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Nomal Speed Mode\n", MODE_DISABLE);
	DEBUG_PRINT_FORCE ("  %d : High Speed Mode\n", MODE_ENABLE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Virtual Height Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorVirtualHeightMode (void *str)
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
			cmdSensorVirtualHeightModeHelp (NULL);
			goto _DONE;
		}
	}

	if ((gInterFaceID != INTERFACE_GIGE) && (gInterFaceID != INTERFACE_GIGE20))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	if (argc == 1)
	{
		// Virtual Height Mode取得
		if ((status = sensorGetVirtualHeightMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// Virtual Hight Mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Virtual Height Mode設定
		if ((status = sensorSetVirtualHeightMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Virtual Height Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorVirtualHeightModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The virtual height mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : vhmode [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Mode\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The virtual height mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : vhmode [param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : virtual height Mode disable\n", MODE_DISABLE);
	DEBUG_PRINT_FORCE ("  %d : virtual height Mode enable\n", MODE_ENABLE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Virtual Height LineScan Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorVirtualHeightLineScanMode (void *str)
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
			cmdSensorVirtualHeightLineScanModeHelp (NULL);
			goto _DONE;
		}
	}

	if ((gInterFaceID != INTERFACE_GIGE) && (gInterFaceID != INTERFACE_GIGE20))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	if (argc == 1)
	{
		// Virtual Height LineScan Mode取得
		if ((status = sensorGetVirtualHeightLineScanMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// Virtual Hight LineScan Mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Virtual Height LineScan Mode設定
		if ((status = sensorSetVirtualHeightLineScanMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Virtual Height LineScan Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorVirtualHeightLineScanModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The virtual height linescan mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : vhlinescan [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Mode\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The virtual height linescan mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : vhlinescan [param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : virtual height linescan Mode disable\n", MODE_DISABLE);
	DEBUG_PRINT_FORCE ("  %d : virtual height linescan Mode enable\n", MODE_ENABLE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Virtual Height
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorVirtualHeight (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int height;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensorVirtualHeightHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Virtual Height取得
		if ((status = sensorGetVirtualHeight (&height)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", height);
	}
	else if (argc == 2)
	{
		// Virtual Hight取得
		if (sscanf (gCmdArg[1], "%d", &height) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Virtual Height設定
		if ((status = sensorSetVirtualHeight (height)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Virtual Height Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorVirtualHeightHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The virtual height is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : virtualheight [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : height\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The virtual height is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : virtualheight [param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : height\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[height]\n");
	DEBUG_PRINT_FORCE ("  Min : 1\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_OTHER);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)

//@@@1
extern int gDebugAAA;
//@@@1

//**********************************************************************************
//	Sensor Standby
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorStandby (void *str)
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
			cmdSensorStandbyHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// モード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		gDebugAAA = mode;	//@@@1
		
		#if 0	//@@@1
		if (mode == 0)
		{
			// Standby Cancel
			if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		else
		{
			// Standby
			if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		#endif //@@@1
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Standby Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorStandbyHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : sensor standby mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensorstandby [param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : standby mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n[Standby Mode]\n");
	DEBUG_PRINT_FORCE ("  0 : Standby Cancel\n");
	DEBUG_PRINT_FORCE ("  1 : Standby\n");

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


#if defined (MODE_SENSOR_DRRS)
//**********************************************************************************
//	Sensor DRRS Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorDrrsMode (void *str)
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
			cmdSensorDrrsModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get DRRS Mode
		if ((status = sensorGetDrrs (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// モード取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Set DRRS Mode
		if ((status = sensorSetDrrsMain2 (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor DRRS Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorDrrsModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DRRS mode is acquired\n");
	DEBUG_PRINT_FORCE ("  Command           : drrs [param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DRRS mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DRRS mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : drrs [param]\n");
	DEBUG_PRINT_FORCE ("  Input Param       : DRRS mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n[DRRS Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Disable\n", MODE_DISABLE);
	DEBUG_PRINT_FORCE ("  %d : Enable\n\n", MODE_ENABLE);

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if defined (MODE_SENSOR_DRRS)

// eof
