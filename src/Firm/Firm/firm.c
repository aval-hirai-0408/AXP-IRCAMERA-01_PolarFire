//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// firm.c - Firmware Main Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
char gConsoleKey[CONSOLE_BUFF_COUNT];
int gConsoleIndex = 0;
int gConsolePassword = 0;

//@@@@@@@@@@
UART_instance_t g_uart;
//@@@@@@@@@@

//**********************************************************************************
//	ファームウェアMain
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int main (void)
{
//@@@@@@@@@@@@@@@@@@@@
    gInterFaceID = INTERFACE_CXP;
//@@@@@@@@@@@@@@@@@@@@

	// 起動状態
	//@@@1ledBootState ();

	// HW Initialize
	hwInitialize (0);
	
	// Connect状態
	//@@@1ledConnectState ();
	
	// CameraLinkのみ
	if (gInterFaceID == INTERFACE_CAMERALINK)
	{
		// CameraLinkメイン処理
		clMain ();
	}
	// CXPのみ
	else if (gInterFaceID == INTERFACE_CXP)
	{
		// CXPメイン処理
		cxpMain ();
	}
	else if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		// GigEメイン処理
		gigeMain ();
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ファームウェアエラー通知(ホストに対し)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		errNo				：エラー番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmErrorAttention (int errNo)
{
	// システムエラーステート
	ledHwSystemErrorState ();
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ファームウェアCOM Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmCheckConsole (void)
{
	char c;

	if (_kbhit (&c))
	{
		// 小文字変換して保存
		gConsoleKey[gConsoleIndex] = (char)tolower ((int)c);
		gConsoleIndex++;

		if ((c == CODE_CR) && (gConsoleIndex != CONSOLE_BUFF_COUNT))
			gConsoleIndex = 0;

		if (gConsoleIndex >= CONSOLE_BUFF_COUNT)
		{
			// Check Password
			if (strncmp (gConsoleKey, PASSWORD_KEY, CONSOLE_BUFF_COUNT) == 0)
				gConsolePassword = 1;

			gConsoleIndex = 0;
		}
	}

	return (AVAL_STATUS_SUCCESS);
}

//@@@1
//**********************************************************************************
//	Firm Cmd Status設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmSetCmdStatus (int mode)
{
	OUT32 (FIRM_DATA_FIRM_CMD_STATUS_ADRS, mode);
	return (AVAL_STATUS_SUCCESS);
}
//@@@1

//@@@1
//**********************************************************************************
//	Firm Cmd Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：モードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmGetCmdStatus (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

	// データ取得
	*pMode = IN32 (FIRM_DATA_FIRM_CMD_STATUS_ADRS);

_DONE:
	return (status);
}
//@@@1

// eof
