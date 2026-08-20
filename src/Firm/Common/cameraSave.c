//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cameraSave.c - Camera Parameter Save Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "cameraSaveParam.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// Camera Save Status
int gCameraSaveStatus = 0;

// アドレス
unsigned int *gpCameraCommonParameter = NULL;			// 共通領域
unsigned int *gpCameraUserParameter = NULL;				// UserSet領域
unsigned int *gpCameraSpectrumParameter = NULL;			// UserSet Spectrum領域


//**********************************************************************************
// カメラ保存パラメータ関連の初期化(ARM0用)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int userNum;

	//------------------------------------------------------------
	// 共通領域
	//------------------------------------------------------------
	
	// 共通パラメータ格納アドレス取得
	gpCameraCommonParameter = (unsigned int *)CAMERA_PARAM_CONNON_ADRS;

#if 0   //@@@1
	// 共通パラメータ領域取得
	if ((status = cameraParamCommonAllRead ()) != AVAL_STATUS_SUCCESS)
	{
		gCameraSaveStatus = status;
		goto _DONE;
	}

	// カメラ設定マークあり？
	if (gpCameraCommonParameter[CAMERA_SAVE_COMMON_MARK_ADRS/4] != CAMERA_MARK_DATA)
		cameraParamCommonMarkClear ();	// クリア

	// Check User Mode
	userNum = gpCameraCommonParameter[CAMERA_SAVE_COMMON_USER_PARAM_MODE_ADRS/4];
	if (userNum > CAMERA_USER_MODE_MAX)
		userNum = CAMERA_USER0_MODE;

	//------------------------------------------------------------
	// UserSet領域
	//------------------------------------------------------------

	// UserSetパラメータ格納アドレス取得
#if !defined (MODE_SPECTRUM)
	gpCameraUserParameter = (unsigned int *)CAMERA_PARAM_USER_ADRS;
#else
	gpCameraUserParameter = (unsigned int *)CAMERA_PARAM_USER_EXTEND_ADRS;
#endif

	// 保存パラメータ取得
	if ((status = cameraParamUserAllRead (userNum)) != AVAL_STATUS_SUCCESS)
	{
		gCameraSaveStatus = status;
		goto _DONE;
	}

	// カメラ設定マークあり？
	if (*gpCameraUserParameter != CAMERA_MARK_DATA)
		cameraParamUserMarkClear (userNum);	// クリア＆デフォルトパラメータ設定

	//------------------------------------------------------------
	// UserSet Spectrum領域
	//------------------------------------------------------------
#if defined (MODE_USERSET_SPECTRUM_PARAM)

	// UserSetパラメータ格納アドレス取得
	gpCameraSpectrumParameter = (unsigned int *)CAMERA_PARAM_USER_SPECTRUM_ADRS;

	// 保存パラメータ取得
	if ((status = cameraParamSpectrumAllRead (userNum)) != AVAL_STATUS_SUCCESS)
	{
		gCameraSaveStatus = status;
		goto _DONE;
	}

	// カメラ設定マークあり？
	if (*gpCameraSpectrumParameter != CAMERA_MARK_DATA)
		cameraParamSpectrumMarkClear (userNum);	// クリア＆デフォルトパラメータ設定

#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)


#endif //@@@1
_DONE:
	return (status);
}


