//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmdTable.h - Command Table
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


//----------------------------------------------------------------------------------
// Command Table
//----------------------------------------------------------------------------------
CMD_TBL mainCmdTbl [] =
{
	// name      			 				 : func         		     			: Help, Func									:optArg	

//============================================================================================================================================
// Command History
//============================================================================================================================================
	{ (char *)"history",					(CMDFUNC)cmdHistoryShow,				(CMDFUNC)cmdHistoryShowHelp, 					OPT_NONE},
	{ (char *)"historyclear",				(CMDFUNC)cmdHistoryClear,				(CMDFUNC)cmdHistoryClearHelp, 					OPT_NONE},


//============================================================================================================================================
// Camera
//============================================================================================================================================
#if (CAMERA_TYPE == CAMERA_TYPE_AREA)
	{ (char *)"acquisitionmode",			(CMDFUNC)cmdAcquisitionMode,			(CMDFUNC)cmdAcquisitionModeHelp, 				OPT_NONE},
	{ (char *)"mode",						(CMDFUNC)cmdAcquisitionMode,			(CMDFUNC)cmdAcquisitionModeHelp, 				OPT_NONE},
	{ (char *)"framecount",					(CMDFUNC)cmdAcquisitionFrameCount,		(CMDFUNC)cmdAcquisitionFrameCountHelp, 			OPT_NONE},
	{ (char *)"acquisitionframecount",		(CMDFUNC)cmdAcquisitionFrameCount,		(CMDFUNC)cmdAcquisitionFrameCountHelp, 			OPT_NONE},
#endif

	{ (char *)"start",		   		 		(CMDFUNC)cmdAcquisitionStart,			(CMDFUNC)cmdAcquisitionStartHelp, 				OPT_NONE},
	{ (char *)"acquisitionstart",			(CMDFUNC)cmdAcquisitionStart,			(CMDFUNC)cmdAcquisitionStartHelp, 				OPT_NONE},
	{ (char *)"stop",		   	 			(CMDFUNC)cmdAcquisitionStop,			(CMDFUNC)cmdAcquisitionStopHelp, 				OPT_NONE},
	{ (char *)"acquisitionstop",			(CMDFUNC)cmdAcquisitionStop,			(CMDFUNC)cmdAcquisitionStopHelp, 				OPT_NONE},
	{ (char *)"acquisitionselectstatus",	(CMDFUNC)cmdAcquisitionStatus,			(CMDFUNC)cmdAcquisitionStatusHelp,			 	OPT_NONE},
	{ (char *)"acstatus",					(CMDFUNC)cmdAcquisitionStatus,			(CMDFUNC)cmdAcquisitionStatusHelp, 				OPT_NONE},
	{ (char *)"acquisition-reset",			(CMDFUNC)cmdAcquisitionReset,			(CMDFUNC)cmdAcquisitionResetHelp, 				OPT_NONE},
	{ (char *)"acreset",					(CMDFUNC)cmdAcquisitionReset,			(CMDFUNC)cmdAcquisitionResetHelp, 				OPT_NONE},

	{ (char *)"frame",	    				(CMDFUNC)cmdAcquisitionFramerate,		(CMDFUNC)cmdAcquisitionFramerateHelp, 			OPT_NONE},
	{ (char *)"framerate",	    			(CMDFUNC)cmdAcquisitionFramerate,		(CMDFUNC)cmdAcquisitionFramerateHelp, 			OPT_NONE},
	{ (char *)"exp",			   			(CMDFUNC)cmdAcquisitionExposure,		(CMDFUNC)cmdAcquisitionExposureHelp, 			OPT_NONE},
	{ (char *)"exposure",		  	 		(CMDFUNC)cmdAcquisitionExposure,		(CMDFUNC)cmdAcquisitionExposureHelp, 			OPT_NONE},
	{ (char *)"exp-mode",					(CMDFUNC)cmdAcquisitionExposureMode,	(CMDFUNC)cmdAcquisitionExposureModeHelp,	 	OPT_NONE},

	{ (char *)"exposuremin",		   		(CMDFUNC)cmdExposureMin,				(CMDFUNC)cmdExposureMinHelp, 					OPT_NONE},
	{ (char *)"exposuremax",		   		(CMDFUNC)cmdExposureMax,				(CMDFUNC)cmdExposureMaxHelp, 					OPT_NONE},
	{ (char *)"ratemin",		   			(CMDFUNC)cmdRateMin,					(CMDFUNC)cmdRateMinHelp, 						OPT_NONE},
	{ (char *)"ratemax",		   			(CMDFUNC)cmdRateMax,					(CMDFUNC)cmdRateMaxHelp, 						OPT_NONE},

	{ (char *)"sensor-read-out",			(CMDFUNC)cmdSensoreGetReadOut,			(CMDFUNC)cmdSensoreGetReadOutHelp, 				OPT_NONE},

	
//============================================================================================================================================
// High Speed Mode
//============================================================================================================================================

#if defined (MODE_FRAMERATE_HIGH_SPEED)
	{ (char *)"hsm",						(CMDFUNC)cmdSensorFrameRateHighSpeedMode,	(CMDFUNC)cmdSensorFrameRateHighSpeedModeHelp, 	OPT_NONE},
	{ (char *)"vhmode",						(CMDFUNC)cmdSensorVirtualHeightMode,		(CMDFUNC)cmdSensorVirtualHeightModeHelp, 		OPT_NONE},
	{ (char *)"vhlinescan",					(CMDFUNC)cmdSensorVirtualHeightLineScanMode,(CMDFUNC)cmdSensorVirtualHeightLineScanModeHelp,OPT_NONE},
	{ (char *)"virtualheight",				(CMDFUNC)cmdSensorVirtualHeight,			(CMDFUNC)cmdSensorVirtualHeightHelp, 			OPT_NONE},
#endif


//============================================================================================================================================
// Timing Generator Version2
//============================================================================================================================================
	{ (char *)"tgse",						(CMDFUNC)cmdTgse,						(CMDFUNC)cmdTgseHelp, 							OPT_NONE},
	{ (char *)"tges",						(CMDFUNC)cmdTges,						(CMDFUNC)cmdTgesHelp, 							OPT_NONE},
	{ (char *)"tgpd",						(CMDFUNC)cmdTgpd,						(CMDFUNC)cmdTgpdHelp, 							OPT_NONE},


//============================================================================================================================================
// Trigger
//============================================================================================================================================
	{ (char *)"trg-selector",				(CMDFUNC)cmdAcquisitionTrgSelect,		(CMDFUNC)cmdAcquisitionTrgSelectHelp, 			OPT_NONE},
	{ (char *)"trg-mode",					(CMDFUNC)cmdAcquisitionTrgMode,			(CMDFUNC)cmdAcquisitionTrgModeHelp, 			OPT_NONE},
	{ (char *)"trg-soft",	    			(CMDFUNC)cmdAcquisitionSoftTrg,			(CMDFUNC)cmdAcquisitionSoftTrgHelp, 			OPT_NONE},
	{ (char *)"trg-source",					(CMDFUNC)cmdAcquisitionTrgSource,		(CMDFUNC)cmdAcquisitionTrgSourceHelp, 			OPT_NONE},
	{ (char *)"trg-activation",				(CMDFUNC)cmdAcquisitionTrgActivation,	(CMDFUNC)cmdAcquisitionTrgActivationHelp, 		OPT_NONE},
	{ (char *)"trg-delay",					(CMDFUNC)cmdAcquisitionTrgDelay,		(CMDFUNC)cmdAcquisitionTrgDelayHelp, 			OPT_NONE},
	{ (char *)"trg-reserved",				(CMDFUNC)cmdAcquisitionTrgReserved,		(CMDFUNC)cmdAcquisitionTrgReservedHelp, 		OPT_NONE},
	{ (char *)"trg-invalidcount",			(CMDFUNC)cmdAcquisitionTrgInvalidCount,	(CMDFUNC)cmdAcquisitionTrgInvalidCountHelp, 	OPT_NONE},

	{ (char *)"trg-count",				  	(CMDFUNC)cmdAcquisitionTrgCount,		(CMDFUNC)cmdAcquisitionTrgCountHelp, 			OPT_NONE},
	{ (char *)"trg-countmode",				(CMDFUNC)cmdCounterSetTrgMode,			(CMDFUNC)cmdCounterSetTrgModeHelp, 				OPT_NONE},
	{ (char *)"trg-signalcount",			(CMDFUNC)cmdCounterGetTrgInputCount,	(CMDFUNC)cmdCounterGetTrgInputCountHelp, 		OPT_NONE},
	{ (char *)"trg-imgcount",				(CMDFUNC)cmdCounterGetImageOutputCount,	(CMDFUNC)cmdCounterGetImageOutputCountHelp, 	OPT_NONE},


//============================================================================================================================================
// ROI
//============================================================================================================================================
	{ (char *)"bit",						(CMDFUNC)cmdAoiBitWidth,				(CMDFUNC)cmdAoiBitWidthHelp, 					OPT_NONE},
	{ (char *)"pixel-depth",				(CMDFUNC)cmdAoiBitWidth,				(CMDFUNC)cmdAoiBitWidthHelp, 					OPT_NONE},
	{ (char *)"widthmax",					(CMDFUNC)cmdWidthMax,					(CMDFUNC)cmdWidthMaxHelp, 						OPT_NONE},
	{ (char *)"heightmax",					(CMDFUNC)cmdHeightMax,					(CMDFUNC)cmdHeightMaxHelp, 						OPT_NONE},
	{ (char *)"sensor-width",				(CMDFUNC)cmdSensorWidth,				(CMDFUNC)cmdSensorWidthHelp, 					OPT_NONE},
	{ (char *)"sensor-height",				(CMDFUNC)cmdSensorHeight,				(CMDFUNC)cmdSensorHeightHelp, 					OPT_NONE},

	{ (char *)"width",						(CMDFUNC)cmdAoiWidth,					(CMDFUNC)cmdAoiWidthHelp, 						OPT_NONE},
	{ (char *)"height",						(CMDFUNC)cmdAoiHeight,					(CMDFUNC)cmdAoiHeightHelp, 						OPT_NONE},
	{ (char *)"offsetx",					(CMDFUNC)cmdAoiWidthOffset,				(CMDFUNC)cmdAoiWidthOffsetHelp, 				OPT_NONE},
	{ (char *)"offsety",					(CMDFUNC)cmdAoiHeightOffset,			(CMDFUNC)cmdAoiHeightOffsetHelp, 				OPT_NONE},

	{ (char *)"flip",						(CMDFUNC)cmdAoiXflip,					(CMDFUNC)cmdAoiXflipHelp, 						OPT_NONE},

	{ (char *)"roiselector",				(CMDFUNC)cmdRoiSelector,				(CMDFUNC)cmdRoiSelectorHelp, 					OPT_NONE},
	{ (char *)"roimultiy",					(CMDFUNC)cmdRoiMultiY,					(CMDFUNC)cmdRoiMultiYHelp, 						OPT_NONE},
	{ (char *)"roishow",					(CMDFUNC)cmdRoiShow,					(CMDFUNC)cmdRoiShowHelp, 						OPT_NONE},
	{ (char *)"roicount",					(CMDFUNC)cmdRoiEntryCount,				(CMDFUNC)cmdRoiEntryCountHelp, 					OPT_NONE},

	{ (char *)"roicameraheighttotal",		(CMDFUNC)cmdRoiCameraHeightTotal,		(CMDFUNC)cmdRoiCameraHeightTotalHelp, 			OPT_NONE},
	{ (char *)"roisensorheighttotal",		(CMDFUNC)cmdRoiSensorHeightTotal,		(CMDFUNC)cmdRoiSensorHeightTotalHelp, 			OPT_NONE},

	{ (char *)"roidefaulty",				(CMDFUNC)cmdRoiDefaultY,				(CMDFUNC)cmdRoiDefaultYHelp, 					OPT_NONE},
	{ (char *)"roiarea",					(CMDFUNC)cmdRoiAreaSize,				(CMDFUNC)cmdRoiAreaSizeHelp, 					OPT_NONE},
	{ (char *)"roiareaflag",				(CMDFUNC)cmdRoiAreaFlag,				(CMDFUNC)cmdRoiAreaFlagHelp, 					OPT_NONE},


//============================================================================================================================================
// Binning
//============================================================================================================================================
	{ (char *)"binningx",					(CMDFUNC)cmdAoiBinningX,				(CMDFUNC)cmdAoiBinningXHelp, 					OPT_NONE},
	{ (char *)"binningy",					(CMDFUNC)cmdAoiBinningY,				(CMDFUNC)cmdAoiBinningYHelp, 					OPT_NONE},
	{ (char *)"binningmode",				(CMDFUNC)cmdAoiBinningMode,				(CMDFUNC)cmdAoiBinningModeHelp, 				OPT_NONE},


//============================================================================================================================================
// Test Pattern
//============================================================================================================================================
	{ (char *)"tpmode",						(CMDFUNC)cmdAoiTpMode,					(CMDFUNC)cmdAoiTpModeHelp, 						OPT_NONE},
	{ (char *)"tpinc",			 			(CMDFUNC)cmdAoiTpInc,					(CMDFUNC)cmdAoiTpIncHelp, 						OPT_NONE},
	{ (char *)"tpposition",					(CMDFUNC)cmdAoiTpPosition,				(CMDFUNC)cmdAoiTpPositionHelp, 					OPT_NONE},

	{ (char *)"tpmodesensor",				(CMDFUNC)cmdSensorTpMode,				(CMDFUNC)cmdSensorTpModeHelp, 					OPT_NONE},
	{ (char *)"tpincsensor",				(CMDFUNC)cmdSensorTpInc,				(CMDFUNC)cmdSensorTpIncHelp, 					OPT_NONE},
	{ (char *)"tpdatasensor",				(CMDFUNC)cmdSensorTpData,				(CMDFUNC)cmdSensorTpDataHelp, 					OPT_NONE},


//============================================================================================================================================
// FFC
//============================================================================================================================================
	{ (char *)"ipu",	 	   			  	(CMDFUNC)cmdIpu,						(CMDFUNC)cmdIpuHelp, 							OPT_NONE},
	{ (char *)"ffcmode",	   				(CMDFUNC)cmdFfcMode,					(CMDFUNC)cmdFfcModeHelp, 						OPT_NONE},
	{ (char *)"ffccormode",					(CMDFUNC)cmdFfcCorMode,					(CMDFUNC)cmdFfcCorModeHelp, 					OPT_NONE},
	{ (char *)"ffcintmem",	   				(CMDFUNC)cmdFfcRegInitIntMem,			(CMDFUNC)cmdFfcRegInitIntMemHelp, 				OPT_NONE},

	{ (char *)"ffc",		   				(CMDFUNC)cmdFfcLoad,					(CMDFUNC)cmdFfcLoadHelp, 						OPT_NONE},
	{ (char *)"ffc-admin",	  				(CMDFUNC)cmdFfcLoadAdmin,				(CMDFUNC)cmdFfcLoadAdminHelp, 					OPT_NONE},

	{ (char *)"ffcsave",					(CMDFUNC)cmdFfcSave,					(CMDFUNC)cmdFfcSaveHelp, 						OPT_NONE},
	{ (char *)"ffcsave-admin",				(CMDFUNC)cmdFfcSaveAdmin,				(CMDFUNC)cmdFfcSaveAdminHelp, 					OPT_NONE},
	{ (char *)"ffcsaveall-admin",			(CMDFUNC)cmdFfcSaveAdminAll,			(CMDFUNC)cmdFfcSaveAdminAllHelp, 				OPT_NONE},

	{ (char *)"ffcadjustblackbatch",		(CMDFUNC)cmdFfcAdjustBatchBlack,		(CMDFUNC)cmdFfcAdjustBatchBlackHelp, 			OPT_NONE},
	{ (char *)"ffcadjustwhitebatch",		(CMDFUNC)cmdFfcAdjustBatchWhite,		(CMDFUNC)cmdFfcAdjustBatchWhiteHelp, 			OPT_NONE},

	{ (char *)"ffcadjustblackbatchall",		(CMDFUNC)cmdFfcAdjustBatchBlackAll,		(CMDFUNC)cmdFfcAdjustBatchBlackAllHelp, 		OPT_NONE},
	{ (char *)"ffcadjustwhitebatchall",		(CMDFUNC)cmdFfcAdjustBatchWhiteAll,		(CMDFUNC)cmdFfcAdjustBatchWhiteAllHelp, 		OPT_NONE},

	{ (char *)"ffcadjustinfo",				(CMDFUNC)cmdFfcAdjustInfo,				(CMDFUNC)cmdFfcAdjustInfoHelp, 					OPT_NONE},
	{ (char *)"ffcadjustinfo-admin",		(CMDFUNC)cmdFfcAdjustInfoAdmin,			(CMDFUNC)cmdFfcAdjustInfoAdminHelp, 			OPT_NONE},

	{ (char *)"ffcadjustrate",				(CMDFUNC)cmdFfcAdjustRate,				(CMDFUNC)cmdFfcAdjustRateHelp, 					OPT_NONE},
	{ (char *)"ffcadjustexp",				(CMDFUNC)cmdFfcAdjustExp,				(CMDFUNC)cmdFfcAdjustExpHelp, 					OPT_NONE},
	{ (char *)"ffcadjusttarget",			(CMDFUNC)cmdFfcAdjustTarget,			(CMDFUNC)cmdFfcAdjustTargetHelp, 				OPT_NONE},
	{ (char *)"ffcadjusttemp",				(CMDFUNC)cmdFfcAdjustTemp,				(CMDFUNC)cmdFfcAdjustTempHelp, 					OPT_NONE},
	{ (char *)"ffcadjustgainx",				(CMDFUNC)cmdFfcAdjustGainX,				(CMDFUNC)cmdFfcAdjustGainXHelp, 				OPT_NONE},
	{ (char *)"ffcadjustbit",				(CMDFUNC)cmdFfcAdjustBit,				(CMDFUNC)cmdFfcAdjustBitHelp, 					OPT_NONE},

	{ (char *)"ffcoffset-data",			 	(CMDFUNC)cmdFfcOffsetData,				(CMDFUNC)cmdFfcOffsetDataHelp, 					OPT_NONE},
	{ (char *)"ffcgain-data",			 	(CMDFUNC)cmdFfcGainData,				(CMDFUNC)cmdFfcGainDataHelp, 					OPT_NONE},
	{ (char *)"ffcgain-x",				 	(CMDFUNC)cmdFfcGainX,					(CMDFUNC)cmdFfcGainXHelp, 						OPT_NONE},

	{ (char *)"ffcoffsetave",			 	(CMDFUNC)cmdFfcOffsetAve,				(CMDFUNC)cmdFfcOffsetAveHelp,				 	OPT_NONE},
	{ (char *)"ffcgainave",				 	(CMDFUNC)cmdFfcGainAve,					(CMDFUNC)cmdFfcGainAveHelp, 					OPT_NONE},

	{ (char *)"ffcdefault", 				(CMDFUNC)cmdFfcDefault,					(CMDFUNC)cmdFfcDefaultHelp, 					OPT_NONE},

	{ (char *)"blacktarget",				(CMDFUNC)cmdFfcBlackTarget,				(CMDFUNC)cmdFfcBlackTargetHelp, 				OPT_NONE},
	{ (char *)"whitetarget",				(CMDFUNC)cmdFfcWhiteTarget,				(CMDFUNC)cmdFfcWhiteTargetHelp, 				OPT_NONE},


//============================================================================================================================================
// DPC
//============================================================================================================================================
#if defined (MODE_DPC)
	{ (char *)"dpc",	 		   			(CMDFUNC)cmdDpc,						(CMDFUNC)cmdDpcHelp, 							OPT_NONE},
	{ (char *)"dpcadjustmode",				(CMDFUNC)cmdDpcAdjustMode,				(CMDFUNC)cmdDpcAdjustModeHelp, 					OPT_NONE},

	{ (char *)"dpcload", 				 	(CMDFUNC)cmdDpcLoad,					(CMDFUNC)cmdDpcLoadHelp, 						OPT_NONE},
	{ (char *)"dpcload-admin", 				(CMDFUNC)cmdDpcLoadAdmin,				(CMDFUNC)cmdDpcLoadAdminHelp, 					OPT_NONE},

	{ (char *)"dpcsave", 				 	(CMDFUNC)cmdDpcSave,					(CMDFUNC)cmdDpcSaveHelp, 						OPT_NONE},
	{ (char *)"dpcsave-admin", 				(CMDFUNC)cmdDpcSaveAdmin,				(CMDFUNC)cmdDpcSaveAdminHelp, 					OPT_NONE},

	{ (char *)"dpcadjustbatch1",	 	  	(CMDFUNC)cmdDpcAdjustBatch1,			(CMDFUNC)cmdDpcAdjustBatch1Help, 				OPT_NONE},
	{ (char *)"dpcadjustbatch2",	  	 	(CMDFUNC)cmdDpcAdjustBatch2,			(CMDFUNC)cmdDpcAdjustBatch2Help, 				OPT_NONE},
	{ (char *)"dpcadjustbatch3",		    (CMDFUNC)cmdDpcAdjustBatch3,			(CMDFUNC)cmdDpcAdjustBatch3Help, 				OPT_NONE},

	{ (char *)"dpcadjustbatch2detail",		(CMDFUNC)cmdDpcAdjustBatch2Detail,		(CMDFUNC)cmdDpcAdjustBatch2DetailHelp, 			OPT_NONE},

	{ (char *)"dpcabort",					(CMDFUNC)cmdDpcAbort,					(CMDFUNC)cmdDpcAbort, 							OPT_NONE},
	{ (char *)"dpcdec",						(CMDFUNC)cmdDpcCounterDecrement,		(CMDFUNC)cmdDpcCounterDecrementHelp, 			OPT_NONE},
	{ (char *)"dpcstatecount",				(CMDFUNC)cmdDpcStateCount,				(CMDFUNC)cmdDpcStateCountHelp, 					OPT_NONE},

	{ (char *)"dpcadjustinfo",		 		(CMDFUNC)cmdDpcAdjustInfo,				(CMDFUNC)cmdDpcAdjustInfoHelp, 					OPT_NONE},
	{ (char *)"dpcadjustinfo-admin",		(CMDFUNC)cmdDpcAdjustInfoAdmin,			(CMDFUNC)cmdDpcAdjustInfoAdminHelp, 			OPT_NONE},

	{ (char *)"dpcadjustrate",				(CMDFUNC)cmdDpcAdjustRate,				(CMDFUNC)cmdDpcAdjustRateHelp, 					OPT_NONE},
	{ (char *)"dpcadjustexp",				(CMDFUNC)cmdDpcAdjustExp,				(CMDFUNC)cmdDpcAdjustExpHelp, 					OPT_NONE},
	{ (char *)"dpcadjustffcnumber",			(CMDFUNC)cmdDpcAdjustFfcNumber,			(CMDFUNC)cmdDpcAdjustFfcNumberHelp, 			OPT_NONE},
	{ (char *)"dpcadjusttemp",				(CMDFUNC)cmdDpcAdjustTemp,				(CMDFUNC)cmdDpcAdjustTempHelp, 					OPT_NONE},
	{ (char *)"dpcadjustsd",				(CMDFUNC)cmdDpcAdjustSd,				(CMDFUNC)cmdDpcAdjustSdHelp, 					OPT_NONE},
	{ (char *)"dpcadjustnonuniform",		(CMDFUNC)cmdDpcAdjustNonUniform,		(CMDFUNC)cmdDpcAdjustNonUniformHelp, 			OPT_NONE},

	{ (char *)"dpcmapinfo",			 		(CMDFUNC)cmdDpcMapInfo,					(CMDFUNC)cmdDpcMapInfoHelp, 					OPT_NONE},
	{ (char *)"dpcmapinfo-admin",			(CMDFUNC)cmdDpcMapInfoAdmin,			(CMDFUNC)cmdDpcMapInfoAdminHelp, 				OPT_NONE},
	{ (char *)"dpcmapinfoimp",			 	(CMDFUNC)cmdDpcMapInfoImpossible,		(CMDFUNC)cmdDpcMapInfoImpossibleHelp, 			OPT_NONE},
	{ (char *)"dpcmapinfoimp-admin",		(CMDFUNC)cmdDpcMapInfoImpossibleAdmin,	(CMDFUNC)cmdDpcMapInfoImpossibleAdminHelp, 		OPT_NONE},

	{ (char *)"dpcpixel-set",				(CMDFUNC)cmdDpcSetPixelNum,				(CMDFUNC)cmdDpcSetPixelNumHelp, 				OPT_NONE},
	{ (char *)"dpcpixel",					(CMDFUNC)cmdDpcPixelNum,				(CMDFUNC)cmdDpcPixelNumHelp, 					OPT_NONE},
	{ (char *)"dpcpixel-admin",				(CMDFUNC)cmdDpcPixelNumAdmin,			(CMDFUNC)cmdDpcPixelNumAdminHelp, 				OPT_NONE},
	{ (char *)"dpcmaxpixel",				(CMDFUNC)cmdDpcPixelMaxNum,				(CMDFUNC)cmdDpcPixelMaxNumHelp, 				OPT_NONE},
	{ (char *)"dpcpixelall",				(CMDFUNC)cmdDpcPixelNumAll,				(CMDFUNC)cmdDpcPixelNumAllHelp, 				OPT_NONE},

	{ (char *)"dpcdefault",					(CMDFUNC)cmdDpcDefault,					(CMDFUNC)cmdDpcDefaultHelp, 					OPT_NONE},

	{ (char *)"dpcgrid-add",				(CMDFUNC)cmdDpcAddGrid,					(CMDFUNC)cmdDpcAddGridHelp, 					OPT_NONE},
	{ (char *)"dpcgrid-delete",				(CMDFUNC)cmdDpcDeleteGrid,				(CMDFUNC)cmdDpcDeleteGridHelp, 					OPT_NONE},
	{ (char *)"dpcgrid-clear",				(CMDFUNC)cmdDpcClearGrid,				(CMDFUNC)cmdDpcClearGridHelp, 					OPT_NONE},
	{ (char *)"dpcgrid-get",				(CMDFUNC)cmdDpcGetGrid,					(CMDFUNC)cmdDpcGetGridHelp, 					OPT_NONE},


	{ (char *)"dpcgrid-check",				(CMDFUNC)cmdDpcGridCheck,				(CMDFUNC)cmdDpcGridCheckHelp, 					OPT_NONE},
	{ (char *)"dpcgrid-line-add",			(CMDFUNC)cmdDpcAddGridLine,				(CMDFUNC)cmdDpcAddGridLineHelp, 				OPT_NONE},
	{ (char *)"dpcgrid-line-delete",		(CMDFUNC)cmdDpcDeleteGridLine,			(CMDFUNC)cmdDpcDeleteGridLineHelp, 				OPT_NONE},
#endif // #if defined (MODE_DPC)


//============================================================================================================================================
// LUT
//============================================================================================================================================
#if defined (MODE_LUT)
	{ (char *)"lut-mode",					(CMDFUNC)cmdLutMode,					(CMDFUNC)cmdLutModeHelp, 						OPT_NONE},
	{ (char *)"lut-format",					(CMDFUNC)cmdLutFormat,					(CMDFUNC)cmdLutFormatHelp, 						OPT_NONE},
	{ (char *)"binarization",				(CMDFUNC)cmdLutBinThreshold,			(CMDFUNC)cmdLutBinThresholdHelp, 				OPT_NONE},
	{ (char *)"lut-bin",					(CMDFUNC)cmdLutBinThreshold,			(CMDFUNC)cmdLutBinThresholdHelp, 				OPT_NONE},
	{ (char *)"lut-gamma",					(CMDFUNC)cmdLutGamma,					(CMDFUNC)cmdLutGammaHelp, 						OPT_NONE},
	{ (char *)"lut",						(CMDFUNC)cmdLutTableData,				(CMDFUNC)cmdLutTableDataHelp, 					OPT_NONE},
	{ (char *)"lut-save",					(CMDFUNC)cmdLutSave,					(CMDFUNC)cmdLutSaveHelp, 						OPT_NONE},
	{ (char *)"lut-default",				(CMDFUNC)cmdLutDefault,					(CMDFUNC)cmdLutDefaultHelp, 					OPT_NONE},
#endif // #if defined (MODE_LUT)


//============================================================================================================================================
// Digital Offset Gain
//============================================================================================================================================
#if defined (MODE_DIGITAL_OFFSET_GAIN)
	{ (char *)"gain",						(CMDFUNC)cmdDigitalGain,				(CMDFUNC)cmdDigitalGainHelp, 					OPT_NONE},
	{ (char *)"gainx",						(CMDFUNC)cmdDigitalGainX,				(CMDFUNC)cmdDigitalGainXHelp, 					OPT_NONE},
	{ (char *)"blacklevel",					(CMDFUNC)cmdDigitalOffset,				(CMDFUNC)cmdDigitalOffsetHelp, 					OPT_NONE},
	{ (char *)"blacklevel1",				(CMDFUNC)cmdDigitalOffset1,				(CMDFUNC)cmdDigitalOffset1Help, 				OPT_NONE},
#endif


//============================================================================================================================================
// Sensor
//============================================================================================================================================
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	{ (char *)"sensorgain",					(CMDFUNC)cmdSensorGainDB,				(CMDFUNC)cmdSensorGainDBHelp, 					OPT_NONE},
	{ (char *)"sensorgainx",				(CMDFUNC)cmdSensorGainX,				(CMDFUNC)cmdSensorGainXHelp, 					OPT_NONE},
	
	#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
	{ (char *)"sensorcg",					(CMDFUNC)cmdSensorConversionGain,		(CMDFUNC)cmdSensorConversionGainHelp, 			OPT_NONE},
	#endif

	#if defined (MODE_SENSOR_DRRS)
	{ (char *)"drrs",						(CMDFUNC)cmdSensorDrrsMode,				(CMDFUNC)cmdSensorDrrsModeHelp, 				OPT_NONE},
	#endif

	#if defined (MODE_SENSOR_GRADATION_COMPRESS)
	{ (char *)"ssm",						(CMDFUNC)cmdSensor8BitConvert,			(CMDFUNC)cmdSensor8BitConvertHelp, 				OPT_NONE},
	{ (char *)"sensor-sensitive-mode",		(CMDFUNC)cmdSensor8BitConvert,			(CMDFUNC)cmdSensor8BitConvertHelp, 				OPT_NONE},
	#endif // #if defined (MODE_SENSOR_GRADATION_COMPRESS)
#endif


//============================================================================================================================================
// 貂ｩ蠎ｦ
//============================================================================================================================================
	{ (char *)"temp",					   	(CMDFUNC)cmdTemp,						(CMDFUNC)cmdTempHelp, 							OPT_NONE},
	{ (char *)"temperature",			   	(CMDFUNC)cmdTemp,						(CMDFUNC)cmdTempHelp, 							OPT_NONE},

	{ (char *)"sensor",						(CMDFUNC)cmdSensorTempAlrm,				(CMDFUNC)cmdSensorTempAlrmHelp, 				OPT_NONE},
	{ (char *)"case",						(CMDFUNC)cmdCaseTempAlrm,				(CMDFUNC)cmdCaseTempAlrmHelp, 					OPT_NONE},
	{ (char *)"target",					   	(CMDFUNC)cmdPeltierTarget,				(CMDFUNC)cmdPeltierTargetHelp, 					OPT_NONE},

	{ (char *)"targetmax",					(CMDFUNC)cmdPeltierTempMax,				(CMDFUNC)cmdPeltierTempMaxHelp, 				OPT_NONE},
	{ (char *)"targetmin",					(CMDFUNC)cmdPeltierTempMin,				(CMDFUNC)cmdPeltierTempMinHelp, 				OPT_NONE},
	{ (char *)"peltierenable",			   	(CMDFUNC)cmdPeltierEnable,				(CMDFUNC)cmdPeltierEnableHelp, 					OPT_NONE},

	{ (char *)"tempalarmstatus",		   	(CMDFUNC)cmdTempAlarmStatus,			(CMDFUNC)cmdTempAlarmStatusHelp, 				OPT_NONE},
	{ (char *)"tempalarmclear",			   	(CMDFUNC)cmdTempAlarmClear,				(CMDFUNC)cmdTempAlarmClearHelp, 				OPT_NONE},

	{ (char *)"peltiervolt",		 		(CMDFUNC)cmdPeltierVolt,				(CMDFUNC)cmdPeltierVoltHelp, 					OPT_NONE},
	{ (char *)"peltiercurrent",		  		(CMDFUNC)cmdPeltierCurrent,				(CMDFUNC)cmdPeltierCurrentHelp, 				OPT_NONE},

#if defined (MODE_TEMP_ABNORMAL_CHECK)
	{ (char *)"tempabnormalstatus",		  	(CMDFUNC)cmdTempAbnormalStatus,			(CMDFUNC)cmdTempAbnormalStatusHelp, 			OPT_NONE},
	{ (char *)"tempabnormalcount",		  	(CMDFUNC)cmdTempAbnormalCount,			(CMDFUNC)cmdTempAbnormalCountHelp, 				OPT_NONE},
#endif
	
#if defined (MODE_PELTIER_CTRL)
	{ (char *)"peltierpowerlevel",		  	(CMDFUNC)cmdPeltierPowerLevel,			(CMDFUNC)cmdPeltierPowerLevelHelp, 				OPT_NONE},
#endif


//============================================================================================================================================
// UserSet
//============================================================================================================================================
	{ (char *)"user",				   		(CMDFUNC)cmdCameraUser,					(CMDFUNC)cmdCameraUserHelp, 					OPT_NONE},
	{ (char *)"userset-default",			(CMDFUNC)cmdCameraUserBoot,				(CMDFUNC)cmdCameraUserBootHelp, 				OPT_NONE},
	{ (char *)"boot",		   				(CMDFUNC)cmdCameraUserBoot,				(CMDFUNC)cmdCameraUserBootHelp, 				OPT_NONE},
	{ (char *)"userset-save",	   			(CMDFUNC)cmdCameraUserSave,				(CMDFUNC)cmdCameraUserSaveHelp, 				OPT_NONE},
	{ (char *)"save",			   			(CMDFUNC)cmdCameraUserSave,				(CMDFUNC)cmdCameraUserSaveHelp, 				OPT_NONE},
	{ (char *)"userset-load",				(CMDFUNC)cmdCameraUserLoad,				(CMDFUNC)cmdCameraUserLoadHelp, 				OPT_NONE},
	{ (char *)"load",						(CMDFUNC)cmdCameraUserLoad,				(CMDFUNC)cmdCameraUserLoadHelp, 				OPT_NONE},
	{ (char *)"factory-default",			(CMDFUNC)cmdCameraUserSetDefault,		(CMDFUNC)cmdCameraUserSetDefaultHelp, 			OPT_NONE},
	{ (char *)"factory",					(CMDFUNC)cmdCameraUserSetDefault,		(CMDFUNC)cmdCameraUserSetDefaultHelp, 			OPT_NONE},


//============================================================================================================================================
// Information
//============================================================================================================================================
	{ (char *)"info", 						(CMDFUNC)cmdBoardInfo,					(CMDFUNC)cmdBoardInfoHelp, 						OPT_NONE},
	{ (char *)"vendor",				 	  	(CMDFUNC)cmdGetVendor,					(CMDFUNC)cmdGetVendorHelp, 						OPT_NONE},
	{ (char *)"manufacture",		 	  	(CMDFUNC)cmdGetManufacture,				(CMDFUNC)cmdGetManufactureHelp, 				OPT_NONE},
	{ (char *)"model",		    			(CMDFUNC)cmdGetModel,					(CMDFUNC)cmdGetModelHelp, 						OPT_NONE},
	{ (char *)"firmware",	    			(CMDFUNC)cmdFirmware,					(CMDFUNC)cmdFirmwareHelp, 						OPT_NONE},
	{ (char *)"boardversion",	  			(CMDFUNC)cmdBoardVersion,				(CMDFUNC)cmdBoardVersionHelp,				 	OPT_NONE},
	{ (char *)"fpgaversion",	  			(CMDFUNC)cmdFpgaVersion,				(CMDFUNC)cmdFpgaVersionHelp, 					OPT_NONE},
	{ (char *)"firmversion",	  			(CMDFUNC)cmdFirmVersion,				(CMDFUNC)cmdFirmVersionHelp, 					OPT_NONE},
	{ (char *)"version",	 	 			(CMDFUNC)cmdMainVersion,				(CMDFUNC)cmdMainVersionHelp, 					OPT_NONE},
	{ (char *)"bootversion",	 	 		(CMDFUNC)cmdBootVersion,				(CMDFUNC)cmdBootVersionHelp, 					OPT_NONE},

	{ (char *)"firmbuild",	  				(CMDFUNC)cmdFirmBuildInfo,				(CMDFUNC)cmdFirmBuildInfoHelp, 					OPT_NONE},
	{ (char *)"fpgabuild",	  				(CMDFUNC)cmdFpgaBuildInfo,				(CMDFUNC)cmdFpgaBuildInfoHelp, 					OPT_NONE},

	{ (char *)"boardid",				   	(CMDFUNC)cmdGetBoardId,					(CMDFUNC)cmdGetBoardIdHelp, 					OPT_NONE},
	{ (char *)"id",						   	(CMDFUNC)cmdGetBoardId,					(CMDFUNC)cmdGetBoardIdHelp, 					OPT_NONE},
	{ (char *)"sensorid",				   	(CMDFUNC)cmdGetSensorId,				(CMDFUNC)cmdGetSensorIdHelp, 					OPT_NONE},


	{ (char *)"vendor-set",			 	  	(CMDFUNC)cmdSetVendor,					(CMDFUNC)cmdSetVendorHelp, 						OPT_NONE},
	{ (char *)"manufacture-set",	 	  	(CMDFUNC)cmdSetManufacture,				(CMDFUNC)cmdSetManufactureHelp, 				OPT_NONE},
	{ (char *)"model-set",		   			(CMDFUNC)cmdSetModel,					(CMDFUNC)cmdSetModelHelp, 						OPT_NONE},
	{ (char *)"boardid-set",			   	(CMDFUNC)cmdSetBoardId,					(CMDFUNC)cmdSetBoardIdHelp, 					OPT_NONE},
	{ (char *)"sensorid-set",			   	(CMDFUNC)cmdSetSensorId,				(CMDFUNC)cmdSetSensorIdHelp, 					OPT_NONE},
	{ (char *)"boardversion-set",		   	(CMDFUNC)cmdSetBoardVersion,			(CMDFUNC)cmdSetBoardVersionHelp, 				OPT_NONE},

	{ (char *)"vendor-delete",			  	(CMDFUNC)cmdDeleteVendor,				(CMDFUNC)cmdDeleteVendorHelp, 					OPT_NONE},
	{ (char *)"manufacture-delete",	 	  	(CMDFUNC)cmdDeleteManufacture,			(CMDFUNC)cmdDeleteManufactureHelp, 				OPT_NONE},
	{ (char *)"model-delete",		   		(CMDFUNC)cmdDeleteModel,				(CMDFUNC)cmdDeleteModelHelp, 					OPT_NONE},

	{ (char *)"vendor-default",			  	(CMDFUNC)cmdDefaultVendor,				(CMDFUNC)cmdDefaultVendorHelp, 					OPT_NONE},
	{ (char *)"manufacture-default",	  	(CMDFUNC)cmdDefaultManufacture,			(CMDFUNC)cmdDefaultManufactureHelp, 			OPT_NONE},
	{ (char *)"model-default",		   		(CMDFUNC)cmdDefaultModel,				(CMDFUNC)cmdDefaultModelHelp, 					OPT_NONE},

	{ (char *)"userid",						(CMDFUNC)cmdCameraUserID,				(CMDFUNC)cmdCameraUserIDHelp, 					OPT_NONE},
	{ (char *)"userid-delete",				(CMDFUNC)cmdCameraUserIDDelete,			(CMDFUNC)cmdCameraUserIDDeleteHelp, 			OPT_NONE},


//============================================================================================================================================
// Camera Command Information
//============================================================================================================================================
	{ (char *)"gcp",			    		(CMDFUNC)cmdGcp,						(CMDFUNC)cmdGcpHelp, 							OPT_NONE},
	{ (char *)"camera",			    		(CMDFUNC)cmdAcquisitionGcp,				(CMDFUNC)cmdAcquisitionGcpHelp, 				OPT_NONE},
	{ (char *)"tempstatus",					(CMDFUNC)cmdTempStatus,					(CMDFUNC)cmdTempStatusHelp, 					OPT_NONE},
	{ (char *)"camerastatus",				(CMDFUNC)cmdCameraStatus,				(CMDFUNC)cmdCameraStatusHelp, 					OPT_NONE},
	{ (char *)"acquisitioncontrol-status", 	(CMDFUNC)cmdAcquisitionStatusAll,		(CMDFUNC)cmdAcquisitionStatusAllHelp, 			OPT_NONE},
	{ (char *)"digitalcontrol-status",	 	(CMDFUNC)cmdDigitaiIoStatusAll,			(CMDFUNC)cmdDigitaiIoStatusAllHelp, 			OPT_NONE},
	{ (char *)"countercontrol-status",		(CMDFUNC)cmdCounterStatusAll,			(CMDFUNC)cmdCounterStatusAllHelp, 				OPT_NONE},
	{ (char *)"timercontrol-status",		(CMDFUNC)cmdTimerStatusAll,				(CMDFUNC)cmdTimerStatusAllHelp, 				OPT_NONE},
#if defined(MODE_ENCODER)
	{ (char *)"enccontrol-status",			(CMDFUNC)cmdEncoderStatusAll,			(CMDFUNC)cmdEncoderStatusAllHelp, 				OPT_NONE},
#endif // #if defined(MODE_ENCODER)
	{ (char *)"regmap",						(CMDFUNC)cmdRegMap,						(CMDFUNC)cmdRegMapHelp, 						OPT_NONE},


//============================================================================================================================================
// Default
//============================================================================================================================================
	{ (char *)"defaultall",		    		(CMDFUNC)cmdDefaultAll,					(CMDFUNC)cmdDefaultAllHelp, 					OPT_NONE},


//============================================================================================================================================
// Board
//============================================================================================================================================
	// GPIO
	{ (char *)"gpi",	  	 			   	(CMDFUNC)cmdGpi,						(CMDFUNC)cmdGpiHelp, 							OPT_NONE},
	{ (char *)"gpo",	   				   	(CMDFUNC)cmdGpo,						(CMDFUNC)cmdGpoHelp, 							OPT_NONE},
	{ (char *)"dnf",	   				   	(CMDFUNC)cmdGpioDnf,					(CMDFUNC)cmdGpioDnfHelp, 						OPT_NONE},

	{ (char *)"line-pulse",	 			   	(CMDFUNC)cmdGpioPulseTime,				(CMDFUNC)cmdGpioPulseTimeHelp, 					OPT_NONE},

	{ (char *)"dipsw",	   				   	(CMDFUNC)cmdDipsw,						(CMDFUNC)cmdDipswHelp, 							OPT_NONE},

	{ (char *)"reset",	   				   	(CMDFUNC)cmdReset,						(CMDFUNC)cmdResetHelp, 							OPT_NONE},

	//@@@1{ (char *)"baudrate",			   		(CMDFUNC)cmdBaudRate,					(CMDFUNC)cmdBaudRateHelp, 						OPT_NONE},

	{ (char *)"led",						(CMDFUNC)cmdLed,						(CMDFUNC)cmdLedHelp, 							OPT_NONE},
	{ (char *)"leddebugmode",				(CMDFUNC)cmdLedDebugMode,				(CMDFUNC)cmdLedDebugModeHelp, 					OPT_NONE},


//============================================================================================================================================
// Voltage FPGA Board
//============================================================================================================================================
#if defined (MODE_VOLTAGE_FPGA_BOARD)
	{ (char *)"voltfpgashow",				(CMDFUNC)cmdVoltFpgaShow,				(CMDFUNC)cmdVoltFpgaShowHelp, 					OPT_NONE},
	{ (char *)"voltfpga09",					(CMDFUNC)cmdVoltFpga09v,				(CMDFUNC)cmdVoltFpga09vHelp, 					OPT_NONE},
	{ (char *)"voltfpga12sys",				(CMDFUNC)cmdVoltFpga12vSys,				(CMDFUNC)cmdVoltFpga12vSysHelp, 				OPT_NONE},
	{ (char *)"voltfpga25",					(CMDFUNC)cmdVoltFpga25v,				(CMDFUNC)cmdVoltFpga25vHelp, 					OPT_NONE},
	{ (char *)"voltfpga115",				(CMDFUNC)cmdVoltFpga115v,				(CMDFUNC)cmdVoltFpga115vHelp, 					OPT_NONE},
	{ (char *)"voltfpga33vd",				(CMDFUNC)cmdVoltFpga33vd,				(CMDFUNC)cmdVoltFpga33vdHelp, 					OPT_NONE},
	{ (char *)"voltfpgavtt",				(CMDFUNC)cmdVoltFpgaVtt,				(CMDFUNC)cmdVoltFpgaVttHelp, 					OPT_NONE},
	{ (char *)"voltfpga12",					(CMDFUNC)cmdVoltFpga12v,				(CMDFUNC)cmdVoltFpga12vHelp, 					OPT_NONE},
	{ (char *)"voltfpga33",					(CMDFUNC)cmdVoltFpga33v,				(CMDFUNC)cmdVoltFpga33vHelp, 					OPT_NONE},
	{ (char *)"voltfpga33va",				(CMDFUNC)cmdVoltFpga33va,				(CMDFUNC)cmdVoltFpga33vaHelp, 					OPT_NONE},
	{ (char *)"voltfpga50",					(CMDFUNC)cmdVoltFpga50v,				(CMDFUNC)cmdVoltFpga50vHelp, 					OPT_NONE},
	{ (char *)"voltfpgavref",				(CMDFUNC)cmdVoltFpgaVRef,				(CMDFUNC)cmdVoltFpgaVRefHelp, 					OPT_NONE},
	{ (char *)"voltfpgavccint",				(CMDFUNC)cmdVoltFpgaVccint,				(CMDFUNC)cmdVoltFpgaVccintHelp, 				OPT_NONE},
	{ (char *)"voltfpgavccaux",				(CMDFUNC)cmdVoltFpgaVccaux,				(CMDFUNC)cmdVoltFpgaVccauxHelp, 				OPT_NONE},
	{ (char *)"voltfpgavbram",				(CMDFUNC)cmdVoltFpgaVbram,				(CMDFUNC)cmdVoltFpgaVbramHelp, 					OPT_NONE},
	{ (char *)"voltfpgavccpsintlp",			(CMDFUNC)cmdVoltFpgaVccPsintpl,			(CMDFUNC)cmdVoltFpgaVccPsintplHelp, 			OPT_NONE},
	{ (char *)"voltfpgavccpsinfp",			(CMDFUNC)cmdVoltFpgaVccPsinfp,			(CMDFUNC)cmdVoltFpgaVccPsinfpHelp, 				OPT_NONE},
	{ (char *)"voltfpgavccpsaux",			(CMDFUNC)cmdVoltFpgaVccPsaux,			(CMDFUNC)cmdVoltFpgaVccPsauxHelp, 				OPT_NONE},
#endif


//============================================================================================================================================
// Voltage IF Board
//============================================================================================================================================
#if defined (MODE_VOLTAGE_IF_BOARD)
	{ (char *)"voltifshow",					(CMDFUNC)cmdVoltIfShow,					(CMDFUNC)cmdVoltIfShowHelp, 					OPT_NONE},
	{ (char *)"voltif105vd",				(CMDFUNC)cmdVoltIf105vd,				(CMDFUNC)cmdVoltIf105vdHelp, 					OPT_NONE},
	{ (char *)"voltif105va",				(CMDFUNC)cmdVoltIf105va,				(CMDFUNC)cmdVoltIf105vaHelp, 					OPT_NONE},
	{ (char *)"voltif125vd",				(CMDFUNC)cmdVoltIf125vd,				(CMDFUNC)cmdVoltIf125vdHelp, 					OPT_NONE},
	{ (char *)"voltif18vd",					(CMDFUNC)cmdVoltIf18vd,					(CMDFUNC)cmdVoltIf18vdHelp, 					OPT_NONE},
	{ (char *)"voltif25vf",					(CMDFUNC)cmdVoltIf25vf,					(CMDFUNC)cmdVoltIf25vfHelp, 					OPT_NONE},
	{ (char *)"voltif33vd",					(CMDFUNC)cmdVoltIf33vd,					(CMDFUNC)cmdVoltIf33vdHelp, 					OPT_NONE},
	{ (char *)"voltif90va",					(CMDFUNC)cmdVoltIf90va,					(CMDFUNC)cmdVoltIf90vaHelp, 					OPT_NONE},
	{ (char *)"voltif065v",					(CMDFUNC)cmdVoltIf065v,					(CMDFUNC)cmdVoltIf065vHelp, 					OPT_NONE},
	{ (char *)"voltif085va",				(CMDFUNC)cmdVoltIf085va,				(CMDFUNC)cmdVoltIf085vaHelp, 					OPT_NONE},
	{ (char *)"voltif23va",					(CMDFUNC)cmdVoltIf230va,				(CMDFUNC)cmdVoltIf230vaHelp, 					OPT_NONE},
	{ (char *)"voltif33v",					(CMDFUNC)cmdVoltIf330v,					(CMDFUNC)cmdVoltIf330vHelp, 					OPT_NONE},
	{ (char *)"voltif12vp",					(CMDFUNC)cmdVoltIf120vp,				(CMDFUNC)cmdVoltIf120vpHelp, 					OPT_NONE},
	{ (char *)"voltif24v",					(CMDFUNC)cmdVoltIf240v,					(CMDFUNC)cmdVoltIf240vHelp, 					OPT_NONE},
	{ (char *)"voltiftemp",					(CMDFUNC)cmdVoltIfTemp,					(CMDFUNC)cmdVoltIfTempHelp, 					OPT_NONE},
#endif // #if defined (MODE_VOLTAGE_IF_BOARD)

	
//============================================================================================================================================
// Line
//============================================================================================================================================
	{ (char *)"line-selector",				(CMDFUNC)cmdDigitalIoLineSelect,		(CMDFUNC)cmdDigitalIoLineSelectHelp, 			OPT_NONE},
	{ (char *)"line-mode",					(CMDFUNC)cmdDigitalIoLineMode,			(CMDFUNC)cmdDigitalIoLineModeHelp, 				OPT_NONE},
	{ (char *)"line-inverter",				(CMDFUNC)cmdDigitalIoLineInverter,		(CMDFUNC)cmdDigitalIoLineInverterHelp, 			OPT_NONE},
	{ (char *)"line-status",				(CMDFUNC)cmdDigitalIoLineStatus,		(CMDFUNC)cmdDigitalIoLineStatusHelp, 			OPT_NONE},
	{ (char *)"line-status-all",			(CMDFUNC)cmdDigitalIoLineStatusAll,		(CMDFUNC)cmdDigitalIoLineStatusAllHelp,			OPT_NONE},
	{ (char *)"line-source",				(CMDFUNC)cmdDigitalIoLineSource,		(CMDFUNC)cmdDigitalIoLineSourceHelp, 			OPT_NONE},
	{ (char *)"line-format",				(CMDFUNC)cmdDigitalIoLineFormat,		(CMDFUNC)cmdDigitalIoLineFormatHelp, 			OPT_NONE},
	{ (char *)"user-selector",				(CMDFUNC)cmdDigitalIoUserSelector,		(CMDFUNC)cmdDigitalIoUserSelectorHelp, 			OPT_NONE},
	{ (char *)"user-value",					(CMDFUNC)cmdDigitalIoUserValue,			(CMDFUNC)cmdDigitalIoUserValueHelp, 			OPT_NONE},
	{ (char *)"user-value-all",				(CMDFUNC)cmdDigitalIoUserAllValue,		(CMDFUNC)cmdDigitalIoUserAllValueHelp, 			OPT_NONE},
	{ (char *)"user-mask-all",				(CMDFUNC)cmdDigitalIoUserAllMask,		(CMDFUNC)cmdDigitalIoUserAllMaskHelp, 			OPT_NONE},


//============================================================================================================================================
// Counter
//============================================================================================================================================
	{ (char *)"counter-selector",			(CMDFUNC)cmdCounterSelect,				(CMDFUNC)cmdCounterSelectHelp, 					OPT_NONE},
	{ (char *)"counter-event-source",		(CMDFUNC)cmdCounterEventSource,			(CMDFUNC)cmdCounterEventSourceHelp, 			OPT_NONE},
	{ (char *)"counter-event-activation",	(CMDFUNC)cmdCounterEventActivation,		(CMDFUNC)cmdCounterEventActivationHelp, 		OPT_NONE},
	{ (char *)"counter-reset-source",		(CMDFUNC)cmdCounterResetSource,			(CMDFUNC)cmdCounterResetSourceHelp, 			OPT_NONE},
	{ (char *)"counter-reset-activation",	(CMDFUNC)cmdCounterResetActivation,		(CMDFUNC)cmdCounterResetActivationHelp, 		OPT_NONE},
	{ (char *)"counter-reset",				(CMDFUNC)cmdCounterReset,				(CMDFUNC)cmdCounterResetHelp, 					OPT_NONE},
	{ (char *)"counter-value",				(CMDFUNC)cmdCounterValue,				(CMDFUNC)cmdCounterValueHelp, 					OPT_NONE},
	{ (char *)"counter-reset-value",		(CMDFUNC)cmdCounterResetValue,			(CMDFUNC)cmdCounterResetValueHelp, 				OPT_NONE},
	{ (char *)"counter-duration",			(CMDFUNC)cmdCounterDuratione,			(CMDFUNC)cmdCounterDurationeHelp, 				OPT_NONE},
	{ (char *)"counter-status",				(CMDFUNC)cmdCounterStatus,				(CMDFUNC)cmdCounterStatusHelp, 					OPT_NONE},
	{ (char *)"counter-trg-source",			(CMDFUNC)cmdCounterTrgSource,			(CMDFUNC)cmdCounterTrgSourceHelp, 				OPT_NONE},
	{ (char *)"counter-trg-activation",		(CMDFUNC)cmdCounterTrgActivation,		(CMDFUNC)cmdCounterTrgActivationHelp, 			OPT_NONE},


//============================================================================================================================================
// Timer
//============================================================================================================================================
	{ (char *)"timer-selector",				(CMDFUNC)cmdTimerSelect,				(CMDFUNC)cmdTimerSelectHelp, 					OPT_NONE},
	{ (char *)"timer-duration",				(CMDFUNC)cmdTimerDuration,				(CMDFUNC)cmdTimerDurationHelp, 					OPT_NONE},
	{ (char *)"timer-delay",				(CMDFUNC)cmdTimerDelay,					(CMDFUNC)cmdTimerDelayHelp, 					OPT_NONE},
	{ (char *)"timer-reset",				(CMDFUNC)cmdTimerReset,					(CMDFUNC)cmdTimerResetHelp, 					OPT_NONE},
	{ (char *)"timer-value",				(CMDFUNC)cmdTimerValue,					(CMDFUNC)cmdTimerValueHelp, 					OPT_NONE},
	{ (char *)"timer-status",				(CMDFUNC)cmdTimerStatus,				(CMDFUNC)cmdTimerStatusHelp, 					OPT_NONE},
	{ (char *)"timer-source",				(CMDFUNC)cmdTimerSource,				(CMDFUNC)cmdTimerSourceHelp, 					OPT_NONE},
	{ (char *)"timer-activation",			(CMDFUNC)cmdTimerActivation,			(CMDFUNC)cmdTimerActivationHelp, 				OPT_NONE},


//============================================================================================================================================
// Encoder
//============================================================================================================================================
#if defined(MODE_ENCODER)
	{ (char *)"enc-phasea-source",			(CMDFUNC)cmdEncoderPhaseATrgSource,		(CMDFUNC)cmdEncoderPhaseATrgSourceHelp, 		OPT_NONE},
	{ (char *)"enc-phaseb-source",			(CMDFUNC)cmdEncoderPhaseBTrgSource,		(CMDFUNC)cmdEncoderPhaseBTrgSourceHelp, 		OPT_NONE},
	{ (char *)"enc-mode",					(CMDFUNC)cmdEncoderMode,				(CMDFUNC)cmdEncoderModeHelp, 					OPT_NONE},
	{ (char *)"enc-divider",				(CMDFUNC)cmdEncoderDivider,				(CMDFUNC)cmdEncoderDividerHelp, 				OPT_NONE},
	{ (char *)"enc-outputmode",				(CMDFUNC)cmdEncoderOutputMode,			(CMDFUNC)cmdEncoderOutputModeHelp,			 	OPT_NONE},
	{ (char *)"enc-status",					(CMDFUNC)cmdEncoderStatus,				(CMDFUNC)cmdEncoderStatusHelp, 					OPT_NONE},
	{ (char *)"enc-timeout",				(CMDFUNC)cmdEncoderTimeout,				(CMDFUNC)cmdEncoderTimeoutHelp, 				OPT_NONE},
	{ (char *)"enc-reset-source",			(CMDFUNC)cmdEncoderResetTrgSource,		(CMDFUNC)cmdEncoderResetTrgSourceHelp, 			OPT_NONE},
	{ (char *)"enc-reset-activation",		(CMDFUNC)cmdEncoderResetActivation,		(CMDFUNC)cmdEncoderResetActivationHelp, 		OPT_NONE},
	{ (char *)"enc-reset",					(CMDFUNC)cmdEncoderReset,				(CMDFUNC)cmdEncoderResetHelp, 					OPT_NONE},
	{ (char *)"enc-value",					(CMDFUNC)cmdEncoderValue,				(CMDFUNC)cmdEncoderValueHelp, 					OPT_NONE},
	{ (char *)"enc-reset-value",			(CMDFUNC)cmdEncoderValueAtReset,		(CMDFUNC)cmdEncoderValueAtResetHelp, 			OPT_NONE},
	{ (char *)"enc-frequency",				(CMDFUNC)cmdEncoderFrequency,			(CMDFUNC)cmdEncoderFrequencyHelp, 				OPT_NONE},
#endif


//============================================================================================================================================
// Firmware Update
//============================================================================================================================================
	{ (char *)"update-all",					(CMDFUNC)cmdFirmUpdateAll,				(CMDFUNC)cmdFirmUpdateAllHelp, 					OPT_NONE},
	{ (char *)"update-all-admin",			(CMDFUNC)cmdFirmUpdateAllAdmin,			(CMDFUNC)cmdFirmUpdateAllAdminHelp, 			OPT_NONE},
	{ (char *)"download-all",				(CMDFUNC)cmdFirmDownloadAll,			(CMDFUNC)cmdFirmDownloadAllHelp, 				OPT_NONE},

#if defined (MODE_CXP)
	{ (char *)"update-xml",					(CMDFUNC)cmdFirmXmlUpdate,				(CMDFUNC)cmdFirmXmlUpdateHelp, 					OPT_NONE},
	{ (char *)"download-xml",				(CMDFUNC)cmdFirmXmlDownload,			(CMDFUNC)cmdFirmXmlDownloadHelp, 				OPT_NONE},
#endif

	{ (char *)"bootheader",					(CMDFUNC)cmdBootHeader,					(CMDFUNC)cmdBootHeaderHelp, 					OPT_NONE},


//============================================================================================================================================
// FPGA Update
//============================================================================================================================================
#if defined (MODE_UPDATE_INTERFACE)
	//@@@1{ (char *)"update-if",					(CMDFUNC)cmdFpgaUpdateInterface,		(CMDFUNC)cmdFpgaUpdateInterfaceHelp, 			OPT_NONE},
	//@@@1{ (char *)"update-if-admin",			(CMDFUNC)cmdFpgaUpdateInterfaceAdmin,	(CMDFUNC)cmdFpgaUpdateInterfaceHelpAdmin, 		OPT_NONE},
	//@@@1{ (char *)"download-if",				(CMDFUNC)cmdFpgaDownloadInterface,		(CMDFUNC)cmdFpgaDownloadInterfaceHelp, 			OPT_NONE},
	//@@@1{ (char *)"iffpgacopy",					(CMDFUNC)cmdFpgaCopyInterface,			(CMDFUNC)cmdFpgaCopyInterfaceHelp, 				OPT_NONE},
#endif // #if defined (MODE_UPDATE_INTERFACE)

	
//============================================================================================================================================
// 10G Ethernet
//============================================================================================================================================
#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
	{ (char *)"update-phy",					(CMDFUNC)cmdPhyUpdate,					(CMDFUNC)cmdPhyUpdateHelp, 						OPT_NONE},
	{ (char *)"phydatacopy",				(CMDFUNC)cmdPhyDataCopy,				(CMDFUNC)cmdPhyDataCopyHelp, 					OPT_NONE},
	{ (char *)"phyfirmversion",				(CMDFUNC)cmdPhyFirmVersion,				(CMDFUNC)cmdPhyFirmVersionHelp, 				OPT_NONE},
	{ (char *)"phyapiversion",				(CMDFUNC)cmdPhyApiVersion,				(CMDFUNC)cmdPhyApiVersionHelp, 					OPT_NONE},
	{ (char *)"phytemp",					(CMDFUNC)cmdPhyTemp,					(CMDFUNC)cmdPhyTempHelp, 						OPT_NONE},

	{ (char *)"phycount", 					(CMDFUNC)cmdPhyCount,					(CMDFUNC)cmdPhyCountHelp, 						OPT_NONE},
	{ (char *)"physpeed", 					(CMDFUNC)cmdPhySpeed,					(CMDFUNC)cmdPhySpeedHelp, 						OPT_NONE},

	{ (char *)"dumpphy", 					(CMDFUNC)cmdPhyDump,					(CMDFUNC)cmdPhyDumpHelp, 						OPT_NONE},
	
	{ (char *)"packetdelaycalc", 			(CMDFUNC)cmdGigEPacketDelayCalc,		(CMDFUNC)cmdGigEPacketDelayCalcHelp, 			OPT_NONE},

	#if defined (MODE_GE_SPEED)
	{ (char *)"gevspeedconfig", 			(CMDFUNC)cmdGevSpeedConfig,				(CMDFUNC)cmdGevSpeedConfigHelp, 				OPT_NONE},
	#endif // #if defined (MODE_GE_SPEED)
#endif // #if defined (MODE_GIGE_10G)


//============================================================================================================================================
// Memory Access
//============================================================================================================================================
	{ (char *)"io8", 						(CMDFUNC)cmdIo8,						(CMDFUNC)cmdIoXHelp, 							OPT_NONE},
	{ (char *)"io16", 						(CMDFUNC)cmdIo16,						(CMDFUNC)cmdIoXHelp, 							OPT_NONE},
	{ (char *)"io32", 						(CMDFUNC)cmdIo32,						(CMDFUNC)cmdIoXHelp,						 	OPT_NONE},
	{ (char *)"iof", 						(CMDFUNC)cmdIof,						(CMDFUNC)cmdIoXHelp, 							OPT_NONE},


//============================================================================================================================================
// Auto Exposure / Gain Command
//============================================================================================================================================
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
	{ (char *)"exposureauto",				(CMDFUNC)cmdAutoBrightExposureMode,		(CMDFUNC)cmdAutoBrightExposureModeHelp, 		OPT_NONE},
	{ (char *)"exposureautostatus",			(CMDFUNC)cmdAutoBrightExposureStatus,	(CMDFUNC)cmdAutoBrightExposureStatusHelp, 		OPT_NONE},
	{ (char *)"exposureautomax",			(CMDFUNC)cmdAutoBrightExposureMax,		(CMDFUNC)cmdAutoBrightExposureMaxHelp, 			OPT_NONE},

	{ (char *)"gainauto",					(CMDFUNC)cmdAutoBrightGainMode,			(CMDFUNC)cmdAutoBrightGainModeHelp, 			OPT_NONE},
	{ (char *)"gainautostatus",				(CMDFUNC)cmdAutoBrightGainStatus,		(CMDFUNC)cmdAutoBrightGainStatusHelp, 			OPT_NONE},
	{ (char *)"gainautomax",				(CMDFUNC)cmdAutoBrightGainMax,			(CMDFUNC)cmdAutoBrightGainMaxHelp, 				OPT_NONE},

	{ (char *)"autobrightoverlay",			(CMDFUNC)cmdAutoBrightDetectArea,		(CMDFUNC)cmdAutoBrightDetectAreaHelp, 			OPT_NONE},
	{ (char *)"autobrighttarget",			(CMDFUNC)cmdAutoBrightTarget,			(CMDFUNC)cmdAutoBrightTargetHelp, 				OPT_NONE},
	{ (char *)"autobrightaverage",		    (CMDFUNC)cmdAutoBrightAverage,			(CMDFUNC)cmdAutoBrightAverageHelp, 				OPT_NONE},

	{ (char *)"autobrightwidth",			(CMDFUNC)cmdAutoBrightWidthSize,		(CMDFUNC)cmdAutoBrightWidthSizeHelp, 			OPT_NONE},
	{ (char *)"autobrightheight",			(CMDFUNC)cmdAutoBrightHeightSize,		(CMDFUNC)cmdAutoBrightHeightSizeHelp, 			OPT_NONE},
	{ (char *)"autobrightoffsetx",		    (CMDFUNC)cmdAutoBrightWidthOffset,		(CMDFUNC)cmdAutoBrightWidthOffsetHelp, 			OPT_NONE},
	{ (char *)"autobrightoffsety",	    	(CMDFUNC)cmdAutoBrightHeightOffset,		(CMDFUNC)cmdAutoBrightHeightOffsetHelp, 		OPT_NONE},
	{ (char *)"autobrighttargetarea",	   	(CMDFUNC)cmdAutoBrigtTargetArea,		(CMDFUNC)cmdAutoBrigtTargetAreaHelp, 			OPT_NONE},
#endif

		
//============================================================================================================================================
// CXP Command
//============================================================================================================================================
#if defined (MODE_CXP)
	//{ (char *)"cxpfiforead", 		 		(CMDFUNC)cmdCxpFifoRead,				(CMDFUNC)cmdCxpFifoReadHelp, 					OPT_NONE},
	//{ (char *)"cxpfifowrite", 		 		(CMDFUNC)cmdCxpFifoWrite,				(CMDFUNC)cmdCxpFifoWriteHelp, 					OPT_NONE},

	{ (char *)"cxpsendadrs", 		 		(CMDFUNC)cmdCxpSendAdrs,				(CMDFUNC)cmdCxpSendAdrsHelp, 					OPT_NONE},
	{ (char *)"cxprecvadrs", 		 		(CMDFUNC)cmdCxpRecvAdrs,				(CMDFUNC)cmdCxpRecvAdrsHelp, 					OPT_NONE},

	#if defined (MODE_BOARD_ACB525CXP)
	{ (char *)"cxpsendstart", 			 	(CMDFUNC)cmdCxpSendFifoStart,			(CMDFUNC)cmdCxpSendFifoStartHelp, 				OPT_NONE},
	{ (char *)"cxpfifoclear", 			 	(CMDFUNC)cmdCxpSendFifoClear,			(CMDFUNC)cmdCxpSendFifoClearHelp, 				OPT_NONE},
	{ (char *)"cxpfifosize", 			 	(CMDFUNC)cmdCxpSendFifoSize,			(CMDFUNC)cmdCxpSendFifoSizeHelp, 				OPT_NONE},
	{ (char *)"cxpsendcmdmode", 		 	(CMDFUNC)cmdCxpSendCmdMode,				(CMDFUNC)cmdCxpSendCmdModeHelp, 				OPT_NONE},
	#endif

	{ (char *)"iffpga-reconfig",		  	(CMDFUNC)cmdIfFpgaReConfig,				(CMDFUNC)cmdIfFpgaReConfigHelp, 				OPT_NONE},

	{ (char *)"cxpi2c",	  		 			(CMDFUNC)cmdCxpI2c,						(CMDFUNC)cmdCxpI2cHelp, 						OPT_NONE},
	{ (char *)"dumpcxp", 					(CMDFUNC)cmdDumpCxpI2cMain,				(CMDFUNC)cmdDumpCxpI2cMainHelp, 				OPT_NONE},
	{ (char *)"iocxp", 						(CMDFUNC)cmdAcesCxpI2cMain,				(CMDFUNC)cmdAcesCxpI2cMainHelp, 				OPT_NONE},

	{ (char *)"packetrx",				  	(CMDFUNC)cmdCxpTestPacketRx,			(CMDFUNC)cmdCxpTestPacketRxHelp, 				OPT_NONE},
	{ (char *)"packeterr",				  	(CMDFUNC)cmdCxpTestPacketErr,			(CMDFUNC)cmdCxpTestPacketErrHelp, 				OPT_NONE},

	#if defined (MODE_BOARD_ACB525CXP)
	{ (char *)"cxppowerctrl",			  	(CMDFUNC)cmdCxpPowerCtrl,				(CMDFUNC)cmdCxpPowerCtrlHelp, 					OPT_NONE},
	#endif

	#if defined (MODE_BOARD_ACB531CXP)
	{ (char *)"cxprate",				  	(CMDFUNC)cmdCxpRate,					(CMDFUNC)cmdCxpRateHelp, 						OPT_NONE},
	{ (char *)"cxpconnection",			  	(CMDFUNC)cmdCxpConnection,				(CMDFUNC)cmdCxpConnectionHelp, 					OPT_NONE},
	#endif
#endif


//============================================================================================================================================
// Diag Command
//============================================================================================================================================

	{ (char *)"diagddr",					(CMDFUNC)cmdDiagDdr,					(CMDFUNC)cmdDiagDdrHelp, 						OPT_NONE},
	{ (char *)"diagqspi",					(CMDFUNC)cmdDiagQspi,					(CMDFUNC)cmdDiagQspiHelp, 						OPT_NONE},
	{ (char *)"diagdma",					(CMDFUNC)cmdDiagDma,					(CMDFUNC)cmdDiagDmaHelp},
	{ (char *)"diagsensortemp",				(CMDFUNC)cmdDiagSensorTemp,				(CMDFUNC)cmdDiagSensorTempHelp, 				OPT_NONE},
	{ (char *)"diagcasetemp",				(CMDFUNC)cmdDiagCaseTemp,				(CMDFUNC)cmdDiagCaseTempHelp, 					OPT_NONE},
	{ (char *)"diagfpgatemp",				(CMDFUNC)cmdDiagFpgaTemp,				(CMDFUNC)cmdDiagFpgaTempHelp, 					OPT_NONE},


//============================================================================================================================================
// Debug
//============================================================================================================================================

	{ (char *)"console",					(CMDFUNC)cmdConsoleMode,				(CMDFUNC)cmdConsoleModeHelp, 					OPT_NONE},
	{ (char *)"dump", 						(CMDFUNC)cmdDumpMain,					(CMDFUNC)cmdDumpMainHelp, 						OPT_NONE},
	{ (char *)"io", 						(CMDFUNC)cmdAcesMain,					(CMDFUNC)cmdAcesMainHelp, 						OPT_NONE},
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	{ (char *)"dumpsensor", 				(CMDFUNC)cmdSensorDumpMain,				(CMDFUNC)cmdSensorDumpMainHelp, 				OPT_NONE},
	{ (char *)"iosensor", 					(CMDFUNC)cmdSensorAcesMain,				(CMDFUNC)cmdSensorAcesMainHelp, 				OPT_NONE},
	{ (char *)"iosensor8", 					(CMDFUNC)cmdIoSensor8,					(CMDFUNC)cmdIoSensor8Help, 						OPT_NONE},
#endif
	{ (char *)"iophy", 						(CMDFUNC)cmdAcesPhy,					(CMDFUNC)cmdAcesPhyHelp, 						OPT_NONE},
	{ (char *)"cacheflush",					(CMDFUNC)cmdCacheFlash,					(CMDFUNC)cmdCacheFlashHelp, 					OPT_NONE},
	{ (char *)"fill", 						(CMDFUNC)cmdFill,						(CMDFUNC)cmdFillHelp, 							OPT_NONE},
	{ (char *)"fillinc", 					(CMDFUNC)cmdFillInc,					(CMDFUNC)cmdFillIncHelp, 						OPT_NONE},
	{ (char *)"usdelay", 					(CMDFUNC)cmdUsDelay,					(CMDFUNC)cmdUsDelayHelp, 						OPT_NONE},

	{ (char *)"capture",					(CMDFUNC)cmdCapture,					(CMDFUNC)cmdCaptureHelp},
	{ (char *)"userset-factory-clear",		(CMDFUNC)cmdCameraFactoryClear,			(CMDFUNC)cmdCameraFactoryClearHelp, 			OPT_NONE},
	{ (char *)"userset-common-clear",		(CMDFUNC)cmdCameraCommonClear,			(CMDFUNC)cmdCameraCommonClearHelp, 				OPT_NONE},
	{ (char *)"qspiflashprotect",			(CMDFUNC)cmdDiagQspiWriteProtect,		(CMDFUNC)cmdDiagQspiWriteProtectHelp, 			OPT_NONE},

#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
	{ (char *)"sensortmg",					(CMDFUNC)cmdSensorTmg,					(CMDFUNC)cmdSensorTmgHelp, 						OPT_NONE},
	{ (char *)"blackpixel",					(CMDFUNC)cmdSensorBlackPixel,			(CMDFUNC)cmdSensorBlackPixelHelp, 				OPT_NONE},
	{ (char *)"sensorstandby",				(CMDFUNC)cmdSensorStandby,				(CMDFUNC)cmdSensorStandbyHelp, 					OPT_NONE},
#endif

	{ (char *)"timer",						(CMDFUNC)cmdDiagTimer,					(CMDFUNC)cmdDiagTimerHelp, 						OPT_NONE},
	{ (char *)"diag2",						(CMDFUNC)cmdDiagMain2,					(CMDFUNC)cmdDiagMain2Help, 						OPT_NONE},
	{ (char *)"aging",						(CMDFUNC)cmdDiagAging,					(CMDFUNC)cmdDiagAgingHelp, 						OPT_NONE},
	
	{ (char *)NULL, 0, 0, OPT_NONE}
};

// eof
