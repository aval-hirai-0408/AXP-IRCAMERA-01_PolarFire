//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// gev.h.c - gigE Header Program
//**********************************************************************************

#ifndef GEV_H_
#define GEV_H_

//----------------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


//----------------------------------------------------------------------------------
// Debug Mode
//----------------------------------------------------------------------------------
#define DEBUG_MODE				// Defines this what if you desire output debug string. Comment out is release version
//#define DEBUG_MODE_MSG (0)	// Defines this what if you desire output debug string. Comment out is release version
#define DEBUG_MODE_MSG (1)		// Defines this what if you desire output debug string. Comment out is release version

#define INIT_NET_CONFIG (0)		// Not initiate the configuration
//#define INIT_NET_CONFIG (1)	// Initiates the all configuration of network as default IP,subnet,gateway and MAC address

#define INIT_NET_MAC_CONFIG (0)	// Not initiate the configuration
//#define INIT_NET_MAC_CONFIG (1) // Initiates the all configuration of network as default IP,subnet,gateway and MAC address


//----------------------------------------------------------------------------------
// CPU Mode
//----------------------------------------------------------------------------------
#define DEVICE_CPU_MAX					(2)

//----------------------------------------------------------------------------------
// typedef
//----------------------------------------------------------------------------------
typedef struct
{
	unsigned long adrs;		  // The register address
	unsigned int  size;		  // The register size in bytes
	unsigned int  value;	  // The value of the register as integer type
	unsigned int  status;	  // The result of operation to access the register
	char          *name;      // The register name
} GEV_CMD_TBL;


// typedef for command message between ARM0
#define CPU_CMD_TIMEOUT					(0x00040000) // 15us ~ 200us using 1.5 ns per 1 tick
#define CPU_CMD_SYNC_OFF				( 0 ) //コマンド非同期実行
#define CPU_CMD_SYNC_ON					( 1 ) //コマンド同期実行

typedef struct
{
	unsigned int  param0;  // フレームレート(us単位)
	unsigned int  param1;  // 露光時間(us単位)
	unsigned int  param2;  // 白/黒レベル目標値(0～14000)
	unsigned int  param3;  // 画素ビット数(8/10/12/14bit)
	unsigned int  param4;  // 保存先QSPI or EEPROM
} CPU_CMD;



//----------------------------------------------------------------------------------
// Device Information
//----------------------------------------------------------------------------------
#define GIGE_CONNECTION_STATE        "[CON]> "
#define GIGE_TAG_IN                  "[ IN]> "
#define GIGE_TAG_OUT                 "[OUT]> "
#define GIGE_BOOTSTRAP_OUT           "[BOT]> "
#define GIGE_EEPROM_OUT              "[ROM]> "
#define GIGE_MAC_REG_OUT             "[REG]> "


//----------------------------------------------------------------------------------
// Definitions for peripheral EPC_0
//----------------------------------------------------------------------------------
#define XPAR_M1_AXI_GEV_BASEADDR								(0x90010000)	// GevIP
#define XPAR_EPC_0_PRH0_BASEADDR								(XPAR_M1_AXI_GEV_BASEADDR)

#define GEV_MAC_ADDR											(XPAR_EPC_0_PRH0_BASEADDR) // GigE Core ~ PHY
#define GEV_MAC_MAX												(XPAR_EPC_0_PRH0_BASEADDR + 0x3FFF) //

#define GEV_TRANSMIT_BUFFER_ADDR								(XPAR_EPC_0_PRH0_BASEADDR + 0x4000) // GigE Core Global configuration and status sregister
#define GEV_TRANSMIT_BUFFER_2KB									(XPAR_EPC_0_PRH0_BASEADDR + 0x4800) // The real buffer size is 2KB
#define GEV_TRANSMIT_BUFFER_MAX									(XPAR_EPC_0_PRH0_BASEADDR + 0x7FFF)

#define GEV_RECEIVE_BUFFER_ADDR									(XPAR_EPC_0_PRH0_BASEADDR + 0x8000) // GigE Core Global configuration and status sregister
#define GEV_RECEIVE_BUFFER_2KB									(XPAR_EPC_0_PRH0_BASEADDR + 0x8800) // The real buffer size is 2KB
#define GEV_RECEIVE_BUFFER_MAX									(XPAR_EPC_0_PRH0_BASEADDR + 0xBFFF)

#define GEV_GCSR_ADDR											(XPAR_EPC_0_PRH0_BASEADDR + 0xC000) // GigE Core Global configuration and status sregister
#define GEV_GCSR_MAX											(XPAR_EPC_0_PRH0_BASEADDR + 0xFFFF)
#define GEV_ETHSIZE_ADDR										(XPAR_EPC_0_PRH0_BASEADDR + 0xC02C) // GigE Core Global configuration and status sregister
#define GEV_STM_IP_ADDR											(XPAR_EPC_0_PRH0_BASEADDR + 0xC080) // GigE Core Global configuration and status sregister
#define GEV_STM_PORT_ADDR										(XPAR_EPC_0_PRH0_BASEADDR + 0xC084) // GigE Core Global configuration and status sregister
#define GEV_STM_DELAY_ADDR										(XPAR_EPC_0_PRH0_BASEADDR + 0xC088) // GigE Core Global configuration and status sregister
#define GEV_STM_SIZE_ADDR										(XPAR_EPC_0_PRH0_BASEADDR + 0xC08C) // GigE Core Global configuration and status sregister
	#define GEV_STM_SIZE_PACKET_SIZE_MASK						(0xffff)
#define GEV_REV_MAC_HIGH_ADDR									(XPAR_EPC_0_PRH0_BASEADDR + 0xC090) // GigE Core Global configuration and status sregister
#define GEV_REV_MAC_LOW_ADDR									(XPAR_EPC_0_PRH0_BASEADDR + 0xC094) // GigE Core Global configuration and status sregister
#define GEV_REV_IP_ADDR											(XPAR_EPC_0_PRH0_BASEADDR + 0xC0A4) // GigE Core Global configuration and status sregister
#define GEV_REV_PORT_ADDR										(XPAR_EPC_0_PRH0_BASEADDR + 0xC0A8) // GigE Core Global configuration and status sregister
#define GEV_INT_MASK_ADDR										(XPAR_EPC_0_PRH0_BASEADDR + 0xC0AC) // GigE Core Global configuration and status sregister
#define GEV_INT_REQ_ADDR										(XPAR_EPC_0_PRH0_BASEADDR + 0xC0B0) // GigE Core Global configuration and status sregister
#define GEV_VER_ID_ADDR											(XPAR_EPC_0_PRH0_BASEADDR + 0xC0B4) // GigE Core Global configuration and status sregister
#define GEV_VER_VER_ADDR										(XPAR_EPC_0_PRH0_BASEADDR + 0xC0B8) // GigE Core Global configuration and status sregister
#define GEV_VER_DATE_ADDR										(XPAR_EPC_0_PRH0_BASEADDR + 0xC0BC) // GigE Core Global configuration and status sregister

#define GEV_STM_IDX												(XPAR_EPC_0_PRH0_BASEADDR + 0xC0C4)

#define GEV_FRAME_BUFFER_ADDR									(XPAR_EPC_0_PRH0_BASEADDR + 0xD000) // GigE Core Control Registers BUT v1.5 doesn't use the register
#define GEV_FRAME_BOTTOM_ADDR									(XPAR_EPC_0_PRH0_BASEADDR + 0xD004) // GigE Core Bottom address of the framebuffer in the memory
#define GEV_FRAME_TOP_ADDR										(XPAR_EPC_0_PRH0_BASEADDR + 0xD008) // GigE Core Bottom address of the framebuffer in the memory


#define MII_MARVELL_PHY_COPPER_CONTROL_PAGE						(0)
#define MII_MARVELL_PHY_COPPER_CONTROL_REG						(0)				// Copper SOftware Rest. Affect pages including 0,2,3,5 and 7
                                                                                // Writing a 1 to this bit causes the PHY state machines to be rest.
                                                                                // When the reset operation is done, this bit is cleared to 0 automatilcally.
                                                                                // D15: 1 = PHY reset
                                                                                // D15: 0 = Normal operation


