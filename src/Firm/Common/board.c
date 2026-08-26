//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// board.c - Board Access Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gInterFaceID = 0;
int gSpectrumType = CAMERA_TYPE_NO_SPECTRUM;


//**********************************************************************************
// Camera情報初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraInformationInitialize (void)
{
	cameraLogMsg (MSG_LEVEL_INFO, __FILE__, __func__, __LINE__, 0, "Camera Information Initialize\n");
#if 0 //@@@1
	// ボードID
	memset ((void *)FIRM_DATA_BOARDID_ADRS, 0x00, BOARD_PARAM_ALIGN);
	getBoardId ((char *)FIRM_DATA_BOARDID_ADRS);

	// センサID
	memset ((void *)FIRM_DATA_SENSORID_ADRS, 0x00, BOARD_PARAM_ALIGN);
	getSensorId ((char *)FIRM_DATA_SENSORID_ADRS);

	// ベンダ名取得
	memset ((void *)FIRM_DATA_VENDOR_ADRS, 0x00, BOARD_PARAM_VENDOR_SIZE);
	getVendor ((char *)FIRM_DATA_VENDOR_ADRS);
	
	// 製造者名取得
	memset ((void *)FIRM_DATA_MANUFACTURE_ADRS, 0x00, BOARD_PARAM_MANUFACTURE_SIZE);
	getManufacture ((char *)FIRM_DATA_MANUFACTURE_ADRS);

	// モデル名取得
	memset ((void *)FIRM_DATA_MODEL_ADRS, 0x00, BOARD_PARAM_MODEL_SIZE);
	getModel ((char *)FIRM_DATA_MODEL_ADRS);

	// ボードバージョン取得
	memset ((void *)FIRM_DATA_BOARD_VERSION_ADRS, 0x00, BOARD_VERSION_SIZE+1);
	boardVersion ((char *)FIRM_DATA_BOARD_VERSION_ADRS);

	// キャッシュFlash
	cacheFlushRange (FIRM_DATA_ADRS, FIRM_DATA_SIZE);
#else	//@@@1
	// ベンダ名取得
	memset ((void *)FIRM_DATA_VENDOR_ADRS, 0x00, BOARD_PARAM_VENDOR_SIZE);
	strcpy ((void *)FIRM_DATA_VENDOR_ADRS, (void *)VENDOR_NAME);
	
	// 製造者名取得
	memset ((void *)FIRM_DATA_MANUFACTURE_ADRS, 0x00, BOARD_PARAM_MANUFACTURE_SIZE);
	strcpy ((void *)FIRM_DATA_MANUFACTURE_ADRS, (void *)MANUFACTURE_NAME);

	// モデル名取得
	memset ((void *)FIRM_DATA_MODEL_ADRS, 0x00, BOARD_PARAM_MODEL_SIZE);
	strcpy ((void *)FIRM_DATA_MODEL_ADRS, (void *)MODEL_NAME_CXP);

	
#endif//@@@1

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ハイパースペクトルカメラかを取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pType				：ハイパースペクトルカメラかを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getSpectrumType (int *pType)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pType Parameter
	if (pType == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Spectrum Type pType NULL Parameter Error\n");
		goto _DONE;
	}

	// Check Camera ID
#ifdef MODE_SPECTRUM
		*pType = CAMERA_TYPE_SPECTRUM;
#else
		*pType = CAMERA_TYPE_NO_SPECTRUM;
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	DIPSW取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pDipsw				：DIPSWを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getDipsw (unsigned int *pDipsw)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data;

	// Check pDipsw Parameter
	if (pDipsw == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "DIPSW pDipsw NULL Parameter Error\n");
		goto _DONE;
	}

	// Get Dipsw
	//@@@1data = IN32 (ZYNQ_ULTRASCALE_GPIO_DIPSW_ADRS);
	//@@@1*pDipsw = ZYNQ_GPIO_DIPSW_MASK (data);
	*pDipsw = 0;
	
_DONE:
	return (status);
}


