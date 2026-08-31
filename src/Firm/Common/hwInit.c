//**********************************************************************************
//
//                               Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// hwInit.c - HW Initialize Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int gSpectrumType;


//**********************************************************************************
//	HW初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		cpuType				：0=ARM0/1=ARM1
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int hwInitialize (int cpuType)
{
	int status;
	unsigned int dipsw = 0;

#if 0 //@@@1
	//------------------------------------------------------------
	// Flag初期化
	//------------------------------------------------------------
	// Acquisition Start初期化
	OUT32 (FIRM_DATA_ACQUISITION_START_ADRS, 0);

	// ROI Area Mode初期化
	OUT32 (FIRM_DATA_ROI_AREA_MODE_ADRS, ROI_AREA_MODE_DEFAULT_SIZE);

	// ROI Area Size初期化
	OUT32 (FIRM_DATA_ROI_AREA_SIZE_ADRS, ROI_AREA_MODE_DEFAULT_SIZE);

	// Variant Type
	OUT32 (FIRM_DATA_VARIANT_MODE_ADRS, DEVICE_VARIANT_TYPE);

	// Frame Rate Time初期化
	OUTF (FIRM_DATA_MAX_FRAME_RATE_TIME, 0);

	// X Flip Mode
#if defined (MODE_SENSOR_XFLIP)

	#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	OUT32 (FPGA_XFLIP_CTRL_ADRS, 0);
	#endif

	dpcSetXFlipFlag (MODE_ENABLE);

	ffcSetXFlipFlag (MODE_ENABLE);
#endif


#if defined (MODE_CAMERA_EVENT_VERSION2)
	memset ((void *)CAMERA_EVENT, 0x00, CAMERA_SIZE);
#endif

	// Gradation Compress Mode
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	sensorGradationCompSetModeDDR (MODE_DISABLE);
	sensorGradationCompSetModeDDR2 (MODE_DISABLE);

	OUT32 (FIRM_DATA_GRADATION_COMPRESS_POS_FIRST, 0);
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_POS_SECOND, 0);
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_GAIN_FIRST, 0);
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_GAIN_SECOND, 0);
#endif


#if defined (MODE_FFC_SHADING_LINE)
	// FFC Move Average
	ffcSetMoveAverage (MODE_DISABLE);
#endif

	// White Gain
#if defined (MODE_FFC_SHADING_LINE)
	ffcSetWhiteGainX (DEFAULT_FFC_WHITE_GAINX);
#endif

#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	// 8Bit Mode
	OUT32 (FIRM_DATA_8BIT_MODE, MODE_DISABLE);
#endif

	// IF Version
	OUT32 (FIRM_DATA_IF_VERSION_ADRS, 0);

	// Firm Command
	firmSetCmdStatus (0);


#if defined (MODE_FRAMERATE_HIGH_SPEED)
	// High Speed Reg設定。レジスタの初期値を１度設定。
	sensorSetFrameRateHighSpeedReg (MODE_DISABLE);
#endif

#if defined (MODE_CXP_MULTI_PORT) && defined (IF_CXP)
	// CXPのPort番号を指定する
	cxpSetPort (0);
#endif

	// ARM1 DDR malloc Flag初期化
	OUT32(FIRM_DATA_DDR_DIAG_FLAG, FIRM_DATA_DDR_DIAG_FLAG_NONE);

	// DDR Mode初期化
	diagDdrSetMode (0);

	// Sensor Shutter Mode(Trigger Mode)
	OUT32 (FIRM_DATA_SENSOR_SHUTTER_MODE, 1);	// Triggerモード

#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
	// GE Speed Default
	gigeSetSpeed (GIGE_SPEED_10GIG_FD);
