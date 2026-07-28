/******************************************************************************/
/*  GigE Vision Core Firmware                                                 */
/*----------------------------------------------------------------------------*/
/*    File :  gige.h                                                          */
/*    Date :  2019-05-09                                                      */
/*     Rev :  0.2                                                             */
/*  Author :  Maasahide Matsubara                                             */
/*  Copyright (c) 2014 - 2019 AVAL DATA Corporation All Right Reserved.       */
/*----------------------------------------------------------------------------*/
/*  The GigE Vision library (libgige.a) public include file                   */
/*----------------------------------------------------------------------------*/
/*  0.1  |  2014-09-12  |  MM  |  Initial release                             */
/*  0.2  |  2019-05-08  |  MM  |  Add several gige status codes for GigE 2.0  */
/******************************************************************************/

#ifndef _GIGE_H_
#define _GIGE_H_

#include "../Common/common.h"

//================================================================================
// GigE 1Gbps
//================================================================================
#if !defined (MODE_GIGE_10G)


// ---- General-purpose unsigned numeric types ---------------------------------

#ifndef XIL_TYPES_H
    typedef unsigned long long  u64;
#endif
#ifndef XBASIC_TYPES_H
    typedef unsigned long   u32;
    typedef unsigned short  u16;
    typedef unsigned char   u8;
#endif


// ---- GigE library global control --------------------------------------------

// Global variables
extern u32 gige_base_addr;
extern u8  gige_reg_bank;
extern u8  gige_phy_access;
extern u8  gige_phy_user;

// I2C access modes for EEPROM and sensor
extern u8  eeprom_access_mode;
extern u8  sensor_access_mode;

// User supplied link status
#define PHY_USER_OFF    0x00
#define PHY_USER_DOWN   0x01
#define PHY_USER_MASK   0x02
#define PHY_USER_UP     0x03

// PHY access modes
#define PHY_ACC_COMMON  0x01
#define PHY_ACC_MULTI   0x02

// Types of the Ethernet PHYs
#define PHY_BROADCOM    0x00
#define PHY_MARVELL     0x01
#define PHY_NATIONAL    0x02
#define PHY_MICREL      0x03
#define PHY_NONE        0xFF

// Device modes (transmitter, receiver, non-streaming)
#define DEV_MODE_TX     0x00
#define DEV_MODE_RX     0x01
#define DEV_MODE_NSTM   0x02

// I2C devices access modes
#define I2C_ACC_NONE    0x00
#define I2C_ACC_COMMON  0x01
#define I2C_ACC_LARGE   0x02
#define I2C_ACC_MULTI   0xFF


// ---- GigE Vision status codes -----------------------------------------------

#define GEV_STATUS_SUCCESS              0x0000
#define GEV_STATUS_PACKET_RESEND        0x1000
#define GEV_STATUS_NOT_IMPLEMENTED      0x8001
#define GEV_STATUS_INVALID_PARAMETER    0x8002
#define GEV_STATUS_INVALID_ADDRESS      0x8003
#define GEV_STATUS_WRITE_PROTECT        0x8004
#define GEV_STATUS_BAD_ALIGNMENT        0x8005
#define GEV_STATUS_ACCESS_DENIED        0x8006
#define GEV_STATUS_BUSY                 0x8007
#define GEV_STATUS_LOCAL_PROBLEM        0x8008
#define GEV_STATUS_MSG_MISMATCH         0x8009
#define GEV_STATUS_INVALID_PROTOCOL     0x800A
#define GEV_STATUS_NO_MSG               0x800B
#define GEV_STATUS_PACKET_UNAVAILABLE   0x800C
#define GEV_STATUS_DATA_OVERRUN         0x800D
#define GEV_STATUS_INVALID_HEADER       0x800E
#define GEV_STATUS_WRONG_CONFIG                         0x800F
#define GEV_STATUS_PACKET_NOT_YET_AVAILABLE             0x8010
#define GEV_STATUS_PACKET_AND_PREV_REMOVED_FROM_MEMORY  0x8011
#define GEV_STATUS_PACKET_REMOVED_FROM_MEMORY           0x8012
#define GEV_STATUS_NO_REF_TIME1                         0x8013
#define GEV_STATUS_PACKET_TEMPORARILY_UNAVAILABLE       0x8014
#define GEV_STATUS_OVERFLOW                             0x8015
#define GEV_STATUS_ACTION_LATE                          0x8016
#define GEV_STATUS_ERROR                                0x8FFF


// Device-Specific Status Code
#define GEV_STATUS_CAMERA_NOT_INIT                0xC001
#define GEV_STATUS_CAMERA_ALWAYS_INIT             0xC002
#define GEV_STATUS_CANNOT_CREATE_SOCKET           0xC003
#define GEV_STATUS_SEND_ERROR                     0xC004
#define GEV_STATUS_RECEIVE_ERROR                  0xC005
#define GEV_STATUS_CAMERA_NOT_FOUND               0xC006
#define GEV_STATUS_CANNOT_ALLOC_MEMORY            0xC007
#define GEV_STATUS_TIMEOUT                        0xC008
#define GEV_STATUS_SOCKET_ERROR                   0xC009
#define GEV_STATUS_INVALID_ACK                    0xC00A
#define GEV_STATUS_CANNOT_START_THREAD            0xC00B
#define GEV_STATUS_CANNOT_SET_SOCKET_OPT          0xC00C
#define GEV_STATUS_CANNOT_OPEN_DRIVER             0xC00D
#define GEV_STATUS_HEARTBEAT_READ_ERROR           0xC00E
#define GEV_STATUS_EVALUATION_EXPIRED             0xC00F
#define GEV_STATUS_GRAB_ERROR                     0xC010
#define GEV_STATUS_DRIVER_READ_ERROR              0xC011
#define GEV_STATUS_XML_READ_ERROR                 0xC012
#define GEV_STATUS_XML_OPEN_ERROR                 0xC013
#define GEV_STATUS_XML_FEATURE_ERROR              0xC014
#define GEV_STATUS_XML_COMMAND_ERROR              0xC015
#define GEV_STATUS_GAIN_NOT_SUPPORTED             0xC016
#define GEV_STATUS_EXPOSURE_NOT_SUPPORTED         0xC017
#define GEV_STATUS_CANNOT_GET_ADAPTER_INFO        0xC018
#define GEV_STATUS_ERROR_INVALID_HANDLE           0xC019
#define GEV_STATUS_CLINK_SET_BAUD                 0xC01A
#define GEV_STATUS_CLINK_SEND_BUFFER_FULL         0xC01B
#define GEV_STATUS_CLINK_RECEIVE_BUFFER_NO_DATA   0xC01C
#define GEV_STATUS_FEATURE_NOT_AVAILABLE          0xC01D
#define GEV_STATUS_MATH_PARSER_ERROR              0xC01E
#define GEV_STATUS_FEATURE_ITEM_NOT_AVAILABLE     0xC01F
#define GEV_STATUS_NOT_SUPPORTED                  0xC020
#define GEV_STATUS_GET_URL_ERROR                  0xC021
#define GEV_STATUS_READ_XML_MEM_ERROR             0xC022
#define GEV_STATUS_XML_SIZE_ERROR                 0xC023
#define GEV_STATUS_XML_ZIP_ERROR                  0xC024
#define GEV_STATUS_XML_ROOT_ERROR                 0xC025
#define GEV_STATUS_XML_FILE_ERROR                 0xC026
#define GEV_STATUS_DIFFERENT_IMAGE_HEADER         0xC027
#define GEV_STATUS_XML_SCHEMA_ERROR               0xC028
#define GEV_STATUS_XML_STYLESHEET_ERROR           0xC029
#define GEV_STATUS_FEATURE_LIST_ERROR             0xC02A
#define GEV_STATUS_ALREADY_OPEN                   0xC02B
#define GEV_STATUS_TEST_PACKET_DATA_ERROR         0xC02C
#define GEV_STATUS_FEATURE_NOT_FLOAT              0xC02D
#define GEV_STATUS_FEATURE_NOT_INTEGER            0xC02E

