//**********************************************************************************
//
//                           Camera Project
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// ABA052VIR2.h - Camera Header
//**********************************************************************************

#ifndef __ABA052VIR2_H__
#define __ABA052VIR2_H__

//----------------------------------------------------------------------------------
// バージョン
//----------------------------------------------------------------------------------

// Firmware Version
#define FIRM_VERSION						"0.4"

// Main Version
#define MAIN_VERSION						"0.4"

// HW Version
#define HW_VERSION							"0.4"


//----------------------------------------------------------------------------------
// Mode
//----------------------------------------------------------------------------------
#define MODE_PELTIER						// Peltier Mode
#define MODE_PELTIER_VOLT_CURRENT			// Peltier Voltage Current Mode
#define MODE_PELTIER_CTRL					// Peltier Control
#define MODE_TEMP							// Temp Mode
#define MODE_TEMP_ABNORMAL_CHECK			// Temp Abnormal Check
#define MODE_FFC_BIT_CALC					// FFC Bit Calc Mode
#define MODE_FFC_INFO_GAIN					// FFC Information Gain Mode
#define MODE_FFC_INFO_BIT					// FFC Information Bit Mode
//#define MODE_FFC_SHADING_DIGITAL_GAIN		// FFC Shading Digital Gain Mode
#define MODE_FFC_MEM_EXTERNAL_MALLOC		// FFC External Memory Malloc
//#define MODE_SPECTRUM						// Spectrum
//#define MODE_SPECTRUM_MULTI_HEIGHT		// Spectrum Multi Height
//#define MODE_SPECTRUM_BAND_SELECT			// Spectrum Band Select
//#define MODE_SPECTRUM_BANDGAIN_FILTER		// Spectrum BandGain Filter
#define MODE_ENCODER						// Encoder
#define MODE_I2C							// I2C Mode
#define MODE_FFC							// Flat Field Correction Mode
#define MODE_FFC_FIRST						// Flat Field Correction First Mode
//#define MODE_FFC_SHADING_LINE				// Flat Field Correction Shading Mode
#define MODE_FFC_DATA_ALIGN_ADJUST			// Flat Field Correction Data Align Adjust
#define MODE_DPC							// Defective Pixel Correction Mode
//#define MODE_DPC_ADJUST_MODE				// Defective Pixel Correction Adjust Mode
#define MODE_DPC_DATA_ALIGN_ADJUST			// Defective Pixel Correction Data Align Adjust
#define MODE_DPC_GRID_UPDATE				// Defective Pixel Correction Grid Update
//#define MODE_DPC_NONUNIFORM_SPECTRUM_MODE	// Defective Pixel Correction NonUniform Spectrum Mode
#define MODE_DPC_MEM_EXTERNAL_MALLOC		// Defective Pixel Correction External Memory Malloc
#define MODE_LUT							// Lookup Table
#define MODE_DIGITAL_OFFSET_GAIN			// Digital Offset Gain
#define MODE_GENICAM_DIGITALIO				// Genicam Digital IO
#define MODE_GENICAM_COUNTER				// Genicam Counter
#define MODE_GENICAM_TIMER					// Genicam Timer
//#define MODE_CAMERA_INTERRUPT				// Camera Interrupt Mode
//#define MODE_USERSET_SPECTRUM_PARAM		// Userset Spectrum Parameter Mode
#define MODE_QSPI_FLASH_DUAL_STACK			// Qspi Flash Dual Stack Mode
//#define MODE_ACQUISITION_TRG_EXTEND		// Acquisition Trigger Extend
//#define MODE_FLASH_PROTECT_NEW			// Flash Protect New Mode
#define MODE_FLASH_PROTECT_SIMPLE			// Flash Protect Simple
#define MODE_ROI_VERSION2					// ROI Version2 Mode
#define MODE_VOLTAGE_FPGA_BOARD				// FPGA Board Voltage Mode
#define MODE_VOLTAGE_IF_BOARD				// IF Board Voltage Mode
//#define MODE_VOLTAGE_SUB_BOARD			// Sub Board Voltage Mode
//#define MODE_VOLTAGE_SENSOR_BOARD			// Sensor Board Voltage Mode
#define MODE_AOI							// AOI Mode
#define MODE_TRG_RESERVED_ADJUST			// Trigger Reserved Adjust
#define MODE_GE_PACKET_SIZE_CUSTOM			// GigE Packet Size Custom
//#define MODE_GE_PACKET_INFO_SAVE			// GigE Packet Information Save
//#define MODE_GE_PACKET_SIZE_SAVE			// GigE Packet Size
//#define MODE_GE_PACKET_DELAY_SAVE			// GigE Packet Delay
#define MODE_GE_SPEED						// GigE Speed
#define MODE_SENSOR_XFLIP					// X Flip Function Mode
//#define MODE_XFLIP_INVERT					// X Flip Invert Mode
#define MODE_AUTO_EXPOSURE					// Auto Exposure
#define MODE_AUTO_GAIN						// Auto Gain
#define MODE_CAMERA_EVENT_VERSION2			// Camera Event Version2
#define MODE_PELTIER_MOUNTING_SWITCH		// Peltier Mounting Switch Mode
#define MODE_SENSOR_GRADATION_COMPRESS		// Sensor Gradation Compress
#define MODE_BIT8_GCMODE_ENABLE				// Default 8bit Gradation Compress
#define MODE_CXP							// Cxp Mode
#define MODE_CXP_MULTI_PORT					// Cxp Multi Port
#define MODE_CXP_VERSION_20					// Cxp Version 2.0
#define MODE_CXP_RATE_CHANGE				// Cxp Rate Change
//#define MODE_BOARD_ACB523GE				// GE  Board ACB-523GE_IF (ABA-013VIR/ABA-003VIR)
#define MODE_BOARD_ACB532GE					// GE  Board ACB-532GE_IF (ABA-052VIR/ABA-032VIR)
//#define MODE_BOARD_ACB525CXP				// CXP Board ACB-525CXP_IF(ABA-013VIR/ABA-003VIR)
#define MODE_BOARD_ACB531CXP				// CXP Board ACB-531CXP_IF(ABA-052VIR/ABA-032VIR)
#define MODE_UPDATE_INTERFACE				// InterfaceBoard Update
#define MODE_FPGA_PF						// InterfaceBoard FPGA TYPE PolarFire
#define MODE_BINNING						// Binning Mode
#define MODE_ACQUISITION_TRG_SOFT_COUNT		// Acquisition Trg Soft Count
#define MODE_FRAMERATE_HIGH_SPEED			// Frame Rate High Speed
#define MODE_XML_SCHEMA_VERSION				// XML SchemaVersion
//#define MODE_SENSOR_MASTER				// Sensor Mode
#define MODE_SENSOR_SHUTTER					// Sensor Shutter Mode
#define MODE_SENSOR_DRRS					// Sensor DRRS Mode
//#define MODE_SENSOR_IMX990				// Sensor Module IMX990
//#define MODE_SENSOR_IMX991				// Sensor Module IMX991
#define MODE_SENSOR_IMX992					// Sensor Module IMX992
//#define MODE_SENSOR_IMX993				// Sensor Module IMX993
#define MODE_SYSTEM_MANAGEMENT				// System Management Support
#define MODE_IPU_MULTI						// IPU Multi Mode
//#define MODE_MULTI_STREAM					// Multi Stream
#define MODE_GIGE_10G						// GigE 10G
//#define MODE_IEEE1588_PTP					// IEE1588 PTP
#define MODE_LUT_NUMBER_FIX					// Lut Number Fix Mode


