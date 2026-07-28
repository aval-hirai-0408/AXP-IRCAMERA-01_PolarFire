//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// ipu.c - FFC/DPC Initialize Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if !defined(AXP_ABA001FIR_01) && !defined(AXP_ABA003FIR_01)
//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int gFfcStatus;								// FFC Status


//**********************************************************************************
//	FFC初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ipuFfcInitialize (void)
{
	int status;
	unsigned int adrs;
	unsigned int ffcEnable, ffcMemAdrs, ffcSize, ffcFlash;
	unsigned int ffcNum;
	unsigned int ffcSelect;
	unsigned int data32;
	int ffcCorMode;

	//------------------------------------------------------------
	// FFCメモリ領域クリア
	//------------------------------------------------------------
	memset ((void *)FFC_MEMORY_ADRS, 0, FFC_ALL_SIZE_FIRST);

	//------------------------------------------------------------
	// FFC補正モード初期化
	//------------------------------------------------------------
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_FFC_COR_MODE_ADRS, &adrs, &data32);
	OUT32 (FIRM_DATA_FFC_CORECTION_MODE_ADRS, data32);
	ffcCorMode = data32;

	//------------------------------------------------------------
	// FFC Disable
	//------------------------------------------------------------
	OUT32 (FPGA_FFC_CTRL_ADRS, 0);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_FFC2_CTRL_ADRS, 0);
