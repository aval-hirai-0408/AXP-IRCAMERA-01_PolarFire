//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// ffcAdjust.c - FFC Adjust Main Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"

//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Acquisition
//----------------------------------------------------------------------------------
double gFrameRateSave = 0;						// フレームレートオリジナル設定値
unsigned int gExpTimeSave = 0;					// 露光時間オリジナル設定値
int gAcquisitionMode = -1;						// Acquisition Mode
int gBitSave = 0;								// Bit幅
unsigned int gFfcSave = 0;						// FFCレジスタ設定
unsigned int gDpcSave = 0;						// DPCレジスタ設定
double gSensortTemp = -1;						// センサターゲット温度
int ffcAdjustAllFlag = 0;


//----------------------------------------------------------------------------------
// Digital Offset Gain
//----------------------------------------------------------------------------------
unsigned int gBlacktarget = 0xffffffff;			// 黒レベル目標値保存用
int gDogOffset1 = -1;							// Digital Offset1保存用
int gDogOffset2 = -1;							// Digital Offset2保存用
float gDogGain = 0;								// Digital Gain保存用
unsigned int gDogGainReg = 0;					// Digital Gain保存用(レジスタ値)


//----------------------------------------------------------------------------------
// FFC情報保存用
//----------------------------------------------------------------------------------
unsigned int gRateTimeInfo = 0xffffffff;		// フレームレート保存用
double gFrameRateBlackInfo;
unsigned int gExpTimeBlackInfo;
float gDigitalGainInfo;


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern FFCINFO pImageFfcBlackInfo[];			// FFC黒レベル情報保存用
extern FFCINFO pImageFfcWhiteInfo[];			// FFC白レベル情報保存用
extern FFCINFO ffcBlackInfo;					// FFC黒レベル情報保存用
extern FFCINFO ffcWhiteInfo;					// FFC白レベル情報保存用
extern double *pImageBlackAve;					// 黒画像平均値格納用
extern double *pImageWhiteAve;					// 白画像平均値格納用
extern unsigned short *pImageOffset;			// オフセット格納用
extern int *pImageGain;							// ゲイン格納用
extern unsigned short *pImageSrc;				// 検出用入力画像の先頭を指すポインタ配列


//**********************************************************************************
// FFC黒画像取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcParam			：FFC情報パラメータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcBlack (FFC_PARAM ffcParam)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int index_img;
	unsigned int black_target = 0;
	unsigned int memAdrs;
	unsigned int blackMax;
	double sensorTempBlackInfo;
	unsigned int ffcNoORG;
	int consoleMode = CONSOLE_MODE_OFF;
	int ffcdpcmode;
#if defined (MODE_FFC_SHADING_LINE)
	int mvMode;
	int corMode;
#endif
#if !defined (MODE_FFC_BIT_CALC)
	int saveBit = -1;
#endif
#ifdef MODE_PELTIER
	int peltierState = MODE_ENABLE;
#endif
#if defined(MODE_BINNING)
	int binningMode;
