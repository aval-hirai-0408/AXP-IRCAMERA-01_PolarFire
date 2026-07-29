//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// dpcAdjust.c - DPC Adjust Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

DPCINFO gDpcInfo;

int gBatchLoopTotal = 0;
int gDPCLoopFfc = 0;
int gDpcBrightCount = 0;

double gSd_thre[] = {DPC_SD_THRESHOLD_0, DPC_SD_THRESHOLD_1};				// 標準偏差
double gNnonuniformity_thre[] = {DPC_NON_THRESHOLD_0, DPC_NON_THRESHOLD_1};	// 輝度不均一性


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------

//  画素欠陥検出
extern unsigned short *pImageSrc;				// 検出用入力画像の先頭を指すポインタ配列
extern double *pImageSd;						// sd納用
extern double *pImageAvePix;					// Average Pixel納用
extern double *pImageNonuniformity;				// Nonuniformity納用
extern unsigned char *pImageDetect[];			// 検出結果格納用

extern unsigned int gDpcImpGridCount;			// 補正不可データカウント
//extern GRID_XY *pDpcImpossibleGrid;			// 欠陥不可検出座標データ

// 画素欠陥補正
extern DEFECTIONINFO *pImageDefectionInfo;		// 補正情報格納用

#ifndef MODE_PELTIER
// センサ温度
extern double gSensortTemp;						// センサターゲット温度
#endif

extern int gDpcImpossibleSaveMode;				// 欠陥不可座標の保存フラグ