// ---- GigE Vision events -----------------------------------------------------
#define GEV_EVENT_AcquisitionTrigger              0x0000
#define GEV_EVENT_AcquisitionTriggerMissed        0x0001
#define GEV_EVENT_AcquisitionStart                0x0002
#define GEV_EVENT_AcquisitionEnd                  0x0003
#define GEV_EVENT_AcquisitionTransferStart        0x0004
#define GEV_EVENT_AcquisitionTransferEnd          0x0005
#define GEV_EVENT_AcquisitionError                0x0006
#define GEV_EVENT_FrameBurstStart                 0x0007
#define GEV_EVENT_FrameBurstEnd                   0x0008
#define GEV_EVENT_FrameTrigger                    0x0009
#define GEV_EVENT_FrameTriggerMissed              0x000A
#define GEV_EVENT_FrameStart                      0x000B
#define GEV_EVENT_FrameEnd                        0x000C
#define GEV_EVENT_FrameTransferStart              0x000D
#define GEV_EVENT_FrameTransferEnd                0x000E
#define GEV_EVENT_LineTrigger                     0x000F
#define GEV_EVENT_LineTriggerMissed               0x0010
#define GEV_EVENT_LineStart                       0x0011
#define GEV_EVENT_LineEnd                         0x0012
#define GEV_EVENT_ExposureStart                   0x0013
#define GEV_EVENT_ExposureEnd                     0x0014
#define GEV_EVENT_Stream0TransferStart            0x0015
#define GEV_EVENT_Stream0TransferEnd              0x0016
#define GEV_EVENT_Stream0TransferPause            0x0017
#define GEV_EVENT_Stream0TransferResume           0x0018
#define GEV_EVENT_Stream0TransferBlockStart       0x0019
#define GEV_EVENT_Stream0TransferBlockEnd         0x001A
#define GEV_EVENT_Stream0TransferBlockTrigger     0x001B
#define GEV_EVENT_Stream0TransferBurstStart       0x001C
#define GEV_EVENT_Stream0TransferBurstEnd         0x001D
#define GEV_EVENT_Stream0TransferOverflow         0x001E
#define GEV_EVENT_SequencerSetChange              0x001F
#define GEV_EVENT_Counter0Start                   0x0020
#define GEV_EVENT_Counter1Start                   0x0021
#define GEV_EVENT_Counter0End                     0x0022
#define GEV_EVENT_Counter1End                     0x0024
#define GEV_EVENT_Timer0Start                     0x0025
#define GEV_EVENT_Timer1Start                     0x0026
#define GEV_EVENT_Timer0End                       0x0029
#define GEV_EVENT_Timer1End                       0x002A
#define GEV_EVENT_Encoder0Stopped                 0x002C
#define GEV_EVENT_Encoder1Stopped                 0x002D
#define GEV_EVENT_Encoder0Restarted               0x002F
#define GEV_EVENT_Encoder1Restarted               0x0030
#define GEV_EVENT_Line0RisingEdge                 0x0032
#define GEV_EVENT_Line1RisingEdge                 0x0033
#define GEV_EVENT_Line0FallingEdge                0x0035
#define GEV_EVENT_Line1FallingEdge                0x0036
#define GEV_EVENT_Line0AnyEdge                    0x0038
#define GEV_EVENT_Line1AnyEdge                    0x0039
#define GEV_EVENT_LinkTrigger0                    0x003B
#define GEV_EVENT_LinkTrigger1                    0x003C
#define GEV_EVENT_LinkSpeedChange                 0x003E
#define GEV_EVENT_ActionLate                      0x003F
#define GEV_EVENT_Error                           0x0040
#define GEV_EVENT_Test                            0x0041
#define GEV_EVENT_PrimaryApplicationSwitch        0x0042
#define GEV_EVENT_MAX                             GEV_EVENT_PrimaryApplicationSwitch

//#if defined(AXP_ABA003IR_01) || defined(AXP_ABA001IR_01) || defined(AXP_ABAU20MIRIR_01) || defined(AXP_ABL005IR_01) || defined(AXP_ABL005IR_02) || defined(AXP_ABL005MIR_01) || defined(AXP_ABL00W5IR_01) || defined(AXP_ABL010IR_01)  || defined(AXP_ABL010IR_02)  || defined (AXP_ABL005IRHS_01) || defined(AXP_AHS001MIR_01) || defined(AXP_AHSU20MIR_01)  || defined(AXP_AHSU21MIR_01)
#define GEV_EVENT_TRIGGER                    0x0002
#define GEV_EVENT_START_OF_EXPOSURE          0x0003
#define GEV_EVENT_END_OF_EXPOSURE            0x0004
#define GEV_EVENT_START_OF_TRANSFER          0x0005
#define GEV_EVENT_END_OF_TRANSFER            0x0006
//#endif	// #if defined(AXP_ABA003IR_01) || defined(AXP_ABA001IR_01) || defined(AXP_ABAU20MIRIR_01) || defined(AXP_ABL005IR_01) || defined(defined(AXP_ABL005IR_02) || defined(AXP_ABL005MIR_01) || defined(AXP_ABL00W5IR_01) || defined(AXP_AHS001MIR_01) || defined(AXP_AHSU20MIR_01)  || defined(AXP_AHSU21MIR_01)


