//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdVoltageIfBoard.c - Voltage IF Board Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_VOLTAGE_IF_BOARD)
//**********************************************************************************
//	Interface Board Voltageコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfShow (void *str)
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
			cmdVoltIfShowHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Interface Board Voltage Show
		voltIfBoardShow();
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
//	Interface Board Voltageコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfShowHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The interface board volt is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : voltifshow\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : IF BoardVoltage Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Interface Board +5.0V Voltageコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfV5_0 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double data;

	// Check Interface
	if (gInterFaceID != INTERFACE_CXP)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdVoltIfV5_0Help (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = voltIfGet5_0 (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.3f ", data);
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
//	Interface Board +5.0V Voltageコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfV5_0Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The interface board +5.0V volt is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : voltif5\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Interface Board +5.0V\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Interface Board +24.0V Voltageコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfV24_0 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double data;

	// Check Interface
	if (gInterFaceID != INTERFACE_CXP)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdVoltIfV24_0Help (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = voltIfGet24_0 (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.3f ", data);
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
//	Interface Board +24.0V Voltageコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfV24_0Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The interface board +24.0V volt is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : voltif24\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Interface Board +24.0V\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Interface Board +1.8V Voltageコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfV1_8 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double data;

	// Check Interface
	if (gInterFaceID != INTERFACE_CXP)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdVoltIfV1_8Help (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = voltIfGet1_8 (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.3f ", data);
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
//	Interface Board +1.8V Voltageコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfV1_8Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The interface board +1.8V volt is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : voltif18\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Interface Board +1.8V\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Interface Board +3.3V Voltageコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfV3_3 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double data;

	// Check Interface
	if (gInterFaceID != INTERFACE_CXP)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdVoltIfV3_3Help (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = voltIfGet3_3 (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.3f ", data);
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
//	Interface Board +3.3V Voltageコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfV3_3Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The interface board +3.3V volt is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : voltif33\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Interface Board +3.3V\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Interface Board +1.25V Voltageコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfV1_25 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	double data;

	// Check Interface
	if (gInterFaceID != INTERFACE_CXP)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdVoltIfV1_25Help (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = voltIfGet1_25 (&data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.3f ", data);
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
//	Interface Board +1.25V Voltageコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdVoltIfV1_25Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The interface board +1.25V volt is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : voltif125\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Interface Board +1.25V\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

#endif // #if defined (MODE_VOLTAGE_IF_BOARD)

// eof
