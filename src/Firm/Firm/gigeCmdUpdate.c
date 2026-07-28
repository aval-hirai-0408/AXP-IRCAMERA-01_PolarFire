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


#if defined (MODE_CXP)
//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern double *gpSpectrumBandInfo;


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
	int *pUncomprLen = NULL; // 解凍データサイズを格納するポインタ
	int saveLed0 = -1;
#ifdef COMPRESS_MODE
	unsigned char *pUnCompBuf = NULL;
#endif

	// 調整ステート
	saveLed0 = ledSettingState ();

	// バッファ&サイズ更新
	pUpdatePtr = (unsigned char *)pBuffer;
	updateSize = size;

	// 圧縮データ?
#ifdef COMPRESS_MODE
	if ((pUpdatePtr[0] == COMPRESS_GZIP_ID1) && (pUpdatePtr[1] == COMPRESS_GZIP_ID2))
	{
		// 解凍データサイズを格納するポインタ
		*pUncomprLen = FIRM_UPDATE_UNCOMP_SIZE;

		#if 0	//@@@1
		// Uncompress
		if ((status = gzipUncomp ((unsigned char *)FIRM_UPDATE_UNCOMP_ADRS, pUncomprLen, (unsigned char *)FIRM_UPDATE_ADRS, size)) != 0)
		{
			DEBUG_PRINT_FORCE ("Uncompress Error. Status = %d\n", status);
			goto _DONE;
		}
		#endif //#if 0	//@@@1

		// バッファ&サイズ更新
		pUpdatePtr = (unsigned char *)FIRM_UPDATE_UNCOMP_ADRS;
		updateSize = *pUncomprLen;
	}
#endif

	// Flash Write
	if ((status = firmUpdateFlashWrite (flashAdrs, pUpdatePtr, updateSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:

#ifdef COMPRESS_MODE
	if (pUnCompBuf != NULL)
		free (pUnCompBuf);
#endif // COMPRESS_MODE

	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}
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
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}


#if defined (MODE_CXP)
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

	// 調整ステート
	saveLed0 = ledSettingState ();

#if defined (MODE_FPGA_PF)
	// Flash Write
	if ((status = fpgaUpdatePolarFireFlashWrite (flashAdrs, pBuffer, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ヘッダー更新
	if ((status = fpgaUpdatePolarFireHeader ())!= AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Reconfig
	//if (adrs == FLASH_UPDATE_POLAFIRE_ADRS)
	//{
		// ReConfig
		//if ((status = fpgaUpdatePolarFireReconfig ()) != AVAL_STATUS_SUCCESS)
			//goto _DONE;
	//}
#endif

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}
#endif


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
