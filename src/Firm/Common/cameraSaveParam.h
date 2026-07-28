//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cameraSaveParam.h - Camera Parameter Header
//**********************************************************************************

//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// 工場出荷アドレス&データリスト
SAVE_PARAM_MODE saveParamDataList[] = {
	// レジスタアドレス:					データ:							モード:	 0=未使用
	//																		 1=Default値はレジスタから読み込み
    //																		 2=Default値はデータ領域に有効な値あり

	//------------------------------------------------------------------------------------------
	// Mark
	//------------------------------------------------------------------------------------------
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x00)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x08)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x10)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x18)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x20)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x28)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x30)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x38)

	//------------------------------------------------------------------------------------------
	// Reserved
	//------------------------------------------------------------------------------------------
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x40)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x48)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x50)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x58)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x60)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x68)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x70)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x78)


	//------------------------------------------------------------------------------------------
	// Peltier
	//------------------------------------------------------------------------------------------
#ifdef MODE_PELTIER
	{FPGA_PELTIER_TARGET_ADRS, 					PELTIER_DEFAULT_TEMP,		CAMERA_SAVE_MODE_DATA},			// センサターゲット温度(0x80)
#else
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x80)
#endif

	{FPGA_PELTIER_SENSOR_ALM_ADRS, 				((SENSOR_DEFAULT_HIGH_TEMP<<16) | (SENSOR_DEFAULT_LOW_TEMP & 0xffff)),	CAMERA_SAVE_MODE_DATA},		// センサ温度リミット(0x88)
	{FPGA_PELTIER_CASE_ALM_ADRS, 			 	((CASE_DEFAULT_HIGH_TEMP<<16)   | (CASE_DEFAULT_LOW_TEMP & 0xffff)),	CAMERA_SAVE_MODE_DATA},		// ケース温度リミット(0x90)
	{CAMERA_SAVE_RESERVE,		 				0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x98)

#if defined (MODE_PELTIER_CTRL)
	{FIRM_DATA_SENSOR_PERTIER_MODE,				0x01,						CAMERA_SAVE_MODE_DATA},			// センサペルチェモード(0xA0)
	{FIRM_DATA_PELTIER_POWER_LEVEL, 			PELTIER_POWER_LEVEL,		CAMERA_SAVE_MODE_DATA},			// ペルチェパワーモード(0xA8)
	{FPGA_PELTIER_CLIP_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// ペルチェHigh Clip(0xB0)
#else
	{CAMERA_SAVE_RESERVE,		 				0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA0)
	{CAMERA_SAVE_RESERVE,		 				0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA8)
	{CAMERA_SAVE_RESERVE,		 				0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB0)
#endif

	{CAMERA_SAVE_RESERVE,		 				0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xC0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xC8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xE0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xE8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xF0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xF8)

	
	//------------------------------------------------------------------------------------------
	// FFC
	//------------------------------------------------------------------------------------------
	{FPGA_FFC_CTRL_ADRS, 						FFC_DEFAULT_MODE,	 		CAMERA_SAVE_MODE_DATA},			// FFCイネーブル(0x100)
	{0x00, 										FLASH_FFC0_ADRS,		 	CAMERA_SAVE_MODE_WRITE_OTHER},	// FFC Flashアドレス(特殊)(0x108)
	{0x00, 										FFC_DEFAULT_NUMBER,		 	CAMERA_SAVE_MODE_WRITE_OTHER},	// FFC 番号(特殊)(0x110)
	{FIRM_DATA_FFC_CORECTION_MODE_ADRS, 		FFC_CORMODE_DEFAULT,		CAMERA_SAVE_MODE_DATA},			// FFC補正モード(0x118)

	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x120)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x128)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x130)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x138)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x140)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x148)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x150)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x158)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x160)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x168)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x170)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x178)


	//------------------------------------------------------------------------------------------
	// DPC
	//------------------------------------------------------------------------------------------
	{FPGA_DPC_CTRL_ADRS, 						DPC_DEFAULT_MODE, 			CAMERA_SAVE_MODE_DATA},			// DPCイネーブル(0x180)
	{FPGA_DPC_DMA_ADRS,		 					DPC_MEMORY_ADRS, 			CAMERA_SAVE_MODE_WRITE_OTHER},	// DPCアドレス(0x188)
	{FPGA_DPC_DMA_SIZE_ADRS, 					0x00, 						CAMERA_SAVE_MODE_NONE},			// DPCサイズ(0x190)
	{0x00,										FLASH_DPC_ADRS, 			CAMERA_SAVE_MODE_WRITE_OTHER},	// DPC Flashアドレス(特殊)(0x198)
	{0x00, 										DPC_FACTORY_NUMBER,			CAMERA_SAVE_MODE_WRITE_OTHER},	// DPC 番号(特殊)(0x1a0)

	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1a8)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1b0)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1b8)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1c0)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1c8)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1d0)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1d8)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1e0)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1e8)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1f0)
	{CAMERA_SAVE_RESERVE, 						0x00, 						CAMERA_SAVE_MODE_NONE},			// Reserve(0x1f8)

	//------------------------------------------------------------------------------------------
	// LUT
	//------------------------------------------------------------------------------------------
