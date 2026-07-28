//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// version.c - Version Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int gInterFaceID;						// Interface ID


//**********************************************************************************
//	バージョン表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int versionShow (void)
{
	firmStartMsg (1);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	スタートメッセージ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：0=デバック時のみ表示/1=常に表示
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int firmStartMsg (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	char boardVer[BOARD_VERSION_SIZE+1];
	char fpgaVer[FPGA_VERSION_SIZE+1];
	char firmVer[FIRM_VERSION_SIZE+1];
	char mainVer[MAIN_VERSION_SIZE+1];
	char bootVer[BOOT_VERSION_SIZE+1];
#if defined (MODE_CXP)
	unsigned int ifFpgaVersion;
#endif
#if defined (MODE_GIGE_10G)
	char phyFirmVer[PHY_FIRM_VERSION_SIZE];
#endif

	// メインバージョン取得
	mainVersion (mainVer);

	// BootLoader Version
	bootVersion (bootVer);

#if 0	//@@@1
	// 基板バージョン取得
	boardVersion (boardVer);

	// FPGAバージョン取得
	fpgaVersion (fpgaVer);

	// Firmバージョン取得
	firmVersion (firmVer);

#if defined (MODE_CXP)
	// CXP FPGAバージョン
	if (gInterFaceID == INTERFACE_CXP)
		 ifFpgaVersion = IN32 (FIRM_DATA_IF_VERSION_ADRS);
#endif

#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		phyGetFirmVersionMem (phyFirmVer);
	}
#endif
#endif //@@@1

	DEBUG_PRINT_FORCE ("\n[Version informations]\n");

	// Mainバージョン表示
	DEBUG_PRINT_FORCE ("   Main                      ");
	DEBUG_PRINT_FORCE ("Version %s\n", mainVer);
#if 0//@@@1
	// 基板バージョン表示
	DEBUG_PRINT_FORCE ("   Board                     ");
	DEBUG_PRINT_FORCE ("Version %s\n", boardVer);

	// FPGAバージョン表示
	DEBUG_PRINT_FORCE ("   FPGA                      ");
	DEBUG_PRINT_FORCE ("Version %s\n", fpgaVer);

	// Firmバージョン表示
	DEBUG_PRINT_FORCE ("   Firmware                  ");
	DEBUG_PRINT_FORCE ("Version %s\n", firmVer);

	// CXP FPGAバージョン
#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		DEBUG_PRINT_FORCE ("   IF FPGA                   ");
		DEBUG_PRINT_FORCE ("Version %x.%x\n", ifFpgaVersion>>4, ifFpgaVersion&0x0f);
	}
#endif
	
	// PHY Version
#if defined (MODE_GIGE_10G)
	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		DEBUG_PRINT_FORCE ("   PHY Firm                  ");
		DEBUG_PRINT_FORCE ("Version %s\n", phyFirmVer);
	}
#endif
#endif  //@@@1
	
	// BootLoaderバージョン表示
	DEBUG_PRINT_FORCE ("   Boot                      ");
	DEBUG_PRINT_FORCE ("Version %s\n", bootVer);

	DEBUG_PRINT_FORCE ("\n");

	return (status);
}


