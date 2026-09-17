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
#include "flash.h"
#include "aval_status.h"
#include "core_spi.h"
#include "hw_platform.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------

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

// Instance
extern spi_instance_t g_flash_core_spi; 


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
		return (status);
	}

	// Check size Parameter
	if (size > QSPI_FLASH_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		return (status);
	}

	// Check adrs + size Parameter
	if ((adrs + size) > QSPI_FLASH_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
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
	for (retry=0; /*retry<timeout*/; retry++)
	{
        SPI_transfer_block (&g_flash_core_spi, &command, 1, &ready_bit, sizeof(ready_bit));

		if ((ready_bit & 0x01) == 0)
			break;

		//msDelay (1);
	}

	#if 0
	// Check Timeout
	if (retry >= timeout)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_TIMEOUT);
		goto _DONE;
	}
	#endif

_DONE:
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
    uint8_t cmd_buffer[1];

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
    uint8_t cmd_buffer[1];

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

// eof
