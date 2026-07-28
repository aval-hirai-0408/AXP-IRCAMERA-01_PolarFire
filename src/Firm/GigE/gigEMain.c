/******************************************************************************/
/*  GigE Vision Core Firmware                                                 */
/*----------------------------------------------------------------------------*/
/*    File :  main.c                                                          */
/*    Date :  2014-09-30                                                      */
/*     Rev :  0.1                                                             */
/*  Author :  JP                                                              */
/*----------------------------------------------------------------------------*/
/*  GigE Vision reference design firmware                                     */
/*----------------------------------------------------------------------------*/
/*  0.1  |  2014-09-30  |  MAS |  Initial release                             */
/*  0.1b |  2014-11-27  |  MM  |  with Marvel                                 */
/******************************************************************************/

#if defined (IF_GIGE)

#if 1
#include <stdio.h>			// fopen_s, etc...
#include <stdlib.h>			// mbstowcs_s, etc...
#include <math.h>			// floorf, etc...
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../Common/common.h"

#if ((CPU_MODE == 1) || ((CPU_MODE == 0) && defined (AXP_ABA003IR_04)))

//#include "platform.h"
#include <xparameters.h>
#include <xil_cache.h>
#include <xuartps_hw.h>
#include <xil_printf.h>
#include <xil_exception.h>
#include <xil_io.h>
#include <xil_mmu.h>

// For GEV
#include "gige.h"
#include "framebuf.h"
#include "user.h"
#include "flash.h"
#include "GEV.h"
#if defined (MODE_GIGE_10G)
#include "phy.h"
#endif


#if defined (MODE_GIGE_10G)

// Operate in GEV 2.x version (required for PTP)
#define _GEV_VERSION_2_

#endif

//----------------------------------------------------------------------------------
// ARM1 Version string
//      Note: The string has to be changed by ARM1 update's timing
//----------------------------------------------------------------------------------
//static char* CAMERA_VERSION_STR = GIGE_CAMERA_VERSION;
static char* CAMERA_VERSION_STR = MAIN_VERSION;
static char* ARM1_VERSION_STR   = GIGE_ARM1_VERSION;

// ---- Global constants and variables -----------------------------------------

// I2C device addresses
//const u8  GIGE_PHY_ADDR         = 0x19;     // Ethernet Broadcom PHY -- SN52525 Board
const u8  GIGE_PHY_ADDR         = 0x00;     // Ethernet marvel PHY -- Board
const u8  I2C_DEV_EEPROM        = 0xA0;     // 24C64 I2C EEPROM
const u8  I2C_DEV_SENSOR        = 0xB8;     // MT9V022 CMOS image sensor (not used)

// Location of the shared external memory within address space
const u32 MPMC_NUM_INSTANCES    = 0;
const u32 MPMC_BASEADDR         = 0;
const u32 MPMC_HIGHADDR         = 0;

// ---- Global variables for DeviceFirmwareVersion -----------------------------
char boardVer[]={0};  // For Board
char fpgaVer[]={0};    // For FPGA
char firmVer[]={0};    // For ARM0 Version string
char firmUpdate[]={0};   // For ARM1 Version string
char deviceFirmwareVersion[]={0};     // For ALL instances
char deviceManufacturerInfo[]={0};    // For ALL instances
char deviceVendorName[]={0};          // For ALL instances
char deviceModelName[]={0};           // For ALL instances
char deviceUserID[]={0};              // For DeviceUserID
char deviceVersion[]={0};             // For DeviceVersion

volatile u32 g_dipsw = 0;             // DIPSW
volatile u32 g_DeviceResetRetryFor100Mbps = 0;

//volatile void* framebuffer = 0;

//----------------------------------------------------------------------------------
// GigE EEPROM Default Data
//----------------------------------------------------------------------------------
u8  GIGE_MANUF[32]        = "\0";
u8  GIGE_MODEL[32]        = "\0";
u8  GIGE_MINFO[48]        = "\0";
//const u8  GIGE_MANUF[32]        = "AVAL DATA CORPORATION\0";
//const u8  GIGE_MODEL[32]        = "ABA-001IR-GE\0";
//const u8  GIGE_MINFO[48]        = "Short Wave Infrared Camera for GigE Vision\0";
u8  GIGE_DVER [16]        = {0};  // For ARM1

