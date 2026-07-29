//**********************************************************************************
//
//                              Camera Program Header
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// diag.h - Command Program Header
//**********************************************************************************

#ifndef __CMD_DIAG_H__
#define __CMD_DIAG_H__

//----------------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------------

#include <stdio.h>			// fopen_s, etc...
#include <stdlib.h>			// mbstowcs_s, etc...
#include <math.h>			// floorf, etc...
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "../Common/common.h"


//----------------------------------------------------------------------------------
// Memory Test Size
//----------------------------------------------------------------------------------
#define TEST_MEM_MAX_SIZE			(32*1024*1024)
#define TEST_MEM_MAX_SIZE_VERSION2	(256*1024*1024)
#define TEST_MEM_MIN_SIZE			(1024*1024)
#define TEST_SIZE_B					(1*1024*1024)

// Display Count
#define MEMORY_DISPLAY_COUNT		(0x100000)


//DDR info
#define NT5AD_DDR_BASE_ADDR			(0x20000000)
#define NT5AD_DDR_DQ				(64)
#define NT5AD_DDR_CAS				(10)
#define NT5AD_DDR_RAS				(16)
#define NT5AD_DDR_BANK				(2)


//----------------------------------------------------------------------------------
// Function Define
//----------------------------------------------------------------------------------

// diagDma.c
int cmdDiagCameraTestPattern (void *str);
int cmdDiagDma (void *str);
int cmdDiagDmaHelp (void *str);
int cmdDiagDmaMain (void *str);
int cmdDiagDmaCmp (void *str);
int cmdDiagDmaCmpHelp (void *str);
int cmdDiagDmaCmpMain (void *str);
int cmdDiagCamera (void *str);
int cmdDiagEnableTP (void);
int cmdDiagDisableTP (void);
int cmdDiagEnableAoiTP (void);
int cmdDiagDisableAoiTP (void);

// diagMain.c
int cmdDiagMain2 (void *str);
int cmdDiagMain2Help (void *str);
int cmdDiagDevelopMain (void *str);
int cmdDiagDumpMain (void *str);
int cmdDiagAging (void *str);
int cmdDiagAgingHelp (void *str);
int cmdDiagAgingMain (unsigned int loop, int mode);

// diagMem.c
int cmdDiagMem (void *str);
int cmdDiagDdr (void *str);
int cmdDiagDdrHelp (void *str);
int cmdDiagMem2 (void *str);
int cmdDiagMemRW (unsigned int loop, unsigned int addr, unsigned int size);
int cmdDiagMemRW2 (unsigned int loop, char *msg);
int cmdDiagMemLong (unsigned int tloop, unsigned int addr, unsigned int size);
int cmdDiagMemWord (unsigned int tloop, unsigned int addr, unsigned int size);
int cmdDiagMemByte (unsigned int tloop, unsigned int addr, unsigned int size);
int cmdDiagMemLongShift (unsigned int tloop, unsigned int addr, unsigned int size);
int cmdDiagMErrorDump (unsigned int addr);
int cmdDiagMemDataLine(unsigned int bus);
int cmdDiagMemCAS(unsigned int cas);
int cmdDiagMemRAS(unsigned int cas, unsigned int ras);
int cmdDiagMemBank(unsigned int bank);
int diagDdrSetMode (int mode);
int diagDdrGetMode (int *pMode);

// diagQspi.c
int cmdDiagQspiErase (void *str);
int cmdDiagQspiEraseAll (void *str);
int cmdDiagQspiFlashRW (void *str);
int cmdDiagQspiFlashRWAll (void *str);
int cmdDiagQspiFlashRWRandom (void *str);
int cmdDiagQspi (void *str);
int cmdDiagQspiHelp (void *str);
int cmdDiagQspiFlashRWSector (void *str);
int cmdDiagQspiBlankCheck (void *str);
int cmdDiagQspiWriteProtect (void *str);
int cmdDiagQspiWriteProtectHelp (void *str);
int cmdDiagQspiWriteProtectCancel (void *str);
int cmdDiagQspiWriteProtectStatus (void *str);

// diagTemp.c
int cmdDiagSensorTemp (void *str);
int cmdDiagSensorTempHelp (void *str);
int cmdDiagSensorTempMain (void *str);
int cmdDiagCaseTemp (void *str);
int cmdDiagCaseTempHelp (void *str);
int cmdDiagCaseTempMain (void *str);
int cmdDiagFpgaTemp (void *str);
int cmdDiagFpgaTempHelp (void *str);
int cmdDiagFpgaTempMain (void *str);

#endif  // __CMD_DIAG_H__

// eof
