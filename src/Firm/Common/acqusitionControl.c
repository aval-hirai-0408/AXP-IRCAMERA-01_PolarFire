//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// acqusitionControl.c - Genicam Acquisition Control Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "sensorVendors.h"
#include "../cxp/cxp.h"

//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gAcquisitionStatus = 0;			// Acquisition Status
int gAcquisitionStartFlag = 0;


//**********************************************************************************
//	Acquisition Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionInit (void)
{
	int status;
	unsigned int adrs, data;

	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "Acquisition Initialize\n");

	// 取り込み停止
	acquisitionAbort ();

	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_ACQUISITION_ADRS, CAMERA_SAVE_ACQUISITION_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gAcquisitionStatus = status;
		goto _DONE;
	}

#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	// Rate Mode
	acquisitionSetRateMode (MODE_EXTEND);
#endif

_DONE:
	return (gAcquisitionStatus);
}


//**********************************************************************************
//	Acquisition Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionInit2 (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
#if defined (MODE_GIGE_10G)
	unsigned int hIntervalClock;
	double hTimeD;
	int bit;
#endif

#if defined (MODE_SENSOR_DRRS)
	int drrsMode = MODE_DISABLE;

	// DRRSモード取得
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (drrsMode == MODE_ENABLE)
		goto _NEXT;
#endif

#if defined (MODE_GIGE_10G)
	//------------------------------------------------------------
	// GEの場合のみフレームレート再設定
	// 10GigEは10GBaseの場合のHintervalで初期化するが、
	// 保存された状態が10GBaseとは限らない為、
	// us単位の時間を取得し再設定する。
	//------------------------------------------------------------
	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		// Bit取得
		if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
			goto _NEXT;

		// H Interval
		if ((status = sensorHIntervalData (bit, &hIntervalClock)) != AVAL_STATUS_SUCCESS)
			goto _NEXT;
		
		// H時間
		hTimeD = (double)hIntervalClock * SENSOR_H_TIME;

		// フレームレート取得
		data32 = IN32 (FIRM_DATA_FRAME_RATE);
		data32 /= hTimeD;
		if (data32 & 0x01)
			data32++;

		// フレームレート設定
		OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, (data32 & GENICAM_ACQUISITION_FRAMERATE_MASK));
		
		// 露光時間取得
		data32 = IN32 (FIRM_DATA_EXPOSURE_TIME);
		data32 /= hTimeD;
		if (data32 & 0x01)
			data32--;

		// 露光時間設定
		OUT32 (GENICAM_ACQUISITION_EXPOSURE_ADRS, (data32 & GENICAM_ACQUISITION_EXPOSURE_MASK));
	}
#endif // #if defined (MODE_GIGE_10G)

#if defined (MODE_SENSOR_DRRS) || defined (MODE_GIGE_10G)
_NEXT:
#endif

	// デフォルトのフレームレート時間を保存
	data32 = IN32 (FIRM_DATA_FRAME_RATE);
	OUT32 (FIRM_DATA_FRAME_RATE_DEFAULT, data32);
	
	// デフォルトの露光時間を保存
	data32 = IN32 (FIRM_DATA_EXPOSURE_TIME);
	OUT32 (FIRM_DATA_EXPOSURE_DEFAULT, data32);

#if defined (MODE_SENSOR_DRRS)
_DONE:
#endif

	return (status);
}


