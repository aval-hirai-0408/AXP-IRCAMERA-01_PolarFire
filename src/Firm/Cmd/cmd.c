//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmd.c - Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "cmdTable.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------

#define MODE_CMD_HISTORY

// Escape Key
#define ESC_KEY					(0x1b)

// 上矢印
#define MODE_UP_KEY1			ESC_KEY
#define MODE_UP_KEY2			(0x5b)
#define MODE_UP_KEY3			(0x41)

// 下矢印
#define MODE_DOWN_KEY1			ESC_KEY
#define MODE_DOWN_KEY2			(0x5b)
#define MODE_DOWN_KEY3			(0x42)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// コマンドバッファ
char gCmdBuff[CONSOLE_BUFF_SIZE];

// コマンド引数
char *gCmdArg[CMD_ARG_NUM_MAX];

// コマンド個数
int gCmdNum = 0;

// コマンドテーブルソート用
CMD_STRING_TBL *pCmdTbl = NULL;

// コマンド履歴保存用バッファ
char gCmdSave[CMD_SAVE_NUM][CONSOLE_BUFF_SIZE];

// コマンド履歴保存位置
unsigned int gCmdCurrentIndex = 0;

// コマンド履歴保存個数
unsigned int gCmdSaveIndex = 0;

// Tab Index
unsigned int gTabIndex = 0;

// Tab String Count
unsigned int gTabStringCount= 0;

// Log Msg Buffer
char gLogMsgBuff[LOG_MSG_BUFF_SIZE];


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int gCxpCmdInterruptFlag;
extern int gConsolePassword;


//**********************************************************************************
//	cmd
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmd (void)
{
	int status;

	// コマンド個数
	gCmdNum = sizeof (mainCmdTbl) / sizeof (mainCmdTbl[0]);

	// コマンドソート用の処理
	cmdSortInitialize ();

	// コマンド履歴Buffer Clear
	memset (gCmdSave, 0x00, (CMD_SAVE_NUM * CONSOLE_BUFF_SIZE));

	// コマンド処理
	status = cmdMain (mainCmdTbl, CMD_MODE_STRING);

	return (status);
}


