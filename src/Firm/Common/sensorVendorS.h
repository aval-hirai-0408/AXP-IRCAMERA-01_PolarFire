//**********************************************************************************
//
//                               Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// sensorVendors.h - Sensor S Header
//**********************************************************************************


#ifndef __SENSOR_VENDOR_S_H__
#define __SENSOR_VENDOR_S_H__

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//--------------------------------------------------------------------------------
// Timeout
//--------------------------------------------------------------------------------
#define SENSOR_IMAGE_RDY_TIMEOUT							(25000)		// 5sec
#define SENSOR_REG_ACES_TIMEOUT								(25000)		// 5ssec


//--------------------------------------------------------------------------------
// Sensor Register
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Chip ID = 2
//--------------------------------------------------------------------------------
#define SENSOR_REG_STANDBY_ADRS								(0x00)
  #define SENSOR_REG_STANDBY_BIT							(1<<0)
#define SENSOR_REG_XMSTA_ADRS								(0x0c)
  #define SENSOR_REG_XMSTA_BIT								(1<<0)
#define SENSOR_REG_REGHOLD_ADRS								(0x34)
  #define SENSOR_REG_REGHOLD_BIT							(1<<0)
#define SENSOR_REG_WINMODE_ADRS								(0x3c)
  #define SENSOR_REG_WINMODE_V_HALF_BIT						(1<<0)
  #define SENSOR_REG_WINMODE_H_HALF_BIT						(1<<4)
  #define SENSOR_REG_PG_WINMODE_MASK						(0x03)
#define SENSOR_REG_VMAX1_ADRS								(0xd4)
#define SENSOR_REG_VMAX2_ADRS								(0xd5)
#define SENSOR_REG_VMAX3_ADRS								(0xd6)
#define SENSOR_REG_HMAX1_ADRS								(0xd8)
#define SENSOR_REG_HMAX2_ADRS								(0xd9)
#define SENSOR_REG_FREQ_ADRS								(0xdc)
  #define SENSOR_REG_FREQ_HALF_BIT							(1<<0)

#if defined (MODE_SENSOR_IMX990) || defined (MODE_SENSOR_IMX991)
	#define SENSOR_REG_GTWAIT_ADRS							(0xe2)
	  #define SENSOR_REG_GTWAIT_DEFAULT						(0x06)
	#define SENSOR_REG_GSDLY_ADRS							(0xe3)
	  #define SENSOR_REG_GSDLY_DEFAULT						(0x04)
#elif defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	#define SENSOR_REG_GSDLY_ADRS							(0xe3)
	  #define SENSOR_REG_GSDLY_DEFAULT						(0x08)
	#define SENSOR_REG_GTWAIT0_ADRS							(0xe6)
	#define SENSOR_REG_GTWAIT1_ADRS							(0xe7)
	#define SENSOR_REG_GTWAIT1_MASK							(0x03)
	  #define SENSOR_REG_GTWAIT_DEFAULT						(0x26)
#endif

//--------------------------------------------------------------------------------
// Chip ID = 3
//--------------------------------------------------------------------------------
#define SENSOR_REG_ROI_ON1_ADRS								(0x04)
#define SENSOR_REG_ROI_ON2_ADRS								(0x05)
	#define SENSOR_REG_ROI_H_VALID							(0x01)
	#define SENSOR_REG_ROI_V_VALID							(0x02)

// Register Interval
#define SENSOR_REG_ROI_INTERVAL								(0x08)

// Width Start Mask
#define SENSOR_REG_ROI_WIDTH_START_UPPER_MASK				(0x1f)
#define SENSOR_REG_ROI_WIDTH_START_LOWER_MASK				(0xff)

// Height Start Mask
#define SENSOR_REG_ROI_HEIGHT_START_UPPER_MASK				(0x0f)
#define SENSOR_REG_ROI_HEIGHT_START_LOWER_MASK				(0xff)

// Width Size Mask
#define SENSOR_REG_ROI_WIDTH_SIZE_UPPER_MASK				(0x1f)
#define SENSOR_REG_ROI_WIDTH_SIZE_LOWER_MASK				(0xff)

// Height Size Mask
#define SENSOR_REG_ROI_HEIGHT_SIZE_UPPER_MASK				(0x0f)
#define SENSOR_REG_ROI_HEIGHT_SIZE_LOWER_MASK				(0xff)

#define SENSOR_REG_ROI_WIDTH_VALID							(1)
#define SENSOR_REG_ROI_HEIGHT_VALID							(2)

