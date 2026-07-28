//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// roiFpga.c - ROI Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
#include "sensorVendors.h"
#endif


#if defined (MODE_ROI_VERSION2)
//**********************************************************************************
// ROI Camera Width設定(FPGAレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：ROIセレクタ
//		offset				：Widthオフセット
//		size				：Widthサイズ
//		valid				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiSetCameraWidth (int selector, int offset, int size, int valid)
{
	int status = AVAL_STATUS_SUCCESS;

#if !defined (MODE_IPU_MULTI)

	status = fpgaRoiSetCameraWidthMain (FPGA_ROI_CAMERA_X_ADRS, selector, offset, size, valid);

#else // #if !defined (MODE_IPU_MULTI)

	status = fpgaRoiSetCameraWidthMain (FPGA_HORIZONTAL_ROI_ADRS, selector, offset, size, valid);

#endif // #if !defined (MODE_IPU_MULTI)


	return (status);
}


//**********************************************************************************
// ROI Camera Width取得(FPGAレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：ROIセレクタ
//		pOffset				：Widthオフセットを格納するポインタ
//		pSize				：Widthサイズを格納するポインタ
//		pValid				：Validを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiGetCameraWidth (int selector, int *pOffset, int *pSize, int *pValid)
{
	int status;
	status = fpgaRoiGetCameraWidthMain (FPGA_ROI_CAMERA_X_ADRS, selector, pOffset, pSize, pValid);
	return (status);
}


//**********************************************************************************
// ROI Camera Height設定(FPGAレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：ROIセレクタ
//		offset				：Heightオフセット
//		size				：Heightサイズ
//		valid				：Valid状態(0=無効/0以外=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiSetCameraHeight (int selector, int offset, int size, int valid)
{
	int status;
	status = fpgaRoiSetCameraHeightMain (FPGA_ROI_CAMERA_Y_ADRS, selector, offset, size, valid);
	return (status);
}


//**********************************************************************************
// ROI Camera Height取得(FPGAレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：ROIセレクタ
//		pOffset				：Heightオフセットを格納するポインタ
//		pSize				：Heightサイズを格納するポインタ
//		pValid				：Validを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiGetCameraHeight (int selector, int *pOffset, int *pSize, int *pValid)
{
	int status;
	status = fpgaRoiGetCameraHeightMain (FPGA_ROI_CAMERA_Y_ADRS, selector, pOffset, pSize, pValid);
	return (status);
}