//**********************************************************************************
//	cmdMain
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		cmdTbl					：コマンド情報を格納するポインタ
//		mode					：0=コマンド番号選択モード/1=コマンド文字列検索モード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdMain (CMD_TBL *cmdTbl, int mode)
{
	int status;
	int cmdNo, testLoop;
	char buff[CONSOLE_BUFF_SIZE];
#ifdef MODE_CMD_HISTORY
	int index;
#endif

	//if (mode == CMD_MODE_STRING)
	//{
		//DEBUG_PRINT_FORCE ("\n");
		//DEBUG_PRINT_FORCE (CMD_OK);
	//}

#ifdef MODE_CMD_HISTORY
	// 初期化
	for (index=0; index<CMD_SAVE_NUM; index++)
		gCmdSave[index][0] = '\0';
#endif

	while (1)
	{
		if (mode == CMD_MODE_NUMBER)
			cmdDispMenu (cmdTbl);

		cmdNo = cmdNoGet (cmdTbl, mode);

		if (cmdNo == -1)		// invalid cmdNo
		{
			DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_CMD);
			DEBUG_PRINT_FORCE (CMD_ERROR);
			continue;
		}
		else if (cmdNo == -2)	// Return Key Only
		{
			DEBUG_PRINT_FORCE (CMD_OK);
			continue;
		}
		else if (cmdNo == -3)	// Exit
		{
			// CameraLinkのみ
			if (gInterFaceID == INTERFACE_CAMERALINK)
			{
				if (cmdTbl == mainCmdTbl)
				{
					DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_CMD);
					DEBUG_PRINT_FORCE (CMD_ERROR);
					continue;
				}
			}

			DEBUG_PRINT_FORCE ("Exit\n");
			return (AVAL_STATUS_SUCCESS);
		}

		//------------------------------
		// コマンド処理
		//------------------------------
		if (mode == CMD_MODE_STRING)
		{
			// コマンド受付実行状態
			ledCmdExeState ();

			if ((status = cmdTbl[cmdNo].func((void *)gCmdBuff)) == AVAL_STATUS_SUCCESS)
			{
				DEBUG_PRINT_FORCE (CMD_OK);
			}
			else
			{
				if (status != AVAL_STATUS_NO_ANSWER)
				{
					DEBUG_PRINT_FORCE (CMD_ERROR);
				}
			}
		}
		else if (mode == CMD_MODE_NUMBER)
		{
			// have LOOP count cmd
			if (cmdTbl[cmdNo].optArg == OPT_LOOP)
			{
				testLoop = cmdLoopCount (buff);
				if (testLoop == 0)
					continue;

				cmdTbl[cmdNo].func((void *)buff);
			}
			// no params cmd
			else
			{
				cmdTbl[cmdNo].func(NULL);
			}
		}
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	cmdNoGet
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		cmdTbl					：コマンド情報を格納するポインタ
//		mode					：0=コマンド番号選択モード/1=コマンド文字列検索モード
//	[ OUTPUT ]
//  RETURNS						：-1=invalid cmdNo
//  							：-2=リターンキーのみ
//  							：-3=終了
//								：others=cmdTbl No.
//==================================================================================
int cmdNoGet (CMD_TBL *cmdTbl, int mode)
{
	int tblNo, maxNum;
	char *ptr;
	int len, cmdLen, cmdTableLen;

	if (mode == CMD_MODE_STRING)
	{
		// Get Command Line
		cmdCharGet (gCmdBuff);

		// Return Key Only
		if (gCmdBuff[0] == '\0')
			return (-2);

		// Q Key Only
		if ((gCmdBuff[0] == 'q') && (gCmdBuff[1] == '\0'))
		{
			gConsolePassword = 0;
			return (-2);
			//return (-3);
		}

		// Check Command
		len = strlen (gCmdBuff);
		ptr = gCmdBuff;
		for (cmdLen=0; cmdLen<len; cmdLen++, ptr++)
		{
			// スペース検索
			if (*ptr == 0x20)
				break;

			// 小文字変換
			*ptr = (char)tolower ((int)*ptr);
		}

		for (tblNo=0; ;tblNo++)
		{
			// コマンドテーブル終端
			if (cmdTbl[tblNo].name == NULL)
				return (-1);        // invalid cmdNo

			// コマンド確認
			if (strncmp(gCmdBuff, cmdTbl[tblNo].name, cmdLen) == 0)
			{
				// Command Length
				cmdTableLen = strlen (cmdTbl[tblNo].name);

				// コマンド長は同じ?
				if (cmdTableLen == cmdLen)
					break;
			}
		}
	}
	else
	{
		maxNum = sizeof (cmdTbl) / sizeof (cmdTbl[0]);
		maxNum -= 1;

		DEBUG_PRINT_FORCE ("\nNumber > ");
		fflush (stdout);

		// Get Command Line
		cmdCharGet (gCmdBuff);

		// Return Key Only
		if (gCmdBuff[0] == '\0')
			return (-2);

		if (gCmdBuff[0] == 'q')
			return (-3);            // exit

		if (sscanf (gCmdBuff, "%d", &tblNo) != 1)
		{
			DEBUG_PRINT_FORCE ("Invalid Number\n");
			return (-2);
		}

		// Check Number
		if ((tblNo < 0) && (tblNo > maxNum))
			return (-1);
	}

	return (tblNo);
}


//**********************************************************************************
//	cmdLoopCount - get Test Loop Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//     Loop Count
//==================================================================================
int cmdLoopCount (char *pBuffer)
{
	int testLoop;

	DEBUG_PRINT_FORCE ("loop count : ");
	fflush (stdout);

	cmdCharGet (pBuffer);
	sscanf (pBuffer,"%d",&testLoop);

	return (testLoop);
}


