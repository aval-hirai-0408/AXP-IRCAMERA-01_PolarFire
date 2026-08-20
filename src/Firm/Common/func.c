//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// func.c - Camera Function Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int gInterFaceID;


//**********************************************************************************
//	Error Message Function
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		level				：メッセージレベル
//		fileName			：ファイル名を格納するポインタ
//		funcName			：関数名を格納するポインタ
//		fileLine			：ファイルの行数
//		errorNumber			：エラー番号
//		msg					：メッセージ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraLogMsg (int level, const char *fileName, const char *funcName, unsigned int fileLine, int errorNumber, char *msg)
{
	int status = AVAL_STATUS_SUCCESS;
	short devCode, errCode;
	unsigned int counter;
	unsigned int data32;
	unsigned short data16;
	unsigned char data8;
	char dataS8;
	double tempD;
	unsigned int dipsw;
	int consoleMode;

	// Get Dipsw
	if ((status = getDipsw (&dipsw)) != AVAL_STATUS_SUCCESS)
		dipsw = 0;

    // Error Device
    devCode = (short)(errorNumber>>16);

    // Error Code
    errCode = (short)(errorNumber&0xffff);

//@@@@@@@@@
	dipsw = 0;
//@@@@@@@@@
	// Checl Error Level & dipsw mode
	//if ((level != MSG_LEVEL_ERROR) && (dipsw != 0x3))
	if (dipsw != 0x3)
		goto _NEXT;
	
	// count
	counter = cameraGetCounter ();
	//DEBUG_PRINT_FORCE("%08x", data32);
	uartSend (UART_PORT0, (unsigned char *)&counter, 4);
	
	// Sensor Temp
	if ((status = peltierGetSensorTemp (&tempD)) != AVAL_STATUS_SUCCESS)
		dataS8 = (char)-127;
	else
		dataS8 = (char)tempD;
	//DEBUG_PRINT_FORCE("%02x", (unsigned char)dataS8);
	uartSend (UART_PORT0, (unsigned char *)&dataS8, 1);
	
	// FPGA Temp
	if ((status = xadcGetFpgaTemp (&tempD)) != AVAL_STATUS_SUCCESS)
		dataS8 = (char)-127;
	else
		dataS8 = (char)tempD;
	//DEBUG_PRINT_FORCE("%02x", (unsigned char)dataS8);
	uartSend (UART_PORT0, (unsigned char *)&dataS8, 1);

	// DI
	if ((status = digitalIoGetLineAllStatus (&data32)) != AVAL_STATUS_SUCCESS)
		data8 = 0;
	else
		data8 = (unsigned char)data32;

	//DEBUG_PRINT_FORCE ("%02x", data8);
	uartSend (UART_PORT0, (unsigned char *)&data8, 1);
	
	// DO
	if ((status = digitalIoGetAllValue (&data32)) != AVAL_STATUS_SUCCESS)
		data8 = 0;
	else
		data8 = (unsigned char)(data32 >> DIGITAL_USER_SELECT_MIN);

	//DEBUG_PRINT_FORCE ("%02x", data8);
	uartSend (UART_PORT0, (unsigned char *)&data8, 1);

	// トリガカウント
	data16 = 0;
	//DEBUG_PRINT_FORCE ("%04x", data16);
	uartSend (UART_PORT0, (unsigned char *)&data16, 2);
	
	// フレーム出力カウント
	data16 = 0;
	//DEBUG_PRINT_FORCE ("%04x", data16);
	uartSend (UART_PORT0, (unsigned char *)&data16, 2);
	
	// RS422カウント
	data16 = 0;
	//DEBUG_PRINT_FORCE ("%04x", data16);
	uartSend (UART_PORT0, (unsigned char *)&data16, 2);

	// RS422パルス
	data16 = 0;
	//DEBUG_PRINT_FORCE ("%04x", data16);
	uartSend (UART_PORT0, (unsigned char *)&data16, 2);

	// 電圧監視A
	data8= 0;
	//DEBUG_PRINT_FORCE ("%02x", data8);
	uartSend (UART_PORT0, (unsigned char *)&data8, 1);

	// 電圧監視B
	data8= 0;
	//DEBUG_PRINT_FORCE ("%02x", data8);
	uartSend (UART_PORT0, (unsigned char *)&data8, 1);

	// 電圧監視C
	data8= 0;
	//DEBUG_PRINT_FORCE ("%02x", data8);
	uartSend (UART_PORT0, (unsigned char *)&data8, 1);

	// 電圧監視D
	data8= 0;
	//DEBUG_PRINT_FORCE ("%02x", data8);
	uartSend (UART_PORT0, (unsigned char *)&data8, 1);

	// Level
	data8 = (unsigned char)level;
	//DEBUG_PRINT_FORCE ("%02x", data8);
	uartSend (UART_PORT0, (unsigned char *)&data8, 1);

	// Device Error
	//DEBUG_PRINT_FORCE ("%04x", (unsigned short)devCode);
	uartSend (UART_PORT0, (unsigned char *)&devCode, 2);
	
	// Error
	//DEBUG_PRINT_FORCE ("%04x", (unsigned short)errCode);
	uartSend (UART_PORT0, (unsigned char *)&errCode, 2);

	// Reserved
	data32 = 0xffffffff;
	//DEBUG_PRINT_FORCE ("%08x", data32);
	uartSend (UART_PORT0, (unsigned char *)&data32, 4);

	// Line
	data16 = (unsigned short)fileLine;
	//DEBUG_PRINT_FORCE ("%04x", data16);
	uartSend (UART_PORT0, (unsigned char *)&data16, 2);

	// Func Name
	DEBUG_PRINT_FORCE ("%-32.32s", funcName);

	// Message
	DEBUG_PRINT_FORCE ("%-192.192s", msg);

_NEXT:
	//@@@2if (level == MSG_LEVEL_ERROR)
	{
		if ((status = consoleGetMode (&consoleMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (consoleMode == 1)
		{
			DEBUG_PRINT_FORCE("Level=%d : Dev=%d : Err=%d : Line=%d : %s : %s", level, devCode, errCode, fileLine, funcName, msg);
		}
	}
	
_DONE:
	return (status);
}


//**********************************************************************************
//	Error Message Function
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		level				：メッセージレベル
//		fileName			：ファイル名を格納するポインタ
//		funcName			：関数名を格納するポインタ
//		fileLine			：ファイルの行数
//		errorNumber			：エラー番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraErrorFunc (int level, const char *fileName, const char *funcName, unsigned int fileLine, int errorNumber)
{
	int status = AVAL_STATUS_SUCCESS;
	short devCode, errCode;
	
	devCode = (short)(errorNumber>>16);
	errCode = (short)(errorNumber&0xffff);

	DEBUG_PRINT_FORCE("Level=%d : %s : Line=%d : Dev=%d : Err=%d : ", level, funcName, fileLine, devCode, errCode);
	
	return (status);
}



//**********************************************************************************
//	Error Message Function
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		level				：メッセージレベル
//		fileName			：ファイル名を格納するポインタ
//		funcName			：関数名を格納するポインタ
//		fileLine			：ファイルの行数
//		errorNumber			：エラー番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
unsigned int cameraGetCounter (void)
{
	unsigned int count;
	count = 0;	//@@@1
	return (count);
}

//**********************************************************************************
// Bit Swap
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		Byte data
//	[ OUTPUT ]
//		Bit Swap Data
//==================================================================================
unsigned char bitSwap (unsigned char byte)
{
	unsigned char swbyte;
	unsigned char readBit;
	unsigned char setBit;
	int i;
	
	swbyte = 0x00;
	for(i=0;i<8;i++)
	{
		readBit = 0x01;
		readBit <<= i;
		
		if(byte & readBit)
		{
			setBit = 0x80;
			setBit >>= i;
			
			swbyte |= setBit;
		}
	}

	return (swbyte);
}


//**********************************************************************************
// All Default
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int defaultAll (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int userNo;

	// FFC Default
	DEBUG_PRINT ("FFC Default\n");

#if defined	(MODE_FFC_FIRST)
	// 1次補正データ
	if ((status = ffcDefault ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif // #if defined	(MODE_FFC_FIRST)

	// DPC Default
	DEBUG_PRINT ("DPC Default\n");

	if ((status = dpcDefault ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// LUT Default
	DEBUG_PRINT ("LUT Default\n");

	if ((status = lutDefault ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// UserSet Default
	DEBUG_PRINT ("Userset Default\n");
	for (userNo=CAMERA_USER1_MODE; userNo<CAMERA_SAVE_NUM; userNo++)
	{
		if ((status = cameraParamUserMarkClear (userNo)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
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
	if ((status = cmdInterruptMode (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
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
int cmdExecuteStatus (int *pStatus)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pStatus Parameter
	if (pStatus == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,  __func__, __LINE__, status, "Cmd Exe Status NULL Parameter Error.\n");
		goto _DONE;
	}

	// Execute
	*pStatus = IN32 (FIRM_DATA_CMD_EXE_FLAG);

_DONE:
	return (status);
}

// eof
