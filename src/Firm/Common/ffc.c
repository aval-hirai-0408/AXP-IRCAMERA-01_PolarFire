//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// ffc.c - FFC Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"

//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gFfcStatus = 0;								// FFC Status
FFCINFO ffcBlackInfo;							// FFC黒レベル情報保存用
FFCINFO ffcWhiteInfo;							// FFC白レベル情報保存用


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern unsigned int *pImageFfcSave[];			// FFCデータ保存用


//**********************************************************************************
//	FFC初期化(User)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：FFC Flashアドレス
//		memAdrs				：FFCメモリアドレス
//		mode				：0=無効/0x01=オフセット有効/0x10=ゲイン有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcInitialize (unsigned int flashAdrs, unsigned int memAdrs, int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	status = ffcInitializeMain (flashAdrs, memAdrs,  mode, FFC_USER);
	
	return (status);
}


//**********************************************************************************
//	FFC初期化(Admin)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：FFC Flashアドレス
//		memAdrs				：FFCメモリアドレス
//		mode				：0=無効/0x01=オフセット有効/0x10=ゲイン有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcInitializeAdmin (unsigned int flashAdrs, unsigned int memAdrs, int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	status = ffcInitializeMain (flashAdrs, memAdrs, mode, FFC_ADMIN);
	
	return (status);
}


