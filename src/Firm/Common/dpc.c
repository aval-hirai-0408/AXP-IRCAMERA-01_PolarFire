//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// dpc.c - DPC Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gDpcStatus = 0;								// DPC Status
int gDpcDefectionCount = 0;						// 欠陥補正数最大数
int gDpcGridImpossibleMode = 0;					// 1=DPC欠陥座標が補正できない場合はエラーで終了/0=DPC欠陥座標が補正できない場合はエラーで終了しない
int gDpcGridImpossibleStatus = 0;				// ステータス
int gDpcGridImpossibleXGrid = 0;				// gDpcGridImpossibleMode有効時のX座標
int gDpcGridImpossibleYGrid = 0;				// gDpcGridImpossibleMode有効時のY座標
int gDpcImpossibleSaveMode = 0;					// 欠陥不可座標の保存フラグ


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern DPCINFO gDpcInfo;						// DPC情報構造体
//extern GRID_XY *pDpcImpossibleGrid;			// 欠陥不可検出座標データ
extern unsigned char *pImageDetect[];			// 検出結果格納用
extern DEFECTIONINFO *pImageDefectionInfo;		// 補正情報格納用
extern GRID_XY *dpcGetDefectionCountBuff;		// 欠陥画素座標
extern GRID_XY *dpcGetDefectionCountBuff2;		// 欠陥画素座標
extern unsigned short *pImageSrc;				// 検出用入力画像の先頭を指すポインタ配列


//**********************************************************************************
//	DPC初期化(Admin)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		memAdrs				：DPCメモリアドレス
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcInitialize (unsigned int flashAdrs, unsigned int memAdrs, int mode)
{
	int status;
	
	status = dpcInitializeMain (flashAdrs, memAdrs, mode, DPC_USER);
	return (status);
}


//**********************************************************************************
//	DPC初期化(Admin)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		memAdrs				：DPCメモリアドレス
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcInitializeAdmin (unsigned int flashAdrs, unsigned int memAdrs, int mode)
{
	int status;
	
	status = dpcInitializeMain (flashAdrs, memAdrs, mode, DPC_ADMIN);
	return (status);
}


