//**********************************************************************************
//
//                           Camera Header
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// firm.h - Firmware Header
//**********************************************************************************

#ifndef __FIRM_H__
#define __FIRM_H__

//----------------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------------
#include "common.h"


//----------------------------------------------------------------------------------
// Mode
//----------------------------------------------------------------------------------
#define MODE_DOG_DECIBEL					// Decibel Mode
#define COMPRESS_MODE						// Compress


//----------------------------------------------------------------------------------
// Camera Type
//----------------------------------------------------------------------------------
#define CAMERA_TYPE_AREA					(0)
#define CAMERA_TYPE_LINE					(1)


//----------------------------------------------------------------------------------
// Variant Type
//----------------------------------------------------------------------------------
#define VARIANT_NORMAL						(0)
#define VARIANT_ROTATE90					(1)


//----------------------------------------------------------------------------------
// Camera Spectrum Type
//----------------------------------------------------------------------------------
#define	CAMERA_TYPE_SPECTRUM				(1)
#define	CAMERA_TYPE_NO_SPECTRUM				(0)


//----------------------------------------------------------------------------------
// 製品情報
//----------------------------------------------------------------------------------

// Vendor Name
#define VENDOR_NAME							"AVAL DATA CORPORATION"

// Manufacture Name
#define MANUFACTURE_NAME					"https://www.avaldata.co.jp"

// Update File Name
#define UPDATE_FILE_NAME_CL_EXSAMPLE		""FIRM_NAME"_CL_Vxx.xxx"
#define UPDATE_FILE_NAME_CL_COMPARE			""FIRM_NAME"_CL"


//----------------------------------------------------------------------------------
// カメラ番号
//----------------------------------------------------------------------------------

// Near Infrared Area
#define CAMERA_ID_AXP_ABA003IR_01			(0)
#define CAMERA_ID_AXP_ABA003IR_02			(1)
#define CAMERA_ID_AXP_ABA001IR_01			(2)
#define CAMERA_ID_AXP_ABA001LIR_01			(3)
#define CAMERA_ID_AXP_ABAU20MIR_01			(4)
#define CAMERA_ID_AXP_ABA001MIR_01			(5)
#define CAMERA_ID_AXP_ABA013VIR_01			(6)
#define CAMERA_ID_AXP_ABA003VIR_01			(7)
#define CAMERA_ID_AXP_ABA001MIR_02			(8)
#define CAMERA_ID_AXP_ABA032VIR_01			(9)
#define CAMERA_ID_AXP_ABA052VIR_01			(10)
#define CAMERA_ID_AXP_ABA052VIR_02			(11)
#define CAMERA_ID_AXP_ABA052VIR2_01			(12)

// Near Infrared  Line
#define CAMERA_ID_AXP_ABL005IR_01			(1000)
#define CAMERA_ID_AXP_ABL005IR_02			(1001)
#define CAMERA_ID_AXP_ABL005WIR_01			(1002)
#define CAMERA_ID_AXP_ABL005MIR_01			(1003)
#define CAMERA_ID_AXP_ABL010IR_01			(1004)
#define CAMERA_ID_AXP_ABL010IR_02			(1005)
#define CAMERA_ID_AXP_ABL005IRHS_01			(1006)

// Hyper Spectrum
#define CAMERA_ID_AXP_AHSU20MIR_01			(2000)
#define CAMERA_ID_AXP_AHSU21MIR_01			(2001)
#define CAMERA_ID_AXP_AHS001MIR_01			(2002)
#define CAMERA_ID_AXP_AHS003VIR_01			(2003)
#define CAMERA_ID_AXP_AHS001MIR_02			(2004)
#define CAMERA_ID_AXP_AHS052VIR_01			(2005)
#define CAMERA_ID_AXP_AHS052VIR_02			(2006)

// Far Infrared Area
#define CAMERA_ID_AXP_ABA001FIR_01			(3000)
#define CAMERA_ID_AXP_ABA003FIR_01			(3001)

// Hyper Spectrum (Fourier)
#define CAMERA_ID_AXP_AHSF001IR_01			(4000)

// Multi Band
#define CAMERA_ID_AXP_ABA013VIRLF_01		(5000)


//----------------------------------------------------------------------------------
// Sensor Vendor
//----------------------------------------------------------------------------------
#define SENSOR_VENDOR_H						(0)
#define SENSOR_VENDOR_S						(1)
#define SENSOR_VENDOR_F						(2)


//----------------------------------------------------------------------------------
// Command Option
//----------------------------------------------------------------------------------
#define CMD_HELP_OPTION						"-?"


//----------------------------------------------------------------------------------
// Log Message Level
//----------------------------------------------------------------------------------
#define MSG_LEVEL_ERROR						(1)
#define MSG_LEVEL_WARNING					(2)
#define MSG_LEVEL_INFO						(3)


//----------------------------------------------------------------------------------
// Command OK/ERROR
//----------------------------------------------------------------------------------
#define	CMD_OK								"OK>\r"
#define	CMD_ERROR							"ERROR>\r"
#define CMD_ERROR_INVALID_CMD				"Invalid Command.\n"			// コマンド不正
#define CMD_ERROR_INVALID_ARG				"Invalid Argument Number.\n"	// 引数不正
#define CMD_ERROR_INVALID_PARAM				"Invalid Parameter.\n"			// パラメータ不正
#define CMD_ERROR_ACES_DENIED_PARAM			"Access Denied.\n"				// アクセスが制限されております
#define CMD_ERROR_DEVICE					"Device Access.\n"				// デバイスへのアクセスでエラー発生
#define CMD_ERROR_RESOURCE					"Resource Insufficiency.\n"		// リソースが足りません
#define CMD_ERROR_ADJUST					"Adjust Failure.\n"				// 調整に失敗しました
#define CMD_ERROR_NOT_SUPPORT				"Command Not Support.\n"		// Not Support


//----------------------------------------------------------------------------------
// Key Code
//----------------------------------------------------------------------------------
#define CODE_CR								'\r'
#define CODE_LF								'\n'


//----------------------------------------------------------------------------------
// Console Buffer Size
//----------------------------------------------------------------------------------
#define CONSOLE_BUFF_SIZE					(32)
#define CONSOLE_BUFF_COUNT					(9)
//#define PASSWORD_KEY						"aval\r"
#define PASSWORD_KEY						"19761028\r"
#define LOG_MSG_BUFF_SIZE					(192)


//----------------------------------------------------------------------------------
// コマンド履歴
//----------------------------------------------------------------------------------
#define CMD_HISTORY_LINE_SIZE				(CONSOLE_BUFF_SIZE)			// コマンド履歴ラインサイズ

// コマンドHistory個数(メモリ保存タイプ)
#define CMD_SAVE_NUM						(64)


//----------------------------------------------------------------------------------
// Mode Enable/Disable
//----------------------------------------------------------------------------------
#define MODE_ENABLE							(1)
#define MODE_DISABLE						(0)


//----------------------------------------------------------------------------------
// Mode Normal/Extend
//----------------------------------------------------------------------------------
#define MODE_NORMAL							(0)
#define MODE_EXTEND							(1)


//----------------------------------------------------------------------------------
// Mode Read/Write
//----------------------------------------------------------------------------------
#define MODE_READ							(0)
#define MODE_WRITE							(1)


//----------------------------------------------------------------------------------
// Mode Add/Delete
//----------------------------------------------------------------------------------
#define MODE_DELETE							(0)
#define MODE_ADD							(1)


//----------------------------------------------------------------------------------
// Mode Lower/Upper
//----------------------------------------------------------------------------------
#define MODE_LOWER							(0)
#define MODE_UPPER							(1)


//----------------------------------------------------------------------------------
// Link Status
//----------------------------------------------------------------------------------
#define MODE_LINK_DOWN						(0)
#define MODE_LINK_UP						(1)


//----------------------------------------------------------------------------------
// ROI Area Mode
//----------------------------------------------------------------------------------
#define ROI_AREA_MODE_DEFAULT_SIZE			(0)
#define ROI_AREA_MODE_FULL_SIZE				(1)
#define ROI_AREA_MODE_DMA_SIZE				(2)

#define ROI_AREA_MODE_MIN					ROI_AREA_MODE_DEFAULT_SIZE
#define ROI_AREA_MODE_MAX					ROI_AREA_MODE_DMA_SIZE


//----------------------------------------------------------------------------------
// ROI Adjust Mode
//----------------------------------------------------------------------------------
#define ROI_MODE_NORMAL						(0)
#define ROI_MODE_ADJUST						(1)


//----------------------------------------------------------------------------------
// Shutter Mode
//----------------------------------------------------------------------------------
#define SHUTTER_MODE_NORMAL					(0)
#define SHUTTER_MODE_TRIGGER				(1)


//----------------------------------------------------------------------------------
// Trans Mode
//----------------------------------------------------------------------------------
#define TRANS_MODE_ADD						(0)
#define TRANS_MODE_SUB						(1)


//----------------------------------------------------------------------------------
// Bit
//----------------------------------------------------------------------------------
#define MAX_BIT8							(255)
#define MAX_BIT10							(1023)
#define MAX_BIT12							(4095)
#define MAX_BIT14							(16383)


//----------------------------------------------------------------------------------
// GE Speed & Duplex
//----------------------------------------------------------------------------------
#define GIGE_ADV_NONE						(0x0000)	// No speeds to be advertised
#define GIGE_SPEED_10M_HD					(0x0001)	// 10BT half-duplex
#define GIGE_SPEED_10M_FD					(0x0002)	// 10BT full-duplex
#define GIGE_SPEED_100M_HD					(0x0004)	// 100BASE-TX half-duplex
#define GIGE_SPEED_100M_FD					(0x0008)	// 100BASE-TX full-duplex
#define GIGE_SPEED_1GIG_HD					(0x0010)	// 1000BASE-T half-duplex
#define GIGE_SPEED_1GIG_FD					(0x0020)	// 1000BASE-T full-duplex
#define GIGE_SPEED_10GIG_FD					(0x0040)	// 10GBASE-T full-duplex
#define GIGE_SPEED_2P5GIG_FD				(0x0800)	// 2.5GBASE-T full-duplex
#define GIGE_SPEED_5GIG_FD					(0x1000)	// 5GBASE-T full-duplex
#define GIGE_SPEED_AUTO_NEGOTIATION			(0xffff)	// Auto Negotiation


//----------------------------------------------------------------------------------
// Command
//----------------------------------------------------------------------------------

// arg option
#define OPT_NONE							(0)		// none option
#define OPT_LOOP							(1)		// loop count
#define OPT_NO_DISP							(2)		// No Display

// Command Function
#define CMD_MODE_NUMBER						(0)		// Number Mode
#define CMD_MODE_STRING						(1)		// String Mode

// Return Key Mode
#define RETURN_KEY							'\0'

// Manual/Batch Mode
#define MODE_BATCH							(0)		// 一括
#define MODE_MANUAL							(1)		// マニュアル
#define MODE_SEMI_BATCH						(2)		// 個別マニュアル

// Command Argument Number
#define CMD_ARG_NUM_MAX						(7)


//----------------------------------------------------------------------------------
// Unit
//----------------------------------------------------------------------------------
#define _1MHZ								(1000000)	// 1MHz
#define _1MS								(1000)		// 1ms
#define _1US								(1000000)	// 1us

#define SPECTRUM_BAND_WAVE_UNIT				(1000)
#define FFCADJUST_FRAME_UNIT				(100)
#define DPC_NONUNIFORM_UNIT					(100)
#define DPC_SD_UNIT							(100)
#define SENSOR_GAIN_UNIT					(10)
#define AUTO_GAIN_UNIT						(10)


//----------------------------------------------------------------------------------
// Dump Mode
//----------------------------------------------------------------------------------
#define MODE_ACES							(1)
#define MODE_DUMP							(2)


//----------------------------------------------------------------------------------
// Spectrum DefaultY Mode
//----------------------------------------------------------------------------------
#define SPECTRUM_DEFAULTY_MODE_DEFAULT		(0)
#define SPECTRUM_DEFAULTY_MODE_FULL_SIZE	(1)


//----------------------------------------------------------------------------------
// Align Mask
//----------------------------------------------------------------------------------
#define MALLOC_ALIGN						(16)
#define ALIGN_MASK_16Byte					(0x0f)
#define ALIGN_MASK_4Byte					(0x03)


//----------------------------------------------------------------------------------
// IO Access Macro
//----------------------------------------------------------------------------------
#define IN8(adrs)							(*(volatile unsigned char *)(unsigned long)adrs)
#define IN16(adrs)							(*(volatile unsigned short *)(unsigned long)adrs)
#define IN32(adrs)							(*(volatile unsigned int *)(unsigned long)adrs)
#define IN64(adrs)							(*(volatile unsigned long long *)(unsigned long)adrs)
#define OUT8(adrs,data)						((*(volatile unsigned char *)(unsigned long)adrs)=data)
#define OUT16(adrs,data)					((*(volatile unsigned short *)(unsigned long)adrs)=data)
#define OUT32(adrs,data)					((*(volatile unsigned int *)(unsigned long)adrs)=data)
#define OUT64(adrs,data)					((*(volatile unsigned long long *)adrs)=data)
#define INF(adrs)							(*(volatile float *)(unsigned long)adrs)
#define OUTF(adrs,data)						((*(volatile float *)(unsigned long)adrs)=data)


//----------------------------------------------------------------------------------
// SWAP Macro
//----------------------------------------------------------------------------------
#define SWAP_L(x)							((x>>24) | ((x&0xff0000)>>8) | ((x&0xff00)<<8) | (x<<24))
#define SWAP_W(x) 							(((x&0xff00)>>8) | ((x&0xff)<<8))


//----------------------------------------------------------------------------------
// sev Macro
//----------------------------------------------------------------------------------
#define sev() __asm__("sev")
#define wfi() __asm__("wfi")


//----------------------------------------------------------------------------------
// typedef
//----------------------------------------------------------------------------------
typedef unsigned long   u32;
typedef unsigned short  u16;
typedef unsigned char   u8;


//----------------------------------------------------------------------------------
// HW Init
//----------------------------------------------------------------------------------
#define HW_INIT_MODE_POWER_ON				(0)
#define HW_INIT_MODE_NOT_POWER_ON			(1)


//----------------------------------------------------------------------------------
// devType
//----------------------------------------------------------------------------------
#define FLASH_DEV_TYPE_QSPI_FLASH			(1)
#define FLASH_DEV_TYPE_AXI_QSPI_FLASH		(2)
#define FLASH_DEV_TYPE_PF					(3)
#define DEV_TYPE_MEMORY						(4)
#define DEV_TYPE_SENSOR_BSL					(5)
#define DEV_TYPE_SENSOR_BOARD				(6)
#define DEV_TYPE_GIGE_PHY					(7)
#define DEV_TYPE_SENSOR						(8)


//----------------------------------------------------------------------------------
// QSPI Flash
//----------------------------------------------------------------------------------

// Flash Select
#define QSPI_FLASH_SELECT_N25Q				(1)		// Micron
#define QSPI_FLASH_SELECT_ISSI				(2)		// ISSI
#define QSPI_FLASH_SELECT_W25Q				(3)		// Winbond

// READ ID

// Micron
#define QSPI_FLASH_READ_ID_MANUFACTURE_N25Q	(0x20)
#define QSPI_FLASH_READ_ID_DEVICE_N25Q		(0xBA)
#define QSPI_FLASH_READ_ID_SIZE_N25Q		(0x19)
#define QSPI_FLASH_READ_ID_SIZE_N25Q_512M	(0x20)

// ISSI
#define QSPI_FLASH_READ_ID_MANUFACTURE_ISSI	(0x9D)
#define QSPI_FLASH_READ_ID_DEVICE_ISSI		(0x60)

// Qspi Flash Device Select
#define QSPI_FLASH_DEVICE_SELECT_BOOT		(0)
#define QSPI_FLASH_DEVICE_SELECT_DATA		(1)


//----------------------------------------------------------------------------------
// PF Flash
//---------------------------------------------------------------------------------
#define PF_SPI_FLASH_SIZE					(16*1024*1024)
#define PF_SPI_FLASH_SEC_SIZE				(4*1024)
#define PF_SPI_FLASH_BLOCK_SIZE				(64*1024)


//----------------------------------------------------------------------------------
// I2Cポート番号
//----------------------------------------------------------------------------------
#define I2C_PORT0							(0)
#define I2C_PORT1							(1)

// IC2ポート番号Min/Max
#define I2C_PORT_MIN						(I2C_PORT0)
#define I2C_PORT_MAX						(I2C_PORT1)


//----------------------------------------------------------------------------------
// Temp
//----------------------------------------------------------------------------------
#define DEVICE_TEMP_UNIT					(100)			// 小数点を正数に変換

//----------------------------------------------------------------------------------
// Gain
//----------------------------------------------------------------------------------
#define DEVICE_GAIN_UNIT					(100)			// 小数点を正数に変換


//----------------------------------------------------------------------------------
// GainX
//----------------------------------------------------------------------------------
#define DEVICE_GAINX_UNIT					(10)			// 小数点を正数に変換


//----------------------------------------------------------------------------------
// Voltage
//----------------------------------------------------------------------------------
#define DEVICE_VOLT_UNIT					(1000)			// 小数点を正数に変換
#define DEVICE_CURRENT_UNIT					(1000)			// 小数点を正数に変換


//----------------------------------------------------------------------------------
// DPC Standard Deviation
//----------------------------------------------------------------------------------
#define DEVICE_SD_UNIT						(100)			// 小数点を正数に変換


//----------------------------------------------------------------------------------
// DPC Non Uniformity
//----------------------------------------------------------------------------------
#define DEVICE_NON_UNIFORM_UNIT				(100)			// 小数点を正数に変換


//----------------------------------------------------------------------------------
// PHY
//----------------------------------------------------------------------------------
#define PHY_FIRM_VERSION_SIZE				(16)
#define PHY_API_VERSION_SIZE				(8)

// Offset
#define FIRM_UPDATE_OFFSET					(0x1700)


