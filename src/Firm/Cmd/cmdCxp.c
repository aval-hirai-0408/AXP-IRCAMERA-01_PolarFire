//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdCxp.c -CXP Device Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "../CXP/cxp.h"


#if defined (MODE_CXP)
//**********************************************************************************
//	CXP FIFO Read
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpFifoRead (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int adrs, size;
	int port;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCxpFifoReadHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Port取得
		if (sscanf (gCmdArg[1], "%x", &port) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Size設定
		size = 4;

		// CXP FifoデータをDDRに格納
		if ((status = cxpFifoToDdr (port, (unsigned long)&adrs, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%08x\n", adrs);
	}
	else if (argc == 3)
	{
		// Port取得
		if (sscanf (gCmdArg[1], "%x", &port) != 1)
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

		// アドレス取得
		if (sscanf (gCmdArg[3], "%x", &size) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// CXP FifoデータをDDRに格納
		if ((status = cxpFifoToDdr (port, adrs, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//data32 = IN32 (adrs);
		//DEBUG_PRINT_FORCE ("%08x\n", data32);
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
//	CXP FIFO Read Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpFifoReadHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP FIFO is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxpfifo [param0] [param1] [param2]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : port\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : address\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : size\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	CXP FIFO Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpFifoWrite (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int data;
	int port;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCxpFifoWriteHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// port取得
		if (sscanf (gCmdArg[1], "%d", &port) != 1)
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

		// CXP Fifoデータ出力
		if ((status = cxpWriteFifo32 (port, CXP_REG_CMD_MODE_I2C, data, 0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// CXP Fifoデータ出力
		//size8 = 1;
		//if ((status = cxpWriteFifoStart (size8)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;
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
//	CXP FIFO Write Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpFifoWriteHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP FIFO is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxpfifo [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : port\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : data(4byte)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	CXP Send Adrs
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpSendAdrs (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int adrs;
	int port;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCxpSendAdrsHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// port取得
		if (sscanf (gCmdArg[1], "%x", &port) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// CXP 送信アドレス取得
		if ((status = cxpGetSendCurrentAdrs (port, &adrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("0x%x ", adrs);
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
//	CXP Send AdrsHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpSendAdrsHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP Send adrs is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxpsendadrs [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : port\n");
	DEBUG_PRINT_FORCE ("  Output Param      : send adress\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	CXP Recv Adrs
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpRecvAdrs (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int adrs;
	int port;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCxpRecvAdrsHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// port取得
		if (sscanf (gCmdArg[1], "%d", &port) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// CXP 受信アドレス取得
		if ((status = cxpGetRecvCurrentAdrs (port, &adrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("0x%x ", adrs);
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
//	CXP Recv Adrs Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpRecvAdrsHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP recv adrs is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxprecvadrs [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : port\n");
	DEBUG_PRINT_FORCE ("  Output Param      : recv adress\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	CXP Connection Config
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpConnectionConfig (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCxpConnectionConfigHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get Connection Config
		if ((status = cxpGetConectionConfig (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("0x%x ", data);
	}
	else if (argc == 2)
	{
		// データ取得
		if (sscanf (gCmdArg[1], "%x", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Set Connection Config
		if ((status = cxpSetConectionConfig (data)) != AVAL_STATUS_SUCCESS)
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
//	CXP Connection Config Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpConnectionConfigHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP Connection Config is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxpconectionconfig [param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : data\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");
	
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP Connection Config is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxpconectionconfig\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : data\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Data]\n");
	DEBUG_PRINT_FORCE ("  1.25G  : 0x%x\n", CXP_RATE_1_25G);
	DEBUG_PRINT_FORCE ("  2.50G  : 0x%x\n", CXP_RATE_2_50G);
	DEBUG_PRINT_FORCE ("  3.125G : 0x%x\n", CXP_RATE_3_125G);
	DEBUG_PRINT_FORCE ("  5.00G  : 0x%x\n", CXP_RATE_5_00G);
	DEBUG_PRINT_FORCE ("  6.35G  : 0x%x\n", CXP_RATE_6_25G);
	DEBUG_PRINT_FORCE ("  10.00G : 0x%x\n", CXP_RATE_10_00G);
	DEBUG_PRINT_FORCE ("  12.5G  : 0x%x\n", CXP_RATE_12_50G);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	IF FPGA ReConfig
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIfFpgaReConfig (void *str)
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
			cmdIfFpgaReConfigHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
#if defined (MODE_FPGA_PF)
		// IF FPGA ReConfig
		if ((status = fpgaUpdatePolarFireReconfig ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif
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
//	IF FPGA REConfig Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIfFpgaReConfigHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : IF FPGA Reconfig is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : iffpga-reconfig\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");
	
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Cxp I2C
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpI2c (void *str)
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
			cmdCxpI2cHelp (NULL);
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

		// I2C Read
		if ((status = cxpI2cRegRead ((unsigned int)adrs, (unsigned char *)&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%02x ", data);
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

		// I2C Write
		if ((status = cxpI2cRegWrite (adrs, data)) != AVAL_STATUS_SUCCESS)
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
//	CXP I2C Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpI2cHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP I2C is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxpi2c [param]]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : adrs\n");
	DEBUG_PRINT_FORCE ("  Output Param      : data\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP I2C is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxpi2c [param0] [param1]]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : adrs\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : data\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	IOアクセス
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesCxpI2cMain (void *str)
{
	int status;

	status = cmdAcesDumpCxpI2cMain (str, MODE_ACES);

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
int cmdAcesCxpI2cMainHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Polafire IO Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxpio [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Address(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Dump
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDumpCxpI2cMain (void *str)
{
	int status;

	status = cmdAcesDumpCxpI2cMain (str, MODE_DUMP);

	return (status);
}


//**********************************************************************************
//	Dumpアクセス Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDumpCxpI2cMainHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Polafilre Dump Access.\n");
	DEBUG_PRINT_FORCE ("  Command           : dumpi2c [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Address(Hex)\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Aces/Dump
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：MODE_ACES=アクセス/MODE_DUMP=dump
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesDumpCxpI2cMain (void *str, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned long adrs = -1;
	unsigned int acesSize;
	int rtn = mode;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			if (mode == MODE_DUMP)
				cmdDumpCxpI2cMainHelp (NULL);
			else if (mode == MODE_ACES)
				cmdAcesCxpI2cMainHelp (NULL);

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

	rtn = mode;

	// アクセスサイズ(default = 1)
	acesSize = 1;

	while (1)
	{
		// Mode保存
		mode = rtn;

		if (rtn == MODE_ACES)
		{
			rtn = cmdAcesCxpI2c (&adrs, &acesSize);
		}
		else if (rtn == MODE_DUMP)
		{
			rtn = cmdDumpCxoP2c (&adrs, &acesSize);
		}

		if (rtn == AVAL_STATUS_SUCCESS)
		{
			adrs = -1;
			//goto _RETRY;
			break;
		}
	}

_DONE:
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	アクセス
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs					：アクセスするアドレス
//		acesSize				：アクセスサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAcesCxpI2c (unsigned long *adrs, unsigned int *acesSize)
{
	unsigned int temp;
	char input[CONSOLE_BUFF_SIZE];
	unsigned int data32;
	unsigned char data8;
	int status;

	while (1)
	{
		DEBUG_PRINT_FORCE("0x%lx : ", *adrs);
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
					// I2C Read
					if ((status = cxpI2cRegRead ((unsigned int)*adrs, (unsigned char *)&data8)) != AVAL_STATUS_SUCCESS)
					{
						DEBUG_PRINT_FORCE ("\nError - Read(Byte) Error. Error = %d\n", status);
					}
					else
					{
						DEBUG_PRINT_FORCE("     0x%02x\n",  data8);
					}
				}
				continue;
			}
			// Dump Mode
			else if (input[1] == 'd')
			{
				return (MODE_DUMP);
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

			if (*acesSize == 1)
			{
				// I2C Read
				if ((status = cxpI2cRegWrite ((unsigned int)*adrs, (unsigned char)data32)) != AVAL_STATUS_SUCCESS)
				{
					DEBUG_PRINT_FORCE ("\nError - Write(Byte) Error. Error = %d\n", status);
				}
			}
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
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDumpCxoP2c (unsigned long *adrs, unsigned  int *acesSize)
{
	char input[CONSOLE_BUFF_SIZE];
	unsigned int size;
	unsigned int i, j;
	unsigned char data8;
	unsigned int offset;
	unsigned int changeAces;
	int status;
	unsigned char dataBuff[16];
	unsigned char *ptr8;

	*adrs &= 0xffffffffffffff00LL;

	ptr8 = (unsigned char *)dataBuff;
	//ptr16 = (unsigned short *)dataBuff;
	//ptr32 = (unsigned int *)dataBuff;

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

				if ((status = cxpI2cRegRead ((unsigned int)*adrs, &data8)) != AVAL_STATUS_SUCCESS)
				{
					DEBUG_PRINT_FORCE ("\nError - Read(Byte) Error. Error = %d / Address = 0x%lx\n", status, *adrs);
				}
				else
				{
					*ptr8++ = data8;
					DEBUG_PRINT_FORCE("%02x", data8);
				}

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
//	CXP Test Packet Rx
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpTestPacketRx (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned long long data64;
	int port;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCxpTestPacketRxHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// port取得
		if (sscanf (gCmdArg[1], "%d", &port) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Get Test Packet Rx Count
		if ((status = cxpGetTestPacketRxCount (port, &data64)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%lld ", data64);
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
//	CXP Test Packet Rx Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpTestPacketRxHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP Test Packet RX Count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : packetrx [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : port\n");
	DEBUG_PRINT_FORCE ("  Output Param      : CXP Test Packet RX Count\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	CXP Test Packet Err
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpTestPacketErr (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int data;
	int port;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCxpTestPacketErrHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// port取得
		if (sscanf (gCmdArg[1], "%d", &port) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Get Test Packet Err Count
		if ((status = cxpGetTestPacketErrCount (port, &data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", data);
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
//	CXP Test Packet Err Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpTestPacketErrHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP Test Packet Err Count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : packeterr [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : port\n");
	DEBUG_PRINT_FORCE ("  Output Param      : CXP Test Packet Err Count\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	CXP Rate
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpRate (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int cxpRate;
	double cxpRateGbps;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCxpRateHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get Rate
		if ((status = cxpGetRateData (&cxpRate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get Rate
		if ((status = cxpGetRegToSpeed (cxpRate, &cxpRateGbps)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.3f ", cxpRateGbps);
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
//	CXP Rate Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpRateHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("\n[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP rate is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxprate\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : CXP Rate\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	CXP Connection
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpConnection (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int cxpRate;
	unsigned int connection;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCxpConnectionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get Rate
		if ((status = cxpGetRateData (&cxpRate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get CXP Single or Dual
		if ((status = cxpGetRegToConnection (cxpRate, &connection)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", connection);
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
//	CXP Connection Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCxpConnectionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("\n[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : CXP connection is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : cxpconnection\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : CXP Connection\n\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[CXP Connectionn]\n");
	DEBUG_PRINT_FORCE ("  1 : Single\n");
	DEBUG_PRINT_FORCE ("  2 : Dual\n\n");

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if defined (MODE_CXP)

// eof