const char GIGE_NULL=                   {'\0'}; 					// The character of null termination
const char GIGE_ZERO=                   {'0'}; 						// The character of null termination
const char GIGE_FILL=                   0xFF; 						// The character of null termination
const unsigned short GIGE_FILL16=       0xFFFF;		 				// The character of null termination
const unsigned int   GIGE_FILL32=       0xFFFFFFFF; 				// The character of null termination

//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
extern unsigned int gTickCountTotalTempCheck;
extern unsigned int gTickCountTotalAutoBright;
#endif //#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)


// Unused simulated EEPROM image
volatile u8 *EEPROM;

volatile u32 timer_log_us = 0;
volatile u32 timer_log_us_prev = 0;
// Sets up the GigE Vision EEPROM data
//No use    int init_gige (unsigned char isClreaAll);

#endif // ((CPU_MODE == 1) || ((CPU_MODE == 0) && defined (AXP_ABA003IR_04))

#if (CPU_MODE == 1)
int main()
{
	u32 status = 0,value=0;
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	int startMode;
#endif
	unsigned int timeout;
	unsigned int data32;
	int cpu1RunnningMode;

    // LED1 for initializing the ethernet....
	// e.g., int ledLinkWaitState (void)
	setLed (LED_NUM0, LED_DISCONNECTION); // Detection of Connection in progress
	setLed (LED_NUM1, LED_OFF);


	//------------------------------------------------------------
	// CPU0起動待ち
	//------------------------------------------------------------
	usleep(1000000);

	for (timeout=0; timeout<PROGRAM_BOOT_TIMEOUT_CPU1; timeout++)
	{
		data32 = IN32(FPGA_SHARED_MEMORY_BOOT0_ADRS);
		if (data32 == FPGA_SHARED_MEMORY_BOOT_START_MARK)
			break;

		usleep(1000);
	}

	if (timeout >= PROGRAM_BOOT_TIMEOUT_CPU1)
		OUT32 (FPGA_SHARED_MEMORY_BOOT0_ADRS, FPGA_SHARED_MEMORY_BOOT_ERROR_MARK);
	else
		OUT32 (FPGA_SHARED_MEMORY_BOOT0_ADRS, FPGA_SHARED_MEMORY_BOOT_END_MARK);


	// Memory Mapping at 0x3FC00000 to 0x3FCFFFFF as 1MB of out of memory management region on Xilinx standalone OS
	map ();

	// HW including SPI EEPROM, QSPI Flash, AXI QSPI , UART and I2C are initialized.
	hwInitialize (CPU_MODE);

    // Gets the bits of DIP-switch on the rear-panel.
    status = getDipsw((unsigned int*)&g_dipsw);

	//	ConSole ON=1/OFF=0
	consoleSetFlag (DEBUG_MODE_MSG);

	// Get manufacturer information from the eeprom on the ARM0
	user_info_get((u8*)GIGE_MANUF,(u8*)GIGE_MODEL,(u8*)GIGE_MINFO);

	// Gets the version on the FPGA and boards
	boardVersion (boardVer); //boardVer[BOARD_VERSION_SIZE]=';';// 基板バージョン取得
	fpgaVersion (fpgaVer);   //fpgaVer[FPGA_VERSION_SIZE]=';';// FPGAバージョン取得
	firmVersion (firmVer);   //firmVer[FIRM_VERSION_SIZE]=';';// AORM0 バージョン取得

	// Make the version string of the camera
	//memcpy(deviceVendorName,GIGE_MANUF,GIGE_EEPROM_CUST_VENDOR_SIZE);// ベンダ名
	//memcpy(deviceModelName,GIGE_MODEL,GIGE_EEPROM_CUST_MODEL_SIZE);// モデル名
	//memcpy(deviceManufacturerInfo,GIGE_MINFO,GIGE_EEPROM_CUST_MANUFACTURER_SIZE);// ベンダ情報
	strcpy((char*)deviceVendorName,(char*)GIGE_MANUF);// ベンダ名
	strcpy((char*)deviceModelName,(char*)GIGE_MODEL);// モデル名
	strcpy((char*)deviceManufacturerInfo,(char*)GIGE_MINFO);// ベンダ情報

#if 0
	sprintf(deviceVersion,"%s\0",CAMERA_VERSION_STR);// CAMERA全体 バージョン決定  used from user.h
	sprintf(deviceFirmwareVersion,"%s;%s;%s;%s\0",ARM1_VERSION_STR ,firmVer,fpgaVer,boardVer);
	//sprintf(deviceFirmwareVersion,"%s;%s%s%s\0",ARM1_VERSION_STR ,firmVer,fpgaVer,CAMERA_VERSION_STR);
	sprintf((char*)GIGE_DVER,"%s\0",(const char*)deviceVersion);
#else
	
	sprintf(deviceVersion,"%s",CAMERA_VERSION_STR);// CAMERA全体 バージョン決定  used from user.h
	sprintf(deviceFirmwareVersion,"%s;%s;%s;%s",ARM1_VERSION_STR ,firmVer,fpgaVer,boardVer);
	//sprintf(deviceFirmwareVersion,"%s;%s%s%s\0",ARM1_VERSION_STR ,firmVer,fpgaVer,CAMERA_VERSION_STR);
	sprintf((char*)GIGE_DVER,"%s",(const char*)deviceVersion);

#endif

#if !defined (ULTRA_SCALE_PLUS)
	// Memory Mapping at 0x3FC00000 to 0x3FCFFFFF as 1MB of out of memory management region on Xilinx standalone OS
	map ();
#endif

	// CPU1 Boot Flag
	OUT32 (FIRM_DATA_CPU1_BOOT_FLAG, 0);

	eeprom_access_mode = I2C_ACC_COMMON;
	// Warning: Don't access below these I/O by ARM1！

	// Initiates the GigE Vision persist information MAC,IP,Gateway ... etc.
	//status = init_gige(1); // as clear the all eeprom area then initiates the default IP,MAC,Gateway,URL ....
	//status = init_gige(0);   // Initiates the all MAC information WITHOUT any writing just reading
	//if(status != 0)
	//{
	//	DEBUG_PRINT ("INIT GEV PERSIST was failed %d\n", status);
	//}

    // Quoted from GigE-Vision_core_spec_X-1.5.2.pdf
    // The gige_init(u8 idx, u32 base_addr, u8 dev_mode, u32 bus_clk_freq, u8 phy_type, u8
    // phy_addr, u32 phy_mdc_freq, u8 data_rate, u16 eth_mtu, u8 verbosity) function initializes one
    // GigE core, MAC, and PHY registers. Parameter idx is the instance number of the core. When
    // used only one core, this is zero. The base_addr parameter is base address of the EPC interface
    // the core is connected to.

//================================================================================
// GigE 1Gbps
//================================================================================
#if !defined (MODE_GIGE_10G)
	gige_init(
    		0,             // index
    		XPAR_EPC_0_PRH0_BASEADDR,  // base address of EPC converter  (0x80600000) // GigE Core and PHY register via IPs
    		DEV_MODE_TX,   // device transmitter
    		125000000,     // Bus frequency
    		PHY_MARVELL,   // MAC PHY type
    		//PHY_BROADCOM,// MAC PHY type
            GIGE_PHY_ADDR, // Phy address as 0x00
            2500000,       // MDC frequency
            0,             // date-rate No use.
            SCPS_MAX,      // MTU
            //DBG_VERBOSE);// verbosity and internal debug info
		    //DBG_ICMP);   // verbosity with ping
		    DBG_NORMAL);   // verbosity with normal

    // Initiates the frame-buffer of GigE IP core
   framebuffer = framebuf_init(1, 0, 0x04000000);

    // Initiates the ABA camera
    user_init(&status);
    if(status != 0)
    {
    	value = IN32 (BOARD_STATUS_GEV_INIT_ADRS);
    	OUT32 (BOARD_STATUS_GEV_INIT_ADRS, (value+1));
    }

#if DEBUG_MODE_MSG
    // Print device information to std_out
    gige_print_header();

    // Print framebuffer information to std_out
    //framebuf_printregs();

    // Shows the information about register of GigE Bootstrap registers
    //gigeOutputBootstrap();

    // Shows the information about the eeprom of GigE Bootstrap registers
    gigeOutputEeprom();
#endif //DEBUG_MODE_MSG


//================================================================================
// GigE 10Gbps
//================================================================================
#else // #if !defined (MODE_GIGE_10G)

	msDelay (2000);

	// Set EEPROM access mode
	// - I2C_ACC_NONE   = no EEPROM device available, use emulated EEPROM
	// - I2C_ACC_COMMON = use physical EEPROM (eeprom.c is not needed)
	eeprom_access_mode = I2C_ACC_COMMON;

	//------------------------------------------------------------
	// Initialize (X)GigE core
	//------------------------------------------------------------
	#if defined (MODE_MULTI_STREAM)
	gige_init(0, XPAR_M1_AXI_GEV_BASEADDR, DEV_MODE_MULTI, 156250000, PHY_NBASET_MRVL, 0x00, 8000000, 0, SCPS_MAX, DBG_NORMAL);
	#else
	gige_init(0, XPAR_M1_AXI_GEV_BASEADDR, DEV_MODE_TX, 156250000, PHY_NBASET_MRVL, 0x00, 8000000, 0, SCPS_MAX, DBG_NORMAL);
	#endif

	//------------------------------------------------------------
	// Phy Driver Initialize
	//------------------------------------------------------------
	phyInitialize ();

	//------------------------------------------------------------
	// Check GigE IP Rx Align Status
	//------------------------------------------------------------
	msDelay (1000);
	gigEIpRxAlignCheckStatus ();

	//------------------------------------------------------------
	// GigE Initialize
	//------------------------------------------------------------
	gige_set_data_rates(300, 10000);                // f(tx_stm_clk) = 300MHz, 10Gbps Ethernet link
	//gige_set_data_rates(0, 0);                      // f(tx_stm_clk) = 300MHz, 10Gbps Ethernet link
	gige_set_link_config_cap(LINK_CONFIG_CAP_SL);   // Physical link configuration capabilities
	gige_set_link_config(LINK_CONFIG_SL);           // Current physical link configuration
	gige_set_sceba(0, MAP_SCEBA);                   // Set stream channel extended bootstrap address

	//------------------------------------------------------------
	// Set PTP operation mode
	//------------------------------------------------------------
#ifdef _PTP_SLAVE_ONLY_
	ptp_set_mode(PTP_MODE_SLAVE);
	ptp_set_utc_offset_en(1);
#endif

	// Initialize framebuffers and set GEV version
#ifndef _GEV_VERSION_2_
	gige_set_gev_version(1);                        // GEV 1.x
	framebuf_init(1, 0, 1, 0x04000000);             // Low latency, progressive-scan, GEV 1.x, buffer size
	framebuf_control &= ~FRAMEBUF_C_EXTSTAT;        // Disable extended GVSP status codes
	DEBUG_PRINT_FORCE("[UU] Running in GEV 1.2 mode\r\n"); 
#else
	gige_set_gev_version(2);                        // GEV 2.x
#if 0
	gige_force_gev_version(0x00020001);             // force GEV version to 2.1
#endif
	framebuf_init(1, 0, 0,0x04000000);              // Low latency, progressive-scan, GEV 2.x, buffer size
	framebuf_control |= FRAMEBUF_C_EXTSTAT;         // Enable extended GVSP status codes
	DEBUG_PRINT_FORCE("[UU] Running in GEV 2.x mode\r\n");
#endif

	//------------------------------------------------------------
	// Initialize design specific hardware
	//------------------------------------------------------------
	user_init(&status);
	if(status != 0)
	{
		value = IN32 (BOARD_STATUS_GEV_INIT_ADRS);
		OUT32 (BOARD_STATUS_GEV_INIT_ADRS, (value+1));
	}

	//------------------------------------------------------------
	// Print device information to std_out
	//------------------------------------------------------------
	gige_print_header();
    gigeOutputEeprom();
#endif // #if !defined (MODE_GIGE_10G)

	//------------------------------------------------------------
	// GigEメイン処理
	//------------------------------------------------------------
#if defined (ULTRA_SCALE_PLUS)
    DEBUG_PRINT_FORCE("CPU1 Start\n");

	// CPU1 Initialize Done
	OUT32 (FPGA_SHARED_MEMORY_BOOT1_ADRS, FPGA_SHARED_MEMORY_BOOT_START_MARK);
#endif

	// CPU1 Boot Flag
	OUT32 (FIRM_DATA_CPU1_BOOT_FLAG, 1);


   // Endless main program loop
    while (1)
    {
    	#if defined (ULTRA_SCALE_PLUS)
		//------------------------------------------------------------
		// CPU1 Runnning
		//------------------------------------------------------------
		cpu1GetRunning (&cpu1RunnningMode);
		if (cpu1RunnningMode == MODE_DISABLE)
    		continue;
		#if defined (MODE_GIGE_10G)
		if (cpu1RunnningMode == 2)
    	{
			//------------------------------------------------------------
			// ARM0→ARM1へのコマンド処理要求
			//------------------------------------------------------------
	    	//@@@1gigeCmdProcs ();
    		continue;
    	}
		#endif
    	#endif //#if defined (ULTRA_SCALE_PLUS)

		// ========================User Callback============
		// Checks the state of GigE Vision link-up
		//status = gige_get_int_status();
		status = gige_callback(0);   // Networking callback function
		user_callback();             // User callback


		// ====================== Link-state ================================
		// Indicates the state at LED of the rear-panel
		status = (u32)ledLinkState (status,&DeviceStatus,&ccpPrivilege);
		status = ledState (DeviceStatus);


        // =========== Built-in Test ================= Built-in Test ========
		if(DeviceBuiltInTest_Mode != DeviceBuiltInTest_Done)
		{

		}

#if defined (MODE_COOL_SYSTEM)
		// Link Status格納
		//if (status != DeviceStatus)
		//{
			//OUT32 (FIRM_DATA_GE_LINK_CHANGE_STATUS, 1);
			//coolSystemGetStatus (&coolStatus);
		//}

		if (IN32 (FIRM_DATA_CONSOLE_LOGIN_ADRS) == MODE_DISABLE)		// Console Login Status
		{
			// 冷却システムCase温度通知
			coolSystemHandler();
		}
#endif

		//------------------------------------------------------------
		// Auto Bright制御
		//------------------------------------------------------------
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
		if (IN32 (FPGA_AUTO_BRIGHT_TYPE_ADRS) == 1)
		{
			if (gTickCountTotalAutoBright > AUTO_BRIGHT_ADJUST_TIME_INTERVAL)
			{

				// Start Status
				acquisitionGetStartFlag (&startMode);

				//スタートしていたら自動露光/ゲイン実行
				if(startMode != 0)
				{
					autoBrightMain ();
				}
				// タイマクリア
				gTickCountTotalAutoBright = 0;
			}
		}
#endif

		//------------------------------------------------------------
		// ARM0→ARM1へのコマンド処理要求
		//------------------------------------------------------------
#if defined (MODE_GIGE_10G)
    	//@@@1gigeCmdProcs ();
#endif    	

    	//------------------------------------------------------------
		// DDR検査用アドレス取得
		//------------------------------------------------------------
#if defined (MODE_DIAG_DDR_VERSION2)
		cmdDiagDdrCpu1Malloc();
#endif
		//wfi ();	// sleep
    }

    // Never reached exit
    Xil_DCacheDisable();
    Xil_ICacheDisable();
    return 0;
}


