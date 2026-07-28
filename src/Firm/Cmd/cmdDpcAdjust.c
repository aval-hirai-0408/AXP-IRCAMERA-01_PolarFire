//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdDpcAdjust.c - DPC Adjust Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_DPC)
//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int gDPCLoopFfc;			// DPC FFC Loopカウント


//**********************************************************************************
//	DPC Adjust Info
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustInfo (void *str)
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
			cmdDpcAdjustInfoHelp (NULL);
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

		// DPC Adjust情報表示
		if ((status = dpcAdjustInfo (dpcNum, DPC_USER)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Adjust Info Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustInfoHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjust-info\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number (Min:%d - Max:%d)\n", DPC_NUMBER_MIN, DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Adjust Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Map Info
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcMapInfo (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum;
	unsigned int memAdrs, size, dmaWait;
	int mode;
	unsigned int dpcCount;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcMapInfoHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 欠陥画素補正数設定
		dpcCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);

		// 欠陥補正あり
		if (dpcCount == 0)
		{
			DEBUG_PRINT_FORCE ("None\n");
			goto _DONE;
		}

		// Get DPC parameter
		if ((status = dpcGetParam (&memAdrs, &size, &dmaWait, &mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

#if defined (MODE_DPC_GRID_UPDATE)
		// DPC Map Mode設定(0:通常バッファ/1:別バッファにmap情報格納)
		dpcSetMapInfoMode (MODE_DISABLE);
#endif

		// DPC情報表示
		if ((status = dpcMapInfoShow ((unsigned char *)memAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
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

		// DPC欠陥座標表示
		if ((status = dpcMapInfoFlash (dpcNum, DPC_USER)) != AVAL_STATUS_SUCCESS)
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
//	DPC Map Info Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcMapInfoHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Map Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcmapinfo\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number (Min:%d - Max:%d)\n", DPC_NUMBER_MIN, DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Map Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Map Info Admin
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcMapInfoAdmin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum;
	unsigned int memAdrs, size, dmaWait;
	int mode;
	unsigned int dpcCount;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcMapInfoAdminHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 欠陥画素補正数設定
		dpcCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);

		// 欠陥補正あり
		if (dpcCount == 0)
		{
			DEBUG_PRINT_FORCE ("None\n");
			goto _DONE;
		}

		// Get DPC parameter
		if ((status = dpcGetParam (&memAdrs, &size, &dmaWait, &mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC情報表示
		if ((status = dpcMapInfoShow ((unsigned char *)memAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
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

		// DPC欠陥座標表示
		if ((status = dpcMapInfoFlash (dpcNum, DPC_ADMIN)) != AVAL_STATUS_SUCCESS)
			return (status);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Map Info Admin Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcMapInfoAdminHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Map Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcmapinfo-admin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number (Min:%d - Max:%d)\n", DPC_NUMBER_MIN, DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Map Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Map Info(欠陥補正付加座標の表示)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcMapInfoImpossible (void *str)
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
			cmdDpcMapInfoImpossibleHelp (NULL);
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

		// DPC補正不可座標表示
		if ((status = dpcImpossibleGridInfo (dpcNum, DPC_USER)) != AVAL_STATUS_SUCCESS)
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
//	DPC Map Info Help(欠陥補正付加座標の表示)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcMapInfoImpossibleHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Impossible Map Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcmapinfoimp\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number (Min:%d - Max:%d)\n", DPC_NUMBER_MIN, DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Impossible Map Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Map Info(欠陥補正付加座標の表示)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcMapInfoImpossibleAdmin (void *str)
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
			cmdDpcMapInfoImpossibleAdminHelp (NULL);
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

		// DPC補正不可座標表示
		if ((status = dpcImpossibleGridInfo (dpcNum, DPC_ADMIN)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Map Info Help(欠陥補正付加座標の表示)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcMapInfoImpossibleAdminHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Impossible Map Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcmapinfoimp-admin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number (Min:%d - Max:%d)\n", DPC_NUMBER_MIN, DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Impossible Map Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Pixel Number
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcPixelNum (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	//int dpcNum;
	//unsigned int mark;
	unsigned int number;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcPixelNumHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 欠陥画素補正数設定
		#if defined (MODE_DPC_GRID_UPDATE)

		// 欠陥座標数取得
		if ((status = dpcGetDefectionCountVersion2 ((int *)&number)) !=  AVAL_STATUS_SUCCESS)
			goto _DONE;

		#else // #if defined (MODE_DPC_GRID_UPDATE)

		int mode;

		// 設定
		if ((status = roiGetAreaFlag (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (mode == ROI_AREA_MODE_FULL_SIZE)
		{
			if ((status = dpcEndSearch (DPC_MEMORY_ADRS, (int *)&number)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		else
		{
			if ((status = dpcEndSearch2 (DPC_MEMORY_ADRS, (int *)&number)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		#endif // #if defined (MODE_DPC_GRID_UPDATE)

		DEBUG_PRINT_FORCE ("%d ", number);
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
//	DPC Pixel Number Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcPixelNumHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Pixel Number Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcpixel [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number (Min:%d - Max:%d)\n", DPC_NUMBER_MIN, DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Pixel Number\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Pixel Number
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcPixelNumAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int number;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcPixelNumAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 欠陥画素補正数取得
		if((status = dpcGetDefectionCount (&number)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", number);
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
//	DPC Pixel Number Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcPixelNumAllHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Pixel Number Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcpixelall [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number (Min:%d - Max:%d)\n", DPC_NUMBER_MIN, DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Pixel Number\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Pixel Number
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcPixelNumAdmin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum;
	unsigned int number, mark;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcPixelNumAdminHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = dpcEndSearch2 (DPC_MEMORY_ADRS, (int *)&number)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", number);
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

		// DPCマーク取得
		if ((status = dpcGetMarkFlashAdmin (dpcNum, &mark)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC補正個数取得
		if ((mark == DPC_NO_DATA) || (mark == 0))
			number = 0;
		else
		{
			number = mark;
		}

		DEBUG_PRINT_FORCE ("%d ", number);
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
//	DPC Pixel Number Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcPixelNumAdminHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Pixel Number Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcpixel-admin [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number (Min:%d - Max:%d)\n", DPC_NUMBER_MIN, DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Pixel Number\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Pixel Number設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcSetPixelNum (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int number;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcSetPixelNumHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// DPC Pixel Number取得
		if (sscanf (gCmdArg[1], "%d", &number) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 欠陥画素補正数設定
		dpcSetDefectionCount (number);
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
//	DPC Pixel Number設定 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcSetPixelNumHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Pixel Number is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcpixel-set [param]\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Pixel Number\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Pixel Max Number
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcPixelMaxNum (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int number;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcPixelMaxNumHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// DPC NUmber
		if ((status = dpcGetPixelMaxNum (&number)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", number);
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
//	DPC Pixel Number Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcPixelMaxNumHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Pixel Max Number Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcmaxpixel\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : None\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Pixel Max Number\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Adjust Info
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustInfoAdmin (void *str)
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
			cmdDpcAdjustInfoAdminHelp (NULL);
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

		// DPC Adjust情報表示
		if ((status = dpcAdjustInfo (dpcNum, DPC_ADMIN)) != AVAL_STATUS_SUCCESS)
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
//	DPC Adjust Info Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustInfoAdminHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjust-info-admin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : DPC Number (Min:%d - Max:%d)\n", DPC_NUMBER_MIN, DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Adjust Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPCコマンド(自動バージョン)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustBatch1 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum, ffcNum;
	DPC_PARAM dpcParam;
	int dpcMode;
	
	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustBatch1Help (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// DPC番号間取得
		if (sscanf (gCmdArg[1], "%d", &dpcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC番号間取得
		if (sscanf (gCmdArg[2], "%d", &ffcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		//--------------------------------------------------------------------------------
		// 画素欠陥補正
		//--------------------------------------------------------------------------------

		// クリア
		memset ((void *)&dpcParam, 0, sizeof(dpcParam));

		// パラメータ設定
		dpcParam.dpcNo = dpcNum;
		dpcParam.ffcNo = ffcNum;
		//@@@1dpcParam.mode = MODE_BATCH;
		dpcParam.frameRate = 0;
		dpcParam.exposure = 0;
		dpcParam.bit = 0;
		dpcParam.sensorTemp = 0;
		dpcParam.memType = DPC_MEMORY_EXT;
		dpcParam.userMode = DPC_USER;
		dpcParam.sd = -1;
		dpcParam.nonuniformity = -1;
		dpcParam.detailMode = 0;

		// DPC Adjust Mode取得
		if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC
		if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		{
			if ((status = dpcMainBatch1 (dpcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
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
//	欠陥検出調整成(自動バージョン)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustBatch1Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Defect image correction is adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustbatch1 [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : DPC Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : FFC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Number]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", DPC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPCコマンド(自動バージョン)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustBatch2 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum, ffcNum;
	DPC_PARAM dpcParam;
	int dpcMode;
	
	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustBatch2Help (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// DPC番号間取得
		if (sscanf (gCmdArg[1], "%d", &dpcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC番号間取得
		if (sscanf (gCmdArg[2], "%d", &ffcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		//--------------------------------------------------------------------------------
		// 画素欠陥補正
		//--------------------------------------------------------------------------------

		// クリア
		memset ((void *)&dpcParam, 0, sizeof(dpcParam));

		// パラメータ設定
		dpcParam.dpcNo = dpcNum;
		dpcParam.ffcNo = ffcNum;
		//@@@1dpcParam.mode = MODE_BATCH;
		dpcParam.frameRate = 0;
		dpcParam.exposure = 0;
		dpcParam.bit = 0;
		dpcParam.sensorTemp = 0;
		dpcParam.memType = DPC_MEMORY_EXT;
		dpcParam.userMode = DPC_USER;
		dpcParam.sd = -1;
		dpcParam.nonuniformity = -1;
		dpcParam.detailMode = 0;

		// DPC Adjust Mode取得
		if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC
		if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		{
			if ((status = dpcMainBatch2 (dpcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
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
//	欠陥検出調整成(自動バージョン)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustBatch2Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Defect image correction is adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustbatch2 [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : DPC Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : FFC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Number]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", DPC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPCコマンド3(自動バージョン)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustBatch3 (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum;
	DPC_PARAM dpcParam;
	int dpcMode;
	
	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustBatch3Help (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// DPC番号間取得
		if (sscanf (gCmdArg[1], "%d", &dpcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		//--------------------------------------------------------------------------------
		// 画素欠陥補正
		//--------------------------------------------------------------------------------

		// クリア
		memset ((void *)&dpcParam, 0, sizeof(dpcParam));

		// パラメータ設定
		dpcParam.dpcNo = dpcNum;
		dpcParam.ffcNo = 0;
		//@@@1dpcParam.mode = MODE_BATCH;
		dpcParam.frameRate = 0;
		dpcParam.exposure = 0;
		dpcParam.bit = 0;
		dpcParam.sensorTemp = 0;
		dpcParam.memType = DPC_MEMORY_EXT;
		dpcParam.userMode = DPC_USER;
		dpcParam.sd = -1;
		dpcParam.nonuniformity = -1;
		dpcParam.detailMode = 0;

		// DPC Adjust Mode取得
		if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC
		if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		{
			if ((status = dpcMainBatch3 (dpcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
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
//	欠陥検出調整成(自動バージョン)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustBatch3Help (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Defect image correction is adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustbatch3 [Param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : DPC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Number]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", DPC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPCコマンド(自動バージョン)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustBatch2Detail (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int dpcNum, ffcNum;
	DPC_PARAM dpcParam;
	double sd, nonuniformity;
	int dpcMode;
	
	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustBatch2DetailHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 5)
	{
		// DPC番号間取得
		if (sscanf (gCmdArg[1], "%d", &dpcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC番号間取得
		if (sscanf (gCmdArg[2], "%d", &ffcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 標準偏差取得
		if (sscanf (gCmdArg[3], "%lf", &sd) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 不均一輝度取得
		if (sscanf (gCmdArg[4], "%lf", &nonuniformity) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		//--------------------------------------------------------------------------------
		// 画素欠陥補正
		//--------------------------------------------------------------------------------

		// クリア
		memset ((void *)&dpcParam, 0, sizeof(dpcParam));

		// パラメータ設定
		dpcParam.dpcNo = dpcNum;
		dpcParam.ffcNo = ffcNum;
		//@@@1dpcParam.mode = MODE_BATCH;
		dpcParam.frameRate = 0;
		dpcParam.exposure = 0;
		dpcParam.bit = 0;
		dpcParam.sensorTemp = 0;
		dpcParam.memType = DPC_MEMORY_EXT;
		dpcParam.userMode = DPC_USER;
		dpcParam.sd = sd;
		dpcParam.nonuniformity = nonuniformity;
		dpcParam.detailMode = 1;

		// DPC Adjust Mode取得
		if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC
		if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		{
			if ((status = dpcMainBatch2 (dpcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
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
//	欠陥検出調整成(自動バージョン)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustBatch2DetailHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Defect image correction is adjusted.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustbatch2detail [Param0] [Param1] [Param2] [Param3]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : DPC Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : FFC Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : standard deviation\n");
	DEBUG_PRINT_FORCE ("  Input  Param3     : nonuniformity\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Number]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", DPC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", DPC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Defect Abort
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAbort (void *str)
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
			cmdDpcAbortHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// カウンタクリア
		dpcClearBatchCount ();
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
//	DPC Abort Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAbortHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC is aborted.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcabort\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Defect Counter Decrement
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcCounterDecrement (void *str)
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
			cmdDpcCounterDecrementHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// カウンタデクリメント
		dpcDecrementCount ();
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
//	DPC Defect Counter Decrement Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcCounterDecrementHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC counter is decrement.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcdec\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Defect Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcStateCount (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int count;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcStateCountHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// DPC Count取得
		if ((status = dpcGetBatchCountGlobal (&count)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", count);
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
//	DPC Defect Counter Decrement Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcStateCountHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC state count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcstatecount\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC State Count\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Adjust Rate
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustRate (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int index;
	float rate;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustRateHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust Rate取得
		if ((status = dpcGetAdjustRate (index, &rate)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", rate);
	}
	else if (argc == 3)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Rate取得
		if (sscanf (gCmdArg[2], "%f", &rate) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust Rate設定
		if ((status = dpcSetAdjustRate (index, rate)) != AVAL_STATUS_SUCCESS)
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
//	DPC Adjust Rate Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustRateHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust Rate is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustrate [Param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Adjust Rate\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust Rate is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustrate [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : DPC Adjust Rate\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[DPC Index]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", FFC_NUMBER_MAX);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Rate]\n");
	DEBUG_PRINT_FORCE ("  Min : %.2f\n", FRAMERATE_MIN);
	DEBUG_PRINT_FORCE ("  Max : %.2f\n", FRAMERATE_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Adjust Exposure
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustExp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int index;
	int exp;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustExpHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust Exposure取得
		if ((status = dpcGetAdjustExposure (index, &exp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", exp);
	}
	else if (argc == 3)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Exposure取得
		if (sscanf (gCmdArg[2], "%d", &exp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust Exposure設定
		if ((status = dpcSetAdjustExposure (index, exp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__,  status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust Exposure Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustExpHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust Exposure Time is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustexp [Param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Adjust Exposure Time\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust Exposure Time is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustexp [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Input Param2      : DPC Adjust Exposure Time\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[DPC Index]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", FFC_NUMBER_MAX);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Exposure Time]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", MIN_EXPOSURE_TIME);
	DEBUG_PRINT_FORCE ("  Max : %d\n", MAX_EXPOSURE_TIME);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Adjust FFC Number
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustFfcNumber (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int index;
	int ffcNumber;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustFfcNumberHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust FFC Number取得
		if ((status = dpcGetAdjustFfcNumber (index, &ffcNumber)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", ffcNumber);
	}
	else if (argc == 3)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC Number取得
		if (sscanf (gCmdArg[2], "%d", &ffcNumber) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust FFC Number設定
		if ((status = dpcSetAdjustFfcNumber (index, ffcNumber)) != AVAL_STATUS_SUCCESS)
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
//	DPC Adjust FFC Number Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustFfcNumberHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust FFC Number is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustffcnumber [Param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Adjust Exposure Time\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust FFC Number is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustffcnumber [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : DPC Adjust Exposure Time\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Index]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", FFC_NUMBER_MAX);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Adjust Temperature
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustTemp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int index;
	float temp;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustTempHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust Temp取得
		if ((status = dpcGetAdjustTemp (index, &temp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", temp);
	}
	else if (argc == 3)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Temp取得
		if (sscanf (gCmdArg[2], "%f", &temp) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust Temp設定
		if ((status = dpcSetAdjustTemp (index, temp)) != AVAL_STATUS_SUCCESS)
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
//	DPC Adjust Temperature Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustTempHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust temperature is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjusttemp [Param0]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Output Param      : DPC Adjust Temperature\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust temperature is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjusttemp [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : DPC Adjust Temperature\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Index]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", FFC_NUMBER_MAX);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Temp]\n");
	DEBUG_PRINT_FORCE ("  Min : %.2f\n", PELTIER_SENSOR_ASJUST_TEMP_MIN);
	DEBUG_PRINT_FORCE ("  Max : %.2f\n", PELTIER_SENSOR_ASJUST_TEMP_MAX);

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Adjust標準偏差
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustSd (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int index, subIndex;
	float sd;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustSdHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Sub Index取得
		if (sscanf (gCmdArg[2], "%d", &subIndex) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust標準偏差取得
		if ((status = dpcGetAdjustSd (index, subIndex, &sd)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", sd);
	}
	else if (argc == 4)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Sub Index取得
		if (sscanf (gCmdArg[2], "%d", &subIndex) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 標準偏差取得
		if (sscanf (gCmdArg[3], "%f", &sd) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust標準偏差設定
		if ((status = dpcSetAdjustSd (index, subIndex, sd)) != AVAL_STATUS_SUCCESS)
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
//	DPC Adjust標準偏差Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustSdHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust standard deviation is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustsd [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : DPC Sub Index\n");
	DEBUG_PRINT_FORCE ("  Output Param      : standard deviation\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust standard deviation is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustsd [Param0] [Param1] [Param2]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : DPC Sub Index\n");
	DEBUG_PRINT_FORCE ("  Input Param2      : standard deviation\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Index]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", FFC_NUMBER_MAX);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Sub Index]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", DPC_BRIGHTNESS_LOOP_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", DPC_BRIGHTNESS_LOOP_MAX);

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Adjust感度不均一
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustNonUniform (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int index, subIndex;
	float nonuniform;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDpcAdjustNonUniformHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Sub Index取得
		if (sscanf (gCmdArg[2], "%d", &subIndex) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Adjust感度不均一取得
		if ((status = dpcGetAdjustNonUniform (index, subIndex, &nonuniform)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.2f ", nonuniform);
	}
	else if (argc == 4)
	{
		// DPC Index取得
		if (sscanf (gCmdArg[1], "%d", &index) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// DPC Sub Index取得
		if (sscanf (gCmdArg[2], "%d", &subIndex) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 感度不均一取得
		if (sscanf (gCmdArg[3], "%f", &nonuniform) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}
		
		// DPC Adjust感度不均一設定
		if ((status = dpcSetAdjustNonUniform (index, subIndex, nonuniform)) != AVAL_STATUS_SUCCESS)
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
//	DPC Adjust感度不均一Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDpcAdjustNonUniformHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust nonuniformity is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustnonuniform [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : DPC Sub Index\n");
	DEBUG_PRINT_FORCE ("  Output Param      : nonuniformity\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : DPC Adjust nonuniformity is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : dpcadjustnonuniform [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input Param0      : DPC Index\n");
	DEBUG_PRINT_FORCE ("  Input Param1      : DPC Sub Index\n");
	DEBUG_PRINT_FORCE ("  Input Param2      : nonuniformity\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Index]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", FFC_NUMBER_MAX);

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[DPC Sub Index]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", DPC_BRIGHTNESS_LOOP_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", DPC_BRIGHTNESS_LOOP_MAX);

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

#endif // #if defined (MODE_DPC)

// eof