//**********************************************************************************
//	cmdCharGet
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		buff			：データを格納するバッファ
//	[ OUTPUT ]
//		0				：正常終了
//==================================================================================
void cmdCharGet (char *buff)
{
	int i;
	char c;
	int historyIndex = 0;		// 上矢印／下矢印を入力した回数。どのコマンドを参照するのかを判断。
	int ignoreCount = 0;
	int port = 0;
	unsigned int fiftCount;
	unsigned int linkStatus;
	uintptr_t old_mstatus;
	
	for (i=0; i<(CONSOLE_BUFF_SIZE-1); )
	{
		while (1)
		{
			//------------------------------------------------------------
			// Process
			//------------------------------------------------------------
			if (gInterFaceID == INTERFACE_CAMERALINK)
			{
			}
			else if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
			{
			}
			else if (gInterFaceID == INTERFACE_CXP)
			{
				#if  0//@@@1
				//------------------------------------------------------------
				// Link Status制御
				//------------------------------------------------------------
				cxpLedLinkState (&linkStatus);

				//------------------------------------------------------------
				// LED制御
				//------------------------------------------------------------
				cxpLedControl (linkStatus);
				#endif //@@@1

				//------------------------------------------------------------
				// CXPコマンド処理
				//------------------------------------------------------------

				// FIFOデータ数確認
				if (cxpGetFifoSizeCount (port, &fiftCount) == AVAL_STATUS_SUCCESS)
				{
					// FIFOにデータあり？
					if (fiftCount != 0)
					{
						cxpProcs (port);
					}
				}
			}

			//------------------------------------------------------------
			// LED制御
			//------------------------------------------------------------
			//ledControl ();

			//------------------------------------------------------------
			// シリアルデータ取得
			//------------------------------------------------------------
			if (gConsolePassword == 0)
			{
				firmCheckConsole ();
				
				if (gConsolePassword == 1)
				{
					DEBUG_PRINT_FORCE ("\n");
					DEBUG_PRINT_FORCE (CMD_OK);
				}
			}
			else
			{
				if (DEBUG_INPUT_POLL(&c))
					break;
			}

			#if 0
			#if 0
			//------------------------------------------------------------
			// Sleep
			//------------------------------------------------------------
			if (cxpGetReadFifoStatus (port, &fiftCount) != AVAL_STATUS_SUCCESS)
				fiftCount = 1;
			
			//fiftCount = IN32 (FPGA_CXP_LSUC_RX_SW_PKT_STATUS_ADRS) & FPGA_CXP_LSUC_RX_SW_PKT_VAL_BIT;

			if (fiftCount == 0)
			{
				wfi ();   // sleep
			}
			#else
			
			//------------------------------------------------------------
			// Sleep
			//------------------------------------------------------------
			
			// Disable IRQ
			disable_global_irq ();

			// Get Fifo Status
			if (cxpGetReadFifoStatus (port, &fiftCount) != AVAL_STATUS_SUCCESS)
				fiftCount = 1;
			
			if (fiftCount == 0)
			{
				// Sleep
				cpu_wfi();
			}
			
			// Enable Enable
			enable_global_irq ();

			#endif
			#endif

			//------------------------------------------------------------
			// Sleep
			//------------------------------------------------------------
			
			// Disable IRQ
			old_mstatus = irq_save_disable_Mie ();

			// Get Fifo Status
			if (cxpGetReadFifoStatus (port, &fiftCount) != AVAL_STATUS_SUCCESS)
				fiftCount = 1;
			
			if (fiftCount == 0)
			{
				// Sleep
				cpu_wfi();
			}
			
			// Enable Enable
			irq_restore_Mie (old_mstatus);
		}

		//------------------------------------------------------------
		// 文字入力処理
		//------------------------------------------------------------
		switch (c)
		{
			case '\t':
				cmdTabSearch (buff, &i);
				break;

				// Tab Stringカウントクリア(Tab Key以外はクリア)
				gTabStringCount = 0;

			case '\b':
				if (i <=  0)
					continue;

				--i;
				DEBUG_PRINT_FORCE ("\b");
				DEBUG_PRINT_FORCE (" ");
				DEBUG_PRINT_FORCE ("\b");
				fflush(stdout);

				//------------------------------------------------------------
				// 文字入力があった為、以下クリア処理実行
				//------------------------------------------------------------
			
				// 上矢印／下矢印を入力した回数クリア
				historyIndex = 0;
			    break;

			case CODE_LF:
			    break;

			case CODE_CR:
				DEBUG_PRINT_FORCE ("%c", CODE_CR);
			    break;

			default:
#ifdef MODE_CMD_HISTORY
				// ESCキー入力?
				if (c == ESC_KEY)
					ignoreCount = 2;		// その後２文字無視
#endif
				// 文字保存
				buff[i++] = c;

				// 文字入力があった為、以下クリア処理実行
				if ((c != EOF) && (ignoreCount == 0))
					historyIndex = 0;					// 上矢印／下矢印を入力した回数クリア

				// 文字表示しない?
				if (ignoreCount == 0)
				{
					DEBUG_PRINT_FORCE ("%c", c);
					fflush(stdout);
				}
				else
				{
					ignoreCount--;
				}

				break;
		}

		if (c == '\r')
		{
			// 終端設定
			buff[i++] = '\0';

#ifdef MODE_CMD_HISTORY
			// コマンド履歴に保存
			if (buff[0] != '\0')
			{
				strcpy (gCmdSave[(gCmdSaveIndex%CMD_SAVE_NUM)], buff);
				gCmdSaveIndex++;
			}
#endif

			// コマンドTabカウンタクリア
			gTabIndex = 0;

			break;
		}

#ifdef MODE_CMD_HISTORY
		// 上矢印キー?
		if (c == MODE_UP_KEY2)
		{
			if (i >= 2)
			{
				if (DEBUG_INPUT_POLL(&c))
				{
  				    buff[i++] = c;

					// 上矢印キーに一致?
					if ((buff[(i-3)] == MODE_UP_KEY1) && (buff[(i-2)] == MODE_UP_KEY2) && (buff[(i-1)] == MODE_UP_KEY3))
					{
						cmdHistortKey (0/*up*/, buff, &i, &historyIndex);
					}
					// 下矢印キーに一致?
					else if ((buff[(i-3)] == MODE_DOWN_KEY1) && (buff[(i-2)] == MODE_DOWN_KEY2) && (buff[(i-1)] == MODE_DOWN_KEY3))
					{
						cmdHistortKey (1/*down*/, buff, &i, &historyIndex);
					}
				}
			}
		}
#endif
	}
}