//**********************************************************************************
// カメラ保存パラメータ関連の初期化(ARM1用)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamInitialize2 (void)
{
	// 共通パラメータ格納アドレス取得
	gpCameraCommonParameter = (unsigned int *)CAMERA_PARAM_CONNON_ADRS;

	// UserSetパラメータ格納アドレス取得
#if !defined (MODE_SPECTRUM)
	gpCameraUserParameter = (unsigned int *)CAMERA_PARAM_USER_ADRS;
#else
	#if defined (MODE_SPECTRUM_BOARD_TYPE)
	gpCameraUserParameter = (unsigned int *)CAMERA_PARAM_USER_ADRS;
	#else
	gpCameraUserParameter = (unsigned int *)CAMERA_PARAM_USER_EXTEND_ADRS;
	#endif
#endif
	
#if defined (MODE_USERSET_SPECTRUM_PARAM)
	// UserSet Spectrumパラメータ格納アドレス取得
	gpCameraSpectrumParameter = (unsigned int *)CAMERA_PARAM_USER_SPECTRUM_ADRS;
#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
// Defaultパラメータ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：userSet番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamSetDefault (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;
	unsigned int size;
	SAVE_PARAM	*pSaveparam = (SAVE_PARAM *)gpCameraUserParameter;
	SAVE_PARAM *pParam;
	int paramSize;

	// リスト数取得
	size = sizeof (saveParamDataList) / sizeof (saveParamDataList[0]);

	if (userNum == CAMERA_FACTORY_NUM)
	{
		// Factoryパラメータ取得
		for (i=1; i<size;i++)
		{
			// Set Register Address
			pSaveparam[i].regAdrs = saveParamDataList[i].regAdrs;

			//sprintf (gLogMsgBuff, "index=%d / Reg = 0x%x / Data = 0x%x\n", i, saveParamDataList[i].regAdrs, saveParamDataList[i].data);
			//cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

			// Get Register Data
			if (saveParamDataList[i].mode == CAMERA_SAVE_MODE_NONE)							// 未使用		=> 未使用なので0
				pSaveparam[i].data = 0;
			else if (saveParamDataList[i].mode == CAMERA_SAVE_MODE_DATA)					// データ有効		=> 構造体から取得
				pSaveparam[i].data = saveParamDataList[i].data;
			else if (saveParamDataList[i].mode == CAMERA_SAVE_MODE_REG_REF)					// レジスタ参照有効	=> レジスタから取得
				pSaveparam[i].data = IN32(saveParamDataList[i].regAdrs);
			else if (saveParamDataList[i].mode == CAMERA_SAVE_MODE_WRITE_OTHER)				// データ有効		=> 構造体から取得
				pSaveparam[i].data = saveParamDataList[i].data;
			else																			// 上記以外		=> 0を設定
				pSaveparam[i].data = 0;
		}
	}
	else
	{
		// UserSet0領域の取得
		if ((status = cameraParamUserAllRead (CAMERA_FACTORY_NUM)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// カメラパラメータ保存マーク設定
	gpCameraUserParameter[0] = CAMERA_MARK_DATA;

_DONE:
	return (status);
}


//**********************************************************************************
// Defaultパラメータ設定(Spectrum関連)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：userSet番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if defined (MODE_USERSET_SPECTRUM_PARAM)
int cameraParamSetSpectrumDefault (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;
	unsigned int address, data;
	unsigned int *ptrL;
	int bandCount;

	// 分光波長数
	if ((status = spectrumBandMaxCount (&bandCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (userNum == CAMERA_FACTORY_NUM)
	{
		//--------------------------------------------------------------------------------
		// Offset/Gain
		//--------------------------------------------------------------------------------

		// アドレス取得
		ptrL = (unsigned int *)(gpCameraSpectrumParameter + (CAMERA_PARAM_SPECTRUM_GAIN_OFFSET/4));

		// Offsetアドレス取得
		address = FPGA_BGF_BAND_OFFSET1_ADRS;

		// Factoryパラメータ取得
		for (i=0; i<bandCount;i++)
		{
			// アドレス設定
			*ptrL = address;
			ptrL++;

			// レジスタ値設定
			if ((i%4) == 0)
			{
				data = IN32 (address);
				*ptrL = data;			// オフセット1
			}
			else if ((i%4) == 1)
			{
				data = IN32 (address);
				*ptrL = data;			// ゲイン
			}
			else if ((i%4) == 2)
			{
				data = IN32 (address);
				*ptrL = data;			// オフセット2
			}
			else if ((i%4) == 3)
			{
				*ptrL = 0;				// リザーブ
				*(ptrL-1) = 0;			// アドレスクリア
			}

			// 更新
			address += 4;
			ptrL++;
		}
	}
	else
	{
		// UserSet0領域の取得
		if ((status = cameraParamSpectrumAllRead (CAMERA_FACTORY_NUM)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// カメラパラメータ保存マーク設定
	gpCameraSpectrumParameter[0] = CAMERA_MARK_DATA;

_DONE:
	return (status);
}
#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)


//**********************************************************************************
// カメラパラメータ保存(ユーザー領域書き込み)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：ユーザーモード(0=User0/1=User1/2=User2)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamUserAllWrite (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save (User ROM Write) User Parameter Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gpCameraUserParameter Parameter
	if (gpCameraUserParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(User ROM Write) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// Get Flash Address
	adrs = CAMERA_SAVE_USER0_ADRS + userNum * CAMERA_SAVE_UINT_SIZE;

	// イレーズ
	if ((status = qspiFlashSectorErase (adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// 書き込み(カメラパラメータ書き込み)
	if ((status = qspiFlashWrite (adrs+4, (unsigned char *)gpCameraUserParameter+4, CAMERA_SAVE_USER_SIZE-4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 書き込み(マークデータ書き込み)
	if ((status = qspiFlashWrite (adrs, (unsigned char *)gpCameraUserParameter, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// カメラパラメータ取得(ユーザー領域読み込み)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：ユーザーモード(0=User0/1=User1/2=User2)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamUserAllRead (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff,  "Camera Save(User ROM Read) User Parameter Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gpCameraUserParameter Parameter
	if (gpCameraUserParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(User ROM Read) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// Get Flash Address
	adrs = CAMERA_SAVE_USER0_ADRS + userNum * CAMERA_SAVE_UINT_SIZE;

	// 読み込み
	if ((status = qspiFlashRead (adrs, (unsigned char *)gpCameraUserParameter, CAMERA_SAVE_USER_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// カメラパラメータ保存(スペクトル関連書き込み)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：ユーザーモード(0=User0/1=User1/2=User2)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if defined (MODE_USERSET_SPECTRUM_PARAM)
int cameraParamSpectrumAllWrite (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save (User Spectrum ROM Write) User Parameter Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gpCameraSpectrumParameter Parameter
	if (gpCameraUserParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(User Spectrum ROM Write) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// Get Flash Address
	adrs = CAMERA_SAVE_USER0_SPECTRUM_ADRS + userNum * CAMERA_SAVE_UINT_SIZE;

	// イレーズ
	if ((status = qspiFlashSectorErase (adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// 書き込み(カメラパラメータ書き込み)
	if ((status = qspiFlashWrite (adrs+4, (unsigned char *)gpCameraSpectrumParameter+4, CAMERA_SAVE_SPECTRUM_COMMON_SIZE-4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 書き込み(マークデータ書き込み)
	if ((status = qspiFlashWrite (adrs, (unsigned char *)gpCameraSpectrumParameter, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}
#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)


//**********************************************************************************
// スペクトルパラメータ取得(ユーザー領域読み込み)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：ユーザーモード(0=User0/1=User1/2=User2)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if defined (MODE_USERSET_SPECTRUM_PARAM)
int cameraParamSpectrumAllRead (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(User Spectrum ROM Read) User Parameter Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gpCameraUserParameter Parameter
	if (gpCameraUserParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(User Spectrum ROM Read) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// Get Flash Address
	adrs = CAMERA_SAVE_USER0_SPECTRUM_ADRS + userNum * CAMERA_SAVE_UINT_SIZE;

	// 読み込み
	if ((status = qspiFlashRead (adrs, (unsigned char *)gpCameraSpectrumParameter, CAMERA_SAVE_SPECTRUM_COMMON_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}
#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)


//**********************************************************************************
// カメラ保存パラメータの設定(ユーザー領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：UserSet番号
//		saveAdrs			：ストレージデバイスへ格納するアドレス
//		pData				：データを格納するポインタ
//		size				：書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamUserWriteRomMulti (int userNum, unsigned int saveAdrs, unsigned char *pData, int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pBuffer = NULL;
	unsigned int flashAdrs;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(User ROM Write) UserNum(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check Size Parameter
	if ((saveAdrs + CAMERA_SAVE_USER_SIZE) < size)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(User ROM Write) Size Parameter Error. Size = %d\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 読み込み用メモリ確保
	if ((pBuffer = malloc (CAMERA_SAVE_USER_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(User ROM Write) Malloc Error\n");
		goto _DONE;
	}

	// Flashアドレス取得
	flashAdrs = CAMERA_SAVE_USER0_ADRS + CAMERA_SAVE_UINT_SIZE * userNum;

	// 読み込み
	if ((status = qspiFlashRead (flashAdrs, (unsigned char *)pBuffer, CAMERA_SAVE_USER_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// イレーズ
	if ((status = qspiFlashSectorErase (flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データ設定
	memcpy ((void *)(pBuffer + saveAdrs), pData, size);

	// 書き込み
	if ((status = qspiFlashWrite (flashAdrs, (unsigned char *)pBuffer, CAMERA_SAVE_USER_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
// カメラ保存パラメータの設定(Spectrum領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：UserSet番号
//		saveAdrs			：ストレージデバイスへ格納するアドレス
//		pData				：データを格納するポインタ
//		size				：書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if defined (MODE_USERSET_SPECTRUM_PARAM)
int cameraParamSpectrumWriteRomMulti (int userNum, unsigned int saveAdrs, unsigned char *pData, int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pBuffer = NULL;
	unsigned int flashAdrs;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Spectrum ROM Write) UserNum(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check Size Parameter
	if ((saveAdrs + CAMERA_SAVE_SPECTRUM_COMMON_SIZE) < size)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Spectrum ROM Write) Size Parameter Error. Size = %d\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 読み込み用メモリ確保
	if ((pBuffer = malloc (CAMERA_SAVE_SPECTRUM_COMMON_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Spectrum ROM Write) Malloc Error\n");
		goto _DONE;
	}

	// Flashアドレス取得
	flashAdrs = CAMERA_SAVE_USER0_SPECTRUM_ADRS + CAMERA_SAVE_UINT_SIZE * userNum;

	// 読み込み
	if ((status = qspiFlashRead (flashAdrs, (unsigned char *)pBuffer, CAMERA_SAVE_SPECTRUM_COMMON_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// イレーズ
	if ((status = qspiFlashSectorErase (flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データ設定
	memcpy ((void *)(pBuffer + saveAdrs), pData, size);

	// 書き込み
	if ((status = qspiFlashWrite (flashAdrs, (unsigned char *)pBuffer, CAMERA_SAVE_SPECTRUM_COMMON_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}
#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)


//**********************************************************************************
// カメラパラメータ保存(共通領域書き込み)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamCommonAllWrite (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check gpCameraCommonParameter Parameter
	if (gpCameraCommonParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common ROM Write) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// イレーズ
	if ((status = qspiFlashSectorErase (CAMERA_SAVE_COMMON_ADRS)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 書き込み(パラメータ)
	if ((status = qspiFlashWrite ((CAMERA_SAVE_COMMON_ADRS+4), (unsigned char *)gpCameraCommonParameter+4, CAMERA_SAVE_COMMON_SIZE-4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// 書き込み(マークデータ)
	if ((status = qspiFlashWrite (CAMERA_SAVE_COMMON_ADRS, (unsigned char *)gpCameraCommonParameter, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// カメラパラメータ取得(共通領域読み込み)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamCommonAllRead (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check gpCameraCommonParameter Parameter
	if (gpCameraCommonParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common ROM Read) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// 読み込み
	if ((status = qspiFlashRead (CAMERA_SAVE_COMMON_ADRS, (unsigned char *)gpCameraCommonParameter, CAMERA_SAVE_COMMON_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// カメラ保存パラメータの設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		saveAdrs			：ストレージデバイスへ格納するアドレス
//		data				：データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamCommonWriteRom (unsigned int saveAdrs, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pBuffer = NULL;

	// 読み込み用メモリ確保
	if ((pBuffer = malloc (CAMERA_SAVE_COMMON_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common ROM Write) Malloc Error\n");
		goto _DONE;
	}

	// 読み込み
	if ((status = qspiFlashRead (CAMERA_SAVE_COMMON_ADRS, (unsigned char *)pBuffer, CAMERA_SAVE_COMMON_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// イレーズ
	if ((status = qspiFlashSectorErase (CAMERA_SAVE_COMMON_ADRS)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データ設定
	pBuffer[(saveAdrs/4)] = data;

	// 書き込み
	if ((status = qspiFlashWrite (CAMERA_SAVE_COMMON_ADRS, (unsigned char *)pBuffer, CAMERA_SAVE_COMMON_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
// カメラ保存パラメータの取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		saveAdrs			：ストレージデバイスへ格納するアドレス
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamCommonReadRom (unsigned int saveAdrs, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common ROM Read) pData NULL Parameter Error\n");
		goto _DONE;
	}

	adrs = saveAdrs + CAMERA_SAVE_COMMON_ADRS;

	// 読み込み
	if ((status = qspiFlashRead (adrs, (unsigned char *)pData, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// カメラ保存パラメータの設定(ユーザー領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		saveAdrs			：ストレージデバイスへ格納するアドレス
//		pData				：データを格納するポインタ
//		size				：書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamCommonWriteRomMulti (unsigned int saveAdrs, unsigned char *pData, int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pBuffer = NULL;
	unsigned int flashAdrs;

	// Check Size
	if ((saveAdrs + CAMERA_SAVE_COMMON_SIZE) < size)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Common ROM Write) Size Parameter Error. Size = %d\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 読み込み用メモリ確保
	if ((pBuffer = malloc (CAMERA_SAVE_COMMON_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common ROM Write) Malloc Error\n");
		goto _DONE;
	}

	// Flashアドレス取得
	flashAdrs = CAMERA_SAVE_COMMON_ADRS;

	// 読み込み
	if ((status = qspiFlashRead (flashAdrs, (unsigned char *)pBuffer, CAMERA_SAVE_COMMON_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// イレーズ
	if ((status = qspiFlashSectorErase (flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データ設定
	memcpy ((void *)(pBuffer + saveAdrs), pData, size);

	// 書き込み
	if ((status = qspiFlashWrite (flashAdrs, (unsigned char *)pBuffer, CAMERA_SAVE_COMMON_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
// カメラパラメータの設定(ユーザー領域/一時メモリへ設定)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：UserSet番号(0=User0/1=User1/2=User2)
//		saveAdrs			：ストレージデバイスへ格納するアドレス
//		adrs				：設定アドレス
//		data				：設定データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamUserWriteMem (int userNum, unsigned int saveAdrs, unsigned int adrs, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	int index;
	unsigned int *pSave;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Common Mem Write) userNum(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check saveAdrs Parameter
	if (saveAdrs > CAMERA_SAVE_USER_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Common Mem Write) Save Address(0x%x) Parameter Error. (Max:%d)\n", saveAdrs, CAMERA_SAVE_USER_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;	
	}

	// Check gpCameraUserParameter Parameter
	if (gpCameraUserParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common Mem Write) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;	
	}

	// 一時アドレス取得
	pSave = gpCameraUserParameter;

	// 保存先アドレス取得
	index = saveAdrs/4;

	// 保存するデータを設定
	pSave[index] = adrs;
	pSave[index+1] = data;

_DONE:
	return (status);
}


//**********************************************************************************
// カメラパラメータの取得(ユーザー領域/一時メモリへ取得)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：UserSet番号(0=User0/1=User1/2=User2)
//		saveAdrs			：ストレージデバイスへ格納されたアドレス
//		pAdrs				：設定アドレスを格納するポインタ
//		pData				：設定データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamUserReadMem (int userNum, unsigned int saveAdrs, unsigned int *pAdrs, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	int index;
	unsigned int *pSave;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Common Mem Read) Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check saveAdrs Parameter
	if (saveAdrs > CAMERA_SAVE_USER_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Common Mem Read)  Save Address(0x%x) Parameter Error. (Max = %d)\n", saveAdrs, CAMERA_SAVE_USER_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common Mem Read) pAdrs NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common Mem Read) pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Check gpCameraUserParameter Parameter
	if (gpCameraUserParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common Mem Read) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// 一時アドレス取得
	pSave = gpCameraUserParameter;

	// 保存先アドレス取得
	index = saveAdrs/4;

	// 保存されているデータを取得
	*pAdrs = pSave[index];
	*pData = pSave[index+1];

_DONE:
	return (status);
}


//**********************************************************************************
// カメラパラメータの設定(共通領域/一時メモリへ設定)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		saveAdrs			：ストレージデバイスへ格納するアドレス
//		data				：設定データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamCommonWriteMem (unsigned int saveAdrs, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	int index;
	unsigned int *pSave;

	// Check saveAdrs Parameter
	if (saveAdrs > CAMERA_SAVE_COMMON_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Common Mem Write) User Parameter Save Address(0x%x) Parameter Error. (Max:%d)\n", saveAdrs, CAMERA_SAVE_COMMON_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gpCameraCommonParameter Parameter
	if (gpCameraCommonParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common Mem Write) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// 一時アドレス取得
	pSave = gpCameraCommonParameter;

	// 保存先アドレス取得
	index = saveAdrs/4;

	// 保存するデータを設定
	pSave[index] = data;
	
_DONE:
	return (status);
}


//**********************************************************************************
// カメラパラメータの取得(共通領域/一時メモリへ取得)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		saveAdrs			：ストレージデバイスへ格納するアドレス
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamCommonReadMem (unsigned int saveAdrs, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	int index;
	unsigned int *pSave;
	
	// Check saveAdrs Parameter
	if (saveAdrs > CAMERA_SAVE_COMMON_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Common Mem Read) User Parameter Save Address(0x%x) Parameter Error. (Max:%d)\n", saveAdrs, CAMERA_SAVE_COMMON_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common Mem Read) pData NULL Parameter Error\n");
		goto _DONE;
	}

	// Check gpCameraCommonParameter Parameter
	if (gpCameraCommonParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common Mem Read) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// 一時アドレス取得
	pSave = gpCameraCommonParameter;

	// 現在のユーザーセットを取得し保存先アドレス取得
	index = saveAdrs/4;
	*pData = pSave[index];
	
_DONE:
	return (status);
}


//**********************************************************************************
// カメラ保存パラメータをレジスタに設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：ユーザーモード(0=User0/1=User1/2=User2)
//		offset				：設定パラメータが保存されているオフセット
//		size				：設定パラメータが保存されているサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamWriteRegister (int userNum, unsigned int offset, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pSave;
	int index;
	unsigned int i;
	unsigned int adrs, data;
	unsigned int black;
#if defined (MODE_SPECTRUM)
	int status;
	int bandMax;
#endif

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Register Write) User Parameter Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset/size Parameter
	if ((offset + size) > CAMERA_SAVE_USER_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Register Write) User Parameter Save Size(0x%x) Parameter Error. (Max:%d)\n", (offset + size), CAMERA_SAVE_USER_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Save Parameter Address
	if (gpCameraUserParameter != NULL)
	{
		// Get Address
		pSave = gpCameraUserParameter;

		// Get Index
		index = offset / 4;

#if defined (MODE_SPECTRUM)
		// 最大バンド数取得
		if ((status = spectrumBandMaxCount (&bandMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

		// Get Parameter Restore
		for (i=0; i<(size/4);)
		{
			adrs = pSave[index + i];
			i++;
			data = pSave[index + i];
			i++;

			//sprintf (gLogMsgBuff, "[%d]adrs=0x%08x/data=0x%08x\n", i, adrs, data);
			//cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

			// 0が設定されている場合は未設定と判断し何もしない
			if (adrs != 0)
			{
				// DOG Offset1
				if (adrs == FPGA_DOG_OFFSET1_ADRS)
				{
					// 黒レベル取得値
					black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
					data -= black;
				}
				// DOG Offset2
				else if (adrs == FPGA_DOG_OFFSET2_ADRS)
				{
					// 黒レベル取得値
					black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
					data += black;
				}
#if defined (MODE_SPECTRUM) && !defined(AXP_AHS052VIR_01) && !defined(AXP_AHS052VIR_02)
				// DOG Band Offset1 & Offset2
				else if ((adrs >= FPGA_DOG_BAND_OFFSET1_ADRS) && (adrs < (FPGA_DOG_BAND_OFFSET1_ADRS + (FPGA_DOG_BAND_INTEVAL * bandMax))))
				{
					if ((adrs&0x0f) == FPGA_DOG_BAND_OFFSET1_ADRS_OFFSET)
					{
						// 黒レベル取得値
						black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
						data -= black;
					}
					else if ((adrs&0x0f) == FPGA_DOG_BAND_OFFSET2_ADRS_OFFSET)
					{
						// 黒レベル取得値
						black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
						data += black;
					}
				}
#endif

				// レジスタ値設定
				OUT32 (adrs, data);
			}
		}
	}

_DONE:
	return (status);
}


#if defined (MODE_IPU_MULTI)
//**********************************************************************************
// カメラ保存パラメータをレジスタに設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：ユーザーモード(0=User0/1=User1/2=User2)
//		offset				：設定パラメータが保存されているオフセット
//		size				：設定パラメータが保存されているサイズ
//		offsetAdrs			：アドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamWriteRegisterOffsetAdrs (int userNum, unsigned int offset, unsigned int size, unsigned int offsetAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pSave;
	int index;
	unsigned int i;
	unsigned int adrs, data;
	unsigned int black;
#if defined (MODE_SPECTRUM)
	int bandMax;
#endif

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save2(Register Write) User Parameter Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset/size Parameter
	if ((offset + size) > CAMERA_SAVE_USER_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save2(Register Write) User Parameter Save Size(0x%x) Parameter Error. (Max:%d)\n", (offset + size), CAMERA_SAVE_USER_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Save Parameter Address
	if (gpCameraUserParameter != NULL)
	{
		// Get Address
		pSave = gpCameraUserParameter;

		// Get Index
		index = offset / 4;

#if defined (MODE_SPECTRUM)
		// 最大バンド数取得
		if ((status = spectrumBandMaxCount (&bandMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

		// Get Parameter Restore
		for (i=0; i<(size/4);)
		{
			adrs = pSave[index + i];
			if (adrs != 0)
				adrs += offsetAdrs;			// OffsetAdrs付加
			i++;
			data = pSave[index + i];
			i++;

			//sprintf (gLogMsgBuff, [%d]adrs=0x%08x/data=0x%08x\n", i, adrs, data);
			//cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

			// 0が設定されている場合は未設定と判断し何もしない
			if (adrs != 0)
			{
				// DOG Offset1
				if (adrs == (offsetAdrs + FPGA_DOG_OFFSET1_ADRS))
				{
					// 黒レベル取得値
					black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
					data -= black;
				}
				// DOG Offset2
				else if (adrs == (offsetAdrs + FPGA_DOG_OFFSET2_ADRS))
				{
					// 黒レベル取得値
					black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
					data += black;
				}
#if defined (MODE_SPECTRUM)
				// DOG Band Offset1 & Offset2
				else if ((adrs >= FPGA_DOG_BAND_OFFSET1_ADRS) && (adrs < (FPGA_DOG_BAND_OFFSET1_ADRS + (FPGA_DOG_BAND_INTEVAL * bandMax))))
				{
					if ((adrs&0x0f) == (offsetAdrs + FPGA_DOG_BAND_OFFSET1_ADRS_OFFSET))
					{
						// 黒レベル取得値
						black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
						data -= black;
					}
					else if ((adrs&0x0f) == (offsetAdrs + FPGA_DOG_BAND_OFFSET2_ADRS_OFFSET))
					{
						// 黒レベル取得値
						black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
						data += black;
					}
				}
#endif

				// レジスタ値設定
				OUT32 (adrs, data);
			}
		}
	}

_DONE:
	return (status);
}
#endif // #if defined (MODE_IPU_MULTI)


//**********************************************************************************
// カメラ保存パラメータをレジスタに設定(スペクトル関連)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：ユーザーモード(0=User0/1=User1/2=User2)
//		offset				：設定パラメータが保存されているオフセット
//		size				：設定パラメータが保存されているサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if defined (MODE_USERSET_SPECTRUM_PARAM)
int cameraParamSpectrumWriteRegister (int userNum, unsigned int offset, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pSave;
	int index;
	unsigned int i;
	unsigned int adrs, data;
	unsigned int black;
	int status;
	int bandMax;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Register Write) Spectrum Parameter Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset/size Parameter
	if ((offset + size) > CAMERA_SAVE_SPECTRUM_COMMON_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(Register Write) Spectrum Parameter Save Size(0x%x) Parameter Error. (Max:%d)\n", (offset + size), CAMERA_SAVE_SPECTRUM_COMMON_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Save Parameter Address
	if (gpCameraSpectrumParameter != NULL)
	{
		// Get Address
		pSave = gpCameraSpectrumParameter;

		// Get Index
		index = offset / 4;

		// 最大バンド数取得
		if ((status = spectrumBandMaxCount (&bandMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get Parameter Restore
		for (i=0; i<(size/4);)
		{
			adrs = pSave[index + i];
			i++;
			data = pSave[index + i];
			i++;

			//sprintf (gLogMsgBuff, "[%d]adrs=0x%08x/data=0x%08x\n", i, adrs, data);
			//cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

			// 0が設定されている場合は未設定と判断し何もしない
			if (adrs != 0)
			{
				// DOG Band Offset1 & Offset2
				if ((adrs >= FPGA_DOG_BAND_OFFSET1_ADRS) && (adrs < (FPGA_DOG_BAND_OFFSET1_ADRS + (FPGA_DOG_BAND_INTEVAL * bandMax))))
				{
					if ((adrs&0x0f) == FPGA_DOG_BAND_OFFSET1_ADRS_OFFSET)
					{
						// 黒レベル取得値
						black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
						data -= black;
					}
					else if ((adrs&0x0f) == FPGA_DOG_BAND_OFFSET2_ADRS_OFFSET)
					{
						// 黒レベル取得値
						black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
						data += black;
					}
				}

				// レジスタ値設定
				OUT32 (adrs, data);
			}
		}
	}

_DONE:
	return (status);
}
#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)


//**********************************************************************************
// カメラ保存マーク情報のクリア(ユーザー領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：ユーザーモード(0=User0/1=User1/2=User2)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamUserMarkClear (int userNum)
{
	int status;
	unsigned int i;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UserSet Default UserNum(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gpCameraUserParameter Parameter
	if (gpCameraUserParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UserSet Default UserNum(%d) NULL Parameter Error\n", userNum);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// カメラパラメータ保存マーク設定
	gpCameraUserParameter[0] = CAMERA_MARK_DATA;

	// 0クリアデータ作成
	for (i=1; i<(CAMERA_SAVE_USER_SIZE/4); i++)
		gpCameraUserParameter[i] = 0;

	// デフォルトパラメータ取得
	cameraParamSetDefault (userNum);

	// 初期化
	if ((status = cameraParamUserAllWrite (userNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// カメラ保存マーク情報のクリア(Spectrum領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：ユーザーモード(0=User0/1=User1/2=User2)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if defined (MODE_USERSET_SPECTRUM_PARAM)
int cameraParamSpectrumMarkClear (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UserSet Spectrum Mark Clear UserNum(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gpCameraSpectrumParameter Parameter
	if (gpCameraSpectrumParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UserSet Spectrum Mark Clear UserNum(%d) NULL Parameter Error\n", userNum);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// カメラパラメータ保存マーク設定
	gpCameraSpectrumParameter[0] = CAMERA_MARK_DATA;

	// 0クリアデータ作成
	for (i=1; i<(CAMERA_SAVE_SPECTRUM_COMMON_SIZE/4); i++)
		gpCameraSpectrumParameter[i] = 0;

	// デフォルトパラメータ取得
	cameraParamSetSpectrumDefault (userNum);

	// 初期化
	if ((status = cameraParamSpectrumAllWrite (userNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}
#endif //#if defined (MODE_USERSET_SPECTRUM_PARAM)


//**********************************************************************************
// カメラ保存マーク情報のクリア(共通領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamCommonMarkClear (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;

	// Check gpCameraCommonParameter Parameter
	if (gpCameraCommonParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common Mark Clear) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// カメラパラメータ保存マーク設定
	gpCameraCommonParameter[CAMERA_SAVE_COMMON_MARK_ADRS/4] = CAMERA_MARK_DATA;

	// 0クリアデータ作成
	for (i=1; i<(CAMERA_SAVE_COMMON_SIZE/4); i++)
		gpCameraCommonParameter[i] = 0;

	// ボーレート
	gpCameraCommonParameter[CAMERA_SAVE_COMMON_BAUDRATE/4] = CAMERA_SAVE_BAUD_DEFAULT;

	// 書き込み
	if ((status = cameraParamCommonAllWrite ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// カメラパラメータのアドレスの取得(ユーザー領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：ユーザーモード(0=User0/1=User1/2=User2)
//		pAdrs				：ユーザー領域アドレスを格納するダブルポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamGetUserAdrs (int userNum, unsigned int **pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save(User Adrs) User Parameter Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(User Adrs) pAdrs NULL Parameter Error\n");
		goto _DONE;
	}

	// Check gpCameraUserParameter Parameter
	if (gpCameraUserParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(User Adrs) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// Set Address
	*pAdrs = (unsigned int *)gpCameraUserParameter;

_DONE:
	return (status);
}


//**********************************************************************************
// カメラパラメータのアドレスの取得(共通領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pAdrs				：共通領域アドレスを格納するダブルポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamGetCommonAdrs (unsigned int **pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common Adrs) pAdrs NULL Parameter Error\n");
		goto _DONE;
	}

	// Check gpCameraCommonParameter Parameter
	if (gpCameraCommonParameter == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Save(Common Adrs) gpCameraUserParameter NULL Parameter Error\n");
		goto _DONE;
	}

	// Set Address
	*pAdrs = (unsigned int *)gpCameraCommonParameter;

_DONE:
	return (status);
}


//**********************************************************************************
// レジスタ情報を取得しカメラパラメータ領域へ展開その後EEPROMへ保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：保存するユーザーパラメータ番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamSaveRegister (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;
	unsigned int size = CAMERA_SAVE_USER_SIZE;
	SAVE_PARAM	*pSaveparam = (SAVE_PARAM *)gpCameraUserParameter;
	unsigned int data, black;
	double tempL, tempH;
	short dataiH, dataiL;
#if defined (MODE_PELTIER)
	double temp;
#endif
#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	int peltierState;
#endif
	float dataf;

	// 先頭はマークなのでその次から
	for (i=1; i<(size/8); i++)
	{
		// レジスタ リストからアドレスを取得
		if (saveParamDataList[i].mode == CAMERA_SAVE_MODE_REG_REF)
		{
			// Dog Offset1
			if (pSaveparam[i].regAdrs == FPGA_DOG_OFFSET1_ADRS)
			{
				// レジスタ値取得
				data = IN32 (pSaveparam[i].regAdrs);

				// 黒レベル取得値
				black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
				data += black;
			}
			// Dog Offset2
			else if (pSaveparam[i].regAdrs == FPGA_DOG_OFFSET2_ADRS)
			{
				// レジスタ値取得
				data = IN32 (pSaveparam[i].regAdrs);

				// 黒レベル取得値
				black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
				data -= black;
			}
#if defined (MODE_SPECTRUM)
			// DOG Band Offset1 & Offset2
			else if ((pSaveparam[i].regAdrs >= FPGA_DOG_BAND_OFFSET1_ADRS) && (pSaveparam[i].regAdrs < (FPGA_DOG_BAND_OFFSET1_ADRS + (FPGA_DOG_BAND_INTEVAL * 96))))
			{
				if ((pSaveparam[i].regAdrs&0x0f) == FPGA_DOG_BAND_OFFSET1_ADRS_OFFSET)
				{
					// レジスタ値取得
					data = IN32 (pSaveparam[i].regAdrs);

					// 黒レベル取得値
					black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
					data += black;
				}
				else if ((pSaveparam[i].regAdrs&0x0f) == FPGA_DOG_BAND_OFFSET2_ADRS_OFFSET)
				{
					// レジスタ値取得
					data = IN32 (pSaveparam[i].regAdrs);

					// 黒レベル取得値
					black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
					data -= black;
				}
				else
				{
					// レジスタ値取得
					data = IN32 (pSaveparam[i].regAdrs);
				}
			}
#endif
			else
			{
				data = IN32 (pSaveparam[i].regAdrs);
			}

			pSaveparam[i].data = data;
			//sprintf (gLogMsgBuff, "Adrs = 0x%x / Data = 0x%x\n", pSaveparam[i].regAdrs,  pSaveparam[i].data);
			//cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		}
		else if (saveParamDataList[i].mode == CAMERA_SAVE_MODE_DATA)
		{
			// センサTarget温度
			if (pSaveparam[i].regAdrs == FPGA_PELTIER_TARGET_ADRS)
			{
#if defined (MODE_SPECTRUM)
	#if defined (MODE_PELTIER_MOUNTING_SWITCH)
				// Peltier実装状態確認
				if ((status = peltierGetMountState (&peltierState)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

				if (peltierState == MODE_ENABLE)
				{
					// 温度を整数に変換して保存
					if (peltierGetTarget (&temp) == AVAL_STATUS_SUCCESS)
					{
						datai = (int)(temp * CAMERA_SAVE_TEMP_UNIT);
						data = (unsigned int)datai;
					}
					else
					{
						data = PELTIER_DEFAULT_TEMP;
					}
				}
				else
				{
					data = PELTIER_DEFAULT_TEMP;
				}
	#else // #if defined (MODE_PELTIER_MOUNTING_SWITCH)
				// 温度を整数に変換して保存
				if (peltierGetTarget (&temp) == AVAL_STATUS_SUCCESS)
				{
					datai = (int)(temp * CAMERA_SAVE_TEMP_UNIT);
					data = (unsigned int)datai;
				}
				else
				{
					data = PELTIER_DEFAULT_TEMP;
				}
	#endif // #if defined (MODE_PELTIER_MOUNTING_SWITCH)

#else // #if defined (MODE_SPECTRUM)
				data = 0;
#endif // #if defined (MODE_SPECTRUM)
			}
			// センサAlam温度
			else if (pSaveparam[i].regAdrs == FPGA_PELTIER_SENSOR_ALM_ADRS)
			{
				// 温度を整数に変換して保存
				if (peltierGetSensorTempAlarm (&tempH, &tempL) == AVAL_STATUS_SUCCESS)
				{
					dataiH = (short)(tempH * CAMERA_SAVE_TEMP_UNIT);
					dataiL = (short)(tempL * CAMERA_SAVE_TEMP_UNIT);
					data =  (unsigned int)((unsigned int)(dataiH<<16) | (unsigned int)(dataiL&0xffff));
				}
				else
				{
					data = (SENSOR_DEFAULT_HIGH_TEMP<<16) | SENSOR_DEFAULT_LOW_TEMP;
				}
			}
			// Case Alam温度
			else if (pSaveparam[i].regAdrs == FPGA_PELTIER_CASE_ALM_ADRS)
			{
				// 温度を整数に変換して保存
				if (peltierGetCaseTempAlarm(&tempH, &tempL) == AVAL_STATUS_SUCCESS)
				{
					dataiH = (short)(tempH * CAMERA_SAVE_TEMP_UNIT);
					dataiL = (short)(tempL * CAMERA_SAVE_TEMP_UNIT);
					data =  (unsigned int)((unsigned int)(dataiH<<16) | (unsigned int)(dataiL&0xffff));
				}
				else
				{
					data = (CASE_DEFAULT_HIGH_TEMP<<16) | CASE_DEFAULT_LOW_TEMP;
				}
			}
			else
			{
				data = IN32 (pSaveparam[i].regAdrs);
			}

			pSaveparam[i].data = data;
		}
	}

	// 書き込み
	if ((status = cameraParamUserAllWrite (userNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// レジスタ情報を取得しカメラパラメータ領域へ展開その後保存(Spectrum領域用)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：保存するユーザーパラメータ番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if defined (MODE_USERSET_SPECTRUM_PARAM)
int cameraParamSaveSpectrumRegister (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;
	unsigned int size = CAMERA_SAVE_SPECTRUM_COMMON_SIZE;
	unsigned int *ptrL = (unsigned int *)gpCameraSpectrumParameter;
	unsigned int data, black;
	int bandMax;
	unsigned int adrs;

	// 最大バンド数取得
	if ((status = spectrumBandMaxCount (&bandMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// アドレス更新
	ptrL++;
	ptrL++;

	// 先頭はマークなのでその次から
	for (i=1; i<(size/8); i++)
	{
		// アドレス取得
		adrs = *ptrL;
		ptrL++;

		// DOG Band Offset1 & Offset2
		if ((adrs >= FPGA_DOG_BAND_OFFSET1_ADRS) && (adrs < (FPGA_DOG_BAND_OFFSET1_ADRS + (FPGA_DOG_BAND_INTEVAL * bandMax))))
		{
			if ((adrs&0x0f) == FPGA_DOG_BAND_OFFSET1_ADRS_OFFSET)
			{
				// レジスタ値取得
				data = IN32 (adrs);

				// 黒レベル取得値
				black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
				data += black;
			}
			else if ((adrs&0x0f) == FPGA_DOG_BAND_OFFSET2_ADRS_OFFSET)
			{
				// レジスタ値取得
				data = IN32 (adrs);

				// 黒レベル取得値
				black = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);
				data -= black;
			}
			else if (adrs != 0)
			{
				// レジスタ値取得
				data = IN32 (adrs);
			}

			*ptrL = data;
		}
		else if (adrs != 0)
		{
			data = IN32 (adrs);
			*ptrL = data;
		}
		
		ptrL++;
		//sprintf (gLogMsgBuff, "Adrs = 0x%08x / Data = 0x%08x / ptrL = 0x%08x\n", adrs,  data, ptrL);
		//cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	}

	// 書き込み
	if ((status = cameraParamSpectrumAllWrite (userNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}
#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)


//**********************************************************************************
// EEPROMからレジスタ情報を取得しカメラパラメータ領域へ展開し、その後レジスタへの初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：ユーザーパラメータ番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraParamRestoreRegister (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pCommonAdrs;
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	int expMode;
#endif

	// 保存パラメータ取得
	if ((status = cameraParamUserAllRead (userNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 保存パラメータ取得(スペクトル関連)
#if defined (MODE_USERSET_SPECTRUM_PARAM)
	if ((status = cameraParamSpectrumAllRead (userNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// カメラ設定マークあり？
	if (*gpCameraUserParameter != CAMERA_MARK_DATA)
		cameraParamUserMarkClear (userNum);	// クリア＆デフォルトパラメータ設定

	// 共通領域のアドレスを取得
	if ((status = cameraParamGetCommonAdrs (&pCommonAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ユーザー領域番号の書き換え
	pCommonAdrs[(CAMERA_SAVE_COMMON_USER_PARAM_MODE_ADRS/4)] = userNum;

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	// Get Auto Exposure Mode
	if ((status = autoBrightGetExposureMode (&expMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (expMode != MODE_DISABLE)
	{
		// Auto Exposure Disable
		if ((status = autoBrightSetExposureMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Get Auto Gain Mode
	if ((status = autoBrightGetGainMode (&expMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (expMode != MODE_DISABLE)
	{
		// Auto Gain Disable
		if ((status = autoBrightSetGainMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// HW Initialize
	hwInitialize2 (HW_INIT_MODE_NOT_POWER_ON);

#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		gigeSetConnectionConfig ();
	}
#endif

_DONE:
	return (status);
}

// eof
