//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// qspiFlash.h - QSPI Flash Program Header
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

int qspiFlashInitialize_ISSI (void);
int qspiFlashRead_ISSI (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int qspiFlashStatusCheck_ISSI (unsigned int timeout);
int qspiFlashWriteEnableCmd_ISSI (void);
int qspiFlashWriteDisableCmd_ISSI (void);
int qspiFlashExAddresseCmd_ISSI (unsigned int adrs);

#endif // __QSPI_FLASH_H__

// eof