#define GEV_EVENT_ERROR                      0x8001
//      GEV_EVENT_ERROR:   Error codes identical to status codes 0x8001 - 0x8FFF
#define GEV_EVENT_DEVSPEC                    0x9000
//      GEV_EVENT_DEVSPEC: Device-specific error codes 0x9000 - 0xFFFF
#define GEV_EVENT_ACQUISITION_START          0x9002
#define GEV_EVENT_ACQUISITION_END            0x9003
#define GEV_EVENT_LINE0                      0x9004
#define GEV_EVENT_LINE1                      0x9005
#define GEV_EVENT_LINE2                      0x9006
#define GEV_EVENT_LINE3                      0x9007
#define GEV_EVENT_LINE4                      0x9008
#define GEV_EVENT_LINE5                      0x9009

// ---- GigE Vision Register address -----------------------------------------------------
// Register address
#define GEV_REG_VERSION                      (0x00000000)
#define GEV_REG_DEVICE_MODE                  (0x00000004)

#define GEV_REG_DEVICE_MAC_HIGH_0            (0x00000008)
#define GEV_REG_DEVICE_MAC_LOW_0             (0x0000000c)
#define GEV_REG_SUPPORTED_IP_CONFIGURATION_0 (0x00000010)
#define GEV_REG_CURIPCFG_0                   (0x00000014)
#define GEV_REG_CURRENT_IP_ADDRESS_0         (0x00000024)
#define GEV_REG_CURRENT_SUBNET_MASK_0        (0x00000034)
#define GEV_REG_CURRENT_DEFAULT_GATEWAY_0    (0x00000044)

#define GEV_REG_MANUFACTURER_NAME            (0x00000048)
#define GEV_REG_MODEL_NAME                   (0x00000068)
#define GEV_REG_DEVICE_VERSION               (0x00000088)
#define GEV_REG_MANUFACTURER_INFO            (0x000000a8)
#define GEV_REG_SERIAL_NUMBER				 (0x000000d8)
#define GEV_REG_USER_DEFINED_NAME			 (0x000000e8)
#define GEV_REG_FIRST_URL                    (0x00000200)
#define GEV_REG_SECOND_URL                   (0x00000400)
#define GEV_REG_NUMBER_OF_NETWORK_INTERFACES (0x00000600)

#define GEV_REG_PERSISTENT_IP_ADDRESS_0		 (0x0000064C)
#define GEV_REG_PERSISTENT_SUBNET_MASK_0	 (0x0000065C)
#define GEV_REG_PERSISTENT_DEFAULT_GATEWAY_0 (0x0000066C)

#define GEV_REG_DEVICE_MAC_HIGH_1            (0x00000680)
#define GEV_REG_DEVICE_MAC_LOW_1             (0x00000684)
#define GEV_REG_SUPPORTED_IP_CONFIGURATION_1 (0x00000688)
#define GEV_REG_CURIPCFG_1                   (0x0000068C)
#define GEV_REG_CURRENT_IP_ADDRESS_1         (0x0000069C)
#define GEV_REG_CURRENT_SUBNET_MASK_1        (0x000006AC)
#define GEV_REG_CURRENT_DEFAULT_GATEWAY_1    (0x000006BC)
#define GEV_REG_PERSISTENT_IP_ADDRESS_1		 (0x000006CC)
#define GEV_REG_PERSISTENT_SUBNET_MASK_1	 (0x000006DC)
#define GEV_REG_PERSISTENT_DEFAULT_GATEWAY_1 (0x000006EC)

#define GEV_REG_DEVICE_MAC_HIGH_2            (0x00000700)
#define GEV_REG_DEVICE_MAC_LOW_2             (0x00000704)
#define GEV_REG_SUPPORTED_IP_CONFIGURATION_2 (0x00000708)
#define GEV_REG_CURIPCFG_2                   (0x0000070C)
#define GEV_REG_CURRENT_IP_ADDRESS_2         (0x0000071C)
#define GEV_REG_CURRENT_SUBNET_MASK_2        (0x0000072C)
#define GEV_REG_CURRENT_DEFAULT_GATEWAY_2    (0x0000073C)
#define GEV_REG_PERSISTENT_IP_ADDRESS_2		 (0x0000074C)
#define GEV_REG_PERSISTENT_SUBNET_MASK_2	 (0x0000075C)
#define GEV_REG_PERSISTENT_DEFAULT_GATEWAY_2 (0x0000076C)

#define GEV_REG_DEVICE_MAC_HIGH_3            (0x00000780)
#define GEV_REG_DEVICE_MAC_LOW_3             (0x00000784)
#define GEV_REG_SUPPORTED_IP_CONFIGURATION_3 (0x00000788)
#define GEV_REG_CURIPCFG_3                   (0x0000078C)
#define GEV_REG_CURRENT_IP_ADDRESS_3         (0x0000079C)
#define GEV_REG_CURRENT_SUBNET_MASK_3        (0x000007AC)
#define GEV_REG_CURRENT_DEFAULT_GATEWAY_3    (0x000007BC)
#define GEV_REG_PERSISTENT_IP_ADDRESS_3		 (0x000007CC)
#define GEV_REG_PERSISTENT_SUBNET_MASK_3	 (0x000007DC)
#define GEV_REG_PERSISTENT_DEFAULT_GATEWAY_3 (0x000007EC)