#define MII_MARVELL_PHY_PAGE									(22)

#define MII_INDENTIFIER_1_PAGE									(0)
#define MII_INDENTIFIER_1_REG									(2)
#define MII_COPPER_CONTROL_REG									(0)				// Coper Control Register Page0, Register 0 (0_0.9)
#define MII_COPPER_CONTROL_RESTART_AUTO_NEGO_REG				(0)				// Auto-Negotiation automatically restarts after hardware or software
                                                                                // reset regardless of whether or not the restart bit (0_0.9) is set.
#define MII_COPPER_CONTROL_RESET_BIT							( 0x1 << 15)  	// Copper Software Reset. Affect pages 0, 2, 3, 5 and 7. 1 =PHY reset, 0 Normal operation.
#define MII_COPPER_CONTROL_SPEED_SEL_BIT						(   3 << 13)  	// Changes to this bit are disruptive to the normal operation.
                                                                                // 3 as 10bits = 1000 Mbps / 1 Gbps
#define MII_COPPER_CONTROL_RESET_PHY							(1)				// 1 = PHY reset
#define MII_COPPER_CONTROL_RESET_NORMAL							(0)				// 0 = Normal operation.
#define MII_COPPER_LINK_PARTNER_ABILITY_REG						(6)
#define MII_COPPER_SPEC_STATUS_REG								(17)			// Copper Specific Status Register 2 (Page 0, Register 17)
#define MII_COPPER_SPEC_STATUS_REG_10Mbps						(0x00)
#define MII_COPPER_SPEC_STATUS_REG_100Mbps						(0x01)
#define MII_COPPER_SPEC_STATUS_REG_1000Mbps						(0x10)

#define MII_88E1121_PHY_MSCR_PAGE								(2)				// Page 2
#define MII_88E1121_PHY_MSCR_REG								(21)			// MAC Specific Control Register 2 (Page 2, Register 21)
#define MII_88E1121_PHY_MSCR_RX_DELAY							(1<<5)			// RW; D5; RGMII Receive Timing Control
                                                                                //   Changes to these bits are disruptive to the normal operation; therefore,
                                                                                //   any changes to these registers must be followed by software reset to take effect.
                                                                                //   1 = Receive clock transition when data stable
                                                                                //   0 = Receive clock transition when data transitions
#define MII_88E1121_PHY_MSCR_TX_DELAY							(1<<4)			// RW; D4; RGMII Transmit Timing Control
                                                                                //   Changes to these bits are disruptive to the normal operation; therefore,
                                                                                //   any changes to these registers must be followed by software reset to take effect.
                                                                                //    1 = Transmit clock internally delayed
                                                                                //    0 = Transmit clock NOT internally delayed

#define MII_88E1121_PHY_MSCR_DELAY_MASK							(~(0x3 << 4))

#define MII_88E1318S_PHY_MSCR1_REG								(16)
#define MII_88E1318S_PHY_MSCR1_PAD_ODD							(1<<6)

#define MII_M1011_PHY_SCR										(0x10)
#define MII_M1011_PHY_SCR_AUTO_CROSS							(0x0060)

#define MII_88E1121_PHY_LED_CTRL								(16)
#define MII_88E1121_PHY_LED_PAGE								(3)
#define MII_88E1121_PHY_LED_DEF									(0x0030)


#ifndef _GEV_EEPROM_DEFAULT
#define _GEV_EEPROM_DEFAULT
//----------------------------------------------------------------------------------
// GigE Parameter Address
//----------------------------------------------------------------------------------
#define GIGE_EEPROM_MAC_ADRS									(0x00)
#define GIGE_EEPROM_IP_CONFIG_ADRS								(0x07)
#define GIGE_EEPROM_IP_ADRS										(0x14)
#define GIGE_EEPROM_SUBNET_ADRS									(0x24)
#define GIGE_EEPROM_GATEWAY_ADRS								(0x34)
#define GIGE_EEPROM_GVCP_PORT_ADRS								(0x3a)
#define GIGE_EEPROM_USER_DEF_NAME_ADRS							(0x40)
#define GIGE_EEPROM_GVSP_IP_ADRS								(0x5c)
#define GIGE_EEPROM_GVSP_UDP_ADRS								(0x62)
#define GIGE_EEPROM_SERIAL_NUM_ADRS								(0x64)
#define GIGE_EEPROM_CONFIG_DATA_ADRS							(0x80)
#define GIGE_EEPROM_XML_URL1_ADRS								(0x1c00)
#define GIGE_EEPROM_XML_URL2_ADRS								(0x1e00)

//----------------------------------------------------------------------------------
// GigE Parameter Size
//----------------------------------------------------------------------------------
#define GIGE_EEPROM_MAC_SIZE									(0x06)
#define GIGE_EEPROM_IP_CONFIG_SIZE								(0x01)
#define GIGE_EEPROM_IP_SIZE										(0x04)
#define GIGE_EEPROM_SUBNET_SIZE									(0x04)
#define GIGE_EEPROM_GATEWAY_SIZE								(0x04)
#define GIGE_EEPROM_GVCP_PORT_SIZE								(0x02)
#define GIGE_EEPROM_USER_DEF_NAME_SIZE							(0x10)
#define GIGE_EEPROM_GVSP_IP_SIZE								(0x04)
#define GIGE_EEPROM_GVSP_UDP_SIZE								(0x04)
#define GIGE_EEPROM_SERIAL_NUM_SIZE								(0x10)
#define GIGE_EEPROM_CONFIG_DATA_SIZE							(0xF80)
#define GIGE_EEPROM_XML_URL_SIZE								(0x200)

//----------------------------------------------------------------------------------
// GigE EEPROM offset address
//----------------------------------------------------------------------------------
#define GIGE_EEPROM_MAC_HIGH_REG								(0x0000) // to 0x0005 as 4 byte
#define GIGE_EEPROM_MAC_LOW_REG									(0x0004) // to 0x0006 as 2 byte
#define GIGE_EEPROM_CURRENT_IP_REG								(0x0007) // to bit8 as 6 byte
#define GIGE_EEPROM_PERSISTENT_IP_REG							(0x0014) // to 0x0017 as 4 byte
#define GIGE_EEPROM_PERSISTENT_SUB_MASK_REG						(0x0024) // to 0x0027 as 4 byte
#define GIGE_EEPROM_PERSISTENT_DEF_GATEWAY_REG					(0x0034) // to 0x0037 as 4 byte
#define GIGE_EEPROM_GVCP_PORT_REG								(0x003A) // to 0x003B as 4 byte
#define GIGE_EEPROM_USER_ID_REG									(0x0040) // to 0x004F as 16 byte
#define GIGE_EEPROM_USER_ID_SIZE								(    16) // in 16 bytes
#define GIGE_EEPROM_SERIAL_NUMBER_REG							(0x0064) // to 0x0073 as 16 byte
#define GIGE_EEPROM_VERSION_SIZE								(    16) // in 16 bytes

#define GIGE_EEPROM_CUST_SERIAL_ADRS							(0x1000) // in 16 bytes
#define GIGE_EEPROM_CUST_FIRMWARE_VER_ADRS						(0x1010) // in 32 bytes
#define GIGE_EEPROM_CUST_FIRMWARE_VER_SIZE						(    32) // in 32 bytes
#define GIGE_EEPROM_CUST_UPDATE_TIMESTAMP_ADRS					(0x1030) // in 16 bytes
#define GIGE_EEPROM_CUST_UPDATE_TIMESTAMP_SIZE					(    32) // in 32 bytes
#define GIGE_EEPROM_CUST_VENDOR_SIZE							(    32) // in 32 bytes
#define GIGE_EEPROM_CUST_MODEL_SIZE								(    32) // in 32 bytes
#define GIGE_EEPROM_CUST_MANUFACTURER_SIZE						(    48) // in 48 bytes

// GigE : the first XML URL register offset
#define GIGE_EEPROM_XML_URL1_REG								(0x1C00) // to 0x1DFF as 512 byte
// GigE : the second XML URL register offset
#define GIGE_EEPROM_XML_URL2_REG								(0x1E00) // to 0x1FFF as 512 byte


