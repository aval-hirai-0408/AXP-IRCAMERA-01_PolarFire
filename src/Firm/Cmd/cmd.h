//**********************************************************************************
//
//                              Camera Program Header
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// cmd.h - Command Program Header
//**********************************************************************************

#ifndef __CMD_H__
#define __CMD_H__


//----------------------------------------------------------------------------------
// Function Define
//----------------------------------------------------------------------------------

// cmdAcquisitionContorl.c
int cmdAcquisitionGcp (void *str);
int cmdAcquisitionGcpHelp (void *str);
int acquisitionCamer (void);
int cmdAcquisitionCamer (void *str);
int acquisitionStatusAll (void);
int cmdAcquisitionStatusAll (void *str);
int cmdAcquisitionStatusAllHelp (void *str);
int cmdAcquisitionMode (void *str);
int cmdAcquisitionModeHelp (void *str);
int cmdAcquisitionStart (void *str);
int cmdAcquisitionStartHelp (void *str);
int cmdAcquisitionStop (void *str);
int cmdAcquisitionStopHelp (void *str);
int cmdAcquisitionFrameCount (void *str);
int cmdAcquisitionFrameCountHelp (void *str);
int cmdAcquisitionStatus (void *str);
int cmdAcquisitionStatusHelp (void *str);
int cmdAcquisitionStatus (void *str);
int cmdAcquisitionStatusHelp (void *str);
int cmdAcquisitionFramerate (void *str);
int cmdAcquisitionFramerateHelp (void *str);
int cmdAcquisitionExposure (void *str);
int cmdAcquisitionExposureHelp (void *str);
int cmdAcquisitionTrgSelect (void *str);
int cmdAcquisitionTrgSelectHelp (void *str);
int cmdAcquisitionTrgMode (void *str);
int cmdAcquisitionTrgModeHelp (void *str);
int cmdAcquisitionSoftTrg (void *str);
int cmdAcquisitionSoftTrgHelp (void *str);
int cmdAcquisitionTrgCondition (void *str);
int cmdAcquisitionTrgConditionHelp (void *str);
int cmdAcquisitionTrgSource (void *str);
int cmdAcquisitionTrgSourceHelp (void *str);
int cmdAcquisitionTrgActivation (void *str);
int cmdAcquisitionTrgActivationHelp (void *str);
int cmdAcquisitionTrgDelay (void *str);
int cmdAcquisitionTrgDelayHelp (void *str);
int cmdAcquisitionExposureMode (void *str);
int cmdAcquisitionExposureModeHelp (void *str);
int cmdAcquisitionTrgInvalidCount (void *str);
int cmdAcquisitionTrgInvalidCountHelp (void *str);
int cmdAcquisitionTrgReserved (void *str);
int cmdAcquisitionTrgReservedHelp (void *str);
int cmdAcquisitionReset (void *str);
int cmdAcquisitionResetHelp (void *str);
int cmdExposureMin (void *str);
int cmdExposureMinHelp (void *str);
int cmdExposureMax (void *str);
int cmdExposureMaxHelp (void *str);
int cmdRateMin (void *str);
int cmdRateMinHelp (void *str);
int cmdRateMax (void *str);
int cmdRateMaxHelp (void *str);
int cmdSensoreGetReadOut (void *str);
int cmdSensoreGetReadOutHelp (void *str);
int cmdRateMode (void *str);
int cmdRateModeHelp (void *str);
int cmdAcquisitionTrgCount (void *str);
int cmdAcquisitionTrgCountHelp (void *str);

// cmdAoi.c
int aoi (void);
int cmdAoi (void *str);
int cmdAoiBitWidth (void *str);
int cmdAoiBitWidthHelp (void *str);
int cmdSensorWidth (void *str);
int cmdSensorWidthHelp (void *str);
int cmdSensorHeight (void *str);
int cmdSensorHeightHelp (void *str);
int cmdWidthMax (void *str);
int cmdWidthMaxHelp (void *str);
int cmdHeightMax (void *str);
int cmdHeightMaxHelp (void *str);
int cmdAoiWidth (void *str);
int cmdAoiWidthHelp (void *str);
int cmdAoiHeight (void *str);
int cmdAoiHeightHelp (void *str);
int cmdAoiWidthOffset (void *str);
int cmdAoiWidthOffsetHelp (void *str);
int cmdAoiHeightOffset (void *str);
int cmdAoiHeightOffsetHelp (void *str);
int cmdAoiTpMode (void *str);
int cmdAoiTpModeHelp (void *str);
int cmdAoiTpInc (void *str);
int cmdAoiTpIncHelp (void *str);
int cmdAoiTpPosition (void *str);
int cmdAoiTpPositionHelp (void *str);
int cmdSensorTpMode (void *str);
int cmdSensorTpModeHelp (void *str);
int cmdSensorTpInc (void *str);
int cmdSensorTpIncHelp (void *str);
int cmdSensorTpData (void *str);
int cmdSensorTpDataHelp (void *str);
int cmdAoiXflip (void *str);
int cmdAoiXflipHelp (void *str);
int cmdAoiYflip (void *str);
int cmdAoiYflipHelp (void *str);
int cmdLutCalcExchange (void *str);
int cmdLutCalcExchangeHelp (void *str);
int cmdAoiBinningX(void *str);
int cmdAoiBinningXHelp(void *str);
int cmdAoiBinningY(void *str);
int cmdAoiBinningYHelp(void *str);
int cmdAoiBinningMode(void *str);
int cmdAoiBinningModeHelp(void *str);

