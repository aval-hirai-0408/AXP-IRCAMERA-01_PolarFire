//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// PolarFireFlash.c - PolarFire SPI Flash Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "../CXP/cxp.h"


#if	defined(MODE_FPGA_PF)

//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------

// デバイスID
#define SPI_DEVICE_ID				XPAR_XSPIPS_1_DEVICE_ID

// Flash Command
#define WRITE_STATUS_CMD			(0x01)
#define WRITE_CMD					(0x02)
#define READ_CMD					(0x03)
#define WRITE_DISABLE_CMD			(0x04)
#define READ_STATUS_CMD				(0x05)
#define WRITE_ENABLE_CMD			(0x06)
#define FAST_READ_CMD				(0x0B)
#define DUAL_READ_CMD				(0x3B)
#define QUAD_READ_CMD				(0x6B)
#define BULK_ERASE_CMD				(0xC7)
//#define	SEC_ERASE_CMD			(0xD8)	// Block(64k)
#define	SEC_ERASE_CMD				(0x20)	// 4k
#define	BLOCK_ERASE_CMD				(0xD8)	// 64k
#define READ_ID						(0x9F)

// Flash Command Offset
#define COMMAND_OFFSET				(0)		/* FLASH instruction */
#define ADDRESS_1_OFFSET			(1)		/* MSB byte of address to read or write */
#define ADDRESS_2_OFFSET			(2)		/* Middle byte of address to read or write */
#define ADDRESS_3_OFFSET			(3)		/* LSB byte of address to read or write */
#define DATA_OFFSET					(4)		/* Start of Data for Read/Write */
#define DUMMY_OFFSET				(4)		/* Dummy byte offset for fast, dual and quad reads */
#define DUMMY_SIZE					(1)		/* Number of dummy bytes for fast, dual and quad reads */
#define RD_ID_SIZE					(4)		/* Read ID command + 3 bytes ID response */
#define BULK_ERASE_SIZE				(1)		/* Bulk Erase command size */
#define SEC_ERASE_SIZE				(4)		/* Sector Erase command + Sector address */

#define OVERHEAD_SIZE				(4)
#define WREAR_SIZE					(2)

// SPI Select
#define SPI_FLASH_SELECT			(0x00)

// Interrupt Timeout
#define SPI_FLASH_INT_TIMEOUT		(10000)		//10s

// Status Check Timeout
#define SPI_FLASH_STATUS_COUNT		(10000)		//10s

// bulk Erase Status Check Timeout
#define SPI_FLASH_BULK_STATUS_COUNT	(80000)		//80s

// Buffer Size
#define SPI_FLASH_READ_BUFF_SIZE	(4*1024*1024)
#define SPI_FLASH_WRITE_BUFF_SIZE 	(4*1024*1024)

// Page Size
#define SPI_FLASH_PAGE_SIZE			(256)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// SPIインスタンス
//@@@1static XSpiPs PolarFireFlashInstance;

// Buffer
unsigned char *PolarFireFlashReadBuffer = NULL;
unsigned char *PolarFireFlashWriteBuffer = NULL;
unsigned char *PolarFireFlashReadBufferORG = NULL;
unsigned char *PolarFireFlashWriteBufferORG = NULL;


//----------------------------------------------------------------------------------
// function define
//----------------------------------------------------------------------------------

