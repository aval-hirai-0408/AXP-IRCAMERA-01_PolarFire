//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// aoi.c - AOI Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "../Common/sensorVendorS.h"
#include "../GigE/gige.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gAoiStatus = 0;		// AOI Status


//**********************************************************************************
// AOI初期化設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiInitialize (void)
{
	int status;
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	unsigned int data32;
#endif

	DEBUG_PRINT ("AOI Initialize\n");

#if !defined (MODE_ROI_VERSION2)
	// AOI Disable
	aoiSetUpdate (AOI_DISABLE);
#endif
	
	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_AOI_ADRS, CAMERA_SAVE_AOI_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gAoiStatus = status;
		return (status);
	}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	// Bit
	data32 = IN32 (FPGA_AOI_BITWIDTH_ADRS);
	OUT32 (FIRM_DATA_PIXEL_FORMAT, data32);
#endif


#if defined (MODE_IPU_MULTI)
	// Register Data Restore
	if ((status = cameraParamWriteRegisterOffsetAdrs (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_AOI_BIT_ADRS, 0x08, FPGA_AOI2_OFFSET)) != AVAL_STATUS_SUCCESS)
	{
		gAoiStatus = status;
		return (status);
	}
#endif


#if !defined (MODE_ROI_VERSION2)
	// 更新処理実行
	if ((status = aoiSetUpdate (AOI_ENABLE)) != AVAL_STATUS_SUCCESS)
	{
		gAoiStatus = status;
		return (status);
	}
#endif

#if defined (MODE_BINNING)
	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_BINING_ADRS, CAMERA_BINING_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gAoiStatus = status;
		return (status);
	}
#endif


	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
