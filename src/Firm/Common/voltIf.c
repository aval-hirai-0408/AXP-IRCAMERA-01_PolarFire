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


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gBoardVoltStatus = 0;


#if defined (MODE_VOLTAGE_IF_BOARD)

// Valid Flag Timeout
#define FPGA_VOLT_IF_VALID_TIMEOUT									(3000)

// Internal Temp
#define FPGA_INTERNAL_TEMP_COE										(0.0625)


#if defined (MODE_BOARD_ACB531CXP) || defined (MODE_BOARD_ACB532GE)

#define FPGA_VOLT_IF_LSB				(305.18)


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
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Volt Valid Timeout Error\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +1.05VD Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet105vd (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +1.05VD Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +1.05VD Voltage取得
	data = IN32 (FPGA_VOLT_IF_105VD_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;			// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;				// uV→V
	*pData = dataD * 4.668;						// 抵抗分圧

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +1.05VA Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet105va (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +1.05VA Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +1.05VA Voltage取得
	data = IN32 (FPGA_VOLT_IF_105VA_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;			// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;				// uV→V
	*pData = dataD * 4.668;						// 抵抗分圧

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +1.25VD Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet125vd (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +1.25VD Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +1.25VD Voltage取得
	data = IN32 (FPGA_VOLT_IF_125VD_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;		// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;			// uV→V
	*pData = dataD * 5.586;					// 抵抗分圧

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +1.8VD Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet18vd (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +1.8VD Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +1.8VD Voltage取得
	data = IN32 (FPGA_VOLT_IF_18VD_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;		// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;			// uV→V
	*pData = dataD * 9.0361;				// 抵抗分圧

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +2.5VF Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet25vf (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +2.5VF Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +2.5VF Voltage取得
	data = IN32 (FPGA_VOLT_IF_25VF_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;		// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;			// uV→V
	*pData = dataD * 12.939;				// 抵抗分圧

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +3.3VD Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet33vd (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +3.3VD Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +3.3VD Voltage取得
	data = IN32 (FPGA_VOLT_IF_33VD_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;		// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;			// uV→V
	*pData = dataD + 2.5;


_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +9.0VA Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet90va (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +9.0VA Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +9.0VA Voltage取得
	data = IN32 (FPGA_VOLT_IF_90VA_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;		// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;			// uV→V
	*pData = dataD * 40.104;				// 抵抗分圧

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
		// +1.05VD Voltage取得
		if ((status = voltIfGet105vd (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +1.05VD                   %.3f [v]\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   +1.05VD                   Error\n\n");

		// +1.05VA Voltage取得
		if ((status = voltIfGet105va (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +1.05VA                   %.3f [v]\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   +1.05VA                   Error\n\n");

		// +1.25VD Voltage取得
		if ((status = voltIfGet125vd (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +1.25VD                   %.3f [v]\n\n",data);
		else
			DEBUG_PRINT_FORCE ("   +1.25VD                   Error\n\n");

		// +1.8VD Voltage取得
		if ((status = voltIfGet18vd (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +1.8VD                    %.3f [v]\n\n",data);
		else
			DEBUG_PRINT_FORCE ("   +1.8VD                    Error\n\n");

		// +2.5VF Voltage取得
		if ((status = voltIfGet25vf (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +2.5VF                    %.3f [v]\n\n",data);
		else
			DEBUG_PRINT_FORCE ("   +2.5VF                    Error\n\n");

		// +3.3VD Voltage取得
		if ((status = voltIfGet33vd (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +3.3VD                    %.3f [v]\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   +3.3VD                    Error\n\n");

		// +9.0VA Voltage取得
		if ((status = voltIfGet90va (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +9.0VA                    %.3f [v]\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   +9.0VA                    Error\n\n");

		// Internal Temp取得
		if ((status = ifGetInternalTemp (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   Internal Temperature      %.2f\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   Internal Temperature      Error\n\n");
	}
	else if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
			// +0.65V Voltage取得
		if ((status = voltIfGet065v (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +0.65V                    %.3f [v]\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   +0.65V                    Error\n\n");

		// +0.85VA Voltage取得
		if ((status = voltIfGet085va (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +0.85VA                   %.3f [v]\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   +0.85VA                   Error\n\n");

		// +2.30VA Voltage取得
		if ((status = voltIfGet230va (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +2.30VA                   %.3f [v]\n\n",data);
		else
			DEBUG_PRINT_FORCE ("   +2.30VA                   Error\n\n");

		// +3.30V Voltage取得
		if ((status = voltIfGet330v (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +3.30V                    %.3f [v]\n\n",data);
		else
			DEBUG_PRINT_FORCE ("   +3.30V                    Error\n\n");

		// +12.0VP Voltage取得
		if ((status = voltIfGet120vp (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +12.0VP                   %.3f [v]\n\n",data);
		else
			DEBUG_PRINT_FORCE ("   +12.0VP                   Error\n\n");

		// +24.0V Voltage取得
		if ((status = voltIfGet240v (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   +24.0V                    %.3f [v]\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   +24.0V                    Error\n\n");

		// Internal Temp取得
		if ((status = ifGetInternalTemp (&data)) == AVAL_STATUS_SUCCESS)
			DEBUG_PRINT_FORCE ("   Internal Temperature      %.2f\n\n", data);
		else
			DEBUG_PRINT_FORCE ("   Internal Temperature      Error\n\n");
	}

	DEBUG_PRINT_FORCE ("\n");

	return (status);
}


//**********************************************************************************
// Interface Board +0.65V Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet065v (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +0.65V Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +0.65VD Voltage取得
	data = IN32 (FPGA_VOLT_IF_065VD_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;			// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;				// uV→V
	*pData = dataD * 5.354165;					// 抵抗分圧

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +0.85VA Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet085va (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +0.85VA Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +0.85VA Voltage取得
	data = IN32 (FPGA_VOLT_IF_085VA_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;			// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;				// uV→V
	*pData = dataD * 5.330574;					// 抵抗分圧

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +2.3VA Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet230va (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +2.3VA Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +2.3VA Voltage取得
	data = IN32 (FPGA_VOLT_IF_230VA_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;		// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;			// uV→V
	*pData = dataD * 12.94993;				// 抵抗分圧

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
int voltIfGet330v (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +3.3V Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +3.3V Voltage取得
	data = IN32 (FPGA_VOLT_IF_330V_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;		// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;			// uV→V
	*pData = dataD + 2.5;

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Board +12.0VP Voltage取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltIfGet120vp (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +12.0VP Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +12.0VP Voltage取得
	data = IN32 (FPGA_VOLT_IF_120VP_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;		// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;			// uV→V
	*pData = dataD * 121.7304;				// 抵抗分圧

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
int voltIfGet240v (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	double dataD;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board +24.0V Voltage IF pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Board +24.0V Voltage取得
	data = IN32 (FPGA_VOLT_IF_240V_ADRS);
	dataD = data * FPGA_VOLT_IF_LSB;		// LSB
	dataD /= FPGA_VOLT_IF_V_UNIT;			// uV→V
	*pData = dataD * 121.7304;				// 抵抗分圧

_DONE:
	return (status);
}


//**********************************************************************************
// Interface Boardデバイス内部温度取得(LTC2990)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：デバイス内部温度を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ifGetInternalTemp (double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface Board Internal Temp pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Interface Boardデバイス内部温度取得
	data = IN32 (FPGA_INTERNAL_TEMP_ADRS);

	*pData = (double)data * FPGA_INTERNAL_TEMP_COE;	// 抵抗分圧

_DONE:
	return (status);
}

#endif // #if defined (MODE_BOARD_ACB531CXP) || defined (MODE_BOARD_ACB532GE)
#endif // #if defined (MODE_VOLTAGE_IF_BOARD)

// eof
