//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// timer.c - Timer Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define TICKS_PER_CHANGE_PERIOD		(1000)	// 1ms
#define TIMER_LOAD_VALUE 			(SYS_CLK_FREQ/256/TICKS_PER_CHANGE_PERIOD)

// 1us count
#define DELAY_TIMER_DIV				(25)
#define DELAY_TIMER_CLOCK			(SYS_CLK_FREQ/DELAY_TIMER_DIV)
#define DELAY_1U_TIME				(1.0/DELAY_TIMER_CLOCK*1000*1000)
#define DELAY_1U_COUNT				(1.0/DELAY_1U_TIME)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// instance
timer_instance_t g_core_timer_0;
miv_timer_instance_t g_miv_timer;

// Timer Controller Status
int gTimerStatus = 0;

// Counter
unsigned int gTimerCount = 0;
unsigned int gTickCount = 0;
unsigned int gTickCountTotal = 0;
unsigned int gTickCountTotalTempAbnormalCheck = 0;
unsigned int gTickCountTotalAutoBright= 0;


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern miv_plic_instance_t g_plic;


//**********************************************************************************
//	タイマ初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerInitilize (void)
{
	int status = AVAL_STATUS_SUCCESS;

	//------------------------------------------------------------
	// Core Timer	
	//------------------------------------------------------------
#if 0
    uint32_t mie;
    __asm volatile("csrr %0, mie" : "=r"(mie));
    printf("mie = 0x%08x\n", mie);
#endif

    // Initialise and configure the timer
    TMR_init (&g_core_timer_0, CORETIMER0_BASE_ADDR, TMR_CONTINUOUS_MODE, PRESCALER_DIV_256, TIMER_LOAD_VALUE);

    // Stop
    TMR_stop (&g_core_timer_0);

    // Enable the timer to generate interrupts
    TMR_enable_int (&g_core_timer_0);

    // Enable Interrupt
    MIV_PLIC_enable_irq (&g_plic, MIV_PLIC_EXT0_IRQn);

    // Start the timer
    //TMR_start (&g_core_timer_0);


	//------------------------------------------------------------
	// MIV Timer
	//------------------------------------------------------------

	// Initialize
	MIV_TIMER_init (&g_miv_timer, MIV_MTIMER_BASE_ADDR);
	
	// Divider
	OUT32 (MIV_TIMER_PRESCALAR_REG_OFFSET, DELAY_TIMER_DIV);
	
	// Config
	MIV_TIMER_config (&g_miv_timer, SYS_CLK_FREQ);
	
	return (status);
}


//**********************************************************************************
//	タイマ割り込みハンドラ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		IntcInstancePtr		：割り込み情報のポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
//int MSYS_EI0_IRQHandler (void)
uint8_t MIV_PLIC_EXT0_IRQHandler(void)
{
	// Clear the interrupt within the timer

    TMR_clear_int(&g_core_timer_0);

	if (++gTickCount >= TICKS_PER_CHANGE_PERIOD)
	{
		gTickCount = 0;
		gTimerCount++;
	}

	gTickCountTotal++;

	return (EXT_IRQ_KEEP_ENABLED);
}


//**********************************************************************************
//	Timer Start
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerStart (void)
{
	TMR_start (&g_core_timer_0);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Timer Stop
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerStop (void)
{
	TMR_stop (&g_core_timer_0);
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	タイマカウント設定(s単位)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：タイマカウント設定値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerSetSec (unsigned int count)
{
	gTimerCount = count;
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	タイマカウント取得値(s単位)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		タイマカウント取得値
//==================================================================================
unsigned int timerGetSec (void)
{
 	return (gTimerCount);
}


//**********************************************************************************
//	タイマカウント設定(ms単位)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		count				：タイマカウント設定値
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int timerSetMs (unsigned int count)
{
	gTickCountTotal = count;
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	タイマカウント取得値(ms単位)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		タイマカウント取得値
//==================================================================================
unsigned int timerGetMs (void)
{
	return (gTickCountTotal);
}


//**********************************************************************************
//	タイマカウント取得(カウント単位)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		タイマカウント取得値
//==================================================================================
unsigned int timerGetCount (void)
{
	uint64_t count;

	count = MIV_TIMER_read_current_time (&g_miv_timer);

	return ((unsigned int)count);
}


//**********************************************************************************
//	タイマカウント取得(us単位)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		タイマカウント取得値
//==================================================================================
unsigned int timerGetUsCount (void)
{
	unsigned int count32;

	// カウンタ取得
	count32 = timerGetCount();
	count32 /= DELAY_1U_COUNT;

	return ((unsigned int)count32);
}


//**********************************************************************************
//	delay:s単位(アップカウント)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		delay				：待ち時間(s単位)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sDelay (unsigned int delay)
{
	unsigned int i, second;

	// No Delay ?
	if (delay == 0)
		return (AVAL_STATUS_SUCCESS);

	second = delay * 1000;

	for (i=0; i<second; i++)
		usDelay (1000);
	
	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	delay:ms単位(アップカウント)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		delay				：待ち時間(ms単位)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int msDelay (unsigned int delay)
{
	return (usDelay (delay*1000));
}


//**********************************************************************************
//	delay:us単位(アップカウント)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		delay				：待ち時間(us単位)
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int usDelay (unsigned int delay)
{
	unsigned int baseTickCount;
	volatile unsigned int curTickCount;
	unsigned int terminalTickCount;
	int actualRollover = 0;
	int calcRollover = 0;
	unsigned int ticksToWait;
	unsigned int reqDelay;
	unsigned int oneUsDelay;
	
	// No Delay ?
	if ((reqDelay = delay) == 0)
		return (AVAL_STATUS_SUCCESS);

	// Set 1usec count
	oneUsDelay = DELAY_1U_COUNT;

	// Convert delay time into ticks
	ticksToWait = (unsigned int)(reqDelay * oneUsDelay);

	// Get Counter
	baseTickCount = timerGetCount ();

	// Compute when to stop
	terminalTickCount = baseTickCount + ticksToWait;

	// Check for expected rollover
	if (terminalTickCount < baseTickCount)
		calcRollover = 1;

	do {
		// Get Counter
		curTickCount = timerGetCount ();

		// Check for actual rollover
		if (curTickCount < baseTickCount)
			actualRollover = 1;

		if(((curTickCount >= terminalTickCount) &&
		(actualRollover == calcRollover)) ||
		((curTickCount < terminalTickCount) &&
		(actualRollover > calcRollover)))
		{
			// Delay time met
			break;
		}
	} while( 1 );

	return (AVAL_STATUS_SUCCESS);
}

// eof
