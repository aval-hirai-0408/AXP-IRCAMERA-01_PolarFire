//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cxpMain.c - CXP Main Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "../GigE/user.h"
#include "../GigE/gev.h"
#include "../GigE/gige.h"
#include "cxp.h"


#if defined (IF_CXP)

//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
char deviceFirmwareVersion[32]={0};		// For ALL instances
char deviceUserID[32]={0};				// For DeviceUserID
char deviceVersion[32]={0};				// For DeviceVersion

// GigE EEPROM Default Data
unsigned char gXmlFileName1 [CXP_XML_URL_SIZE];
unsigned char gXmlFileName2 [CXP_XML_URL_SIZE];
unsigned char gXmlFileNameUpdate [CXP_XML_URL_SIZE];

int gLinkStatusCheck = 0;
int gCxpCmdProcessFlag = 0;
int gCxpIntProcsFlag = 0;
unsigned int gXMLSize = 0;

//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern unsigned int xmlStartAddress;
extern unsigned int xmlSize;
extern unsigned char xmlURL[];
extern unsigned int xmlStartAddressSecond;
extern unsigned int xmlSizeSecond;
extern unsigned char xmlURLSecond[];

// ---- Global variables for File Access Control --------------------------------
extern volatile u32 fileSelector;
extern volatile u32 fileSel[];
extern volatile u32 fileExec[];
extern volatile u32 fileMode[];
extern volatile u32* fileBuffer[];
extern volatile u32 fileOffset[FileSelector_MAX][FileOperationeSelector_MAX];
extern volatile u32 fileLength[FileSelector_MAX][FileOperationeSelector_MAX];
extern volatile u32 fileStatus[FileSelector_MAX][FileOperationeSelector_MAX];
extern volatile u32 fileResult[FileSelector_MAX][FileOperationeSelector_MAX];
extern volatile u32 fileSize[];

// ---- Global variables for Device Control ------------------------------------
extern volatile u32 DeviceIndicatorMode_Selecotr;

// --  Global Flash variables -------------
extern volatile u32 Flash_UserSetSelector;
extern volatile u32 Flash_UserSetLoad;
extern volatile u32 Flash_UserSetSave;
extern volatile u32 Flash_UserSetDefault;

// --  Global Counter variables -------------
extern volatile u32 Counter_CounterSelector;

// --  Global LUTControl variables -------------
extern volatile u32 LUTSelector_Selector;
extern volatile u32 LUTFormat_Selector;

// --  Global FlatFiledCorrectionControl variables -------------
extern volatile u32 FlatFieldCorrection_Selector;
extern volatile u32 FlatFieldCorrectionBrightAdjustment_Lumi;
extern volatile u32 FlatFieldCorrectionModeSelector;

// --  Global DefectivePixeiControl variables -------------
extern volatile u32 DefectPixelCorrection_Selector;
extern volatile u32 DefectPixelCorrection_AdjustFfcSelector;
extern volatile u32 DefectPixelCorrection_AdjustBrightSelector;

// --  Global High Speed Mode variables -------------
#if defined (MODE_FRAMERATE_HIGH_SPEED)
extern int gHighSpeedMode;
extern int gHighSpeedModeLineCount;
#endif

// ---- Global DRRS for variables  -------------------------
#if defined (MODE_SENSOR_DRRS)
extern int gDrrsMode;
#endif

extern miv_plic_instance_t g_plic;


//----------------------------------------------------------------------------------
// Function Define
//----------------------------------------------------------------------------------
extern int ElectricalComplianceTest_st;


