//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// qspiFlash.c - QSPI Flash Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "qspiFlash.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// QSPI Flash Status
int gQspiFlashStatus = 0;

// QSPI Flash Select
int gQspiFlashSelect = -1;

// Buffer
unsigned char QspiFlashWriteBuffer[(QSPI_FLASH_BUFFER_SIZE+4)];	// Writeコマンド用に+4

// Instance
spi_instance_t g_flash_core_spi; 


//**********************************************************************************
//	初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashInitialize (void)
{
	uint8_t manufacturer_id, device_id;

	// Initialize
    SPI_init (&g_flash_core_spi, CORESPI_BASE_ADDR, 32);

	// Configuration
    SPI_configure_master_mode (&g_flash_core_spi);

	// Flag Initialize
	OUT32 (FIRM_DATA_QSPI_BANK_FLAG_ADRS, 0xffffffff);

	// Unprotect
	//FLASH_global_unprotect ();

	// Get DeviceID
	qspiFlashGetFlashId (&manufacturer_id, &device_id);
	
	// Check Read ID
	if ((manufacturer_id == QSPI_FLASH_READ_ID_MANUFACTURE_N25Q) && (device_id == QSPI_FLASH_READ_ID_DEVICE_N25Q))
	{
		gQspiFlashSelect = QSPI_FLASH_SELECT_N25Q;
		
		if ((gQspiFlashStatus = qspiFlashInitialize_N25Q ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	
_DONE:
	return (gQspiFlashStatus);
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
int qspiFlashWrite (unsigned int adrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;

	if (gQspiFlashSelect == QSPI_FLASH_SELECT_N25Q)
	{
		status = qspiFlashWrite_N25Q (adrs, pBuffer, size);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Write Device Select Error\n");
		goto _DONE;
	}

_DONE:
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
int qspiFlashRead (unsigned int adrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;

	if (gQspiFlashSelect == QSPI_FLASH_SELECT_N25Q)
	{
		status = qspiFlashRead_N25Q (adrs, pBuffer, size);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Read Device Select Error\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Flashイレーズ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：Flashアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int qspiFlashSectorErase (unsigned int adrs)
{
	int status = AVAL_STATUS_SUCCESS;

	if (gQspiFlashSelect == QSPI_FLASH_SELECT_N25Q)
	{
		status = qspiFlashSectorErase_N25Q (adrs);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Erase Device Select Error\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Flashイレーズ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashEraseAll (void)
{
	int status = AVAL_STATUS_SUCCESS;

	if (gQspiFlashSelect == QSPI_FLASH_SELECT_N25Q)
	{
		status = qspiFlashEraseAll_N25Q ();
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Erase All Device Select Error\n");
		goto _DONE;
	}

_DONE:
	return (status);
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
int qspiFlashWriteProtect (unsigned int address)
{
	int status = AVAL_STATUS_SUCCESS;

	if (gQspiFlashSelect == QSPI_FLASH_SELECT_N25Q)
	{
		status = qspiFlashWriteProtect_N25Q (address);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Write Protect All Device Select Error\n");
		goto _DONE;
	}
	
_DONE:
	return (status);
}


//**********************************************************************************
//	Write Protect Read
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ｐStatus				：Readステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashWriteProtectRead (int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;

	if (gQspiFlashSelect == QSPI_FLASH_SELECT_N25Q)
	{
		status = qspiFlashWriteProtectRead_N25Q (pStatus);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Write Protect Read All Device Select Error\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Write Protect Read Show
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		status				：Readステータス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashWriteProtectReadShow (void)
{
	int status = AVAL_STATUS_SUCCESS;

	if (gQspiFlashSelect == QSPI_FLASH_SELECT_N25Q)
	{
		status = qspiFlashWriteProtectReadShow_N25Q ();
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Write Protect Show All Device Select Error\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	QSPI Set Flash Bank
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bankNum				：バンク番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashSetBank (unsigned int bankNum)
{
	// Bank設定
	OUT32 (FIRM_DATA_QSPI_BANK_FLAG_ADRS, bankNum);

	// Cache Flush
	cacheFlushRange (FIRM_DATA_QSPI_BANK_FLAG_ADRS, 4);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	QSPI Get Flash Bank
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pBankNum			：バンク番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashGetBank (unsigned int *pBankNum)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pBankNum Parameter
	if (pBankNum == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash pBankNum NULL Parameter Error.\n");
		goto _DONE;
	}

	// Bank取得
	*pBankNum = IN32 (FIRM_DATA_QSPI_BANK_FLAG_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	QSPI Get Flash ID
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pBankNum			：バンク番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int qspiFlashGetFlashId (unsigned char *pManufacturerId, unsigned char *pDeviceId)
{
	int status = AVAL_STATUS_SUCCESS;
    uint8_t read_device_id_cmd = 0x9f;
    uint8_t read_buffer[3];

	// Check pManufacturerId Parameter
	if (pManufacturerId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Manufacture ID NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pDeviceId Parameter
	if (pDeviceId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Device ID NULL Parameter Error.\n");
		goto _DONE;
	}

	// Slave Select   
    SPI_set_slave_select (&g_flash_core_spi, SPI_SLAVE);

	// Get ID
    SPI_transfer_block (&g_flash_core_spi, &read_device_id_cmd, 1, read_buffer, sizeof(read_buffer));

	// Clear Slave
	SPI_clear_slave_select(&g_flash_core_spi, SPI_SLAVE);

	// Set Manufacture ID
    *pManufacturerId = read_buffer[0];

	// Set Device ID
	*pDeviceId = read_buffer[1];

_DONE:
	return (status);
}

// eof
