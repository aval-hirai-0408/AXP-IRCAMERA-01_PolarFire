//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// roi.c - ROI Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
#include "sensorVendors.h"
#endif


#if defined (MODE_ROI_VERSION2)
//----------------------------------------------------------------------------------
// defined
//----------------------------------------------------------------------------------
#define ROI_SET_END_TIMEOUT			(5000)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gRoiStatus = 0;		// ROI Status


//**********************************************************************************
// ROI Set Selector
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetSelector (int selector)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check selector Parameter
	if ((selector < ROI_SELECTOR_MIN) || (selector > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Set Selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Selector設定
	OUT32 (FIRM_DATA_ROI_SELECTOR_ADRS, selector);

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Get Selector
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSelector			：セレクタを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetSelector (int *pSelector)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pSelector Parameter
	if (pSelector == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Selector pSelector NULL Parameter Error.\n");
		goto _DONE;
	}

	// 取得
	*pSelector = IN32 (FIRM_DATA_ROI_SELECTOR_ADRS);

_DONE:
	return (status);
}
#endif // #defined (MODE_ROI_VERSION2)


#if defined (MODE_ROI_VERSION2)
//**********************************************************************************
// ROI初期化設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int xOffset, xSize;
	int yOffset, ySize;
	int width, height;
	int valid;
	int count;
#if defined(MODE_SPECTRUM)
	int spectrumOffset = 0;
#endif

	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "ROI Initialize\n");
	
	// セレクタ初期化
	OUT32 (FIRM_DATA_ROI_SELECTOR_ADRS, ROI_SELECTOR_MIN);

	// Area Mode初期化
	OUT32 (FIRM_DATA_ROI_AREA_MODE_ADRS, ROI_AREA_MODE_DEFAULT_SIZE);

	// Save Restore Flag初期化
	OUT32 (FIRM_DATA_ROI_SAVE_RESTORE_ADRS, 0);

	// Camera Width Total初期化
	width = WidthMax();
	OUT32 (FIRM_DATA_ROI_CAMERA_WIDTH_TOTAL_ADRS, width);

	// Camera Height Total 初期化
	height = HeightMax();
	OUT32 (FIRM_DATA_ROI_CAMERA_HEIGHT_TOTAL_ADRS, height);

	// Sensor Width Total初期化
	width = sensorWidth();
	OUT32 (FIRM_DATA_ROI_SENSOR_WIDTH_TOTAL_ADRS, width);

	// Sensor Height Total 初期化
	height = sensorHeight();
	OUT32 (FIRM_DATA_ROI_SENSOR_HEIGHT_TOTAL_ADRS, height);

	// ROI一時領域初期化
	memset ((void *)FIRM_DATA_ROI_BASE_ADRS, 0x00, FIRM_DATA_ROI_BASE_SIZE);

	// ROI Count設定
	roiSetEntryCount (1);

	//--------------------------------------------------------------------------------
	// Register Data Restore(Height)
	//--------------------------------------------------------------------------------
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_ROI_HEIGHT_ADRS, CAMERA_SAVE_ROI_HEIGHT_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gRoiStatus = status;
		goto _DONE;
	}

	#if defined (MODE_IPU_MULTI)
	//--------------------------------------------------------------------------------
	// Register Data Restore(Height)
	//--------------------------------------------------------------------------------
	if ((status = cameraParamWriteRegisterOffsetAdrs (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_ROI_HEIGHT_ADRS, CAMERA_SAVE_ROI_HEIGHT_FPGA_SIZE, FPGA_ROI2_OFFSET)) != AVAL_STATUS_SUCCESS)
	{
		gRoiStatus = status;
		goto _DONE;
	}
	#endif

	//--------------------------------------------------------------------------------
	// Register Data Restore(Width)
	//--------------------------------------------------------------------------------
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_ROI_WIDTH_ADRS, CAMERA_SAVE_ROI_WIDTH_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gRoiStatus = status;
		goto _DONE;
	}

	#if defined (MODE_IPU_MULTI)
	//--------------------------------------------------------------------------------
	// Register Data Restore(Width)
	//--------------------------------------------------------------------------------
	if ((status = cameraParamWriteRegisterOffsetAdrs (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_ROI_WIDTH_ADRS, CAMERA_SAVE_ROI_WIDTH_FPGA_SIZE, FPGA_ROI2_OFFSET)) != AVAL_STATUS_SUCCESS)
	{
		gRoiStatus = status;
		goto _DONE;
	}

	// 従来のROIは固定値を設定
	OUT32 (FPGA_ROI_CAMERA_X_ADRS, DEFAULT_ROI_WIDTH0_LEFT);
	OUT32 (FPGA_ROI2_CAMERA_X_ADRS, DEFAULT_ROI_WIDTH0_RIGHT);
	#endif

	// 設定通知
	if ((gRoiStatus = roiSetEnd ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Spectrum Offset
	//--------------------------------------------------------------------------------
#if defined(MODE_SPECTRUM)
	// 先頭オフセット検索
	if((gRoiStatus = spectrumSearchBandOffset()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 先頭オフセット取得
	if((gRoiStatus = spectrumGetBandOffset(&spectrumOffset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//--------------------------------------------------------------------------------
	// Sensor Width
	//--------------------------------------------------------------------------------
	xOffset = 0;
	xSize = IMG_WIDTH;
	valid = 1;

	// Sensor Widthレジスタ設定
	if ((gRoiStatus = sensorRoiSetWidth (0, xOffset, xSize, valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// センサレジスタに設定Height
	//--------------------------------------------------------------------------------
	for (selector = 0; selector < ROI_SELECTOR_COUNT; selector++)
	{
		// FPGA Heightレジスタ取得
		if ((gRoiStatus = fpgaRoiGetSensorHeight (selector, &yOffset, &ySize, &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 無効なら未設定
		if (valid == 0)
			break;

		// Sensor Heightレジスタ設定
		if ((gRoiStatus = sensorRoiSetHeight (selector, yOffset, ySize, valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//--------------------------------------------------------------------------------
	// 何個ROIが設定されているかを判断
	//--------------------------------------------------------------------------------
	count = 0;
	for (selector = 0; selector < ROI_SELECTOR_COUNT; selector++)
	{
		// Get Height(Memory)
		if ((status = memRoiGetCameraHeight (selector, &yOffset, &ySize, &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 無効なら未設定
		if (valid == 0)
			continue;

		count++;
	}

	// 登録数設定
	roiSetEntryCount (count);

#if defined (MODE_SPECTRUM_BANDGAIN_FILTER)
	//バンドインデックス配列初期化
	if ((status = spectrumSetBandIndex()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//ゲインバンドインデックス設定
	if ((status = bgfSetBandGainIndex()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

_DONE:
	return (status);
}


#if defined (MODE_IPU_MULTI)
//**********************************************************************************
// ROI 設定完了通知
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetEnd (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data1, data2, errStatus;
	int i;

	// 設定完了通知
	OUT32 (FPGA_ROI_SET_END_ADRS, FPGA_ROI_SET_END_BIT);
	OUT32 (FPGA_ROI2_SET_END_ADRS, FPGA_ROI_SET_END_BIT);

	for (i=0; i<ROI_SET_END_TIMEOUT; i++)
	{
		data1 = IN32 (FPGA_ROI_CUL_END_ADRS);
		data2 = IN32 (FPGA_ROI2_CUL_END_ADRS);
		if ((data1 & FPGA_ROI_CUL_END_BIT) && (data2 & FPGA_ROI_CUL_END_BIT))
			break;

		msDelay (1);
	}

	// Check Timeout
	if (i >= ROI_SET_END_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Set End Timeout Error.\n");
		goto _DONE;
	}

	// エラーCheck
	for (i=0; i<IPU_COUNT; i++)
	{
		if ((status = roiGetErrStatusMulti (i, &errStatus)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// エラーあり?
		if (errStatus != 0)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "ROI Error = 0x%x\n", errStatus);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

_DONE:
	return (status);
}

#else // #if defined (MODE_IPU_MULTI)

//**********************************************************************************
// ROI 設定完了通知
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetEnd (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, errStatus;
	int i;

	// 設定完了通知
	OUT32 (FPGA_ROI_SET_END_ADRS, FPGA_ROI_SET_END_BIT);

	for (i=0; i<ROI_SET_END_TIMEOUT; i++)
	{
		data32 = IN32 (FPGA_ROI_CUL_END_ADRS);
		if (data32 & FPGA_ROI_CUL_END_BIT)
			break;

		msDelay (1);
	}

	// Check Timeout
	if (i >= ROI_SET_END_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Set End Timeout Error.\n");
		goto _DONE;
	}

	// エラーCheck
	if ((status = roiGetErrStatus (&errStatus)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// エラーあり?
	if (errStatus != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Error = 0x%x\n", errStatus);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}
#endif // #if defined (MODE_IPU_MULTI)


//**********************************************************************************
// ROI エラー取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStat				：エラー情報を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetErrStatus (unsigned int *pStat)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	unsigned int vol, vop, hop;

	// Check pStat Parameter
	if (pStat == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Check Error pStat NULL Parameter Error.\n");
		goto _DONE;
	}

	// エラー取得
	data32 = IN32 (FPGA_ROI_CAMERA_ERROR_ADRS);

	// VOL取得
	vol = FPGA_ROI_CAMERA_ERROR_GET_VOL (data32);
	if (vol != 0)
	{
		sprintf (gLogMsgBuff, "ROI Vertical OverLapping Error. Error = 0x%x\n", vol);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	}

	// VOP取得
	vop = FPGA_ROI_CAMERA_ERROR_GET_VOP (data32);
	if (vop != 0)
	{
		sprintf (gLogMsgBuff, "ROI Vertical Out of Active Pixel. Error = 0x%x\n", vop);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	}

	// HOP取得
	hop = FPGA_ROI_CAMERA_ERROR_GET_HOP (data32);
	if (hop != 0)
	{
		sprintf (gLogMsgBuff, "ROI Horizontal Out of Active Pixel. Error = 0x%x\n", hop);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	}

	// エラー情報格納
	*pStat = data32;

_DONE:
	return (status);
}


#if defined (MODE_IPU_MULTI)
//**********************************************************************************
// ROI エラー取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//		pStat				：エラー情報を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetErrStatusMulti (int selector, unsigned int *pStat)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	unsigned int vol, vop, hop;
	unsigned int adrs;

	// Check selector Parameter
	if ((selector < 0) || (selector >= IPU_COUNT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Check Error selector(%d) Parameter Error.(Min=0 / Max=%d)\n", IPU_COUNT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pStat Parameter
	if (pStat == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Check Error pStat NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Adrs
	if (selector == 0)
		adrs = FPGA_ROI_CAMERA_ERROR_ADRS;
	else
		adrs = FPGA_ROI2_CAMERA_ERROR_ADRS;

	// エラー取得
	data32 = IN32 (adrs);

	// VOL取得
	vol = FPGA_ROI_CAMERA_ERROR_GET_VOL (data32);
	if (vol != 0)
	{
		sprintf (gLogMsgBuff, "ROI Vertical OverLapping Error. Error = 0x%x\n", vol);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	}

	// VOP取得
	vop = FPGA_ROI_CAMERA_ERROR_GET_VOP (data32);
	if (vop != 0)
	{
		sprintf (gLogMsgBuff, "ROI Vertical Out of Active Pixel. Error = 0x%x\n", vop);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	}

	// HOP取得
	hop = FPGA_ROI_CAMERA_ERROR_GET_HOP (data32);
	if (hop != 0)
	{
		sprintf (gLogMsgBuff, "ROI Horizontal Out of Active Pixel. Error = 0x%x\n", hop);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	}

	// エラー情報格納
	*pStat = data32;

_DONE:
	return (status);
}
#endif // #if defined (MODE_IPU_MULTI)


//**********************************************************************************
// ROI Set Entry Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：登録数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetEntryCount (int count)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check count Parameter
	if ((count < ROI_SELECTOR_MIN) || (count > (ROI_SELECTOR_MAX+1)))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Entry count(%d) Parameter Error.(Min:%d / Max:%d)\n", count, ROI_SELECTOR_MIN+1, ROI_SELECTOR_MAX+1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 登録数設定
	OUT32 (FIRM_DATA_ROI_COUNT_ADRS, count);

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Get Entry Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：登録数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetEntryCount (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Entry Count pCount NULL Parameter Error.\n");
		goto _DONE;
	}

	// 登録数取得
	*pCount = IN32 (FIRM_DATA_ROI_COUNT_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Set Valid
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		valid				：ROI有効／無効の設定
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetValid (int valid)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int tempValid;
	int offset, size;

	// Get Selector
	if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Height(Memory)
	if ((status = memRoiGetCameraHeight (selector, &offset, &size, &tempValid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((valid == 1) && (tempValid == 0))	// 今回有効要求、且つ今までは無効状態
	{
		// 一致するものを検索
		//if ((status = roiGetHeightIndex (&index, offset, size)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// Get Height(FPGA)
		//if ((status =fpgaRoiGetCameraHeight (index, &offset, &size, &tempValid)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		offset -= IMG_HEIGHT_OFFSET;

		// メモリ領域を無効化にしてから以下で登録作業(Valid無効はFPGA&センサ領域は未登録)
		if ((status =memRoiSetCameraHeight (selector, 0, 0, 0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 登録
		if ((status =roiSetHeightMain (offset, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if ((valid == 0) && (tempValid == 1))	// 今回無効要求、且つ今までは有効状態
	{
		// 無効要求の場合は削除
		if ((status =roiSetHeightMain (0, 0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Set Height Valid(Memory)
		if ((status =memRoiSetCameraHeight (selector, offset, size, 0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	//if ((valid == 1) && (tempValid == 1))	// 今回有効要求、且つ今までは有効状態 => すでに登録済みの為、何もしない
	//if ((valid == 0) && (tempValid == 0))	// 今回無効要求、且つ今までは無効状態 => 登録されてない為、何もしない

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Get Valid
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pValid				：ROI有効／無効を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetValid (int *pValid)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int offset, size;

	// Check pValid Parameter
	if (pValid == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Valid pValid NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Selector
	if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set Width Valid
	//if ((status = memRoiSetValid (selector, SENSOR_REG_ROI_WIDTH_VALID, valid)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// Get Height Valid
	//if ((status = memRoiGetValid (selector, SENSOR_REG_ROI_HEIGHT_VALID, pValid)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// Get Height(Memory)
	if ((status =memRoiGetCameraHeight (selector, &offset, &size, pValid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Widthサイズ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：Widthサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetWidth (int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int widthMinSize, widthMaxSize;
	int rOffset, rSize;
	int offset;
	int valid;
	int mode = MODE_DISABLE;
#if defined (MODE_CXP)
	int cxpPort = 0;
#endif
	int binningNum;
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	int abMode = AUTO_BRIGHT_TARGET_AREA_ALL_RANGE;
#endif

	// Width Min取得
	widthMinSize = ROI_WIDTH_ALIGH;

	// Width Max取得
	widthMaxSize = WidthMax();


#if defined(MODE_BINNING)
	if ((status = aoiGetBinningX (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode != MODE_DISABLE)
		size *= mode;
#endif

	if (mode == MODE_DISABLE)
		binningNum = 1;
	else
		binningNum = mode;

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// Check size Parameter
		if ((size < CXP_WIDTH_MIN) || (size > widthMaxSize))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Width Set Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size / binningNum, CXP_WIDTH_MIN, widthMaxSize / binningNum);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
	else
#endif // #if defined (MODE_CXP)
	{
		// Check size Parameter
		if (((size/binningNum) < widthMinSize) || (size > widthMaxSize))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Width Set Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size / binningNum, widthMinSize, widthMaxSize / binningNum);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

	// Check size Align Parameters
	if ((size % ROI_FPGA_WIDTH_SIZE_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Width Set Size Align(%d) Parameter Error. Align = %d.\n", size / binningNum, ROI_FPGA_WIDTH_SIZE_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Align Parameters
	if (((size/binningNum) % ROI_FPGA_WIDTH_SIZE_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Width Set Size Align(%d) Parameter Error. Align = %d.\n", size / binningNum, ROI_FPGA_WIDTH_SIZE_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if defined(MODE_BINNING)
	if (gInterFaceID != INTERFACE_CXP)
	{
		// Check size Even number Parameters
		if ((size % 2) != 0)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Width Set Even number(%d) Parameter Error.\n", size);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
#endif

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// Check Size
		if (size < CXP_WIDTH_MIN)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Width Set Size(%d) Parameter Error. (Min:%d / Max:%d)\n", size / binningNum, CXP_WIDTH_MIN, widthMaxSize / binningNum);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// CXPは32画素単位にする
		if ((size % CXP_WIDTH_ALIGH) != 0)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Width Set Size Align(%d) Parameter Error. Align = %d.\n",  size / binningNum, CXP_WIDTH_ALIGH);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}


	#if defined(MODE_BINNING)
		// 最小画素は128はCXP側の制限。
		// ビニング時は2倍した256が最小。
		// センサは256画素の画素を取得し、ビニングで128画素に縮小。
		// CXPで転送する最小サイズは128画素となるのでビニング時は2倍。

		if(mode == MODE_ENABLE)
		{
			// Check Size
			if (size < (CXP_WIDTH_MIN * binningNum))
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "Width Set Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size / binningNum, CXP_WIDTH_MIN * binningNum, widthMaxSize / binningNum);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// ビニング時はbinningNum画素単位にする
			if ((size % (CXP_WIDTH_ALIGH * binningNum)) != 0)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "Width Set Size Align(%d) Parameter Error. Align = %d.\n", size / binningNum, CXP_WIDTH_ALIGH * binningNum);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

		}
	#endif	// #if defined(MODE_BINNING)
	}
#endif

	// WidthはSelector=0のみ
	selector = 0;

	// Get Width
	if ((status = memRoiGetCameraWidth (selector, &rOffset, &rSize, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// オフセット格納
	if (rOffset < IMG_WIDTH_OFFSET)
	{
		rOffset = 0;
	}
	else
	{
		#if !defined (MODE_IPU_MULTI)
		rOffset -= IMG_WIDTH_OFFSET;
		#endif
	}

	// オフセット設定
	offset = rOffset;

	// rOffset取得値異常
	if ((offset < 0) || (offset > widthMaxSize - ROI_FPGA_WIDTH_OFFSET_ALIGH))
		offset = 0;

	// Check Offset
	if ((size + offset) > widthMaxSize)
		offset = widthMaxSize - size;

	// 設定
	if ((status = roiSetWidthMain (offset, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	// Get TargetArea
	if((status = autoBrightGetTargetArea (&abMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set TargetArea
	if((status = autoBrightSetTargetArea (abMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		#if defined(MODE_BINNING)
		if(mode != MODE_DISABLE)
		{
			size /= mode;
			offset /= mode;
		}
		#endif // #if defined(MODE_BINNING)


		#if !defined (MODE_CXP_MULTI_PORT)

		// Width設定
		if ((status = cxpSetWidth (cxpPort, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offsetx設定
		if ((status = cxpSetOffsetX (cxpPort, offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#else // #if !defined (MODE_CXP_MULTI_PORT)

		// CXP Port取得
		if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Width設定
		if ((status = cxpSetWidth (cxpPort, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offsetx設定
		if ((status = cxpSetOffsetX (cxpPort, offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#endif // #if !defined (MODE_CXP_MULTI_PORT)
	}
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// Widthサイズ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSize				：Widthサイズを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetWidth (int *pSize)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int rOffset, rSize;
	int valid;
#if defined(MODE_BINNING)
	int mode = MODE_DISABLE;
#endif

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Width Get pSize NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Selector
	if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Width Offset
	if ((status = memRoiGetCameraWidth (selector, &rOffset, &rSize, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// サイズ格納
	*pSize = rSize;

#if defined(MODE_BINNING)

	if ((status = aoiGetBinningX (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		// rSizeはスルー
		if(rSize <= (mode-1))
			goto _DONE;

		if((rSize % mode) == 0)
			*pSize /= mode;
		else
			*pSize = (rSize & ~(mode - 1)) / mode;
	}

#endif

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Widthオフセット設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		offset				：Widthオフセット
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetWidthOffset (int offset)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int widthMaxSize;
	int rOffset, rSize;
	int size;
	int valid;
	int mode = MODE_DISABLE;
	int binningNum;
#if defined(MODE_BINNING) && defined (MODE_CXP)
	int offsetHalf, maxSizeHalf;
#endif
#if defined (MODE_CXP)
	int cxpPort = 0;
#endif

	// Width Max取得
	widthMaxSize = WidthMax();

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningX (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//ビニング有効時の設定値は倍
	if(mode != MODE_DISABLE)
		offset *= mode;
#endif

	if (mode == MODE_DISABLE)
		binningNum = 1;
	else
		binningNum = mode;

#if defined (MODE_CXP)
	// Check offset Parameter
	if (gInterFaceID == INTERFACE_CXP)
	{
		if ((offset < 0) || (offset > widthMaxSize-CXP_WIDTH_MIN))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Width Set Offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset / binningNum, 0, (widthMaxSize / binningNum) - CXP_WIDTH_MIN);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
	else
#endif
	{
		if ((offset < 0) || (offset > (widthMaxSize-ROI_FPGA_WIDTH_SIZE_ALIGH*binningNum)))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Width Set Offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset / binningNum, 0, (widthMaxSize / binningNum) - ROI_FPGA_WIDTH_SIZE_ALIGH);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

#if defined(MODE_BINNING)
	if (gInterFaceID != INTERFACE_CXP)
	{
		// Check size Even number Parameters
		if ((offset % mode) != 0)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Width Set Offset Even number(%d) Parameter Error.\n", offset / binningNum);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}		
	}
#endif // #if defined(MODE_BINNING)

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		#if defined(MODE_BINNING)
		if(mode != MODE_DISABLE)
		{
			// Check offset Parameter
			offsetHalf = offset / mode;
			maxSizeHalf = (widthMaxSize/mode) - CXP_WIDTH_MIN;

			if (offsetHalf > maxSizeHalf)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "Width Set Offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offsetHalf, 0, maxSizeHalf);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// CXPは64画素単位にする
			if ((offset % (CXP_WIDTH_ALIGH * binningNum)) != 0)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "Width Set Offset Align(%d) Parameter Error. Align = %d.\n", offset / binningNum, CXP_WIDTH_ALIGH);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}
		#endif // #if defined(MODE_BINNING)

		// CXPは32画素単位にする
		if ((offset % (CXP_WIDTH_ALIGH)) != 0)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Width Set Offset Align(%d) Parameter Error. Align = %d.\n", offset / binningNum, CXP_WIDTH_ALIGH);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
#endif	// #if defined (MODE_CXP)

	// Check offset Align Parameters
	if (((offset / binningNum) % ROI_FPGA_WIDTH_OFFSET_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Width Set Offset Align(%d) Parameter Error. Align = %d.\n", offset / binningNum, ROI_FPGA_WIDTH_OFFSET_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// WidthはSelector=0のみ
	selector = 0;

	// Get Width
	if ((status = memRoiGetCameraWidth (selector, &rOffset, &rSize, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// サイズ設定
	size = rSize;

	// rSize取得値異常
	if ((rSize <= 0) || (rSize > widthMaxSize))
		size = widthMaxSize;

	// Check Offset
	if ((offset + size) > widthMaxSize)
		size = widthMaxSize - offset;

	// 設定
	if ((status = roiSetWidthMain (offset, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		#if defined(MODE_BINNING)
		if(mode != MODE_DISABLE)
		{
			size /= mode;
			offset /= mode;
		}
		#endif // #if defined(MODE_BINNING)

		#if !defined (MODE_CXP_MULTI_PORT)

		// Width設定
		if ((status = cxpSetWidth (cxpPort, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offsetx設定
		if ((status = cxpSetOffsetX (cxpPort, offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#else // #if !defined (MODE_CXP_MULTI_PORT)

		// CXp Port取得
		if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Width設定
		if ((status = cxpSetWidth (cxpPort, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offsetx設定
		if ((status = cxpSetOffsetX (cxpPort, offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#endif // #if !defined (MODE_CXP_MULTI_PORT)
	}
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// Widthオフセット取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pOffset				：Witdhオフセットを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetWidthOffset (int *pOffset)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int rOffset, rSize, tmpOffset;
	int valid;
#if defined(MODE_BINNING)
	int mode = MODE_DISABLE;
#endif

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Width Get pOffset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Selector
	if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Width
	if ((status = memRoiGetCameraWidth (selector, &rOffset, &rSize, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// オフセット格納
	if (rOffset < IMG_WIDTH_OFFSET)
	{
#if !defined (MODE_IPU_MULTI)
		tmpOffset = 0;
#else
		tmpOffset = rOffset;
#endif
	}
	else
	{
#if !defined (MODE_IPU_MULTI)
		tmpOffset = rOffset - IMG_WIDTH_OFFSET;
#else
		tmpOffset = rOffset;
#endif
	}

	*pOffset = tmpOffset;

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningX (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		if(tmpOffset <= 0)
			goto _DONE;

		if(tmpOffset % mode == 0)
			*pOffset = tmpOffset / mode;
		else
			//奇数なら-1
			*pOffset = (tmpOffset & ~(mode - 1)) / mode;
	}
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Width Main
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		offset				：Widthオフセット
//		size				：Widthサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetWidthMain (int offset, int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int rOffsetFpga, rSizeFpga;
	int rOffsetMem, rSizeMem;
	int rOffsetSensor, rSizeSensor;
	int fpgaOffset, fpgaSize;
	int valid;
	int startMode = 0;
	int settingError = 0;

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	acquisitionAbort ();

	// WidthはSelector=0のみ
	selector = 0;

	// FPGA設定値をリカバリ用に取得
	if ((status = fpgaRoiGetCameraWidth (selector, &rOffsetFpga, &rSizeFpga, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Memory設定値をリカバリ用に取得
	if ((status = memRoiGetCameraWidth (selector, &rOffsetMem, &rSizeMem, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor設定値をリカバリ用に取得
	if ((status = sensorRoiGetWidth (selector, &rOffsetSensor, &rSizeSensor, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FPGA Camera Widthのレジスタは１つのみ
#if !defined (MODE_IPU_MULTI)
	fpgaOffset = offset + IMG_WIDTH_OFFSET;
#else
	fpgaOffset = offset;
#endif

	fpgaSize = size;

	if ((status = fpgaRoiSetCameraWidth (selector, fpgaOffset, fpgaSize, 1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Memory Camera Widthのレジスタは１つのみ
	if ((status = memRoiSetCameraWidth (selector, fpgaOffset, fpgaSize, 1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 登録設定
	if ((status = roiSetEnd()) != AVAL_STATUS_SUCCESS)
	{
		settingError = 1;
		goto _DONE;
	}

_DONE:
	if ((status != AVAL_STATUS_SUCCESS) && (settingError == 1))
	{
		// FPGA Camera Width
		fpgaRoiSetCameraWidth (0, rOffsetFpga, rSizeFpga, 1);

		// Memory Camera Width
		memRoiSetCameraWidth (0, rOffsetMem, rSizeMem, 1);

		// 設定
		roiSetEnd();

		// Sensor Width
		//sensorRoiSetWidth (0, rOffsetSensor, rSizeSensor);
	}

	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// ROI Heightサイズ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：Heightサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiSetHeight (int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int heightMinSize, heightMaxSize;
	int rOffset, rSize;
	int offset;
	int valid;
	int mode = MODE_DISABLE;
	int binningNum;
	int startMode = 0;
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	int abMode = AUTO_BRIGHT_TARGET_AREA_ALL_RANGE;
#endif
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode, tges;
	int heightCurrent, lineStart;
	int ffccormode;
	int binningy;
#endif
#if defined (MODE_CXP)
	int cxpPort = 0;
#endif
#if defined (MODE_SENSOR_DRRS)
	int drrsMode = MODE_DISABLE;
	double frameRateSave;
	double frameRateMax, frameRateMin;
#endif

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (drrsMode == MODE_ENABLE)
	{
		if ((status = irGetFrameRateShutterNormal (&frameRateSave)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// Get Selector
	if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Min取得
	heightMinSize = 0;

	// Height Max取得
	heightMaxSize = HeightMax();

	// Height Max取得
	//heightMaxSize = HeightMax();

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//ビニング有効時の設定値は倍
	if(mode != MODE_DISABLE)
		size *= mode;
#endif

	if (mode == MODE_DISABLE)
	{
		binningNum = 1;
	}
	else
	{
		binningNum = mode;
	}

	// Check size Parameter
	if ((size < heightMinSize) || (size > heightMaxSize))	// height = 0はエラーとしない。マルチroi時に削除する場合がある為
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Height Set Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size / binningNum, heightMinSize, heightMaxSize / binningNum);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Align Parameters
	if ((size % ROI_FPGA_HEIGHT_SIZE_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Height Set Size Align(%d) Parameter Error. Align = %d.\n", size / binningNum, ROI_FPGA_HEIGHT_SIZE_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// Check size 4Line単位
		if ((size % CXP_HEIGHT_ALIGH) != 0)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Height Set Size Align(%d) Parameter Error. Align = %d.\n", size / binningNum, CXP_HEIGHT_ALIGH);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

	#if defined(MODE_BINNING)
		if(mode != MODE_DISABLE)
		{
			if (IN32 (FIRM_DATA_HIGHSPPED_INIT2_ADRS) == 0)
			{
				// ビニング時は4 * mode Line単位にする
				if ((size % (CXP_HEIGHT_ALIGH * binningNum)) != 0)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
					sprintf (gLogMsgBuff, "Height Set Size Align(%d) Parameter Error. Align = %d.\n", size / binningNum, CXP_HEIGHT_ALIGH);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}
	#endif	// #if defined(MODE_BINNING)

	}
#endif

#if defined (MODE_FRAMERATE_HIGH_SPEED)
	// High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode(&hsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (hsMode == MODE_ENABLE)
	{
		// BinningY
		if ((status = aoiGetBinningY (&binningy)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((size == 2) || (size == 4) || (size == 6))
		{
		}
		else
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);

			if (binningy == 0)
				sprintf (gLogMsgBuff, "Height Set Size (%d) Parameter Error. Height = 2 or 4 or 6\n", size);
			else
				sprintf (gLogMsgBuff, "Height Set Size (%d) Parameter Error. Height = 1 or 2 or 3\n", size);

			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

			goto _DONE;
		}

		// 現在のサイズを取得
		if ((status =aoiGetHeight (&heightCurrent)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// Get Height(オフセットは未設定の場合0のはず)
	if ((status = memRoiGetCameraHeight (selector, &rOffset, &rSize, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Size=0が指定され、offsetが0以外、validが0の場合（無効化状態の削除）
    // すでにFPGAのレジスタは削除されている為、メモリテーブルのみ初期化
	if (size == 0)
	{
		if ((rOffset != 0) && (valid == 0))
		{
			// 1度Validを無効にする
			if ((status = memRoiSetCameraHeight (selector, 0, 0, 0)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			goto _DONE;
		}
	}

	// オフセット格納
	if (rOffset < IMG_HEIGHT_OFFSET)
		rOffset = 0;
	else
		rOffset -= IMG_HEIGHT_OFFSET;

	// オフセット設定
	offset = rOffset;
	
	// まだ未設定?
	//if ((rOffset == 0) && (rSize == 0))
	//{
		//rOffset = 0;
		//rSize = heightMaxSize;
	//}

	// rOffset取得値異常
	if ((offset < 0) || (offset > heightMaxSize - ROI_FPGA_HEIGHT_OFFSET_ALIGH))
		offset = 0;

	// rSize取得値異常
	//if ((rSize < 0) || (rSize > heightMaxSize))
		//rSize = heightMaxSize;

	// Check Offset
	if ((size + offset) > heightMaxSize)
		offset = heightMaxSize - size;

	// 設定
	if ((status = roiSetHeightMain (offset, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	// Get TargetArea
	if((status = autoBrightGetTargetArea (&abMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set TargetArea
	if((status = autoBrightSetTargetArea (abMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif


#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// CXP Height Param設定
		#if !defined (MODE_CXP_MULTI_PORT)

		if ((status = cxpSetHeightParam (cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#else // #if !defined (MODE_CXP_MULTI_PORT)

		if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((status = cxpSetHeightParam (cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#endif // #if !defined (MODE_CXP_MULTI_PORT)
	}
#endif


#if defined (MODE_FRAMERATE_HIGH_SPEED)
	if (hsMode == MODE_ENABLE)
	{
		// TGES
		if (size == 4)
			tges = SENSOR_READOUT_TIME_LNINE4_HIGH_SPEED;
		else if (size == 6)
			tges = SENSOR_READOUT_TIME_LNINE6_HIGH_SPEED;
		else
			tges = SENSOR_READOUT_TIME_LNINE2_HIGH_SPEED;

		if ((status = tgSetTges (tges)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&ffccormode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (binningy != 0)
			heightCurrent *= binningy;

		// FFC Line Start取得
		if ((status = ffcGetReplacementLine (&lineStart, heightCurrent, ffccormode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFCデータ入れ替え(元の状態に戻す)
		if ((status = ffcDataReplacement (lineStart, heightCurrent, ffccormode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC Line Start取得
		if ((status = ffcGetReplacementLine (&lineStart, size, ffccormode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFCデータ入れ替え(元の状態から変更した状態に入れ替え)
		if ((status = ffcDataReplacement (lineStart, size, ffccormode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

#if defined (MODE_SENSOR_DRRS)
	if (drrsMode == MODE_ENABLE)
	{
		// 最大フレームレート取得
		if ((status = rateMax (&frameRateMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 最小フレームレート取得
		if ((status = rateMin (&frameRateMin)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 最大フレームレート確認
		if (frameRateMax < frameRateSave)
			frameRateSave = frameRateMax - 0.2;

		// 最小フレームレート確認
		if (frameRateMin > frameRateSave)
			frameRateSave = frameRateMin + 0.2;
		
		// フレームレート設定
		if ((status = irSetFrameRateDrrs (frameRateSave)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// Heightサイズ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSize				：Heightサイズを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetHeight (int *pSize)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int rOffset, rSize;
	int valid;
#if defined(MODE_BINNING)
	int mode = MODE_DISABLE;
#endif

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Height Get pSize NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Selector
	if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Height
	if ((status = memRoiGetCameraHeight (selector, &rOffset, &rSize, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// サイズ格納
	*pSize = rSize;

#if defined(MODE_BINNING)

	if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		//1以下はスルー
		if(rSize <= (mode-1))
			goto _DONE;

		if(mode == 3)
		{
			// 切り上げ
			*pSize= (rSize + 2) / 3;
			*pSize += (rSize % 3) != 0 ? 1 : 0;
		}
		else
		{
			//偶数なら半分
			if((rSize % mode) == 0)
				*pSize = rSize / mode;
			else
				//奇数なら-1
				*pSize = (rSize & ~(mode -1)) / mode;
		}
	}

#endif

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Heightオフセット設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		offset					：Heightオフセット
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int aoiSetHeightOffset (int offset)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int heightMaxSize;
	int rOffset, rSize;
	int size;
	int valid;
	int mode = MODE_DISABLE;
	int binningNum;
#if defined (MODE_CXP)
	int cxpPort = 0;
#endif
	
	// Height Max取得
	heightMaxSize = HeightMax();

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//ビニング有効時設定値は倍
	if(mode != MODE_DISABLE)
		offset *= mode;
#endif

	if (mode == MODE_DISABLE)
	{
		binningNum = 1;
	}
	else
	{
		binningNum = mode;
	}

	// Check offset Parameter
	if ((offset < 0) || (offset > heightMaxSize-ROI_FPGA_HEIGHT_SIZE_ALIGH))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Height Set Offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset / binningNum, 0, (heightMaxSize/binningNum)-ROI_FPGA_HEIGHT_SIZE_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset Align Parameters
	if ((offset % ROI_FPGA_HEIGHT_OFFSET_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Height Set Offset Align(%d) Parameter Error. Align = %d.\n", offset / binningNum, ROI_FPGA_HEIGHT_OFFSET_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// Check size 4Line単位
		if ((offset % CXP_HEIGHT_ALIGH) != 0)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Height Set Offset Align(%d) Parameter Error. Align = %d.\n", offset / binningNum, CXP_HEIGHT_ALIGH);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

	#if defined(MODE_BINNING)
		if(mode != MODE_DISABLE)
		{
			// ビニング時は4 * modeLine単位にする
			if ((offset% (CXP_HEIGHT_ALIGH * binningNum)) != 0)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "Height Set Offset Align(%d) Parameter Error. Align = %d.\n", offset / binningNum,  CXP_HEIGHT_ALIGH);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}
	#endif	// #if defined(MODE_BINNING)
	}
#endif

	// Get Selector
	if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Height
	if ((status = memRoiGetCameraHeight (selector, &rOffset, &rSize, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// まだ未設定?
	//if ((rOffset == 0) && (rSize == 0))
	//{
		//rOffset = 0;
		//rSize = heightMaxSize;
	//}

	// サイズ設定
	size = rSize;

	// rOffset取得値異常
	//if ((rOffset < 0) || (rOffset > heightMaxSize - ROI_FPGA_HEIGHT_OFFSET_ALIGH))
		//rOffset = 0;

	// rSize取得値異常
	if ((rSize < 0) || (rSize > heightMaxSize))
		size = heightMaxSize;

	// Check Offset
	if ((offset + size) > heightMaxSize)
		size = heightMaxSize - offset;

	// 設定
	if ((status = roiSetHeightMain (offset, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// CXP Height Param設定
		#if !defined (MODE_CXP_MULTI_PORT)

		if ((status = cxpSetHeightParam (cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#else // #if !defined (MODE_CXP_MULTI_PORT)

		if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((status = cxpSetHeightParam (cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#endif // #if !defined (MODE_CXP_MULTI_PORT)
	}
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// Heightオフセット取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pOffset				：Heightオフセットを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int aoiGetHeightOffset (int *pOffset)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int rOffset, rSize, tmpOffset;
	int valid;
#if defined(MODE_BINNING)
	int mode = MODE_DISABLE;
#endif

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Height Get pOffset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Selector
	if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Height
	if ((status = memRoiGetCameraHeight (selector, &rOffset, &rSize, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// オフセット格納
	if (rOffset < IMG_HEIGHT_OFFSET)
		tmpOffset = 0;
	else
		tmpOffset = rOffset - IMG_HEIGHT_OFFSET;

	*pOffset = tmpOffset;

#if defined(MODE_BINNING)

	//Binning Y 取得
	if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		if(tmpOffset <= 0)
			goto _DONE;

		if(mode == 3)
		{
			if(tmpOffset % mode == 0)
				*pOffset = (tmpOffset / mode);
			else
				// 切りあげ
				*pOffset= (tmpOffset + 2) / 3;
		}
		else
		{
			if(tmpOffset % mode == 0)
				*pOffset = tmpOffset / mode;
			else
				//奇数のときは-1
				*pOffset = (tmpOffset & ~(mode -1)) / mode;
		}
	}

#endif


_DONE:
	return (status);
}


//**********************************************************************************
// ROI Height Main
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		offset				：Heightオフセット
//		size				：Heightサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int rFpgaOffset[ROI_SELECTOR_COUNT], rFpgaSize[ROI_SELECTOR_COUNT], rFpgaValid[ROI_SELECTOR_COUNT];
int rMemOffset[ROI_SELECTOR_COUNT], rMemSize[ROI_SELECTOR_COUNT], rMemValid[ROI_SELECTOR_COUNT];
int rSensorOffset[ROI_SELECTOR_COUNT], rSensorSize[ROI_SELECTOR_COUNT], rSensorValid[ROI_SELECTOR_COUNT];

int roiSetHeightMain (int offset, int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector, selector2;
	int entryCount, entryCountSave;
	int currentOffset, currentSize;
	int valid;
	int rTempOffset, rTempSize;
	int startMode = 0;
	int settingError = 0;
	int rYOffset, rYSize, rValid;
	int i;
	int totalHeight;
	int tgpd;
	int rateMode;
	int trgMode, rsvMode;
	double frameMax, frameRate;
	int drrsMode = MODE_DISABLE;
#if defined (MODE_SENSOR_DRRS)
	int totalHeightSave = -1;
	unsigned int vmax;
	int id, acTmg, regHold;
	unsigned char data8;
#endif
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;
#endif

#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Selector
	if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 登録数の取得
	if ((status = roiGetEntryCount (&entryCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 登録数保存
	entryCountSave = entryCount;

	// 登録数は最大?
	if (entryCount > ROI_SELECTOR_COUNT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Height Set Entry=%d Check Max Error.(Min : %d / Max : %d)\n", entryCount, ROI_SELECTOR_MIN+1, ROI_SELECTOR_MAX+1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Memory Height
	if ((status = memRoiGetCameraHeight (selector, &rTempOffset, &rTempSize, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (valid == 1)
	{
		// これ以上は削除できない
		if ((size == 0) && (entryCount == 1))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "ROI Height Set Entry Delete Error. Entry Count = %d\n", entryCount);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

#if defined (MODE_SENSOR_DRRS)
	// HeightのTotal SizeをSave
	if (drrsMode == MODE_ENABLE)
	{
		if ((status = fpgaRoiGetSensorHeightTotalSize (&totalHeightSave)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// 設定値を保存
	for (selector2=0; selector2<entryCount; selector2++)
	{
		// FPGA Height
		if ((status = fpgaRoiGetCameraHeight (selector2, &rFpgaOffset[selector2], &rFpgaSize[selector2], &rFpgaValid[selector2])) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Memory Height
		if ((status = memRoiGetCameraHeight (selector2, &rMemOffset[selector2], &rMemSize[selector2], &rMemValid[selector2])) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Sensor Height
		if ((status = sensorRoiGetHeight (selector2, &rSensorOffset[selector2], &rSensorSize[selector2], &rSensorValid[selector2])) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// selector情報を取得
	if ((status = memRoiGetCameraHeight (selector, &currentOffset, &currentSize, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// エラーの場合は再設定
	settingError = 1;

	// 登録済みで指定サイズがsize=0の場合は削除
	if ((size == 0) && (valid == 1))
	{
		// 削除
		if ((status = roiHeightDel (selector, currentOffset, currentSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 削除
		if ((status = memRoiSetCameraHeight (selector, 0, 0, 0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// すでに設定されている?
		if (valid == 1)
		{
			if ((currentOffset != 0) || (currentSize != 0))
			{
				// 既存のものを更新。このため１回削除してから追加する。
				if ((status = roiHeightDel (selector, currentOffset, currentSize)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

				// カウント減算
				entryCount--;
				if ((status = roiSetEntryCount (entryCount)) != AVAL_STATUS_SUCCESS)
					goto _DONE;
			}
		}

		if ((offset == 0) && (size == 0))
		{
			goto _NEXT;
		}
		else
		{
			// 追加
			if ((status = roiHeightAdd (selector, offset, size)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

_NEXT:
	//--------------------------------------------------------------------------------
	// センサ側の設定
	//--------------------------------------------------------------------------------

	// ROI Set
	if ((status = roiSetEnd()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	for (i=0; i<ROI_SELECTOR_COUNT; i++)
	{
		// Sensor Height取得(FPGAのレジスタ側)
		if ((status = fpgaRoiGetSensorHeight (i, &rYOffset, &rYSize, &rValid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 有効なら設定
		if (rValid == 1)
		{
			// Sensor Height設定
			if ((status = sensorRoiSetHeight (i, rYOffset, rYSize, rValid)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		else
		{
			// Sensor Heightクリア
			if ((status = sensorRoiSetHeight (i, 0, 0, 0)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

	// カウント制御
	if ((size == 0) && (valid == 1))
	{
		// カウント減算
		if ((status = roiSetEntryCount (entryCount-1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// 登録数は最大?
		if (entryCount >= ROI_SELECTOR_COUNT)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "ROI Height Set Entry Count=%d Max Error.(Min : %d / Max : %d)\n", entryCount, ROI_SELECTOR_MIN+1, ROI_SELECTOR_MAX+1);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// カウント加算
		if (size != 0)
		{
			if ((status = roiSetEntryCount (entryCount+1)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

	// Memory Height設定
	//if ((status = memRoiSetCameraHeight (selector, offset, size)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// HeightのTotal Sizeを求める
	if ((status = fpgaRoiGetSensorHeightTotalSize (&totalHeight)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレートモード取得
	if ((status = acquisitionGetRateMode (&rateMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_DRRS)
	if (drrsMode == MODE_ENABLE)
	{
		// フレームレート設定(フレームレートはVMAXの値を設定)
		OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, ((totalHeight + SENSOR_DRRS_FIX_LINE) & GENICAM_ACQUISITION_FRAMERATE_MASK));
		
		id = 2;
		acTmg = 0;
		regHold = 0;

		// vmax設定
		vmax = totalHeight + SENSOR_DRRS_FIX_LINE;

		// VMAX1
		data8 = (unsigned char)(vmax & 0xff);
		if ((status = sensorRegWriteByte (id, SENSOR_REG_VMAX1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// VMAX2
		data8 = (unsigned char)((vmax >> 8) & 0xff);
		if ((status = sensorRegWriteByte (id, SENSOR_REG_VMAX2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// VMAX3
		data8 = (unsigned char)((vmax >> 16) & 0xff);
		if ((status = sensorRegWriteByte (id, SENSOR_REG_VMAX3_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// 最大フレームレート取得(内部で最大フレームレートを再計算)
	if ((status = rateMax (&frameMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (rateMode == MODE_EXTEND)
	{
#if defined (MODE_TRG_RESERVED_ADJUST)
		// Frameトリガモード取得
		if ((status = acquisitionGetFrameTrgMode (&trgMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 予約トリガモード取得
		if ((status = acquisitionGetTrgReserveMain (&rsvMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#else
		trgMode = MODE_DISABLE;
		rsvMode = MODE_DISABLE;
#endif

		// Frameトリガ無効時もしくは予約トリガ無効時のみTGPDを調整
		// Frameトリガ or 予約トリガ有効時は有効に設定したタイミングで専用の設定にしている
		if ((trgMode == MODE_DISABLE) || (rsvMode == MODE_DISABLE))
		{
#if defined (MODE_FRAMERATE_HIGH_SPEED)
			// Frame Rate High Speed Mode取得
			if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// TGPD設定
			if (hsMode == MODE_ENABLE)
			{
				// Get TGPD
				if ((status = sensorGetTgpdHighSpeed (size, &tgpd)) != AVAL_STATUS_SUCCESS)
					goto _DONE;
			}
			else
			{
				tgpd = totalHeight + /*SENSOR_HEIGHT_HIDE_LINE +*/ SENSOR_VINTERVAL_FIX_LINE;
			}

#else
			// TGPD設定
			tgpd = totalHeight + /*SENSOR_HEIGHT_HIDE_LINE +*/ SENSOR_VINTERVAL_FIX_LINE;
#endif

			if ((status = tgSetTgpd (tgpd)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

	if (drrsMode == MODE_DISABLE)
	{
		// フレームレート取得
		if ((status = irGetFrameRateMain (&frameRate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// フレームレートオーバー?
		if (frameRate > frameMax)
		{
			// フレームレート設定
			if ((status = acquisitionSetFrameRate (frameMax)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

_DONE:
	if ((status != AVAL_STATUS_SUCCESS) && (settingError == 1))
	{
		// FPGA Camera Height
		for (selector2=0; selector2<entryCountSave; selector2++)
		{
			if (rFpgaSize[selector2] == 0)
				valid = 0;
			else
				valid = 1;

			fpgaRoiSetCameraHeight (selector2, rFpgaOffset[selector2], rFpgaSize[selector2], rFpgaValid[selector2]);
		}

		// 設定
		roiSetEnd();

		// Memory Camera Height
		for (selector2=0; selector2<entryCountSave; selector2++)
		{
			if (rMemSize[selector2] == 0)
				valid = 0;
			else
				valid = 1;

			memRoiSetCameraHeight (selector2, rMemOffset[selector2], rMemSize[selector2], rMemSize[selector2]);
		}

		// Sensor Height
		for (selector2=0; selector2<entryCountSave; selector2++)
		{
			if (rSensorSize[selector2] == 0)
				valid = 0;
			else
				valid = 1;

			sensorRoiSetHeight (selector2, rSensorOffset[selector2], rSensorSize[selector2], rSensorValid[selector2]);
		}

		// カウント
		roiSetEntryCount (entryCountSave);

		#if defined (MODE_SENSOR_DRRS)
		if (drrsMode == MODE_ENABLE)
		{
			if (totalHeightSave != -1)
			{
				// フレームレート設定(フレームレートはVMAXの値を設定)
				OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, ((totalHeightSave + SENSOR_DRRS_FIX_LINE) & GENICAM_ACQUISITION_FRAMERATE_MASK));
				
				id = 2;
				acTmg = 0;
				regHold = 0;

				// vmax設定
				vmax = totalHeightSave + SENSOR_DRRS_FIX_LINE;

				// VMAX1
				data8 = (unsigned char)(vmax & 0xff);
				if ((status = sensorRegWriteByte (id, SENSOR_REG_VMAX1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

				// VMAX2
				data8 = (unsigned char)((vmax >> 8) & 0xff);
				if ((status = sensorRegWriteByte (id, SENSOR_REG_VMAX2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

				// VMAX3
				data8 = (unsigned char)((vmax >> 16) & 0xff);
				if ((status = sensorRegWriteByte (id, SENSOR_REG_VMAX3_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
					goto _DONE;
			}
		}
		#endif
	}

	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// ROI Height Add
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//		yOffset				：Yオフセット
//		ySize				：Yサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiHeightAdd (int selector, int yOffset, int ySize)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	int rYOffset, rYSize;
	int entryCount;
	int index;
	int valid;
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;
#endif

	// パラメータCheckは上記で実施済み

	// 登録個数の確認
	if ((status = roiGetEntryCount (&entryCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Entry Full?
	if (entryCount >= ROI_SELECTOR_COUNT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Add Full Entry(%d) Error.(Min : %d / Max : %d)\n", entryCount, ROI_SELECTOR_MIN, ROI_SELECTOR_COUNT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 追加する位置を検索
	if ((status = roiHeightPositionCheck (selector, yOffset, ySize, &index)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// オフセットだけの登録はメモリのみでFPGA&センサには設定しない
	if (ySize == 0)
		goto _NEXT;

	for (i=entryCount; i>=0; i--)
	{
		if (i == index)
		{
			//--------------------------------------------------------------------------------
			// 新規登録
			//--------------------------------------------------------------------------------
			if (ySize == 0)
				valid = 0;
			else
				valid = 1;

#if defined (MODE_FRAMERATE_HIGH_SPEED)
			// High Speed Mode取得
			if ((status = sensorGetFrameRateHighSpeedMode(&hsMode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			if (hsMode == MODE_ENABLE)
			{
				// FPGA Height設定
				if ((status = fpgaRoiSetCameraHeight (i, yOffset, ySize, valid)) != AVAL_STATUS_SUCCESS)
					goto _DONE;
			}
			else
			{
				// FPGA Height設定
				if ((status = fpgaRoiSetCameraHeight (i, yOffset+IMG_HEIGHT_OFFSET, ySize, valid)) != AVAL_STATUS_SUCCESS)
					goto _DONE;
			}
#else

			// FPGA Height設定
			if ((status = fpgaRoiSetCameraHeight (i, yOffset+IMG_HEIGHT_OFFSET, ySize, valid)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

#endif

			// Memory Height設定
			//if ((status = memRoiSetCameraHeight (i, yOffset+IMG_HEIGHT_OFFSET, ySize, valid)) != AVAL_STATUS_SUCCESS)
				//goto _DONE;

			break;	// コピー終了
		}
		else
		{
			//--------------------------------------------------------------------------------
			// 1個下にずらす
			//--------------------------------------------------------------------------------
			if (i == 0)
				continue;	// 先頭ならばコピーなし

			// FPGA Width取得
			//if ((status = fpgaRoiGetCameraWidth (i, &rXOffset, &rXSize)) != AVAL_STATUS_SUCCESS)
				//goto _DONE;

			// FPGA Width設定
			//if ((status = fpgaRoiSetCameraWidth (i+1, rXOffset, rXSize)) != AVAL_STATUS_SUCCESS)
				//goto _DONE;

			// Sensor Width設定
			//if ((status = sensorRoiGetWidth (i, &rXOffset, &rXSize)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

			// Sensor Width設定
			//if ((status = sensorRoiSetWidth (i+1, rXOffset, rXSize)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

			// FPGA Height取得
			if ((status = fpgaRoiGetCameraHeight (i-1, &rYOffset, &rYSize, &valid)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			if (rYSize  == 0)
				valid = 0;
			else
				valid = 1;

			// FPGA Height設定
			if ((status = fpgaRoiSetCameraHeight (i, rYOffset, rYSize, valid)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Memory Height設定
			//if ((status = memRoiSetCameraHeight (i, rYOffset, rYSize, valid)) != AVAL_STATUS_SUCCESS)
				//goto _DONE;
		}
	}

_NEXT:
	// Memory Height設定(メモリは指定selectorに格納)
	if (ySize == 0)
		valid = 0;
	else
		valid = 1;

#if defined (MODE_FRAMERATE_HIGH_SPEED)
	// High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode(&hsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (hsMode == MODE_ENABLE)
	{
		if ((status = memRoiSetCameraHeight (selector, yOffset, ySize, valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		if ((status = memRoiSetCameraHeight (selector, yOffset+IMG_HEIGHT_OFFSET, ySize, valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#else

	if ((status = memRoiSetCameraHeight (selector, yOffset+IMG_HEIGHT_OFFSET, ySize, valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#endif

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Height Delete
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//		yOffset				：Yオフセット
//		ySize				：Yサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiHeightDel (int selector, int yOffset, int ySize)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	int rYOffset, rYSize;
	//int rXOffset,rXSize;
	int entryCount;
	int index;
	int valid;

	// パラメータCheckは上記で実施済み

	// 登録個数の確認
	if ((status = roiGetEntryCount (&entryCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 一致するものを検索
	if ((status = roiGetHeightIndex (&index, yOffset, ySize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// 1個上にずらす
	//--------------------------------------------------------------------------------
	for (i=index; i<(entryCount-1); i++)
	{
		//--------------------------------------------------------------------------------
		// FPGA
		//--------------------------------------------------------------------------------

		// FPGA Width取得
		//if ((status = fpgaRoiGetCameraWidth (i+1, &rXOffset, &rXSize)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// FPGA Height取得
		if ((status = fpgaRoiGetCameraHeight (i+1, &rYOffset, &rYSize, &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FPGA Width設定
		//if ((status = fpgaRoiSetCameraWidth (i, rXOffset, rXSize)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		if (rYSize == 0)
			valid = 0;
		else
			valid = 1;

		// FPGA Height設定
		if ((status = fpgaRoiSetCameraHeight (i, rYOffset, rYSize, valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//--------------------------------------------------------------------------------
	// 終端をクリア
	//--------------------------------------------------------------------------------

	// FPGA Width設定
	//if ((status = fpgaRoiSetCameraWidth (i, 0, 0)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// Mem Height設定
	//if ((status = memRoiSetCameraHeight (i, 0, 0, 0)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// FPGA Height設定
	if ((status = fpgaRoiSetCameraHeight (i, 0, 0, 0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor FPGA Width設定
	//if ((status = sensorRoiSetWidth (i, 0, 0)) != AVAL_STATUS_SUCCESS)
	//goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Height Index
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pIndex				：インデックスを格納するポインタ
//		yOffset				：Yオフセット
//		ySize				：Yサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetHeightIndex (int *pIndex, int yOffset, int ySize)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	int rYOffset, rYSize;
	int entryCount;
	int valid;

	// パラメータCheckは上記で実施済み

	// Check pIndex Parameter
	if (pIndex == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Index pIndex NULL Parameter Error.\n");
		goto _DONE;
	}

	// 登録個数の確認
	if ((status = roiGetEntryCount (&entryCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// パラメータが一致するものを検索
	*pIndex = -1;
	for (i=0; i<entryCount; i++)
	{
		// Height取得
		if ((status = fpgaRoiGetCameraHeight (i, &rYOffset, &rYSize, &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// パラメータが一致?
		if ((ySize == rYSize) && (yOffset == rYOffset))
		{
			*pIndex = i;
			break;
		}
	}

	// 一致せず
	if (*pIndex == -1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Get Index No Entry Error\nyOffset = %d / ySize = %d\n", yOffset, ySize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Height Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//		yOffset				：Y Offset サイズ
//		ySize				：Yサイズ
//		pIndex				：Indexを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiHeightPositionCheck (int selector, int yOffset, int ySize, int *pIndex)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector2;
	int rYOffset, rYSize;
	int entryCount;
	int valid;

	// Check pIndex Parameter
	if (pIndex == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Height Position pIndex NULL Parameter Error.\n");
		goto _DONE;
	}

	// 登録個数の確認
	if ((status = roiGetEntryCount (&entryCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Entry Full?
	if (entryCount >= ROI_SELECTOR_COUNT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Height Position Full Entry(%d) Error.(Min : %d / Max : %d)\n", entryCount, ROI_SELECTOR_MIN, ROI_SELECTOR_COUNT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 初期化
	*pIndex = -1;

	for (selector2=0; selector2<entryCount; selector2++)
	{
		// Height Offset/Size
		if ((status = fpgaRoiGetCameraHeight (selector2, &rYOffset, &rYSize, &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (rYOffset >= IMG_HEIGHT_OFFSET)
			rYOffset -= IMG_HEIGHT_OFFSET;

		// 未登録
		if ((rYOffset == 0) && (rYSize == 0))
			break;

		// 指定yOffsetは登録済みのものよりも大きい？
		if (rYOffset < yOffset)
		{
			// 指定yOffsetの方が大きい
			
			// 登録済みyOffset+ySizeはYOffsetにかぶってない?
			if ((rYOffset + rYSize) > yOffset)
			{
				// y方向はかぶってる。
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "ROI Area Overlap1 Parameter Error\nSelector = %d\nYOffset=%d, YSize=%d\n", selector2, yOffset, ySize);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}
		// 指定yOffsetは登録済みのものよりも小さい？
		else
		{
			// 指定yOffsetの方が小さい

			// 指定yOffset+ySizeは登録済みのrYOffsetにかぶってない?
			if ((yOffset + ySize) > rYOffset)
			{
				// y方向はかぶってる。
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "ROI Area Overlap2 Parameter Error\nSelector = %d\nYOffset=%d, YSize=%d\n", selector2, yOffset, ySize);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
			else
			{
				// ここの位置に格納
				*pIndex = selector2;
				goto _DONE;
			}
		}
	}

	// 終端に格納
	*pIndex = selector2;

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Set Default
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetDefaultY (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int entryCount;
	int i;
	int offset, size;
	int size0;
	int mode;
	int startMode = 0;
	int selector, count, valid;
#if defined(MODE_SPECTRUM)
	int spectrumMode;

	// Spectrum default mode 取得
	if ((status = spectrumGetDefaultMode(&spectrumMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// 初期化中
	OUT32 (FIRM_DATA_ROI_DEFAULT_ADRS, 1);

#if defined(MODE_SPECTRUM)
	// binningx1に設定
	if ((status = aoiSetBinningY(BINNING1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 登録個数の確認
	if ((status = roiGetEntryCount (&entryCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ROI Selector 0の設定を確認(ROI 0を有効にしないと、他のエントリが消せない為)
	if ((status = roiSetSelector (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Size
	if ((status = aoiGetHeight (&size0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Enable/Disable
	if ((status = roiGetValid (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == 0)
	{
		// Set Enable
		if ((status = roiSetValid (1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	entryCount = ROI_SELECTOR_COUNT;

	for (i=entryCount-1; i>=0; i--)
	{
		// ROI Selector
		if ((status = roiSetSelector (i)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get Enable
		if ((status = roiGetValid (&valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (valid == 1)
		{
			// ROIエントリ数取得
			if ((status = roiGetEntryCount (&count)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// ROIエントリ最後?(最後の１個がselecto=0でない場合は消せない)
			if (count == 1)
			{
				// ROI Selector
				if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

				// セレクタは0でない？
				if (selector != 0)
				{
					// 現在のセレクタのSize変更
					if ((status = aoiSetHeight (1)) != AVAL_STATUS_SUCCESS)
						goto _DONE;

					// 現在のセレクタOffset変更
					if ((status = aoiSetHeightOffset (16)) != AVAL_STATUS_SUCCESS)
						goto _DONE;

					// Selector0を設定
					if ((status = roiSetSelector (0)) != AVAL_STATUS_SUCCESS)
						goto _DONE;

					// セレクタ0のSize設定
					if ((status = aoiSetHeight (1)) != AVAL_STATUS_SUCCESS)
						goto _DONE;

					// セレクタ0のOffset設定
					if ((status = aoiSetHeightOffset (0)) != AVAL_STATUS_SUCCESS)
						goto _DONE;

					// 本来のセレクタに戻す
					if ((status = roiSetSelector (i)) != AVAL_STATUS_SUCCESS)
						goto _DONE;
				}
			}
		}

		// Offset
		offset = 0;

		// Size
		if (i == 0)
			size = HeightMax ();
		else
			size = 0;

		// Size
		if ((status = aoiSetHeight (size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offset
		if (i == 0)
		{
			if ((status = aoiSetHeightOffset (offset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

	#if defined(MODE_SPECTRUM)
	if(spectrumMode == SPECTRUM_DEFAULTY_MODE_DEFAULT)
	{
		if((status = spectrumGetBandOffset(&offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		size = SPECTRUM_BAND_COUNT_ROI;

		// Size
		if ((status = aoiSetHeight (size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((status = aoiSetHeightOffset (offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	#endif // #if defined(MODE_SPECTRUM)

_DONE:
	// ROI Selector 0
	roiSetSelector (0);

	if (startMode != 0)
		acquisitionStart ();

	// 終了
	OUT32 (FIRM_DATA_ROI_DEFAULT_ADRS, 0);

	return (status);
}


//**********************************************************************************
// ROI Show
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0:通常モード/1:詳細モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiShow (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector, entryCount;
	int xOffset, xSize;
	int yOffset, ySize;
	int wEnable, hEnable;
	int xValid, yValid;
	int rTempOffset, rTempSize;
	int selectorSave = -1;

	// 登録個数の確認
	//if ((status = roiGetEntryCount (&entryCount2)) != AVAL_STATUS_SUCCESS)
	//goto _DONE;

	entryCount = ROI_SELECTOR_MAX + 1;

	// ROI Selector取得
	if ((status = roiGetSelector (&selectorSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("\n[ROI information]\n");
	for (selector=0; selector<entryCount; selector++)
	{
		DEBUG_PRINT_FORCE ("   No%d : ", selector);

		// ROI Selector設定
		if ((status = roiSetSelector (selector)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Width Valid
		if (selector == 0)
			xValid = 1;
		else
			xValid = 0;

		// Memory Height
		if ((status = memRoiGetCameraHeight (selector, &rTempOffset, &rTempSize, &yValid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Width Offset
		if ((status = aoiGetWidthOffset (&xOffset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Width Size
		if ((status = aoiGetWidth (&xSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Height Offset
		if ((status = aoiGetHeightOffset (&yOffset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Height Size
		if ((status = aoiGetHeight (&ySize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Check Entry
		if (selector >= 1)
		{
			xOffset = 0;
			xSize = 0;
		}

		DEBUG_PRINT_FORCE ("X Enable %d, ", xValid);
		DEBUG_PRINT_FORCE ("Y Enable %d, ", yValid);
		DEBUG_PRINT_FORCE ("StartX %4d, ", xOffset);
		DEBUG_PRINT_FORCE ("SizeX %4d, ", xSize);
		DEBUG_PRINT_FORCE ("StartY %4d, ", yOffset);
		DEBUG_PRINT_FORCE ("SizeY %4d\n", ySize);
	}

	if (mode != 0)
	{
		DEBUG_PRINT_FORCE ("\n[ROI Detail information]\n");

		DEBUG_PRINT_FORCE ("\n  [FPGA]\n");
		for (selector=0; selector<entryCount; selector++)
		{
			DEBUG_PRINT_FORCE ("   No%d : ", selector);

			// Width Valid
			if (selector == 0)
				wEnable = 1;
			else
				wEnable = 0;

			// Width Offset/Size
			if ((status = fpgaRoiGetCameraWidth (selector, &xOffset, &xSize, &xValid)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Height Offset/Size
			if ((status = fpgaRoiGetCameraHeight (selector, &yOffset, &ySize, &yValid)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Check Entry
			if (selector >= 1)
			{
				xOffset = 0;
				xSize = 0;
			}

			DEBUG_PRINT_FORCE ("X Enable %d, ", xValid);
			DEBUG_PRINT_FORCE ("Y Enable %d, ", yValid);
			DEBUG_PRINT_FORCE ("StartX %4d, ", xOffset);
			DEBUG_PRINT_FORCE ("SizeX %4d, ", xSize);
			DEBUG_PRINT_FORCE ("StartY %4d, ", yOffset);
			DEBUG_PRINT_FORCE ("SizeY %4d\n", ySize);
		}

		DEBUG_PRINT_FORCE ("\n  [Sensor]\n");
		for (selector=0; selector<entryCount; selector++)
		{
			DEBUG_PRINT_FORCE ("   No%d : ", selector);

			// Width Valid
			if ((status = sensorRoiGetValid (selector, SENSOR_REG_ROI_WIDTH_VALID, &wEnable)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Height Valid
			if ((status = sensorRoiGetValid (selector, SENSOR_REG_ROI_HEIGHT_VALID, &hEnable)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Width Offset/Size
			if ((status = sensorRoiGetWidth (selector, &xOffset, &xSize, &xValid)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Height Offset/Size
			if ((status = sensorRoiGetHeight (selector, &yOffset, &ySize, &yValid)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			DEBUG_PRINT_FORCE ("X Enable %d, ", xValid);
			DEBUG_PRINT_FORCE ("Y Enable %d, ", yValid);
			DEBUG_PRINT_FORCE ("StartX %4d, ", xOffset);
			DEBUG_PRINT_FORCE ("SizeX %4d, ", xSize);
			DEBUG_PRINT_FORCE ("StartY %4d, ", yOffset);
			DEBUG_PRINT_FORCE ("SizeY %4d\n", ySize);
		}
	}

	DEBUG_PRINT_FORCE ("\n");

_DONE:

	// ROI Selector設定
	if (selectorSave != -1)
		roiSetSelector (selectorSave);

	return (status);
}


// ROIデータ保存用
int gRoiFpgaXOffsetSave=0, gRoiFpgaXSizeSave=0;
int gRoiSensorXOffsetSave=0, gRoiSensorXSizeSave=0;
int gRoiMemoryXOffsetSave=0, gRoiMemoryXSizeSave=0;

int gRoiFpgaYOffsetSave[ROI_SELECTOR_COUNT] ={}, gRoiFpgaYSizeSave[ROI_SELECTOR_COUNT] ={};
int gRoiSensorYOffsetSave[ROI_SELECTOR_COUNT] ={}, gRoiSensorYSizeSave[ROI_SELECTOR_COUNT] ={};
int gRoiMemoryYOffsetSave[ROI_SELECTOR_COUNT] ={}, gRoiMemoryYSizeSave[ROI_SELECTOR_COUNT] ={};

int gRoiEntryCountSave = 0;

//**********************************************************************************
// ROI Area Main
// Mode = 0(Default)	x : 8 - 1280 / y : 4 - 1024
// Mode = 1(Full)		x : 0 - 1296 / y : 0 - 1032
// Mode = 2(DMA)		FPGA   : x : 8 - 1280 / y : 4 - 1024 
//                      Sensor : x : 0 - 1296 / y : 0 - 1032
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=Defaultサイズ/1=Fullサイズ/2=DMAサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetAreaSize (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int sizeFpga, sizeSensor;
	int offsetFpga, offsetSensor;
	int valid;
	int startMode = 0;
#if defined (MODE_DPC_GRID_UPDATE)
	int count;
#endif
	double frameMax;
#if defined (MODE_CXP)
	int cxpPort = 0;
#endif
#if defined (MODE_IPU_MULTI)
	unsigned int data32Left, data32Right;
	unsigned int offsetOld, sizeOld;
	unsigned int offsetOldLeft, offsetOldRight;
#endif
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	int bit;
	double frameRate;
	unsigned int expTime;
#endif

	// Check mode Parameter
	if ((mode < ROI_AREA_MODE_MIN) || (mode > ROI_AREA_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Set Area Size mode(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, ROI_AREA_MODE_MIN, ROI_AREA_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPC設定
	if ((status = dpcSetMode (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Width Save
	//--------------------------------------------------------------------------------

	// FPGA Widthレジスタ取得
	if ((status = fpgaRoiGetCameraWidth (0, &gRoiFpgaXOffsetSave, &gRoiFpgaXSizeSave, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor Widthレジスタ取得
	if ((status = sensorRoiGetWidth (0, &gRoiSensorXOffsetSave, &gRoiSensorXSizeSave, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Memory Widthレジスタ取得
	if ((status = memRoiGetCameraWidth (0, &gRoiMemoryXOffsetSave, &gRoiMemoryXSizeSave, &valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//--------------------------------------------------------------------------------
	// Height Save
	//--------------------------------------------------------------------------------
	for (selector = 0; selector < ROI_SELECTOR_COUNT; selector++)
	{
		// FPGA Heightレジスタ取得
		if ((status = fpgaRoiGetCameraHeight (selector, &gRoiFpgaYOffsetSave[selector], &gRoiFpgaYSizeSave[selector], &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Size=0なら未設定
		if (gRoiFpgaYSizeSave[selector] == 0)
			break;

		// Sensor Heightレジスタ取得
		if ((status = sensorRoiGetHeight (selector, &gRoiSensorYOffsetSave[ROI_SELECTOR_COUNT], &gRoiSensorYSizeSave[ROI_SELECTOR_COUNT], &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Memory Heightレジスタ取得
		if ((status = memRoiGetCameraHeight (selector, &gRoiMemoryYOffsetSave[selector], &gRoiMemoryYSizeSave[selector], &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//--------------------------------------------------------------------------------
	// 登録数取得
	//--------------------------------------------------------------------------------
	if ((status = roiGetEntryCount (&gRoiEntryCountSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Width/Height Size
	//--------------------------------------------------------------------------------
	if ((status = roiGetAreaWidthSize (mode, &sizeFpga, &offsetFpga, &sizeSensor, &offsetSensor)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FPGA Widthレジスタ設定
	if ((status = fpgaRoiSetCameraWidth (0, offsetFpga, sizeFpga, 1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor Widthレジスタ設定
	if ((status = sensorRoiSetWidth (0, offsetSensor, sizeSensor, 1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Memory Widthレジスタ設定
	if ((status = memRoiSetCameraWidth (0, offsetFpga, sizeFpga, 1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		#if !defined (MODE_CXP_MULTI_PORT)

		// Width設定
		if ((status = cxpSetWidth (cxpPort, sizeFpga)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offsetx設定
		if ((status = cxpSetOffsetX (cxpPort, offsetFpga)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#else // #if !defined (MODE_CXP_MULTI_PORT)

		// CXP Port取得
		if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Width設定
		if ((status = cxpSetWidth (cxpPort, sizeFpga)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offsetx設定
		if ((status = cxpSetOffsetX (cxpPort, offsetFpga)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#endif // #if !defined (MODE_CXP_MULTI_PORT)

	}
#endif

	// 残りはクリア
	for (selector = 1; selector < ROI_SELECTOR_COUNT; selector++)
	{
		if ((gRoiStatus = sensorRoiSetWidth (selector, 0, 0, 0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//--------------------------------------------------------------------------------
	// 旧ROI設定
	//--------------------------------------------------------------------------------
#if defined (MODE_IPU_MULTI)
	if (mode == ROI_AREA_MODE_DEFAULT_SIZE)
	{
		data32Left = DEFAULT_ROI_WIDTH0_LEFT;
		data32Right = DEFAULT_ROI_WIDTH0_RIGHT;
	}
	else if (mode == ROI_AREA_MODE_DMA_SIZE)
	{
		offsetOld = 0;
		sizeOld = FPGA_ROI_CAMERA_SIZE_SET(IMG_WIDTH_IPU_SIZE);

		data32Left = offsetOld | sizeOld;
		data32Right = data32Left;
	}
	else	// ROI_AREA_MODE_FULL_SIZE
	{
		offsetOldLeft = 0;
		offsetOldRight = FPGA_ROI_CAMERA_START_SET(IMG_WIDTH_OFFSET);

		sizeOld = FPGA_ROI_CAMERA_SIZE_SET(IMG_WIDTH_IPU_SIZE);

		data32Left = offsetOldLeft | sizeOld;
		data32Right = offsetOldRight | sizeOld;
	}

	// 従来のROIを設定
	OUT32 (FPGA_ROI_CAMERA_X_ADRS, data32Left);
	OUT32 (FPGA_ROI2_CAMERA_X_ADRS, data32Right);

#endif // #if defined (MODE_IPU_MULTI)

	//--------------------------------------------------------------------------------
	// Height Size
	//--------------------------------------------------------------------------------
	if ((status = roiGetAreaHeightSize (mode, &sizeFpga, &offsetFpga, &sizeSensor, &offsetSensor)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FPGA Heightレジスタ設定
	if (sizeFpga == 0)
		valid = 0;
	else
		valid = 1;

	if ((status = fpgaRoiSetCameraHeight (0, offsetFpga, sizeFpga, valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor Heightレジスタ設定
	if (sizeSensor == 0)
		valid = 0;
	else
		valid = 1;

	if ((status = sensorRoiSetHeight (0, offsetSensor, sizeSensor, valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Memory Heightレジスタ設定
	if ((status = memRoiSetCameraHeight (0, offsetFpga, sizeFpga, valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{

		#if !defined (MODE_CXP_MULTI_PORT)

		// Height設定
		if ((status = cxpSetHeight (cxpPort, sizeFpga)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offsety設定
		if ((status = cxpSetOffsetY (cxpPort, offsetFpga)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#else // #if !defined (MODE_CXP_MULTI_PORT)

		// CXP Port取得
		if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Height設定
		if ((status = cxpSetHeight (cxpPort, sizeFpga)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Offsety設定
		if ((status = cxpSetOffsetY (cxpPort, offsetFpga)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		#endif // #if !defined (MODE_CXP_MULTI_PORT)

	}
#endif

	// 残りはクリア
	valid = 0;
	for (selector = 1; selector < ROI_SELECTOR_COUNT; selector++)
	{
		if ((status = fpgaRoiSetCameraHeight (selector, 0, 0, valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((status = sensorRoiSetHeight (selector, 0, 0, valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((status = memRoiSetCameraHeight (selector, 0, 0, valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//--------------------------------------------------------------------------------
	// Set Save/Restore Flag
	//--------------------------------------------------------------------------------
	OUT32 (FIRM_DATA_ROI_SAVE_RESTORE_ADRS, 1);

	
	//--------------------------------------------------------------------------------
	// 最大フレームレート取得(内部で最大フレームレートを再計算)
	//--------------------------------------------------------------------------------
	if ((status = rateMax (&frameMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//--------------------------------------------------------------------------------
	// ROI Set
	//--------------------------------------------------------------------------------
	if ((status = roiSetEnd()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Set Data
	//--------------------------------------------------------------------------------
	OUT32 (FIRM_DATA_ROI_AREA_SIZE_ADRS, mode);

	//--------------------------------------------------------------------------------
	// DPC Enable
	//--------------------------------------------------------------------------------
	if (mode == ROI_AREA_MODE_DEFAULT_SIZE)
		dpcSetMode (1);

	//------------------------------------------------------------
	// DPC Map情報更新
	//------------------------------------------------------------
#if defined (MODE_DPC_GRID_UPDATE)
	if ((status = dpcGetMapInfo ((unsigned char *)DPC_MEMORY_ADRS, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標登録
	dpcSetDefectionCountVersion2 (count);
#endif

	//------------------------------------------------------------
	// H Interval
	//------------------------------------------------------------
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

	// 露光時間取得
	if ((status = acquisitionGetExposure (&expTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレート取得
	if ((status = acquisitionGetFrameRate (&frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Bit
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set H Interval
	if ((status = sensorSetHInterval (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 露光時間取得
	if ((status = acquisitionSetExposure (expTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレート取得
	if ((status = acquisitionSetFrameRate (frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// ROI Get Area Size
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Modeを格納するポインタ（0=Defaultサイズ/1=Fullサイズ/2=DMAサイズ)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetAreaSize (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Area Size pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Mode
	*pMode = IN32 (FIRM_DATA_ROI_AREA_SIZE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Get Area Mode Width Size
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Mode（0=Defaultサイズ/1=Fullサイズ/2=DMAサイズ)
//		pFpgaSize			：FPGA Width Sizeを格納するポインタ
//		pFpgaOffset			：FPGA Width Offsetを格納するポインタ
//		pSensorSize			：Sensor Width Sizeを格納するポインタ
//		pSensorffset		：Sensor Width Offsetを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetAreaWidthSize (int mode, int *pFpgaSize, int *pFpgaOffset, int *pSensorSize, int *pSensorOffset)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check mode Parameter
	if ((mode < ROI_AREA_MODE_MIN) || (mode > ROI_AREA_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Set Area Width Size mode(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, ROI_AREA_MODE_MIN, ROI_AREA_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pFpgaSize Parameter
	if (pFpgaSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Area Width Size pFpgaSize NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pFpgaOffset Parameter
	if (pFpgaOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Area Width Size pFpgaOffset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pSensorSize Parameter
	if (pSensorSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Area Width Size pSensorSize NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pSensorOffset Parameter
	if (pSensorOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Area Width Size pSensorOffset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Area Mode
	//if ((status = roiGetAreaSize (&mode)) != AVAL_STATUS_SUCCESS)
	//goto _DONE;

	//--------------------------------------------------------------------------------
	// Width Size
	//--------------------------------------------------------------------------------
	if (mode == ROI_AREA_MODE_DEFAULT_SIZE)
	{
#if !defined (MODE_IPU_MULTI)
		*pFpgaSize     = WidthMax();
		*pSensorSize   = IMG_WIDTH_MAX;
		*pFpgaOffset   = IMG_WIDTH_OFFSET;
		*pSensorOffset = 0;
#else // #if !defined (MODE_IPU_MULTI)
		*pFpgaSize     = WidthMax();
		*pSensorSize   = IMG_WIDTH_MAX;
		*pFpgaOffset   = 0;
		*pSensorOffset = 0;
#endif // #if !defined (MODE_IPU_MULTI)
	}
	else if (mode == ROI_AREA_MODE_FULL_SIZE)
	{
		*pFpgaSize     = IMG_WIDTH_MAX;
		*pSensorSize   = IMG_WIDTH_MAX;
		*pFpgaOffset   = 0;
		*pSensorOffset = 0;
	}
	else
	{
#if !defined (MODE_IPU_MULTI)
		*pFpgaSize     = WidthMax();
		*pSensorSize   = IMG_WIDTH_MAX;
		*pFpgaOffset   = IMG_WIDTH_OFFSET;
		*pSensorOffset = 0;
#else
		*pFpgaSize     = WidthMax();
		*pSensorSize   = IMG_WIDTH_MAX;
		*pFpgaOffset   = 0;
		*pSensorOffset = 0;
#endif
	}

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Get Area Mode Height Size
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Mode（0=Defaultサイズ/1=Fullサイズ/2=DMAサイズ)
//		pFpgaSize			：FPGA Height Sizeを格納するポインタ
//		pFpgaOffset			：FPGA Height Offsetを格納するポインタ
//		pSensorSize			：Sensor Height Sizeを格納するポインタ
//		pSensorffset		：Sensor Height Offsetを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetAreaHeightSize (int mode, int *pFpgaSize, int *pFpgaOffset, int *pSensorSize, int *pSensorOffset)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check mode Parameter
	if ((mode < ROI_AREA_MODE_MIN) || (mode > ROI_AREA_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Set Area Height Size mode(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, ROI_AREA_MODE_MIN, ROI_AREA_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pFpgaSize Parameter
	if (pFpgaSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Area Height Size pFpgaSize NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pFpgaOffset Parameter
	if (pFpgaOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Area Height Size pFpgaOffset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pSensorSize Parameter
	if (pSensorSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Area Height Size pSensorSize NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pSensorOffset Parameter
	if (pSensorOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Area Height Size pSensorOffset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Area Mode
	//if ((status = roiGetAreaSize (&mode)) != AVAL_STATUS_SUCCESS)
	//goto _DONE;

	//--------------------------------------------------------------------------------
	// Height Size
	//--------------------------------------------------------------------------------
	if (mode == ROI_AREA_MODE_DEFAULT_SIZE)
	{
		*pFpgaSize     = HeightMax();
		*pSensorSize   = IMG_HEIGHT_MAX;
		*pFpgaOffset   = IMG_HEIGHT_OFFSET;
		*pSensorOffset = 0;
	}
	else if (mode == ROI_AREA_MODE_FULL_SIZE)
	{
		*pFpgaSize     = IMG_HEIGHT_MAX;
		*pSensorSize   = IMG_HEIGHT_MAX;
		*pFpgaOffset   = 0;
		*pSensorOffset = 0;
	}
	else
	{
		*pFpgaSize     = HeightMax();
		*pSensorSize   = IMG_HEIGHT_MAX;
		*pFpgaOffset   = IMG_HEIGHT_OFFSET;
		*pSensorOffset = 0;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Area Restore
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiRestoreAreaSize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int flag;
	int valid;
	int startMode = 0;

	// Save/Restore Flag
	flag = IN32 (FIRM_DATA_ROI_SAVE_RESTORE_ADRS);

	// Saveされてない
	if (flag == 0)
		goto _DONE;

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	acquisitionAbort ();

	//--------------------------------------------------------------------------------
	// Width Restore
	//--------------------------------------------------------------------------------
	valid = 1;

	// FPGA Widthレジスタ設定
	if ((gRoiStatus = fpgaRoiSetCameraWidth (0, gRoiFpgaXOffsetSave, gRoiFpgaXSizeSave, valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if 0
	// Sensor Widthレジスタ設定
	if ((gRoiStatus = sensorRoiSetWidth (selector, gRoiSensorXOffsetSave, gRoiSensorXSizeSave, valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Memory Widthレジスタ取得
	if ((status = memRoiSetCameraWidth (0, gRoiMemoryXOffsetSave, gRoiMemoryXSizeSave, valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#endif

	//--------------------------------------------------------------------------------
	// Height Restore
	//--------------------------------------------------------------------------------
	for (selector = 0; selector < gRoiEntryCountSave; selector++)
	{
		if (gRoiFpgaYSizeSave[selector] == 0)
			valid = 0;
		else
			valid = 1;

		// FPGA Heightレジスタ設定
		if ((gRoiStatus = fpgaRoiSetCameraHeight (selector, gRoiFpgaYOffsetSave[selector], gRoiFpgaYSizeSave[selector], valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

#if 0
		// Sensor Heightレジスタ設定
		if ((gRoiStatus = sensorRoiSetHeight (selector, gRoiSensorYOffsetSave[selector], gRoiSensorYSizeSave[selector], valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;


		// Memory Heightレジスタ取得
		if ((status = memRoiSetCameraHeight (selector, gRoiMemoryYOffsetSave[selector], gRoiMemoryYSizeSave[selector], valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif
	}

	//--------------------------------------------------------------------------------
	// 登録数設定
	//--------------------------------------------------------------------------------
	if ((status = roiSetEntryCount (gRoiEntryCountSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// ROI Set
	//--------------------------------------------------------------------------------
	if ((status = roiSetEnd()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Save/Restore Flag Clear
	//--------------------------------------------------------------------------------
	OUT32 (FIRM_DATA_ROI_SAVE_RESTORE_ADRS, 0);

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// ROI Set DMA Full
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetDmaFull (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// ROI DMA Size
	if ((status = roiSetAreaSize (ROI_AREA_MODE_DMA_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Adjust Mode
	if ((status = roiSetAdjustMode (ROI_MODE_ADJUST)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Set DMA Restore
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetDmaResutore (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Restore
	//if ((status = roiRestoreAreaSize ()) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// ROI Default Size
	if ((status = roiSetAreaSize (ROI_AREA_MODE_DEFAULT_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Adjust Mode
	if ((status = roiSetAdjustMode (ROI_MODE_NORMAL)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Area Grid Offset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pX					：X座標を格納するポインタ
//		pY					：X座標を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetAreaGridOffset (int *pX, int *pY)
{
	int status = AVAL_STATUS_SUCCESS;
	int areaMode;

	// Check pX Parameter
	if (pX == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Area Grid Offset pX NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pY Parameter
	if (pY == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Area Grid Offset pY NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check Area MOde
	if ((status = roiGetAreaFlag (&areaMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// デフォルト領域ならば、オフセット付加
	if (areaMode == ROI_AREA_MODE_DEFAULT_SIZE)
	{
		*pX += IMG_WIDTH_OFFSET;
		*pY += IMG_HEIGHT_OFFSET;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Area Width Max
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pWidth				：Width Max格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetAreaWidthMax (int *pWidth)
{
	int status = AVAL_STATUS_SUCCESS;
	int areaMode;

	// Check pWidth Parameter
	if (pWidth == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Area Width MAx pWidth NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check Area Mode
	if ((status = roiGetAreaFlag (&areaMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Maxサイズ取得
	if (areaMode == ROI_AREA_MODE_DEFAULT_SIZE)
		*pWidth = WidthMax();
	else
		*pWidth = sensorWidth();

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Area Height Max
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pHeight				：Height Max格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetAreaHeightMax (int *pHeight)
{
	int status = AVAL_STATUS_SUCCESS;
	int areaMode;

	// Check pHeight Parameter
	if (pHeight == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Area Height Max pHeight NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check Area MOde
	if ((status = roiGetAreaFlag (&areaMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Maxサイズ取得
	if (areaMode == ROI_AREA_MODE_DEFAULT_SIZE)
		*pHeight = HeightMax();
	else
		*pHeight = sensorHeight();

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Set Adjust Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Adjust Mode(0:Normal / 1:Adjust)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetAdjustMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Check mode Parameter
	if ((mode != ROI_MODE_NORMAL) && (mode != ROI_MODE_ADJUST))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Set Adjust Mode(%d) Parameter Error.(Default:%d / Full Size:%d)\n", mode, ROI_MODE_NORMAL, ROI_MODE_ADJUST);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Adjust Mode設定
	OUT32 (FPGA_ROI_CAL_MODE_ADRS, mode);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_ROI2_CAL_MODE_ADRS, mode);
#endif

	// Set End
	if ((status = roiSetEnd ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// ROI Get Area Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Adjust Modeを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetAdjustMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Adjust Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// 取得
	*pMode = IN32 (FPGA_ROI_CAL_MODE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Set Area Flag
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Area Mode(0:Default / 1:Full Size)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiSetAreaFlag (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_DPC_GRID_UPDATE)
	int count;
#endif

	// Check mode Parameter
	if ((mode != ROI_AREA_MODE_DEFAULT_SIZE) && (mode != ROI_AREA_MODE_FULL_SIZE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Set Area Flag mode(%d) Parameter Error.(Default:%d / Full:%d)\n", mode, ROI_AREA_MODE_DEFAULT_SIZE, ROI_AREA_MODE_FULL_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Area Mode設定
	OUT32 (FIRM_DATA_ROI_AREA_MODE_ADRS, mode);

	//------------------------------------------------------------
	// DPC Map情報更新
	//------------------------------------------------------------
#if defined (MODE_DPC_GRID_UPDATE)
	if ((status = dpcGetMapInfo ((unsigned char *)DPC_MEMORY_ADRS, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標登録
	dpcSetDefectionCountVersion2 (count);
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Get Area Flag
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Area Flagを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetAreaFlag (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Flag pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// 取得
	*pMode = IN32 (FIRM_DATA_ROI_AREA_MODE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Set Valid
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：自身のセレクタ
//		pCount				：自身を除いた登録数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiGetNoSelfValidCount (int selector, int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector2;
	int valid;
	int offset, size;
	int totalCount;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI No Self Valid Count pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	totalCount = 0;
	for (selector2=0; selector2<ROI_SELECTOR_COUNT; selector2++)
	{
		if (selector == selector2)
			continue;	// 自身以外

		// Get Height(Memory)
		if ((status = memRoiGetCameraHeight (selector2, &offset, &size, &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// トータルカウント
		if (valid == 1)
			totalCount++;
	}

	// ROIカウント格納
	*pCount = totalCount;

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Check Multi Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：モードを格納するポインタ(0=マルチモード無効/1=マルチモード有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int roiCheckMultiMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int valid;
	int selectorSave = -1;
	int result = 0;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Check Multi Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Selector
	if ((status = roiGetSelector (&selectorSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Selector0のみのValidが有効であってほしい
	for (selector = 0; selector < ROI_SELECTOR_COUNT; selector++)
	{
		// Set Selector
		if ((status = roiSetSelector (selector)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get Selector
		if ((status = roiGetValid (&valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Selector0のValidは有効?
		if (selector == ROI_SELECTOR_MIN)
		{
			if (valid == 0)
			{
				result = 1;
				break;
			}
		}
		else	// Selector0以外ののValidは有効?
		{
			if (valid == 1)
			{
				result = 1;
				break;
			}
		}
	}

	// 設定
	*pMode = result;

_DONE:

	if (selectorSave != -1)
		roiSetSelector (selectorSave);

	return (status);
}


//**********************************************************************************
// Roi coordinate Trans
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		srcX				:変換前X座標
//		srcY				:変換前Y座標
//      dstX				:変換後X座標
//		dstY				:変換後Y座標
//		mode				:0 = 加算 / 1 = 減算
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	:正常終了
//		上記以外				：異常終了
//==================================================================================
int roiCoordinateTrans (int srcX, int srcY, int *dstX, int *dstY, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int offsetX = 0;
	int offsetY = 0;
	int dstOffsetX = 0;
	int dstOffsetY = 0;
	int widthMax = 0;
	int heightMax = 0;

	//if(srcY == 0)
		//goto _DONE;

	//width 最大値
	widthMax = WidthMax();

	//height 最大値
	heightMax = HeightMax();

	// offsetx取得
	if((status = aoiGetWidthOffset(&offsetX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// offsety取得
	if((status = aoiGetHeightOffset(&offsetY)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode == TRANS_MODE_ADD)
	{
		dstOffsetX = srcX + offsetX;
		dstOffsetY = srcY + offsetY;
	}
	else
	{
		dstOffsetX = srcX - offsetX;
		dstOffsetY = srcY - offsetY;
	}

	if ((dstOffsetX < 0) || (dstOffsetX > widthMax))
		goto _DONE;

	if ((dstOffsetY < 0) || (dstOffsetY > heightMax))
		goto _DONE;

	*dstX = dstOffsetX;
	*dstY = dstOffsetY;

_DONE:
	return (status);
}
#endif // #if defined (MODE_ROI_VERSION2)

// eof
