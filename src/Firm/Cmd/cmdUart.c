//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdUart.c - UART Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define UART_TEST_BUFF_SIZE	(256)


//**********************************************************************************
//	UART Test
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagUart (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *sendBuff = NULL;
	unsigned char *recvBuff = NULL;
	unsigned int tloop;
	int sentCount, recvCount;
	unsigned int i;
	unsigned char *sPtrL;
	unsigned char *rPtrL;
	unsigned char dataL;
	int port = 0;
	unsigned int loop;

	// wait 5sec
	usDelay (5000000);

	// ループ回数取得
	loop = atoi ((char *)str);

	// メモリ確保
	if ((sendBuff = (unsigned char *)malloc (UART_TEST_BUFF_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "UART Memory Request Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリ確保
	if ((recvBuff = (unsigned char *)malloc (UART_TEST_BUFF_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "UART Memory Request Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		//DEBUG_PRINT("--------------------UART Loop = %d--------------------\n", tloop);

		//----------------------------------------
		// データ作成
		//----------------------------------------
		sPtrL = (unsigned char *)sendBuff;
		dataL = 0;
		for (i=0; i<UART_TEST_BUFF_SIZE; i++, sPtrL++, dataL++)
			*sPtrL = dataL;

		// 送信
		if ((sentCount = uartSend (port, sendBuff, UART_TEST_BUFF_SIZE)) != UART_TEST_BUFF_SIZE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_NO_DATA);
			sprintf (gLogMsgBuff, "UART Send Error\n");
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// 受信
		if ((recvCount = uartRecv (port, recvBuff, UART_TEST_BUFF_SIZE)) != UART_TEST_BUFF_SIZE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_NO_DATA);
			sprintf (gLogMsgBuff, "UART Recv Error\n");
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// コンペア
		sPtrL = (unsigned char *)sendBuff;
		rPtrL = (unsigned char *)recvBuff;
		for (i=0; i<UART_TEST_BUFF_SIZE; i++, sPtrL++, rPtrL++)
		{
			if (*sPtrL != *rPtrL)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_COMPARE);
				sprintf (gLogMsgBuff, "UART Compare Error.\nAdrs=0x%x\nSend=0x%x\nRecv=0x%x\n", i, *sPtrL, *rPtrL);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}
		}
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	if (sendBuff != NULL)
		free (sendBuff);

	if (recvBuff != NULL)
		free (recvBuff);

	return (status);
}

#if 0	//@@@1
//**********************************************************************************
//	BaudRate設定／取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdBaudRate (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int baudrate, getBaudrate;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdBaudRateHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 一時メモリから取得
		if ((status = cameraParamCommonReadMem (CAMERA_SAVE_COMMON_BAUDRATE, &baudrate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// ボーレートレート取得
		DEBUG_PRINT_FORCE ("%d ", baudrate);
	}
	else if (argc == 2)
	{
		// ボーレートレート取得
		if (sscanf (gCmdArg[1], "%d", &baudrate) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Baudrate Parameter
		if ((status = uartCheckBaudRate (baudrate, &getBaudrate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// パラメータが大丈夫あればOKを返す
		DEBUG_PRINT_FORCE (CMD_OK);

		// Delay
		msDelay (100);

		// ボーレートレート設定
		if ((status = uartSetBaudRate (UART_PORT_CL, baudrate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// ボーレートレート設定(GigE側)※エラーCheckはなし
		if ((status = uartSetBaudRate (UART_PORT_GIGE, baudrate)) != AVAL_STATUS_SUCCESS)
			return (status);

		// 一時メモリに保存
		cameraParamCommonWriteMem (CAMERA_SAVE_COMMON_BAUDRATE, baudrate);

		// Flashに書き込み
		cameraParamCommonWriteRom (CAMERA_SAVE_COMMON_BAUDRATE, baudrate);

		// Delay
		usDelay (500);

		// 受信バッファクリア
		uartRecvClear (UART_PORT_CL);

		// 最後に応答は返さない
		status = AVAL_STATUS_NO_ANSWER;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	BaudRate設定／取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdBaudRateHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The UART Baudrate is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : baudrate\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : UART Baudrate[bps]\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The UART Baudrate is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : baudrate\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : UART Baudrate[bps]\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}
#endif //@@@1

// eof
