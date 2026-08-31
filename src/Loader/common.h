//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// common.h - common Header
//**********************************************************************************

#ifndef __COMMON_H__
#define __COMMON_H__

//----------------------------------------------------------------------------------
// defined
//----------------------------------------------------------------------------------

// Version
#define FIRM_VERSION				"0.4"

// DDR
#define DDR_BASE_ADDRESS			(0x80010000)
#define DDR_APP_MAX_SIZE			(3*1024*1024)

// Flash
#define FLASH_PRGRAM_START_ADRS		(0x00200000)			// Flash Start Address
#define FLASH_SEGMENT_SIZE			(256)					// Write segment size

// Buffer
#define FLASH_READ_SIZE				(1024)

// Version Save Adrs
#define FIRM_DATA_ADRS				(0x82004200)            // DDR Base
#define FIRM_DATA_BOOT_VERSION		(FIRM_DATA_ADRS+0x6c8)  // 8byte


//----------------------------------------------------------------------------------
// IO Access Macro
//----------------------------------------------------------------------------------
#define IN8(adrs)                           (*(volatile unsigned char *)(unsigned long)adrs)
#define IN16(adrs)                          (*(volatile unsigned short *)(unsigned long)adrs)
#define IN32(adrs)                          (*(volatile unsigned int *)(unsigned long)adrs)
#define IN64(adrs)                          (*(volatile unsigned long long *)(unsigned long)adrs)
#define OUT8(adrs,data)                     ((*(volatile unsigned char *)(unsigned long)adrs)=data)
#define OUT16(adrs,data)                    ((*(volatile unsigned short *)(unsigned long)adrs)=data)
#define OUT32(adrs,data)                    ((*(volatile unsigned int *)(unsigned long)adrs)=data)
#define OUT64(adrs,data)                    ((*(volatile unsigned long long *)adrs)=data)
#define INF(adrs)                           (*(volatile float *)(unsigned long)adrs)
#define OUTF(adrs,data)                     ((*(volatile float *)(unsigned long)adrs)=data)

#endif // __COMMON_H__

// eof