//**********************************************************************************
//	初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int PolarFireFlashInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0//@@@1

	XSpiPs_Config *SpiConfig;

	// spiドライバ初期化
	if ((SpiConfig = XSpiPs_LookupConfig (SPI_DEVICE_ID)) == NULL)
	{
		DEBUG_PRINT ("SPI Flash XSpiPs_LookupConfig Error\n");
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO));
	}

	// spi初期化
	if ((status = XSpiPs_CfgInitialize (&PolarFireFlashInstance, SpiConfig, SpiConfig->BaseAddress)) != XST_SUCCESS)
	{
		DEBUG_PRINT ("SPI Flash XSpiPs_CfgInitialize Error\n");
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO));
	}

	// オプション設定
	XSpiPs_SetOptions (&PolarFireFlashInstance, (XSPIPS_MANUAL_START_OPTION | XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION));

	// クロック設定
	XSpiPs_SetClkPrescaler (&PolarFireFlashInstance, XSPIPS_CLK_PRESCALE_64);

	// Assert the FLASH chip select
	XSpiPs_SetSlaveSelect (&PolarFireFlashInstance, SPI_FLASH_SELECT);

	// 受信用バッファ確保
	if (PolarFireFlashReadBufferORG == NULL)
	{
		if ((PolarFireFlashReadBufferORG = malloc (SPI_FLASH_READ_BUFF_SIZE + MALLOC_ALIGN)) == NULL)
		{
			DEBUG_PRINT ("SPI Flash Read Buffer Reqest Error\n");
			return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_RESOURCE_EXHAUSTED));
		}

		// Align調整
		if (((unsigned int)PolarFireFlashReadBufferORG & ALIGN_MASK_16Byte) != 0)
		{
			PolarFireFlashReadBuffer = (unsigned char *)((unsigned int)PolarFireFlashReadBufferORG & ~ALIGN_MASK_16Byte) + (ALIGN_MASK_16Byte + 1);
		}
		else
		{
			PolarFireFlashReadBuffer = PolarFireFlashReadBufferORG;
		}
	}

	// 送信用バッファ確保
	if (PolarFireFlashWriteBufferORG == NULL)
	{
		if ((PolarFireFlashWriteBufferORG = malloc (SPI_FLASH_WRITE_BUFF_SIZE + MALLOC_ALIGN)) == NULL)
		{
			DEBUG_PRINT ("SPI Flash Write Buffer Reqest Error\n");
			return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_RESOURCE_EXHAUSTED));
		}

		// Align調整
		if (((unsigned int)PolarFireFlashWriteBufferORG & ALIGN_MASK_16Byte) != 0)
		{
			PolarFireFlashWriteBuffer = (unsigned char *)((unsigned int)PolarFireFlashWriteBufferORG & ~ALIGN_MASK_16Byte) + (ALIGN_MASK_16Byte + 1);
		}
		else
		{
			PolarFireFlashWriteBuffer = PolarFireFlashWriteBufferORG;
		}
	}

	//PolarFire SPIアクセスモード設定
	//if((status = cxpRegWrite (CXP_REG_SPI_ADRS, CXP_REG_SPI_ACCSESS_ENB, 1)) != AVAL_STATUS_SUCCESS)
		//PolarFireFlashFinalize();
	
#if 0
	//if((status = cxpRegWrite (CXP_REG_SPI_ADRS, CXP_REG_SPI_ACCSESS_ENB, 1)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	PolarFireFlashWriteBuffer[0] = READ_ID;
	PolarFireFlashWriteBuffer[1] = 0;
	PolarFireFlashWriteBuffer[2] = 0;
	PolarFireFlashWriteBuffer[3] = 0;

	// コマンド発行
	if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, PolarFireFlashWriteBuffer, PolarFireFlashReadBuffer, 4) != XST_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Read Command Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
		goto _DONE;;
	}

	DEBUG_PRINT_FORCE("@0:0x%x\n", PolarFireFlashReadBuffer[0]);
	DEBUG_PRINT_FORCE("@1:0x%x\n", PolarFireFlashReadBuffer[1]);
	DEBUG_PRINT_FORCE("@2:0x%x\n", PolarFireFlashReadBuffer[2]);
	DEBUG_PRINT_FORCE("@3:0x%x\n", PolarFireFlashReadBuffer[3]);
_DONE:
#endif
	
#endif ////@@@1

	return (status);
}


