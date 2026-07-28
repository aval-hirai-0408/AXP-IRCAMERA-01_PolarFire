//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// gigeCmdDpc.c - GigE Command DPC Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined(MODE_DPC)
//**********************************************************************************
//	DPC1コマンドメイン
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		ffcNum				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdDpc1Main (int dpcNum, int ffcNum)
{
	int status = AVAL_STATUS_SUCCESS;
	DPC_PARAM dpcParam;
	int dpcMode;

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

	// DPC Adjust Mode取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPC
	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
	{
		if ((status = dpcMainBatch1 (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	#if 0	//@@@1
	else if (dpcMode == DPC_ADJUST_MODE_NE)
	{
		count = dpcGetBatchCount ();
		if (count != 0)
			goto _DONE;

		// FFC無効設定
		if ((status = dpcSetFfcEnableMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC1
		if ((status = dpcMainBatch1Ne (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if (dpcMode == DPC_ADJUST_MODE_NE_FFC_ENABLE)
	{
		count = dpcGetBatchCount ();
		if (count != 0)
			goto _DONE;

		// FFC有効設定
		if ((status = dpcSetFfcEnableMode (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC1
		if ((status = dpcMainBatch1Ne (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if (dpcMode == DPC_ADJUST_MODE_LINE)
	{
		count = dpcGetBatchCount ();
		if (count != 0)
			goto _DONE;

		// FFC無効設定
		if ((status = dpcSetFfcEnableMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC1
		if ((status = dpcMainBatch1Ne (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if (dpcMode == DPC_ADJUST_MODE_LINE_FFC_ENABLE)
	{
		count = dpcGetBatchCount ();
		if (count != 0)
			goto _DONE;

		// FFC有効設定
		if ((status = dpcSetFfcEnableMode (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// DPC1
		if ((status = dpcMainBatch1Ne (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	#endif // #if 0	//@@@1

_DONE:
	return (status);
}


//**********************************************************************************
//	DPC2コマンド
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//		ffcNum				：FFC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdDpc2Main (int dpcNum, int ffcNum)
{
	int status = AVAL_STATUS_SUCCESS;
	DPC_PARAM dpcParam;
	int dpcMode;
	int data32;

	//--------------------------------------------------------------------------------
	// 画素欠陥補正
	//--------------------------------------------------------------------------------

	// クリア
	memset ((void *)&dpcParam, 0, sizeof(dpcParam));

	// パラメータ設定
	dpcParam.dpcNo = dpcNum;
	dpcParam.ffcNo = ffcNum;
	//@@@1 = MODE_BATCH;
	dpcParam.frameRate = 0;
	dpcParam.exposure = 0;
	dpcParam.bit = 0;
	dpcParam.sensorTemp = 0;
	dpcParam.memType = DPC_MEMORY_EXT;
	dpcParam.userMode = DPC_USER;
	dpcParam.sd = -1;
	dpcParam.nonuniformity = -1;

	// DPC Adjust Mode取得
	if ((status = dpcGetAdjustMode (&dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DPC
	if (dpcMode == DPC_ADJUST_MODE_NOMAL)
	{
		if ((status = dpcMainBatch2 (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	#if 0	//@@@1
	else if ((dpcMode == DPC_ADJUST_MODE_NE) || (dpcMode == DPC_ADJUST_MODE_NE_FFC_ENABLE))
	{
		#if defined (MODE_DPC_NONUNIFORM_SPECTRUM_MODE)
		// 感度不均一
		data32 = IN32 (FIRM_CMD_PARAM2_ADRS);
		dpcParam.nonuniformity = (double)((double)data32 / (double)DPC_NONUNIFORM_UNIT);
		#endif

		if ((status = dpcMainBatch12Ne (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((status = dpcMainBatch2Ne (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if ((dpcMode == DPC_ADJUST_MODE_LINE) || (dpcMode == DPC_ADJUST_MODE_LINE_FFC_ENABLE))
	{
		// 感度不均一
		data32 = IN32 (FIRM_CMD_PARAM2_ADRS);
		dpcParam.nonuniformity = (double)((double)data32 / (double)DPC_NONUNIFORM_UNIT);

		// 標準偏差
		data32 = IN32 (FIRM_CMD_PARAM3_ADRS);
		dpcParam.sd = (double)((double)data32 / (double)DPC_SD_UNIT);

		if ((status = dpcMainBatch12Ne (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((status = dpcMainBatch2Ne (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	#endif // #if 0	//@@@1


_DONE:
	return (status);
}


//**********************************************************************************
//	DPC Defect
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		dpcNum				：DPC番号
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeCmdDpc3Main (int dpcNum)
{
	int status = AVAL_STATUS_SUCCESS;
	DPC_PARAM dpcParam;
	int saveLed0 = -1;
	int dpcMode;
	
	// 調整ステート
	saveLed0 = ledSettingState ();

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
	#if 0 //@@@1
	else if ((dpcMode == DPC_ADJUST_MODE_NE) || (dpcMode == DPC_ADJUST_MODE_NE_FFC_ENABLE))
	{
		if ((status = dpcMainBatch3Ne (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if ((dpcMode == DPC_ADJUST_MODE_LINE) || (dpcMode == DPC_ADJUST_MODE_LINE_FFC_ENABLE) || (dpcMode == DPC_ADJUST_MODE_LINE_FFC_ENABLE))
	{
		if ((status = dpcMainBatch3Ne (dpcParam)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	#endif //#if 0 //@@@1

_DONE:
	// LEDの設定を元に戻す
	ledReturnState (saveLed0, -1);

	return (status);
}

#endif // #if defined(MODE_DPC)

// eof
