//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// autoBright.c - Auto Bright Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
//----------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------
#define EXP_KP								(0.9)		//比例ゲイン
#define EXP_KI								(0.2)		//積分ゲイン
#define EXP_KD								(0.025)		//微分ゲイン

#define GAIN_KP								(0.95)		//比例ゲイン
#define GAIN_KI								(0.5)		//積分ゲイン
#define GAIN_KD								(0.03)		//微分ゲイン
// 積分最大値
#define PID_I_MAX							(100)

// 積分間隔
#define PID_D_LOOP							(10)

// PID最大値
#define AUTO_BRIGHT_EXP_PID_VALUE_MAX		(100)
#define AUTO_BRIGHT_EXP_PID_VALUE_MIN		(-100)

#define AUTO_BRIGHT_GAIN_PID_VALUE_MAX		(200)
#define AUTO_BRIGHT_GAIN_PID_VALUE_MIN		(-200)

// 露光時間マージン
#define AUTO_BRIGHT_EXPTIME_MARGIN			(20)

// 目標値と平均値の誤差範囲(12bit)
#define AUTO_BRIGHT_MARGIN_MAX				(64)
#define AUTO_BRIGHT_MARGIN_MIN				(-64)

//露光/ゲイン最小値
#define AUTO_EXP_MIN						(13)
#define AUTO_GAIN_MIN						(1.0)
#define AUTO_GAIN_MAX						(31.0)

//絶対値計算マクロ
#define ABS(v)	((v) >= 0 ? (v) : -(v))

//----------------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------------
int gAutoBrightStatus = 0;
double gExpPid_Derror, gExpPid_Perror, gExpPid_Ierror, prevExp_bright;
int gExpDCount = PID_D_LOOP;
double gGainPid_Derror, gGainPid_Perror, gGainPid_Ierror, prevGain_bright;
int gGainDCount = PID_D_LOOP;
int gGainInitFlag = 0, gExpInitFlag = 0;
int gAutoBrightAveSave = -1;
int gExpMinCount = 0, gExpMaxCount = 0;