#define GIGE_EEPROM_MAC_DATA									(0x00000080B320C000LL)		// Mac Address as for Tokyo Semitsu
//#define GIGE_EEPROM_MAC_DATA									(0x00000080B3341234LL)		// Mac Address as for Development dummy
#define GIGE_EEPROM_MAC_ADRS_DATA								("0080B37001B6")       		// Mac Address
//#define GIGE_EEPROM_MAC_ADRS_DATA								("80B3341234")       		// Mac Address
#define GIGE_EEPROM_IP_CONFIG_DATA								(0x06)						// Current IP Configuration as LLA and DHCP is on and persistent IP is off
#define GIGE_EEPROM_IP_CONFIG_LLA								(0x01)						// Current IP Configuration as LLA and DHCP is on and persistent IP is off
#define GIGE_EEPROM_IP_CONFIG_DHCP								(0x02)						// Current IP Configuration as LLA and DHCP is on and persistent IP is off
#define GIGE_EEPROM_IP_CONFIG_PERSISTENT_IP						(0x04)						// Current IP Configuration as LLA and DHCP is on and persistent IP is off
#define GIGE_EEPROM_IP_DATA										(0xA9FE0001)				// IP Address       : 169.254.000.001
#define GIGE_EEPROM_SUBNET_DATA									(0xFFFF0000)				// Sub-net Mask     : 255.255.000.000
#define GIGE_EEPROM_GATEWAY_DATA								(0x00000000)				// Default Gateway  : 000.000.000.000
//#define GIGE_EEPROM_GATEWAY_DATA								(0xC0A800C8)				// Default Gateway  : 192.168.0.200
#define GIGE_EEPROM_GVCP_PORT_DATA								(3956)						// GVCP Port Number
#define GIGE_EEPROM_USER_DEF_NAME_DATA							"default1\0"				// User defined name
#define GIGE_EEPROM_GVSP_IP_DATA								(0x00000000)				// GVCP destination IP address
#define GIGE_EEPROM_GVSP_UDP_DATA								(3956)						// GVCP UDP Port
#define GIGE_EEPROM_SERIAL_NUM_DATA								"5130000000\0"			    // Serial Number

// Formal XML URL
#define GIGE_EEPROM_XML_URL1									"local:ABA-003IR-GE_V0R1.xml;61000000;1F120?SchemaVersion=1.0.0\0"		// First XML File
#define GIGE_EEPROM_XML_URL2									"local:ABA-003IR-GE_Rev01.zip;61000000;332C?SchemaVersion=1.0.0\0\0"    // Second XML File

// GigE parameter Area Size
#define GIGE_EEPROM_PARAM_AREA_SIZE								(0x2000)
// Buffer Size
#define GIGE_EEPROM_PARAM_BUFF_SIZE								(0x200)
// MAC Address Compare Data
#define GIGE_EEPROM_MAC_BASE_DATA								(0x0080)
#endif // _GEV_EEPROM_DEFAULT


//====================================================================================
//
// For vendor specific addresses
// Group: ImageFormatControl
//
//====================================================================================
#define SensorWidth												( 0x0000A008 ) // RW; 4 bytes;
#define SensorHeight											( 0x0000A00C ) // RW; 4 bytes;
#define PixelFormat_MAKE_BIT(mode)								( (mode==PIXEL_8BIT)?8:(mode==PIXEL_10BIT)?10:(mode==PIXEL_12BIT)?12:(mode==PIXEL_14BIT)?14:8 )
#define LinePitch												( 0x0000A014 ) // RW; 4 bytes; video_padding;Total number of bytes between 2 successive lines. This feature is used to facilitate alignment of image data.
#define PayloadSize												( 0x0000A024 ) // RW; 4 bytes;
#define VideoChunkCtrl											( 0x0000A100 ) // RW; 4 bytes;
#define VideoChunkEnable										( 0x0000A104 ) // RW; 4 bytes;


//====================================================================================
//
// For GEV specific addresses
// Group: DeviceInformation
//
//====================================================================================
#define DeviceVersion											( 0x00000088 ) // RO; 32 bytes
#define DeviceUserID											( 0x000000E8 ) // RW; 16 bytes

// Temperature
#define DeviceTemperaturePhy									( 0x0000A1F4 ) // RO; 4 bytes
#define DeviceTemperatureAbnormalStatus							( 0x0000A1F8 ) // RO; 4 bytes
#define DeviceTemperatureAbnormalCount							( 0x0000A1FC ) // RO; 4 bytes
#define DeviceTemperatureSelector								( 0x0000A200 ) // RO; 4 bytes
#define DeviceTemperatureSelector_Sensor						(   0 ) // InGaAs
#define DeviceTemperatureSelector_Case							(   1 ) // InGaAs
#define DeviceTemperatureSelector_FPGA							(   2 ) // InGaAs
#define DeviceTemperaturePeltierMode							( 0x0000A20C)  // RW; 4 bytes
#define DeviceTemperature_Sensor_Max							( 0x0000A210)  // RO; 4 bytes
#define DeviceTemperature_Sensor_Min							( 0x0000A214)  // RO; 4 bytes
#define DeviceTemperatureAlarmMin_Sensor						( 0x0000A224)  // RW; 4 bytes
#define DeviceTemperatureAlarmMin_Housing						( 0x0000A22C)  // RW; 4 bytes
#define DeviceTemperatureTarget_Min								( 0x0000A230)  // RW; 4 bytes
#define DeviceTemperatureTarget_Max								( 0x0000A234)  // RW; 4 bytes
#define DevicePeltierPowerLevel									( 0x0000A23C)  // RO; 4 bytes

#define DeviceBoot												( 0x0000A240)  // WO; 4 bytes
#define DeviceReset 											( 0x0000A244)  // RW; 4 bytes
#define DeviceFactory											( 0x0000A248)  // WO; 4 bytes
#define DeviceDipsw												( 0x0000A24C)  // RO; 4 bytes

#define DeviceIndicatorMode  									( 0x0000A250)
#define DeviceIndicatorMode_Inactive							(          0)
#define DeviceIndicatorMode_Active								(          1)
#define DeviceIndicatorMode_ErrorStatus							(          2)

#define DeviceScanType											( 0x0000A280 ) // RO; 4 bytes
#define   DeviceScanType_Areascan								( 0 ) // RO; 4 bytes
#define   DeviceScanType_Linescan								( 1 ) // RO; 4 bytes

#define DeviceTLType											( 0x0000A284 ) // RO; 4 bytes
#define   DeviceTLType_GigEVision								( 0 ) // RO; 4 bytes
#define   DeviceTLType_CameraLink								( 1 ) // RO; 4 bytes
#define   DeviceTLType_CameraLinkHS								( 2 ) // RO; 4 bytes
#define   DeviceTLType_CoaXPress								( 3 ) // RO; 4 bytes
#define   DeviceTLType_USB3Vision								( 4 ) // RO; 4 bytes
#define   DeviceTLType_Custom									( 5 ) // RO; 4 bytes

#define DeviceTapGeometry										( 0x0000A288 ) // RO; 4 bytes
#define DeviceTapGeometry_Geometry_1X_1Y						( 0x0 )

#define DeviceMainVolt											( 0x0000A2F0 ) // RO; 4 bytes;
#define DeviceRateMode											( 0x0000A2F4 ) // RW; 4 bytes;


//====================================================================================
//
// For Device specific addresses
// Group: DeviceInformation
//
//====================================================================================

#define ExposureMode_Off										(       0x00 ) // RW; 4 bytes;
#define ExposureMode_Timed										(       0x01 ) // RW; 4 bytes;
#define ExposureMode_TriggerWidth								(       0x02 ) // RW; 4 bytes;
#define ExposureMode_TriggerControlled							(       0x03 ) // RW; 4 bytes;
#define ExposureAuto											( 0x0000A304 ) // RW; 4 bytes; Exp. Default is 356161(44,549,440 ms). min is 25(1us)
#define ExposureTimeAuto_Off									( 0x00000000 ) // RW; 4 bytes; Exp. Default is 356161(44,549,440 ms). min is 25(1us)
#define ExposureTimeAuto_Once									( 0x00000001 ) // RW; 4 bytes; Exp. Default is 356161(44,549,440 ms). min is 25(1us)
#define ExposureTimeMax											( 0x0000A308 ) // RW; 4 bytes; Exp. Default is 356161(44,549,440 ms). min is 25(1us)
#define ExposureTimeMin											( 0x0000A30C ) // RW; 4 bytes; Exp. Default is 356161(44,549,440 ms). min is 25(1us)