// cmdAutoBright.c
int cmdAutoBrightExposureMode (void *str);
int cmdAutoBrightExposureModeHelp (void *str);
int cmdAutoBrightExposureStatus (void *str);
int cmdAutoBrightExposureStatusHelp (void *str);
int cmdAutoBrightExposureMax (void *str);
int cmdAutoBrightExposureMaxHelp (void *str);
int cmdAutoBrightGainMode (void *str);
int cmdAutoBrightGainModeHelp (void *str);
int cmdAutoBrightGainStatus (void *str);
int cmdAutoBrightGainStatusHelp (void *str);
int cmdAutoBrightGainMax (void *str);
int cmdAutoBrightGainMaxHelp (void *str);
int cmdAutoBrightDetectArea (void *str);
int cmdAutoBrightDetectAreaHelp (void *str);
int cmdAutoBrightTarget (void *str);
int cmdAutoBrightTargetHelp (void *str);
int cmdAutoBrightAverage (void *str);
int cmdAutoBrightAverageHelp (void *str);
int cmdAutoBrightWidthSize (void *str);
int cmdAutoBrightWidthSizeHelp (void *str);
int cmdAutoBrightHeightSize (void *str);
int cmdAutoBrightHeightSizeHelp (void *str);
int cmdAutoBrightWidthOffset (void *str);
int cmdAutoBrightWidthOffsetHelp (void *str);
int cmdAutoBrightHeightOffset (void *str);
int cmdAutoBrightHeightOffsetHelp (void *str);
int cmdAutoBrigtTargetArea (void *str);
int cmdAutoBrigtTargetAreaHelp (void *str);

// cmdBoard.c
int cmdDipsw (void *str);
int cmdDipswHelp (void *str);
int cmdReset (void *str);
int cmdResetHelp (void *str);
int cmdFirmBuildInfo (void *str);
int cmdFirmBuildInfoHelp (void *str);
int cmdFpgaBuildInfo (void *str);
int cmdFpgaBuildInfoHelp (void *str);
int cmdBoardInfo (void *str);
int cmdBoardInfoHelp (void *str);
int boardInfo (void);
int cmdProductInfo (void *str);
int product (void);
int cmdGetVendor (void *str);
int cmdGetVendorHelp (void *str);
int cmdSetVendor (void *str);
int cmdSetVendorHelp (void *str);
int cmdDeleteVendor (void *str);
int cmdDeleteVendorHelp (void *str);
int cmdDefaultVendor (void *str);
int cmdDefaultVendorHelp (void *str);
int cmdGetManufacture (void *str);
int cmdGetManufactureHelp (void *str);
int cmdSetManufacture (void *str);
int cmdSetManufactureHelp (void *str);
int cmdDeleteManufacture (void *str);
int cmdDeleteManufactureHelp (void *str);
int cmdDefaultManufacture (void *str);
int cmdDefaultManufactureHelp (void *str);
int cmdGetModel (void *str);
int cmdGetModelHelp (void *str);
int cmdSetModel (void *str);
int cmdSetModelHelp (void *str);
int cmdDeleteModel (void *str);
int cmdDeleteModelHelp (void *str);
int cmdDefaultModel (void *str);
int cmdDefaultModelHelp (void *str);
int cmdSetBoardId (void *str);
int cmdSetBoardIdHelp (void *str);
int cmdGetBoardId (void *str);
int cmdGetBoardIdHelp (void *str);
int cmdSetSensorId (void *str);
int cmdSetSensorIdHelp (void *str);
int cmdGetSensorId (void *str);
int cmdGetSensorIdHelp (void *str);

// cmdCamera.c
int camerInfomation (void);
int cmdCameraType (void *str);
int cmdCameraTypeHelp (void *str);
int cmdCapture  (void *str);
int cmdCaptureHelp (void *str);
int cmdCameraStatus (void *str);
int cmdCameraStatusHelp (void *str);
int cmdConsoleMode (void *str);
int cmdConsoleModeHelp (void *str);

// cmdCameraSave.c
int cmdCameraUser (void *str);
int cmdCameraUserHelp (void *str);
int cmdCameraUserBoot (void *str);
int cmdCameraUserBootHelp (void *str);
int cmdCameraUserSave (void *str);
int cmdCameraUserSaveHelp (void *str);
int cmdCameraUserLoad (void *str);
int cmdCameraUserLoadHelp (void *str);
int cmdCameraUserSetDefault (void *str);
int cmdCameraUserSetDefaultHelp (void *str);
int cmdCameraCommonClear (void *str);
int cmdCameraCommonClearHelp (void *str);
int cmdCameraFactoryClear (void *str);
int cmdCameraFactoryClearHelp (void *str);

// cmdCxp.c
int cmdCxpFifoRead (void *str);
int cmdCxpFifoReadHelp (void *str);
int cmdCxpFifoWrite (void *str);
int cmdCxpFifoWriteHelp (void *str);
int cmdCxpSendAdrs (void *str);
int cmdCxpSendAdrsHelp (void *str);
int cmdCxpRecvAdrs (void *str);
int cmdCxpRecvAdrsHelp (void *str);
int cmdCxpConnectionConfig (void *str);
int cmdCxpConnectionConfigHelp (void *str);
int cmdIfFpgaReConfig (void *str);
int cmdIfFpgaReConfigHelp (void *str);
int cmdCxpI2c (void *str);
int cmdCxpI2cHelp (void *str);
int cmdAcesCxpI2cMain (void *str);
int cmdAcesCxpI2cMainHelp (void *str);
int cmdDumpCxpI2cMain (void *str);
int cmdDumpCxpI2cMainHelp (void *str);
int cmdAcesDumpCxpI2cMain (void *str, int mode);
int cmdAcesCxpI2c (unsigned long *adrs, unsigned int *acesSize);
int cmdDumpCxoP2c (unsigned long *adrs, unsigned  int *acesSize);
int cmdCxpTestPacketRx (void *str);
int cmdCxpTestPacketRxHelp (void *str);
int cmdCxpTestPacketErr (void *str);
int cmdCxpTestPacketErrHelp (void *str);
int cmdCxpRate (void *str);
int cmdCxpRateHelp (void *str);
int cmdCxpConnection (void *str);
int cmdCxpConnectionHelp (void *str);

