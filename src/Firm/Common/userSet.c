//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// userSet.c - UserSet Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	UserSet Load
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：UserSe番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int userSetLoad (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UserSet Load UserNum(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// レジスタ情報取得
	if ((status = cameraParamRestoreRegister (userNum))!= AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	UserSet Save
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：UserSe番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int userSetSave (int userNum)
{
	int status;
#if defined(MODE_LUT)
	unsigned int adrs;
#endif
	
	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UserSet Save UserNum(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Factory番号かのCheck
	if (userNum == CAMERA_FACTORY_NUM)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UserSet Save UserNum(%d) Default Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN+1, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// レジスタ情報取得
	if ((status = cameraParamSaveRegister (userNum))!= AVAL_STATUS_SUCCESS)
	{
		goto _DONE;
	}

#if defined (MODE_USERSET_SPECTRUM_PARAM)
	// レジスタ情報取得(スペクトル関連)
	if ((status = cameraParamSaveSpectrumRegister (userNum))!= AVAL_STATUS_SUCCESS)
	{
		goto _DONE;
	}
#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)


	// GigEのみ
	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20) || (gInterFaceID == INTERFACE_CXP))
	{
		// Factory
		if (userNum == CAMERA_FACTORY_NUM)
			goto _DONE;
		
#if defined(MODE_LUT)
		#if (LUT_SELECT_NUM == 1)
		{
			// LUTテーブルアドレス取得
			if ((status = lutGetMemAdrs (LUT_SELECT1, &adrs)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// LUTデータFlashへの書き込み
			if ((status = lutSetFlash (LUT_SELECT1, (unsigned int *)adrs, LUT_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		#else // #if (LUT_SELECT_NUM == 1)
		{
			// LUTテーブルアドレス取得
			if ((status = lutGetMemAdrs (userNum, &adrs)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// LUTデータFlashへの書き込み
			if ((status = lutSetFlash (userNum, (unsigned int *)adrs, LUT_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		#endif // #if (LUT_SELECT_NUM == 1)
#endif
	}


#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
	// バンドゲイン保存
	if ((status = bgfBandGainSave(userNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	UserSet Default
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：UserSe番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int userSetDefault (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;

	// Factory番号かのCheck
	if (userNum == CAMERA_FACTORY_NUM)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UserSet Clear UserNum(%d) Default Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN+1, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// ユーザー領域クリア
	if ((status = cameraParamUserMarkClear (userNum))!= AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 現在ユーザセットと同一ならば、現在のパラメータもデフォルトに戻す(Ver.1.2)
	if (userNum == CAMERA_SAVE_USER_NUM)
	{
#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
		// バンドゲイン初期化
		if((status = bgfBandGainDefault()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// バンドゲイン保存
		if((status = bgfBandGainSave(userNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

		// Userカメラパラメータリストア
		if ((status = userSetLoad (userNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	UserSet Boot設定コマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：UserSe番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int userSetBoot (int userNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pCommonAdrs;
	
	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UserSet Boot UserNum(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 共通領域のアドレスを取得
	if ((status = cameraParamGetCommonAdrs (&pCommonAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 共通領域書き込み(ユーザーを設定)(Ver.1.2)
	if ((status = cameraParamCommonWriteRom (CAMERA_SAVE_COMMON_USER_PARAM_MODE_ADRS, userNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	UserSet Boot取得コマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pUserNum			：UserSe番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int userGetBoot (int *pUserNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int userNum;
	unsigned int *pCommonAdrs;

	// Check pUserNum Parameter
	if (pUserNum == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "UserSet Boot pUserNum NULL Parameter Error\n");
		goto _DONE;
	}

	// 共通領域のアドレスを取得
	if ((status = cameraParamGetCommonAdrs (&pCommonAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 共通領域 起動番号の取得
	cameraParamCommonReadRom (CAMERA_SAVE_COMMON_USER_PARAM_MODE_ADRS, &userNum);

	// ユーザーパラメータ番号
	*pUserNum = userNum;

_DONE:
	return (status);
}


//**********************************************************************************
//	UserSet Clearコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int userSetFactoryClear (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// カメラパラメータクリア
	data = 0xffffffff;
	if ((status = cameraParamUserWriteRomMulti (CAMERA_FACTORY_NUM, CAMERA_SAVE_MARK_ADRS, (unsigned char *)&data, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_USERSET_SPECTRUM_PARAM)
	// カメラパラメータクリア(Sppectrum関連)
	if ((status = cameraParamSpectrumWriteRomMulti (CAMERA_FACTORY_NUM, CAMERA_SAVE_MARK_ADRS, (unsigned char *)&data, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)

	// 起動Userカメラパラメータ設定
	if ((status = userSetBoot (CAMERA_FACTORY_NUM)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}

// eof