//----------------------------------------------------------------------------------
// OS管理外データ
//----------------------------------------------------------------------------------
#define FIRM_DATA_CAMERA_ID_ADRS					(FIRM_DATA_ADRS+0x00)	// カメラ番号
#define FIRM_DATA_DPC_COUNT_ADRS					(FIRM_DATA_ADRS+0x04)	// DPCカウント
#define FIRM_DATA_DPC_NUM_ADRS						(FIRM_DATA_ADRS+0x08)	// 欠陥画素補正数
#define FIRM_DATA_DPC_MAX_NUM_COUNT_ADRS			(FIRM_DATA_ADRS+0x0c)	// 欠陥画素最大補正数
#define FIRM_DATA_LED_DEBUG_MODE_ADRS				(FIRM_DATA_ADRS+0x10)	// LEDデバックモード
#define FIRM_DATA_QSPI_BANK_FLAG_ADRS				(FIRM_DATA_ADRS+0x14)	// QSPI Bank Flag
#define FIRM_DATA_DPC_STATUS_ADRS					(FIRM_DATA_ADRS+0x18)	// DPC Status
#define FIRM_DATA_AXI_QSPI_BANK_FLAG_ADRS			(FIRM_DATA_ADRS+0x1c)	// AXI QSPI Bank Flag
#define FIRM_DATA_FFC_BLACK_ADRS					(FIRM_DATA_ADRS+0x20)	// FFC 黒レベルターゲット
#define FIRM_DATA_FFC_WHITE_ADRS					(FIRM_DATA_ADRS+0x24)	// FFC白レベルターゲット
#define FIRM_DATA_ACQUISITIO_RATE_MODE_ADRS			(FIRM_DATA_ADRS+0x28)	// Acquisition Rate Mode
#define FIRM_DATA_FFC_CORECTION_MODE_ADRS			(FIRM_DATA_ADRS+0x2C)	// FFC Correction Mode
#define FIRM_DATA_DPC_X_ADRS						(FIRM_DATA_ADRS+0x30)	// x欠陥座標
#define FIRM_DATA_DPC_Y_ADRS						(FIRM_DATA_ADRS+0x34)	// y欠陥座標
#define FIRM_DATA_UART_SEND0_COUNT_ADRS				(FIRM_DATA_ADRS+0x38)	// UART送信カウント
#define FIRM_DATA_UART_SEND1_COUNT_ADRS				(FIRM_DATA_ADRS+0x3c)	// UART送信カウント
#define FIRM_DATA_UART_RECV0_COUNT_ADRS				(FIRM_DATA_ADRS+0x40)	// UART受信カウント
#define FIRM_DATA_UART_RECV1_COUNT_ADRS				(FIRM_DATA_ADRS+0x44)	// UART受信カウント
#define FIRM_DATA_UART_RECV0_POP_COUNT_ADRS			(FIRM_DATA_ADRS+0x48)	// UART受信Popカウント
#define FIRM_DATA_UART_RECV1_POP_COUNT_ADRS			(FIRM_DATA_ADRS+0x4c)	// UART受信Popカウント
#define FIRM_DATA_UART_RECV0_PUSH_COUNT_ADRS		(FIRM_DATA_ADRS+0x50)	// UART受信Pushカウント
#define FIRM_DATA_UART_RECV1_PUSH_COUNT_ADRS		(FIRM_DATA_ADRS+0x54)	// UART受信Pushカウント
#define FIRM_DATA_UART_SEND0_FLAG_ADRS				(FIRM_DATA_ADRS+0x58)	// UART送信排他用Flag
#define FIRM_DATA_UART_RECV0_FLAG_ADRS				(FIRM_DATA_ADRS+0x5c)	// UART受信排他用Flag
#define FIRM_DATA_UART_SEND1_FLAG_ADRS				(FIRM_DATA_ADRS+0x60)	// UART送信排他用Flag
#define FIRM_DATA_UART_RECV1_FLAG_ADRS				(FIRM_DATA_ADRS+0x64)	// UART受信排他用Flag
#define FIRM_DATA_UART_CONSOLE_MODE_ADRS			(FIRM_DATA_ADRS+0x68)	// Console Mode
#define FIRM_DATA_XML_SCHEMA_VERSION_MODE_ADRS		(FIRM_DATA_ADRS+0x6c)	// XML Schema Version Mode
#define FIRM_DATA_FFC_ADJUST_BLACK_ADRS				(FIRM_DATA_ADRS+0x70)	// FFC 黒レベルターゲット
#define FIRM_DATA_FFC_ADJUST_WHITE_ADRS				(FIRM_DATA_ADRS+0x74)	// FFC白レベルターゲット
#define FIRM_DATA_FFC_ADJUST_BLACK_RATE_ADRS		(FIRM_DATA_ADRS+0x78)	// FFC 黒レベルレート
#define FIRM_DATA_FFC_ADJUST_WHITE_RATE_ADRS		(FIRM_DATA_ADRS+0x7c)	// FFC白レベルレート
#define FIRM_DATA_FFC_ADJUST_BLACK_EXP_ADRS			(FIRM_DATA_ADRS+0x80)	// FFC 黒レベル露光時間
#define FIRM_DATA_FFC_ADJUST_WHITE_EXP_ADRS			(FIRM_DATA_ADRS+0x84)	// FFC白レベル露光時間
#define FIRM_DATA_FFC_ADJUST_BLACK_TEMP_ADRS		(FIRM_DATA_ADRS+0x88)	// FFC 黒レベル温度
#define FIRM_DATA_FFC_ADJUST_WHITE_TEMP_ADRS		(FIRM_DATA_ADRS+0x8c)	// FFC白レベル温度
#define FIRM_DATA_FFC_ADJUST_BLACK_GAIN_ADRS		(FIRM_DATA_ADRS+0x90)	// FFC 黒レベルGain
#define FIRM_DATA_FFC_ADJUST_WHITE_GAIN_ADRS		(FIRM_DATA_ADRS+0x94)	// FFC白レベルGain
#define FIRM_DATA_FFC_ADJUST_BLACK_BIT_ADRS			(FIRM_DATA_ADRS+0x98)	// FFC 黒レベルBit
#define FIRM_DATA_FFC_ADJUST_WHITE_BIT_ADRS			(FIRM_DATA_ADRS+0x9c)	// FFC白レベルBit
#define FIRM_DATA_FIRM_BOOT_FLAG_ADRS				(FIRM_DATA_ADRS+0xa0)	// Boot Flag(0=起動時/1=起動済)
#define FIRM_DATA_FIRM_CMD_STATUS_ADRS				(FIRM_DATA_ADRS+0xa4)	// Firm Cmd Status
#define FIRM_DATA_QSPI_FLASH_TYPE_ADRS				(FIRM_DATA_ADRS+0xa8)	// QSPI Flash Device Type
#define FIRM_DATA_AXI_QSPI_FLASH_TYPE_ADRS			(FIRM_DATA_ADRS+0xac)	// AXI QSPI Flash Device Type
#define FIRM_DATA_SPECTRUM_BANDVALUE_RANGE_ADRS		(FIRM_DATA_ADRS+0xb0)	// Spectrum Band Value Range
#define FIRM_DATA_GIGE_FRAME_BUFF_ADRS				(FIRM_DATA_ADRS+0xb4)	// GigE Frame Buffer Address
#define FIRM_DATA_SENSOR_POWER_STATUS_ADRS			(FIRM_DATA_ADRS+0xb8)	// Sensor Power Status
#define FIRM_DATA_CAMERA_ERROR_SELECT_ADRS			(FIRM_DATA_ADRS+0xbc)	// Camera Error Select
#define FIRM_DATA_CAMERA_ERROR_STATUS_ADRS			(FIRM_DATA_ADRS+0xc0)	// Camera Error Status
#define FIRM_DATA_BLACKPIXEL_MODE_ADRS				(FIRM_DATA_ADRS+0xc4)	// Black Pixel Mode
#define FIRM_DATA_FLIP_FFC_MODE_ADRS				(FIRM_DATA_ADRS+0xc8)	// Flip FFC Mode(FFCデータを変換するしないを判断)
#define FIRM_DATA_FLIP_DPC_MODE_ADRS				(FIRM_DATA_ADRS+0xcc)	// Flip DPC Mode(DPCデータを変換するしないを判断)
#define FIRM_DATA_ROI_DEFAULT_ADRS					(FIRM_DATA_ADRS+0xd0)	// ROI Default
#define FIRM_DATA_DPC_NUM_VERSION2_ADRS				(FIRM_DATA_ADRS+0xd4)	// DPC Pixel Count(有効な領域内のみの数(Fullサイズではない))
#define FIRM_DATA_DPC_MAPINFO_MODE_ADRS				(FIRM_DATA_ADRS+0xd8)	// DPC Pixel Map Info Mode
#define FIRM_DATA_FLIP_MODE_ADRS					(FIRM_DATA_ADRS+0xdc)	// Flip Mode
#define FIRM_DATA_SENSOR_GAIN_ADRS					(FIRM_DATA_ADRS+0xc0)	// Sensor Gain X
#define FIRM_DATA_PELTIER_MOUNT_STATE_ADRS			(FIRM_DATA_ADRS+0xc4)	// Peltirt Mount State
#define FIRM_DATA_DPC_ADJUST_MODE_ADRS				(FIRM_DATA_ADRS+0xc8)	// DPC Adjust Mode
#define FIRM_DATA_VARIANT_MODE_ADRS					(FIRM_DATA_ADRS+0xcc)
#define FIRM_DATA_EXPOSURE_DEFAULT					(FIRM_DATA_ADRS+0xd0)
#define FIRM_DATA_EXPOSURE2_DEFAULT					(FIRM_DATA_ADRS+0xd4)
#define FIRM_DATA_HIGHSPPED_INIT2_ADRS				(FIRM_DATA_ADRS+0xd8)
#define FIRM_DATA_CPU0_MAIN_ACCESS_FLAG				(FIRM_DATA_ADRS+0xdc)
#define FIRM_DATA_TG_CTRL_MODE						(FIRM_DATA_ADRS+0xe0)
#define FIRM_DATA_GRADATION_COMPRESS_MODE2			(FIRM_DATA_ADRS+0xe4)
#define FIRM_DATA_CPU1_BOOT_FLAG					(FIRM_DATA_ADRS+0xe8)
#define FIRM_DATA_FRAME_RATE_DEFAULT				(FIRM_DATA_ADRS+0xec)
#define FIRM_DATA_FRAME_RATE						(FIRM_DATA_ADRS+0xf0)
#define FIRM_DATA_FRAME_RATE_HIGH_SPEED_MODE_ADRS	(FIRM_DATA_ADRS+0xf4)	// Frame Rate High Speed Mode
#define FIRM_DATA_FRAME_RATE_HIGH_SPEED_HEIGHT_ADRS (FIRM_DATA_ADRS+0xf8)	// Frame Rate High Speed Virtual Height
#define FIRM_DATA_SENSOR_PERTIER_MODE				(FIRM_DATA_ADRS+0xfc)
#define FIRM_DATA_VENDOR_ADRS						(FIRM_DATA_ADRS+0x100)	// ベンダ名
#define FIRM_DATA_MANUFACTURE_ADRS					(FIRM_DATA_ADRS+0x140)	// 製造者名
#define FIRM_DATA_MODEL_ADRS						(FIRM_DATA_ADRS+0x180)	// モデル名
#define FIRM_DATA_BOARDID_ADRS						(FIRM_DATA_ADRS+0x1c0)	// ボードID
#define FIRM_DATA_SENSORID_ADRS						(FIRM_DATA_ADRS+0x1e0)	// センサID
#define FIRM_DATA_SENSOR_FIRM_VERSION_ADRS			(FIRM_DATA_ADRS+0x200)	//0x000から0x008まで
#define FIRM_DATA_BOARD_VERSION_ADRS				(FIRM_DATA_ADRS+0x208)	// ボードバージョン(4Byte)
#define FIRM_DATA_DPC_ADJUST_CORMODE_ADRS			(FIRM_DATA_ADRS+0x20c)	// DPC調整時の補正モード
#define FIRM_DATA_DPC_ADJUST_FFC0_ADRS				(FIRM_DATA_ADRS+0x210)	// DPC調整時のパラメータ(FFC0用)
#define FIRM_DATA_DPC_ADJUST_FFC1_ADRS				(FIRM_DATA_ADRS+0x260)	// DPC調整時のパラメータ(FFC1用)
#define FIRM_DATA_DPC_ADJUST_FFC2_ADRS				(FIRM_DATA_ADRS+0x2b0)	// DPC調整時のパラメータ(FFC2用)
#define FIRM_DATA_DPC_ADJUST_FFC3_ADRS				(FIRM_DATA_ADRS+0x300)	// DPC調整時のパラメータ(FFC3用)
#define FIRM_DATA_DPC_ADJUST_FFC_SIZE				(0x50)					// DPC調整時のパラメータサイズ
	#define FIRM_DATA_DPC_ADJUST_RATE_ADRS			(0x00)					// DPC調整時のレート
	#define FIRM_DATA_DPC_ADJUST_EXPOSURE_ADRS		(0x04)					// DPC調整時の露光時間
	#define FIRM_DATA_DPC_ADJUST_TEMP_ADRS			(0x08)					// DPC調整時の温度
	#define FIRM_DATA_DPC_ADJUST_FFC_NUMBER_ADRS	(0x0C)					// DPC調整時のFFC番号
	#define FIRM_DATA_DPC_ADJUST_SD0_ADRS			(0x10)					// DPC調整時の標準偏差
	#define FIRM_DATA_DPC_ADJUST_SD1_ADRS			(0x14)					// DPC調整時の標準偏差
	#define FIRM_DATA_DPC_ADJUST_SD2_ADRS			(0x18)					// DPC調整時の標準偏差
	#define FIRM_DATA_DPC_ADJUST_SD3_ADRS			(0x1C)					// DPC調整時の標準偏差
	#define FIRM_DATA_DPC_ADJUST_SD4_ADRS			(0x20)					// DPC調整時の標準偏差
	#define FIRM_DATA_DPC_ADJUST_SD5_ADRS			(0x24)					// DPC調整時の標準偏差
	#define FIRM_DATA_DPC_ADJUST_SD6_ADRS			(0x28)					// DPC調整時の標準偏差
	#define FIRM_DATA_DPC_ADJUST_SD7_ADRS			(0x2C)					// DPC調整時の標準偏差
	#define FIRM_DATA_DPC_ADJUST_NON_UNIFORM0_ADRS	(0x30)					// DPC調整時の感度不均一
	#define FIRM_DATA_DPC_ADJUST_NON_UNIFORM1_ADRS	(0x34)					// DPC調整時の感度不均一
	#define FIRM_DATA_DPC_ADJUST_NON_UNIFORM2_ADRS	(0x38)					// DPC調整時の感度不均一
	#define FIRM_DATA_DPC_ADJUST_NON_UNIFORM3_ADRS	(0x3C)					// DPC調整時の感度不均一
	#define FIRM_DATA_DPC_ADJUST_NON_UNIFORM4_ADRS	(0x40)					// DPC調整時の感度不均一
	#define FIRM_DATA_DPC_ADJUST_NON_UNIFORM5_ADRS	(0x44)					// DPC調整時の感度不均一
	#define FIRM_DATA_DPC_ADJUST_NON_UNIFORM6_ADRS	(0x48)					// DPC調整時の感度不均一
	#define FIRM_DATA_DPC_ADJUST_NON_UNIFORM7_ADRS	(0x4C)					// DPC調整時の感度不均一

#define FIRM_DATA_ROI_BASE_ADRS						(FIRM_DATA_ADRS+0x500)			// ROI Base
#define FIRM_DATA_ROI_BASE_SIZE						(0x80)

 #define FIRM_DATA_ROI_X0_ADRS						(FIRM_DATA_ROI_BASE_ADRS+0x00)	// ROI X0 Size

 #define FIRM_DATA_ROI_Y0_ADRS						(FIRM_DATA_ROI_BASE_ADRS+0x20)	// ROI Y0 Size
 #define FIRM_DATA_ROI_Y1_ADRS						(FIRM_DATA_ROI_BASE_ADRS+0x24)	// ROI Y1 Size
 #define FIRM_DATA_ROI_Y2_ADRS						(FIRM_DATA_ROI_BASE_ADRS+0x28)	// ROI Y2 Size
 #define FIRM_DATA_ROI_Y3_ADRS						(FIRM_DATA_ROI_BASE_ADRS+0x2C)	// ROI Y3 Size
 #define FIRM_DATA_ROI_Y4_ADRS						(FIRM_DATA_ROI_BASE_ADRS+0x30)	// ROI Y4 Size
 #define FIRM_DATA_ROI_Y5_ADRS						(FIRM_DATA_ROI_BASE_ADRS+0x34)	// ROI Y5 Size
 #define FIRM_DATA_ROI_Y6_ADRS						(FIRM_DATA_ROI_BASE_ADRS+0x38)	// ROI Y6 Size
 #define FIRM_DATA_ROI_Y7_ADRS						(FIRM_DATA_ROI_BASE_ADRS+0x3C)	// ROI Y7 Size
 #define FIRM_DATA_ROI_UNIT_SIZE					(0x4)

#define FIRM_DATA_ACQUISITION_START_ADRS			(FIRM_DATA_ADRS+0x580)	// Acquisition Start
#define FIRM_DATA_WIDTH_ADRS						(FIRM_DATA_ADRS+0x584)	// Width
#define FIRM_DATA_HEIGHT_ADRS						(FIRM_DATA_ADRS+0x588)	// Height
#define FIRM_DATA_BIT_ADRS							(FIRM_DATA_ADRS+0x58c)	// Bit
#define FIRM_DATA_ROI_SELECTOR_ADRS					(FIRM_DATA_ADRS+0x590)	// ROI Selector
#define FIRM_DATA_ROI_COUNT_ADRS					(FIRM_DATA_ADRS+0x594)	// ROI Entry Count
#define FIRM_DATA_ROI_AREA_MODE_ADRS				(FIRM_DATA_ADRS+0x598)	// ROI_Area Mode
#define FIRM_DATA_ROI_SAVE_RESTORE_ADRS				(FIRM_DATA_ADRS+0x59c)	// ROI Save Restore
#define FIRM_DATA_ROI_AREA_SIZE_ADRS				(FIRM_DATA_ADRS+0x5a0)	// ROI Area Size

#define FIRM_DATA_ROI_CAMERA_WIDTH_TOTAL_ADRS		(FIRM_DATA_ADRS+0x5a8)	// ROI Camera Width Total Size
#define FIRM_DATA_ROI_CAMERA_HEIGHT_TOTAL_ADRS		(FIRM_DATA_ADRS+0x5ac)	// ROI Camera Height Total Size
#define FIRM_DATA_ROI_SENSOR_WIDTH_TOTAL_ADRS		(FIRM_DATA_ADRS+0x5b0)	// ROI Sensor Width Total Size
#define FIRM_DATA_ROI_SENSOR_HEIGHT_TOTAL_ADRS		(FIRM_DATA_ADRS+0x5b4)	// ROI Sensor Height Total Size
#define FIRM_DATA_AUTO_BRIGHT_EXP_MODE_ADRS			(FIRM_DATA_ADRS+0x5b8)	// Auto Bright Exposure Mode
#define FIRM_DATA_AUTO_BRIGHT_EXP_STATUS_ADRS		(FIRM_DATA_ADRS+0x5bc)	// Auto Bright Exposure Once Status
	#define AUTO_BRIGHT_MODE_DISABLE				(0)
	#define AUTO_BRIGHT_MODE_ONCE					(1)
	#define AUTO_BRIGHT_MODE_CONTINUE				(2)
	#define AUTO_BRIGHT_MODE_MIN					(AUTO_BRIGHT_MODE_DISABLE)
	#define AUTO_BRIGHT_MODE_MAX					(AUTO_BRIGHT_MODE_CONTINUE)
