//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// gigeParam.c - GigE Camera Parameter Initialize Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#include "../GigE/gev.h"
#include "../GigE/gige.h"

#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
#include "../mtd/mtdFeatures.h"
#include "../mtd/mtdApiTypes.h"
#include "../mtd/mtdAPI.h"
#endif


#if defined (MODE_ACQUISITION_TRG_EXTEND)
//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define GIGE_EVENT_PARAM_REG_SIZE			(9)	


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
int gGigEStatus = 0;				// GigE Status


//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
#if defined (MODE_CAMERA_EVENT_VERSION2)
extern volatile u32 *Event_Notification;
#else
extern volatile u32 Event_Notification[];
#endif

extern unsigned int *gpCameraUserParameter;


//**********************************************************************************
//	GigE Camera Parameter Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeParamInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Register Data Restore
	if ((status = gigeParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_GIGE_ADRS, CAMERA_SAVE_GIGE_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gGigEStatus = status;
		goto _DONE;;
	}

_DONE:
	return (status);
}


//**********************************************************************************
// カメラ保存パラメータをレジスタに設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		mode				：ユーザーモード(0=User0/1=User1/2=User2)
//		offset				：設定パラメータが保存されているオフセット
//		size				：設定パラメータが保存されているサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeParamWriteRegister (int userNum, unsigned int offset, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	SAVE_PARAM	*pSaveparam;
	unsigned int data;
	unsigned int total;
	int i, bit;
	int index;

	// Check userNum Parameter
	if ((userNum < CAMERA_USER_MODE_MIN) || (userNum > CAMERA_USER_MODE_MAX))
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_GIGE, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save GigE(Register Write) User Parameter Mode(%d) Parameter Error. (Min:%d / Max:%d)\n", userNum, CAMERA_USER_MODE_MIN, CAMERA_USER_MODE_MAX);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Check offset/size Parameter
	if ((offset + size) > CAMERA_SAVE_USER_SIZE)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_GIGE, AVAL_STATUS_INVALID_PARAMETER);
		sprintf (gLogMsgBuff, "Camera Save GigE(Register Write) User Parameter Save Size(0x%x) Parameter Error. (Max:%d)\n", (offset + size), CAMERA_SAVE_USER_SIZE);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
		goto _DONE;
	}

	// Get Save Parameter Address
	if (gpCameraUserParameter != NULL)
	{
		// Get Address
		pSaveparam = (SAVE_PARAM *)gpCameraUserParameter;

		// Get Index
		index = offset / sizeof (SAVE_PARAM);

		total = 0;
		for (i=0; i<GIGE_EVENT_PARAM_REG_SIZE; i++)
		{
			// 保存データ取得
			data = pSaveparam[(index + i)].data;

			for (bit=0; bit<8; bit++)
			{
				if (EventNotification_Max < total)
					goto _DONE;
				
				if ((data & (1<<bit)) != 0)
					Event_Notification[total] = 1;
				else
					Event_Notification[total] = 0;

				total++;
			}
		}
	}

_DONE:
	return (status);
}


//**********************************************************************************
// レジスタ情報を取得しカメラパラメータ領域へ展開その後EEPROMへ保存
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		userNum				：保存するユーザーパラメータ番号
//		offset				：設定パラメータが保存されているオフセット
//		size				：設定パラメータが保存されているサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeParamSaveRegister (int userNum,  unsigned int offset, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	int i, j, bit, index;
	int total;
	SAVE_PARAM	*pSaveparam = (SAVE_PARAM *)gpCameraUserParameter;
	unsigned int saveReg;

	// Get Index
	index = offset / sizeof (SAVE_PARAM);

	total = 0;
	for (i=0; i<GIGE_EVENT_PARAM_REG_SIZE; i++)
	{
		saveReg = 0;
		for (bit=0; bit<8; bit++)
		{
			if (EventNotification_Max < total)
				break;

			if (Event_Notification[total] == 1)
				saveReg |= (1<<bit);

			total++;
		}
		
		// 保存
		j = (total-1) / 8;
		pSaveparam[(index + j)].data = saveReg;
	}

	return (status);
}


//**********************************************************************************
// カメラ保存パラメータをデフォルトに設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeParamDefault (void)
{
	int status = AVAL_STATUS_SUCCESS;
	int i;

	for (i=0; i<EventNotification_Max; i++)
		Event_Notification[i] = 0;

	return (status);
}


#if defined (MODE_GE_PACKET_INFO_SAVE)
//**********************************************************************************
//	GigE Camera Parameter Initialize2
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeParamInitialize2 (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_GEV_REG_ADRS, CAMERA_GEV_REG_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		gGigEStatus = status;
		goto _DONE;;
	}

_DONE:
	return (status);
}
#endif

#endif // #if defined (MODE_ACQUISITION_TRG_EXTEND)



#if defined (MODE_GE_SPEED)
//**********************************************************************************
//	Gev Initialize
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSpeed				：speedを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gevParamInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;

	// Register Data Restore
	if ((status = cameraParamWriteRegister (CAMERA_SAVE_USER_NUM, CAMERA_GEV_PARAM_ADRS, CAMERA_GEV_PARAM_SIZE)) != AVAL_STATUS_SUCCESS)
	{
		return (status);
	}

	return (status);
}


//**********************************************************************************
//	Get gev speed
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		pSpeed				：speedを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gevGetSpeedConfig (unsigned int *pSpeed)
{
	int status = AVAL_STATUS_SUCCESS;

	// Check pSpeed Parameter
	if (pSpeed == NULL)
	{
		status = MAKE_ERROR_STATUS (AVAL_STATUS_GIGE, AVAL_STATUS_INVALID_PARAMETER);
		cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Gev Get Speed Config NULL Parameter Error\n");
		goto _DONE;
	}

	// Read Data
	*pSpeed = IN32 (FIRM_DATA_GEV_SPEED_ADRS);

_DONE:
	return (status);
}


//**********************************************************************************
//	Set gev speed
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		speed				：speed
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gevSetSpeedConfig (unsigned int speed)
{
	int status = AVAL_STATUS_SUCCESS;

	switch (speed)
	{
		case GIGE_SPEED_AUTO_NEGOTIATION:
		case GIGE_SPEED_1GIG_FD:
		case GIGE_SPEED_10GIG_FD:
		case GIGE_SPEED_2P5GIG_FD:
		case GIGE_SPEED_5GIG_FD:
	
			// Write Data
			OUT32 (FIRM_DATA_GEV_SPEED_ADRS, speed);
			break;
		
		default:
			status = MAKE_ERROR_STATUS (AVAL_STATUS_GIGE, AVAL_STATUS_INVALID_PARAMETER);
			sprintf (gLogMsgBuff, "Gev set Speed(0x%x) Config Parameter Error\n", speed);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
			break;
	}		

	return (status);
}
#endif // #if defined (MODE_GE_SPEED)

// eof