// X1 Position
#define SENSOR_REG_ROI_H1_POS_ADRS1							(0x20)
#define SENSOR_REG_ROI_H1_POS_ADRS2							(0x21)
#define SENSOR_REG_ROI_V1_POS_ADRS1							(0x22)
#define SENSOR_REG_ROI_V1_POS_ADRS2							(0x23)

// X1 Width
#define SENSOR_REG_ROI_H1_WIDTH_ADRS1						(0x24)
#define SENSOR_REG_ROI_H1_WIDTH_ADRS2						(0x25)
#define SENSOR_REG_ROI_V1_WIDTH_ADRS1						(0x26)
#define SENSOR_REG_ROI_V1_WIDTH_ADRS2						(0x27)

// X2 Position
#define SENSOR_REG_ROI_H2_POS_ADRS1							(0x28)
#define SENSOR_REG_ROI_H2_POS_ADRS2							(0x29)
#define SENSOR_REG_ROI_V2_POS_ADRS1							(0x2a)
#define SENSOR_REG_ROI_V2_POS_ADRS2							(0x2b)

// X2 Width
#define SENSOR_REG_ROI_H2_WIDTH_ADRS1						(0x2c)
#define SENSOR_REG_ROI_H2_WIDTH_ADRS2						(0x2d)
#define SENSOR_REG_ROI_V2_WIDTH_ADRS1						(0x2e)
#define SENSOR_REG_ROI_V2_WIDTH_ADRS2						(0x2f)

// X3 Position
#define SENSOR_REG_ROI_H3_POS_ADRS1							(0x30)
#define SENSOR_REG_ROI_H3_POS_ADRS2							(0x31)
#define SENSOR_REG_ROI_V3_POS_ADRS1							(0x32)
#define SENSOR_REG_ROI_V3_POS_ADRS2							(0x33)

// X3 Width
#define SENSOR_REG_ROI_H3_WIDTH_ADRS1						(0x34)
#define SENSOR_REG_ROI_H3_WIDTH_ADRS2						(0x35)
#define SENSOR_REG_ROI_V3_WIDTH_ADRS1						(0x36)
#define SENSOR_REG_ROI_V3_WIDTH_ADRS2						(0x37)

// X4 Position
#define SENSOR_REG_ROI_H4_POS_ADRS1							(0x38)
#define SENSOR_REG_ROI_H4_POS_ADRS2							(0x39)
#define SENSOR_REG_ROI_V4_POS_ADRS1							(0x3a)
#define SENSOR_REG_ROI_V4_POS_ADRS2							(0x3b)

// X4 Width
#define SENSOR_REG_ROI_H4_WIDTH_ADRS1						(0x3c)
#define SENSOR_REG_ROI_H4_WIDTH_ADRS2						(0x3d)
#define SENSOR_REG_ROI_V4_WIDTH_ADRS1						(0x3e)
#define SENSOR_REG_ROI_V4_WIDTH_ADRS2						(0x3f)

// X5 Position
#define SENSOR_REG_ROI_H5_POS_ADRS1							(0x40)
#define SENSOR_REG_ROI_H5_POS_ADRS2							(0x41)
#define SENSOR_REG_ROI_V5_POS_ADRS1							(0x42)
#define SENSOR_REG_ROI_V5_POS_ADRS2							(0x43)

// X5 Width
#define SENSOR_REG_ROI_H5_WIDTH_ADRS1						(0x44)
#define SENSOR_REG_ROI_H5_WIDTH_ADRS2						(0x45)
#define SENSOR_REG_ROI_V5_WIDTH_ADRS1						(0x46)
#define SENSOR_REG_ROI_V5_WIDTH_ADRS2						(0x47)

// X6 Position
#define SENSOR_REG_ROI_H6_POS_ADRS1							(0x48)
#define SENSOR_REG_ROI_H6_POS_ADRS2							(0x49)
#define SENSOR_REG_ROI_V6_POS_ADRS1							(0x4a)
#define SENSOR_REG_ROI_V6_POS_ADRS2							(0x4b)

// X6 Width
#define SENSOR_REG_ROI_H6_WIDTH_ADRS1						(0x4c)
#define SENSOR_REG_ROI_H6_WIDTH_ADRS2						(0x4d)
#define SENSOR_REG_ROI_V6_WIDTH_ADRS1						(0x4e)
#define SENSOR_REG_ROI_V6_WIDTH_ADRS2						(0x4f)

