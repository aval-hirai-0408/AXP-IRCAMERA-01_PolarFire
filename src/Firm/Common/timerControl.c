//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// timerControl.c - Genicam Timer Control Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gTimerControlStatus = 0;	// Timer Control Status


//**********************************************************************************
//	Timer Control Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;

	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "Timer Control Initialize\n");

	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_TIMER_ADRS, CAMERA_SAVE_TIMER_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gTimerControlStatus = status;
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Timerセレクト設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：Timerセレクト
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlSetSelect (int select)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check select Parameter
	if ((select < TIMER_SELECT_MIN) || (select > TIMER_SELECT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Timer Control Select(%d) Parameter Error. (Min:%d / Max:%d)\n", select, TIMER_SELECT_MIN, TIMER_SELECT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Timer Select設定
	OUT32 (GENICAM_TIMER_SELECT_ADRS, (select & GENICAM_TIMER_SELECT_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Timerセレクト取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSelect				：Timerセレクトをっかうのうするポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlGetSelect (int *pSelect)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pSelect == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Timer Control Select pSelect NULL Parameter Error.\n");
		goto _DONE;
	}

	// Timer Select取得
	*pSelect = IN32 (GENICAM_TIMER_SELECT_ADRS) & GENICAM_TIMER_SELECT_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Timer Duration設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：Timerカウント
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlSetDuration (unsigned int count)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check Count Parameter
	if (count > GENICAM_TIMER_DURATION_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Timer Control Duration(%d) Parameter Error. (Min:0 / Max:%d)\n", count, GENICAM_TIMER_DURATION_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Timer Count設定
	OUT32 (GENICAM_TIMER_DURATION_ADRS, (count & GENICAM_TIMER_DURATION_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Timer Duration取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：Timerカウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlGetDuration (unsigned int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Timer Control Duration pCount NULL Parameter Error.\n");
		goto _DONE;
	}

	// Timer Count取得
	*pCount = IN32 (GENICAM_TIMER_DURATION_ADRS) & GENICAM_TIMER_DURATION_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Timer Delay設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：Timerカウント
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlSetDelay (unsigned int count)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Count Parameter
	if (count > GENICAM_TIMER_DELAY_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Timer Control Delay(%d) Parameter Error. (Min:0 / Max:%d)\n", count, GENICAM_TIMER_DELAY_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Timer Delay設定
	OUT32 (GENICAM_TIMER_DELAY_ADRS, (count & GENICAM_TIMER_DELAY_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Timer Delay取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：Timerカウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlGetDelay (unsigned int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Timer Control Delay pCount NULL Parameter Error.\n");
		goto _DONE;
	}

	// Timer Delay取得
	*pCount = IN32 (GENICAM_TIMER_DELAY_ADRS) & GENICAM_TIMER_DELAY_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Timer Reset設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlReset (void)
{
	// Timer Reset設定
	OUT32 (GENICAM_TIMER_RESET_ADRS, GENICAM_TIMER_RESET);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	現在のTimer Count設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：Timerカウント
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlSetValue (unsigned int count)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Count Parameter
	if (count > GENICAM_TIMER_CURRENT_COUNT_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Timer Control Value(%d) Parameter Error. (Min:0 / Max:%d)\n", count, GENICAM_TIMER_CURRENT_COUNT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Timer Current Count設定
	OUT32 (GENICAM_TIMER_CURRENT_COUNT_ADRS, (count & GENICAM_TIMER_CURRENT_COUNT_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	現在のTimer Count取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：Timerカウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlGetValue (unsigned int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Timer Control Value pCount NULL Parameter Error.\n");
		goto _DONE;
	}

	// Timer Current Count取得
	*pCount = IN32 (GENICAM_TIMER_CURRENT_COUNT_ADRS) & GENICAM_TIMER_CURRENT_COUNT_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Timer Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStatus				：Timerステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlGetStatus (unsigned int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Timer Control Status pStatus NULL Parameter Error.\n");
		goto _DONE;
	}

	// Timer Status取得
	*pStatus = IN32 (GENICAM_TIMER_STATUS_ADRS) & GENICAM_TIMER_STATUS_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Timerソース設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		source				：Timer接続元
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlSetSource (int source)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check source Parameter
	if ((status = lfTimerControlSourceCheck (source)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Timer Source設定
	OUT32 (GENICAM_TIMER_SOURCE_ADRS, (source & GENICAM_TIMER_SOURCE_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Timerソース取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		source				：Timer接続元を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlGetSource (int *pSource)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pSource Parameter
	if (pSource == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Timer Control Source pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Timer Source取得
	*pSource = IN32 (GENICAM_TIMER_SOURCE_ADRS) & GENICAM_TIMER_SOURCE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Timer Source Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：Source番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfTimerControlSourceCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case GENICAM_TIMER_SOURCE_ACQUISITION_START_TRIGGER:
		case GENICAM_TIMER_SOURCE_ACQUISITION_START:
		case GENICAM_TIMER_SOURCE_ACQUISITION_END:
		case GENICAM_TIMER_SOURCE_FRAME_START_TRIGGER:
		case GENICAM_TIMER_SOURCE_FRAME_START:
		case GENICAM_TIMER_SOURCE_FRAME_END:

		case GENICAM_TIMER_SOURCE_EXPOSURE_START:
		case GENICAM_TIMER_SOURCE_EXPOSURE_END:

		case GENICAM_TIMER_SOURCE_LINE0:
		case GENICAM_TIMER_SOURCE_USER0_OUTPUT:
		case GENICAM_TIMER_SOURCE_LINE1:
		case GENICAM_TIMER_SOURCE_USER1_OUTPUT:
		case GENICAM_TIMER_SOURCE_LINE2:
		case GENICAM_TIMER_SOURCE_USER2_OUTPUT:
		case GENICAM_TIMER_SOURCE_LINE3:
		case GENICAM_TIMER_SOURCE_USER3_OUTPUT:
		case GENICAM_TIMER_SOURCE_LINE4:
		case GENICAM_TIMER_SOURCE_USER4_OUTPUT:
		case GENICAM_TIMER_SOURCE_LINE5:
		case GENICAM_TIMER_SOURCE_USER5_OUTPUT:
		case GENICAM_TIMER_SOURCE_LINE6:
		case GENICAM_TIMER_SOURCE_USER6_OUTPUT:

		case GENICAM_TIMER_SOURCE_COUNTER0_START:
		case GENICAM_TIMER_SOURCE_COUNTER0_END:
		case GENICAM_TIMER_SOURCE_COUNTER1_START:
		case GENICAM_TIMER_SOURCE_COUNTER1_END:
		case GENICAM_TIMER_SOURCE_COUNTER2_START:
		case GENICAM_TIMER_SOURCE_COUNTER2_END:
		case GENICAM_TIMER_SOURCE_COUNTER3_START:
		case GENICAM_TIMER_SOURCE_COUNTER3_END:
		case GENICAM_TIMER_SOURCE_TIMER0_START:
		case GENICAM_TIMER_SOURCE_TIMER0_END:
		case GENICAM_TIMER_SOURCE_TIMER1_START:
		case GENICAM_TIMER_SOURCE_TIMER1_END:
		case GENICAM_TIMER_SOURCE_TIMER2_START:
		case GENICAM_TIMER_SOURCE_TIMER2_END:
		case GENICAM_TIMER_SOURCE_TIMER3_START:
		case GENICAM_TIMER_SOURCE_TIMER3_END:
#if defined (MODE_ENCODER)
		case GENICAM_TIMER_SOURCE_ENCODER0:
#endif
		case GENICAM_TIMER_SOURCE_OFF:
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Timer Source(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;

	}

	return (status);
}


//**********************************************************************************
//	Timer Active設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Active論理
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlSetTrgActive (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check mode Parameter
	if ((mode < TIMER_ACTIVE_MIN) || (mode > TIMER_ACTIVE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Timer Control Activation Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, TIMER_ACTIVE_MIN, TIMER_ACTIVE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Timer Activation設定
	OUT32 (GENICAM_TIMER_ACTIVE_ADRS, (mode & GENICAM_TIMER_ACTIVE_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Timer Active取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Active論理を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerControlGetTrgActive (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Timer Control Activation pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Timer Activation取得
	*pMode = IN32 (GENICAM_TIMER_ACTIVE_ADRS) & GENICAM_TIMER_ACTIVE_MASK;

_DONE:
	return (status);
}

// eof
