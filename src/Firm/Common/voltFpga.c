//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// voltage.c - Voltage Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_VOLTAGE_FPGA_BOARD)
#if defined (MODE_SYSTEM_MANAGEMENT)
//**********************************************************************************
// FPGA Board V09 取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGet09v (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board +0.9V Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// V09 取得
	data32 = IN32 (FPGA_SM_VAUX0_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 977 / 1000 / 1000;

_DONE:
	return (status);
}


//**********************************************************************************
// FPGA Board V12Sys 取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGet12vSys (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board +1.2V Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// V12Sys 取得
	data32 = IN32 (FPGA_SM_VAUX2_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 977 / 1000 / 1000 * 1.33;

_DONE:
	return (status);
}


//**********************************************************************************
// FPGA Board +2.5V 取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGet25v (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board +2.5V Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// +2.5V 取得
	data32 = IN32 (FPGA_SM_VAUX3_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;


	*pData = (double)data32 * 977 / 1000 / 1000 * 4.0303;

_DONE:
	return (status);
}


//**********************************************************************************
// FPGA Board +1.15V 取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGet115v (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board +1.15V Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// +1.15V 取得
	data32 = IN32 (FPGA_SM_VAUX4_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 977 / 1000 / 1000 * 1.33;

_DONE:
	return (status);
}


//**********************************************************************************
// FPGA Board +3.3 VD取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGet33vd (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board +3.3VD Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// +3.3VD 取得
	data32 = IN32 (FPGA_SM_VAUX6_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 977 / 1000 / 1000 * 4.0303;

_DONE:
	return (status);
}


//**********************************************************************************
// FPGA Board VTT 取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGetVtt (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board VTT Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// VTT 取得
	data32 = IN32 (FPGA_SM_VAUX7_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 977 / 1000 / 1000;

_DONE:
	return (status);
}


//**********************************************************************************
// FPGA Board +1.2V取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGet12v (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board +1.2V Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// +1.2V 取得
	data32 = IN32 (FPGA_SM_VAUX9_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 977 / 1000 / 1000 * 1.33;

_DONE:
	return (status);
}


//**********************************************************************************
// FPGA Board +3.3V取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGet33v (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board +3.3V Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// +3.3V 取得
	data32 = IN32 (FPGA_SM_VAUX11_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 977 / 1000 / 1000 * 4.0303;

_DONE:
	return (status);
}


//**********************************************************************************
// FPGA Board +3.3VA取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGet33va (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board +3.3VA Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// +3.3VA 取得
	data32 = IN32 (FPGA_SM_VAUX12_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 977 / 1000 / 1000 * 4.0303;

_DONE:
	return (status);
}


//**********************************************************************************
// FPGA Board +5V取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGet50v (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board +5V Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// +5V 取得
	data32 = IN32 (FPGA_SM_VAUX13_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 977 / 1000 / 1000 * 5.7;

_DONE:
	return (status);
}


//**********************************************************************************
// FPGA Board VREFV取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGetVref (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Board VREF Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// VREF 取得
	data32 = IN32 (FPGA_SM_VAUX15_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 977 / 1000 / 1000;

_DONE:
	return (status);
}


//**********************************************************************************
// VCCINT取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGetVccint (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "VCCINT Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// VCCINT取得
	data32 = IN32 (FPGA_SM_VCCINT_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 2.93 / 1000;

_DONE:
	return (status);
}


//**********************************************************************************
// VCCAUX取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGetVccaux (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "VCCAUX Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// VCCAUX取得
	data32 = IN32 (FPGA_SM_VCCAUX_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 2.93 / 1000;

_DONE:
	return (status);
}


//**********************************************************************************
// VBRAM取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGetVbram (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "VBRAM Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// VBRAM取得
	data32 = IN32 (FPGA_SM_VBRAM_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 2.93 / 1000;

_DONE:
	return (status);
}