//----------------------------------------------------------------------------------
// Bit数
//----------------------------------------------------------------------------------
#define PIXEL_BIT_MIN						(PIXEL_8BIT)
#define PIXEL_BIT_MAX						(PIXEL_12BIT)

#define MODE_CAMERA_BIT						(12)
#define MODE_FFC_BIT						(12)
#define MODE_DPC_BIT						(12)
#define MODE_BRIGHT_MAX						(4095)
#define MODE_BRIGHT_MIN						(4095)


//----------------------------------------------------------------------------------
// Sensor 
//----------------------------------------------------------------------------------
#define MODE_NEAR_INFRARED					// Near Infrared Sensor
//#define MODE_FAR_INFRARED					// Far Infrared Sensor
#define MODE_SENSOR_VENDOR					(SENSOR_VENDOR_S)


//----------------------------------------------------------------------------------
// Camera Type
//----------------------------------------------------------------------------------
#define CAMERA_TYPE							(CAMERA_TYPE_AREA)


//----------------------------------------------------------------------------------
// Variant Type
//----------------------------------------------------------------------------------
#define DEVICE_VARIANT_TYPE					(VARIANT_NORMAL)


//----------------------------------------------------------------------------------
// Interface HW Type
//----------------------------------------------------------------------------------
#define IF_HW_TYPE							(CAMERA_TYPE_AREA)


//----------------------------------------------------------------------------------
// Camera ID
//----------------------------------------------------------------------------------
#define CAMERA_ID							CAMERA_ID_AXP_ABA052VIR2_01

//----------------------------------------------------------------------------------
// 製品情報
//----------------------------------------------------------------------------------

// Model Name
#define MODEL_NAME_CAMERALINK				"ABA-052VIR2-CL"
#define MODEL_NAME_GIGE						"ABA-052VIR2-GE"
#define MODEL_NAME_CXP						"ABA-052VIR2-CXP"

// Firmware Name
#define FIRM_NAME							"AXP-ABA052VIR2-01"

// IF Board Name
#define IF_CXP_BOARD_NAME					"ACB-534IF-CXP"


//----------------------------------------------------------------------------------
// Camera Information
//----------------------------------------------------------------------------------

// 接続センサ数
#define SENSOR_NUM							(1)

// Width/Height
#define CAMERA_WIDTH_MIN					(8)
#define CAMERA_HEIGHT_MIN					(8)
#define CAMERA_WIDTH_MAX					(2560)
#define CAMERA_HEIGHT_MAX					(2048)

// 画素数
#define CAMERA_PIXEL_COUNT					"2560 x 2048 Pixel"

// 画素サイズ
#define CAMERA_PIXEL_SIZE					"3.45 x 3.45 um"

// Pixel Size
#define PIXEL_SIZE							(2)			// 14bit

// レート
#define FRAMERATE_MAX						(FRAMERATE_8BIT_MAX)
#define FRAMERATE_MIN						(0.1)
#define FRAMERATE_TIME_MIN					(1.0/FRAMERATE_8BIT_MAX)
#define FRAMERATE_TIME_MAX					(0xffffff)	// ここはレジスタに設定できる最大値(16777215)

#define FRAMERATE_8BIT_MAX					(131.9)
#define FRAMERATE_10BIT_MAX					(120.8)
#define FRAMERATE_12BIT_MAX					(70.9)

// 露光時間
#define MIN_EXPOSURE_TIME					(1)
#define MAX_EXPOSURE_TIME					(9990000)	// 9.99s

#define MIN_EXPOSURE_TIME_8BIT				(7)		// 3.54us (1H/74.25) 2ライン単位の為x2
#define MIN_EXPOSURE_TIME_10BIT				(8)		// 3.86us (1H/74.25) 2ライン単位の為x2
#define MIN_EXPOSURE_TIME_12BIT				(13)	// 6.58us (1H/74.25) 2ライン単位の為x2

// H Interval Clock
#define SENSOR_HINTERVAL_CLOCK_8BIT			(0x107)
#define SENSOR_HINTERVAL_CLOCK_10BIT		(0x11f)
#define SENSOR_HINTERVAL_CLOCK_12BIT		(0x1e9)

// センサRead Out時間(High Speed Mode)
#define SENSOR_READOUT_TIME_LNINE2_HIGH_SPEED	(0x1a)
#define SENSOR_READOUT_TIME_LNINE4_HIGH_SPEED	(0x1a)
#define SENSOR_READOUT_TIME_LNINE6_HIGH_SPEED	(0x1a)

// TGPD(High Speed Mode)
#define SENSOR_TGPD_TIME_LNINE2_HIGH_SPEED	(0x20)
#define SENSOR_TGPD_TIME_LNINE4_HIGH_SPEED	(0x22)
#define SENSOR_TGPD_TIME_LNINE6_HIGH_SPEED	(0x24)

// High Speed Mode時のHeight位置
#define FRAME_RATE_HIGH_SPEED_MODE_START_LINE2	(961+4)
#define FRAME_RATE_HIGH_SPEED_MODE_START_LINE4	(961+4)
#define FRAME_RATE_HIGH_SPEED_MODE_START_LINE6	(961+4)

// High Speed Mode Rate Max VMAX
#define FRAME_RATE_HIGH_SPEEED_MODE_RATE_MAX_VMAX (30)

// Hinterval Singl
#define HINTERVAL_CLOCK_8BIT_SINGLE_1_25G	(0x652)
#define HINTERVAL_CLOCK_10BIT_SINGLE_1_25G	(0x7FF)
#define HINTERVAL_CLOCK_12BIT_SINGLE_1_25G	(0x982)

