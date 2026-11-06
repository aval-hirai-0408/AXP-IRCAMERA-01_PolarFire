//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// peltier.c - Peltier Control Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gPeltierSensorUpperTempCount = 0;	// センサ温度上限カウント
int gPeltierSensorLowerTempCount = 0;	// センサ温度下限カウント
int gPeltierCaseTempCount = 0;			// ケース温度カウント
int gPeltierFpgaTempCount = 0;			// FPGA温度カウント

// Peltier Status
int gPeltierStatus = 0;


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern miv_plic_instance_t g_plic;


//**********************************************************************************
//	初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierInitialize (void)
{
	int status;
	unsigned int adrs, data;
	double tempH, tempL;
	double tempMin, tempMax;
	short datai16;
#ifdef MODE_PELTIER
	double temp;
	int datai;
#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	int peltierState = MODE_ENABLE;
#endif
#endif
#if defined (MODE_PELTIER_CTRL)
	int powerLevel;
#endif

	//@@@1
	DEBUG_PRINT_FORCE("@@@Pertier Star\n");
	//@@@1
	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "Peltier Initialize\n");

#if 0	//@@@1
	// FPGA割り込み無効
	fpgaSetInterrupt (0);

	// Peltier Disable
	peltierDisable ();

	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_PELTIER_ADRS, CAMERA_SAVE_PERTIER_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gPeltierStatus = status;
		goto _DONE;
	}

#if defined (MODE_PELTIER_CTRL)
	// Start Flag
	peltierSetPowerStartFlag (0);

	// Get Power Level
	peltierGetPowerLevel (&powerLevel);

	// Check Power Level
	if ((powerLevel < PELTIER_POWER_LEVEL_MIN) || (powerLevel > PELTIER_POWER_LEVEL_MAX))
		powerLevel = PELTIER_POWER_LEVEL_HIGH;

	// Set Power Level
	peltierSetPowerLevel (powerLevel);
#endif

	//------------------------------------------------------------
	// 温度異常ステータスクリア
	//------------------------------------------------------------
#if defined (MODE_TEMP_ABNORMAL_CHECK)
	// Temp Abnormal Status設定
	tempSetAbnormalStatus (0);
	
	// Temp Abnormal Count設定
	tempSetAbnormalCount (0);
	
	// Temp Abnormal Aqquisition設定
	tempSetAbnormalAcquisition (0);
#endif

	//------------------------------------------------------------
	// Peltier 搭載／非搭載の確認
	//------------------------------------------------------------
#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	peltierState = MODE_ENABLE;
#else
	peltierState = MODE_DISABLE;
#endif
	
	// ペルチェ実装状態
	if ((status = peltierSetMountState (peltierState)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// Sensor Alarm
	//------------------------------------------------------------

	// Sensor Alarm温度初期化
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_SENSOR_LIMIT_TEMP_ADRS, &adrs, &data);
	
	// 上限温度
	datai16 = (short)(data >> 16);
	tempH = (double)((double)datai16 / (double)CAMERA_SAVE_TEMP_UNIT);

	// 下限温度
	datai16 = (short)data;
	tempL = (double)((double)datai16 / (double)CAMERA_SAVE_TEMP_UNIT);

	// 温度上下限値Check
	peltierGetSensorTempAlarmMinMax (&tempMin, &tempMax);

	// 温度下限値Check
	if (tempMin > tempL)
		tempL = tempMin;

	// 温度上限値Check
	if (tempMax < tempL)
		tempL = tempMax;

	// 温度下限値Check
	if (tempMin > tempH)
		tempH = tempMin;

	// 温度上限値Check
	if (tempMax < tempH)
		tempH = tempMax;

	// Sensor Alarm設定
	peltierSetSensorTempAlarm (tempH, tempL);

	
	//------------------------------------------------------------
	// Case Alarm
	//------------------------------------------------------------

	// Case Alarm温度初期化
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_CASE_LIMIT_TEMP_ADRS, &adrs, &data);

	// 上限温度
	datai16 = (short)(data >> 16);
	tempH = (double)((double)datai16 / (double)CAMERA_SAVE_TEMP_UNIT);

	// 下限温度
	datai16 = (short)data;
	tempL = (double)((double)datai16 / (double)CAMERA_SAVE_TEMP_UNIT);

	// Case Alarm設定
	peltierSetCaseTempAlarm (tempH, tempL);
#endif //@@@1

	//------------------------------------------------------------
	// FPGA Alarm
	//------------------------------------------------------------

	//@@@1
	DEBUG_PRINT_FORCE("FPGA Alarm\n");
	//@@@1
	
	// FPGA Alarm温度初期化
	//@@@1cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_FPGA_LIMIT_TEMP_ADRS, &adrs, &data);

	//@@1
	data = (unsigned int)((unsigned int)(DIAG_TEMP_FPGA_MAX * CAMERA_SAVE_TEMP_UNIT)<<16) | (unsigned int)(DIAG_TEMP_FPGA_MIN * CAMERA_SAVE_TEMP_UNIT);
	//@@1

	// 上限温度
	datai16 = (short)(data >> 16);
	tempH = (double)((double)datai16 / (double)CAMERA_SAVE_TEMP_UNIT);

	// 下限温度
	datai16 = (short)data;
	tempL = (double)((double)datai16 / (double)CAMERA_SAVE_TEMP_UNIT);

	//@@@1
	DEBUG_PRINT_FORCE("Data  = 0x%x\n", data);
	DEBUG_PRINT_FORCE("tempH = %f\n", tempH);
	DEBUG_PRINT_FORCE("tempL = %f\n", tempL);
	//@@@1

	// Alarm設定
	fpgaSetTempAlarm (tempH, tempL);

	// Set Interrupt Enable
	fpgaSetInterrupt (FPGA_INT_TEMP_UPPER);

#if 0	//@@@1
	//------------------------------------------------------------
	// Target温度設定
	//------------------------------------------------------------

#if defined (MODE_PELTIER)
	//ペルチェ実装時
	if (peltierState == MODE_ENABLE)
	{
		// target温度Check
		cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_SENSOR_TARGET_TEMP_ADRS, &adrs, &data);
		datai = (int)data;
		temp = (double)((double)datai / (double)CAMERA_SAVE_TEMP_UNIT);

		// 温度上下限値Check
		peltierGetTargetMinMax (&tempMin, &tempMax);

		// 温度下限値Check
		if (tempMin > temp)
			temp = tempMin;

		// 温度上限値Check
		if (tempMax < temp)
			temp = tempMax;
		
		// Target温度設定
		peltierSetTarget (temp);
	}
#endif // #if defined (MODE_PELTIER)

	// Peltier Enable
	//peltierEnable ();	///@@@@1
	peltierPowerInitialize();
#endif //@@@1

	// Interrupt Enable
    MIV_PLIC_enable_irq (&g_plic, MIV_PLIC_EXT3_IRQn);

_DONE:
	return (status);
}


//**********************************************************************************
//	ハンドラ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		-
//==================================================================================
void MIV_PLIC_EXT3_IRQHandler (void)
{
	unsigned int data32;
	unsigned int intStatus, intEnable;
	unsigned int tempStatus;

	// Get Interrupt Status
	intStatus = IN32 (FPGA_INT_STATUS_ADRS);
	
	// Get Interrupt Status
	fpgaGetInterrupt (&intEnable);

//@@@1
	DEBUG_PRINT_FORCE("@@@1 Temp Int = 0x%x / Enable = 0x%x\n", intStatus, intEnable);
//@@@1

	// Mask
	intStatus &= intEnable;
	
	// Clear Status
	OUT32 (FPGA_INT_STATUS_ADRS, intStatus);

	// FPGA上限値異常
	if (intStatus & FPGA_INT_TEMP_UPPER)
	{
		// Get Interrupt Enable
		fpgaGetInterrupt (&intEnable);

		intEnable &= ~FPGA_INT_TEMP_UPPER;	// 上限割り込み無効
		intEnable |= FPGA_INT_TEMP_LOWER;	// 下限割り込み有効
		
		// Set Interrupt Enable
		fpgaSetInterrupt (intEnable);

		// ステータス設定
		tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS);
		tempStatus |= FPGA_TEMP_STATUS;
		OUT32 (BOARD_STATUS_TEMP_ADRS, tempStatus);

		// 温度カウント設定
		gPeltierFpgaTempCount++;

		// HW System Error
		ledHwSystemErrorState ();
	}
	// FPGA下限値異常
	else if (intStatus & FPGA_INT_TEMP_LOWER)
	{
		// Get Interrupt Enable
		fpgaGetInterrupt (&intEnable);

		intEnable &= ~FPGA_INT_TEMP_LOWER;	// 下限割り込み無効
		intEnable |= FPGA_INT_TEMP_UPPER;	// 上限割り込み有効
		
		// Set Interrupt Enable
		fpgaSetInterrupt (intEnable);

		// ステータスクリア
		tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS);
		tempStatus &= ~FPGA_TEMP_STATUS;
		OUT32 (BOARD_STATUS_TEMP_ADRS, tempStatus);

		// HW System ErrorClear
		if (tempStatus == 0)
			ledConnectStateForce ();
	}

