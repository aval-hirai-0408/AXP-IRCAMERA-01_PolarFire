//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cxp.h - CXP Header
//**********************************************************************************

#ifndef CXP_H
#define CXP_H

//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define CXP_XML_URL_SIZE					(0x200)		// GIGE_EEPROM_XML_URL_SIZE

// Bit Rate
#define CXP_RATE_1_25G						(0x28)
#define CXP_RATE_2_50G						(0x30)
#define CXP_RATE_3_125G						(0x38)
#define CXP_RATE_5_00G						(0x40)
#define CXP_RATE_6_25G						(0x48)
#define CXP_RATE_10_00G						(0x50)
#define CXP_RATE_12_50G						(0x58)

// StreamID
#define CXP_STREAM_ID1_NUMBER				(0x00)
#define CXP_STREAM_ID2_NUMBER				(0x01)


//----------------------------------------------------------------------------------
// PolaFire Register
//----------------------------------------------------------------------------------
#define FPGA_CXP_PF_BASE_ADRS				(FPGA_CXP_PF_ADRS)

#define FPGA_CXP_PF_OFFSET					(0x400)
#define FPGA_CXP_PF_INTERVAL				(0x80)

#define FPGA_CXP_PF_DEV_OFFSET				(0x1400)
#define FPGA_CXP_PF_IP_OFFSET				(0x1800)
#define FPGA_CXP_PF_IP_INTERVAL				(0x1000)

#define CXP_REG_DATA_SIZE_MAX				(0x800)


//----------------------------------------------------------------------------------
// CoaXPress
//----------------------------------------------------------------------------------
#define CXP_REG_VERSION_ADRS				(FPGA_CXP_BASE_ADDR + 0x00)
#define CXP_REG_DRI_CTRLADRS				(FPGA_CXP_BASE_ADDR + 0x04)
	#define CXP_REG_DRI_CTRL_1_25G			(0x00)
	#define CXP_REG_DRI_CTRL_2_5G			(0x01)
	#define CXP_REG_DRI_CTRL_3_125G			(0x02)
	#define CXP_REG_DRI_CTRL_5G				(0x03)
	#define CXP_REG_DRI_CTRL_6_25G			(0x04)
	#define CXP_REG_DRI_CTRL_10_0G			(0x05)
	#define CXP_REG_DRI_CTRL_12_5G			(0x06)
	#define CXP_REG_DRI_CTRL_BUSY			(1<<31)
	#define CXP_REG_DRI_CTRL_BUSY_TIMEOUT	(1000)


//----------------------------------------------------------------------------------
// CXP Register
//----------------------------------------------------------------------------------
#define CXP_REG_CTRL_ADRS					(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_OFFSET + 0x00)
	#define CXP_REG_CTRL_FIFO_RESET			(1<<0)
	#define CXP_REG_CTRL_2PORT_2STREAM		(1<<8)
	#define CXP_REG_CTRL_PORT_DUAL			(1<<16)

	#define CXP_PORT_SINGLE					(1)
	#define CXP_PORT_DUAL					(2)

	#define CXP_MODE_SINGLE_STREAM			(1)
	#define CXP_MODE_MULTI_STREAM			(2)

#define CXP_REG_XSIZE_ADRS					(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_OFFSET + 0x40)
#define CXP_REG_XOFFSET_ADRS				(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_OFFSET + 0x44)
#define CXP_REG_YSIZE_ADRS					(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_OFFSET + 0x48)
#define CXP_REG_YOFFSET_ADRS				(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_OFFSET + 0x4c)
#define CXP_REG_DSIZEL_ADRS					(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_OFFSET + 0x50)
#define CXP_REG_PIXEL_L_ADRS				(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_OFFSET + 0x54)
	#define CXP_REG_PIXEL_MONO8				(0x0101)
	#define CXP_REG_PIXEL_MONO10			(0x0102)
	#define CXP_REG_PIXEL_MONO12			(0x0103)
#define CXP_REG_TAP_GEO_ADRS				(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_OFFSET + 0x58)
#define CXP_REG_IMG_FLG_ADRS				(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_OFFSET + 0x5C)
#define CXP_REG_STREAM_ID_ADRS				(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_OFFSET + 0x60)


//----------------------------------------------------------------------------------
// CXP Dev Register
//----------------------------------------------------------------------------------
#define CXP_REG_TEST_MODE_ADRS				(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_DEV_OFFSET + 0x00)
#define CXP_REG_TEST_MODE_TX_LOW_ADRS		(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_DEV_OFFSET + 0x04)
#define CXP_REG_TEST_MODE_TX_HIGH_ADRS		(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_DEV_OFFSET + 0x08)
#define CXP_REG_LED_ADRS					(FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_DEV_OFFSET + 0x20)


//----------------------------------------------------------------------------------
// CXP IP Register
//----------------------------------------------------------------------------------
#define CXP_REG_IP_STREAM1_ID_ADRS 			((FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_IP_OFFSET + 0x00 * 4))
#define CXP_REG_IP_STREAM2_ID_ADRS 			((FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_IP_OFFSET + 0x02 * 4))

#define FPGA_CXP_TX_CMD_CTRL_ADRS			((FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_IP_OFFSET + 0x10 * 4))
	#define FPGA_CXP_TX_CMD_END				(1<<0)
	#define FPGA_CXP_TX_CMD_START			(1<<1)
	#define FPGA_CXP_TX_CMD_DATA_HL			(1<<2)
	#define FPGA_CXP_TX_CMD_DATA_WRITE		(1<<8)

