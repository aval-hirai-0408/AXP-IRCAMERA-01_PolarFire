//**********************************************************************************
//
//                           ARM Header
//
//      Copyright (c) 2014 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// gigeCmd.h - GigE Command Header
//**********************************************************************************

#ifndef __GIGE_CMD_H__
#define __GIGE_CMD_H__

//----------------------------------------------------------------------------------
// Command
//----------------------------------------------------------------------------------

// GigE Command Access Mode
#define GIGE_CMD_ACES_READ					(0)
#define GIGE_CMD_ACES_WRITE					(1)

// Common
#define FIRM_CMD_OK							(0x00)		// Success
#define FIRM_CMD_ERROR						(0xffffffff)// Error
#define FIRM_CMD_NOP						(0x01)		// NOP
#define FIRM_CMD_RESET						(0x02)		// Reset
#define FIRM_CMD_DEFAULT_ALL				(0x03)		// All Default
//#define FIRM_CMD_DEVICE_CMD_EXE			(0x04)		// Command Execute


// FFC
#define FIRM_CMD_FFC_LOAD					(0x10)		// FFC Read from Flash
#define FIRM_CMD_FFC_SAVE					(0x11)		// FFC Write to Flash
#define FIRM_CMD_FFC_SAVE_FACTORY			(0x12)		// FFC Write to Flash Factory
#define FIRM_CMD_FFC_DEFAULT				(0x13)		// FFC Default

#define FIRM_CMD_FFC_BLACK					(0x17)		// FFC Black Image
#define FIRM_CMD_FFC_WHITE					(0x18)		// FFC White Image
#define FIRM_CMD_FFC_SAVE_ADMIN				(0x19)		// FFC Write to Flash Administrator

#define FIRM_CMD_FFC_CORRECTION_MODE		(0x70)		// FFC 補正モード切替
#define FIRM_CMD_FFC_ADJUST_UPDATE			(0x73)		// FFC 情報をDDR管理外からARM0の変数にコピー
#define FIRM_CMD_FFC_LOAD_ADMIN				(0x74)		// FFC Read from Flash(Administrator)
#define FIRM_CMD_FFC_SHADING_LINE			(0x76)		// FFC Shading Line補正


// DPC
#define FIRM_CMD_DPC_LOAD					(0x20)		// DPC Load
#define FIRM_CMD_DPC_SAVE_FACTORY			(0x21)		// DPC Save Factory
#define FIRM_CMD_DPC_DEFAULT				(0x22)		// DPC Default
#define FIRM_CMD_DPC1_FACTORY				(0x23)		// DPC Adjust Factory(パラメータ設定)
#define FIRM_CMD_DPC2_FACTORY				(0x24)		// DPC Adjust Factory(欠陥補正)
#define FIRM_CMD_DPC3_FACTORY				(0x25)		// DPC Adjust Factory(補正計算)
#define FIRM_CMD_DPC_ABORT					(0x26)		// DPC Abort
#define FIRM_CMD_DPC_GET_COUNT				(0x27)		// DPC Get Count
#define FIRM_CMD_DPC_DECREMENT_COUNT		(0x28)		// DPC Decrement Count
#define FIRM_CMD_DPC_SAVE_ADMIN				(0x29)		// DPC Save Administrator
#define FIRM_CMD_DPC_DEFAULT_REVERSE		(0x2a)		// DPC Default Reverse
#define FIRM_CMD_DPC_PIXEL_COUNT			(0x2b)		// DPC Pixel Count
#define FIRM_CMD_DPC_GET_GRID				(0x2c)		// DPC Get Grid
#define FIRM_CMD_DPC_ADD_GRID				(0x2d)		// DPC Add Grid
#define FIRM_CMD_DPC_DEL_GRID				(0x2e)		// DPC Delete Grid
#define FIRM_CMD_DPC_CLR_GRID				(0x2f)		// DPC Clear Grid