#define FIRM_DATA_AUTO_BRIGHT_EXPOSUER_MIN			(FIRM_DATA_ADRS+0x5c0)	// Auto Bright Exposure Min
#define FIRM_DATA_AUTO_BRIGHT_EXPOSUER_MAX			(FIRM_DATA_ADRS+0x5c4)	// Auto Bright Exposure Max
#define FIRM_DATA_AUTO_BRIGHT_GAIN_MODE_ADRS		(FIRM_DATA_ADRS+0x5c8)	// Auto Bright Gain Mode
#define FIRM_DATA_AUTO_BRIGHT_GAIN_STATUS_ADRS		(FIRM_DATA_ADRS+0x5cc)	// Auto Bright Gain Once Status
#define FIRM_DATA_AUTO_BRIGHT_GAIN_MIN				(FIRM_DATA_ADRS+0x5d0)	// Auto Bright Gain Min
#define FIRM_DATA_AUTO_BRIGHT_GAIN_MAX				(FIRM_DATA_ADRS+0x5d4)	// Auto Bright Gain Max
#define FIRM_DATA_GRADATION_COMPRESS_POS_FIRST		(FIRM_DATA_ADRS+0x5d8)	// Gradation Compress Position First
#define FIRM_DATA_GRADATION_COMPRESS_POS_SECOND		(FIRM_DATA_ADRS+0x5dc)	// Gradation Compress Position Second
#define FIRM_DATA_GRADATION_COMPRESS_GAIN_FIRST		(FIRM_DATA_ADRS+0x5e0)	// Gradation Compress Gain First
#define FIRM_DATA_GRADATION_COMPRESS_GAIN_SECOND	(FIRM_DATA_ADRS+0x5e4)	// Gradation Compress Gain Second
#define FIRM_DATA_GRADATION_COMPRESS_MODE			(FIRM_DATA_ADRS+0x5e8)	// Gradation Compress Mode
#define FIRM_DATA_GRADATION_COMPRESS_MODE_PRE		(FIRM_DATA_ADRS+0x5ec)	// Gradation Compress Mode Pre
#define FIRM_DATA_MAX_FRAME_RATE_TIME				(FIRM_DATA_ADRS+0x5f0)	// Max Frame Rate Time
#define FIRM_DATA_8BIT_MODE							(FIRM_DATA_ADRS+0x5f4)	// 8Bit Mode
#define FIRM_DATA_AUTO_BRIGHT_TARGET_AREA			(FIRM_DATA_ADRS+0x5f8)	// Auto Bright Target Area
	#define AUTO_BRIGHT_TARGET_AREA_ALL_RANGE		(0)
	#define AUTO_BRIGHT_TARGET_AREA_CENTER			(1)
	#define	AUTO_BRIGHT_TARGET_AREA_TOP				(2)
	#define AUTO_BRIGHT_TARGET_AREA_BOTTOM			(3)
	#define AUTO_BRIGHT_TARGET_AREA_LEFT			(4)
	#define AUTO_BRIGHT_TARGET_AREA_RIGHT			(5)
#define AUTO_BRIGHT_TARGET_AREA_MIN					(AUTO_BRIGHT_TARGET_AREA_ALL_RANGE)
#define AUTO_BRIGHT_TARGET_AREA_MAX					(AUTO_BRIGHT_TARGET_AREA_RIGHT)
#define FIRM_DATA_AUTO_BRIGHT_USERSET_SIZE			(FIRM_DATA_ADRS+0x5fc)
#define FIRM_DATA_AUTO_BRIGHT_USERSET_OFFSET		(FIRM_DATA_ADRS+0x600)
#define FIRM_DATA_SPI_PL_BANK_FLAG_ADRS				(FIRM_DATA_ADRS+0x604)
#define FIRM_DATA_BIT8_CONVERT_MODE					(FIRM_DATA_ADRS+0x608)
//#define FIRM_DATA_CXP_SEND_DATA_COUNT				(FIRM_DATA_ADRS+0x60c)
//#define FIRM_DATA_CXP_RECV_DATA_COUNT				(FIRM_DATA_ADRS+0x610)
//#define FIRM_DATA_CXP_DATA_COUNT					(FIRM_DATA_ADRS+0x614)
//#define FIRM_DATA_CXP_SEND_FPGA_ADRS				(FIRM_DATA_ADRS+0x618)
#define FIRM_DATA_IF_VERSION_ADRS					(FIRM_DATA_ADRS+0x61c)
#define FIRM_DATA_CXP_LED1_ADRS						(FIRM_DATA_ADRS+0x620)
#define FIRM_DATA_TRG_SOFT_COUNT_ADRS				(FIRM_DATA_ADRS+0x624)
#define FIRM_DATA_FFC_WHITE_GAINX_ADRS				(FIRM_DATA_ADRS+0x628)
#define FIRM_DATA_CXP_COMPLIANCE_TEST_MODE_ADRS		(FIRM_DATA_ADRS+0x62c)
#define FIRM_DATA_PIXEL_DYNAMIC_RANGE_MAX			(FIRM_DATA_ADRS+0x630)	// Pixel Dynamic Range Max
	#define FIRM_DATA_PIXEL_DYNAMIC_GC_DISABLE		(255)
	#define FIRM_DATA_PIXEL_DYNAMIC_GC_ENABLE		(1023)

#define FIRM_DATA_GEV_SCPS_PACKET_SIZE_ADRS			(FIRM_DATA_ADRS+0x634)	// GEV Packet Size
#define FIRM_DATA_GEV_SCPD_PACKET_DELAY_ADRS		(FIRM_DATA_ADRS+0x638)	// GEV Packet Delay
#define FIRM_DATA_GEV_SPEED_ADRS					(FIRM_DATA_ADRS+0x63c)	// GEV Speed

#define FIRM_CXP_PORT_ADRS							(FIRM_DATA_ADRS+0x640)
#define FIRM_DATA_CXP_DATA_COUNT_MULTI_ADRS			(FIRM_DATA_ADRS+0x644)
#define FIRM_DATA_CXP_SEND_DATA_COUNT_MULTI_ADRS	(FIRM_DATA_ADRS+0x648)
#define FIRM_DATA_CXP_RECV_DATA_COUNT_MULTI_ADRS	(FIRM_DATA_ADRS+0x650)	//0x650から0x67Fまで
#define FIRM_DATA_CXP_RATE_ADRS						(FIRM_DATA_ADRS+0x680)
#define FIRM_DATA_SENSOR_CONVERSION_GAIN_ADRS		(FIRM_DATA_ADRS+0x684)
#define FIRM_DATA_DDR_DIAG_FLAG						(FIRM_DATA_ADRS+0x68c)
#define FIRM_DATA_DDR_DIAG_FLAG_NONE				(0)
#define FIRM_DATA_DDR_DIAG_FLAG_MALLOC				(1)
#define FIRM_DATA_DDR_DIAG_FLAG_MALLOC_DONE			(2)
#define FIRM_DATA_DDR_DIAG_FLAG_FREE				(3)
#define FIRM_DATA_DDR_DIAG_FLAG_FREE_DONE			(4)

//#define FIRM_DATA_DDR_DIAG_ADRS						(FIRM_DATA_ADRS+0x690)
//#define FIRM_DATA_DDR_DIAG_SIZE						(FIRM_DATA_ADRS+0x694)
#define FIRM_DATA_DIAG_DDR_MODE						(FIRM_DATA_ADRS+0x698)
#define FIRM_DATA_PIXEL_FORMAT						(FIRM_DATA_ADRS+0x69c)
#define FIRM_DATA_SENSOR_SHUTTER_MODE				(FIRM_DATA_ADRS+0x670)
#define FIRM_DATA_SENSOR_DRRS_MODE					(FIRM_DATA_ADRS+0x674)
#define FIRM_DATA_EXPOSURE_TIME						(FIRM_DATA_ADRS+0x678)
#define FIRM_DATA_UPDATE_SIZE						(FIRM_DATA_ADRS+0x67c)
#define FIRM_DATA_PHY_DATA_SIZE						(FIRM_DATA_ADRS+0x680)
#define FIRM_DATA_CXP_CONNECTION_CONFIG				(FIRM_DATA_ADRS+0x684)
#define FIRM_DATA_GE_SPEED							(FIRM_DATA_ADRS+0x688)
#define FIRM_DATA_TEMP_ABNORMAL_STATUS				(FIRM_DATA_ADRS+0x68c)
#define FIRM_DATA_TEMP_ABNORMAL_COUNT				(FIRM_DATA_ADRS+0x690)
#define FIRM_DATA_TEMP_ABNORMAL_ACQUISITIONT		(FIRM_DATA_ADRS+0x694)
#define FIRM_DATA_PELTIER_POWER_LEVEL				(FIRM_DATA_ADRS+0x698)
#define FIRM_DATA_PELTIER_POWER_INTERVAL			(FIRM_DATA_ADRS+0x69c)
#define FIRM_DATA_PELTIER_POWER_HIGH_CLIP			(FIRM_DATA_ADRS+0x6a0)
#define FIRM_DATA_PELTIER_POWER_START_FLAG			(FIRM_DATA_ADRS+0x6a4)
#define FIRM_DATA_CMD_EXE_FLAG						(FIRM_DATA_ADRS+0x6a8)
#define FIRM_DATA_TEST_PATTERN						(FIRM_DATA_ADRS+0x6aC)
#define FIRM_DATA_PHY_FIRM_VERSION					(FIRM_DATA_ADRS+0x6b0)	// 16byte
#define FIRM_DATA_PHY_API_VERSION					(FIRM_DATA_ADRS+0x6c0)	// 8byte
#define FIRM_DATA_BOOT_VERSION						(FIRM_DATA_ADRS+0x6c8)	// 8byte


//----------------------------------------------------------------------------------
// 初期化エラー情報格納 Map
//----------------------------------------------------------------------------------
#define BOARD_STATUS_INTC_ADRS				(BOARD_ERROR_ADRS+0x00)		// INTC
#define BOARD_STATUS_TIMER_ADRS				(BOARD_ERROR_ADRS+0x04)		// Timer
#define BOARD_STATUS_QSPI_ADRS				(BOARD_ERROR_ADRS+0x08)		// QSPI
#define BOARD_STATUS_AXI_QSPI_ADRS			(BOARD_ERROR_ADRS+0x0c)		// QSPI AXI
#define BOARD_STATUS_I2C_ADRS				(BOARD_ERROR_ADRS+0x10)		// I2C
#define BOARD_STATUS_UART_ADRS				(BOARD_ERROR_ADRS+0x14)		// UART
#define BOARD_STATUS_DMA_ADRS				(BOARD_ERROR_ADRS+0x18)		// DMA
#define BOARD_STATUS_ROI_ADRS				(BOARD_ERROR_ADRS+0x1c)		// ROI
#define BOARD_STATUS_SENSOR_TIMING_ADRS		(BOARD_ERROR_ADRS+0x20)		// Sensor Timing
#define BOARD_STATUS_SENSOR_ADRS			(BOARD_ERROR_ADRS+0x24)		// Sensor
#define BOARD_STATUS_PELTIER_ADRS			(BOARD_ERROR_ADRS+0x28)		// PELTIER
#define BOARD_STATUS_ACQUISITION_ADRS		(BOARD_ERROR_ADRS+0x2c)		// Acquisition
#define BOARD_STATUS_DIGITALIO_ADRS			(BOARD_ERROR_ADRS+0x30)		// Digital IO
#define BOARD_STATUS_COUNTER_ADRS			(BOARD_ERROR_ADRS+0x34)		// Counter Control
#define BOARD_STATUS_TIMER_CONTROL_ADRS		(BOARD_ERROR_ADRS+0x38)		// Timer Control
#define BOARD_STATUS_AOI_ADRS				(BOARD_ERROR_ADRS+0x3c)		// AOI
#define BOARD_STATUS_FFC_ADRS				(BOARD_ERROR_ADRS+0x40)		// FFC
#define BOARD_STATUS_DPC_ADRS				(BOARD_ERROR_ADRS+0x44)		// DPC
#define BOARD_STATUS_LUT_ADRS				(BOARD_ERROR_ADRS+0x48)		// LUT
#define BOARD_STATUS_DOG_ADRS				(BOARD_ERROR_ADRS+0x4c)		// Digital Offset Gain
#define BOARD_STATUS_USERSET_ADRS			(BOARD_ERROR_ADRS+0x50)		// UserSet
#define BOARD_STATUS_SENSOR_TEMP_UPPER_ADRS	(BOARD_ERROR_ADRS+0x54)		// Sensor Upper Temp
#define BOARD_STATUS_SENSOR_TEMP_LOWER_ADRS	(BOARD_ERROR_ADRS+0x58)		// Sensor Lower Temp
#define BOARD_STATUS_CASE_TEMP_ADRS			(BOARD_ERROR_ADRS+0x5c)		// Case Temp
#define BOARD_STATUS_TEMP_ADRS				(BOARD_ERROR_ADRS+0x60)		// Temp Status
#define BOARD_STATUS_ENCODER_CONTROL_ADRS	(BOARD_ERROR_ADRS+0x64)		// Encoder Control
#define BOARD_STATUS_TIMING_GENERATOR_ADRS	(BOARD_ERROR_ADRS+0x68)		// Timing Generator
#define BOARD_STATUS_SPECTRUM_ADRS			(BOARD_ERROR_ADRS+0x6c)		// Spectrum
#define BOARD_STATUS_BOARD_VOLT_ADRS		(BOARD_ERROR_ADRS+0x70)		// Board Voltage
#define BOARD_STATUS_AUTO_BRIGHT_ADRS		(BOARD_ERROR_ADRS+0x74)		// Auto Bright
#define BOARD_STATUS_AGING_ADRS				(BOARD_ERROR_ADRS+0x78)		// Aging Status

//----------------------------------------------------------------------------------
// 初期化エラー情報番号
//----------------------------------------------------------------------------------
#define BOARD_STATUS_NUMBER_INTC				(0)						// INTC
#define BOARD_STATUS_NUMBER_TIMER				(1)						// Timer
#define BOARD_STATUS_NUMBER_QSPI				(2)						// QSPI
#define BOARD_STATUS_NUMBER_AXI_QSPI			(3)						// AXI QSPI
#define BOARD_STATUS_NUMBER_I2C					(4)						// I2C
#define BOARD_STATUS_NUMBER_UART				(5)						// UART
#define BOARD_STATUS_NUMBER_DMA					(6)						// DMA
#define BOARD_STATUS_NUMBER_ROI					(7)						// ROI
#define BOARD_STATUS_NUMBER_LVDS				(8)						// LVDS
#define BOARD_STATUS_NUMBER_SENSOR				(9)						// Sensor
#define BOARD_STATUS_NUMBER_PELTIER				(10)					// Peltier
#define BOARD_STATUS_NUMBER_ACQUISITION			(11)					// Acquisition
#define BOARD_STATUS_NUMBER_DIGITALIO			(12)					// Digital IO
#define BOARD_STATUS_NUMBER_COUNTER				(13)					// Counter Control
#define BOARD_STATUS_NUMBER_TIMERCONTROL		(14)					// Timer Control
#define BOARD_STATUS_NUMBER_AOI					(15)					// AOI
#define BOARD_STATUS_NUMBER_FFC					(16)					// FFC
#define BOARD_STATUS_NUMBER_DPC					(17)					// DPC
#define BOARD_STATUS_NUMBER_LUT					(18)					// LUT
#define BOARD_STATUS_NUMBER_DOG					(10)					// Digital Offset Gain
#define BOARD_STATUS_NUMBER_USERSET				(20)					// User Set
#define BOARD_STATUS_NUMBER_SENSOR_UPPER_TEMP	(21)					// Sensor Upper Temp
#define BOARD_STATUS_NUMBER_SENSOR_LOWER_TEMP	(22)					// Sensor Lower Temp
#define BOARD_STATUS_NUMBER_CASE_TEMP			(23)					// Case Temp
#define BOARD_STATUS_NUMBER_TEMP_STATUS			(24)					// Temp Status
#define BOARD_STATUS_NUMBER_ENCODER				(25)					// Encoder
#define BOARD_STATUS_NUMBER_TIMING_GENERATOR	(26)					// Timing Generator
#define BOARD_STATUS_NUMBER_SPECTRUM			(27)					// Spectrum
#define BOARD_STATUS_NUMBER_BOARD_VOLT			(28)					// Board Voltage
#define BOARD_STATUS_NUMBER_AUTO_BRIGHT			(29)					// Auto Bright
#define BOARD_STATUS_NUMBER_AGING				(30)					// Aging

#define BOARD_STATUS_NUMBER_MIN					(BOARD_STATUS_NUMBER_INTC)
#define BOARD_STATUS_NUMBER_MAX					(BOARD_STATUS_NUMBER_AGING)


//----------------------------------------------------------------------------------
// Board Parameter Address Map
//----------------------------------------------------------------------------------

// 全領域
#define BOARD_PARAM_SIZE					(64*1024)

// Address Map
#define BOARD_PARAM_BOARD_ID_ADRS			(0x00)
#define BOARD_PARAM_SENSOR_ID_ADRS			(0x20)
#define BOARD_PARAM_VENDOR_ADRS				(0x40)
#define BOARD_PARAM_MANUFACTURE_ADRS		(0x80)
#define BOARD_PARAM_MODEL_ADRS				(0xc0)
#define BOARD_PARAM_BOARD_VERSION_ADRS		(0x100)

// Size
#define BOARD_PARAM_BOARD_ID_SIZE			(10)
#define BOARD_PARAM_VENDOR_SIZE				(64)
#define BOARD_PARAM_MANUFACTURE_SIZE		(64)
#define BOARD_PARAM_MODEL_SIZE				(64)
#define BOARD_PARAM_ALIGN					(32)
#define BOARD_PARAM_BOARD_VERSION_SIZE		(4)


//----------------------------------------------------------------------------------
// FFC
//----------------------------------------------------------------------------------

// FFC Adjust Level
#define FFC_ADJUST_BLACK					(0)
#define FFC_ADJUST_WHITE					(1)

// Unit Size
#define FFC_DATA_UINT_SIZE					(4)

// 個数
#define FFC_DATA_NUM						(CAMERA_WIDTH_MAX*CAMERA_HEIGHT_MAX)

// FFC Factory
#define FFC_FACTORY_NUMBER					(0)

// FFC Default
#define FFC_DEFAULT_NUMBER					(0)

// FFC個数(User時)
#define FFC_NUM_USER						(3)

// FFC Offset/Gain
#define FFC_OFFSET							(0)
#define FFC_GAIN							(1)

// FFC Memory Type
#define FFC_MEMORY_EXT						(0)		// OS管理外
#define FFC_MEMORY_INT						(1)		// OS管理内(malloc)

// FFC Admin/User
#define FFC_USER							(0)
#define FFC_ADMIN							(1)

// FFC Gainデータ切捨て（小数点第5位）
#define FFC_GAIN_X_UNIT						(100000)

// FFC Gainデータ四捨五入
#define FFC_GAIN_X_ROUNDING					(0.000005)

// FFC Mode
#define FFC_CORRECTION_MODE_FIRST			(1)
#define FFC_CORRECTION_MODE_SECOND			(2)
#define FFC_CORRECTION_MODE_SHADING			(3)
#define FFC_CORRECTION_MODE_SHADING_LINE	(4)

