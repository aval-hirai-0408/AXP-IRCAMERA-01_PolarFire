//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// lut.c - LUT Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"

//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// LUT Status
int gLutStatus = 0;


//----------------------------------------------------------------------------------
// static
//----------------------------------------------------------------------------------

// LUT Format
int gLutFormat[LUT_SELECT_NUM] = {};

// LUT Threshold
int gLutBinThrethold[LUT_SELECT_NUM] = {};


//**********************************************************************************
// LUT初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs, data;
	int select, format;

	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "LUT Initialize\n");

	// LUT Disable
	lutSetEnable (LUT_SELECT1, LUT_DISABLE);

	// LUT1 2値化しきい値取得
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT1_BIN_THRESHOLD_ADRS, &adrs, &data);
	gLutBinThrethold[0] = data;

#if (LUT_SELECT_NUM >= 2)
	// LUT2 2値化しきい値取得
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT2_BIN_THRESHOLD_ADRS, &adrs, &data);
	gLutBinThrethold[1] = data;
#endif

	// LUT1 Format値取得
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT1_FORMAT_ADRS, &adrs, &data);
	gLutFormat[0] = data;

#if (LUT_SELECT_NUM >= 2)
	// LUT2 Format値取得
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT2_FORMAT_ADRS, &adrs, &data);
	gLutFormat[1] = data;
#endif

	// CameraLinkのみ
	if (gInterFaceID == INTERFACE_CAMERALINK)
	{
		for (select=1; select<=LUT_SELECT_NUM; select++)
			lutSetFormat (select, gLutFormat[(select -1)]);
	}
	// GigE/CXPのみ
	else if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20) || (gInterFaceID == INTERFACE_CXP))
	{
		if (CAMERA_SAVE_USER_NUM == CAMERA_FACTORY_NUM)
			format = LUT_FORMAT_INC;
		else
			format = LUT_FORMAT_FLASH;

		for (select=1; select<=LUT_SELECT_NUM; select++)
			lutSetFormat (select, format);
	}

	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT_ADRS, CAMERA_SAVE_LUT_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gLutStatus = status;
		goto _DONE;
	}

#if defined (MODE_IPU_MULTI)
	// Register Data Restore
	if ((status = cameraParamWriteRegisterOffsetAdrs (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT_ADRS, 0x04, FPGA_LUT2_OFFSET)) != AVAL_STATUS_SUCCESS)
	{
		gLutStatus = status;
		goto _DONE;
	}
#endif


_DONE:
	return (status);
}


