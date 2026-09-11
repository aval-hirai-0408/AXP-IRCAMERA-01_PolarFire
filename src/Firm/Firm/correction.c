//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// correction.c - Correction Program
//**********************************************************************************


//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
unsigned int gDpcImpGridCount = 0;							// 欠陥座標数カウント


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------

// 画素欠陥検出
//extern GRID_XY *pDpcImpossibleGrid;						// 欠陥不可検出座標データ

// 画素欠陥補正
extern double *pDpcSdSum;									// 欠陥画像補正Sum
extern double *pDpcSdSquare;								// 欠陥画像補正Square


//==================================================================================
//==================================================================================
//								画素欠陥検出
//==================================================================================
//==================================================================================


//**********************************************************************************
//	calc_sd
//	複数枚の画像入力に対し、各画素について標準偏差を計算
//	後段の輝度不均一性計算のため、各画素の輝度平均値と平均画像の全画素平均値を出力
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		sd				：各画素の時間方向標準偏差
//		ave_pix			：全入力画像に対する画素毎の輝度平均値
//		area_average	：平均画像の全画素平均値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int calc_sd2 (double* sd, double* ave_pix, double* area_average)
{
	int status = AVAL_STATUS_SUCCESS;
	int row = 0;
	int col = 0;
	double tmp = 0.0;

	// Check sd Parameter
	if (sd == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Calc sd sd NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check ave_pix Parameter
	if (ave_pix == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Calc sd ave_pix NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check area_average Parameter
	if (area_average == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Calc sd area_average NULL Parameter Error.\n");
		goto _DONE;
	}

	*area_average = 0.0;

	// 画素毎の平均輝度値と二乗平均輝度値を計算
	for (row = 0; row < IMG_HEIGHT; row++)
	{
		for (col = 0; col < IMG_WIDTH; col++)
		{
			ave_pix[col + row * IMG_STRIDE] = (double)pDpcSdSum[col + row * IMG_STRIDE] / (double)NUM_IMG_FOR_DETECT;

			*area_average += ave_pix[col + row * IMG_STRIDE];
			tmp = (pDpcSdSquare[col + row * IMG_STRIDE]  / (double)NUM_IMG_FOR_DETECT) - (ave_pix[col + row * IMG_STRIDE] * ave_pix[col + row * IMG_STRIDE]);

			if (tmp < 0.0)
				sd[col + row * IMG_STRIDE] = 0.0;
			else
				sd[col + row * IMG_STRIDE] = sqrt(tmp);
		}
	}

	*area_average /= (double)(IMG_WIDTH * IMG_HEIGHT);
	
_DONE:
	return (status);
}


//**********************************************************************************
//	calc_sdSum
//	複数枚の画像入力に対し、各画素について標準偏差を計算するためのデータを取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		src_img			：複数枚入力画像
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int calc_sd_sum (unsigned short* src_img)
{
	int status = AVAL_STATUS_SUCCESS;
	int row = 0;
	int col = 0;
	int calc = 1;
	unsigned short data16;
#if defined (MODE_IPU_MULTI)
	int ipu;
	int srcGrid, desGrid;
	int xOffset;
#endif

	// Check src_img Parameter
	if (src_img == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Calc sd src_img NULL Parameter Error.\n");
		goto _DONE;
	}

#if defined (MODE_FFC_BIT_CALC)
	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// 画素毎の平均輝度値と二乗平均輝度値を計算
#if !defined (MODE_IPU_MULTI)

	for (row = 0; row < IMG_HEIGHT; row++)
	{
		for (col = 0; col < IMG_WIDTH; col++)
		{
			data16 = src_img[col + row * DMA_WIDTH_DATA_ALIGH_PIXEL] * calc;
			pDpcSdSum[col + row * IMG_STRIDE] += (double)data16;
			pDpcSdSquare[col + row * IMG_STRIDE] += (double)(data16 * data16);
		}
	}

#else // #if !defined (MODE_IPU_MULTI)

	for (row = 0; row < IMG_HEIGHT; row++)
	{
		for (ipu=0; ipu < IPU_COUNT; ipu++)
		{
			if (ipu == 0)
				xOffset = 0;
			else
				xOffset = IMG_WIDTH_OFFSET;

			for (col = 0; col < IMG_WIDTH_IPU_SIZE; col++)
			{
				srcGrid = col + row * DMA_WIDTH_DATA_ALIGH_PIXEL + xOffset + (DMA_MEMORY_IPU_MULTI_INTERVAL * ipu) / 2;
				desGrid = col + (ipu * IMG_WIDTH_IPU_SIZE) + (row * IMG_STRIDE);

				data16 = src_img[srcGrid] * calc;
				pDpcSdSum[desGrid] += (double)data16;
				pDpcSdSquare[desGrid] += (double)(data16 * data16);
			}
		}
	}

#endif // #if !defined (MODE_IPU_MULTI)

_DONE:
	return (status);
}


//**********************************************************************************
//	calc_sd_clear
//	複数枚の画像入力に対し、各画素について標準偏差を計算するためのデータを取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		src_img			：複数枚入力画像
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int calc_sd_clear (void)
{
	int status = AVAL_STATUS_SUCCESS;

	if (pDpcSdSum == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Calc SD Clear pDpcSdSum NULL Parameter Error.\n");
		goto _DONE;
	}

	if (pDpcSdSquare == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Calc SD Clear pDpcSdSquare NULL Parameter Error.\n");
		goto _DONE;
	}

	// 初期化
#if !defined (MODE_IPU_MULTI)
	memset (pDpcSdSum, 0, sizeof(double) * IMG_WIDTH * IMG_HEIGHT);
	memset (pDpcSdSquare, 0, sizeof(double) * IMG_WIDTH * IMG_HEIGHT);
#else
	memset (pDpcSdSum, 0, sizeof(double) * IMG_WIDTH_IPU_MULTI_FULL * IMG_HEIGHT);
	memset (pDpcSdSquare, 0, sizeof(double) * IMG_WIDTH_IPU_MULTI_FULL * IMG_HEIGHT);
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	calc_nonuniformity
//	複数枚画像入力の平均画像に対し、各画素について輝度不均一性を計算
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		ave_pix				：画素毎の平均画像を格納するポインタ
//		area_average		：全画素の平均画像を格納するポインタ
//		nonuniformity		：平均画像の輝度不均一性を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int calc_nonuniformity (double* ave_pix, double area_average, double* nonuniformity)
{
	int status = AVAL_STATUS_SUCCESS;
	int row = 0;
	int col = 0;

	// Check ave_pix Parameter
	if (ave_pix == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Calc nonuniformity ave_pix NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check nonuniformity Parameter
	if (nonuniformity == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Calc nonuniformity nonuniformity NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check area_average Parameter
	if (area_average < 1.0)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Calc nonuniformity area_average(%f) Parameter Error.\n", area_average);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}
	
	// 画素毎の平均輝度値と二乗平均輝度値を計算
	for (row = 0; row < IMG_HEIGHT; row++)
	{
		for (col = 0; col < IMG_WIDTH; col++)
		{
			nonuniformity[col + row * IMG_STRIDE] = fabs((ave_pix[col + row * IMG_STRIDE] - area_average) / area_average);
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	dpc_threshold
//	標準偏差と輝度不均一性に対し閾値をかけ、欠陥検出マップを作成します。
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		sd					：時間方向標準偏差
//		nonuniformity		：輝度不均一性
//		sd_thre				：時間方向標準偏差閾値
//		nonuniformity_thre	：輝度不均一性に対する閾値
//		detect_map			：欠陥検出マップ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dpc_threshold (double* sd, double* nonuniformity, double sd_thre, double nonuniformity_thre, unsigned char* detect_map)
{
	int status = AVAL_STATUS_SUCCESS;
	int row = 0;
	int col = 0;

	// Check sd Parameter
	if (sd == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Threshold sd NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check nonuniformity Parameter
	if (nonuniformity == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Threshold nonuniformity NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check sd_thre Parameter
	if ((sd_thre < DPC_SD_THRE_MIN) || (DPC_SD_THRE_MAX <= sd_thre))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Threshold sd_thre(%.2f) Parameter Error.(Min:%.2f / Max:%.2f)\n", sd_thre, DPC_SD_THRE_MIN, DPC_SD_THRE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check nonuniformity_thre Parameter
	if ((nonuniformity_thre < DPC_NONUNIFORMITY_THRE_MIN) || (DPC_NONUNIFORMITY_THRE_MAX <= nonuniformity_thre))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "DPC Threshold nonuniformity_thre(%.2f) Parameter Error.(Min:%.2f / Max:%.2f)\n", nonuniformity_thre, DPC_NONUNIFORMITY_THRE_MIN, DPC_NONUNIFORMITY_THRE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check detect_map Parameter
	if (detect_map == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Threshold detect_map NULL Parameter Error.\n");
		goto _DONE;
	}

	for (row = 0; row < IMG_HEIGHT; row++)
	{
		for (col = 0; col < IMG_WIDTH; col++)
		{
			if ((sd_thre <= sd[col + row * IMG_STRIDE]) || (nonuniformity_thre <= nonuniformity[col + row * IMG_STRIDE]))
			{
				sprintf (gLogMsgBuff, "row = %d / col = %d / sd = %.2f / non = %.2f \n", row, col, sd[col + row * IMG_STRIDE], nonuniformity[col + row * IMG_STRIDE]);
				cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				detect_map[col + row * IMG_STRIDE] = 1;
			}
			else
				detect_map[col + row * IMG_STRIDE] = 0;
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	merge_detect_map
//	2つの欠陥検出マップをマージし、1つの欠陥検出マップを生成します。
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		map1				：1つ目のマージ対象欠陥検出マップ
//		map2				：2つ目のマージ対象欠陥検出マップ
//		detect_map			：欠陥検出マップ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int merge_detect_map (unsigned char* map1, unsigned char* map2, unsigned char* detect_map)
{
	int status = AVAL_STATUS_SUCCESS;
	int row = 0;
	int col = 0;

	// Check map1 Parameter
	if (map1 == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Marge Detect Map map1 NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check map2 Parameter
	if (map2 == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Marge Detect Map map2 NULL Parameter Error.\n");
		goto _DONE;
	}

	// Check detect_map Parameter
	if (detect_map == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Marge Detect Map detect_map NULL Parameter Error.\n");
		goto _DONE;
	}

	for (row = 0; row < IMG_HEIGHT; row++)
	{
		for (col = 0; col < IMG_WIDTH; col++)
		{
			if ((map1[col + row * IMG_STRIDE] == 1) || (map2[col + row * IMG_STRIDE] == 1))
				detect_map[col + row * IMG_STRIDE] = 1;
			else
				detect_map[col + row * IMG_STRIDE] = 0;
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	calc_average
//	入力画像全てを用いて、全画素において平均値を計算する
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		src_img				：各入力画像の先頭画素を指すポインタ
//		average				：平均値計算結果格納領域の先頭を指すポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int calc_average2 (unsigned short *src_img, double *average)
{
	int status = AVAL_STATUS_SUCCESS;
	int x = 0, y = 0;	// 画素座標
#if defined (MODE_IPU_MULTI)
	int ipu;
	int srcGrid, desGrid;
	int xOffset;
	unsigned short data16;
	unsigned short *ptr16;
	double *desD;
#endif

	if (src_img == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Calc Average src_img NULL Parameter Error.\n");
		goto _DONE;
	}

	// averageが有効でない
	if (average == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Calc Average average NULL Parameter Error.\n");
		goto _DONE;
	}

	// 全画素走査
#if !defined (MODE_IPU_MULTI)

	for (y = 0; y < IMG_HEIGHT; y++)
	{
		for (x = 0; x < IMG_WIDTH; x++)
		{
			// 入力画像の同画素輝度値を全て加算
			average[x + y * IMG_WIDTH] += (double)src_img[x + y * DMA_WIDTH_DATA_ALIGH_PIXEL];
		}
	}

#else // #if !defined (MODE_IPU_MULTI)

	for (y = 0; y < IMG_HEIGHT; y++)
	{
		for (ipu=0; ipu < IPU_COUNT; ipu++)
		{
			if (ipu == 0)
				xOffset = 0;
			else
				xOffset = IMG_WIDTH_OFFSET;

			for (x = 0; x < IMG_WIDTH_IPU_SIZE; x++)
			{
				srcGrid = x + y * (DMA_WIDTH_DATA_ALIGH / 2) + xOffset + (DMA_MEMORY_IPU_MULTI_INTERVAL * ipu) / 2;
				desGrid = x + (ipu * IMG_WIDTH_IPU_SIZE) + (y * IMG_STRIDE);

				// 入力画像の同画素輝度値を全て加算
				ptr16 = &src_img[srcGrid];
				data16 = *ptr16;
				desD = &average[desGrid];
				*desD += data16;
			}
		}
	}

#endif // #if !defined (MODE_IPU_MULTI)

_DONE:
	return (status);
}


//**********************************************************************************
//	calc_average_clear
//	画像平均値のバッファをクリア
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		average				：平均値計算結果格納領域の先頭を指すポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int calc_average_clear (double *average)
{
	// 平均値計算結果格納用領域を初期化
#if !defined (MODE_IPU_MULTI)
	memset (average, 0, sizeof(double) * IMG_WIDTH * IMG_HEIGHT);
#else
	memset (average, 0, sizeof(double) * IMG_WIDTH_IPU_MULTI_FULL * IMG_HEIGHT);
#endif

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	calc_average_divider
//	加算された画像平均値を割る
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		average				：平均値計算結果格納領域の先頭を指すポインタ
//		number				：個数
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int calc_average_divider (double *average, int number)
{
	int x = 0, y = 0;	// 画素座標

	// 全画素走査
	for (y = 0; y < IMG_HEIGHT; y++)
	{
		for (x = 0; x < IMG_WIDTH; x++)
		{
			// 画像枚数で除算し平均値を計算
			average[x + y * IMG_STRIDE] /= number;
		}
	}

	return (AVAL_STATUS_SUCCESS);
}



//**********************************************************************************
//	threshold
//	2つの特徴量に対し閾値を用いて二値化(正常画素、異常画素を検出)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		chara1				：特徴量1を格納した領域の先頭を指すポインタ
//		chara2				：特徴量2を格納した領域の先頭を指すポインタ
//		thre				：特徴量の閾値
//		detect				：検出結果格納領域の先頭を指すポインタ (1:欠陥, 0:正常)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int threshold (double *chara1, double *chara2, THRESHOLDCHARA thre, unsigned char *detect)
{
	int status = AVAL_STATUS_SUCCESS;
	int x = 0, y = 0;	// 画素座標

	// chara1が有効でない
	if (chara1 == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Threshold chara1 NULL Parameter Error.\n");
		goto _DONE;
	}

	// chara2が有効でない
	if (chara2 == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Threshold chara2 NULL Parameter Error.\n");
		goto _DONE;
	}

	// detectが有効でない
	if (detect == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Threshold detect NULL Parameter Error.\n");
		goto _DONE;
	}

	// 検出結果格納用領域を初期化
	memset (detect, 0, sizeof(unsigned char) * IMG_WIDTH * IMG_HEIGHT);

	// 2パラメータそれぞれに対する閾値を満足する場合に検出判定
	for (y = 0; y < IMG_HEIGHT; y++)
	{
		for (x = 0; x < IMG_WIDTH; x++)
		{
			if ((thre.chara1 > chara1[x + y * IMG_STRIDE]) && (thre.chara2 > chara2[x + y * IMG_STRIDE]))
				detect[x + y * IMG_STRIDE] = 1;		// 欠陥検出
			else
				detect[x + y * IMG_STRIDE] = 0;		// 正常画素
		}
	}

_DONE:
	return (status);
}


//==================================================================================
//==================================================================================
//							Flat Field Correction
//==================================================================================
//==================================================================================

//**********************************************************************************
//	ffc_offset
//	FFC用オフセット取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		black_img			：黒画像データ格納領域へのポインタ
//		offset				：FFC用オフセットが格納された領域へのポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffc_offset (double *black_img, unsigned short *offset)
{
	int status = AVAL_STATUS_SUCCESS;
	int x = 0, y = 0;
	float gain, dBlack;
	int calc = 1;

	// black_imgが有効か
	if (black_img == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Offset black_img NULL Parameter Error.\n");
		goto _DONE;
	}

	// offsetが有効か
	if (offset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Offset offset NULL Parameter Error.\n");
		goto _DONE;
	}

	// Gain取得(倍率)
	if ((status = digitalGetGainX (&gain)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined (MODE_FFC_BIT_CALC)
	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	// 目標値に合わせたオフセットを計算する
	for (y = 0; y < IMG_HEIGHT; y++)
	{
		for (x = 0; x < IMG_WIDTH; x++)
		{
			// オフセット計算
			dBlack = black_img[x + y * IMG_STRIDE] * calc / gain;
			offset[x + y * IMG_STRIDE] = (unsigned short)dBlack;
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	ffc_gain
//	FFC用ゲイン取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		white_img			：白画像データ格納領域へのポインタ
//		offset				：FFC用オフセットが格納された領域へのポインタ
//		black_target		：黒レベル目標値
//		white_target		：白レベル目標値
//		gain				：FFC用ゲイン設定値が格納される領域へのポインタ
//		bit					：画素サイズ(8/10/12/14)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ffc_gain (double *white_img, unsigned short *offset, unsigned short black_target, unsigned short white_target, int *gain, int bit)
{
	int status = AVAL_STATUS_SUCCESS;
	int x = 0, y = 0;
	double tmp_gain = 0.0;
	unsigned int whiteData;
	unsigned int gainData;
	unsigned int targetMin, targetMax;
	int calc = 1;

	// white_imgが有効か
	if (white_img == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Gain white_img NULL Parameter Error.\n");
		goto _DONE;
	}

	// offsetが有効か
	if (offset == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Gain offset NULL Parameter Error.\n");
		goto _DONE;
	}

	// gainは有効か
	if (gain == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "FFC Gain gain NULL Parameter Error.\n");
		goto _DONE;
	}

	// 最小値取得
	targetMin = 0;
	targetMax = LUM_MAX_14BIT;
	
	// 最大値取得
	if (bit== 8)
		targetMax = FFC_WHITE_LEVEL_BIT8_MAX;
	else if (bit== 10)
		targetMax = FFC_WHITE_LEVEL_BIT10_MAX;
	else if (bit== 12)
		targetMax = FFC_WHITE_LEVEL_BIT12_MAX;
	else if (bit== 14)
		targetMax = FFC_WHITE_LEVEL_BIT14_MAX;
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain Bit(%d) Parameter Error.(Min:%dbit / Max:%dbit)\n", bit, PIXEL_BIT_MIN, PIXEL_BIT_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

#if defined (MODE_FFC_BIT_CALC)
	if ((status = ffcGetBitCalc (&calc)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Target Max Bit Adjust
	targetMax *= calc;
#endif

	// 黒レベル目標値は範囲内か
	if ((black_target < targetMin) || (targetMax < black_target))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain black_target(%d) Parameter Error.(Min:%d / Max:%d)\n", black_target, LUM_MIN_14BIT, LUM_MAX_14BIT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 白レベル目標値は範囲内か
	if ((white_target < targetMin) || (targetMax < white_target))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain white_target(%d) Parameter Error.(Min:%d / Max:%d)\n", white_target, LUM_MIN_14BIT, LUM_MAX_14BIT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 白レベル目標値は黒レベル目標値よりも大きい必要有
	if (black_target >= white_target)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_FFC, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "FFC Gain black_target(%d) > white_target(%d) Parameter Error.\n", black_target, white_target);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// 目標値に合わせたゲインを計算する
	for (y = 0; y < IMG_HEIGHT; y++)
	{
		for (x = 0; x < IMG_WIDTH; x++)
		{
			// 白画像に対しオフセットをかけた状態で目標値に達するようなゲインを計算する
			whiteData = (unsigned int)white_img[x + y * IMG_STRIDE] * calc;
			tmp_gain = (double)(white_target - black_target) / (double)((double)whiteData - (double)black_target);
			gainData = (int)(tmp_gain * BIT_16_MAX);

			if (gainData > 0x0003FFFF)
				gainData = 0x0003FFFF;

			// 計算されたゲイン値になるような設定値を計算する
			gain[x + y * IMG_STRIDE] = gainData;
		}
	}

_DONE:
	return (status);
}


//==================================================================================
//==================================================================================
//								画素欠陥補正
//==================================================================================
//==================================================================================


//**********************************************************************************
//	get_correct_coord
//	全欠陥画素について補正に使用するフィルタのインデックスを取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		detect				：欠陥検出結果
//		defection_info		：欠陥画素座標、補正に使用するフィルタのインデックスを格納した構造体へのポインタ
//		index				：欠陥画素座標、補正に使用するフィルタのインデックス数を格納したポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int get_correct_coord (unsigned char *detect, DEFECTIONINFO *defection_info, int *index)
{
	int status = AVAL_STATUS_SUCCESS;
	int x = 0, y = 0;
	int fi;
	int ix, iy;
	int defectivePixelCount;
	int pixelCount;

	// detectが有効でない
	if (detect == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Correct Coord detect NULL Parameter Error.\n");
		goto _DONE;
	}

	// defection_infoが有効でない
	if (defection_info == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Correct Coord defection_info NULL Parameter Error.\n");
		goto _DONE;
	}

	// indexが有効でない
	if (index == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DPC Correct Coord index NULL Parameter Error.\n");
		goto _DONE;
	}

	// 補正不可座標個数初期化
	gDpcImpGridCount = 0;

	// 欠陥画素検出数
	fi = 0;

	// 全画素走査
#if (CAMERA_TYPE == CAMERA_TYPE_AREA)
	for (y = 0; y < IMG_HEIGHT; y++)
#elif (CAMERA_TYPE == CAMERA_TYPE_LINE)
	y = 0;
#endif
	{
		for (x = 0; x < IMG_WIDTH; x++)
		{
			// 画素欠陥の情報格納サイズの空きあり?
			if (fi > NUM_DEFECTION_PIX)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_DPC, AVAL_STATUS_RESOURCE_EXHAUSTED);
				sprintf (gLogMsgBuff, "DPC Correct Coord DPC Size Over MAX = %d / x = %d / y= %d\n", NUM_DEFECTION_PIX, x, y);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
				goto _DONE;
			}

			// 欠陥画素の場合、座標とフィルタインデックスを取得する
			if (detect[x + y * IMG_WIDTH] == 1)	// 欠陥画素あり
			{
				sprintf (gLogMsgBuff, "Normal detect x = %d / y= %d\n", x, y);
				cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

				defectivePixelCount = 0;

				// 画素数初期化
				pixelCount = 0;

				// 5x5フィルタ
				for (iy=-2; iy<3; iy++)
				{
					//------------------------------
					// ①y=0
					//------------------------------
					if ((y==0) && (iy==-2))
						continue;
					if ((y==0) && (iy==-1))
						continue;
					//------------------------------
					// ②y=1
					//------------------------------
					if ((y==1) && (iy==-2))
						continue;

					//------------------------------
					// ③y=max-1
					//------------------------------
					if ((y==(IMG_HEIGHT-2)) && (iy==2))
						continue;
					//------------------------------
					// ④y=max
					//------------------------------
					if ((y==(IMG_HEIGHT-1)) && (iy==1))
						continue;
					if ((y==(IMG_HEIGHT-1)) && (iy==2))
						continue;

					for (ix=-2; ix<3; ix++)
					{
						//------------------------------
						// ⑤x=0
						//------------------------------
						if ((x==0) && (ix==-2))
							continue;
						if ((x==0) && (ix==-1))
							continue;
						//------------------------------
						// ⑥x=1
						//------------------------------
						if ((x==1) && (ix==-1))
							continue;
						//------------------------------
						// ⑦x=max-1
						//------------------------------
						if ((x==(IMG_WIDTH-2)) && (ix==1))
							continue;
						//------------------------------
						// ⑧x=max
						//------------------------------
						if ((x==(IMG_WIDTH-1)) && (ix==1))
							continue;
						if ((x==(IMG_WIDTH-1)) && (ix==2))
							continue;

						//------------------------------
						// 自身
						//------------------------------
						if ((ix==0) && (iy==0))
							continue;

						// 周辺画素数
						pixelCount++;

						// 欠陥画素?
						if ((detect[(x + ix + (IMG_WIDTH*(y + iy)))]) == 1)
							defectivePixelCount++;
					}
				}

				// 周辺画素数と周辺画素の欠陥数は同じ?==>すべて欠陥
				if (pixelCount == defectivePixelCount)
				{
					// 欠陥補正不可座標
					sprintf (gLogMsgBuff, "Impossible detect x = %d / y= %d\n", x, y);
					cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

					//pDpcImpossibleGrid[gDpcImpGridCount].x = x;
					//pDpcImpossibleGrid[gDpcImpGridCount].y = y;
					gDpcImpGridCount++;
				}
				else
				{
					// 欠陥座標
					defection_info[fi].x = x;
					defection_info[fi].y = y;
					fi++;
				}
			}
		}
	}

	// 検出個数を設定
	*index = fi;

_DONE:
	return (status);
}

// eof