//**********************************************************************************
//	終了
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int PolarFireFlashFinalize (void)
{
	if (PolarFireFlashReadBufferORG != NULL)
		free (PolarFireFlashReadBufferORG);
	PolarFireFlashReadBufferORG = NULL;

	if (PolarFireFlashWriteBufferORG != NULL)
		free (PolarFireFlashWriteBufferORG);
	PolarFireFlashWriteBufferORG = NULL;

	//PolarFire SPIアクセスモード解除
	//cxpRegWrite (CXP_REG_SPI_ADRS, CXP_REG_SPI_ACCSESS_DIS, 1);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	SPI Enable Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				: 0=Disable/1=Enable
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int PolarFireSpiMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

#if defined (MODE_BOARD_ACB525CXP)
	unsigned int spiMode;

	if (mode == MODE_ENABLE)
		spiMode = CXP_REG_SPI_ACCSESS_ENB;
	else
		spiMode = CXP_REG_SPI_ACCSESS_DIS;

	//PolarFire SPIアクセスモード設定
	if((status = cxpRegWrite (CXP_REG_SPI_ADRS, spiMode, 1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
#endif
	return (status);
}


//**********************************************************************************
//	Flash書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：Flashアドレス
//		pBuffer					：Flashへ書き込むデータを格納するポインタ
//		size					：書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int PolarFireFlashWrite (unsigned int adrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0//@@@1
	u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
	u8 WriteDisableCmd = { WRITE_DISABLE_CMD };
	unsigned int Index;
	unsigned int transed, transSize;
	unsigned int *srcPtrL, *desPtrL;
	unsigned char *srcPtrB, *desPtrB;
	unsigned int transSizeL, transSizeB;
	unsigned int alignAdrs, alignSize;

	// Check adrs Parameter
	if (adrs >= PF_SPI_FLASH_SIZE)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Address(0x%x) Parameter Error. (Min=0x0 / Max=0x%x)\n", adrs, PF_SPI_FLASH_SIZE);
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_INVALID_PARAMETER));
	}

	// Check size Parameter
	if (size > PF_SPI_FLASH_SIZE)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Size(0x%x) Parameter Error. (Min=0x0 / Max=0x%x)\n", size, PF_SPI_FLASH_SIZE);
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_INVALID_PARAMETER));
	}

	// Check adrs + size Parameter
	if ((adrs + size) > PF_SPI_FLASH_SIZE)
	{
		DEBUG_PRINT_FORCE ("SPI Flash adrs(0x%x) + size(0x%x) Parameter Error. (Min=0x1 / Max=0x%x)\n", adrs, size, PF_SPI_FLASH_SIZE);
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_INVALID_PARAMETER));
	}

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		DEBUG_PRINT_FORCE ("SPI Flash pBuffer NULL Parameter Error.\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_INVALID_PARAMETER));
	}

	transSize = 0;
	for (transed=0; transed<size; transed+=transSize, adrs+=transSize)
	{
		//------------------------------------------------------------
		// Write Enableコマンド発行
		//------------------------------------------------------------

		// ステータスCheck
		if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
			DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
			goto _DONE;;
		}

		// コマンド発行
		if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd)) != XST_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("SPI Flash Write Enable Command Error\n");
			DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
			goto _DONE;;
		}

		//------------------------------------------------------------
		// Writeコマンド発行
		//------------------------------------------------------------

		// ステータスCheck
		if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
			DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
			goto _DONE;;
		}

		// Flash Write Command設定
		PolarFireFlashWriteBuffer[COMMAND_OFFSET]   = WRITE_CMD;
		PolarFireFlashWriteBuffer[ADDRESS_1_OFFSET] = (u8)((adrs & 0xFF0000) >> 16);
		PolarFireFlashWriteBuffer[ADDRESS_2_OFFSET] = (u8)((adrs & 0xFF00) >> 8);
		PolarFireFlashWriteBuffer[ADDRESS_3_OFFSET] = (u8)(adrs & 0xFF);

		// Write転送サイズ設定
		transSize = size - transed;
		if (transSize > SPI_FLASH_PAGE_SIZE)
			transSize = SPI_FLASH_PAGE_SIZE;

		// スタートアドレスの確認
		if (adrs & (SPI_FLASH_PAGE_SIZE-1))
		{
			// SPI_FLASH_PAGE_SIZEアライン分のサイズのみを転送する
			alignAdrs = adrs & (SPI_FLASH_PAGE_SIZE-1);
			alignSize = SPI_FLASH_PAGE_SIZE - alignAdrs;
			
			if (transSize > alignSize)
				transSize = alignSize;
		}

		// intでデータコピーする為の計算
		transSizeL = transSize / 4;

		// 転送元/転送先アドレス
		srcPtrL = (unsigned int *)&pBuffer[transed];
		desPtrL = (unsigned int *)&PolarFireFlashWriteBuffer[DATA_OFFSET];

		// データコピー
		for (Index=0; Index<transSizeL; Index++, srcPtrL++, desPtrL++)
			*desPtrL = *srcPtrL;

		// 余りあり？
		transSizeB = transSize%4;
		if (transSizeB != 0)
		{
			// 転送元/転送先アドレス
			srcPtrB = (unsigned char *)srcPtrL;
			desPtrB = (unsigned char *)desPtrL;
			
			// データコピー
			for (Index=0; Index<transSizeB; Index++, srcPtrB++, desPtrB++)
				*desPtrB = *srcPtrB;
		}

		// コマンド発行
		if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, PolarFireFlashWriteBuffer, NULL, transSize + OVERHEAD_SIZE) != XST_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("SPI Flash Write Command Error\n");
			DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
			goto _DONE;;
		}


		//------------------------------------------------------------
		// ステータスCheck
		//------------------------------------------------------------
		if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
			DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
			goto _DONE;;
		}

		//------------------------------------------------------------
		// Write Disableコマンド発行
		//------------------------------------------------------------

		// コマンド発行
		if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, &WriteDisableCmd, NULL, sizeof(WriteDisableCmd)) != XST_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("SPI Flash Write Disable Command Error\n");
			DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
			goto _DONE;;
		}
	}