#define HINTERVAL_CLOCK_8BIT_SINGLE_2_50G	(0x34F)
#define HINTERVAL_CLOCK_10BIT_SINGLE_2_50G	(0x412)
#define HINTERVAL_CLOCK_12BIT_SINGLE_2_50G	(0x4D3)

#define HINTERVAL_CLOCK_8BIT_SINGLE_3_125G	(0x2AC)
#define HINTERVAL_CLOCK_10BIT_SINGLE_3_125G	(0x348)
#define HINTERVAL_CLOCK_12BIT_SINGLE_3_125G	(0x3E4)

#define HINTERVAL_CLOCK_8BIT_SINGLE_5_00G	(0x1BA)
#define HINTERVAL_CLOCK_10BIT_SINGLE_5_00G	(0x21C)
#define HINTERVAL_CLOCK_12BIT_SINGLE_5_00G	(0x27C)

#define HINTERVAL_CLOCK_8BIT_SINGLE_6_25G	(0x168)
#define HINTERVAL_CLOCK_10BIT_SINGLE_6_25G	(0x1B6)
#define HINTERVAL_CLOCK_12BIT_SINGLE_6_25G	(0x203)

#define HINTERVAL_CLOCK_8BIT_SINGLE_10_00G	(0x109)
#define HINTERVAL_CLOCK_10BIT_SINGLE_10_00G	(0x11F)
#define HINTERVAL_CLOCK_12BIT_SINGLE_10_00G	(0x1E9)

#define HINTERVAL_CLOCK_8BIT_SINGLE_12_50G	(0x109)
#define HINTERVAL_CLOCK_10BIT_SINGLE_12_50G	(0x11F)
#define HINTERVAL_CLOCK_12BIT_SINGLE_12_50G	(0x1E9)

// DRRS Hinterval
#define HINTERVAL_CLOCK_8BIT_DRRS			(0x14a)
#define HINTERVAL_CLOCK_10BIT_DRRS			(0x162)
#define HINTERVAL_CLOCK_12BIT_DRRS			(0x209)

// DRRS Add Height Size
#define DRRS_HEIGHT_ADD_SIZE				(8)

// Sensor Height Clock
#define SENSOR_H_CLOCK						(74.25)
#define SENSOR_H_TIME						(1.0/74.25)

// V Interval Line
#define SENSOR_HEIGHT_EFFECTIVE_MARGIN		(8)
#define SENSOR_HEIGHT_HIDE_LINE				(0)
#define SENSOR_VINTERVAL_FIX_LINE			(84)		// 6+1+12+12+7 + 38(GTWAIT)+8(GSDLY)
#define SENSOR_VINTERVAL_LINE				(2140)		// 出力ライン数(2056(All Pixel)+38(Other Pixel)+38(GTWAIT)+8(GSDLY) = 2140

// V Valid Line
#define SENSOR_VVALIDL_FIX_LINE_HIGH_SPEED	(8)			// フレームレート高速化時

// DRRS VMAX
#define SENSOR_DRRS_FIX_LINE				(62)		// 10+1+18+12+6+8+7
#define SENSOR_DRRS_VMAX					(2118)

// Readout
#define SENSOR_TGSE							(2)
#define SENSOR_TGES							(48)							// 10+38(GTWAIT)
#define SENSOR_TGPD							(SENSOR_VINTERVAL_LINE)			// VMAX

// DRRS TGES
#define SENSOR_TGES_DRRS					(12)

// SHS
#define SENSOR_SHS_MIN_OFFSET				(52)		// GTWAIT(38) + 14
#define SENSOR_SHS_MIN						(2)			// Number of lines per frame - 2

// ノーマルモード時に設定するGenicam側の露光時間
#define SENSOR_EXPOSURE_FIX					(1)

// Register
#define SENSOR_REG_ID_MIN					(0x2)
#define SENSOR_REG_ID_MAX					(0x1c)
#define SENSOR_REG_NUM						(0x100)

// Data Update Size
#define UPDATE_DATA_SIZE					(16*1024*1024)

//----------------------------------------------------------------------------------
// Memory MAP
//----------------------------------------------------------------------------------
#define CAMERA_PARAM_CONNON_ADRS			(0x82000000)	// カメラパラメータ共通領域
#define CAMERA_PARAM_USER_ADRS				(0x82000200)	// カメラパラメータユーザー領域
#define BOARD_ERROR_ADRS					(0x82004000)	// 初期化エラー情報格納アドレス
	#define BOARD_ERROR_SIZE				(0x200)			// 初期化エラー情報格納アドレス
#define FIRM_DATA_ADRS						(0x82004200)	// ファームウェアデータ領域アドレス
	#define FIRM_DATA_SIZE					(0x3E00)		// ファームウェアデータ領域サイズ
#define VIDEO_REGS        					(0x82006200)	// Non-heap out of OS memories management
	#define VIDEO_REGS_SIZE 				(0x200)
#define CAMERA_EVENT        				(0x82006300)	// カメライベント
	#define CAMERA_SIZE        				(0x400)
#define CAMERA_ROI_CPU1        				(0x88006700)	// カメラROI
	#define CAMERA_ROI_CPU1_SIZE			(CAMERA_ROI_CPU1+0x00)
	#define CAMERA_ROI_CPU1_OFFSET			(CAMERA_ROI_CPU1+0x80)
	#define CAMERA_ROI_CPU1_VALID			(CAMERA_ROI_CPU1+0x100)
	#define CAMERA_SIZE        				(0x400)
#define UART0_RECV_BUFF_ADRS				(0x82006C00)	// UART0受信用
#define UART0_SEND_BUFF_ADRS				(0x82007000)	// UART0送信用
#define FIRM_SENSOR_REGISTER_SAVE_ADRS		(0x82007400)	// センサレジスタ格納アドレス
#define FIRM_UPDATE_XML_FILE_NAME_ADRS		(0x82009400)	// Update XML File Name領域
#define FIRM_UPDATE_XML_FILE_NAME_SIZE		(0x200)			// Update XML File Nameサイズ
#define FIRM_PHY_DATA_ADRS					(0x82100000)	// PHYデータ格納アドレス
#define FIRM_XML_FILE_ADRS					(0x82200000)	// XMLファイル格納アドレス
	#define FIRM_XML_FILE_SIZE				(0x400000)		// XMLファイル格納サイズ
