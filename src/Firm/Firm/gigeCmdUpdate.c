//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// gigeCmdUpdate.c - GigE Command Update Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "../Cxp/cxp.h"
#include "../GigE/gev.h"


#if defined (MODE_CXP)
//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern double *gpSpectrumBandInfo;
extern unsigned int fileSelector;
extern unsigned int fileSel[];
extern unsigned int fileLength[FileSelector_MAX][FileOperationeSelector_MAX];
extern unsigned int fileResult[FileSelector_MAX][FileOperationeSelector_MAX];
extern unsigned int fileExecWriteDoneSize;


//**********************************************************************************
//	Firm Upload
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		pBuffer				：Bufferアドレスを格納するポインタ
//		size				：データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdFirmUpload (unsigned int flashAdrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int endCmd = FIRM_CMD_OK;
	unsigned int buff;
	unsigned char *pUpdatePtr;
	unsigned int updateSize;
	int uncomprLen;
	int saveLed0 = -1;
	unsigned char *pUnCompBuf = (unsigned char *)FIRM_UPDATE_UNCOMP_ADRS;
	unsigned char *pUpdateBuff = (unsigned char *)FIRM_UPDATE_ADRS;

	#if !defined (MODE_FLASH_WRITE_COPY)
	pUnCompBuf  += QSPI_FLASH_WRITE_COPY_ADD_SIZE;
	pUpdateBuff += QSPI_FLASH_WRITE_COPY_ADD_SIZE;
	#endif

	// 調整ステート
	saveLed0 = ledSettingState ();

	// バッファ&サイズ更新
	pUpdatePtr = (unsigned char *)pBuffer;
	updateSize = size;

	//@@@@@@@@@@@@@@@@
	DEBUG_PRINT_FORCE("UnComp!!!!!!!!!!\n");
	//@@@@@@@@@@@@@@@@

	// 圧縮データ?
#ifdef COMPRESS_MODE
	if ((pUpdatePtr[0] == COMPRESS_GZIP_ID1) && (pUpdatePtr[1] == COMPRESS_GZIP_ID2))
	{
		// 解凍データサイズを格納するポインタ
		uncomprLen = FIRM_UPDATE_UNCOMP_SIZE;

		// Uncompress
		if ((status = gzipUncomp ((unsigned char *)pUnCompBuf, &uncomprLen, (unsigned char *)pUpdateBuff, size)) != 0)
		{
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Uncompress Error.");
			goto _DONE;
		}

		// バッファ&サイズ更新
		pUpdatePtr = (unsigned char *)FIRM_UPDATE_UNCOMP_ADRS;
		updateSize = uncomprLen;

		#if !defined (MODE_FLASH_WRITE_COPY)
		pUpdatePtr += QSPI_FLASH_WRITE_COPY_ADD_SIZE;
		#endif
	}