#define FIRM_CMD_DPC_LOAD_ADMIN				(0x80)		// DPC Load Factory(QSPI Flashデータ)
#define FIRM_CMD_DPC_ADJUST_UPDATE			(0x81)		// DPC情報をDDR管理外からARM0の変数にコピー
#define FIRM_CMD_DPC_MAP_CHANGE				(0x82)		// DPC Map Change

// Sensor
#define FIRM_CMD_SENSOR_BLACKPIXEL			(0x88)		// Black Pixel
#define FIRM_CMD_SENSOR_XFLIP				(0x89)		// X Flip
#define FIRM_CMD_SENSOR_BIT					(0x8a)		// Bit
#define FIRM_CMD_SENSOR_GAIN				(0x8b)		// Gain
#define FIRM_CMD_SENSOR_GRAD_8BIT_CONVERT	(0x8f)		// Gradation Compress 8bit Convert

// LUT
#define FIRM_CMD_LUT_LOAD					(0x30)		// LUT Read from Flash
#define FIRM_CMD_LUT_SAVE					(0x31)		// LUT Write to Flash
#define FIRM_CMD_LUT_DEFAULT				(0x32)		// LUT Default
#define FIRM_CMD_LUT_WRITE					(0x33)		// LUT Write to Memory
#define FIRM_CMD_LUT_READ					(0x34)		// LUT Read from Memory

// UserSet
#define FIRM_CMD_USERSET_LOAD				(0x40)		// UserSet Load
#define FIRM_CMD_USERSET_SAVE				(0x41)		// UserSet Save
#define FIRM_CMD_USERSET_DEFAULT			(0x42)		// UserSet Default
#define FIRM_CMD_USERSET_BOOT				(0x43)		// UserSet Boot
#define FIRM_CMD_USERID_SET					(0x44)		// User ID Set
#define FIRM_CMD_USERID_GET					(0x45)		// User ID Get

// Camera Information
#define FIRM_CMD_CAMERA_VENDOR				(0x50)		// ベンダ名設定
#define FIRM_CMD_CAMERA_MANUFACTURE			(0x51)		// 製造者名設定
#define FIRM_CMD_CAMERA_MODEL				(0x52)		// モデル名設定
#define FIRM_CMD_CAMERA_BOARDID				(0x53)		// ボードID設定
#define FIRM_CMD_CAMERA_SENSORID			(0x54)		// センサID設定

// Spectrum
#define FIRM_CMD_SPECTRUM_DEFAULTY			(0x6d)		// Spectrum DefaultY

// Sensor
#define FIRM_CMD_HIGH_SPEED_MODE			(0x92)		// High Speed Mode
#define FIRM_CMD_SENSOR_CONVERSION_GAIN		(0x93)		// Sensor Conversion Gain

// Upload
#define FIRM_CMD_FFC_UPLOAD					(0x94)		// FFC Upload
#define FIRM_CMD_FFC_DOWNLOAD				(0x95)		// FFC Download
#define FIRM_CMD_DPC_UPLOAD					(0x96)		// DPC Upload
#define FIRM_CMD_DPC_DOWNLOAD				(0x97)		// DPC Download
#define FIRM_CMD_ADM_UPLOAD					(0x98)		// ADM Upload
#define FIRM_CMD_IF_FPGA_UPLOAD				(0x99)		// IF FPGA Upload
#define FIRM_CMD_SENSOR_FPGA_UPLOAD			(0x9A)		// Sensor FPGA Upload
#define FIRM_CMD_FIRM_UPLOAD				(0x9B)		// Firm Upload
#define FIRM_CMD_XML_UPLOAD					(0x9C)		// XML Upload
#define FIRM_CMD_SPECTRUM_WAVE				(0x9D)		// Spectrum Wave Upload
#define FIRM_CMD_PHY_UPLOAD					(0x9E)		// PHY Upload

#define FIRM_CMD_AGING						(0xb0)		// Aging
#define FIRM_CMD_ROI_AREA					(0xb1)		// ROI Area
#define FIRM_CMD_DRRS						(0xb3)		// DRRS

#endif // __GIGE_CMD_H__

// eof
