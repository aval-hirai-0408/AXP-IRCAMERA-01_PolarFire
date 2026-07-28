//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// user.h - user Header
//**********************************************************************************

#ifndef _USER_H_
#define _USER_H_

#include "../Common/common.h"
#include "gev.h"
#include "../common/fpga.h"
#include <limits.h>

//----------------------------------------------------------------------------------
// 初期化エラー情報格納 Map
//----------------------------------------------------------------------------------
#if 0	//@@@1
#define BOARD_STATUS_GEV_ADRS				(BOARD_ERROR_ADRS+0x100)		// For GigE Vision Offset
#define BOARD_STATUS_GEV_MAC_ADRS			(BOARD_STATUS_GEV_ADRS+0x00)	// MAC error
#define BOARD_STATUS_GEV_INIT_ADRS			(BOARD_STATUS_GEV_ADRS+0x04)	// user_init error
#define BOARD_STATUS_GEV_LINK_ADRS			(BOARD_STATUS_GEV_ADRS+0x08)	// link-detection
#define BOARD_STATUS_GEV_DISL_ADRS			(BOARD_STATUS_GEV_ADRS+0x0C)	// link-disconnection error
#define BOARD_STATUS_GEV_CMDR_ADRS			(BOARD_STATUS_GEV_ADRS+0x10)	// user_get error
#define BOARD_STATUS_GEV_CMDW_ADRS			(BOARD_STATUS_GEV_ADRS+0x14)	// user_set error
#define BOARD_STATUS_GEV_USERSET_ADRS		(BOARD_STATUS_GEV_ADRS+0x18)	// user_set error
#define BOARD_STATUS_GEV_LUT_ADRS			(BOARD_STATUS_GEV_ADRS+0x1C)	// user_set error
#define BOARD_STATUS_GEV_FFC_ADRS			(BOARD_STATUS_GEV_ADRS+0x20)	// user_set error
#define BOARD_STATUS_GEV_DPC_ADRS			(BOARD_STATUS_GEV_ADRS+0x24)	// user_set error
#define BOARD_STATUS_GEV_AGING_ADRS			(BOARD_STATUS_GEV_ADRS+0x28)	// user_set error
#endif //@@@1

// ---- Global flat for isExecuteCommandDone -----------------------------
#define command_running (1)
#define command_done    (0)


//--------------------------------------------------------------------------------
// Function prototypes
//--------------------------------------------------------------------------------
int toG(int error);
u32  get_user_reg(u32 address, u16 *status);
void set_user_reg(u32 address, u32 value, u16 *status);
void user_init(u32* status);
void user_callback(void);
void user_network_init(u32* status);
void user_userid_init(u32* status);
void user_info_get(u8* strVendor,u8* strModel,u8* strManufacturer);


u32 executeCommand(u32 cmd,u32 sync,u32* buff32,u32 sizeOfarray);
u32 isExecuteCommandDone(u32 timeout,u32 cmd,u32* isDone);

int ledLinkState (u32 linkStatus,volatile u32* linkStatusPre,volatile u32* ccpPre);
void ledSetState(u32 type_of_led,u32 state, int status);
int ledState (u32 linkStatus);

int FileSelectorDPC(void);
int FileSelectorFFC (void);
int FileSelectorSpectrumWave (void);
int FileSelectorFpga (void);
int FileSelectorXML (void);
int FileSelectorIfFpgaWrite (void);
int FileSelectorPhyWrite (void);

u32 gige_event(u32 id, u32 param, __attribute__((unused)) void *data);
int gigeUpdateBuffer (void);

void gigeOutputEeprom();


//--------------------------------------------------------------------------------
// externs
//--------------------------------------------------------------------------------
#if 0//@@@1
// ---- Global variables for DeviceFirmwareVersion -----------------------------
extern char boardVer[BOARD_VERSION_SIZE+1]; // For Board
extern char fpgaVer[FPGA_VERSION_SIZE+1];   // For FPGA
extern char firmVer[FIRM_VERSION_SIZE+1];   // For ARM0
extern char firmUpdate[GIGE_EEPROM_CUST_UPDATE_TIMESTAMP_SIZE+1];   // For ARM0
extern char deviceFirmwareVersion[GIGE_EEPROM_CUST_FIRMWARE_VER_SIZE+1];      // For ALL instances
extern char deviceVendorName[GIGE_EEPROM_CUST_VENDOR_SIZE+1];              // For DeviceVendoerName
extern char deviceModelName[GIGE_EEPROM_CUST_MODEL_SIZE+1];                // For DeviceModelName
extern char deviceManufacturerInfo[GIGE_EEPROM_CUST_MANUFACTURER_SIZE];  // For DeviceManufacturerInfo
extern char deviceUserID[GIGE_EEPROM_USER_ID_SIZE+1];             // For DeviceUserID
extern char deviceVersion[GIGE_EEPROM_VERSION_SIZE+1];            // For DeviceVersion

