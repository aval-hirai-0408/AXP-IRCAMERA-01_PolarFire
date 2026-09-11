//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// diagTemp.c - Diag Temp Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------

// センサターゲット温度タイムアウト
#define DIAG_SENSOR_TEMP_TIMEOUT					(6000)	// 60sec

// 温度割り込み発生待ち
#define DIAG_TEMP_INT_TIMEOUT						(1000)	// 10ms

//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------

extern int gPeltierSensorUpperTempCount;	// センサ温度上限カウント
extern int gPeltierSensorLowerTempCount;	// センサ温度下限カウント
extern int gPeltierCaseTempCount;			// ケース温度カウント


//**********************************************************************************
//	Sensor Temp評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagSensorTemp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed1 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDiagSensorTempHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Diagステート
		saveLed1 = ledDiagnosticState ();

		// 検査
		status = cmdDiagSensorTempMain (gCmdArg[1]);

		// LEDの設定を元に戻す
		ledReturnState (-1, saveLed1);
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
//	Sensor Temp Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagSensorTempHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A sensor temp check is performed.\n");
	DEBUG_PRINT_FORCE ("  Command           : diagsensortemp [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Check Number of times\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	温度検査(Sensor)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagSensorTempMain (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int loop, tloop;
	double temp;
#ifdef MODE_PELTIER
	double sensorTemp, sensorTempSave, setTempMin, setTempMax;
	double getTemp;
#endif
	unsigned int timeout;
	unsigned int saveIntCount;
	double sensorTempHigh, sensorTempLow;

	// 検査回数取得
	if (sscanf (str, "%d", &loop) != 1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

#if defined (MODE_PELTIER)
	// Get Target Save
	if ((status = peltierGetTarget (&sensorTempSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// センサ温度上限／下限値取得
	peltierGetSensorTempAlarm (&sensorTempHigh, &sensorTempLow);

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("-------------------- loop = %d--------------------\n", tloop);

		//--------------------------------------------------------------------------------
		// センサ温度取得
		//--------------------------------------------------------------------------------
		if ((status = peltierGetSensorTemp (&temp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Check Sensor Temp
		if ((temp < DIAG_TEMP_SENSOR_MIN) || (temp > DIAG_TEMP_SENSOR_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "Sensor Temp Error. ensor Temp = %.2f.(Min:%f - Max:%f)\n", temp, DIAG_TEMP_SENSOR_MIN, DIAG_TEMP_SENSOR_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		DEBUG_PRINT ("Sensor Temp          ");
		DEBUG_PRINT ("%.2f\n", temp);

#if defined (MODE_PELTIER)
		//--------------------------------------------------------------------------------
		// センサ温度、指定温度になるまで待つ
		//--------------------------------------------------------------------------------

		// 温度設定
		sensorTemp = DIAG_TEMP_SENSOR_TARGET;
		DEBUG_PRINT ("Sensor Target Temp Check..Target = %.2f\n", DIAG_TEMP_SENSOR_TARGET);
		if ((status = peltierSetTarget (sensorTemp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 温度Check
		setTempMin = sensorTemp - DIAG_SENSOR_TEMP_RANGE;
		setTempMax = sensorTemp + DIAG_SENSOR_TEMP_RANGE;

		for (timeout=0; timeout<DIAG_SENSOR_TEMP_TIMEOUT; timeout++)
		{
			// 温度取得
			if ((status = peltierGetSensorTemp (&getTemp)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// コマンドラインクリア
			cmdBackspaceLine ();

			DEBUG_PRINT ("Temp = %.2f  (%d sec / %d sec)", getTemp, timeout/100, DIAG_SENSOR_TEMP_TIMEOUT/100);
			fflush (stdout);

			// 温度は範囲内？
			if ((getTemp >= setTempMin) && (getTemp <= setTempMax))
				break;

			msDelay (10);
		}

		// Check Timeout
		if (timeout >= DIAG_SENSOR_TEMP_TIMEOUT)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_TIMEOUT);
			sprintf (gLogMsgBuff, "\nSensor Temp Timeout Error. Target = %.2f / Current = %.2f\n", sensorTemp, getTemp);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
#endif // #if defined (MODE_PELTIER)

		//--------------------------------------------------------------------------------
		// センサ温度割り込み(上限)
		//--------------------------------------------------------------------------------
		DEBUG_PRINT ("\nSensor Temp Alarm Upper Check..\n");

		// 現在の割り込みカウント取得
		saveIntCount = gPeltierSensorUpperTempCount;

		// センサ温度上限／下限値設定
		if ((status = peltierSetSensorTempAlarm (DIAG_TEMP_SENSOR_UPPER_ALARM, DIAG_TEMP_SENSOR_UNDER_ALARM_NOMAL)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 割り込み発生待ち
		for (timeout = 0; timeout<DIAG_TEMP_INT_TIMEOUT; timeout++)
		{
			if (saveIntCount < gPeltierSensorUpperTempCount)
				break;

			msDelay (10);
		}

		// Check Timeout
		if (timeout >= DIAG_TEMP_INT_TIMEOUT)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_TIMEOUT);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "\nSensor Temp Upper Alarm Timeout Error.\n");
			goto _DONE;
		}

		DEBUG_PRINT ("Sensor Temp Alarm Under Check..\n");

		//--------------------------------------------------------------------------------
		// センサ温度割り込み(下限)
		//--------------------------------------------------------------------------------
		DEBUG_PRINT ("Sensor Temp Alarm Under Check..\n");

		// 現在の割り込みカウント取得
		saveIntCount = gPeltierSensorLowerTempCount;

		// センサ温度上限／下限値設定
		if ((status = peltierSetSensorTempAlarm (DIAG_TEMP_SENSOR_UPPER_ALARM_NOMAL, DIAG_TEMP_SENSOR_UNDER_ALARM)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 割り込み発生待ち
		for (timeout = 0; timeout<DIAG_TEMP_INT_TIMEOUT; timeout++)
		{
			if (saveIntCount < gPeltierSensorLowerTempCount)
				break;

			msDelay (10);
		}

		// Check Timeout
		if (timeout >= DIAG_TEMP_INT_TIMEOUT)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_TIMEOUT);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Temp Under Alarm Timeout Error.\n");
			goto _DONE;
		}
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	if ((sensorTempHigh != 0) && (sensorTempLow != 0))
	{
		// センサ温度上限／下限値設定
		peltierSetSensorTempAlarm (sensorTempHigh, sensorTempLow);
	}


#ifdef MODE_PELTIER
	// Targetリストア
	peltierSetTarget (sensorTempSave);
#endif

	// センサ温度上限／下限値設定（通常設定）
	peltierSetSensorTempAlarm (DIAG_TEMP_SENSOR_UPPER_ALARM_NOMAL, DIAG_TEMP_SENSOR_UNDER_ALARM_NOMAL);

	return (status);
}


//**********************************************************************************
//	Case温度評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagCaseTemp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed1 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDiagCaseTempHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Diagステート
		saveLed1 = ledDiagnosticState ();

		// 検査
		status = cmdDiagCaseTempMain (gCmdArg[1]);

		// LEDの設定を元に戻す
		ledReturnState (-1, saveLed1);
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
//	Case温度評価 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagCaseTempHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A Case Temp check is performed.\n");
	DEBUG_PRINT_FORCE ("  Command           : diagcasetemp [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Check Number of times\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	温度検査(Case/FPGA)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagCaseTempMain (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int loop, tloop;
	char *dev = (char *)"Case Temp";
	double temp;
	unsigned int timeout;
	unsigned int saveIntCount;

	// 検査回数取得
	if (sscanf (str, "%d", &loop) != 1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("--------------------[%s] loop = %d--------------------\n", dev, tloop);

		//--------------------------------------------------------------------------------
		// Case温度取得
		//--------------------------------------------------------------------------------
		if ((status = peltierGetCaseTemp (&temp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Check Case Temp
		if ((temp < DIAG_TEMP_CASE_MIN) || (temp > DIAG_TEMP_CASE_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "Case Temp Error. Case Temp = %.2f.(Min:%f - Max:%f)\n", temp, DIAG_TEMP_CASE_MIN, DIAG_TEMP_CASE_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		DEBUG_PRINT ("Case Temp            ");
		DEBUG_PRINT ("%.2f\n", temp);

		//--------------------------------------------------------------------------------
		// ケース温度割り込み
		//--------------------------------------------------------------------------------
		DEBUG_PRINT ("Case Temp Alarm Check..\n");

		// 現在の割り込みカウント取得
		saveIntCount = gPeltierCaseTempCount;

		// ケース温度上限／下限値設定
		if ((status = peltierSetCaseTempAlarm (DIAG_TEMP_CASE_UPPER_ALARM, DIAG_TEMP_CASE_UNDER_ALARM_NORMAL)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 割り込み発生待ち
		for (timeout = 0; timeout<DIAG_TEMP_INT_TIMEOUT; timeout++)
		{
			if (saveIntCount < gPeltierCaseTempCount)
				break;

			msDelay (10);
		}

		// Check Timeout
		if (timeout >= DIAG_TEMP_INT_TIMEOUT)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_TIMEOUT);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Case Temp Alarm Timeout Error.\n");
			goto _DONE;
		}

		// ケース温度上限／下限値設定(下限値を下回るようにする)
		if ((status = peltierSetCaseTempAlarm (DIAG_TEMP_CASE_MAX, DIAG_TEMP_CASE_MAX-0.1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		msDelay (1000);
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	// センサ温度上限／下限値設定（通常設定）
	peltierSetCaseTempAlarm (DIAG_TEMP_CASE_UPPER_ALARM_NORMAL, DIAG_TEMP_CASE_UNDER_ALARM_NORMAL);

	return (status);
}


//**********************************************************************************
//	FPGA温度評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagFpgaTemp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed1 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDiagFpgaTempHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Diagステート
		saveLed1 = ledDiagnosticState ();

		// 検査
		status = cmdDiagFpgaTempMain (gCmdArg[1]);

		// LEDの設定を元に戻す
		ledReturnState (-1, saveLed1);
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
//	FPGA温度評価 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagFpgaTempHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A Fpga Temp check is performed.\n");
	DEBUG_PRINT_FORCE ("  Command           : diagfpgatemp [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Check Number of times\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	温度検査(FPGA)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagFpgaTempMain (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int loop, tloop;
	char *dev = (char *)"FPGA Temp";
	double temp;

	// 検査回数取得
	if (sscanf (str, "%d", &loop) != 1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("--------------------[%s] loop = %d--------------------\n", dev, tloop);

		//--------------------------------------------------------------------------------
		// FPGA温度
		//--------------------------------------------------------------------------------
		if ((status = xadcGetFpgaTemp (&temp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Check FPGA Temp
		if ((temp < DIAG_TEMP_FPGA_MIN) || (temp > DIAG_TEMP_FPGA_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "FPGA Temp Error. FPGA Temp = %.2f.(Min:%f - Max:%f)\n", temp, DIAG_TEMP_FPGA_MIN, DIAG_TEMP_FPGA_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		DEBUG_PRINT ("FPGA Temp            ");
		DEBUG_PRINT ("%.2f\n", temp);
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	return (status);
}

// eof
