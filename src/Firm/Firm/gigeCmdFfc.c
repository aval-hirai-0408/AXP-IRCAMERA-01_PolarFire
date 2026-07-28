//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// gigeCmdFfc.c - GigE Command FFC Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined(MODE_FFC)
//**********************************************************************************
//	FFC Flashセーブメイン
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNo				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdFfcSaveMain (int ffcNo)
{
	int status = AVAL_STATUS_SUCCESS;
	FFC_PARAM ffcParam;
	int corMode;

	// クリア
	memset ((void *)&ffcParam, 0, sizeof(ffcParam));

	// パラメータ設定
	ffcParam.ffcNo = ffcNo;
	//@@@1ffcParam.mode = MODE_BATCH;
	ffcParam.memType = FFC_MEMORY_EXT;
	ffcParam.userMode = FFC_USER;
	
	// FFC Correction Mode取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// 1次補正
	//--------------------------------------------------------------------------------
	if (corMode == FFC_CORRECTION_MODE_FIRST)
	{
		if ((status = ffcFlashSave  (ffcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	//--------------------------------------------------------------------------------
	// Shading Line補正
	//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		if ((status = ffcFlashSaveShadingLine  (ffcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Flashセーブメイン
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNo				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdFfcSaveAdminMain (int ffcNo)
{
	int status = AVAL_STATUS_SUCCESS;
	FFC_PARAM ffcParam;
	int corMode;

	// クリア
	memset ((void *)&ffcParam, 0, sizeof(ffcParam));

	// パラメータ設定
	ffcParam.ffcNo = ffcNo;
	//@@@1ffcParam.mode = MODE_BATCH;
	ffcParam.memType = FFC_MEMORY_EXT;
	ffcParam.userMode = FFC_ADMIN;

	// FFC Correction Mode取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// 1次補正
	//--------------------------------------------------------------------------------
	if (corMode == FFC_CORRECTION_MODE_FIRST)
	{
		if ((status = ffcFlashSave (ffcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	//--------------------------------------------------------------------------------
	// Shading Line補正
	//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		if ((status = ffcFlashSaveShadingLine (ffcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif
	
_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Defaultコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdFfcDefault (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int saveLed0 = -1;
	int corMode;
	
	// 調整ステート
	saveLed0 = ledSettingState ();

	// FFC Correction Mode取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// 1次補正
	//--------------------------------------------------------------------------------
	if (corMode == FFC_CORRECTION_MODE_FIRST)
	{
		if ((status = ffcDefault ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	//--------------------------------------------------------------------------------
	// Shading Line補正
	//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		if ((status = ffcDefaultShadingLine ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	
_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	// OK/ERROR 設定
	return (status);
}


//**********************************************************************************
//	FFC Blackコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNo				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdFfcBlackMain (int ffcNo)
{
	int status = AVAL_STATUS_SUCCESS;
	FFC_PARAM ffcParam;
	unsigned int expTime;
	unsigned int blackTarget, bit;
	double targetTemp;
	double frameRateD;
	int calc;
	int corMode;

	// フレームレート取得
	if ((status = acquisitionGetFrameRate(&frameRateD)) !=AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 露光時間取得
	if ((status = acquisitionGetExposure(&expTime)) !=AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 黒レベル取得
	if ((status = ffcGetBlackTarget (&blackTarget)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status =  ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	blackTarget /= calc;

	// Target温度取得
	if ((status = peltierGetTarget (&targetTemp)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Black Image
	//--------------------------------------------------------------------------------

	// クリア
	memset ((void *)&ffcParam, 0, sizeof(ffcParam));

	// パラメータ設定
	ffcParam.ffcNo = ffcNo;
	//@@@1ffcParam.mode = MODE_BATCH;
	ffcParam.frameRate = frameRateD;
	ffcParam.exposure = expTime;
	ffcParam.target = blackTarget;
	ffcParam.bit = bit;
	ffcParam.sensorTemp = targetTemp;
	ffcParam.memType = FFC_MEMORY_EXT;
	ffcParam.userMode = 0;

	// FFC Correction Mode取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC Black

	//--------------------------------------------------------------------------------
	// 1次補正
	//--------------------------------------------------------------------------------
	if (corMode == FFC_CORRECTION_MODE_FIRST)
	{
		if ((status = ffcBlack (ffcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	//--------------------------------------------------------------------------------
	// Shading Line補正
	//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		if ((status = ffcBlack (ffcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Whiteコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNo				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdFfcWhiteMain (int ffcNo)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int frameRate, expTime;
	unsigned int bit;
	unsigned int whiteTarget;
	double targetTemp;
	FFC_PARAM ffcParam;
	double frameRateD;
	int calc;
	int corMode;

	// フレームレート取得
	if ((status = acquisitionGetFrameRate(&frameRateD)) !=AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 露光時間取得
	if ((status = acquisitionGetExposure(&expTime)) !=AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 白レベル取得
	if ((status = ffcGetWhiteTarget (&whiteTarget)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status =  ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	whiteTarget /= calc;

	// Target温度取得
	if ((status = peltierGetTarget (&targetTemp)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// White Image
	//--------------------------------------------------------------------------------

	// クリア
	memset ((void *)&ffcParam, 0, sizeof(ffcParam));

	// パラメータ設定
	ffcParam.ffcNo = ffcNo;
	//@@@1ffcParam.mode = MODE_BATCH;
	ffcParam.frameRate = frameRateD;
	ffcParam.exposure = expTime;
	ffcParam.target = whiteTarget;
	ffcParam.bit = bit;
	ffcParam.sensorTemp = targetTemp;
	ffcParam.memType = FFC_MEMORY_EXT;
	ffcParam.userMode = 0;

	// FFC Correction Mode取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC White

	//--------------------------------------------------------------------------------
	// 1次補正
	//--------------------------------------------------------------------------------
	if (corMode == FFC_CORRECTION_MODE_FIRST)
	{
		if ((status = ffcWhite (ffcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	//--------------------------------------------------------------------------------
	// Shading Line補正
	//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		if ((status = ffcWhite (ffcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

_DONE:
	return (status);
}


#if defined(MODE_FFC_SHADING_LINE)
//**********************************************************************************
//	FFC Shading Line Mainコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNo				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdFfcShadingLineMain (int ffcNo)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int expTime;
	unsigned int bit;
	unsigned int whiteTarget;
	double targetTemp;
	double frameRateD;
	FFC_PARAM ffcParam;
	int corMode;
	
	// FFC Correction Mode取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (corMode != FFC_CORRECTION_MODE_SHADING_LINE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	// フレームレート取得
	if ((status = acquisitionGetFrameRate(&frameRateD)) !=AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 露光時間取得
	if ((status = acquisitionGetExposure(&expTime)) !=AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 白レベル値取得
	if ((status = ffcGetBlackTarget (&whiteTarget)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status =  ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	whiteTarget /= calc;

	// Target温度取得
	if ((status = peltierGetTarget (&targetTemp)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//--------------------------------------------------------------------------------
	// Shading Line
	//--------------------------------------------------------------------------------

	// クリア
	memset ((void *)&ffcParam, 0, sizeof(ffcParam));

	// パラメータ設定
	ffcParam.ffcNo = ffcNo;
	//@@@1ffcParam.mode = MODE_BATCH;
	ffcParam.frameRate = frameRateD;
	ffcParam.exposure = expTime;
	ffcParam.target = whiteTarget;
	ffcParam.bit = bit;
	ffcParam.sensorTemp = targetTemp;
	ffcParam.memType = FFC_MEMORY_EXT;
	ffcParam.userMode = 0;

	// FFC Shading Line
	if ((status = ffcShadingLine (ffcParam)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}
#endif // #if defined (MODE_FFC_SHADING_LINE)


//**********************************************************************************
//	FFC補正Mode切り替えコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：補正モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdFfcCorecctionMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int ffcNum;

	// Check mode Parameter
	if ((mode < FFC_CORRECTION_MODE_MIN) || (mode > FFC_CORRECTION_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC mode(%d) Parameter Parameter Error.(Min:%d / Max:%d)\n", mode, FFC_CORRECTION_MODE_MIN, FFC_CORRECTION_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// FFC番号取得
	if ((status = ffcGetLoadNum (&ffcNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC Correction Mode設定
	if ((status = ffcSetCorMode (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC Load Main
	if ((status = ffcLoadMain (ffcNum, FFC_USER)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}

#endif // #if defined(MODE_FFC)

// eof
