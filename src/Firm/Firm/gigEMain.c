//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// gigEMain.c - Main Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern unsigned int gTickCountTotalTempAbnormalCheck;


//**********************************************************************************
//	GigE Main
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeMain (void)
{
	#if defined (MODE_PELTIER_CTRL)
	unsigned int peltierStartFlag;
	#endif

	// コマンド&エラー領域クリア
	//@@@1gigeSetStatus (FIRM_CMD_OK, 0);

	msDelay (2000);

	// Main Loop
	while (1)
	{
		//------------------------------------------------------------
		// Check Console
		//------------------------------------------------------------
		firmCheckConsole ();


		//------------------------------------------------------------
		// 温度異常check
		//------------------------------------------------------------
#if defined (MODE_TEMP_ABNORMAL_CHECK)
		if (gTickCountTotalTempAbnormalCheck > TEMP_ABNORMAL_CHECK_INTERVAL)
		{
			tempAbnormalCheck ();
			gTickCountTotalTempAbnormalCheck = 0;
		}
#endif // MODE_TEMP_ABNORMAL_CHECK


		//------------------------------------------------------------
		// Peltierコントロール
		//------------------------------------------------------------
#if defined (MODE_PELTIER_CTRL)
		// Get Start Flag
		peltierGetPowerStartFlag (&peltierStartFlag);

		if (peltierStartFlag == MODE_ENABLE)
		{
			peltierControlProcs ();
			peltierSetPowerStartFlag (MODE_DISABLE);
		}
#endif // MODE_PELTIER_CTRL


		//------------------------------------------------------------
		// GigE コマンド処理
		//------------------------------------------------------------
		//@@@1gigeCmd ();
	}

	return (AVAL_STATUS_SUCCESS);
}

// eof
