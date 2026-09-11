//**********************************************************************************
//
//                               Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// sensorVendorS.c - Sensor Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)

//----------------------------------------------------------------------------------
// IMX992/IMX993
//----------------------------------------------------------------------------------
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "sensorVendors.h"
#include "../Cxp\cxp.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define FPGA_LVDS_TMG_DATA_SAMPLING_COUNT					(256)	// サンプリングする回数
#define FPGA_LVDS_SIGNAL_COUNT								(9)		// LVDS信号本数
#define FPGA_LVDS_ON_OFF_COUNT								(4)		// ON OFF状態を格納する個数
#define FPGA_LVDS_TMG_CTRL_DLY_CNT							(256)


//----------------------------------------------------------------------------------
// structs
//----------------------------------------------------------------------------------

#define MODE_ON			(1)
#define MODE_OFF		(0)

// LVDS On/Off
typedef struct {
	int start[FPGA_LVDS_ON_OFF_COUNT];			// 添え字：0=１個目の変化/1=2個目の変化
	int length[FPGA_LVDS_ON_OFF_COUNT];
} LVDS_OnOff_St;


// LVDS Info
typedef struct {
	LVDS_OnOff_St onOff[2];						// 添え字：0=Onデータ/1=Offデータ
	int center;
} LVDS_Info_St;

unsigned char gSensorMatchData[FPGA_LVDS_SIGNAL_COUNT][FPGA_LVDS_TMG_DATA_SAMPLING_COUNT];
LVDS_Info_St gLvdsInfo[FPGA_LVDS_SIGNAL_COUNT];


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int gSensorTmgStatus;
extern int gSensorStatus;