//**********************************************************************************
//	LUT Default
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutDefault (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;
	int select;
	int setBit, getBit = -1;

	// bit取得
	if ((status = aoiGetBitWidth (&getBit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// bit設定
	setBit = LUT_DEFAULT_BIT;
	if ((status = aoiSetBitWidth (setBit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	for (select=1; select<=LUT_SELECT_NUM; select++)
	{
		// LUT設定
		if ((status = lutSetFormat (select, LUT_FORMAT_INC)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// LUTテーブルアドレス取得
		if ((status = lutGetMemAdrs (select, &adrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// LUTテーブルデータ保存
		if ((status = lutSetFlash (select, (unsigned int *)adrs, LUT_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	// bit設定(元に戻す)
	if (getBit != -1)
		aoiSetBitWidth (getBit);

	return (status);
}


//**********************************************************************************
// LUTモード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：モード(0=無効/1=LUT1有効/2=LUT2有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutSetMode (int lutMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int select, mode;

	// Check lutMode Parameter
	if ((lutMode < LUT_MODE_MIN) || (lutMode > LUT_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Lut Set Mode(%d) Parameter Error.(Min:%d / Max:%.d)\n", lutMode, LUT_MODE_MIN, LUT_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if (lutMode == LUT_MODE_DISABLE)
	{
		select = LUT_SELECT1;
		mode = LUT_DISABLE;
	}
	else if (lutMode == LUT_MODE1)
	{
		select = LUT_SELECT1;
		mode = LUT_ENABLE;
	}
	else if (lutMode == LUT_MODE2)
	{
		select = LUT_SELECT2;
		mode = LUT_ENABLE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Set Mode(%d) Parameter Error.(Min:%d / Max:%d)\n", lutMode, LUT_MODE_MIN, LUT_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// LUT設定
	if ((status = lutSetEnable (select, mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// LUTモード取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：モードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetMode (int *pLutMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int select, mode;

	// Check pLutMode Parameter
	if (pLutMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Get Mode pLutMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// LUT取得
	if ((status = lutGetEnable (&select, &mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == LUT_ENABLE)
	{
		if (select == LUT_SELECT1)
			*pLutMode = LUT_MODE1;
		else
			*pLutMode = LUT_MODE2;
	}
	else
	{
		*pLutMode = LUT_MODE_DISABLE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// LUT設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：使用LUT番号(1=LUT1/2=LUT2)
//		mode				：LUT有効／無効設定(0=無効/1=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutSetEnable (int select, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Enable select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check mode Parameter
	if ((mode != LUT_ENABLE) && (mode != LUT_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Enable mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, LUT_DISABLE, LUT_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// LUT有効/無効設定
	if (mode == LUT_ENABLE)
		data = FPGA_LUT_CTRL_ENABLE_BIT;
	else
		data = 0;

	// LUT有効/無効設定
	if (select == LUT_SELECT2)
		data |= FPGA_LUT_CTRL_SELECT_BIT;

	// LUT設定
	OUT32 (FPGA_LUT_CTRL_ADRS, data);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_LUT2_CTRL_ADRS, data);
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// LUT取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSelect				：LUT番号を格納するポインタ(1=LUT1/2=LUT2)
//		pMode				：LUT有効状態を格納するポインタ(0=無効/1=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetEnable (int *pSelect, int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pSelect Parameter
	if (pSelect == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Enable pSelect NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Enable pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// LUT Ctrl取得
	data = IN32 (FPGA_LUT_CTRL_ADRS);

	// LUT有効/無効状態取得
	if (data & FPGA_LUT_CTRL_ENABLE_BIT)
		*pMode = LUT_ENABLE;
	else
		*pMode = LUT_DISABLE;

	// LUT番号取得
	if ((data & FPGA_LUT_CTRL_SELECT_BIT) == 0)
		*pSelect = LUT_SELECT1;
	else
		*pSelect = LUT_SELECT2;

_DONE:
	return (status);
}


//**********************************************************************************
// LUTデータフォーマット設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：使用LUT番号(1=LUT1/2=LUT2)
//		format				：LUTフォーマット
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutSetFormat (int select, int format)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs, size;
	unsigned int i, j, offset;
	unsigned int data;
	unsigned int interval;
	unsigned int realThreshold;
	unsigned int ganma;
	int bit;
	int shift;
	unsigned int sizeMax;
	double sizeMaxD;
	char *buff;
	int gammaMode;
	float gamma;

	// GigEの場合のみ
	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		if (select == 0)
			return (AVAL_STATUS_SUCCESS);
	}

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Set Format select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check format Parameter
	if ((format < LUT_FORMAT_MIN) || (format > LUT_FORMAT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Set Format(%d) Parameter Error.(Min:%d / Max:%d)\n", format, LUT_FORMAT_MIN, LUT_FORMAT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データサイズ取得
	if ((status = lutGetTablInterval (bit, &interval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get LUT table Address
	if ((status = lutGetMemAdrs (select, &adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データサイズ取得
	if ((status = lutGetTableSize (&sizeMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// LUTテーブルサイズ
	size = PIXEL_SIZE_MAX;

	// データ作成
	switch (format)
	{
		case LUT_FORMAT_INC:
			data = 0;
			for (i=0, offset=0; i<size; data++)
				for (j=0; j<interval; j++, i++, offset+=4)
				{
					OUT32 ((adrs + offset), ((data & LUT_DATA_MASK)<<shift));
					
					#if defined (MODE_IPU_MULTI)
					OUT32 ((adrs + offset + FPGA_LUT2_OFFSET), ((data & LUT_DATA_MASK)<<shift));
					#endif
				}
			break;

		case LUT_FORMAT_REV:
			data = size - 1;
			for (i=0, offset=0; i<size; data--)
				for (j=0; j<interval; j++, i++, offset+=4)
				{
					OUT32 ((adrs + offset), ((data & LUT_DATA_MASK)<<shift));

					#if defined (MODE_IPU_MULTI)
					OUT32 ((adrs + offset + FPGA_LUT2_OFFSET), ((data & LUT_DATA_MASK)<<shift));
					#endif
				}
			break;

		case LUT_FORMAT_BIN:
			if (gLutBinThrethold[(select - 1)] > sizeMax)
				gLutBinThrethold[(select - 1)] = sizeMax;

			realThreshold = gLutBinThrethold[(select - 1)] * interval;

			data = 0;
			for (i=0, offset=0; i<realThreshold;)
				for (j=0; j<interval; j++, i++, offset+=4)
				{
					OUT32 ((adrs + offset), ((data & LUT_DATA_MASK)<<shift));

					#if defined (MODE_IPU_MULTI)
					OUT32 ((adrs + offset + FPGA_LUT2_OFFSET), ((data & LUT_DATA_MASK)<<shift));
					#endif
				}

			data = size - 1;
			for (; i<size;)
				for (j=0; j<interval; j++, i++, offset+=4)
				{
					OUT32 ((adrs + offset), ((data & LUT_DATA_MASK)<<shift));

					#if defined (MODE_IPU_MULTI)
					OUT32 ((adrs + offset + FPGA_LUT2_OFFSET), ((data & LUT_DATA_MASK)<<shift));
					#endif
				}
			break;

		case LUT_FORMAT_FLASH:
			if ((status = lutGetFlash (select, (unsigned int *)adrs, LUT_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			break;

		case LUT_FORMAT_GAMMA:
			// Userパラメータ領域にも保存（一時メモリ）
			if (select == LUT_SELECT1)
				cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT1_GAMMA_ADRS, &adrs, &data);
			else
				cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT2_GAMMA_ADRS, &adrs, &data);

			gamma = (float)((float)data / (float)LUT_GAMMA_DATA_ADJUST);

			if ((status = lutSetDataGamma (select, gamma)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			break;
	}

	// キャッシュFlash
	cacheFlush ();

	// LUT Format保存
	gLutFormat[(select - 1)] = format;

	// Userパラメータ領域にも保存（一時メモリ）
	if (select == LUT_SELECT1)
		cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT1_FORMAT_ADRS, 0, format);
	else
		cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT2_FORMAT_ADRS, 0, format);

_DONE:
	return (status);
}


//**********************************************************************************
// LUTデータフォーマット取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：使用LUT番号(1=LUT1/2=LUT2)
//		pFormat				：LUTフォーマットを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetFormat (int select, int *pFormat)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs = 0, data;

	// Check pFormat Parameter
	if (pFormat == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Get Format pFormat pBuffer NULL Parameter Error.\n");
		goto _DONE;
	}

	// GigE/CXPの場合のみ
	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20) ||  (gInterFaceID == INTERFACE_CXP))
	{
		if (select == 0)
		{
			*pFormat = LUT_FORMAT_INC;
			return (AVAL_STATUS_SUCCESS);
		}
	}

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Get Format select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// LUT Format取得
	if (select == LUT_SELECT1)
		cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT1_FORMAT_ADRS, &adrs, &data);
	else
		cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT2_FORMAT_ADRS, &adrs, &data);

	*pFormat = data;

_DONE:
	return (status);
}


//**********************************************************************************
// LUT 2値化データしきい値設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：使用LUT番号(1=LUT1/2=LUT2)
//		threshold			：2値化データしきい値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutSetBinThreshold (int select, int threshold)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs, sizeMax;
	unsigned int i, j, offset;
	unsigned int data;
	unsigned int interval;
	unsigned int realThreshold;
	int bit;
	int shift;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Binarization select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// データサイズ取得
	if ((status = lutGetTableSize (&sizeMax)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Table Parameter Error.\n");
		goto _DONE;
	}

	// Check threshold Parameter
	if ((threshold < THRESHOLD_SIZE_MIN) || (threshold > sizeMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Binarization Threshold(%d) Parameter Error.(Min:%d / Max:%d)\n", threshold, THRESHOLD_SIZE_MIN, sizeMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// LUTインターバル取得
	if ((status = lutGetTablInterval (bit, &interval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get LUT table Address
	if ((status = lutGetMemAdrs (select, &adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// LUT 2値化データしきい値保存
	gLutBinThrethold[(select - 1)] = threshold;
	realThreshold = threshold * interval;

	// Userパラメータ領域にも保存（一時メモリ）
	if (select == LUT_SELECT1)
		cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT1_BIN_THRESHOLD_ADRS, 0, threshold);
	else
		cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT2_BIN_THRESHOLD_ADRS, 0, threshold);

	// データ設定
	data = 0;
	for (i=0, offset=0; i<realThreshold;)
		for (j=0; j<interval; j++, i++, offset+=4)
		{
			OUT32 ((adrs + offset), ((data & LUT_DATA_MASK)<<shift));

			#if defined (MODE_IPU_MULTI)
			OUT32 ((adrs + offset + FPGA_LUT2_OFFSET), ((data & LUT_DATA_MASK)<<shift));
			#endif
		}

	for (; i<PIXEL_SIZE_MAX;)
	{
		for (j=0; j<interval; j++, i++, offset+=4)
		{
			OUT32 ((adrs + offset), PIXEL_14_MASK);

			#if defined (MODE_IPU_MULTI)
			OUT32 ((adrs + offset + FPGA_LUT2_OFFSET), PIXEL_14_MASK);
			#endif
		}
	}

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
// LUT 2値化データしきい値取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：使用LUT番号(1=LUT1/2=LUT2)
//		pThreshold			：2値化データしきい値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetBinThreshold (int select, int *pThreshold)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs = 0, data;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Binarization select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// LUT 2値化データしきい値取得
	if (select == LUT_SELECT1)
		cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT1_BIN_THRESHOLD_ADRS, &adrs, &data);
	else
		cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT2_BIN_THRESHOLD_ADRS, &adrs, &data);

	*pThreshold = data;

_DONE:
	return (status);
}


//**********************************************************************************
// LUTガンマデータ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：使用LUT番号(1=LUT1/2=LUT2)
//		gamma				：ガンマ値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutSetGamma (int select, float gamma)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Gamma select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gamma Parameter
	if ((gamma < LUT_GAMMA_MIN) || (gamma > LUT_GAMMA_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Gamma(%f) Parameter Error.(Min:%.1f / Max:%.1f)\n", gamma, LUT_GAMMA_MIN, LUT_GAMMA_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// ガンマデータ設定
	if ((status = lutSetDataGamma (select, gamma)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// キャッシュFlash
	cacheFlush ();

	// Gammaデータはfloat型の為、10倍してinteger型に変換して保存
	data = (unsigned int)(gamma * LUT_GAMMA_DATA_ADJUST);

	// Userパラメータ領域にも保存（一時メモリ）
	if (select == LUT_SELECT1)
		cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT1_GAMMA_ADRS, 0, data);
	else
		cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT2_GAMMA_ADRS, 0, data);

_DONE:
	return (status);
}


//**********************************************************************************
// LUTガンマデータ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：使用LUT番号(1=LUT1/2=LUT2)
//		pGamma				：ガンマ値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetGamma (int select, float *pGamma)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs = 0, data;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Gamma select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pGamma Parameter
	if (pGamma == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Gamma pGamma NULL Parameter Error.\n");
		goto _DONE;
	}

	// Userパラメータ領域にも保存（一時メモリ）
	if (select == LUT_SELECT1)
		cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT1_GAMMA_ADRS, &adrs, &data);
	else
		cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_LUT2_GAMMA_ADRS, &adrs, &data);

	*pGamma = (float)((float)data / (float)LUT_GAMMA_DATA_ADJUST);

_DONE:
	return (status);
}


//**********************************************************************************
// ガンマデータ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：使用LUT番号(1=LUT1/2=LUT2)
//		gamma				：ガンマ値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutSetDataGamma (int select, float gamma)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;
	int i, j;
	unsigned int offset;
	int bit, shift;
	unsigned int interval, sizeMax;
	double sizeMaxD;
	unsigned int data, dataGamma;
	unsigned int size;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Gamma select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// LUTインターバル取得
	if ((status = lutGetTablInterval (bit, &interval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
		
	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get LUT table Address
	if ((status = lutGetMemAdrs (select, &adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データサイズ取得
	if ((status = lutGetTableSize (&sizeMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// LUTテーブルサイズ
	size = PIXEL_SIZE_MAX;

	// ガンマデータ
	sizeMaxD = (double)(sizeMax - 1);
	for (i=0, offset=0, data = 0; i<size; data++)
	{
		dataGamma = MAKE_GANMA_CAMERA_DATA(data, sizeMaxD, gamma);
		for (j=0; j<interval; j++, i++, offset+=4)
		{
			OUT32 ((adrs + offset), ((dataGamma & LUT_DATA_MASK)<<shift));

			#if defined (MODE_IPU_MULTI)
			OUT32 ((adrs + offset + FPGA_LUT2_OFFSET), ((dataGamma & LUT_DATA_MASK)<<shift));
			#endif
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
// LUT Table設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：LUT番号(1=LUT1/2=LUT2)
//		pBuff				：LUTテーブルに設定するデータのポインタ
//		count				：LUTテーブルに設定するデータ個数（PIXEL_8_SIZE/PIXEL_10_SIZE/PIXEL_12_SIZE/PIXEL_14_SIZE）
//		bit					：Bit Width(8=8bit/10=10bit/12=12bit/14=14bit)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutSetTable (int select, unsigned char *pBuff, int count, int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i, j, offset;
	unsigned char *ptr8;
	unsigned short *ptr16;
	unsigned int interval;
	unsigned int adrs, data;
	unsigned short data16;
	unsigned int maxCount;
	int shift;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Set Table select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pBuff Parameter
	if (pBuff == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Set Table pBuffer NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check bit Parameter
	if (bit == 8)
		maxCount = PIXEL_8_SIZE;
	else if  (bit == 10)
		maxCount = PIXEL_10_SIZE;
	else if  (bit == 12)
		maxCount = PIXEL_12_SIZE;
	else if  (bit == 14)
		maxCount = PIXEL_14_SIZE;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Set Table Bit(%d) Parameter Error.(Min:%d / Max:%d)\n", bit, PIXEL_BIT_MIN, PIXEL_BIT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check count Parameter
	if ((count <= 0) || (count > maxCount))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Set Table Count(%d) Parameter Error.(Min:1 / Max:%d)\n", count, maxCount);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// LUTインターバル取得
	if ((status = lutGetTablInterval (bit, &interval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get LUT Address
	if ((status = lutGetMemAdrs (select, &adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// LUT更新
	if (bit == 8)
	{
		ptr8 = pBuff;
		for (i=0, offset=0; i<count; i++, ptr8++)
		{
			data = (unsigned int)*ptr8;
			for (j=0; j<interval; j++, offset+=4)
			{
				OUT32 ((adrs + offset), ((data & LUT_DATA_MASK)<<shift));

				#if defined (MODE_IPU_MULTI)
				OUT32 ((adrs + offset + FPGA_LUT2_OFFSET), ((data & LUT_DATA_MASK)<<shift));
				#endif
			}
		}
	}
	else
	{
		ptr16 = (unsigned short *)pBuff;
		for (i=0, offset=0; i<count; i++, ptr16++)
		{
			data16 = (unsigned short)*ptr16;
			data = data16;
			for (j=0; j<interval; j++, offset+=4)
			{
				OUT32 ((adrs + offset), (unsigned int)((data & LUT_DATA_MASK)<<shift));

				#if defined (MODE_IPU_MULTI)
				OUT32 ((adrs + offset + FPGA_LUT2_OFFSET), (unsigned int)((data & LUT_DATA_MASK)<<shift));
				#endif
			}
		}
	}

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
// LUT Table取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：LUT番号(1=LUT1/2=LUT2)
//		pBuff				：LUTテーブルに設定されたデータを格納するポインタ
//		count				：LUTテーブルから取得する個数（PIXEL_8_SIZE/PIXEL_10_SIZE/PIXEL_12_SIZE/PIXEL_14_SIZE）
//		bit					：Bit Width(8=8bit/10=10bit/12=12bit/14=14bit)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetTable (int select, unsigned char *pBuff, int count, int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i, offset;
	unsigned char *ptr8;
	unsigned short *ptr16;
	unsigned int interval;
	unsigned int adrs;
	unsigned int maxCount;
	int shift;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Get Table select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pBuff Parameter
	if (pBuff == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Get Table pBuffer NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check bit Parameter
	if (bit == 8)
		maxCount = PIXEL_8_SIZE;
	else if  (bit == 10)
		maxCount = PIXEL_10_SIZE;
	else if  (bit == 12)
		maxCount = PIXEL_12_SIZE;
	else if  (bit == 14)
		maxCount = PIXEL_14_SIZE;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Get Table Bit(%d) Parameter Error.(Min:%d / Max:%d)\n", bit, PIXEL_BIT_MIN, PIXEL_BIT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check count Parameter
	if ((count <= 0) || (count > maxCount))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Get Table Count(%d) Parameter Error.(Min:1 / Max:%d)\n", count, maxCount);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// LUTインターバル取得
	if ((status = lutGetTablInterval (bit, &interval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get LUT Address
	if ((status = lutGetMemAdrs (select, &adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// LUTデータ取得
	if (bit == 8)
	{
		ptr8 = (unsigned char *)pBuff;
		for (i=0, offset=0; i<count; ptr8++, i++)
		{
			*ptr8 = (unsigned char)((IN32 ((adrs + offset)) >> shift) & LUT_DATA_MASK);
			offset += (interval * 4);
		}
	}
	else
	{
		ptr16 = (unsigned short *)pBuff;
		for (i=0, offset=0; i<count; ptr16++, i++)
		{
			*ptr16 = (unsigned short)((IN32 ((adrs + offset)) >> shift) & LUT_DATA_MASK);
			offset += (interval * 4);
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
// LUT 任意データ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：使用LUT番号(1=LUT1/2=LUT2)
//		offset				：LUTテーブルアドレス
//		data				：LUTデータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutSetTableData (int select, unsigned int offset, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs, tableAdrs;
	unsigned int dataSize, interval;
	unsigned int j;
	int bit;
	int shift;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Table select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// データサイズ取得
	if ((status = lutGetTableSize (&dataSize)) != AVAL_STATUS_SUCCESS)
	{
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Table Parameter Error.\n");
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		goto _DONE;
	}

	// Check offset Parameter
	if (offset >= dataSize)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Table Data offset(%d) Parameter Error.(Min:0 / Max:%d)\n", offset, dataSize-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check data Parameter
	if (data >= dataSize)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Table Data data(%d) Parameter Error.(Min:0 / Max:%d)\n", data, dataSize-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get LUT table Address
	if ((status = lutGetMemAdrs (select, &adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// LUTインターバルデータ
	if ((status = lutGetTablInterval (bit, &interval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データ設定
	tableAdrs =  adrs + offset * LUT_TABLE_INTERVAL * interval;
	for (j=0; j<interval; j++)
	{
		OUT32 ((tableAdrs + j * LUT_TABLE_INTERVAL), ((data & LUT_DATA_MASK)<<shift));

		#if defined (MODE_IPU_MULTI)
		OUT32 ((tableAdrs + j * LUT_TABLE_INTERVAL + FPGA_LUT2_OFFSET), ((data & LUT_DATA_MASK)<<shift));
		#endif
	}

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
// LUT 任意データ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：使用LUT番号(1=LUT1/2=LUT2)
//		offset				：LUTテーブルアドレス
//		pDdata				：LUTデータを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetTableData (int select, unsigned int offset, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs, tableAdrs;
	unsigned int dataSize, interval;
	int bit;
	int shift;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Table select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// データサイズ取得
	if ((status = lutGetTableSize (&dataSize)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Table Parameter Error.\n");
		goto _DONE;
	}

	// Check offset Parameter
	if (offset >= dataSize)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Table Data offset(%d) Parameter Error.(Min:0 / Max:%d)\n", offset, dataSize-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Table pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get LUT table Address
	if ((status = lutGetMemAdrs (select, &adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// LUTインターバルデータ
	if ((status = lutGetTablInterval (bit, &interval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データ設定
	tableAdrs =  adrs + offset * LUT_TABLE_INTERVAL * interval;

	// データ設定
	*pData = (IN32 (tableAdrs) >> shift) & LUT_DATA_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
// LUTデータFlashへの書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：LUT番号(1=LUT1/2=LUT2)
//		pBuffer				：LUTデータが格納されたポインタ
//		size				：書き込みデータ数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutSetFlash (int select, unsigned int *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int flashAdrs;

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Flash Write select(%d) Parameter Error.\n", select);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Flash Write pBuffer Parameter Error.\n");
		goto _DONE;
	}

	// Check size Parameter
	if ((size < 1) && (size > LUT_DATA_SIZE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Flash Write size(0x%x) Parameter Error.(Min:1 / Max:%d)\n", size, LUT_DATA_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get LUT Table Address
	if (select == LUT_SELECT1)
		flashAdrs = FLASH_LUT1_ADRS;
	else
		flashAdrs = FLASH_LUT2_ADRS;

	// Erase
	if ((status = qspiFlashSectorErase (flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Flash Data Write
	if ((status = qspiFlashWrite (flashAdrs, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// LUTデータFlashからの読み込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：LUT番号(1=LUT1/2=LUT2)
//		pBuffer				：LUTデータを格納するポインタ
//		size				：書き込みデータ数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetFlash (int select, unsigned int *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int flashAdrs;
#if defined (MODE_IPU_MULTI)
	unsigned int srcAdrs, desAdrs;
	unsigned int i;
	unsigned int data32;
#endif

	// Check select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Flash Read select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Flash Read pBuffer Parameter Error.\n");
		goto _DONE;
	}

	// Check size Parameter
	if ((size < 1) && (size > LUT_DATA_SIZE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Flash Read size(0x%x) Parameter Error.(Min:1 / Max:%d)\n", size, LUT_DATA_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get LUT Table Address
	if (select == LUT_SELECT1)
		flashAdrs = FLASH_LUT1_ADRS;
	else
		flashAdrs = FLASH_LUT2_ADRS;

	// flash to memory
	if ((status = qspiFlashRead (flashAdrs, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_IPU_MULTI)
	srcAdrs = (unsigned int)pBuffer;
	desAdrs = (unsigned int)pBuffer;
	desAdrs += FPGA_LUT2_OFFSET;
	for (i=0; i<(size/4); i++)
	{
		data32 = IN32 (srcAdrs);
		OUT32 (desAdrs, data32);

		srcAdrs += 4;
		desAdrs += 4;
	}
#endif

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
// LUTをFlashから読み込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：LUT番号(1=LUT1/2=LUT2)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutLoadData (int select)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Load select(%d) Parameter Error.(Min:%d / Max:d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Load
	if (select == LUT_SELECT1)
	{
		// LUT1 data Form Flash
		if ((status = lutGetFlash (LUT_SELECT1, (unsigned int *)FPGA_LUT_MEM1_ADRS, LUT_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// LUT2 data Form Flash
		if ((status = lutGetFlash (LUT_SELECT2, (unsigned int *)FPGA_LUT_MEM2_ADRS, LUT_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
// LUT メモリアドレス取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：LUT番号(1=LUT1/2=LUT2)
//		pSize				：LUTテーブルサイズ取得
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetMemAdrs (int select, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Get Adress select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Get Adrs pAdrs NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get LUT table Address
	if (select == LUT_SELECT1)
		*pAdrs = FPGA_LUT_MEM1_ADRS;
	else
		*pAdrs = FPGA_LUT_MEM2_ADRS;

_DONE:
	return (status);
}


//**********************************************************************************
// LUT Flashアドレス取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		select				：LUT番号(1=LUT1/2=LUT2)
//		pSize				：LUTテーブルサイズ取得
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetFlashAdrs (int select, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Select Parameter
	if ((select < LUT_SELECT_MIN_NUM) || (select > LUT_SELECT_MAX_NUM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Get Flash Adress select(%d) Parameter Error.(Min:%d / Max:%d)\n", select, LUT_SELECT_MIN_NUM, LUT_SELECT_MAX_NUM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Get Flash Adress pBuffer NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get LUT Table Address
	if (select == LUT_SELECT1)
		*pAdrs = FLASH_LUT1_ADRS;
	else
		*pAdrs = FLASH_LUT2_ADRS;

_DONE:
	return (status);
}


//**********************************************************************************
// LUT Bitサイズ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSize				：LUTテーブルサイズ取得
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetTableSize (unsigned int *pSize)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Table Size pSize NULL Parameter Error.\n");
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		return (status);

	// データサイズ取得
	if (bit == 8)
		*pSize = PIXEL_8_SIZE;
	else if  (bit == 10)
		*pSize = PIXEL_10_SIZE;
	else if  (bit == 12)
		*pSize = PIXEL_12_SIZE;
	else if  (bit == 14)
		*pSize = PIXEL_14_SIZE;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Table Size Bit(%d) Parameter Error.\n", bit);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// LUT Bitインターバル取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bit					：Bit Width(8=8bit/10=10bit/12=12bit/14=14bit)
//		pSize				：LUTテーブルサイズ取得
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lutGetTablInterval (int bit, unsigned int *pSize)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "LUT Table Interval pSize NULL Parameter Error.\n");
		goto _DONE;
	}

	// データサイズ取得
	if (bit == 8)
		*pSize = PIXEL_SIZE_MAX / PIXEL_8_SIZE;
	else if  (bit == 10)
		*pSize = PIXEL_SIZE_MAX / PIXEL_10_SIZE;
	else if  (bit == 12)
		*pSize = PIXEL_SIZE_MAX / PIXEL_12_SIZE;
	else if  (bit == 14)
		*pSize = PIXEL_SIZE_MAX / PIXEL_14_SIZE;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_LUT, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "LUT Table Size Bit(%d) Parameter Error.\n", bit);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}

// eof
