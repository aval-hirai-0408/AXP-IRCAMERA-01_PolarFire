//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// encoderControl.c - Genicam Encoder Control Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gEncoderStatus = 0;		// Encoder Status


#if defined (MODE_ENCODER)
//**********************************************************************************
//	Encoder Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT ("Encoder IO Initialize\n");

	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_ENCODER_ADRS, CAMERA_SAVE_ENCODER_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gEncoderStatus = status;
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder PaseAソース設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		source				：ソース
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderSetPhaseATrgSource (int source)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check Encoder PhaseA Source Parameter
	if ((status = lfEncoderPhaseATrgSourceCheck (source)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Encoder PhaseA Source設定
	OUT32 (GENICAM_ENCODER_PASEA_TRG_SOURCE_ADRS, (source & GENICAM_ENCODER_PASEA_TRG_SOURCE_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_ENCODER_PASEA_INVALIDATE_ADRS);

	if (data & GENICAM_ENCODER_PHASEA_INVALIDATE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder PhaseA Trg Invalidate Source Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder PhaseAソース取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSource				：ソースを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetPhaseATrgSource (int *pSource)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pSource Parameter
	if (pSource == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder PhaseA Trg Source pSource NULL Parameter Error.\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_ENCODER_PASEA_INVALIDATE_ADRS);

	if (data & GENICAM_ENCODER_PHASEA_INVALIDATE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder PhaseA Trg Invalidate Source Error.\n");
		goto _DONE;
	}

	// Encoder PhaseA Source取得
	*pSource = IN32 (GENICAM_ENCODER_PASEA_TRG_SOURCE_ADRS) & GENICAM_ENCODER_PASEA_TRG_SOURCE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder PhaseAソースCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：source番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfEncoderPhaseATrgSourceCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case GENICAM_ENCODER_PASEA_TRG_SOURCE_LINE0:
		case GENICAM_ENCODER_PASEA_TRG_SOURCE_LINE1:
		case GENICAM_ENCODER_PASEA_TRG_SOURCE_OFF:
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Encoder PhaseA Trg Source Number(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder PaseBソース設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		source				：ソース
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderSetPhaseBTrgSource (int source)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check Encoder PhaseB Source Parameter
	if ((status = lfEncoderPhaseBTrgSourceCheck (source)) != AVAL_STATUS_SUCCESS)
		return (status);

	// Encoder PhaseB Source設定
	OUT32 (GENICAM_ENCODER_PASEB_TRG_SOURCE_ADRS, (source & GENICAM_ENCODER_PASEB_TRG_SOURCE_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_ENCODER_PASEB_INVALIDATE_ADRS);

	if (data & GENICAM_ENCODER_PHASEB_INVALIDATE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder PhaseB Trg Invalidate Source Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder PhaseBソース取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSource				：ソースを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetPhaseBTrgSource (int *pSource)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pSource Parameter
	if (pSource == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder PhaseB Trg Source pSource NULL Parameter Error.\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_ENCODER_PASEB_INVALIDATE_ADRS);

	if (data & GENICAM_ENCODER_PHASEB_INVALIDATE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder PhaseB Trg Invalidate Source Error.\n");
		goto _DONE;
	}

	// Encoder PhaseB Source取得
	*pSource = IN32 (GENICAM_ENCODER_PASEB_TRG_SOURCE_ADRS) & GENICAM_ENCODER_PASEB_TRG_SOURCE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder PhaseBソースCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：source番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfEncoderPhaseBTrgSourceCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case GENICAM_ENCODER_PASEB_TRG_SOURCE_LINE0:
		case GENICAM_ENCODER_PASEB_TRG_SOURCE_LINE1:
		case GENICAM_ENCODER_PASEB_TRG_SOURCE_OFF:
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Encoder PhaseB Trg Source Number(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderSetMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check mode Parameter
	if ((mode != GENICAM_ENCODER_MODE_HI_PRECISION) && (mode != GENICAM_ENCODER_MODE_HI_RESOLUTION))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Encoder Mode(%d) Parameter Error. (%d:High Precision / %d:High Resolution)\n", mode, GENICAM_ENCODER_MODE_HI_PRECISION, GENICAM_ENCODER_MODE_HI_RESOLUTION);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Encoder Mode設定
	OUT32 (GENICAM_ENCODER_MODE_ADRS, (mode & GENICAM_ENCODER_MODE_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：モードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Encoder Mode取得
	*pMode = IN32 (GENICAM_ENCODER_MODE_ADRS) & GENICAM_ENCODER_MODE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Dvider設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		divider				：分周値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderSetDivider (int divider)
{
	int status = AVAL_STATUS_SUCCESS;
	int mode;

	// Check divider Parameter
	if ((divider < GENICAM_ENCODER_DVIDER_MIN) || (divider > GENICAM_ENCODER_DVIDER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Encoder divider(%d) Parameter Error. (Min:%d / Max:%d)\n", divider, GENICAM_ENCODER_DVIDER_MIN, GENICAM_ENCODER_DVIDER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Encoder Divider設定
	OUT32 (GENICAM_ENCODER_DVIDER_ADRS, (divider & GENICAM_ENCODER_DVIDER_MASK));

	// Encoder Output MOde取得
	if ((status = encoderGetOutputMode (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((mode == GENICAM_ENCODER_OUTPUT_MODE_POSITION_UP) || (mode == GENICAM_ENCODER_OUTPUT_MODE_POSITION_DOWN))
	{
		// Encoder Reset
		encoderSetReset ();
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Dvider取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pDdivider				：分周値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetDivider (int *pDdivider)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pDdivider Parameter
	if (pDdivider == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Divider pDdivider NULL Parameter Error.\n");
		goto _DONE;
	}

	// Encoder Divider取得
	*pDdivider = IN32 (GENICAM_ENCODER_DVIDER_ADRS) & GENICAM_ENCODER_DVIDER_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Output Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderSetOutputMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check mode Parameter
	if ((mode < GENICAM_ENCODER_OUTPUT_MODE_MIN) || (mode > GENICAM_ENCODER_OUTPUT_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Encoder Output Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, GENICAM_ENCODER_OUTPUT_MODE_MIN, GENICAM_ENCODER_OUTPUT_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Encoder Output Mode設定
	OUT32 (GENICAM_ENCODER_OUTPUT_MODE_ADRS, (mode & GENICAM_ENCODER_OUTPUT_MODE_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Output Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：モードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetOutputMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Output Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Encoder Output Mode取得
	*pMode = IN32 (GENICAM_ENCODER_OUTPUT_MODE_ADRS) & GENICAM_ENCODER_OUTPUT_MODE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStatus				：ステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetStatus (int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Status pStatus NULL Parameter Error.\n");
		goto _DONE;
	}

	// Encoder Status取得
	*pStatus = IN32 (GENICAM_ENCODER_STATUS_ADRS) & GENICAM_ENCODER_STATUS_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Timeout設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		timeout				：タイムアウト時間
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderSetTimeout (int timeout)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check timeout Parameter
	if ((timeout < GENICAM_ENCODER_TIMEOUT_MIN) || (timeout > GENICAM_ENCODER_TIMEOUT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Encoder Timeout(%d) Parameter Error. (Min:%d / Max:%d)\n", timeout, GENICAM_ENCODER_TIMEOUT_MIN, GENICAM_ENCODER_TIMEOUT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Encoder Timeout設定
	OUT32 (GENICAM_ENCODER_TIMEOUT_ADRS, (timeout & GENICAM_ENCODER_TIMEOUT_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Timeout取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTimeout				：タイムアウト時間を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetTimeout (int *pTimeout)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pTimeout Parameter
	if (pTimeout == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Timeout pTimeout NULL Parameter Error.\n");
		goto _DONE;
	}

	// Encoder Timeout取得
	*pTimeout = IN32 (GENICAM_ENCODER_TIMEOUT_ADRS) & GENICAM_ENCODER_TIMEOUT_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Reset Trgソース設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		source				：ソース
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderSetResetTrgSource (int source)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check Encoder Reset Trg Source Parameter
	if ((status = lfEncoderResetTrgASourceCheck (source)) != AVAL_STATUS_SUCCESS)
		return (status);

	// Encoder Reset Trg Source設定
	OUT32 (GENICAM_ENCODER_RESET_TRG_SOURCE_ADRS, (source & GENICAM_ENCODER_RESET_TRG_SOURCE_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_ENCODER_RESET_TS_INVALIDATE_ADRS);

	if (data & GENICAM_ENCODER_RESET_TS_INVALIDATE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Reset Trg Invalidate Source Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Reset Trgソース取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSource				：ソースを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetResetTrgSource (int *pSource)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pSource Parameter
	if (pSource == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Reset Trg Source pSource NULL Parameter Error.\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_ENCODER_PASEA_INVALIDATE_ADRS);

	if (data & GENICAM_ENCODER_RESET_TS_INVALIDATE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Reset Trg Invalidate Source Error.\n");
		goto _DONE;
	}

	// Encoder Reset Trg Source取得
	*pSource = IN32 (GENICAM_ENCODER_RESET_TRG_SOURCE_ADRS) & GENICAM_ENCODER_RESET_TRG_SOURCE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Reset TrgソースCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：source番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfEncoderResetTrgASourceCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case GENICAM_ENCODER_RESET_TRG_ACQUISITION_START_TRG:
		case GENICAM_ENCODER_RESET_TRG_ACQUISITION_START:
		case GENICAM_ENCODER_RESET_TRG_ACQUISITION_END:
		case GENICAM_ENCODER_RESET_TRG_LINE0:
		case GENICAM_ENCODER_RESET_TRG_USER0_OUTPUT:
		case GENICAM_ENCODER_RESET_TRG_LINE1:
		case GENICAM_ENCODER_RESET_TRG_USER1_OUTPUT:
		case GENICAM_ENCODER_RESET_TRG_LINE2:
		case GENICAM_ENCODER_RESET_TRG_USER2_OUTPUT:
		case GENICAM_ENCODER_RESET_TRG_USER3_OUTPUT:
		case GENICAM_ENCODER_RESET_TRG_USER4_OUTPUT:
		case GENICAM_ENCODER_RESET_TRG_USER5_OUTPUT:
		case GENICAM_ENCODER_RESET_TRG_USER6_OUTPUT:

#if defined (MODE_GENICAM_COUNTER)
		case GENICAM_ENCODER_RESET_TRG_COUNTER0_START:
		case GENICAM_ENCODER_RESET_TRG_COUNTER0_END:
		case GENICAM_ENCODER_RESET_TRG_COUNTER1_START:
		case GENICAM_ENCODER_RESET_TRG_COUNTER1_END:
		case GENICAM_ENCODER_RESET_TRG_COUNTER2_START:
		case GENICAM_ENCODER_RESET_TRG_COUNTER2_END:
		case GENICAM_ENCODER_RESET_TRG_COUNTER3_START:
		case GENICAM_ENCODER_RESET_TRG_COUNTER3_END:
#endif

#if defined (MODE_GENICAM_TIMER)
		case GENICAM_ENCODER_RESET_TRG_TIMER0_START:
		case GENICAM_ENCODER_RESET_TRG_TIMER0_END:
		case GENICAM_ENCODER_RESET_TRG_TIMER1_START:
		case GENICAM_ENCODER_RESET_TRG_TIMER1_END:
		case GENICAM_ENCODER_RESET_TRG_TIMER2_START:
		case GENICAM_ENCODER_RESET_TRG_TIMER2_END:
		case GENICAM_ENCODER_RESET_TRG_TIMER3_START:
		case GENICAM_ENCODER_RESET_TRG_TIMER3_END:
#endif
		case GENICAM_ENCODER_RESET_TRG_OFF:
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Encoder Reset Trg Source Number(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;
	}

	return (status);
}


//**********************************************************************************
//	Encoder Reset Activation設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		activation			：アクティベーション
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderSetResetActivation (int activation)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	
	// Check activation Parameter
	if ((activation < GENICAM_ENCODER_RESET_ACTIVATION_MIN) || (activation > GENICAM_ENCODER_RESET_ACTIVATION_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Encoder Reset Activation(%d) Parameter Error. (Min:%d / Max:%d)\n", activation, GENICAM_ENCODER_RESET_ACTIVATION_MIN, GENICAM_ENCODER_RESET_ACTIVATION_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Encoder Reset Activation設定
	OUT32 (GENICAM_ENCODER_RESET_ACTIVATION_ADRS, (activation & GENICAM_ENCODER_RESET_ACTIVATION_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_ENCODER_RESET_TA_INVALIDATE_ADRS);

	if (data & GENICAM_ENCODER_RESET_TA_INVALIDATE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Reset Activation Invalidate Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Reset Activation取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pActivation				：アクティベーションを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetResetActivation (int *pActivation)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pActivation Parameter
	if (pActivation == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Reset Activation pActivation NULL Parameter Error.\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_ENCODER_RESET_TA_INVALIDATE_ADRS);

	if (data & GENICAM_ENCODER_RESET_TA_INVALIDATE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Reset Activation Invalidate Error.\n");
		goto _DONE;
	}

	// Encoder Reset Activation
	*pActivation = IN32 (GENICAM_ENCODER_RESET_ACTIVATION_ADRS) & GENICAM_ENCODER_RESET_ACTIVATION_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Reset 設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderSetReset (void)
{
	// Encoder Reset設定
	OUT32 (GENICAM_ENCODER_RESET_ADRS, GENICAM_ENCODER_RESET);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Encoder Value設定(High => Lowの順に設定)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		value				：Encoder Value
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderSetValue (long long value)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int hiValue, loValue;

	// Check value Parameter
	if ((value < (long long int)GENICAM_ENCODER_VALUE_MIN) || (value > GENICAM_ENCODER_VALUE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Encoder Value(%lld) Parameter Error. (Min:%lld / Max:%lld)\n", value, GENICAM_ENCODER_VALUE_MIN, GENICAM_ENCODER_VALUE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 32bit変数に分解
	hiValue = (unsigned int)(value >> 32) & 0xffffffff;
	loValue = (unsigned int)(value  & 0xffffffff);

	// Encoder High Value設定
	OUT32 (GENICAM_ENCODER_VALUE_HI_ADRS, hiValue);

	// Encoder Low Value設定
	OUT32 (GENICAM_ENCODER_VALUE_LO_ADRS, loValue);

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Value取得(Low => highの順に取得)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pHiValue				：Encoder Valueを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetValue (long long *pValue)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int hiValue, loValue;

	// Check pValue Parameter
	if (pValue == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Value pValue NULL Parameter Error.\n");
		goto _DONE;
	}

	// Encoder Low Value取得
	loValue = IN32 (GENICAM_ENCODER_VALUE_LO_ADRS);

	// Encoder High Value取得
	hiValue = IN32 (GENICAM_ENCODER_VALUE_HI_ADRS);

	// 64bitデータ
	*pValue = (long long)((long long)hiValue << 32) | loValue;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Value at Reset取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pHiValue				：Encoder Reset Valueを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int encoderGetValueAtReset (long long *pValue)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int hiValue, loValue;

	// Check pValue Parameter
	if (pValue == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Value at Reset pValue NULL Parameter Error.\n");
		goto _DONE;
	}

	// Encoder Low Value取得
	loValue = IN32 (GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS);

	// Encoder High Value取得
	hiValue = IN32 (GENICAM_ENCODER_VALUE_AT_RESET_HI_ADRS);

	// 64bitデータ
	*pValue = (long long)((long long)hiValue << 32) | loValue;

_DONE:
	return (status);
}


//**********************************************************************************
//	Encoder Frequency取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFreq					：エンコーダトリガ周期を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int encoderGetFrequency (double *pFreq)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double temp;

	// Check pFreq Parameter
	if (pFreq == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Encoder Frequency pFreq NULL Parameter Error.\n");
		goto _DONE;
	}

	// Encoder Frequency取得
	data = IN32 (GENICAM_ENCODER_FREQ_ADRS);

	// Check Timeout
	if (data & GENICAM_ENCODER_FREQ_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sampling....\n");
		goto _DONE;
	}

	if (data == 0)
		data = 1;

	temp =  (double)((double)(GENICAM_ENCODER_FREQ_DIV * GENICAM_ENCODER_FREQ_UNIT) / (double)data);
	*pFreq = temp;	// Hz

_DONE:
	return (status);
}

#endif // MODE_ENCODER

// eof
