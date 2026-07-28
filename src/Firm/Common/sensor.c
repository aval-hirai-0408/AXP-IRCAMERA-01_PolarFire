//**********************************************************************************
//
//                               Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// sensor.c - Sensor Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define DIAG_SENSOR_INT_TIMEOUT			(5000)


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gSensorTmgStatus = 0;
int gSensorStatus = 0;
int gSensorRegAces = 0;


//**********************************************************************************
//	センサ初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int sensorInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;
	
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)

	#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

	//------------------------------------------------------------
	// Sensor Initialize
	//------------------------------------------------------------
	if ((status = sensorVendorSInitialize ()) != AVAL_STATUS_SUCCESS)
	{
		gSensorStatus = status;
		goto _DONE;
	}

	//------------------------------------------------------------
	// Sensor Timing Initialize
	//------------------------------------------------------------
	#if 0
	if ((status = sensorTimingInitialize ()) != AVAL_STATUS_SUCCESS)
	{
		gSensorTmgStatus = status;
		goto _DONE;
	}
	#endif

	#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

_DONE:
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)

	return (status);
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
int sensorInitialize2 (void)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT ("Sensor Initialize2\n");

#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)

	#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

	if ((status = sensorVendorSInitialize2 ()) != AVAL_STATUS_SUCCESS)
	{
		gSensorStatus = status;
		goto _DONE;
	}

	#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)

_DONE:
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)

	return (status);
}

// eof
