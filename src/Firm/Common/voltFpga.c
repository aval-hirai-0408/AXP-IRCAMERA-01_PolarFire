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
//**********************************************************************************
// FPGA Board V18 取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：レジスタアドレス
//		hi					：0=adrsの下位16bitを使用/1=adrsの上位16bitを使用
//		pVolt				：電圧値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int voltFpgaGetV (unsigned int adrs, int hi, double *pVolt)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32, data32Save;
	int dataI;
	double dataD;

	// Check pVolt Parameter
	if (pVolt == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FPGA Board Voltage NULL Parameter Error. Adrs = 0x%08x, Hi_Low = %d\n", adrs, hi);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Data
	data32 = IN32 (adrs);
	
	if (hi == 1)
		data32 >>= 16;
	
	data32 &= 0xffff;
	data32Save = data32;
	
	// 符号を消す
	data32 &= ~FPGA_VOLT_SIGH_BIT;

	// 整数部
	dataI = data32 >> FPGA_VOLT_INTEGER_SHIFT;
	dataI &= FPGA_VOLT_INTEGER_MASK;
	
	// 小数部
	dataD = (double)(data32 & FPGA_VOLT_FRACTIONAL_MASK);
	dataD /= FPGA_VOLT_FRACTIONAL_POINT;
	
	// 温度取得
	*pVolt  = (double)dataI + dataD;
	*pVolt /= 1000.0;		// mv単位をvに変更

	// 負数？
	if (data32Save & FPGA_VOLT_SIGH_BIT)
		*pVolt = 0 - *pVolt;

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
	double volt;

	DEBUG_PRINT_FORCE ("\n[FPGA Board Voltage]\n\n");

	// +1.8V取得
	if ((status = voltFpgaGetV (FPGA_VOLT0_ADRS, 1, &volt)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +1.8V                     %.3f [v]\n\n", volt);
	else
		DEBUG_PRINT_FORCE ("   +1.8V                     Error\n\n");

	// +1.0V取得
	if ((status = voltFpgaGetV (FPGA_VOLT0_ADRS, 0, &volt)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +1.0V                     %.3f [v]\n\n", volt);
	else
		DEBUG_PRINT_FORCE ("   +1.0V                     Error\n\n");

	// +2.5V取得
	if ((status = voltFpgaGetV (FPGA_VOLT1_ADRS, 0, &volt)) == AVAL_STATUS_SUCCESS)
		DEBUG_PRINT_FORCE ("   +2.5V                     %.3f [v]\n\n", volt);
	else
		DEBUG_PRINT_FORCE ("   +2.5V                     Error\n\n");

	DEBUG_PRINT_FORCE ("\n");

	return (status);
}
#endif // #if defined (MODE_VOLTAGE_FPGA_BOARD)

// eof