// FFC Gain Checkデータ切捨て（小数点第5位）
#define FFC_GAIN_CHECK_UNIT					(1000)

// FFC Offset Check
#define FFC_OFFSET_CHECK_DATA_MIN			(0)
#define FFC_OFFSET_CHECK_DATA_MAX			(16384)

// FFC Gain Check
#define	FFC_GAIN_CHECK_DATA_MIN				(0.0)
#define	FFC_GAIN_CHECK_DATA_MAX				(4.0)


//----------------------------------------------------------------------------------
// FFC補正情報
//----------------------------------------------------------------------------------

// Size
 #define FFC_SAVE_INFO_SIZE					(0x40)

// Black Address Map
#define FFC_SAVE_BLACK_FRAME_H				(0x00)
#define FFC_SAVE_BLACK_FRAME_L				(0x04)
#define FFC_SAVE_BLACK_EXPOSURE				(0x08)
#define FFC_SAVE_BLACK_TARGET				(0x0c)
#define FFC_SAVE_BLACK_TEMP_H				(0x10)
#define FFC_SAVE_BLACK_TEMP_L				(0x14)

// White Address Map
#define FFC_SAVE_WHITE_FRAME_H				(0x18)
#define FFC_SAVE_WHITE_FRAME_L				(0x1C)
#define FFC_SAVE_WHITE_EXPOSURE				(0x20)
#define FFC_SAVE_WHITE_TARGET				(0x24)
#define FFC_SAVE_WHITE_TEMP_H				(0x28)
#define FFC_SAVE_WHITE_TEMP_L				(0x2C)

#define FFC_SAVE_BLACK_GAIN					(0x30)
#define FFC_SAVE_WHITE_GAIN					(0x34)
#define FFC_SAVE_BLACK_BIT					(0x38)
#define FFC_SAVE_WHITE_BIT					(0x3c)


//----------------------------------------------------------------------------------
// オフセット/ゲイン配置
//----------------------------------------------------------------------------------

// ------------------------
// | オフセット        |   ゲイン         |
// ------------------------
// 31bit       18bit       0bit
#define OFFSET_SHIFT						(18)
#define OFFSET_MASK							(0x03fff)		// 14bit
#define GAIN_MASK							(0x3ffff)		// 18bit
#define OFFSET_GAIN_UNIT_SIZE				(4)

// Set
#define OFFSET_SET_DATA(x)					((x&OFFSET_MASK)<<OFFSET_SHIFT)
#define GAIN_SET_DATA(x)					(x&GAIN_MASK)

// Get
#define OFFSET_GET_DATA(x)					((x>>OFFSET_SHIFT)&OFFSET_MASK)
#define GAIN_GET_DATA(x)					(x&GAIN_MASK)


//----------------------------------------------------------------------------------
// Shading Line
//----------------------------------------------------------------------------------
#define FFC_WHITE_GAIN_MAX					(4.0)
#define FFC_WHITE_GAIN_MIN					(1.0)

#define FFC_WHITE_GAIN_UNIT					(10)


//----------------------------------------------------------------------------------
// DPC
//----------------------------------------------------------------------------------

// DPC Base Number
#define DPC_NUMBER0							(0)

// DPC Factory
#define DPC_FACTORY_NUMBER					(DPC_NUMBER0)

// User0のデフォルト
#define DPC_NUM_USER_DEFAULT				(0)

// DPC個数
#define DPC_NUM								(1)

// DPC Min/Max Admin
#define DPC_NUMBER_MIN						(0)
#define DPC_NUMBER_MAX						(0)

// DPC No Data
#define DPC_NO_DATA							(0xffffffff)

// DPC Admin/User
#define DPC_USER							(0)
#define DPC_ADMIN							(1)

// DPC Memory Type
#define DPC_MEMORY_EXT						(0)		// OS管理外
#define DPC_MEMORY_INT						(1)		// OS管理内(malloc)

// 欠陥補正不可座標格納サイズ
#define DPC_IMPOSSIBLE_GRID_SIZE			(64*1024)

// 欠陥補正不可座標無効データ
#define DPC_IMPOSSIBLE_GRID_INVALID_DATA	(0xffffffff)
#define DPC_IMPOSSIBLE_GRID_INVALID_DATA16	(0xffff)

// FFC回数
#define DPC_FFC_LOOP_DEFAULT				(2)
#define DPC_NE_FFC_LOOP_DEFAULT				(4)

// 調光回数
#define DPC_BRIGHTNESS_LOOP_DEFAULT			(2)

// DPC Grid
#define DPC_GRID_MODE_DELETE				(0)
#define DPC_GRID_MODE_ADD					(1)

// DPC調整モード
#define DPC_ADJUST_MODE_NOMAL				(1)
#define DPC_ADJUST_MODE_NE					(2)
#define DPC_ADJUST_MODE_NE_FFC_ENABLE		(3)
#define DPC_ADJUST_MODE_LINE				(4)
#define DPC_ADJUST_MODE_LINE_FFC_ENABLE		(5)
#define DPC_ADJUST_MODE_HS					(6)


//----------------------------------------------------------------------------------
// DPC補正情報
//----------------------------------------------------------------------------------

// Size
#define DPC_SAVE_INFO_UNIT_SIZE				(0x100)
#define DPC_SAVE_INFO_SIZE					(DPC_SAVE_INFO_UNIT_SIZE*DPC_FFC_LOOP_NUMBER)

// Black Address Map(FFC:11/DPC:12なので11*12*0x100が１個のサイズ)
#define DPC_SAVE_FRAME_H					(0x00)
#define DPC_SAVE_FRAME_L					(0x04)
#define DPC_SAVE_TEMP_H						(0x08)
#define DPC_SAVE_TEMP_L						(0x0c)
#define DPC_SAVE_EXPOSURE					(0x10)
#define DPC_SAVE_FFC						(0x14)
#define DPC_SAVE_SD0_H						(0x18)
#define DPC_SAVE_SD0_L						(0x1c)
#define DPC_SAVE_NONUNIFORM0_H				(0x20)
#define DPC_SAVE_NONUNIFORM0_L				(0x24)
#define DPC_SAVE_SD1_H						(0x28)
#define DPC_SAVE_SD1_L						(0x2c)
#define DPC_SAVE_NONUNIFORM1_H				(0x30)
#define DPC_SAVE_NONUNIFORM1_L				(0x34)
#define DPC_SAVE_SD2_H						(0x38)
#define DPC_SAVE_SD2_L						(0x3c)
#define DPC_SAVE_NONUNIFORM2_H				(0x40)
#define DPC_SAVE_NONUNIFORM2_L				(0x44)
#define DPC_SAVE_SD3_H						(0x48)
#define DPC_SAVE_SD3_L						(0x4c)
#define DPC_SAVE_CORRECTION_MODE			(0xfc)

#define DPC_SAVE_SD_INTERVAL				(0x10)


//----------------------------------------------------------------------------------
// Spectrum
//----------------------------------------------------------------------------------

// Bound Count
#define SPECTRUM_BAND_COUNT_MIN				(0)

// Band Informationデータ間隔
#define SPECTRUM_BAND_INFO_NUM				(1)

// Spectrum帯域情報バイナリサイズ
#define SPECTRUM_BIN_UNIT_SIZE				(sizeof(double)*SPECTRUM_BAND_INFO_NUM)

// データ切捨て（小数点第4位）
#define SPECTRUM_BAND_INFO_UNIT				(100000)

// データ四捨五入
#define SPECTRUM_BAND_INFO_ROUNDING			(0.000005)

// Band Gain Adjust
#define BAND_GAIN_TARGET_ADJUST_MAX_LOOP	(300)
#define NUM_BAND_GAIN_IMG_FOR_DETECT		(10)


//----------------------------------------------------------------------------------
// Upload/Download
//----------------------------------------------------------------------------------

// Program All Size
#define FIRM_DOWNLOAD_ALL_SIZE				(32*1024*1024)

// Spectrum
#define SPECTRUM_UPLOAD_SIZE				(1*1024*1024)
#define SPECTRUM_DOWNLOAD_SIZE				(1*1024*1024)

// Temp Buffer Size
#define FFC_TEMP_SIZE						(64)
#define SPECTRUM_TEMP_SIZE					(32)


//----------------------------------------------------------------------------------
// Timeout
//----------------------------------------------------------------------------------
#define GE_UPDATE_IF_FPGA_TIMEOUT			(180000)	// 180s
#define GE_PHY_FPGA_TIMEOUT					(60000)		// 60s
#define GE_UPDATE_SPECTRUM_WAVE_TIMEOUT		(60000)		// 60s


//----------------------------------------------------------------------------------
// Kermit Send Filename
//----------------------------------------------------------------------------------
#define FFC_SEND_FILE_NAME					"ffcDownload.csv"
#define FFC_SEND_FILE_COMPRESS_NAME			"ffcDownload.csv.gz"
#define FFC_SEND_FILE_BIN_NAME				"ffcDownload.bin"
#define FFC_SEND_FILE_BIN_COMPRESS_NAME		"ffcDownload.bin.gz"
#define DPC_SEND_FILE_NAME					"dpcDownload.txt"
#define DPC_SEND_FILE_COMPRESS_NAME			"dpcDownload.txt.gz"


//----------------------------------------------------------------------------------
// Update Mode
//----------------------------------------------------------------------------------
#define UPDATE_COMPRESS						(0)		// 圧縮
#define UPDATE_UNCOMPRESS					(1)		// 非圧縮


//----------------------------------------------------------------------------------
// Kermit Download Wait Time
//----------------------------------------------------------------------------------
#define DOWNLOAD_WAIT_TIME_MAX				(30)


//----------------------------------------------------------------------------------
// Compress
//----------------------------------------------------------------------------------

// Block Size
#define UNCOMPRESS_BLOCK_SIZE				(32768)
#define COMPRESS_BLOCK_SIZE					(32768)

// Compress
#define COMPRESS_GZIP_ID1					(0x1f)
#define COMPRESS_GZIP_ID2					(0x8b)

// 拡張子
#define UPDATE_FILE_NAME_CL_GZIP_EXTENSION	".gz"
#define UPDATE_FILE_NAME_CL_BIN_EXTENSION	".bin"
#define UPDATE_FILE_NAME_CL_RPD_EXTENSION	".rpd"
#define UPDATE_FILE_NAME_CL_HEX_EXTENSION	".hex"

#define UPDATE_FILE_NAME_CXP_SPI_EXTENSION	".spi"

// 追加メモリ
#define COMPRESS_DOWNLOAD_ADD_MEMORY		(32)


//----------------------------------------------------------------------------------
// Polafire Flash Write
//----------------------------------------------------------------------------------
#define FLASH_UPDATE_POLAFIRE_HEADER_GDN_ADRS	(0x00000000)
#define FLASH_UPDATE_POLAFIRE_HEADER_ADRS		(0x00000008)

#define FLASH_UPDATE_POLAFIRE_GDN_ADRS			(0x00000400)
#define FLASH_UPDATE_POLAFIRE_ADRS				(0x00400000)

#define FLASH_UPDATE_POLAFIRE_SIZE				(4*1024*1024)

#define FLASH_UPDATE_POLAFIRE_DOWNLOAD_SIZE		(3531552)
#define FLASH_UPDATE_POLAFIRE_COPY_SIZE			(0x3ff000)

// Update File Name
#define UPDATE_FILE_NAME_POLAFIRE_EXSAMPLE		""FIRM_NAME"_Interface_Fpga_Vxx.xxx"
#define UPDATE_FILE_NAME_POLAFIRE_COMPARE		""FIRM_NAME"_Interface_Fpga"

#define UPDATE_FILE_NAME_POLAFIRE_BOARD_EXSAMPLE	""IF_CXP_BOARD_NAME"_Fpga_Vxx.xxx"
#define UPDATE_FILE_NAME_POLAFIRE_BOARD_COMPARE		""IF_CXP_BOARD_NAME"_Fpga"


//----------------------------------------------------------------------------------
// Console
//----------------------------------------------------------------------------------
#define CONSOLE_MODE_OFF					(0)
#define CONSOLE_MODE_ON						(1)


//----------------------------------------------------------------------------------
// Diag
//----------------------------------------------------------------------------------
#define DIAG_MODE_AGING						(0)
#define DIAG_MODE_ONOFF						(1)


//----------------------------------------------------------------------------------
// Flash
//----------------------------------------------------------------------------------
// Mode
#define FLASH_WRITE_PROTECT					(1)
#define FLASH_WRITE_PROTECT_CANCEL			(0)

#define FLASH_WRITE_PROTECT_ADRS			(-1)


//----------------------------------------------------------------------------------
// Digital Offset / Gain
//----------------------------------------------------------------------------------
#define DIGITAL_GAIN_VALID_POINT			(100)


//----------------------------------------------------------------------------------
// GCP
//----------------------------------------------------------------------------------
#define GCP_MODE_BOARD						(1)
#define GCP_MODE_CAMERA						(2)
#define GCP_MODE_IPU						(3)
#define GCP_MODE_TEMP						(4)
#define GCP_MODE_CAMERA_STATUS				(5)
#define GCP_MODE_ACQUISITION				(6)
#define GCP_MODE_DIGITALIO					(7)
#define GCP_MODE_COUNTER					(8)
#define GCP_MODE_TIMER						(9)
#define GCP_MODE_ENCODER					(10)
#define GCP_MODE_SPECTRUM					(11)
#define GCP_MODE_VOLTAGE					(12)


//----------------------------------------------------------------------------------
// typedef
//----------------------------------------------------------------------------------

// printf
typedef int (* DEF_DEBUG_PRINT)(const char *, ...);
typedef int (* DEF_DEBUG_PRINT_FORCE)(const char *, ...);
typedef int (* DEF_DEBUG_PRINT_NO_CHECK)(const char *, ...);
typedef int (* DEF_DEBUG_INPUT_POLL)(char *);
typedef char (* DEF_DEBUG_INPUT)(void);
typedef void (* DEF_DEBUG_OUTPUT)(char);


// Command
typedef int (* CMDFUNC)(void *);


// Command Table
typedef struct
{
	char     *name;          // name
	CMDFUNC  func;           // entry
	CMDFUNC  funcHelp;       // Help entry
	int      optArg;		 // option
	//char     *operation;   // Operation
} CMD_TBL;


// Command Only
typedef struct
{
	char name [CONSOLE_BUFF_SIZE];	// name
	int optArg;						// Loop or Display Mode
} CMD_STRING_TBL;


// Save Parameter List
typedef struct {
	unsigned int regAdrs;
	unsigned int data;
	int mode;
} SAVE_PARAM_MODE;


// Save Parameter List
typedef struct {
	unsigned int regAdrs;
	unsigned int data;
} SAVE_PARAM;


// Board Parameter List
typedef struct {
	int		cmdNo;		// Number
	char	*name;      // Name
	int		dispMode;	// Display Mode
	int		length;		// Data Length
} BOARD_PARAM;


// 画素欠陥検出閾値
typedef struct _THRESHOLDCHARA
{
	double chara1;	// 閾値1
	double chara2;	// 閾値2
} THRESHOLDCHARA;


// 画素欠陥情報
typedef struct _DEFECTIONINFO
{
	unsigned short x;							// 欠陥X座標
	unsigned short y;							// 欠陥Y座標
	//unsigned char filter_index[FILTER_USE_MAX];// 補正に使用するフィルタインデックス
	//unsigned short divsor;					// フィルタ除数
	//unsigned short upCount;					// 上方向までの距離(y方向補正のみ使用)
	//unsigned short downCount;					// 下方向までの距離(y方向補正のみ使用)
} DEFECTIONINFO;


// FFC構造体
typedef struct _FFC_PARAM
{
	int ffcNo;
	//int mode;
	double frameRate;
	unsigned int exposure;
	int bit;
	unsigned int target;
	double sensorTemp;
	int memType;
	int userMode;
} FFC_PARAM;


// DPC構造体
typedef struct _DPC_PARAM
{
	int dpcNo;
	int ffcNo;
	//int mode;
	double frameRate;
	unsigned int exposure;
	int bit;
	double sensorTemp;
	int memType;
	int userMode;
	double sd;
	double nonuniformity;
	int detailMode;
} DPC_PARAM;


// FFC調整情報構造体
typedef struct _FFCINFO
{
	double frameRate;
	unsigned int expTime;
	unsigned int target;
	double sensorTemp;
	float digitalGain;
	int bit;
	int mark;
} FFCINFO;


// FFC情報構造体
typedef struct FFC_OG_INFO{
   double data;
   int x;
   int y;
} FFC_OG_INFO;


// DPC調整情報構造体
struct DPC_PRIVATE
{
	double frameRate;
	unsigned int expTime;
	double sensorTemp;
	double sd[DPC_BRIGHTNESS_LOOP_NUMBER];
	double ununiformity[DPC_BRIGHTNESS_LOOP_NUMBER];
	int ffcNo;
};


// DPC情報
typedef struct _DPCINFO
{
	int dpcCorMode;
	struct DPC_PRIVATE dpcPrivate[FFC_NUM_USER];
} DPCINFO;


// Grid XY
typedef struct _GRID_XY
{
	unsigned short x;
	unsigned short y;
} GRID_XY;


// CXPパケット情報
typedef struct CxpPacketSt {
	unsigned int start;
	unsigned int cmdIndication;
	unsigned int tag;
	unsigned int size;
	unsigned int cmd;
	unsigned int adrs;
	unsigned int data;
	unsigned int *pData;
	unsigned int crc;
	unsigned int end;

	int status;
	int ackSize;
	int dataIndex;
	unsigned int sendcmdIndication;
} CXP_PACKET_ST;


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern char *gCmdArg[];				// Command Argument
extern int gInterFaceID;			// Interface ID
extern char gLogMsgBuff[];			// Log Msg Buffer

// input/output関数宣言
extern DEF_DEBUG_PRINT 				DEBUG_PRINT;
extern DEF_DEBUG_PRINT_FORCE 		DEBUG_PRINT_FORCE;
extern DEF_DEBUG_PRINT_NO_CHECK 	DEBUG_PRINT_NO_CHECK;
extern DEF_DEBUG_INPUT_POLL 		DEBUG_INPUT_POLL;
extern DEF_DEBUG_INPUT				DEBUG_INPUT;
extern DEF_DEBUG_OUTPUT 			DEBUG_OUTPUT;


//----------------------------------------------------------------------------------
// Function Define
//----------------------------------------------------------------------------------