// cmdCounterControl.c
int counterStatusAll (void);
int cmdCounterStatusAll (void *str);
int cmdCounterStatusAllHelp (void *str);
int cmdCounterSelect (void *str);
int cmdCounterSelectHelp (void *str);
int cmdCounterEventSource (void *str);
int cmdCounterEventSourceHelp (void *str);
int cmdCounterEventActivation (void *str);
int cmdCounterEventActivationHelp (void *str);
int cmdCounterResetSource (void *str);
int cmdCounterResetSourceHelp (void *str);
int cmdCounterResetActivation (void *str);
int cmdCounterResetActivationHelp (void *str);
int cmdCounterReset (void *str);
int cmdCounterResetHelp (void *str);
int cmdCounterValue (void *str);
int cmdCounterValueHelp (void *str);
int cmdCounterResetValue (void *str);
int cmdCounterResetValueHelp (void *str);
int cmdCounterDuratione (void *str);
int cmdCounterDurationeHelp (void *str);
int cmdCounterStatus (void *str);
int cmdCounterStatusHelp (void *str);
int cmdCounterTrgSource (void *str);
int cmdCounterTrgSourceHelp (void *str);
int cmdCounterTrgActivation (void *str);
int cmdCounterTrgActivationHelp (void *str);
int cmdCounterSetTrgMode (void *str);
int cmdCounterSetTrgModeHelp (void *str);
int cmdCounterGetTrgInputCount (void *str);
int cmdCounterGetTrgInputCountHelp (void *str);
int cmdCounterGetImageOutputCount (void *str);
int cmdCounterGetImageOutputCountHelp (void *str);

// cmdDOffsetGain.c
int dog (void);
int cmdDog (void *str);
int cmdDogMode (void *str);
int cmdDogModeHelp (void *str);
int cmdDigitalGain (void *str);
int cmdDigitalGainHelp (void *str);
int cmdDigitalGainX (void *str);
int cmdDigitalGainXHelp (void *str);
int cmdDigitalOffset (void *str);
int cmdDigitalOffsetHelp (void *str);
int cmdDigitalOffset1 (void *str);
int cmdDigitalOffset1Help (void *str);

// cmdDigitalIoControl.c
int digitaiIoStatusAll (void);
int cmdDigitaiIoStatusAll (void *str);
int cmdDigitaiIoStatusAllHelp (void *str);
int cmdDigitalIoLineSelect (void *str);
int cmdDigitalIoLineSelectHelp (void *str);
int cmdDigitalIoLineMode (void *str);
int cmdDigitalIoLineModeHelp (void *str);
int cmdDigitalIoLineInverter (void *str);
int cmdDigitalIoLineInverterHelp (void *str);
int cmdDigitalIoLineStatus (void *str);
int cmdDigitalIoLineStatusHelp (void *str);
int cmdDigitalIoLineStatusAll (void *str);
int cmdDigitalIoLineStatusAllHelp (void *str);
int cmdDigitalIoLineSource (void *str);
int cmdDigitalIoLineSourceHelp (void *str);
int cmdDigitalIoLineFormat (void *str);
int cmdDigitalIoLineFormatHelp (void *str);
int cmdDigitalIoUserSelector (void *str);
int cmdDigitalIoUserSelectorHelp (void *str);
int cmdDigitalIoUserValue (void *str);
int cmdDigitalIoUserValueHelp (void *str);
int cmdDigitalIoUserAllValue (void *str);
int cmdDigitalIoUserAllValueHelp (void *str);
int cmdDigitalIoUserAllMask (void *str);
int cmdDigitalIoUserAllMaskHelp (void *str);

// cmdDiagFlash.c
int devGetFlashSize (int devType, unsigned int *pSize, unsigned int *pSecSize);
int devFlashEraseSector (int devType, int sector);
int devFlashEraseAll (int devType);
int devFlashRead (int devType, unsigned int adrs, unsigned char *pData, unsigned int size);
int devFlashWrite (int devType, unsigned int adrs, unsigned char *pData, unsigned int size);
int diagFlashEraseSector (void *str, int devType);
int diagFlashEraseAll (void *str, int devType);
int diagFlashRW (void *str, int devType);
int diagFlashRWAll (void *str, int devType);
int diagFlashRWRandom (void *str, int devType);
int diagFlashRWSector (void *str, int devType);
int diagFlashBlankCheck (void *str, int devType);

// cmdDigitalOffsetGain.c
int dogPre (void);
int cmdDogPre (void *str);
int cmdDogPreUpdate (void *str);
int cmdDogPreUpdateHelp (void *str);
int cmdDogPreMode (void *str);
int cmdDogPreModeHelp (void *str);
int cmdDogPreClip (void *str);
int cmdDogPreClipHelp (void *str);
int cmdDigitalPreGain (void *str);
int cmdDigitalPreGainHelp (void *str);
int cmdDigitalPreGainX (void *str);
int cmdDigitalPreGainXHelp (void *str);
int cmdDigitalPreOffset (void *str);
int cmdDigitalPreOffsetHelp (void *str);
int cmdDigitalPreOffset1 (void *str);
int cmdDigitalPreOffset1Help (void *str);

