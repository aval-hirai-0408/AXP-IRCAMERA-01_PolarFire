//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// correction.h - Correction Header
//**********************************************************************************

#ifndef _CORRECTION_H_
#define _CORRECTION_H_

//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------

#define DPC_SD_THRE_MIN					(0.0)		// 時間方向標準偏差閾値最小値
#define DPC_SD_THRE_MAX					(256.0)		// 時間方向標準偏差閾値最大値
#define DPC_NONUNIFORMITY_THRE_MIN		(0.0)		// 輝度不均一性閾値最小値
#define DPC_NONUNIFORMITY_THRE_MAX		(16382.0)	// 輝度不均一性閾値最大値

// 画像情報
#define LUM_MAX_14BIT					(16383)		// 14bit最大輝度値
#define LUM_MIN_14BIT					(0)			// 最小輝度値

#define BIT_16_MAX						(65536)
#define BIT_15_MAX						(32768)
#define BIT_14_MAX						(16384)

// Black Level Range
#define FFC_BLACK_LEVEL_MIN				(0)
#define FFC_BLACK_LEVEL_BIT8_MAX		(217)	// 8bit
#define FFC_BLACK_LEVEL_BIT10_MAX		(874)	// 10bit
#define FFC_BLACK_LEVEL_BIT12_MAX		(1023)	// 12bit
#define FFC_BLACK_LEVEL_BIT14_MAX		(2047)	// 14bit

// White Level Range
#define FFC_WHITE_LEVEL_MIN				(0)
#define FFC_WHITE_LEVEL_BIT8_MAX		(255)	// 8bit
#define FFC_WHITE_LEVEL_BIT10_MAX		(1023)	// 10bit
#define FFC_WHITE_LEVEL_BIT12_MAX		(4095)	// 12bit
#define FFC_WHITE_LEVEL_BIT14_MAX		(16383)	// 14bit

// Default Black
#define FFC_BLACK_LEVEL_BIT8_DEFAULT	(8)
#define FFC_BLACK_LEVEL_BIT10_DEFAULT	(32)
#define FFC_BLACK_LEVEL_BIT12_DEFAULT	(128)
#define FFC_BLACK_LEVEL_BIT14_DEFAULT	(512)

// Default White
#define FFC_WHITE_LEVEL_BIT8_DEFAULT	(222)
#define FFC_WHITE_LEVEL_BIT10_DEFAULT	(890)
#define FFC_WHITE_LEVEL_BIT12_DEFAULT	(3563)
#define FFC_WHITE_LEVEL_BIT14_DEFAULT	(14253)

// 温度設定待ち
#define FFC_TEMP_TIMEOUT				(60)

// FFC Markデータ
#define FFC_MARK_DATA					(0x12345678)


//----------------------------------------------------------------------------------
// Function Define
//----------------------------------------------------------------------------------

// correction.c
int calc_sd2 (double* sd, double* ave_pix, double* area_average);														// 複数枚入力画像に対する各画素の時間方向標準偏差を計算
int calc_sd_sum (unsigned short* src_img);																				// 複数枚入力画像に対する各画素の時間方向標準偏差を計算するデータを取得
int calc_sd_clear (void);																								// バッファ領域クリア
int calc_nonuniformity (double* ave_pix, double area_average, double* nonuniformity);									// 複数枚入力画像に対する各画素の輝度不均一性を計算
int dpc_threshold (double* sd, double* nonuniformity, double sd_thre, double nonuniformity_thre, unsigned char* detect_map);// 時間方向標準偏差と輝度不均一性に対し閾値をかけて欠陥検出マップを生成
int merge_detect_map (unsigned char* map1, unsigned char* map2, unsigned char* detect_map);								// 2つの欠陥検出マップをマージ
int calc_average2 (unsigned short *src_img, double *average);															// 画素毎の平均値を計算
int calc_average_clear (double *average);																				// 画像平均値のバッファをクリア
int calc_average_divider (double *average, int number);																	// 画像平均値の加算値を割る
int threshold (double* chara1, double* chara2, THRESHOLDCHARA thre, unsigned char* detect);								// 複数パラメータを用いて閾値をかける

int ffc_offset (double* black_img, unsigned short* offset);																// FFC用オフセット取得
int ffc_gain (double* white_img, unsigned short* offset, unsigned short black_target, unsigned short white_target, int* gain, int bit);	// FFC用ゲイン取得

int get_correct_coord (unsigned char *detect, DEFECTIONINFO *defection_info, int *index);								// 補正座標決定関数；

int exception_interpolating (int x, unsigned char *filter_index);														// 5x5領域で使用可能マスがない場合の例外的な選択処理

#endif // _CORRECTION_H_
