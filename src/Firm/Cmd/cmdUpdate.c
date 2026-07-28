//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdUpdate.c - Update Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	Firmware All Update(全領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmUpdateAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed0 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFirmUpdateAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// プログラム設定ステート
		saveLed0 = ledSettingState ();

		// Firmware All Update
		if ((status = firmUpdateAll ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


//**********************************************************************************
//	Firmware All Update(全領域)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmUpdateAllHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : All data is updated.\n");
	DEBUG_PRINT_FORCE ("  Command           : update-all\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	XML Update
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmXmlUpdate (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed0 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFirmXmlUpdateHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// プログラム設定ステート
		saveLed0 = ledSettingState ();

		// Firmware XML Update
		if ((status = firmUpdateXml ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


//**********************************************************************************
//	XML Update Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmXmlUpdateHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The XML program is updated.\n");
	DEBUG_PRINT_FORCE ("  Command           : update-xml\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	XML Download
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmXmlDownload (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed0 = -1;
	int waitTimeS, size;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFirmXmlDownloadHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// プログラム設定ステート
		saveLed0 = ledSettingState ();

		// Download Size
		if (sscanf (gCmdArg[1], "%d", &size) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Wait Time（秒単位）
		if (sscanf (gCmdArg[2], "%d", &waitTimeS) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Wait Time Parameter
		if (waitTimeS > DOWNLOAD_WAIT_TIME_MAX)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "XML Data Download Wait Time(%d) Parameter Error. (Min:0 / Max:%d)\n", waitTimeS, DOWNLOAD_WAIT_TIME_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Wait
		sDelay (waitTimeS);

		// Firmware XML Download
		if ((status = firmDownloadXml (size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


//**********************************************************************************
//	XML Update Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmXmlDownloadHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The XML is download.\n");
	DEBUG_PRINT_FORCE ("  Command           : download-xmp [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input0 Param      : XML File Size(Dec)\n");
	DEBUG_PRINT_FORCE ("  Input1 Param      : Start Wait Time Secound(Min:0 - Max:%d)\n", DOWNLOAD_WAIT_TIME_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Firmware All Download(全領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmDownloadAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int waitTimeS;
	int saveLed0 = -1;
	unsigned int size;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFirmDownloadAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// プログラム設定ステート
		saveLed0 = ledSettingState ();

		// Download Size
		if (sscanf (gCmdArg[1], "%d", &size) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Wait Time（秒単位）
		if (sscanf (gCmdArg[2], "%d", &waitTimeS) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Wait Time Parameter
		if (waitTimeS > DOWNLOAD_WAIT_TIME_MAX)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Program Download Wait Time(%d) Parameter Error. (Min=0 / Max=%d)\n", waitTimeS, DOWNLOAD_WAIT_TIME_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Wait
		sDelay (waitTimeS);

		// DPC Data Download
		if ((status = firmDownloadAll (size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


//**********************************************************************************
//	Firmware All Update(全領域)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmDownloadAllHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : All data is download.\n");
	DEBUG_PRINT_FORCE ("  Command           : download-all [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Download Size(Dec)(Min:1 - Max:%d)\n", FIRM_DOWNLOAD_ALL_SIZE);
	DEBUG_PRINT_FORCE ("  Input  Param1     : Start Wait Time Secound(Min:0 - Max:%d)\n", DOWNLOAD_WAIT_TIME_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


#if defined (MODE_UPDATE_INTERFACE)
//**********************************************************************************
//	Interface FPGA Update
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFpgaUpdateInterface (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed0 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFpgaUpdateInterfaceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// プログラム設定ステート
		saveLed0 = ledSettingState ();

		// Update
#if defined (MODE_FPGA_PF)
		if ((status = fpgaUpdatePolarFire ()) != AVAL_STATUS_SUCCESS)
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
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


//**********************************************************************************
//	Interface FPGA Update Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFpgaUpdateInterfaceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Interface FPGA data is updated.\n");
	DEBUG_PRINT_FORCE ("  Command           : update-if\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Interface FPGA Update (Administrator)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFpgaUpdateInterfaceAdmin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed0 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFpgaUpdateInterfaceAdmin (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// プログラム設定ステート
		saveLed0 = ledSettingState ();

		// Update
#if defined (MODE_FPGA_PF)
		if ((status = fpgaUpdatePolarFireAdmin ()) != AVAL_STATUS_SUCCESS)
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
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


//**********************************************************************************
//	Interface FPGA Update Help (Administrator)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFpgaUpdateInterfaceHelpAdmin (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Interface FPGA data is updated.\n");
	DEBUG_PRINT_FORCE ("  Command           : update-if-admin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Interface FPGA Download
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFpgaDownloadInterface (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed0 = -1;
	int waitTimeS;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFpgaDownloadInterfaceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// プログラム設定ステート
		saveLed0 = ledSettingState ();

		// Wait Time（秒単位）
		if (sscanf (gCmdArg[1], "%d", &waitTimeS) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Wait Time Parameter
		if (waitTimeS > DOWNLOAD_WAIT_TIME_MAX)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Interface FPGA Data Download Wait Time(%d) Parameter Error. (Min:0 / Max:%d)\n", waitTimeS, DOWNLOAD_WAIT_TIME_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Wait
		sDelay (waitTimeS);

		// Download
#if defined (MODE_FPGA_PF)
		if ((status = fpgaDownloadPolarFire ()) != AVAL_STATUS_SUCCESS)
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
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


//**********************************************************************************
//	Interface FPGA Download Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFpgaDownloadInterfaceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Interface FPGA data is download.\n");
	DEBUG_PRINT_FORCE ("  Command           : download-if\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Start Wait Time Secound(Min:0 - Max:%d) \n", DOWNLOAD_WAIT_TIME_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Interface FPGA Copy
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFpgaCopyInterface (void *str)
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
			cmdFpgaCopyInterfaceHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
#if defined (MODE_FPGA_PF)
		if ((status = ifFpgaCopyUserToGolden ()) != AVAL_STATUS_SUCCESS)
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
//	Interface FPGA Copy Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFpgaCopyInterfaceHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Interface FPGA data is copy.\n");
	DEBUG_PRINT_FORCE ("  Command           : iffpgacopy\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

#endif // #if defined (MODE_UPDATE_INTERFACE)


//**********************************************************************************
//	Firmware All Update(全領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmUpdateAllAdmin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed0 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFirmUpdateAllAdminHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// プログラム設定ステート
		saveLed0 = ledSettingState ();

		// Firmware All Update
		if ((status = firmUpdateAllAdmin ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


//**********************************************************************************
//	Firmware All Update(全領域)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFirmUpdateAllAdminHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : All data is updated.\n");
	DEBUG_PRINT_FORCE ("  Command           : update-all-admin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


#if defined (MODE_GIGE_10G) && defined(IF_GIGE)
//**********************************************************************************
//	Update Phy
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyUpdate (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed0 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdPhyUpdateHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// プログラム設定ステート
		saveLed0 = ledSettingState ();

		// Phy Data Update
		if ((status = firmUpdatePhy ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);
	return (status);
}


//**********************************************************************************
//	Update Phy Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyUpdateHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Phy data is updated.\n");
	DEBUG_PRINT_FORCE ("  Command           : update-gigephy\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Phy Data Copy
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyDataCopy (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;

	if ((gInterFaceID != INTERFACE_GIGE) && (gInterFaceID != INTERFACE_GIGE20))
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
			cmdPhyDataCopyHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Phy Data Backup
		if ((status = phyDataCopy ()) != AVAL_STATUS_SUCCESS)
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
//	Phy Data Copy Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdPhyDataCopyHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Phy data is copy.\n");
	DEBUG_PRINT_FORCE ("  Command           : phydatacopy\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

#endif // #if defined (MODE_GIGE_10G) && defined(IF_GIGE)


//**********************************************************************************
//	Boot Header
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdBootHeader (void *str)
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
			cmdBootHeaderHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Boot Header Create
		if ((status = fpgaUpdatePolarFireHeader ()) != AVAL_STATUS_SUCCESS)
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
//	Boot Header Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdBootHeaderHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Boot Header is created.\n");
	DEBUG_PRINT_FORCE ("  Command           : bootheader\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
