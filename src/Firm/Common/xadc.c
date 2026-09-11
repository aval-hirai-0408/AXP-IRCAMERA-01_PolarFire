//**********************************************************************************
//
//                               ARM Proram
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// xadc.c - XADC Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_SYSTEM_MANAGEMENT)
//**********************************************************************************
//	Get FPGA Temp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTemp					：FPGA温度を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int xadcGetFpgaTemp (double *pTemp)
{
	int status = AVAL_STATUS_SUCCESS;
	double dataD;
	unsigned int data32;

//@@@1
	*pTemp = 0;
//@@@1
	
	// Check pTemp Parameter
	if (pTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "XADC pTemp NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Data
	data32 = IN32 (FPGA_SM_TEMP_ADRS);

	// 上位10bitが有効
	data32 >>= FPGA_SM_DATA_SHIFT;
	data32 &= FPGA_SM_DATA_MASK;
	dataD  = (double)data32;

	// 温度変換
	*pTemp = (dataD * 509.3140064 / 1024) - 280.23087870;

_DONE:
	return (status);
}
#endif // #if defined (MODE_SYSTEM_MANAGEMENT)

// eof