//**********************************************************************************
//	CXP Main
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpMain (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int port = 0;

	//------------------------------------------------------------
	// CPU1 Boot Flag
	//------------------------------------------------------------
	OUT32 (FIRM_DATA_CPU1_BOOT_FLAG, 0);
	
	//------------------------------------------------------------
	// Make the version string of the camera
	//------------------------------------------------------------
	sprintf (deviceVersion,"%s",MAIN_VERSION);
	sprintf (deviceFirmwareVersion,"%s;%s;%s",FIRM_VERSION,"1.0"/*FPGA*/,FIRM_DATA_BOARD_VERSION_ADRS);

	//------------------------------------------------------------
	// CPU1 Boot Flag
	//------------------------------------------------------------
	OUT32 (FIRM_DATA_CPU1_BOOT_FLAG, 1);

	//------------------------------------------------------------
	// Detection of Connection in progress
	//------------------------------------------------------------
	//@@@1cxpLedConnectionDetection();

	//------------------------------------------------------------
	// User Init
	//------------------------------------------------------------
    cxpUserInit ();

	//------------------------------------------------------------
	// CXP IP Initialize
	//------------------------------------------------------------
    cxpIpInitialize ();

	//------------------------------------------------------------
	// CXP Init2
	//------------------------------------------------------------
    cxpInitialize2 ();

	//------------------------------------------------------------
	// CXP Command FIFO Reset
	//------------------------------------------------------------
	cxpCmdFifoReset ();

	//------------------------------------------------------------
	// CXP Interrupt Enable
	//------------------------------------------------------------
    MIV_PLIC_enable_irq (&g_plic, MIV_PLIC_EXT2_IRQn);

	//------------------------------------------------------------
	// CXP Rx Interrupt Enable
	//------------------------------------------------------------
	cxpSetRecvIntMode (MODE_ENABLE);

	//------------------------------------------------------------
	// Main Loop
	//------------------------------------------------------------
    cmd ();

	return (status);
}