#define DiagnosticSelector										( 0x0000A350)
#define    DiagnosticSelector_Off								( 0)
#define    DiagnosticSelector_Test1								( 1)
#define Diagnostic												( 0x0000A354)
#define DiagnosticResult										( 0x0000A358)
#define DiagnosticResultNumber									( 0x0000A35C)

#define DeviceBuiltInTest										( 0x0000A370)  // RW; 4 bytes
#define DeviceBuiltInTestStatus									( 0x0000A374)  // RW; 4 bytes
#define    DeviceBuiltInTest_Done								( 0x00000000)

#define TriggerDelayMin											( 0x0000A390 ) // RW; 4 bytes; Exp. Default is 356161(44,549,440 ms). min is 25(1us)
#define TriggerDelayMax											( 0x0000A394 ) // RW; 4 bytes; Exp. Default is 356161(44,549,440 ms). min is 25(1us)
#define SensorReadOutTime										( 0x0000A398 ) // RW; 4 bytes;

#define DeviceVendorNameOnEEPROM								( 0x0000A800 ) // RO; 32 bytes on Bootstrap register. You ,namely, couldn't get/set it.
#define DeviceModelNameOnEEPROM									( 0x0000A820 ) // RO; 32 bytes on Bootstrap register. You ,namely, couldn't get/set it.
#define DeviceManufacturerInfoOnEEPROM							( 0x0000A840 ) // RO; 48 bytes on Bootstrap register. You ,namely, couldn't get/set it.

#define DeviceFirmwareVersion									( 0x0000A880 ) // RO; 32 bytes
#define DeviceFirmwareTimestamp									( 0x0000A900 ) // RO; 32 bytes
#define DeviceFirmwareTimestamp_Max								(         32 ) // RO; 32 bytes

#define DeviceVendorNameWrite									( 0x0000AA00 ) // RW; 32 bytes
#define DeviceModelNameWrite									( 0x0000AA20 ) // RW; 32 bytes
#define DeviceManufacturerInfoWrite								( 0x0000AA40 ) // RW; 48 bytes
#define DeviceVendoroWriteCmd									( 0x0000AA70 ) // RW;  4 bytes
#define DeviceModelWriteCmd										( 0x0000AA74 ) // RW;  4 bytes
#define DeviceManufacturerInfoWriteCmd							( 0x0000AA78 ) // RW;  4 bytes


//====================================================================================
//
// Group: AcquisitionControl
//
//====================================================================================
#define AcquisitionPreset										( 0xB004 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Off								(  0 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Default							(  1 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Mode1								(  2 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Mode2								(  3 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Mode3								(  4 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Mode4								(  5 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Mode5								(  6 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Mode6								(  7 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Mode7								(  8 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Mode8								(  9 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Mode9								( 10 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define     AcquisitionPreset_Mode10							( 11 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define AcquisitionFrameRate									( 0xA034 ) // RW; 4 bytes; frame rate = (float)VIDEO_CLK/((video_width+video_gap_x)*(video_height+video_gap_y));
#define AcquisitionFrameRateMax									( 0xB010 ) // RO; 4 bytes; frame rate max
#define AcquisitionFrameRateMin									( 0xB014 ) // RO; 4 bytes; frame rate min
#define AcquisitionFrameRateRawMax								( 0xB018 ) // RO; 4 bytes; frame rate max
#define AcquisitionFrameRateRawMin								( 0xB01C ) // RO; 4 bytes; frame rate min
#define AcquisitionFrameCountMin								( 0xB020 ) // RW; 4 bytes; 0xFFFFFFFF is infinity.
#define AcquisitionFrameCountMax								( 0xB024 ) // RW; 4 bytes; 0xFFFFFFFF is infinity.

#define AcquisitionTrgHighCount									( 0xB030 ) // RW; 4 bytes;
#define AcquisitionTrgLowCount									( 0xB034 ) // RW; 4 bytes;
#define AcquisitionTrgCountMode									( 0xB038 ) // RW; 4 bytes;
#define AcquisitionTrgSignalCount								( 0xB03C ) // RW; 4 bytes;
#define AcquisitionTrgImageCount								( 0xB040 ) // RW; 4 bytes;


//====================================================================================
//
// Group: UserSetControl
//
//====================================================================================
#define UserSetSelector											( 0x0000C000 ) // RW; 4 bytes;
#define UserSetLoad												( 0x0000C004 ) // RW; 4 bytes;
#define UserSetSave												( 0x0000C008 ) // RW; 4 bytes;
#define UserSetDefault											( 0x0000C00C ) // RW; 4 bytes;
#define UserSet_Default											( 0 ) // RW; 4 bytes;  The default factory settings
#define UserSetFactory											( 0x0000C010 ) // RW; 4 bytes;


//====================================================================================
//
// Group: LUTControl
//
//====================================================================================
#define LUTSelector												( 0x0000E000 ) // RW; 4 bytes;
#define LUTIndex												( 0x0000E004 ) // RW; 4 bytes;
#define LUTIndex_Max											( 0x0000E008 ) // RW; 4 bytes;
#define LUTFormat												( 0x0000E00C ) // RW; 4 bytes;
#define LUTThreshold											( 0x0000E010 ) // RW; 4 bytes;
#define LUTGamma												( 0x0000E014 ) // RW; 4 bytes;
#define LUTBaseAdrs												( 0x0000E018 ) // RW; 4 bytes;
#define LUTValue												( 0x0000E028 ) // RW; 4 bytes;


//====================================================================================
//
// Group: AnalogControl
//
//====================================================================================
#define Gain_MASK												( 0x00007FFF ) // RW; D0 to D13 as 14 bit;
#define GainxFloat												( 0x0000E800 ) // RW; 4 bytes;
#define SensorGainX												( 0x0000E810 ) // RW; 4 bytes;
#define SensorConversionGain									( 0x0000E818 ) // RW; 4 bytes;
#define SensorGradComp8BitConvert								( 0x0000E838 ) // RW; 4 bytes;


//====================================================================================
//
// Group: FlatFiledCorrectionControl
//
//====================================================================================
#define FlatFieldCorrectionSelector								( 0x0000F000 )   // RW; 4 bytes;
#define FlatFieldCorrectionSetLoad								( 0x0000F004 )   // RW; 4 bytes;
#define FlatFieldCorrectionSetSave								( 0x0000F008 )   // RW; 4 bytes;

#define FlatFieldCorrectionDarkAdjustment						( 0x0000F010 )   // RW; 4 bytes;
#define FlatFieldCorrectionBrightAdjustment						( 0x0000F014 )   // RW; 4 bytes;
#define FlatFieldCorrectionShadinLinegAdjustment				( 0x0000F01C )   // RW; 4 bytes;

#define FlatFieldCorrectionFactory								( 0x0000F020 )   // RW; 4 bytes;
#define FlatFieldCorrectionBrightTarget 						( 0x0000F024 )   // RW; 4 bytes;
#define FlatFieldCorrectionBrightMode 							( 0x0000F028 )   // RW; 4 bytes;
#define FlatFieldCorrectionWhiteGainX 							( 0x0000F02C )   // RW; 4 bytes;

#define FlatFieldCorrectionStatus								( 0x0000F030 )   // RW; 4 bytes;
#define FlatFieldCorrectionAdjustmentSelector					( 0x0000F034 )   // RW; 4 bytes;
#define FlatFieldCorrectionDarkTemperature						( 0x0000F038 )   // RW; 4 bytes;
#define FlatFieldCorrectionBrightTemperature					( 0x0000F03C )   // RW; 4 bytes;

#define FlatFieldCorrectionCorrectionSelector					( 0x0000F044 )   // RW; 4 bytes;
#define FlatFieldCorrectionSetCorrectionMode					( 0x0000F048 )   // RW; 4 bytes;
#define FlatFieldCorrectionAdjustUpdate							( 0x0000F04C )   // RW; 4 bytes;