#define GEV_REG_NUMBER_OF_MESSAGE_CHANNELS   (0x00000900)
#define GEV_REG_NUMBER_OF_STREAM_CHANNELS    (0x00000904)
#define GEV_REG_NUMBER_OF_ACTION_SIGNALS	 (0x00000908)
#define GEV_REG_STREAM_CHANNELS_CAPABILITY	 (0X0000092c)
#define GEV_STREAM_CHANNELS_CAPABILITY_SCSP_SUPPORTED	(0x00000001)
#define GEV_REG_MESSAGE_CHANNEL_CAPABILITY	 (0X00000930)
#define GEV_MESSAGE_CHANNEL_CAPABILITY_MCSP_SUPPORTED	(0x00000001)
#define GEV_REG_GVCP_SUPPORTED_COMMANDS      (0x00000934)
#define GEV_REG_HEARTBEAT_TIMEOUT            (0x00000938)
#define GEV_REG_TSTF_HIGH                    (0x0000093c)
#define GEV_REG_TSTF_LOW                     (0x00000940)
#define GEV_REG_TSC                          (0x00000944)
#define GEV_REG_TSV_HIGH                     (0x00000948)
#define GEV_REG_TSV_LOW                      (0x0000094c)
#define GEV_REG_CCP                          (0x00000a00)
#define GEV_REG_MCP                          (0x00000b00)
#define GEV_REG_MCDA                         (0x00000b10)
#define GEV_REG_MCTT                         (0x00000b14)
#define GEV_REG_MCRC                         (0x00000b18)
#define GEV_REG_MCSP                         (0x00000b1C)

#define GVCP_REG_CMD                         (0x00000934)
#define INVALID_REG                          (0x00000111)
#define NOT_ALLIGNED                         (0x00000005)

#define GEV_REG_SCP(I)                       (0x0d00+(0x40*I))
#define GEV_REG_SCPS(I)                      (0x0d04+(0x40*I))
#define GEV_REG_SCPD(I)                      (0x0d08+(0x40*I))
#define GEV_REG_SCDA(I)                      (0x0d18+(0x40*I))

// Register definitions
#define GEV_CCP_OPEN_ACCESS                  (0x00000000)
#define GEV_CCP_EXCLUSIVE_ACCESS             (0x00000001)
#define GEV_CCP_CONTROL_ACCESS               (0x00000002)

#define GEV_CURIPCFG_LOCAL_LINK              (0x00000004)
#define GEV_CURIPCFG_DHCP                    (0x00000002)

#define GEV_SCPS_PACKET_SIZE_MASK            (0x0000ffff)
#define GEV_SCPS_DO_NOT_FRAGMENT             (0x40000000)
#define GEV_SCPS_FIRE_TEST_PACKET            (0x80000000)

#define GEV_TSC_LATCH                        (0x00000002)

// ---- GigE Vision pixel formats ----------------------------------------------

#define GVSP_PIX_MONO8						0x01080001
#define GVSP_PIX_MONO8_SIGNED				0x01080002
#define GVSP_PIX_MONO10						0x01100003
#define GVSP_PIX_MONO10_PACKED				0x010C0004
#define GVSP_PIX_MONO12						0x01100005
#define GVSP_PIX_MONO12_PACKED				0x010C0006
#define GVSP_PIX_MONO14						0x01100025
#define GVSP_PIX_MONO16						0x01100007
#define GVSP_PIX_BAYGR8						0x01080008
#define GVSP_PIX_BAYRG8						0x01080009
#define GVSP_PIX_BAYGB8						0x0108000A
#define GVSP_PIX_BAYBG8						0x0108000B
#define GVSP_PIX_BAYGR10					0x0110000C
#define GVSP_PIX_BAYRG10					0x0110000D
#define GVSP_PIX_BAYGB10					0x0110000E
#define GVSP_PIX_BAYBG10					0x0110000F
#define GVSP_PIX_BAYGR12					0x01100010
#define GVSP_PIX_BAYRG12					0x01100011
#define GVSP_PIX_BAYGB12					0x01100012
#define GVSP_PIX_BAYBG12					0x01100013
#define GVSP_PIX_RGB8_PACKED				0x02180014
#define GVSP_PIX_BGR8_PACKED				0x02180015
#define GVSP_PIX_RGBA8_PACKED				0x02200016
#define GVSP_PIX_BGRA8_PACKED				0x02200017
#define GVSP_PIX_RGB10_PACKED				0x02300018
#define GVSP_PIX_BGR10_PACKED				0x02300019
#define GVSP_PIX_RGB12_PACKED				0x0230001A
#define GVSP_PIX_BGR12_PACKED				0x0230001B
#define GVSP_PIX_RGB10V1_PACKED				0x0220001C
#define GVSP_PIX_RGB10V2_PACKED				0x0220001D
#define GVSP_PIX_YUV411_PACKED				0x020C001E
#define GVSP_PIX_YUV422_PACKED				0x0210001F
#define GVSP_PIX_YUV444_PACKED				0x02180020
#define GVSP_PIX_RGB8_PLANAR				0x02180021
#define GVSP_PIX_RGB10_PLANAR				0x02300022
#define GVSP_PIX_RGB12_PLANAR				0x02300023
#define GVSP_PIX_RGB16_PLANAR				0x02300024

// new name 2.0
#define GVSP_PIX_RGB8						0x02180014

#define GVSP_PIX_BGR8_PACKED				0x02180015
// new name 2.0
#define GVSP_PIX_BGR8						0x02180015

#define GVSP_PIX_RGBA8_PACKED				0x02200016
// new name 2.0
#define GVSP_PIX_RGBA8						0x02200016

#define GVSP_PIX_BGRA8_PACKED				0x02200017
// new name 2.0
#define GVSP_PIX_BGRA8						0x02200017

#define GVSP_PIX_RGB10_PACKED				0x02300018
// new name 2.0
#define GVSP_PIX_RGB10						0x02300018

#define GVSP_PIX_BGR10_PACKED				0x02300019
// new name 2.0
#define GVSP_PIX_BGR10						0x02300019

#define GVSP_PIX_RGB12_PACKED				0x0230001A
// new name 2.0
#define GVSP_PIX_RGB12						0x0230001A

#define GVSP_PIX_BGR12_PACKED				0x0230001B
// new name 2.0
#define GVSP_PIX_BGR12						0x0230001B

#define GVSP_PIX_RGB16_PACKED				0x02300033
// new name 2.0
#define GVSP_PIX_RGB16						0x02300033

#define GVSP_PIX_RGB10V1_PACKED				0x0220001C
#define GVSP_PIX_RGB10V2_PACKED				0x0220001D
// new name 2.0
#define GVSP_PIX_RGB10P32					0x0220001D