_DONE:
#endif//@@@1

	return (status);
}


//**********************************************************************************
//	Flash読み込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：Flashアドレス
//		pBuffer					：Flashへ書き込むデータを格納するポインタ
//		size					：書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int PolarFireFlashRead (unsigned int adrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0//@@@1

	unsigned int Index;
	unsigned char Command;
	unsigned int transSizeL, transSizeB;
	unsigned int transed, transSize;
	unsigned int *srcPtrL, *desPtrL;
	unsigned char *srcPtrB, *desPtrB;
	unsigned int alignAdrs, alignSize;
	unsigned int offset;

	// Check adrs Parameter
	if (adrs >= PF_SPI_FLASH_SIZE)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Address(0x%x) Parameter Error. (Min=0x0 / Max=0x%x)\n", adrs, PF_SPI_FLASH_SIZE);
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_INVALID_PARAMETER));
	}

	// Check size Parameter
	if (size > PF_SPI_FLASH_SIZE)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Size(0x%x) Parameter Error. (Min=0x0 / Max=0x%x)\n", size, PF_SPI_FLASH_SIZE);
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_INVALID_PARAMETER));
	}

	// Check adrs + size Parameter
	if ((adrs + size) > PF_SPI_FLASH_SIZE)
	{
		DEBUG_PRINT_FORCE ("SPI Flash adrs(0x%x) + size(0x%x) Parameter Error. (Min=0x1 / Max=0x%x)\n", adrs, size, PF_SPI_FLASH_SIZE);
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_INVALID_PARAMETER));
	}

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		DEBUG_PRINT_FORCE ("SPI Flash pBuffer NULL Parameter Error.\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_INVALID_PARAMETER));
	}

	for (transed=0; transed<size; transed+=transSize, adrs+=transSize)
	{
		// サイズCheck(最大4Mbyte)
		if (size > SPI_FLASH_READ_BUFF_SIZE)
			transSize = SPI_FLASH_READ_BUFF_SIZE;
		else
			transSize = size - transed;

		// スタートアドレスの確認
		if (adrs & (SPI_FLASH_READ_BUFF_SIZE-1))
		{
			// SPI_FLASH_READ_BUFF_SIZEアライン分のサイズのみを転送する
			alignAdrs = adrs & (SPI_FLASH_READ_BUFF_SIZE-1);
			alignSize = SPI_FLASH_READ_BUFF_SIZE - alignAdrs;
			
			if (transSize > alignSize)
				transSize = alignSize;
		}

		//------------------------------------------------------------
		// Readコマンド発行
		//------------------------------------------------------------
		// READ_CMD
		// FAST_READ_CMD
		// DUAL_READ_CMD
		//------------------------------------------------------------

		// ステータスCheck
		if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
			DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
			goto _DONE;
		}

		Command = FAST_READ_CMD;
		PolarFireFlashWriteBuffer[COMMAND_OFFSET]   = Command;
		PolarFireFlashWriteBuffer[ADDRESS_1_OFFSET] = (u8)((adrs & 0xFF0000) >> 16);
		PolarFireFlashWriteBuffer[ADDRESS_2_OFFSET] = (u8)((adrs & 0xFF00) >> 8);
		PolarFireFlashWriteBuffer[ADDRESS_3_OFFSET] = (u8)(adrs & 0xFF);

		// ダミーサイズ付加
		if ((Command == FAST_READ_CMD) || (Command == DUAL_READ_CMD) || (Command == QUAD_READ_CMD))
		{
			transSize += DUMMY_SIZE;
		}

		// コマンド発行
		if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, PolarFireFlashWriteBuffer, PolarFireFlashReadBuffer, transSize + OVERHEAD_SIZE) != XST_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("SPI Flash Read Command Error\n");
			DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
			goto _DONE;;
		}

		//------------------------------------------------------------
		// ReadデータCopy
		//------------------------------------------------------------

		// ダミーサイズ調整
		if ((Command == FAST_READ_CMD) || (Command == DUAL_READ_CMD) || (Command == QUAD_READ_CMD))
		{
			transSize -= DUMMY_SIZE;
			offset = DATA_OFFSET + DUMMY_SIZE;
		}
		else
		{
			offset = DATA_OFFSET;
		}

		// intでデータコピーする為の計算
		transSizeL = transSize / 4;

		// 転送元/転送先アドレス
		srcPtrL = (unsigned int *)&PolarFireFlashReadBuffer[offset];
		desPtrL = (unsigned int *)&pBuffer[transed];

		// データコピー
		for (Index=0; Index<transSizeL; Index++, srcPtrL++, desPtrL++)
			*desPtrL = *srcPtrL;

		// 余りあり？
		transSizeB = transSize%4;
		if (transSizeB != 0)
		{
			// 転送元/転送先アドレス
			srcPtrB = (unsigned char *)srcPtrL;
			desPtrB = (unsigned char *)desPtrL;
			
			// データコピー
			for (Index=0; Index<transSizeB; Index++, srcPtrB++, desPtrB++)
				*desPtrB = *srcPtrB;
		}
	}