#define FlatFieldCorrectionAdjustDarkTarget						( 0x0000F050 )   // RW; 4 bytes;
#define FlatFieldCorrectionAdjustBrightTarget					( 0x0000F058 )   // RW; 4 bytes;

#define FlatFieldCorrectionAdjustDarkRate						( 0x0000F060 )   // RW; 4 bytes;
#define FlatFieldCorrectionAdjustBrightRate						( 0x0000F068 )   // RW; 4 bytes;

#define FlatFieldCorrectionAdjustDarkExposure					( 0x0000F070 )   // RW; 4 bytes;
#define FlatFieldCorrectionAdjustBrightExposure					( 0x0000F078 )   // RW; 4 bytes;

#define FlatFieldCorrectionAdjustDarkTemperature				( 0x0000F080 )   // RW; 4 bytes;
#define FlatFieldCorrectionAdjustBrightTemperature				( 0x0000F088 )   // RW; 4 bytes;

#define FlatFieldCorrectionLoadAdmin							( 0x0000F090 )   // RW; 4 bytes;
#define FlatFieldCorrectionSaveAdmin							( 0x0000F094 )   // RW; 4 bytes;
#define FlatFieldCorrectionBaseAdrs								( 0x0000F098 )   // RW; 4 bytes;

#define FlatFieldCorrectionXGrid								( 0x0000F0A0 )   // RW; 4 bytes;
#define FlatFieldCorrectionYGrid								( 0x0000F0A4 )   // RW; 4 bytes;

#define FlatFieldCorrectionAdjustDarkGainX						( 0x0000F0B0 )   // RW; 4 bytes;
#define FlatFieldCorrectionAdjustBrightGainX					( 0x0000F0B8 )   // RW; 4 bytes;

#define FlatFieldCorrectionAdjustDarkBit						( 0x0000F0C0 )   // RW; 4 bytes;
#define FlatFieldCorrectionAdjustBrightBit						( 0x0000F0C8 )   // RW; 4 bytes;


//====================================================================================
//
// Group: DefectivePixelCorrectionControl
//
//====================================================================================
#define DefectivePixelCorrectionSelector						( 0x0000F100 ) // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustmentState					( 0x0000F104 ) // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustment						( 0x0000F108 ) // RW; 4 bytes;
#define DefectivePixelCorrectionCount							( 0x0000F10C ) // RW; 4 bytes;
#define   DefectivePixelCorrectionAdjustment_Off				( 0 ) // RW; 4 bytes;
#define   DefectivePixelCorrectionAdjustment_1					( 1 ) // RW; 4 bytes;
#define   DefectivePixelCorrectionAdjustment_2					( 2 ) // RW; 4 bytes;
#define   DefectivePixelCorrectionAdjustment_3					( 3 ) // RW; 4 bytes;
#define   DefectivePixelCorrectionAdjustment_4					( 4 ) // RW; 4 bytes;
#define DefectivePixelCorrectionDetection						( 0x0000F110 ) // RW; 4 bytes;
#define DefectivePixelCorrectionSave							( 0x0000F114 ) // RW; 4 bytes;
#define DefectivePixelCorrectionFactory							( 0x0000F118 ) // RW; 4 bytes;
#define DefectivePixelCorrectionSaveAdmin						( 0x0000F11c ) // RW; 4 bytes;
#define DefectivePixelCorrectionDetectionAbort					( 0x0000F120 ) // RW; 4 bytes;
#define DefectivePixelIndex										( 0x0000F124 ) // RW; 4 bytes; 29th August 2017
#define DefectivePixelCoordinateX								( 0x0000F128 ) // RW; 4 bytes; 29th August 2017
#define DefectivePixelCoordinateY								( 0x0000F12C ) // RW; 4 bytes; 29th August 2017
#define DefectivePixelApply										( 0x0000F130 ) // RW; 4 bytes; 29th August 2017
#define DefectivePixelRemove									( 0x0000F134 ) // RW; 4 bytes; 29th August 2017
#define DefectivePixelReset										( 0x0000F138 ) // RW; 4 bytes; 29th August 2017
#define DefectivePixelCorrectionMax								( 0x0000F13C ) // RW; 4 bytes; 29th August 2017
#define DefectivePixelModify									( 0x0000F140 ) // RW; 4 bytes;  8th September 2017
#define   DefectivePixelModify_Off								( 0 ) // RW; 4 bytes;
#define   DefectivePixelModify_On								( 1 ) // RW; 4 bytes;
#define DefectivePixelCorrectionMode							( 0x0000F144 )   // RW; 4 bytes;
#define DefectivePixelCorrectionLoad							( 0x0000F148 )   // RW; 4 bytes;
#define DefectivePixelCorrectionLoadAdmin						( 0x0000F14c )   // RW; 4 bytes;
#define DefectivePixelCorrection								( 0x40210000 )   // RW; 4 bytes;


//====================================================================================
//
// For Device specific addresses
// Group: SpectrumInformation
//
//====================================================================================
#define SpectrumBandCount										( 0x0000F150 ) // RW; 4 bytes;
#define SpectrumBandIndex										( 0x0000F154 ) // RW; 4 bytes;
#define SpectrumBandValue										( 0x0000F158 ) // RW; 4 bytes;
#define SpectrumBandWidth										( 0x0000F15C ) // RW; 4 bytes;


//====================================================================================
//
// Group: DefectivePixelCorrectionControl
//
//====================================================================================
#define DefectivePixelCorrectionAdjustFfcIndex					( 0x0000F160 )   // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustBrightIndex				( 0x0000F164 )   // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustRate						( 0x0000F168 )   // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustExposure					( 0x0000F16c )   // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustTemperature				( 0x0000F170 )   // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustFfcNumber					( 0x0000F174 )   // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustStandardDeviation			( 0x0000F178 )   // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustNonuniformity				( 0x0000F17c )   // RW; 4 bytes;

#define DefectivePixelCorrectionPixelStateCount					( 0x0000F0F0 )   // RW; 4 bytes;
#define DefectivePixelCorrectionPixelCount						( 0x0000F0F4 )   // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustBaseAdrs					( 0x0000F0F8 )   // RW; 4 bytes;
#define DefectivePixelCorrectionAdjustUpdate					( 0x0000F0FC )   // RW; 4 bytes;

#define DebugTime												( 0x0000F1e8 ) // RO; 4 bytes;


//====================================================================================
//
// Group: EventControl
// see also :  GEV_EVENT_ in gige.h
//====================================================================================
#define EventSelector											( 0x0000F200 ) // RW; 4 bytes;
#define EventNotification										( 0x0000F204 ) // RW; 4 bytes;
#define EventNotification_Max									( GEV_EVENT_MAX) // RW; 4 bytes;

#define Event_AcquisitionTrigger								(          0 )
#define Event_AcquisitionStart									(          1 )
#define Event_AcquisitionEnd									(          2 )
#define Event_AcquisitionTransferStart							(          3 )
#define Event_AcquisitionTransferEnd							(          4 )
#define Event_AcquisitionError									(          5 )
#define Event_FrameTrigger										(          6 )
#define Event_FrameStart										(          7 )
#define Event_FrameEnd											(          8 )
#define Event_FrameBurstStart									(          9 )
#define Event_FrameBurstEnd										(         10 )

#define Event_FrameTransferStart								(         11 )
#define Event_FrameTransferEnd									(         12 )
#define Event_ExposureStart										(         12 )
#define Event_ExposureEnd										(         14 )
#define Event_ActionLate										(         47 )
#define Event_LinkSpeedChange									(         48 )
#define Event_Error												(         49 )
#define Event_PrimaryApplicationSwitch							(         50 )

#define EventExposureEnd										( 0x0000F208 ) // RW; 4 bytes;
#define EventExposureEndTimestamp								( 0x0000F20C ) // RW; 4 bytes;
#define EventExposureEndFrameID									( 0x0000F210 ) // RW; 4 bytes;


