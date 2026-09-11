//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdDpc.c - DPC Programm
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	DPC情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int dpc (void)
{
	int status;

	status = cmdDpcShow (NULL);
	return (status);	
}


//**********************************************************************************
//	DPC Show
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcShow (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int memAdrs, size, dmaWait;
	int mode;
	unsigned int flashAdrs, flashData;
	int dpcNum;

	// Get DPC parameter
	if ((status = dpcGetParam (&memAdrs, &size, &dmaWait, &mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Flash Address
	if ((status = cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_DPC_FLASH_ADRS, &flashAdrs, &flashData)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get DPC Number
	if ((status = dpcGetLoadNum (&dpcNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------
	// DPC情報
	//--------------------------------------------------
	DEBUG_PRINT_FORCE ("\n[DPC informations]\n");

	// メモリアドレス
	DEBUG_PRINT_FORCE ("   DPC Number                ");
	DEBUG_PRINT_FORCE ("%d\n", dpcNum);

	DEBUG_PRINT_FORCE ("   Address                   ");
	DEBUG_PRINT_FORCE ("0x%x\n", memAdrs);

	DEBUG_PRINT_FORCE ("   DPC Mode                  ");
	if (mode == DPC_ENABLE)
		DEBUG_PRINT_FORCE ("%d(Enable)\n", mode);
	else
		DEBUG_PRINT_FORCE ("%d(Disable)\n", mode);

	DEBUG_PRINT_FORCE ("   Flash Adrs                ");
	DEBUG_PRINT_FORCE ("0x%x\n", flashAdrs);

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Enable/Disable
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpc (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// DPC取得
		if ((status = dpcGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// DPC Mode
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC設定
		if ((status = dpcSetMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Enable/Disable Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The DPC Enable/Disable Mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : defect-correction/dpc\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The DPC Enable/Disable Mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : defect-correction/dpc [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Disable\n", DPC_DISABLE);
	DEBUG_PRINT_FORCE ("  %d : Enable\n\n", DPC_ENABLE);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Adjust Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustMode (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// DPCモード取得
		if ((status = dpcGetAdjustMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPCモード設定
		if ((status = dpcSetAdjustMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The DPC adjust mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustmode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Adjust Mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The DPC adjust mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustmode [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Adjust Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[DPC adjust Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : DPC Normal Adjust Mode\n", DPC_ADJUST_MODE_NOMAL);
	DEBUG_PRINT_FORCE ("  %d : DPC Neighborhood Adjust Mode(FFC Disable)\n", DPC_ADJUST_MODE_NE);
	DEBUG_PRINT_FORCE ("  %d : DPC Neighborhood Adjust Mode(FFC Enable)\n", DPC_ADJUST_MODE_NE_FFC_ENABLE);
	DEBUG_PRINT_FORCE ("  %d : DPC Line Adjust Mode(FFC Disable)\n", DPC_ADJUST_MODE_LINE);
	DEBUG_PRINT_FORCE ("  %d : DPC Line Adjust Mode(FFC Enable)\n", DPC_ADJUST_MODE_LINE_FFC_ENABLE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPCデータロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		loop					：未使用
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcLoad (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcLoadHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get DPC Number
		if ((status = dpcGetLoadNum (&dpcNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	
		DEBUG_PRINT_FORCE ("%d ", dpcNum);
	}
	else if (argc == 2)
	{
		// DPC Number取得
		if (sscanf (gCmdArg[1], "%d", &dpcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPCデータロード
		if ((status = dpcLoad (dpcNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPCデータロードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcLoadHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The DPC number is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcload\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Number\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Data of the designated DPC number is loaded.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcload\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", DPC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n\n", DPC_NUMBER_MAX);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPCデータロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		loop					：未使用
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcLoadAdmin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int dpcNum;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcLoadAdminHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// DPC Number取得
		if (sscanf (gCmdArg[1], "%d", &dpcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPCデータロード
		if ((status = dpcLoadAdmin (dpcNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPCデータロードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcLoadAdminHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The DPC number is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcload-admin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Number\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Data of the designated DPC number is loaded.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcload-admin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", DPC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n\n", DPC_NUMBER_MAX);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC保存(メモリにあるイメージをそのままFlashへ保存)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcSave (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum;
	int dpcLoadNum;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcSaveHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// DPC番号
		if (sscanf (gCmdArg[1], "%d", &dpcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Get DPC Number
		if ((status = dpcGetLoadNum (&dpcLoadNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC Save
		if ((status = dpcSave (dpcNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC保存Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcSaveHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Data is saved.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcsave [Param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", DPC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n\n", DPC_NUMBER_MAX);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC保存(メモリにあるイメージをそのままFlashへ保存)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcSaveAdmin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum;
	int dpcLoadNum;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcSaveAdminHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// DPC番号
		if (sscanf (gCmdArg[1], "%d", &dpcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Get DPC Number
		if ((status = dpcGetLoadNum (&dpcLoadNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC Save
		if ((status = dpcSaveAdmin (dpcNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC保存Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcSaveAdminHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Data is saved.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcsave-admin [Param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", DPC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n\n", DPC_NUMBER_MAX);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPCデータアップロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDataUpload (void *str)
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
			cmdDpcDataUploadHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// DPC Data Update
		if ((status = dpcDataUpload ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPCデータアップロードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDataUploadHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Data is upload.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpc-upload\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPCデータダウンロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDataDownload (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int waitTimeS;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcDataDownloadHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Wait Time（秒単位）
		if (sscanf (gCmdArg[1], "%d", &waitTimeS) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Wait Time Parameter
		if (waitTimeS > DOWNLOAD_WAIT_TIME_MAX)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "DPC Download Wait Time(%d) Parameter Error.\n", waitTimeS);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Wait
		sDelay (waitTimeS);

		// DPC Data Download
		if ((status = dpcDataDownload (UPDATE_UNCOMPRESS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPCデータダウンロードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDataDownloadHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Data is download.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpc-download [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Start Wait Time Secound(Min:0 - Max:%d)\n", DOWNLOAD_WAIT_TIME_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPCデータダウンロード(圧縮版)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDataDownloadCompress (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int waitTimeS;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcDataDownloadCompressHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Wait Time（秒単位）
		if (sscanf (gCmdArg[1], "%d", &waitTimeS) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Wait Time Parameter
		if (waitTimeS > DOWNLOAD_WAIT_TIME_MAX)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "DPC Download Wait Time(%d) Parameter Error.\n", waitTimeS);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Wait
		sDelay (waitTimeS);

		// DPC Data Download
		if ((status = dpcDataDownload (UPDATE_COMPRESS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPCデータダウンロードHelp(圧縮版)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDataDownloadCompressHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Data is download.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpc-download-compress [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Start Wait Time Secound(Min:0 - Max:%d)\n", DOWNLOAD_WAIT_TIME_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Default All
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDefault (void *str)
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
			cmdDpcDefaultHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// DPC Default
		if ((status = dpcDefault ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Default Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDefaultHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Data is default.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcdefault\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Grid Set
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAddGrid (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int x, y;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAddGridHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// x座標取得
		if (sscanf (gCmdArg[1], "%d", &x) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// ｙ座標取得
		if (sscanf (gCmdArg[2], "%d", &y) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Add Grid
		if ((status = dpcAddGrid (x, y)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Grid Add Help(Ver.2.0)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAddGridHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC grid is add.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcgrid-add [Pram0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : X Grid\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Y Grid\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[X Grid]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", WidthMax()-1);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Y Grid]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", HeightMax()-1);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Grid Delete
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDeleteGrid (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int x, y;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcDeleteGridHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// x座標取得
		if (sscanf (gCmdArg[1], "%d", &x) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// ｙ座標取得
		if (sscanf (gCmdArg[2], "%d", &y) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Delete Grid
		if ((status = dpcDeleteGrid (x, y)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Grid Delete Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDeleteGridHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC grid is delete.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcgrid-delete [Pram0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : X Grid\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Y Grid\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[X Grid]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", WidthMax()-1);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Y Grid]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", HeightMax()-1);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Grid Clear
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcClearGrid (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum = 0;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcClearGridHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// DPC Clear Grid
		if ((status = dpcClearGrid (dpcNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Grid Clear Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcClearGridHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC grid is clear.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcgrid-clear\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Get DPC Grid
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcGetGrid (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int index;
	int x, y;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcGetGridHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Get DPC Grid
		if ((status = dpcGetMapInfoIndex2 (index, &x, &y)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", x);
		DEBUG_PRINT_FORCE ("%d ", y);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Get DPC Grid Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcGetGridHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int dpcCount;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The DPC Grid is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcgrid-get [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : index\n");
	DEBUG_PRINT_FORCE ("  Output0 Param     : DPC X Grid\n");
	DEBUG_PRINT_FORCE ("  Output1 Param     : DPC Y Grid\n");
	DEBUG_PRINT_FORCE ("\n");

	if ((status = dpcGetDefectionCount (&dpcCount)) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("[Index]\n");

		// 欠陥なし
		if (dpcCount == 0)
		{
			DEBUG_PRINT_FORCE ("  Min : -\n");
			DEBUG_PRINT_FORCE ("  Max : -\n");
		}
		else
		{
			DEBUG_PRINT_FORCE ("  Min : 0\n");
			DEBUG_PRINT_FORCE ("  Max : %d\n", dpcCount-1);
		}
	}

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC X Grid]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", WidthMax()-1);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Y Grid]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", HeightMax()-1);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Grid Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcGridCheck (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int x, y;
	int detect;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcGridCheckHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// x座標取得
		if (sscanf (gCmdArg[1], "%d", &x) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// y座標取得
		if (sscanf (gCmdArg[2], "%d", &y) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Get DPC Grid
		if ((status = dpcGridCheck (x, y, &detect)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", detect);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Grid Check Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcGridCheckHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The DPC Grid is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcgridcheck [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input0 Param      : DPC X Grid\n");
	DEBUG_PRINT_FORCE ("  Input1 Param      : DPC Y Grid\n");
	DEBUG_PRINT_FORCE ("  Output  Param     : DPC Grid Detect\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[DPC X Grid]\n");
	DEBUG_PRINT_FORCE ("  0  : X Min\n");;
	DEBUG_PRINT_FORCE ("  %d : X Max\n", WidthMax());
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[DPC Y Grid]\n");
	DEBUG_PRINT_FORCE ("  0  : Y Min\n");;
	DEBUG_PRINT_FORCE ("  %d : Y Max\n", HeightMax());
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[DPC Grid Detect]\n");
	DEBUG_PRINT_FORCE ("  0 : Normal Pixel\n");;
	DEBUG_PRINT_FORCE ("  1 : Defective Pixel\n");;
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Grid Set
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAddGridLine (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int x;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAddGridLineHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// x座標取得
		if (sscanf (gCmdArg[1], "%d", &x) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Add Grid Line
		if ((status = dpcAddGridLine (x)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Grid Add Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAddGridLineHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC line is add.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcgrid-line-add [Pram]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : X Grid\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[X Grid]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", WidthMax()-1);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Grid Line Delete
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDeleteGridLine (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int x;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcDeleteGridLineHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// x座標取得
		if (sscanf (gCmdArg[1], "%d", &x) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Add Grid Line
		if ((status = dpcDeleteGridLine (x)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Grid Delete Line Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcDeleteGridLineHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC grid is delete.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcgrid-line-delete [Pram]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : X Grid\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[X Grid]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n", WidthMax()-1);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof

