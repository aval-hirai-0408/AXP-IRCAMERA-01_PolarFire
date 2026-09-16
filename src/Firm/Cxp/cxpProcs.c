//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// procs.c - CXP Procs Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"

#if defined (MODE_CXP)
#if defined (IF_CXP)
#include "cxpBs.h"
#include "cxpCrc.h"
#include "../GigE/gev.h"
#include "../GigE/user.h"
#include "cxp.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
unsigned int ElectricalComplianceTest_Led = 0;
int gCxpAckDoneFlag = 0;
//@@@@@@@@@@@@@@@@
unsigned int gCxpAllRecvCount = 0;
//@@@@@@@@@@@@@@@@


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern unsigned int xmlStartAddress;
extern unsigned int xmlSize;
extern unsigned int xmlStartAddressSecond;
extern unsigned int xmlSizeSecond;

extern unsigned int fileSelector;
extern unsigned int fileSel[];
extern unsigned int fileExec[];
extern unsigned int fileMode[];
extern unsigned int* fileBuffer[];
extern unsigned int fileResult[FileSelector_MAX][FileOperationeSelector_MAX];

extern char deviceFirmwareVersion[];
extern char deviceUserID[];
extern unsigned char gXmlFileName1[];
extern char deviceVersion[];
extern unsigned char gXmlFileNameUpdate [];

#if defined (MODE_FPGA_PF)
extern int gIfFpgaReConfig;
#endif

extern int gLinkStatusCheck;

extern int gCxpCmdProcessFlag;

//@@@@@@@@@@
int gDebugAAA = 0;
//@@@@@@@@@@

//**********************************************************************************
//	CXP Initialize(受信関連レジスタのみ：ARM0から初期化)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int port;

	// CXPのPort番号を指定する
	cxpSetPort (0);

	// 送信カウンタクリア
	for (port=0; port<CXP_PORT_COUNT; port++)
		OUT32 ((FIRM_DATA_CXP_SEND_DATA_COUNT_MULTI_ADRS + port * 4), 0);

	// 受信カウンタクリア
	for (port=0; port<CXP_PORT_COUNT; port++)
		OUT32 ((FIRM_DATA_CXP_RECV_DATA_COUNT_MULTI_ADRS + port * 4), 0);

	// 受信データ数クリア
	for (port=0; port<CXP_PORT_COUNT; port++)
		OUT32 ((FIRM_DATA_CXP_DATA_COUNT_MULTI_ADRS + port * 4), 0);

	return (status);
}



