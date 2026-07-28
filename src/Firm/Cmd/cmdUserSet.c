//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdUserset.c.c - UserSet Command Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//**********************************************************************************
//	ユーザーモードを確認
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUser (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int user;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCameraUserHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Userカメラパラメータ取得
		if ((status = cameraParamCommonReadMem (CAMERA_SAVE_COMMON_USER_PARAM_MODE_ADRS, (unsigned int *)&user)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	
		DEBUG_PRINT_FORCE ("%d ", user);
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
//	User Set取得 Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : The userset number which is being used at present is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : user [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : userset number(Min:%d - Max:%d)\n", CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	起動ユーザーセットを設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserBoot (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int user;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCameraUserBootHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// 起動Userカメラパラメータ取得
		if ((status = userGetBoot (&user)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

		DEBUG_PRINT_FORCE ("%d ", user);
	}
	else if (argc == 2)
	{
		// 起動ユーザー番号取得
		if (sscanf (gCmdArg[1], "%d", &user) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// 起動Userカメラパラメータ設定
		if ((status = userSetBoot (user)) != AVAL_STATUS_SUCCESS)
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
//	起動ユーザーセットを設定Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserBootHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Get]\n");
	DEBUG_PRINT_FORCE ("  Function          : When booting the used userset number is acquired.\n");
	DEBUG_PRINT_FORCE ("  Command           : userset-default/boot\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : userset number(Min:%d - Max:%d)\n", CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
	DEBUG_PRINT_FORCE ("\n");

	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : When booting the used userset number is established.\n");
	DEBUG_PRINT_FORCE ("  Command           : userset-default/boot [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : userset number(Min:%d - Max:%d)\n", CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	カメラパラメータ保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserSave (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int user;
	
	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCameraUserSaveHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// 保存ユーザー番号取得(Ver.1.1)
		if (sscanf (gCmdArg[1], "%d", &user) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// カメラパラメータ保存(ユーザー領域)
		if ((status = userSetSave (user)) != AVAL_STATUS_SUCCESS)
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
//	カメラパラメータ保存Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserSaveHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A camera parameter is preserved in the user set number which is being used at present.\n");
	DEBUG_PRINT_FORCE ("  Command           : userset-save/save [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : userset number(Min:%d - Max:%d)\n", CAMERA_USER1_MODE, CAMERA_USER2_MODE);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	指定ユーザーモードでレジスタをリロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserLoad (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int user;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCameraUserLoadHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// ユーザー番号取得
		if (sscanf (gCmdArg[1], "%d", &user) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// Userカメラパラメータリストア
		if ((status = userSetLoad (user)) != AVAL_STATUS_SUCCESS)
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
//	指定ユーザーモードでレジスタをリロードHelp
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserLoadHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A camera parameter is loaded by a designated userset.\n");
	DEBUG_PRINT_FORCE ("  Command           : userset-load/load [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : userset number(Min:%d - Max:%d)\n", CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	カメラパラメータクリア(ユーザー領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserSetDefault (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	int user;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCameraUserSetDefaultHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 2)
	{
		// ユーザー番号取得
		if (sscanf (gCmdArg[1], "%d", &user) != 1)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, CMD_ERROR_INVALID_PARAM);
			goto _DONE;
		}

		// UserカメラパラメータClear
		if ((status = userSetDefault (user)) != AVAL_STATUS_SUCCESS)
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
//	カメラパラメータクリア(ユーザー領域)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraUserSetDefaultHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A camera parameter of a designation user set is cleared.\n");
	DEBUG_PRINT_FORCE ("  Command           : factory-default/factory [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : userset number(Min:%d - Max:%d)\n", CAMERA_USER1_MODE, CAMERA_USER2_MODE);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	カメラパラメータクリア(共通領域)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraCommonClear (void *str)
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
			cmdCameraCommonClearHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// Common領域クリア
		if ((status = cameraParamCommonMarkClear ()) != AVAL_STATUS_SUCCESS)
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
//	カメラパラメータクリア(共通領域)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraCommonClearHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : A Common parameter is cleared.\n");
	DEBUG_PRINT_FORCE ("  Command           : userset-common-clear [param]\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : userset number(Min:%d - Max:%d)\n", CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	工場出荷部のマークを消す(次回再起動時にデフォルトのレジスタ情報を保持)
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraFactoryClear (void *str)
{
	int status = AVAL_STATUS_SUCCESS;
	int argc;
	unsigned int data;

	// Get Argument
	argc = cmdCheckArg ((char *)str);

	// Help?
	if (argc == 2)
	{
		if (strcmp (gCmdArg[1], CMD_HELP_OPTION) == 0)
		{
			cmdCameraFactoryClearHelp (NULL);
			goto _DONE;
		}
	}

	if (argc == 1)
	{
		// カメラパラメータクリア
		data = 0xffffffff;
		if ((status = cameraParamUserWriteRomMulti (CAMERA_FACTORY_NUM, CAMERA_SAVE_MARK_ADRS, (unsigned char *)&data, 4)) != AVAL_STATUS_SUCCESS)
			goto _DONE;

#if defined (MODE_USERSET_SPECTRUM_PARAM)
		// カメラパラメータクリア(Spectrum領域)
		if ((status = cameraParamSpectrumWriteRomMulti (CAMERA_FACTORY_NUM, CAMERA_SAVE_MARK_ADRS, (unsigned char *)&data, 4)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
#endif // #if defined (MODE_USERSET_SPECTRUM_PARAM)

		// 起動Userカメラパラメータ設定
		if ((status = userSetBoot (CAMERA_FACTORY_NUM)) != AVAL_STATUS_SUCCESS)
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
//	工場出荷部のマークを消す(次回再起動時にデフォルトのレジスタ情報を保持)Help
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		str						：文字列を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS		：正常終了
//		上記以外					：異常終了
//==================================================================================
int cmdCameraFactoryClearHelp (void *str)
{
	DEBUG_PRINT_FORCE ("\n");
	DEBUG_PRINT_FORCE ("[Set]\n");
	DEBUG_PRINT_FORCE ("  Function          : Factory shipment parameter is cleared.\n");
	DEBUG_PRINT_FORCE ("  Command           : userset-factory-clear\n");
	DEBUG_PRINT_FORCE ("  Input  Param      : none\n");
	DEBUG_PRINT_FORCE ("  Output Param      : none\n");
	DEBUG_PRINT_FORCE ("\n");

	return (AVAL_STATUS_SUCCESS);
}


// eof
