//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// roiFpgaCamera.c - ROI Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
#include "sensorVendors.h"
#endif


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define ROI_SENSIOR_CHIP_ID			(3)
#define ROI_SENSIOR_REG_HOLD		(0)
#define ROI_SENSIOR_ACC_TMG			(1)


#if defined (MODE_ROI_VERSION2)
//**********************************************************************************
//	ROI Width設定関数(センサレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//		offset				：Widthオフセット
//		size				：Widthサイズ
//		valid				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorRoiSetWidth (int selector, int offset, int size, int valid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char adrs, data;
	int acTmg = ROI_SENSIOR_ACC_TMG;
	int regHold = ROI_SENSIOR_REG_HOLD;
	unsigned char id = ROI_SENSIOR_CHIP_ID;

	// センサ実サイズを考慮
	//size += IMG_WIDTH_OFFSET_SIZE;

	// Check selector Parameter
	if ((selector < ROI_SELECTOR_MIN) || (selector  > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Width selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset Parameters
	if ((offset < ROI_WIDTH_OFFSET_MIN) || (offset > ROI_WIDTH_OFFSET_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Width Offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset, ROI_WIDTH_OFFSET_MIN, ROI_WIDTH_OFFSET_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Parameters
	if ((size < ROI_WIDTH_SIZE_MIN) || (size > ROI_WIDTH_SIZE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Width Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size, ROI_WIDTH_SIZE_MIN, ROI_WIDTH_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset + size Parameters
	if ((offset + size) > ROI_WIDTH_SIZE_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Width Offset(%d) + Size(%d) = %d Parameter Error.(Min:%d / Max:%d)\n", offset, size , offset + size, ROI_WIDTH_SIZE_MIN, ROI_WIDTH_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset Align Parameters
	if ((offset % ROI_SENSOR_WIDTH_OFFSET_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Width Offset Align(%d) Parameter Error. Align = %d.\n", offset, ROI_SENSOR_WIDTH_OFFSET_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Align Parameters
	if ((size % ROI_SENSOR_WIDTH_SIZE_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor ROI Set Width size Align(%d) Parameter Error. Align = %d.\n", size, ROI_SENSOR_WIDTH_SIZE_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check Valid Parameters
	if ((valid != 0) && (valid != 1))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor ROI Set Width size Valid(%d) Parameter Error.\n", valid);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	size += BLACK_OFFSET_AUTO_WIDTH_SIZE;

	// Widthオフセット下位設定
	adrs = SENSOR_REG_ROI_H1_POS_ADRS1 + selector * SENSOR_REG_ROI_INTERVAL;
	data = offset & SENSOR_REG_ROI_WIDTH_START_LOWER_MASK;
	if ((status = sensorRegWriteByte (id, adrs, data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Widthオフセット上位設定
	adrs = SENSOR_REG_ROI_H1_POS_ADRS2 + selector * SENSOR_REG_ROI_INTERVAL;
	data = (offset >> 8) & SENSOR_REG_ROI_WIDTH_START_UPPER_MASK;
	if ((status = sensorRegWriteByte (id, adrs, data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Widthサイズ下位設定
	adrs = SENSOR_REG_ROI_H1_WIDTH_ADRS1 + selector * SENSOR_REG_ROI_INTERVAL;
	data = size & SENSOR_REG_ROI_WIDTH_SIZE_LOWER_MASK;
	if ((status = sensorRegWriteByte (id, adrs, data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Widthサイズ上位設定
	adrs = SENSOR_REG_ROI_H1_WIDTH_ADRS2 + selector * SENSOR_REG_ROI_INTERVAL;
	data = (size >> 8) & SENSOR_REG_ROI_WIDTH_SIZE_UPPER_MASK;
	if ((status = sensorRegWriteByte (id, adrs, data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 有効／無効設定
	if ((status = sensorRoiSetValid (selector, SENSOR_REG_ROI_WIDTH_VALID, valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	ROI Width取得関数(センサレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//		pOffset	 			：Widthオフセットを格納するポインタ
//		pSize				：Widthサイズを格納するポインタ
//		pValid				：Valid状態を格納するポインタ(0=無効/0以外=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorRoiGetWidth (int selector, int *pOffset, int *pSize, int *pValid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char adrs, dataL, dataH;
	int acTmg = ROI_SENSIOR_ACC_TMG;
	int regHold = ROI_SENSIOR_REG_HOLD;
	unsigned char id = ROI_SENSIOR_CHIP_ID;

	// Check selector Parameter
	if ((selector < ROI_SELECTOR_MIN) || (selector > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		printf (gLogMsgBuff, "ROI Sensor Get Width selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Sensor Get Width pOffset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Sensor Get Width pSize NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pValid Parameter
	if (pValid == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Sensor Get Width pValid NULL Parameter Error.\n");
		goto _DONE;
	}

	// Widthオフセット下位設定
	adrs = SENSOR_REG_ROI_H1_POS_ADRS1 + selector * SENSOR_REG_ROI_INTERVAL;
	if ((status = sensorRegReadByte (id, adrs, &dataL, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Widthオフセット上位設定
	adrs = SENSOR_REG_ROI_H1_POS_ADRS2 + selector * SENSOR_REG_ROI_INTERVAL;
	if ((status = sensorRegReadByte (id, adrs, &dataH, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// オフセット取得
	*pOffset = (int)(((dataH & SENSOR_REG_ROI_WIDTH_START_UPPER_MASK) << 8) | (dataL & SENSOR_REG_ROI_WIDTH_START_LOWER_MASK));

	// センサ実サイズを考慮
	//*pOffset += ROI_WIDTH_OFFSET;

	// Widthサイズ下位取得
	adrs = SENSOR_REG_ROI_H1_WIDTH_ADRS1 + selector * SENSOR_REG_ROI_INTERVAL;
	if ((status = sensorRegReadByte (id, adrs, &dataL, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Widthサイズ上位取得
	adrs = SENSOR_REG_ROI_H1_WIDTH_ADRS2 + selector * SENSOR_REG_ROI_INTERVAL;
	if ((status = sensorRegReadByte (id, adrs, &dataH, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// サイズ取得
	*pSize = (int)(((dataH & SENSOR_REG_ROI_WIDTH_SIZE_UPPER_MASK) << 8) | (dataL & SENSOR_REG_ROI_WIDTH_SIZE_LOWER_MASK));

	// センサ実サイズを考慮
	//*pSize -= IMG_WIDTH_OFFSET_SIZE;

	// 有効／無効取得
	if ((status = sensorRoiGetValid (selector, SENSOR_REG_ROI_WIDTH_VALID, pValid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	ROI Height設定関数(センサレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//		offset				：Heightオフセット
//		size				：Heightサイズ
//		valid				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorRoiSetHeight (int selector, int offset, int size, int valid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char adrs, data;
	int acTmg = ROI_SENSIOR_ACC_TMG;
	int regHold = ROI_SENSIOR_REG_HOLD;
	unsigned char id = ROI_SENSIOR_CHIP_ID;
#if defined (MODE_SENSOR_DRRS)
	int drrsMode = MODE_DISABLE;
#endif

#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;

	// High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode(&hsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (hsMode == MODE_ENABLE)
		goto _DONE;	// センサへの設定はしない
#endif

	// センサ実サイズを考慮
	//size += IMG_HEIGHT_OFFSET_SIZE;

	// Check selector Parameter
	if ((selector < ROI_SELECTOR_MIN) || (selector  > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Height selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset Parameters
	if ((offset < ROI_HEIGHT_OFFSET_MIN) || (offset > ROI_HEIGHT_OFFSET_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Height Offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset, ROI_HEIGHT_OFFSET_MIN, ROI_HEIGHT_OFFSET_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Parameters
	if ((size < ROI_HEIGHT_SIZE_MIN) || (size > ROI_HEIGHT_SIZE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Height size(%d) Parameter Error.(Min:%d / Max:%d)\n", size, ROI_HEIGHT_SIZE_MIN, ROI_HEIGHT_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset + size Parameters
	if ((offset + size) > ROI_HEIGHT_SIZE_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Height Offset(%d) + Size(%d) = %d Parameter Error.(Min:%d / Max:%d)\n", offset, size , offset + size, ROI_HEIGHT_SIZE_MIN, ROI_HEIGHT_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset Align Parameters
	if ((offset % ROI_SENSOR_HEIGHT_OFFSET_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Height Offset Align(%d) Parameter Error. Align = %d.\n", offset, ROI_SENSOR_HEIGHT_OFFSET_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Align Parameters
	if ((size % ROI_SENSOR_HEIGHT_SIZE_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Height size Align(%d) Parameter Error. Align = %d.\n", size, ROI_SENSOR_HEIGHT_SIZE_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check Valid Parameters
	if ((valid != 0) && (valid != 1))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Height size Valid(%d) Parameter Error.\n", valid);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DRRSモード取得
	#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DRRS有効時のみHeight数を増やす
	if (drrsMode == MODE_ENABLE)
		size += DRRS_HEIGHT_ADD_SIZE;
	#endif

	// Heightオフセット下位設定
	adrs = SENSOR_REG_ROI_V1_POS_ADRS1 + selector * SENSOR_REG_ROI_INTERVAL;
	data = offset & SENSOR_REG_ROI_HEIGHT_START_LOWER_MASK;
	if ((status = sensorRegWriteByte (id, adrs, data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Heightオフセット上位設定
	adrs = SENSOR_REG_ROI_V1_POS_ADRS2 + selector * SENSOR_REG_ROI_INTERVAL;
	data = (offset >> 8) & SENSOR_REG_ROI_HEIGHT_START_UPPER_MASK;
	if ((status = sensorRegWriteByte (id, adrs, data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Heightサイズ下位設定
	adrs = SENSOR_REG_ROI_V1_WIDTH_ADRS1 + selector * SENSOR_REG_ROI_INTERVAL;
	data = size & SENSOR_REG_ROI_HEIGHT_SIZE_LOWER_MASK;
	if ((status = sensorRegWriteByte (id, adrs, data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Heightサイズ上位設定
	adrs = SENSOR_REG_ROI_V1_WIDTH_ADRS2 + selector * SENSOR_REG_ROI_INTERVAL;
	data = (size >> 8) & SENSOR_REG_ROI_HEIGHT_SIZE_UPPER_MASK;
	if ((status = sensorRegWriteByte (id, adrs, data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 有効／無効設定
	if ((status = sensorRoiSetValid (selector, SENSOR_REG_ROI_HEIGHT_VALID, valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	ROI Height取得関数(センサレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//		pOffset				：Heightオフセットを格納するポインタ
//		pSize				：Heightサイズを格納するポインタ
//		pValid				：Valid状態を格納するポインタ(0=無効/0以外=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorRoiGetHeight (int selector, int *pOffset, int *pSize, int *pValid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char adrs, dataL, dataH;
	int acTmg = ROI_SENSIOR_ACC_TMG;
	int regHold = ROI_SENSIOR_REG_HOLD;
	unsigned char id = ROI_SENSIOR_CHIP_ID;

	// Check selector Parameter
	if ((selector < ROI_SELECTOR_MIN) || (selector > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Get Height selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Sensor Get Height pOffset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Sensor Get Height pSize NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pValid Parameter
	if (pValid == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Sensor Get Height pValid NULL Parameter Error.\n");
		goto _DONE;
	}

	// Heightオフセット下位取得
	adrs = SENSOR_REG_ROI_V1_POS_ADRS1 + selector * SENSOR_REG_ROI_INTERVAL;
	if ((status = sensorRegReadByte (id, adrs, &dataL, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Heightオフセット上位取得
	adrs = SENSOR_REG_ROI_V1_POS_ADRS2 + selector * SENSOR_REG_ROI_INTERVAL;
	if ((status = sensorRegReadByte (id, adrs, &dataH, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// オフセット取得
	*pOffset = (int)(((dataH & SENSOR_REG_ROI_HEIGHT_START_UPPER_MASK) << 8) | (dataL & SENSOR_REG_ROI_HEIGHT_START_LOWER_MASK));

	// センサ実サイズを考慮
	//*pOffset -= IMG_WIDTH_OFFSET_SIZE;

	// Heightサイズ下位取得
	adrs = SENSOR_REG_ROI_V1_WIDTH_ADRS1 + selector * SENSOR_REG_ROI_INTERVAL;
	if ((status = sensorRegReadByte (id, adrs, &dataL, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Heightサイズ上位取得
	adrs = SENSOR_REG_ROI_V1_WIDTH_ADRS2 + selector * SENSOR_REG_ROI_INTERVAL;
	if ((status = sensorRegReadByte (id, adrs, &dataH, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// サイズ取得
	*pSize = (int)(((dataH & SENSOR_REG_ROI_HEIGHT_SIZE_UPPER_MASK) << 8) | (dataL & SENSOR_REG_ROI_HEIGHT_SIZE_LOWER_MASK));

	// センサ実サイズを考慮
	//*pSize -= IMG_HEIGHT_OFFSET_SIZE;

	// 有効／無効取得
	if ((status = sensorRoiGetValid (selector, SENSOR_REG_ROI_HEIGHT_VALID, pValid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	ROI Valid設定関数(センサレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//		selectWH			：Width/Heightセレクタ(1=Width/2=Height)
//		valid				：Valid状態(0=無効/0以外=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorRoiSetValid (int selector, int selectWH, int valid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char adrs, data, validReg;
	int shift;
	int acTmg = ROI_SENSIOR_ACC_TMG;
	int regHold = ROI_SENSIOR_REG_HOLD;
	unsigned char id = ROI_SENSIOR_CHIP_ID;

	// Check selector Parameter
	if ((selector < ROI_SELECTOR_MIN) || (selector > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Valid selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check selectWH Parameter
	if ((selectWH != SENSOR_REG_ROI_WIDTH_VALID) && (selectWH != SENSOR_REG_ROI_HEIGHT_VALID))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Valid selectorWH(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, SENSOR_REG_ROI_WIDTH_VALID, SENSOR_REG_ROI_HEIGHT_VALID);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// valid設定下位設定
	if (selector < 4)
	{
		adrs = SENSOR_REG_ROI_ON1_ADRS;
		shift = selector * 2;
	}
	else
	{
		adrs = SENSOR_REG_ROI_ON2_ADRS;
		shift = (selector - 4) * 2;
	}

	// 水平＆垂直選択
	if (selectWH == SENSOR_REG_ROI_WIDTH_VALID)
		validReg = SENSOR_REG_ROI_H_VALID << shift;
	else
		validReg = SENSOR_REG_ROI_V_VALID << shift;

	// Read
	if ((status = sensorRegReadByte (id, adrs, &data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (valid == 0)	// 無効
		data &= ~validReg;
	else			// 有効
		data |=	validReg;

	// Write
	if ((status = sensorRegWriteByte (id, adrs, data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	ROI Valid取得関数(センサレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		selector			：セレクタ
//		selectWH			：Width/Heightセレクタ(1=Width/2=Height)
//		pValid				：Valid状態を格納するポインタ(0=無効/0以外=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorRoiGetValid (int selector, int selectWH, int *pValid)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char adrs, data, validReg;
	int shift;
	int acTmg = ROI_SENSIOR_ACC_TMG;
	int regHold = ROI_SENSIOR_REG_HOLD;
	unsigned char id = ROI_SENSIOR_CHIP_ID;

	// Check selector Parameter
	if ((selector < ROI_SELECTOR_MIN) || (selector  > ROI_SELECTOR_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Get Valid selector(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, ROI_SELECTOR_MIN, ROI_SELECTOR_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check selectWH Parameter
	if ((selectWH != SENSOR_REG_ROI_WIDTH_VALID) && (selectWH != SENSOR_REG_ROI_HEIGHT_VALID))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "ROI Sensor Set Valid selectorWH(%d) Parameter Error.(Min:%d / Max:%d)\n", selector, SENSOR_REG_ROI_WIDTH_VALID, SENSOR_REG_ROI_HEIGHT_VALID);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pValid Parameter
	if (pValid == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI Sensor Get Valid pValid NULL Parameter Error.\n");
		goto _DONE;
	}

	// valid設定下位設定
	if (selector < 4)
	{
		adrs = SENSOR_REG_ROI_ON1_ADRS;
		shift = selector * 2;
	}
	else
	{
		adrs = SENSOR_REG_ROI_ON2_ADRS;
		shift = (selector - 4) * 2;
	}

	// 水平＆垂直選択
	if (selectWH == SENSOR_REG_ROI_WIDTH_VALID)
		validReg = SENSOR_REG_ROI_H_VALID << shift;
	else
		validReg = SENSOR_REG_ROI_V_VALID << shift;

	// Read
	if ((status = sensorRegReadByte (id, adrs, &data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((data & validReg) == 0)
		*pValid = 0;
	else
		*pValid = 1;

_DONE:
	return (status);
}

#endif // #if defined (MODE_ROI_VERSION2)

// eof