#if defined (MODE_GIGE_10G)
//**********************************************************************************
//	GigE 再初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigEInitialize2 (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int value;
	unsigned int adrs;
	
	// Set EEPROM access mode
	// - I2C_ACC_NONE   = no EEPROM device available, use emulated EEPROM
	// - I2C_ACC_COMMON = use physical EEPROM (eeprom.c is not needed)
	eeprom_access_mode = I2C_ACC_COMMON;

	//------------------------------------------------------------
	// Initialize (X)GigE core
	//------------------------------------------------------------
	#if defined (MODE_MULTI_STREAM)
	gige_init(0, XPAR_M1_AXI_GEV_BASEADDR, DEV_MODE_MULTI, 156250000, PHY_NBASET_MRVL, 0x00, 8000000, 0, SCPS_MAX, DBG_NORMAL);
	#else
	gige_init(0, XPAR_M1_AXI_GEV_BASEADDR, DEV_MODE_TX, 156250000, PHY_NBASET_MRVL, 0x00, 8000000, 0, SCPS_MAX, DBG_NORMAL);
	#endif

	//------------------------------------------------------------
	// Phy Driver Initialize
	//------------------------------------------------------------
	//phyInitialize ();

	//------------------------------------------------------------
	// GigE Initialize
	//------------------------------------------------------------
	gige_set_data_rates(300, 10000);              // f(tx_stm_clk) = 300MHz, 10Gbps Ethernet link
	//gige_set_data_rates(0, 0);                      // f(tx_stm_clk) = 300MHz, 10Gbps Ethernet link
	gige_set_link_config_cap(LINK_CONFIG_CAP_SL);   // Physical link configuration capabilities
	gige_set_link_config(LINK_CONFIG_SL);           // Current physical link configuration
	gige_set_sceba(0, MAP_SCEBA);                   // Set stream channel extended bootstrap address

	//------------------------------------------------------------
	// Set PTP operation mode
	//------------------------------------------------------------