#define FIRM_CXP_SEND_DATA_ALL_ADRS			(0x82300000)	// CXP Send All Dataアドレス
	#define FIRM_CXP_SEND_DATA_ALL_SIZE		(0x100000)		// CXP Send All Dataサイズ
#define FIRM_CXP_SEND_DATA_CMD_ADRS			(0x82400000)	// CXP Send Command Dataアドレス
	#define FIRM_CXP_SEND_DATA_CMD_SIZE		(0x100000)		// CXP Send Command Dataサイズ
#define FIRM_CXP_RECV_DATA_ALL_ADRS			(0x82500000)	// CXP Recv All Dataアドレス
	#define FIRM_CXP_RECV_DATA_ALL_SIZE		(0x100000)		// CXP Recv All Dataサイズ
#define FIRM_CXP_RECV_DATA_CMD_ADRS			(0x82600000)	// CXP Recv Command Dataアドレス
	#define FIRM_CXP_RECV_DATA_CMD_SIZE		(0x100000)		// CXP Recv Command Dataサイズ
	#define FIRM_CXP_DATA_INTERVAL			(0x00400000)	// CXP データ間隔
#define FIRM_UPDATE_ADRS					(0x82800000)	// Update領域(圧縮領域)
	#define FIRM_UPDATE_SIZE				(0x02000000)	// Updateサイズ(32M)
#define FIRM_UPDATE_UNCOMP_ADRS				(0x84700000)	// Update領域(非圧縮領域)
	#define FIRM_UPDATE_UNCOMP_SIZE			(24*1024*1024)	// Update領域(非圧縮領域サイズ)
#define DMA_MEMORY_ADRS						(0x84800000)	// DMA用メモリアドレス
#define DMA_MEMORY_IPU_MULTI_INTERVAL		(DMA_WIDTH_DATA_ALIGH * IMG_HEIGHT)
#define DPC_GRID_ADRS						(0x86820000)	// 4byte * 欠陥画素最大数
#define DPC_GRID2_ADRS						(0x866A0000)	// 4byte * 欠陥画素最大数
#define DPCINFO_ADRS						(0x86920000)	// 4byte * 欠陥画素最大数
#define DPC_MEMORY_ADRS						(0x86A00000)	// 画像補正データ用係数格納メモリアドレス
	#define DPC_MEMORY_IPU_MULTI_INTERVAL	(DPC_WIDTH_DATA_ALIGH * IMG_HEIGHT)
#define FFC_MEMORY_ADRS						(0x87000000)	// FFCデータ用メモリアドレス
	#define FFC_MEMORY_IPU_MULTI_INTERVAL	(FFC_WIDTH_DATA_ALIGH * IMG_HEIGHT)
#define FFC_OFFSET_MEMORY_ADRS				(0x8B400000)	// FFC時:オフセットデータ格納用アドレス
#define FFC_GAIN_MEMORY_ADRS				(0x8C400000)	// FFC ゲインデータ格納用アドレス
#define FFC0_TEMP_MEMORY_ADRS				(0x8E400000)	// FFC時:FFC0格納用アドレス
#define DPC_SD_MEMORY_ADRS					(0x8E400000)	// DPC時:標準偏差格納用アドレス
#define FFC3_TEMP_MEMORY_ADRS				(0x92400000)	// FFC時:FFC3格納用アドレス
#define DPC_AVG_MEMORY_ADRS					(0x92400000)	// DPC時:平均データ格納用アドレス
#define FFC_BLACK_MEMORY_ADRS				(0x96400000)	// FFC時:FFC 黒データ格納用アドレス
#define DPC_SUM_MEMORY_ADRS					(0x96400000)	// DPC時:加算データ格納用アドレス
#define FFC_WHITE_MEMORY_ADRS				(0x9A400000)	// FFC時:FFC 白データ格納用アドレス
#define DPC_SQUARE_SUM_MEMORY_ADRS			(0x9A400000)	// DPC時:２乗加算データ格納用アドレス
#define DPC_NON_MEMORY_ADRS					(0x9E400000)	// DPC時:感度不均一データ格納用アドレス
#define DPC_TEMP0_MEMORY_ADRS				(0xA2400000)	// DPC時:DPC欠陥座標１次格納用アドレス
#define DPC_TEMP1_MEMORY_ADRS				(0xA2C00000)	// DPC時:DPC欠陥座標１次格納用アドレス
#define DPC_TEMP2_MEMORY_ADRS				(0xA3400000)	// DPC時:DPC欠陥座標１次格納用アドレス
#define DPC_TEMP3_MEMORY_ADRS				(0xA3C00000)	// DPC時:DPC欠陥座標１次格納用アドレス
#define DRRS_IMG_MEMORY_ADRS				(0xA4400000)	// DRRS画像格納アドレス
#define DRRS_IMG_MEMORY_ALIGN				(8192)
#define DRRS_IMG_MEMORY_IPU_MULTI_INTERVAL	(DRRS_IMG_MEMORY_ALIGN * IMG_HEIGHT)

#define DIAG_DDR_BASE1_ADRS					(0xA0000000)
#define DIAG_DDR_BASE2_ADRS					(0xB0000000)


//----------------------------------------------------------------------------------
// QSPI Flash
//----------------------------------------------------------------------------------
#define QSPI_FLASH_SIZE						(128*1024*1024)
#define QSPI_FLASH_SEC_SIZE					(64*1024)


//----------------------------------------------------------------------------------
// AXI QSPI Flash
//----------------------------------------------------------------------------------
#define AXI_QSPI_FLASH_SIZE					(128*1024*1024)
#define AXI_QSPI_FLASH_SEC_SIZE				(64*1024)


//----------------------------------------------------------------------------------
// Flash MAP
//----------------------------------------------------------------------------------
#define FLASH_XML_ADRS						(0x00100000)	// XML Address
	#define FLASH_XML_SIZE					(0x000f0000)	// XML Size
#define FLASH_XML_FILE_NAME_ADRS			(0x001f0000)	// XML File Name Address
	#define FLASH_XML_FILE_NAME_SIZE		(0x00001000)	// XML File Name Size
#define FLASH_UPDATE_ALL_ADRS				(0x00200000)	// All Address
#define FLASH_PHY_DATA_ADRS					(0x01500000)	// PHY Data Adrs
	#define FLASH_PHY_DATA_SIZE				(0x00100000)	// PHY Data Size
