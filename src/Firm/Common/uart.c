//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// uart.c - UART Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "common.h"
#include "miv_rv32_hal.h"
#include "CoreUARTapb/core_uart_apb.h"
#include "CoreUARTapb/coreuartapb_regs.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
// UARTドライバ情報
struct uartDrvInfo uartDrv[UART_DEVICE_NUM];

// ボーレートテーブル
int gBaudrateTable[] = {9600, 19200, 38400, 57600, 115200};

// UART Controller Status
int gUartStatus = 0;


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern UART_instance_t g_uart;
extern miv_plic_instance_t g_plic;


//**********************************************************************************
//	初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int uartInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int port;
	unsigned int ModeRegister;
	unsigned int IntrMask;
	unsigned int data;
	unsigned int RecvTimeout, RtoRegister;
	unsigned int baudrate;
	int i, num;
	unsigned int dipsw;
	char c;

	// Initialize
    UART_init (&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));
	
    // Dummy Read
    //UART_get_rx (&g_uart, &c, 1);

	// デバイス情報取得
	for (port=0; port<UART_DEVICE_NUM; port++)
	{
		// clear
		memset (&uartDrv[port], 0, sizeof(struct uartDrvInfo));

		//------------------------------------------------------------
		// 情報設定
		//------------------------------------------------------------

		// デバイス情報設定
		uartDrv[port].baseAdrs = COREUARTAPB0_BASE_ADDR;
		uartDrv[port].irq      = 0;

		// ポート番号設定
		uartDrv[port].port = port;
		
		// 排他Flag初期化
		OUT32 ((FIRM_DATA_UART_SEND0_FLAG_ADRS + port * 8), 0);
		OUT32 ((FIRM_DATA_UART_RECV0_FLAG_ADRS + port * 8), 0);

		// 受信バッファ初期化
		uartDrv[port].recvBuff = (unsigned char *)UART0_RECV_BUFF_ADRS + port * UART_RECV_BUFF_SIZE ;

		// 送信バッファ初期化
		uartDrv[port].sendBuff = (unsigned char *)UART0_SEND_BUFF_ADRS + port * UART_RECV_BUFF_SIZE ;

		// 送受信バッファカウント設定のポインタ
		uartDrv[port].pRecvBuffCount = (unsigned int *)(FIRM_DATA_UART_RECV0_COUNT_ADRS + (port * 4));
		uartDrv[port].pSendBuffCount = (unsigned int *)(FIRM_DATA_UART_SEND0_COUNT_ADRS + (port * 4));

		// 受信バッファPop/Pushカウント設定のポインタ
		uartDrv[port].pRecvPopCount = (unsigned int *)(FIRM_DATA_UART_RECV0_POP_COUNT_ADRS + (port * 4));
		uartDrv[port].pRecvPushCount = (unsigned int *)(FIRM_DATA_UART_RECV0_PUSH_COUNT_ADRS + (port * 4));

		// 送受信バッファカウント初期化
		*uartDrv[port].pRecvBuffCount = 0;
		*uartDrv[port].pSendBuffCount = 0;

		// 受信バッファPop/pushカウント初期化
		*uartDrv[port].pRecvPopCount = 0;
		*uartDrv[port].pRecvPushCount = 0;

		// 送受信バッファサイズ設定
		uartDrv[port].recvBuffSize = UART_RECV_BUFF_SIZE;
		uartDrv[port].sendBuffSize = UART_SEND_BUFF_SIZE;

		// 通信パラメータ設定(ボーレートは別の箇所で設定)
		uartDrv[port].length = 8;	// データ長
		uartDrv[port].stop   = 1;	// ストップビット
		uartDrv[port].parity = 0;	// パリティ
		uartDrv[port].flow   = 0;	// フロー制御
	}

	// Interrupt Enable
    MIV_PLIC_enable_irq (&g_plic, MIV_PLIC_EXT1_IRQn);

_DONE:
	return (status);
}