_DONE:
#endif//@@@1

	return (status);
}


//**********************************************************************************
//	Flash Sectorイレーズ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：Flashアドレス
//		pBuffer					：Flashへ書き込むデータを格納するポインタ
//		size					：書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int PolarFireFlashSectorErase (unsigned int adrs)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0//@@@1

	u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
	u8 WriteDisableCmd = { WRITE_DISABLE_CMD };

	// Check adrs Parameter
	if (adrs >= PF_SPI_FLASH_SIZE)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Address(0x%x) Parameter Error. (Min=0x0 / Max=0x%x)\n", adrs, PF_SPI_FLASH_SIZE);
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_INVALID_PARAMETER));
	}

	//------------------------------------------------------------
	// Write Enableコマンド発行
	//------------------------------------------------------------

	// ステータスCheck
	if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		return (status);
	}

	// コマンド発行
	if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd)) != XST_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Write Enable Command Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO));
	}

	//------------------------------------------------------------
	// セクタイレーズコマンド発行
	//------------------------------------------------------------

	// ステータスCheck
	if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		goto _DONE;
	}

	// コマンド設定
	PolarFireFlashWriteBuffer[COMMAND_OFFSET]   = SEC_ERASE_CMD;
	PolarFireFlashWriteBuffer[ADDRESS_1_OFFSET] = (u8)(adrs >> 16);
	PolarFireFlashWriteBuffer[ADDRESS_2_OFFSET] = (u8)(adrs >> 8);
	PolarFireFlashWriteBuffer[ADDRESS_3_OFFSET] = (u8)(adrs & 0xFF);

	// コマンド発行
	if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, PolarFireFlashWriteBuffer, NULL, SEC_ERASE_SIZE) != XST_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Erase Command Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
		goto _DONE;
	}

	// ステータスCheck
	if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		goto _DONE;
	}

	//------------------------------------------------------------
	// Write Disableコマンド発行
	//------------------------------------------------------------

	// コマンド発行
	if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, &WriteDisableCmd, NULL, sizeof(WriteDisableCmd)) != XST_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Write Disable Command Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
		goto _DONE;
	}

_DONE:
#endif//@@@1
	return (status);
}


//**********************************************************************************
//	Flash Bloackイレーズ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：Flashアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int PolarFireFlashBlockErase (unsigned int adrs)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0//@@@1
	u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
	u8 WriteDisableCmd = { WRITE_DISABLE_CMD };

	// Check adrs Parameter
	if (adrs >= PF_SPI_FLASH_SIZE)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Address(0x%x) Parameter Error. (Min=0x0 / Max=0x%x)\n", adrs, PF_SPI_FLASH_SIZE);
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_INVALID_PARAMETER));
	}

	//------------------------------------------------------------
	// Write Enableコマンド発行
	//------------------------------------------------------------

	// ステータスCheck
	if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		return (status);
	}

	// コマンド発行
	if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd)) != XST_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Write Enable Command Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO));
	}

	//------------------------------------------------------------
	// ブロックイレーズコマンド発行
	//------------------------------------------------------------

	// ステータスCheck
	if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		goto _DONE;
	}

	// コマンド設定
	PolarFireFlashWriteBuffer[COMMAND_OFFSET]   = BLOCK_ERASE_CMD;
	PolarFireFlashWriteBuffer[ADDRESS_1_OFFSET] = (u8)(adrs >> 16);
	PolarFireFlashWriteBuffer[ADDRESS_2_OFFSET] = (u8)(adrs >> 8);
	PolarFireFlashWriteBuffer[ADDRESS_3_OFFSET] = (u8)(adrs & 0xFF);

	// コマンド発行
	if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, PolarFireFlashWriteBuffer, NULL, SEC_ERASE_SIZE) != XST_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Erase Command Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
		goto _DONE;
	}

	// ステータスCheck
	if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		goto _DONE;
	}

	//------------------------------------------------------------
	// Write Disableコマンド発行
	//------------------------------------------------------------

	// コマンド発行
	if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, &WriteDisableCmd, NULL, sizeof(WriteDisableCmd)) != XST_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Write Disable Command Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
		goto _DONE;
	}