#define GVSP_PIX_RGB12V1_PACKED				0x02240034

#define GVSP_PIX_RGB565_PACKED				0x02100035
// new name 2.0
#define GVSP_PIX_RGB565P					0x02100035

#define GVSP_PIX_BGR565_PACKED				0x02100036
// new name 2.0
#define GVSP_PIX_BGR565P					0x02100036

#define GVSP_PIX_YUV411_PACKED				0x020C001E
// new name 2.0
#define GVSP_PIX_YUV411_8_UYYVYY			0x020C001E

#define GVSP_PIX_YUV422_PACKED				0x0210001F
// new name 2.0
#define GVSP_PIX_YUV422_8_UYVY				0x0210001F

#define GVSP_PIX_YUV422_YUYV_PACKED			0x02100032
// new name 2.0
#define GVSP_PIX_YUV422_8					0x02100032

#define GVSP_PIX_YUV444_PACKED				0x02180020
// new name 2.0
#define GVSP_PIX_YUV8_UYV					0x02180020

#define GVSP_PIX_YCBCR_8_CBYCR				0x0218003A

#define GVSP_PIX_YCBCR422_8					0x0210003B

#define GVSP_PIX_YCBCR422_8_CBYCRY			0x02100043

#define GVSP_PIX_YCBCR411_8_CBYYCRYY		0x020C003C

#define GVSP_PIX_YCbCr601_8_CbYCr			0x0218003D
#define GVSP_PIX_YCbCr601_422_8				0x0218003E
#define GVSP_PIX_YCbCr601_422_8_CbYCrY		0x02100044
#define GVSP_PIX_YCbCr601_422_8_CbYYCrYY	 0x020C003F


#define GVSP_PIX_YCBCR709_8_CBYCR			0x02180040
#define GVSP_PIX_YCbCr709_422_8				0x02100041
#define GVSP_PIX_YCbCr709_422_8_CbYCrY		0x02100045
#define GVSP_PIX_YCbCr709_422_8_CbYYCrYY	0x020C0042

#define GVSP_PIX_RGB8_PLANAR				0x02180021
#define GVSP_PIX_RGB10_PLANAR				0x02300022
#define GVSP_PIX_RGB12_PLANAR				0x02300023
#define GVSP_PIX_RGB16_PLANAR				0x02300024

#define GVSP_PIX_EFFECTIVE_PIXEL_SIZE_MASK	0x00FF0000
#define GVSP_PIX_EFFECTIVE_PIXEL_SIZE_SHIFT	16

// ---- Acquisition modes as defined in XML file -------------------------------

#define ACQ_MODE_CONTINUOUS					0x00000000


// ---- Debug info verbosity levels --------------------------------------------

#define DBG_QUIET							0
#define DBG_NORMAL							1
#define DBG_ICMP							2
#define DBG_VERBOSE							3

#if 0//@@@1
// ---- Prototypes of public functions -----------------------------------------

// Main control
void gige_init(u8 idx, u32 base_addr, u8 dev_mode, u32 bus_clk_freq, u8 phy_type, u8 phy_addr, u32 phy_mdc_freq, u8 data_rate, u16 eth_mtu, u8 verbosity);
int  gige_callback(u8 idx);
void gige_switch(u8 idx);
u32  gige_get_int_status(void);
void gige_clr_int_req(void);
u32  gige_get_int_mask(void);
void gige_set_int_mask(u32 mask);

// Control channel
u32  gvcp_get_reg(u32 address, u16 *status);
u16  gvcp_set_reg(u32 address, u32 value);

// Message channel
void gige_send_message(u16 event, u16 channel, u16 data_len, u8 *data);

// UDP
void ethernet_header(u16 eth_type, u64 destination);
void ip_header      (u16 data_len, u8 protocol, u32 src_addr, u32 dest_addr);
void udp_header     (u16 data_len, u16 src_port, u16 dest_port);
void telnet_send    (u32 len, u8 *data);

// I2C access to image sensor
void sensor_write_byte(u8 address, u8 value);
u8   sensor_read_byte (u8 address);
void sensor_write_word(u8 address, u16 value);
u16  sensor_read_word (u8 address);

// Access to the I2C EEPROM
void eeprom_write_dword(u16 address, u32 value);
u32  eeprom_read_dword (u16 address);
void eeprom_write_word(u16 address, u16 value);
u32  eeprom_read_word (u16 address);
void eeprom_write_byte(u16 address, u8 value);
u32  eeprom_read_byte(u16 address);

// Control of the GigE library
void gige_set_params(u8 uart_bypass, u8 heartbeat_en, u8 payload_type);
void gige_get_params(u8 *uart_bypass, u8 *heartbeat_en, u8 *payload_type);

// Auxiliary functions
u16  get_phy_reg(u8 reg);
void set_phy_reg (u8 reg, u16 val); // Added from Matthias's comment Ticket: #201400456 [28/11/2014]
void gige_print_header(void);
void gige_set_sernum  (char *sn);
void print_mac(const char *txt, u64 mac);
void print_ip(const char *txt, u32 ip);

#endif//@@@1

//================================================================================
// GigE 10Gbps
//================================================================================
#else // #if !defined (MODE_GIGE_10G)


// ---- General-purpose types and constants ------------------------------------

// Standard platform-independent types and definitions
#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>

// Shortcuts to basic unsigned types
#ifndef XIL_TYPES_H
typedef uint64_t    u64;
#endif
#ifndef XBASIC_TYPES_H
typedef uint32_t    u32;
typedef uint16_t    u16;
typedef uint8_t     u8;
#endif

// Endianness of the CPU
#if (XPAR_MICROBLAZE_ENDIANNESS != 0) || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif
#else
#ifndef BIG_ENDIAN
#define BIG_ENDIAN
#endif
#endif

// GenICam SFNC PtpControl features
typedef struct {
    uint8_t     clockAccuracy;
    uint8_t     status;
    uint8_t     servoStatus;
    int64_t     offsetFromMaster;
    int64_t     meanPathDelay;
    uint64_t    clockId;
    uint64_t    parentClockId;
    uint64_t    grandmasterClockId;
} sfnc_ptp_t;


// ---- GigE library global control --------------------------------------------

// Global variables
extern uintptr_t gige_base_addr;
extern u8  gige_reg_bank;
extern u8  gige_phy_access;
extern u8  gige_phy_user;

