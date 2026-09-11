//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// gpio.c - GPIO Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	GPI取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：GPO出力データ（bit0=GP0/bit1=GP1）を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getGpi (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data = 0;
	unsigned int i;
	int mode;
	unsigned int count;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "GPI pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// 個数取得
	count = GENICAM_DIGITAL_LINE_SELECT_LINE_MAX;

	// 設定
	for (i=0; i<count; i++)
	{
		// Line選択
		if ((status = digitalIoSetLineSelect ((GENICAM_DIGITAL_LINE_SELECT_LINE0 + i))) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Direction
		if ((status = digitalIoGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Mode(出力ならば何もしない)
		if (mode == GENICAM_DIGITAL_LINE_MODE_OUTPUT)
			continue;

		// Line Status
		if ((status = digitalIoGetLineStatus (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (mode == 1)
			data |= (1<<i);
	}

	// Data取得
	*pData = data;

_DONE:
	return (status);
}


//**********************************************************************************
//	GPO取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：GPO出力データ（bit0=GP0/bit1=GP1）を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getGpo (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	int data;
	int output = 0;
	unsigned int i;
	int mode;
	unsigned int count;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "GPO pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// 個数取得
	count = GENICAM_DIGITAL_LINE_SELECT_LINE_MAX;

	// 設定
	for (i=0; i<count; i++)
	{
		// Line選択
		if ((status = digitalIoSetLineSelect ((GENICAM_DIGITAL_LINE_SELECT_LINE0 + i))) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Line Direction
		if ((status = digitalIoGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Input?
		if (mode == GENICAM_DIGITAL_LINE_MODE_INPUT)
			continue;

		// Line Source
		if ((status = digitalIoSetLineSource (GENICAM_DIGITAL_LINE_SOURCE_USER0_OUTPUT + i*2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// User Selector
		if ((status = digitalIoSetUserSelect (DIGITAL_USER0_SELECT+i)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get User Value
		if ((status = digitalIoGetUserValue (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (data == DIGITAL_USER_VALUE_ASSERT)
			output |= (1<<i);
	}

	*pData = output;

_DONE:
	return (status);
}


//**********************************************************************************
//	GPO設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：GPO出力データ（bit0=GP0/bit1=GP1）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setGpo (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	int mode;
	unsigned int output;
	unsigned int i;
	unsigned int count;
	unsigned int mask;

	// 個数取得
	count = GENICAM_DIGITAL_LINE_SELECT_LINE_MAX;

	// User Mask
	mask = 0;
	for (i=0; i<count; i++)
		mask |= (1 << i);

	if ((status = digitalIoSetUserMask (mask)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 設定
	for (i=0; i<count; i++)
	{
		// Line選択
		if ((status = digitalIoSetLineSelect ((GENICAM_DIGITAL_LINE_SELECT_LINE0 + i))) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Line Direction
		if ((status = digitalIoGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Input?
		if (mode == GENICAM_DIGITAL_LINE_MODE_INPUT)
			continue;

		// Line Source
		if ((status = digitalIoSetLineSource ((GENICAM_DIGITAL_LINE_SOURCE_USER0_OUTPUT + i*2))) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Line Format
		//if ((status = digitalIoSetLineFormat (DIGITAL_LINE_FORMAT_RS422)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// User Selector
		if ((status = digitalIoSetUserSelect (i)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (data & (1<<i))
			output = DIGITAL_USER_VALUE_ASSERT;
		else
			output = DIGITAL_USER_VALUE_NEGATE;

		// Set User Value
		if ((status = digitalIoSetUserValue (output)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Digital Noise Filter設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number				：GPIO Number
//		highData			：デジタルノイズフィルタHigh(us)
//		lowData				：デジタルノイズフィルタLow(us)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gpioSetDnf (int number, unsigned int highData, unsigned int lowData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	unsigned int dir;

	// Check number Parameter
	if ((number < GPIO_LINE_MIN) || (number > GPIO_LINE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Noise Filter Number(%d) Parameter Error.(Min:%d / Max:%d)\n", number, GPIO_LINE_MIN, GPIO_LINE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 入出力方法取得
	dir = IN32 ((GENICAM_DIGITAL_LINE0_MODE_ADRS + (GENICAM_DIGITAL_LINE_INTERVAL * number)));

	// OutputはError
	if (dir == GENICAM_DIGITAL_LINE_MODE_OUTPUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Noise Filter Direction Mode Parameter Error.\n");
		goto _DONE;
	}

	// Check highData Parameter
	if (highData > GPIO_DNF_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Noise Filter High Data(%d) Parameter Error.(Min:0 / Max:%d)\n", highData, GPIO_DNF_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check lowData Parameter
	if (lowData > GPIO_DNF_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Noise Filter Low Data(%d) Parameter Error.(Min:0 / Max:%d)\n", lowData, GPIO_DNF_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// データ作成
	data = SET_DNF_DATA(highData, lowData);

	// 設定
	OUT32 ((GENICAM_DIGITAL_LINE0_DNF_ADRS + (GENICAM_DIGITAL_LINE_INTERVAL * number)), data);

_DONE:
	return (status);
}


//**********************************************************************************
//	Digital Noise Filter取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number				：GPIO Number
//		pHighData			：デジタルノイズHighフィルタ(us)を格納するポインタ
//		pLowData			：デジタルノイズLowフィルタ(us)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gpioGetDnf (int number, unsigned int *pHighData, unsigned int *pLowData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	unsigned int dir;

	// Check number Parameter
	if ((number < GPIO_LINE_MIN) || (number > GPIO_LINE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Noise Filter Number(%d) Parameter Error.(Min:%d / Max:%d)\n", number, GPIO_LINE_MIN, GPIO_LINE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 入出力方法取得
	dir = IN32 ((GENICAM_DIGITAL_LINE0_MODE_ADRS + (GENICAM_DIGITAL_LINE_INTERVAL * number)));

	// OutputはError
	if (dir == GENICAM_DIGITAL_LINE_MODE_OUTPUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Noise Filter Direction Mode Parameter Error.\n");
		goto _DONE;
	}

	// Check pHighData Parameter
	if (pHighData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Noise Filter pHighData NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pLowData Parameter
	if (pLowData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Digital Noise Filter pLowData NULL Parameter Error.\n");
		goto _DONE;
	}

	// DNF取得
	data = IN32 ((GENICAM_DIGITAL_LINE0_DNF_ADRS + (GENICAM_DIGITAL_LINE_INTERVAL * number)));

	// High Data
	*pHighData = GET_DNF_HIGH_DATA (data);

	// Low Data
	*pLowData = GET_DNF_LOW_DATA (data);

_DONE:
	return (status);
}


//**********************************************************************************
//	GPIOパルス幅時間取得（時間単位）
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		number				：GPIモード
//		pHighFreq			：Highパルス幅(us)を格納するポインタ
//		pLowFreq			：Lowパルス幅(us)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gpioGetPulseTime (int number, unsigned int *pHighFreq, unsigned int *pLowFreq)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	unsigned int dir;

	// Check number Parameter
	if ((number < GPIO_LINE_MIN) || (number > GPIO_LINE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "GPI Pulse Width Time Number(%d) Parameter Error.(Min:%d / Max:%d)\n", number, GPIO_LINE_MIN, GPIO_LINE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pHighFreq Parameter
	if (pHighFreq == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "GPI Pulse Width Time pHighFreq NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pLowFreq Parameter
	if (pLowFreq == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "GPI Pulse Width Time pLowFreq NULL Parameter Error.\n");
		goto _DONE;
	}

	// 入出力方法取得
	dir = IN32 ((GENICAM_DIGITAL_LINE0_MODE_ADRS + (GENICAM_DIGITAL_LINE_INTERVAL * number)));

	//--------------------------------------------------
	// High Pulse
	//--------------------------------------------------

	// Pulse High 取得(入力はHighとLowのレジスタが逆)
	if (dir == GENICAM_DIGITAL_LINE_MODE_OUTPUT)
		data = IN32 ((GENICAM_DIGITAL_LINE0_PWC_H_ADRS + (GENICAM_DIGITAL_LINE_PWC_INTERVAL * number)));
	else
	{
		if ((number == 0) || (number == 1))
			data = IN32 ((GENICAM_DIGITAL_LINE0_PWC_H_ADRS + (GENICAM_DIGITAL_LINE_PWC_INTERVAL * number)));
		else
			data = IN32 ((GENICAM_DIGITAL_LINE0_PWC_L_ADRS + (GENICAM_DIGITAL_LINE_PWC_INTERVAL * number)));
	}

	// Check Measure Over
	if (data & FPGA_GPIO_PULSE_OVER_BIT)
	{
		// Highパルス取得
		*pHighFreq = 0;
	}
	else
	{
		// Highパルス取得
		*pHighFreq = FPGA_GPIO_PULSE_TIME (data);
	}

	//--------------------------------------------------
	// Low Pulse
	//--------------------------------------------------

	// Pulse Low 取得(入力はHighとLowのレジスタが逆)
	if (dir == GENICAM_DIGITAL_LINE_MODE_OUTPUT)
		data = IN32 ((GENICAM_DIGITAL_LINE0_PWC_L_ADRS + (GENICAM_DIGITAL_LINE_PWC_INTERVAL * number)));
	else
	{
		if ((number == 0) || (number == 1))
			data = IN32 ((GENICAM_DIGITAL_LINE0_PWC_L_ADRS + (GENICAM_DIGITAL_LINE_PWC_INTERVAL * number)));
		else
			data = IN32 ((GENICAM_DIGITAL_LINE0_PWC_H_ADRS + (GENICAM_DIGITAL_LINE_PWC_INTERVAL * number)));
	}

	// Check Measure Over
	if (data & FPGA_GPIO_PULSE_OVER_BIT)
	{
		// Lowパルス取得
		*pLowFreq = 0;
	}
	else
	{
		// Lowパルス取得
		*pLowFreq = FPGA_GPIO_PULSE_TIME (data);
	}

_DONE:
	return (status);
}

// eof