#define FLASH_UPDATE_ALL_ADMIN_ADRS			(0x01600000)	// All Address(Backup)
#define FLASH_PHY_DATA_BACKUP_ADRS			(0x01900000)	// PHY Data Adrs(Backup)
#define FLASH_LUT1_ADRS						(0x02A00000)	// LUT1データ格納Flashアドレス
#define FLASH_LUT2_ADRS						(0x02A10000)	// LUT2データ格納Flashアドレス
#define FLASH_BOARD_PARAM_ADRS				(0x02A20000)	// 基板情報格納
#define FLASH_USERSET0_ADRS					(0x02A30000)	// UserSet0保存アドレス
#define FLASH_USERSET1_ADRS					(0x02A40000)	// UserSet1保存アドレス
#define FLASH_USERSET2_ADRS					(0x02A50000)	// UserSet2保存アドレス
#define FLASH_USERSET_COMMON_ADRS			(0x02A60000)	// UserSet共通保存アドレス
#define FLASH_DPC_INFO_ORG_ADRS				(0x02A70000)	// DPC調整情報格納アドレス(オリジナル)
#define FLASH_DPC_IMPOSSIBLE_GRID_ORG_ADRS	(0x02A80000)	// DPC補正不可座標アドレス(オリジナル)
#define FLASH_DPC_INFO_ADRS					(0x02A90000)	// DPC調整情報格納アドレス
#define FLASH_DPC_IMPOSSIBLE_GRID_ADRS		(0x02AA0000)	// DPC補正不可座標アドレス
#define FLASH_DPC_ADRS						(0x02AB0000)	// 画像補正データ用係数格納Flashアドレス
#define FLASH_DPC_ORG_ADRS					(0x02BC0000)	// 画像補正データ用係数格納Flashアドレス(オリジナル)
#define FLASH_FFC_INFO_ADRS					(0x02CD0000)	// FFC調整情報格納
#define FLASH_FFC0_ADRS						(0x02CE0000)	// FFCゲインデータ格納Flashアドレス
#define FLASH_DIAG_ADRS						(0x07FF0000)	// Read/Writeテスト用


//----------------------------------------------------------------------------------
// IPU
//----------------------------------------------------------------------------------

// IPU Size
#define IMG_WIDTH							(2592)					// 画像幅
#define IMG_HEIGHT							(2056)					// 画像高さ
#define IMG_WIDTH_OFFSET					(16)
#define IMG_HEIGHT_OFFSET					(4)

#define IMG_WIDTH_IPU_MULTI_FULL			(CAMERA_WIDTH_MAX + (IPU_WIDTH_ADD_SIZE * IPU_COUNT))
#define IMG_WIDTH_IPU_MULTI_HALF			((CAMERA_WIDTH_MAX / IPU_COUNT) + IPU_WIDTH_ADD_SIZE)
#define IPU_WIDTH_ADD_SIZE					(32)

#define IMG_WIDTH_IPU_SIZE					(IMG_WIDTH / IPU_COUNT)

#define IMG_WIDTH_MIN						(0)
#define IMG_HEIGHT_MIN						(0)
#define IMG_WIDTH_MAX						(IMG_WIDTH)
#define IMG_HEIGHT_MAX						(IMG_HEIGHT)

#define IMG_STRIDE							(IMG_WIDTH)				// 画像ストライド:2byteのため本来は1280だが、2byteはunsigned shortで表現するためIMG_WIDTHと同数

#define IMG_WIDTH_IPU_MULTI_ADD_SIZE		(32)


// IPU Width / Height offset
#define IMG_WIDTH_OFFSET_SIZE				(IMG_WIDTH_OFFSET*2)
#define IMG_HEIGHT_OFFSET_SIZE				(IMG_HEIGHT_OFFSET*2)

// DMA Align
#define DMA_WIDTH_DATA_ALIGH				(8192)
#define DMA_WIDTH_DATA_ALIGH_PIXEL			(DMA_WIDTH_DATA_ALIGH/2)

// Black Pixel Width
#define BLACK_PIXEL_WIDTH					(88)

// Line Black
#define LINE_BLACK_UPLOAD_SIZE				(1024*1024)
#define LINE_BLACK_DATA_SIZE				(IMG_HEIGHT*FPGA_LINE_BLACK_AUTO_MEM_ALIGH)
#define LINE_BLACK_DATA_ASCII_SIZE			(IMG_HEIGHT*FPGA_LINE_BLACK_AUTO_MEM_ALIGH*2+IMG_HEIGHT*4)

// IPU Count
#define IPU_COUNT							(2)


//----------------------------------------------------------------------------------
// FFC
//----------------------------------------------------------------------------------

// FFC積算回数
#define NUM_FFC_IMG_FOR_DETECT				(16)

// FFC Correction Mode
#define FFC_CORRECTION_MODE_MIN				(FFC_CORRECTION_MODE_FIRST)
#define FFC_CORRECTION_MODE_MAX				(FFC_CORRECTION_MODE_FIRST)

// FFC Min/Max
#define FFC_NUMBER_MIN						(0)
#define FFC_NUMBER_MAX						(3)

// FFC Min/Max User
#define FFC_NUMBER_USER_MIN					(1)
#define FFC_NUMBER_USER_MAX					(2)

// FFC Min/Max Admin
#define FFC_NUMBER_ADMIN_MIN				(0)
#define FFC_NUMBER_ADMIN_MAX				(3)

// FFC Number
#define FFC_NUM_FACTORY						(4)

// FFC Width Data Size(FFC １次補正)
#define FFC_WIDTH_DATA_SIZE_FIRST			(IMG_WIDTH*3)

// FFC Data Align
#define FFC_WIDTH_DATA_ALIGH				(16384)

// FFC All Size(FFC １次補正)
#define FFC_ALL_SIZE_FIRST					(FFC_WIDTH_DATA_ALIGH*IMG_HEIGHT)

// FFCオフセットゲインデータ個数
#define FFC_DATA_NUM_ALIGN					(IMG_WIDTH*IMG_HEIGHT)

// サイズ(単位バージョン)
#define FFC_DATA_INTERVAL_SIZE				(0x14A0000)		// width*height*4をFlashのセクタサイズでAlign

// サイズ
#define FFC_DATA_SIZE						(IMG_WIDTH*IMG_HEIGHT*4)		// FFCオフセットゲインデータサイズ

// FFC Upload/Download
#define FFC_UPLOAD_SIZE						(128*1024*1024)
#define FFC_DOWNLOAD_SIZE					(128*1024*1024)

// FFC Initialize Black Level Max
#define FFC_BLACK_LEVEL_INIT_MAX			(4096)	// 14bit


//----------------------------------------------------------------------------------
// DPC
//----------------------------------------------------------------------------------