#endif

	//------------------------------------------------------------
	// FFC Enable取得
	//------------------------------------------------------------
	adrs = 0;
	ffcEnable = 0;
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_FFC_ENABLE_ADRS, &adrs, &ffcEnable);
	if ((adrs == 0) && (ffcEnable == 0))
		ffcEnable = (FPGA_FFC_CTRL_GAIN_ENABLE_BIT | FPGA_FFC_CTRL_OFFSET_ENABLE_BIT);	// Offset/Gain有効

	// FFC Select
	ffcSelect = FPGA_FFC_CTRL_GET_SELECT(ffcEnable);

	// Offset & Gainのみ有効
	ffcEnable &= (FPGA_FFC_CTRL_GAIN_SECOND_ENABLE_BIT | FPGA_FFC_CTRL_GAIN_ENABLE_BIT | FPGA_FFC_CTRL_OFFSET_ENABLE_BIT);

	//------------------------------------------------------------
	// FFCアドレス取得
	//------------------------------------------------------------
	ffcMemAdrs = FFC_MEMORY_ADRS;

	//------------------------------------------------------------
	// FFC番号取得
	//------------------------------------------------------------
	adrs = 0;
	ffcNum = 0;
	ffcGetLoadNum ((int *)&ffcNum);

	// Error Clear
	gFfcStatus = 0;

	//--------------------------------------------------------------------------------
	// 1次補正 FFCパラメータ初期化 & Enable
	//--------------------------------------------------------------------------------
	if (ffcCorMode == FFC_CORRECTION_MODE_FIRST)
	{
#if defined (MODE_FFC_SHADING_LINE)
_FIRST_MODE:
#endif
		// FFC補正モード
		if ((status = ffcSetCorMode (FFC_CORRECTION_MODE_FIRST)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC Flashアドレス取得
		if ((status = ffcGetFlashAdrs (ffcNum, &ffcFlash)) != AVAL_STATUS_SUCCESS)
			ffcFlash = FLASH_FFC0_ADRS;

		// メモリへ保存
		cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_FFC_FLASH_ADRS, 0, ffcFlash);

		// 通常機能有効
		if ((status = ffcInitialize (ffcFlash, ffcMemAdrs, ffcEnable)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}	
	//--------------------------------------------------------------------------------
	// Shading Line補正 FFCパラメータ初期化 & Enable
	//--------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	else if (ffcCorMode == FFC_CORRECTION_MODE_SHADING_LINE)
	{
		// FFC補正モード
		if ((status = ffcSetCorMode (ffcCorMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// FFC Flashアドレス取得
		if ((status = ffcGetFlashAdrsShadingLine (ffcNum, &ffcFlash)) != AVAL_STATUS_SUCCESS)
			ffcFlash = FLASH_FFC0_ADRS;

		// メモリへ保存
		cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_FFC_FLASH_ADRS, 0, ffcFlash);

		// 通常機能有効
		if ((status = ffcInitializeShadingLine (ffcFlash, ffcMemAdrs, ffcEnable)) != AVAL_STATUS_SUCCESS)
		{
			if (status == MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_VALUE))
			{
				goto _FIRST_MODE;
			}
		}
	}
#endif // #if defined (MODE_FFC_SHADING_LINE)

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ipuDpcInitialize (void)
{
	int status;
	unsigned int adrs;
	unsigned int dpcEnable, dpcMemAdrs, dpcSize;
	unsigned int dpcFlash;
	unsigned int dpcNum;

	//------------------------------------------------------------
	// DPCメモリ領域クリア
	//------------------------------------------------------------
#if defined (MODE_DPC_DATA_ALIGN_ADJUST)
	// DPCは別の場所で実施
	//memset ((void *)DPC_MEMORY_ADRS, 0, DPC_ALL_SIZE);
#endif // #if defined (MODE_DPC_DATA_ALIGN_ADJUST)

	//------------------------------------------------------------
	// DPC補正モード初期化
	//------------------------------------------------------------
	OUT32 (FIRM_DATA_DPC_ADJUST_MODE_ADRS, 0);

	//------------------------------------------------------------
	// DPC Disable
	//------------------------------------------------------------
	dpcSetEnableMode (MODE_DISABLE);

	//------------------------------------------------------------
	// DPCクリア
	//------------------------------------------------------------
	dpcClearBatchCount ();

	//------------------------------------------------------------
	// DPC Enable取得
	//------------------------------------------------------------
	adrs = 0;
	dpcEnable = 0;
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_DPC_ENABLE_ADRS, &adrs, &dpcEnable);
	if ((adrs == 0) && (dpcEnable == 0))
		dpcEnable = FPGA_DPC_CTRL_ENABLE_BIT;

	//------------------------------------------------------------
	// DPCアドレス取得
	//------------------------------------------------------------
	dpcMemAdrs = DPC_MEMORY_ADRS;

	//------------------------------------------------------------
	// DPCサイズ取得(未サポート)
	//------------------------------------------------------------
	adrs = 0;
	dpcSize = 0;
	cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_DPC_ADRS, &adrs, &dpcSize);
	if ((adrs == 0) && (dpcSize == 0))
		dpcSize = 0;

	//------------------------------------------------------------
	// DPC番号取得
	//------------------------------------------------------------
	adrs = 0;
	dpcNum = 0;
	dpcGetLoadNum ((int *)&dpcNum);

	// メモリへ保存
	dpcSetLoadNum (dpcNum);
	
	//------------------------------------------------------------
	// DPC Flashアドレス取得
	//------------------------------------------------------------
	if ((status = dpcGetFlashAdrs (dpcNum, &dpcFlash)) != AVAL_STATUS_SUCCESS)
		dpcFlash = FLASH_DPC_ADRS;

	// メモリへ保存
	cameraParamUserWriteMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_DPC_FLASH_ADRS, 0, dpcFlash);

	//------------------------------------------------------------
	// DPCパラメータ初期化 & Enable
	//------------------------------------------------------------

	// DPC調整モード初期化
	OUT32 (FIRM_DATA_DPC_ADJUST_MODE_ADRS, DPC_ADJUST_MODE_DEFAULT);

	// DPC初期化
	if ((status = dpcInitialize (dpcFlash, dpcMemAdrs, dpcEnable)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// 欠陥画素最大補正数設定
	//------------------------------------------------------------
	OUT32 (FIRM_DATA_DPC_MAX_NUM_COUNT_ADRS, NUM_DEFECTION_PIX);

	//------------------------------------------------------------
	// ステータス設定(DPC固有)
	//------------------------------------------------------------
	OUT32 (FIRM_DATA_DPC_STATUS_ADRS, 0);

_DONE:
	return (status);
}
#endif


//**********************************************************************************
//	Camera Global Interrupt設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Global割り込み設定
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ipuSetGlobalInt (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check data Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "IPU Global Interrupt mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// グローバル割り込み取得
	data = IN32 (FPGA_IPU_IRQ_CTRL_ADRS);

	if (mode == MODE_ENABLE)
		data |= FPGA_IPU_IRQ_CTRL_ENABLE_BIT;
	else
		data &= ~FPGA_IPU_IRQ_CTRL_ENABLE_BIT;

	// グローバル割り込み設定
	OUT32 (FPGA_IPU_IRQ_CTRL_ADRS, data);

_DONE:
	return (status);
}


//**********************************************************************************
//	割り込み有効
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data					：割り込み許可(1=許可/0=何もしない)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int ipuIntEnable (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int temp;

	// 割り込みEnableレジスタ取得
	temp = IN32 (FPGA_CAMERA_IRQ_MASK_ADRS);

	// 割り込みEnableレジスタ設定
	temp |= data;
	OUT32 (FPGA_IPU_IRQ_MASK_ADRS, temp);

	return (status);
}


//**********************************************************************************
//	割り込み禁止
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data					：割り込み禁止(1=禁止/0=何もしない)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int ipuIntDisable (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int temp;

	// 割り込みEnableレジスタ取得
	temp = IN32 (FPGA_CAMERA_IRQ_MASK_ADRS);

	// 割り込みEnableレジスタ設定
	temp &= ~data;
	OUT32 (FPGA_IPU_IRQ_MASK_ADRS, temp);

	return (status);
}


//**********************************************************************************
//	割り込みステータス取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		割り込みステータス
//==================================================================================
unsigned int ipuIntStatus (void)
{
	unsigned int data;

	// 割り込みステータス取得
	data = IN32 (FPGA_IPU_IRQ_STATUS_ADRS);

	return (data);
}


//**********************************************************************************
//	割り込みクリア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data					：割り込みクリアデータ
//	[ OUTPUT ]
//		割り込みステータス
//==================================================================================
void ipuIntClear (unsigned int data)
{
	// 割り込みクリア
	OUT32 (FPGA_IPU_IRQ_STATUS_ADRS, data);
}

// eof