//**********************************************************************************
//	Boardバージョン設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pVer				：ボードバージョンを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setboardVersion (char *pVer)
{
	int status = AVAL_STATUS_SUCCESS;
	int len;
	unsigned int sizeMax = BOARD_PARAM_BOARD_VERSION_SIZE;
	char tempBuff[BOARD_PARAM_BOARD_VERSION_SIZE];

	// Check pName Parameter
	if (pVer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Board Version pVer NULL Parameter Error\n");
		goto _DONE;
	}

	// データ長取得
	len = strlen (pVer);
	if (len > (sizeMax - 1))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Board Version Length(%d) Parameter Error. (Min:1 - Max:%d)\n", len, sizeMax - 1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリ領域クリア
	memset ((void *)tempBuff, 0x00, sizeMax);

	// ボードバージョン Copy
	strcpy ((char *)tempBuff, (char *)pVer);

	// Set Board Version
	if((status = setBoardParam (tempBuff, BOARD_PARAM_BOARD_VERSION_ADRS, sizeMax)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 共通メモリ領域クリア
	memset ((void *)FIRM_DATA_BOARD_VERSION_ADRS, 0x00, sizeMax);

	// モデル名 Copy
	strcpy ((char *)FIRM_DATA_BOARD_VERSION_ADRS, (char *)tempBuff);

_DONE:
	return (status);
}


//**********************************************************************************
//	基板バージョン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pVer				：バージョンを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int boardVersion (char *pVer)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pVer Parameter
	if (pVer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Board Version pVer NULL Parameter Error\n");
		goto _DONE;
	}

	// Board Version Copy
	if ((status = getBoardParam (pVer, BOARD_PARAM_BOARD_VERSION_ADRS, BOARD_PARAM_BOARD_VERSION_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		pVer[0] = '\0';
		return (status);
	}

	// 設定されていなければデフォルト
	if (((pVer[0] < 0x20) || (pVer[0] > 0x7f)) && (pVer[0] != 0x00))
	{
		// クリア
		memset ((void *)pVer, 0x00, BOARD_PARAM_BOARD_VERSION_SIZE);

		// 設定
		strncpy (pVer, HW_VERSION, BOARD_VERSION_SIZE);
	}
	else
	{
		*(pVer + BOARD_VERSION_SIZE) = '\0';
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	FPGAバージョン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pVer				：バージョンを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaVersion (char *pVer)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;
	char *ptr;

	// Check pVer Parameter
	if (pVer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Version pVer NULL Parameter Error\n");
		goto _DONE;
	}

	ptr = pVer;
	for(i=0; i<FPGA_VERSION_SIZE; i++, ptr++)
		*ptr = IN8((GENICAM_VER_FPGA_ADRS+i));

	*ptr = '\0';

_DONE:
	return (status);
}


//**********************************************************************************
//	ファームウェアバージョン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pVer				：バージョンを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmVersion (char *pVer)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Parameter
	if (pVer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Firmware Version pVer NULL Parameter Error\n");
		goto _DONE;
	}

	// Copy Version
	strcpy (pVer, FIRM_VERSION);

_DONE:
	return (status);
}


//**********************************************************************************
//	メインバージョン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pVer				：バージョンを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int mainVersion (char *pVer)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Parameter
	if (pVer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Main Version pVer NULL Parameter Error\n");
		goto _DONE;
	}

	// Copy Version
	strcpy (pVer, MAIN_VERSION);

_DONE:
	return (status);
}


//**********************************************************************************
//	Bootバージョン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pVer				：バージョンを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int bootVersion (char *pVer)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check Parameter
	if (pVer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Boot Version pVer NULL Parameter Error\n");
		goto _DONE;
	}

	// Copy Version
	strcpy (pVer, (char *)FIRM_DATA_BOOT_VERSION);

_DONE:
	return (status);
}


//**********************************************************************************
//	Firmware名取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pName				：ファームウェア名を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmName (char *pName)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pName Parameter
	if (pName == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Firmware Name pName NULL Parameter Error\n");
		goto _DONE;
	}

	// Copy Name
	strcpy (pName, FIRM_NAME);

_DONE:
	return (status);
}


//**********************************************************************************
//	ファームウェアBuild情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int firmBuildShow (void)
{
	int status = AVAL_STATUS_SUCCESS;
	char strDate[32];
	char strTime[32];

	DEBUG_PRINT_FORCE ("[Firm Build informations]\n");

	// ビルド日付情報取得
	if ((status = firmGetBuildDate (strDate)) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("  Build Date                 ");
		DEBUG_PRINT_FORCE ("%s\n",strDate);
	}

	// ビルド時間情報取得
	if ((status = firmGetBuildTime (strTime)) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("  Build Time                 ");
		DEBUG_PRINT_FORCE ("%s\n",strTime);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	ファームウェアビルド日付情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStr				：ファームウェアビルド日付を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmGetBuildDate (char *pStr)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pStr Parameter
	if (pStr == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Firmware Build Date pStr NULL Parameter Error\n");
		goto _DONE;
	}

	// Copy
	strcpy (pStr, __DATE__);

_DONE:
	return (status);
}


//**********************************************************************************
//	ファームウェアビルド時間情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStr				：ファームウェアビルド時間を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int firmGetBuildTime (char *pStr)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pStr Parameter
	if (pStr == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Firmware Build Time pStr NULL Parameter Error\n");
		goto _DONE;
	}

	// Copy
	strcpy (pStr, __TIME__);

_DONE:
	return (status);
}


//**********************************************************************************
//	FPGABuild情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int fpgaBuildShow (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	DEBUG_PRINT_FORCE ("[FPGA Build informations]\n");

	// ビルド日付情報取得
	if ((status = fpgaGetBuildDate (&data)) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("  Build Date                 ");
		DEBUG_PRINT_FORCE ("%08x\n",data);
	}

	// ビルドNumber取得
	if ((status = fpgaGetBuildNumber (&data)) == AVAL_STATUS_SUCCESS)
	{
		DEBUG_PRINT_FORCE ("  Build Number               ");
		DEBUG_PRINT_FORCE ("%08x\n",data);
	}

	return (status);
}


//**********************************************************************************
//	FPGAビルド日付情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStr				：ファームウェアビルド日付を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaGetBuildDate (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Build Date pData NULL Parameter Error\n");
		goto _DONE;
	}

	*pData = IN32 (FPGA_BUILD_DATE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	ファームウェアビルド時間情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStr				：ファームウェアビルド時間を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaGetBuildNumber (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FPGA Build Number pData NULL Parameter Error\n");
		goto _DONE;
	}

	*pData = IN32 (FPGA_BUILD_NUMBER_ADRS);

_DONE:
	return (status);
}

// eof