#endif
	int startFlag = 0;

	// Check ffcNum Parameter
	if ((ffcParam.ffcNo < FFC_NUMBER_MIN) || (ffcParam.ffcNo > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Black ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcParam.ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check memType Parameter
	if ((ffcParam.memType != FFC_MEMORY_EXT) && (ffcParam.memType != FFC_MEMORY_INT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Black memType(%d) Parameter Error.(EXT:%d / INT:%d)\n", ffcParam.memType, FFC_MEMORY_EXT, FFC_MEMORY_INT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//----------------------------------------------------------------------
	// AcquisitionStop
	//----------------------------------------------------------------------
	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// Peltier実装状態確認
	//----------------------------------------------------------------------
#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	if ((status = peltierGetMountState(&peltierState) != AVAL_STATUS_SUCCESS))
		goto _DONE;
#endif

#if defined (MODE_FFC_SHADING_LINE)
	// 補正モード取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 移動平均モードの設定
	if (corMode == FFC_CORRECTION_MODE_FIRST)
		mvMode = MODE_DISABLE;
	else
		mvMode = MODE_ENABLE;

	if ((status = ffcSetMoveAverage (mvMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//----------------------------------------------------------------------
	// FFC/DPCレジスタ保存
	//----------------------------------------------------------------------

	// FFCレジスタ保存
	gFfcSave = IN32 (FPGA_FFC_CTRL_ADRS);

	// DPCレジスタ保存
	dpcGetEnableMode ((int *)&gDpcSave);

	//----------------------------------------------------------------------
	// FFC & DPC Disable
	//----------------------------------------------------------------------
	OUT32 (FPGA_FFC_CTRL_ADRS, 0);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_FFC2_CTRL_ADRS, 0);
#endif

	// 移動平均モードの場合DPCは無効にしない
#if defined (MODE_FFC_SHADING_LINE)
	if (mvMode == MODE_ENABLE)
		goto _DPC_SKIP;
#endif

	// DPC Disable
	if ((status = dpcSetEnableMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_FFC_SHADING_LINE)
_DPC_SKIP:
#endif

	//----------------------------------------------------------------------
	// Auto Bright Mode 無効設定
	//----------------------------------------------------------------------
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	if ((status = autoBrightSetExposureMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = autoBrightSetGainMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//----------------------------------------------------------------------
	// Binning 無効設定
	//----------------------------------------------------------------------
#if defined(MODE_BINNING)
	if((status = aoiSetBinningX (BINNING1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if((status = aoiSetBinningY (BINNING1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//----------------------------------------------------------------------
	// Camera Parameter Save
	//----------------------------------------------------------------------
	if ((status = acquisitionParamSave ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// センサターゲット温度保存
	//----------------------------------------------------------------------
#ifdef MODE_PELTIER
	if(peltierState == MODE_ENABLE)
	{
		if ((status = tempParamSave ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // MODE_PELTIER

	//----------------------------------------------------------------------
	// Digital Offset Gain保存
	//----------------------------------------------------------------------
	if ((status = dogParamSave ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// 黒レベル設定
	//   ・オフセット1：0
	//   ・オフセット2：0
	//   ・ゲイン：1.0
    //   ・BlackTarget：0
	//----------------------------------------------------------------------

	// 黒レベル設定
	ffcSetBlackTarget (0);

	// 黒レベル調整時はゲイン、オフセットデフォルト
	if ((status = dogSetOffsetGain (0, 0, 0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// ROI Area All設定
	//----------------------------------------------------------------------
	if ((status = roiSetDmaFull ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// Bit幅設定
	//----------------------------------------------------------------------
#if defined (MODE_FFC_BIT_CALC)
	if ((status = aoiGetBitWidth (&ffcParam.bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	if ((status = aoiGetBitWidth (&saveBit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	ffcParam.bit = MODE_FFC_BIT;
#endif

	//----------------------------------------------------------------------
	// Bit設定
	//----------------------------------------------------------------------
	if ((status = aoiSetBitWidth (ffcParam.bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// フレームレート
	//----------------------------------------------------------------------
	if ((status = acquisitionSetFrameRate (ffcParam.frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// 露光時間
	//----------------------------------------------------------------------
	if ((status = acquisitionSetExposure (ffcParam.exposure)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// センサ温度設定
	//----------------------------------------------------------------------
#ifdef MODE_PELTIER
	if (peltierState == MODE_ENABLE)
	{
		if ((status = cmdSensorTempBatch (ffcParam.sensorTemp)) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	//----------------------------------------------------------------------
	// フレームレート/露光時間を保存用に取得
	//----------------------------------------------------------------------
	if ((status = acquisitionGetFrameRate (&gFrameRateBlackInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = acquisitionGetExposure (&gExpTimeBlackInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// ゲインを保存用に取得
	//----------------------------------------------------------------------
#if defined (MODE_FFC_INFO_GAIN)
	if ((status = digitalGetGainX (&gDigitalGainInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//----------------------------------------------------------------------
	// 黒レベル最大値取得
	//----------------------------------------------------------------------
	if (ffcParam.bit == 8)
		blackMax = FFC_BLACK_LEVEL_BIT8_MAX;
	else if (ffcParam.bit == 10)
		blackMax = FFC_BLACK_LEVEL_BIT10_MAX;
	else if (ffcParam.bit == 12)
		blackMax = FFC_BLACK_LEVEL_BIT12_MAX;
	else if (ffcParam.bit == 14)
		blackMax = FFC_BLACK_LEVEL_BIT14_MAX;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Black Bit(%d) Parameter Error. (Min:%dbit / Max:%dbit)\n", ffcParam.bit, PIXEL_BIT_MIN, PIXEL_BIT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 黒レベル設定値Check
	if (ffcParam.target > blackMax)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Black Target(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcParam.target, FFC_BLACK_LEVEL_MIN, blackMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 黒レベル設定
	black_target = ffcParam.target;

	//----------------------------------------------------------------------
	// センサ温度取得
	//----------------------------------------------------------------------
	peltierGetSensorTemp (&sensorTempBlackInfo);

	//----------------------------------------------------------------------
	// 画像取り込み開始
	//----------------------------------------------------------------------

	// 連続モード取り込み
	acquisitionSetMode (ACQUISITION_MODE_CONTIN);

	// 取り込み開始
	acquisitionStart ();

	// Delay
	msDelay (100);

	//----------------------------------------------------------------------
	// 画像平均値クリア
	//----------------------------------------------------------------------
	calc_average_clear (pImageBlackAve);

	// Console Mode取得
	consoleGetMode (&consoleMode);

	//----------------------------------------------------------------------
	// 画像転送
	//----------------------------------------------------------------------
	for (index_img=0; index_img<NUM_FFC_IMG_FOR_DETECT; index_img++)
	{
		DEBUG_PRINT ("Capture Start..%d", index_img);
		fflush (stdout);

		// コマンドラインbackspace
		if (consoleMode == CONSOLE_MODE_ON)
			cmdBackspaceLine ();

		// DMA転送
		if ((status = dmaStart ((unsigned char *)pImageSrc)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 平均値計算
		if ((status = calc_average2 (pImageSrc, pImageBlackAve)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	DEBUG_PRINT ("\n");

	//----------------------------------------------------------------------
	// 平均値計算
	//----------------------------------------------------------------------
	calc_average_divider (pImageBlackAve, NUM_FFC_IMG_FOR_DETECT);

	//----------------------------------------------------------------------
	// FFCオフセット計算
	//----------------------------------------------------------------------
	DEBUG_PRINT ("FFC Offset Check..\n");
	if ((status = ffc_offset (pImageBlackAve, (unsigned short *)pImageOffset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// データ並び替え
	//----------------------------------------------------------------------
	DEBUG_PRINT ("FFC Offset Data Memory Write..\n");
	
	// メモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcParam.ffcNo, ffcParam.memType, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// 外部メモリアドレス取得(1度外部メモリにデータを格納)
	//----------------------------------------------------------------------
 	if (ffcParam.memType == FFC_MEMORY_INT)
	{
		if ((status = ffcGetMemAdrs (ffcParam.ffcNo, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// オフセットデータ設定
	ffcSetMemoryOffsetGain (ffcParam.ffcNo, memAdrs, (unsigned short *)pImageOffset, (int *)NULL);

	//----------------------------------------------------------------------
	// 外部メモリアドレスから内部メモリアドレスにコピー(データを保持するために)
	//----------------------------------------------------------------------
 	if (ffcParam.memType == FFC_MEMORY_INT)
	{
		unsigned int intMemAdrs;

		// 内部メモリアドレス取得
		if ((status = ffcGetMemAdrs (ffcParam.ffcNo, ffcParam.memType, &intMemAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// データコピー
		if ((status = ffcGetExtMemory (memAdrs, intMemAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	}

	//----------------------------------------------------------------------
	// 白レベル用に設定
	//   ・オフセット1：0 - target
	//   ・オフセット2：0 + target
    //   ・BlackTarget：target
	//----------------------------------------------------------------------
#if defined (MODE_FFC_BIT_CALC)
	int calc;

	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 黒レベル目標値設定
	black_target *= calc;
	ffcSetBlackTarget (black_target);
#else
	// 黒レベル目標値設定
	ffcSetBlackTarget (black_target);
#endif

	gBlacktarget = black_target;

	// Black Targetを新しい値に設定した上での再設定
	if ((status = dogSetOffsetGain (0, 0, gDogGain)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Digital Gain設定（レジスタ値）
	if ((status = digitalSetGainReg (gDogGainReg)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// FFC黒レベル情報保存
	//----------------------------------------------------------------------
	if (ffcAdjustAllFlag == 0)
	{
		ffcBlackInfo.frameRate = gFrameRateBlackInfo;
		ffcBlackInfo.expTime = gExpTimeBlackInfo;
		ffcBlackInfo.target = black_target;
		ffcBlackInfo.sensorTemp = sensorTempBlackInfo;
		ffcBlackInfo.mark = FFC_MARK_DATA;
		ffcBlackInfo.digitalGain = gDigitalGainInfo;
		ffcBlackInfo.bit = ffcParam.bit;
	}
	else
	{
		pImageFfcBlackInfo[ffcParam.ffcNo].frameRate = gFrameRateBlackInfo;
		pImageFfcBlackInfo[ffcParam.ffcNo].expTime = gExpTimeBlackInfo;
		pImageFfcBlackInfo[ffcParam.ffcNo].target = black_target;
		pImageFfcBlackInfo[ffcParam.ffcNo].sensorTemp = sensorTempBlackInfo;
		pImageFfcBlackInfo[ffcParam.ffcNo].mark = FFC_MARK_DATA;
		pImageFfcBlackInfo[ffcParam.ffcNo].digitalGain = gDigitalGainInfo;
		pImageFfcBlackInfo[ffcParam.ffcNo].bit = ffcParam.bit;
	}

_DONE:
	//----------------------------------------------------------------------
	// FFC設定（白レベルで調整できるように、黒レベルで測定した状態を維持）※元に戻さない
	//----------------------------------------------------------------------
	if (status == AVAL_STATUS_SUCCESS)
	{
		// メモリアドレス取得
		ffcGetMemAdrs (ffcParam.ffcNo, ffcParam.memType, &memAdrs);

		// キャッシュFlash
		cacheFlush ();

		// FFCパラメータ初期化 & Enable
		ffcRegInit (memAdrs, FFC_DATA_SIZE, (/*FPGA_FFC_CTRL_GAIN_ENABLE_BIT |*/ FPGA_FFC_CTRL_OFFSET_ENABLE_BIT), FPGA_FFC_CTRL_SELECT_FIRST);

		// FFC番号保存(メモリ)
		ffcSetLoadNum (ffcParam.ffcNo);

#if defined(MODE_BINNING)
		//ビニングオフセット設定
		aoiSetBinningOffsetBase (black_target);
#endif
	}
	else
	{
		//----------------------------------------------------------------------
		// Digital Offset Gain書き戻し（※エラーなので元に戻す）
		//----------------------------------------------------------------------
		dogParamRestore ();

		//----------------------------------------------------------------------
		// FFC書き戻し（※エラーなので元に戻す）
		//----------------------------------------------------------------------
		ffcGetLoadNum ((int *)&ffcNoORG);
		ffcLoad (ffcNoORG);

		if (startFlag == 1)
			acquisitionStart ();
	}

	//----------------------------------------------------------------------
	// ROI Area Restore設定
	//----------------------------------------------------------------------
	roiSetDmaResutore ();

	//----------------------------------------------------------------------
	// Bit Restore設定
	//----------------------------------------------------------------------
#if !defined (MODE_FFC_BIT_CALC)
	if (saveBit != -1)
		aoiSetBitWidth (saveBit);
#endif

	//----------------------------------------------------------------------
	// DPCレジスタ設定
	//----------------------------------------------------------------------
	dpcSetEnableMode (gDpcSave);

	return (status);
}


//**********************************************************************************
// FFC白画像取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcParam			：FFC情報パラメータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcWhite (FFC_PARAM ffcParam)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int index_img;
	unsigned int memAdrs;
	unsigned int whiteMax;
	unsigned int blackTarget;
	unsigned int expTimeWhiteInfo;
	double sensorTempWhiteInfo;
	double frameRateWhiteInfo;
	int consoleMode = CONSOLE_MODE_OFF;
	int ffcdpcmode;
#if defined (MODE_FFC_BIT_CALC)
	int calc;
#endif
#if defined (MODE_FFC_SHADING_LINE)
	double averageMax;
	int mvMode;
#endif
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	int gcMode;
#endif
#if defined (MODE_FFC_SHADING_LINE)
	int corMode;
#endif
#if !defined (MODE_FFC_BIT_CALC)
	int saveBit = -1;
#endif
#if defined(MODE_PELTIER)
	int peltierState = MODE_ENABLE;
#endif
#if defined(MODE_BINNING)
	int binningMode;
#endif
	int startFlag = 0;

	// Check ffcNum Parameter
	if ((ffcParam.ffcNo < FFC_NUMBER_MIN) || (ffcParam.ffcNo > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC White ffcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcParam.ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if 0 //@@@1
	// Check mode Parameter
	if ((ffcParam.mode < MODE_FFC_MIN) || (ffcParam.mode > MODE_FFC_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC White mode(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcParam.mode, MODE_FFC_MIN, MODE_FFC_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
#endif //@@@1
	
	// Check memType Parameter
	if ((ffcParam.memType != FFC_MEMORY_EXT) && (ffcParam.memType != FFC_MEMORY_INT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC White memType(%d) Parameter Error.(EXT:%d / INT:%d)\n", ffcParam.memType, FFC_MEMORY_EXT, FFC_MEMORY_INT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}


	//----------------------------------------------------------------------
	// AcquisitionStop
	//----------------------------------------------------------------------
	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// Peltier実装状態確認
	//----------------------------------------------------------------------
#if defined (MODE_PELTIER_MOUNTING_SWITCH)
	if ((status = peltierGetMountState(&peltierState) != AVAL_STATUS_SUCCESS))
		goto _DONE;
#endif

	//----------------------------------------------------------------------
	// 移動平均モードの取得
	//----------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	// 補正モード取得
	if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 移動平均モードの設定
	if (corMode == FFC_CORRECTION_MODE_FIRST)
		mvMode = MODE_DISABLE;
	else
		mvMode = MODE_ENABLE;

	if ((status = ffcSetMoveAverage (mvMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//----------------------------------------------------------------------
	// FFC/DPCレジスタ保存
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------
	// FFC & DPC Disable
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------
	// 移動平均モードの場合DPCは無効にしない
	//----------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	if (mvMode == MODE_ENABLE)
		goto _DPC_SKIP;
#endif

	//----------------------------------------------------------------------
	// DPC Disable
	//----------------------------------------------------------------------
	if ((status = dpcSetEnableMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_FFC_SHADING_LINE)
_DPC_SKIP:
#endif

	//----------------------------------------------------------------------
	// Auto Bright Mode 無効設定
	//----------------------------------------------------------------------
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	if ((status = autoBrightSetExposureMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = autoBrightSetGainMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//----------------------------------------------------------------------
	// Binning 無効設定
	//----------------------------------------------------------------------
#if defined(MODE_BINNING)
	if((status = aoiSetBinningX (BINNING1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if((status = aoiSetBinningY (BINNING1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//----------------------------------------------------------------------
	// LUT 無効設定
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------
	// FFC設定
	//----------------------------------------------------------------------

	// FFCレジスタ設定(オフセットのみ有効)
	OUT32 (FPGA_FFC_CTRL_ADRS, FPGA_FFC_CTRL_OFFSET_ENABLE_BIT);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_FFC2_CTRL_ADRS, FPGA_FFC_CTRL_OFFSET_ENABLE_BIT);
#endif

	//----------------------------------------------------------------------
	// Bit幅設定
	//----------------------------------------------------------------------
#if defined (MODE_FFC_BIT_CALC)
	if ((status = aoiGetBitWidth (&ffcParam.bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	if ((status = aoiGetBitWidth (&saveBit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	ffcParam.bit = MODE_FFC_BIT;
#endif

	//----------------------------------------------------------------------
	// Digital Offset Gain保存
	//----------------------------------------------------------------------
	if ((status = dogParamSave ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// 白レベル用に設定
	//   ・オフセット1：0 - target
	//   ・オフセット2：0 + target
    //   ・BlackTarget：target
	//----------------------------------------------------------------------

	// 黒レベル目標値をレジスタから取得
	ffcGetBlackTarget (&blackTarget);	// 黒レベルは設定されている事前提

#if defined (MODE_FFC_BIT_CALC)
	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	blackTarget /= calc;
#endif

	// オフセットは0に変更
	if ((status = dogSetOffsetGain (0, 0, gDogGain)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Digital Gain設定（レジスタ値）
	if ((status = digitalSetGainReg (gDogGainReg)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// ROI Area All設定
	//----------------------------------------------------------------------
	if ((status = roiSetDmaFull ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// Camera Parameter Save
	//----------------------------------------------------------------------
	if ((status = acquisitionParamSave ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// センサターゲット温度保存
	//----------------------------------------------------------------------
#ifdef MODE_PELTIER
	if(peltierState == MODE_ENABLE)
	{
		if ((status = tempParamSave ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	//----------------------------------------------------------------------
	// Bit設定
	//----------------------------------------------------------------------
	if ((status = aoiSetBitWidth (ffcParam.bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// フレームレート
	//----------------------------------------------------------------------
	if ((status = acquisitionSetFrameRate (ffcParam.frameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// 露光時間
	//----------------------------------------------------------------------
	if ((status = acquisitionSetExposure (ffcParam.exposure)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// センサ温度設定
	//----------------------------------------------------------------------
#ifdef MODE_PELTIER
	if(peltierState == MODE_ENABLE)
	{
		if ((status = cmdSensorTempBatch (ffcParam.sensorTemp)) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	//----------------------------------------------------------------------
	// フレームレート/露光時間を保存用に取得
	//----------------------------------------------------------------------
	if ((status = acquisitionGetFrameRate (&frameRateWhiteInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = acquisitionGetExposure (&expTimeWhiteInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// ゲインを保存用に取得
	//----------------------------------------------------------------------
#if defined (MODE_FFC_INFO_GAIN)
	if ((status = digitalGetGainX (&gDigitalGainInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//----------------------------------------------------------------------
	// 白レベル最大値取得
	//----------------------------------------------------------------------
	if (ffcParam.bit == 8)
		whiteMax = FFC_WHITE_LEVEL_BIT8_MAX;
	else if (ffcParam.bit == 10)
		whiteMax = FFC_WHITE_LEVEL_BIT10_MAX;
	else if (ffcParam.bit == 12)
		whiteMax = FFC_WHITE_LEVEL_BIT12_MAX;
	else if (ffcParam.bit == 14)
		whiteMax = FFC_WHITE_LEVEL_BIT14_MAX;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC White Bit(%d) Parameter Error. (Min:%dbit / Max:%dbit)\n", ffcParam.bit, PIXEL_BIT_MIN, PIXEL_BIT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//----------------------------------------------------------------------
	// 移動平均モードは目標値を取得しない
	//----------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	if (mvMode == MODE_ENABLE)
		goto _NEXT;
#endif

	//----------------------------------------------------------------------
	// 白レベル目標値設定
	//----------------------------------------------------------------------
	if ((ffcParam.target <= blackTarget) || (ffcParam.target > whiteMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC White white_target(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcParam.target, blackTarget+1, whiteMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if defined (MODE_FFC_SHADING_LINE)
_NEXT:
#endif

#if defined (MODE_FFC_BIT_CALC)
	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	ffcParam.target *= calc;
	blackTarget *= calc;
#endif

	//----------------------------------------------------------------------
	// センサ温度取得
	//----------------------------------------------------------------------
	peltierGetSensorTemp (&sensorTempWhiteInfo);

	//----------------------------------------------------------------------
	// 画像取り込み開始
	//----------------------------------------------------------------------

	// 連続モード取り込み
	acquisitionSetMode (ACQUISITION_MODE_CONTIN);

	// 取り込み開始
	acquisitionStart ();

	// Delay
	msDelay (100);

	//----------------------------------------------------------------------
	// 画像平均値クリア
	//----------------------------------------------------------------------
	calc_average_clear (pImageWhiteAve);

	// Console Mode取得
	consoleGetMode (&consoleMode);

	//----------------------------------------------------------------------
	// 画像転送
	//----------------------------------------------------------------------
	for (index_img=0; index_img<NUM_FFC_IMG_FOR_DETECT; index_img++)
	{
		DEBUG_PRINT ("Capture Start..%d", index_img);
		fflush (stdout);

		// コマンドラインbackspace
		if (consoleMode == CONSOLE_MODE_ON)
			cmdBackspaceLine ();

		// DMA転送
		if ((status = dmaStart ((unsigned char *)pImageSrc)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 平均値計算
		if ((status = calc_average2 (pImageSrc, pImageWhiteAve)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	DEBUG_PRINT ("\n");

	//----------------------------------------------------------------------
	// 平均値計算
	//----------------------------------------------------------------------
	calc_average_divider (pImageWhiteAve, NUM_FFC_IMG_FOR_DETECT);

	//----------------------------------------------------------------------
	// FFCゲイン計算
	//----------------------------------------------------------------------
	DEBUG_PRINT ("FFC Gain Check..\n");

#if !defined (MODE_FFC_SHADING_LINE)
	
	if ((status = ffc_gain (pImageWhiteAve, pImageOffset, blackTarget, ffcParam.target, (int *)pImageGain, ffcParam.bit)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

#else

	if (mvMode == MODE_DISABLE)
	{
		//----------------------------------------------------------------------
		// ゲイン計算
		//----------------------------------------------------------------------
		if ((status = ffc_gain (pImageWhiteAve, pImageOffset, blackTarget, ffcParam.target, (int *)pImageGain, ffcParam.bit)) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		//----------------------------------------------------------------------
		// 移動平均
		//----------------------------------------------------------------------
		if ((status = ffc_move_average (pImageWhiteAve, pImageBlackAve, &averageMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//----------------------------------------------------------------------
		// ゲインデータ算出
		//----------------------------------------------------------------------
		if ((status = ffc_move_average_gain (pImageWhiteAve, pImageBlackAve, (int *)pImageGain, blackTarget, averageMax, ffcParam.bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if !defined (MODE_FFC_SHADING_LINE)

	//----------------------------------------------------------------------
	// データ並び替え
	//----------------------------------------------------------------------
	DEBUG_PRINT ("FFC Data Memory Write..\n");

	// メモリアドレス取得
	if ((status = ffcGetMemAdrs (ffcParam.ffcNo, ffcParam.memType, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// 外部メモリアドレス取得(1度外部メモリにデータを格納)
	//----------------------------------------------------------------------
#if defined (MODE_FFC_DATA_ALIGN_ADJUST)
 	if (ffcParam.memType == FFC_MEMORY_INT)
	{
		if ((status = ffcGetMemAdrs (ffcParam.ffcNo, FFC_MEMORY_EXT, &memAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// ゲインデータ設定
	ffcSetMemoryOffsetGain (ffcParam.ffcNo, memAdrs, (unsigned short *)NULL, (int *)pImageGain);

	//----------------------------------------------------------------------
	// 外部メモリアドレスから内部メモリアドレスにコピー(データを保持するために)
	//----------------------------------------------------------------------
#if defined (MODE_FFC_DATA_ALIGN_ADJUST)
 	if (ffcParam.memType == FFC_MEMORY_INT)
	{
		unsigned int intMemAdrs;

		// 内部メモリアドレス取得
		if ((status = ffcGetMemAdrs (ffcParam.ffcNo, ffcParam.memType, &intMemAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// データコピー
		if ((status = ffcGetExtMemory (memAdrs, intMemAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	//----------------------------------------------------------------------
	// FFC白レベル情報保存
	//----------------------------------------------------------------------
	if (ffcAdjustAllFlag == 0)
	{
		ffcWhiteInfo.frameRate = frameRateWhiteInfo;
		ffcWhiteInfo.expTime = expTimeWhiteInfo;
		ffcWhiteInfo.target = ffcParam.target;
		ffcWhiteInfo.sensorTemp = sensorTempWhiteInfo;
		ffcWhiteInfo.mark = FFC_MARK_DATA;
		ffcWhiteInfo.digitalGain = gDigitalGainInfo;
		ffcWhiteInfo.bit = ffcParam.bit;
	}
	else
	{
		pImageFfcWhiteInfo[ffcParam.ffcNo].frameRate = frameRateWhiteInfo;
		pImageFfcWhiteInfo[ffcParam.ffcNo].expTime = expTimeWhiteInfo;
		pImageFfcWhiteInfo[ffcParam.ffcNo].target = ffcParam.target;
		pImageFfcWhiteInfo[ffcParam.ffcNo].sensorTemp = sensorTempWhiteInfo;
		pImageFfcWhiteInfo[ffcParam.ffcNo].mark = FFC_MARK_DATA;
		pImageFfcWhiteInfo[ffcParam.ffcNo].digitalGain = gDigitalGainInfo;
		pImageFfcWhiteInfo[ffcParam.ffcNo].bit = ffcParam.bit ;
	}

_DONE:

	//----------------------------------------------------------------------
	// FFC番号保存(メモリ)
	//----------------------------------------------------------------------
	ffcSetLoadNum (ffcParam.ffcNo);

	// キャッシュFlash
	cacheFlush ();

	 if (status == AVAL_STATUS_SUCCESS)
	 {
		// FFC有効
		ffcSetMode (FFC_ENABLE, FFC_ENABLE);
	 }

	//--------------------------------------------------
	// ROI Area Restore設定
	//--------------------------------------------------
	roiSetDmaResutore ();

	//--------------------------------------------------
	// Bit Restore設定
	//--------------------------------------------------
#if !defined (MODE_FFC_BIT_CALC)
	if (saveBit != -1)
		aoiSetBitWidth (saveBit);
#endif

	//--------------------------------------------------
	// DPCレジスタ設定
	//--------------------------------------------------
	dpcSetEnableMode (gDpcSave);

	return (status);
}


//**********************************************************************************
// FFCデータFlash書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcParam			：FFC情報パラメータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcFlashSave (FFC_PARAM ffcParam)
{
	int status;
	int startFlag = 0;
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int lineStart, ffcHeight;
	int hsMode;
	int changedFlag = 0;
#endif

	// Check ffcNo Parameter
	if ((ffcParam.ffcNo < FFC_NUMBER_MIN) || (ffcParam.ffcNo > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Save ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcParam.ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check memType Parameter
	if ((ffcParam.memType != FFC_MEMORY_EXT) && (ffcParam.memType != FFC_MEMORY_INT))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Save memType(%d) Parameter Error.(EXT:%d / INT:%d)\n", ffcParam.memType, FFC_MEMORY_EXT, FFC_MEMORY_INT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((ffcParam.userMode != FFC_USER) && (ffcParam.userMode != FFC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Save User(%d) Parameter Error.(User:%d / Admin:%d)\n", ffcParam.userMode, FFC_USER, FFC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//----------------------------------------------------------------------
	// AcquisitionStop
	//----------------------------------------------------------------------

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// FFCデータ入れ替え(元に戻す)
	//----------------------------------------------------------------------
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
		if ((status = ffcGetReplacementLine (&lineStart, ffcHeight, FFC_CORRECTION_MODE_FIRST)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFCデータ入れ替え
		if ((status = ffcDataReplacement (lineStart, ffcHeight, FFC_CORRECTION_MODE_FIRST)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Flag設定
		changedFlag = 1;
	}
#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)


	//--------------------------------------------------
	// FFCデータ保存
	//--------------------------------------------------
	if (ffcParam.userMode == FFC_USER)
	{
		if ((status = ffcSave (ffcParam.ffcNo, ffcParam.memType)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		if ((status = ffcSaveAdmin (ffcParam.ffcNo, ffcParam.memType)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//--------------------------------------------------
	// FFC Adjust情報保存
	//--------------------------------------------------
	if (ffcAdjustAllFlag == 0)
	{
		ffcAdjustInfoBlackSave (ffcParam.ffcNo, ffcParam.userMode, ffcBlackInfo);
		ffcAdjustInfoWhiteSave (ffcParam.ffcNo, ffcParam.userMode, ffcWhiteInfo);
	}
	else
	{
		ffcAdjustInfoBlackSave (ffcParam.ffcNo, ffcParam.userMode, pImageFfcBlackInfo[ffcParam.ffcNo]);
		ffcAdjustInfoWhiteSave (ffcParam.ffcNo, ffcParam.userMode, pImageFfcWhiteInfo[ffcParam.ffcNo]);
	}

_DONE:
	//--------------------------------------------------
	// FFCデータ入れ替え(fhmモード時の設定にする)
	//--------------------------------------------------
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	if ((hsMode == MODE_ENABLE) && (changedFlag == 1))
	{
		// FFCデータ入れ替え
		ffcDataReplacement (lineStart, ffcHeight, FFC_CORRECTION_MODE_FIRST);
	}
#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)

	if (startFlag == 1)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// カメラパラメータ保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionParamSave (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// 初期化
	gBitSave = 0;
	gFrameRateSave = 0;
	gExpTimeSave = 0;
	gAcquisitionMode = -1;

	// Bit幅取得
	if ((status = aoiGetBitWidth (&gBitSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレート取得
	if ((status = acquisitionGetFrameRate (&gFrameRateSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 露光時間取得
	if ((status = acquisitionGetExposure (&gExpTimeSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 連続モード取り込み取得
	if ((status = acquisitionGetMode (&gAcquisitionMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// カメラパラメータ書き戻し
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int acquisitionParamRestore (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Bit幅設定
	if (gBitSave != 0)
	{
		if ((status = aoiSetBitWidth (gBitSave)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// クリア
		gBitSave = 0;
	}

	// フレームレート設定
	if (gFrameRateSave != 0)
	{
		if ((status = acquisitionSetFrameRate (gFrameRateSave)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// クリア
		gFrameRateSave = 0;
	}

	// 露光時間設定
	if (gExpTimeSave != 0)
	{
		if ((status = acquisitionSetExposure (gExpTimeSave)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// クリア
		gExpTimeSave = 0;
	}

	// 連続モード取り込み取得
	if (gAcquisitionMode != -1)
	{
		if ((status = acquisitionSetMode (gAcquisitionMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// クリア
		gAcquisitionMode = -1;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset Gainパラメータ保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dogParamSave (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// 黒レベル目標値保存用
	gBlacktarget = 0xffffffff;

	// Digital Offset1保存用
	gDogOffset1 = -1;

	// Digital Offset2保存用
	gDogOffset2 = -1;
	
	// Digital Gain保存用
	gDogGain = 0;

	// Digital Gain保存用
	gDogGainReg = 0;

	// 黒レベル目標値をレジスタから取得
	ffcGetBlackTarget (&gBlacktarget);

	// デジタルオフセットゲイン取得
	if ((status = dogGetOffsetGain (&gDogOffset1, &gDogOffset2, &gDogGain)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ゲインレジスタ取得
	if ((status = digitalGetGainReg (&gDogGainReg)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	gDogGainReg = IN32 (FPGA_DOG_GAIN_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// Digital Offset Gainパラメータリストア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dogParamRestore (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// 黒レベル目標値
	if (gBlacktarget != 0xffffffff)
		ffcSetBlackTarget (gBlacktarget);

	// Digital Offset Gain
	if (gDogOffset1 != -1)
	{
		// デジタルオフセットゲイン設定
		if ((status = dogSetOffsetGain (gDogOffset1, gDogOffset2, gDogGain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// デジタルゲイン設定(レジスタ値)
		if ((status = digitalSetGainReg (gDogGainReg)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


#if defined (MODE_PELTIER)
//**********************************************************************************
// センサ温度パラメータ保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tempParamSave (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// センサターゲット温度保存
	gSensortTemp = -1;
	if ((status = peltierGetTarget (&gSensortTemp)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}
#endif // #if defined (MODE_PELTIER)


#if defined (MODE_PELTIER)
//**********************************************************************************
// センサ温度パラメータリストア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int tempParamRestore (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Sensor target Temp
	if (gSensortTemp != -1)
	{
		// デジタルオフセットゲイン設定
		if ((status = peltierSetTarget (gSensortTemp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}
#endif // #if defined (MODE_PELTIER)


//**********************************************************************************
//	FFC調整データ表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		userMode			：0=User/1=Admin
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcAdjustInfo (int ffcNum, int userMode)
{
	int status;
	FFCINFO ffcInfo;
	int min, max;
#if defined (MODE_FFC_BIT_CALC)
	int calc;
#endif

	// Check userMode Parameter
	if ((userMode != FFC_USER) && (userMode != FFC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Adjust Information User(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, FFC_USER, FFC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 最大／最小値を取得
	if (userMode == FFC_USER)
	{
		min = FFC_NUMBER_MIN;
		max = FFC_NUMBER_MAX;
	}
	else
	{
		min = FFC_NUMBER_ADMIN_MIN;
		max = FFC_NUMBER_ADMIN_MAX;
	}

	// Check ffcNum Parameter
	if ((ffcNum < min) || (ffcNum > max))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Adjust Information ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, min, max);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//----------------------------------------------------------------------
	// FFC黒レベル情報読み込み
	//----------------------------------------------------------------------
	if ((status = ffcAdjustInfoBlackRead (ffcNum, userMode, &ffcInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// FFC Adjust情報(黒レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC adjust Black Level informations]\n");

	//----------------------------------------------------------------------
	// フレームレート(黒レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Frame          %.2f[fps]\n", ffcInfo.frameRate);

	//----------------------------------------------------------------------
	// 露光時間(黒レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Exposure       %d[us]\n", ffcInfo.expTime);

	//----------------------------------------------------------------------
	// 黒レベル設定値
	//----------------------------------------------------------------------
#if defined (MODE_FFC_BIT_CALC)
	if ((status = ffcGetBitCalc2 (ffcInfo.bit, &calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 黒レベル目標値設定
	ffcInfo.target /= calc;
#endif
	DEBUG_PRINT_FORCE ("   Black Target   %d\n", ffcInfo.target);

	//----------------------------------------------------------------------
	// 温度(黒レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Temp           %.2f\n", ffcInfo.sensorTemp);

	//----------------------------------------------------------------------
	// ゲイン(黒レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Gain           %.2f\n", ffcInfo.digitalGain);

	//----------------------------------------------------------------------
	// Bit(黒レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Bit            %d\n", ffcInfo.bit);

	DEBUG_PRINT_FORCE ("\n");

	//----------------------------------------------------------------------
	// FFC白レベル情報読み込み
	//----------------------------------------------------------------------
	if ((status = ffcAdjustInfoWhiteRead (ffcNum, userMode, &ffcInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//----------------------------------------------------------------------
	// FFC Adjust情報(白レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC adjust White Level informations]\n");

	//----------------------------------------------------------------------
	// フレームレート(白レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Frame          %.2f[fps]\n", ffcInfo.frameRate);

	//----------------------------------------------------------------------
	// 露光時間(白レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Exposure       %d[us]\n", ffcInfo.expTime);

	//----------------------------------------------------------------------
	// 白レベル設定値
	//----------------------------------------------------------------------
#if defined (MODE_FFC_BIT_CALC)
	if ((status = ffcGetBitCalc2 (ffcInfo.bit, &calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 黒レベル目標値設定
	ffcInfo.target /= calc;
#endif
	DEBUG_PRINT_FORCE ("   White Target   %d\n", ffcInfo.target);

	//----------------------------------------------------------------------
	// 温度(白レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Temp           %.2f\n", ffcInfo.sensorTemp);

	//----------------------------------------------------------------------
	// Bit(白レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Gain           %.2f\n", ffcInfo.digitalGain);

	//----------------------------------------------------------------------
	// ゲイン((白レベル)
	//----------------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Bit            %d\n", ffcInfo.bit);

	DEBUG_PRINT_FORCE ("\n");

_DONE:
	return (status);
}


//**********************************************************************************
// FFC情報保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		userMode			：0=User/1=Admin
//		ffcInfo				：FFC情報
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcAdjustInfoBlackSave (int ffcNum, int userMode, FFCINFO ffcInfo)
{
	int status;
	unsigned int high, low;
	unsigned long long temp64;
	unsigned char *pBuffer = NULL;
	unsigned int saveAdrs;
	unsigned int size = FFC_NUM_FACTORY * FFC_SAVE_INFO_SIZE;
	unsigned int flashAdrs;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Adjust Black Information ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((userMode != FFC_USER) && (userMode != FFC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Adjust Black Information User(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, FFC_USER, FFC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリリクエスト
	if ((pBuffer = malloc ((size))) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "FFC Adjust Black Information Buffer Request Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// アドレスCopy
	saveAdrs = (unsigned int)pBuffer;
	saveAdrs += ffcNum * FFC_SAVE_INFO_SIZE;

	// FFC情報読み込み
	if (userMode == FFC_USER)
	{
		if ((status = qspiFlashRead (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else	// Admin
	{
		if ((status = qspiFlashRead (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// フレームレートが設定されていれば保存
	if (ffcInfo.expTime != 0)
	{
		//----------------------------------------------------------------------
		// フレームレート保存
		//----------------------------------------------------------------------
		memcpy (&temp64, &ffcInfo.frameRate, 8);
		high = (unsigned int)((unsigned int)((unsigned long long)temp64>>32)&0xffffffff);
		low  = (unsigned int)(temp64&0xffffffff);

		OUT32 ((saveAdrs + FFC_SAVE_BLACK_FRAME_H), high);
		OUT32 ((saveAdrs + FFC_SAVE_BLACK_FRAME_L), low);

		//----------------------------------------------------------------------
		// 露光時間保存
		//----------------------------------------------------------------------
		OUT32 ((saveAdrs + FFC_SAVE_BLACK_EXPOSURE), ffcInfo.expTime);

		//----------------------------------------------------------------------
		// 黒レベル設定値保存
		//----------------------------------------------------------------------
		OUT32 ((saveAdrs + FFC_SAVE_BLACK_TARGET), ffcInfo.target);

		//----------------------------------------------------------------------
		// 温度設定値保存
		//----------------------------------------------------------------------
		memcpy (&temp64, &ffcInfo.sensorTemp, 8);
		high = (unsigned int)((unsigned int)((unsigned long long)temp64>>32)&0xffffffff);
		low  = (unsigned int)(temp64&0xffffffff);

		OUT32 ((saveAdrs + FFC_SAVE_BLACK_TEMP_H), high);
		OUT32 ((saveAdrs + FFC_SAVE_BLACK_TEMP_L), low);

		//----------------------------------------------------------------------
		// ゲイン保存
		//----------------------------------------------------------------------
		OUTF ((saveAdrs + FFC_SAVE_BLACK_GAIN), ffcInfo.digitalGain);

		//----------------------------------------------------------------------
		// Bit保存
		//----------------------------------------------------------------------
		OUT32 ((saveAdrs + FFC_SAVE_BLACK_BIT), ffcInfo.bit);
	}

	// FFC情報書き込み
	if (userMode == FFC_USER)
	{
		// イレーズ
		if ((status = qspiFlashSectorErase (FLASH_FFC_INFO_ADRS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 書き込み
		if ((status = qspiFlashWrite (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else // Admin
	{
		// イレーズ
		if ((status = qspiFlashSectorErase (FLASH_FFC_INFO_ADRS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 書き込み
		if ((status = qspiFlashWrite (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// クリア
	ffcInfo.frameRate = 0xffffffff;

_DONE:
	// メモリ開放
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
// FFC情報保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		userMode			：0=User/1=Admin
//		ffcInfo				：FFC情報
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcAdjustInfoWhiteSave (int ffcNum, int userMode, FFCINFO ffcInfo)
{
	int status;
	unsigned int high, low;
	unsigned long long temp64;
	unsigned char *pBuffer = NULL;
	unsigned int saveAdrs;
	unsigned int size = FFC_NUM_FACTORY * FFC_SAVE_INFO_SIZE;
	unsigned int flashAdrs;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Adjust White Information ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((userMode != FFC_USER) && (userMode != FFC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Adjust White Information User(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, FFC_USER, FFC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリリクエスト
	if ((pBuffer = malloc ((size))) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "FFC Adjust White Information Buffer Request Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// アドレスCopy
	saveAdrs = (unsigned int)pBuffer;
	saveAdrs += ffcNum * FFC_SAVE_INFO_SIZE;

	// FFC情報読み込み
	if (userMode == FFC_USER)
	{
		if ((status = qspiFlashRead (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else	// Admin
	{
		if ((status = qspiFlashRead (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// フレームレートが設定されていれば保存
	if (ffcInfo.expTime != 0)
	{
		//----------------------------------------------------------------------
		// フレームレート保存
		//----------------------------------------------------------------------
		memcpy (&temp64, &ffcInfo.frameRate, 8);
		high = (unsigned int)((unsigned int)((unsigned long long)temp64>>32)&0xffffffff);
		low  = (unsigned int)(temp64&0xffffffff);

		OUT32 ((saveAdrs + FFC_SAVE_WHITE_FRAME_H), high);
		OUT32 ((saveAdrs + FFC_SAVE_WHITE_FRAME_L), low);

		//----------------------------------------------------------------------
		// 露光時間保存
		//----------------------------------------------------------------------
		OUT32 ((saveAdrs + FFC_SAVE_WHITE_EXPOSURE), ffcInfo.expTime);

		//----------------------------------------------------------------------
		// 白レベル設定値保存
		//----------------------------------------------------------------------
		OUT32 ((saveAdrs + FFC_SAVE_WHITE_TARGET), ffcInfo.target);

		//----------------------------------------------------------------------
		// 温度設定値保存
		//----------------------------------------------------------------------
		memcpy (&temp64, &ffcInfo.sensorTemp, 8);
		high = (unsigned int)((unsigned int)((unsigned long long)temp64>>32)&0xffffffff);
		low  = (unsigned int)(temp64&0xffffffff);

		OUT32 ((saveAdrs + FFC_SAVE_WHITE_TEMP_H), high);
		OUT32 ((saveAdrs + FFC_SAVE_WHITE_TEMP_L), low);

		//----------------------------------------------------------------------
		// ゲイン保存
		//----------------------------------------------------------------------
		OUTF ((saveAdrs + FFC_SAVE_WHITE_GAIN), ffcInfo.digitalGain);

		//----------------------------------------------------------------------
		// Bit保存
		//----------------------------------------------------------------------
		OUT32 ((saveAdrs + FFC_SAVE_WHITE_BIT), ffcInfo.bit);
	}

	// FFC情報書き込み
	if (userMode == FFC_USER)
	{
		// イレーズ
		if ((status = qspiFlashSectorErase (FLASH_FFC_INFO_ADRS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 書き込み
		if ((status = qspiFlashWrite (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// イレーズ
		if ((status = qspiFlashSectorErase (FLASH_FFC_INFO_ADRS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 書き込み
		if ((status = qspiFlashWrite (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// クリア
	ffcInfo.frameRate = 0xffffffff;

_DONE:
	// メモリ開放
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
//	FFC黒レベル調整データ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		userMode			：0=User/1=Admin
//		ffcInfo				：FFC情報を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcAdjustInfoBlackRead (int ffcNum, int userMode, FFCINFO *pFfcInfo)
{
	int status;
	unsigned long long temp64;
	unsigned char *pBuffer = NULL;
	unsigned int saveAdrs;
	unsigned int size = FFC_NUM_FACTORY * FFC_SAVE_INFO_SIZE;
	double temp;
	unsigned int flashAdrs;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Black Adjust Information Read ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((userMode != FFC_USER) && (userMode != FFC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Black Adjust Information Read User(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, FFC_USER, FFC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pFfcInfo Parameter
	if (pFfcInfo == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Black Adjust Information Read pFfcInfo NULL Parameter Error.\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリリクエスト
	if ((pBuffer = malloc ((size))) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "FFC Black Adjust Read Information Buffer Request Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// アドレスCopy
	saveAdrs = (unsigned int)pBuffer;
	saveAdrs += ffcNum * FFC_SAVE_INFO_SIZE;

	// FFC情報読み込み
	if (userMode == FFC_USER)
	{
		if ((status = qspiFlashRead (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		if ((status = qspiFlashRead (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//----------------------------------------------------------------------
	// フレームレート(黒レベル)
	//----------------------------------------------------------------------
	temp64 = IN32 ((saveAdrs + FFC_SAVE_BLACK_FRAME_H));
	temp64 <<= 32;
	temp64 |= IN32 ((saveAdrs + FFC_SAVE_BLACK_FRAME_L));
	memcpy (&temp, &temp64, 8);
	pFfcInfo->frameRate = temp;

	//----------------------------------------------------------------------
	// 露光時間(黒レベル)
	//----------------------------------------------------------------------
	pFfcInfo->expTime = IN32 ((saveAdrs + FFC_SAVE_BLACK_EXPOSURE));

	//----------------------------------------------------------------------
	// 黒レベル設定値
	//----------------------------------------------------------------------
	pFfcInfo->target = IN32 ((saveAdrs + FFC_SAVE_BLACK_TARGET));

	//----------------------------------------------------------------------
	// 温度(黒レベル)
	//----------------------------------------------------------------------
	temp64 = IN32 ((saveAdrs + FFC_SAVE_BLACK_TEMP_H));
	temp64 <<= 32;
	temp64 |= IN32 ((saveAdrs + FFC_SAVE_BLACK_TEMP_L));
	memcpy (&temp, &temp64, 8);
	pFfcInfo->sensorTemp = temp;

	//----------------------------------------------------------------------
	// ゲイン(黒レベル)
	//----------------------------------------------------------------------
	pFfcInfo->digitalGain = INF ((saveAdrs + FFC_SAVE_BLACK_GAIN));

	//----------------------------------------------------------------------
	// Bit(黒レベル)
	//----------------------------------------------------------------------
	pFfcInfo->bit = IN32 ((saveAdrs + FFC_SAVE_BLACK_BIT));

_DONE:
	// メモリ開放
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
//	FFC白レベル調整データ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//		userMode			：0=User/1=Admin
//		ffcInfo				：FFC情報を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffcAdjustInfoWhiteRead (int ffcNum, int userMode, FFCINFO *pFfcInfo)
{
	int status;
	unsigned long long temp64;
	unsigned char *pBuffer = NULL;
	unsigned int saveAdrs;
	unsigned int size = FFC_NUM_FACTORY * FFC_SAVE_INFO_SIZE;
	double temp;

	// Check ffcNum Parameter
	if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC White Adjust Information Read ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((userMode != FFC_USER) && (userMode != FFC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC White Adjust Information Read User(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, FFC_USER, FFC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pFfcInfo Parameter
	if (pFfcInfo == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC White Adjust Information Read pFfcInfo NULL Parameter Error.\n");
		goto _DONE;
	}

	// メモリリクエスト
	if ((pBuffer = malloc ((size))) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC White Adjust Information Read Buffer Request Error\n");
		goto _DONE;
	}

	// アドレスCopy
	saveAdrs = (unsigned int)pBuffer;
	saveAdrs += ffcNum * FFC_SAVE_INFO_SIZE;

	// FFC情報読み込み
	if (userMode == FFC_USER)
	{
		if ((status = qspiFlashRead (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		if ((status = qspiFlashRead (FLASH_FFC_INFO_ADRS, (unsigned char *)pBuffer, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//----------------------------------------------------------------------
	// フレームレート(白レベル)
	//----------------------------------------------------------------------
	temp64 = IN32 ((saveAdrs + FFC_SAVE_WHITE_FRAME_H));
	temp64 <<= 32;
	temp64 |= IN32 ((saveAdrs + FFC_SAVE_WHITE_FRAME_L));
	memcpy (&temp, &temp64, 8);
	pFfcInfo->frameRate = temp;

	//----------------------------------------------------------------------
	// 露光時間(白レベル)
	//----------------------------------------------------------------------
	pFfcInfo->expTime = IN32 ((saveAdrs + FFC_SAVE_WHITE_EXPOSURE));

	//----------------------------------------------------------------------
	// 白レベル設定値
	//----------------------------------------------------------------------
	pFfcInfo->target = IN32 ((saveAdrs + FFC_SAVE_WHITE_TARGET));

	//----------------------------------------------------------------------
	// 温度(白レベル)
	//----------------------------------------------------------------------
	temp64 = IN32 ((saveAdrs + FFC_SAVE_WHITE_TEMP_H));
	temp64 <<= 32;
	temp64 |= IN32 ((saveAdrs + FFC_SAVE_WHITE_TEMP_L));
	memcpy (&temp, &temp64, 8);
	pFfcInfo->sensorTemp = temp;

	//----------------------------------------------------------------------
	// ゲイン
	//----------------------------------------------------------------------
	pFfcInfo->digitalGain = INF ((saveAdrs + FFC_SAVE_WHITE_GAIN));

	//----------------------------------------------------------------------
	// Bit
	//----------------------------------------------------------------------
	pFfcInfo->bit = IN32 ((saveAdrs + FFC_SAVE_WHITE_BIT));

_DONE:
	// メモリ開放
	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


#if defined (MODE_PELTIER)
//**********************************************************************************
//	センサ温度取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//			sensorTemp		：センサ温度設定値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdSensorTempBatch (double sensorTemp)
{
	int status = AVAL_STATUS_SUCCESS;
	double getTemp;
	double setTempMin, setTempMax;
	unsigned int timeout;
	int consoleMode = CONSOLE_MODE_OFF;
	double targetMin, targetMax;
	char input[CONSOLE_BUFF_SIZE];
	char c;

	// センサ温度設定
	if ((status = peltierGetTargetMinMax (&targetMin, &targetMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pSnesorTemp Parameter
	if ((sensorTemp < targetMin) || (sensorTemp > targetMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Temp(%.2f) Parameter Error. (Min:%.2f / Max:%.2f)\n", sensorTemp, targetMin, targetMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// センサターゲット温度設定
	if ((status = peltierSetTarget (sensorTemp)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Console Mode取得
	consoleGetMode (&consoleMode);

	//--------------------------------------------------------------------------------
	// 指定温度になるまで待つ
	//--------------------------------------------------------------------------------
	for (timeout=0; timeout<FFC_TEMP_TIMEOUT; timeout++)
	{
		// 温度取得
		if ((status = peltierGetSensorTemp (&getTemp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 温度Check
		setTempMin = sensorTemp - SENSOR_TEMP_ADJUST_MARGIN;
		setTempMax = sensorTemp + SENSOR_TEMP_ADJUST_MARGIN;

		DEBUG_PRINT ("[%d]Sensor Temp:%.2f", timeout+1, getTemp);
		fflush (stdout);

		// 温度は範囲内？
		if ((getTemp >= setTempMin) && (getTemp <= setTempMax))
			break;

		// コマンドラインbackspace
		if (consoleMode == CONSOLE_MODE_ON)
			cmdBackspaceLine ();

		// キー入力あり？
		if (_kbhit (&c))
		{
			cmdCharGet (input);

			if (c == 'y')
			{
				if (input[0] == '\0')
				{
					// 正常終了＝＞処理継続
					DEBUG_PRINT ("\nFFC Adjust Temp Through.\n");
					break;
				}
			}
			else if (c == 'n')
			{
				if (input[0] == '\0')
				{
					// 異常終了＝＞処理中断
					status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_TIMEOUT);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "\nFFC Adjust End.\n");
					break;
				}
			}
		}

		msDelay (1000);
	}

	DEBUG_PRINT ("\n");

	// Timeout
	if (timeout >= FFC_TEMP_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_TIMEOUT);
		sprintf (gLogMsgBuff, "Sensor Temp Timeout Error. (Current Temp:%.2f / Target Temp:%.2f)\n", getTemp, sensorTemp);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}
#endif // #if defined (MODE_PELTIER)

// eof