// AOI Update設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：AOI更新設定(0=無効/1=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetUpdate (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check mode Parameter
	if ((mode != AOI_ENABLE) && (mode != AOI_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "AOI Update mode(%d) Parameter Error. (Disable=%d / Enable=%d)\n", mode, AOI_DISABLE, AOI_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// AOI取得
	data = IN32 (FPGA_AOI_UPDATE_ADRS);

	if (mode == AOI_ENABLE)
		data |= FPGA_AOI_UPDATE_ENABLE_BIT;
	else
		data &= ~FPGA_AOI_UPDATE_ENABLE_BIT;
	
	// AOI設定
	OUT32 (FPGA_AOI_UPDATE_ADRS, data);

_DONE:
	return (status);
}


//**********************************************************************************
// AOI Update取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：AOI更新情報を格納するポインタ(0=無効/1=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetUpdate (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "AOI Update pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Update取得
	data = IN32 (FPGA_AOI_UPDATE_ADRS);
	if (data & FPGA_AOI_UPDATE_ENABLE_BIT)
		*pMode = AOI_ENABLE;
	else
		*pMode = AOI_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
// Bit Width設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bit					：Bit Width(8=8bit/10=10bit/12=12bit/14=14bit)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetBitWidth (int bit)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	int autoGainMode, autoExpMode;
#endif //defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)


#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	// 自動露光が実行されていたら一回停止
	if ((status = autoBrightGetExposureMode (&autoExpMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (autoExpMode == MODE_ENABLE)
	{
		if ((status = autoBrightSetExposureMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// 自動露ゲインが実行されていたら一回停止
	if ((status = autoBrightGetGainMode (&autoGainMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

	if (autoGainMode == MODE_ENABLE)
	{
		if ((status = autoBrightSetGainMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif //defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)


	if ((status = irSetBitWidth (bit) != AVAL_STATUS_SUCCESS))
		goto _DONE;


#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	//イネーブルなら再開
	if (autoExpMode == MODE_ENABLE)
	{
		if ((status = autoBrightSetExposureMode (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//イネーブルなら再開
	if (autoGainMode == MODE_ENABLE)
	{
		if ((status = autoBrightSetGainMode (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif //defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	
_DONE:
	return (status);
}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
// IRVSカメラBit Width設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bit					：Bit Width(8=8bit/10=10bit/12=12bit)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irSetBitWidth (int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char adc, output;
	unsigned short black;
	unsigned char id;
	unsigned char data8;
	double rMax, rMin, frameRate;
	unsigned int fpgaBit;
	unsigned int expTime;
	int startMode = 0;
	int expMin, expMax;
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	int gcModeCurrent, gcModeReqest = 0;
#endif
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;
#endif
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	int hIntervalBit;
	int shutterMode;
	unsigned int hIntervalClock;
	int expTimeH;
	double hTimeD, expD;
	int drrsMode = MODE_DISABLE;
#endif
	int cxpPort = 0;

	if (bit > MODE_CAMERA_BIT)
	{
		goto _DONE_ERROR;
	}

#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	if (bit == 8)
	{
		//--------------------------------------------------------------------------------
		// 8bit→8bit時のgcmodeの一貫性（gcmode有効時は有効のまま。無効時は無効のまま。）
		//--------------------------------------------------------------------------------

		// GC Mode取得
#if defined (MODE_BIT8_GCMODE_ENABLE)
		gcModeCurrent = IN32 (FIRM_DATA_BIT8_CONVERT_MODE);
		if (gcModeCurrent == MODE_ENABLE)
		{
			bit = 0;		// Gradation Compress機能を有効な設定にする
			goto _NEXT;
		}
#else
		gcModeCurrent = IN32 (FIRM_DATA_GRADATION_COMPRESS_MODE_PRE);
		if (gcModeCurrent == MODE_ENABLE)
		{
			bit = 0;		// Gradation Compress機能を有効な設定にする
			goto _NEXT;
		}
#endif
	}

_NEXT:
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	if (bit == 0)
	{
		// Get High Speed Mode
		if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (hsMode == MODE_ENABLE)
			bit = 8;
	}
#endif

	// Check Bit Parameter
	if (bit == 0)
	{
		fpgaBit = FPGA_AOI_BITWIDTH_8BIT;
		adc = GC_SENSOR_AD_BIT;
		output = SENSOR_REG_OD_8BIT;
		black = SENSOR_REG_BLACKLEVEL_10BIT;
		gcModeReqest = 1;
		bit = 8;
		hIntervalBit = 10;
	}
	else
#endif
	// Check Bit Parameter
	if (bit == 8)
	{
		fpgaBit = FPGA_AOI_BITWIDTH_8BIT;
		adc = SENSOR_REG_AD_8BIT;
		output = SENSOR_REG_OD_8BIT;
		black = SENSOR_REG_BLACKLEVEL_8BIT;
		hIntervalBit = 8;
	}
	else if (bit == 10)
	{
		fpgaBit = FPGA_AOI_BITWIDTH_10BIT;
		adc = SENSOR_REG_AD_10BIT;
		output = SENSOR_REG_OD_10BIT;
		black = SENSOR_REG_BLACKLEVEL_10BIT;
		hIntervalBit = 10;
	}
	else if (bit == 12)
	{
		fpgaBit = FPGA_AOI_BITWIDTH_12BIT;
		adc = SENSOR_REG_AD_12BIT;
		output = SENSOR_REG_OD_12BIT;
		black = SENSOR_REG_BLACKLEVEL_12BIT;
		hIntervalBit = 12;
	}
	else
	{
_DONE_ERROR:
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Bit(%d) Parameter Error.", bit);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード移行
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレート取得
	if ((status = acquisitionGetFrameRate (&frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 露光時間取得
	if ((status = acquisitionGetExposure (&expTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set Bit Width
	OUT32 (FPGA_AOI_BITWIDTH_ADRS, fpgaBit);
	#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_AOI2_BITWIDTH_ADRS, fpgaBit);
	#endif

	// Shutterモード取得
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Shutterモードはトリガモード?
	if (shutterMode == SHUTTER_MODE_TRIGGER)
	{
		if ((status = sensorSetHInterval (hIntervalBit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Sensor ADC
	id = 0x04;
	if ((status = sensorRegReadByte (id, SENSOR_REG_AD_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 &= ~SENSOR_REG_AD_MASK;
	data8 |= adc;

	if ((status = sensorRegWriteByte (id, SENSOR_REG_AD_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor ODBit
	id = 0x06;
	if ((status = sensorRegReadByte (id, SENSOR_REG_ODBIT_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 &= ~SENSOR_REG_OD_BIT_MASK;
	data8 |= output;

	if ((status = sensorRegWriteByte (id, SENSOR_REG_ODBIT_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor Black1
	id = 0x07;
	data8 = (unsigned char)(black & SENSOR_REG_BLACKLEVEL1_MASK);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_BLACKLEVEL1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor Black2
	id = 0x07;
	data8 = (unsigned char)((black >> 8) & SENSOR_REG_BLACKLEVEL2_MASK);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_BLACKLEVEL2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 隠しレジスタ設定
	if ((status = sensorBitSetHide (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (drrsMode == MODE_ENABLE)
	{
		// DRRS隠しレジスタ設定
		if ((status = sensorVendorSDrrsHideRegBit (bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	//------------------------------------------------------------
	// Gradation Compress Mode
	//------------------------------------------------------------
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	// Gradation Compress Mode取得
	gcModeCurrent = sensorGradationCompGetModeDDR ();

	// Gradation Compress Mode要求あり?
	if (gcModeReqest != 0)
	{
		if ((status = sensorGradationCompSetModeMain2 (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		OUT32(FIRM_DATA_GRADATION_COMPRESS_MODE_PRE, MODE_ENABLE);
	}
	else if (gcModeCurrent != 0)
	{
		if ((status = sensorGradationCompSetModeMain2 (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		OUT32(FIRM_DATA_GRADATION_COMPRESS_MODE_PRE, MODE_DISABLE);
	}
#endif

	// 最大レート取得
	if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最小レート取得
	if ((status = rateMin (&rMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Max Frame Rate
	if (frameRate > rMax)
		frameRate = rMax;

	// Check Min Frame Rate
	if (frameRate < rMin)
		frameRate = rMin;

	// 最大レート取得
	if ((status = exposureMax (&expMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最小レート取得
	if ((status = exposureMin (&expMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Max Exposure Time
	if (expTime > expMax)
		expTime = expMax;

	// Check Min Exposure Time
	if (expTime < expMin)
		expTime = expMin;

	// 露光時間設定
	if ((status = acquisitionSetExposure (expTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレート設定
	if ((status = acquisitionSetFrameRate (frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_TRG_RESERVED_ADJUST)
	// Timing Generator調整
	if ((status = tgSetSensorTrgCheck2 ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// 起動時は設定しない
		if (IN32 (FIRM_DATA_FIRM_BOOT_FLAG_ADRS) != 0)
		{
			if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			if ((status = cxpSetBit (cxpPort, bit)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}
#endif

#if defined (MODE_SENSOR_DRRS)
	if (drrsMode == MODE_ENABLE)
	{
		// Set Black Level(FPGAのレジスタ)
		if ((status = sensorDrrsSetBlacklevel ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	// StandByモード解除
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	OUT32 (FIRM_DATA_PIXEL_FORMAT, fpgaBit);
#endif

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
// Bit Width取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pBit				：Bit Widthを格納するポインタ(8=8bit/10=10bit/12=12bit/14=14bit)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetBitWidth (int *pBit)
{
	int status = AVAL_STATUS_SUCCESS;

	if ((status = irGetBitWidth (pBit) != AVAL_STATUS_SUCCESS))
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// IRカメラBit Width取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pBit				：Bit Widthを格納するポインタ(8=8bit/10=10bit/12=12bit/14=14bit)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int irGetBitWidth (int *pBit)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pBit Parameter
	if (pBit == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Bit pBit NULL Parameter Error\n");
		goto _DONE;
	}

	data = IN32 (FPGA_AOI_BITWIDTH_ADRS) & FPGA_AOI_BITWIDTH_MASK;

	if (data == FPGA_AOI_BITWIDTH_8BIT)
	{
		*pBit = 8;
	}
	else if (data == FPGA_AOI_BITWIDTH_10BIT)
	{
		*pBit = 10;
	}
	else if (data == FPGA_AOI_BITWIDTH_12BIT)
	{
		*pBit = 12;
	}
	else if (data == FPGA_AOI_BITWIDTH_14BIT)
	{
		*pBit = 14;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Bit data(0x%x) Parameter Error. mode = %d\n", data);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
// Bit Width取得(GigE版)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pBit				：Bit Widthを格納するポインタ(8=8bit/10=10bit/12=12bit/14=14bit)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraGetBitWidthGigE (int *pBit)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pBit Parameter
	if (pBit == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Bit pBit NULL Parameter Error\n");
		goto _DONE;
	}

	data = IN32 (FPGA_AOI_BITWIDTH_ADRS) & FPGA_AOI_BITWIDTH_MASK;

	if (data == FPGA_AOI_BITWIDTH_8BIT)
		*pBit = GVSP_PIX_MONO8;
	else if (data == FPGA_AOI_BITWIDTH_10BIT)
		*pBit = GVSP_PIX_MONO10;
	else if (data == FPGA_AOI_BITWIDTH_12BIT)
		*pBit = GVSP_PIX_MONO12;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Bit data(0x%x) Parameter Error. mode = %d\n", data);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
// AOI PAD設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：Xパディングサイズ
//		y					：Yパディングサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetPad (int x, int y)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check x Parameter
	if (x > WidthMax())
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "AOI Pad X(%d) Parameter Error. (Min:0 / Max:%d)\n", x, WidthMax());
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check y Parameter
	if (y > HeightMax())
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "AOI Pad Y(%d) Parameter Error. (Min:0 / Max:%d)\n", y, HeightMax());
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Set X Padding
	OUT32 (FPGA_AOI_XPAD_ADRS, (x & FPGA_AOI_XPAD_MASK));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_AOI2_XPAD_ADRS, (x & FPGA_AOI_XPAD_MASK));
#endif

	// Set Y Padding
	OUT32 (FPGA_AOI_YPAD_ADRS, (y & FPGA_AOI_YPAD_MASK));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_AOI2_YPAD_ADRS, (y & FPGA_AOI_XPAD_MASK));
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// AOI XY PAD取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pX					：Xパディングサイズを格納するポインタ
//		pY					：Yパディングサイズを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetPad (int *pX, int *pY)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pX Parameter
	if (pX == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "AOI Pad pX NULL Parameter Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pY Parameter
	if (pY == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "AOI Pad pY NULL Parameter Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	*pX = IN32 (FPGA_AOI_XPAD_ADRS) & FPGA_AOI_XPAD_MASK;
	*pY = IN32 (FPGA_AOI_YPAD_ADRS) & FPGA_AOI_YPAD_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	テストパターンIndex設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：テストパターンインデックス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetPattern (int index)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Check index Parameter
	if ((index < AOI_TP_MIN_AREA) || (index > AOI_TP_MAX_AREA))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Test Pattern(%d) Parameter Error. (Min:%d / Max:%d)\n", index, AOI_TP_MIN_AREA, AOI_TP_MAX_AREA);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set Index
	OUT32 (FPGA_AOI_TP_INDEX_ADRS, (index & FPGA_AOI_TP_MASK));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_AOI2_TP_INDEX_ADRS, (index & FPGA_AOI_TP_MASK));
#endif

	if (startMode != 0)
		acquisitionStart ();

_DONE:
	return (status);
}


//**********************************************************************************
//	テストパターンIndex取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pIndex				：テストパターンインデックスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetPattern (int *pIndex)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pIndex Parameter
	if (pIndex == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Test Pattern pIndex NULL Parameter Error\n");
		goto _DONE;
	}

	// Get Index
	*pIndex = IN32 (FPGA_AOI_TP_INDEX_ADRS) & FPGA_AOI_TP_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
// AOI テストパターン インクリメント設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		horizon				：Xインクリメント
//		vertical			：Yインクリメント
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetTpInc (int horizon, int virtical)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check horizon Parameter
	if ((horizon < AOI_TP_HORIZON_INC_VERSION2_MIN) || (horizon > AOI_TP_HORIZON_INC_VERSION2_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Test Pattern X Increment(%d) Parameter Error. (Min:%d / Max:%d)\n", horizon, AOI_TP_HORIZON_INC_VERSION2_MIN, AOI_TP_HORIZON_INC_VERSION2_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check vertical Parameter
	if ((virtical < AOI_TP_VIRTICAL_INC_VERSION2_MIN) || (virtical > AOI_TP_VIRTICAL_INC_VERSION2_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Test Pattern Y Increment(%d) Parameter Error. (Min:%d / Max:%d)\n", virtical, AOI_TP_VIRTICAL_INC_VERSION2_MIN, AOI_TP_VIRTICAL_INC_VERSION2_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Set horizon Increment
	OUT32 (FPGA_AOI_TP_XINC_ADRS, (horizon & FPGA_AOI_TP_XINC_VERSION2_MASK));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_AOI2_TP_XINC_ADRS, (horizon & FPGA_AOI_TP_XINC_VERSION2_MASK));
#endif

	// Set virtical Increment
	OUT32 (FPGA_AOI_TP_YINC_ADRS, (virtical & FPGA_AOI_TP_YINC_VERSION2_MASK));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_AOI2_TP_YINC_ADRS, (virtical & FPGA_AOI_TP_YINC_VERSION2_MASK));
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// AOI テストパターン インクリメント取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pHorizon			：Xインクリメントを格納するポインタ
//		pVirtical			：Yインクリメントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetTpInc (int *pHorizon, int *pVirtical)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pHorizon Parameter
	if (pHorizon == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Test Pattern pHorizon NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pVirtical Parameter
	if (pVirtical == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Test Pattern pVirtical NULL Parameter Error\n");
		goto _DONE;
	}

	*pHorizon = IN32 (FPGA_AOI_TP_XINC_ADRS) & FPGA_AOI_TP_XINC_VERSION2_MASK;
	*pVirtical = IN32 (FPGA_AOI_TP_YINC_ADRS) & FPGA_AOI_TP_YINC_VERSION2_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	テストパターンPosition設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=通常位置/1=前段
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetPatternPosition (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Check index Parameter
	if ((mode != FPGA_AOI_TP_POSITION_NORMAL) && (mode != FPGA_AOI_TP_POSITION_PRE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff,  "Test Pattern Position(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, FPGA_AOI_TP_POSITION_NORMAL, FPGA_AOI_TP_POSITION_PRE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set Test Pattern Position
	OUT32 (FPGA_AOI_TP_POSITION_ADRS, (mode & FPGA_AOI_TP_POSITION_MASK));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_AOI2_TP_POSITION_ADRS, (mode & FPGA_AOI_TP_POSITION_MASK));
#endif

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	テストパターンPosition取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：テストパターンPositionを格納するポインタ(0=通常位置/1=前段)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetPatternPosition (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Test Pattern Position pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Get Test Pattern Position
	*pMode = IN32 (FPGA_AOI_TP_POSITION_ADRS) & FPGA_AOI_TP_POSITION_MASK;

_DONE:
	return (status);
}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
// XFLIP設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：XFLIPモード(0=ライン反転を無効/1=ライン反転を有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetXflip (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	int startMode = 0;
#if defined (MODE_DPC_GRID_UPDATE)
	int count;
	int dpcNum = 0;
	unsigned int dpcMemAdrs;
#endif

	// Check mode Parameter
	if ((mode != XFLIP_ENABLE) && (mode != XFLIP_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Xflip Mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, XFLIP_DISABLE, XFLIP_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// XFLIP設定
	data = IN32 (FPGA_XFLIP_CTRL_ADRS);

	if (mode == XFLIP_ENABLE)
	{
		// すでにEnable?
		if (data & FPGA_XFLIP_ENABLE_BIT)
		{
			// 何もしないで終了
			goto _DONE;
		}

		// Flag設定
		data |= FPGA_XFLIP_ENABLE_BIT;
		OUT32 (FPGA_XFLIP_CTRL_ADRS, data);

		// Optical Black反転
		if ((status = opticalBlackSetInvert (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC反転
		if ((status = ffcDataXFlip (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// DPCメモリアドレス取得
		if ((status =dpcGetMemAdrs (dpcNum, &dpcMemAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC反転
		if ((status = dpcDataXFlip (MODE_ENABLE, dpcMemAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// すでにDisable?
		if ((data & FPGA_XFLIP_ENABLE_BIT) == 0)
		{
			// 何もしないで終了
			goto _DONE;
		}

		// Flagクリア
		data &= ~FPGA_XFLIP_ENABLE_BIT;
		OUT32 (FPGA_XFLIP_CTRL_ADRS, data);

		// Optical Black正転
		if ((status = opticalBlackSetInvert (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC正転
		if ((status = ffcDataXFlip (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// DPCメモリアドレス取得
		if ((status =dpcGetMemAdrs (dpcNum, &dpcMemAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC正転
		if ((status = dpcDataXFlip (MODE_DISABLE, dpcMemAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//------------------------------------------------------------
	// DPC Map情報更新
	//------------------------------------------------------------
#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map情報更新
	if ((status = dpcGetMapInfo ((unsigned char *)DPC_MEMORY_ADRS, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標登録
	dpcSetDefectionCountVersion2 (count);
#endif

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
// XFLIP取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：XFLIPモードを格納するポインタ(0=ライン反転を無効/1=ライン反転を有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetXflip (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Xflip pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// XFLIP取得
	data = IN32 (FPGA_XFLIP_CTRL_ADRS);

	if (data & FPGA_XFLIP_ENABLE_BIT)
		*pMode = XFLIP_ENABLE;
	else
		*pMode = XFLIP_DISABLE;

_DONE:
	return (status);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
// Bitシフト数取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bit					：Bit Width(8=8bit/10=10bit/12=12bit/14=14bit)
//		pShift				：シフト数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetShift (int bit, int *pShift)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pShift Parameter
	if (pShift == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "AOI Shift pSize Parameter Error\n");
		goto _DONE;
	}

	// データサイズ取得
	if (bit == 8)
		*pShift = PIXEL_8_SHIFT;
	else if  (bit == 10)
		*pShift = PIXEL_10_SHIFT;
	else if  (bit == 12)
		*pShift = PIXEL_12_SHIFT;
	else if  (bit == 14)
		*pShift = PIXEL_14_SHIFT;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "AOI Shift Bit(%d) Parameter Error. \n", bit);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Bitシフト数取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bit					：Bit Width(8=8bit/10=10bit/12=12bit/14=14bit)
//		pShift				：シフト数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetShift2 (int bit, int *pShift)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pShift Parameter
	if (pShift == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "AOI Shift pSize Parameter Error\n");
		goto _DONE;
	}

	// シフト数取得
	*pShift = MODE_CAMERA_BIT - bit;

_DONE:
	return (status);
}


//**********************************************************************************
// Flip X取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：ｘ座標(0はじまり)
//		pFlipx				：Flipを考慮したx座標を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetFlipX (int x, int *pFlipx)
{
	int status = AVAL_STATUS_SUCCESS;
	int width;
	int flipMode;

	// Get Width
	width = sensorWidth ();

	// Check x Parameter
	if (x >= width)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Get FlipX x(0x%x) Parameter Error.(Min = 0x00 / Max = 0x%x)\n", x, width);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pFlipx Parameter
	if (pFlipx == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Get FlipX pFlipx Parameter Error.\n");
		goto _DONE;
	}

	// Get Flip
#if !defined (MODE_SENSOR_XFLIP)
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	flipMode = XFLIP_DISABLE;
#endif

	// Flip有効?
#if !defined (MODE_SENSOR_X_REVERSE)
	if (flipMode == XFLIP_ENABLE)
		x = width - 1 - x;
#else
	if (flipMode == XFLIP_DISABLE)
		x = width - 1 - x;
#endif

	// x座標設定
	*pFlipx = x;

_DONE:
	return (status);
}


//**********************************************************************************
// Flip X取得(Flip状態を引数で指定)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：ｘ座標(0はじまり)
//		pFlipx				：Flipを考慮したx座標を格納するポインタ
//		flip				：Flip
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetFlipX2 (int x, int *pFlipx, int flipMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int width;

	// Get Width
	width = sensorWidth ();

	// Check x Parameter
	if (x >= width)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Get FlipX x(0x%x) Parameter Error.(Min = 0x00 / Max = 0x%x)\n", x, width);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pFlipx Parameter
	if (pFlipx == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Get FlipX pFlipx Parameter Error\n");
		goto _DONE;
	}

	// Flip有効?
#if !defined (MODE_SENSOR_X_REVERSE)
	if (flipMode == XFLIP_ENABLE)
		x = width - 1 - x;
#else
	if (flipMode == XFLIP_DISABLE)
		x = width - 1 - x;
#endif

	// x座標設定
	*pFlipx = x;

_DONE:
	return (status);
}


//**********************************************************************************
// Binning パラメータCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		binning					：ビニング
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiBinninCheckParam (int binning)
{
	int status = AVAL_STATUS_SUCCESS;

	if (binning == BINNING1)
		goto _DONE;
	else if (binning == BINNING2)
		goto _DONE;
	else if(binning == BINNING3)
		goto _DONE;
	else if (binning == BINNING4)
		goto _DONE;
	else if (binning == BINNING8)
		goto _DONE;
	else if (binning == BINNING16)
		goto _DONE;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// レジスタ値 => Binning
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data					：データ
//		pBinning				：ビニング値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiBinningRegToSize (unsigned int data, int *pBinning)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pBinning Parameter
	if (pBinning == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Binning Reg To Size pBinning NULL Parameter Error.\n");
		goto _DONE;
	}

	if (data == FPGA_BINNING_2)
		*pBinning = 2;
	else if(data == FPGA_BINNING_3)
		*pBinning = 3;
	else if (data == FPGA_BINNING_4)
		*pBinning = 4;
	else if (data == FPGA_BINNING_8)
		*pBinning = 8;
	else if (data == FPGA_BINNING_16)
		*pBinning = 16;
	else	// Disable
		*pBinning = 1;

_DONE:
	return (status);
}


//**********************************************************************************
// Binning => レジスタ値
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		binning				：ビニング値
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiBinningSizeToReg (int binning, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Binning Size To Reg pData NULL Parameter Error.\n");
		goto _DONE;
	}

	if (binning == 2)
		*pData = FPGA_BINNING_2;
	else if(binning == 3)
		*pData = FPGA_BINNING_3;
	else if (binning == 4)
		*pData = FPGA_BINNING_4;
	else if (binning == 8)
		*pData = FPGA_BINNING_8;
	else if (binning == 16)
		*pData = FPGA_BINNING_16;
	else	// Disable
		*pData = FPGA_BINNING_1;

_DONE:
	return (status);
}


//**********************************************************************************
// Binning => レジスタ値
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		binning				：ビニング値
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetBinningDivide (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int binningX, binningY, divide;
	unsigned int data32, divideValue;

	// BinningX取得
	if ((status = aoiGetBinningX (&binningX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// BinningY取得
	if ((status = aoiGetBinningY (&binningY)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// divide計算
	divide = binningX * binningY;
	divideValue = 65536 / divide;
	
	// Divide設定
	data32 = IN32 (FPGA_BINNING_DIVIDE_ADRS);
	data32 &= ~FPGA_BINNING_DIVIDE_MASK;
	data32 |= divideValue;
	OUT32 (FPGA_BINNING_DIVIDE_ADRS, data32);

_DONE:
	return (status);
}


#if defined(MODE_BINNING)
//**********************************************************************************
// Binning X設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=Off/1=On
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetBinningX (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int startMode;

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	int widthSize, offsetSize;
#endif

#if defined (MODE_CXP)
	int cxpWidth, cxpOffset, currentMode;
#endif
	unsigned int reg;

	int cxpPort = 0;

	// Check mode Parameter
	if ((status = aoiBinninCheckParam (mode)) != AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("Binning x(%d) Set Parameter Error. (Min:%d / Max:%d)\n", mode, BINNING_MIN, BINNING_MAX);
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		goto _DONE;
	}

	//------------------------------------------------------------
	// サイズ設定
	//------------------------------------------------------------

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	// AutoBrightWidth再設定
	if ((status = autoBrightGetWidthSize (&widthSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// AutoBrightWidth再設定
	if ((status = autoBrightSetWidthSize (widthSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// AutoBrightWidthOffset再設定
	if ((status = autoBrightGetWidthOffset (&offsetSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// AutoBrightWidthOffset再設定
	if ((status = autoBrightSetWidthOffset (offsetSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//------------------------------------------------------------
	// CXPサイズ設定
	//------------------------------------------------------------
#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// 現在のモードを取得
		if ((status = aoiGetBinningX (&currentMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Width Size取得
		if ((status = aoiGetWidth (&cxpWidth)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Width Offset取得
		if ((status = aoiGetWidthOffset (&cxpOffset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		//------------------------------------------------------------
		// 複数Binning対応
		//------------------------------------------------------------

		// 現在のモードから素の値を算出
		cxpWidth  *= currentMode;
		cxpOffset *= currentMode;

		// 今回のモードはMODE_ENABLE?
		cxpWidth  /= mode;
		cxpOffset /= mode;

		#if defined (MODE_CXP_MULTI_PORT)
		// CXp Port取得
		if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		#endif // #if !defined (MODE_CXP_MULTI_PORT)
		
		// Width設定
		if ((status = cxpSetWidth (cxpPort, cxpWidth)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offsetx設定
		if ((status = cxpSetOffsetX (cxpPort, cxpOffset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if defined (MODE_CXP)

	//------------------------------------------------------------
	// 複数Binning対応
	//------------------------------------------------------------

	// binning => レジスタ値変換
	if ((status = aoiBinningSizeToReg (mode, &reg)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// BinningX取得
	data32 = IN32 (FPGA_BINNING_CTRL_ADRS);
	data32 &= ~(FPGA_X_BINNING_MASK << FPGA_X_BINNING_SHIFT);
	data32 |= (reg << FPGA_X_BINNING_SHIFT);

	// BinningX設定
	OUT32 (FPGA_BINNING_CTRL_ADRS, data32);

	// Diveide設定
	if ((status = aoiSetBinningDivide ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// Binning X取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：modeを格納するポインタ (0=Off/1=On)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetBinningX (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Binning X Get pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// BinningX取得
	data32 = IN32 (FPGA_BINNING_CTRL_ADRS);
	data32 >>= FPGA_X_BINNING_SHIFT;
	data32 &=  FPGA_X_BINNING_MASK;

	// レジスタ値 => binning 変換
	if ((status = aoiBinningRegToSize (data32, pMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// Binning Y設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=Off/1=On
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetBinningY (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int startMode;
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	int heightSize, offsetSize;
#endif
#if defined (MODE_CXP)
	int cxpHeight, cxpOffset, currentMode;
#endif
	unsigned int reg;
	#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;
	#endif
	int cxpPort = 0;

	// Check mode Parameter
	if ((status = aoiBinninCheckParam (mode)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Binning y(%d) Set Parameter Error. (Min:%d / Max:%d)\n", mode, BINNING_MIN, BINNING_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#if defined (MODE_FRAMERATE_HIGH_SPEED)
	// High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode(&hsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (hsMode == MODE_ENABLE)
	{
		if ((mode != 1) && (mode != 2) && (mode != 4))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Binning y(%d) Set Parameter Error. (Min:%d / Max:4)\n", mode, BINNING_MIN);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
	#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)


	//------------------------------------------------------------
	// サイズ設定
	//------------------------------------------------------------

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	// AutoBrightHeight再設定
	if ((status = autoBrightGetHeightSize (&heightSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// AutoBrightHeight再設定
	if ((status = autoBrightSetHeightSize (heightSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// AutoBrightHeightOffset再設定
	if ((status = autoBrightGetHeightOffset (&offsetSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// AutoBrightHeightOffset再設定
	if ((status = autoBrightSetHeightOffset (offsetSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif


	//------------------------------------------------------------
	// CXPサイズ設定
	//------------------------------------------------------------
#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// 現在のモードを取得
		if ((status = aoiGetBinningY (&currentMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Height Size取得
		if ((status = aoiGetHeight (&cxpHeight)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Height Offset取得
		if ((status = aoiGetHeightOffset (&cxpOffset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 現在のモードから素の値を算出
		cxpHeight *= currentMode;
		cxpOffset *= currentMode;

		// 今回のモードはMODE_ENABLE?
		cxpHeight /= mode;
		cxpOffset /= mode;


		#if defined (MODE_CXP_MULTI_PORT)
		// CXp Port取得
		if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		#endif // #if !defined (MODE_CXP_MULTI_PORT)

		// Height設定
		if ((status = cxpSetHeight (cxpPort, cxpHeight)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offsety設定
		if ((status = cxpSetOffsetY (cxpPort, cxpOffset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if defined (MODE_CXP)


	// binning => レジスタ値変換
	if ((status = aoiBinningSizeToReg (mode, &reg)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// BinningY取得
	data32 = IN32 (FPGA_BINNING_CTRL_ADRS);
	data32 &= ~(FPGA_Y_BINNING_MASK << FPGA_Y_BINNING_SHIFT);
	data32 |= (reg << FPGA_Y_BINNING_SHIFT);

	// BinningY設定
	OUT32 (FPGA_BINNING_CTRL_ADRS, data32);

	// Diveide設定
	if ((status = aoiSetBinningDivide ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// Binning Y取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：modeを格納するポインタ (0=Off/1=On)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetBinningY (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Binning X Get pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// BinningY取得

	//------------------------------------------------------------
	// 複数Binning対応
	//------------------------------------------------------------
	data32 = IN32 (FPGA_BINNING_CTRL_ADRS);
	data32 >>= FPGA_Y_BINNING_SHIFT;
	data32 &=  FPGA_Y_BINNING_MASK;

	// レジスタ値 => binning 変換
	if ((status = aoiBinningRegToSize (data32, pMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}

//**********************************************************************************
// Binning Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=Sum/1=Average
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetBinningMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check mode Parameter
	if ((mode != FPGA_BINNING_MODE_SUM) && (mode != FPGA_BINNING_MODE_AVG))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Binning Set Mode(%d) Parameter Error. (Sum:%d / Average:%d)\n", mode, FPGA_BINNING_MODE_SUM, FPGA_BINNING_MODE_AVG);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// BinningMode取得
	data32 = IN32 (FPGA_BINNING_CTRL_ADRS) & ~FPGA_BINNING_MODE_MASK;
	data32 |= FPGA_BINNING_MODE_SET (mode);
	OUT32 (FPGA_BINNING_CTRL_ADRS, data32);

_DONE:
	return (status);
}

//**********************************************************************************
// Binning Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：modeを格納するポインタ (0=Sum/ 1=Average)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetBinningMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Binning Mode Get pMode NULL Parameter Error\n");
		goto _DONE;
	}

	data32 = IN32(FPGA_BINNING_CTRL_ADRS);

	// BinningMode取得
	*pMode = FPGA_BINNING_MODE_GET(data32);

_DONE:
	return (status);
}


//**********************************************************************************
// Binning Offset Base設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		offset				：黒補正オフセット(14bit)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetBinningOffsetBase (int offset)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check mode Parameter
	if ((offset < FFC_BLACK_TARGET_MIN) || (offset > FFC_BLACK_TARGET_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Binning Offset Base Set Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", offset, FFC_BLACK_TARGET_MIN, FFC_BLACK_TARGET_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//16bitにシフト
	offset <<= 2;

	// BinningOffsetBase取得
	data32 = IN32 (FPGA_BINNING_CTRL_ADRS) & ~FPGA_BINNING_OFFSET_BASE_MASK;
	data32 |= FPGA_BINNING_OFFSET_BASE_SET (offset);
	OUT32 (FPGA_BINNING_CTRL_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
// Binning Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：黒補正オフセットを格納するポインタ(14bit)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetBinningOffsetBase (int* pOffset)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Binning Offset Base Get pMode NULL Parameter Error\n");
		goto _DONE;
	}

	data32 = IN32(FPGA_BINNING_CTRL_ADRS);

	// BinningOffsetBase取得
	*pOffset = (FPGA_BINNING_OFFSET_BASE_GET(data32)) >> 2;

_DONE:
	return (status);
}

#endif //#if defined(MODE_BINNING)

// eof