//**********************************************************************************
//	UART割り込み有効
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port					：ポート番号(0 or 1)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int uartSetIntEnable (int port)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Port Parameters
	if ((port < UART_PORT_MIN) || (port > UART_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UART Enable Interrupt Port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, UART_PORT_MIN, UART_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Interrupt Enable
    MIV_PLIC_enable_irq (&g_plic, MIV_PLIC_EXT1_IRQn);

_DONE:
	return (status);
}


//**********************************************************************************
//	UART割り込み無効
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port					：ポート番号(0 or 1)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int uartSetIntDisable (int port)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Port Parameters
	if ((port < UART_PORT_MIN) || (port > UART_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UART Disable Interrupt Port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, UART_PORT_MIN, UART_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Interrupt Disable
    MIV_PLIC_disable_irq (&g_plic, MIV_PLIC_EXT1_IRQn);

_DONE:
	return (status);
}


//**********************************************************************************
//	UART受信ハンドラ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pDrvInfo				：ドライバ情報を格納するポインタ
//	[ OUTPUT ]
//		受信データ数
//==================================================================================
int MIV_PLIC_EXT1_IRQHandler (void)
{
	unsigned int ReceivedCount = 0;
	unsigned int rdata;
	unsigned int status;
	int recvNum;
	unsigned int adrs;
	UART_DRV_INFO *pDrvInfo = (UART_DRV_INFO *)&uartDrv[0];

	// Get Address
	adrs = pDrvInfo->baseAdrs;
	
	// Read Status Register
	status = IN32 ((adrs + STATUS_REG_OFFSET));
	
	// 受信データあり?
	while ((status & STATUS_RXFULL_MASK) == STATUS_RXFULL_MASK)
	{
		//-----------------------------------------------------
		// 受信処理
		//-----------------------------------------------------

		// Read Data
		rdata = IN32 ((adrs + RXDATA_REG_OFFSET));
		rdata &= 0xff;

		//  受信バッファがオーバーフロー?
		recvNum = *pDrvInfo->pRecvBuffCount;
		
		if (recvNum >= pDrvInfo->recvBuffSize)
		{
			// エラーカウント加算
			pDrvInfo->recvOverRun++;
			break;
		}
		else
		{
			// データ受信
			pDrvInfo->recvBuff[(*pDrvInfo->pRecvPushCount)] = (unsigned char)rdata;

			// 受信カウンタインクリメント
			(*pDrvInfo->pRecvBuffCount)++;
			ReceivedCount++;

			// 受信データ格納ポインタインクリメント
			(*pDrvInfo->pRecvPushCount)++;

			// 受信バッファの終端?
			if (*pDrvInfo->pRecvPushCount >= pDrvInfo->recvBuffSize)
				*pDrvInfo->pRecvPushCount  = 0;			// 受信バッファのポインタを先頭に!
		}

		// Read Status Register
		status = IN32 ((adrs + STATUS_REG_OFFSET));
	}

	return (EXT_IRQ_KEEP_ENABLED);
}


//**********************************************************************************
//	UART受信
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port					：ポート番号(0=COM0/1=COM1)
//		pBuffer					：受信データを格納するポインタ
//		size					：受信データサイズ
//	[ OUTPUT ]
//		負数						：異常終了
//		上記以外					：受信サイズ
//==================================================================================
int uartRecv (int port, unsigned char *pBuffer, int size)
{
	int status;
	int count;
	unsigned char *ptr;
	int read_num = 0;
	int recvNum;
	int readCount;

	// Check Port Parameter
	if ((port < UART_PORT_MIN) || (port > UART_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UART Recv Buff Port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, UART_PORT_MIN, UART_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "UART Recv Buff pBuffer NULL Parameter Error\n");
		return (status);
	}

	// パラメータコピー
	ptr = pBuffer;
	count = size;

	while (count > 0)
	{
		// 受信データあり?
		recvNum = *uartDrv[port].pRecvBuffCount;

		if (recvNum > 0)
		{
			// 受信データをユーザーバッファへ
			*ptr++ = uartDrv[port].recvBuff[*uartDrv[port].pRecvPopCount];

			// 受信データ引き出しポインタ加算
			(*uartDrv[port].pRecvPopCount)++;

			// 割り込み禁止
			uartSetIntDisable (port);

			// 受信バッファカウント減算
			(*uartDrv[port].pRecvBuffCount)--;

			// 割り込み有効
			uartSetIntEnable (port);

			// ユーザー受信カウント加算
			read_num++;

			// 受信バッファの終端?
			if (*uartDrv[port].pRecvPopCount >= uartDrv[port].recvBuffSize)
				(*uartDrv[port].pRecvPopCount) = 0;			// 受信バッファのポインタを先頭に!

			count--;
		}
		//else
			//break;
	}

	return (read_num);
}


//**********************************************************************************
//	UART送信
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port					：ポート番号(0=COM0/1=COM1)
//		pBuffer					：送信データを格納するポインタ
//		size					：送信データサイズ
//	[ OUTPUT ]
//		負数						：異常終了
//		上記以外					：送信サイズ
//==================================================================================
int uartSend (int port, unsigned char *pBuffer, int size)
{
	int status;
	int count;
	unsigned char *ptr;
	int sent_num = 0;

	// Check Port Parameter
	if ((port < UART_PORT_MIN) || (port > UART_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UART Send Buff Port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, UART_PORT_MIN, UART_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "UART Send Buff pBuffer NULL Parameter Error\n");
		return (status);
	}

	// パラメータコピー
	ptr = pBuffer;
	count = size;

	while (count > sent_num)
	{
		UART_send (&g_uart, ptr, 1);
		ptr++;
		
		// ユーザーデータを送信バッファに格納した数
		sent_num++;
	}

	return (sent_num);
}


//**********************************************************************************
//	UART受信カウント数取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port					：ポート番号(0=COM0/1=COM1)
//	[ OUTPUT ]
//		負数					：異常終了
//		上記以外					：受信サイズ
//==================================================================================
int uartRecvCount (int port)
{
	int status;
	int count;
	
	// Check Port Parameter
	if ((port < UART_PORT_MIN) || (port > UART_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UART Recv Count Port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, UART_PORT_MIN, UART_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		return (status);
	}

	count = *uartDrv[port].pRecvBuffCount;
	
	return (count);
}


//**********************************************************************************
//	UART受信バッファクリア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port					：ポート番号(0=COM0/1=COM1)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int uartRecvClear (int port)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Port Parameter
	if ((port < UART_PORT_MIN) || (port > UART_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UART Recv Clear Port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, UART_PORT_MIN, UART_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Clear
	*uartDrv[port].pRecvPushCount = 0;	// pushカウント
	*uartDrv[port].pRecvPopCount = 0;	// popカウント
	*uartDrv[port].pRecvBuffCount = 0;	// 受信バッファ内のデータ数

_DONE:
	return (status);
}


//**********************************************************************************
//	UART送信バッファクリア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port					：ポート番号(0=COM0/1=COM1)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int uartSendClear (int port)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Port Parameter
	if ((port < UART_PORT_MIN) || (port > UART_PORT_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_UART, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "UART Send Clear Port(%d) Parameter Error.(Min:%d / Max:%d)\n", port, UART_PORT_MIN, UART_PORT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Clear
	uartDrv[port].sendPushCount = 0;	// pushカウント
	uartDrv[port].sendPopCount = 0;		// popカウント
	*uartDrv[port].pSendBuffCount = 0;	// 送信バッファ内のデータ数

_DONE:
	return (status);
}

// eof
