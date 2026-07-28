//**********************************************************************************
//
//                                 Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdDiagFlash.c - Flash Test
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
// Flash Size取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		devType					：デバイスタイプ
//		pSize					：Flash All Sizeを格納するポインタ
//		pSecSize				：Flash Sector Sizeを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int devGetFlashSize (int devType, unsigned int *pSize, unsigned int *pSecSize)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Device Erase pSize NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pSecSize Parameter
	if (pSecSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Device Erase pSecSize NULL Parameter Error\n");
		goto _DONE;
	}

	//------------------------------------------------------------
	// QSPI Flash
	//------------------------------------------------------------
	if (devType == FLASH_DEV_TYPE_QSPI_FLASH)
	{
		*pSize = QSPI_FLASH_SIZE;
		*pSecSize = QSPI_FLASH_SEC_SIZE;
	}
	//------------------------------------------------------------
	// Other
	//------------------------------------------------------------
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Device Erase devType(%d) Parameter Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Erase Sector
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		devType					：デバイスタイプ
//		sector					：Sector
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int devFlashEraseSector (int devType, int sector)
{
	int status = AVAL_STATUS_SUCCESS;

	//------------------------------------------------------------
	// QSPI Flash
	//------------------------------------------------------------
	if (devType == FLASH_DEV_TYPE_QSPI_FLASH)
	{
		// Erase
		if ((status = qspiFlashSectorErase (sector)) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Erase Sector Error\n");
			goto _DONE;
		}
	}
	//------------------------------------------------------------
	// Other
	//------------------------------------------------------------
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Device Erase Sector devType(%d) Parameter Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Erase All
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		devType					：デバイスタイプ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int devFlashEraseAll (int devType)
{
	int status = AVAL_STATUS_SUCCESS;

	//------------------------------------------------------------
	// QSPI Flash
	//------------------------------------------------------------
	if (devType == FLASH_DEV_TYPE_QSPI_FLASH)
	{
		// Erase
		if ((status = qspiFlashEraseAll ()) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Erase All Error\n");
			goto _DONE;
		}
	}
	//------------------------------------------------------------
	// Other
	//------------------------------------------------------------
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Device Erase All devType(%d) Parameter Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Flash Read
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		devType					：デバイスタイプ
//		adrs					：アドレス
//		pData					：データを格納するポインタ
//		size					：Read Size
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int devFlashRead (int devType, unsigned int adrs, unsigned char *pData, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Device Flash Read pData NULL Parameter Error\n");
		goto _DONE;
	}

	//------------------------------------------------------------
	// QSPI Flash
	//------------------------------------------------------------
	if (devType == FLASH_DEV_TYPE_QSPI_FLASH)
	{
		// Read
		if ((status = qspiFlashRead (adrs, (unsigned char *)pData, size)) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Read Error\n");
			goto _DONE;
		}
	}
	//------------------------------------------------------------
	// Other
	//------------------------------------------------------------
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Device Flash Read devType(%d) Parameter Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Flash Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		devType					：デバイスタイプ
//		adrs					：アドレス
//		pData					：データを格納するポインタ
//		size					：Read Size
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int devFlashWrite (int devType, unsigned int adrs, unsigned char *pData, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Device Flash Write pData NULL Parameter Error\n");
		goto _DONE;
	}

	//------------------------------------------------------------
	// QSPI Flash
	//------------------------------------------------------------
	if (devType == FLASH_DEV_TYPE_QSPI_FLASH)
	{
		// Read
		if ((status = qspiFlashWrite (adrs, (unsigned char *)pData, size)) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "QSPI Flash Write Error\n");
			goto _DONE;
		}
	}
	//------------------------------------------------------------
	// Other
	//------------------------------------------------------------
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Device Flash Read devType(%d) Parameter Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Erase
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//		devType					：デバイスタイプ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int diagFlashEraseSector (void *str, int devType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int tloop, ix;
	unsigned int size;
	unsigned int secSize;
	unsigned int sec;
	unsigned int loop;

	// ループ回数取得
	loop = atoi ((char *)str);

	// Eraseサイズ取得
	if ((status = devGetFlashSize (devType, &size, &secSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Erase Memory Request Error\n");
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("--------------------loop = %d--------------------\n", tloop);

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		DEBUG_PRINT("Erase..\n");
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("sec = 0x%x size = 0x%x  \n", sec, secSize);

			if ((status = devFlashEraseSector (devType, sec)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}


		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("verify..sec = 0x%x size = 0x%x \n", sec, secSize);

			// Data clear
			for (ix=0; ix<(secSize/4); ix++)
				pBuff[ix] = 0;

			// Read
			if ((status = devFlashRead (devType, sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// コンペア
			for (ix=0; ix<(secSize/4); ix++)
			{
				if (pBuff[ix] != 0xffffffff)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Diag Flash Erase Blank Check Error\nAdrs   = 0x%x\nOffset = 0x%x\nData   = 0x%x\n", sec, ix*4, pBuff[ix]);
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
// Erase All
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//		devType					：デバイスタイプ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int diagFlashEraseAll (void *str, int devType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int tloop, ix;
	unsigned int size;
	unsigned int secSize;
	unsigned int sec;
	unsigned int loop;

	// ループ回数取得
	loop = atoi ((char *)str);

	// Eraseサイズ取得
	if ((status = devGetFlashSize (devType, &size, &secSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash Erase All Memory Request Error\n");
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("--------------------loop = %d--------------------\n", tloop);

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		if ((status = devFlashEraseAll (devType)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("verify..sec = 0x%x size = 0x%x \n", sec, secSize);

			// Data clear
			for (ix=0; ix<(secSize/4); ix++)
				pBuff[ix] = 0;

			// Read
			if ((status = devFlashRead (devType, sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// コンペア
			for (ix=0; ix<(secSize/4); ix++)
			{
				if (pBuff[ix] != 0xffffffff)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Diag Flash All Erase Blank Check Error\nAdrs   = 0x%x\nOffset = 0x%x\nData   = 0x%x\n", sec, ix*4, pBuff[ix]);
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
//		devType					：デバイスタイプ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int diagFlashRW (void *str, int devType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int *pBuffWrite = NULL;
	unsigned int tloop, ix;
	unsigned int cmpData;
	unsigned int count;
	unsigned int size;
	unsigned int secSize;
	unsigned int sec;
	unsigned int loop;

	// ループ回数取得
	loop = atoi ((char *)str);

	// Flashサイズ取得
	if ((status = devGetFlashSize (devType, &size, &secSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "Flash RW Memory Request Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("--------------------loop = %d--------------------\n", tloop);

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		DEBUG_PRINT("Erase..\n");
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("sec = 0x%x size = 0x%x  \n", sec, secSize);

			if ((status = devFlashEraseSector (devType, sec)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT("verify..sec = 0x%x size = 0x%x \n", sec, secSize);

			// Data clear
			for (ix=0; ix<(secSize/4); ix++)
				pBuff[ix] = 0;

			// Read
			if ((status = devFlashRead (devType, sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// コンペア
			for (ix=0; ix<(secSize/4); ix++)
			{
				if (pBuff[ix] != 0xffffffff)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Flash Erase Blank Check Error\nAdrs   = 0x%x\nOffset = 0x%x\nData   = 0x%x\n", sec, ix*4, pBuff[ix]);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			}
		}


		//--------------------------------------------------
		// Write
		//--------------------------------------------------
		count = tloop;
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT ("Write..sec = 0x%x size = 0x%x \n", sec, secSize);

			// Data make
			pBuffWrite = (unsigned int *)pBuff;
			for (ix=0; ix<(secSize/4); ix++, count++)
				pBuffWrite[ix] = count;

			// Write
			if ((status = devFlashWrite (devType, sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		//--------------------------------------------------
		// Read
		//--------------------------------------------------
		cmpData = tloop;
		for (sec = 0; sec < size; sec += secSize)
		{
			DEBUG_PRINT ("Read..sec = 0x%x size = 0x%x \n", sec, secSize);

			// Data clear
			for (ix=0; ix<(secSize/4); ix++)
				pBuff[ix] = 0;

			// Read
			if ((status = devFlashRead (devType, sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// コンペア
			pBuffWrite = (unsigned int *)pBuff;
			for (ix=0; ix<(secSize/4); ix++, cmpData++)
			{
				if (pBuff[ix] != cmpData)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Diag Flash Compare Error\nAdrs   = 0x%x\nOffset = 0x%x\nWrite Data = 0x%x\nRead  Data = 0x%x\n", sec, ix*4, cmpData, pBuff[ix]);
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
//	Flash Read/Write評価(１度で全領域アクセス)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//		devType					：デバイスタイプ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int diagFlashRWAll (void *str, int devType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int *pBuffWrite = NULL;
	unsigned int tloop, ix;
	unsigned int cmpData;
	unsigned int count;
	unsigned int size, secSize;
	unsigned int loop;

	// ループ回数取得
	loop = atoi ((char *)str);

	// Flashサイズ取得
	if ((status = devGetFlashSize (devType, &size, &secSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Flash RW All Memory Request Error\n");
		goto _DONE;
	}


MORE:
	for (tloop=0; tloop<loop; tloop++)
	{

		DEBUG_PRINT("--------------------loop = %d--------------------\n", tloop);

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		DEBUG_PRINT("Erase..\n");
		if ((status = devFlashEraseAll (devType)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		DEBUG_PRINT("verify.. size = 0x%x \n", size);

		// Data clear
		for (ix=0; ix<(secSize/4); ix++)
			pBuff[ix] = 0;

		// Read
		if ((status = devFlashRead (devType, 0, (unsigned char *)pBuff, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// コンペア
		for (ix=0; ix<(size/4); ix++)
		{
			if (pBuff[ix] != 0xffffffff)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "Diag Flash Erase Blank Check Error\nOffset = 0x%x\nData   = 0x%x\n", ix*4, pBuff[ix]);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}

		//--------------------------------------------------
		// Write
		//--------------------------------------------------
		count = tloop;
		DEBUG_PRINT ("Write..\n");

		// Data make
		pBuffWrite = (unsigned int *)pBuff;
		for (ix=0; ix<(size/4); ix++, count++)
			pBuffWrite[ix] = count;

		// Write
		if ((status = devFlashWrite (devType, 0, (unsigned char *)pBuff, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//--------------------------------------------------
		// Read
		//--------------------------------------------------
		cmpData = tloop;
		DEBUG_PRINT ("Read..\n");

		// Data clear
		for (ix=0; ix<(size/4); ix++)
			pBuff[ix] = 0;

		// Read
		if ((status = devFlashRead (devType, 0, (unsigned char *)pBuff, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// コンペア
		for (ix=0; ix<(size/4); ix++, cmpData++)
		{
			if (pBuff[ix] != cmpData)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "Diag Flash Compare Error\nOffset = 0x%x\nWrite Data = 0x%x\nRead  Data = 0x%x\n", ix*4, cmpData, pBuff[ix]);
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
//		devType					：デバイスタイプ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int diagFlashRWRandom (void *str, int devType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int *pBuffWrite = NULL;
	unsigned int tloop, ix;
	unsigned int cmpData;
	unsigned int count;
	unsigned int size;
	unsigned int adrs, size2;
	unsigned char *pBuffB, *pCmpBuffB;
	unsigned int loop;
	unsigned int secSize;

	// ループ回数取得
	loop = atoi ((char *)str);

	// Flashサイズ取得
	if ((status = devGetFlashSize (devType, &size, &secSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "Flash RW Random Memory Request Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("--------------------loop = %d--------------------\n", tloop);

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		DEBUG_PRINT("Erase..\n");
		if ((status = devFlashEraseAll (devType)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		DEBUG_PRINT("verify.. size = 0x%x \n", size);

		// Data clear
		for (ix=0; ix<(size/4); ix++)
			pBuff[ix] = 0;

		// Read
		if ((status = devFlashRead (devType, 0, (unsigned char *)pBuff, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// コンペア
		for (ix=0; ix<(size/4); ix++)
		{
			if (pBuff[ix] != 0xffffffff)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "Diag Flash Erase Blank Check Error\nOffset = 0x%x\nData   = 0x%x\n", ix*4, pBuff[ix]);
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
		if ((status = devFlashWrite (devType, adrs, (unsigned char *)pBuff, size2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//--------------------------------------------------
		// Read
		//--------------------------------------------------
		cmpData = tloop;
		DEBUG_PRINT ("Read..\n");

		// Data clear
		for (ix=0; ix<(size2/4); ix++)
			pBuff[ix] = 0;

		// Read
		if ((status = devFlashRead (devType, adrs, (unsigned char *)pBuff, size2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// コンペア
		for (ix=0; ix<(size2/4); ix++, cmpData++)
		{
			if (pBuff[ix] != cmpData)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "Diag Flash Compare Error\nOffset = 0x%x\nWrite Data = 0x%x\nRead  Data = 0x%x\n", ix*4, cmpData, pBuff[ix]);
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
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Diag Flash Compare Error\nOffset = 0x%x\nWrite Data = 0x%x\nRead  Data = 0x%x\n", ix, pCmpBuffB[ix], pBuffB[ix]);
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
//	Flash Read/Write評価(指定セクタのみをアクセス)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//		devType					：デバイスタイプ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int diagFlashRWSector (void *str, int devType)
{
	int status = AVAL_STATUS_SUCCESS;

	unsigned char *pBuff = NULL;
	unsigned char *pBuffWrite = NULL;
	unsigned int tloop, ix;
	unsigned char cmpData;
	unsigned int count;
	unsigned int size;
	unsigned int secSize;
	int lastSec;
	unsigned int sec, startAdrs;
	unsigned int loop;

	// Flashサイズ取得
	if ((status = devGetFlashSize (devType, &size, &secSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最終セクタ
	lastSec = size/secSize - 1;

	// 検査回数取得
	if (sscanf (str, "%d", &loop) != 1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

	// メモリリクエスト
	if ((pBuff = (unsigned char *)malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "Flash RW Memory Request Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{

		DEBUG_PRINT("--------------------loop = %d--------------------\n", tloop);

		// 最終セクタアドレス
		startAdrs = lastSec * secSize;

		//--------------------------------------------------
		// Erase
		//--------------------------------------------------
		for (sec = startAdrs; sec <(startAdrs + size); sec += secSize)
		{
			DEBUG_PRINT("[Erase]  Adrs=0x%x / Size=0x%x\n", sec, secSize);

			// Erase
			if ((status = devFlashEraseSector (devType, sec)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		//--------------------------------------------------
		// Blank Check
		//--------------------------------------------------
		for (sec = startAdrs; sec <(startAdrs + size); sec += secSize)
		{
			DEBUG_PRINT("[Verify] Adrs=0x%x / Size=0x%x\n", sec, secSize);

			// Data clear
			for (ix=0; ix<(secSize/4); ix++)
				pBuff[ix] = 0;

			// Read
			if ((status = devFlashRead (devType, sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// コンペア
			for (ix=0; ix<secSize; ix++)
			{
				if (pBuff[ix] != 0xff)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff,  "Flash Erase Blank Check Error\nAdrs   = 0x%x\nOffset = 0x%x\nData   = 0x%x\n", sec, ix, pBuff[ix]);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
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

			// Data make
			pBuffWrite = (unsigned char *)pBuff;
			for (ix=0; ix<secSize; ix++, count++)
				pBuffWrite[ix] = count;

			// Write
			if ((status = devFlashWrite (devType, sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		//--------------------------------------------------
		// Read
		//--------------------------------------------------
		cmpData = tloop;
		for (sec = startAdrs; sec <(startAdrs + size); sec += secSize)
		{
			DEBUG_PRINT ("[Read]   Adrs=0x%x / Size=0x%x\n", sec, secSize);

			// Data clear
			for (ix=0; ix<(secSize/4); ix++)
				pBuff[ix] = 0;

			// Read
			if ((status = devFlashRead (devType, sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// コンペア
			pBuffWrite = (unsigned char *)pBuff;
			for (ix=0; ix<secSize; ix++, cmpData++)
			{
				if (pBuff[ix] != cmpData)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Diag Flash Compare Error\nAdrs   = 0x%x\nOffset = 0x%x\nWrite Data = 0x%x\nRead  Data = 0x%x\n", sec, ix*4, cmpData, pBuff[ix]);
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
	devFlashRead (devType, 0, (unsigned char *)pBuff, 4);

	if (pBuff != NULL)
		free (pBuff);

	return (status);
}


//**********************************************************************************
// Blank Check評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//		devType					：デバイスタイプ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int diagFlashBlankCheck (void *str, int devType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuff = NULL;
	unsigned int tloop, ix;
	unsigned int size;
	unsigned int secSize;
	unsigned int sec;
	unsigned int loop;

	// ループ回数取得
	loop = atoi ((char *)str);

	// Flashサイズ取得
	if ((status = devGetFlashSize (devType, &size, &secSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// メモリリクエスト
	if ((pBuff = (unsigned int *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "Flash Erase Memory Request Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
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

			// Data clear
			for (ix=0; ix<(secSize/4); ix++)
				pBuff[ix] = 0;

			// Read
			if ((status = devFlashRead (devType, sec, (unsigned char *)pBuff, secSize)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// コンペア
			for (ix=0; ix<(secSize/4); ix++)
			{
				if (pBuff[ix] != 0xffffffff)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
					sprintf (gLogMsgBuff, "Flash Erase Blank Check Error\nAdrs   = 0x%x\nOffset = 0x%x\nData   = 0x%x\n", sec, ix*4, pBuff[ix]);
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


// eof
