//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdRoi.c - ROI Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	ROI Selector
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiSelector (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int selector;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdRoiSelectorHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 取得
		if ((status = roiGetSelector (&selector)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", selector);
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &selector) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 設定
		if ((status = roiSetSelector (selector)) != AVAL_STATUS_SUCCESS)
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
//	ROI Selector Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiSelectorHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi selector is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : roi-selector\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : selector\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi selector is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : roi-selector [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : selector\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Selector]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", ROI_SELECTOR_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", ROI_SELECTOR_MAX);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ROI Multi Y
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiMultiY (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int selector;
	int mode;
	int heightSize, heightOffset;
	int count;
#if defined (MODE_CXP_MULTI_PORT)
	int cxpPort;
#endif

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdRoiMultiYHelp (NULL);
			goto _DONE;
		}
	}

	//--------------------------------------------------------------------------------
	// ROI情報表示
	//--------------------------------------------------------------------------------
	if (argc == 1)
	{
		if ((status = roiShow (0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

	}
	//--------------------------------------------------------------------------------
	// ROI取得
	//--------------------------------------------------------------------------------
	else if (argc == 2)
	{
		// Selector取得
		if (sscanf (gCmdArg[1], "%d", &selector) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Set ROI Selector
		if ((status = roiSetSelector (selector)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get Enable/Disable
		if ((status = roiGetValid (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get Width
		//if ((status = aoiGetWidth (&widthSize)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// Get Height
		if ((status = aoiGetHeight (&heightSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Get Width Offset
		//if ((status = aoiGetWidthOffset (&widthOffset)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// Get Height Offset
		if ((status = aoiGetHeightOffset (&heightOffset)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d %d %d ", mode, heightSize, heightOffset);
	}
	//--------------------------------------------------------------------------------
	// ROI Enable/Disable設定
	//--------------------------------------------------------------------------------
	else if (argc == 3)
	{
		// Selector取得
		if (sscanf (gCmdArg[1], "%d", &selector) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// mode取得
		if (sscanf (gCmdArg[2], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Valid無効?
		if (mode == 0)
		{
			// Get Valid Count
			if ((status = roiGetNoSelfValidCount (selector, &count)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			if (count == 0)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI cannot be disabled.\n");
				goto _DONE;
			}
		}

		// Set ROI Selector
		if ((status = roiSetSelector (selector)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Set Enable/Disable
		if ((status = roiSetValid (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

#if defined (MODE_CXP)
		if (gInterFaceID == INTERFACE_CXP)
		{
			// CXP Height Param設定
			#if !defined (MODE_CXP_MULTI_PORT)

			if ((status = cxpSetHeightParam (0)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			#else // #if !defined (MODE_CXP_MULTI_PORT)

			if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			if ((status = cxpSetHeightParam (cxpPort)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			#endif // #if !defined (MODE_CXP_MULTI_PORT)
		}
#endif
	}
	//--------------------------------------------------------------------------------
	// ROI Enable/Disable設定
	//--------------------------------------------------------------------------------
	else if (argc == 5)
	{
		// Selector取得
		if (sscanf (gCmdArg[1], "%d", &selector) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// mode取得
		if (sscanf (gCmdArg[2], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// height size取得
		if (sscanf (gCmdArg[3], "%d", &heightSize) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// height offset取得
		if (sscanf (gCmdArg[4], "%d", &heightOffset) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Valid無効?
		if (mode == 0)
		{
			// Get Valid Count
			if ((status = roiGetNoSelfValidCount (selector, &count)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			if (count == 0)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "ROI cannot be disabled.\n");
				goto _DONE;
			}
		}

		// Set ROI Selector
		if ((status = roiSetSelector (selector)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Set Width Offset
		//if ((status = aoiSetWidthOffset (widthOffset)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// Set Width
		//if ((status = aoiSetWidth (widthSize)) != AVAL_STATUS_SUCCESS)
			//goto _DONE;

		// Set Height Offset
		if (heightSize != 0)
		{
			if ((status = aoiSetHeightOffset (heightOffset)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

		// Set Height
		if ((status = aoiSetHeight (heightSize)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		// Set Enable/Disable
		if (heightSize != 0)
		{
			if ((status = roiSetValid (mode)) != AVAL_STATUS_SUCCESS)
				goto _DONE;
		}

#if defined (MODE_CXP)
		if (gInterFaceID == INTERFACE_CXP)
		{
			// CXP Height Param設定

			#if !defined (MODE_CXP_MULTI_PORT)

			if ((status = cxpSetHeightParam (0)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			#else // #if !defined (MODE_CXP_MULTI_PORT)

			if ((status = cxpGetPort (&cxpPort)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			if ((status = cxpSetHeightParam (cxpPort)) != AVAL_STATUS_SUCCESS)
				goto _DONE;

			#endif // #if !defined (MODE_CXP_MULTI_PORT)
		}
#endif
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
//	ROI Multi Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiMultiYHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : roimultiy [Param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : selector\n");
	DEBUG_PRINT_FORCE ("  Output0 Param     : ROI Enable/Disable\r");
	DEBUG_PRINT_FORCE ("  Output1 Param     : Height\n");
	DEBUG_PRINT_FORCE ("  Output2 Param     : OffsetY\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : roimultiy [Param0] [Param1] [Param2] [Param3]\n");
	DEBUG_PRINT_FORCE ("  Input0  Param     : selector\n");
	DEBUG_PRINT_FORCE ("  Input1  Param     : ROI Enable/Disable\n");
	DEBUG_PRINT_FORCE ("  Input2  Param     : Height\n");
	DEBUG_PRINT_FORCE ("  Input3  Param     : OffsetY\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Selector]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", ROI_SELECTOR_MIN);
	DEBUG_PRINT_FORCE ("  Max : %d\n", ROI_SELECTOR_MAX);

	DEBUG_PRINT_FORCE ("\n[ROI Enable/Disable]\n");
	DEBUG_PRINT_FORCE ("  Disable : %d\n", MODE_DISABLE);
	DEBUG_PRINT_FORCE ("  Enable  : %d\n", MODE_ENABLE);

	//DEBUG_PRINT_FORCE ("\n[Width]\n");
	//DEBUG_PRINT_FORCE ("  Min : %d\n", 1);
	//DEBUG_PRINT_FORCE ("  Max : %d\n",  WidthMax());

	DEBUG_PRINT_FORCE ("\n[Height]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 1);
	DEBUG_PRINT_FORCE ("  Max : %d\n",  HeightMax());

	//DEBUG_PRINT_FORCE ("\n[OffsetX]\n");
	//DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	//DEBUG_PRINT_FORCE ("  Max : %d\n",  WidthMax()-1);

	DEBUG_PRINT_FORCE ("\n[OffsetY]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", 0);
	DEBUG_PRINT_FORCE ("  Max : %d\n\n",  HeightMax()-1);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ROI Show
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiShow (void *str)
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
			cmdRoiShowHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = roiShow (0)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	}
	else if (argc == 2)
	{
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		if ((status = roiShow (mode)) != AVAL_STATUS_SUCCESS)
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
//	ROI Show Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiShowHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi information is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : roishow\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ROI Entry Count
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiEntryCount (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int count;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdRoiEntryCountHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = roiGetEntryCount (&count)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", count);
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
//	ROI Entry Count Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiEntryCountHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi entry count is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : roicount\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : ROI Entry Count\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[ROI Entry Count]\n");
	DEBUG_PRINT_FORCE ("  Min : %d\n", ROI_SELECTOR_MIN+1);
	DEBUG_PRINT_FORCE ("  Max : %d\n", ROI_SELECTOR_MAX+1);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ROI Camera Height Total
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiCameraHeightTotal (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int size;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdRoiCameraHeightTotalHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = fpgaRoiGetCameraHeightTotalSize (&size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", size);
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
//	ROI Camera Height Total Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiCameraHeightTotalHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi camera height total size is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : roicameraheighttotal\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : ROI Camera Height Total Size\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[ROI Camera Height Total Size]\n");
	DEBUG_PRINT_FORCE ("  Min : 1\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n\n",  HeightMax ());

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ROI Sensor Height Total
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiSensorHeightTotal (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int size;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdRoiSensorHeightTotalHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = fpgaRoiGetSensorHeightTotalSize (&size)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", size);
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
//	ROI Sensor Height Total Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiSensorHeightTotalHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi sensor height total size is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : roisensorheighttotal\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : ROI Sensor Height Total Size\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[ROI Sensor Height Total Size]\n");
	DEBUG_PRINT_FORCE ("  Min : 1\n");
	DEBUG_PRINT_FORCE ("  Max : %d\n\n",  sensorHeight ());

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ROI DefaultY
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiDefaultY (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdRoiDefaultYHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = roiSetDefaultY ()) != AVAL_STATUS_SUCCESS)
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
//	ROI DefaultY Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiDefaultYHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi height default is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : roidefaulty\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : ROI Height Default\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ROI Full Size
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiAreaSize (void *str)
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
			cmdRoiAreaSizeHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		if ((status = roiGetAreaSize (&mode)) != AVAL_STATUS_SUCCESS)
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

		if ((status = roiSetAreaSize (mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		if (mode == ROI_AREA_MODE_DMA_SIZE)
			mode = 0;

		if ((status = roiSetAreaFlag (mode)) != AVAL_STATUS_SUCCESS)
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
//	ROI Full Size Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiAreaSizeHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi area size is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : roiarea\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : Arera Mode");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi area size is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : roiarea [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : Arera Mode\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Area Mode]\n");
	DEBUG_PRINT_FORCE ("  Default   : %d\n", ROI_AREA_MODE_DEFAULT_SIZE);
	DEBUG_PRINT_FORCE ("  Full Size : %d\n", ROI_AREA_MODE_FULL_SIZE);
	DEBUG_PRINT_FORCE ("  DMA Size  : %d\n", ROI_AREA_MODE_DMA_SIZE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ROI Area Mode
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiAreaFlag (void *str)
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
			cmdRoiAreaFlagHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 取得
		if ((status = roiGetAreaFlag (&mode)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", mode);
	}
	else if (argc == 2)
	{
		// 取得
		if (sscanf (gCmdArg[1], "%d", &mode) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 設定
		if ((status = roiSetAreaFlag (mode)) != AVAL_STATUS_SUCCESS)
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
//	ROI Area Restore Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdRoiAreaFlagHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi area flag is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : roiareflag\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : area flag\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : The roi area flag is set.\n");
	DEBUG_PRINT_FORCE ("  Command           : roiareaflag [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : area flag\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Area Flag]\n");
	DEBUG_PRINT_FORCE ("  Default   : %d\n", ROI_AREA_MODE_DEFAULT_SIZE);
	DEBUG_PRINT_FORCE ("  Full Size : %d\n", ROI_AREA_MODE_FULL_SIZE);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}

// eof
