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
#include "qspiFlash.h"
#include "aval_status.h"
#include "core_spi.h"
#include "hw_platform.h"
#include "common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------

// Flash Select
#define QSPI_FLASH_SELECT_N25Q				(1)		// Micron
#define QSPI_FLASH_SELECT_ISSI				(2)		// ISSI

// Device ID

// Micron
#define QSPI_FLASH_READ_ID_MANUFACTURE_N25Q	(0x20)
#define QSPI_FLASH_READ_ID_DEVICE_N25Q		(0xBA)

// ISSI
#define QSPI_FLASH_READ_ID_MANUFACTURE_ISSI	(0x9D)
#define QSPI_FLASH_READ_ID_DEVICE_ISSI		(0x60)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// Instance
spi_instance_t g_flash_core_spi; 

// QSPI Flash Select
int gQspiFlashSelect = -1;

// QSPI Bank Select
unsigned int gQspiFlashBank = 0;


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
	int status = AVAL_STATUS_SUCCESS;
	uint8_t manufacturer_id, device_id;

//@@@1	
	OUT32 ((CORESPI_BASE_ADDR+0x00), 2);
	OUT32 ((CORESPI_BASE_ADDR+0x2c), 2);
	OUT32 ((CORESPI_BASE_ADDR+0x00), 3);
//@@@1	
	// Initialize
    SPI_init (&g_flash_core_spi, CORESPI_BASE_ADDR, 32);

	// Configuration
    SPI_configure_master_mode (&g_flash_core_spi);

	// Get DeviceID
	qspiFlashGetFlashId (&manufacturer_id, &device_id);
	
	// Check Read ID
	if ((manufacturer_id == QSPI_FLASH_READ_ID_MANUFACTURE_ISSI) && (device_id == QSPI_FLASH_READ_ID_DEVICE_ISSI))
	{
		gQspiFlashSelect = QSPI_FLASH_SELECT_ISSI;
		
		if ((status = qspiFlashInitialize_ISSI ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if ((manufacturer_id == QSPI_FLASH_READ_ID_MANUFACTURE_N25Q) && (device_id == QSPI_FLASH_READ_ID_DEVICE_N25Q))
	{
		gQspiFlashSelect = QSPI_FLASH_SELECT_N25Q;
		
		if ((status = qspiFlashInitialize_N25Q ()) != AVAL_STATUS_SUCCESS)
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

	if (gQspiFlashSelect == QSPI_FLASH_SELECT_ISSI)
	{
		status = qspiFlashRead_ISSI (adrs, pBuffer, size);
	}
	else if (gQspiFlashSelect == QSPI_FLASH_SELECT_N25Q)
	{
		status = qspiFlashRead_N25Q (adrs, pBuffer, size);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_QSPI_FLASH, AVAL_STATUS_IO);
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
	gQspiFlashBank = bankNum;

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

	// Bank取得
	*pBankNum = gQspiFlashBank;

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

	return (status);
}

// eof