//**********************************************************************************
//	CXP Initialize2
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpInitialize2 (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int port;
	unsigned int data32;
	int dataI32;
	unsigned int cxpSpeed;
#if defined (MODE_BOARD_ACB531CXP)
	unsigned int linkCount;
	int streamMode = CXP_MODE_SINGLE_STREAM;
	unsigned int regData;
#endif
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	int bit;
	int shutterMode;
#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)


	//------------------------------------------------------------
	// CoaxPress IP Initialize
	//------------------------------------------------------------

	// Spped Mode
	OUT32 (FPGA_CXP_LSUC_SPEED_MODE_ADRS, 0);

	// Heaet Bert Disable
	OUT32 (FPGA_CXP_HSDC_HB_EN_ADRS, 0);

	// Stream Disable
	OUT32 (FPGA_CXP_S0_STREAM_EN_ADRS, 0);
	
	// MAX Packet & id=0
	OUT32 (FPGA_CXP_S0_FLAG_SID_MZXSIZE_ADRS, (0x800<<16));


	//------------------------------------------------------------
	// Stream ID
	//------------------------------------------------------------
	for (port=0; port<CXP_PORT_COUNT; port++)
	{
		if (port == 0)
			dataI32 = CXP_STREAM_ID1_NUMBER;
		else
			dataI32 = CXP_STREAM_ID2_NUMBER;

		if ((status = cxpSetStreamId (port, dataI32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}


	//------------------------------------------------------------
	// DeviceConnectionID
	//------------------------------------------------------------
	for (port=0; port<CXP_PORT_COUNT; port++)
		ConnectionDeviceConnection_st[port] = port;

//@@@1
	ConnectionConfig_st = ((1<<16) | 0x38);
goto _DONE;
//@@@1

#if 0	//@@@1
	//------------------------------------------------------------
	// Register Data Restore
	//------------------------------------------------------------
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_CXP_ADRS, CAMERA_CXP_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif //@@@1


	//--------------------------------------------------------------------------------
	// Set CXP Rate
	//--------------------------------------------------------------------------------
#if defined (MODE_BOARD_ACB531CXP)
	// 受信FIFO Disable
	data32 = IN32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL));
	OUT32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL), (data32 & ~FPGA_CXP_RX_CMD_FIFO_CTRL_ENABLE));

	// Up Link Rate変更
	OUT32 (FPGA_CXP_LSUC_CTRL_ADRS, 0);

	// 受信FIFO Reset
	OUT32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL), (data32 | FPGA_CXP_RX_CMD_FIFO_CTRL_RESET));

	// 受信FIFO ENABLE
	OUT32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL), (data32 | FPGA_CXP_RX_CMD_FIFO_CTRL_ENABLE));
	
	// IPレジスタ設定値取得
	data32 = CXP_RATE_3_125G;
	if ((status = cxpGetDataToRegData (data32, &regData)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// CXP Rate設定
	if ((status = cxpSetRateReg (regData)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Connection Config設定
	cxpSetRateData (((1<<16) | CXP_RATE_3_125G));
#endif


	//------------------------------------------------------------
	// CXP Rate & Connection
	//------------------------------------------------------------
#if defined (MODE_BOARD_ACB531CXP)
	// Usersetの値を取得
	ConnectionConfig_st = IN32 (FIRM_DATA_CXP_CONNECTION_CONFIG);
	
	// Usersetの値のリンクカウントを取得
	data32 = ConnectionConfig_st >> 16;

	// リンクカウント取得
	if ((status = cxpGetLinkCount (&linkCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Usersetの値のリンクカウントが2ならば、現在のリンクカウント値を参照
	// Usersetの値のリンクカウントが1ならば、リンクカウントは1にする
	if (data32 == 2)
	{
		ConnectionConfig_st &= 0xffff;
		ConnectionConfig_st |= (linkCount << 16); 
	}
	
	// Connection Config設定
	//cxpSetRateData (ConnectionConfig_st);

	// Connection Config Default設定
	ConnectionConfigDefault_st = ConnectionConfig_st;
	
	// Connection Configは上記で3Gbpsに設定
	ConnectionConfig_st &= ~0xffff;
	ConnectionConfig_st |= CXP_RATE_3_125GBPS;

	OUT32 (FIRM_DATA_CXP_CONNECTION_CONFIG, ConnectionConfig_st);
	
	//------------------------------------------------------------
	// Single/Dual
	//------------------------------------------------------------
	if ((linkCount != 1) && (linkCount != 2))
		linkCount = 1;

	if ((status = cxpSetPortDual (0/*共通レジスタの為、Port=0に設定*/, 1/*linkCount*/)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// Stream Mode
	//------------------------------------------------------------
	#if defined MODE_MULTI_STREAM
	streamMode = CXP_MODE_MULTI_STREAM;
	#endif

	if ((status = cxpSetStreamMode (0/*共通レジスタの為、Port=0に設定*/, streamMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#endif // #if defined (MODE_BOARD_ACB531CXP)


	//------------------------------------------------------------
	// Set H Interval
	//------------------------------------------------------------
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

	// Shutterモード取得
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Shutterモードはトリガモード?
	if (shutterMode == SHUTTER_MODE_TRIGGER)
	{
		// Get Bit
		if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Set H Interval
		if ((status = sensorSetHInterval (bit))!= AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// Compliance Test Mode取得
	ElectricalComplianceTest_st = IN32 (FIRM_DATA_CXP_COMPLIANCE_TEST_MODE_ADRS);
	ElectricalComplianceTest_Led = ElectricalComplianceTest_st;

	if (ElectricalComplianceTest_st != 0)
	{
		// IPレジスタ設定値取得
		cxpSpeed = ElectricalComplianceTest_st & 0xffff;
		if ((status = cxpGetDataToRegData (cxpSpeed, &regData)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		port = 0;
		// 受信FIFO Disable
		data32 = IN32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL));
		OUT32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL), (data32 & ~FPGA_CXP_RX_CMD_FIFO_CTRL_ENABLE));

		// Up Link Rate変更
		if ((cxpSpeed == CXP_RATE_10_000GBPS) || (cxpSpeed == CXP_RATE_12_500GBPS))
			OUT32 (FPGA_CXP_LSUC_CTRL_ADRS, FPGA_CXP_LSUC_LINK_SPEED_41M);
		else
			OUT32 (FPGA_CXP_LSUC_CTRL_ADRS, 0);

		// 受信FIFO Reset
		OUT32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL), (data32 | FPGA_CXP_RX_CMD_FIFO_CTRL_RESET));

		// 受信FIFO ENABLE
		OUT32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL), (data32 | FPGA_CXP_RX_CMD_FIFO_CTRL_ENABLE));

		// CXP Rate設定
		if ((status = cxpSetRateReg (regData)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Single設定
		if ((status = cxpSetPortDual (0/*共通レジスタの為、Port=0に設定*/, 1/*linkCount*/)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 送信テストモード開始
		for (port=0; port<1/*CXP_PORT_COUNT*/; port++)
		{
			if ((status = cxpRegWrite (port, CXP_REG_TEST_MODE_ADRS, 1, 1)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		// TestModeステータス設定
		TestMode_st = 1;

		// LED制御
		setLed (LED_STATE, LED_DIAG);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Image Parameter Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpInitializeImageParam (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;
	int dataI32;
#if defined(MODE_BINNING)
	int binning;
#endif
	int port;

	for (port=0; port<CXP_PORT_COUNT; port++)
	{
		//------------------------------------------------------------
		// Width
		//------------------------------------------------------------
		if ((status = aoiGetWidth (&dataI32)) != AVAL_STATUS_SUCCESS)
			dataI32 = IMG_WIDTH;

		if ((status = cxpSetWidth (port, dataI32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Height
		//------------------------------------------------------------
		if ((status = fpgaRoiGetCameraHeightTotalSize (&dataI32)) != AVAL_STATUS_SUCCESS)
			dataI32 = IMG_HEIGHT;

#if defined(MODE_BINNING)

		// BiiningY
		if ((status = aoiGetBinningY (&binning)) == AVAL_STATUS_SUCCESS)
		{
			if(binning != MODE_DISABLE)
			{
				// 1以下はスルー
				if(dataI32 <= (binning-1))
					goto _DONE;

				// 偶数なら半分
				if((dataI32 % binning) == 0)
					dataI32 = dataI32 / binning;
				else
					// 奇数なら-1
					dataI32 = (dataI32 & ~(binning -1)) / binning;
			}
		}

#endif // #if defined(MODE_BINNING)

		if ((status = cxpSetHeight (port, dataI32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Offsetx
		//------------------------------------------------------------
		if ((status = aoiGetWidthOffset (&dataI32)) != AVAL_STATUS_SUCCESS)
			dataI32 = 0;

		if ((status = cxpSetOffsetX (port, dataI32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Offsety
		//------------------------------------------------------------
		if ((status = fpgaRoiGetOffsetY (&dataI32)) != AVAL_STATUS_SUCCESS)
			dataI32 = 0;

		if ((status = cxpSetOffsetY (port, dataI32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Bit
		//------------------------------------------------------------

		// Bit取得
		if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
			bit= 12;

		// Bit設定
		if ((status = cxpSetBit (port, bit))  != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Procs Main
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpProcs (int port)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pCxpRxBuff;
	unsigned int i, data, dataSwap;
	CXP_PACKET_ST cxpPaket;
	unsigned int crc_value;
	unsigned int rxCount = 0;
	unsigned int packetCount;
	
	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Procs port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//============================================================
	//============================================================
	//============================================================
	// 初期化
	//============================================================
	//============================================================
	//============================================================

	// CXP受信データをDDRに取得するアドレス
	pCxpRxBuff = (unsigned int *)(FIRM_CXP_RECV_DATA_CMD_ADRS + port * FIRM_CXP_DATA_INTERVAL);

	//============================================================
	//============================================================
	//============================================================
	// パケット取得処理
	//============================================================
	//============================================================
	//============================================================

	//------------------------------------------------------------
	// パケット数取得
	//------------------------------------------------------------
	if ((status = cxpGetCmdPacket (port, &packetCount)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Start Code No Data\n");
		goto _DONE;
	}

	//------------------------------------------------------------
	// 開始 K Code取得
	//------------------------------------------------------------
	if ((status = cxpGetCmdPacket (port, &cxpPaket.start)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Start Code No Data\n");
		goto _DONE;
	}

	// CXP K Code取得
	if (cxpPaket.start != CXP_K_CODE_K27_7)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Read Start Code(0x%08x) Error.\n", cxpPaket.start);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DDRにデータ格納
	//*pCxpRxBuff++ = cxpPaket.start;
	//rxCount++;

	//------------------------------------------------------------
	// Control Command Indication取得
	//------------------------------------------------------------
	if ((status = cxpGetCmdPacket (port, &cxpPaket.cmdIndication)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Comand Indication No Data\n");
		goto _DONE;
	}

	// DDRにデータ格納
	//*pCxpRxBuff++ = cxpPaket.cmdIndication;
	//rxCount++;

	// Command Packet?
	if (cxpPaket.cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND)
	{
#if defined (MODE_CXP_VERSION_20)
		goto _NEXT_NOTAG;
#else
		goto _NEXT1;
#endif
	}

	#if defined (MODE_CXP_VERSION_20)
	if (VersionUsed_st == CXP_VERSION_20)
	{
		// Tag Command Packet?
		if (cxpPaket.cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
			goto _NEXT1;
	}
	#endif

	// それ以外のindication
#if defined (MODE_CXP_VERSION_20)
	if (VersionUsed_st == CXP_VERSION_20)
	{
		if ((cxpPaket.cmdIndication != CXP_DATA_PACKET_TYPE_COMMAND) && (cxpPaket.cmdIndication != CXP_DATA_PACKET_TYPE_COMMAND_TAG))
			goto _DONE;
	}
	else
	{
		if (cxpPaket.cmdIndication != CXP_DATA_PACKET_TYPE_COMMAND)
			goto _DONE;
	}
#else
	if (cxpPaket.cmdIndication != CXP_DATA_PACKET_TYPE_COMMAND)
		goto _DONE;
#endif


_NEXT1:
	#if defined (MODE_CXP_VERSION_20)
	//------------------------------------------------------------
	// Tag取得
	//------------------------------------------------------------
	if (VersionUsed_st == CXP_VERSION_20)
	{
		if (cxpPaket.cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
		{
			// tagあり
			if ((status = cxpGetCmdPacket (port, &cxpPaket.tag)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// DDRにデータ格納
			*pCxpRxBuff++ = cxpPaket.tag;
			rxCount++;

			if (rxCount >= (FIRM_CXP_RECV_DATA_CMD_SIZE/4))
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Command Buffer Over Flow Error\n");
				goto _DONE;
			}
		}
		else
		{
			cxpPaket.tag = 0;
		}
	}
	else
	{
#if defined (MODE_CXP_VERSION_20)
_NEXT_NOTAG:
#endif
		cxpPaket.tag = 0;
	}
	#else
	cxpPaket.tag = 0;
	#endif

	//------------------------------------------------------------
	// Cmd & Size取得
	//------------------------------------------------------------
	if ((status = cxpGetCmdPacket (port, &data)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Size & Cmd No Data\n");
		goto _DONE;
	}

	// DDRにデータ格納
	*pCxpRxBuff++ = data;
	rxCount++;

	cxpPaket.size = data & CXP_CTRL_SIZE_MASK;
	cxpPaket.cmd = (data & CXP_CTRL_CMD_MASK) >> CXP_CTRL_CMD_SHIFT;

	//------------------------------------------------------------
	// アドレス取得
	//------------------------------------------------------------
	if ((status = cxpGetCmdPacket (port, &data)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Address No Data\n");
		goto _DONE;
	}

	// DDRにデータ格納
	*pCxpRxBuff++ = data;
	rxCount++;

	cxpPaket.adrs = data;

	//------------------------------------------------------------
	// コマンドデータ取得
	//------------------------------------------------------------
	cxpPaket.pData = (unsigned int *)pCxpRxBuff;

	if (cxpPaket.cmd == CXP_CTRL_CMD_WRITE)
	{
		for (i=0; i<(cxpPaket.size/4); i++, pCxpRxBuff++, rxCount++)
		{
			if ((status = cxpGetCmdPacket (port, pCxpRxBuff)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Data Area No Data\n");
				goto _DONE;
			}

			// 先頭だけ格納
			if (i == 0)
				cxpPaket.data = *pCxpRxBuff;
		}

		if ((cxpPaket.size%4) != 0)
		{
			if ((status = cxpGetCmdPacket (port, pCxpRxBuff)) != AVAL_STATUS_SUCCESS)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Data Area No Data\n");
				goto _DONE;
			}

			rxCount++;
			pCxpRxBuff++;
		}
	}

	//------------------------------------------------------------
	// CRC取得
	//------------------------------------------------------------
	if ((status = cxpGetCmdPacket (port, &cxpPaket.crc)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CRC No Data\n");
		goto _DONE;
	}

	dataSwap = SWAP_L(cxpPaket.crc);
	cxpPaket.crc = dataSwap;


	//------------------------------------------------------------
	// 終了 K Code取得
	//------------------------------------------------------------
	if ((status = cxpGetCmdPacket (port, &cxpPaket.end)) != AVAL_STATUS_SUCCESS)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "End Code No Data\n");
		goto _DONE;
	}

	//------------------------------------------------------------
	// CRC計算
	//------------------------------------------------------------
	cxpCalculateCrc32 (&crc_value, (unsigned int *)(FIRM_CXP_RECV_DATA_CMD_ADRS + port * FIRM_CXP_DATA_INTERVAL), rxCount);

	if (crc_value != cxpPaket.crc)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CRC Error. Calc=0x%08x, Packet=0x%08x\n", crc_value, cxpPaket.crc);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		
		// status設定
		cxpPaket.status = CXP_ACK_CODE_CRC;

		// Ack Dataサイズ
		cxpPaket.ackSize = 0;

		// Send Cmd Indication
#if defined (MODE_CXP_VERSION_20)
		if (VersionUsed_st == CXP_VERSION_20)
		{
			if (cxpPaket.cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
				cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK_TAG;
			else
				cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
		}
		else
		{
			cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
		}
#else
		cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
#endif

		// CXPパケット作成
		cxpSetAckPacket (port, &cxpPaket);
		goto _DONE;
	}

	//============================================================
	//============================================================
	//============================================================
	// 設定＆取得処理
	//============================================================
	//============================================================
	//============================================================

	//------------------------------------------------------------
	// Write
	//------------------------------------------------------------
	if (cxpPaket.cmd == 1)
	{
		// コマンド処理中の場合は、エラーを返す（Writeの場合のみ）
		if (gCxpCmdProcessFlag == 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Command Processing.\n");
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			
			// status設定
			cxpPaket.status = CXP_ACK_CODE_INVALID_DATA;

			// Ack Dataサイズ
			cxpPaket.ackSize = 0;

			// Send Cmd Indication
			#if defined (MODE_CXP_VERSION_20)
			if (VersionUsed_st == CXP_VERSION_20)
			{
				if (cxpPaket.cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
					cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK_TAG;
				else
					cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
			}
			else
			{
				cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
			}
			#else
			cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
			#endif

			// CXPパケット作成
			cxpSetAckPacket (port, &cxpPaket);
			goto _DONE;
		}
		
		// Ack Dataサイズ
		cxpPaket.ackSize = 0;
		
		// Ack返信済Flag初期化
		gCxpAckDoneFlag = 0;

		// パラメータ設定
		status = cxpSetUser (port, &cxpPaket);

		// Execute系はAck返信済でgCxpAckDoneFlag=1になる(cxpSetUser関数でAck返信)
		if (gCxpAckDoneFlag == 0)
		{
			// status設定
			cxpPaket.status = status;

			// Send Cmd Indication
			if (VersionUsed_st == CXP_VERSION_20)
			{
				if (cxpPaket.cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
					cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK_TAG;
				else
					cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
			}
			else
			{
				cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
			}

			// Ack Packet設定
			cxpSetAckPacket (port, &cxpPaket);
		}

		//------------------------------------------------------------
		// IF Fpga Reconfig制御
		//------------------------------------------------------------
	#if 0	//@@@1
	#if defined (MODE_FPGA_PF)
		if ((gIfFpgaReConfig == 1) && (cxpPaket.adrs == FileOperationExecute))
		{
			cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "IF FPGA ReConfig Start\n");

			// Debug Mode
			ledSetDebugMode (LED_DEBUG_MODE_DISABLE);

			// 黄色
			setLed (LED_NUM0, LED_PATTERN1);

			if ((status = fpgaUpdatePolarFireReconfig ()) != AVAL_STATUS_SUCCESS)
			{
				// 赤点滅
				setLed (LED_NUM0, LED_PATTERN2);
			}
			else
			{
				// 緑点灯
				setLed (LED_NUM0, LED_PATTERN5);
			}

			gIfFpgaReConfig = 0;
			PolarFireSpiMode (MODE_DISABLE);

			cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "IF FPGA ReConfig End\n");

		}
	#endif
	#endif //@@@1
	}
	//------------------------------------------------------------
	// Read
	//------------------------------------------------------------
	else if (cxpPaket.cmd == 0)
	{
		// パラメータ取得
		status = cxpGetUser (port, &cxpPaket);

		// Ack Dataサイズ
		if (status != AVAL_STATUS_SUCCESS)
			cxpPaket.ackSize = 0;

		// status設定
		cxpPaket.status = status;

		// Send Cmd Indication
		if (VersionUsed_st == CXP_VERSION_20)
		{
			if (cxpPaket.cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
				cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK_TAG;
			else
				cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
		}
		else
		{
			cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
		}

		// CXPパケット作成
		cxpSetAckPacket (port, &cxpPaket);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Set User
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pCxpSt				：CXPパケットデータを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetUser (int port, CXP_PACKET_ST *pCxpSt)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs, adrs2;
	unsigned int *pDataRecv;
	unsigned int *pDataSend;
	unsigned short status16;
	unsigned int data1, data2;
	unsigned int *ptr32;
	unsigned int swapData32;
	unsigned int ix, iy;
	unsigned char *ptrSrc8, *ptrDes8, *ptrDes8_DDR;
	unsigned int *ptrL;

	sprintf (gLogMsgBuff,"adrs=0x%08x, size=0x%08x\n", pCxpSt->adrs, pCxpSt->size, status);
	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set User port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pCxpSt Parameter
	if (pCxpSt == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Set User pCxpSt NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pData Parameter
	if (pCxpSt->pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Set User pData NULL Parameter Error\n");
		goto _DONE;
	}

	// アドレス取得
	adrs = pCxpSt->adrs;

	// 受信データ格納位置取得
	pDataRecv = pCxpSt->pData;

	// 送信データ格納位置取得(0x00:Ackコード/0x04=Size/0x08=データなので0x08のオフセットを付加)
	pDataSend = (unsigned int *)(FIRM_CXP_SEND_DATA_CMD_ADRS + FIRM_CXP_DATA_INTERVAL * port + CXP_SEND_DATA_OFFSET);

	if (VersionUsed_st == CXP_VERSION_20)
	{
		if (pCxpSt->cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
			pDataSend++;		// Tag追加(+4)
	}

	switch (adrs)
	{
		//------------------------------------------------------------
		// Standard設定
		//------------------------------------------------------------
		//case Standard:
			//break;

		//------------------------------------------------------------
		// Revision設定
		//------------------------------------------------------------
		//case Revision:
			//break;

		//------------------------------------------------------------
		// XmlManifestSize設定
		//------------------------------------------------------------
		//case XmlManifestSize:
			//break;

		//------------------------------------------------------------
		// XmlManifestSelector設定
		//------------------------------------------------------------
		case XmlManifestSelector:
			XmlManifestSelector_st = *pDataRecv;
			break;

		//------------------------------------------------------------
		// XmlVersion設定
		//------------------------------------------------------------
		//case XmlVersion:
			//break;

		//------------------------------------------------------------
		// XmlSchemaVersion設定
		//------------------------------------------------------------
		//case XmlSchemaVersion:
			//break;

		//------------------------------------------------------------
		// XmlUrlAddress設定
		//------------------------------------------------------------
		//case XmlUrlAddress:
			//break;

		//------------------------------------------------------------
		// Iidc2Address設定
		//------------------------------------------------------------
		//case Iidc2Address:
			//break;

		//------------------------------------------------------------
		// CXP Interface FPGA Version設定
		//------------------------------------------------------------
		//case IfFpgaVersion:
			//break;

		//------------------------------------------------------------
		// DeviceVendorName設定
		//------------------------------------------------------------
		//case DeviceVendorName:
			//break;

		//------------------------------------------------------------
		// DeviceVendorNameWrite設定
		//------------------------------------------------------------
		case DeviceVendorNameWrite:
		case DeviceVendorNameWrite+0x04:
		case DeviceVendorNameWrite+0x08:
		case DeviceVendorNameWrite+0x0C:
		case DeviceVendorNameWrite+0x10:
		case DeviceVendorNameWrite+0x14:
		case DeviceVendorNameWrite+0x18:
		case DeviceVendorNameWrite+0x1C:

			adrs2 = adrs - DeviceVendorNameWrite;

			if ((pCxpSt->size + adrs2) > 32)
				pCxpSt->size = 32 - adrs2;

			for (ix=0; ix<pCxpSt->size; ix+=4, pDataRecv++)
			{
				data1 = *pDataRecv;
				swapData32 = SWAP_L(data1);
				OUT32 ((FIRM_DATA_VENDOR_ADRS + adrs2 + ix), swapData32);
			}

			break;

		//------------------------------------------------------------
		// DeviceModelName設定
		//------------------------------------------------------------
		//case DeviceModelName:
			//break;

		//------------------------------------------------------------
		// DeviceModelNameWrite設定
		//------------------------------------------------------------
		case DeviceModelNameWrite:
		case DeviceModelNameWrite + 0x04:
		case DeviceModelNameWrite + 0x08:
		case DeviceModelNameWrite + 0x0C:
		case DeviceModelNameWrite + 0x10:
		case DeviceModelNameWrite + 0x14:
		case DeviceModelNameWrite + 0x18:
		case DeviceModelNameWrite + 0x1C:

			adrs2 = adrs - DeviceModelNameWrite;

			if ((pCxpSt->size + adrs2) > 32)
				pCxpSt->size = 32 - adrs2;

			for (ix=0; ix<pCxpSt->size; ix+=4, pDataRecv++)
			{
				data1 = *pDataRecv;
				swapData32 = SWAP_L(data1);
				OUT32 ((FIRM_DATA_MODEL_ADRS + adrs2 + ix), swapData32);
			}

			break;

		//------------------------------------------------------------
		// DeviceManufacturerInfo設定
		//------------------------------------------------------------
		//case DeviceManufacturerInfo:
			//break;

		//------------------------------------------------------------
		// DeviceManufacturerInfoWrite設定
		//------------------------------------------------------------
		case DeviceManufacturerInfoWrite:
		case DeviceManufacturerInfoWrite + 0x04:
		case DeviceManufacturerInfoWrite + 0x08:
		case DeviceManufacturerInfoWrite + 0x0C:
		case DeviceManufacturerInfoWrite + 0x10:
		case DeviceManufacturerInfoWrite + 0x14:
		case DeviceManufacturerInfoWrite + 0x18:
		case DeviceManufacturerInfoWrite + 0x1C:
		case DeviceManufacturerInfoWrite + 0x20:
		case DeviceManufacturerInfoWrite + 0x24:
		case DeviceManufacturerInfoWrite + 0x28:
		case DeviceManufacturerInfoWrite + 0x2C:

			adrs2 = adrs - DeviceManufacturerInfoWrite;

			if ((pCxpSt->size + adrs2) > 32)
				pCxpSt->size = 32 - adrs2;

			for (ix=0; ix<pCxpSt->size; ix+=4, pDataRecv++)
			{
				data1 = *pDataRecv;
				swapData32 = SWAP_L(data1);
				OUT32 ((FIRM_DATA_MANUFACTURE_ADRS + adrs2 + ix), swapData32);
			}

			break;

		//------------------------------------------------------------
		// DeviceVersion設定
		//------------------------------------------------------------
		//case DeviceVersion:
			//break;

		//------------------------------------------------------------
		// DeviceSerialNumber設定
		//------------------------------------------------------------
		//case DeviceSerialNumber:
			//break;

		//------------------------------------------------------------
		// DeviceUserIdentification設定
		//------------------------------------------------------------
		case DeviceUserIdentification:
		case DeviceUserIdentification+0x04:
		case DeviceUserIdentification+0x08:
		case DeviceUserIdentification+0x0C:
			//memcpy((char *)deviceUserID, (char *)pDataRecv, 16);

			ptr32= (unsigned int *)deviceUserID;
			for (ix=0; ix<pCxpSt->size; ix+=4, pDataRecv++, ptr32++)
			{
				swapData32 = *pDataRecv;
				*ptr32 = SWAP_L(swapData32);
			}

			setUserId(deviceUserID);
			break;

		//------------------------------------------------------------
		// WidthAddress設定
		//------------------------------------------------------------
		//case WidthAddress:
			//break;

		//------------------------------------------------------------
		// HeightAddress設定
		//------------------------------------------------------------
		//case HeightAddress:
			//break;

		//------------------------------------------------------------
		// AcquisitionModeAddress設定
		//------------------------------------------------------------
		//case AcquisitionModeAddress:
			//break;

		//------------------------------------------------------------
		// AcquistionStartAddress設定
		//------------------------------------------------------------
		//case AcquistionStartAddress:
			//break;

		//------------------------------------------------------------
		// AcquistionStopAddress設定
		//------------------------------------------------------------
		//case AcquistionStopAddress:
			//break;

		//------------------------------------------------------------
		// PixelFormatAddress設定
		//------------------------------------------------------------
		//case PixelFormatAddress:
			//break;

		//------------------------------------------------------------
		// DeviceTapGeometryAddress設定
		//------------------------------------------------------------
		//case DeviceTapGeometryAddress:
			//break;

		//------------------------------------------------------------
		// Image1StreamIDAddress設定
		//------------------------------------------------------------
		//case Image1StreamIDAddress:
			//break;

		//------------------------------------------------------------
		// Image2StreamIDAddress設定
		//------------------------------------------------------------
		//case Image2StreamIDAddress:
			//break;

		//------------------------------------------------------------
		// Image1StreamID設定
		//------------------------------------------------------------
		//case Image1StreamID:
			//break;

		//------------------------------------------------------------
		// Image2StreamID設定
		//------------------------------------------------------------
		//case Image2StreamID:
			//break;

		//------------------------------------------------------------
		// ImagenStreamIDAddress設定
		//------------------------------------------------------------
		//case ImagenStreamIDAddress:
			//break;

		//------------------------------------------------------------
		// 10.3.28 ConnectionReset設定
		// このレジスタに値0x00000001を書き込むと、デバイス接続がリセットされます。
		// マスター接続（接続0）を介してこの接続リセットコマンドを受信したデバイスは、接続リセットを実行し、200ms以内に検出接続構成をアクティブにする必要があります。
		// デバイスは、検出接続構成をアクティブにすると、レジスタをクリアして0x00000000に戻します。
		// ホストによる書き込みは、確認応答を待たずに「ファイアアンドフォーゲット」と見なす必要があります
		//------------------------------------------------------------
		case ConnectionReset:
			ConnectionReset_st = *pDataRecv;

			// Stream Disable
			OUT32(FPGA_CXP_S0_STREAM_EN_ADRS, 0);
		
			// Heat Beat ID
			OUT32 (FPGA_CXP_HSDC_HB_ID_ADRS, *pDataRecv);

			// LED
			//ledConnectStateForce();

			// 取り込み停止
			acquisitionAbort ();

			// Speed & Connection
			//@@@1cxpSetConnectionConfig (0/*port*/, ((1<<16) | CXP_RATE_3_125G));

			// Activate the master connection. Extension connections shall not be activated.
			// マスター接続をアクティブにします。 拡張接続はアクティブにしてはなりません。
			//@@@@@@@@@@

			// Initialize the bit rate for the master connection to the lowest discovery bit rate it supports (see section 4.3), with the corresponding value stored in ConnectionConfig.
			// 対応する値を ConnectionConfig に保存して、マスター接続のビット レートをそれがサポートする最小の発見ビット レートに初期化します (セクション 4.3 を参照)。
			// ConnectionConfig_stは設定しない為、初期化なし

			// MasterHostConnectionId shall be set to 0x00000000, indicating unknown Host Connection ID.
			// MasterHostConnectionId は、不明なホスト接続 ID を示す 0x00000000 に設定されます。
			ConnectionHostConnection_st = 0;

			// StreamPacketSizeMax shall be set to 0x00000000, indicating “not initialized”, and preventing any data packets being sent. The Device therefore sends just IDLE characters on all connections.
			// StreamPacketSizeMax は、「初期化されていない」ことを示す 0x00000000 に設定され、データ パケットが送信されないようにする必要があります。したがって、デバイスはすべての接続で IDLE 文字だけを送信します。
			StreamPacketSizeMax_st = 0;

			// The stream control in the Device shall be reset so that the Packet Tag in the stream data packet shall start from 0, and for a multi-connection Device, the first stream packet sent shall be on connection 0.
			// デバイスのストリーム制御は、ストリーム データ パケットのパケット タグが 0 から開始するようにリセットされ、マルチ接続デバイスの場合、送信される最初のストリーム パケットは接続 0 になります。
			//@@@@@@@@@@

			// TestMode and TestErrorCountSelector shall be set to 0x00000000.
			// TestMode および TestErrorCountSelector は 0x00000000 に設定されます。
			TestMode_st = 0;					// Test Mode

			// 送信テストモード停止
			//@@@1cxpRegWrite (port, CXP_REG_TEST_MODE_ADRS, 0, 1);

			TestErrorCountSelector_st = 0;		// Test Error Counter

			// All test mode counters shall be set to 0.
			// すべてのテスト モード カウンターを 0 に設定する必要があります。
			TestPacketCountTx_st = 0;
			TestPacketCountRx_st = 0;
			TestErrorCount_st = 0;

			// ComplianceTest shall be set to 0x00000000.
			// ComplianceTest は 0x00000000 に設定されます。
			if (ElectricalComplianceTest_st != 0)
			{
				gLinkStatusCheck = 1;
				setLed (LED_STATE, LED_OFF);
			}

			ElectricalComplianceTest_st = 0;
			OUT32 (FIRM_DATA_CXP_COMPLIANCE_TEST_MODE_ADRS, 0);
			ElectricalComplianceTest_Led = 0;

			// HsUpconnection shall be set to 0x00000000 if the Device does not support a high speed upconnection, or 0x00000001 if it does.
			// デバイスが高速アップ接続をサポートしていない場合、HsUpconnection は 0x00000000 に設定され、サポートされている場合は 0x00000001 に設定されます。
			// Not Support

			// Device trigger signal shall be set to 0.
			// デバイス トリガー信号は 0 に設定する必要があります。
			//@@@@@@@@@@

			// XmlManifestSelector shall be set to 0.
			// XmlManifestSelector は 0 に設定されます
			XmlManifestSelector_st = 0;

			// デバイスは、電源投入後に接続リセットも実行する必要があります。
			// コメント：ホストは最初は何に接続されているかを知らないため、すべての接続に接続リセットコマンドを送信します。
			// デバイスは拡張接続上のものを無視する必要があります。そうしないと、検出が失敗します。
			// コメント：一般に、値が0x00000001の場合、このレジスタを読み取ることはできません。

			// サイズ初期値
			pCxpSt->ackSize = 4;

			// データ設定
			*pDataSend = 0;

			// CXP Version
//#if defined (MODE_CXP_VERSION_20)
//			VersionUsed_st = 0x00020000;
//#endif
			break;

		//------------------------------------------------------------
		// 10.3.29 DeviceConnectionID設定
		// このレジスタは、図 37 に示すように、このレジスタが読み取られるデバイス接続のIDを提供します。
		// コメント: 接続 ID 0 は、接続がマスター接続であることを意味します。
		// これは静的レジスタですが、どの接続から読み取られるかによって値が異なります。
		//------------------------------------------------------------
		//case DeviceConnectionID:
			//break;

		//------------------------------------------------------------
		// 10.3.30 MasterHostConnectionID設定
		// このレジスタは、デバイス マスター接続に接続されたホスト接続のホスト接続のIDを保持します。
		// 値 0x00000000 は、不明なホスト ID を示すために予約されています。
		// 注: デバイスは、デバイス拡張接続への書き込みを無視します。
		// コメント: このレジスタは、デバイス検出プロセスの一部として、ホストによってシステム全体 (ホスト) の一意の 32 ビット ID 番号に設定されます (セクション 10.1.3 を参照)。
		// 接続のリセットにより、値 0x00000000 が設定されます。
		//------------------------------------------------------------
		case MasterHostConnectionID:
			ConnectionHostConnection_st = *pDataRecv;
			OUT32 (FPGA_CXP_HSDC_HB_ID_ADRS, *pDataRecv);
			break;

		//------------------------------------------------------------
		// 10.3.31 ControlPacketSizeMax設定
		// このレジスタは、ホストがデバイスから読み取ったり、デバイスに書き込んだり
 		// できる最大制御パケットサイズを提供するものとします。
		// デバイスに書き込むことができる最大制御パケット サイズを提供します。
		// サイズはバイトで定義され、4 バイトの倍数でなければなりません。
		// 定義されたサイズは、ペイロードだけでなく、パケット全体のサイズです。
		// コメント: 制御パケットのサイズは少なくとも 128 バイトです。
		// セクション 10.1.5 を参照してください。
		//------------------------------------------------------------
		//case ControlPacketSizeMax:
			//break;

		//------------------------------------------------------------
		// 10.3.32 StreamPacketSizeMax設定
		// このレジスタは、ホストが受け入れることができる最大ストリーム パケット サイズを保持します。
		// サイズはバイトで定義され、4 バイトの倍数でなければなりません。
		// デバイスは、このサイズまでの任意のパケット サイズを使用できます。
		// 定義されたサイズは、ペイロードだけでなく、パケット全体のサイズです。
		// コメント: このレジスタは、デバイス検出プロセスの一部としてホストによって設定されます。
		// 接続のリセットにより、値 0x00000000 が設定されます。
		// クション 10.1.5 を参照
		//------------------------------------------------------------
		case StreamPacketSizeMax:
			if (*pDataRecv > 2048)
				*pDataRecv = 2048;

			// Set Max Packet Size
			data1 = IN32(FPGA_CXP_S0_FLAG_SID_MZXSIZE_ADRS);
			data1 &= 0xffff;
			data1 |= (*pDataRecv << 16);
			OUT32 (FPGA_CXP_S0_FLAG_SID_MZXSIZE_ADRS, data1);

			StreamPacketSizeMax_st = *pDataRecv;
			break;

		//------------------------------------------------------------
		// 10.3.33 ConnectionConfig設定
		// このレジスタは、デバイス接続速度とアクティブなダウン接続の数の有効な組み合わせを保持します。
		// このレジスタに書き込むと、指定された接続の接続速度が設定され、サポートされている場合は高速接続が設定されます。
		// 新しい ConnectionConfig 値によって接続速度が変更される場合、デバイスは、元の接続速度での ConnectionConfig 
		// アクセスを確認するものとします。
		// したがって、接続速度を変更する前にアクセスを承認する必要があります。
		// セクション 10.1.6.1 を参照してください。
		// コメント: 接続速度と接続数の理論上のすべての組み合わせが使用できるとは限りません。
		// 1 つのレジスタを使用して、2 つの変数が同時に設定されるようにします。
		// XML ファイルと製品ドキュメントには、デバイスの有効な組み合わせが記載されています。
		// 接続のリセットは、選択した発見率と 1 つの接続に対応する値を設定します。
		// 1.250 0x28
		// 2.500 0x30
		// 3.125 0x38
		// 5.000 0x40
		// 6.250 0x48
		// 10.000 0x50
		// 12.500 0x58
		//------------------------------------------------------------
		case ConnectionConfig:
			if (gDebugAAA != 0)	//@@@@@1
			status = cxpSetConnectionConfig (port, *pDataRecv);
			break;

		//------------------------------------------------------------
		// 10.3.34 ConnectionConfigDefault設定
		// このレジスタは、デバイスがデフォルト モードで動作できるようにする
		// ConnectionConfig レジスタの値を提供します。
		// デバイスが複数の ConnectionConfig モードで動作できる場合、ユーザーが 
		// ConnectionConfigDefault を再プログラムできるように、製造元のレジスタ スペースを
		// 介してメカニズムを提供することをお勧めします。
		// コメント: これにより、ユーザーは次のことができます。
		// ユーザーのシステムに適している場合は、1 つの接続で動作するように 2 つの接続カメラをセットアップします。
		// 予期されるメカニズムは、製造元固有のスペースのレジスタを使用して、特別なユーティリティ プログラムを使用して 
		// ConnectionConfigDefault を変更する「値のロック解除/更新/コミット」です。
		// セクション 10.1.3 を参照
		//------------------------------------------------------------
		//case ConnectionConfigDefault:
			//break;

		//------------------------------------------------------------
		// 10.3.35 TestMode設定
		// このレジスタに値 0x00000001 を書き込むと、デバイスからホストへのテスト パケットの送信が有効になります。
		// 値 0x00000000 は、通常の操作を許可します。
		// 値が 0x00000001 から 0x00000000 に変更されると、デバイスは現在送信されている 1024 個のテスト ワードのパケットを完了します。
		// セクション 8.7.4 を参照してください。
		// コメント: 接続リセットは値 0x00000000 を設定します
		//------------------------------------------------------------
		case TestMode:

			// 取り込み停止
			acquisitionAbort ();

			if (*pDataRecv == 0)
				data1 = 0;	// TestMode停止
			else
				data1 = FPGA_CXP_LSUC_TX_TEST_EN;	// TestMode開始

			// 送信テストモード開始
			OUT32 (FPGA_CXP_LSUC_TX_TEST_MODE_ADRS, data1);

			// TestModeステータス設定
			TestMode_st = *pDataRecv;

			// LED制御
			if (data1 == 1)
				cxpSetLed (LED_STATE, LED_DIAG);
			else
				cxpSetLed (LED_STATE, LED_OFF);

			break;

		//------------------------------------------------------------
		// 10.3.36 TestErrorCountSelector設定
		// このレジスタは、必要なテスト カウント [TestErrorCountSelector] レジスタを選択します。
		// これは、有効なデバイス接続 ID 0 .. n-1、またはオプションの高速アップ接続用の n を保持します。
		// コメント: 接続リセットは値 0x00000000 を設定します
		//------------------------------------------------------------
		case TestErrorCountSelector:
			TestErrorCountSelector_st = *pDataRecv;
			break;

		//------------------------------------------------------------
		// 10.3.37 TestErrorCount設定
		// このレジスタは、レジスタ TestErrorCountSelector によって参照される接続の現在の接続エラー カウントを提供します。
		// このレジスタに 0x00000000 を書き込むと、レジスタ TestErrorCountSelector によって参照される接続の
		// 接続エラー カウントがゼロにリセットされます。
		// コメント: 接続リセットは、すべての接続テスト カウンターをゼロに設定します。
		// エラー カウントは、テスト パケットで受信された不正なワードの数です (セクション 8.7.3 を参照)。
		//------------------------------------------------------------
		case TestErrorCount:
			TestErrorCount_st = *pDataRecv;
			break;

		//------------------------------------------------------------
		// 10.3.38 TestPacketCountTx設定
		// このレジスタは、レジスタ TestErrorCountSelector によって参照される接続の現在の送信接続テスト パケット カウント
		// を提供します。セクション 8.7.3 を参照してください。
		// このレジスタに 0x00000000 を書き込むと、レジスタ TestErrorCountSelector によって参照される接続の送信された
		// 接続パケット カウントがゼロにリセットされます。
		// コメント: 接続リセットは、すべての接続テスト カウンターをゼロに設定します。
		//------------------------------------------------------------
		case TestPacketCountTx:
		case TestPacketCountTx+0x04:

			if (pCxpSt->size == 8)
			{
				data1 = *pDataRecv;
				pDataRecv++;
				data2 = *pDataRecv;

				TestPacketCountTx_st = (((unsigned long long)data1<<32) | data2);
			}
			else
			{
				TestPacketCountTx_st = *pDataRecv;
			}

			break;

		//------------------------------------------------------------
		// 10.3.39 TestPacketCountRx設定
		// このレジスタは、レジスタ TestErrorCountSelector によって参照される接続の現在の受信接続テスト パケット カウント
		// を提供します。
		// セクション 8.7.3 を参照してください。
		// このレジスタに 0x00000000 を書き込むと、レジスタ TestErrorCountSelector によって参照される接続の受信接続パケット数
		// がゼロにリセットされます。
		// コメント: 接続リセットは、すべての接続テスト カウンターをゼロに設定します。
		//------------------------------------------------------------
		case TestPacketCountRx:
		case TestPacketCountRx+0x04:

			if (pCxpSt->size == 8)
			{
				data1 = *pDataRecv;
				pDataRecv++;
				data2 = *pDataRecv;

				TestPacketCountRx_st = (((unsigned long long)data1<<32) | data2);
			}
			else
			{
				TestPacketCountRx_st = *pDataRecv;
			}
			break;

		//------------------------------------------------------------
		// 10.3.40 ElectricalComplianceTest設定
		// 実装される場合、これはデバイスの正式なコンプライアンス テストをサポートするための不揮発性レジスタでなければなりません。それ以外の時間には使用しないものとします。値 0x00000000 を書き込むと、通常の動作が許可されます。有効な ConnectionConfig 値を書き込むと、デバイスが次に電源投入され、アップ接続でコマンドが送信されないときに、次の動作が発生します。
		// ・ 接続速度と接続数は、このレジスタに書き込まれた値に従って設定されます。
		// ・ セクション 8.7.2 で定義されているテスト パケットは、これらの接続で出力されます。
		// ・ Control Acknowledges 以外の接続では、他のパケットを送信してはなりません。
		// ・ コネクタ インジケータ ランプ (セクション 5.4) が取り付けられている場合は、コンプライアンス テスト モードを表示するものとします。
		// ・ デバイスは、アップコネクションで送信されるパケットまたはアイドル ワードを必要としないものとします。
		// コメント: これにより、デバイスを物理層のコンプライアンス テスト用のアナライザーに接続できます。デバイスは、テストするビット レートに設定できます。ホストからプラグを抜いてアナライザーに差し込むと、すぐに必要なビット レートでテスト データが出力されます。ただし、ホストに接続すると、検出中の通常の ConnectionReset アクセスにより、このレジスタが 0x00000000 にリセットされるため、このレジスタへの偶発的なアクセス (たとえば、ホスト PC のクラッシュによる) は自動的に回復します。
		// コメント: 別の方法として、製造業者は、物理層のコンプライアンス テストを可能にする専用ファームウェアを備えたデバイスを提供できます。
		// セクション 8.7.3 を参照してください。
		// このに 0x00000000 リセットを行うと、レジスタ TestErrorCountSelector によって参照されるの接続されたパケット レジスタがゼロに接続されます。
		// コメント: 接続リセットは、すべての接続テスト カウンターをゼロに設定します。
		//------------------------------------------------------------
		case ElectricalComplianceTest:

			// ABA-013VIR & ABA-003VIRはConnectionConfigDefault_stのモードしか対応してない
			if ((*pDataRecv == 0) || (*pDataRecv == ConnectionConfigDefault_st))
			{
				ElectricalComplianceTest_st = *pDataRecv;
				OUT32 (FIRM_DATA_CXP_COMPLIANCE_TEST_MODE_ADRS, *pDataRecv);
			}
			break;

		//------------------------------------------------------------
		// 10.3.41 ElectricalComplianceTest設定
		// このレジスタは、オプションの高速アップ接続のデバイス サポートを示します。
		//------------------------------------------------------------
		//case HSupconnection:
			//break;

		//============================================================
		// CXP 2.0
		//============================================================
#if defined (MODE_CXP_VERSION_20)

		//------------------------------------------------------------
		// CapabilityRegister設定
		//------------------------------------------------------------
		//case CapabilityRegister:
			//break;

		//------------------------------------------------------------
		// FeatureControlRegister設定
		//------------------------------------------------------------
		case FeatureControlRegister:
			FeatureControlRegister_st = *pDataRecv;
			break;

		//------------------------------------------------------------
		// VersionsSupported設定
		//------------------------------------------------------------
		//case VersionsSupported:
			//break;

		//------------------------------------------------------------
		// VersionUsed設定
		//------------------------------------------------------------
		case VersionUsed:
			VersionUsed_st = *pDataRecv;
			break;

		//------------------------------------------------------------
		// LinkSharingStatus設定
		//------------------------------------------------------------
		//case LinkSharingStatus:
			//break;

		//------------------------------------------------------------
		// LinkSharingHorizontalStripeCount設定
		//------------------------------------------------------------
		//case LinkSharingHorizontalStripeCount:
			//break;

		//------------------------------------------------------------
		// LinkSharingVerticalStripeCount設定
		//------------------------------------------------------------
		//case LinkSharingVerticalStripeCount:
			//break;

		//------------------------------------------------------------
		// LinkSharingHorizontalOverlap設定
		//------------------------------------------------------------
		//case LinkSharingHorizontalOverlap:
			//break;

		//------------------------------------------------------------
		// LinkSharingVerticalOverlap設定
		//------------------------------------------------------------
		//case LinkSharingVerticalOverlap:
			//break;

		//------------------------------------------------------------
		// LinkSharingDuplicateStripe設定
		//------------------------------------------------------------
		//case LinkSharingDuplicateStripe:
			//break;
#endif

		//--------------------------------------------------------------------------------
		// Encoder Value設定
		//--------------------------------------------------------------------------------
		case EncoderValue8Byte:
		case EncoderValue8Byte+4:
			OUT32 (GENICAM_ENCODER_VALUE_HI_ADRS,  *pDataRecv);
			pDataRecv++;

			OUT32 (GENICAM_ENCODER_VALUE_LO_ADRS,  *pDataRecv);
			break;

		//--------------------------------------------------------------------------------
		// Encoder At Value設定
		//--------------------------------------------------------------------------------
		case EncoderValueAtReset8Byte:
		case EncoderValueAtReset8Byte+4:

			OUT32 (GENICAM_ENCODER_VALUE_AT_RESET_HI_ADRS,  *pDataRecv);
			pDataRecv++;

			OUT32 (GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS,  *pDataRecv);
			break;

		//------------------------------------------------------------
		// default設定
		//------------------------------------------------------------
		default:

			//------------------------------------------------------------
			// Update Buffer
			//------------------------------------------------------------
			if ((adrs >= FileAccessBuffer) && (adrs < BASE_FILE_BUFFER_MAX))
			{
				if ((status = cxpUploadBuffer (adrs, (unsigned char *)pDataRecv, pCxpSt->size)) != AVAL_STATUS_SUCCESS)
					goto _DONE;
				break;
			}

			//------------------------------------------------------------
			// Update XML File Name
			//------------------------------------------------------------
			if ((adrs >= BASE_NET_BOOTROM_XMLURL_FIRST) && (adrs < (BASE_NET_BOOTROM_XMLURL_FIRST + CXP_XML_URL_SIZE)))
			{
				adrs2 = BASE_NET_BOOTROM_XMLURL_FIRST - adrs;

				if ((pCxpSt->size + adrs2) > CXP_XML_URL_SIZE)
					pCxpSt->size = CXP_XML_URL_SIZE - adrs2;

				ptrDes8 = (unsigned char *)&gXmlFileNameUpdate[adrs2];
				ptrDes8_DDR = (unsigned char *)(FIRM_UPDATE_XML_FILE_NAME_ADRS + adrs2);
				for (ix=0; ix<pCxpSt->size; ix+=4, adrs2++, pDataRecv++)
				{
					data1 = *pDataRecv;
					swapData32 = SWAP_L(data1);
					ptrSrc8 = (unsigned char *)&swapData32;
					for (iy=0; iy<4; iy++, ptrSrc8++, ptrDes8++, ptrDes8_DDR++)
					{
						*ptrDes8 = *ptrSrc8;
						*ptrDes8_DDR = *ptrSrc8;
					}
				}

				break;
			}

			//------------------------------------------------------------
			// Start時の処理
			//------------------------------------------------------------
			if (adrs == GENICAM_ACQUISITION_START_ADRS)
				cxpSetFifoReset ();

			//------------------------------------------------------------
			// Wait Ack
			//------------------------------------------------------------
			if ((adrs == FPGA_AOI_BITWIDTH_ADRS) 				||
				(adrs == FPGA_XFLIP_CTRL_ADRS_FPGA)				||
				(adrs == FPGA_XFLIP_CTRL_ADRS_FPGA)				||
				(adrs == SensorGradComp8BitConvert)				||
				(adrs == (DeviceVendorNameOnEEPROM + 28))		||
				(adrs == (DeviceModelNameOnEEPROM + 28))		||
				(adrs == (DeviceManufacturerInfoOnEEPROM + 44))
				)
			{
				CXP_PACKET_ST cxpPaket;

				// wait status設定
				cxpPaket.status = CXP_ACK_CODE_WAIT;

				// Send Cmd Indication
				if (VersionUsed_st == CXP_VERSION_20)
				{
					if (pCxpSt->cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
					{
						cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK_TAG;

						// Cmd Tag
						cxpPaket.tag = pCxpSt->tag;
					}
					else
						cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
				}
				else
				{
					cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
				}

				// Cmd Indication
				cxpPaket.cmdIndication = pCxpSt->cmdIndication;

				// Ack Size
				cxpPaket.ackSize = 4;

				// Ack Buffer
				ptrL = (unsigned int *)(FIRM_CXP_SEND_DATA_CMD_ADRS + port * FIRM_CXP_DATA_INTERVAL + CXP_SEND_DATA_OFFSET);
				//cxpPaket.pData = ptrL;

				if (VersionUsed_st == CXP_VERSION_20)
				{
					if (pCxpSt->cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
						ptrL++;		// Tag追加(+4)
				}

				*ptrL = 1000; // 1s(ms単位)

				// Ack Packet設定
				cxpSetAckPacket (port, &cxpPaket);
			}


			//------------------------------------------------------------
			// Execute系(先にAckだけ返す)
			//------------------------------------------------------------
			if ((adrs == DeviceVendoroWriteCmd)						||
				(adrs == DeviceModelWriteCmd)						||
				(adrs == DeviceManufacturerInfoWriteCmd)			||
				(adrs == UserSetLoad)								||
				(adrs == UserSetSave)								||
				(adrs == UserSetFactory)							||
				(adrs == UserSetDefault)							||
				(adrs == DefectivePixelCorrectionLoad)				||
				(adrs == DefectivePixelCorrectionLoadAdmin)			||
				(adrs == DefectivePixelCorrectionAdjustment)		||
				(adrs == DefectivePixelCorrectionDetection)			||
				(adrs == DefectivePixelCoordinateX)					||
				(adrs == DefectivePixelCoordinateY)					||
				(adrs == DefectivePixelApply)						||
				(adrs == DefectivePixelRemove)						||
				(adrs == DefectivePixelReset)						||
				(adrs == DefectivePixelCorrectionSave)				||
				(adrs == DefectivePixelCorrectionSaveAdmin)			||
				(adrs == DefectivePixelCorrectionFactory)			||
				(adrs == FlatFieldCorrectionSetLoad)				||
				(adrs == FlatFieldCorrectionSetSave)				||
				(adrs == FlatFieldCorrectionDarkAdjustment)			||
				(adrs == FlatFieldCorrectionBrightAdjustment)		||
				(adrs == FlatFieldCorrectionShadinLinegAdjustment)	||
				(adrs == FlatFieldCorrectionFactory)				||
				(adrs == FlatFieldCorrectionSetCorrectionMode)		||
				(adrs == HighSpeedModeCmd)							||
				(adrs == Diagnostic)								||
				(adrs == DeviceDrrsCommand)							||
				(adrs == FileOperationExecute))
			{

				// Ack返信Flag設定
				gCxpAckDoneFlag = 1;

				// status設定
				pCxpSt->status = 0;

				// Send Cmd Indication
				if (VersionUsed_st == CXP_VERSION_20)
				{
					if (pCxpSt->cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
						pCxpSt->sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK_TAG;
					else
						pCxpSt->sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
				}
				else
				{
					pCxpSt->sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;
				}

				// Ack Packet設定
				cxpSetAckPacket (port, pCxpSt);
				sprintf (gLogMsgBuff,"Set User Execute : adrs=0x%08x, size=0x%08x, data=0x%08x, status =0x%08x\n", pCxpSt->adrs, pCxpSt->size, *pCxpSt->pData, status);
			}


			//------------------------------------------------------------
			// Set User
			//------------------------------------------------------------
			set_user_reg (adrs, *pDataRecv, (unsigned short *)&status16);


			//------------------------------------------------------------
			// Bit変更時の処理
			//------------------------------------------------------------
			if (adrs == FPGA_AOI_BITWIDTH_ADRS)
			{
				int startMode = 0;

				// Start Status
				acquisitionGetStartFlag (&startMode);

				// 取り込み停止
				acquisitionAbort ();

				// FIFO Reset
				cxpSetFifoReset ();

				// 取り込み開始
				if (startMode != 0)
					acquisitionStart ();
			}

			status = status16;

			break;
	}

	sprintf (gLogMsgBuff,"adrs=0x%08x, size=0x%08x, data=0x%08x, status=0x%08x\n", pCxpSt->adrs, pCxpSt->size, *pCxpSt->pData, status);
	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Get User
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pCxpSt				：CXPパケットデータを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetUser (int port, CXP_PACKET_ST *pCxpSt)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs, adrs2, offset;
    unsigned int *pData, *pData2;
	unsigned short status16;
	int index;
	unsigned int data32, swapData32;
	unsigned int data1, data2;
	unsigned int ix;
	unsigned int amari;

	sprintf (gLogMsgBuff,"adrs=0x%08x, size=0x%08x\n", pCxpSt->adrs, pCxpSt->size);
	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	
	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Get Use port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pCxpSt Parameter
	if (pCxpSt == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Get User pCxpSt NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pData Parameter
	if (pCxpSt->pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Get User pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// アドレス取得
	adrs = pCxpSt->adrs;

	// 送信データ格納位置取得(0x00:Ackコード/0x04=Size/0x08=データなので0x08のオフセットを付加)
	pData = (unsigned int *)(FIRM_CXP_SEND_DATA_CMD_ADRS + port * FIRM_CXP_DATA_INTERVAL + CXP_SEND_DATA_OFFSET);

	if (VersionUsed_st == CXP_VERSION_20)
	{
		if (pCxpSt->cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
			pData++;		// Tag追加(+4)
	}

	pData2 = pData;

	// サイズ初期値
	pCxpSt->ackSize = pCxpSt->size;

	switch (adrs)
	{
		//------------------------------------------------------------
		// Standard取得
		//------------------------------------------------------------
		case Standard:
			*pData = 0xC0A79AE5;
			break;

		//------------------------------------------------------------
		// Revision取得
		//------------------------------------------------------------
		case Revision:
			*pData = CXP_VERSION;
			break;

		//------------------------------------------------------------
		// XmlManifestSize取得
		//------------------------------------------------------------
		case XmlManifestSize:
			*pData = XmlManifestSize_st;
			break;

		//------------------------------------------------------------
		// XmlManifestSelector取得
		//------------------------------------------------------------
		case XmlManifestSelector:
			*pData = XmlManifestSelector_st;
			break;
	
		//------------------------------------------------------------
		// XmlVersion取得
		//------------------------------------------------------------
		case XmlVersion:
			*pData = CXP_XML_VERSION;
			break;

		//------------------------------------------------------------
		// XmlSchemaVersion取得
		//------------------------------------------------------------
		case XmlSchemaVersion:
			*pData = CXP_XML_SCHEMA_VERSION;
			break;

		//------------------------------------------------------------
		// XmlUrlAddress取得
		//------------------------------------------------------------
		case XmlUrlAddress:
			*pData = DevicePrimaryURL;
			break;

		//------------------------------------------------------------
		// Iidc2Address取得
		//------------------------------------------------------------
		case Iidc2Address:
			*pData = 0x0;
			break;

		//------------------------------------------------------------
		// CXP Interface FPGA Version取得
		//------------------------------------------------------------
		case IfFpgaVersion:
			data1 = IN32(FIRM_DATA_IF_VERSION_ADRS);

			data2 = data1>>4 & 0x0f;
			if(data2 >= 0x0a)
				data2++;
			swapData32  = (data2 + 0x30) << 0;	// 数値を文字列に変換

			swapData32 |= (0x2e << 8);			// '.'

			data2 = data1>>0 & 0x0f;
			if(data2 >= 0x0a)
				data2++;

			swapData32 |= (data2 + 0x30) << 16;	// 数値を文字列に変換

			*pData = SWAP_L (swapData32);

			break;

		//------------------------------------------------------------
		// DeviceVendorName取得
		//------------------------------------------------------------
		case DeviceVendorName:
		case DeviceVendorName + 0x04:
		case DeviceVendorName + 0x08:
		case DeviceVendorName + 0x0C:
		case DeviceVendorName + 0x10:
		case DeviceVendorName + 0x14:
		case DeviceVendorName + 0x18:
		case DeviceVendorName + 0x1C:

		case DeviceVendorNameWrite:
		case DeviceVendorNameWrite + 0x04:
		case DeviceVendorNameWrite + 0x08:
		case DeviceVendorNameWrite + 0x0C:
		case DeviceVendorNameWrite + 0x10:
		case DeviceVendorNameWrite + 0x14:
		case DeviceVendorNameWrite + 0x18:
		case DeviceVendorNameWrite + 0x1C:

			if ((adrs >= DeviceVendorNameWrite) && (adrs <= (DeviceVendorNameWrite + 32)))
				adrs2 = adrs - DeviceVendorNameWrite;
			else
				adrs2 = adrs - DeviceVendorName;

			if ((pCxpSt->ackSize + adrs2) > 32)
				pCxpSt->ackSize = 32 - adrs2;

			for (ix=0; ix<pCxpSt->ackSize; ix+=4, pData++)
			{
				swapData32 = IN32 ((FIRM_DATA_VENDOR_ADRS + adrs2 + ix));
				*pData = SWAP_L(swapData32);
			}

			break;

		//------------------------------------------------------------
		// DeviceModelName取得
		//------------------------------------------------------------
		case DeviceModelName:
		case DeviceModelName + 0x04:
		case DeviceModelName + 0x08:
		case DeviceModelName + 0x0C:
		case DeviceModelName + 0x10:
		case DeviceModelName + 0x14:
		case DeviceModelName + 0x18:
		case DeviceModelName + 0x1C:

		case DeviceModelNameWrite:
		case DeviceModelNameWrite + 0x04:
		case DeviceModelNameWrite + 0x08:
		case DeviceModelNameWrite + 0x0C:
		case DeviceModelNameWrite + 0x10:
		case DeviceModelNameWrite + 0x14:
		case DeviceModelNameWrite + 0x18:
		case DeviceModelNameWrite + 0x1C:

			if ((adrs >= DeviceModelNameWrite) && (adrs <= (DeviceModelNameWrite + 32)))
				adrs2 = adrs - DeviceModelNameWrite;
			else
				adrs2 = adrs - DeviceModelName;

			if ((pCxpSt->ackSize + adrs2) > 32)
				pCxpSt->ackSize = 32 - adrs2;

			for (ix=0; ix<pCxpSt->ackSize; ix+=4, pData++)
			{
				swapData32 = IN32 ((FIRM_DATA_MODEL_ADRS + adrs2 + ix));
				*pData = SWAP_L(swapData32);
			}

			break;

		//------------------------------------------------------------
		// DeviceManufacturerInfo取得
		//------------------------------------------------------------
		case DeviceManufacturerInfo:
		case DeviceManufacturerInfo + 0x04:
		case DeviceManufacturerInfo + 0x08:
		case DeviceManufacturerInfo + 0x0C:
		case DeviceManufacturerInfo + 0x10:
		case DeviceManufacturerInfo + 0x14:
		case DeviceManufacturerInfo + 0x18:
		case DeviceManufacturerInfo + 0x1C:
		case DeviceManufacturerInfo + 0x20:
		case DeviceManufacturerInfo + 0x24:
		case DeviceManufacturerInfo + 0x28:
		case DeviceManufacturerInfo + 0x2C:

		case DeviceManufacturerInfoWrite:
		case DeviceManufacturerInfoWrite + 0x04:
		case DeviceManufacturerInfoWrite + 0x08:
		case DeviceManufacturerInfoWrite + 0x0C:
		case DeviceManufacturerInfoWrite + 0x10:
		case DeviceManufacturerInfoWrite + 0x14:
		case DeviceManufacturerInfoWrite + 0x18:
		case DeviceManufacturerInfoWrite + 0x1C:
		case DeviceManufacturerInfoWrite + 0x20:
		case DeviceManufacturerInfoWrite + 0x24:
		case DeviceManufacturerInfoWrite + 0x28:
		case DeviceManufacturerInfoWrite + 0x2C:

			if ((adrs >= DeviceManufacturerInfoWrite) && (adrs <= (DeviceManufacturerInfoWrite + 48)))
				adrs2 = adrs - DeviceManufacturerInfoWrite;
			else
				adrs2 = adrs - DeviceManufacturerInfo;

			if ((pCxpSt->ackSize + adrs2) > 48)
				pCxpSt->ackSize = adrs2 - 48;

			for (ix=0; ix<pCxpSt->ackSize; ix+=4, pData++)
			{
				swapData32 = IN32 ((FIRM_DATA_MANUFACTURE_ADRS + adrs2 + ix));
				*pData = SWAP_L(swapData32);
			}

			break;

		//------------------------------------------------------------
		// DeviceVersionInfo取得
		//------------------------------------------------------------
		case DeviceVersionInfo:
		case DeviceVersionInfo+0x04:
		case DeviceVersionInfo+0x08:
		case DeviceVersionInfo+0x0C:
		case DeviceVersionInfo+0x10:
		case DeviceVersionInfo+0x14:
		case DeviceVersionInfo+0x18:
		case DeviceVersionInfo+0x1C:

			adrs2 = adrs - DeviceVersionInfo;

			if ((pCxpSt->ackSize + adrs2) > 32)
				pCxpSt->ackSize = adrs2 - 32;

			for (ix=0; ix<pCxpSt->ackSize; ix+=4, pData++)
			{
				swapData32 = IN32 ((deviceVersion + adrs2 + ix));
				*pData = SWAP_L(swapData32);
			}

			break;

		//------------------------------------------------------------
		// DeviceFirmwareVersion取得
		//------------------------------------------------------------
		case DeviceFirmwareVersion:
		case DeviceFirmwareVersion+0x04:
		case DeviceFirmwareVersion+0x08:
		case DeviceFirmwareVersion+0x0C:
		case DeviceFirmwareVersion+0x10:
		case DeviceFirmwareVersion+0x14:
		case DeviceFirmwareVersion+0x18:
		case DeviceFirmwareVersion+0x1C:

			adrs2 = adrs - DeviceFirmwareVersion;

			if ((pCxpSt->ackSize + adrs2) > 32)
				pCxpSt->ackSize = adrs2 - 32;

			for (ix=0; ix<pCxpSt->ackSize; ix+=4, pData++)
			{
				swapData32 = IN32 ((deviceFirmwareVersion + adrs2 + ix));
				*pData = SWAP_L(swapData32);
			}

			break;

		//------------------------------------------------------------
		// DeviceSerialNumber取得
		//------------------------------------------------------------
		case DeviceSerialNumber:
		case DeviceSerialNumber+0x04:
		case DeviceSerialNumber+0x08:
		case DeviceSerialNumber+0x0C:

			adrs2 = adrs - DeviceSerialNumber;

			if ((pCxpSt->ackSize + adrs2) > 16)
				pCxpSt->ackSize = adrs2 - 16;

			for (ix=0; ix<pCxpSt->ackSize; ix+=4, pData++)
			{
				swapData32 = IN32 ((FIRM_DATA_BOARDID_ADRS + adrs2 + ix));
				*pData = SWAP_L(swapData32);
			}

			break;

		//--------------------------------------------------------------------------------
		// Device Sensor ID取得
		//--------------------------------------------------------------------------------
		case DeviceSensorID:
		case DeviceSensorID+0x04:
		case DeviceSensorID+0x08:
		case DeviceSensorID+0x0C:
		case DeviceSensorID+0x10:

			adrs2 = adrs - DeviceSensorID;

			if ((pCxpSt->ackSize + adrs2) > BOARD_PARAM_ALIGN)
				pCxpSt->ackSize = adrs2 - BOARD_PARAM_ALIGN;

			for (ix=0; ix<pCxpSt->ackSize; ix+=4, pData++)
			{
				swapData32 = IN32 ((FIRM_DATA_SENSORID_ADRS + adrs2 + ix));
				*pData = SWAP_L(swapData32);
			}

			break;

		//------------------------------------------------------------
		// DeviceUserIdentification取得
		//------------------------------------------------------------
		case DeviceUserIdentification:
		case DeviceUserIdentification+0x04:
		case DeviceUserIdentification+0x08:
		case DeviceUserIdentification+0x0C:

			adrs2 = adrs - DeviceUserIdentification;

			if ((pCxpSt->ackSize+adrs2) > 16)
				pCxpSt->ackSize = adrs2 - 16;

			//@@@1getUserId(deviceUserID);

			for (ix=0; ix<pCxpSt->ackSize; ix+=4, pData++)
			{
				//@@@1swapData32 = IN32 ((deviceUserID + adrs2 + ix));
				//@@@1*pData = SWAP_L(swapData32);
				*pData = 0;
			}

			break;

		//------------------------------------------------------------
		// WidthAddress取得
		//------------------------------------------------------------
		case WidthAddress:
			*pData = FPGA_AOI_XSIZE_ADRS;
			break;

		//------------------------------------------------------------
		// HeightAddress取得
		//------------------------------------------------------------
		case HeightAddress:
			*pData = FPGA_AOI_YSIZE_ADRS;
			break;

		//------------------------------------------------------------
		// AcquisitionModeAddress取得
		//------------------------------------------------------------
		case AcquisitionModeAddress:
			*pData = GENICAM_ACQUISITION_MODE_ADRS;
			break;

		//------------------------------------------------------------
		// AcquistionStartAddress取得
		//------------------------------------------------------------
		case AcquistionStartAddress:
			*pData = GENICAM_ACQUISITION_START_ADRS;
			break;

		//------------------------------------------------------------
		// AcquistionStopAddress取得
		//------------------------------------------------------------
		case AcquistionStopAddress:
			*pData = GENICAM_ACQUISITION_ABORT_ADRS;
			break;

		//------------------------------------------------------------
		// PixelFormatAddress取得
		//------------------------------------------------------------
		case PixelFormatAddress:
			*pData = FPGA_AOI_BITWIDTH_ADRS;
			break;

		//------------------------------------------------------------
		// DeviceTapGeometryAddress取得
		//------------------------------------------------------------
		case DeviceTapGeometryAddress:
			*pData = DeviceTapGeometry;
			break;

		//------------------------------------------------------------
		// Image1StreamIDAddress取得
		//------------------------------------------------------------
		case Image1StreamIDAddress:
			*pData = Image1StreamID;
			//*pData = 0x00;
			break;

		//------------------------------------------------------------
		// Image2StreamIDAddress取得
		//------------------------------------------------------------
		case Image2StreamIDAddress:
			*pData = Image2StreamID;
			break;

		//------------------------------------------------------------
		// Image1StreamID取得
		//------------------------------------------------------------
		case Image1StreamID:
			*pData = CXP_STREAM_ID1_NUMBER;
			break;

		//------------------------------------------------------------
		// Image2StreamID取得
		//------------------------------------------------------------
		case Image2StreamID:
			*pData = CXP_STREAM_ID2_NUMBER;
			break;

		//------------------------------------------------------------
		// ImagenStreamIDAddress取得
		//------------------------------------------------------------
		//case ImagenStreamIDAddress:
			//*pData = 0x00;
			//break;

		//------------------------------------------------------------
		// ConnectionReset取得
		//------------------------------------------------------------
		case ConnectionReset:
			*pData = ConnectionReset_st;
			break;

		//------------------------------------------------------------
		// DeviceConnectionID取得
		//------------------------------------------------------------
		case DeviceConnectionID:  // 0 = MasterHostConnectionID
			*pData = ConnectionDeviceConnection_st[port];
			break;

		//------------------------------------------------------------
		// MasterHostConnectionID取得
		//------------------------------------------------------------
		case MasterHostConnectionID:
			*pData = ConnectionHostConnection_st;
			break;

		//------------------------------------------------------------
		// ControlPacketSizeMax取得
		//------------------------------------------------------------
		case ControlPacketSizeMax:
			*pData = ControlPacketSizeMax_st;
			break;

		//------------------------------------------------------------
		// StreamPacketSizeMax取得
		//------------------------------------------------------------
		case StreamPacketSizeMax:
			*pData = StreamPacketSizeMax_st;
			break;

		//------------------------------------------------------------
		// ConnectionConfig取得
		//------------------------------------------------------------
		case ConnectionConfig:
			*pData = ConnectionConfig_st;
			//@@@1*pData  = ((1<<16) | 0x38);
			break;

		//------------------------------------------------------------
		// ConnectionConfigDefault取得
		//------------------------------------------------------------
		case ConnectionConfigDefault:
			//@@@*pData = ConnectionConfigDefault_st;
			*pData = ((1<<16) | 0x38);
			break;

		//------------------------------------------------------------
		// TestMode取得
		//------------------------------------------------------------
		case TestMode:
			*pData = TestMode_st;
			break;

		//------------------------------------------------------------
		// TestErrorCountSelector取得
		//------------------------------------------------------------
		case TestErrorCountSelector:
			*pData = TestErrorCountSelector_st;
			break;

		//------------------------------------------------------------
		// TestErrorCount取得
		//------------------------------------------------------------
		case TestErrorCount:

			// Get Test Packet Err Count
			if ((status = cxpGetTestPacketErrCount (port, &TestErrorCount_st)) != AVAL_STATUS_SUCCESS)
				break;

			*pData = TestErrorCount_st;
			break;

		//------------------------------------------------------------
		// TestPacketCountTx取得
		//------------------------------------------------------------
		case TestPacketCountTx:
		case TestPacketCountTx+0x04:
		
			// Get Test Packet Rx Count
			if ((status = cxpGetTestPacketRxCount (port, &TestPacketCountTx_st)) != AVAL_STATUS_SUCCESS)
				break;

			pCxpSt->ackSize = 8;
			*pData = 0;
			pData++;
			*pData = TestPacketCountTx_st;
			break;

		//------------------------------------------------------------
		// TestPacketCountRx取得
		//------------------------------------------------------------
		case TestPacketCountRx:
		case TestPacketCountRx+0x04:

			// Get Test Packet Rx Count
			if ((status = cxpGetTestPacketRxCount (port, &TestPacketCountRx_st)) != AVAL_STATUS_SUCCESS)
				break;

			pCxpSt->ackSize = 8;
			*pData = 0;
			pData++;
			*pData = TestPacketCountRx_st;
			break;

		//------------------------------------------------------------
		// ElectricalComplianceTest取得
		//------------------------------------------------------------
		case ElectricalComplianceTest:
			*pData = ElectricalComplianceTest_st;
			break;

#if !defined (MODE_CXP_VERSION_20)
		//------------------------------------------------------------
		// HSupconnection取得
		//------------------------------------------------------------
		case HSupconnection:
			*pData = 0;	 // not support
			break;
#endif

		//============================================================
		// CXP 2.0
		//============================================================
#if defined (MODE_CXP_VERSION_20)

		//------------------------------------------------------------
		// CapabilityRegister取得
		//------------------------------------------------------------
		case CapabilityRegister:
			*pData = 0;
			break;

		//------------------------------------------------------------
		// FeatureControlRegister取得
		//------------------------------------------------------------
		case FeatureControlRegister:
			*pData = FeatureControlRegister_st;
			break;

		//------------------------------------------------------------
		// VersionsSupported取得
		//------------------------------------------------------------
		case VersionsSupported:
#if !defined (MODE_CXP_VERSION_20)
			*pData = 0x02;	// CXP Ver.1.1.1
#else
			*pData = 0x06;	// CXP Ver.1.1.1 & 2.0
#endif
			break;

		//------------------------------------------------------------
		// VersionUsed取得
		//------------------------------------------------------------
		case VersionUsed:
			*pData = VersionUsed_st;
			break;

		//------------------------------------------------------------
		// LinkSharingStatus取得
		//------------------------------------------------------------
		case LinkSharingStatus:
			*pData = 0x00;
			break;

		//------------------------------------------------------------
		// LinkSharingHorizontalStripeCount取得
		//------------------------------------------------------------
		case LinkSharingHorizontalStripeCount:
			*pData = 0x00;
			break;

		//------------------------------------------------------------
		// LinkSharingVerticalStripeCount取得
		//------------------------------------------------------------
		case LinkSharingVerticalStripeCount:
			*pData = 0x00;
			break;

		//------------------------------------------------------------
		// LinkSharingHorizontalOverlap取得
		//------------------------------------------------------------
		case LinkSharingHorizontalOverlap:
			*pData = 0x00;
			break;

		//------------------------------------------------------------
		// LinkSharingVerticalOverlap取得
		//------------------------------------------------------------
		case LinkSharingVerticalOverlap:
			*pData = 0x00;
			break;

		//------------------------------------------------------------
		// LinkSharingDuplicateStripe取得
		//------------------------------------------------------------
		case LinkSharingDuplicateStripe:
			*pData = 0x00;
			break;
#endif

		//--------------------------------------------------------------------------------
		// Encoder Value取得
		//--------------------------------------------------------------------------------
		case EncoderValue8Byte:
		case EncoderValue8Byte+4:
			data1 = IN32 (GENICAM_ENCODER_VALUE_LO_ADRS);
			data2 = IN32 (GENICAM_ENCODER_VALUE_HI_ADRS);

			*pData = data2;
			pData++;

			*pData = data1;
			break;


		//--------------------------------------------------------------------------------
		// Encoder At Value取得
		//--------------------------------------------------------------------------------
		case EncoderValueAtReset8Byte:
		case EncoderValueAtReset8Byte+4:
			data1 = IN32 (GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS);
			data2 = IN32 (GENICAM_ENCODER_VALUE_AT_RESET_HI_ADRS);

			*pData = data2;
			pData++;
			*pData = data1;
			break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgHighCount取得
		//----------------------------------------------------------------------------------
        case AcquisitionTrgHighCount:
			if ((status = acquisitionGetTrgCountHigh (port, &data1)) != AVAL_STATUS_SUCCESS)
				break;
			
			*pData = data1;
			pData++;

	      	break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgLowCount取得
		//----------------------------------------------------------------------------------
        case AcquisitionTrgLowCount:
			if ((status = acquisitionGetTrgCountLow (port, &data1)) != AVAL_STATUS_SUCCESS)
				break;
			
			*pData = data1;
			pData++;

	      	break;

		//------------------------------------------------------------
		// default取得
		//------------------------------------------------------------
		default:
			//------------------------------------------------------------
			// XML File取得
			//------------------------------------------------------------
			if (((adrs >= xmlStartAddress) && (adrs < xmlStartAddress + xmlSize + 1024))
			||  ((adrs >= xmlStartAddressSecond) && (adrs < xmlStartAddressSecond + xmlSizeSecond + 1024)))
			{
				adrs2 = adrs - xmlStartAddress;
				pData2 = pData;
				for (ix=0; ix<pCxpSt->ackSize/4; ix++, *pData2++)
				{
					// XML Fileデータ取得
					*pData2 = IN32 ((FIRM_XML_FILE_ADRS+adrs2+(ix*4)));
					swapData32 = SWAP_L (*pData2);
					*pData2 = swapData32;
				}

				// Ver.2.7 Start
				amari = pCxpSt->ackSize%4;
				if (amari != 0)
				{
					// XML Fileデータ取得
					*pData2 = IN32 ((FIRM_XML_FILE_ADRS+adrs2+(ix*4)));
					//@@@1swapData32 = SWAP_L (*pData2);
					//@@@1*pData2 = swapData32;
				}
				// Ver.2.7 End
			}
			//------------------------------------------------------------
			// XML File Name
			//------------------------------------------------------------
			else if  (((adrs >= DevicePrimaryURL) && (adrs < DevicePrimaryURL + CXP_XML_URL_SIZE)) ||
                      ((adrs >= DeviceSecondaryURL) && (adrs < DeviceSecondaryURL + CXP_XML_URL_SIZE)))
			{

				// Primary & Secondary?
				if ((adrs >= DevicePrimaryURL) && (adrs < DevicePrimaryURL + CXP_XML_URL_SIZE))
					adrs2 = DevicePrimaryURL;
				else
					adrs2 = DeviceSecondaryURL;

				// オフセット
				offset = adrs - adrs2;

				// サイズがオーバーしている?
				if ((adrs + pCxpSt->ackSize) > (adrs + CXP_XML_URL_SIZE))
					pCxpSt->ackSize = CXP_XML_URL_SIZE - offset;

				for (index=0; index<pCxpSt->ackSize; index+=4, pData++)
				{
					data32 = ((gXmlFileName1[index+offset] << 24) | (gXmlFileName1[index+offset+1] << 16) | (gXmlFileName1[index+offset+2] <<8) | (gXmlFileName1[index+offset+3] << 0));
					*pData = data32;
				}
			}
			//------------------------------------------------------------
			// Update Buffer
			//------------------------------------------------------------
			else if ((adrs >= FileAccessBuffer) && (adrs < BASE_FILE_BUFFER_MAX))
			{
				if ((status = cxpDownloadBuffer (adrs, (unsigned char *)pData, pCxpSt->size)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

				break;
			}
			//------------------------------------------------------------
			// Other
			//------------------------------------------------------------
			else
			{
				*pData = get_user_reg (adrs, (unsigned short *)&status16);
				status = status16;
			}
			break;
	}
	
	// Show XML
	//if ((adrs >= 0x61000000) && (adrs < 0x61800000))
		//goto _DONE;

	sprintf (gLogMsgBuff,"adrs=0x%08x, size=0x%08x, data=0x%08x, status=0x%08x\n", adrs, pCxpSt->ackSize, *pData2, status);
	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Get Packet
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetCmdPacket (int port, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	unsigned int timeout;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Get Packet port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// カウントCheck
	for (timeout=0; timeout<CXP_COMMAND_PACKET_TIMEOUT; timeout++)
	{
#if 1
		data32 = IN32 (FPGA_CXP_LSUC_RX_SW_PKT_STATUS_ADRS);
		data32 &= (FPGA_CXP_LSUC_RX_SW_PKT_VAL_BIT | FPGA_CXP_LSUC_RX_SW_PKT_FIFI_EMPTY_BIT);
		if (data32 == FPGA_CXP_LSUC_RX_SW_PKT_VAL_BIT)
			break;
#else
		data32 = IN32 (FPGA_CXP_LSUC_RX_SW_PKT_STATUS_ADRS);
		if (data32 & FPGA_CXP_LSUC_RX_SW_PKT_VAL_BIT)
			break;
#endif
		usDelay (2);
	}

	// Check Timeout
	if (timeout >= CXP_COMMAND_PACKET_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP FIFO Read Timeout.\n");
		goto _DONE;
	}

	// データ取得
	data32 = IN32 (FPGA_CXP_LSUC_SW_RX_PKT_DATA_ADRS);
	*pData = data32;

//@@@@@@@@@@@@@@@@@@@@@
	//DEBUG_PRINT_FORCE("[%d]0x%08x\n", ++gCxpAllRecvCount, data32);
//@@@@@@@@@@@@@@@@@@@@@
	
	// 受信データをバッファに格納
	cxpRecvBuffer (port, *pData);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Set Ack Packet
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCxpSt				：CXPパケットデータを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetAckPacket (int port, CXP_PACKET_ST *pCxpSt)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int *ptrL, *ptrL2;
	unsigned int dataSwap;
	unsigned int ackCode;
	unsigned int crc_value;
	unsigned int count = 0;
	unsigned char cxpSendCount = 0;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Ack Packet port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pCxpSt Parameter
	if (pCxpSt == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Ack Packet pCxpSt NULL Parameter Error.\n");
		goto _DONE;
	}


// Ver.2.7 Start
#if 0	//@@@1
	// Check size Parameter
	if ((pCxpSt->ackSize % 4) != 0)
	{
		status = CXP_ACK_CODE_LARGE_SIZE;
		sprintf (gLogMsgBuff, "CXP Ack Packet size(%d) Parameter Error\n", pCxpSt->ackSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		pCxpSt->size = 0;
		//goto _DONE;
		// エラーのパケットを返す
	}
#endif //@@@1
// Ver.2.7 End

	// Chekc ackSize Parameter
	if ((pCxpSt->ackSize/4) > CXP_REG_DATA_SIZE_MAX)
	{
		status = CXP_ACK_CODE_LARGE_SIZE;
		printf (gLogMsgBuff, "CXP Fifo Ack Size(0x%x) Error.(Max:0x%x)\n", pCxpSt->ackSize, CXP_REG_DATA_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		pCxpSt->size = 0;
		//goto _DONE;
		// エラーのパケットを返す
	}

	//------------------------------------------------------------
	// バッファ設定
	//------------------------------------------------------------
	ptrL = (unsigned int *)(unsigned long)(FIRM_CXP_SEND_DATA_CMD_ADRS + port * FIRM_CXP_DATA_INTERVAL);

	
	//------------------------------------------------------------
	// Recive Interrupt Disable
	//------------------------------------------------------------
	if ((status = cxpSetRecvIntMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// コメント
	//------------------------------------------------------------
	//返信時にはpCxpSt->pDataの領域をCRC計算用として使用

	//------------------------------------------------------------
	// 開始 K Code設定
	//------------------------------------------------------------
	if ((status = cxpWriteFifo32 (port, CXP_K_CODE_K27_7, CXP_K_CODE_K27_7)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	//*ptrL = CXP_K_CODE_K27_7;			// CRC計算用
	//ptrL++;
	//count++;
	cxpSendCount++;						// CXPレジスタ設定用

	//------------------------------------------------------------
	// Command Indication設定
	//------------------------------------------------------------
	if ((status = cxpWriteFifo32 (port, pCxpSt->sendcmdIndication, 0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//ptrL++;
	//count++;
	cxpSendCount++;						// CXPレジスタ設定用

	//------------------------------------------------------------
	// Tag設定
	//------------------------------------------------------------
	if (VersionUsed_st == CXP_VERSION_20)
	{
		if (pCxpSt->sendcmdIndication == CXP_DATA_PACKET_TYPE_ACK_TAG)
		{
			if ((status = cxpWriteFifo32 (port, pCxpSt->tag, 0)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			
			*ptrL = pCxpSt->tag;				// CRC計算用 // 2025.06.19追加
			ptrL++;								// 2025.06.19追加
			count++;							// 2025.06.19追加
			cxpSendCount++;						// CXPレジスタ設定用
		}
	}

	//------------------------------------------------------------
	// Command Code設定
	//------------------------------------------------------------
	if (pCxpSt->status == AVAL_STATUS_SUCCESS)
	{
		if (pCxpSt->ackSize == 0)	 // 返信データなし
			ackCode = CXP_ACK_CODE_OK_NO_REPLY;
		else	 		// 返信データあり
			ackCode = CXP_ACK_CODE_OK;
	}
	else if (pCxpSt->status == CXP_ACK_CODE_WAIT)
	{
		ackCode = CXP_ACK_CODE_WAIT;
	}
	else if (pCxpSt->status == CXP_ACK_CODE_CRC)
	{
		ackCode = CXP_ACK_CODE_CRC;
	}
	else if (pCxpSt->status == CXP_ACK_CODE_INVALID_ADRS)
	{
		ackCode = CXP_ACK_CODE_INVALID_ADRS;
	}
	else if (pCxpSt->status == CXP_ACK_CODE_INVALID_DATA)
	{
		ackCode = CXP_ACK_CODE_INVALID_DATA;
	}
	else if (pCxpSt->status == CXP_ACK_CODE_INVALID_CODE)
	{
		ackCode = CXP_ACK_CODE_INVALID_CODE;
	}
	else if (pCxpSt->status == CXP_ACK_CODE_READ_ONLY)
	{
		ackCode = CXP_ACK_CODE_READ_ONLY;
	}
	else if (pCxpSt->status == CXP_ACK_CODE_WRITE_ONLY)
	{
		ackCode = CXP_ACK_CODE_WRITE_ONLY;
	}
	else if (pCxpSt->status == CXP_ACK_CODE_LARGE_SIZE)
	{
		ackCode = CXP_ACK_CODE_LARGE_SIZE;
	}
	else if (pCxpSt->status == CXP_ACK_CODE_INCORRECT_SIZE)
	{
		ackCode = CXP_ACK_CODE_INCORRECT_SIZE;
	}
	else if (pCxpSt->status == CXP_ACK_CODE_MALFORMED_PACKET)
	{
		ackCode = CXP_ACK_CODE_MALFORMED_PACKET;
	}
	else
	{
		ackCode = CXP_ACK_CODE_INVALID_CODE;
	}

	// Write
	if ((status = cxpWriteFifo32 (port, ackCode, 0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	*ptrL = ackCode;						// CRC計算用
	ptrL++;
	count++;
	cxpSendCount++;							// CXPレジスタ設定用

	if ((ackCode == CXP_ACK_CODE_OK) || (ackCode == CXP_ACK_CODE_WAIT))
	{
		//------------------------------------------------------------
		// size
		//------------------------------------------------------------
		if ((status = cxpWriteFifo32 (port, pCxpSt->ackSize, 0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		*ptrL = pCxpSt->ackSize;				// CRC計算用
		ptrL++;
		count++;
		cxpSendCount++;							// CXPレジスタ設定用

		//------------------------------------------------------------
		// Data
		//------------------------------------------------------------

		// 送信データ格納位置取得(0x00:Ackコード/0x04=Size/0x08=データなので0x08のオフセットを付加)
		ptrL2 = (unsigned int *)(unsigned long)(FIRM_CXP_SEND_DATA_CMD_ADRS + port * FIRM_CXP_DATA_INTERVAL + CXP_SEND_DATA_OFFSET);

		if (VersionUsed_st == CXP_VERSION_20)
		{
			if (pCxpSt->cmdIndication == CXP_DATA_PACKET_TYPE_COMMAND_TAG)
				ptrL2++;		// Tag追加(+4)
		}

		for (i=0; i<(pCxpSt->ackSize/4); i++, ptrL2++, count++, cxpSendCount++)
		{
			if ((status = cxpWriteFifo32 (port, *ptrL2, 0)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		// Ver.2.7 Start
		if ((pCxpSt->ackSize%4) != 0)
		{
			if ((status = cxpWriteFifo32 (port, *ptrL2, 0)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			
			count++;
			cxpSendCount++;
		}
		// Ver.2.7 End

		//------------------------------------------------------------
		// CRC
		//------------------------------------------------------------
	   	cxpCalculateCrc32 (&crc_value, (unsigned int *)(FIRM_CXP_SEND_DATA_CMD_ADRS + port * FIRM_CXP_DATA_INTERVAL), count);

		dataSwap = SWAP_L (crc_value);
		if ((status = cxpWriteFifo32 (port, dataSwap, 0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		cxpSendCount++;						// CXPレジスタ設定用
	}


	//------------------------------------------------------------
	// 終了 K Code設定
	//------------------------------------------------------------
	if ((status = cxpWriteFifo32 (port, CXP_K_CODE_K29_7, CXP_K_CODE_K29_7)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	cxpSendCount++;						// CXPレジスタ設定用

_DONE:
	//------------------------------------------------------------
	// Recive Interrupt Enable
	//------------------------------------------------------------
	cxpSetRecvIntMode (MODE_ENABLE);

	return (status);
}


//**********************************************************************************
//	CXP CRC Calculation
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCrc				：CRCデータを格納するポインタ
//		pData				：データを格納するポインタ
//		count				：データ数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpCalculateCrc32 (unsigned int *pCrc, unsigned int *pData, unsigned int count)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int crc = 0xffffffff;
	unsigned int i;
	unsigned int index = 4;
	unsigned char buf[4];
	unsigned char bitSwap8;
	unsigned char *ptr8;
	unsigned int data32;
	unsigned int *crc_buffer32 = (unsigned int *)pData;

	// Check pCrc Parameter
	if (pCrc == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP CRC pCrc NULL Parameter Error.\n");
		goto _DONE;
	}

	// CRC Calculation
	for (i=0; i<count; i++, crc_buffer32++)
	{
		data32 = *crc_buffer32;

		index = 4;
		buf[0] = data32 & 0xff;
		buf[1] = (data32>>8) & 0xff;
		buf[2] = (data32>>16) & 0xff;
		buf[3] = (data32>>24) & 0xff;

		while (index--)
		{
			bitSwap8 = bitSwap (buf[index]);
			crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ bitSwap8) & 255];
		}
	}

	// CRC bit Swap
	ptr8 = (unsigned char *)&crc;
	data32 = 0;
	for (i=0; i<4; i++, ptr8++)
	{
		bitSwap8 = bitSwap (*ptr8);
		data32 |= (bitSwap8 << (24 - i*8));
	}

	// CRC格納
	*pCrc = data32;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Send Test Packet
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSendTestPacketAckCmd (unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *ptrB;
	unsigned char data8;
	int i;
	CXP_PACKET_ST cxpPaket;
	int port = 0;

	TestPacketCountTx_st++;;

	// Connection Test Packet
	cxpPaket.pData = (unsigned int *)(FIRM_CXP_SEND_DATA_CMD_ADRS + CXP_SEND_DATA_OFFSET);
	ptrB = (unsigned char *)cxpPaket.pData;

	// Data Make
	data8 = 0;
	for (i=0; i<size; i++, ptrB++, data8++)
		*ptrB = data8;

	// アドレス(使用しないが0にする。I2Cモードで動作するように)
	cxpPaket.adrs = 0;

	// status設定
	cxpPaket.status = AVAL_STATUS_SUCCESS;

	// Ack Dataサイズ
	cxpPaket.ackSize = size;

	// Size
	cxpPaket.size = CXP_CONNECTION_TEST_SIZE_BYTE;

	// Send Cmd Indication
	cxpPaket.sendcmdIndication = CXP_DATA_PACKET_TYPE_ACK;

	// CXPパケット作成
	cxpSetAckPacket (port, &cxpPaket);

	return (status);
}



//**********************************************************************************
//	CXP受信カウント数取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pCount				：受信カウントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetFifoSizeCount (int port, unsigned int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data, count;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP FIFO Size port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP FIFO Size Count pCount NULL Parameter Error.\n");
		goto _DONE;
	}

	// Fead Fifo Status
	if ((status = cxpGetReadFifoStatus (port, &data)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (data & FPGA_CXP_LSUC_RX_SW_PKT_VAL_BIT)
		*pCount = 1;
	else
		*pCount = 0;
	
_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Write FIFO
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		data				：送信データ(4byte)
//		mark				：マークデータ(開始の場合：CXP_K_CODE_K27_7/終了の場合：CXP_K_CODE_K29_7/それ以外=0) 	// 2025.06.20
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpWriteFifo32 (int port, unsigned int data, unsigned int mark)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int ctrl;
	unsigned int swapData;
	unsigned int ix;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Write32 FIFO port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//--------------------------------------------------------------------------------
	// Check Tx Ready
	//--------------------------------------------------------------------------------
	for (ix = 0; ix <CXP_TX_READY_TIMEOUT; ix++)
	{
		data32 = IN32(FPGA_CXP_LSUC_RX_SW_PKT_STATUS_ADRS);

		if ((data32&FPGA_CXP_HSDC_TX_READY_BIT) != 0)
			break;

		usDelay (2);
	}

	if (ix >= CXP_TX_READY_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP TX Ready Tiemout Error.\n");
		goto _DONE;
	}

	// FIFO Data Write
	OUT32 (FPGA_CXP_LSUC_SW_TX_PKT_DATA_ADRS, data);

_DONE:

	// 送信データをバッファに格納
	swapData = SWAP_L(data);
	cxpSendBuffer (port, swapData);

	return (status);
}


//**********************************************************************************
//	CXP送信バッファ格納
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		data				：送信データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSendBuffer (int port, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int count;
	unsigned int *pBuff32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Send Buffer port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 送信カウンタ取得
	count = IN32 ((FIRM_DATA_CXP_SEND_DATA_COUNT_MULTI_ADRS + port * 4));

	// 送信バッファの終端?
	if (count >= FIRM_CXP_SEND_DATA_ALL_SIZE)
		count = 0;

	// 送信バッファアドレス取得
	pBuff32 = (unsigned int *)(unsigned long)(FIRM_CXP_SEND_DATA_ALL_ADRS + port * FIRM_CXP_DATA_INTERVAL + count);

	// データ格納
	*pBuff32 = data;

	// 送信カウント更新
	OUT32 ((FIRM_DATA_CXP_SEND_DATA_COUNT_MULTI_ADRS + port * 4), (count + 4));

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP送信アドレス取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pAdrs				：送信アドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetSendCurrentAdrs (int port, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int count;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Send Buffer port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Send Adrs pAdrs NULL Parameter Error.\n");
		goto _DONE;
	}	

	// 送信カウンタ取得
	count = IN32 ((FIRM_DATA_CXP_SEND_DATA_COUNT_MULTI_ADRS + port * 4));

	// 送信バッファアドレス取得
	*pAdrs = FIRM_CXP_SEND_DATA_ALL_ADRS + port * FIRM_CXP_DATA_INTERVAL + count;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP受信アドレス取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pAdrs				：受信アドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetRecvCurrentAdrs (int port, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int count;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Recv Adrs port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Recv Adrs pAdrs NULL Parameter Error.\n");
		goto _DONE;
	}	

	// 受信カウンタ取得
	count = IN32 ((FIRM_DATA_CXP_RECV_DATA_COUNT_MULTI_ADRS + port * 4));

	// 受信バッファアドレス取得
	*pAdrs = FIRM_CXP_RECV_DATA_ALL_ADRS + port * FIRM_CXP_DATA_INTERVAL + count;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP受信バッファ格納
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		data				：受信データ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpRecvBuffer (int port, unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int count;
	unsigned int *pBuff32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Recv Buffer port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 受信カウンタ取得
	count = IN32 ((FIRM_DATA_CXP_RECV_DATA_COUNT_MULTI_ADRS + port * 4));

	// 受信バッファの終端?
	if (count >= FIRM_CXP_RECV_DATA_ALL_SIZE)
		count = 0;

	// 受信バッファアドレス取得
	pBuff32 = (unsigned int *)(FIRM_CXP_RECV_DATA_ALL_ADRS + port * FIRM_CXP_DATA_INTERVAL + count);

	// データ格納
	*pBuff32 = data;

	// 受信カウント更新
	OUT32 ((FIRM_DATA_CXP_RECV_DATA_COUNT_MULTI_ADRS + port * 4), (count + 4));

_DONE:

	return (status);
}


//**********************************************************************************
//	CXP FIFO Reset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetFifoReset (void)
{
	int status = AVAL_STATUS_SUCCESS;
#if 0 //@@@1
	unsigned int data32;

	// CTRL取得
	if ((status = cxpRegRead (0, CXP_REG_CTRL_ADRS, &data32, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FIFO Reset
	data32 |= CXP_REG_CTRL_FIFO_RESET;

	// CTRL設定
	if ((status = cxpRegWrite (0, CXP_REG_CTRL_ADRS, data32, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
#endif //@@@1
	return (status);
}


//**********************************************************************************
//	CXP Connection Config設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：Connection Config Data
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetConectionConfig (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// データ設定
	ConnectionConfig_st = data;

	return (status);
}


//**********************************************************************************
//	CXP Connection Config取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：Connection Config Dataを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetConectionConfig (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Connection Config pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// データ格納
	*pData = ConnectionConfig_st;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Bit設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pixelFormat			：Pixel Formatデータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetPixelFormat (int port, int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	unsigned int bitData32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set Bit port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if (bit == 12)
	{
		bitData32 = CXP_REG_PIXEL_MONO12;
	}
	else if (bit == 10)
	{
		bitData32 = CXP_REG_PIXEL_MONO10;
	}
	else
	{
		bitData32 = CXP_REG_PIXEL_MONO8;
	}

	// Pixel Format設定
	data32 = IN32 (FPGA_CXP_S0_TAPG_PIXEL_ADRS);
	data32 &= ~0xffff;
	data32 |= (bitData32 & 0xffff);
	OUT32 (FPGA_CXP_S0_TAPG_PIXEL_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP D Size L設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		wSize				：Width Size
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetDSizeL (int port, int wSize)
{
	int status = AVAL_STATUS_SUCCESS;
	int wMin, wMax;
	int bit;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set DSizeL port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Sensor Width Min
	wMin= 0;

	// Sensor Width Max
	wMax = sensorWidth ();

	wMax += IMG_WIDTH_IPU_MULTI_ADD_SIZE;

	// Check wSize Parameter
	if ((wSize <= wMin) || (wSize > wMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP D Size L(%d) Parameter Error.(Min:%d / Max:%d)\n", wSize, wMin, wMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Bit
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// サイズ計算
	data32 = wSize * bit / 32;

	// D Size L設定
	OUT32 (FPGA_CXP_S0_DSIZE_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Bit設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		bit					：bit
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetBit (int port, int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	int wsize;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set Bit port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Bit設定
	if ((status = cxpSetPixelFormat (port, bit)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Width取得
	if ((status = aoiGetWidth (&wsize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Data Size設定
	if ((status = cxpSetDSizeL (port, wsize)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FIFO設定
	if ((status = cxpSetFifoReset ()) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Width設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		size				：Width Size
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetWidth (int port, int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int wMin, wMax;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set Width port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Sensor Width Min
	wMin= 0;

	// Sensor Width Max
	wMax = sensorWidth ();

	wMax += IMG_WIDTH_IPU_MULTI_ADD_SIZE;

	// Check size Parameter
	if ((size <= wMin) || (size > wMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Width Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size, wMin, wMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	data32 = IN32 (FPGA_CXP_S0_XSIZE_OFFSET_ADRS);
	data32 &= ~0xffff;
	data32 |= (size & 0xffff);
	OUT32 (FPGA_CXP_S0_XSIZE_OFFSET_ADRS, data32);

	// Data Size設定
	if ((status = cxpSetDSizeL (port, size)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP OffsetX設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		offset				：Offset X
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetOffsetX (int port, int offset)
{
	int status = AVAL_STATUS_SUCCESS;
	int wMin, wMax;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set OffsetX port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Sensor Width Min
	wMin= 0;

	// Sensor Width Max
	wMax = sensorWidth ();

	// Check offset Parameter
	if ((offset < wMin) || (offset > (wMax-1)))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Offsetx(%d) Parameter Error.(Min:%d / Max:%d)\n", offset, wMin, wMax-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	data32 = IN32 (FPGA_CXP_S0_XSIZE_OFFSET_ADRS);
	data32 &= ~(0xffff0000);
	data32 |= ((offset & 0xffff)<<16);
	OUT32 (FPGA_CXP_S0_XSIZE_OFFSET_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Height設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		size				：Height Size
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetHeight (int port, int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int hMin, hMax;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set Height port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Sensor Height Min
	hMin= 0;

	// Sensor Height Max
	hMax = sensorHeight ();

	// Check size Parameter
	if ((size <= hMin) || (size > hMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Height Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size, hMin, hMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Height設定
	data32 = IN32 (FPGA_CXP_S0_YSIZE_OFFSET_ADRS);
	data32 &= ~0xffff;
	data32 |= (size & 0xffff);
	OUT32 (FPGA_CXP_S0_YSIZE_OFFSET_ADRS, data32);
	
_DONE:
	return (status);
}


//**********************************************************************************
//	CXP OffsetY設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		offset				：Offset Y
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetOffsetY (int port, int offset)
{
	int status = AVAL_STATUS_SUCCESS;
	int hMin, hMax;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set OffsetY port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Sensor Height Min
	hMin= 0;

	// Sensor Height Max
	hMax = sensorWidth ();

	// Check offset Parameter
	if ((offset < hMin) || (offset > (hMax-1)))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Offsety(%d) Parameter Error.(Min:%d / Max:%d)\n", offset, hMin, hMax-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status,gLogMsgBuff);
		goto _DONE;
	}

	// OffsetY設定
	data32 = IN32 (FPGA_CXP_S0_YSIZE_OFFSET_ADRS);
	data32 &= ~(0xffff0000);
	data32 |= ((offset & 0xffff)<<16);
	OUT32 (FPGA_CXP_S0_YSIZE_OFFSET_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Height Size, OffsetY設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetHeightParam (int port)
{
	int status = AVAL_STATUS_SUCCESS;
	int totaolSize, offsetYStart;
	int startMode = 0;
#if defined(MODE_BINNING)
	int binningMode;
#endif

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set Height Param port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Camera Total Height取得
	if ((status = fpgaRoiGetCameraHeightTotalSize (&totaolSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined(MODE_BINNING)
	// ビニングモード取得
	if ((status = aoiGetBinningY (&binningMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ビニングモード確認
	if(binningMode != MODE_DISABLE)
		totaolSize /= binningMode;
#endif

	// Height設定
	if ((status = cxpSetHeight (port, totaolSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Camera Offset Y取得
	if ((status = fpgaRoiGetOffsetY (&offsetYStart)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined(MODE_BINNING)
	// ビニングモード確認
	if(binningMode != MODE_DISABLE)
		offsetYStart /= binningMode;
#endif

	// Offsety設定
	if ((status = cxpSetOffsetY (port, offsetYStart)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	eeprom_write_dword
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		address
//		value
//	[ OUTPUT ]
//		-
//==================================================================================
void eeprom_write_dword(unsigned short address, unsigned int value)
{
}


//**********************************************************************************
//	eeprom_read_dword
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		address
//	[ OUTPUT ]
//		0
//==================================================================================
int  eeprom_read_dword (unsigned short address)
{
	return (0);
}


//**********************************************************************************
//	eeprom_write_dword
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		address
//		value
//	[ OUTPUT ]
//		-
//==================================================================================
void eeprom_write_word(unsigned short address, unsigned short value)
{
}


//**********************************************************************************
//	eeprom_read_dword
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		address
//	[ OUTPUT ]
//		0
//==================================================================================
int  eeprom_read_word (unsigned short address)
{
	return (0);
}


//**********************************************************************************
//	eeprom_write_dword
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		address
//		value
//	[ OUTPUT ]
//		-
//==================================================================================
void eeprom_write_byte(unsigned short address, unsigned char value)
{
}


//**********************************************************************************
//	eeprom_read_dword
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		address
//	[ OUTPUT ]
//		0
//==================================================================================
int  eeprom_read_byte(unsigned short address)
{
	return (0);
}


//**********************************************************************************
//	CXP Firmware Download(ダウンロードデータをバッファに書き込み)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：読み込みアドレス
//		pData				：データを格納するアドレス
//		size				：データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpDownloadBuffer (unsigned int adrs, unsigned char *pData, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pSrc, *pDst, *ptrB;
	unsigned int offset;
	int i;
	unsigned int adrs2;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Download Buffer pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check Size
	if ((adrs+size) > BASE_FILE_BUFFER_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Download Buffer Size Parameter Error. adrs = 0x%08x, size = 0x%08x\n", adrs, size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// オフセット
	offset = adrs - FileAccessBuffer;

	// 転送先格納アドレス
	ptrB = (unsigned char *)fileBuffer[fileSelector];
	pDst = (unsigned char *)(ptrB + offset);

	// 転送元格納アドレス
	pSrc = pData;

	// データCopy
	for (i=0; i<size; i++, pSrc++, pDst++)
		*pSrc = *pDst;

	adrs2 = adrs & BASE_FILE_BUFFER_MASK;
	fileResult[fileSelector][fileSel[fileSelector]] = adrs2 + size;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Firmware Update(アップデートデータをバッファに書き込み)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：書き込みアドレス
//		pData				：データを格納するアドレス
//		size				：データサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpUploadBuffer (unsigned int adrs, unsigned char *pData, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pSrc, *pDst, *ptrB;
	unsigned int offset;
	int i;
	unsigned int adrs2;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Update Buffer pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check Size
	if ((adrs+size) > BASE_FILE_BUFFER_MAX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Update Buffer Size Parameter Error. adrs = 0x%08x, size = 0x%08x\n", adrs, size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// オフセット
	offset = adrs - FileAccessBuffer;

	// 転送先格納アドレス
	ptrB = (unsigned char *)fileBuffer[fileSelector];
	pDst = (unsigned char *)(ptrB + offset);

	// 転送元格納アドレス
	pSrc = pData;

	// データCopy
	//@@@1for (i=0; i<size; i++, pSrc++, pDst++)
		//@@@1*pDst = *pSrc;

	adrs2 = adrs & BASE_FILE_BUFFER_MASK;
	fileResult[fileSelector][fileSel[fileSelector]] = adrs2 + size;

_DONE:
	return (status);
}


//**********************************************************************************
//	Cxpケーブル接続状態を取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pMode				：CXPケーブル接続状態を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetCableConnection (int port, int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Cable Connection port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pDatapModeParameter
	if (pMode == NULL)
    {
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Cable Connection pMode NULL Parameter Error.\n");
		goto _DONE;
    }

	// Cable Connection状態を取得する
	*pMode = IN32 ((FPGA_CXP_LINK_STATUS_ADRS + FPGA_CXP_REGISTER_PORT_INTERVAL * port)) & FPGA_CXP_LINK_STATUS_CONNECTION;

_DONE:
	return (status);
}


//**********************************************************************************
//	Cxp Get Test Packet Rx Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pData				：Rx Countを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetTestPacketRxCount (int port, unsigned long long *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Test Packet Rx Count port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Test Packet Rx Count NULL Parameter Error.\n");
		goto _DONE;
	}

	// Rx Count取得
	*pData = IN64 ((FPGA_CXP_LSUC_RX_TEST_NUM_ADRS + FPGA_CXP_REGISTER_PORT_INTERVAL * port));

_DONE:
	return (status);
}


//**********************************************************************************
//	Cxp Get Test Packet Rx Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pData				：Rx Countを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetTestPacketTxCount (int port, unsigned long long *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Test Packet Tx Count port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Test Packet Tx Count NULL Parameter Error.\n");
		goto _DONE;
	}

	// Rx Count取得
	*pData = IN64 ((FPGA_CXP_LSUC_TX_TEST_NUM_ADRS + FPGA_CXP_REGISTER_PORT_INTERVAL * port));

_DONE:
	return (status);
}


//**********************************************************************************
//	Cxp Get Test Packet Err Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pData				：Err Countを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetTestPacketErrCount (int port, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Test Packet Error Count port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Test Packet Error Count NULL Parameter Error.\n");
		goto _DONE;
	}

	// Rx Error Count取得
	*pData = IN32 ((FPGA_CXP_LSUC_RX_ERR_TEST_NUM_ADRS + FPGA_CXP_REGISTER_PORT_INTERVAL * port));

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Port設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetPort (int port)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// CXPのPort番号を指定する
	OUT32 (FIRM_CXP_PORT_ADRS, port);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Port取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pPort				：ポート番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetPort (int *pPort)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pPort Parameter
	if (pPort == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Get Port NULL Parameter Error.\n");
		goto _DONE;
	}

	// CXP Power Ctrl取得
	*pPort = IN32 (FIRM_CXP_PORT_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Rate設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		rate				：レート
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetRateData (unsigned int rate)
{
	int status = AVAL_STATUS_SUCCESS;

	OUT32 (FIRM_DATA_CXP_RATE_ADRS, rate);

	return (status);
}


//**********************************************************************************
//	CXP Rate取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pRate				：レートを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetRateData (unsigned int *pRate)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pRate Parameter
	if (pRate == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Get Rate NULL Parameter Error.\n");
		goto _DONE;
	}

	*pRate = IN32 (FIRM_DATA_CXP_RATE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP レジスタ値→CXPレート変換
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		regData				：レジスタ値
//		pCxpRateBps			：CXPレートを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetRegToSpeed (unsigned int regData, double *pCxpRateBps)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCxpRateBps Parameter
	if (pCxpRateBps == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Get Reg to Rate NULL Parameter Error.\n");
		goto _DONE;
	}

	regData &= 0xffff;

	// Change Rate
	if (regData == CXP_RATE_1_25G)
		*pCxpRateBps = 1.25;
	else if (regData == CXP_RATE_2_50G)
		*pCxpRateBps = 2.5;
	else if (regData == CXP_RATE_3_125G)
		*pCxpRateBps = 3.125;
	else if (regData == CXP_RATE_5_00G)
		*pCxpRateBps = 5;
	else if (regData == CXP_RATE_6_25G)
		*pCxpRateBps = 6.25;
	else if (regData == CXP_RATE_10_00G)
		*pCxpRateBps = 10;
	else if (regData == CXP_RATE_12_50G)
		*pCxpRateBps = 12.5;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Get Reg to Rate Data(0x%x) Parameter Erro.r\n", regData);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP速度 規格値→CXP IP設定値変換
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：規格値
//		pCxpRateBps			：CXP IP設定値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetDataToRegData (unsigned int data, unsigned int *pRegData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data2;

	// Check pRegData Parameter
	if (pRegData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Get Data to Reg Data NULL Parameter Error.\n");
		goto _DONE;
	}

	if (data == CXP_RATE_1_250GBPS)
	{
		data2 = CXP_REG_DRI_CTRL_1_25G;
	}
	else if (data == CXP_RATE_2_500GBPS)
	{
		data2 = CXP_REG_DRI_CTRL_2_5G;
	}
	else if (data == CXP_RATE_3_125GBPS)
	{
		data2 = CXP_REG_DRI_CTRL_3_125G;
	}
	else if (data == CXP_RATE_5_000GBPS)
	{
		data2 = CXP_REG_DRI_CTRL_5G;
	}
	else if (data == CXP_RATE_6_250GBPS)
	{
		data2 = CXP_REG_DRI_CTRL_6_25G;
	}
#if defined (MODE_CXP_VERSION_20)
	else if (data == CXP_RATE_10_000GBPS)
	{
		data2 = CXP_REG_DRI_CTRL_10_0G;
	}
	else if (data == CXP_RATE_12_500GBPS)
	{
		data2 = CXP_REG_DRI_CTRL_12_5G;
	}
#endif
	else
	{
		data2 = CXP_REG_DRI_CTRL_3_125G;
	}

	// 設定
	*pRegData = data2;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP レジスタ値→CXPコネクション変換
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		regData				：レジスタ値
//		pCxpRateBps			：CXPコネクションを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetRegToConnection (unsigned int regData, unsigned int *pCxpConnection)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCxpConnection Parameter
	if (pCxpConnection == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Get Reg to Connection NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get CXP Single or Dua
	*pCxpConnection = (regData >> 16) & 0xffff;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Port Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		mode				：0=Single / 1=Dualモード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetPortDual (int port, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Port Mode port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check mode Parameter
	if ((mode != CXP_PORT_SINGLE) && (mode != CXP_PORT_DUAL))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Port Mode mode(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, CXP_PORT_SINGLE, CXP_PORT_DUAL);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// CTRL取得
	//@@@1if ((status = cxpRegRead (0, CXP_REG_CTRL_ADRS, &data32, 4)) != AVAL_STATUS_SUCCESS)
		//@@@1goto _DONE;

	if (mode == CXP_PORT_DUAL)
		data32 |= CXP_REG_CTRL_PORT_DUAL;
	else
		data32 &= ~CXP_REG_CTRL_PORT_DUAL;

	// CTRL設定
	//@@@1if ((status = cxpRegWrite (0, CXP_REG_CTRL_ADRS, data32, 4)) != AVAL_STATUS_SUCCESS)
		//@@@1goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Port Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		mode				：Single/Dualモードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetPortDual (int port, int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Port Mode port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Port Mode NULL Parameter Error.\n");
		goto _DONE;
	}

	// CTRL取得
	if ((status = cxpRegRead (0, CXP_REG_CTRL_ADRS, &data32, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((data32 & CXP_REG_CTRL_PORT_DUAL) == 0)
		*pMode = CXP_PORT_SINGLE;
	else
		*pMode = CXP_PORT_DUAL;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Stream Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		mode				：0= 2Port&2Stream(AMS) / 1=2Port&1Stream(ABA)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetStreamMode (int port, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Port Stream Mode port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check mode Parameter
	if ((mode != CXP_MODE_SINGLE_STREAM) && (mode != CXP_MODE_MULTI_STREAM))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Port Stream Mode port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_MODE_SINGLE_STREAM, CXP_MODE_MULTI_STREAM);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// CTRL取得
	//@@@1if ((status = cxpRegRead (0, CXP_REG_CTRL_ADRS, &data32, 4)) != AVAL_STATUS_SUCCESS)
		//@@@1goto _DONE;

	if (mode == CXP_MODE_MULTI_STREAM)
		data32 &= ~CXP_REG_CTRL_2PORT_2STREAM;
	else
		data32 |= CXP_REG_CTRL_2PORT_2STREAM;

	// CTRL設定
	//@@@1if ((status = cxpRegWrite (0, CXP_REG_CTRL_ADRS, data32, 4)) != AVAL_STATUS_SUCCESS)
		//@@@1goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Stream Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		mode				：2Port&2Stream / 2Port&1Streamを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetStreamMode (int port, int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Stream Mode port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Stream Mode NULL Parameter Error.\n");
		goto _DONE;
	}

	// CTRL取得
	if ((status = cxpRegRead (0, CXP_REG_CTRL_ADRS, &data32, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((data32 & CXP_REG_CTRL_2PORT_2STREAM) == 0)
		*pMode = CXP_MODE_MULTI_STREAM;
	else
		*pMode = CXP_MODE_SINGLE_STREAM;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP LED設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		mode				：LED点灯 Mode
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetLed (int port, int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP LED port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check mode Parameter
	if ((mode < LED_PATTERN_MIN) || (mode > LED_PATTERN_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP LED Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", mode, LED_PATTERN_MIN, LED_PATTERN_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// LED設定
	//@@@1if ((status = cxpRegWrite (port, CXP_REG_LED_ADRS, (unsigned int)mode, 4)) != AVAL_STATUS_SUCCESS)
		//@@@1goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Port Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		mode				：LED点灯 Modeを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetLed (int port, int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP LED port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP LED NULL Parameter Error.\n");
		goto _DONE;
	}

	// LED取得
	//@@@1if ((status = cxpRegRead (port, CXP_REG_LED_ADRS, &data32, 4)) != AVAL_STATUS_SUCCESS)
		//@@@1goto _DONE;

	*pMode  = data32;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP StreamID取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pId					：StreamIDを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetStreamId (int port, unsigned int *pId)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Get Stream ID port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pId Parameter
	if (pId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Get Stream ID NULL Parameter Error.\n");
		goto _DONE;
	}

	// Steram ID取得
	*pId = INT32 (FPGA_CXP_S0_FLAG_SID_MZXSIZE_ADRS) & FPGA_CXP_S0_SID_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP StreamID設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		id					：StreamID
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetStreamId (int port, unsigned int id)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Set Stream ID port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Steram ID取得
	data32 = IN32 (FPGA_CXP_S0_FLAG_SID_MZXSIZE_ADRS);
	data32 &= ~FPGA_CXP_S0_SID_MASK;
	data32 |= (id & FPGA_CXP_S0_SID_MASK);
	OUT32 (FPGA_CXP_S0_FLAG_SID_MZXSIZE_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Rate Register設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：設定値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetRateReg (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int ix;
	int port = 0;
//@@@@@
//goto _DONE;
//@@@@@
	
	//--------------------------------------------------------------------------------
	// Check Busy
	//--------------------------------------------------------------------------------
	for (ix = 0; ix <CXP_REG_DRI_CTRL_BUSY_TIMEOUT; ix++)
	{
		data32 = IN32(CXP_REG_DRI_CTRLADRS);

		if ((data32&CXP_REG_DRI_CTRL_BUSY) == 0)
			break;

		usDelay (2);
	}

	if (ix >= CXP_REG_DRI_CTRL_BUSY_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Dynamic Reconfig Tiemout1 Error.\n");
		goto _DONE;
	}


	//--------------------------------------------------------------------------------
	// Dynamic Reconfig
	//--------------------------------------------------------------------------------
	OUT32 (CXP_REG_DRI_CTRLADRS, data);


	//--------------------------------------------------------------------------------
	// Check Busy
	//--------------------------------------------------------------------------------
	for (ix = 0; ix <CXP_REG_DRI_CTRL_BUSY_TIMEOUT; ix++)
	{
		data32 = IN32(CXP_REG_DRI_CTRLADRS);

		if ((data32&CXP_REG_DRI_CTRL_BUSY) == 0)
			break;

		usDelay (2);
	}

	//--------------------------------------------------------------------------------
	// Timeout
	//--------------------------------------------------------------------------------
	if (ix >= CXP_REG_DRI_CTRL_BUSY_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Dynamic Reconfig Tiemout2 Error.\n");
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP Rate設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：port番号
//		configData			：ConnectionConfigデータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpSetConnectionConfig (int port, unsigned int configData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data1, data2, data3;
	double dbFrameRate, dbFrameRateMax;
	unsigned int uiExp;
	int dataI1;

//@@@@@@@@1
	//goto _DONE;
//@@@@@@@@1
	
	// 取り込み停止
	//@@@1acquisitionAbort ();

	//--------------------------------------------------------------------------------
	// IPレジスタ設定値取得
	//--------------------------------------------------------------------------------
	data1 = configData & 0xffff;
	if ((status = cxpGetDataToRegData (data1, &data2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if 0	//@@@1

	// 受信FIFO Disable
	data3 = IN32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL));
	OUT32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL), (data3 & ~FPGA_CXP_RX_CMD_FIFO_CTRL_ENABLE));

	// Up Link Rate変更
	if ((data1 == CXP_RATE_10_000GBPS) || (data1 == CXP_RATE_12_500GBPS))
		OUT32 (FPGA_CXP_LSUC_CTRL_ADRS, FPGA_CXP_LSUC_LINK_SPEED_41M);
	else
		OUT32 (FPGA_CXP_LSUC_CTRL_ADRS, 0);

	// 受信FIFO Reset
	OUT32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL), (data3 | FPGA_CXP_RX_CMD_FIFO_CTRL_RESET));

	// 受信FIFO ENABLE
	OUT32 ((FPGA_CXP_RX_CMD_FIFO_CTRL_ADRS + port * FPGA_CXP_REGISTER_PORT_INTERVAL), (data3 | FPGA_CXP_RX_CMD_FIFO_CTRL_ENABLE));

	//--------------------------------------------------------------------------------
	// 現在のフレームレート/露光時間設定取得
	//--------------------------------------------------------------------------------
	data1 = IN32 (FIRM_DATA_FRAME_RATE_DEFAULT);

	if (data1 == 0)
	{
		if ((status = acquisitionGetFrameRate (&dbFrameRate))!= AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((status = acquisitionGetExposure (&uiExp))!= AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// フレームレート取得
		dbFrameRate = (double)(1.0 / (double)data1) * 1000000;

		// 露光時間取得
		uiExp = IN32 (FIRM_DATA_EXPOSURE_DEFAULT);
	}
#endif //@@@1
	
	//--------------------------------------------------------------------------------
	// CXP Rate設定
	//--------------------------------------------------------------------------------
	if ((status = cxpSetRateReg (data2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if 0	//@@@1
	//--------------------------------------------------------------------------------
	// Single / Dualモード設定
	//--------------------------------------------------------------------------------
	data1 = configData >> 16 & 0xffff;

	if ((status = cxpGetPortDual (port, (int *)&data3)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (data1 != data3)
	{
		if ((status = cxpSetPortDual (port, data1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif //@@@1

	//--------------------------------------------------------------------------------
	// ConnectionConfig設定
	//--------------------------------------------------------------------------------
	ConnectionConfig_st = configData;
	OUT32 (FIRM_DATA_CXP_CONNECTION_CONFIG, ConnectionConfig_st);

	// 設定値保存
	if ((status = cxpSetRateData (configData)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if 0 //@@@1
	//--------------------------------------------------------------------------------
	// Set H Interval
	//--------------------------------------------------------------------------------
	// センサ有効?
	if ((status = sensorGetValidMode ((int *)&data3)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (data3 == MODE_ENABLE)
	{
		// Get Bit
		if ((status = aoiGetBitWidth (&dataI1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	
		// Set H Interval
		if ((status = sensorSetHInterval (dataI1))!= AVAL_STATUS_SUCCESS)
		goto _DONE;
	}

	//--------------------------------------------------------------------------------
	// 現在のフレームレート/露光時間設定設定
	//--------------------------------------------------------------------------------
	// 最大レート取得
	if ((status = rateMax (&dbFrameRateMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (dbFrameRate > dbFrameRateMax)
		dbFrameRate = dbFrameRateMax;
	
	if ((status = acquisitionSetFrameRate (dbFrameRate))!= AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = acquisitionSetExposure (uiExp))!= AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif //@@@1

_DONE:
	return (status);
}


//**********************************************************************************
//	CXP受信カウント数取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//		pStatus				：ステータスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cxpGetReadFifoStatus (int port, unsigned int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data, count;

	// Check port Parameter
	if ((port < CXP_PORT_MIN) || (port > CXP_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "CXP Rx FIFO Stauts port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, CXP_PORT_MIN, CXP_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CXP, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "CXP Rx FIFO Status NULL Parameter Error.\n");
		goto _DONE;
	}

	// 受信ステータス
	*pStatus = IN32 (FPGA_CXP_LSUC_RX_SW_PKT_STATUS_ADRS) & FPGA_CXP_LSUC_RX_SW_PKT_VAL_BIT;

_DONE:
	return (status);
}

#endif // #if defined (IF_CXP)
#endif // #if defined (MODE_CXP)

// eof