// X7 Position
#define SENSOR_REG_ROI_H7_POS_ADRS1							(0x50)
#define SENSOR_REG_ROI_H7_POS_ADRS2							(0x51)
#define SENSOR_REG_ROI_V7_POS_ADRS1							(0x52)
#define SENSOR_REG_ROI_V7_POS_ADRS2							(0x53)

// X7 Width
#define SENSOR_REG_ROI_H7_WIDTH_ADRS1						(0x54)
#define SENSOR_REG_ROI_H7_WIDTH_ADRS2						(0x55)
#define SENSOR_REG_ROI_V7_WIDTH_ADRS1						(0x56)
#define SENSOR_REG_ROI_V7_WIDTH_ADRS2						(0x57)

// X8 Position
#define SENSOR_REG_ROI_H8_POS_ADRS1							(0x58)
#define SENSOR_REG_ROI_H8_POS_ADRS2							(0x59)
#define SENSOR_REG_ROI_V8_POS_ADRS1							(0x5a)
#define SENSOR_REG_ROI_V8_POS_ADRS2							(0x5b)

// X8 Width
#define SENSOR_REG_ROI_H8_WIDTH_ADRS1						(0x5c)
#define SENSOR_REG_ROI_H8_WIDTH_ADRS2						(0x5d)
#define SENSOR_REG_ROI_V8_WIDTH_ADRS1						(0x5e)
#define SENSOR_REG_ROI_V8_WIDTH_ADRS2						(0x5f)


//--------------------------------------------------------------------------------
// Chip ID = 4
//--------------------------------------------------------------------------------
#define SENSOR_REG_AD_ADRS									(0x00)
  #define SENSOR_REG_AD_RSV									(1<<2)
  #define SENSOR_REG_AD_8BIT								(2<<5)
  #define SENSOR_REG_AD_10BIT								(0<<5)
  #define SENSOR_REG_AD_12BIT								(1<<5)
  #define SENSOR_REG_AD_MASK								(3<<5)
  #define SENSOR_REG_H_BINNING								(1<<3)		// IMX992 / IMX993
  #define SENSOR_REG_V_BINNING								(1<<7)		// IMX992 / IMX993
#define SENSOR_REG_REVERSE_ADRS								(0x04)
  #define SENSOR_REG_REVERSE_V								(1<<0)
  #define SENSOR_REG_REVERSE_H								(1<<1)
#define SENSOR_REG_INCKSEL0_ADRS							(0x20)
#define SENSOR_REG_INCKSEL1_ADRS							(0x21)
#define SENSOR_REG_INCKSEL2_ADRS							(0x24)
#define SENSOR_REG_INCKSEL3_ADRS							(0x25)
#define SENSOR_REG_FREQ_SYNC_ADRS							(0x26)
#define SENSOR_REG_FIRST_TRG1_ADRS							(0x30)
  #define SENSOR_REG_FIRST_TRG1_FIRST_BIT					(1<<1)
#if defined (MODE_SENSOR_IMX990) || defined (MODE_SENSOR_IMX991)
  #define SENSOR_REG_FIRST_TRG1_DEFAULT						(0x11)
#elif defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
  #define SENSOR_REG_FIRST_TRG1_DEFAULT						(0x29)
#endif
#define SENSOR_REG_FIRST_TRG2_ADRS							(0x31)
#define SENSOR_REG_VINT_EN_ADRS								(0x32)
  #define SENSOR_REG_VINT_EN_DEFAULT						(0x01)
#define SENSOR_REG_SHS0_ADRS								(0x40)
#define SENSOR_REG_SHS1_ADRS								(0x41)
#define SENSOR_REG_SHS2_ADRS								(0x42)

#define SENSOR_REG_SMD1_ADRS								(0x30)
#define SENSOR_REG_SMD2_ADRS								(0x32)


//--------------------------------------------------------------------------------
// Chip ID = 6
//--------------------------------------------------------------------------------
#define SENSOR_REG_TRG_EN_ADRS								(0x00)
  #define SENSOR_REG_TRG_EN_BIT								(1<<0)
#define SENSOR_REG_ODBIT_ADRS								(0x30)
  #define SENSOR_REG_OD_8BIT								(2)
  #define SENSOR_REG_OD_10BIT								(0)
  #define SENSOR_REG_OD_12BIT								(1)
  #define SENSOR_REG_OD_BIT_MASK							(0x03)
