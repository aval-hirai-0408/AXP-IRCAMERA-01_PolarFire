//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// diagMem.c - Memory Test
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define MEMORY_DISPLAY_COUNT (0x20000)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
unsigned int gTestMemBase;
unsigned int gTestMemReqSize;
int multiMode;


//**********************************************************************************
//	メモリ評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDdr (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed1 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDiagDdrHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Diagステート
		saveLed1 = ledDiagnosticState ();

		// 検査
		status = cmdDiagMem2 (gCmdArg[1]);

		// LEDの設定を元に戻す
		ledReturnState (-1, saveLed1);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Memory Aces Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDdrHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A memory check is performed.\n");
	DEBUG_PRINT_FORCE ("  Command           : diagddr [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Check Number of times\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	メモリ評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagMem2 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int addr;
	unsigned int loop;
	int mode = 1;

	// 検査回数取得
	if (sscanf (str, "%d", &loop) != 1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

	// DDR Mode取得
	if ((status = diagDdrGetMode (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Memory Request
	gTestMemBase = (unsigned int)NULL;

	if (mode == 0)
	{
		gTestMemReqSize = TEST_MEM_MAX_SIZE_VERSION2 / 2;
	}
	else
	{
		gTestMemReqSize = 32*1024*1024;
	}

	//--------------------------------------------------------------------------------
	// Test1
	//--------------------------------------------------------------------------------

	// アドレス取得
	addr = DIAG_DDR_BASE1_ADRS;

	// DDR検査
	if ((status = cmdDiagMemRW (loop, addr, gTestMemReqSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//--------------------------------------------------------------------------------
	// Test2
	//--------------------------------------------------------------------------------

	// Check DDR Mode
	if (mode != 0)
		goto _DONE;

	// アドレス取得
	addr = DIAG_DDR_BASE2_ADRS;

	// サイズ設定
	gTestMemReqSize = TEST_MEM_MAX_SIZE_VERSION2;

	// DDR検査
	if ((status = cmdDiagMemRW (loop, addr, gTestMemReqSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	メモリ評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		loop					：テスト回数
//		addr					：評価アドレス
//		size					：評価サイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagMemRW (unsigned int loop, unsigned int addr, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int tloop;

MORE_MEM:
	for (tloop=1; tloop<=loop; tloop++)
	{
		DEBUG_PRINT ("\n----------Test Loop=%d/%d Size=%dM Address=0x%lx---------\n", tloop, loop, (size/1024/1024), addr);

		/*---------------------------*/
		/* 通常RW評価                */
		/*---------------------------*/
		if (multiMode == 0)
			DEBUG_PRINT ("Long Test");
		fflush (stdout);

		if ((status = cmdDiagMemLong (tloop, addr, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (multiMode == 0)
			DEBUG_PRINT ("\nWord Test");
		fflush (stdout);

		if ((status = cmdDiagMemWord (tloop, addr, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (multiMode == 0)
			DEBUG_PRINT ("\nByte Test");
		fflush (stdout);

		if ((status = cmdDiagMemByte (tloop, addr, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		/*---------------------------*/
		/* Shift評価                 */
		/*---------------------------*/
		if (multiMode == 0)
			DEBUG_PRINT ("\nShiftTest");
		fflush (stdout);

		if ((status = cmdDiagMemLongShift (tloop, addr, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;


		/*---------------------------*/
		/* Data Line評価             */
		/*---------------------------*/
		if (multiMode == 0)
			DEBUG_PRINT ("Data Line Test\n");
		fflush (stdout);

		if ((status = cmdDiagMemDataLine (NT5AD_DDR_DQ)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		/*---------------------------*/
		/* CAS評価                   */
		/*---------------------------*/
		if (multiMode == 0)
			DEBUG_PRINT ("\nCAS Test\n");
		fflush (stdout);

		if ((status = cmdDiagMemCAS (NT5AD_DDR_CAS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		/*---------------------------*/
		/* RAS評価                   */
		/*---------------------------*/
		if (multiMode == 0)
			DEBUG_PRINT ("\nRAS Test\n");
		fflush (stdout);

		if ((status = cmdDiagMemRAS (NT5AD_DDR_CAS, NT5AD_DDR_RAS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		/*---------------------------*/
		/* Bank評価                  */
		/*---------------------------*/
		if (multiMode == 0)
			DEBUG_PRINT ("\nBank Test\n");
		fflush (stdout);

		if ((status = cmdDiagMemBank (NT5AD_DDR_BANK)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	DEBUG_PRINT ("\n");

	if (loop == 0xFFFFFFFF)
		goto MORE_MEM;

_DONE:
	return (status);
}


//**********************************************************************************
//	メモリLong評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		tloop					：検査回数
//		addr					：評価アドレス
//		size					：評価サイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外                       		：異常終了
//==================================================================================
int cmdDiagMemLong (unsigned int tloop, unsigned int addr, unsigned int size)
{
	volatile unsigned int *pBase1, *pBase2, *ptr1, *ptr2;
	int loop, loopBase;
	unsigned int readData1, readData2;
	int status = AVAL_STATUS_SUCCESS;
	int iii=0;

	pBase1   = (volatile unsigned int *)addr;
	pBase2   = (volatile unsigned int *)(addr + (size/2));
	loopBase = size/4/2;

/****************************************/
/* address data                         */
/****************************************/

	//--------------------------------------------------
	// 書き込み
	//--------------------------------------------------

	//キャッシュフラッシュ
	cacheFlush ();

	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if ((loop%MEMORY_DISPLAY_COUNT) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		*ptr1 = (unsigned int)ptr1;
		*ptr2 = (unsigned int)ptr2;
		ptr1++;
		ptr2++;
	}

	//--------------------------------------------------
	// ｺﾝﾍﾟｱ
	//--------------------------------------------------

	//キャッシュフラッシュ
	cacheFlush ();

	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if ((loop%MEMORY_DISPLAY_COUNT) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		readData1 = *ptr1;
		if (readData1 != (unsigned int)ptr1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Long Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%08x\nRead  = 0x%08x\n", (unsigned int)ptr1, (unsigned int)ptr1, readData1);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		readData2 = *ptr2;
		if (readData2 != (unsigned int)ptr2)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Long Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%08x\nRead  = 0x%08x\n", (unsigned int)ptr2, (unsigned int)ptr2, readData2);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		ptr1++;
		ptr2++;
	}


/****************************************/
/* address reverse data                 */
/****************************************/

	//--------------------------------------------------
	// 書き込み
	//--------------------------------------------------

	//キャッシュフラッシュ
	cacheFlush ();

	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if ((loop%MEMORY_DISPLAY_COUNT) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		*ptr1 = ~(unsigned int)ptr1;
		*ptr2 = ~(unsigned int)ptr2;
		ptr1++;
		ptr2++;
	}

	//--------------------------------------------------
	// ｺﾝﾍﾟｱ
	//--------------------------------------------------

	//キャッシュフラッシュ
	cacheFlush ();

	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;

	while (loop--)
	{
		if (multiMode == 0)
		{
			if ((loop%MEMORY_DISPLAY_COUNT) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		readData1 = *ptr1;
		if (readData1 != ~(unsigned int)ptr1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Long Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%08x\nRead  = 0x%08x\n", (unsigned int)ptr1, ~(unsigned int)ptr1, readData1);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		readData2 = *ptr2;
		if (readData2 != ~(unsigned int)ptr2)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Long Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%08x\nRead  = 0x%08x\n", (unsigned int)ptr2, ~(unsigned int)ptr2, readData2);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		ptr1++;
		ptr2++;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	メモリWord評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		tloop					：検査回数
//		addr					：評価アドレス
//		size					：評価サイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外                      		：異常終了
//==================================================================================
int cmdDiagMemWord (unsigned int tloop, unsigned int addr, unsigned int size)
{
	volatile unsigned short *pBase1, *pBase2, *ptr1, *ptr2;
	unsigned short data1, data2;
	unsigned short readData1, readData2;
	int loop, loopBase;
	int status = AVAL_STATUS_SUCCESS;

	pBase1   = (volatile unsigned short *)addr;
	pBase2   = (volatile unsigned short *)(addr + (size/2));
	loopBase = size/2/2;

/****************************************/
/* address data (16 bit)                */
/****************************************/

	//--------------------------------------------------
	// 書き込み
	//--------------------------------------------------

	//キャッシュフラッシュ
	cacheFlush ();

	data1 = (unsigned short)addr;
	data2 = (unsigned short)(addr + (size/2));
	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if ((loop%(MEMORY_DISPLAY_COUNT*2)) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		*ptr1++ = data1++;
		*ptr2++ = data2++;
	}

	//--------------------------------------------------
	// ｺﾝﾍﾟｱ
	//--------------------------------------------------

	//キャッシュフラッシュ
	cacheFlush ();

	data1 = (unsigned short)addr;
	data2 = (unsigned short)(addr + (size/2));
	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if ((loop%(MEMORY_DISPLAY_COUNT*2)) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		readData1 = *ptr1;
		if (readData1 != data1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Word Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%04x\nRead  = 0x%04x\n", (unsigned int)ptr1, data1, readData1);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		readData2 = *ptr2;
		if (readData2 != data2)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Word Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%04x\nRead  = 0x%04x\n", (unsigned int)ptr2, data2, readData2);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		ptr1++;
		ptr2++;
		data1++;
		data2++;
	}

/****************************************/
/* address revintse data (16 bit)        */
/****************************************/

	//--------------------------------------------------
	// 書き込み
	//--------------------------------------------------
	//キャッシュフラッシュ
	cacheFlush ();

	data1 = (unsigned short)addr;
	data2 = (unsigned short)(addr + (size/2));
	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if ((loop%((MEMORY_DISPLAY_COUNT*2))) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		*ptr1++ = ~data1;
		*ptr2++ = ~data2;
		data1++;
		data2++;
	}

	//--------------------------------------------------
	// ｺﾝﾍﾟｱ
	//--------------------------------------------------

	//キャッシュフラッシュ
	cacheFlush ();

	data1 = (unsigned short)addr;
	data2 = (unsigned short)(addr + (size/2));
	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if ((loop%(MEMORY_DISPLAY_COUNT*2)) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		readData1 = *ptr1;
		if (readData1 != (unsigned short)~data1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Word Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%04x\nRead  = 0x%04x\n", (unsigned int)ptr1, ~data1, readData1);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		readData2 = *ptr2;
		if (readData2 != (unsigned short)~data2)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Word Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%04x\nRead  = 0x%04x\n", (unsigned int)ptr2, ~data2, readData2);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		ptr1++;
		ptr2++;
		data1++;
		data2++;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	メモリByte評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		tloop					：検査回数
//		addr					：評価アドレス
//		size					：評価サイズ
//		msg						：評価対象デバイスの文字列
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagMemByte (unsigned int tloop, unsigned int addr, unsigned int size)
{
	volatile unsigned char *pBase1, *pBase2, *ptr1, *ptr2;
	unsigned char data1, data2;
	unsigned char readData1, readData2;
	int loop, loopBase;
	int status = AVAL_STATUS_SUCCESS;

	pBase1   = (volatile unsigned char *)addr;
	pBase2   = (volatile unsigned char *)(addr + (size/2));
	loopBase = size/1/2;

/****************************************/
/* address data (8 bit)                 */
/****************************************/

	//--------------------------------------------------
	// 書き込み
	//--------------------------------------------------
	//キャッシュフラッシュ
	cacheFlush ();

	data1 = 0x00;
	data2 = 0x80;
	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if((loop%(MEMORY_DISPLAY_COUNT*4)) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		*ptr1++ = data1++;
		*ptr2++ = data2++;

	}

	//--------------------------------------------------
	// ｺﾝﾍﾟｱ
	//--------------------------------------------------
	//キャッシュフラッシュ
	cacheFlush ();

	data1 = 0x00;
	data2 = 0x80;
	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if( (loop%(MEMORY_DISPLAY_COUNT*4)) == 0 )
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		readData1 = *ptr1;
		if (readData1 != data1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Byte Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%02x\nRead  = 0x%02x\n", (unsigned int)ptr1, data1, readData1);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		readData2 = *ptr2;
		if (readData2 != data2)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Byte Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%02x\nRead  = 0x%02x\n", (unsigned int)ptr2, data2, readData2);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		ptr1++;
		ptr2++;
		data1++;
		data2++;
	}

/****************************************/
/* address revintse data (8 bit)        */
/****************************************/

	//--------------------------------------------------
	// 書き込み
	//--------------------------------------------------
	//キャッシュフラッシュ
	cacheFlush ();

	data1 = 0x80;
	data2 = 0x00;
	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if( (loop%(MEMORY_DISPLAY_COUNT*4)) == 0 )
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		*ptr1++ = data1--;
		*ptr2++ = data2--;

	}

	//--------------------------------------------------
	// ｺﾝﾍﾟｱ
	//--------------------------------------------------
	//キャッシュフラッシュ
	cacheFlush ();

	data1 = 0x80;
	data2 = 0x00;
	ptr1 = pBase1;
	ptr2 = pBase2;
	loop = loopBase;
	while (loop--)
	{
		if (multiMode == 0)
		{
			if ((loop%(MEMORY_DISPLAY_COUNT*4)) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		readData1 = *ptr1;
		if (readData1 != data1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Byte Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%02x\nRead  = 0x%02x\n", (unsigned int)ptr1, data1, readData1);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		readData2 = *ptr2;
		if (readData2 != data2)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Byte Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%02x\nRead  = 0x%02x\n", (unsigned int)ptr2, data2, readData2);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		ptr1++;
		ptr2++;
		data1--;
		data2--;

	}

_DONE:
	return (status);
}


//**********************************************************************************
//	メモリLong Shift評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		tloop					：検査回数
//		addr					：評価アドレス
//		size					：評価サイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagMemLongShift (unsigned int tloop, unsigned int addr, unsigned int size)
{
	volatile unsigned int *pBase1, *ptr1;
	int loop, loopBase;
	unsigned int shiftData;
	unsigned int readData;
	int status = AVAL_STATUS_SUCCESS;

/****************************************/
/* ｼﾌﾄ                                  */
/****************************************/
	//--------------------------------------------------
	// 書き込み
	//--------------------------------------------------

	//キャッシュフラッシュ
	//@@@1Xil_DCacheFlush ();

	pBase1   = (volatile unsigned int *)addr;
	loopBase = size/4;

	ptr1 = pBase1;
	loop = loopBase;
	shiftData = 0x00000001;
	for ( ; loop--;)
	{
		if (multiMode == 0)
		{
			if ((loop%(MEMORY_DISPLAY_COUNT*2)) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		*ptr1 = (unsigned int)shiftData;
		ptr1++;

		// Shift Data Check
		if (shiftData == 0x80000000)
		    shiftData = 0x00000001;
		else
			shiftData<<=1;
	}

	//--------------------------------------------------
	// ｺﾝﾍﾟｱ
	//--------------------------------------------------

	//キャッシュフラッシュ
	cacheFlush ();

	ptr1 = pBase1;
	loop = loopBase;
	shiftData = 0x00000001;
	for ( ; loop--;)
	{
		if (multiMode == 0)
		{
			if ((loop%(MEMORY_DISPLAY_COUNT*2)) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		readData = *ptr1;
		if (readData != (unsigned int)shiftData)
		{
			sprintf (gLogMsgBuff, "DDR Long Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%08x\nRead  = 0x%08x\n", (unsigned int)ptr1, shiftData, readData);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Shift Data Check
		if (shiftData == 0x80000000)
		    shiftData = 0x00000001;
		else
			shiftData<<=1;

		ptr1++;
	}

/****************************************/
/* ｼﾌﾄ反転                              */
/****************************************/

	//--------------------------------------------------
	// 書き込み
	//--------------------------------------------------

	//キャッシュフラッシュ
	cacheFlush ();

	ptr1 = pBase1;
	loop = loopBase;
	shiftData = 0x80000000;
	for (; loop--;)
	{
		if (multiMode == 0)
		{
			if ((loop%(MEMORY_DISPLAY_COUNT*2)) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		*ptr1 = (unsigned int)~shiftData;
		ptr1++;

		// Shift Data Check
		if (shiftData == 0x00000001)
			shiftData = 0x80000000;
		else
			shiftData>>=1;
	}

	//--------------------------------------------------
	// ｺﾝﾍﾟｱ
	//--------------------------------------------------

	//キャッシュフラッシュ
	cacheFlush ();

	ptr1 = pBase1;
	loop = loopBase;
	shiftData = 0x80000000;
	for (; loop--;)
	{
		if (multiMode == 0)
		{
			if ((loop%(MEMORY_DISPLAY_COUNT*2)) == 0)
			{
				DEBUG_PRINT(".");
				fflush(stdout);
			}
		}

		//TASK_SLICE_CNT(loop);				//Delay(Mult Mode only)

		readData = *ptr1;
		if (readData != (unsigned int)~shiftData)
		{
			sprintf (gLogMsgBuff, "DDR Long Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%08x\nRead  = 0x%08x\n", (unsigned int)ptr1, ~shiftData, readData);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Shift Data Check
		if (shiftData == 0x00000001)
			shiftData = 0x80000000;
		else
			shiftData>>=1;

		ptr1++;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DataLine検査
//----------------------------------------------------------------------------------
//	[ INPUT ]
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagMemDataLine (unsigned int bus)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int addr = 0;
	unsigned int baseAddr = NT5AD_DDR_BASE_ADDR;
	unsigned long long testData = 0;
	unsigned long long readData = 0;

	for(i = 0; i < bus; i++)
	{
		DEBUG_PRINT("BAS:%d Address=0x%lx\n", i, baseAddr + addr);

		// テストデータ作成
		testData = (0x01 << i);

		// データ書き込み
		OUT64((baseAddr + addr), testData);

		// データ読み込み
		readData = IN64((baseAddr + addr));

		// キャッシュフラッシュ
		cacheFlush ();

		// コンペア
		if (testData != readData)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Data Line Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%08x\nRead  = 0x%08x\n", (unsigned int)(baseAddr+addr), (unsigned long long)testData, readData);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		addr+=0x08;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	CAS検査
//----------------------------------------------------------------------------------
//	[ INPUT ]
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagMemCAS (unsigned int cas)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int addr = 0;
	unsigned int baseAddr = NT5AD_DDR_BASE_ADDR;
	unsigned long long testData = 0;
	unsigned long long readData = 0;
	unsigned long long tmpData = 0;

	for(i = 0, tmpData = 0; i < cas; i++, tmpData += 4)
	{
		DEBUG_PRINT("CAS:%d Address=0x%lx\n", i, baseAddr + addr);

		// CAS分のビットシフトアドレスに書き込めるようデータを格納
		testData = (~(tmpData) << 48) & 0xffff000000000000;
		testData |= (~(tmpData + 1) << 32) & 0xffff00000000;
		testData |= (~(tmpData + 2) << 16) & 0xffff0000;
		testData |= (~(tmpData + 3)) & 0xffff;

		// データ書き込み
		OUT64((baseAddr + addr), testData);

		// CASアドレス指定
		addr = (0x08 << i);
	}

	// キャッシュフラッシュ
	cacheFlush ();

	testData = 0;
	readData = 0;
	addr = 0;

	for(i = 0, tmpData = 0; i < cas; i++, tmpData += 4)
	{
		// CAS分のビットシフトアドレスに書き込めるようデータを格納
		testData = (~(tmpData) << 48) & 0xffff000000000000;
		testData |= (~(tmpData + 1) << 32) & 0xffff00000000;
		testData |= (~(tmpData + 2) << 16) & 0xffff0000;
		testData |= (~(tmpData + 3)) & 0xffff;

		// データ書き込み
		readData = IN64((baseAddr + addr));

		// コンペア
		if(readData != testData)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Data Line Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%08x\nRead  = 0x%08x\n", (unsigned int)(baseAddr+addr), (unsigned long long)testData, readData);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// CASアドレス指定
		addr = (0x08 << i);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	RAS検査
//----------------------------------------------------------------------------------
//	[ INPUT ]
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagMemRAS(unsigned int cas, unsigned int ras)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int addr = 0;
	unsigned int baseAddr = NT5AD_DDR_BASE_ADDR;
	unsigned long long testData = 0;
	unsigned long long readData = 0;
	unsigned long long tmpData = 0;

	for(i = 0, tmpData = 0; i < ras;i++, tmpData += 4)
	{
		DEBUG_PRINT("RAS:%d Address=0x%lx\n", i, baseAddr + addr);

		testData = (~(tmpData) << 48) & 0xffff000000000000;
		testData |= (~(tmpData + 1) << 32) & 0xffff00000000;
		testData |= (~(tmpData + 2) << 16) & 0xffff0000;
		testData |= (~(tmpData + 3)) & 0xffff;

		//データ書き込み
		OUT64((baseAddr + addr), testData);

		addr = (0x01 << cas) * (0x01 << i);
	}

	//キャッシュフラッシュ
	cacheFlush ();

	testData = 0;
	addr = 0;

	for(i = 0, tmpData = 0; i < ras; i++, tmpData += 4)
	{
		testData = (~(tmpData) << 48) & 0xffff000000000000;
		testData |= (~(tmpData + 1) << 32) & 0xffff00000000;
		testData |= (~(tmpData + 2) << 16) & 0xffff0000;
		testData |= (~(tmpData + 3)) & 0xffff;

		//データ読み込み
		readData = IN64((baseAddr + addr));

		// コンペア
		if(readData != testData)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Data Line Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%08x\nRead  = 0x%08x\n", (unsigned int)(baseAddr+addr), (unsigned long long)testData, readData);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		addr = (0x01 << cas) * (0x01 << i);
	}

_DONE:
	return (status);

}


//**********************************************************************************
//	Bank検査
//----------------------------------------------------------------------------------
//	[ INPUT ]
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagMemBank (unsigned int bank)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int addr = 0;
	unsigned int baseAddr = NT5AD_DDR_BASE_ADDR;
	unsigned long checkBank = 0;
	unsigned long long testData = 0;
	unsigned long long readData = 0;
	unsigned long long tmpData = 0;

	for(i = 0, tmpData = 0; i < bank;i++, tmpData += 4)
	{
		DEBUG_PRINT("Bank:%d Address=0x%lx\n", i, baseAddr + addr);

		testData = (~(tmpData) << 48) & 0xffff000000000000;
		testData |= (~(tmpData + 1) << 32) & 0xffff00000000;
		testData |= (~(tmpData + 2) << 16) & 0xffff0000;
		testData |= (~(tmpData + 3)) & 0xffff;

		checkBank = 0x01 << i;

		// データ書き込み
		OUT64((baseAddr + addr), testData);

		// 総サイズ / 総バンク数
		addr = ((0x80000000/4) * checkBank);
	}

	// キャッシュフラッシュ
	cacheFlush ();

	testData = 0;
	addr = 0;

	for(i = 0, tmpData = 0; i < bank; i++, tmpData += 4)
	{
		testData = (~(tmpData) << 48) & 0xffff000000000000;
		testData |= (~(tmpData + 1) << 32) & 0xffff00000000;
		testData |= (~(tmpData + 2) << 16) & 0xffff0000;
		testData |= (~(tmpData + 3)) & 0xffff;

		checkBank = 0x01 << i;

		// データ読み込み
		readData = IN64((baseAddr + addr));

		if(readData != testData)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "DDR Data Line Compare Error.\nAdrs  = 0x%lx\nWrite = 0x%08x\nRead  = 0x%08x\n", (unsigned int)(baseAddr+addr), (unsigned long long)testData, readData);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__,  __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// 総サイズ / 総バンク数
		addr = ((0x80000000/4) * checkBank);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DDR Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：0=通常検査モード/1=Agingモード(mallocで確保した領域のみ検査)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int diagDdrSetMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Mode設定
	OUT32 (FIRM_DATA_DIAG_DDR_MODE, mode);

	return (status);
}


//**********************************************************************************
//	DDR Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int diagDdrGetMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_BOARD, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Diag Ddr pMode Parameter Error\n");
		goto _DONE;
	}

	// Mode取得
	*pMode = IN32 (FIRM_DATA_DIAG_DDR_MODE);

_DONE:
	return (status);
}

// eof