#if defined (MODE_LUT)
	{FPGA_LUT_CTRL_ADRS, 						0x00, 						CAMERA_SAVE_MODE_REG_REF},		// LUTイネーブル(0x200)
	{0x00, 										LUT_DEFAULT_BIN_THRESHOLD, 	CAMERA_SAVE_MODE_WRITE_OTHER},	// LUT1 2値化しきい値(特殊)(0x208)
	{0x00, 										LUT_DEFAULT_BIN_THRESHOLD, 	CAMERA_SAVE_MODE_WRITE_OTHER},	// LUT2 2値化しきい値(特殊)(0x210)
	{0x00, 										LUT_DEFAULT_FORMAT, 		CAMERA_SAVE_MODE_WRITE_OTHER},	// LUT1 Format(特殊)(0x218)
	{0x00, 										LUT_DEFAULT_FORMAT,			CAMERA_SAVE_MODE_WRITE_OTHER},	// LUT2 Format(特殊)(0x220)
	{0x00, 										LUT_DEFAULT_GAMMA, 			CAMERA_SAVE_MODE_WRITE_OTHER},	// LUT1 2値化ガンマ値(特殊)(0x228)
	{0x00, 										LUT_DEFAULT_GAMMA, 			CAMERA_SAVE_MODE_WRITE_OTHER},	// LUT2 2値化ガンマ値(特殊)(0x230)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x238)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x240)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x248)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x250)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x258)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x260)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x268)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x270)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x278)

#else //#if defined (MODE_LUT)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x200)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x208)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x210)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x218)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x220)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x228)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x238)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x240)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x248)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x250)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x258)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x260)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x268)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x270)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x278)

#endif //#if defined (MODE_LUT)


	//------------------------------------------------------------------------------------------
	// AOI
	//------------------------------------------------------------------------------------------
	{FPGA_AOI_BITWIDTH_ADRS,					DEFAULT_BIT_FPGA,			CAMERA_SAVE_MODE_DATA},			// Bit幅(0x280)
	{FIRM_DATA_WIDTH_ADRS, 						IMG_WIDTH,					CAMERA_SAVE_MODE_DATA},			// Xサイズ(0x288)
	{FIRM_DATA_HEIGHT_ADRS, 					IMG_HEIGHT,					CAMERA_SAVE_MODE_DATA},			// Yサイズ(0x290)
	{FIRM_DATA_BIT8_CONVERT_MODE, 				0x01,						CAMERA_SAVE_MODE_DATA},			// Bit8 Convert Mode(0x298)

#if defined (MODE_SENSOR_XFLIP)
	#if !defined (MODE_XFLIP_INVERT)
	{FPGA_XFLIP_CTRL_ADRS,		 				0x00,						CAMERA_SAVE_MODE_REG_REF},		// XFLIP(0x2a0)
	#else
	{FPGA_XFLIP_CTRL_ADRS,		 				0x01,						CAMERA_SAVE_MODE_DATA},			// XFLIP(0x2a0)
	#endif
#else // #if defined (MODE_SENSOR_XFLIP)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2a0)
#endif // #if defined (MODE_SENSOR_XFLIP)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2a8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2b0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2b8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2c0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2c8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2d0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2d8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2e0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2e8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2f0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x2f8)


	//------------------------------------------------------------------------------------------
	// Acquisition Control
	//------------------------------------------------------------------------------------------
	{GENICAM_ACQUISITION_MODE_ADRS,				DEFAULT_ACQUISITION_MODE,	CAMERA_SAVE_MODE_DATA},			// モード(0x300)
	{GENICAM_ACQUISITION_FRAME_COUNT_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// フレームカウント(0x308)
	{GENICAM_ACQUISITION_FRAMERATE_ADRS,		DEFAULT_FRAMERATE_TIME,		CAMERA_SAVE_MODE_DATA},			// フレームレート(0x310)
	{GENICAM_ACQUISITION_EXPOSURE_MODE_ADRS,	0x00,						CAMERA_SAVE_MODE_REG_REF},		// 露光制御モード(0x318)
	{GENICAM_ACQUISITION_EXPOSURE_ADRS,			DEFAULT_EXPOSURE_TIME,		CAMERA_SAVE_MODE_DATA},			// 露光時間(0x320)

	{FIRM_DATA_EXPOSURE_TIME, 					DEFAULT_EXPOSURE_TIME2,		CAMERA_SAVE_MODE_DATA},			// Exposure Time us単位(0x328)
	{FIRM_DATA_FRAME_RATE, 						DEFAULT_FRAMERATE_TIME2,	CAMERA_SAVE_MODE_DATA},			// FrameRate us単位(0x330)
	{GENICAM_ACQUISITION_TRG_RESERVE_MODE_ADRS,	0x00,						CAMERA_SAVE_MODE_REG_REF},		// Trigger Reserved(0x338)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x340)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x348)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x350)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x358)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x360)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x368)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x370)
	{GENICAM_ACQUISITION_TRG_SELECT_ADRS, 		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Trigger Selctor(0x378)
	
	{GENICAM_ACQUISITION_START_MODE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition Start Mode(0x380)
	{GENICAM_ACQUISITION_START_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition Start Source(0x388)
	{GENICAM_ACQUISITION_START_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition Start Activation(0x390)
	{GENICAM_ACQUISITION_START_DELAY_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition Start Delay(0x398)

	{GENICAM_ACQUISITION_END_MODE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition End Mode(0x3a0)
	{GENICAM_ACQUISITION_END_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition End Source(0x3a8)
	{GENICAM_ACQUISITION_END_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition End Activation(0x3b0)
	{GENICAM_ACQUISITION_END_DELAY_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition End Delay(0x3b8)

	{GENICAM_ACQUISITION_ACTIVE_MODE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition Active Mode(0x3c0)
	{GENICAM_ACQUISITION_ACTIVE_SOURCE_ADRS,	0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition Active Source(0x3c8)
	{GENICAM_ACQUISITION_ACTIVE_ACTIVE_ADRS,	0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition Active Activation(0x3d0)
	{GENICAM_ACQUISITION_ACTIVE_DELAY_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Acquisition Active Delay(0x3d8)

	{GENICAM_EXPOSURE_ACTIVE_MODE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Exposure Active Mode(0x3e0)
	{GENICAM_EXPOSURE_ACTIVE_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Exposure Active Source(0x3e8)
	{GENICAM_EXPOSURE_ACTIVE_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Exposure Active Activation(0x3f0)
	{GENICAM_EXPOSURE_ACTIVE_DELAY_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Exposure Active Delay(0x3f8)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x400)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x408)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x410)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x418)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x420)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x428)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x430)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x438)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x440)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x448)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x450)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x458)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x460)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x468)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x470)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x478)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x480)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x488)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x490)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x498)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4a0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4a8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4b0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4b8)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4c0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4c8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4d0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4d8)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4e0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4e8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4f0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x4f8)