#ifdef _PTP_SLAVE_ONLY_
	ptp_set_mode(PTP_MODE_SLAVE);
	ptp_set_utc_offset_en(1);
#endif
	
    // Frame Buffer Address
    adrs = IN32 (FIRM_DATA_GIGE_FRAME_BUFF_ADRS);
	if (adrs != 0)
	{
		free ((void *)adrs);
		OUT32 (FIRM_DATA_GIGE_FRAME_BUFF_ADRS, 0);
	}
	

	// Initialize framebuffers and set GEV version
#ifndef _GEV_VERSION_2_
	gige_set_gev_version(1);                        // GEV 1.x
	framebuf_init(1, 0, 1, 0x04000000);             // Low latency, progressive-scan, GEV 1.x, buffer size
	framebuf_control &= ~FRAMEBUF_C_EXTSTAT;        // Disable extended GVSP status codes
	DEBUG_PRINT_FORCE("[UU] Running in GEV 1.2 mode\r\n"); 
#else
	gige_set_gev_version(2);                        // GEV 2.x
#if 0
	gige_force_gev_version(0x00020001);             // force GEV version to 2.1
#endif
	framebuf_init(1, 0, 0,0x04000000);              // Low latency, progressive-scan, GEV 2.x, buffer size
	framebuf_control |= FRAMEBUF_C_EXTSTAT;         // Enable extended GVSP status codes
	DEBUG_PRINT_FORCE("[UU] Running in GEV 2.x mode\r\n");
#endif

	//------------------------------------------------------------
	// Initialize design specific hardware
	//------------------------------------------------------------
	user_init(&status);
	if(status != 0)
	{
		value = IN32 (BOARD_STATUS_GEV_INIT_ADRS);
		OUT32 (BOARD_STATUS_GEV_INIT_ADRS, (value+1));
	}

	//------------------------------------------------------------
	// Print device information to std_out
	//------------------------------------------------------------
	gige_print_header();
    gigeOutputEeprom();

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if defined (MODE_GIGE_10G)

#endif
#endif

#endif // #if defined (IF_GIGE)

// eof