extern volatile u8 version;                  // The version of GigE Vision standard
extern volatile u32 g_dipsw;                 // DIPSW
extern volatile u32 g_DeviceResetRetryFor100Mbps;    // Force to re-link up till 1Gbps...
#define DeviceResetRetryFor100Mbps_Max (10) // Max number of force to re-link up till 1Gbps...


// ---- Global variables for video IP of GigE Vision----------------------------
extern volatile u32 video_max_width;
extern volatile u32 video_max_height;
extern volatile u32 video_total_bpf;
extern volatile u32 video_acq_mode;
extern volatile u32 video_max_offgap;

// ---- Global variables for Device Control ------------------------------------
extern volatile u32 DeviceIndicatorMode_Selecotr;

// --  Global Device for  Built-in test variables -------------
extern volatile u32 DeviceBuiltInTest_Mode;
extern volatile u32 DeviceBuiltInTest_Status;

// ---- Global variables for XML URL -------------------------------------------
//extern volatile char XML_URL_FILE[64];
extern volatile u32  xmlStartAddress;
extern volatile u32  xmlSize;
extern volatile u8   xmlURL[512];
extern volatile u32  xmlStartAddressSecond;
extern volatile u32  xmlSizeSecond;
extern volatile u8   xmlURLSecond[512];

// ---- Global variables for status  -------------------------------------------
extern volatile u32  DeviceStatus;
extern volatile u32  ccpPrivilege;
#define DeviceStatus_LinkUndetection (0)
#define DeviceStatus_LinkDetection   (1)

// ---- Global variables for LED indicators  -------------------------------------------
extern volatile u32  DeviceIndicatorPower;
extern volatile u32  DeviceIndicatorStatus;

// ---- Global variables for File Access Control --------------------------------
extern volatile u32   fileSelector;                         // RW 4 bytes : IEnumeration UserSetDefault,UserSet1,UserSet2,LUTLuminance,
extern volatile u32   fileSel[FileSelector_MAX];                          // FileOperationSelector
extern volatile u32   fileExec[FileSelector_MAX];                         //
extern volatile u32   fileMode[FileSelector_MAX];                         //
extern volatile u32*  fileBuffer[FileSelector_MAX];                       //
extern volatile u32   fileOffset[FileSelector_MAX][FileOperationeSelector_MAX];     //
extern volatile u32   fileLength[FileSelector_MAX][FileOperationeSelector_MAX];     //
extern volatile u32   fileStatus[FileSelector_MAX][FileOperationeSelector_MAX];     //
extern volatile u32   fileResult[FileSelector_MAX][FileOperationeSelector_MAX];     //
extern volatile u32   fileSize[FileSelector_MAX];                         //


// ---- Global variables for framebuf.c --------------------------------
extern volatile void* framebuffer;

#endif//@@@1


//================================================================================
// GigE 10Gbps
//================================================================================

// ---- Macros and constants ---------------------------------------------------

// Special GEV memory areas address map
#define MAP_SCEBA           0x20000000
#define MAP_EEPROM          0xFBFF0000
#define MAP_EEPROM_LEN      0x00002000
#define MAP_FLASH           0xFE000000

// Video TPG clock frequency
#define VIDEO_CLK           200000000      // 200MHz Video clock

// ---- Video processor interface ----------------------------------------------

// Offsets of the registers
#define video_gcsr          (*(volatile u32 *)(VIDEO_REGS + 0x0000))
#define video_width         (*(volatile u32 *)(VIDEO_REGS + 0x0004))
#define video_height        (*(volatile u32 *)(VIDEO_REGS + 0x0008))
#define video_offs_x        (*(volatile u32 *)(VIDEO_REGS + 0x000C))
#define video_offs_y        (*(volatile u32 *)(VIDEO_REGS + 0x0010))
#define video_pixfmt        (*(volatile u32 *)(VIDEO_REGS + 0x0018))
#define video_gap_x         (*(volatile u32 *)(VIDEO_REGS + 0x001C))
#define video_gap_y         (*(volatile u32 *)(VIDEO_REGS + 0x0020))
#define video_gpio_in       (*(volatile u32 *)(VIDEO_REGS + 0x0024))
#define video_gpio_out      (*(volatile u32 *)(VIDEO_REGS + 0x0028))
#define video_chunk_ctrl    (*(volatile u32 *)(VIDEO_REGS + 0x002C))
#define video_chunkid_img   (*(volatile u32 *)(VIDEO_REGS + 0x0030))
#define video_chunkid_fc    (*(volatile u32 *)(VIDEO_REGS + 0x0034))
#define video_tpg_mode      (*(volatile u32 *)(VIDEO_REGS + 0x0038))

#endif

// eof
