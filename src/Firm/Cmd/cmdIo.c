//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdIo.c - IO Access
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"

#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
#include "../mtd/mtdFeatures.h"
#include "../mtd/mtdApiTypes.h"
#include "../mtd/mtdAPI.h"
#endif


//**********************************************************************************
//	Memory Aces(Byte)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIo8 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int adrs;
	unsigned char data8;
	unsigned int data32;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdIoXHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		//------------------------------------------------------------
		// Read
		//------------------------------------------------------------
		
		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ取得
		data8 = IN8 (adrs);

		DEBUG_PRINT_FORCE ("%x ", data8);
	}
	else if (argc == 3)
	{
		//------------------------------------------------------------
		// Write
		//------------------------------------------------------------

		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ取得
		if (sscanf (gCmdArg[2], "%x", &data32) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ設定
		data8 = (unsigned char)data32;
		OUT8 (adrs, data8);
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
//	Memory Aces(Word)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIo16 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int adrs;
	unsigned short data16;
	unsigned int data32;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdIoXHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		//------------------------------------------------------------
		// Read
		//------------------------------------------------------------
		
		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ取得
		data16 = IN16 (adrs);

		DEBUG_PRINT_FORCE ("%04x ", data16);
	}
	else if (argc == 3)
	{
		//------------------------------------------------------------
		// Write
		//------------------------------------------------------------

		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ取得
		if (sscanf (gCmdArg[2], "%x", &data32) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ設定
		data16 = (unsigned short)data32;
		OUT16 (adrs, data16);
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
//	Memory Aces(Long)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIo32 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int adrs;
	unsigned int data32;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdIoXHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		//------------------------------------------------------------
		// Read
		//------------------------------------------------------------
		
		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ取得
		data32 = IN32 (adrs);

		DEBUG_PRINT_FORCE ("%08x ", data32);
	}
	else if (argc == 3)
	{
		//------------------------------------------------------------
		// Write
		//------------------------------------------------------------

		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ取得
		if (sscanf (gCmdArg[2], "%x", &data32) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}


		// データ設定
		OUT32 (adrs, data32);
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
//	Memory Aces(Float)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIof (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int adrs;
	unsigned int data32;
	float dataF;
	float *pDataF;
	unsigned int *pAdrs;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdIoXHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		//------------------------------------------------------------
		// Read
		//------------------------------------------------------------

		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ取得
		pDataF = (float *)(unsigned long)adrs;
		DEBUG_PRINT_FORCE ("%f ", *pDataF);
	}
	else if (argc == 3)
	{
		//------------------------------------------------------------
		// Write
		//------------------------------------------------------------

		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ取得
		if (sscanf (gCmdArg[2], "%f", &dataF) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ設定
		pAdrs = (unsigned int *)&dataF;
		data32 = *pAdrs;
		OUT32 (adrs, data32);
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
//	Memory Aces Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIoXHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Read Memory Byte Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : ioX [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Address(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Read Data(Hex)\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Write Memory Byte Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : io8 [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Address(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Write Data(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	QSPI Flash Aces
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesQspiFlashMain (void *str)
{
	cmdAcesDiagDumpMain (MODE_ACES, FLASH_DEV_TYPE_QSPI_FLASH);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	QSPI Flash Dump
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDumpQspiFlashMain (void *str)
{
	cmdAcesDiagDumpMain (MODE_DUMP, FLASH_DEV_TYPE_QSPI_FLASH);
	return (AVAL_STATUS_SUCCESS);
}


#if defined (IF_GIGE)
//**********************************************************************************
//	GigE Phy Register Aces
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesGigEPhyMain (void *str)
{
	int status;

	status = cmdAcesDiagDumpMain (MODE_ACES, DEV_TYPE_GIGE_PHY);

	return (status);
}


//**********************************************************************************
//	GigE Phy Register Dump
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDumpGigEPhyMain (void *str)
{
	int status;

	status = cmdAcesDiagDumpMain (MODE_DUMP, DEV_TYPE_GIGE_PHY);

	return (status);
}
#endif // #if defined (IF_GIGE)


//**********************************************************************************
//	IOアクセス
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesMain (void *str)
{
	int status;

	// Memory Access
	status = cmdAcesDumpMain (str, MODE_ACES);

	return (status);
}


//**********************************************************************************
//	IOアクセス Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesMainHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Memory IO Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : io [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Address(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	メモリアクセス
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDumpMain (void *str)
{
	int status;

	// Memory Dump
	status = cmdAcesDumpMain (str, MODE_DUMP);

	return (status);
}


//**********************************************************************************
//	メモリアクセス Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDumpMainHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Memory Dump Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : dump [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Address(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Aces/Dump
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//		mode					：MODE_ACES=アクセス/MODE_DUMP=dump
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesDumpMain (void *str, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned long adrs = -1;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			if (mode == MODE_DUMP)
				cmdDumpMainHelp (NULL);
			else if (mode == MODE_ACES)
				cmdAcesMainHelp (NULL);

			goto _DONE;
		}
	}

	if (argc != 2)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

	// アドレス取得
	if (sscanf (gCmdArg[1], "%lx", &adrs) != 1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

	// Aces
	status = cmdAcesDiagDumpMain2 (adrs, mode, DEV_TYPE_MEMORY);

_DONE:
	return (status);
}



//**********************************************************************************
//	Aces/Dump
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：MODE_ACES=アクセス/MODE_DUMP=dump
//		devType					：Device Type
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesDiagDumpMain (int mode, int devType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned long adrs = -1;
	char input[CONSOLE_BUFF_SIZE];

	memset(input, 0, CONSOLE_BUFF_SIZE);

	while(1)
	{
		DEBUG_PRINT_FORCE("Address = 0x");
		fflush(stdout);
		memset(input, 0, CONSOLE_BUFF_SIZE);
		cmdCharGet(input);

		if ((input[0] == 'q') || (input[0] == 'Q'))
			return (AVAL_STATUS_SUCCESS);

		if (input[0] != RETURN_KEY)
			break;
	}

	// Set Adrs
	sscanf(input, "%lx", &adrs);

	// Dump Main
	status = cmdAcesDiagDumpMain2 (adrs, mode, devType);

	return (status);
}


//**********************************************************************************
//	Aces/Dump
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：MODE_ACES=アクセス/MODE_DUMP=dump
//		devType					：Device Type
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesDiagDumpMain2 (unsigned long adrs, int mode, int devType)
{
	unsigned int acesSize;
	int rtn = mode;

	// アクセスサイズ(default = 4)
	acesSize = 4;

	while (1)
	{
		// Mode保存
		mode = rtn;

		if (rtn == MODE_ACES)
		{
			rtn = cmdDiagDeviceAces (&adrs, &acesSize, devType);
		}
		else if (rtn == MODE_DUMP)
		{
			rtn = cmdDiagDeviceDump (&adrs, &acesSize, devType);
		}

		if (rtn == AVAL_STATUS_SUCCESS)
		{
			adrs = -1;
			//goto _RETRY;
			break;
		}
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	アクセス
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：アクセスするアドレス
//		acesSize				：アクセスサイズ
//		devType					：Device Type
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDeviceAces (unsigned long *adrs, unsigned int *acesSize, int devType)
{
	unsigned int temp;
	char input[CONSOLE_BUFF_SIZE];
	unsigned int changeAces;
	unsigned int data32;
	//unsigned short data16;
	//unsigned char data8;
	int status;

	while (1)
	{
		DEBUG_PRINT_FORCE("0x%08lx : ", *adrs);
		fflush(stdout);
		memset(input, 0, CONSOLE_BUFF_SIZE);
		cmdCharGet(input);

		//--------------------------------------------------
		// 終了
		//--------------------------------------------------
		if ((input[0] == 'q') || (input[0] == 'Q'))
		{
			break;
		}
		//--------------------------------------------------
		// Read or モード切り替え
		//--------------------------------------------------
		else if (input[0] == '@')
		{
			// Read
			if (input[1] == RETURN_KEY)
			{
				if ((status = cmdDiagDeviceRead ((unsigned int)*adrs, (unsigned int *)&data32, *acesSize, devType)) == AVAL_STATUS_SUCCESS)
				{
					if (*acesSize == 1)
						DEBUG_PRINT_FORCE("     0x%02x\n",  (unsigned char)data32);
					else if (*acesSize == 2)
						DEBUG_PRINT_FORCE("     0x%04x\n",  (unsigned short)data32);
					else
						DEBUG_PRINT_FORCE("     0x%08x\n",  data32);
				}

				continue;
			}
			// Dump Mode
			else if (input[1] == 'd')
			{
				return (MODE_DUMP);
			}
			// アクセスサイズ変更
			else if ((input[1] == '1') || (input[1] == '2') || (input[1] == '4'))
			{
				sscanf (&input[1], "%d", &changeAces);
				*acesSize = changeAces;
			}
			// Erase
			else if (input[1] == 'e')
			{
				DEBUG_PRINT_FORCE("Flash Erase?[y/n] > ");
				fflush(stdout);
				memset(input, 0, CONSOLE_BUFF_SIZE);
				cmdCharGet(input);

				if(input[0] == 'y')
				{
					DEBUG_PRINT_FORCE ("Erase Start..\n");
					cmdDiagDeviceErase (*adrs, devType);
				}
			}
			else
			{
				DEBUG_PRINT_FORCE("\nError - Invalid Parameter Error = %s\n\n", input);
			}
		}
		//--------------------------------------------------
		// 前回の領域へのアクセス
		//--------------------------------------------------
		else if (input[0] == '-')
		{
			if (input[1] == RETURN_KEY)
			{
				if (*acesSize == 1)
					*adrs -= 1;
				else if (*acesSize == 2)
					*adrs -= 2;
				else
					*adrs -= 4;
			}
			else
			{
				sscanf (&input[1], "%x", &temp);

				if (*acesSize == 2)
					temp &= 0xfffffffe;
				else if (*acesSize == 4)
					temp &= 0xfffffffc;

				*adrs -= temp;
			}

			continue;
		}
		//--------------------------------------------------
		// 指定オフセット領域へのアクセス
		//--------------------------------------------------
		else if (input[0] == '+')
		{
			if (input[1] == RETURN_KEY)
			{
				if (*acesSize == 1)
					*adrs -= 1;
				else if (*acesSize == 2)
					*adrs -= 2;
				else
					*adrs -= 4;
			}
			else
			{
				sscanf (&input[1], "%x", &temp );

				if (*acesSize == 2)
					temp &= 0xfffffffe;
				else if (*acesSize == 4)
					temp &= 0xfffffffc;

				*adrs += temp;
				continue;
			}
		}
		//--------------------------------------------------
		// 次の領域へのアクセス
		//--------------------------------------------------
		else if (input[0] == RETURN_KEY)
		{
			if (*acesSize == 1)
				*adrs += 1;
			else if (*acesSize == 2)
				*adrs += 2;
			else
				*adrs += 4;

			continue;
		}
		else
		{
			//--------------------------------------------------
			// Write
			//--------------------------------------------------
			data32 = 0;
			sscanf(input, "%x", &data32);
			cmdDiagDeviceWrite ((unsigned int)*adrs, data32, *acesSize, devType);
		}
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Dump
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：アドレス
//		acesSize				：アクセスサイズ
//		devType					：Device Type
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDeviceDump (unsigned long *adrs, unsigned  int *acesSize, int devType)
{
	char input[CONSOLE_BUFF_SIZE];
	unsigned int size;
	unsigned int i, j;
	unsigned char data8;
	unsigned short data16;
	unsigned int data32;
	unsigned int offset;
	unsigned int changeAces;
	int status;
	unsigned char dataBuff[16];
	unsigned char *ptr8;
	unsigned short *ptr16;
	unsigned int *ptr32;

	*adrs &= 0xffffffffffffff00LL;

	ptr8 = (unsigned char *)dataBuff;
	ptr16 = (unsigned short *)dataBuff;
	ptr32 = (unsigned int *)dataBuff;

	*adrs &= 0xffffffffffffff00LL;

	if ((*acesSize == 1) || (*acesSize == 2) || (*acesSize == 4))
	{
		size = 256 / *acesSize;
	}
	else
	{
		DEBUG_PRINT_FORCE("Error - Access Size Error = %d\n", *acesSize);
		return (AVAL_STATUS_ERROR);
	}

	while (1)
	{
MORE_DUMP:
		for (i=0; i<size; i++, *adrs += *acesSize )
		{
			if (*acesSize == 1)
			{
				if ((i%16) == 0)
				{
					if (i != 0)
					{
						DEBUG_PRINT_FORCE("  ");
						for (j=0; j<16; j++)
						{
							if ((dataBuff[j] >= 0x20) && (dataBuff[j] < 0x7f))
								DEBUG_PRINT_FORCE("%c", dataBuff[j]);
							else
								DEBUG_PRINT_FORCE(".");
						}
					}

					ptr8 = (unsigned char *)dataBuff;
					DEBUG_PRINT_FORCE("\n");
					DEBUG_PRINT_FORCE("%08lx:  ", *adrs);
				}

				if ((status = cmdDiagDeviceRead ((unsigned int)*adrs, (unsigned int *)&data8, *acesSize, devType)) != AVAL_STATUS_SUCCESS)
					continue;

				*ptr8++ = data8;
				DEBUG_PRINT_FORCE("%02x", data8);


//				if (i!=0 && (i%2) && (i%16 != 0))
					DEBUG_PRINT_FORCE(" ");
			}

			if (*acesSize == 2)
			{
				if ((i%8) == 0)
				{
					if (i != 0)
					{
						DEBUG_PRINT_FORCE("  ");
						for (j=0; j<16; j++)
						{
							if ((dataBuff[j] >= 0x20) && (dataBuff[j] < 0x7f))
								DEBUG_PRINT_FORCE("%c", dataBuff[j]);
							else
								DEBUG_PRINT_FORCE(".");
						}
					}

					ptr16 = (unsigned short *)dataBuff;
					DEBUG_PRINT_FORCE("\n");
					DEBUG_PRINT_FORCE("%08lx:  ", *adrs);
				}

				if ((status = cmdDiagDeviceRead ((unsigned int)*adrs, (unsigned int *)&data16, *acesSize, devType)) != AVAL_STATUS_SUCCESS)
					continue;

				*ptr16++ = data16;
				DEBUG_PRINT_FORCE("%04x ", data16);
			}

			if (*acesSize == 4)
			{
				if ((i%4) == 0)
				{
					if (i != 0)
					{
						DEBUG_PRINT_FORCE("  ");
						for (j=0; j<16; j++)
						{
							if ((dataBuff[j] >= 0x20) && (dataBuff[j] < 0x7f))
								DEBUG_PRINT_FORCE("%c", dataBuff[j]);
							else
								DEBUG_PRINT_FORCE(".");
						}
					}

					ptr32 = (unsigned int *)dataBuff;
					DEBUG_PRINT_FORCE("\n");
					DEBUG_PRINT_FORCE("%08lx:  ", *adrs);
				}

				if ((status = cmdDiagDeviceRead ((unsigned int)*adrs, (unsigned int *)&data32, *acesSize, devType)) != AVAL_STATUS_SUCCESS)
					continue;

				*ptr32++ = data32;
				DEBUG_PRINT_FORCE("%04x",((data32&0xffff0000)>>16));
				DEBUG_PRINT_FORCE("%04x ", (data32&0xffff));
			}
		}

		// 最終行の表示
		DEBUG_PRINT_FORCE("  ");
		for (j=0; j<16; j++)
		{
			if ((dataBuff[j] >= 0x20) && (dataBuff[j] < 0x7f))
				DEBUG_PRINT_FORCE("%c", dataBuff[j]);
			else
				DEBUG_PRINT_FORCE(".");
		}

		DEBUG_PRINT_FORCE("\n");

MORE_KEY:
		DEBUG_PRINT_FORCE("next ? push return key > ");
		fflush(stdout);
		cmdCharGet(input);

		//--------------------------------------------------
		// 終了
		//--------------------------------------------------
		if ((input[0] == 'q') || (input[0] == 'Q'))
		{
			return (AVAL_STATUS_SUCCESS);
		}
		//--------------------------------------------------
		// 次の領域
		//--------------------------------------------------
		else if (input[0] == RETURN_KEY)
		{
			goto MORE_DUMP;
		}
		//--------------------------------------------------
		// 前回の領域
		//--------------------------------------------------
		else if (input[0] == '-')
		{
			if (input[1] == RETURN_KEY)
			{
				*adrs -= 512;
			}
			else
			{
				sscanf (&input[1], "%x", &offset);
				offset &= 0xffffff00;
				*adrs -= offset;
				*adrs -= 256;
			}
			goto MORE_DUMP;
		}
		//--------------------------------------------------
		// 指定オフセットアドレスへの移動
		//--------------------------------------------------
		else if (input[0] == '+')
		{
			if (input[1] != RETURN_KEY)
			{
				sscanf (&input[1], "%x", &offset);
				offset &= 0xffffff00;
				*adrs += offset;
				*adrs -= 256;
			}
			goto MORE_DUMP;
		}
		//--------------------------------------------------
		// モード切替
		//--------------------------------------------------
		else if (input[0] == '@')
		{
			// メモリDump
			if (input[1] == 'm')
			{
				*adrs -= 256;
				return (MODE_ACES);
			}
			// アクセスサイズ変更
			else if ((input[1] == '1') || (input[1] == '2') || (input[1] == '4'))
			{
				sscanf (&input[1], "%d", &changeAces);

				*acesSize = changeAces;
				size = 256 / *acesSize;
			}
			// Erase
			else if (input[1] == 'e')
			{
				DEBUG_PRINT_FORCE("Flash Erase?[y/n] > ");
				fflush(stdout);
				memset(input, 0, CONSOLE_BUFF_SIZE);
				cmdCharGet(input);

				if(input[0] == 'y')
				{
					DEBUG_PRINT_FORCE ("Erase Start..\n");
					cmdDiagDeviceErase (*adrs, devType);
				}
			}
			// @は現在のアドレスを再表示するそれ以外はエラー
			else if (input[1] != '\0')
			{
				DEBUG_PRINT_FORCE("\nError - Invalid Parameter Error = %s\n\n", input);
				goto MORE_KEY;
			}

			*adrs -= 256;
			goto MORE_DUMP;
		}

		goto MORE_KEY;
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Device Read
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：アドレス
//		pData					：データを格納するポインタ
//		acesSize				：アクセスサイズ
//		devType					：Device Type
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDeviceRead (unsigned long adrs, unsigned int *pData, int acesSize, int devType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pData8;
	unsigned short *pData16;
	unsigned int *pData32;
#if defined (MODE_GIGE_10G)
	unsigned int dev;
#endif

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Device Read pData NULL Parameter Error\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//------------------------------------------------------------
	// QSPI Flash
	//------------------------------------------------------------
	if (devType == FLASH_DEV_TYPE_QSPI_FLASH)
	{
		if (acesSize == 1)
		{
			pData8 = (unsigned char *)pData;
			status = qspiFlashRead (adrs, (unsigned char *)pData8, 1);
		}
		else if (acesSize == 2)
		{
			pData16 = (unsigned short *)pData;
			status = qspiFlashRead (adrs, (unsigned char *)pData16, 2);
		}
		else
		{
			pData32 = (unsigned int *)pData;
			status = qspiFlashRead (adrs, (unsigned char *)pData32, 4);
		}

		if (status != AVAL_STATUS_SUCCESS)
		{
			sprintf (gLogMsgBuff, "\nError - QSPI Flash Read Error. Error = %d, Aces Size = %d\n", status, acesSize);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

	//------------------------------------------------------------
	// Memory
	//------------------------------------------------------------
	else if (devType == DEV_TYPE_MEMORY)
	{
		if (acesSize == 1)
		{
			pData8 = (unsigned char *)pData;
			*pData8 = IN8 (adrs);
		}
		else if (acesSize == 2)
		{
			pData16 = (unsigned short *)pData;
			*pData16 = IN16 (adrs);
		}
		else
		{
			pData32 = (unsigned int *)pData;
			*pData32 = IN32 (adrs);
		}
	}
	
	//------------------------------------------------------------
	// Phy
	//------------------------------------------------------------
#if defined (MODE_GIGE_10G)
	else if (devType == DEV_TYPE_GIGE_PHY)
	{
		dev = (adrs >> 16) & 0xffff;
		adrs = adrs & 0xffff;
		if ((status = phyReadReg (dev, adrs, pData)) != AVAL_STATUS_SUCCESS)
		{
			sprintf (gLogMsgBuff, "\nError - Phy Read Error. Error = %d, Aces Size = %d\n", status, acesSize);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
	#else
	else if (devType == DEV_TYPE_GIGE_PHY)
	{
		pData8 = (unsigned char *)pData;
		if ((status = phyReadReg (adrs, pData)) != AVAL_STATUS_SUCCESS)
		{
			sprintf (gLogMsgBuff, "\nError - Phy Read Error. Error = %d, Aces Size = %d\n", status, acesSize);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
#endif

	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff,  "Device Read devType(%d) Parameter Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Device Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：アドレス
//		data					：データ
//		acesSize				：アクセスサイズ
//		devType					：Device Type
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDeviceWrite (unsigned long adrs, unsigned int data, int acesSize, int devType)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char data8;
	unsigned short data16;
	unsigned int data32;
#if defined (MODE_GIGE_10G)
	unsigned int dev;
#endif

	//------------------------------------------------------------
	// QSPI Flash
	//------------------------------------------------------------
	if (devType == FLASH_DEV_TYPE_QSPI_FLASH)
	{
		if (acesSize == 1)
		{
			data8 = (unsigned char)data;
			status = qspiFlashWrite (adrs, (unsigned char *)&data8, 1);
		}
		else if (acesSize == 2)
		{
			data16 = (unsigned short)data;
			status = qspiFlashWrite (adrs, (unsigned char *)&data16, 2);
		}
		else
		{
			data32 = (unsigned int)data;
			status = qspiFlashWrite (adrs, (unsigned char *)&data32, 4);
		}

		if (status != AVAL_STATUS_SUCCESS)
		{
			sprintf (gLogMsgBuff, "\nError - QSPI Flash Write Error. Error = %d, Aces Size = %d\n", status, acesSize);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}


	//------------------------------------------------------------
	// Memory
	//------------------------------------------------------------
	else if (devType == DEV_TYPE_MEMORY)
	{
		if (acesSize == 1)
		{
			data8 = (unsigned char)data;
			OUT8 (adrs, data8);
		}
		else if (acesSize == 2)
		{
			data16 = (unsigned short)data;
			OUT16 (adrs, data16);
		}
		else
		{
			data32 = (unsigned int)data;
			OUT32 (adrs, data32);
		}
	}

	
	//------------------------------------------------------------
	// Phy
	//------------------------------------------------------------
#if defined (MODE_GIGE_10G)
	else if (devType == DEV_TYPE_GIGE_PHY)
	{
		dev = (adrs >> 16) & 0xffff;
		adrs = adrs & 0xffff;
		if ((status = phyWriteReg (dev, adrs, data)) != AVAL_STATUS_SUCCESS)
		{
			sprintf (gLogMsgBuff, "\nError - Phy Write Error. Error = %d, Aces Size = %d\n", status, acesSize);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
#else
	else if (devType == DEV_TYPE_GIGE_PHY)
	{
		if ((status = phyWriteReg (adrs, data)) != AVAL_STATUS_SUCCESS)
		{
			sprintf (gLogMsgBuff, "\nError - Phy Write Error. Error = %d, Aces Size = %d\n", status, acesSize);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
#endif

	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Device Write devType(%d) Parameter Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Device Erase
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：アドレス
//		devType					：Device Type
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDeviceErase (unsigned long adrs, int devType)
{
	int status = AVAL_STATUS_SUCCESS;

	//------------------------------------------------------------
	// QSPI Flash
	//------------------------------------------------------------
	if (devType == FLASH_DEV_TYPE_QSPI_FLASH)
	{
		if ((status = qspiFlashSectorErase (adrs)) != AVAL_STATUS_SUCCESS)
		{
			sprintf (gLogMsgBuff, "\nError - QSPI Flash Erase Error = %d / Adrs = 0x%lx\n\n", status, adrs);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Device Erase devType(%d) Parameter Error\n", devType);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Phy Aces(1Gbps)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
#if !defined (MODE_GIGE_10G)
int cmdAcesPhy (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int adrs, data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAcesPhyHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Phy Read
		if ((status = phyReadReg (adrs, &data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("0x%x ", data);
	}
	else if (argc == 3)
	{
		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ取得
		if (sscanf (gCmdArg[2], "%x", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Phy Write
		if ((status = phyWriteReg (adrs, data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
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
//	Phy Aces Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesPhyHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The phy register is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : iophy [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : adrs\n");
	DEBUG_PRINT_FORCE ("  Output Param      : data\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The phy register is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : iophy [param0] [paam1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : adrs(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : data(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

#else // #if !defined (MODE_GIGE_10G)

//**********************************************************************************
//	Phy Aces(10Gbps)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesPhy (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int dev, adrs, data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAcesPhyHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// Dev取得
		if (sscanf (gCmdArg[1], "%x", &dev) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// アドレス取得
		if (sscanf (gCmdArg[2], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Phy Read
		if ((status = phyReadReg (dev, adrs, &data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("0x%x ", data);
	}
	else if (argc == 4)
	{
		// Dev取得
		if (sscanf (gCmdArg[1], "%x", &dev) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// アドレス取得
		if (sscanf (gCmdArg[2], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// データ取得
		if (sscanf (gCmdArg[3], "%x", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Phy Write
		if ((status = phyWriteReg (dev, adrs, data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
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
//	Phy Aces Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesPhyHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The phy register is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : iophy [param0]  [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : dev(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : adrs(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : data(Hex)\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The phy register is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : iophy [param0] [paam1] [paam2]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : dev(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : adrs(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : data(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Phy Dump
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyDump (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int option;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	if ((gInterFaceID != INTERFACE_GIGE) && (gInterFaceID != INTERFACE_GIGE20))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPhyDumpHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// option取得
		if (sscanf (gCmdArg[1], "%x", &option) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Phy Dump
		if ((status = phyDump (option)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
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
//	Phy Dump Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyDumpHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The phy register is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : phydump\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : option\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n\n");

	return (AVAL_STATUS_SUCCESS);
}

#endif // #else // #if !defined (MODE_GIGE_10G)

// eof