// acqusitionControl.c
int acquisitionInit (void);
int acquisitionInit2 (void);
int acquisitionSetMode (int mode);
int acquisitionGetMode (int *pMode);
int acquisitionStart (void);
int acquisitionStop (void);
int acquisitionAbort (void);
int acquisitionGetStartFlag (int *pMode);
int acquisitionSetFrameCount (unsigned int count);
int acquisitionGetFrameCount (unsigned int *pCount);
int acquisitionSetFrameRate (double frameRate);
int irSetFrameRate (double frameRate);
int irSetFrameRateDrrs (double frameRate);
int acquisitionGetFrameRate (double *pFrameRate);
int irGetFrameRate (double *pFrameRate);
int irGetFrameRateMain (double *pFrameRate);
int irGetFrameRateShutterNormal (double *pFrameRate);
int irGetFrameRateShutterNormalMain (double *pFrameRate);
int acquisitionGetStatus (int mode, int *pStatus);
int acquisitionSetSelect (int select);
int acquisitionGetSelect (int *pMode);
int lfAcquisitionTrgSelectCheck (int number);
int acquisitionSetTrgMode (int mode);
int acquisitionGetTrgMode (int *pMode);
int acquisitionSetSoftTrg (void);
int acquisitionSetTrgSource (int mode);
int acquisitionGetTrgSource (int *pMode);
int lfTrgSourceCheck (int number);
int acquisitionSetTrgActivation (int active);
int acquisitionGetTrgActivation (int *pActive);
int acquisitionSetTrgDelay (unsigned int delay);
int acquisitionGetTrgDelay (unsigned int *pDelay);
int acquisitionGetInvalidedTrg (int *pMode);
int acquisitionSetExposureMode (int mode);
int acquisitionGetExposureMode (int *pMode);
int acquisitionSetExposure (unsigned int expTime);
int acquisitionSetExposureSimple(unsigned int expTime);
int irSetExposure (unsigned int expTime);
int irSetExposureDrrs (unsigned int expTime);
int irSetExposureSimple (unsigned int expTime);
int irSetExposureSimpleShutterNormal (unsigned int expTime);
int acquisitionGetExposure (unsigned int *pExpTime);
int irGetExposure (unsigned int *pExpTime);
int irGetExposureDrrs (unsigned int *pExpTime);
int acquisitionGetTrgInvalidCount (unsigned int *pCount);
int acquisitionSetTrgReserve (int mode);
int acquisitionGetTrgReserve (int *pMode);
int acquisitionGetTrgReserveMain (int *pMode);
int acquisitionGetFrameTrgMode (int *pMode);
int acquisitionReset (void);
int exposureMin (int *pExp);
int exposureMax (int *pExp);
int rateMin (double *pRate);
int rateMax (double *pRate);
int rateGetSensorMax (double *pRate);
int rateGetGigEMax (double *pRate);
int rateGetCxpMax (double *pRate);
int rateSetMax (double rate);
int rateSetClMax (double rate);
int rateSetGigEMax (double rate);
int sensoreGetReadOut(int *pTime);
int acquisitionGetTrgSourceEncoder (int *pStatus);
int irvGetHIntervalClock (unsigned int *pHinterval);
int acquisitionSetRateMode (int mode);
int acquisitionGetRateMode (int *pMode);
int acquisitionGetTrgCountHigh (int port, unsigned int *pCount);
int acquisitionGetTrgCountLow (int port, unsigned int *pCount);
int acquisitionRestTrgSoftCount (void);
int acquisitionSetTrgSoftCount (void);
int acquisitionRestLinkTrgCount (void);

// aoi.c
int aoiInitialize (void);
int aoiSetUpdate (int mode);
int aoiGetUpdate (int *pMode);
int aoiSetBitWidth (int bit);
int irSetBitWidth (int bit);
int aoiGetBitWidth (int *pBit);
int irGetBitWidth (int *pBit);
int cameraGetBitWidthGigE (int *pBit);
int aoiSetPad (int x, int y);
int aoiGetPad (int *pX, int *pY);
int aoiSetPatternMain (int index);
int aoiSetPattern (int index);
int aoiGetPattern (int *pIndex);
int aoiSetTpInc (int horizon, int virtical);
int aoiGetTpInc (int *pHorizon, int *pVirtical);
int aoiSetPatternPosition (int mode);
int aoiGetPatternPosition (int *pMode);
int aoiSetXflip (int mode);
int aoiGetXflip (int *pMode);
int aoiGetShift (int bit, int *pShift);
int aoiGetShift2 (int bit, int *pShift);
int aoiGetFlipX (int x, int *pFlipx);
int aoiGetFlipX2 (int x, int *pFlipx, int flipMode);
int aoiBinninCheckParam (int binning);
int aoiBinningRegToSize (unsigned int data, int *pBinning);
int aoiBinningSizeToReg (int binning, unsigned int *pData);
int aoiSetBinningDivide (void);
int aoiSetBinningX(int mode);
int aoiGetBinningX(int *pMode);
int aoiSetBinningY(int mode);
int aoiGetBinningY(int *pMode);
int aoiSetBinningMode(int mode);
int aoiGetBinningMode(int *pMode);
int aoiSetBinningOffsetBase(int offset);
int aoiGetBinningOffsetBase(int *pOffset);

// autoBright.c
int autoBrightInitialize (void);
int autoBrightMain (void);
int autoExposureControl(int average, int target);
int autoGainControl(int average, int target);
int autoExposurePid (int bright, int target, double *pPID);
int autoGainPid (int bright, int target, double *pPID);
int autoBrightInformation (void);
int autoBrightGetFunctionValid (int *pValid);
int autoBrightCheckFunctionValid (void);
int autoBrightSetExposureMode (int mode);
int autoBrightGetExposureMode (int *pMode);
int autoBrightSetExposureStatus (int mode);
int autoBrightGetExposureStatus (int *pMode);
int autoBrightSetExposureMax (int exp);
int autoBrightGetExposureMax (int *pExp);
int autoBrightCheckExposureMinMax (int exp);
int autoBrightSetGainMode (int mode);
int autoBrightGetGainMode (int *pMode);
int autoBrightSetGainStatus (int mode);
int autoBrightGetGainStatus (int *pMode);
int autoBrightGetGainMin (double *pData);
int autoBrightSetGainMax (double data);
int autoBrightGetGainMax (double *pData);
int autoBrightSetDetectArea (int mode);
int autoBrightGetDetectArea (int *pMode);
int autoBrightSetTarget (int target);
int autoBrightGetTarget (int *pTarget);
int autoBrightGetAverage (int *pData);
int autoBrightSetWidthSize (int size);
int autoBrightGetWidthSize (int *pSize);
int autoBrightSetHeightSize (int size);
int autoBrightGetHeightSize (int *pSize);
int autoBrightSetWidthOffset (int offset);
int autoBrightGetWidthOffset (int *pOffset);
int autoBrightSetHeightOffset (int offset);
int autoBrightGetHeightOffset (int *pOffset);
int autoBrightSetTargetArea (int area);
int autoBrightGetTargetArea (int *area);

// asm.c
void cpu_wfi (void);
void enable_global_irq (void);
void disable_global_irq (void);
void irq_restore_Mie (uintptr_t old_mstatus);
uintptr_t irq_save_disable_Mie (void);

// board.c
int cameraInformationInitialize (void);
int getSpectrumType (int *pType);
int getDipsw (unsigned int *pDipsw);
int boardReset (void);
int setBoardParam (char *pParam, int offset, int size);
int getBoardParam (char *pParam, int offset, int size);
int setBoardId (char *pId);
int getBoardId (char *pId);
int setSensorId (char *pId);
int getSensorId (char *pId);
int getInterfaceId (int *pId);
int getGpioType (int *pId);
int setUserId (char *pId);
int getUserId (char *pId);
int setVendor (char *pName);
int getVendor (char *pName);
int setManufacture (char *pName);
int getManufacture (char *pName);
int setModel (char *pName);
int getModel (char *pName);
int xmlFileLoadMemory (void);
int cameraPowerSupply (unsigned int *pData);

// buffer.c
int buffInitialize (void);

// camera.c
int WidthMin (void);
int HeightMin (void);
int WidthMax (void);
int HeightMax (void);
int sensorWidth (void);
int sensorHeight (void);
int cameraGetType (int *pType);
int cacheFlush (void);
int cacheFlushRange (unsigned int adrs, unsigned int size);
int cacheInvalidateRange (unsigned int adrs, unsigned int size);

// cameraParamCustom.c
int cameraParamCustom (void);

// cameraSave.c
int cameraParamInitialize (void);
int cameraParamInitialize2 (void);
int cameraParamSetDefault (int userNum);
int cameraParamSetSpectrumDefault (int userNum);
int cameraParamUserAllWrite (int userNum);
int cameraParamUserAllRead (int userNum);
int cameraParamSpectrumAllWrite (int userNum);
int cameraParamSpectrumAllRead (int userNum);
int cameraParamUserWriteRomMulti (int userNum, unsigned int saveAdrs, unsigned char *pData, int size);
int cameraParamSpectrumWriteRomMulti (int userNum, unsigned int saveAdrs, unsigned char *pData, int size);
int cameraParamCommonAllWrite (void);
int cameraParamCommonAllRead (void);
int cameraParamCommonWriteRom (unsigned int saveAdrs, unsigned int data);
int cameraParamCommonReadRom (unsigned int saveAdrs, unsigned int *pData);
int cameraParamCommonWriteRomMulti (unsigned int saveAdrs, unsigned char *pData, int size);
int cameraParamUserWriteMem (int userNum, unsigned int saveAdrs, unsigned int adrs, unsigned int data);
int cameraParamUserReadMem (int userNum, unsigned int saveAdrs, unsigned int *pAdrs, unsigned int *pData);
int cameraParamCommonWriteMem (unsigned int saveAdrs, unsigned int data);
int cameraParamCommonReadMem (unsigned int saveAdrs, unsigned int *pData);
int cameraParamWriteRegister (int userNum, unsigned int offset, unsigned int size);
int cameraParamWriteRegisterOffsetAdrs (int userNum, unsigned int offset, unsigned int size, unsigned int offsetAdrs);
int cameraParamSpectrumWriteRegister (int userNum, unsigned int offset, unsigned int size);
int cameraParamUserMarkClear (int userNum);
int cameraParamSpectrumMarkClear (int userNum);
int cameraParamCommonMarkClear (void);
int cameraParamGetUserAdrs (int userNum, unsigned int **pAdrs);
int cameraParamGetCommonAdrs (unsigned int **pAdrs);
int cameraParamSaveRegister (int userNum);
int cameraParamSaveSpectrumRegister (int userNum);
int cameraParamRestoreRegister (int userNum);
int cameraParamRestoreSpectrumRegister (int userNum);

// cameraStatus.c
int cameraStatusInitialize (void);
int cameraSetStatus (int num, int status);
int cameraGetStatus (int num, int *pStatus);
int cameraSetStatusAll (void);
int cameraStatusShow (void);

// clMain.c
int clMain (void);

// cmd.c
int cmd (void);
int cmdMain (CMD_TBL *cmdTbl, int mode);
int cmdNoGet (CMD_TBL *cmdTbl, int mode);
int cmdLoopCount (char *pBuffer);
void cmdCharGet (char *buff);
void cmdDispMenu (CMD_TBL *cmdTbl);
int cmdCheckArg (char *str);
int cmdTabSearch (char *pBuffer, int *pIndex);	
int cmdHistortKey (int upDown, char *pBuffer, int *pInputCount, int *pCurrentIndex);
int cmdSortInitialize (void);
int lfcmdSort (const void *a, const void *b);
int cmdClearLine (void);
int cmdBackspaceLine (void);
int cmdGetArgment (char *str, char *cmd, char **arg);
int strmatch (const char *ptn, const char *str);
int cmdHistoryShow (void *str);
int cmdHistoryShowHelp (void *str);
int cmdHistoryClear (void *str);
int cmdHistoryClearHelp (void *str);

//cxpI2c.c
int cxpI2cRegWrite (unsigned int adrs, unsigned char data);
int cxpI2cRegRead (unsigned int adrs, unsigned char *pData);

//cxpMain.c
int cxpMain (void);
int cxpUserInit (void);
int cxpLedControl (int linkState);
int cxpLedLinkState (unsigned int *pLinkStatus);
int cxpLedConnectionDetection (void);
int cxpLedConnectioned (void);
int cxpGetLinkCount (unsigned int *pCount);
int cxpSetRecvIntMode (int mode);

//cxpProcs.c
int cxpInitialize (void);
int cxpInitialize2 (void);
int cxpInitializeImageParam (void);
int cxpProcs (int port);
int cxpSetUser (int port, CXP_PACKET_ST *pCxpSt);
int cxpGetUser (int port, CXP_PACKET_ST *pCxpSt);
int cxpSetAckPacket (int port, CXP_PACKET_ST *pCxpSt);
int cxpCalculateCrc32 (unsigned int *pCrc, unsigned int *pData, unsigned int count);
int cxpGetCmdPacket (int port, unsigned int *pData);
int cxpWriteFifo32 (int port, unsigned int data, unsigned int mark);
int cxpSendFifoStart (int port, int mode);
int cxpSendFifoStartI2c (int port);
int cxpSendFifoStartParallel (int port);
int cxpSendFifoEnable (int mode);
int cxpSendFifoSize (int port, unsigned int data);
int cxpSendFifoClear (int port, int mode);
int cxpSendFifoClearI2c (int port);
int cxpSendFifoClearParallel (int port);
int cxpSetCmdMode (int mode);
int cxpGetCmdMode (int *pMode);
int cxpGetFifoCount (unsigned int *pCount);
int cxpSendTestPacketAckCmd (unsigned int size);
int cxpSendBuffer (int port, unsigned int data);
int cxpGetSendCurrentAdrs (int port, unsigned int *pAdrs);
int cxpGetRecvCurrentAdrs (int port, unsigned int *pAdrs);
int cxpRecvBuffer (int port, unsigned int data);
int cxpGetSendCount (unsigned int *pData);
int cxpGetFifoSizeCount (int port, unsigned int *pCount);
int cxpCheckFpgaMemAdrs (unsigned int *pAdrs, unsigned int size);
int cxpSetFifoAdrs (unsigned int adrs);
int cxpSetDmaAdrs (unsigned int adrs);
int cxpSetFifoReset (void);
int cxpSetConectionConfig (unsigned int data);
int cxpGetConectionConfig (unsigned int *pData);
int cxpSetPixelFormat (int port, int bit);
int cxpSetDSizeL (int port, int wSize);
int cxpSetBit (int port, int bit);
int cxpSetWidth (int port, int size);
int cxpSetOffsetX (int port, int offset);
int cxpSetHeight (int port, int size);
int cxpSetOffsetY (int port, int offset);
int cxpSetHeightParam (int port);
int cxpDownloadBuffer (unsigned int adrs, unsigned char *pData, unsigned int size);
int cxpUploadBuffer (unsigned int adrs, unsigned char *pData, unsigned int size);
int cxpUpdateIfFpga (unsigned int flashAdrs, unsigned char *pData, unsigned int size);
int cxpGetCableConnection (int port, int *pMode);
int cxpGetTestPacketRxCount (int port, unsigned long long *pData);
int cxpGetTestPacketTxCount (int port, unsigned long long *pData);
int cxpGetTestPacketErrCount (int port, unsigned int *pData);
int cxpSetPowerCtrl (int port, int autoMode, int mode);
int cxpGetPowerCtrl (int port, int *pAutoMode, int *pMode);
int cxpGetPowerExtDetect (int *pDetect);
int cxpGetPowerIfDetect (int *pDetect);
int cxpSetPort (int port);
int cxpGetPort (int *pPort);
int cxpSetRateData (unsigned int rate);
int cxpGetRateData (unsigned int *pRate);
int cxpSetPortDual (int port, int mode);
int cxpGetRegToSpeed (unsigned int regData, double *pCxpRateBps);
int cxpGetDataToRegData (unsigned int data, unsigned int *pRegData);
int cxpGetRegToConnection (unsigned int regData, unsigned int *pCxpConnection);
int cxpGetPortDual (int port, int *pMode);
int cxpSetStreamMode (int port, int mode);
int cxpGetStreamMode (int port, int *pMode);
int cxpSetLed (int port, int mode);
int cxpGetLed (int port, int *pMode);
int cxpGetStreamId (int port, unsigned int *pId);
int cxpSetStreamId (int port, unsigned int id);
int cxpSetRateReg (unsigned int data);
int cxpSetConnectionConfig (int port, unsigned int configData);
int cxpGetReadFifoStatus (int port, unsigned int *pStatus);

// compress.c
int gzipUncomp (unsigned char *pUnCompress, int *uncomprLen, unsigned char *pCompress, int comprLen);
int gzipComp (unsigned char *pUnCompress, int uncomprLen, unsigned char *pCompress, int *comprLen);

// console.c
int consoleDefaultInitialize (void);
int consoleInitialize (void);
int consoleModeInitialize (void);
int consoleSetMode (int mode);
int consoleGetMode (int *pMode);
int consoleSelectMain (int port);
int printfDebugCl (const char *fmt, ...);
int printfDebugGigE (const char *fmt, ...);
int printfDebugDouble (const char *fmt, ...);
int printfDebugGigE2 (const char *fmt, ...);
int printfDebugClForce (const char *fmt, ...);
int printfDebugGigEForce (const char *fmt, ...);
int printfDebugDoubleForce (const char *fmt, ...);
int printfDebugGigEForce2 (const char *fmt, ...);
int printfDebugPort (int port, char *pBuffer, int len);
int printfDebugPort2 (int port, char *pBuffer, int len);
char input (void);
char inputCl (void);
char inputGigE (void);
int inputPoll (char *pData);
int inputPollCl (char *pData);
int inputPollGigE (char *pData);
int inputPollDouble (char *pData);
void output (char data);
int _kbhit (char *pData);
void outputCl (char data);
void outputGigE (char data);
void outbyte (char data);

// correctionMain.c
int correctionMain (void);

// counterControl.c
int counterInitialize (void);
int counterSetSelect (int select);
int counterGetSelect (int *pSelect);
int counterSetEventSource (int source);
int counterGetEventSource (int *pSource);
int lfCounterEventSourceCheck (int number);
int counterSetEventActive (int mode);
int counterGetEventActive (int *pMode);
int counterSetResetSource (int source);
int counterGetResetSource (int *pSource);
int lfCounterResetSourceCheck (int number);
int counterSetResetActive (int mode);
int counterGetResetActive (int *pMode);
int counterReset (void);
int counterSetValue (int count);
int counterGetValue (int *pCount);
int counterSetResetValue (int count);
int counterGetResetValue (int *pCount);
int counterSetDuration (int count);
int counterGetDuration (int *pCount);
int counterGetStatus (unsigned int *pStatus);
int counterSetTrgSource (int source);
int counterGetTrgSource (int *pSource);
int lfCounterTrgSourceCheck (int number);
int counterSetTrgActive (int mode);
int counterGetTrgActive (int *pMode);
int counterSetTrgMode (int lineSrc);
int counterGetTrgMode (int *pLineSrc);
int counterGetTrgInputCount (int *pCount);
int counterGetImageOutputCount (int *pCount);

// digitalBnadOffsetGain.c
int digitalBandOffsetGainInitialize (void);
int digitalSetBandGain (int line, float gain);
int digitalGetBandGain (int line, float *pGain);
int digitalSetBandGainX (int line, float gain);
int digitalGetBandGainX (int line, float *pGain);
int digitalSetBandOffset (int line, int offset);
int digitalGetBandOffset (int line, int *pOffset);
int digitalSetBandOffset1 (int line, int offset);
int digitalGetBandOffset1 (int line, int *pOffset);
int digitalInitBandGain (void);
int digitalShowBandGain (void);
int digitalShowBandGainX (void);
int digitalInitBandOffset (void);
int digitalShowBandOffset (void);
int digitalInitBandOffset1 (void);
int digitalShowBandOffset1 (void);
int digitalBandOffsetSave (void);
int digitalBandOffsetRestore (void);
int digitalBandOffsetInitialize (void);
int digitalBandGainAdjust (int target, int range, int mode);
int digitalGainTotal (int band, float *pBandGain, float *pGain, float *pFfcGain);