//**********************************************************************************
//	Board Reset
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//==================================================================================
int boardReset (void)
{
	// Acquisition Abort
	OUT32 (GENICAM_ACQUISITION_ABORT_ADRS, GENICAM_ACQUISITION_ABORT_BIT);


	// TG UnLock
	OUT32 (FPGA_TG_LOCK_ADRS, FPGA_TL_UNLOCK);

	// Disable Trg Control
	sensorSetTrgControl (MODE_DISABLE);

	usDelay (2);

	// Acquisition Abort解除
	OUT32 (GENICAM_ACQUISITION_ABORT_ADRS, 0);

	usDelay (300);

	return (AVAL_STATUS_SUCCESS);
}


//**********************************************************************************
//	ボードパラメータ設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pParam				：ボードパラメータを格納するポインタ
//		offset				：ボードパラメータオフセット
//		size				：ボードパラメータサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setBoardParam (char *pParam, int offset, int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int length;
	char *ptr = NULL;

	// Check pParam Parameter
	if (pParam == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Board Param pParam NULL Parameter Error\n");
		goto _DONE;
	}

	// データ長取得
	length = strlen ((char *)pParam);

	// データ長コンペア
	if (length > BOARD_PARAM_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Board Param Length(%d) Error. Length = %d\n", length, size);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリリクエスト
	if ((ptr = malloc (BOARD_PARAM_SIZE)) == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_RESOURCE_EXHAUSTED);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Board ID Buffer Request Error\n");
		goto _DONE;
	}

	// Board Parameter読み込み
	if ((status = qspiFlashRead (FLASH_BOARD_PARAM_ADRS, (unsigned char *)ptr, BOARD_PARAM_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// データ更新
	memcpy (&ptr[offset], pParam, size);

	// イレーズ
	if ((status = qspiFlashSectorErase (FLASH_BOARD_PARAM_ADRS)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Boardパラメータ書き込み
	if ((status = qspiFlashWrite (FLASH_BOARD_PARAM_ADRS, (unsigned char *)ptr, BOARD_PARAM_SIZE)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	// メモリ開放
	if (ptr != NULL)
		free (ptr);

	return (status);
}


//**********************************************************************************
//	ボードパラメータ取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pParam				：ボードパラメータを格納するポインタ
//		offset				：ボードパラメータオフセット
//		size				：ボードパラメータサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getBoardParam (char *pParam, int offset, int size)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pParam Parameter
	if (pParam == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Board Param pParam NULL Parameter Error\n");
		goto _DONE;
	}

	// Board Param読み込み
	if ((status = qspiFlashRead ((FLASH_BOARD_PARAM_ADRS + offset), (unsigned char *)pParam, size)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

_DONE:
	return (status);
}


//**********************************************************************************
//	基板シリアル設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pId					：基板シリアル番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setBoardId (char *pId)
{
	int status;
	char idSave[BOARD_PARAM_BOARD_ID_SIZE+1];
	unsigned int sizeMax = BOARD_PARAM_BOARD_ID_SIZE;
	
	// Check pId Parameter
	if (pId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Board ID pId NULL Parameter Error\n");
		goto _DONE;
	}

	// Set Board ID
	if ((status = setBoardParam (pId, BOARD_PARAM_BOARD_ID_ADRS, BOARD_PARAM_BOARD_ID_SIZE)) !=  AVAL_STATUS_SUCCESS)
		return (status);

	// Clear
	memset ((void *)idSave, 0x00, BOARD_PARAM_BOARD_ID_SIZE+1);
	
	// Board ID Copy
	strcpy (idSave, pId);

	// 共通メモリ領域クリア(管理外)
	memset ((void *)FIRM_DATA_BOARDID_ADRS, 0x00, sizeMax);

	// Board Copy(管理外)
	strcpy ((char *)FIRM_DATA_BOARDID_ADRS, (char *)idSave);

	// キャッシュFlash
	cacheFlushRange (FIRM_DATA_BOARDID_ADRS, BOARD_PARAM_BOARD_ID_SIZE);

_DONE:
	return (status);
}


//**********************************************************************************
//	基板シリアル取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pId					：基板シリアル番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getBoardId (char *pId)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pId Parameter
	if (pId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Board ID pId NULL Parameter Error\n");
		goto _DONE;
	}

	// 基板シリアル取得
	status = getBoardParam (pId, BOARD_PARAM_BOARD_ID_ADRS, BOARD_PARAM_BOARD_ID_SIZE);
	pId[BOARD_PARAM_BOARD_ID_SIZE] = '\0';

_DONE:
	return (status);
}


//**********************************************************************************
//	センサシリアル設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pId					：センサシリアル番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setSensorId (char *pId)
{
	int status = AVAL_STATUS_SUCCESS;
	char idSave[BOARD_PARAM_SENSOR_ID_SIZE+1];
	unsigned int sizeMax = BOARD_PARAM_SENSOR_ID_SIZE;

	// Check pId Parameter
	if (pId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor ID pId NULL Parameter Error\n");
		goto _DONE;
	}

	// Set Sensor ID
	if ((status = setBoardParam (pId, BOARD_PARAM_SENSOR_ID_ADRS, BOARD_PARAM_SENSOR_ID_SIZE)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// Clear
	memset ((void *)idSave, 0x00, BOARD_PARAM_SENSOR_ID_SIZE+1);
	
	// Board ID Copy
	strcpy (idSave, pId);

	// 共通メモリ領域クリア(管理外)
	memset ((void *)FIRM_DATA_SENSORID_ADRS, 0x00, sizeMax);

	// Sensor Copy(管理外)
	strcpy ((char *)FIRM_DATA_SENSORID_ADRS, (char *)idSave);

	// キャッシュFlash
	cacheFlushRange (FIRM_DATA_SENSORID_ADRS, BOARD_PARAM_SENSOR_ID_SIZE);

_DONE:
	return (status);
}


//**********************************************************************************
//	センサシリアル取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pId					：センサシリアル番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getSensorId (char *pId)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pId Parameter
	if (pId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Sensor ID pId NULL Parameter Error\n");
		goto _DONE;
	}

	// センサシリアル取得
	status = getBoardParam (pId, BOARD_PARAM_SENSOR_ID_ADRS, BOARD_PARAM_SENSOR_ID_SIZE);
	pId[BOARD_PARAM_SENSOR_ID_SIZE] = '\0';

_DONE:
	return (status);
}


//**********************************************************************************
//	インターフェイス番号取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pId					：インターフェイス番号を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getInterfaceId (int *pId)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pId Parameter
	if (pId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Interface ID pId NULL Parameter Error\n");
		goto _DONE;
	}

	// InterFace ID取得
	*pId = IN32 (FPGA_INTERFACE_ID_ADRS);

#if 0
	*pId = INTERFACE_CAMERALINK;
	*pId = INTERFACE_GIGE;
	*pId = INTERFACE_GIGE20;
#endif

_DONE:
	return (status);
}


//**********************************************************************************
//	User ID設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pId					：User IDを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setUserId (char *pId)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pAdrs;
	void *memAdrs;
	int len;

	// Check pId Parameter
	if (pId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "User ID pId NULL Parameter Error\n");
		goto _DONE;
	}

	// データ長取得
	len = strlen (pId);
	if (len > CAMERA_SAVE_USERID_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "User ID Length(%d) Parameter Error. (Min:1 - Max:%d)\n", len, CAMERA_SAVE_USERID_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get User Address
	if ((status = cameraParamGetUserAdrs (CAMERA_SAVE_USER_NUM, &pAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// アドレス
	memAdrs = (void *)((unsigned long)pAdrs + CAMERA_SAVE_USERID_ADRS);

	// メモリクリア
	memset ((void *)memAdrs, 0, CAMERA_SAVE_USERID_SIZE);

	// ユーザーID Copy
	memcpy ((char *)memAdrs, (char *)pId, len);

_DONE:
	return (status);
}


//**********************************************************************************
//	User ID取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pId					：User IDを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getUserId (char *pId)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int *pAdrs;
	char *adrs;
	unsigned int len;
	unsigned char data;

	// Check pId Parameter
	if (pId == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "User ID pId NULL Parameter Error\n");
		goto _DONE;
	}

	// Get User Address
	if ((status = cameraParamGetUserAdrs (CAMERA_SAVE_USER_NUM, &pAdrs)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	// ユーザーIDアドレス検索
	adrs = (char *)((unsigned long)pAdrs + CAMERA_SAVE_USERID_ADRS);

	// NULL検索
	for (len=0; len<CAMERA_SAVE_USERID_SIZE; len++)
	{
		data = IN8 ((adrs+len));
		if (data == 0)
			break;
		else if ((data < 0x20) || (data > 0x7f))
			break;
	}

	// サイズ確認
	if (len>CAMERA_SAVE_USERID_SIZE)
		len = CAMERA_SAVE_USERID_SIZE;

	// Clear
	memset ((char *)pId, 0x00, CAMERA_SAVE_USERID_SIZE);

	// User ID Copy
	memcpy ((char *)pId, (char *)adrs, len);

	// Set NULL
	pId[len] = 0;

	// キャッシュFlash
	cacheFlushRange ((unsigned long)pId, CAMERA_SAVE_USERID_SIZE);

_DONE:
	return (status);
}


//**********************************************************************************
//	Vendor名設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pId					：Vendor名を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setVendor (char *pName)
{
	int status = AVAL_STATUS_SUCCESS;
	int len;
	unsigned int sizeMax = BOARD_PARAM_VENDOR_SIZE;
	char tempBuff[BOARD_PARAM_VENDOR_SIZE];

	// Check pName Parameter
	if (pName == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Vendor pName NULL Parameter Error\n");
		goto _DONE;
	}

	// データ長取得
	len = strlen (pName);
	if (len > (sizeMax - 1))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Vendor Length(%d) Parameter Error. (Min:1 - Max:%d)\n", len, sizeMax - 1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリ領域クリア
	memset ((void *)tempBuff, 0x00, sizeMax);

	// モデル名 Copy
	strcpy ((char *)tempBuff, (char *)pName);

	// Set Model
	if((status = setBoardParam (tempBuff, BOARD_PARAM_VENDOR_ADRS, sizeMax)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 共通メモリ領域クリア
	memset ((void *)FIRM_DATA_VENDOR_ADRS, 0x00, sizeMax);

	// モデル名 Copy
	strcpy ((char *)FIRM_DATA_VENDOR_ADRS, (char *)tempBuff);

	// キャッシュFlash
	cacheFlushRange (FIRM_DATA_VENDOR_ADRS, BOARD_PARAM_VENDOR_SIZE);

_DONE:
	return (status);
}


//**********************************************************************************
//	Vendor名取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pName					：ベンダ名を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getVendor (char *pName)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pName Parameter
	if (pName == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Vendor Name pName NULL Parameter Error\n");
		goto _DONE;
	}

	// ベンダ名 Copy
	if ((status = getBoardParam (pName, BOARD_PARAM_VENDOR_ADRS, BOARD_PARAM_VENDOR_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		pName[0] = '\0';
		return (status);
	}

	// 設定されていなければデフォルト名
	if (((pName[0] < 0x20) || (pName[0] > 0x7f)) && (pName[0] != 0x00))
	{
		memset ((void *)pName, 0x00, BOARD_PARAM_VENDOR_SIZE);
		strcpy (pName, VENDOR_NAME);
	}
	
	pName[(BOARD_PARAM_VENDOR_SIZE-1)] = '\0';

_DONE:
	return (status);
}


//**********************************************************************************
//	Manufacture名設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pId					：Vendor名を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setManufacture (char *pName)
{
	int status = AVAL_STATUS_SUCCESS;
	//unsigned int memAdrs, romAdrs;
	int len;
	unsigned int sizeMax = BOARD_PARAM_MANUFACTURE_SIZE;
	char tempBuff[BOARD_PARAM_MANUFACTURE_SIZE];

	// Check pName Parameter
	if (pName == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Manufacture pName NULL Parameter Error\n");
		goto _DONE;
	}

	// データ長取得
	len = strlen (pName);
	if (len > (sizeMax - 1))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Manufacture Length(%d) Parameter Error. (Min:1 - Max:%d)\n", len, sizeMax - 1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリ領域クリア
	memset ((void *)tempBuff, 0x00, sizeMax);

	// モデル名 Copy
	strcpy ((char *)tempBuff, (char *)pName);

	// Set Model
	if((status = setBoardParam (tempBuff, BOARD_PARAM_MANUFACTURE_ADRS, sizeMax)) !=  AVAL_STATUS_SUCCESS)
		goto _DONE;

	// 共通メモリ領域クリア
	memset ((void *)FIRM_DATA_MANUFACTURE_ADRS, 0x00, sizeMax);

	// モデル名 Copy
	strcpy ((char *)FIRM_DATA_MANUFACTURE_ADRS, (char *)tempBuff);

	// キャッシュFlash
	cacheFlushRange (FIRM_DATA_MANUFACTURE_ADRS, BOARD_PARAM_MANUFACTURE_SIZE);

_DONE:
	return (status);
}


//**********************************************************************************
//	Manufacture名取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pName					：製造者名を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getManufacture (char *pName)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pName Parameter
	if (pName == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Manufacture Name pName NULL Parameter Error\n");
		goto _DONE;
	}

	// 製造者名Copy
	if ((status = getBoardParam (pName, BOARD_PARAM_MANUFACTURE_ADRS, BOARD_PARAM_MANUFACTURE_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		pName[0] = '\0';
		return (status);
	}

	// 設定されていなければデフォルト名
	if (((pName[0] < 0x20) || (pName[0] > 0x7f)) && (pName[0] != 0x00))
	{
		memset ((void *)pName, 0x00, BOARD_PARAM_MANUFACTURE_SIZE);
		strcpy (pName, MANUFACTURE_NAME);
	}

	pName[(BOARD_PARAM_MANUFACTURE_SIZE-1)] = '\0';

_DONE:
	return (status);
}


//**********************************************************************************
//	Model名設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pName				：Model名を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int setModel (char *pName)
{
	int status = AVAL_STATUS_SUCCESS;
	int len;
	unsigned int sizeMax = BOARD_PARAM_MODEL_SIZE;
	char tempBuff[BOARD_PARAM_MODEL_SIZE];

	// Check pName Parameter
	if (pName == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Model pName NULL Parameter Error\n");
		goto _DONE;
	}

	// データ長取得
	len = strlen (pName);
	if (len > (sizeMax - 1))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Model Length(%d) Parameter Error. (Min:1 - Max:%d)\n", len, sizeMax - 1);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// メモリ領域クリア
	memset ((void *)tempBuff, 0x00, sizeMax);

	// モデル名 Copy
	strcpy ((char *)tempBuff, (char *)pName);

	// Set Model
	if((status = setBoardParam (tempBuff, BOARD_PARAM_MODEL_ADRS, sizeMax)) !=  AVAL_STATUS_SUCCESS)
		return (status);

	// 共通メモリ領域クリア
	memset ((void *)FIRM_DATA_MODEL_ADRS, 0x00, sizeMax);

	// モデル名 Copy
	strcpy ((char *)FIRM_DATA_MODEL_ADRS, (char *)tempBuff);

	// キャッシュFlash
	cacheFlushRange (FIRM_DATA_MODEL_ADRS, BOARD_PARAM_MODEL_SIZE);

_DONE:
	return (status);
}


//**********************************************************************************
//	Model名取得
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pName				：Model名を格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int getModel (char *pName)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pName Parameter
	if (pName == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Model Name pName NULL Parameter Error\n");
		goto _DONE;
	}

	// Model名Copy
	if ((status = getBoardParam (pName, BOARD_PARAM_MODEL_ADRS, BOARD_PARAM_MODEL_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		pName[0] = '\0';
		return (status);
	}
	
	// 設定されていなければデフォルト名
	if (((pName[0] < 0x20) || (pName[0] > 0x7f)) && (pName[0] != 0x00))
	{
		// クリア
		memset ((void *)pName, 0x00, BOARD_PARAM_MODEL_SIZE);

		if (gInterFaceID == INTERFACE_CAMERALINK)
			strcpy (pName, MODEL_NAME_CAMERALINK);
		else if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
			strcpy (pName, MODEL_NAME_GIGE);
		else if (gInterFaceID == INTERFACE_CXP)
			strcpy (pName, MODEL_NAME_CXP);
	}
	else
	{
		pName[(BOARD_PARAM_MODEL_SIZE-1)] = '\0';
	}

_DONE:
	return (status);
}


//**********************************************************************************
//	XML Fileメモリロード
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int xmlFileLoadMemory (void)
{
	int status = AVAL_STATUS_SUCCESS;

	status = qspiFlashRead(FLASH_XML_ADRS, (unsigned char *)FIRM_XML_FILE_ADRS, FLASH_XML_SIZE);

	return (status);
}


//**********************************************************************************
//	Camera Power Supply
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pData				：データを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int cameraPowerSupply (unsigned int *pData)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pData Parameter
	if (pData == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Camera Power Supply NULL Parameter Error\n");
		goto _DONE;
	}

	*pData = IN32 (FPGA_POWER_STATUS_ADRS);

_DONE:
	return (status);
}

// eof