// I2C access modes for EEPROM and sensor
extern u8  eeprom_access_mode;
extern u8  sensor_access_mode;

// Stream channel packet size margins
extern const u32 SCPS_MIN;
extern const u32 SCPS_MAX;
extern const u32 SCPS_INC;

// User supplied link status
#define PHY_USER_OFF    0x00
#define PHY_USER_DOWN   0x01
#define PHY_USER_MASK   0x02
#define PHY_USER_UP     0x03

// PHY access modes
#define PHY_ACC_COMMON  0x01
#define PHY_ACC_MULTI   0x02

// Types of the Ethernet PHYs
#define PHY_BROADCOM    0x00
#define PHY_MARVELL     0x01
#define PHY_NATIONAL    0x02
#define PHY_MICREL      0x03
#define PHY_10G_AEL2005 0x04
#define PHY_10G_TN80XX  0x05
#define PHY_10G_PCS_PMA 0x06
#define PHY_1G_PCS_PMA  0x07
#define PHY_TI          0x08
#define PHY_NBASET_MRVL 0x09
#define PHY_NBASET_AQR  0x0A
#define PHY_SWITCH_MRVL 0x0B
#define PHY_25G_PCS_PMA 0x0C
#define PHY_10G_IP_CORE 0x0D
#define PHY_ANALOG      0x0E
#define PHY_NBASET_BCM  0x0F
#define PHY_25G_IP_CORE 0x10
#define PHY_REALTEK     0x11
#define PHY_NONE        0xFF

// Ethernet MAC NBASE-T link speed and interface mode constants
#define NBASET_USXGMII  0x80000000
#define NBASET_SPD_MASK 0x0000000F
#define NBASET_SPD_10G  0
#define NBASET_SPD_5G   1
#define NBASET_SPD_2500 3
#define NBASET_SPD_1000 9
// ... aliases
#define NBASET_SPD_10000    NBASET_SPD_10G
#define NBASET_SPD_5000     NBASET_SPD_5G

// Device modes (transmitter, receiver, non-streaming)
#define DEV_MODE_TX     0x00
#define DEV_MODE_RX     0x01
#define DEV_MODE_NSTM   0x02
#define DEV_MODE_MULTI  0xFF

// I2C devices access modes
#define I2C_ACC_NONE    0x00
#define I2C_ACC_COMMON  0x01
#define I2C_ACC_LARGE   0x02
#define I2C_ACC_MULTI   0xFF

// Event IDs for libgige user firmware callback
#define LIB_EVENT_NONE                  0x00000000
#define LIB_EVENT_GVCP_CONFIG_WRITE     0x00000001
#define LIB_EVENT_STREAM_OPEN_CLOSE     0x00000002
#define LIB_EVENT_SCCFG_WRITE           0x00000003
#define LIB_EVENT_APP_DISCONNECT        0x00000004
#define LIB_EVENT_LINK_DOWN             0x00000005
#define LIB_EVENT_LINK_CONFIG_WRITE     0x00000006
#define LIB_EVENT_SCHEDULED_ACTION      0x00000007

// Physical link configuration capability bits
#define LINK_CONFIG_CAP_MASK            0x0000000F
#define LINK_CONFIG_CAP_SL              0x00000001
#define LINK_CONFIG_CAP_ML              0x00000002
#define LINK_CONFIG_CAP_SLAG            0x00000004
#define LINK_CONFIG_CAP_DLAG            0x00000008

// Physical link configuration options
#define LINK_CONFIG_MASK                0x00000003
#define LINK_CONFIG_SL                  0
#define LINK_CONFIG_ML                  1
#define LINK_CONFIG_SLAG                2
#define LINK_CONFIG_DLAG                3

// PTP operation modes
#define PTP_MODE_FULL                   0
#define PTP_MODE_SLAVE                  1

// Trigger types for action_trigger()
#define TRIGGER_UNKNOWN         0x00000000
#define TRIGGER_IMMEDIATE       0x00000001
#define TRIGGER_SCHEDULED       0x00000002
#define TRIGGER_PAST            0x00000003


// ---- GigE Vision status codes -----------------------------------------------

#define GEV_STATUS_SUCCESS              0x0000
#define GEV_STATUS_NOT_IMPLEMENTED      0x8001
#define GEV_STATUS_INVALID_PARAMETER    0x8002
#define GEV_STATUS_INVALID_ADDRESS      0x8003
#define GEV_STATUS_WRITE_PROTECT        0x8004
#define GEV_STATUS_BAD_ALIGNMENT        0x8005
#define GEV_STATUS_ACCESS_DENIED        0x8006
#define GEV_STATUS_BUSY                 0x8007
#define GEV_STATUS_LOCAL_PROBLEM        0x8008
#define GEV_STATUS_MSG_MISMATCH         0x8009
#define GEV_STATUS_INVALID_PROTOCOL     0x800A
#define GEV_STATUS_NO_MSG               0x800B
#define GEV_STATUS_PACKET_UNAVAILABLE   0x800C
#define GEV_STATUS_DATA_OVERRUN         0x800D
#define GEV_STATUS_INVALID_HEADER       0x800E
#define GEV_STATUS_NO_REF_TIME          0x8013
#define GEV_STATUS_OVERFLOW             0x8015
#define GEV_STATUS_ACTION_LATE          0x8016
#define GEV_STATUS_ERROR                0x8FFF

