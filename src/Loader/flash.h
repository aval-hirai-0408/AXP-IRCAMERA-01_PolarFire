//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// flash.h - Flash Program Header
//**********************************************************************************

#ifndef __QSPI_FLASH_H__
#define __QSPI_FLASH_H__

//----------------------------------------------------------------------------------
// defined
//----------------------------------------------------------------------------------

// Flash Size
#define QSPI_FLASH_SIZE					(128*1024*1024)

// Buffer Size
#define QSPI_FLASH_BUFFER_SIZE			(256)

// Bank Size
#define QSPI_FLASH_BANK_SIZE			(16*1024*1024)

// SPI Slave
#define SPI_SLAVE						(0)

// Don't Care
#define DONT_CARE						(0x00u)


//----------------------------------------------------------------------------------
// Mode
//----------------------------------------------------------------------------------
//#define MODE_FLASH_MICRON


//----------------------------------------------------------------------------------
// Flash Select
//----------------------------------------------------------------------------------
#define QSPI_FLASH_SELECT_N25Q			(1)		// Micron
#define QSPI_FLASH_SELECT_ISSI			(2)		// ISSI
#define QSPI_FLASH_SELECT_W25Q			(3)		// Winbond


//----------------------------------------------------------------------------------
// Function Define
//----------------------------------------------------------------------------------
int qspiFlashInitialize (void);
int qspiFlashRead (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int qspiFlashSetBank (unsigned int bankNum);
int qspiFlashGetBank (unsigned int *pBankNum);
int qspiFlashGetFlashId (unsigned char *pManufacturerId, unsigned char *pDeviceId);

int qspiFlashInitialize_N25Q (void);
int qspiFlashRead_N25Q (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int qspiFlashStatusCheck_N25Q (unsigned int timeout);
int qspiFlashWaitReadyEraseCheck_N25Q (unsigned int timeout);
int qspiFlashWriteEnableCmd_N25Q (void);
int qspiFlashWriteDisableCmd_N25Q (void);
int qspiFlashExAddresseCmd_N25Q (unsigned int adrs);
int qspiFlash3ByteAddresseCmd_N25Q (void);

#endif // __QSPI_FLASH_H__

// eof