#if defined (MODE_GENICAM_COUNTER)
	//------------------------------------------------------------------------------------------
	// Counter Control
	//------------------------------------------------------------------------------------------
	{GENICAM_COUNTER0_EVENT_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter0 Event Source(0x500)
	{GENICAM_COUNTER0_EVENT_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter0 Event Active(0x508)
	{GENICAM_COUNTER0_RESET_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter0 Reset Source(0x510)
	{GENICAM_COUNTER0_RESET_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter0 Reset Active(0x518)
//	{GENICAM_COUNTER0_VALUE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter0 Value
//	{GENICAM_COUNTER0_VALUE_AT_RESET_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter0 Value at Reset
	{GENICAM_COUNTER0_DURATION_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter0 Duration(0x520)
	{GENICAM_COUNTER0_TRG_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter0 Trg Source(0x528)
	{GENICAM_COUNTER0_TRG_ACTIVE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter0 Trg Active(0x530)

	{GENICAM_COUNTER1_EVENT_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter1 Event Source(0x538)
	{GENICAM_COUNTER1_EVENT_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter1 Event Active(0x540)
	{GENICAM_COUNTER1_RESET_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter1 Reset Source(0x548)
	{GENICAM_COUNTER1_RESET_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter1 Reset Active(0x550)
//	{GENICAM_COUNTER1_VALUE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter1 Value
//	{GENICAM_COUNTER1_VALUE_AT_RESET_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter1 Value at Reset
	{GENICAM_COUNTER1_DURATION_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter1 Duration(0x558)
	{GENICAM_COUNTER1_TRG_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter1 Trg Source(0x560)
	{GENICAM_COUNTER1_TRG_ACTIVE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter1 Trg Active(0x568)

	{GENICAM_COUNTER2_EVENT_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter2 Event Source(0x570)
	{GENICAM_COUNTER2_EVENT_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter2 Event Active(0x578)
	{GENICAM_COUNTER2_RESET_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter2 Reset Source(0x580)
	{GENICAM_COUNTER2_RESET_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter2 Reset Active(0x588)
//	{GENICAM_COUNTER2_VALUE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter2 Value
//	{GENICAM_COUNTER2_VALUE_AT_RESET_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter2 Value at Reset
	{GENICAM_COUNTER2_DURATION_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter2 Duration(0x590)
	{GENICAM_COUNTER2_TRG_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter2 Trg Source(0x598)
	{GENICAM_COUNTER2_TRG_ACTIVE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter2 Trg Active(0x5a0)

	{GENICAM_COUNTER3_EVENT_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter3 Event Source(0x5a8)
	{GENICAM_COUNTER3_EVENT_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter3 Event Active(0x5b0)
	{GENICAM_COUNTER3_RESET_SOURCE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter3 Reset Source(0x5b8)
	{GENICAM_COUNTER3_RESET_ACTIVE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter3 Reset Active(0x5c0)
//	{GENICAM_COUNTER3_VALUE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter3 Value
//	{GENICAM_COUNTER3_VALUE_AT_RESET_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter3 Value at Reset(0x5c8)
	{GENICAM_COUNTER3_DURATION_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter3 Duration(0x5d0)
	{GENICAM_COUNTER3_TRG_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter3 Trg Source(0x5d8)
	{GENICAM_COUNTER3_TRG_ACTIVE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter3 Trg Active(0x5e0)

	{GENICAM_COUNTER_SELECT_ADRS, 				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Counter Selector(0x5e0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5e8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5f0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5f8)

#else

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x500)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x508)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x510)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x518)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x520)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x528)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x530)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x538)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x540)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x548)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x550)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x558)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x560)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x568)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x570)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x578)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x580)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x588)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x590)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x598)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5a0)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5a8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5b0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5b8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5c0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5c8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5d0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5d8)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5e0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5e8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5f0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x5f8)
#endif


	//------------------------------------------------------------------------------------------
	// Timer Control
	//------------------------------------------------------------------------------------------
#if defined (MODE_GENICAM_TIMER)
	{GENICAM_TIMER0_DURATION_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer0 Duration(0x600)
	{GENICAM_TIMER0_DELAY_ADRS,					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer0 Delay(0x608)
//	{GENICAM_TIMER0_VALUE_ADRS,					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer0 Value
	{GENICAM_TIMER0_SOURCE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer0 Source(0x610)
	{GENICAM_TIMER0_ACTIVE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer0 Activation(0x618)

	{GENICAM_TIMER1_DURATION_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer1 Duration(0x620)
	{GENICAM_TIMER1_DELAY_ADRS,					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer1 Delay(0x628)
//	{GENICAM_TIMER1_VALUE_ADRS,					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer1 Value
	{GENICAM_TIMER1_SOURCE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer1 Source(0x630)
	{GENICAM_TIMER1_ACTIVE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer1 Activation(0x638)

	{GENICAM_TIMER2_DURATION_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer2 Duration(0x640)
	{GENICAM_TIMER2_DELAY_ADRS,					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer2 Delay(0x648)
//	{GENICAM_TIMER2_VALUE_ADRS,					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer2 Value
	{GENICAM_TIMER2_SOURCE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer2 Source(0x650)
	{GENICAM_TIMER2_ACTIVE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer2 Activation(0x658)

	{GENICAM_TIMER3_DURATION_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer3 Duration(0x660)
	{GENICAM_TIMER3_DELAY_ADRS,					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer3 Delay(0x668)
//	{GENICAM_TIMER3_VALUE_ADRS,					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer3 Value
	{GENICAM_TIMER3_SOURCE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer3 Source(0x670)
	{GENICAM_TIMER3_ACTIVE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Timer3 Activation(0x678)

#else

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x600)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x608)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x610)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x618)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x620)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x628)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x630)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x638)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x640)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x648)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x650)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x658)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x660)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x668)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x670)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x678)
#endif


	//------------------------------------------------------------------------------------------
	// Encoder Control
	//------------------------------------------------------------------------------------------
#if defined (MODE_ENCODER)
	{GENICAM_ENCODER_0_PASEA_TRG_SOURCE_ADRS,	0x00,						CAMERA_SAVE_MODE_REG_REF},		// PhaseA Trg Source(0x680)
	{GENICAM_ENCODER_0_PASEB_TRG_SOURCE_ADRS,	0x00,						CAMERA_SAVE_MODE_REG_REF},		// PhaseB Trg Source(0x688)
	{GENICAM_ENCODER_0_MODE_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Mode(0x690)
	{GENICAM_ENCODER_0_DVIDER_ADRS,				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Divider(0x698)
	{GENICAM_ENCODER_0_OUTPUT_MODE_ADRS,		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Output Mode(0x6a0)
	{GENICAM_ENCODER_0_TIMEOUT_ADRS,			DEFAULT_ENCODER_TIMEOUT,	CAMERA_SAVE_MODE_DATA},			// TimeOut(0x6a8)
	{GENICAM_ENCODER_0_RESET_TRG_SOURCE_ADRS,	0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reset Trg Source(0x6b0)
	{GENICAM_ENCODER_0_RESET_ACTIVATION_ADRS,	0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reset Trg Activation(0x6b8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6c0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6c8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6d0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6d8)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6e0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6e8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6f0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6f8)

#else

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x680)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x688)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x690)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x698)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6a0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6a8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6b0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6b8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6c0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6c8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6d0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6d8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6e0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6e8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6f0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x6f8)

#endif


	//------------------------------------------------------------------------------------------
	// Digital IO
	//------------------------------------------------------------------------------------------
#if defined (MODE_GENICAM_DIGITALIO)

	// Line0
	{GENICAM_DIGITAL_LINE0_MODE_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line0 Mode(0x700)
	{GENICAM_DIGITAL_LINE0_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line0 Inverter(0x708)
	{GENICAM_DIGITAL_LINE0_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line0 Source(0x710)
	{GENICAM_DIGITAL_LINE0_FORMAT_ADRS, 		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line0 Format(0x718)
	{GENICAM_DIGITAL_LINE0_DNF_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line0 Dnf(0x720)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x728)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x730)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x738)

	// Line1
	{GENICAM_DIGITAL_LINE1_MODE_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line1 Mode(0x740)
	{GENICAM_DIGITAL_LINE1_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line1 Inverter(0x748)
	{GENICAM_DIGITAL_LINE1_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line1 Source(0x750)
	{GENICAM_DIGITAL_LINE1_FORMAT_ADRS, 		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line1 Format(0x758)
	{GENICAM_DIGITAL_LINE1_DNF_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line1 Dnf(0x760)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x768)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x770)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x778)

	// Line2
	{GENICAM_DIGITAL_LINE2_MODE_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line2 Mode(0x780)
	{GENICAM_DIGITAL_LINE2_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line2 Inverter(0x788)
	{GENICAM_DIGITAL_LINE2_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line2 Source(0x790)
	{GENICAM_DIGITAL_LINE2_FORMAT_ADRS, 		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line2 Format(0x798)
	{GENICAM_DIGITAL_LINE2_DNF_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line2 Dnf(0x7A0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x7A8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x7B0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x7B8)

	// Line3
	{GENICAM_DIGITAL_LINE3_MODE_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line3 Mode(0x7C0)
	{GENICAM_DIGITAL_LINE3_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line3 Inverter(0x7C8)
	{GENICAM_DIGITAL_LINE3_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line3 Source(0x7D0)
	{GENICAM_DIGITAL_LINE3_FORMAT_ADRS, 		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line3 Format(0x7D8)
	{GENICAM_DIGITAL_LINE3_DNF_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line3 Dnf(0x7E0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x7E8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x7F0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x7F8)

	// Line4
	{GENICAM_DIGITAL_LINE4_MODE_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line4 Mode(0x800)
	{GENICAM_DIGITAL_LINE4_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line4 Inverter(0x808)
	{GENICAM_DIGITAL_LINE4_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line4 Source(0x810)
	{GENICAM_DIGITAL_LINE4_FORMAT_ADRS, 		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line4 Format(0x818)
	{GENICAM_DIGITAL_LINE4_DNF_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line4 Dnf(0x820)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x828)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x830)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x838)

	// Line5
	{GENICAM_DIGITAL_LINE5_MODE_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line5 Mode(0x840)
	{GENICAM_DIGITAL_LINE5_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line5 Inverter(0x848)
	{GENICAM_DIGITAL_LINE5_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line5 Source(0x850)
	{GENICAM_DIGITAL_LINE5_FORMAT_ADRS, 		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line5 Format(0x858)
	{GENICAM_DIGITAL_LINE5_DNF_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line5 Dnf(0x860)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x868)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x870)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x878)

	// Line6
	{GENICAM_DIGITAL_LINE6_MODE_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line6 Mode(0x980)
	{GENICAM_DIGITAL_LINE6_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line6 Inverter(0x988)
	{GENICAM_DIGITAL_LINE6_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line6 Source(0x990)
	{GENICAM_DIGITAL_LINE6_FORMAT_ADRS, 		0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line6 Format(0x998)
	{GENICAM_DIGITAL_LINE6_DNF_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line6 Dnf(0x9A0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9A8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9B0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9B8)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9C0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9C8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9D0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9D8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9E0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9E8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9F0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9F8)

	// CC
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA00)
	{GENICAM_DIGITAL_CC1_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// CC1 Inverter(0xA08)
	{GENICAM_DIGITAL_CC1_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// CC1 Source(0xA10)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA18)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA20)
	{GENICAM_DIGITAL_CC2_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// CC2 Inverter(0xA28)
	{GENICAM_DIGITAL_CC2_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// CC2 Source(0xA30)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA38)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA40)
	{GENICAM_DIGITAL_CC3_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// CC3 Inverter(0xA48)
	{GENICAM_DIGITAL_CC3_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// CC3 Source(0xA50)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA58)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA60)
	{GENICAM_DIGITAL_CC4_INVERT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// CC4 Inverter(0xA68)
	{GENICAM_DIGITAL_CC4_SOURCE_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// CC4 Source(0xA70)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA78)

	// User Outout
	{GENICAM_DIGITAL_USER_MASK_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// User Mask(0xA80)
	{GENICAM_DIGITAL_USER0_OUTPUT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// User0 Output(0xA88)
	{GENICAM_DIGITAL_USER1_OUTPUT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// User1 Output(0xA90)
	{GENICAM_DIGITAL_USER2_OUTPUT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// User2 Output(0xA98)
	{GENICAM_DIGITAL_USER3_OUTPUT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// User3 Output(0xAA0)
	{GENICAM_DIGITAL_USER4_OUTPUT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// User4 Output(0xAA8)
	{GENICAM_DIGITAL_USER5_OUTPUT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// User5 Output(0xAB0)
	{GENICAM_DIGITAL_USER6_OUTPUT_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// User6 Output(0xAB8)

	{GENICAM_DIGITAL_LINE_SELECT_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line Selector(0xAC0)
	{GENICAM_DIGITAL_USER_SELECT_ADRS, 			0x00,						CAMERA_SAVE_MODE_REG_REF},		// User Selector(0xAC8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAD0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAD8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAE0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAE8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAF0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAF8)

#else // #if defined (MODE_GENICAM_DIGITALIO)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x700)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x708)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x710)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x718)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x720)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x728)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x730)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x738)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x740)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x748)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x750)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x758)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x750)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x758)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x760)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x768)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x770)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x778)
	
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x780)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x788)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x790)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x798)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7a0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7a8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7b0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7b8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7c0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7c8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7d0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7d8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7e0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7e8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7f0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x7f8)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x800)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x808)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x810)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x818)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x820)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x828)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x830)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x838)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x840)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x848)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x850)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x858)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x860)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x868)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x870)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x878)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x880)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x888)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x890)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x898)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8a0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8a8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8b0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8b8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8c0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8c8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8d0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8d8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8e0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8e8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8f0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x8f8)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x900)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x908)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x910)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x918)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x920)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x928)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x930)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x938)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x940)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x948)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x950)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x958)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x960)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x968)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x970)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x978)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x980)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x988)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x990)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x998)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9a0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9a8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9b0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9b8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9c0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9c8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9d0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9d8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9e0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9e8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9f0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0x9f8)

