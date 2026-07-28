//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// boardStatus.c - Board Status Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------

// ステータス
extern int gIntcStatus;						// Interrupt Controller Status
extern int gTimerStatus;					// Timer Controller Status
extern int gCameraSaveStatus;				// Camera Save Status
extern int gUartStatus;						// UART Controller Status
extern int gQspiFlashStatus;				// QSPI Flash Status
extern int gAxiQspiFlashStatus;				// AXI QSPI Flash Status
extern int gDmaStatus;						// DMA Status
extern int gPeltierStatus;					// Peltier Status
extern int gAcquisitionStatus;				// Acquisition Status
extern int gDigitalIoStatus;				// Digital IO Status
extern int gCounterStatus;					// Counter Status
extern int gTimerControlStatus;				// Timer Control Status
extern int gEncoderStatus;					// Encoder Status
extern int gAoiStatus;						// AOI Status
extern int gRoiStatus;						// ROI Status
extern int gFfcStatus;						// FFC Status
extern int gDpcStatus;						// DPC Status
extern int gLutStatus;						// LUT Status
extern int gDogStatus;						// Digital Offset Gain Status
extern int gTimingGeneratorStatus;			// Timing Generator Status
extern int gSpectrumStatus;					// Spectrum Status
extern int gI2cPsStatus;					// I2C PS Status
extern int gBoardVoltStatus;				// Board Voltage Status
extern int gSensorTmgStatus;				// Sensor LVDS Status
extern int gSensorStatus;					// Sensor Status
extern int gAutoBrightStatus;				// Auto Bright Status

// 温度割り込みカウント
extern int gPeltierSensorUpperTempCount;	// センサ温度上限カウント
extern int gPeltierSensorLowerTempCount;	// センサ温度下限カウント
extern int gPeltierCaseTempCount;			// ケース温度カウント

extern int gSpectrumType;					// Spectrum Type