#define SENSOR_REG_TOUT_ADRS								(0x35)
#define SENSOR_REG_TRG_TOUT_ADRS							(0x3a)
#define SENSOR_REG_SYNCSEL_ADRS								(0x3c)
#define SENSOR_REG_STBSLVS_ADRS								(0x44)
#define SENSOR_REG_OPORTSEL_ADRS							(0x45)
#define SENSOR_REG_PULSE1_ADRS								(0x78)
#define SENSOR_REG_PULSE1_UP1_ADRS							(0x79)
#define SENSOR_REG_PULSE1_UP2_ADRS							(0x7a)
#define SENSOR_REG_PULSE1_UP3_ADRS							(0x7b)
#define SENSOR_REG_PULSE1_DN1_ADRS							(0x7c)
#define SENSOR_REG_PULSE1_DN2_ADRS							(0x7d)
#define SENSOR_REG_PULSE1_DN3_ADRS							(0x7e)
#define SENSOR_REG_PULSE2_ADRS								(0x80)
#define SENSOR_REG_PULSE2_UP1_ADRS							(0x81)
#define SENSOR_REG_PULSE2_UP2_ADRS							(0x82)
#define SENSOR_REG_PULSE2_UP3_ADRS							(0x83)
#define SENSOR_REG_PULSE2_DN1_ADRS							(0x84)
#define SENSOR_REG_PULSE2_DN2_ADRS							(0x85)
#define SENSOR_REG_PULSE2_DN3_ADRS							(0x86)
#define SENSOR_REG_TMP_OUT1_ADRS							(0x90)
#define SENSOR_REG_TMP_OUT2_ADRS							(0x91)


//--------------------------------------------------------------------------------
// Chip ID = 7
//--------------------------------------------------------------------------------
#define SENSOR_REG_GAIN_RTS_ADRS							(0x02)
  #define SENSOR_REG_GAIN_RTS_FRAME_SAME					(0x00)
  #define SENSOR_REG_GAIN_RTS_FRAME_LATE					(0x01)
#define SENSOR_REG_CONVERSION_GAIN_ADRS						(0x10)
  #define SENSOR_REG_CONVERSION_GAIN_LHGC					(1<<0)
#define SENSOR_REG_GAIN_L_ADRS								(0x14)
  #define SENSOR_REG_GAIN_L_MASK							(0xff)
  #define SENSOR_REG_GAIN_L_SHIFT							(0)
#define SENSOR_REG_GAIN_H_ADRS								(0x15)
  #define SENSOR_REG_GAIN_H_MASK							(0x1)
  #define SENSOR_REG_GAIN_H_SHIFT							(8)

  #define SENSOR_REG_GAIN_DB_MIN							(0.0)
  #define SENSOR_REG_GAIN_DB_MAX							(42.1)
  #define SENSOR_REG_GAIN_X_MIN								(1.0)
  #define SENSOR_REG_GAIN_X_MAX								(125.91)

  #define SENSOR_REG_GAIN_DB_UNIT							(10)
  #define SENSOR_REG_GAIN_VALID_POINT						(10)

#define SENSOR_REG_FIX1_ADRS								(0x20)
#define SENSOR_REG_FIX2_ADRS								(0x40)

#define SENSOR_REG_PG_CTRL_ADRS								(0x50)
	#define SENSOR_REG_PG_CTRL_MODE							(1<<0)
	#define SENSOR_REG_PG_THRU_MODE							(1<<1)
	#define SENSOR_REG_PG_CLK_MODE							(1<<2)
#define SENSOR_REG_PG_MODE_ADRS								(0x51)
	#define SENSOR_REG_PG_MODE_MIN							(0)
	#define SENSOR_REG_PG_MODE_MAX							(8)
	#define SENSOR_REG_PG_MODE_MASK							(0x1f)
#define SENSOR_REG_PG_HPSTEP_ADRS							(0x58)
#define SENSOR_REG_PG_VPSTEP_ADRS							(0x59)
#define SENSOR_REG_PG_DATA1_L_ADRS							(0x5C)
#define SENSOR_REG_PG_DATA1_H_ADRS							(0x5D)
	#define SENSOR_REG_PG_DATA1_MASK						(0x1fff)
#define SENSOR_REG_PG_DATA2_L_ADRS							(0x5E)
#define SENSOR_REG_PG_DATA2_H_ADRS							(0x5F)
	#define SENSOR_REG_PG_DATA2_MASK						(0x1fff)
