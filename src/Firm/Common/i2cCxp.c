//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cxpi2c.c - I2C CXP Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"

#if defined (MODE_I2C)

//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------

// I2c Port
#define CXP_I2C_PORT						(I2C_PORT1)

// I2C Clock
#define CXP_I2C_SCLK_RATE					(400000)

// Slave Address
#if defined (MODE_BOARD_ACB525CXP)
#define CXP_I2C_SLAVE_ADRS					(0x4c)	// 0x98>>1
#else
#define CXP_I2C_SLAVE_ADRS					(0x10)	// 0x20>>1
#endif

// Register Min/Max
#if defined (MODE_BOARD_ACB525CXP)
#define CXP_I2C_REG_ADRS_MIN				(0x00)
#define CXP_I2C_REG_ADRS_MAX				(0xff)
#else
#define CXP_I2C_REG_ADRS_MIN				(0x00)
#define CXP_I2C_REG_ADRS_MAX				(0xffff)
#endif


//**********************************************************************************
//	CXP I2c Reg Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：アドレス
//		data				：データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpI2cRegWrite (unsigned int adrs, unsigned char data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char data8[3];
	unsigned int size;

	// Check adrs Parameter
	if ((adrs < CXP_I2C_REG_ADRS_MIN) || (adrs > CXP_I2C_REG_ADRS_MAX))
    {
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
    	sprintf (gLogMsgBuff, "CXP I2C Reg Write Address(0x%x) Parameter Error. Min : 0x%x / Max : 0x%x\n", adrs, CXP_I2C_REG_ADRS_MIN, CXP_I2C_REG_ADRS_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
    }

	// Write Adrs & Data
	data8[0] = (unsigned char)((adrs >> 8) & 0x000000FF);
	data8[1] = (unsigned char)(adrs & 0x000000FF);
	data8[2] = (unsigned char)data;
	size = 3;

	if ((status = i2cWrite (CXP_I2C_PORT, CXP_I2C_SLAVE_ADRS, adrs, data8, size)) != AVAL_STATUS_SUCCESS)
    {
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
    	sprintf (gLogMsgBuff, "CXP I2C Reg Write Address Error. Write Adrs = 0x%x\n", adrs);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
    }

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP I2c Reg  Read
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：アドレス
//		*pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpI2cRegRead (unsigned int adrs, unsigned char *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	//unsigned char data8;
	unsigned char data8A[2];

	// Check adrs Parameter
	if ((adrs < CXP_I2C_REG_ADRS_MIN) || (adrs > CXP_I2C_REG_ADRS_MAX))
    {
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
    	sprintf (gLogMsgBuff, "CXP I2C Reg Read Address(0x%x) Parameter Error. Min : 0x%x / Max : 0x%x\n", adrs, CXP_I2C_REG_ADRS_MIN, CXP_I2C_REG_ADRS_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
    }

	// Check pData Parameter
	if (pData == NULL)
    {
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP I2C Reg Read pData NULL Parameter Error.\n");
		goto _DONE;
    }

	// Write Adrs
	data8A[0] = (unsigned char)((adrs >> 8) & 0x000000FF);
	data8A[1] = (unsigned char)(adrs & 0x000000FF);

	if ((status = i2cWrite (CXP_I2C_PORT, CXP_I2C_SLAVE_ADRS, adrs, data8A, 2)) != AVAL_STATUS_SUCCESS)
    {
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
    	sprintf (gLogMsgBuff, "CXP I2C Reg Read Address Error. Write Adrs = 0x%x\n", adrs);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
    }

	// Read Data
	if ((status = i2cRead (CXP_I2C_PORT, CXP_I2C_SLAVE_ADRS, adrs, data8A, 1)) != AVAL_STATUS_SUCCESS)
    {
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
    	sprintf (gLogMsgBuff, "CXP I2C Reg Read Data Error. Adrs = 0x%x\n", adrs);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
    }

	// データ格納
	*pData = data8A[0];

_DONE:
	return (status);
}

#endif // #if defined (MODE_I2C_PS)

// eof