#if 0	//@@@1
	// Clear
	data = IN32 (FPGA_PELTIER_IRQ_STATUS_ADRS);
	data &= FPGA_PELTIER_STATUS_MASK;
	OUT32 (FPGA_PELTIER_IRQ_STATUS_ADRS, data);

	// 温度上限値異常
	if (data & FPGA_PELTIER_STATUS_TEMP_OVER_BIT)
	{
		// ステータス設定
		tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS);
		//@@@1OUT32 (BOARD_STATUS_TEMP_ADRS, (tempStatus | SENSOR_TEMP_OVER_STATUS));

		// センサ温度上限カウント
		gPeltierSensorUpperTempCount++;

		// センサ温度上限カウント
		OUT32 (BOARD_STATUS_SENSOR_TEMP_UPPER_ADRS, gPeltierSensorUpperTempCount);

		// HW System Error
		ledHwSystemErrorState ();
	}
	
	// 温度下限値異常
	if (data & FPGA_PELTIER_STATUS_TEMP_UNDER_BIT)
	{
		// ステータス設定
		tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS);
		//@@@1OUT32 (BOARD_STATUS_TEMP_ADRS, (tempStatus | SENSOR_TEMP_UNDER_STATUS));

		// センサ温度下限カウント
		gPeltierSensorLowerTempCount++;

		// センサ温度下限カウント
		OUT32 (BOARD_STATUS_SENSOR_TEMP_LOWER_ADRS, gPeltierSensorLowerTempCount);

		// HW System Error
		ledHwSystemErrorState ();
	}

	// Case温度異常
	if (data & FPGA_PELTIER_STATUS_CASE_FAIL_BIT)
	{
		// ステータス設定
		tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS);
		OUT32 (BOARD_STATUS_TEMP_ADRS, (tempStatus | CASE_TEMP_STATUS));

		// ケース温度カウント
		gPeltierCaseTempCount++;

		// ケース温度カウント
		OUT32 (BOARD_STATUS_CASE_TEMP_ADRS, gPeltierCaseTempCount);

		// HW System Error
		ledHwSystemErrorState ();
	}
#endif //@@@1
	
	return;
}