#endif // #if defined (MODE_GENICAM_DIGITALIO)


	//------------------------------------------------------------------------------------------
	// ROI
	//------------------------------------------------------------------------------------------
#if defined (MODE_ROI_VERSION2)
	{FPGA_ROI_CAMERA_Y0_ADRS, 					DEFAULT_ROI_HEIGHT0,		CAMERA_SAVE_MODE_DATA},			// Height0(FPGA/Sensor)(0xA00)
	{FPGA_ROI_CAMERA_Y1_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Height1(FPGA/Sensor)(0xA08)
	{FPGA_ROI_CAMERA_Y2_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Height2(FPGA/Sensor)(0xA10)
	{FPGA_ROI_CAMERA_Y3_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Height3(FPGA/Sensor)(0xA18)
	{FPGA_ROI_CAMERA_Y4_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Height4(FPGA/Sensor)(0xA20)
	{FPGA_ROI_CAMERA_Y5_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Height5(FPGA/Sensor)(0xA28)
	{FPGA_ROI_CAMERA_Y6_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Height6(FPGA/Sensor)(0xA30)
	{FPGA_ROI_CAMERA_Y7_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Height7(FPGA/Sensor)(0xA38)

	{FIRM_DATA_ROI_Y0_ADRS, 					DEFAULT_ROI_HEIGHT0,		CAMERA_SAVE_MODE_DATA},			// Height0(Memory)(0xA40)
	{FIRM_DATA_ROI_Y1_ADRS, 					0x00,						CAMERA_SAVE_MODE_DATA},			// Height1(Memory)(0xA48)
	{FIRM_DATA_ROI_Y2_ADRS, 					0x00,						CAMERA_SAVE_MODE_DATA},			// Height2(Memory)(0xA50)
	{FIRM_DATA_ROI_Y3_ADRS, 					0x00,						CAMERA_SAVE_MODE_DATA},			// Height3(Memory)(0xA58)
	{FIRM_DATA_ROI_Y4_ADRS, 					0x00,						CAMERA_SAVE_MODE_DATA},			// Height4(Memory)(0xA60)
	{FIRM_DATA_ROI_Y5_ADRS, 					0x00,						CAMERA_SAVE_MODE_DATA},			// Height5(Memory)(0xA68)
	{FIRM_DATA_ROI_Y6_ADRS, 					0x00,						CAMERA_SAVE_MODE_DATA},			// Height6(Memory)(0xA70)
	{FIRM_DATA_ROI_Y7_ADRS, 					0x00,						CAMERA_SAVE_MODE_DATA},			// Height7(Memory)(0xA78)

	#if !defined (MODE_IPU_MULTI)
	{FPGA_ROI_CAMERA_X0_ADRS, 					DEFAULT_ROI_WIDTH0,			CAMERA_SAVE_MODE_DATA},			// Width0(FPGA/Sensor)(0xA80)
	#else // #if !defined (MODE_IPU_MULTI)
	{FPGA_HORIZONTAL_ROI_ADRS, 					DEFAULT_ROI_WIDTH0,			CAMERA_SAVE_MODE_DATA},			// Width0(FPGA/Sensor)(0xA80)
	#endif

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA88)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA90)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA98)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAA0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAA8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAB0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAB8)

	{FIRM_DATA_ROI_X0_ADRS, 					DEFAULT_ROI_WIDTH0,			CAMERA_SAVE_MODE_DATA},			// Width0(Memory)(0xAC0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAC8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAD50)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAD8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAE0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAE8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAF0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAF8)

#else

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA00)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA08)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA10)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA18)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA20)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA28)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA30)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA38)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA40)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA48)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA50)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA58)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA60)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA68)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA70)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA78)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA80)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA88)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA90)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xA98)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAA0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAA8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAB0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAB8)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAC0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAC8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAD0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAD8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAE0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAE8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAF0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xAF8)
	