// Device-Specific Status Code
#define GEV_STATUS_CAMERA_NOT_INIT                0xC001
#define GEV_STATUS_CAMERA_ALWAYS_INIT             0xC002
#define GEV_STATUS_CANNOT_CREATE_SOCKET           0xC003
#define GEV_STATUS_SEND_ERROR                     0xC004
#define GEV_STATUS_RECEIVE_ERROR                  0xC005
#define GEV_STATUS_CAMERA_NOT_FOUND               0xC006
#define GEV_STATUS_CANNOT_ALLOC_MEMORY            0xC007
#define GEV_STATUS_TIMEOUT                        0xC008
#define GEV_STATUS_SOCKET_ERROR                   0xC009
#define GEV_STATUS_INVALID_ACK                    0xC00A
#define GEV_STATUS_CANNOT_START_THREAD            0xC00B
#define GEV_STATUS_CANNOT_SET_SOCKET_OPT          0xC00C
#define GEV_STATUS_CANNOT_OPEN_DRIVER             0xC00D
#define GEV_STATUS_HEARTBEAT_READ_ERROR           0xC00E
#define GEV_STATUS_EVALUATION_EXPIRED             0xC00F
#define GEV_STATUS_GRAB_ERROR                     0xC010
#define GEV_STATUS_DRIVER_READ_ERROR              0xC011
#define GEV_STATUS_XML_READ_ERROR                 0xC012
#define GEV_STATUS_XML_OPEN_ERROR                 0xC013
#define GEV_STATUS_XML_FEATURE_ERROR              0xC014
#define GEV_STATUS_XML_COMMAND_ERROR              0xC015
#define GEV_STATUS_GAIN_NOT_SUPPORTED             0xC016
#define GEV_STATUS_EXPOSURE_NOT_SUPPORTED         0xC017
#define GEV_STATUS_CANNOT_GET_ADAPTER_INFO        0xC018
#define GEV_STATUS_ERROR_INVALID_HANDLE           0xC019
#define GEV_STATUS_CLINK_SET_BAUD                 0xC01A
#define GEV_STATUS_CLINK_SEND_BUFFER_FULL         0xC01B
#define GEV_STATUS_CLINK_RECEIVE_BUFFER_NO_DATA   0xC01C
#define GEV_STATUS_FEATURE_NOT_AVAILABLE          0xC01D
#define GEV_STATUS_MATH_PARSER_ERROR              0xC01E
#define GEV_STATUS_FEATURE_ITEM_NOT_AVAILABLE     0xC01F
#define GEV_STATUS_NOT_SUPPORTED                  0xC020
#define GEV_STATUS_GET_URL_ERROR                  0xC021
#define GEV_STATUS_READ_XML_MEM_ERROR             0xC022
#define GEV_STATUS_XML_SIZE_ERROR                 0xC023
#define GEV_STATUS_XML_ZIP_ERROR                  0xC024
#define GEV_STATUS_XML_ROOT_ERROR                 0xC025
#define GEV_STATUS_XML_FILE_ERROR                 0xC026
#define GEV_STATUS_DIFFERENT_IMAGE_HEADER         0xC027
#define GEV_STATUS_XML_SCHEMA_ERROR               0xC028
#define GEV_STATUS_XML_STYLESHEET_ERROR           0xC029
#define GEV_STATUS_FEATURE_LIST_ERROR             0xC02A
#define GEV_STATUS_ALREADY_OPEN                   0xC02B
#define GEV_STATUS_TEST_PACKET_DATA_ERROR         0xC02C
#define GEV_STATUS_FEATURE_NOT_FLOAT              0xC02D


// ---- GigE Vision events -----------------------------------------------------

#define GEV_EVENT_TRIGGER               0x0002
#define GEV_EVENT_START_OF_EXPOSURE     0x0003
#define GEV_EVENT_END_OF_EXPOSURE       0x0004
#define GEV_EVENT_START_OF_TRANSFER     0x0005
#define GEV_EVENT_END_OF_TRANSFER       0x0006
#define GEV_EVENT_MAX                   GEV_EVENT_END_OF_TRANSFER

#define GEV_EVENT_ERROR                 0x8001
//      GEV_EVENT_ERROR:   Error codes identical to status codes 0x8001 - 0x8FFF
#define GEV_EVENT_DEVSPEC               0x9000
//      GEV_EVENT_DEVSPEC: Device-specific error codes 0x9000 - 0xFFFF


// ---- GigE Vision pixel formats ----------------------------------------------

#define GVSP_PIX_MONO8                  0x01080001
#define GVSP_PIX_MONO8_SIGNED           0x01080002
#define GVSP_PIX_MONO10                 0x01100003
#define GVSP_PIX_MONO10_PACKED          0x010C0004
#define GVSP_PIX_MONO12                 0x01100005
#define GVSP_PIX_MONO12_PACKED          0x010C0006
#define GVSP_PIX_MONO14                 0x01100025
#define GVSP_PIX_MONO16                 0x01100007
#define GVSP_PIX_BAYGR8                 0x01080008
#define GVSP_PIX_BAYRG8                 0x01080009
#define GVSP_PIX_BAYGB8                 0x0108000A
#define GVSP_PIX_BAYBG8                 0x0108000B
#define GVSP_PIX_BAYGR10                0x0110000C
#define GVSP_PIX_BAYRG10                0x0110000D
#define GVSP_PIX_BAYGB10                0x0110000E
#define GVSP_PIX_BAYBG10                0x0110000F
#define GVSP_PIX_BAYGR12                0x01100010
#define GVSP_PIX_BAYRG12                0x01100011
#define GVSP_PIX_BAYGB12                0x01100012
#define GVSP_PIX_BAYBG12                0x01100013
#define GVSP_PIX_RGB8_PACKED            0x02180014
#define GVSP_PIX_BGR8_PACKED            0x02180015
#define GVSP_PIX_RGBA8_PACKED           0x02200016
#define GVSP_PIX_BGRA8_PACKED           0x02200017
#define GVSP_PIX_RGB10_PACKED           0x02300018
#define GVSP_PIX_BGR10_PACKED           0x02300019
#define GVSP_PIX_RGB12_PACKED           0x0230001A
#define GVSP_PIX_BGR12_PACKED           0x0230001B
#define GVSP_PIX_RGB10V1_PACKED         0x0220001C
#define GVSP_PIX_RGB10V2_PACKED         0x0220001D
#define GVSP_PIX_YUV411_PACKED          0x020C001E
#define GVSP_PIX_YUV422_PACKED          0x0210001F
#define GVSP_PIX_YUV444_PACKED          0x02180020
#define GVSP_PIX_RGB8_PLANAR            0x02180021
#define GVSP_PIX_RGB10_PLANAR           0x02300022
#define GVSP_PIX_RGB12_PLANAR           0x02300023
#define GVSP_PIX_RGB16_PLANAR           0x02300024
#define GVSP_PIX_RGB565p                0x02100035
#define GVSP_PIX_BGR565p                0x02100036


// ---- Acquisition modes as defined in XML file -------------------------------

#define ACQ_MODE_CONTINUOUS             0x00000001


// ---- Debug info verbosity levels --------------------------------------------

#define DBG_QUIET       0
#define DBG_NORMAL      1
#define DBG_ICMP        2
#define DBG_VERBOSE     3


// ---- Prototypes of public functions -----------------------------------------

