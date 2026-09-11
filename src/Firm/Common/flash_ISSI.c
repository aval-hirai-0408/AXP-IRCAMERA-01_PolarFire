//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// flash_ISSI.c - Flash Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "flash.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------

// Write Protect
//@@@1#define QSPI_FLASH_WRITE_PROTECT_SUPPORT_ISSI

// Status Check Timeout
#define QSPI_FLASH_STATUS_COUNT_ISSI	(10000)		//10s

// Bulk Erase Status Check Timeout
#define QSPI_FLASH_BULK_STATUS_COUNT_ISSI (250000)	//250s

// Bank Size
#define QSPI_FLASH_BANK_SIZE_ISSI		(16*1024*1024)

// Page Size
#define QSPI_FLASH_PAGE_SIZE_ISSI		(256)

// Read Size
#define QSPI_FLASH_READ_SIZE_ISSI		(32*1024)

// Flash Command
#define QSPI_WRITE_STATUS_CMD_ISSI		(0x01)
#define QSPI_WRITE_CMD_ISSI				(0x02)
#define QSPI_READ_CMD_ISSI				(0x03)
#define QSPI_WRITE_DISABLE_CMD_ISSI		(0x04)
#define QSPI_READ_STATUS_CMD_ISSI		(0x05)
#define QSPI_WRITE_ENABLE_CMD_ISSI		(0x06)
#define QSPI_FAST_QSPI_READ_CMD_ISSI	(0x0B)
#define QSPI_DUAL_QSPI_READ_CMD_ISSI	(0x3B)
#define QUAD_QSPI_READ_CMD_ISSI			(0x6B)
#define QUAD_QSPI_WRITE_CMD_ISSI		(0x32)
#define QSPI_BULK_ERASE_CMD_ISSI		(0xC7)
#define	QSPI_SEC_ERASE_CMD_ISSI			(0xD8)
#define QSPI_READ_ID_ISSI				(0x9F)
#define QSPI_WREAR_CMD_ISSI				(0xC5)
#define QSPI_READ_FLAG_STATUS_CMD_ISSI	(0x70)
#define QSPI_EXIT_4BYTE_ADRS_CMD_ISSI	(0xE9)

// Write Protect
#define QSPI_STATUS_TOP_BOTTOM_ISSI		(1<<5)		// Top or Bottom
#define QSPI_STATUS_PROTECT3_ISSI		(1<<6)		// bit3
#define QSPI_STATUS_PROTECT2_ISSI		(1<<4)		// bit2
#define QSPI_STATUS_PROTECT1_ISSI		(1<<3)		// bit1
#define QSPI_STATUS_PROTECT0_SHIFT_ISSI	(1<<2)		// bit0
#define QSPI_STATUS_PROTECT012_SHIFT_ISSI	(2)
#define QSPI_STATUS_DATA3_ISSI			(1<<3)
#define QSPI_STATUS_DATA012_ISSI		(0x07)
#define QSPI_STATUS_MASK_ISSI			(QSPI_STATUS_TOP_BOTTOM_ISSI | QSPI_STATUS_PROTECT3_ISSI | QSPI_STATUS_PROTECT2_ISSI | QSPI_STATUS_PROTECT1_ISSI | QSPI_STATUS_PROTECT0_SHIFT_ISSI)

// Sector Min/Max
#define QSPI_SECTOR_MIN_ISSI			(0)
#define QSPI_SECTOR_MAX_ISSI			(512)
#define QSPI_TOP_BOTTOM_SECTOR_ISSI		(256)

// Bank Default
#define QSPI_FLASH_BANK_DEFAULT_ISSI	(0xffffffff)


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------

// QSPI Flash Select
extern int gQspiFlashSelect;

// Instance
extern spi_instance_t g_flash_core_spi; 

// Write Buffer
extern unsigned char QspiFlashWriteBuffer[];