// cmdDpc.c
int dpc (void);
int cmdDpcShow  (void *str);
int cmdDpc (void *str);
int cmdDpcHelp (void *str);
int cmdDpcAdjustMode (void *str);
int cmdDpcAdjustModeHelp (void *str);
int cmdDpcLoad (void *str);
int cmdDpcLoadHelp (void *str);
int cmdDpcLoadAdmin (void *str);
int cmdDpcLoadAdminHelp (void *str);
int cmdDpcSave (void *str);
int cmdDpcSaveHelp (void *str);
int cmdDpcSaveAdmin (void *str);
int cmdDpcSaveAdminHelp (void *str);
int cmdDpcDataUpload (void *str);
int cmdDpcDataUploadHelp (void *str);
int cmdDpcDataDownload (void *str);
int cmdDpcDataDownloadHelp (void *str);
int cmdDpcDataDownloadCompress (void *str);
int cmdDpcDataDownloadCompressHelp (void *str);
int cmdDpcDefault (void *str);
int cmdDpcDefaultHelp (void *str);
int cmdDpcAddGrid (void *str);
int cmdDpcAddGridHelp (void *str);
int cmdDpcDeleteGrid (void *str);
int cmdDpcDeleteGridHelp (void *str);
int cmdDpcClearGrid (void *str);
int cmdDpcClearGridHelp (void *str);
int cmdDpcGetGrid (void *str);
int cmdDpcGetGridHelp (void *str);
int cmdDpcGridCheck (void *str);
int cmdDpcGridCheckHelp (void *str);
int cmdDpMmapChange (void *str);
int cmdDpMmapChangeHelp (void *str);
int cmdDpcAddGridLine (void *str);
int cmdDpcAddGridLineHelp (void *str);
int cmdDpcDeleteGridLine (void *str);
int cmdDpcDeleteGridLineHelp (void *str);

// cmdDpcAdjust.c
int cmdDpcAdjustInfo (void *str);
int cmdDpcAdjustInfoHelp (void *str);
int cmdDpcMapInfo (void *str);
int cmdDpcMapInfoHelp (void *str);
int cmdDpcMapInfoAdmin (void *str);
int cmdDpcMapInfoAdminHelp (void *str);
int cmdDpcMapInfoImpossible (void *str);
int cmdDpcMapInfoImpossibleHelp (void *str);
int cmdDpcMapInfoImpossibleAdmin (void *str);
int cmdDpcMapInfoImpossibleAdminHelp (void *str);
int cmdDpcPixelNum (void *str);
int cmdDpcPixelNumHelp (void *str);
int cmdDpcPixelNumAdmin (void *str);
int cmdDpcPixelNumAdminHelp (void *str);
int cmdDpcPixelNumAll (void *str);
int cmdDpcPixelNumAllHelp (void *str);
int cmdDpcSetPixelNum (void *str);
int cmdDpcSetPixelNumHelp (void *str);
int cmdDpcPixelMaxNum (void *str);
int cmdDpcPixelMaxNumHelp (void *str);
int cmdDpcAdjustInfoAdmin (void *str);
int cmdDpcAdjustInfoAdminHelp (void *str);
int cmdDpcAdjustBatch1 (void *str);
int cmdDpcAdjustBatch1Help (void *str);
int cmdDpcAdjustBatch2 (void *str);
int cmdDpcAdjustBatch2Help (void *str);
int cmdDpcAdjustBatch3 (void *str);
int cmdDpcAdjustBatch3Help (void *str);
int cmdDpcAdjustBatch2Detail (void *str);
int cmdDpcAdjustBatch2DetailHelp (void *str);
int cmdDpcAbort (void *str);
int cmdDpcAbortHelp (void *str);
int cmdDpcCounterDecrement (void *str);
int cmdDpcCounterDecrementHelp (void *str);
int cmdDpcStateCount (void *str);
int cmdDpcStateCountHelp (void *str);
int cmdDpcAdjustRate (void *str);
int cmdDpcAdjustRateHelp (void *str);
int cmdDpcAdjustExp (void *str);
int cmdDpcAdjustExpHelp (void *str);
int cmdDpcAdjustFfcNumber (void *str);
int cmdDpcAdjustFfcNumberHelp (void *str);
int cmdDpcAdjustTemp (void *str);
int cmdDpcAdjustTempHelp (void *str);
int cmdDpcAdjustSd (void *str);
int cmdDpcAdjustSdHelp (void *str);
int cmdDpcAdjustNonUniform (void *str);
int cmdDpcAdjustNonUniformHelp (void *str);

// cmdEncoderControl.c
int encoderStatusAll (void);
int cmdEncoderStatusAll (void *str);
int cmdEncoderStatusAllHelp (void *str);
int cmdEncoderPhaseATrgSource (void *str);
int cmdEncoderPhaseATrgSourceHelp (void *str);
int cmdEncoderPhaseBTrgSource (void *str);
int cmdEncoderPhaseBTrgSourceHelp (void *str);
int cmdEncoderMode (void *str);
int cmdEncoderModeHelp (void *str);
int cmdEncoderDivider (void *str);
int cmdEncoderDividerHelp (void *str);
int cmdEncoderOutputMode (void *str);
int cmdEncoderOutputModeHelp (void *str);
int cmdEncoderStatus (void *str);
int cmdEncoderStatusHelp (void *str);
int cmdEncoderTimeout (void *str);
int cmdEncoderTimeoutHelp (void *str);
int cmdEncoderTimeout (void *str);
int cmdEncoderTimeoutHelp (void *str);
int cmdEncoderResetTrgSource (void *str);
int cmdEncoderResetTrgSourceHelp (void *str);
int cmdEncoderResetActivation (void *str);
int cmdEncoderResetActivationHelp (void *str);
int cmdEncoderReset (void *str);
int cmdEncoderResetHelp (void *str);
int cmdEncoderValue (void *str);
int cmdEncoderValueHelp (void *str);
int cmdEncoderValueAtReset (void *str);
int cmdEncoderValueAtResetHelp (void *str);
int cmdEncoderFrequency (void *str);
int cmdEncoderFrequencyHelp (void *str);