//**********************************************************************************
// VCC_PSINTLPM取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGetVccPsintlp (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "VCC PSINTPL Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// PSINTPL取得
	data32 = IN32 (FPGA_SM_VCC_PSINTLP_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 2.93 / 1000;

_DONE:
	return (status);
}


//**********************************************************************************
// VCC_PSINFPM取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGetVccPsinfp (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "VCC PSINFP Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// PSINFP取得
	data32 = IN32 (FPGA_SM_VCC_PSINFP_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 2.93 / 1000;

_DONE:
	return (status);
}


//**********************************************************************************
// VCC_PSAUXM取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGetVccPSAux (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "VCC PSAUX Voltage pData NULL Parameter Error\n");
		goto _DONE;
	}

	// PSAUX取得
	data32 = IN32 (FPGA_SM_VCC_PSAUX_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;

	*pData = (double)data32 * 2.93 / 1000;

_DONE:
	return (status);
}


//**********************************************************************************
// Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaBoardShow (void)
{
	int status = AVAL_STATUS_SUCCESS;
	double data;

	DEBUG_PRINT_FORCE ("\n[FPGA Board Voltage]\n\n");

	// +0.9V取得
	if ((status = voltFpgaGet09v (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +0.9V                     %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   +0.9V                     Error\n\n");

	// +1.2V SYS取得
	if ((status = voltFpgaGet12vSys (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +1.2V SYS                 %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   +1.2V SYS                 Error\n\n");

	// +2.5V取得
	if ((status = voltFpgaGet25v (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +2.5V                     %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   +2.5V                     Error\n\n");

	// +1.15V取得
	if ((status = voltFpgaGet115v (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +1.15V                    %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   +1.15V                    Error\n\n");

	// +3.3vd取得
	if ((status = voltFpgaGet33vd (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +3.3VD                    %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   +3.3VD                    Error\n\n");

	// VTT取得
	if ((status = voltFpgaGetVtt (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   VTT                       %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   VTT                       Error\n\n");

	// +1.2V取得
	if ((status = voltFpgaGet12v (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +1.2V                     %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   +1.2V                     Error\n\n");

	// +3.3V取得
	if ((status = voltFpgaGet33v (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +3.3V                     %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   +3.3V                     Error\n\n");

	// +3.3VA取得
	if ((status = voltFpgaGet33va (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +3.3VA                    %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   +3.3VA                    Error\n\n");

	// +5.0V取得
	if ((status = voltFpgaGet50v (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +5.0V                     %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   +5.0V                     Error\n\n");

	// VREF取得
	if ((status = voltFpgaGetVref (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   VREF                      %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   VREF                      Error\n\n");

	// VCCINT取得
	if ((status = voltFpgaGetVccint (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   VCCINT                    %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   VCCINT                    Error\n\n");

	// VCCAUX取得
	if ((status = voltFpgaGetVccaux (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   VCCAUX                    %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   VCCAUX                    Error\n\n");

	// VBRAM取得
	if ((status = voltFpgaGetVbram (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   VBRAM                     %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   VBRAM                     Error\n\n");

	// VCCPSINTLP取得
	if ((status = voltFpgaGetVccPsintlp (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   VCC PSINTLP               %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   VCC PSINTLP               Error\n\n");

	// VCCPSINFP取得
	if ((status = voltFpgaGetVccPsinfp (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   VCC PSINFP                %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   VCC PSINFP                Error\n\n");

	// VCCPSAUX取得
	if ((status = voltFpgaGetVccPSAux (&data)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   VCC PSAUX                 %.3f [v]\n\n", data);
	else
		DEBUG_PRINT_FORCE ("   VCC PSAUX                 Error\n\n");

	DEBUG_PRINT_FORCE ("\n");

	return (status);
}
#endif // #if defined (MODE_SYSTEM_MANAGEMENT)
#endif // #if defined (MODE_VOLTAGE_FPGA_BOARD)

// eof
