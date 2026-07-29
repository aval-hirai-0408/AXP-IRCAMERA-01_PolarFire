//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// digitalIoControl.c - Genicam Digital I/O Control Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gDigitalIoStatus = 0;	// Digital IO Status


//**********************************************************************************
//	Digital IO Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;

	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "Digital IO Initialize\n");

	// Register Data Restore(Digital IO Line)
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_DIGITAL_LINE_VERSION2_ADRS, CAMERA_SAVE_DIGITAL_LINE_VERSION2_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gDigitalIoStatus = status;
		goto _DONE;
	}

	// User Selector Default
	digitalIoSetUserSelect (USER_SELECTOR_DEFAULT);

_DONE:
	return (status);
}


//**********************************************************************************
//	Lineセレクト設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：Lineセレクト
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoSetLineSelect (int select)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Select Parameter
	if ((status = lfLineSelectorCheck (select)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Line Selectモード設定
	OUT32 (GENICAM_DIGITAL_LINE_SELECT_ADRS, (select & GENICAM_DIGITAL_LINE_SELECT_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Lineセレクト取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：Lineセレクトを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetLineSelect (int *pSelect)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pSelect Parameter
	if (pSelect == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Line Select pSelect NULL Parameter Error.\n");
		goto _DONE;
	}

	// Line Select取得
	*pSelect = IN32 (GENICAM_DIGITAL_LINE_SELECT_ADRS) & GENICAM_DIGITAL_LINE_SELECT_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	ラインセレクタCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：ライン番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfLineSelectorCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case GENICAM_DIGITAL_LINE_SELECT_LINE0:
		case GENICAM_DIGITAL_LINE_SELECT_LINE1:
		case GENICAM_DIGITAL_LINE_SELECT_LINE2:
		case GENICAM_DIGITAL_LINE_SELECT_LINE3:
		case GENICAM_DIGITAL_LINE_SELECT_LINE4:
		case GENICAM_DIGITAL_LINE_SELECT_LINE5:
		case GENICAM_DIGITAL_LINE_SELECT_LINE6:

		case GENICAM_DIGITAL_LINE_SELECT_CC1:
		case GENICAM_DIGITAL_LINE_SELECT_CC2:
		case GENICAM_DIGITAL_LINE_SELECT_CC3:
		case GENICAM_DIGITAL_LINE_SELECT_CC4:
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Line Select Number(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;
	}


	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Line Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=入力信号/1=出力信号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoSetMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int select;

	// Check mode Parameter
	if ((mode != GENICAM_DIGITAL_LINE_MODE_INPUT) && (mode != GENICAM_DIGITAL_LINE_MODE_OUTPUT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Line Mode(%d) Parameter Error. (Input:%d / Output:%d)\n", mode, GENICAM_DIGITAL_LINE_MODE_INPUT, GENICAM_DIGITAL_LINE_MODE_OUTPUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// ラインセレクト取得
	if ((status = digitalIoGetLineSelect (&select)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	// CC信号?
	if (mode == GENICAM_DIGITAL_LINE_MODE_OUTPUT)
	{
		if ((select >= GENICAM_DIGITAL_LINE_SELECT_CC1) && (select <= GENICAM_DIGITAL_LINE_SELECT_CC4))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Line Mode CC signal is Selected Error.\n");
			goto _DONE;
		}
	}

	// Line Mode設定
	OUT32 (GENICAM_DIGITAL_LINE_MODE_ADRS, (mode & GENICAM_DIGITAL_LINE_MODE_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Line Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：0=入力信号/1=出力信号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Line Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Line Mode取得
	*pMode = IN32 (GENICAM_DIGITAL_LINE_MODE_ADRS) & GENICAM_DIGITAL_LINE_MODE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Line反転設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=信号のアクティブレベル反転なし/1=信号のアクティブレベル反転
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoSetInverter (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// check mode Parameter
	if ((mode != DIGITAL_LINE_INVERTER_DISABLE) && (mode != DIGITAL_LINE_INVERTER_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Line Inverter mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, DIGITAL_LINE_INVERTER_DISABLE, DIGITAL_LINE_INVERTER_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Line Inverter設定
	OUT32 (GENICAM_DIGITAL_LINE_INVERTER_ADRS, (mode & GENICAM_DIGITAL_LINE_INVERTER_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Line反転取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：0=信号のアクティブレベル反転なし/1=信号のアクティブレベル反転を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetInverter (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Line Inverter pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Line Inverter取得
	*pMode = IN32 (GENICAM_DIGITAL_LINE_INVERTER_ADRS) & GENICAM_DIGITAL_LINE_INVERTER_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Line Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pDirection			：Lineステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetLineStatus (int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Line Status pStatus NULL Parameter Error.\n");
		goto _DONE;
	}

	// Line Status取得
	*pStatus = IN32 (GENICAM_DIGITAL_LINE_STATUS_ADRS) & GENICAM_DIGITAL_LINE_STATUS_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Line All Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pDirection			：Lineステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetLineAllStatus (unsigned int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Line All Status pStatus NULL Parameter Error.\n");
		goto _DONE;
	}

	*pStatus = IN32 (GENICAM_DIGITAL_LINE_STATUS_ALL_ADRS) & GENICAM_DIGITAL_LINE_STATUS_ALL_MASK_EXT_VERSION2;

_DONE:
	return (status);
}


//**********************************************************************************
//	Lineソース設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		source				：Lineソース
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoSetLineSource (int source)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	int select;

	// Check Line Source Parameter
	if ((status = lfLineSourceCheck (source)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ラインセレクト取得
	if ((status = digitalIoGetLineSelect (&select)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	// Line Source設定
	OUT32 (GENICAM_DIGITAL_LINE_SOURCE_ADRS, (source & GENICAM_DIGITAL_LINE_SOURCE_MASK));

	// 無効設定かを確認
	data = IN32 (GENICAM_DIGITAL_LINE_TS_INVALIDATE_ADRS);

	if (data & GENICAM_DIGITAL_LINE_TS_INVALIDATE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Line All Status pStatus NULL Parameter Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Lineソース取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSource				：Lineソースを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetLineSource (int *pSource)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pSource Parameter
	if (pSource == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Line Source pSource NULL Parameter Error.\n");
		goto _DONE;
	}

	// 無効設定かを確認
	data = IN32 (GENICAM_DIGITAL_LINE_TS_INVALIDATE_ADRS);

	if (data & GENICAM_DIGITAL_LINE_TS_INVALIDATE_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital IO Invalid Source Error.\n");
		goto _DONE;
	}

	// Line Source取得
	*pSource = IN32 (GENICAM_DIGITAL_LINE_SOURCE_ADRS) & GENICAM_DIGITAL_LINE_SOURCE_MASK;

	// Line Sourceが0の場合がある
	if (*pSource == 0)
		*pSource = GENICAM_DIGITAL_LINE_SOURCE_OFF;

_DONE:
	return (status);
}


//**********************************************************************************
//	ラインソースCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：ライン番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfLineSourceCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case GENICAM_DIGITAL_LINE_SOURCE_ACQUISITION_TRG_WAIT:
		case GENICAM_DIGITAL_LINE_SOURCE_ACQUISITION_ACTIVE:
		case GENICAM_DIGITAL_LINE_SOURCE_FRAME_TRG_WAIT:
		case GENICAM_DIGITAL_LINE_SOURCE_FRAME_ACTIVE:
		case GENICAM_DIGITAL_LINE_SOURCE_EXPOSURE_ACTIVE:

		case GENICAM_DIGITAL_LINE_SOURCE_USER0_OUTPUT:
		case GENICAM_DIGITAL_LINE_SOURCE_USER1_OUTPUT:
		case GENICAM_DIGITAL_LINE_SOURCE_USER2_OUTPUT:
		case GENICAM_DIGITAL_LINE_SOURCE_USER3_OUTPUT:
		case GENICAM_DIGITAL_LINE_SOURCE_USER4_OUTPUT:
		case GENICAM_DIGITAL_LINE_SOURCE_USER5_OUTPUT:
		case GENICAM_DIGITAL_LINE_SOURCE_USER6_OUTPUT:

		case GENICAM_DIGITAL_LINE_SOURCE_COUNTER0_ACTIVE:
		case GENICAM_DIGITAL_LINE_SOURCE_COUNTER1_ACTIVE:
		case GENICAM_DIGITAL_LINE_SOURCE_COUNTER2_ACTIVE:
		case GENICAM_DIGITAL_LINE_SOURCE_COUNTER3_ACTIVE:
		case GENICAM_DIGITAL_LINE_SOURCE_TIMER0_ACTIVE:
		case GENICAM_DIGITAL_LINE_SOURCE_TIMER1_ACTIVE:
		case GENICAM_DIGITAL_LINE_SOURCE_TIMER2_ACTIVE:
		case GENICAM_DIGITAL_LINE_SOURCE_TIMER3_ACTIVE:
		case GENICAM_DIGITAL_LINE_SOURCE_OFF:
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Line Source Number(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Lineフォーマット設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		format				：Lineフォーマット
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoSetLineFormat (int format)
{
	int status = AVAL_STATUS_SUCCESS;
	int select;

	// Check format Parameter
	if ((status = lfLineformatCheck (format)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ラインセレクト取得
	if ((status = digitalIoGetLineSelect (&select)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Line Format設定
	OUT32 (GENICAM_DIGITAL_LINE_FORMAT_ADRS, (format & GENICAM_DIGITAL_LINE_FORMAT_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Lineフォーマット取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFormat			：Lineフォーマットを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetLineFormat (int *pFormat)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pFormat Parameter
	if (pFormat == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Line Format pFormat NULL Parameter Error.\n");
		goto _DONE;
	}

	// Line Format取得
	*pFormat = IN32 (GENICAM_DIGITAL_LINE_FORMAT_ADRS) & GENICAM_DIGITAL_LINE_FORMAT_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	ラインフォーマットCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：ラインフォーマット番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfLineformatCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;
	int select;

	// ラインセレクト取得
	if ((status = digitalIoGetLineSelect (&select)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	switch (number)
	{
		case DIGITAL_LINE_FORMAT_NON_CONNECT:
		case DIGITAL_LINE_FORMAT_TLL:
			break;

		case DIGITAL_LINE_FORMAT_RS422:
		case DIGITAL_LINE_FORMAT_RS422_WITHOUT_TERM:
			if ((select == 0) || (select == 1))
			{
				break;
			}
			else
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "Line Format Number(%d) Parameter Error.\n", number);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				break;
			}

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Line Format Number(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Userセレクタ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：Userセレクタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoSetUserSelect (int select)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check select Parameter
	if ((select < DIGITAL_USER_SELECT_MIN) || (select > DIGITAL_USER_SELECT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "User Select(%d) Parameter Error. (Min:%d / Max:%d)\n", select, DIGITAL_USER_SELECT_MIN, DIGITAL_USER_SELECT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// User Select設定
	OUT32 (GENICAM_DIGITAL_USER_SELECT_ADRS, (select & DIGITAL_USER_SELECT_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Userセレクタ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSelect				：Userセレクタを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetUserSelect (int *pSelect)
{
	int status = AVAL_STATUS_SUCCESS;
	int mask;

	// Check pSelect Parameter
	if (pSelect == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "User Select pSelect NULL Parameter Error.\n");
		goto _DONE;
	}

	// User Select取得
	mask = GENICAM_DIGITAL_USER_SELECT_EXT_VERSION2_MASK;
	*pSelect = IN32 (GENICAM_DIGITAL_USER_SELECT_ADRS) & mask;

_DONE:
	return (status);
}


//**********************************************************************************
//	User Value設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		value				：User value
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoSetUserValue (int value)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check value Parameter
	if ((value != DIGITAL_USER_VALUE_NEGATE) && (value != DIGITAL_USER_VALUE_ASSERT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "User Value(%d) Parameter Error. (Negate:%d / Assert:%d)\n", value, DIGITAL_USER_VALUE_NEGATE, DIGITAL_USER_VALUE_ASSERT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// User Value設定
	OUT32 (GENICAM_DIGITAL_USER_VALUE_ADRS, (value & GENICAM_DIGITAL_USER_VALUE_MASK));

_DONE:
	return (status);
}



//**********************************************************************************
//	User Value取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：User Value格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetUserValue (int *pValue)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pValue Parameter
	if (pValue == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "User Value pValue NULL Parameter Error.\n");
		goto _DONE;
	}

	// User Value取得
	*pValue = IN32 (GENICAM_DIGITAL_USER_VALUE_ADRS) & GENICAM_DIGITAL_USER_VALUE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	User All Value設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：User Value格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoSetAllValue (unsigned int value)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int mask;

	mask = GENICAM_DIGITAL_USER_ALL_EXT_VERSION2_MASK;

	// Check value Parameter
	if (value > mask)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "User All Value(%d) Parameter Error. (Min:%d / Max:%d)\n", value, GENICAM_DIGITAL_USER_ALL_MIN, mask);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// User All Value設定
	OUT32 (GENICAM_DIGITAL_USER_ALL_VALUE_ADRS, (value & mask));

_DONE:
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	User All Value取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：User Value格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetAllValue (unsigned int *pValue)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int mask;

	// Check pValue Parameter
	if (pValue == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "User All Value pValue NULL Parameter Error.\n");
		goto _DONE;
	}

	// User All Value取得
	mask = GENICAM_DIGITAL_USER_ALL_EXT_VERSION2_MASK;
	*pValue = IN32 (GENICAM_DIGITAL_USER_ALL_VALUE_ADRS) & mask;

_DONE:
	return (status);
}


//**********************************************************************************
//	User Mask設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mask				：User Mask
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoSetUserMask (unsigned int mask)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int regMask;

	regMask = GENICAM_DIGITAL_USER_MASK_EXT_VERSION2_MASK;

	// Check mask Parameter
	if (mask > regMask)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "User Mask(0x%x) Parameter Error. (Max:0x%x)\n", mask, regMask);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// User Mask設定
	OUT32 (GENICAM_DIGITAL_USER_MASK_ADRS, (mask & regMask));
	
_DONE:
	return (status);
}


//**********************************************************************************
//	User Mask取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mask				：User Maskを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetUserMask (unsigned int *pMask)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int regMask;

	// Check pMask Parameter
	if (pMask == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "User Mask pMask NULL Parameter Error.\n");
		goto _DONE;
	}

	// User Mask取得
	regMask = GENICAM_DIGITAL_USER_MASK_EXT_VERSION2_MASK;
	*pMask = IN32 (GENICAM_DIGITAL_USER_MASK_ADRS) & regMask;

_DONE:
	return (status);
}


//**********************************************************************************
//	User Output設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：User Select Number
//		value				：User Output Value
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoSetUserOutput (int select, int value)
{
	int status = AVAL_STATUS_SUCCESS;
	int min, max;

	min = DIGITAL_USER_SELECT_MIN;
	max = DIGITAL_USER_SELECT_EXT_VERSION2_MAX;
	
	// Check select Parameter
	if ((select < min) || (select > max))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "User Output select(%d) Parameter Error. (Min:%d / Max:%d)\n", select, min, max);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check value Parameter
	if ((value != DIGITAL_USER_VALUE_NEGATE) && (value != DIGITAL_USER_VALUE_ASSERT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "User Output Value(%d) Parameter Error. (Negate:%d / Assert:%d)\n", value, DIGITAL_USER_VALUE_NEGATE, DIGITAL_USER_VALUE_ASSERT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// User Output設定
	OUT32 ((GENICAM_DIGITAL_USER0_OUTPUT_ADRS + select *4), (value & GENICAM_DIGITAL_USER_OUTPUT_MASK));
	
_DONE:
	return (status);
}


//**********************************************************************************
//	User Output取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：User Select Number
//		pValue				：User Output Valueを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalIoGetUserOutput (int select, int *pValue)
{
	int status = AVAL_STATUS_SUCCESS;
	int min, max;

	min = DIGITAL_USER_SELECT_MIN;
	max = DIGITAL_USER_SELECT_EXT_VERSION2_MAX;
	
	// Check select Parameter
	if ((select < min) || (select > max))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "User Output select(%d) Parameter Error. (Min:%d / Max:%d)\n", select, min, max);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pValue Parameter
	if (pValue == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "User Output pValue NULL Parameter Error.\n");
		goto _DONE;
	}

	// User Output取得
	*pValue = IN32 ((GENICAM_DIGITAL_USER0_OUTPUT_ADRS + select *4));
	
_DONE:
	return (status);
}

// eof
