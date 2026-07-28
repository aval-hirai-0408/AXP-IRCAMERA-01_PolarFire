//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// led.c - LED Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gLed0Save = -1;
int gLed1Save = -1;
int gLedDebugSave = -1;


//**********************************************************************************
//	LED設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number              ：0=LED0
//							：1=LED1
//							：2=Debug
//		mode				：LED点灯モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setLed (int number, int mode)
{
	int status;
	int debugMode;

	// デバックモードの場合、LEDは更新しない
	if (ledGetDebugMode(&debugMode) == AVAL_STATUS_SUCCESS)
	{
		if (debugMode == LED_DEBUG_MODE_ENABLE)
		{
			return (AVAL_STATUS_SUCCESS);
		}
		else if (debugMode == LED_DEBUG_MODE_ERROR)
		{
			switch (mode)
			{
				case LED_OFF:
				case LED_HW_SYSTEM_ERROR:
				case LED_STREAMING_ERROR:
				case LED_SYSTEM_ERROR:
					break;
				default:
					return (AVAL_STATUS_SUCCESS);
			}
		}
	}

	// LED制御
	status = setLedMain (number, mode);

	return (status);
}


//**********************************************************************************
//	LED設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number              ：0=LED0
//							：1=LED1
//							：2=Debug
//		mode				：LED点灯モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setLedMain (int number, int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check number Parameter
	if ((number < LED_NUM_MIN) || (number > LED_NUM_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LED Number(%d) Parameter Error. (Min:%d / Max:%d)\n", number, LED_NUM_MIN, LED_NUM_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check mode Parameter
	if ((mode < LED_PATTERN_MIN) || (mode > LED_PATTERN_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LED Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, LED_PATTERN_MIN, LED_PATTERN_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Set LED
	OUT32 ((FPGA_LED_CTRL_LED0_ADRS + number*4), mode);

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
		OUT32 (FIRM_DATA_CXP_LED1_ADRS, mode);
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	LED取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number              ：0=LED0
//							：1=LED1
//							：2=Debug
//		pMode				：
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getLed (int number, int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check number Parameter
	if ((number < LED_NUM_MIN) || (number > LED_NUM_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		DEBUG_PRINT_FORCE ("LED Number(%d) Parameter Error. (Min:%d / Max:%d)\n", number, LED_NUM_MIN, LED_NUM_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LED pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Get LED
	*pMode = IN32 ((FPGA_LED_CTRL_LED0_ADRS + number*4));

_DONE:
	return (status);
}


//**********************************************************************************
//	起動ステートLED
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledBootState (void)
{
	unsigned int dipsw;

	// デバックモード更新
	OUT32 (FIRM_DATA_LED_DEBUG_MODE_ADRS, LED_DEBUG_MODE_DISABLE);

	// キャッシュFlash
	cacheFlushRange (FIRM_DATA_LED_DEBUG_MODE_ADRS, 4);

	// LED設定
	setLed (LED_POWER, LED_BOOTING);
	setLed (LED_STATE, LED_OFF);
	
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	HW System Error
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledHwSystemErrorState (void)
{
	if (gInterFaceID == INTERFACE_CXP)
		setLed (LED_STATE, LED_HW_SYSTEM_ERROR);
	else
		setLed (LED_POWER, LED_HW_SYSTEM_ERROR);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ConnectステートLED
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledConnectState (void)
{
	int led ;

	if ((gInterFaceID == INTERFACE_CAMERALINK) || (gInterFaceID == INTERFACE_CXP))
	{
		getLed (LED_POWER, &led);

		if (led != LED_HW_SYSTEM_ERROR)
			setLed (LED_POWER, LED_CONNECTED);
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ConnectステートLED
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledConnectStateForce (void)
{
	// CameraLinkのみ
	if (gInterFaceID == INTERFACE_CXP)
	{
		setLed (LED_STATE, LED_CONNECTED);
	}
	else
	{
		setLed (LED_POWER, LED_CONNECTED);
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	設定ステートLED
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledSettingState (void)
{
	int saveLedPattern;
	
	// 設定前の値を取得
	getLed (LED_POWER, &saveLedPattern);
	setLed (LED_POWER, LED_CONFIG);

	return (saveLedPattern);
}


//**********************************************************************************
//	コマンド実行ステートLED
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledCmdExeState (void)
{
	// CameraLinkのみ
	if (gInterFaceID == INTERFACE_CAMERALINK)
		setLed (LED_STATE2, LED_COMMAND);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	診断ステートLED
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledDiagnosticState (void)
{
	int saveLedPattern = -1;

	getLed (LED_STATE2, &saveLedPattern);
	setLed (LED_STATE2, LED_DIAG);

	return (saveLedPattern);
}


//**********************************************************************************
//	システムエラーステートLED
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledSystemErrorState (void)
{
	// LED設定
	setLed (LED_STATE, LED_SYSTEM_ERROR);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	LEDの設定を元に戻す
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		led0				：LED0ステータス
//		led1				：LED1ステータス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledReturnState (int led0, int led1)
{
	if (led0 != -1)
		setLed (LED_POWER, led0);

	if (led1 != -1)
		setLed (LED_STATE, led1);
	
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	LEDデバックモード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：デバックモード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledSetDebugMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check mode Parameter
	if ((mode < LED_DEBUG_MODE_MIN) || (mode > LED_DEBUG_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LED Debug Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, LED_DEBUG_MODE_MIN, LED_DEBUG_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// デバックモード更新
	OUT32 (FIRM_DATA_LED_DEBUG_MODE_ADRS, mode);

	// Cache Flush
	cacheFlushRange (FIRM_DATA_LED_DEBUG_MODE_ADRS, 4);

	if ((mode == LED_DEBUG_MODE_ENABLE) || (mode == LED_DEBUG_MODE_ERROR))
	{
		// Enable時は設定値を保存
		gLed0Save = -1;
		gLed1Save = -1;
		gLedDebugSave = -1;
		getLed (LED_NUM0, &gLed0Save);
		getLed (LED_NUM1, &gLed1Save);
	}
	else if (mode == LED_DEBUG_MODE_DISABLE)
	{
		// Enable時は設定値を保存
		if (gLed0Save != -1)
			setLed (LED_NUM0, gLed0Save);

		if (gLed1Save != -1)
			setLed (LED_NUM1, gLed1Save);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	LEDデバックモード取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：デバックモードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledGetDebugMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LED Debug pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// デバックモード取得
	*pMode = IN32 (FIRM_DATA_LED_DEBUG_MODE_ADRS);

_DONE:
	return (status);
}

// eof