// DPC積算回数
#define NUM_IMG_FOR_DETECT					(16)

// 欠陥画素最大数
#define NUM_DEFECTION_PIX					(50000)		// width * height の約1%

// 欠陥画素マーク位置
#define DPC_DATA_UNIT_NUM					(1)

// 欠陥検出に使用するテスト回数(調光：最大2/FFC最大：2)
#define NUM_IMG_FOR_LOOP					(4)

// FFCループ
#define DPC_FFC_LOOP_NUMBER					(2)
#define DPC_FFC_LOOP_MIN					(0)
#define DPC_FFC_LOOP_MAX					(1)

// 輝度値ループ
#define DPC_BRIGHTNESS_LOOP_NUMBER			(2)
#define DPC_BRIGHTNESS_LOOP_MIN				(0)
#define DPC_BRIGHTNESS_LOOP_MAX				(1)

// 欠陥画素Data Align
#define DPC_WIDTH_DATA_ALIGH				(256)

// Size
#define DPC_MEMORY_NEW_SIZE					((IMG_WIDTH*IMG_HEIGHT)/8)			// 単純にWidth * height /8のサイズ ※Flash等の保存サイズ
#define DPC_MEMORY_NEW_ALL_SIZE				(DPC_WIDTH_DATA_ALIGH*IMG_HEIGHT*IPU_COUNT)	// メモリへの展開サイズ。メモリはWidth方向でアラインがある
#define DPC_MEMORY_NEW_UNIT_SIZE			(DPC_MEMORY_NEW_ALL_SIZE)			// Flashへの保存サイズにFlashのセクタを考慮してアラインされたサイズ
#define DPC_MEMORY_NEW_FLASH_UNIT_SIZE		(DPC_MEMORY_NEW_UNIT_SIZE)			// Flashへの保存サイズにFlashのセクタを考慮してアラインされたサイズ

// DPC Adjust Mode
#define DPC_ADJUST_MODE_MIN					(DPC_ADJUST_MODE_NOMAL)
#define DPC_ADJUST_MODE_MAX					(DPC_ADJUST_MODE_NOMAL)

// DPC Adjust Mode Default
#define DPC_ADJUST_MODE_DEFAULT				(DPC_ADJUST_MODE_NOMAL)

// DPC Download/Upload Size
#define DPC_UPLOAD_SIZE						(2*1024*1024)
#define DPC_DOWNLOAD_SIZE					(2*1024*1024)

// DPC Standard Deviation
#define DPC_SD_THRESHOLD_0					(200)
#define DPC_SD_THRESHOLD_1					(200)

// DPC Nonuniformity
#define DPC_NON_THRESHOLD_0					(0.05)
#define DPC_NON_THRESHOLD_1					(0.10)


//----------------------------------------------------------------------------------
// Bit Calc
//----------------------------------------------------------------------------------
#define FFC_OFFSET_GAIN_BIT_ADJUST			(4)			// FFCは14bit。これを12Bitで制御するためx4


//----------------------------------------------------------------------------------
// ROI
//----------------------------------------------------------------------------------

// Selector Number
#define ROI_SELECTOR_MIN					(0)
#define ROI_SELECTOR_MAX					(7)

// Selector Count
#define ROI_SELECTOR_COUNT					(ROI_SELECTOR_MAX+1)

// Align
#define ROI_WIDTH_ALIGH						(4)
#define ROI_HEIGHT_ALIGH					(1)

// Size Min / Max
#define ROI_WIDTH_SIZE_MIN					(0)
#define ROI_WIDTH_SIZE_MAX					(IMG_WIDTH)
#define ROI_HEIGHT_SIZE_MIN					(0)
#define ROI_HEIGHT_SIZE_MAX					(IMG_HEIGHT)

// Offset Min / Max
#define ROI_WIDTH_OFFSET_MIN				(0)
#define ROI_HEIGHT_OFFSET_MIN				(0)
#define ROI_WIDTH_OFFSET_MAX				(IMG_WIDTH-ROI_WIDTH_ALIGH)
#define ROI_HEIGHT_OFFSET_MAX				(IMG_HEIGHT-ROI_HEIGHT_ALIGH)

// Offset
#define ROI_WIDTH_OFFSET					(8)
#define ROI_HEIGHT_OFFSET					(4)

// Align Sensor
#define ROI_SENSOR_WIDTH_OFFSET_ALIGH		(8)
#define ROI_SENSOR_WIDTH_SIZE_ALIGH			(4)
#define ROI_SENSOR_HEIGHT_OFFSET_ALIGH		(8)
#define ROI_SENSOR_HEIGHT_SIZE_ALIGH		(8)

// Align FPGA
#define ROI_FPGA_WIDTH_OFFSET_ALIGH			(4)
#define ROI_FPGA_WIDTH_SIZE_ALIGH			(4)
#define ROI_FPGA_HEIGHT_OFFSET_ALIGH		(1)
#define ROI_FPGA_HEIGHT_SIZE_ALIGH			(1)

// CXP Height Aligh
#define CXP_HEIGHT_ALIGH					(4)

// CXP Width
#define CXP_WIDTH_ALIGH						(32)
#define CXP_WIDTH_MIN						(128)

// Sensor DPC Adjust
#define IMG_WIDTH_DPC_ADJUST_OFFSET			(2)
#define IMG_WIDTH_DPC_ADJUST_SIZE			(4)
#define IMG_HEIGHT_DPC_ADJUST_OFFSET		(4)
#define IMG_HEIGHT_DPC_ADJUST_SIZE			(8)

// Sensor Default Width
#define SENSOR_DEFAULT_WIDTH				(0xa80)

	
//----------------------------------------------------------------------------------
// LUT
//----------------------------------------------------------------------------------
// LUT Mode
#define LUT_MODE_MIN						(LUT_MODE_DISABLE)
#define LUT_MODE_MAX						(LUT_MODE1)

// LUT Select
#define LUT_SELECT_NUM						(1)
#define LUT_SELECT_MIN_NUM					(LUT_SELECT1)
#define LUT_SELECT_MAX_NUM					(LUT_SELECT1)

// Default Bit
#define LUT_DEFAULT_BIT						(12)


//----------------------------------------------------------------------------------
// User Selector
//----------------------------------------------------------------------------------
#define DIGITAL_USER_SELECT_MIN				(DIGITAL_USER3_SELECT)
#define DIGITAL_USER_SELECT_MAX				(DIGITAL_USER6_SELECT)
#define DIGITAL_USER_SELECT_MASK			(0x07)