//**********************************************************************************
//	画像取り込みモード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Acquisitionモード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	if (mode == ACQUISITION_MODE_SINGLE)
	{
		// 1画面入力
		data = GENICAM_ACQUISITION_MODE_SINGLE_BIT;
	}
	else if (mode == ACQUISITION_MODE_MULTI)
	{
		// 複数画面入力
		data = GENICAM_ACQUISITION_MODE_MULTI_BIT;
	}
	else if (mode == ACQUISITION_MODE_CONTIN)
	{
		// 無限
		data = GENICAM_ACQUISITION_MODE_CONTIN_BIT;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, ACQUISITION_MODE_MIN, ACQUISITION_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Acquisitionモード設定
	OUT32 (GENICAM_ACQUISITION_MODE_ADRS, (data & GENICAM_ACQUISITION_MODE_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	モード取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Acquisitionモードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Acquisitionモード取得
	*pMode = IN32 (GENICAM_ACQUISITION_MODE_ADRS) & GENICAM_ACQUISITION_MODE_MASK;


_DONE:
	return (status);
}


//**********************************************************************************
//	取り込み開始
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionStart (void)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_TEMP_ABNORMAL_CHECK)
	unsigned int tempStatus;
#endif

#if defined (MODE_TEMP_ABNORMAL_CHECK)
	// Temp Abnormal Status取得
	if ((status = tempGetAbnormalStatus (&tempStatus)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 温度以上の為、何もしない
	if (tempStatus != 0)
		goto _DONE;
#endif

	// 取り込み開始
	OUT32 (GENICAM_ACQUISITION_START_ADRS, GENICAM_ACQUISITION_START_BIT);

	// Set Flag
	OUT32 (FIRM_DATA_ACQUISITION_START_ADRS, 1);

#if defined (MODE_ACQUISITION_TRG_SOFT_COUNT)
	// Acquisition Reset Trg Soft Count
	acquisitionRestTrgSoftCount();
#endif

#if defined (MODE_CXP)
	// Trg Count Reset
	if (gInterFaceID == INTERFACE_CXP)
		acquisitionRestLinkTrgCount ();
#endif

#if defined (MODE_TEMP_ABNORMAL_CHECK)
_DONE:
#endif
	return (status);
}


//**********************************************************************************
//	取り込み停止
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionStop (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// 取り込み停止
	OUT32 (GENICAM_ACQUISITION_STOP_ADRS, GENICAM_ACQUISITION_STOP_BIT);

	// Set Flag
	OUT32 (FIRM_DATA_ACQUISITION_START_ADRS, 0);

	return (status);
}


//**********************************************************************************
//	取り込み中断
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionAbort (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int state1, state2;
	unsigned int i;

	// 取り込み中断
	OUT32 (GENICAM_ACQUISITION_ABORT_ADRS, GENICAM_ACQUISITION_ABORT_BIT);

	// ステータスCheck
	for (i=0; i<ACQUISITION_ABORT_TIMEOUT; i++)
	{
		// Acquisition Trg Waitステータス
		state1 = IN32 (GENICAM_ACQUISITION_TRGWAIT_ADRS);

		// Acquisition Activeステータス
		state2 = IN32 (GENICAM_ACQUISITION_ACTIVE_ADRS);

		// ステータスCheck
		if ((state1 == 0) && (state2 == 0))
			break;

		// 1ms
		msDelay (1);
	}

	// Timeout
	if (i >= ACQUISITION_ABORT_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Abort Timeout Error.\n");
		goto _DONE;
	}

	// 取り込み中断解除
	OUT32 (GENICAM_ACQUISITION_ABORT_ADRS, 0);

	// Set Flag
	OUT32 (FIRM_DATA_ACQUISITION_START_ADRS, 0);

_DONE:
	return (status);
}


//**********************************************************************************
//	Acquisition Start Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：撮像状態を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetStartFlag (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Start Status pMode NULL Parameter Error\n");
		goto _DONE;
	}

	*pMode = IN32 (FIRM_DATA_ACQUISITION_START_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	取り込み枚数設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：取り込み枚数（指定フレーム取り込み時の場合のみ有効）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetFrameCount (unsigned int count)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check count Parameter
	if ((count < GENICAM_ACQUISITION_FRAME_COUNT_MIN) || (count > GENICAM_ACQUISITION_FRAME_COUNT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Set Frame Count(%d) Parameter Error.(Min:%d / Max:%u)\n", count, GENICAM_ACQUISITION_FRAME_COUNT_MIN, GENICAM_ACQUISITION_FRAME_COUNT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 取り込み枚数設定
	OUT32 (GENICAM_ACQUISITION_FRAME_COUNT_ADRS, count);

_DONE:
	return (status);
}


//**********************************************************************************
//	取り込み枚数取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：取り込み枚数（指定フレーム取り込み時の場合のみ有効）を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetFrameCount (unsigned int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Get Frame Count NULL Parameter Error.\n");
		goto _DONE;
	}

	// 取り込み枚数取得
	*pCount = IN32 (GENICAM_ACQUISITION_FRAME_COUNT_ADRS);

_DONE:
	return (status);
}



//**********************************************************************************
//	レート設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		frameRate			：レート時間を格納するポインタ（fps/Hz単位）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetFrameRate (double frameRate)
{
	int status = AVAL_STATUS_SUCCESS;
	int shutterMode = SHUTTER_MODE_TRIGGER;
	int drrsMode = MODE_DISABLE;

	#if defined (MODE_SENSOR_SHUTTER)
	// Shutterモード取得
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	//------------------------------------------------------------
	// Triggerモード時
	//------------------------------------------------------------
	if (shutterMode == SHUTTER_MODE_TRIGGER)
	{
		if ((status = irSetFrameRate (frameRate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// DRRSモード取得
		#if defined (MODE_SENSOR_DRRS)
		if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		#endif
		
		//------------------------------------------------------------
		// DRRSモード時
		//------------------------------------------------------------
		if (drrsMode == MODE_ENABLE)
		{
			#if defined (MODE_SENSOR_DRRS)
			if ((status = irSetFrameRateDrrs (frameRate)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			#endif
		}
	}

_DONE:
	return (status);
}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
//	IRカメラレート設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		frameRate			：レート時間を格納するポインタ（fps/Hz単位）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irSetFrameRate (double frameRate)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int frameRateTime;
	unsigned int frameRateTime2;
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	unsigned int frameRateTime3;
#endif
	unsigned int expTime;
	double rate;
	double rMax;
#if defined (MODE_SENSOR_MASTER)
	unsigned char id;
	int acTmg, regHold;
	unsigned char data8;
	unsigned int vInterval;
#endif
	unsigned int hIntervalClock;
	double intervalD;
	double tempD;
	unsigned int tempTime;
	double hTimeD;
	double frameRateTimeD;
	int rateMode;
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	int readOutTime;
	int expTimeMax, saveExpTimeMax;
	int autoExpMode = MODE_DISABLE;
#endif
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode, heightSize;
#endif
	unsigned int tgpd;
	int startMode = 0;

	// 最大レート取得
	if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレートモード取得
	if ((status = acquisitionGetRateMode (&rateMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 小数点第２位以下切り捨て
	frameRateTime = frameRate * 100;
	frameRate = (double)((double)frameRateTime / (double)100);

	// Check frameRate Parameter
	if ((frameRate < FRAMERATE_MIN) || (frameRate > rMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Frame Rate(%.2f) Parameter Error. (Min:%.2f / Max:%.2f)\n", frameRate, FRAMERATE_MIN, rMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	//auto exposure mode
	autoBrightGetExposureMode(&autoExpMode);

	if (autoExpMode == MODE_ENABLE)
	{
		//自動露光停止
		if ((status = autoBrightSetExposureMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// H Interval
	if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// fps => us時間変換
	rate = (double)(1.0 / frameRate);
	frameRateTime = (unsigned int)(rate * ACQUISITION_FRAMERATE_UINIT);
	frameRateTimeD = rate * ACQUISITION_FRAMERATE_UINIT;

	// H時間
	hTimeD = (double)hIntervalClock * SENSOR_H_TIME;

	// 露光時間取得
	expTime = IN32 (GENICAM_ACQUISITION_EXPOSURE_ADRS) & GENICAM_ACQUISITION_EXPOSURE_MASK;
	expTime *= hTimeD;

	// 設定可能な露光時間を算出
	// TGPD:次のXTRGを受け付けれれない期間
	// したがって、フレームレートはTGPD以上(frameRateTime > TGPD)の間隔でないといけない
	// フレームレートがTGPD以下(frameRateTime < TGPD)だとNG

	// TGPD取得
	if ((status = tgGetTgpd (&tgpd)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	intervalD = tgpd * hTimeD;
	if ((double)frameRateTimeD < (double)intervalD)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Frame Rate(%.2f) Setting Error\nRate Time = %.2f\nInterval = %.2f(TGPD = %d * hInterval = %.2f)\nhClock = %d\n", frameRate, frameRateTimeD, intervalD, tgpd,  hTimeD, hIntervalClock);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 設定可能な露光時間を算出(TGES)
	// TGES:露光完了から次のフレームの開始までの時間(Readout時間)
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	// Frame Rate High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (hsMode == MODE_ENABLE)
	{
		// Height取得
		if ((status = aoiGetHeight (&heightSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (heightSize == 2)
		{
			intervalD = (SENSOR_READOUT_TIME_LNINE2_HIGH_SPEED + 2) * hTimeD;
		}
		else if (heightSize == 4)
		{
			intervalD = (SENSOR_READOUT_TIME_LNINE4_HIGH_SPEED + 2) * hTimeD;
		}
		else
		{
			intervalD = (SENSOR_READOUT_TIME_LNINE6_HIGH_SPEED + 2) * hTimeD;
		}
	}
	else
	{
		intervalD = (SENSOR_TGES + 1) * hTimeD;
	}
#else		
	intervalD = (SENSOR_TGES + 1) * hTimeD;
#endif

	if ((double)((int)frameRateTime - (int)expTime) < (double)intervalD)
	{
		// 露光時間再計算
		expTime = frameRateTime - (unsigned int)intervalD;

		// 1フレームのH単位で割る
		tempD = (double)((double)expTime / hTimeD);
		if (tempD < 1)
			tempD = 1;

#if defined (MODE_FRAMERATE_HIGH_SPEED)
		if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (hsMode == MODE_ENABLE)
			tempD += 0.5;
#endif
		// 露光時間設定
		tempTime  = (unsigned int)tempD;


#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		if (tempTime == 1)
			tempTime++;
		else if (tempTime & 0x01)
			tempTime--;
#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

		OUT32 (GENICAM_ACQUISITION_EXPOSURE_ADRS, (tempTime & GENICAM_ACQUISITION_EXPOSURE_MASK));

		// 露光時間DDRに設定(us単位)
		OUT32 (FIRM_DATA_EXPOSURE_TIME, expTime);
	}

	// フレームレート設定
	frameRateTime2 = frameRateTimeD / hTimeD;	// 1フレームのH単位で割る

#if defined (MODE_FRAMERATE_HIGH_SPEED)
	// フレームレートが速いため、設定する精度を向上
	if (hsMode == MODE_ENABLE)
	{
		frameRateTime2 = (frameRateTimeD / hTimeD + 0.5);	// 1フレームのH単位で割る
	}
#endif

	// 最小値確認
	if (rateMode == MODE_NORMAL)
	{
		if (frameRateTime2 < SENSOR_VINTERVAL_LINE)
			frameRateTime2 = SENSOR_VINTERVAL_LINE;
	}

	#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	if (frameRateTime2 == 0)
		frameRateTime2 = 2;
	else if (frameRateTime2 & 0x01)
		frameRateTime2++;
	#endif

	// フレームレート設定
	OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, (frameRateTime2 & GENICAM_ACQUISITION_FRAMERATE_MASK));

	// フレームレートをDDRへ保存(us単位)
	OUT32 (FIRM_DATA_FRAME_RATE, (frameRateTime2 * hTimeD));
	

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)

	// センサRead Out Time取得
	if ((status = sensoreGetReadOut (&readOutTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 今回のフレームレートで設定できる最大露光時間
	frameRateTime3 = frameRateTime2 * hTimeD;	// 1フレームのH単位で割る

	//expTimeMax = frameRateTime - readOutTime;
	expTimeMax = frameRateTime3 - readOutTime;

	// Get Exposure Max
	if ((status = autoBrightGetExposureMax (&saveExpTimeMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// expTimeMaxは今までの設定値に合わせる
	if (expTimeMax > saveExpTimeMax)
		expTimeMax = saveExpTimeMax;
	
	// Set Exposure Max
	if ((status = autoBrightSetExposureMax (expTimeMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#endif // #if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)


#if defined (MODE_TRG_RESERVED_ADJUST)
	// Timing Generator調整
	if ((status = tgSetSensorTrgCheck2 ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

_DONE:

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	if(autoExpMode == MODE_ENABLE)
		autoBrightSetExposureMode (MODE_ENABLE);
#endif

	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


#if defined (MODE_SENSOR_DRRS)
//**********************************************************************************
//	IRカメラレート設定(DRRS版)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		frameRate			：レート（fps/Hz単位）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irSetFrameRateDrrs (double frameRate)
{
	int status = AVAL_STATUS_SUCCESS;
	double rMax, rMin;
	unsigned int frameRateTime;
	double rate;
	int startMode = 0;
	int drrsMode = MODE_DISABLE;

	// DRRSモード取得
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// 最大レート取得
	if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最小レート取得
	if ((status = rateMin (&rMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 小数点第２位以下切り捨て
	frameRateTime = frameRate * 100;
	frameRate = (double)((double)frameRateTime / (double)100);

	// Check frameRate Parameter
	if ((frameRate < rMin) || (frameRate > rMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Frame Rate(%.2f) Parameter Error. (Min:%.2f / Max:%.2f)\n", frameRate, rMin, rMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DRRS時は2倍
	frameRate *= 2;
	
	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// fps => us時間変換
	rate = (double)(1.0 / frameRate);
	frameRateTime = (unsigned int)(rate * ACQUISITION_FRAMERATE_UINIT);

	// H Interval設定(Shutterモード時はH Intervalでフレームレートが確定)
	if ((status = sensorSetHIntervalTrgNormal (frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレートをDDRへ保存(us単位)
	OUT32 (FIRM_DATA_FRAME_RATE, (frameRateTime * 2));

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}
#endif // #if defined (MODE_SENSOR_DRRS)

#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
//	レート取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFrameRate			：レートを格納するポインタ（fps/Hz単位）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetFrameRate (double *pFrameRate)
{
	int status = AVAL_STATUS_SUCCESS;
	int shutterMode = SHUTTER_MODE_TRIGGER;

	#if defined (MODE_SENSOR_SHUTTER)
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	if (shutterMode == SHUTTER_MODE_TRIGGER)
	{
		if ((status = irGetFrameRate (pFrameRate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		#if defined (MODE_SENSOR_SHUTTER)
		if ((status = irGetFrameRateShutterNormal (pFrameRate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		#endif
	}

_DONE:
	return (status);
}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
//	IRカメラレート取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFrameRate			：レートを格納するポインタ（fps/Hz単位）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irGetFrameRate (double *pFrameRate)
{
	int status;
	double rMax;
	unsigned int rateI;

	// フレームレート取得
	if ((status = irGetFrameRateMain (pFrameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大レート取得
	if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大値Check
	if (*pFrameRate > rMax)
	{
		rateI = (unsigned int)(rMax * 100);
		rMax = (double)((double)rateI / (double)100);
		*pFrameRate = rMax;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	IRカメラレート取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFrameRate			：レートを格納するポインタ（fps/Hz単位）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irGetFrameRateMain (double *pFrameRate)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int frameTime;
	double frameRate;
	unsigned int hIntervalClock;
	double rMax, hTimeD;
	//int rateMode;

	// H Interval
	if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pFrameTime Parameter
	if (pFrameRate == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Frame Rate pFrameRate NULL Parameter Error\n");
		goto _DONE;
	}

	// 最大レート取得
	if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 1Hの時間
	hTimeD = (double)hIntervalClock * SENSOR_H_TIME;

	// フレームレート時間取得
	frameTime = IN32 (GENICAM_ACQUISITION_FRAMERATE_ADRS) & GENICAM_ACQUISITION_FRAMERATE_MASK;
	frameTime *= hTimeD;

	// 時間(us) => fps変換
	frameRate = (double)((double)1.0 / (double)frameTime);
	frameRate *= ACQUISITION_FRAMERATE_UINIT;

	// 小数点第２位以下切り捨て
	frameTime = frameRate * 100;
	frameRate = (double)((double)frameTime / (double)100);

	*pFrameRate = frameRate;

_DONE:
	return (status);
}


//**********************************************************************************
//	IRカメラレート取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFrameRate			：レートを格納するポインタ（fps/Hz単位）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irGetFrameRateShutterNormal (double *pFrameRate)
{
	int status;
	double rMax;
	unsigned int rateI;
#if defined (MODE_SENSOR_DRRS)
	int drrsMode = MODE_DISABLE;
#endif

	// フレームレート取得
	if ((status = irGetFrameRateShutterNormalMain (pFrameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大レート取得
	if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (drrsMode == MODE_ENABLE)
		*pFrameRate /= 2;
	#endif

	// 最大値Check
	if (*pFrameRate > rMax)
	{
		rateI = (unsigned int)(rMax * 100);
		rMax = (double)((double)rateI / (double)100);
		*pFrameRate = rMax;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	IRカメラレート取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFrameRate			：レートを格納するポインタ（fps/Hz単位）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irGetFrameRateShutterNormalMain (double *pFrameRate)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int frameInt;
	double frameRateD;
	unsigned int hInterval;
#if defined (MODE_SENSOR_DRRS)
	int drrsMode = MODE_DISABLE;
	int totalHeight;
#endif
	unsigned int vmax = SENSOR_VINTERVAL_LINE;

	// Check pFrameTime Parameter
	if (pFrameRate == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Frame Rate pFrameRate NULL Parameter Error\n");
		goto _DONE;
	}

	// H Interval
	if ((status = irvGetHIntervalClock (&hInterval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (drrsMode == MODE_ENABLE)
	{
		//vmax = SENSOR_DRRS_VMAX;

		// HeightのTotal Sizeを求める
		if ((status = fpgaRoiGetSensorHeightTotalSize (&totalHeight)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		vmax = totalHeight + SENSOR_DRRS_FIX_LINE;
	}
	
	#endif

	// フレームレート計算
	frameRateD = (double)(((double)(SENSOR_H_CLOCK * 1000 * 1000) / (double)vmax) / (double)hInterval);

	// 小数点第２位以下切り捨て
	frameInt = frameRateD * 100;
	frameRateD = (double)((double)frameInt / (double)100);

	// フレームレート格納
	*pFrameRate = frameRateD;

_DONE:
	return (status);
}

#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
//	Acquisitionステータス取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：ステータスモード
//		pStatus				：ステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetStatus (int mode, int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check mode Parameter
	if ((mode < ACQUISITION_STATUS_MIN) || (mode > ACQUISITION_STATUS_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Status mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, ACQUISITION_STATUS_MIN, ACQUISITION_STATUS_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Status pStatus NULL Parameter Error\n");
		goto _DONE;
	}

	// ステータス選択
	OUT32 (GENICAM_ACQUISITION_STATUS_SELECT_ADRS, mode);

	// ステータス取得
	*pStatus = IN32 (GENICAM_ACQUISITION_STATUS_ADRS) & GENICAM_ACQUISITION_STATUS_SELECT_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	トリガselect設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：トリガselect
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetSelect (int select)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check select Parameter
	if ((status = lfAcquisitionTrgSelectCheck (select)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガSelect設定
	OUT32 (GENICAM_ACQUISITION_TRG_SELECT_ADRS, (select & GENICAM_ACQUISITION_TRG_SELECT_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	トリガselect取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：トリガセレクタを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetSelect (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trg pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// トリガ取得
	*pMode = IN32 (GENICAM_ACQUISITION_TRG_SELECT_ADRS) & GENICAM_ACQUISITION_TRG_SELECT_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Acquisition Trg Select Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：カウンタ番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfAcquisitionTrgSelectCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case ACQUISITION_TRG_SELECT_ACQUISITION_START:
		case ACQUISITION_TRG_SELECT_ACQUISITION_END:
		case ACQUISITION_TRG_SELECT_ACQUISITION_ACTIVE:
		case ACQUISITION_TRG_SELECT_FRAME_START:
		case ACQUISITION_TRG_SELECT_FRAME_END:
		case ACQUISITION_TRG_SELECT_FRAME_ACTIVE:
		case ACQUISITION_TRG_SELECT_EXPOSURE_START:
		case ACQUISITION_TRG_SELECT_EXPOSURE_END:
		case ACQUISITION_TRG_SELECT_EXPOSURE_ACTIVE:
			break;
		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Acquisition Trg Select(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;
	}

	return (status);
}


//**********************************************************************************
//	トリガ有効/無効設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=Disable/1=Enable
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetTrgMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check mode Parameter
	if ((mode != ACQUISITION_TRG_MODE_DISABLE) && (mode != ACQUISITION_TRG_MODE_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Trg Mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, ACQUISITION_TRG_MODE_DISABLE, ACQUISITION_TRG_MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// トリガMode設定
	OUT32 (GENICAM_ACQUISITION_TRG_MODE_ADRS, (mode & GENICAM_ACQUISITION_TRG_MODE_MASK));

#if defined (MODE_TRG_RESERVED_ADJUST)
	// Timing Generator調整
	if ((status = tgSetSensorTrgCheck ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	トリガ有効/無効取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：0=Disable/1=Enableを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetTrgMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trg Mode pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// トリガMode設定
	*pMode = IN32 (GENICAM_ACQUISITION_TRG_MODE_ADRS) & GENICAM_ACQUISITION_TRG_MODE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	ソフトトリガ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetSoftTrg (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// ソフトトリガ設定
	OUT32 (GENICAM_ACQUISITION_SOFT_TRG_ADRS, ACQUISITION_SOFT_TRG_ASSERT);

#if defined (MODE_ACQUISITION_TRG_SOFT_COUNT)
	// Acquisition Set Trg Soft Count
	acquisitionSetTrgSoftCount();
#endif

	return (status);
}


//**********************************************************************************
//	トリガソース設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：トリガソース
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetTrgSource (int mode)
{
	int status = AVAL_STATUS_SUCCESS;;
	unsigned int data;

	// Check mode Parameter
	if ((status = lfTrgSourceCheck (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガソース設定
	OUT32 (GENICAM_ACQUISITION_TRG_SOURCE_ADRS, (mode & GENICAM_ACQUISITION_TRG_SOURCE_MASK));

	// トリガ設定は有効?
	data = IN32 (GENICAM_ACQUISITION_TRG_INVALIDED_ADRS);
	if (data & GENICAM_ACQUISITION_TRG_INVALIDED_BIT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trg Source Invalid Mode Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	トリガソース取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：トリガソース
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetTrgSource (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trg Source pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// トリガ設定は正常?
	data = IN32 (GENICAM_ACQUISITION_TRG_INVALIDED_ADRS);
	if (data & GENICAM_ACQUISITION_TRG_INVALIDED_BIT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trg Source Activation Invalid Mode Error.\n");
		goto _DONE;
	}

	// トリガソース取得
	data = IN32 (GENICAM_ACQUISITION_TRG_SOURCE_ADRS) & GENICAM_ACQUISITION_TRG_SOURCE_MASK;

	// Check mode Parameter
	if ((status = lfTrgSourceCheck (data)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガソース取得
	*pMode = data;

_DONE:
	return (status);
}


//**********************************************************************************
//	トリガソース番号Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number					：トリガソース番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int lfTrgSourceCheck (int number)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (number)
	{
		case GENICAM_ACQUISITION_TRG_SOURCE_SOFT:
		case GENICAM_ACQUISITION_TRG_SOURCE_LINE0:
		case GENICAM_ACQUISITION_TRG_SOURCE_USER0_OUTPUT:
		case GENICAM_ACQUISITION_TRG_SOURCE_LINE1:
		case GENICAM_ACQUISITION_TRG_SOURCE_USER1_OUTPUT:
		case GENICAM_ACQUISITION_TRG_SOURCE_LINE2:
		case GENICAM_ACQUISITION_TRG_SOURCE_USER2_OUTPUT:
		case GENICAM_ACQUISITION_TRG_SOURCE_LINE3:
		case GENICAM_ACQUISITION_TRG_SOURCE_USER3_OUTPUT:
		case GENICAM_ACQUISITION_TRG_SOURCE_LINE4:
		case GENICAM_ACQUISITION_TRG_SOURCE_USER4_OUTPUT:
		case GENICAM_ACQUISITION_TRG_SOURCE_LINE5:
		case GENICAM_ACQUISITION_TRG_SOURCE_USER5_OUTPUT:

		case GENICAM_ACQUISITION_TRG_SOURCE_CC1:
		case GENICAM_ACQUISITION_TRG_SOURCE_CC2:
		case GENICAM_ACQUISITION_TRG_SOURCE_CC3:
		case GENICAM_ACQUISITION_TRG_SOURCE_CC4:

		case GENICAM_ACQUISITION_TRG_SOURCE_COUNTER0_START:
		case GENICAM_ACQUISITION_TRG_SOURCE_COUNTER0_END:
		case GENICAM_ACQUISITION_TRG_SOURCE_COUNTER1_START:
		case GENICAM_ACQUISITION_TRG_SOURCE_COUNTER1_END:
		case GENICAM_ACQUISITION_TRG_SOURCE_COUNTER2_START:
		case GENICAM_ACQUISITION_TRG_SOURCE_COUNTER2_END:
		case GENICAM_ACQUISITION_TRG_SOURCE_COUNTER3_START:
		case GENICAM_ACQUISITION_TRG_SOURCE_COUNTER3_END:
		case GENICAM_ACQUISITION_TRG_SOURCE_TIMER0_START:
		case GENICAM_ACQUISITION_TRG_SOURCE_TIMER0_END:
		case GENICAM_ACQUISITION_TRG_SOURCE_TIMER1_START:
		case GENICAM_ACQUISITION_TRG_SOURCE_TIMER1_END:
		case GENICAM_ACQUISITION_TRG_SOURCE_TIMER2_START:
		case GENICAM_ACQUISITION_TRG_SOURCE_TIMER2_END:
		case GENICAM_ACQUISITION_TRG_SOURCE_TIMER3_START:
		case GENICAM_ACQUISITION_TRG_SOURCE_TIMER3_END:
#if defined (MODE_ENCODER)
		case GENICAM_ACQUISITION_TRG_SOURCE_ENCODER0:
#endif
			break;
		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Acquisition Trg Source Number(%d) Parameter Error.\n", number);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;
	}

	return (status);
}


//**********************************************************************************
//	トリガ論理設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		active				：トリガActive
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetTrgActivation (int active)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check active Parameter
	if ((active < ACQUISITION_TRG_ACTIVATION_MIN) || (active > ACQUISITION_TRG_ACTIVATION_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Trg Activation(%d) Parameter Error. (Min:%d / Max:%d)\n", active, ACQUISITION_TRG_ACTIVATION_MIN, ACQUISITION_TRG_ACTIVATION_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// トリガActivation設定
	OUT32 (GENICAM_ACQUISITION_TRG_ACTIVATION_ADRS, (active & GENICAM_ACQUISITION_TRG_ACTIVATION_MASK));

	// トリガ設定は有効?
	data = IN32 (GENICAM_ACQUISITION_TRG_INVALIDED_ADRS);
	if (data & GENICAM_ACQUISITION_TRG_INVALIDED_BIT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trg Activation Invalid Mode Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	トリガ論理取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pActive				：トリガActiveを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetTrgActivation (int *pActive)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pActive Parameter
	if (pActive == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trg Activation pActive NULL Parameter Error\n");
		goto _DONE;
	}

	// トリガ設定は正常?
	data = IN32 (GENICAM_ACQUISITION_TRG_INVALIDED_ADRS);
	if (data & GENICAM_ACQUISITION_TRG_INVALIDED_BIT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trg Activation Invalid Mode Error.\n");
		goto _DONE;
	}

	// トリガActivation取得
	*pActive = IN32 (GENICAM_ACQUISITION_TRG_ACTIVATION_ADRS) & GENICAM_ACQUISITION_TRG_ACTIVATION_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	トリガDelay設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		delay				：トリガDelay
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetTrgDelay (unsigned  int delay)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check delay Parameter
	if ((delay < GENICAM_ACQUISITION_TRG_DELAY_MIN) || (delay > GENICAM_ACQUISITION_TRG_DELAY_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Trg Delay(%d) Parameter Error. (Min:%d / Max:%d)\n", delay, GENICAM_ACQUISITION_TRG_DELAY_MIN, GENICAM_ACQUISITION_TRG_DELAY_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// トリガdelay設定
	OUT32 (GENICAM_ACQUISITION_TRG_DELAY_ADRS, (delay & GENICAM_ACQUISITION_TRG_DELAY_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	トリガDelay取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pDelay				：トリガDelayを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetTrgDelay (unsigned int *pDelay)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pDelay Parameter
	if (pDelay == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trg Delay pDelay NULL Parameter Error\n");
		goto _DONE;
	}

	// トリガdelay設定
	*pDelay = IN32 (GENICAM_ACQUISITION_TRG_DELAY_ADRS) & GENICAM_ACQUISITION_TRG_DELAY_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	無効トリガ設定取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：トリガソース
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetInvalidedTrg (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Invalid Trg pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// 無効トリガ設定取得
	*pMode = IN32 (GENICAM_ACQUISITION_TRG_INVALIDED_ADRS) & GENICAM_ACQUISITION_TRG_INVALIDED_BIT;

_DONE:
	return (status);
}


//**********************************************************************************
//	露光時間モード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：トリガソース
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetExposureMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	int autoMode;
#endif

	// Check mode Parameter
	if ((mode < ACQUISITION_EXPOSURE_MODE_MIN) || (mode > ACQUISITION_EXPOSURE_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Exposure Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, ACQUISITION_EXPOSURE_MODE_MIN, ACQUISITION_EXPOSURE_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	// Timedでない場合
	if (mode != ACQUISITION_EXPOSURE_TIMED)
	{
		// Auto Bright Mode取得
		if ((status = autoBrightGetExposureMode (&autoMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Auto Bright Mode有効?
		if (autoMode == MODE_ENABLE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Acquisition Exposure Mode(%d) Auto Bright Mode Enable Error.\n", mode);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
#endif

	// Exposure Mode設定
	OUT32 (GENICAM_ACQUISITION_EXPOSURE_MODE_ADRS, (mode & ACQUISITION_EXPOSURE_MODE_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	露光時間モード取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：トリガソース
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetExposureMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Exposure Mode pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Exposure Mode取得
	*pMode = IN32 (GENICAM_ACQUISITION_EXPOSURE_MODE_ADRS) & ACQUISITION_EXPOSURE_MODE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	露光時間設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		expTime				：露光時間(us単位)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetExposure (unsigned int expTime)
{
	int status = AVAL_STATUS_SUCCESS;
	int shutterMode = SHUTTER_MODE_TRIGGER;
	int drrsMode = MODE_DISABLE;

	#if defined (MODE_SENSOR_SHUTTER)
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	//------------------------------------------------------------
	// Triggerモード時
	//------------------------------------------------------------
	if (shutterMode == SHUTTER_MODE_TRIGGER)
	{
		if ((status = irSetExposure (expTime)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		#if defined (MODE_SENSOR_DRRS)
		if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		#endif // #if defined (MODE_SENSOR_DRRS)

		//------------------------------------------------------------
		// DRRSモード時
		//------------------------------------------------------------
		if (drrsMode == MODE_ENABLE)
		{
			#if defined (MODE_SENSOR_DRRS)
			if ((status = irSetExposureDrrs (expTime)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			#endif
		}
	}

_DONE:
	return (status);
}


#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
//**********************************************************************************
//	露光時間設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		expTime				：露光時間(us単位)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetExposureSimple (unsigned int expTime)
{
	int status = AVAL_STATUS_SUCCESS;
	int shutterMode = SHUTTER_MODE_TRIGGER;

	#if defined (MODE_SENSOR_SHUTTER)
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	if (shutterMode == SHUTTER_MODE_TRIGGER)
	{
		if ((status = irSetExposureSimple (expTime)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		#if defined (MODE_SENSOR_SHUTTER)
		if ((status = irSetExposureSimpleShutterNormal (expTime)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		#endif
	}

_DONE:
	return (status);
}
#endif


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
//	露光時間設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		expTime				：露光時間(us単位)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irSetExposure (unsigned int expTime)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int frameRateTime, frameRateTime2;
	double frameRate;
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	int autoExpMode = MODE_DISABLE;
#endif
	unsigned int hIntervalClock;
	int height;
	double intervalD;
	double hTimeD;
	double tempD;
	double expD;
	unsigned int tgpd;
	int startMode = 0;
	int frameFlag = 0;
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	unsigned int tges;
	int hsMode;
#endif
	int expMin, expMax;

	// 最小露光時間取得
	if ((status = exposureMin (&expMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大露光時間取得
	if ((status = exposureMax (&expMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check expTime Parameter
	if ((expTime < expMin) || (expTime > expMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Exposure Time(%d[us]) Parameter Error.(Min:%d / Max:%d)\n", expTime, expMin, expMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}


#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	//auto exposure mode
	if ((status = autoBrightGetExposureMode(&autoExpMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(autoExpMode == MODE_ENABLE)
	{
		//自動露光停止
		if ((status = autoBrightSetExposureMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	msDelay (100);

	// Height
	if ((status = aoiGetHeight (&height)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// H Interval
	if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// H時間
	hTimeD = (double)hIntervalClock * SENSOR_H_TIME;

	// フレームレート取得
	if ((status = acquisitionGetFrameRate (&frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 現在設定されているフレームレートで設定可能な露光時間かをCheck
	tempD = 1.0 / frameRate;
	frameRateTime = (unsigned int)(tempD * ACQUISITION_FRAMERATE_UINIT);


	#if defined (MODE_FRAMERATE_HIGH_SPEED)

	// High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (hsMode == MODE_ENABLE)
	{

		if ((status = tgGetTges (&tges)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		intervalD = tges * hTimeD;
	}
	else
	{
		intervalD = (SENSOR_TGES + 1) * hTimeD;
	}

	#else // #if defined (MODE_FRAMERATE_HIGH_SPEED)

	// 設定可能なフレーム時間を算出(TGES)
	// TGES:露光完了から次のフレームの開始までの時間
	intervalD = (SENSOR_TGES + 1) * hTimeD;

	#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)

	if ((int)((int)frameRateTime - (int)expTime) < intervalD)
	{
		frameRateTime = expTime + intervalD;
		frameFlag = 1;
	}

	// 設定可能な露光時間を算出
	// TGPD:露光完了から次の露光時間の完了までの時間
	
	// TGPD取得
	if ((status = tgGetTgpd (&tgpd)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	intervalD = tgpd * hTimeD;
	if ((double)frameRateTime < (double)intervalD)
	{
		frameRateTime = intervalD;
		frameFlag = 1;
	}

	if (frameFlag == 1)
	{
		// フレームレートをH単位に変換
		frameRateTime2 = frameRateTime / hTimeD + 0.5;

		#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		if (frameRateTime2 == 0)
			frameRateTime2 = 2;
		else if (frameRateTime2 & 0x01)
			frameRateTime2++;
		#endif

		// フレームレート設定
		OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, (frameRateTime2 & GENICAM_ACQUISITION_FRAMERATE_MASK));

		// フレームレートをDDRへ保存(us単位)
		OUT32 (FIRM_DATA_FRAME_RATE, (frameRateTime2 * hTimeD));
	}

	// 露光時間再設定
	expD = (double)((double)expTime / hTimeD);
	expD += 0.5;	// 四捨五入
	expTime = (unsigned int)expD;

	if (expTime == 0)
		expTime = 1;

	#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	if (expTime == 1)
		expTime++;
	else if (expTime & 0x01)
		expTime--;
	#endif

	OUT32 (GENICAM_ACQUISITION_EXPOSURE_ADRS, (expTime & GENICAM_ACQUISITION_EXPOSURE_MASK));

	// 露光時間DDRに設定(us単位)
	OUT32 (FIRM_DATA_EXPOSURE_TIME, (expTime * hTimeD));


#if defined (MODE_TRG_RESERVED_ADJUST)
	// Timing Generator調整
	if ((status = tgSetSensorTrgCheck2 ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

_DONE:

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	if (autoExpMode == MODE_ENABLE)
		autoBrightSetExposureMode (MODE_ENABLE);
#endif

	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


#if defined (MODE_SENSOR_DRRS)
//**********************************************************************************
//	露光時間設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		expTime				：露光時間(us単位)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irSetExposureDrrs (unsigned int expTime)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int hInterval, frameOfLine;
	int expMin, expMax;
	double frameRateD, frameRateD2;
	unsigned int frameRateTime;
	int startMode = 0;

	// 最小露光時間取得
	if ((status = exposureMin (&expMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大露光時間取得
	if ((status = exposureMax (&expMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check expTime Parameter
	if ((expTime < expMin) || (expTime > expMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition DRRS Exposure Time(%d[us]) Parameter Error.(Min:%d / Max:%d)\n", expTime, expMin, expMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// DRRS時はFrameRateレジスタにライン数が格納されている
	frameOfLine = IN32 (GENICAM_ACQUISITION_FRAMERATE_ADRS) & GENICAM_ACQUISITION_FRAMERATE_MASK;

	// 露光時間設定(DRRSのデータシートをP92参照) [式：Integration time = {(1H period * Number of line)}]
	hInterval = (unsigned int)((double)expTime * SENSOR_H_CLOCK / (double)frameOfLine);

	// H Interval設定
	if ((status = sensorSetHIntervalData (hInterval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FrameRate取得(2倍)
	if ((status = irGetFrameRateShutterNormalMain (&frameRateD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	frameRateD2 = (double)(1.0 / frameRateD);
	frameRateTime = (unsigned int)(frameRateD2 * ACQUISITION_FRAMERATE_UINIT);

	// フレームレート保存
	OUT32 (FIRM_DATA_FRAME_RATE, frameRateTime * 2);

	// 露光時間保存
	OUT32 (FIRM_DATA_EXPOSURE_TIME, expTime);
	
_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}
#endif // #if defined (MODE_SENSOR_DRRS)
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S) && defined (MODE_AUTO_EXPOSURE)
//**********************************************************************************
//	露光時間設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		expTime				：露光時間(us単位)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irSetExposureSimple (unsigned int expTime)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int frameRateTime;
	double frameRate;
	unsigned int hIntervalClock;
	double intervalD;
	double hTimeD;
	double tempD;
	double expD;
#if defined (MODE_SENSOR_MASTER)
	unsigned int vInterval;
#endif

	// Check expTime Parameter
	if ((expTime < MIN_EXPOSURE_TIME) || (expTime > MAX_EXPOSURE_TIME))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Exposure Time(%d[us]) Parameter Error.(Min:%d / Max:%d)\n", expTime, MIN_EXPOSURE_TIME, MAX_EXPOSURE_TIME);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// H Interval
	if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// H時間
	hTimeD = (double)hIntervalClock * SENSOR_H_TIME;

	// フレームレート取得
	if ((status = acquisitionGetFrameRate (&frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 現在設定されているフレームレートで設定可能な露光時間かをCheck
	tempD = 1.0 / frameRate;
	frameRateTime = (unsigned int)(tempD * ACQUISITION_FRAMERATE_UINIT);

	// 設定可能なフレーム時間を算出(TGES)
	// TGES:露光完了から次のフレームの開始までの時間
	intervalD = (SENSOR_TGES + 1) * hTimeD;
	if ((int)((int)frameRateTime - (int)expTime) < intervalD)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Exposure Time(%d[us]) Over Error.(Min:%d / Max:%d)\n", expTime, MIN_EXPOSURE_TIME, (int)((int)frameRateTime - (int)expTime));
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 露光時間再設定
	expD = (double)((double)expTime / hTimeD);
	expD += 0.5;	// 四捨五入
	expTime = (unsigned int)expD;

	if (expTime == 0)
		expTime = 1;

	#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	if (expTime == 1)
		expTime++;
	else if (expTime & 0x01)
		expTime--;
	#endif

	OUT32 (GENICAM_ACQUISITION_EXPOSURE_ADRS, (expTime & GENICAM_ACQUISITION_EXPOSURE_MASK));

	// 露光時間DDRに設定(us単位)
	OUT32 (FIRM_DATA_EXPOSURE_TIME, (expTime * hTimeD));

_DONE:
	return (status);
}


#if defined (MODE_SENSOR_SHUTTER)
//**********************************************************************************
//	露光時間設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		expTime				：露光時間(us単位)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irSetExposureSimpleShutterNormal (unsigned int expTime)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int frameRateTime;
	double frameRate;
	unsigned int hInterval;
	double hTimeD;
	double tempD;
	double expD;
	int drrsMode = MODE_DISABLE;

	#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	// Check expTime Parameter
	if ((expTime < MIN_EXPOSURE_TIME) || (expTime > MAX_EXPOSURE_TIME))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Exposure Time(%d[us]) Parameter Error.(Min:%d / Max:%d)\n", expTime, MIN_EXPOSURE_TIME, MAX_EXPOSURE_TIME);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// H Interval
	if ((status = irvGetHIntervalClock (&hInterval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// H時間
	hTimeD = (double)hInterval * SENSOR_H_TIME;

	// フレームレート取得
	if ((status = acquisitionGetFrameRate (&frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 現在設定されているフレームレートで設定可能な露光時間かをCheck
	tempD = 1.0 / frameRate;
	frameRateTime = (unsigned int)(tempD * ACQUISITION_FRAMERATE_UINIT);

	// フレームレートより露光時間の方が大きい？
	if (frameRateTime < expTime)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Exposure Time(%d[us]) Over Error.(Min:%d / Max:%d)\n", expTime, MIN_EXPOSURE_TIME, frameRateTime);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 露光時間再設定
	expD = (double)((double)expTime / hTimeD);
	expD += 0.5;	// 四捨五入
	expTime = (unsigned int)expD;

	if (expTime == 0)
		expTime = 1;

	if (expTime == 1)
		expTime++;
	else if (expTime & 0x01)
		expTime--;

	if (drrsMode == MODE_DISABLE)
	{
		// Set Sensor SHS
		if ((status = sensorSetSHS_H (expTime)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}
#endif // #if defined (MODE_SENSOR_SHUTTER)
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S) || defined (MODE_AUTO_EXPOSURE)


//**********************************************************************************
//	露光時間取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		expTime				：露光時間(us単位)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetExposure (unsigned int *pExpTime)
{
	int status = AVAL_STATUS_SUCCESS;
	int shutterMode = SHUTTER_MODE_TRIGGER;
	int drrsMode = MODE_DISABLE;

	#if defined (MODE_SENSOR_SHUTTER)
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	//------------------------------------------------------------
	// Triggerモード時
	//------------------------------------------------------------
	if (shutterMode == SHUTTER_MODE_TRIGGER)
	{
		if ((status = irGetExposure (pExpTime)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		#if defined (MODE_SENSOR_DRRS)
		if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		#endif // #if defined (MODE_SENSOR_DRRS)

		//------------------------------------------------------------
		// DRRSモード時
		//------------------------------------------------------------
		if (drrsMode == MODE_ENABLE)
		{
			#if defined (MODE_SENSOR_DRRS)
			if ((status = irGetExposureDrrs (pExpTime)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			#endif
		}
	}

_DONE:
	return (status);
}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
//	露光時間取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		expTime				：露光時間(us単位)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irGetExposure (unsigned int *pExpTime)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int expTime;
	unsigned int hIntervalClock;
	double hTimeD;

	// Check pExpTime Parameter
	if (pExpTime == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Exposure pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// H Interval
	if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 1Hの時間
	hTimeD = (double)hIntervalClock * SENSOR_H_TIME;

	// 露光時間取得
	expTime = IN32 (GENICAM_ACQUISITION_EXPOSURE_ADRS) & GENICAM_ACQUISITION_EXPOSURE_MASK;
	expTime = (double)expTime * hTimeD + 0.5;

	if (expTime > MAX_EXPOSURE_TIME)
		*pExpTime = MAX_EXPOSURE_TIME;
	else
		*pExpTime = expTime;

_DONE:
	return (status);
}


#if defined (MODE_SENSOR_DRRS)
//**********************************************************************************
//	露光時間取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		expTime				：露光時間(us単位)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irGetExposureDrrs (unsigned int *pExpTime)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int hIntervalClock, frameOfLine;
	int expTimeMin, expTimeMax;

	// Check pExpTime Parameter
	if (pExpTime == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Exposure pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// H Interval
	if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DRRS時はFrameRateレジスタにライン数が格納されている
	frameOfLine = IN32 (GENICAM_ACQUISITION_FRAMERATE_ADRS) & GENICAM_ACQUISITION_FRAMERATE_MASK;

	// 露光時間算出(DRRSのデータシートをP92参照 [式：Integration time = {(1H period * Number of line)}]
	*pExpTime = (unsigned int)((double)hIntervalClock / SENSOR_H_CLOCK  * (double)frameOfLine);

	// 最小露光時間取得
	if ((status = exposureMin (&expTimeMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;	

	// 最小露光時間Check
	if (*pExpTime < expTimeMin)
		*pExpTime = expTimeMin;

	// 最大露光時間取得
	if ((status = exposureMax (&expTimeMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;	

	// 最大露光時間Check
	if (*pExpTime > expTimeMax)
		*pExpTime = expTimeMax;

_DONE:
	return (status);
}
#endif // #if defined (MODE_SENSOR_DRRS)
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
//	Frame無効Active Trg制御信号取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：Frame無効Active Trgを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetTrgInvalidCount (unsigned int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trigger Invalid Frame Count pCount NULL Parameter Error\n");
		goto _DONE;
	}

	// Trigger Selector取得
	if ((status = acquisitionGetSelect (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Selector
	if (selector != ACQUISITION_TRG_SELECT_FRAME_ACTIVE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	// Frame無効Activeトリガカウント取得
	*pCount = IN32 (GENICAM_ACQUISITION_FRAME_INVALID_ACTIVE_TRG_CNT_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	Frameトリガ予約機能設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Enable/Disable
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetTrgReserve (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	int selector;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Trigger Reserved(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Trigger Selector取得
	if ((status = acquisitionGetSelect (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Selector
	if (selector != ACQUISITION_TRG_SELECT_FRAME_ACTIVE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	// 取得
	data = IN32 (GENICAM_ACQUISITION_TRG_RESERVE_MODE_ADRS);

	if (mode == MODE_ENABLE)
		data |= GENICAM_ACQUISITION_TRG_FRAME_BIT;
	else
		data &= ~GENICAM_ACQUISITION_TRG_FRAME_BIT;

	// 設定
	OUT32 (GENICAM_ACQUISITION_TRG_RESERVE_MODE_ADRS, data);

#if defined (MODE_TRG_RESERVED_ADJUST)
	// Timing Generator調整
	if ((status = tgSetSensorTrgCheck ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	Frameトリガ予約機能取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Enable/Disableを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetTrgReserve (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	int selector;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trigger Reserved pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Trigger Selector取得
	if ((status = acquisitionGetSelect (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Selector
	if (selector != ACQUISITION_TRG_SELECT_FRAME_ACTIVE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	// 取得
	data = IN32 (GENICAM_ACQUISITION_TRG_RESERVE_MODE_ADRS);

	if (data & GENICAM_ACQUISITION_TRG_FRAME_BIT)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Frameトリガ予約機能取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Enable/Disableを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetTrgReserveMain (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Trigger Reserved pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// 取得
	data = IN32 (GENICAM_ACQUISITION_TRG_RESERVE_MODE_ADRS);

	if (data & GENICAM_ACQUISITION_TRG_FRAME_BIT)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Frameトリガ有効／無効取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Enable/Disableを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetFrameTrgMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Frame Trigger Mode pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// 取得
	data = IN32 (GENICAM_FRAME_ACTIVE_MODE_ADRS);

	if (data & GENICAM_ACQUISITION_TRG_MODE_MASK)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Acquisition Reset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionReset (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int expTime;
	double frameRate;
#if defined (MODE_SENSOR_DRRS)
	unsigned int saveFrame, saveExp;
	int drrsMode = MODE_DISABLE;
#endif // #if defined (MODE_SENSOR_DRRS)

#if defined (MODE_SENSOR_DRRS)
	//---------------------------------------------------------------
	// GEt DRRS Mode
	//---------------------------------------------------------------
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif // #if defined (MODE_SENSOR_DRRS)

	//---------------------------------------------------------------
	// Acquisition Stop
	//---------------------------------------------------------------
	acquisitionAbort ();

	//---------------------------------------------------------------
	// 露光時間取得
	//---------------------------------------------------------------
	if ((status = acquisitionGetExposure (&expTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//---------------------------------------------------------------
	// フレームレート取得
	//---------------------------------------------------------------
	if ((status = acquisitionGetFrameRate (&frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//---------------------------------------------------------------
	// パラメータ取得
	//---------------------------------------------------------------
	#if defined (MODE_SENSOR_DRRS)
	if (drrsMode == MODE_ENABLE)
	{
		saveFrame = IN32 (GENICAM_ACQUISITION_FRAMERATE_ADRS);
		saveExp = IN32 (GENICAM_ACQUISITION_EXPOSURE_ADRS);
	}
	#endif // #if defined (MODE_SENSOR_DRRS)

	//---------------------------------------------------------------
	// Reset
	//---------------------------------------------------------------
	OUT32 (GENICAM_ACQUISITION_RESET_ADRS, GENICAM_ACQUISITION_RESET);

	//---------------------------------------------------------------
	// パラメータ設定
	//---------------------------------------------------------------
	#if defined (MODE_SENSOR_DRRS)
	if (drrsMode == MODE_ENABLE)
	{
		OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, saveFrame);
		OUT32 (GENICAM_ACQUISITION_EXPOSURE_ADRS, saveExp);
	}
	#endif // #if defined (MODE_SENSOR_DRRS)

	//---------------------------------------------------------------
	// 露光時間設定
	//---------------------------------------------------------------
	if ((status = acquisitionSetExposure (expTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//---------------------------------------------------------------
	// フレームレート設定
	//---------------------------------------------------------------
	if ((status = acquisitionSetFrameRate (frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//---------------------------------------------------------------
	// Acquisition Start
	//---------------------------------------------------------------
	if (gInterFaceID == INTERFACE_CAMERALINK)
		acquisitionStart ();

_DONE:
	return (status);
}


//**********************************************************************************
//	最小露光時間
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pExp				：最小露光時間(us単位)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int exposureMin (int *pExp)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;
	double hTimeD;
	unsigned int hInterval;
	int shutterMode = SHUTTER_MODE_TRIGGER;
#if defined (MODE_SENSOR_SHUTTER)
	double rMax;
	double expD;
	unsigned int frameOfLine;
	int drrsMode = MODE_DISABLE;
#endif
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	int gcMode;
#endif

	// Check pExp Parameter
	if (pExp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Exposure Min pExp NULL Parameter Error\n");
		goto _DONE;
	}

	// Bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	gcMode = sensorGradationCompGetModeDDR2();

	// Gradation Compress Mode有効?
	if (gcMode == MODE_ENABLE)
		bit = GC_CAMERA_BIT;
	#endif

	#if defined (MODE_SENSOR_SHUTTER)
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	//------------------------------------------------------------
	// Triggerモード時
	//------------------------------------------------------------
	if (shutterMode == SHUTTER_MODE_TRIGGER)
	{
		// H Interval取得
		if ((status = irvGetHIntervalClock (&hInterval)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// H時間
		hTimeD = (double)hInterval * SENSOR_H_TIME;
		*pExp = (int)((double)(hTimeD * 2.0) + 0.5);
	}
	else
	{
	#if defined (MODE_SENSOR_SHUTTER)

		#if defined (MODE_SENSOR_DRRS)
		if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
			return (status);
		#endif // #if defined (MODE_SENSOR_DRRS)

		// 最大レート取得
		if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DRRS時は2倍
		rMax *= 2;
		
		// Get H Interval
		if ((status = sensorHIntervalCalc (rMax, &hInterval)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	
		// H時間
		hTimeD = (double)hInterval * SENSOR_H_TIME;
		
		//------------------------------------------------------------
		// Shutterモード時
		//------------------------------------------------------------
		if (drrsMode == MODE_DISABLE)
		{
			// 最小露光時間
			expD = (double)SENSOR_SHS_MIN * hTimeD;
			*pExp = (int)(expD + 1);
		}
		//------------------------------------------------------------
		// DRRSモード時
		//------------------------------------------------------------
		else
		{
			// DRRS時はFrameRateレジスタにライン数が格納されている
			frameOfLine = IN32 (GENICAM_ACQUISITION_FRAMERATE_ADRS) & GENICAM_ACQUISITION_FRAMERATE_MASK;

			// 露光時間算出(DRRSのデータシートを参照)
			*pExp = (unsigned int)((double)hInterval / SENSOR_H_CLOCK  * (double)frameOfLine);
		}
	#endif // 	#if defined (MODE_SENSOR_SHUTTER)
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	最大露光時間
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pExp				：最大露光時間(us単位)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int exposureMax (int *pExp)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_SENSOR_DRRS)
	double rMin;
	unsigned int hInterval;
	unsigned int frameOfLine;
	int drrsMode = MODE_DISABLE;
#endif

	// Check pExp Parameter
	if (pExp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Exposure Max pExp NULL Parameter Error\n");
		goto _DONE;
	}

#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (drrsMode == MODE_ENABLE)
	{
		// 最小レート取得
		if ((status = rateMin (&rMin)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		rMin*= 2;
		
		// Get H Interval
		if ((status = sensorHIntervalCalc (rMin, &hInterval)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DRRS時はFrameRateレジスタにライン数が格納されている
		frameOfLine = IN32 (GENICAM_ACQUISITION_FRAMERATE_ADRS) & GENICAM_ACQUISITION_FRAMERATE_MASK;

		// 露光時間算出(DRRSのデータシートを参照)
		*pExp = (unsigned int)((double)hInterval / SENSOR_H_CLOCK  * (double)frameOfLine);
	}
	else
	{
		*pExp = MAX_EXPOSURE_TIME;
	}

#else // #if defined (MODE_SENSOR_DRRS)
	
	// 最大露光時間
	*pExp = MAX_EXPOSURE_TIME;

#endif // #if defined (MODE_SENSOR_DRRS)

_DONE:
	return (status);
}


//**********************************************************************************
//	最小レート
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pRate				：最小レートを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int rateMin (double *pRate)
{
#if defined (MODE_SENSOR_DRRS)
	int status;
	int height;
	int drrsMode = MODE_DISABLE;
#endif
	
	// Check pRate Parameter
	if (pRate == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Rate Min pRate NULL Parameter Error\n");
		goto _DONE;
	}

#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最小レート
	if (drrsMode == MODE_ENABLE)
	{
		// Sensor Total Height
		if ((status = fpgaRoiGetSensorHeightTotalSize (&height)) != AVAL_STATUS_SUCCESS)
			return (status);

		*pRate = SENSOR_H_CLOCK * 1000 * 1000 / (height + SENSOR_VINTERVAL_FIX_LINE) / FPGA_SENSOR_HINTERVAL_MASK;
	}
	else
	{
		*pRate = FRAMERATE_MIN;
	}
#else // #if defined (MODE_SENSOR_DRRS)

	// 最小レート
	*pRate = FRAMERATE_MIN;
	
#endif // #if defined (MODE_SENSOR_DRRS)

_DONE:
	return (status);
}


//**********************************************************************************
//	最大レート
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pRate				：最大レートを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
int rateMax (double *pRate)
{
	int status = AVAL_STATUS_SUCCESS;

	if (gInterFaceID == INTERFACE_CXP)
		status = rateGetCxpMax (pRate);
#if defined (MODE_GIGE)
	else if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
		status = rateGetGigEMax (pRate);
#endif
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Rate Max Interface Error\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	最大レート(センサ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pRate				：最大レート(エリア：fps単位/ライン：Hz単位)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int rateGetSensorMax (double *pRate)
{
	int status = AVAL_STATUS_SUCCESS;
	int height;
	unsigned int hIntervalClock;
	int bit;
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	int gcMode;
#endif
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int height2;
	int hsMode;
#endif
	int shutterMode;
#if defined (MODE_SENSOR_DRRS)
	int drrsMode = MODE_DISABLE;
#endif

	// Check pRate Parameter
	if (pRate == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Rate Max pRate NULL Parameter Error\n");
		goto _DONE;
	}

	// bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	gcMode = sensorGradationCompGetModeDDR2();
	
	// Gradation Compress Mode有効?
	if (gcMode == MODE_ENABLE)
		bit = GC_CAMERA_BIT;
#endif

	// Shutterモード取得
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// Shutterモードはトリガモード?
	if (shutterMode == SHUTTER_MODE_TRIGGER)
	{
		// H Interval取得
		if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// H Interval取得
		if ((status = sensorHIntervalData (bit, &hIntervalClock)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Sensor Total Height
	if ((status = fpgaRoiGetSensorHeightTotalSize (&height)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_FRAMERATE_HIGH_SPEED)

	// Frame Rate High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (hsMode == MODE_ENABLE)
	{
		// Sensor Total Height
		if ((status = fpgaRoiGetCameraHeightTotalSize (&height2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// IMX990の場合、アバールデータ様_高速フレームレートモードについて_.pdfのP2参照
		// IMX992の場合、IMX992_AppNote_ROI_High_Speed_Mode_E_Rev1.0.pdfの6.2. XVS input period or VMAX参照
		*pRate = SENSOR_H_CLOCK * 1000 * 1000 / hIntervalClock / (height2 + FRAME_RATE_HIGH_SPEEED_MODE_RATE_MAX_VMAX);
	}
	else
	{
		*pRate = SENSOR_H_CLOCK * 1000 * 1000 / hIntervalClock / (height + SENSOR_VINTERVAL_FIX_LINE);
	}

#else // #if defined (MODE_FRAMERATE_HIGH_SPEED)

	// 最大フレームレート
	*pRate = SENSOR_H_CLOCK * 1000 * 1000 / hIntervalClock / (height + SENSOR_VINTERVAL_FIX_LINE);

#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)


#if defined (MODE_SENSOR_DRRS)
	if (drrsMode == MODE_ENABLE)
	{
		// Hinterval取得
		if ((status = sensorHIntervalDataDrrs (bit, &hIntervalClock)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 最大フレームレート
		*pRate = SENSOR_H_CLOCK * 1000 * 1000 / hIntervalClock / (height + SENSOR_DRRS_FIX_LINE);
		*pRate /= 2;
	}
#endif // #if defined (MODE_SENSOR_DRRS)

_DONE:
	return (status);
}

#if defined (MODE_GIGE)
//**********************************************************************************
//	最大レート(GigE)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pRate				：最大レート(エリア：fps単位/ライン：Hz単位)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int rateGetGigEMax (double *pRate)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;
	double rateD;
#if !defined (MODE_GIGE_10G)
	unsigned int rateI;
#endif
	int width, height;
	int bitByte;
	double rateSensorD, rateGigED;
	int widthDefault, heightDefault;
	double rateGigEDefaultD;
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	int gcMode;
#endif
	float frameTimeFloat;
	double frameTimeDouble;
	double lanBandMaxD;
#if defined (MODE_GIGE_10G)
	int speed;
#endif

	// Check pRate Parameter
	if (pRate == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Rate Max pRate NULL Parameter Error\n");
		goto _DONE;
	}

	// Width Max取得
#if !defined (MODE_GIGE_10G)
	width = WidthMax ();
#else
	if ((status = aoiGetWidth (&width)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif
	
	// bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	gcMode = sensorGradationCompGetModeDDR2();

	// Gradation Compress Mode有効?
	if (gcMode == MODE_ENABLE)
		bit = GC_CAMERA_BIT;
#endif

	// Camera Total Height
	if ((status = fpgaRoiGetCameraHeightTotalSize (&height)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	// カメラ側は8bit
	if (gcMode == MODE_ENABLE)
	{
		bitByte = 1;
	}
	else
	{
		if (bit == 8)
			bitByte = 1;
		else
			bitByte = 2;
	}
#else
	if (bit == 8)
		bitByte = 1;
	else
		bitByte = 2;
#endif

#if !defined (MODE_GIGE_10G)
	lanBandMaxD = GE_NETWORK_BAND_MAX;
#else
	// Get LAN Speed
	if ((status = gigeGetSpeed (&speed)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get LAN帯域最大値
	if ((status = gigeGetLanBandMax (speed, &lanBandMaxD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// 最大フレームレート
	rateGigED = (double)lanBandMaxD / (double)(width * height * bitByte);

	// センサフレームレート取得
	if ((status = rateGetSensorMax (&rateSensorD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// GEの帯域とセンサのフレームレートどちらが早いか？遅いほうにする。
	if (rateSensorD < rateGigED)
		rateD = rateSensorD;
	else
		rateD = rateGigED;

	// 表に出す最大フレームレートは以下で計算
	if (rateSensorD > rateGigED)	// センサのほうが早ければ、遅いGEをMaxにする
	{
		// Width Default 取得
		widthDefault = WidthMax ();

		// Height Default 取得
		heightDefault = HeightMax ();

#if !defined (MODE_GIGE_10G)
		lanBandMaxD = GE_NETWORK_BAND_MAX;
#else
		// Get LAN Speed
		if ((status = gigeGetSpeed (&speed)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get LAN帯域最大値
		if ((status = gigeGetLanBandMax (speed, &lanBandMaxD)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

		// FrameMax = GE帯域 / (height * width * bit) ※12Bitモードで計算
		rateGigEDefaultD = (double)lanBandMaxD / (double)(widthDefault * heightDefault * 2);

#if !defined (MODE_GIGE_10G)
		// 小数点以下を削除
		rateI = (unsigned int)rateGigEDefaultD;
		rateGigEDefaultD = (double)rateI;
#endif // #if !defined (MODE_GIGE_10G)

		// 表に出す最大フレームレート
		rateD = (double)((double)heightDefault / (double)height);
		rateD *= rateGigEDefaultD;

#if defined (MODE_SENSOR_GRADATION_COMPRESS)
		if (gcMode == MODE_ENABLE)
		{
			rateD *= 2;		// カメラ側は8bit
		}
		else
		{
			if (bit == 8)
				rateD *= 2;
		}
#else
		if (bit == 8)
			rateD *= 2;
#endif
	}
	else	// GEのほうが早い。遅いセンサに合わせる。
	{
		rateD = rateSensorD;

#if !defined (MODE_GIGE_10G)
		// 小数点以下を削除
		rateI = (unsigned int)rateD;
		rateD = (double)rateI;
#endif
	}

	*pRate = rateD;

	// フレームレート時間をDDRに格納
	frameTimeDouble = (double)1.0 / rateD;
	frameTimeDouble *= (double)_1US;
	frameTimeFloat = (float)frameTimeDouble;
	OUTF (FIRM_DATA_MAX_FRAME_RATE_TIME, frameTimeFloat);

_DONE:
	return (status);
}
#endif // #if defined (MODE_GIGE)

	
//**********************************************************************************
//	最大レート(Coaxpress)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pRate				：最大レート(エリア：fps単位/ライン：Hz単位)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int rateGetCxpMax (double *pRate)
{
	int status = AVAL_STATUS_SUCCESS;
	int width, height;
	int bit, bitCxp;
	unsigned int cxpRate;
	unsigned int connection;
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	int gcMode;
#endif
	double cxpRateGbps, cxpRateMax;
	double frameTimeDouble;
	float frameTimeFloat;
	double rateSensorD;
	int shutterMode;

	// Check pRate Parameter
	if (pRate == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Rate Max pRate NULL Parameter Error\n");
		goto _DONE;
	}

	// bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	bitCxp = bit;

#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	gcMode = sensorGradationCompGetModeDDR2();

	// Gradation Compress Mode有効?
	if (gcMode == MODE_ENABLE)
		bit = GC_CAMERA_BIT;
#endif

	// Shutterモード取得
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Width Max取得
	width = WidthMax ();

	// Sensor Total Height
	if ((status = fpgaRoiGetSensorHeightTotalSize (&height)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// センサフレームレート取得
	if ((status = rateGetSensorMax (&rateSensorD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get CXP Rate Data
	if ((status = cxpGetRateData (&cxpRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get CXP Rate
	if ((status = cxpGetRegToSpeed (cxpRate, &cxpRateGbps)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get CXP Single or Dual
	if ((status = cxpGetRegToConnection (cxpRate, &connection)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Dual?
	if (connection == CXP_PORT_DUAL)
		cxpRateGbps *= 2;

	// 8b10b実効帯域補正
	cxpRateGbps *= 1000 * 1000 * 1000 /10 * 8;

	// 最大フレームレート
	// FrameMax = CXP帯域 / (height * width * bit)
	cxpRateMax = (double)cxpRateGbps / (double)(width * height * bitCxp);

	// CXPの帯域とセンサのフレームレートどちらが早いか？遅いほうにする。
	if (cxpRateMax > rateSensorD)
	{
		*pRate = rateSensorD;	// センサ側のフレームレート
	}
	else
	{
		*pRate = cxpRateMax;	// CXP側のフレームレート
	}

	// フレームレート時間をDDRに格納
	frameTimeDouble = (double)1.0 / *pRate ;
	frameTimeDouble *= (double)_1US;
	frameTimeFloat = (float)frameTimeDouble;
	OUTF (FIRM_DATA_MAX_FRAME_RATE_TIME, frameTimeFloat);

_DONE:
	return (status);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
//	センサRead Out時間
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTime				：センサRaed Out時間(us単位)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensoreGetReadOut(int *pTime)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_SENSOR_SHUTTER)
	#if !defined (MODE_FRAMERATE_HIGH_SPEED)
	int shutterMode = SHUTTER_MODE_TRIGGER;
	#endif
#endif // 	#if defined (MODE_SENSOR_SHUTTER)
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	unsigned int hIntervalClock;
#endif
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;
	unsigned int tges;
#endif

	// Check pTime Parameter
	if (pTime == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Read Out pTime NULL Parameter Error\n");
		goto _DONE;
	}

	// H Interval
	if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	#if defined (MODE_FRAMERATE_HIGH_SPEED)

	// Frame Rate High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (hsMode == MODE_ENABLE)
	{
		// TGES取得
		if ((status = tgGetTges (&tges)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		*pTime = (hIntervalClock * SENSOR_H_TIME * tges) + 1/*切り上げ*/;
	}
	else
	{
		*pTime = (hIntervalClock * SENSOR_H_TIME * (SENSOR_TGES + 1)/*@*/) + 1/*切り上げ*/;
	}

	#else // #if defined (MODE_FRAMERATE_HIGH_SPEED)

	#if defined (MODE_SENSOR_SHUTTER)
	// Shutterモード取得
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif // #if defined (MODE_SENSOR_SHUTTER)

	// us時間
	if (shutterMode == SHUTTER_MODE_TRIGGER)
		*pTime = (hIntervalClock * SENSOR_H_TIME * (SENSOR_TGES + 1)/*@*/) + 1/*切り上げ*/;
	else
		*pTime = 0;
	
	#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)

_DONE:
	return (status);
}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
//	H Interval取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pClock				：H Interval Clockを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irvGetHIntervalClock (unsigned int *pHinterval)
{
	int status = AVAL_STATUS_SUCCESS;

	status = sensorGetHInterval (pHinterval);

	return (status);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
// Frame Rate Set Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Mode(0:Normal / 1:Extend)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetRateMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	double frameMax;

	// Check mode Parameter
	if ((mode != MODE_NORMAL) && (mode != MODE_EXTEND))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Acquisition Set Rate Mode(%d) Parameter Error.(Normal:%d / Extend:%d)\n", mode, MODE_NORMAL, MODE_EXTEND);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Rate Mode設定
	OUT32 (FIRM_DATA_ACQUISITIO_RATE_MODE_ADRS, mode);

	if (mode == MODE_NORMAL)
	{
		// 最大フレームレート取得
		if ((status = rateMax (&frameMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 最大フレームレート取得
		if ((status = acquisitionSetFrameRate (DEFAULT_FRAMERATE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Rate Get Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Modeを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetRateMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Acquisition Get Rate Mode pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// 取得
	*pMode = IN32 (FIRM_DATA_ACQUISITIO_RATE_MODE_ADRS);

_DONE:
	return (status);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
//	Trg High Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pCount				：Highトリガカウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetTrgCountHigh (int port, unsigned int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector, source;

#if defined (MODE_CXP_MULTI_PORT)
	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Trg Count  port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
#endif

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Trg Count NULL Parameter Error\n");
		goto _DONE;
	}

	// トリガセレクト取得
	if ((status = acquisitionGetSelect (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガソース取得
	if ((status = acquisitionGetTrgSource (&source)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Trg Count初期化
	*pCount = 0;

	if (selector == ACQUISITION_TRG_SELECT_ACQUISITION_START)
	{
	}
	else if (selector == ACQUISITION_TRG_SELECT_ACQUISITION_END)
	{
	}
	else if (selector == ACQUISITION_TRG_SELECT_ACQUISITION_ACTIVE)
	{
	}
	else if (selector == ACQUISITION_TRG_SELECT_LINE_START)
	{
	}
	else if (selector == ACQUISITION_TRG_SELECT_FRAME_ACTIVE)
	{
		if (source == GENICAM_ACQUISITION_TRG_SOURCE_SOFT)
		{
			*pCount = IN32 (FIRM_DATA_TRG_SOFT_COUNT_ADRS);
		}
		else if (source == GENICAM_ACQUISITION_TRG_SOURCE_CC1)
		{
			// Trg High Count取得
			*pCount = IN32 ((FPGA_CXP_TRG_H_COUNT_ADRS + FPGA_CXP_REGISTER_PORT_INTERVAL * port));
		}
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Trg Count Selector(%d) Parameter Error\n", selector);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Trg Low Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pCount				：Lowトリガカウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionGetTrgCountLow (int port, unsigned int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector, source;

#if defined (MODE_CXP_MULTI_PORT)
	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Trg Count  port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
#endif

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Trg Count NULL Parameter Error\n");
		goto _DONE;
	}

	// トリガセレクト取得
	if ((status = acquisitionGetSelect (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガソース取得
	if ((status = acquisitionGetTrgSource (&source)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Trg Count初期化
	*pCount = 0;

	if (selector == ACQUISITION_TRG_SELECT_ACQUISITION_START)
	{
	}
	else if (selector == ACQUISITION_TRG_SELECT_ACQUISITION_END)
	{
	}
	else if (selector == ACQUISITION_TRG_SELECT_ACQUISITION_ACTIVE)
	{
	}
	else if (selector == ACQUISITION_TRG_SELECT_LINE_START)
	{
	}
	else if (selector == ACQUISITION_TRG_SELECT_FRAME_ACTIVE)
	{
		if (source == GENICAM_ACQUISITION_TRG_SOURCE_SOFT)
		{
			*pCount = IN32 (FIRM_DATA_TRG_SOFT_COUNT_ADRS);
		}
		else if (source == GENICAM_ACQUISITION_TRG_SOURCE_CC1)
		{
			// Trg Low Count取得
			*pCount = IN32 ((FPGA_CXP_TRG_L_COUNT_ADRS + FPGA_CXP_REGISTER_PORT_INTERVAL * port));
		}
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Trg Count Selector(%d) Parameter Error\n", selector);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Trg Soft Count Reset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionRestTrgSoftCount (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Trg Count Reset
	OUT32 (FIRM_DATA_TRG_SOFT_COUNT_ADRS, 0);

	return (status);
}


//**********************************************************************************
//	Trg Soft Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionSetTrgSoftCount (void)
{
	unsigned int data;

	// Trg Count Reset
	data = IN32 (FIRM_DATA_TRG_SOFT_COUNT_ADRS);
	data++;
	OUT32 (FIRM_DATA_TRG_SOFT_COUNT_ADRS, data);

	return (AVAL_STATUS_SUCCESS);
}


#if defined (MODE_CXP)
//**********************************************************************************
//	Trg Link Reset Count Reset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionRestLinkTrgCount (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Trg Count Reset
#if !defined (MODE_CXP_MULTI_PORT)
	OUT32 (FPGA_CXP_TRG_CTRL_ADRS, FPGA_CXP_TRG_CTRL_RESET);
#else
	int cxpPort;

	// CXP Port取得
	if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	OUT32 ((FPGA_CXP_TRG_CTRL_ADRS + FPGA_CXP_REGISTER_PORT_INTERVAL * cxpPort), FPGA_CXP_TRG_CTRL_RESET);

_DONE:
#endif

	return (status);
}
#endif

// eof
