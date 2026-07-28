//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// counterControl.c - Genicam Counter Control Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gCounterStatus = 0;		// Counter Control Status


//**********************************************************************************
//	Counter Control Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterInitialize (void)
{
	int status;

	DEBUG_PRINT ("Counter Initialize\n");

	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_COUNTER_ADRS, CAMERA_SAVE_COUNTER_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gCounterStatus = status;
		return (status);
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counterセレクト設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：Lineセレクト
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetSelect (int select)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check select Parameter
	if ((select < COUNTER_SELECT_MIN) || (select > COUNTER_SELECT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Counter Select(%d) Parameter Error. (Min:%d / Max:%d)\n", select, COUNTER_SELECT_MIN, COUNTER_SELECT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Counter Selectモード設定
	OUT32 (GENICAM_COUNTER_SELECT_ADRS, (select & GENICAM_COUNTER_SELECT_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Counterセレクト取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSelect				：Counterセレクトを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetSelect (int *pSelect)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pSelect Parameter
	if (pSelect == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Select pSelect Parameter Error\n");
		goto _DONE;
	}

	// Counter Selectモード設定
	*pSelect = IN32 (GENICAM_COUNTER_SELECT_ADRS) & GENICAM_COUNTER_SELECT_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Eventソース設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		source				：カウントを行うトリガ接続元
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetEventSource (int source)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check source Parameter
	if ((status = lfCounterEventSourceCheck (source)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Source設定
	OUT32 (GENICAM_COUNTER_SOURCE_ADRS, (source & GENICAM_COUNTER_SOURCE_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_EVENT_INVALID_SOURCE_ADRS);

	if (data & GENICAM_COUNTER_EVENT_INVALID_SOURCE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Event Invalid Source Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Eventソース取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSource				：カウントを行うトリガ接続元を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetEventSource (int *pSource)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pSource Parameter
	if (pSource == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Event pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_EVENT_INVALID_SOURCE_ADRS);

	if (data & GENICAM_COUNTER_EVENT_INVALID_SOURCE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Event Invalid Source Error.\n");
		goto _DONE;
	}

	// Counter Source取得
	*pSource = IN32 (GENICAM_COUNTER_SOURCE_ADRS) & GENICAM_COUNTER_SOURCE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Event Source Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：カウンタ番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfCounterEventSourceCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_START_TRIGGER:
		case GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_START:
		case GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_END:
		case GENICAM_COUNTER_EVENT_SOURCE_ACQUISITION_INVALID_TRG:
		case GENICAM_COUNTER_EVENT_SOURCE_FRAME_START_TRIGGER:
		case GENICAM_COUNTER_EVENT_SOURCE_FRAME_START:
		case GENICAM_COUNTER_EVENT_SOURCE_FRAME_ACTIVE:
		case GENICAM_COUNTER_EVENT_SOURCE_FRAME_END:
		case GENICAM_COUNTER_EVENT_SOURCE_FRAME_INVALID_TRG:
		case GENICAM_COUNTER_EVENT_SOURCE_EXPOSURE_START:
		case GENICAM_COUNTER_EVENT_SOURCE_EXPOSURE_END:
		case GENICAM_COUNTER_EVENT_SOURCE_EXPOSURE_INVALID_TRG:
		case GENICAM_COUNTER_EVENT_SOURCE_LINE0:
		case GENICAM_COUNTER_EVENT_SOURCE_LINE1:
		case GENICAM_COUNTER_EVENT_SOURCE_LINE2:
		case GENICAM_COUNTER_EVENT_SOURCE_LINE3:
		case GENICAM_COUNTER_EVENT_SOURCE_LINE4:
		case GENICAM_COUNTER_EVENT_SOURCE_LINE5:
		case GENICAM_COUNTER_EVENT_SOURCE_LINE6:

		case GENICAM_COUNTER_EVENT_SOURCE_COUNTER0_START:
		case GENICAM_COUNTER_EVENT_SOURCE_COUNTER0_END:
		case GENICAM_COUNTER_EVENT_SOURCE_COUNTER1_START:
		case GENICAM_COUNTER_EVENT_SOURCE_COUNTER1_END:
		case GENICAM_COUNTER_EVENT_SOURCE_COUNTER2_START:
		case GENICAM_COUNTER_EVENT_SOURCE_COUNTER2_END:
		case GENICAM_COUNTER_EVENT_SOURCE_COUNTER3_START:
		case GENICAM_COUNTER_EVENT_SOURCE_COUNTER3_END:
		case GENICAM_COUNTER_EVENT_SOURCE_TIMER0_START:
		case GENICAM_COUNTER_EVENT_SOURCE_TIMER0_END:
		case GENICAM_COUNTER_EVENT_SOURCE_TIMER1_START:
		case GENICAM_COUNTER_EVENT_SOURCE_TIMER1_END:
		case GENICAM_COUNTER_EVENT_SOURCE_TIMER2_START:
		case GENICAM_COUNTER_EVENT_SOURCE_TIMER2_END:
		case GENICAM_COUNTER_EVENT_SOURCE_TIMER3_START:
		case GENICAM_COUNTER_EVENT_SOURCE_TIMER3_END:
		case GENICAM_COUNTER_EVENT_SOURCE_ENCODER0:
		case GENICAM_COUNTER_EVENT_SOURCE_TICK:
		case GENICAM_COUNTER_EVENT_SOURCE_OFF:
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Counter Event Source(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;
	}
	
_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Event Active設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Active論理
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetEventActive (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check mode Parameter
	if ((mode < COUNTER_EVENT_ACTIVE_MIN) || (mode > COUNTER_EVENT_ACTIVE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Counter Event Activation Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, COUNTER_EVENT_ACTIVE_MIN, COUNTER_EVENT_ACTIVE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Counter Activation設定
	OUT32 (GENICAM_COUNTER_EVENT_ACTIVE_ADRS, (mode & GENICAM_COUNTER_EVENT_ACTIVE_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_EVENT_INVALID_ACTIVE_ADRS);

	if (data & GENICAM_COUNTER_EVENT_INVALID_ACTIVE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Event Invalid Activation Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Event Active取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Active論理を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetEventActive (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Event Activation pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_EVENT_INVALID_ACTIVE_ADRS);

	if (data & GENICAM_COUNTER_EVENT_INVALID_ACTIVE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Event Invalid Activation Error.\n");
		goto _DONE;
	}

	// Counter Event Activation取得
	*pMode = IN32 (GENICAM_COUNTER_EVENT_ACTIVE_ADRS) & GENICAM_COUNTER_EVENT_ACTIVE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Resetソース設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		source				：カウントResetを行うトリガ接続元
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetResetSource (int source)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check Source Number
	if ((status = lfCounterResetSourceCheck (source)) != AVAL_STATUS_SUCCESS)
		return (status);

	// Counter Reset Source設定
	OUT32 (GENICAM_COUNTER_RESET_SOURCE_ADRS, (source & GENICAM_COUNTER_RESET_SOURCE_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_RESET_INVALID_SOURCE_ADRS);

	if (data & GENICAM_COUNTER_RESET_INVALID_SOURCE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Reset Invalid Source Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Resetソース取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSource				：カウントResetを行うトリガ接続元を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetResetSource (int *pSource)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pSource Parameter
	if (pSource == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Reset pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_RESET_INVALID_SOURCE_ADRS);

	if (data & GENICAM_COUNTER_RESET_INVALID_SOURCE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Reset Invalid Source Error.\n");
		goto _DONE;
	}

	// Counter Source取得
	*pSource = IN32 (GENICAM_COUNTER_RESET_SOURCE_ADRS) & GENICAM_COUNTER_RESET_SOURCE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Reset Source Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：カウンタ番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfCounterResetSourceCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_START_TRIGGER:
		case GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_START:
		case GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_END:
		case GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_INVALID_TRG:
		case GENICAM_COUNTER_RESET_SOURCE_FRAME_START_TRIGGER:
		case GENICAM_COUNTER_RESET_SOURCE_FRAME_START:
		case GENICAM_COUNTER_RESET_SOURCE_FRAME_ACTIVE:
		case GENICAM_COUNTER_RESET_SOURCE_FRAME_END:
		case GENICAM_COUNTER_RESET_SOURCE_FRAME_INVALID_TRG:
		case GENICAM_COUNTER_RESET_SOURCE_EXPOSURE_START:
		case GENICAM_COUNTER_RESET_SOURCE_EXPOSURE_END:
		case GENICAM_COUNTER_RESET_SOURCE_EXPOSURE_INVALID_TRG:

		case GENICAM_COUNTER_RESET_SOURCE_LINE0:
		case GENICAM_COUNTER_RESET_SOURCE_USER0_OUTPUT:
		case GENICAM_COUNTER_RESET_SOURCE_LINE1:
		case GENICAM_COUNTER_RESET_SOURCE_USER1_OUTPUT:
		case GENICAM_COUNTER_RESET_SOURCE_LINE2:
		case GENICAM_COUNTER_RESET_SOURCE_USER2_OUTPUT:
		case GENICAM_COUNTER_RESET_SOURCE_LINE3:
		case GENICAM_COUNTER_RESET_SOURCE_USER3_OUTPUT:
		case GENICAM_COUNTER_RESET_SOURCE_LINE4:
		case GENICAM_COUNTER_RESET_SOURCE_USER4_OUTPUT:
		case GENICAM_COUNTER_RESET_SOURCE_LINE5:
		case GENICAM_COUNTER_RESET_SOURCE_USER5_OUTPUT:
		case GENICAM_COUNTER_RESET_SOURCE_LINE6:
		case GENICAM_COUNTER_RESET_SOURCE_USER6_OUTPUT:

		case GENICAM_COUNTER_RESET_SOURCE_COUNTER0_START:
		case GENICAM_COUNTER_RESET_SOURCE_COUNTER0_END:
		case GENICAM_COUNTER_RESET_SOURCE_COUNTER1_START:
		case GENICAM_COUNTER_RESET_SOURCE_COUNTER1_END:
		case GENICAM_COUNTER_RESET_SOURCE_COUNTER2_START:
		case GENICAM_COUNTER_RESET_SOURCE_COUNTER2_END:
		case GENICAM_COUNTER_RESET_SOURCE_COUNTER3_START:
		case GENICAM_COUNTER_RESET_SOURCE_COUNTER3_END:
		case GENICAM_COUNTER_RESET_SOURCE_TIMER0_START:
		case GENICAM_COUNTER_RESET_SOURCE_TIMER0_END:
		case GENICAM_COUNTER_RESET_SOURCE_TIMER1_START:
		case GENICAM_COUNTER_RESET_SOURCE_TIMER1_END:
		case GENICAM_COUNTER_RESET_SOURCE_TIMER2_START:
		case GENICAM_COUNTER_RESET_SOURCE_TIMER2_END:
		case GENICAM_COUNTER_RESET_SOURCE_TIMER3_START:
		case GENICAM_COUNTER_RESET_SOURCE_TIMER3_END:
		case GENICAM_COUNTER_RESET_SOURCE_ENCODER0:
		case GENICAM_COUNTER_RESET_SOURCE_OFF:
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Counter Reset Source(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Reset Active設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Active論理
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetResetActive (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check mode Parameter
	if ((mode < COUNTER_RESET_ACTIVE_MIN) || (mode > COUNTER_RESET_ACTIVE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Counter Reset Activation Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, COUNTER_RESET_ACTIVE_MIN, COUNTER_RESET_ACTIVE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Counter Activation設定
	OUT32 (GENICAM_COUNTER_RESET_ACTIVE_ADRS, (mode & GENICAM_COUNTER_RESET_ACTIVE_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_RESET_INVALID_ACTIVE_ADRS);

	if (data & GENICAM_COUNTER_RESET_INVALID_ACTIVE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Reset Invalid Activation Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Reset Active取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Active論理を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetResetActive (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Reset  Activation pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_RESET_INVALID_ACTIVE_ADRS);

	if (data & GENICAM_COUNTER_RESET_INVALID_ACTIVE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Reset Invalid  Activation Error.\n");
		goto _DONE;
	}

	// Counter Reset Activation取得
	*pMode = IN32 (GENICAM_COUNTER_RESET_ACTIVE_ADRS) & GENICAM_COUNTER_RESET_ACTIVE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Reset設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterReset (void)
{
	// Counter Reset設定
	OUT32 (GENICAM_COUNTER_RESET_ADRS, GENICAM_COUNTER_RESET_RESET_DATA);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Counter Value設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：Counter値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetValue (int count)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check count Parameter
	if ((count < GENICAM_COUNTER_VALUE_MIN) || (count > GENICAM_COUNTER_VALUE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Counter Value(%d) Parameter Error. (Min:%d / Max:%d)\n", count, GENICAM_COUNTER_VALUE_MIN, GENICAM_COUNTER_VALUE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Counter設定
	OUT32 (GENICAM_COUNTER_ADRS, (count & GENICAM_COUNTER_VALUE_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Value取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：Counter値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetValue (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Get Value pCount NULL Parameter Error\n");
		goto _DONE;
	}

	// Counter取得
	*pCount = IN32 (GENICAM_COUNTER_ADRS) & GENICAM_COUNTER_VALUE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Reset時の値を設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：Counter値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetResetValue (int count)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check count Parameter
	if ((count < GENICAM_COUNTER_RESET_VALUE_MIN) || (count > GENICAM_COUNTER_RESET_VALUE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Counter Reset Value(%d) Parameter Error. (Min:%d / Max:%d)\n", count, GENICAM_COUNTER_RESET_VALUE_MIN, GENICAM_COUNTER_RESET_VALUE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Counter設定
	OUT32 (GENICAM_COUNTER_RESET_VALUE_ADRS, (count & GENICAM_COUNTER_RESET_VALUE_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Reset時の値を取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：Counter値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetResetValue (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Reset Value pCount NULL Parameter Error\n");
		goto _DONE;
	}

	// Counter取得
	*pCount = IN32 (GENICAM_COUNTER_RESET_VALUE_ADRS) & GENICAM_COUNTER_RESET_VALUE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Resetのカウント終了比較値を設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：Counter値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetDuration (int count)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check count Parameter
	if ((count < GENICAM_COUNTER_DURATION_MIN) || (count > GENICAM_COUNTER_DURATION_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Counter Duration Value(%d) Parameter Error. (Min:%d / Max:%d)\n", count, GENICAM_COUNTER_DURATION_MIN, GENICAM_COUNTER_DURATION_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Duration設定
	OUT32 (GENICAM_COUNTER_DURATION_ADRS, (count & GENICAM_COUNTER_DURATION_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Resetのカウント終了比較値を取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：Counter値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetDuration (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Reset Duration pCount NULL Parameter Error\n");
		goto _DONE;
	}

	// Duration取得
	*pCount = IN32 (GENICAM_COUNTER_DURATION_ADRS) & GENICAM_COUNTER_DURATION_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Resetステータス取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStatus				：Counterステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetStatus (unsigned int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Reset Status pStatus NULL Parameter Error\n");
		goto _DONE;
	}

	// Status取得
	*pStatus = IN32 (GENICAM_COUNTER_RESET_STATUS_ADRS) & GENICAM_COUNTER_RESET_STATUS_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Trgソース設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		source				：カウントTrgを行うトリガ接続元
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetTrgSource (int source)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check source Parameter
	if ((status = lfCounterTrgSourceCheck (source)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Trg Source設定
	OUT32 (GENICAM_COUNTER_TRG_SOURCE_ADRS, (source & GENICAM_COUNTER_TRG_SOURCE_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_TRG_INVALID_SOURCE_ADRS);

	if (data & GENICAM_COUNTER_TRG_INVALID_SOURCE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Trg Invalid Mode Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Trgソース取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSource				：カウントTrgを行うトリガ接続元を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetTrgSource (int *pSource)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pSource Parameter
	if (pSource == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Trg pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_TRG_INVALID_SOURCE_ADRS);

	if (data & GENICAM_COUNTER_TRG_INVALID_SOURCE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Trg Invalid Mode Error.\n");
		goto _DONE;
	}

	// Counter Source取得
	*pSource = IN32 (GENICAM_COUNTER_TRG_SOURCE_ADRS) & GENICAM_COUNTER_TRG_SOURCE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Trg Source Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：カウンタ番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfCounterTrgSourceCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_START_TRIGGER:
		case GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_START:
		case GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_END:
		case GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_INVALID_TRG:
		case GENICAM_COUNTER_TRG_SOURCE_FRAME_START_TRIGGER:
		case GENICAM_COUNTER_TRG_SOURCE_FRAME_START:
		case GENICAM_COUNTER_TRG_SOURCE_FRAME_ACTIVE:
		case GENICAM_COUNTER_TRG_SOURCE_FRAME_END:
		case GENICAM_COUNTER_TRG_SOURCE_FRAME_INVALID_TRG:
		case GENICAM_COUNTER_TRG_SOURCE_EXPOSURE_START:
		case GENICAM_COUNTER_TRG_SOURCE_EXPOSURE_END:
		case GENICAM_COUNTER_TRG_SOURCE_EXPOSURE_INVALID_TRG:

		case GENICAM_COUNTER_TRG_SOURCE_LINE0:
		case GENICAM_COUNTER_TRG_SOURCE_USER0_OUTPUT:
		case GENICAM_COUNTER_TRG_SOURCE_LINE1:
		case GENICAM_COUNTER_TRG_SOURCE_USER1_OUTPUT:
		case GENICAM_COUNTER_TRG_SOURCE_LINE2:
		case GENICAM_COUNTER_TRG_SOURCE_USER2_OUTPUT:

		case GENICAM_COUNTER_TRG_SOURCE_LINE3:
		case GENICAM_COUNTER_TRG_SOURCE_USER3_OUTPUT:
		case GENICAM_COUNTER_TRG_SOURCE_LINE4:
		case GENICAM_COUNTER_TRG_SOURCE_USER4_OUTPUT:
		case GENICAM_COUNTER_TRG_SOURCE_LINE5:
		case GENICAM_COUNTER_TRG_SOURCE_USER5_OUTPUT:
		case GENICAM_COUNTER_TRG_SOURCE_LINE6:
		case GENICAM_COUNTER_TRG_SOURCE_USER6_OUTPUT:

		case GENICAM_COUNTER_TRG_SOURCE_COUNTER0_START:
		case GENICAM_COUNTER_TRG_SOURCE_COUNTER0_END:
		case GENICAM_COUNTER_TRG_SOURCE_COUNTER1_START:
		case GENICAM_COUNTER_TRG_SOURCE_COUNTER1_END:
		case GENICAM_COUNTER_TRG_SOURCE_COUNTER2_START:
		case GENICAM_COUNTER_TRG_SOURCE_COUNTER2_END:
		case GENICAM_COUNTER_TRG_SOURCE_COUNTER3_START:
		case GENICAM_COUNTER_TRG_SOURCE_COUNTER3_END:
		case GENICAM_COUNTER_TRG_SOURCE_TIMER0_START:
		case GENICAM_COUNTER_TRG_SOURCE_TIMER0_END:
		case GENICAM_COUNTER_TRG_SOURCE_TIMER1_START:
		case GENICAM_COUNTER_TRG_SOURCE_TIMER1_END:
		case GENICAM_COUNTER_TRG_SOURCE_TIMER2_START:
		case GENICAM_COUNTER_TRG_SOURCE_TIMER2_END:
		case GENICAM_COUNTER_TRG_SOURCE_TIMER3_START:
		case GENICAM_COUNTER_TRG_SOURCE_TIMER3_END:
		case GENICAM_COUNTER_TRG_SOURCE_COUNTER_START_TRG:
		case GENICAM_COUNTER_TRG_SOURCE_ENCODER0:
		case GENICAM_COUNTER_TRG_SOURCE_OFF:
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Counter Trg Source(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Trg Active設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Active論理
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetTrgActive (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check mode Parameter
	if ((mode < COUNTER_TRG_ACTIVE_MIN) || (mode > COUNTER_TRG_ACTIVE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Counter Trg Activation Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, COUNTER_TRG_ACTIVE_MIN, COUNTER_TRG_ACTIVE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Counter Activation設定
	OUT32 (GENICAM_COUNTER_TRG_ACTIVE_ADRS, (mode & GENICAM_COUNTER_TRG_ACTIVE_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_TRG_INVALID_ACTIVE_ADRS);

	if (data & GENICAM_COUNTER_TRG_INVALID_ACTIVE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Trg Invalid Activation Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Trg Active取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Active論理を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetTrgActive (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Trg Active pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_COUNTER_TRG_INVALID_ACTIVE_ADRS);

	if (data & GENICAM_COUNTER_TRG_INVALID_ACTIVE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Counter Trg Invalid Activation Error.\n");
		goto _DONE;
	}

	// Counter Trg Activation取得
	*pMode = IN32 (GENICAM_COUNTER_TRG_ACTIVE_ADRS) & GENICAM_COUNTER_TRG_ACTIVE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Trg モード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		lineSrc				：外部信号の番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterSetTrgMode (int lineSrc)
{
	int status = AVAL_STATUS_SUCCESS;
	int imageCountEvent;

	//--------------------------------------------------------------------------------
	// 外部信号のカウント数
	//--------------------------------------------------------------------------------
	switch (lineSrc)
	{
		case GENICAM_COUNTER_EVENT_SOURCE_OFF:
		case GENICAM_COUNTER_EVENT_SOURCE_LINE0:
		case GENICAM_COUNTER_EVENT_SOURCE_LINE1:
		case GENICAM_COUNTER_EVENT_SOURCE_LINE2:
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Counter Trg Mode(%d) Parameter Error.\n", lineSrc);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
	}

	// Counterセレクト設定
	if ((status = counterSetSelect (COUNTER2_SELECT)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Event Source設定
	if ((status = counterSetEventSource (lineSrc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Trg Source設定
	if ((status = counterSetTrgSource (GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_START_TRIGGER)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Reset Source設定
	if ((status = counterSetResetSource (GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_START_TRIGGER)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Duration Value設定
	if ((status = counterSetDuration (GENICAM_COUNTER_DURATION_MAX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Reset設定
	if ((status = counterReset ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//--------------------------------------------------------------------------------
	// Exposure Startのカウント数
	//--------------------------------------------------------------------------------

	// Counterセレクト設定
	if ((status = counterSetSelect (COUNTER3_SELECT)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (lineSrc == GENICAM_COUNTER_EVENT_SOURCE_OFF)
		imageCountEvent = GENICAM_COUNTER_EVENT_SOURCE_OFF;
	else
		imageCountEvent = GENICAM_COUNTER_EVENT_SOURCE_EXPOSURE_START;

	// Counter Event Source設定
	if ((status = counterSetEventSource (imageCountEvent)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Trg Source設定
	if ((status = counterSetTrgSource (GENICAM_COUNTER_TRG_SOURCE_ACQUISITION_START_TRIGGER)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Reset Source設定
	if ((status = counterSetResetSource (GENICAM_COUNTER_RESET_SOURCE_ACQUISITION_START_TRIGGER)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Duration Value設定
	if ((status = counterSetDuration (GENICAM_COUNTER_DURATION_MAX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Reset設定
	if ((status = counterReset ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Trg モード取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pLineSrc			：外部信号の番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetTrgMode (int *pLineSrc)
{
	int status = AVAL_STATUS_SUCCESS;

	// Counterセレクト設定
	if ((status = counterSetSelect (COUNTER2_SELECT)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Event Source取得
	if ((status = counterGetEventSource (pLineSrc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Trg カウント取得(外部信号の入力カウント数)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：外部信号の入力カウント数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetTrgInputCount (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Counterセレクト設定
	if ((status = counterSetSelect (COUNTER2_SELECT)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Value取得
	if ((status = counterGetValue (pCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Counter Trg カウント取得(露光したカウント数)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：露光したカウント数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int counterGetImageOutputCount (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Counterセレクト設定
	if ((status = counterSetSelect (COUNTER3_SELECT)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Counter Value取得
	if ((status = counterGetValue (pCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}

// eof