//====================================================================================
//
// For Device specific addresses
// Group: SpectrumInformation
//
//====================================================================================
#define SpectrumBandWave										( 0x0000F300 ) // RW; 4 bytes;
#define SpectrumBandWaveToIndex									( 0x0000F304 ) // RW; 4 bytes;
#define SpectrumBandAllValue									( 0x0000F308 ) // RW; 4 bytes;
#define SpectrumBandValueRange									( 0x0000F30C ) // RW; 4 bytes;


//====================================================================================
//
// For Device specific addresses
// Group: -
//
//====================================================================================
#define DeviceSensorClockDetect									( 0x0000F330 ) // RW; 4 bytes;
#define DeviceSensorPower										( 0x0000F334 ) // RW; 4 bytes;
#define DeviceDrrsCommand										( 0x0000F338 ) // RW; 4 bytes;
#define DeviceDrrsMode											( 0x0000F33C ) // RW; 4 bytes;
#define SensorBlackPixel										( 0x0000F340 ) // RW; 4 bytes;
#define HighSpeedMode											( 0x0000F344 ) // RW; 4 bytes;
#define HighSpeedModeCmd										( 0x0000F348 ) // RW; 4 bytes;
#define HighSpeedModeLineCount									( 0x0000F34C ) // RW; 4 bytes;
#define HighSpeedModeSelector									( 0x0000F350 ) // RW; 4 bytes;
#define DeviceAcesFlag											( 0x0000F354 ) // RW; 4 bytes;
#define DeviceDrrsStatus										( 0x0000F358 ) // RW; 4 bytes;
#define GevLinkSpeedConfig										( 0x0000F35C ) // RW; 4 bytes;


//====================================================================================
//
// Group: FlatFieldCorrectionControl
//
//====================================================================================
#define FlatFieldCorrectionOffset								( 0x0000F3C4 ) // RW; 4 bytes;
#define FlatFieldCorrectionGain									( 0x0000F3C8 ) // RW; 4 bytes;
#define FlatFieldCorrectionShadingBrightAdjustment				( 0x0000F3DC ) // RW; 4 bytes;


//====================================================================================
//
// Group: DeviceControl
//
//====================================================================================
#define DeviceSensorID											( 0x0000F400 ) // RO; 32byte
#define DeviceBoardID											( 0x0000F420 ) // RO; 32byte
#define DevicePrimaryURL										( 0x0000F600 ) // RO; 256byte
#define DeviceSecondaryURL										( 0x0000F800 ) // RO; 256byte


//====================================================================================
//
// Group: CounterAndTimerControl
//
//====================================================================================
#define CounterDiagnosticSelector        						( 0x0000D000 ) // RW; 4 bytes;
#define CounterDiagnosticSelector_ARM0_MASK						( 0x00000040 ) // RW; 4 bytes;
#define CounterDiagnosticSelector_ARM1_MASK 					( 0x00000050 ) // RW; 4 bytes;
#define CounterDiagnosticValue									( 0x0000D004 ) // RW; 4 bytes;

//@@@1#define ARM1_DIAG_VALUE_START_ADRS								( FIRM_DATA_ARM1_GIGE_INFO + 0x00)  // 初期化エラー情報格納アドレス
//@@@1#define ARM1_DIAG_VALUE_END_ADRS       							( FIRM_DATA_ARM1_GIGE_INFO + 0xFF)  // RW; 4 bytes;
//@@@1#define ARM1_DIAG_VALUE_ADRS_OFFSET(offset)	  					( FIRM_DATA_ARM1_GIGE_INFO + offset) // 初期化エラー値のアドレスをオフセットから作成
//@@@1#define ARM1_DIAG_INC_VALUE(offset) 							OUT32( ARM1_DIAG_VALUE_ADRS_OFFSET(offset), ( IN32( ARM1_DIAG_VALUE_ADRS_OFFSET(offset)) + 1) );

#define CounterDiagnosticValue_NetworkMAC						(0x00) // RW; 4 bytes;
#define CounterDiagnosticValue_NetworkInit						(0x04) // RW; 4 bytes;
#define CounterDiagnosticValue_NetworkLinkConnection			(0x08) // RW; 4 bytes;
#define CounterDiagnosticValue_NetworkLinkDisconnection			(0x0C) // RW; 4 bytes;
#define CounterDiagnosticValue_NetworkCommandReadError  		(0x10) // RW; 4 bytes;
#define CounterDiagnosticValue_NetworkCommandWriteError			(0x14) // RW; 4 bytes;

#define CounterDiagnosticValue_UserSetError						(0x18) // RW; 4 bytes;
#define CounterDiagnosticValue_LUTError							(0x1C) // RW; 4 bytes;
#define CounterDiagnosticValue_FlatFieldCorrectionError			(0x20) // RW; 4 bytes;
#define CounterDiagnosticValue_DefectivePixelCorrectionError	(0x24) // RW; 4 bytes;

#define CounterDiagnosticValue_FramebufferBlockDrop		 		(0x30) // RW; 4 bytes;
#define CounterDiagnosticValue_FramebufferBlockSkipped			(0x34) // RW; 4 bytes;
#define CounterDiagnosticValue_FramebufferFIFODrop				(0x38) // RW; 4 bytes;
#define CounterDiagnosticValue_FramebufferResendRequest			(0x3C) // RW; 4 bytes;
#define CounterDiagnosticValue_FramebufferResend				(0x40) // RW; 4 bytes;
#define CounterDiagnosticValue_FramebufferResendFailed  		(0x44) // RW; 4 bytes;
#define CounterDiagnosticValue_FramebufferResendOverflow		(0x48) // RW; 4 bytes;
#define CounterDiagnosticValue_FramebufferDescOverflow			(0x4C) // RW; 4 bytes;

#define CounterStatus_Idle										(          0 ) // RW; 4 bytes;
#define CounterStatus_TriggerWait								(          1 ) // RW; 4 bytes;
#define CounterStatus_Active									(          2 ) // RW; 4 bytes;
#define CounterStatus_Completed									(          3 ) // RW; 4 bytes;
#define CounterStatus_Overflow									(          4 ) // RW; 4 bytes;


//====================================================================================
//
// Group: DigitalIOControl
//
//====================================================================================
#define LineDebouncerHigh							 			( 0x0000D040 ) // RW; 4 bytes;
#define LineDebouncerLow										( 0x0000D044 ) // RW; 4 bytes;
#define LinePulseHigh											( 0x0000D050 ) // RW; 4 bytes;
#define LinePulseLow											( 0x0000D054 ) // RW; 4 bytes;


//====================================================================================
//
// Group: EncoderControl
//
//====================================================================================
#define EncoderValue											( 0x0000D200 ) // RW; 4 bytes; Value for output signal to an internal trigger
#define EncoderValueLow											( 0x0000D204 ) // RW; 4 bytes; Value for output signal to an internal trigger
#define EncoderValueAtReset										( 0x0000D208 ) // RW; 4 bytes; Reseted Value for output signal to an internal trigger
#define EncoderValueAtResetLow									( 0x0000D20C ) // RW; 4 bytes; Reseted Value for output signal to an internal trigger
#define EncoderValue8Byte										( 0x0000D210 ) // RW; 8 bytes; Value for output signal to an internal trigger
#define EncoderValueAtReset8Byte								( 0x0000D218 ) // RW; 8 bytes; Value for output signal to an internal trigger


//====================================================================================
//
// Group: DeviceControl
//
//====================================================================================
#define DeviceTemperature_Sensor_Float							( 0x0000D3D4)  // RW; 4 bytes
#define DeviceTemperature_Case_Float							( 0x0000D3D8)  // RW; 4 bytes
#define DeviceTemperature_FPGA_Float							( 0x0000D3DC)  // RW; 4 bytes

#define DevicePeltierVolt										( 0x0000D3E0 )  // RW; 4 bytes
#define DevicePeltierCurrent									( 0x0000D3E4 )  // RW; 4 bytes
#define DeviceVariant											( 0x0000D3E8 )  // RW; 4 bytes
#define DeviceXMLSchmaVersionMode								( 0x0000D3EC )  // RW; 4 bytes
#define DeviceIfFpgaSelector									( 0x0000D3F8 )  // RW; 4 bytes