#define FPGA_CXP_TX_CMD_LOW_DATA_ADRS		((FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_IP_OFFSET + 0x11 * 4))
#define FPGA_CXP_TX_CMD_HIGH_DATA_ADRS 		((FPGA_CXP_PF_BASE_ADRS + FPGA_CXP_PF_IP_OFFSET + 0x12 * 4))


//----------------------------------------------------------------------------------
// Common
//----------------------------------------------------------------------------------
// Mode Parallel
#define MODE_PARALLEL

#define CXP_VERSION_10						(0x00010000)
#define CXP_VERSION_11						(0x00010001)
#define CXP_VERSION_20						(0x00020000)


// Cxp Version
#if defined (MODE_CXP_VERSION_20)
#define CXP_VERSION							(CXP_VERSION_20)
#else
#define CXP_VERSION							(CXP_VERSION_11)
#endif

// XML Version
#define CXP_XML_VERSION						(0x100000)

// XML Schema Version
#define CXP_XML_SCHEMA_VERSION				(0x100000)

// Command パケット
#define CXP_K_CODE_K27_7					(0xfbfbfbfb)
#define CXP_K_CODE_K29_7					(0xfdfdfdfd)

// トリガパケット
#define CXP_K_CODE_K28_2_BYTE				(0x5c)
#define CXP_K_CODE_K28_4_BYTE				(0x9c)

// GPIOパケット
#define CXP_K_CODE_K28_0					(0x1c1c1c1c)

// IO Ackパケット
#define CXP_K_CODE_K28_6					(0xdcdcdcdc)

// IDLEパケット
#define CXP_K_CODE_K28_5					(0xbcbcbcbc)
#define CXP_K_CODE_K28_1					(0x3c3c3c3c)
#define CXP_K_CODE_D21_5					(0xb5b5b5b5)

#define CXP_IDLE_PACKET						(0xbc3c3cb5)

// Command Packet Data Type
#define CXP_DATA_PACKET_TYPE_RESERVED		(0x00000000)
#define CXP_DATA_PACKET_TYPE_STREAM			(0x01010101)
#define CXP_DATA_PACKET_TYPE_COMMAND		(0x02020202)
#define CXP_DATA_PACKET_TYPE_ACK			(0x03030303)
#define CXP_DATA_PACKET_TYPE_TEST			(0x04040404)
#define CXP_DATA_PACKET_TYPE_COMMAND_TAG	(0x05050505)
#define CXP_DATA_PACKET_TYPE_ACK_TAG		(0x06060606)

// Size Mask
#define CXP_CTRL_SIZE_MASK					(0x00ffffff)

// Command Mask & Shift
#define CXP_CTRL_CMD_MASK					(0xff000000)
#define CXP_CTRL_CMD_SHIFT					(24)
#define CXP_CTRL_CMD_READ					(0)
#define CXP_CTRL_CMD_WRITE					(1)

// Ack Code
#define CXP_ACK_CODE_OK						(0x00000000)	// OK返答データあり
#define CXP_ACK_CODE_OK_NO_REPLY			(0x01010101)	// OK返答データなし
#define CXP_ACK_CODE_WAIT					(0x04040404)	// Wait
#define CXP_ACK_CODE_INVALID_ADRS			(0x40404040)	// Invalid Address
#define CXP_ACK_CODE_INVALID_DATA			(0x41414141)	// Invalid Data
#define CXP_ACK_CODE_INVALID_CODE			(0x42424242)	// Invalid Operation Code
#define CXP_ACK_CODE_READ_ONLY				(0x43434343)	// Read Only
#define CXP_ACK_CODE_WRITE_ONLY				(0x44444444)	// Write Only
#define CXP_ACK_CODE_LARGE_SIZE				(0x45454545)	// Large Size
#define CXP_ACK_CODE_INCORRECT_SIZE			(0x46464646)	// Incorrect Size
#define CXP_ACK_CODE_MALFORMED_PACKET		(0x47474747)	// 不正パケット
#define CXP_ACK_CODE_CRC					(0x80808080)	// CRC Error

// Bit Rate Code
#define CXP_RATE_1_250GBPS					(0x28)
#define CXP_RATE_2_500GBPS					(0x30)
#define CXP_RATE_3_125GBPS					(0x38)
#define CXP_RATE_5_000GBPS					(0x40)
#define CXP_RATE_6_250GBPS					(0x48)
#define CXP_RATE_10_000GBPS					(0x50)
#define CXP_RATE_12_500GBPS					(0x58)

// Timeout
#define CXP_COMMAND_PACKET_TIMEOUT			(2500000)	// 5us

// Connection Test Size
#define CXP_CONNECTION_TEST_SIZE_BYTE		(4096)

// Send Data Offset
#define CXP_SEND_DATA_OFFSET				(8)

// Send Packet Fix Count
#define CXP_SEND_DATA_FIX_COUNT				(6)
#define CXP_SEND_DATA_FIX_NO_DATA_COUNT		(4)

// Send DMA Timeout
#define CXP_SEND_DMA_TIMEOUT				(3000)		// 3s

// Send Fifo Timeout
#define CXP_FIRO_SEND_TIMEOUT				(50000)		//5s

// Align
#define CXP_FPGA_ADRS_ALIGN					(0x8)
#define CXP_FPGA_ADRS_ALIGN_MASK			(CXP_FPGA_ADRS_ALIGN-1)

#endif // CXP_H