//**********************************************************************************
//	初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashInitialize_ISSI (void)
{
	int status = AVAL_STATUS_SUCCESS;
	
	//------------------------------------------------------------
	// Slave Select   
	//------------------------------------------------------------
    SPI_set_slave_select (&g_flash_core_spi, SPI_SLAVE);

	//------------------------------------------------------------
	// Bank設定
	//------------------------------------------------------------
	if ((status = qspiFlashSetBank (QSPI_FLASH_BANK_DEFAULT_ISSI)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Write Protect
	//------------------------------------------------------------
#ifdef QSPI_FLASH_WRITE_PROTECT_SUPPORT_ISSI
	if ((status = qspiFlashWriteProtect_ISSI (-1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//------------------------------------------------------------
	// Write Enableコマンド発行
	//------------------------------------------------------------
	if ((status = qspiFlashWriteEnableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// 3byteアドレスコマンド発行
	//------------------------------------------------------------
	//if ((status = qspiFlash3ByteAddresseCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// 拡張アドレスコマンド発行
	//------------------------------------------------------------
	if ((status = qspiFlashExAddresseCmd_ISSI (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Write Disableコマンド発行
	//------------------------------------------------------------
	if ((status = qspiFlashWriteDisableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	//------------------------------------------------------------
	// Clear Slave
	//------------------------------------------------------------
	SPI_clear_slave_select(&g_flash_core_spi, SPI_SLAVE);

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
int qspiFlashWrite_ISSI (unsigned int adrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int Index;
	unsigned int transed, transSize;
	unsigned int *srcPtrL, *desPtrL;
	unsigned char *srcPtrB, *desPtrB;
	unsigned int transSizeL, transSizeB;
	unsigned int alignAdrs, alignSize;
	unsigned int bank;
	unsigned char cmdBuff[4];
	unsigned int qspiFlashCurrentBank_ISSI;

	// Check adrs Parameter
	if (adrs >= QSPI_FLASH_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Flash Address(0x%x) Parameter Error.(Min=0x0 / Max=0x%x)\n", adrs, QSPI_FLASH_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	// Check size Parameter
	if (size > QSPI_FLASH_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Flash Write Size(0x%x) Parameter Error.(Min=0x0 / Max=0x%x)\n", size, QSPI_FLASH_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	// Check adrs + size Parameter
	if ((adrs + size) > QSPI_FLASH_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Flash Write Adrs(0x%x) + size(0x%x) Parameter Error.(Min=0x1 / Max=0x%x)\n", adrs, size, QSPI_FLASH_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Flash Write pBuffer NULL Parameter Error.\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	// Slave Select
    SPI_set_slave_select (&g_flash_core_spi, SPI_SLAVE);

	transSize = 0;
	for (transed=0; transed<size; transed+=transSize, adrs+=transSize)
	{
		// アクセスするバンクをCheck
		bank = (adrs & 0xFF000000) >> 24;

		// Bank取得
		if ((status = qspiFlashGetBank(&qspiFlashCurrentBank_ISSI)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 現在のバンクと同一?
		if (qspiFlashCurrentBank_ISSI != bank)
		{
			//------------------------------------------------------------
			// Write Enableコマンド発行
			//------------------------------------------------------------
			if ((status = qspiFlashWriteEnableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			//------------------------------------------------------------
			// 拡張アドレスコマンド発行
			//------------------------------------------------------------
			if ((status = qspiFlashExAddresseCmd_ISSI (adrs)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		#ifdef QSPI_FLASH_WRITE_PROTECT_SUPPORT_ISSI
		//------------------------------------------------------------
		// Write Protect解除
		//------------------------------------------------------------
		if (((adrs%QSPI_FLASH_SEC_SIZE) == 0) || (transed == 0))
		{
			if ((status = qspiFlashWriteProtect_ISSI (adrs)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		#endif

		//------------------------------------------------------------
		// Write Enableコマンド発行
		//------------------------------------------------------------
		if ((status = qspiFlashWriteEnableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Write転送サイズ設定
		//------------------------------------------------------------
		transSize = size - transed;
		if (transSize > QSPI_FLASH_PAGE_SIZE_ISSI)
			transSize = QSPI_FLASH_PAGE_SIZE_ISSI;

		//------------------------------------------------------------
		// スタートアドレスの確認
		//------------------------------------------------------------
		if (adrs & (QSPI_FLASH_PAGE_SIZE_ISSI-1))
		{
			// QSPI_FLASH_PAGE_SIZE_ISSIアライン分のサイズのみを転送する
			alignAdrs = adrs & (QSPI_FLASH_PAGE_SIZE_ISSI-1);
			alignSize = QSPI_FLASH_PAGE_SIZE_ISSI - alignAdrs;
			
			if (transSize > alignSize)
				transSize = alignSize;
		}

		//------------------------------------------------------------
		// ステータスCheck
		//------------------------------------------------------------
		if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Wait Ready Check
		//------------------------------------------------------------
		//if ((status = qspiFlashWaitReadyEraseCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		//------------------------------------------------------------
		// Writeコマンド発行
		//------------------------------------------------------------
		QspiFlashWriteBuffer[0] = QSPI_WRITE_CMD_ISSI;				// Write Command
		//QspiFlashWriteBuffer[0] = QUAD_QSPI_WRITE_CMD_ISSI;		// Quad Write Command
		QspiFlashWriteBuffer[1] = (u8)((adrs >> 16) & 0xff);
		QspiFlashWriteBuffer[2] = (u8)((adrs >> 8) & 0xff);
		QspiFlashWriteBuffer[3] = (u8)(adrs & 0xFF);

		//------------------------------------------------------------
		// Writeデータ
		//------------------------------------------------------------
		// intでデータコピーする為の計算
		transSizeB = transSize;

		// 転送元/転送先アドレス
		srcPtrB = (unsigned char *)&pBuffer[transed];
		desPtrB = (unsigned char *)&QspiFlashWriteBuffer[4];

		// データコピー
		for (Index=0; Index<transSizeB; Index++, srcPtrB++, desPtrB++)
			*desPtrB = *srcPtrB;

		// Write
		SPI_transfer_block (&g_flash_core_spi, QspiFlashWriteBuffer, (transSize+4), 0, 0);    

		//------------------------------------------------------------
		// ステータスCheck
		//------------------------------------------------------------
		if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Write Disableコマンド発行
		//------------------------------------------------------------
		if ((status = qspiFlashWriteDisableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	//------------------------------------------------------------
	// Write Protectコマンド発行
	//------------------------------------------------------------
#ifdef QSPI_FLASH_WRITE_PROTECT_SUPPORT_ISSI
	qspiFlashWriteProtect_ISSI (-1);
#endif
	
	//------------------------------------------------------------
	// Write Disableコマンド発行
	//------------------------------------------------------------
	qspiFlashWriteDisableCmd_ISSI ();

	//------------------------------------------------------------
	// Slave Clear
	//------------------------------------------------------------
    SPI_clear_slave_select (&g_flash_core_spi, SPI_SLAVE);

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
int qspiFlashRead_ISSI (unsigned int adrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int Index;
	unsigned int transed, transSize;
	unsigned int alignAdrs, alignSize;
	unsigned int bank;
    unsigned char cmdBuff[6];
	unsigned int qspiFlashCurrentBank_ISSI;

	// Check adrs Parameter
	if (adrs >= QSPI_FLASH_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Flash Read Address(0x%x) Parameter Error.(Min=0x0 / Max=0x%x)\n", adrs, QSPI_FLASH_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	// Check size Parameter
	if (size > QSPI_FLASH_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Flash Read Size(0x%x) Parameter Error.(Min=0x0 / Max=0x%x)\n", size, QSPI_FLASH_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	// Check adrs + size Parameter
	if ((adrs + size) > QSPI_FLASH_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Flash Read Adrs(0x%x) + size(0x%x) Parameter Error.(Min=0x1 / Max=0x%x)\n", adrs, size, QSPI_FLASH_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Read pBuffer NULL Parameter Error.\n");
		return (status);
	}

	// Slave Select
    SPI_set_slave_select (&g_flash_core_spi, SPI_SLAVE);

	transSize = 0;
	for (transed=0; transed<size; transed+=transSize, adrs+=transSize)
	{
		// サイズCheck
		if (size > QSPI_FLASH_READ_SIZE_ISSI)
			transSize = QSPI_FLASH_READ_SIZE_ISSI;
		else
			transSize = size - transed;

		// スタートアドレスの確認
		if (adrs & (QSPI_FLASH_READ_SIZE_ISSI-1))
		{
			// QSPI_FLASH_BANK_SIZE_ISSIアライン分のサイズのみを転送する
			alignAdrs = adrs & (QSPI_FLASH_READ_SIZE_ISSI-1);
			alignSize = QSPI_FLASH_READ_SIZE_ISSI - alignAdrs;
			
			if (transSize > alignSize)
				transSize = alignSize;
		}

		// アクセスするバンクをCheck
		bank = (adrs & 0xFF000000) >> 24;

		// Bank取得
		if ((status = qspiFlashGetBank (&qspiFlashCurrentBank_ISSI)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 現在のバンクと同一?
		if (qspiFlashCurrentBank_ISSI != bank)
		{
			//------------------------------------------------------------
			// Write Enableコマンド発行
			//------------------------------------------------------------
			if ((status = qspiFlashWriteEnableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			//------------------------------------------------------------
			// 拡張アドレスコマンド発行
			//------------------------------------------------------------
			if ((status = qspiFlashExAddresseCmd_ISSI (adrs)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		//------------------------------------------------------------
		// Readコマンド発行
		//------------------------------------------------------------
		// QSPI_READ_CMD_ISSI
		// QSPI_FAST_QSPI_READ_CMD_ISSI
		// QSPI_DUAL_QSPI_READ_CMD_ISSI
		// QUAD_QSPI_READ_CMD_ISSI
		//------------------------------------------------------------

		cmdBuff[0] = QSPI_READ_CMD_ISSI;
		//cmdBuff[0] = QSPI_FAST_QSPI_READ_CMD_ISSI;
		//cmdBuff[0] = QUAD_QSPI_READ_CMD_ISSI;
	    cmdBuff[1] = (uint8_t)((adrs >> 16) & 0xFF);
	    cmdBuff[2] = (uint8_t)((adrs >> 8) & 0xFF);
	    cmdBuff[3] = (uint8_t)(adrs & 0xFF);
	    cmdBuff[4] = DONT_CARE;
	    cmdBuff[5] = DONT_CARE;
	    
		// Wait Redy Check
		//if ((status = qspiFlashWaitReadyEraseCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// Status Check
		if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Read
	    SPI_transfer_block (&g_flash_core_spi, cmdBuff, 4, &pBuffer[transed], transSize);

		// Status Check
		//if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;
	}

_DONE:
	//------------------------------------------------------------
	// Write Disableコマンド発行
	//------------------------------------------------------------
	qspiFlashWriteDisableCmd_ISSI ();

	//------------------------------------------------------------
	// Slave Clear
	//------------------------------------------------------------
    SPI_clear_slave_select (&g_flash_core_spi, SPI_SLAVE);

	return (status);
}


//**********************************************************************************
//	Flashイレーズ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：Flashアドレス
//		pBuffer					：Flashへ書き込むデータを格納するポインタ
//		size					：書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int qspiFlashSectorErase_ISSI (unsigned int adrs)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int bank;
	unsigned char cmdBuff[4];
	unsigned int qspiFlashCurrentBank_ISSI;

	// Check adrs Parameter
	if (adrs >= QSPI_FLASH_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Flash Erase Size(0x%x) Parameter Error.(Min=0x0 / Max=0x%x)\n", adrs, QSPI_FLASH_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	//------------------------------------------------------------
	// Slave Select
	//------------------------------------------------------------
	SPI_set_slave_select (&g_flash_core_spi, SPI_SLAVE);

	// アクセスするバンクをCheck
	bank = (adrs & 0xFF000000) >> 24;

	// Bank取得
	if ((status = qspiFlashGetBank (&qspiFlashCurrentBank_ISSI)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 現在のバンクと同一?
	if (qspiFlashCurrentBank_ISSI != bank)
	{
		//------------------------------------------------------------
		// Write Enableコマンド発行
		//------------------------------------------------------------
		if ((status = qspiFlashWriteEnableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// 拡張アドレスコマンド発行
		//------------------------------------------------------------
		if ((status = qspiFlashExAddresseCmd_ISSI (adrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//------------------------------------------------------------
	// Write Protect解除コマンド発行
	//------------------------------------------------------------
#ifdef QSPI_FLASH_WRITE_PROTECT_SUPPORT_ISSI
	if ((status = qspiFlashWriteProtect_ISSI (adrs))  != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//------------------------------------------------------------
	// Write Enableコマンド発行
	//------------------------------------------------------------
	if ((status = qspiFlashWriteEnableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// ステータスCheck
	//------------------------------------------------------------
	if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// セクタイレーズコマンド発行
	//------------------------------------------------------------
	cmdBuff[0] = QSPI_SEC_ERASE_CMD_ISSI;
	cmdBuff[1] = (u8)((adrs >> 16) & 0xff);
	cmdBuff[2] = (u8)((adrs >> 8) & 0xff);
	cmdBuff[3] = (u8)(adrs & 0xFF);

	//------------------------------------------------------------
	// Erase
	//------------------------------------------------------------
    SPI_transfer_block (&g_flash_core_spi, cmdBuff, sizeof(cmdBuff), 0, 0);

	//------------------------------------------------------------
	// ステータスCheck
	//------------------------------------------------------------
	if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	//------------------------------------------------------------
	// Write Protectコマンド
	//------------------------------------------------------------
#ifdef QSPI_FLASH_WRITE_PROTECT_SUPPORT_ISSI
	qspiFlashWriteProtect_ISSI (-1);
#endif

	//------------------------------------------------------------
	// Write Disableコマンド発行
	//------------------------------------------------------------
	qspiFlashWriteDisableCmd_ISSI ();

	//------------------------------------------------------------
	// Slave Clear
	//------------------------------------------------------------
    SPI_clear_slave_select (&g_flash_core_spi, SPI_SLAVE);

	return (status);
}


//**********************************************************************************
//	Flashイレーズ(全領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashEraseAll_ISSI (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;

	for (adrs = 0; adrs<QSPI_FLASH_SIZE; adrs += QSPI_FLASH_SEC_SIZE)
	{
		if ((status = qspiFlashSectorErase_ISSI (adrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Status Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		timeout				：タイムアウト時間
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashStatusCheck_ISSI (unsigned int timeout)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int retry;
    uint8_t command = QSPI_READ_STATUS_CMD_ISSI;
	uint8_t ready_bit;

	// ステータスCheck
	for (retry=0; retry<timeout; retry++)
	{
        SPI_transfer_block (&g_flash_core_spi, &command, 1, &ready_bit, sizeof(ready_bit));

		if ((ready_bit & 0x01) == 0)
			break;

		msDelay (1);
	}

	// Check Timeout
	if (retry >= timeout)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Status Timeout Error\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


#if 0
//**********************************************************************************
//	Wait ReadyCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		timeout				：タイムアウト時間
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashWaitReadyEraseCheck_ISSI (unsigned int timeout)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int retry;
    uint8_t command = QSPI_READ_FLAG_STATUS_CMD_ISSI;
	uint8_t ready_bit;

	// ステータスCheck
	for (retry=0; retry<timeout; retry++)
	{
        SPI_transfer_block (&g_flash_core_spi, &command, 1, &ready_bit, sizeof(ready_bit));

		if ((ready_bit & 0x80))
			break;

		msDelay (1);
	}

	// Check Timeout
	if (retry >= timeout)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Wait Ready Erase Timeout Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}
#endif


//**********************************************************************************
//	Write Protect Make
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		top					:0=Bottom/1=Top
//		data				:Protect Data
//		pMakeData			:Protect Make Dataを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashWriteProtectMake_ISSI (int top, int data, int *pMakeData)
{
#if 0 //@@@1
	unsigned char makeData;
	if (pMakeData == NULL)
		return (0);
	
	// Top or Bottom
	if (top == 0)
		makeData = 0;
	else
		makeData = QSPI_STATUS_TOP_BOTTOM_ISSI;

	// Bit3
	if (data & QSPI_STATUS_DATA3_ISSI)
		makeData |= QSPI_STATUS_PROTECT3_ISSI;

	// Bit012
	makeData |= ((data & QSPI_STATUS_DATA012_ISSI) << QSPI_STATUS_PROTECT012_SHIFT_ISSI);

	*pMakeData = makeData;
#endif //@@@1
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Write Protect
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		address				：Writeプロテクトを解除するアドレス(-1=プロテクト/0=すべてプロテクト解除)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashWriteProtect_ISSI (unsigned int address)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0//@@@1
#if !defined (MODE_FLASH_PROTECT_SIMPLE)
	unsigned int ix;
#endif
	u8 ReadStatusCmd[] = { QSPI_READ_STATUS_CMD_ISSI, 0 };
	u8 WriteStatusCmd[] = { QSPI_WRITE_STATUS_CMD_ISSI, 0 };
	int makeData;
	unsigned char writeProtectData;
#if !defined (MODE_FLASH_PROTECT_SIMPLE)
	int check;
#endif
	int top = 0;
#if !defined (MODE_FLASH_PROTECT_SIMPLE)
	unsigned int sector;
#endif
	u8 FlashStatus[2];

	// Check address parameter
	if ((address > QSPI_FLASH_SIZE) && (address != 0xffffffff))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Flash Write Protect Address(0x%x) Parameter Error.(Min:0 / Max:0x%x)\n", address, QSPI_FLASH_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if !defined (MODE_FLASH_PROTECT_SIMPLE)
	// セクタ番号算出
	sector = address / QSPI_FLASH_SEC_SIZE;
#endif

	// Check Protect Area
	if (address == -1)
	{
		// All Protect
		writeProtectData = 0x0f;
		top = 0;
	}
	else
	{
		// All Protect
		writeProtectData = 0x0f;
		top = 0;

		// All Protect Reset
		writeProtectData = 0x0;
		top = 0;
	}

	//------------------------------------------------------------
	// Write Enableコマンド発行
	//------------------------------------------------------------
	if ((status = qspiFlashWriteEnableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Read Statusコマンド発行
	//------------------------------------------------------------
	// Command
	FlashMsg[0].TxBfrPtr = ReadStatusCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	FlashMsg[1].TxBfrPtr = NULL;
	FlashMsg[1].RxBfrPtr = FlashStatus;
	FlashMsg[1].ByteCount = 2;
	FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

	// コマンド発行
	if (XQspiPsu_PolledTransfer (&QspiFlashInstance, FlashMsg, 2) != XST_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Read Status Command Error.\n");
		goto _DONE;
	}

	//------------------------------------------------------------
	// Writeプロテクト
	//------------------------------------------------------------

	// Statusレジスタに設定するデータを作成
	qspiFlashWriteProtectMake_ISSI (top, writeProtectData, &makeData);
	
	// Write Protectデータ設定
	FlashStatus[1] &= ~QSPI_STATUS_MASK_ISSI;
	FlashStatus[1] |= (unsigned char)makeData;
	WriteStatusCmd[1] = (unsigned char)FlashStatus[1];

	//------------------------------------------------------------
	// Write Statusコマンド
	//------------------------------------------------------------
	FlashMsg[0].TxBfrPtr = WriteStatusCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 2;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	// コマンド発行
	if (XQspiPsu_PolledTransfer (&QspiFlashInstance, FlashMsg, 1) != XST_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Write Status Command Error.\n");
		goto _DONE;
	}

	//------------------------------------------------------------
	// ステータスCheck
	//------------------------------------------------------------
	if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
	{
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Write Status Check Error.\n");
		goto _DONE;
	}

_DONE:
#endif//@@@1
	return (status);
}


//**********************************************************************************
//	Write Enableコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashWriteEnableCmd_ISSI (void)
{
	int status = AVAL_STATUS_SUCCESS;
    uint8_t cmd_buffer[1];	//@@@@1

	// ステータスCheck
	if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Weite Enable
    cmd_buffer[0] = QSPI_WRITE_ENABLE_CMD_ISSI;
    SPI_transfer_block (&g_flash_core_spi, cmd_buffer, 1, 0, 0);     	

_DONE:
	return (status);
}


//**********************************************************************************
//	Write Disableコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashWriteDisableCmd_ISSI (void)
{
	int status = AVAL_STATUS_SUCCESS;
    uint8_t cmd_buffer[1];	//@@@1

	// ステータスCheck
	if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Weite Disable
    cmd_buffer[0] = QSPI_WRITE_DISABLE_CMD_ISSI;
    SPI_transfer_block (&g_flash_core_spi, cmd_buffer, 1, 0, 0);     	

_DONE:
	return (status);
}


//**********************************************************************************
//	拡張アドレスコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：アドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashExAddresseCmd_ISSI (unsigned int adrs)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char cmdBuff[2];
	unsigned int qspiFlashCurrentBank_ISSI;

	//------------------------------------------------------------
	// 拡張アドレスコマンド発行
	//------------------------------------------------------------

	// ステータスCheck
	if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Extend Command設定
	cmdBuff[0] = QSPI_WREAR_CMD_ISSI;
	cmdBuff[1] = (unsigned char)((adrs & 0xFF000000) >> 24);
    SPI_transfer_block (&g_flash_core_spi, cmdBuff, sizeof(cmdBuff), 0, 0);     	

	// Bank設定
	qspiFlashCurrentBank_ISSI = (adrs & 0xFF000000) >> 24;
	if ((status = qspiFlashSetBank (qspiFlashCurrentBank_ISSI)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	3Byte アドレスコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if 0
int qspiFlash3ByteAddresseCmd_ISSI (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char cmdBuff[1];

	// ステータスCheck
	if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Extend Command設定
	cmdBuff[0] = QSPI_EXIT_4BYTE_ADRS_CMD_ISSI;
    SPI_transfer_block (&g_flash_core_spi, cmdBuff, sizeof(cmdBuff), 0, 0);     	

_DONE:
	return (status);
}
#endif


//**********************************************************************************
//	Write Protect Read
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ｐStatus				：Readステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashWriteProtectRead_ISSI (int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0//@@@1
	u8 ReadStatusCmd[] = { QSPI_READ_STATUS_CMD_ISSI, 0 };
	u8 WriteStatusCmd[] = { QSPI_WRITE_STATUS_CMD_ISSI, 0 };
	u8 FlashStatus[2];
	unsigned int writeProtectData = 0;
	unsigned int loop;
	unsigned int data;

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER));
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Write Protect Status Command Error.\n");
		goto _DONE;
	}

	// Top/Bottomの両方のプロテクト状態を確認
	for (loop=0; loop<2; loop++)
	{
		//------------------------------------------------------------
		// Write Enableコマンド発行
		//------------------------------------------------------------
		if ((status = qspiFlashWriteEnableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Read Statusコマンド発行
		//------------------------------------------------------------
		FlashMsg[0].TxBfrPtr = ReadStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = FlashStatus;
		FlashMsg[1].ByteCount = 2;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

		// コマンド発行
		if (XQspiPsu_PolledTransfer (&QspiFlashInstance, FlashMsg, 2) != XST_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Read Status Command Error.\n");
			goto _DONE;
		}

		//------------------------------------------------------------
		// Write Enableコマンド発行
		//------------------------------------------------------------
		if ((status = qspiFlashWriteEnableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Bottom/Topの指定
		//------------------------------------------------------------
		if (loop==0)
			WriteStatusCmd[1] = FlashStatus[1] | QSPI_STATUS_TOP_BOTTOM_ISSI;
		else
			WriteStatusCmd[1] = FlashStatus[1] & ~QSPI_STATUS_TOP_BOTTOM_ISSI;

		//------------------------------------------------------------
		// Write Statusコマンド発行
		//------------------------------------------------------------
		FlashMsg[0].TxBfrPtr = WriteStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 2;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		// コマンド発行
		if (XQspiPsu_PolledTransfer (&QspiFlashInstance, FlashMsg, 1) != XST_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Write Status Command Error.\n");
			goto _DONE;
		}

		// ステータスCheck
		if ((status = qspiFlashStatusCheck_ISSI (QSPI_FLASH_STATUS_COUNT_ISSI)) != AVAL_STATUS_SUCCESS)
		{
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Status Check Error.\n");
			goto _DONE;
		}

		//------------------------------------------------------------
		// Write Enableコマンド発行
		//------------------------------------------------------------
		if ((status = qspiFlashWriteEnableCmd_ISSI ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Read Statusコマンド発行
		//------------------------------------------------------------
		FlashMsg[0].TxBfrPtr = ReadStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = FlashStatus;
		FlashMsg[1].ByteCount = 2;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

		// コマンド発行
		if (XQspiPsu_PolledTransfer (&QspiFlashInstance, FlashMsg, 2) != XST_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Read Status Command Error.\n");
			goto _DONE;
		}


		//------------------------------------------------------------
		// Writeプロテクト取得
		//------------------------------------------------------------
		data = 0;

		if (FlashStatus[1] & QSPI_STATUS_PROTECT0_SHIFT_ISSI)
			data |= (1<<0);

		if (FlashStatus[1] & QSPI_STATUS_PROTECT1_ISSI)
			data |= (1<<1);

		if (FlashStatus[1] & QSPI_STATUS_PROTECT2_ISSI)
			data |= (1<<2);

		if (FlashStatus[1] & QSPI_STATUS_PROTECT3_ISSI)
			data |= (1<<3);

		if (loop == 0)
			writeProtectData = data;
		else
			writeProtectData |= (data<<4);
	}

	// ステータス取得
	*pStatus = writeProtectData;

_DONE:
#endif//@@@1
	return (status);
}


//**********************************************************************************
//	Write Protect Read
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		status				：Readステータス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashWriteProtectReadShow_ISSI (void)
{
	int status;
#if 0//@@@1
	int writeProtectStatus, data;

	// Write Protect Status取得
	if ((status = qspiFlashWriteProtectRead (&writeProtectStatus)) != AVAL_STATUS_SUCCESS)
		return (status);

	//--------------------------------------------------------------------------------
	// Bottom
	//--------------------------------------------------------------------------------
	data = writeProtectStatus & 0x0f;
	DEBUG_PRINT_FORCE ("[Bottom]\n");
	switch (data)
	{
		case 0:
			DEBUG_PRINT_FORCE ("Protected    : None\n");
			DEBUG_PRINT_FORCE ("Un Protected : All Sectors\n");
			break;
		case 1:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (511)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (0 to 510)\n");
			break;
		case 2:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (510 to 511)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (0   to 509)\n");
			break;
		case 3:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (508 to 511)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (0   to 507)\n");
			break;
		case 4:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (504 to 511)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (0   to 503)\n");
			break;
		case 5:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (496 to 511)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (0   to 495)\n");
			break;
		case 6:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (480 to 511)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (0   to 479)\n");
			break;
		case 7:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (448 to 511)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (0   to 447)\n");
			break;
		case 8:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (384 to 511)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (0   to 383)\n");
			break;
		case 9:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (256 to 511)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (0   to 255)\n");
			break;
		default:
			DEBUG_PRINT_FORCE ("Protected    : All Area\n");
			DEBUG_PRINT_FORCE ("Un Protected : None\n");
			break;
	}

	DEBUG_PRINT_FORCE ("\n");

	//--------------------------------------------------------------------------------
	// Top
	//--------------------------------------------------------------------------------
	data = (writeProtectStatus>>4) & 0x0f;
	DEBUG_PRINT_FORCE ("[Top]\n");
	switch (data)
	{
		case 0:
			DEBUG_PRINT_FORCE ("Protected    : None\n");
			DEBUG_PRINT_FORCE ("Un Protected : All Sectors\n");
			break;
		case 1:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (0)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (1 to 511)\n");
			break;
		case 2:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (0   to 1)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (2   to 511)\n");
			break;
		case 3:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (0   to 3)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (4   to 511)\n");
			break;
		case 4:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (0   to 7)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (8   to 511)\n");
			break;
		case 5:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (0   to 15)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (16  to 511)\n");
			break;
		case 6:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (0   to 31)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (32   to 511)\n");
			break;
		case 7:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (0   to 63)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (64  to 511)\n");
			break;
		case 8:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (0   to 127)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (128 to 511)\n");
			break;
		case 9:
			DEBUG_PRINT_FORCE ("Protected    : Sector  (0   to 256)\n");
			DEBUG_PRINT_FORCE ("Un Protected : Sectors (256 to 511)\n");
			break;
		default:
			DEBUG_PRINT_FORCE ("Protected    : All Area\n");
			DEBUG_PRINT_FORCE ("Un Protected : None\n");
			break;
	}

	DEBUG_PRINT_FORCE ("\n");
#endif //@@@1
	return (AVAL_STATUS_SUCCESS);
}

// eof
