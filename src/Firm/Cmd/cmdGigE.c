//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdGigE.c - GigE Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (IF_GIGE)
//**********************************************************************************
//	Phy Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyCount (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int speed, selector, data32;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPhyCountHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// speed取得
		if (sscanf (gCmdArg[1], "%x", &speed) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// selector取得
		if (sscanf (gCmdArg[2], "%x", &selector) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Phy Get Count
		if ((status = phyGetCount (speed, selector, &data32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", data32);
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
//	Phy Count Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyCountHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The phy register is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : phycount [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : speed\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : selector\n");
	DEBUG_PRINT_FORCE ("  Output Param      : byte count\n\n");

	DEBUG_PRINT_FORCE ("[speed]\n");
	DEBUG_PRINT_FORCE ("  0x%04x : 10Base   Harf\n", MTD_SPEED_10M_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 10Base   Full\n", MTD_SPEED_10M_HD);
	DEBUG_PRINT_FORCE ("  0x%04x : 100Base  Harf\n", MTD_SPEED_100M_HD);
	DEBUG_PRINT_FORCE ("  0x%04x : 100Base  Full\n", MTD_SPEED_100M_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 1000Base Harf\n", MTD_SPEED_1GIG_HD);
	DEBUG_PRINT_FORCE ("  0x%04x : 1000Base Full\n", MTD_SPEED_1GIG_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 10GBase  Full\n", MTD_SPEED_10GIG_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 2.5GBase Full\n", MTD_SPEED_2P5GIG_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 5.0GBase Full\n\n", MTD_SPEED_5GIG_FD);

	DEBUG_PRINT_FORCE ("[Select]\n");
	DEBUG_PRINT_FORCE ("  0x00 : Tx\n");
	DEBUG_PRINT_FORCE ("  0x01 : Rx\n");
	DEBUG_PRINT_FORCE ("  0x02 : Err\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Phy Speed
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhySpeed (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int speed;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPhySpeedHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Phy Get Speed
		if ((status = phyGetSpeed (&speed)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("0x%x ", speed);
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
//	Phy Speed Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhySpeedHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The phy speed is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : physpeed\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : speed\n\n");

	DEBUG_PRINT_FORCE ("[speed]\n");
	DEBUG_PRINT_FORCE ("  0x%04x : 10Base   Harf\n", MTD_SPEED_10M_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 10Base   Full\n", MTD_SPEED_10M_HD);
	DEBUG_PRINT_FORCE ("  0x%04x : 100Base  Harf\n", MTD_SPEED_100M_HD);
	DEBUG_PRINT_FORCE ("  0x%04x : 100Base  Full\n", MTD_SPEED_100M_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 1000Base Harf\n", MTD_SPEED_1GIG_HD);
	DEBUG_PRINT_FORCE ("  0x%04x : 1000Base Full\n", MTD_SPEED_1GIG_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 10GBase  Full\n", MTD_SPEED_10GIG_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 2.5GBase Full\n", MTD_SPEED_2P5GIG_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 5.0GBase Full\n\n", MTD_SPEED_5GIG_FD);

	return (AVAL_STATUS_SUCCESS);
}


#if defined (MODE_GE_SPEED)
//**********************************************************************************
//	Gev Speed Config
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGevSpeedConfig (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int speed;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdGevSpeedConfigHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get gev speed
		if ((status = gevGetSpeedConfig (&speed)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("0x%x ", speed);
	}
	else if (argc == 2)
	{
		// speed取得
		if (sscanf (gCmdArg[1], "%x", &speed) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Set gev speed
		if ((status = gevSetSpeedConfig (speed)) != AVAL_STATUS_SUCCESS)
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
//	Gev Speed Config Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGevSpeedConfigHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The gev speed is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gevspeedconfig [param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : speed\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The gev speed is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : gevspeedconfig\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : speed\n\n");

	DEBUG_PRINT_FORCE ("[speed]\n");
	//DEBUG_PRINT_FORCE ("  0x%04x : 10Base   Harf\n", MTD_SPEED_10M_FD);
	//DEBUG_PRINT_FORCE ("  0x%04x : 10Base   Full\n", MTD_SPEED_10M_HD);
	//DEBUG_PRINT_FORCE ("  0x%04x : 100Base  Harf\n", MTD_SPEED_100M_HD);
	//DEBUG_PRINT_FORCE ("  0x%04x : 100Base  Full\n", MTD_SPEED_100M_FD);
	//DEBUG_PRINT_FORCE ("  0x%04x : 1000Base Harf\n", MTD_SPEED_1GIG_HD);
	DEBUG_PRINT_FORCE ("  0x%04x : 1000Base Full\n", GIGE_SPEED_1GIG_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 10GBase  Full\n", GIGE_SPEED_10GIG_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 2.5GBase Full\n", GIGE_SPEED_2P5GIG_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : 5.0GBase Full\n", GIGE_SPEED_5GIG_FD);
	DEBUG_PRINT_FORCE ("  0x%04x : Auto Negotiation\n\n", GIGE_SPEED_AUTO_NEGOTIATION);

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if defined (MODE_GE_SPEED)


//**********************************************************************************
//	GigE Calc Packet Delay Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGigEPacketDelayCalc (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int count;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdGigEPacketDelayCalcHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Packet Delay Calc
		if ((status = gigECalcPacketDelay (&count)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", count);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	GigE Calc Packet Delay Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdGigEPacketDelayCalcHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The GigE packet delay count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : packetdelaycal\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : count\n\n");

	return (AVAL_STATUS_SUCCESS);
}


#if defined (MODE_GIGE_10G)
//**********************************************************************************
//	Phy Firm Verison
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyFirmVersion (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	char ver[PHY_FIRM_VERSION_SIZE];

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPhyFirmVersionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Phy Firm Version
		if ((status = phyGetFirmVersionMem (ver)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%s ", ver);
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
//	Phy Firm Version Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyFirmVersionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Phy Firmware Version is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : phyfirmversion\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Firmware Version\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Phy API Verison
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyApiVersion (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	char ver[PHY_API_VERSION_SIZE];

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPhyApiVersionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Phy API Version
		if ((status = phyGetApiVersionMem (ver)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%s ", ver);
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
//	Phy API Version Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyApiVersionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Phy API Version is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : phyapiversion\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Phy Api Version\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Phy Temp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyTemp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	float temp;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPhyTempHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Phy Temp
		if ((status = phyGetTempCmd (&temp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%.2f ", temp);
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
//	Phy Temp Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyTempHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Phy temperature is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : phytemp\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : temperature\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if defined (MODE_GIGE_10G)
#endif // #if defined (IF_GIGE)

// eof
