//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// uart.h - UART Header
//**********************************************************************************


#ifndef _UART_H_
#define _UART_H_

//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------

// 通信パラメータ 
#define UART_RECV_BUFF_SIZE			(1024)		// 受信バッファサイズ
#define UART_SEND_BUFF_SIZE			(1024)		// 送信バッファサイズ

// デバイス数
#define UART_DEVICE_NUM				(1)

// 使用ポート番号
#define UART_PORT0					(0)
#define UART_PORT1					(1)

#define UART_PORT_CL				UART_PORT0
#define UART_PORT_GIGE				UART_PORT0

// 使用ポートMIN/MAX
#define UART_PORT_MIN				UART_PORT0
#define UART_PORT_MAX				UART_PORT0

// UARTボーレート
#define UART_BAUD_115200			(115200)
#define UART_BAUD_9600				(9600)

// UARTボーレートMIN/MAX/Default
#define UART_BAUD_DEFAULT			UART_BAUD_115200	// default
#define UART_BAUD_MAX				UART_BAUD_115200	// max
#define UART_BAUD_MIN				UART_BAUD_9600		// min

// Timeout
#define UART_RECV_TIMEOUT			(5000)
#define UART_SEND_TIMEOUT			(5000)


//----------------------------------------------------------------------------------
// typedef
//----------------------------------------------------------------------------------

// UARTドライバ情報
typedef struct uartDrvInfo {

	// デバイスベースアドレス
	unsigned int baseAdrs;

	// 割り込み
	unsigned int irq;


	// 受信バッファ
	unsigned char *recvBuff;

	// 送信バッファ
	unsigned char *sendBuff;

	// バッファサイズ
	unsigned int recvBuffSize;				// 受信バッファサイズ
	unsigned int sendBuffSize;				// 送信バッファサイズ

	// 受信用
	unsigned int recvPushCount;				// pushカウント
	unsigned int *pRecvPushCount;			// pushカウント(OS管理外)

	unsigned int recvPopCount;				// popカウント
	unsigned int *pRecvPopCount;			// popカウント(OS管理外)

	unsigned int recvBuffCount;				// 受信バッファ内のデータ数
	unsigned int *pRecvBuffCount;			// 受信バッファ内のデータ数(OS管理外)

	// 送信用
	unsigned int sendPushCount;				// pushカウント
	unsigned int sendPopCount;				// popカウント
	unsigned int sendBuffCount;				// 送信バッファ内のデータ数
	unsigned int *pSendBuffCount;			// 送信バッファ内のデータ数(OS管理外)

	// エラー系
	unsigned int recvOverRun;				// 受信バッファオーバランカウンタ
	unsigned int recvError;					// 受信エラーカウンタ

	// 通信パラメータ
	int baudrate;							// ボーレート
	int length;								// データ長
	int stop;								// ストップビット
	int parity;								// パリティ
	int flow;								// フロー制御
	
	// ポート番号
	int port;

} UART_DRV_INFO;

#endif	// _UART_H_

// eof