//----------------------------------------------------------------------------------
// Flash Size
//----------------------------------------------------------------------------------
#define FLASH_UPDATE_ALL_SIZE				(20*1024*1024)


//----------------------------------------------------------------------------------
// GPIO Min/Max
//----------------------------------------------------------------------------------
#define GPIO_LINE_MIN						(0)
#define GPIO_LINE_MAX						(6)


//----------------------------------------------------------------------------------
// Userset
//----------------------------------------------------------------------------------
#define CAMERA_SAVE_COMMON_ADRS				(FLASH_USERSET_COMMON_ADRS)		// Common
#define CAMERA_SAVE_USER0_ADRS				(FLASH_USERSET0_ADRS)			// User Parameter0


//----------------------------------------------------------------------------------
// Userset Set Param Size
//----------------------------------------------------------------------------------
#define CAMERA_SAVE_USER_SIZE				(0x1100)						// UserSetサイズ


//----------------------------------------------------------------------------------
// Userset Mark Data
//----------------------------------------------------------------------------------
#define CAMERA_MARK_DATA					((0x0001<<16) | CAMERA_ID_AXP_ABA052VIR_02)	// Mark Data(Ver.0.1)


//----------------------------------------------------------------------------------
// Temp
//----------------------------------------------------------------------------------

// Peltier Target Min/Max
#define PELTIER_TARGET_MIN_TEMP				(5.0)
#define PELTIER_TARGET_MAX_TEMP				(35.0)

// センサアラーム温度
#define SENSOR_TEMP_ALRAM_MIN				(0.0)
#define SENSOR_TEMP_ALRAM_MAX				(100.0)

// ケースアラーム温度
#define CASE_TEMP_ALRAM_MIN					(0.0)
#define CASE_TEMP_ALRAM_MAX					(100.0)

// センサ温度マージン
#define SENSOR_TEMP_ADJUST_MARGIN			(0.5)

// Case温度監視間隔
#define CASE_TEMP_CHECK_INTERVAL			(1000)		// 1s

// 温度異常監視間隔
#define TEMP_ABNORMAL_CHECK_INTERVAL		(10000)		// 10s

// ペルチェ制御間隔
#define PELTIER_CONTROL_CHECK_INTERVAL		(5000)		// 5s

// FPGA温度異常上限(外部電源)
#define TEMP_ABNORMAL_FPGA_UPPER_EXT		(95)

// FPGA温度異常下限(外部電源)
#define TEMP_ABNORMAL_FPGA_LOWER_EXT		(90)

// FPGA温度異常上限(PO)
#define TEMP_ABNORMAL_FPGA_UPPER_PO			(70)

// FPGA温度異常下限(PO)
#define TEMP_ABNORMAL_FPGA_LOWER_PO			(65)

// Pertier Power Level
#define PELTIER_POWER_LEVEL_MIN				(PELTIER_POWER_LEVEL_LOW)
#define PELTIER_POWER_LEVEL_MAX				(PELTIER_POWER_LEVEL_ULTRA)

#define PELTIER_POWER_LEVEL_LOW				(1)
#define PELTIER_POWER_LEVEL_MID				(2)
#define PELTIER_POWER_LEVEL_HIGH			(3)
#define PELTIER_POWER_LEVEL_ULTRA			(4)

#define PELTIER_POWER_LEVEL_LOW_CLIP		(0x220)
#define PELTIER_POWER_LEVEL_MIDLE_CLIP		(0x270)
#define PELTIER_POWER_LEVEL_HIGH_CLIP		(0x350)
#define PELTIER_POWER_LEVEL_ULTRA_CLIP		(0x380)


//----------------------------------------------------------------------------------
// LED
//----------------------------------------------------------------------------------
#define LED_NUM_MIN							(LED_NUM0)
#define LED_NUM_MAX							(LED_NUM1)


//----------------------------------------------------------------------------------
// GigE Information
//----------------------------------------------------------------------------------
#define GE_PACKET_SIZE_DEFAULT				(1400)		// Packet Size
#define GE_PACKET_DELAY_DEFAULT				(0)			// Packet Delay

	
//----------------------------------------------------------------------------------
// Board Prameter Size
//----------------------------------------------------------------------------------
#define BOARD_PARAM_SENSOR_ID_SIZE			(31)


//----------------------------------------------------------------------------------
// GCP
//----------------------------------------------------------------------------------
#define GCP_MODE_MIN						(GCP_MODE_BOARD)
#define GCP_MODE_MAX						(GCP_MODE_VOLTAGE)


//----------------------------------------------------------------------------------
// Gradation Compress
//----------------------------------------------------------------------------------
#define GC_SENSOR_AD_BIT					(SENSOR_REG_AD_10BIT)
#define GC_CAMERA_BIT						(10)


//----------------------------------------------------------------------------------
// Black Offset Auto
//----------------------------------------------------------------------------------
#define BLACK_OFFSET_AUTO_WIDTH_SIZE		(96)


//----------------------------------------------------------------------------------
// Binning
//----------------------------------------------------------------------------------
#define	BINNING_MIN							(1)
#define	BINNING_MAX							(16)


//----------------------------------------------------------------------------------
// CoaxPress
//----------------------------------------------------------------------------------

// Port Count
#define CXP_PORT_COUNT						(1)

// Port Min/Max
#define CXP_PORT_MIN						(0)
#define CXP_PORT_MAX						(1)


//----------------------------------------------------------------------------------
// Default Parameter
//----------------------------------------------------------------------------------

// Target Default
#define PELTIER_DEFAULT_TEMP				(15*CAMERA_SAVE_TEMP_UNIT)

// Sensor Alarm
#define SENSOR_DEFAULT_LOW_TEMP				(0*CAMERA_SAVE_TEMP_UNIT)
#define SENSOR_DEFAULT_HIGH_TEMP			(80*CAMERA_SAVE_TEMP_UNIT)

// Case Limit
#define CASE_DEFAULT_LOW_TEMP				(80*CAMERA_SAVE_TEMP_UNIT)
#define CASE_DEFAULT_HIGH_TEMP				(100*CAMERA_SAVE_TEMP_UNIT)

// Peltier Power Level
#define PELTIER_POWER_LEVEL					(PELTIER_POWER_LEVEL_HIGH)

// FFC/DPC
#define FFC_DEFAULT_MODE					(0x11)
#define DPC_DEFAULT_MODE					(0x01)
#define FFC_CORMODE_DEFAULT					(1)

