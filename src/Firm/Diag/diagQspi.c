//**********************************************************************************
//
//                                 Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// diagQspi.c - QSPI Flash Test
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
// Erase評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiErase (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int tloop, ix;
	unsigned int size = QSPI_FLASH_SIZE;
	unsigned int secSize = QSPI_FLASH_SEC_SIZE;
	unsigned int sec=0;
	unsigned int loop;

	// ループ回数取得
	loop = atoi ((char *)str);

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Flash Memory Request Error.\n");
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("--------------------loop = %d--------------------\n", tloop);

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("sec = 0x%x size = 0x%x  \n", sec, secSize);

			// Erase
			if ((status = qspiFlashSectorErase (sec)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Erase Error. Adrs = %d\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}

		DEBUG_PRINT("\n");

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("verify..sec = 0x%x size = 0x%x \n", sec, secSize);
			fflush (stdout);

			// Read
			if ((status = qspiFlashRead (sec, (unsigned char *)pBuff, QSPI_FLASH_SEC_SIZE)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Read Error. Adrs = 0x%x\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// コンペア
			for (ix=0; ix<(secSize/4); ix++)
			{
				if (pBuff[ix] != 0xffffffff)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Flash Blank Check Error.\nAdrs = 0x%x\nOffset = 0x%x\nData = 0x%x\n", sec, ix, pBuff[ix]);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	if (pBuff != NULL)
		free (pBuff);

	return (status);
}


//**********************************************************************************
// Erase評価(N25q256)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiEraseAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int tloop, ix;
	unsigned int size = QSPI_FLASH_SIZE;
	unsigned int secSize = QSPI_FLASH_SEC_SIZE;
	unsigned int sec;
	unsigned int loop;

	// ループ回数取得
	loop = atoi ((char *)str);

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Flash Memory Request Error.\n");
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("--------------------loop = %d--------------------\n", tloop);

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		if ((status = qspiFlashEraseAll ()) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
			sprintf (gLogMsgBuff, "Flash Erase Error. Adrs = %d\n", sec);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		DEBUG_PRINT("\n");

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("verify..sec = 0x%x size = 0x%x \n", sec, secSize);
			fflush (stdout);

			// Read
			if ((status = qspiFlashRead (sec, (unsigned char *)pBuff, QSPI_FLASH_SEC_SIZE)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Read Error. Adrs = 0x%x\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// コンペア
			for (ix=0; ix<(secSize/4); ix++)
			{
				if (pBuff[ix] != 0xffffffff)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Flash Blank Check Error.\nAdrs = 0x%x\nOffset = 0x%x\nData = 0x%x\n", sec, ix, pBuff[ix]);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	if (pBuff != NULL)
		free (pBuff);

	return (status);
}


//**********************************************************************************
//	Flash Read/Write評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiFlashRW (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int *pBuffWrite = NULL;
	unsigned int tloop, ix;
	unsigned int cmpData;
	unsigned int count;
	//unsigned int size = QSPI_FLASH_SIZE;
	unsigned int size = QSPI_FLASH_SEC_SIZE;
	unsigned int secSize = QSPI_FLASH_SEC_SIZE;
	unsigned int sec, second;
	unsigned int loop;

	// ループ回数取得
	loop = atoi ((char *)str);

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(secSize)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Flash Memory Request Error.\n");
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("-------------------- loop = %d--------------------\n", tloop);

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("Erase sec = 0x%x size = 0x%x  \n", sec, secSize);
			fflush (stdout);

			// Erase
			if ((status = qspiFlashSectorErase (sec)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Erase Error. Adrs = %d\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}

		DEBUG_PRINT("\n");

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("verify..sec = 0x%x size = 0x%x \n", sec, secSize);
			fflush (stdout);

			// Read
			if ((status = qspiFlashRead (sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Read Error. Adrs = 0x%x\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// コンペア
			for (ix=0; ix<(secSize/4); ix++)
			{
				if (pBuff[ix] != 0xffffffff)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Flash Blank Check Error.\nAdrs  = 0x%x\nOffset = 0x%x\nData = 0x%x\n", sec, ix, pBuff[ix]);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}
		second = timerGetSec();
		DEBUG_PRINT ("Sec = %d\n", second);
		DEBUG_PRINT("\n");

		//--------------------------------------------------
		// Write
		//--------------------------------------------------
		count = tloop;
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT ("Write..sec = 0x%x size = 0x%x \n", sec, secSize);
			fflush (stdout);

			// Data make
			pBuffWrite = (unsigned int *)pBuff;
			for (ix=0; ix<(secSize/4); ix++, count++)
				pBuffWrite[ix] = count;

			// Write
			if ((status = qspiFlashWrite (sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Write Error. Adrs = 0x%x\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}

		second = timerGetSec();
		DEBUG_PRINT ("Sec = %d\n", second);
		DEBUG_PRINT("\n");

		//--------------------------------------------------
		// Read
		//--------------------------------------------------
		cmpData = tloop;
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT ("Read..sec = 0x%x size = 0x%x \n", sec, secSize);
			fflush (stdout);

			// Read
			if ((status = qspiFlashRead (sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Read Error. Adrs = 0x%x\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// コンペア
			pBuffWrite = (unsigned int *)pBuff;
			for (ix=0; ix<(secSize/4); ix++, cmpData++)
			{
				if (pBuff[ix] != cmpData)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Flash Compare Error.\nAdrs = 0x%x\nOffset = 0x%x\nWrite = 0x%x\nRead = 0x%x\n", sec, ix, cmpData, pBuff[ix]);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}
		
		second = timerGetSec();
		DEBUG_PRINT ("Sec = %d\n", second);
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	if (pBuff != NULL)
		free (pBuff);

	return (status);
}


//**********************************************************************************
//	Flash Read/Write評価(１度で全領域アクセス)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiFlashRWAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int *pBuffWrite = NULL;
	unsigned int tloop, ix;
	unsigned int cmpData;
	unsigned int count;
	unsigned int size = QSPI_FLASH_SIZE;
	unsigned int loop;

	// ループ回数取得
	loop = atoi ((char *)str);

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Flash Memory Request Error.\n");
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("-------------------- loop = %d--------------------\n", tloop);

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		DEBUG_PRINT("Erase..\n");
		if ((status = qspiFlashEraseAll ()) != AVAL_STATUS_SUCCESS)
		{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Flash Erase Error.\n");
				goto _DONE;
		}

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		DEBUG_PRINT("verify..\n");

		// Read
		if ((status = qspiFlashRead (0, (unsigned char *)pBuff, size)) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Read Error.\n");
			goto _DONE;
		}

		// コンペア
		for (ix=0; ix<(size/4); ix++)
		{
			if (pBuff[ix] != 0xffffffff)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "Flash Blank Check Error.\nAdrs = 0x%x\nData = 0x%x\n", ix, pBuff[ix]);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}

		//--------------------------------------------------
		// Write
		//--------------------------------------------------
		count = tloop;
		DEBUG_PRINT ("Write..\n");
		fflush (stdout);

		// Data make
		pBuffWrite = (unsigned int *)pBuff;
		for (ix=0; ix<(size/4); ix++, count++)
			pBuffWrite[ix] = count;

		// Write
		if ((status = qspiFlashWrite (0, (unsigned char *)pBuff, size)) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Write Error.\n");
			goto _DONE;
		}

		//--------------------------------------------------
		// Read
		//--------------------------------------------------
		cmpData = tloop;
		DEBUG_PRINT ("Read..\n");

		// read
		if ((status = qspiFlashRead (0, (unsigned char *)pBuff, size)) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Read Error.\n");
			goto _DONE;
		}

		// コンペア
		pBuffWrite = (unsigned int *)pBuff;
		for (ix=0; ix<(size/4); ix++, cmpData++)
		{
			if (pBuff[ix] != cmpData)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "Flash Compare Error.\nAdrs = 0x%x\nWrite  = 0x%x\nRead = 0x%x\n", ix, cmpData, pBuff[ix]);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	if (pBuff != NULL)
		free (pBuff);

	return (status);
}


//**********************************************************************************
//	Flash Read/Write評価(ランダム)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiFlashRWRandom (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int *pBuffWrite = NULL;
	unsigned int tloop, ix;
	unsigned int cmpData;
	unsigned int count;
	unsigned int size = QSPI_FLASH_SIZE;
	unsigned int adrs, size2;
	unsigned char *pBuffB, *pCmpBuffB;
	unsigned int loop;
	unsigned int sec;
	unsigned int secSize = QSPI_FLASH_SEC_SIZE;

	// ループ回数取得
	loop = atoi ((char *)str);

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Flash Memory Request Error.\n");
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("-------------------- loop = %d--------------------\n", tloop);


		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		DEBUG_PRINT("Erase..\n");
		for (sec = 0; sec < size; sec += secSize)
		{
			// Erase
			if ((status = qspiFlashSectorErase (sec)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Erase Error. Adrs = %d\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		DEBUG_PRINT("verify..\n");

		// Read
		if ((status = qspiFlashRead (0, (unsigned char *)pBuff, size)) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Read Error.\n");
			goto _DONE;
		}

		// コンペア
		for (ix=0; ix<(size/4); ix++)
		{
			if (pBuff[ix] != 0xffffffff)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "Flash Blank Check Error.\nAdrs = 0x%x\nData = 0x%x\n", ix, pBuff[ix]);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}

		//--------------------------------------------------
		// Write
		//--------------------------------------------------
		count = tloop;

		// 書き込みアドレスをランダムに
		adrs = rand() % size;
		size2 = rand() % size;

		if ((adrs + size2) > size)
			size2 = size - adrs;

		DEBUG_PRINT ("Write..Adrs = 0x%x size = 0x%x\n", adrs, size2);

		// Data make
		pBuffWrite = (unsigned int *)pBuff;
		for (ix=0; ix<(size/4); ix++, count++)
			pBuffWrite[ix] = count;

		// Write
		if ((status = qspiFlashWrite (adrs, (unsigned char *)pBuff, size2)) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Write Error.\n");
			goto _DONE;
		}

		//--------------------------------------------------
		// Read
		//--------------------------------------------------
		cmpData = tloop;
		DEBUG_PRINT ("Read..\n");

		// read
		if ((status = qspiFlashRead (adrs, (unsigned char *)pBuff, size2)) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Read Error.\n");
			goto _DONE;
		}

		// コンペア
		for (ix=0; ix<(size2/4); ix++, cmpData++)
		{
			if (pBuff[ix] != cmpData)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "Flash Compare Error.\nAdrs = 0x%x\nWrite= 0x%x\nRead = 0x%x\n", ix, cmpData, pBuff[ix]);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}

		if ((size2%4) != 0)
		{
			pBuffB = (unsigned char *)&pBuff[ix];
			pCmpBuffB = (unsigned char *)&cmpData;
			for (ix=0; ix<(size2%4); ix++)
			{
				if (pBuffB[ix] != pCmpBuffB[ix])
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Flash Compare Error.\nAdrs = 0x%x\nWrite  = 0x%x\nRead = 0x%x\n", ix, pCmpBuffB[ix], pBuffB[ix]);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	if (pBuff != NULL)
		free (pBuff);

	return (status);
}


//**********************************************************************************
//	QSPI Flash評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspi (void *str)
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
			cmdDiagQspiHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Diagステート
		saveLed1 = ledDiagnosticState ();

		// 検査
		status = cmdDiagQspiFlashRWSector (gCmdArg[1]);

		// LEDの設定を元に戻す
		ledReturnState (-1, saveLed1);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);

}


//**********************************************************************************
//	QSPI Flash Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A qspi flash check is performed.\n");
	DEBUG_PRINT_FORCE ("  Command           : diagqspi [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Check Number of times\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Flash Read/Write評価(指定セクタのみをアクセス)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiFlashRWSector (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pBuff = NULL;
	unsigned char *pBuffWrite = NULL;
	unsigned int tloop, ix;
	unsigned char cmpData;
	unsigned int count;
	unsigned int size = QSPI_FLASH_SEC_SIZE;
	unsigned int secSize = QSPI_FLASH_SEC_SIZE;
	int lastSec = QSPI_FLASH_SIZE/QSPI_FLASH_SEC_SIZE - 1;
	unsigned int sec, startAdrs;
	unsigned int loop;

	// 検査回数取得
	if (sscanf (str, "%d", &loop) != 1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

	// メモリリクエスト
	if ((pBuff = (unsigned char *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Flash Memory Request Error.\n");
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("-------------------- loop = %d--------------------\n", tloop);

		startAdrs = lastSec*QSPI_FLASH_SEC_SIZE;

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		for (sec = startAdrs; sec <(startAdrs + size); sec += secSize)
		{
			DEBUG_PRINT("[Erase]  Adrs=0x%x / Size=0x%x\n", sec, secSize);
			fflush (stdout);

			// Erase
			if ((status = qspiFlashSectorErase (sec)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Erase Error. Adrs = %d\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		for (sec = startAdrs; sec <(startAdrs + size); sec += secSize)
		{
			DEBUG_PRINT("[Verify] Adrs=0x%x / Size=0x%x\n", sec, secSize);
			fflush (stdout);

			// Read
			if ((status = qspiFlashRead (sec, (unsigned char *)pBuff, QSPI_FLASH_SEC_SIZE)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Read Error. Adrs = 0x%x\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// コンペア
			for (ix=0; ix<secSize; ix++)
			{
				if (pBuff[ix] != 0xff)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Flash Blank Check Error.\nAdrs = 0x%x\nOffset = 0x%x\nData = 0x%x\n", sec, ix, pBuff[ix]);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}

		//--------------------------------------------------
		// Write
		//--------------------------------------------------
		count = tloop;
		for (sec = startAdrs; sec <(startAdrs + size); sec += secSize)
		{
			DEBUG_PRINT ("[Write]  Adrs=0x%x / Size=0x%x\n", sec, secSize);
			fflush (stdout);

			// Data make
			pBuffWrite = (unsigned char *)pBuff;
			for (ix=0; ix<secSize; ix++, count++)
				pBuffWrite[ix] = count;

			// Write
			if ((status = qspiFlashWrite (sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Write Error. Adrs = 0x%x\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}

		//--------------------------------------------------
		// Read
		//--------------------------------------------------
		cmpData = tloop;
		for (sec = startAdrs; sec <(startAdrs + size); sec += secSize)
		{
			DEBUG_PRINT ("[Read]   Adrs=0x%x / Size=0x%x\n", sec, secSize);
			fflush (stdout);

			// read
			if ((status = qspiFlashRead (sec, (unsigned char *)pBuff, QSPI_FLASH_SEC_SIZE)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Read Error. Adrs = 0x%x\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// コンペア
			pBuffWrite = (unsigned char *)pBuff;
			for (ix=0; ix<secSize; ix++, cmpData++)
			{
				if (pBuff[ix] != cmpData)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Flash Compare Error.\nAdrs = 0x%x\nOffset = 0x%x\nWrit = 0x%x\nRead = 0x%x\n", sec, ix, cmpData, pBuff[ix]);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	//--------------------------------------------------
	// Read(バンク切り替え用途)
	//--------------------------------------------------
	if ((status = qspiFlashRead (0, (unsigned char *)pBuff, 4)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Read Error.\n");
	}

	if (pBuff != NULL)
		free (pBuff);

	return (status);
}


//**********************************************************************************
// Blanck Check評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiBlankCheck (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int tloop, ix;
	unsigned int size = QSPI_FLASH_SIZE;
	unsigned int secSize = QSPI_FLASH_SEC_SIZE;
	unsigned int sec;
	unsigned int loop;

	// ループ回数取得
	loop = atoi ((char *)str);

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,__func__, __LINE__, status, "Flash Memory Request Error.\n");
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("--------------------loop = %d--------------------\n", tloop);

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("verify..sec = 0x%x size = 0x%x \n", sec, secSize);
			fflush (stdout);

			// Read
			if ((status = qspiFlashRead (sec, (unsigned char *)pBuff, QSPI_FLASH_SEC_SIZE)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_IO);
				sprintf (gLogMsgBuff, "Flash Read Error. Adrs = 0x%x\n", sec);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// コンペア
			for (ix=0; ix<(secSize/4); ix++)
			{
				if (pBuff[ix] != 0xffffffff)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Flash Blank Check Error.\nAdrs = 0x%x\nOffset = 0x%x\nData = 0x%x\n", sec, ix, pBuff[ix]);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	if (pBuff != NULL)
		free (pBuff);

	return (status);
}


//**********************************************************************************
//	Flash Write Protect
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiWriteProtect (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDiagQspiWriteProtectHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// コンソールモード
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Protect Mode
		if ((mode != FLASH_WRITE_PROTECT) && (mode != FLASH_WRITE_PROTECT_CANCEL))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Protect Mode(%d) Parameter Error. (Protect:%d / Protect Cancel:%d)\n", mode, FLASH_WRITE_PROTECT, FLASH_WRITE_PROTECT_CANCEL);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Protect?
		if (mode == FLASH_WRITE_PROTECT)
			mode = FLASH_WRITE_PROTECT_ADRS;

		// Write  Protect Mode設定
		if ((status = qspiFlashWriteProtect (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FLASH, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Flash Write Protect Help(Ver.1.1)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiWriteProtectHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Write protection of Flash is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : qspiflashprotect [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Protect Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Protect Mode]\n");
	DEBUG_PRINT_FORCE ("  %3d : Write Protect\n", FLASH_WRITE_PROTECT);
	DEBUG_PRINT_FORCE ("  %3d : Write Protect Cancel\n", FLASH_WRITE_PROTECT_CANCEL);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Flash Write Protect Cancel
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiWriteProtectCancel (void *str)
{
	qspiFlashWriteProtect (0);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Flash Write Protect Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagQspiWriteProtectStatus (void *str)
{
	int status;

	// Write Protect Status Show
	if ((status = qspiFlashWriteProtectReadShow ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (AVAL_STATUS_SUCCESS);
}

// eof