//**********************************************************************************
// 欠陥検出カウンタクリア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcClearBatchCount (void)
{
	// カウンタクリア
	dpcSetBatchCount (0);
	
	gDPCLoopFfc = 0;

	// Bright Count
	gDpcBrightCount = 0;

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
// 欠陥検出カウント取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetBatchCount (void)
{
	// カウンタ取得
	return (gBatchLoopTotal);
}


//**********************************************************************************
// 欠陥検出カウント取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：カウンタ値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetBatchCountGlobal (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Get Count pMode NULL Parameter Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// カウンタ値取得
	*pCount = IN32 (FIRM_DATA_DPC_COUNT_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// 欠陥検出カウント設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：カウンタ値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetBatchCount (int count)
{
	// カウンタ設定
	gBatchLoopTotal = count;
	OUT32 (FIRM_DATA_DPC_COUNT_ADRS, gBatchLoopTotal);

	// キャッシュFlash
	cacheFlush ();

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
// 欠陥検出カウンタ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：DPCカウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetCount (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Count pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// カウンタ値取得
	*pCount = dpcGetBatchCount ();

_DONE:
	return (status);
}


//**********************************************************************************
// 欠陥検出カウンタデクリメント
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcDecrementCount (void)
{
	int count;
	int mode;
	
	// カウンタ取得
	count = dpcGetBatchCount();
	
	// カウンタデクリメント
	if (count > 0)
		dpcSetBatchCount (count-1);

	// DPC補正モード取得
	mode = IN32 (FIRM_DATA_DPC_ADJUST_MODE_ADRS);
	if ((mode == DPC_ADJUST_MODE_NE) || (mode == DPC_ADJUST_MODE_NE_FFC_ENABLE))
	{
		if (gDPCLoopFfc > 0)
			gDPCLoopFfc--;
	}
	else if ((mode == DPC_ADJUST_MODE_LINE) || (mode == DPC_ADJUST_MODE_LINE_FFC_ENABLE))
	{
		if (gDPCLoopFfc > 0)
			gDPCLoopFfc--;
	}
	
	// Bright Count
	if (gDpcBrightCount > 0)
		gDpcBrightCount--;

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
// 欠陥検出(一括)パラメータ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcParam			：DPCパラメータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcMainBatch1 (DPC_PARAM dpcParam)
{
	int status = AVAL_STATUS_SUCCESS;

	//----------------------------------------------------------------------
	// Check ffcLoop Parameter
	//----------------------------------------------------------------------
	if ((gDPCLoopFfc < 0) || (gDPCLoopFfc > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC gDPCLoopFfc(%d) Parameter Error.(Min:0 / Max:%d)\n", gDPCLoopFfc, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//----------------------------------------------------------------------
	// DPC情報構造体クリア
	//----------------------------------------------------------------------
	if (gDPCLoopFfc == 0)
		memset ((void *)&gDpcInfo, 0, sizeof(DPCINFO));

	//----------------------------------------------------------------------
	// DPC Parameter Check
	//----------------------------------------------------------------------
	if ((status = dpcCheckParam (dpcParam)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// パラメータ設定
	//----------------------------------------------------------------------
	if ((status = dpcSetParam (&dpcParam)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// DPC情報保存
	//----------------------------------------------------------------------
	if ((status = dpcSetInfo (&dpcParam, gDPCLoopFfc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// カウンタ設定
	//----------------------------------------------------------------------
	gDPCLoopFfc++;

	//----------------------------------------------------------------------
	// Bright Count
	//----------------------------------------------------------------------
	gDpcBrightCount = 0;

	//----------------------------------------------------------------------
	// Auto Bright Mode 無効設定
	//----------------------------------------------------------------------
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	if ((status = autoBrightSetExposureMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = autoBrightSetGainMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif


_DONE:
	// ステータス設定
	dpcSetError (status);
	return (status);
}


//**********************************************************************************
// 欠陥検出(一括)補正画像取り込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcParam			：DPCパラメータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcMainBatch2 (DPC_PARAM dpcParam)
{
	int status = AVAL_STATUS_SUCCESS;
	int count;

	// Check dpcNo Parameter
	if ((dpcParam.dpcNo < DPC_NUMBER_MIN) || (dpcParam.dpcNo > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC dpcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcParam.dpcNo, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check ffcNo Parameter
	if ((dpcParam.ffcNo < FFC_NUMBER_MIN) || (dpcParam.ffcNo > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcParam.ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gDpcBrightCount Parameter
	if ((gDpcBrightCount < DPC_BRIGHTNESS_LOOP_MIN) || (gDpcBrightCount > DPC_BRIGHTNESS_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC gDpcBrightCount(%d) Parameter Error.(Min:%d / Max:%d)\n", gDpcBrightCount, DPC_BRIGHTNESS_LOOP_MIN, DPC_BRIGHTNESS_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// カウント取得
	count = dpcGetBatchCount ();
	sprintf (gLogMsgBuff, "dpcMainBatch2 : count = %d, FFC Loop = %d, Bright Loop = %d\n", count+1, gDPCLoopFfc, gDpcBrightCount+1);
	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

	//----------------------------------------------------------------------
	// 欠陥検出
	//----------------------------------------------------------------------
	if ((status = lfDefectMain (&dpcParam, count, (count%DPC_BRIGHTNESS_LOOP_DEFAULT))) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// DPC詳細情報保存
	//----------------------------------------------------------------------
	if ((status = dpcSetInfoDetail (&dpcParam, gDPCLoopFfc-1, gDpcBrightCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 検出回数
	dpcSetBatchCount (count+1);

	// Bright Count
	gDpcBrightCount++;
	
_DONE:
	// ステータス設定
	dpcSetError (status);

	return (status);
}


//**********************************************************************************
// 欠陥検出(一括)補正計算
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcParam			：DPCパラメータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcMainBatch3 (DPC_PARAM dpcParam)
{
	int status = AVAL_STATUS_SUCCESS;
	int count;

	// Check dpcNo Parameter
	if ((dpcParam.dpcNo < DPC_NUMBER_MIN) || (dpcParam.dpcNo > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC dpcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcParam.dpcNo, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// カウント取得
	count = dpcGetBatchCount ();

	//----------------------------------------------------------------------
	// 補正計算
	//----------------------------------------------------------------------
	if ((status = dpcCorrectionMain (dpcParam, count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	// ステータス設定
	dpcSetError (status);
	return (status);
}


//**********************************************************************************
// DPC情報設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcParam			：DPC構造体を格納するポインタ
//		ffcLoop				：テスト回数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetInfo (DPC_PARAM *pDpcParam, int ffcLoop)
{
	int status = AVAL_STATUS_SUCCESS;
	int mode;

	// Check ffcLoop Parameter
	if ((ffcLoop < 0) || (ffcLoop > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Set Info ffcLoop(%d) Parameter Error.(Min:0 / Max:%d)\n", ffcLoop, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	gDpcInfo.dpcCorMode = mode;
	gDpcInfo.dpcPrivate[ffcLoop].frameRate = pDpcParam->frameRate;
	gDpcInfo.dpcPrivate[ffcLoop].expTime = pDpcParam->exposure;
	gDpcInfo.dpcPrivate[ffcLoop].sensorTemp = pDpcParam->sensorTemp;
	gDpcInfo.dpcPrivate[ffcLoop].ffcNo = pDpcParam->ffcNo;

_DONE:
	return (status);
}


//**********************************************************************************
// DPC情報設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcParam			：DPC構造体を格納するポインタ
//		ffcLoop				：テスト回数
//		index				：テスト回数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetInfoDetail (DPC_PARAM *pDpcParam, int ffcLoop, int index)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check ffcLoop Parameter
	if ((ffcLoop < 0) || (ffcLoop > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Set Info Detail ffcLoop(%d) Parameter Error.(Min:0 / Max:%d)\n", ffcLoop, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check index Parameter
	if ((index < 0) || (index > DPC_BRIGHTNESS_LOOP_NUMBER))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Set Info Detail index(%d) Parameter Error.(Min:0 / Max:%d)\n", index, DPC_BRIGHTNESS_LOOP_NUMBER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	gDpcInfo.dpcPrivate[ffcLoop].sd[index] = pDpcParam->sd;
	gDpcInfo.dpcPrivate[ffcLoop].ununiformity[index] = pDpcParam->nonuniformity;

_DONE:
	return (status);
}


//**********************************************************************************
// 欠陥検出パラメータCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcParam			：DPCパラメータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcCheckParam (DPC_PARAM dpcParam)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check dpcNo Parameter
	if ((dpcParam.dpcNo < DPC_NUMBER_MIN) || (dpcParam.dpcNo > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC dpcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcParam.dpcNo, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check ffcNo Parameter
	if ((dpcParam.ffcNo < FFC_NUMBER_MIN) || (dpcParam.ffcNo > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcParam.ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check memType Parameter
	if ((dpcParam.memType != DPC_MEMORY_EXT) && (dpcParam.memType != DPC_MEMORY_INT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC memType(%d) Parameter Error.(EXT:%d / INT:%d)\n", dpcParam.memType, DPC_MEMORY_EXT, DPC_MEMORY_INT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// 欠陥検出パラメータ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pDpcParam			：DPCパラメータ情報ポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetParam (DPC_PARAM *pDpcParam)
{
	int status = AVAL_STATUS_SUCCESS;
	FFCINFO ffcInfo;
#ifdef MODE_PELTIER
	double targetMin, targetMax;
	unsigned int timeout;
	double setTempMin, setTempMax;
	double getTemp;
	int peltierState = MODE_ENABLE;
#endif
	int cmdMode;
	int corMode;
	int startFlag = 0;

	//--------------------------------------------------
	// AcquisitionStop
	//--------------------------------------------------

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// DPC Disable
	//----------------------------------------------------------------------
	if ((status = dpcSetEnableMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//----------------------------------------------------------------------
	// Peltier実装状態確認
	//----------------------------------------------------------------------
#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	if ((status = peltierGetMode (&peltierState)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//----------------------------------------------------------------------
	// Bit幅設定(14bit固定)
	//----------------------------------------------------------------------
	pDpcParam->bit = MODE_DPC_BIT;
	if ((status = aoiSetBitWidth (pDpcParam->bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// FFC Correction Mode取得
	//--------------------------------------------------------------------------------
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
		
	//--------------------------------------------------------------------------------
	// 1次補正
	//--------------------------------------------------------------------------------
	if (corMode == FFC_CORRECTION_MODE_FIRST)
	{
		//----------------------------------------------------------------------
		// FFC Load
		//----------------------------------------------------------------------
		if ((status = ffcLoad (pDpcParam->ffcNo)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//----------------------------------------------------------------------
		// FFC黒レベル情報読み込み
		//----------------------------------------------------------------------
		if ((status = ffcAdjustInfoBlackRead (pDpcParam->ffcNo, FFC_USER, &ffcInfo)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	//--------------------------------------------------------------------------------
	// Shading Line補正
	//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		//----------------------------------------------------------------------
		// FFC Load
		//----------------------------------------------------------------------
		if ((status = ffcLoadSadingLine (pDpcParam->ffcNo)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//----------------------------------------------------------------------
		// FFC黒レベル情報読み込み
		//----------------------------------------------------------------------
		if ((status = ffcAdjustInfoBlackReadShadingLine (pDpcParam->ffcNo, FFC_USER, &ffcInfo)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif


	// FFC情報取得
	pDpcParam->frameRate = ffcInfo.frameRate;
	pDpcParam->sensorTemp = ffcInfo.sensorTemp;
	pDpcParam->exposure = ffcInfo.expTime;

	//----------------------------------------------------------------------
	// Digital Offset Gain
	//----------------------------------------------------------------------

	// Digital Offset Gain保存
	if ((status = dogParamSave ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// デジタルオフセットゲイン設定
	if ((status = dogSetOffsetGain (0, 0, 1.0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//----------------------------------------------------------------------
	// フレームレート
	//----------------------------------------------------------------------
	if ((status = acquisitionSetFrameRate (pDpcParam->frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#ifdef MODE_PELTIER
	//----------------------------------------------------------------------
	// センサ温度設定範囲取得
	//----------------------------------------------------------------------
	if ((status = peltierGetTargetMinMax (&targetMin, &targetMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Temp Parameter
	if ((pDpcParam->sensorTemp < targetMin) || (pDpcParam->sensorTemp > targetMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Sensor Temp(%.2f) Parameter Error.(Min:%.2f / Max:%.2f)\n", pDpcParam->sensorTemp, targetMin, targetMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if (peltierState == MODE_ENABLE)
	{
		sprintf (gLogMsgBuff, "Sensor Target = %.2f\n", pDpcParam->sensorTemp);
		cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

		// 温度設定
		if ((status = peltierSetTarget (pDpcParam->sensorTemp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//--------------------------------------------------------------------------------
		// 指定温度になるまで待つ
		//--------------------------------------------------------------------------------

		// 温度Check
		setTempMin = pDpcParam->sensorTemp - 0.2;
		setTempMax = pDpcParam->sensorTemp + 0.2;
	}

#if defined (MODE_PELTIER_MOUNTING_SWITCH)

	// Peltier実装状態確認
	if(peltierState == MODE_DISABLE)
	{
		setTempMin = pDpcParam->sensorTemp - 15.0;
		setTempMax = pDpcParam->sensorTemp + 15.0;
	}

#endif //defined (MODE_PELTIER_MOUNTING_SWITCH)


MORE:
	for (timeout=0; timeout<FFC_TEMP_TIMEOUT; timeout++)
	{
		// 温度取得
		if ((status = peltierGetSensorTemp (&getTemp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// コマンドラインクリア
		cmdBackspaceLine ();
		sprintf (gLogMsgBuff, "Temp = %.2f  ", getTemp);
		cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		fflush (stdout);

		// 温度は範囲内？
		if ((getTemp >= setTempMin) && (getTemp <= setTempMax))
			break;

		msDelay (1000);
	}

	// Check Timeout
	if (timeout >= FFC_TEMP_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_TIMEOUT);
		sprintf (gLogMsgBuff, "\nDPC Sensor Temp Timeout Error. Target = %.2f / Current = %.2f\n", pDpcParam->sensorTemp, getTemp);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
#endif // MODE_PELTIER

	//--------------------------------------------------
	// 露光時間
	//--------------------------------------------------
	if ((status = acquisitionSetExposure (pDpcParam->exposure)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startFlag == 1)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// 欠陥検出パラメータ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcParam			：DPCパラメータ
//		loopTotal			：Total Count
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcCorrectionMain (DPC_PARAM dpcParam, int loopTotal)
{
	int status = AVAL_STATUS_SUCCESS;
	int index;
	unsigned int size;
#if defined (MODE_DPC_GRID_UPDATE)
	int count;
#endif
	int startFlag = 0;

	//------------------------------------------------------------
	// Stop
	//------------------------------------------------------------
	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// 画像欠陥補正
	//------------------------------------------------------------
	if ((status = lfDpcPxelCorrection (&index, loopTotal)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// 補正データをメモリへ配置
	//------------------------------------------------------------
	dpcSetMemory (dpcParam.dpcNo, pImageDefectionInfo, index);

	// 欠陥画素補正数設定
	OUT32 (FIRM_DATA_DPC_NUM_ADRS, index);

	//------------------------------------------------------------
	// DPC Enable
	//------------------------------------------------------------
	size = DPC_MEMORY_NEW_SIZE;
	dpcRegInit (DPC_MEMORY_ADRS, size, FPGA_DPC_CTRL_ENABLE_BIT);

	//------------------------------------------------------------
	// DPC Map情報取得
	//------------------------------------------------------------
#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map情報更新
	if ((status = dpcGetMapInfo ((unsigned char *)DPC_MEMORY_ADRS, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標登録
	dpcSetDefectionCountVersion2 (count);
#endif

	//------------------------------------------------------------
	// 欠陥不可座標の保存
	//------------------------------------------------------------
	gDpcImpossibleSaveMode = 1;

_DONE:
	if (startFlag == 1)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// 画素欠陥検出メイン
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcParam			：DPCパラメータ
//		loopTotal			：検査回数
//		loopBri				：輝度値変更回数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lfDefectMain (DPC_PARAM *pDpcParam, int loopTotal, int loopBri)
{
	int status = AVAL_STATUS_SUCCESS;
	double sdParam, nonuniformityParam;

	// 標準偏差指定パラメータ使用?
	if (pDpcParam->sd != -1)
	{
		sdParam = pDpcParam->sd;
	}
	else
	{
		sdParam = gSd_thre[(loopTotal%DPC_BRIGHTNESS_LOOP_DEFAULT)];
		pDpcParam->sd = gSd_thre[(loopTotal%DPC_BRIGHTNESS_LOOP_DEFAULT)];
	}

	// 感度不均一 指定パラメータ使用?
	if (pDpcParam->nonuniformity != -1)
	{
		nonuniformityParam = pDpcParam->nonuniformity;
	}
	else
	{
		nonuniformityParam = gNnonuniformity_thre[(loopTotal%DPC_BRIGHTNESS_LOOP_DEFAULT)];
		pDpcParam->nonuniformity = gNnonuniformity_thre[(loopTotal%DPC_BRIGHTNESS_LOOP_DEFAULT)];
	}
	
	//--------------------------------------------------
	// 画像取り込み開始
	//--------------------------------------------------

	// 連続モード取り込み
	acquisitionSetMode (ACQUISITION_MODE_CONTIN);

	// 取り込み開始
	acquisitionStart ();

	// Delay
	msDelay (100);

	sprintf (gLogMsgBuff, "Sd = %f / Nu = %f\n", sdParam, nonuniformityParam);
	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

	// 欠陥検出マップ作成
	if ((status = lfDefect (loopTotal, sdParam, nonuniformityParam)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// 画素欠陥検出
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		loop				：検査回数
//		sd_thre				：標準偏差
//		nonuniformity_thre	：不均一輝度
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lfDefect (int loop, double sd_thre, double nonuniformity_thre)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int index_img;
	double area_average = 0.0;
	int consoleMode = CONSOLE_MODE_OFF;
	int startFlag = 0;

	// Check Loop Parameter
	if (loop > NUM_IMG_FOR_LOOP)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC lfDefect Loop(%d) Error. (Min:0 / Max:%d)\n", loop, NUM_IMG_FOR_LOOP);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// clear
	if ((status = calc_sd_clear ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Console Mode取得
	consoleGetMode (&consoleMode);

	// ROI Area All設定
	if ((status = roiSetDmaFull ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Start
	acquisitionStart ();

	//--------------------------------------------------
	// 画像転送
	//--------------------------------------------------
	for (index_img=0; index_img<NUM_IMG_FOR_DETECT; index_img++)
	{
		DEBUG_PRINT ("Capture Start..%d", index_img);
		fflush (stdout);

		// コマンドラインbackspace
		if (consoleMode == CONSOLE_MODE_ON)
			cmdBackspaceLine ();

		// 画像取得開始
		if ((status = dmaStart ((unsigned char *)pImageSrc)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 取得データを加算
		calc_sd_sum (pImageSrc);
	}

	// 時間方向標準偏差計算
	if ((status = calc_sd2 (pImageSd, pImageAvePix, &area_average)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 輝度不均一性計算
	if ((status = calc_nonuniformity (pImageAvePix, area_average, pImageNonuniformity)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 閾値にて欠陥検出マップ生成
	if ((status = dpc_threshold (pImageSd, pImageNonuniformity, sd_thre, nonuniformity_thre, pImageDetect[loop])) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	// ROI Area Restore設定
	roiSetDmaResutore ();
	dpcSetMode (0);

	if (startFlag == 1)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// 画素欠陥補正
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pIndex				：インデックス数を格納するポインタ
//		loop				：検出回数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lfDpcPxelCorrection (int *pIndex, int loop)
{
	int status = AVAL_STATUS_SUCCESS;
	int index;
	int i;

	// Check pIndex Parameter
	if (pIndex == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Correction pIndex NULL Parameter Error.\n");
		goto _DONE;
	}

	// 全検出結果のマージ
	for (i=1; i<loop; i++)
	{
		if ((status = merge_detect_map (pImageDetect[0], pImageDetect[i], pImageDetect[0])) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// 補正不可データクリア
	//memset ((void *)pDpcImpossibleGrid, 0xff, DPC_IMPOSSIBLE_GRID_SIZE);

	// カウンタクリア
	gDpcImpGridCount = 0;

	// 欠陥補正標準モード
	if ((status = get_correct_coord (pImageDetect[0], pImageDefectionInfo, &index)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// インデックス設定
	*pIndex = index;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC調整データ表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		userMode			：0=User/1=Admin
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcAdjustInfo (int dpcNum, int userMode)
{
	int status;
	DPCINFO dpcInfo;
	int ffcLoop, ffcLoopMax, index;
	int mode;
#if defined (MODE_DPC_ADJUST_MODE)
	int dpcMode;
#endif

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Information dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((userMode != DPC_USER) && (userMode != DPC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Information User(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, DPC_USER, DPC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//--------------------------------------------------
	// DPC情報読み込み
	//--------------------------------------------------
	if ((status = dpcAdjustInfoRead (dpcNum, userMode, &dpcInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------
	// DPC Adjust情報
	//--------------------------------------------------
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC adjust informations]\n");

	//--------------------------------------------------
	// DPC Mode
	//--------------------------------------------------
	mode = dpcInfo.dpcCorMode;
	if ((mode == 0xffffffff) || (mode == 0x00))
		mode = DPC_ADJUST_MODE_NOMAL;
	
	DEBUG_PRINT_FORCE ("   Mode         %d\n", mode);

	for (ffcLoop=0; ffcLoop<DPC_FFC_LOOP_NUMBER; ffcLoop++)
	{
		// 有効なデータはあり?
		if ((dpcInfo.dpcPrivate[ffcLoop].frameRate == 0) || dpcInfo.dpcPrivate[ffcLoop].expTime == 0xffffffff)
			continue;

		DEBUG_PRINT_FORCE ("[Index=%d]--------------------------------------------------\n", ffcLoop);

		//--------------------------------------------------
		// フレームレート
		//--------------------------------------------------
		DEBUG_PRINT_FORCE ("   Frame Rate   %.2f[fps]\n", dpcInfo.dpcPrivate[ffcLoop].frameRate);

		//--------------------------------------------------
		// 露光時間
		//--------------------------------------------------
		DEBUG_PRINT_FORCE ("   Exposure     %d[us]\n", dpcInfo.dpcPrivate[ffcLoop].expTime);

		//--------------------------------------------------
		// 温度
		//--------------------------------------------------
		DEBUG_PRINT_FORCE ("   Temp         %.2f\n", dpcInfo.dpcPrivate[ffcLoop].sensorTemp);

		//--------------------------------------------------
		// FFC番号
		//--------------------------------------------------
		DEBUG_PRINT_FORCE ("   FFC          %d\n", dpcInfo.dpcPrivate[ffcLoop].ffcNo);

		//--------------------------------------------------
		// 標準偏差&不均一輝度値
		//--------------------------------------------------
		for (index=0; index<DPC_BRIGHTNESS_LOOP_NUMBER; index++)
		{
			DEBUG_PRINT_FORCE ("   [%d]",index);

			//--------------------------------------------------
			// 標準偏差
			//--------------------------------------------------
			DEBUG_PRINT_FORCE ("   SD : %6.2f /", dpcInfo.dpcPrivate[ffcLoop].sd[index]);
		
			//--------------------------------------------------
			// 不均一輝度値
			//--------------------------------------------------
			DEBUG_PRINT_FORCE (" Ununiform : %6.2f\n", dpcInfo.dpcPrivate[ffcLoop].ununiformity[index]);
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
// DPC情報保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		userMode			：0=User/1=Admin
//		dpcInfo				：DPC情報
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcAdjustInfoSave (int dpcNum, int userMode, DPCINFO dpcInfo)
{
	int status;
	unsigned int high, low;
	unsigned long long temp64;
	unsigned char *pBuffer = NULL;
	unsigned int saveAdrs;
	unsigned int size = DPC_NUM * DPC_SAVE_INFO_SIZE;
	int ffcLoop, index;
	int ffcLoopMax;
	int initFlag = 0;
	int unitSize;

	// Size
	size = DPC_NUM * DPC_SAVE_INFO_UNIT_SIZE * DPC_FFC_LOOP_NUMBER;
	unitSize = DPC_SAVE_INFO_UNIT_SIZE * DPC_FFC_LOOP_NUMBER;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Information dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((userMode != DPC_USER) && (userMode != DPC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Information User(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, DPC_USER, DPC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリリクエスト
	if ((pBuffer = malloc ((size))) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Adjust Information Buffer Request Error\n");
		goto _DONE;
	}

	// アドレスCopy
	saveAdrs = (unsigned int)pBuffer;
	saveAdrs += (dpcNum * size);

	// DPC情報読み込み
	if (userMode == DPC_USER)
	{
		if ((status = qspiFlashRead (FLASH_DPC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else	// Admin
	{
		if ((status = qspiFlashRead (FLASH_DPC_INFO_ORG_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Loop Max
	for (ffcLoop=0; ffcLoop<DPC_FFC_LOOP_NUMBER; ffcLoop++)
	{
		// 露光時間が設定されていれば保存
		if (dpcInfo.dpcPrivate[ffcLoop].expTime != 0)
		{
			if (initFlag == 0)
			{
				// Clear
				memset ((void *)saveAdrs, 0, unitSize);
				initFlag = 1;
			}

			//------------------------------------------------------------
			// フレームレート保存
			//------------------------------------------------------------
			memcpy (&temp64, &dpcInfo.dpcPrivate[ffcLoop].frameRate, 8);
			high = (unsigned int)((unsigned int)((unsigned long long)temp64>>32)&0xffffffff);
			low  = (unsigned int)(temp64&0xffffffff);

			OUT32 ((saveAdrs + DPC_SAVE_FRAME_H + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop), high);
			OUT32 ((saveAdrs + DPC_SAVE_FRAME_L + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop), low);

			//------------------------------------------------------------
			// 露光時間保存
			//------------------------------------------------------------
			OUT32 ((saveAdrs + DPC_SAVE_EXPOSURE + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop), dpcInfo.dpcPrivate[ffcLoop].expTime);

			//------------------------------------------------------------
			// 温度設定値保存
			//------------------------------------------------------------
			memcpy (&temp64, &dpcInfo.dpcPrivate[ffcLoop].sensorTemp, 8);
			high = (unsigned int)((unsigned int)((unsigned long long)temp64>>32)&0xffffffff);
			low  = (unsigned int)(temp64&0xffffffff);

			OUT32 ((saveAdrs + DPC_SAVE_TEMP_H + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop), high);
			OUT32 ((saveAdrs + DPC_SAVE_TEMP_L + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop), low);

			//------------------------------------------------------------
			// FFC番号保存
			//------------------------------------------------------------
			OUT32 ((saveAdrs + DPC_SAVE_FFC + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop), dpcInfo.dpcPrivate[ffcLoop].ffcNo);

			for (index=0; index<DPC_BRIGHTNESS_LOOP_NUMBER; index++)
			{
				//--------------------------------------------------
				// 標準偏差
				//--------------------------------------------------
				memcpy (&temp64, &dpcInfo.dpcPrivate[ffcLoop].sd[index], 8);
				high = (unsigned int)((unsigned int)((unsigned long long)temp64>>32)&0xffffffff);
				low  = (unsigned int)(temp64&0xffffffff);

				OUT32 ((saveAdrs + DPC_SAVE_SD0_H + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop + index * DPC_SAVE_SD_INTERVAL), high);
				OUT32 ((saveAdrs + DPC_SAVE_SD0_L + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop + index * DPC_SAVE_SD_INTERVAL), low);

				//--------------------------------------------------
				// 不均一輝度値
				//--------------------------------------------------
				memcpy (&temp64, &dpcInfo.dpcPrivate[ffcLoop].ununiformity[index], 8);
				high = (unsigned int)((unsigned int)((unsigned long long)temp64>>32)&0xffffffff);
				low  = (unsigned int)(temp64&0xffffffff);

				OUT32 ((saveAdrs + DPC_SAVE_NONUNIFORM0_H + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop + index * DPC_SAVE_SD_INTERVAL), high);
				OUT32 ((saveAdrs + DPC_SAVE_NONUNIFORM0_L + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop + index * DPC_SAVE_SD_INTERVAL), low);
			}
		}
	}

	// DPCモード設定
	OUT32 ((saveAdrs + DPC_SAVE_CORRECTION_MODE), dpcInfo.dpcCorMode);

	// DPC情報書き込み
	if (userMode == DPC_USER)
	{
		// イレーズ
		if ((status = qspiFlashSectorErase (FLASH_DPC_INFO_ADRS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 書き込み
		if ((status = qspiFlashWrite (FLASH_DPC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else // Admin
	{
		// イレーズ
		if ((status = qspiFlashSectorErase (FLASH_DPC_INFO_ORG_ADRS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 書き込み
		if ((status = qspiFlashWrite (FLASH_DPC_INFO_ORG_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	// メモリ開放
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
//	DPC調整データ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		userMode			：0=User/1=Admin
//		pDpcInfo			：DPC情報を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcAdjustInfoRead (int dpcNum, int userMode, DPCINFO *pDpcInfo)
{
	int status;
	unsigned long long temp64;
	unsigned char *pBuffer = NULL;
	unsigned int saveAdrs;
	unsigned int size = DPC_NUM * DPC_SAVE_INFO_SIZE;
	double temp;
	int ffcLoop, index;
	unsigned int temp32;

	// Size
	size = DPC_NUM * DPC_SAVE_INFO_UNIT_SIZE * DPC_FFC_LOOP_NUMBER;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Information Read dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((userMode != DPC_USER) && (userMode != DPC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Information Read User(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, DPC_USER, DPC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pDpcInfo Parameter
	if (pDpcInfo == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Adjust Information Read pDpcInfo NULL Parameter Error.\n");
		goto _DONE;
	}

	// メモリリクエスト
	if ((pBuffer = malloc ((size))) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Adjust Read Information Buffer Request Error\n");
		goto _DONE;
	}

	// アドレスCopy
	saveAdrs = (unsigned int)pBuffer;
	saveAdrs += dpcNum * DPC_SAVE_INFO_UNIT_SIZE * DPC_FFC_LOOP_NUMBER;

	// DPC情報読み込み
	if (userMode == FFC_USER)
	{
		if ((status = qspiFlashRead (FLASH_DPC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		if ((status = qspiFlashRead (FLASH_DPC_INFO_ORG_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// DPC Mode取得
	temp32 = IN32 ((saveAdrs + DPC_SAVE_CORRECTION_MODE));
	pDpcInfo->dpcCorMode = temp32;

	for (ffcLoop=0; ffcLoop<DPC_FFC_LOOP_NUMBER; ffcLoop++)
	{
		//--------------------------------------------------
		// フレームレート
		//--------------------------------------------------
		temp64 = IN32 ((saveAdrs + DPC_SAVE_FRAME_H + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop));
		temp64 <<= 32;
		temp64 |= IN32 ((saveAdrs + DPC_SAVE_FRAME_L+ DPC_SAVE_INFO_UNIT_SIZE * ffcLoop));
		memcpy (&temp, &temp64, 8);
		pDpcInfo->dpcPrivate[ffcLoop].frameRate = temp;

		//--------------------------------------------------
		// 露光時間
		//--------------------------------------------------
		pDpcInfo->dpcPrivate[ffcLoop].expTime = IN32 ((saveAdrs + DPC_SAVE_EXPOSURE + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop));

		//--------------------------------------------------
		// 温度
		//--------------------------------------------------
		temp64 = IN32 ((saveAdrs + DPC_SAVE_TEMP_H + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop));
		temp64 <<= 32;
		temp64 |= IN32 ((saveAdrs + DPC_SAVE_TEMP_L + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop));
		memcpy (&temp, &temp64, 8);
		pDpcInfo->dpcPrivate[ffcLoop].sensorTemp = temp;

		//------------------------------------------------------------
		// FFC番号保存
		//------------------------------------------------------------
		pDpcInfo->dpcPrivate[ffcLoop].ffcNo = IN32 ((saveAdrs + DPC_SAVE_FFC + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop));

		for (index=0; index<DPC_BRIGHTNESS_LOOP_NUMBER; index++)
		{
			//--------------------------------------------------
			// 標準偏差
			//--------------------------------------------------
			temp64 = IN32 ((saveAdrs + DPC_SAVE_SD0_H + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop + index * DPC_SAVE_SD_INTERVAL));
			temp64 <<= 32;
			temp64 |= IN32 ((saveAdrs + DPC_SAVE_SD0_L + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop + index * DPC_SAVE_SD_INTERVAL));
			memcpy (&temp, &temp64, 8);
			pDpcInfo->dpcPrivate[ffcLoop].sd[index] = temp;

			//--------------------------------------------------
			// 不均一輝度値
			//--------------------------------------------------
			temp64 = IN32 ((saveAdrs + DPC_SAVE_NONUNIFORM0_H + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop + index * DPC_SAVE_SD_INTERVAL));
			temp64 <<= 32;
			temp64 |= IN32 ((saveAdrs + DPC_SAVE_NONUNIFORM0_L + DPC_SAVE_INFO_UNIT_SIZE * ffcLoop + index * DPC_SAVE_SD_INTERVAL));
			memcpy (&temp, &temp64, 8);
			pDpcInfo->dpcPrivate[ffcLoop].ununiformity[index] = temp;
		}
	}

_DONE:
	// メモリ開放
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
// DPC補正不可座標保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		userMode			：0=Uer/1=Admin
//		pBuffer				：補正不可座標を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcImpossibleGridSave (int dpcNum, int userMode, unsigned short *pBuffer)
{
	int status;
	unsigned int *ptr32;
	int size = 0;
	int i;
	unsigned int flashAdrs;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Impossible Grid Save dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((userMode != DPC_USER) && (userMode != DPC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Impossible Grid Save userMode(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, DPC_USER, DPC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Impossible Grid pBuffer NULL Parameter Error.\n");
		goto _DONE;
	}

	// 書き込みサイズCheck
	ptr32 = (unsigned int *)pBuffer;
	for (i=0; i<(DPC_IMPOSSIBLE_GRID_SIZE/4); i++, ptr32++)
	{
		if (*ptr32 == DPC_IMPOSSIBLE_GRID_INVALID_DATA)
			break;
	}

	// DPC欠陥検出不可座標サイズ
	size = i*4;

	// DPC情報書き込み
	if (userMode == DPC_USER)
	{
		// Flashアドレス取得
		if ((status = dpcGetFlashImpossibleAdrs (dpcNum, &flashAdrs)) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;

		// イレーズ
		if ((status = qspiFlashSectorErase (flashAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 書き込み
		if ((status = qspiFlashWrite (flashAdrs, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else // Admin
	{
		// Flashアドレス取得
		if ((status = dpcGetFlashImpossibleAdrsAdmin (dpcNum, &flashAdrs)) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;

		// イレーズ
		if ((status = qspiFlashSectorErase (flashAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 書き込み
		if ((status = qspiFlashWrite (flashAdrs, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC補正不可座標取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		userMode			：0=User/1=Admin
//		pBuffer				：補正不可座標を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcImpossibleGridRead (int dpcNum, int userMode, unsigned short *pBuffer)
{
	int status;
	unsigned int size = DPC_IMPOSSIBLE_GRID_SIZE;
	unsigned int flashAdrs;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Impossible Grid Read dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((userMode != DPC_USER) && (userMode != DPC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Impossible Grid Read userMode(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, DPC_USER, DPC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Impossible Grid Read pBuffer NULL Parameter Error.\n");
		goto _DONE;
	}

	// DPC情報読み込み
	if (userMode == DPC_USER)
	{
		// Flashアドレス取得
		if ((status = dpcGetFlashImpossibleAdrs (dpcNum, &flashAdrs)) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 読み込み
		if ((status = qspiFlashRead (flashAdrs, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// Flashアドレス取得
		if ((status = dpcGetFlashImpossibleAdrsAdmin (dpcNum, &flashAdrs)) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((status = qspiFlashRead (flashAdrs, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Set Error Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		status				：エラーステータス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetError (int status)
{
	// ステータス設定(DPC固有)
	OUT32 (FIRM_DATA_DPC_STATUS_ADRS, status);

	// ステータス設定(共通エラー)
	OUT32 (BOARD_STATUS_DPC_ADRS, status);

	// Cache Flush
	cacheFlushRange (FIRM_DATA_DPC_STATUS_ADRS, 4);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	データに対し閾値をかけ、欠陥検出マップを作成します。
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：検査データ
//		threshold			：閾値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcAdjustThreshold (double *pData, double threshold)
{
	int status = AVAL_STATUS_SUCCESS;
	int x, y;
	int dpcNo = 0;
	unsigned int dpcAdrs;
#if !defined (MODE_IPU_MULTI)
	int xBitAma;
	unsigned int adrs;
#endif
	unsigned char xBit;
	unsigned int dpcCountNew;
	unsigned char data8;
	unsigned int yoffset;
#if defined (MODE_IPU_MULTI)
	int ipu, xIndex;
	int xChange;
	unsigned int offsetTemp;
#endif

	//--------------------------------------------------
	// Check pData Parameter
	//--------------------------------------------------
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Threshold pData NULL Parameter Error.\n");
		goto _DONE;
	}

	//--------------------------------------------------
	// Get Memory Address
	//--------------------------------------------------
	if ((status = dpcGetMemAdrs (dpcNo, &dpcAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// 欠陥検出
	//------------------------------------------------------------
#if !defined (MODE_IPU_MULTI)

	dpcCountNew = 0;
	for (y = 0; y < IMG_HEIGHT; y++)
	{
		// DPC Y Offsetアドレス取得
		yoffset =  dpcAdrs + DPC_WIDTH_DATA_ALIGH * y;

		for (x = 0; x < IMG_WIDTH; x++)
		{
			if (threshold <= pData[x + y * IMG_STRIDE])
			{

				// DPC欠陥座標アドレス
				adrs = x / 8 + yoffset;

				// 欠陥座標Adrs取得
				xBitAma = x % 8;
				xBit = 1 << xBitAma;
				data8 = IN8 (adrs);

				// すでに欠陥登録済み?
				if ((data8 & xBit) != 0)
				{
					// 欠陥座標追加
					data8 &= ~xBit;
					OUT8 (adrs, data8);
					dpcCountNew++;
					
					sprintf (gLogMsgBuff, "x = %d / y = %d / Threshold = %.2f\n",  x, y, pData[x + y * IMG_STRIDE]);
					cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				}
				
				// 欠陥画素オーバー?
				if (dpcCountNew >= NUM_DEFECTION_PIX)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_IO);
					sprintf (gLogMsgBuff, "Defective pixel(%d) over.(Max = %d)\n", dpcCountNew, NUM_DEFECTION_PIX);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}
	}

#else // #if !defined (MODE_IPU_MULTI)

	dpcCountNew = 0;
	for (y = 0; y < IMG_HEIGHT; y++)
	{
		x = 0;
		for (ipu=0; ipu < IPU_COUNT; ipu++)
		{
			// DPC Y Offsetアドレス取得
			yoffset =  dpcAdrs + DPC_WIDTH_DATA_ALIGH * y + DPC_MEMORY_IPU_MULTI_INTERVAL * ipu;

			if (ipu != 0)
				yoffset += (IMG_WIDTH_OFFSET / 8);

			for (xIndex=0; xIndex < IMG_WIDTH / IPU_COUNT; xIndex++, x++)
			{
				// 輝度値取得
				if (threshold <= pData[x + y * IMG_STRIDE])
				{
					// 分割アドレス取得
					if ((status = dpcGetDivAdrs (x, y, &xChange, &offsetTemp)) != AVAL_STATUS_SUCCESS)
						goto _DONE;

					// ビット毎に座標が割り付けられる
					xBit = x % 8;

					// Set DPC Flag(0にすると欠陥)
					data8 = IN8 ((dpcAdrs + offsetTemp));

					// すでに欠陥登録済み?
					if ((data8 & (1<<xBit)) != 0)
					{
						// 欠陥座標追加
						data8 &= ~(1<<xBit);
						OUT8 ((dpcAdrs + offsetTemp), data8);

						sprintf (gLogMsgBuff, "x = %d / y = %d / Threshold = %.2f\n",  x, y,  pData[x + y * IMG_STRIDE]);
						cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
						dpcCountNew++;
					}
					
					// 欠陥画素オーバー?
					if (dpcCountNew >= NUM_DEFECTION_PIX)
					{
						status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
						sprintf (gLogMsgBuff, "Defective pixel(%d) over.(Max = %d)\n", dpcCountNew, NUM_DEFECTION_PIX);
						cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
						goto _DONE;
					}
				}
			}
		}
	}

	// DPCマージン座標の追加
	// ipuの1個目の最後の16画素の追加。
	// ipuの2個目の最初の16画素の追加。
	if ((status = dpcSetMarginGridData()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#endif // #if !defined (MODE_IPU_MULTI)

	//------------------------------------------------------------
	// Mapに追加
	//------------------------------------------------------------
	if ((status = dpcMapAdd (dpcCountNew)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	検出した欠陥をMapに追加します
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcCountNew			：DPCカウント数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcMapAdd (int dpcCountNew)
{
	int status = AVAL_STATUS_SUCCESS;
	int dpcCount;
	int size;
	unsigned int dpcAdrs;
	int dpcNo = 0;
	unsigned int dpcFpgaMode;
#if defined (MODE_DPC_GRID_UPDATE)
	int count;
#endif

	//------------------------------------------------------------
	// 新規追加なし
	//------------------------------------------------------------
	if (dpcCountNew == 0)
		goto _DONE;

	//--------------------------------------------------
	// Get Memory Address
	//--------------------------------------------------
	if ((status = dpcGetMemAdrs (dpcNo, &dpcAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// 欠陥画素補正数取得
	//------------------------------------------------------------
	dpcCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);
	dpcCount += dpcCountNew;

	if (dpcCount >= NUM_DEFECTION_PIX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC Count Over Error =  %d. Max = %d\n", dpcCount, NUM_DEFECTION_PIX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

		// 欠陥画素が多すぎる為、現在Flashに格納されているデータをロード
		if ((status = dpcLoad (dpcNo)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// 欠陥画素補正数設定
		OUT32 (FIRM_DATA_DPC_NUM_ADRS, dpcCount);

		// サイズ取得
		size = DPC_MEMORY_NEW_SIZE;

		dpcFpgaMode = FPGA_DPC_CTRL_ENABLE_BIT;
		dpcRegInit (dpcAdrs, size, dpcFpgaMode);

		//------------------------------------------------------------
		// DPC Map情報更新
		//------------------------------------------------------------
#if defined (MODE_DPC_GRID_UPDATE)
		// DPC Map情報更新
		if ((status = dpcGetMapInfo ((unsigned char *)dpcAdrs, &count)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 欠陥座標登録
		dpcSetDefectionCountVersion2 (count);
#endif
	}

_DONE:
	return (status);
}

// eof