//**********************************************************************************
//	Command Tab Search
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pBuffer				：コマンド格納バッファポインタ
//		pIndex				：文字サイズを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdTabSearch (char *pBuffer, int *pIndex)
{
	int no;
	int index = 0;
	int hitFlag = 0;
	int currentLen;
	int i;
	int candidacy = 0;

	// Check pBuffer Parameter
	if (pBuffer == NULL)
		return (AVAL_STATUS_SUCCESS);

	// Check pIndex Parameter
	if (pIndex == NULL)
		return (AVAL_STATUS_SUCCESS);

	// 最初のTab入力時のコマンド文字数
	if (gTabIndex == 0)
		gTabStringCount = *pIndex;

MORE:
	for (no=0; no<gCmdNum; no++)
	{
		// 入力した文字列まで一致するものを検索
		if (strncmp(pBuffer, pCmdTbl[no].name, gTabStringCount) == 0)
		{
			if (index >= gTabIndex)	// 今回は何個目のコマンドを検索するか？
			{
				gTabIndex++;		// 次回は次の候補を検索できるように++

				if (pCmdTbl[no].optArg == OPT_NO_DISP)
				{
					index++;
					continue;
				}

				hitFlag = 1;		// あたり

				// 現在の文字表示をクリア
				currentLen = strlen (pBuffer);
				for (i=0; i<currentLen; i++)
				{
					DEBUG_PRINT_FORCE ("\b");
					DEBUG_PRINT_FORCE (" ");
					DEBUG_PRINT_FORCE ("\b");
				}

				// コマンドCopy
				strcpy (pBuffer, pCmdTbl[no].name);

				// コマンド表示
				DEBUG_PRINT_FORCE ("%s", pBuffer);

				// コマンド長更新
				*pIndex	= strlen (pBuffer);

				break;
			}

			index++;				// 一致

			if (pCmdTbl[no].optArg == OPT_NONE)
				candidacy++;		// 候補あり
		}
	}

	// 当たりあり?
	if (hitFlag == 0)
	{
		// 候補はあった?
		if (candidacy != 0)
		{
			gTabIndex = 0;
			goto MORE;
		}
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Check History Key
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		upDown					：0=上矢印キー/1=下矢印キー
//		pBuffer					：コマンド文字列を格納するポインタ
//		pInputCount				：文字数を格納するポインタ
//		pCurrentIndex			：上矢印／下矢印を入力した回数。どのコマンドを参照するのかを判断。
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdHistortKey (int upDown, char *pBuffer, int *pInputCount, int *pCurrentIndex)
{
	int index;
	int j;

	*pInputCount = 0;	// 現在まで取り込んだデータはクリア

	// 上矢印
	if (upDown == 0)
	{
		if (gCmdSaveIndex > *pCurrentIndex)			// 保存個数 > 上下矢印を押された回数
			(*pCurrentIndex)++;
	}
	else // 下矢印
	{
		if (*pCurrentIndex > 1)						// 上下矢印を押された回数 > 1
			(*pCurrentIndex)--;
	}

	// コマンド履歴参照位置を計算
	index = (gCmdSaveIndex - *pCurrentIndex) % CMD_SAVE_NUM;

	// コマンドは存在?
	if (gCmdSave[index][0] != (char)'\0')
	{
		// 過去の文字の表示クリア
//		DEBUG_PRINT_FORCE ("\r");

		*pInputCount = strlen (pBuffer);
		for (j=0; j<*pInputCount; j++)
			DEBUG_PRINT_FORCE ("\b");

		for (j=0; j<*pInputCount; j++)
			DEBUG_PRINT_FORCE (" ");
//		DEBUG_PRINT_FORCE ("\r");
		for (j=0; j<*pInputCount; j++)
			DEBUG_PRINT_FORCE ("\b");

		// コマンド表示&Copy
		strcpy (pBuffer, &gCmdSave[index][0]);
		DEBUG_PRINT_FORCE ("%s", pBuffer);
		fflush (stdout);
		*pInputCount = strlen (pBuffer);
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	display Menu
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		cmdTbl					：コマンド情報を格納するポインタ
//	[ OUTPUT ]
//		-
//==================================================================================
void cmdDispMenu (CMD_TBL *cmdTbl)
{
	int no, ix;

	DEBUG_PRINT_FORCE("=====  Menu =====\n");
	no = 0;
	for (ix=0; ;no++)
	{
		if (cmdTbl[no].name == NULL)
			break;

		if (ix == 3)
		{
			DEBUG_PRINT_FORCE ("\n");
			ix = 1;
		}
		else
			ix++;

		DEBUG_PRINT_FORCE ("%3d : %s", no, cmdTbl[no].name);
	}

	DEBUG_PRINT_FORCE ("\n");
}


//**********************************************************************************
//	Check Argument
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		取得した引数の個数
//==================================================================================
int cmdCheckArg (char *str)
{
	unsigned int i;

	// Check str Parameter
	if (str == NULL)
	{
		DEBUG_PRINT ("str Parameter Error\n");
		return (0);
	}

	gCmdArg[0] = strtok (str, " ");
//	DEBUG_PRINT_FORCE ("[Cmd] %s\n", i, gCmdArg[0]);

	for (i=1; i<CMD_ARG_NUM_MAX; i++)
	{
		gCmdArg[i] = strtok (NULL, " ");

		if (gCmdArg[i] == NULL)
			break;

//		DEBUG_PRINT_FORCE("[%d]%s\n", i, gCmdArg[i]);
	}

	return (i);
}


//**********************************************************************************
//	Command Sort
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdSortInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	char *pCmdBuffBase = NULL;
	int cmdTableSize;
	int i;

	// Command Table Size
	cmdTableSize = sizeof (CMD_STRING_TBL);

	// Memory Request
	if (pCmdTbl != NULL)
		goto _DONE;

	if ((pCmdBuffBase = (char *)malloc ((gCmdNum * cmdTableSize))) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,  __func__, __LINE__, status, "Help Buffer Request Error\n");
		goto _DONE;
	}

	// Command Copy
	pCmdTbl = (CMD_STRING_TBL *)pCmdBuffBase;
	for (i=0; i<(gCmdNum - 1); i++)
	{
		strcpy ((char *)pCmdTbl[i].name, (char *)mainCmdTbl[i].name);
		pCmdTbl[i].optArg = mainCmdTbl[i].optArg;
	}

	// 終端NULL
	pCmdTbl[i].name[0] = 0;
    pCmdTbl[i].optArg = 0;

	// Sort
	qsort((void *)pCmdTbl, gCmdNum, sizeof (CMD_STRING_TBL), lfcmdSort);

_DONE:
	return (status);
}


//**********************************************************************************
//	Sort
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		a					：文字列を格納するポインタ
//		b					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lfcmdSort (const void *a, const void *b)
{
  return (strcmp ((char *)a , (char *)b));
}


//**********************************************************************************
// コマンドラインクリア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdClearLine (void)
{
	int i;

	for (i=0; i<CONSOLE_BUFF_SIZE; i++)
		DEBUG_PRINT_FORCE ("\b");

	for (i=0; i<CONSOLE_BUFF_SIZE; i++)
		DEBUG_PRINT_FORCE (" ");

	for (i=0; i<CONSOLE_BUFF_SIZE; i++)
		DEBUG_PRINT_FORCE ("\b");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
// コマンドラインバックスペース
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdBackspaceLine (void)
{
	int i;

	for (i=0; i<CONSOLE_BUFF_SIZE; i++)
		DEBUG_PRINT_FORCE ("\b");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
// 文字列からコマンド名を削除し引数のみを取り出す(Ver.1.2)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：入力文字を格納するポインタ(入力)
//		cmd					：コマンドを格納するポインタ(入力)
//		arg					：引数を格納するポインタ（出力）
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdGetArgment (char *str, char *cmd, char **arg)
{
	int status = AVAL_STATUS_SUCCESS;
	int len;
	int i;
	char *ptr;

	// Check str Parameter
	if (str == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,  __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

	// Check cmd Parameter
	if (cmd == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,  __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

	// コマンド長取得
	len = strlen (cmd);

	// コマンド＋スペース以降の位置を検索
	ptr = (char *)(str + len);
	for (i=len; i<(CONSOLE_BUFF_SIZE-1); i++, ptr++)
	{
		// スペースを削除
		if ((*ptr != 0x20) && (*ptr != 0x0))
			break;
	}

	// 引数取得
	*arg = ptr;

_DONE:
	return (status);
}

//**********************************************************************************
// コマンド履歴クリア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdHistoryLocalClear (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Indexクリア
	gCmdSaveIndex = 0;

	// Buffer Clear
	memset (gCmdSave, 0x00, (CMD_SAVE_NUM * CONSOLE_BUFF_SIZE));

	return (status);
}


//**********************************************************************************
// コマンド履歴表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdHistoryLocalShow (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i, index, cmdIndex;

	if ((gCmdSaveIndex < CMD_SAVE_NUM) && (gCmdSave[gCmdSaveIndex][0] == 0))
	{
		// 最初から表示
		for (index=0; index<gCmdSaveIndex; index++)
			DEBUG_PRINT_FORCE ("%d : %s\n", index+1, gCmdSave[index]);
	}
	else
	{
		// 現在のIndex以降から取得(現在のIndexは０クリアされている為、その次から表示)
		cmdIndex = gCmdSaveIndex % CMD_SAVE_NUM;

		// 現在のIndex以降(最古の履歴)を表示
		for (i=0, index=cmdIndex; i<(CMD_SAVE_NUM - cmdIndex); i++, index++)
			DEBUG_PRINT_FORCE ("%d : %s\n", i+1, gCmdSave[index]);

		// コマンド履歴バッファの最初から表示
		for (index=0; index<cmdIndex; i++, index++)
			DEBUG_PRINT_FORCE ("%d : %s\n", i+1, gCmdSave[index]);
	}

	return (status);
}


//**********************************************************************************
//	match
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ptn						：ワイルドカード
//		str						：比較する文字列
//	[ OUTPUT ]
//		1						：マッチ
//		0						：マッチせず
//==================================================================================
int strmatch (const char *ptn, const char *str)
{
	switch (*ptn)
	{
		case '\0':
			return '\0' == *str;
		case '*':
			return strmatch (ptn+1, str) || (('\0' != *str) && strmatch(ptn, str+1));
		case '?':
			return ('\0' != *str) && strmatch (ptn+1, str+1);
		default:
			return ((unsigned char)*ptn == (unsigned char)*str) && strmatch (ptn+1, str+1);
	}

	return (0);
}


//**********************************************************************************
//	コマンド履歴表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdHistoryShow (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdHistoryShowHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// コマンド履歴表示
		cmdHistoryLocalShow ();
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,  __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	コマンド履歴表示Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdHistoryShowHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : A command histories is indicated.\n");
	DEBUG_PRINT_FORCE ("  Command           : history\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	コマンド履歴クリア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdHistoryClear (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdHistoryClearHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// コマンド履歴クリア
		cmdHistoryLocalClear ();
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__,  __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	コマンド履歴表示Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdHistoryClearHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A command histories is cleared.\n");
	DEBUG_PRINT_FORCE ("  Command           : historyclear\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
