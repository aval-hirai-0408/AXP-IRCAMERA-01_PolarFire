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