//**********************************************************************************
// Sensor LVDS Timing Initialize
//   1:256回のサンプリングで違いがあった
//   2:0もしくは複数のbitが1(Signalは除く)
//   3:代表値に違いがあった(上記1,2で変化が無い場合のみCheck)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorTimingInitialize (void)
{
	int gSensorTmgStatus = AVAL_STATUS_SUCCESS;
	unsigned char id;
	int regHold, acTmg;
	unsigned char data8;
	unsigned short data16;
	unsigned char saveData8_20h, saveData8_40h;
	int signal, delay;
	int sensor;
	int onCount, offCount, count;
	int now, current, nextCount;
	int lenTemp;
	int j;
	int center;
	double rateMaxD;
	int bitSave = -1;

	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, 0, "Sensor Timing Initialize\n");

	//------------------------------------------------------------
	// 1hの時間取得
	//------------------------------------------------------------
	//oneH = (SENSOR_HINTERVAL_CLOCK_10BIT * SENSOR_H_TIME) + 0.5/*四捨五入*/;


	//------------------------------------------------------------
	// ①センサOutput Enable Disable
	//------------------------------------------------------------
	for (sensor=0; sensor<SENSOR_NUM; sensor++)
		OUT32 ((FPGA_SENSOR_OUTPUT_CTRL_ADRS + sensor * FPGA_SENSOR_REG_SERDES_INTERVAL), 0);

	//------------------------------------------------------------
	// ②センサテストパターン設定
	//------------------------------------------------------------

	//------------------------------------------------------------
	// Sensor Standby
	//------------------------------------------------------------
	if ((gSensorTmgStatus = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//============================================================
	// Chip ID = 0x07
	//============================================================
	id = 0x07;

	//------------------------------------------------------------
	// PG Mode
	//------------------------------------------------------------
	data8 = 8; // Checks pattern of the arbitrary valueOthers: Setting prohibited
	if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_PG_MODE_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// PG DATA1
	//------------------------------------------------------------
	data16 = 0x5555 & SENSOR_REG_PG_DATA1_MASK;
	data8 = (unsigned char)(data16 & 0xff);
	if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_PG_DATA1_L_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 = (unsigned char)(data16>>8 & 0xff);
	if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_PG_DATA1_H_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// PG DATA2
	//------------------------------------------------------------
	data16 = 0xaaaa & SENSOR_REG_PG_DATA2_MASK;
	data8 = (unsigned char)(data16 & 0xff);
	if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_PG_DATA2_L_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 = (unsigned char)(data16>>8 & 0xff);
	if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_PG_DATA2_H_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Fix 0x5F
	//------------------------------------------------------------
	if ((gSensorTmgStatus = sensorRegReadByte (id, SENSOR_REG_FIX1_ADRS, &saveData8_20h, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 = 0x5F;
	if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_FIX1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Fix 0x00
	//------------------------------------------------------------
	if ((gSensorTmgStatus = sensorRegReadByte (id, SENSOR_REG_FIX2_ADRS, &saveData8_40h, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 = 0x00;
	if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_FIX2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// PG Enable
	//------------------------------------------------------------
	if ((gSensorTmgStatus = sensorRegReadByte (id, SENSOR_REG_PG_CTRL_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 |= SENSOR_REG_PG_CTRL_MODE;
	data8 &= ~SENSOR_REG_PG_THRU_MODE;
	data8 |= SENSOR_REG_PG_CLK_MODE;

	if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_PG_CTRL_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Sensor Standby解除
	//------------------------------------------------------------
	if ((gSensorTmgStatus = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// ③フレームレート最速(10bit)
	//------------------------------------------------------------

	// 画像入力停止
	if ((gSensorTmgStatus = acquisitionStop ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 撮像モード設定
	if ((gSensorTmgStatus = acquisitionSetMode (ACQUISITION_MODE_CONTIN)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// bit取得
	if ((gSensorTmgStatus = aoiGetBitWidth (&bitSave)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最大レート取得
	if ((gSensorTmgStatus = rateMax (&rateMaxD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// bit設定
	//if ((gSensorTmgStatus = aoiSetBitWidth (10)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// 露光時間設定
	//if ((gSensorTmgStatus = acquisitionSetExposure (20)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// フレームレート設定
	if ((gSensorTmgStatus = acquisitionSetFrameRate (rateMaxD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 画像入力開始
	if ((gSensorTmgStatus = acquisitionStart ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	for (sensor=0; sensor<SENSOR_NUM; sensor++)
	{
	    DEBUG_PRINT ("\n[Sensor%d Signal Data]\n", sensor);

		//------------------------------------------------------------
		// ④の処理
		//------------------------------------------------------------
		for (signal=0; signal<FPGA_LVDS_SIGNAL_COUNT; signal++)
		{
			for (delay=0; delay<FPGA_LVDS_TMG_DATA_SAMPLING_COUNT; delay++)
			{
				//------------------------------------------------------------
				// ④-1 or ④-4 Delay Tap設定
				//------------------------------------------------------------
				OUT8 ((FPGA_SENSOR_REG_SERDES_CTRL0_BYTE0_ADRS + signal * 4 + sensor * FPGA_SENSOR_REG_SERDES_INTERVAL), delay);


				//------------------------------------------------------------
				// ④-2 match clr制御
				//------------------------------------------------------------
				OUT8 ((FPGA_SENSOR_REG_SERDES_CTRL0_BYTE3_ADRS + signal * 4 + sensor * FPGA_SENSOR_REG_SERDES_INTERVAL), FPGA_SENSOR_REG_SERDES_CTRL_MATCH_CLR);

				//@@@usDelay (oneH);
				msDelay (2);

				OUT8 ((FPGA_SENSOR_REG_SERDES_CTRL0_BYTE3_ADRS + signal * 4 + sensor * FPGA_SENSOR_REG_SERDES_INTERVAL), 0);


				//------------------------------------------------------------
				// ④-3 match clr制御
				//------------------------------------------------------------
				//usDelay ((oneH * 2));
				msDelay (2);

				data8 = IN8 ((FPGA_SENSOR_REG_SERDES_CTRL0_BYTE1_ADRS + signal * 4 + sensor * FPGA_SENSOR_REG_SERDES_INTERVAL)) & FPGA_SENSOR_REG_SERDES_CTRL_MATCH_BIT;

				if (data8 == 0)
					gSensorMatchData[signal][delay] = 0;
				else
					gSensorMatchData[signal][delay] = 1;

				if (delay == 0)
				    DEBUG_PRINT ("Line%0d : ", signal);

				DEBUG_PRINT ("%d ", gSensorMatchData[signal][delay]);
			}

			DEBUG_PRINT ("\n\n");
		}

		//------------------------------------------------------------
		// ⑤の処理
		//------------------------------------------------------------
		// 信号本数分ループ
		for (signal=0; signal<FPGA_LVDS_SIGNAL_COUNT; signal++)
		{
			// 初期化
			onCount = 0;
			offCount = 0;

			// 先頭確認
			if (gSensorMatchData[signal][0] == 0)
				now = 0;
			else
				now = 1;

			// スタート位置
			gLvdsInfo[signal].onOff[now].start[0] = 0;

			// 長さ
			lenTemp = 1;

			// 信号変化確認
			//onOffChangeStatus = 0;

			// 長さを確認(0番目は初期値として上記で確認済み。したがってdelay=1から開始。)
			for (delay=1; delay<FPGA_LVDS_TMG_CTRL_DLY_CNT; delay++)
			{
				if (gSensorMatchData[signal][delay] == 0)
					current = 0;
				else
					current = 1;

				if (now == current)
				{
					// 長さ++
					lenTemp++;

					// 最後
					if (delay == (FPGA_LVDS_TMG_CTRL_DLY_CNT-1))
					{
						if (now == 0)				// 0の状態
						{
							count = offCount;		// データOff格納番号
							offCount++;				// 次回に備えインクリメント
						}
						else						// 1の状態
						{
							count = onCount;		// データOｎ格納番号
							onCount++;				// 次回に備えインクリメント
						}

						// 情報を格納できる個数を確認
						if (count >= FPGA_LVDS_ON_OFF_COUNT)
						{
							DEBUG_PRINT ("Sensor Tmg On Off Count Error[1]. Count = %d / Now = %d\n", count, now);
							gSensorTmgStatus = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
							//goto _DONE;
							continue;
						}

						// 終了
						gLvdsInfo[signal].onOff[now].length[count] = lenTemp;

						#if 0
						// 先頭と最後は同じ?
						//if (gSensorMatchData[signal][0] == gSensorMatchData[signal][(FPGA_LVDS_TMG_CTRL_DLY_CNT-1)])
						if (((gSensorMatchData[signal][0] != 0) && (gSensorMatchData[signal][(FPGA_LVDS_TMG_CTRL_DLY_CNT-1)] != 0)) ||
							((gSensorMatchData[signal][0] == 0) && (gSensorMatchData[signal][(FPGA_LVDS_TMG_CTRL_DLY_CNT-1)] == 0)))
						{
							// カウントが0ならばOnまたはOffが１回以上無い。
							if (onOffChangeStatus != 0)
							{
								// 同じならば、先頭と終端がつながっていると判断し、終端に先頭Lengthを加算する
	                            // countは現在の位置
								gLvdsInfo[signal].onOff[now].length[count] += gLvdsInfo[signal].onOff[now].length[0];

								// １個目のLengthは初期化
								gLvdsInfo[signal].onOff[now].length[0] = 0;
							}
						}
						#endif
					}	// if (signal == (FPGA_LVDS_SIGNAL_COUNT-1))
				}	// if (now == current)
				else // 変化あり
				{
					// 信号変化を確認
					//onOffChangeStatus = 1;

					if (current == 0)	// 0に変化した。即ち今まで1だった。
					{
						count = onCount;		// データOn格納番号
						nextCount = offCount;	// 次はOff
						onCount++;				// 次回に備えインクリメント
					}
					else				// 1に変化した。即ち今まで0だった。
					{
						count = offCount;		// データOff格納番号
						nextCount = onCount;	// 次はOn
						offCount++;				// 次回に備えインクリメント
					}

					// 情報を格納できる個数を確認
					if (count >= FPGA_LVDS_ON_OFF_COUNT)
					{
						DEBUG_PRINT ("Sensor Tmg On Off Count Error[2]. Count = %d / Now = %d / Current = %D\n", count, now, current);
						gSensorTmgStatus = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
						//goto _DONE;
						continue;
					}

					// 長さを設定
					gLvdsInfo[signal].onOff[now].length[count] = lenTemp;

					// 情報を格納できる個数を確認
					if (nextCount >= FPGA_LVDS_ON_OFF_COUNT)
					{
						DEBUG_PRINT ("Sensor Tmg On Off Count Error[3]. Count = %d / Now = %d / Current = %D\n", nextCount, now, current);
						gSensorTmgStatus = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
						//goto _DONE;
						continue;
					}

					// 次の開始
					gLvdsInfo[signal].onOff[current].start[nextCount] = delay;

					// 次のカウント初期値
					lenTemp = 1;

					// 更新
					now = current;

					// 最後
					if (delay == (FPGA_LVDS_TMG_CTRL_DLY_CNT-1))
					{
						// 最終カウント
						gLvdsInfo[signal].onOff[current].length[nextCount] = lenTemp;

						#if 0
						// 先頭と最後は同じ?
						//if (gSensorMatchData[signal][0] == gSensorMatchData[signal][(FPGA_LVDS_TMG_CTRL_DLY_CNT-1)])
						if (((gSensorMatchData[signal][0] != 0) && (gSensorMatchData[signal][(FPGA_LVDS_TMG_CTRL_DLY_CNT-1)] != 0)) ||
							((gSensorMatchData[signal][0] == 0) && (gSensorMatchData[signal][(FPGA_LVDS_TMG_CTRL_DLY_CNT-1)] == 0)))
						{
							// カウントが0ならばOnまたはOffが１回以上無い。
							if (onOffChangeStatus != 0)
							{
								// 同じならば、先頭と終端がつながっていると判断し、終端に先頭Lengthを加算する
								gLvdsInfo[signal].onOff[now].length[nextCount] += gLvdsInfo[signal].onOff[now].length[0];

								// １個目のLengthは初期化
								gLvdsInfo[signal].onOff[now].length[0] = 0;
							}
						}
						#endif
					}
				}
			}	// for (delay=1; delay<FPGA_LVDS_SIGNAL_COUNT; delay++)

			// データの有効性の確認(0の状態を確認)
			if ((gLvdsInfo[signal].onOff[MODE_OFF].length[0] == 0) && (gLvdsInfo[signal].onOff[MODE_OFF].length[1] == 0) &&
	            (gLvdsInfo[signal].onOff[MODE_OFF].length[2] == 0) && (gLvdsInfo[signal].onOff[MODE_OFF].length[3] == 0))
			{
				gSensorTmgStatus = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
				sprintf (gLogMsgBuff, "Sensor Tmg Delay Data Invalid. Signal = %d\n", signal);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, gSensorTmgStatus, gLogMsgBuff);
				//goto _DONE;
			}

			// 変化がなかった
			if ((gLvdsInfo[signal].onOff[MODE_OFF].length[0] == FPGA_LVDS_TMG_CTRL_DLY_CNT) || (gLvdsInfo[signal].onOff[MODE_ON].length[0] == FPGA_LVDS_TMG_CTRL_DLY_CNT))
			{
				center = FPGA_LVDS_TMG_CTRL_DLY_CNT / 2;
			}
			else	// 変化あった
			{
				int saveOnOffLen, onOffLen;
				int saveCount;

				// 一番大きいものを検索
				saveOnOffLen = 0;
				saveCount = 0;
				for (j=0; j<FPGA_LVDS_ON_OFF_COUNT; j++)
				{
					onOffLen = gLvdsInfo[signal].onOff[MODE_ON].length[j];
					if (saveOnOffLen < onOffLen)
					{
						saveOnOffLen = onOffLen;
						saveCount = j;
					}
				}

				// 中心位置を計算
				center = (gLvdsInfo[signal].onOff[MODE_ON].start[saveCount] + gLvdsInfo[signal].onOff[MODE_ON].length[saveCount] / 2) % FPGA_LVDS_TMG_CTRL_DLY_CNT;
			}

			// save
			gLvdsInfo[signal].center = center;

			// Delay設定
			OUT8 ((FPGA_SENSOR_REG_SERDES_CTRL0_BYTE0_ADRS + signal * 4 + sensor * FPGA_SENSOR_REG_SERDES_INTERVAL), center);

		}	// for (signal=1; signal<FPGA_LVDS_SIGNAL_COUNT; signal++)

		//------------------------------------------------------------
		// OnOff情報表示
		//------------------------------------------------------------
		// 信号本数分ループ
		for (signal=0; signal<FPGA_LVDS_SIGNAL_COUNT; signal++)
		{
			// 情報表示
			if (signal == 0)
			{
			    DEBUG_PRINT ("[Information]\n");
			}

		    DEBUG_PRINT ("Line%0d :  ", signal);

			DEBUG_PRINT ("Center=%d\n", gLvdsInfo[signal].center);

		    DEBUG_PRINT ("  ");
			for (j=0; j<FPGA_LVDS_ON_OFF_COUNT; j++)
			{
				DEBUG_PRINT ("Off[%d]:Sta=%2d, Len=%2d", j, gLvdsInfo[signal].onOff[MODE_OFF].start[j], gLvdsInfo[signal].onOff[MODE_OFF].length[j]);
				if  (j!=(FPGA_LVDS_ON_OFF_COUNT-1))
					DEBUG_PRINT (" / ");
			}
		    DEBUG_PRINT ("\n");

		    DEBUG_PRINT ("  ");
			for (j=0; j<FPGA_LVDS_ON_OFF_COUNT; j++)
			{
				DEBUG_PRINT ("On [%d]:Sta=%2d, Len=%2d", j, gLvdsInfo[signal].onOff[MODE_ON].start[j], gLvdsInfo[signal].onOff[MODE_ON].length[j]);
				if  (j!=(FPGA_LVDS_ON_OFF_COUNT-1))
					DEBUG_PRINT (" / ");
			}

		    DEBUG_PRINT ("\n");
		}

	} // for (sensor=0; sensor<SENSOR_NUM; sensor++)

_DONE:
	//------------------------------------------------------------
	// 画像入力停止
	//------------------------------------------------------------
	acquisitionStop ();

	//------------------------------------------------------------
	// センサOutput Enable
	//------------------------------------------------------------
	for (sensor=0; sensor<SENSOR_NUM; sensor++)
		OUT32 ((FPGA_SENSOR_OUTPUT_CTRL_ADRS + sensor * FPGA_SENSOR_REG_SERDES_INTERVAL), FPGA_SENSOR_OUTPUT_CTRL_ENABLE_BIT);

	//------------------------------------------------------------
	// Sensor Standby
	//------------------------------------------------------------
	sensorStandBy ();

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	id = 7;
	//------------------------------------------------------------
	// Fix 0x5F
	//------------------------------------------------------------
	if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_FIX1_ADRS, saveData8_20h, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Fix 0x00
	//------------------------------------------------------------
	if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_FIX2_ADRS, saveData8_40h, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// PG Disable
	//------------------------------------------------------------
	sensorRegReadByte (id, SENSOR_REG_PG_CTRL_ADRS, &data8, acTmg, regHold);

	data8 &= ~SENSOR_REG_PG_CTRL_MODE;
	data8 |= SENSOR_REG_PG_THRU_MODE;
	data8 &= ~SENSOR_REG_PG_CLK_MODE;

	sensorRegWriteByte (id, SENSOR_REG_PG_CTRL_ADRS, data8, acTmg, regHold);

	//------------------------------------------------------------
	// Sensor Standby解除
	//------------------------------------------------------------
	sensorStandByCancel ();

	return (gSensorTmgStatus);
}


//**********************************************************************************
//	センサ初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorVendorSInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
#if defined (MODE_SENSOR_XFLIP)
	int datai32;
#endif
	int regHold, acTmg;
	unsigned char id;
	unsigned int bitData;
	unsigned char data8;
	int bit;
	unsigned int data32;
	int shutterMode;
	
	DEBUG_PRINT ("Sensor Initialize\n");

	//------------------------------------------------------------
	// BlackPixel
	//------------------------------------------------------------
	OUT32 (FIRM_DATA_BLACKPIXEL_MODE_ADRS, 1);

	//--------------------------------------------------------------------------------
	// Sensor Gain DDR初期化
	//--------------------------------------------------------------------------------
	OUT32(FIRM_DATA_SENSOR_GAIN_ADRS, SENSOR_GAIN_UNIT);	// 1倍

	//--------------------------------------------------------------------------------
	// CXP 3.125G / Single
	// hInterval設定用。初期化時に自走するため、ConectionConfig Default値を設定。
	// ここの値はCXPでのレート/Single or Dual設定時に更新され、
	// またHintervalも変更する必要がある
	//--------------------------------------------------------------------------------
#if defined (IF_CXP)
	cxpSetRateData ((CXP_PORT_SINGLE << 16)  | CXP_RATE_3_125G);
#endif

	//------------------------------------------------------------
	// Bit取得
	//------------------------------------------------------------
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Power Status
	OUT32 (FIRM_DATA_SENSOR_POWER_STATUS_ADRS, 1);


	msDelay (100);


	//============================================================
	//============================================================
	//============================================================
	// Sensor側の初期化開始
	//============================================================
	//============================================================
	//============================================================

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//============================================================
	// Chip ID = 0x02
	//============================================================
	id = 0x02;

	//------------------------------------------------------------
	// StandBy
	//------------------------------------------------------------
	data8 = SENSOR_REG_STANDBY_BIT;	// Standbyモード
	if ((status = sensorRegWriteByte (id, SENSOR_REG_STANDBY_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// Master/Slaveモード(Masterの時には最後に解除)
	//------------------------------------------------------------
	data8 = SENSOR_REG_XMSTA_BIT;	// Slaveモード
	if ((status = sensorRegWriteByte (id, SENSOR_REG_XMSTA_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// Register Hold
	//------------------------------------------------------------
	// ここでは未設定


	//------------------------------------------------------------
	// Windowモード
	//------------------------------------------------------------
	data8 = 0;	// 全画素
	if ((status = sensorRegWriteByte (id, SENSOR_REG_WINMODE_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// VMAX1
	//------------------------------------------------------------
	// マスタモード時のみ
	data8 = (unsigned char)(SENSOR_VINTERVAL_LINE & 0xff);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_VMAX1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// VMAX2
	//------------------------------------------------------------
	// マスタモード時のみ
	data8 = (unsigned char)((SENSOR_VINTERVAL_LINE >> 8) & 0xff);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_VMAX2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// VMAX3
	//------------------------------------------------------------
	// マスタモード時のみ
	data8 = (unsigned char)((SENSOR_VINTERVAL_LINE >> 16) & 0xff);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_VMAX3_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	
	//------------------------------------------------------------
	// HMAX1
	//------------------------------------------------------------
	// マスタモード時のみ
	if (bit == 10)
		bitData = SENSOR_HINTERVAL_CLOCK_10BIT;
	else if (bit == 12)
		bitData = SENSOR_HINTERVAL_CLOCK_12BIT;
	else
		bitData = SENSOR_HINTERVAL_CLOCK_8BIT;

	#if defined (MODE_SENSOR_MASTER)
	bitData = SENSOR_HINTERVAL_CLOCK_12BIT;
	#endif

	data8 = (unsigned char)(bitData & 0xff);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_HMAX1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// HMAX2
	//------------------------------------------------------------
	// マスタモード時のみ
	data8 = (unsigned char)((bitData >> 8) & 0xff);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_HMAX2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// FREQ
	//------------------------------------------------------------
#if defined (FREQ_1)
	data8 = 1;
#else
	data8 = 0;	// ノーマル
#endif
	if ((status = sensorRegWriteByte (id, SENSOR_REG_FREQ_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// GSDLY
	//------------------------------------------------------------
	data8 = SENSOR_REG_GSDLY_DEFAULT;
	if ((status = sensorRegWriteByte (id, SENSOR_REG_GSDLY_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// GTWAIT0
	//------------------------------------------------------------
	data8 = SENSOR_REG_GTWAIT_DEFAULT;
	if ((status = sensorRegWriteByte (id, SENSOR_REG_GTWAIT0_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// GTWAIT1
	//------------------------------------------------------------
	data8 = (SENSOR_REG_GTWAIT_DEFAULT >> 8) & SENSOR_REG_GTWAIT1_MASK;
	if ((status = sensorRegWriteByte (id, SENSOR_REG_GTWAIT1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x03
	//============================================================
	id = 0x03;

	//------------------------------------------------------------
	// ON1
	//------------------------------------------------------------
	data8 = 0;	// 無効
	if ((status = sensorRegWriteByte (id, SENSOR_REG_ROI_ON1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// ON2
	//------------------------------------------------------------
	data8 = 0;	// 無効
	if ((status = sensorRegWriteByte (id, SENSOR_REG_ROI_ON2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// ROI関連はここでは初期化しない
	//------------------------------------------------------------


	//============================================================
	// Chip ID = 0x04
	//============================================================
	id = 0x04;

	//------------------------------------------------------------
	// AD
	//------------------------------------------------------------
	if ((status = sensorSetAD (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// Reverse
	//------------------------------------------------------------
	data8 = 0;	// 正転
	if ((status = sensorRegWriteByte (id, SENSOR_REG_REVERSE_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// INCKSEL0
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// INCKSEL1
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// INCKSEL2
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// INCKSEL3
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// Freq Sync
	//------------------------------------------------------------
	// デフォルトの為、設定しない
#if defined (FREQ_1)
	if ((status = sensorRegWriteByte (id, SENSOR_REG_FREQ_SYNC_ADRS, 0x92, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

	//------------------------------------------------------------
	// First Trg1
	//------------------------------------------------------------
	if ((status = sensorRegReadByte (id, SENSOR_REG_FIRST_TRG1_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

	data8 &= ~SENSOR_REG_FIRST_TRG1_FIRST_BIT;	// Fast以外

	if ((status = sensorRegWriteByte (id, SENSOR_REG_FIRST_TRG1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// First Trg2
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// VINT EN
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// SHS0
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// SHS1
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// SHS2
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//============================================================
	// Chip ID = 0x06
	//============================================================
	id = 0x06;

	//------------------------------------------------------------
	// TRG EN
	//------------------------------------------------------------
#if !defined (MODE_SENSOR_MASTER)
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (shutterMode == SHUTTER_MODE_TRIGGER)
		data8 = SENSOR_REG_TRG_EN_BIT;
	else
		data8 = 0;

	if ((status = sensorRegWriteByte (id, SENSOR_REG_TRG_EN_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#else
	shutterMode = SHUTTER_MODE_NORMAL;
#endif

	//------------------------------------------------------------
	// OD Bit
	//------------------------------------------------------------
	if ((status = sensorSetOB (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// TOUT
	//------------------------------------------------------------
	// 未使用の為、設定しない


	//------------------------------------------------------------
	// TRG TOUT
	//------------------------------------------------------------
	// 未使用の為、設定しない


	//------------------------------------------------------------
	// SYNC SEL
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// STB SLVS
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// OPORT SEL
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// Pulse 1
	//------------------------------------------------------------
	// 未使用の為、設定しない


	//------------------------------------------------------------
	// Pulse 1 UP
	//------------------------------------------------------------
	// 未使用の為、設定しない


	//------------------------------------------------------------
	// Pulse 1 DN
	//------------------------------------------------------------
	// 未使用の為、設定しない


	//------------------------------------------------------------
	// Pulse 2
	//------------------------------------------------------------
	// 未使用の為、設定しない


	//------------------------------------------------------------
	// Pulse 2 UP
	//------------------------------------------------------------
	// 未使用の為、設定しない


	//------------------------------------------------------------
	// Pulse 2 DN
	//------------------------------------------------------------
	// 未使用の為、設定しない


	//------------------------------------------------------------
	// TEMP OUT
	//------------------------------------------------------------
	// Read Only


	//------------------------------------------------------------
	// EXPOPRD_DLY
	//------------------------------------------------------------
	// 未使用の為、設定しない

	//============================================================
	// Chip ID = 0x07
	//============================================================
	id = 0x07;


	//------------------------------------------------------------
	// Gain RTS
	//------------------------------------------------------------
	data8 = SENSOR_REG_GAIN_RTS_FRAME_LATE;	// 1 Frame遅れ
	if ((status = sensorRegWriteByte (id, SENSOR_REG_GAIN_RTS_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// GAINUPDSL
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// FDG_SEL
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// Gain
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// TMD Lutch Sel
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//------------------------------------------------------------
	// Black Level
	//------------------------------------------------------------
	if ((status = sensorSetBL (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x09
	//============================================================
	id = 0x09;

	//------------------------------------------------------------
	// TTOUT0_EXPOPRDEN
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//============================================================
	// Chip ID = 0x0e
	//============================================================
	id = 0x0e;

	//------------------------------------------------------------
	// DPHY_CKLANE_MODE
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// TCLKESC_FREQ
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// THS_PREPARE
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// TCLK_POST
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// THS_TRAIL
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// THS_ZERO
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// TCLK_PREPARE
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// TCLK_TRAIL
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// TLPX
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// TCLK_ZERO
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// TCLK_PRE
	//------------------------------------------------------------
	// デフォルトの為、設定しない

	//------------------------------------------------------------
	// THS_EXIT
	//------------------------------------------------------------
	// デフォルトの為、設定しない


	//============================================================
	// Chip ID = 0x1c
	//============================================================
	id = 0x1c;

	//------------------------------------------------------------
	// VR_EN
	//------------------------------------------------------------
	// ACB-930マイコンFWで制御

	//------------------------------------------------------------
	// VR_RG_DIS
	//------------------------------------------------------------
	// ACB-930マイコンFWで制御


	//------------------------------------------------------------
	// X Flip
	//------------------------------------------------------------
#if defined (MODE_SENSOR_XFLIP)
	if ((status = aoiGetXflip (&datai32)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (datai32 == MODE_ENABLE)
	{
		if ((status = sensorSetXFlip (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif


	//------------------------------------------------------------
	// 隠しレジスタ初期化
	//------------------------------------------------------------
	if ((status = sensorVendorSHideRegSetting ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = sensorBitSetHide (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	//============================================================
	//============================================================
	// Sensor側の終了
	//============================================================
	//============================================================
	//============================================================


	//------------------------------------------------------------
	// Gradiation Compress
	//------------------------------------------------------------
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	if ((status = sensorGradationCompInitialize ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif


#if defined (MODE_SPECTRUM)
	//------------------------------------------------------------
	// Sensor Gain設定
	//------------------------------------------------------------
	if ((status = sensorSetGainX (DEFAULT_SENSOR_GAINX)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//------------------------------------------------------------
	// H Interval取得
	//------------------------------------------------------------
#if !defined (MODE_SENSOR_MASTER)

	if (shutterMode == 1)
	{
		// Shutter ModeがTrigger Modeの場合は、bit/IFの回線速度に合わせたH intervalを取得
		if ((status = sensorHIntervalData (bit, &data32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// Shutter ModeがNormal Modeの場合は、フレームレートからH interval計算して取得
		if ((status = sensorHIntervalCalc (DEFAULT_FRAMERATE, &data32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

#else // #if !defined (MODE_SENSOR_MASTER)

	// H Interval取得
	if ((status = sensorHIntervalCalc (DEFAULT_FRAMERATE, &data32)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#endif // #if !defined (MODE_SENSOR_MASTER)

	//------------------------------------------------------------
	// H Interval設定(はじめてのHinterval設定)
	//------------------------------------------------------------
	if ((status = sensorSetHIntervalReg (data32)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// FPGA Shutter Normal Mode
	//------------------------------------------------------------
	if (shutterMode == SHUTTER_MODE_NORMAL)
	{
		if ((status = sensorSetFpgaShutterMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//------------------------------------------------------------
	// Sensor Standby解除
	//------------------------------------------------------------
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// TGES/TGSE/TGPD初期化(デフォルト値)
	//------------------------------------------------------------

	// TGSE設定
	if ((status = tgSetTgse (SENSOR_TGSE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// TGES設定
	if ((status = tgSetTges (SENSOR_TGES)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// TGPD設定
	if ((status = tgSetTgpd (SENSOR_TGPD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// TEMP OK初期化
	OUT32 (FPGA_PELTIER_TEC_POK_ADRS, 1);

_DONE:
	return (status);
}


//**********************************************************************************
//	センサ初期化2
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorVendorSInitialize2 (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;
	int flip;
	int mode;
	unsigned int hInterval;
	double frameRateD;
	int shutterMode;
#if defined (MODE_SENSOR_DRRS)
	int drrsMode;
#endif

	//------------------------------------------------------------
	// Sensor Standby
	//------------------------------------------------------------
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Gradiation Compress
	//------------------------------------------------------------
#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	if ((status = sensorGradationCompInitialize ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//------------------------------------------------------------
	// Bit設定
	//------------------------------------------------------------
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// AD初期化設定
	//------------------------------------------------------------
	if ((status = sensorSetAD (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// OB初期化設定
	//------------------------------------------------------------
	if ((status = sensorSetOB (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Black Level設定
	//------------------------------------------------------------
	if ((status = sensorSetBL (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Flip設定
	//------------------------------------------------------------
#if defined (MODE_SENSOR_XFLIP)
	if ((status = aoiGetXflip (&flip)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	#if !defined (MODE_SENSOR_DRRS)
	if ((status = sensorSetXFlip (flip)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif
	
	if (flip == MODE_ENABLE)
	{
		// Optical Black反転
		if ((status = opticalBlackSetInvert (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// Optical Black正転
		if ((status = opticalBlackSetInvert (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif

	//------------------------------------------------------------
	// 隠しレジスタ初期化
	//------------------------------------------------------------
	if ((status = sensorBitSetHide (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// H Iterval設定(bitが変わるかもしれないので、再設定)
	//------------------------------------------------------------
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (shutterMode == SHUTTER_MODE_TRIGGER)
	{
		// Shutter ModeがTrigger Modeの場合は、bit/IFの回線速度に合わせたH intervalを取得
		if ((status = sensorHIntervalData (bit, &hInterval)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// Shutter ModeがNormal Modeの場合は、フレームレートからH interval計算して取得

		// フレームレート取得
		if ((status = acquisitionGetFrameRate (&frameRateD)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// H Interval取得
		if ((status = sensorHIntervalCalc (frameRateD, &hInterval)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// H Interval設定
	if ((status = sensorSetHIntervalReg (hInterval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Register Data Restore
	//--------------------------------------------------------------------------------
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SENSOR_ADRS, CAMERA_SENSOR_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gSensorStatus = status;
		goto _DONE;
	}

	//--------------------------------------------------------------------------------
	// DRRS初期化
	//--------------------------------------------------------------------------------
#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorDrrsInitialize ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//--------------------------------------------------------------------------------
	// Conversion Gain取得(Register Data Restoreでパラメータが反映される)
	//--------------------------------------------------------------------------------
	if ((status = sensorGetConversionGain (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Conversion Gain設定
	//--------------------------------------------------------------------------------
	if ((status = sensorSetConversionGain (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Set Black Level(FPGAのレジスタ)
	//--------------------------------------------------------------------------------
#if defined (MODE_SENSOR_DRRS)
	// DRRSモード取得
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (drrsMode == MODE_ENABLE)
	{
		if ((status = sensorDrrsSetBlacklevel ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if defined (MODE_SENSOR_DRRS)

_DONE:
	//------------------------------------------------------------
	// Sensor Standby解除
	//------------------------------------------------------------
	sensorStandByCancel ();

	return (status);
}


//**********************************************************************************
//	Sensor Hide Register Setting
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorVendorSHideRegSetting (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char adrs;
	unsigned char data8;
	int regHold, acTmg;
	unsigned char id;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//============================================================
	//============================================================
	// 隠しレジスタ
	//============================================================
	//============================================================

	//============================================================
	// Chip ID = 0x02
	//============================================================
	id = 0x02;

	//------------------------------------------------------------
	// Adrs = 0xf7
	//------------------------------------------------------------
	adrs = 0xf7;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x04
	//============================================================
	id = 0x04;

	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	adrs = 0x44;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x48
	//------------------------------------------------------------
	adrs = 0x48;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4c
	//------------------------------------------------------------
	adrs = 0x4c;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0xa5;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x63
	//------------------------------------------------------------
	adrs = 0x63;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd0
	//------------------------------------------------------------
	adrs = 0xd0;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd1
	//------------------------------------------------------------
	adrs = 0xd1;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x05
	//============================================================
	id = 0x05;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x81
	//------------------------------------------------------------
	adrs = 0x81;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x06
	//============================================================
	id = 0x06;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	if ((status = sensorRegReadByte (id, adrs, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 |= 0x20;

	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x07
	//============================================================
	id = 0x07;

	//------------------------------------------------------------
	// Adrs = 0x42
	//------------------------------------------------------------
	adrs = 0x42;
	data8 = 0x05;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4b
	//------------------------------------------------------------
	adrs = 0x4b;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x98
	//------------------------------------------------------------
	adrs = 0x98;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xC8
	//------------------------------------------------------------
	//sensorBitSetHide()で設定

	//------------------------------------------------------------
	// Adrs = 0xe0
	//------------------------------------------------------------
	adrs = 0xe0;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xec
	//------------------------------------------------------------
	adrs = 0xec;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x08
	//============================================================
	id = 0x08;

	//------------------------------------------------------------
	// Adrs = 0x22(Add Sensor Revision 0.8)
	//------------------------------------------------------------
	adrs = 0x22;
	data8 = 0x10;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5a(Add Sensor Revision 0.8)
	//------------------------------------------------------------
	adrs = 0x5a;
	data8 = 0x16;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x92(Add Sensor Revision 0.8)
	//------------------------------------------------------------
	adrs = 0x92;
	data8 = 0x10;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x09
	//============================================================
	id = 0x09;

	//============================================================
	// Chip ID = 0x0a
	//============================================================
	id = 0x0a;

	//============================================================
	// Chip ID = 0x0b
	//============================================================
	id = 0x0b;

	//============================================================
	// Chip ID = 0x0c
	//============================================================
	id = 0x0c;

	//------------------------------------------------------------
	// Adrs = 0x6a
	//------------------------------------------------------------
	adrs = 0x6a;
	data8 = 0xa5;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6b
	//------------------------------------------------------------
	adrs = 0x6b;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe8
	//------------------------------------------------------------
	adrs = 0xe8;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe9
	//------------------------------------------------------------
	adrs = 0xe9;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x0d
	//============================================================
	id = 0x0d;

	//------------------------------------------------------------
	// Adrs = 0x8c
	//------------------------------------------------------------
	adrs = 0x8c;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8d
	//------------------------------------------------------------
	adrs = 0x8d;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x10
	//============================================================
	id = 0x10;

	//------------------------------------------------------------
	// Adrs = 0x60
	//------------------------------------------------------------
	adrs = 0x60;
	data8 = 0x1d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0xd1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf0
	//------------------------------------------------------------
	adrs = 0xf0;
	data8 = 0x18;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf2
	//------------------------------------------------------------
	adrs = 0xf2;
	data8 = 0xd6;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x11
	//============================================================
	id = 0x11;

	//------------------------------------------------------------
	// Adrs = 0x14
	//------------------------------------------------------------
	adrs = 0x14;
	data8 = 0xd7;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x74
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0x88;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0xd7;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x12
	//============================================================
	id = 0x12;

	//------------------------------------------------------------
	// Adrs = 0x18
	//------------------------------------------------------------
	adrs = 0x18;
	data8 = 0x83;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1a
	//------------------------------------------------------------
	adrs = 0x1a;
	data8 = 0x14;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2a
	//------------------------------------------------------------
	adrs = 0x2a;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2b
	//------------------------------------------------------------
	adrs = 0x2b;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2c
	//------------------------------------------------------------
	adrs = 0x2c;
	data8 = 0xe4; // Add Sensor Revision 0.8
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2d
	//------------------------------------------------------------
	adrs = 0x2d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2e
	//------------------------------------------------------------
	adrs = 0x2e;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2f
	//------------------------------------------------------------
	adrs = 0x2f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x38
	//------------------------------------------------------------
	adrs = 0x38;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3a
	//------------------------------------------------------------
	adrs = 0x3a;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3b
	//------------------------------------------------------------
	adrs = 0x3b;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6a
	//------------------------------------------------------------
	adrs = 0x6a;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6b
	//------------------------------------------------------------
	adrs = 0x6b;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6c
	//------------------------------------------------------------
	adrs = 0x6c;
	data8 = 0xe4; // Add Sensor Revision 0.8
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6d
	//------------------------------------------------------------
	adrs = 0x6d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x25;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7e
	//------------------------------------------------------------
	adrs = 0x7e;
	data8 = 0x6e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x88
	//------------------------------------------------------------
	adrs = 0x88;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8a
	//------------------------------------------------------------
	adrs = 0x8a;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8b
	//------------------------------------------------------------
	adrs = 0x8b;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc8
	//------------------------------------------------------------
	adrs = 0xc8;
	data8 = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xca
	//------------------------------------------------------------
	adrs = 0xca;
	data8 = 0x73;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x13
	//============================================================
	id = 0x13;

	//------------------------------------------------------------
	// Adrs = 0xd0
	//------------------------------------------------------------
	adrs = 0xd0;
	data8 = 0x61; // Add Sensor Revision 0.8
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd5
	//------------------------------------------------------------
	adrs = 0xd5;
	data8 = 0x11;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x14
	//============================================================
	id = 0x14;

	//============================================================
	// Chip ID = 0x15
	//============================================================
	id = 0x15;

	//============================================================
	// Chip ID = 0x16
	//============================================================
	id = 0x16;

	//============================================================
	// Chip ID = 0x17
	//============================================================
	id = 0x17;

	//============================================================
	// Chip ID = 0x18
	//============================================================
	id = 0x18;

	//============================================================
	// Chip ID = 0x19
	//============================================================
	id = 0x19;

	//------------------------------------------------------------
	// Adrs = 0x0b
	//------------------------------------------------------------
	adrs = 0x0b;
	data8 = 0x1f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0x07;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x85
	//------------------------------------------------------------
	adrs = 0x85;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x86
	//------------------------------------------------------------
	adrs = 0x86;
	data8 = 0x0d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x87
	//------------------------------------------------------------
	adrs = 0x87;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8c
	//------------------------------------------------------------
	adrs = 0x8c;
	data8 = 0x71;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8d
	//------------------------------------------------------------
	adrs = 0x8d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8e
	//------------------------------------------------------------
	adrs = 0x8e;
	data8 = 0x75;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8f
	//------------------------------------------------------------
	adrs = 0x8f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x94
	//------------------------------------------------------------
	adrs = 0x94;
	data8 = 0x06;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x95
	//------------------------------------------------------------
	adrs = 0x95;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x96
	//------------------------------------------------------------
	adrs = 0x96;
	data8 = 0x0e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x97
	//------------------------------------------------------------
	adrs = 0x97;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9c
	//------------------------------------------------------------
	adrs = 0x9c;
	data8 = 0x70;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9d
	//------------------------------------------------------------
	adrs = 0x9d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9e
	//------------------------------------------------------------
	adrs = 0x9e;
	data8 = 0x76;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9f
	//------------------------------------------------------------
	adrs = 0x9f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x1a
	//============================================================
	id = 0x1a;

	//============================================================
	// Chip ID = 0x1b
	//============================================================
	id = 0x1b;

	//============================================================
	// Chip ID = 0x1c
	//============================================================
	id = 0x1c;

	//------------------------------------------------------------
	// Adrs = 0x1c
	//------------------------------------------------------------
	adrs = 0x1c;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1d
	//------------------------------------------------------------
	adrs = 0x1d;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1e
	//------------------------------------------------------------
	adrs = 0x1e;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1f
	//------------------------------------------------------------
	adrs = 0x1f;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x24
	//------------------------------------------------------------
	adrs = 0x24;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7c
	//------------------------------------------------------------
	adrs = 0x7c;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7d
	//------------------------------------------------------------
	adrs = 0x7d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7e
	//------------------------------------------------------------
	adrs = 0x7e;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x81
	//------------------------------------------------------------
	adrs = 0x81;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x82
	//------------------------------------------------------------
	adrs = 0x82;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8e
	//------------------------------------------------------------
	adrs = 0x8e;
	data8 = 0xe4; // Add Sensor Revision 0.8
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8f
	//------------------------------------------------------------
	adrs = 0x8f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x92
	//------------------------------------------------------------
	adrs = 0x92;
	data8 = 0xe4; // Add Sensor Revision 0.8
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x93
	//------------------------------------------------------------
	adrs = 0x93;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd1
	//------------------------------------------------------------
	adrs = 0xd1;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// Bit設定(隠しレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bit					：bit
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorBitSetHide (int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char id;
	unsigned char data8;
	int regHold, acTmg;
	unsigned int adrs;
	int mode;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//============================================================
	// ID
	//============================================================
	id = 0x07;

	//------------------------------------------------------------
	// Adrs = 0xc8
	// 設定値=12bit=0x6f/10bit=0x7d/8bit=0x69
	// 8bitは圧縮階調を使用するので10bitと同じ設定
	//------------------------------------------------------------
	adrs = 0xc8;

	if (bit == 12)
		data8 = 0x6f;
	else if (bit == 10)
		data8 = 0x7d;
	else
	{
		#if defined (MODE_SENSOR_GRADATION_COMPRESS)
		mode = sensorGradationCompGetModeDDR ();

		if (mode == MODE_DISABLE)
			data8 = 0x7d;
		else
			data8 = 0x69;
		#else // #if defined (MODE_SENSOR_GRADATION_COMPRESS)
		data8 = 0x7d;
		#endif // #if defined (MODE_SENSOR_GRADATION_COMPRESS)
	}

	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	StandByモード移行
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorStandBy (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char id;
	int acTmg, regHold;
	unsigned char data8;

	//------------------------------------------------------------
	// 画像入力停止
	//------------------------------------------------------------
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Sensor Output Disable
	//------------------------------------------------------------
	OUT32 (FPGA_SENSOR_OUTPUT_CTRL_ADRS, 0);

	//------------------------------------------------------------
	// Sensor Temp Auto Refresh Disable
	//------------------------------------------------------------
	OUT32 (FPGA_SENSOR_TEMP_REFRESH_ADRS, 0);

	//------------------------------------------------------------
	// Set Sensor Input Ctrl Disable
	//------------------------------------------------------------
	OUT32 (FPGA_SENSOR_INPUT_CTRL_ADRS, 0);

	//------------------------------------------------------------
	// StandBy
	//------------------------------------------------------------
	id = 0x02;
	acTmg = 1;
	regHold = 0;

	data8 = SENSOR_REG_STANDBY_BIT;	// Standbyモード
	if ((status = sensorRegWriteByte (id, SENSOR_REG_STANDBY_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	usDelay(200);		// 仕様上は150us Delay


#if !defined (MODE_SENSOR_MASTER)
	//------------------------------------------------------------
	// Disable Trg Control
	//------------------------------------------------------------
	if ((status = sensorSetTrgControl (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Disable HV Gen
	//------------------------------------------------------------
	if ((status = sensorSetHvGen (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif // #if defined (MODE_SENSOR_MASTER)


	//------------------------------------------------------------
	// Master Mode
	//------------------------------------------------------------
#if defined (MODE_SENSOR_MASTER)
	id = 2;
	regHold = 0;
	acTmg = 0;

	data8 = 1;	// 動作停止
	if ((status = sensorRegWriteByte (id, SENSOR_REG_XMSTA_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	StandByモード解除
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorStandByCancel (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char id;
	int acTmg, regHold;
	unsigned char data8;
	unsigned int hIntervalClock;
	unsigned int expTime;
	unsigned int saveExpTime = 0xffffffff;
	unsigned int frameRateTime;
	unsigned int saveFrameRateTime = 0xffffffff;
	int saveAcMode = -1;
	double hTimeD;
	int trgModeAcquisitionStart = 0;
	int trgModeAcquisitionStop = 0;
	int trgModeAcquisitionActive = 0;
	int trgModeFrameActive = 0;
	int trgSelector = -1;
	unsigned int saveFrameCount = 1;
	unsigned int saveSensorSHS = 0xffffffff;
	unsigned int saveHInterval = 0xffffffff;
	int expMode = -1;
	int shutterMode;
	int drrsMode = MODE_DISABLE;
	double frameRate;
	unsigned int hInterval;
	unsigned int i, data32;
	int trgMode, hvGenMode;

	//------------------------------------------------------------
	// DRRSモード取得
	//------------------------------------------------------------
	#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	//------------------------------------------------------------
	// StandBy
	//------------------------------------------------------------
	id = 0x02;
	acTmg = 0;
	regHold = 0;

	data8 = 0;	// Standbyモード解除
	if ((status = sensorRegWriteByte (id, SENSOR_REG_STANDBY_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 仕様上は29ms
	msDelay (30);

	//------------------------------------------------------------
	// H Interval
	//------------------------------------------------------------
	if (drrsMode == MODE_DISABLE)
	{
		// H Interval保存
		if ((status = sensorGetHIntervalReg (&saveHInterval)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// H Interval設定
		if (saveHInterval != HINTERVAL_CLOCK_12BIT_SINGLE_10_00G)
		{
			if ((status = sensorSetHIntervalReg (HINTERVAL_CLOCK_12BIT_SINGLE_10_00G)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}
	else	// DRRS Enable
	{
		//------------------------------------------------------------
		// フレームレート設定=>DRRS時はフレームレートが決まれば露光時間も決まる
		//------------------------------------------------------------
		frameRate = 60;
	
		// H Interval取得
		if ((status = sensorHIntervalCalc (frameRate, &hInterval)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// H Interval保存
		if ((status = sensorGetHIntervalReg (&saveHInterval)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// H Interval設定
		if ((status = sensorSetHIntervalReg (hInterval)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//------------------------------------------------------------
	// Set Sensor Input Ctrl Enable
	//------------------------------------------------------------
	OUT32 (FPGA_SENSOR_INPUT_CTRL_ADRS, FPGA_SENSOR_INPUT_CTRL_ENABLE_BIT);

	//------------------------------------------------------------
	// Check Sensor Input Ctrl Status
	//------------------------------------------------------------
	for (i=0; i<SENSOR_INPUT_CTRL_STATUS_TIMEOUT; i++)
	{
		data32 = IN32 (FPGA_SENSOR_INPUT_STATUS_ADRS);

		if ((data32 & FPGA_SENSOR_INPUT_STATUS_RESET_END_BIT) == 0)
			break;
		
		usDelay (2);
	}
	
	// Check Timeout
	if (i >= SENSOR_INPUT_CTRL_STATUS_TIMEOUT)
	{
		DEBUG_PRINT ("Sensor Input Ctrl Timeout Error\n");
		//status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_TIMEOUT);
		//goto _DONE;
	}

	//------------------------------------------------------------
	// Get Shutter Mode
	//------------------------------------------------------------
	if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//@@@1#if defined (MODE_SENSOR_MASTER)
	//------------------------------------------------------------
	// FPGA Master Mode
	//------------------------------------------------------------
	//@@@1if ((status = sensorSetMasterMode (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		//@@@1goto _DONE;
	//@@@1#endif

	//------------------------------------------------------------
	// Master Mode
	//------------------------------------------------------------
	#if defined (MODE_SENSOR_MASTER)
	id = 2;
	regHold = 0;
	acTmg = 0;
	data8 = 0;	// 動作開始
	if ((status = sensorRegWriteByte (id, SENSOR_REG_XMSTA_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif


	//------------------------------------------------------------
	// Sensor Temp Auto Refresh Enable
	//------------------------------------------------------------
	OUT32 (FPGA_SENSOR_TEMP_REFRESH_ADRS, FPGA_SENSOR_TEMP_REFRESH_ENABLE_BIT);

#if !defined (MODE_SENSOR_MASTER)
	//------------------------------------------------------------
	// Enable HV Gen
	//------------------------------------------------------------
	if ((status = sensorSetHvGen (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Enable Trg Control
	//------------------------------------------------------------
	if ((status = sensorSetTrgControl (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif // #if defined (MODE_SENSOR_MASTER)

	//------------------------------------------------------------
	// 撮像モード設定
	//------------------------------------------------------------
	if ((status = acquisitionGetMode (&saveAcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = acquisitionSetMode (ACQUISITION_MODE_MULTI)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// フレームカウント設定
	//------------------------------------------------------------
	if ((status = acquisitionGetFrameCount (&saveFrameCount)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = acquisitionSetFrameCount (8)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// H時間計算
	//------------------------------------------------------------
	// H Interval
	if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// H時間
	hTimeD = (double)hIntervalClock * SENSOR_H_TIME;

	//------------------------------------------------------------
	// 露光時間設定
	//------------------------------------------------------------
	if (drrsMode == MODE_DISABLE)
	{
		expTime = 1000;			// 1ms
		expTime /= hTimeD;
		if (expTime == 0)
			expTime = 2;		// 2h

		if (expTime == 1)
			expTime++;
		else if (expTime & 0x01)
			expTime--;

		if (shutterMode == SHUTTER_MODE_NORMAL)
		{
#if defined (MODE_SENSOR_SHUTTER)
			// Get Sensor SHS
			if ((status = sensorGetSHS_H (&saveSensorSHS)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Set Sensor SHS
			if ((status = sensorSetSHS_H (expTime)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
#endif
		}
		else
		{
			saveExpTime = IN32 (GENICAM_ACQUISITION_EXPOSURE_ADRS) & GENICAM_ACQUISITION_EXPOSURE_MASK;
			OUT32 (GENICAM_ACQUISITION_EXPOSURE_ADRS, (expTime & GENICAM_ACQUISITION_EXPOSURE_MASK));
		}

		//------------------------------------------------------------
		// フレームレート設定
		//------------------------------------------------------------
		frameRateTime = 16666;		// 60fps(16666us)
		frameRateTime /= hTimeD;	// 1フレームのH単位で割る
		if (frameRateTime == 0)
			frameRateTime = 0x4000;
		
		if (frameRateTime == 0)
			frameRateTime = 2;
		else if (frameRateTime & 0x01)
			frameRateTime++;

		saveFrameRateTime = IN32 (GENICAM_ACQUISITION_FRAMERATE_ADRS) & GENICAM_ACQUISITION_FRAMERATE_MASK;
		OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, (frameRateTime & GENICAM_ACQUISITION_FRAMERATE_MASK));
	}

	//------------------------------------------------------------
	// トリガセレクタ取得
	//------------------------------------------------------------
	if ((status = acquisitionGetSelect (&trgSelector)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// AcquisitionStartトリガ無効
	//------------------------------------------------------------
	// セレクタ設定
	if ((status = acquisitionSetSelect (ACQUISITION_TRG_SELECT_ACQUISITION_START)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガ有効/無効状態取得
	if ((status = acquisitionGetTrgMode (&trgModeAcquisitionStart)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガ無効状態設定
	if ((status = acquisitionSetTrgMode (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// AcquisitionStopトリガ無効
	//------------------------------------------------------------
	// セレクタ設定
	if ((status = acquisitionSetSelect (ACQUISITION_TRG_SELECT_ACQUISITION_END)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// AcquisitionStop有効/無効状態取得
	if ((status = acquisitionGetTrgMode (&trgModeAcquisitionStop)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガ無効状態設定
	if ((status = acquisitionSetTrgMode (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// AcquisitionActiveトリガ無効
	//------------------------------------------------------------
	// セレクタ設定
	if ((status = acquisitionSetSelect (ACQUISITION_TRG_SELECT_ACQUISITION_ACTIVE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガ有効/無効状態取得
	if ((status = acquisitionGetTrgMode (&trgModeAcquisitionActive)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガ無効状態設定
	if ((status = acquisitionSetTrgMode (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// ExposureActiveトリガ無効
	//------------------------------------------------------------
	// セレクタ設定
	if ((status = acquisitionSetSelect (ACQUISITION_TRG_SELECT_FRAME_ACTIVE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガ有効/無効状態取得
	if ((status = acquisitionGetTrgMode (&trgModeFrameActive)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// トリガ無効状態設定
	if ((status = acquisitionSetTrgMode (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// ExposureMode無効
	//------------------------------------------------------------

	// ExposureMode取得
	if ((status = acquisitionGetExposureMode (&expMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (expMode == 1)
	{
		// ExposureMode無効設定
		if ((status = acquisitionSetExposureMode (0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		expMode = -1;
	}

	//------------------------------------------------------------
	// 画像入力開始
	//------------------------------------------------------------
	if ((status = acquisitionStart ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// 16.666msx8Frameなので133.333ms
	msDelay (150);

	//------------------------------------------------------------
	// 画像入力停止
	//------------------------------------------------------------
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	usDelay(200);

	// H Intervalリストア
	if (saveHInterval != 0xffffffff)
	{
		if (saveHInterval != HINTERVAL_CLOCK_12BIT_SINGLE_10_00G)
		{
			// Sensor Trg Disable
			sensorTrgDisable (&trgMode, &hvGenMode);
			
			// H Interval設定
			sensorSetHIntervalReg (saveHInterval);

			// Sensor Trg Enable
			sensorTrgEnable (trgMode, hvGenMode);
		}
	}

	//------------------------------------------------------------
	// Sensor Output Enable
	//------------------------------------------------------------
	OUT32 (FPGA_SENSOR_OUTPUT_CTRL_ADRS, FPGA_SENSOR_OUTPUT_CTRL_ENABLE_BIT);

	// AcquisitionMode設定
	if (saveAcMode != -1)
		acquisitionSetMode (saveAcMode);

	// FrameCount設定
	//if (saveFrameCount != 0xffffffff)
		acquisitionSetFrameCount (saveFrameCount);

	// 露光時間設定
	if (saveExpTime != 0xffffffff)
	{
		if (shutterMode != SHUTTER_MODE_NORMAL)
			OUT32 (GENICAM_ACQUISITION_EXPOSURE_ADRS, (saveExpTime & GENICAM_ACQUISITION_EXPOSURE_MASK));
	}

#if defined (MODE_SENSOR_SHUTTER)
	// Set Sensor SHS
	if (saveSensorSHS != 0xffffffff)
		sensorSetSHS_H (saveSensorSHS);
#endif

	// フレームレート設定
	if (saveFrameRateTime != 0xffffffff)
		OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, (saveFrameRateTime & GENICAM_ACQUISITION_FRAMERATE_MASK));

	// AcquisitionStartトリガ設定
	if (trgModeAcquisitionStart != 0)
	{
		acquisitionSetSelect (ACQUISITION_TRG_SELECT_ACQUISITION_START);
		acquisitionSetTrgMode (trgModeAcquisitionStart);
	}

	// AcquisitionStopトリガ設定
	if (trgModeAcquisitionStop != 0)
	{
		acquisitionSetSelect (ACQUISITION_TRG_SELECT_ACQUISITION_END);
		acquisitionSetTrgMode (trgModeAcquisitionStop);
	}

	// AcquisitionActiveトリガ設定
	if (trgModeAcquisitionActive != 0)
	{
		acquisitionSetSelect (ACQUISITION_TRG_SELECT_ACQUISITION_ACTIVE);
		acquisitionSetTrgMode (trgModeAcquisitionActive);
	}

	// FrameActiveトリガ設定
	if (trgModeFrameActive != 0)
	{
		acquisitionSetSelect (ACQUISITION_TRG_SELECT_FRAME_ACTIVE);
		acquisitionSetTrgMode (trgModeFrameActive);
	}

	// ExposureMode設定
	if (expMode != -1)
	{
		acquisitionSetExposureMode (expMode);
	}

	// トリガセレクタ設定
	if (trgSelector != -1)
		acquisitionSetSelect (trgSelector);
		
	return (status);
}


//**********************************************************************************
//	H Interval Data取得設定(通常版)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bit					：bit値
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorHIntervalData (int bit, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	unsigned int cxpConnectionMode, rate;
	unsigned int hInterval;
	int gcMode;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "H Interval Get NULL Parameter Error.\n");
		goto _DONE;
	}

#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	if (bit == 8)
	{
		// Gradation Compress Mode取得
		gcMode = IN32 (FIRM_DATA_GRADATION_COMPRESS_MODE_PRE);

		// Gradation Compress Mode有効?
		if (gcMode == MODE_ENABLE)
			bit = GC_CAMERA_BIT;
	}
#endif

	// CXP
	if (gInterFaceID == INTERFACE_CXP)
	{
#if defined (IF_CXP)
		// Get Rate
		if ((status = cxpGetRateData (&data32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get CXP Rate
		rate = data32 & 0xffff;

		// Get CXP Single or Dual
		cxpConnectionMode = (data32 >> 16);

		// Single
		if (rate == CXP_RATE_1_25G)
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_1_25G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_1_25G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_1_25G;
		}
		else if (rate == CXP_RATE_2_50G)
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_2_50G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_2_50G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_2_50G;
		}
		else if (rate == CXP_RATE_3_125G)
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_3_125G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_3_125G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_3_125G;
		}
		else if (rate == CXP_RATE_5_00G)
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_5_00G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_5_00G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_5_00G;
		}
		else if (rate == CXP_RATE_6_25G)
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_6_25G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_6_25G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_6_25G;
		}
		else if (rate == CXP_RATE_10_00G)
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_10_00G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_10_00G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_10_00G;
		}
		else if (rate == CXP_RATE_12_50G)
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_12_50G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_12_50G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_12_50G;
		}
		else
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "CXP Rate(0x%x) Parameter Error.\n", rate);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
#endif // #if defined (IF_CXP)
	}
	// GigE
	else if (gInterFaceID == INTERFACE_GIGE)
	{
#if defined (IF_GIGE)
		// Get Speed
		if ((status = gigeGetSpeed ((int *)&data32)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	
		if ((data32 == GIGE_SPEED_1GIG_HD) || (data32 == GIGE_SPEED_1GIG_FD))
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_1_25G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_1_25G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_1_25G;
		}
		else if (data32 == GIGE_SPEED_2P5GIG_FD)
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_2_50G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_2_50G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_2_50G;
		}
		else if (data32 == GIGE_SPEED_5GIG_FD)
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_5_00G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_5_00G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_5_00G;
		}
		else if (data32 == GIGE_SPEED_10GIG_FD)
		{
			if (bit == 8)
				hInterval = HINTERVAL_CLOCK_8BIT_SINGLE_10_00G;
			else if (bit == 10)
				hInterval = HINTERVAL_CLOCK_10BIT_SINGLE_10_00G;
			else
				hInterval = HINTERVAL_CLOCK_12BIT_SINGLE_10_00G;
		}
		else
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, CXP "Rate(0x%x) Parameter Error.\n", data32);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
#endif //#if defined (IF_GIGE)
	}

	// データ格納
	*pData = hInterval;

_DONE:
	return (status);
}


#if defined (MODE_SENSOR_DRRS)
//**********************************************************************************
//	H Interval Data取得設定(DRRSモード時)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bit					：bit値
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorHIntervalDataDrrs (int bit, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int hInterval, hIntervalCxp;
	int gcMode;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "H Interval Get NULL Parameter Error.\n");
		goto _DONE;
	}

#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	if (bit == 8)
	{
		// Gradation Compress Mode取得
		gcMode = IN32 (FIRM_DATA_GRADATION_COMPRESS_MODE_PRE);

		// Gradation Compress Mode有効?
		if (gcMode == MODE_ENABLE)
			bit = GC_CAMERA_BIT;
	}
#endif

	// CXP
	if (gInterFaceID == INTERFACE_CXP)
	{
		if (bit == 8)
			hInterval = HINTERVAL_CLOCK_8BIT_DRRS;
		else if (bit == 10)
			hInterval = HINTERVAL_CLOCK_10BIT_DRRS;
		else
			hInterval = HINTERVAL_CLOCK_12BIT_DRRS;
		
		// CXPの制限によるHIntervalを取得
		if ((status = sensorHIntervalData (bit, &hIntervalCxp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// CXPの制限以下ならば、CXP側に合わせる
		// GEはフレームバッファがあるので、このような制限は必要なし
		if (hInterval < hIntervalCxp)
			hInterval = hIntervalCxp + 1/*マージン*/;
	}
	// GigE
	else if (gInterFaceID == INTERFACE_GIGE)
	{
		if (bit == 8)
			hInterval = HINTERVAL_CLOCK_8BIT_DRRS;
		else if (bit == 10)
			hInterval = HINTERVAL_CLOCK_10BIT_DRRS;
		else
			hInterval = HINTERVAL_CLOCK_12BIT_DRRS;
	}

	// データ格納
	*pData = hInterval;

_DONE:

	return (status);
}
#endif // #if defined (MODE_SENSOR_DRRS)


//**********************************************************************************
//	H Interval Data取得設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		frameRate			：フレームレート
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorHIntervalCalc (double frameRate, unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int hInterval;
	int drrsMode = MODE_DISABLE;
	int totalHeight;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "H Interval Get NULL Parameter Error.\n");
		goto _DONE;
	}

	#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	// H Interval取得
	if (drrsMode == MODE_ENABLE)
	{
		// DRRS時の注意事項
		// DRRS時は通常画像と黒画像２枚出力される
		// この為、センサに与えるH Intervalはフレームレートの倍の設定とする
		
		// HeightのTotal Sizeを求める
		if ((status = fpgaRoiGetSensorHeightTotalSize (&totalHeight)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		totalHeight += SENSOR_DRRS_FIX_LINE;

		//hInterval = (unsigned int)(((SENSOR_H_CLOCK * 1000 * 1000) / (double)SENSOR_DRRS_VMAX) / frameRate);
		hInterval = (unsigned int)(((SENSOR_H_CLOCK * 1000 * 1000) / (double)totalHeight) / frameRate);
	}
	else
	{
		hInterval = (unsigned int)(((SENSOR_H_CLOCK * 1000 * 1000) / (double)SENSOR_VINTERVAL_LINE) / frameRate);
	}
	
	// データ格納
	*pData = hInterval;

_DONE:
	return (status);
}


//**********************************************************************************
//	H Interval設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data				：H Interval
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetHIntervalReg (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;

	// H Interval
	OUT32 (FPGA_SENSOR_HINTERVAL_ADRS, (data & FPGA_SENSOR_HINTERVAL_MASK));

	return (status);
}


//**********************************************************************************
//	H Interval取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：H Intervalを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetHIntervalReg (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "H Interval Get NULL Parameter Error.\n");
		goto _DONE;
	}

	// H Interval
	*pData = IN32 (FPGA_SENSOR_HINTERVAL_ADRS) & FPGA_SENSOR_HINTERVAL_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
//	H Interval設定(トリガモード)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bit					：bit値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetHInterval (int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int hInterval;
	int hvGenMode = MODE_DISABLE;
	int trgMode = MODE_DISABLE;

#if !defined (MODE_SENSOR_MASTER)
	// Get Trg Mode
	if ((status = sensorGetTrgControl (&trgMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get HV Gen Mode
	if ((status = sensorGetHvGen (&hvGenMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Disable Trg Control
	if ((status = sensorSetTrgControl (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Disable HV Gen
	if ((status = sensorSetHvGen (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif // #if !defined (MODE_SENSOR_MASTER)

	// H Interval取得
	if ((status = sensorHIntervalData (bit, &hInterval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// H Interval設定
	if ((status = sensorSetHIntervalReg (hInterval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if !defined (MODE_SENSOR_MASTER)
	// Enable HV Gen
	if (hvGenMode == MODE_ENABLE)
	{
		if ((status = sensorSetHvGen (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Enable Trg Control
	if (trgMode == MODE_ENABLE)
	{
		if ((status = sensorSetTrgControl (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if !defined (MODE_SENSOR_MASTER)

_DONE:
	return (status);
}


//**********************************************************************************
//	H Interval設定(トリガノーマルモード)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		frameRate			：フレームレート
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetHIntervalTrgNormal (double frameRate)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int hInterval;
	int hvGenMode = MODE_DISABLE;
	int trgMode = MODE_DISABLE;

#if !defined (MODE_SENSOR_MASTER)
	// Get Trg Mode
	if ((status = sensorGetTrgControl (&trgMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get HV Gen Mode
	if ((status = sensorGetHvGen (&hvGenMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Disable Trg Control
	if ((status = sensorSetTrgControl (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Disable HV Gen
	if ((status = sensorSetHvGen (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif // #if !defined (MODE_SENSOR_MASTER)

	// H Interval取得
	if ((status = sensorHIntervalCalc (frameRate, &hInterval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// H Interval設定
	if ((status = sensorSetHIntervalReg (hInterval)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if !defined (MODE_SENSOR_MASTER)
	// Enable HV Gen
	if (hvGenMode == MODE_ENABLE)
	{
		if ((status = sensorSetHvGen (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Enable Trg Control
	if (trgMode == MODE_ENABLE)
	{
		if ((status = sensorSetTrgControl (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if !defined (MODE_SENSOR_MASTER)

_DONE:
	return (status);
}


//**********************************************************************************
//	H Interval設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data			：H Interval
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetHIntervalData (unsigned int data)
{
	int status = AVAL_STATUS_SUCCESS;
	int hvGenMode = MODE_DISABLE;
	int trgMode = MODE_DISABLE;

#if !defined (MODE_SENSOR_MASTER)
	// Get Trg Mode
	if ((status = sensorGetTrgControl (&trgMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get HV Gen Mode
	if ((status = sensorGetHvGen (&hvGenMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Disable Trg Control
	if ((status = sensorSetTrgControl (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Disable HV Gen
	if ((status = sensorSetHvGen (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif // #if !defined (MODE_SENSOR_MASTER)

	// H Interval設定
	if ((status = sensorSetHIntervalReg (data)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if !defined (MODE_SENSOR_MASTER)
	// Enable HV Gen
	if (hvGenMode == MODE_ENABLE)
	{
		if ((status = sensorSetHvGen (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Enable Trg Control
	if (trgMode == MODE_ENABLE)
	{
		if ((status = sensorSetTrgControl (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if !defined (MODE_SENSOR_MASTER)

_DONE:
	return (status);
}


//**********************************************************************************
//	H Interval取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pHinterval			：H Intervalを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetHInterval (unsigned int *pHinterval)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pHInterval Parameter
	if (pHinterval == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Get H Interval NULL Parameter Error.\n");
		goto _DONE;
	}

	*pHinterval = IN32 (FPGA_SENSOR_HINTERVAL_ADRS) & FPGA_SENSOR_HINTERVAL_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
// HV Gen設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：MODE＿ENABLE=有効/MODE_DISABLE=無効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetHvGen (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;
	unsigned int data32, setData;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor HV Set Gen Mode(%d) Parameter Error. (Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if (mode == MODE_ENABLE)
	{
		setData = FPGA_SENSOR_HV_GEN_ENABLE_BIT;
	}
	else
	{
		setData = 0;
	}

	// Enable/Disable HV Gen
	OUT32 (FPGA_SENSOR_HV_GEN_ADRS, setData);

	// Check HV Gen
	for (i=0; i<SENSOR_IMAGE_RDY_TIMEOUT; i++)
	{
		data32 = IN32 (FPGA_SENSOR_HV_GEN_ADRS);

		if (mode == MODE_ENABLE)
		{
			if (data32 & FPGA_SENSOR_HV_GEN_DONE_BIT)
				break;
		}
		else
		{
			if ((data32 & FPGA_SENSOR_HV_GEN_DONE_BIT) == 0)
				break;
		}

		usDelay (2);
	}

	// Check Timeout
	if (i >= SENSOR_IMAGE_RDY_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);

		if (mode == MODE_ENABLE)
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor HV Gen Enable Timeout Error.\n");
		else
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor HV Gen Disable Timeout Error.\n");

		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// HV Gen取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：MODE_ENABLE=有効/MODE_DISABLE=無効を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetHvGen (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor HV Gen Get Mode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get HV Gen
	data32 = IN32 (FPGA_SENSOR_HV_GEN_ADRS);

	if (data32 & FPGA_SENSOR_HV_GEN_ENABLE_BIT)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
// Trg Control設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：MODE_ENABLE=有効/MODE_DISABLE=無効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetTrgControl (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int i;
	unsigned int data32, setData;

	// TG UnLock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_UNLOCK);

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Set Trg Control Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if (mode == MODE_ENABLE)
	{
		setData = FPGA_SENSOR_TRG_CTRL_ENABLE_BIT;
	}
	else
	{
		setData = 0;
	}

	// Enable/Disable Trg Control
	OUT32 (FPGA_SENSOR_TRG_CTRL_ADRS, setData);

	// Check Trg Control
	for (i=0; i<SENSOR_IMAGE_RDY_TIMEOUT; i++)
	{
		data32 = IN32 (FPGA_SENSOR_TRG_CTRL_ADRS);

		if (mode == MODE_ENABLE)
		{
			if (data32 & FPGA_SENSOR_TRG_CTRL_ENABLE_DONE_BIT)
				break;
		}
		else
		{
			if ((data32 & FPGA_SENSOR_TRG_CTRL_ENABLE_DONE_BIT) == 0)
				break;
		}

		usDelay (2);
	}

	// Check Timeout
	if (i >= SENSOR_IMAGE_RDY_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		
		if (mode == MODE_ENABLE)
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Trg Control Enable Timeout Error.\n");
		else
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Trg Control Disable Timeout Error.\n");

		goto _DONE;
	}

_DONE:
	// TG Lock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_LOCK);

	return (status);
}


//**********************************************************************************
// Trg Control取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：MODE_ENABLE=有効/MODE_DISABLE=無効を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetTrgControl (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Trg Control Get Mode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Trg Control
	data32 = IN32 (FPGA_SENSOR_TRG_CTRL_ADRS);

	if (data32 & FPGA_SENSOR_TRG_CTRL_ENABLE_BIT)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	センサレジスタ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetRegDefault (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	int reg;
	unsigned char *pAdrs8;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// データ格納領域
	pAdrs8 = (unsigned char *)FIRM_SENSOR_REGISTER_SAVE_ADRS;

	for (id=SENSOR_REG_ID_MIN; id<=SENSOR_REG_ID_MAX; id++)
	{
		for (reg=0; reg<SENSOR_REG_NUM; reg++, pAdrs8++)
		{
			if ((status = sensorRegReadByte (id, (unsigned char)reg, pAdrs8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	センサレジスタ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetRegDefault (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	int reg;
	unsigned char *pAdrs8;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// データ格納領域
	pAdrs8 = (unsigned char *)FIRM_SENSOR_REGISTER_SAVE_ADRS;

	for (id=SENSOR_REG_ID_MIN; id<=SENSOR_REG_ID_MAX; id++)
	{
		for (reg=0; reg<SENSOR_REG_NUM; reg++, pAdrs8++)
		{
			if (id == 0x02)
			{
				if (reg == 0x00)	// Standby
					continue;
			}

			if (id == 0x1c)
			{
				if (reg == 0xD1)	// VR_EN
					continue;

				if (reg == 0xD3)	// VR_RG_DIS
					continue;
			}

			if ((status = sensorRegWriteByte (id, (unsigned char)reg, *pAdrs8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Shutter Normal/Trigger Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：SHUTTER_MODE_NORMAL = Normal / SHUTTER_MODE_TRIGGER = Trigger
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetShutterMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int acTmg, regHold;
	unsigned char data8;
	int id;
	int bit;
	double frameRateD;
	unsigned int expTime;
	unsigned int expTimeMin, expTimeMax;
	unsigned int hIntervalClock;
	unsigned int frameRateTime;
	unsigned int modeSave = 0xffffffff;
	int totalHeight;
	int drrsMode = MODE_DISABLE;

	// Check mode Parameter
	if ((mode != SHUTTER_MODE_NORMAL) && (mode != SHUTTER_MODE_TRIGGER))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Set Shutter Mode(%d) Parameter Error. (Normal:%d / Trigger:%d)\n", mode, SHUTTER_MODE_NORMAL, SHUTTER_MODE_TRIGGER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//============================================================
	// Chip ID = 0x06 / Sensor Shutter Mode
	//============================================================
	id = 0x06;
	acTmg = 0;
	regHold = 0;

	if (mode == SHUTTER_MODE_TRIGGER)
		data8 = SENSOR_REG_TRG_EN_BIT;
	else
		data8 = 0;

	if ((status = sensorRegWriteByte (id, SENSOR_REG_TRG_EN_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// FPGA Shutter Normal Mode
	if ((status = sensorSetFpgaShutterMode (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DRRSモード取得
	#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	#endif

	// フレームレート取得
	frameRateTime = IN32 (FIRM_DATA_FRAME_RATE);
	frameRateD = (1.0 / frameRateTime) * ACQUISITION_FRAMERATE_UINIT;
	
	// 露光時間取得
	expTime = IN32 (FIRM_DATA_EXPOSURE_TIME);

	// モード設定
	modeSave = IN32 (FIRM_DATA_SENSOR_SHUTTER_MODE);
	OUT32 (FIRM_DATA_SENSOR_SHUTTER_MODE, mode);

	// 起動時は以下の初期化のみ実施して終了
	if (IN32 (FIRM_DATA_FIRM_BOOT_FLAG_ADRS) == 0)
	{
		if (mode != SHUTTER_MODE_TRIGGER)
		{
			// TGES設定
			if ((status = tgSetTges2 (SENSOR_TGES_DRRS)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			
			goto _DONE;
		}
	}
	
	if (mode == SHUTTER_MODE_TRIGGER)	// トリガモード
	{
		// bit取得
		if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Shutter ModeがTrigger Modeの場合は、bit/IFの回線速度に合わせたH intervalを取得
		if ((status = sensorSetHInterval (bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// H Interval
		if ((status = irvGetHIntervalClock (&hIntervalClock)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// フレームレート設定
		if ((status = acquisitionSetFrameRate (frameRateD)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		// TGPD設定
		if ((status = tgSetTgpd2 (SENSOR_TGPD)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// TGES設定
		if ((status = tgSetTges2 (SENSOR_TGES)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// TGSE設定
		if ((status = tgSetTgse2 (SENSOR_TGSE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else	// Normalモード
	{
		// Shutter ModeがNormal Modeの場合は、フレームレートからH interval計算して取得

		// DRRS時のフレームレートはHintervalで決まる
		// Hintervalは以下のacquisitionSetExposure関数で設定する為、ここでは未設定にする
		#if 0
		// 最小レート時間取得
		if ((status = rateMin (&rMin)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 最大レート時間取得
		if ((status = rateMax (&rMax)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 最小露光時間Check
		if (frameRateD < rMin)
			frameRateD = rMin + 0.2;

		// 最大露光時間Check
		if (frameRateD > rMax)
			frameRateD = rMax - 0.2;

		// Set H Interval(今回のフレームレートでのHinterval値を設定)
		if ((status = sensorSetHIntervalTrgNormal (frameRateD)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		#endif
		
		// 露光時間設定
		//if ((status = acquisitionSetExposure (expTime)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// Get H Interval
		//if ((status = sensorGetHInterval (&hInterval)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// HeightのTotal Sizeを求める
		if ((status = fpgaRoiGetSensorHeightTotalSize (&totalHeight)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// フレームレート設定(フレームレートはVMAXの値を設定)
		if (drrsMode == MODE_ENABLE)
			//OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, (SENSOR_DRRS_VMAX & GENICAM_ACQUISITION_FRAMERATE_MASK));
			OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, ((totalHeight +  SENSOR_DRRS_FIX_LINE) & GENICAM_ACQUISITION_FRAMERATE_MASK));
		else
			OUT32 (GENICAM_ACQUISITION_FRAMERATE_ADRS, (SENSOR_VINTERVAL_LINE & GENICAM_ACQUISITION_FRAMERATE_MASK));
	

		// 露光時間取得
		//if ((status = acquisitionGetExposure (&expTime)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// Genicam露光時間は固定値を設定
		OUT32 (GENICAM_ACQUISITION_EXPOSURE_ADRS, (SENSOR_EXPOSURE_FIX & GENICAM_ACQUISITION_EXPOSURE_MASK));
		
		// TGPD設定
		//if ((status = tgSetTgpd2 (SENSOR_TGPD_DRRS)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// TGES設定
		if ((status = tgSetTges2 (SENSOR_TGES_DRRS)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// TGSE設定
		//if ((status = tgSetTgse2 (SENSOR_TGSE_DRRS)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;
	}

	// モード設定
	//OUT32 (FIRM_DATA_SENSOR_SHUTTER_MODE, mode);

	// 最小露光時間取得
	if ((status = exposureMin ((int *)&expTimeMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;	

	// 最大露光時間取得
	if ((status = exposureMax ((int *)&expTimeMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;	

	// 最小露光時間Check
	if (expTime < expTimeMin)
		expTime = expTimeMin;

	// 最大露光時間Check
	if (expTime > expTimeMax)
		expTime = expTimeMax;

	// Set Exposure
	if ((status = acquisitionSetExposure (expTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;	

_DONE:
	if (status != AVAL_STATUS_SUCCESS)
	{
		// モード設定もとに戻す
		if (modeSave != 0xffffffff)
			OUT32 (FIRM_DATA_SENSOR_SHUTTER_MODE, modeSave);
	}

	return (status);
}


//**********************************************************************************
//	Sensor Shutter Normal/Trigger Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：SHUTTER_MODE_NORMAL=Normal/SHUTTER_MODE_TRIGGER=Triggerを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetShutterMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Get Shutter Mode NULL Parameter Error.\n");
		goto _DONE;
	}

	*pMode = IN32 (FIRM_DATA_SENSOR_SHUTTER_MODE);

_DONE:
	return (status);
}


#if defined (MODE_SENSOR_DRRS)
//**********************************************************************************
//	DRRSモード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorDrrsInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int currentMode, newMode;

	// Frame Buffer Initialize
	OUT32 (FPGA_DRRS_FRAME_BUFFER_ADRS, DRRS_IMG_MEMORY_ADRS);
	OUT32 (FPGA_DRRS2_FRAME_BUFFER_ADRS, (DRRS_IMG_MEMORY_ADRS + DRRS_IMG_MEMORY_IPU_MULTI_INTERVAL));

	// Set DRRSモード(現在設定されているモード)
	if ((status = sensorGetDrrs (&currentMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 今回設定するDRRSモード取得(camePaparamSave領域から取得)
	newMode = IN32 (FIRM_DATA_SENSOR_DRRS_MODE);
	
	if (currentMode != newMode)
	{
		// Set DRRSモード
		if ((status = sensorSetDrrsMain (newMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}

//**********************************************************************************
//	DRRSモード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：MODE_DISABLE=DRRS Disable / MODE_ENABLE=DRRS Enable
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetDrrsMain2 (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor Standby
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DRRS Main
	status = sensorSetDrrsMain (mode);

	// Sensor Standby解除
	sensorStandByCancel ();

	msDelay (1000);

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	DRRSモード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：MODE_DISABLE=DRRS Disable / MODE_ENABLE=DRRS Enable
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetDrrsMain (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;
	int shutterMode;
	unsigned char data8;
	int id;
	int regHold, acTmg;
	unsigned int data32;
	int gaincg;
	int drrsCurrentMode;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DRRS Set Main Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DRRSモード取得
	if ((status = sensorGetDrrs (&drrsCurrentMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 現状Enableで再度Enable?
	if ((mode == MODE_ENABLE) && (drrsCurrentMode == MODE_ENABLE))
		goto _DONE;

	// 現状Disableで再度Disable?
	if ((mode == MODE_DISABLE) && (drrsCurrentMode == MODE_DISABLE))
		goto _DONE;
	
	// Sensor Standby
	//if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		//goto _DONE;
	
	if (mode == MODE_ENABLE)
	{
		// センサレジスタデフォルト値取得
		if ((status = sensorGetRegDefault()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 隠しレジスタ
		if ((status = sensorVendorSDrrsHideRegSetting ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 隠しレジスタ(bitにより異なる)
		if ((status = sensorVendorSDrrsHideRegBit (bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//============================================================
		// Chip ID = 0x02 / Sensor Height
		//============================================================
		id = 0x03;
		regHold = 0;	// レジスタホールドなし
		acTmg = 0;		// 即時アクセス

		// Read Height Low Byte
		if ((status = sensorRegReadByte (id, SENSOR_REG_ROI_V1_WIDTH_ADRS1, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		data32 = data8;

		// Read Height High Byte
		if ((status = sensorRegReadByte (id, SENSOR_REG_ROI_V1_WIDTH_ADRS2, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		data32 |= (data8 << 8);

		//if (mode == MODE_ENABLE)
			data32 += DRRS_HEIGHT_ADD_SIZE;
		//else
			//data32 -= DRRS_HEIGHT_ADD_SIZE;

		// Write Height Low Byte
		data8 = (unsigned char)(data32 & 0xff);
		if ((status = sensorRegWriteByte (id, SENSOR_REG_ROI_V1_WIDTH_ADRS1, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Write Height High Byte
		data8 = (unsigned char)((data32 >> 8) & 0xff);
		if ((status = sensorRegWriteByte (id, SENSOR_REG_ROI_V1_WIDTH_ADRS2, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// センサレジスタデフォルト値設定
		if ((status = sensorSetRegDefault()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// 隠しレジスタ初期化
		//if ((status = sensorVendorSHideRegSetting ()) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// 隠しレジスタ(bitにより異なる)
		if ((status = sensorBitSetHide (bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// DRRSモード設定
	if ((status = sensorSetDrrs (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	if (mode == MODE_ENABLE)
		shutterMode = SHUTTER_MODE_NORMAL;
	else
		shutterMode = SHUTTER_MODE_TRIGGER;

	// Shutterモード設定
	if ((status = sensorSetShutterMode (shutterMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == MODE_ENABLE)
	{
		// Get Conversion Gain
		if ((status = sensorGetConversionGain (&gaincg)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Set Conversion Gain
		if ((status = sensorSetConversionGain (gaincg)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Set Black Level(FPGAのレジスタ)
		if ((status = sensorDrrsSetBlacklevel ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		// 念のため、最後にbitを再設定
		
		// bit取得
		if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// bit設定
		if ((status = aoiSetBitWidth (bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	// Sensor Standby解除
	//if ((status2 = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		//status = status2;

	return (status);
}


//**********************************************************************************
//	DRRSモード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：MODE_DISABLE=DRRS Disable / MODE_ENABLE=DRRS Enable
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetDrrs (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DRRS Set Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// DRRSモード取得
	data32 = IN32 (FPGA_DRRS_CTRL_ADRS);

	if (mode == MODE_DISABLE)
		data32 &= ~FPGA_DRRS_CTRL_ENABLE_BIT;
	else
		data32 |= FPGA_DRRS_CTRL_ENABLE_BIT;

	// DRRSモード設定
	OUT32 (FPGA_DRRS_CTRL_ADRS, data32);
	OUT32 (FPGA_DRRS2_CTRL_ADRS, data32);
	
	// DDRに保存
	OUT32 (FIRM_DATA_SENSOR_DRRS_MODE, mode);
	
	// DRRSモード設定(センサレジスタ)
	if ((status = sensorSetDrrsSensorRegister (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	DRRSモード取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：MODE_DISABLE=DRRS Disable / MODE_ENABLE=DRRS Enableを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetDrrs (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Get Shutter Mode NULL Parameter Error.\n");
		goto _DONE;
	}

	#if !defined (MODE_SENSOR_DRRS)
	*pMode = MODE_DISABLE;
	goto _DONE;
	#endif

	// DRRSモード取得
	data32 = IN32 (FPGA_DRRS_CTRL_ADRS);

	if ((data32 & FPGA_DRRS_CTRL_ENABLE_BIT) != 0)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	DRRSモード設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：MODE_DISABLE=DRRS Disable / MODE_ENABLE=DRRS Enable
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetDrrsSensorRegister (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int acTmg, regHold;
	unsigned char data8;
	int id;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DRRS Set Sensor Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//============================================================
	// Chip ID = 0x04 / SMD1
	//============================================================
	id = 0x04;
	acTmg = 0;
	regHold = 0;

	// Read
	if ((status = sensorRegReadByte (id, SENSOR_REG_SMD1_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == MODE_ENABLE)
		data8 &= ~(1<<0);		// DRRSモード
	else
		data8 |= (1<<0);		// 通常撮像モード

	// Write
	if ((status = sensorRegWriteByte (id, SENSOR_REG_SMD1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x04 / SMD2
	//============================================================
	id = 0x04;
	acTmg = 0;
	regHold = 0;

	// Read
	if ((status = sensorRegReadByte (id, SENSOR_REG_SMD2_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == MODE_ENABLE)
		data8 |= (1<<5);		// DRRSモード
	else
		data8 &= ~(1<<5);		// 通常撮像モード

	// Write
	if ((status = sensorRegWriteByte (id, SENSOR_REG_SMD2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor DRRS Hide Register Setting
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorVendorSDrrsHideRegSetting (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char adrs;
	unsigned char data8;
	int regHold, acTmg;
	unsigned char id;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//============================================================
	//============================================================
	// 隠しレジスタ
	//============================================================
	//============================================================

	//============================================================
	// Chip ID = 0x02
	//============================================================
	id = 0x02;

	//------------------------------------------------------------
	// Adrs = 0x40
	//------------------------------------------------------------
	adrs = 0x40;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x41
	//------------------------------------------------------------
	adrs = 0x41;
	data8 = 0x10;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	adrs = 0x44;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x46
	//------------------------------------------------------------
	adrs = 0x46;
	data8 = 0x04;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x48
	//------------------------------------------------------------
	adrs = 0x48;
	data8 = 0x42;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4C
	//------------------------------------------------------------
	adrs = 0x4C;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x54
	//------------------------------------------------------------
	adrs = 0x54;
	data8 = 0x0d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x56
	//------------------------------------------------------------
	adrs = 0x56;
	data8 = 0x1c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x58
	//------------------------------------------------------------
	adrs = 0x58;
	data8 = 0x42;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5C
	//------------------------------------------------------------
	adrs = 0x5C;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x64
	//------------------------------------------------------------
	adrs = 0x64;
	data8 = 0x3d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x66
	//------------------------------------------------------------
	adrs = 0x66;
	data8 = 0x22;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x68
	//------------------------------------------------------------
	adrs = 0x68;
	data8 = 0x42;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6c
	//------------------------------------------------------------
	adrs = 0x6c;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x74
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0x71;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0x46;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x77
	//------------------------------------------------------------
	adrs = 0x77;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x42;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7c
	//------------------------------------------------------------
	adrs = 0x7c;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd4
	//------------------------------------------------------------
	adrs = 0xd4;
	data8 = 0x46;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe0
	//------------------------------------------------------------
	adrs = 0xe0;
	data8 = 0x04;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe1
	//------------------------------------------------------------
	adrs = 0xe1;
	data8 = 0x04;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xec
	//------------------------------------------------------------
	adrs = 0xec;
	data8 = 0x2f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf4
	//------------------------------------------------------------
	adrs = 0xf4;
	data8 = 0x9c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf5
	//------------------------------------------------------------
	adrs = 0xf5;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf6
	//------------------------------------------------------------
	adrs = 0xf6;
	data8 = 0xa7;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf7
	//------------------------------------------------------------
	adrs = 0xf7;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf8
	//------------------------------------------------------------
	adrs = 0xf8;
	data8 = 0x13;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x04
	//============================================================
	id = 0x04;

	//------------------------------------------------------------
	// Adrs = 0x00
	//------------------------------------------------------------
	// ビット毎に変わるのでここでは省略

	//------------------------------------------------------------
	// Adrs = 0x30
	//------------------------------------------------------------
	adrs = 0x30;
	data8 = 0x28;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x32
	//------------------------------------------------------------
	adrs = 0x32;
	data8 = 0x21;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x40
	//------------------------------------------------------------
	adrs = 0x40;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	adrs = 0x44;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x48
	//------------------------------------------------------------
	adrs = 0x48;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4c
	//------------------------------------------------------------
	adrs = 0x4c;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x60
	//------------------------------------------------------------
	adrs = 0x60;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62/通常時の動作とは異なる設定
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x63/通常時の動作とは異なる設定
	//------------------------------------------------------------
	adrs = 0x63;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa0
	//------------------------------------------------------------
	adrs = 0xa0;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa1
	//------------------------------------------------------------
	adrs = 0xa1;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x05
	//============================================================
	id = 0x05;

	//------------------------------------------------------------
	// Adrs = 0x70
	//------------------------------------------------------------
	adrs = 0x70;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x71
	//------------------------------------------------------------
	adrs = 0x71;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc2
	//------------------------------------------------------------
	adrs = 0xc2;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc3
	//------------------------------------------------------------
	adrs = 0xc3;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x06
	//============================================================
	id = 0x06;

	//------------------------------------------------------------
	// Adrs = 0x30
	//------------------------------------------------------------
	// Bit毎に異なるのでここでは省略

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa4
	//------------------------------------------------------------
	adrs = 0xa4;
	data8 = 0x3b;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa5
	//------------------------------------------------------------
	adrs = 0xa5;
	data8 = 0x71;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xac
	//------------------------------------------------------------
	adrs = 0xac;
	data8 = 0x3b;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xad
	//------------------------------------------------------------
	adrs = 0xad;
	data8 = 0x71;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xbd
	//------------------------------------------------------------
	adrs = 0xbd;
	data8 = 0x40;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x07
	//============================================================
	id = 0x07;

	//------------------------------------------------------------
	// Adrs = 0x3d
	//------------------------------------------------------------
	adrs = 0x3d;
	data8 = 0xc0;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3e
	//------------------------------------------------------------
	adrs = 0x3e;
	data8 = 0x12;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x42
	//------------------------------------------------------------
	adrs = 0x42;
	data8 = 0x05;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4b
	//------------------------------------------------------------
	adrs = 0x4b;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x98
	//------------------------------------------------------------
	adrs = 0x98;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc0 => Bitごとで異なるため、sensorVendorSDrrsHideRegBitで設定
	//------------------------------------------------------------


	//------------------------------------------------------------
	// Adrs = 0xc8 => Bitごとで異なるため、sensorVendorSDrrsHideRegBitで設定
	//------------------------------------------------------------


	//------------------------------------------------------------
	// Adrs = 0xe0
	//------------------------------------------------------------
	adrs = 0xe0;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xec
	//------------------------------------------------------------
	adrs = 0xec;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x08
	//============================================================
	id = 0x08;

	//------------------------------------------------------------
	// Adrs = 0x00
	//------------------------------------------------------------
	adrs = 0x00;
	data8 = 0x06;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x02
	//------------------------------------------------------------
	adrs = 0x02;
	data8 = 0x0c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x04
	//------------------------------------------------------------
	adrs = 0x04;
	data8 = 0x34;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x06
	//------------------------------------------------------------
	adrs = 0x06;
	data8 = 0x31;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x08
	//------------------------------------------------------------
	adrs = 0x08;
	data8 = 0x3c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0a
	//------------------------------------------------------------
	adrs = 0x0a;
	data8 = 0x17;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0c
	//------------------------------------------------------------
	adrs = 0x0c;
	data8 = 0x6c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0e
	//------------------------------------------------------------
	adrs = 0x0e;
	data8 = 0x04;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x10
	//------------------------------------------------------------
	adrs = 0x10;
	data8 = 0x7a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x12
	//------------------------------------------------------------
	adrs = 0x12;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x14
	//------------------------------------------------------------
	adrs = 0x14;
	data8 = 0xa8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x16
	//------------------------------------------------------------
	adrs = 0x16;
	data8 = 0x0a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x18
	//------------------------------------------------------------
	adrs = 0x18;
	data8 = 0xba;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1a
	//------------------------------------------------------------
	adrs = 0x1a;
	data8 = 0x07;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1c
	//------------------------------------------------------------
	adrs = 0x1c;
	data8 = 0x8e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1d
	//------------------------------------------------------------
	adrs = 0x1d;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1e
	//------------------------------------------------------------
	adrs = 0x1e;
	data8 = 0x11;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x20
	//------------------------------------------------------------
	adrs = 0x20;
	data8 = 0x11;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x22
	//------------------------------------------------------------
	adrs = 0x22;
	data8 = 0x24;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x24
	//------------------------------------------------------------
	adrs = 0x24;
	data8 = 0x3a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x26
	//------------------------------------------------------------
	adrs = 0x26;
	data8 = 0x0c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x28
	//------------------------------------------------------------
	adrs = 0x28;
	data8 = 0x54;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x29
	//------------------------------------------------------------
	adrs = 0x29;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2a
	//------------------------------------------------------------
	adrs = 0x2a;
	data8 = 0x0a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2c
	//------------------------------------------------------------
	adrs = 0x2c;
	data8 = 0x66;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2d
	//------------------------------------------------------------
	adrs = 0x2d;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2e
	//------------------------------------------------------------
	adrs = 0x2e;
	data8 = 0x48;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x38
	//------------------------------------------------------------
	adrs = 0x38;
	data8 = 0x06;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3a
	//------------------------------------------------------------
	adrs = 0x3a;
	data8 = 0x0c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3c
	//------------------------------------------------------------
	adrs = 0x3c;
	data8 = 0x34;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3e
	//------------------------------------------------------------
	adrs = 0x3e;
	data8 = 0x31;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x40
	//------------------------------------------------------------
	adrs = 0x40;
	data8 = 0x3c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x42
	//------------------------------------------------------------
	adrs = 0x42;
	data8 = 0x30;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	adrs = 0x44;
	data8 = 0x6c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x46
	//------------------------------------------------------------
	adrs = 0x46;
	data8 = 0x04;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x48
	//------------------------------------------------------------
	adrs = 0x48;
	data8 = 0x7a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4a
	//------------------------------------------------------------
	adrs = 0x4a;
	data8 = 0x11;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4c
	//------------------------------------------------------------
	adrs = 0x4c;
	data8 = 0xa8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4e
	//------------------------------------------------------------
	adrs = 0x4e;
	data8 = 0x10;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x50
	//------------------------------------------------------------
	adrs = 0x50;
	data8 = 0xba;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x52 => Bitごとで異なるため、sensorVendorSDrrsHideRegBitで設定
	//------------------------------------------------------------

	//------------------------------------------------------------
	// Adrs = 0x54
	//------------------------------------------------------------
	adrs = 0x54;
	data8 = 0x8e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x55
	//------------------------------------------------------------
	adrs = 0x55;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x56
	//------------------------------------------------------------
	adrs = 0x56;
	data8 = 0x11;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x58
	//------------------------------------------------------------
	adrs = 0x58;
	data8 = 0x11;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5a
	//------------------------------------------------------------
	adrs = 0x5a;
	data8 = 0x24;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5c
	//------------------------------------------------------------
	adrs = 0x5c;
	data8 = 0x3a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5e
	//------------------------------------------------------------
	adrs = 0x5e;
	data8 = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5f
	//------------------------------------------------------------
	adrs = 0x5f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x60
	//------------------------------------------------------------
	adrs = 0x60;
	data8 = 0x54;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x61
	//------------------------------------------------------------
	adrs = 0x61;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0x10;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x64
	//------------------------------------------------------------
	adrs = 0x64;
	data8 = 0x66;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x65
	//------------------------------------------------------------
	adrs = 0x65;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x66 => Bitごとで異なるため、sensorVendorSDrrsHideRegBitで設定
	//------------------------------------------------------------

	//------------------------------------------------------------
	// Adrs = 0x67
	//------------------------------------------------------------
	adrs = 0x67;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x70
	//------------------------------------------------------------
	adrs = 0x70;
	data8 = 0x06;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x72
	//------------------------------------------------------------
	adrs = 0x72;
	data8 = 0x0c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x74
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0x34;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0x31;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x3c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7a
	//------------------------------------------------------------
	adrs = 0x7a;
	data8 = 0x17;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7c
	//------------------------------------------------------------
	adrs = 0x7c;
	data8 = 0x6c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7e
	//------------------------------------------------------------
	adrs = 0x7e;
	data8 = 0x04;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0x7a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x82
	//------------------------------------------------------------
	adrs = 0x82;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0xa8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x86
	//------------------------------------------------------------
	adrs = 0x86;
	data8 = 0x0a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x88
	//------------------------------------------------------------
	adrs = 0x88;
	data8 = 0xba;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8a
	//------------------------------------------------------------
	adrs = 0x8a;
	data8 = 0x07;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8c
	//------------------------------------------------------------
	adrs = 0x8c;
	data8 = 0x8e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8d
	//------------------------------------------------------------
	adrs = 0x8d;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8e
	//------------------------------------------------------------
	adrs = 0x8e;
	data8 = 0x11;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x90
	//------------------------------------------------------------
	adrs = 0x90;
	data8 = 0x11;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x92
	//------------------------------------------------------------
	adrs = 0x92;
	data8 = 0x24;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x94
	//------------------------------------------------------------
	adrs = 0x94;
	data8 = 0x3a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x96
	//------------------------------------------------------------
	adrs = 0x96;
	data8 = 0x0c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x98
	//------------------------------------------------------------
	adrs = 0x98;
	data8 = 0x54;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x99
	//------------------------------------------------------------
	adrs = 0x99;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9a
	//------------------------------------------------------------
	adrs = 0x9a;
	data8 = 0x0a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9c
	//------------------------------------------------------------
	adrs = 0x9c;
	data8 = 0x66;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9d
	//------------------------------------------------------------
	adrs = 0x9d;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9e
	//------------------------------------------------------------
	adrs = 0x9e;
	data8 = 0x18;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x09
	//============================================================
	id = 0x09;

	//------------------------------------------------------------
	// Adrs = 0x60
	//------------------------------------------------------------
	adrs = 0x60;
	data8 = 0xd1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0x81;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x0a
	//============================================================
	id = 0x0a;

	//============================================================
	// Chip ID = 0x0b
	//============================================================
	id = 0x0b;

	//============================================================
	// Chip ID = 0x0c
	//============================================================
	id = 0x0c;

	//============================================================
	// Chip ID = 0x0d
	//============================================================
	id = 0x0d;

	//------------------------------------------------------------
	// Adrs = 0x6c
	//------------------------------------------------------------
	adrs = 0x6c;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6d
	//------------------------------------------------------------
	adrs = 0x6d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x0e
	//============================================================
	id = 0x0e;

	//============================================================
	// Chip ID = 0x0f
	//============================================================
	id = 0x0f;

	//============================================================
	// Chip ID = 0x10
	//============================================================
	id = 0x10;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0xd1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x70
	//------------------------------------------------------------
	adrs = 0x70;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x71
	//------------------------------------------------------------
	adrs = 0x71;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0x91;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x77
	//------------------------------------------------------------
	adrs = 0x77;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa0
	//------------------------------------------------------------
	adrs = 0xa0;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa1
	//------------------------------------------------------------
	adrs = 0xa1;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa6
	//------------------------------------------------------------
	adrs = 0xa6;
	data8 = 0x91;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa7
	//------------------------------------------------------------
	adrs = 0xa7;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc0
	//------------------------------------------------------------
	adrs = 0xc0;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc1
	//------------------------------------------------------------
	adrs = 0xc1;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf0
	//------------------------------------------------------------
	adrs = 0xf0;
	data8 = 0x18;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf2
	//------------------------------------------------------------
	adrs = 0xf2;
	data8 = 0xd6;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x11
	//============================================================
	id = 0x11;

	//------------------------------------------------------------
	// Adrs = 0x14
	//------------------------------------------------------------
	adrs = 0x14;
	data8 = 0x1e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x15
	//------------------------------------------------------------
	adrs = 0x15;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x16
	//------------------------------------------------------------
	adrs = 0x16;
	data8 = 0xed;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x38
	//------------------------------------------------------------
	adrs = 0x38;
	data8 = 0x95;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x74
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0x88;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0x1e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x85
	//------------------------------------------------------------
	adrs = 0x85;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x86
	//------------------------------------------------------------
	adrs = 0x86;
	data8 = 0xed;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb8
	//------------------------------------------------------------
	adrs = 0xb8;
	data8 = 0x95;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe4
	//------------------------------------------------------------
	adrs = 0xe4;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe5
	//------------------------------------------------------------
	adrs = 0xe5;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x12
	//============================================================
	id = 0x12;

	//------------------------------------------------------------
	// Adrs = 0x18
	//------------------------------------------------------------
	adrs = 0x18;
	data8 = 0x83;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1a
	//------------------------------------------------------------
	adrs = 0x1a;
	data8 = 0x14;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x28
	//------------------------------------------------------------
	adrs = 0x28;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x68
	//------------------------------------------------------------
	adrs = 0x68;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x25;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7e
	//------------------------------------------------------------
	adrs = 0x7e;
	data8 = 0x6e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc8
	//------------------------------------------------------------
	adrs = 0xc8;
	data8 = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xca
	//------------------------------------------------------------
	adrs = 0xca;
	data8 = 0x73;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xda
	//------------------------------------------------------------
	adrs = 0xda;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xea
	//------------------------------------------------------------
	adrs = 0xea;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xeb
	//------------------------------------------------------------
	adrs = 0xeb;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf0
	//------------------------------------------------------------
	adrs = 0xf0;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x13
	//============================================================
	id = 0x13;

	//------------------------------------------------------------
	// Adrs = 0x04
	//------------------------------------------------------------
	adrs = 0x04;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x05
	//------------------------------------------------------------
	adrs = 0x05;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xee
	//------------------------------------------------------------
	adrs = 0xee;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xef
	//------------------------------------------------------------
	adrs = 0xef;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x14
	//============================================================
	id = 0x14;

	//------------------------------------------------------------
	// Adrs = 0x08
	//------------------------------------------------------------
	adrs = 0x08;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x09
	//------------------------------------------------------------
	adrs = 0x09;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x18
	//------------------------------------------------------------
	adrs = 0x18;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x19
	//------------------------------------------------------------
	adrs = 0x19;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1a
	//------------------------------------------------------------
	adrs = 0x1a;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1b
	//------------------------------------------------------------
	adrs = 0x1b;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x32
	//------------------------------------------------------------
	adrs = 0x32;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x33
	//------------------------------------------------------------
	adrs = 0x33;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x34
	//------------------------------------------------------------
	adrs = 0x34;
	data8 = 0x93;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x35
	//------------------------------------------------------------
	adrs = 0x35;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x50
	//------------------------------------------------------------
	adrs = 0x50;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x51
	//------------------------------------------------------------
	adrs = 0x51;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x56
	//------------------------------------------------------------
	adrs = 0x56;
	data8 = 0x8f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x57
	//------------------------------------------------------------
	adrs = 0x57;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x92
	//------------------------------------------------------------
	adrs = 0x92;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x93
	//------------------------------------------------------------
	adrs = 0x93;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x94
	//------------------------------------------------------------
	adrs = 0x94;
	data8 = 0x93;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x95
	//------------------------------------------------------------
	adrs = 0x95;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb0
	//------------------------------------------------------------
	adrs = 0xb0;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb1
	//------------------------------------------------------------
	adrs = 0xb1;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb6
	//------------------------------------------------------------
	adrs = 0xb6;
	data8 = 0x8f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb7
	//------------------------------------------------------------
	adrs = 0xb7;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf0
	//------------------------------------------------------------
	adrs = 0xf0;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf1
	//------------------------------------------------------------
	adrs = 0xf1;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf6
	//------------------------------------------------------------
	adrs = 0xf6;
	data8 = 0x8f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf7
	//------------------------------------------------------------
	adrs = 0xf7;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x15
	//============================================================
	id = 0x15;

	//------------------------------------------------------------
	// Adrs = 0x14
	//------------------------------------------------------------
	adrs = 0x14;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x15
	//------------------------------------------------------------
	adrs = 0x15;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x16
	//------------------------------------------------------------
	adrs = 0x16;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x17
	//------------------------------------------------------------
	adrs = 0x17;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x18
	//------------------------------------------------------------
	adrs = 0x18;
	data8 = 0x91;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x19
	//------------------------------------------------------------
	adrs = 0x19;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x24
	//------------------------------------------------------------
	adrs = 0x24;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x25
	//------------------------------------------------------------
	adrs = 0x25;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x26
	//------------------------------------------------------------
	adrs = 0x26;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x27
	//------------------------------------------------------------
	adrs = 0x27;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x54
	//------------------------------------------------------------
	adrs = 0x54;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x55
	//------------------------------------------------------------
	adrs = 0x55;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5a
	//------------------------------------------------------------
	adrs = 0x5a;
	data8 = 0x8f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5b
	//------------------------------------------------------------
	adrs = 0x5b;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x74
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x75
	//------------------------------------------------------------
	adrs = 0x75;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x77
	//------------------------------------------------------------
	adrs = 0x77;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x91;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x79
	//------------------------------------------------------------
	adrs = 0x79;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x85
	//------------------------------------------------------------
	adrs = 0x85;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x86
	//------------------------------------------------------------
	adrs = 0x86;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x87
	//------------------------------------------------------------
	adrs = 0x87;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x16
	//============================================================
	id = 0x16;

	//============================================================
	// Chip ID = 0x17
	//============================================================
	id = 0x17;

	//------------------------------------------------------------
	// Adrs = 0x4c
	//------------------------------------------------------------
	adrs = 0x4c;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4e
	//------------------------------------------------------------
	adrs = 0x4e;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x50
	//------------------------------------------------------------
	adrs = 0x50;
	data8 = 0x64;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x52
	//------------------------------------------------------------
	adrs = 0x52;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x54
	//------------------------------------------------------------
	adrs = 0x54;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x56
	//------------------------------------------------------------
	adrs = 0x56;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x58
	//------------------------------------------------------------
	adrs = 0x58;
	data8 = 0x64;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5a
	//------------------------------------------------------------
	adrs = 0x5a;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5c
	//------------------------------------------------------------
	adrs = 0x5c;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5e
	//------------------------------------------------------------
	adrs = 0x5e;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x60
	//------------------------------------------------------------
	adrs = 0x60;
	data8 = 0x64;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x64
	//------------------------------------------------------------
	adrs = 0x64;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x66
	//------------------------------------------------------------
	adrs = 0x66;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x68
	//------------------------------------------------------------
	adrs = 0x68;
	data8 = 0x64;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6a
	//------------------------------------------------------------
	adrs = 0x6a;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6c
	//------------------------------------------------------------
	adrs = 0x6c;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6e
	//------------------------------------------------------------
	adrs = 0x6e;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x70
	//------------------------------------------------------------
	adrs = 0x70;
	data8 = 0x64;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x72
	//------------------------------------------------------------
	adrs = 0x72;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x74
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x64;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7a
	//------------------------------------------------------------
	adrs = 0x7a;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7c
	//------------------------------------------------------------
	adrs = 0x7c;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7e
	//------------------------------------------------------------
	adrs = 0x7e;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0x64;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x82
	//------------------------------------------------------------
	adrs = 0x82;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x86
	//------------------------------------------------------------
	adrs = 0x86;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x88
	//------------------------------------------------------------
	adrs = 0x88;
	data8 = 0x64;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8a
	//------------------------------------------------------------
	adrs = 0x8a;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8c
	//------------------------------------------------------------
	adrs = 0x8c;
	data8 = 0xa6;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8e
	//------------------------------------------------------------
	adrs = 0x8e;
	data8 = 0xd1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x90
	//------------------------------------------------------------
	adrs = 0x90;
	data8 = 0x52;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x92
	//------------------------------------------------------------
	adrs = 0x92;
	data8 = 0x7d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x94
	//------------------------------------------------------------
	adrs = 0x94;
	data8 = 0xa0;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x96
	//------------------------------------------------------------
	adrs = 0x96;
	data8 = 0xe1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x98
	//------------------------------------------------------------
	adrs = 0x98;
	data8 = 0x40;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9a
	//------------------------------------------------------------
	adrs = 0x9a;
	data8 = 0x88;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf4
	//------------------------------------------------------------
	adrs = 0xf4;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf6
	//------------------------------------------------------------
	adrs = 0xf6;
	data8 = 0x5f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf8
	//------------------------------------------------------------
	adrs = 0xf8;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xfa
	//------------------------------------------------------------
	adrs = 0xfa;
	data8 = 0x5f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x18
	//============================================================
	id = 0x18;

	//------------------------------------------------------------
	// Adrs = 0x00
	//------------------------------------------------------------
	adrs = 0x00;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x02
	//------------------------------------------------------------
	adrs = 0x02;
	data8 = 0x5f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x04
	//------------------------------------------------------------
	adrs = 0x04;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x06
	//------------------------------------------------------------
	adrs = 0x06;
	data8 = 0x5f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x08
	//------------------------------------------------------------
	adrs = 0x08;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0a
	//------------------------------------------------------------
	adrs = 0x0a;
	data8 = 0x5f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0c
	//------------------------------------------------------------
	adrs = 0x0c;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0e
	//------------------------------------------------------------
	adrs = 0x0e;
	data8 = 0x5f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x10
	//------------------------------------------------------------
	adrs = 0x10;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x12
	//------------------------------------------------------------
	adrs = 0x12;
	data8 = 0x5f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x14
	//------------------------------------------------------------
	adrs = 0x14;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x16
	//------------------------------------------------------------
	adrs = 0x16;
	data8 = 0x5f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x18
	//------------------------------------------------------------
	adrs = 0x18;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1a
	//------------------------------------------------------------
	adrs = 0x1a;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1c
	//------------------------------------------------------------
	adrs = 0x1c;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1e
	//------------------------------------------------------------
	adrs = 0x1e;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x20
	//------------------------------------------------------------
	adrs = 0x20;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x22
	//------------------------------------------------------------
	adrs = 0x22;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x24
	//------------------------------------------------------------
	adrs = 0x24;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x26
	//------------------------------------------------------------
	adrs = 0x26;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x28
	//------------------------------------------------------------
	adrs = 0x28;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2a
	//------------------------------------------------------------
	adrs = 0x2a;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2c
	//------------------------------------------------------------
	adrs = 0x2c;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2e
	//------------------------------------------------------------
	adrs = 0x2e;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x30
	//------------------------------------------------------------
	adrs = 0x30;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x32
	//------------------------------------------------------------
	adrs = 0x32;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x34
	//------------------------------------------------------------
	adrs = 0x34;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x36
	//------------------------------------------------------------
	adrs = 0x36;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x38
	//------------------------------------------------------------
	adrs = 0x38;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3a
	//------------------------------------------------------------
	adrs = 0x3a;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3c
	//------------------------------------------------------------
	adrs = 0x3c;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3e
	//------------------------------------------------------------
	adrs = 0x3e;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x40
	//------------------------------------------------------------
	adrs = 0x40;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x42
	//------------------------------------------------------------
	adrs = 0x42;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	adrs = 0x44;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x46
	//------------------------------------------------------------
	adrs = 0x46;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x48
	//------------------------------------------------------------
	adrs = 0x48;
	data8 = 0x63;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4a
	//------------------------------------------------------------
	adrs = 0x4a;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4c
	//------------------------------------------------------------
	adrs = 0x4c;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4e
	//------------------------------------------------------------
	adrs = 0x4e;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x50
	//------------------------------------------------------------
	adrs = 0x50;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x52
	//------------------------------------------------------------
	adrs = 0x52;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x54
	//------------------------------------------------------------
	adrs = 0x54;
	data8 = 0x78;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x56
	//------------------------------------------------------------
	adrs = 0x56;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x58
	//------------------------------------------------------------
	adrs = 0x58;
	data8 = 0xab;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5a
	//------------------------------------------------------------
	adrs = 0x5a;
	data8 = 0x98;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5c
	//------------------------------------------------------------
	adrs = 0x5c;
	data8 = 0x57;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5e
	//------------------------------------------------------------
	adrs = 0x5e;
	data8 = 0x44;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5f
	//------------------------------------------------------------
	adrs = 0x5f;
	data8 = 0xa1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x60
	//------------------------------------------------------------
	adrs = 0x60;
	data8 = 0x2a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0x94;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x68
	//------------------------------------------------------------
	adrs = 0x68;
	data8 = 0x2c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6a
	//------------------------------------------------------------
	adrs = 0x6a;
	data8 = 0x7c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x70
	//------------------------------------------------------------
	adrs = 0x70;
	data8 = 0x2b;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x72
	//------------------------------------------------------------
	adrs = 0x72;
	data8 = 0x2e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x31;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7a
	//------------------------------------------------------------
	adrs = 0x7a;
	data8 = 0x94;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0x32;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x82
	//------------------------------------------------------------
	adrs = 0x82;
	data8 = 0x95;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xac
	//------------------------------------------------------------
	adrs = 0xac;
	data8 = 0x82;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xae
	//------------------------------------------------------------
	adrs = 0xae;
	data8 = 0x88;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xbc
	//------------------------------------------------------------
	adrs = 0xbc;
	data8 = 0x86;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xbe
	//------------------------------------------------------------
	adrs = 0xbe;
	data8 = 0x8c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd8
	//------------------------------------------------------------
	adrs = 0xd8;
	data8 = 0x82;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xda
	//------------------------------------------------------------
	adrs = 0xda;
	data8 = 0x88;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe8
	//------------------------------------------------------------
	adrs = 0xe8;
	data8 = 0x86;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xea
	//------------------------------------------------------------
	adrs = 0xea;
	data8 = 0x8c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x19
	//============================================================
	id = 0x19;

	//------------------------------------------------------------
	// Adrs = 0x0b
	//------------------------------------------------------------
	adrs = 0x0b;
	data8 = 0x1f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x20
	//------------------------------------------------------------
	adrs = 0x20;
	data8 = 0xa6;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x22
	//------------------------------------------------------------
	adrs = 0x22;
	data8 = 0xd1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x24
	//------------------------------------------------------------
	adrs = 0x24;
	data8 = 0x52;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x26
	//------------------------------------------------------------
	adrs = 0x26;
	data8 = 0x7d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x28
	//------------------------------------------------------------
	adrs = 0x28;
	data8 = 0x85;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2a
	//------------------------------------------------------------
	adrs = 0x2a;
	data8 = 0x7d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x30
	//------------------------------------------------------------
	adrs = 0x30;
	data8 = 0x7c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x32
	//------------------------------------------------------------
	adrs = 0x32;
	data8 = 0x86;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x34
	//------------------------------------------------------------
	adrs = 0x34;
	data8 = 0x1a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x35
	//------------------------------------------------------------
	adrs = 0x35;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x36
	//------------------------------------------------------------
	adrs = 0x36;
	data8 = 0x2b;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x37
	//------------------------------------------------------------
	adrs = 0x37;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x38
	//------------------------------------------------------------
	adrs = 0x38;
	data8 = 0x44;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3a
	//------------------------------------------------------------
	adrs = 0x3a;
	data8 = 0x58;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x40
	//------------------------------------------------------------
	adrs = 0x40;
	data8 = 0x7d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x42
	//------------------------------------------------------------
	adrs = 0x42;
	data8 = 0x87;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	adrs = 0x44;
	data8 = 0x1b;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x45
	//------------------------------------------------------------
	adrs = 0x45;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x46
	//------------------------------------------------------------
	adrs = 0x46;
	data8 = 0x2c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x47
	//------------------------------------------------------------
	adrs = 0x47;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x48
	//------------------------------------------------------------
	adrs = 0x48;
	data8 = 0x45;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4a
	//------------------------------------------------------------
	adrs = 0x4a;
	data8 = 0x59;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x50
	//------------------------------------------------------------
	adrs = 0x50;
	data8 = 0x7e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x52
	//------------------------------------------------------------
	adrs = 0x52;
	data8 = 0x88;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x54
	//------------------------------------------------------------
	adrs = 0x54;
	data8 = 0x1c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x55
	//------------------------------------------------------------
	adrs = 0x55;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x56
	//------------------------------------------------------------
	adrs = 0x56;
	data8 = 0x2d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x57
	//------------------------------------------------------------
	adrs = 0x57;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x58
	//------------------------------------------------------------
	adrs = 0x58;
	data8 = 0x46;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5a
	//------------------------------------------------------------
	adrs = 0x5a;
	data8 = 0x5a;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x60
	//------------------------------------------------------------
	adrs = 0x60;
	data8 = 0x15;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x61
	//------------------------------------------------------------
	adrs = 0x61;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0x1f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x63
	//------------------------------------------------------------
	adrs = 0x63;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x64
	//------------------------------------------------------------
	adrs = 0x64;
	data8 = 0x3f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x66
	//------------------------------------------------------------
	adrs = 0x66;
	data8 = 0x47;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x68
	//------------------------------------------------------------
	adrs = 0x68;
	data8 = 0x16;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x69
	//------------------------------------------------------------
	adrs = 0x69;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6a
	//------------------------------------------------------------
	adrs = 0x6a;
	data8 = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6b
	//------------------------------------------------------------
	adrs = 0x6b;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6c
	//------------------------------------------------------------
	adrs = 0x6c;
	data8 = 0x40;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6e
	//------------------------------------------------------------
	adrs = 0x6e;
	data8 = 0x48;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x70
	//------------------------------------------------------------
	adrs = 0x70;
	data8 = 0x17;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x71
	//------------------------------------------------------------
	adrs = 0x71;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x72
	//------------------------------------------------------------
	adrs = 0x72;
	data8 = 0x21;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x73
	//------------------------------------------------------------
	adrs = 0x73;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x74
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0x41;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0x49;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0x7e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x82
	//------------------------------------------------------------
	adrs = 0x82;
	data8 = 0x93;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0x3d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x85
	//------------------------------------------------------------
	adrs = 0x85;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x86
	//------------------------------------------------------------
	adrs = 0x86;
	data8 = 0x43;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x87
	//------------------------------------------------------------
	adrs = 0x87;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x88
	//------------------------------------------------------------
	adrs = 0x88;
	data8 = 0x16;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8a
	//------------------------------------------------------------
	adrs = 0x8a;
	data8 = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8c
	//------------------------------------------------------------
	adrs = 0x8c;
	data8 = 0x9e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8d
	//------------------------------------------------------------
	adrs = 0x8d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8e
	//------------------------------------------------------------
	adrs = 0x8e;
	data8 = 0xa2;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8f
	//------------------------------------------------------------
	adrs = 0x8f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x90
	//------------------------------------------------------------
	adrs = 0x90;
	data8 = 0x7d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x92
	//------------------------------------------------------------
	adrs = 0x92;
	data8 = 0x85;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x94
	//------------------------------------------------------------
	adrs = 0x94;
	data8 = 0x3c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x95
	//------------------------------------------------------------
	adrs = 0x95;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x96
	//------------------------------------------------------------
	adrs = 0x96;
	data8 = 0x44;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x97
	//------------------------------------------------------------
	adrs = 0x97;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x98
	//------------------------------------------------------------
	adrs = 0x98;
	data8 = 0x0c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9a
	//------------------------------------------------------------
	adrs = 0x9a;
	data8 = 0x14;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9c
	//------------------------------------------------------------
	adrs = 0x9c;
	data8 = 0x9d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9d
	//------------------------------------------------------------
	adrs = 0x9d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9e
	//------------------------------------------------------------
	adrs = 0x9e;
	data8 = 0xa3;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9f
	//------------------------------------------------------------
	adrs = 0x9f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa0
	//------------------------------------------------------------
	adrs = 0xa0;
	data8 = 0xcf;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa2
	//------------------------------------------------------------
	adrs = 0xa2;
	data8 = 0xd1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa4
	//------------------------------------------------------------
	adrs = 0xa4;
	data8 = 0x7b;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa6
	//------------------------------------------------------------
	adrs = 0xa6;
	data8 = 0x7d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa8
	//------------------------------------------------------------
	adrs = 0xa8;
	data8 = 0x47;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xac
	//------------------------------------------------------------
	adrs = 0xac;
	data8 = 0xa7;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xae
	//------------------------------------------------------------
	adrs = 0xae;
	data8 = 0x53;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb0
	//------------------------------------------------------------
	adrs = 0xb0;
	data8 = 0x8b;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb2
	//------------------------------------------------------------
	adrs = 0xb2;
	data8 = 0x18;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb4
	//------------------------------------------------------------
	adrs = 0xb4;
	data8 = 0xcf;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb6
	//------------------------------------------------------------
	adrs = 0xb6;
	data8 = 0xd1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb8
	//------------------------------------------------------------
	adrs = 0xb8;
	data8 = 0x7b;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xba
	//------------------------------------------------------------
	adrs = 0xba;
	data8 = 0x7d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe0
	//------------------------------------------------------------
	adrs = 0xe0;
	data8 = 0xa0;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe2
	//------------------------------------------------------------
	adrs = 0xe2;
	data8 = 0xe8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe4
	//------------------------------------------------------------
	adrs = 0xe4;
	data8 = 0x40;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe6
	//------------------------------------------------------------
	adrs = 0xe6;
	data8 = 0x98;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x1a
	//============================================================
	id = 0x1a;

	//------------------------------------------------------------
	// Adrs = 0x00
	//------------------------------------------------------------
	adrs = 0x00;
	data8 = 0x9d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x01
	//------------------------------------------------------------
	adrs = 0x01;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x02
	//------------------------------------------------------------
	adrs = 0x02;
	data8 = 0xa3;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x03
	//------------------------------------------------------------
	adrs = 0x03;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x04
	//------------------------------------------------------------
	adrs = 0x04;
	data8 = 0x49;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x05
	//------------------------------------------------------------
	adrs = 0x05;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x06
	//------------------------------------------------------------
	adrs = 0x06;
	data8 = 0x4f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x07
	//------------------------------------------------------------
	adrs = 0x07;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x08
	//------------------------------------------------------------
	adrs = 0x08;
	data8 = 0x9c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0a
	//------------------------------------------------------------
	adrs = 0x0a;
	data8 = 0xa4;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0c
	//------------------------------------------------------------
	adrs = 0x0c;
	data8 = 0x48;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0e
	//------------------------------------------------------------
	adrs = 0x0e;
	data8 = 0x50;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb4
	//------------------------------------------------------------
	adrs = 0xb4;
	data8 = 0x94;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb6
	//------------------------------------------------------------
	adrs = 0xb6;
	data8 = 0x31;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xbc
	//------------------------------------------------------------
	adrs = 0xbc;
	data8 = 0x95;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xbe
	//------------------------------------------------------------
	adrs = 0xbe;
	data8 = 0x32;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x1b
	//============================================================
	id = 0x1b;

	//------------------------------------------------------------
	// Adrs = 0xe0
	//------------------------------------------------------------
	adrs = 0xe0;
	data8 = 0x27;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe2
	//------------------------------------------------------------
	adrs = 0xe2;
	data8 = 0x8d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe8
	//------------------------------------------------------------
	adrs = 0xe8;
	data8 = 0x27;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xea
	//------------------------------------------------------------
	adrs = 0xea;
	data8 = 0x41;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xeb
	//------------------------------------------------------------
	adrs = 0xeb;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf2
	//------------------------------------------------------------
	adrs = 0xf2;
	data8 = 0x36;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf8
	//------------------------------------------------------------
	adrs = 0xf8;
	data8 = 0x36;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x1c
	//============================================================
	id = 0x1c;

	//------------------------------------------------------------
	// Adrs = 0x04
	//------------------------------------------------------------
	adrs = 0x04;
	data8 = 0x2c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x06
	//------------------------------------------------------------
	adrs = 0x06;
	data8 = 0x27;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0c
	//------------------------------------------------------------
	adrs = 0x0c;
	data8 = 0x9c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x0e
	//------------------------------------------------------------
	adrs = 0x0e;
	data8 = 0x95;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x14
	//------------------------------------------------------------
	adrs = 0x14;
	data8 = 0x48;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x16
	//------------------------------------------------------------
	adrs = 0x16;
	data8 = 0x41;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x17
	//------------------------------------------------------------
	adrs = 0x17;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1c
	//------------------------------------------------------------
	adrs = 0x1c;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1d
	//------------------------------------------------------------
	adrs = 0x1d;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1e
	//------------------------------------------------------------
	adrs = 0x1e;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1f
	//------------------------------------------------------------
	adrs = 0x1f;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2c
	//------------------------------------------------------------
	adrs = 0x2c;
	data8 = 0x96;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2e
	//------------------------------------------------------------
	adrs = 0x2e;
	data8 = 0x2c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x34
	//------------------------------------------------------------
	adrs = 0x34;
	data8 = 0x2c;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x36
	//------------------------------------------------------------
	adrs = 0x36;
	data8 = 0x96;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x40
	//------------------------------------------------------------
	adrs = 0x40;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x42
	//------------------------------------------------------------
	adrs = 0x42;
	data8 = 0x8d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7e
	//------------------------------------------------------------
	adrs = 0x7e;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x82
	//------------------------------------------------------------
	adrs = 0x82;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x86
	//------------------------------------------------------------
	adrs = 0x86;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8a
	//------------------------------------------------------------
	adrs = 0x8a;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa4
	//------------------------------------------------------------
	adrs = 0xa4;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa5
	//------------------------------------------------------------
	adrs = 0xa5;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xa6
	//------------------------------------------------------------
	adrs = 0xa6;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xac
	//------------------------------------------------------------
	adrs = 0xac;
	data8 = 0xed;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xad
	//------------------------------------------------------------
	adrs = 0xad;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xae
	//------------------------------------------------------------
	adrs = 0xae;
	data8 = 0x7f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xaf
	//------------------------------------------------------------
	adrs = 0xaf;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb4
	//------------------------------------------------------------
	adrs = 0xb4;
	data8 = 0xed;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb5
	//------------------------------------------------------------
	adrs = 0xb5;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb6
	//------------------------------------------------------------
	adrs = 0xb6;
	data8 = 0x7f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xb7
	//------------------------------------------------------------
	adrs = 0xb7;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd1
	//------------------------------------------------------------
	adrs = 0xd1;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe4
	//------------------------------------------------------------
	adrs = 0xe4;
	data8 = 0x86;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe6
	//------------------------------------------------------------
	adrs = 0xe6;
	data8 = 0x7e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//  DRRS 隠しレジスタ(Bitで異なるレジスタ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bit					：bit
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorVendorSDrrsHideRegBit (int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char id;
	unsigned char data8;
	int regHold, acTmg;
	unsigned int adrs;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//============================================================
	// ID = 7
	//============================================================
	id = 0x07;

	//------------------------------------------------------------
	// Adrs = 0xc8
	//------------------------------------------------------------
	adrs = 0xc8;

	if (bit == 12)
		data8 = 0xc5;
	else
		data8 = 0xa7;

	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	
	//============================================================
	// ID = 8
	//============================================================
	id = 0x08;

	//------------------------------------------------------------
	// Adrs = 0x52
	//------------------------------------------------------------
	adrs = 0x52;

	if (bit == 12)
		data8 = 0x2a;
	else
		data8 = 0x3b;

	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//------------------------------------------------------------
	// Adrs = 0x66
	//------------------------------------------------------------
	adrs = 0x66;

	if (bit == 12)
		data8 = 0x2b;
	else
		data8 = 0x3c;

	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	DRRS Black Level設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorDrrsSetBlacklevel (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int black;
	int bit;
	int gcMode;

	// bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	if (bit == 8)
	{
		// Gradation Compress Mode取得
		gcMode = IN32 (FIRM_DATA_GRADATION_COMPRESS_MODE_PRE);

		// Gradation Compress Mode有効?
		if (gcMode == MODE_ENABLE)
			bit = GC_CAMERA_BIT;
	}
	#endif // #if defined (MODE_SENSOR_GRADATION_COMPRESS)

	// Black Level
	if (bit == 10)
		black = SENSOR_REG_BLACKLEVEL_10BIT << 2;
	else if (bit == 12)
		black = SENSOR_REG_BLACKLEVEL_12BIT << 0;
	else
		black = SENSOR_REG_BLACKLEVEL_8BIT  << 4;

	// Black Level設定
	OUT32 (FPGA_DRRS_SENSOR_BLACK_LEVEL_ADRS, black);
	OUT32 (FPGA_DRRS2_SENSOR_BLACK_LEVEL_ADRS, black);
	
_DONE:
	return (status);
}
#endif // #if defined (MODE_SENSOR_DRRS)


//**********************************************************************************
//	FPGA Shutter Normal/Trigger Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：MODE_DISABLE=Normal/MODE_ENABLE=Trigger
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetFpgaShutterMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int hvGenMode = MODE_DISABLE;
	int trgMode = MODE_DISABLE;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Set FPGA Shutter Mode(%d) Parameter Error.(Normal:%d / Trigger:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if !defined (MODE_SENSOR_MASTER)
	// Get Trg Mode
	if ((status = sensorGetTrgControl (&trgMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (trgMode == MODE_ENABLE)
	{
		if ((status = sensorSetTrgControl (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Get HV Gen Mode
	if ((status = sensorGetHvGen (&hvGenMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (hvGenMode == MODE_ENABLE)
	{
		if ((status = sensorSetHvGen (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if !defined (MODE_SENSOR_MASTER)


	data32 = IN32 (FPGA_SENSOR_MODE_ADRS);

	if (mode == MODE_ENABLE)
		data32 |= FPGA_SENSOR_MODE_TRIGGER;
	else
		data32 &= ~FPGA_SENSOR_MODE_TRIGGER;

	OUT32 (FPGA_SENSOR_MODE_ADRS, data32);

_DONE:
#if !defined (MODE_SENSOR_MASTER)

	if (hvGenMode == MODE_ENABLE)
		sensorSetHvGen (MODE_ENABLE);

	if (trgMode == MODE_ENABLE)
		sensorSetTrgControl (MODE_ENABLE);
#endif // #if !defined (MODE_SENSOR_MASTER)

	return (status);
}


//**********************************************************************************
//	FPGA Shutter Normal/Trigger Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：MODE_DISABLE=Normal/MODE_ENABLE=Triggereを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetFpgaShutterMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor GetFPGA  Shutter Mode NULL Parameter Error.\n");
		goto _DONE;
	}

	data32 = IN32 (FPGA_SENSOR_MODE_ADRS);

	if (data32 & FPGA_SENSOR_MODE_TRIGGER)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	センサConversionGain設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=LGCモード/1=HGCモード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetConversionGain (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;
	int startMode = 0;
	int drrsMode = MODE_DISABLE;

	// Check mode Parameter
	if ((mode != MODE_DISABLE) && (mode != MODE_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Set Conversion Gain(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	// Chip ID = 0x07
	//============================================================
	id = 0x07;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Conversion Gain取得
	if ((status = sensorRegReadByte (id, SENSOR_REG_CONVERSION_GAIN_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == MODE_DISABLE)
		data8 &= ~SENSOR_REG_CONVERSION_GAIN_LHGC;
	else
		data8 |= SENSOR_REG_CONVERSION_GAIN_LHGC;

	// Conversion Gain設定
	if ((status = sensorRegWriteByte (id, SENSOR_REG_CONVERSION_GAIN_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DDRに設定値を保存
	OUT32 (FIRM_DATA_SENSOR_CONVERSION_GAIN_ADRS, mode);

	
	//============================================================
	// Chip ID = 0x13
	//============================================================
#if defined (MODE_SENSOR_DRRS)
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	if (drrsMode == MODE_ENABLE)
	{
		id = 0x13;
		regHold = 0;	// レジスタホールドなし
		acTmg = 0;		// 即時アクセス

		// Conversion Gain Switching1取得
		if ((status = sensorRegReadByte (id, SENSOR_REG_CONVERSION_GAIN_SWITCHING1_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (mode == MODE_DISABLE)
			data8 &= ~SENSOR_REG_CONVERSION_GAIN_SWITCHING1_HCG;
		else
			data8 |= SENSOR_REG_CONVERSION_GAIN_SWITCHING1_HCG;

		// Conversion Gain Switching1設定
		if ((status = sensorRegWriteByte (id, SENSOR_REG_CONVERSION_GAIN_SWITCHING1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Conversion Gain Switching2取得
		if ((status = sensorRegReadByte (id, SENSOR_REG_CONVERSION_GAIN_SWITCHING2_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (mode == MODE_DISABLE)
			data8 |= SENSOR_REG_CONVERSION_GAIN_SWITCHING2_LCG;
		else
			data8 &= ~SENSOR_REG_CONVERSION_GAIN_SWITCHING2_LCG;

		// Conversion Gain Switching2設定
		if ((status = sensorRegWriteByte (id, SENSOR_REG_CONVERSION_GAIN_SWITCHING2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	センサConversionGain取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Conversion Gainモードを格納するポインタ(0=LGCモード/1=HGCモード)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetConversionGain (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Get Conversion Gain NULL Parameter Error.\n");
		goto _DONE;
	}

	// DDRから取得
	*pMode = IN32 (FIRM_DATA_SENSOR_CONVERSION_GAIN_ADRS);

_DONE:
	return (status);
}
#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)


//**********************************************************************************
//	Sensor Register Hold
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		regHold				：Reg Hold(0=Not Hold/1=Hold)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetRegHold (int regHold)
{
	int status = AVAL_STATUS_SUCCESS;
	int acTmg;
	unsigned char data;

	if (regHold == 0)
		data = 0;
	else
		data = SENSOR_REG_REGHOLD_BIT;

	acTmg = 1;	 	// Reflection Timing 'I'
	regHold = 0; 	// この関数内では0

	// レジスタアクセス
	if ((status = sensorRegWriteByte (2, SENSOR_REG_REGHOLD_ADRS, data, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	センサレジスタ書き込み(1Byte)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		id					：chip ID
//		adrs				：アドレス
//		data				：書き込みデータ
//		acTmg				：Access Timing EN(0=Disable/1=Enable)
//		regHold				：Reg Hold(0=Not Hold/1=Hold)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorRegWriteByte (unsigned char id, unsigned char adrs, unsigned char data, int acTmg, int regHold)
{
	int status = AVAL_STATUS_SUCCESS;

	// レジスタホールド設定
	if (regHold != 0)
	{
		if ((status = sensorSetRegHold (1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Sensor Write
	if ((status = sensorRegWriteBase (&id, &adrs, &data, 1, acTmg)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	// レジスタホールド解除
	if (regHold != 0)
		sensorSetRegHold (0);

	return (status);
}


//**********************************************************************************
//	センサレジスタ読み込み(1Byte)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		id					：chip ID
//		adrs				：アドレス
//		pData				：読み込みデータを格納するポインタ
//		acTmg				：Access Timing EN(0=Disable/1=Enable)
//		regHold				：Reg Hold(0=Not Hold/1=Hold)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorRegReadByte (unsigned char id, unsigned char adrs, unsigned char *pData, int acTmg, int regHold)
{
	int status = AVAL_STATUS_SUCCESS;

	// レジスタホールド設定
	if (regHold != 0)
	{
		if ((status = sensorSetRegHold (1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Sensor Read
	if ((status = sensorRegReadBase (&id, &adrs, pData, 1, acTmg)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	// レジスタホールド解除
	if (regHold != 0)
		sensorSetRegHold (0);

	return (status);
}


//**********************************************************************************
//	センサレジスタ書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		id					：chip IDデータを格納するポインタ
//		adrs				：アドレスデータを格納するポインタ
//		pData				：読み込みデータを格納するポインタ
//		サイズ				：サイズ
//		acTmg				：Access Timing EN(0=Disable/1=Enable)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorRegWriteBase (unsigned char *pId, unsigned char *pAdrs, unsigned char *pData, int size, int acTmg)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char id, adrs, data8;
	unsigned int data32;
	int i;
	unsigned char *ptr8;

	// Check pId Parameter
	if (pId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Register Write pId NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Register Write pAdrs NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check size Parameter
	if ((size < 0) || (size > FPGA_SENSOR_REG_SIZE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Register Write size(0x%x) Parameter Error.(Min:0 / Max:%d)\n", size, FPGA_SENSOR_REG_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Register Write pData NULL Parameter Error.\n");
		goto _DONE;
	}

	//------------------------------------------------------------
	// ID & アドレス & データ書き込み
	//------------------------------------------------------------
	ptr8 = pData;
	for (i=0; i<size; i++, pId++, pAdrs++, ptr8++)
	{
		// IDデータ
		id = *pId;

		// アドレスデータ
		adrs = *pAdrs;

		// データ
		data8 = *ptr8;

		// Check id Parameter
		if ((id < 0) || (id > FPGA_SENSOR_REG_ID_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Sensor Register Write id(%d) Parameter Error.(Min:0 / Max:%d). Index=%d\n", id, FPGA_SENSOR_REG_ID_MAX, i);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Check adrs Parameter
		if ((adrs < 0) || (adrs > FPGA_SENSOR_MAX_ADRS))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Sensor Register Write adrs(0x%x) Parameter Error.(Min:0 / Max:%d). Index=%d\n", adrs, FPGA_SENSOR_MAX_ADRS, i);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// 設定
		data32 = FPGA_SENSOR_REG_TBL_WRITE_DATA_MAKE(data8) | FPGA_SENSOR_REG_TBL_ADRS_MAKE(adrs) | FPGA_SENSOR_REG_TBL_WRITE_BIT | FPGA_SENSOR_REG_TBL_CHIPID_MAKE(id);
		OUT32 (FPGA_SENSOR_REG_TBL_ADRS, data32);
	}

	// End Clear
	OUT32 (FPGA_SENSOR_REG_CTRL_ADRS, FPGA_SENSOR_REG_CTRL_END_BIT);

	// Start
	data32 = FPGA_SENSOR_REG_CTRL_START_BIT | FPGA_SENSOR_REG_CTRL_ACES_COUNT_SET(size);
	if (acTmg != 0)
		data32 |= FPGA_SENSOR_REG_CTRL_ACES_TMG_ENABLE_BIT;

	OUT32 (FPGA_SENSOR_REG_CTRL_ADRS, data32);

	// Check End
	for (i=0; i<SENSOR_REG_ACES_TIMEOUT; i++)
	{
		data32 = IN32 (FPGA_SENSOR_REG_CTRL_ADRS);
		if ((data32 & FPGA_SENSOR_REG_CTRL_END_BIT) == FPGA_SENSOR_REG_CTRL_END_BIT)
			break;

		usDelay (2);
	}

	// Check Timeout
	if (i >= SENSOR_REG_ACES_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Write Timeout Error.\n");
		goto _DONE;
	}

_DONE:

	return (status);
}


//**********************************************************************************
//	センサレジスタ読み込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		id					：chip IDデータを格納するポインタ
//		adrs				：アドレスデータを格納するポインタ
//		pData				：読み込みデータを格納するポインタ
//		サイズ				：サイズ
//		acTmg				：Access Timing EN(0=Disable/1=Enable)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorRegReadBase (unsigned char *pId, unsigned char *pAdrs, unsigned char *pData, int size, int acTmg)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char id, adrs, data8;
	unsigned int data32;
	unsigned char *ptr8;
	int i;

	// Check pId Parameter
	if (pId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Register Read pId NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pAdrs Parameter
	if (pAdrs == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Register Read pAdrs NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check size Parameter
	if ((size < 0) || (size > FPGA_SENSOR_REG_SIZE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Register Read size(0x%x) Parameter Error.(Min:0 / Max:%d)\n", size, FPGA_SENSOR_REG_SIZE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Register Read pData NULL Parameter Error.\n");
		goto _DONE;
	}

	//------------------------------------------------------------
	// ID & アドレス読み込み
	//------------------------------------------------------------
	for (i=0; i<size; i++, pId++, pAdrs++)
	{
		// IDデータ
		id = *pId;

		// アドレスデータ
		adrs = *pAdrs;

		// Check id Parameter
		if ((id < 0) || (id > FPGA_SENSOR_REG_ID_MAX))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Sensor Register Read id(%d) Parameter Error.(Min:0 / Max:%d). Index=%d\n", id, FPGA_SENSOR_REG_ID_MAX, i);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Check adrs Parameter
		if ((adrs < 0) || (adrs > FPGA_SENSOR_MAX_ADRS))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Sensor Register Read adrs(0x%x) Parameter Error.(Min:0 / Max:%d). Index=%d\n", adrs, FPGA_SENSOR_MAX_ADRS, i);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// 設定
		data32 = FPGA_SENSOR_REG_TBL_ADRS_MAKE(adrs) | FPGA_SENSOR_REG_TBL_READ_BIT | FPGA_SENSOR_REG_TBL_CHIPID_MAKE(id);
		OUT32 (FPGA_SENSOR_REG_TBL_ADRS, data32);
	}

	// End Clear
	OUT32 (FPGA_SENSOR_REG_CTRL_ADRS, FPGA_SENSOR_REG_CTRL_END_BIT);

	// Start
	data32 = FPGA_SENSOR_REG_CTRL_START_BIT | FPGA_SENSOR_REG_CTRL_ACES_COUNT_SET(size);
	if (acTmg != 0)
		data32 |= FPGA_SENSOR_REG_CTRL_ACES_TMG_ENABLE_BIT;

	OUT32 (FPGA_SENSOR_REG_CTRL_ADRS, data32);

	// Check End
	for (i=0; i<SENSOR_REG_ACES_TIMEOUT; i++)
	{
		data32 = IN32 (FPGA_SENSOR_REG_CTRL_ADRS);
		if ((data32 & FPGA_SENSOR_REG_CTRL_END_BIT) == FPGA_SENSOR_REG_CTRL_END_BIT)
			break;

		usDelay (2);
	}

	// Check Timeout
	if (i >= SENSOR_REG_ACES_TIMEOUT)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Read Timeout Error.\n");
		goto _DONE;
	}

	//------------------------------------------------------------
	// データ取得
	//------------------------------------------------------------
	ptr8 = pData;
	for (i=0; i<size; i++, ptr8++)
	{
		data32 = IN32 (FPGA_SENSOR_REG_TBL_ADRS);
		data8 = FPGA_SENSOR_REG_TBL_READ_DATA_MAKE(data32);
		*ptr8 = data8;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Black Pixel
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetBlackPixel (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int acTmg, regHold;
	unsigned char adrs, data1, data2;
	int id;
	int startMode = 0;

	// Check data Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Black Pixel mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード移行
	//if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	if (mode == MODE_DISABLE)
	{
		data1 = 0x5f;
		data2 = 0x00;
	}
	else
	{
		data1 = 0x50;
		data2 = 0x01;
	}

	// StandByモード移行
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// センサ設定関連
	id = 0x07;
	acTmg = 0;
	regHold = 0;

	// BlackPixel1
	adrs = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data1, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// BlackPixel2
	adrs = 0x40;
	if ((status = sensorRegWriteByte (id, adrs, data2, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード解除
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// モード設定
	OUT32 (FIRM_DATA_BLACKPIXEL_MODE_ADRS, mode);

	usDelay(1000);

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	Black Pixel
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Black Pixel Modeを格納するポインタ(0=無効/1=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetBlackPixel (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Black Pixel Get pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	*pMode = IN32 (FIRM_DATA_BLACKPIXEL_MODE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// Sensor Gain設定(dB)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		gain				：ゲイン(dB)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetGainDB (float gain)
{
	int status = AVAL_STATUS_SUCCESS;
	int id;
	int acTmg, regHold;
	unsigned int data32;
	unsigned char data8;

	// Check gain Parameter
	if ((gain < SENSOR_REG_GAIN_DB_MIN) || (gain > SENSOR_REG_GAIN_DB_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Gain dB(%f) Parameter Error.(Min:%.1f / Max:%.1f)\n", gain, SENSOR_REG_GAIN_DB_MIN, SENSOR_REG_GAIN_DB_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// レジスタの単位に合わせる
	data32 = (unsigned int)(gain * SENSOR_REG_GAIN_DB_UNIT);

	// センサ設定関連
	id = 0x07;
	acTmg = 1;
	regHold = 0;

	// Gain Low
	data8 = data32 & SENSOR_REG_GAIN_L_MASK;
	if ((status = sensorRegWriteByte (id, SENSOR_REG_GAIN_L_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Gain High
	data8 = (data32 >> SENSOR_REG_GAIN_H_SHIFT) & SENSOR_REG_GAIN_H_MASK;
	if ((status = sensorRegWriteByte (id, SENSOR_REG_GAIN_H_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// Sensor Gain取得(dB)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pGain				：ゲインを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetGainDB (float *pGain)
{
	int status = AVAL_STATUS_SUCCESS;
	int id;
	int acTmg, regHold;
	unsigned char data8;
	unsigned int data32;

	// Check pGain Parameter
	if (pGain == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Gain pGain NULL Parameter Error.\n");
		goto _DONE;
	}

	// センサ設定関連
	id = 0x07;
	acTmg = 1;
	regHold = 0;

	// Gain Low
	if ((status = sensorRegReadByte (id, SENSOR_REG_GAIN_L_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data32 = (unsigned int)data8 & SENSOR_REG_GAIN_L_MASK;

	// Gain High
	if ((status = sensorRegReadByte (id, SENSOR_REG_GAIN_H_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data32 |= ((data8 & SENSOR_REG_GAIN_H_MASK) << SENSOR_REG_GAIN_H_SHIFT);

	// データ格納
	*pGain = (float)((float)data32 / (float)SENSOR_REG_GAIN_DB_UNIT);

_DONE:
	return (status);
}


//**********************************************************************************
// Sensor Gain設定(倍率)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		gain				：ゲイン(倍率)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetGainX (float gain)
{
	int status = AVAL_STATUS_SUCCESS;
	int data;
	float gainF, gainDB;

	// 小数点第1位まで有効
	data = gain * SENSOR_REG_GAIN_VALID_POINT;
	gainF = (float)((float)data / (float)SENSOR_REG_GAIN_VALID_POINT);

	// Check gain Parameter
	if ((gainF < SENSOR_REG_GAIN_X_MIN) || (gainF > SENSOR_REG_GAIN_X_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Digital Gain(%lf) Parameter Error.(Min:%.1f / Max:%.1f)\n", gainF, SENSOR_REG_GAIN_X_MIN, SENSOR_REG_GAIN_X_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 倍率 => Decibel変換
	gainDB = 20 * log10(gainF);
	gainDB += 0.05;		// 四捨五入

	// Gain設定
	if ((status = sensorSetGainDB (gainDB)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 取得した値をメモリに格納
	OUT32 (FIRM_DATA_SENSOR_GAIN_ADRS, (gain * SENSOR_GAIN_UNIT));

_DONE:
	return (status);
}


//**********************************************************************************
// Sensor Gain取得(倍率)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pGain				：ゲインを格納するポインタ(倍率)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetGainX (float *pGain)
{
	int status = AVAL_STATUS_SUCCESS;
	float gainDB, gainX;

	// Check pGain Parameter
	if (pGain == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Gain pGain NULL Parameter Error.\n");
		goto _DONE;
	}

	// Gain取得
	if ((status = sensorGetGainDB (&gainDB)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DB=>倍率
	gainX = pow(10, gainDB/20);
	gainX += 0.05;		// 四捨五入
	 *pGain = gainX;

_DONE:
	return (status);
}


//**********************************************************************************
// Sensor X Flip設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Flip Mode
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetXFlip (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char data8;
	int regHold, acTmg;
	unsigned char id;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Xflip Mode(%d) Parameter Error.(Reverse Disable:%d / Reverse Enable:%d)\n", mode, MODE_ENABLE, MODE_DISABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// レジスタホールドなし
	regHold = 0;

	// 即時アクセス
	acTmg = 0;

	// センサ設定
	id = 0x04;
	if ((status = sensorRegReadByte (id, SENSOR_REG_REVERSE_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == MODE_ENABLE)
		data8 |= SENSOR_REG_REVERSE_H;
	else
		data8 &= ~SENSOR_REG_REVERSE_H;

	if ((status = sensorRegWriteByte (id, SENSOR_REG_REVERSE_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// テストパターン設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：テストパターンモード
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorTpSetMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;
	unsigned short black;
	unsigned int gcMode;
	int bit;
	int startMode = 0;

	// Check mode Parameter
	if ((mode < SENSOR_REG_PG_MODE_MIN) || (mode > SENSOR_REG_PG_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Test Pattern(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, SENSOR_REG_PG_MODE_MIN, SENSOR_REG_PG_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード移行
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Test Pattern取得
	if ((status = sensorRegReadByte (id, SENSOR_REG_PG_MODE_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Maskクリア
	data8 &= ~SENSOR_REG_PG_MODE_MASK;

	// 無効時は初期値に戻す
	if (mode == 0)
		data8 |= (unsigned char)SENSOR_REG_PG_MODE_MASK;
	else
		data8 |= (unsigned char)mode;

	// Test Pattern設定
	if ((status = sensorRegWriteByte (id, SENSOR_REG_PG_MODE_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// PG取得
	if ((status = sensorRegReadByte (id, SENSOR_REG_PG_CTRL_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == 0)
	{
		data8 &= ~SENSOR_REG_PG_CTRL_MODE;
		data8 |= SENSOR_REG_PG_THRU_MODE;
		data8 &= ~SENSOR_REG_PG_THRU_MODE;
	}
	else
	{
		data8 |= SENSOR_REG_PG_CTRL_MODE;
		data8 &= ~SENSOR_REG_PG_THRU_MODE;
		data8 |= SENSOR_REG_PG_CLK_MODE;
	}

	// PG設定
	if ((status = sensorRegWriteByte (id, SENSOR_REG_PG_CTRL_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == 0)
	{
		#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		// Default
		data8 = 0x50;
		if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_FIX1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Default
		data8 = 0x01;
		if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_FIX2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		#endif //#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	}
	else
	{
		#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		// Fix 0x5F
		data8 = 0x5F;
		if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_FIX1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Fix 0x00
		data8 = 0x00;
		if ((gSensorTmgStatus = sensorRegWriteByte (id, SENSOR_REG_FIX2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		#endif //#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	}

	if (mode == 0)
	{
		if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		if (bit == 8)
			black = SENSOR_REG_BLACKLEVEL_8BIT;
		else if (bit == 10)
			black = SENSOR_REG_BLACKLEVEL_10BIT;
		else
			black = SENSOR_REG_BLACKLEVEL_12BIT;
		
		#if defined (MODE_SENSOR_GRADATION_COMPRESS)
		gcMode = sensorGradationCompGetModeDDR2();
		// Gradation Compress Mode有効?
		if (gcMode == MODE_ENABLE)
			bit = GC_CAMERA_BIT;
		#endif
	}
	else
	{
		black = 0;
	}
	
	// Sensor Black1
	id = 0x07;
	data8 = (unsigned char)(black & SENSOR_REG_BLACKLEVEL1_MASK);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_BLACKLEVEL1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor Black2
	id = 0x07;
	data8 = (unsigned char)((black >> 8) & SENSOR_REG_BLACKLEVEL2_MASK);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_BLACKLEVEL2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// PGHGSTEP
	id = 0x07;
	data8 = 4;	// 4step
	if ((status = sensorRegWriteByte (id, SENSOR_REG_PG_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード解除
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	msDelay (200);
#endif
	
_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// テストパターン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：テストパターンを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorTpGetMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;
	int startMode = 0;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Test Pattern Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード移行
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Test Pattern取得
	if ((status = sensorRegReadByte (id, SENSOR_REG_PG_MODE_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Test Pattern設定
	data8 &= SENSOR_REG_PG_MODE_MASK;
	if (data8 == SENSOR_REG_PG_MODE_MASK)
		*pMode = 0;
	else
		*pMode = data8;

	// StandByモード解除
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// テストパターンインクリメント設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		wData				：水平インクリメント
//		hData				：垂直インクリメント
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorTpSetInc (unsigned char hInc, unsigned char vInc)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	int startMode = 0;

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード移行
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// 水平インクリメント取得
	if ((status = sensorRegWriteByte (id, SENSOR_REG_PG_HPSTEP_ADRS, hInc, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 垂直インクリメント取得
	if ((status = sensorRegWriteByte (id, SENSOR_REG_PG_VPSTEP_ADRS, vInc, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// StandByモード解除
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	msDelay (200);
#endif

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// テストパターンインクリメント取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pHInc				：水平インクリメントを格納するポインタ
//		pVInc				：垂直インクリメントを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorTpGetInc (unsigned char *pHInc, unsigned char *pVInc)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;
	int startMode = 0;

	// Check pHInc Parameter
	if (pHInc == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Test Pattern Increment pHInc NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pVInc Parameter
	if (pHInc == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Test Pattern Increment pVInc NULL Parameter Error.\n");
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード移行
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// 水平インクリメント取得
	if ((status = sensorRegReadByte (id, SENSOR_REG_PG_HPSTEP_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	*pHInc = data8;

	// 垂直インクリメント取得
	if ((status = sensorRegReadByte (id, SENSOR_REG_PG_VPSTEP_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	*pVInc = data8;

	// StandByモード解除
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// テストパターンデータ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		data1				：データ1
//		data2				：データ2
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorTpSetData (unsigned int data1, unsigned int data2)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	int startMode = 0;
	unsigned char data8;

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード移行
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Data1 Low
	data8 = (unsigned char)data1;
	if ((status = sensorRegWriteByte (id, SENSOR_REG_PG_DATA1_L_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Data1 High
	data8 = (unsigned char)((data1 & SENSOR_REG_PG_DATA1_MASK)>>8);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_PG_DATA1_H_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Data2 Low
	data8 = (unsigned char)data2;
	if ((status = sensorRegWriteByte (id, SENSOR_REG_PG_DATA2_L_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Data2 High
	data8 = (unsigned char)((data2 & SENSOR_REG_PG_DATA2_MASK)>>8);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_PG_DATA2_H_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	// StandByモード解除
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	msDelay (200);
#endif

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// テストパターンデータ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData1				：データ1を格納するポインタ
//		pData2				：データ2を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorTpGetData (unsigned int *pData1, unsigned int *pData2)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;
	int startMode = 0;

	// Check pData1 Parameter
	if (pData1 == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Test Pattern Data pData1 NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pData2 Parameter
	if (pData2 == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Test Pattern Data pDat2 NULL Parameter Error.\n");
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Data1 Low
	if ((status = sensorRegReadByte (id, SENSOR_REG_PG_DATA1_L_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	*pData1 = data8;

	// Data1 High
	if ((status = sensorRegReadByte (id, SENSOR_REG_PG_DATA1_H_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	*pData1 |= ((unsigned int)data8<<8);

	// Data2 Low
	if ((status = sensorRegReadByte (id, SENSOR_REG_PG_DATA2_L_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	*pData2 = data8;

	// Data2 High
	if ((status = sensorRegReadByte (id, SENSOR_REG_PG_DATA2_H_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	*pData2 |= ((unsigned int)data8<<8);

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	センサADレジスタ初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetAD (int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8, temp8;
	int mode;

	//------------------------------------------------------------
	// Check Fradiation Compress
	//------------------------------------------------------------
	//mode  = IN32 (FIRM_DATA_GRADATION_COMPRESS_MODE);
	mode = sensorGradationCompGetModeDDR();
	if (mode == MODE_ENABLE)
		bit = 10;


	//============================================================
	// Chip ID = 0x04
	//============================================================
	id = 0x04;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//------------------------------------------------------------
	// AD
	//------------------------------------------------------------
	if (bit == 10)
		temp8 = SENSOR_REG_AD_10BIT;
	else if (bit == 12)
		temp8 = SENSOR_REG_AD_12BIT;
	else
		temp8 = SENSOR_REG_AD_8BIT;

	if ((status = sensorRegReadByte (id, SENSOR_REG_AD_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 &= ~SENSOR_REG_AD_MASK;
	data8 |= temp8;

	if ((status = sensorRegWriteByte (id, SENSOR_REG_AD_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	センサOptical Blackレジスタ初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetOB (int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8, temp8;

	//============================================================
	// Chip ID = 0x06
	//============================================================
	id = 0x06;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//------------------------------------------------------------
	// OD Bit
	//------------------------------------------------------------
	if (bit == 10)
		temp8 = SENSOR_REG_OD_10BIT;
	else if (bit == 12)
		temp8 = SENSOR_REG_OD_12BIT;
	else
		temp8 = SENSOR_REG_OD_8BIT;

	if ((status = sensorRegReadByte (id, SENSOR_REG_ODBIT_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 &= ~SENSOR_REG_OD_BIT_MASK;
	data8 |= temp8;

	if ((status = sensorRegWriteByte (id, SENSOR_REG_ODBIT_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	センサBlack Levelレジスタ初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetBL (int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;
	unsigned short black;
	int mode;

	//------------------------------------------------------------
	// Check Fradiation Compress
	//------------------------------------------------------------
	//mode  = IN32 (FIRM_DATA_GRADATION_COMPRESS_MODE);
	mode = sensorGradationCompGetModeDDR();
	if (mode == MODE_ENABLE)
		bit = 10;


	//============================================================
	// Chip ID = 0x07
	//============================================================
	id = 0x07;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//------------------------------------------------------------
	// Black Level
	//------------------------------------------------------------
	if (bit == 10)
		black = SENSOR_REG_BLACKLEVEL_10BIT;
	else if (bit == 12)
		black = SENSOR_REG_BLACKLEVEL_12BIT;
	else
		black = SENSOR_REG_BLACKLEVEL_8BIT;

	// Sensor Black1
	data8 = (unsigned char)(black & SENSOR_REG_BLACKLEVEL1_MASK);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_BLACKLEVEL1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Sensor Black2
	data8 = (unsigned char)((black >> 8) & SENSOR_REG_BLACKLEVEL2_MASK);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_BLACKLEVEL2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


#if defined (MODE_SENSOR_GRADATION_COMPRESS)
//**********************************************************************************
// 階調圧縮機能初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int mode;
	int first, second;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;
	unsigned short black;
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	int shutterMode;
#endif

	//--------------------------------------------------------------------------------
	// Register Data Restore
	//--------------------------------------------------------------------------------
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_GRADATION_COMPRESS_REG_ADRS, CAMERA_GRADATION_COMPRESS_REG_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gSensorStatus = status;
		goto _DONE;
	}

	//--------------------------------------------------------------------------------
	// Position
	//--------------------------------------------------------------------------------
	first  = IN32 (FIRM_DATA_GRADATION_COMPRESS_POS_FIRST);
	second = IN32 (FIRM_DATA_GRADATION_COMPRESS_POS_SECOND);

	if ((status = sensorGradationCompSetPositionReg (first, second)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Gain
	//--------------------------------------------------------------------------------
	first  = IN32 (FIRM_DATA_GRADATION_COMPRESS_GAIN_FIRST);
	second = IN32 (FIRM_DATA_GRADATION_COMPRESS_GAIN_SECOND);

	if ((status = sensorGradationCompSetGainReg (first, second)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Mode
	//--------------------------------------------------------------------------------
	OUT32 (FIRM_DATA_PIXEL_DYNAMIC_RANGE_MAX, 0);

	//mode  = IN32 (FIRM_DATA_GRADATION_COMPRESS_MODE);
	mode = sensorGradationCompGetModeDDR();
	
	// Pre Version Set
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_MODE_PRE, mode);

	if ((status = sensorGradationCompSetModeReg (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == MODE_ENABLE)
		OUT32 (FIRM_DATA_PIXEL_DYNAMIC_RANGE_MAX, FIRM_DATA_PIXEL_DYNAMIC_GC_ENABLE);
	else
		OUT32 (FIRM_DATA_PIXEL_DYNAMIC_RANGE_MAX, FIRM_DATA_PIXEL_DYNAMIC_GC_DISABLE);


	if (mode == MODE_ENABLE)
	{
		regHold = 0;	// レジスタホールドなし
		acTmg = 0;		// 即時アクセス

		//============================================================
		// Chip ID = 0x04
		//============================================================
		id = 0x04;

		//------------------------------------------------------------
		// AD
		//------------------------------------------------------------
		if ((status = sensorRegReadByte (id, SENSOR_REG_AD_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		data8 &= ~SENSOR_REG_AD_MASK;
		data8 |= GC_SENSOR_AD_BIT;

		if ((status = sensorRegWriteByte (id, SENSOR_REG_AD_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;


		//------------------------------------------------------------
		// Get Shutter Mode
		//------------------------------------------------------------
		if ((status = sensorGetShutterMode (&shutterMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (shutterMode == SHUTTER_MODE_TRIGGER)
		{
			//------------------------------------------------------------
			// H Interval
			//------------------------------------------------------------
			if ((status = sensorSetHInterval (GC_CAMERA_BIT)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}


		//============================================================
		// Chip ID = 0x07
		//============================================================
		id = 0x07;
		black = SENSOR_REG_BLACKLEVEL_10BIT;

		// Sensor Black1
		id = 0x07;
		data8 = (unsigned char)(black & SENSOR_REG_BLACKLEVEL1_MASK);
		if ((status = sensorRegWriteByte (id, SENSOR_REG_BLACKLEVEL1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Sensor Black2
		id = 0x07;
		data8 = (unsigned char)((black >> 8) & SENSOR_REG_BLACKLEVEL2_MASK);
		if ((status = sensorRegWriteByte (id, SENSOR_REG_BLACKLEVEL2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//============================================================
	// Gradiation Compress
	//============================================================

	// Gradiation Compress Position
	if ((status = sensorGradationCompSetPositionReg (1, 8)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Gradiation Compress Gain
	if ((status = sensorGradationCompSetGainReg (2, 2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// 階調圧縮 有効／無効設定(DDR設定のみ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompSetModeDDR (int mode)
{
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_MODE, mode);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
// 階調圧縮 有効／無効取得(DDR取得のみ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		取得値
//==================================================================================
int sensorGradationCompGetModeDDR (void)
{
	unsigned int data;
	data = IN32 (FIRM_DATA_GRADATION_COMPRESS_MODE);
	return ((int)data);
}


//**********************************************************************************
// 階調圧縮 有効／無効設定(DDR設定のみ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompSetModeDDR2 (int mode)
{
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_MODE2, mode);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
// 階調圧縮 有効／無効取得(DDR取得のみ)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		取得値
//==================================================================================
int sensorGradationCompGetModeDDR2 (void)
{
	unsigned int data;
	data = IN32 (FIRM_DATA_GRADATION_COMPRESS_MODE2);
	return ((int)data);
}


//**********************************************************************************
// 階調圧縮 有効／無効設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompSetMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int bit;
	int saveMode = -1;

	// Check mode Parameter
	if ((mode != MODE_DISABLE) && (mode != MODE_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Gradation Compress Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Pre Set領域
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_MODE_PRE, mode);

#if defined (MODE_BIT8_GCMODE_ENABLE)
	// 8bit時に階調圧縮モードをデフォルト有効にするかのフラグ
	OUT32 (FIRM_DATA_BIT8_CONVERT_MODE, mode);
#endif

	if (mode == MODE_ENABLE)
	{
		// bit設定
		if ((status = irSetBitWidth (0)) != AVAL_STATUS_SUCCESS)
			goto _DONE_RESET;
	}
	else
	{
		// bit取得
		if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE_RESET;

		// bit設定
		if ((status = irSetBitWidth (bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE_RESET;
	}

_DONE_RESET:
	// エラーの為、元に戻す
	if (status != AVAL_STATUS_SUCCESS)
		sensorGradationCompSetModeMain (saveMode);

_DONE:
	return (status);
}


//**********************************************************************************
// 階調圧縮 有効／無効 Main設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompSetModeMain (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Check mode Parameter
	if ((mode != MODE_DISABLE) && (mode != MODE_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Gradation Compress Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード移行
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// レジスタ設定
	if ((status = sensorGradationCompSetModeMain2 (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード解除
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// 階調圧縮 有効／無効 Main設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompSetModeMain2 (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check mode Parameter
	if ((mode != MODE_DISABLE) && (mode != MODE_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Gradation Compress Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// レジスタ設定
	if ((status = sensorGradationCompSetModeReg (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DDR管理外に保存
	sensorGradationCompSetModeDDR (mode);

	if (mode == MODE_ENABLE)
		OUT32 (FIRM_DATA_PIXEL_DYNAMIC_RANGE_MAX, FIRM_DATA_PIXEL_DYNAMIC_GC_ENABLE);
	else
		OUT32 (FIRM_DATA_PIXEL_DYNAMIC_RANGE_MAX, FIRM_DATA_PIXEL_DYNAMIC_GC_DISABLE);

_DONE:
	return (status);
}


//**********************************************************************************
// 階調圧縮 有効／無効 Register設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompSetModeReg (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Gradation Compress Mode取得
	if ((status = sensorRegReadByte (id, SENSOR_REG_GRAD_COMP_MODE_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == 0)
		data8 &= ~SENSOR_REG_GRAD_COMP_MODE_ENABLE;
	else
		data8 |= SENSOR_REG_GRAD_COMP_MODE_ENABLE;

	// Gradation Compress Mode設定
	if ((status = sensorRegWriteByte (id, SENSOR_REG_GRAD_COMP_MODE_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DDRに設定
	sensorGradationCompSetModeDDR2(mode);

_DONE:
	return (status);
}


//**********************************************************************************
// 階調圧縮 有効／無効取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：0=無効/1=有効を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompGetMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Gradation Compress Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Gradation Compress Mode取得
	if ((status = sensorRegReadByte (id, SENSOR_REG_GRAD_COMP_MODE_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (data8 & SENSOR_REG_GRAD_COMP_MODE_ENABLE)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
// 階調圧縮 Position設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		firstPos			：１段目の位置
//		secondPos			：２段目の位置
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompSetPosition (int first, int second)
{
	int status = AVAL_STATUS_SUCCESS;
	//int startMode = 0;

	// Check first Parameter
	if ((first < SENSOR_REG_GRAD_COMP_POS_FIRST_MIN) || (first > SENSOR_REG_GRAD_COMP_POS_FIRST_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Gradation Compress First Position(%d) Parameter Error.(Min:%d / Max:%d)\n", first, SENSOR_REG_GRAD_COMP_POS_FIRST_MIN, SENSOR_REG_GRAD_COMP_POS_FIRST_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check second Parameter
	if ((second < SENSOR_REG_GRAD_COMP_POS_SECOND_MIN) || (second > SENSOR_REG_GRAD_COMP_POS_SECOND_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Gradation Compress Second Position(%d) Parameter Error.(Min:%d / Max:%d)\n", second, SENSOR_REG_GRAD_COMP_POS_SECOND_MIN, SENSOR_REG_GRAD_COMP_POS_SECOND_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check first > second Parameter
	if (first > second)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Gradation Compress First(%d) > Second(%d) Position Parameter Error.\n", first, second);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check second Parameter
	if (second == SENSOR_REG_GRAD_COMP_POS_SECOND_POS1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Gradation Compress Second Position(%d) Not Support Parameter Error.\n", second);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	//acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	//if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// StandByモード移行
	//if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// センサレジスタ設定
	if ((status = sensorGradationCompSetPositionReg (first, second)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード解除
	//if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// DDRへ設定
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_POS_FIRST, first);
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_POS_SECOND, second);

_DONE:
	//if (startMode != 0)
		//acquisitionStart ();

	return (status);
}


//**********************************************************************************
// 階調圧縮 Position Register設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		firstPos			：１段目の位置
//		secondPos			：２段目の位置
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompSetPositionReg (int first, int second)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// First Position
	data8 = (first & SENSOR_REG_GRAD_COMP_POS_FIRST_MASK) << SENSOR_REG_GRAD_COMP_POS_FIRST_SHIFT;

	// Second Position
	data8 |= ((second & SENSOR_REG_GRAD_COMP_POS_SECOND_MASK) << SENSOR_REG_GRAD_COMP_POS_SECOND_SHIFT);

	// Gradation Compress Position設定
	if ((status = sensorRegWriteByte (id, SENSOR_REG_GRAD_COMP_POS_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// 階調圧縮 Position取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFirst			：１段目の位置を格納するポインタ
//		pSecond			：２段目の位置を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompGetPosition (int *pFirst, int *pSecond)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;

	// Check pFirst Parameter
	if (pFirst == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Gradation Compress Position pFirst NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pSecond Parameter
	if (pSecond == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Gradation Compress Position pSecond NULL Parameter Error.\n");
		goto _DONE;
	}

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Gradation Compress Position取得
	if ((status = sensorRegReadByte (id, SENSOR_REG_GRAD_COMP_POS_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// First Position
	*pFirst = (data8 >> SENSOR_REG_GRAD_COMP_POS_FIRST_SHIFT) & SENSOR_REG_GRAD_COMP_POS_FIRST_MASK;

	// Second Position
	*pSecond = (data8 >> SENSOR_REG_GRAD_COMP_POS_SECOND_SHIFT) & SENSOR_REG_GRAD_COMP_POS_SECOND_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
// 階調圧縮 Gain設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		firstPos			：１段目のGain
//		secondPos			：２段目のGain
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompSetGain (int first, int second)
{
	int status = AVAL_STATUS_SUCCESS;
	//int startMode = 0;

	// Check first Parameter
	if ((first < SENSOR_REG_GRAD_COMP_GAIN_FIRST_MIN) || (first > SENSOR_REG_GRAD_COMP_GAIN_FIRST_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Gradation Compress First Gain(%d) Parameter Error.(Min:%d / Max:%d)\n", first, SENSOR_REG_GRAD_COMP_GAIN_FIRST_MIN, SENSOR_REG_GRAD_COMP_GAIN_FIRST_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check second Parameter
	if ((second < SENSOR_REG_GRAD_COMP_GAIN_SECOND_MIN) || (second > SENSOR_REG_GRAD_COMP_GAIN_SECOND_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Gradation Compress Gain Position(%d) Parameter Error.(Min:%d / Max:%d)\n", second, SENSOR_REG_GRAD_COMP_GAIN_SECOND_MIN, SENSOR_REG_GRAD_COMP_GAIN_SECOND_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	//acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	//if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// StandByモード移行
	//if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// センサレジスタ設定
	if ((status = sensorGradationCompSetGainReg (first, second)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード解除
	//if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	// DDRへ設定
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_GAIN_FIRST, first);
	OUT32 (FIRM_DATA_GRADATION_COMPRESS_GAIN_SECOND, second);

_DONE:
	//if (startMode != 0)
		//acquisitionStart ();

	return (status);
}


//**********************************************************************************
// 階調圧縮 Gain Register設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		firstPos			：１段目のGain
//		secondPos			：２段目のGain
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompSetGainReg (int first, int second)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// First Gain
	data8 = (first & SENSOR_REG_GRAD_COMP_GAIN_FIRST_MASK) << SENSOR_REG_GRAD_COMP_GAIN_FIRST_SHIFT;

	// Second Gain
	data8 |= ((second & SENSOR_REG_GRAD_COMP_GAIN_SECOND_MASK) << SENSOR_REG_GRAD_COMP_GAIN_SECOND_SHIFT);

	// Gradation Compress Gain設定
	if ((status = sensorRegWriteByte (id, SENSOR_REG_GRAD_COMP_GAIN_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
// 階調圧縮 Gain取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pFirst			：１段目のGainを格納するポインタ
//		pSecond			：２段目のGainを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGradationCompGetGain (int *pFirst, int *pSecond)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;

	// Check pFirst Parameter
	if (pFirst == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Gradation Compress Gain pFirst NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check pSecond Parameter
	if (pSecond == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Gradation Compress Gain pSecond NULL Parameter Error.\n");
		goto _DONE;
	}

	// センサレジスタ設定
	id = 7;			// ID
	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Gradation Compress Gain取得
	if ((status = sensorRegReadByte (id, SENSOR_REG_GRAD_COMP_GAIN_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// First Position
	*pFirst = (data8 >> SENSOR_REG_GRAD_COMP_GAIN_FIRST_SHIFT) & SENSOR_REG_GRAD_COMP_GAIN_FIRST_MASK;

	// Second Position
	*pSecond = (data8 >> SENSOR_REG_GRAD_COMP_GAIN_SECOND_SHIFT) & SENSOR_REG_GRAD_COMP_GAIN_SECOND_MASK;

_DONE:
	return (status);
}


//**********************************************************************************
// 10Bitから8Bitに変換するモードを設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					： 0:8bit/1:10→8bit
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSet8BitConvert (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;

	// Check mode Parameter
	if ((mode != MODE_DISABLE) && (mode != MODE_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Bit 8  Set Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード移行
	if ((status = sensorStandBy ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Gradation Position設定
	if ((status = sensorGradationCompSetPosition (1, 8)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Gradation Gain設定
	if ((status = sensorGradationCompSetGain (2, 2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Gradation Mode設定
	if ((status = sensorGradationCompSetMode (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// StandByモード解除
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// メモリへ設定
	//OUT32 (FIRM_DATA_8BIT_MODE, mode);

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// 10Bitから8Bitに変換するモードを取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode					： 0:8bit/1:10→8bitのモードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGet8BitConvert (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Bit 8 Set Mode pMode NULL Parameter Error\n");
		goto _DONE;
	}

	// メモリから取得
	//*pMode = IN32 (FIRM_DATA_8BIT_MODE);
	if ((status = sensorGradationCompGetMode (pMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}
#endif // #if defined (MODE_SENSOR_GRADATION_COMPRESS)


#if defined (MODE_FRAMERATE_HIGH_SPEED)
//**********************************************************************************
//	Frame Rate High Speed Mode Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorFrameRateHighSpeedModeInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, FRAME_RATE_HIGH_SPEED_ADRS, FRAME_RATE_HIGH_SPEED_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gSensorStatus = status;
		return (status);
	}

	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		// Get High Speed Mode;
		//data32 = IN32 (FIRM_DATA_FRAME_RATE_HIGH_SPEED_MODE_ADRS);
		sensorGetFrameRateHighSpeedMode ((int *)&data32);
		
		// Get Virtual Height
		data32 |= (IN32(FIRM_DATA_FRAME_RATE_HIGH_SPEED_HEIGHT_ADRS) << 16);
		
		// Set Virtual Height Register
		OUT32 (FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_ADRS, data32);
	}
	
	return (status);
}


//**********************************************************************************
//	Frame Rate High Speed Mode Initialize Default
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorFrameRateHighSpeedModeInitializeDefault (void)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// Mode Disable
	OUT32 (FIRM_DATA_FRAME_RATE_HIGH_SPEED_MODE_ADRS, MODE_DISABLE);

	// FPGA設定
	if ((status = sensorSetFrameRateHighSpeedFpga (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Frame Rate High Speed Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：High Speed Modeを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetFrameRateHighSpeedMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Frame Rate Get High Speed Mode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Frame Rate High Speed Mode
	data32 = IN32 (FIRM_DATA_FRAME_RATE_HIGH_SPEED_MODE_ADRS);

	if (data32 == 0)
		*pMode = MODE_DISABLE;
	else
		*pMode = MODE_ENABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Frame Rate High Speed Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Frame rate High Speed Mode(MODE_DISABLE:Normal Speed Mode / MODE_ENABLE : High Speed Mode)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetFrameRateHighSpeedMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int startMode = 0;
	int currentMode;
	int dpcMode;
	double rateMaxValueD, frameRateD;
	int offsetx, offsety;
	int width, height;
	int heightMode;
	int bit;
	int lineStart, ffcHeight;
	int ffccormode;
	int gcMode;
	int binningy;
	int temp32;
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	int startLine;
#endif
	int heightMax;
	int tges;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Frame Rete Set High Speed Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 現在の設定状況を取得
	if ((status = sensorGetFrameRateHighSpeedMode (&currentMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Start Status
	acquisitionGetStartFlag (&startMode);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// BinningY
	if ((status = aoiGetBinningY (&binningy)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == MODE_ENABLE)
	{
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		if (binningy > 4)
		{
			binningy = 4;
	
			// BinningY設定変更
			if ((status = aoiSetBinningY (binningy)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
#endif

		//--------------------------------------------------------------------------------
		// 取得
		//--------------------------------------------------------------------------------

		// Width Offset取得
		if ((status = aoiGetWidthOffset (&offsetx)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Height Offset取得
		if ((status = aoiGetHeightOffset (&offsety)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Widthを取得
		if ((status = aoiGetWidth (&width)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Height取得
		if ((status = aoiGetHeight (&height)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (binningy != 0)
			height *= binningy;

		//--------------------------------------------------------------------------------
		// 設定
		//--------------------------------------------------------------------------------

#if defined (MODE_CXP)
		// CXPはBoot時はcxpInitializeImageParamで再初期化する
		if (gInterFaceID == INTERFACE_CXP)
		{
			if (IN32 (FIRM_DATA_FIRM_BOOT_FLAG_ADRS) == 0)
			{
				heightMode = height;
				goto _NEXT;
			}
		}
#endif

		//------------------------------------------------------------
		// TGES(aoiSetHeightOffset→roiSetHeightMain→acquisitionSetFrameRateで
		// autoBrightSetExposureMaxを設定しているが、
		// TGESを参照しているため、ここで設定
		//------------------------------------------------------------
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		if ((status = sensorGetTgesHighSpeed (4, (int *)&tges)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		if ((status = tgSetTges (tges)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

		// Width Offset設定
		if ((status = aoiSetWidthOffset (0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Height Offset設定
		if ((status = aoiSetHeightOffset (0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Width設定(Binningを考慮し、取得したWidthを設定)
		if ((status = aoiSetWidth (width)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((height == 2) || (height == 4) || (height == 6))
		{
			// 何もしない
			heightMode = height;
		}
		else
		{
	#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

			heightMode = 4;
			temp32 = 4 / binningy;

			
	#else // #if defined (MODE_SENSOR_IMX992) && defined (MODE_SENSOR_IMX993)

			if (gInterFaceID == INTERFACE_CXP)
				heightMode = 4;
			else
				heightMode = 2;

			if (binningy != 0)
				temp32 = heightMode / binningy;
			else
				temp32 = heightMode;

	#endif // #if defined (MODE_SENSOR_IMX992) && defined (MODE_SENSOR_IMX993)

			// High Speed Initialize2
			OUT32 (FIRM_DATA_HIGHSPPED_INIT2_ADRS, 1);

			// Height設定
			if ((status = aoiSetHeight (temp32)) != AVAL_STATUS_SUCCESS)
			{
				// High Speed Initialize2
				OUT32 (FIRM_DATA_HIGHSPPED_INIT2_ADRS, 0);
				goto _DONE;
			}

			// High Speed Initialize2
			OUT32 (FIRM_DATA_HIGHSPPED_INIT2_ADRS, 0);
		}
	}
	else
	{
		heightMode = 0;
	}

#if defined (MODE_CXP)
_NEXT:
#endif

	// Height数を取得
	if (mode == MODE_ENABLE)
	{
		ffcHeight = heightMode;
	}
	else
	{
		// Height取得
		if ((status = aoiGetHeight (&ffcHeight)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (binningy != 0)
			ffcHeight *= binningy;
	}

	if (mode == MODE_ENABLE)
	{
		dpcMode = MODE_DISABLE;
	}
	else
	{
		dpcMode = MODE_ENABLE;
	}

	// DPC設定
	if ((status = dpcSetMode (dpcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	if (mode == MODE_ENABLE)
		startLine = FPGA_FRAMERATE_HIGH_SPEED_START_LINE_FROM_TOP_HS;
	else
		startLine = FPGA_FRAMERATE_HIGH_SPEED_START_LINE_FROM_TOP_NORMAL;
	
	// Start Line設定
	if ((status = sensorSetFrameRateHighSpeedStartLine (startLine)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

	// FPGA設定
	if ((status = sensorSetFrameRateHighSpeedFpga (heightMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// DDR設定
	if ((status = sensorSetFrameRateHighSpeedReg (heightMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	// センサ初期化
	if ((status = sensorVendorSFrameRateSpeedModeInitialize (mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif  // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)


	if (mode == MODE_DISABLE)
	{
		// 最大レート取得
		if ((status = rateMax (&rateMaxValueD)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// レート取得
		if ((status = acquisitionGetFrameRate (&frameRateD)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// フレームレートオーバー？
		if (rateMaxValueD < frameRateD)
		{
			if ((status = acquisitionSetFrameRate (rateMaxValueD)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		if (gInterFaceID == INTERFACE_CAMERALINK)
		{
			// Heightを取得
			if ((status = aoiGetHeight (&height)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		else
		{
			// Heightを取得
			if ((status = sensorGetVirtualHeight (&height)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Width Max
			heightMax = HeightMax ();

			if (height > heightMax)
				height = heightMax;
			
			if (height > (heightMax / binningy))
				height = (heightMax / binningy);
			
			if ((height % binningy) != 0)
				height &= ~(binningy - 1);
		}

		// Heightを再設定
		if ((status = aoiSetHeight (height)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Bit取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 8bitの場合、Enable時は高感度モードなるように再度設定。Disable時は標準感度。
	if (bit == 8)
	{
		if (mode == MODE_DISABLE)
			gcMode = MODE_ENABLE;
		else
			gcMode = MODE_DISABLE;
	}
	else
	{
		gcMode = MODE_DISABLE;
	}

	// GC Mode設定
	if ((status = sensorSet8BitConvert (gcMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	#if defined (MODE_BIT8_GCMODE_ENABLE)
	if (bit != 8)
	{
		// 8bit時に階調圧縮モードをデフォルト有効にするかのフラグ
		//
		// bit 8 => hsm 1  => bit 12  => hsm 0  => bit 8の場合NG。階調圧縮モードになってない。
		// この問題に対応する為の設定。
		OUT32 (FIRM_DATA_BIT8_CONVERT_MODE, MODE_ENABLE);
	}
	#endif

	// Enable & Disableへの変更時のみFFCデータ入れ替え
	if ( ((currentMode == MODE_DISABLE) && (mode == MODE_ENABLE)) ||
         ((currentMode == MODE_ENABLE)  && (mode == MODE_DISABLE)))
	{
		// FFC Correction Mode取得
		if ((status = ffcGetCorMode (&ffccormode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFC Line Start取得
		if ((status = ffcGetReplacementLine (&lineStart, ffcHeight, ffccormode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// FFCデータ入れ替え
		if ((status = ffcDataReplacement (lineStart, ffcHeight, ffccormode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	//------------------------------------------------------------
	// Virtaul Height
	//------------------------------------------------------------
	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		if (mode == MODE_ENABLE)
		{
			// Virtual Height Mode設定
			if ((status = sensorSetVirtualHeightMode (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
			
			// Virtual Height設定
			if (currentMode != MODE_ENABLE)
			{
				height /= binningy;

				// Set Virtual Height(通常時のHeightをVirtual Heightに設定)
				if ((status = sensorSetVirtualHeight (height)) != AVAL_STATUS_SUCCESS)
					goto _DONE;
			}
			else
			{
				// Get Virtual Height
				if ((status = sensorGetVirtualHeight (&height)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

				// Set Virtual Height
				if ((status = sensorSetVirtualHeight (height)) != AVAL_STATUS_SUCCESS)
					goto _DONE;
			}
		}
		else
		{
			// Virtual Height Mode設定
			if ((status = sensorSetVirtualHeightMode (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
	}

_DONE:
	if (startMode != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
//	Frame Rate High Speed レジスタ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Frame rate High Speed Mode
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetFrameRateHighSpeedFpga (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// レジスタ取得
	data32 = IN32 (FPGA_FRAMERATE_HIGH_SPEED_CTRL_ADRS);
	data32 &= ~FPGA_FRAMERATE_HIGH_SPEED_MASK;

	if (mode == 2)
		data32 |= FPGA_FRAMERATE_HIGH_SPEED_2LINE;
	else if (mode == 4)
		data32 |= FPGA_FRAMERATE_HIGH_SPEED_4LINE;
	else if (mode == 6)
		data32 |= FPGA_FRAMERATE_HIGH_SPEED_6LINE;

	// レジスタ設定
	OUT32 (FPGA_FRAMERATE_HIGH_SPEED_CTRL_ADRS, data32);

	return (status);
}


//**********************************************************************************
//	Frame Rate High Speed レジスタ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Frame rate High Speed Mode
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetFrameRateHighSpeedReg (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Set Frame Rate High Speed Mode
	OUT32 (FIRM_DATA_FRAME_RATE_HIGH_SPEED_MODE_ADRS, (unsigned int)mode);

	return (status);
}


#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
//**********************************************************************************
//	Frame Rate High Speed Start Line設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		startLine			：Start Line
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetFrameRateHighSpeedStartLine (int startLine)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check startLine Parameter
	if ((startLine < FPGA_FRAMERATE_HIGH_SPEED_START_LINE_MIN) || (startLine > FPGA_FRAMERATE_HIGH_SPEED_START_LINE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Frame Rate High Speed Set Start Line(%d) Parameter Error.(Min:%d / Max:%d)\n", startLine, FPGA_FRAMERATE_HIGH_SPEED_START_LINE_MIN, FPGA_FRAMERATE_HIGH_SPEED_START_LINE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	// レジスタ取得
	data32 = IN32 (FPGA_FRAMERATE_HIGH_SPEED_START_LINE_ADRS);
	data32 &= ~(FPGA_FRAMERATE_HIGH_SPEED_START_LINE_MASK << FPGA_FRAMERATE_HIGH_SPEED_START_LINE_SHIFT);

	if (startLine == 0)
	{
		data32 |= (0x01 << FPGA_FRAMERATE_HIGH_SPEED_START_LINE_SHIFT);
		data32 &= ~FPGA_FRAMERATE_HIGH_SPEED_START_LINE_ENABLE;
	}
	else
	{
		data32 |= (startLine << FPGA_FRAMERATE_HIGH_SPEED_START_LINE_SHIFT);
		data32 |= FPGA_FRAMERATE_HIGH_SPEED_START_LINE_ENABLE;
	}

	// レジスタ設定
	OUT32 (FPGA_FRAMERATE_HIGH_SPEED_START_LINE_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
//	Frame Rate High Speed Start Line取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pStartLine			：StartLineを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetFrameRateHighSpeedStartLine (int *pStartLine)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pStartLine Parameter
	if (pStartLine == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Frame Rate High Speed Get Start Line NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Start Line
	data32 = IN32 (FPGA_FRAMERATE_HIGH_SPEED_START_LINE_ADRS);
	*pStartLine = (data32 >> FPGA_FRAMERATE_HIGH_SPEED_START_LINE_SHIFT) & FPGA_FRAMERATE_HIGH_SPEED_START_LINE_MASK;

_DONE:
	return (status);
}
#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)


//**********************************************************************************
//	Vitrual Height Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode					：モードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetVirtualHeightMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Virtial High Get Mode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Virtual Height Mode
	data32 = IN32 (FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_ADRS);
	
	if (data32 & FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_MODE_BIT)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Vitrual Height Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：MODE_ENABLE=有効/MODE_DISABLE_=無効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetVirtualHeightMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Virtial High Set Mode(%d) Parameter Error.\n", mode);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Virtual Height Mode
	data32 = IN32 (FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_ADRS);
	
	if (mode == MODE_ENABLE)
		data32 |= FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_MODE_BIT;
	else
		data32 &= ~FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_MODE_BIT;

	// Set Virtual Height Mode
	OUT32 (FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
//	Vitrual Height LineScan Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode					：モードを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetVirtualHeightLineScanMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Virtial High Get LineScan Mode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Get Virtual Height LineScan Mode
	data32 = IN32 (FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_ADRS);
	
	if (data32 & FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_LINESCAN_BIT)
		*pMode = MODE_ENABLE;
	else
		*pMode = MODE_DISABLE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Vitrual Height LineScan Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode					：MODE_ENABLE=有効/MODE_DISABLE_=無効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetVirtualHeightLineScanMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Virtial High Set LineScan Mode(%d) Parameter Error.\n", mode);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Virtual Height LineScan Mode
	data32 = IN32 (FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_ADRS);
	
	if (mode == MODE_ENABLE)
		data32 |= FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_LINESCAN_BIT;
	else
		data32 &= ~FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_LINESCAN_BIT;

	// Set Virtual Height LineScan Mode
	OUT32 (FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
//	Vitrual Height取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pHeight					：High Speed Modeでの仮想Height(GE/CXPのみ)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetVirtualHeight (int *pHeight)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Check pHeight Parameter
	if (pHeight == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Virtial Height Get NULL Parameter Error.\n");
		goto _DONE;
	}

	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		// Get Frame Rate High Speed Virtual Height
		data32 = IN32 (FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_ADRS);
		*pHeight = (data32 >> FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_SHIFT) & FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_MASK;
	}
	else
	{
		// Get Frame Rate High Speed Virtual Height
		*pHeight = IN32 (FIRM_DATA_FRAME_RATE_HIGH_SPEED_HEIGHT_ADRS);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Vitrual Height設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		height				：High Speed Modeでの仮想Height
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetVirtualHeight (int height)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int virtualHeight;
	
	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		// Check height Parameter
		if ((height <= 0) || (height > FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_GE))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Virtual Height(%d) Parameter Error.(Min:0 / Max:%d).\n", height, FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_GE);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Check Height Size
		if ((status = sensorCheckVirtualHeight (height, &virtualHeight)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Set Frame Rate High Speed Virtual Height
		data32 = IN32 (FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_ADRS);
		data32 &= ~(FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_MASK << FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_SHIFT);
		data32 |= ((virtualHeight & FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_MASK) << FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_SHIFT);
		OUT32 (FPGA_FRAMERATE_HIGH_SPEED_VIRTUAL_HEIGHT_ADRS, data32);

		// Set Frame Rate High Speed Virtual Height
		OUT32 (FIRM_DATA_FRAME_RATE_HIGH_SPEED_HEIGHT_ADRS, height);
	}
	else
	{
		// Check height Parameter
		if ((height <= 0) || (height > FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_OTHER))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Virtual Height(%d) Parameter Error.(Min:0 / Max:%d).\n", height, FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_OTHER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		// Set Frame Rate High Speed Virtual Height
		OUT32 (FIRM_DATA_FRAME_RATE_HIGH_SPEED_HEIGHT_ADRS, height);
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Vitrual Height Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		virtualHeight		：仮想Height
//		pVirtualHeight		：Hish Speedモード時のHeight
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorCheckVirtualHeight (int virtualHeight, int *pVirtualHeight)
{
	int status = AVAL_STATUS_SUCCESS;
	int height;
	int amari;

	// Check pVirtualHeight Parameter
	if (pVirtualHeight == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Virtial Height Check NULL Parameter Error.\n");
		goto _DONE;
	}

	// Heightを取得
	if ((status = aoiGetHeight (&height)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 割り切れない場合は修正
	amari = virtualHeight % height;

	// データ格納
	*pVirtualHeight = virtualHeight - amari;

	// 負数? or 0?
	if (*pVirtualHeight <= 0)
		*pVirtualHeight = height;
	
_DONE:
	return (status);
}


//**********************************************************************************
//	センサ初期化(Frame Rateモード)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorVendorSFrameRateSpeedModeInitialize (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	int regHold, acTmg;
	unsigned char id;
	unsigned char data8;
	int bit;
	unsigned int blackPixelMode;
	int size;
	unsigned int VSize;
	int tges, tgpd;

	// Check mode Parameter
	if ((mode != MODE_ENABLE) && (mode != MODE_DISABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Initialize Frame Rate Mode(%d) Parameter Error.(Disable:%d / Enable:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	//------------------------------------------------------------
	// BlackPixel
	//------------------------------------------------------------
	if (mode == MODE_DISABLE)
		blackPixelMode = 1;
	else
		blackPixelMode = 0;

	OUT32 (FIRM_DATA_BLACKPIXEL_MODE_ADRS, blackPixelMode);

	//------------------------------------------------------------
	// Bit取得
	//------------------------------------------------------------
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	//============================================================
	//============================================================
	// Sensor側の初期化開始
	//============================================================
	//============================================================
	//============================================================

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//------------------------------------------------------------
	// StandBy
	//------------------------------------------------------------
	id = 0x02;
	data8 = SENSOR_REG_STANDBY_BIT;	// Standbyモード
	if ((status = sensorRegWriteByte (id, SENSOR_REG_STANDBY_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == MODE_DISABLE)
	{
		//------------------------------------------------------------
		// Height
		//------------------------------------------------------------
		if ((status = fpgaRoiGetCameraHeightTotalSize (&size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Height Offsetレジスタ設定
		//------------------------------------------------------------
		if ((status = fpgaRoiSetCameraHeight (0, IMG_HEIGHT_OFFSET, size, 1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Memory Heightレジスタ設定
		//------------------------------------------------------------
		if ((status = memRoiSetCameraHeight (0, IMG_HEIGHT_OFFSET, size, 1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// ROI Set
		//------------------------------------------------------------
		if ((status = roiSetEnd()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// 隠しレジスタ初期化
		//------------------------------------------------------------
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		if ((status = sensorVendorS_FrameRateHighSpeedModeDisableSetting ()) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

		//------------------------------------------------------------
		// TGES
		//------------------------------------------------------------
		if ((status = tgSetTges (SENSOR_TGES)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// TGPD
		//------------------------------------------------------------
		tgpd = SENSOR_VINTERVAL_LINE;
		if ((status = tgSetTgpd (tgpd)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		//------------------------------------------------------------
		// Height
		//------------------------------------------------------------
		if ((status = fpgaRoiGetCameraHeightTotalSize (&size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Height Offsetレジスタ設定
		//------------------------------------------------------------
		if ((status = fpgaRoiSetCameraHeight (0, 0, size, 1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// Memory Heightレジスタ設定
		//------------------------------------------------------------
		if ((status = memRoiSetCameraHeight (0, 0, size, 1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// ROI Set
		//------------------------------------------------------------
		if ((status = roiSetEnd()) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// 隠しレジスタ初期化
		//------------------------------------------------------------
		if ((status = sensorVendorS_FrameRateHighSpeedModeEnableSetting (size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// TGES
		//------------------------------------------------------------
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

		if ((status = sensorGetTgesHighSpeed (size, (int *)&tges)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
#endif

		if ((status = tgSetTges (tges)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		//------------------------------------------------------------
		// TGPD
		//------------------------------------------------------------
#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		if ((status = sensorGetTgpdHighSpeed (size, (int *)&VSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif
		// Get TGPD
		if ((status = tgSetTgpd (VSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Bit隠しレジスタ
	if ((status = sensorBitSetHide (bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//============================================================
	//============================================================
	//============================================================
	// Sensor側の終了
	//============================================================
	//============================================================
	//============================================================

	//------------------------------------------------------------
	// Sensor Standby解除
	//------------------------------------------------------------
	if ((status = sensorStandByCancel ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
//**********************************************************************************
//	Sensor Hide Register Setting
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		height				：height数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorVendorS_FrameRateHighSpeedModeEnableSetting (int height)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char adrs;
	unsigned char data8;
	int regHold, acTmg;
	unsigned char id;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//============================================================
	//============================================================
	// 隠しレジスタ
	//============================================================
	//============================================================

	//============================================================
	// Chip ID = 0x02
	//============================================================
	id = 0x02;

	//------------------------------------------------------------
	// Adrs = 0x40
	//------------------------------------------------------------
	adrs = 0x40;
	data8 = 0x3F;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x44(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x44;
	//data8 = (68<<1 | 0x01);
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x45(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x45;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x46(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x46;
	//data8 = 69;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x47(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x47;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x48(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x48;
	//data8 = 0x42;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x49(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x49;
	//data8 = 0x02;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4c(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x4c;
	//data8 = 0x01;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x4d(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x4d;
	//data8 = 0x01;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x74
	// 0x75番地も合わせると0x0809
	// 0x09の最下位ビットはEnablebitなので>>1すると0x0404=1028
	// このレジスタには+67して設定しているので1028-67=961ライン目を使用
	// ※奇数ラインを設定する必要があり
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0x09;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x75
	//------------------------------------------------------------
	adrs = 0x75;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	// 0x77番地も合わせると0x0405=1029
    // このレジスタにはStart+height-1を設定しているので、
	// 1028 + height - 1 = 1029すなわちHeight=2
	//------------------------------------------------------------
	adrs = 0x76;
	//data8 = 0x05;
	data8 = 0x04 + height - 1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x77
	//------------------------------------------------------------
	adrs = 0x77;
	data8 = 0x04;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x42;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x79
	//------------------------------------------------------------
	adrs = 0x79;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7c
	//------------------------------------------------------------
	adrs = 0x7c;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd4
	//------------------------------------------------------------
	// VMAX:Slaveモードで使用している為、未使用
	//adrs = 0xd4;
	//data8 = 0x1c;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd5
	//------------------------------------------------------------
	// VMAX:Slaveモードで使用している為、未使用
	//adrs = 0xd5;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd6
	//------------------------------------------------------------
	// VMAX:Slaveモードで使用している為、未使用
	//adrs = 0xd6;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd8
	//------------------------------------------------------------
	// HMAX:Slaveモードで使用している為、未使用
	//adrs = 0xd8;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd9
	//------------------------------------------------------------
	// HMAX:Slaveモードで使用している為、未使用
	//adrs = 0xd9;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xdc
	//------------------------------------------------------------
	adrs = 0xdc;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe3
	//------------------------------------------------------------
	adrs = 0xe3;
	data8 = 0x04;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe6
	//------------------------------------------------------------
	adrs = 0xe6;
	data8 = 0x10;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0xe7
	//------------------------------------------------------------
	adrs = 0xe7;
	if ((status = sensorRegReadByte (id, adrs, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 &= ~0x03;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0xf7
	//------------------------------------------------------------
	adrs = 0xf7;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x03
	//============================================================
	id = 0x03;

	//------------------------------------------------------------
	// Adrs = 0x04
	//------------------------------------------------------------
	adrs = 0x04;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x05
	//------------------------------------------------------------
	adrs = 0x05;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x20
	//------------------------------------------------------------
	adrs = 0x20;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x21
	//------------------------------------------------------------
	adrs = 0x21;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x22
	//------------------------------------------------------------
	adrs = 0x22;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x23
	//------------------------------------------------------------
	adrs = 0x23;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x24
	//------------------------------------------------------------
	adrs = 0x24;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x25
	//------------------------------------------------------------
	adrs = 0x25;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x26
	//------------------------------------------------------------
	adrs = 0x26;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x27
	//------------------------------------------------------------
	adrs = 0x27;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x04
	//============================================================
	id = 0x04;

	//------------------------------------------------------------
	// Adrs = 0x00
	//------------------------------------------------------------
	// ADCのBit設定の為、ここでは未設定

	//------------------------------------------------------------
	// Adrs = 0x26
	//------------------------------------------------------------
	adrs = 0x26;
	data8 = 0x82;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x30
	//------------------------------------------------------------
	adrs = 0x30;
	if ((status = sensorRegReadByte (id, adrs, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 &= 0x03;
	data8 |= (0x08<<2);
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x31
	//------------------------------------------------------------
	adrs = 0x31;
	if ((status = sensorRegReadByte (id, adrs, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	data8 &= ~0x03;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x32(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	adrs = 0x32;
	data8 = 0x03;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x40/0x41/0x42(トリガーモードで未使用なので設定しない)
	//------------------------------------------------------------

	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	adrs = 0x44;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x48
	//------------------------------------------------------------
	adrs = 0x48;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4c
	//------------------------------------------------------------
	adrs = 0x4c;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0xa5;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x63
	//------------------------------------------------------------
	adrs = 0x63;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd0
	//------------------------------------------------------------
	adrs = 0xd0;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd1
	//------------------------------------------------------------
	adrs = 0xd1;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x05
	//============================================================
	id = 0x05;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x81
	//------------------------------------------------------------
	adrs = 0x81;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x06
	//============================================================
	id = 0x06;

	//------------------------------------------------------------
	// Adrs = 0x30
	//------------------------------------------------------------
	// 出力bit設定の為、ここでは未設定


	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	adrs = 0x44;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x45
	//------------------------------------------------------------
	adrs = 0x45;
	data8 = 0x02;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x07
	//============================================================
	id = 0x07;

	//------------------------------------------------------------
	// Adrs = 0x20(BlackPixel1)
	//------------------------------------------------------------
	adrs = 0x20;
	data8 = 0x5f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x40(BlackPixel2)
	//------------------------------------------------------------
	adrs = 0x40;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x42
	//------------------------------------------------------------
	adrs = 0x42;
	data8 = 0x05;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4b
	//------------------------------------------------------------
	adrs = 0x4b;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x98
	//------------------------------------------------------------
	adrs = 0x98;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc0
	//------------------------------------------------------------
	//黒レベルはbitごとに異なるので、ここでは設定しない

	//------------------------------------------------------------
	// Adrs = 0xc8
	//------------------------------------------------------------
	// sensorBitSetHide関数で設定
	
	//------------------------------------------------------------
	// Adrs = 0xe0
	//------------------------------------------------------------
	adrs = 0xe0;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xec
	//------------------------------------------------------------
	adrs = 0xec;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x08
	//============================================================
	id = 0x08;

	//------------------------------------------------------------
	// Adrs = 0x22
	//------------------------------------------------------------
	adrs = 0x22;
	data8 = 0x10;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5a
	//------------------------------------------------------------
	adrs = 0x5a;
	data8 = 0x16;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x92
	//------------------------------------------------------------
	adrs = 0x92;
	data8 = 0x10;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x0c
	//============================================================
	id = 0x0c;

	//------------------------------------------------------------
	// Adrs = 0x6a
	//------------------------------------------------------------
	adrs = 0x6a;
	data8 = 0xa5;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6b
	//------------------------------------------------------------
	adrs = 0x6b;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe8
	//------------------------------------------------------------
	adrs = 0xe8;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe9
	//------------------------------------------------------------
	adrs = 0xe9;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x0d
	//============================================================
	id = 0x0d;

	//------------------------------------------------------------
	// Adrs = 0x8c
	//------------------------------------------------------------
	adrs = 0x8c;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8d
	//------------------------------------------------------------
	adrs = 0x8d;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x10
	//============================================================
	id = 0x10;

	//------------------------------------------------------------
	// Adrs = 0x60
	//------------------------------------------------------------
	adrs = 0x60;
	data8 = 0x1d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0xd1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf0
	//------------------------------------------------------------
	adrs = 0xf0;
	data8 = 0x18;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf2
	//------------------------------------------------------------
	adrs = 0xf2;
	data8 = 0xd6;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x11
	//============================================================
	id = 0x11;

	//------------------------------------------------------------
	// Adrs = 0x14
	//------------------------------------------------------------
	adrs = 0x14;
	data8 = 0xD7;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x74
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0x88;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0xd7;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x12
	//============================================================
	id = 0x12;

	//------------------------------------------------------------
	// Adrs = 0x18
	//------------------------------------------------------------
	adrs = 0x18;
	data8 = 0x83;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1a
	//------------------------------------------------------------
	adrs = 0x1a;
	data8 = 0x14;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x2a
	//------------------------------------------------------------
	adrs = 0x2a;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2b
	//------------------------------------------------------------
	adrs = 0x2b;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2c
	//------------------------------------------------------------
	adrs = 0x2c;
	data8 = 0xe4;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2d
	//------------------------------------------------------------
	adrs = 0x2d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2e
	//------------------------------------------------------------
	adrs = 0x2e;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2f
	//------------------------------------------------------------
	adrs = 0x2f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x38
	//------------------------------------------------------------
	adrs = 0x38;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3a
	//------------------------------------------------------------
	adrs = 0x3a;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3b
	//------------------------------------------------------------
	adrs = 0x3b;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6a
	//------------------------------------------------------------
	adrs = 0x6a;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6b
	//------------------------------------------------------------
	adrs = 0x6b;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6c
	//------------------------------------------------------------
	adrs = 0x6c;
	data8 = 0xe4;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6d
	//------------------------------------------------------------
	adrs = 0x6d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x25;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7e
	//------------------------------------------------------------
	adrs = 0x7e;
	data8 = 0x6e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x88
	//------------------------------------------------------------
	adrs = 0x88;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8a
	//------------------------------------------------------------
	adrs = 0x8a;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8b
	//------------------------------------------------------------
	adrs = 0x8b;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc8
	//------------------------------------------------------------
	adrs = 0xc8;
	data8 = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xca
	//------------------------------------------------------------
	adrs = 0xca;
	data8 = 0x73;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x13
	//============================================================
	id = 0x13;

	//------------------------------------------------------------
	// Adrs = 0xd0
	//------------------------------------------------------------
	adrs = 0xd0;
	data8 = 0x61;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd5
	//------------------------------------------------------------
	adrs = 0xd5;
	data8 = 0x11;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x19
	//============================================================
	id = 0x19;

	//------------------------------------------------------------
	// Adrs = 0x0b
	//------------------------------------------------------------
	adrs = 0x0b;
	data8 = 0x1f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0x07;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x85
	//------------------------------------------------------------
	adrs = 0x85;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x86
	//------------------------------------------------------------
	adrs = 0x86;
	data8 = 0x0d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x87
	//------------------------------------------------------------
	adrs = 0x87;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8c
	//------------------------------------------------------------
	adrs = 0x8c;
	data8 = 0x71;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8d
	//------------------------------------------------------------
	adrs = 0x8d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8e
	//------------------------------------------------------------
	adrs = 0x8e;
	data8 = 0x75;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8f
	//------------------------------------------------------------
	adrs = 0x8f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x94
	//------------------------------------------------------------
	adrs = 0x94;
	data8 = 0x06;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x95
	//------------------------------------------------------------
	adrs = 0x95;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x96
	//------------------------------------------------------------
	adrs = 0x96;
	data8 = 0x0e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x97
	//------------------------------------------------------------
	adrs = 0x97;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9c
	//------------------------------------------------------------
	adrs = 0x9c;
	data8 = 0x70;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9d
	//------------------------------------------------------------
	adrs = 0x9d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9e
	//------------------------------------------------------------
	adrs = 0x9e;
	data8 = 0x76;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9f
	//------------------------------------------------------------
	adrs = 0x9f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x1c
	//============================================================
	id = 0x1c;

	//------------------------------------------------------------
	// Adrs = 0x1c
	//------------------------------------------------------------
	adrs = 0x1c;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1d
	//------------------------------------------------------------
	adrs = 0x1d;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1e
	//------------------------------------------------------------
	adrs = 0x1e;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1f
	//------------------------------------------------------------
	adrs = 0x1f;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x24
	//------------------------------------------------------------
	adrs = 0x24;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7c
	//------------------------------------------------------------
	adrs = 0x7c;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7d
	//------------------------------------------------------------
	adrs = 0x7d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7e
	//------------------------------------------------------------
	adrs = 0x7e;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x81
	//------------------------------------------------------------
	adrs = 0x81;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x82
	//------------------------------------------------------------
	adrs = 0x82;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8e
	//------------------------------------------------------------
	adrs = 0x8e;
	data8 = 0xe4;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x8f
	//------------------------------------------------------------
	adrs = 0x8f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x92
	//------------------------------------------------------------
	adrs = 0x92;
	data8 = 0xe4;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x93
	//------------------------------------------------------------
	adrs = 0x93;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd1
	//------------------------------------------------------------
	adrs = 0xd1;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Hide Register Setting(IMX992/IMX993)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorVendorS_FrameRateHighSpeedModeDisableSetting (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char adrs;
	unsigned char data8;
	int regHold, acTmg;
	unsigned char id;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//============================================================
	//============================================================
	// 隠しレジスタ
	//============================================================
	//============================================================

	//============================================================
	// Chip ID = 0x02
	//============================================================
	id = 0x02;

	//------------------------------------------------------------
	// Adrs = 0x40
	//------------------------------------------------------------
	adrs = 0x40;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	//adrs = 0x44;
	//data8 = 0x01;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x45
	//------------------------------------------------------------
	//adrs = 0x45;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x46
	//------------------------------------------------------------
	//adrs = 0x46;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x47
	//------------------------------------------------------------
	//adrs = 0x47;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x48(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x48;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x49(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x49;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4c(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x4c;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4d(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	//adrs = 0x4d;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x74
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x75
	//------------------------------------------------------------
	adrs = 0x75;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x77
	//------------------------------------------------------------
	adrs = 0x77;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x79
	//------------------------------------------------------------
	adrs = 0x79;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7c
	//------------------------------------------------------------
	adrs = 0x7c;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd4
	//------------------------------------------------------------
	// VMAX:Slaveモードで使用している為、未使用
	//adrs = 0xd4;
	//data8 = 0x1c;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd5
	//------------------------------------------------------------
	// VMAX:Slaveモードで使用している為、未使用
	//adrs = 0xd5;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd6
	//------------------------------------------------------------
	// VMAX:Slaveモードで使用している為、未使用
	//adrs = 0xd6;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd8
	//------------------------------------------------------------
	// HMAX:Slaveモードで使用している為、未使用
	//adrs = 0xd8;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd9
	//------------------------------------------------------------
	// HMAX:Slaveモードで使用している為、未使用
	//adrs = 0xd9;
	//data8 = 0x00;
	//if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xdc
	//------------------------------------------------------------
	adrs = 0xdc;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe3
	//------------------------------------------------------------
	adrs = 0xe3;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe6
	//------------------------------------------------------------
	adrs = 0xe6;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe7
	//------------------------------------------------------------
	adrs = 0xe7;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0xf7
	//------------------------------------------------------------
	adrs = 0xf7;
	data8 = 0x80;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x03
	//============================================================
	id = 0x03;

	//------------------------------------------------------------
	// Adrs = 0x04
	//------------------------------------------------------------
	adrs = 0x04;
	data8 = 0x03;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x05
	//------------------------------------------------------------
	adrs = 0x05;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x20
	//------------------------------------------------------------
	adrs = 0x20;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x21
	//------------------------------------------------------------
	adrs = 0x21;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x22
	//------------------------------------------------------------
	adrs = 0x22;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x23
	//------------------------------------------------------------
	adrs = 0x23;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x24
	//------------------------------------------------------------
	adrs = 0x24;
	data8 = SENSOR_DEFAULT_WIDTH & 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x25
	//------------------------------------------------------------
	adrs = 0x25;
	data8 = (SENSOR_DEFAULT_WIDTH >> 8) & 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x26 @@@@@aoiSetHeightで実施
	//------------------------------------------------------------
	adrs = 0x26;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x27 @@@@@aoiSetHeightで実施
	//------------------------------------------------------------
	adrs = 0x27;
	data8 = 0x08;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x04
	//============================================================
	id = 0x04;

	//------------------------------------------------------------
	// Adrs = 0x00
	//------------------------------------------------------------
	// ADCのBit設定の為、ここでは未設定

	//------------------------------------------------------------
	// Adrs = 0x26
	//------------------------------------------------------------
	adrs = 0x26;
	data8 = 0x82;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x30
	//------------------------------------------------------------
	adrs = 0x30;
	//data8 = 0x29;
	data8 = SENSOR_REG_FIRST_TRG1_DEFAULT;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x31
	//------------------------------------------------------------
	adrs = 0x31;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x32(ROI_High_Speed_Mode_E.pfd)
	//------------------------------------------------------------
	adrs = 0x32;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x40/0x41/0x42(SHSは未使用なので設定しない)
	//------------------------------------------------------------

	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	adrs = 0x44;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x48
	//------------------------------------------------------------
	adrs = 0x48;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4c
	//------------------------------------------------------------
	adrs = 0x4c;
	data8 = 0x26;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0xa5;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x63
	//------------------------------------------------------------
	adrs = 0x63;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd0
	//------------------------------------------------------------
	adrs = 0xd0;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd1
	//------------------------------------------------------------
	adrs = 0xd1;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x05
	//============================================================
	id = 0x05;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x81
	//------------------------------------------------------------
	adrs = 0x81;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x06
	//============================================================
	id = 0x06;

	//------------------------------------------------------------
	// Adrs = 0x30
	//------------------------------------------------------------
	// 出力bit設定の為、ここでは未設定


	//------------------------------------------------------------
	// Adrs = 0x44
	//------------------------------------------------------------
	adrs = 0x44;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x45
	//------------------------------------------------------------
	adrs = 0x45;
	data8 = 0x02;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x07
	//============================================================
	id = 0x07;

	//------------------------------------------------------------
	// Adrs = 0x20(BlackPixel1)
	//------------------------------------------------------------
	adrs = 0x20;
	data8 = 0x50;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x40(BlackPixel2)
	//------------------------------------------------------------
	adrs = 0x40;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x42
	//------------------------------------------------------------
	adrs = 0x42;
	data8 = 0x05;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x4b
	//------------------------------------------------------------
	adrs = 0x4b;
	data8 = 0xb8;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x98
	//------------------------------------------------------------
	adrs = 0x98;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc0
	//------------------------------------------------------------
	//黒レベルはbitごとに異なるので、ここでは設定しない

	//------------------------------------------------------------
	// Adrs = 0xc8
	//------------------------------------------------------------
	// sensorBitSetHide関数で設定
	
	//------------------------------------------------------------
	// Adrs = 0xe0
	//------------------------------------------------------------
	adrs = 0xe0;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xec
	//------------------------------------------------------------
	adrs = 0xec;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x08
	//============================================================
	id = 0x08;

	//------------------------------------------------------------
	// Adrs = 0x22
	//------------------------------------------------------------
	adrs = 0x22;
	data8 = 0x10;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x5a
	//------------------------------------------------------------
	adrs = 0x5a;
	data8 = 0x16;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x92
	//------------------------------------------------------------
	adrs = 0x92;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x0c
	//============================================================
	id = 0x0c;

	//------------------------------------------------------------
	// Adrs = 0x6a
	//------------------------------------------------------------
	adrs = 0x6a;
	data8 = 0xa5;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6b
	//------------------------------------------------------------
	adrs = 0x6b;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe8
	//------------------------------------------------------------
	adrs = 0xe8;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xe9
	//------------------------------------------------------------
	adrs = 0xe9;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x0d
	//============================================================
	id = 0x0d;

	//------------------------------------------------------------
	// Adrs = 0x8c
	//------------------------------------------------------------
	adrs = 0x8c;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8d
	//------------------------------------------------------------
	adrs = 0x8d;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x10
	//============================================================
	id = 0x10;

	//------------------------------------------------------------
	// Adrs = 0x60
	//------------------------------------------------------------
	adrs = 0x60;
	data8 = 0x1d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x62
	//------------------------------------------------------------
	adrs = 0x62;
	data8 = 0xd1;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf0
	//------------------------------------------------------------
	adrs = 0xf0;
	data8 = 0x18;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xf2
	//------------------------------------------------------------
	adrs = 0xf2;
	data8 = 0xd6;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x11
	//============================================================
	id = 0x11;

	//------------------------------------------------------------
	// Adrs = 0x14
	//------------------------------------------------------------
	adrs = 0x14;
	data8 = 0xD7;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x74
	//------------------------------------------------------------
	adrs = 0x74;
	data8 = 0x88;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x76
	//------------------------------------------------------------
	adrs = 0x76;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0xd7;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x12
	//============================================================
	id = 0x12;

	//------------------------------------------------------------
	// Adrs = 0x18
	//------------------------------------------------------------
	adrs = 0x18;
	data8 = 0x83;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1a
	//------------------------------------------------------------
	adrs = 0x1a;
	data8 = 0x14;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x2a
	//------------------------------------------------------------
	adrs = 0x2a;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2b
	//------------------------------------------------------------
	adrs = 0x2b;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2c
	//------------------------------------------------------------
	adrs = 0x2c;
	data8 = 0xe4;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2d
	//------------------------------------------------------------
	adrs = 0x2d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2e
	//------------------------------------------------------------
	adrs = 0x2e;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x2f
	//------------------------------------------------------------
	adrs = 0x2f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x38
	//------------------------------------------------------------
	adrs = 0x38;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3a
	//------------------------------------------------------------
	adrs = 0x3a;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x3b
	//------------------------------------------------------------
	adrs = 0x3b;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6a
	//------------------------------------------------------------
	adrs = 0x6a;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6b
	//------------------------------------------------------------
	adrs = 0x6b;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6c
	//------------------------------------------------------------
	adrs = 0x6c;
	data8 = 0xe4;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x6d
	//------------------------------------------------------------
	adrs = 0x6d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x78
	//------------------------------------------------------------
	adrs = 0x78;
	data8 = 0x25;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7e
	//------------------------------------------------------------
	adrs = 0x7e;
	data8 = 0x6e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x88
	//------------------------------------------------------------
	adrs = 0x88;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8a
	//------------------------------------------------------------
	adrs = 0x8a;
	data8 = 0xff;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8b
	//------------------------------------------------------------
	adrs = 0x8b;
	data8 = 0x0f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xc8
	//------------------------------------------------------------
	adrs = 0xc8;
	data8 = 0x20;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xca
	//------------------------------------------------------------
	adrs = 0xca;
	data8 = 0x73;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x13
	//============================================================
	id = 0x13;

	//------------------------------------------------------------
	// Adrs = 0xd0
	//------------------------------------------------------------
	adrs = 0xd0;
	data8 = 0x61;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd5
	//------------------------------------------------------------
	adrs = 0xd5;
	data8 = 0x11;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x19
	//============================================================
	id = 0x19;

	//------------------------------------------------------------
	// Adrs = 0x0b
	//------------------------------------------------------------
	adrs = 0x0b;
	data8 = 0x1f;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x84
	//------------------------------------------------------------
	adrs = 0x84;
	data8 = 0x07;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x85
	//------------------------------------------------------------
	adrs = 0x85;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x86
	//------------------------------------------------------------
	adrs = 0x86;
	data8 = 0x0d;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x87
	//------------------------------------------------------------
	adrs = 0x87;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8c
	//------------------------------------------------------------
	adrs = 0x8c;
	data8 = 0x71;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8d
	//------------------------------------------------------------
	adrs = 0x8d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8e
	//------------------------------------------------------------
	adrs = 0x8e;
	data8 = 0x75;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8f
	//------------------------------------------------------------
	adrs = 0x8f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x94
	//------------------------------------------------------------
	adrs = 0x94;
	data8 = 0x06;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x95
	//------------------------------------------------------------
	adrs = 0x95;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x96
	//------------------------------------------------------------
	adrs = 0x96;
	data8 = 0x0e;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x97
	//------------------------------------------------------------
	adrs = 0x97;
	data8 = 0x01;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9c
	//------------------------------------------------------------
	adrs = 0x9c;
	data8 = 0x70;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9d
	//------------------------------------------------------------
	adrs = 0x9d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9e
	//------------------------------------------------------------
	adrs = 0x9e;
	data8 = 0x76;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x9f
	//------------------------------------------------------------
	adrs = 0x9f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	//============================================================
	// Chip ID = 0x1c
	//============================================================
	id = 0x1c;

	//------------------------------------------------------------
	// Adrs = 0x1c
	//------------------------------------------------------------
	adrs = 0x1c;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1d
	//------------------------------------------------------------
	adrs = 0x1d;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1e
	//------------------------------------------------------------
	adrs = 0x1e;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x1f
	//------------------------------------------------------------
	adrs = 0x1f;
	data8 = 0x77;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x24
	//------------------------------------------------------------
	adrs = 0x24;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7c
	//------------------------------------------------------------
	adrs = 0x7c;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7d
	//------------------------------------------------------------
	adrs = 0x7d;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x7e
	//------------------------------------------------------------
	adrs = 0x7e;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x80
	//------------------------------------------------------------
	adrs = 0x80;
	data8 = 0xbc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x81
	//------------------------------------------------------------
	adrs = 0x81;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x82
	//------------------------------------------------------------
	adrs = 0x82;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x8e
	//------------------------------------------------------------
	adrs = 0x8e;
	data8 = 0xe4;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x8f
	//------------------------------------------------------------
	adrs = 0x8f;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0x92
	//------------------------------------------------------------
	adrs = 0x92;
	data8 = 0xe4;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// Adrs = 0x93
	//------------------------------------------------------------
	adrs = 0x93;
	data8 = 0x00;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// Adrs = 0xd1
	//------------------------------------------------------------
	adrs = 0xd1;
	data8 = 0xcc;
	if ((status = sensorRegWriteByte (id, adrs, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}
#endif // #elif defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)


#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
//**********************************************************************************
//	TGES取得(High Speed Mode)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		height				；height
//		pTgpd				；TGESを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetTgesHighSpeed (int height, int *pTges)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check height Parameter
	if ((height != 2) && (height != 4) && (height != 6))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "TGES Get High Speed Height(%d) Parameter Error.\n", height);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	// Check pTges Parameter
	if (pTges == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "TGES Get High Speed NULL Parameter Error.\n");
		goto _DONE;
	}

	if (height == 4)
		*pTges = SENSOR_READOUT_TIME_LNINE4_HIGH_SPEED;
	else if (height == 6)
		*pTges = SENSOR_READOUT_TIME_LNINE6_HIGH_SPEED;
	else
		*pTges = SENSOR_READOUT_TIME_LNINE2_HIGH_SPEED;

_DONE:
	return (status);
}


//**********************************************************************************
//	TGPD取得(High Speed Mode)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		height				；height
//		pTgpd				；TGPDを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetTgpdHighSpeed (int height, int *pTgpd)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check height Parameter
	if ((height != 2) && (height != 4) && (height != 6))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "TGPD Get High Speed Height(%d) Parameter Error.\n");
		goto _DONE;
	}
	
	// Check pTgpd Parameter
	if (pTgpd == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "TGPD Get High Speed NULL Parameter Error.\n");
		goto _DONE;
	}

	if (height == 4)
		*pTgpd = SENSOR_TGPD_TIME_LNINE4_HIGH_SPEED;
	else if (height == 6)
		*pTgpd = SENSOR_TGPD_TIME_LNINE6_HIGH_SPEED;
	else
		*pTgpd = SENSOR_TGPD_TIME_LNINE2_HIGH_SPEED;

_DONE:
	return (status);
}
#endif // #if defined (MODE_SENSOR_IMX990) || defined (MODE_SENSOR_IMX991)
#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)


#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
#if defined (MODE_SENSOR_SHUTTER)
//**********************************************************************************
//	SHS設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		expTime				：露光時間(h単位)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorSetSHS_H (unsigned int expTime)
{
	int status = AVAL_STATUS_SUCCESS;
	int heightTotal;
	unsigned char id;
	int regHold, acTmg;
	unsigned char data8;
	unsigned int shs, shsMin, shsMax;
	unsigned int alfa;
	int vMax;
	int drrsMode = MODE_DISABLE;

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	// Get Height
	if ((status = fpgaRoiGetCameraHeightTotalSize (&heightTotal)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_SENSOR_DRRS)
	// DRRSモード取得
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif
	
	if (drrsMode == MODE_DISABLE)
	{
		// SHS算出
		alfa = SENSOR_VINTERVAL_LINE - CAMERA_HEIGHT_MAX;
		shs = heightTotal + alfa - expTime;
	}
	else
	{
		// SHS算出
		shs = SENSOR_DRRS_VMAX - expTime;
	}

	// 偶数にする
	if ((shs&0x01) != 0)
		shs++;

	#if 0
	//============================================================
	// Chip ID = 0x02
	//============================================================
	id = 0x02;

	//------------------------------------------------------------
	// GTWAIT0
	//------------------------------------------------------------
	data8 = SENSOR_REG_GTWAIT_DEFAULT;
	if ((status = sensorRegReadByte (id, SENSOR_REG_GTWAIT0_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	gtWait = data8;

	//------------------------------------------------------------
	// GTWAIT1
	//------------------------------------------------------------
	if ((status = sensorRegReadByte (id, SENSOR_REG_GTWAIT1_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	gtWait |= (unsigned int)(data8 & SENSOR_REG_GTWAIT1_MASK) << 8;
	#endif

	if (drrsMode == MODE_ENABLE)
		vMax = SENSOR_DRRS_VMAX;
	else
		vMax = SENSOR_VINTERVAL_LINE;
	
	// Check SHS
	shsMin = SENSOR_SHS_MIN_OFFSET;
	shsMax = vMax - SENSOR_SHS_MIN;
	if ((shs < shsMin) || (shs > shsMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Sensor Set SHS(%d) Parameter Error.(Min:%d / Max:%d)\n", shs, shsMin, shsMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	
	//============================================================
	// Chip ID = 0x04
	//============================================================
	id = 0x04;

	//------------------------------------------------------------
	// SHS0
	//------------------------------------------------------------
	data8 = (unsigned char)(shs & 0xff);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_SHS0_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//------------------------------------------------------------
	// SHS1
	//------------------------------------------------------------
	data8 = (unsigned char)((shs >> 8) & 0xff);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_SHS1_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//------------------------------------------------------------
	// SHS2
	//------------------------------------------------------------
	data8 = (unsigned char)((shs >> 16) & 0xff);
	if ((status = sensorRegWriteByte (id, SENSOR_REG_SHS2_ADRS, data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	SHS取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		*pExpTime				：露光時間(h単位)を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorGetSHS_H (unsigned int *pExpTime)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned char id;
	int regHold, acTmg;
	unsigned char data8;
	unsigned int shs;
	unsigned int alfa;
	int heightTotal;
	unsigned int exposureH;
	int drrsMode = MODE_DISABLE;

	// Check pExpTime Parameter
	if (pExpTime == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_SENSOR, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor Get SHS NULL Parameter Error.\n");
		goto _DONE;
	}

	regHold = 0;	// レジスタホールドなし
	acTmg = 0;		// 即時アクセス

	//============================================================
	// Chip ID = 0x04
	//============================================================
	id = 0x04;

	//------------------------------------------------------------
	// SHS0
	//------------------------------------------------------------
	if ((status = sensorRegReadByte (id, SENSOR_REG_SHS0_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	shs = data8 & 0xff;

	//------------------------------------------------------------
	// SHS1
	//------------------------------------------------------------
	if ((status = sensorRegReadByte (id, SENSOR_REG_SHS1_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	shs |= (unsigned int)((data8 & 0xff) << 8);
	
	//------------------------------------------------------------
	// SHS2
	//------------------------------------------------------------
	if ((status = sensorRegReadByte (id, SENSOR_REG_SHS2_ADRS, &data8, acTmg, regHold)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	shs |= (unsigned int)((data8 & 0xff) << 16);

	// Get Height
	if ((status = fpgaRoiGetCameraHeightTotalSize (&heightTotal)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
#if defined (MODE_SENSOR_DRRS)
	// DRRSモード取得
	if ((status = sensorGetDrrs (&drrsMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif
	
	if (drrsMode == MODE_DISABLE)
	{
		// SHS算出
		alfa = SENSOR_VINTERVAL_LINE - CAMERA_HEIGHT_MAX;

		// Exposure H単位
		exposureH = heightTotal + alfa - shs;
	}
	else
	{
		// Exposure H単位
		exposureH = SENSOR_DRRS_VMAX - shs;
	}

	// 露光時間取得
	*pExpTime = exposureH;

_DONE:
	return (status);
}
#endif // #if defined (MODE_SENSOR_SHUTTER)


//**********************************************************************************
//	Sensor Trg Disable
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTrgMode			：Trg Modeを格納するポインタ
//		pHvGenMode			：HV Gen Modeを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorTrgDisable (int *pTrgMode, int *pHvGenMode)
{
	int status = AVAL_STATUS_SUCCESS;

#if !defined (MODE_SENSOR_MASTER)
	// Get Trg Mode
	if ((status = sensorGetTrgControl (pTrgMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Get HV Gen Mode
	if ((status = sensorGetHvGen (pHvGenMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Disable Trg Control
	if ((status = sensorSetTrgControl (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Disable HV Gen
	if ((status = sensorSetHvGen (MODE_DISABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif // #if !defined (MODE_SENSOR_MASTER)
	
_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Trg Enable
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		trgMode			：Trg Mode
//		hvGenMode			：HV Gen Mode
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorTrgEnable (int trgMode, int hvGenMode)
{
	int status = AVAL_STATUS_SUCCESS;

#if !defined (MODE_SENSOR_MASTER)
	// Enable HV Gen
	if (hvGenMode == MODE_ENABLE)
	{
		if ((status = sensorSetHvGen (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Enable Trg Control
	if (trgMode == MODE_ENABLE)
	{
		if ((status = sensorSetTrgControl (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
#endif // #if !defined (MODE_SENSOR_MASTER)
	
_DONE:
	return (status);
}

#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)

// eof