//**********************************************************************************
// ROI Sensor Height取得(FPGAレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：ROIセレクタ
//		pOffset				：Heightオフセットを格納するポインタ
//		pSize				：Heightサイズを格納するポインタ
//		pValid				：Validを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiGetSensorHeight (int selector, int *pOffset, int *pSize, int *pValid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check selector Parameters
	if ((selector < ROI_SELECTOR_MIN) || (selector > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor FPGA Get Height Selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Sensor FPGA Get Height pOffset NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Sensor FPGA Get Height pSize NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pValid Parameter
	if (pValid == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Sensor FPGA Get Height pValid NULL Parameter Error\n");
		goto _DONE;
	}

	// Height取得
	data32 = IN32 ((FPGA_ROI_SENSOR_Y_ADRS + selector * FPGA_ROI_ADRS_INTERVAL));

	// オフセット取得
	*pOffset = FPGA_ROI_CAMERA_START_GET (data32);

	// サイズ取得
	*pSize = FPGA_ROI_CAMERA_SIZE_GET (data32);

	// Valid取得
	if (data32 & FPGA_ROI_CAMERA_VALID_BIT)
		*pValid = 1;
	else
		*pValid = 0;

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Camera Width設定(Memory)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：ROIセレクタ
//		offset				：Widthオフセット
//		size				：Widthサイズ
//		valid				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int memRoiSetCameraWidth (int selector, int offset, int size, int valid)
{
	int status;
	status = fpgaRoiSetCameraWidthMain (FIRM_DATA_ROI_X0_ADRS, selector, offset, size, valid);
	return (status);
}


//**********************************************************************************
// ROI Camera Width取得(Memory)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：ROIセレクタ
//		pOffset				：Widthオフセットを格納するポインタ
//		pSize				：Widthサイズを格納するポインタ
//		pValid				：Validを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int memRoiGetCameraWidth (int selector, int *pOffset, int *pSize, int *pValid)
{
	int status;
	status = fpgaRoiGetCameraWidthMain (FIRM_DATA_ROI_X0_ADRS, selector, pOffset, pSize, pValid);
	return (status);
}


//**********************************************************************************
// ROI Camera Height設定(Memory)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：ROIセレクタ
//		offset				：Heightオフセット
//		size				：Heightサイズ
//		valid				：Valid状態(0=無効/0以外=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int memRoiSetCameraHeight (int selector, int offset, int size, int valid)
{
	int status;
	status = fpgaRoiSetCameraHeightMain (FIRM_DATA_ROI_Y0_ADRS, selector, offset, size, valid);
	return (status);
}


//**********************************************************************************
// ROI Camera Height取得(Memory)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：ROIセレクタ
//		pOffset				：Heightオフセットを格納するポインタ
//		pSize				：Heightサイズを格納するポインタ
//		pValid				：Valid状態を格納するポインタ(0=無効/0以外=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int memRoiGetCameraHeight (int selector, int *pOffset, int *pSize, int *pValid)
{
	int status;
	status = fpgaRoiGetCameraHeightMain (FIRM_DATA_ROI_Y0_ADRS, selector, pOffset, pSize, pValid);
	return (status);
}


//**********************************************************************************
// ROI Camera Width設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：ROIアドレス
//		selector			：ROIセレクタ
//		offset				：Widthオフセット
//		size				：Widthサイズ
//		valid				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiSetCameraWidthMain (unsigned int adrs, int selector, int offset, int size, int valid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
#if !defined (MODE_IPU_MULTI)
	unsigned int rData32, offset32, size32;
#endif

	// Widthセレクタは0固定
	selector = 0;

	// センサ実サイズを考慮
	//offset += IMG_WIDTH_OFFSET;

	// Check selector Parameters
	if ((selector < ROI_SELECTOR_MIN) || (selector > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Width Selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset Parameters
	if ((offset < ROI_WIDTH_OFFSET_MIN) || (offset > ROI_WIDTH_OFFSET_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Width Offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset, ROI_WIDTH_OFFSET_MIN, ROI_WIDTH_OFFSET_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if !defined (MODE_IPU_MULTI)

	// Check size Parameters
	if ((size < ROI_WIDTH_SIZE_MIN) || (size > ROI_WIDTH_SIZE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Width Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size, ROI_WIDTH_SIZE_MIN, ROI_WIDTH_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset + size Parameters
	if ((offset + size) > ROI_WIDTH_SIZE_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Width Offset(%d) + Size(%d) = %d Parameter Error.(Min:%d / Max:%d)\n", offset, size , offset + size, ROI_WIDTH_SIZE_MIN, ROI_WIDTH_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#else // #if !defined (MODE_IPU_MULTI)

	// Check size Parameters
	if ((size < ROI_WIDTH_SIZE_MIN) || (size > (ROI_WIDTH_SIZE_MAX + IMG_WIDTH_IPU_MULTI_ADD_SIZE)))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Width Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size, ROI_WIDTH_SIZE_MIN, (ROI_WIDTH_SIZE_MAX + IMG_WIDTH_IPU_MULTI_ADD_SIZE));
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset + size Parameters
	if ((offset + size) > (ROI_WIDTH_SIZE_MAX + IMG_WIDTH_IPU_MULTI_ADD_SIZE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Width Offset(%d) + Size(%d) = %d Parameter Error.(Min:%d / Max:%d)\n", offset, size , offset + size, ROI_WIDTH_SIZE_MIN, (ROI_WIDTH_SIZE_MAX + IMG_WIDTH_IPU_MULTI_ADD_SIZE));
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#endif // #if !defined (MODE_IPU_MULTI)

	// Check offset Align Parameter
	if ((offset % ROI_FPGA_WIDTH_OFFSET_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA  Set Width Offset Align(%d) Parameter Error. Align = %d.\n", offset, ROI_FPGA_WIDTH_OFFSET_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Align Parameter
	if ((size % ROI_FPGA_WIDTH_SIZE_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Width Size Align(%d) Parameter Error. Align = %d.\n", size, ROI_FPGA_WIDTH_SIZE_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check Valid Parameters
	if ((valid != 0) && (valid != 1))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Width Valid(%d) Parameter Error.\n", valid);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if !defined (MODE_IPU_MULTI)

	// Width取得
	rData32 = IN32 ((adrs + selector * FPGA_ROI_ADRS_INTERVAL));

	// Width Startクリア
	data32 = FPGA_ROI_CAMERA_START_CLEAR(rData32);

	// Width Sizeクリア
	data32 = FPGA_ROI_CAMERA_SIZE_CLEAR(data32);

	// Width Offset取得
	offset32 = FPGA_ROI_CAMERA_START_SET(offset);

	// Width Size取得
	size32 = FPGA_ROI_CAMERA_SIZE_SET(size);

	// Width Offset設定
	data32 |= offset32;

	// Width Size設定
	data32 |= size32;

	// Valid
	if (valid == 0)
		data32 &= ~FPGA_ROI_CAMERA_VALID_BIT;
	else
		data32 |= FPGA_ROI_CAMERA_VALID_BIT;

	// レジスタ設定
	OUT32 ((adrs + selector * FPGA_ROI_ADRS_INTERVAL), data32);

#else // #if !defined (MODE_IPU_MULTI)

	// Offset
	data32 = (offset & FPGA_HORIZONTAL_ROI_OFFSET_MASK) << FPGA_HORIZONTAL_ROI_OFFSET_SHIFT;

	// Size
	data32 |= ((size & FPGA_HORIZONTAL_ROI_SIZE_MASK) << FPGA_HORIZONTAL_ROI_SIZE_SHIFT);

	// レジスタ設定
	OUT32 ((adrs + selector * FPGA_ROI_ADRS_INTERVAL), data32);


#endif // #if !defined (MODE_IPU_MULTI)

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Camera Width取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：ROIアドレス
//		selector			：ROIセレクタ
//		pOffset				：Widthオフセットを格納するポインタ
//		pSize				：Widthサイズを格納するポインタ
//		pValid				：Validを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiGetCameraWidthMain (unsigned int adrs, int selector, int *pOffset, int *pSize, int *pValid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Widthセレクタは0固定
	selector = 0;

	// Check selector Parameters
	if ((selector < ROI_SELECTOR_MIN) || (selector > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Get Width Selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Camera FPGA Get Width pOffset NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Get Width pSize NULL Parameter Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pValid Parameter
	if (pValid == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Get Width pValid NULL Parameter Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if !defined (MODE_IPU_MULTI)
	// Width取得
	data32 = IN32 ((adrs + selector * FPGA_ROI_ADRS_INTERVAL));

	// オフセット取得
	*pOffset = FPGA_ROI_CAMERA_START_GET (data32);

	// センサ実サイズを考慮
	//*pOffset -= IMG_WIDTH_OFFSET;

	// サイズ取得
	*pSize = FPGA_ROI_CAMERA_SIZE_GET (data32);

	// Valid取得
	if (data32 & FPGA_ROI_CAMERA_VALID_BIT)
		*pValid = 1;
	else
		*pValid = 0;

#else // #if !defined (MODE_IPU_MULTI)

	// Width取得
	data32 = IN32 ((adrs + selector * FPGA_ROI_ADRS_INTERVAL));

	// オフセット取得
	*pOffset = (data32 >> FPGA_HORIZONTAL_ROI_OFFSET_SHIFT) & FPGA_HORIZONTAL_ROI_OFFSET_MASK;

	// サイズ取得
	*pSize = (data32 >> FPGA_HORIZONTAL_ROI_SIZE_SHIFT) & FPGA_HORIZONTAL_ROI_SIZE_MASK;


#endif // #if defined (MODE_IPU_MULTI)

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Camera Height設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：ROIアドレス
//		selector			：ROIセレクタ
//		offset				：Heightオフセット
//		size				：Heightサイズ
//		valid				：Valid状態(0=無効/0以外=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiSetCameraHeightMain (unsigned int adrs, int selector, int offset, int size, int valid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, rData32, offset32, size32;

	// センサ実サイズを考慮
	//offset += IMG_HEIGHT_OFFSET;

	// Check selector Parameters
	if ((selector < ROI_SELECTOR_MIN) || (selector > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Height Selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset Parameters
	if ((offset < ROI_HEIGHT_OFFSET_MIN) || (offset > ROI_HEIGHT_OFFSET_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Height Offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset, ROI_HEIGHT_OFFSET_MIN, ROI_HEIGHT_OFFSET_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Parameters
	if ((size < ROI_HEIGHT_SIZE_MIN) || (size > ROI_HEIGHT_SIZE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Height Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size, ROI_HEIGHT_SIZE_MIN, ROI_HEIGHT_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset + size Parameters
	if ((offset + size) > ROI_HEIGHT_SIZE_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Height Offset(%d) + Size(%d) = %d Parameter Error.(Min:%d / Max:%d)\n", offset, size , offset + size, ROI_HEIGHT_SIZE_MIN, ROI_HEIGHT_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset Align Parameters
	if ((offset % ROI_FPGA_HEIGHT_OFFSET_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Height Offset Align(%d) Parameter Error. Align = %d.\n", offset, ROI_FPGA_HEIGHT_OFFSET_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Align Parameter
	if ((size % ROI_FPGA_HEIGHT_SIZE_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Height Size Align(%d) Parameter Error. Align = %d.\n", size, ROI_FPGA_HEIGHT_SIZE_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check Valid Parameter
	if ((valid != 0) && (valid != 1))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Set Height Valid(%d) Parameter Error.\n", valid);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Height取得
	rData32 = IN32 ((adrs + selector * FPGA_ROI_ADRS_INTERVAL));

	// Height Startクリア
	data32 = FPGA_ROI_CAMERA_START_CLEAR(rData32);

	// Height Sizeクリア
	data32 = FPGA_ROI_CAMERA_SIZE_CLEAR(data32);

	// センサ実サイズを考慮
	//size += IMG_HEIGHT_OFFSET_SIZE;

	// Height Offset取得
	offset32 = FPGA_ROI_CAMERA_START_SET(offset);

	// Height Size取得
	size32 = FPGA_ROI_CAMERA_SIZE_SET(size);

	// Height Offset設定
	data32 |= offset32;

	// Height Size設定
	data32 |= size32;

	// Valid
	if (valid == 0)
		data32 &= ~FPGA_ROI_CAMERA_VALID_BIT;
	else
		data32 |= FPGA_ROI_CAMERA_VALID_BIT;

	// レジスタ設定
	OUT32 ((adrs + selector * FPGA_ROI_ADRS_INTERVAL), data32);

#if defined (MODE_IPU_MULTI)
	OUT32 ((adrs + selector * FPGA_ROI_ADRS_INTERVAL + FPGA_ROI2_OFFSET), data32);
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// ROI Camera Height取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：ROIアドレス
//		selector			：ROIセレクタ
//		pOffset				：Heightオフセットを格納するポインタ
//		pSize				：Heightサイズを格納するポインタ
//		pValid				：Valid状態を格納するポインタ(0=無効/0以外=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiGetCameraHeightMain (unsigned int adrs, int selector, int *pOffset, int *pSize, int *pValid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check selector Parameters
	if ((selector < ROI_SELECTOR_MIN) || (selector > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Camera FPGA Get Height Selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Camera FPGA Get Height pOffset NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Camera FPGA Get Height pSize NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pValid Parameter
	if (pValid == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Camera FPGA Get Height pValid NULL Parameter Error\n");
		goto _DONE;
	}

	// Height取得
	data32 = IN32 ((adrs + selector * FPGA_ROI_ADRS_INTERVAL));

	// オフセット取得
	*pOffset = FPGA_ROI_CAMERA_START_GET (data32);

	// センサ実サイズを考慮
	//*pOffset -= IMG_HEIGHT_OFFSET;

	// サイズ取得
	*pSize = FPGA_ROI_CAMERA_SIZE_GET (data32);

	// Valid取得
	if ((data32 & FPGA_ROI_CAMERA_VALID_BIT) == 0)
		*pValid = 0;
	else
		*pValid = 1;

_DONE:
	return (status);
}


//**********************************************************************************
//	ROI Camera Height Total Size(FPGAからの出力サイズ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiGetCameraHeightTotalSize (int *pSize)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int offset, size, bSize;
	unsigned int totalSize;
	int valid;
	int mode = MODE_DISABLE;

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Camera Height Total Size pSize NULL Parameter Error\n");
		goto _DONE;
	}

	totalSize = 0;
	for (selector=0; selector<ROI_SELECTOR_COUNT; selector++)
	{
		// Get Height
		if ((status = fpgaRoiGetCameraHeight (selector, &offset, &size, &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

#if defined(MODE_SPECTRUM)

		if(valid == 1)
		{
			if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			if(mode != MODE_DISABLE)
			{
				//1以下はスルー
				if(size <= (mode-1))
					continue;

				if(mode == 3)
				{
					// 切り上げ
					bSize= (size + 2) / 3;
					bSize += (size % 3) != 0 ? 1 : 0;
				}
				else
				{
					//偶数なら半分
					if((size % mode) == 0)
						bSize = size / mode;
					else
						//奇数なら-1
						bSize = (size & ~(mode -1)) / mode;
				}
			}

			// トータルサイズ
			totalSize += bSize;
		}

#else // defined(MODE_SPECTRUM)
		// トータルサイズ
		if (valid == 1)
			totalSize += size;
#endif // #if defined(MODE_SPECTRUM)
	}

	// 設定
	OUT32 (FIRM_DATA_ROI_CAMERA_HEIGHT_TOTAL_ADRS, totalSize);

	*pSize = totalSize;

_DONE:
	return (status);
}


//**********************************************************************************
//	ROI Sensor Height Total Size(Sensorからの出力サイズ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiGetSensorHeightTotalSize (int *pSize)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int offset, size;
	unsigned int totalSize;
	int valid;

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Sensor Height Total Size pSize NULL Parameter Error\n");
		goto _DONE;
	}

	totalSize = 0;
	for (selector=0; selector<ROI_SELECTOR_COUNT; selector++)
	{
		// Get Height
		if ((status = fpgaRoiGetSensorHeight (selector, &offset, &size, &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// トータルサイズ
		if (valid == 1)
			totalSize += size;
	}

	// 設定
	OUT32 (FIRM_DATA_ROI_SENSOR_HEIGHT_TOTAL_ADRS, totalSize);

	*pSize = totalSize;

_DONE:
	return (status);
}


//**********************************************************************************
//	ROI Camera OffsetY検索
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pOffsetY			：OffsetYを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRoiGetOffsetY (int *pOffsetY)
{
	int status = AVAL_STATUS_SUCCESS;
	int selector;
	int offset, size;
	int offsetY;
	int valid;
	int hit = 0;

	// Check pOffsetY Parameter
	if (pOffsetY == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Get Camera Offsety pOffsetY NULL Parameter Error\n");
		goto _DONE;
	}

	offsetY = CAMERA_HEIGHT_MAX - 1;
	for (selector=0; selector<ROI_SELECTOR_COUNT; selector++)
	{
		// Get Height
		if ((status = fpgaRoiGetCameraHeight (selector, &offset, &size, &valid)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (valid == 1)
		{
			if (offsetY > offset)
				offsetY = offset;

			hit = 1;
		}
	}

	if (hit == 0)
	{
		*pOffsetY = 0;
	}
	else
	{
		if (offsetY < IMG_HEIGHT_OFFSET)
			*pOffsetY = 0;
		else
			*pOffsetY = offsetY - IMG_HEIGHT_OFFSET;
	}

_DONE:
	return (status);
}

#endif // #if defined (MODE_ROI_VERSION2)

// eof