// cmdFfc.c
int cmdIpu (void *str);
int cmdIpuHelp (void *str);
int ffc (void);
int cmdFfcShow  (void *str);
int cmdFfcLoad (void *str);
int cmdFfcLoadHelp (void *str);
int cmdFfcLoadAdmin (void *str);
int cmdFfcLoadAdminHelp (void *str);
int cmdFfc (void *str);
int cmdFfcHelp (void *str);
int cmdFfcMode (void *str);
int cmdFfcModeHelp (void *str);
int cmdFfcCorMode (void *str);
int cmdFfcCorModeHelp (void *str);
int cmdFfcRegInitIntMem (void *str);
int cmdFfcRegInitIntMemHelp (void *str);
int cmdFfcSave (void *str);
int cmdFfcSaveHelp (void *str);
int cmdFfcSaveAdmin (void *str);
int cmdFfcSaveAdminHelp (void *str);
int cmdFfcSaveAdminAll (void *str);
int cmdFfcSaveAdminAllHelp (void *str);
int cmdFfcOffsetData (void *str);
int cmdFfcOffsetDataHelp (void *str);
int cmdFfcGainData (void *str);
int cmdFfcGainDataHelp (void *str);
int cmdFfcGainX (void *str);
int cmdFfcGainXHelp (void *str);
int cmdFfcOffsetAve (void *str);
int cmdFfcOffsetAveHelp (void *str);
int cmdFfcGainAve (void *str);
int cmdFfcGainAveHelp (void *str);
int cmdFfcDataUpload (void *str);
int cmdFfcDataUploadHelp (void *str);
int cmdFfcDataUploadBin (void *str);
int cmdFfcDataUploadBinHelp (void *str);
int cmdFfcDataDownload (void *str);
int cmdFfcDataDownloadHelp (void *str);
int cmdFfcDataDownloadCompress (void *str);
int cmdFfcDataDownloadCompressHelp (void *str);
int cmdFfcDataDownloadBin (void *str);
int cmdFfcDataDownloadBinHelp (void *str);
int cmdFfcDataDownloadCompressBin (void *str);
int cmdFfcDataDownloadCompressBinHelp (void *str);
int cmdFfcDefault (void *str);
int cmdFfcDefaultHelp (void *str);
int cmdFfcBlackTarget (void *str);
int cmdFfcBlackTargetHelp (void *str);
int cmdFfcWhiteTarget (void *str);
int cmdFfcWhiteTargetHelp (void *str);

// cmdFfcAdjust.c
int cmdIpu  (void *str);
int cmdIpuHelp (void *str);
int cmdFfcAdjustInfo (void *str);
int cmdFfcAdjustInfoHelp (void *str);
int cmdFfcAdjustInfoFactory (void *str);
int cmdFfcAdjustInfoFactoryHelp (void *str);
int cmdFfcAdjustInfoAdmin (void *str);
int cmdFfcAdjustInfoAdminHelp (void *str);
int cmdFfcAdjustRate (void *str);
int cmdFfcAdjustRateHelp (void *str);
int cmdFfcAdjustExp (void *str);
int cmdFfcAdjustExpHelp (void *str);
int cmdFfcAdjustTarget (void *str);
int cmdFfcAdjustTargetHelp (void *str);
int cmdFfcAdjustTemp (void *str);
int cmdFfcAdjustTempHelp (void *str);
int cmdFfcAdjustGainX (void *str);
int cmdFfcAdjustGainXHelp (void *str);
int cmdFfcAdjustBit (void *str);
int cmdFfcAdjustBitHelp (void *str);
int cmdFfcAdjustBatchBlack(void *str);
int cmdFfcAdjustBatchBlackHelp (void *str);
int cmdFfcAdjustBatchWhite (void *str);
int cmdFfcAdjustBatchWhiteHelp (void *str);
int cmdFfcAdjustBatchShading (void *str);
int cmdFfcAdjustBatchShadingHelp (void *str);
int cmdFfcWhiteGainX (void *str);
int cmdFfcWhiteGainXHelp (void *str);
int cmdFfcAdjustBatchBlackAll(void *str);
int cmdFfcAdjustBatchBlackAllHelp (void *str);
int cmdFfcAdjustBatchWhiteAll (void *str);
int cmdFfcAdjustBatchWhiteAllHelp (void *str);
int cmdFfcAdjustBatchMediumAll (void *str);
int cmdFfcAdjustBatchMediumAllHelp (void *str);
int cmdFfcAdjustBatchShadingAll (void *str);
int cmdFfcAdjustBatchShadingAllHelp (void *str);

// cmdFunc.c
int cmdGcp (void *str);
int cmdGcpHelp (void *str);
int cmdSensorGcp (void *str);
int cmdSensorGcpHelp (void *str);
int cmdUsDelay (void *str);
int cmdUsDelayHelp (void *str);
int cmdFill  (void *str);
int cmdFillHelp (void *str);
int cmdFillInc  (void *str);
int cmdFillIncHelp (void *str);
int cmdCacheFlash (void *str);
int cmdCacheFlashHelp (void *str);
int cmdDefaultAll (void *str);
int cmdDefaultAllHelp (void *str);
int cmdRegMap (void *str);
int cmdRegMapHelp (void *str);
int cmdCameraUserID (void *str);
int cmdCameraUserIDHelp (void *str);
int cmdCameraUserIDDelete (void *str);
int cmdCameraUserIDDeleteHelp (void *str);
int cmdDiagTimer (void *str);
int cmdDiagTimerHelp (void *str);