//====================================================================================
//
// Group: File Access Control
//
//====================================================================================
#define FileSelector											( 0x0000D100)  // RW   4 bytes : IEnumeration UserSetDefault,UserSet1,UserSet2,LUTLuminance,
#define   FileSelector_SYSTEM									(          0)  // RW   4 bytes : IEnumEntry Custom Reserved
#define   FileSelector_FPGA										(          1)  // RW   4 bytes : IEnumEntry Custom
#define   FileSelector_XML										(          2)  // RW   4 bytes : IEnumEntry Custom
#define   FileSelector_LUTLuminance0 							(          3)  // RW   4 bytes : IEnumEntry
#define   FileSelector_UserSetDefault   						(          4)  // RW   4 bytes : IEnumEntry
#define   FileSelector_UserSet1									(          5)  // RW   4 bytes : IEnumEntry
#define   FileSelector_UserSet2									(          6)  // RW   4 bytes : IEnumEntry
#define   FileSelector_DPC0										(          7)  // RW   4 bytes : IEnumEntry Custom
#define   FileSelector_FFC0										(          8)  // RW   4 bytes : IEnumEntry Custom DSNU + PRNU
#define   FileSelector_SENSOR_FPGA								(          9)  // RW   4 bytes : IEnumEntry Custom
#define   FileSelector_ADM										(         10)  // RW   4 bytes : IEnumEntry Custom
#define   FileSelector_SPECTRUM_WAVE							(         11)  // RW   4 bytes : IEnumEntry Custom
#define   FileSelector_IF_FPGA									(         12)  // RW   4 bytes : IEnumEntry Custom
#define   FileSelector_PHY										(         13)  // RW   4 bytes : IEnumEntry Custom
#define   FileSelector_MAX										(         14)  // MAX of FileSelector
#define FileOperationeSelector									( 0x0000D104)  // RW   4 bytes : IEnumeration Open,Close,Read,Write,Delete
#define   FileOperationeSelector_Open							(          0)  // RW   4 bytes : IEnumEntry
#define   FileOperationeSelector_Close							(          1)  // RW   4 bytes : IEnumEntry
#define   FileOperationeSelector_Read							(          2)  // RW   4 bytes : IEnumEntry
#define   FileOperationeSelector_Write							(          3)  // RW   4 bytes : IEnumEntry
#define   FileOperationeSelector_Delete							(          4)  // RW   4 bytes : IEnumEntry Custom
#define   FileOperationeSelector_MAX							(          5)  // MAX of FileOpenMode
#define FileOperationExecute									( 0x0000D108)  // (R)W 4 bytes : ICommand   FileOperationeSelector as FileOperationExecute[FileSelector][FileOperationSelector]
#define FileOpenMode											( 0x0000D10C)  // R(W) 4 bytes : ICommand   FileOpenMode as FileOperationExecute[FileSelector]
#define   FileOpenMode_Read										(          0)  // RW   4 bytes : IEnumEntry
#define   FileOpenMode_Write									(          1)  // RW   4 bytes : IEnumEntry
#define   FileOpenMode_ReadWrite								(          2)  // RW   4 bytes : IEnumEntry
#define   FileOpenMode_MAX           							(          3)  // RW   4 bytes : IEnumEntry
#define FileAccessBuffer										( BASE_FILE_BUFFER)  // RW   4 bytes : IRegister     Read(/Write) Defines the intermediate access buffer
#define FileAccessOffset			 							( 0x0000D114)  // RW   4 bytes : IInteger      Read(/Write) Controls the Offset of the mapping between the device file storage and the FileAccessBuffer
#define FileAccessLength										( 0x0000D118)  // RW   4 bytes : IInteger      Read /Write  Controls the Length of the mapping between the device file storage and the FileAccessBuffer
#define FileOperationStatus										( 0x0000D11C)  // RW   4 bytes : IEnumeration  Read /Write  Represents the result in operation either Success or Failure
#define   FileOperationStatus_Success							(          0)  // RW   4 bytes : IEnumeration  Read /Write  Represents the result in operation either Success or Failure
#define   FileOperationStatus_Failure							(          1)  // RW   4 bytes : IEnumeration  Read /Write  Represents the result in operation either Success or Failure
#define FileOperationResult										( 0x0000D120)  // RW   4 bytes : IInteger      Read         Represents the result in the number of successfully read/written bytes is returned.
#define FileSize												( 0x0000D124)  // RW   4 bytes : IInteger      Read         Represents the size of the selected file in bytes.


//====================================================================================
//
// Group: ROI
//
//====================================================================================
#define RoiHeightSize											(0x0000D400)
#define RoiHeightOffset											(0x0000D404)
#define RoiHeightValid											(0x0000D408)
#define RoiHeightDefault										(0x0000D40C)
#define SpectrumBandStart										(0x0000D410)
#define SpectrumBandEnd											(0x0000D414)
#define SpectrumBandValid										(0x0000D418)

#define CameraWidthMax											(0x0000D420)
#define CameraHeightMax											(0x0000D424)
#define SensorTotalWidth										(0x0000D428)
#define SensorTotalHeight										(0x0000D42C)


//====================================================================================
//
// Group: CoaXPress
//
//====================================================================================
#define CXP_TEST_SEND_RECIVE_SELECTOR							(0x0000D4B0) // RW; 4 bytes;


//====================================================================================
//
// Group: Auto Bright
//
//====================================================================================
#define AUTO_BRIGHT_EXPOSURE_MODE								(0x0000D4C0) // RW; 4 bytes;
#define AUTO_BRIGHT_EXPOSURE_STATUS								(0x0000D4C4) // RW; 4 bytes;
#define AUTO_BRIGHT_EXPOSURE_MIN								(0x0000D4C8) // RW; 4 bytes;
#define AUTO_BRIGHT_EXPOSURE_MAX								(0x0000D4CC) // RW; 4 bytes;
#define AUTO_BRIGHT_GAIN_MODE									(0x0000D4D0) // RW; 4 bytes;
#define AUTO_BRIGHT_GAIN_STATUS									(0x0000D4D4) // RW; 4 bytes;
#define AUTO_BRIGHT_GAIN_MIN									(0x0000D4D8) // RW; 4 bytes;
#define AUTO_BRIGHT_GAIN_MAX									(0x0000D4DC) // RW; 4 bytes;
#define AUTO_BRIGHT_OVERLAY										(0x0000D4E0) // RW; 4 bytes;
#define AUTO_BRIGHT_TARGET										(0x0000D4E4) // RW; 4 bytes;
#define AUTO_BRIGHT_AVERAGE										(0x0000D4E8) // RO; 4 bytes;
#define AUTO_BRIGHT_WIDTH_SIZE									(0x0000D4EC) // RW; 4 bytes;
#define AUTO_BRIGHT_HEIGHT_SIZE									(0x0000D4F0) // RW; 4 bytes;
#define AUTO_BRIGHT_WIDTH_OFFSET								(0x0000D4F4) // RW; 4 bytes;
#define AUTO_BRIGHT_HEIGHT_OFFSET								(0x0000D4F8) // RW; 4 bytes;
#define AUTO_BRIGHT_TARGET_AREA									(0x0000D4FC) // RW; 4 bytes;


//====================================================================================
//
// Group: Binning
//
//====================================================================================
#define BINNING_HORIZONTAL										(0x0000D600) // RW; 4 bytes;
#define BINNING_VERTICAL										(0x0000D604) // RW; 4 bytes;
#define BINNING_MODE											(0x0000D608) // RW; 4 bytes;


//====================================================================================
//
// For Device specific addresses
// Group: SpectrumImageProcessing
//
//====================================================================================
#define SpectrumOverlap											( 0x0000D700 ) // RW; 4 bytes;
#define SpectrumBandDefaultMode									( 0x0000D708 ) // RW; 4 bytes;


//====================================================================================
//
// For 10G Ethernet
// Group: 
//
//====================================================================================
#define DevicePhyFirmVersion									( 0x0000DA00 ) // RW; 16 bytes;
#define DevicePhyApiVersion										( 0x0000DA10 ) // RW; 8 bytes;
#define DevicePacketDelayCalc									( 0x0000DA18 ) // RW; 4 bytes;
#define DevicePacketDelayData									( 0x0000DA20 ) // RW; 4 bytes;