// digitalIoControl.c
int digitalIoInitialize (void);
int digitalIoSetLineSelect (int select);
int digitalIoGetLineSelect (int *pSelect);
int lfLineSelectorCheck (int number);
int digitalIoSetMode (int mode);
int digitalIoGetMode (int *pMode);
int digitalIoSetInverter (int mode);
int digitalIoGetInverter (int *pMode);
int digitalIoGetLineStatus (int *pStatus);
int digitalIoGetLineAllStatus (unsigned int *pStatus);
int digitalIoSetLineSource (int source);
int digitalIoGetLineSource (int *pSource);
int lfLineSourceCheck (int number);
int digitalIoSetLineFormat (int format);
int digitalIoGetLineFormat (int *pFormat);
int digitalIoSetUserSelect (int select);
int lfLineformatCheck (int number);
int digitalIoGetUserSelect (int *pSelect);
int digitalIoSetUserValue (int mode);
int digitalIoGetUserValue (int *pMode);
int digitalIoSetAllValue (unsigned int value);
int digitalIoGetAllValue (unsigned int *pValue);
int digitalIoSetUserMask (unsigned int mask);
int digitalIoGetUserMask (unsigned int *pMask);
int digitalIoSetUserOutput (int select, int value);
int digitalIoGetUserOutput (int select, int *pValue);

// digitalOffsetGain.c
int dogInitialize (void);
int dogSetUpdate (int mode);
int dogGetUpdate (int *pMode);
int dogSetOffsetGain (int offset1, int offset2, float gain);
int dogGetOffsetGain (int *pOffset1, int *pOffset2, float *pGain);
int dogGetOffsetMaxMin (int *pOffset1Min, int *pOffset1Max, int *pOffset2Min, int *pOffset2Max);
int dogGetOffset1MaxMin (int *pOffset1Min, int *pOffset1Max);
int dogGetOffset2MaxMin (int *pOffset2Min, int *pOffset2Max);
int digitalSetGain (float gain);
int digitalGetGain (float *pGain);
int digitalSetGainX (float gain);
int digitalGetGainX (float *pGain);
int digitalSetGainReg (unsigned int gain);
int digitalGetGainReg (unsigned int *pGain);
int digitalSetOffset (int offset);
int digitalGetOffset (int *pOffset);
int digitalSetOffset1 (int offset);
int digitalGetOffset1 (int *pOffset);
int opticalBlackSetInvert (int mode);

// dma.c
int dmaInitialize (void);
int dmaStart (unsigned char *pBuffer);
int dmaWait (void);

// dpc.c
int dpcInitialize (unsigned int flashAdrs, unsigned int memAdrs, int mode);
int dpcInitializeAdmin (unsigned int flashAdrs, unsigned int memAdrs, int mode);
int dpcInitializeMain (unsigned int flashAdrs, unsigned int memAdrs, int mode, int userMode);
int dpcRegInit (unsigned int adrs, unsigned int size, int mode);
int dpcSetMode (int mode);
int dpcGetMode (int *pMode);
int dpcSetEnableMode (int mode);
int dpcGetEnableMode (int *pMode);
int dpcSetAdjustParamZero ();
int dpcSetAdjustParam ();
int dpcSetLoadNum (int dpcNum);
int dpcGetLoadNum (int *pDpcNum);
int dpcLoad (int dpcNum);
int dpcLoadAdmin (int dpcNum);
int dpcLoadMain (int dpcNum, int userMode);
int dpcSave (int dpcNum);
int dpcSaveAdmin (int dpcNum);
int dpcSaveMain (int dpcNum, int userMode);
int dpcGetParam (unsigned int *pMemAdrs, unsigned int *pSize, unsigned int *pDmaWait, int *pMode);
int dpcGetMemAdrs (int dpcNum, unsigned int *pAdrs);
int dpcGetFlashAdrs (int dpcNum, unsigned int *pAdrs);
int dpcGetFlashAdrsAdmin (int dpcNum, unsigned int *pAdrs);
int dpcGetFlashImpossibleAdrs (int dpcNum, unsigned int *pAdrs);
int dpcGetFlashImpossibleAdrsAdmin (int dpcNum, unsigned int *pAdrs);
int dpcGetNumFromFlash (unsigned int flashAdrs, int *pDpc);
int dpcGetNumFromFlashAdmin (unsigned int flashAdrs, int *pDpc);
int dpcDataUpload (void);
int lfDpcUploadBintoAscii (void *pAscii, void *pBinary, unsigned int size);
int dpcDataDownload (int mode);
int lfDpcDownloadBintoAscii (void *pAscii, void *pBinary, unsigned int *pSize, unsigned int mallocSize);
int dpcSetMemory (int dpcNum, DEFECTIONINFO *pData, int index);
int dpcEndSearch (unsigned int adrs, int *pIndex);
int dpcEndSearch2 (unsigned int adrs, int *pIndex);
int dpcToFlash (unsigned int flashAdrs, unsigned int memAdrs);
int dpcToFlashAdmin (unsigned int flashAdrs, unsigned int memAdrs);
int dpcToFlashMain (unsigned int flashAdrs, unsigned int memAdrs);
int dpcToMemory (unsigned int flashAdrs, unsigned int memAdrs);
int dpcToMemoryAdmin (unsigned int flashAdrs, unsigned int memAdrs);
int dpcToMemoryMain (unsigned int flashAdrs, unsigned int memAdrs);
int dpcSetMarkFlash (int dpcNum, unsigned int mark);
int dpcSetMarkFlashAdmin (int dpcNum, unsigned int mark);
int dpcGetMarkFlash (int dpcNum, unsigned int *pMark);
int dpcGetMarkFlashAdmin (int dpcNum, unsigned int *pMark);
int dpcCopyAdminToUser (int srcDpcNum, int desDpcNum);
int dpcCopyUserToAdmin (int srcDpcNum, int desDpcNum);
int dpcDefault (void);
int dpcGetPixelMaxNum (unsigned int *pNum);
int dpcGetGridMem (int dpcNum, GRID_XY *pGrid, int *pNum);
int dpcGetDefectionMaxCount (int *pCount);
int dpcSetDefectionCount (int count);
int dpcGetDefectionCount (int *pCount);
int dpcSetDefectionCountVersion2 (int count);
int dpcGetDefectionCountVersion2 (int *pCount);
int dpcSetMapInfoMode (int mode);
int dpcGetMapInfoMode (int *pMode);
int dpcSetMapFlashMode (int mode);
int dpcGetMapFlashMode (int *pMode);
int dpcGetGridLine (unsigned int memAdrs, unsigned char *dpcPtr, int checkX, int *pCount);
int dpcGetGrid (unsigned int memAdrs, int index, int *pX, int *pY);
int dpcAddGrid (int x, int y);
int dpcDeleteGrid (int x, int y);
int dpcGridMain (int x, int y, int mode);
int dpcAddGridLine (int x);
int dpcDeleteGridLine (int x);
int dpcGridLineMain (int x, int mode);
int dpcClearGrid (int dpcNum);
int dpcMapInfoFlash (int dpcNum, int userMode);
int dpcMapInfoShow (unsigned char *pAdrs);
int dpcGetMapInfo (unsigned char *pAdrs, int *pCount);
int dpcGetMapInfoIndex2 (int index, int *pX, int *pY);
int dpcImpossibleGridInfo (int dpcNum, int userMode);
int dpcSetAdjustMode (int mode);
int dpcGetAdjustMode (int *pMode);
int dpcSetAdjustRate (int index, float rate);
int dpcGetAdjustRate (int index, float *pRate);
int dpcSetAdjustExposure (int index, int exp);
int dpcGetAdjustExposure (int index, int *pExp);
int dpcSetAdjustFfcNumber (int index, int ffcNumber);
int dpcGetAdjustFfcNumber (int index, int *pFfcNumber);
int dpcSetAdjustTemp (int index, float temp);
int dpcGetAdjustTemp (int index, float *pTemp);
int dpcSetAdjustSd (int index, int bIndex, float sd);
int dpcGetAdjustSd (int index, int bIndex, float *pSd);
int dpcSetAdjustNonUniform (int index, int bIndex, float unUniform);
int dpcGetAdjustNonUniform (int index, int bIndex, float *pUnuniform);
int dpcAdjustUpdate (void);
int dpcSetMemoryNormal (void *srcAdrs, unsigned int offset, unsigned int size);
int dpcGetMemoryNormal (void *desAdrs, unsigned int offset, unsigned int size);
int dpcGridCheck (int x, int y, int *pDetect);
int dpcGridCheckAll (int x, int y, int *pDetect);
int dpcDataXFlip (int mode, unsigned int dpcMemAdrs);
void dpcSetXFlipFlag (int mode);
int dpcGetXFlipFlag (void);
int dpcToMemoryHalf (void);
int dpcCopyBuffToExtMem (unsigned long memAdrs, unsigned int *pBuffer);
int dpcCopyExtMemToBuff (unsigned long memAdrs, unsigned int *pBuffer);
int dpcGetDivAdrs (int x, int y, int *pX, unsigned int *pAdrs);
int dpcSetMarginGridData (void);
int hideGridCalc (int x, int y, int *pLeft, int *pRight);

// dpcAdjust.c
int dpcClearBatchCount (void);
int dpcMainBatch1 (DPC_PARAM dpcParam);
int dpcMainBatch2 (DPC_PARAM dpcParam);
int dpcMainBatch3 (DPC_PARAM dpcParam);
int dpcGetBatchCount (void);
int dpcGetBatchCountGlobal (int *pCount);
int dpcSetBatchCount (int count);
int dpcGetCount (int *pCount);
int dpcDecrementCount (void);
int dpcSetInfo (DPC_PARAM *pDpcParam, int ffcLoop);
int dpcSetInfoDetail (DPC_PARAM *pDpcParam, int ffcLoop, int index);
int dpcCheckParam (DPC_PARAM dpcParam);
int dpcSetParam (DPC_PARAM *pDpcParam);
int dpcCorrectionMain (DPC_PARAM dpcParam, int loopTotal);
int lfDefectMain (DPC_PARAM *pDpcParam, int loopTotal, int loopBri);
int lfDefect (int loop, double sd_thre, double nonuniformity_thre);
int lfDpcPxelCorrection (int *pIndex, int loop);
int dpcAdjustInfo (int dpcNum, int userMode);
int dpcAdjustInfoSave (int dpcNum, int userMode, DPCINFO dpcInfo);
int dpcAdjustInfoRead (int dpcNum, int userMode, DPCINFO *pDpcInfo);
int dpcImpossibleGridSave (int dpcNum, int userMode, unsigned short *pBuffer);
int dpcImpossibleGridRead (int dpcNum, int userMode, unsigned short *pBuffer);
int dpcSetError (int status);
int dpcAdjustThreshold (double *pData, double threshold);
int dpcMapAdd (int dpcCountNew);

// encoderControl.c
int encoderInitialize (void);
int encoderSetPhaseATrgSource (int source);
int encoderGetPhaseATrgSource (int *pSource);
int lfEncoderPhaseATrgSourceCheck (int number);
int encoderSetPhaseBTrgSource (int source);
int encoderGetPhaseBTrgSource (int *pSource);
int lfEncoderPhaseBTrgSourceCheck (int number);
int encoderSetMode (int mode);
int encoderGetMode (int *pMode);
int encoderSetDivider (int divider);
int encoderGetDivider (int *pDdivider);
int encoderSetOutputMode (int mode);
int encoderGetOutputMode (int *pMode);
int encoderGetStatus (int *pStatus);
int encoderSetTimeout (int timeout);
int encoderGetTimeout (int *pTimeout);
int encoderSetResetTrgSource (int source);
int encoderGetResetTrgSource (int *pSource);
int lfEncoderResetTrgASourceCheck (int number);
int encoderSetResetActivation (int activation);
int encoderGetResetActivation (int *pActivation);
int encoderSetReset (void);
int encoderSetValue (long long value);
int encoderGetValue (long long *pValue);
int encoderSetValueAtReset (long long value);
int encoderGetValueAtReset (long long *pValue);
int encoderGetFrequency (double *pFreq);
int encoderSetLineSelectMode (int mode);
int encoderGetLineSelectMode (int *pMode);

// ffc.c
int ffcInitialize (unsigned int flashAdrs, unsigned int memAdrs, int mode);
int ffcInitializeAdmin (unsigned int flashAdrs, unsigned int memAdrs, int mode);
int ffcInitializeMain (unsigned int flashAdrs, unsigned int memAdrs, int mode, int userMode);
int ffcRegInit (unsigned int adrs, unsigned int size, int mode, int select);
int ffcSetAdjustParamZero (void);
int ffcSetAdjustParam (void);
int ffcRegInitIntMem (int ffcNo, int mode);
int ffcSetMode (int offsetMode, int gainMode);
int ffcGetMode (int *pOffsetMode, int *pGainMode);
int ffcSetLoadNum (int ffcNum);
int ffcGetLoadNum (int *pFfcNum);
int ffcLoadMain (int ffcNum, int userMode);
int ffcLoad (int ffcNum);
int ffcLoadAdmin (int ffcNum);
int ffcSave (int ffcNum, int memType);
int ffcSaveAdmin (int ffcNum, int memType);
int ffcGetParam (unsigned int *pMemAdrs, unsigned int *pSize, unsigned int *pDmaWait, int *pMode);
int ffcGetMemAdrs (int ffcNum, int memType, unsigned int *pAdrs);
int ffcGetFlashAdrs (int ffcNum, unsigned int *pAdrs);
int ffcGetFlashAdrsAdmin (int ffcNum, unsigned int *pAdrs);
int ffcGetNumFromMem (unsigned int memAdrs, int *pFfc);
int ffcGetNumFromFlash (unsigned int flashAdrs, int *pFfc);
int ffcGetNumFromFlashAdmin (unsigned int flashAdrs, int *pFfc);
int ffcSetOffsetData (int ffcNum, int x, int y, unsigned int data);
int ffcGetOffsetData (int ffcNum, int x, int y, unsigned int *pData);
int ffcSetGainData (int ffcNum, int x, int y, unsigned int data);
int ffcGetGainData (int ffcNum, int x, int y, unsigned int *pData);
int ffcSetGainX (int ffcNum, int x, int y, double data);
int ffcGetGainX (int ffcNum, int x, int y, double *pData);
int ffcGetOffsetAve (int ffcNum, int dpcNum, double *pAve, FFC_OG_INFO *pMin, FFC_OG_INFO *pMax);
int ffcGetGainAve (int ffcNum, int dpcNum, double *pAve, FFC_OG_INFO *pMin, FFC_OG_INFO *pMax);
int ffcDataUpload (int ffcNum);
int lfFfcUploadAsciiToBin (void *pAscii, void *pBinary, unsigned int size);
int ffcDataUploadBin (int ffcNum);
int lfFfcUploadAsciiToReg (void *pAscii, void *pBinary, unsigned int size);
int ffcDataDownload (int ffcNum, int mode);
int lfFfcDownloadBintoAscii (void *pAscii, void *pBinary, unsigned int *pSize);
int ffcDataDownloadBin (int ffcNum, int mode);
int lfFfcDownloadRegtoAscii (void *pAscii, void *pBinary, unsigned int *pSize);
int ffcSetMemoryOffsetGain (int ffcNo, unsigned int memAdrs, unsigned short *pImageOffset, int *pImageGain);
int ffcGetMemoryOffsetGain (int ffcNo, unsigned int memAdrs, unsigned short *pImageOffset, int *pImageGain);
int ffcGetExtMemory (unsigned int extMemAdrs, unsigned int intMemAdrs);
int ffcSetExtMemory (unsigned int extMemAdrs, unsigned int intMemAdrs);
int ffcToFlash (unsigned int flashAdrs, unsigned int memAdrs, int size);
int ffcToFlashAdmin (unsigned int flashAdrs, unsigned int memAdrs, int size);
int ffcToMemory (unsigned int flashAdrs, unsigned int memAdrs, int size);
int ffcToMemoryAdmin (unsigned int flashAdrs, unsigned int memAdrs, int size);
int ffcCopy (int srcFfcNum, int desFfcNum);
int ffcDefault (void);
int ffcSetBlackTarget (unsigned int data);
int ffcGetBlackTarget (unsigned int *pData);
int ffcSetWhiteTarget (unsigned int data);
int ffcGetWhiteTarget (unsigned int *pData);
int ffcGetBlackTargetFromFlash (int ffcNum, unsigned int *pData);
int ffcGetBlackTargetFromFlashAdmin (int ffcNum, unsigned int *pData);
int ffcGetWhiteTarget (unsigned int *pData);
int ffcAdjustCheckModeParam (int mode);
int ffcSetAdjustRate (int mode, float rate);
int ffcGetAdjustRate (int mode, float *pRate);
int ffcSetAdjustExposure (int mode, int exp);
int ffcGetAdjustExposure (int mode, int *pExp);
int ffcSetAdjustTarget (int mode, int target);
int ffcGetAdjustTarget (int mode, int *pTarget);
int ffcSetAdjustTemp (int mode, float temp);
int ffcGetAdjustTemp (int mode, float *pTemp);
int ffcSetAdjustGainX (int mode, float gain);
int ffcGetAdjustGainX (int mode, float *pGain);
int ffcSetAdjustBit (int mode, int bit);
int ffcGetAdjustBit (int mode, int *bit);
int ffcAdjustUpdate (void);
int ffcSetMemoryNormalFirst (void *srcAdrs, unsigned int offset, unsigned int size);
int ffcGetMemoryNormalFirst (void *desAdrs, unsigned int offset, unsigned int size);
int ffcSetMemoryNormal (void *srcAdrs, unsigned int offset, unsigned int size, int ffcCorMode);
int ffcGetMemoryNormal (void *desAdrs, unsigned int offset, unsigned int size, int ffcCorMode);
int ffcDataSwap (unsigned int adrs, unsigned int size);
int ffcGetBitCalc (int *pData);
int ffcGetBitCalc2 (int bit, int *pData);
int ffcDataXFlip (int mode);
void ffcSetXFlipFlag (int mode);
int ffcGetXFlipFlag (void);
int ffcSetCorMode (int mode);
int ffcGetCorMode (int *pMode);
int ffcDataReplacement  (int lineStart, int lineSize, int ffcCorMode);
int ffcDataReplacementFirst (int lineStart, int lineSize);
int ffcGetReplacementLine (int *pLineStart, int lineSize, int ffcCorMode);
int ffcGetReplacementLineFirst (int *pLineStart, int lineSize);
int ffcCopyBuffToExtMem (unsigned long memAdrs, unsigned int *pBuffer);
int ffcCopyExtMemToBuff (unsigned long memAdrs, unsigned int *pBuffer);
int ffcSetMarginGridData (void);

