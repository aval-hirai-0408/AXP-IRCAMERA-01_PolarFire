//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// intc.c - Interrupt Controller Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define PLIC_EXT_INTR_SOURCES                          (31u)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// Interrupt Controller Status
int gIntcStatus = 0;

// Instance
miv_plic_instance_t g_plic;


//**********************************************************************************
//	割り込みコントローラ初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int intcInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;

	MRV_enable_local_irq (MRV32_EXT_IRQn);

	// Initialize
	MIV_PLIC_init (&g_plic, MIV_PLIC_BASE_ADDR, PLIC_EXT_INTR_SOURCES);

	// Disable IRQ
    MIV_PLIC_disable_irq (&g_plic, MIV_PLIC_EXT0_IRQn);
    MIV_PLIC_disable_irq (&g_plic, MIV_PLIC_EXT1_IRQn);

    // Enable
	MRV_enable_local_irq (MIP_MEIP);
	HAL_enable_interrupts ();

	return (status);
}


//**********************************************************************************
//  割り込みハンドラ
//----------------------------------------------------------------------------------
//  [ INPUT ]
//      -
//  [ OUTPUT ]
//      AVAL_STATUS_SUCCESS ：正常終了
//      上記以外                ：異常終了
//==================================================================================
void External_IRQHandler(void)
{
    uint32_t reg_val = read_csr(mip);
    MIV_PLIC_isr(&g_plic);
}


//**********************************************************************************
//  無効割り込みハンドラ
//----------------------------------------------------------------------------------
//  [ INPUT ]
//      -
//  [ OUTPUT ]
//      AVAL_STATUS_SUCCESS ：正常終了
//      上記以外                ：異常終了
//==================================================================================
void Invalid_IRQHandler(void)
{
}

// eof
