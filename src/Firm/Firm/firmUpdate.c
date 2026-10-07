//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// firmUpdate.c - Firmware Update Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "../CXP/cxp.h"


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern char gKermitFileName[];


//**********************************************************************************
//	ファームウェアUpdate(全プログラム)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmUpdateAll (void)
{
	int status;

	if ((status = firmUpdateMain (FLASH_UPDATE_ALL_ADRS, FLASH_UPDATE_ALL_SIZE, 0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	ファームウェアUpdate(全プログラム)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmUpdateAllAdmin (void)
{
	int status;

	status = firmUpdateMain (FLASH_UPDATE_ALL_ADMIN_ADRS, FLASH_UPDATE_ALL_SIZE, 0);
	return (status);
}


//**********************************************************************************
//	ファームウェアUpdate(XML用)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmUpdateXml (void)
{
	int status;

	status = firmUpdateMain (FLASH_XML_ADRS, FLASH_XML_SIZE, 0);
	return (status);
}


//**********************************************************************************
//	ファームウェアUpdate(Main用)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：updateするFlashアドレス
//		size				：Mallocサイズ
//		offsetMode			：固定オフセットを足したデータを書き込む
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmUpdateMain (unsigned int flashAdrs, unsigned int size, int offsetMode)
{
	int status;
	char *pUpdateBuff = NULL;
	int downLoadSize;
	int len;
	unsigned char *pUpdatePtr;
	unsigned int updateSize;
#ifdef COMPRESS_MODE
	unsigned char *pUnCompBuf = NULL;
	int uncomprLen;
#endif // COMPRESS_MODE

	// メモリリクエスト
#if 1
	pUpdateBuff = (char *)FIRM_UPDATE_ADRS;
#else
	if ((pUpdateBuff = malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Firmware Update Buffer Request Error.\n");
		goto _DONE;
	}
#endif
	
	// Kermit Recive
	if ((downLoadSize = kermitRecv (pUpdateBuff, size)) < 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_IO);
		sprintf (gLogMsgBuff, "Firmware Update kermit Error. Size = %d\n", downLoadSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// CameraLinkのみ
	if (gInterFaceID == INTERFACE_CAMERALINK)
	{
		// File Name Check
		switch (flashAdrs)
		{
			case FLASH_UPDATE_ALL_ADRS:
			case FLASH_UPDATE_ALL_ADMIN_ADRS:

				// Get Size
				len = strlen (UPDATE_FILE_NAME_CL_COMPARE);

				// Check File Name
				if (strncmp (gKermitFileName, UPDATE_FILE_NAME_CL_COMPARE, len) == 0)
				{
					status = AVAL_STATUS_SUCCESS;
				}
				else
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_FILE);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					sprintf (gLogMsgBuff, "Update File Name Error.\nInvalid File Name = %s\nValid File Name = %s\n", gKermitFileName, UPDATE_FILE_NAME_CL_EXSAMPLE);
					goto _DONE;
				}

				// Get Size
				len = strlen (gKermitFileName);

				// Check Extension gz
				if (strncmp (&gKermitFileName[len-3], UPDATE_FILE_NAME_CL_GZIP_EXTENSION, 3) == 0)
				{
					status = AVAL_STATUS_SUCCESS;
					break;
				}

				// Check Extension bin
				if (strncmp (&gKermitFileName[len-4], UPDATE_FILE_NAME_CL_BIN_EXTENSION, 4) == 0)
				{
					status = AVAL_STATUS_SUCCESS;
					break;
				}
				else
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_FILE);
					sprintf (gLogMsgBuff, "Update File Extension Error.\nInvalid File Name = %s\n", gKermitFileName);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}
			
				break;

			default:
				break;
		}
	}

	// バッファ&サイズ更新
	pUpdatePtr = (unsigned char *)pUpdateBuff;
	updateSize = downLoadSize;

#ifdef COMPRESS_MODE
	if ((status = kermitGzipDataCheck (&pUnCompBuf, &uncomprLen, (unsigned char *)pUpdateBuff, downLoadSize, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// バッファ&サイズ更新
	if (pUnCompBuf != NULL)
	{
		pUpdatePtr = pUnCompBuf;
		updateSize = uncomprLen;
	}
#endif // COMPRESS_MODE

	// Offset調整
	if (offsetMode)
	{
		pUpdatePtr += FIRM_UPDATE_OFFSET;
		updateSize -= FIRM_UPDATE_OFFSET;
	}

	if (gInterFaceID == INTERFACE_CXP)
	{
		if (flashAdrs == FLASH_XML_ADRS)
		{
			unsigned int adrs2;
			adrs2 = (unsigned int)pUpdatePtr + updateSize;
			OUT8 (adrs2, 0);	// NULL
			updateSize++;
			char str[128];

			//----------------------------------------
			// File Name書き込み
			//----------------------------------------
			memset((void *)str, 0, 128);
			sprintf(str, "local:%s;61700000;%X?SchemaVersion=1.0.0\n", gKermitFileName, updateSize-1);

			// ファイル名長取得
			len = strlen (str);

			// 終端NULL設定
			str[len-1] = 0;

			// Flash Write (XML File Name)
			if ((status = firmUpdateFlashWrite (FLASH_XML_FILE_NAME_ADRS, (unsigned char *)str, len)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

	// Flash Write
	if ((status = firmUpdateFlashWrite (flashAdrs, (unsigned char *)pUpdatePtr, updateSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Update Size保存
	OUT32 (FIRM_DATA_UPDATE_SIZE, updateSize);

_DONE:
	//if (pUpdateBuff != NULL)
		//free (pUpdateBuff);

#ifdef COMPRESS_MODE
	#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)
	if (pUnCompBuf != NULL)
		free (pUnCompBuf);
	#endif
#endif // COMPRESS_MODE

	return (status);
}


//**********************************************************************************
//	ファームウェアFlash書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：updateするFlashアドレス
//		pBuffer				：updateデータが格納されたポインタ
//		size				：updateデータサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmUpdateFlashWrite (unsigned int adrs, unsigned char *pBuffer, unsigned int size)
{
	int status;
	unsigned int ix;
	unsigned char *pTempBuff = NULL;
	unsigned int *pBuffVerify, *pReadBuff;
	int sec;
	int secSize = QSPI_FLASH_SEC_SIZE;
	unsigned int readSize;
	unsigned int offset;

	// メモリリクエスト
	if ((pTempBuff = malloc (secSize)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Update Temp Buffer Request Error.\n");
		goto _DONE;
	}

//@@@1
	DEBUG_PRINT_FORCE("Erase..\n");
//@@@1
	
	// Erase
	for (sec=adrs; sec<(adrs + size); sec+=secSize)
	{
		// Erase
		if ((status = qspiFlashSectorErase (sec)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

//@@@1
	DEBUG_PRINT_FORCE("Blacnk Check..\n");
//@@@1

	// Blank Check
	for (sec=adrs; sec<(adrs + size); sec+=secSize)
	{
		// Read
		if ((status = qspiFlashRead (sec, (unsigned char *)pTempBuff, secSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// コンペア
		pBuffVerify = (unsigned int *)pTempBuff;
		for (ix=0; ix<(secSize/4); ix++, pBuffVerify++)
		{
			if (*pBuffVerify != 0xffffffff)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "Update Blank Check Error.\nAdrs = 0x%x\nData = 0x%x\n", sec + ix*4, *pBuffVerify);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}
	}

//@@@1
	DEBUG_PRINT_FORCE("Write..\n");
//@@@1

	// Write
	if ((status = qspiFlashWrite (adrs, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

//@@@1
	DEBUG_PRINT_FORCE("Verify..\n");
//@@@1

	// Read & ベリファイ
	for (sec=adrs, offset=0; sec<(adrs + size); sec+=secSize, offset+=secSize)
	{
		// Size Check
		if ((int)(size - offset) > secSize)
			readSize = secSize;
		else
			readSize = size - offset;

		// Read
		if ((status = qspiFlashRead (sec, (unsigned char *)pTempBuff, readSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// ベリファイ
		pBuffVerify = (unsigned int *)(pBuffer + offset);
		pReadBuff = (unsigned int *)pTempBuff;
		for (ix=0; ix<(readSize/4); ix++, pBuffVerify++, pReadBuff++)
		{
			if (*pBuffVerify != *pReadBuff)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "Update Verify Error.\nAdrs = 0x%x\nWrite = 0x%x\nRead = 0x%x\n", sec + ix*4, *pBuffVerify, *pReadBuff);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}
	}

//@@@1
	DEBUG_PRINT_FORCE("End..\n");
//@@@1

_DONE:
	if (pTempBuff != NULL)
		free (pTempBuff);

	return (status);
}


//**********************************************************************************
//	ファームウェアDownload(全プログラム)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：Download Size
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmDownloadAll (unsigned int size)
{
	int status;

	status = firmDownloadMain (FLASH_UPDATE_ALL_ADRS, size, "download.bin");
	return (status);
}


//**********************************************************************************
//	ファームウェアDownload(全プログラム)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：Download Size
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmDownloadAllAdmin (unsigned int size)
{
	int status;

	status = firmDownloadMain (FLASH_UPDATE_ALL_ADMIN_ADRS, size, "download.bin");
	return (status);
}


//**********************************************************************************
//	ファームウェアDownload(XML用)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmDownloadXml (int size)
{
	int status;

	status = firmDownloadMain (FLASH_XML_ADRS, size, "xmlDownload.xml");
	return (status);
}


//**********************************************************************************
//	ファームウェアDownload(Main用)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：DownloadするFlashアドレス
//		size				：Download Size
//		pName				：ファイル名を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmDownloadMain (unsigned int flashAdrs, unsigned int size, char *pName)
{
	int status = AVAL_STATUS_SUCCESS;
	char *pBuffer = NULL;
	int kermitSendSize;

	// Check Size Parameter
	if ((size < 1) || (size > FIRM_DOWNLOAD_ALL_SIZE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff,  "Download Size Error. Size = 0x%x\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリリクエスト
	if ((pBuffer = malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Download Buffer Request Error.\n");
		goto _DONE;
	}

	// Check pName Parameter
	if (pName == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Download NULL Parameter Error.\n");
		goto _DONE;
	}

	// Flash Read
	if ((status = qspiFlashRead (flashAdrs, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Kermit Send
	if ((kermitSendSize = kermitSend (pName, pBuffer, size)) < 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_IO);
		sprintf (gLogMsgBuff, "Download kermit Error. Size = %d\n", kermitSendSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
//	FPGA Update
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaUpdate (void)
{
	int status;

	if ((status = fpgaUpdateMain (FLASH_FPGA_DATA_ADRS, FLASH_UPDATE_ALL_SIZE, 0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	FPGA Update (Administrator)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaUpdateAdmin (void)
{
	int status;

	if ((status = fpgaUpdateMain (FLASH_FPGA_DATA_ADMIN_ADRS, FLASH_UPDATE_ALL_SIZE, 0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	FPGA Update Main
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：updateするFlashアドレス
//		size				：Mallocサイズ
//		offsetMode			：固定オフセットを足したデータを書き込む
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaUpdateMain (unsigned int flashAdrs, unsigned int size, int offsetMode)
{
	int status;
	char *pUpdateBuff = NULL;
	int downLoadSize;
	int len1;
	unsigned char *pUpdatePtr;
	unsigned int updateSize;
#ifdef COMPRESS_MODE
	unsigned char *pUnCompBuf = NULL;
	int uncomprLen;
#endif // COMPRESS_MODE

	// メモリリクエスト
#if 1
	pUpdateBuff = (char *)FIRM_UPDATE_ADRS;
#else
	if ((pUpdateBuff = malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Update Buffer Request Error.\n");
		goto _DONE;
	}
#endif

	// Kermit Recive
	if ((downLoadSize = kermitRecv (pUpdateBuff, size)) < 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_IO);
		sprintf (gLogMsgBuff, "FPGA Update kermit Error. Size = %d\n", downLoadSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// CXPのみ
	if (gInterFaceID == INTERFACE_CXP)
	{
		// Get Size
		len1 = strlen (UPDATE_FILE_NAME_FPGA_COMPARE);

		// Check File Name
		if ((strncmp (gKermitFileName, UPDATE_FILE_NAME_FPGA_COMPARE, len1) != 0))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_FILE);
			sprintf (gLogMsgBuff, "FPGA Update File Name Error.\nInvalid File Name = %s\nValid File Name = %s\n", gKermitFileName, UPDATE_FILE_NAME_FPGA_EXSAMPLE);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Get Size
		len1 = strlen (gKermitFileName);

		// Check Extension gz
		if (strncmp (&gKermitFileName[len1-3], UPDATE_FILE_NAME_CL_GZIP_EXTENSION, 3) == 0)
			goto _NEXT;

		// Check Extension spi
		if (strncmp (&gKermitFileName[len1-4], UPDATE_FILE_NAME_CXP_SPI_EXTENSION, 4) == 0)
			goto _NEXT;

		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_FILE);
		sprintf (gLogMsgBuff, "FPGA Update File Extension Error.\nInvalid File Name = %s\n", gKermitFileName);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_NEXT:
	// バッファ&サイズ更新
	pUpdatePtr = (unsigned char *)pUpdateBuff;
	updateSize = downLoadSize;

#ifdef COMPRESS_MODE
	if ((status = kermitGzipDataCheck (&pUnCompBuf, &uncomprLen, (unsigned char *)pUpdateBuff, downLoadSize, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// バッファ&サイズ更新
	if (pUnCompBuf != NULL)
	{
		pUpdatePtr = pUnCompBuf;
		updateSize = uncomprLen;
	}
#endif // COMPRESS_MODE

	// Offset調整
	if (offsetMode)
	{
		pUpdatePtr += FIRM_UPDATE_OFFSET;
		updateSize -= FIRM_UPDATE_OFFSET;
	}

	// Flash Write
	if ((status = firmUpdateFlashWrite (flashAdrs, (unsigned char *)pUpdatePtr, updateSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ヘッダー更新
	if ((status = fpgaUpdateHeader ())!= AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (flashAdrs == FLASH_FPGA_DATA_ADRS)
	{
		// ReConfig
		if ((status = fpgaReconfig ()) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	//if (pUpdateBuff != NULL)
		//free (pUpdateBuff);

#ifdef COMPRESS_MODE
#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)
	if (pUnCompBuf != NULL)
		free (pUnCompBuf);
#endif
#endif // COMPRESS_MODE

	return (status);
}


//**********************************************************************************
//	FPGA ReConfig
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaReconfig (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char data;
	int i;

	// PolarFire Reconfig Index設定
	data = FPGA_RECONFIG_INDEX_DEFAULT;
	OUT32 (FPGA_RECONFIG_ADRS, data);

	// PolarFire Reconfig 実行
	data = FPGA_RECONFIG_INDEX_DEFAULT | FPGA_RECONFIG_START_BIT;
	OUT32 (FPGA_RECONFIG_ADRS, data);

	// Reconfig完了待ち
	for (i = 0; i < 20; i++)
		msDelay(1000);

_DONE:
	return (status);
}


//**********************************************************************************
//	FPGA Header Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaUpdateHeader (void)
{
	int status = AVAL_STATUS_SUCCESS;
	const int data_size = QSPI_FLASH_SEC_SIZE;
	unsigned char *pTempBuff = NULL;
	unsigned char *pReadBuff = NULL;
	unsigned int ix;

	// Memory Reqest
	if ((pTempBuff = malloc (data_size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Header Temp Buffer Request Error.\n");
		goto _DONE;
	}

	// Memory Reqest
	if ((pReadBuff = malloc (data_size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Header Read Buffer Request Error.\n");
		goto _DONE;
	}

	// セクタ0リード
	if ((status = qspiFlashRead (FLASH_HEADER_ADRS, pTempBuff, data_size))!= AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ヘッダ情報更新
	pTempBuff[FLASH_HEADER_ADRS]     = FLASH_FPGA_DATA_ADMIN_ADRS & 0xFF;
	pTempBuff[FLASH_HEADER_ADRS + 1] = (FLASH_FPGA_DATA_ADMIN_ADRS >> 8) & 0xFF;
	pTempBuff[FLASH_HEADER_ADRS + 2] = (FLASH_FPGA_DATA_ADMIN_ADRS >> 16) & 0xFF;
	pTempBuff[FLASH_HEADER_ADRS + 3] = (FLASH_FPGA_DATA_ADMIN_ADRS >> 24) & 0xFF;

	pTempBuff[FLASH_HEADER_ADRS + 4] = 0x00;
	pTempBuff[FLASH_HEADER_ADRS + 5] = 0x00;
	pTempBuff[FLASH_HEADER_ADRS + 6] = 0x00;
	pTempBuff[FLASH_HEADER_ADRS + 7] = 0x00;

	pTempBuff[FLASH_HEADER_ADRS + 8] = FLASH_FPGA_DATA_ADRS & 0xFF;
	pTempBuff[FLASH_HEADER_ADRS + 9] = (FLASH_FPGA_DATA_ADRS >> 8) & 0xFF;
	pTempBuff[FLASH_HEADER_ADRS + 10] = (FLASH_FPGA_DATA_ADRS >> 16) & 0xFF;
	pTempBuff[FLASH_HEADER_ADRS + 11] = (FLASH_FPGA_DATA_ADRS >> 24) & 0xFF;


	// Erase
	if ((status = qspiFlashSectorErase (FLASH_HEADER_ADRS)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Flash Write
	if ((status = qspiFlashWrite (FLASH_HEADER_ADRS, pTempBuff, data_size))!= AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Flash Read
	if ((status = qspiFlashRead (FLASH_HEADER_ADRS, pReadBuff, data_size))!= AVAL_STATUS_SUCCESS)
		goto _DONE;

	//ベリファイ
	for (ix = 0 ; ix < data_size ; ix++)
	{
		if (pTempBuff[ix] != pReadBuff[ix])
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_COMPARE);
			sprintf (gLogMsgBuff, "FPGA Header Verify Error.\nAdrs = 0x%x\nWrite Data = 0x%x\nRead  Data = 0x%x\n", ix, pTempBuff[ix], pReadBuff[ix]);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

_DONE:
	if(pTempBuff != NULL)
		free(pTempBuff);

	if(pReadBuff != NULL)
		free(pReadBuff);

	return (status);
}


//**********************************************************************************
//	FPGA Download
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：Download Size
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaDownload  (unsigned int size)
{
	int status;

	if ((status = fpgaDownloadMain (FLASH_FPGA_DATA_ADRS, size, "FpagDownload.bin")) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	PolarFire Interface FPGA Download(Main用)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：DownloadするFlashアドレス
//		size				：Download Size
//		pName				：ファイル名を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaDownloadMain (unsigned int flashAdrs, unsigned int size, char *pName)
{
	int status;
	char *pBuffer = NULL;
	int kermitSendSize;

	// Check Size Parameter
	if ((size < 1) || (size > QSPI_FLASH_SIZE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FPGA Download Size Error. Size = 0x%x\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリリクエスト
	if ((pBuffer = malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Download Buffer Request Error.\n");
		goto _DONE;
	}

	// Check pName Parameter
	if (pName == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Download pName NULL Parameter Error.\n");
		goto _DONE;
	}

	// Flash Read
	if ((status = qspiFlashRead (flashAdrs, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Kermit Send
	if ((kermitSendSize = kermitSend (pName, pBuffer, size)) < 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_IO);
		sprintf (gLogMsgBuff, "FPGA Download kermit Error. Size = %d\n", kermitSendSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
//	Copy(ユーザー領域データをゴールデン領域にコピー)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmDataCopy (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int size = FLASH_UPDATE_COPY_SIZE;

	// Flash Read
	if ((status = qspiFlashRead (FLASH_FPGA_DATA_ADRS, (unsigned char *)FIRM_UPDATE_ADRS, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Flash Write
	if ((status = firmUpdateFlashWrite (FLASH_FPGA_DATA_ADMIN_ADRS, (unsigned char *)FIRM_UPDATE_ADRS, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


#if defined (MODE_GIGE_10G)
//**********************************************************************************
//	Phyデータ更新
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmUpdatePhy (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Erase(サイズを書き込む領域を消去)
	if ((status = qspiFlashSectorErase (FLASH_PHY_DATA_ADRS + FLASH_PHY_DATA_SIZE - 0x10000)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// データ書き込み
	if ((status = firmUpdateMain (FLASH_PHY_DATA_ADRS, FLASH_PHY_DATA_SIZE, 0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// Update Size保存(最後の領域に書き込み)
	if ((status = qspiFlashWrite ((FLASH_PHY_DATA_ADRS + FLASH_PHY_DATA_SIZE - 4), (unsigned char *)FIRM_DATA_UPDATE_SIZE, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Phy Data Copy
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int phyDataCopy (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// データRead
	if ((status = qspiFlashRead (FLASH_PHY_DATA_ADRS, (unsigned char *)FIRM_UPDATE_ADRS, FLASH_PHY_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データWrite
	if ((status = qspiFlashWrite (FLASH_PHY_DATA_BACKUP_ADRS, (unsigned char *)FIRM_UPDATE_ADRS, FLASH_PHY_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}
#endif // #if defined (MODE_GIGE_10G)

// eof