//**********************************************************************************
//	FFC初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：FFC Flashアドレス
//		memAdrs				：FFCメモリアドレス
//		mode				：0=無効/0x01=オフセット有効/0x10=ゲイン有効
//		userMode			：0=User/1=Admin
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcInitializeMain (unsigned int flashAdrs, unsigned int memAdrs, int mode, int userMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int ffcNum;
	unsigned int blackTarget;

	// Get FFC Number
	if (userMode == FFC_USER)
	{	
		if ((status = ffcGetNumFromFlash (flashAdrs, &ffcNum)) != AVAL_STATUS_SUCCESS)
		{
			gFfcStatus = status;
			goto _DONE;
		}
	}
	else
	{
		if ((status = ffcGetNumFromFlashAdmin (flashAdrs, &ffcNum)) != AVAL_STATUS_SUCCESS)
		{
			gFfcStatus = status;
			goto _DONE;
		}
	}

_RETRY:
	// Get Black Target
	if (userMode == FFC_USER)
	{	
		if ((status = ffcGetBlackTargetFromFlash (ffcNum, &blackTarget)) != AVAL_STATUS_SUCCESS)
		{
			gFfcStatus = status;
			goto _DONE;
		}
	}
	else
	{
		if ((status = ffcGetBlackTargetFromFlashAdmin (ffcNum, &blackTarget)) != AVAL_STATUS_SUCCESS)
		{
			gFfcStatus = status;
			goto _DONE;
		}
	}

	// Check blackTarget Parameter
	if (blackTarget > FFC_BLACK_LEVEL_INIT_MAX)
	{
		if (ffcNum != FFC_FACTORY_NUMBER)
		{
			sprintf (gLogMsgBuff, "FFC Black Target(%d) Parameter Error. FFC = %d\nChange FFC FFC_FACTORY_NUMBER Mode!\n", blackTarget, ffcNum);
			cameraLogMsg (MSG_LEVEL_WARNING, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

			ffcNum = FFC_FACTORY_NUMBER;

			// FFC Flashアドレス取得
			if ((status = ffcGetFlashAdrs (ffcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			goto _RETRY;
		}

		sprintf (gLogMsgBuff, "FFC Black Target(%d) Parameter Error. FFC = %d\nFFC Mode Off\n", blackTarget, ffcNum);
		cameraLogMsg (MSG_LEVEL_WARNING, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	
		// FFC黒レベルが無効な為、FFC OFF
		mode = 0;

		// 黒レベル目標値=0
		blackTarget = 0;

		// Error Status
		gFfcStatus = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_VALUE);
		status = gFfcStatus;

		// FFC番号をメモリへ保存
		ffcSetLoadNum (ffcNum);

		// 黒レベル設定
		ffcSetBlackTarget (blackTarget);

		// 白レベルターゲット値
		OUT32 (FIRM_DATA_FFC_WHITE_ADRS, 0);

		// 調整パラメータをDDRに設定
		ffcSetAdjustParamZero();
		
		// FFCパラメータ初期化 & Disable
		ffcRegInit (0, FFC_DATA_SIZE, mode, FPGA_FFC_CTRL_SELECT_FIRST);
	}
	else
	{
		// FFC番号をメモリへ保存
		ffcSetLoadNum (ffcNum);

		// 黒レベル設定
		ffcSetBlackTarget (blackTarget);

#if defined(MODE_BINNING)
		//ビニング時黒レベル設定
		if ((status = aoiSetBinningOffsetBase (blackTarget)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

		// FFCオフセット/ゲインデータの読み込み
		if (userMode == FFC_USER)
			ffcToMemory (flashAdrs, memAdrs, FFC_DATA_SIZE);
		else
			ffcToMemoryAdmin (flashAdrs, memAdrs, FFC_DATA_SIZE);

		// FFCパラメータ初期化 & Enable
		ffcRegInit (memAdrs, FFC_DATA_SIZE, mode, FPGA_FFC_CTRL_SELECT_FIRST);

		if (userMode == FFC_USER)
		{
			// FFC黒レベル情報読み込み
			if ((status = ffcAdjustInfoBlackRead (ffcNum, FFC_USER, &ffcBlackInfo)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// FFC白レベル情報読み込み
			if ((status = ffcAdjustInfoWhiteRead (ffcNum, FFC_USER, &ffcWhiteInfo)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		else
		{
			// FFC黒レベル情報読み込み
			if ((status = ffcAdjustInfoBlackRead (ffcNum, FFC_ADMIN, &ffcBlackInfo)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// FFC白レベル情報読み込み
			if ((status = ffcAdjustInfoWhiteRead (ffcNum, FFC_ADMIN, &ffcWhiteInfo)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		// 黒レベルターゲット値
		// 上記のffcSetBlackTargetで設定
		
		// 白レベルターゲット値
#if defined (MODE_FFC_BIT_CALC)
		ffcSetWhiteTarget (ffcWhiteInfo.target);
#else
		OUT32 (FIRM_DATA_FFC_WHITE_ADRS, ffcWhiteInfo.target);
#endif

		// FFC調整パラメータ設定
		ffcSetAdjustParam();

		// キャッシュFlash
		cacheFlush ();

		DEBUG_PRINT ("FFC Initialize Flash = 0x%x, Memory = 0x%x, Size = 0x%x, DMA Wait = 0x%x, Mode = 0x%x\n", flashAdrs, memAdrs, FFC_DATA_SIZE, dmaWait, mode);
		DEBUG_PRINT ("Black Target = %d / White Target = %d\n", blackTarget, ffcWhiteInfo.target);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCレジスタ初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：FFCメモリアドレス
//		size				：FFCメモリサイズ
//		dmaWait				：FFC DMA設定
//		mode				：0=無効/0x01=オフセットEnable/0x10=ゲイン有効
//		select				：0=1次補正/1=2次補正/2=Shading補正
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcRegInit (unsigned int adrs, unsigned int size, int mode, int select)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int selectBit;

	// Address設定
	OUT32 (FPGA_FFC_DMA_ADRS, adrs);

	#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_FFC2_DMA_ADRS, (adrs + FFC_MEMORY_IPU_MULTI_INTERVAL));
	#endif

	// Check select Parameter
	if ((select < FPGA_FFC_CTRL_SELECT_MIN) || (select > FPGA_FFC_CTRL_SELECT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Reg Init select(%d) Parameter Error. (Min:%d / Max:%d)\n", select, FPGA_FFC_CTRL_SELECT_MIN, FPGA_FFC_CTRL_SELECT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Set Bit
	selectBit = select << FPGA_FFC_CTRL_SELECT_SHIFT;

	// FFC Enable
	OUT32 (FPGA_FFC_CTRL_ADRS, 0);						// OFF
#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_FFC2_CTRL_ADRS, 0);						// OFF
#endif

	OUT32 (FPGA_FFC_CTRL_ADRS, selectBit);				// Select
#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_FFC2_CTRL_ADRS, selectBit);				// Select
#endif

	OUT32 (FPGA_FFC_CTRL_ADRS, (selectBit | mode));		// Select & ON
#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_FFC2_CTRL_ADRS, (selectBit | mode));	// Select & ON
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjustパラメータ 0初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetAdjustParamZero (void)
{
	int status = AVAL_STATUS_SUCCESS;

	//------------------------------------------------------------
	// ターゲット
	//------------------------------------------------------------

	// 黒レベル設定
    // 上記で設定
	// ffcSetBlackTarget (blackTarget);

	// 中間レベルターゲット値
    // 上記で設定
	//OUT32 (FIRM_DATA_FFC_MEDIUM_ADRS, 0);

	// 白レベルターゲット値
    // 上記で設定
	//OUT32 (FIRM_DATA_FFC_WHITE_ADRS, 0);

	// 黒レベル設定
	OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_ADRS, 0);

	// 白レベルターゲット値
	OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_ADRS, 0);

	//------------------------------------------------------------
	// レート
	//------------------------------------------------------------

	// 黒レベルレート
	OUTF (FIRM_DATA_FFC_ADJUST_BLACK_RATE_ADRS, 0);

	// 白レベルレート
	OUTF (FIRM_DATA_FFC_ADJUST_WHITE_RATE_ADRS, 0);

	//------------------------------------------------------------
	// 露光時間
	//------------------------------------------------------------

	// 黒レベル露光時間
	OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_EXP_ADRS, 0);

	// 白レベル露光時間
	OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_EXP_ADRS, 0);

	//------------------------------------------------------------
	// 温度
	//------------------------------------------------------------

	// 黒レベル温度
	OUTF (FIRM_DATA_FFC_ADJUST_BLACK_TEMP_ADRS, 0);

	// 白レベル温度
	OUTF (FIRM_DATA_FFC_ADJUST_WHITE_TEMP_ADRS, 0);

	//------------------------------------------------------------
	// Gainx
	//------------------------------------------------------------

	// 黒レベルGainX
	OUTF (FIRM_DATA_FFC_ADJUST_BLACK_GAIN_ADRS, 0);

	// 白レベルGainX
	OUTF (FIRM_DATA_FFC_ADJUST_WHITE_GAIN_ADRS, 0);

	//------------------------------------------------------------
	// Bit
	//------------------------------------------------------------

	// 黒レベルBit
	OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_BIT_ADRS, 0);

	// 白レベルBit
	OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_BIT_ADRS, 0);

	return (status);
}


//**********************************************************************************
//	FFC Adjustパラメータ初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetAdjustParam (void)
{
	int status = AVAL_STATUS_SUCCESS;

	//------------------------------------------------------------
	// ターゲット
	//------------------------------------------------------------

	// 黒レベルターゲット値
	// 上記のffcSetBlackTargetで設定

	// 中間レベルターゲット値
	// 上記で設定
	//OUT32 (FIRM_DATA_FFC_MEDIUM_ADRS, 0);

	// 白レベルターゲット値
	// 上記で設定
	//OUT32 (FIRM_DATA_FFC_WHITE_ADRS, ffcWhiteInfo.target);

	// 黒レベル設定
	OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_ADRS, ffcBlackInfo.target);

	// 白レベルターゲット値
	OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_ADRS, ffcWhiteInfo.target);

	//------------------------------------------------------------
	// レート
	//------------------------------------------------------------

	// 黒レベルレート
	OUTF (FIRM_DATA_FFC_ADJUST_BLACK_RATE_ADRS, (float)ffcBlackInfo.frameRate);

	// 白レベルレート
	OUTF (FIRM_DATA_FFC_ADJUST_WHITE_RATE_ADRS, (float)ffcWhiteInfo.frameRate);

	//------------------------------------------------------------
	// 露光時間
	//------------------------------------------------------------

	// 黒レベル露光時間
	OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_EXP_ADRS, ffcBlackInfo.expTime);

	// 白レベル露光時間
	OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_EXP_ADRS, ffcWhiteInfo.expTime);

	//------------------------------------------------------------
	// 温度
	//------------------------------------------------------------

	// 黒レベル温度
	OUTF (FIRM_DATA_FFC_ADJUST_BLACK_TEMP_ADRS, (float)ffcBlackInfo.sensorTemp);

	// 白レベル温度
	OUTF (FIRM_DATA_FFC_ADJUST_WHITE_TEMP_ADRS, (float)ffcWhiteInfo.sensorTemp);

	//------------------------------------------------------------
	// Gainx
	//------------------------------------------------------------

	// 黒レベルGainX
	OUTF (FIRM_DATA_FFC_ADJUST_BLACK_GAIN_ADRS, (float)ffcBlackInfo.digitalGain);

	// 白レベルGainX
	OUTF (FIRM_DATA_FFC_ADJUST_WHITE_GAIN_ADRS, (float)ffcWhiteInfo.digitalGain);

	//------------------------------------------------------------
	// Bit
	//------------------------------------------------------------

	// 黒レベルBit
	OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_BIT_ADRS, ffcBlackInfo.bit);

	// 白レベルBit
	OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_BIT_ADRS, ffcWhiteInfo.bit);

	return (status);
}


//**********************************************************************************
//	FFC内部メモリデータ使用
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNo				：FFC番号
//		mode				：0=無効/0x01=オフセットEnable/0x10=ゲイン有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcRegInitIntMem (int ffcNo, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int memAdrs;
	unsigned int extMemAdrs, intMemAdrs;
	
	//------------------------------------------------------------
	// 内部メモリアドレスから外部メモリアドレスにコピー(データを保持するために)
	//------------------------------------------------------------

	// 外部メモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcNo, FFC_MEMORY_EXT, &extMemAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 内部メモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcNo, FFC_MEMORY_INT, &intMemAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データコピー
	if ((status = ffcSetExtMemory (extMemAdrs, intMemAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// メモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcNo, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// キャッシュFlash
	cacheFlush ();

	// FFCパラメータ初期化 & Enable
	ffcRegInit (memAdrs, FFC_DATA_SIZE, mode, FPGA_FFC_CTRL_SELECT_FIRST);

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Enable/Disable設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		offsetMode			：0=オフセットDisable/1=オフセットEnable/それ以外=現状維持
//		gainMode			：0=ゲインDisable/1=ゲイントEnable/それ以外=現状維持
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetMode (int offsetMode, int gainMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;
	int startMode = 0;

	// Check offsetMode Parameter
	if ((offsetMode != FFC_ENABLE) && (offsetMode != FFC_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset mode[%d] Parameter Error. (Disable:%d / Enable:%d)\n", offsetMode, FFC_DISABLE, FFC_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check gainMode Parameter
	if ((gainMode != FFC_ENABLE) && (gainMode != FFC_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain mode[%d] Parameter Error. (Disable:%d / Enable:%d)\n", gainMode, FFC_DISABLE, FFC_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get FFC Mode
	data = IN32 (FPGA_FFC_CTRL_ADRS);

	// Set Offset
	if (offsetMode == FFC_ENABLE)
		data |= FPGA_FFC_CTRL_OFFSET_ENABLE_BIT;
	else if (offsetMode == FFC_DISABLE)
		data &= ~FPGA_FFC_CTRL_OFFSET_ENABLE_BIT;

	// Set Gain
	if (gainMode == FFC_ENABLE)
		data |= FPGA_FFC_CTRL_GAIN_ENABLE_BIT;
	else if (gainMode == FFC_DISABLE)
		data &= ~FPGA_FFC_CTRL_GAIN_ENABLE_BIT;

	// FFC Enable
	OUT32 (FPGA_FFC_CTRL_ADRS, data);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_FFC2_CTRL_ADRS, data);
#endif

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	FFC Enable/Disable取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pOffsetMode			：0=オフセットDisable/1=オフセットEnableを格納するポインタ
//		pGainMode			：0=ゲインDisable/1=ゲイントEnableを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetMode (int *pOffsetMode, int *pGainMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pOffsetMode Parameter
	if (pOffsetMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Mode pOffsetMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pGainMode Parameter
	if (pOffsetMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Mode pGainMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Get FFC Mode
	data = IN32 (FPGA_FFC_CTRL_ADRS);

	// Get Offset Mode
	if (data & FPGA_FFC_CTRL_OFFSET_ENABLE_BIT)
		*pOffsetMode = FFC_ENABLE;
	else
		*pOffsetMode = FFC_DISABLE;

	// Get Gain Mode
	if (data & FPGA_FFC_CTRL_GAIN_ENABLE_BIT)
		*pGainMode = FFC_ENABLE;
	else
		*pGainMode = FFC_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータをロードする番号を設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetLoadNum (int ffcNum)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Load Number ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// FFC番号設定
	status = cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_FFC_NUMBER_ADRS, 0, ffcNum);

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータをロードしている番号を設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFfcNum				：FFC番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetLoadNum (int *pFfcNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int ffcNumAdrs;

	// Check pFfcNum Parameter
	if (pFfcNum == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Load Number pFfcNum NULL Parameter Error.\n");
		goto _DONE;
	}

	// FFC番号取得
	status = cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_FFC_NUMBER_ADRS, &ffcNumAdrs, (unsigned int *)pFfcNum);

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Load Main
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：ffc番号
//		userMode			：0=User/1=Admin
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcLoadMain (int ffcNum, int userMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int offset1, offset2;
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int lineStart, ffcHeight;
	int hsMode;
#endif
	int corMode;

	// デジタルオフセット1取得
	if ((status = digitalGetOffset1 (&offset1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// デジタルオフセット2取得
	if ((status = digitalGetOffset (&offset2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SPECTRUM)
	// デジタルBandオフセット取得
	if ((status = digitalBandOffsetSave ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// FFC Correction Mode取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// 1次補正
	//--------------------------------------------------------------------------------
	if (corMode == FFC_CORRECTION_MODE_FIRST)
	{
		if (userMode == FFC_USER)
		{
			if ((status = ffcLoad (ffcNum)) != AVAL_STATUS_SUCCESS)
			{
				digitalSetOffset1 (0);
				digitalSetOffset (0);
				goto _DONE;
			}
		}
		else
		{
			if ((status = ffcLoadAdmin (ffcNum)) != AVAL_STATUS_SUCCESS)
			{
				digitalSetOffset1 (0);
				digitalSetOffset (0);
				goto _DONE;
			}
		}
	}

	//--------------------------------------------------------------------------------
	// Shading Line補正
	//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		if ((status = ffcLoadSadingLine (ffcNum)) != AVAL_STATUS_SUCCESS)
		{
			digitalSetOffset1 (0);
			digitalSetOffset (0);
			goto _DONE;
		}
	}
#endif

	// デジタルオフセット1設定
	if ((status = digitalSetOffset1 (offset1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// デジタルオフセット2設定
	if ((status = digitalSetOffset (offset2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


#if defined (MODE_SPECTRUM)
	// デジタルBandオフセット設定
	if ((status = digitalBandOffsetRestore ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif


#if defined (MODE_FRAMERATE_HIGH_SPEED)
	// High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (hsMode == MODE_ENABLE)
	{
		// 現在のサイズを取得
		if ((status =aoiGetHeight (&ffcHeight)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC Line Start取得
		if ((status = ffcGetReplacementLine (&lineStart, ffcHeight, corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFCデータ入れ替え
		if ((status = ffcDataReplacement (lineStart, ffcHeight, corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータをFlashからメモリにロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcLoad (int ffcNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int memAdrs, flashAdrs;
	int startFlag = 0;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Load ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;

	}

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	acquisitionAbort ();

	// FFCメモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC Flashアドレス取得
	if ((status = ffcGetFlashAdrs (ffcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC初期化
	if ((status = ffcInitialize (flashAdrs, memAdrs, (FPGA_FFC_CTRL_GAIN_ENABLE_BIT | FPGA_FFC_CTRL_OFFSET_ENABLE_BIT))) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startFlag == 1)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	FFCデータをFlashからメモリにロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcLoadAdmin (int ffcNum)
{
	int status;
	unsigned int memAdrs, flashAdrs;
	int min, max;
	int startFlag = 0;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_ADMIN_MIN) || (ffcNum > FFC_NUMBER_ADMIN_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Load ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_ADMIN_MIN, FFC_NUMBER_ADMIN_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	acquisitionAbort ();

	// FFCメモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC Flashアドレス取得
	if ((status = ffcGetFlashAdrsAdmin (ffcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC初期化
	if ((status = ffcInitializeAdmin (flashAdrs, memAdrs, (FPGA_FFC_CTRL_GAIN_ENABLE_BIT | FPGA_FFC_CTRL_OFFSET_ENABLE_BIT))) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startFlag == 1)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	FFCデータをFlashメモリにセーブ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		memType				：0=OS管理内/1=OS管理外
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSave (int ffcNum, int memType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int memAdrs, flashAdrs;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Save ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check memType Parameter
	if ((memType != FFC_MEMORY_EXT) && (memType != FFC_MEMORY_INT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Save memType(%d) Parameter Error.(EXT:%d / INT:%d)\n", memType, FFC_MEMORY_EXT, FFC_MEMORY_INT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	// Get Flash Adrs
	if ((status = ffcGetFlashAdrs (ffcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Memory Adrs
	if ((status = ffcGetMemAdrs (ffcNum, memType, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFCデータをFlashに書き込み
	if ((status = ffcToFlash (flashAdrs, memAdrs, FFC_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータをFlashメモリにセーブ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		memType				：0=OS管理内/1=OS管理外
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSaveAdmin (int ffcNum, int memType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int memAdrs, flashAdrs;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Admin Save ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check memType Parameter
	if ((memType != FFC_MEMORY_EXT) && (memType != FFC_MEMORY_INT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Admin Save memType(%d) Parameter Error.(EXT:%d / INT:%d)\n", memType, FFC_MEMORY_EXT, FFC_MEMORY_INT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Flash Adrs
	if ((status = ffcGetFlashAdrsAdmin (ffcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Memory Adrs
	if ((status = ffcGetMemAdrs (ffcNum, memType, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFCデータをFlashに書き込み
	if ((status = ffcToFlashAdmin (flashAdrs, memAdrs, FFC_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCパラメータ情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMemAdrs			：FFCメモリアドレスを格納するポインタ
//		pSize				：FFCサイズを格納するポインタ
//		pDmaWait			：FFC DMA設定を格納するポインタ
//		pMode				：FFCオフセット有効/無効状態を格納するポインタ(0x01=オフセット有効/0x10=ゲイント有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetParam (unsigned int *pMemAdrs, unsigned int *pSize, unsigned int *pDmaWait, int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int corMode;

	// Check pMemAdrs Parameter
	if (pMemAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Param pMemAdrs Parameter Error.\n");
		goto _DONE;
	}

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Param pSize pSize Error.\n");
		goto _DONE;
	}

	// Check pDmaWait Parameter
	if (pDmaWait == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Param pDmaWait pSize Error.\n");
		goto _DONE;
	}

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Param pMode pSize Error.\n");
		goto _DONE;
	}

	// FFC Correction Mode取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
			
	// Get FFC Control
	if ((corMode == FFC_CORRECTION_MODE_FIRST) || (corMode == FFC_CORRECTION_MODE_SHADING_LINE))
		*pMode = IN32 (FPGA_FFC_CTRL_ADRS) & (FPGA_FFC_CTRL_GAIN_ENABLE_BIT | FPGA_FFC_CTRL_OFFSET_ENABLE_BIT);
	else
		*pMode = IN32 (FPGA_FFC_CTRL_ADRS) & (FPGA_FFC_CTRL_GAIN_SECOND_ENABLE_BIT | FPGA_FFC_CTRL_GAIN_ENABLE_BIT | FPGA_FFC_CTRL_OFFSET_ENABLE_BIT);

	// Get FFC Address
	*pMemAdrs = IN32 (FPGA_FFC_DMA_ADRS);

	// Get FFC Size
	*pSize = IN32 (FPGA_FFC_DMA_SIZE_ADRS);

	// Get FFC DMA Wait Threshold
	*pDmaWait = IN32 (FPGA_FFC_DMA_WAIT_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCメモリアドレスを取得(FFC番号から)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		memType				：0=OS管理内/1=OS管理外
//		pAdrs				：FFCメモリアドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetMemAdrs (int ffcNum, int memType, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Memory Adrs ffcNum(%d) Parameter Error.\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check memType Parameter
	if ((memType != FFC_MEMORY_EXT) && (memType != FFC_MEMORY_INT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Memory Adrs memType(%d) Parameter Error.(EXT:%d / INT:%d)\n", memType, FFC_MEMORY_EXT, FFC_MEMORY_INT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Memory Adrs pMemAdrs Parameter Error.\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリアドレス取得
	if (memType == FFC_MEMORY_EXT)
		*pAdrs = FFC_MEMORY_ADRS;							// OS管理外メモリ
	else
		*pAdrs = (unsigned int)pImageFfcSave [ffcNum];		// OS管理内メモリ

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Flashアドレスを取得(FFC番号から)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		pAdrs				：FFC Flashアドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetFlashAdrs (int ffcNum, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Flash ffcNum(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Flash pMemAdrs Parameter Error.\n");
		goto _DONE;
	}

	// Get Memory Address
	*pAdrs = FLASH_FFC0_ADRS + FFC_DATA_INTERVAL_SIZE * ffcNum;

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Flashアドレスを取得(FFC番号から)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号(1から始まり)
//		pAdrs				：FFC Flashアドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetFlashAdrsAdmin (int ffcNum, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_ADMIN_MIN) || (ffcNum > FFC_NUMBER_ADMIN_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Flash Admin ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_ADMIN_MIN, FFC_NUMBER_ADMIN_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Flash Admin pMemAdrs Parameter Error.\n");
		goto _DONE;
	}

	// Get Memory Address
	*pAdrs = FLASH_FFC0_ADRS + FFC_DATA_INTERVAL_SIZE * ffcNum;

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCメモリアドレスからFFC番号を取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		memAdrs				：メモリアドレス
//		pAdrs				：FFC番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetNumFromMem (unsigned int memAdrs, int *pFfc)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int Checkdrs;

	// Check pFfc Parameter
	if (pFfc == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Num From Memory pFfc NULL Parameter Error.\n");
		return (status);
	}

	// Get FFC Number
	Checkdrs = FFC_MEMORY_ADRS;
	for (i=0; i<FFC_NUM_USER; i++)
	{
		// Adrs Check
		if (memAdrs == Checkdrs)
		{
			*pFfc = i;
			return (AVAL_STATUS_SUCCESS);
		}

		// Flashアドレス更新
		Checkdrs += FFC_DATA_INTERVAL_SIZE;
	}

	status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
	sprintf (gLogMsgBuff, "FFC Num From Memory Address(0x%x) Parameter Error.\n", memAdrs);
	cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	return (status);
}


//**********************************************************************************
//	FFC FlashアドレスからFFC番号を取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		pAdrs				：FFC番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetNumFromFlash (unsigned int flashAdrs, int *pFfc)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int CheckFlashAdrs;

	// Check pFfc Parameter
	if (pFfc == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Num From Flash pFfc NULL Parameter Error.\n");
		return (status);
	}

	// Get FFC Number
	CheckFlashAdrs = FLASH_FFC0_ADRS;
	for (i=0; i<FFC_NUM_FACTORY; i++)
	{
		// Flash Adrs Check
		if (flashAdrs == CheckFlashAdrs)
		{
			*pFfc = i;
			return (AVAL_STATUS_SUCCESS);
		}

		// Flashアドレス更新
		CheckFlashAdrs += FFC_DATA_INTERVAL_SIZE;
	}

	status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
	sprintf (gLogMsgBuff, "FFC Num From Flash Address(0x%x) Parameter Error.\n", flashAdrs);
	cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	return (status);
}


//**********************************************************************************
//	FFC FlashアドレスからFFC番号を取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		pAdrs				：FFC番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetNumFromFlashAdmin (unsigned int flashAdrs, int *pFfc)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int CheckFlashAdrs;
	int ffcCount;

	// Check pFfc Parameter
	if (pFfc == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Num From Flash pFfc NULL Parameter Error.\n");
		return (status);
	}

	// Get FFC Number
	CheckFlashAdrs = FLASH_FFC0_ADRS;
	ffcCount = FFC_NUM_FACTORY;

	for (i=0; i<ffcCount; i++)
	{
		// Flash Adrs Check
		if (flashAdrs == CheckFlashAdrs)
		{
			*pFfc = i;
			return (AVAL_STATUS_SUCCESS);
		}

		// Flashアドレス更新
		CheckFlashAdrs += FFC_DATA_INTERVAL_SIZE;
	}

	status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
	sprintf (gLogMsgBuff, "FFC Num From Flash Address(0x%x) Parameter Error.\n", flashAdrs);
	cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	return (status);
}


//**********************************************************************************
// Offset Data設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		x					：Height
//		y					：Width
//		data				：データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetOffsetData (int ffcNum, int x, int y, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int index, adrs, readData;
	int flipX;
	int widthMax, heightMax;
#if defined (MODE_FFC_BIT_CALC)
	int calc;
#endif
#if defined (MODE_IPU_MULTI)
	int ipu;
	int widthHalf;
	int xLeft = -1;
	int xRight = -1;
#endif

	// Width Max
	if ((status = roiGetAreaWidthMax (&widthMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	if ((status = roiGetAreaHeightMax (&heightMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

 	// 座標変換
	if ((status = roiCoordinateTrans (x, y, &x, &y,TRANS_MODE_ADD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check x Parameter
	if ((x < 0) || (x >= widthMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data Width(%d) Parameter Error.(Min:0 / Max:%d)\n", x, widthMax-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check y Parameter
	if ((y < 0) || (y >= heightMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data Height(%d) Parameter Error.(Min:0 / Max:%d)\n", y, heightMax-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if defined (MODE_FFC_BIT_CALC)
	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif


#if defined (MODE_FFC_BIT_CALC)
	// Check data Parameter
	if (data > OFFSET_MASK/calc)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data (%d) Parameter Error.(Min:0 / Max:%d)\n", data, OFFSET_MASK/calc);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
#else
	// Check data Parameter
	if (data > OFFSET_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data (%d) Parameter Error.(Min:0 / Max:%d)\n", data, OFFSET_MASK);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
#endif

	// FFCアドレス取得
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, (unsigned int *)&adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ROIアドレスCheck
	if ((status = roiGetAreaGridOffset (&x, &y)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標変換
	if ((status = aoiGetFlipX (x, &flipX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標設定
	x = flipX;

#if defined (MODE_IPU_MULTI)
	//--------------------------------------------------------------------------------
	// 右左アドレスの算出
	//--------------------------------------------------------------------------------
	ipu = x / IMG_WIDTH_IPU_SIZE;
	adrs += (ipu  * FFC_MEMORY_IPU_MULTI_INTERVAL);

	//--------------------------------------------------------------------------------
	// IPU=0
	// ABA-052VIR：左側の1280～1295の領域を書き換えたら、右側の0～15(1296～1311)の領域も書き換える
	// 
	// IPU=1
	// ABA-052VIR：右側の0～15(1328～1343)の領域を書き換えたら、左側の1296～1311の領域も書き換える
	//--------------------------------------------------------------------------------

	// 隠し座標の確認
	if ((status = hideGridCalc (x, y, &xLeft, &xRight)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// xアドレスの算出
	//--------------------------------------------------------------------------------
	x = x % IMG_WIDTH_IPU_SIZE;

	if (ipu != 0)
		x += IMG_WIDTH_OFFSET;
#endif // #if defined (MODE_IPU_MULTI)

	// アドレス算出
	index = x * OFFSET_GAIN_UNIT_SIZE + y * FFC_WIDTH_DATA_ALIGH;

	// データ取得
	readData = IN32 ((adrs + index));

	// ゲイン値保持
	readData = readData & (unsigned int)~(OFFSET_MASK<<OFFSET_SHIFT);

#if defined (MODE_FFC_BIT_CALC)
	data *= calc;
#endif

	// データ設定
	OUT32 ((adrs + index), (readData | ((data & OFFSET_MASK)<<OFFSET_SHIFT)));


#if defined (MODE_IPU_MULTI)
	//--------------------------------------------------------------------------------
	// IPU=0
	// ABA-052VIR：左側の1280～1295の領域を書き換えたら、右側の0～15(1296～1311)の領域も書き換える
	// 
	// IPU=1
	// ABA-052VIR：右側の0～15(1328～1343)の領域を書き換えたら、左側の1296～1311の領域も書き換える
	//--------------------------------------------------------------------------------
	// FFCアドレス取得
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, (unsigned int *)&adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// 左が更新されたので、右も更新(先頭(0)のほう)
	//--------------------------------------------------------------------------------
	if (xRight != -1)
	{
		// アドレス算出
		adrs +=  FFC_MEMORY_IPU_MULTI_INTERVAL;
		index = xRight * 4 + y * FFC_WIDTH_DATA_ALIGH;

		// データ設定
		OUT32 ((adrs + index), (readData | ((data & OFFSET_MASK)<<OFFSET_SHIFT)));
	}

	//--------------------------------------------------------------------------------
	// 右が更新されたので、左も更新(終端(widthHalf)のほう)
	//--------------------------------------------------------------------------------
	if (xLeft != -1)
	{
		// アドレス算出
		widthHalf = IMG_WIDTH / IPU_COUNT;
		index = widthHalf * 4 + xLeft * 4 + y * FFC_WIDTH_DATA_ALIGH;

		// データ設定
		OUT32 ((adrs + index), (readData | ((data & OFFSET_MASK)<<OFFSET_SHIFT)));
	}
#endif // #if defined (MODE_IPU_MULTI)

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
// Offset Data取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum					：FFC番号
//		x						：Height
//		y						：Width
//		pData					：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetOffsetData (int ffcNum, int x, int y, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;;
	unsigned int index, adrs;
	int flipX;
	int widthMax, heightMax;
#if defined (MODE_FFC_BIT_CALC)
	int calc;
#endif
#if defined (MODE_IPU_MULTI)
	int ipu;
#endif

	// Width Max
	if ((status = roiGetAreaWidthMax (&widthMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	if ((status = roiGetAreaHeightMax (&heightMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

 	// 座標変換
	if((status = roiCoordinateTrans(x, y, &x, &y,TRANS_MODE_ADD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check x Parameter
	if ((x < 0) || (x >= widthMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data Width(%d) Parameter Error.(Min:0 / Max:%d)\n", x, widthMax-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check y Parameter
	if ((y < 0) || (y >= heightMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data Height(%d) Parameter Error.(Min:0 / Max:%d)\n", y, HeightMax-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Offset Data pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// FFCアドレス取得
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, (unsigned int *)&adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ROIアドレスCheck
	if ((status = roiGetAreaGridOffset (&x, &y)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標変換
	if ((status = aoiGetFlipX (x, &flipX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標設定
	x = flipX;

#if defined (MODE_IPU_MULTI)
	ipu = x / IMG_WIDTH_IPU_SIZE;
	adrs += (ipu  * FFC_MEMORY_IPU_MULTI_INTERVAL);

	x = x % IMG_WIDTH_IPU_SIZE;

	if (ipu != 0)
		x += IMG_WIDTH_OFFSET;
#endif

	// アドレス算出
#if defined (MODE_FFC_DATA_ALIGN_ADJUST)
	index = x * OFFSET_GAIN_UNIT_SIZE + y * FFC_WIDTH_DATA_ALIGH;
#else
	index = x + y * WidthMax();
	index *= OFFSET_GAIN_UNIT_SIZE;
#endif

	*pData = (IN32 ((adrs + index)) >> OFFSET_SHIFT) & OFFSET_MASK;

#if defined (MODE_FFC_BIT_CALC)
	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	*pData /= calc;
#endif

_DONE:
	return (status);
}


//**********************************************************************************
// Gain Data設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		x					：Height
//		y					：Width
//		data				：データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetGainData (int ffcNum, int x, int y, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int index, adrs, readData;
	int flipX;
	int widthMax, heightMax;
#if defined (MODE_IPU_MULTI)
	int ipu;
	int widthHalf;
	int xLeft = -1;
	int xRight = -1;
#endif

	// Width Max
	if ((status = roiGetAreaWidthMax (&widthMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	if ((status = roiGetAreaHeightMax (&heightMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

 	// 座標変換
	if((status = roiCoordinateTrans(x, y, &x, &y,TRANS_MODE_ADD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain Data ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check x Parameter
	if ((x < 0) || (x >= widthMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain Data Width(%d) Parameter Error.(Min:0 / Max:%d)\n", x, widthMax-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check y Parameter
	if ((y < 0) || (y >= heightMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain Data Height(%d) Parameter Error.(Min:0 / Max:%d)\n", y, heightMax-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check data Parameter
	if (data > GAIN_MASK)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain Data(%d) Parameter Error.(Min:0 / Max:%d)\n", data, GAIN_MASK);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// FFCアドレス取得
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, (unsigned int *)&adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ROIアドレスCheck
#if defined (MODE_ROI_VERSION2)
	if ((status = roiGetAreaGridOffset (&x, &y)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// X座標変換
	if ((status = aoiGetFlipX (x, &flipX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標設定
	x = flipX;

#if defined (MODE_IPU_MULTI)
	//--------------------------------------------------------------------------------
	// 右左アドレスの算出
	//--------------------------------------------------------------------------------
	ipu = x / IMG_WIDTH_IPU_SIZE;
	adrs += (ipu  * FFC_MEMORY_IPU_MULTI_INTERVAL);

	//--------------------------------------------------------------------------------
	// IPU=0
	// ABA-052VIR：左側の1280～1295の領域を書き換えたら、右側の0～15(1296～1311)の領域も書き換える
	// 
	// IPU=1
	// ABA-052VIR：右側の0～15(1328～1343)の領域を書き換えたら、左側の1296～1311の領域も書き換える
	//--------------------------------------------------------------------------------

	// 隠し座標の確認
	if ((status = hideGridCalc (x, y, &xLeft, &xRight)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// xアドレスの算出
	//--------------------------------------------------------------------------------
	x = x % IMG_WIDTH_IPU_SIZE;

	if (ipu != 0)
		x += IMG_WIDTH_OFFSET;

#endif // #if defined (MODE_IPU_MULTI)

	// アドレス算出
	index = x * OFFSET_GAIN_UNIT_SIZE + y * FFC_WIDTH_DATA_ALIGH;

	// データ取得
	readData = IN32 ((adrs + index));

	// オフセット値保持
	readData = readData & (unsigned int)~GAIN_MASK;

	// データ設定
	OUT32 ((adrs + index), (readData | (data & GAIN_MASK)));

#if defined (MODE_IPU_MULTI)
	//--------------------------------------------------------------------------------
	// IPU=0
	// ABA-052VIR：左側の1280～1295の領域を書き換えたら、右側の0～15(1296～1311)の領域も書き換える
	// 
	// IPU=1
	// ABA-052VIR：右側の0～15(1328～1343)の領域を書き換えたら、左側の1296～1311の領域も書き換える
	//--------------------------------------------------------------------------------
	// FFCアドレス取得
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, (unsigned int *)&adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// 左が更新されたので、右も更新(先頭(0)のほう)
	//--------------------------------------------------------------------------------
	if (xRight != -1)
	{
		// アドレス算出
		adrs +=  FFC_MEMORY_IPU_MULTI_INTERVAL;
		index = xRight * 4 + y *FFC_WIDTH_DATA_ALIGH;

		// データ設定
		OUT32 ((adrs + index), (readData | (data & GAIN_MASK)));
	}

	//--------------------------------------------------------------------------------
	// 右が更新されたので、左も更新(終端(widthHalf)のほう)
	//--------------------------------------------------------------------------------
	if (xLeft != -1)
	{
		// アドレス算出
		widthHalf = IMG_WIDTH / IPU_COUNT;
		index = widthHalf * 4 + xLeft * 4 + y * FFC_WIDTH_DATA_ALIGH;

		// データ設定
		OUT32 ((adrs + index), (readData | (data & GAIN_MASK)));
	}
#endif // #if defined (MODE_IPU_MULTI)

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
// Gain Data取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		x					：Height
//		y					：Width
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetGainData (int ffcNum, int x, int y, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int index, adrs;
	int flipX;
	int widthMax, heightMax;
#if defined (MODE_IPU_MULTI)
	int ipu;
#endif

	// Width Max
	if ((status = roiGetAreaWidthMax (&widthMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	if ((status = roiGetAreaHeightMax (&heightMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

 	// 座標変換
	if((status = roiCoordinateTrans(x, y, &x, &y,TRANS_MODE_ADD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain Data ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check x Parameter
	if ((x < 0) || (x >= widthMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain Data Width(%d) Parameter Error.(Min:0 / Max:%d)\n", x, widthMax-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check y Parameter
	if ((y < 0) || (y >= heightMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain Data Height(%d) Parameter Error.(Min:0 / Max:%d)\n", y, heightMax-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Gain Data pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// FFCアドレス取得
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, (unsigned int *)&adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ROIアドレスCheck
	if ((status = roiGetAreaGridOffset (&x, &y)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標変換
	if ((status = aoiGetFlipX (x, &flipX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標設定
	x = flipX;

#if defined (MODE_IPU_MULTI)
	ipu = x / IMG_WIDTH_IPU_SIZE;
	adrs += (ipu  * FFC_MEMORY_IPU_MULTI_INTERVAL);

	x = x % IMG_WIDTH_IPU_SIZE;

	if (ipu != 0)
		x += IMG_WIDTH_OFFSET;
#endif

	// アドレス算出
	index = x * OFFSET_GAIN_UNIT_SIZE + y * FFC_WIDTH_DATA_ALIGH;

	*pData = IN32 ((adrs + index)) & GAIN_MASK;
	
_DONE:
	return (status);
}


//**********************************************************************************
// Gain Data設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		x					：Height
//		y					：Width
//		data				：データ(倍率)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetGainX (int ffcNum, int x, int y, double data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int tempGain;
	double temp;

	// 小数点切捨て(第5位以下)
	tempGain = (int)(data * FFC_GAIN_X_UNIT);
	temp = (double)((double)tempGain / (double)FFC_GAIN_X_UNIT) + FFC_GAIN_X_ROUNDING;
	
	// 倍率=>レジスタ値に変換
	tempGain = temp * BIT_16_MAX;
	
	status = ffcSetGainData (ffcNum, x, y, tempGain);

	return (status);
}


//**********************************************************************************
// Gain Data取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		x					：Height
//		y					：Width
//		pData				：データ(倍率)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetGainX (int ffcNum, int x, int y, double *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int tempGain;

	if ((status = ffcGetGainData (ffcNum, x, y, &tempGain)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// レジスタ値=>倍率に変換
	*pData = (double)((double)tempGain / (double)BIT_16_MAX);

_DONE:
	return (status);
}


//**********************************************************************************
// Offset Data取得(オフセットデータの平均／最大／最小)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		dpcNum				：FFC番号
//		pAve				：平均値を格納するポインタ
//		pMin				：最小値格納するポインタ
//		pMax				：最大値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetOffsetAve (int ffcNum, int dpcNum, double *pAve, FFC_OG_INFO *pMin, FFC_OG_INFO *pMax)
{
	int status = AVAL_STATUS_SUCCESS;
	int wMax, hMax;
	int x, y;
	unsigned int tempOffset;
	unsigned int offsetAve, offsetMin, offsetMax;
	int saveMinX, saveMinY;
	int saveMaxX, saveMaxY;
	GRID_XY *pGrid = NULL;
	GRID_XY *pGridTemp;
	int gridNum;
	int hitCount;
	int flipX;
	int flipModeSave = -1;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Average Data ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAve Parameter
	if (pAve == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Offset Average Data pAve NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pMin Parameter
	if (pMin == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Offset Average Data pMin NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pMax Parameter
	if (pMax == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Offset Average Data pMax NULL Parameter Error.\n");
		goto _DONE;
	}

	// Memory Request
	if ((pGrid = (GRID_XY *)malloc (NUM_DEFECTION_PIX * sizeof(GRID_XY))) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		DEBUG_PRINT_FORCE ("FFC Offset Average Data Buffer Request Error. size = 0x%x.\n", (NUM_DEFECTION_PIX * sizeof(GRID_XY)));
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if defined (MODE_SENSOR_X_REVERSE)
	// Get Flip
	if ((status = aoiGetXflip (&flipModeSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set Flip
	aoiSetXflip (XFLIP_ENABLE);
#endif

	// 画像補正用情報取得
	if ((status = dpcGetGridMem (dpcNum, pGrid, &gridNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Width Max
	if ((status = roiGetAreaWidthMax (&wMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	if ((status = roiGetAreaHeightMax (&hMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 初期化
	offsetAve = 0;
	offsetMin = 0xffffffff;
	offsetMax = 0;

	saveMinX = -1;
	saveMinY = -1;
	saveMaxX = -1;
	saveMaxY = -1;

	hitCount = 0;
	pGridTemp = pGrid;

	for (y= 0; y <hMax; y++)
	{
		for (x= 0; x <wMax; x++)
		{
			// 欠陥画素?
			if (hitCount < gridNum)
			{
				// 欠陥画素ならば次の画素へ
				if ((x == pGridTemp->x) && (y == pGridTemp->y))
				{
					pGridTemp++;
					hitCount++;
					continue;
				}
			}

			// オフセットデータ取得
			if ((status = ffcGetOffsetData (ffcNum, x, y, &tempOffset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// 平均値
			offsetAve += tempOffset;

			// 最小値Check
			if (tempOffset < offsetMin)
			{
				offsetMin = tempOffset;
				saveMinX = x;
				saveMinY = y;
			}

			// 最大値Check
			if (tempOffset > offsetMax)
			{
				offsetMax = tempOffset;
				saveMaxX = x;
				saveMaxY = y;
			}
		}
	}

	// Restore Flip
	if (flipModeSave != -1)
	{
		aoiSetXflip (flipModeSave);
		flipModeSave = -1;
	}

	// データ格納
	*pAve = (double)((double)offsetAve / (double)((wMax * hMax) - hitCount));

	// X座標変換
	if ((status = aoiGetFlipX (saveMinX, &flipX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標設定
	saveMinX = flipX;
	
	// 最小データ格納
	pMin->data = offsetMin;
	pMin->x = saveMinX;
	pMin->y = saveMinY;

	// X座標変換
	if ((status = aoiGetFlipX (saveMaxX, &flipX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標設定
	saveMaxX = flipX;

	// 最大データ格納
	pMax->data = offsetMax;
	pMax->x = saveMaxX;
	pMax->y = saveMaxY;

_DONE:
	// Restore Flip
	if (flipModeSave != -1)
		aoiSetXflip (flipModeSave);

	if (pGrid != NULL)
		free (pGrid);

	return (status);
}


//**********************************************************************************
// Gain Data取得(ゲインデータの平均/Min/Max)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		dpcNum				：FFC番号
//		pAve				：平均値を格納するポインタ
//		pMin				：最小値格納するポインタ
//		pMax				：最大値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetGainAve (int ffcNum, int dpcNum, double *pAve, FFC_OG_INFO *pMin, FFC_OG_INFO *pMax)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int tempGain;
	int wMax, hMax;
	int x, y;
	double gainTemp;
	double gainAve, gainMin, gainMax;
	int saveMinX, saveMinY;
	int saveMaxX, saveMaxY;
	GRID_XY *pGrid = NULL;
	GRID_XY *pGridTemp;
	int gridNum;
	int hitCount;
	int flipX;
	int flipModeSave = -1;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain Average Data ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAve Parameter
	if (pAve == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Gain Average Data pAve NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pMin Parameter
	if (pMin == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Gain Average Data pMin NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pMax Parameter
	if (pMax == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Gain Average Data pMax NULL Parameter Error.\n");
		goto _DONE;
	}

	// Memory Request
	if ((pGrid = (GRID_XY *)malloc (NUM_DEFECTION_PIX * sizeof(GRID_XY))) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "FFC Offset Average Data Buffer Request Error. size = 0x%x.\n", (NUM_DEFECTION_PIX * sizeof(GRID_XY)));
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 画像補正用情報取得
	if ((status = dpcGetGridMem (dpcNum, pGrid, &gridNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Width Max
	if ((status = roiGetAreaWidthMax (&wMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	if ((status = roiGetAreaHeightMax (&hMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 初期化
	gainAve = 0;
	gainMin = 4;
	gainMax = 0;

	saveMinX = -1;
	saveMinY = -1;
	saveMaxX = -1;
	saveMaxY = -1;

	hitCount = 0;
	pGridTemp = pGrid;

	for (y= 0; y <hMax; y++)
	{
		for (x= 0; x <wMax; x++)
		{
			// 欠陥画素?
			if (hitCount < gridNum)
			{
				// 欠陥画素ならば次の画素へ
				if ((x == pGridTemp->x) && (y == pGridTemp->y))
				{
					pGridTemp++;
					hitCount++;
					continue;
				}
			}

			// ゲインデータ取得
			if ((status = ffcGetGainData (ffcNum, x, y, &tempGain)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// レジスタ値=>倍率に変換
			gainTemp = (double)((double)tempGain / (double)BIT_16_MAX);

			// 平均値
			gainAve += gainTemp;

			// 最小値Check
			if (gainTemp < gainMin)
			{
				gainMin = gainTemp;
				saveMinX = x;
				saveMinY = y;
			}

			// 最大値Check
			if (gainTemp > gainMax)
			{
				gainMax = gainTemp;
				saveMaxX = x;
				saveMaxY = y;
			}
		}
	}

	// Restore Flip
	if (flipModeSave != -1)
	{
		aoiSetXflip (flipModeSave);
		flipModeSave = -1;
	}

	// 平均データ格納
	*pAve = (double)gainAve / (double)((wMax * hMax) - hitCount);

	// X座標変換
	if ((status = aoiGetFlipX (saveMinX, &flipX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標設定
	saveMinX = flipX;

	// 最小データ格納
	pMin->data = gainMin;
	pMin->x = saveMinX;
	pMin->y = saveMinY;

	// X座標変換
	if ((status = aoiGetFlipX (saveMaxX, &flipX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標設定
	saveMaxX = flipX;

	// 最大データ格納
	pMax->data = gainMax;
	pMax->x = saveMaxX;
	pMax->y = saveMaxY;

_DONE:
	// Restore Flip
	if (flipModeSave != -1)
		aoiSetXflip (flipModeSave);

	if (pGrid != NULL)
		free (pGrid);

	return (status);
}


//**********************************************************************************
// Offset Data取得(オフセットデータのCheck)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		dpcNum				：DPC番号
//		min					：最小値
//		max					：最大値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetOffsetCheck (int ffcNum, int dpcNum, int min, int max)
{
	int status = AVAL_STATUS_SUCCESS;
	int wMax, hMax;
	int x, y;
	unsigned int tempOffset;
	GRID_XY *pGrid = NULL;
	GRID_XY *pGridTemp;
	int gridNum;
	int i;
	int hit;
	int flag = 0;
	char c;
	int flipX;
	int flipModeSave = -1;

	// Check ffcNum Parameter
	if (((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX)) && (ffcNum != -1))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data Check ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data Check dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check min Parameter
	if ((min < FFC_OFFSET_CHECK_DATA_MIN) || (min > FFC_OFFSET_CHECK_DATA_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data Check min(%d) Parameter Error.(Min:%d / Max:%d)\n", min, FFC_OFFSET_CHECK_DATA_MIN, FFC_OFFSET_CHECK_DATA_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check max Parameter
	if ((max < FFC_OFFSET_CHECK_DATA_MIN) || (max > FFC_OFFSET_CHECK_DATA_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data Check max(%d) Parameter Error.(Min:%d / Max:%d)\n", min, FFC_OFFSET_CHECK_DATA_MIN, FFC_OFFSET_CHECK_DATA_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check min/max Parameter
	if (min >= max)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data Check min(%d) > max(%d) Parameter Error.\n", min, max);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Memory Request
	if ((pGrid = (GRID_XY *)malloc (NUM_DEFECTION_PIX * sizeof(GRID_XY))) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Offset Data Check Buffer Request Error. size = 0x%x.\n", (NUM_DEFECTION_PIX * sizeof(GRID_XY)));
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 画像補正用情報取得
	if ((status = dpcGetGridMem (dpcNum, pGrid, &gridNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Width Max
	if ((status = roiGetAreaWidthMax (&wMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	if ((status = roiGetAreaHeightMax (&hMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	for (y= 0; y <hMax; y++)
	{
		for (x= 0; x <wMax; x++)
		{
			if (_kbhit(&c))
			{
				if (c == 'q')
				{
					flag = 1;
					goto _DONE;	// End
				}
			}

			// Clear
			hit = 0;

			// オフセットデータ取得
			if ((status = ffcGetOffsetData (ffcNum, x, y, &tempOffset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// オフセットCheck
			if ((tempOffset < min) || (tempOffset > max))
			{
				// 欠陥座標と一致する?
				pGridTemp = pGrid;
				for (i= 0; i<gridNum; i++, pGridTemp++)
				{
					if ((x == pGridTemp->x) && (y == pGridTemp->y))
					{
						hit = 1;
						break;
					}
				}

				if (hit == 0)
				{
					// X座標変換
					if ((status = aoiGetFlipX2 (x, &flipX, flipModeSave)) != AVAL_STATUS_SUCCESS)
						goto _DONE;

					DEBUG_PRINT_FORCE ("x:%d, y:%d, %d\n", flipX, y, tempOffset);
					flag = 1;
				}
			}
		}
	}

_DONE:
	// Restore Flip
	if (flipModeSave != -1)
		aoiSetXflip (flipModeSave);

	if (pGrid != NULL)
		free (pGrid);

	if (flag != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_ADJUST);
	}

	return (status);
}


//**********************************************************************************
//	FFC Data Upload
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：updateするFFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcDataUpload (int ffcNum)
{
	int status;
	int size = FFC_DATA_SIZE;
	unsigned int memAdrs;
	char *pAsciiBuff = NULL;
	char *pAsciiNewBuff = NULL;
	char *pBinBuff = NULL;
	int updateSize;
#ifdef COMPRESS_MODE
	unsigned char *pUnCompBuf = NULL;
	int uncomprLen;
#endif // COMPRESS_MODE
#if defined (MODE_FFC_DATA_ALIGN_ADJUST) && !defined (MODE_IPU_MULTI)
	int x, y;
	unsigned int memAdrs2;
	unsigned int *ptrL;
	unsigned int data;
#endif

	// Check ffcNo Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Upload ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Memory Adrs
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Upload Buffer Request Error\n");
		goto _DONE;
	}

#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	// メモリリクエスト(ASCIIデータ格納用)
	if ((pAsciiBuff = malloc (FFC_UPLOAD_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Upload Buffer Request Error\n");
		goto _DONE;
	}

	// メモリリクエスト(Binデータ格納用)
	if ((pBinBuff = malloc (FFC_DATA_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Upload Upload Buffer Request Error\n");
		goto _DONE;
	}

#else // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	pAsciiBuff = (char *)FFC0_TEMP_MEMORY_ADRS;
	pBinBuff = (char *)FFC_GAIN_MEMORY_ADRS;

#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	// Kermit Recive
	if ((size = kermitRecv ((char *)pAsciiBuff, FFC_UPLOAD_SIZE)) < 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_IO);
		sprintf (gLogMsgBuff, "FFC Data Upload kermit Error. Size = %d\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// バッファ&サイズ更新
	pAsciiNewBuff = (char *)pAsciiBuff;
	updateSize = size;

#ifdef COMPRESS_MODE
	if ((status = kermitGzipDataCheck (&pUnCompBuf, &uncomprLen, (unsigned char *)pAsciiBuff, updateSize, FFC_UPLOAD_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// バッファ&サイズ更新
	if (pUnCompBuf != NULL)
	{
		pAsciiNewBuff = (char *)pUnCompBuf;
		updateSize = uncomprLen;
	}
#endif // COMPRESS_MODE

	// ASCII=>Binary変換
	if ((status = lfFfcUploadAsciiToBin ((void *)pAsciiNewBuff, (void *)pBinBuff, updateSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFCテーブルにCopy
#if !defined (MODE_IPU_MULTI)

	ptrL = (unsigned int *)pBinBuff;
	memAdrs2 = memAdrs;

	for (y=0; y<IMG_HEIGHT; y++)
	{
		for (x=0; x<IMG_WIDTH; x++, ptrL++)
		{
			data = *ptrL;
			OUT32 ((memAdrs2 + x * 4), data);
		}

		memAdrs2 += FFC_WIDTH_DATA_ALIGH;
	}

#else // #if !defined (MODE_IPU_MULTI)

	ffcCopyBuffToExtMem (memAdrs, (unsigned int *)pBinBuff);

	if ((status = ffcSetMarginGridData ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;


#endif	// #if !defined (MODE_IPU_MULTI)

	// キャッシュFlash
	cacheFlush ();

_DONE:

#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	if (pAsciiBuff != NULL)
		free (pAsciiBuff);

	if (pBinBuff != NULL)
		free (pBinBuff);

#ifdef COMPRESS_MODE
	#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)
	if (pUnCompBuf != NULL)
		free (pUnCompBuf);
	#endif
#endif // COMPRESS_MODE

#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	return (status);
}


//**********************************************************************************
//	FFC Upload Data ASCII=>Binary変換
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pAscii				：ASCIIデータを格納するポインタ
//		pBinary				：Binaryデータを格納するポインタ
//		size				：ASCIIデータサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lfFfcUploadAsciiToBin (void *pAscii, void *pBinary, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pBuffer;
	unsigned int *ptr32;
	unsigned int i, index;
	unsigned char tempBuff[FFC_TEMP_SIZE];
	unsigned int data, count;
	int offsetGainFlag;
	int flipMode;
	int width, height = 0;
	unsigned int tempAdrs;
	int ffcSize;

	// Check pAscii Parameter
	if (pAscii == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Upload pAscii NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pBinary Parameter
	if (pBinary == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Upload pBinary NULL Parameter Error\n");
		goto _DONE;
	}

	// ASCII=>Bin変換
	index = 0;
	count = 0;
	pBuffer = (unsigned char *)pAscii;
	ptr32 =(unsigned int *)pBinary;

	// Get Flip
#if !defined (MODE_SENSOR_XFLIP)
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	flipMode = XFLIP_DISABLE;
#endif

	// Get Width
	width = IMG_WIDTH;

	// FFC Number
	ffcSize = FFC_DATA_NUM_ALIGN;

	//-----------------------------------
	// offsetGainFlag&0x01 = 0 => オフセット
	// offsetGainFlag&0x01 = 1 => ゲイン
	//-----------------------------------
	offsetGainFlag = 0;

	for (i=0; i<size; i++)
	{
		// データ取得完了?
		if (count >= ffcSize)
			break;

		// カンマ検索
		if (pBuffer[i] == ',')
		{
			if ((offsetGainFlag%2) == 0)
			{
				if ((count%width) == 0)
				{
					height++;

					// Flip有効?
#if !defined (MODE_SENSOR_X_REVERSE)
					if (flipMode == XFLIP_ENABLE)
					{
						tempAdrs = (unsigned int)((unsigned int)pBinary + ((height * width) - 1) * 4);
						ptr32 = (unsigned int *)tempAdrs;
					}
#else
					if (flipMode == XFLIP_DISABLE)
					{
						tempAdrs = (unsigned int)((unsigned int)pBinary + ((height * width) - 1) * 4);
						ptr32 = (unsigned int *)tempAdrs;
					}
#endif
				}
			}

			// NULL設定
			tempBuff[index] = '\0';

			// 一時バッファの内容を数値に変換
			if (sscanf ((char *)tempBuff, "%d", &data) != 1)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "FFC Upload Data Error. Index = %d / Data = %s\n", count, tempBuff);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// データCheck
			if ((offsetGainFlag%2) == 0 )
			{
				// オフセットデータ範囲Check
				if (data > OFFSET_MASK)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
					sprintf (gLogMsgBuff, "FFC Offset Data Range Error. Index = %d / Data = %d.(Min:0 / Max:%d)\n", count, data, OFFSET_MASK);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}

				// 変換したデータを別のバッファにコピー
				*ptr32 = (unsigned int)((data & OFFSET_MASK)<<OFFSET_SHIFT);
			}
			else
			{
				// ゲインデータ範囲Check
				if (data > GAIN_MASK)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
					sprintf (gLogMsgBuff, "FFC Gain Data Range Error. Index = %d / Data = %d.(Min:0 / Max:%d)\n", count, data, GAIN_MASK);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					goto _DONE;
				}

				// 変換したデータを別のバッファにコピー
				*ptr32 |= (unsigned int)(data & GAIN_MASK);

				// Flip有効?
#if !defined (MODE_SENSOR_X_REVERSE)
				if (flipMode == XFLIP_ENABLE)
					ptr32--;
				else
					ptr32++;
#else
				if (flipMode == XFLIP_DISABLE)
					ptr32--;
				else
					ptr32++;
#endif

				// カウントインクリメント
				count++;
			}

			// オフセット/ゲイン確認Flag
			offsetGainFlag++;

			// 再度検索の為、クリア
			index = 0;
		}
		// 数値
		else if ((pBuffer[i] >= '0') && (pBuffer[i] <= '9'))
		{
			// カンマが見つかるまで一時バッファにコピー
			tempBuff[index] = pBuffer[i];
			index++;
			if (index >= FFC_TEMP_SIZE)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "FFC Upload Temp Buffer Size Over Error. Index = %d\n", count);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}
		// \r / \n / スペースは無視
		else if ((pBuffer[i] == '\n') || (pBuffer[i] == '\r') || (pBuffer[i] == ' '))
		{
			// 何も処理はなし
		}
		// 上記以外はエラー
		else
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Upload Invalid Data Error. Data = 0x%x, Count = %d\n", pBuffer[i], count);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

	// データ数Check
	if (count != ffcSize)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Upload Data Count Error. Count = %d\n", count);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Data Upload
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：updateするFFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcDataUploadBin (int ffcNum)
{
	int status;
	int size = FFC_DATA_SIZE;
	char *pAsciiNewBuff = NULL;
	unsigned int memAdrs;
	char *pAsciiBuff = NULL;
	int updateSize;
#if !defined (MODE_IPU_MULTI)
	unsigned int srcAdrs, data;
	int i;
#endif
#if !defined (MODE_FFC_DATA_ALIGN_ADJUST)
	int copySize;
#endif

#ifdef COMPRESS_MODE
	unsigned char *pUnCompBuf = NULL;
	int uncomprLen;
#endif // COMPRESS_MODE

#if defined (MODE_FFC_DATA_ALIGN_ADJUST) && !defined (MODE_IPU_MULTI)
	int x, y;
	unsigned int memAdrs2;
#endif

	// Check ffcNo Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Upload ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Memory Adrs
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Upload Buffer Request Error\n");
		goto _DONE;
	}

#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	// メモリリクエスト(ASCIIデータ格納用)
	if ((pAsciiBuff = malloc (FFC_UPLOAD_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Upload Buffer Request Error\n");
		goto _DONE;
	}

#else // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	pAsciiBuff = (char *)FFC0_TEMP_MEMORY_ADRS;

#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	// Kermit Recive
	if ((size = kermitRecv ((char *)pAsciiBuff, FFC_UPLOAD_SIZE)) < 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_IO);
		sprintf (gLogMsgBuff, "FFC Data Upload kermit Error. Size = %d\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// バッファ&サイズ更新
	pAsciiNewBuff = (char *)pAsciiBuff;
	updateSize = size;

#ifdef COMPRESS_MODE
	if ((status = kermitGzipDataCheck (&pUnCompBuf, &uncomprLen, (unsigned char *)pAsciiBuff, updateSize, FFC_UPLOAD_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// バッファ&サイズ更新
	if (pUnCompBuf != NULL)
	{
		pAsciiNewBuff = (char *)pUnCompBuf;
		updateSize = uncomprLen;
	}
#endif // COMPRESS_MODE

	// FFCデータSWAP
	if ((status = ffcDataSwap ((unsigned int)pAsciiNewBuff, FFC_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if !defined (MODE_IPU_MULTI)
	// FFCテーブルにCopy
	srcAdrs = (unsigned int) pAsciiNewBuff;
	memAdrs2 = memAdrs;
	i=0;
	for (y=0; y<IMG_HEIGHT; y++)
	{
		for (x=0; x<IMG_WIDTH; x++, i++)
		{
			data = IN32 ((srcAdrs + i * 4));
			OUT32 ((memAdrs2 + x * 4), data);
		}

		memAdrs2 += FFC_WIDTH_DATA_ALIGH;
	}

#else // #if !defined (MODE_IPU_MULTI)

	ffcCopyBuffToExtMem (memAdrs, (unsigned int *)pAsciiNewBuff);

#endif	// #if !defined (MODE_IPU_MULTI)

	// キャッシュFlash
	cacheFlush ();

_DONE:

#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	if (pAsciiBuff != NULL)
		free (pAsciiBuff);

#ifdef COMPRESS_MODE
	#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)
	if (pUnCompBuf != NULL)
		free (pUnCompBuf);
	#endif
#endif // COMPRESS_MODE

#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	return (status);
}


//**********************************************************************************
//	FFC Upload Data ASCII=>Register変換
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pAscii				：ASCIIデータを格納するポインタ
//		pBinary				：Binaryデータを格納するポインタ
//		size				：ASCIIデータサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lfFfcUploadAsciiToReg (void *pAscii, void *pBinary, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pBuffer;
	unsigned int *ptr32;
	unsigned int i, index;
	unsigned char tempBuff[FFC_TEMP_SIZE];
	unsigned int data, count;
	int ffcSize;

	// Check pAscii Parameter
	if (pAscii == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Upload pAscii NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pBinary Parameter
	if (pBinary == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Upload pBinary NULL Parameter Error\n");
		goto _DONE;
	}

	// FFC Number
	ffcSize = FFC_DATA_NUM_ALIGN;

	// ASCII=>Bin変換
	index = 0;
	count = 0;
	pBuffer = (unsigned char *)pAscii;
	ptr32 =(unsigned int *)pBinary;

	for (i=0; i<size; i++)
	{
		// データ取得完了?
		if (count >= ffcSize)
			break;

		// カンマ検索
		if (pBuffer[i] == ',')
		{
			// NULL設定
			tempBuff[index] = '\0';

			// 一時バッファの内容を数値に変換
			if (sscanf ((char *)tempBuff, "%x", &data) != 1)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "FFC Upload Data Error. Index = %d / Data = %s\n", count, tempBuff);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// 変換したデータを別のバッファにコピー
			*ptr32 = (unsigned int)data;
			ptr32++;

			// カウントインクリメント
			count++;

			// 再度検索の為、クリア
			index = 0;
		}
		// 数値
		else if (((pBuffer[i] >= '0') && (pBuffer[i] <= '9')) || ((pBuffer[i] >= 'a') && (pBuffer[i] <= 'f')) || ((pBuffer[i] >= 'A') && (pBuffer[i] <= 'F')))
		{
			// カンマが見つかるまで一時バッファにコピー
			tempBuff[index] = pBuffer[i];
			index++;
			if (index >= FFC_TEMP_SIZE)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "FFC Upload Temp Buffer Size Over Error. Index = %d\n", count);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}
		// \r / \n / スペースは無視
		else if ((pBuffer[i] == '\n') || (pBuffer[i] == '\r') || (pBuffer[i] == ' '))
		{
			// 何も処理はなし
		}
		// 上記以外はエラー
		else
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Upload Invalid Data Error. Data = 0x%x, Count = %d\n", pBuffer[i], count);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

	// データ数Check
	if (count != ffcSize)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Upload Data Count Error. Count = %d\n", count);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Data Download
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：updateするFFC番号
//		mode				：圧縮モード(0=圧縮/1=非圧縮))
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcDataDownload (int ffcNum, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int size = FFC_DOWNLOAD_SIZE;
	unsigned int memAdrs;
	char *pAsciiBuff = NULL;
	unsigned char *pCompressBuff = NULL;
	int downloadSize;
	int kermitSendSize;
	int comprLen;
	unsigned char *ptrSend;
	int sendSize;
	char *fileName[64];

	// Check ffcNo Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Download ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check mode Parameter
	if ((mode != UPDATE_COMPRESS) && (mode != UPDATE_UNCOMPRESS))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Download mode(%d) Parameter Error. (Compress:%d / Uncpmpress:%d)\n", mode, UPDATE_COMPRESS, UPDATE_UNCOMPRESS);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Memory Adrs
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Download Get FFC Mem Adrs Error\n");
		goto _DONE;
	}

#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	// メモリリクエスト(ASCIIデータ格納用)
	if ((pAsciiBuff = malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Download Buffer Request Error\n");
		goto _DONE;
	}

#else // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	pAsciiBuff = (char *)FFC0_TEMP_MEMORY_ADRS;

#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	// Binary => ASCII変換
	if ((status = lfFfcDownloadBintoAscii ((void *)pAsciiBuff, (void *)memAdrs, (unsigned int *)&downloadSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 圧縮あり？
	if (mode == UPDATE_COMPRESS)
	{
#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)
		// メモリリクエスト(圧縮データ格納用)
		if ((pCompressBuff = malloc (size + COMPRESS_DOWNLOAD_ADD_MEMORY)) == NULL)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Download Buffer Request Error\n");
			goto _DONE;
		}

#else // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

		pCompressBuff = (unsigned char *)FFC_BLACK_MEMORY_ADRS;

#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

		// 圧縮
		comprLen = size + COMPRESS_DOWNLOAD_ADD_MEMORY;
		if ((status = gzipComp ((unsigned char *)pAsciiBuff, downloadSize, (unsigned char *)pCompressBuff, &comprLen)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// アドレス&サイズ設定
		ptrSend = pCompressBuff;
		sendSize = comprLen;
		
		// ファイル名
		strcpy ((char *)fileName, FFC_SEND_FILE_COMPRESS_NAME);
	}
	else
	{
		// アドレス&サイズ設定
		ptrSend = (unsigned char *)pAsciiBuff;
		sendSize = downloadSize;
		
		// ファイル名
		strcpy ((char *)fileName, FFC_SEND_FILE_NAME);
	}
	
	// Kermit Send
	if ((kermitSendSize = kermitSend ((char *)fileName, (char *)ptrSend, sendSize)) < 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Download kermit Error. Size = %d\n", kermitSendSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:

#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	if (pAsciiBuff != NULL)
		free (pAsciiBuff);

	if (pCompressBuff != NULL)
		free (pCompressBuff);

#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	return (status);
}


//**********************************************************************************
//	FFC Download Data Bin=>ASCII変換
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pAscii				：ASCIIデータを格納するポインタ
//		pBinary				：Binaryデータを格納するポインタ
//		pSize				：変換サイズを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if !defined (MODE_IPU_MULTI)
int lfFfcDownloadBintoAscii (void *pAscii, void *pBinary, unsigned int *pSize)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i, j;
	unsigned char *ptrA;
	unsigned int *ptrBin32;
	unsigned int dataCount;
	unsigned int data32;
	unsigned int offsetData, gainData;
	char tempBuff[FFC_TEMP_SIZE];
	int len;
	int flipMode;
	int width, height = 0;
	unsigned int tempAdrs;
	int ffcSize;

	// Check pAscii Parameter
	if (pAscii == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Download pAscii NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pBinary Parameter
	if (pBinary == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Download pBinary NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Download pSize NULL Parameter Error\n");
		goto _DONE;
	}

	// Get Flip
#if !defined (MODE_SENSOR_XFLIP)
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	flipMode = XFLIP_DISABLE;
#endif
	
	// Bin=>ASCII変換
	dataCount = 0;
	ptrA = (unsigned char *)pAscii;
	ptrBin32 = (unsigned int *)pBinary;

	// Get Width
	width = WidthMax();

	// FFC Number
	ffcSize = FFC_DATA_NUM;

	for (i=0; dataCount<ffcSize; dataCount++)
	{
		if ((dataCount%width) == 0)
		{
			ptrBin32 = (unsigned int *)((unsigned int)pBinary + height * FFC_WIDTH_DATA_ALIGH);
			height++;

			// Flip有効?
		#if !defined (MODE_SENSOR_X_REVERSE)
			if (flipMode == XFLIP_ENABLE)
			{
				tempAdrs = (unsigned int)((unsigned int)ptrBin32 + (width - 1) * 4);
				ptrBin32 = (unsigned int *)tempAdrs;
			}
			#else
			if (flipMode == XFLIP_DISABLE)
			{
				tempAdrs = (unsigned int)((unsigned int)ptrBin32 + (width - 1) * 4);
				ptrBin32 = (unsigned int *)tempAdrs;
			}
			#endif
		}

		// 実際にメモリを使用しているカウントのCheck
		if (i > FFC_DOWNLOAD_SIZE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
			sprintf (gLogMsgBuff, "FFC Download Save Data Size Over Error. size = 0x%x\n", FFC_DOWNLOAD_SIZE);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		//------------------------------------------------------------
		// データ変換
		//------------------------------------------------------------
		data32 = *ptrBin32;

		// Flip有効?
#if !defined (MODE_SENSOR_X_REVERSE)
		if (flipMode == XFLIP_ENABLE)
			ptrBin32--;
		else
			ptrBin32++;
#else
		if (flipMode == XFLIP_DISABLE)
			ptrBin32--;
		else
			ptrBin32++;
#endif

		// オフセットデータ
		offsetData = (data32 >> OFFSET_SHIFT) & OFFSET_MASK;

		// ゲインデータ
		gainData = data32 & GAIN_MASK;

		//------------------------------------------------------------
		// オフセットデータをASCIIに変換
		//------------------------------------------------------------
		sprintf ((char *)tempBuff, "%d", offsetData);
		len = strlen ((char *)tempBuff);
		for (j=0; j<len; j++)
		{
			ptrA[i] = (unsigned char)(tempBuff[j]&0xff);

			i++;
			if (i > FFC_DOWNLOAD_SIZE)
				continue;
		}

		//------------------------------------------------------------
		// カンマ追加
		//------------------------------------------------------------
		if (i != 0)
			ptrA[i++] = ',';

		if (i > FFC_DOWNLOAD_SIZE)
			continue;

		//------------------------------------------------------------
		// ゲインデータをASCIIに変換
		//------------------------------------------------------------
		sprintf ((char *)tempBuff, "%d", gainData);
		len = strlen ((char *)tempBuff);
		for (j=0; j<len; j++)
		{
			ptrA[i] = (unsigned char)(tempBuff[j]&0xff);
			i++;
			if (i > FFC_DOWNLOAD_SIZE)
				continue;
		}

		//------------------------------------------------------------
		// カンマ追加
		//------------------------------------------------------------
		if (i != 0)
			ptrA[i++] = ',';

		if (i > FFC_DOWNLOAD_SIZE)
			continue;

		//------------------------------------------------------------
		// 改行
		//------------------------------------------------------------
		ptrA[i++] = 0x0d;	// \r
		if (i > FFC_DOWNLOAD_SIZE)
			continue;

		ptrA[i++]   = 0x0a;	// \n
		if (i > FFC_DOWNLOAD_SIZE)
			continue;
	}

	// データ数Check
	if (dataCount != ffcSize)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Upload Data Count Error. Count = %d\n", dataCount);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 変換サイズ
	*pSize = i;

_DONE:
	return (status);
}

#else // #if !defined (MODE_IPU_MULTI)

//**********************************************************************************
//	FFC Download Data Bin=>ASCII変換
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pAscii				：ASCIIデータを格納するポインタ
//		pBinary				：Binaryデータを格納するポインタ
//		pSize				：変換サイズを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lfFfcDownloadBintoAscii (void *pAscii, void *pBinary, unsigned int *pSize)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i, j;
	unsigned char *ptrA;
	unsigned int *ptrBin32;
	unsigned int dataCount;
	unsigned int data32;
	unsigned int offsetData, gainData;
	char tempBuff[FFC_TEMP_SIZE];
	int len;
	int flipMode;
	int width, height;
	int ffcSize;
	unsigned int x, y;
	int ipuWidtUnit, ipu;

	// Check pAscii Parameter
	if (pAscii == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Download pAscii NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pBinary Parameter
	if (pBinary == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Download pBinary NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Download pSize NULL Parameter Error\n");
		goto _DONE;
	}

	// Get Flip
#if !defined (MODE_SENSOR_XFLIP)
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	flipMode = XFLIP_DISABLE;
#endif
	
	// Bin=>ASCII変換
	dataCount = 0;
	ptrA = (unsigned char *)pAscii;
	ptrBin32 = (unsigned int *)pBinary;


	// Get Width
	width = IMG_WIDTH;

	// Get Height
	height = IMG_HEIGHT;

	// FFC Number
	ffcSize = FFC_DATA_NUM_ALIGN;

	ipuWidtUnit = width / IPU_COUNT;
	ipu = 0;
	dataCount = 0;
	i= 0;
	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++, dataCount++)
		{
			if ((x%ipuWidtUnit) == 0)
			{
				ipu = x / ipuWidtUnit;

				ptrBin32 = (unsigned int *)((unsigned long)pBinary + y * FFC_WIDTH_DATA_ALIGH + (ipu * FFC_MEMORY_IPU_MULTI_INTERVAL));

				if (ipu != 0)
					ptrBin32 += IMG_WIDTH_OFFSET;

				// Flip有効?
				#if !defined (MODE_SENSOR_X_REVERSE)

				if (flipMode == XFLIP_ENABLE)
					ptrBin32 = (unsigned int *)((unsigned long)ptrBin32 + (width - 1) * 4);

				#else

				if (flipMode == XFLIP_DISABLE)
					ptrBin32 = (unsigned int *)((unsigned long)ptrBin32 + (width - 1) * 4);

				#endif
			}

			// 実際にメモリを使用しているカウントのCheck
			if (i > FFC_DOWNLOAD_SIZE)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
				sprintf (gLogMsgBuff, "FFC Download Save Data Size Over Error. size = 0x%x\n", FFC_DOWNLOAD_SIZE);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			//------------------------------------------------------------
			// データ変換
			//------------------------------------------------------------
			data32 = *ptrBin32;

			// Flip有効?
	#if !defined (MODE_SENSOR_X_REVERSE)
			if (flipMode == XFLIP_ENABLE)
				ptrBin32--;
			else
				ptrBin32++;
	#else
			if (flipMode == XFLIP_DISABLE)
				ptrBin32--;
			else
				ptrBin32++;
	#endif

			// オフセットデータ
			offsetData = (data32 >> OFFSET_SHIFT) & OFFSET_MASK;

			// ゲインデータ
			gainData = data32 & GAIN_MASK;

			//------------------------------------------------------------
			// オフセットデータをASCIIに変換
			//------------------------------------------------------------
			sprintf ((char *)tempBuff, "%d", offsetData);
			len = strlen ((char *)tempBuff);
			for (j=0; j<len; j++)
			{
				ptrA[i] = (unsigned char)(tempBuff[j]&0xff);

				i++;
				if (i > FFC_DOWNLOAD_SIZE)
					continue;
			}

			//------------------------------------------------------------
			// カンマ追加
			//------------------------------------------------------------
			if (i != 0)
				ptrA[i++] = ',';

			if (i > FFC_DOWNLOAD_SIZE)
				continue;

			//------------------------------------------------------------
			// ゲインデータをASCIIに変換
			//------------------------------------------------------------
			sprintf ((char *)tempBuff, "%d", gainData);
			len = strlen ((char *)tempBuff);
			for (j=0; j<len; j++)
			{
				ptrA[i] = (unsigned char)(tempBuff[j]&0xff);
				i++;
				if (i > FFC_DOWNLOAD_SIZE)
					continue;
			}

			//------------------------------------------------------------
			// カンマ追加
			//------------------------------------------------------------
			if (i != 0)
				ptrA[i++] = ',';

			if (i > FFC_DOWNLOAD_SIZE)
				continue;

			//------------------------------------------------------------
			// 改行
			//------------------------------------------------------------
			ptrA[i++] = 0x0d;	// \r
			if (i > FFC_DOWNLOAD_SIZE)
				continue;

			ptrA[i++]   = 0x0a;	// \n
			if (i > FFC_DOWNLOAD_SIZE)
				continue;
		}
	}

	// データ数Check
	if (dataCount != ffcSize)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Upload Data Count Error. Count = %d\n", dataCount);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 変換サイズ
	*pSize = i;

_DONE:
	return (status);
}
#endif // #if !defined (MODE_IPU_MULTI)


//**********************************************************************************
//	FFC Data Download(レジスタ版)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：updateするFFC番号
//		mode				：圧縮モード(0=圧縮/1=非圧縮))
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcDataDownloadBin (int ffcNum, int mode)
{
	int status;
	unsigned int size = FFC_DOWNLOAD_SIZE;
	unsigned int memAdrs;
	char *pAsciiBuff = NULL;
	unsigned char *pCompressBuff = NULL;
	int downloadSize;
	int kermitSendSize;
	int comprLen;
	unsigned char *ptrSend;
	int sendSize;
	char *fileName[64];

	// Check ffcNo Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Download ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check mode Parameter
	if ((mode != UPDATE_COMPRESS) && (mode != UPDATE_UNCOMPRESS))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Download mode(%d) Parameter Error. (Compress:%d / Uncpmpress:%d)\n", mode, UPDATE_COMPRESS, UPDATE_UNCOMPRESS);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Memory Adrs
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Download FFC Mem Adrs Error\n");
		goto _DONE;
	}

#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	// メモリリクエスト(ASCIIデータ格納用)
	if ((pAsciiBuff = malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Download Buffer Request Error\n");
		goto _DONE;
	}

#else // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	pAsciiBuff = (char *)FFC0_TEMP_MEMORY_ADRS;

#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	// FFCメモリ領域からのコピー
	if ((status = ffcGetMemoryNormalFirst ((void *)pAsciiBuff, 0, FFC_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFCデータSWAP
	if ((status = ffcDataSwap ((unsigned int)pAsciiBuff, FFC_DATA_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// サイズ取得
	downloadSize = FFC_DATA_SIZE;

	// 圧縮あり？
	if (mode == UPDATE_COMPRESS)
	{
#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)
		// メモリリクエスト(圧縮データ格納用)
		if ((pCompressBuff = malloc (size + COMPRESS_DOWNLOAD_ADD_MEMORY)) == NULL)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Data Download Buffer Request Error\n");
			goto _DONE;
		}
#else // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

		pCompressBuff = (unsigned char *)FFC_BLACK_MEMORY_ADRS;

#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

		// 圧縮
		comprLen = size + COMPRESS_DOWNLOAD_ADD_MEMORY;
		if ((status = gzipComp ((unsigned char *)pAsciiBuff, downloadSize, (unsigned char *)pCompressBuff, &comprLen)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// アドレス&サイズ設定
		ptrSend = pCompressBuff;
		sendSize = comprLen;

		// ファイル名
		strcpy ((char *)fileName, FFC_SEND_FILE_BIN_COMPRESS_NAME);
	}
	else
	{
		// アドレス&サイズ設定
		ptrSend = (unsigned char *)pAsciiBuff;
		sendSize = downloadSize;

		// ファイル名
		strcpy ((char *)fileName, FFC_SEND_FILE_BIN_NAME);
	}

	// Kermit Send
	if ((kermitSendSize = kermitSend ((char *)fileName, (char *)ptrSend, sendSize)) < 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Download kermit Error. Size = %d\n", kermitSendSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)
	if (pAsciiBuff != NULL)
		free (pAsciiBuff);

	if (pCompressBuff != NULL)
		free (pCompressBuff);
#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)

	return (status);
}

#if !defined (MODE_IPU_MULTI)
//**********************************************************************************
// FFCデータを配置しなおし
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNo				： FFC番号
//		pImageOffset		： オフセットデータが格納されたポインタ
//		pImageGain			： ゲインデータが格納されたポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetMemoryOffsetGain (int ffcNo, unsigned int memAdrs, unsigned short *pImageOffset, int *pImageGain)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *ptrWriteB;
	unsigned char *ptrReadB;
	int i;
	unsigned int oData;
	int gData;
	int x, y;
	unsigned char data8;
	unsigned int data32;
	
	i=0;
	for (y=0; y<IMG_HEIGHT; y++)
	{
		// 外部メモリアドレス設定
		ptrWriteB = (unsigned char *)(memAdrs + FFC_WIDTH_DATA_ALIGH * y);
		ptrReadB = ptrWriteB;

		for (x=0; x<IMG_WIDTH; x++, i++)
		{
			// Read
			data8 = *ptrReadB;
			data32 = (unsigned int)(data8 << 16);
			ptrReadB++;

			data8 = *ptrReadB;
			data32 |= (unsigned int)(data8 << 8);
			ptrReadB++;
			
			data8 = *ptrReadB;
			data32 |= (unsigned int)data8;
			ptrReadB++;
			
			// Save
			oData = (data32>>16) & 0xffff;
			gData = data32 & 0xff;

			// オフセットデータがNULLでなければ新規データを設定
			if (pImageOffset != NULL)
				oData = (unsigned int)pImageOffset[i];

			// ゲインデータがNULLでなければ新規データを設定
			if (pImageGain != NULL)
				gData = (int)pImageGain[i];

			// Write
			data32 = (oData & 0xffff) << 16;
			data32 |= (gData & 0xffff);

			*ptrWriteB = (unsigned char)(data32 >> 16);
			ptrWriteB++;

			*ptrWriteB = (unsigned char)(data32 >> 8);
			ptrWriteB++;

			*ptrWriteB = (unsigned char)data32;
			ptrWriteB++;
		}
	}

	// キャッシュFlash
	cacheFlush ();

	return (status);
}

#else // #if !defined (MODE_IPU_MULTI)

//**********************************************************************************
// FFCデータを配置しなおし
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNo				： FFC番号
//		pImageOffset		： オフセットデータが格納されたポインタ
//		pImageGain			： ゲインデータが格納されたポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetMemoryOffsetGain (int ffcNo, unsigned int memAdrs, unsigned short *pImageOffset, int *pImageGain)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *ptrWriteB;
	unsigned char *ptrReadB;
	unsigned int oData;
	int gData;
	unsigned int data;
	int x, y;
	int ipu;
	unsigned short *pPtrOffset;
	int *pPtrGain;
	int xOffset;
	unsigned char data8;
	unsigned int data32;

	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		for (y=0; y<IMG_HEIGHT; y++)
		{
			if (ipu == 0)
				xOffset = 0;
			else
				xOffset = IMG_WIDTH_OFFSET;

			// 外部メモリアドレス設定
			ptrWriteB = (unsigned char *)(memAdrs + (FFC_WIDTH_DATA_ALIGH * y) + (FFC_MEMORY_IPU_MULTI_INTERVAL * ipu) + (xOffset * 4));
			ptrReadB = ptrWriteB;

			// オフセット格納アドレス
			pPtrOffset = (unsigned short *)(pImageOffset + (y * IMG_WIDTH) + (ipu * (IMG_WIDTH/IPU_COUNT)));

			// ゲイン格納アドレス
			pPtrGain = (int *)(pImageGain + (y * IMG_WIDTH) + ipu * (IMG_WIDTH/IPU_COUNT));

			for (x=0; x<IMG_WIDTH_IPU_SIZE; x++, pPtrOffset++, pPtrGain++)
			{
				// Read
				data8 = *ptrReadB;
				data32 = (unsigned int)(data8 << 16);
				ptrReadB++;

				data8 = *ptrReadB;
				data32 |= (unsigned int)(data8 << 8);
				ptrReadB++;
				
				data8 = *ptrReadB;
				data32 |= (unsigned int)data8;
				ptrReadB++;
				
				// Save
				oData = (data32>>16) & 0xffff;
				gData = data32 & 0xff;

				// オフセットデータがNULLでなければ新規データを設定
				if (pImageOffset != NULL)
					oData = (unsigned int)*pPtrOffset;

				// ゲインデータがNULLでなければ新規データを設定
 				if (pImageGain != NULL)
					gData = (int)*pPtrGain;

				// Write
				data32 = (oData & 0xffff) << 16;
				data32 |= (gData & 0xffff);

				*ptrWriteB = (unsigned char)(data32 >> 16);
				ptrWriteB++;

				*ptrWriteB = (unsigned char)(data32 >> 8);
				ptrWriteB++;

				*ptrWriteB = (unsigned char)data32;
				ptrWriteB++;
			}
		}
	}

	// ipuの1個目の最後の16画素の追加。
	// ipuの2個目の最初の16画素の追加。
	if ((status = ffcSetMarginGridData ()) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}
#endif // #if !defined (MODE_IPU_MULTI)


#if !defined (MODE_IPU_MULTI)
//**********************************************************************************
// FFCデータを取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNo				： FFC番号
//		memAdrs				： FFCデータが格納されたアドレス
//		pImageOffset		： オフセットデータを格納するポインタ
//		pImageGain			： ゲインデータを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetMemoryOffsetGain (int ffcNo, unsigned int memAdrs, unsigned short *pImageOffset, int *pImageGain)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *ptrL;
	int i;
	unsigned int oData;
	int gData;
	unsigned int data;
	int x, y;

	i=0;
	for (y=0; y<IMG_HEIGHT; y++)
	{
		// 外部メモリアドレス設定
		ptrL = (unsigned int *)(memAdrs + FFC_WIDTH_DATA_ALIGH * y);

		for (x=0; x<IMG_WIDTH; x++, ptrL++, i++)
		{
			// Read
			data = *ptrL;

			// Save
			oData = OFFSET_GET_DATA (data);
			gData = GAIN_GET_DATA (data);

			// オフセットデータがNULLでなければ新規データを設定
			if (pImageOffset != NULL)
				pImageOffset[i] = (unsigned short)oData;

			// ゲインデータがNULLでなければ新規データを設定
			if (pImageGain != NULL)
				pImageGain[i] = (int)gData;
		}
	}

	// キャッシュFlash
	cacheFlush ();

	return (status);
}

#else // #if !defined (MODE_IPU_MULTI)

//**********************************************************************************
// FFCデータを取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNo				： FFC番号
//		memAdrs				： FFCデータが格納されたアドレス
//		pImageOffset		： オフセットデータを格納するポインタ
//		pImageGain			： ゲインデータを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetMemoryOffsetGain (int ffcNo, unsigned int memAdrs, unsigned short *pImageOffset, int *pImageGain)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *ptrL;
	unsigned int oData;
	int gData;
	unsigned int data;
	int x, y;
	int ipu;
	unsigned short *pPtrOffset;
	int *pPtrGain;
	int xOffset;

	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		for (y=0; y<IMG_HEIGHT; y++)
		{
			if (ipu == 0)
				xOffset = 0;
			else
				xOffset = IMG_WIDTH_OFFSET;

			// 外部メモリアドレス設定
			ptrL = (unsigned int *)(memAdrs + (FFC_WIDTH_DATA_ALIGH * y) + (FFC_MEMORY_IPU_MULTI_INTERVAL * ipu) + (xOffset * 4));

			// オフセット格納アドレス
			pPtrOffset = (unsigned short *)(pImageOffset + (y * IMG_WIDTH) + (ipu * (IMG_WIDTH/IPU_COUNT)));

			// ゲイン格納アドレス
			pPtrGain = (int *)(pImageGain + (y * IMG_WIDTH) + ipu * (IMG_WIDTH/IPU_COUNT));

			for (x=0; x<IMG_WIDTH_IPU_SIZE; x++, ptrL++, pPtrOffset++, pPtrGain++)
			{
				// Read
				data = *ptrL;

				// Save
				oData = OFFSET_GET_DATA (data);
				gData = GAIN_GET_DATA (data);

				// オフセットデータがNULLでなければ新規データを設定
				if (pImageOffset != NULL)
					*pPtrOffset = oData;

				// ゲインデータがNULLでなければ新規データを設定
 				if (pImageGain != NULL)
					*pPtrGain = gData;

				*ptrL = (unsigned int)(OFFSET_SET_DATA(oData) | GAIN_SET_DATA(gData));
			}
		}
	}

	// ipuの1個目の最後の16画素の追加。
	// ipuの2個目の最初の16画素の追加。
	if ((status = ffcSetMarginGridData ()) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}

#endif // #if !defined (MODE_IPU_MULTI)


//**********************************************************************************
// FFCデータをDDR管理外から内部メモリに移動
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		extMemAdrs			： 外部メモリアドレス
//		intMemAdrs			： 内部メモリアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetExtMemory (unsigned int extMemAdrs, unsigned int intMemAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	int x, y;
	int p;
	unsigned char *prtExtMem8;
	unsigned char *prtIntMem8;
#if defined (MODE_IPU_MULTI)
	int ipu;
#endif

	// Check extMemAdrs Parameter
	if (extMemAdrs == 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Set External Addressd extMemAdrs Parameter Error\n");
		goto _DONE;
	}

	// Check intMemAdrs Parameter
	if (intMemAdrs == 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Set External Addressd intMemAdrs Parameter Error\n");
		goto _DONE;
	}

	// 内部メモリアドレス設定
	prtIntMem8 = (unsigned char *)intMemAdrs;

#if !defined (MODE_IPU_MULTI)

	for (y=0; y<IMG_HEIGHT; y++)
	{
		// 外部メモリアドレス設定
		prtExtMem8 = (unsigned char *)(extMemAdrs + FFC_WIDTH_DATA_ALIGH * y);

		for (x=0; x<IMG_WIDTH; x++)
		{
			// FFCデータが3画素
			for (p=0; p<3; p++, prtExtMem8++, prtIntMem8++)
				*prtIntMem8 = *prtExtMem8;
		}
	}

#else // #if !defined (MODE_IPU_MULTI)

	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		for (y=0; y<IMG_HEIGHT; y++)
		{
			// 外部メモリアドレス設定
			prtExtMem8 = (unsigned int *)(extMemAdrs + FFC_WIDTH_DATA_ALIGH * y + FFC_MEMORY_IPU_MULTI_INTERVAL * ipu);

			for (x=0; x<IMG_WIDTH_IPU_MULTI_HALF; x++)
			{
				// FFCデータが3画素
				for (p=0; p<3; p++, prtExtMem8++, prtIntMem8++)
					*prtIntMem8 = *prtExtMem8;
			}
		}
	}

#endif // #if !defined (MODE_IPU_MULTI)

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
// FFCデータを内部メモリからDDR管理外に移動
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		extMemAdrs			： 外部メモリアドレス
//		intMemAdrs			： 内部メモリアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetExtMemory (unsigned int extMemAdrs, unsigned int intMemAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	int x, y;
	unsigned char *prtExtMem8;
	unsigned char *prtIntMem8;
	int p;
#if defined (MODE_IPU_MULTI)
	int ipu;
#endif

	// Check extMemAdrs Parameter
	if (extMemAdrs == 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Set Internal Addressd extMemAdrs Parameter Error\n");
		goto _DONE;
	}

	// Check intMemAdrs Parameter
	if (intMemAdrs == 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Set Internal Addressd intMemAdrs Parameter Error\n");
		goto _DONE;
	}

	// 内部メモリアドレス設定
	prtIntMem8 = (unsigned char *)intMemAdrs;

#if !defined (MODE_IPU_MULTI)

	for (y=0; y<IMG_HEIGHT; y++)
	{
		// 外部メモリアドレス設定
		prtExtMem8 = (unsigned char *)(extMemAdrs + FFC_WIDTH_DATA_ALIGH * y);

		for (x=0; x<IMG_WIDTH; x++)
		{
			// FFCデータが3画素
			for (p=0; p<3; p++, prtExtMem8++, prtIntMem8++)
				*prtExtMem8 = *prtIntMem8;
		}
	}

#else // #if !defined (MODE_IPU_MULTI)

	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		for (y=0; y<IMG_HEIGHT; y++)
		{
			// 外部メモリアドレス設定
			prtExtMem8 = (unsigned char *)(extMemAdrs + FFC_WIDTH_DATA_ALIGH * y + FFC_MEMORY_IPU_MULTI_INTERVAL * ipu);

			for (x=0; x<IMG_WIDTH_IPU_MULTI_HALF; x++)
			{
				// FFCデータが3画素
				for (p=0; p<3; p++, prtExtMem8++, prtIntMem8++)
					*prtExtMem8 = *prtIntMem8;
			}
		}
	}

#endif // #if !defined (MODE_IPU_MULTI)

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


#if !defined (MODE_IPU_MULTI)
//**********************************************************************************
// FFCオフセット/ゲインデータをFlashに書き込み(Ver.1.5)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		memAdrs				：オフセットデータが格納されたメモリアドレス
//		size				：書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcToFlash (unsigned int flashAdrs, unsigned int memAdrs, int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;
#if defined (MODE_SENSOR_XFLIP)
	int flipMode;
#endif
	int y;
	unsigned int flashAdrs2, memAdrs2;
	int size2;

	// FFCデータ反転
#if defined (MODE_SENSOR_XFLIP)
	// Flip取得
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE

	// FFCデータ反転
	if (flipMode == MODE_ENABLE)
	{
		if ((status = ffcDataXFlip (flipMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE
	}
#endif

	// Erase
	for (adrs=flashAdrs; adrs<(flashAdrs + size); adrs+=AXI_QSPI_FLASH_SEC_SIZE)
	{
		if ((status = qspiFlashSectorErase (adrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE
	}

	// Write
	flashAdrs2 = flashAdrs;
	memAdrs2 = memAdrs;
	size2 = FFC_WIDTH_DATA_SIZE_FIRST;
	for (y=0; y<IMG_HEIGHT; y++)
	{
		if ((status = qspiFlashWrite (flashAdrs2, (unsigned char *)memAdrs2, size2)) != AVAL_STATUS_SUCCESS)
			goto _DONE

		flashAdrs2 += FFC_WIDTH_DATA_SIZE_FIRST;
		memAdrs2 += FFC_WIDTH_DATA_ALIGH;
	}


	// FFCデータ反転
#if defined (MODE_SENSOR_XFLIP)
	// Flip取得
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE

	// FFCデータ反転
	if (flipMode == MODE_ENABLE)
	{
		if ((status = ffcDataXFlip (flipMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE
	}
#endif

_DONE:
	return (status);
}

#else // #if !defined (MODE_IPU_MULTI)

//**********************************************************************************
// FFCオフセット/ゲインデータをFlashに書き込み(Ver.1.5)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		memAdrs				：オフセットデータが格納されたメモリアドレス
//		size				：書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcToFlash (unsigned int flashAdrs, unsigned int memAdrs, int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;
	int y, ipu;
	unsigned int flashAdrs2;
	int size2;
	unsigned long memAdrs2;
#if defined (MODE_SENSOR_XFLIP)
	int flipMode;
#endif
	
	//-----------------------------------------------------------
	// FFCデータ反転
	//-----------------------------------------------------------
#if defined (MODE_SENSOR_XFLIP)
	// Flip取得
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFCデータ反転
	if (flipMode == MODE_ENABLE)
	{
		if ((status = ffcDataXFlip (flipMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	//-----------------------------------------------------------
	// Erase
	//-----------------------------------------------------------
	for (adrs=flashAdrs; adrs<(flashAdrs + size); adrs+=AXI_QSPI_FLASH_SEC_SIZE)
	{
		if ((status = qspiFlashSectorErase (adrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//-----------------------------------------------------------
	// Write
	//-----------------------------------------------------------
	flashAdrs2 = flashAdrs;
	size2 = IMG_WIDTH / IPU_COUNT * 3;
	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		memAdrs2 = memAdrs + FFC_MEMORY_IPU_MULTI_INTERVAL * ipu;
		if (ipu != 0)
			memAdrs2 += (IMG_WIDTH_OFFSET * 3);

		for (y=0; y<IMG_HEIGHT; y++)
		{
			if ((status = qspiFlashWrite (flashAdrs2, (unsigned char *)memAdrs2, size2)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			flashAdrs2 += size2;
			memAdrs2 += FFC_WIDTH_DATA_ALIGH;
		}
	}

	//-----------------------------------------------------------
	// FFCデータ反転
	//-----------------------------------------------------------
#if defined (MODE_SENSOR_XFLIP)
	// Flip取得
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFCデータ反転
	if (flipMode == MODE_ENABLE)
	{
		if ((status = ffcDataXFlip (flipMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

_DONE:
	return (status);
}

#endif // #if !defined (MODE_IPU_MULTI)


//**********************************************************************************
// FFCオフセット/ゲインデータをFlashに書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		memAdrs				：オフセットデータが格納されたメモリアドレス
//		size				：書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcToFlashAdmin (unsigned int flashAdrs, unsigned int memAdrs, int size)
{
	int status = AVAL_STATUS_SUCCESS;

	status = ffcToFlash (flashAdrs, memAdrs, size);

	return (status);
}


#if !defined (MODE_IPU_MULTI)
//**********************************************************************************
// FFCオフセット/ゲインデータをメモリに書き込む(Ver.1.5)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：FFC Flashアドレス
//		memAdrs				：FFC メモリアドレス
//		size				：FFC サイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcToMemory (unsigned int flashAdrs, unsigned int memAdrs, int size)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_SENSOR_XFLIP)
	int flipMode;
#endif
	int y;
	unsigned int flashAdrs2, memAdrs2;
	int size2;

	flashAdrs2 = flashAdrs;
	memAdrs2 = memAdrs;
	size2 = FFC_WIDTH_DATA_SIZE_FIRST;
	for (y=0; y<IMG_HEIGHT; y++)
	{
		if ((status = qspiFlashRead (flashAdrs2, (unsigned char *)memAdrs2, size2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		flashAdrs2 += FFC_WIDTH_DATA_SIZE_FIRST;
		memAdrs2 += FFC_WIDTH_DATA_ALIGH;
	}


	// FFCデータ反転
#if defined (MODE_SENSOR_XFLIP)
	// Flip取得
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFCデータ反転
	if (flipMode == MODE_ENABLE)
	{
		if ((status = ffcDataXFlip (flipMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}

#else // #if !defined (MODE_IPU_MULTI)

//**********************************************************************************
// FFCオフセット/ゲインデータをメモリに書き込む
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：FFC Flashアドレス
//		memAdrs				：FFC メモリアドレス
//		size				：FFC サイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcToMemory (unsigned int flashAdrs, unsigned int memAdrs, int size)
{
	int status;
#if defined (MODE_SENSOR_XFLIP)
	int flipMode;
#endif
	int y, ipu;
	unsigned int flashAdrs2;
	int size2;
	unsigned long memAdrs2;

	flashAdrs2 = flashAdrs;
	size2 = IMG_WIDTH / IPU_COUNT * 3;

	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		memAdrs2 = memAdrs + FFC_MEMORY_IPU_MULTI_INTERVAL * ipu;

		if (ipu != 0)
			memAdrs2 += (IMG_WIDTH_OFFSET * 3);

		for (y=0; y<IMG_HEIGHT; y++)
		{
			if ((status = qspiFlashRead (flashAdrs2, (unsigned char *)memAdrs2, size2)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			flashAdrs2 += size2;
			memAdrs2 += FFC_WIDTH_DATA_ALIGH;
		}
	}

	// ipuの1個目の最後の16画素の追加。
	// ipuの2個目の最初の16画素の追加。
	if ((status = ffcSetMarginGridData()) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFCデータ反転
#if defined (MODE_SENSOR_XFLIP)
	// Flip取得
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFCデータ反転
	if (flipMode == MODE_ENABLE)
	{
		if ((status = ffcDataXFlip (flipMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}

#endif // #if !defined (MODE_IPU_MULTI)


//**********************************************************************************
// FFCオフセット/ゲインデータをメモリに書き込む
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：FFC Flashアドレス
//		memAdrs				：FFCメモリアドレス
//		size				：FFCサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcToMemoryAdmin (unsigned int flashAdrs, unsigned int memAdrs, int size)
{
	int status;

	status = ffcToMemory (flashAdrs, memAdrs, size);

	return (status);
}


//**********************************************************************************
// FFCコピー(AXI QSPI間のみのコピー)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		srcFfcNum			：コピー元FFC番号
//		desFfcNum			：コピー先FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcCopy (int srcFfcNum, int desFfcNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int srcFlashAdrs, desFlashAdrs;
	unsigned char *pBuffer = NULL;
	FFCINFO ffcInfo;
	int size;
	int flashSize;
	int factoryMode;
	int startMode = 0;

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_XFLIP)
	// Flipしない
	ffcSetXFlipFlag (MODE_DISABLE);
#endif

	size = FFC_ALL_SIZE_FIRST;
	flashSize = FFC_DATA_SIZE;

	// Memory Request
	if ((pBuffer = (unsigned char *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Copy Buffer Request Error\n");
		goto _DONE;
	}

	// Check srcFfcNum Parameter
	if ((srcFfcNum < FFC_NUMBER_MIN) || (srcFfcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Copy Source ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", srcFfcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check desFfcNum Parameter
	if ((desFfcNum < (FFC_NUMBER_MIN + 1)) || (desFfcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Copy Destination ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", desFfcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check srcFfcNum & desFfcNum Parameter
	if (srcFfcNum == desFfcNum)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Copy Source ffcNo(%d)/Destination ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", srcFfcNum, desFfcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 転送元Flashアドレス取得
	if ((status = ffcGetFlashAdrs (srcFfcNum, &srcFlashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 転送先Flashアドレス取得
	if ((status = ffcGetFlashAdrs (desFfcNum, &desFlashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 転送元をFlashからメモリへ展開
	if ((status = ffcToMemory (srcFlashAdrs, (unsigned int)pBuffer, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 転送先へ書き込み
	if ((status = ffcToFlash (desFlashAdrs, (unsigned int)pBuffer, flashSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC黒レベル情報読み込み
	if ((status = ffcAdjustInfoBlackRead (srcFfcNum, FFC_USER, &ffcInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC黒レベル情報書き込み
	if ((status = ffcAdjustInfoBlackSave (desFfcNum, FFC_USER, ffcInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC白レベル情報読み込み
	if ((status = ffcAdjustInfoWhiteRead (srcFfcNum, FFC_USER, &ffcInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC白レベル情報書き込み
	if ((status = ffcAdjustInfoWhiteSave (desFfcNum, FFC_USER, ffcInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (pBuffer != NULL)
		free (pBuffer);

	if (startMode != 0)
		acquisitionStart ();

#if defined (MODE_SENSOR_XFLIP)
	// Flipする
	ffcSetXFlipFlag (MODE_ENABLE);
#endif

	return (status);
}


//**********************************************************************************
//	FFC Copy All
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcDefault (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_XFLIP)
	// Flipしない
	ffcSetXFlipFlag (MODE_DISABLE);
#endif

	// FFCコピー
	if ((status = ffcCopy (0, 1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFCコピー
	if ((status = ffcCopy (0, 2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startMode != 0)
		acquisitionStart ();

#if defined (MODE_SENSOR_XFLIP)
	// Flipする
	ffcSetXFlipFlag (MODE_ENABLE);
#endif

	return (status);
}


//**********************************************************************************
//	Black Target設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：黒レベル目標値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetBlackTarget (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	int calc;

#if defined (MODE_FFC_BIT_CALC)
	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	calc = 1;
#endif

	// Check data Parameter
	if ((data < FFC_BLACK_TARGET_MIN) || (data > FFC_BLACK_TARGET_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Black Target data(%d) Parameter Error. (Min:%d / Max:%d)\n", data/calc, FFC_BLACK_TARGET_MIN, FFC_BLACK_TARGET_MAX/calc);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 黒レベル目標値設定
	OUT32 (FPGA_FFC_BLACK_TARGET_ADRS, data);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_FFC2_BLACK_TARGET_ADRS, data);
#endif

	// DDR管理外黒レベル目標値設定
	OUT32 (FIRM_DATA_FFC_BLACK_ADRS, data);

_DONE:
	return (status);
}


//**********************************************************************************
//	Black Target取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：黒レベル目標値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetBlackTarget (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Black Target pData NULL Parameter Error\n");
		goto _DONE;
	}

	// 黒レベル目標値
	*pData = IN32 (FPGA_FFC_BLACK_TARGET_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	White Target設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：白レベル目標値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetWhiteTarget (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check data Parameter
	if ((data < FFC_WHITE_LEVEL_MIN) || (data > FFC_WHITE_LEVEL_BIT14_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC White Target data(%d) Parameter Error. (Min:%d / Max:%d)\n", data, FFC_WHITE_LEVEL_MIN, FFC_WHITE_LEVEL_BIT14_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DDR管理外白レベル目標値設定
	OUT32 (FIRM_DATA_FFC_WHITE_ADRS, data);

_DONE:
	return (status);
}


//**********************************************************************************
//	White Target取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：白レベル目標値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetWhiteTarget (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC White Target pData NULL Parameter Error\n");
		goto _DONE;
	}

	// 白レベル目標値
	*pData = IN32 (FIRM_DATA_FFC_WHITE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	Black Target取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		pData				：黒レベル目標値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetBlackTargetFromFlashAdmin (int ffcNum, unsigned int *pData)
{
	int status;
	
	status = ffcGetBlackTargetFromFlash (ffcNum, pData);
	
	return (status);
}


//**********************************************************************************
//	Black Target取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		pData				：黒レベル目標値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetBlackTargetFromFlash (int ffcNum, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int offset;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Black Target(Flash) ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Black Target(Flash) pData NULL Parameter Error\n");
		goto _DONE;
	}

	// アドレスCopy
	offset = ffcNum * FFC_SAVE_INFO_SIZE + FFC_SAVE_BLACK_TARGET;

	// FFC情報読み込み
	if ((status = qspiFlashRead ((FLASH_FFC_INFO_ADRS + offset), (unsigned char *)pData, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Mode Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcAdjustCheckModeParam (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int corMode;

	// FFC Correction Mode取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// 1次補正
	//--------------------------------------------------------------------------------
	if (corMode == FFC_CORRECTION_MODE_FIRST)
	{
		// Check index Parameter
		if ((mode < FFC_ADJUST_BLACK) || (mode > FFC_ADJUST_WHITE))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Get Adjust mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, FFC_ADJUST_BLACK, FFC_ADJUST_WHITE);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
	//--------------------------------------------------------------------------------
	// Shading Line補正
	//--------------------------------------------------------------------------------
	else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		// Check index Parameter
		if ((mode < FFC_ADJUST_BLACK) || (mode > FFC_ADJUST_WHITE))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Get Adjust mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, FFC_ADJUST_BLACK, FFC_ADJUST_WHITE);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Rate設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		rate				：FFC調整時のRate
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetAdjustRate (int mode, float rate)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Rate取得
	if (mode == FFC_ADJUST_BLACK)
	{
		ffcBlackInfo.mark = FFC_MARK_DATA;
		ffcBlackInfo.frameRate = (double)rate;
		OUTF (FIRM_DATA_FFC_ADJUST_BLACK_ADRS, rate);
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		ffcWhiteInfo.mark = FFC_MARK_DATA;
		ffcWhiteInfo.frameRate = (double)rate;
		OUTF (FIRM_DATA_FFC_ADJUST_WHITE_ADRS, rate);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Rate取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		pRate				：FFC調整時のRateを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetAdjustRate (int mode, float *pRate)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// check pRate Parameter
	if (pRate == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Adjust Get Rate pRate NULL Parameter Error.\n");
		goto _DONE;
	}

	// Rate取得
	if (mode == FFC_ADJUST_BLACK)
	{
		*pRate = (float)ffcBlackInfo.frameRate;
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		*pRate = (float)ffcWhiteInfo.frameRate;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Exposure Time設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		exp					：FFC調整時の露光時間
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetAdjustExposure (int mode, int exp)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Exposure Time取得
	if (mode == FFC_ADJUST_BLACK)
	{
		ffcBlackInfo.mark = FFC_MARK_DATA;
		ffcBlackInfo.expTime = exp;
		OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_EXP_ADRS, exp);
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		ffcWhiteInfo.mark = FFC_MARK_DATA;
		ffcWhiteInfo.expTime = exp;
		OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_EXP_ADRS, exp);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Exposure Time取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		pExp				：FFC調整時の露光時間を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetAdjustExposure (int mode, int *pExp)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// check pExp Parameter
	if (pExp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Adjust Get Exposure pExp NULL Parameter Error.\n");
		goto _DONE;
	}

	// Exposure Time取得
	if (mode == FFC_ADJUST_BLACK)
	{
		*pExp = ffcBlackInfo.expTime;
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		*pExp = ffcWhiteInfo.expTime;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Target設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		target				：FFC調整時の目標値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetAdjustTarget (int mode, int target)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check target Parameter
	if ((target < 0) || (target > PIXEL_14_MASK))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Adjust Set target(%d) Parameter Error. (Min:%d / Max:%d)\n", target, 0, PIXEL_14_MASK);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Target取得
	if (mode == FFC_ADJUST_BLACK)
	{
		ffcBlackInfo.mark = FFC_MARK_DATA;
		ffcBlackInfo.target = target;
		OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_ADRS, target);
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		ffcWhiteInfo.mark = FFC_MARK_DATA;
		ffcWhiteInfo.target = target;
		OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_ADRS, target);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Target取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		pTarget				：FFC調整時の目標値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetAdjustTarget (int mode, int *pTarget)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// check pTarget Parameter
	if (pTarget == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Adjust Get Target pExp NULL Parameter Error.\n");
		goto _DONE;
	}

	// Target取得
	if (mode == FFC_ADJUST_BLACK)
	{
		*pTarget = ffcBlackInfo.target;
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		*pTarget = ffcWhiteInfo.target;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Temperature設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		temp				：FFC調整時の温度を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetAdjustTemp (int mode, float temp)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check temp Parameter
	if ((temp < PELTIER_SENSOR_ASJUST_TEMP_MIN) || (temp > PELTIER_SENSOR_ASJUST_TEMP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Adjust Set Temperature(%.2f) Parameter Error. (Min:%.2f / Max:%.2f)\n", temp, PELTIER_SENSOR_ASJUST_TEMP_MIN, PELTIER_SENSOR_ASJUST_TEMP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 温度取得
	if (mode == FFC_ADJUST_BLACK)
	{
		ffcBlackInfo.mark = FFC_MARK_DATA;
		ffcBlackInfo.sensorTemp = (double)temp;
		OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_TEMP_ADRS, temp);
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		ffcWhiteInfo.mark = FFC_MARK_DATA;
		ffcWhiteInfo.sensorTemp = (double)temp;
		OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_TEMP_ADRS, temp);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Temperature取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		pTemp				：FFC調整時の温度を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetAdjustTemp (int mode, float *pTemp)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// check pTemp Parameter
	if (pTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Adjust Get Temperature pTemp NULL Parameter Error.\n");
		goto _DONE;
	}

	// 温度取得
	if (mode == FFC_ADJUST_BLACK)
	{
		*pTemp = (float)ffcBlackInfo.sensorTemp;
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		*pTemp = (float)ffcWhiteInfo.sensorTemp;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Gain設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		gain				：FFC調整時のGain(倍率)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetAdjustGainX (int mode, float gain)
{
	int status = AVAL_STATUS_SUCCESS;
	float gainMax;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check gain Parameter
	gainMax = (float)DOG_GAIN_MAX + 0.01;
	if ((gain < DOG_GAIN_MIN) || (gain > gainMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		sprintf (gLogMsgBuff, "FFC Adjust Set Gain(%.2f) Parameter Error. (Min:%.2f / Max:%.2f)\n", gain, DOG_GAIN_MIN, DOG_GAIN_MAX);
		goto _DONE;
	}


	if (mode == FFC_ADJUST_BLACK)
	{
		ffcBlackInfo.mark = FFC_MARK_DATA;
		ffcBlackInfo.digitalGain = gain;
		OUTF (FIRM_DATA_FFC_ADJUST_BLACK_GAIN_ADRS, gain);
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		ffcWhiteInfo.mark = FFC_MARK_DATA;
		ffcWhiteInfo.digitalGain = gain;
		OUTF (FIRM_DATA_FFC_ADJUST_WHITE_GAIN_ADRS, gain);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Gain取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		pGain				：FFC調整時のGain(倍率)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetAdjustGainX (int mode, float *pGain)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// check pGain Parameter
	if (pGain == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Adjust Get Gain pGain NULL Parameter Error.\n");
		goto _DONE;
	}

	// 温度取得
	if (mode == FFC_ADJUST_BLACK)
	{
		*pGain = (float)ffcBlackInfo.digitalGain;
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		*pGain = (float)ffcWhiteInfo.digitalGain;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Bit設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		bit					：FFC調整時のBitを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetAdjustBit (int mode, int bit)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Bit Parameter
	if ((bit != 8) && (bit != 10) && (bit != 12) && (bit != 14))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Bit(%d) Parameter Error. (%d:8Bit / %d:10Bit / %d:12Bit / %d:14Bit)\n", bit, PIXEL_8BIT, PIXEL_10BIT, PIXEL_12BIT, PIXEL_14BIT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 温度取得
	if (mode == FFC_ADJUST_BLACK)
	{
		ffcBlackInfo.mark = FFC_MARK_DATA;
		ffcBlackInfo.bit = bit;
		OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_BIT_ADRS, bit);
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		ffcWhiteInfo.mark = FFC_MARK_DATA;
		ffcWhiteInfo.bit = bit;
		OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_BIT_ADRS, bit);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Bit取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC調整モード
//		pBit				：FFC調整時のBitを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetAdjustBit (int mode, int *pBit)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Mode Parameter
	if ((status = ffcAdjustCheckModeParam (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// check pBit Parameter
	if (pBit == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Adjust Get Bit pBit NULL Parameter Error.\n");
		goto _DONE;
	}

	// 温度取得
	if (mode == FFC_ADJUST_BLACK)
	{
		*pBit = ffcBlackInfo.bit;
	}
	else if (mode == FFC_ADJUST_WHITE)
	{
		*pBit = ffcWhiteInfo.bit;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Adjust Update(管理外メモリにあるデータを構造体にコピー)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcAdjustUpdate (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int tempL;
	float tempF;

	//----------------------------------------------------------------------
	// ターゲット
	//----------------------------------------------------------------------

	// 黒レベル
	tempL = IN32 (FIRM_DATA_FFC_ADJUST_BLACK_ADRS);
	ffcBlackInfo.target = tempL;

	// 白レベル
	tempL = IN32 (FIRM_DATA_FFC_ADJUST_WHITE_ADRS);
	ffcWhiteInfo.target = tempL;


	//----------------------------------------------------------------------
	// レート
	//----------------------------------------------------------------------

	// 黒レベル
	tempF = INF (FIRM_DATA_FFC_ADJUST_BLACK_RATE_ADRS);
	ffcBlackInfo.frameRate = (double)tempF;

	// 白レベル
	tempF = INF (FIRM_DATA_FFC_ADJUST_WHITE_RATE_ADRS);
	ffcWhiteInfo.frameRate = (double)tempF;


	//----------------------------------------------------------------------
	// 露光時間
	//----------------------------------------------------------------------

	// 黒レベル
	tempL = IN32 (FIRM_DATA_FFC_ADJUST_BLACK_EXP_ADRS);
	ffcBlackInfo.expTime = tempL;

	// 白レベル
	tempL = IN32 (FIRM_DATA_FFC_ADJUST_WHITE_EXP_ADRS);
	ffcWhiteInfo.expTime = tempL;


	//----------------------------------------------------------------------
	// 温度
	//----------------------------------------------------------------------

	// 黒レベル
	tempF = INF (FIRM_DATA_FFC_ADJUST_BLACK_TEMP_ADRS);
	ffcBlackInfo.sensorTemp = (double)tempF;

	// 白レベル
	tempF = INF (FIRM_DATA_FFC_ADJUST_WHITE_TEMP_ADRS);
	ffcWhiteInfo.sensorTemp = (double)tempF;


	//----------------------------------------------------------------------
	// Gainx
	//----------------------------------------------------------------------

	// 黒レベル
	tempF = INF (FIRM_DATA_FFC_ADJUST_BLACK_GAIN_ADRS);
	ffcBlackInfo.digitalGain = tempF;

	// 白レベル
	tempF = INF (FIRM_DATA_FFC_ADJUST_WHITE_GAIN_ADRS);
	ffcWhiteInfo.digitalGain = tempF;


	//----------------------------------------------------------------------
	// Bit
	//----------------------------------------------------------------------

	// 黒レベル
	tempL = IN32 (FIRM_DATA_FFC_ADJUST_BLACK_BIT_ADRS);
	ffcBlackInfo.bit = tempL;
	
	// 白レベル
	tempL = IN32 (FIRM_DATA_FFC_ADJUST_WHITE_BIT_ADRS);
	ffcWhiteInfo.bit = tempL;

	return (status);
}


//**********************************************************************************
// FFC領域へ書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		srcAdrs				：転送元アドレス
//		offset				：オフセット
//		size				：サイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetMemoryNormalFirst (void *srcAdrs, unsigned int offset, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
#if !defined (MODE_IPU_MULTI)
	int i;
	unsigned int data;
	int x, y;
	unsigned int memAdrs2;
#endif
	unsigned int memAdrs;
	int ffcNo = 0;

	// メモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcNo, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if !defined (MODE_IPU_MULTI)

	memAdrs2 = memAdrs;
	i=0;
	for (y=0; y<IMG_HEIGHT; y++)
	{
		for (x=0; x<IMG_WIDTH; x++, i++)
		{
			data = IN32 (((unsigned int)srcAdrs + i*4));
			OUT32 ((memAdrs2 + x*4), data);
		}

		memAdrs2 += FFC_WIDTH_DATA_ALIGH;
	}

#else // #if !defined (MODE_IPU_MULTI)

	ffcCopyBuffToExtMem (memAdrs, (unsigned int *)srcAdrs);

#endif // #if !defined (MODE_IPU_MULTI)

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC領域へ読み込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		desAdrs				：転送先アドレス
//		offset				：オフセット
//		size				：サイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetMemoryNormalFirst (void *desAdrs, unsigned int offset, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
#if !defined (MODE_IPU_MULTI)
	int i;
	unsigned int data;
	int x, y;
	unsigned int memAdrs2;
#endif
	unsigned int memAdrs;
	int ffcNo = 0;

	// メモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcNo, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if !defined (MODE_IPU_MULTI)

	memAdrs2 = memAdrs;
	i=0;
	for (y=0; y<IMG_HEIGHT; y++)
	{
		for (x=0; x<IMG_WIDTH; x++, i++)
		{
			data = IN32 ((memAdrs2 + x*4));
			OUT32 (((unsigned int)desAdrs + i*4), data);
		}

		memAdrs2 += FFC_WIDTH_DATA_ALIGH;
	}
#else // #if !defined (MODE_IPU_MULTI)

	ffcCopyExtMemToBuff (memAdrs, (unsigned int *)desAdrs);

#endif // #if !defined (MODE_IPU_MULTI)

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC領域へ書き込み（メイン）
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		srcAdrs				：転送元アドレス
//		offset				：オフセット
//		size				：サイズ
//		ffcCorMode			：FFC補正モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetMemoryNormal (void *srcAdrs, unsigned int offset, unsigned int size, int ffcCorMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int ffcSize;

	// srcAdrsのパラメータCheckはなし

	// Get FFC Size
	if (ffcCorMode == FFC_CORRECTION_MODE_FIRST)
	{
		ffcSize = FFC_ALL_SIZE_FIRST;
	}
	else if (ffcCorMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		ffcSize = FFC_DATA_SIZE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Set Memory Correction mode(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcCorMode, FFC_CORRECTION_MODE_MIN, FFC_CORRECTION_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset Parameter
	if ((offset < 0) || ((offset + size) > ffcSize))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Set Memory offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset + size, 0, ffcSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Parameter
	if ((size <= 0) || (size > ffcSize))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Set Memory size(%d) Parameter Error.(Min:%d / Max:%d)\n", size, 0, ffcSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if (ffcCorMode == FFC_CORRECTION_MODE_FIRST)
	{
		status = ffcSetMemoryNormalFirst (srcAdrs, offset, size);
	}
#if defined (MODE_FFC_SHADING_LINE)
	else if (ffcCorMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		status = ffcSetMemoryNormalFirst (srcAdrs, offset, size);
	}
#endif
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Set Memory Correction mode(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcCorMode, FFC_CORRECTION_MODE_MIN, FFC_CORRECTION_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC領域からの読み込み（メイン）
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		desAdrs				：転送先アドレス
//		offset				：オフセット
//		size				：サイズ
//		ffcCorMode			：FFC補正モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetMemoryNormal (void *desAdrs, unsigned int offset, unsigned int size, int ffcCorMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int ffcSize;

	// srcAdrsのパラメータCheckはなし

	// Get FFC Size
	if (ffcCorMode == FFC_CORRECTION_MODE_FIRST)
	{
		ffcSize = FFC_DATA_SIZE;
	}
	else if (ffcCorMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		ffcSize = FFC_DATA_SIZE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Get Memory Correction mode(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcCorMode, FFC_CORRECTION_MODE_MIN, FFC_CORRECTION_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset Parameter
	if ((offset < 0) || ((offset + size) > ffcSize))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Get Memory offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset + size, 0, ffcSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check size Parameter
	if ((size <= 0) || (size > ffcSize))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Get Memory size(%d) Parameter Error.(Min:%d / Max:%d)\n", size, 0, ffcSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if (ffcCorMode == FFC_CORRECTION_MODE_FIRST)
	{
		status = ffcGetMemoryNormalFirst (desAdrs, offset, size);
	}
#if defined (MODE_FFC_SHADING_LINE)
	else if (ffcCorMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		status = ffcGetMemoryNormalFirst (desAdrs, offset, size);
	}
#endif
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Get Memory Correction mode(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcCorMode, FFC_CORRECTION_MODE_MIN, FFC_CORRECTION_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータSWAP
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：アドレス
//		size				：サイズ
//		ffcCorMode			：FFC補正モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcDataSwap (unsigned int adrs, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	int count;
	unsigned int data32;
	unsigned int swapData32;
	unsigned int *ptr32;

	// Count
	count = size / 4;

	// Address
	ptr32 = (unsigned int *)adrs;

	for (i=0; i<count; i++, ptr32++)
	{
		data32 = *ptr32;
		swapData32 = SWAP_L(data32);
		*ptr32 = swapData32;
	}

	return (status);
}


//**********************************************************************************
//	FFC Bit計算
//----------------------------------------------------------------------------------
//	[ INPUT ]
///		pData				：FFC Bit計算を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetBitCalc (int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;

	// Check pMode Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Bit Calc pData NULL Parameter Error\n");
		goto _DONE;
	}

	// bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (bit == 8)
		*pData = 64;
	else if (bit == 10)
		*pData = 16;
	else if (bit == 12)
		*pData = 4;
	else if (bit == 14)
		*pData = 1;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Bit Calc Bit(%d) Parameter Error\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Bit計算
//----------------------------------------------------------------------------------
//	[ INPUT ]
///		bit					：Bit
///		pData				：FFC Bit計算を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetBitCalc2 (int bit, int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Bit Calc2 pData NULL Parameter Error\n");
		goto _DONE;
	}

	if (bit == 8)
		*pData = 64;
	else if (bit == 10)
		*pData = 16;
	else if (bit == 12)
		*pData = 4;
	else if (bit == 14)
		*pData = 1;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Bit Calc2 Bit(%d) Parameter Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


#if defined (MODE_SENSOR_XFLIP)

#if !defined (MODE_IPU_MULTI)
//**********************************************************************************
//	FFC Data XFlip
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=正転する/1=反転する
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcDataXFlip (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int x, y;
	unsigned int ffcMemAdrs;
	int width, widthL;
	int height;
	unsigned int *pBuff32 = NULL;
	unsigned int *pBuffTemp32 = NULL;
	int ffcNo = 0;

	// データFlipあり／なし
	if (ffcGetXFlipFlag () == MODE_DISABLE)
		goto _DONE;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC X Flip mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Width Max取得
	width = sensorWidth ();
	widthL = width * 4;

	// Height Max取得
	height = sensorHeight ();

	// メモリリクエスト
	if ((pBuff32 = (unsigned int *)malloc (widthL)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "FFC X Flip Malloc Error. size = 0x%x\n", width);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcNo, FFC_MEMORY_EXT, &ffcMemAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	for (y=0; y<height; y++)
	{
		// 1ラインバッファに読み込み(終端から)
		pBuffTemp32 = pBuff32;
		for (x=0; x<width; x++, pBuffTemp32++)
			*pBuffTemp32 = IN32 ((ffcMemAdrs + width * 4 - ((x+1) * 4)));

		// 反転
		pBuffTemp32 = pBuff32;
		for (x=0; x<width; x++, pBuffTemp32++)
			OUT32 ((ffcMemAdrs + x * 4), *pBuffTemp32);

		ffcMemAdrs += FFC_WIDTH_DATA_ALIGH;
	}

_DONE:
	if (pBuff32 != NULL)
		free (pBuff32);

	return (status);
}

#else // #if !defined (MODE_IPU_MULTI)

//**********************************************************************************
//	FFC Data XFlip
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=正転する/1=反転する
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcDataXFlip (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int x, y;
	unsigned int ffcMemAdrs;
	unsigned int ffcMemAdrsLeftSrc, ffcMemAdrsLeftDes;
	unsigned int ffcMemAdrsRightSrc, ffcMemAdrsRightDes;
	int width, widthL;
	int height;
	unsigned int *pBuffLeft = NULL;
	unsigned int *pBuffRight = NULL;
	unsigned int *pBuffLeft2 = NULL;
	unsigned int *pBuffRight2= NULL;
	unsigned int tempAdrs;
	int ffcNo = 0;

	// データFlipあり／なし
	if (ffcGetXFlipFlag () == MODE_DISABLE)
		goto _DONE;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC X Flip mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Width Max取得
	width = IMG_WIDTH / IPU_COUNT;
	widthL = width * 4;

	// Height Max取得
	height = sensorHeight ();

	// メモリリクエスト
	if ((pBuffLeft = (unsigned int *)malloc (widthL)) == NULL)
	{ 
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "FFC X Flip Malloc Error. size = 0x%x\n", width);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if ((pBuffRight = (unsigned int *)malloc (widthL)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "FFC X Flip Malloc Error. size = 0x%x\n", width);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcNo, FFC_MEMORY_EXT, &ffcMemAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 右左も入れ替えで左用データ
	ffcMemAdrsLeftSrc = ffcMemAdrs;
	ffcMemAdrsLeftDes = ffcMemAdrs + FFC_MEMORY_IPU_MULTI_INTERVAL;

	// 右左も入れ替えで右用データ
	ffcMemAdrsRightSrc = ffcMemAdrs + FFC_MEMORY_IPU_MULTI_INTERVAL;
	ffcMemAdrsRightDes = ffcMemAdrs;

	for (y=0; y<height; y++)
	{
		// 1ラインバッファに読み込み(終端から)
		pBuffLeft2 = pBuffLeft;
		pBuffRight2 = pBuffRight;
		for (x=0; x<width; x++, pBuffLeft2++, pBuffRight2++)
		{
			tempAdrs = ffcMemAdrsLeftSrc  + width * 4 - ((x+1) * 4);
			*pBuffLeft2  = IN32 (tempAdrs);

			tempAdrs = ffcMemAdrsRightSrc + width * 4 - ((x+1) * 4) + IMG_WIDTH_OFFSET * 4;
			*pBuffRight2 = IN32 (tempAdrs);
		}

		// 左右上下反転
		pBuffLeft2 = pBuffLeft;
		pBuffRight2 = pBuffRight;
		for (x=0; x<width; x++, pBuffLeft2++, pBuffRight2++)
		{
			tempAdrs = ffcMemAdrsLeftDes + IMG_WIDTH_OFFSET * 4 + x * 4;
			OUT32 (tempAdrs, *pBuffLeft2);

			tempAdrs = ffcMemAdrsRightDes + x * 4;
			OUT32 (tempAdrs, *pBuffRight2);
		}

		// 次のライン
		ffcMemAdrsLeftSrc += FFC_WIDTH_DATA_ALIGH;
		ffcMemAdrsRightSrc += FFC_WIDTH_DATA_ALIGH;

		ffcMemAdrsLeftDes += FFC_WIDTH_DATA_ALIGH;
		ffcMemAdrsRightDes += FFC_WIDTH_DATA_ALIGH;
	}

	// ipuの1個目の最後の16画素の追加。
	// ipuの2個目の最初の16画素の追加。
	if ((status = ffcSetMarginGridData()) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (pBuffLeft != NULL)
		free (pBuffLeft);

	if (pBuffRight != NULL)
		free (pBuffRight);

	return (status);
}
#endif // #if !defined (MODE_IPU_MULTI)
#endif // #if defined (MODE_SENSOR_XFLIP)


#if defined (MODE_SENSOR_XFLIP)
//**********************************************************************************
//	FFC X Flip Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=FFCデータをX Flipしない/1=FFCデータをX Flipする
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
void ffcSetXFlipFlag (int mode)
{
	OUT32 (FIRM_DATA_FLIP_FFC_MODE_ADRS, mode);
}


//**********************************************************************************
//	FFC X Flip Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		mode				：0=FFCデータをX Flipしない/1=FFCデータをX Flipする
//==================================================================================
int ffcGetXFlipFlag (void)
{
	int data32;
	data32 = (int)IN32 (FIRM_DATA_FLIP_FFC_MODE_ADRS);
	return (data32);
}
#endif


//**********************************************************************************
//	FFC Correction Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：FFC補正モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetCorMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check mode Parameter
	if ((mode < FFC_CORRECTION_MODE_MIN) && (mode > FFC_CORRECTION_MODE_MIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Correction Mode Parameter Error. (Min:%d / Max:%d)\n", mode, FFC_CORRECTION_MODE_MIN, FFC_CORRECTION_MODE_MIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Set FFC Correction Mode
	OUT32 (FIRM_DATA_FFC_CORECTION_MODE_ADRS, mode);

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Correction Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：FFC補正モードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetCorMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Correction Mode NULL Parameter Error\n");
		goto _DONE;
	}

	// Get FFC Correction Mode
	*pMode = IN32 (FIRM_DATA_FFC_CORECTION_MODE_ADRS);

_DONE:
	return (status);
}



#if defined (MODE_FRAMERATE_HIGH_SPEED)
//**********************************************************************************
//	FFC データの入れ替え(メイン)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		lineStart			：指定ライン数と0Line目の入れ替え
//		lineSize			：ライン数
//		ffcCorMode			：FFC補正モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcDataReplacement (int lineStart, int lineSize, int ffcCorMode)
{
	int status = AVAL_STATUS_SUCCESS;

	if (ffcCorMode == FFC_CORRECTION_MODE_FIRST)
	{
		if ((status = ffcDataReplacementFirst (lineStart, lineSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Replace(%d) Correction Mode  Error.(Min:%d / Max:%d)\n", ffcCorMode, FFC_CORRECTION_MODE_MIN, FFC_CORRECTION_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC データの入れ替え
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		lineStart			：指定ライン数と0Line目の入れ替え
//		lineSize			：ライン数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcDataReplacementFirst (int lineStart, int lineSize)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;
	int ffcNum = 0;
	unsigned int srcIndex, dstIndex;
	int widthMax;
	unsigned int tempSrc, tempDst;
	int x, y;
#if defined (MODE_IPU_MULTI)
	unsigned int memAdrs2;
	int ipu;
#endif

	// Check lineStart Parameter
	if ((lineStart < 0) || (lineStart > HeightMax()))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Replace Line Start(%d) Parameter Error. (Min:%d / Max:%d)\n", lineStart, HeightMin(), HeightMax());
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check lineSize Parameter
	if ((lineSize <= 0) || (lineSize > HeightMax()))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Replace Line Size(%d) Parameter Error. (Min:%d / Max:%d)\n", lineSize, 0, HeightMax());
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// FFCアドレス取得
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, (unsigned int *)&adrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_IPU_MULTI)

	widthMax = CAMERA_WIDTH_MAX / IPU_COUNT + IMG_WIDTH_IPU_MULTI_ADD_SIZE;
	
	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		// アドレス算出
		memAdrs2 = adrs + FFC_MEMORY_IPU_MULTI_INTERVAL * ipu;

		srcIndex = lineStart * FFC_WIDTH_DATA_ALIGH;
		srcIndex += memAdrs2;
		dstIndex = memAdrs2;

		for (y = 0; y<lineSize; y++)
		{	
			for (x = 0; x<widthMax; x++)
			{
				tempSrc = IN32 ((srcIndex + x * OFFSET_GAIN_UNIT_SIZE));
				tempDst = IN32 ((dstIndex + x * OFFSET_GAIN_UNIT_SIZE));

				OUT32 ((srcIndex + x * OFFSET_GAIN_UNIT_SIZE), tempDst);
				OUT32 ((dstIndex + x * OFFSET_GAIN_UNIT_SIZE), tempSrc);
			}

			srcIndex += FFC_WIDTH_DATA_ALIGH;
			dstIndex += FFC_WIDTH_DATA_ALIGH;
		}
	}	
	
#else // #if defined (MODE_IPU_MULTI)

	// アドレス算出
	#if defined (MODE_FFC_DATA_ALIGN_ADJUST)
	srcIndex = lineStart * FFC_WIDTH_DATA_ALIGH;
	#else
	srcIndex = lineStart * WidthMax();
	srcIndex *= OFFSET_GAIN_UNIT_SIZE;
	#endif

	srcIndex += adrs;
	dstIndex = adrs;

	widthMax = sensorWidth ();

	for (y = 0; y<lineSize; y++)
	{	
		for (x = 0; x<widthMax; x++)
		{
			tempSrc = IN32 ((srcIndex + x * OFFSET_GAIN_UNIT_SIZE));
			tempDst = IN32 ((dstIndex + x * OFFSET_GAIN_UNIT_SIZE));

			OUT32 ((srcIndex + x * OFFSET_GAIN_UNIT_SIZE), tempDst);
			OUT32 ((dstIndex + x * OFFSET_GAIN_UNIT_SIZE), tempSrc);
		}

		#if defined (MODE_FFC_DATA_ALIGN_ADJUST)
		srcIndex += FFC_WIDTH_DATA_ALIGH;
		dstIndex += FFC_WIDTH_DATA_ALIGH;
		#else
		srcIndex += WidthMax() * OFFSET_GAIN_UNIT_SIZE;
		dstIndex += WidthMax() * OFFSET_GAIN_UNIT_SIZE;
		#endif
	}
#endif // #if defined (MODE_IPU_MULTI)
	
_DONE:
	return (status);
}


//**********************************************************************************
//	FFC データの入れ替え(メイン)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		lineStart			：指定ライン数と0Line目の入れ替え
//		lineSize			：ライン数
//		ffcCorMode			：FFC補正モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetReplacementLine (int *pLineStart, int lineSize, int ffcCorMode)
{
	int status = AVAL_STATUS_SUCCESS;

	if (ffcCorMode == FFC_CORRECTION_MODE_FIRST)
	{
		if ((status = ffcGetReplacementLineFirst (pLineStart, lineSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Data Replace Line (%d) Correction Mode  Error.(Min:%d / Max:%d)\n", ffcCorMode, FFC_CORRECTION_MODE_MIN, FFC_CORRECTION_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータの入れ替え時のスタートLine取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pLineStart			：開始ライン数の取得
//		lineSize			：ライン数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcGetReplacementLineFirst (int *pLineStart, int lineSize)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pLineStart Parameter
	if (pLineStart == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Get Replace Line pLineStart NULL Parameter Error.\n");
		goto _DONE;
	}

	if (lineSize == 2)
	{
		*pLineStart = FRAME_RATE_HIGH_SPEED_MODE_START_LINE2;
	}
	else if (lineSize == 4)
	{
		*pLineStart = FRAME_RATE_HIGH_SPEED_MODE_START_LINE4;
	}
	else if (lineSize == 6)
	{
		*pLineStart = FRAME_RATE_HIGH_SPEED_MODE_START_LINE6;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Get Replace Line Size(%d) Parameter Error. (Min:%d / Max:%d)\n", lineSize, HeightMin(), HeightMax());
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	
_DONE:
	return (status);
}

#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)


#if defined (MODE_IPU_MULTI)
//**********************************************************************************
//	FFCデータCopy(バッファからFFCメモリにCopy)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		memAdrs				：FFCデータ格納アドレス
//		pBuffer				：Copy元アドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcCopyBuffToExtMem (unsigned long memAdrs, unsigned int *pBuffer)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *ptrL;
	unsigned long memAdrs2;
	int x, y;
	unsigned int data;
	int ipu;

	ptrL = (unsigned int *)pBuffer;
	memAdrs2 = memAdrs;

	for (y=0; y<IMG_HEIGHT; y++)
	{
		for (ipu=0; ipu < IPU_COUNT; ipu++)
		{
			#if defined (MODE_FFC_DATA_ALIGN_ADJUST)
			memAdrs2 = memAdrs + (y * FFC_WIDTH_DATA_ALIGH) + (FFC_MEMORY_IPU_MULTI_INTERVAL * ipu);
			#endif

			if (ipu != 0)
				memAdrs2 += (IMG_WIDTH_OFFSET * 4);

			for (x=0; x<(IMG_WIDTH / IPU_COUNT); x++, ptrL++)
			{
				data = *ptrL;
				OUT32 ((memAdrs2 + x * 4), data);
			}

			#if !defined (MODE_FFC_DATA_ALIGN_ADJUST)
			memAdrs2 += IMG_WIDTH * 4;
			#endif
		}
	}

	return (status);
}


//**********************************************************************************
//	FFCデータCopy(FFCメモリからバッファにCopy)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		memAdrs				：FFCデータ格納アドレス
//		pBuffer				：Copy元アドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcCopyExtMemToBuff (unsigned long memAdrs, unsigned int *pBuffer)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *ptrL;
	unsigned long memAdrs2;
	int x, y;
	unsigned int data;
	int ipu;

	ptrL = (unsigned int *)pBuffer;
	memAdrs2 = memAdrs;

	for (y=0; y<IMG_HEIGHT; y++)
	{
		for (ipu=0; ipu < IPU_COUNT; ipu++)
		{
			#if defined (MODE_FFC_DATA_ALIGN_ADJUST)
			memAdrs2 = memAdrs + (y * FFC_WIDTH_DATA_ALIGH) + FFC_MEMORY_IPU_MULTI_INTERVAL * ipu;
			#endif

			if (ipu != 0)
				memAdrs2 += (IMG_WIDTH_OFFSET * 4);

			for (x=0; x<(IMG_WIDTH / IPU_COUNT); x++, ptrL++)
			{
				data = IN32 ((memAdrs2 + x * 4));
				*ptrL = data;
			}

			#if !defined (MODE_FFC_DATA_ALIGN_ADJUST)
			memAdrs2 += IMG_WIDTH * 4;
			#endif
		}
	}

	return (status);
}

#endif	// #if defined (MODE_IPU_MULTI)


#if defined (MODE_IPU_MULTI)
//**********************************************************************************
// DPCマージン座標の追加
// ipuの1個目の最後の16画素の追加。
// ipuの2個目の最初の16画素の追加。
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcSetMarginGridData (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int ipu;
	int x, y;
	int ffcNum = 0;
	unsigned int memAdrs;
	unsigned int srcAdrs, desAdrs;
	unsigned int data8[3];

	// メモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcNum, FFC_MEMORY_EXT, (unsigned int *)&memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		for (y = 0; y < IMG_HEIGHT; y++)
		{
			if (ipu == 0)
			{
				// ipu1の最初の16画素
				srcAdrs =  memAdrs;
				srcAdrs += (y * FFC_WIDTH_DATA_ALIGH);
				srcAdrs += (IMG_WIDTH_OFFSET * 3);
				srcAdrs += (FFC_MEMORY_IPU_MULTI_INTERVAL * (ipu + 1));

				// ipu0の最後の16画素
				desAdrs =  memAdrs;
				desAdrs += ((y * FFC_WIDTH_DATA_ALIGH) + (IMG_WIDTH_IPU_SIZE * 3));
			}
			else
			{
				// ipu0の最後の16画素
				srcAdrs =  memAdrs;
				srcAdrs += ((y * FFC_WIDTH_DATA_ALIGH) + (IMG_WIDTH_IPU_SIZE * 3));
				srcAdrs -= (IMG_WIDTH_OFFSET * 3);

				// ipu1の最初の画素
				desAdrs =  memAdrs;
				desAdrs += (y * FFC_WIDTH_DATA_ALIGH);
				desAdrs += (FFC_MEMORY_IPU_MULTI_INTERVAL * ipu);
			}

			for (x = 0; x < IMG_WIDTH_OFFSET; x++)
			{
				data8[0] = IN8 (srcAdrs);
				srcAdrs++;
				data8[1] = IN8 (srcAdrs);
				srcAdrs++;
				data8[2] = IN8 (srcAdrs);
				srcAdrs++;

				OUT8 (desAdrs, data8[0]);
				desAdrs++;

				OUT8 (desAdrs, data8[1]);
				desAdrs++;

				OUT8 (desAdrs, data8[2]);
				desAdrs++;
			}
		}
	}

_DONE:
	return (status);
}
#endif