#endif

	//@@@@@@@@@@@@@@@@
	DEBUG_PRINT_FORCE("Update Start!!!!!!!!!!\n");
	//@@@@@@@@@@@@@@@@
	
	// Flash Write
	if ((status = firmUpdateFlashWrite (flashAdrs, pUpdatePtr, updateSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	// Update Header
	if ((status = fpgaUpdateHeader ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	//@@@@@@@@@@@@@@@@
	DEBUG_PRINT_FORCE("Update End!!!!!!!!!!\n");
	//@@@@@@@@@@@@@@@@

	return (status);
}

#define UPDATE_ALL_TIMEOUT		(5000000)

#if defined (MODE_UPDATE_ALL_MULTI)
//**********************************************************************************
//	Firm Upload Multi
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		pBuffer				：Bufferアドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdFirmUploadMulti (unsigned int flashAdrs, unsigned char *pBuffer)
{
	int status = AVAL_STATUS_SUCCESS;
	int endCmd = FIRM_CMD_OK;
	int saveLed0 = -1;
	unsigned int size;
	int sec;
	int flashTransed, cxpTransed;
	int writeSize;
	unsigned int flashAdrs2;
	int secSize = QSPI_FLASH_SEC_SIZE;
	unsigned int timeout;

	// 調整ステート
	saveLed0 = ledSettingState ();

	//@@@1
	DEBUG_PRINT_FORCE("Erase!!!!!!!!!\n");
	//@@@1

	// Erase
	size = FLASH_FIRM_DATA_SIZE + FLASH_FPGA_DATA_SIZE;
	for (sec=flashAdrs; sec<(flashAdrs + size); sec+=secSize)
	{
		// Erase
		if ((status = qspiFlashSectorErase (sec)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//@@@1
	DEBUG_PRINT_FORCE("Write.....\n");
	//@@@1
	
	flashTransed = 0;
	flashAdrs2 = flashAdrs;

	// 書き込みデータがなくなったから10秒のタイムアウト
	for (timeout=0; timeout<UPDATE_ALL_TIMEOUT; timeout++)
	{
		// Get Size
		cxpTransed = fileResult[fileSelector][fileSel[fileSelector]];

		if (flashTransed != cxpTransed)
		{
			writeSize = cxpTransed - flashTransed;

			//@@@1
			DEBUG_PRINT_FORCE("flashTransed = %d, cxpTransed = %d, writeSize = %d\n", flashTransed, cxpTransed, writeSize);
			//@@@1

			if (writeSize <= 0)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "Update Size Error. Flash Transed = %d, CXP Transed = %d\n", flashTransed, cxpTransed);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// Write
			if ((status = qspiFlashWrite (flashAdrs2, (unsigned char *)(pBuffer + flashTransed), writeSize)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Clear
			timeout = 0;

			// Update
			flashAdrs2 += writeSize;
			flashTransed += writeSize;
		}
		else
		{
			if (fileExecWriteDoneSize != 0)
			{
				if (flashTransed == fileExecWriteDoneSize)
					break;
			}
			
			usDelay (2);
			timeout++;
		}
	}

	if (timeout >= UPDATE_ALL_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Update Timeout Error.");
		goto _DONE;
	}
	
	//@@@1
	DEBUG_PRINT_FORCE("End..\n");
	DEBUG_PRINT_FORCE("flashTransed = %d, cxpTransed = %d, fileExecWriteDoneSize = %d\n", flashTransed, cxpTransed, fileExecWriteDoneSize);
	//@@@1

	// Update Header
	if ((status = fpgaUpdateHeader ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	//@@@@@@@@@@@@@@@@
	DEBUG_PRINT_FORCE("Update End!!!!!!!!!!\n");
	//@@@@@@@@@@@@@@@@

	return (status);
}
#endif // #if defined (MODE_UPDATE_ALL_MULTI)
#endif // ##if defined (MODE_CXP)


//**********************************************************************************
//	XML Upload
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		pBuffer				：Bufferアドレスを格納するポインタ
//		size				：データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdXmlUpload (unsigned int flashAdrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int endCmd = FIRM_CMD_OK;
	int saveLed0 = -1;
	int i;
	unsigned char data8;

//@@@1
DEBUG_PRINT_FORCE("Update XML Start\n")	;
//@@@1

	// 調整ステート
	saveLed0 = ledSettingState ();

	// XML Data Write
	if ((status = firmUpdateFlashWrite (flashAdrs, pBuffer, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// CXPの場合のみファイル名も書き込む
	for (i=0; i<CXP_XML_URL_SIZE; i++)
	{
		data8 = IN8 ((FIRM_UPDATE_XML_FILE_NAME_ADRS + i));
		if (data8 == 0)
		{
			i++;
			break;
		}
	}

	// File Name Write
	if ((status = firmUpdateFlashWrite (FLASH_XML_FILE_NAME_ADRS, (unsigned char *)FIRM_UPDATE_XML_FILE_NAME_ADRS, i)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
//@@@1
DEBUG_PRINT_FORCE("Update XML End\n")	;
//@@@1

	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}


//**********************************************************************************
//	IF FPGA Upload
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		pBuffer				：Bufferアドレスを格納するポインタ
//		size				：データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdIfFpgaUpload  (unsigned int flashAdrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int saveLed0 = -1;

//@@@1
DEBUG_PRINT_FORCE("IF FPGA Start. Adrs = 0x%x\n", flashAdrs);
//@@@1

	// 調整ステート
	saveLed0 = ledSettingState ();

	// Flash Write
	if ((status = firmUpdateFlashWrite (flashAdrs, pBuffer, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ヘッダー更新
	if ((status = fpgaUpdateHeader ())!= AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Reconfig
	//if (adrs == FLASH_UPDATE_FPGA_ADRS)
	//{
		// ReConfig
		//if ((status = fpgaUpdatePolarFireReconfig ()) != AVAL_STATUS_SUCCESS)
			//goto _DONE;
	//}

_DONE:
//@@@1
DEBUG_PRINT_FORCE("Update IF FPGA End\n")	;
//@@@1

	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


#if defined (MODE_GIGE_10G)
//**********************************************************************************
//	PHY Upload
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		pBuffer				：Bufferアドレスを格納するポインタ
//		size				：データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdPhyUpload (unsigned int flashAdrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int saveLed0 = -1;

	// 調整ステート
	saveLed0 = ledSettingState ();

	// Update Size保存
	OUT32 (FIRM_DATA_UPDATE_SIZE, size);
	
	// Erase
	if ((status = qspiFlashSectorErase (flashAdrs + FLASH_PHY_DATA_SIZE - 0x10000)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// Flash Write
	if ((status = firmUpdateFlashWrite (flashAdrs, pBuffer, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Update Size保存
	if ((status = qspiFlashWrite ((flashAdrs + FLASH_PHY_DATA_SIZE - 4), (unsigned char *)FIRM_DATA_UPDATE_SIZE, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;	

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}
#endif


#if defined (MODE_SPECTRUM)
//**********************************************************************************
//	Spectrum Wave Upload
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pBuffer				：Buffアドレスを格納するポインタ
//		size				：データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdSpectrumWaveUpload (void *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int binSize;
	int bandMaxCount;
	int saveLed0 = -1;

	// 調整ステート
	saveLed0 = ledSettingState ();

	// 波長最大個数取得
	if ((status = spectrumBandMaxCount (&bandMaxCount))  != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// バイナリサイズ
	binSize = SPECTRUM_BIN_UNIT_SIZE * bandMaxCount;

	// ASCII=>Binary変換
	if ((status = spectrunBandInfoUploadAsciiToBin ((void *)pBuffer, (void *)gpSpectrumBandInfo, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Flashへの書き込み
	if ((status = spectrumToFlash (FLASH2_SPECTRUM_BAND_INFO_ADRS, (unsigned int)gpSpectrumBandInfo, binSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}
#endif


#if defined (MODE_CXP)
//**********************************************************************************
//	DPC Upload
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pAdrs				：転送元アドレス
//		offset				：DPCメモリオフセットアドレス
//		size				：データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdDpcUpload (void *pAdrs, unsigned int offset, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_DPC_GRID_UPDATE)
	int dpcIndex;
#endif

	// DPCデータ設定
	if ((status = dpcSetMemoryNormal (pAdrs, offset, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 終端を検索
	if ((status = dpcEndSearch (DPC_MEMORY_ADRS, &dpcIndex)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPCカウント数設定
	if ((status = dpcSetDefectionCount (dpcIndex)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map情報更新
	if ((status = dpcGetMapInfo ((unsigned char *)DPC_MEMORY_ADRS, &dpcIndex)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標登録
	dpcSetDefectionCountVersion2 (dpcIndex);
#endif

_DONE:
	return (status);
}

#endif // #if defined (MODE_CXP)

// eof