// ffcAdjust.c
int ffcBlack (FFC_PARAM ffcParam);
int ffcWhite (FFC_PARAM ffcParam);
int ffcFlashSave (FFC_PARAM ffcParam);
int acquisitionParamSave (void);
int acquisitionParamRestore (void);
int dogParamSave (void);
int dogParamRestore (void);
int tempParamSave (void);
int tempParamRestore (void);
int ffcGetFlashAdrs (int ffcNo, unsigned int *pFlashAdrs);
int ffcGetMemoryAdrs (int ffcNo, unsigned int *pMemAdrs);
int ffcAdjustInfo (int ffcNum, int userMode);
int ffcAdjustInfoBlackSave (int ffcNum, int userMode, FFCINFO ffcInfo);
int ffcAdjustInfoWhiteSave (int ffcNum, int userMode, FFCINFO ffcInfo);
int ffcAdjustInfoBlackRead (int ffcNum, int userMode, FFCINFO *pFfcInfo);
int ffcAdjustInfoWhiteRead (int ffcNum, int userMode, FFCINFO *pFfcInfo);
int cmdSensorTempBatch (double sensorTemp);

// firm.c
int firmMain (void);
int firmErrorAttention (int errNo);
int firmCheckConsole (void);
int firmSetCmdStatus (int mode);
int firmGetCmdStatus (int *pMode);

