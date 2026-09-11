//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// gigeCmdDiag.c - GigE Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "../GigE/gev.h"


//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#if defined (MODE_GIGE_10G)
#if 0 //@@@1
#include "../mtd/mtdFeatures.h"
#include "../mtd/mtdApiTypes.h"
#include "../mtd/mtdHwCntl.h"
#include "../mtd/mtdDiagnostics.h"
#endif //@@@1
#endif

//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
//@@@1extern MTD_DEV mtd_dev;



#if !defined (MODE_GIGE_10G)
#define PHY_ACCES_PHY_MASK		(0x1f)
#define PHY_ACCES_PHY_SHIFT		(7)
#define PHY_ACCES_REG_MASK		(0x1f)
#define PHY_ACCES_REG_SHIFT		(2)
#define PHY_NUMBER				(0)
#define PHY_FORMAT_BIT			(1<<12)

//**********************************************************************************
//	Phy Read Acess
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：レジスタアドレス
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int phyReadReg (unsigned int adrs, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int phy, reg;
	unsigned int phyAcesFormat;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_GIGE, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Phy Read pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// Phy
	phy = PHY_NUMBER & PHY_ACCES_PHY_MASK;
	phy <<= PHY_ACCES_PHY_SHIFT;

	// Register
	reg = adrs & PHY_ACCES_REG_MASK;
	reg <<= PHY_ACCES_REG_SHIFT;

	// Phy Aces Format
	phyAcesFormat = XPAR_EPC_0_PRH0_BASEADDR | PHY_FORMAT_BIT | phy | reg;

	// データ取得
	*pData = (unsigned int)IN16 (phyAcesFormat);

_DONE:
	return (status);
}


//**********************************************************************************
//	Phy Write Acess
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：レジスタアドレス
//		data				：データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int phyWriteReg (unsigned int adrs, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int phy, reg;
	unsigned int phyAcesFormat;

	// Phy
	phy = PHY_NUMBER & PHY_ACCES_PHY_MASK;
	phy <<= PHY_ACCES_PHY_SHIFT;

	// Register
	reg = adrs & PHY_ACCES_REG_MASK;
	reg <<= PHY_ACCES_REG_SHIFT;

	// Phy Aces Format
	phyAcesFormat = XPAR_EPC_0_PRH0_BASEADDR | PHY_FORMAT_BIT | phy | reg;

	// データ設定
	OUT16 (phyAcesFormat, (unsigned short)data);

	return (status);
}

#else // #if !defined (MODE_GIGE_10G)

//**********************************************************************************
//	Phy Read Acess
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dev					：デバイス番号
//		adrs				：レジスタアドレス
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int phyReadReg (unsigned int dev, unsigned int adrs, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0	//@@@1
	MTD_STATUS mtdStatus;
	unsigned short data16;
	int port = 0;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_GIGE, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Phy Read pData NULL Parameter Error.\n");
		goto _DONE;
	}
		
	// Read
	if ((mtdStatus = mtdHwXmdioRead (&mtd_dev, port, dev, adrs, &data16)) != MTD_OK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_GIGE, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Phy Read Error.\n");
		goto _DONE;
	}


	// Read Data
	*pData = (unsigned int)data16;

_DONE:
#endif //@@@1
	return (status);
}


//**********************************************************************************
//	Phy Write Acess
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dev					：デバイス番号
//		adrs				：レジスタアドレス
//		data				：データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int phyWriteReg (unsigned int dev, unsigned int adrs, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0	//@@@1
	MTD_STATUS mtdStatus;
	unsigned short data16;
	int port = 0;
	
	data16 = (unsigned short)data;

	// Write
	if ((mtdStatus = mtdHwXmdioWrite (&mtd_dev, port, dev, adrs, data16)) != MTD_OK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_GIGE, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Phy Wrie Error.\n");
		goto _DONE;
	}

_DONE:
#endif //@@@1
	return (status);
}


//**********************************************************************************
//	Phy Get Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		speed				：LAN Speed
//		selector			：Tx/Rx/Err
//		pData				：Byteカウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int phyGetCount (unsigned int speed, unsigned int selector, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0 //@@@1
	int port = 0;
	unsigned long long packetCount, byteCount;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_GIGE, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Phy Count NULL Parameter Error.\n");
		goto _DONE;
	}


	// Read Data
	*pData = IN32 (GIGE_FIRM_CMD_DATA3);

	// Get Phy Count
	if ((mtdStatus = mtdTunitPktGeneratorGetCounter (&mtd_dev, port, speed, selector, &packetCount, &byteCount)) != MTD_OK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_GIGE, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Phy Get Count Error.\n");
		goto _DONE;
	}

	*pData = (unsigned int)byteCount;
	
_DONE:
#endif //@@@1
	return (status);
}
#endif // #if !defined (MODE_GIGE_10G)

// eof
