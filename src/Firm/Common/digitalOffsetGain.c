//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// digitalOffsetGain.c - Digital Offset Gain Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define NUM_LINE_OFFSET_IMG_FOR_DETECT			(1)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// Digital Offset Gain Status
int gDogStatus = 0;


//**********************************************************************************
// Digital Offset Gain初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dogInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT ("Digital Offset Gain Initialize\n");

	// Disable
	dogSetUpdate (DOG_DISABLE);

	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_OG_ADRS, CAMERA_SAVE_OG_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gDogStatus = status;
		goto _DONE;
	}

#if defined (MODE_IPU_MULTI)
	// Register Data Restore
	if ((status = cameraParamWriteRegisterOffsetAdrs (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_OG_ADRS, CAMERA_SAVE_OG_SIZE, FPGA_DOG2_OFFSET)) != AVAL_STATUS_SUCCESS)
	{
		gDogStatus = status;
		goto _DONE;
	}
#endif

	// 更新設定
	if ((status = dogSetUpdate (DOG_ENABLE)) != AVAL_STATUS_SUCCESS)
	{
		gDogStatus = status;
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset Gain設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Digital Offset Gain更新設定(0=無効/1=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dogSetUpdate (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check mode Parameter
	if ((mode != DOG_ENABLE) && (mode != DOG_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Offset Gain Update mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, DOG_DISABLE, DOG_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Digital Offset Gain取得
	data = IN32 (FPGA_DOG_CTRL_ADRS);

	if (mode == DOG_ENABLE)
		data |= FPGA_DOG_CTRL_UPDATE_BIT;
	else
		data &= ~FPGA_DOG_CTRL_UPDATE_BIT;
	
	// Digital Offset Gain設定
	OUT32 (FPGA_DOG_CTRL_ADRS, data);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DOG2_CTRL_ADRS, data);
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset Gain設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：更新情報を格納するポインタ(0=無効/1=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dogGetUpdate (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Offset Gain Update pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Digital Offset Gain取得
	data = IN32 (FPGA_DOG_CTRL_ADRS);

	if (data & FPGA_DOG_CTRL_UPDATE_BIT)
		*pMode = DOG_ENABLE;
	else
		*pMode = DOG_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset/Gain設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		offset1				：オフセット1
//		offset2				：オフセット2
//		gain				：ゲイン
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dogSetOffsetGain (int offset1, int offset2, float gain)
{
	int status = AVAL_STATUS_SUCCESS;
	int offset1Min, offset1Max;
	int offset2Min, offset2Max;

	// Offset Max/Min
	if ((status = dogGetOffsetMaxMin (&offset1Min, &offset1Max, &offset2Min, &offset2Max)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check offset1 Parameter
	if ((offset1 < offset1Min) || (offset1 > offset1Max))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Dog offset1(%d) Parameter Error. (Min:%d / Max:%d)\n", offset1, offset1Min, offset1Max);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset2 Parameter
	if ((offset2 < offset2Min) || (offset2 > offset2Max))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Dog offset2(%d) Parameter Error. (Min:%d / Max:%d)\n", offset2, offset2Min, offset2Max);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gain Parameter
#ifdef MODE_DOG_DECIBEL
	if ((gain < DOG_DECIBEL_MIN) || (gain > DOG_DECIBEL_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Dog gain(%f) Parameter Error. (Min:%.2f / Max:%.2f)\n", gain, DOG_DECIBEL_MIN, DOG_DECIBEL_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
#else
	if ((gain < DOG_GAIN_MIN) || (gain > DOG_GAIN_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Dog gain(%f) Parameter Error. (Min:2f / Max:2f)\n", gain, DOG_GAIN_MIN, DOG_GAIN_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
#endif

	// Digital Offset1設定
	if ((status = digitalSetOffset1 (offset1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Digital Offset2設定
	if ((status = digitalSetOffset (offset2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Digital Gain設定
	if ((status = digitalSetGain (gain)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset/Gain取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pOffset1			：オフセット1を格納するポインタ
//		pOffset2			：オフセット2を格納するポインタ
//		pGain				：ゲインを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dogGetOffsetGain (int *pOffset1, int *pOffset2, float *pGain)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pOffset1 Parameter
	if (pOffset1 == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Dog Offset Gain pOffset1 NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pOffset2 Parameter
	if (pOffset2 == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Dog Offset Gain pOffset2 NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pGain Parameter
	if (pGain == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Dog Offset Gain pGain NULL Parameter Error\n");
		goto _DONE;
	}

	// Digital Offset1取得
	if ((status = digitalGetOffset1 (pOffset1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Digital Offset2取得
	if ((status = digitalGetOffset (pOffset2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Digital Gain取得
	if ((status = digitalGetGain (pGain)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset Max/Min取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pOffset1Min			：Offset1最小値を格納するポインタ
//		pOffset1Max			：Offset1最大値を格納するポインタ
//		pOffset2Min			：Offset2最小値を格納するポインタ
//		pOffset2Max			：Offset2最大値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dogGetOffsetMaxMin (int *pOffset1Min, int *pOffset1Max, int *pOffset2Min, int *pOffset2Max)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;

	// Check pOffset1Min Parameter
	if (pOffset1Min == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Dog Offset Max/Min pOffset1Min NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pOffset1Max Parameter
	if (pOffset1Max == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Dog Offset Max/Min pOffset1Max NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pOffset2Min Parameter
	if (pOffset2Min == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Dog Offset Max/Min pOffset2Min NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pOffset2Max Parameter
	if (pOffset2Max == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Dog Offset Max/Min pOffset2Max NULL Parameter Error\n");
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Offset Max/Min
	if (bit == 8)
	{
		*pOffset1Min = DOG_OFFSET1_8BIT_MIN;
		*pOffset1Max = DOG_OFFSET1_8BIT_MAX;
		*pOffset2Min = DOG_OFFSET2_8BIT_MIN;
		*pOffset2Max = DOG_OFFSET2_8BIT_MAX;
	}
	else if (bit == 10)
	{
		*pOffset1Min = DOG_OFFSET1_10BIT_MIN;
		*pOffset1Max = DOG_OFFSET1_10BIT_MAX;
		*pOffset2Min = DOG_OFFSET2_10BIT_MIN;
		*pOffset2Max = DOG_OFFSET2_10BIT_MAX;
	}
	else if (bit == 12)
	{
		*pOffset1Min = DOG_OFFSET1_12BIT_MIN;
		*pOffset1Max = DOG_OFFSET1_12BIT_MAX;
		*pOffset2Min = DOG_OFFSET2_12BIT_MIN;
		*pOffset2Max = DOG_OFFSET2_12BIT_MAX;
	}
	else if (bit == 14)
	{
		*pOffset1Min = DOG_OFFSET1_14BIT_MIN;
		*pOffset1Max = DOG_OFFSET1_14BIT_MAX;
		*pOffset2Min = DOG_OFFSET2_14BIT_MIN;
		*pOffset2Max = DOG_OFFSET2_14BIT_MAX;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Dog offset Bit(%d) Parameter Error.\n", bit);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset Max/Min取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pOffset1Min			：Offset1最小値を格納するポインタ
//		pOffset1Max			：Offset1最大値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dogGetOffset1MaxMin (int *pOffset1Min, int *pOffset1Max)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;

	// Check pOffset1Min Parameter
	if (pOffset1Min == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Dog Offset Max/Min pOffset1Min NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pOffset1Max Parameter
	if (pOffset1Max == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Dog Offset Max/Min pOffset1Max NULL Parameter Error\n");
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Offset Max/Min
	if (bit == 8)
	{
		*pOffset1Min = DOG_OFFSET1_8BIT_MIN;
		*pOffset1Max = DOG_OFFSET1_8BIT_MAX;
	}
	else if (bit == 10)
	{
		*pOffset1Min = DOG_OFFSET1_10BIT_MIN;
		*pOffset1Max = DOG_OFFSET1_10BIT_MAX;
	}
	else if (bit == 12)
	{
		*pOffset1Min = DOG_OFFSET1_12BIT_MIN;
		*pOffset1Max = DOG_OFFSET1_12BIT_MAX;
	}
	else if (bit == 14)
	{
		*pOffset1Min = DOG_OFFSET1_14BIT_MIN;
		*pOffset1Max = DOG_OFFSET1_14BIT_MAX;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Dog offset Bit(%d) Parameter Error.\n", bit);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset Max/Min取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pOffset2Min			：Offset2最小値を格納するポインタ
//		pOffset2Max			：Offset2最大値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dogGetOffset2MaxMin (int *pOffset2Min, int *pOffset2Max)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;

	// Check pOffset2Min Parameter
	if (pOffset2Min == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Offset Max/Min pOffset2Min NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pOffset2Max Parameter
	if (pOffset2Max == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Offset Max/Min pOffset2Max NULL Parameter Error\n");
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Offset Max/Min
	if (bit == 8)
	{
		*pOffset2Min = DOG_OFFSET2_8BIT_MIN;
		*pOffset2Max = DOG_OFFSET2_8BIT_MAX;
	}
	else if (bit == 10)
	{
		*pOffset2Min = DOG_OFFSET2_10BIT_MIN;
		*pOffset2Max = DOG_OFFSET2_10BIT_MAX;
	}
	else if (bit == 12)
	{
		*pOffset2Min = DOG_OFFSET2_12BIT_MIN;
		*pOffset2Max = DOG_OFFSET2_12BIT_MAX;
	}
	else if (bit == 14)
	{
		*pOffset2Min = DOG_OFFSET2_14BIT_MIN;
		*pOffset2Max = DOG_OFFSET2_14BIT_MAX;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Offset Bit(%d) Parameter Error.\n", bit);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Gain設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		gain				：ゲイン
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalSetGain (float gain)
{
	int status = AVAL_STATUS_SUCCESS;
	int data;
	double magnification;

	// Check gain Parameter
	if ((gain < DOG_DECIBEL_MIN) || (gain > DOG_DECIBEL_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Gain(%f) Parameter Error. (Min:%.2f / Max:%.2f)\n", gain, DOG_DECIBEL_MIN, DOG_DECIBEL_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DB=>倍率
	magnification = pow(10, gain/20);

	// Digital Gain設定 (double値をレジスタ設定値に変換)
	data = DOG_GAIN_VALUE_TO_REG(magnification);

	if (data > FPGA_DOG_GAIN_MASK)
		data = FPGA_DOG_GAIN_MASK;

	// 負数?
	if (gain < 0)
		data |= FPGA_DOG_GAIN_CODE;

	OUT32 (FPGA_DOG_GAIN_ADRS, (data & FPGA_DOG_GAIN_MASK));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DOG2_GAIN_ADRS, (data & FPGA_DOG_GAIN_MASK));
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Gain取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pGain				：ゲインを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalGetGain (float *pGain)
{
	int status = AVAL_STATUS_SUCCESS;
	int data;
	double magnification;
	float dbData;
	int flag = 0;
	
	// Check pGain Parameter
	if (pGain == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Gain pGain NULL Parameter Error\n");
		goto _DONE;
	}

	// Digital Gain取得
	data = IN32 (FPGA_DOG_GAIN_ADRS) & FPGA_DOG_GAIN_MASK;
	if (data & FPGA_DOG_GAIN_CODE)
	{
		flag = 1;
		data &= ~FPGA_DOG_GAIN_CODE;
	}

	 // 倍率データ取得
	 magnification = (double)DOG_GAIN_REG_TO_VALUE (data);

	 // 倍率 => Decibel変換
	 dbData = 20 * log10(magnification);

	// 負数?
	if (flag == 1)
		dbData = 0 - dbData;
	
	*pGain = dbData;

_DONE:
	return (status);
}


//**********************************************************************************
// Gain設定(倍率)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		gain				：ゲイン
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalSetGainX (float gain)
{
	int status = AVAL_STATUS_SUCCESS;
	int data;
	double gainDouble;

	gain += 0.005; //四捨五入

	// 小数点第2位まで有効
	data = gain * DIGITAL_GAIN_VALID_POINT;
	gainDouble = (double)((double)data / (double)DIGITAL_GAIN_VALID_POINT);

	// Check gain Parameter
	if ((gainDouble < DOG_GAIN_MIN) || (gainDouble > DOG_GAIN_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Gain(%lf) Parameter Error. (Min:%.2f / Max:%.2f)\n", gainDouble, DOG_GAIN_MIN, DOG_GAIN_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Digital Gain設定 (double値をレジスタ設定値に変換)
	data = DOG_GAIN_VALUE_TO_REG(gainDouble);

	// 負数?
	//if (gain < 0)
		//data |= FPGA_DOG_GAIN_CODE;

	OUT32 (FPGA_DOG_GAIN_ADRS, (data & FPGA_DOG_GAIN_MASK));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DOG2_GAIN_ADRS, (data & FPGA_DOG_GAIN_MASK));
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Gain取得(倍率)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pGain				：ゲインを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalGetGainX (float *pGain)
{
	int status = AVAL_STATUS_SUCCESS;
	int data;
	int flag = 0;
	float gain;
	
	// Check pGain Parameter
	if (pGain == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Gain pGain NULL Parameter Error\n");
		goto _DONE;
	}

	// Digital Gain取得
	data = IN32 (FPGA_DOG_GAIN_ADRS) & FPGA_DOG_GAIN_MASK;
	if (data & FPGA_DOG_GAIN_CODE)
	{
		flag = 1;
		data &= ~FPGA_DOG_GAIN_CODE;
	}

	gain = (float)DOG_GAIN_REG_TO_VALUE (data);

	// 負数?
	if (flag == 1)
		gain = 0 - gain;

	 *pGain = gain;

_DONE:
	return (status);
}


//**********************************************************************************
// Gain設定(レジスタ値)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		gain				：ゲイン
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalSetGainReg (unsigned int gain)
{
	int status = AVAL_STATUS_SUCCESS;

	OUT32 (FPGA_DOG_GAIN_ADRS, (gain & FPGA_DOG_GAIN_MASK));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DOG2_GAIN_ADRS, (gain & FPGA_DOG_GAIN_MASK));
#endif

	return (status);
}


//**********************************************************************************
// Gain取得(レジスタ値)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pGain				：ゲインを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalGetGainReg (unsigned int *pGain)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pGain Parameter
	if (pGain == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Gain pGain NULL Parameter Error\n");
		goto _DONE;
	}

	*pGain = IN32 (FPGA_DOG_GAIN_ADRS) & FPGA_DOG_GAIN_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset設定(オフセット2)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		offset				：オフセット
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalSetOffset (int offset)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int black;
	int offset2Min, offset2Max;
	int data;
	int bit, shift;

	// Offset Max/Min
	if ((status = dogGetOffset2MaxMin (&offset2Min, &offset2Max)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check offset Parameter
	if ((offset < offset2Min) || (offset > offset2Max))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Offset(%d) Parameter Error. (Min:%d / Max:%d)\n", offset, offset2Min, offset2Max);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 黒レベル取得
	ffcGetBlackTarget (&black);

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 設定値
	data = black + (offset << shift);

	// 最小値調整
	if (data < DOG_OFFSET2_14BIT_MIN)
		data = DOG_OFFSET2_14BIT_MIN;

	// 最大値調整
	if (data > DOG_OFFSET2_14BIT_MAX)
		data = DOG_OFFSET2_14BIT_MAX;

	// Digital Offset2設定
	OUT32 (FPGA_DOG_OFFSET2_ADRS, data);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DOG2_OFFSET2_ADRS, data);
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset取得(オフセット2)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pOffset				：オフセットを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalGetOffset (int *pOffset)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int black;
	int data;
	int bit, shift;

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Offset pOffset NULL Parameter Error\n");
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 黒レベル取得
	ffcGetBlackTarget (&black);

	// 設定値
	data = IN32 (FPGA_DOG_OFFSET2_ADRS);
	data -= black;
	data >>= shift;

	// Digital Offset取得
	*pOffset = data;

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset設定(オフセット１)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		offset1				：オフセット1
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalSetOffset1 (int offset)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int black;
	int offset1Min, offset1Max;
	int bit, shift;
	int data;

	// Offset Max/Min
	if ((status = dogGetOffset1MaxMin (&offset1Min, &offset1Max)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check offset Parameter
	if ((offset < offset1Min) || (offset > offset1Max))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Offset1(%d) Parameter Error. (Min:%d / Max:%d)\n", offset, offset1Min, offset1Max);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 黒レベル取得
	ffcGetBlackTarget (&black);

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 設定値
	data = (offset << shift) - black;

	// 最小値調整
	if (data < DOG_OFFSET2_14BIT_MIN)
		data = DOG_OFFSET2_14BIT_MIN;

	// 最大値調整
	if (data > DOG_OFFSET2_14BIT_MAX)
		data = DOG_OFFSET2_14BIT_MAX;

	// Digital Offset1設定
	OUT32 (FPGA_DOG_OFFSET1_ADRS, data);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DOG2_OFFSET1_ADRS, data);
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset1取得(オフセット１)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pOffset				：オフセットを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int digitalGetOffset1 (int *pOffset)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int black;
	int bit, shift;
	int data;

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Offset Gain pOffset NULL Parameter Error\n");
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// シフト数取得
	if ((status = aoiGetShift (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 黒レベル取得
	ffcGetBlackTarget (&black);

	// 設定値
	data = IN32 (FPGA_DOG_OFFSET1_ADRS);
	data += black;
	data >>= shift;

	// Digital Offset1取得
	*pOffset = data;

_DONE:
	return (status);
}


//**********************************************************************************
//	LineBlack OB左右反転
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：MODE_DISABLE=正転/MODE_ENABLE=反転
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int opticalBlackSetInvert (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int hobCtrl;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Optical Black Invert mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if (mode == MODE_ENABLE)
	{
		hobCtrl = IN32 (FPGA_LINE_BLACK_AUTO_CTRL_ADRS);
		hobCtrl |= FPGA_LINE_BLACK_AUTO_INVERT;
		OUT32 (FPGA_LINE_BLACK_AUTO_CTRL_ADRS, hobCtrl);
	}
	else
	{
		hobCtrl = IN32 (FPGA_LINE_BLACK_AUTO_CTRL_ADRS);
		hobCtrl &= ~FPGA_LINE_BLACK_AUTO_INVERT;
		OUT32 (FPGA_LINE_BLACK_AUTO_CTRL_ADRS, hobCtrl);
	}

_DONE:
	return (status);
}

// eof