// cmdGigE.c
int cmdPhyCount (void *str);
int cmdPhyCountHelp (void *str);
int cmdPhySpeed (void *str);
int cmdPhySpeedHelp (void *str);
int cmdGevSpeedConfig (void *str);
int cmdGevSpeedConfigHelp (void *str);
int cmdGigEPacketDelayCalc (void *str);
int cmdGigEPacketDelayCalcHelp (void *str);
int cmdPhyFirmVersion (void *str);
int cmdPhyFirmVersionHelp (void *str);
int cmdPhyApiVersion (void *str);
int cmdPhyApiVersionHelp (void *str);
int cmdPhyTemp (void *str);
int cmdPhyTempHelp (void *str);

// cmdGpio.c
int cmdGpi (void *str);
int cmdGpiHelp (void *str);
int cmdGpo (void *str);
int cmdGpoHelp (void *str);
int cmdGpioDnf (void *str);
int cmdGpioDnfHelp (void *str);
int cmdGpioPulseTime (void *str);
int cmdGpioPulseTimeHelp (void *str);

// cmdIo.c
int cmdIo8 (void *str);
int cmdIo16 (void *str);
int cmdIo32 (void *str);
int cmdIof (void *str);
int cmdIoXHelp (void *str);
int cmdAcesMain  (void *str);
int cmdAcesMainHelp (void *str);
int cmdDumpMain  (void *str);
int cmdDumpMainHelp  (void *str);
int cmdAcesDumpMain (void *str, int mode);
int cmdAces (unsigned long *adrs, unsigned int *acesSize);
int cmdDump (unsigned long *adrs, unsigned int *acesSize);
int cmdAcesAxiQspiFlash (unsigned long *adrs, unsigned int *acesSize);
int cmdDumpAxiQspiFlash (unsigned long *adrs, unsigned  int *acesSize);
int cmdAcesQspiFlashMain (void *str);
int cmdDumpQspiFlashMain  (void *str);
int cmdAcesGigEPhyMain (void *str);
int cmdDumpGigEPhyMain (void *str);
int cmdAcesDiagDumpMain (int mode, int devType);
int cmdAcesDiagDumpMain2 (unsigned long adrs, int mode, int devType);
int cmdDiagDeviceAces (unsigned long *adrs, unsigned int *acesSize, int devType);
int cmdDiagDeviceDump (unsigned long *adrs, unsigned  int *acesSize, int devType);
int cmdDiagDeviceRead (unsigned long adrs, unsigned int *pData, int acesSize, int devType);
int cmdDiagDeviceWrite (unsigned long adrs, unsigned int data, int acesSize, int devType);
int cmdDiagDeviceErase (unsigned long adrs, int devType);
int cmdAcesPhy (void *str);
int cmdAcesPhyHelp (void *str);
int cmdPhyDump (void *str);
int cmdPhyDumpHelp (void *str);

// cmdIoSensor.c
int cmdSensorAcesMain (void *str);
int cmdSensorAcesMainHelp (void *str);
int cmdSensorDumpMain (void *str);
int cmdSensorDumpMainHelp (void *str);
int cmdSensorAcesDumpMain (void *str, int mode);
int cmdSensorAces (int *id, unsigned long *adrs, unsigned int *acesSize);
int cmdSensorDump (int *id, unsigned long *adrs, unsigned int *acesSize);
int cmdIoSensor8 (void *str);
int cmdIoSensor8Help (void *str);

// cmdLed.c
int cmdLed (void *str);
int cmdLedHelp (void *str);
int cmdLedDebugMode (void *str);
int cmdLedDebugModeHelp (void *str);

// cmdLut.c
int lut (void );
int cmdLut  (void *str);
int cmdLutMode (void *str);
int cmdLutModeHelp (void *str);
int cmdLutFormat (void *str);
int cmdLutFormatHelp (void *str);
int cmdLutBinThreshold (void *str);
int cmdLutBinThresholdHelp (void *str);
int cmdLutGamma (void *str);
int cmdLutGammaHelp (void *str);
int cmdLutTableData (void *str);
int cmdLutTableDataHelp (void *str);
int cmdLutSave (void *str);
int cmdLutSaveHelp (void *str);
int cmdLutDefault (void *str);
int cmdLutDefaultHelp (void *str);

// cmdRoi.c
int cmdRoiSelector (void *str);
int cmdRoiSelectorHelp (void *str);
int cmdRoiMultiY (void *str);
int cmdRoiMultiYHelp (void *str);
int cmdRoiShow (void *str);
int cmdRoiShowHelp (void *str);
int cmdRoiEntryCount (void *str);
int cmdRoiEntryCountHelp (void *str);
int cmdRoiCameraHeightTotal (void *str);
int cmdRoiCameraHeightTotalHelp (void *str);
int cmdRoiSensorHeightTotal (void *str);
int cmdRoiSensorHeightTotalHelp (void *str);
int cmdRoiDefaultY (void *str);
int cmdRoiDefaultYHelp (void *str);
int cmdRoiAreaSize (void *str);
int cmdRoiAreaSizeHelp (void *str);
int cmdRoiAreaFlag (void *str);
int cmdRoiAreaFlagHelp (void *str);

