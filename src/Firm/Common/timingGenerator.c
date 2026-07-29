//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// timingGenerator.c - Timing Generator Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define CAMERA_INTR_ID				(0)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gTimingGeneratorStatus = 0;


//**********************************************************************************
//	Timing Generator Normal初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timingGeneratorNormalInitalize (void)
{
	int status = AVAL_STATUS_SUCCESS;

	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "Timing Generator Initialize\n");

	// 取り込み停止
	acquisitionAbort ();

	// TG UnLock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_UNLOCK);

	// Disable Trg Control
	if ((status = sensorSetTrgControl (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Enable Trg Control
	if ((status = sensorSetTrgControl (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// TG Lock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_LOCK);

	// Acquisition Abort解除
	OUT32 (GENICAM_ACQUISITION_ABORT_ADRS, 0);

_DONE:
	return (status);
}


//**********************************************************************************
//	Timing Generator Version2初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timingGeneratorVersion2Initalize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs, data;

	// TGSE取得
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_TG_VERSION2_TGSE_ADRS, &adrs, &data);

	// TGSE設定
	if (adrs != 0)
	{
		if ((status = tgSetTgse (data)) != AVAL_STATUS_SUCCESS)
			gTimingGeneratorStatus = status;
	}

	// TGES取得
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_TG_VERSION2_TGES_ADRS, &adrs, &data);

	// TGES設定
	if (adrs != 0)
	{
		if ((status = tgSetTges (data)) != AVAL_STATUS_SUCCESS)
			gTimingGeneratorStatus = status;
	}

	// TGPD取得
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_TG_VERSION2_TGPD_ADRS, &adrs, &data);

	// TGPD設定
	if (adrs != 0)
	{
		if ((status = tgSetTgpd (data)) != AVAL_STATUS_SUCCESS)
			gTimingGeneratorStatus = status;
	}

	return (gTimingGeneratorStatus);
}


//**********************************************************************************
//	TGレジスタ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：アドレス
//		data				：データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetReg2 (unsigned int adrs, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	tgCtrlDisable2 ();

	OUT32 (adrs, data);

	tgCtrlEnable2 ();

	return (status);
}


#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
//**********************************************************************************
//	TG Control Enable
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgCtrlEnable2 (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Enable Trg Control
#if !defined (MODE_SENSOR_MASTER)
	int mode;
	mode = IN32 (FIRM_DATA_TG_CTRL_MODE);
	
	if (mode == MODE_ENABLE)
		status = sensorSetTrgControl (MODE_ENABLE);
#endif

	// TG Lock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_LOCK);

	return (status);
}


//**********************************************************************************
//	TG Control Disable
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgCtrlDisable2 (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Disable Trg Control
#if !defined (MODE_SENSOR_MASTER)
	int mode;
	sensorGetTrgControl (&mode);
	OUT32 (FIRM_DATA_TG_CTRL_MODE, mode);
	status = sensorSetTrgControl (MODE_DISABLE);
#endif

	// TG UnLock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_UNLOCK);

	return (status);
}

#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)