#endif // #if defined (MODE_ROI_VERSION2)


	//------------------------------------------------------------------------------------------
	// Auto Bright
	//------------------------------------------------------------------------------------------
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	{FIRM_DATA_AUTO_BRIGHT_EXP_MODE_ADRS, 		DEFAULT_AUTO_BRIGHT_MODE,	CAMERA_SAVE_MODE_DATA},			// Exposure Mode(0xB00)
	{FPGA_AUTO_BRIGHT_CTRL_ADRS, 				DEFAULT_AUTO_BRIGHT_TARGET,	CAMERA_SAVE_MODE_DATA},			// Target Bright(0xB08)
	{FPGA_AUTO_BRIGHT_OFFSET_ADRS, 				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Offset(0xB10)
	//{FPGA_AUTO_BRIGHT_SIZE_ADRS, 				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Size(0xB18)
	{FPGA_AUTO_BRIGHT_SIZE_ADRS, 				DEFAULT_AUTO_WIDTH_HEIGHT,	CAMERA_SAVE_MODE_DATA},			// Size(0xB18)
	{FIRM_DATA_AUTO_BRIGHT_EXPOSUER_MIN, 		DEFAULT_AUTO_BRIGHT_EXP_MIN,CAMERA_SAVE_MODE_DATA},			// ExposureMin(0xB20)
	{FIRM_DATA_AUTO_BRIGHT_EXPOSUER_MAX, 		DEFAULT_AUTO_BRIGHT_EXP_MAX,CAMERA_SAVE_MODE_DATA},			// ExposureMax(0xB28)
	{FIRM_DATA_AUTO_BRIGHT_GAIN_MODE_ADRS, 		DEFAULT_AUTO_BRIGHT_MODE,	CAMERA_SAVE_MODE_DATA},			// Gain Mode(0xB30)
	{FIRM_DATA_AUTO_BRIGHT_GAIN_MIN, 			DEFAULT_AUTO_BRIGHT_GAIN_MIN,CAMERA_SAVE_MODE_DATA},		// GainMin(0xB38)
	{FIRM_DATA_AUTO_BRIGHT_GAIN_MAX,	 		DEFAULT_AUTO_BRIGHT_GAIN_MAX,CAMERA_SAVE_MODE_DATA},		// GaineMax(0xB40)
	{FIRM_DATA_AUTO_BRIGHT_TARGET_AREA, 	 	DEFAULT_AUTO_BRIGHT_TAEGET_AREA,CAMERA_SAVE_MODE_DATA},		// Target Area(0xB48)
	{FIRM_DATA_AUTO_BRIGHT_USERSET_SIZE, 		0x00,						CAMERA_SAVE_MODE_DATA},			// Size(0xB50)
	{FIRM_DATA_AUTO_BRIGHT_USERSET_OFFSET, 		0x00,						CAMERA_SAVE_MODE_DATA},			// Offset(0xB58)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xB60)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xB68)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xB70)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xB78)
#else
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB00)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB08)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB10)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB18)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB20)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB28)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB30)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB38)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB40)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB48)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB50)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB58)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB60)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB68)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB70)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xB78)
#endif

	//------------------------------------------------------------------------------------------
	// Digital Offset Gain
	//------------------------------------------------------------------------------------------
	{FPGA_DOG_CTRL_ADRS,						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Offset/Gainイネーブル(0xB80)
	{FPGA_DOG_OFFSET1_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Offset 1st(0xB88)
	{FPGA_DOG_GAIN_ADRS, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Gain(0xB90)
	{FPGA_DOG_OFFSET2_ADRS,		 				0x00,						CAMERA_SAVE_MODE_REG_REF},		// Offset 2nd(0xB98)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xBA0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xBA8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xBB0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xBB8)

	//------------------------------------------------------------------------------------------
	// Timing Generator
	//------------------------------------------------------------------------------------------
	{FPGA_TG_TGSE_ADRS,							DEFAULT_TG_TGSE,			CAMERA_SAVE_MODE_DATA},			// TGSE(0xBC0)
	{FPGA_TG_TGES_ADRS,							DEFAULT_TG_TGES,			CAMERA_SAVE_MODE_DATA},			// TGES(0xBC8)
	{FPGA_TG_TGPD_ADRS,							DEFAULT_TG_TGPD,			CAMERA_SAVE_MODE_DATA},			// TGPD(0xBD0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xBD8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xBE8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xBE8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xBF8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xBF8)
	
	//------------------------------------------------------------------------------------------
	// Bining
	//------------------------------------------------------------------------------------------
#if defined (MODE_BINNING)
	{FPGA_BINNING_CTRL_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Binning(0xC00)
	{FPGA_BINNING_DIVIDE_ADRS, 					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Binning Divide(0xC08)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC10)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC18)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC20)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC28)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC30)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC38)