//**********************************************************************************
//	Camera Status初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraStatusInitialize (void)
{
	memset ((void *)BOARD_ERROR_ADRS, 0x00, BOARD_ERROR_SIZE);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Set Camera Error Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		num					：エラー項目番号
//		status				：エラーステータス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraSetStatus (int num, int cameraStatus)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check num Parameter
	if ((num < BOARD_STATUS_NUMBER_MIN) || (num > BOARD_STATUS_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Status num(%d) Parameter Error. (Min:1 / Max:%d)\n", num, BOARD_STATUS_NUMBER_MIN, BOARD_STATUS_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// ステータス設定
	OUT32 ((BOARD_ERROR_ADRS + (num*4)), cameraStatus);

_DONE:
	return (status);
}


//**********************************************************************************
//	Get Camera Error Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		num					：エラー項目番号
//		status				：エラーステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraGetStatus (int num, int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check num Parameter
	if ((num < BOARD_STATUS_NUMBER_MIN) || (num > BOARD_STATUS_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Status num(%d) Parameter Error. (Min:%d / Max:%d)\n", num, BOARD_STATUS_NUMBER_MIN, BOARD_STATUS_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Status pStatus NULL Parameter Error.\n");
		goto _DONE;
	}

	// ステータス取得
	*pStatus = IN32 ((BOARD_ERROR_ADRS + (num*4)));

_DONE:
	return (status);
}


//**********************************************************************************
//	Set Camera Error Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraSetStatusAll (void)
{
	// Error Selector Clear
	OUT32 (FIRM_DATA_CAMERA_ERROR_SELECT_ADRS, 0);

	// INTC
	OUT32 (BOARD_STATUS_INTC_ADRS, gIntcStatus);

	// Timer
	OUT32 (BOARD_STATUS_TIMER_ADRS, gTimerStatus);

	// UART
	OUT32 (BOARD_STATUS_UART_ADRS, gUartStatus);

	// QSPI Flash
	OUT32 (BOARD_STATUS_QSPI_ADRS, gQspiFlashStatus);

	// AXI QSPI Flash
	OUT32 (BOARD_STATUS_AXI_QSPI_ADRS, gAxiQspiFlashStatus);

	// DMA
	OUT32 (BOARD_STATUS_DMA_ADRS, gDmaStatus);

	// Peltier
	OUT32 (BOARD_STATUS_PELTIER_ADRS, gPeltierStatus);

	// Acquisition
	OUT32 (BOARD_STATUS_ACQUISITION_ADRS, gAcquisitionStatus);

	// Digital IO
	OUT32 (BOARD_STATUS_DIGITALIO_ADRS, gDigitalIoStatus);

	// Counter Control
	OUT32 (BOARD_STATUS_COUNTER_ADRS, gCounterStatus);

	// Timer Control
	OUT32 (BOARD_STATUS_TIMER_CONTROL_ADRS, gTimerControlStatus);

	// AOI
	OUT32 (BOARD_STATUS_AOI_ADRS, gAoiStatus);

	// ROI
	OUT32 (BOARD_STATUS_ROI_ADRS, gRoiStatus);

	// FFC
	OUT32 (BOARD_STATUS_FFC_ADRS, gFfcStatus);

	// DPC
	OUT32 (BOARD_STATUS_DPC_ADRS, gDpcStatus);

	// LUT
	OUT32 (BOARD_STATUS_LUT_ADRS, gLutStatus);

	// Digital Offset Gain
	OUT32 (BOARD_STATUS_DOG_ADRS, gDogStatus);

	// Userset
	OUT32 (BOARD_STATUS_USERSET_ADRS, gCameraSaveStatus);

	// センサ温度上限カウント
	OUT32 (BOARD_STATUS_SENSOR_TEMP_UPPER_ADRS, gPeltierSensorUpperTempCount);

	// センサ温度下限カウント
	OUT32 (BOARD_STATUS_SENSOR_TEMP_LOWER_ADRS, gPeltierSensorLowerTempCount);

	// ケース温度カウント
	OUT32 (BOARD_STATUS_CASE_TEMP_ADRS, gPeltierCaseTempCount);

	// Encoder Control
	OUT32 (BOARD_STATUS_ENCODER_CONTROL_ADRS, gEncoderStatus);

	// Timing Generator
	OUT32 (BOARD_STATUS_TIMING_GENERATOR_ADRS, gTimingGeneratorStatus);

	if (gSpectrumType == CAMERA_TYPE_SPECTRUM)
	{
		// Spectrum
		OUT32 (BOARD_STATUS_SPECTRUM_ADRS, gSpectrumStatus);
	}
	
	// I2C
	OUT32 (BOARD_STATUS_I2C_ADRS, gI2cPsStatus);

	// Board Voltage
	OUT32 (BOARD_STATUS_BOARD_VOLT_ADRS, gBoardVoltStatus);

	// Sensor Timing
	OUT32 (BOARD_STATUS_SENSOR_TIMING_ADRS, gSensorTmgStatus);

	// Sensor
	OUT32 (BOARD_STATUS_SENSOR_ADRS, gSensorStatus);

	// Auto Bright
	OUT32 (BOARD_STATUS_AUTO_BRIGHT_ADRS, gAutoBrightStatus);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Camera Status表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraStatusShow (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int errno;

	DEBUG_PRINT_FORCE ("[Camera Status]\n");

	//------------------------------------------------------------
	// Interrupt Controller
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Interrupt Controller       ");
	errno = IN32(BOARD_STATUS_INTC_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Timer Controller
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Timer Controller           ");
	errno = IN32(BOARD_STATUS_TIMER_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// UserSet
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   User Set                   ");
	errno = IN32(BOARD_STATUS_USERSET_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// UART Controller
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   UART                       ");
	errno = IN32(BOARD_STATUS_UART_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// QSPI Flash Controller
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   QSPI Flash                 ");
	errno = IN32(BOARD_STATUS_QSPI_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// AXI QSPI Flash Controller
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   AXI QSPI Flash             ");
	errno = IN32(BOARD_STATUS_AXI_QSPI_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// DMA Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   DMA                        ");
	errno = IN32(BOARD_STATUS_DMA_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Temperature Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Temperature                ");
	errno = IN32(BOARD_STATUS_PELTIER_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// AOI Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   AOI                        ");
	errno = IN32(BOARD_STATUS_AOI_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// ROI Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   ROI                        ");
	errno = IN32(BOARD_STATUS_ROI_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Acquisition Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Acquisition                ");
	errno = IN32(BOARD_STATUS_ACQUISITION_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Digital IO Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Digital IO                 ");
	errno = IN32(BOARD_STATUS_DIGITALIO_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Counter Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Counter Control            ");
	errno = IN32(BOARD_STATUS_COUNTER_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Timer Control Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Timer Control              ");
	errno = IN32(BOARD_STATUS_TIMER_CONTROL_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Encoder Control Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Encoder Control            ");
	errno = IN32(BOARD_STATUS_ENCODER_CONTROL_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// FFC Status
	//-----------------------------------------------------------
	DEBUG_PRINT_FORCE ("   FFC                        ");
	errno = IN32(BOARD_STATUS_FFC_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// DPC Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   DPC                        ");
	errno = IN32(BOARD_STATUS_DPC_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// LUT Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   LUT                        ");
	errno = IN32(BOARD_STATUS_LUT_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Digital Offset Gain Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Digital Offset Gain        ");
	errno = IN32(BOARD_STATUS_DOG_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Timing Generator Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Timing Generator           ");
	errno = IN32(BOARD_STATUS_TIMING_GENERATOR_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Spectrum Status
	//------------------------------------------------------------
	if (gSpectrumType == CAMERA_TYPE_SPECTRUM)
	{
		DEBUG_PRINT_FORCE ("   Spectrum                   ");
		errno = IN32(BOARD_STATUS_SPECTRUM_ADRS);
		if (errno == 0)
			DEBUG_PRINT_FORCE ("[OK]\n");
		else
			DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));
	}

	//------------------------------------------------------------
	// I2C Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   I2C                        ");
	errno = IN32(BOARD_STATUS_I2C_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Board Voltage Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Board Voltage              ");
	errno = IN32(BOARD_STATUS_BOARD_VOLT_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Sensor Timing
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Sensor Timing              ");
	errno = IN32(BOARD_STATUS_SENSOR_TIMING_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Sensor
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Sensor                     ");
	errno = IN32(BOARD_STATUS_SENSOR_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	//------------------------------------------------------------
	// Auto Bright Status
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Auto Bright                ");
	errno = IN32(BOARD_STATUS_AUTO_BRIGHT_ADRS);
	if (errno == 0)
		DEBUG_PRINT_FORCE ("[OK]\n");
	else
		DEBUG_PRINT_FORCE ("[ERROR] : Dev = %d / Error = %d\n", MAKE_DEVICE_STATUS(errno), MAKE_ERRNUM_STATUS(errno));

	DEBUG_PRINT_FORCE ("\n");

	return (status);
}

// eof