// Main control
void gige_init(u8 idx, uintptr_t base_addr, u8 dev_mode, u32 bus_clk_freq, u8 phy_type, u8 phy_addr, u32 phy_mdc_freq, u8 data_rate, u16 eth_mtu, u8 verbosity);
int  gige_callback(u8 idx);
void gige_switch(u8 idx);
void gige_send_message(u16 event, u16 channel, u16 data_len, u8 *data, u64 *msg_timestamp);

// Advanced parameters access
void gige_set_params(u8 uart_bypass, u8 heartbeat_en, u8 payload_type);
void gige_get_params(u8 *uart_bypass, u8 *heartbeat_en, u8 *payload_type);
int  gige_set_stmdir(u32 channel, int dir_rx);
int  gige_get_stmdir(u32 channel);
void gige_set_gev_version(u8 ver);
u8   gige_get_gev_version();
void gige_force_gev_version(u32 ver);
void gige_set_sernum(char *sn);
void gige_set_multipart_support(u8 en);
u8   gige_get_multipart_support();
void gige_set_gendc_support(u8 en);
u8   gige_get_gendc_support();
void gige_set_resend_support(u8 en);
u8   gige_get_resend_support();
void gige_set_data_rates(u32 stm_tx_freq, u32 eth_rate);
void gige_set_acquisition_status(u32 channel, u32 status);
u32  gige_get_acquisition_status(u32 channel);
void gige_set_sceba(u32 channel, u32 address);
u32  gige_get_sceba(u32 channel);
void gige_set_scmbs(u32 channel, u64 mbs);
u64  gige_get_scmbs(u32 channel);
int  gige_set_action_numsig(u32 num);
u32  gige_get_action_numsig();
int  gige_set_action_qsize(u32 size);
u32  gige_get_action_qsize();
void gige_set_link_config_cap(u32 capability);
u32  gige_get_link_config_cap();
void gige_set_link_config(u32 configuration);
u32  gige_get_link_config();
void gige_set_phy_base(u32 phy_base);
u64  gige_get_stm_src_mac(u32 channel);
void gige_set_stm_src_mac(u32 channel, u64 mac);
u32  gige_get_stm_src_ip(u32 channel);
void gige_set_stm_src_ip(u32 channel, u32 ip);
u16  gige_get_stm_src_port(u32 channel);
void gige_set_stm_src_port(u32 channel, u16 port);
u32  gige_get_stm_src_cfg();
void gige_set_stm_src_cfg(u32 val);

// Authentication and licensing
uint8_t  gige_get_auth_status();
uint32_t gige_get_license_checksum();
int      gige_get_license_hash(uint8_t *hash);

// Interrupts
u32  gige_get_int_status(void);
void gige_clr_int_req(void);
u32  gige_get_int_mask(void);
void gige_set_int_mask(u32 mask);

// Console output functions
void print_mac(const char *txt, u64 mac);
void print_ip(const char *txt, u32 ip);
void print_speed(const char *txt);
void print_setup(void);
void gige_print_header(void);

// Ethernet PHY access
u16  get_phy_reg(u8 reg);
void set_phy_reg(u8 reg, u16 val);
u16  read_phy_reg(u8 reg);
void set_phy_addr(u8 reg, u16 addr);
u16  mdio_read(u8 reg, u16 addr);
void mdio_write(u8 reg, u16 addr, u16 val);
int  mdio_wait(uint32_t timeout);

// I2C access
u32  i2c_err(void);
// ... EEPROM
void eeprom_write_byte(u16 address, u8 value);
u8   eeprom_read_byte(u16 address);
void eeprom_write_word(u16 address, u16 value);
u16  eeprom_read_word(u16 address);
void eeprom_write_dword(u16 address, u32 value);
u32  eeprom_read_dword(u16 address);
// ... sensor with 8b address
void sensor_write_byte(u8 address, u8 value);
u8   sensor_read_byte(u8 address);
void sensor_write_word(u8 address, u16 value);
u16  sensor_read_word(u8 address);
// ... sensor with 16b address
void sensor16_write_byte(u16 address, u8 value);
u8   sensor16_read_byte(u16 address);
void sensor16_write_word(u16 address, u16 value);
u16  sensor16_read_word(u16 address);
// ... generic device one byte access
void i2c_write_byte(u8 dev, u8 val);
u8   i2c_read_byte(u8 dev);
void i2c_write_byte_addr(u8 dev, u8 addr, u8 val);

// General networking
void ethernet_header(u16 eth_type, u64 destination);
void ip_header(u16 data_len, u8 protocol, u32 src_addr, u32 dest_addr);
void udp_header(u16 data_len, u16 src_port, u16 dest_port);
void telnet_send(u32 len, u8 *data);
 
// IEEE 1588 PTP
uint64_t ptp_gev_time();
void     ptp_set_time(uint64_t seconds, uint32_t nanoseconds);
uint8_t  ptp_get_state_num();
int64_t  ptp_get_mean_path_delay();
int64_t  ptp_get_offset_from_master();
void     ptp_get_sfnc_ptpcontrol(sfnc_ptp_t *data);
void     ptp_set_mode(uint8_t mode);
uint8_t  ptp_get_mode();
void     ptp_set_utc_offset_en(int enable);
int      ptp_get_utc_offset_en();
int16_t  ptp_get_utc_offset();
void     ptp_get_control_params(uint64_t *step_corr_div, uint32_t *offset_margin_h, uint32_t *offset_margin_l, int64_t *delay_iir_coeff, int16_t *leap_seconds);
void     ptp_set_control_params(uint64_t  step_corr_div, uint32_t  offset_margin_h, uint32_t  offset_margin_l, int64_t  delay_iir_coeff, int16_t  leap_seconds);

// Delay functions
u64 gige_now();
int gige_timeout(u64 start, u64 timeout);
u32 gige_random(u32 min, u32 max);
#if defined __MICROBLAZE__
    unsigned int usleep(unsigned int useconds);
    unsigned int sleep(unsigned int seconds);
#elif defined __PPC__
#   include <sleep.h>
#elif defined __NIOS2__
#   include <unistd.h>
#elif defined __arm__  && !defined(__ALTSOC__)
#   include <unistd.h>
#elif defined __aarch64__  && !defined(__ALTSOC__)
#   include <unistd.h>
#endif

#endif // #if !defined (MODE_GIGE_10G)

#endif // _GIGE_H_

// eof