#else
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC00)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC08)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC10)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC18)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC20)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC28)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC30)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC38)
#endif
	
	//------------------------------------------------------------------------------------------
	// Gradation Compress
	//------------------------------------------------------------------------------------------
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	{FIRM_DATA_GRADATION_COMPRESS_POS_FIRST,	GC_POS_FIRST_DEFAULT,		CAMERA_SAVE_MODE_DATA},			// Gradation Compress Position First(0xC40)
	{FIRM_DATA_GRADATION_COMPRESS_POS_SECOND,	GC_POS_SECOND_DEFAULT,		CAMERA_SAVE_MODE_DATA},			// Gradation Compress Position Second(0xC48)
	{FIRM_DATA_GRADATION_COMPRESS_GAIN_FIRST,	GC_GAIN_FIRST_DEFAULT,		CAMERA_SAVE_MODE_DATA},			// Gradation Compress Gain First(0CF50)
	{FIRM_DATA_GRADATION_COMPRESS_GAIN_SECOND,	GC_GAIN_SECOND_DEFAULT,		CAMERA_SAVE_MODE_DATA},			// Gradation Compress Gain Second(0xC58)
	{FIRM_DATA_GRADATION_COMPRESS_MODE, 		GC_MODE_DEFAULT,			CAMERA_SAVE_MODE_DATA},			// Gradation Compress Mode(0xC60)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC68)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC70)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC78)
