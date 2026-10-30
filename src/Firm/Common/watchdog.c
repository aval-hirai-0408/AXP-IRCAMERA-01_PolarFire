//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// watchdog.c -WatchDog Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
miv_wdog_config_t gWd0_config;



//**********************************************************************************
//	Timer Control Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int watchDogInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;

	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, status, "WatchDog Initialize\n");

#if 0
	// Initialize
    MIV_WDOG_init (MIV_ESS_WDOG_BASE_ADDR);
#endif

	return (status);
}


//**********************************************************************************
//	WatchDog Start
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int watchDogStart (void)
{
	int status = AVAL_STATUS_SUCCESS;

#if 0
	// Read the default config
    MIV_WDOG_get_config (&gWd0_config);

    /* Set such that the MVRP interrupt will happen after ~13 seconds after
     * reset and the Trigger interrupt will happen after ~25 seconds.
     */

    gWd0_config.forbidden_en = MIV_WDOG_ENABLE;
    gWd0_config.timeout_val = 0x3e0u;
    gWd0_config.mvrp_val = (10000000UL);
    gWd0_config.time_val = (2UL*10000000UL);

    MIV_WDOG_configure(&wd0_config);
#endif

	return (status);
}

// eof