//**********************************************************************************
// Auto Bright初期化設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int type;

	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "Auto Bright Initialize\n");

	// Auto機能取得
	if ((status = autoBrightGetFunctionValid (&type)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Auto機能無効?
	if (type == 0)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Register Data Restore
	//--------------------------------------------------------------------------------
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_AUTO_BRIGHT_ADRS, CAMERA_SAVE_AUTO_BRIGHT_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gAutoBrightStatus = status;
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Main
//----------------------------------------------------------------------------------
//	[ INPUT ]
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightMain (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int expMode,gainMode;
	int bit, shift;
	int target, average;
	int brigthMarginMax, brightMarginMin, brightDif;
	float gainF;
	int maxExpTime;
	unsigned int expTime;

	// Auto Exposure Mode取得
	if ((status =  autoBrightGetExposureMode (&expMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Auto Gain Mode取得
	if ((status =  autoBrightGetGainMode (&gainMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Auto Mode Disable?
	if (expMode == MODE_DISABLE && gainMode == MODE_DISABLE)
		goto _DONE;

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Bit Shift数取得
	if ((status = aoiGetShift2 (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 輝度目標値取得
	if ((status =  autoBrightGetTarget (&target)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 輝度平均値取得
	if ((status =  autoBrightGetAverage (&average)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 輝度差を12Bitに変換
	//brightDif >>= shift;

	// 輝度マージン
	brigthMarginMax = AUTO_BRIGHT_MARGIN_MAX >> shift;
	brightMarginMin = AUTO_BRIGHT_MARGIN_MIN >> shift;

	// 輝度差
	brightDif = target - average;

	// 目標値との差は範囲内ならば何もしない
	if ((brightMarginMin <= brightDif) && (brigthMarginMax >= brightDif))
	{
		gExpInitFlag = 0;
		gGainInitFlag = 0;

		if ((status =  autoBrightSetExposureStatus (1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if ((status =  autoBrightSetGainStatus (1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		goto _DONE;
	}

	// 最大露光時間取得
	if ((status =  autoBrightGetExposureMax (&maxExpTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 露光時間取得
	if ((status =  acquisitionGetExposure (&expTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Digital Offset Gain取得
	if ((status = digitalGetGainX (&gainF)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//Auto Gain処理 & Auto Exposure処理
	if (expMode == MODE_ENABLE && gainMode == MODE_ENABLE)
	{
		if(average < target + brightMarginMin )
		{
			if(expTime < (maxExpTime - AUTO_BRIGHT_EXPTIME_MARGIN))
			{
				if((status = autoExposureControl(average, target)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

				if ((status =  autoBrightSetGainStatus (1)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

			}
			else if(expTime >= (maxExpTime - AUTO_BRIGHT_EXPTIME_MARGIN))
			{
				if((status = autoGainControl(average, target))!= AVAL_STATUS_SUCCESS)
						goto _DONE;

				if ((status =  autoBrightSetExposureStatus (1)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

			}
		}
		else if(average > target + brigthMarginMax)
		{
			if(expTime >= (maxExpTime - AUTO_BRIGHT_EXPTIME_MARGIN) && gainF != AUTO_GAIN_MIN)
			{
				if((status = autoGainControl(average, target))!= AVAL_STATUS_SUCCESS)
					goto _DONE;

				if ((status =  autoBrightSetExposureStatus (1)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

			}
			else if(gainF == AUTO_GAIN_MIN)
			{
				if((status = autoExposureControl(average, target)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

				if ((status =  autoBrightSetGainStatus (1)) != AVAL_STATUS_SUCCESS)
					goto _DONE;
			}
		}
	}
	else if(expMode == MODE_ENABLE)	//Auto Exposure処理のみ
	{
		if((status = autoExposureControl(average, target)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if(gainMode == MODE_ENABLE)	// Auto Gain処理のみ
	{
		if((status = autoGainControl(average, target))!= AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	露光時間コントロール
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		average				：平均輝度値
//		target				:目標輝度値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoExposureControl (int average, int target)
{
	int status = AVAL_STATUS_SUCCESS;
	double pid,expPid;
	int expTimeMax;
	unsigned int expTime, newExpTime;

	// 露光時間最大値取得
	if ((status =  autoBrightGetExposureMax (&expTimeMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 露光時間取得
	if ((status =  acquisitionGetExposure (&expTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//露光時間PID制御
	if((status = autoExposurePid(average, target, &pid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//sprintf (gLogMsgBuff, "Auto Exp PID value = %lf \n", pid);
	//cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

	//PIDの値が閾値異常なら
	if(pid > AUTO_BRIGHT_EXP_PID_VALUE_MAX || pid < AUTO_BRIGHT_EXP_PID_VALUE_MIN)
	{
		gExpInitFlag = 0;
		goto _DONE;
	}

	//露光時間計算
	expPid = ABS(expTime * (pid / 100));
	//expPid = expTime + ABS(pid);

	// 輝度値Check
	if (pid > 0)
		newExpTime = expTime - expPid;		// 暗くする
	else
		newExpTime = expTime + expPid;		// 明るくする

	// 最小露光時間Check
	if (newExpTime < AUTO_EXP_MIN)
		newExpTime = AUTO_EXP_MIN;

	// 最大露光時間Check
	if (newExpTime > expTimeMax)
		newExpTime = expTimeMax;

	//最小露光時間設定時でも暗くならなかったらInit
	if(pid > 0 && newExpTime == AUTO_EXP_MIN)
	{
		gExpInitFlag = 0;
		//goto _DONE;
	}

	//sprintf (gLogMsgBuff, "Auto Exp value = %d \n", newExpTime);
	//cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	
	// 露光時間設定
	if ((status = acquisitionSetExposureSimple (newExpTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status =  autoBrightSetExposureStatus (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	デジタルゲインコントロール
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		average				：平均輝度値
//		target				:目標輝度値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoGainControl (int average, int target)
{
	int status = AVAL_STATUS_SUCCESS;
	double pid,gainPid;
	float gainF, newGainF;
	double gainMax;

	// Digital Offset Gain Max取得
	if ((status = autoBrightGetGainMax (&gainMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if((status = autoGainPid(average, target, &pid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//sprintf (gLogMsgBuff, "Auto Gain PID value = %lf \n", pid);
	//cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

	//PIDの値が閾値異常なら
	if(pid > AUTO_BRIGHT_GAIN_PID_VALUE_MAX || pid < AUTO_BRIGHT_GAIN_PID_VALUE_MIN)
	{
		gGainInitFlag = 0;
		goto _DONE;
	}

	// Digital Offset Gain取得
	if ((status = digitalGetGainX (&gainF)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	gainPid = ABS(gainF * (pid / 100));

	// 輝度値Check
	if (pid > 0)
		newGainF = gainF - gainPid;		// 暗くする
	else
		newGainF = gainF + gainPid;		// 明るくする

	// 最小ゲインCheck
	if (newGainF < AUTO_GAIN_MIN)
		newGainF = AUTO_GAIN_MIN;

	// 最大ゲインCheck
	if (newGainF > gainMax)
		newGainF = gainMax;

	//最小露光時間設定時でも暗くならなかったらInit
	if(pid > 0 && newGainF == AUTO_GAIN_MIN)
	{
		gGainInitFlag = 0;
		//goto _DONE;
	}

	//sprintf (gLogMsgBuff,  "Auto Gain value = %f \n", newGainF);
	//cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
	
	// Digital Offset Gain設定
	if ((status = digitalSetGainX (newGainF)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Gainの最小に達したので、Auto Exposure再起動。
	if ((status =  autoBrightSetGainStatus (0)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}

//**********************************************************************************
//	露光時間PID(輝度値を取得し、露光時間制御する)（現在の設定値に対して ± XX パーセントする）
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bright				:平均輝度値
//		target				:目標輝度値
//		pPID				：PID格納用ポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoExposurePid (int bright, int target, double *pPID)
{
	int status = AVAL_STATUS_SUCCESS;
	double data;
	int maxBright ,bit ,shift;

	// Check pPID Parameter
	if (pPID == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Exp PID NULL Parameter Error.\n");
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Bit Shift数取得
	if ((status = aoiGetShift2 (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//最大輝度値取得
	maxBright = (1 << bit) - 1;

	// パーセント
	data = (double)(bright - target) / maxBright * 100;

	 // 比例(目標値と取得値の差分)
	gExpPid_Perror = data;

	// 積分(差分の累積)
	gExpPid_Ierror += gExpPid_Perror;

	// 最大値Check
	if (gExpPid_Ierror > PID_I_MAX)
		gExpPid_Ierror = PID_I_MAX;

	if (gExpInitFlag == 0)
	{
		gExpInitFlag = 1;

		// 前回の偏差と今回の偏差の差
		gExpPid_Derror = 0;

		// 今回の偏差を保存
		prevExp_bright = 0;

		gExpPid_Perror = 0;

		gExpPid_Ierror = 0;
	}
	else
	{
		gExpDCount--;
		if (gExpDCount == 0)
		{
			gExpPid_Derror = prevExp_bright - gExpPid_Perror;	// 前回の偏差と今回の偏差の差

			prevExp_bright = gExpPid_Derror;					// 今回の偏差を保存

			gExpDCount = PID_D_LOOP;							// 積分間隔
		}
	}

	// PID計算(比例P＋積分I＋微分D)
	*pPID = EXP_KP * gExpPid_Perror + EXP_KI * gExpPid_Ierror + EXP_KD * gExpPid_Derror;

_DONE:
	return (status);
}


//**********************************************************************************
// ゲインPID(輝度値を取得し、ゲイン制御する)（現在の設定値に対して ± XX パーセントする）
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		bright				:平均輝度値
//		target				:目標輝度値
//		pPID				：PID格納用ポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoGainPid (int bright, int target, double *pPID)
{
	int status = AVAL_STATUS_SUCCESS;
	double data;
	int maxBright ,bit ,shift;
	//static int initFlag = 0;

	// Check pPID Parameter
	if (pPID == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Ex PID NULL Parameter Error.\n");
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Bit Shift数取得
	if ((status = aoiGetShift2 (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//最大輝度値取得
	maxBright = (1 << bit) - 1;

	// パーセント
	data = (double)(bright - target) / maxBright * 100;

	 // 比例(目標値と取得値の差分)
	gGainPid_Perror = data;

	// 積分(差分の累積)
	gGainPid_Ierror += gGainPid_Perror;

	// 最大値Check
	if (gGainPid_Ierror > PID_I_MAX)
		gGainPid_Ierror = PID_I_MAX;

	if (gGainInitFlag == 0)
	{
		gGainInitFlag = 1;

		// 前回の偏差と今回の偏差の差
		gGainPid_Derror = 0;

		// 今回の偏差を保存
		prevGain_bright = 0;

		gGainPid_Perror = 0;

		gGainPid_Ierror = 0;
	}
	else
	{
		gGainDCount--;
		if (gGainDCount == 0)
		{
			gGainPid_Derror = prevGain_bright - gGainPid_Perror;	// 前回の偏差と今回の偏差の差

			prevGain_bright = gGainPid_Derror;						// 今回の偏差を保存

			gGainDCount = PID_D_LOOP;								// 積分間隔
		}
	}

	// PID計算(比例P＋積分I＋微分D)
	*pPID = GAIN_KP * gGainPid_Perror + GAIN_KI * gGainPid_Ierror + GAIN_KD * gGainPid_Derror;


_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Information
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightInformation (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int mode;
	int data1, data2;
	double dataD2;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("[Auto Bright informations]\n");

	//------------------------------------------------------------
	// Auto Exposure Mode取得
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Auto Exposure Mode");
	if ((status =  autoBrightGetExposureMode (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == MODE_DISABLE)
		DEBUG_PRINT_FORCE ("        Disable\n");
	else
		DEBUG_PRINT_FORCE ("        Enable\n");


	//------------------------------------------------------------
	// Auto Exposure Once Status取得
	//------------------------------------------------------------
	if ((status =  autoBrightGetExposureStatus (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Exposure Status           %d\n", mode);


	//------------------------------------------------------------
	// Auto Exposure Min/Max取得
	//------------------------------------------------------------
	//if ((status =  autoBrightGetExposureMin (&data1)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	if ((status =  autoBrightGetExposureMax (&data2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Exposure Minimum          %d\n", AUTO_EXP_MIN);
	DEBUG_PRINT_FORCE ("   Exposure Maximum          %d\n", data2);


	//------------------------------------------------------------
	// Auto Gain Mode取得
	//------------------------------------------------------------
	DEBUG_PRINT_FORCE ("   Auto Gain Mode");
	if ((status =  autoBrightGetGainMode (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode == MODE_DISABLE)
		DEBUG_PRINT_FORCE ("            Disable\n");
	else
		DEBUG_PRINT_FORCE ("            Enable\n");


	//------------------------------------------------------------
	// Auto Gain Once Status取得
	//------------------------------------------------------------
	if ((status =  autoBrightGetGainStatus (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Gain Status               %d\n", mode);
 

	//------------------------------------------------------------
	// Auto Gain Min/Max取得
	//------------------------------------------------------------
	//if ((status =  autoBrightGetGainMin (&dataD1)) != AVAL_STATUS_SUCCESS)
		//goto _DONE;

	if ((status =  autoBrightGetGainMax (&dataD2)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Gain Minimum              %.2f\n", AUTO_GAIN_MIN);
	DEBUG_PRINT_FORCE ("   Gain Maximum              %.2f\n", dataD2);


	//------------------------------------------------------------
	// Auto Bright Target取得
	//------------------------------------------------------------
	if ((status =  autoBrightGetTarget (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Target                    %d\n", mode);

	//------------------------------------------------------------
	// Detect Area取得
	//------------------------------------------------------------
	if ((status =  autoBrightGetDetectArea (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Detect Area               %d\n", mode);


	//------------------------------------------------------------
	// Detect Area Width Offset取得
	//------------------------------------------------------------
	if ((status =  autoBrightGetWidthOffset (&data1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Width Offset              %d\n", data1);


	//------------------------------------------------------------
	// Detect Area Width取得
	//------------------------------------------------------------
	if ((status =  autoBrightGetWidthSize (&data1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Width Size                %d\n", data1);


	//------------------------------------------------------------
	// Detect Area Height Offset取得
	//------------------------------------------------------------
	if ((status =  autoBrightGetHeightOffset (&data1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Height Offset             %d\n", data1);


	//------------------------------------------------------------
	// Detect Area Height取得
	//------------------------------------------------------------
	if ((status =  autoBrightGetHeightSize (&data1)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Height Size               %d\n", data1);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Function Valid取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pValid				：validを格納するポインタ(0=無効/1=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetFunctionValid (int *pValid)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pValid Parameter
	if (pValid == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Get Valid Function pValid NULL Parameter Error.\n");
		goto _DONE;
	}

	// Auto Bright Valid取得
	*pValid = IN32 (FPGA_AUTO_BRIGHT_TYPE_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Function Valid Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightCheckFunctionValid (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int valid;

	// Function Valid取得
	if ((status = autoBrightGetFunctionValid (&valid)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Valid
	if (valid == MODE_DISABLE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_NOT_IMPLEMENTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_NOT_SUPPORT);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Exposure Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=無効/1=Once/2=Continue
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetExposureMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int expMode;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check mode Parameter
	if ((mode < MODE_DISABLE) || (mode > MODE_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Exposure Set Mode(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 露光モード取得
	if ((status = acquisitionGetExposureMode (&expMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 露光モードはTimed?
	if (expMode != ACQUISITION_EXPOSURE_TIMED)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Exposure Set Mode Exposure Mode Not Timed Error.\n");
		goto _DONE;
	}

	// Auto Bright Exposure Mode取得
	data32 = IN32 (FPGA_AUTO_BRIGHT_CTRL_ADRS) & ~FPGA_AUTO_EXP_CTRL_ENABLE_BIT;

	if (mode == MODE_ENABLE)
	{
		gExpInitFlag = 0;
		// Mode設定
		data32 |= FPGA_AUTO_EXP_CTRL_ENABLE_BIT;
	}

	// Set Auto Bright Exposure Mode設定(FPGA ※Enable/Disableの制御)
	OUT32 (FPGA_AUTO_BRIGHT_CTRL_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Exposure Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Auto Bright Exposure Modeを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetExposureMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Exposure Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Auto Bright Exposure Mode取得
	*pMode = IN32 (FPGA_AUTO_BRIGHT_CTRL_ADRS) & FPGA_AUTO_EXP_CTRL_ENABLE_BIT;

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Exposure Once Status設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Status(0 = Action, 1 = Finish)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetExposureStatus (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Auto Bright Once Exposure Status設定
	OUT32 (FIRM_DATA_AUTO_BRIGHT_EXP_STATUS_ADRS, mode);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Exposure Once Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Auto Bright Once Statusを格納するポインタ(0 = Action, 1 = Finish)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetExposureStatus (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Exposure Once Status pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Auto Bright Once Exposure Status取得
	*pMode = IN32 (FIRM_DATA_AUTO_BRIGHT_EXP_STATUS_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Exposure Max設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		exp			：Auto Exposureで変更される露光時間の最大値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetExposureMax (int exp)
{
	int status = AVAL_STATUS_SUCCESS;
	int getExpTimeMin, getExpTimeMax;
	int mode;
	double rateD;
	int readOutTime, currentRateTime;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 最小露光時間取得
	if ((status = exposureMin (&getExpTimeMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレート取得
	if ((status = acquisitionGetFrameRate (&rateD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// センサRead Out Time取得
	if ((status = sensoreGetReadOut (&readOutTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレートを時間に変換
	currentRateTime = (int)((1.0 / rateD) * ACQUISITION_FRAMERATE_UINIT);

	// 今回のフレームレートで設定できる最大露光時間
	getExpTimeMax = currentRateTime - readOutTime;

	// Check exp Parameter
	if ((exp < getExpTimeMin) || (exp > getExpTimeMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Set Exposure Max(%d) Parameter Error.(Min:%d / Max:%d)\n", exp, getExpTimeMin, getExpTimeMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if((status = autoBrightGetExposureMode(&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//自動露光が有効ならば
	if(mode == MODE_ENABLE)
	{
		// 露光時間設定
		if ((status = acquisitionSetExposure(exp)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// Set Auto Bright Exposure Max設定
	OUT32 (FIRM_DATA_AUTO_BRIGHT_EXPOSUER_MAX, exp);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Exposure Min/Max Check
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		exp			：Auto Exposureで変更される露光時間の最大値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightCheckExposureMinMax (int exp)
{
	int status = AVAL_STATUS_SUCCESS;
	int getExpTimeMin, getExpTimeMax;
	double rateD;
	int readOutTime, currentRateTime;

	// 最小露光時間取得
	if ((status = exposureMin (&getExpTimeMin)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレート取得
	if ((status = acquisitionGetFrameRate (&rateD)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// センサRead Out Time取得
	if ((status = sensoreGetReadOut (&readOutTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// フレームレートを時間に変換
	currentRateTime = (int)((1.0 / rateD) * ACQUISITION_FRAMERATE_UINIT);

	// 今回のフレームレートで設定できる最大露光時間
	getExpTimeMax = currentRateTime - readOutTime;

	// Check exp Parameter
	if ((exp < getExpTimeMin) || (exp > getExpTimeMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Exposure Max取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pExp			：Auto Bright 最大露光時間を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetExposureMax (int *pExp)
{
	int status = AVAL_STATUS_SUCCESS;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pExp Parameter
	if (pExp == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Exposure Max pExp NULL Parameter Error.\n");
		goto _DONE;
	}

	// Auto Bright Exposure Max取得
	*pExp = IN32 (FIRM_DATA_AUTO_BRIGHT_EXPOSUER_MAX);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Gain Mode設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=無効/1=Once/2=Continue
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetGainMode (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	//int onceStatus;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check mode Parameter
	if ((mode < MODE_DISABLE) || (mode > MODE_ENABLE))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Gain Set Mode(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, MODE_DISABLE, MODE_ENABLE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Auto Bright Gain Mode取得
	data32 = IN32 (FPGA_AUTO_BRIGHT_CTRL_ADRS) & ~FPGA_AUTO_GAIN_CTRL_ENABLE_BIT;

	if (mode == MODE_ENABLE)
	{
		gGainInitFlag = 0;
		// Mode設定
		data32 |= FPGA_AUTO_GAIN_CTRL_ENABLE_BIT;
	}

	// Set Auto Bright Gain Mode設定(FPGA ※Enable/Disableの制御)
	OUT32 (FPGA_AUTO_BRIGHT_CTRL_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Gain Mode取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Auto Bright Gain Modeを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetGainMode (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int data32;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Gain Mode pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Auto Bright Gain Mode取得
	data32 = IN32 (FPGA_AUTO_BRIGHT_CTRL_ADRS);

	*pMode = FPGA_AUTO_BRIGHT_CTRL_MODE_GET(data32);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Gain Once Status設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：Once Status(0 = Action, 1 = Finish)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetGainStatus (int mode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Auto Bright Once Gain Status設定
	OUT32 (FIRM_DATA_AUTO_BRIGHT_GAIN_STATUS_ADRS, mode);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Gain Once Status取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Auto Bright Once Statusを格納するポインタ(0 = Action, 1 = Finish)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetGainStatus (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Gain Once Status pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	// Auto Bright Gain Once Status取得
	*pMode = IN32 (FIRM_DATA_AUTO_BRIGHT_GAIN_STATUS_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Gain Max設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		gain			：Auto Gain変更されるGainの最大値(倍率)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetGainMax (double gain)
{
	int status = AVAL_STATUS_SUCCESS;
	int mode;
	unsigned int data32;
	//double gainTemp;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check gainTemp Parameter
	if ((gain < DOG_GAIN_MIN) || (gain > DOG_GAIN_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Set Gain Max(%lf) Parameter Error.(Min:%lf / Max:%lf)\n", gain, AUTO_GAIN_MIN, AUTO_GAIN_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if((status = autoBrightGetGainMode(&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	//自動ゲインが有効ならば
	if(mode == MODE_ENABLE)
	{
		if((status = digitalSetGainX(gain)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}

	// 少数点第1位以下を削除
	data32 = (unsigned int)(gain * AUTO_GAIN_UNIT);

	// Set Auto Bright Gain Max設定
	OUT32 (FIRM_DATA_AUTO_BRIGHT_GAIN_MAX, data32);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Gain Max取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pGain			：Auto Bright Max Gainを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetGainMax (double *pGain)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pGain Parameter
	if (pGain == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Gain Max pGain NULL Parameter Error.\n");
		goto _DONE;
	}

	// Auto Bright Gain Max取得
	data32 = IN32 (FIRM_DATA_AUTO_BRIGHT_GAIN_MAX);
	*pGain = (double)((double)data32 / (double)AUTO_GAIN_UNIT);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Detect Area有効／無効設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：0=無効/1=有効
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetDetectArea (int mode)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check mode Parameter
	if ((mode < FPGA_AUTO_BRIGHT_CTRL_OVERLAY_MIN) || (mode > FPGA_AUTO_BRIGHT_CTRL_OVERLAY_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Set Area(%d) Parameter Error.(Min:%d / Max:%d)\n", mode, FPGA_AUTO_BRIGHT_CTRL_OVERLAY_MIN, FPGA_AUTO_BRIGHT_CTRL_OVERLAY_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Auto Bright Overlay取得
	data32 = IN32 (FPGA_AUTO_BRIGHT_CTRL_ADRS) & ~FPGA_AUTO_BRIHT_CTRL_OVERLAY_MASK;
	data32 |= FPGA_AUTO_BRIGHT_CTRL_OVERLAY_SET (mode);

	// Auto Bright Overlay設定
	OUT32 (FPGA_AUTO_BRIGHT_CTRL_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Detect Area有効／無効取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pMode				：Auto Bright Modeを格納するポインタ(0=無効/1=有効)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetDetectArea (int *pMode)
{
	int status = AVAL_STATUS_SUCCESS;
	int data32;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pMode Parameter
	if (pMode == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Get Detect Area pMode NULL Parameter Error.\n");
		goto _DONE;
	}

	data32 = IN32(FPGA_AUTO_BRIGHT_CTRL_ADRS);

	// Auto Bright Overlay取得
	*pMode = FPGA_AUTO_BRIGHT_CTRL_OVERLAY_GET(data32);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Target設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		target				：目標値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetTarget (int target)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int targetMax;
	int bit;
	int shift;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Target Max
	targetMax = (1 << bit) - 1;

	// Check target Parameter
	if (target < 0 || target > targetMax)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Set Target(%d) Parameter Error. (Min:0 / Max:%d)\n", target, targetMax);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if(target > targetMax)
	{
		target = targetMax;
	}

	//PIDフラグ初期化
	gGainInitFlag = 0;
	gExpInitFlag = 0;

	// Bit Shift数取得
	if ((status = aoiGetShift2 (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 12Bitに調整
	target <<= shift;

	// Auto Bright Target取得
	data32 = IN32 (FPGA_AUTO_BRIGHT_CTRL_ADRS) & ~FPGA_AUTO_BRIGHT_CTRL_TARGET_MASK;
	data32 |= FPGA_AUTO_BRIGHT_CTRL_TARGET_SET(target);
 
	// Auto Bright Target設定(レジスタは12Bit固定)
	OUT32 (FPGA_AUTO_BRIGHT_CTRL_ADRS, data32);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Target取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pTarget				：Targetを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetTarget (int *pTarget)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int bit, shift;
	unsigned int data12Bit;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pTarget Parameter
	if (pTarget == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Get Target pTarget NULL Parameter Error.\n");
		goto _DONE;
	}

	// Auto Bright Target取得
	data32 = IN32 (FPGA_AUTO_BRIGHT_CTRL_ADRS);
	data12Bit = FPGA_AUTO_BRIGHT_CTRL_TARGET_GET(data32);

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Bit Shift数取得
	if ((status = aoiGetShift2 (bit, &shift)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 目標値設定
	*pTarget = data12Bit >> shift;

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Average取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：平均輝度値を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetAverage (int *pData)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
#if defined(MODE_BINNING)
	int xMode = MODE_DISABLE;
	int yMode = MODE_DISABLE;
	int bMode = FPGA_BINNING_MODE_SUM;
	int bit, targetMax;
#endif
	int widthSize, heightSize;
	int temp32;
	unsigned int pixelCount;
	int averageShift;
	double dataDouble;
	unsigned int blackTarget;
	int calc;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Get Average pData NULL Parameter Error.\n");
		goto _DONE;
	}

	// Bit数取得
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Averageは12bitデータで格納されているので、bitの状態に合わせる
	averageShift = 12 - bit;

	// Average取得
	data32 = IN32 (FPGA_AUTO_BRIGHT_AVERAGE_ADRS);

	// Width Size取得
	temp32 = IN32 (FPGA_AUTO_BRIGHT_SIZE_ADRS);
	widthSize = FPGA_AUTO_BRIGHT_SIZEX_GET (temp32);

	// Height Size取得
	temp32 = IN32 (FPGA_AUTO_BRIGHT_SIZE_ADRS);
	heightSize = FPGA_AUTO_BRIGHT_SIZEY_GET (temp32);

	// Pixel Count
	pixelCount = widthSize * heightSize;

	dataDouble = (double)data32 / (double)pixelCount * 8.0;
	if (averageShift != 0)
		dataDouble /= (1 << averageShift);

#if defined(MODE_BINNING)

	// Target Max
	targetMax = (1 << bit) - 1;

	if ((status = aoiGetBinningX (&xMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = aoiGetBinningY (&yMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = aoiGetBinningMode (&bMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;


	if((xMode != BINNING1) || (yMode != BINNING1))
	{
		if (bMode == FPGA_BINNING_MODE_SUM)
		{
			// Black Target取得
			if ((status = ffcGetBlackTarget (&blackTarget)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Bit数を考慮
			if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Bit数を考慮
			blackTarget /= calc;

			// Binning乗算
			dataDouble *= (xMode * yMode);

			// 上記計算だと黒レベルも乗算している為、１画素分の黒レベルだけを含めるように考慮
			dataDouble -= (blackTarget * ((xMode * yMode) - 1));
		}
	}

	if(dataDouble > targetMax)
		dataDouble = targetMax;

	if(dataDouble < 0)
		dataDouble = blackTarget;

#endif // #if defined(MODE_BINNING)

	// 平均値設定
	*pData = (int)dataDouble;

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Widthサイズ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：Widthサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetWidthSize (int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int offset;
	int widthMax, widthMin;
	unsigned int data32;
	int startFlag = 0;
	int mode = MODE_DISABLE;
	int binningNum;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Width Max
	widthMax = WidthMax ();

	// Width Min
	widthMin = WidthMin ();

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningX (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if (mode != MODE_DISABLE)
	{
		size *= mode;
	}
#endif

	if (mode == MODE_DISABLE)
	{
		binningNum = 1;
	}
	else
	{
		binningNum = mode;
	}

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		// Check size Parameter
		if ((size < CXP_WIDTH_MIN) || (size > widthMax))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Auto Bright Width Size(%d) Parameter Error. (Min:%d / Max:%d)\n",size/binningNum, CXP_WIDTH_MIN, widthMax/binningNum);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
	else
#endif // #if defined (MODE_CXP)
	{
		// Check size Parameter
		if ((size < widthMin) || (size > widthMax))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Auto Bright Width Size(%d) Parameter Error. (Min:%d / Max:%d)\n",size/binningNum, widthMin, widthMax/binningNum);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

	// Check size Align Parameters
	if ((size % ROI_FPGA_WIDTH_SIZE_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Width Set Size Align(%d) Parameter Error. Align = %d.\n", size / binningNum, ROI_FPGA_WIDTH_SIZE_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Width Offset取得
	if ((status = autoBrightGetWidthOffset (&offset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Offset
	if ((size + offset) > widthMax)
	{
		// Adjust Width Offset
		offset = widthMax - size;

		// Width Offset設定
		data32 = IN32 (FPGA_AUTO_BRIGHT_OFFSET_ADRS) & ~FPGA_AUTO_BRIGHT_OFFSETX_MASK;
		data32 |= FPGA_AUTO_BRIGHT_OFFSETX_SET (offset);
		OUT32 (FPGA_AUTO_BRIGHT_OFFSET_ADRS, data32);
	}

	// Width Size設定
	data32 = IN32 (FPGA_AUTO_BRIGHT_SIZE_ADRS) & ~FPGA_AUTO_BRIGHT_SIZEX_MASK;
	data32 |= FPGA_AUTO_BRIGHT_SIZEX_SET (size);
	OUT32 (FPGA_AUTO_BRIGHT_SIZE_ADRS, data32);

_DONE:
	if (startFlag != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// Auto Bright Widthサイズ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSize				：Widthサイズを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetWidthSize (int *pSize)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int size;
#if defined(MODE_BINNING)
	int mode = MODE_DISABLE;
#endif

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Get Width Size pSize NULL Parameter Error.\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Width Size取得
	data32 = IN32 (FPGA_AUTO_BRIGHT_SIZE_ADRS);
	size = FPGA_AUTO_BRIGHT_SIZEX_GET (data32);

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningX (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		// 以下はスルー
		if(size <= (mode-1))
			goto _DONE;

		if((size % mode) == 0)
			size /= mode;
		else
			size = (size & ~(mode -1))  / mode;
	}
#endif

	*pSize = size;

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Heightサイズ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：Heightサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetHeightSize (int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int heightMax, heightMin;
	unsigned int data32;
	int offset;
	int startFlag = 0;
	int mode = MODE_DISABLE;
	int binningNum;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	heightMax = HeightMax ();

	// Height Min
	heightMin = 0;

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		size *= mode;
	}
#endif

	if (mode == MODE_DISABLE)
	{
		binningNum = 1;
	}
	else
	{
		binningNum = mode;
	}

	// Check size Parameter
	if ((size < heightMin) || (size > heightMax))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Height Size(%d) Parameter Error.(Min:%d / Max:%d)\n", size / binningNum, heightMin, heightMax / binningNum);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Height Ofset取得
	if ((status = autoBrightGetHeightOffset (&offset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Offset
	if ((size + offset) > heightMax)
	{
		// Adjust Height Offset
		offset = heightMax - size;

		// Height Offset設定
		data32 = IN32 (FPGA_AUTO_BRIGHT_OFFSET_ADRS) & ~(FPGA_AUTO_BRIGHT_OFFSETY_MASK);
		data32 |= FPGA_AUTO_BRIGHT_OFFSETY_SET (offset);
		OUT32 (FPGA_AUTO_BRIGHT_OFFSET_ADRS, data32);
	}

	// Height Size設定
	data32 = IN32 (FPGA_AUTO_BRIGHT_SIZE_ADRS) & ~(FPGA_AUTO_BRIGHT_SIZEY_MASK);
	data32 |= FPGA_AUTO_BRIGHT_SIZEY_SET (size);
	OUT32 (FPGA_AUTO_BRIGHT_SIZE_ADRS, data32);

_DONE:
	if (startFlag != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// Auto Bright Heightサイズ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSize				：Widthサイズを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetHeightSize (int *pSize)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int size;
#if defined(MODE_BINNING)
	int mode = MODE_DISABLE;
#endif

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pSize Parameter
	if (pSize == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Get Height Size pSize NULL Parameter Error.\n");
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Height Size取得
	data32 = IN32 (FPGA_AUTO_BRIGHT_SIZE_ADRS);
	size = FPGA_AUTO_BRIGHT_SIZEY_GET (data32);

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		// 以下はスルー
		if(size <= (mode-1))
			goto _DONE;

		if((size % mode) == 0)
			size /= mode;
		else
			size = (size & ~(mode -1)) / 2;
	}
#endif

	*pSize = size;

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Widthオフセット設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：Widthオフセット
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightSetWidthOffset (int offset)
{
	int status = AVAL_STATUS_SUCCESS;
	int widthMax, widthMin;
	int widthSize;
	unsigned int data32;
	int startFlag = 0;
	int mode = MODE_DISABLE;
	int binningNum;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Width Max
	widthMax = WidthMax ();

	// Width Min
	widthMin = 0;

	// Width Size取得
	if ((status = autoBrightGetWidthSize (&widthSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningX (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		offset *= mode;
		widthSize *= mode;
	}
#endif

	if (mode == MODE_DISABLE)
	{
		binningNum = 1;
	}
	else
	{
		binningNum = mode;
	}

#if defined (MODE_CXP)
	// Check offset Parameter
	if (gInterFaceID == INTERFACE_CXP)
	{
		if ((offset < 0) || (offset > widthMax-CXP_WIDTH_MIN))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Auto Bright Width Offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset / binningNum, widthMin, (widthMax / binningNum) - CXP_WIDTH_MIN);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}
	else
#endif
	{
		// Check offset Parameter
		if ((offset < widthMin) || (offset  > (widthMax - ROI_WIDTH_ALIGH)))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Auto Bright Width Offset(%d) Parameter Error.(Min:%d / Max:%d)\n", offset / binningNum, widthMin, (widthMax / binningNum) - ROI_WIDTH_ALIGH);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}
	}

	// Check offset Align Parameters
	if ((offset % ROI_FPGA_WIDTH_OFFSET_ALIGH) != 0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Width Set Offset Align(%d) Parameter Error. Align = %d.\n", offset / binningNum, ROI_FPGA_WIDTH_OFFSET_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Offset Parameter
	if ((offset + widthSize) > widthMax)
	{
		// Adjust Width Size
		widthSize = widthMax - offset;

		data32 = IN32 (FPGA_AUTO_BRIGHT_SIZE_ADRS) & ~FPGA_AUTO_BRIGHT_SIZEX_MASK;
		data32 |= FPGA_AUTO_BRIGHT_SIZEX_SET (widthSize);
		OUT32 (FPGA_AUTO_BRIGHT_SIZE_ADRS, data32);
	}

	// Width Offset
	data32 = IN32 (FPGA_AUTO_BRIGHT_OFFSET_ADRS) & ~FPGA_AUTO_BRIGHT_OFFSETX_MASK;
	data32 |= FPGA_AUTO_BRIGHT_OFFSETX_SET (offset);
	OUT32 (FPGA_AUTO_BRIGHT_OFFSET_ADRS, data32);

_DONE:
	if (startFlag != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// Auto Bright Witdthオフセット取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：Witdhオフセットを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetWidthOffset (int *pOffset)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int tmpOffset;
#if defined(MODE_BINNING)
	int mode = MODE_DISABLE;
#endif

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Get Width Offset pOffset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Width Offset取得
	data32 = IN32 (FPGA_AUTO_BRIGHT_OFFSET_ADRS);
	tmpOffset = FPGA_AUTO_BRIGHT_OFFSETX_GET (data32);

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningX (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		if(tmpOffset <= 0)
			tmpOffset = 0;
		else
			tmpOffset /= mode;
	}
#endif

	*pOffset = tmpOffset;

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright Heightオフセット設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size					：Heightオフセット
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int autoBrightSetHeightOffset (int offset)
{
	int status = AVAL_STATUS_SUCCESS;
	int heightMax, heightMin;
	int heightSize;
	unsigned int data32;
	int startFlag = 0;
	int mode = MODE_DISABLE;
	int binningNum;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Max
	heightMax = HeightMax ();

	// Height Min
	heightMin = 0;

	// Height Size取得
	if ((status = autoBrightGetHeightSize (&heightSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined(MODE_BINNING)
	if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		offset *= mode;
		heightSize *= mode;
	}
#endif

	if (mode == MODE_DISABLE)
	{
		binningNum = 1;
	}
	else
	{
		binningNum = mode;
	}

	// 撮像状態を取得
	acquisitionGetStartFlag (&startFlag);

	// 取り込み停止
	if ((status = acquisitionAbort ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Offset Parameter
	if ((offset < heightMin) || (offset > (heightMax - ROI_HEIGHT_ALIGH)))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Height Offset(%d) Parameter Error. (Min:%d / Max:%d)\n", offset / binningNum, heightMin, (heightMax / binningNum) - ROI_HEIGHT_ALIGH);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check Offset Parameter
	if ((offset + heightSize) > heightMax)
		heightSize = heightMax - offset;

	// Adjust Width Size
	data32 = IN32 (FPGA_AUTO_BRIGHT_SIZE_ADRS) & ~FPGA_AUTO_BRIGHT_SIZEY_MASK;
	data32 |= FPGA_AUTO_BRIGHT_SIZEY_SET (heightSize);
	OUT32 (FPGA_AUTO_BRIGHT_SIZE_ADRS, data32);

	// Height offset
	data32 = IN32 (FPGA_AUTO_BRIGHT_OFFSET_ADRS) & ~FPGA_AUTO_BRIGHT_SIZEY_MASK;
	data32 |= FPGA_AUTO_BRIGHT_OFFSETY_SET (offset);
	OUT32 (FPGA_AUTO_BRIGHT_OFFSET_ADRS, data32);

_DONE:
	if (startFlag != 0)
		acquisitionStart ();

	return (status);
}


//**********************************************************************************
// Auto Bright Heightオフセット取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：Heightオフセットを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetHeightOffset (int *pOffset)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data32;
	int tmpOffset;

#if defined(MODE_BINNING)
	int mode = MODE_DISABLE;
#endif

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pOffset Parameter
	if (pOffset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Get Height Offset pOffset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Height Offset Size
	data32 = IN32 (FPGA_AUTO_BRIGHT_OFFSET_ADRS);
	tmpOffset = FPGA_AUTO_BRIGHT_OFFSETY_GET(data32);


#if defined(MODE_BINNING)
	if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if(mode != MODE_DISABLE)
	{
		if(tmpOffset <= 0)
			tmpOffset = 0;
		else
			tmpOffset /= mode;
	}
#endif

	*pOffset = tmpOffset;

_DONE:
	return (status);
}

//**********************************************************************************
// Auto Bright 指定範囲設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size					：指定範囲
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int autoBrightSetTargetArea (int area)
{
	int status = AVAL_STATUS_SUCCESS;
	int width, height, xSize, ySize, xOffset, yOffset;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check Area Parameter
	if ((area < AUTO_BRIGHT_TARGET_AREA_MIN) || (area > AUTO_BRIGHT_TARGET_AREA_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Auto Bright Target Area(%d) Parameter Error.(Min:%d / Max:%d)\n", area, AUTO_BRIGHT_TARGET_AREA_MIN,AUTO_BRIGHT_TARGET_AREA_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	if((status = aoiGetWidth(&width)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if((status = aoiGetHeight(&height)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	switch(area)
	{
		case AUTO_BRIGHT_TARGET_AREA_ALL_RANGE:
			xSize = width;
			xOffset = 0;
			ySize = height;
			yOffset = 0;
			break;

		case AUTO_BRIGHT_TARGET_AREA_CENTER:
			xSize = width / 2;
			xOffset = width / 4;
			ySize = height / 2;
			yOffset = height / 4;
			break;

		case AUTO_BRIGHT_TARGET_AREA_TOP:
			xSize = width;
			xOffset = 0;
			ySize = height / 4;
			yOffset = 0;
			break;

		case AUTO_BRIGHT_TARGET_AREA_BOTTOM:
			xSize = width;
			xOffset = 0;
			ySize = height / 4;
			yOffset = (height / 4) * 3;
			break;

		case AUTO_BRIGHT_TARGET_AREA_LEFT:
			xSize = width / 4;
			xOffset = 0;
			ySize = height;
			yOffset = 0;
			break;

		case AUTO_BRIGHT_TARGET_AREA_RIGHT:
			xSize = width / 4;
			xOffset = (width / 4) * 3;
			ySize = height;
			yOffset = 0;
			break;

		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Auto Bright Target Area(%d) Parameter Error.\n", area);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
	}

	// Width Size設定
	if ((status = autoBrightSetWidthSize (xSize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Width Offset設定
	if ((status = autoBrightSetWidthOffset (xOffset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Size設定
	if ((status = autoBrightSetHeightSize (ySize)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Height Offset設定
	if ((status = autoBrightSetHeightOffset (yOffset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Set Auto Bright Target Area設定
	OUT32 (FIRM_DATA_AUTO_BRIGHT_TARGET_AREA, area);

_DONE:
	return (status);
}


//**********************************************************************************
// Auto Bright 指定範囲取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		size				：指定範囲を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int autoBrightGetTargetArea (int *area)
{
	int status = AVAL_STATUS_SUCCESS;

	// Auto Bright機能は有効?
	if ((status = autoBrightCheckFunctionValid ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Check pOffset Parameter
	if (area == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Auto Bright Get Height Offset area NULL Parameter Error.\n");
		goto _DONE;
	}

	//Auto Bright Target Area取得
	*area = IN32 (FIRM_DATA_AUTO_BRIGHT_TARGET_AREA);

_DONE:
	return (status);
}
#endif // #if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)

// eof