//**********************************************************************************
//	CXP User Init
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpUserInit (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int i, j;
	unsigned int address2nd;
	unsigned char regAddress8 = 0;
	unsigned int xmlDemilitor = 0;
	unsigned int xmlCount = 0;
	char str16[16 + 1] = { 0 };
	char strXMLAddress[10] = { 0 };
	char strXMLSize[6] = { 0 };

//@@@@1
	char c;
	DEBUG_PRINT_FORCE("cxpUserInit\n");
//@@@@1
	
//@@@1
#if 0
	//------------------------------------------------------------
	// CXP Parameter Initialize
	//------------------------------------------------------------
	cxpInitializeImageParam ();
#endif
//@@@1

	//------------------------------------------------------------
	// DeviceIndicatorMode_Selecotr
	//------------------------------------------------------------
	DeviceIndicatorMode_Selecotr = DeviceIndicatorMode_Active;

	//------------------------------------------------------------
	// User Selector
	//------------------------------------------------------------
	Flash_UserSetSelector = CAMERA_SAVE_USER_NUM;
	Flash_UserSetLoad = 0;
	Flash_UserSetSave = 0;
	Flash_UserSetDefault = CAMERA_SAVE_USER_NUM;

//@@@1
#if 0
	//------------------------------------------------------------
	// Counter
	//------------------------------------------------------------
#if defined (MODE_GENICAM_COUNTER)
	Counter_CounterSelector = IN32(GENICAM_COUNTER_SELECT_ADRS);
#endif

	//------------------------------------------------------------
	// Initiates the CounterDiagnosticValue on the DDR:
	//------------------------------------------------------------
	//@@@1for (i = ARM1_DIAG_VALUE_START_ADRS; i <= ARM1_DIAG_VALUE_END_ADRS; i += 4)
		//@@@1OUT32(i, 0);

	//------------------------------------------------------------
	// LUT
	//------------------------------------------------------------
#if defined (MODE_LUT)

	#if defined (MODE_LUT_NUMBER_FIX)

	LUTSelector_Selector = LUT_SELECT_NUM;
	
	#else // #if defined (MODE_LUT_NUMBER_FIX)
 
	LUTSelector_Selector = Flash_UserSetSelector;

	if (LUTSelector_Selector == 0)
		LUTFormat_Selector = LUT_FORMAT_INC;
	else
		lutGetFormat(Flash_UserSetSelector, (int*) &LUTFormat_Selector);

	#endif // #if defined (MODE_LUT_NUMBER_FIX)

#endif // #if defined (MODE_LUT)

	
	//------------------------------------------------------------
	// FFC
	//------------------------------------------------------------
#if defined (MODE_FFC)
	FlatFieldCorrectionBrightAdjustment_Lumi = (u32) IN32(FIRM_DATA_FFC_WHITE_ADRS);
	FlatFieldCorrectionModeSelector = IN32 (FIRM_DATA_FFC_CORECTION_MODE_ADRS);

	address2nd = 0x0;
	cameraParamUserReadMem(CAMERA_SAVE_USER_NUM, CAMERA_SAVE_FFC_NUMBER_ADRS, (unsigned int*) &address2nd, (unsigned int*) &FlatFieldCorrection_Selector);
#endif

	//------------------------------------------------------------
	// DPC
	//------------------------------------------------------------
#if defined (MODE_DPC)
	address2nd = 0x0;
	cameraParamUserReadMem(CAMERA_SAVE_USER_NUM, CAMERA_SAVE_DPC_NUMBER_ADRS, (unsigned int*) &address2nd, (unsigned int*) &DefectPixelCorrection_Selector);

	DefectPixelCorrection_AdjustFfcSelector = 0;
	DefectPixelCorrection_AdjustBrightSelector = 0;
#endif


	//---------------------------------------------------------------
	// Global High Speed Mode variables
	//---------------------------------------------------------------
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	// Mode取得
	if (sensorGetFrameRateHighSpeedMode (&gHighSpeedMode) != AVAL_STATUS_SUCCESS)
		gHighSpeedMode = MODE_DISABLE;

	// Line Count取得
	if (gHighSpeedMode == MODE_ENABLE)
	{
		if (aoiGetHeight (&gHighSpeedModeLineCount) != AVAL_STATUS_SUCCESS)
			gHighSpeedModeLineCount = FRAME_RATE_HIGH_SPEED_MODE_LINE_COUNT_DEFAULT_CXP;
	}
	else
	{
		gHighSpeedModeLineCount = FRAME_RATE_HIGH_SPEED_MODE_LINE_COUNT_DEFAULT_CXP;
	}
#endif


	//---------------------------------------------------------------
	// DRRSモード取得
	//---------------------------------------------------------------
#if defined (MODE_SENSOR_DRRS)
	sensorGetDrrs (&gDrrsMode);
#endif

#endif//@@@1

	
	//---------------------------------------------------------------
	// Reads the first XML URL to the eeprom
	//local:ABA-013V-GE_Rev_1_0.xml;61700000;709C9?SchemaVersion=1.0.0
	//------------------------------------------------------------
	memset((void*) xmlURL, 0, 512);
	memset((void*) gXmlFileName1, 0, CXP_XML_URL_SIZE);
	memset((void*) gXmlFileName2, 0, CXP_XML_URL_SIZE);
	memset((void*) gXmlFileNameUpdate, 0, CXP_XML_URL_SIZE);

//@@@@1
	DEBUG_PRINT_FORCE("Qspi Read XML  File Name\n");
//@@@@1

	// XML File Name取得
	if ((status = qspiFlashRead((unsigned int)FLASH_XML_FILE_NAME_ADRS, (unsigned char*)gXmlFileName1, (unsigned int) CXP_XML_URL_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		memset((void*) gXmlFileName1, 0, CXP_XML_URL_SIZE);
	}

//@@@@1
	DEBUG_PRINT_FORCE("XML Search\n");
//@@@@1


	for (i = 0; i < CXP_XML_URL_SIZE; i++)
	{
		regAddress8 = gXmlFileName1[i];
		gXmlFileName2[i] = regAddress8;

		xmlURL[i] = regAddress8;
		if (i < 6)
		{
			xmlCount++;
			if (xmlCount == 6)
			{
				xmlDemilitor++;
				xmlCount = 0;
			}
		}
		else if (xmlDemilitor == 1)
		{
			//strXMLFile[xmlCount] = regAddress8;  // *.xml;
			xmlCount++;
			if (regAddress8 == ';')
			{
				//strXMLFile[xmlCount - 1] = '\0';
				xmlDemilitor++;
				xmlCount = 0;
			}
		}
		else if (xmlDemilitor == 2)
		{
			strXMLAddress[xmlCount] = regAddress8;  // 61000000;
			xmlCount++;
			if (regAddress8 == ';')
			{
				strXMLAddress[xmlCount - 1] = '\0';
				xmlDemilitor++;
				xmlCount = 0;
			}
		}
		else if (xmlDemilitor == 3)
		{
			strXMLSize[xmlCount] = regAddress8;  // 1DF1;
			xmlCount++;
			//if (regAddress8 == '?')
			if (regAddress8 == 'S')
			{
				strXMLSize[xmlCount - 1] = '\0';
				xmlDemilitor++;
				xmlCount = 0;
				
				if (sscanf (strXMLSize, "%x", &gXMLSize) != 1)
					gXMLSize = 0;
			}
		}
		else
		{
			// Done to parse the XML URL string
			if (regAddress8 == '\0')
				break;
		}
	}

	if (i < CXP_XML_URL_SIZE)
	{
		xmlURL[++i] = '\n';
		xmlStartAddress = strtol(strXMLAddress, (char**) &str16, 16);
		xmlSize = strtol(strXMLSize, (char**) &str16, 16);

		// Copy the second XML URL from the first
		xmlStartAddressSecond = strtol(strXMLAddress, (char**) &str16, 16);
		xmlSizeSecond = strtol(strXMLSize, (char**) &str16, 16);
		memcpy((void*) xmlURLSecond, (void*) xmlURL, 512);

		// XML Data Read
		sprintf (gLogMsgBuff, "XML File Name : %s / Adrs : 0x%08x / Size : 0x%08x\n", gXmlFileName1, xmlStartAddress, xmlSize);
		cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

		// XML Load
		if (gXMLSize > FLASH_XML_SIZE)
			gXMLSize = FLASH_XML_SIZE;

//@@@@1
	DEBUG_PRINT_FORCE("Qspi Read XML\n");
//@@@@1

		qspiFlashRead (FLASH_XML_ADRS, (unsigned char *)FIRM_XML_FILE_ADRS, gXMLSize);
	}
	else
	{
		memset((void*) gXmlFileName1, 0, CXP_XML_URL_SIZE);
		memset((void*) gXmlFileName2, 0, CXP_XML_URL_SIZE);
	}
	
	DEBUG_PRINT_FORCE("@@@1\n")	;
	
	// ---- Initiates Global variables for File Access Control --------------------------------
	fileSelector = 0;
	for (i = 0; i < FileSelector_MAX; i++)
	{
		fileSel[i] = FileSelector_SYSTEM;	// FileOperationSelector
		fileExec[i] = 0;					// No execution
		fileMode[i] = FileOpenMode_Read;	// Default is read access
		fileSize[i] = 0;					// 0 zero byte as Default size of a file

		// Allocates the memories of LUT for the table 1
		switch (i)
		{
			//------------------------------------------------------------
			// FPGA
			//------------------------------------------------------------
			case FileSelector_FPGA:
				fileBuffer[i] = (u32*)FIRM_UPDATE_ADRS;
				//memset((void*) fileBuffer[i], 0, FIRM_UPDATE_SIZE);

			//------------------------------------------------------------
			// XML
			//------------------------------------------------------------
			case FileSelector_XML:
				fileBuffer[i] = (u32*)FIRM_UPDATE_ADRS;
				break;

			//------------------------------------------------------------
			// LUT
			//------------------------------------------------------------
			case FileSelector_LUTLuminance0:
				fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
				break;

			//------------------------------------------------------------
			// FFC
			//------------------------------------------------------------
			case FileSelector_FFC0:
				fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
				break;

			//------------------------------------------------------------
			// DPC
			//------------------------------------------------------------
			case FileSelector_DPC0:
				fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
				break;

			//------------------------------------------------------------
			// Spectrum Wave Information
			//------------------------------------------------------------
			case FileSelector_SPECTRUM_WAVE:
				fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
				break;

			//------------------------------------------------------------
			// Default
			//------------------------------------------------------------
			default:
				fileBuffer[i] = 0;
				break;
			}

		for (j = 0; j < FileOperationeSelector_MAX; j++)
		{
			fileOffset[i][j] = 0;
			fileLength[i][j] = 0;
			fileStatus[i][j] = 0;
			fileResult[i][j] = 0;
		}
	}

	DEBUG_PRINT_FORCE("@@@2\n")	;

	return (status);
}


//**********************************************************************************
//	LED制御
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		linlState			：リンクステータス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpLedControl (int linlState)
{
	unsigned int value, value2nd;
	int port;

	//getLed (LED_NUM1, (int*) &value);
	value = IN32 (FIRM_DATA_CXP_LED1_ADRS);
	if (value == LED_PATTERN2) // HW Error
		goto _DONE;

	// Waitステータス取得
	value = IN32(GENICAM_ACQUISITION_TRGWAIT_ADRS);
	value2nd = IN32(GENICAM_ACQUISITION_FRAME_TRGWAIT_ADRS);

	if (value == 1 || value2nd == 1)
	{
		for (port=0; port<CXP_PORT_COUNT; port++)
		{
			// Link Downならば何もしない
			if ((linlState & (1<<port)) == 0)
				continue;

			cxpGetLed (port, (int*) &value);

			if (value != LED_WAIT_CMD)
			{
				// Wait
				cxpSetLed (port, LED_WAIT_CMD);	// slow pulse orange
			}
		}
	}
	else
	{
		// No waiting an action and trigger
		// Streaming
		value = IN32(GENICAM_ACQUISITION_ACTIVE_ADRS);

		// Now under AcquisitionActive but no streaming
		value2nd = IN32(GENICAM_ACQUISITION_TRANSFER_ADRS); // Acquisition Transfer

		if (value == 1 || value2nd == 1)
		{
			for (port=0; port<CXP_PORT_COUNT; port++)
			{
				// Link Downならば何もしない
				if ((linlState & (1<<port)) == 0)
					continue;

				// FPGAが制御するため、何もしない
				//cxpSetLed(port, LED_PATTERN7);	// Fast flush green
			}
		}
		else
		{
			for (port=0; port<CXP_PORT_COUNT; port++)
			{
				// Link Downならば何もしない
				if ((linlState & (1<<port)) == 0)
					continue;

				// Just the state where stay a connection active.
				cxpGetLed(port, (int*) &value);

				// Streaming or Trigger wait
				if ((value == LED_STREAMING) || (value == LED_WAIT_CMD))
				{
					cxpSetLed (port, LED_PATTERN5);	// Lighting
				}
			}
		}
	}

_DONE:
	return (AVAL_STATUS_SUCCESS);
}



//**********************************************************************************
//	LED: ledLinkState
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pLinkStatus			：リンクステータス(bit=ch1/bit0=ch0【0=Link Down / 1=Link Up】)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpLedLinkState (unsigned int *pLinkStatus)
{
	int status = AVAL_STATUS_SUCCESS;
	int linkStatusCurrent, linkStatusCurrentBit;
	int port;

	// 今までの状態を取得
	linkStatusCurrentBit = *pLinkStatus;

	for (port=0; port<CXP_PORT_COUNT; port++)
	{
		// ケーブル接続状態取得
		if ((status = cxpGetCableConnection (port, &linkStatusCurrent)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 現在の状態を反映
		if (linkStatusCurrent == 0)
			linkStatusCurrentBit &= ~(1<<port);
		else
			linkStatusCurrentBit |= (1<<port);


		#if 0
		if (ElectricalComplianceTest_Led != 0)
		{
			*pLinkStatus = linkStatusCurrentBit;
			continue;
		}
		#endif

		// Link状態の変化あり
		if ((*pLinkStatus != linkStatusCurrentBit) || (gLinkStatusCheck != 0))
		{
			if (linkStatusCurrent == MODE_LINK_UP)
			{
				cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "CXP Link UP\n");

				// コネクション確立
			    cxpSetLed (port, LED_PATTERN5); // solid green;
			}
			else
			{
				cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "CXP Link Down\n");

				// Abort the previous acquisition
				acquisitionAbort();

				// LED Connection Detection
				cxpSetLed (port, LED_PATTERN4); // fast pulse Orange
			}

			if (gLinkStatusCheck != 0)
				gLinkStatusCheck = 0;
			
			// 今回のステータスを保存
			*pLinkStatus = linkStatusCurrentBit;
		}
	}

_DONE:
	return (status);
}



//**********************************************************************************
//	LED Connection Detection
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpLedConnectionDetection (void)
{
	int port;

	// LED Connection Detection
	for (port=0; port<CXP_PORT_COUNT; port++)
		cxpSetLed (port, LED_PATTERN4); // fast pulse Orange

	return (AVAL_STATUS_SUCCESS);
}



//**********************************************************************************
//	Get Link Num
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount			：リンク数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetLinkCount (unsigned int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;
	int linkStatus;
	int port;
	unsigned int count;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Get Link Count NULL Parameter Error.\n");
		goto _DONE;
	}

	count = 0;
	for (port=0; port<CXP_PORT_COUNT; port++)
	{
		// ケーブル接続状態取得
		if ((status = cxpGetCableConnection (port, &linkStatus)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (linkStatus == 1)
			count++;
	}

	*pCount = count;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Packet Recive Interrupt
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		-
//==================================================================================
int MIV_PLIC_EXT2_IRQHandler (void)
{
	unsigned int intState, intEaable;
	int exeFlag;
	
	// Get Interrupt Status
	intState = IN32 (FPGA_CXP_INT_STATUS_ADRS);

	// Get Interrupt Mode
	intEaable = IN32 (FPGA_CXP_INT_ENABLE_ADRS);
	
	// Mask
	intState &= intEaable;

	// RX Interrupt?
	if (intState & FPGA_CXP_INT_STATUS_RX_PACKET)
	{
		// Exeコマンド実行中？
		exeFlag = IN32 (FIRM_DATA_CMD_EXE_FLAG);
		
		if (exeFlag == 1)
		{
			gCxpIntProcsFlag = 1;

			// コマンド処理中なので、割り込みルーチン内でコマンド解析
			cxpProcs (0);
			
			gCxpIntProcsFlag = 0;
		}
	}
	
	return (EXT_IRQ_KEEP_ENABLED);
}


//**********************************************************************************
//	CXP Packet Recive Interrupt Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode : 0 = Interrupt Disable / 1 = Interrupt Enable
//	[ OUTPUT ]
//		-
//==================================================================================
int cxpSetRecvIntMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check data Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Packet Recive Int mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Interrupt Mode
	data32 = IN32 (FPGA_CXP_INT_ENABLE_ADRS);
	
	if (mode == MODE_ENABLE)
		data32 |= FPGA_CXP_INT_ENABLE_RX_PACKET;
	else
		data32 &= ~FPGA_CXP_INT_ENABLE_RX_PACKET;
	
	// Set Interrupt Mode
	OUT32 (FPGA_CXP_INT_ENABLE_ADRS, data32);
	
_DONE:
	return (status);
}


//**********************************************************************************
//	CXP IP Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		-
//==================================================================================
int cxpIpInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int data32;
	char c;
	unsigned int data, wsize;
		
//@@@@1
	DEBUG_PRINT_FORCE("Param Start111\n");
//@@@@1
	data = IN32 (FPGA_CXP_TOP_CTRL_ADRS);
//@@@@1
	DEBUG_PRINT_FORCE("DATA = 0x%x\n", data);
//@@@@1

	// CoaXPress IP Reset Cancel
	data &= ~FPGA_CXP_TOP_CTRL_RSTN;
	OUT32 (FPGA_CXP_TOP_CTRL_ADRS, data);

	for (i=0; i<FPGA_CXP_TOP_CTRL_RSTN_TIMEOUT; i++)
	{
		data = IN32 (FPGA_CXP_TOP_CTRL_ADRS);
		data &= (FPGA_CXP_TOP_CTRL_RSTN_STATUS | FPGA_CXP_TOP_CTRL_PIX_RSTN_STATUS | FPGA_CXP_TOP_CTRL_TX0_RSTN_STATUS);

		// Check Status
		if (data == (FPGA_CXP_TOP_CTRL_RSTN_STATUS | FPGA_CXP_TOP_CTRL_PIX_RSTN_STATUS | FPGA_CXP_TOP_CTRL_TX0_RSTN_STATUS))
			break;
		
		usDelay (1000);
	}
	
	if (i >= FPGA_CXP_TOP_CTRL_RSTN_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Reset Cancel Timeout Error.\n");
	}

//@@@@1
	DEBUG_PRINT_FORCE("Param0\n");
#if 0
	while(1)
		{
            if (_kbhit(&c))
            {
                if (c == 'q')
                    break;
            }
		}
#endif
//@@@@1

	OUT32(FPGA_CXP_S0_XSIZE_OFFSET_ADRS, 2560);

//@@@@1
	DEBUG_PRINT_FORCE("Param1\n");
//@@@@1

	OUT32(FPGA_CXP_S0_YSIZE_OFFSET_ADRS, 2048);

//@@@@1
	DEBUG_PRINT_FORCE("Param2\n");
//@@@@1

	OUT32(FPGA_CXP_S0_DSIZE_ADRS, (2560*8/32));

//@@@@1
	DEBUG_PRINT_FORCE("Param3\n");
//@@@@1

	OUT32(FPGA_CXP_S0_TAPG_PIXEL_ADRS, CXP_REG_PIXEL_MONO8);

//@@@@1
	DEBUG_PRINT_FORCE("Param4\n");
//@@@@1

	OUT32(0x6b200008, 0x08000A00);

	return (status);
}

//**********************************************************************************
//	Cmd Interrupt Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				:MODE_DISABLE/MODE_ENABLE
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdInterruptMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Cmd Interrupt Mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,  __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	if (gInterFaceID == INTERFACE_CXP)
	{
		//@@@1
	}
	else if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		//@@@1
	}
	
_DONE:
	return (status);
}


//**********************************************************************************
//	Cmd Execute Procs
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdExecuteInit (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Execute
	OUT32 (FIRM_DATA_CMD_EXE_FLAG, 1);

	// Cmd Interrupt Enable
	//@@@1if ((status = cmdInterruptMode (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		//@@@1goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Cmd Execute Done
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdExecuteDone (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Execute Clear
	OUT32 (FIRM_DATA_CMD_EXE_FLAG, 0);

	return (status);
}


//**********************************************************************************
//	Cmd Execute Status
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStatus				：コマンドステータスを格納するポンタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdExecuteStatus (void)
{
	int status;
	
	status = IN32 (FIRM_DATA_CMD_EXE_FLAG);

	return (status);
}

#endif // #if defined (IF_CXP)

// eof
