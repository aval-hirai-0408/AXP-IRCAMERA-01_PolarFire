//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdTemp.c - Sensor Temp Control Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_TEMP)
//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int gPeltierSensorUpperTempCount;	// センサ温度上限カウント
extern int gPeltierSensorLowerTempCount;	// センサ温度下限カウント
extern int gPeltierCaseTempCount;			// ケース温度カウント


//**********************************************************************************
//	温度情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempStatus (void *str)
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
			cmdTempStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		tempStatus ();
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	温度情報表示 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempStatusHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Temperature Status Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : tempstatus\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Temperature Information\n");
	DEBUG_PRINT_FORCE ("\n");
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	温度情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int tempStatus (void)
{
	//--------------------------------------------------
	// 温度アラーム設定値
	//--------------------------------------------------
	tempAlarm ();

	//--------------------------------------------------
	// 温度アラームステータス
	//--------------------------------------------------
	tempAlarmStatus ();

	//--------------------------------------------------
	// 温度アラームカウント
	//--------------------------------------------------
	tempAlarmCount ();

	//--------------------------------------------------
	// 温度(測定パラメータ)
	//--------------------------------------------------
	tempShow ();

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	温度表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int tempShow (void)
{
	int status;
	
	status = cmdTempShow (NULL);
	return (status);
}


//**********************************************************************************
//	温度表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempShow (void *str)
{
	double temp;

	DEBUG_PRINT_FORCE ("[Temperature values]\n");

	// センサ温度取得
	if (peltierGetSensorTemp (&temp) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("   Sensor Temp               ");
		DEBUG_PRINT_FORCE ("%.2f\n", temp);
	}
	else
	{
		DEBUG_PRINT_FORCE ("\n");
	}

	// Case温度取得
	if (peltierGetCaseTemp (&temp) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("   Case Temp                 ");
		DEBUG_PRINT_FORCE ("%.2f\n", temp);
	}
	else
	{
		DEBUG_PRINT_FORCE ("\n");
	}

	// FPGA温度
	if (xadcGetFpgaTemp (&temp) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("   FPGA Temp                 ");
		DEBUG_PRINT_FORCE ("%.2f\n", temp);
	}
	else
	{
		DEBUG_PRINT_FORCE ("\n");
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Sensor/Case Temp Limit取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int tempAlarm (void)
{
	int status;

	status = cmdTempAlarm (NULL);
	return (status);
}


//**********************************************************************************
//	Sensor/Case Temp Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int tempAlarmStatus (void)
{
	int status;

	status = cmdTempAlarmStatusShow (NULL);
	return (status);
}


//**********************************************************************************
//	Sensor/Case Temp Count取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int tempAlarmCount (void)
{
	int status;

	status = cmdTempAlarmCount (NULL);
	return (status);
}


//**********************************************************************************
//	Sensor/Case Temp Limit取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempAlarm (void *str)
{
	int status;
	double tempOver, tempUnder;
#ifdef MODE_PELTIER
	double target;
#endif
	//----------------------------------------------------------------------
	// 温度上限／下限値取得
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("\n[Temperature Alarm information]\n");

	// センサ温度上限／下限値取得
	if ((status = peltierGetSensorTempAlarm (&tempOver, &tempUnder)) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("   Sensor                    ");
		DEBUG_PRINT_FORCE ("Over = %.2f / Under = %.2f\n", tempOver, tempUnder);
	}

	// Case温度上限／下限値取得
	if ((status = peltierGetCaseTempAlarm (&tempOver, &tempUnder)) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("   Case                      ");
		DEBUG_PRINT_FORCE ("Over = %.2f / Under = %.2f\n", tempOver, tempUnder);
	}

#ifdef MODE_PELTIER
	// Target温度取得
	if ((status = peltierGetTarget (&target)) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("   Target                    ");
		DEBUG_PRINT_FORCE ("%.2f\n", target);
	}
#endif

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Sensor/Case Temp Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempAlarmStatusShow (void *str)
{
	unsigned int tempStatus;

	//----------------------------------------------------------------------
	// 温度異常ステータス
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("[Temperature Alarm Status information]\n");

	tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS);

	// センサ上限温度ステータス
	DEBUG_PRINT_FORCE ("   Sensor Upper Temp         ");
	if (tempStatus & SENSOR_TEMP_OVER_STATUS)
		DEBUG_PRINT_FORCE ("NG\n");
	else
		DEBUG_PRINT_FORCE ("OK\n");

	// センサ下限温度ステータス
	DEBUG_PRINT_FORCE ("   Sensor Lower Temp         ");
	if (tempStatus & SENSOR_TEMP_UNDER_STATUS)
		DEBUG_PRINT_FORCE ("NG\n");
	else
		DEBUG_PRINT_FORCE ("OK\n");

	// ケース温度ステータス
	DEBUG_PRINT_FORCE ("   Case Temp                 ");
	if (tempStatus & CASE_TEMP_STATUS)
		DEBUG_PRINT_FORCE ("NG\n");
	else
		DEBUG_PRINT_FORCE ("OK\n");

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Sensor/Case Temp Count取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempAlarmCount (void *str)
{
	//----------------------------------------------------------------------
	// 温度異常ステータス
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("[Temperature Alarm Count information]\n");

	// センサ上限温度カウント
	DEBUG_PRINT_FORCE ("   Sensor Upper Temp         ");
	DEBUG_PRINT_FORCE ("%d\n", gPeltierSensorUpperTempCount);

	// センサ下限温度カウント
	DEBUG_PRINT_FORCE ("   Sensor Lower Temp         ");
	DEBUG_PRINT_FORCE ("%d\n", gPeltierSensorLowerTempCount);

	//  ケース温度カウント
	DEBUG_PRINT_FORCE ("   Case Temp                 ");
	DEBUG_PRINT_FORCE ("%d\n", gPeltierCaseTempCount);

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	温度表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTemp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int statusLocal;
	int argc;
	double tempSensor, tempCase, fpgaTemp;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdTempHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// センサ温度取得
		if ((statusLocal = peltierGetSensorTemp (&tempSensor)) != AVAL_STATUS_SUCCESS)
		{
			status |= statusLocal;
			tempSensor = -1;
		}

		// Case温度取得
		if ((statusLocal = peltierGetCaseTemp (&tempCase)) != AVAL_STATUS_SUCCESS)
		{
			status |= statusLocal;
			tempCase = -1;
		}

		// FPGA温度
		if ((statusLocal = xadcGetFpgaTemp (&fpgaTemp)) != AVAL_STATUS_SUCCESS)
		{
			status |= statusLocal;
			fpgaTemp = -1;
		}

		DEBUG_PRINT_FORCE ("%.2f %.2f %.2f ", tempSensor, tempCase, fpgaTemp);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	温度表示 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor and Case and FPGA Temperature are acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : temperature/temp\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param0     : Sensor Temperature\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : Case Temperature\n");
	DEBUG_PRINT_FORCE ("  Output Param2     : FPGA Temperature\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Sensor Temp Limit設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorTempAlrm (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double tempOver, tempUnder;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensorTempAlrmHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// センサ温度リミット取得
		if ((status = peltierGetSensorTempAlarm (&tempOver, &tempUnder)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%.2f %.2f ", tempOver, tempUnder);
	}
	else if (argc == 3)
	{
		// 上限値
		if (sscanf (gCmdArg[1], "%lf", &tempOver) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 下限値
		if (sscanf (gCmdArg[2], "%lf", &tempUnder) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// センサ温度リミット設定
		if ((status = peltierSetSensorTempAlarm (tempOver, tempUnder)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Temp Limit設定Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorTempAlrmHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	double tempMin, tempMax;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Temperature Alarm is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensor\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param0     : Sensor Temperature High Limit\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : Sensor Temperature Low  Limit\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Temperature Alarm is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensor [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Sensor Temperature High Limit\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Sensor Temperature Low  Limit\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	// Peltier Temp Min/Max取得
	if ((status = peltierGetSensorTempAlarmMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("[Sensor Temperature High/Low Limit]\n");
	DEBUG_PRINT_FORCE ("  Low  : %.2f\n", tempMin);
	DEBUG_PRINT_FORCE ("  High : %.2f\n", tempMax);
	DEBUG_PRINT_FORCE ("\n");
	
_DONE:
	return (status);
}


//**********************************************************************************
//	Case Temp Limit設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCaseTempAlrm (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double tempOver, tempUnder;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCaseTempAlrmHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Case温度リミット取得
		if ((status = peltierGetCaseTempAlarm (&tempOver, &tempUnder)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%.2f %.2f ", tempOver, tempUnder);
	}
	else if (argc == 3)
	{
		// 上限値
		if (sscanf (gCmdArg[1], "%lf", &tempOver) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 下限値
		if (sscanf (gCmdArg[2], "%lf", &tempUnder) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Case温度リミット設定
		if ((status = peltierSetCaseTempAlarm (tempOver, tempUnder)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Case Temp Limit設定Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCaseTempAlrmHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Case Temperature Alarm is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : case\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param0     : case Temperature High Limit\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : case Temperature Low  Limit\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Case Temperature Alarm is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : case [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : case Temperature High Limit\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : case Temperature Low  Limit\n");
	DEBUG_PRINT_FORCE ("  Output  Param     : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Case Temperature High/Low Limit]\n");
	DEBUG_PRINT_FORCE ("  Low  : %.2f\n", PELTIER_CASE_ALM_MIN_TEMP);
	DEBUG_PRINT_FORCE ("  High : %.2f\n", PELTIER_CASE_ALM_MAX_TEMP);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	温度アラームステータス取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempAlarmStatus (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int tempStatus;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdTempAlarmStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 温度アラームステータス取得
		if ((status = peltierGetTempAlarmStatus (&tempStatus)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", tempStatus);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	温度アラームステータス取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempAlarmStatusHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor and Case Temperature alarm status are acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : tempalarmstatus\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Temperature Alarm Status\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Temperature Alarm Status]\n");
	DEBUG_PRINT_FORCE ("  Bit0 : Sensor Over Temperature\n");
	DEBUG_PRINT_FORCE ("  Bit1 : Sensor Under Temperature\n");
	DEBUG_PRINT_FORCE ("  Bit2 : Case Temperature\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	温度アラームステータスクリア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempAlarmClear (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int clear;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdTempAlarmClearHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &clear) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 温度アラームステータスクリア
		if ((status = peltierClearTempAlarm (clear)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	温度アラームステータスクリアHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempAlarmClearHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor and Case Temperature alarm status are clear.\n");
	DEBUG_PRINT_FORCE ("  Command           : tempalarmclear\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Temperature Alarm Clear\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Temperature Alarm Clear]\n");
	DEBUG_PRINT_FORCE ("  Bit0 : Sensor Over Temperature\n");
	DEBUG_PRINT_FORCE ("  Bit1 : Sensor Under Temperature\n");
	DEBUG_PRINT_FORCE ("  Bit2 : Case Temperature\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


#if defined (MODE_PELTIER)
//**********************************************************************************
//	Peltier Target温度設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdPeltierTarget (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double temp;
#if defined (MODE_PELTIER_CTRL)
	int mode;
#endif

	// Get Argument
	argc = cmdCheckArg (str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPeltierTargetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Target温度取得
		if ((status = peltierGetTarget (&temp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%.2f ", temp);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%lf", &temp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}
		
		#if defined (MODE_PELTIER_CTRL)
		if ((status = peltierGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		if (mode == 1)
		{
			// Peltier Comntrol Init
			if ((status = peltierControlProcsInit ()) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		#endif

		// Target温度設定
		if ((status = peltierSetTarget (temp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		#if defined (MODE_PELTIER_CTRL)
		if (mode == 1)
		{
			// Peltier Enable Delay
			if ((peltierControlProcs ()) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Target温度設定Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPeltierTargetHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	double tempMin, tempMax;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Target Temperature is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : target\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Target Temperature Limit\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Target Temperature is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : target [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Target Temperature Limit\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	// Peltier Temp Min/Max取得
	if ((status = peltierGetTargetMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("[Target Temperature Limit]\n");
	DEBUG_PRINT_FORCE ("  Min : %.2f\n", tempMin);
	DEBUG_PRINT_FORCE ("  Max : %.2f\n", tempMax);
	DEBUG_PRINT_FORCE ("\n");

_DONE:
	return (status);
}
#endif // #if defined (MODE_PELTIER)


#if defined (MODE_PELTIER)
//**********************************************************************************
//	Peltier温度制御最大値
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPeltierTempMax (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double tempMax, tempMin;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPeltierTempMaxHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Peltier Temp Min/Max取得
		if ((status = peltierGetTargetMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", tempMax);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier温度制御最大値Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPeltierTempMaxHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Target Maximum Temperature is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : taregetmax\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Sensor Target Maximum Temperature\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Peltier温度制御最小値
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPeltierTempMin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double tempMax, tempMin;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPeltierTempMinHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Peltier Temp Min/Max取得
		if ((status = peltierGetTargetMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", tempMin);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier温度制御最小値Help(Ver.1.4)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPeltierTempMinHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Target Minimum Temperature is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : taregetmin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Sensor Target Minimum Temperature\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Peltier Enable/Disable Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdPeltierEnable (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	#if defined (MODE_TEMP_ABNORMAL_CHECK)
	unsigned int tempStatus;
	#endif

	// Get Argument
	argc = cmdCheckArg (str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPeltierEnableHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Peltier Mode取得
		if ((status = peltierGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		#if defined (MODE_TEMP_ABNORMAL_CHECK)
		if (mode == 1)
		{
			// Temp Abnormal Status取得
			if ((status = tempGetAbnormalStatus (&tempStatus)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// 温度以上の為、何もしない
			if (tempStatus != 0)
				goto _DONE;
		}
		#endif

		#if defined (MODE_PELTIER_CTRL)
		if (mode == 1)
		{
			// Peltier Comntrol Init
			if ((status = peltierControlProcsInit ()) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		#endif

		// Peltier Mode設定
		if ((status = peltierSetMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		#if defined (MODE_PELTIER_CTRL)
		if (mode == 1)
		{
			// Peltier Enable Delay
			if ((peltierControlProcs ()) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Enable Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPeltierEnableHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The peltier mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : peltiermode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Peltier Mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The peltier mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : peltiermode [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Peltier Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Peltier Mode]\n");
	DEBUG_PRINT_FORCE ("  Disable : %d\n", MODE_ENABLE);
	DEBUG_PRINT_FORCE ("  Enable  : %d\n", MODE_DISABLE);
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}
#endif // #if defined (MODE_PELTIER)


#if defined (MODE_PELTIER_VOLT_CURRENT)
//**********************************************************************************
//	Peltier電圧取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdPeltierVolt (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double volt;

	// Get Argument
	argc = cmdCheckArg (str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPeltierVoltHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 電圧取得
		if ((status = peltierGetVolt (&volt)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", volt);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier電圧取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPeltierVoltHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The peltier voltage is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : pertiervolt\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}


//**********************************************************************************
//	Peltier電流取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdPeltierCurrent (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double current;

	// Get Argument
	argc = cmdCheckArg (str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPeltierCurrentHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 電流取得
		if ((status = peltierGetCurrent (&current)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", current);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier電流取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPeltierCurrentHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The peltier currrent is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : pertiercurrent\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}

#endif	// #if defined (MODE_PELTIER_VOLT_CURRENT)


#if defined (MODE_TEMP_ABNORMAL_CHECK)
//**********************************************************************************
//	Temp Abnormal Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdTempAbnormalStatus (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int data32;

	// Get Argument
	argc = cmdCheckArg (str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdTempAbnormalStatusHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Temp Abnormal Status取得
		if ((status = tempGetAbnormalStatus (&data32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", data32);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &data32) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Temp Abnormal Status設定
		if ((status = tempSetAbnormalStatus (data32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// LED Clear
		if (data32 == 0)
			setLed (LED_NUM1, LED_PATTERN0);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Temp Abnormal Status Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempAbnormalStatusHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The temperature abnormal status is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : tempabnormalstatus\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : mode\n\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The temperature abnormal status is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : tempabnormalstatus [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : status\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n\n");

	DEBUG_PRINT_FORCE ("[status]\n");
	DEBUG_PRINT_FORCE ("  0 : Normal\n");
	DEBUG_PRINT_FORCE ("  1 : Abnormal\n\n");

	return (status);
}


//**********************************************************************************
//	Temp Abnormal Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdTempAbnormalCount (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int data32;

	// Get Argument
	argc = cmdCheckArg (str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdTempAbnormalCountHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Temp Abnormal Count取得
		if ((status = tempGetAbnormalCount (&data32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", data32);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &data32) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Temp Abnormal Count設定
		if ((status = tempSetAbnormalCount (data32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Temp Abnormal Status Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdTempAbnormalCountHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The temperature abnormal count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : tempabnormalcount\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : mode\n\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The temperature abnormal count is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : tempabnormalcount[param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : count\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n\n");

	return (status);
}
#endif // #if defined (MODE_TEMP_ABNORMAL_CHECK)


#if defined (MODE_PELTIER_CTRL)
//**********************************************************************************
//	Peltier Power Level
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdPeltierPowerLevel (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int level, mode;

	// Get Argument
	argc = cmdCheckArg (str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPeltierPowerLevelHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Peltier Power Level取得
		if ((status = peltierGetPowerLevel (&level)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", level);
	}
	else if (argc == 2)
	{
		// Peltier Power Level取得
		if (sscanf (gCmdArg[1], "%d", &level) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Peltier Power Level設定
		if ((status = peltierSetPowerLevel (level)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// Peltier Mode取得
		if ((status = peltierGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (mode == 1)
		{
			// Peltier Control
			if ((peltierControlProcs ()) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Power Level Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPeltierPowerLevelHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The peltier power level is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : peltierpowerlevel\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : level\n\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The peltier power level is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : peltierpowerlevel[param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : level\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n\n");
	
	DEBUG_PRINT_FORCE ("[level]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", PELTIER_POWER_LEVEL_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n\n", PELTIER_POWER_LEVEL_MAX);

	return (status);
}
#endif // #if defined (MODE_PELTIER_CTRL)

#endif // #if defined (MODE_TEMP)

// eof