//====================================================================================
//
// Group: TransportLayerControl
//
//====================================================================================
#define PayloadSize												( 0x0000A024 ) // RO; 4 bytes; video_total_bpf;


//====================================================================================
//
// Group: GigE Vision
//
//====================================================================================
#define GevVersion												( 0x00000000 ) // RO; 4 bytes; Bootstrap
#define GevDeviceMode											( 0x00000004 ) // RO; 4 bytes; Bootstrap
#define GevVersionMajor											( 0x00000000 ) // RO; D0  to D15 2 bytes; Bootstrap
#define GevVersionMinor											( 0x00000000 ) // RO; D16 to D31 2 bytes; Bootstrap
#define GevDeviceModeIsBigEndian								( 0x0000A024 ) // RO; D0 1 bit; Bootstrap
#define GevDeviceModeCharacterSet								( 0x00000004 ) // RO; 1 bytes; Bootstrap D24 to D31
#define GevInterfaceSelector									( 0x0000A024 ) // RO; 4 bytes; Bootstrap
#define GevMACAddressHigh										( 0x00000008 ) // RO; D16 to D31 2 bytes; Bootstrap High
#define GevMACAddressLow										( 0x0000000C ) // RO; D0  to D31 4 bytes; Bootstrap Low
#define GevSupportedIPConfig									( 0x00000010 ) // RO; 4 bytes; Bootstrap
#define GevSupportedOptionGVCP									( 0x00000934 ) // RO; 4 bytes; Bootstrap
#define GevSupportedOption										( 0x0000092C ) // RO; 4 bytes; Bootstrap
#define GevCurrentIPConfigurationLLA							( 0x00000014 ) // RO; D29 1 bit; Bootstrap
#define GevCurrentIPConfigurationDHCP							( 0x00000014 ) // RO; D30 1 bit; Bootstrap
#define GevCurrentIPConfigurationPersistentIP 					( 0x00000014 ) // RO; D31 1 bit; Bootstrap
#define GevCurrentIPAddress										( 0x00000024 ) // RO; 4 bytes; Bootstrap + offset
#define GevCurrentSubnetMask  									( 0x00000034 ) // RO; 4 bytes; Bootstrap + offset
#define GevCurrentDefaultGateway 								( 0x00000044 ) // RO; 4 bytes; Bootstrap + offset
#define GevFirstURL			 									( 0x0000A024 ) // RO; 4 bytes; Bootstrap + offset
#define GevSecondURL		 									( 0x0000A024 ) // RO; 4 bytes; Bootstrap + offset
#define GevNumberOfInterfaces									( 0x00000600 ) // RO; 4 bytes; Bootstrap
#define GevPersistentIPAddress   								( 0x0000064C ) // RO; 4 bytes; Bootstrap
#define GevPersistentSubnetMask  								( 0x0000065C ) // RO; 4 bytes; Bootstrap
#define GevPersistentDefaultGateway 							( 0x000006CC ) // RO; 4 bytes; Bootstrap
#define GevLinkSpeed			 								( 0x00000670 ) // RO; 4 bytes; Bootstrap
#define GevMessageChannelCount									( 0x00000900 ) // RO; 4 bytes; Bootstrap
#define GevStreamChannelCount									( 0x00009004 ) // RO; 4 bytes; Bootstrap
#define GevHeartbeatTimeout										( 0x00000938 ) // RO; 4 bytes; Bootstrap
#define GevTimestampTickFrequencyHigh							( 0x0000093C ) // RO; 4 bytes; Bootstrap
#define GevTimestampTickFrequencyLow							( 0x00000940 ) // RO; 4 bytes; Bootstrap
#define GevBootstrapTimestampControlReset   					( 0x00000944 ) // WO; B31; Bootstrap
#define GevBootstrapTimestampControlLatch						( 0x00000944 ) // WO; B30; Bootstrap
#define GevBootstrapTimestampValueHigh							( 0x00000948 ) // RO; 4 bytes; Bootstrap
#define GevBootstrapTimestampValueLow							( 0x0000094C ) // RO; 4 bytes; Bootstrap
#define GevCCP													( 0x00000A00 ) // RW; 4 bytes; Controls the device access privilege of an application
#define GevMCPHostPort											( 0x00000B00 ) // RW; 4 bytes; Indicates the destination IP address for the message channel.
#define GevMCDA													( 0x00000B10 ) // RO; 4 bytes; Bootstrap
#define GevMCTT													( 0x00000B14 ) // RO; 4 bytes; Bootstrap
#define GevMCRC													( 0x00000B18 ) // RO; 4 bytes; Bootstrap
#define GevMCSP													( 0x00000B1C ) // RO; 4 bytes; Bootstrap
#define GevStreamChannelSelector  								( 0x00000D00 ) // RO; 4 bytes; Bootstrap
#define GevSCPInterfaceIndex									( 0x00000D00 ) // RO; 4 bytes; Bootstrap
#define GevSCPHostPort			 								( 0x00000D00 ) // RO; 4 bytes; Bootstrap
#define GevSCPSFireTestPacket									( 0x00000D04 ) // RW; D0 1-bit; Bootstrap
#define GevSCPSDoNotFragment									( 0x00000D04 ) // RW; D1 1-bit; Bootstrap
#define GevSCPSBigEndian										( 0x00000D04 ) // RW; D2 1-bit; Bootstrap
#define GevSCPSPacketSize		 								( 0x00000D04 ) // RW; 2 bytes; Bootstrap D16 to D31
#define GevSCPD													( 0x00000D08 ) // RW; 4 bytes; Bootstrap
#define GevSCDA													( 0x00000D18 ) // RW; 4 bytes; Bootstrap
#define LineDelay												( 0x0000A02C ) // RW; 4 bytes; video_gap_x;    Additional gap after each image line in pixels.
#define FrameDelay												( 0x0000A030 ) // RW; 4 bytes; video_gap_y;


//====================================================================================
//
// For stream data size
// Group: TransportLayerControl
//
//====================================================================================
// Packet size margins
#define SCPSPacketSizeMin										( 0x0000A040 ) // RW; 4 bytes; SCPS_MIN;
#define SCPSPacketSizeMax										( 0x0000A044 ) // RW; 4 bytes; SCPS_MAX;
#define SCPSPacketSizeInc										( 0x0000A048 ) // RW; 4 bytes; SCPS_INC;

#define SCPSPacketSizeMin2										( 0x10000000 ) // RW; 4 bytes; SCPS_MIN;
#define SCPSPacketSizeMax2										( 0x10000004 ) // RW; 4 bytes; SCPS_MAX;
#define SCPSPacketSizeInc2										( 0x10000008 ) // RW; 4 bytes; SCPS_INC;


//====================================================================================
//
// For IEEE 1588 PTP 
// Group: 
//
//====================================================================================
#define IEEE1588_PTP_CLOCK_ACCURACY								(0x10000014)
#define IEEE1588_PTP_DATA_SET									(0x10000018)
#define IEEE1588_PTP_STATUS										(0x1000001C)
#define IEEE1588_PTP_SERVER_STATUS								(0x10000020)
#define IEEE1588_PTP_OFFSET_FORM_MASTER_H						(0x10000024)
#define IEEE1588_PTP_OFFSET_FORM_MASTER_L						(0x10000028)
#define IEEE1588_PTP_MEAN_PATH_DELAY_H							(0x1000002C)
#define IEEE1588_PTP_MEAN_PATH_DELAY_L							(0x10000030)
#define IEEE1588_PTP_CLOCK_ID_H									(0x10000034)
#define IEEE1588_PTP_CLOCK_ID_L									(0x10000038)
#define IEEE1588_PTP_PARENT_CLOCK_ID_H							(0x1000003C)
#define IEEE1588_PTP_PARENT_CLOCK_ID_L							(0x10000040)
#define IEEE1588_PTP_GRAND_MASTER_CLOCK_ID_H					(0x10000044)
#define IEEE1588_PTP_GRAND_MASTER_CLOCK_ID_L					(0x10000048)

#endif // GEV_H_

// eof
