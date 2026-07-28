//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// dma.c - DMA Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------

// 割り込みID
#define DMA_INTR_ID				(0)

//割り込みモード
#define DMA_INT_MODE

// DMAタイムアウト
#define DMA_INT_TIMEOUT			(10000)		// 10s


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
// DMA Status
int gDmaStatus = 0;

int dmaIntFlag = 0;


//**********************************************************************************
//	初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int dmaInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	
	// FPGA DMA割り込み有効
	OUT32 (FPGA_DMA_IRQ_ENABLE_ADRS, FPGA_DMA_IRQ_ENABLE_BIT);

#if defined (MODE_IPU_MULTI)
	// FPGA DMA2割り込み有効
	OUT32 (FPGA_DMA2_IRQ_ENABLE_ADRS, FPGA_DMA_IRQ_ENABLE_BIT);

	// FPGA IPU DMAC割り込み有効
	if ((status = ipuIntEnable (FPGA_IPU_IRQ_MASK_DMAC_BIT)) != AVAL_STATUS_SUCCESS)
		goto _DONE;;

	// FPGA IPU Global割り込み有効
	if ((status = ipuSetGlobalInt (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;;
#endif

	//@@@1
	//@@@1
	//@@@1

	
_DONE:
	return (status);
}


//**********************************************************************************
//	ハンドラ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		CallBackRef				：コールバック引数ポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
static void dma_InterruptHandler (void *CallBackRef)
{
	// Set Flag
	dmaIntFlag = 1;

	// Interrupt Clear
	OUT32 (FPGA_DMA_IRQ_CLR_ADRS, FPGA_DMA_IRQ_CLEAR_BIT);
	OUT32 (FPGA_DMA_IRQ_CLR_ADRS, 0);

#if defined (MODE_IPU_MULTI)
	// Interrupt Clear
	OUT32 (FPGA_DMA2_IRQ_CLR_ADRS, FPGA_DMA_IRQ_CLEAR_BIT);
	OUT32 (FPGA_DMA2_IRQ_CLR_ADRS, 0);

	// Common Interrupt Clear
	//OUT32 (FPGA_IPU_IRQ_STATUS_ADRS, FPGA_IPU_IRQ_STATUS_DMAC_BIT);
	ipuIntClear (FPGA_IPU_IRQ_STATUS_DMAC_BIT);
#endif
}


//**********************************************************************************
//	DMA開始
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pBuff					：画像データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int dmaStart (unsigned char *pBuffer)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int ctrl;
	unsigned int frame;
	
	// Check pBuffer Parameter
	if (pBuffer == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_DMA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DMA Start pBuffer NULL Parameter Error.\n");
		goto _DONE;
	}

	// 1画面サイズ
	frame = DMA_WIDTH_DATA_ALIGH_PIXEL * IMG_HEIGHT * PIXEL_SIZE;

	// 割り込みFlag初期化
	dmaIntFlag = 0;

	// DMA Control Set & Start Clear
	ctrl = FPGA_DMA_CTRL_CACHE_BIT;
	OUT32 (FPGA_DMA_CTRL_ADRS, ctrl);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DMA2_CTRL_ADRS, ctrl);
#endif

	// DMA転送先アドレス
	OUT32 (FPGA_DMA_START_ADRS, (unsigned int)pBuffer);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DMA2_START_ADRS, (unsigned long)(pBuffer + DMA_MEMORY_IPU_MULTI_INTERVAL));
#endif

#if defined (MODE_IPU_MULTI)
	frame /= IPU_COUNT;
#endif

	// DMA転送サイズ
	OUT32 (FPGA_DMA_DEPTH_ADRS, (frame/8));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DMA2_DEPTH_ADRS, (frame/8));
#endif

	// DMA割り込み発生サイズ
	OUT32 (FPGA_DMA_IRQ_WORD_ADRS, (frame/8));

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DMA2_IRQ_WORD_ADRS, (frame/8));
#endif

	// 転送される領域はキャッシュInvalidate
	cacheInvalidateRange ((unsigned int)pBuffer, frame);

	// DMAスタート
	ctrl = FPGA_DMA_CTRL_CACHE_BIT | FPGA_DMA_CTRL_START_BIT;
	OUT32 (FPGA_DMA_CTRL_ADRS, ctrl);

#if defined (MODE_IPU_MULTI)
	OUT32 (FPGA_DMA2_CTRL_ADRS, ctrl);
#endif

	// DMA割り込み待ち
	if ((status = dmaWait ()) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	DMA終了待ち
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int dmaWait (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int ix;
	unsigned int data;
#if defined (MODE_IPU_MULTI)
	unsigned int data2;
#endif

	// 割り込み発生待ち
	for (ix=0; ix<DMA_INT_TIMEOUT; ix++)
	{
		if (dmaIntFlag == 1)
		{
#if !defined (MODE_IPU_MULTI)

			// DMA Control End Check
			data = IN32 (FPGA_DMA_CTRL_ADRS);

			// DMA End?
			if (data & FPGA_DMA_CTRL_END_BIT)
				break;

#else // #if !defined (MODE_IPU_MULTI)

			// DMA Control End Check
			data = IN32 (FPGA_DMA_CTRL_ADRS);

			// DMA Control End Check
			data2 = IN32 (FPGA_DMA2_CTRL_ADRS);

			// DMA End?
			if ((data & FPGA_DMA_CTRL_END_BIT) && (data2 & FPGA_DMA_CTRL_END_BIT))
				break;
#endif
		}

		msDelay(1);
	}

	// Timeout?
	if (ix >= DMA_INT_TIMEOUT)
	{
		// Set DMA Control Stop
		data = IN32 (FPGA_DMA_CTRL_ADRS);
		data &= ~FPGA_DMA_CTRL_START_BIT;
		OUT32 (FPGA_DMA_CTRL_ADRS, (data | FPGA_DMA_CTRL_ABORT_BIT));

#if defined (MODE_IPU_MULTI)
		data = IN32 (FPGA_DMA2_CTRL_ADRS);
		data &= ~FPGA_DMA_CTRL_START_BIT;
		OUT32 (FPGA_DMA2_CTRL_ADRS, (data | FPGA_DMA_CTRL_ABORT_BIT));
#endif

		status = MAKE_ERROR_STATUS (AVAL_STATUS_DMA, AVAL_STATUS_TIMEOUT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DMA Timeout Error.\n");
		goto _DONE;
	}

	// Set DMA Control Stop
	data = IN32 (FPGA_DMA_CTRL_ADRS);
	data &= ~FPGA_DMA_CTRL_START_BIT;
	OUT32 (FPGA_DMA_CTRL_ADRS, data);

	#if defined (MODE_IPU_MULTI)
	data = IN32 (FPGA_DMA2_CTRL_ADRS);
	data &= ~FPGA_DMA_CTRL_START_BIT;
	OUT32 (FPGA_DMA2_CTRL_ADRS, data);
	#endif

_DONE:
	return (status);
}

// eof
