//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// diagDma.c - DMA Test
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern unsigned short *pImageSrc;


//**********************************************************************************
//	DMA評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDma (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int saveLed1 = -1;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdDiagDmaHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// Diagステート
		saveLed1 = ledDiagnosticState ();

		// 検査
		status = cmdDiagDmaMain (gCmdArg[1]);

		// LEDの設定を元に戻す
		ledReturnState (-1, saveLed1);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DMA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	DMA Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdDiagDmaHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A dma check is performed.\n");
	DEBUG_PRINT_FORCE ("  Command           : diagdma [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Check Number of times\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	DMA評価
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外 					：異常終了
//==================================================================================
int cmdDiagDmaMain (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	char *dev = (char *)"DMA";
	unsigned short *pBuff = NULL;
	unsigned short *pBuffORG = NULL;
#if !defined (MODE_FFC_DATA_ALIGN_ADJUST)
	unsigned int size, capSize;
#endif
	int ix;
	unsigned int tloop;
#if !defined (MODE_FFC_DATA_ALIGN_ADJUST)
	int align = 256;
#endif
	unsigned int loop;

	// 検査回数取得
	if (sscanf (str, "%d", &loop) != 1)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
		goto _DONE;
	}

#if !defined (MODE_FFC_DATA_ALIGN_ADJUST)
	// サイズ取得
	size = WidthMax() * HeightMax () * PIXEL_SIZE;

	// 取り込みサイズ
	capSize = WidthMax () * HeightMax ();
#endif


#if defined (MODE_FFC_DATA_ALIGN_ADJUST)

	pBuff = (unsigned short *)pImageSrc;

#else // #if defined (MODE_FFC_DATA_ALIGN_ADJUST)

	// メモリリクエスト
	if ((pBuffORG = (unsigned short *)malloc(size + align*2)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DMA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DMA Memory Request Error\n");
		goto _DONE;
	}

	pBuff = (unsigned short *)(((unsigned int)pBuffORG & ~(align-1)) + align);

#endif // #if defined (MODE_FFC_DATA_ALIGN_ADJUST)


	//--------------------------------------------------
	// 画像取り込み開始
	//--------------------------------------------------

	// 連続モード取り込み
	acquisitionSetMode (ACQUISITION_MODE_CONTIN);

	// 取り込み開始
	acquisitionStart ();

	// ROI Full Size
	if ((status = roiSetDmaFull ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

MORE:
	for (tloop=0; tloop<loop; tloop++)
	{
		DEBUG_PRINT("--------------------[%s] loop = %d--------------------\n", dev, tloop);


#if !defined (MODE_FFC_DATA_ALIGN_ADJUST)
		// バッファクリア
		for (ix=0; ix<capSize; ix++)
			pBuff[ix] = 0;;
#endif

		// キャッシュフラッシュ
		cacheFlush ();

		// DMA開始
		if ((status = dmaStart ((unsigned char *)pBuff)) != AVAL_STATUS_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_DMA, AVAL_STATUS_IO);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DMA Start Error\n");
			goto _DONE;
		}
	}

	if (loop == 0xffffffff)
		goto MORE;

_DONE:
	// 取り込み停止
	acquisitionAbort ();

	if (pBuffORG != NULL)
		free (pBuffORG);

	// Restore
	roiSetDmaResutore ();

	return (status);
}

// eof
