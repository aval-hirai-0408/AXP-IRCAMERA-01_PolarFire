//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// voltageIfBoard.c - Voltage If Board Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_VOLTAGE_IF_BOARD)
//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gBoardVoltStatus = 0;


//**********************************************************************************
// Interface Board 電圧初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	gBoardVoltStatus = status;
	//@@@1
	return (status);
}


//**********************************************************************************
// Interface Board Check Volt Valid
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfCheckValid (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int data;

	//@@@1
	#if 0
	for (i=0; i<FPGA_VOLT_IF_VALID_TIMEOUT; i++)
	{
		data = IN32(FPGA_VOLT_IF_VT_VALID_ADRS);

		if (data & FPGA_VOLT_IF_VT_VALID_BIT)
			break;

		usDelay (1000);	// 1m
	}

	// Check Timeout
	if (i >= FPGA_VOLT_IF_VALID_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Volt Valid Timeout Error.\n");
		goto _DONE;
	}
	#endif
	//@@@1

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +5.0V Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet5_0 (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +5.0V NULL Parameter Error.\n");
		goto _DONE;
	}

	// Interface Board +5.0V Voltage取得
	data = IN32 (FPGA_GPIO_VOLT0_ADRS);
	dataD = (data >> 16) & 0x7fff;
	*pData = dataD; 

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +24.0V Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet24_0 (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +24.0V NULL Parameter Error.\n");
		goto _DONE;
	}

	// Interface Board +24.0V Voltage取得
	data = IN32 (FPGA_GPIO_VOLT0_ADRS);
	dataD = data & 0x7fff;
	*pData = dataD; 

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +1.8V Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet1_8 (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +1.8V NULL Parameter Error.\n");
		goto _DONE;
	}

	// Interface Board +1.8V Voltage取得
	data = IN32 (FPGA_GPIO_VOLT1_ADRS);
	dataD = (data >> 16) & 0x7fff;
	*pData = dataD; 

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +3.3V Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet3_3 (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +3.3V NULL Parameter Error.\n");
		goto _DONE;
	}

	// Interface Board +3.3V Voltage取得
	data = IN32 (FPGA_GPIO_VOLT1_ADRS);
	dataD = data & 0x7fff;
	*pData = dataD; 

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +1.25V Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet1_25 (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +1.25V NULL Parameter Error.\n");
		goto _DONE;
	}

	// Interface Board +1.25V Voltage取得
	data = IN32 (FPGA_GPIO_VOLT2_ADRS);
	dataD = data & 0x7fff;
	*pData = dataD; 

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfBoardShow (void)
{
	int status = AVAL_STATUS_SUCCESS;
	double data;

	DEBUG_PRINT_FORCE ("\n[Interface Board Voltage]\n\n");

	if (gInterFaceID == INTERFACE_CXP)
	{
		// +5.0V Voltage取得
		if ((status = voltIfGet5_0 (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +5.0V                     %.3f [v]\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   +5.0V                     Error\n\n");

		// +24.0V Voltage取得
		if ((status = voltIfGet24_0 (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +24.0V                    %.3f [v]\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   +24.0V                    Error\n\n");

		// +1.8V Voltage取得
		if ((status = voltIfGet1_8 (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +1.8V                     %.3f [v]\n\n",data);
		else
			DEBUG_PRINT_FORCE ("   +1.8V                     Error\n\n");

		// +1.25V Voltage取得
		if ((status = voltIfGet1_25 (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +1.25V                    %.3f [v]\n\n",data);
		else
			DEBUG_PRINT_FORCE ("   +1.25V                    Error\n\n");
	}

	DEBUG_PRINT_FORCE ("\n");

	return (status);
}

#endif // #if defined (MODE_VOLTAGE_IF_BOARD)

// eof