// cmdSensor.c
int cmdSensorTmg (void *str);
int cmdSensorTmgHelp (void *str);
int cmdSensorBlackPixel (void *str);
int cmdSensorBlackPixelHelp (void *str);
int cmdSensorGainDB (void *str);
int cmdSensorGainDBHelp (void *str);
int cmdSensorGainX (void *str);
int cmdSensorGainXHelp (void *str);
int cmdSensorConversionGain (void *str);
int cmdSensorConversionGainHelp (void *str);
int cmdSensorGradCompMode (void *str);
int cmdSensorGradCompModeHelp (void *str);
int cmdSensorGradCompPosition (void *str);
int cmdSensorGradCompPositionHelp (void *str);
int cmdSensorGradCompGain (void *str);
int cmdSensorGradCompGainHelp (void *str);
int cmdSensor8BitConvert (void *str);
int cmdSensor8BitConvertHelp (void *str);
int cmdSensorFrameRateHighSpeedMode (void *str);
int cmdSensorFrameRateHighSpeedModeHelp (void *str);
int cmdSensorVirtualHeightMode (void *str);
int cmdSensorVirtualHeightModeHelp (void *str);
int cmdSensorVirtualHeightLineScanMode (void *str);
int cmdSensorVirtualHeightLineScanModeHelp (void *str);
int cmdSensorVirtualHeight (void *str);
int cmdSensorVirtualHeightHelp (void *str);
int cmdSensorStandby (void *str);
int cmdSensorStandbyHelp (void *str);
int cmdSensorDrrsMode (void *str);
int cmdSensorDrrsModeHelp (void *str);

// cmdTemp.c
int cmdTempStatus (void *str);
int cmdTempStatusHelp (void *str);
int tempStatus (void);
int tempShow (void);
int cmdTempShow (void *str);
int tempAlarm (void);
int tempAlarmStatus (void);
int tempAlarmCount (void);
int cmdTempAlarm (void *str);
int cmdTempAlarmStatusShow (void *str);
int cmdTempAlarmCount (void *str);
int cmdTemp (void *str);
int cmdTempHelp (void *str);
int cmdSensorTempAlrm (void *str);
int cmdSensorTempAlrmHelp (void *str);
int cmdCaseTempAlrm (void *str);
int cmdCaseTempAlrmHelp (void *str);
int cmdTempAlarmStatus (void *str);
int cmdTempAlarmStatusHelp (void *str);
int cmdTempAlarmClear (void *str);
int cmdTempAlarmClearHelp (void *str);
int cmdPeltierTarget  (void *str);
int cmdPeltierTargetHelp (void *str);
int cmdPeltierTempMax (void *str);
int cmdPeltierTempMaxHelp (void *str);
int cmdPeltierTempMin (void *str);
int cmdPeltierTempMinHelp (void *str);	
int cmdPeltierEnable (void *str);
int cmdPeltierEnableHelp (void *str);
int cmdPeltierVolt (void *str);
int cmdPeltierVoltHelp (void *str);
int cmdPeltierCurrent (void *str);
int cmdPeltierCurrentHelp (void *str);
int cmdCaseTempMode (void *str);
int cmdCaseTempModeHelp (void *str);
int cmdTempAbnormalStatus (void *str);
int cmdTempAbnormalStatusHelp (void *str);
int cmdTempAbnormalCount (void *str);
int cmdTempAbnormalCountHelp (void *str);
int cmdPeltierPowerLevel (void *str);
int cmdPeltierPowerLevelHelp (void *str);

// cmdTg.c
int cmdTgpd (void *str);
int cmdTgpdHelp (void *str);
int cmdTges (void *str);
int cmdTgesHelp (void *str);
int cmdTgse (void *str);
int cmdTgseHelp (void *str);

// cmdTimerControl.c
int timerStatusAll (void);
int cmdTimerStatusAll (void *str);
int cmdTimerStatusAllHelp (void *str);
int cmdTimerSelect (void *str);
int cmdTimerSelectHelp (void *str);
int cmdTimerDuration (void *str);
int cmdTimerDurationHelp (void *str);
int cmdTimerDelay (void *str);
int cmdTimerDelayHelp (void *str);
int cmdTimerReset (void *str);
int cmdTimerResetHelp (void *str);
int cmdTimerValue (void *str);
int cmdTimerValueHelp (void *str);
int cmdTimerStatus (void *str);
int cmdTimerStatusHelp (void *str);
int cmdTimerSource (void *str);
int cmdTimerSourceHelp (void *str);
int cmdTimerActivation (void *str);
int cmdTimerActivationHelp (void *str);

// cmdUart.c
int cmdDiagUart (void *str);
int cmdBaudRate (void *str);
int cmdBaudRateHelp (void *str);

// cmdUpdate.c
int cmdFirmUpdateAll (void *str);
int cmdFirmUpdateAllHelp (void *str);
int cmdFirmXmlUpdate (void *str);
int cmdFirmXmlUpdateHelp (void *str);
int cmdFirmXmlDownload (void *str);
int cmdFirmXmlDownloadHelp (void *str);
int cmdFirmDownloadAll (void *str);
int cmdFirmDownloadAllHelp (void *str);
int cmdFirmUpdateAllAdmin (void *str);
int cmdFirmUpdateAllAdminHelp (void *str);
int cmdPhyUpdate (void *str);
int cmdPhyUpdateHelp (void *str);
int cmdPhyDataCopy (void *str);
int cmdPhyDataCopyHelp (void *str);
int cmdFpgaUpdateInterface (void *str);
int cmdFpgaUpdateInterfaceAdmin (void *str);
int cmdFpgaUpdateInterfaceHelp (void *str);
int cmdFpgaUpdateInterfaceHelpAdmin (void *str);
int cmdFpgaDownloadInterface (void *str);
int cmdFpgaDownloadInterfaceHelp (void *str);
int cmdFpgaCopyInterface (void *str);
int cmdFpgaCopyInterfaceHelp (void *str);
int cmdAcesPolarFireIFFlashMain (void *str);
int cmdDumpPolarFireIFFlashMain (void *str);
int cmdAcesDumpPFFlashMain (int mode);
int cmdAcesPFFlash (unsigned long *adrs, unsigned int *acesSize);
int cmdDumpPFFlash (unsigned long *adrs, unsigned  int *acesSize);
int cmdBootHeader (void *str);
int cmdBootHeaderHelp (void *str);