//**********************************************************************************
// Peltier実装状態設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		state				：0=ペルチェなし/1=ペルチェあり
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierSetMountState (int state)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check state Parameter
	if ((state != MODE_DISABLE) && (state != MODE_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Set Mount State(%d) Parameter Error.(Disable:%d / Enable:%d)\n", state, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Peltier実装状態設定
	OUT32 (FIRM_DATA_PELTIER_MOUNT_STATE_ADRS, state);

_DONE:
	return (status);
}


//**********************************************************************************
// Peltier実装状態取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pState				：ペルチェ実装状態を格納するポインタ(0=ペルチェなし/1=ペルチェあり)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetMountState (int *pState)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pState Parameter
	if (pState == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Get Mount State NULL Parameter Error.\n");
		goto _DONE;
	}

	// Peltier実装状態取得
	*pState = IN32 (FIRM_DATA_PELTIER_MOUNT_STATE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// Check Mount
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierCheckMountState (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int state;

	// Peltier実装状態取得
	if ((status = peltierGetMountState (&state)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check実装状態
	if (state == MODE_DISABLE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Not Mount.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Set Enable/Disable
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Peltier Mode(0:無効/1：無効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierSetEnable (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check data Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Enable mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if (mode == MODE_ENABLE)
	{
		if ((status = peltierEnable ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		if ((status = peltierDisable ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Set Enable/Disable
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Peltier Mode(0:無効/1：無効)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetEnable (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Get Enable pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Peltier Enable Mode取得
	data = IN32 (FPGA_PELTIER_CTRL_ADRS);

	if (data & FPGA_PELTIER_CTRL_FIXED_BIT)
		*pMode = MODE_DISABLE;
	else
		*pMode = MODE_ENABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Enable
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierEnable (void)
{
	int status = AVAL_STATUS_SUCCESS;
	//unsigned int data;
#ifdef MODE_PELTIER
	unsigned int mode;
	int peltierState = MODE_ENABLE;

	#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	// Peltier実装状態確認
	if ((status = peltierGetMountState (&peltierState)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	if(peltierState == MODE_ENABLE)
	{
		//------------------------------------------------------------
		// Peltier Enable
		//------------------------------------------------------------
		// Peltier Enable取得
		mode = IN32 (FPGA_PELTIER_CTRL_ADRS);

		// Clear
		OUT32 (FPGA_PELTIER_CTRL_ADRS, 0);

		// Peltier Enable
		mode &= ~FPGA_PELTIER_CTRL_FIXED_BIT;
		mode |= FPGA_PELTIER_CTRL_ENABLE_BIT;
		OUT32 (FPGA_PELTIER_CTRL_ADRS, mode);
	}
#endif

	//------------------------------------------------------------
	// Interrupt Enable
	//------------------------------------------------------------

	// Peltier Interrupt Enable
	peltierSetIntEnable ((FPGA_PELTIER_IRQ_MASK_CASE_FAIL_BIT | FPGA_PELTIER_IRQ_MASK_TEMP_UNDER_BIT | FPGA_PELTIER_IRQ_MASK_TEMP_OVER_BIT));

	// グローバル割り込み有効
	if ((status = peltierSetGlobalInt (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Enable2
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierEnable2 (void)
{
	int status = AVAL_STATUS_SUCCESS;
#ifdef MODE_PELTIER
	unsigned int mode;
	int peltierState = MODE_ENABLE;

	#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	// Peltier実装状態確認
	if ((status = peltierGetMountState (&peltierState)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	if(peltierState == MODE_ENABLE)
	{
		//------------------------------------------------------------
		// Peltier Enable
		//------------------------------------------------------------
		// Peltier Enable取得
		mode = IN32 (FPGA_PELTIER_CTRL_ADRS);

		// Clear
		OUT32 (FPGA_PELTIER_CTRL_ADRS, 0);

		// Peltier Enable
		mode &= ~FPGA_PELTIER_CTRL_FIXED_BIT;
		//mode |= FPGA_PELTIER_CTRL_ENABLE_BIT;
		OUT32 (FPGA_PELTIER_CTRL_ADRS, mode);
	}
#endif

	//------------------------------------------------------------
	// Interrupt Enable
	//------------------------------------------------------------

	// Peltier Interrupt Enable
	peltierSetIntEnable ((FPGA_PELTIER_IRQ_MASK_CASE_FAIL_BIT | FPGA_PELTIER_IRQ_MASK_TEMP_UNDER_BIT | FPGA_PELTIER_IRQ_MASK_TEMP_OVER_BIT));

	// グローバル割り込み有効
	if ((status = peltierSetGlobalInt (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Disable
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierDisable (void)
{
	int status = AVAL_STATUS_SUCCESS;
#ifdef MODE_PELTIER
	unsigned int mode;
#endif

#ifdef MODE_PELTIER
	// Peltier Disable
	mode = IN32 (FPGA_PELTIER_CTRL_ADRS);
	//mode |= (FPGA_PELTIER_CTRL_FIXED_BIT | FPGA_PELTIER_CTRL_ENABLE_BIT);
	mode |= FPGA_PELTIER_CTRL_FIXED_BIT;
	mode &= ~FPGA_PELTIER_CTRL_ENABLE_BIT;
	OUT32 (FPGA_PELTIER_CTRL_ADRS, mode);
#endif

	// 個別割り込み無効
	if ((status = peltierSetIntEnable (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// グローバル割り込み無効
	if ((status = peltierSetGlobalInt (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


#if defined (MODE_PELTIER)
//**********************************************************************************
//	Peltier Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0:Disable/1:Enable
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierSetMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	// Peltier実装状態確認
	if ((status = peltierCheckMountState ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// Check data Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Set Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Peltier Enable取得
	data = IN32 (FPGA_PELTIER_CTRL_ADRS);

	// クリア
	data &= ~FPGA_PELTIER_CTRL_ENABLE_BIT;

	if (mode == MODE_ENABLE)
		data |= FPGA_PELTIER_CTRL_ENABLE_BIT;

	// Peltier Enable設定
	OUT32 (FPGA_PELTIER_CTRL_ADRS, data);

	// Peltier Enable設定(DDR)
	OUT32(FIRM_DATA_SENSOR_PERTIER_MODE, mode);

_DONE:
	return (status);
}
#endif


#if defined (MODE_PELTIER)
//**********************************************************************************
//	Peltier Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Peltier Enable状態を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Get pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	*pMode = IN32 (FPGA_PELTIER_CTRL_ADRS) & FPGA_PELTIER_CTRL_ENABLE_BIT;

_DONE:
	return (status);
}
#endif


#if defined (MODE_PELTIER)
//**********************************************************************************
//	Peltier Power Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierPowerInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
#if defined (MODE_TEMP_ABNORMAL_CHECK)
	double fpgaTemp, fpgaTempMax;
	unsigned int count, powerSupply;
#endif

	// Enable
	peltierEnable2 ();

	// ペルチェ有効／無効取得
	data32 = IN32(FIRM_DATA_SENSOR_PERTIER_MODE);

#if defined (MODE_TEMP_ABNORMAL_CHECK)
	// FPGA温度
	if ((status = fpgaGetTemp (&fpgaTemp)) != AVAL_STATUS_SUCCESS)
		fpgaTemp = 0;

	// Get Power Supply
	if ((status = cameraPowerSupply (&powerSupply)) != AVAL_STATUS_SUCCESS)
		fpgaTemp = 0;
	
	// 電源供給もとにより、FPGA温度MAX値が異なる
	if (powerSupply & FPGA_POWER_STATUS_POWER_CONNECTOR)
		fpgaTempMax = TEMP_ABNORMAL_FPGA_UPPER_EXT;
	else
		fpgaTempMax = TEMP_ABNORMAL_FPGA_UPPER_PO;

	// FPGA上限温度異常？
	if (fpgaTemp >= fpgaTempMax)
	{
		// LED
		setLed (LED_NUM1, LED_PATTERN2);
		
		// Temp Abnormal Status設定
		tempSetAbnormalStatus (1);
		
		// Temp Abnormal Count設定
		tempGetAbnormalCount (&count);
		tempSetAbnormalCount (count+1);
		
		data32 = 0;
	}
#endif

	// ペルチェ無効
	if (data32 == 0)
	{
		peltierSetMode (0);
	}
	else
	{
		#if defined (MODE_PELTIER_CTRL)
		peltierControlProcsInit ();
		#endif

		peltierSetMode (1);

		#if defined (MODE_PELTIER_CTRL)
		// Peltier Enable Delay
		peltierControlProcs ();
		#endif
	}

	return (status);
}
#endif


//**********************************************************************************
//	Peltier Interrupt Enable設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：個別割り込み設定
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierSetIntEnable (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// 個別割り込み設定
	OUT32 (FPGA_PELTIER_IRQ_MASK_ADRS, (data & FPGA_PELTIER_IRQ_MASK));

	return (status);
}


//**********************************************************************************
//	Peltier Interrupt Enable取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：個別割り込みを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetIntEnable (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Get Interrupt Enable pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// 個別割り込み取得
	*pData = IN32 (FPGA_PELTIER_IRQ_MASK_ADRS) & FPGA_PELTIER_IRQ_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Global Interrupt設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Global割り込み設定
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierSetGlobalInt (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check data Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Global Interrupt mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// グローバル割り込み取得
	data = IN32 (FPGA_PELTIER_CTRL_ADRS);

	if (mode == MODE_ENABLE)
		data |= FPGA_PELTIER_CTRL_IRQ_BIT;
	else
		data &= ~FPGA_PELTIER_CTRL_IRQ_BIT;

	// グローバル割り込み設定
	OUT32 (FPGA_PELTIER_CTRL_ADRS, data);

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Global Interrupt取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Global割り込みを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetGlobalInt (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Get Global Interrupt pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// グローバル割り込み取得
	data = IN32 (FPGA_PELTIER_CTRL_ADRS);

	if (data & FPGA_PELTIER_CTRL_IRQ_BIT)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Target温度設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		temp					：Target温度
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierSetTarget (double temp)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, reg32, dec32, temp32;
	double dataD;
	double tempMin, tempMax;

#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	// Peltier実装状態確認
	if ((status = peltierCheckMountState ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// Peltier Temp Min/Max取得
	if ((status = peltierGetTargetMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check temp Parameter
	if ((temp < tempMin) || (temp > tempMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Target temp(%.2f) Parameter Error.(Min:%.2f / Max:%.2f)\n", temp, tempMin, tempMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 小数点計算
	if (temp > 0)
		dataD = temp - (int)temp;	// 小数点のみ取得
	else
		dataD = (int)temp - temp;	// 小数点のみ取得

	reg32 = dataD * (1 << FPGA_PELTIER_VERSION2_TARGET_REAL_SHIFT);
	dec32 = reg32 & FPGA_PELTIER_VERSION2_TARGET_DECIMAL_MASK;

	// 負数?
	if (temp < 0)
	{
		// 整数計算
		temp32 = (unsigned int)abs(temp);
		temp32--;
		data32 = ~temp32;

		reg32 = data32 << FPGA_PELTIER_VERSION2_TARGET_REAL_SHIFT;
	}
	else
	{
		// 整数計算
		data32 = (unsigned int)temp;
		reg32 = (data32 & FPGA_PELTIER_VERSION2_TARGET_REAL_MASK) << FPGA_PELTIER_VERSION2_TARGET_REAL_SHIFT;
	}

	// 小数点部追加
	reg32 |= dec32;

	// レジスタ設定
	OUT32 (FPGA_PELTIER_TARGET_ADRS, (reg32 & FPGA_PELTIER_VERSION2_TARGET_REG_MASK));

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Target温度取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTemp					：Target温度を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierGetTarget (double *pTemp)
{
	int status = AVAL_STATUS_SUCCESS;
	int dataS32;
	unsigned int data32, reg32;
	double dataD;

#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	// Peltier実装状態確認
	if ((status = peltierCheckMountState ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// Check pTemp Parameter
	if (pTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Target pTemp NULL Parameter Error.\n");
		goto _DONE;
	}

	// Peltier Target温度取得
	reg32 = IN32 (FPGA_PELTIER_TARGET_ADRS) & FPGA_PELTIER_VERSION2_TARGET_REG_MASK;

	// 小数点計算
	data32 = reg32 & FPGA_PELTIER_VERSION2_TARGET_DECIMAL_MASK;
	dataD = (double)((double)data32 / (double)(1<<FPGA_PELTIER_VERSION2_TARGET_REAL_SHIFT));

	// 整数計算
	data32 = (reg32 >> FPGA_PELTIER_VERSION2_TARGET_REAL_SHIFT) & FPGA_PELTIER_VERSION2_TARGET_REAL_MASK ;

	// 負数?
	if (reg32 & FPGA_PELTIER_VERSION2_TARGET_CODE)
	{
		dataS32 = (int)(FPGA_PELTIER_VERSION2_TARGET_SIGNED_MASK | data32);

		// 小数点加算
		dataD =  (double)((double)dataS32 - dataD);
	}
	else
	{
		dataS32 = data32;

		// 小数点加算
		dataD =   (double)(double)(dataS32 + dataD);
	}

	// 最小値調整
	if (dataD < PELTIER_TARGET_MIN_TEMP)
		dataD = PELTIER_TARGET_MIN_TEMP;

	// 最大値調整
	if (dataD > PELTIER_TARGET_MAX_TEMP)
		dataD = PELTIER_TARGET_MAX_TEMP;

	// 温度設定
	*pTemp = dataD;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Target温度Min/Max取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMinTemp				：Target温度最小値を格納するポインタ
//		pMaxTemp				：Target温度最大値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierGetTargetMinMax (double *pMinTemp, double *pMaxTemp)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMinTemp Parameter
	if (pMinTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Target pMinTemp NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pMaxTemp Parameter
	if (pMaxTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Target pMaxTemp NULL Parameter Error.\n");
		goto _DONE;
	}

	*pMinTemp = PELTIER_TARGET_MIN_TEMP;
	*pMaxTemp = PELTIER_TARGET_MAX_TEMP;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Sensor通知温度設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		tempOver				：センサ通知温度の上限値
//		tempUnder				：センサ通知温度の下限値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierSetSensorTempAlarm (double tempOver, double tempUnder)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, data32H, data32L;
	double tempMin, tempMax;
	unsigned int tempStatus;
	unsigned int dataOver, dataUnder;

	// Peltier Temp Min/Max取得
	if ((status = peltierGetSensorTempAlarmMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check tempOver Parameter
	if ((tempOver < tempMin) || (tempOver > tempMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Sensor Alarm tempOver(%.2f) Parameter Error.(Min:%.2f / Max:%.2f)\n", tempOver, tempMin, tempMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check tempUnder Parameter
	if ((tempUnder < tempMin) || (tempUnder > tempMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Sensor Alarm tempUnder(%.2f) Parameter Error.(Min:%.2f / Max:%.2f)\n", tempUnder, tempMin, tempMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check tempOver/tempUnder Parameter
	if (tempOver < tempUnder)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Sensor Alarm tempOver(%.2f) < tempUnder(%.2f) Parameter Error.\n", tempOver, tempUnder);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// センサ温度ステータスクリア
	tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS);

	//@@@1if (tempStatus & (SENSOR_TEMP_OVER_STATUS | SENSOR_TEMP_UNDER_STATUS))
	{
		// グローバル割り込み無効
		if ((status = peltierSetGlobalInt (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// ステータスクリア
		//@@@1tempStatus &= ~(SENSOR_TEMP_OVER_STATUS | SENSOR_TEMP_UNDER_STATUS);
		OUT32 (BOARD_STATUS_TEMP_ADRS, tempStatus);

		// Check Status
		if (tempStatus == 0)
			ledConnectStateForce();

		// グローバル割り込み有効
		if ((status = peltierSetGlobalInt (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// 上限値計算
	if ((status = peltierSetSensorTempAlarmCalc (tempOver, &dataOver)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 下限値計算
	if ((status = peltierSetSensorTempAlarmCalc (tempUnder, &dataUnder)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// レジスタ設定
	data32H = (dataOver & FPGA_PELTIER_SENSOR_ALARM_OVER_REG_MASK) << FPGA_PELTIER_SENSOR_ALARM_OVER_REG_SHIFT;
	data32L = (dataUnder & FPGA_PELTIER_SENSOR_ALARM_UNDER_REG_MASK) << FPGA_PELTIER_SENSOR_ALARM_UNDER_REG_SHIFT;
	data32 = data32H | data32L;
	OUT32 (FPGA_PELTIER_SENSOR_ALM_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Sensor取得計算
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		alarm					：センサ通知温度
//		pData					：センサ通知温度レジスタ値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierSetSensorTempAlarmCalc (double alarm, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, reg32, save32, temp32;
	double dataD ;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Sensor Alarm pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// 小数点計算
	if (alarm > 0)
		dataD = alarm - (int)alarm;	// 小数点のみ取得
	else
		dataD = (int)alarm - alarm;	// 小数点のみ取得

	reg32 = dataD * (1 << FPGA_PELTIER_VERSION2_SENSOR_ALARM_REAL_SHIFT);
	save32 = reg32 & FPGA_PELTIER_VERSION2_SENSOR_ALARM_DECIMAL_MASK;

	// 負数?
	if (alarm < 0)
	{
		// 整数計算
		temp32 = (unsigned int)abs(alarm);
		temp32--;
		data32 = ~temp32;

		reg32 = data32 << FPGA_PELTIER_VERSION2_SENSOR_ALARM_REAL_SHIFT;
	}
	else
	{
		// 整数計算
		data32 = (unsigned int)alarm;
		reg32 = (data32 & FPGA_PELTIER_VERSION2_SENSOR_ALARM_MASK) << FPGA_PELTIER_VERSION2_SENSOR_ALARM_REAL_SHIFT;
	}

	// 小数点部追加
	reg32 |= save32;

	// 格納
	*pData = reg32;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Sensor通知温度取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTempOver				：センサ通知温度の上限値を格納するポインタ
//		pTempUnder				：センサ通知温度の下限値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierGetSensorTempAlarm (double *pTempOver, double *pTempUnder)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int reg32, data32;

//@@@1
*pTempOver = 59;
*pTempUnder = 20;
goto _DONE;
//@@@1

	// Check pTempOver Parameter
	if (pTempOver == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Sensor Alarm pTempOver NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pTempUnder Parameter
	if (pTempUnder == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Sensor Alarm pTempUnder NULL Parameter Error.\n");
		goto _DONE;
	}

	// データ取得
	reg32 = IN32 (FPGA_PELTIER_SENSOR_ALM_ADRS);

	// 上限値取得
	data32 = reg32 >> FPGA_PELTIER_SENSOR_ALARM_OVER_REG_SHIFT;
	data32 &= FPGA_PELTIER_SENSOR_ALARM_OVER_REG_MASK;

	// 上限値計算
	if ((status = peltierGetSensorTempAlarmCalc (data32, pTempOver)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 下限値取得
	data32 = reg32 >> FPGA_PELTIER_SENSOR_ALARM_UNDER_REG_SHIFT;
	data32 &= FPGA_PELTIER_SENSOR_ALARM_UNDER_REG_MASK;

	// 下限値計算
	if ((status = peltierGetSensorTempAlarmCalc (data32, pTempUnder)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Sensor取得計算
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		reg						：センサ通知温度レジスタ値
//		pTempAlarm				：センサ通知温度を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierGetSensorTempAlarmCalc (unsigned int reg, double *pTempAlarm)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	double tempMin, tempMax;
	double dataD;
	int dataS32;

	// Check pTempAlarm Parameter
	if (pTempAlarm == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Sensor Alarm pTempAlarm NULL Parameter Error.\n");
		goto _DONE;
	}

	// Peltier Temp Min/Max取得
	if ((status = peltierGetSensorTempAlarmMinMax (&tempMin, &tempMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 小数点計算
	data32 = reg & FPGA_PELTIER_VERSION2_SENSOR_ALARM_DECIMAL_MASK;
	dataD = (double)((double)data32 / (double)(1<<FPGA_PELTIER_VERSION2_SENSOR_ALARM_REAL_SHIFT));

	// 整数計算
	data32 = (reg >> FPGA_PELTIER_VERSION2_SENSOR_ALARM_REAL_SHIFT) & FPGA_PELTIER_VERSION2_SENSOR_ALARM_MASK;

	// 負数?
	if (reg & FPGA_PELTIER_VERSION2_SENSOR_ALARM_CODE)
	{
		dataS32 = (int)(FPGA_PELTIER_VERSION2_SENSOR_ALARM_SIGNED_MASK | data32);

		// 小数点加算
		dataD =  (double)((double)dataS32 - dataD);
	}
	else
	{
		// 小数点加算
		dataD =  (double)((double)data32 + dataD);
	}

	// 上下限値Check
	if (dataD > tempMax)
		dataD = tempMax;

	if (dataD < tempMin)
		dataD = tempMin;

	// 格納
	*pTempAlarm = dataD;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Sensor 温度Min/Max取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMinTemp				：センサ温度最小値を格納するポインタ
//		pMaxTemp				：センサ温度最大値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierGetSensorTempAlarmMinMax (double *pMinTemp, double *pMaxTemp)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	int peltierState = MODE_ENABLE;
#endif

	// Check pMinTemp Parameter
	if (pMinTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Target pMinTemp NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pMaxTemp Parameter
	if (pMaxTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Target pMaxTemp NULL Parameter Error.\n");
		goto _DONE;
	}

	*pMinTemp = PELTIER_SENSOR_ALM_MIN_TEMP;
	*pMaxTemp = PELTIER_SENSOR_ALM_MAX_TEMP;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Case通知温度設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		tempOver				：Case通知温度の上限値
//		tempUnder				：Case通知温度の下限値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierSetCaseTempAlarm (double tempOver, double tempUnder)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	unsigned int over, under;
	unsigned int tempStatus;

	// Check tempOver Parameter
	if ((tempOver < PELTIER_CASE_ALM_MIN_TEMP) || (tempOver > PELTIER_CASE_ALM_MAX_TEMP))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Case Alarm tempOver(%.2f) Parameter Error.(Min:%.2f / Max:%.2f)\n", tempOver, PELTIER_CASE_ALM_MIN_TEMP, PELTIER_CASE_ALM_MAX_TEMP);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check tempUnder Parameter
	if ((tempUnder < PELTIER_CASE_ALM_MIN_TEMP) || (tempUnder > PELTIER_CASE_ALM_MAX_TEMP))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Case Alarm tempUnder(%.2f) Parameter Error.(Min:%.2f / Max:%.2f)\n", tempUnder, PELTIER_CASE_ALM_MIN_TEMP, PELTIER_CASE_ALM_MAX_TEMP);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check tempOver/tempUnder Parameter
	if (tempOver < tempUnder)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Case Alarm tempOver(%.2f) < tempUnder(%.2f) Parameter Error.\n", tempOver, tempUnder);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//------------------------------------------------------------
	// 1度下限をクリアする
	//------------------------------------------------------------

	// Over取得
	if ((status = peltierSetCaseTempAlarmCalc (PELTIER_CASE_ALM_MAX_TEMP, &over)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Under取得
	if ((status = peltierSetCaseTempAlarmCalc (PELTIER_CASE_ALM_MAX_TEMP - 0.5, &under)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データ設定
	data = FPGA_PELTIER_CASE_ALM_DATA_MAKE (over,under);
	OUT32 (FPGA_PELTIER_CASE_ALM_ADRS, data);

	// Case温度ステータスクリア(Ver.1.5)
	tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS);
	if (tempStatus & CASE_TEMP_STATUS)
	//if (gPeltierTempStatus & CASE_TEMP_STATUS)
	{
		// グローバル割り込み無効
		if ((status = peltierSetGlobalInt (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Case温度ステータスクリア
		tempStatus &= ~CASE_TEMP_STATUS;
		OUT32 (BOARD_STATUS_TEMP_ADRS, tempStatus);

		// Check Status
		if (tempStatus == 0)
			ledConnectStateForce();

		// グローバル割り込み有効
		if ((status = peltierSetGlobalInt (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Over取得
	if ((status = peltierSetCaseTempAlarmCalc (tempOver, &over)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Under取得
	if ((status = peltierSetCaseTempAlarmCalc (tempUnder, &under)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データ設定
	data = FPGA_PELTIER_CASE_ALM_DATA_MAKE (over,under);
	OUT32 (FPGA_PELTIER_CASE_ALM_ADRS, data);

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Case取得計算
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		alarm					：センサ通知温度
//		pData					：センサ通知温度レジスタ値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierSetCaseTempAlarmCalc (double alarm, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, reg32, save32, temp32;
	double dataD ;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Sensor Alarm pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// 小数点計算
	if (alarm > 0)
		dataD = alarm - (int)alarm;	// 小数点のみ取得
	else
		dataD = (int)alarm - alarm;	// 小数点のみ取得

	reg32 = dataD * (1 << FPGA_PELTIER_VERSION2_CASE_ALARM_REAL_SHIFT);
	save32 = reg32 & FPGA_PELTIER_VERSION2_CASE_ALARM_DECIMAL_MASK;

	// 負数?
	if (alarm < 0)
	{
		// 整数計算
		temp32 = (unsigned int)abs(alarm);
		temp32--;
		data32 = ~temp32;

		// 整数計算
		reg32 = data32 << FPGA_PELTIER_VERSION2_CASE_ALARM_REAL_SHIFT;
	}
	else
	{
		// 整数計算
		data32 = (unsigned int)alarm;
		reg32 = (data32 & FPGA_PELTIER_VERSION2_CASE_ALARM_MASK) << FPGA_PELTIER_VERSION2_CASE_ALARM_REAL_SHIFT;
	}

	// 小数点部追加
	reg32 |= save32;

	// 格納
	*pData = reg32;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Case通知温度取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTempOver				：Case通知温度の上限値を格納するポインタ
//		pTempUnder				：Case通知温度の下限値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierGetCaseTempAlarm (double *pTempOver, double *pTempUnder)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, reg32;

	// Check pTempOver Parameter
	if (pTempOver == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Case Alarm pTempOver NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pTempUnder Parameter
	if (pTempUnder == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Case Alarm pTempUnder NULL Parameter Error.\n");
		goto _DONE;
	}

	// データ取得
	reg32 = IN32 (FPGA_PELTIER_CASE_ALM_ADRS);

	// 上限値取得
	data32 = reg32 >> FPGA_PELTIER_CASE_ALARM_OVER_REG_SHIFT;
	data32 &= FPGA_PELTIER_CASE_ALARM_OVER_REG_MASK;

	// 上限値計算
	if ((status = peltierGetCaseTempAlarmCalc (data32, pTempOver)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 下限値取得
	data32 = reg32 >> FPGA_PELTIER_CASE_ALARM_UNDER_REG_SHIFT;
	data32 &= FPGA_PELTIER_CASE_ALARM_UNDER_REG_MASK;

	// 下限値計算
	if ((status = peltierGetCaseTempAlarmCalc (data32, pTempUnder)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Case取得計算
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		reg					：センサ通知温度レジスタ値
//		pTempAlarm				：センサ通知温度を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierGetCaseTempAlarmCalc (unsigned int reg, double *pTempAlarm)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	double dataD;
	int dataS32;

	// Check pTempAlarm Parameter
	if (pTempAlarm == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Case Alarm pTempAlarm NULL Parameter Error.\n");
		goto _DONE;
	}

	// 小数点計算
	data32 = reg & FPGA_PELTIER_VERSION2_CASE_ALARM_DECIMAL_MASK;
	dataD = (double)((double)data32 / (double)(1<<FPGA_PELTIER_VERSION2_CASE_ALARM_REAL_SHIFT));

	// 整数計算
	data32 = (reg >> FPGA_PELTIER_VERSION2_CASE_ALARM_REAL_SHIFT) & FPGA_PELTIER_VERSION2_CASE_ALARM_MASK;

	// 負数?
	if (reg & FPGA_PELTIER_VERSION2_CASE_ALARM_CODE)
	{
		dataS32 = (int)(FPGA_PELTIER_VERSION2_CASE_ALARM_SIGNED_MASK | data32);

		// 小数点加算
		dataD = (double)((double)dataS32 - dataD);
	}
	else
	{
		// 小数点加算
		dataD = (double)(double)(data32 + dataD);
	}

	// 上下限値Check
	if (dataD > PELTIER_CASE_ALM_MAX_TEMP)
		dataD = PELTIER_CASE_ALM_MAX_TEMP;

	if (dataD < PELTIER_CASE_ALM_MIN_TEMP)
		dataD = PELTIER_CASE_ALM_MIN_TEMP;

	// 格納
	*pTempAlarm = dataD;

_DONE:
	return (status);
}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
//	Peltier Sensor温度取得(Ver.1.3)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTemp					：センサ温度取得
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierGetSensorTemp (double *pTemp)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	short dataS16;

//@@@1
	*pTemp = 0;
	return (0);
//@@@1
	
	// Check pTemp Parameter
	if (pTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Sensor Temp pTemp NULL Parameter Error.\n");
		goto _DONE;
	}

	// データ取得
	data = IN32 (FPGA_SENSOR_TEMP_ADRS) & FPGA_SENSOR_TEMP_MASK;

	// 負数
	if (data & FPGA_SENSOR_TEMP_CODE_BIT)
	{
		data |= 0xfffff000;
		dataS16 = (short)data;
		*pTemp = dataS16 * FPGA_SENSOR_TEMP_LSB;
	}
	else
	{
		*pTemp = data * FPGA_SENSOR_TEMP_LSB;
	}

_DONE:
	if (status != AVAL_STATUS_SUCCESS)
		peltierErrorStatus (status);

	return (status);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
//	Peltier Case温度取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTemp					：Case温度取得
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierGetCaseTemp (double *pTemp)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, reg32;
	int dataS32;
	double dataD;
//@@@1
	*pTemp = 0;
	return (0);
//@@@1
	

	// Check pTemp Parameter
	if (pTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Case Temp pTemp NULL Parameter Error.\n");
		goto _DONE;
	}

	// データ取得
	reg32 = IN32 (FPGA_PELTIER_CASE_TEMP_ADRS) & FPGA_PELTIER_VERSION2_CASE_TEMP_MASK;

	// 小数点計算
	data32 = reg32 & FPGA_PELTIER_VERSION2_CASE_TEMP_DECIMAL_MASK;
	dataD = (double)((double)data32 / (double)(1<<FPGA_PELTIER_VERSION2_CASE_TEMP_REAL_SHIFT));

	// 整数計算
	data32 = (reg32 >> FPGA_PELTIER_VERSION2_CASE_TEMP_REAL_SHIFT) & FPGA_PELTIER_VERSION2_CASE_TEMP_REAL_MASK ;

	// 負数?
	if (reg32 & FPGA_PELTIER_VERSION2_CASE_TEMP_CODE)
	{
		dataS32 = (int)(FPGA_PELTIER_VERSION2_CASE_TEMP_SIGNED_MASK | data32);

		// 小数点加算
		dataD =  (double)((double)dataS32 - dataD);
	}
	else
	{
		// 小数点加算
		dataD =   (double)(double)(data32 + dataD);
	}

	// 温度設定
	*pTemp = dataD;

	if (status != AVAL_STATUS_SUCCESS)
		peltierErrorStatus (status);

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier 温度ステータス取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStatus					：温度ステータスを格納しするポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierGetTempAlarmStatus (unsigned int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int temp;
	unsigned int tempStatus;

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Temp Alarm Status pStatus NULL Parameter Error.\n");
		goto _DONE;
	}

	// グローバル割り込み無効
	if ((status = peltierSetGlobalInt (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Peltier Status
	if ((status = peltierGetMakeTempAlarmStatus (&temp)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 温度ステータス再設定
	tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS) | temp;
	OUT32 (BOARD_STATUS_TEMP_ADRS, tempStatus);

	// グローバル割り込み有効
	if ((status = peltierSetGlobalInt (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 温度ステータス取得
	*pStatus = tempStatus;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier 温度ステータスクリア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		clear					：温度ステータスをクリア
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int peltierClearTempAlarm (unsigned int clear)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int temp;
	unsigned int tempStatus;

	// Check clear Parameter
	if (clear > PELTIER_TEMP_STATUS_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Temp Alarm Status(0x%x) Parameter Error.(Max = 0x%x)\n", clear, PELTIER_TEMP_STATUS_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// グローバル割り込み無効
	if ((status = peltierSetGlobalInt (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 温度ステータス取得
	tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS);
	tempStatus &= ~clear;
	OUT32 (BOARD_STATUS_TEMP_ADRS, tempStatus);

	// Check Peltier Status
	if ((status = peltierGetMakeTempAlarmStatus (&temp)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 温度ステータス再設定
	tempStatus |= temp;
	OUT32 (BOARD_STATUS_TEMP_ADRS, tempStatus);

	// Check Status
	if (tempStatus == 0)
		ledConnectStateForce();

	// グローバル割り込み有効
	if ((status = peltierSetGlobalInt (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Get Alarm Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStatus				：ステータス情報を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetMakeTempAlarmStatus (unsigned int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data, temp;

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Alarm Status pStatus NULL Parameter Error.\n");
		goto _DONE;
	}

	// FPGA温度ステータス取得
	data = IN32 (FPGA_PELTIER_STATUS_ADRS);

	// 初期化
	temp = 0;

	// Sensor Over?
	//@@@1if (data & FPGA_PELTIER_STATUS_TEMP_OVER_BIT)
		//@@@1temp |= SENSOR_TEMP_OVER_STATUS;

	// Sensor Under?
	//@@@1if (data & FPGA_PELTIER_STATUS_TEMP_UNDER_BIT)
		//@@@1temp |= SENSOR_TEMP_UNDER_STATUS;

	// Case Over?
	if (data & FPGA_PELTIER_STATUS_CASE_FAIL_BIT)
		temp |= CASE_TEMP_STATUS;

	// ステータス取得
	*pStatus = temp;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Error Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		errNo				：エラー番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierErrorStatus (int errNo)
{
	// Peltier
	OUT32 (BOARD_STATUS_PELTIER_ADRS, errNo);

	// HW System Error
	ledHwSystemErrorState();

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Peltier Get Clip
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pLow				：Low Clipを格納するポインタ
//		pHigh				：High Clipを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetClip (unsigned int *pLow, unsigned int *pHigh)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pLow Parameter
	if (pLow == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Get Low Clip Parameter Error.\n");
		goto _DONE;
	}

	// Check pHigh Parameter
	if (pHigh == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Get High Clip Parameter Error.\n");
		goto _DONE;
	}

	// データ取得
	data32 = IN32 (FPGA_PELTIER_CLIP_ADRS);
	*pLow = (data32 >> FPGA_PELTIER_CLIP_LOW_SHIFT) & FPGA_PELTIER_CLIP_MASK;
	*pHigh = (data32 >> FPGA_PELTIER_CLIP_HIGH_SHIFT) & FPGA_PELTIER_CLIP_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Set Clip
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		low					：Low Clip
//		high				：High Clip
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierSetClip (unsigned int low, unsigned int high)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check low Parameter
	if (low > FPGA_PELTIER_CLIP_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Set Clip Low(%d) Parameter Error.(Min:0 / Max:%d)\n", low, FPGA_PELTIER_CLIP_MASK);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check high Parameter
	if (high > FPGA_PELTIER_CLIP_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Set Clip Low(%d) Parameter Error.(Min:0 / Max:%d)\n", low, FPGA_PELTIER_CLIP_MASK);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	// データ設定
	data32 = low << FPGA_PELTIER_CLIP_LOW_SHIFT;
	data32 |= (high << FPGA_PELTIER_CLIP_HIGH_SHIFT);
	OUT32 (FPGA_PELTIER_CLIP_ADRS, data32);

_DONE:
	return (status);
}


#if defined (MODE_PELTIER_VOLT_CURRENT)
//**********************************************************************************
//	Peltier電圧取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pVolt				：電圧を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetVolt (double *pVolt)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pVolt Parameter
	if (pVolt == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Get Volt pVolt NULL Parameter Error.\n");
		goto _DONE;
	}

	// 電圧値取得
	data32 = IN32 (FPGA_PELTIER_TEC_VOLT_CURRENT_ADRS);
	data32 >>= FPGA_PELTIER_TEC_VOLT_SHIFT;
	data32 &= FPGA_PELTIER_TEC_VOLT_MASK;
	*pVolt = (double)((double)data32 * FPGA_PELTIER_TEC_VOLT_UNIT);

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier電流取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pVolt				：電流を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetCurrent (double *pCurrent)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pCurrent Parameter
	if (pCurrent == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Get Current pCurrent NULL Parameter Error.\n");
		goto _DONE;
	}

	// 電流値取得
	data32 = IN32 (FPGA_PELTIER_TEC_VOLT_CURRENT_ADRS);
	data32 >>= FPGA_PELTIER_TEC_CURRENT_SHIFT;
	data32 &= FPGA_PELTIER_TEC_CURRENT_MASK;
	*pCurrent = (double)((double)data32 * FPGA_PELTIER_TEC_CURRENT_UNIT);

_DONE:
	return (status);
}
#endif // #if defined (MODE_PELTIER_VOLT_CURRENT)


#if defined (MODE_TEMP_ABNORMAL_CHECK)
//**********************************************************************************
//	Temp Abnormal Status設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：ステータス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tempSetAbnormalStatus (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// 設定
	OUT32 (FIRM_DATA_TEMP_ABNORMAL_STATUS, data);

	return (status);
}


//**********************************************************************************
//	Temp Abnormal Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：ステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tempGetAbnormalStatus (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Temp Abnormal Status NULL Parameter Error.\n");
		goto _DONE;
	}

	// 取得
	*pData = IN32 (FIRM_DATA_TEMP_ABNORMAL_STATUS);

_DONE:
	return (status);
}


//**********************************************************************************
//	Temp Abnormal Count設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：カウント
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tempSetAbnormalCount (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// 設定
	OUT32 (FIRM_DATA_TEMP_ABNORMAL_COUNT, data);

	return (status);
}


//**********************************************************************************
//	Temp Abnormal Count取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：カウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tempGetAbnormalCount (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Temp Abnormal Count NULL Parameter Error.\n");
		goto _DONE;
	}

	// 取得
	*pData = IN32 (FIRM_DATA_TEMP_ABNORMAL_COUNT);

_DONE:
	return (status);
}


//**********************************************************************************
//	Temp Abnormal Acquisition設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：カウント
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tempSetAbnormalAcquisition (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// 設定
	OUT32 (FIRM_DATA_TEMP_ABNORMAL_ACQUISITIONT, data);

	return (status);
}


//**********************************************************************************
//	Temp Abnormal Acquisition取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：カウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tempGetAbnormalAcquisition (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Temp Abnormal Acquisition NULL Parameter Error.\n");
		goto _DONE;
	}

	// 取得
	*pData = IN32 (FIRM_DATA_TEMP_ABNORMAL_ACQUISITIONT);

_DONE:
	return (status);
}


//**********************************************************************************
//	Temp Check Abnorma
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tempAbnormalCheck (void)
{
	int status = AVAL_STATUS_SUCCESS;
	double fpgaTemp, fpgaTempMax, fpgaTempMin;
	unsigned int tempStatus;
	unsigned int count;
	int startMode;
	unsigned int powerSupply;

	// FPGA温度
	if ((status = fpgaGetTemp (&fpgaTemp)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//	Temp Abnormal Status取得
	if ((status = tempGetAbnormalStatus (&tempStatus)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Power Supply
	if ((status = cameraPowerSupply (&powerSupply)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (tempStatus == 0)
	{
		// 電源供給もとにより、FPGA温度MAX値が異なる
		if (powerSupply & FPGA_POWER_STATUS_POWER_CONNECTOR)
			fpgaTempMax = TEMP_ABNORMAL_FPGA_UPPER_EXT;
		else
			fpgaTempMax = TEMP_ABNORMAL_FPGA_UPPER_PO;
		
		// FPGA上限温度異常？
		if (fpgaTemp >= fpgaTempMax)
		{
			// Temp Abnormal Status設定
			tempSetAbnormalStatus (1);
			
			// Temp Abnormal Count設定
			tempGetAbnormalCount (&count);
			tempSetAbnormalCount (count+1);
			
			// HW System Error
			ledHwSystemErrorState ();
			
			// Start Status
			acquisitionGetStartFlag (&startMode);

			// Set Start Status
			tempSetAbnormalAcquisition (startMode);
			
			if (startMode != 0)
			{
				// 取り込み停止
				if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
					goto _DONE;
			}

			// Peltier Off
			if ((status = peltierSetMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}
	else
	{
		// 電源供給もとにより、FPGA温度MIN値が異なる
		if (powerSupply & FPGA_POWER_STATUS_POWER_CONNECTOR)
			fpgaTempMin = TEMP_ABNORMAL_FPGA_LOWER_EXT;
		else
			fpgaTempMin = TEMP_ABNORMAL_FPGA_LOWER_PO;
		
		// FPGA下限温度異常？
		if (fpgaTemp <= fpgaTempMin)
		{
			// Temp Abnormal Status設定
			tempSetAbnormalStatus (0);

			// LED
			ledConnectStateForce();
		}
	}

_DONE:
	return (status);
}
#endif// #if defined (MODE_TEMP_ABNORMAL_CHECK)


#if defined (MODE_PELTIER_CTRL)
//**********************************************************************************
//	Peltier Control Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：カウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierControlInitiallize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	
	return (status);
}


//**********************************************************************************
//	Peltier Control Procs
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierControlProcsInit (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int low, high;
	
	// Get Clip
	if ((status = peltierGetClip (&low, &high)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
		
	// Set Clip
	if ((status = peltierSetClip (low, 0x00)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set Gigh Clip(DDR)
	if ((status = peltierSetPowerHighClip (high)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Control Procs
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierControlProcs (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int low, high, highSave;

	// Get Clip
	if ((status = peltierGetClip (&low, &high)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set Gigh Clip(DDR)
	if ((status = peltierGetPowerHighClip (&highSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 設定値の80%
	high = highSave * 0.8;

	for (; high<highSave; high+=0x4)
	{
		// Delay
		msDelay (100);
		
		if (high > highSave)
			high = highSave;

		// Set Clip
		if ((status = peltierSetClip (low, high)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	
	// Get Clip
	if ((status = peltierGetClip (&low, &high)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (highSave != high)
	{
		// Delay
		msDelay (100);

		// Set Clip
		if ((status = peltierSetClip (low, highSave)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	
_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Power Level取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pLevel				：パワーレベルを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetPowerLevel (int *pLevel)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check pLevel Parameter
	if (pLevel == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Power Level NULL Parameter Error.\n");
		goto _DONE;
	}

	// データ取得
	*pLevel = IN32 (FIRM_DATA_PELTIER_POWER_LEVEL);

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Power Level設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		level				：パワーレベル
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierSetPowerLevel (int level)
{
	int status = AVAL_STATUS_SUCCESS;
	int peltierMode;
	unsigned int low, high;
	unsigned int powerSupply;

	// Check level Parameter
	if ((level < PELTIER_POWER_LEVEL_MIN) || (level > PELTIER_POWER_LEVEL_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Power Level(%d) Parameter Error.(Min:%d / Max:%d)\n", level, PELTIER_POWER_LEVEL_MIN, PELTIER_POWER_LEVEL_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Peltier Mode
	if ((status = peltierGetMode (&peltierMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// Peltier Disable
	if (peltierMode == MODE_ENABLE)
	{
		if ((status = peltierSetMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Get Clip
	if ((status = peltierGetClip (&low, &high)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (level == PELTIER_POWER_LEVEL_LOW)
		high = PELTIER_POWER_LEVEL_LOW_CLIP;
	else if (level == PELTIER_POWER_LEVEL_MID)
		high = PELTIER_POWER_LEVEL_MIDLE_CLIP;
	else if (level == PELTIER_POWER_LEVEL_HIGH)
		high = PELTIER_POWER_LEVEL_HIGH_CLIP;
	else
	{
		// Get Power Supply
		if ((status = cameraPowerSupply (&powerSupply)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 電源供給もとにより、Clip値が異なる
		if (powerSupply & FPGA_POWER_STATUS_POWER_CONNECTOR)
		{
			high = PELTIER_POWER_LEVEL_ULTRA_CLIP;
		}
		else
		{
			high = PELTIER_POWER_LEVEL_HIGH_CLIP;
			level = PELTIER_POWER_LEVEL_HIGH;
		}
	}
	
	// Set Clip
	if ((status = peltierSetClip (low, high)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// データ設定
	OUT32 (FIRM_DATA_PELTIER_POWER_LEVEL, level);

	if (peltierMode == MODE_ENABLE)
	{
		// Peltier Control Initialize
		if ((status = peltierControlProcsInit ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Peltier Enable
		if ((status = peltierSetMode (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Power High Clip取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：High Clipを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetPowerHighClip (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Power High Clip NULL Parameter Error.\n");
		goto _DONE;
	}

	// データ取得
	*pData = IN32 (FIRM_DATA_PELTIER_POWER_HIGH_CLIP);

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Power High Clip設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：High Clip
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierSetPowerHighClip (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// データ設定
	OUT32 (FIRM_DATA_PELTIER_POWER_HIGH_CLIP, data);

	return (status);
}


//**********************************************************************************
//	Peltier Power Start Flag取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：StartFlagを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierGetPowerStartFlag (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Peltier Power Start Flag NULL Parameter Error.\n");
		goto _DONE;
	}

	// データ取得
	*pData = IN32 (FIRM_DATA_PELTIER_POWER_START_FLAG);

_DONE:
	return (status);
}


//**********************************************************************************
//	Peltier Power Start Flag設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：StartFlag
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int peltierSetPowerStartFlag (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check mode Parameter
	if ((data != MODE_ENABLE) && (data != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Peltier Power Start Flag(%d) Parameter Error.(Disable:%d / Enable:%d)\n", data, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// データ設定
	OUT32 (FIRM_DATA_PELTIER_POWER_START_FLAG, data);

_DONE:
	return (status);
}

#endif // #if defined (MODE_PELTIER_CURRENT_LIMIT_CTRL)


//**********************************************************************************
//	Get FPGA Interrupt
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		*pData					：FPGA割り込み有効／無効データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int fpgaGetInterrupt (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Get Interrupt NULL Parameter Error.\n");
		goto _DONE;
	}

	*pData = IN32 (FPGA_INT_ENABLE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	Set FPGA Interrupt
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data					：FPGA割り込み有効／無効データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int fpgaSetInterrupt (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	OUT32 (FPGA_INT_ENABLE_ADRS, data);
	
	return (status);
}


//**********************************************************************************
//	Get FPGA Interrupt Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		*pData					：FPGA割り込み有効／無効データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int fpgaGetIntStatus (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Get Interrupt Status NULL Parameter Error.\n");
		goto _DONE;
	}

	*pData = IN32 (FPGA_INT_STATUS_ADRS);
	
_DONE:
	return (status);
}


//**********************************************************************************
//	Get FPGA Temp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTemp					：FPGA温度を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int fpgaGetTemp (double *pTemp)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int dataI;
	double dataD;

	// Check pTemp Parameter
	if (pTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Temp NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Data
	data32 = IN32 (FPGA_TEMP_ADRS);

	// Get Temp
	if ((status = fpgaCalcRegToTemp (data32, pTemp)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Get FPGA Temp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data					：レジスタ値
//		pTemp					：FPGA温度を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int fpgaCalcRegToTemp (unsigned int data, double *pTemp)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32Save;
	int dataI, dataI2;
	double dataD;
	

	// Check pTemp Parameter
	if (pTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Calc Temp NULL Parameter Error.\n");
		goto _DONE;
	}

	// save
	data32Save = data;
	
	// 負数をクリア
	data &= ~FPGA_TEMP_CODE_BIT;
	
	// 整数部
	dataI = data >> FPGA_TEMP_INTEGER_SHIFT;
	dataI &= FPGA_TEMP_INTEGER_MASK;
	
	if (data32Save & FPGA_TEMP_CODE_BIT)
	{
		// 2の補数で計算
		dataI2 = ~dataI + 1;
	}
	else
	{
		dataI2 = dataI;
	}
	
	// 小数部
	dataD = (double)(data & FPGA_TEMP_FRACTIONAL_MASK);
	dataD /= FPGA_TEMP_FRACTIONAL_POINT;
	
	// 温度取得
	*pTemp = (double)dataI + dataD;

_DONE:
	return (status);
}


//**********************************************************************************
//	Get FPGA Temp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		temp					：FPGA温度
//		pData					：レジスタ値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int fpgaCalcTempToReg (double temp, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, data32Point;
	int dataI, dataI2;
	double dataDPoint;

	// Check pTemp Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Calc Data NULL Parameter Error.\n");
		goto _DONE;
	}

	// 整数部
	dataI = (int)temp;

	// 負数？
	if (dataI < 0)
	{
		// 2の補数で計算
		dataI2 = ~dataI + 1;
	}
	else
	{
		dataI2 = dataI;
	}

	// 整数部
	data32 = dataI2 & FPGA_TEMP_INTEGER_MASK;
	data32 = data32 << FPGA_TEMP_INTEGER_SHIFT;

	// 小数部
	dataDPoint = (temp - dataI);
	dataDPoint += 0.05;			// 四捨五入
	data32Point = dataDPoint * FPGA_TEMP_FRACTIONAL_POINT;

	// レジスタ値
	*pData = data32 | data32Point;

_DONE:
	return (status);
}


//**********************************************************************************
//	FPGA通知温度取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTempOver				：センサ通知温度の上限値を格納するポインタ
//		pTempUnder				：センサ通知温度の下限値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int fpgaGetTempAlarm (double *pTempOver, double *pTempUnder)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int reg32, data32;

	// Check pTempOver Parameter
	if (pTempOver == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Temp Alarm Over NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pTempUnder Parameter
	if (pTempUnder == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Temp Alarm Under NULL Parameter Error.\n");
		goto _DONE;
	}

	// データ取得
	reg32 = IN32 (FPGA_TEMP_ALARM_ADRS);

	// 上限値取得
	data32 = reg32 >> 16;
	data32 &= 0xffff;

	// 上限値計算
	if ((status = fpgaCalcRegToTemp (data32, pTempOver)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 下限値取得
	data32 = reg32 & 0xffff;

	// 下限値計算
	if ((status = fpgaCalcRegToTemp (data32, pTempUnder)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	FPGA通知温度設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		tempOver				：センサ通知温度の上限値
//		tempUnder				：センサ通知温度の下限値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int fpgaSetTempAlarm (double tempOver, double tempUnder)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, data32H, data32L;
	double tempMin, tempMax;
	unsigned int tempStatus;
	unsigned int dataOver, dataUnder;

	// Check tempOver Parameter
	if ((tempOver < FPGA_TEMP_MIN) || (tempOver > FPGA_TEMP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FPGA Alarm tempOver(%.2f) Parameter Error.(Min:%.2f / Max:%.2f)\n", tempOver, FPGA_TEMP_MIN, FPGA_TEMP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check tempUnder Parameter
	if ((tempUnder < FPGA_TEMP_MIN) || (tempUnder > FPGA_TEMP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FPGA Alarm tempUnder(%.2f) Parameter Error.(Min:%.2f / Max:%.2f)\n", tempUnder, FPGA_TEMP_MIN, FPGA_TEMP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check tempOver/tempUnder Parameter
	if (tempOver < tempUnder)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_PELTIER, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FPGA Alarm tempOver(%.2f) < tempUnder(%.2f) Parameter Error.\n", tempOver, tempUnder);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 温度ステータスクリア
	tempStatus = IN32 (BOARD_STATUS_TEMP_ADRS);

	if (tempStatus & FPGA_TEMP_STATUS)
	{
		// FPGA割り込み無効
		if ((status = fpgaSetInterrupt (0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// ステータスクリア
		tempStatus &= ~FPGA_TEMP_STATUS;
		OUT32 (BOARD_STATUS_TEMP_ADRS, tempStatus);

		// Check Status
		if (tempStatus == 0)
			ledConnectStateForce();

		// FPGA割り込み有効
		if ((status = fpgaSetInterrupt (FPGA_INT_TEMP_UPPER)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// 上限値計算
	if ((status = fpgaCalcTempToReg (tempOver, &dataOver)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 下限値計算
	if ((status = fpgaCalcTempToReg (tempUnder, &dataUnder)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// レジスタ設定
	data32 = (dataOver << 16) | dataUnder;
	OUT32 (FPGA_TEMP_ALARM_ADRS, data32);

_DONE:
	return (status);
}

// eof