//**********************************************************************************
//	TGSE設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：TGSE
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetTgse (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Check data Parameter
	if ((data < FPGA_TG_TGSE_MIN) || (data > FPGA_TG_TGSE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "TGES Set (%d) Parameter Error. (Min:%d / Max:%d)\n", data, FPGA_TG_TGSE_MIN, FPGA_TG_TGSE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set TGSE
	tgSetReg2 (FPGA_TG_TGSE_ADRS, (data & FPGA_TG_TGSE_MASK));

_DONE:
	// Start?
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	TGSE2設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：TGSE
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetTgse2 (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Check data Parameter
	if ((data < FPGA_TG_TGSE_MIN) || (data > FPGA_TG_TGSE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "TGES Set (%d) Parameter Error. (Min:%d / Max:%d)\n", data, FPGA_TG_TGSE_MIN, FPGA_TG_TGSE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// TG UnLock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_UNLOCK);

	// Set TGSE
	OUT32 (FPGA_TG_TGSE_ADRS, (data & FPGA_TG_TGSE_MASK));

	// TG Lock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_LOCK);

_DONE:
	// Start?
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	TGSE取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：TGSEを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgGetTgse (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "TGSE pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Get TGSE
	*pData = IN32 (FPGA_TG_TGSE_ADRS) & FPGA_TG_TGSE_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	TGES設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：TGES
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetTges (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Check data Parameter
	if ((data < FPGA_TG_TGES_MIN) || (data > FPGA_TG_TGES_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "TGES Set (%d) Parameter Error. (Min:%d / Max:%d)\n", data, FPGA_TG_TGES_MIN, FPGA_TG_TGES_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set TGES
	tgSetReg2 (FPGA_TG_TGES_ADRS, (data & FPGA_TG_TGES_MASK));

_DONE:
	// Start?
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	TGES設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：TGES
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetTges2 (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check data Parameter
	if ((data < FPGA_TG_TGES_MIN) || (data > FPGA_TG_TGES_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "TGES Set (%d) Parameter Error. (Min:%d / Max:%d)\n", data, FPGA_TG_TGES_MIN, FPGA_TG_TGES_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// TG UnLock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_UNLOCK);

	// Set TGES
	OUT32 (FPGA_TG_TGES_ADRS, (data & FPGA_TG_TGES_MASK));

	// TG Lock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_LOCK);

_DONE:

	return (status);
}


//**********************************************************************************
//	TGES取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：TGESを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgGetTges (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "TGES pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Get TGES
	*pData = IN32 (FPGA_TG_TGES_ADRS) & FPGA_TG_TGES_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	TGPD設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：TGPD
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetTgpd (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Check data Parameter
	if ((data < FPGA_TG_TGPD_MIN) || (data > FPGA_TG_TGPD_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "TGPD Set (%d) Parameter Error. (Min:%d / Max:%d)\n", data, FPGA_TG_TGPD_MIN, FPGA_TG_TGPD_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set TGPD
	tgSetReg2 (FPGA_TG_TGPD_ADRS, (data & FPGA_TG_TGPD_MASK));

_DONE:
	// Start?
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	TGPD設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：TGPD
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetTgpd2 (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check data Parameter
	if ((data < FPGA_TG_TGPD_MIN) || (data > FPGA_TG_TGPD_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "TGPD Set (%d) Parameter Error. (Min:%d / Max:%d)\n", data, FPGA_TG_TGPD_MIN, FPGA_TG_TGPD_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// TG UnLock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_UNLOCK);

	// Set TGPD
	OUT32 (FPGA_TG_TGPD_ADRS, (data & FPGA_TG_TGPD_MASK));

	// TG Lock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_LOCK);

_DONE:
	return (status);
}


//**********************************************************************************
//	TGPD取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：TGPDを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgGetTgpd (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "TGPD pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Get TGPD
	*pData = IN32 (FPGA_TG_TGPD_ADRS) & FPGA_TG_TGPD_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	予約トリガ使用時の設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetSensorTrgRsvCtrl (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int expTime;
	double rateD;
	unsigned int rateTime;
	unsigned int hIntervalClock;
	double hTimeD, tempD;
	//int tges;
	
#if defined (MODE_SENSOR_SHUTTER)
	int shutterMode;

	// Shutterモード取得
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//  Shutterモードはノーマルモード?
	if (shutterMode == SHUTTER_MODE_NORMAL)
	{
		// 露光時間取得(H単位)
		expTime = 1;
	}
	else
	{
		// 露光時間取得(H単位)
		expTime = IN32 (GENICAM_ACQUISITION_EXPOSURE_ADRS) & GENICAM_ACQUISITION_EXPOSURE_MASK;
	}

#else // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

	// 露光時間取得(H単位)
	expTime = IN32 (GENICAM_ACQUISITION_EXPOSURE_ADRS) & GENICAM_ACQUISITION_EXPOSURE_MASK;

#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

	// TGSE設定
	if ((status = tgSetTgse (expTime)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大レート取得
	if ((status = rateMax (&rateD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// H Interval
	if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// H時間
	hTimeD = (double)hIntervalClock * SENSOR_H_TIME;

	// Rate時間(H単位）
	tempD = (1.0 / rateD) * ACQUISITION_FRAMERATE_UINIT;
	rateTime = tempD / hTimeD;
	rateTime++;		// 切り上げ

	// TGESは16固定(デフォルト値)
#if 0
	// TGES
	tges = rateTime - expTime;
	if (tges <= 0)
		tges = 1;

	// TGES設定
	if ((status = tgSetTges (tges)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

#if defined (MODE_SENSOR_SHUTTER)
	if (shutterMode == SHUTTER_MODE_NORMAL)
		rateTime = 0;
#endif // #if defined (MODE_SENSOR_SHUTTER)

	// TGPD設定
	if ((status = tgSetTgpd (rateTime)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	TG Sensor Trg Default設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetSensorTrgDefault (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int rateMode;
	int totalHeight;
	int tgpd;
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;
#endif
	int tges, tgse;
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int shutterMode = SHUTTER_MODE_TRIGGER;
#endif
#if defined (MODE_SENSOR_DRRS)
	int drrsMode = MODE_DISABLE;
#endif
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	int binningy;
#endif

#if defined (MODE_SENSOR_DRRS)
	// DRRSモード取得
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	tgse = SENSOR_TGSE;

	// TGSE設定
	if ((status = tgSetTgse (tgse)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_FRAMERATE_HIGH_SPEED)

	// Frame Rate High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (hsMode == MODE_ENABLE)
	{
		// Sensor Total Height
		if ((status = fpgaRoiGetCameraHeightTotalSize (&totalHeight)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// TGES設定
		if ((status = sensorGetTgesHighSpeed (totalHeight, (int *)&tges)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		if ((status = tgSetTges (tges)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		if (shutterMode == SHUTTER_MODE_NORMAL)
			tges = 0;
		else
			tges = SENSOR_TGES;

		// TGES設定
		if ((status = tgSetTges (tges)) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

#else // #if defined (MODE_FRAMERATE_HIGH_SPEED)

	#if defined (MODE_SENSOR_DRRS)
	if (drrsMode == MODE_ENABLE)
		tges = SENSOR_TGES_DRRS;
	else
	#endif // #if defined (MODE_SENSOR_DRRS)
		tges = SENSOR_TGES;

	// TGES設定
	if ((status = tgSetTges (tges)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

#endif //#if defined (MODE_FRAMERATE_HIGH_SPEED)

	// フレームレートモード取得
	if ((status = acquisitionGetRateMode (&rateMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (rateMode == MODE_EXTEND)
	{

#if defined (MODE_FRAMERATE_HIGH_SPEED)
		if (hsMode == MODE_ENABLE)
		{
			//--------------------------------------------------------------------------------
			// IMX992/IMX993
			//--------------------------------------------------------------------------------
			#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
			
			// Height取得
			if ((status = aoiGetHeight (&totalHeight)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// BinningY
			if ((status = aoiGetBinningY (&binningy)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			totalHeight *= binningy;

			// Get TGPD
			if ((status = sensorGetTgpdHighSpeed (totalHeight, &tgpd)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			
			#endif // #if defined (MODE_SENSOR_IMX990) || defined (MODE_SENSOR_IMX991)
		}
		else
		{
			// HeightのTotal Sizeを求める
			if ((status = fpgaRoiGetSensorHeightTotalSize (&totalHeight)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// TGPD設定
			tgpd = totalHeight + /*SENSOR_HEIGHT_HIDE_LINE +*/ SENSOR_VINTERVAL_FIX_LINE;
		}

#else // #if defined (MODE_FRAMERATE_HIGH_SPEED)

		// HeightのTotal Sizeを求める
		if ((status = fpgaRoiGetSensorHeightTotalSize (&totalHeight)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// TGPD設定
		tgpd = totalHeight + /*SENSOR_HEIGHT_HIDE_LINE +*/ SENSOR_VINTERVAL_FIX_LINE;

#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)

	}
	else
	{
		tgpd = SENSOR_TGPD;
	}

	// TGPD設定
	if ((status = tgSetTgpd (tgpd)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Trg Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetSensorTrgCheck (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int trgMode, rsvMode;

	// Frameトリガモード取得
	if ((status = acquisitionGetFrameTrgMode (&trgMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 予約トリガモード取得
	if ((status = acquisitionGetTrgReserveMain (&rsvMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Frameトリガモード有効 && 予約トリガ有効?
	if ((trgMode == MODE_ENABLE) && (rsvMode == MODE_ENABLE))
	{
		// Timing Generator調整
		if ((status = tgSetSensorTrgRsvCtrl ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// Timing Generator Default
		if ((status = tgSetSensorTrgDefault ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Trg Check(無効時は設定しない)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tgSetSensorTrgCheck2 (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int trgMode, rsvMode;

	// Frameトリガモード取得
	if ((status = acquisitionGetFrameTrgMode (&trgMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 予約トリガモード取得
	if ((status = acquisitionGetTrgReserveMain (&rsvMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Frameトリガモード有効 && 予約トリガ有効?
	if ((trgMode == MODE_ENABLE) && (rsvMode == MODE_ENABLE))
	{
		// Timing Generator調整
		if ((status = tgSetSensorTrgRsvCtrl ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}

// eof
