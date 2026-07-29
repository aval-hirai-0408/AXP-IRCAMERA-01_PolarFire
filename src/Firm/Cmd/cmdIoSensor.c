//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdIoSensor.c - Sensor Register Access
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gAcTmg;
int gRegHold;


//**********************************************************************************
//	IOアクセス
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorAcesMain (void *str)
{
	int status;

	// IO Access
	status = cmdSensorAcesDumpMain (str, MODE_ACES);

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
int cmdSensorAcesMainHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Sensor Register IO Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : iosensor [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : ID(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Address(Hex)\n");
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
int cmdSensorDumpMain (void *str)
{
	int status;

	// Dump
	status = cmdSensorAcesDumpMain (str, MODE_DUMP);

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
int cmdSensorDumpMainHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Sensor Register Dump Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : dumpsensor [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : ID(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Address(Hex)\n");
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
int cmdSensorAcesDumpMain (void *str, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned long adrs = -1;
	unsigned int acesSize;
	int rtn;
	int argc;
	int id;
	//char refMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			if (mode == MODE_DUMP)
				cmdSensorDumpMainHelp (NULL);
			else if (mode == MODE_ACES)
				cmdSensorAcesMainHelp (NULL);

			goto _DONE;
		}
	}

	if (argc != 3)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

	// ID取得
	if (sscanf (gCmdArg[1], "%x", &id) != 1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

	// アドレス取得
	if (sscanf (gCmdArg[2], "%lx", &adrs) != 1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

	gAcTmg = 1;
	gRegHold = 0;

	// Aces Mode
	rtn = mode;

	// アクセスサイズ(default = 1)
	acesSize = 1;

	while (1)
	{
		// Mode保存
		mode = rtn;

		if (rtn == MODE_ACES)
		{
			rtn = cmdSensorAces (&id, &adrs, &acesSize);
		}
		else if (rtn == MODE_DUMP)
		{
			rtn = cmdSensorDump (&id, &adrs, &acesSize);
		}

		if (rtn == AVAL_STATUS_SUCCESS)
		{
			adrs = -1;
			//goto _RETRY;
			break;
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Registerアクセス
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		id						：アクセスするID
//		adrs					：アクセスするアドレス
//		acesSize				：アクセスサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorAces (int *id, unsigned long *adrs, unsigned int *acesSize)
{
	unsigned int temp;
	char input[CONSOLE_BUFF_SIZE];
	unsigned int data32;
	unsigned char data8;

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
				if (*acesSize == 1)
				{
					//data8 = (*(volatile unsigned char *)*adrs);
					if (sensorRegReadByte (*id, *adrs, &data8, gAcTmg, gRegHold) == AVAL_STATUS_SUCCESS)
						DEBUG_PRINT_FORCE("     0x%02x\n",  data8);
				}
				#if 0
				else if (*acesSize == 2)
				{
					data16 = (*(volatile unsigned short *)*adrs);
					DEBUG_PRINT_FORCE("     0x%04x\n",  data16);
				}
				else
				{
					data32 = (*(volatile unsigned int *)*adrs);
					DEBUG_PRINT_FORCE("     0x%08x\n",  data32);
				}
				#endif
				continue;
			}
			// Dump Mode
			else if (input[1] == 'd')
			{
				return (MODE_DUMP);
			}
			#if 0
			// アクセスサイズ変更
			else if ((input[1] == '1') || (input[1] == '2') || (input[1] == '4'))
			{
				sscanf (&input[1], "%d", &changeAces);
				*acesSize = changeAces;
			}
			#endif
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

			if (*acesSize == 1)
			{
				data8 = (unsigned char)data32;

				sensorRegWriteByte (*id, *adrs, data8, gAcTmg, gRegHold);
			}
			#if 0
			else if (*acesSize == 2)
			{
				data16 = (unsigned short)data32;
				(*(volatile unsigned short *)*adrs) = (unsigned short)data16;
			}
			else
			{
				(*(volatile unsigned int *)*adrs) = (unsigned int)data32;
			}
			#endif
		}
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	メモリDump
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		id						：アクセスするID
//		adrs					：アクセスするアドレス
//		acesSize				：アクセスサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorDump (int *id, unsigned long *adrs, unsigned int *acesSize)
{
	char input[CONSOLE_BUFF_SIZE];
	unsigned int size;
	unsigned int i, j;
	unsigned char data8;
	unsigned int offset;
	unsigned char dataBuff[16];
	unsigned char *ptr8;

	ptr8 = (unsigned char *)dataBuff;

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
		for (i=0; i<size; i++, *adrs += *acesSize)
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

				//data8 = (*(volatile unsigned char *)*adrs);
				if (sensorRegReadByte (*id, *adrs, &data8, gAcTmg, gRegHold) == AVAL_STATUS_SUCCESS)
					*ptr8++ = data8;
				else
					*ptr8++ = 0;

				DEBUG_PRINT_FORCE("%02x", data8);

				//if (i!=0 && (i%2) && (i%16 != 0))
					DEBUG_PRINT_FORCE(" ");
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
			if( input[1] == RETURN_KEY)
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
			#if 0
			// アクセスサイズ変更
			else if ((input[1] == '1') || (input[1] == '2') || (input[1] == '4'))
			{
				sscanf (&input[1], "%d", &changeAces);

				*acesSize = changeAces;
				size = 256 / *acesSize;
			}
			#endif
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
//	Sensor Aces(Byte)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIoSensor8 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int id, adrs;
	unsigned char id8, adrs8, data8;
	unsigned int data32;
	int acTmg, regHold;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdIoSensor8Help (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		//------------------------------------------------------------
		// Read
		//------------------------------------------------------------

		// ID取得
		if (sscanf (gCmdArg[1], "%x", &id) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		id8 = (unsigned char)id;

		// アドレス取得
		if (sscanf (gCmdArg[2], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		adrs8 = (unsigned char)adrs;

		// データ取得
		acTmg = 0;
		regHold = 0;
		if ((status = sensorRegReadByte (id8, adrs8, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%02x ", data8);
	}
	else if (argc == 4)
	{
		//------------------------------------------------------------
		// Write
		//------------------------------------------------------------

		// アドレス取得
		if (sscanf (gCmdArg[1], "%x", &id) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		id8 = id;

		// アドレス取得
		if (sscanf (gCmdArg[2], "%x", &adrs) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		adrs8 = adrs;

		// データ取得
		if (sscanf (gCmdArg[3], "%x", &data32) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		data8 = data32;

		// データ設定
		acTmg = 0;
		regHold = 0;
		if ((status = sensorRegWriteByte (id8, adrs8, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
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
//	Memory Aces(Byte) Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIoSensor8Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Read Sensor Register Byte Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : iosensor8 [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input0 Param      : id(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input1 Param      : Address(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Read Data(Hex)\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Write Sensor Register Byte Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : iosensor8 [param0] [param1] [param2]\n");
	DEBUG_PRINT_FORCE ("  Input0 Param0     : id(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Address(Hex)\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : Write Data(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)

// eof