#else
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC40)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC48)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC50)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC58)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC60)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC68)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC70)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC78)
#endif
	
	//------------------------------------------------------------------------------------------
	// Sensor
	//------------------------------------------------------------------------------------------
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	{FIRM_DATA_SENSOR_CONVERSION_GAIN_ADRS, 	0x00,						CAMERA_SAVE_MODE_DATA},			// Sensor Conversion Gain(0xC80)
	{FIRM_DATA_SENSOR_DRRS_MODE, 				0x00,						CAMERA_SAVE_MODE_DATA},			// DRRS(0xC88)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC90)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC98)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCA0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCA8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCB0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCB8)
#else
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC80)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC88)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC90)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xC98)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCA0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCA8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCB0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCB8)
#endif

	//------------------------------------------------------------------------------------------
	// High Speed Mode
	//------------------------------------------------------------------------------------------
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	{FIRM_DATA_FRAME_RATE_HIGH_SPEED_MODE_ADRS,	0x00,						CAMERA_SAVE_MODE_DATA},			// High Speed Mode(0xCC0)
	{FIRM_DATA_FRAME_RATE_HIGH_SPEED_HEIGHT_ADRS, FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_DEFAULT, CAMERA_SAVE_MODE_DATA},// High Speed Height(0xCC8)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCD0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCD8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCE0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCE8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCF0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCF8)