_DONE:
#endif//@@@1

	return (status);
}



//**********************************************************************************
//	Flashイレーズ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外				：異常終了
//==================================================================================
int PolarFireFlashEraseAll (void)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0//@@@1
	u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
	u8 WriteDisableCmd = { WRITE_DISABLE_CMD };
	u8 ReadStatusCmd[] = { READ_STATUS_CMD, 0 };  /* must send 2 bytes */
	u8 FlashStatus[2];
	unsigned int retry;

	//------------------------------------------------------------
	// Write Enableコマンド発行
	//------------------------------------------------------------

	// ステータスCheck
	if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		return (status);
	}

	// コマンド発行
	if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd)) != XST_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Write Enable Command Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		return (MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO));
	}

	//------------------------------------------------------------
	// イレーズコマンド発行
	//------------------------------------------------------------

	// ステータスCheck
	if ((status = PolarFireFlashStatusCheck ()) != AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Status Check Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		goto _DONE;
	}

	PolarFireFlashWriteBuffer[COMMAND_OFFSET] = BULK_ERASE_CMD;

	// コマンド発行
	if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, PolarFireFlashWriteBuffer, NULL, BULK_ERASE_SIZE) != XST_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Erase Enable Command Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
		goto _DONE;
	}

	//------------------------------------------------------------
	// ステータスCheck
	//------------------------------------------------------------
	for (retry=0; retry<SPI_FLASH_BULK_STATUS_COUNT; retry++)
	{
		// コマンド発行
		if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd)) != XST_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("SPI Flash Read Status Command Error\n");
			DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
			goto _DONE;
		}

		// ステータスCheck
		if ((FlashStatus[1] & 0x01) == 0)
			break;

		msDelay(1);
	}

	// Check Timeout
	if (retry >= SPI_FLASH_BULK_STATUS_COUNT)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Erase Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_TIMEOUT);
		goto _DONE;
	}

	//------------------------------------------------------------
	// Write Disableコマンド発行
	//------------------------------------------------------------

	// コマンド発行
	if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, &WriteDisableCmd, NULL, sizeof(WriteDisableCmd)) != XST_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Write Disable Command Error\n");
		DEBUG_PRINT_FORCE (CMD_ERROR_DEVICE);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
		goto _DONE;
	}

_DONE:
#endif//@@@1

	return (status);
}


//**********************************************************************************
//	ステータスCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int PolarFireFlashStatusCheck (void)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0//@@@1
	unsigned int retry;
	u8 ReadStatusCmd[] = { READ_STATUS_CMD, 0 };  /* must send 2 bytes */
	u8 FlashStatus[2];

	//------------------------------------------------------------
	// ステータスCheck
	//------------------------------------------------------------
	for (retry=0; retry<SPI_FLASH_STATUS_COUNT; retry++)
	{
		// コマンド発行
		if (XSpiPs_PolledTransfer (&PolarFireFlashInstance, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd)) != XST_SUCCESS)
		{
			DEBUG_PRINT_FORCE ("SPI Flash Read Status Command Error\n");
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_IO);
			goto _DONE;
		}

		// ステータスCheck
		if ((FlashStatus[1] & 0x01) == 0)
			break;

		msDelay(1);
	}

	// Check Timeout
	if (retry >= SPI_FLASH_STATUS_COUNT)
	{
		DEBUG_PRINT_FORCE ("SPI Flash Read Status Timeout Error\n");
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SPI_FLASH, AVAL_STATUS_TIMEOUT);
		goto _DONE;
	}
		
_DONE:
#endif//@@@1
	return (status);
}
#endif

// eof