#endif

	// Access Flag
	OUT32 (FIRM_DATA_CPU0_MAIN_ACCESS_FLAG, 0);

	// TG Mode
	OUT32 (FIRM_DATA_TG_CTRL_MODE, 0);

	// CPU1 Runnning
	//@@@1cpu1SetRunning (MODE_ENABLE);

	// High Speed Initialize2
	OUT32 (FIRM_DATA_HIGHSPPED_INIT2_ADRS, 0);
	
	// Cmd Execute Flag Clear
	OUT32 (FIRM_DATA_CMD_EXE_FLAG, 0);

	//------------------------------------------------------------
	// Get Type
	//------------------------------------------------------------

	// Get InterFace ID
	getInterfaceId (&gInterFaceID);

	// Get Spectrum Type
	getSpectrumType (&gSpectrumType);
#endif //@@@1
	
	//------------------------------------------------------------
	// Console Initialize
	//------------------------------------------------------------
	consoleDefaultInitialize ();

	//------------------------------------------------------------
	// Interrupt Controller Initialize
	//------------------------------------------------------------
	if ((status = intcInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// Timer Initialize
	//------------------------------------------------------------
	if ((status = timerInitilize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// QSPI Flash Initialize
	//------------------------------------------------------------
	if ((status = qspiFlashInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// Camera Parameter Initialize
	//------------------------------------------------------------
	if ((status = cameraParamInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// UART Initialize
	//------------------------------------------------------------
	if ((status = uartInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	// Memory Mapping
	//@@@1map ();

#if 0//@@@1
	//------------------------------------------------------------
	// I2C Initialize
	//------------------------------------------------------------
#if defined(MODE_I2C)
	if ((status = i2cInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// SPI PL PolaFire Initialize
	//------------------------------------------------------------
#if defined (MODE_FPGA_PF)
	if (gInterFaceID == INTERFACE_CXP)
	{
		if ((status = PolarFireFlashInitialize ()) != AVAL_STATUS_SUCCESS)
			firmErrorAttention (status);
	}
#endif
#endif //@@@1

	//------------------------------------------------------------
	// Console Initialize
	//------------------------------------------------------------
	if ((status = consoleInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

#if 0 //@@@1
	//------------------------------------------------------------
	// DMA Initialize
	//------------------------------------------------------------
	if ((status = dmaInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// Sensor Initialize
	//------------------------------------------------------------
	if ((status = sensorInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// 画像補正バッファ初期化
	//------------------------------------------------------------
	if ((status = buffInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif //@@@1

	//------------------------------------------------------------
	//	XML Fileメモリロード
	//------------------------------------------------------------
	//@@1if ((status = xmlFileLoadMemory ()) != AVAL_STATUS_SUCCESS)
		//@@@1firmErrorAttention (status);

#if 0 //@@@1
	//------------------------------------------------------------
	//	Gev Parameter Initialize
	//------------------------------------------------------------
#if defined (MODE_GE_SPEED)
	if ((status = gevParamInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif		

	//------------------------------------------------------------
	// PHYデータロード
	//------------------------------------------------------------
#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
	if (gInterFaceID == INTERFACE_GIGE)
	{
		if ((status = phyDataFlashToMem ()) != AVAL_STATUS_SUCCESS)
			firmErrorAttention (status);
	}
#endif

	//------------------------------------------------------------
	// ボードエラーステータス初期化
	//------------------------------------------------------------
	if ((status = cameraStatusInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif //@@@1
	//------------------------------------------------------------
	// カメラ情報初期化
	//------------------------------------------------------------
	if ((status = cameraInformationInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#if 0 //@@@1
	//------------------------------------------------------------
	// DIPSW Bit0 ONならばFPGA系の初期化は実施しない
	//------------------------------------------------------------
	getDipsw (&dipsw);

	if (gInterFaceID == INTERFACE_CAMERALINK)
	{
		// Dipsw=0x03はCameraLinkシリアル無効化設定
		if (dipsw == 0x03)
			dipsw = 0;
	}
	
	if ((dipsw & 0x01) == 0)
	{
		//------------------------------------------------------------
		// HW Initialize2
		//------------------------------------------------------------
		hwInitialize2 (HW_INIT_MODE_POWER_ON);
	}
	else
	{
		//------------------------------------------------------------
		// HW Initialize Normal
		//------------------------------------------------------------
		hwInitializeNormal ();

		//------------------------------------------------------------
		// Userset Factory Clear
		//------------------------------------------------------------
		userSetFactoryClear ();

		//------------------------------------------------------------
		// Userset Common Clear
		//------------------------------------------------------------
		cameraParamCommonMarkClear ();

		//------------------------------------------------------------
		// Userset Format
		//------------------------------------------------------------
		cameraParamUserMarkClear (CAMERA_FACTORY_NUM);

		//------------------------------------------------------------
		// Userset Spectrum Format
		//------------------------------------------------------------
		#if defined (MODE_USERSET_SPECTRUM_PARAM)
		cameraParamSpectrumMarkClear (CAMERA_FACTORY_NUM);
		#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)
	}

	//------------------------------------------------------------
	// カメラ毎の設定変更がある場合、ここで記載
	//------------------------------------------------------------
	if ((status = cameraParamCustom ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// FPGAボード電圧チェック
	//------------------------------------------------------------
#if defined (MODE_VOLTAGE_IF_BOARD)
	if ((status = voltIfInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif
#endif //@@@1

	//------------------------------------------------------------
	// CXP初期化
	//------------------------------------------------------------
#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		if ((status = cxpInitialize ()) != AVAL_STATUS_SUCCESS)
			firmErrorAttention (status);
	}
#endif

	//------------------------------------------------------------
	// ボードエラーステータス設定
	//------------------------------------------------------------
	if ((status = cameraSetStatusAll ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	// Boot Flag
	OUT32 (FIRM_DATA_FIRM_BOOT_FLAG_ADRS, 1);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	HW初期化2
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：電源投入後の最初の場合=0/loadコマンドの場合=1
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int hwInitialize2 (int mode)
{
	int status;
	int bootStartMode;
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsModeNew, hsModeOld;
#endif


	//------------------------------------------------------------
	// FrameRate High Speed Mode取得
	//------------------------------------------------------------
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	// High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode (&hsModeOld)) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	// High Speed Mode Initialize
	//if ((status = sensorFrameRateHighSpeedModeInitialize ()) != AVAL_STATUS_SUCCESS)
	if ((status = sensorFrameRateHighSpeedModeInitializeDefault ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif


#if defined(MODE_TEMP)
	//------------------------------------------------------------
	// Peltier Initialize
	//------------------------------------------------------------
	if ((status = peltierInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

#if defined (MODE_PELTIER_CTRL)
	//------------------------------------------------------------
	// Peltier Control Initialize
	//------------------------------------------------------------
	if ((status = peltierControlInitiallize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif


	//------------------------------------------------------------
	// Timing Generator Version2初期化
	//------------------------------------------------------------
	if ((status = timingGeneratorVersion2Initalize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);


	if (mode == HW_INIT_MODE_POWER_ON)
	{
		//------------------------------------------------------------
		// 特殊な初期化
		//------------------------------------------------------------
		if ((status = specialInitialize ()) != AVAL_STATUS_SUCCESS)
			firmErrorAttention (status);
	}

	//------------------------------------------------------------
	// Acquisition Control Initialize
	//------------------------------------------------------------
	if ((status = acquisitionInit ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// AOI Initialize
	//------------------------------------------------------------
#if defined(MODE_AOI)
	if ((status = aoiInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// FFC初期化
	//------------------------------------------------------------
#if defined (MODE_FFC)
	if ((status = ipuFfcInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// DPC初期化
	//------------------------------------------------------------
#if defined (MODE_DPC)
	if ((status = ipuDpcInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// LUT初期化
	//------------------------------------------------------------
#if defined (MODE_LUT)
	if ((status = lutInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// Digital Offset Gain Initialize
	//------------------------------------------------------------
#if defined (MODE_DIGITAL_OFFSET_GAIN)
	if ((status = dogInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// Digital Band Offset Gain Initialize
	//------------------------------------------------------------
#if defined (MODE_SPECTRUM)
	if ((status = digitalBandOffsetGainInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// Digital IO Control Initialize
	//------------------------------------------------------------
	if ((status = digitalIoInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// Counter Control Initialize
	//------------------------------------------------------------
#if defined(MODE_GENICAM_COUNTER)
	if ((status = counterInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// Timer Control Initialize
	//------------------------------------------------------------
#if defined(MODE_GENICAM_TIMER)
	if ((status = timerControlInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// Encoder Control Initialize
	//------------------------------------------------------------
#if defined(MODE_ENCODER)
	if ((status = encoderInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif


#ifdef MODE_SPECTRUM
	//------------------------------------------------------------
	// Spectrum Initialize
	//------------------------------------------------------------
	if ((status = spectrumInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif


	//------------------------------------------------------------
	// Sensor Initialize
	//------------------------------------------------------------
	if ((status = sensorInitialize2 ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// Acquisition Control Initialize
	//------------------------------------------------------------
	if ((status = acquisitionInit2 ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// ROI Initialize
	//------------------------------------------------------------
	if ((status = roiInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	//------------------------------------------------------------
	// Digital Band Gain filter Initialize
	//------------------------------------------------------------
#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
	if ((status = bandGainFilterInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// Auto Bright Initialize
	//------------------------------------------------------------
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	if ((status = autoBrightInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);
#endif

	//------------------------------------------------------------
	// FrameRate High Speed Mode Initialize
	//------------------------------------------------------------
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	if ((status = sensorFrameRateHighSpeedModeInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	// High Speed Mode取得
	if ((status = sensorGetFrameRateHighSpeedMode (&hsModeNew)) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	if ((hsModeOld == MODE_DISABLE) && (hsModeNew == MODE_DISABLE))
	{
		// MODE_DISABLEならば何もしない
		// hsModeOld：電源投入時は常にDisable。loadコマンド時はloadコマンド前に使用していた値
		// hsModeNew：Usersetに保存されている値
	}
	else
	{
		// High Speed Mode設定
		if ((status = sensorSetFrameRateHighSpeedMode (hsModeNew)) != AVAL_STATUS_SUCCESS)
			firmErrorAttention (status);
	}
#endif


	//------------------------------------------------------------
	// CXP Image Paramter初期化
	//------------------------------------------------------------
#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// Boot Flag Check(1の場合、電源投入時ではない。usersetLoadコマンド実行等)
		if (IN32 (FIRM_DATA_FIRM_BOOT_FLAG_ADRS) == 1)
		{
			if ((status = cxpInitializeImageParam ()) != AVAL_STATUS_SUCCESS)
				firmErrorAttention (status);
		}
	}
#endif


	//------------------------------------------------------------
	// Peltier Power初期化
	//------------------------------------------------------------
#if defined (MODE_PELTIER_CTRL)
	// Boot Flag Check(1の場合、電源投入時ではない。usersetLoadコマンド実行等)
	if (IN32 (FIRM_DATA_FIRM_BOOT_FLAG_ADRS) == 1)
	{
		peltierPowerInitialize ();
	}
#endif

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	HW Normal初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int hwInitializeNormal (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Timing Generator初期化
	if ((status = timingGeneratorNormalInitalize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	// 特殊な初期化
	if ((status = specialInitialize ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

#if defined (MODE_ROI_VERSION2)
	// ROI セレクタ初期化
	OUT32 (FIRM_DATA_ROI_SELECTOR_ADRS, ROI_SELECTOR_MIN);

	// Width
	OUT32 (FIRM_DATA_ROI_X0_ADRS, DEFAULT_ROI_WIDTH0);

	// Height
	OUT32 (FIRM_DATA_ROI_Y0_ADRS, DEFAULT_ROI_HEIGHT0);
#endif

	// 取り込み停止
	acquisitionAbort ();

#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	// Set Exposure Max
	autoBrightSetExposureMax (1000);
#endif

	// フレームレート設定
	if ((status = acquisitionSetFrameRate (DEFAULT_FRAMERATE)) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	// Sensor Initialize
	if ((status = sensorInitialize2 ()) != AVAL_STATUS_SUCCESS)
		firmErrorAttention (status);

	return (AVAL_STATUS_SUCCESS);
}

// eof