#else
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCC0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCC8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCD0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCD8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCE0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCE8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCF0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xCF8)
#endif

	//------------------------------------------------------------------------------------------
	// GE
	//------------------------------------------------------------------------------------------
#if defined (MODE_GE_SPEED)
	{FIRM_DATA_GEV_SPEED_ADRS, 					GIGE_SPEED_AUTO_NEGOTIATION,CAMERA_SAVE_MODE_DATA},			// Reserved(0xD00)
#else	
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xD00)
#endif
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xD08)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xD10)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xD18)

	//------------------------------------------------------------------------------------------
	// CXP
	//------------------------------------------------------------------------------------------
#if defined (MODE_CXP)
	{FIRM_DATA_CXP_COMPLIANCE_TEST_MODE_ADRS,	0x00,						CAMERA_SAVE_MODE_DATA},			// Copmliance Test(0xD20)
	{FIRM_DATA_CXP_CONNECTION_CONFIG,	DEFAULT_CONNECTION_CONFIG_DEFAULT,	CAMERA_SAVE_MODE_DATA},			// Copmliance Test(0xD28)
#else
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xF20)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0xD28)
#endif


	//------------------------------------------------------------------------------------------
	// Reserved
	//------------------------------------------------------------------------------------------
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD30)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD38)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD40)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD48)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD50)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD58)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD60)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD68)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD70)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserve(0xD78)

	//------------------------------------------------------------------------------------------
	// Spectrum
	//------------------------------------------------------------------------------------------
#if defined (MODE_SPECTRUM)

	{FPGA_SPECTRUM_CTRL_ADRS,					DEFAULT_SPECTRUM_CTRL,		CAMERA_SAVE_MODE_DATA},			// Control(0xD80)
	{FPGA_SPECTRUM_LINE_PER_FRAME_ADRS,			0x00,						CAMERA_SAVE_MODE_REG_REF},		// Line Per Frame(0xD88)
	{FPGA_SPECTRUM_BAND_ADRS,					0x00,						CAMERA_SAVE_MODE_REG_REF},		// Band Count(0xD90)

	#if defined (MODE_USERSET_SPECTRUM_PARAM)
	{FPGA_BGF_CTRL_ADRS,						0x00,						CAMERA_SAVE_MODE_REG_REF},      // BandGainFilter(0xD98)
	#else
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_NONE},			// Reserved(0x898)
	#endif

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDA0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDA8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDB0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDB8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDC0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDC8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDD0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDD8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDE0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDE8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDF0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDF8)

#else // #if defined (MODE_SPECTRUM)

	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xD80)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xD88)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xD90)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xD98)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDA0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDA8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDB0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDB8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDC0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDC8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDD0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDD8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDE0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDE8)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDF0)
	{CAMERA_SAVE_RESERVE, 						0x00,						CAMERA_SAVE_MODE_REG_REF},		// Reserve(0xDF8)

#endif // #if defined (MODE_SPECTRUM)

};

// eof
