//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// flash_N25Q_Micron.c - Flash Program
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
#define QSPI_FLASH_STATUS_COUNT_N25Q	(10000)		//10s

// Bank Size
#define QSPI_FLASH_BANK_SIZE_N25Q		(16*1024*1024)

// Read Size
#define QSPI_FLASH_READ_SIZE_N25Q		(32*1024)

// Bank Default
#define QSPI_FLASH_BANK_DEFAULT_N25Q	(0xffffffff)

// Flash Command
#define QSPI_WRITE_STATUS_CMD_N25Q		(0x01)
#define QSPI_READ_CMD_N25Q				(0x03)
#define QSPI_WRITE_DISABLE_CMD_N25Q		(0x04)
#define QSPI_READ_STATUS_CMD_N25Q		(0x05)
#define QSPI_WRITE_ENABLE_CMD_N25Q		(0x06)
#define QSPI_FAST_QSPI_READ_CMD_N25Q	(0x0B)
#define QSPI_DUAL_QSPI_READ_CMD_N25Q	(0x3B)
#define QSPI_READ_ID_N25Q				(0x9F)
#define QSPI_READ_FLAG_STATUS_CMD_N25Q	(0x70)
#define QSPI_EXIT_4BYTE_ADRS_CMD_N25Q	(0xE9)
#define QSPI_WREAR_CMD_N25Q				(0xC5)


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
int qspiFlashInitialize_N25Q (void)
{
	int status = AVAL_STATUS_SUCCESS;
	
	//------------------------------------------------------------
	// Slave Select   
	//------------------------------------------------------------
    SPI_set_slave_select (&g_flash_core_spi, SPI_SLAVE);

	//------------------------------------------------------------
	// Bank設定
	//------------------------------------------------------------
	if ((status = qspiFlashSetBank (QSPI_FLASH_BANK_DEFAULT_N25Q)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Write Enableコマンド発行
	//------------------------------------------------------------
	if ((status = qspiFlashWriteEnableCmd_N25Q ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// 3byteアドレスコマンド発行
	//------------------------------------------------------------
	if ((status = qspiFlash3ByteAddresseCmd_N25Q ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// 拡張アドレスコマンド発行
	//------------------------------------------------------------
	if ((status = qspiFlashExAddresseCmd_N25Q (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Write Disableコマンド発行
	//------------------------------------------------------------
	if ((status = qspiFlashWriteDisableCmd_N25Q ()) != AVAL_STATUS_SUCCESS)
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
int qspiFlashRead_N25Q (unsigned int adrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int Index;
	unsigned int transed, transSize;
	unsigned int alignAdrs, alignSize;
	unsigned int bank;
    unsigned char cmdBuff[6];
	unsigned int qspiFlashCurrentBank_N25Q;

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
		if (size > QSPI_FLASH_READ_SIZE_N25Q)
			transSize = QSPI_FLASH_READ_SIZE_N25Q;
		else
			transSize = size - transed;

		// スタートアドレスの確認
		if (adrs & (QSPI_FLASH_READ_SIZE_N25Q-1))
		{
			// QSPI_FLASH_BANK_SIZE_N25Qアライン分のサイズのみを転送する
			alignAdrs = adrs & (QSPI_FLASH_READ_SIZE_N25Q-1);
			alignSize = QSPI_FLASH_READ_SIZE_N25Q - alignAdrs;
			
			if (transSize > alignSize)
				transSize = alignSize;
		}

		// アクセスするバンクをCheck
		bank = (adrs & 0xFF000000) >> 24;

		// Bank取得
		if ((status = qspiFlashGetBank (&qspiFlashCurrentBank_N25Q)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 現在のバンクと同一?
		if (qspiFlashCurrentBank_N25Q != bank)
		{
			//------------------------------------------------------------
			// Write Enableコマンド発行
			//------------------------------------------------------------
			if ((status = qspiFlashWriteEnableCmd_N25Q ()) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			//------------------------------------------------------------
			// 拡張アドレスコマンド発行
			//------------------------------------------------------------
			if ((status = qspiFlashExAddresseCmd_N25Q (adrs)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		//------------------------------------------------------------
		// Readコマンド発行
		//------------------------------------------------------------
		// QSPI_READ_CMD_N25Q
		// QSPI_FAST_QSPI_READ_CMD_N25Q
		// QSPI_DUAL_QSPI_READ_CMD_N25Q
		// QUAD_QSPI_READ_CMD_N25Q
		//------------------------------------------------------------

		cmdBuff[0] = QSPI_READ_CMD_N25Q;
		//cmdBuff[0] = QSPI_FAST_QSPI_READ_CMD_N25Q;
		//cmdBuff[0] = QUAD_QSPI_READ_CMD_N25Q;
	    cmdBuff[1] = (uint8_t)((adrs >> 16) & 0xFF);
	    cmdBuff[2] = (uint8_t)((adrs >> 8) & 0xFF);
	    cmdBuff[3] = (uint8_t)(adrs & 0xFF);
	    cmdBuff[4] = DONT_CARE;
	    cmdBuff[5] = DONT_CARE;
	    
		// Wait Redy Check
		if ((status = qspiFlashWaitReadyEraseCheck_N25Q (QSPI_FLASH_STATUS_COUNT_N25Q)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Status Check
		if ((status = qspiFlashStatusCheck_N25Q (QSPI_FLASH_STATUS_COUNT_N25Q)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Read
	    SPI_transfer_block (&g_flash_core_spi, cmdBuff, 4, &pBuffer[transed], transSize);
	}

_DONE:
	//------------------------------------------------------------
	// Write Disableコマンド発行
	//------------------------------------------------------------
	qspiFlashWriteDisableCmd_N25Q ();

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
int qspiFlashStatusCheck_N25Q (unsigned int timeout)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int retry;
    uint8_t command = QSPI_READ_STATUS_CMD_N25Q;
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
//	Wait ReadyCheck
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		timeout				：タイムアウト時間
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashWaitReadyEraseCheck_N25Q (unsigned int timeout)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int retry;
    uint8_t command = QSPI_READ_FLAG_STATUS_CMD_N25Q;
	uint8_t ready_bit;

	// ステータスCheck
	for (retry=0; /*retry<timeout*/; retry++)
	{
        SPI_transfer_block (&g_flash_core_spi, &command, 1, &ready_bit, sizeof(ready_bit));

		if ((ready_bit & 0x80))
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
int qspiFlashWriteEnableCmd_N25Q (void)
{
	int status = AVAL_STATUS_SUCCESS;
    uint8_t cmd_buffer[1];

	// ステータスCheck
	if ((status = qspiFlashStatusCheck_N25Q (QSPI_FLASH_STATUS_COUNT_N25Q)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Weite Enable
    cmd_buffer[0] = QSPI_WRITE_ENABLE_CMD_N25Q;
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
int qspiFlashWriteDisableCmd_N25Q (void)
{
	int status = AVAL_STATUS_SUCCESS;
    uint8_t cmd_buffer[1];

	// ステータスCheck
	if ((status = qspiFlashStatusCheck_N25Q (QSPI_FLASH_STATUS_COUNT_N25Q)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Weite Disable
    cmd_buffer[0] = QSPI_WRITE_DISABLE_CMD_N25Q;
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
int qspiFlashExAddresseCmd_N25Q (unsigned int adrs)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char cmdBuff[2];
	unsigned int qspiFlashCurrentBank_N25Q;

	// ステータスCheck
	if ((status = qspiFlashStatusCheck_N25Q (QSPI_FLASH_STATUS_COUNT_N25Q)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Extend Command設定
	cmdBuff[0] = QSPI_WREAR_CMD_N25Q;
	cmdBuff[1] = (unsigned char)((adrs & 0xFF000000) >> 24);
    SPI_transfer_block (&g_flash_core_spi, cmdBuff, sizeof(cmdBuff), 0, 0);     	

	// Bank設定
	qspiFlashCurrentBank_N25Q = (adrs & 0xFF000000) >> 24;
	if ((status = qspiFlashSetBank (qspiFlashCurrentBank_N25Q)) != AVAL_STATUS_SUCCESS)
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
int qspiFlash3ByteAddresseCmd_N25Q (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char cmdBuff[1];

	// ステータスCheck
	if ((status = qspiFlashStatusCheck_N25Q (QSPI_FLASH_STATUS_COUNT_N25Q)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Extend Command設定
	cmdBuff[0] = QSPI_EXIT_4BYTE_ADRS_CMD_N25Q;
    SPI_transfer_block (&g_flash_core_spi, cmdBuff, sizeof(cmdBuff), 0, 0);     	

_DONE:
	return (status);
}

// eof
