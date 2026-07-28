//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// cmdFfc.c - FFC Programm
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int ffcAdjustAllFlag;					// FFC All Mode


//**********************************************************************************
//	IPU情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIpu (void *str)
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
			cmdIpuHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// FFC
#if defined (MODE_FFC)
		ffc ();
#endif

		// DPC
#if defined (MODE_DPC)
		dpc ();
#endif

		// LUT
#if defined (MODE_LUT)
		lut ();
#endif

		// Digital Offset Gain
#if defined (MODE_DIGITAL_OFFSET_GAIN)
		dog ();
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
//	IPU Show
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdIpuHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : IPU Information Display.\n");
	DEBUG_PRINT_FORCE ("  Command           : ipu\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : IPU Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int ffc (void)
{
	int status;
	
	status = cmdFfcShow (NULL);
	return (status);	
}


//**********************************************************************************
//	FFC Show
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcShow (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int memAdrs, size, dmaWait;
	int mode;
	unsigned int flashAdrs/*, flashData*/;
	unsigned int blackTarget;
	int ffcNum;
	int ffccormode;
	int calc;

	// Get FFC parameter
	if ((status = ffcGetParam (&memAdrs, &size, &dmaWait, &mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get FFC Number
	if ((status = ffcGetLoadNum (&ffcNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC Correction Mode取得
	if ((status = ffcGetCorMode (&ffccormode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FFC Flashアドレス取得
	if ((status = ffcGetFlashAdrs (ffcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Ge FFC Bit
	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------
	// FFC情報
	//--------------------------------------------------
	DEBUG_PRINT_FORCE ("\n[FFC informations]\n");

	// メモリアドレス
	DEBUG_PRINT_FORCE ("   FFC Number                ");
	DEBUG_PRINT_FORCE ("%d\n", ffcNum);

	// メモリアドレス
	DEBUG_PRINT_FORCE ("   Address                   ");
	DEBUG_PRINT_FORCE ("0x%x\n", memAdrs);

	// サイズ
	DEBUG_PRINT_FORCE ("   Size                      ");
	DEBUG_PRINT_FORCE ("0x%x\n", size);

	// DMA Wait
	DEBUG_PRINT_FORCE ("   DMA Wait                  ");
	DEBUG_PRINT_FORCE ("0x%x\n", dmaWait);

	// オフセットモード
	DEBUG_PRINT_FORCE ("   Offset Mode               ");

	if (mode & FPGA_FFC_CTRL_OFFSET_ENABLE_BIT)
		DEBUG_PRINT_FORCE ("1(Enable)\n");
	else
		DEBUG_PRINT_FORCE ("0(Disable)\n");

	// ゲインモード
	DEBUG_PRINT_FORCE ("   Gain Mode                 ");

	if (mode & FPGA_FFC_CTRL_GAIN_ENABLE_BIT)
		DEBUG_PRINT_FORCE ("1(Enable)\n");
	else
		DEBUG_PRINT_FORCE ("0(Disable)\n");

	// Flashアドレス
	DEBUG_PRINT_FORCE ("   Flash Adrs                ");
	DEBUG_PRINT_FORCE ("0x%x\n", flashAdrs);

	// 黒レベル目標値
	ffcGetBlackTarget (&blackTarget);
	DEBUG_PRINT_FORCE ("   Black Target              ");
	blackTarget /= calc;
	DEBUG_PRINT_FORCE ("%d\n", blackTarget);

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Enable/Disable
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcMode (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int offset, gain;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// FFC取得
		if ((status = ffcGetMode (&offset, &gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d %d ", offset, gain);
	}
	else if (argc == 3)
	{
		// offset mode
		if (sscanf (gCmdArg[1], "%d", &offset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// gain mode
		if (sscanf (gCmdArg[2], "%d", &gain) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC設定
		if ((status = ffcSetMode (offset, gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Enable/Disable Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC Enable/Disable Mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcmode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param0     : Offset Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : Gain Mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC Enable/Disable Mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcmode [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Offset Mode\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Gain Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Disable\n", FFC_DISABLE);
	DEBUG_PRINT_FORCE ("  %d : Enable\n",  FFC_ENABLE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Correction Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcCorMode (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;
	int ffcNum;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcCorModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check mode Parameter
		if ((mode < FFC_CORRECTION_MODE_MIN) || (mode > FFC_CORRECTION_MODE_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Correction mode(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, FFC_CORRECTION_MODE_MIN, FFC_CORRECTION_MODE_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// FFC番号取得
		if ((status = ffcGetLoadNum (&ffcNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC Correction Mode設定
		if ((status = ffcSetCorMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// FFC Load Main
		if ((status = ffcLoadMain (ffcNum, FFC_USER)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Correction Mode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcCorModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC correction mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffccormode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Correction Mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC correction mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffccormode [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Correction Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Correction Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Normal FFC Correction Mode\n", FFC_CORRECTION_MODE_FIRST);
#ifdef MODE_SPECTRUM
	DEBUG_PRINT_FORCE ("  %d : Shading Correction Mode\n", FFC_CORRECTION_MODE_SHADING_LINE);
#endif
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Internal Memory(Ver.1.5)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcRegInitIntMem (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum;
	int offset, gain;
	int data = 0;
	int startMode = 0;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcRegInitIntMemHelp (NULL);
			goto _DONE;
		}
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// 1次補正
	//--------------------------------------------------------------------------------
	if (argc == 4)
	{
		// FFC Number
		if (sscanf (gCmdArg[1], "%d", &ffcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// offset mode
		if (sscanf (gCmdArg[2], "%d", &offset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// gain mode
		if (sscanf (gCmdArg[3], "%d", &gain) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check offset Parameter
		if ((offset != FFC_ENABLE) && (offset != FFC_DISABLE))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "offset mode[%d] Parameter Error. (Disable:%d / Enable:%d)\n", offset, FFC_DISABLE, FFC_ENABLE);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Check gain Parameter
		if ((gain != FFC_ENABLE) && (gain != FFC_DISABLE))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "gain mode[%d] Parameter Error. (Disable:%d / Enable:%d)\n", gain, FFC_DISABLE, FFC_ENABLE);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Set Offset
		if (offset == FFC_ENABLE)
			data |= FPGA_FFC_CTRL_OFFSET_ENABLE_BIT;
		else if (offset == FFC_DISABLE)
			data &= ~FPGA_FFC_CTRL_OFFSET_ENABLE_BIT;

		// Set Gain
		if (gain == FFC_ENABLE)
			data |= FPGA_FFC_CTRL_GAIN_ENABLE_BIT;
		else if (gain == FFC_DISABLE)
			data &= ~FPGA_FFC_CTRL_GAIN_ENABLE_BIT;

		// FFCレジスタ初期化設定
		if ((status = ffcRegInitIntMem (ffcNum, data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		}
		else
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
			goto _DONE;
		}

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	FFC Enable/Disable Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcRegInitIntMemHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC Register Initialize.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcintmem [Param0] [Param1] [Param2]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : FFC Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Offset Mode\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : Gain Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n", FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Disable\n", FFC_DISABLE);
	DEBUG_PRINT_FORCE ("  %d : Enable\n",  FFC_ENABLE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCデータロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		loop					：未使用
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcLoad (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int ffcNum;
	
	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcLoadHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// FFC番号取得
		ffcNum = 0;

		// Get FFC Number
		if ((status = ffcGetLoadNum ((int *)&ffcNum)) != AVAL_STATUS_SUCCESS)
			return (status);

		DEBUG_PRINT_FORCE ("%d ", ffcNum);
	}
	else if (argc == 2)
	{
		// FFC Number取得
		if (sscanf (gCmdArg[1], "%d", &ffcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check ffcNum Parameter
		if ((ffcNum < FFC_NUMBER_MIN) || (ffcNum > FFC_NUMBER_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Load ffcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", ffcNum, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	
		// FFC Load Main
		if ((status = ffcLoadMain (ffcNum, FFC_USER)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータロードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcLoadHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC number is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffc\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Number\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Data of the designated FFC number is loaded.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffc\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n", FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCデータロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		loop					：未使用
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcLoadAdmin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int ffcNum;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcLoadAdminHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFC Number取得
		if (sscanf (gCmdArg[1], "%d", &ffcNum) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// FFC Load Main
		if ((status = ffcLoadMain (ffcNum, FFC_ADMIN)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータロードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcLoadAdminHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC number is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffc-admin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Number\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Data of the designated FFC number is loaded.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffc-admin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", FFC_NUMBER_ADMIN_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n",  FFC_NUMBER_ADMIN_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Data Saveコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcSave (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int ffcNo;
	FFC_PARAM ffcParam;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcSaveHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFC番号
		if (sscanf (gCmdArg[1], "%d", &ffcNo) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Factory番号かのCheck
		if (ffcNo == FFC_FACTORY_NUMBER)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Save ffcNo(%d) Default Parameter Error.(Min:%d / Max:%d)\n", ffcNo, FFC_NUMBER_MIN+1, FFC_NUMBER_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Check ffcNo Parameter
		if ((ffcNo < FFC_NUMBER_MIN+1) || (ffcNo > FFC_NUMBER_MAX-1))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Save ffcNo(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNo, FFC_NUMBER_MIN+1, FFC_NUMBER_MAX-1);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
		
		// クリア
		memset ((void *)&ffcParam, 0, sizeof(ffcParam));

		// パラメータ設定
		ffcParam.ffcNo = ffcNo;
		//@@@1ffcParam.mode = MODE_BATCH;
		ffcParam.memType = FFC_MEMORY_EXT;
		ffcParam.userMode = FFC_USER;

		
		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// FFC Save

		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if (corMode == FFC_CORRECTION_MODE_FIRST)
		{
			if ((status = ffcFlashSave (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		//--------------------------------------------------------------------------------
		// Shading Line補正
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
		{
			if ((status = ffcFlashSaveShadingLine (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
#endif

	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Data Saveコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcSaveHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC Offset Gain is written Flash Memory.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcsave [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", FFC_NUMBER_MIN+1);
	DEBUG_PRINT_FORCE ("  %d : Max\n",  FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Data Save(Admin)コマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcSaveAdmin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int ffcNo;
	FFC_PARAM ffcParam;
	int min, max;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcSaveAdminHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// FFC番号
		if (sscanf (gCmdArg[1], "%d", &ffcNo) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// クリア
		memset ((void *)&ffcParam, 0, sizeof(ffcParam));

		// パラメータ設定
		ffcParam.ffcNo = ffcNo;
		//@@@1ffcParam.mode = MODE_BATCH;
		ffcParam.memType = FFC_MEMORY_EXT;
		ffcParam.userMode = FFC_ADMIN;

		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// FFC Save

		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if (corMode == FFC_CORRECTION_MODE_FIRST)
		{
			ffcParam.userMode = FFC_USER;
			min = FFC_NUMBER_ADMIN_MIN;
			max = FFC_NUMBER_ADMIN_MAX;

			// Check ffcNo Parameter
			if ((ffcNo < min) || (ffcNo > max))
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "FFC Save Admin ffcNo(%d) Parameter Error. (Min=%d / Max=%d)\n", ffcNo, min, max);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			if ((status = ffcFlashSave (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		//--------------------------------------------------------------------------------
		// Shading Line補正
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
		{
			if ((status = ffcFlashSaveShadingLine (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Flash Saveコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcSaveAdminHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC Offset Gain is written Flash Memory.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcsave-admin [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : FFC Number\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Password\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", FFC_NUMBER_ADMIN_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n", FFC_NUMBER_ADMIN_MAX);
	DEBUG_PRINT_FORCE ("\n");
	
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Data Saveコマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcSaveAdminAll (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int ffcNo;
	FFC_PARAM ffcParam;
	int min, max;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcSaveAdminAllHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{

		// FFC番号
		if (sscanf (gCmdArg[1], "%d", &ffcNo) != 1)
		{
			DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_PARAM);
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			goto _DONE;
		}

		// Check ffcNo Parameter
		if ((ffcNo < FFC_NUMBER_MIN) || (ffcNo > FFC_NUMBER_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Save Admin ffcNo(%d) Parameter Error. (Min=%d / Max=%d)\n", ffcNo, FFC_NUMBER_MIN, FFC_NUMBER_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// クリア
		memset ((void *)&ffcParam, 0, sizeof(ffcParam));

		// All Mode
		ffcAdjustAllFlag = 1;

		// パラメータ設定
		ffcParam.ffcNo = ffcNo;
		//@@@1ffcParam.mode = MODE_BATCH;
		ffcParam.userMode = FFC_ADMIN;
		ffcParam.memType = FFC_MEMORY_EXT;
		ffcParam.userMode = FFC_ADMIN;

		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC Save

		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if (corMode == FFC_CORRECTION_MODE_FIRST)
		{
			ffcParam.userMode = FFC_USER;
			if ((status = ffcFlashSave (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		//--------------------------------------------------------------------------------
		// Shading Line補正
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
		{
			if ((status = ffcFlashSaveShadingLine (ffcParam)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	// All Mode Clear
	ffcAdjustAllFlag = 0;
	return (status);
}


//**********************************************************************************
//	FFC Flash Saveコマンド Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcSaveAdminAllHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC Offset Gain is all written Flash Memory.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcsaveall-admin [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : FFC Number\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Number]\n");
	DEBUG_PRINT_FORCE ("  %d : Min\n", FFC_NUMBER_MIN);
	DEBUG_PRINT_FORCE ("  %d : Max\n", FFC_NUMBER_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCオフセットデータ Read/Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcOffsetData (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;
	int width, height;
	unsigned int data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcOffsetDataHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// Width
		if (sscanf (gCmdArg[1], "%d", &width) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Height
		if (sscanf (gCmdArg[2], "%d", &height) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Get Offset Data
		if ((status = ffcGetOffsetData (ffcNum, width, height, &data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", data);
	}
	else if (argc == 4)
	{
		// Width
		if (sscanf (gCmdArg[1], "%d", &width) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Height
		if (sscanf (gCmdArg[2], "%d", &height) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Data
		if (sscanf (gCmdArg[3], "%d", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Set Offset Data
		if ((status = ffcSetOffsetData (ffcNum, width, height, data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCオフセットデータRead/Write Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcOffsetDataHelp (void *str)
{
	int widthMax, heightMax;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Offset Data is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcoffset-data [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Width\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Height\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Offset Data\n");
	
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Offset Data is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcoffset-data [Param0] [Param1] [Param2]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Width\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Height\n");
	DEBUG_PRINT_FORCE ("  Input Param2      : FFC Offset Data\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	// Width Max
	roiGetAreaWidthMax (&widthMax);

	// Height Max
	roiGetAreaHeightMax (&heightMax);

	DEBUG_PRINT_FORCE ("[Width]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", widthMax-1);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Height]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", heightMax-1);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Offset Data]\n");

	DEBUG_PRINT_FORCE ("  [Bit=8]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n\n", OFFSET_MASK/64);

	DEBUG_PRINT_FORCE ("  [Bit=10]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n\n", OFFSET_MASK/16);

	DEBUG_PRINT_FORCE ("  [Bit=12]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", OFFSET_MASK/4);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCゲインデータ Read/Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcGainData (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;
	int width, height;
	unsigned int data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcGainDataHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// Width
		if (sscanf (gCmdArg[1], "%d", &width) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Height
		if (sscanf (gCmdArg[2], "%d", &height) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Get Gain Data
		if ((status = ffcGetGainData (ffcNum, width, height, &data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if (argc == 4)
	{
		// Width
		if (sscanf (gCmdArg[1], "%d", &width) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Height
		if (sscanf (gCmdArg[2], "%d", &height) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		if (sscanf (gCmdArg[3], "%d", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Set Gain DAta
		if ((status = ffcSetGainData (ffcNum, width, height, data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCゲインデータRead/Write Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcGainDataHelp (void *str)
{
	int widthMax, heightMax;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Gain Data is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcgain-data [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Width\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Height\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Gain Data\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Gain Data is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcgain-data [Param0] [Param1] [Param2]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Width\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Height\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : FFC Gain Data\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	// Width Max
	roiGetAreaWidthMax (&widthMax);

	// Height Max
	roiGetAreaHeightMax (&heightMax);

	DEBUG_PRINT_FORCE ("[Width]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", widthMax-1);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Height]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", heightMax-1);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Gain Data]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", GAIN_MASK);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCゲインデータ(倍率) Read/Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcGainX (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;
	int width, height;
	double data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcGainXHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 3)
	{
		// Width
		if (sscanf (gCmdArg[1], "%d", &width) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Height
		if (sscanf (gCmdArg[2], "%d", &height) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Get Gain Data
		if ((status = ffcGetGainX (ffcNum, width, height, &data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%.5f ", data);
	}
	else if (argc == 4)
	{
		// Width
		if (sscanf (gCmdArg[1], "%d", &width) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Height
		if (sscanf (gCmdArg[2], "%d", &height) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Data
		if (sscanf (gCmdArg[3], "%lf", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Set Gain Data
		if ((status = ffcSetGainX (ffcNum, width, height, data)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCゲインデータ(倍率) Read/Write Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcGainXHelp (void *str)
{
	int widthMax, heightMax;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Gain Data is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcgain-x [Param0] [Param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Width\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Height\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Gain Data\n");

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Gain Data is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcgain-x [Param0] [Param1] [Param2]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Width\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Height\n");
	DEBUG_PRINT_FORCE ("  Input  Param2     : FFC Gain Data\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	// Width Max
	roiGetAreaWidthMax (&widthMax);

	// Height Max
	roiGetAreaHeightMax (&heightMax);

	DEBUG_PRINT_FORCE ("[Width]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", widthMax-1);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Height]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n", heightMax-1);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[FFC Gain Data]\n");
	DEBUG_PRINT_FORCE ("  Min : 0\n");
	DEBUG_PRINT_FORCE ("  Max : %.5f\n",  (double)((double)GAIN_MASK / (double)BIT_16_MAX));
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCオフセットデータ平均/最小/最大取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcOffsetAve (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;
	int dpcNum = 0;
	double ave;
	FFC_OG_INFO min, max;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcOffsetAveHelp (NULL);
			goto _DONE;
		}
	}

	if ((argc == 1) || (argc == 2))
	{
		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if ((status = ffcGetOffsetAve (ffcNum, dpcNum, &ave, &min, &max)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (argc == 1)
		{
			DEBUG_PRINT_FORCE ("%.3f ", ave);
		}
		else
		{
			DEBUG_PRINT_FORCE ("Average  : %.3f\n", ave);
			DEBUG_PRINT_FORCE ("Minimum  : %d (x=%d, y=%d)\n", (int)min.data, min.x, min.y);
			DEBUG_PRINT_FORCE ("Maximum  : %d (x=%d, y=%d)\n", (int)max.data, max.x, max.y);
		}
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCオフセットデータ平均/最小/最大取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcOffsetAveHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Offset Data(Average/Minimum/Maximum) is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcoffsetave\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Offset Data Average/Minimum/Maximum Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCゲインデータ(倍率) 平均/最小/最大取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcGainAve (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;
	int dpcNum = 0;
	double ave;
	FFC_OG_INFO min, max;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcGainAveHelp (NULL);
			goto _DONE;
		}
	}

	if ((argc == 1) || (argc == 2))
	{
		if ((status = ffcGetGainAve (ffcNum, dpcNum, &ave, &min, &max)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (argc == 1)
		{
			DEBUG_PRINT_FORCE ("%.3f ", ave);
		}
		else
		{
			DEBUG_PRINT_FORCE ("Average  : %.3f\n", ave);
			DEBUG_PRINT_FORCE ("Minimum  : %.3f (x=%d, y=%d)\n", min.data, min.x, min.y);
			DEBUG_PRINT_FORCE ("Maximum  : %.3f (x=%d, y=%d)\n", max.data, max.x, max.y);
		}
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCゲインデータ(倍率) 平均/最小/最大取得Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcGainAveHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Gain Data(Average/Minimum/Maximum) is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcgainave\n");
	DEBUG_PRINT_FORCE ("  Input Param       : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : FFC Gain Data Average/Minimum/Maximum Information\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCデータアップロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataUpload (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcDataUploadHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = ffcDataUpload (ffcNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータアップロードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataUploadHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Data is upload.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffc-upload\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCデータアップロード(Bin値)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataUploadBin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcDataUploadBinHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = ffcDataUploadBin (ffcNum)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータアップロードHelp(Bin値)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataUploadBinHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Data is upload.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffc-upload-bin\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCデータダウンロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataDownload (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;
	unsigned int waitTimeS;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcDataDownloadHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Wait Time（秒単位）
		if (sscanf (gCmdArg[1], "%d", &waitTimeS) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Wait Time Parameter
		if (waitTimeS > DOWNLOAD_WAIT_TIME_MAX)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Data Download Wait Time(%d) Parameter Error. (Min:0 / Max:%d)\n", waitTimeS, DOWNLOAD_WAIT_TIME_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Wait
		sDelay (waitTimeS);

		// FFC Download
		if ((status = ffcDataDownload (ffcNum, UPDATE_UNCOMPRESS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータダウンロードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataDownloadHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Data is download.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffc-download [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Start Wait Time Secound(Min:0 - Max:%d)\n", DOWNLOAD_WAIT_TIME_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCデータダウンロード(圧縮版)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataDownloadCompress (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;
	unsigned int waitTimeS;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcDataDownloadCompressHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Wait Time（秒単位）
		if (sscanf (gCmdArg[1], "%d", &waitTimeS) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Wait Time Parameter
		if (waitTimeS > DOWNLOAD_WAIT_TIME_MAX)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Data Download Wait Time(%d) Parameter Error. (Min:0 / Max:%d)\n", waitTimeS, DOWNLOAD_WAIT_TIME_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Wait
		sDelay (waitTimeS);

		// FFC Downlaod
		if ((status = ffcDataDownload (ffcNum, UPDATE_COMPRESS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータダウンロードHelp(圧縮版)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataDownloadCompressHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Data is download.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffc-download-compress [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Start Wait Time Secound(Min:0 - Max:%d)\n", DOWNLOAD_WAIT_TIME_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCデータダウンロード(Bin版)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataDownloadBin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;
	unsigned int waitTimeS;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcDataDownloadBinHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Wait Time（秒単位）
		if (sscanf (gCmdArg[1], "%d", &waitTimeS) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Wait Time Parameter
		if (waitTimeS > DOWNLOAD_WAIT_TIME_MAX)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Data Download Wait Time(%d) Parameter Error. (Min:0 / Max:%d)\n", waitTimeS, DOWNLOAD_WAIT_TIME_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Wait
		sDelay (waitTimeS);

		// FFC Download
		if ((status = ffcDataDownloadBin (ffcNum, UPDATE_UNCOMPRESS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータダウンロード(Bin版)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataDownloadBinHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Data is download.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffc-download-bin [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Start Wait Time Secound(Min:0 - Max:%d)\n", DOWNLOAD_WAIT_TIME_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFCデータダウンロード(圧縮版Bin)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataDownloadCompressBin (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int ffcNum = 0;
	unsigned int waitTimeS;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcDataDownloadCompressBinHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Wait Time（秒単位）
		if (sscanf (gCmdArg[1], "%d", &waitTimeS) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Check Wait Time Parameter
		if (waitTimeS > DOWNLOAD_WAIT_TIME_MAX)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "FFC Data Download Wait Time(%d) Parameter Error. (Min:0 / Max:%d)\n", waitTimeS, DOWNLOAD_WAIT_TIME_MAX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Wait
		sDelay (waitTimeS);

		// FFC Download
		if ((status = ffcDataDownloadBin (ffcNum, UPDATE_COMPRESS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFCデータダウンロードHelp(圧縮版Bin)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDataDownloadCompressBinHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Data is download.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffc-download-bin-compress [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Start Wait Time Secound(Min:0 - Max:%d)\n", DOWNLOAD_WAIT_TIME_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Default
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDefault (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int corMode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcDefaultHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		//--------------------------------------------------------------------------------
		// FFC Correction Mode取得
		//--------------------------------------------------------------------------------
		if ((status = ffcGetCorMode (&corMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		//--------------------------------------------------------------------------------
		// 1次補正
		//--------------------------------------------------------------------------------
		if (corMode == FFC_CORRECTION_MODE_FIRST)
			status = ffcDefault ();

		//--------------------------------------------------------------------------------
		// Shading Line補正
		//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
		else if (corMode == FFC_CORRECTION_MODE_SHADING_LINE)
			status = ffcDefaultShadingLine ();
#endif
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC Default Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcDefaultHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : FFC Data is default.\n");
	DEBUG_PRINT_FORCE ("  Command           : ffcdefault\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	FFC Black Target
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcBlackTarget (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int target;
	int offset1, offset2;
	int targetSigned;
	int calc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcBlackTargetHelp (NULL);
			goto _DONE;
		}
	}

	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (argc == 1)
	{
		// Black Target取得
		if ((status = ffcGetBlackTarget (&target)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		target /= calc;

		DEBUG_PRINT_FORCE ("%d ", target);
	}
	else if (argc == 2)
	{
		// black target
		if (sscanf (gCmdArg[1], "%d", &target) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		target *= calc;

		// Digital Offset取得
		if ((status = digitalGetOffset1 (&offset1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Digital Offset取得
		if ((status = digitalGetOffset (&offset2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Black Target設定
		if ((status = ffcSetBlackTarget (target)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Digital Offset設定
		if ((status = digitalSetOffset1 (offset1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Digital Offset設定
		if ((status = digitalSetOffset (offset2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Black Target Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcBlackTargetHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int calc;

	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC black target is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : blacktarget\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Black Target(Min:%d - Max:%d)\n", FFC_BLACK_TARGET_MIN, FFC_BLACK_TARGET_MAX/calc);
	DEBUG_PRINT_FORCE ("\n");

_DONE:
	return (status);
}


//**********************************************************************************
//	FFC White Target
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcWhiteTarget (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int target;
	int calc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdFfcWhiteTargetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Get FFC Bit
		if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// White Target取得
		if ((status = ffcGetWhiteTarget (&target)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		target /= calc;

		DEBUG_PRINT_FORCE ("%d ", target);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	White Target Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdFfcWhiteTargetHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The FFC white target is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : whitetarget\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : White Target\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
