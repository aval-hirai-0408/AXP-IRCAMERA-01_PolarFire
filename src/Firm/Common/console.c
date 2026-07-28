//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// console.c - Console Controll Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gConsoleMode = 0;
int gConsoleSelect = UART_PORT0;
int gConsoleUpdateSelect = UART_PORT0;
static char debugBuff[512];

// input/output関数宣言
DEF_DEBUG_PRINT 		DEBUG_PRINT;
DEF_DEBUG_PRINT_FORCE 	DEBUG_PRINT_FORCE;
DEF_DEBUG_INPUT_POLL 	DEBUG_INPUT_POLL;
DEF_DEBUG_INPUT			DEBUG_INPUT;
DEF_DEBUG_OUTPUT 		DEBUG_OUTPUT;


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int gInterFaceID;


//**********************************************************************************
//	ConSole Default Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int consoleDefaultInitialize (void)
{
	// コンソール設定
	consoleSelectMain (0);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ConSole Mode Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int consoleInitialize (void)
{
	// Console Mode Initialize
	consoleModeInitialize ();

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ConSole Mode Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int consoleModeInitialize (void)
{
	int status;
	int mode;

#if 0 //@@@1
	// Get Console Mode
	if ((status = cameraParamCommonReadMem (CAMERA_SAVE_COMMON_CONSOLE_MODE_ADRS, (unsigned int *)&mode)) != AVAL_STATUS_SUCCESS)
		return (status);

#else
	mode = 1;
#endif //@@@1


	// モード取得
	gConsoleMode = mode;

	// 管理外に設定
	OUT32 (FIRM_DATA_UART_CONSOLE_MODE_ADRS, mode);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ConSole ON/OFF
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Console Mode(0=OFF/1=ON)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int consoleSetMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check mode Parameter
	if ((mode != CONSOLE_MODE_ON) && (mode != CONSOLE_MODE_OFF))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Console Mode mode(%d) Parameter Error.(OFF:%d / ON:%d)\n", mode, CONSOLE_MODE_OFF, CONSOLE_MODE_ON);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Consoleモード設定
	gConsoleMode = mode;

	// 管理外に設定
	OUT32 (FIRM_DATA_UART_CONSOLE_MODE_ADRS, mode);

	// Save Console Mode(Memory)
	if ((status = cameraParamCommonWriteMem (CAMERA_SAVE_COMMON_CONSOLE_MODE_ADRS, mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Save Console Mode(ROM)
	if ((status = cameraParamCommonWriteRom (CAMERA_SAVE_COMMON_CONSOLE_MODE_ADRS, mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	ConSole ON/OFF
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Console Mode
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int consoleGetMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

//@@@1
*pMode = 1;
goto _DONE;
//@@@1
	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Console Mode pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Read Console Mode(Memory)
	if ((status = cameraParamCommonReadMem (CAMERA_SAVE_COMMON_CONSOLE_MODE_ADRS, (unsigned int *)&gConsoleMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Mode取得
	*pMode = IN32 (FIRM_DATA_UART_CONSOLE_MODE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// consoleSelect Main
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：ポート番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int consoleSelectMain (int port)
{
	unsigned int dipsw;
	
	// CameraLink
	if (gInterFaceID == INTERFACE_CAMERALINK)
	{
		// DIPSW取得
		if (getDipsw (&dipsw) != AVAL_STATUS_SUCCESS)
			dipsw = 0;

		if (dipsw != 0x03)
		{
			DEBUG_PRINT 		= printfDebugDouble;
			DEBUG_PRINT_FORCE 	= printfDebugDoubleForce;
			DEBUG_INPUT_POLL	= inputPollDouble;
			DEBUG_INPUT 		= inputCl;
			DEBUG_OUTPUT 		= outputCl;
		}
		else
		{
			DEBUG_PRINT 		= printfDebugGigE;
			DEBUG_PRINT_FORCE 	= printfDebugGigEForce;
			DEBUG_INPUT_POLL	= inputPollGigE;
			DEBUG_INPUT 		= inputGigE;
			DEBUG_OUTPUT 		= outputGigE;
		}
	}
	// GigE/CXP
	else if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20) || (gInterFaceID == INTERFACE_CXP))
	{
		DEBUG_PRINT 		= printfDebugGigE;
		DEBUG_PRINT_FORCE 	= printfDebugGigEForce;
		DEBUG_INPUT_POLL 	= inputPollGigE;
		DEBUG_INPUT 		= inputGigE;
		DEBUG_OUTPUT 		= outputGigE;

		#if 0// @@@1
		{
			DEBUG_PRINT 		= printfDebugGigE2;
			DEBUG_PRINT_FORCE 	= printfDebugGigEForce2;
			DEBUG_INPUT_POLL 	= inputPollGigE;
			DEBUG_INPUT 		= input;
			DEBUG_OUTPUT 		= outputGigE;
		}
		#endif //@@@1
	}
	// Other
	else
	{
		DEBUG_PRINT 		= printfDebugGigE;
		DEBUG_PRINT_FORCE	= printfDebugGigEForce;
		DEBUG_INPUT_POLL 	= inputPollGigE;
		DEBUG_INPUT 		= inputGigE;
		DEBUG_OUTPUT 		= outputGigE;
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	printfDebugCl
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
int printfDebugCl (const char *fmt, ...)
{
	va_list args;
	int i=0, len = 0;

	if (IN32 (FIRM_DATA_UART_CONSOLE_MODE_ADRS) != 0)
	{
		va_start (args, fmt);
		len = vsprintf (debugBuff, fmt, args);
		va_end (args);

		i = printfDebugPort (UART_PORT_CL, (char *)debugBuff, len);
	}

	return (i);
}


//**********************************************************************************
//	printfDebugGigE
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
int printfDebugGigE (const char *fmt, ...)
{
	va_list args;
	int i=0, len = 0;

	if (IN32 (FIRM_DATA_UART_CONSOLE_MODE_ADRS) != 0)
	{
		va_start (args, fmt);
		len = vsprintf (debugBuff, fmt, args);
		va_end (args);

		i = printfDebugPort (UART_PORT_GIGE, (char *)debugBuff, len);
	}

	return (i);
}


//**********************************************************************************
//	printfDebugDouble
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
int printfDebugDouble (const char *fmt, ...)
{
	va_list args;
	int i=0, len = 0;

	if (IN32 (FIRM_DATA_UART_CONSOLE_MODE_ADRS) != 0)
	{
		va_start (args, fmt);
		len = vsprintf (debugBuff, fmt, args);
		va_end (args);

		// CameraLinkのみ
		if (gInterFaceID == INTERFACE_CAMERALINK)
			i = printfDebugPort (UART_PORT_CL, (char *)debugBuff, len);

		i = printfDebugPort (UART_PORT_GIGE, (char *)debugBuff, len);
	}

	return (i);
}


//**********************************************************************************
//	printfDebugGigE2
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
int printfDebugGigE2 (const char *fmt, ...)
{
	va_list args;
	int i=0, len = 0;

	if (IN32 (FIRM_DATA_UART_CONSOLE_MODE_ADRS) != 0)
	{
		va_start (args, fmt);
		len = vsprintf (debugBuff, fmt, args);
		va_end (args);

		i = printfDebugPort2 (0, (char *)debugBuff, len);
	}

	return (i);
}


//**********************************************************************************
//	printfDebugClForce
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
int printfDebugClForce (const char *fmt, ...)
{
	va_list args;
	int i, len = 0;

	va_start (args, fmt);
	len = vsprintf (debugBuff, fmt, args);
	va_end (args);

	i = printfDebugPort (UART_PORT_CL, (char *)debugBuff, len);

	return (i);
}


//**********************************************************************************
//	printfDebugGigEForce
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
int printfDebugGigEForce (const char *fmt, ...)
{
	va_list args;
	int i, len = 0;

	va_start (args, fmt);
	len = vsprintf (debugBuff, fmt, args);
	va_end (args);

	i = printfDebugPort (UART_PORT_GIGE, (char *)debugBuff, len);
	return (i);
}


//**********************************************************************************
//	printfDebuDoubleForce
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
int printfDebugDoubleForce (const char *fmt, ...)
{
	va_list args;
	int i, len = 0;

	va_start (args, fmt);
	len = vsprintf (debugBuff, fmt, args);
	va_end (args);

	// CameraLinkのみ
	if (gInterFaceID == INTERFACE_CAMERALINK)
		i = printfDebugPort (UART_PORT_CL, (char *)debugBuff, len);
	
	i = printfDebugPort (UART_PORT_GIGE, (char *)debugBuff, len);

	return (i);
}


//**********************************************************************************
//	printfDebugGigEForce2
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
int printfDebugGigEForce2 (const char *fmt, ...)
{
	va_list args;
	int i=0, len = 0;

	if (IN32 (FIRM_DATA_UART_CONSOLE_MODE_ADRS) != 0)
	{
		va_start (args, fmt);
		len = vsprintf (debugBuff, fmt, args);
		va_end (args);

		i = printfDebugPort2 (0, (char *)debugBuff, len);
	}

	return (i);
}


//**********************************************************************************
//	printfDebugPort
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port		：ポート番号
//		pBuffer		：出力データを格納するポインタ
//		len			：出力データサイズ
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
int printfDebugPort (int port, char *pBuffer, int len)
{
	int i;
	char cr = CODE_CR;

	for (i=0; i<len; i++)
	{
		if (pBuffer[i] == CODE_LF)
		{
			uartSend (port, (unsigned char *)&cr, 1);	// CR出力
			continue;
		}

		uartSend (port, (unsigned char *)&pBuffer[i], 1);
	}
	
	return (i);
}


//**********************************************************************************
//	printfDebugPort2
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port		：ポート番号
//		pBuffer		：出力データを格納するポインタ
//		len			：出力データサイズ
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
int printfDebugPort2 (int port, char *pBuffer, int len)
{
	int i;

	for (i=0; i<len; i++)
	{
		if (pBuffer[i] == CODE_LF)
		{
			if (gInterFaceID == INTERFACE_CXP)
				outbyte (CODE_CR);	// CR出力

			continue;
		}

		outbyte (debugBuff[i]);
	}
	
	return (i);
}


//**********************************************************************************
//	inbyte
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
char inbyte (void)
{
//@@@1
//@@@1
//@@@1
	return (0);
}


//**********************************************************************************
//	inputChar
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
char input (void)
{
	return (inbyte());
}


//**********************************************************************************
//	inputCl
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
char inputCl (void)
{
	char c;

	uartRecv (UART_PORT_CL, (unsigned char *)&c, 1);
	return (c);
}


//**********************************************************************************
//	inputGigE
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		出力文字数
//==================================================================================
char inputGigE (void)
{
	char c;

	uartRecv (UART_PORT_GIGE, (unsigned char *)&c, 1);
	return (c);
}


//**********************************************************************************
//	inputPollCl
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData			：受信データを格納するポインタ
//	[ OUTPUT ]
//		コンソールからの受信データ数(0はデータなし)
//==================================================================================
int inputPollCl (char *pData)
{
	// Check pData Parameter
	if (pData == NULL)
		return (0);

	// 受信カウント数取得
	if (uartRecvCount (UART_PORT_CL) <= 0)
		return (0);

	uartRecv (UART_PORT_CL, (unsigned char *)pData, 1);

	return (1);
}


//**********************************************************************************
//	inputPollGigE
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData			：受信データを格納するポインタ
//	[ OUTPUT ]
//		コンソールからの受信データ数(0はデータなし)
//==================================================================================
int inputPollGigE (char *pData)
{
	// Check pData Parameter
	if (pData == NULL)
		return (0);

	// 受信カウント数取得
	if (uartRecvCount (UART_PORT_GIGE) <= 0)
		return (0);

	uartRecv (UART_PORT_GIGE, (unsigned char *)pData, 1);

	return (1);
}


//**********************************************************************************
//	inputPollDouble
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData			：受信データを格納するポインタ
//	[ OUTPUT ]
//		コンソールからの受信データ数(0はデータなし)
//==================================================================================
int inputPollDouble (char *pData)
{
	// CameraLinkのみ
	if (gInterFaceID == INTERFACE_CAMERALINK)
	{
		if (inputPollCl (pData) != 0)
			return (1);
	}

	if (inputPollGigE (pData) != 0)
		return (1);

	return (0);
}


//**********************************************************************************
//	_kbhit
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData			：受信データを格納するポインタ
//	[ OUTPUT ]
//		コンソールからの受信データ数(0はデータなし)
//==================================================================================
int _kbhit (char *pData)
{
	return (DEBUG_INPUT_POLL (pData));
}


//**********************************************************************************
//	outputCl
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data			：送信データ
//	[ OUTPUT ]
//		-
//==================================================================================
void outputCl (char data)
{
	uartSend (UART_PORT_CL, (unsigned char *)&data, 1);
}


//**********************************************************************************
//	outputGigE
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data			：送信データ
//	[ OUTPUT ]
//		-
//==================================================================================
void outputGigE (char data)
{
	uartSend (UART_PORT_GIGE, (unsigned char *)&data, 1);
}


//**********************************************************************************
//	outByte
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data			：送信データ
//	[ OUTPUT ]
//		-
//==================================================================================
void outByte (char data)
{
	//@@@1
}

// eof