#define SENSOR_REG_PG_ADRS									(0x62)
#define SENSOR_REG_TMD_LUTCH_ADRS							(0x88)
	#define SENSOR_REG_TMD_LUTCH_DEFAULT					(0x33)
#define SENSOR_REG_BLACKLEVEL1_ADRS							(0xc0)
  #define SENSOR_REG_BLACKLEVEL1_MASK						(0xff)
#define SENSOR_REG_BLACKLEVEL2_ADRS							(0xc1)
  #define SENSOR_REG_BLACKLEVEL2_MASK						(0x0f)
  #define SENSOR_REG_BLACKLEVEL_8BIT						(0x0f)
  #define SENSOR_REG_BLACKLEVEL_10BIT						(0x3c)
  #define SENSOR_REG_BLACKLEVEL_12BIT						(0xf0)

#define SENSOR_REG_GRAD_COMP_MODE_ADRS						(0xc4)
	#define SENSOR_REG_GRAD_COMP_MODE_ENABLE				(1<<0)

#define SENSOR_REG_GRAD_COMP_POS_ADRS						(0xc5)
	#define SENSOR_REG_GRAD_COMP_POS_FIRST_MASK				(0x0f)
	#define SENSOR_REG_GRAD_COMP_POS_FIRST_SHIFT			(0)
	#define SENSOR_REG_GRAD_COMP_POS_FIRST_MIN				(0)
	#define SENSOR_REG_GRAD_COMP_POS_FIRST_MAX				(9)

	#define SENSOR_REG_GRAD_COMP_POS_SECOND_MASK			(0x0f)
	#define SENSOR_REG_GRAD_COMP_POS_SECOND_SHIFT			(4)
	#define SENSOR_REG_GRAD_COMP_POS_SECOND_MIN				(0)
	#define SENSOR_REG_GRAD_COMP_POS_SECOND_MAX				(9)
	#define SENSOR_REG_GRAD_COMP_POS_SECOND_POS1			(0x01)

#define SENSOR_REG_GRAD_COMP_GAIN_ADRS						(0xc6)
	#define SENSOR_REG_GRAD_COMP_GAIN_FIRST_MASK			(0x0f)
	#define SENSOR_REG_GRAD_COMP_GAIN_FIRST_SHIFT			(0)
	#define SENSOR_REG_GRAD_COMP_GAIN_FIRST_MIN				(0)
	#define SENSOR_REG_GRAD_COMP_GAIN_FIRST_MAX				(11)

	#define SENSOR_REG_GRAD_COMP_GAIN_SECOND_MASK			(0x0f)
	#define SENSOR_REG_GRAD_COMP_GAIN_SECOND_SHIFT			(4)
	#define SENSOR_REG_GRAD_COMP_GAIN_SECOND_MIN			(0)
	#define SENSOR_REG_GRAD_COMP_GAIN_SECOND_MAX			(11)


//--------------------------------------------------------------------------------
// Chip ID = 0x0a
//--------------------------------------------------------------------------------
#define SENSOR_REG_MODEL_LOW_ADRS							(0x15)
#define SENSOR_REG_MODEL_HIGH_ADRS							(0x16)
	#define SENSOR_REG_MODEL_HIGH_MASK						(0x03)


//--------------------------------------------------------------------------------
// Chip ID = 0x13
//--------------------------------------------------------------------------------
#define SENSOR_REG_CONVERSION_GAIN_SWITCHING1_ADRS			(0x07)
	#define SENSOR_REG_CONVERSION_GAIN_SWITCHING1_LCG		(0)
	#define SENSOR_REG_CONVERSION_GAIN_SWITCHING1_HCG		(1<<4)
#define SENSOR_REG_CONVERSION_GAIN_SWITCHING2_ADRS			(0x0b)
	#define SENSOR_REG_CONVERSION_GAIN_SWITCHING2_LCG		(1<<5)
	#define SENSOR_REG_CONVERSION_GAIN_SWITCHING2_HCG		(0)


//--------------------------------------------------------------------------------
// Chip ID = 0x1C
//--------------------------------------------------------------------------------
#define SENSOR_REG_POWER_ADRS								(0xD1)
	#define SENSOR_REG_VR_EN								(1<<3)
#define SENSOR_REG_POWER_OFF_ADRS							(0xD3)
	#define SENSOR_REG_DIS									(1<<1)

#endif // __SENSOR_VENDOR_S_H__

// eof
