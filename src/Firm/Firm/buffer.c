//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// buffer.c - Buffer Initialize Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//  画素欠陥検出
//----------------------------------------------------------------------------------
unsigned short *pImageSrc;									// 検出用入力画像の先頭を指すポインタ配列
double *pImageSd = NULL;									// sd納用
double *pImageAvePix = NULL;								// Average Pixel納用
double *pImageNonuniformity = NULL;							// Nonuniformity納用
unsigned char *pImageDetect[NUM_IMG_FOR_LOOP] = {};			// 検出結果格納用
//GRID_XY *pDpcImpossibleGrid = NULL;						// 欠陥不可検出座標データ

//----------------------------------------------------------------------------------
// FFC
//----------------------------------------------------------------------------------
unsigned int *pImageFfcTemp = NULL;							// FFCデータ保存用
unsigned int *pImageFfcSave[FFC_NUM_USER] = {};				// FFCデータ保存用

FFCINFO pImageFfcBlackInfo[FFC_NUM_USER]  = {};				// FFC黒レベル情報保存用
FFCINFO pImageFfcWhiteInfo[FFC_NUM_USER]  = {};				// FFC白レベル情報保存用

double *pImageBlackAve = NULL;								// 黒画像平均値格納用
double *pImageWhiteAve = NULL;								// 白画像平均値格納用

unsigned short *pImageOffset = NULL;						// オフセット格納用
int *pImageGain = NULL;										// ゲイン格納用

#if defined (MODE_FFC_SHADING_LINE)
double *pImageLine = NULL;									// Line毎のデータ格納用
#endif

//----------------------------------------------------------------------------------
// 画素欠陥補正
//----------------------------------------------------------------------------------
DEFECTIONINFO *pImageDefectionInfo = NULL;					// 補正情報格納用
double *pDpcSdSum = NULL;									// 欠陥画像補正Sum
double *pDpcSdSquare = NULL;								// 欠陥画像補正Square
GRID_XY *dpcGetDefectionCountBuff = NULL;					// 欠陥画素座標
GRID_XY *dpcGetDefectionCountBuff2 = NULL;					// 欠陥画素座標


//**********************************************************************************
//	Buffer Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int buffInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	
	//----------------------------------------------------------------------------------
	// 入力画像領域確保(FFC/DPCも共用)
	//----------------------------------------------------------------------------------
	pImageSrc = (unsigned short*)DMA_MEMORY_ADRS;

	//----------------------------------------------------------------------------------
	// sd
	//----------------------------------------------------------------------------------
	pImageSd = (double *)DPC_SD_MEMORY_ADRS;

	//----------------------------------------------------------------------------------
	// Average Pixel
	//----------------------------------------------------------------------------------
	pImageAvePix = (double *)DPC_AVG_MEMORY_ADRS;

	//----------------------------------------------------------------------------------
	// Non nuniformity
	//----------------------------------------------------------------------------------
	pImageNonuniformity = (double *)DPC_NON_MEMORY_ADRS;

	//----------------------------------------------------------------------------------
	// 検出結果
	//----------------------------------------------------------------------------------
	pImageDetect[0] = (unsigned char*)DPC_TEMP0_MEMORY_ADRS;
	pImageDetect[1] = (unsigned char*)DPC_TEMP1_MEMORY_ADRS;
	pImageDetect[2] = (unsigned char*)DPC_TEMP2_MEMORY_ADRS;
	pImageDetect[3] = (unsigned char*)DPC_TEMP3_MEMORY_ADRS;

	//----------------------------------------------------------------------------------
	// 欠陥補正検出不可座標
	//----------------------------------------------------------------------------------
	//pDpcImpossibleGrid = (GRID_XY *)DPC_IMPOSSIBLE_GRID_ADRS;
	
	//----------------------------------------------------------------------------------
	// DPC Pixel
	//----------------------------------------------------------------------------------
	dpcGetDefectionCountBuff = (GRID_XY *)DPC_GRID_ADRS;
	dpcGetDefectionCountBuff2 = (GRID_XY *)DPC_GRID2_ADRS;
	
	//----------------------------------------------------------------------------------
	// FFCデータ一時格納用メモリ(1次補正のみ使用)
	//----------------------------------------------------------------------------------
	pImageFfcSave[0] = (unsigned int*)FFC0_TEMP_MEMORY_ADRS;
	pImageFfcSave[3] = (unsigned int*)FFC3_TEMP_MEMORY_ADRS;

	//----------------------------------------------------------------------------------
	// FFC黒レベル情報一時格納用メモリ(sizeof(FFCINFO) * FFC_NUM_USER個)
	//----------------------------------------------------------------------------------
	//pImageFfcBlackInfo[0] = (FFCINFO *)FFCINFO0_BLACK_ADRS;
	//pImageFfcBlackInfo[1] = (FFCINFO *)FFCINFO0_BLACK_ADRS;
	//pImageFfcBlackInfo[2] = (FFCINFO *)FFCINFO0_BLACK_ADRS;
	//pImageFfcBlackInfo[3] = (FFCINFO *)FFCINFO0_BLACK_ADRS;

	//----------------------------------------------------------------------------------
	// FFC白レベル情報一時格納用メモリ(FFC_NUM_USER個)
	//----------------------------------------------------------------------------------
	//pImageFfcWhiteInfo[0] = (FFCINFO *)FFCINFO0_WHITE_ADRS;
	//pImageFfcWhiteInfo[1] = (FFCINFO *)FFCINFO1_WHITE_ADRS;
	//pImageFfcWhiteInfo[2] = (FFCINFO *)FFCINFO2_WHITE_ADRS;
	//pImageFfcWhiteInfo[3] = (FFCINFO *)FFCINFO3_WHITE_ADRS;

	//----------------------------------------------------------------------------------
	// 黒画像平均
	//----------------------------------------------------------------------------------

	// Black
	pImageBlackAve = (double *)FFC_BLACK_MEMORY_ADRS;

	// White
	pImageWhiteAve = (double *)FFC_WHITE_MEMORY_ADRS;

	// Offset用メモリ(1次補正)
	pImageOffset = (unsigned short*)FFC_OFFSET_MEMORY_ADRS;
	
	// Gain用メモリ(1次補正)
	pImageGain = (int *)FFC_GAIN_MEMORY_ADRS;

	//----------------------------------------------------------------------------------
	// 1ライン分のバッファ(sizeof(double) * IMG_HEIGHT)
	//----------------------------------------------------------------------------------
#if defined (MODE_FFC_SHADING_LINE)
	pImageLine = (double *)FFC_SHADING_LINE_ADRS;
#endif

	//----------------------------------------------------------------------------------
	// 補正情報格納(sizeof(DEFECTIONINFO) * NUM_DEFECTION_PIX)
	//----------------------------------------------------------------------------------
	pImageDefectionInfo = (DEFECTIONINFO*)DPCINFO_ADRS;

	//----------------------------------------------------------------------------------
	// 欠陥画像補正SD Sum
	//----------------------------------------------------------------------------------
	pDpcSdSum = (double *)DPC_SUM_MEMORY_ADRS;

	//----------------------------------------------------------------------------------
	// 欠陥画像補正SD Squart
	//----------------------------------------------------------------------------------
	pDpcSdSquare = (double *)DPC_SQUARE_SUM_MEMORY_ADRS;

	return (status);
}

// eof