// firmUpdate.c
int firmUpdateAll (void);
int firmUpdateXml (void);
int firmDownloadXml (int size);
int firmUpdateAllAdmin (void);
int firmUpdateMain (unsigned int flashAdrs, unsigned int size, int offsetMode);
int firmUpdateFlashWrite (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int firmDownloadAll (unsigned int size);
int firmDownloadAllAdmin (unsigned int size);
int firmDownloadMain (unsigned int flashAdrs, unsigned int size, char *pName);
int fpgaUpdatePolafire (void);
int fpgaUpdatePolafireMain (unsigned int flashAdrs, unsigned int size, int offsetMode);
int fpgaUpdatePolafirelashWrite (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int fpgaDownloadPolafire (int mode);
int fpgaUpdatePolarFire (void);
int fpgaUpdatePolarFireAdmin (void);
int fpgaUpdatePolarFireMain (unsigned int flashAdrs, unsigned int size, int offsetMode);
int fpgaUpdatePolarFireReconfig (void);
int fpgaUpdatePolarFireFlashWrite (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int fpgaUpdatePolarFireHeader(void);
int fpgaDownloadPolarFire (void);
int fpgaDownloadPolarFireMain (unsigned int flashAdrs, unsigned int size, char *pName);
int ifFpgaCopyUserToGolden (void);
int firmUpdatePhy (void);
int phyDataCopy (void);

// func.c
int cameraLogMsg (int level, const char *fileName, const char *funcName, unsigned int fileLine, int errorNumber, char *msg);
int cameraErrorFunc (int level, const char *fileName, const char *funcName, unsigned int fileLine, int errorNumber);
unsigned int cameraGetCounter (void);
unsigned char bitSwap (unsigned char byte);
int defaultAll (void);
int cmdInterruptMode (int mode);
int cmdExecuteInit (void);
int cmdExecuteStatus (int *pStatus);

// gigeCmdDiag.c
#if !defined (MODE_GIGE_10G)
int phyReadReg (unsigned int adrs, unsigned int *pData);
int phyWriteReg (unsigned int adrs, unsigned int data);
#else
int phyReadReg (unsigned int dev, unsigned int adrs, unsigned int *pData);
int phyWriteReg (unsigned int dev, unsigned int adrs, unsigned int data);
int phyGetCount (unsigned int speed, unsigned int selector, unsigned int *pCount);
int phyGetSpeed (unsigned int *pSpeed);
int gevParamInitialize (void);
int gevGetSpeedConfig (unsigned int *pSpeed);
int gevSetSpeedConfig (unsigned int speed);
int phyDump (unsigned int option);
int phyInit (void);
int gigEInit (void);
int phyFirmDownload (void);
int phyGetTempCmd (float *pTemp);
int gigESend (void);
#endif

// gigeCmdDpc.c
int gigeCmdDpc1Main (int dpcNum, int ffcNum);
int gigeCmdDpc2Main (int dpcNum, int ffcNum);
int gigeCmdDpc3Main (int dpcNum);

// gigeCmdFfc.c
int gigeCmdFfcSaveMain (int ffcNo);
int gigeCmdFfcSaveAdminMain (int ffcNo);
int gigeCmdFfcDefault (void);
int gigeCmdFfcBlackMain (int ffcNo);
int gigeCmdFfcWhiteMain (int ffcNo);
int gigeCmdFfcShadingLineMain (int ffcNo);
int gigeCmdFfcCorecctionMode (int mode);

// gigeMain.c
int gigeMain (void);

// gigeCmdSpectrum.c
int gigeCmdSpectrunGetPeakWave (void);
int gigeCmdSpectrunSetPixelFormat (void);
int gigeCmdSpectrunSetHeader (void);
int gigeCmdSpectrunSetBandPerHeight (void);
int gigeCmdSpectrunBandGain (void);
int gigeCmdSpectrunBandGainX(void);
int gigeCmdSpectrunBandOffset (void);
int gigeCmdSpectrumBandOffset1(void);
int gigeCmdSpectrunFfcDmaSelectEnable (void);
int gigeCmdSpectrunFfcGainUpdate (void);
int gigeCmdSpectrunFfcGainUpdateX (void);
int gigeCmdSpectrunFfcGainUpdateStatus (void);
int gigeCmdSpectrunGetMaxBand (void);
int gigeCmdSpectrumDefaulty(void);

// gigeCmdUpdate.c
int gigeCmdAdmVersion (void);
int gigeCmdSensorFpgaUpload (void);
int gigeCmdFirmUpload (unsigned int flashAdrs, unsigned char *pBuffer, unsigned int size);
int gigeCmdXmlUpload (unsigned int flashAdrs, unsigned char *pBuffer, unsigned int size);
int gigeCmdIfFpgaUpload  (unsigned int flashAdrs, unsigned char *pBuffer, unsigned int size);
int gigeCmdPhyUpload (unsigned int flashAdrs, unsigned char *pBuffer, unsigned int size);
int gigeCmdSpectrumWaveUpload (void *pBuffer, unsigned int size);
int gigeCmdDpcUpload (void *pAdrs, unsigned int offset, unsigned int size);

// gigeMain.c
int gigeMain (void);
int gigEInitialize (void);
int gigEMainProcess (void);

// gigeParam.c
int gigeParamInitialize (void);
int gigeParamWriteRegister (int userNum, unsigned int offset, unsigned int size);
int gigeParamSaveRegister (int userNum,  unsigned int offset, unsigned int size);
int gigeParamDefault (void);
int gigeParamInitialize2 (void);

// gpio.c
int getGpi (unsigned int *pData);
int getGpo (unsigned int *pData);
int setGpo (unsigned int data);
int gpioSetDnf (int number, unsigned int highData, unsigned int lowData);
int gpioGetDnf (int number, unsigned int *pHighData, unsigned int *pLowData);
int gpioGetPulseTime (int number, unsigned int *pHighFreq, unsigned int *pLowFreq);

// hwInit.c
int hwInitialize (int cpuType);
int hwInitialize2 (int mode);
int hwInitializeNormal (void);

// i2c.c
int i2cInitialize (void);
int i2cWrite (int port, unsigned int slaveAdrs, unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int i2cRead (int port, unsigned int slaveAdrs, unsigned int adrs, unsigned char *pBuffer, unsigned int size);

// intc.c
int intcInitialize (void);
int intcCpuAttention (int irq, int cpuID);

// ipu.c
int ipuFfcInitialize (void);
int ipuDpcInitialize (void);
int ipuSetGlobalInt (int mode);
int ipuIntEnable (unsigned int data);
int ipuIntDisable (unsigned int data);
unsigned int ipuIntStatus (void);
void ipuIntClear (unsigned int data);

// kermit.c
int kermitRecv (char *pBuffer, unsigned int size);
int kermitRcvfil (void);
int kermitClosof (void);
int kermitInput (void);
int kermitResend (void);
void kermitNxtpkt (void);
void kermitTinit (void);
void kermitRinit (void);
int kermitAck (void);
int kermitAck1 (char *);
int kermitNak (void);
int kermitSpack (char type, int n, int len, char *d);
int kermitRpack (void);
int kermitChkl (char *packet);
int kermitChksum (char *);
void kermitEncode (char a);
int kermitDecode (void);
int kermitPnchar (char c);
void kermitSpar (char *s);
char *kermitRpar (void);
int kermitTtflui (void);
int kermitTtol (char *s, int n);
int kermitTtinl (char *dest, int max, char eol, int timo);
char *kermitStrchr (char *s, int c);
void kermiSendByte (char byte);
char kermitRecvByte (void);

int kermitSend (char *pFileName, char *pBuffer, int size);
int kermitSfile (char *pFilename, char *pBuffer, int size);
int kermitSdata (void);
int kermitEncstr (char *s);
int kermitGetpkt (int maxlen);
int kermitSeof (char *s);
int kermitSeot (void);
int kermitGnchar (void);
int kermitSinit (char c);
int kermitGzipDataCheck (unsigned char **pUnCompress, int *pUncomprLen, unsigned char *pCompress, int comprLen, int mallocSize);

// led.c
int setLed (int number, int mode);
int setLedMain (int number, int mode);
int getLed (int number, int *pMode);
int ledBootState (void);
int ledHwSystemErrorState (void);
int ledConnectState (void);
int ledConnectStateForce (void);
int ledSettingState (void);
int ledCmdExeState (void);
int ledDiagnosticState (void);
int ledSystemErrorState (void);
int ledReturnState (int led0, int led1);
int ledSetDebugMode (int mode);
int ledGetDebugMode (int *pMode);

// lut.c
int lutInitialize (void);
int lutDefault (void);
int lutSetMode (int lutMode);
int lutGetMode (int *pLutMode);
int lutSetEnable (int select, int mode);
int lutGetEnable (int *pSelect, int *pMode);
int lutSetFormat (int select, int format);
int lutGetFormat (int select, int *pFormat);
int lutSetBinThreshold (int select, int threshold);
int lutGetBinThreshold (int select, int *pThreshold);
int lutSetGamma (int select, float gamma);
int lutGetGamma (int select, float *pGamma);
int lutSetDataGamma (int select, float gamma);
int lutSetGammaCamera (int select, float gamma);
int lutGetGammaCamera (int select, float *pGamma);
int lutSetDataGammaCamera(int select, float gamma);
int lutSetTable (int select, unsigned char *pBuff, int count, int bit);
int lutGetTable (int select, unsigned char *pBuff, int count, int bit);
int lutSetTableData (int select, unsigned int offset, unsigned int data);
int lutGetTableData (int select, unsigned int offset, unsigned int *pData);
int lutSetDataFromFlash (int select, unsigned int size);
int lutSetFlash (int select, unsigned int *pBuffer, unsigned int size);
int lutGetFlash (int select, unsigned int *pBuffer, unsigned int size);
int lutLoadData (int select);
int lutReadData (int select, void *pAdrs);
int lutGetMemAdrs (int select, unsigned int *pAdrs);
int lutGetFlashAdrs (int select, unsigned int *pAdrs);
int lutGetTableSize (unsigned int *pSize);
int lutGetTablInterval (int bit, unsigned int *pSize);
int lutDataUpload (int select, int bit);
int lfLutUploadAsciiToBin (void *pAscii, void *pBinary, int bit, int size);
int lutDataDownload (int select, int bit, int mode);
int lfLutDownloadBintoAscii (void *pAscii, void *pBinary, int bit, unsigned int *pSize);
int lutSetRange (int select, int max, int min);
int lutGetRange (int select, int *pMax, int *pMin);

// main.c
int main (void);

// map.c
int map (void);

// mtd.c
int phyInitialize (void);
int phyInitialize2 (void);
int gigEIpRxAlignCheckStatus (void);
int phyGet10G_LinkStatus (int *pLinkStatus);
int phyGet1G_LinkStatus  (int *pLinkStatus);
//@@@1int phyGetCopperLinkStatus (u32 *pLinkState);
//@@@1int phyGetHostLinkStatus (u32 *pLinkStatus);
int phyGetSpeedDuplex (int *pSpeed);
void phyDump_debug(int opt);
int phyGetFirmVersion (char *pVer);
int phyGetApiVersion (char *pVer);
int phyGetFirmVersionMem (char *pVer);
int phyGetApiVersionMem (char *pVer);
int phyGetTemp (float *pTemp);
int phyDataFlashToMem (void);
int gigeGetSpeed (int *pSpeed);
int gigeSetSpeed (int speed);
int gigeGetSpeedBps (int *pSpeed);
int gigeGetLanBandMax (int speed, double *pLanBandMax);
int gigECalcPacketDelay (unsigned int *pCount);
int gigeGetPacketDelayData (unsigned int *pData);
int mdtFirmDownload (void);

// peltier.c
int peltierInitialize (void);
void peltier_InterruptHandler (void *CallBackRef);
int peltierSetMountState (int state);
int peltierGetMountState (int *pState);
int peltierCheckMountState (void);
int peltierSetEnable (int mode);
int peltierGetEnable (int *pMode);
int peltierEnable (void);
int peltierEnable2 (void);
int peltierDisable (void);
int peltierSetMode (int mode);
int peltierGetMode (int *pMode);
int peltierPowerInitialize (void);
int peltierSetIntEnable (unsigned int data);
int peltierGetIntEnable (unsigned int *pData);
int peltierSetGlobalInt (int mode);
int peltierGetGlobalInt (int *pMode);
int peltierSetTarget (double temp);
int peltierGetTarget (double *pTemp);
int peltierGetTargetMinMax (double *pMinTemp, double *pMaxTemp);
int peltierSetSensorTempAlarm (double tempOver, double tempUnder);
int peltierSetSensorTempAlarmCalc (double alarm, unsigned int *pData);
int peltierGetSensorTempAlarm (double *pTempOver, double *pTempUnder);
int peltierGetSensorTempAlarmCalc (unsigned int reg, double *pTempAlarm);
int peltierGetSensorTempAlarmMinMax (double *pMinTemp, double *pMaxTemp);
int peltierSetCaseTempAlarm (double tempOver, double tempUnder);
int peltierSetCaseTempAlarmCalc (double alarm, unsigned int *pData);
int peltierGetCaseTempAlarm (double *pTempOver, double *pTempUnder);
int peltierGetCaseTempAlarmCalc (unsigned int reg, double *pTempAlarm);
int peltierGetSensorTemp (double *pTemp);
int peltierGetCaseTemp (double *pTemp);
int peltierGetTempAlarmStatus (unsigned int *pStatus);
int peltierClearTempAlarm (unsigned int clear);
int peltierGetMakeTempAlarmStatus (unsigned int *pStatus);
int peltierGetMakeTempAlarmStatus2 (unsigned int *pStatus);
int peltierErrorStatus (int errNo);
int peltierGetClip (unsigned int *pLow, unsigned int *pHigh);
int peltierSetClip (unsigned int low, unsigned int high);
int peltierGetVolt (double *pVolt);
int peltierGetCurrent (double *pCurrent);
int tempSetAbnormalStatus (unsigned int data);
int tempGetAbnormalStatus (unsigned int *pData);
int tempSetAbnormalCount (unsigned int data);
int tempGetAbnormalCount (unsigned int *pData);
int tempSetAbnormalAcquisition (unsigned int data);
int tempGetAbnormalAcquisition (unsigned int *pData);
int tempAbnormalCheck (void);
int peltierControlInitiallize (void);
int peltierControlProcsInit (void);
int peltierControlProcs (void);
int peltierGetPowerLevel (int *pLevel);
int peltierSetPowerLevel (int level);
int peltierGetPowerInterval (unsigned int *pData);
int peltierSetPowerInterval (unsigned int data);
int peltierGetPowerHighClip (unsigned int *pData);
int peltierSetPowerHighClip (unsigned int data);
int peltierGetPowerStartFlag (unsigned int *pData);
int peltierSetPowerStartFlag (unsigned int data);

// pfFlash.c
int PolarFireFlashInitialize (void);
int PolarFireFlashFinalize (void);
int PolarFireSpiMode (int mode);
int PolarFireFlashWrite (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int PolarFireFlashRead (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int PolarFireFlashSectorErase (unsigned int adrs);
int PolarFireFlashBlockErase (unsigned int adrs);
int PolarFireFlashEraseAll (void);
int PolarFireFlashStatusCheck (void);

// qspiFlash.c
int qspiFlashInitialize (void);
int qspiFlashWrite (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int qspiFlashRead (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int qspiFlashSectorErase (unsigned int adrs);
int qspiFlashEraseAll (void);
int qspiFlashWriteProtect (unsigned int address);
int qspiFlashWriteProtectRead (int *pStatus);
int qspiFlashWriteProtectReadShow (void);
int qspiFlashErrorStatus (int errNo);
int qspiFlashSetBank (unsigned int bankNum);
int qspiFlashGetBank (unsigned int *pBankNum);
int qspiFlashGetFlashId (unsigned char *pManufacturerId, unsigned char *pDeviceId);

// qspiFlash_N25Q_Micron.c
int qspiFlashInitialize_N25Q (void);
int qspiFlashWrite_N25Q (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int qspiFlashRead_N25Q (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int qspiFlashSectorErase_N25Q (unsigned int adrs);
int qspiFlashEraseAll_N25Q (void);
int qspiFlashStatusCheck_N25Q (unsigned int timeout);
int qspiFlashWaitReadyEraseCheck_N25Q (unsigned int timeout);
int qspiFlashWriteProtectMake_N25Q (int top, int data, int *pMakeData);
int qspiFlashWriteProtect_N25Q (unsigned int address);
int qspiFlashWriteEnableCmd_N25Q (void);
int qspiFlashWriteDisableCmd_N25Q (void);
int qspiFlashExAddresseCmd_N25Q (unsigned int adrs);
int qspiFlash3ByteAddresseCmd_N25Q (void);
int qspiFlashWriteProtectRead_N25Q (int *pStatus);
int qspiFlashWriteProtectReadShow_N25Q (void);
int qspiFlashSetBank_N25Q (unsigned int bankNum);
int qspiFlashGetBank_N25Q (unsigned int *pBankNum);

// qspiFlash_ISSI.c
int qspiFlashInitialize_ISSI (void);
int qspiFlashWrite_ISSI (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int qspiFlashRead_ISSI (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
int qspiFlashSectorErase_ISSI (unsigned int adrs);
int qspiFlashEraseAll_ISSI (void);
int qspiFlashStatusCheck_ISSI (unsigned int timeout);
int qspiFlashWaitReadyEraseCheck_ISSI (unsigned int timeout);
int qspiFlashWriteProtectMake_ISSI (int top, int data, int *pMakeData);
int qspiFlashWriteProtect_ISSI (unsigned int address);
int qspiFlashWriteEnableCmd_ISSI (void);
int qspiFlashWriteDisableCmd_ISSI (void);
int qspiFlashExAddresseCmd_ISSI (unsigned int adrs);
int qspiFlash3ByteAddresseCmd_ISSI (void);
int qspiFlashWriteProtectRead_ISSI (int *pStatus);
int qspiFlashWriteProtectReadShow_ISSI (void);
int qspiFlashSetBank_ISSI (unsigned int bankNum);
int qspiFlashGetBank_ISSI (unsigned int *pBankNum);

// regMap.c
int fpgaRegMap (void);

// roi.c
int roiSetSelector (int selector);
int roiGetSelector (int *pSelector);
int roiInitialize (void);
int roiSetEnd (void);
int roiGetErrStatus (unsigned int *pStat);
int roiGetErrStatusMulti (int selector, unsigned int *pStat);
int roiSetEntryCount (int count);
int roiGetEntryCount (int *pCount);
int roiSetValid (int valid);
int roiGetValid (int *pValid);
int aoiSetWidth (int size);
int aoiGetWidth (int *pSize);
int aoiSetWidthOffset (int offset);
int aoiGetWidthOffset (int *pOffset);
int roiSetWidthMain (int offset, int size);
int aoiSetHeight (int size);
int aoiGetHeight (int *pSize);
int aoiSetHeightOffset (int offset);
int aoiGetHeightOffset (int *pOffset);
int roiSetHeightMain (int offset, int size);
int roiHeightAdd (int selector, int yOffset, int ySize);
int roiHeightDel (int selector, int yOffset, int ySize);
int roiGetHeightIndex (int *pIndex, int yOffset, int ySize);
int roiHeightPositionCheck (int selector, int yOffset, int ySize, int *pIndex);
int roiSetDefaultY (void);
int roiShow (int mode);
int roiSetAreaSize (int mode);
int roiGetAreaSize (int *pMode);
int roiGetAreaWidthSize (int mode, int *pFpgaSize, int *pFpgaOffset, int *pSensorSize, int *pSensorOffset);
int roiGetAreaHeightSize (int mode, int *pFpgaSize, int *pFpgaOffset, int *pSensorSize, int *pSensorOffset);
int roiRestoreAreaSize (void);
int roiSetDmaFull();
int roiSetDmaResutore();
int roiGetAreaGridOffset (int *pX, int *pY);
int roiGetAreaWidthMax (int *pWidth);
int roiGetAreaHeightMax (int *pWidth);
int roiSetAdjustMode (int mode);
int roiGetAdjustMode (int *pMode);
int roiSetAreaFlag (int mode);
int roiGetAreaFlag (int *pMode);
int roiGetNoSelfValidCount (int selector, int *pCount);
int roiCheckMultiMode (int *pMode);
int roiCoordinateTrans (int srcX, int srcY, int *dstX, int *dstY, int mode);

// roiFpgaCamera.c
int fpgaRoiSetCameraWidth (int selector, int offset, int size, int valid);
int fpgaRoiGetCameraWidth (int selector, int *pOffset, int *pSize, int *pValid);
int fpgaRoiSetCameraHeight (int selector, int offset, int size, int valid);
int fpgaRoiGetCameraHeight (int selector, int *pOffset, int *pSize, int *pValid);
int fpgaRoiGetSensorHeight (int selector, int *pOffset, int *pSize, int *pValid);
int fpgaRoiGetSensorValid (int selector, int *pValid);
int fpgaRoiSetValid (int selector, int selectWH, int valid);
int fpgaRoiGetValid (int selector, int selectWH, int *pValid);

int memRoiSetCameraWidth (int selector, int offset, int size, int valid);
int memRoiGetCameraWidth (int selector, int *pOffset, int *pSize, int *pValid);
int memRoiSetCameraHeight (int selector, int offset, int size, int valid);
int memRoiGetCameraHeight (int selector, int *pOffset, int *pSize, int *pValid);
int memRoiSetValid (int selector, int selectWH, int valid);
int memRoiGetValid (int selector, int selectWH, int *pValid);

int fpgaRoiSetCameraWidthMain (unsigned int adrs, int selector, int offset, int size, int valid);
int fpgaRoiGetCameraWidthMain (unsigned int adrs, int selector, int *pOffset, int *pSize, int *pValid);
int fpgaRoiSetCameraHeightMain (unsigned int adrs, int selector, int offset, int size, int valid);
int fpgaRoiGetCameraHeightMain (unsigned int adrs, int selector, int *pOffset, int *pSize, int *pValid);
int fpgaRoiGetCameraHeightTotalSize (int *pSize);
int fpgaRoiGetSensorHeightTotalSize (int *pSize);
int fpgaRoiGetOffsetY (int *pOffsetY);

// roiSensor.c
int sensorRoiSetWidth (int selector, int offset, int size, int valid);
int sensorRoiGetWidth (int selector, int *pOffset, int *pSize, int *pValid);
int sensorRoiSetHeight (int selector, int offset, int size, int valid);
int sensorRoiGetHeight (int selector, int *pOffset, int *pSize, int *pValid);
int sensorRoiSetValid (int selector, int selectWH, int valid);
int sensorRoiGetValid (int selector, int selectWH, int *pValid);

// sensor.c
int sensorInitialize (void);
int sensorInitialize2 (void);

// sensorVendorS.c
int sensorTimingInitialize (void);
int sensorVendorSInitialize (void);
int sensorVendorSInitialize2 (void);
int sensorResizeModeInitialize(void);
int sensorVendorSHideRegSetting (void);
int sensorSetRegHold (int regHold);
int sensorRegWriteByte (unsigned char id, unsigned char adrs, unsigned char data, int acTmg, int regHold);
int sensorRegReadByte (unsigned char id, unsigned char adrs, unsigned char *pData, int acTmg, int regHold);
int sensorRegWriteBase (unsigned char *pId, unsigned char *pAdrs, unsigned char *pData, int size, int acTmg);
int sensorRegReadBase (unsigned char *pId, unsigned char *pAdrs, unsigned char *pData, int size, int acTmg);
int sensorGetFPGAVersion (unsigned int *pVer);
int sensorModel(char *pModel);
int sensorFpgaSetTestPattern (int mode);
int sensorFpgaGetTestPattern (int *pMode);
int sensorFpgaSetTestPatternData (int data);
int sensorFpgaGetTestPatternData (int *pData);
int sensorStandBy (void);
int sensorStandByCancel (void);
int sensorSetBlackPixel (int mode);
int sensorGetBlackPixel (int *pMode);
int sensorSetGainDB (float gain);
int sensorGetGainDB (float *pGain);
int sensorSetGainX (float gain);
int sensorGetGainX (float *pGain);
int sensorSetXFlip (int mode);
int sensorTpSetMode (int mode);
int sensorTpGetMode (int *pMode);
int sensorTpSetInc (unsigned char hInc, unsigned char vInc);
int sensorTpGetInc (unsigned char *pHInc, unsigned char *pVInc);
int sensorTpSetData (unsigned int data1, unsigned int data2);
int sensorTpGetData (unsigned int *pData1, unsigned int *pData2);
int sensorBitSetHide (int bit);
int sensorSetHvGen (int mode);
int sensorGetHvGen (int *pMode);
int sensorSetTrgControl (int mode);
int sensorGetTrgControl (int *pMode);
int sensorSetConversionGain (int mode);
int sensorGetConversionGain (int *pMode);
int sensorGradationCompInitialize (void);
int sensorGradationCompSetModeDDR (int mode);
int sensorGradationCompGetModeDDR (void);
int sensorGradationCompSetModeDDR2 (int mode);
int sensorGradationCompGetModeDDR2 (void);
int sensorGradationCompSetMode (int mode);
int sensorGradationCompSetModeMain (int mode);
int sensorGradationCompSetModeMain2 (int mode);
int sensorGradationCompSetModeReg (int mode);
int sensorGradationCompGetMode (int *pMode);
int sensorGradationCompSetPosition (int first, int second);
int sensorGradationCompSetPositionReg (int first, int second);
int sensorGradationCompGetPosition (int *pFirst, int *pSecond);
int sensorGradationCompSetGain (int first, int second);
int sensorGradationCompSetGainReg (int first, int second);
int sensorGradationCompGetGain (int *pFirst, int *pSecond);
int sensorSet8BitConvert (int mode);
int sensorGet8BitConvert (int *pMode);
int sensorFrameRateHighSpeedModeInitialize (void);
int sensorFrameRateHighSpeedModeInitializeDefault (void);
int sensorGetFrameRateHighSpeedMode (int *pMode);
int sensorSetFrameRateHighSpeedMode (int mode);
int sensorSetFrameRateHighSpeedFpga (int mode);
int sensorSetFrameRateHighSpeedReg (int mode);
int sensorSetFrameRateHighSpeedStartLine (int startLine);
int sensorGetFrameRateHighSpeedStartLine (int *pStartLine);
int sensorGetVirtualHeightMode (int *pMode);
int sensorSetVirtualHeightMode (int mode);
int sensorGetVirtualHeightLineScanMode (int *pMode);
int sensorSetVirtualHeightLineScanMode (int mode);
int sensorGetVirtualHeight (int *pHeight);
int sensorSetVirtualHeight (int height);
int sensorCheckVirtualHeight (int virtualHeight, int *pVirtualHeight);
int sensorVendorSFrameRateSpeedModeInitialize (int mode);
int sensorVendorS_FrameRateHighSpeedModeEnableSetting (int height);
int sensorVendorS_FrameRateHighSpeedModeDisableSetting (void);
int sensorGetTgpdHighSpeed (int height, int *pTgpd);
int sensorGetTgesHighSpeed (int height, int *pTges);
int sensorSetResizeMode (int mode);
int sensorGetResizeMode (int *pMode);
int sensorSetFPGAResizeMode (int mode);
int sensorGetFPGAResizeMode (int *pMode);
int sensorSetSHS_H (unsigned int expTime);
int sensorGetSHS_H (unsigned int *pExpTime);
int sensorSetSHS_Second (unsigned int expTime);
int sensorGetSHS_Second (unsigned int *pExpTime);
int sensorTrgDisable (int *pTrgMode, int *pHvGenMode);
int sensorTrgEnable (int trgMode, int hvGenMode);
int sensorSetAD (int bit);
int sensorSetOB (int bit);
int sensorSetBL (int bit);
int sensorHIntervalData (int bit, unsigned int *pData);
int sensorHIntervalDataDrrs (int bit, unsigned int *pData);
int sensorHIntervalCalc (double frameRate, unsigned int *pData);
int sensorSetHIntervalReg (unsigned int data);
int sensorGetHIntervalReg (unsigned int *pData);
int sensorSetHInterval (int bit);
int sensorSetHIntervalTrgNormal (double frameRate);
int sensorSetHIntervalData (unsigned int data);
int sensorGetHInterval (unsigned int *pHinterval);
int sensorSetOutputMode (int mode);
int sensorGetOutputMode (int *pMode);
int sensorGetRegDefault (void);
int sensorSetRegDefault (void);
int sensorDrrsInitialize (void);
int sensorDrrsSetBlacklevel (void);
int sensorSetDrrsMain2 (int mode);
int sensorSetDrrsMain (int mode);
int sensorSetDrrs (int mode);
int sensorGetDrrs (int *pMode);
int sensorSetDrrsSensorRegister (int mode);
int sensorSetFpgaShutterMode (int mode);
int sensorGetFpgaShutterMode (int *pMode);
int sensorSetShutterMode (int mode);
int sensorGetShutterMode (int *pMode);
int sensorVendorSDrrsHideRegSetting (void);
int sensorVendorSDrrsHideRegBit (int bit);

// special.c
int specialInitialize (void);

// spectrum.c
int spectrumInitialize (void);
int spectrumBandMax (double *pData);
int spectrumBandMin (double *pData);
int spectrumBandPeakWave (double *pData);
int spectrumBandPeakWaveIndex (int index, double *pData);
int spectrumBandMaxCount (int *pData);
int spectrumBandResolution (int *pData);
int spectrumBandHalfWidth (double *pData);
int spectrumBandPeakWave2 (int index, double *pData);
int spectrumBandPeakAllWave(int index, double *pData);
int spectrumBandHalfWidth2 (int index, double *pData);
int spectrumInfomation (void);
int spectrumBandInfomation (int index);
int spectrumSetPixelFormat (int capMode);
int spectrumGetPixelFormat (int *pCapMode);
int spectrumSetHeightPerBand (int count);
int spectrumGetHeightPerBand (int *pCoiunt);
int spectrumSetBandCount (int count);
int spectrumGetBandCount (int *pCount);
int spectrumSetDescriptor (int index, int mode);
int spectrumGetDescriptor (int index, int *pMode);
int spectrumShowDescriptor (void);
int spectrumInitDescriptor (void);
int spectrumSetHeader (int hMode);
int spectrumGetHeader (int *pHMode);
int spectrumBufferReset (void);
int spectrumBandInfoUpload (void);
int spectrunBandInfoUploadAsciiToBin (void *pAscii, void *pBinary, unsigned int size);
int spectrumBandInfoDataDownload (int mode);
int spectrumDownloadBintoAscii (void *pAscii, void *pBinary, unsigned int *pSize);
int spectrumToFlash (unsigned int flashAdrs, unsigned int memAdrs, int size);
int spectrumToMemory (unsigned int flashAdrs, unsigned int memAdrs, int size);
int spectrumBandIndex2 (double wave, int *pIndex);
int specrtumBandSelectShow (void);
int spectrumBandROIMaxCount (int *pData);
int spectrumSetBandIndex(void);
int spectrumGetBandIndex(int srcIndex, int *dstIndex);
int spectrumBandIndexSort(void);
int spectrumSearchBandOffset(void);
int spectrumGetBandOffset(int* offset);
int spectrumSetDefaultMode (int mode);
int spectrumGetDefaultMode (int *mode);

// timer.c
int timerInitilize (void);
int timerStart (void);
int timerStop (void);
int timerSetSec (unsigned int count);
unsigned int timerGetSec (void);
int timerSetMs (unsigned int count);
unsigned int timerGetMs (void);
unsigned int timerGetCount (void);
unsigned int timerGetUsCount (void);
unsigned long long timerGetCount64 (void);
int sDelay (unsigned int delay);
int msDelay (unsigned int delay);
int usDelay (unsigned int delay);

// timerControl.c
int timerControlInitialize (void);
int timerControlSetSelect (int select);
int timerControlGetSelect (int *pSelect);
int timerControlSetDuration (unsigned int count);
int timerControlGetDuration (unsigned int *pCount);
int timerControlSetDelay (unsigned int count);
int timerControlGetDelay (unsigned int *pCount);
int timerControlReset (void);
int timerControlSetValue (unsigned int count);
int timerControlGetValue (unsigned int *pCount);
int timerControlGetStatus (unsigned int *pStatus);
int timerControlSetSource (int source);
int timerControlGetSource (int *pSource);
int lfTimerControlSourceCheck (int number);
int timerControlSetTrgActive (int mode);
int timerControlGetTrgActive (int *pMode);

// timingGenerator.c
int timingGeneratorNormalInitalize (void);
int timingGeneratorVersion2Initalize (void);
int tgSetReg2 (unsigned int adrs, unsigned int data);
int tgCtrlEnable2 (void);
int tgCtrlDisable2 (void);
int tgSetTgse (unsigned int data);
int tgSetTgse2 (unsigned int data);
int tgGetTgse (unsigned int *pData);
int tgSetTges (unsigned int data);
int tgSetTges2 (unsigned int data);
int tgGetTges (unsigned int *pData);
int tgSetTgpd (unsigned int data);
int tgSetTgpd2 (unsigned int data);
int tgGetTgpd (unsigned int *pData);
int tgSetSensorTrgRsvCtrl (void);
int tgSetSensorTrgDefault (void);
int tgSetSensorTrgCheck (void);
int tgSetSensorTrgCheck2 (void);

// uart.c
int uartInitialize (void);
int uartSetIntEnable(int port);
int uartSetIntDisable (int port);
int uartSend (int port, unsigned char *pBuffer, int size);
int uartRecv (int port, unsigned char *pBuffer, int size);
int uartRecvCount (int port);
int uartRecvClear (int port);
int uartSendClear (int port);

// user.c
int gigeSetConnectionConfig (void);

// userset.c
int userSetLoad (int userNum);
int userSetSave (int userNum);
int userSetDefault (int userNum);
int userSetBoot (int userNum);
int userGetBoot (int *pUserNum);
int userSetFactoryClear (void);

// version.c
int versionShow (void);
int firmStartMsg (int mode);
int setboardVersion (char *pVer);
int boardVersion (char *pVer);
int fpgaVersion (char *pVer);
int firmVersion (char *pVer);
int mainVersion (char *pVer);
int bootVersion (char *pVer);
int firmName (char *pName);
int firmBuildShow (void);
int firmGetBuildDate (char *pStr);
int firmGetBuildTime (char *pStr);
int fpgaBuildShow (void);
int fpgaGetBuildDate (unsigned int *pData);
int fpgaGetBuildNumber (unsigned int *pData);

// voltageIfBoard.c
#if defined (MODE_BOARD_ACB531CXP)

int voltIfInitialize (void);
int voltIfCheckValid (void);
int voltIfGet105vd (double *pData);
int voltIfGet105va (double *pData);
int voltIfGet125vd (double *pData);
int voltIfGet18vd (double *pData);
int voltIfGet25vf (double *pData);
int voltIfGet33vd (double *pData);
int voltIfGet90va (double *pData);
int ifGetInternalTemp (double *pData);
int voltIfBoardShow (void);

#endif

#if defined (MODE_BOARD_ACB532GE)

int voltIfInitialize (void);
int voltIfGet065v (double *pData);
int voltIfGet085va (double *pData);
int voltIfGet230va (double *pData);
int voltIfGet330v (double *pData);
int voltIfGet120vp (double *pData);
int voltIfGet240v (double *pData);
int ifGetInternalTemp (double *pData);
int voltIfBoardShow (void);

#endif

// voltageFpgaboard.c
int voltFpgaGetVccint (double *pData);
int voltFpgaGetVccaux (double *pData);
int voltFpgaGetVbram (double *pData);
int voltFpgaGetVccpint (double *pData);
int voltFpgaGetVccpaux (double *pData);
int voltFpgaGetVccoDdr (double *pData);
int voltFpgaGet33vd (double *pData);
int voltFpgaGet25v (double *pData);
int voltFpgaGetVtt (double *pData);
int voltFpgaBoardShow (void);

#if defined (MODE_SYSTEM_MANAGEMENT)
int voltFpgaGet09v (double *pData);
int voltFpgaGet12vSys (double *pData);
int voltFpgaGet115v (double *pData);
int voltFpgaGet12v (double *pData);
int voltFpgaGet33v (double *pData);
int voltFpgaGet33va (double *pData);
int voltFpgaGet50v (double *pData);
int voltFpgaGetVref (double *pData);
int voltFpgaGetVccPsintlp (double *pData);
int voltFpgaGetVccPsinfp (double *pData);
int voltFpgaGetVccPSAux (double *pData);
#endif

// xad.c
int xadcGetFpgaTemp (double *pTemp);

#endif  // __FIRM_H__

// eof