//**********************************************************************************
//	DPC初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		memAdrs				：DPCメモリアドレス
//		mode				：0=無効/1=有効
//		userMode			：0=Noraml/1=Admin
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcInitializeMain (unsigned int flashAdrs, unsigned int memAdrs, int mode, int userMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int mark;
	int dpcNum;
	unsigned int adrs;
#if defined (MODE_DPC_GRID_UPDATE)
	int count;
#endif

#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map Mode設定(0:通常バッファ/1:別バッファにmap情報格納)
	dpcSetMapInfoMode (MODE_DISABLE);
#endif

	// Get DPC Defection Count取得
	if ((status = dpcGetDefectionMaxCount (&gDpcDefectionCount)) != AVAL_STATUS_SUCCESS)
	{
		gDpcStatus = status;
		goto _DONE;
	}

	// Get DPC Number
	if (userMode == DPC_USER)
	{
		if ((status = dpcGetNumFromFlash (flashAdrs, &dpcNum)) != AVAL_STATUS_SUCCESS)
		{
			gDpcStatus = status;
			goto _DONE;
		}
	}
	else
	{
		if ((status = dpcGetNumFromFlashAdmin (flashAdrs, &dpcNum)) != AVAL_STATUS_SUCCESS)
		{
			gDpcStatus = status;
			goto _DONE;
		}
	}

	// メモリFill
	memset ((void *)DPC_MEMORY_ADRS, 0xff, DPC_MEMORY_NEW_ALL_SIZE);

	// DPCマークCheck
	if (userMode == DPC_USER)
	{
		if ((status = dpcGetMarkFlash (dpcNum, &mark)) != AVAL_STATUS_SUCCESS)
		{
			gDpcStatus = status;
			goto _DONE;
		}
	}
	else
	{
		if ((status = dpcGetMarkFlashAdmin (dpcNum, &mark)) != AVAL_STATUS_SUCCESS)
		{
			gDpcStatus = status;
			goto _DONE;
		}
	}
	
	sprintf (gLogMsgBuff, "DPC MARK Check..0x%x\n", mark);
	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

	// DPCマークは有効?
	if ((mark == DPC_NO_DATA) || (mark == 0))
	{
		// DPC無効
		mode = 0;

		// サイズ
		mark = 0;

		// 欠陥画素補正数設定
		OUT32 (FIRM_DATA_DPC_NUM_ADRS, 0);

		// 1度この領域に展開後Flashに書き込み
		memset ((void *)memAdrs, 0xff, DPC_MEMORY_NEW_ALL_SIZE);
	}
	else
	{
		// 欠陥補正データの読み込み
		if (userMode == DPC_USER)
			dpcToMemory (flashAdrs, memAdrs);
		else
			dpcToMemoryAdmin (flashAdrs, memAdrs);

		// 欠陥画素補正数設定
		OUT32 (FIRM_DATA_DPC_NUM_ADRS, mark);
	}

	// キャッシュFlash
	cacheFlush ();

	// DPCパラメータ初期化 & Enable
	dpcRegInit (memAdrs, DPC_MEMORY_NEW_SIZE, mode);
	sprintf (gLogMsgBuff, "DPC Initialize Flash = 0x%x, Memory = 0x%x, Size = 0x%x, Mode = %d\n", flashAdrs, memAdrs, DPC_MEMORY_NEW_SIZE, mode);
	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	
	// DPC情報読み込み
	if ((status = dpcAdjustInfoRead (dpcNum, userMode, &gDpcInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥補正不可データ読み込み
	//if ((status = dpcImpossibleGridRead (dpcNum, userMode, (unsigned short *)pDpcImpossibleGrid)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// DPC調整パラメータ設定
	dpcSetAdjustParam ();

#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map情報更新
	if ((status = dpcGetMapInfo ((unsigned char *)memAdrs, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標数登録
	dpcSetDefectionCountVersion2 (count);
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：DPCメモリアドレス
//		size				：DPCメモリサイズ
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcRegInit (unsigned int adrs, unsigned int size, int mode)
{
	// Set Address
	OUT32 (FPGA_DPC_DMA_ADRS, adrs);

	#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DPC2_DMA_ADRS, (adrs + DPC_MEMORY_IPU_MULTI_INTERVAL));
	#endif

	if (size == 0)
	{
		// DPC Disable
		dpcSetEnableMode (MODE_DISABLE);
	}
	else
	{
		// DPC Enable
		dpcSetEnableMode (mode);
	}

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DPC Enable/Disable設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
///		mode				：0=Disable/1=Enable/それ以外=現状維持
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int number;
	int currentMode;

	// 欠陥画素補正数取得
	number = IN32 (FIRM_DATA_DPC_NUM_ADRS);
	if (number == 0)
	{
		if ((status = dpcGetMode (&currentMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (currentMode != DPC_DISABLE)
		{
			if ((status = dpcSetEnableMode (DPC_DISABLE)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		goto _DONE;
	}

	// Set Enable
	if ((status = dpcSetEnableMode (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}



//**********************************************************************************
//	DPC Enable/Disable取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
///		pMode				：0=Disable/1=Enableを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Mode pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Get DPC Mode
	data = IN32 (FPGA_DPC_CTRL_ADRS);

	// Get DPC
	if (data & FPGA_DPC_CTRL_ENABLE_BIT)
		*pMode = DPC_ENABLE;
	else
		*pMode = DPC_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Enable/Disable設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
///		mode				：0=Disable/1=Enable
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetEnableMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Check data Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPC Enable / Disable
	OUT32 (FPGA_ROI_DPC_ADRS, mode);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_ROI2_DPC_ADRS, mode);
#endif

	// ROI Set
	if ((status = roiSetEnd()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	DPC Enable/Disable設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
///		pMode				：Mode情報を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetEnableMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Enable Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// DPC Enable / Disable
	*pMode = IN32 (FPGA_ROI_DPC_ADRS) & FPGA_ROI_DPC_ENABLE_BIT;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjustパラメータ 0初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetAdjustParamZero (void)
{
	int status = AVAL_STATUS_SUCCESS;

	return (status);
}


//**********************************************************************************
//	DPC Adjustパラメータ初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetAdjustParam (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int index, bIndex;
	unsigned int adrs, offset;
	float fValue;

	// DPC補正モード
	OUT32 (FIRM_DATA_DPC_ADJUST_CORMODE_ADRS, gDpcInfo.dpcCorMode);

	for (index=0; index<FFC_NUM_USER; index++)
	{
		// アドレス
		adrs = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + index * FIRM_DATA_DPC_ADJUST_FFC_SIZE;

		// Rate
		fValue = (float)gDpcInfo.dpcPrivate[index].frameRate;
		OUTF ((adrs + FIRM_DATA_DPC_ADJUST_RATE_ADRS), fValue);

		// Exposure
		OUT32 ((adrs + FIRM_DATA_DPC_ADJUST_EXPOSURE_ADRS), gDpcInfo.dpcPrivate[index].expTime);

		// Temp
		fValue = (float)gDpcInfo.dpcPrivate[index].sensorTemp;
		OUTF ((adrs + FIRM_DATA_DPC_ADJUST_TEMP_ADRS), fValue);

		// FFC Number
		OUT32 ((adrs + FIRM_DATA_DPC_ADJUST_FFC_NUMBER_ADRS), gDpcInfo.dpcPrivate[index].ffcNo);

		for (bIndex=0; bIndex<DPC_BRIGHTNESS_LOOP_NUMBER; bIndex++)
		{
			// Offset
			offset = bIndex * 4;

			// Standard Deviation
			fValue = (float)gDpcInfo.dpcPrivate[index].sd[bIndex];
			OUTF ((adrs + offset + FIRM_DATA_DPC_ADJUST_SD0_ADRS), fValue);

			// Non Uniformity
			fValue = (float)gDpcInfo.dpcPrivate[index].ununiformity[bIndex];
			OUTF ((adrs + offset + FIRM_DATA_DPC_ADJUST_NON_UNIFORM0_ADRS), fValue);
		}
	}

	return (status);
}


//**********************************************************************************
//	DPCデータをロードする番号を設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ffcNum				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetLoadNum (int dpcNum)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Load Number dpc(%d) Parameter Error. (Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DPC番号設定
	status = cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_DPC_NUMBER_ADRS, 0, dpcNum);

_DONE:
	return (status);
}


//**********************************************************************************
//	DPCデータをロードしている番号を設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFfcNum				：FFC番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetLoadNum (int *pDpcNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int dpcNumAdrs;

	// Check pDpcNum Parameter
	if (pDpcNum == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Load Number pDpcNum NULL Parameter Error.\n");
		goto _DONE;
	}

	// DPC番号取得
	status = cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_DPC_NUMBER_ADRS, &dpcNumAdrs, (unsigned int *)pDpcNum);

_DONE:
	return (status);
}


//**********************************************************************************
//	DPCデータをFlashからメモリにロード(User)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcLoad (int dpcNum)
{
	int status = AVAL_STATUS_SUCCESS;

	status = dpcLoadMain (dpcNum, DPC_USER);
	
	return (status);
}


//**********************************************************************************
//	DPCデータをFlashからメモリにロード(Admin)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcLoadAdmin (int dpcNum)
{
	int status = AVAL_STATUS_SUCCESS;

	status = dpcLoadMain (dpcNum, DPC_ADMIN);
	
	return (status);
}


//**********************************************************************************
//	DPCデータをFlashからメモリにロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		userMode			：0=Normal/1=Admin
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcLoadMain (int dpcNum, int userMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int memAdrs, flashAdrs;
	int startFlag = 0;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Load dpcNum(%d) Parameter Error. (Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	acquisitionAbort ();

	// DPCメモリアドレス取得
	if ((status = dpcGetMemAdrs (dpcNum, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (userMode == DPC_USER)
	{
		// DPC Flashアドレス取得
		if ((status = dpcGetFlashAdrs (dpcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC初期化
		if ((status = dpcInitialize (flashAdrs, memAdrs, FPGA_DPC_CTRL_ENABLE_BIT)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

	}
	else
	{
		// DPC Flashアドレス取得
		if ((status = dpcGetFlashAdrsAdmin (dpcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC初期化
		if ((status = dpcInitializeAdmin (flashAdrs, memAdrs, FPGA_DPC_CTRL_ENABLE_BIT)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// DPC番号保存(メモリ)
	if ((status = dpcSetLoadNum (dpcNum)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startFlag != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	DPCデータをFlashメモリにセーブ(User)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSave (int dpcNum)
{
	int status = AVAL_STATUS_SUCCESS;

	status = dpcSaveMain (dpcNum, DPC_USER);

	return (status);
}


//**********************************************************************************
//	DPCデータをFlashメモリにセーブ(Admin)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSaveAdmin (int dpcNum)
{
	int status = AVAL_STATUS_SUCCESS;

	status = dpcSaveMain (dpcNum, DPC_ADMIN);

	return (status);
}


//**********************************************************************************
//	DPCデータをFlashメモリにセーブ(補正データとマーク情報を書き込み)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		userMode			：0=Normal/1=Admin
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSaveMain (int dpcNum, int userMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int memAdrs, flashAdrs;
	unsigned int size;
	int index;
	unsigned int dpcCount;
	int startFlag = 0;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Save dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	acquisitionAbort ();

	// Get Flash Adrs
	if (userMode == DPC_USER)
	{
		if ((status = dpcGetFlashAdrs (dpcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		if ((status = dpcGetFlashAdrsAdmin (dpcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	
	// Get Memory Adrs
	if ((status = dpcGetMemAdrs (dpcNum, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------
	// DPCデータをFlashに書き込み
	//--------------------------------------------------
	if (userMode == DPC_USER)
	{
		if ((status = dpcToFlash (flashAdrs, memAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		if ((status = dpcToFlashAdmin (flashAdrs, memAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	
	// 終端を検出
	if ((status = dpcEndSearch (memAdrs, &index)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check index Parameter
	if (index > NUM_DEFECTION_PIX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Save index(%d) Parameter Error. (Min:0 / Max:%d)\n", index, NUM_DEFECTION_PIX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//--------------------------------------------------
	// Mark情報をFlashに書き込み
	//--------------------------------------------------

	// 欠陥画素補正数設定
	dpcCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);

	if (dpcCount == 0)
		size = DPC_NO_DATA;
	else
		size = dpcCount;

	if (userMode == DPC_USER)
	{
		if ((status = dpcSetMarkFlash (dpcNum, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		if ((status = dpcSetMarkFlashAdmin (dpcNum, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//--------------------------------------------------
	// DPC Adjust情報保存
	//--------------------------------------------------
	dpcAdjustInfoSave (dpcNum, userMode, gDpcInfo);

	//--------------------------------------------------
	// DPC 補正不可座標保存
	//--------------------------------------------------
	if (gDpcImpossibleSaveMode == 1)
	{
		//dpcImpossibleGridSave (dpcNum, userMode, (unsigned short *)pDpcImpossibleGrid);
		gDpcImpossibleSaveMode = 0;
	}

_DONE:
	if (startFlag != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	DPCパラメータ情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMemAdrs			：DPCメモリアドレスを格納するポインタ
//		pSize				：DPCサイズを格納するポインタ
//		pDmaWait			：DPC DMA設定を格納するポインタ
//		pMode				：DPC有効/無効状態を格納するポインタ(0=無効/1=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetParam (unsigned int *pMemAdrs, unsigned int *pSize, unsigned int *pDmaWait, int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMemAdrs Parameter
	if (pMemAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Param pMemAdrs Parameter Error.\n");
		goto _DONE;
	}

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Param pSize pSize Error.\n");
		goto _DONE;
	}

	// Check pDmaWait Parameter
	if (pDmaWait == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Param pDmaWait pSize Error.\n");
		goto _DONE;
	}

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Param pMode pSize Error.\n");
		goto _DONE;
	}

	// Get DPC Control
	dpcGetEnableMode (pMode);

	// Get DPC Address
	*pMemAdrs = IN32 (FPGA_DPC_DMA_ADRS);

	// Get DPC Size
	*pSize = IN32 (FPGA_DPC_DMA_SIZE_ADRS);

	// Get DPC DMA Wait Threshold
	*pDmaWait = IN32 (FPGA_DPC_DMA_WAIT_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	DPCメモリアドレスを取得(DPC番号から)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		pAdrs				：DPCメモリアドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetMemAdrs (int dpcNum, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Memory Adrs dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Memory Adrs pMemAdrs Parameter Error.\n");
		goto _DONE;
	}

	// Get Memory Address
	*pAdrs = DPC_MEMORY_ADRS;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Flashアドレスを取得(DPC番号から)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		pAdrs				：DPC Flashアドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetFlashAdrs (int dpcNum, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Flash dpcNum(%d) Parameter Error.\n", dpcNum);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Flash pMemAdrs Parameter Error.\n");
		goto _DONE;
	}

	// Get Flash Address
	*pAdrs = FLASH_DPC_ADRS + DPC_MEMORY_NEW_UNIT_SIZE * dpcNum;
	
_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Flashアドレスを取得(DPC番号から)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		pAdrs				：DPC Flashアドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetFlashAdrsAdmin (int dpcNum, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Flash Admin dpcNum(%d) Parameter Error.\n", dpcNum);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Flash Admin pMemAdrs Parameter Error.\n");
		goto _DONE;
	}

	// Get Flash Address
	*pAdrs = FLASH_DPC_ORG_ADRS + DPC_MEMORY_NEW_UNIT_SIZE * dpcNum;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC 補正不可座標のFlashアドレスを取得(DPC番号から)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		pAdrs				：DPC Flashアドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetFlashImpossibleAdrs (int dpcNum, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Flash dpcNum(%d) Parameter Error.\n", dpcNum);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Flash pMemAdrs Parameter Error.\n");
		goto _DONE;
	}

	// Get Flash Address
	*pAdrs = FLASH_DPC_IMPOSSIBLE_GRID_ADRS + DPC_MEMORY_NEW_UNIT_SIZE * dpcNum;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC 補正不可座標のFlashアドレスを取得(DPC番号から)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		pAdrs				：DPC Flashアドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetFlashImpossibleAdrsAdmin (int dpcNum, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Flash dpcNum(%d) Parameter Error.\n", dpcNum);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Flash pMemAdrs Parameter Error.\n");
		goto _DONE;
	}

	// Get Flash Address
	*pAdrs = FLASH_DPC_IMPOSSIBLE_GRID_ORG_ADRS + DPC_MEMORY_NEW_UNIT_SIZE * dpcNum;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC FlashアドレスからDPC番号を取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		pAdrs				：DPC番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetNumFromFlash (unsigned int flashAdrs, int *pDpc)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int CheckFlashAdrs;

	// Check pDpc Parameter
	if (pDpc == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Num From Flash pDpc NULL Parameter Error.\n");
		return (status);
	}

	// Get Flash Address
	if ((status = dpcGetFlashAdrs (0, &CheckFlashAdrs)) != AVAL_STATUS_SUCCESS)
		return (status);

	// Get DPC Number
	for (i=0; i<DPC_NUM; i++)
	{
		// Flash Adrs Check
		if (flashAdrs == CheckFlashAdrs)
		{
			*pDpc = i;
			return (AVAL_STATUS_SUCCESS);
		}

		// Flashアドレス更新
		CheckFlashAdrs += DPC_MEMORY_NEW_UNIT_SIZE;
	}

	status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
	sprintf (gLogMsgBuff, "DPC Num From Flash Address(0x%x) Parameter Error.\n", flashAdrs);
	cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	return (status);
}


//**********************************************************************************
//	DPC FlashアドレスからDPC番号を取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		pAdrs				：DPC番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetNumFromFlashAdmin (unsigned int flashAdrs, int *pDpc)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;
	unsigned int CheckFlashAdrs;

	// Check pDpc Parameter
	if (pDpc == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Num From Flash pDpc NULL Parameter Error.\n");
		return (status);
	}

	// Get Flash Address
	if ((status = dpcGetFlashAdrsAdmin (0, &CheckFlashAdrs)) != AVAL_STATUS_SUCCESS)
		return (status);

	// Get DPC Number
	for (i=0; i<DPC_NUM; i++)
	{
		// Flash Adrs Check
		if (flashAdrs == CheckFlashAdrs)
		{
			*pDpc = i;
			return (AVAL_STATUS_SUCCESS);
		}

		// Flashアドレス更新
		CheckFlashAdrs += DPC_MEMORY_NEW_UNIT_SIZE;
	}

	status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
	sprintf (gLogMsgBuff, "DPC Num From Flash Address(0x%x) Parameter Error.\n", flashAdrs);
	cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	return (status);
}


//**********************************************************************************
//	DPC Data Upload
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcDataUpload (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int size = DPC_UPLOAD_SIZE;
	int downLoadSize;
	int mode;
	unsigned int dpcSize;
	int dpcIndex;
	int updateSize;
	unsigned char *pAsciiBuff = NULL;
	char *pAsciiNewBuff = NULL;
	unsigned int memAdrs;
#ifdef COMPRESS_MODE
	unsigned char *pUnCompBuf = NULL;
	int uncomprLen;
#endif
#if defined (MODE_DPC_GRID_UPDATE)
	int count;
#endif

	// DPC Mode
	if ((status = dpcGetMode (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPC OFF
	if ((status = dpcSetMode (DPC_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// メモリリクエスト
	if ((pAsciiBuff = (unsigned char *)malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Data Upload Malloc Error. size = 0x%x\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Kermit Recive
	if ((downLoadSize = kermitRecv ((char *)pAsciiBuff, size)) < 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Data Upload kermit Error. Size = %d\n", downLoadSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Fill
	memAdrs = DPC_MEMORY_ADRS;
	memset ((void *)memAdrs, 0xff, DPC_MEMORY_NEW_ALL_SIZE);

	// バッファ&サイズ更新
	pAsciiNewBuff = (char *)pAsciiBuff;
	updateSize = downLoadSize;

#ifdef COMPRESS_MODE
	if ((status = kermitGzipDataCheck (&pUnCompBuf, &uncomprLen, (unsigned char *)pAsciiBuff, updateSize, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// バッファ&サイズ更新
	if (pUnCompBuf != NULL)
	{
		pAsciiNewBuff = (char *)pUnCompBuf;
		updateSize = uncomprLen;
	}
#endif // COMPRESS_MODE

	// ASCII=>Bin変換
	if ((status = lfDpcUploadBintoAscii ((void *)pAsciiNewBuff, (void *)DPC_MEMORY_ADRS, updateSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// キャッシュFlash
	cacheFlush ();

	// Check index Parameter
	if (updateSize != 0)
	{
		// 終端を検索
		if ((status = dpcEndSearch (DPC_MEMORY_ADRS, &dpcIndex)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (dpcIndex > NUM_DEFECTION_PIX)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "DPC Upload index(%d) Parameter Error. (Min:1 / Max:%d)\n", dpcIndex, NUM_DEFECTION_PIX);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
	else
	{
		dpcIndex = 0;
	}

	// DPCカウント数設定
	if ((status = dpcSetDefectionCount (dpcIndex)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// サイズ取得
	dpcSize = DPC_MEMORY_NEW_SIZE;
	
	// DPC初期化
	if ((status = dpcRegInit (DPC_MEMORY_ADRS, dpcSize, FPGA_DPC_CTRL_ENABLE_BIT)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// DPC Map情報更新
	//------------------------------------------------------------
#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map情報更新
	if ((status = dpcGetMapInfo ((unsigned char *)DPC_MEMORY_ADRS, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標登録
	dpcSetDefectionCountVersion2 (count);
#endif


_DONE:
	if (pAsciiBuff != NULL)
		free (pAsciiBuff);

#ifdef COMPRESS_MODE
#if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)
	if (pUnCompBuf != NULL)
		free (pUnCompBuf);
#endif // #if !defined (MODE_FFC_MEM_EXTERNAL_MALLOC)
#endif // COMPRESS_MODE

	return (status);
}


//**********************************************************************************
//	DPC Upload Data Bin=>ASCII変換
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pAscii				：ASCIIデータを格納するポインタ
//		pBinary				：Binaryデータを格納するポインタ
//		size				：変換サイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lfDpcUploadBintoAscii (void *pAscii, void *pBinary, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pAsciiBuffer;
	unsigned int i;
	unsigned int downLoadSize;
	unsigned int temp;
	unsigned int data;
	unsigned int adrs;
	unsigned char saveData8;
	int widthMax, heightMax;
	int x, y;
	int j;
#if defined (MODE_IPU_MULTI)
	unsigned int offset;
	int xChange;
#endif

	// Check pAscii Parameter
	if (pAscii == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Download pAscii NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pBinary Parameter
	if (pBinary == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Download pBinary NULL Parameter Error\n");
		goto _DONE;
	}

	// Width Max
	widthMax = sensorWidth();

	// Height Max
	heightMax = sensorHeight();

	// ASCII=>Bin変換
	pAsciiBuffer = (unsigned char *)pAscii;
	downLoadSize = size;
	saveData8 = 0;
	adrs = (unsigned int)pBinary;
	i = 0;

	for (y=0; y<heightMax; y++)
	{
		for (x=0; x<(widthMax/8); x++)
		{
			// 1byteの読み込み
			for (j=0; j<2;)
			{
				if (i >= downLoadSize)
					goto _DONE;

				temp = (unsigned int)pAsciiBuffer[i++];
				if (sscanf ((char *)&temp, "%x", &data) == 1)
				{
					if (j== 0)
						saveData8 = (data & 0x0f)<<4;
					else
						saveData8 |= (data & 0x0f);

					j++;
				}
			}
	
			#if !defined (MODE_IPU_MULTI)

			// 1byteの書き込み
			OUT8 ((adrs + x), saveData8);

			#else

			// 分割アドレス取得
			if ((status = dpcGetDivAdrs (x*8, y, &xChange, &offset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// 1byteの書き込み
			OUT8 ((adrs + offset), saveData8);

			#endif
		}

	#if !defined (MODE_IPU_MULTI)
		adrs += DPC_WIDTH_DATA_ALIGH;
	#endif
	}

#if defined (MODE_IPU_MULTI)
	// ipuの1個目の最後の16画素の追加。
	// ipuの2個目の最初の16画素の追加。
	if ((status = dpcSetMarginGridData ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Data Download
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：圧縮モード(0=圧縮/1=非圧縮))
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcDataDownload (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int downloadSize;
	unsigned int mallocSize = DPC_DOWNLOAD_SIZE;
	unsigned char *pAsciiBuff = NULL;
	int kermitSendSize;
	unsigned char *pCompressBuff = NULL;
	unsigned char *ptrSend;
	int comprLen;
	int sendSize;
	char *fileName[64];

	// Check mode Parameter
	if ((mode != UPDATE_COMPRESS) && (mode != UPDATE_UNCOMPRESS))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Data Download mode(%d) Parameter Error. (Compress:%d / Uncpmpress:%d)\n",  UPDATE_COMPRESS, UPDATE_UNCOMPRESS);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリリクエスト
	if ((pAsciiBuff = (unsigned char *)malloc (mallocSize)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC Data Download Malloc Error. size = 0x%x\n", mallocSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Bin => ASCII変換
	if ((status = lfDpcDownloadBintoAscii ((void *)pAsciiBuff, (void *)DPC_MEMORY_ADRS, &downloadSize, mallocSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 圧縮あり？
	if (mode == UPDATE_COMPRESS)
	{
		// メモリリクエスト(圧縮データ格納用)
		if ((pCompressBuff = malloc (downloadSize + COMPRESS_DOWNLOAD_ADD_MEMORY)) == NULL)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Data Download Buffer Request Error\n");
			goto _DONE;
		}

		// 圧縮
		comprLen = downloadSize + COMPRESS_DOWNLOAD_ADD_MEMORY;
		if ((status = gzipComp ((unsigned char *)pAsciiBuff, downloadSize, (unsigned char *)pCompressBuff, &comprLen)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// アドレス&サイズ設定
		ptrSend = pCompressBuff;
		sendSize = comprLen;
		
		// ファイル名
		strcpy ((char *)fileName, DPC_SEND_FILE_COMPRESS_NAME);
	}
	else
	{
		// アドレス&サイズ設定
		ptrSend = (unsigned char *)pAsciiBuff;
		sendSize = downloadSize;
		
		// ファイル名
		strcpy ((char *)fileName, DPC_SEND_FILE_NAME);
	}

	// Kermit Send
	if ((kermitSendSize = kermitSend ((char *)fileName, (char *)ptrSend, sendSize)) < 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_IO);
		sprintf (gLogMsgBuff, "DPC Data Download kermit Error. Size = %d\n", kermitSendSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	if (pAsciiBuff != NULL)
		free (pAsciiBuff);

	if (pCompressBuff != NULL)
		free (pCompressBuff);

	return (status);
}


//**********************************************************************************
//	FFC Download Data Bin=>ASCII変換
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pAscii				：ASCIIデータを格納するポインタ
//		pBinary				：Binaryデータを格納するポインタ
//		pSize				：変換サイズを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int lfDpcDownloadBintoAscii (void *pAscii, void *pBinary, unsigned int *pSize, unsigned int mallocSize)
{
	int status = AVAL_STATUS_SUCCESS;
	int index;
	unsigned int i;
	unsigned int temp;
	unsigned int size;
	unsigned int data32;
	unsigned char *pAsciiBuffer = NULL;
	unsigned int adrs;
	int widthMax, heightMax;
	int x, y;
#if defined (MODE_IPU_MULTI)
	unsigned int offset;
	int xChange;
#endif

	// Check pAscii Parameter
	if (pAscii == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Download pAscii NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pBinary Parameter
	if (pBinary == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Download pBinary NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Download pSize NULL Parameter Error\n");
		goto _DONE;
	}

	// サイズ設定
	// ASCIIに変換するので2倍 + 改行。1画素を1bitでDPC制御する。
	// 欠陥画素＝画素数/ 8。+Acciiなのでx2。
	// 改行 = 画素数/ 8。また１行で16byte分表示するテキストファイルなので、さらに16で割る。+Acciiなのでx2。
	//size = ((IMG_WIDTH*IMG_HEIGHT)/8) * 2 + (((IMG_WIDTH*IMG_HEIGHT)/8) / 16) * 2;
	size = ((IMG_WIDTH*IMG_HEIGHT)/8) * 2;

	// サイズCheck
	if (size > mallocSize)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC Data Download Size Over Error. size = 0x%x\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Width Max
	widthMax = sensorWidth();

	// Height Max
	heightMax = sensorHeight();

	// Bin=>ASCII変換
	pAsciiBuffer = (unsigned char *)pAscii;
	index = 0;
	adrs = (unsigned int)pBinary;

	i = 0;
	for (y=0; y<heightMax; y++)
	{
		for (x=0; x<(widthMax/8); x++)
		{
			// 実際にメモリを使用しているカウントのCheck
			if (i > mallocSize)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
				sprintf (gLogMsgBuff, "DPC Data Download Save Data Size Over Error. size = 0x%x\n", mallocSize);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			//------------------------------------------------------------
			// 改行必要?
			//------------------------------------------------------------
			if (((index%16) == 0) && ((index != 0)))
			{
				pAsciiBuffer[i++] = 0x0d; // \r
				if (i>mallocSize)
					continue;
				pAsciiBuffer[i++] = 0x0a; // \n
				index = 1;
				//continue;
			}
			else
			{
				index++;
			}

			//------------------------------------------------------------
			// データ変換
			//------------------------------------------------------------
			#if !defined (MODE_IPU_MULTI)

			// データ取得
			data32 = (unsigned int)IN8((adrs + x));

			#else // #if defined (MODE_IPU_MULTI)

			// 分割アドレス取得
			if ((status = dpcGetDivAdrs (x*8, y, &xChange, &offset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// データ取得
			data32 = (unsigned int)IN8((adrs + offset));

			#endif // #if defined (MODE_IPU_MULTI)


			sprintf ((char *)&temp, "%02X", data32);

			// データ保存
			pAsciiBuffer[i++] = (unsigned char)(temp&0xff);
			if (i>mallocSize)
				continue;

			pAsciiBuffer[i++] = (unsigned char)((temp>>8)&0xff);
		}

	#if !defined (MODE_IPU_MULTI)
		adrs += DPC_WIDTH_DATA_ALIGH;
	#endif
	}

	// サイズ設定
	*pSize = i;

_DONE:
	return (status);
}


//**********************************************************************************
// 補正データを配置しなおし
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				： DPC番号
//		pData				： 補正データが格納されたポインタ
//		index				： 補正データが格納された個数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetMemory (int dpcNum, DEFECTIONINFO *pData, int index)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;
	unsigned int adrs;
	DEFECTIONINFO *ptr = pData;
	int bit;
	unsigned char data;
#if !defined (MODE_IPU_MULTI)
	int width;
	int xOffset, yOffset;
#endif
#if defined (MODE_IPU_MULTI)
	unsigned int offsetTemp;
	int xChange;
#endif

	// 1度この領域に展開後Flashに書き込み
	memset ((void *)DPC_MEMORY_ADRS, 0xff, DPC_MEMORY_NEW_ALL_SIZE);

	// DPCアドレス取得
	adrs = DPC_MEMORY_ADRS;

	if (index == 0)
	{
		// 欠陥画素補正数設定
		OUT32 (FIRM_DATA_DPC_NUM_ADRS, 0);
		goto _DONE;
	}

	// Get Width
#if !defined (MODE_IPU_MULTI)

	width = DPC_WIDTH_DATA_ALIGH;

#endif // #if !defined (MODE_IPU_MULTI)

	// DPCに設定する為のメモリへ設定
	for (i=0; i<index; i++, ptr++)
	{
		#if !defined (MODE_IPU_MULTI)

		// 座標をアドレスに変換
		yOffset = (ptr->y * width);

		// ビット毎に座標が割り付けられる
		xOffset = ptr->x / 8;
		bit = ptr->x % 8;

		// Set DPC Flag(0にすると欠陥)
		data = IN8 ((adrs + yOffset + xOffset));
		data &= ~(1<<bit);
		OUT8 ((adrs + yOffset + xOffset), data);


		#else // #if !defined (MODE_IPU_MULTI)


		// 分割アドレス取得
		if ((status = dpcGetDivAdrs (ptr->x, ptr->y, &xChange, &offsetTemp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// ビット毎に座標が割り付けられる
		bit = ptr->x % 8;

		// Set DPC Flag(0にすると欠陥)
		data = IN8 ((adrs + offsetTemp));
		data &= ~(1<<bit);
		OUT8 ((adrs + offsetTemp), data);

		#endif // #if !defined (MODE_IPU_MULTI)
	}

#if defined (MODE_IPU_MULTI)
	// ipuの1個目の最後の16画素の追加。
	// ipuの2個目の最初の16画素の追加。
	if ((status = dpcSetMarginGridData ()) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

_DONE:
	// キャッシュFlash
	cacheFlush ();

	return (status);
}


//**********************************************************************************
// DPCデータ終端検索
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：検索するアドレス
//		pIndex				： 補正データが格納された個数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if !defined (MODE_IPU_MULTI)
int dpcEndSearch (unsigned int adrs, int *pIndex)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char data;
	int bit;
	unsigned int index;
	int x, y;
	unsigned int memAdrs2;
	int widthMax, heightMax;

	// Check pIndex Parameter
	if (pIndex == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC End Search pIndex NULL Parameter Error\n");
		goto _DONE;
	}

	// Width Max
	widthMax = sensorWidth();

	// Height Max
	heightMax = sensorHeight();

	// メモリアドレス
	memAdrs2 = adrs;

	// 初期化
	index = 0;

	for (y=0; y<heightMax; y++)
	{
		for (x=0; x<(widthMax/8); x++)
		{
			// 0=欠陥画素/1=正常画素
			data = IN8 ((memAdrs2 + x));
			for (bit=0; bit<8; bit++)
			{
				if ((data & (1<<bit)) == 0)
					index++;
			}
		}

		memAdrs2 += DPC_WIDTH_DATA_ALIGH;
	}

	*pIndex = index;

_DONE:
	return (status);
}

#else //#if !defined (MODE_IPU_MULTI)

//**********************************************************************************
// DPCデータ終端検索
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：検索するアドレス
//		pIndex				： 補正データが格納された個数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcEndSearch (unsigned int adrs, int *pIndex)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char data;
	int bit;
	unsigned int index;
	int x, y;
	unsigned int offset;
	int xChange;
	
	// Check pIndex Parameter
	if (pIndex == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC End Search pIndex NULL Parameter Error\n");
		goto _DONE;
	}

	// 初期化
	index = 0;
	for (y=0; y<IMG_HEIGHT; y++)
	{
		for (x=0; x<(IMG_WIDTH / 8); x++)
		{
			// 分割アドレス取得
			if ((status = dpcGetDivAdrs (x*8, y, &xChange, &offset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			data = IN8 ((adrs + offset));
			for (bit=0; bit<8; bit++)
			{
				if ((data & (1<<bit)) == 0)
					index++;
			}
		}
	}

	*pIndex = index;

_DONE:
	return (status);
}
#endif // #if !defined (MODE_IPU_MULTI)


//**********************************************************************************
// DPCデータ終端検索
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		adrs				：検索するアドレス
//		pIndex				： 補正データが格納された個数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcEndSearch2 (unsigned int adrs, int *pIndex)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char data;
	int bit;
	unsigned int index;
	int x, y;
	unsigned int memAdrs2;
	int xPixel;
	int widthMax, heightMax;
#if defined (MODE_IPU_MULTI)
	unsigned int offset;
	int xChange;
#endif

	// Check pIndex Parameter
	if (pIndex == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC End Search pIndex NULL Parameter Error\n");
		goto _DONE;
	}

	// Width Max
	widthMax = sensorWidth();

	// Height Max
	heightMax = sensorHeight();

	// メモリアドレス
	memAdrs2 = adrs;

	// 初期化
	index = 0;
	for (y=0; y<heightMax; y++)
	{
		xPixel = 0;
		for (x=0; x<(widthMax/8); x++)
		{
			#if !defined (MODE_IPU_MULTI)

			// 0=欠陥画素/1=正常画素
			data = IN8 ((memAdrs2 + x));

			#else // #if !defined (MODE_IPU_MULTI)

			// 分割アドレス取得
			if ((status = dpcGetDivAdrs (x*8, y, &xChange, &offset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// 0=欠陥画素/1=正常画素
			data = IN8 ((memAdrs2 + offset));

			#endif // #if !defined (MODE_IPU_MULTI)

			for (bit=0; bit<8; bit++, xPixel++)
			{
				// Height上
				if (y < IMG_HEIGHT_OFFSET)
					continue;

				// Height下
				if (y >= (heightMax - IMG_HEIGHT_OFFSET))
					continue;

				// Width左端
				if (xPixel < IMG_WIDTH_OFFSET)
					continue;

				// Width右端
				if (xPixel >= (widthMax - IMG_WIDTH_OFFSET))
					continue;

				if ((data & (1<<bit)) == 0)
					index++;
			}
		}

#if !defined (MODE_IPU_MULTI)
		memAdrs2 += DPC_WIDTH_DATA_ALIGH;
#endif // #if !defined (MODE_IPU_MULTI)
	}

	*pIndex = index;

_DONE:
	return (status);
}


//**********************************************************************************
// 補正データをFlashに書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		memAdrs				：メモリアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcToFlash (unsigned int flashAdrs, unsigned int memAdrs)
{
	int status;
	
	status = dpcToFlashMain (flashAdrs, memAdrs);
	
	return (status);
}


//**********************************************************************************
// DPCオフセット/ゲインデータをFlashに書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		memAdrs				：オフセットデータが格納されたメモリアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcToFlashAdmin (unsigned int flashAdrs, unsigned int memAdrs)
{
	int status;
	
	status = dpcToFlashMain (flashAdrs, memAdrs);
	
	return (status);
}


//**********************************************************************************
// 補正データをFlashに書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：Flashアドレス
//		memAdrs				：メモリアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcToFlashMain (unsigned int flashAdrs, unsigned int memAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs, size;
	int index;
#if !defined (MODE_DPC_DATA_ALIGN_ADJUST)
	int dpcSize = DPC_MEMORY_NEW_SIZE;
#endif
	int dpcUnitSize = DPC_MEMORY_NEW_FLASH_UNIT_SIZE;
#if defined (MODE_SENSOR_XFLIP)
	int flipMode;
#endif
	unsigned int flashAdrs2, memAdrs2;
	int size2;

	// DPCデータ反転
#if defined (MODE_SENSOR_XFLIP)
	// Flip取得
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPCデータ反転
	if (flipMode == MODE_ENABLE)
	{
		if ((status = dpcDataXFlip (flipMode, memAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// Erase
	for (adrs=flashAdrs, size = 0; size<dpcUnitSize; adrs+=AXI_QSPI_FLASH_SEC_SIZE, size+=AXI_QSPI_FLASH_SEC_SIZE)
	{
		if ((status = qspiFlashSectorErase (adrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// 終端を検出
	if ((status = dpcEndSearch (memAdrs, &index)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check index Parameter
	if (index > NUM_DEFECTION_PIX)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Flash index(%d) Parameter Error. (Min:0 / Max:%d)\n", index, NUM_DEFECTION_PIX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	#if !defined (MODE_IPU_MULTI)
	int y;

	flashAdrs2 = flashAdrs;
	memAdrs2 = memAdrs;
	size2 = IMG_WIDTH/8;
	for (y=0; y<IMG_HEIGHT; y++)
	{
		if ((status = qspiFlashWrite (flashAdrs2, (unsigned char *)memAdrs2, size2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		flashAdrs2 += (IMG_WIDTH/8);
		memAdrs2 += DPC_WIDTH_DATA_ALIGH;
	}

	#else // #if !defined (MODE_IPU_MULTI)

	int ipu;

	flashAdrs2 = flashAdrs;
	memAdrs2 = memAdrs;
	size2 = DPC_WIDTH_DATA_ALIGH * IMG_HEIGHT;
	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		if ((status = qspiFlashWrite (flashAdrs2, (unsigned char *)memAdrs2, size2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		flashAdrs2 += size2;
		memAdrs2 += DPC_MEMORY_IPU_MULTI_INTERVAL;
	}

	#endif // #if !defined (MODE_IPU_MULTI)

	// DPCデータ反転
#if defined (MODE_SENSOR_XFLIP)
	// Flip取得
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

	// DPCデータ反転
	if (flipMode == MODE_ENABLE)
	{
		if ((status = dpcDataXFlip (flipMode, memAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//  補正データをメモリに書き込む
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：DPC Flashアドレス
//		memAdrs				：DPC メモリアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcToMemory (unsigned int flashAdrs, unsigned int memAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	status = dpcToMemoryMain (flashAdrs, memAdrs);

	return (status);
}


//**********************************************************************************
//  補正データをメモリに書き込む
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：DPC Flashアドレス
//		memAdrs				：DPC メモリアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcToMemoryAdmin (unsigned int flashAdrs, unsigned int memAdrs)
{
	int status = AVAL_STATUS_SUCCESS;

	status = dpcToMemoryMain (flashAdrs, memAdrs);

	return (status);
}


//**********************************************************************************
//  補正データをメモリに書き込む
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		flashAdrs			：DPC Flashアドレス
//		memAdrs				：DPC メモリアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcToMemoryMain (unsigned int flashAdrs, unsigned int memAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_SENSOR_XFLIP)
	int flipMode;
#endif
	int size = DPC_MEMORY_NEW_SIZE;
	unsigned int flashAdrs2;
	unsigned int memAdrs2;
	
	#if !defined (MODE_IPU_MULTI)
	int y;

	flashAdrs2 = flashAdrs;
	memAdrs2 = memAdrs;
	size = IMG_WIDTH/8;
	if ((IMG_WIDTH%8) != 0)
		size++;

	for (y=0; y<IMG_HEIGHT; y++)
	{
		if ((status = qspiFlashRead (flashAdrs2, (unsigned char *)memAdrs2, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		flashAdrs2 += (IMG_WIDTH/8);
		memAdrs2 += DPC_WIDTH_DATA_ALIGH;
	}

	#else // #if !defined (MODE_IPU_MULTI)

	int ipu;
	flashAdrs2 = flashAdrs;
	memAdrs2 = memAdrs;
	size = DPC_WIDTH_DATA_ALIGH * IMG_HEIGHT;

	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		if ((status = qspiFlashRead (flashAdrs2, (unsigned char *)memAdrs2, size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		flashAdrs2 += size;
		memAdrs2 += DPC_MEMORY_IPU_MULTI_INTERVAL;
	}

	// ipuの1個目の最後の16画素の追加。
	// ipuの2個目の最初の16画素の追加。
	if ((status = dpcSetMarginGridData()) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	#endif  // #if !defined (MODE_IPU_MULTI)

	// DPCデータ反転
#if defined (MODE_SENSOR_XFLIP)
	// Flip取得
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPCデータ反転
	if (flipMode == MODE_ENABLE)
	{
		if ((status = dpcDataXFlip (flipMode, memAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	// キャッシュFlash
	cacheFlush ();

_DONE:
	return (status);
}


//**********************************************************************************
// DPCデータ設定済みマーク設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		mark				：Markデータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetMarkFlash (int dpcNum, unsigned int mark)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int offset;
	unsigned int flashAdrs;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Set Mark dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// アドレス
	offset = (dpcNum * DPC_MEMORY_NEW_FLASH_UNIT_SIZE) + DPC_MEMORY_NEW_FLASH_UNIT_SIZE - 4;

	// Get Flash Address
	if ((status = dpcGetFlashAdrs (dpcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Write
	if ((status = qspiFlashWrite ((flashAdrs + offset), (unsigned char *)&mark, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// DPCデータ設定済みマーク設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		mark				：Markデータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetMarkFlashAdmin (int dpcNum, unsigned int mark)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int offset;
	unsigned int flashAdrs;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Set Mark dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// アドレス
	offset = (dpcNum * DPC_MEMORY_NEW_FLASH_UNIT_SIZE) + DPC_MEMORY_NEW_FLASH_UNIT_SIZE - 4;

	// Get Flash Address
	if ((status = dpcGetFlashAdrsAdmin (dpcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Write
	if ((status = qspiFlashWrite ((flashAdrs + offset), (unsigned char *)&mark, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// 画像補正データ設定済みマーク取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		pMark				：マークデータ（dpcサイズ）を格納するポインタ領域
//							  0xffffffffは無効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetMarkFlash (int dpcNum, unsigned int *pMark)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int offset;
	unsigned int flashAdrs;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Get Mark dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pMark Parameter
	if (pMark == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Mark pMark NULL Parameter Error.\n");
		goto _DONE;
	}

	// アドレス
	offset = (dpcNum * DPC_MEMORY_NEW_FLASH_UNIT_SIZE) + DPC_MEMORY_NEW_FLASH_UNIT_SIZE - 4;

	// Get Flash Address
	if ((status = dpcGetFlashAdrs (dpcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Mark取得
	if ((status = qspiFlashRead ((flashAdrs + offset), (unsigned char *)pMark, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// 画像補正データ設定済みマーク取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		pMark				：マークデータ（dpcサイズ）を格納するポインタ領域
//							  0xffffffffは無効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetMarkFlashAdmin (int dpcNum, unsigned int *pMark)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int offset;
	unsigned int flashAdrs;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Get Mark dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pMark Parameter
	if (pMark == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Mark pMark NULL Parameter Error.\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// アドレス
	offset = (dpcNum * DPC_MEMORY_NEW_FLASH_UNIT_SIZE) + DPC_MEMORY_NEW_FLASH_UNIT_SIZE - 4;

	// Get Flash Address
	if ((status = dpcGetFlashAdrsAdmin (dpcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Mark取得
	if ((status = qspiFlashRead ((flashAdrs + offset), (unsigned char *)pMark, 4)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// DPCコピー(QSPIからAXI QSPIへのコピー)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		srcDpcNum			：コピー元DPC番号
//		desDpcNum			：コピー先DPC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcCopyAdminToUser (int srcDpcNum, int desDpcNum)
{
	int status;
	unsigned int srcFlashAdrs, desFlashAdrs;
	unsigned char *pBuffer = NULL;
	GRID_XY *pGridBuffer = NULL;
	unsigned int mark;
	DPCINFO dpcInfo;
	int size;

	size = DPC_MEMORY_NEW_ALL_SIZE;

	// Memory Request
	if ((pBuffer = (unsigned char *)malloc(size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC Admin Copy Buffer Request. size = 0x%x.\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DPC Grid Buffer Memory Request
	if ((pGridBuffer = (GRID_XY *)malloc (DPC_IMPOSSIBLE_GRID_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC Admin Copy Buffer Request. size = 0x%x.\n", DPC_IMPOSSIBLE_GRID_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check srcDpcNum Parameter
	if ((srcDpcNum < DPC_NUMBER_MIN) || (srcDpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC Admin To User Copy Source dpcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", srcDpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check desDpcNum Parameter
	if ((desDpcNum < DPC_NUMBER_MIN) || (desDpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC Admin To User Destination dpcNo(%d) Parameter Error. (Min:%d / Max:%d)\n", desDpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 転送元Flashアドレス取得
	if ((status = dpcGetFlashAdrsAdmin (srcDpcNum, &srcFlashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 転送先Flashアドレス取得
	if ((status = dpcGetFlashAdrs (desDpcNum, &desFlashAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 転送元をFlashからメモリへ展開
	if ((status = dpcToMemoryAdmin (srcFlashAdrs, (unsigned int)pBuffer)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 転送先へ書き込み
	if ((status = dpcToFlash (desFlashAdrs, (unsigned int)pBuffer)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPCマーク取得
	if ((status = dpcGetMarkFlashAdmin (srcDpcNum, &mark)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPCマーク設定
	if ((status = dpcSetMarkFlash (desDpcNum, mark)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPC情報読み込み
	if ((status = dpcAdjustInfoRead (srcDpcNum, DPC_ADMIN, &dpcInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPC情報書き込み
	if ((status = dpcAdjustInfoSave (desDpcNum, DPC_USER, dpcInfo)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPC補正不可情報読み込み
	if ((status = dpcImpossibleGridRead (srcDpcNum, DPC_ADMIN, (unsigned short *)pGridBuffer)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPC補正不可情報書き込み
	if ((status = dpcImpossibleGridSave (desDpcNum, DPC_USER, (unsigned short *)pGridBuffer)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (pBuffer != NULL)
		free (pBuffer);

	if (pGridBuffer != NULL)
		free (pGridBuffer);

	return (status);
}


//**********************************************************************************
// DPC Default
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcDefault (void)
{
	int status;
	int startFlag = 0;

	// Start Status
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;


#if defined (MODE_SENSOR_XFLIP)
	// DPCのデータはFlipしない
	dpcSetXFlipFlag (MODE_DISABLE);
#endif

	// ユーザー領域の0番だけ特定の箇所をコピー
	if ((status = dpcCopyAdminToUser (DPC_NUM_USER_DEFAULT, 0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startFlag != 0)
		acquisitionStart ();

#if defined (MODE_SENSOR_XFLIP)
	dpcSetXFlipFlag (MODE_ENABLE);
#endif

	return (status);
}


//**********************************************************************************
// DPC Max Number
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pNum				：DPC最大補正個数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetPixelMaxNum (unsigned int *pNum)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pNum Parameter
	if (pNum == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Pixel Max Number pNum NULL Parameter Error\n");
		goto _DONE;
	}

	// 最大個数取得
	*pNum = NUM_DEFECTION_PIX;

_DONE:
	return (status);
}


//**********************************************************************************
// 欠陥座標情報取得(メモリ上から読み出し)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		pGrid				：座標データを格納するポインタ
//		pNum				：座標データ数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetGridMem (int dpcNum, GRID_XY *pGrid, int *pNum)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;
	GRID_XY *pGrid2;
	GRID_XY *pGrid2Org;
	int count;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Get Grid dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pGrid Parameter
	if (pGrid == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Grid pGrid NULL Parameter Error\n");
		goto _DONE;
	}

	// Check pNum Parameter
	if (pNum == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Grid pNum NULL Parameter Error\n");
		goto _DONE;
	}

	// 欠陥座標データ数
	if ((status = dpcGetDefectionCountVersion2 (&count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// アドレス設定
	pGrid2 = pGrid;
	pGrid2Org = dpcGetDefectionCountBuff;

	// 座標コピー
	for (i=0; i<count; i++, pGrid2++, pGrid2Org++)
	{
		pGrid2->x = pGrid2Org->x;
		pGrid2->y = pGrid2Org->y;
	}

	// 欠陥座標数
	*pNum = count;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC欠陥補正数最大値取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：DPC欠陥補正数最大値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetDefectionMaxCount (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Defection Max Count pCount NULL Parameter Error\n");
		goto _DONE;
	}

	// 欠陥画素最大数
	*pCount = NUM_DEFECTION_PIX;

_DONE:
	return (status);
}


//**********************************************************************************
// DPC 欠陥座標数設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：DPC欠陥個数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetDefectionCount (int count)
{
	int status = AVAL_STATUS_SUCCESS;

	// 欠陥画素補正数設定
	OUT32 (FIRM_DATA_DPC_NUM_ADRS, count);

	return (status);
}


//**********************************************************************************
// DPC 欠陥座標数取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：DPC欠陥個数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetDefectionCount (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Defection Count pCount NULL Parameter Error.\n");
		goto _DONE;
	}

	// 欠陥画素補正数取得
	*pCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);

_DONE:
	return (status);
}


#if defined (MODE_DPC_GRID_UPDATE)
//**********************************************************************************
// DPC 欠陥座標数設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：DPC欠陥個数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetDefectionCountVersion2 (int count)
{
	int status = AVAL_STATUS_SUCCESS;

	// 欠陥画素補正数設定
	OUT32 (FIRM_DATA_DPC_NUM_VERSION2_ADRS, count);

	return (status);
}


//**********************************************************************************
// DPC 欠陥座標数取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：DPC欠陥個数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetDefectionCountVersion2 (int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Defection Count pCount NULL Parameter Error.\n");
		goto _DONE;
	}

	// 欠陥画素補正数取得
	*pCount = IN32 (FIRM_DATA_DPC_NUM_VERSION2_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// DPC Map Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：DPC Map Info Mode
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetMapInfoMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check data Parameter
	if ((mode != MODE_DISABLE) && (mode != MODE_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Map Info Mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DPC Map Info Mode設定
	OUT32 (FIRM_DATA_DPC_MAPINFO_MODE_ADRS, mode);

_DONE:
	return (status);
}


//**********************************************************************************
// DPC Map Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pCount				：DPC Map Info Modeを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetMapInfoMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Map Info Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// DPC Map Info Mode取得
	*pMode = IN32 (FIRM_DATA_DPC_MAPINFO_MODE_ADRS);

_DONE:
	return (status);
}
#endif // #if defined (MODE_DPC_GRID_UPDATE)


//**********************************************************************************
// DPC座標取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		memAdrs				：DPCアドレス
//		dpcPtr				：DPC欠陥座標格納ポインタ
//		checkX				：今回登録するX座標。すでに登録されていれば、重複と判断
//		pCount				：重複と判断された座標の個数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetGridLine (unsigned int memAdrs, unsigned char *dpcPtr, int checkX, int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;
	int dpcCount;
	int x, y;
	int xCount;
	unsigned int data;
	int bit;
	int dCount;
#if defined (MODE_IPU_MULTI)
	int xDiv, ipu;
#endif

	// Check dpcPtr Parameter
	if (dpcPtr == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Grid dpcPtr NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Grid pCount NULL Parameter Error.\n");
		goto _DONE;
	}

	// 欠陥画素補正数
	dpcCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);
	if (dpcCount == 0)
		goto _DONE;

	// 初期化
	dCount = 0;

#if !defined (MODE_IPU_MULTI)

	// 欠陥座標検索
	for (y=0; y<IMG_HEIGHT; y++)
	{
		// DPCアドレス取得
		adrs = (unsigned int)memAdrs + y * DPC_WIDTH_DATA_ALIGH;

		xCount = 0;
		for (x=0; x<IMG_WIDTH; x+=8, adrs++)
		{
			// 欠陥データ取得
			data = IN8 (adrs);

			for (bit=0; bit<8; bit++)
			{
				if ((data&(1<<bit)) == 0)
				{
					*(dpcPtr + xCount + y * IMG_STRIDE) = 1;
					if (checkX == xCount)
						dCount++;
				}

				xCount++;
			}	// for (bit=0; bit<8; bit++)
		}	// for (width=0; width<IMG_WIDTH; width+=8, adrs++)
	}	// for (height=0; height<IMG_HEIGHT; height++)

#else // #if !defined (MODE_IPU_MULTI)

	xDiv = IMG_WIDTH / IPU_COUNT;

	// 欠陥座標検索
	for (y=0; y<IMG_HEIGHT; y++)
	{
		for (ipu=0; ipu < IPU_COUNT; ipu++)
		{
			// DPCアドレス取得
			adrs = (unsigned int)memAdrs + y * DPC_WIDTH_DATA_ALIGH + DPC_MEMORY_IPU_MULTI_INTERVAL * ipu;

			if (ipu == 0)
			{
				xCount = 0;
			}
			else
			{
				xCount = xDiv * ipu;
				adrs += (IMG_WIDTH_OFFSET/8);
			}

			for (x=0; x<xDiv; x+=8, adrs++)
			{
				// 欠陥データ取得
				data = IN8 (adrs);

				for (bit=0; bit<8; bit++)
				{
					if ((data&(1<<bit)) == 0)
					{
						*(dpcPtr + xCount + y * IMG_STRIDE) = 1;
						if (checkX == xCount)
							dCount++;
					}

					xCount++;
				}	// for (bit=0; bit<8; bit++)
			}	// for (width=0; width<IMG_WIDTH; width+=8, adrs++)
		}	// for (ipu=0; ipu < IPU_COUNT; ipu++)
	}	// for (height=0; height<IMG_HEIGHT; height++)

#endif // #if !defined (MODE_IPU_MULTI)

	// 重複と判断された欠陥の個数
	*pCount = dCount;

_DONE:
	return (status);
}


//**********************************************************************************
// DPC座標取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		memAdrs				：DPCアドレス
//		index				：DPCインデックス
//		pX					：X座標を格納するポインタ
//		pY					：Y座標を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetGrid (unsigned int memAdrs, int index, int *pX, int *pY)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int dpcMaxCount;
	unsigned int adrs;
	int dpcCount;
	int width, height;
	int hit;
	unsigned int data;
	int bit;
	int find;
#if defined (MODE_IPU_MULTI)
	int xDiv, ipu;
#endif

	// 欠陥画素最大補正数
	dpcMaxCount = IN32 (FIRM_DATA_DPC_MAX_NUM_COUNT_ADRS);

	// Check index Parameter
	if ((index < 0) || (index >= dpcMaxCount))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Get Grid index(%d) Parameter Error.(Min:0 / Max:%d)\n", index, dpcMaxCount);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pX Parameter
	if (pX == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Grid pX NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pY Parameter
	if (pY == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Grid pY NULL Parameter Error.\n");
		goto _DONE;
	}

	// 欠陥画素補正数
	dpcCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);
	if (dpcCount == 0)
	{
		*pX = -1;
		*pY = -1;
		goto _DONE;
	}

	// Search Index
	hit = 0;
	find = 0;

#if !defined (MODE_IPU_MULTI)

	for (height=0; height<IMG_HEIGHT; height++)
	{
		// DPCアドレス取得
		adrs = (unsigned int)memAdrs + height * DPC_WIDTH_DATA_ALIGH;

		for (width=0; width<IMG_WIDTH; width+=8, adrs++)
		{
			// 欠陥データ取得
			data = IN8 (adrs);

			for (bit=0; bit<8; bit++)
			{
				if ((data&(1<<bit)) == 0)
				{
					// 欠陥Index一致?
					if (hit == index)
					{
						*pX = width + bit;
						*pY = height;
						find = 1;
						goto _NEXT;
					}

					hit++;
				}
			}	// for (bit=0; bit<8; bit++)
		}	// for (width=0; width<IMG_WIDTH; width+=8, adrs++)
	}	// for (height=0; height<IMG_HEIGHT; height++)

#else // #if !defined (MODE_IPU_MULTI)

	xDiv = IMG_WIDTH / IPU_COUNT;
	for (height=0; height<IMG_HEIGHT; height++)
	{
		for (ipu=0; ipu < IPU_COUNT; ipu++)
		{
			// DPCアドレス取得
			adrs = (unsigned int)memAdrs + height * DPC_WIDTH_DATA_ALIGH + DPC_MEMORY_IPU_MULTI_INTERVAL * ipu;

			for (width=0; width<xDiv; width+=8, adrs++)
			{
				// 欠陥データ取得
				data = IN8 (adrs);

				for (bit=0; bit<8; bit++)
				{
					if ((data&(1<<bit)) == 0)
					{
						// 欠陥Index一致?
						if (hit == index)
						{
							*pX = width + bit;
							*pY = height;
							find = 1;
							goto _NEXT;
						}

						hit++;
					}
				}	// for (bit=0; bit<8; bit++)
			}	// for (width=0; width<IMG_WIDTH; width+=8, adrs++)
		}	// for (ipu=0; ipu < IPU_COUNT; ipu++)
	}	// for (height=0; height<IMG_HEIGHT; height++)

#endif // #if !defined (MODE_IPU_MULTI)


_NEXT:

	// 発見できず？
	if (find == 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_NO_DATA);
		sprintf (gLogMsgBuff, "DPC Get Grid Not Found Error.\nDPC Count = %d\nDPC Index = %d\n", IN32 (FIRM_DATA_DPC_NUM_ADRS), index);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// DPC Add Grid
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：x座標
//		y					：y座標
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcAddGrid (int x, int y)
{
	int status = AVAL_STATUS_SUCCESS;

	// 座標変換
	if ((status = roiCoordinateTrans(x, y, &x, &y, TRANS_MODE_ADD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 座標追加
	if ((status = dpcGridMain (x, y, DPC_GRID_MODE_ADD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// DPC Delete Grid
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：x座標
//		y					：y座標
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcDeleteGrid (int x, int y)
{
	int status = AVAL_STATUS_SUCCESS;

	// 座標変換
	if ((status = roiCoordinateTrans(x, y, &x, &y, TRANS_MODE_ADD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 座標削除
	if ((status = status = dpcGridMain (x, y, DPC_GRID_MODE_DELETE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
_DONE:
	return (status);
}


//**********************************************************************************
// DPC Grid Add/Delete
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：x座標
//		y					：y座標
//		mode				：0=削除/1=追加
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGridMain (int x, int y, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int dpcCount;
	//unsigned int dpcMaxCount;
	unsigned int dpcAdrs, size;
	int dpcNo = 0;
	int flipMode;
	int width, height;
	unsigned char *ptr;
	int acquisitionFlag = 0;
	int dpcMode;
	unsigned int dpcFpgaMode;
	unsigned int adrs, offset;
	unsigned char data8;
	int xBitAma;
	unsigned char xBit;
#if defined (MODE_DPC_GRID_UPDATE)
	int count;
#endif
#if defined (MODE_IPU_MULTI)
	int xChange;
	int xLeft = -1, xRight = -1;
	int xOffset;
#endif

	// Width Max
	if ((status = roiGetAreaWidthMax (&width)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	if ((status = roiGetAreaHeightMax (&height)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check x Parameter
	if ((x < 0) || (x >= width))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Grid x(%d) Parameter Error.(Min:0 / Max:%d)\n", x, width-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check y Parameter
	if ((y < 0) || (y >= height))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Grid y(%d) Parameter Error.(Min:0 / Max:%d)\n", y, height-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check mode Parameter
	if ((mode != DPC_GRID_MODE_ADD) && (mode != DPC_GRID_MODE_DELETE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Grid Mode(%d) Parameter Error.(Add:%d / Delete:%d)\n", mode, DPC_GRID_MODE_ADD, DPC_GRID_MODE_DELETE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 欠陥画素補正数
	dpcCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);
	if (dpcCount == 0)
		dpcCount = 1;

	// 欠陥画素最大補正数
	//dpcMaxCount = IN32 (FIRM_DATA_DPC_MAX_NUM_COUNT_ADRS);

	// Get Memory Address
	if ((status = dpcGetMemAdrs (dpcNo, &dpcAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Flip
#if !defined (MODE_SENSOR_XFLIP)
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	flipMode = XFLIP_DISABLE;
#endif

	// Acquisition Start Flag
	if ((status = acquisitionGetStartFlag (&acquisitionFlag)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Stop
	acquisitionAbort ();

	// 欠陥情報格納領域クリア
	ptr = pImageDetect[0];

	// ROI座標Check
	if ((status = roiGetAreaGridOffset (&x, &y)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// バッファクリア
	memset ((void *)ptr, 0x00, (IMG_WIDTH * IMG_HEIGHT));

	// Flip有効?
#if !defined (MODE_SENSOR_X_REVERSE)
	if (flipMode == XFLIP_ENABLE)
		x = width - 1 - x;
#else
	if (flipMode == XFLIP_DISABLE)
		x = width - 1 - x;
#endif

	// 欠陥座標Adrs取得
#if !defined (MODE_IPU_MULTI)

	offset = x / 8 + DPC_WIDTH_DATA_ALIGH * y;

#else

	// 分割アドレス取得
	if ((status = dpcGetDivAdrs (x, y, &xChange, &offset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 隠し座標の確認
	if ((status = hideGridCalc (x, y, &xLeft, &xRight)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標変換
	x = xChange;

#endif

	adrs = dpcAdrs + offset;
	xBitAma = x % 8;
	xBit = (1<<xBitAma);

	// 欠陥座標追加
	//data8 = IN32 (adrs);
	data8 = IN8 (adrs);
	if (mode == DPC_GRID_MODE_ADD)
	{
		if ((data8 & xBit) == xBit)
		{
			// 欠陥座標追加
			data8 &= ~xBit;
			OUT8 (adrs, data8);

			// 現在の欠陥画素補正数を取得
			dpcCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);
			dpcCount += 1;

			// 欠陥画素補正数設定
			OUT32 (FIRM_DATA_DPC_NUM_ADRS, dpcCount);

			//------------------------------------------------------------
			// DPC 隠し座標設定
			//------------------------------------------------------------

			//--------------------------------------------------------------------------------
			// IPU=0
			// ABA-052VIR：左側の1280～1295の領域を書き換えたら、右側の0～15(1296～1311)の領域も書き換える
			// 
			// IPU=1
			// ABA-052VIR：右側の0～15(1328～1343)の領域を書き換えたら、左側の1296～1311の領域も書き換える
			//--------------------------------------------------------------------------------
			#if defined (MODE_IPU_MULTI)
			if ((xRight != -1) || (xLeft != -1))
			{
				if (xRight != -1)
				{
					adrs = dpcAdrs + y * DPC_WIDTH_DATA_ALIGH + DPC_MEMORY_IPU_MULTI_INTERVAL;
					
					xOffset = xRight / 8;
					xBitAma = xRight % 8;
				}
				else if (xLeft != -1)
				{
					adrs = dpcAdrs + y * DPC_WIDTH_DATA_ALIGH;
					adrs += (IMG_WIDTH / IPU_COUNT / 8);

					xOffset = xLeft / 8;
					xBitAma = xLeft % 8;
				}
				else
				{
					goto _NEXT;
				}

				xBit = (1<<xBitAma);
				adrs += xOffset;

				// 欠陥座標追加
				data8 = IN8 (adrs);
				data8 &= ~xBit;
				OUT8 (adrs, data8);
			}
			#endif // #if defined (MODE_IPU_MULTI)
		} // if ((data8 & xBit) == xBit)
	} // if (mode == DPC_GRID_MODE_ADD)
	else
	{
		if ((data8 & xBit) == 0)
		{
			// 欠陥座標削除
			data8 |= xBit;
			OUT8 (adrs, data8);

			// 現在の欠陥画素補正数を取得
			dpcCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);
			if (dpcCount != 0)
			{
				dpcCount -= 1;

				// 欠陥画素補正数設定
				OUT32 (FIRM_DATA_DPC_NUM_ADRS, dpcCount);
			}

			//------------------------------------------------------------
			// DPC 隠し座標設定
			//------------------------------------------------------------

			//--------------------------------------------------------------------------------
			// IPU=0
			// ABA-052VIR：左側の1280～1295の領域を書き換えたら、右側の0～15(1296～1311)の領域も書き換える
			// 
			// IPU=1
			// ABA-052VIR：右側の0～15(1328～1343)の領域を書き換えたら、左側の1296～1311の領域も書き換える
			//--------------------------------------------------------------------------------
			#if defined (MODE_IPU_MULTI)
			if ((xRight != -1) || (xLeft != -1))
			{
				if (xRight != -1)
				{
					adrs = dpcAdrs + y * DPC_WIDTH_DATA_ALIGH + DPC_MEMORY_IPU_MULTI_INTERVAL;
					
					xOffset = xRight / 8;
					xBitAma = xRight % 8;
				}
				else if (xLeft != -1)
				{
					adrs = dpcAdrs + y * DPC_WIDTH_DATA_ALIGH;
					adrs += (IMG_WIDTH / IPU_COUNT / 8);

					xOffset = xLeft / 8;
					xBitAma = xLeft % 8;
				}
				else
				{
					goto _NEXT;
				}

				xBit = (1<<xBitAma);
				adrs += xOffset;

				// 欠陥座標追加
				data8 = IN8 (adrs);
				data8 |= xBit;
				OUT8 (adrs, data8);
			}
			#endif // #if defined (MODE_IPU_MULTI)
		} // if ((data8 & xBit) == 0)
	} // if (mode == DPC_GRID_MODE_ADD)

#if defined (MODE_IPU_MULTI)
_NEXT:
#endif
	//------------------------------------------------------------
	// DPC Enable
	//------------------------------------------------------------

	// 1度DPC Disable
	if ((status = dpcGetEnableMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		dpcMode = MODE_DISABLE;

	dpcSetEnableMode (MODE_DISABLE);

	usDelay (1000);	// wait 1ms

	// DPCパラメータ初期化 & Enable
	size = DPC_MEMORY_NEW_SIZE;

	if (dpcMode == MODE_ENABLE)
		dpcFpgaMode = FPGA_DPC_CTRL_ENABLE_BIT;
	else
		dpcFpgaMode = 0;

	dpcRegInit (dpcAdrs, size, dpcFpgaMode);


	//------------------------------------------------------------
	// DPC Map情報更新
	//------------------------------------------------------------
#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map情報更新
	if ((status = dpcGetMapInfo ((unsigned char *)dpcAdrs, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標登録
	dpcSetDefectionCountVersion2 (count);
#endif

_DONE:
	if (acquisitionFlag == 1)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// DPC Add Grid Line
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：x座標
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcAddGridLine (int x)
{
	int status = AVAL_STATUS_SUCCESS;

	status = dpcGridLineMain (x, DPC_GRID_MODE_ADD);
	return (status);
}


//**********************************************************************************
// DPC Delete Grid Line
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：x座標
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcDeleteGridLine (int x)
{
	int status = AVAL_STATUS_SUCCESS;

	status = dpcGridLineMain (x, DPC_GRID_MODE_DELETE);
	return (status);
}


//**********************************************************************************
// DPC Grid Line Add/Delete
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：x座標
//		mode				：0=削除/1=追加
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGridLineMain (int x, int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int dpcCount, dpcCount2;
	unsigned int dpcAdrs, size;
	int index;
	int dpcNo = 0;
	int flipMode;
	int width;
	unsigned char *ptr;
	int y, yMax;
	int acquisitionFlag = 0;
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	int tempY;
#endif
	int sameCount;
#if defined (MODE_ROI_VERSION2)
	int areaMode;
#endif
	unsigned char data8;
#if defined (MODE_DPC_GRID_UPDATE)
	int count;
#endif

	// Width Max
	if ((status = roiGetAreaWidthMax (&width)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 座標変換
	if ((status = roiCoordinateTrans (x, y, &x, &y, TRANS_MODE_ADD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check x Parameter
	if ((x < 0) || (x >= width))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Grid x(%d) Parameter Error.(Min:0 / Max:%d)\n", x, width-1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check mode Parameter
	if ((mode != DPC_GRID_MODE_ADD) && (mode != DPC_GRID_MODE_DELETE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Grid Mode(%d) Parameter Error.(Add:%d / Delete:%d)\n", mode, DPC_GRID_MODE_ADD, DPC_GRID_MODE_DELETE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 欠陥画素補正数
	dpcCount = IN32 (FIRM_DATA_DPC_NUM_ADRS);
	if (dpcCount == 0)
		dpcCount = 1;

	// Get Memory Address
	if ((status = dpcGetMemAdrs (dpcNo, &dpcAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Flip
#if !defined (MODE_SENSOR_XFLIP)
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	flipMode = XFLIP_DISABLE;
#endif

	// Acquisition Start Flag
	if ((status = acquisitionGetStartFlag (&acquisitionFlag)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Stop
	acquisitionAbort ();

	// 欠陥情報格納領域クリア
	ptr = pImageDetect[0];

	// Check Area Mode
	if ((status = roiGetAreaFlag (&areaMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (areaMode == ROI_AREA_MODE_DEFAULT_SIZE)
	{
		tempY = 0;
		if ((status = roiGetAreaGridOffset (&x, &tempY)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	memset ((void *)ptr, 0x00, (IMG_WIDTH * IMG_HEIGHT));
#else
	memset ((void *)ptr, 0x00, (WidthMax() * HeightMax()));
#endif

	// Flip有効?
#if !defined (MODE_SENSOR_X_REVERSE)
	if (flipMode == XFLIP_ENABLE)
		x = width - 1 - x;
#else
	if (flipMode == XFLIP_DISABLE)
		x = width - 1 - x;
#endif

	// Search Current Grid
	sameCount = 0;

	// 現在登録されている座標をMapに反映
	if ((status = dpcGetGridLine (dpcAdrs, ptr, x, &sameCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥画素補正数取得
	dpcCount2 = IN32 (FIRM_DATA_DPC_NUM_ADRS);

	// ライン座標追加or削除
	yMax = IMG_HEIGHT;
	if (mode == DPC_GRID_MODE_ADD)
	{
		for (y=0; y<yMax; y++)
		{
			if (dpcCount2 >= NUM_DEFECTION_PIX)
				break;

			*(ptr + x + y * IMG_STRIDE) = 1;
			dpcCount2++;
		}

		dpcCount2 -= sameCount;
	}
	else if (mode == DPC_GRID_MODE_DELETE)
	{
		for (y=0; y<yMax; y++)
		{
			if (dpcCount2 <=0)
				break;

			data8 = *(ptr + x + y * IMG_STRIDE);
			if (data8 == 1)
			{
				*(ptr + x + y * IMG_STRIDE) = 0;
				dpcCount2--;
			}
		}
	}

	//------------------------------------------------------------
	// 画像欠陥補正
	//------------------------------------------------------------

	// 補正不可データクリア
	//memset ((void *)pDpcImpossibleGrid, 0xff, DPC_IMPOSSIBLE_GRID_SIZE);

	if (mode == DPC_GRID_MODE_ADD)
	{
		// 補正できなければ終了するモード
		gDpcGridImpossibleMode = 1;
		gDpcGridImpossibleStatus = AVAL_STATUS_SUCCESS;
	}

	// 欠陥補正標準モード
	if ((status = get_correct_coord (pImageDetect[0], pImageDefectionInfo, &index)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == DPC_GRID_MODE_ADD)
	{
		// 欠陥座標は登録できた？
		if (gDpcGridImpossibleStatus != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Grid Register Error\n");
			goto _DONE;
		}
	}

	//------------------------------------------------------------
	// 補正データをメモリへ配置
	//------------------------------------------------------------
	dpcSetMemory (dpcNo, pImageDefectionInfo, index);


	//------------------------------------------------------------
	// DPC Enable
	//------------------------------------------------------------

	// 1度DPC Disable
	if ((status = dpcSetEnableMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	usDelay (1000);	// wait 1ms

	// DPCパラメータ初期化 & Enable
	size = DPC_MEMORY_NEW_SIZE;
	dpcRegInit (dpcAdrs, size, FPGA_DPC_CTRL_ENABLE_BIT);

	// 欠陥画素補正数設定
	OUT32 (FIRM_DATA_DPC_NUM_ADRS, dpcCount2);


	//------------------------------------------------------------
	// DPC Map情報更新
	//------------------------------------------------------------
#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map情報更新
	if ((status = dpcGetMapInfo ((unsigned char *)dpcAdrs, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標登録
	dpcSetDefectionCountVersion2 (count);
#endif

_DONE:
	if (acquisitionFlag == 1)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// DPC Grid Clear
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				:DPC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcClearGrid (int dpcNum)
{
	int status = AVAL_STATUS_SUCCESS;
	int index = 0;
	int size;
#if defined (MODE_DPC_GRID_UPDATE)
	int count;
#endif

	//------------------------------------------------------------
	// 補正データをメモリへ配置
	//------------------------------------------------------------
	dpcSetMemory (dpcNum, pImageDefectionInfo, index);

	//------------------------------------------------------------
	// DPC Enable
	//------------------------------------------------------------
	// DPCパラメータ初期化 & Enable
	size = DPC_MEMORY_NEW_SIZE;
	dpcRegInit (DPC_MEMORY_ADRS, size, FPGA_DPC_CTRL_ENABLE_BIT);


	//------------------------------------------------------------
	// 欠陥座標更新
	//------------------------------------------------------------
#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map情報取得
	if ((status = dpcGetMapInfo ((unsigned char *)DPC_MEMORY_ADRS, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標数登録
	dpcSetDefectionCountVersion2 (count);

_DONE:
#endif

	return (status);
}


//**********************************************************************************
// 画像補正用情報表示(メモリに配置されたものを表示)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		userMode			：ユーザーモード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcMapInfoFlash (int dpcNum, int userMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *pBuffer = NULL;
	unsigned int mark;
	unsigned int flashAdrs;
	int mallocSize;

	// Check dpcNum Parameter
	if ((dpcNum < DPC_NUMBER_MIN) || (dpcNum > DPC_NUMBER_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Map Information dpcNum(%d) Parameter Error.(Min:%d / Max:%d)\n", dpcNum, DPC_NUMBER_MIN, DPC_NUMBER_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check userMode Parameter
	if ((userMode != DPC_USER) && (userMode != DPC_ADMIN))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Map Information User(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, DPC_USER, DPC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	mallocSize = DPC_MEMORY_NEW_UNIT_SIZE;

	// Memory Request
	if ((pBuffer = (unsigned char *)malloc(mallocSize)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC Map Buffer Request Error. size = 0x%x.\n", mallocSize);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if (userMode == DPC_USER)
	{
		// 転送元Flashアドレス取得
		if ((status = dpcGetFlashAdrs (dpcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 転送元をFlashからメモリへ展開
		if ((status = dpcToMemory (flashAdrs, (unsigned int)pBuffer)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC Mark取得
		if ((status = dpcGetMarkFlash (dpcNum, &mark)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if (userMode == DPC_ADMIN)
	{
		// 転送元Flashアドレス取得
		if ((status = dpcGetFlashAdrsAdmin (dpcNum, &flashAdrs)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 転送元をFlashからメモリへ展開
		if ((status = dpcToMemoryAdmin (flashAdrs, (unsigned int)pBuffer)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC Mark取得
		if ((status = dpcGetMarkFlashAdmin (dpcNum, &mark)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Map Information User(%d) Parameter Error.(User:%d / Admin:%d)\n", userMode, DPC_USER, DPC_ADMIN);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 欠陥補正なし
	if ((mark == DPC_NO_DATA) || (mark == 0))
	{
		goto _DONE;
	}

#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map Mode設定(0:通常バッファ/1:別バッファにmap情報格納)
	dpcSetMapInfoMode (MODE_ENABLE);
#endif

	// DPC情報表示
	if ((status = dpcMapInfoShow ((unsigned char *)pBuffer)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:

#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map Mode設定(0:通常バッファ/1:別バッファにmap情報格納)
	dpcSetMapInfoMode (MODE_DISABLE);
#endif

	if (pBuffer != NULL)
		free (pBuffer);

	return (status);
}


//**********************************************************************************
// DPC Map情報表示
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pAdrs				：DPCアドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcMapInfoShow (unsigned char *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;
	GRID_XY *ptrGrid;
	int count;
	int x, y;
#if defined (MODE_DPC_GRID_UPDATE)
	int dpcMapMode;
#endif

	// DPC Map情報取得
	if ((status = dpcGetMapInfo (pAdrs, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map Mode取得(0:通常バッファ/1:別バッファにmap情報格納)
	if ((status = dpcGetMapInfoMode (&dpcMapMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥座標数登録
	if (dpcMapMode == MODE_DISABLE)
		dpcSetDefectionCountVersion2 (count);
#endif

	DEBUG_PRINT_FORCE ("[DPC Map Grid]\n");

	// 表示
#if defined (MODE_DPC_GRID_UPDATE)
	if (dpcMapMode == MODE_DISABLE)
		ptrGrid = dpcGetDefectionCountBuff;
	else
		ptrGrid = dpcGetDefectionCountBuff2;
#else
	ptrGrid = dpcGetDefectionCountBuff;
#endif

	for (i=0; i<count; i++, ptrGrid++)
	{
		x = ptrGrid->x;
		y = ptrGrid->y;
		DEBUG_PRINT_FORCE ("%3d :  ", i+1);
		DEBUG_PRINT_FORCE ("x = %3d / ", x);
		DEBUG_PRINT_FORCE ("y = %3d\n", y);
	}

_DONE:
	return (status);
}


//**********************************************************************************
// DPC Map Index座標取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：欠陥座標のindex
//		pX					：x座標を格納するポインタ
//		pY					：y座標を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetMapInfoIndex2 (int index, int *pX, int *pY)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int memAdrs, size, dmaWait;
	int mode;
	GRID_XY *ptrGrid;
	int number, numberIndex;
	int count;
	int x, y;
	
	// 欠陥画素補正数設定
	number = IN32 (FIRM_DATA_DPC_NUM_ADRS);

	if (number == 0)
		numberIndex = 0;
	else
		numberIndex = number - 1;

	if (gInterFaceID == INTERFACE_CAMERALINK)
	{
		// Check index Parameter
		if ((index < 0) || (index > numberIndex) || (number == 0))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "DPC Map Info Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, 0, numberIndex);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
	else
	{
		*pX = 0;
		*pY = 0;
	}

	// Check pX Parameter
	if (pX == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Map Info pX NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pY Parameter
	if (pY == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Map Info pY NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get DPC parameter
	if ((status = dpcGetParam (&memAdrs, &size, &dmaWait, &mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPC Map情報取得
#if defined (MODE_DPC_GRID_UPDATE)
	// 欠陥座標数取得
	if ((status = dpcGetDefectionCountVersion2 (&count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	// DPC Map情報更新
	if ((status = dpcGetMapInfo ((unsigned char *)memAdrs, &count)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 従来のものはカウント設定無し
#endif

	if (gInterFaceID == INTERFACE_CAMERALINK)
	{
		// Check DPC Count
		if (index > (count-1))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "DPC Map Info Count Parameter Error. Index = %d, Count = %d\n", index, count-1);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// 座標取得
		ptrGrid = dpcGetDefectionCountBuff + index;
		x = ptrGrid->x;
		y = ptrGrid->y;
	}
	else
	{
		// Check DPC Count
		if (index > (count-1))
		{
			x = 0;
			y = 0;
		}
		else
		{
			// 座標取得
			ptrGrid = dpcGetDefectionCountBuff + index;
			x = ptrGrid->x;
			y = ptrGrid->y;
		}
	}

	// 座標変換
	if ((status = roiCoordinateTrans (x, y, &x, &y, TRANS_MODE_SUB)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	*pX = x;
	*pY = y;

_DONE:
	return (status);
}


//**********************************************************************************
// DPC Map情報の取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pAdrs				：DPCアドレスを格納するポインタ
//		pCount				：DPC欠陥個数を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetMapInfo (unsigned char *pAdrs, int *pCount)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int adrs;
	unsigned int j;
	int flipMode;
	int width, height;
	int widthMax, heightMax;
	int offsetX, offsetY;
	unsigned int saveAdrs, saveSize;
	GRID_XY *ptrGrid;
	int count;
	int x, y;
	int yOld;
	int xCount;
	unsigned short *ptrXSave  = (unsigned short *)pImageSrc;
	int hit;
	unsigned int data;
	int bit;
	int xPixel;
	unsigned int memadrs2;
	int ix, iy;
	int areaMode;
#if defined (MODE_DPC_GRID_UPDATE)
	int dpcMapMode/*, dpcMapFlashMode*/;
#endif
#if defined (MODE_IPU_MULTI)
	int xDiv = IMG_WIDTH / IPU_COUNT;
	int ipu;
#endif

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Map Info pAdrs NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pCount Parameter
	if (pCount == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Map Info pCount NULL Parameter Error.\n");
		goto _DONE;
	}

	// ROI Area Mode
	if ((status = roiGetAreaFlag (&areaMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Width
	if (areaMode == ROI_AREA_MODE_DEFAULT_SIZE)
		width = WidthMax();
	else
		width = sensorWidth();

	// Height
	height = HeightMax();

	// Width Max
	widthMax = sensorWidth();

	// Height Max
	heightMax = sensorHeight();

	// OffsetX
	offsetX = IMG_WIDTH_OFFSET;

	// OffsetY
	offsetY = IMG_HEIGHT_OFFSET;

	// Get Flip
#if !defined (MODE_SENSOR_XFLIP)
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	flipMode = XFLIP_DISABLE;
#endif

	// Clear
	saveSize = sizeof(GRID_XY) * NUM_DEFECTION_PIX;

#if defined (MODE_DPC_GRID_UPDATE)
	// DPC Map Mode取得(0:通常バッファ/1:別バッファにmap情報格納)
	if ((status = dpcGetMapInfoMode (&dpcMapMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMapMode == MODE_DISABLE)
		saveAdrs = (unsigned int)dpcGetDefectionCountBuff;
	else
		saveAdrs = (unsigned int)dpcGetDefectionCountBuff2;
#else
	saveAdrs = (unsigned int)dpcGetDefectionCountBuff;
#endif

	// バッファクリア
	memset ((void *)saveAdrs, 0xff, saveSize);

	// 並び替え
	adrs = (unsigned int)pAdrs;

	// アドレス取得
#if defined (MODE_DPC_GRID_UPDATE)
	if (dpcMapMode == MODE_DISABLE)
		ptrGrid = dpcGetDefectionCountBuff;
	else
		ptrGrid = dpcGetDefectionCountBuff2;
#else
	ptrGrid = dpcGetDefectionCountBuff;
#endif

	count = 0;
	xCount = 0;
	yOld = 0;

	for (iy=0; iy<heightMax; iy++)
	{
		// 全領域対象?
		if (areaMode == ROI_AREA_MODE_DEFAULT_SIZE)
		{
			// Height上
			if (iy < offsetY)
				continue;

			// Height下
			if (iy >= (heightMax - offsetY))
				continue;
		}

		memadrs2 = adrs + (iy * DPC_WIDTH_DATA_ALIGH);

		xPixel = 0;

#if !defined (MODE_IPU_MULTI)
		for (ix=0; ix<(widthMax/8); ix++)
		{
#else
		for (ipu=0; ipu < IPU_COUNT; ipu++)
		{
			memadrs2 = adrs + (iy * DPC_WIDTH_DATA_ALIGH) + DPC_MEMORY_IPU_MULTI_INTERVAL * ipu;

#if defined (MODE_IPU_MULTI)
			if (ipu != 0)
				memadrs2 += IMG_WIDTH_OFFSET / 8;
#endif

			for (ix=0; ix<(xDiv/8); ix++)
			{
#endif

			// 0=欠陥画素/1=正常画素
			data = IN8 ((memadrs2 + ix));
			for (bit=0; bit<8; bit++, xPixel++)
			{
				if (areaMode == ROI_AREA_MODE_DEFAULT_SIZE)
				{
					// Width左端
					if (xPixel < offsetX)
						continue;

					// Width右端
					if (xPixel >= (widthMax - offsetX))
						continue;
				}

				if ((data & (1<<bit)) == 0)
				{
					if (areaMode == ROI_AREA_MODE_DEFAULT_SIZE)
					{
						x = xPixel - offsetX;
						y = iy - offsetY;
					}
					else
					{
						x = xPixel;
						y = iy;
					}

					hit++;
				
#if !defined (MODE_SENSOR_X_REVERSE)
					if (flipMode == XFLIP_ENABLE)
						x = width - 1 - x;
#else
					if (flipMode == XFLIP_DISABLE)
						x = width - 1 - x;
#endif

#if !defined (MODE_SENSOR_X_REVERSE)
					if (flipMode == XFLIP_ENABLE)
#else
					if (flipMode == XFLIP_DISABLE)
#endif
					{
						if (yOld != y)
						{
							ptrXSave--;
							if (xCount == 0)
							{
								ptrGrid->x = x;
								ptrGrid->y = y;
								ptrGrid++;
								count++;
							}
							else
							{
								for (j=0; j<xCount; j++, ptrXSave--)
								{
									ptrGrid->x = *ptrXSave;
									ptrGrid->y = yOld;
									ptrGrid++;
									count++;
								}
							}

							yOld = y;
							ptrXSave = (unsigned short *)pImageSrc;
							xCount = 0;
						}

						// x保存
						*ptrXSave = x;
						ptrXSave++;
						xCount++;
					}
					else
					{
						ptrGrid->x = x;
						ptrGrid->y = y;
						ptrGrid++;
						count++;
					}
				} // if ((data&(1<<bit)) == 0)
			}	// for (bit=0; bit<8; bit++, xPixel++)
#if defined (MODE_IPU_MULTI)
		}	// for (ipu=0; ipu < IPU_COUNT; ipu++)
#endif
		}	// for (ix=0; ix<(IMG_WIDTH/8); ix++)
	} // for (iy=IMG_HEIGHT_OFFSET; iy<CAMERA_HEIGHT_MAX; iy++)


	// 終端
	if (iy == (height + offsetY))
	{
		ptrXSave--;
		for (j=0; j<xCount; j++, ptrXSave--)
		{
			ptrGrid->x = *ptrXSave;
			ptrGrid->y = yOld;
			ptrGrid++;
			count++;
		}
	}

	// カウント値格納
	*pCount = count;

_DONE:
	return (status);
}


//**********************************************************************************
// 補正不可座標表示(メモリに配置されたものを表示)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		userMode			：ユーザーモード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcImpossibleGridInfo (int dpcNum, int userMode)
{
	int status = AVAL_STATUS_SUCCESS;
	GRID_XY *pGridBuffer = NULL;
	GRID_XY *ptrGrid;
	int i, j;
	int flipMode;
	int width, height;
	int x, y;
	int yOld = 0;
	unsigned short *ptrXSave  = (unsigned short *)pImageSrc;
	unsigned int saveAdrs, saveSize;
	int xCount;
	int toTalCount;
	int count;

	// Width Max
	if ((status = roiGetAreaWidthMax (&width)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	if ((status = roiGetAreaHeightMax (&height)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get Flip
#if !defined (MODE_SENSOR_XFLIP)
	if ((status = aoiGetXflip (&flipMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	flipMode = XFLIP_DISABLE;
#endif

	// DPC Grid Buffer Memory Request
	if ((pGridBuffer = (GRID_XY *)malloc (DPC_IMPOSSIBLE_GRID_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Map Buffer Request Error. size = 0x%x.\n", DPC_IMPOSSIBLE_GRID_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Clear
	saveSize = sizeof(GRID_XY) * NUM_DEFECTION_PIX;
	saveAdrs = (unsigned int)dpcGetDefectionCountBuff2;
	memset ((void *)saveAdrs, 0xff, saveSize);

	// 並び替え
	ptrGrid = dpcGetDefectionCountBuff2;

	// DPC補正不可情報読み込み
	if ((status = dpcImpossibleGridRead (dpcNum, userMode, (unsigned short *)pGridBuffer)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("\n[DPC Impossible Grid]\n");

	yOld = 0;
	xCount = 0;
	toTalCount = 0;
	for (i=0; i<(DPC_IMPOSSIBLE_GRID_SIZE/8); i++, toTalCount++)
	{
		if ((pGridBuffer[i].x == DPC_IMPOSSIBLE_GRID_INVALID_DATA16) && (pGridBuffer[i].y == DPC_IMPOSSIBLE_GRID_INVALID_DATA16))
			break;

		// y座標取得
		y = pGridBuffer[i].y;

#if !defined (MODE_SENSOR_X_REVERSE)
		// Flip有効?
		if (flipMode == XFLIP_ENABLE)
			x = width - 1 -  pGridBuffer[i].x;
		else
			x =  pGridBuffer[i].x;
#else
		// Flip有効?
		if (flipMode == XFLIP_DISABLE)
			x = width - 1 -  pGridBuffer[i].x;
		else
			x =  pGridBuffer[i].x;
#endif


#if !defined (MODE_SENSOR_X_REVERSE)
		if (flipMode == XFLIP_ENABLE)
#else
		if (flipMode == XFLIP_DISABLE)

#endif
		{
			if (yOld != y)
			{
				ptrXSave--;
				for (j=0; j<xCount; j++, ptrXSave--)
				{
					ptrGrid->x = *ptrXSave;
					ptrGrid->y = yOld;
					ptrGrid++;
				}

				yOld = y;
				ptrXSave = (unsigned short *)pImageSrc;
				xCount = 0;
			}

			// x保存
			*ptrXSave =  x;
			ptrXSave++;
			xCount++;
		}
		else
		{
			ptrGrid->x = x;
			ptrGrid->y = y;
			ptrGrid++;
		}
	}

	// 最後のラインをコピー
#if !defined (MODE_SENSOR_X_REVERSE)
	if (flipMode == XFLIP_ENABLE)
#else
	if (flipMode == XFLIP_DISABLE)
#endif
	{
		ptrXSave--;
		for (j=0; j<xCount; j++, ptrXSave--)
		{
			ptrGrid->x = *ptrXSave;
			ptrGrid->y = yOld;
			ptrGrid++;
		}
	}

	// 表示
	if (i == 0)
	{
		DEBUG_PRINT_FORCE ("None\n");
	}
	else
	{
		ptrGrid = dpcGetDefectionCountBuff2;
		for (i=0, count=0; i<toTalCount; i++, ptrGrid++)
		{
			// 座標取得
			x = ptrGrid->x;
			y = ptrGrid->y;

			// Height上
			if (y < IMG_HEIGHT_OFFSET)
				continue;

			// Height下
			if (y > (height - IMG_HEIGHT_OFFSET))
				continue;

			// Width左端
			if (x < IMG_WIDTH_OFFSET)
				continue;

			// Width右端
			if (x > (width - IMG_WIDTH_OFFSET))
				continue;

			DEBUG_PRINT_FORCE ("%3d :  ", ++count);
			DEBUG_PRINT_FORCE ("x = %3d / ", ptrGrid->x);
			DEBUG_PRINT_FORCE ("y = %3d\n",  ptrGrid->y);
		}
	}

_DONE:
	if (pGridBuffer != NULL)
		free (pGridBuffer);

	return (status);
}


//**********************************************************************************
//	DPC Adjustモード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
///		mode				：DPC補正モード(1=通常補正/2=近傍画素補正)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetAdjustMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check mode Parameter
	if ((mode < DPC_ADJUST_MODE_MIN) || (mode > DPC_ADJUST_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust mode(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, DPC_ADJUST_MODE_MIN, DPC_ADJUST_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DPC補正モード設定
	OUT32 (FIRM_DATA_DPC_ADJUST_MODE_ADRS, mode);

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjustモード取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
///		pMode				：DPCAdjustモードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetAdjustMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Adjust Mode pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// Get DPC補正モード取得
	*pMode = IN32 (FIRM_DATA_DPC_ADJUST_MODE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust Rate設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		rate				：DPC調整時のRate
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetAdjustRate (int index, float rate)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Rate Set Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else // #if defined (MODE_DPC_ADJUST_MODE)

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Rate Set Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;

	}

	#endif // #if defined (MODE_DPC_ADJUST_MODE)

	// Check rate Parameter
	if ((rate < FRAMERATE_MIN) || (rate > FRAMERATE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Rate Set Rate(%f) Parameter Error. (Min:%.2f / Max:%.2f)\n", rate, FRAMERATE_MIN, FRAMERATE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Rate設定
	gDpcInfo.dpcPrivate[index].frameRate = (double)rate;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust Rate取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		pRate				：DPC調整時のRateを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetAdjustRate (int index, float *pRate)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#endif

	// check pRate Parameter
	if (pRate == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Adjust Get Rate pRate NULL Parameter Error.\n");
		goto _DONE;
	}

	// Rate取得
	*pRate = (float)gDpcInfo.dpcPrivate[index].frameRate;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust Exposure Time設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		exp					：DPC調整時の露光時間
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetAdjustExposure (int index, int exp)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Set Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Set Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#endif

	// Exposure設定
	gDpcInfo.dpcPrivate[index].expTime = exp;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust Exposure Time取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		pExp				：DPC調整時の露光時間を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetAdjustExposure (int index, int *pExp)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#endif

	// check pExp Parameter
	if (pExp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Adjust Exposure pExp NULL Parameter Error.\n");
		goto _DONE;
	}

	// Exposure取得
	*pExp = gDpcInfo.dpcPrivate[index].expTime;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust FFC Number設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		ffcNumber			：DPC調整時のFFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetAdjustFfcNumber (int index, int ffcNumber)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Set Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Set Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#endif


	// Check ffcNumber Parameter
	if ((ffcNumber < FFC_NUMBER_MIN) || (ffcNumber > FFC_NUMBER_MAX+1))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Set ffcNumber(%d) Parameter Error.(Min:%d / Max:%d)\n", ffcNumber, FFC_NUMBER_MIN, FFC_NUMBER_MAX+1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// FFC Number設定
	gDpcInfo.dpcPrivate[index].ffcNo = ffcNumber;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust FFC Number取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		pFfcNumber			：FFC調整時のFFC番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetAdjustFfcNumber (int index, int *pFfcNumber)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#endif

	// check pFfcNumber Parameter
	if (pFfcNumber == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Adjust Get FFC Number pFfcNumber NULL Parameter Error.\n");
		goto _DONE;
	}

	// FFC Number取得
	*pFfcNumber = gDpcInfo.dpcPrivate[index].ffcNo;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust Temperature設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		temp				：DPC調整時の温度
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetAdjustTemp (int index, float temp)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#endif

	// Check temp Parameter
	if ((temp < PELTIER_SENSOR_ASJUST_TEMP_MIN) || (temp > PELTIER_SENSOR_ASJUST_TEMP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Set Temperature(%.2f) Parameter Error. (Min:%.2f / Max:%.2f)\n", temp, PELTIER_SENSOR_ASJUST_TEMP_MIN, PELTIER_SENSOR_ASJUST_TEMP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DPC Temperature設定
	gDpcInfo.dpcPrivate[index].sensorTemp = (double)temp;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust Temperature取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		pTemp				：DPC調整時の温度を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetAdjustTemp (int index, float *pTemp)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#endif

	// check pTemp Parameter
	if (pTemp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Adjust Get Temperature pTemp NULL Parameter Error.\n");
		goto _DONE;
	}

	// 温度取得
	*pTemp = (float)gDpcInfo.dpcPrivate[index].sensorTemp;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust 標準偏差設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		bIndex				：DPC明るさインデックス
//		sd					：標準偏差
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetAdjustSd (int index, int bIndex, float sd)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#endif

	// Check bIndex Parameter
	if ((bIndex < DPC_BRIGHTNESS_LOOP_MIN) || (bIndex > DPC_BRIGHTNESS_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get bIndex(%d) Parameter Error.(Min:%d / Max:%d)\n", bIndex, DPC_BRIGHTNESS_LOOP_MIN, DPC_BRIGHTNESS_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DPC 標準偏差設定
	gDpcInfo.dpcPrivate[index].sd[bIndex] = (double)sd;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust 標準偏差取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		bIndex				：DPC明るさインデックス
//		pSd					：標準偏差を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetAdjustSd (int index, int bIndex, float *pSd)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#endif

	// Check bIndex Parameter
	if ((bIndex < DPC_BRIGHTNESS_LOOP_MIN) || (bIndex > DPC_BRIGHTNESS_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get bIndex(%d) Parameter Error.(Min:%d / Max:%d)\n", bIndex, DPC_BRIGHTNESS_LOOP_MIN, DPC_BRIGHTNESS_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// check pSd Parameter
	if (pSd == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Adjust Get SD pSd NULL Parameter Error.\n");
		goto _DONE;
	}

	// DPC 標準偏差取得
	*pSd = (float)gDpcInfo.dpcPrivate[index].sd[bIndex];

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust 感度不均一設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		bIndex				：DPC明るさインデックス
//		unUniform			：感度不均一
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetAdjustNonUniform (int index, int bIndex, float unUniform)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#endif

	// Check bIndex Parameter
	if ((bIndex < DPC_BRIGHTNESS_LOOP_MIN) || (bIndex > DPC_BRIGHTNESS_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get bIndex(%d) Parameter Error.(Min:%d / Max:%d)\n", bIndex, DPC_BRIGHTNESS_LOOP_MIN, DPC_BRIGHTNESS_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DPC 標準偏差設定
	gDpcInfo.dpcPrivate[index].ununiformity[bIndex] = (double)unUniform;

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust 感度不均一取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		index				：DPCインデックス
//		bIndex				：DPC明るさインデックス
//		pUnUniform			：感度不均一を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetAdjustNonUniform (int index, int bIndex, float *pUnuniform)
{
	int status = AVAL_STATUS_SUCCESS;

	#if defined (MODE_DPC_ADJUST_MODE)

	int dpcMode;
	int indexMax;

	// DPCモード取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
		indexMax = DPC_FFC_LOOP_MAX;
	else
		indexMax = DPC_FFC_LOOP_NEIGHBORHOOD_MAX - 1;

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > indexMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, indexMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	#else

	// Check index Parameter
	if ((index < DPC_FFC_LOOP_MIN) || (index > DPC_FFC_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get Index(%d) Parameter Error.(Min:%d / Max:%d)\n", index, DPC_FFC_LOOP_MIN, DPC_FFC_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	#endif

	// Check bIndex Parameter
	if ((bIndex < DPC_BRIGHTNESS_LOOP_MIN) || (bIndex > DPC_BRIGHTNESS_LOOP_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Adjust Get bIndex(%d) Parameter Error.(Min:%d / Max:%d)\n", bIndex, DPC_BRIGHTNESS_LOOP_MIN, DPC_BRIGHTNESS_LOOP_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// check pUnuniform Parameter
	if (pUnuniform == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Adjust Get nonuniform pUnuniform NULL Parameter Error.\n");
		goto _DONE;
	}

	// DPC 感度不均一取得
	*pUnuniform = (float)gDpcInfo.dpcPrivate[index].ununiformity[bIndex];

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Adjust Update(管理外メモリにあるデータを構造体にコピー)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcAdjustUpdate (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int index, bIndex;
	unsigned int adrs, offset;

	for (index=0; index<FFC_NUM_USER; index++)
	{
		// アドレス
		adrs = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + index * FIRM_DATA_DPC_ADJUST_FFC_SIZE;

		// Rate
		gDpcInfo.dpcPrivate[index].frameRate = (double)INF ((adrs + FIRM_DATA_DPC_ADJUST_RATE_ADRS));

		// Exposure
		gDpcInfo.dpcPrivate[index].expTime = IN32 ((adrs + FIRM_DATA_DPC_ADJUST_EXPOSURE_ADRS));

		// Temp
		gDpcInfo.dpcPrivate[index].sensorTemp = (double)INF ((adrs + FIRM_DATA_DPC_ADJUST_TEMP_ADRS));

		// FFC Number
		gDpcInfo.dpcPrivate[index].ffcNo = IN32 ((adrs + FIRM_DATA_DPC_ADJUST_FFC_NUMBER_ADRS));

		for (bIndex=0; bIndex<DPC_BRIGHTNESS_LOOP_NUMBER; bIndex++)
		{
			// Offset
			offset = bIndex * 4;

			// Standard Deviation
			gDpcInfo.dpcPrivate[index].sd[bIndex] = (double)INF ((adrs + offset + FIRM_DATA_DPC_ADJUST_SD0_ADRS));

			// Non Uniformity
			gDpcInfo.dpcPrivate[index].ununiformity[bIndex] = (double)INF ((adrs + offset + FIRM_DATA_DPC_ADJUST_NON_UNIFORM0_ADRS));
		}
	}

	return (status);
}


//**********************************************************************************
//	DPCデータをDPCメモリ領域にコピー
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		srcAdrs				：転送元アドレス
//		offset				：オフセットアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetMemoryNormal (void *srcAdrs, unsigned int offset, unsigned int size)
{
	int status;
	unsigned int memAdrs;
	int sizeMax;
#if !defined (MODE_IPU_MULTI)
	unsigned int memAdrs2;
	unsigned int data;
	unsigned int i;
	int widthMax, heightMax;
	int x, y;
#endif
	int dpcNum = 0;

	sizeMax = DPC_MEMORY_NEW_SIZE;

	// Check offset Parameter
	if ((offset < 0) || (offset > sizeMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Set Memory offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset, 0, sizeMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DPCメモリアドレス取得
	if ((status = dpcGetMemAdrs (dpcNum, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if !defined (MODE_IPU_MULTI)
	// Width Max
	widthMax = sensorWidth();

	// Height Max
	heightMax = sensorHeight();

	memAdrs2 = memAdrs;
	i = 0;
	for (y=0; y<heightMax; y++)
	{
		for (x=0; x<(widthMax/8); x++, i++)
		{
			data = IN8 (((unsigned int)srcAdrs + i));
			OUT8 ((memAdrs2 + offset + x), data);
		}

		memAdrs2 += DPC_WIDTH_DATA_ALIGH;
	}

#else // #if !defined (MODE_IPU_MULTI)

	// データCopy
	dpcCopyBuffToExtMem (memAdrs, (unsigned int *)srcAdrs);

	// DPCマージン座標の追加
	// ipuの1個目の最後の16画素の追加。
	// ipuの2個目の最初の16画素の追加。
	if ((status = dpcSetMarginGridData ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#endif // #if !defined (MODE_IPU_MULTI)


_DONE:
	return (status);
}


//**********************************************************************************
//	DPCデータをDPCメモリ領域にコピー
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		desAdrs				：転送先アドレス
//		offset				：オフセットアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetMemoryNormal (void *desAdrs, unsigned int offset, unsigned int size)
{
	int status;
	unsigned int memAdrs;
	int sizeMax;
#if !defined (MODE_IPU_MULTI)
	unsigned int memAdrs2;
	unsigned int data;
	unsigned int i;
	int widthMax, heightMax;
	int x, y;
#endif
	int dpcNum = 0;

	sizeMax = DPC_MEMORY_NEW_SIZE;

	// Check offset Parameter
	if ((offset < 0) || (offset > sizeMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Get Memory offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset, 0, sizeMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DPCメモリアドレス取得
	if ((status = dpcGetMemAdrs (dpcNum, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if !defined (MODE_IPU_MULTI)
	// Width Max
	widthMax = sensorWidth();

	// Height Max
	heightMax = sensorHeight();

	memAdrs2 = memAdrs;
	i=0;
	for (y=0; y<heightMax; y++)
	{
		for (x=0; x<(widthMax/8); x++, i++)
		{
			data = IN8 ((memAdrs2 + x));
			OUT8 (((unsigned int)desAdrs + i), data);
		}

		memAdrs2 += DPC_WIDTH_DATA_ALIGH;
	}

#else // !defined (MODE_IPU_MULTI)

	dpcCopyExtMemToBuff (memAdrs, (unsigned int *)desAdrs);

#endif // !defined (MODE_IPU_MULTI)

_DONE:
	return (status);
}


//**********************************************************************************
// DPC座標Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：x座標
//		y					：y座標
//		pDetect				：0=正常画素/1=欠陥画素を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGridCheck (int x, int y, int *pDetect)
{
	int status = AVAL_STATUS_SUCCESS;
	int wMax, hMax;
	int dpcNum = 0;
	unsigned int memAdrs;
	int bit;
	unsigned char data8;
	int areaMode;
	unsigned int offset;
#if defined (MODE_IPU_MULTI)
	int xChange;
#else
	int yOffset;
#endif

	// 座標変換
	if ((status = roiCoordinateTrans (x, y, &x, &y, TRANS_MODE_ADD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Area Mode
	if ((status = roiGetAreaFlag (&areaMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (areaMode == ROI_AREA_MODE_DEFAULT_SIZE)
	{
		// Width Max
		wMax = WidthMax();

		// Height Max
		hMax = HeightMax();

		// Check x Parameter
		if ((x < 0) || (x > wMax))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "DPC Grid Check x size(%d) Parameter Error. (Min:0 / Max:%d)\n", x, wMax);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Check y Parameter
		if ((y < 0) || (y > hMax))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "DPC Grid Check y size(%d) Parameter Error. (Min:0 / Max:%d)\n", x, hMax);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Offset追加
		if ((status = roiGetAreaGridOffset (&x, &y)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// Width Max
		wMax = sensorWidth();

		// Height Max
		hMax = sensorHeight();

		// Check x Parameter
		if ((x < 0) || (x > wMax))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "DPC Grid Check x size(%d) Parameter Error. (Min:0 / Max:%d)\n", x, wMax);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Check y Parameter
		if ((y < 0) || (y > hMax))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "DPC Grid Check y size(%d) Parameter Error. (Min:0 / Max:%d)\n", y, hMax);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

	// Check pDetect Parameter
	if (pDetect == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Grid Check pDetect NULL Parameter Error.\n");
		goto _DONE;
	}

	// DPCメモリアドレス取得
	if ((status = dpcGetMemAdrs (dpcNum, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥画素アドレス
#if !defined (MODE_IPU_MULTI)

	// Yオフセット
	yOffset = y * DPC_WIDTH_DATA_ALIGH;

	// 欠陥画素アドレス
	offset = memAdrs + yOffset + x / 8;

	// 欠陥画素ビット
	bit = x % 8;

#else

	// 分割アドレス取得
	if ((status = dpcGetDivAdrs (x, y, &xChange, &offset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標変換
	x = xChange;

	// ベースアドレス加算
	offset += memAdrs;

	// 欠陥画素ビット
	bit = xChange % 8;

#endif

	// データ取得
	data8 = IN32 (offset);
	if (data8 & (1<<bit))
		*pDetect = 0;	// 欠陥画素なし
	else
		*pDetect = 1;	// 欠陥画素あり

_DONE:
	return (status);
}


//**********************************************************************************
// DPC座標Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：x座標
//		y					：y座標
//		pDetect				：0=正常画素/1=欠陥画素を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGridCheckAll (int x, int y, int *pDetect)
{
	int status = AVAL_STATUS_SUCCESS;
	int wMax, hMax;
	int dpcNum = 0;
	unsigned int memAdrs;
	int bit;
	unsigned char data8;
	unsigned int offset;
#if defined (MODE_IPU_MULTI)
	int xChange;
#else
	int yOffset;
#endif

	// Width Max
	wMax = sensorWidth();

	// Height Max
	hMax = sensorHeight();

	// Check x Parameter
	if ((x < 0) || (x > wMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Grid Check x size(%d) Parameter Error. (Min:0 / Max:%d)\n", x, wMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check y Parameter
	if ((y < 0) || (y > hMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Grid Check y size(%d) Parameter Error. (Min:0 / Max:%d)\n", y, hMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pDetect Parameter
	if (pDetect == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Grid Check pDetect NULL Parameter Error.\n");
		goto _DONE;
	}

	// DPCメモリアドレス取得
	if ((status = dpcGetMemAdrs (dpcNum, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 欠陥画素アドレス
#if !defined (MODE_IPU_MULTI)

	// Yオフセット
	yOffset = y * DPC_WIDTH_DATA_ALIGH;

	// 欠陥画素アドレス
	offset = memAdrs + yOffset + x / 8;

	// 欠陥画素ビット
	bit = x % 8;

#else

	// 分割アドレス取得
	if ((status = dpcGetDivAdrs (x, y, &xChange, &offset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// X座標変換
	x = xChange;

	// ベースアドレス加算
	offset += memAdrs;

	// 欠陥画素ビット
	bit = xChange % 8;
#endif

	// データ取得
	data8 = IN32 (offset);
	if (data8 & (1<<bit))
		*pDetect = 0;	// 欠陥画素なし
	else
		*pDetect = 1;	// 欠陥画素あり

_DONE:
	return (status);
}


#if defined (MODE_SENSOR_XFLIP)
#if !defined (MODE_IPU_MULTI)
//**********************************************************************************
//	DPC Data X Flip
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=正転する/1=反転する
//		dpcMemAdrs			：メモリアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcDataXFlip (int mode, unsigned int dpcMemAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	int x, y;
	unsigned int /*dpcMemAdrs,*/ dpcMemAdrs2;
	int width;
	int height;
	unsigned int data8, dataRev8;
	int bit, bit2;
	int endX, remX;
	unsigned char *pBuffTemp8;
	unsigned int *pBuff32 = NULL;
	//int dpcNum = 0;
	int size = DPC_WIDTH_DATA_ALIGH;

	// データFlipあり／なし
	if (dpcGetXFlipFlag () == MODE_DISABLE)
		goto _DONE;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC X Flip mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Width Max取得
	width = sensorWidth ();

	// Height Max取得
	height = sensorHeight ();

	// メモリリクエスト
	if ((pBuff32 = (unsigned int *)malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC X Flip Malloc Error. size = 0x%x\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	dpcMemAdrs2 = dpcMemAdrs;
	for (y=0; y<height; y++)
	{
		// 0xffにクリア
		memset ((void *)pBuff32, 0xff, size);

		// 1ラインバッファに読み込み
		pBuffTemp8 = (unsigned char *)pBuff32;

		// X終端位置
		endX = width / 8;
		remX = width % 8;
		if (remX != 0)
			endX++;

		bit2=0;
		dataRev8 = 0xff;
		for (x=0; x<endX; x++)
		{
			// 終端から確認
			data8 = IN8 ((dpcMemAdrs2 + (endX - x - 1)));

			// 終端から確認の為、最初の読み込みは終端となる為の調整シフト
			if ((x==0) && (remX!=0))
				bit = remX;
			else
				bit = 7;

			// bitは現在の欠陥を検索
			// bit2は反転した欠陥を格納
			// widthが8で割り切れない場合、bitとbit2が異なる
			for (; bit>=0; bit--, bit2++)
			{
				// 0=欠陥画素/1=正常画素
				if ((data8 & (1<<bit)) == 0)
				{
					dataRev8 &= ~(1 << bit2);
				}

				// 8bit分設定したらメモリへ格納
				if (bit2 >= 7)
				{
					// 一時バッファに設定
					*pBuffTemp8 = dataRev8;
					pBuffTemp8++;
					dataRev8 = 0xff;		// 初期化
					bit2 = 0;
					break;
				}
			}
		}

		// Fill
		for (x=0; x<size; x++)
			//OUT8 ((dpcMemAdrs + x), 0xff);
			OUT8 ((dpcMemAdrs2 + x), 0xff);

		// 反転
		pBuffTemp8 = (unsigned char *)pBuff32;
		for (x=0; x<endX; x++, pBuffTemp8++)
			OUT8 ((dpcMemAdrs2 + x), *pBuffTemp8);

		// Align
		dpcMemAdrs2 += DPC_WIDTH_DATA_ALIGH;
	}

_DONE:
	if (pBuff32 != NULL)
		free (pBuff32);

	return (status);
}

#else // #if !defined (MODE_IPU_MULTI)

//**********************************************************************************
//	DPC Data X Flip
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=正転する/1=反転する
//		dpcMemAdrs			：メモリアドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcDataXFlip (int mode, unsigned int dpcMemAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	int x, y;
	unsigned int /*dpcMemAdrs,*/ dpcMemAdrs2;
	int width;
	int height;
	unsigned int data8, dataRev8;
	int bit, bit2;
	int endX, remX;
	unsigned char *pBuffTemp8;
	unsigned int *pBuffLeft = NULL;
	unsigned int *pBuffRight = NULL;
	//int dpcNum = 0;
	int size = DPC_WIDTH_DATA_ALIGH;
	int ipu;

	// データFlipあり／なし
	if (dpcGetXFlipFlag () == MODE_DISABLE)
		goto _DONE;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC X Flip mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Width Max取得
	width = IMG_WIDTH / IPU_COUNT;

	// Height Max取得
	height = sensorHeight ();

	// メモリリクエスト
	if ((pBuffLeft = (unsigned int *)malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC X Flip Malloc Error. size = 0x%x\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if ((pBuffRight = (unsigned int *)malloc (size)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
		sprintf (gLogMsgBuff, "DPC X Flip Malloc Error. size = 0x%x\n", size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}


	for (y=0; y<height; y++)
	{
		//------------------------------------------------------------
		// データ取得
		//------------------------------------------------------------
		for (ipu=0; ipu < IPU_COUNT; ipu++)
		{
			dpcMemAdrs2 = dpcMemAdrs + DPC_MEMORY_IPU_MULTI_INTERVAL * ipu;

			if (ipu == 0)
			{
				// 0xffにクリア
				memset ((void *)pBuffLeft, 0xff, size);

				// 1ラインバッファに読み込み
				pBuffTemp8 = (unsigned char *)pBuffLeft;
			}
			else
			{
				// 右の最初の16画素は補正用のデータ
				dpcMemAdrs2 += (IMG_WIDTH_OFFSET / 8);

				// 0xffにクリア
				memset ((void *)pBuffRight, 0xff, size);

				// 1ラインバッファに読み込み
				pBuffTemp8 = (unsigned char *)pBuffRight;
			}

			// X終端位置
			endX = width / 8;
			remX = width % 8;
			if (remX != 0)
				endX++;

			bit2=0;
			dataRev8 = 0xff;
			for (x=0; x<endX; x++)
			{
				// 終端から確認
				data8 = IN8 ((dpcMemAdrs2 + (endX - x - 1)));

				// 終端から確認の為、最初の読み込みは終端となる為の調整シフト
				if ((x==0) && (remX!=0))
					bit = remX;
				else
					bit = 7;

				// bitは現在の欠陥を検索
				// bit2は反転した欠陥を格納
				// widthが8で割り切れない場合、bitとbit2が異なる
				for (; bit>=0; bit--, bit2++)
				{
					// 0=欠陥画素/1=正常画素
					if ((data8 & (1<<bit)) == 0)
					{
						dataRev8 &= ~(1 << bit2);
					}

					// 8bit分設定したらメモリへ格納
					if (bit2 >= 7)
					{
						// 一時バッファに設定
						*pBuffTemp8 = dataRev8;
						pBuffTemp8++;
						dataRev8 = 0xff;		// 初期化
						bit2 = 0;
						break;
					}
				}
			}
		} // for (ipu=0; ipu < IPU_COUNT; ipu++)

		//------------------------------------------------------------
		// データ設定(左右上下入れ替え)
		//------------------------------------------------------------
		for (ipu=0; ipu < IPU_COUNT; ipu++)
		{
			if (ipu == 0)
			{
				dpcMemAdrs2 = dpcMemAdrs + DPC_MEMORY_IPU_MULTI_INTERVAL;

				// 右の最初の16画素は補正用のデータ
				dpcMemAdrs2 += (IMG_WIDTH_OFFSET / 8);

				pBuffTemp8 = (unsigned char *)pBuffLeft;
			}
			else
			{
				dpcMemAdrs2 = dpcMemAdrs;
				pBuffTemp8 = (unsigned char *)pBuffRight;
			}

			// Fill
			for (x=0; x<size; x++)
				OUT8 ((dpcMemAdrs2 + x), 0xff);

			// 反転
			for (x=0; x<endX; x++, pBuffTemp8++)
				OUT8 ((dpcMemAdrs2 + x), *pBuffTemp8);
		}

		// Align
		dpcMemAdrs += DPC_WIDTH_DATA_ALIGH;
	}

	// ipuの1個目の最後の16画素の追加。
	// ipuの2個目の最初の16画素の追加。
	if ((status = dpcSetMarginGridData ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (pBuffLeft != NULL)
		free (pBuffLeft);

	if (pBuffRight != NULL)
		free (pBuffRight);

	return (status);
}
#endif // #if !defined (MODE_IPU_MULTI)
#endif // #if defined (MODE_SENSOR_XFLIP)


#if defined (MODE_SENSOR_XFLIP)
//**********************************************************************************
//	DPC X Flip Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=DPCデータをX Flipしない/1=DPCデータをX Flipする
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
void dpcSetXFlipFlag (int mode)
{
	OUT32 (FIRM_DATA_FLIP_DPC_MODE_ADRS, mode);
}


//**********************************************************************************
//	DPC X Flip Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		mode				：0=DPCデータをX Flipしない/1=DPCデータをX Flipする
//==================================================================================
int dpcGetXFlipFlag (void)
{
	int data32;
	data32 = (int)IN32 (FIRM_DATA_FLIP_DPC_MODE_ADRS);
	return (data32);
}
#endif


#if defined (MODE_IPU_MULTI)
//**********************************************************************************
//	DPCデータCopy(バッファからDPCメモリにCopy)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		memAdrs				：DPCデータ格納アドレス
//		pBuffer				：Copy元アドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcCopyBuffToExtMem (unsigned long memAdrs, unsigned int *pBuffer)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *ptrB;
	int x, y;
	unsigned char data;
	int xChange;
	unsigned int offset;

	ptrB = (unsigned char *)pBuffer;

	for (y=0; y<IMG_HEIGHT; y++)
	{
		for (x=0; x<(IMG_WIDTH / 8); x++, ptrB++)
		{
			// 分割アドレス取得
			if ((status = dpcGetDivAdrs (x*8, y, &xChange, &offset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			data = *ptrB;
			OUT8 ((memAdrs + offset), data);
		}
	}

_DONE:
	return (status);
}
#endif // #if defined (MODE_IPU_MULTI)


#if defined (MODE_IPU_MULTI)
//**********************************************************************************
//	DPCデータCopy(DPCメモリからバッファにCopy)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		memAdrs				：FFCデータ格納アドレス
//		pBuffer				：Copy元アドレス
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcCopyExtMemToBuff (unsigned long memAdrs, unsigned int *pBuffer)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char *ptrB;
	int x, y;
	unsigned char data;
	int xChange;
	unsigned int offset;

	ptrB = (unsigned char *)pBuffer;

	for (y=0; y<IMG_HEIGHT; y++)
	{
		for (x=0; x<(IMG_WIDTH / 8); x++, ptrB++)
		{
			// 分割アドレス取得
			if ((status = dpcGetDivAdrs (x*8, y, &xChange, &offset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			data = IN8 ((memAdrs + offset));
			*ptrB = data;
		}
	}

_DONE:
	return (status);
}
#endif	// #if defined (MODE_IPU_MULTI)


#if defined (MODE_IPU_MULTI)
//**********************************************************************************
// DPCマージン座標の追加
// ipuの1個目の最後の16画素の追加。
// ipuの2個目の最初の16画素の追加。
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcSetMarginGridData (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int ipu;
	int x, y;
	int dpcNum = 0;
	unsigned int memAdrs;
	unsigned int srcAdrs, desAdrs;
	unsigned char data8;

	// メモリアドレス取得
	if ((status =dpcGetMemAdrs (dpcNum, &memAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	for (ipu=0; ipu < IPU_COUNT; ipu++)
	{
		for (y = 0; y < IMG_HEIGHT; y++)
		{
			if (ipu == 0)
			{
				// ipu1の最初の16画素
				srcAdrs =  memAdrs;
				srcAdrs += (y * DPC_WIDTH_DATA_ALIGH);
				srcAdrs += (IMG_WIDTH_OFFSET/8);
				srcAdrs += (DPC_MEMORY_IPU_MULTI_INTERVAL * (ipu + 1));

				// ipu0の最後の16画素
				desAdrs =  memAdrs;
				desAdrs += ((y * DPC_WIDTH_DATA_ALIGH) + (IMG_WIDTH_IPU_SIZE/8));
			}
			else
			{
				// ipu0の最後の16画素
				srcAdrs =  memAdrs;
				srcAdrs += ((y * DPC_WIDTH_DATA_ALIGH) + (IMG_WIDTH_IPU_SIZE/8));
				srcAdrs -= (IMG_WIDTH_OFFSET / 8);

				// ipu1の最初の画素
				desAdrs =  memAdrs;
				desAdrs += (y * DPC_WIDTH_DATA_ALIGH);
				desAdrs += (DPC_MEMORY_IPU_MULTI_INTERVAL * ipu);
			}

			for (x = 0; x < (IMG_WIDTH_OFFSET / 8)/*1画素=1bit*/; x++)
			{
				data8 = IN8 (srcAdrs);
				OUT8 (desAdrs, data8);

				srcAdrs++;
				desAdrs++;
			}
		}
	}

_DONE:
	return (status);
}
#endif // #if defined (MODE_IPU_MULTI)


#if defined (MODE_IPU_MULTI)
//**********************************************************************************
// DPC 分割アドレスの取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：x座標
//		y					：y座標
//		pX					：変換後のx座標を格納するポインタ
//		pAdrs				：変換アドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpcGetDivAdrs (int x, int y, int *pX, unsigned int *pAdrs)
{
	int status = AVAL_STATUS_SUCCESS;
	int widthMax, heightMax;
	int ipu, xAmari;
	int yOffset;
	int widthIpuUnit;

	// Width Max
	widthMax = sensorWidth();

	// Height Max
	heightMax = sensorHeight();

	// Check x Parameter
	if ((x < 0) || (x > widthMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Get Div Adrs x(%d) Parameter Error.(Min=0 / Max=%d)\n", x, widthMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check y Parameter
	if ((y < 0) || (y > heightMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Get Div Adrs y(%d) Parameter Error.(Min=0 / Max=%d)\n", y, heightMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pX Parameter
	if (pX == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Div Adrs NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Get Div Adrs NULL Parameter Error.\n");
		goto _DONE;
	}

	// Yオフセット
	yOffset = y * DPC_WIDTH_DATA_ALIGH;

	// widthをipuの個数で割る
	widthIpuUnit = IMG_WIDTH / IPU_COUNT;

	// ipuの何個目?
	ipu = x / (IMG_WIDTH / IPU_COUNT);

	// x座標は何画素目に該当するかを計算
	xAmari = x % widthIpuUnit;

	// ipu=0以外はオフセットが必要
	if (ipu != 0)
		xAmari += IMG_WIDTH_OFFSET;

	// DPCアドレスの算出
	*pAdrs = yOffset + (xAmari / 8) + DPC_MEMORY_IPU_MULTI_INTERVAL * ipu;

	// 変換後のx座標
	*pX = xAmari;

_DONE:
	return (status);
}
#endif // #if defined (MODE_IPU_MULTI)


#if defined (MODE_IPU_MULTI)
//**********************************************************************************
// DPC 分割アドレスの取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		x					：x座標
//		y					：y座標
//		pX					：変換後のx座標を格納するポインタ
//		pAdrs				：変換アドレスを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int hideGridCalc (int x, int y, int *pLeft, int *pRight)
{
	int status = AVAL_STATUS_SUCCESS;
	int widthHalf;
	int ipu;

	// Check pLeft Parameter
	if (pLeft == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Hide Grid pLeft NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pRight Parameter
	if (pRight == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Hide Grid pLeft NULL Parameter Error.\n");
		goto _DONE;
	}

	// 初期化
	*pLeft = -1;
	*pRight = -1;

	// ipuの何個目?
	ipu = x / (IMG_WIDTH / IPU_COUNT);

	//--------------------------------------------------------------------------------
	// IPU=0
	// ABA-052VIR：左側の1280～1295の領域を書き換えたら、右側の0～15(1296～1311)の領域も書き換える
	// 
	// IPU=1
	// ABA-052VIR：右側の0～15(1328～1343)の領域を書き換えたら、左側の1296～1311の領域も書き換える
	//--------------------------------------------------------------------------------
	if (ipu == 0)
	{
		widthHalf = CAMERA_WIDTH_MAX / IPU_COUNT;

		if (x >= widthHalf)
			*pRight = x - widthHalf;
	}
	else
	{
		widthHalf = IMG_WIDTH / IPU_COUNT;
		if ((x >= widthHalf) && (x < (widthHalf + 16)))
			*pLeft = x - widthHalf;
	}

_DONE:
	return (status);
}
#endif // #if defined (MODE_IPU_MULTI)

// eof