// LUT
#define LUT_DEFAULT_FORMAT					(1) 			// LUT Format
#define LUT_DEFAULT_BIN_THRESHOLD			(0) 			// LUT 2値化しきい値
#define LUT_DEFAULT_GAMMA					(10) 			// LUT ガンマ1.0(10倍している)

// Framerate
#define DEFAULT_FRAMERATE					(20)		// fps
#define DEFAULT_FRAMERATE_TIME2				((1.0/DEFAULT_FRAMERATE) * ACQUISITION_FRAMERATE_UINIT)
#define DEFAULT_FRAME_HTIME				(((SENSOR_H_CLOCK*1000*1000)/SENSOR_VINTERVAL_LINE)/DEFAULT_FRAMERATE)
#define DEFAULT_FRAMERATE_TIME			((((1.0/DEFAULT_FRAMERATE)*1000000)/(DEFAULT_FRAME_HTIME * SENSOR_H_TIME))+0.5/*四捨五入*/)

// Exposure
#define DEFAULT_EXPOSURE					(10000)
#define DEFAULT_EXPOSURE_TIME				((DEFAULT_EXPOSURE/(HINTERVAL_CLOCK_12BIT_SINGLE_12_50G * SENSOR_H_TIME))+0.5/*四捨五入*/)
#define DEFAULT_EXPOSURE_TIME2				(10000)

// Bit
#define DEFAULT_BIT							(12)
#define DEFAULT_BIT_FPGA					(FPGA_AOI_BITWIDTH_12BIT)

// Acquisition Mode
#define DEFAULT_ACQUISITION_MODE			(2)		// Infinity

// Encoder
#define DEFAULT_ENCODER_TIMEOUT				(1000000)

// User Selector Default
#define USER_SELECTOR_DEFAULT				(3)

// ROI Width
#define DEFAULT_ROI_WIDTH0					(0x00000a00)	// Offset=0  / Size=2560
#define DEFAULT_ROI_WIDTH0_LEFT				(0x00100500)	// Offset=16 / Size=1280
#define DEFAULT_ROI_WIDTH0_RIGHT			(0x00100500)	// Offset=16 / Size=1280

// ROI Height
#define DEFAULT_ROI_HEIGHT0					(0x80040800)

// Timing Generator
#define DEFAULT_TG_TGSE						(SENSOR_TGSE)
#define DEFAULT_TG_TGES						(SENSOR_TGES)
#define DEFAULT_TG_TGPD						(SENSOR_TGPD)

// Auto Bright Mode
#define DEFAULT_AUTO_BRIGHT_MODE			(MODE_DISABLE)

// Auto Bright Target
#define DEFAULT_AUTO_BRIGHT_TARGET			(FPGA_AUTO_BRIGHT_CTRL_TARGET_SET(2048))

// Auto Exposure Min/Max
#define DEFAULT_AUTO_BRIGHT_EXP_MIN			(100)
#define DEFAULT_AUTO_BRIGHT_EXP_MAX			(49676)

// Auto Gain Min/Max
#define DEFAULT_AUTO_BRIGHT_GAIN_MIN		(1*AUTO_GAIN_UNIT)
#define DEFAULT_AUTO_BRIGHT_GAIN_MAX		(31*AUTO_GAIN_UNIT)
#define DEFAULT_AUTO_BRIGHT_TAEGET_AREA 	(AUTO_BRIGHT_TARGET_AREA_ALL_RANGE)

// Auto Bright Adjust Time Interval
#define AUTO_BRIGHT_ADJUST_TIME_INTERVAL	(100)	// 100ms

// Auto Bright Width/Height
#define DEFAULT_AUTO_WIDTH_HEIGHT			((CAMERA_HEIGHT_MAX<<16) | CAMERA_WIDTH_MAX)

// Gradation Compress
#define GC_POS_FIRST_DEFAULT				(1)		// Gradation Compress Position First
#define GC_POS_SECOND_DEFAULT				(7)		// Gradation Compress Position Second
#define GC_GAIN_FIRST_DEFAULT				(0)		// Gradation Compress Gain First
#define GC_GAIN_SECOND_DEFAULT				(0)		// Gradation Compress Gain Second
#define GC_MODE_DEFAULT						(0)		// Gradation Compress Mode

// High Speed Mode Height
#define FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_DEFAULT			(CAMERA_HEIGHT_MAX)
#define FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_GE			(4096)
#define FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_OTHER			(16384)

// High Speed Mode Line Count
#define FRAME_RATE_HIGH_SPEED_MODE_LINE_COUNT_DEFAULT_GE	(4)
#define FRAME_RATE_HIGH_SPEED_MODE_LINE_COUNT_DEFAULT_CXP	(4)

// ConnectionConfigDefault
#define DEFAULT_CONNECTION_CONFIG_DEFAULT	((1<<16) | 0x48)		// 1port / 6.25Gbps


//----------------------------------------------------------------------------------
// EventSelector Default
//----------------------------------------------------------------------------------
#define DEFAULT_EVENT_SELECTOR						(0x14)


//----------------------------------------------------------------------------------
// Diag
//----------------------------------------------------------------------------------

// センサ温度変更評価
#define DIAG_TEMP_SENSOR_TARGET						(12.0)

// センサ温度範囲
#define DIAG_TEMP_SENSOR_MIN						(0.0)
#define DIAG_TEMP_SENSOR_MAX						(40.0)

// センサ温度正常値
#define DIAG_TEMP_SENSOR_UPPER_ALARM_NOMAL			(40.0)
#define DIAG_TEMP_SENSOR_UNDER_ALARM_NOMAL			(5.0)

// センサ温度異常
#define DIAG_TEMP_SENSOR_UPPER_ALARM				(5.0)
#define DIAG_TEMP_SENSOR_UNDER_ALARM				(30.0)

// ケース温度正常値
#define DIAG_TEMP_CASE_UPPER_ALARM_NORMAL			(50.0)
#define DIAG_TEMP_CASE_UNDER_ALARM_NORMAL			(0.0)

// ケース温度異常値
#define DIAG_TEMP_CASE_UPPER_ALARM					(15.0)

// ケース温度範囲
#define DIAG_TEMP_CASE_MIN							(PELTIER_CASE_ALM_MIN_TEMP)
#define DIAG_TEMP_CASE_MAX							(PELTIER_CASE_ALM_MAX_TEMP)

// FPGA温度範囲
#define DIAG_TEMP_FPGA_MIN							(0.0)
#define DIAG_TEMP_FPGA_MAX							(80.0)

// センサ温度Check範囲
#define DIAG_SENSOR_TEMP_RANGE						(1.0)

#endif  // __ABA052VIR2_H__

// eof
