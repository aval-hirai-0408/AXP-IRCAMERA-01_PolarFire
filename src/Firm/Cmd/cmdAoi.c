//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdAoi.c - AOI/XFLIP Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"

#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
#include "../Common/sensorVendors.h"
#endif


//**********************************************************************************
//	AOI情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int aoi (void)
{
	int status;

	status = cmdAoi (NULL);
	return (status);
}


//**********************************************************************************
//	AOI情報取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoi (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int mode;
	int bit;
	int x, y;
	int index;
	int xOffset;
	int yOffset;

	//--------------------------------------------------
	// AOI情報
	//--------------------------------------------------
	DEBUG_PRINT_FORCE ("[AOI informations]\n");

	//--------------------------------------------------
	// Bit幅
	//--------------------------------------------------
	if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Bit                       ");
	DEBUG_PRINT_FORCE ("%d\n", bit);


	//--------------------------------------------------
	// Size
	//--------------------------------------------------

	if ((status = aoiGetWidth (&x)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = aoiGetWidthOffset (&xOffset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	
	if ((status = aoiGetHeight (&y)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = aoiGetHeightOffset (&yOffset)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Width                     ");
	DEBUG_PRINT_FORCE ("%d\n", x);

	DEBUG_PRINT_FORCE ("   Height                    ");
	DEBUG_PRINT_FORCE ("%d\n", y);

	DEBUG_PRINT_FORCE ("   Offset X                  ");
	DEBUG_PRINT_FORCE ("%d\n", xOffset);

	DEBUG_PRINT_FORCE ("   Offset Y                  ");
	DEBUG_PRINT_FORCE ("%d\n", yOffset);


	//--------------------------------------------------
	// Test Pattern
	//--------------------------------------------------
	if ((status = aoiGetPattern (&index)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Test Pattern              ");
	DEBUG_PRINT_FORCE ("%d\n", index);


	//--------------------------------------------------
	// Test Pattern Increment
	//--------------------------------------------------
	if ((status = aoiGetTpInc (&x, &y)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Width Increment           ");
	DEBUG_PRINT_FORCE ("%d\n", x);

	DEBUG_PRINT_FORCE ("   Height Increment          ");
	DEBUG_PRINT_FORCE ("%d\n", y);


	//--------------------------------------------------
	// Xflip
	//--------------------------------------------------
	if ((status = aoiGetXflip (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   Flip                      ");

	if (mode == FPGA_XFLIP_ENABLE_BIT)
		DEBUG_PRINT_FORCE ("%d(Reverse Enable)\n", mode);
	else
		DEBUG_PRINT_FORCE ("%d(Reverse Disable)\n", mode);


#if defined(MODE_BINNING)
	//--------------------------------------------------
	// BinningX
	//--------------------------------------------------
	if ((status = aoiGetBinningX (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   BinningX                  ");
	DEBUG_PRINT_FORCE ("%d\n", mode);

	//--------------------------------------------------
	// BinningY
	//--------------------------------------------------
	if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   BinningY                  ");
	DEBUG_PRINT_FORCE ("%d\n", mode);

	//--------------------------------------------------
	// BinningMode
	//--------------------------------------------------
	if ((status = aoiGetBinningMode (&mode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	DEBUG_PRINT_FORCE ("   BinningMode               ");

	if (mode == FPGA_BINNING_MODE_SUM)
		DEBUG_PRINT_FORCE ("%d(Sum)\n", mode);
	else
		DEBUG_PRINT_FORCE ("%d(Average)\n", mode);
#endif

	DEBUG_PRINT_FORCE ("\n");

_DONE:
	return (status);
}


//**********************************************************************************
//	AOI Bit Width
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiBitWidth (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int bit;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiBitWidthHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// bit取得
		if ((status = aoiGetBitWidth (&bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", bit);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &bit) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// bit設定
		if ((status = aoiSetBitWidth (bit)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	AOI Bit Width Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiBitWidthHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Pixel Bit is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : bit/pixel-depth\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : bit\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Pixel Bit is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : bit/pixel-depth [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : bit\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[bit]\n");
#if (MODE_CAMERA_BIT >= PIXEL_8BIT)
	DEBUG_PRINT_FORCE ("   8 : 8bit\n");
#endif
#if (MODE_CAMERA_BIT >= PIXEL_10BIT)
	DEBUG_PRINT_FORCE ("  10 : 10bit\n");
#endif
#if (MODE_CAMERA_BIT >= PIXEL_12BIT)
	DEBUG_PRINT_FORCE ("  12 : 12bit\n");
#endif
#if (MODE_CAMERA_BIT >= PIXEL_14BIT)
	DEBUG_PRINT_FORCE ("  14 : 14bit\n");
#endif
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Width Max
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdWidthMax (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int width;
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	int mode;
#endif

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdWidthMaxHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
		if ((status = roiGetAreaSize (&mode)) != AVAL_STATUS_SUCCESS)
			mode = 0;

		if (mode == 1)
			width = sensorWidth ();
		else
			width = WidthMax ();
#else
		// Width Max
		width = WidthMax ();
#endif

		DEBUG_PRINT_FORCE ("%d ", width);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Width Max Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdWidthMaxHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The width is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : widthmax\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Width Max\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Height Max
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdHeightMax (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int height;
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	int mode;
#endif

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdHeightMaxHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)

		if ((status = roiGetAreaSize (&mode)) != AVAL_STATUS_SUCCESS)
			mode = 0;

		if (mode == 1)
			height = sensorHeight ();
		else
			height = HeightMax ();
#else
		// Height Max
		height = HeightMax ();
#endif

		DEBUG_PRINT_FORCE ("%d ", height);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Height Max Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdHeightMaxHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The height is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : heightmax\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Height Max\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Sensor Width
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorWidth (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int width;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensorWidthHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Sensor Width
		width = sensorWidth ();

		DEBUG_PRINT_FORCE ("%d ", width);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Width Max Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorWidthHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The sensor width is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensor-width\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Sensor Width\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	Sensor Height
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorHeight (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int height;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensorHeightHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Sensor Height
		height = sensorHeight ();

		DEBUG_PRINT_FORCE ("%d ", height);
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	Sensor Height Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorHeightHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The sensor height is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : sensorheight\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Sensor Height\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	AOI Width
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiWidth (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int size;
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;
#endif

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiWidthHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Width取得
		if ((status = aoiGetWidth (&size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", size);
	}
	else if (argc == 2)
	{
#if defined (MODE_FRAMERATE_HIGH_SPEED)
		// High Speed Mode取得
		if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (hsMode == MODE_ENABLE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Width cannot be changed in High Speed mode.\n");
			goto _DONE;
		}
#endif

		// Width取得
		if (sscanf (gCmdArg[1], "%d", &size) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Width設定
		if ((status = aoiSetWidth (size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	AOI Width Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiWidthHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Width Size is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : width\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : width size\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Width Size is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : width [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : width size\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Width Size]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", ROI_WIDTH_ALIGH);
	DEBUG_PRINT_FORCE ("  Max : %d\n", WidthMax()-ROI_WIDTH_ALIGH);
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}


//**********************************************************************************
//	AOI Height
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiHeight (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int size;
#if defined (MODE_ROI_VERSION2)
	int multiMode;
#endif

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiHeightHelp (NULL);
			goto _DONE;
		}
	}

#if defined (MODE_ROI_VERSION2)
		// ROIモード取得
		if ((status = roiCheckMultiMode (&multiMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

	if (argc == 1)
	{
#if defined (MODE_ROI_VERSION2)
		if (multiMode == MODE_ENABLE)
		{
			// Total Height Size取得
			if ((status = fpgaRoiGetCameraHeightTotalSize (&size)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
		else
		{
			// Selector設定
			if ((status = roiSetSelector (ROI_SELECTOR_MIN)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Height取得
			if ((status = aoiGetHeight (&size)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

#else

		// Height取得
		if ((status = aoiGetHeight (&size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

#endif

		DEBUG_PRINT_FORCE ("%d ", size);
	}
	else if (argc == 2)
	{
		// Height取得
		if (sscanf (gCmdArg[1], "%d", &size) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

#if defined (MODE_ROI_VERSION2)
		if (multiMode == MODE_ENABLE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Cannot write because multi mode is enabled\n");
			goto _DONE;
		}

		// Selector設定
		if ((status = roiSetSelector (ROI_SELECTOR_MIN)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

		// Height設定
		if ((status = aoiSetHeight (size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		DEBUG_PRINT_FORCE (CMD_ERROR_INVALID_ARG);
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	AOI Height Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiHeightHelp (void *str)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Height Size is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : height\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : height size\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Height Size is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : height [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : height size\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Height Size]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", ROI_HEIGHT_ALIGH);
	DEBUG_PRINT_FORCE ("  Max : %d\n", HeightMax()-ROI_HEIGHT_ALIGH);
	DEBUG_PRINT_FORCE ("\n");

	return (status);
}


//**********************************************************************************
//	AOI Width Offset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiWidthOffset (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int offset;
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;
#endif

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiWidthOffsetHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Width Offset取得
		if ((status = aoiGetWidthOffset (&offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", offset);
	}
	else if (argc == 2)
	{

		#if defined (MODE_FRAMERATE_HIGH_SPEED)
		// High Speed Mode取得
		if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (hsMode == MODE_ENABLE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Width Offset cannot be changed in High Speed mode.\n");
			goto _DONE;
		}
		#endif

		// Width Offset取得
		if (sscanf (gCmdArg[1], "%d", &offset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Width Offset設定
		if ((status = aoiSetWidthOffset (offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	AOI Width Offset Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiWidthOffsetHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Width Offset is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : offsetx\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Width Offset\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Width Offset is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : offsetx [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Width Offset\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Width Size]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", ROI_WIDTH_OFFSET_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", CAMERA_WIDTH_MAX-ROI_WIDTH_ALIGH);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	AOI Height Offset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiHeightOffset (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int offset;
#if defined (MODE_ROI_VERSION2)
	int multiMode;
#endif
#if defined (MODE_FRAMERATE_HIGH_SPEED)
	int hsMode;
#endif

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiHeightOffsetHelp (NULL);
			goto _DONE;
		}
	}

#if defined (MODE_ROI_VERSION2)
	// ROIモード取得
	if ((status = roiCheckMultiMode (&multiMode)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	if (argc == 1)
	{
#if defined (MODE_ROI_VERSION2)
		if (multiMode == MODE_ENABLE)
		{
			offset = 0;
		}
		else
		{
			// Selector設定
			if ((status = roiSetSelector (ROI_SELECTOR_MIN)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Height Offset取得
			if ((status = aoiGetHeightOffset (&offset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}
#else
		// Height Offset取得
		if ((status = aoiGetHeightOffset (&offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

		DEBUG_PRINT_FORCE ("%d ", offset);
	}
	else if (argc == 2)
	{
		// Height Offset取得
		if (sscanf (gCmdArg[1], "%d", &offset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

#if defined (MODE_FRAMERATE_HIGH_SPEED)
		// High Speed Mode取得
		if ((status = sensorGetFrameRateHighSpeedMode (&hsMode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (hsMode == MODE_ENABLE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Height Offset cannot be changed in High Speed mode.\n");
			goto _DONE;
		}
#endif

#if defined (MODE_ROI_VERSION2)
		if (multiMode == MODE_ENABLE)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Cannot write because multi mode is enabled\n");
			goto _DONE;
		}

		// ROI 0に設定
		if ((status = roiSetSelector (ROI_SELECTOR_MIN)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif

		// Height Offset設定
		if ((status = aoiSetHeightOffset (offset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	AOI Height Offset Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiHeightOffsetHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Height Offset is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : offsety\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Height Offset\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Height Offset is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : offsety [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Height Offset\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Height Offset]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", ROI_HEIGHT_OFFSET_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", CAMERA_HEIGHT_MAX-ROI_HEIGHT_ALIGH);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	AOIテストパターン
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiTpMode (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiTpModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Test Pattern取得
		if ((status = aoiGetPattern (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Test Pattern設定
		if ((status = aoiSetPatternMain (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	AOIテストパターンHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiTpModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Test Pattern is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpmode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output  Param     : Test Pattern\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Test Pattern is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpmode [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Test Pattern\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  0 = Capture Image\n");
	DEBUG_PRINT_FORCE ("  1 = White Image\n");
	DEBUG_PRINT_FORCE ("  2 = Black Image\n");
	DEBUG_PRINT_FORCE ("  3 = Width  Graduation Image(Stop)\n");
	DEBUG_PRINT_FORCE ("  4 = Height Graduation Image(Stop)\n");
	DEBUG_PRINT_FORCE ("  5 = Width  Graduation Image(Moving)\n");
	DEBUG_PRINT_FORCE ("  6 = Height Graduation Image(Moving)\n");
	DEBUG_PRINT_FORCE ("  7 = Slanting Graduation Image(Stop)\n");
	DEBUG_PRINT_FORCE ("  8 = Slanting Graduation Image(Moving)\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	AOIテストパターンインクリメント
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiTpInc (void *str)
{
	int status;
	int argc;
	int x, y;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiTpIncHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Test Patternインクリメント取得
		if ((status = aoiGetTpInc (&x, &y)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d %d ", x, y);
	}
	else if (argc == 3)
	{
		if (sscanf (gCmdArg[1], "%d", &x) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		if (sscanf (gCmdArg[2], "%d", &y) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Test Patternインクリメント設定
		if ((status = aoiSetTpInc (x, y)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	AOIテストパターンインクリメントHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiTpIncHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Test Pattern Increment Data is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpinc\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param0     : Horizon  Increment Data\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : Vertical Increment Data\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Test Pattern Increment Data is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpinc [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Horizon  Increment Data\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Vertical Increment Data\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Horizon Increment Data]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", AOI_TP_HORIZON_INC_VERSION2_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", AOI_TP_HORIZON_INC_VERSION2_MAX);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Vertical Increment Data]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", AOI_TP_VIRTICAL_INC_VERSION2_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", AOI_TP_VIRTICAL_INC_VERSION2_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	AOIテストパターンPosition
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiTpPosition (void *str)
{
	int status;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiTpPositionHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Test Pattern Position取得
		if ((status = aoiGetPatternPosition (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Test Pattern Position設定
		if ((status = aoiSetPatternPosition (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	AOIテストパターンHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiTpPositionHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The test pattern position is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpposition\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Test Pattern Position\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The test pattern position is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpposition [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Test Pattern Position\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  %d : Normal Position\n", FPGA_AOI_TP_POSITION_NORMAL);
	DEBUG_PRINT_FORCE ("  %d : Pre Position\n", FPGA_AOI_TP_POSITION_PRE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
//**********************************************************************************
//	センサテストパターン
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorTpMode (void *str)
{
	int status;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensorTpModeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Test Pattern取得
		if ((status = sensorTpGetMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Test Pattern設定
		if ((status = sensorTpSetMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	センサテストパターンHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorTpModeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Test Pattern is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpmodesensor \n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output  Param     : Test Pattern\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Test Pattern is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpmodesensor  [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Test Pattern\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  Min = %d\n", SENSOR_REG_PG_MODE_MIN);
	DEBUG_PRINT_FORCE ("  Max = %d\n", SENSOR_REG_PG_MODE_MAX);

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	センサテストインクリメント
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorTpInc (void *str)
{
	int status;
	int argc;
	int data;
	unsigned char hInc, vInc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensorTpIncHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Test Patternインクリメント取得
		if ((status = sensorTpGetInc (&hInc, &vInc)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d %d ", hInc, vInc);
	}
	else if (argc == 3)
	{
		if (sscanf (gCmdArg[1], "%d", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		hInc = (unsigned char)data;


		if (sscanf (gCmdArg[2], "%d", &data) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		vInc = (unsigned char)data;

		// Test Patternインクリメント設定
		if ((status = sensorTpSetInc (hInc, vInc)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	センサテストインクリメントHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorTpIncHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Test Pattern increment is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpincsensor \n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param0     : Test Pattern Increment Width\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : Test Pattern Increment Height\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Test Pattern increment is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpincesensor  [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Test Pattern Increment Width\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Test Pattern Increment Height\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	センサテストデータ
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorTpData (void *str)
{
	int status;
	int argc;
	unsigned int data1, data2;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdSensorTpDataHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Test Pattern Data取得
		if ((status = sensorTpGetData (&data1, &data2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
		
		DEBUG_PRINT_FORCE ("%d %d ", data1, data2);
	}
	else if (argc == 3)
	{
		if (sscanf (gCmdArg[1], "%d", &data1) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		if (sscanf (gCmdArg[2], "%d", &data2) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Test Pattern Data設定
		if ((status = sensorTpSetData (data1, data2)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	センサテストインクリメントHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdSensorTpDataHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Test Pattern data is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpdatasensor \n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param0     : Test Pattern Data1\n");
	DEBUG_PRINT_FORCE ("  Output Param1     : Test Pattern Data2\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Sensor Test Pattern data is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : tpdatasensor  [param0] [param1]\n");
	DEBUG_PRINT_FORCE ("  Input  Param0     : Test Pattern Data1\n");
	DEBUG_PRINT_FORCE ("  Input  Param1     : Test Pattern Data2\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)


//**********************************************************************************
//	AOI Xflip
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiXflip (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiXflipHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Xflip取得
		if ((status = aoiGetXflip (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

#if defined (MODE_XFLIP_INVERT)
		if (mode == XFLIP_ENABLE)
			mode = XFLIP_DISABLE;
		else
			mode = XFLIP_ENABLE;
#endif
		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

#if defined (MODE_XFLIP_INVERT)
		// Check mode Parameter
		if ((mode != XFLIP_ENABLE) && (mode != XFLIP_DISABLE))
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Xflip Mode(%d) Parameter Error.\n", mode);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			goto _DONE;
		}

		if (mode == XFLIP_ENABLE)
			mode = XFLIP_DISABLE;
		else
			mode = XFLIP_ENABLE;
#endif

		// Xflip設定
		if ((status = aoiSetXflip (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	AOI Xflip Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiXflipHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Flip is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : flip\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Flip Mode\n");
	DEBUG_PRINT_FORCE ("                      0 = A screen isn't reversed\n");
	DEBUG_PRINT_FORCE ("                      1 = A screen is reversed\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The Flip is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : flip [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Flip Mode\n");
	DEBUG_PRINT_FORCE ("                      0 = A screen isn't reversed\n");
	DEBUG_PRINT_FORCE ("                      1 = A screen is reversed\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


#if defined(MODE_BINNING)
//**********************************************************************************
//	BinningX
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdAoiBinningX(void* str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiBinningXHelp(NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// BinningX
		if ((status = aoiGetBinningX (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Binningx
		if ((status = aoiSetBinningX (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	BinningX Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiBinningXHelp(void* str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Binning X is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : binningx\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Binning X is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : binningx [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", BINNING_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n\n", BINNING_MAX);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	BinningY
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdAoiBinningY(void* str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiBinningYHelp(NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// BinningY
		if ((status = aoiGetBinningY (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// BinningY
		if ((status = aoiSetBinningY (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	BinningY Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiBinningYHelp(void* str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Binning Y is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : binningy\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Binning Y is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : binningy [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", BINNING_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n\n", BINNING_MAX);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	BinningMode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str					：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cmdAoiBinningMode(void* str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int mode;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdAoiBinningModeHelp(NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// BinningY
		if ((status = aoiGetBinningMode (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// mode取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// BinningMode
		if ((status = aoiSetBinningMode (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_ARGUMENT);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_ARG);
		goto _DONE;
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	BinningMode Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdAoiBinningModeHelp(void* str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : Binning Mode is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : binningmode\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : mode\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Binning Mode is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : binningmode [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Mode]\n");
	DEBUG_PRINT_FORCE ("  0 : Sum\n");
	DEBUG_PRINT_FORCE ("  1 : Average\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}
#endif //#if defined(MODE_BINNING)

// eof