// cmdVersion.c
int cmdBoardVersion (void *str);
int cmdBoardVersionHelp (void *str);
int cmdSetBoardVersion (void *str);
int cmdSetBoardVersionHelp (void *str);
int cmdFpgaVersion (void *str);
int cmdFpgaVersionHelp (void *str);
int cmdFirmVersion (void *str);
int cmdFirmVersionHelp (void *str);
int cmdFirmware (void *str);
int cmdFirmwareHelp (void *str);
int cmdMainVersion (void *str);
int cmdMainVersionHelp (void *str);
int cmdFirmVersion (void *str);
int cmdFirmVersionHelp (void *str);
int cmdIfFpgaVersion (void *str);
int cmdIfFpgaVersionHelp (void *str);
int cmdBootVersion (void *str);
int cmdBootVersionHelp (void *str);

// cmdVoltIfBoard.c
#if defined (MODE_BOARD_ACB531CXP) || defined (MODE_BOARD_ACB532GE)

int cmdVoltIfShow (void *str);
int cmdVoltIfShowHelp (void *str);
int cmdVoltIfTemp (void *str);
int cmdVoltIfTempHelp (void *str);

// ACB-531-CXP
int cmdVoltIf105vd (void *str);
int cmdVoltIf105vdHelp (void *str);
int cmdVoltIf105va (void *str);
int cmdVoltIf105vaHelp (void *str);
int cmdVoltIf125vd (void *str);
int cmdVoltIf125vdHelp (void *str);
int cmdVoltIf18vd (void *str);
int cmdVoltIf18vdHelp (void *str);
int cmdVoltIf25vf (void *str);
int cmdVoltIf25vfHelp (void *str);
int cmdVoltIf33vd (void *str);
int cmdVoltIf33vdHelp (void *str);
int cmdVoltIf90va (void *str);
int cmdVoltIf90vaHelp (void *str);

// ACB-532-GE
int cmdVoltIf065v (void *str);
int cmdVoltIf065vHelp (void *str);
int cmdVoltIf085va (void *str);
int cmdVoltIf085vaHelp (void *str);
int cmdVoltIf230va (void *str);
int cmdVoltIf230vaHelp (void *str);
int cmdVoltIf330v (void *str);
int cmdVoltIf330vHelp (void *str);
int cmdVoltIf120vp (void *str);
int cmdVoltIf120vpHelp (void *str);
int cmdVoltIf240v (void *str);
int cmdVoltIf240vHelp (void *str);

#endif

// cmdVoltFpgaBoard.c
int cmdVoltFpgaShow(void *str);
int cmdVoltFpgaShowHelp (void *str);
int cmdVoltFpgaVccint (void *str);
int cmdVoltFpgaVccintHelp (void *str);
int cmdVoltFpgaVccaux (void *str);
int cmdVoltFpgaVccauxHelp (void *str);
int cmdVoltFpgaVbram (void *str);
int cmdVoltFpgaVbramHelp (void *str);
int cmdVoltFpgaVccpint (void *str);
int cmdVoltFpgaVccpintHelp (void *str);
int cmdVoltFpgaVccpaux (void *str);
int cmdVoltFpgaVccpauxHelp (void *str);
int cmdVoltFpgaVccoDdr (void *str);
int cmdVoltFpgaVccoDdrHelp (void *str);
int cmdVoltFpga33vd (void *str);
int cmdVoltFpga33vdHelp (void *str);
int cmdVoltFpga25v (void *str);
int cmdVoltFpga25vHelp (void *str);
int cmdVoltFpgaVtt (void *str);
int cmdVoltFpgaVttHelp (void *str);

#if defined (MODE_SYSTEM_MANAGEMENT)
int cmdVoltFpga09v (void *str);
int cmdVoltFpga09vHelp (void *str);
int cmdVoltFpga12vSys (void *str);
int cmdVoltFpga12vSysHelp (void *str);
int cmdVoltFpga115v (void *str);
int cmdVoltFpga115vHelp (void *str);
int cmdVoltFpga12v (void *str);
int cmdVoltFpga12vHelp (void *str);
int cmdVoltFpga33v (void *str);
int cmdVoltFpga33vHelp (void *str);
int cmdVoltFpga33va (void *str);
int cmdVoltFpga33vaHelp (void *str);
int cmdVoltFpga50v (void *str);
int cmdVoltFpga50vHelp (void *str);
int cmdVoltFpgaVRef (void *str);
int cmdVoltFpgaVRefHelp (void *str);
int cmdVoltFpgaVccPsintpl (void *str);
int cmdVoltFpgaVccPsintplHelp (void *str);
int cmdVoltFpgaVccPsinfp (void *str);
int cmdVoltFpgaVccPsinfpHelp (void *str);
int cmdVoltFpgaVccPsaux (void *str);
int cmdVoltFpgaVccPsauxHelp (void *str);
#endif

#endif  // __CMD_H__

// eof
