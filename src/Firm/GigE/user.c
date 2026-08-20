//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// user.c - user Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

#include "../Common/common.h"
#include "gige.h"
#include "gev.h"
#include "user.h"
#include "flash.h"
#include "framebuf.h"

#if defined (MODE_GIGE_10G)
#include "phy.h"
#endif

#if defined (IF_CXP)
#include "../CXP/cxp.h"
#endif


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
#if defined (MODE_CAMERA_EVENT_VERSION2)
volatile u32 *Event_Notification = (u32 *)CAMERA_EVENT;
#else
volatile u32 Event_Notification[EventNotification_Max];
#endif


//----------------------------------------------------------------------------------
// Function Define
//----------------------------------------------------------------------------------
#if defined (IF_GIGE) && defined (MODE_CAMERA_INTERRUPT)
static void user_event_callback(u32 event_mask); // Add: an interruption of critical events [Masahide: 8th May 2019]
#endif


// ---- Global variables -------------------------------------------------------
volatile u32 video_max_width = 0;
volatile u32 video_max_height = 0;
volatile u32 video_total_bpf = 0;
volatile u32 video_acq_mode = 0;
volatile u32 video_max_offset = 0;
volatile u32 video_max_gap = 0;
volatile u8 bpp = 1;
volatile u8 bit = 8;


#if defined (IF_GIGE) && defined (MODE_GIGE_10G)

volatile u32 video_chunk_enable   = 0;  // Chunk enable register, bit 0 = frame counter enable
volatile u64 event_test_timestamp = 0;
sfnc_ptp_t sfnc_ptp = {.clockAccuracy      = 0xFE,
                       .status             = 1,
                       .servoStatus        = 0,
                       .offsetFromMaster   = 0,
                       .meanPathDelay      = 0,
                       .clockId            = 0,
                       .parentClockId      = 0,
                       .grandmasterClockId = 0};

#endif // #if defined (IF_GIGE) && defined (MODE_GIGE_10G)

// ---- Global variables for XML URL --------------------------------------------
//char XML_URL_FILE;
unsigned int xmlStartAddress = 0;
unsigned int xmlSize = 0;
unsigned char xmlURL[] = { 0 };
unsigned int xmlStartAddressSecond = 0;
unsigned int xmlSizeSecond = 0;
unsigned char xmlURLSecond[] = { 0 };

// ---- Global variables for status  --------------------------------------------
volatile u32 DeviceStatus = 0;
volatile u32 ccpPrivilege = 0;

// ---- Global variables for LED indicators  -------------------------------------------
volatile u32  DeviceIndicatorPower = 0;
volatile u32  DeviceIndicatorStatus = 0;

// ---- Global variables for Acquisition  --------------------------------------------
volatile u32 Acquisition_Preset = 0;

// ---- Global variables for File Access Control --------------------------------
unsigned int fileSelector = 0; 				// RW 4 bytes : IEnumeration UserSetDefault,UserSet1,UserSet2,LUTLuminance,
unsigned int fileSel[FileSelector_MAX];
unsigned int fileExec[FileSelector_MAX];
unsigned int fileMode[FileSelector_MAX];
unsigned int* fileBuffer[FileSelector_MAX];
unsigned int fileOffset[FileSelector_MAX][FileOperationeSelector_MAX];
unsigned int fileLength[FileSelector_MAX][FileOperationeSelector_MAX];
unsigned int fileStatus[FileSelector_MAX][FileOperationeSelector_MAX];
unsigned int fileResult[FileSelector_MAX][FileOperationeSelector_MAX];
unsigned int fileSize[FileSelector_MAX];

// --  Global Flash variables -------------
volatile u32 ExposureTime_Auto = 0;

// --  Global Flash variables -------------
volatile u32 Flash_UserSetSelector = 0;
volatile u32 Flash_UserSetLoad = 0;
volatile u32 Flash_UserSetSave = 0;
volatile u32 Flash_UserSetDefault = 0;

// --  Global Counter variables -------------
volatile u32 Counter_CounterSelector = 0;
volatile u32 CounterDiagnostic_Selector = 0;

// --  Global LUTControl variables -------------
volatile u32 LUTSelector_Selector = 0;
volatile u32 LUTIndex_Index = 0;
volatile u32 LUTFormat_Selector = 0;

// --  Global EventControl variables -------------
volatile u32 Event_Selector = DEFAULT_EVENT_SELECTOR;
volatile u32 Event_Timestamp[EventNotification_Max];
volatile u32 Event_FrameID[EventNotification_Max];

// --  Global DefectivePixeiControl variables -------------
volatile u32 DefectPixelCorrection_Selector = 0;
volatile u32 DefectivePixel_Modify = 0;
volatile u32 DefectPixel_Index = 0;
volatile u32 DefectPixelCoordinate_X = 0;
volatile u32 DefectPixelCoordinate_Y = 0;
volatile u32 DefectPixelCorrection_AdjustFfcSelector = 0;
volatile u32 DefectPixelCorrection_AdjustBrightSelector = 0;
volatile float DefectPixelCorrection_NonUniform = 0;
volatile float DefectPixelCorrection_Sd = 0;

// --  Global FlatFiledCorrectionControl variables -------------
volatile u32 FlatFieldCorrection_Selector = 0;
volatile u32 FlatFieldCorrectionAdjustment_Selector = 0;
volatile u32 FlatFieldCorrection_Status = 0;
volatile u32 FlatFieldCorrectionBrightAdjustment_Lumi = 14253; // Better
volatile u32 FlatFieldCorrectionModeSelector = 1;

volatile u32 FlatFieldCorrectionX_Index = 0;
volatile u32 FlatFieldCorrectionY_Index = 0;

// --  Global SpectrumControl variables -------------
volatile u32 SpectrumBand_Index = 0;
volatile u32 SpectrumBand_Index_Offset = 0;
volatile u32 SpectrumBandGainOffset_Index = 0;

#if defined (MODE_SPECTRUM_BAND_SELECT)
volatile float SpectrumBand_Wave = SPECTRUM_BAND_WAVE_MIN;
#endif

extern double *gpSpectrumBandInfo;

// --  Global Device Temperature variables -------------
volatile u32 DeviceTemperature_Selector = 0;

// ---- Global variables for Device Control ------------------------------------
volatile u32 DeviceIndicatorMode_Selecotr = 0;

// --  Global Device for  Built-in test variables -------------
volatile u32 DeviceBuiltInTest_Mode = 0;
volatile u32 DeviceBuiltInTest_Status = 0;

// ---- Global variables for framebuf.c --------------------------------
volatile void* framebuffer = 0;

// --  Global Device for Diagnostic variables -------------
volatile u32 Diagnostic_Selector = 0;

// ---- Global Debug for variables  -------------------------
unsigned int gDebugTime, gDebugTimeSave, gDebugStartTime, gDebugEndTime;

// ---- Global ROI for variables  -------------------------
int roiHeightSizeGe[ROI_SELECTOR_COUNT];
int roiHeightOffsetGe[ROI_SELECTOR_COUNT];
int roiHeightValidGe[ROI_SELECTOR_COUNT];

// ---- Global Binning for variables  -------------------------
int aoiBinningXSize = 0;
int aoiBinningYSize = 0;
int aoiBinningXOffset = 0;
int aoiBinningYOffset = 0;
int aoiBinningXFlag = 0;
int aoiBinningYFlag = 0;

// ---- Global DeviceCtrl for variables  -------------------------
int gDeviceIfUpdateSelector = 0;	// 0=User Area/1=Factory Area

// ---- Global High Speed Mode for variables  -------------------------
#if defined (MODE_FRAMERATE_HIGH_SPEED)
int gHighSpeedMode = MODE_DISABLE;
int gHighSpeedModeLineCount = 0;
#endif

// ---- Global DRRS for variables  -------------------------
#if defined (MODE_SENSOR_DRRS)
int gDrrsMode = MODE_DISABLE;
#endif

// ---- Global PF update for variables  -------------------------
#if defined (MODE_FPGA_PF)
unsigned int gIfFpgaFlashAdrs = 0;
int gIfFpgaReConfig = 0;
#endif

//----------------------------------------------------------------------------------
// Externs
//----------------------------------------------------------------------------------
#if defined (IF_CXP)
extern unsigned char gXmlFileNameUpdate [];
#endif

#if defined (MODE_GIGE_10G)
extern volatile uint16_t framebuf_pad_x;	// Line padding
extern volatile uint16_t framebuf_pad_y;	// Block (frame) padding
extern volatile uint64_t framebuf_bpb;		// Total bytes per block
#endif // #if defined (MODE_GIGE_10G)

extern char firmUpdate[];					// For ARM1 Version string
extern char deviceFirmwareVersion[];		// For ALL instances
extern char deviceManufacturerInfo[];		// For ALL instances
extern char deviceVendorName[];				// For ALL instances
extern char deviceModelName[];				// For ALL instances
extern char deviceUserID[];					// For DeviceUserID
extern char deviceVersion[];				// For DeviceVersion



// ---- Read GigE Vision user-space bootstrap register -------------------------
//
//           This function must be always implemented!
//           It is called by the gige_callback() from libgige
//
// address = address of the register within GigE Vision manufacturer-specific
//           register address space (0x0000A000 - 0xFFFFFFFF)
// status  = return status value, should be one of following:
//           GEV_STATUS_SUCCESS         - read passed correctly
//           GEV_STATUS_INVALID_ADDRESS - register at 'address' does not exist
//           GEV_STATUS_LOCAL_PROBLEM   - problem while getting register value
//           GEV_STATUS_ERROR           - unspecified error
//
// return value = content of the register at 'address'
//
u32 get_user_reg (u32 address, u16 *status)
{
	*status = GEV_STATUS_SUCCESS;
	u32 value = 0, value2nd = 0, value3rd = 0, address2nd = 0, address3rd = 0;
	double dblValue = 0.0, dblValue2nd = 0.0;
	float fltValue = 0.0;
	u32* valueBuffer = 0;
	int intValue;

	switch (address)
	{
		//====================================================================================
		//
		// Group: Image Format Control registers of standard
		// Note:
		//
		//====================================================================================

		//----------------------------------------------------------------------------------
		// Sensor Width取得
		//----------------------------------------------------------------------------------
		case SensorWidth:
			value = sensorWidth();
			break;

		//----------------------------------------------------------------------------------
		// Sensor Height取得
		//----------------------------------------------------------------------------------
		case SensorHeight:
			value = sensorHeight();
			break;

		//----------------------------------------------------------------------------------
		// Camera Width Max取得
		//----------------------------------------------------------------------------------
		case CameraWidthMax:
			if ((*status = toG (roiGetAreaSize ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				value2nd = 0;

			if (value2nd == 1)
			{
				value = sensorWidth ();
			}
			else
			{
				value = WidthMax ();

				#if defined(MODE_BINNING)
				if ((*status = toG(aoiGetBinningX ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
					value2nd = 0;

				if(value2nd != MODE_DISABLE)
				value = WidthMax() / value2nd;

				#endif
			}
		
			break;


		//----------------------------------------------------------------------------------
		// Camera Height Max取得
		//----------------------------------------------------------------------------------
		case CameraHeightMax:
			if ((*status = toG (roiGetAreaSize ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				value2nd = 0;

			if (value2nd == 1)
			{
				value = sensorHeight ();
			}
			else
			{
				value = HeightMax ();

				// High Speed Mode
				#if defined (MODE_FRAMERATE_HIGH_SPEED)
				if ((*status = toG (sensorGetFrameRateHighSpeedMode ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
					break;

				if (value3rd == MODE_ENABLE)
				{
					if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
						value = FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_GE;
					else
						value = FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_OTHER;
					break;
				}
				#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)


				#if defined(MODE_BINNING)
				if ((*status = toG(aoiGetBinningY ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
					value2nd = 0;
	
				if(value2nd != MODE_DISABLE)
					value = HeightMax() / value2nd;
				#endif
			}

			break;

		//----------------------------------------------------------------------------------
		// Width取得
		//----------------------------------------------------------------------------------
		case FPGA_AOI_XSIZE_ADRS:
			*status = toG (aoiGetWidth ((int*)&value));
			video_width = value;
			break;

		//----------------------------------------------------------------------------------
		// Height取得
		//----------------------------------------------------------------------------------
		case FPGA_AOI_YSIZE_ADRS:

			// High Speed Mode
			#if defined (MODE_FRAMERATE_HIGH_SPEED)
			if ((*status = toG (sensorGetFrameRateHighSpeedMode ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			if (value3rd == MODE_ENABLE)
			{
				// Get Virtual Height
				if ((*status = toG (sensorGetVirtualHeight ((int *)&value))) != AVAL_STATUS_SUCCESS)
					break;

				// Set Video Height
				video_height = value;
				break;
			}
			#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)

			// ROIモード取得
			if ((*status = toG (roiCheckMultiMode ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// ROIマルチモード有効ならばTotal Sizeを取得
			if (value2nd == MODE_ENABLE)
			{
				// Total Height Size取得
				if ((*status = toG (fpgaRoiGetCameraHeightTotalSize ((int *)&value))) != AVAL_STATUS_SUCCESS)
					break;;

				video_height = value;
			}
			else
			{
				// Height取得
				*status = toG(aoiGetHeight((int*) &value));

				video_height = value;

				#if defined(MODE_BINNING)
				aoiBinningYSize = value;
				#endif
			}

			break;

		//----------------------------------------------------------------------------------
		// Width Offset取得
		//----------------------------------------------------------------------------------
		case FPGA_AOI_XOFFSET_ADRS:
			*status = toG(aoiGetWidthOffset((int*) &value));
			video_offs_x = value;

			#if defined(MODE_BINNING)
			aoiBinningXOffset = value;
			#endif

			break;

		//----------------------------------------------------------------------------------
		// Height Offset取得
		//----------------------------------------------------------------------------------
		case FPGA_AOI_YOFFSET_ADRS:

			// ROIモード取得
			if ((*status = toG (roiCheckMultiMode ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// ROIマルチモード有効ならばOffsetyは0
			if (value2nd == MODE_ENABLE)
			{
				value = 0;
				video_offs_y = 0;
			}
			else
			{
				// Offset取得
				*status = toG(aoiGetHeightOffset((int*) &value));
				video_offs_y = value;

				#if defined(MODE_BINNING)
				aoiBinningYOffset = value;
				#endif
			}

			break;

		//----------------------------------------------------------------------------------
		// ReverseX取得
		//----------------------------------------------------------------------------------
		case FPGA_XFLIP_CTRL_ADRS:
		case FPGA_XFLIP_CTRL_ADRS_FPGA:
			*status = toG(aoiGetXflip((int*) &value));

			#if defined (MODE_XFLIP_INVERT)
			if (*status == AVAL_STATUS_SUCCESS)
			{
				if (value == XFLIP_ENABLE)
					value = XFLIP_DISABLE;
				else
					value = XFLIP_ENABLE;
			}
			#endif

			break;

		//----------------------------------------------------------------------------------
		// ROI Selector取得
		//----------------------------------------------------------------------------------
		case FIRM_DATA_ROI_SELECTOR_ADRS:
			*status = toG (roiGetSelector ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// ROI Height Size取得
		//----------------------------------------------------------------------------------
		case RoiHeightSize:

			// Get Selector
			if ((*status = toG (roiGetSelector ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Valid
			if ((*status = toG (roiGetValid ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Height
			if (value3rd == 0)
				value = roiHeightSizeGe[value2nd];
			else
				*status = toG (aoiGetHeight ((int *)&value));

			break;
		
		//----------------------------------------------------------------------------------
		// ROI Height Offset取得
		//----------------------------------------------------------------------------------
		case RoiHeightOffset:

			// Get Selector
			if ((*status = toG (roiGetSelector ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Valid
			if ((*status = toG (roiGetValid ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Height
			if (value3rd == 0)
				value = roiHeightOffsetGe[value2nd];
			else
				*status = toG (aoiGetHeightOffset ((int *)&value));


			break;

		//----------------------------------------------------------------------------------
		// ROI Height Valid
		//----------------------------------------------------------------------------------
		case RoiHeightValid:
			*status = toG (roiGetValid ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// ROI Area Flag
		//----------------------------------------------------------------------------------
		case FIRM_DATA_ROI_AREA_MODE_ADRS:
			*status = toG (roiGetAreaFlag ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// ROI Area Size取得
		//----------------------------------------------------------------------------------
		case FIRM_DATA_ROI_AREA_SIZE_ADRS:
			*status = toG (roiGetAreaSize ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// SensorTotalHeight取得
		//----------------------------------------------------------------------------------
		case SensorTotalHeight:
			*status = toG (fpgaRoiGetSensorHeightTotalSize ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// RoiHeightDefault取得
		//----------------------------------------------------------------------------------
		case RoiHeightDefault:
			#if defined(AXP_AHS052VIR_01)
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_SPECTRUM_DEFAULTY,	(u32*) &value);
			*status = toG (cmdExecuteStatus (&value));
			#else
			value = IN32 (FIRM_DATA_ROI_DEFAULT_ADRS);
			#endif
			break;

#if defined (IF_GIGE)
		//----------------------------------------------------------------------------------
		// Padding X取得
		//----------------------------------------------------------------------------------
		case FPGA_AOI_XPAD_ADRS:
			*status = toG (aoiGetPad ((int*)&value, (int*)&value2nd));
			break;

		//----------------------------------------------------------------------------------
		// Padding Y取得
		//----------------------------------------------------------------------------------
		case FPGA_AOI_YPAD_ADRS:
			*status = toG (aoiGetPad ((int*)&value2nd, (int*)&value));
			break;
#endif // defined (IF_GIGE)

#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
		//--------------------------------------------------------------------------------
		// VideoChunkCtrl取得
		//--------------------------------------------------------------------------------
		case VideoChunkCtrl:
			value = video_chunk_ctrl;
			break;

		//--------------------------------------------------------------------------------
		// VideoChunkEnable取得
		//--------------------------------------------------------------------------------
		case VideoChunkEnable:
			value = video_chunk_enable;
			break;
#endif // #if defined (MODE_GIGE_10G) && defined (IF_GIGE)

		//----------------------------------------------------------------------------------
		// LineDelay取得
		//----------------------------------------------------------------------------------
		case LineDelay:
			value = 0;
			break;

		//----------------------------------------------------------------------------------
		// FrameDelay取得
		//----------------------------------------------------------------------------------
		case FrameDelay:
			value = video_gap_y;
			break;

		//----------------------------------------------------------------------------------
		// LinePitch取得
		//----------------------------------------------------------------------------------
#if !defined (MODE_GIGE_10G)
			case LinePitch:
			value = video_padding;
			break;
#endif

		//----------------------------------------------------------------------------------
		// PixelFormat取得
		//----------------------------------------------------------------------------------
		case FPGA_AOI_BITWIDTH_ADRS:
			if ((*status = toG (cameraGetBitWidthGigE ((int *)&value))) == AVAL_STATUS_SUCCESS)
			{
				video_pixfmt = value;
			}
			else
			{
				*status = GEV_STATUS_INVALID_PARAMETER;
				value = video_pixfmt = GVSP_PIX_MONO8;
			}

			break;


		//====================================================================================
		//
		// Group: Acquisition Control registers of standard
		// Note:
		//
		//====================================================================================

		//----------------------------------------------------------------------------------
		// Acquisition Start取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_START_ADRS:
			value = (IN32(GENICAM_ACQUISITION_START_ADRS) & GENICAM_ACQUISITION_START_BIT) ? 1 : 0;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Abort取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_STOP_ADRS:
		case GENICAM_ACQUISITION_ABORT_ADRS:
			value = (IN32(GENICAM_ACQUISITION_ABORT_ADRS) & GENICAM_ACQUISITION_ABORT_BIT) ? 1 : 0;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Reset取得
		//----------------------------------------------------------------------------------
		case AcquisitionPreset:
			value = Acquisition_Preset;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Mode取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_MODE_ADRS:
			*status = toG (acquisitionGetMode((int*)&value));
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Frame Count取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_FRAME_COUNT_ADRS:
			value = IN32(GENICAM_ACQUISITION_FRAME_COUNT_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Frame Rate取得(Rateバージョン)
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_FRAMERATE_ADRS:	//@@@1
		case AcquisitionFrameRate:
	    	*status = toG( acquisitionGetFrameRate(&dblValue) );
			value = (unsigned int)(dblValue * 100);
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Frame Rate最大値取得
		//----------------------------------------------------------------------------------
		case AcquisitionFrameRateMax:
			*status = toG (rateMax ((double*)&dblValue));
			dblValue *= 100;
			value = (u32)dblValue;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Frame Rate最小値取得
		//----------------------------------------------------------------------------------
		case AcquisitionFrameRateMin:
			*status = toG (rateMin ((double*)&dblValue));
			dblValue *= 100;
			value = (u32)dblValue;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Frame Rate Raw(Time)最大値取得
		//----------------------------------------------------------------------------------
		case AcquisitionFrameRateRawMax:
			// Rate取得
			if ((*status = toG (rateMax (&dblValue))) != AVAL_STATUS_SUCCESS)
				break;

			// Rate時間取得
			value =  (1.0 / dblValue) * _1US;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Frame Rate Raw(Time)最小値取得
		//----------------------------------------------------------------------------------
		case AcquisitionFrameRateRawMin:
			// Rate取得
			if ((*status = toG (rateMin (&dblValue))) != AVAL_STATUS_SUCCESS)
				break;

			// Rate時間取得
			value =  (1.0 / dblValue) * _1US;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Frame Count最大値取得
		//----------------------------------------------------------------------------------
		case AcquisitionFrameCountMax:
			value = ULONG_MAX;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Frame Count最小値取得
		//----------------------------------------------------------------------------------
		case AcquisitionFrameCountMin:
			value = 0;
			break;

		//----------------------------------------------------------------------------------
		// 露光時間(us単位)取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_EXPOSURE_ADRS:
			*status = toG (acquisitionGetExposure ((unsigned int*) &value));
			break;

		//----------------------------------------------------------------------------------
		// 露光時間最大値(us単位)取得
		//----------------------------------------------------------------------------------
		case ExposureTimeMax:
			*status =  toG (exposureMax ((int*)&value));
			break;

		//----------------------------------------------------------------------------------
		// 露光時間最小値(us単位)取得
		//----------------------------------------------------------------------------------
		case ExposureTimeMin:
			*status =  toG (exposureMin ((int*)&value));
			break;

		//----------------------------------------------------------------------------------
		// ExposureTime 露光時間(us単位)取得
		//----------------------------------------------------------------------------------
		case ExposureAuto:
			value = ExposureTime_Auto;
			break;

		//----------------------------------------------------------------------------------
		// ExposureMode取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_EXPOSURE_MODE_ADRS:
			*status = toG (acquisitionGetExposureMode ((int*) &value));
			break;

		//----------------------------------------------------------------------------------
		// Sensor Read Out時間取得(us単位)取得
		//----------------------------------------------------------------------------------
		case SensorReadOutTime:
			*status =  toG (sensoreGetReadOut ((int*)&value));
			break;

		//----------------------------------------------------------------------------------
		// Test Pattern取得
		//----------------------------------------------------------------------------------
		case FPGA_AOI_TP_INDEX_ADRS:
			*status = toG(aoiGetPattern ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Status Select取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_STATUS_SELECT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Status取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_STATUS_ADRS:
			value = IN32 (address) & GENICAM_ACQUISITION_STATUS_SELECT_MASK;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Trigger Select取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_TRG_SELECT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Trigger Mode取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_TRG_MODE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Software Trigger取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_SOFT_TRG_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Trigger Source取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_TRG_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Trigger Activation取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_TRG_ACTIVATION_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Trigger Delay取得
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_TRG_DELAY_ADRS:
			*status = toG(acquisitionGetTrgDelay((unsigned int*) &value));
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Trigger Delay Min取得
		//----------------------------------------------------------------------------------
		case TriggerDelayMin:
			value = GENICAM_ACQUISITION_TRG_DELAY_MIN;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Trigger Delay Max取得
		//----------------------------------------------------------------------------------
		case TriggerDelayMax:
			value = GENICAM_ACQUISITION_TRG_DELAY_MAX;
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Frame Invalided Active Trg Count取得
		//----------------------------------------------------------------------------------
        case GENICAM_ACQUISITION_FRAME_INVALID_ACTIVE_TRG_CNT_ADRS:
         	value = IN32(address);
           	break;

		//----------------------------------------------------------------------------------
   		// Acquisition Trg Reserved取得
		//----------------------------------------------------------------------------------
        case GENICAM_ACQUISITION_TRG_RESERVE_MODE_ADRS:
         	value = IN32(address);
           	break;

        	break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgHighCount取得取得
		//----------------------------------------------------------------------------------
        case AcquisitionTrgHighCount:
			*status = toG (acquisitionGetTrgCountHigh (0, (unsigned int *)&value));
	      	break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgLowCount取得取得
		//----------------------------------------------------------------------------------
        case AcquisitionTrgLowCount:
			*status = toG (acquisitionGetTrgCountLow (0, (unsigned int *)&value));
	      	break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgCountMode取得
		//----------------------------------------------------------------------------------
        case AcquisitionTrgCountMode:
			*status = toG (counterGetTrgMode ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgSignalCount取得
		//----------------------------------------------------------------------------------
        case AcquisitionTrgSignalCount:
			*status = toG (counterGetTrgInputCount ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgImageCount取得
		//----------------------------------------------------------------------------------
        case AcquisitionTrgImageCount:
			*status = toG (counterGetImageOutputCount ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Debug Time取得
		//----------------------------------------------------------------------------------
		case DebugTime:
			value = (int)gDebugTimeSave;
			break;


		//====================================================================================
		//
		// Group: Spectrum Control
		// Note:
		//
		//====================================================================================
#if defined (MODE_SPECTRUM)
		//----------------------------------------------------------------------------------
		// Spectrum Output Data Format取得
		//----------------------------------------------------------------------------------
		case FPGA_SPECTRUM_CTRL_ADRS:
           	value = IN32(address);
           	break;

		//----------------------------------------------------------------------------------
		// Spectrum Line Per Frame取得
		//----------------------------------------------------------------------------------
	    case FPGA_SPECTRUM_LINE_PER_FRAME_ADRS:
         	value = IN32(FPGA_SPECTRUM_LINE_PER_FRAME_ADRS);
         	break;

		//----------------------------------------------------------------------------------
		// Spectrum Band Count取得
		//----------------------------------------------------------------------------------
	    case SpectrumBandCount:
         	value = SPECTRUM_BAND_COUNT;
         	break;

		//----------------------------------------------------------------------------------
		// Spectrum Band Index取得
		//----------------------------------------------------------------------------------
	    case SpectrumBandIndex:
        	value = SpectrumBand_Index;
			break;

		//----------------------------------------------------------------------------------
		// Spectrum Band Peak Wave取得
		//----------------------------------------------------------------------------------
	    case SpectrumBandValue:

			// Get Selector
			if ((*status = toG (spectrumGetBandIndex ((SpectrumBand_Index + value3rd), (int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// Band Peak Wave取得
			if((*status = toG (spectrumBandPeakWave2 (value2nd, &dblValue))) != AVAL_STATUS_SUCCESS)
				break;

			value = (u32)(dblValue * SPECTRUM_BAND_WAVE_UNIT);

			break;

		//----------------------------------------------------------------------------------
		// Spectrum Band Peak All取得
		//----------------------------------------------------------------------------------
		case SpectrumBandAllValue:

			// Band Peak Wave取得
			if((*status = toG (spectrumBandPeakWave2 (SpectrumBand_Index, &dblValue))) != AVAL_STATUS_SUCCESS)
				break;

			value = (u32)(dblValue * SPECTRUM_BAND_WAVE_UNIT);

			break;

		//----------------------------------------------------------------------------------
		// Spectrum Band Width取得
		//----------------------------------------------------------------------------------
	    case SpectrumBandWidth:
			*status = toG (spectrumBandHalfWidth2 (SpectrumBand_Index, &dblValue));
			value = (u32)dblValue;
         	break;

		//----------------------------------------------------------------------------------
		// SpectrumBandValueRange取得
		//----------------------------------------------------------------------------------
	    case SpectrumBandValueRange:
	    	value = IN32(FIRM_DATA_SPECTRUM_BANDVALUE_RANGE_ADRS);
	    	break;

		//----------------------------------------------------------------------------------
		// SpectrumBandDefaultMode取得
		//----------------------------------------------------------------------------------
    	case SpectrumBandDefaultMode:
    		*status = toG (spectrumGetDefaultMode ((int *)&value));
    		break;

		//----------------------------------------------------------------------------------
		// Overlap
		//----------------------------------------------------------------------------------
	#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
		case SpectrumOverlap:
			*status = toG (bgfGetOverlapMode((int*) &value));
			break;
	#endif
		
	#if defined (MODE_SPECTRUM_BAND_SELECT)
		//----------------------------------------------------------------------------------
		// Band Wave取得
		//----------------------------------------------------------------------------------
		case SpectrumBandWave:
			value = SpectrumBand_Wave * SPECTRUM_BAND_WAVE_UNIT;
			break;

		//----------------------------------------------------------------------------------
		// Band Wave Index取得
		//----------------------------------------------------------------------------------
		case SpectrumBandWaveToIndex:
			*status = toG (spectrumBandIndex2 ((double)SpectrumBand_Wave, (int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Band Start取得
		//----------------------------------------------------------------------------------
		case SpectrumBandStart:
		
			// Get Selector
			if ((*status = toG (roiGetSelector ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Valid
			if ((*status = toG (roiGetValid ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Height
			if (value3rd == 0)
			{
				intValue = roiHeightSizeGe[value2nd];
				value = roiHeightOffsetGe[value2nd];

				if ((value == 0) && (intValue == 0))
					break;
			}
			else
			{
				*status = toG (aoiGetHeightOffset ((int *)&value));
			}

			#if defined(AXP_AHS052VIR_01) || defined(AXP_AHS052VIR_02)

			// 現在のモードを取得
			if ((*status = toG(aoiGetBinningY ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			value *= value2nd;
			#endif

			// 開始波長取得
			if ((*status = toG (spectrumBandPeakWave2 (value, &dblValue))) != AVAL_STATUS_SUCCESS)
				break;

			// 波長情報を整数に変換
			value = dblValue * SPECTRUM_BAND_WAVE_UNIT;

			break;

		//----------------------------------------------------------------------------------
		// Band End取得
		//----------------------------------------------------------------------------------
		case SpectrumBandEnd:

			// Get Selector
			if ((*status = toG (roiGetSelector ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Valid
			if ((*status = toG (roiGetValid ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Height
			if (value3rd == 0)
			{
				value = roiHeightSizeGe[value2nd];
				intValue = roiHeightOffsetGe[value2nd];

				if (value == 0)
				{
					value = 0;
					break;
				}
			}
			else
			{
				// Height
				if ((*status = toG (aoiGetHeight ((int *)&value))) != AVAL_STATUS_SUCCESS)
					break;

				// Offset
				if ((*status = toG (aoiGetHeightOffset ((int *)&intValue))) != AVAL_STATUS_SUCCESS)
					break;
			}

			#if defined(AXP_AHS052VIR_01) || defined(AXP_AHS052VIR_01)
			// 現在のモードを取得
			if ((*status = toG(aoiGetBinningY ((int *)&value4th))) != AVAL_STATUS_SUCCESS)
				break;

			intValue *= value4th;
			value *= value4th;
			#endif

			value3rd = intValue + value - 1;

			// 終了波長取得
			if ((*status = toG (spectrumBandPeakWave2 (value3rd, &dblValue))) != AVAL_STATUS_SUCCESS)
				break;

			// 波長情報を整数に変換
			value = dblValue * SPECTRUM_BAND_WAVE_UNIT;

			break;

			//----------------------------------------------------------------------------------
			// Spectrum Band Valid取得
			//----------------------------------------------------------------------------------
			case SpectrumBandValid:

				if ((*status = toG (roiGetSelector ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
					break;

				value = roiHeightValidGe[value2nd];
				break;

	#endif // #if defined (MODE_SPECTRUM_BAND_SELECT)

#endif //  #if defined (MODE_SPECTRUM)


		//====================================================================================
		//
		// Group: Device Control registers of standard
		// Note:
		//
		//====================================================================================

		//----------------------------------------------------------------------------------
		// DeviceVendorNameOnEEPROM取得
		//----------------------------------------------------------------------------------
		case DeviceVendorNameOnEEPROM:
		case DeviceVendorNameOnEEPROM + 4:
		case DeviceVendorNameOnEEPROM + 8:
		case DeviceVendorNameOnEEPROM + 12:
		case DeviceVendorNameOnEEPROM + 16:
		case DeviceVendorNameOnEEPROM + 20:
		case DeviceVendorNameOnEEPROM + 24:
		case DeviceVendorNameOnEEPROM + 28:
			memcpy(&value, ((char*) deviceVendorName) + (u8) (address - DeviceVendorNameOnEEPROM), 4);
			value = SWAP_L(value); // with swapped endian
			break;

		//----------------------------------------------------------------------------------
		// DeviceModelNameOnEEPROM取得
		//----------------------------------------------------------------------------------
		case DeviceModelNameOnEEPROM:
		case DeviceModelNameOnEEPROM + 4:
		case DeviceModelNameOnEEPROM + 8:
		case DeviceModelNameOnEEPROM + 12:
		case DeviceModelNameOnEEPROM + 16:
		case DeviceModelNameOnEEPROM + 20:
		case DeviceModelNameOnEEPROM + 24:
		case DeviceModelNameOnEEPROM + 28:
			memcpy(&value, ((char*) deviceModelName) + (u8) (address - DeviceModelNameOnEEPROM), 4);
			value = SWAP_L(value); // with swapped endian
			break;

		//----------------------------------------------------------------------------------
		// DeviceManufacturerInfoOnEEPROM取得
		//----------------------------------------------------------------------------------
		case DeviceManufacturerInfoOnEEPROM:
		case DeviceManufacturerInfoOnEEPROM + 4:
		case DeviceManufacturerInfoOnEEPROM + 8:
		case DeviceManufacturerInfoOnEEPROM + 12:
		case DeviceManufacturerInfoOnEEPROM + 16:
		case DeviceManufacturerInfoOnEEPROM + 20:
		case DeviceManufacturerInfoOnEEPROM + 24:
		case DeviceManufacturerInfoOnEEPROM + 28:
		case DeviceManufacturerInfoOnEEPROM + 32:
		case DeviceManufacturerInfoOnEEPROM + 36:
		case DeviceManufacturerInfoOnEEPROM + 40:
		case DeviceManufacturerInfoOnEEPROM + 44:
			memcpy(&value, ((char*) deviceManufacturerInfo) + (u8) (address - DeviceManufacturerInfoOnEEPROM), 4);
			value = SWAP_L(value); // with swapped endian
			break;

		//----------------------------------------------------------------------------------
		// DeviceVendoroWriteCmd取得
		//----------------------------------------------------------------------------------
		case DeviceVendoroWriteCmd:
			//*status = (u32)isExecuteCommandDone((u32)1,FIRM_CMD_CAMERA_VENDOR,(u32*)&value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// DeviceModelWriteCmd取得
		//----------------------------------------------------------------------------------
		case DeviceModelWriteCmd:
			//*status = (u32)isExecuteCommandDone((u32)1,FIRM_CMD_CAMERA_MODEL,(u32*)&value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// DeviceManufacturerInfoWriteCmd取得
		//----------------------------------------------------------------------------------
		case DeviceManufacturerInfoWriteCmd:
			//*status = (u32)isExecuteCommandDone((u32)1,FIRM_CMD_CAMERA_MANUFACTURE,(u32*)&value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// DeviceVersion取得
		//----------------------------------------------------------------------------------
		case DeviceVersion:
		case DeviceVersion + 4:
		case DeviceVersion + 8:
		case DeviceVersion + 12:
		case DeviceVersion + 16:
		case DeviceVersion + 20:
		case DeviceVersion + 24:
		case DeviceVersion + 28:
			// DeviceVersion FPGAバージョン取得 in 32 bytes
			value = (u32) (((char*) deviceVersion) + (address & ~DeviceVersion));
			break;

		//----------------------------------------------------------------------------------
		// DeviceFirmwareVersion取得
		//----------------------------------------------------------------------------------
		case DeviceFirmwareVersion:
		case DeviceFirmwareVersion + 4:
		case DeviceFirmwareVersion + 8:
		case DeviceFirmwareVersion + 12:
		case DeviceFirmwareVersion + 16:
		case DeviceFirmwareVersion + 20:
		case DeviceFirmwareVersion + 24:
		case DeviceFirmwareVersion + 28:
			memcpy(&value, ((char*) deviceFirmwareVersion) + (u8) (address - DeviceFirmwareVersion), 4);
			value = SWAP_L(value); // with swapped endian
			break;

		//----------------------------------------------------------------------------------
		// DeviceFirmwareTimestamp取得
		//----------------------------------------------------------------------------------
		case DeviceFirmwareTimestamp:
		case DeviceFirmwareTimestamp + 4:
		case DeviceFirmwareTimestamp + 8:
		case DeviceFirmwareTimestamp + 12:
		case DeviceFirmwareTimestamp + 16:
		case DeviceFirmwareTimestamp + 20:
		case DeviceFirmwareTimestamp + 24:
		case DeviceFirmwareTimestamp + 28:
			memcpy(&value, ((char*) firmUpdate) + (u8) (address - DeviceFirmwareTimestamp), 4);
			value = SWAP_L(value); // with swapped endian
			break;

		//----------------------------------------------------------------------------------
		// DeviceUserID取得
		//----------------------------------------------------------------------------------
		case DeviceUserID:
		case DeviceUserID + 4:
		case DeviceUserID + 8:
		case DeviceUserID + 12:
			getUserId(deviceUserID);
			value = (u32) (((char*) deviceUserID) + (address & ~DeviceUserID));
			break;

		//--------------------------------------------------------------------------------
		// Device Sensor ID取得
		//--------------------------------------------------------------------------------
		case DeviceSensorID:
		case DeviceSensorID + 4:
		case DeviceSensorID + 8:
		case DeviceSensorID + 12:
		case DeviceSensorID + 16:
		case DeviceSensorID + 20:
		case DeviceSensorID + 24:
		case DeviceSensorID + 28:
			memcpy (&value, (char *)(FIRM_DATA_SENSORID_ADRS + address - DeviceSensorID), 4);
			value2nd = value;
			value = SWAP_L(value2nd);
			break;

#if defined (MODE_XML_SCHEMA_VERSION)
		//----------------------------------------------------------------------------------
		// DeviceXMLSchmaVersionMode取得
		//----------------------------------------------------------------------------------
		case DeviceXMLSchmaVersionMode:
			value = IN32 (FIRM_DATA_XML_SCHEMA_VERSION_MODE_ADRS);
			break;
#endif

		//----------------------------------------------------------------------------------
		// DeviceIfFpgaSelector取得
		//----------------------------------------------------------------------------------
		case DeviceIfFpgaSelector:
			value = gDeviceIfUpdateSelector;
			break;

 		//====================================================================================
		//
		// Group: TransportLayerControl
		//
		//====================================================================================

		//----------------------------------------------------------------------------------
		// PayloadSize取得
		//----------------------------------------------------------------------------------
		case PayloadSize:

			switch (video_pixfmt)
			{
				case GVSP_PIX_MONO14:
					value = video_width * video_height * 2;
					break;
				case GVSP_PIX_MONO12:
				case GVSP_PIX_MONO12_PACKED:
					value = video_width * video_height * 2;
					break;
				case GVSP_PIX_MONO10:
					value = video_width * video_height * 2;
					break;
				case GVSP_PIX_MONO8:
					value = video_width * video_height * 1;
					break;
				default:
					value = video_width * video_height * 1;
					break;
			}
			break;

#if defined (IF_GIGE) && defined (MODE_GIGE_10G)
		//--------------------------------------------------------------------------------
		// Packet size margin取得
		//--------------------------------------------------------------------------------
		case SCPSPacketSizeMin2:  	//SCPS_MIN
			value = SCPS_MIN + (gige_get_gev_version() == 2 ? 48 : 36);
			break;

		//--------------------------------------------------------------------------------
		// SCPS_MAX設定取得
		//--------------------------------------------------------------------------------
		case SCPSPacketSizeMax2:    //SCPS_MAX
			value = SCPS_MAX + (gige_get_gev_version() == 2 ? 48 : 36);
			break;

		//--------------------------------------------------------------------------------
		// SCPS_INC設定取得
		//--------------------------------------------------------------------------------
		case SCPSPacketSizeInc2:    //SCPS_INC
			value = SCPS_INC;
			break;
		
		//----------------------------------------------------------------------------------
		// GevLinkSpeed取得
		//----------------------------------------------------------------------------------
		case GevLinkSpeed:
			*status = toG (gigeGetSpeedBps ((int *)&value));
			break;
#endif // defined (IF_GIGE) && defined (MODE_GIGE_10G)


#if defined (MODE_GE_SPEED)
		//----------------------------------------------------------------------------------
		// GevLinkSpeedConfig取得
		//----------------------------------------------------------------------------------
		case GevLinkSpeedConfig:
			*status = toG (gevGetSpeedConfig ((unsigned int *)&value));
			break;
#endif // #if defined (MODE_GE_SPEED)


		//====================================================================================
		//
		// Group: UserSetControl
		//
		//====================================================================================

		//----------------------------------------------------------------------------------
		// User Selector取得
		//----------------------------------------------------------------------------------
		case UserSetSelector:
			value = Flash_UserSetSelector;
			break;

		//----------------------------------------------------------------------------------
		// User Load取得
		//----------------------------------------------------------------------------------
		case UserSetLoad:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_USERSET_LOAD, (u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			//@@@1LUTFormat_Selector = LUT_FORMAT_FLASH;
			//@@@1Flash_UserSetLoad = !value;
			break;

		//----------------------------------------------------------------------------------
		// User Save取得
		//----------------------------------------------------------------------------------
		case UserSetSave:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_USERSET_SAVE, (u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			//@@@1Flash_UserSetSave = !value;
			break;

		//----------------------------------------------------------------------------------
		// User Factory取得
		//----------------------------------------------------------------------------------
		case UserSetFactory:
			//*status = (u32) isExecuteCommandDone((u32) 1, 0x15, (u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// User Default取得
		//----------------------------------------------------------------------------------
		case UserSetDefault:
			value = Flash_UserSetDefault;
			break;


		//====================================================================================
		//
		// Group: LUTControl
		//
		//====================================================================================
#if defined (MODE_LUT)
		//----------------------------------------------------------------------------------
		// LUT Selector取得
		//----------------------------------------------------------------------------------
		case LUTSelector:
			value = 0; // as Luminance
			break;

		//----------------------------------------------------------------------------------
		// LUT Index取得
		//----------------------------------------------------------------------------------
		case LUTIndex:
			value = LUTIndex_Index;
			break;

		//----------------------------------------------------------------------------------
		// LUTIndex_Max取得
		//----------------------------------------------------------------------------------
		case LUTIndex_Max:
			value = LUT_DATA_MASK;
			break;

		//----------------------------------------------------------------------------------
		// LUT Control取得
		//----------------------------------------------------------------------------------
		case FPGA_LUT_CTRL_ADRS:
			*status = toG (lutGetEnable( (int*)&value2nd, (int*)&value3rd));
			value = value3rd;
			break;

		//----------------------------------------------------------------------------------
		// LUT Format取得
		//----------------------------------------------------------------------------------
		case LUTFormat:
			*status = toG (lutGetFormat ((int)LUTSelector_Selector, (int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// LUT Threshold取得
		//----------------------------------------------------------------------------------
		case LUTThreshold:

			if (LUTSelector_Selector == 0)
			{
				value = LUT_DEFAULT_BIN_THRESHOLD;
			}
			else
			{
				// Chose the threshold of LUT_FORMAT_BIN LUT format
				if ((*status = toG(lutGetBinThreshold(LUTSelector_Selector , (int*) &value))) != AVAL_STATUS_SUCCESS)
					break;

				// データサイズ取得
				if ((*status = lutGetTableSize ((unsigned int *)&value2nd)) != AVAL_STATUS_SUCCESS)
					break;

				// Check threshold Parameter
				if ((value < THRESHOLD_SIZE_MIN) || (value > value2nd))
					value = value2nd;

				// Get Lut Mode
				if ((*status = toG(lutGetEnable((int*) &value2nd, (int*) &value3rd))) != AVAL_STATUS_SUCCESS)
					break;

				#if defined (IF_CXP)
				if (gInterFaceID == INTERFACE_CXP)
				{
					// Disableの場合は0を返す
					if (value3rd == LUT_DISABLE)
						value = 0;
				}
				#endif
			}

			break;

		//--------------------------------------------------------------------------------
		// LUTGamma取得
		//--------------------------------------------------------------------------------
		case LUTGamma:
			if ((*status = toG( lutGetGamma (LUTSelector_Selector, &fltValue))) == AVAL_STATUS_SUCCESS)
			{
				value = (int)(fltValue * LUT_GAMMA_DATA_ADJUST);
			}
			else
			{
				value = (int)(1.0 * LUT_GAMMA_DATA_ADJUST);
				*status = toG (AVAL_STATUS_SUCCESS);
			}

			break;

		//--------------------------------------------------------------------------------
		// LUTBaseAdrs取得
		//--------------------------------------------------------------------------------
		case LUTBaseAdrs:
			#if !defined (MODE_LUT_NUMBER_FIX)
			if (LUTSelector_Selector == 0)
			{
				value = 0;
				break;
			}
			#endif
		
			if ((*status = toG(lutGetMemAdrs (LUTSelector_Selector, (unsigned int *)&value))) != AVAL_STATUS_SUCCESS)
			{
				if (gInterFaceID == INTERFACE_CXP)
				{
					*status = toG (AVAL_STATUS_SUCCESS);
					value = 0;
				}
			}

			break;
#endif // #if defined (MODE_LUT) 


		//====================================================================================
		//
		// Group: DefectPixelCorrectionControl
		//
		//====================================================================================
#if defined (MODE_DPC)
		//----------------------------------------------------------------------------------
		// Defective Pixel Correction Selector取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionSelector:
			value2nd = 0x0;
			*status = toG (cameraParamUserReadMem (CAMERA_SAVE_USER_NUM, CAMERA_SAVE_DPC_NUMBER_ADRS, (unsigned int*)&value2nd, (unsigned int*)&DefectPixelCorrection_Selector));
			value = DefectPixelCorrection_Selector;
			break;

		 //--------------------------------------------------------------------------------
		 // DefectivePixelCorrectionLoad取得
		 //--------------------------------------------------------------------------------
		 case DefectivePixelCorrectionLoad:
			//*status = (u32)isExecuteCommandDone((u32)1,FIRM_CMD_DPC_LOAD,(u32*)&value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionLoadAdmin取得
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionLoadAdmin:
			//*status = (u32)isExecuteCommandDone((u32)1,FIRM_CMD_DPC_LOAD_ADMIN,(u32*)&value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction Control取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrection:
			*status = toG (dpcGetEnableMode ((int *)&value));
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustmentState取得
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustmentState:
			value = IN32(FIRM_DATA_DPC_COUNT_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustment取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustment:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_DPC2_FACTORY,(u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionDetection取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionDetection:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_DPC3_FACTORY,(u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction Count取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionCount:
			#if defined (MODE_DPC_GRID_UPDATE)

			// 欠陥座標数取得
			*status = toG (dpcGetDefectionCountVersion2 ((int *)&value));

			#else // #if defined (MODE_DPC_GRID_UPDATE)
			
			// Get Area
			if  ((*status = toG (roiGetAreaFlag ((int*)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			if (value2nd == ROI_AREA_MODE_DEFAULT_SIZE)
			{
				if ((*status = toG (dpcEndSearch2 (DPC_MEMORY_ADRS, (int *)&value))) != AVAL_STATUS_SUCCESS)
					break;
			}
			else
			{
				value = IN32(FIRM_DATA_DPC_NUM_ADRS);
			}

			#endif // #if defined (MODE_DPC_GRID_UPDATE)

			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction Count Max取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionMax:
			value = IN32(FIRM_DATA_DPC_MAX_NUM_COUNT_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction変更モード取得
		//----------------------------------------------------------------------------------
		case DefectivePixelModify:
			value = DefectivePixel_Modify;
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction画素インデックス取得
		//----------------------------------------------------------------------------------
		case DefectivePixelIndex:
	        value = DefectPixel_Index;
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction画素x座標取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCoordinateX:
			if( DefectivePixel_Modify == DefectivePixelModify_On)
				value = DefectPixelCoordinate_X;
			else
				//*status = executeCommandDPCEdit(DefectPixel_Index,FIRM_CMD_DPC_GET_GRID,(u32*)&value,(u32*)&value2nd);
				*status = toG (cmdExecuteStatus ((int *)&value));

			break;
		
		//----------------------------------------------------------------------------------
		// Defective Pixel Correction画素y座標取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCoordinateY:
			if( DefectivePixel_Modify == DefectivePixelModify_On)
				value = DefectPixelCoordinate_Y;
			else
				//*status = executeCommandDPCEdit(DefectPixel_Index,FIRM_CMD_DPC_GET_GRID,(u32*)&value2nd,(u32*)&value);
				*status = toG (cmdExecuteStatus ((int *)&value));

			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction画素追加取得
		//----------------------------------------------------------------------------------
		case DefectivePixelApply:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_DPC_ADD_GRID,(u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction画素削除取得
		//----------------------------------------------------------------------------------
		case DefectivePixelRemove:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_DPC_DEL_GRID,(u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction画素全てをクリア取得
		//----------------------------------------------------------------------------------
		case DefectivePixelReset:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_DPC_CLR_GRID,(u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Save工場出荷設定取得
		//----------------------------------------------------------------------------------
		 case DefectivePixelCorrectionSave:
			//*status = (u32)isExecuteCommandDone((u32)1,FIRM_CMD_DPC_SAVE_FACTORY,(u32*)&value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Save工場出荷設定取得
		//----------------------------------------------------------------------------------
		 case DefectivePixelCorrectionSaveAdmin:
			//*status = (u32)isExecuteCommandDone((u32)1,FIRM_CMD_DPC_SAVE_ADMIN,(u32*)&value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction工場出荷設定取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionFactory:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_DPC_DEFAULT,	(u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Defective Pixel Correction補正モード取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionMode:
			value = IN32(FIRM_DATA_DPC_ADJUST_MODE_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustFfcIndex取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustFfcIndex:
			value = DefectPixelCorrection_AdjustFfcSelector;
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustBrightIndex取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustBrightIndex:
			value = DefectPixelCorrection_AdjustBrightSelector;
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustRate取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustRate:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			fltValue = INF ((value2nd + FIRM_DATA_DPC_ADJUST_RATE_ADRS));
			fltValue = (float)(1.0 / (float)fltValue);
			value  = (u32)(fltValue * ACQUISITION_FRAMERATE_UINIT);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustExposure取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustExposure:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			value = IN32 ((value2nd + FIRM_DATA_DPC_ADJUST_EXPOSURE_ADRS));
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustTemperature取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustTemperature:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			fltValue = INF ((value2nd + FIRM_DATA_DPC_ADJUST_TEMP_ADRS));
			intValue = (int)(fltValue * DEVICE_TEMP_UNIT);
			value = (u32)intValue;
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustFfcNumber取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustFfcNumber:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			value = IN32 ((value2nd + FIRM_DATA_DPC_ADJUST_FFC_NUMBER_ADRS));
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustStandardDeviation
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustStandardDeviation:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			value2nd += (FIRM_DATA_DPC_ADJUST_SD0_ADRS + DefectPixelCorrection_AdjustBrightSelector * 4);

			fltValue = INF (value2nd);
			intValue = (int)(fltValue * DEVICE_SD_UNIT);
			value = (u32)intValue;
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustNonuniformity取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustNonuniformity:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			value2nd += (FIRM_DATA_DPC_ADJUST_NON_UNIFORM0_ADRS + DefectPixelCorrection_AdjustBrightSelector * 4);

			fltValue = INF (value2nd);
			intValue = (int)(fltValue * DEVICE_NON_UNIFORM_UNIT);
			value = (u32)intValue;
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustUpdate取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustUpdate:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_DPC_ADJUST_UPDATE, (u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustBaseAdrs取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustBaseAdrs:
			*status = dpcGetMemAdrs (DefectPixelCorrection_Selector, (unsigned int *)&value);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionPixelCount取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionPixelCount:
			*status = dpcGetDefectionCount ((int *)&value);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionPixelStateCount取得
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionPixelStateCount:
			*status = dpcGetBatchCountGlobal ((int *)&value);
			break;

#endif // #if defined (MODE_DPC)


		//====================================================================================
		//
		// Group: FlatFiledCorrectionControl
		//
		//====================================================================================
#if defined (MODE_FFC)
		//----------------------------------------------------------------------------------
		// Flat Filed Correction Selector取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionSelector:
			*status = toG (ffcGetLoadNum ((int*)&FlatFieldCorrection_Selector));
			value = FlatFieldCorrection_Selector;
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction調整セレクタ取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustmentSelector:
			value = FlatFieldCorrectionAdjustment_Selector;
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionSetLoad取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionSetLoad:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_FFC_LOAD, (u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionSetSave取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionSetSave:
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;
#if 0
			switch (FlatFieldCorrectionAdjustment_Selector)
			{
				case 0:
					*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_FFC_SAVE, (u32*) &value);
					break;
				default:
					*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_FFC_SAVE_FACTORY, (u32*) &value);
					break;
			}
			break;
#endif

		//----------------------------------------------------------------------------------
		// Flat Filed Correction取得
		//----------------------------------------------------------------------------------
		case FPGA_FFC_CTRL_ADRS:	//@@@1
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionBrightMode取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionBrightMode:
			if (FlatFieldCorrectionModeSelector == 1)
				*status = toG (ffcGetMode ((int *)&value2nd, (int *)&value));

			#if defined (MODE_FFC_SHADING_LINE)
			else if (FlatFieldCorrectionModeSelector == 4)
				*status = toG (ffcGetMode ((int *)&value2nd, (int *)&value));
			#endif
		
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction Black Target取得
		//----------------------------------------------------------------------------------
		case FPGA_FFC_BLACK_TARGET_ADRS:
			#if defined (MODE_FFC_BIT_CALC)
			if ((*status = toG (ffcGetBlackTarget ((unsigned int *)&value))) != AVAL_STATUS_SUCCESS)
				break;

			if ((*status = toG (ffcGetBitCalc ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			value /= value2nd;

			#else // #if defined (MODE_FFC_BIT_CALC)
		
			value = IN32(address);

			#endif // #if defined (MODE_FFC_BIT_CALC)

			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction White Target取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionBrightTarget:
			#if defined (MODE_FFC_BIT_CALC)
			if ((*status = toG (ffcGetWhiteTarget ((unsigned int *)&value))) != AVAL_STATUS_SUCCESS)
				break;

			if ((*status = toG (ffcGetBitCalc ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			value /= value2nd;

			#else // #if defined (MODE_FFC_BIT_CALC)

			value = IN32(FIRM_DATA_FFC_WHITE_ADRS);
			//value = (value > FFC_WHITE_LEVEL_BIT14_MAX) ? FlatFieldCorrectionBrightAdjustment_Lumi : value;
			if (value > FFC_WHITE_LEVEL_BIT14_MAX)
				value = FlatFieldCorrectionBrightAdjustment_Lumi;
			else
				FlatFieldCorrectionBrightAdjustment_Lumi = value; // FlatFieldCorrectionBrightAdjustment_Lumiを更新

			#endif // #if defined (MODE_FFC_BIT_CALC)
			
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction 黒レベル調整取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionDarkAdjustment:
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;
#if 0
			switch (FlatFieldCorrection_Selector)
			{
				case 0:
					*status = (u32) isExecuteCommandDone((u32) 1,(u32) FIRM_CMD_FFC_BLACK, (u32*) &value);
					break;
				default:
					*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_FFC_BLACK,(u32*) &value);
					break;
			}
			break;
#endif

		//----------------------------------------------------------------------------------
		// Flat Filed Correction 白レベル調整取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionBrightAdjustment:
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;
#if 0
			switch (FlatFieldCorrection_Selector)
			{
				case 0:
					*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_FFC_WHITE,(u32*) &value);
					break;
				default:
					*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_FFC_WHITE,(u32*) &value);
					break;
			}
			break;
#endif

#if defined (MODE_FFC_SHADING_LINE)
		//----------------------------------------------------------------------------------
		// Flat Filed Correction Shading Lineレベル調整取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionShadinLinegAdjustment:
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

#if 0
			switch (FlatFieldCorrection_Selector)
			{
				case 0:
					*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_FFC_SHADING_LINE,(u32*) &value);
					break;
				default:
					*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_FFC_SHADING_LINE,(u32*) &value);
					break;
			}
			break;
#endif

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionWhiteGainX取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionWhiteGainX:
			if ((*status = ffcGetWhiteGainX (&dblValue)) != AVAL_STATUS_SUCCESS)
				break;

			value = dblValue * FFC_WHITE_GAIN_UNIT;
			break;
#endif // #if defined (MODE_FFC_SHADING_LINE)

		//----------------------------------------------------------------------------------
		// Flat Filed Correction工場出荷取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionFactory:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_FFC_DEFAULT,(u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correctionステータス取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionStatus:
			value = FlatFieldCorrection_Status;
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correctionセレクタ取得
		//----------------------------------------------------------------------------------
		 case FlatFieldCorrectionCorrectionSelector:
			value = FlatFieldCorrectionModeSelector;
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correctionモード取得
		//----------------------------------------------------------------------------------
		 case FlatFieldCorrectionSetCorrectionMode:
			//*status = (u32)isExecuteCommandDone((u32)1,FIRM_CMD_FFC_CORRECTION_MODE,(u32*)&value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時の目標値取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustDarkTarget:
			value = IN32 (FIRM_DATA_FFC_ADJUST_BLACK_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時の目標値取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustBrightTarget:
			value = IN32 (FIRM_DATA_FFC_ADJUST_WHITE_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時のレート取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustDarkRate:
			fltValue = INF (FIRM_DATA_FFC_ADJUST_BLACK_RATE_ADRS);
			fltValue = (float)(1.0 / (float)fltValue);
			value  = (u32)(fltValue * ACQUISITION_FRAMERATE_UINIT);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時のレート取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustBrightRate:
			fltValue = INF (FIRM_DATA_FFC_ADJUST_WHITE_RATE_ADRS);
			fltValue = (float)(1.0 / (float)fltValue);
			value  = (u32)(fltValue * ACQUISITION_FRAMERATE_UINIT);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時の露光時間取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustDarkExposure:
			value = IN32 (FIRM_DATA_FFC_ADJUST_BLACK_EXP_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時の露光時間取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustBrightExposure:
			value = IN32 (FIRM_DATA_FFC_ADJUST_WHITE_EXP_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時の温度取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionDarkTemperature:
		case FlatFieldCorrectionAdjustDarkTemperature:
			fltValue = INF (FIRM_DATA_FFC_ADJUST_BLACK_TEMP_ADRS);
			intValue = (int)(fltValue * DEVICE_TEMP_UNIT);
			value = (u32)intValue;
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時の温度取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionBrightTemperature:
		case FlatFieldCorrectionAdjustBrightTemperature:
			fltValue = INF (FIRM_DATA_FFC_ADJUST_WHITE_TEMP_ADRS);
			intValue = (int)(fltValue * DEVICE_TEMP_UNIT);
			value = (u32)intValue;
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時のGain取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustDarkGainX:
			fltValue = INF (FIRM_DATA_FFC_ADJUST_BLACK_GAIN_ADRS);
			intValue = (int)(fltValue * DEVICE_GAIN_UNIT);
			value = (u32)intValue;
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時のGain取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustBrightGainX:
			fltValue = INF (FIRM_DATA_FFC_ADJUST_WHITE_GAIN_ADRS);
			intValue = (int)(fltValue * DEVICE_GAIN_UNIT);
			value = (u32)intValue;
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時のBit取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustDarkBit:
			value = IN32 (FIRM_DATA_FFC_ADJUST_BLACK_BIT_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時のBit取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustBrightBit:
			value = IN32 (FIRM_DATA_FFC_ADJUST_WHITE_BIT_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction調整データの更新取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustUpdate:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_FFC_ADJUST_UPDATE, (u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionBaseAdrs取得
		//----------------------------------------------------------------------------------
		 case FlatFieldCorrectionBaseAdrs:
			*status = ffcGetMemAdrs (FlatFieldCorrection_Selector, FFC_MEMORY_EXT, (unsigned int *)&value);
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionXGrid座標取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionXGrid:
	        value = FlatFieldCorrectionX_Index;
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionY座標取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionYGrid:
	        value = FlatFieldCorrectionY_Index;
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionOffset取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionOffset:
			if ((FlatFieldCorrectionModeSelector == 1) || (FlatFieldCorrectionModeSelector == 4))
			{
				*status = toG (ffcGetOffsetData (0, FlatFieldCorrectionX_Index, FlatFieldCorrectionY_Index, (unsigned int *)&value));
			}
			else if (FlatFieldCorrectionModeSelector == 2)
			{
				value = 0;
			}

			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionGain取得
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionGain:
			if ((FlatFieldCorrectionModeSelector == 1) || (FlatFieldCorrectionModeSelector == 4))
			{
				*status = toG (ffcGetGainData (0, FlatFieldCorrectionX_Index, FlatFieldCorrectionY_Index, (unsigned int *)&value));
			}
			else if (FlatFieldCorrectionModeSelector == 2)
			{
				value = 0;
			}

			break;

#endif // #if defined (MODE_FFC)


		//====================================================================================
		//
		// Group: DigitaolControl as AnalogControl
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// GainX Float取得
		//--------------------------------------------------------------------------------
		case GainxFloat:
			*status = toG (digitalGetGainX (&fltValue));
			fltValue += 0.05;	// 四捨五入（小数点第１位まで有効）
			value = (int)(fltValue * DEVICE_GAINX_UNIT);
			break;

		//--------------------------------------------------------------------------------
		// BlackLevel Offset1取得
		//--------------------------------------------------------------------------------
		case FPGA_DOG_OFFSET1_ADRS:
			*status = toG(digitalGetOffset1 ((int*)&value));
			break;

		//--------------------------------------------------------------------------------
		// BlackLevel Offset2取得
		//--------------------------------------------------------------------------------
		case FPGA_DOG_OFFSET2_ADRS:
			*status = toG(digitalGetOffset ((int*)&value));
			break;

#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
		//--------------------------------------------------------------------------------
		// Sensor Gain取得
		//--------------------------------------------------------------------------------
		case SensorGainX:
			value = IN32(FIRM_DATA_SENSOR_GAIN_ADRS);
			break;
#endif

#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		//--------------------------------------------------------------------------------
		// Sensor Conversion Gain取得
		//--------------------------------------------------------------------------------
		case SensorConversionGain:
			*status = toG (sensorGetConversionGain ((int *)&value));
			break;
#endif

#if defined (MODE_SENSOR_GRADATION_COMPRESS)
		//--------------------------------------------------------------------------------
		// Sensor Gradation Compress 8Bit Convert取得
		//--------------------------------------------------------------------------------
		case SensorGradComp8BitConvert:
			if ((*status = toG (aoiGetBitWidth ((int*) &value2nd))) != AVAL_STATUS_SUCCESS)
				break;
			
			if (value2nd == 8)
				value = sensorGradationCompGetModeDDR ();
			else
				value = 1;

			break;
#endif


		//====================================================================================
		//
		// Group: DigitalIOControl
		//
		//====================================================================================

		//----------------------------------------------------------------------------------
		// LineSelector取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_SELECT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// LineMode取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_MODE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// LineInverter取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_INVERTER_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// LineStatus取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_STATUS_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// LineStatusAll取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_STATUS_ALL_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// LineSource取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_SOURCE_ADRS:
			value = IN32(address);
			value2nd = IN32(GENICAM_DIGITAL_LINE_SELECT_ADRS);

			switch (value2nd)
			{
				case 0: // LineSelector = Line0
				case 1: // LineSelector = Line1
				case 2: // LineSelector = Line2
					// Off value should be 0xFF instead of 0x0 for XML LineSource's value.
					// This value always should be 0xFF as Off because of not supported LineSource with Lin0
					//value = (value==0x0)?0xFF:value;
					value = 0xFF;
					break;
				default:
					break;
			}
			break;

		//----------------------------------------------------------------------------------
		// LineFormat取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_FORMAT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// LinePulseHigh取得
		//----------------------------------------------------------------------------------
		case LinePulseHigh:

			// Selector取得
			value = IN32 (GENICAM_DIGITAL_LINE_SELECT_ADRS);

			// パルス取得
			*status = toG (gpioGetPulseTime((int)value,(unsigned int*)&address2nd,(unsigned int*)&address3rd));

			// パルス格納
			value = address2nd;
			break;

		//----------------------------------------------------------------------------------
		// LinePulseLow取得
		//----------------------------------------------------------------------------------
		case LinePulseLow:

			// Selector取得
			value = IN32 (GENICAM_DIGITAL_LINE_SELECT_ADRS);

			// パルス取得
			*status = toG (gpioGetPulseTime((int)value,(unsigned int*)&address2nd,(unsigned int*)&address3rd));

			// パルス格納
			value = address3rd;
			break;

		//----------------------------------------------------------------------------------
		// GPIO de-buncer mask address取得
		//----------------------------------------------------------------------------------
		case  GENICAM_DIGITAL_LINE_DNF_ADRS:
			value = IN32 (GENICAM_DIGITAL_LINE_DNF_ADRS);
			break;

		//----------------------------------------------------------------------------------
		// GPIO de-buncer0 mask address取得
		//----------------------------------------------------------------------------------
		case FPGA_GPIO_DNF0_ADRS:

			// Get Line Selector
			address2nd = IN32 (GENICAM_DIGITAL_LINE_SELECT_ADRS);

			// DNF取得
			value = IN32 ((GENICAM_DIGITAL_LINE0_DNF_ADRS + (GENICAM_DIGITAL_LINE_INTERVAL * address2nd)));

			break;

		//----------------------------------------------------------------------------------
		// UserOutputSelector取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER_SELECT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// UserOutputValue取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER_VALUE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// UserOutputValueAll取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER_ALL_VALUE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// UserOutputValueAllMask取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER_MASK_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// UserOutput0取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER0_OUTPUT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// UserOutput1取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER1_OUTPUT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// UserOutput2取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER2_OUTPUT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// UserOutput3取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER3_OUTPUT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// UserOutput4取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER4_OUTPUT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// UserOutput5取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER5_OUTPUT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// UserOutput6取得
		//----------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER6_OUTPUT_ADRS:
			value = IN32(address);
			break;


		//====================================================================================
		//
		// Group: Counter Control
		//
		//====================================================================================
#if defined (MODE_GENICAM_COUNTER)
		//----------------------------------------------------------------------------------
		// CounterSelector取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_SELECT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterEventSource取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterEventActivation取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_EVENT_ACTIVE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterResetSource取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterResetActivation取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_ACTIVE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterReset取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterValue取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterValueAtReset取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_VALUE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterResetDuration取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_DURATION_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterResetStatus取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_STATUS_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterTriggerSource取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_TRG_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterTriggerActivation取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_TRG_ACTIVE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterTriggerInvalidSource取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_TRG_INVALID_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterTriggerInvalidActivation取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_TRG_INVALID_ACTIVE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterEventInvalidSource取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_EVENT_INVALID_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterEventInvalidAcitivation取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_EVENT_INVALID_ACTIVE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterResetInvalidSource取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_INVALID_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// CounterResetInvalidActivation取得
		//----------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_INVALID_ACTIVE_ADRS:
			value = IN32(address);
			break;
#endif // #if defined (MODE_GENICAM_COUNTER)


		//====================================================================================
		//
		// Group: Timer Control registers of standard
		// Note:
		//
		//====================================================================================
#if defined (MODE_GENICAM_TIMER)
		//----------------------------------------------------------------------------------
		// TimerSelector取得
		//----------------------------------------------------------------------------------
		case GENICAM_TIMER_SELECT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// TimerDuration取得
		//----------------------------------------------------------------------------------
		case GENICAM_TIMER_DURATION_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// TimerDelay取得
		//----------------------------------------------------------------------------------
		case GENICAM_TIMER_DELAY_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// TimerReset取得
		//----------------------------------------------------------------------------------
		case GENICAM_TIMER_RESET_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// TimerValue取得
		//----------------------------------------------------------------------------------
		case GENICAM_TIMER_CURRENT_COUNT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// TimerStatus取得
		//----------------------------------------------------------------------------------
		case GENICAM_TIMER_STATUS_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// TimerTriggerSource取得
		//----------------------------------------------------------------------------------
		case GENICAM_TIMER_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// TimerTriggerActivation取得
		//----------------------------------------------------------------------------------
		case GENICAM_TIMER_ACTIVE_ADRS:
			value = IN32(address);
			break;
#endif // #if defined (MODE_GENICAM_TIMER)


		//====================================================================================
		//
		// Group: Encoder Control registers of standard
		// Note:
		//
		//====================================================================================
#if defined (MODE_ENCODER)
		//----------------------------------------------------------------------------------
		// EncoderSelector取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_SELECT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderSourceA取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_PASEA_TRG_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderSourceB取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_PASEB_TRG_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderMode取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_MODE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderDivider取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_DVIDER_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderOutputMode取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_OUTPUT_MODE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderStatus取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_STATUS_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderFrequency取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_0_FREQ_ADRS:
			*status = toG (encoderGetFrequency (&dblValue));
			value = (long)dblValue;
			break;

		//----------------------------------------------------------------------------------
		// EncoderTimeout取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_TIMEOUT_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderResetSource取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_RESET_TRG_SOURCE_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderResetActivation取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_RESET_ACTIVATION_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderReset取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_RESET_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderValue at High part in 32-bit取得
		//----------------------------------------------------------------------------------
		case EncoderValue:
			value3rd = IN32(GENICAM_ENCODER_VALUE_LO_ADRS);
			value2nd = IN32(GENICAM_ENCODER_VALUE_HI_ADRS);
			value = value2nd;
			break;

		//----------------------------------------------------------------------------------
		// EncoderValue at Low part in 32-bit取得
		//----------------------------------------------------------------------------------
		case EncoderValueLow:
			value3rd = IN32(GENICAM_ENCODER_VALUE_LO_ADRS);
			value2nd = IN32(GENICAM_ENCODER_VALUE_HI_ADRS);
			value = value3rd;
			break;

		//----------------------------------------------------------------------------------
		// EncoderValue at Low part in 32-bit取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_VALUE_LO_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderValueHigh取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_VALUE_HI_ADRS:
			value3rd = IN32(GENICAM_ENCODER_VALUE_LO_ADRS);
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderValueAtReset取得
		//----------------------------------------------------------------------------------
		case EncoderValueAtReset:
			value3rd = IN32(GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS);
			value2nd = IN32(GENICAM_ENCODER_VALUE_AT_RESET_HI_ADRS);
			value = value2nd;
			break;

		//----------------------------------------------------------------------------------
		// EncoderValueAtReset Low取得
		//----------------------------------------------------------------------------------
		case EncoderValueAtResetLow:
			value2nd = IN32(GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS);
			value = value2nd;
			break;

		//----------------------------------------------------------------------------------
		// EncoderValueAtResetLow取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS:
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// EncoderValueAtResetHigh取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_VALUE_AT_RESET_HI_ADRS:
			value3rd = IN32(GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS);
			value = IN32(address);
			break;

		//----------------------------------------------------------------------------------
		// GENICAM_ENCODER_FREQ_ADRS取得
		//----------------------------------------------------------------------------------
		case GENICAM_ENCODER_FREQ_ADRS:
			if ((*status = toG (encoderGetFrequency (&dblValue))) != AVAL_STATUS_SUCCESS)
			{
				value = 0;
				*status = toG (AVAL_STATUS_SUCCESS);
				break;
			}
			
			value = dblValue * GENICAM_ENCODER_GE_UNIT;
			break;

#endif // #if defined (MODE_ENCODER)


		//====================================================================================
		//
		// Group: Event Control registers of standard
		// Note:
		//
		//====================================================================================

		//----------------------------------------------------------------------------------
		// EventSelector取得
		//----------------------------------------------------------------------------------
		case EventSelector:
			value = Event_Selector;
			break;

		//----------------------------------------------------------------------------------
		// EventNotification取得
		//----------------------------------------------------------------------------------
		case EventNotification:
			value = Event_Notification[Event_Selector];
			break;

#if !defined (MODE_GIGE_10G)
		//----------------------------------------------------------------------------------
		// EventExposureEnd取得
		//----------------------------------------------------------------------------------
		 case EventExposureEnd:
			value = Event_Data[GEV_EVENT_ExposureEnd];;
			break;

		//----------------------------------------------------------------------------------
		// EventExposureEndTimestamp取得
		//----------------------------------------------------------------------------------
		 case EventExposureEndTimestamp:
			value = Event_Timestamp[GEV_EVENT_ExposureEnd];
			break;

		//----------------------------------------------------------------------------------
		// EventExposureEndFrameID取得
		//----------------------------------------------------------------------------------
		 case EventExposureEndFrameID:
			value = Event_Data[GEV_EVENT_ExposureEnd];
			break;
#endif // #if !defined (MODE_GIGE_10G)

		//----------------------------------------------------------------------------------
		// CounterDiagnosticSelector取得
		//----------------------------------------------------------------------------------
		case CounterDiagnosticSelector:
			value = CounterDiagnostic_Selector;
			break;

		//----------------------------------------------------------------------------------
		// CounterDiagnosticValue取得
		//----------------------------------------------------------------------------------
		case CounterDiagnosticValue:
			//@@@1value = IN32(ARM1_DIAG_VALUE_ADRS_OFFSET(CounterDiagnostic_Selector));
			break;


		//====================================================================================
		//
		// Group: DeviceControl
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// DeviceScanType取得
		//--------------------------------------------------------------------------------
		case DeviceScanType:
			value = IN32(GENICAM_SCAN_TYPE_ADRS);
			break;

		//--------------------------------------------------------------------------------
		// DeviceTLType取得
		//--------------------------------------------------------------------------------
		case DeviceTLType:
			if (gInterFaceID == INTERFACE_CXP)
				value = DeviceTLType_CoaXPress;
			else
				value = DeviceTLType_GigEVision;
			break;

		//--------------------------------------------------------------------------------
		// DeviceTapGeometry取得
		//--------------------------------------------------------------------------------
		case DeviceTapGeometry:
			value = DeviceTapGeometry_Geometry_1X_1Y;
			break;

		//--------------------------------------------------------------------------------
		// DeviceVariant取得
		//--------------------------------------------------------------------------------
		case DeviceVariant:
			value = IN32 (FIRM_DATA_VARIANT_MODE_ADRS);
			break;

		//--------------------------------------------------------------------------------
		// DeviceReset取得
		//--------------------------------------------------------------------------------
		case DeviceReset:
			value = 0;
			break;

		//--------------------------------------------------------------------------------
		// DeviceBoot取得
		//--------------------------------------------------------------------------------
		case DeviceBoot:
			value = 0;
			break;

		//--------------------------------------------------------------------------------
		// DeviceDipsw取得
		//--------------------------------------------------------------------------------
		case DeviceDipsw:
			*status = toG (getDipsw((unsigned int *)&value));
			break;

		//--------------------------------------------------------------------------------
		// DeviceFactory取得
		//--------------------------------------------------------------------------------
		case DeviceFactory:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_DEFAULT_ALL,	(u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//--------------------------------------------------------------------------------
		// DeviceCameraErrorSelect取得
		//--------------------------------------------------------------------------------
		case FIRM_DATA_CAMERA_ERROR_SELECT_ADRS:
			value = IN32(address);
			break;

		//--------------------------------------------------------------------------------
		// DeviceCameraErrorStatus取得
		//--------------------------------------------------------------------------------
		case FIRM_DATA_CAMERA_ERROR_STATUS_ADRS:
			value2nd = IN32 (FIRM_DATA_CAMERA_ERROR_SELECT_ADRS);	// Selector
			value2nd *= 4;
			value2nd += BOARD_ERROR_ADRS;
			value = IN32(value2nd);
			break;

		//--------------------------------------------------------------------------------
		// DeviceMainVolt取得
		//--------------------------------------------------------------------------------
#if defined (MODE_VOLTAGE_IF_BOARD)
	#if defined (MODE_BOARD_ACB532GE) || defined (MODE_BOARD_ACB531CXP)
		case DeviceMainVolt:
			if (gInterFaceID == INTERFACE_CXP)
				*status = toG (voltIfGet90va (&dblValue));
			else
				*status = toG (voltIfGet240v (&dblValue));
			fltValue = (float) dblValue;
			value = (int)(fltValue * DEVICE_VOLT_UNIT);
			break;
	#endif // #if defined (MODE_BOARD_ACB523GE) || defined (MODE_BOARD_ACB525CXP)
#endif // #if defined (MODE_VOLTAGE_IF_BOARD)


		//--------------------------------------------------------------------------------
		// SensorBlackPixel取得
		//--------------------------------------------------------------------------------
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
		case SensorBlackPixel:
			*status = toG (sensorGetBlackPixel ((int *)&value));
			break;
#endif

		//--------------------------------------------------------------------------------
		// DeviceAcesFlag取得
		//--------------------------------------------------------------------------------
		case DeviceAcesFlag:
			value = IN32 (FIRM_DATA_CPU0_MAIN_ACCESS_FLAG);
			break;

		//--------------------------------------------------------------------------------
		// DeviceRateMode取得
		//--------------------------------------------------------------------------------
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
		case DeviceRateMode:
			*status = toG (acquisitionGetRateMode ((int *)&value));
			break;
#endif

		//--------------------------------------------------------------------------------
		// FPGA_TG_TGSE_ADRS取得
		//--------------------------------------------------------------------------------
		case FPGA_TG_TGSE_ADRS:
			*status = toG (tgGetTgse ((unsigned int *)&value));
			break;

		//--------------------------------------------------------------------------------
		// FPGA_TG_TGES_ADRS取得
		//--------------------------------------------------------------------------------
		case FPGA_TG_TGES_ADRS:
			*status = toG (tgGetTges ((unsigned int *)&value));
			break;

		//--------------------------------------------------------------------------------
		// FPGA_TG_TGPD_ADRS取得
		//--------------------------------------------------------------------------------
		case FPGA_TG_TGPD_ADRS:
			*status = toG (tgGetTgpd ((unsigned int *)&value));
			break;


		//====================================================================================
		//
		// Group: High Speed Mode
		// Note:
		//
		//====================================================================================

#if defined (MODE_FRAMERATE_HIGH_SPEED)
		//--------------------------------------------------------------------------------
		// HighSpeedMode取得
		//--------------------------------------------------------------------------------
		case HighSpeedMode:
			*status = toG (sensorGetFrameRateHighSpeedMode ((int *)&value));
			break;

		//--------------------------------------------------------------------------------
		// HighSpeedModeCmd取得
		//--------------------------------------------------------------------------------
		case HighSpeedModeCmd:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_HIGH_SPEED_MODE, (u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//--------------------------------------------------------------------------------
		// HighSpeedModeLineCount取得
		//--------------------------------------------------------------------------------
		case HighSpeedModeLineCount:
			//*status = toG(aoiGetHeight((int*) &value));
			value = gHighSpeedModeLineCount;
			break;

		//--------------------------------------------------------------------------------
		// HighSpeedModeSelector取得
		//--------------------------------------------------------------------------------
		case HighSpeedModeSelector:
			value = gHighSpeedMode;
			break;
#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)


		//====================================================================================
		//
		// Group: Diagnostic Control registers of standard
		// Note:
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// DeviceBuiltInTest取得
		//--------------------------------------------------------------------------------
		case DeviceBuiltInTest:
			value = DeviceBuiltInTest_Mode;
			// 診断ステートLED ledDiagnosticState (void);
			if (DeviceBuiltInTest_Mode == 0)
			{
				//ledSetState(LED_STATE,LED_BOOTING,0);
			}
			else
			{
				//ledSetState(LED_STATE,LED_DIAG,0);
			}
			break;

		//--------------------------------------------------------------------------------
		// DeviceBuiltInTestStatus取得
		//--------------------------------------------------------------------------------
		case DeviceBuiltInTestStatus:
			value = DeviceBuiltInTest_Status;
			break;

		//--------------------------------------------------------------------------------
		// DiagnosticSelector取得
		//--------------------------------------------------------------------------------
		case DiagnosticSelector:
			value = Diagnostic_Selector;
			break;

		//--------------------------------------------------------------------------------
		// Diagnostic取得
		//--------------------------------------------------------------------------------
		case Diagnostic:
			switch (Diagnostic_Selector)
			{
				case 0:
					break;
				case 1:
					//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_AGING,(u32*) &value);
					*status = toG (cmdExecuteStatus ((int *)&value));
					break;
				default:
					break;
			}
			break;

		//--------------------------------------------------------------------------------
		// DiagnosticResult取得
		//--------------------------------------------------------------------------------
		case DiagnosticResult:
			switch (Diagnostic_Selector)
			{
				case 0:
					break;
				case 1:
					value = (IN32(BOARD_STATUS_AGING_ADRS) > 0) ? 0 : 1;
					break;
				default:
					break;
			}
			break;

		//--------------------------------------------------------------------------------
		// DiagnosticResultNumber取得
		//--------------------------------------------------------------------------------
		case DiagnosticResultNumber:
			switch (Diagnostic_Selector)
			{
				case 0:
					break;
				case 1:
					value = IN32(BOARD_STATUS_AGING_ADRS);
					break;
				default:
					break;
			}
			break;


		//====================================================================================
		//
		// Group: Device Information registers of standard
		// Note:
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// Temperature Selector取得
		//--------------------------------------------------------------------------------
		case DeviceTemperatureSelector:
			value = DeviceTemperature_Selector;
			break;

		//--------------------------------------------------------------------------------
		// DeviceTemperaturePeltierMode取得
		//--------------------------------------------------------------------------------
#if defined (MODE_PELTIER)
		case DeviceTemperaturePeltierMode:
			*status = peltierGetMode ((int *)&value);
			break;
#endif

		//--------------------------------------------------------------------------------
		// Sensor Target Temperature取得
		//--------------------------------------------------------------------------------
		case FPGA_PELTIER_TARGET_ADRS:
#if defined (MODE_PELTIER_MOUNTING_SWITCH)
			if ((*status = toG (peltierGetMountState ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;
#else
			value2nd = MODE_ENABLE;
#endif

			if(value2nd == MODE_ENABLE)
			{
				*status = toG (peltierGetTarget (&dblValue));

				if (dblValue >= 0)
					fltValue = (float)(dblValue + 0.5);
				else
					fltValue = (float)(dblValue - 0.5);

				value = (int) fltValue;
			}
			break;

		//--------------------------------------------------------------------------------
		// Sensor Temperature Float取得
		//--------------------------------------------------------------------------------
		case DeviceTemperature_Sensor_Float:
			*status = toG (peltierGetSensorTemp (&dblValue));
			fltValue = (float) dblValue;
			value = (int)(fltValue * DEVICE_TEMP_UNIT);
			break;

		//--------------------------------------------------------------------------------
		// Case Temperature Float取得
		//--------------------------------------------------------------------------------
		case DeviceTemperature_Case_Float:
			*status = toG (peltierGetCaseTemp (&dblValue));
			fltValue = (float) dblValue;
			value = (int)(fltValue * DEVICE_TEMP_UNIT);
			break;

		//--------------------------------------------------------------------------------
		// FPGA Temperature Float取得
		//--------------------------------------------------------------------------------
		case DeviceTemperature_FPGA_Float:
			*status = toG (xadcGetFpgaTemp (&dblValue));
			fltValue = (float) dblValue;
			value = (int)(fltValue * DEVICE_TEMP_UNIT);
			break;

		//--------------------------------------------------------------------------------
		// Sensor Max Temperature取得
		//--------------------------------------------------------------------------------
		case DeviceTemperature_Sensor_Max:
			value2nd = (u32) 80;
			fltValue = (float) value2nd;
			value = (int) fltValue;
			break;

		//--------------------------------------------------------------------------------
		// Sensor Min Temperature取得
		//--------------------------------------------------------------------------------
		case DeviceTemperature_Sensor_Min:
			value = (int) -30;
			break;

		//--------------------------------------------------------------------------------
		// Sensor Min Target Temperature取得
		//--------------------------------------------------------------------------------
		case DeviceTemperatureTarget_Min:
			*status = peltierGetTargetMinMax (&dblValue,&dblValue2nd);
			value = (int)dblValue;
			break;

		//--------------------------------------------------------------------------------
		// Sensor Max Target Temperature取得
		//--------------------------------------------------------------------------------
		case DeviceTemperatureTarget_Max:
			*status = peltierGetTargetMinMax (&dblValue,&dblValue2nd);
			value = (int)dblValue2nd;
			break;

		//--------------------------------------------------------------------------------
		// Sensor Alarm Max Temperature取得
		//--------------------------------------------------------------------------------
		case FPGA_PELTIER_SENSOR_ALM_ADRS:
			*status = toG (peltierGetSensorTempAlarm (&dblValue, &dblValue2nd));
			fltValue = (float) dblValue;
			value = (int) fltValue;
			break;

		//--------------------------------------------------------------------------------
		// Sensor Alarm Min Temperature取得
		//--------------------------------------------------------------------------------
		case DeviceTemperatureAlarmMin_Sensor:
			*status = toG (peltierGetSensorTempAlarm (&dblValue, &dblValue2nd));
			fltValue = (float) dblValue2nd;
			value = (int) fltValue;
			break;

		//--------------------------------------------------------------------------------
		// Case Alarm Max Temperature取得
		//--------------------------------------------------------------------------------
		case FPGA_PELTIER_CASE_ALM_ADRS:
			*status = toG (peltierGetCaseTempAlarm (&dblValue, &dblValue2nd));
			fltValue = (float) dblValue;
			value = (u32) fltValue;
			break;

		//--------------------------------------------------------------------------------
		// Case Alarm Min Temperature取得
		//--------------------------------------------------------------------------------
		case DeviceTemperatureAlarmMin_Housing:
			*status = toG (peltierGetCaseTempAlarm (&dblValue, &dblValue2nd));
			fltValue = (float) dblValue2nd;
			value = (int) fltValue;
			break;

		//--------------------------------------------------------------------------------
		// DeviceTemperatureAlarmStatus取得
		//--------------------------------------------------------------------------------
		case DeviceTemperatureAlarmStatus:
			value = (int)IN32 (BOARD_STATUS_TEMP_ADRS);
			break;

		//--------------------------------------------------------------------------------
		// DeviceTemperatureSensorAlarmCount取得
		//--------------------------------------------------------------------------------
		case DeviceTemperatureSensorAlarmCount:
			value = (int)IN32 (BOARD_STATUS_SENSOR_TEMP_UPPER_ADRS);
			break;

		//--------------------------------------------------------------------------------
		// DeviceTemperatureCaseAlarmCount取得
		//--------------------------------------------------------------------------------
		case DeviceTemperatureCaseAlarmCount:
			value = (int)IN32 (BOARD_STATUS_CASE_TEMP_ADRS);
			break;

#if defined (MODE_PELTIER_CTRL)
		//--------------------------------------------------------------------------------
		// DevicePeltierPowerLevel取得
		//--------------------------------------------------------------------------------
		case DevicePeltierPowerLevel:
			*status = toG (peltierGetPowerLevel ((int *)&value));
			break;
#endif

#if defined (MODE_PELTIER_VOLT_CURRENT)
		//--------------------------------------------------------------------------------
		// Peltier Voltage取得取得
		//--------------------------------------------------------------------------------
		case DevicePeltierVolt:
			*status = toG (peltierGetVolt (&dblValue));
			value = (int)(dblValue * DEVICE_VOLT_UNIT);
			break;

		//--------------------------------------------------------------------------------
		// Peltier Current取得取得
		//--------------------------------------------------------------------------------
		case DevicePeltierCurrent:
			*status = toG (peltierGetCurrent (&dblValue));
			value = (int)(dblValue * DEVICE_CURRENT_UNIT);
			break;
#endif

#if defined (MODE_TEMP_ABNORMAL_CHECK)
		//--------------------------------------------------------------------------------
		// 温度異常ステータス取得
		//--------------------------------------------------------------------------------
		case DeviceTemperatureAbnormalStatus:
			*status = toG (tempGetAbnormalStatus ((unsigned int *)&value));
			break;

		//--------------------------------------------------------------------------------
		// 温度異常カウント取得
		//--------------------------------------------------------------------------------
		case DeviceTemperatureAbnormalCount:
			*status = toG (tempGetAbnormalCount ((unsigned int *)&value));
			break;
#endif

#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
		//--------------------------------------------------------------------------------
		// PHY温度取得
		//--------------------------------------------------------------------------------
		case DeviceTemperaturePhy:
			if ((*status = toG (phyGetTemp (&fltValue))) == AVAL_STATUS_SUCCESS)
				value = (int)(fltValue * DEVICE_TEMP_UNIT);
			break;
#endif

		//--------------------------------------------------------------------------------
		// Sensor Power Status取得
		//--------------------------------------------------------------------------------
		case FIRM_DATA_SENSOR_POWER_STATUS_ADRS:
			value = (int)IN32 (address);
			break;

#if defined (MODE_SENSOR_DRRS)
		//--------------------------------------------------------------------------------
		// DeviceDrrsCommand取得
		//--------------------------------------------------------------------------------
		case DeviceDrrsCommand:
			//*status = (u32) isExecuteCommandDone((u32) 1, FIRM_CMD_DRRS, (u32*) &value);
			*status = toG (cmdExecuteStatus ((int *)&value));
			break;

		//--------------------------------------------------------------------------------
		// DeviceDrrsMode取得
		//--------------------------------------------------------------------------------
		case DeviceDrrsMode:
			value = gDrrsMode;
			break;

		//--------------------------------------------------------------------------------
		// DeviceDrrsStatus取得
		//--------------------------------------------------------------------------------
		case DeviceDrrsStatus:
			*status = toG (sensorGetDrrs ((int *)&value));
			break;
#endif


		//====================================================================================
		//
		// Group: Auto Bright
		// Note:
		//
		//====================================================================================
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
		//----------------------------------------------------------------------------------
		// Auto Bright Exposure Mode取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_EXPOSURE_MODE:
			*status = toG (autoBrightGetExposureMode ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Exposure Once Status取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_EXPOSURE_STATUS:
			*status = toG (autoBrightGetExposureStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Exposure Min取得
		//----------------------------------------------------------------------------------
		//case AUTO_BRIGHT_EXPOSURE_MIN:
			//*status = toG (autoBrightGetExposureMin ((int *)&value));
			//break;

		//----------------------------------------------------------------------------------
		// Auto Bright Exposure Max取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_EXPOSURE_MAX:
			*status = toG (autoBrightGetExposureMax ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Gain Mode取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_GAIN_MODE:
			*status = toG (autoBrightGetGainMode ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Gain Once Status取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_GAIN_STATUS:
			*status = toG (autoBrightGetGainStatus ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Gain Min取得
		//----------------------------------------------------------------------------------
		//case AUTO_BRIGHT_GAIN_MIN:
			//*status = toG (autoBrightGetGainMin ((double *)&dblValue));
			//value = (int)dblValue;
			//break;

		//----------------------------------------------------------------------------------
		// Auto Bright Gain Max取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_GAIN_MAX:
			*status = toG (autoBrightGetGainMax ((double *)&dblValue));
			value = (int)dblValue;
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Area取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_OVERLAY:
			*status = toG (autoBrightGetDetectArea ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Target取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_TARGET:
			*status = toG (autoBrightGetTarget ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Average取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_AVERAGE:
			*status = toG (autoBrightGetAverage ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Area取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_TARGET_AREA:
			*status = toG (autoBrightGetTargetArea ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Width Size取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_WIDTH_SIZE:
			*status = toG (autoBrightGetWidthSize ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Height Size取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_HEIGHT_SIZE:
			*status = toG (autoBrightGetHeightSize ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Width Offset取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_WIDTH_OFFSET:
			*status = toG (autoBrightGetWidthOffset ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Height Offset取得
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_HEIGHT_OFFSET:
			*status = toG (autoBrightGetHeightOffset ((int *)&value));
			break;
#endif // #if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)


		//====================================================================================
		//
		// Group: Binning
		// Note:
		//
		//====================================================================================
#if defined(MODE_BINNING)
		//----------------------------------------------------------------------------------
		// BinningX取得
		//----------------------------------------------------------------------------------
		case BINNING_HORIZONTAL:
			*status = toG (aoiGetBinningX ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// BinningY取得
		//----------------------------------------------------------------------------------
		case BINNING_VERTICAL:
			*status = toG (aoiGetBinningY ((int *)&value));
			break;

		//----------------------------------------------------------------------------------
		// Binningmode取得
		//----------------------------------------------------------------------------------
		case BINNING_MODE:
			*status = toG (aoiGetBinningMode ((int *)&value));
			break;
#endif //#if defined(MODE_BINNING)

	
		//====================================================================================
		//
		// For 10G Ethernet
		// Group: 
		//
		//====================================================================================
#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
		//----------------------------------------------------------------------------------
		// DevicePhyFirmVersion取得
		//----------------------------------------------------------------------------------
		case DevicePhyFirmVersion:
		case DevicePhyFirmVersion + 4:
		case DevicePhyFirmVersion + 8:
		case DevicePhyFirmVersion + 12:
			value2nd = IN32( (FIRM_DATA_PHY_FIRM_VERSION + (address & ~DevicePhyFirmVersion)) );
			value = SWAP_L(value2nd);
			break;

		//----------------------------------------------------------------------------------
		// DevicePhyApiVersion取得
		//----------------------------------------------------------------------------------
		case DevicePhyApiVersion:
		case DevicePhyApiVersion + 4:
			value2nd = IN32( (FIRM_DATA_PHY_API_VERSION + (address & ~DevicePhyApiVersion)) );
			value = SWAP_L(value2nd);
			break;
	
		//----------------------------------------------------------------------------------
		// DevicePacketDelayCalc取得
		//----------------------------------------------------------------------------------
		case DevicePacketDelayCalc:
			*status = gigECalcPacketDelay ((unsigned int *)&value);
			break;

		//----------------------------------------------------------------------------------
		// DevicePacketDelayData取得
		//----------------------------------------------------------------------------------
		case DevicePacketDelayData:
			*status = gigeGetPacketDelayData ((unsigned int *)&value);
			break;
#endif // #if defined (MODE_GIGE_10G) && defined (IF_GIGE)	

	
		//====================================================================================
		//
		// For IEEE 1588 PTP 
		// Group: 
		//
		//====================================================================================

#if defined (MODE_IEEE1588_PTP) && defined (IF_GIGE)
		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_CLOCK_ACCURACY取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_CLOCK_ACCURACY:
			value = sfnc_ptp.clockAccuracy;		// PtpClockAccuracy (24..31)
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_DATA_SET取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_DATA_SET:
			value = 0;							// PtpDataSetLatch (31) WRITE-ONLY
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_STATUS取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_STATUS:
	 		value = sfnc_ptp.status;;			// PtpStatus (28..31)
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_SERVER_STATUS取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_SERVER_STATUS:
			value = sfnc_ptp.servoStatus;		// PtpServoStatus (28..31)
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_OFFSET_FORM_MASTER_H取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_OFFSET_FORM_MASTER_H:
	 		value = (uint32_t)(sfnc_ptp.offsetFromMaster >> 32);		// PtpOffsetFromMaster
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_OFFSET_FORM_MASTER_L取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_OFFSET_FORM_MASTER_L:
			value = (uint32_t)(sfnc_ptp.offsetFromMaster & 0xFFFFFFFF);	// PtpOffsetFromMaster
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_MEAN_PATH_DELAY_H取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_MEAN_PATH_DELAY_H:
	 		value = (uint32_t)(sfnc_ptp.meanPathDelay >> 32);			// PtpMeanPathDelay
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_MEAN_PATH_DELAY_L取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_MEAN_PATH_DELAY_L:
			value = (uint32_t)(sfnc_ptp.meanPathDelay & 0xFFFFFFFF);	// PtpMeanPathDelay
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_CLOCK_ID_H取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_CLOCK_ID_H:
	 		value = (uint32_t)(sfnc_ptp.clockId >> 32);			// PtpClockId
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_CLOCK_ID_L取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_CLOCK_ID_L:
	 		value = (uint32_t)(sfnc_ptp.clockId & 0xFFFFFFFF);	// PtpClockId
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_PARENT_CLOCK_ID_H取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_PARENT_CLOCK_ID_H:
			value = (uint32_t)(sfnc_ptp.parentClockId >> 32);		// PtpParentClockId
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_PARENT_CLOCK_ID_L取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_PARENT_CLOCK_ID_L:
			value = (uint32_t)(sfnc_ptp.parentClockId & 0xFFFFFFFF);	// PtpParentClockId
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_GRAND_MASTER_CLOCK_ID_H取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_GRAND_MASTER_CLOCK_ID_H:
			value = (uint32_t)(sfnc_ptp.grandmasterClockId >> 32);			// PtpGrandmasterClockId
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_GRAND_MASTER_CLOCK_ID_L取得
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_GRAND_MASTER_CLOCK_ID_L:
	 		value = (uint32_t)(sfnc_ptp.grandmasterClockId & 0xFFFFFFFF);	// PtpGrandmasterClockId
			break;
#endif // #if defined (MODE_IEEE1588_PTP) && defined (IF_GIGE)


		//====================================================================================
		//
		// Group: File Access Control registers of standard
		// Note:
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// FileSelector取得
		//--------------------------------------------------------------------------------
		case FileSelector:
			value = fileSelector;
			break;

		//--------------------------------------------------------------------------------
		// FileOperationeSelector取得
		//--------------------------------------------------------------------------------
		case FileOperationeSelector:
			value = fileSel[fileSelector];
			break;

		//--------------------------------------------------------------------------------
		// FileOperationExecute取得取得
		//--------------------------------------------------------------------------------
		case FileOperationExecute:
			// コマンドステータス取得
			*status = toG (cmdExecuteStatus ((int *)&value2nd));
			if (value2nd == command_done)
			{
				fileExec[fileSelector] = 0;
				value = 0;

				// Success or Failure
				fileStatus[fileSelector][fileSel[fileSelector]] = *status;
			}
			else
			{
				value = fileExec[fileSelector];
			}

			break;

		//--------------------------------------------------------------------------------
		// FileOpenMode
		//--------------------------------------------------------------------------------
		case FileOpenMode:
			value = fileMode[fileSelector];
			break;

		//--------------------------------------------------------------------------------
		// FileAccessOffset
		//--------------------------------------------------------------------------------
		case FileAccessOffset:
			value = fileOffset[fileSelector][fileSel[fileSelector]];
			break;

		//--------------------------------------------------------------------------------
		// FileAccessLength
		//--------------------------------------------------------------------------------
		case FileAccessLength:
			value = fileLength[fileSelector][fileSel[fileSelector]];
			break;

		//--------------------------------------------------------------------------------
		// FileOperationStatus
		//--------------------------------------------------------------------------------
		case FileOperationStatus:
			value = fileStatus[fileSelector][fileSel[fileSelector]];
			break;

		//--------------------------------------------------------------------------------
		// FileOperationResult
		//--------------------------------------------------------------------------------
		case FileOperationResult:
			value = fileResult[fileSelector][fileSel[fileSelector]];
			break;

		//--------------------------------------------------------------------------------
		// FileSize
		//--------------------------------------------------------------------------------
		case FileSize:
			value = fileSize[fileSelector];
			break;

		//--------------------------------------------------------------------------------
		// default
		//--------------------------------------------------------------------------------
		default:


			//====================================================================================
			//
			// Group: Spectrum Control
			// Note:
			//
			//====================================================================================
			#if 0	//@@@1
			#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
			if (address >= FPGA_BGF_BAND_OFFSET1_ADRS && address < (FPGA_BGF_BAND_OFFSET1_ADRS + (SPECTRUM_BAND_COUNT * FPGA_BGF_BAND_INTEVAL)) )
			{
				//value2nd = address % 0x400;
			    //value = IN32 (address);
				//if( address == 0x402209F4 )
				//{
				//	// The last Gain of Band spectrum for debugging
				//	value = value;
				//}

				value3rd =  (address - FPGA_BGF_BAND_OFFSET1_ADRS);
				value3rd =  ((long)value3rd >= (long)FPGA_BGF_BAND_INTEVAL)? value3rd / FPGA_BGF_BAND_INTEVAL : 0;
				value2nd = (address & 0xF);
			    switch(value2nd)
			    {
					case 0x0: // SpectrumBlackLevel as FPGA_BGF_BAND_OFFSET1_ADRS
						*status = toG(bgfGetBandOffset((int)value3rd,(int *)&value));
						break;
					case 0x4: // SpectrumGain as FPGA_BGF_BAND_GAIN_ADRS
						*status = toG(bgfGetBandGainX((int)value3rd, &fltValue));
						fltValue += 0.005;	// 四捨五入（小数点第2位まで有効）
						value = (int)(fltValue * DEVICE_GAIN_UNIT);
						break;
					case 0x8: // SpectrumBlackLevelPreceding as FPGA_BGF_BAND_OFFSET2_ADRS
						*status = toG(bgfGetBandOffset1((int)value3rd,(int *)&value));
						break;
					case 0x10:
						// Reserved
						*status = GEV_STATUS_INVALID_PARAMETER;
						break;
					default:
						// Unknown
						*status = GEV_STATUS_INVALID_PARAMETER;
						break;
			    }
			}
			#endif // #if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
			#endif // #if 0	//@@@1


			//====================================================================================
			//
			// Group: File Access Control  virtual buffer registers map of FPGA
			// Note:
			//
			//====================================================================================
			if (address >= FileAccessBuffer && address < BASE_FILE_BUFFER_MAX)
			{
				address2nd = address & BASE_FILE_BUFFER_MASK; // Gets the address of offset by starting point
				//address3rd   = fileLength[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
				valueBuffer = (u32*) fileBuffer[fileSelector]; //Gets the start pointer of temporary buffer
				if (valueBuffer != 0)
				{
					valueBuffer += (address2nd > 0) ? address2nd / 4 : 0;
					value = *valueBuffer;
				}
			}

			//====================================================================================
			//
			// Group: 8M byte to virtual Offset address for the Camera
			// Note: QSPI Flash 8,388,607 Byte
			//
			//====================================================================================
			#if 0 //@@@1
			if ((address >= BASE_BOOTROM) && (address < BASE_BOOTROM_MAX))
			{
				address2nd = (address & BASE_BOOTROM_MASK); // Clip the high virtual address
				*status = qspiFlashRead((unsigned int) address2nd, (unsigned char*) &value, (unsigned int) sizeof(value));
				break;
			}
			#endif //@@@1

			//====================================================================================
			//
			// Group: EEPROM Flash Control registers of GigE standard
			// Note:
			//
			//====================================================================================
			// Configuration EEPROM 8-64kByte, here 8k)
			if ((address >= BASE_NET_BOOTROM) && (address <= BASE_NET_BOOTROM_MAX))
			{
				address2nd = address & BASE_NET_BOOTROM_MASK;
				value = eeprom_read_dword((u16) address2nd);
				break;
			}

			//====================================================================================
			//
			// Group: Flat Field Correction Data
			// Note:
			//
			//====================================================================================
			//@@@1if ((address >= FFC_MEMORY_ADRS) && (address <= (FFC_MEMORY_ADRS + FFC_DATA_SECOND_SIZE)))
			//@@@1{
				//@@@1value = IN32 (address);
				//@@@1break;
			//@@@1}

			//====================================================================================
			//
			// Group: Defective Pixel Correction Data
			// Note:
			//
			//====================================================================================
			//@@@1if ((address >= DPC_MEMORY_ADRS) && (address <= (DPC_MEMORY_ADRS + DPC_DATA_SIZE)))
			//@@@1{
				//@@@1value = IN32 (address);
				//@@@1break;
			//@@@1}

			//====================================================================================
			//
			// Group: LUT registers map of FPGA
			// Note:
			//
			//====================================================================================
			//@@@1if ((address >= FPGA_LUT_MEM1_ADRS)	&& (address < FPGA_LUT_MEM1_ADRS + LUT_DATA_SIZE))
			//@@@1{
				//@@@1*status = toG(aoiGetBitWidth((int*) &value2nd));		// ビット幅取得
				//@@@1*status = toG(aoiGetShift((int) value2nd, (int*) &value3rd));// シフト数取得
				//@@@1value2nd = (int) IN32(address);
				//@@@1value = (value2nd & LUT_DATA_MASK) >> value3rd; // Mask the actual LUT address from a virtual address
				//@@@1break;
			//@@@1}
			//@@@1else if ((address >= FPGA_LUT_MEM2_ADRS) && (address < FPGA_LUT_MEM2_ADRS + LUT_DATA_SIZE))
			//@@@1{
				//@@@1*status = toG(aoiGetBitWidth((int*) &value2nd)); // ビット幅取得
				//@@@1*status = toG(aoiGetShift((int) value2nd, (int*) &value3rd)); // シフト数取得
				//@@@1value2nd = (int) IN32(address);
				//@@@1value = (value2nd & LUT_DATA_MASK) >> value3rd; // Mask the actual LUT address from a virtual address
				//@@@1break;
			//@@@1}

			//====================================================================================
			//
			// Group: XML registers map of FPGA
			// Note:
			//
			//====================================================================================
			if ((address >= xmlStartAddress) && (address < xmlStartAddress + xmlSize + 1024))
			{
				//valueBuffer  = (u32*)fileBuffer[FileSelector_XML]; //Gets the start pointer of XML file buffer
				//if(valueBuffer!=0)
				//{
				//	valueBuffer +=(address2nd>0)?address2nd/4:0;
				//	value = SWAP_L((*valueBuffer));
				//}
				address2nd = address - xmlStartAddress;
				value = IN32 ((FIRM_XML_FILE_ADRS + address2nd));
				value = SWAP_L (value);
				break;
			}
			else if ((address >= xmlStartAddressSecond) && (address < xmlStartAddressSecond + xmlSizeSecond + 1024))
			{
				address2nd = address & BASE_NET_BOOTROM_XMLFILE_MASK;
				*status = qspiFlashRead((unsigned int) address2nd, (unsigned char*) &value, (unsigned int) sizeof(value));
				value = SWAP_L(value); // with swapped endian
				break;
			}
			else
			{

			}

	        //====================================================================================
			//
			// For IP Core Design
			// Configuration EEPROM 8-64kByte, here 8k)
			//
	        //====================================================================================
			//@@1if ((address >= 0xFBFF0000) && (address < 0xFBFF2000))
			//@@1{
				//@@1*status = eeprom_read_dword((u16) (address - 0xFBFF0000));
				//@@1break;
			//@@1}
			// SPI flash memory
			//@@1if (address >= 0xFE000000)
			//@@1{
				//@@@1*status = flash_read_dword(address - 0xFE000000);
				//@@1break;
			//@@1}

	        //====================================================================================
			//
			// Group: Board Error Address
			// Note:
			//
			//====================================================================================
		    //@@1if( (address >= BOARD_ERROR_ADRS) && (address < (BOARD_ERROR_ADRS + BOARD_ERROR_SIZE)) )
		    //@@1{
		    	//value2nd = IN32 (address);
		    	//value = SWAP_L (value2nd);
		    	//@@1value = IN32 (address);
		    //@@1}
		}

		// Indicates the current state at the LED of a rear-panel
		ledSetState(LED_STATE,(*status == GEV_STATUS_SUCCESS) ? LED_COMMAND : LED_STREAMING_ERROR,*status);

		//@@1if(address < xmlStartAddress || (address >= FPGA_BASE_ADRS && address < GENICAM_ADRS+0x00FFFFFF))
		//@@@1{
			//@@@1DEBUG_PRINT("%s reads 0x%08X(%8d / %f / %f ) at 0x%08X(0x%08X) %s with 0x%08X\r\n",
				//@@@1GIGE_TAG_OUT, (int)value, (int)value,(float)fltValue,(double)dblValue,(unsigned int)address,(unsigned int)address2nd,(*status==0)?"success":"failed",(unsigned int)*status);
		//@@@1}

	return value;
}


//##############################################################################
// ---- Write GigE Vision user-space bootstrap register ------------------------
//
//           This function must be always implemented!
//           It is called by the gige_callback() from libgige
//
// address = address of the register within GigE Vision manufacturer-specific
//           register address space (0x0000A000 - 0xFFFFFFFF)
// value   = data to be written
// status  = return status value, should be one of following:
//           GEV_STATUS_SUCCESS         - write passed correctly
//           GEV_STATUS_INVALID_ADDRESS - register at 'address' does not exist
//           GEV_STATUS_WRITE_PROTECT   - register at 'address' is read-only
//           GEV_STATUS_LOCAL_PROBLEM   - problem while setting register value
//           GEV_STATUS_ERROR           - unspecified error
//
void set_user_reg(u32 address, u32 value, u16 *status)
{
	*status = GEV_STATUS_SUCCESS;
	u32 value2nd = 0, value3rd = 0, i;
#if defined (MODE_SPECTRUM_BAND_SELECT)
	u32 value4th;
#endif
	u32 address2nd = 0, address3rd = 0;
	u32* valueBuffer = 0;
	u32* valueBuffer2nd = 0;
	int intValue;
	double dblValue = 0.0, dblValue2nd = 0.0;
	float fltValue = 0.0;
	unsigned char bytValue = 0;

#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
    int update_leader = 0, update_trailer = 0;
    u32 chunk_size;
    u32 chunk_layout_id = 0;    // Chunk layout id
	static u32 old_chunk_layout_id = 0;
#endif

	switch (address)
	{
		//====================================================================================
		//
		// Group: Image Format Control registers of standard
		// Note:
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// Sensor Width設定
		//--------------------------------------------------------------------------------
		case SensorWidth:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Sensor Height設定
		//--------------------------------------------------------------------------------
		case SensorHeight:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Camera Width設定
		//--------------------------------------------------------------------------------
		case CameraWidthMax:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Camera Height設定
		//--------------------------------------------------------------------------------
		case CameraHeightMax:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Width設定
		//--------------------------------------------------------------------------------
		case FPGA_AOI_XSIZE_ADRS:
			if (IN32(FIRM_DATA_CPU1_BOOT_FLAG) != 0)
				*status = toG (aoiSetWidth((int) value));

			toG (aoiGetWidth((int*) &value));
			video_width = value;

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			if (video_chunk_ctrl & 0x80000000)
				update_trailer = 1;
			#endif

			break;

		//--------------------------------------------------------------------------------
		// Height設定
		//--------------------------------------------------------------------------------
		case FPGA_AOI_YSIZE_ADRS:
		
			//--------------------------------------------------------------------------------
			// High Speed Mode
			//--------------------------------------------------------------------------------
			#if defined (MODE_FRAMERATE_HIGH_SPEED)
			if ((*status = toG (sensorGetFrameRateHighSpeedMode ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			if (value3rd == MODE_ENABLE)
			{
				if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
				{
					// Check Height
					if ((value <= 0) || (value > FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_GE))
						value = FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_GE;
				}
				else
				{
					// Check Height
					if ((value <= 0) || (value > FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_OTHER))
						value = FRAME_RATE_HIGH_SPEED_MODE_HEIGHT_MAX_OTHER;
				}

				#if defined (IF_CXP)
				if (gInterFaceID == INTERFACE_CXP)
				{
					if (value < CXP_HEIGHT_ALIGH)
						value = CXP_HEIGHT_ALIGH;

					if ((value % CXP_HEIGHT_ALIGH) != 0)
						value &= ~(CXP_HEIGHT_ALIGH-1);

				}
				#endif // #if defined (IF_CXP)

				// Set Virtual Height
				sensorSetVirtualHeight (value);

				// Get Virtual Height
				sensorGetVirtualHeight ((int *)&value);
				
				// Set Video Height
				video_height = value;
				
				#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
				update_leader  = 1;
				update_trailer = 1;
				#endif

				break;
			}
			else
			#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)
			{
			//--------------------------------------------------------------------------------
			// Area
			//--------------------------------------------------------------------------------

			// ROIモード取得
			if ((*status = toG (roiCheckMultiMode ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			if (value2nd == MODE_ENABLE)
			{
				*status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, *status, "Cannot write because multi mode is enabled\n");
				break;
			}

			// Height設定
			if (IN32(FIRM_DATA_CPU1_BOOT_FLAG) != 0)
			{
				if ((*status = toG(aoiSetHeight((int) value))) != AVAL_STATUS_SUCCESS)
					break;
			}

			// Height取得
			if ((*status = toG(aoiGetHeight((int*) &value))) != AVAL_STATUS_SUCCESS)
				break;

			video_height = value;

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader  = 1;
			update_trailer = 1;
			#endif
			}

			break;

		//--------------------------------------------------------------------------------
		// OffsetX設定
		//--------------------------------------------------------------------------------
		case FPGA_AOI_XOFFSET_ADRS:
			if (IN32(FIRM_DATA_CPU1_BOOT_FLAG) != 0)
				*status = toG (aoiSetWidthOffset((int)value));

			toG (aoiGetWidthOffset((int*)&value));
			video_offs_x = value;

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			#endif

			break;

		//--------------------------------------------------------------------------------
		// OffsetY設定
		//--------------------------------------------------------------------------------
		case FPGA_AOI_YOFFSET_ADRS:

			// ROIモード取得
			if ((*status = toG (roiCheckMultiMode ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			if (value2nd == MODE_ENABLE)
			{
				*status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, *status, "Cannot write because multi mode is enabled\n");
				break;
			}

			if (IN32(FIRM_DATA_CPU1_BOOT_FLAG) != 0)
			{
				// Offset設定
				if ((*status = toG( aoiSetHeightOffset((int)value))) != AVAL_STATUS_SUCCESS)
					break;
			}
			
			// Offset取得
			toG( aoiGetHeightOffset((int*)&value) );

			video_offs_y = value;

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			#endif

			break;

		//--------------------------------------------------------------------------------
		// ReverseX設定
		//--------------------------------------------------------------------------------
		case FPGA_XFLIP_CTRL_ADRS_FPGA:

		#if defined (MODE_XFLIP_INVERT)
			if (value == XFLIP_ENABLE)
				value = XFLIP_DISABLE;
			else
				value = XFLIP_ENABLE;
		#endif

			// X Flip Command
			//value2nd = executeCommand(FIRM_CMD_SENSOR_XFLIP, CPU_CMD_SYNC_ON, (u32*) &value, 1);
			//*status = toG (value2nd);
			*status = toG (aoiSetXflip (value));
			break;

		//----------------------------------------------------------------------------------
		// ROI Selector設定
		//----------------------------------------------------------------------------------
		case FIRM_DATA_ROI_SELECTOR_ADRS:
			*status = toG (roiSetSelector (value));
			break;

		//----------------------------------------------------------------------------------
		// ROI Height Size設定
		//----------------------------------------------------------------------------------
		case RoiHeightSize:
			// Get Selector
			if ((*status = toG (roiGetSelector ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Valid
			if ((*status = toG (roiGetValid ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			if (value3rd == 1)
			{
				// Set Height
				if ((*status = toG (aoiSetHeight (value))) != AVAL_STATUS_SUCCESS)
					break;

				// Total Height
				if ((*status = toG (fpgaRoiGetCameraHeightTotalSize ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
					break;

				// Height Size
				video_height = value3rd;
			}

			// Set Save Data
			roiHeightSizeGe[value2nd] = value;

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			update_trailer = 1;
			#endif

			break;
		
		//----------------------------------------------------------------------------------
		// ROI Height Offset設定
		//----------------------------------------------------------------------------------
		case RoiHeightOffset:

			// Get Selector
			if ((*status = toG (roiGetSelector ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Valid
			if ((*status = toG (roiGetValid ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			if (value3rd == 1)
			{
				// Set Offset
				if ((*status = toG (aoiSetHeightOffset (value))) != AVAL_STATUS_SUCCESS)
					break;

				// Total Height
				if ((*status = toG (fpgaRoiGetCameraHeightTotalSize ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
					break;

				// Height Size
				video_height = value3rd;
			}

			// Set Save Data
			roiHeightOffsetGe[value2nd] = value;

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			#endif

			break;

		//----------------------------------------------------------------------------------
		// ROI Height Valid設定
		//----------------------------------------------------------------------------------
		case RoiHeightValid:

			// Get Selector
			if ((*status = toG (roiGetSelector ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Valid
			if ((*status = toG (roiGetValid ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			// 現在無効?
			if (value3rd == 0)
			{
				// Valid有効要求?(現在無効の為、無効要求は何もしない)
				if (value == 1)
				{
					// Set Height Offset
					if (roiHeightOffsetGe[value2nd] != 0)
					{
						if ((*status = toG (aoiSetHeightOffset (roiHeightOffsetGe[value2nd]))) != AVAL_STATUS_SUCCESS)
							break;
					}

					// Set Height
					if ((*status = toG (aoiSetHeight (roiHeightSizeGe[value2nd]))) != AVAL_STATUS_SUCCESS)
						break;

					// Set Enable/Disable
					if (roiHeightSizeGe[value2nd] != 0)
					{
						if ((*status = toG (roiSetValid (value))) != AVAL_STATUS_SUCCESS)
							break;
					}
				}
			}
			else	// 現在有効
			{
				// Valid無効要求?(現在有効の為、有効要求は何もしない)
				if (value == 0)
				{
					// Get Valid Count(最後の１個でないかを確認)
					if ((*status = roiGetNoSelfValidCount ((int)value2nd, (int *)&value3rd)) != AVAL_STATUS_SUCCESS)
						break;

					if (value3rd == 0)
					{
						*status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
						cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, *status, "ROI cannot be disabled.\n");
						break;
					}

					// Set Valid
					*status = toG (roiSetValid (value));
				}
			}

			#if defined (MODE_CXP)
			if (gInterFaceID == INTERFACE_CXP)
			{
				// CXP Height Param設定
				#if !defined (MODE_CXP_MULTI_PORT)
				
				if ((*status = toG(cxpSetHeightParam (0))) != AVAL_STATUS_SUCCESS)
					break;

				#else // #if !defined (MODE_CXP_MULTI_PORT)

				if ((*status = toG(cxpGetPort ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
					break;

				if ((*status = toG(cxpSetHeightParam (value2nd))) != AVAL_STATUS_SUCCESS)
					break;

				#endif // #if !defined (MODE_CXP_MULTI_PORT)
			}
			#endif

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			update_trailer = 1;
			#endif

			break;

		//----------------------------------------------------------------------------------
		// ROI Area Mode
		//----------------------------------------------------------------------------------
		case FIRM_DATA_ROI_AREA_MODE_ADRS:
		{
#if defined (MODE_SPECTRUM)

			//----------------------------------------------------------------------------------
			// Set Mode
			//----------------------------------------------------------------------------------
			if  ((*status = toG (roiSetAreaFlag (value))) != AVAL_STATUS_SUCCESS)
				break;

			// ROI DMA Size
			if ((*status = toG (roiSetAreaSize (value))) != AVAL_STATUS_SUCCESS)
				break;

			//----------------------------------------------------------------------------------
			// Get Width Size
			//----------------------------------------------------------------------------------
			//if ((*status = toG (roiGetAreaWidthSize (value, &fpgaSize, &fpgaOffset, &sensorSize, &sensorOffset))) != AVAL_STATUS_SUCCESS)
				//break;

			//Set Width Size
			//if ((*status = toG(aoiSetWidth((int)fpgaSize))) != AVAL_STATUS_SUCCESS)
				//break;

			if ((*status = toG(aoiGetWidth((int*) &value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			video_width = value2nd;

			//Set Width Offset
			//if ((*status = toG (aoiSetWidthOffset((int)fpgaOffset))) != AVAL_STATUS_SUCCESS)
				//break;

			if ((*status = toG (aoiGetWidthOffset((int*)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			video_offs_x = value2nd;


			//----------------------------------------------------------------------------------
			// Get Height Size
			//----------------------------------------------------------------------------------
			//if ((*status = toG (roiGetAreaHeightSize (value, &fpgaSize, &fpgaOffset, &sensorSize, &sensorOffset))) != AVAL_STATUS_SUCCESS)
				//break;

			// Set Height Size
			//if ((*status = toG(aoiSetHeight((int)fpgaSize))) != AVAL_STATUS_SUCCESS)
				//break;

			//if ((*status = toG( aoiGetHeightOffset((int*)&value2nd))) != AVAL_STATUS_SUCCESS)
				//break;

			//video_offs_y = value2nd;


			if ((*status = toG( aoiGetHeight((int*)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			video_height = value2nd;
			
			// Set Height Offset
			//if ((*status = toG(aoiSetHeightOffset((int)fpgaOffset))) != AVAL_STATUS_SUCCESS)
				//break;

			if ((*status = toG( aoiGetHeightOffset((int*)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			video_offs_y = value2nd;

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			update_trailer = 1;
			#endif

			break;

#else // #if defined (MODE_SPECTRUM)

			int fpgaSize, fpgaOffset, sensorSize, sensorOffset;
			
			//----------------------------------------------------------------------------------
			// Set Mode
			//----------------------------------------------------------------------------------
			if  ((*status = toG (roiSetAreaFlag (value))) != AVAL_STATUS_SUCCESS)
				break;

			//----------------------------------------------------------------------------------
			// Get Width Size
			//----------------------------------------------------------------------------------
			if ((*status = toG (roiGetAreaWidthSize (value, &fpgaSize, &fpgaOffset, &sensorSize, &sensorOffset))) != AVAL_STATUS_SUCCESS)
				break;

			//Set Width Size
			if ((*status = toG(aoiSetWidth((int)fpgaSize))) != AVAL_STATUS_SUCCESS)
				break;

			if ((*status = toG(aoiGetWidth((int*) &value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			video_width = value2nd;

			//Set Width Offset
			if ((*status = toG (aoiSetWidthOffset((int)fpgaOffset))) != AVAL_STATUS_SUCCESS)
				break;

			if ((*status = toG (aoiGetWidthOffset((int*)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			video_offs_x = value2nd;


			//----------------------------------------------------------------------------------
			// Get Height Size
			//----------------------------------------------------------------------------------
			if ((*status = toG (roiGetAreaHeightSize (value, &fpgaSize, &fpgaOffset, &sensorSize, &sensorOffset))) != AVAL_STATUS_SUCCESS)
				break;

			// Set Height Size
			if ((*status = toG(aoiSetHeight((int)fpgaSize))) != AVAL_STATUS_SUCCESS)
				break;

			//if ((*status = toG( aoiGetHeightOffset((int*)&value2nd))) != AVAL_STATUS_SUCCESS)
				//break;

			//video_offs_y = value2nd;

			if ((*status = toG( aoiGetHeight((int*)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			video_height = value2nd;
			
			// Set Height Offset
			if ((*status = toG(aoiSetHeightOffset((int)fpgaOffset))) != AVAL_STATUS_SUCCESS)
				break;

			if ((*status = toG( aoiGetHeightOffset((int*)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			video_offs_y = value2nd;

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			update_trailer = 1;
			#endif

			break;
#endif // #if defined (MODE_SPECTRUM)
		}

		//----------------------------------------------------------------------------------
		// ROI Area Size設定
		//----------------------------------------------------------------------------------
		case FIRM_DATA_ROI_AREA_SIZE_ADRS:

			//value2nd = executeCommand(FIRM_CMD_ROI_AREA, CPU_CMD_SYNC_ON, (u32*) &value, 1);
			//*status = toG(value2nd);
		
			// Frame Rate High Speed Mode設定
			if ((*status = toG (roiSetAreaSize (value))) != AVAL_STATUS_SUCCESS)
				break;

			if (value == ROI_AREA_MODE_DMA_SIZE)
				value = 0;

			// Set Flag
			if  ((*status = toG (roiSetAreaFlag (value))) != AVAL_STATUS_SUCCESS)
				break;
		

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
		
			if (value == 1)
			{
				video_width = sensorWidth ();
				video_height = sensorHeight ();
			}
			else
			{
				video_width = WidthMax ();
				video_height = HeightMax ();
			}
		
			update_leader = 1;
			update_trailer = 1;
			#endif

			break;

		//----------------------------------------------------------------------------------
		// SensorTotalHeight設定
		//----------------------------------------------------------------------------------
		case SensorTotalHeight:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// ROI Height Default設定
		//----------------------------------------------------------------------------------
		case RoiHeightDefault:

#if defined (MODE_SPECTRUM)
			//if ((*status = executeCommandSpectrumDefaultY()) != AVAL_STATUS_SUCCESS)
				//break;

			// ROI DMA Size
			//if ((*status = toG(roiSetDefaultY ())) != AVAL_STATUS_SUCCESS)
				//break;

			value2nd = executeCommand(FIRM_CMD_SPECTRUM_DEFAULTY, CPU_CMD_SYNC_ON, 0, 0);
			if((*status = toG(value2nd)) != AVAL_STATUS_SUCCESS)
				break;

			// Set Save Data
			for (value2nd=0; value2nd < ROI_SELECTOR_COUNT; value2nd++)
			{
				roiHeightSizeGe[value2nd] = 0;
				roiHeightOffsetGe[value2nd] = 0;

				if (value2nd == 0)
					roiHeightValidGe[value2nd] = 1;
				else
					roiHeightValidGe[value2nd] = 0;
			}

			// Spectrum default mode 取得
			if((*status = toG(spectrumGetDefaultMode((int*)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			//if((*status = toG(aoiGetHeight((int*) &value3rd))) != AVAL_STATUS_SUCCESS)
				//break;

			if(value2nd == SPECTRUM_DEFAULTY_MODE_DEFAULT)
				value3rd = SPECTRUM_BAND_COUNT_ROI;
			else
				value3rd = HeightMax();

			roiHeightSizeGe[0] = value3rd;
			video_height = roiHeightSizeGe[0];

			// Set Offset
			//if ((*status = toG (aoiGetHeightOffset ((int*) &value3rd))) != AVAL_STATUS_SUCCESS)
				//break;

			if ((*status = toG (spectrumGetBandOffset ((int*) &value))) != AVAL_STATUS_SUCCESS)
				break;

			if(value2nd == SPECTRUM_DEFAULTY_MODE_DEFAULT)
				value3rd = value;
			else
				value3rd = 0;

			roiHeightOffsetGe[0] = value3rd;
			video_offs_y = roiHeightOffsetGe[0];

			//インデックス初期化
			SpectrumBand_Index = 0;

		#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
			if((*status = toG(bgfSetBandGainIndex())) != AVAL_STATUS_SUCCESS)
				break;

			//バンドインデックス設定
			//if((*status = toG(spectrumSetBandIndex())) != AVAL_STATUS_SUCCESS)
				//break;
		#endif
			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			update_trailer = 1;
			#endif

		break;

#else	// #if defined (MODE_SPECTRUM)
		
			if  ((*status = toG (roiSetDefaultY ())) != AVAL_STATUS_SUCCESS)
				break;

			#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
			if((*status = toG(bgfSetBandGainIndex())) != AVAL_STATUS_SUCCESS)
				break;

			//バンドインデックス設定
			if((*status = toG(spectrumSetBandIndex()) ) != AVAL_STATUS_SUCCESS)
				break;
			#endif

			// Set Save Data
			for (value2nd=0; value2nd < ROI_SELECTOR_COUNT; value2nd++)
			{
				roiHeightSizeGe[value2nd] = 0;
				roiHeightOffsetGe[value2nd] = 0;

				if (value2nd == 0)
					roiHeightValidGe[value2nd] = 1;
				else
					roiHeightValidGe[value2nd] = 0;
			}

			//インデックス初期化
			SpectrumBand_Index = 0;

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			update_trailer = 1;
			#endif

			break;
	
#endif // #if defined (MODE_SPECTRUM)

		//--------------------------------------------------------------------------------
		// PaddingX設定
		//--------------------------------------------------------------------------------
		case FPGA_AOI_XPAD_ADRS:
			*status = toG(aoiGetPad((int*) &value2nd, (int*) &value3rd));
			toG(aoiSetPad((int) value, (int) value3rd));
			break;

		//--------------------------------------------------------------------------------
		// PaddingY設定
		//--------------------------------------------------------------------------------
		case FPGA_AOI_YPAD_ADRS:
			*status = toG(aoiGetPad((int*) &value2nd, (int*) &value3rd));
			toG(aoiSetPad((int) value2nd, (int) value));
			break;

		
#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
		//--------------------------------------------------------------------------------
		// VideoChunkCtrl設定
		//--------------------------------------------------------------------------------
		case VideoChunkCtrl:
			value2nd = value >> 31;
			if (value2nd <= 1)
			{
				if (value2nd == 0)
					video_chunk_ctrl = video_chunk_ctrl & ~0x80000000;
				else
					video_chunk_ctrl = video_chunk_ctrl | 0x80000000;

				update_trailer = 1;
				update_leader  = 1;
			}
			else
			{
				*status = GEV_STATUS_INVALID_PARAMETER;
			}

			break;

		//--------------------------------------------------------------------------------
		// VideoChunkEnable設定
		//--------------------------------------------------------------------------------
		case VideoChunkEnable:
			if (value <= 1)
			{
				video_chunk_enable = value;
				update_trailer = 1;
				update_leader = 1;
			}
			else
			{
				*status = GEV_STATUS_INVALID_PARAMETER;
			}

			break;

#endif // #if defined (MODE_GIGE_10G) && defined (IF_GIGE)

		//--------------------------------------------------------------------------------
		// LineDelay設定
		//--------------------------------------------------------------------------------
		case LineDelay:
			//    Additional gap after each image line in pixels.
			if (value <= video_max_gap)
			{
				video_gap_x = value;
			}
			else
			{
				*status = GEV_STATUS_LOCAL_PROBLEM;
			}
			break;

		//--------------------------------------------------------------------------------
		// FrameDelay設定
		//--------------------------------------------------------------------------------
		case FrameDelay:
			if (value <= video_max_gap)
			{
				video_gap_y = value;
			}
			else
			{
				*status = GEV_STATUS_LOCAL_PROBLEM;
			}
			break;

		//--------------------------------------------------------------------------------
		// LinePitch設定
		//--------------------------------------------------------------------------------
#if !defined (MODE_GIGE_10G)
		case LinePitch:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;
#endif	
	
		//--------------------------------------------------------------------------------
		// PixelFormat設定
		//--------------------------------------------------------------------------------
		case FPGA_AOI_BITWIDTH_ADRS:
			switch (value)
			{
				case GVSP_PIX_MONO14:
					video_pixfmt = value;
					value2nd = FPGA_AOI_BITWIDTH_14BIT;
					intValue = 14;
					break;
				case GVSP_PIX_MONO12:
					video_pixfmt = value;
					value2nd = FPGA_AOI_BITWIDTH_12BIT;
					intValue = 12;
					break;
				case GVSP_PIX_MONO10:
					video_pixfmt = value;
					value2nd = FPGA_AOI_BITWIDTH_10BIT;
					intValue = 10;
					break;
				case GVSP_PIX_MONO8:
					video_pixfmt = value;
					intValue = 8;
					value2nd = FPGA_AOI_BITWIDTH_8BIT;
					break;
				default:
					*status = GEV_STATUS_INVALID_PARAMETER;
					value2nd = PIXEL_8BIT;
					break;
			}

			// CPU1 Boot Flag (0=起動時/1=通常動作時)
			value3rd = IN32 (FIRM_DATA_CPU1_BOOT_FLAG);

			if (value3rd != 0)
			{
				if (*status == 0)
				{
					*status = toG (aoiSetBitWidth (intValue));
				}
			}

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			//user_set_pixperclock(value);
			update_leader   = 1;
			update_trailer  = 1;
			#endif

			break;


		//====================================================================================
		//
		// Group: Acquisition Control registers of standard
		// Note:
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// Acquisition Start設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_START_ADRS:
			*status = toG(acquisitionStart());
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Abort設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_ABORT_ADRS:
			*status = acquisitionAbort();
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Stop設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_STOP_ADRS:
			*status = toG(acquisitionStop());
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Preset設定
		//--------------------------------------------------------------------------------
		case AcquisitionPreset:

			Acquisition_Preset = value;
			value2nd = IN32(GENICAM_ACQUISITION_RESET_ADRS);
			switch (Acquisition_Preset)
			{
				case AcquisitionPreset_Off:     // Off
					break;
				case AcquisitionPreset_Default:     // Default as factory-setting
					// Acquisition Stop
					acquisitionAbort ();		//Area None
					*status = toG(acquisitionReset());
					Acquisition_Preset = AcquisitionPreset_Off;
					break;
				case AcquisitionPreset_Mode1:     // Mode*
				case AcquisitionPreset_Mode2:     // Mode*
				case AcquisitionPreset_Mode3:     // Mode*
				case AcquisitionPreset_Mode4:     // Mode*
				case AcquisitionPreset_Mode5:     // Mode*
				case AcquisitionPreset_Mode6:     // Mode*
				case AcquisitionPreset_Mode7:     // Mode*
				case AcquisitionPreset_Mode8:     // Mode*
				case AcquisitionPreset_Mode9:     // Mode*
				case AcquisitionPreset_Mode10:    // Mode*
					break;
				default:
					// Reserved for future
					break;
			}

			break;

		//--------------------------------------------------------------------------------
		// Acquisition Mode設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_MODE_ADRS:
			*status = toG (acquisitionSetMode (value));
			video_acq_mode = value;
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Frame Count設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_FRAME_COUNT_ADRS:
			*status = toG (acquisitionSetFrameCount ((unsigned int )value));
			break;

		//----------------------------------------------------------------------------------
		// Acquisition Frame Rate設定(Rateバージョン)設定
		//----------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_FRAMERATE_ADRS:	//@@@1
		case AcquisitionFrameRate:
			dblValue = (double)value / 100;
	    	*status = toG( acquisitionSetFrameRate(dblValue) );
		
			// フレームレートを正式に変更してきたので、以下領域はクリア
			// この領域がクリアされるまでは、CXPのConnectionConfigの
			// 切り替え時は常にこのフレームレート／露光時時間を設定する
			OUT32 (FIRM_DATA_FRAME_RATE_DEFAULT, 0);
			OUT32 (FIRM_DATA_EXPOSURE_DEFAULT, 0);

			break;

		//--------------------------------------------------------------------------------
		// Acquisition Frame Rate関連設定
		//--------------------------------------------------------------------------------
		case AcquisitionFrameRateRawMax:
		case AcquisitionFrameRateRawMin:
		case AcquisitionFrameRateMax:
		case AcquisitionFrameRateMin:
		case AcquisitionFrameCountMin:
		case AcquisitionFrameCountMax:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// ExposureTime 露光時間(us単位)設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_EXPOSURE_ADRS:
			*status = toG(acquisitionSetExposure((unsigned int) value));

			// フレームレートを正式に変更してきたので、以下領域はクリア
			// この領域がクリアされるまでは、CXPのConnectionConfigの
			// 切り替え時は常にこのフレームレート／露光時時間を設定する
			OUT32 (FIRM_DATA_FRAME_RATE_DEFAULT, 0);
			OUT32 (FIRM_DATA_EXPOSURE_DEFAULT, 0);

			break;

		//--------------------------------------------------------------------------------
		// ExposureTime 露光時間 Min/Max(us単位)設定
		//--------------------------------------------------------------------------------
		case ExposureTimeMax:
		case ExposureTimeMin:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// ExposureTime 露光時間(us単位)設定
		//--------------------------------------------------------------------------------
		case ExposureAuto:
			ExposureTime_Auto = value;
			break;

		//--------------------------------------------------------------------------------
		// ExposureMode設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_EXPOSURE_MODE_ADRS:
			*status = toG(acquisitionSetExposureMode((unsigned int) value));
			break;

		//----------------------------------------------------------------------------------
		// Sensor Read Out時間 設定(us単位)設定
		//----------------------------------------------------------------------------------
		case SensorReadOutTime:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Test Pattern設定
		//--------------------------------------------------------------------------------
		case FPGA_AOI_TP_INDEX_ADRS:
			*status = toG (aoiSetPatternMain (value));
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Status Select設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_STATUS_SELECT_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Status設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_STATUS_ADRS:
			OUT32(address, (value & GENICAM_ACQUISITION_STATUS_SELECT_MASK));
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Trigger Select設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_TRG_SELECT_ADRS:
			*status = toG (acquisitionSetSelect (value));
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Trigger Mode設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_TRG_MODE_ADRS:
			*status = toG (acquisitionSetTrgMode (value));
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Software Trigger設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_SOFT_TRG_ADRS:
			*status = toG (acquisitionSetSoftTrg ());
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Trigger Source設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_TRG_SOURCE_ADRS:
			*status = toG (acquisitionSetTrgSource (value));
        	break;

		//--------------------------------------------------------------------------------
		// Acquisition Trigger Activation設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_TRG_ACTIVATION_ADRS:
			*status = toG (acquisitionSetTrgActivation (value));
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Trigger Delay設定
		//--------------------------------------------------------------------------------
		case GENICAM_ACQUISITION_TRG_DELAY_ADRS:
			*status = toG(acquisitionSetTrgDelay (value));
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Trigger Delay Min/Max設定
		//--------------------------------------------------------------------------------
		case TriggerDelayMin:
		case TriggerDelayMax:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Acquisition Frame Invalided Active Trg Count設定
		//--------------------------------------------------------------------------------
        case GENICAM_ACQUISITION_FRAME_INVALID_ACTIVE_TRG_CNT_ADRS:
	        *status = GEV_STATUS_WRITE_PROTECT;
           	break;

		//--------------------------------------------------------------------------------
		// Acquisition Trigger Reserved Mode設定
		//--------------------------------------------------------------------------------
        case GENICAM_ACQUISITION_TRG_RESERVE_MODE_ADRS:
			*status = toG(acquisitionSetTrgReserve (value));
           	break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgHighCount設定
		//----------------------------------------------------------------------------------
        case AcquisitionTrgHighCount:
			*status = GEV_STATUS_WRITE_PROTECT;
	      	break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgLowCount設定
		//----------------------------------------------------------------------------------
        case AcquisitionTrgLowCount:
			*status = GEV_STATUS_WRITE_PROTECT;
	      	break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgCountMode設定
		//----------------------------------------------------------------------------------
        //case AcquisitionTrgCountMode:
			//*status = toG (counterSetTrgMode (value));
			//break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgSignalCount設定
		//----------------------------------------------------------------------------------
        //case AcquisitionTrgSignalCount:
			//*status = GEV_STATUS_WRITE_PROTECT;
			//break;

		//----------------------------------------------------------------------------------
		// AcquisitionTrgImageCount設定
		//----------------------------------------------------------------------------------
        //case AcquisitionTrgImageCount:
			//*status = GEV_STATUS_WRITE_PROTECT;
			//break;

		//--------------------------------------------------------------------------------
		// Time(Debug)設定
		//--------------------------------------------------------------------------------
		case DebugTime:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;


		//====================================================================================
		//
		// Group: Spectrum Control registers of custom
		// Note:
		//
		//====================================================================================
#if defined (MODE_SPECTRUM)
		//--------------------------------------------------------------------------------
		// Spectrum Output Data Format設定
		//--------------------------------------------------------------------------------
			case FPGA_SPECTRUM_CTRL_ADRS:
			*status = toG (spectrumSetPixelFormat (FPGA_SP_CTRL_GET_FORMAT(value)));
			break;

		//--------------------------------------------------------------------------------
		// Spectrum Line Per Frame設定
		//--------------------------------------------------------------------------------
		case FPGA_SPECTRUM_LINE_PER_FRAME_ADRS:
			*status = toG (spectrumSetHeightPerBand (value));
			break;

		//--------------------------------------------------------------------------------
		// Spectrum Band Count設定
		//--------------------------------------------------------------------------------
		case SpectrumBandCount:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Spectrum Band Index設定
		//--------------------------------------------------------------------------------
		case SpectrumBandIndex:
			SpectrumBand_Index = value;
			break;

		//--------------------------------------------------------------------------------
		// Spectrum Band Peak Wave設定
		//--------------------------------------------------------------------------------
		case SpectrumBandValue:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Spectrum Band Peak All Wave設定
		//--------------------------------------------------------------------------------
		case SpectrumBandAllValue:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Spectrum band Width設定
		//--------------------------------------------------------------------------------
		case SpectrumBandWidth:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// SpectrumBandValueRange設定
		//--------------------------------------------------------------------------------
		case SpectrumBandValueRange:
			OUT32(FIRM_DATA_SPECTRUM_BANDVALUE_RANGE_ADRS, value);
			break;

		//----------------------------------------------------------------------------------
		// SpectrumBandDefaultMode設定
		//----------------------------------------------------------------------------------
		case SpectrumBandDefaultMode:
			*status = toG(spectrumSetDefaultMode (value));
			break;

#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
		//----------------------------------------------------------------------------------
		// Overlap設定
		//----------------------------------------------------------------------------------
		case SpectrumOverlap:
			*status = toG(bgfSetOverlapMode((int) value));
			break;
#endif


#if defined (MODE_SPECTRUM_BAND_SELECT)
		//----------------------------------------------------------------------------------
		// Band Wave設定
		//----------------------------------------------------------------------------------
		case SpectrumBandWave:
			fltValue = (float)((float)value / (float)SPECTRUM_BAND_WAVE_UNIT);
			SpectrumBand_Wave = fltValue;
			break;

		//----------------------------------------------------------------------------------
		// Band Wave Index設定
		//----------------------------------------------------------------------------------
		case SpectrumBandWaveToIndex:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// Band Start/End設定(Height方向版)
		//----------------------------------------------------------------------------------
		case SpectrumBandStart:
		case SpectrumBandEnd:

	    	if((*status = toG(spectrumGetDefaultMode ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
	    		break;

	    	// Fullsizeならスキップ
	    	if(value2nd == SPECTRUM_DEFAULTY_MODE_FULL_SIZE)
	    		break;

			// Get Selector
			if ((*status = toG (roiGetSelector ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Valid
			if ((*status = toG (roiGetValid ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			value4th = 0;
		
			// Valid有効?
			//if (value3rd == 1)
			{
				// 波長情報を少数に変換
				dblValue = value / SPECTRUM_BAND_WAVE_UNIT;

				// Valueが0の場合はクリアの為、波長情報は取得しない
				if (value != 0)
				{
					// Offset/Size取得
					if ((*status = toG (spectrumBandIndex2 (dblValue, (int *)&value))) != AVAL_STATUS_SUCCESS)
						break;

					if((*status = toG(spectrumGetBandOffset((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
						break;

					// Start
					if (address == SpectrumBandStart)
					{
						// 波長上限下限確認
						if(value < value3rd || value > (value3rd + SPECTRUM_BAND_COUNT_ROI))
							value = value3rd; //410nmに設定
						
						// Get Offset
						if ((*status = toG (aoiGetHeightOffset ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
							break;

						// Get Size
						if ((*status = toG (aoiGetHeight ((int *)&value4th))) != AVAL_STATUS_SUCCESS)
							break;

						// Height Sizeが0ならばとりあえず1にする
						if (value4th <= 0)
							value4th = 1;

						// 現在の終了位置
						value3rd += value4th;


						#if defined(MODE_CXP)
						if(value % CXP_HEIGHT_ALIGH != 0)
						{
							value &= ~(CXP_HEIGHT_ALIGH-1);
						}
						#endif // #if defined(MODE_CXP)
						
						// 新しい終了位置
						intValue = value3rd - value;

						// プラスだったら計算。(前のサイズと同じにしたいため)
						// マイナスだったらvalue4th(Heightサイズ)をそのまま使用
						if (intValue > 0)
							value4th = intValue;

						// Set Offset
						if ((*status = toG (aoiSetHeightOffset (value))) != AVAL_STATUS_SUCCESS)
							break;

						#if defined(MODE_CXP)
						if(value4th % CXP_HEIGHT_ALIGH != 0)
						{
							value4th &= ~(CXP_HEIGHT_ALIGH-1);
						}
						#endif

						// Set Size
						if ((*status = toG (aoiSetHeight (value4th))) != AVAL_STATUS_SUCCESS)
							break;
					}
					// End
					else
					{
						value++;

						// Get Offset
						if ((*status = toG (aoiGetHeightOffset ((int *)&value3rd)) )!= AVAL_STATUS_SUCCESS)
							break;

						value4th = value - value3rd;

						#if defined(MODE_CXP)
						if(value4th % CXP_HEIGHT_ALIGH != 0)
							value4th &= ~(CXP_HEIGHT_ALIGH-1);
						#endif

						// Set Height
						if ((*status = toG (aoiSetHeight (value4th))) != AVAL_STATUS_SUCCESS)
							break;
					}
				}
				else
				{

					if (address == SpectrumBandStart)
					{
						// Set Offset
						if ((*status = toG (aoiSetHeightOffset (value))) != AVAL_STATUS_SUCCESS)
							break;
					}
					else
					{
						// Set Height
						if ((*status = toG (aoiSetHeight (value))) != AVAL_STATUS_SUCCESS)
							break;
					}

					// 以降で使用するため、0に初期化
					value3rd = 0;
				}
			}

#if defined (MODE_SPECTRUM_MULTI_HEIGHT)
			// Total Height
			if ((*status = toG (fpgaRoiGetCameraHeightTotalSize ((int *)&intValue))) != AVAL_STATUS_SUCCESS)
				break;
#else
			// Set Height
			if ((*status = toG (aoiGetHeight (&intValue))) != AVAL_STATUS_SUCCESS)
				break;
#endif

			// Height Size
			video_height = intValue;

			// Get Offset
			if ((*status = toG (aoiGetHeightOffset ((int *)&intValue))) != AVAL_STATUS_SUCCESS)
				break;

			video_offs_y = intValue;

			// Set Save Data
			if (address == SpectrumBandStart)
			{
				roiHeightOffsetGe[value2nd] = video_offs_y;
				roiHeightSizeGe[value2nd] = video_height;
			}
			else
			{
				//roiHeightSizeGe[value2nd] = value - value3rd;
				roiHeightSizeGe[value2nd] = video_height;
			}

			//バンドインデックス設定
			if((*status = toG(spectrumSetBandIndex())) != AVAL_STATUS_SUCCESS)
				break;


			#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
			//インデックス設定
			if((*status = toG (bgfSetBandGainIndex())) != AVAL_STATUS_SUCCESS)
				break;

			//ROI最大バンド数取得
			if((*status = toG (spectrumBandROIMaxCount((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			//Overlap取得
			if((*status = toG (bgfGetOverlapMode((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			//有効時
			if(value3rd == MODE_ENABLE)
			{
				//トータル3Line以下ならエラー
				if(value2nd < SPECTRUM_YFLITER_HEIGHT_MIN || value2nd > SPECTRUM_BAND_COUNT)
				{
					bgfSetOverlapMode(MODE_DISABLE);
					break;
				}
			}
			#endif

			
			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			update_trailer = 1;
			#endif

			break;


			//----------------------------------------------------------------------------------
			// Spectrum Band Valid設定
			//----------------------------------------------------------------------------------
			case SpectrumBandValid:

			// Set Valid
			if ((*status = toG (roiSetValid (value))) != AVAL_STATUS_SUCCESS)
				break;

			// Get Selector
			if ((*status = toG (roiGetSelector ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// Vallid Save
			roiHeightValidGe[value2nd] = value;

			//バンドインデックス設定
			if((*status = toG(spectrumSetBandIndex())) != AVAL_STATUS_SUCCESS)
				break;


		#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)

			//インデックス設定
			if((*status = toG(bgfSetBandGainIndex())) != AVAL_STATUS_SUCCESS)
				break;

			//ROI最大バンド数取得
			if((*status = toG(spectrumBandROIMaxCount((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			//Overlap取得
			if((*status = toG(bgfGetOverlapMode((int *)&value4th))) != AVAL_STATUS_SUCCESS)
				break;

			//有効時
			if(value4th == MODE_ENABLE)
			{
				//トータル3Line以下ならエラー
				if(value3rd < SPECTRUM_YFLITER_HEIGHT_MIN || value3rd > SPECTRUM_BAND_COUNT)
				{
					bgfSetOverlapMode(MODE_DISABLE);

					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_INVALID_PARAMETER);
					sprintf (gLogMsgBuff, "Overlap size(%d) Parameter Error. (Min=%d / Max=%d)\n", value3rd, SPECTRUM_YFLITER_HEIGHT_MIN, SPECTRUM_BAND_COUNT);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);
					break;
				}
			}
		#endif

			break;

	#endif // #if defined (MODE_SPECTRUM_BAND_SELECT)

#endif // #if defined (MODE_SPECTRUM)


		//====================================================================================
		//
		// Group: Device Control registers of standard
		// Note:
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// DeviceVendorNameOnEEPROM設定
		//--------------------------------------------------------------------------------
		case DeviceVendorNameOnEEPROM:
		case DeviceVendorNameOnEEPROM + 4:
		case DeviceVendorNameOnEEPROM + 8:
		case DeviceVendorNameOnEEPROM + 12:
		case DeviceVendorNameOnEEPROM + 16:
		case DeviceVendorNameOnEEPROM + 20:
		case DeviceVendorNameOnEEPROM + 24:
		case DeviceVendorNameOnEEPROM + 28:
			value2nd = SWAP_L(value);
			memcpy((void*) (FIRM_DATA_VENDOR_ADRS + (address - DeviceVendorNameOnEEPROM)), &value2nd, 4);
			if ((DeviceVendorNameOnEEPROM + 28) == address)
			{
				//executeCommandVendorName((u8*) FIRM_DATA_VENDOR_ADRS);
				
				// Cmd Initialze
				if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
					break;

				memset(deviceVendorName, 0, GIGE_EEPROM_CUST_VENDOR_SIZE);
				sprintf((char*) deviceVendorName, "%s", (char*) value);

				// Set Vendor
				if ((*status = toG (setVendor ((char *)FIRM_DATA_VENDOR_ADRS))) != AVAL_STATUS_SUCCESS)
					break;
			}
			break;

		//--------------------------------------------------------------------------------
		// DeviceModelNameOnEEPROM設定
		//--------------------------------------------------------------------------------
		case DeviceModelNameOnEEPROM:
		case DeviceModelNameOnEEPROM + 4:
		case DeviceModelNameOnEEPROM + 8:
		case DeviceModelNameOnEEPROM + 12:
		case DeviceModelNameOnEEPROM + 16:
		case DeviceModelNameOnEEPROM + 20:
		case DeviceModelNameOnEEPROM + 24:
		case DeviceModelNameOnEEPROM + 28:
			value2nd = SWAP_L(value);
			memcpy((void*) (FIRM_DATA_MODEL_ADRS + (address - DeviceModelNameOnEEPROM)),&value2nd,4);
			if ((DeviceModelNameOnEEPROM + 28) == address)
			{
				//executeCommandModelName((u8*) FIRM_DATA_MODEL_ADRS);
				
				// Cmd Initialze
				if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
					break;
				
				memset(deviceModelName, 0, GIGE_EEPROM_CUST_MODEL_SIZE);
				sprintf((char*) deviceModelName, "%s", (char*) value);

				// Set Model
				if ((*status = toG (setModel ((char *)FIRM_DATA_MODEL_ADRS))) != AVAL_STATUS_SUCCESS)
					break;
			}
			break;

		//--------------------------------------------------------------------------------
		// DeviceManufacturerInfoOnEEPROM設定
		//--------------------------------------------------------------------------------
		case DeviceManufacturerInfoOnEEPROM:
		case DeviceManufacturerInfoOnEEPROM + 4:
		case DeviceManufacturerInfoOnEEPROM + 8:
		case DeviceManufacturerInfoOnEEPROM + 12:
		case DeviceManufacturerInfoOnEEPROM + 16:
		case DeviceManufacturerInfoOnEEPROM + 20:
		case DeviceManufacturerInfoOnEEPROM + 24:
		case DeviceManufacturerInfoOnEEPROM + 28:
		case DeviceManufacturerInfoOnEEPROM + 32:
		case DeviceManufacturerInfoOnEEPROM + 36:
		case DeviceManufacturerInfoOnEEPROM + 40:
		case DeviceManufacturerInfoOnEEPROM + 44:
			value2nd = SWAP_L(value);
			memcpy((void*) (FIRM_DATA_MANUFACTURE_ADRS + (address - DeviceManufacturerInfoOnEEPROM)), &value2nd, 4);
			if ((DeviceManufacturerInfoOnEEPROM + 44) == address)
			{
				//executeCommandManufactureInfo((u8*) FIRM_DATA_MANUFACTURE_ADRS);
				
				// Cmd Initialze
				if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
					break;
			
				memset(deviceManufacturerInfo, 0, GIGE_EEPROM_CUST_MANUFACTURER_SIZE);
				sprintf((char*) deviceManufacturerInfo, "%s", (char*) value);

				// Set Manufacture
				if ((*status = toG (setManufacture ((char *)FIRM_DATA_MANUFACTURE_ADRS))) != AVAL_STATUS_SUCCESS)
					break;
			}
			break;

		//----------------------------------------------------------------------------------
		// DeviceVendoroWriteCmd設定
		//----------------------------------------------------------------------------------
		case DeviceVendoroWriteCmd:
			//*status = executeCommand (FIRM_CMD_CAMERA_VENDOR, CPU_CMD_SYNC_OFF, 0, 0);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			memset(deviceVendorName, 0, GIGE_EEPROM_CUST_VENDOR_SIZE);
			sprintf((char*) deviceVendorName, "%s", (char*) value);

			// Set Vendor
			if ((*status = toG (setVendor ((char *)FIRM_DATA_VENDOR_ADRS))) != AVAL_STATUS_SUCCESS)
				break;

			break;

		//----------------------------------------------------------------------------------
		// DeviceModelWriteCmd設定
		//----------------------------------------------------------------------------------
		case DeviceModelWriteCmd:
			//*status = executeCommand (FIRM_CMD_CAMERA_MODEL, CPU_CMD_SYNC_OFF, 0, 0);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			memset(deviceModelName, 0, GIGE_EEPROM_CUST_MODEL_SIZE);
			sprintf((char*) deviceModelName, "%s", (char*) value);

			// Set Model
			if ((*status = toG (setModel ((char *)FIRM_DATA_MODEL_ADRS))) != AVAL_STATUS_SUCCESS)
				break;

			break;

		//----------------------------------------------------------------------------------
		// DeviceManufacturerInfoWriteCmd設定
		//----------------------------------------------------------------------------------
		case DeviceManufacturerInfoWriteCmd:
			//*status = executeCommand (FIRM_CMD_CAMERA_MANUFACTURE, CPU_CMD_SYNC_OFF, 0, 0);
		
			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			memset(deviceManufacturerInfo, 0, GIGE_EEPROM_CUST_MANUFACTURER_SIZE);
			sprintf((char*) deviceManufacturerInfo, "%s", (char*) value);

			// Set Manufacture
			if ((*status = toG (setManufacture ((char *)FIRM_DATA_MANUFACTURE_ADRS))) != AVAL_STATUS_SUCCESS)
				break;

			break;

		//--------------------------------------------------------------------------------
		// DeviceVersion設定
		//--------------------------------------------------------------------------------
		case DeviceVersion:
		case DeviceVersion + 4:
		case DeviceVersion + 8:
		case DeviceVersion + 12:
		case DeviceVersion + 16:
		case DeviceVersion + 20:
		case DeviceVersion + 24:
		case DeviceVersion + 28:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceFirmwareVersion設定
		//--------------------------------------------------------------------------------
		case DeviceFirmwareVersion:
		case DeviceFirmwareVersion + 4:
		case DeviceFirmwareVersion + 8:
		case DeviceFirmwareVersion + 12:
		case DeviceFirmwareVersion + 16:
		case DeviceFirmwareVersion + 20:
		case DeviceFirmwareVersion + 24:
		case DeviceFirmwareVersion + 28:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceFirmwareTimestamp
		//--------------------------------------------------------------------------------
		case DeviceFirmwareTimestamp:
		case DeviceFirmwareTimestamp + 4:
		case DeviceFirmwareTimestamp + 8:
		case DeviceFirmwareTimestamp + 12:
		case DeviceFirmwareTimestamp + 16:
		case DeviceFirmwareTimestamp + 20:
		case DeviceFirmwareTimestamp + 24:
		case DeviceFirmwareTimestamp + 28:
			valueBuffer = (u32*) (((char*) firmUpdate) + (address & ~DeviceFirmwareTimestamp));
			value2nd = SWAP_L(value);
			memcpy((void*) valueBuffer, (const u8*) &value2nd, 4);
			if (DeviceFirmwareTimestamp + 28 == address)
			{
				for (i = 0; i < GIGE_EEPROM_CUST_UPDATE_TIMESTAMP_SIZE; i++)
				{
					eeprom_write_byte((u16) GIGE_EEPROM_CUST_UPDATE_TIMESTAMP_ADRS + i, firmUpdate[i]);
				}
			}
			break;

		//--------------------------------------------------------------------------------
		// DeviceUserID設定
		//--------------------------------------------------------------------------------
		case DeviceUserID:
		case DeviceUserID + 4:
		case DeviceUserID + 8:
		case DeviceUserID + 12:
			valueBuffer = (u32*) (((char*) deviceUserID) + (address & ~DeviceUserID));
			memcpy((u8*) valueBuffer, (const u8*) &value, 4);
			*status = toG(setUserId(deviceUserID));
			break;

		//--------------------------------------------------------------------------------
		// Device Sensor ID設定
		//--------------------------------------------------------------------------------
		case DeviceSensorID:
		case DeviceSensorID + 4:
		case DeviceSensorID + 8:
		case DeviceSensorID + 12:
		case DeviceSensorID + 16:
		case DeviceSensorID + 20:
		case DeviceSensorID + 24:
		case DeviceSensorID + 28:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

#if defined (MODE_XML_SCHEMA_VERSION)
		//----------------------------------------------------------------------------------
		// DeviceXMLSchmaVersionMode設定
		//----------------------------------------------------------------------------------
		case DeviceXMLSchmaVersionMode:
		{
			char *shemaStr = "?SchemaVersion=1.0.0";
			char *shemaPtr = (char *)shemaStr;
			int shemaLen = strlen (shemaStr);

			for (address2nd = 0; address2nd < GIGE_EEPROM_XML_URL_SIZE; address2nd++)
			{
				// URL取得
				value2nd = eeprom_read_byte((GIGE_EEPROM_XML_URL1_REG + address2nd));

				// ?SchemaVersion=1.0.0 検索(0=?SchemaVersion=1.0.0なし/1=?SchemaVersion=1.0.0あり※標準)
				if ((value2nd == '?') || (value2nd == 0))
				{
					if (value == 0)		// SchmaVersion削除(?SchemaVersion=1.0.0が有れば削除=>?をNULLに置き換え。無い場合は無いので何もしない)
					{
						if (value2nd == '?')
						{
							// NULL設定
							eeprom_write_byte((GIGE_EEPROM_XML_URL1_REG + address2nd), 0x00);
							eeprom_write_byte((GIGE_EEPROM_XML_URL2_REG + address2nd), 0x00);
						}
					}
					else				// SchmaVersion追加(?SchemaVersion=1.0.0が有ろうが無かろうが追加)
					{
						// ポジションCheck
						value3rd = GIGE_EEPROM_XML_URL_SIZE - address2nd;

						if (value3rd >= shemaLen)
						{
							// ?SchemaVersion=1.0.0設定
							for (address3rd = 0; address3rd < shemaLen; address3rd++, shemaPtr++)
							{
								eeprom_write_byte((GIGE_EEPROM_XML_URL1_REG + address2nd + address3rd), *shemaPtr);
								eeprom_write_byte((GIGE_EEPROM_XML_URL2_REG + address2nd + address3rd), *shemaPtr);
							}
						}
					}

					// value設定
					OUT32 (FIRM_DATA_XML_SCHEMA_VERSION_MODE_ADRS, value);
				
					break;
				} // if ((value2nd == '?') || (value2nd == 0))
			} // for (address2nd = 0; address2nd < GIGE_EEPROM_XML_URL_SIZE; address2nd++)

			break;
		}
#endif // #if defined (MODE_XML_SCHEMA_VERSION)

		//----------------------------------------------------------------------------------
		// DeviceIfFpgaSelector設定
		//----------------------------------------------------------------------------------
		case DeviceIfFpgaSelector:
			gDeviceIfUpdateSelector = value;
			break;


		//====================================================================================
		//
		// Group: TransportLayerControl
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// PayloadSize設定
		//--------------------------------------------------------------------------------
		case PayloadSize:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Packet size margins設定
		//--------------------------------------------------------------------------------
		case SCPSPacketSizeMin:
		case SCPSPacketSizeMax:
		case SCPSPacketSizeInc:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

#if defined (IF_GIGE) && defined (MODE_GIGE_10G)
		//--------------------------------------------------------------------------------
		// Packet size margins設定
		//--------------------------------------------------------------------------------
		case SCPSPacketSizeMin2:  	//SCPS_MIN
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// SCPS_MAX設定
		//--------------------------------------------------------------------------------
		case SCPSPacketSizeMax2:    //SCPS_MAX
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// SCPS_INC設定
		//--------------------------------------------------------------------------------
		case SCPSPacketSizeInc2:    //SCPS_INC
			*status = GEV_STATUS_WRITE_PROTECT;
			break;
				
		//----------------------------------------------------------------------------------
		// GevLinkSpeed設定
		//----------------------------------------------------------------------------------
		case GevLinkSpeed:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;
#endif // #if defined (IF_GIGE) && defined (MODE_GIGE_10G)


#if defined (MODE_GE_SPEED)
		//----------------------------------------------------------------------------------
		// GevLinkSpeedConfig設定
		//----------------------------------------------------------------------------------
		case GevLinkSpeedConfig:
			*status = toG (gevSetSpeedConfig (value));
			break;
#endif // #if defined (MODE_GE_SPEED)


		//====================================================================================
		//
		// Group: UserSetControl
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// UserSetSelector設定
		//--------------------------------------------------------------------------------
		case UserSetSelector:
			Flash_UserSetSelector = value;
			
			#if !defined (MODE_LUT_NUMBER_FIX)
			LUTSelector_Selector = value;
			#endif

			break;

		//--------------------------------------------------------------------------------
		// UserSetLoad設定
		//--------------------------------------------------------------------------------
		case UserSetLoad:
			Flash_UserSetLoad = value;

			//if ((*status = executeCommandUserSetLoad(Flash_UserSetSelector)) != AVAL_STATUS_SUCCESS)
				//break;

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// Load
			if ((*status = toG (userSetLoad (value))) != AVAL_STATUS_SUCCESS)
				break;

		#if defined (IF_GIGE)
			#if defined (MODE_ACQUISITION_TRG_EXTEND) && defined (MODE_CAMERA_EVENT_VERSION2)
			if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
			{
				// GigE Eventパラメータ
				if ((*status = toG (gigeParamWriteRegister (value, CAMERA_SAVE_GIGE_ADRS, CAMERA_SAVE_GIGE_SIZE))) != AVAL_STATUS_SUCCESS)
					break;
			}
			#endif

			#if defined (MODE_GIGE_10G)
			if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
			{
				// Update Buffer
				if ((*status = toG (gigeUpdateBuffer ())) != AVAL_STATUS_SUCCESS)
					break;
			}
			#endif
		#endif // #if defined (IF_GIGE)

			break;

		//--------------------------------------------------------------------------------
		// UserSetSave設定
		//--------------------------------------------------------------------------------
		case UserSetSave:
			if (value > UserSet_Default)
			{
				Flash_UserSetSave = value;

				#if defined (IF_GIGE)
				#if defined (MODE_ACQUISITION_TRG_EXTEND)
				if ((*status = gigeParamSaveRegister (Flash_UserSetSelector, CAMERA_SAVE_GIGE_ADRS, CAMERA_SAVE_GIGE_SIZE)) != AVAL_STATUS_SUCCESS)
					break;
				#endif
				#endif // #if defined (IF_GIGE)

				//*status = executeCommandUserSetSave(Flash_UserSetSelector);
				
				// Cmd Initialze
				if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
					break;

				// UserSet Save
				if ((*status = toG (userSetSave (value))) != AVAL_STATUS_SUCCESS)
					break;
			}
			else
			{
				*status = GEV_STATUS_WRITE_PROTECT;
			}
			break;

		//--------------------------------------------------------------------------------
		// UserSetFactory設定
		//--------------------------------------------------------------------------------
		case UserSetFactory:
			if (value > UserSet_Default)
			{
				#if defined (MODE_ACQUISITION_TRG_EXTEND)
 				if ((*status = gigeParamDefault ()) != AVAL_STATUS_SUCCESS)
 					break;
				#endif
				
				//*status = executeCommandUserSetFactory(Flash_UserSetSelector);
				
				// Cmd Initialze
				if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
					break;

				// UserSet Default
				if ((*status = toG (userSetDefault (value))) != AVAL_STATUS_SUCCESS)
					break;
			}
			else
			{
				*status = GEV_STATUS_WRITE_PROTECT;
			}
			break;

		//--------------------------------------------------------------------------------
		// UserSetDefault設定
		//--------------------------------------------------------------------------------
		case UserSetDefault:
			Flash_UserSetDefault = value;
			CAMERA_SAVE_USER_NUM = Flash_UserSetDefault;

			#if defined (MODE_ACQUISITION_TRG_EXTEND)
 			if ((*status = gigeParamDefault ()) != AVAL_STATUS_SUCCESS)
 				break;
			#endif
		
			//executeCommandUserSetDefault(Flash_UserSetDefault);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// UserSet Boot
			*status = toG (userSetBoot (value));
			break;


		//====================================================================================
		//
		// Group: LUTControl
		//
		//====================================================================================
#if defined (MODE_LUT)
		//--------------------------------------------------------------------------------
		// LUTSelector設定
		//--------------------------------------------------------------------------------
		case LUTSelector:
			LUTSelector_Selector = value;
			break;

		//--------------------------------------------------------------------------------
		// LUTIndex設定
		//--------------------------------------------------------------------------------
		case LUTIndex:
			LUTIndex_Index = value;
			break;

		//--------------------------------------------------------------------------------
		// LUTIndex_Max設定
		//--------------------------------------------------------------------------------
		case LUTIndex_Max:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// FPGA_LUT_CTRL_ADRS設定
		//--------------------------------------------------------------------------------
		case FPGA_LUT_CTRL_ADRS:
			if (LUTSelector_Selector == UserSet_Default)
			{
				// Chose LUT1 corresponding to UserSet1 or UserSet2
				*status = toG(lutSetEnable((int) LUT_SELECT1, (int) value));
			}
			else
			{
				// Chose LUT1 or LUT2 corresponding to UserSet1 or UserSet2
				*status = toG(lutSetEnable((int) LUTSelector_Selector, (int) value));
			}
			break;

		//--------------------------------------------------------------------------------
		// LUTFormat設定
		//--------------------------------------------------------------------------------
		case LUTFormat:
			#if !defined (MODE_LUT_NUMBER_FIX)
			if (value > LUT_FORMAT_MIN)
			#endif
			{
				*status = toG(lutSetFormat((int) LUTSelector_Selector , (int) value));
			}
			LUTFormat_Selector = value;
			break;

		//--------------------------------------------------------------------------------
		// LUTThreshold設定
		//--------------------------------------------------------------------------------
		case LUTThreshold:
			*status = toG(lutSetBinThreshold(LUTSelector_Selector , (int) value));
			break;

		//--------------------------------------------------------------------------------
		// LUTGamma設定
		//--------------------------------------------------------------------------------
		case LUTGamma:
			fltValue = (float)((float)value / (float)LUT_GAMMA_DATA_ADJUST);
			*status = toG( lutSetGamma (LUTSelector_Selector, fltValue));
			break;

		//--------------------------------------------------------------------------------
		// LUTBaseAdrs設定
		//--------------------------------------------------------------------------------
		case LUTBaseAdrs:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;
#endif // #if defined (MODE_LUT)


		//====================================================================================
		//
		// Group: DefectPixelCorrectionControl
		//
		//====================================================================================
#if defined (MODE_DPC)
		//--------------------------------------------------------------------------------
 		// DefectivePixelCorrectionSelector設定
 		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionSelector:
			DefectPixelCorrection_Selector = value;
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionLoad設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionLoad:
			//*status = executeCommandDPCLoad (DefectPixelCorrection_Selector);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// DPC Load
			if ((*status =toG (dpcLoad (DefectPixelCorrection_Selector))) != AVAL_STATUS_SUCCESS)
				break;
		
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionLoadAdmin設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionLoadAdmin:
			//*status = executeCommandDPCLoadAdmin (DefectPixelCorrection_Selector);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// DPC Load
			if ((*status = toG (dpcLoadAdmin (DefectPixelCorrection_Selector))) != AVAL_STATUS_SUCCESS)
				break;

			break;

		//--------------------------------------------------------------------------------
		// DefectPixelCorrection設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrection:
			*status = toG (dpcSetEnableMode (value));
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustmentState設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustmentState:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustment設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustment:

			// CPU0にCPU1からの制御だと伝えるために設定
			//if (gInterFaceID == INTERFACE_CXP)
				//firmSetCmdStatus (1);

			//CPU_CMD cmd = { 0 };
			//cmd.param0 = DefectPixelCorrection_Selector;	//DPC番号
			//cmd.param1 = FlatFieldCorrection_Selector;		//FFC番号
			//cmd.param2 = (int)(DefectPixelCorrection_NonUniform * DPC_NONUNIFORM_UNIT);
			//cmd.param3 = (int)(DefectPixelCorrection_Sd * DPC_SD_UNIT);

			//if (gInterFaceID == INTERFACE_CXP)
				//*status = executeCommand (FIRM_CMD_DPC2_FACTORY, CPU_CMD_SYNC_OFF, (u32*) &cmd, sizeof(CPU_CMD) / 4);
			//else 
				//*status = executeCommand (FIRM_CMD_DPC2_FACTORY, CPU_CMD_SYNC_ON, (u32*) &cmd, sizeof(CPU_CMD) / 4);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// 補正回数取得
			value2nd = dpcGetBatchCount ();

			if ((value2nd % 2) == 0)
			{
				// DPC1 Execute
				if ((*status = toG (gigeCmdDpc1Main (DefectPixelCorrection_Selector, FlatFieldCorrection_Selector))) != AVAL_STATUS_SUCCESS)
					break;
			}

			// DPC2  Execute
			if ((*status = toG (gigeCmdDpc2Main (DefectPixelCorrection_Selector, FlatFieldCorrection_Selector))) != AVAL_STATUS_SUCCESS)
				break;

			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionDetection設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionDetection:
			//*status = executeCommandDPCAdjustFactoryDetection(DefectPixelCorrection_Selector);
		
			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// DPC3  Execute
			if ((*status = toG (gigeCmdDpc3Main (DefectPixelCorrection_Selector))) != AVAL_STATUS_SUCCESS)
				break;
		
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionCount設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionCount:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionMax設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionMax:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelModify設定
		//--------------------------------------------------------------------------------
		case DefectivePixelModify:
			DefectivePixel_Modify = value;
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelIndex設定
		//--------------------------------------------------------------------------------
		case DefectivePixelIndex:
			DefectPixel_Index = value;
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCoordinateX設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCoordinateX:
			if (DefectivePixel_Modify == DefectivePixelModify_On)
			{
				DefectPixelCoordinate_X = value;
			}
			else
			{
				//*status = executeCommandDPCEdit(DefectPixel_Index,FIRM_CMD_DPC_GET_GRID,(u32*)&value,(u32*)&value2nd);

				// Cmd Initialze
				if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
					break;

				// DPC Get Grid
				if ((*status = toG (dpcGetMapInfoIndex2 (DefectPixel_Index, (int *)&value, (int *)&value2nd))) != AVAL_STATUS_SUCCESS)
					break;
			}

			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCoordinateY設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCoordinateY:
			if (DefectivePixel_Modify == DefectivePixelModify_On)
			{
				DefectPixelCoordinate_Y = value;
			}
			else
			{
				//*status = executeCommandDPCEdit(DefectPixel_Index,FIRM_CMD_DPC_GET_GRID,(u32*)&value2nd,(u32*)&value);

				// Cmd Initialze
				if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
					break;

				// DPC Get Grid
				if ((*status = toG (dpcGetMapInfoIndex2 (DefectPixel_Index, (int *)&value2nd, (int *)&value))) != AVAL_STATUS_SUCCESS)
					break;
			}
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelApply設定
		//--------------------------------------------------------------------------------
		case DefectivePixelApply:
			//*status = executeCommandDPCEdit(DefectPixelCorrection_Selector,FIRM_CMD_DPC_ADD_GRID,(u32*)&DefectPixelCoordinate_X,(u32*)&DefectPixelCoordinate_Y);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// DPC Get Grid
			if ((*status = toG (dpcAddGrid (DefectPixelCoordinate_X, DefectPixelCoordinate_Y))) != AVAL_STATUS_SUCCESS)
				break;

			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelRemove設定
		//--------------------------------------------------------------------------------
		case DefectivePixelRemove:
			//*status = executeCommandDPCEdit(DefectPixelCorrection_Selector,FIRM_CMD_DPC_DEL_GRID,(u32*)&DefectPixelCoordinate_X,(u32*)&DefectPixelCoordinate_Y);
		
			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// DPC Delete Grid
			if ((*status = toG (dpcDeleteGrid (DefectPixelCoordinate_X, DefectPixelCoordinate_Y))) != AVAL_STATUS_SUCCESS)
				break;
		
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelReset設定
		//--------------------------------------------------------------------------------
		case DefectivePixelReset:
			//*status = executeCommandDPCEdit(DefectPixelCorrection_Selector,FIRM_CMD_DPC_CLR_GRID,(u32*)&DefectPixelCoordinate_X,(u32*)&DefectPixelCoordinate_Y);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// DPC Delete Grid
			if ((*status = toG (dpcClearGrid (DefectPixelCorrection_Selector))) != AVAL_STATUS_SUCCESS)
				break;

			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionSave設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionSave:
			//*status = executeCommandDPCAdjustSave(DefectPixelCorrection_Selector);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// DPC Save
			if ((*status = toG (dpcSave (DefectPixelCorrection_Selector))) != AVAL_STATUS_SUCCESS)
				break;
		
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionSaveAdmin設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionSaveAdmin:
			//*status = executeCommandDPCAdjustSaveAdmin(DefectPixelCorrection_Selector);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// DPC Save
			if ((*status = toG (dpcSaveAdmin (DefectPixelCorrection_Selector))) != AVAL_STATUS_SUCCESS)
				break;

			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionFactory設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionFactory:
			//*status = executeCommandDPCFactory(DefectPixelCorrection_Selector);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// DPC Default
			if ((*status = toG (dpcDefault ())) != AVAL_STATUS_SUCCESS)
				break;

			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionDetectionAbort設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionDetectionAbort:
			//*status = executeCommandDPCAbort(DefectPixelCorrection_Selector);
			*status = toG (dpcClearBatchCount());
			break;

		//--------------------------------------------------------------------------------
		// DefectivePixelCorrectionMode設定
		//--------------------------------------------------------------------------------
		case DefectivePixelCorrectionMode:
			*status = toG (dpcSetAdjustMode (value));
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustFfcIndex設定
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustFfcIndex:
			 DefectPixelCorrection_AdjustFfcSelector = value;
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustBrightIndex設定
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustBrightIndex:
			DefectPixelCorrection_AdjustBrightSelector = value;
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustRate設定
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustRate:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			fltValue = (float)(1.0 / (float)value);
			fltValue *= ACQUISITION_FRAMERATE_UINIT;
			OUTF ((value2nd + FIRM_DATA_DPC_ADJUST_RATE_ADRS), (float)fltValue);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustExposure設定
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustExposure:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			OUT32 ((value2nd + FIRM_DATA_DPC_ADJUST_EXPOSURE_ADRS), value);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustTemperature設定
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustTemperature:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			intValue = (int)value;
			fltValue = (float)((float)intValue / (float)DEVICE_TEMP_UNIT);
			OUTF ((value2nd + FIRM_DATA_DPC_ADJUST_TEMP_ADRS), fltValue);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustFfcNumber設定
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustFfcNumber:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			OUT32 ((value2nd + FIRM_DATA_DPC_ADJUST_FFC_NUMBER_ADRS), value);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustStandardDeviation設定
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustStandardDeviation:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			value2nd += (FIRM_DATA_DPC_ADJUST_SD0_ADRS + DefectPixelCorrection_AdjustBrightSelector * 4);

			intValue = (int)value;
			fltValue = (float)((float)intValue / (float)DEVICE_SD_UNIT);
			OUTF (value2nd, fltValue);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustNonuniformity設定
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustNonuniformity:
			value2nd = FIRM_DATA_DPC_ADJUST_FFC0_ADRS + FIRM_DATA_DPC_ADJUST_FFC_SIZE * DefectPixelCorrection_AdjustFfcSelector;
			value2nd += (FIRM_DATA_DPC_ADJUST_NON_UNIFORM0_ADRS + DefectPixelCorrection_AdjustBrightSelector * 4);

			intValue = (int)value;
			fltValue = (float)((float)intValue / (float)DEVICE_NON_UNIFORM_UNIT);
			OUTF (value2nd, fltValue);
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionAdjustUpdate設定
		//----------------------------------------------------------------------------------
		case DefectivePixelCorrectionAdjustUpdate:
			*status = toG (dpcAdjustUpdate());
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionメモリアドレス設定
		//----------------------------------------------------------------------------------
		 case DefectivePixelCorrectionAdjustBaseAdrs:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionPixelCount設定
		//----------------------------------------------------------------------------------
		 case DefectivePixelCorrectionPixelCount:
			*status = toG (dpcSetDefectionCount (value));
			break;

		//----------------------------------------------------------------------------------
		// DefectivePixelCorrectionPixelStateCount設定
		//----------------------------------------------------------------------------------
		 case DefectivePixelCorrectionPixelStateCount:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

#endif // #if defined (MODE_DPC)


		//====================================================================================
		//
		// Group: FlatFiledCorrectionControl
		//
		//====================================================================================
#if defined (MODE_FFC)
		//--------------------------------------------------------------------------------
		// FlatFieldCorrectionSelector設定
		//--------------------------------------------------------------------------------
		case FlatFieldCorrectionSelector:
			*status = toG(ffcSetLoadNum((int) value));
			FlatFieldCorrection_Selector = value;
			break;

		//--------------------------------------------------------------------------------
		// FlatFieldCorrectionAdjustmentSelector設定
		//--------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustmentSelector:
			FlatFieldCorrectionAdjustment_Selector = value;
			break;

		//--------------------------------------------------------------------------------
		// FlatFieldCorrectionSetLoad設定
		//--------------------------------------------------------------------------------
		case FlatFieldCorrectionSetLoad:
			//*status = executeCommandFFCLoad(FlatFieldCorrection_Selector);
			//*status = toG(ffcGetLoadNum((int*) &FlatFieldCorrection_Selector));
		
			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// FFC Load
			if ((*status = toG (ffcLoadMain (FlatFieldCorrection_Selector, FFC_USER))) != AVAL_STATUS_SUCCESS)
				break;
		
			break;

		//--------------------------------------------------------------------------------
		// FlatFieldCorrectionSetSave設定
		//--------------------------------------------------------------------------------
		case FlatFieldCorrectionSetSave:
			switch (FlatFieldCorrectionAdjustment_Selector)
			{
				case 0:
					if (gInterFaceID == INTERFACE_CXP)
					{
						if (FlatFieldCorrection_Selector == 0)
						{
							*status = -1;
							break;
						}
					}

					//*status = executeCommandFFCSave(FlatFieldCorrection_Selector);

					// Cmd Initialze
					if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
						break;

					// FFC Save
					if ((*status = toG (gigeCmdFfcSaveMain (FlatFieldCorrection_Selector))) != AVAL_STATUS_SUCCESS)
						break;

					break;

				default:

					//*status = executeCommandFFCFactorySave(FlatFieldCorrection_Selector);

					// Cmd Initialze
					if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
						break;

					// FFC Save
					if ((*status = toG (gigeCmdFfcSaveMain (FlatFieldCorrection_Selector))) != AVAL_STATUS_SUCCESS)
						break;

					break;
			}
			break;

		//--------------------------------------------------------------------------------
		// FPGA_FFC_CTRL_ADRS設定
		//--------------------------------------------------------------------------------
		case FPGA_FFC_CTRL_ADRS:
			OUT32(address, value);

#if defined (MODE_IPU_MULTI)
			OUT32(FPGA_FFC2_CTRL_ADRS, value);
#endif

			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionBrightMode設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionBrightMode:

			// 取得
			if ((*status = toG (ffcGetMode ((int *)&value2nd, (int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			// 設定
			if ((*status = toG (ffcSetMode (value2nd, value))) != AVAL_STATUS_SUCCESS)
				break;
		
			break;

		//--------------------------------------------------------------------------------
		// FPGA_FFC_BLACK_TARGET_ADRS設定
		//--------------------------------------------------------------------------------
		case FPGA_FFC_BLACK_TARGET_ADRS:
			if ((*status = toG (ffcGetBitCalc ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			value *= value2nd;

			*status = toG (ffcSetBlackTarget (value));
			break;

		//--------------------------------------------------------------------------------
		// FlatFieldCorrectionBrightTarget設定
		//--------------------------------------------------------------------------------
		case FlatFieldCorrectionBrightTarget:
			if ((*status = toG (ffcGetBitCalc ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			value *= value2nd;

			FlatFieldCorrectionBrightAdjustment_Lumi = value;
			*status = toG (ffcSetWhiteTarget (value));
			break;

		//--------------------------------------------------------------------------------
		// FlatFieldCorrectionDarkAdjustment設定
		//--------------------------------------------------------------------------------
		case FlatFieldCorrectionDarkAdjustment:
			switch (FlatFieldCorrectionAdjustment_Selector)
			{
				case 0:
					if (FlatFieldCorrection_Selector > 0)
					{
						//*status = executeCommandFFCDarkAdjust(FlatFieldCorrection_Selector);

						// Cmd Initialze
						if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
							break;

						// FFC Black Adjust
						if ((*status = toG (gigeCmdFfcBlackMain (FlatFieldCorrection_Selector))) != AVAL_STATUS_SUCCESS)
							break;
					}
					else
					{
						*status = GEV_STATUS_WRITE_PROTECT;
					}
					break;
				default:
					//*status = executeCommandFFCFactoryDarkAdjust(FlatFieldCorrection_Selector);
				
					// Cmd Initialze
					if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
						break;

					// FFC Black Adjust
					if ((*status = toG (gigeCmdFfcBlackMain (FlatFieldCorrection_Selector))) != AVAL_STATUS_SUCCESS)
						break;
				
					break;
			}
			break;


		//--------------------------------------------------------------------------------
		// FlatFieldCorrectionBrightAdjustment設定
		//--------------------------------------------------------------------------------
		case FlatFieldCorrectionBrightAdjustment:
			switch (FlatFieldCorrectionAdjustment_Selector)
			{
				case 0:
					if (FlatFieldCorrection_Selector > 0)
					{
						//*status = executeCommandFFCBrightAdjust(FlatFieldCorrection_Selector);
						
						// Cmd Initialze
						if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
							break;

						// FFC White Adjust
						if ((*status = toG (gigeCmdFfcWhiteMain (FlatFieldCorrection_Selector))) != AVAL_STATUS_SUCCESS)
							break;
					}
					else
					{
						*status = GEV_STATUS_WRITE_PROTECT;
					}
					break;
				default:
					//*status = executeCommandFFCFactoryBrightAdjust(FlatFieldCorrection_Selector);
				
					// Cmd Initialze
					if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
						break;

					// FFC White Adjust
					if ((*status = toG (gigeCmdFfcWhiteMain (FlatFieldCorrection_Selector))) != AVAL_STATUS_SUCCESS)
						break;

					break;
			}
			break;


#if defined (MODE_FFC_SHADING_LINE)
		//----------------------------------------------------------------------------------
		// Flat Filed Correction Shading Lineレベル調整設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionShadinLinegAdjustment:
			switch (FlatFieldCorrectionAdjustment_Selector)
			{
				case 0:
					if (FlatFieldCorrection_Selector > 0)
					{
						//*status = executeCommandFFCShadingLineAdjust (FlatFieldCorrection_Selector);

						// Cmd Initialze
						if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
							break;

						// FFC White Adjust
						if ((*status = toG (gigeCmdFfcShadingLineMain (FlatFieldCorrection_Selector))) != AVAL_STATUS_SUCCESS)
							break;
					}
					else
					{
						*status = GEV_STATUS_WRITE_PROTECT;
					}
					break;
				default:
					//*status = executeCommandFFCShadingLineAdjust (FlatFieldCorrection_Selector);
				
					// Cmd Initialze
					if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
						break;

					// FFC White Adjust
					if ((*status = toG (gigeCmdFfcShadingLineMain (FlatFieldCorrection_Selector))) != AVAL_STATUS_SUCCESS)
						break;

					break;
			}
			break;
		
		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionWhiteGainX設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionWhiteGainX:
			dblValue = (double)((double)value/(double)FFC_WHITE_GAIN_UNIT);
			*status = ffcSetWhiteGainX (dblValue);
			break;
#endif // #if defined (MODE_FFC_SHADING_LINE)

		//--------------------------------------------------------------------------------
		// FlatFieldCorrectionFactory設定
		//--------------------------------------------------------------------------------
		case FlatFieldCorrectionFactory:
			//*status = executeCommandFFCFactory(FlatFieldCorrection_Selector);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// FFC Default
			if ((*status = toG (gigeCmdFfcDefault ())) != AVAL_STATUS_SUCCESS)
				break;
		
			break;

		//--------------------------------------------------------------------------------
		// FlatFieldCorrectionStatus設定
		//--------------------------------------------------------------------------------
		case FlatFieldCorrectionStatus:
			FlatFieldCorrection_Status = value;
			break;

		//--------------------------------------------------------------------------------
		// Flat Filed Correctionセレクタ設定
		//--------------------------------------------------------------------------------
		 case FlatFieldCorrectionCorrectionSelector:
			FlatFieldCorrectionModeSelector = value;
 			break;

		//--------------------------------------------------------------------------------
		// FlatFieldCorrectionSetCorrectionMode設定
		//--------------------------------------------------------------------------------
		 case FlatFieldCorrectionSetCorrectionMode:
			//*status = executeCommandFFCCorrectionMode(FlatFieldCorrectionModeSelector);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// FFC Defgaul
			if ((*status = toG (gigeCmdFfcCorecctionMode (value))) != AVAL_STATUS_SUCCESS)
				break;
		
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時の目標値設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustDarkTarget:
			OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_ADRS, value);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時の目標値設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustBrightTarget:
			OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_ADRS, value);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時のレート設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustDarkRate:
			fltValue = (float)(1.0 / (float)value);
			fltValue *= ACQUISITION_FRAMERATE_UINIT;
			OUTF (FIRM_DATA_FFC_ADJUST_BLACK_RATE_ADRS, (float)fltValue);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時のレート設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustBrightRate:
			fltValue = (float)(1.0 / (float)value);
			fltValue *= ACQUISITION_FRAMERATE_UINIT;
			OUTF (FIRM_DATA_FFC_ADJUST_WHITE_RATE_ADRS, (float)fltValue);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時の露光時間設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustDarkExposure:
			OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_EXP_ADRS, value);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時の露光時間設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustBrightExposure:
			OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_EXP_ADRS, value);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時の温度設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionDarkTemperature:
		case FlatFieldCorrectionAdjustDarkTemperature:
			intValue = (int)value;
			fltValue = (float)((float)intValue / (float)DEVICE_TEMP_UNIT);
			OUTF (FIRM_DATA_FFC_ADJUST_BLACK_TEMP_ADRS, fltValue);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時の温度設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionBrightTemperature:
		case FlatFieldCorrectionAdjustBrightTemperature:
			intValue = (int)value;
			fltValue = (float)((float)intValue / (float)DEVICE_TEMP_UNIT);
			OUTF (FIRM_DATA_FFC_ADJUST_WHITE_TEMP_ADRS, fltValue);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時のGain設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustDarkGainX:
			intValue = (int)value;
			fltValue = (float)((float)intValue / (float)DEVICE_GAIN_UNIT);
			OUTF (FIRM_DATA_FFC_ADJUST_BLACK_GAIN_ADRS, fltValue);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時のGain設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustBrightGainX:
			intValue = (int)value;
			fltValue = (float)((float)intValue / (float)DEVICE_GAIN_UNIT);
			OUTF (FIRM_DATA_FFC_ADJUST_WHITE_GAIN_ADRS, fltValue);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction黒レベル調整時のBit設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustDarkBit:
			OUT32 (FIRM_DATA_FFC_ADJUST_BLACK_BIT_ADRS, value);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction白レベル調整時のBit設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustBrightBit:
			OUT32 (FIRM_DATA_FFC_ADJUST_WHITE_BIT_ADRS, value);
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction調整データの更新設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionAdjustUpdate:
			//*status = executeCommandFFCAdjustUpdate();
			*status = toG (ffcAdjustUpdate());
			break;

		//----------------------------------------------------------------------------------
		// Flat Filed Correction メモリアドレス設定
		//----------------------------------------------------------------------------------
		 case FlatFieldCorrectionBaseAdrs:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionX座標設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionXGrid:
			if ((value >= 0) && (value < sensorWidth ()))
	        	FlatFieldCorrectionX_Index = value;
			else
				*status = GEV_STATUS_INVALID_PARAMETER;
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionY座標設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionYGrid:
			if ((value >= 0) && (value < sensorHeight ()))
	        	FlatFieldCorrectionY_Index = value;
			else
				*status = GEV_STATUS_INVALID_PARAMETER;
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionOffset設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionOffset:
			*status = toG (ffcSetOffsetData (0, FlatFieldCorrectionX_Index, FlatFieldCorrectionY_Index, value));
			break;

		//----------------------------------------------------------------------------------
		// FlatFieldCorrectionGain設定
		//----------------------------------------------------------------------------------
		case FlatFieldCorrectionGain:
			*status = toG (ffcSetGainData (0, FlatFieldCorrectionX_Index, FlatFieldCorrectionY_Index, value));
			break;

#endif // #if defined (MODE_FFC)


		//====================================================================================
		//
		// Group: DigitaolControl as AnalogControl
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// GainX Float設定
		//--------------------------------------------------------------------------------
		case GainxFloat:
			fltValue = (float)((float)value / (float)DEVICE_GAINX_UNIT);
			*status = toG (digitalSetGainX (fltValue));
			break;

		//--------------------------------------------------------------------------------
		// BlackLevel1設定
		//--------------------------------------------------------------------------------
		case FPGA_DOG_OFFSET1_ADRS:
			*status = toG(digitalSetOffset1((int) value));
			break;

		//--------------------------------------------------------------------------------
		// BlackLevel2設定
		//--------------------------------------------------------------------------------
		case FPGA_DOG_OFFSET2_ADRS:
			*status = toG(digitalSetOffset((int) value));
			break;

#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
		//--------------------------------------------------------------------------------
		// Sensor Gain設定
		//--------------------------------------------------------------------------------
		case SensorGainX:
			//value2nd = executeCommand(FIRM_CMD_SENSOR_GAIN, CPU_CMD_SYNC_ON, (u32*) &value, 1);
			//*status = toG(value2nd);
			fltValue = (float)((float)value / (float)SENSOR_GAIN_UNIT);
			*status = toG (sensorSetGainX (fltValue));
			break;
#endif


#if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)
		//--------------------------------------------------------------------------------
		// Sensor Conversion Gain設定
		//--------------------------------------------------------------------------------
		case SensorConversionGain:
			//value2nd = executeCommand(FIRM_CMD_SENSOR_CONVERSION_GAIN, CPU_CMD_SYNC_ON, (u32*) &value, 1);
			//*status = toG(value2nd);
			*status = toG (sensorSetConversionGain (value));
			break;
#endif // #if defined (MODE_SENSOR_IMX992) || defined (MODE_SENSOR_IMX993)


#if defined (MODE_SENSOR_GRADATION_COMPRESS)
		//--------------------------------------------------------------------------------
		// Sensor Gradation Compress 8Bit Convert設定
		//--------------------------------------------------------------------------------
		case SensorGradComp8BitConvert:
			if ((*status = toG(aoiGetBitWidth((int*) &value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			if (value2nd != 8)
				break;
				
			#if defined (MODE_FRAMERATE_HIGH_SPEED)
			// High Speed Mode取得
			if ((*status = toG (sensorGetFrameRateHighSpeedMode ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			// High Speed Mode時は変更付加
			if (value2nd == 1)
				break;
			#endif
			
			// Sensor Gradation Compress
			//cmd.param0 = value;
			//value2nd = executeCommand (FIRM_CMD_SENSOR_GRAD_8BIT_CONVERT, CPU_CMD_SYNC_ON, (u32*) &cmd,  sizeof(CPU_CMD) / 4);
			//*status = toG(value2nd);
			*status = toG (sensorSet8BitConvert (value));
			break;
#endif


		//====================================================================================
		//
		// Group: DigitalIOControl
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// LineSelector設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_SELECT_ADRS:
			*status = toG (digitalIoSetLineSelect (value));
			break;

		//--------------------------------------------------------------------------------
		// LineMode設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_MODE_ADRS:
			*status = toG (digitalIoSetMode (value));
			break;

		//--------------------------------------------------------------------------------
		// LineInverter設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_INVERTER_ADRS:
			*status = toG (digitalIoSetInverter (value));
			break;

		//--------------------------------------------------------------------------------
		// LineStatus設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_STATUS_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// LineStatusAll設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_STATUS_ALL_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// LineSource設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_SOURCE_ADRS:
#if 1
			*status = toG (digitalIoSetLineSource (value));
#else
			value2nd = IN32(GENICAM_DIGITAL_LINE_SELECT_ADRS);

			switch (value2nd)
			{
				case 0: // LineSelector = Line0
				case 1: // LineSelector = Line1
				case 2: // LineSelector = Line2
					// Off value should be 0xFF instead of 0x0 for XML LineSource's value.
					// This value always should be 0xFF as Off because of not supported LineSource with Lin0
					//value = (value==0x0)?0xFF:value;
					value = 0xFF;
					OUT32(address, value);
					break;
				default:
					OUT32(address, value);
					break;
			}
#endif
			break;

		//--------------------------------------------------------------------------------
		// LineFormat設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_LINE_FORMAT_ADRS:
			*status = toG (digitalIoSetLineFormat (value));
			break;

		//--------------------------------------------------------------------------------
		// LinePulseHigh設定
		//--------------------------------------------------------------------------------
		case LinePulseHigh:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// LinePulseLow設定
		//--------------------------------------------------------------------------------
		case LinePulseLow:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// GPIO de-buncer mask address設定
		//----------------------------------------------------------------------------------
		case  GENICAM_DIGITAL_LINE_DNF_ADRS:
				OUT32 (GENICAM_DIGITAL_LINE_DNF_ADRS, value);
				break;

		//--------------------------------------------------------------------------------
		// GPIO de-bouncer0設定
		//--------------------------------------------------------------------------------
		case FPGA_GPIO_DNF0_ADRS:

			// Get Line Selector
			address2nd = IN32 (GENICAM_DIGITAL_LINE_SELECT_ADRS);

			// DNF取得
			OUT32 ((GENICAM_DIGITAL_LINE0_DNF_ADRS + (GENICAM_DIGITAL_LINE_INTERVAL * address2nd)), value);

			break;

		//--------------------------------------------------------------------------------
		// UserOutputSelector設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER_SELECT_ADRS:
			*status = toG (digitalIoSetUserSelect (value));
			break;

		//--------------------------------------------------------------------------------
		// UserOutputValue設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER_VALUE_ADRS:
			*status = toG (digitalIoSetUserValue (value));
			break;

		//--------------------------------------------------------------------------------
		// UserOutputValueAll設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER_ALL_VALUE_ADRS:
			*status = toG (digitalIoSetAllValue (value));
			break;

		//--------------------------------------------------------------------------------
		// UserOutputValueAllMask設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER_MASK_ADRS:
			*status = toG (digitalIoSetUserMask (value));
			break;

		//--------------------------------------------------------------------------------
		// UserOutput0設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER0_OUTPUT_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// UserOutput1設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER1_OUTPUT_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// UserOutput2設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER2_OUTPUT_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// UserOutput3設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER3_OUTPUT_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// UserOutput4設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER4_OUTPUT_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// UserOutput5設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER5_OUTPUT_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// UserOutput6設定
		//--------------------------------------------------------------------------------
		case GENICAM_DIGITAL_USER6_OUTPUT_ADRS:
			OUT32(address, value);
			break;

		//====================================================================================
		//
		// Group: Counter Control registers of standard
		//
		//====================================================================================
#if defined (MODE_GENICAM_COUNTER)
		//--------------------------------------------------------------------------------
		// CounterSelector設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_SELECT_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterEventSource設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_SOURCE_ADRS:
			OUT32(address, value);
			*status =((IN32(GENICAM_COUNTER_EVENT_INVALID_SOURCE_ADRS)) == 0) ? 0 : GEV_STATUS_NOT_IMPLEMENTED;
			break;

		//--------------------------------------------------------------------------------
		// CounterEventActivation設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_EVENT_ACTIVE_ADRS:
			OUT32(address, value);
			*status =((IN32(GENICAM_COUNTER_EVENT_INVALID_ACTIVE_ADRS)) == 0) ? 0 : GEV_STATUS_NOT_IMPLEMENTED;
			break;

		//--------------------------------------------------------------------------------
		// CounterResetSource設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_SOURCE_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterResetActivation設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_ACTIVE_ADRS:
			OUT32(address, value);
			*status = ((IN32(GENICAM_COUNTER_RESET_INVALID_ACTIVE_ADRS)) == 0) ? 0 : GEV_STATUS_NOT_IMPLEMENTED;
			break;

		//--------------------------------------------------------------------------------
		// CounterReset設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterValue設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterValueAtReset設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_VALUE_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterResetDuration設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_DURATION_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterResetStatus設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_STATUS_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterTriggerSource設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_TRG_SOURCE_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterTriggerActivation設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_TRG_ACTIVE_ADRS:
			OUT32(address, value);
			*status = ((IN32(GENICAM_COUNTER_TRG_INVALID_ACTIVE_ADRS)) == 0) ? 0 : GEV_STATUS_NOT_IMPLEMENTED;
			break;

		//--------------------------------------------------------------------------------
		// CounterTriggerInvalidSource設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_TRG_INVALID_SOURCE_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterTriggerInvalidActivation設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_TRG_INVALID_ACTIVE_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterEventInvalidSource設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_EVENT_INVALID_SOURCE_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterEventInvalidAcitivation設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_EVENT_INVALID_ACTIVE_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterResetInvalidSource設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_INVALID_SOURCE_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// CounterResetInvalidActivation設定
		//--------------------------------------------------------------------------------
		case GENICAM_COUNTER_RESET_INVALID_ACTIVE_ADRS:
			OUT32(address, value);
			break;
#endif // #if defined (MODE_GENICAM_COUNTER)


		//====================================================================================
		//
		// Group: Timer Control registers of standard
		// Note:
		//
		//====================================================================================
#if defined (MODE_GENICAM_TIMER)
		//--------------------------------------------------------------------------------
		// TimerSelector設定
		//--------------------------------------------------------------------------------
		case GENICAM_TIMER_SELECT_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// TimerDuration設定
		//--------------------------------------------------------------------------------
		case GENICAM_TIMER_DURATION_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// TimerDelay設定
		//--------------------------------------------------------------------------------
		case GENICAM_TIMER_DELAY_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// TimerReset設定
		//--------------------------------------------------------------------------------
		case GENICAM_TIMER_RESET_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// TimerValue設定
		//--------------------------------------------------------------------------------
		case GENICAM_TIMER_CURRENT_COUNT_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// TimerStatus設定
		//--------------------------------------------------------------------------------
		case GENICAM_TIMER_STATUS_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// TimerTriggerSource設定
		//--------------------------------------------------------------------------------
		case GENICAM_TIMER_SOURCE_ADRS:
			OUT32(address, value);
			break;

		//--------------------------------------------------------------------------------
		// TimerTriggerActivation設定
		//--------------------------------------------------------------------------------
		case GENICAM_TIMER_ACTIVE_ADRS:
			OUT32(address, value);
			break;
#endif // #if defined (MODE_GENICAM_TIMER)


		//====================================================================================
		//
		// Group: Encoder Control registers of standard
		// Note:
		//
		//====================================================================================
#if defined (MODE_ENCODER)
		//--------------------------------------------------------------------------------
		// EncoderSelector設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_SELECT_ADRS:
			OUT32(address,value);
			break;

		//--------------------------------------------------------------------------------
		// EncoderSourceA設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_PASEA_TRG_SOURCE_ADRS:
			*status = toG (encoderSetPhaseATrgSource (value));
			break;

		//--------------------------------------------------------------------------------
		// EncoderSourceB設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_PASEB_TRG_SOURCE_ADRS:
			*status = toG (encoderSetPhaseBTrgSource (value));
			break;

		//--------------------------------------------------------------------------------
		// EncoderMode設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_MODE_ADRS:
			*status = toG (encoderSetMode (value));
			break;

		//--------------------------------------------------------------------------------
		// EncoderDivider設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_DVIDER_ADRS:
			*status = toG (encoderSetDivider(value));
			break;

		//--------------------------------------------------------------------------------
		// EncoderOutputMode設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_OUTPUT_MODE_ADRS:
			*status = toG (encoderSetOutputMode (value));
			break;

		//--------------------------------------------------------------------------------
		// EncoderStatus設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_STATUS_ADRS:
			OUT32(address,value);
			break;

		//--------------------------------------------------------------------------------
		// EncoderFrequency設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_0_FREQ_ADRS:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// EncoderTimeout設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_TIMEOUT_ADRS:
			*status = toG (encoderSetTimeout (value));
			break;

		//--------------------------------------------------------------------------------
		// EncoderResetSource設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_RESET_TRG_SOURCE_ADRS:
			*status = toG (encoderSetResetTrgSource (value));
			break;

		//--------------------------------------------------------------------------------
		// EncoderResetActivation設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_RESET_ACTIVATION_ADRS:
			*status = toG (encoderSetResetActivation (value));
			break;

		//--------------------------------------------------------------------------------
		// EncoderReset設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_RESET_ADRS:
			*status = toG (encoderSetReset ());
			break;

		//--------------------------------------------------------------------------------
		// EncoderValue at High part in 32-bit設定
		//--------------------------------------------------------------------------------
		case EncoderValue:
			value3rd = IN32(GENICAM_ENCODER_VALUE_LO_ADRS);
			OUT32(GENICAM_ENCODER_VALUE_HI_ADRS,value);
			OUT32(GENICAM_ENCODER_VALUE_LO_ADRS,value3rd);
			break;

		//--------------------------------------------------------------------------------
    	// EncoderValue at Low part in 32-bit設定
		//--------------------------------------------------------------------------------
		case EncoderValueLow:
			OUT32(GENICAM_ENCODER_VALUE_LO_ADRS,value);
			break;

		//--------------------------------------------------------------------------------
		// EncoderValueLow設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_VALUE_LO_ADRS:
			OUT32(address,value);
			break;

		//--------------------------------------------------------------------------------
		// EncoderValueHigh設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_VALUE_HI_ADRS:
			OUT32(address,value);
			value3rd = IN32(GENICAM_ENCODER_VALUE_LO_ADRS);
			OUT32(GENICAM_ENCODER_VALUE_LO_ADRS,value3rd);
			break;

		//--------------------------------------------------------------------------------
		// EncoderValueAtReset設定
		//--------------------------------------------------------------------------------
		case EncoderValueAtReset:
			value3rd = IN32(GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS);
			OUT32(address,value);
			OUT32(GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS,value3rd);
			break;

		//--------------------------------------------------------------------------------
		// EncoderValueAtReset Low設定
		//--------------------------------------------------------------------------------
		case EncoderValueAtResetLow:
			value3rd = IN32(GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS);
			OUT32(address,value);
			break;

		//--------------------------------------------------------------------------------
		// EncoderValueAtResetLow設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS:
			OUT32(address,value);
			break;

		//--------------------------------------------------------------------------------
		// EncoderValueAtResetHigh設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_VALUE_AT_RESET_HI_ADRS:
			OUT32(address,value);
			value3rd = IN32(GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS);
			OUT32(GENICAM_ENCODER_VALUE_AT_RESET_LO_ADRS,value3rd);
			break;

		//--------------------------------------------------------------------------------
		// GENICAM_ENCODER_FREQ_ADRS設定
		//--------------------------------------------------------------------------------
		case GENICAM_ENCODER_FREQ_ADRS:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

#endif // #if defined (MODE_ENCODER)


		//====================================================================================
		//
		// Group: Event Control registers of standard
		// Note:
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// EventSelector設定
		//--------------------------------------------------------------------------------
		case EventSelector:
			if (EventNotification_Max < value)
				value = EventNotification_Max;
			Event_Selector = value;
			break;

		//--------------------------------------------------------------------------------
		// EventNotification設定
		//--------------------------------------------------------------------------------
		case EventNotification:
			Event_Notification[Event_Selector] = value;

#if !defined (MODE_GIGE_10G)
	#if defined (MODE_CAMERA_INTERRUPT)
			// 露光完了割り込み有効／無効
			if (Event_Selector == GEV_EVENT_ExposureEnd)
			{
				if (Event_Notification[GEV_EVENT_ExposureEnd] == 0)
					cameraIntDisable (FPGA_CAMERA_IRQ_EXPOSURE_END_BIT);
				else
					cameraIntEnable (FPGA_CAMERA_IRQ_EXPOSURE_END_BIT);
			}
	#endif // #if defined (MODE_CAMERA_INTERRUPT)
#endif // #if !defined (MODE_GIGE_10G)
			break;

#if !defined (MODE_GIGE_10G)
		//--------------------------------------------------------------------------------
		// EventExposureEnd設定
		//--------------------------------------------------------------------------------
		 case EventExposureEnd:
             *status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// EventExposureEndTimestamp設定
		//--------------------------------------------------------------------------------
		 case EventExposureEndTimestamp:
             *status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// EventExposureEndFrameID設定
		//--------------------------------------------------------------------------------
		 case EventExposureEndFrameID:
             *status = GEV_STATUS_WRITE_PROTECT;
			break;
#endif // #if !defined (MODE_GIGE_10G)
		
		//--------------------------------------------------------------------------------
		// CounterDiagnosticSelector設定
		//--------------------------------------------------------------------------------
#if defined (IF_GIGE)
		case CounterDiagnosticSelector:
			CounterDiagnostic_Selector = value;
			if ( CounterDiagnosticValue_FramebufferBlockDrop <= CounterDiagnostic_Selector
					&& CounterDiagnosticValue_FramebufferDescOverflow >= CounterDiagnostic_Selector)
			{
				// Print framebuffer information to std_out
				framebuf_printregs();
			}
			break;
#endif

		//--------------------------------------------------------------------------------
		// CounterDiagnosticValue設定
		//--------------------------------------------------------------------------------
		case CounterDiagnosticValue:
			//@@@1OUT32(ARM1_DIAG_VALUE_ADRS_OFFSET(CounterDiagnostic_Selector), value);
			break;


		//====================================================================================
		//
		// Group: DeviceControl
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// DeviceScanType設定
		//--------------------------------------------------------------------------------
		case DeviceScanType:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceTLType設定
		//--------------------------------------------------------------------------------
		case DeviceTLType:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceTapGeometry設定
		//--------------------------------------------------------------------------------
		case DeviceTapGeometry:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceVariant設定
		//--------------------------------------------------------------------------------
		case DeviceVariant:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceReset設定
		//--------------------------------------------------------------------------------
		case DeviceReset:
			// TODO: Resets the all selector on the DDR and Trigger and Modes etc...
			break;

		//--------------------------------------------------------------------------------
		// DeviceBoot設定
		//--------------------------------------------------------------------------------
		case DeviceBoot:
		*status = toG (boardReset());
			break;

		//--------------------------------------------------------------------------------
		// DeviceDipsw設定
		//--------------------------------------------------------------------------------
		 case DeviceDipsw:
            *status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceFactory
		//--------------------------------------------------------------------------------
		case DeviceFactory:
		#if defined (IF_GIGE)
			// Restores the all configuration first.
			// Then restores the DeviceUserID string as empty string.
			user_userid_init((u32*) status);
			// Then restores the network configuration.
			user_network_init((u32*) status);
		#endif
			break;

		//--------------------------------------------------------------------------------
		// DeviceCameraErrorSelect設定
		//--------------------------------------------------------------------------------
		case FIRM_DATA_CAMERA_ERROR_SELECT_ADRS:
			OUT32 (address, value);
			break;

		//--------------------------------------------------------------------------------
		// DeviceCameraErrorStatus設定
		//--------------------------------------------------------------------------------
		case FIRM_DATA_CAMERA_ERROR_STATUS_ADRS:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceMainVolt設定
		//--------------------------------------------------------------------------------
#if defined (MODE_VOLTAGE_IF_BOARD)
		case DeviceMainVolt:
            *status = GEV_STATUS_WRITE_PROTECT;
			break;
#endif

		//--------------------------------------------------------------------------------
		// SensorBlackPixel設定
		//--------------------------------------------------------------------------------
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
		case SensorBlackPixel:
			//*status = toG (executeCommandSensorBlackPixel ((int)value));
			*status = toG (sensorSetBlackPixel (value));
			break;
#endif


#if defined (MODE_FRAMERATE_HIGH_SPEED)
		//--------------------------------------------------------------------------------
		// HighSpeedMode設定
		//--------------------------------------------------------------------------------
		case HighSpeedMode:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// HighSpeedModeCmd設定
		//--------------------------------------------------------------------------------
		case HighSpeedModeCmd:
			//*status = (u32) executeCommandHighSpeedMode (gHighSpeedMode, gHighSpeedModeLineCount);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// Frame Rate High Speed Mode設定
			if ((*status = toG (sensorSetFrameRateHighSpeedMode (gHighSpeedMode))) != AVAL_STATUS_SUCCESS)
				break;
	
			break;

		//--------------------------------------------------------------------------------
		// HighSpeedModeLineCount設定
		//--------------------------------------------------------------------------------
		case HighSpeedModeLineCount:
			//*status = toG(aoiSetHeight((int) value));
			gHighSpeedModeLineCount = value;
			break;

		//--------------------------------------------------------------------------------
		// HighSpeedModeSelector設定
		//--------------------------------------------------------------------------------
		case HighSpeedModeSelector:
			gHighSpeedMode = value;
			break;
#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)

		//--------------------------------------------------------------------------------
		// DeviceAcesFlag設定
		//--------------------------------------------------------------------------------
		case DeviceAcesFlag:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceRateMode設定
		//--------------------------------------------------------------------------------
#if (MODE_SENSOR_VENDOR == SENSOR_VENDOR_S)
		case DeviceRateMode:
			*status = toG (acquisitionSetRateMode (value));
			break;
#endif

		//--------------------------------------------------------------------------------
		// FPGA_TG_TGSE_ADRS設定
		//--------------------------------------------------------------------------------
		case FPGA_TG_TGSE_ADRS:
			*status = toG (tgSetTgse (value));
			break;

		//--------------------------------------------------------------------------------
		// FPGA_TG_TGES_ADRS設定
		//--------------------------------------------------------------------------------
		case FPGA_TG_TGES_ADRS:
			*status = toG (tgSetTges (value));
			break;

		//--------------------------------------------------------------------------------
		// FPGA_TG_TGPD_ADRS設定
		//--------------------------------------------------------------------------------
		case FPGA_TG_TGPD_ADRS:
			*status = toG (tgSetTgpd (value));
			break;


		//====================================================================================
		//
		// Group: Diagnostic Control registers of standard
		// Note:
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// DeviceBuiltInTest設定
		//--------------------------------------------------------------------------------
		case DeviceBuiltInTest:
			DeviceBuiltInTest_Mode = value;
			break;

		//--------------------------------------------------------------------------------
		// DeviceBuiltInTestStatus設定
		//--------------------------------------------------------------------------------
		case DeviceBuiltInTestStatus:
			DeviceBuiltInTest_Status = value;
			break;

		//--------------------------------------------------------------------------------
		// DiagnosticSelector設定
		//--------------------------------------------------------------------------------
		case DiagnosticSelector:
			Diagnostic_Selector = value;
			switch (Diagnostic_Selector)
			{
				case 0:
#if defined (IF_GIGE)
					// Print framebuffer information to std_out
					framebuf_printregs();
#endif
					break;
				case 1:
				default:
					break;
			}
			break;

		//--------------------------------------------------------------------------------
		// Diagnostic設定
		//--------------------------------------------------------------------------------
		case Diagnostic:
			//*status = executeCommandDiagnostic(Diagnostic_Selector);

			switch (Diagnostic_Selector)
			{
				case 0:
					break;
				case 1:
					//dummy = 1;    // 1回のみ実行15秒
					//dummy = -1; // Discard 永久ループ
					//status = executeCommand(FIRM_CMD_AGING, CPU_CMD_SYNC_OFF, (u32*) &dummy, 1);
				
					// Cmd Initialze
					if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
						break;

					// Aging
					if ((*status = toG (cmdDiagAgingMain (1, DIAG_MODE_AGING))) != AVAL_STATUS_SUCCESS)
						break;

					break;
				default:
					break;
			}

		
		
			break;

		//--------------------------------------------------------------------------------
		// DiagnosticResult設定
		//--------------------------------------------------------------------------------
		case DiagnosticResult:
			switch (Diagnostic_Selector)
			{
				case 0:
					break;
				case 1:
					OUT32(BOARD_STATUS_AGING_ADRS, value);
					break;
				default:
					break;
			}
			break;

		//--------------------------------------------------------------------------------
		// DiagnosticResultNumber設定
		//--------------------------------------------------------------------------------
		case DiagnosticResultNumber:
			switch (Diagnostic_Selector)
			{
				case 0:
					break;
				case 1:
					OUT32(BOARD_STATUS_AGING_ADRS, value);
					break;
				default:
					break;
			}
			break;


		//====================================================================================
		//
		// Group: Device Information registers of standard
		// Note:
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// DeviceTemperatureSelector設定
		//--------------------------------------------------------------------------------
		case DeviceTemperatureSelector:
			DeviceTemperature_Selector = value;
			break;

		//--------------------------------------------------------------------------------
		// DeviceTemperaturePeltierMode設定
		//--------------------------------------------------------------------------------
#if defined (MODE_PELTIER)
		case DeviceTemperaturePeltierMode:

			#if defined (MODE_TEMP_ABNORMAL_CHECK)
			if (value == 1)
			{
				// Temp Abnormal Status取得
				if (tempGetAbnormalStatus ((unsigned int *)&value2nd) != AVAL_STATUS_SUCCESS)
					break;

				// 温度以上の為、何もしない
				if (value2nd != 0)
					break;
			}
			#endif

			#if defined (MODE_PELTIER_CTRL)
			if (value == 1)
			{
					// Peltier Control Init
				peltierControlProcsInit ();
			}
			#endif

			*status = peltierSetMode (value);
				
			#if defined (MODE_PELTIER_CTRL)
			if (value == 1)
			{
				// Peltier Enable Delay
				//peltierControlProcs ();
				peltierSetPowerStartFlag (MODE_ENABLE);
			}
			#endif

			break;
#endif


		//--------------------------------------------------------------------------------
		// Sensor Target Temperature設定
		//--------------------------------------------------------------------------------
		case FPGA_PELTIER_TARGET_ADRS:
			#if defined (MODE_PELTIER_CTRL)
			value2nd = 0;
			*status = peltierGetMode ((int *)&value2nd);

			if (value2nd == 1)
			{
				// Peltier Control Init
				peltierControlProcsInit ();
			}
			#endif

			intValue = (int)value;
			dblValue = (float) intValue;
			if ((*status = toG(peltierSetTarget(dblValue))) != AVAL_STATUS_SUCCESS)
				break;
				
			#if defined (MODE_PELTIER_CTRL)
			if (value2nd == 1)
			{
				// Peltier Enable Delay
				//peltierControlProcs ();
				peltierSetPowerStartFlag (MODE_ENABLE);
			}
			#endif

			break;

		//--------------------------------------------------------------------------------
		// Sensor Temperature Float
		//--------------------------------------------------------------------------------
		case DeviceTemperature_Sensor_Float:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Camera Temperature Float
		//--------------------------------------------------------------------------------
		case DeviceTemperature_Case_Float:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// FPGA Temperature	Float
		//--------------------------------------------------------------------------------
		case DeviceTemperature_FPGA_Float:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Sensor Max Temperature
		//--------------------------------------------------------------------------------
		case DeviceTemperature_Sensor_Max:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;
		
		//--------------------------------------------------------------------------------
		// Sensor Min Temperature
		//--------------------------------------------------------------------------------
		case DeviceTemperature_Sensor_Min:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Sensor Min Target Temperature
		//--------------------------------------------------------------------------------
		case DeviceTemperatureTarget_Min:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Sensor Max Target Temperature
		//--------------------------------------------------------------------------------
		case DeviceTemperatureTarget_Max:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Sensor Alarm Max Temperature設定
		//--------------------------------------------------------------------------------
		case FPGA_PELTIER_SENSOR_ALM_ADRS:
			// Alarm取得
			if ((*status = toG(peltierGetSensorTempAlarm(&dblValue, &dblValue2nd))) != AVAL_STATUS_SUCCESS)
				break;

			intValue = (int)value;
			dblValue = (double)intValue;

			// Alarm設定
			*status = toG(peltierSetSensorTempAlarm(dblValue, dblValue2nd));
			break;

		//--------------------------------------------------------------------------------
		// Sensor Alarm Min Temperature設定
		//--------------------------------------------------------------------------------
		case DeviceTemperatureAlarmMin_Sensor:
			// Alarm取得
			if ((*status = toG(peltierGetSensorTempAlarm(&dblValue, &dblValue2nd))) != AVAL_STATUS_SUCCESS)
				break;

			intValue = (int)value;
			dblValue2nd = (double)intValue;

			// Alarm設定
			*status = toG(peltierSetSensorTempAlarm(dblValue, dblValue2nd));
			break;

		//--------------------------------------------------------------------------------
		// Case Alarm Max Temperature設定
		//--------------------------------------------------------------------------------
		case FPGA_PELTIER_CASE_ALM_ADRS:
			*status = toG(peltierGetCaseTempAlarm(&dblValue, &dblValue2nd));
			dblValue = value;
			*status = toG(peltierSetCaseTempAlarm(dblValue, dblValue2nd));
			break;

		//--------------------------------------------------------------------------------
		// Case Alarm Min Temperature設定
		//--------------------------------------------------------------------------------
		case DeviceTemperatureAlarmMin_Housing:
			*status = toG(peltierGetCaseTempAlarm(&dblValue, &dblValue2nd));
			dblValue2nd = value;
			*status = toG(peltierSetCaseTempAlarm(dblValue, dblValue2nd));
			break;

		//--------------------------------------------------------------------------------
		// DeviceTemperatureAlarmStatus設定
		//--------------------------------------------------------------------------------
		case DeviceTemperatureAlarmStatus:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceTemperatureSensorAlarmCount設定
		//--------------------------------------------------------------------------------
		case DeviceTemperatureSensorAlarmCount:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// DeviceTemperatureCaseAlarmCount設定
		//--------------------------------------------------------------------------------
		case DeviceTemperatureCaseAlarmCount:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

#if defined (MODE_PELTIER_CTRL)
		//--------------------------------------------------------------------------------
		// DevicePeltierPowerLevel設定
		//--------------------------------------------------------------------------------
		case DevicePeltierPowerLevel:
			// Get Pelter Mode
			peltierGetMode ((int *)&value2nd);

			// Set Peltier Power Level
			*status = toG (peltierSetPowerLevel(value));
			
			if (value2nd == 1)
			{
				// Peltier Enable Delay
				peltierSetPowerStartFlag (MODE_ENABLE);
			}
			break;
#endif
			

#if defined (MODE_PELTIER_VOLT_CURRENT)
		//--------------------------------------------------------------------------------
		// Peltier Voltage設定
		//--------------------------------------------------------------------------------
		case DevicePeltierVolt:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//--------------------------------------------------------------------------------
		// Peltier Current設定
		//--------------------------------------------------------------------------------
		case DevicePeltierCurrent:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;
#endif


#if defined (MODE_TEMP_ABNORMAL_CHECK)
		//--------------------------------------------------------------------------------
		// 温度異常ステータス設定
		//--------------------------------------------------------------------------------
		case DeviceTemperatureAbnormalStatus:
			*status = toG (tempSetAbnormalStatus(value));

			// LED Clear
			if (value == 0)
				setLed (LED_NUM1, LED_PATTERN0);

			break;

		//--------------------------------------------------------------------------------
		// 温度異常カウント設定
		//--------------------------------------------------------------------------------
		case DeviceTemperatureAbnormalCount:
			*status = toG (tempSetAbnormalCount(value));
			break;
#endif // #if defined (MODE_TEMP_ABNORMAL_CHECK)

#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
		//--------------------------------------------------------------------------------
		// PHY温度設定
		//--------------------------------------------------------------------------------
		case DeviceTemperaturePhy:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;
#endif // #if defined (MODE_GIGE_10G) && defined (IF_GIGE)
				
		//--------------------------------------------------------------------------------
		// Sensor Power Status設定
		//--------------------------------------------------------------------------------
		case FIRM_DATA_SENSOR_POWER_STATUS_ADRS:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

#if defined (MODE_SENSOR_DRRS)
		//--------------------------------------------------------------------------------
		// DeviceDrrsCommand設定
		//--------------------------------------------------------------------------------
		case DeviceDrrsCommand:
			//value2nd = executeCommand(FIRM_CMD_DRRS, CPU_CMD_SYNC_OFF, (u32*) &gDrrsMode, 1);
			//*status = toG(value2nd);

			// Cmd Initialze
			if ((*status = toG (cmdExecuteInit ())) != AVAL_STATUS_SUCCESS)
				break;

			// DRRS
			if ((*status = toG (sensorSetDrrsMain2 (value))) != AVAL_STATUS_SUCCESS)
				break;
		
			break;

		//--------------------------------------------------------------------------------
		// DeviceDrrsMode設定
		//--------------------------------------------------------------------------------
		case DeviceDrrsMode:
			gDrrsMode = value;
			break;
				
		//--------------------------------------------------------------------------------
		// DeviceDrrsStatus設定
		//--------------------------------------------------------------------------------
		case DeviceDrrsStatus:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;
#endif


		//====================================================================================
		//
		// Group: Auto Bright
		// Note:
		//
		//====================================================================================
#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)
		//----------------------------------------------------------------------------------
		// Auto Bright Exposure Mode設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_EXPOSURE_MODE:
			*status = toG (autoBrightSetExposureMode (value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Exposure Once Status設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_EXPOSURE_STATUS:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Exposure Min設定
		//----------------------------------------------------------------------------------
		//case AUTO_BRIGHT_EXPOSURE_MIN:
			//*status = toG (autoBrightSetExposureMin (value));
			//break;

		//----------------------------------------------------------------------------------
		// Auto Bright Exposure Max設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_EXPOSURE_MAX:
			*status = toG (autoBrightSetExposureMax (value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Gain Mode設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_GAIN_MODE:
			*status = toG (autoBrightSetGainMode (value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Gain Once Status設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_GAIN_STATUS:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Gain Min設定
		//----------------------------------------------------------------------------------
		//case AUTO_BRIGHT_GAIN_MIN:
			//dblValue = (double)((double)value / (double)AUTO_GAIN_UNIT);
			//*status = toG (autoBrightSetGainMin ((double)value));
			//break;

		//----------------------------------------------------------------------------------
		// Auto Bright Gain Max設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_GAIN_MAX:
			*status = toG (autoBrightSetGainMax ((double)value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Area設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_OVERLAY:
			*status = toG (autoBrightSetDetectArea (value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Target設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_TARGET:
			*status = toG (autoBrightSetTarget (value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Average設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_AVERAGE:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Area設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_TARGET_AREA:
			*status =  toG (autoBrightSetTargetArea (value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Width Size設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_WIDTH_SIZE:
			*status = toG (autoBrightSetWidthSize (value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Height Size設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_HEIGHT_SIZE:
			*status = toG (autoBrightSetHeightSize (value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Width Offset設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_WIDTH_OFFSET:
			*status = toG (autoBrightSetWidthOffset (value));
			break;

		//----------------------------------------------------------------------------------
		// Auto Bright Height Offset設定
		//----------------------------------------------------------------------------------
		case AUTO_BRIGHT_HEIGHT_OFFSET:
			*status = toG (autoBrightSetHeightOffset (value));
			break;
#endif // #if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)


		//====================================================================================
		//
		// Group: Acquisition Control registers of standard
		// Note:
		//
		//====================================================================================
#if defined(MODE_BINNING)
		//----------------------------------------------------------------------------------
		// BinningX設定
		//----------------------------------------------------------------------------------
		case BINNING_HORIZONTAL:

			if((*status = toG (aoiSetBinningX(value))) != AVAL_STATUS_SUCCESS)
				break;

			if((*status = toG (aoiGetWidth((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
				break;

			video_width = value2nd;

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			if (video_chunk_ctrl & 0x80000000)
				update_trailer = 1;
			#endif

			break;

		//----------------------------------------------------------------------------------
		// BinningY設定
		//----------------------------------------------------------------------------------
		case BINNING_VERTICAL:
			if((*status = toG(aoiSetBinningY(value))) != AVAL_STATUS_SUCCESS)
				break;

			// Height取得
			if ((*status = toG (sensorGetFrameRateHighSpeedMode ((int *)&value3rd))) != AVAL_STATUS_SUCCESS)
				break;

			if (value3rd == MODE_ENABLE)
			{
				if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
				{
					// Get Virtual Height
					sensorGetVirtualHeight ((int *)&value);
				
					// Set Video Height
					video_height = value;
				}
				else
				{
					if((*status = toG(aoiGetHeight ((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
						break;
	
					video_height = value2nd;
				}
			}
			else
			{
				if((*status = toG(aoiGetHeight((int *)&value2nd))) != AVAL_STATUS_SUCCESS)
					break;

				video_height = value2nd;
			}

			#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
			update_leader = 1;
			update_trailer = 1;
			#endif

			break;

		//----------------------------------------------------------------------------------
		// Binningmode設定
		//----------------------------------------------------------------------------------
		case BINNING_MODE:
			*status = toG (aoiSetBinningMode (value));
			break;

#endif //#if defined(MODE_BINNING)


		//====================================================================================
		//
		// For 10G Ethernet
		// Group: 
		//
		//====================================================================================
#if defined (MODE_GIGE_10G) && defined (IF_GIGE)
		//----------------------------------------------------------------------------------
		// DevicePhyFirmVersion設定
		//----------------------------------------------------------------------------------
		case DevicePhyFirmVersion:
		case DevicePhyFirmVersion + 4:
		case DevicePhyFirmVersion + 8:
		case DevicePhyFirmVersion + 12:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// DevicePhyApiVersion設定
		//----------------------------------------------------------------------------------
		case DevicePhyApiVersion:
		case DevicePhyApiVersion + 4:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// DevicePacketDelayCalc設定
		//----------------------------------------------------------------------------------
		case DevicePacketDelayCalc:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// DevicePacketDelayData設定
		//----------------------------------------------------------------------------------
		case DevicePacketDelayData:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;
#endif // #if defined (MODE_GIGE_10G) && defined (IF_GIGE)	


		//====================================================================================
		//
		// For IEEE 1588 PTP 
		// Group: 
		//
		//====================================================================================
#if defined (MODE_IEEE1588_PTP) && defined (IF_GIGE)
		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_CLOCK_ACCURACY設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_CLOCK_ACCURACY:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_DATA_SET設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_DATA_SET:
			ptp_get_sfnc_ptpcontrol(&sfnc_ptp); // PtpDataSetLatch
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_STATUS設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_STATUS:
			 *status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_SERVER_STATUS設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_SERVER_STATUS:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_OFFSET_FORM_MASTER_H設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_OFFSET_FORM_MASTER_H:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_OFFSET_FORM_MASTER_L設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_OFFSET_FORM_MASTER_L:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_MEAN_PATH_DELAY_H設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_MEAN_PATH_DELAY_H:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_MEAN_PATH_DELAY_L設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_MEAN_PATH_DELAY_L:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_CLOCK_ID_H設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_CLOCK_ID_H:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_CLOCK_ID_L設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_CLOCK_ID_L:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_PARENT_CLOCK_ID_H設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_PARENT_CLOCK_ID_H:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_PARENT_CLOCK_ID_L設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_PARENT_CLOCK_ID_L:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_GRAND_MASTER_CLOCK_ID_H設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_GRAND_MASTER_CLOCK_ID_H:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;

		//----------------------------------------------------------------------------------
		// IEEE1588_PTP_GRAND_MASTER_CLOCK_ID_L設定
		//----------------------------------------------------------------------------------
		case IEEE1588_PTP_GRAND_MASTER_CLOCK_ID_L:
			*status = GEV_STATUS_WRITE_PROTECT;
			break;
#endif // #if defined (MODE_IEEE1588_PTP) && defined (IF_GIGE)


		//====================================================================================
		//
		// Group: File Access Control registers of standard
		// Note:
		//
		//====================================================================================

		//--------------------------------------------------------------------------------
		// FileSelector
		//--------------------------------------------------------------------------------
		case FileSelector:
			fileSelector = value;
			break;

		//--------------------------------------------------------------------------------
		// FileOperationeSelector
		//--------------------------------------------------------------------------------
		case FileOperationeSelector:
			fileSel[fileSelector] = value;
			break;

		//--------------------------------------------------------------------------------
		// FileOperationExecute
		//--------------------------------------------------------------------------------
		case FileOperationExecute:
			fileExec[fileSelector] = value;
			ledSetState(LED_POWER, LED_CONFIG, 0);
			switch (fileSelector)
			{
				case FileSelector_SYSTEM:
					break;

				case FileSelector_FPGA:
					*status = toG (FileSelectorFpga ());
					break;

				case FileSelector_XML:
					*status = toG (FileSelectorXML ());
					break;

				case FileSelector_LUTLuminance0:
					switch (value)
					{
						case FileOperationeSelector_Open:
							break;
						case FileOperationeSelector_Close:
							break;
						case FileOperationeSelector_Read:
							break;
						case FileOperationeSelector_Write:
							break;
						case FileOperationeSelector_Delete:
							break;
						default:
							break;
					}
					break;

				case FileSelector_UserSetDefault:
				case FileSelector_UserSet1:
				case FileSelector_UserSet2:
					switch (value)
					{
						case FileOperationeSelector_Open:
							break;
						case FileOperationeSelector_Close:
							break;
						case FileOperationeSelector_Read:
							break;
						case FileOperationeSelector_Write:
							break;
						case FileOperationeSelector_Delete:
							break;
						default:
							break;
					}
					break;

				case FileSelector_DPC0:
					 *status = toG (FileSelectorDPC());
					 break;

				case FileSelector_FFC0:
					 *status = toG (FileSelectorFFC());
					 break;

				#if defined (MODE_SPECTRUM)
				case FileSelector_SPECTRUM_WAVE:
					*status = toG (FileSelectorSpectrumWave ());
					 break;
				#endif

				#if 0 //@@@1
				#if defined (MODE_FPGA_PF)
				case FileSelector_IF_FPGA:
					*status = toG (FileSelectorIfFpgaWrite());
					break;
				#endif
				#endif //@@@1

				#if defined (MODE_GIGE_10G)
				case FileSelector_PHY:
					*status = toG (FileSelectorPhyWrite());
					break;
				#endif

				default:
					// Unknown
					*status = GEV_STATUS_INVALID_PARAMETER;
					break;
				}

				fileExec[fileSelector] = 0;
				break;

		//--------------------------------------------------------------------------------
		// FileOpenMode
		//--------------------------------------------------------------------------------
		case FileOpenMode:
			fileMode[fileSelector] = value;
			break;

		//--------------------------------------------------------------------------------
		// FileAccessOffset
		//--------------------------------------------------------------------------------
		case FileAccessOffset:
			fileOffset[fileSelector][fileSel[fileSelector]] = value;
			break;

		//--------------------------------------------------------------------------------
		// FileAccessLength
		//--------------------------------------------------------------------------------
		case FileAccessLength:
			fileLength[fileSelector][fileSel[fileSelector]] = value;
			break;

		//--------------------------------------------------------------------------------
		// FileOperationStatus
		//--------------------------------------------------------------------------------
		case FileOperationStatus:
			fileStatus[fileSelector][fileSel[fileSelector]] = value;
			break;

		//--------------------------------------------------------------------------------
		// FileOperationResult
		//--------------------------------------------------------------------------------
		case FileOperationResult:
			fileResult[fileSelector][fileSel[fileSelector]] = value;
			break;

		//--------------------------------------------------------------------------------
		// FileSize
		//--------------------------------------------------------------------------------
		case FileSize:
			fileSize[fileSelector] = value;
			break;

		//--------------------------------------------------------------------------------
		// default
		//--------------------------------------------------------------------------------
		default:

			#if defined (MODE_SPECTRUM)
			//====================================================================================
			//
			// Group: Spectrum Control
			// Note:
			//
			//====================================================================================
			if (address >= FPGA_DOG_BAND_OFFSET1_ADRS && address < (FPGA_DOG_BAND_OFFSET1_ADRS + (SPECTRUM_BAND_COUNT*FPGA_DOG_BAND_INTEVAL)) )
			{
				value3rd =  (address - FPGA_DOG_BAND_OFFSET1_ADRS);
				value3rd =  ((long)value3rd >= (long)FPGA_DOG_BAND_INTEVAL)? value3rd / FPGA_DOG_BAND_INTEVAL : 0;
				value2nd = (address & 0xF);
				switch(value2nd)
				{
					case 0x0: // SpectrumBlackLevel as FPGA_DOG_BAND_OFFSET1_ADRS_OFFSET
						*status = toG(digitalSetBandOffset(value3rd,value));
						break;
					case 0x4: // SpectrumGain as FPGA_DOG_BAND_GAIN_ADRS
						OUT32 (address,value);

						#if defined (MODE_IPU_MULTI)
						OUT32 ((address + FPGA_DOG2_OFFSET),value);
						#endif

						*status = GEV_STATUS_SUCCESS;
						break;
					case 0x8: // SpectrumBlackLevelPreceding as FPGA_DOG_BAND_OFFSET2_ADRS_OFFSET
						*status = toG(digitalSetBandOffset1(value3rd,value));
						break;
					case 0x10:
						// Reserved
						*status = GEV_STATUS_INVALID_PARAMETER;
						break;
					default:
						// Unknown
						*status = GEV_STATUS_INVALID_PARAMETER;
						break;
				}
			}
			#endif // #if defined (MODE_SPECTRUM)

		
			#if defined (MODE_SPECTRUM_BANDGAIN_FILTER)
			//====================================================================================
			//
			// Group: Spectrum Control
			// Note:
			//
			//====================================================================================
			if (address >= FPGA_BGF_BAND_OFFSET1_ADRS && address < (FPGA_BGF_BAND_OFFSET1_ADRS + (SPECTRUM_BAND_COUNT * FPGA_BGF_BAND_INTEVAL)) )
			{

				value3rd =  (address - FPGA_BGF_BAND_OFFSET1_ADRS);
				value3rd =  ((long)value3rd >= (long)FPGA_BGF_BAND_INTEVAL)? value3rd / FPGA_BGF_BAND_INTEVAL : 0;
				value2nd = (address & 0xF);
				switch(value2nd)
				{
					case 0x0: // SpectrumBlackLevel as FPGA_BGF_BAND_OFFSET1_ADRS
						*status = toG(bgfSetBandOffset(value3rd,value));
						break;
					case 0x4: // SpectrumGain as FPGA_BGF_BAND_GAIN_ADRS
						fltValue = (float)((float)value / (float)DEVICE_GAIN_UNIT);
						*status = toG(bgfSetBandGainX(value3rd,fltValue));
						break;
					case 0x8: // SpectrumBlackLevelPreceding as FPGA_BGF_BAND_OFFSET2_ADRS
						*status = toG(bgfSetBandOffset1(value3rd,value));
						break;
					case 0x10:
						// Reserved
						*status = GEV_STATUS_INVALID_PARAMETER;
						break;
					default:
						// Unknown
						*status = GEV_STATUS_INVALID_PARAMETER;
						break;
				}
			}
			#endif


			//====================================================================================
			//
			// Group: File Access Control  virtual buffer registers map of FPGA
			// Note:
			//
			//====================================================================================
			if (address >= FileAccessBuffer && address <= BASE_FILE_BUFFER_MAX)
			{
				address2nd = address & BASE_FILE_BUFFER_MASK; // Gets the address of offset by starting point
				//address3rd   = fileLength[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
				valueBuffer = (u32*) fileBuffer[fileSelector]; //Gets the start pointer of temporary buffer
				valueBuffer2nd = valueBuffer;
				if (valueBuffer != 0)
				{
					value2nd = SWAP_L(value);  // Convert to big-endian
					valueBuffer2nd += (address2nd > 0) ? address2nd / 4 : 0; // Seek to specified address
					*valueBuffer2nd = value2nd; // Writes the new value to the specified buffer address
					fileResult[fileSelector][fileSel[fileSelector]] = address2nd + 4;
				}
				break;
			}


			//====================================================================================
			//
			// Group: Flat Field Correction Data
			// Note:
			//
			//====================================================================================
			//@@@1if ((address >= FFC_MEMORY_ADRS) && (address <= (FFC_MEMORY_ADRS + FFC_DATA_SECOND_SIZE)))
			//@@@1{
				//@@@1OUT32 (address, value);
				//@@@1break;
			//@@@1}


			//====================================================================================
			//
			// Group: Defective Pixel Correction Data
			// Note:
			//
			//====================================================================================
			//@@@1if ((address >= DPC_MEMORY_ADRS) && (address <= (DPC_MEMORY_ADRS + DPC_DATA_SIZE)))
			//@@@1{
				//@@@1OUT32 (address, value);
				//@@@1break;
			//@@@1}


			#if 0 //@@@1
			//====================================================================================
			//
			// Group: LUT registers map of FPGA
			// Note:
			//
			//====================================================================================
			if (((address >= FPGA_LUT_MEM1_ADRS) && (address < FPGA_LUT_MEM1_ADRS + LUT_DATA_SIZE)) ||
			    ((address >= FPGA_LUT_MEM2_ADRS) && (address < FPGA_LUT_MEM2_ADRS + LUT_DATA_SIZE)))
			{
				interval = offset = 0;
				*status = toG(aoiGetBitWidth((int*) &bit));			// ビット幅取得
				*status = toG(lutGetTablInterval(bit, (unsigned int*) &interval)); // interval = 8bit=64,10bit=16,12bit=4,14bit=1
				*status = toG(aoiGetShift((int) bit, (int*) &value3rd)); // シフト数取得
				address2nd = address & LUT_DATA_MASK; // Mask the actual LUT address from a virtual address

				if (value2nd == 14)
				{
					// 14 bits
					OUT32(address, ((value & LUT_DATA_MASK)<<value3rd)); // 指定アドレスの上位に有効データを書き込む

					#if defined (MODE_IPU_MULTI)
					OUT32((address + FPGA_LUT2_OFFSET), ((value & LUT_DATA_MASK)<<value3rd)); // 指定アドレスの上位に有効データを書き込む
					#endif

				}
				else
				{
					for (i = 0; i < interval; i++, offset += LUT_TABLE_INTERVAL)
					{
						OUT32((address + offset), ((value & LUT_DATA_MASK) << value3rd)); // 指定アドレスの上位に有効データを書き込む

						#if defined (MODE_IPU_MULTI)
						OUT32((address + offset + FPGA_LUT2_OFFSET), ((value & LUT_DATA_MASK) << value3rd)); // 指定アドレスの上位に有効データを書き込む
						#endif
					}
				}
				break;
			}
			#endif //@@@1

			#if defined (IF_GIGE)
			//====================================================================================
			//
			// Group: Memories map of EEPROM on EPC of GigE IPs
			// Note:
			//
			//====================================================================================
			// Configuration EEPROM 8-64kByte, here 8k)
			if (address == BASE_NET_BOOTROM_MAC)
			{
				// MAC High address
				address2nd = address & BASE_NET_BOOTROM_MASK; // Clip the high virtual address
				bytValue = (value & 0xFF000000) >> 24;
				eeprom_write_byte((u16) address2nd, bytValue); // MSB at 1 byte point
				bytValue = (value & 0x00FF0000) >> 16;
				eeprom_write_byte((u16) address2nd + 1, bytValue); // MSB at 2 byte point
				bytValue = (value & 0x0000FF00) >> 8;
				eeprom_write_byte((u16) address2nd + 2, bytValue); // MSB at 3 byte point
				bytValue = (value & 0x000000FF) >> 0;
				eeprom_write_byte((u16) address2nd + 3, bytValue); // MSB at 4 byte point
				break;
			}
			else if (address == (BASE_NET_BOOTROM_MAC + 4))
			{
				// MAC Low address
				address2nd = address & BASE_NET_BOOTROM_MASK; // Clip the high virtual address
				bytValue = (value & 0xFF000000) >> 24;
				eeprom_write_byte((u16) address2nd, bytValue); // LSB at 5 byte point
				bytValue = (value & 0x00FF0000) >> 16;
				eeprom_write_byte((u16) address2nd + 1, bytValue); // LSB at 6 byte point
				break;
			}
			else if ((address >= BASE_NET_BOOTROM) && (address <= BASE_NET_BOOTROM_MAX))
			{
				address2nd = address & BASE_NET_BOOTROM_MASK; // Clip the high virtual address
				value2nd = SWAP_L(value);
				//eeprom_write_dword((u16)address2nd, value2nd); // with    swapped endian
				eeprom_write_dword((u16) address2nd, value); // without swapped endian
				break;  // Success
			}
			

			// ---------------------------------------------------------
			// For IP Core Design
			// Configuration EEPROM 8-64kByte, here 8k)
			//
			#if 0	//@@@1
			if ((address >= 0xFBFF0000) && (address < 0xFBFF2000))
			{
				eeprom_write_dword((u16) (address - 0xFBFF0000), value);
				break;
			}
			// SPI flash memory
			if (address >= 0xFE000000)
			{
				if (address < 0xFE010000)                      // 64 kB write buffer
				{
					flash_buffer[(address - 0xFE000000) / 4] = value;
				}
				else
				{
					if (address == 0xFE010000)                  // Write address
					{
						flash_write_block(value, (u32 *) flash_buffer, 65536);
					}
					else                                        // Read-only space
					{
						*status = GEV_STATUS_WRITE_PROTECT;
					}
				}
				break; // Success
			}
			#endif //@@@1
		
			// Undefined address space
			//*status = GEV_STATUS_INVALID_ADDRESS;
			#endif // #if defined (IF_GIGE)
			break;
		}

		#if defined (IF_GIGE)
		#if !defined (MODE_GIGE_10G)
		//***********************************************
		// Checks AOI and Update the settings of frame buffer controller
		if( address == FPGA_AOI_BITWIDTH_ADRS
			||address == FPGA_AOI_XSIZE_ADRS
			|| address == FPGA_AOI_YSIZE_ADRS )
		{
			switch (video_pixfmt)
			{
			case GVSP_PIX_MONO8:
				bpp = 1; // in unit of bytes
				bit = 8; // in unit of bits
				break;
			case GVSP_PIX_BAYGR10:
			case GVSP_PIX_BAYRG10:
			case GVSP_PIX_BAYGB10:
			case GVSP_PIX_BAYBG10:
			case GVSP_PIX_MONO10:
				bpp = 2; // in unit of bytes
				bit = 10; // in unit of bits
				break;
			case GVSP_PIX_BAYGR12:
			case GVSP_PIX_BAYRG12:
			case GVSP_PIX_BAYGB12:
			case GVSP_PIX_BAYBG12:
			case GVSP_PIX_MONO12:
				bpp = 2; // in unit of bytes
				bit = 12; // in unit of bits
				break;
			case GVSP_PIX_MONO14:
				bpp = 2; // in unit of bytes
				bit = 14; // in unit of bits
				break;
			case GVSP_PIX_MONO16:
				bpp = 2; // in unit of bytes
				bit = 16; // in unit of bits
				break;
			case GVSP_PIX_YUV411_PACKED:
			case GVSP_PIX_YUV422_PACKED:
			case GVSP_PIX_YUV444_PACKED:
				bpp = 2; // in unit of bytes
				bit = 16; // in unit of bits
				break;
			case GVSP_PIX_RGB8_PACKED:
			case GVSP_PIX_BGR8_PACKED:
				bpp = 3; // in unit of bytes
				bit = 8; // in unit of bits
				break;
			case GVSP_PIX_RGBA8_PACKED:
			case GVSP_PIX_BGRA8_PACKED:
				bpp = 4; // in unit of bytes
				bit = 8; // in unit of bits
				break;
			case GVSP_PIX_RGB10_PACKED:
			case GVSP_PIX_BGR10_PACKED:
				bpp = 4; // in unit of bytes
				bit = 10; // in unit of bits
				break;
			case GVSP_PIX_RGB12_PACKED:
			case GVSP_PIX_BGR12_PACKED:
				bpp = 6; // in unit of bytes
				bit = 12; // in unit of bits
				break;
			default:
				bpp = 1; // in unit of bytes
				bit = 8; // in unit of bits
				break;
			}

			// Adjust auxiliary registers
			framebuf_padding(bpp, (framebuf_control & FRAMEBUF_C_DEINT ? 1 : 0), 4 * ((framebuf_status & FRAMEBUF_S_BURST) >> 8));
		}

		#else // #if !defined (MODE_GIGE_10G)
	
		if( address == FPGA_AOI_BITWIDTH_ADRS
		||  address == FPGA_AOI_XSIZE_ADRS
		||  address == FPGA_AOI_YSIZE_ADRS
		||  address == FPGA_AOI_XOFFSET_ADRS
		||  address == FPGA_AOI_YOFFSET_ADRS
		||  address == RoiHeightSize
		||  address == RoiHeightOffset
		||  address == RoiHeightValid
		||  address == FIRM_DATA_ROI_AREA_MODE_ADRS
		||  address == FIRM_DATA_ROI_AREA_SIZE_ADRS
		||  address == RoiHeightDefault
		||  address == BINNING_HORIZONTAL
		||  address == BINNING_VERTICAL
		||  address == SpectrumBandStart
		||  address == SpectrumBandEnd)
		{
		    if ((video_chunk_ctrl & 0x80000000) == 0)       // Check if extended chunk mode is activated
		    {
		        chunk_size      = 0;
		        chunk_layout_id = 0;
		    }
		    else
		    {
		        if (video_chunk_enable == 0)                // Check if framecounter chunk is activated
		        {
		            chunk_size      = 8;                    // Additional bytes to describe image chunk only (4 byte id, 4 byte size)
		            chunk_layout_id = video_chunkid_img |
		                              video_pixfmt      |
		                              video_width       |
		                              video_height;         // Set chunk layout id
		        }
		        else
		        {
		            chunk_size      = 20;                   // Additional bytes to describe image chunk (8 bytes) and frame counter chunk (4 bytes data, 4 bytes id, 4 bytes size)
		            chunk_layout_id = video_chunkid_img |
		                              video_chunkid_fc  |
		                              video_pixfmt      |
		                              video_width       |
		                              video_height;         // Set chunk layout id
		        }
	
		        // Verify that chunk_layout_id gets changed when chunk layout changes
		        if (update_trailer == 1)
		        {
		            if (chunk_layout_id == old_chunk_layout_id)
		                chunk_layout_id++;
		            old_chunk_layout_id = chunk_layout_id;
		        }
		    }
	
		    // Adjust padding and total bytes per block
		    framebuf_padding(video_pixfmt, video_width, video_height, chunk_size);
		    gige_set_scmbs(0, framebuf_bpb);
	
		    // Set Payload Type
		    if ((video_chunk_ctrl & 0x80000000) == 0)
		        framebuf_set_pld_type(PLD_IMAGE);
		    else
		        framebuf_set_pld_type(PLD_IMAGE | PLD_EXTCHUNK_MODE);
	
		    // Update leader/trailer
		    if (update_leader)
		        framebuf_img_leader(video_pixfmt, video_width, video_height, video_offs_x, video_offs_y);
		    if (update_trailer)
		        framebuf_img_trailer(video_height, chunk_layout_id);
		}

		#endif // #if !defined (MODE_GIGE_10G)
		#endif // #if defined (IF_GIGE)

		// Indicates the current state at the LED of a rear-panel
		ledSetState(LED_STATE, (*status == GEV_STATUS_SUCCESS) ? LED_COMMAND : LED_STREAMING_ERROR, *status);

		// Stores the status counter where indicates happened errors
		if (*status == GEV_STATUS_SUCCESS)
		{
			//@@@1value2nd = IN32(BOARD_STATUS_GEV_CMDW_ADRS);
			//@@@1OUT32(BOARD_STATUS_GEV_CMDW_ADRS, (value2nd + 1));
		}
		else
		{
			// Increment the number of written error counter
			//@@@1ARM1_DIAG_INC_VALUE(CounterDiagnosticValue_NetworkCommandWriteError);
		}

		#if 0	//@@@1
		if (address <= xmlStartAddress || (address >= FPGA_BASE_ADRS && address < GENICAM_ADRS + 0x00FFFFFF))
		{
				DEBUG_PRINT("%s writes 0x%08X(%8d) at 0x%08X(0x%08X) %s with 0x%08X\r\n",
						GIGE_TAG_OUT, (unsigned int) value,
						(unsigned int) value, (unsigned int) address,(unsigned int) address2nd,
						(*status == 0) ? "success" : "failed",	(unsigned int) *status);
		}
		#endif //@@@1

		return;
}


#if defined (IF_GIGE)
// ---- User callback function -------------------------------------------------
//
//   This function should be used as an entry point for application-specific
//   code except interaction with a GigE Vision application which is handled
//   using the get_user_reg(), set_user_reg(), and send_message() functions
//   defined in the user.c file
//
//   Example shows sending asynchronous messages to the GigE Vision application
//

void user_callback(void)
{
#if 0///@@@1
	static u32 old_transfer = 0;
	static u32 old_acq_trigger = 0;
	static u32 old_frame_trigger = 0;
	static u32 old_exposure_start = 0;
	static u32 old_acquisition_active = 0;
	static u32 value = 0;
//#if !defined (MODE_ACQUISITION_TRG_EXTEND)
    //static unsigned long long timer = 0,timer_end = 0;
//#endif

	//--------------------------------------------------------
	// Send messages after start and stop of image acquisition
	//--------------------------------------------------------
	value = IN32(GENICAM_ACQUISITION_TRANSFER_ADRS); // Acquisition Transfer
	if ((value & 0x00000001) != (old_transfer & 0x00000001))
	{
#if !defined (MODE_ACQUISITION_TRG_EXTEND)
		// EventFrameTransferStart
		if (value & 0x00000001)
		{
			if (Event_Notification[Event_FrameTransferStart] == 1)
			{
				gige_send_message(GEV_EVENT_START_OF_TRANSFER, 0, 0, NULL);
			}
		}
		// EventFrameTransferEnd
		else
		{
        	//timer = timerGetCount64();
			if (Event_Notification[Event_FrameTransferEnd] == 1)
			{
				gige_send_message(GEV_EVENT_END_OF_TRANSFER, 0, 0, NULL);
			}
        	//timer_end = timerGetCount64();
		}
#endif // #if !defined (MODE_ACQUISITION_TRG_EXTEND)
		old_transfer = value;
	}

	//--------------------------------------------------------
	// Send messages after ExposureStart and ExposureEnd
	//--------------------------------------------------------
	//value = IN32(ACQUISITION_STATUS_EXPOSURE_ACTIVE);
	value = IN32(GENICAM_ACQUISITION_EXPOSURE_ACTIVE_ADRS);
	if ((value & 0x00000001) != (old_exposure_start & 0x00000001))
	{
#if !defined (MODE_ACQUISITION_TRG_EXTEND)
		// EventExpsoureStart
		if (value & 0x00000001)
		{
			if (Event_Notification[Event_ExposureStart] == 1)
			{
				gige_send_message(GEV_EVENT_START_OF_EXPOSURE, 0, 0, NULL);
			}
		}
		// EventExpsoureEnd
		else
		{
			if (Event_Notification[GEV_EVENT_ExposureEnd] == 1)
			{
				gige_send_message(GEV_EVENT_END_OF_EXPOSURE, 0, 0, NULL);
			}
		}
#endif // #if !defined (MODE_ACQUISITION_TRG_EXTEND)
		old_exposure_start = value;
	}

	//--------------------------------------------------------
	// Send messages after AcquisitionStart and AcquisitionEnd
	//--------------------------------------------------------
	//value = IN32(ACQUISITION_STATUS_ACQUISITION_ACTIVE);
	value = IN32(GENICAM_ACQUISITION_ACTIVE_ADRS);
	if ((value & 0x00000001) != (old_acquisition_active & 0x00000001))
	{
#if !defined (MODE_ACQUISITION_TRG_EXTEND)
		// EventAcquisitionStart
		if (value & 0x00000001)
		{
			if (Event_Notification[Event_AcquisitionStart] == 1)
			{
				gige_send_message(GEV_EVENT_ACQUISITION_START, 0, 0, NULL);
			}
		}
		// EventAcquisitionEnd
		else
		{
			if (Event_Notification[Event_AcquisitionEnd] == 1)
			{
				gige_send_message(GEV_EVENT_ACQUISITION_END, 0, 0, NULL);
			}
		}
#endif // #if !defined (MODE_ACQUISITION_TRG_EXTEND)
		old_acquisition_active = value;
	}

	//--------------------------------------------------------
	// Send messages
	//--------------------------------------------------------
	value = IN32(GENICAM_ACQUISITION_TRGWAIT_ADRS);
	if ((value & 0x00000001) != (old_acq_trigger & 0x00000001))
	{
#if !defined (MODE_ACQUISITION_TRG_EXTEND)
		// EventAcquisitionTrigger
		if (value & 0x00000001)
		{
			// Waiting for a trigger from GPI-1 or other logic
		}
		else
		{
			// A trigger has arrived at a frame.
			// An active signal has been falling edge.
			if (Event_Notification[Event_AcquisitionTrigger] == 1)
			{
				gige_send_message(GEV_EVENT_TRIGGER, 0, 0, NULL);
			}
		}
#endif // #if !defined (MODE_ACQUISITION_TRG_EXTEND)
		old_acq_trigger = value;
	}

	//--------------------------------------------------------
	// Send messages after start and stop of image acquisition
	//--------------------------------------------------------
	value = IN32(GENICAM_ACQUISITION_FRAME_TRGWAIT_ADRS); // Acquisition Transfer
	if ((value & 0x00000001) != (old_frame_trigger & 0x00000001))
	{
#if !defined (MODE_ACQUISITION_TRG_EXTEND)
		// EventFrameTrigger
		if (value & 0x00000001)
		{
			// Waiting for a trigger from GPI-1 or other logic
		}
		else
		{
			// A trigger has arrived at a frame.
			// An active signal has been falling edge.
			if (Event_Notification[Event_FrameTrigger] == 1)
			{
				gige_send_message(GEV_EVENT_TRIGGER, 0, 0, NULL);
			}
		}
#endif // #if !defined (MODE_ACQUISITION_TRG_EXTEND)
		old_frame_trigger = value;
	}
#endif //@@@1
	return;
}
#endif // #if defined (IF_GIGE)


#if defined (IF_GIGE) && defined (MODE_CAMERA_INTERRUPT)
// ---- User callback function -------------------------------------------------
//
//   This function should be used as an entry point for application-specific
//   code except interaction with a GigE Vision application which is handled
//   using the get_user_reg(), set_user_reg(), and send_message() functions
//   defined in the user.c file
//
//   Example shows sending asynchronous messages to the GigE Vision application
//
static void user_event_callback(u32 event_mask)
{
#if !defined (MODE_GIGE_10G)
	//static u32 value = 0;
    static unsigned long long timer = 0;
    //static unsigned long long timer_end = 0;
    //static double time = 0.0f;
	if(event_mask & FPGA_CAMERA_IRQ_EXPOSURE_END_BIT)
    {
		//timer = timerGetCount64();
		Event_Data[GEV_EVENT_ExposureEnd] = 0;
		Event_Timestamp[GEV_EVENT_ExposureEnd] = timer;
		Event_Data[GEV_EVENT_ExposureEnd] = 0;

		// An end of exposure event was aroused
		if(Event_Notification[GEV_EVENT_ExposureEnd] == 1)
		{
			// It will take a time 23,774 usec as about 24msec! Mya 21th 2019. MM.
			gige_send_message(GEV_EVENT_ExposureEnd, 0, 0, NULL);
			//timer_end = timerGetCount64();
			//time = (double)(timer_end - timer) * 2.9998f; // in usec (*2.9998f)
			//time = (double)(timer_end - timer) * (1000.0f / (float)(667.0f / 2.0f)); // in usec (*2.9998f)
			//DEBUG_PRINT("%s%s event 0x%08X(in %8f usec)\r\n",GIGE_TAG_OUT,
			//	(char*)"EoE", (unsigned int)event_mask, (double)time);
		}
    }
#endif // #if !defined (MODE_GIGE_10G)
}
#endif // #if defined (IF_GIGE) && defined (MODE_CAMERA_INTERRUPT)


#if defined (IF_GIGE)
// ---- Process the action signals ---------------------------------------------
//
//   This function must be always implemented!
//   It is called by the gige_callback() from libgige
//   The function is not allowed to block execution!
//
//   sig = array of four 32b bit fields identifying particular triggered signals
//
void action_trigger(u32 *sig)
{
	int i;

	for (i = 0; i < 128; i++)
	{
		if (sig[i / 32] & (0x80000000 >> (i % 32)))
		{
			printf("ACTION_CMD: triggered signal %d(0x%08X) in address 0x%08X\r\n", (unsigned int) i, (unsigned int) (0x80000000 >> ((unsigned int) i % 32)), (unsigned int) sig[i / 32]);
		}
	}

	return;
}
#endif // #if defined (IF_GIGE)


#if defined (IF_GIGE)
// ---- Initialize custom part of the device -----------------------------------
//
//   This function should initialize the rest of the device which is not under
//   control of the GIgE library
//
void user_init(u32* status)
{
#if !defined (MODE_GIGE_10G)
	u16 regvalue16 = 0;
#endif
	u32 i = 0, j = 0, address2nd = 0, value2nd = 0, value3rd = 0;
	u8 regAddress8 = 0;
	u8* str = 0;
	u32 size_of_string = 0;

	u8* strXML = 0;   // XML buffer
	char str16[16 + 1] = { 0 }; // temporary string buffer for coverting characters to integer
	char strXMLAddress[10] = { 0 };  // 61000000;
	char strXMLSize[6] = { 0 };  // 1DF1;
	//char strXMLFile[64] = { 0 };  //*.xml or *.zip;
	u32 xmlDemilitor = 0;
	u32 xmlCount = 0;
#if defined (MODE_GE_PACKET_SIZE_CUSTOM)
#if !defined (MODE_GIGE_10G)
	unsigned int data32;
#endif
#endif

#if defined (MODE_GE_PACKET_INFO_SAVE)
	unsigned int temp32;
#endif

#if defined (MODE_XML_SCHEMA_VERSION)
	int xmlScemaVersionFlag;
#endif

#if defined (MODE_GIGE_10G)
    // Chunk parameters
    u32 chunk_size;
    u32 chunk_layout_id = 0;    // Chunk layout id
#endif

#if defined (MODE_GE_PACKET_SIZE_CUSTOM)
#if !defined (MODE_GIGE_10G)
	// GigE Packet Size Custom
	data32 = IN32 (GEV_STM_SIZE_ADDR);
	data32 &= ~GEV_STM_SIZE_PACKET_SIZE_MASK;
	data32 |= (GE_PACKET_SIZE_DEFAULT & GEV_STM_SIZE_PACKET_SIZE_MASK);
	OUT32 (GEV_STM_SIZE_ADDR, data32);
#endif // #if !defined (MODE_GIGE_10G)
#endif


#if defined (MODE_GE_PACKET_INFO_SAVE)
#if 0
	//------------------------------------------------------------
	// GigE Index
	//------------------------------------------------------------
//#if defined (MODE_GIGE_10G)
	//OUT32 (GEV_STM_IDX, 0);
//#endif

	//------------------------------------------------------------
	// GigE Packet Size Custom
	//------------------------------------------------------------
	#if defined (MODE_GE_PACKET_SIZE_SAVE)
	data32 = IN32 (GEV_STM_SIZE_ADDR);

	// パケットサイズ取得
	temp32 = IN32 (FIRM_DATA_GEV_SCPS_PACKET_SIZE_ADRS);

	// レジスタ設定
	data32 &= ~GEV_STM_SIZE_PACKET_SIZE_MASK;
	data32 |= (temp32 & GEV_STM_SIZE_PACKET_SIZE_MASK);
	OUT32 (GEV_STM_SIZE_ADDR, data32);
	#endif // #if defined (MODE_GE_PACKET_SIZE_SAVE)

	//------------------------------------------------------------
	// GigE Packet Delay Custom
	//------------------------------------------------------------
	#if defined (MODE_GE_PACKET_DELAY_SAVE)
	temp32 = IN32 (FIRM_DATA_GEV_SCPD_PACKET_DELAY_ADRS);
	OUT32 (GEV_STM_DELAY_ADDR, temp32);
	#endif // #if defined (MODE_GE_PACKET_DELAY_SAVE)
#endif	// #if 0
#endif


    aoiGetWidthOffset((int*)&value2nd);
    set_user_reg(FPGA_AOI_XOFFSET_ADRS,value2nd,(u16*)status);

    aoiGetHeightOffset((int*)&value2nd);
    set_user_reg(FPGA_AOI_YOFFSET_ADRS,value2nd,(u16*)status);

    aoiGetWidth((int*)&value2nd);
    set_user_reg(FPGA_AOI_XSIZE_ADRS,value2nd,(u16*)status);

    aoiGetHeight((int*)&value2nd);

    #if defined (MODE_FRAMERATE_HIGH_SPEED)
    value3rd = MODE_DISABLE;
	sensorGetFrameRateHighSpeedMode ((int*)&value3rd);
    if (value3rd == MODE_ENABLE)
    {
    	if ((*status = sensorGetVirtualHeight((int*)&value3rd)) == AVAL_STATUS_SUCCESS)
    		value2nd = value3rd;
    }
	#endif // #if defined (MODE_FRAMERATE_HIGH_SPEED)

    set_user_reg(FPGA_AOI_YSIZE_ADRS,value2nd,(u16*)status);

	// PixelFormat
	if ((*status = cameraGetBitWidthGigE ((int *)&value2nd)) != AVAL_STATUS_SUCCESS)
	{
		//*status = GEV_STATUS_INVALID_PARAMETER;
		value2nd = GVSP_PIX_MONO12;
	}
	

	#if 0
	#if defined (MODE_AUTO_EXPOSURE) || defined (MODE_AUTO_GAIN)

 	//autoBrightGetExposureMax(&data32);
    //set_user_reg(FPGA_AOI_BITWIDTH_ADRS,value2nd,(u16*)status);
    //set_user_reg(GENICAM_ACQUISITION_EXPOSURE_ADRS,DEFAULT_EXPOSURE,(u16*)status);

	#else
 	set_user_reg(FPGA_AOI_BITWIDTH_ADRS,value2nd,(u16*)status);
	#endif

	#endif

	set_user_reg(FPGA_AOI_BITWIDTH_ADRS,value2nd,(u16*)status);

    /*
    video_width      = CAMERA_WIDTH_MAX; //620;
    video_height     = CAMERA_HEIGHT_MAX; //256;
    video_offs_x     = 0;
    video_offs_y     = 0;
    video_pixfmt     = GVSP_PIX_MONO8;
    */

    video_acq_mode   = ACQ_MODE_CONTINUOUS;
    video_max_width  = CAMERA_WIDTH_MAX;
    video_max_height = CAMERA_HEIGHT_MAX;
    video_gap_x      = 256; //1247;
    video_gap_y      = 256; //144;
    video_max_offset = CAMERA_HEIGHT_MAX;
    video_max_gap    = 4096;

    video_gap_x = 2000;
    video_gap_y = 2000;


//================================================================================
// GigE 10Gbps
//================================================================================
    //user_set_pixperclock(video_pixfmt);

    if ((video_chunk_ctrl & 0x80000000) == 0)       // Check if extended chunk mode is activated
    {
        chunk_size      = 0;
        chunk_layout_id = 0;
    }
    else
    {
        if (video_chunk_enable == 0)                // Check if framecounter chunk is activated
        {
            chunk_size      = 8;                    // Additional bytes to describe image chunk only (4 byte id, 4 byte size)
            chunk_layout_id = video_chunkid_img |
                              video_pixfmt      |
                              video_width       |
                              video_height;         // Set chunk layout id
        }
        else
        {
            chunk_size      = 20;                   // Additional bytes to describe image chunk (8 bytes) and frame counter chunk (4 bytes data, 4 bytes id, 4 bytes size)
            chunk_layout_id = video_chunkid_img |
                              video_chunkid_fc  |
                              video_pixfmt      |
                              video_width       |
                              video_height;         // Set chunk layout id
        }
    }

    // Setup GVSP leader and trailer packets
    framebuf_padding(video_pixfmt, video_width, video_height, chunk_size);
    framebuf_img_leader(video_pixfmt, video_width, video_height, video_offs_x, video_offs_y);
    framebuf_img_trailer(video_height, chunk_layout_id);
    gige_set_scmbs(0, framebuf_bpb);

    // Reset GCSR to default state
    video_gcsr = 0x00000000;
    gige_set_acquisition_status(0, 0);

	// Controls the behavior of the indicators (such as LEDs) showing the status of the Device
	DeviceIndicatorMode_Selecotr = DeviceIndicatorMode_Active;

	// --  Global Flash variables -------------
	Flash_UserSetSelector = CAMERA_SAVE_USER_NUM;
	Flash_UserSetLoad = 0;
	Flash_UserSetSave = 0;
	Flash_UserSetDefault = CAMERA_SAVE_USER_NUM;

//#if !defined(AXP_ABA001FIR_01) && !defined(AXP_ABA003FIR_01)
#if defined (MODE_GENICAM_COUNTER)
	// --  Global Counter variables -------------
	Counter_CounterSelector = IN32(GENICAM_COUNTER_SELECT_ADRS);
	// Initiates the CounterDiagnosticValue on the DDR:
	for (i = ARM1_DIAG_VALUE_START_ADRS; i <= ARM1_DIAG_VALUE_END_ADRS; i += 4)
	{
		// Clear the status of network and GenICam on the DDR shared memories
		OUT32(i, 0);
	}
#endif
	
#if defined (MODE_LUT)
	// --  Global LUT variables -------------
	#if defined (MODE_LUT_NUMBER_FIX)

	LUTSelector_Selector = LUT_SELECT_NUM;
	
	#else // #if defined (MODE_LUT_NUMBER_FIX)

	LUTSelector_Selector = Flash_UserSetSelector;

	if (LUTSelector_Selector == 0)
		LUTFormat_Selector = LUT_FORMAT_INC;
	else
		lutGetFormat(Flash_UserSetSelector, (int*) &LUTFormat_Selector);

	#endif // #if defined (MODE_LUT_NUMBER_FIX)

#endif // #if defined (MODE_LUT)

	
#if defined (MODE_FFC)
	// --  Global FFC variables -------------
	FlatFieldCorrectionBrightAdjustment_Lumi = (u32) IN32(FIRM_DATA_FFC_WHITE_ADRS);
	FlatFieldCorrectionBrightAdjustment_Lumi = max(2048,FlatFieldCorrectionBrightAdjustment_Lumi); // Better
	FlatFieldCorrectionBrightAdjustment_Lumi = min(FFC_WHITE_LEVEL_BIT14_MAX,FlatFieldCorrectionBrightAdjustment_Lumi); // Better
	OUT32(FIRM_DATA_FFC_WHITE_ADRS, FlatFieldCorrectionBrightAdjustment_Lumi);
	FlatFieldCorrectionModeSelector =	IN32 (FIRM_DATA_FFC_CORECTION_MODE_ADRS);
#endif
	
#if defined (MODE_DPC)
	// --  Global DPC Selector  DPC番号保存(メモリ) variables -------------
	address2nd = 0x0;
	cameraParamUserReadMem(CAMERA_SAVE_USER_NUM, CAMERA_SAVE_DPC_NUMBER_ADRS, (unsigned int*) &address2nd, (unsigned int*) &DefectPixelCorrection_Selector);

	DefectPixelCorrection_AdjustFfcSelector = 0;
	DefectPixelCorrection_AdjustBrightSelector = 0;
#endif
	
#if defined (MODE_FFC)
	// --  Global FFC variables -------------
	address2nd = 0x0;
	cameraParamUserReadMem(CAMERA_SAVE_USER_NUM, CAMERA_SAVE_FFC_NUMBER_ADRS, (unsigned int*) &address2nd, (unsigned int*) &FlatFieldCorrection_Selector);
#endif

	// --- Global Event Control variables ---------
	//Event_Selector = Event_FrameTransferStart;
#if !defined (MODE_GIGE_10G)
	Event_Selector = GEV_EVENT_ExposureEnd;
#endif

#if !defined (MODE_ACQUISITION_TRG_EXTEND)
	for (i = 0; i < EventNotification_Max; i++)
		Event_Notification[i] = 0;
#endif


#if defined(MODE_SPECTRUM_BANDGAIN_FILTER)
	//バンドゲインインデックス配列更新
	bgfSetBandGainIndex();
#endif

#if defined(MODE_SPECTRUM)
	spectrumSetBandIndex();
	#if defined(AXP_AHS052VIR_01) || defined(AXP_AHS052VIR_02)
	for(i = 0; i < ROI_SELECTOR_COUNT; i++)
	{
		roiSetSelector(i);
		roiGetValid((int*)&value2nd);
		roiHeightValidGe[i] = value2nd;
	}

	//　元に戻す
	roiSetSelector(0);
	#endif // #if defined(AXP_AHS052VIR_01) || defined(AXP_AHS052VIR_02)

#endif

#if defined (MODE_FRAMERATE_HIGH_SPEED)
	// Mode取得
	if (sensorGetFrameRateHighSpeedMode (&gHighSpeedMode) != AVAL_STATUS_SUCCESS)
		gHighSpeedMode = MODE_DISABLE;

	// Line Count取得
	if (gHighSpeedMode == MODE_ENABLE)
	{
		if (aoiGetHeight (&gHighSpeedModeLineCount) != AVAL_STATUS_SUCCESS)
			gHighSpeedModeLineCount = FRAME_RATE_HIGH_SPEED_MODE_LINE_COUNT_DEFAULT_GE;
	}
	else
	{
		gHighSpeedModeLineCount = FRAME_RATE_HIGH_SPEED_MODE_LINE_COUNT_DEFAULT_GE;
	}
#endif


	//---------------------------------------------------------------
	// DRRSモード取得
	//---------------------------------------------------------------
#if defined (MODE_SENSOR_DRRS)
	sensorGetDrrs (&gDrrsMode);
#endif
	

	//---------------------------------------------------------------
	//
	// Reads the first XML URL to the eeprom on the I2C bus via GigE IP Core
	//

	// ?SchemaVersion=1.0.0ありなしデータ設定(0=?SchemaVersion=1.0.0なし/1=?SchemaVersion=1.0.0あり※標準)
	OUT32 (FIRM_DATA_XML_SCHEMA_VERSION_MODE_ADRS, 0);

#if defined (MODE_XML_SCHEMA_VERSION)

	// 仕様-----------------------------------------------------------------------------
	//・DeviceSchemaVersionModeのFertureで"?SchemaVersion=1.0.0"のありなしを制御
	//・userset defaultでの起動ではAVALではあり、BVはなしとする
	//・userset default以外は何も制御しない
	//・BVで"?SchemaVersion=1.0.0"の有りにしたい場合、userset default以外の設定にし、あとはDeviceSchemaVersionModeの設定しだい
	// 仕様-----------------------------------------------------------------------------

	if (Flash_UserSetSelector == UserSet_Default)
	{
		if (strcmp (deviceModelName, "BV-C3103GE") == 0)
			xmlScemaVersionFlag = 1;
		else if  (strcmp (deviceModelName, "BV-C3110GE") == 0)
			xmlScemaVersionFlag = 1;
		else
			xmlScemaVersionFlag = 0;
	}
	else
	{
		xmlScemaVersionFlag = 0;
	}
#endif


	memset((void*) xmlURL, 0, 512);
	for (i = 0; i < GIGE_EEPROM_XML_URL_SIZE; i++)
	{
		regAddress8 = eeprom_read_byte(GIGE_EEPROM_XML_URL1_REG + i);
		eeprom_write_byte(GIGE_EEPROM_XML_URL2_REG + i, regAddress8); // Copy the character into the second URL.

		xmlURL[i] = regAddress8;
		if (i < 6)
		{
			xmlCount++;
			if (xmlCount == 6)
			{
				xmlDemilitor++;
				xmlCount = 0;
			}
		}
		else if (xmlDemilitor == 1)
		{
			//strXMLFile[xmlCount] = regAddress8;  // *.xml;
			xmlCount++;
			if (regAddress8 == ';')
			{
				//strXMLFile[xmlCount - 1] = '\0';
				xmlDemilitor++;
				xmlCount = 0;
			}
		}
		else if (xmlDemilitor == 2)
		{
			strXMLAddress[xmlCount] = regAddress8;  // 61000000;
			xmlCount++;
			if (regAddress8 == ';')
			{
				strXMLAddress[xmlCount - 1] = '\0';
				xmlDemilitor++;
				xmlCount = 0;
			}
		}
		else if (xmlDemilitor == 3)
		{
			strXMLSize[xmlCount] = regAddress8;  // 1DF1;
			xmlCount++;
			//if (regAddress8 == '?')
			if ((regAddress8 == '?') || (regAddress8 == 0))
			{
				strXMLSize[xmlCount - 1] = '\0';
				xmlDemilitor++;
				xmlCount = 0;

				if (regAddress8 == '?')
					OUT32 (FIRM_DATA_XML_SCHEMA_VERSION_MODE_ADRS, 1);	// ?SchemaVersion=1.0.0あり※標準
				else
					OUT32 (FIRM_DATA_XML_SCHEMA_VERSION_MODE_ADRS, 0);	// ?SchemaVersion=1.0.0なし

				// NULLなら終了
				if (regAddress8 == 0)
					break;

			#if defined (MODE_XML_SCHEMA_VERSION)
				if (xmlScemaVersionFlag == 1)
				{
					if (regAddress8 == '?')
					{
						// ?SchemaVersion=1.0.0削除
						eeprom_write_byte ((GIGE_EEPROM_XML_URL1_REG + i), 0x00);
						eeprom_write_byte ((GIGE_EEPROM_XML_URL2_REG + i), 0x00);

						// ?SchemaVersion=1.0.0なし
						OUT32 (FIRM_DATA_XML_SCHEMA_VERSION_MODE_ADRS, 0);

						// 終了
						break;
					}
				}
			#endif // #if defined (MODE_XML_SCHEMA_VERSION)
			}
		}
		else
		{
			// Done to parse the XML URL string
			if (regAddress8 == '\0')
				break;
		}
	}
	xmlURL[++i] = '\n';
	xmlStartAddress = strtol(strXMLAddress, (char**) &str16, 16);
	xmlSize = strtol(strXMLSize, (char**) &str16, 16);


	//---------------------------------------------------------------
	//
	// Copy the second XML URL from the first to the eeprom on the I2C bus via GigE IP Core
	//
	xmlStartAddressSecond = strtol(strXMLAddress, (char**) &str16, 16);
	xmlSizeSecond = strtol(strXMLSize, (char**) &str16, 16);
	memcpy((void*) xmlURLSecond, (void*) xmlURL, 512);

#if 0
	regAddress8 = xmlCount = xmlDemilitor = 0;
	memset((void*)xmlURLSecond,0,512);
	memset((void*)strXMLAddress,0,10);
	memset((void*)strXMLSize,0,6);
	memset((void*)strXMLFile,0,64);
	for( i=0; i < GIGE_EEPROM_XML_URL_SIZE; i++)
	{
		regAddress8 = eeprom_read_byte(GIGE_EEPROM_XML_URL2_REG + i);
		xmlURLSecond[i] = regAddress8;
		if(i<6)
		{
			xmlCount++;
			if(xmlCount==6)
			{
				xmlDemilitor++;
				xmlCount=0;
			}
		}
		else if(xmlDemilitor==1)
		{
			strXMLFile[xmlCount] = regAddress8;  // *.xml;
			xmlCount++;
			if(regAddress8==';')
			{
				strXMLFile[xmlCount-1]='\0';
				xmlDemilitor++;
				xmlCount=0;
			}
		}
		else if(xmlDemilitor==2)
		{
			strXMLAddress[xmlCount] = regAddress8;  // 61000000;
			xmlCount++;
			if(regAddress8==';')
			{
				strXMLAddress[xmlCount-1]='\0';
				xmlDemilitor++;
				xmlCount=0;
			}
		}
		else if(xmlDemilitor==3)
		{
			strXMLSize[xmlCount] = regAddress8;  // 1DF1;
			xmlCount++;
			if(regAddress8=='?')
			{
				strXMLSize[xmlCount-1]= '\0';
				xmlDemilitor++;
				xmlCount=0;
			}
		}
		else
		{
			// Done to parse the XML URL string
			if(regAddress8=='\0')break;
		}
	}
	xmlURLSecond[++i] = '\n';
	xmlStartAddressSecond = strtol(strXMLAddress,(char**)&str16,16);
	xmlSizeSecond = strtol(strXMLSize,(char**)&str16,16);
#endif

	// ---- Initiates Global variables for File Access Control --------------------------------
	fileSelector = 0;
	for (i = 0; i < FileSelector_MAX; i++)
	{
		fileSel[i] = FileSelector_SYSTEM;  // FileOperationSelector
		fileExec[i] = 0;                    // No execution
		fileMode[i] = FileOpenMode_Read;    // Default is read access
		fileSize[i] = 0;                // 0 zero byte as Default size of a file

		// Allocates the memories of LUT for the table 1
		switch (i)
		{
			case FileSelector_FPGA:
				fileBuffer[i] = (u32*)FIRM_UPDATE_ADRS;
				memset((void*) fileBuffer[i], 0, FIRM_UPDATE_SIZE);
				break;

			case FileSelector_XML:
				fileBuffer[i] = (u32*) malloc(XMLFILE_SIZE);
				if (fileBuffer[i] != 0)
				{
					// Success
					memset((void*) fileBuffer[i], 0, XMLFILE_SIZE);

					// Reads the XML file from the QSPI flash device
					address2nd = xmlStartAddress & BASE_NET_BOOTROM_XMLURL_MASK;
					if (XMLFILE_SIZE < xmlSize)
					{
						// Exceeds the limitation size of buffer for the xml's file
						*status = AVAL_STATUS_RESOURCE_EXHAUSTED;
#ifdef DEBUG_MODE
#if DEBUG_MODE_MSG
						DEBUG_PRINT("Resourse XML FIle's memory was failed 0x%08X\n",status);
#endif //DEBUG_MODE_MSG
#endif //DEBUGMODE
					}
					else if (xmlStartAddress != 0 && xmlSize != 0)
					{
						*status = qspiFlashRead((unsigned int) address2nd, (unsigned char*) fileBuffer[i], (unsigned int) xmlSize);
						strXML = (u8*) fileBuffer[i];
					}
					strXML = strXML;
				}
				else
				{
					*status = AVAL_STATUS_RESOURCE_EXHAUSTED;
#ifdef DEBUG_MODE
#if DEBUG_MODE_MSG
					DEBUG_PRINT("Allocating FPGA bitstream's memory was failed 0x%08X\n",status);
#endif //DEBUG_MODE_MSG
#endif //DEBUGMODE
				}
				break;

				case FileSelector_LUTLuminance0:
					fileBuffer[i] = (u32*) malloc(LUT_DATA_SIZE);
					if (fileBuffer[i] != 0)
					{
						// Success
						memset((void*) fileBuffer[i], 0, LUT_DATA_SIZE);
					}
					else
					{
						*status = AVAL_STATUS_RESOURCE_EXHAUSTED;
#ifdef DEBUG_MODE
#if DEBUG_MODE_MSG
						DEBUG_PRINT("Allocating LUT's memory was failed 0x%08X\n",status);
#endif //DEBUG_MODE_MSG
#endif //DEBUGMODE
					}
					break;

				case FileSelector_FFC0:
					fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
					break;

				case FileSelector_DPC0:
					fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
					break;

				case FileSelector_SENSOR_FPGA:
					fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
					break;

				case FileSelector_ADM:
					fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
					break;

				case FileSelector_SPECTRUM_WAVE:
					fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
					break;

				case FileSelector_IF_FPGA:
					fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
					break;

				case FileSelector_PHY:
					fileBuffer[i] = fileBuffer[FileSelector_FPGA];	// FileSelector_FPGAで確保した領域を使用
					break;

				default:
					fileBuffer[i] = 0;
					break;
				}

			for (j = 0; j < FileOperationeSelector_MAX; j++)
			{
				fileOffset[i][j] = 0;     //
				fileLength[i][j] = 0;     //
				fileStatus[i][j] = 0;     //
				fileResult[i][j] = 0;     //
			}
		}

		// Device persistent information on the EEPROM
		//---------------------------------------------------------------
#if 0
		memcpy((void*)deviceVendorName,(void*)FIRM_DATA_VENDOR_ADRS,GIGE_EEPROM_CUST_VENDOR_SIZE);
		memcpy((void*)deviceModelName,(void*)FIRM_DATA_MODEL_ADRS,GIGE_EEPROM_CUST_MODEL_SIZE);
		memcpy((void*)deviceManufacturerInfo,(void*)FIRM_DATA_MANUFACTURE_ADRS,GIGE_EEPROM_CUST_MANUFACTURER_SIZE);
#endif
		// Frimware2 version
		str = (u8*) deviceFirmwareVersion;
		size_of_string = sizeof(deviceFirmwareVersion) + 1;
		for (i = 0; i < size_of_string; i++)
		{
			regAddress8 = str[i];
			eeprom_write_byte(GIGE_EEPROM_CUST_FIRMWARE_VER_ADRS + i,regAddress8);
		}

		// Network persistent information on the EEPROM
		//---------------------------------------------------------------

		//---------------------------------------------------------------
		//
		// Write the GVCP Port Number to the eeprom on the I2C bus via GigE IP Core
		// FIXED: To avoid the incorrect data such as an incremental number of diag data
		eeprom_write_dword(GIGE_EEPROM_GVCP_PORT_REG, GIGE_EEPROM_GVSP_UDP_DATA);

		//---------------------------------------------------------------
		//
		// Writes the fixed configuration IP to the eeprom on the I2C bus via GigE IP Core
		//
		// D2 : LLA
		// D1 : DHCP
		// D0 : Persistent IP
		value2nd = eeprom_read_dword(GIGE_EEPROM_CURRENT_IP_REG);
		value2nd = SWAP_L(value2nd);
		if (value2nd & GIGE_EEPROM_IP_CONFIG_PERSISTENT_IP)
		{
			value3rd = eeprom_read_dword(GIGE_EEPROM_PERSISTENT_IP_REG);
			if (value3rd == 0)
			{
				// The ignore persistent IP address is found
				// 0x6 = LLA=On and DHCP=On only except for Persistent IP
				value2nd = GIGE_EEPROM_IP_CONFIG_DATA;
				value2nd = SWAP_L(value2nd);
				eeprom_write_dword(GIGE_EEPROM_CURRENT_IP_REG, value2nd);
			}
		}

		//---------------------------------------------------------------
		//
		// Read the firmare update from the eeprom on the I2C bus via GigE IP Core
		//
		memset(firmUpdate, 0, GIGE_EEPROM_CUST_UPDATE_TIMESTAMP_SIZE);
		for (i = 0; i < GIGE_EEPROM_CUST_UPDATE_TIMESTAMP_SIZE; i++)
		{
			regAddress8 = eeprom_read_byte(GIGE_EEPROM_CUST_UPDATE_TIMESTAMP_ADRS + i);
			firmUpdate[i] = regAddress8;
		}
#if 1
		if (strlen(firmUpdate) <= 1)
		{
			sprintf(firmUpdate, "%s %s", __DATE__, __TIME__);
			for (i = 0; i < GIGE_EEPROM_CUST_UPDATE_TIMESTAMP_SIZE; i++)
			{
				regAddress8 = firmUpdate[i];
				eeprom_write_byte(GIGE_EEPROM_CUST_UPDATE_TIMESTAMP_ADRS + i, regAddress8);
			}
		}
#endif

		//
		// Initiates the Network configuration if enable on DIPSW 2.
		//---------------------------------------------------------------
#if !defined(AXP_ABA001FIR_01) && !defined(AXP_ABA003FIR_01)
		if ((g_dipsw & 0x1) || (g_dipsw & 0x2))
		{
			user_network_init((u32*) &status);
		}
#endif

	//---------------------------------------------------------------
	//
	// Register the callback routine to cameraInit.c in order to gets interruption of events.
	// Masahide Matsubara, May 21th 2019
	//
#if defined(MODE_CAMERA_INTERRUPT)
	*status = cameraIntRegister((p_user_event_callback)user_event_callback);
#endif // #if defined(MODE_CAMERA_INTERRUPT)

		return;
	}
#endif //#if defined (IF_GIGE)


#if defined (IF_GIGE)
//**********************************************************************************
//	Initiates the network configuration for DeviceFactory
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		status				：statusを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
void user_network_init (u32* status)
{
	u32 value = 0, value2nd = 0;

	//---------------------------------------------------------------
	//
	// Write the factory-setting of IP,Subnet and Gateway whenever DIPSW is ON
	//

	// Initiates IP , subnet and Gateway at once
	eeprom_write_dword(GIGE_EEPROM_PERSISTENT_IP_REG, GIGE_EEPROM_IP_DATA);
	eeprom_write_dword(GIGE_EEPROM_PERSISTENT_SUB_MASK_REG, GIGE_EEPROM_SUBNET_DATA);
	eeprom_write_dword(GIGE_EEPROM_PERSISTENT_DEF_GATEWAY_REG, GIGE_EEPROM_GATEWAY_DATA);
	eeprom_write_dword(GIGE_EEPROM_GVCP_PORT_REG, GIGE_EEPROM_GVSP_UDP_DATA);

	//---------------------------------------------------------------
	//
	// Writes the fixed configuration IP to the eeprom on the I2C bus via GigE IP Core
	//
	// D2 : LLA
	// D1 : DHCP
	// D0 : Persistent IP
	value2nd = eeprom_read_dword(GIGE_EEPROM_CURRENT_IP_REG);
	value2nd = SWAP_L(value2nd);
	value = GIGE_EEPROM_IP_CONFIG_DATA; // 0x6 = LLA=On and DHCP=On only except for Persistent IP
	value = SWAP_L(value);
	eeprom_write_dword(GIGE_EEPROM_CURRENT_IP_REG, value);
	//value2nd = eeprom_read_dword(GIGE_EEPROM_CURRENT_IP_REG);

#if 0
	DEBUG_PRINT("%s config: %s %s %s at 0x%08X [before: 0x08X]\r\n",GIGE_EEPROM_OUT,
			(0x4 & regAddress2nd32)?"LLA=On":"LLA=Off",
			(0x2 & regAddress2nd32)?"DHCP=On":"DHCP=Off",
			(0x1 & regAddress2nd32)?"Persistent=On":"Persistent=Off",
			(unsigned int)GIGE_EEPROM_CURRENT_IP_REG,value2nd);
#endif

	*status = 0;
	return;
}
#endif // #if defined (IF_GIGE)

	
#if defined (IF_GIGE)
//**********************************************************************************
//	Initiates the network configuration  for DeviceFactory
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		status				：statusを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
void user_userid_init(u32* status)
{
	u32 i = 0, size = 16;
	u8 valu8 = 0;

	//---------------------------------------------------------------
	//
	// Write the serial number to the eeprom on the I2C bus via GigE IP Core
	//
	for (i = 0; i < size; i++)
	{
		eeprom_write_byte(GIGE_EEPROM_USER_ID_REG + i, valu8);
	}

	*status = 0;
	return;
}
#endif // #if defined (IF_GIGE)


//**********************************************************************************
//	user_info_get
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		strVendor			：Vendorを格納するポインタ
//		strModel			：Modeを格納するポインタ
//		strManufacturer		：Manufactureを格納するポインタ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================

void user_info_get(u8* strVendor, u8* strModel, u8* strManufacturer)
{
	//unsigned int regAddress32 = 0;
	int value = 0;

	consoleGetMode (&value);

	//---------------------------------------------------------------
	//
	// Reads the DeviceVendorName in the eeprom on the I2C bus via GigE IP Core
	//
	if (strVendor != 0)
	{
		memset((void*) strVendor, 0, GIGE_EEPROM_CUST_VENDOR_SIZE);
		memcpy((void*) strVendor, (void*) FIRM_DATA_VENDOR_ADRS, GIGE_EEPROM_CUST_VENDOR_SIZE);
		DEBUG_PRINT("%s Vendor: %s\r\n", GIGE_EEPROM_OUT,(char*) strVendor);
	}

	//---------------------------------------------------------------
	//
	// Reads the DeviceModelName in the eeprom on the I2C bus via GigE IP Core
	//
	if (strModel != 0)
	{
		memset((void*) strModel, 0, GIGE_EEPROM_CUST_MODEL_SIZE);
		memcpy((void*) strModel, (void*) FIRM_DATA_MODEL_ADRS, GIGE_EEPROM_CUST_MODEL_SIZE);
		DEBUG_PRINT("%s Model: %s\r\n", GIGE_EEPROM_OUT, (char*) strModel);
	}

	//---------------------------------------------------------------
	//
	// Reads the DeviceManufacturerInfo in the eeprom on the I2C bus via GigE IP Core
	//
	if (strManufacturer != 0)
	{
		memset((void*) strManufacturer, 0, GIGE_EEPROM_CUST_MANUFACTURER_SIZE);
		memcpy((void*) strManufacturer, (void*) FIRM_DATA_MANUFACTURE_ADRS, GIGE_EEPROM_CUST_MANUFACTURER_SIZE);
		DEBUG_PRINT("%s Manufacture: %s\r\n", GIGE_EEPROM_OUT,(char*) strManufacturer);
	}
}


#if defined (IF_GIGE)
//**********************************************************************************
//	LED: ledLinkState
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledLinkState(u32 linkStatus, volatile u32* linkStatusPre,volatile u32* ccpPre)
{
	int status = AVAL_STATUS_SUCCESS;
	int speed, speedOld;
	static int startFlag = 0;
	#if defined (MODE_GE_PACKET_DELAY_SAVE)
	unsigned int data32;
	#endif

	if (*linkStatusPre != linkStatus)
	{
		if (*linkStatusPre == DeviceStatus_LinkUndetection)
		{
			// Get Speed
			if ((status = phyGetSpeedDuplex (&speed)) !=  AVAL_STATUS_SUCCESS)
				goto _DONE;

			// Get Old Speed
			gigeGetSpeed (&speedOld);

			// Set Speed
			gigeSetSpeed (speed);

			// 1Gbpsよりも低い速度の場合はリブート
			if (speed < GIGE_SPEED_1GIG_FD)
			{
				DEBUG_PRINT("%s [LED] Reseting ...\n", GIGE_CONNECTION_STATE);

				// Reboot its own device...
				//@@@1
				//@@@1executeCommandDeviceReset();
				//@@@1
			}

			// linked up
			ledSetState(LED_POWER, LED_CONNECTED, 0);
			ledSetState(LED_STATE, LED_OFF, 0);

			#if defined (MODE_GE_PACKET_DELAY_SAVE)
			// GigE Packet Delay(DDR=>Regに設定)
			data32 = IN32 (FIRM_DATA_GEV_SCPD_PACKET_DELAY_ADRS);
			OUT32 (GEV_STM_DELAY_ADDR, data32);
			//gige_set_stm_delay(0, data32);
			#endif // #if defined (MODE_GE_PACKET_DELAY_SAVE)

			// Resets the series of commands
			//@@@1user_command_init ((u32 *)&status);
			
			// GigE Connection Config
			if ((speedOld != speed) || (startFlag == 0)) // 異なる設定になったら実行 || 最初の一回は実行
			{
				gigeSetConnectionConfig ();
				startFlag = 1;
			}
		}
		else	// Link Down
		{
			DEBUG_PRINT("%s [LED] Link Down \n", GIGE_CONNECTION_STATE);
			ledSetState(LED_POWER, LED_DISCONNECTION, 0);
			ledSetState(LED_STATE, LED_OFF, 0);

			#if defined (MODE_GE_PACKET_DELAY_SAVE)
			// GigE Packet Delay(Reg=>DDRに保存)
			data32 = IN32 (GEV_STM_DELAY_ADDR);
			OUT32 (FIRM_DATA_GEV_SCPD_PACKET_DELAY_ADRS, data32);
			#endif // #if defined (MODE_GE_PACKET_DELAY_SAVE)

			// Resets the series of commands
			//@@@1user_command_init((u32 *)&status);

			// Abort the previous acquisition
			acquisitionAbort();
		}

		// 今回のステータスを保存
		*linkStatusPre = linkStatus;
	}

#if 0
	if (linkStatus == DeviceStatus_LinkDetection)
	{
		// The camera has already a ethernt connection with a host NOT GigE Vision.
		// D0-15: Control switch-over key
		//   D29: Control switch-over enable bit
		//   D30: Control Access bit
		//   D31: Exclusive Access bit
		value = (u32) gvcp_get_reg(GEV_REG_CCP, (u16*) &status);
		access = value & 0x3;
		if (*ccpPre != access)
		{
			// Changed the state where has been released or opened by the primary application.
			switch (access)
			{
			case GEV_CCP_OPEN_ACCESS:
				// Not open yet
				//ledSetState(LED_POWER, LED_STREAMING_ERROR, 0);
				//ledSetState(LED_STATE, LED_STREAMING_ERROR, 0);
				break;
			case GEV_CCP_EXCLUSIVE_ACCESS:
				// Opened with Exclusive access
				for (i = 0; i < 2; i++)
				{
					ledSetState(LED_POWER, LED_COMMAND, 0);
					ledSetState(LED_STATE, LED_COMMAND, 0);
				}
				break;
			case GEV_CCP_CONTROL_ACCESS:
				// Opened with Control access
				for (i = 0; i < 2; i++)
				{
					ledSetState(LED_POWER, LED_COMMAND, 0);
					ledSetState(LED_STATE, LED_COMMAND, 0);
				}
				break;
			case GEV_CCP_EXCLUSIVE_ACCESS + GEV_CCP_CONTROL_ACCESS:
				// Opened with Exclusive Control access
				for (i = 0; i < 2; i++)
				{
					ledSetState(LED_POWER, LED_COMMAND, 0);
					ledSetState(LED_STATE, LED_COMMAND, 0);
				}
				break;
			default:
				break;
			}
		}
		else
		{
			// No change
		}
		*ccpPre = access;
	}
#endif
	
_DONE:
	return (status);
}
#endif // #if defined (IF_GIGE)


//**********************************************************************************
//	LED Set:
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
void ledSetState (u32 type_of_led, u32 state, int status)
{
	switch (DeviceIndicatorMode_Selecotr)
	{
		case DeviceIndicatorMode_Inactive:
			// Nothing to indicators
			break;
		case DeviceIndicatorMode_Active:
			setLed(type_of_led, state);
			break;
		case DeviceIndicatorMode_ErrorStatus:
			if (status != GEV_STATUS_SUCCESS)
			{
				setLed(type_of_led, state);
			}
			break;
		default:
			setLed(type_of_led, state);
			break;
	}
}


#if defined (IF_GIGE)
//**********************************************************************************
//	LED: ledState
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int ledState (u32 linkStatus)
{
	u32 value = 0, cmd = 0; //@@@1address = 0;
	u32 value2nd = 0;
	int index = 0, found_index = 0, found_index_pre = 0;
	int status = 0;

	if (linkStatus == 0)
	{
		// Disconnection
		ledSetState(LED_POWER, LED_DISCONNECTION, 0);
		ledSetState(LED_STATE, LED_OFF, 0);
	}
	else
	{
		// Waiting an action
		value = IN32(GENICAM_ACQUISITION_TRGWAIT_ADRS);
		value2nd = IN32(GENICAM_ACQUISITION_FRAME_TRGWAIT_ADRS);
		if (value == 1 || value2nd == 1)
		{
			ledSetState(LED_STATE, LED_WAIT_CMD, 0);
		}
		else
		{
			getLed(LED_POWER, (int*) &value);
			if (value == LED_WAIT_CMD)
			{
				ledSetState(LED_STATE, LED_CONNECTED, 0);
			}

			// No waiting an action and trigger
			// Streaming
			value = IN32(GENICAM_ACQUISITION_ACTIVE_ADRS);
			if (value == 1)
			{
				// Now under AcquisitionActive but no streaming
				value = IN32(GENICAM_ACQUISITION_TRANSFER_ADRS);
				if (value == 1)
				{
					// During streaming
					ledSetState(LED_STATE, LED_STREAMING, 0);
				}
				else
				{
					// No-streaming and wait a next command or trigger
					getLed(LED_STATE, (int*) &value);
				}
			}
			else
			{
				// Just the state where stay a connection active.
				getLed(LED_STATE, (int*) &value);

				if ((value == LED_STREAMING) || (value == LED_WAIT_CMD))
				{
					ledSetState(LED_STATE, LED_OFF, 0);
				}
			}
		}
#if 0//@@@1
		// Command
		{
			// Gets the current command
			cmd = IN32(FIRM_CMD_CMD_ADRS);
			status = IN32(FIRM_CMD_ERROR_ADRS); // Gets the current status
			if (cmd == FIRM_CMD_ERROR)
			{
				//@@@1address = 0;

				// Indicates the error indicator
				ledSetState(LED_STATE, LED_SYSTEM_ERROR, GEV_STATUS_LOCAL_PROBLEM);

				// Clear the error command and status first.
				OUT32(FIRM_CMD_CMD_ADRS, FIRM_CMD_OK);
				OUT32(FIRM_CMD_ERROR_ADRS, FIRM_CMD_OK);
				switch (cmd_prev) {
				//  FFC
				case FIRM_CMD_DEFAULT_ALL:
					//@@@1address = BOARD_STATUS_GEV_USERSET_ADRS;
					break;
				case FIRM_CMD_FFC_LOAD:
				case FIRM_CMD_FFC_LOAD_ADMIN:
				case FIRM_CMD_FFC_SAVE:
				case FIRM_CMD_FFC_SAVE_FACTORY:
				case FIRM_CMD_FFC_DEFAULT:
				case FIRM_CMD_FFC_CORRECTION_MODE:

				case FIRM_CMD_FFC_WHITE:
				case FIRM_CMD_FFC_BLACK:
				case FIRM_CMD_FFC_ADJUST_UPDATE:
					//@@@1address = BOARD_STATUS_GEV_FFC_ADRS;
					break;
					//  LUT
				case FIRM_CMD_LUT_SAVE:
				case FIRM_CMD_LUT_LOAD:
				case FIRM_CMD_LUT_DEFAULT:
				case FIRM_CMD_LUT_WRITE:
				case FIRM_CMD_LUT_READ:
					//@@@1address = BOARD_STATUS_GEV_LUT_ADRS;
					break;
					//  UserSet
				case FIRM_CMD_USERSET_SAVE:
				case FIRM_CMD_USERSET_LOAD:
				case FIRM_CMD_USERSET_DEFAULT:
				case FIRM_CMD_USERSET_BOOT:
					//@@@1address = BOARD_STATUS_GEV_USERSET_ADRS;
					break;
					// DeviceUserID
				case FIRM_CMD_USERID_SET:
				case FIRM_CMD_USERID_GET:
					break;

					//  DPC
				case FIRM_CMD_DPC_LOAD:
				case FIRM_CMD_DPC_LOAD_ADMIN:
				case FIRM_CMD_DPC_SAVE_FACTORY:
				case FIRM_CMD_DPC_SAVE_ADMIN:
				case FIRM_CMD_DPC_DEFAULT:
				case FIRM_CMD_DPC1_FACTORY:
				case FIRM_CMD_DPC2_FACTORY:
				case FIRM_CMD_DPC3_FACTORY:
				case FIRM_CMD_DPC_ABORT:
				case FIRM_CMD_DPC_GET_COUNT:
				case FIRM_CMD_DPC_DECREMENT_COUNT:
				case FIRM_CMD_DPC_ADJUST_UPDATE:
					//@@@1address = BOARD_STATUS_GEV_DPC_ADRS;
					break;

				case FIRM_CMD_AGING:
					//@@@1address = BOARD_STATUS_GEV_AGING_ADRS;
					break;

				case FIRM_CMD_CAMERA_VENDOR:
				case FIRM_CMD_CAMERA_MODEL:
				case FIRM_CMD_CAMERA_MANUFACTURE:
				case FIRM_CMD_CAMERA_BOARDID:
				case FIRM_CMD_CAMERA_SENSORID:
					//@@@1address = BOARD_STATUS_GEV_CMDW_ADRS;
					break;

				default:
					break;
				}

				// Stores the status of error
				//@@@1if (address != 0)
				//@@@1{
					//@@@1value = IN32(address);
					//@@@1OUT32(address, (value + 1));
				//@@@1}
			}

			if (cmd == 0 && cmd_prev != 0)
			{
				// The previous command is done
				timer_us = (u32) timerGetUsCount(); // Gets the tick us counter
				for (index = 0;; index++)
				{
					if (gevcmdTbl[index].name == NULL)
					{
						break;
					}

					if (gevcmdTbl[index].cmd == cmd_prev)
					{
						gevcmdTbl[index].status = status;
					}
				}
				cmd_prev = cmd;
			}
			else if (cmd != 0 && cmd_prev == 0)
			{
				// The new command has been started...
				timerSetCount(0);// Resets the tick counter
				cmd_prev = cmd;
			}
			else if (cmd != cmd_prev)
			{
				found_index = -1;
				found_index_pre = -1;
				for (index = 0;; index++)
				{
					if (gevcmdTbl[index].name == NULL|| (found_index != -1 && found_index_pre != -1))
					{
						break;
					}
					if (gevcmdTbl[index].cmd == cmd)
					{
						found_index = index;
					}
					if (gevcmdTbl[index].cmd == cmd_prev)
					{
						found_index_pre = index;
					}
				}

				cmd_prev = cmd;
			}
		}
#endif	//@@@1
		// Diagnostic
		if (DeviceBuiltInTest_Mode != DeviceBuiltInTest_Done)
		{
			ledSetState(LED_POWER, LED_DIAG, 0);
		}

	}

	return (AVAL_STATUS_SUCCESS);
}
#endif // #if defined (IF_GIGE)


#if defined (IF_GIGE)
//**********************************************************************************
//	gigeOutputEeprom
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		     					：
//	[ OUTPUT ]
//  RETURNS						：none
//  							： Shows the information about eeprom of GigE Bootstrap registers
//								：
//==================================================================================
void gigeOutputEeprom()
{
	u32 i = 0;
	u8 regAddress8 = 0;
	u32 regAddress32 = 0, regAddress2nd32 = 0;
	char str6[6] = { 0 };
	char str16[16] = { 0 };
	DEBUG_PRINT("\n\r");

	DEBUG_PRINT("==================== Build Date: %s ======================\r\n",__DATE__);

	DEBUG_PRINT("%s Dip-Switch: Factory Reset=%d, Network Reset=%d, GPOUT1=%d, GPOUT2=%d \r\n", GIGE_EEPROM_OUT, 
	(u32) (g_dipsw & 0x1), (u8) ((g_dipsw >> 1) & 0x1),(u32) ((g_dipsw >> 2) & 0x1),(u32) ((g_dipsw >> 3) & 0x1));

	//---------------------------------------------------------------
	//
	// Reads the DeviceVendorName in the eeprom on the I2C bus via GigE IP Core
	//
#if 0
	memset(str16,0,GIGE_EEPROM_CUST_VENDOR_SIZE);
	for( i=0; i < GIGE_EEPROM_CUST_VENDOR_SIZE; i++)
	{
		regAddress32 = eeprom_read_byte(GIGE_EEPROM_CUST_VENDOR_ADRS + i);
		deviceVendorName[i] = regAddress32 & 0x000000FF;
	}
	deviceVendorName[GIGE_EEPROM_CUST_VENDOR_SIZE-1] = '\0';
	DEBUG_PRINT("%s Vendor: %s at 0x%08X\r\n",GIGE_EEPROM_OUT,
			(char*)deviceVendorName,(unsigned int)GIGE_EEPROM_CUST_VENDOR_ADRS);
#endif
	DEBUG_PRINT("%s Vendor: %s at 0x%08X\r\n", GIGE_EEPROM_OUT,
			(char*) FIRM_DATA_VENDOR_ADRS,(unsigned int) FIRM_DATA_VENDOR_ADRS);

	//---------------------------------------------------------------
	//
	// Reads the DeviceModelName in the eeprom on the I2C bus via GigE IP Core
	//
#if 0
	memset(deviceModelName,0,GIGE_EEPROM_CUST_MODEL_SIZE);
	for( i=0; i < GIGE_EEPROM_CUST_MODEL_SIZE; i++)
	{
		regAddress32 = eeprom_read_byte(GIGE_EEPROM_CUST_MODEL_ADRS + i);
		deviceModelName[i] = regAddress32 & 0x000000FF;
	}
	deviceManufacturerInfo[GIGE_EEPROM_CUST_MODEL_SIZE-1] = '\0';
	DEBUG_PRINT("%s Model: %s at 0x%08X\r\n",GIGE_EEPROM_OUT,
			(char*)deviceModelName,(unsigned int)GIGE_EEPROM_CUST_MODEL_ADRS);
#endif
	DEBUG_PRINT("%s Model: %s at 0x%08X\r\n", GIGE_EEPROM_OUT,
			(char*) FIRM_DATA_MODEL_ADRS,(unsigned int) FIRM_DATA_MODEL_ADRS);

	//---------------------------------------------------------------
	//
	// Reads the DeviceManufacturerInfo in the eeprom on the I2C bus via GigE IP Core
	//
#if 0
	memset(deviceManufacturerInfo,0,GIGE_EEPROM_CUST_MANUFACTURER_SIZE);
	for( i=0; i < GIGE_EEPROM_CUST_MANUFACTURER_SIZE; i++)
	{
		regAddress32 = eeprom_read_byte(GIGE_EEPROM_CUST_MANUFACTURER_ADRS + i);
		deviceManufacturerInfo[i] = regAddress32 & 0x000000FF;
	}
	deviceManufacturerInfo[GIGE_EEPROM_CUST_MANUFACTURER_SIZE-1] = '\0';
	DEBUG_PRINT("%s Manufacture: %s at 0x%08X\r\n",GIGE_EEPROM_OUT,
			(char*)deviceManufacturerInfo,(unsigned int)GIGE_EEPROM_CUST_MANUFACTURER_ADRS);
#endif
	DEBUG_PRINT("%s Manufacture: %s at 0x%08X\r\n", GIGE_EEPROM_OUT,
			(char*) FIRM_DATA_MANUFACTURE_ADRS,	(unsigned int) FIRM_DATA_MANUFACTURE_ADRS);

	//---------------------------------------------------------------
	//
	// Reads the Firmware Version in the eeprom on the I2C bus via GigE IP Core
	//
	DEBUG_PRINT("%s Firmware: Ver %s on update %s\r\n",GIGE_EEPROM_OUT,deviceFirmwareVersion,firmUpdate);

	//---------------------------------------------------------------
	//
	// MAC address
	//
	for (i = 0; i < GIGE_EEPROM_MAC_SIZE; i++)
	{
		str6[i] = eeprom_read_byte(GIGE_EEPROM_MAC_HIGH_REG + i);
	}
	DEBUG_PRINT("%s MAC: %X:%X:%X:%X:%X:%X at 0x%08X\r\n",
			GIGE_EEPROM_OUT, str6[0], str6[1], str6[2], str6[3],
			str6[4], str6[5],(unsigned int) GIGE_EEPROM_MAC_HIGH_REG);

	//---------------------------------------------------------------
	//
	// Reads the configuration IP to the eeprom on the I2C bus via GigE IP Core
	// FIXED:
	// D2 : LLA
	// D1 : DHCP
	// D0 : Persistent IP
#if 0
	regAddress32 = eeprom_read_dword(GIGE_EEPROM_CURRENT_IP_REG);
	regAddress2nd32 = SWAP_L(regAddress32);
	DEBUG_PRINT("%s config: %s %s %s at 0x%08X\r\n",GIGE_EEPROM_OUT,
			(0x4 & regAddress2nd32)?"LLA=On":"LLA=Off",
			(0x2 & regAddress2nd32)?"DHCP=On":"DHCP=Off",
			(0x1 & regAddress2nd32)?"Persistent=On":"Persistent=Off",
			(unsigned int)GIGE_EEPROM_CURRENT_IP_REG);
#endif
	regAddress2nd32 = eeprom_read_byte(GIGE_EEPROM_CURRENT_IP_REG);
	//regAddress2nd32  = SWAP_L(regAddress2nd32);
	regAddress8 = 0xFF & regAddress2nd32;
	DEBUG_PRINT("%s config: %s %s %s at 0x%08X\r\n", GIGE_EEPROM_OUT,
			(0x4 & regAddress8) ? "LLA=On" : "LLA=Off",
			(0x2 & regAddress8) ? "DHCP=On" : "DHCP=Off",
			(0x1 & regAddress8) ? "Persistent=On" : "Persistent=Off",
			(unsigned int) GIGE_EEPROM_CURRENT_IP_REG);

	//---------------------------------------------------------------
	//
	// Reads the GVCP Port Number to the eeprom on the I2C bus via GigE IP Core
	// FIXED: To avoid the incorrect data such as an incremental number of diag data
	regAddress32 = eeprom_read_dword(GIGE_EEPROM_GVCP_PORT_REG);
	DEBUG_PRINT("%s %d UDP port at 0x%08X\r\n", GIGE_EEPROM_OUT, regAddress32, (unsigned int) GIGE_EEPROM_GVCP_PORT_REG);

	//---------------------------------------------------------------
	//
	// Reads the Persistent IP to the eeprom on the I2C bus via GigE IP Core
	//
	regAddress32 = eeprom_read_dword(GIGE_EEPROM_PERSISTENT_IP_REG);
	DEBUG_PRINT("%s Persistent IP     : %d.%d.%d.%d at 0x%08X\r\n",	GIGE_EEPROM_OUT, 
			(0xFF000000 & regAddress32) >> 24,
			(0x00FF0000 & regAddress32) >> 16,
			(0x0000FF00 & regAddress32) >> 8,
			(0x000000FF & regAddress32) >> 0,
			(unsigned int) GIGE_EEPROM_PERSISTENT_IP_REG);

	//---------------------------------------------------------------
	//
	// Reads the Persistent sub-net mask data to the eeprom on the I2C bus via GigE IP Core
	//
	regAddress32 = eeprom_read_dword(GIGE_EEPROM_PERSISTENT_SUB_MASK_REG);
	DEBUG_PRINT("%s Persistent subnet : %d.%d.%d.%d at 0x%08X\r\n",	GIGE_EEPROM_OUT,
			(unsigned int) ((0xFF000000 & regAddress32) >> 24),
			(unsigned int) ((0x00FF0000 & regAddress32) >> 16),
			(unsigned int) ((0x0000FF00 & regAddress32) >> 8),
			(unsigned int) ((0x000000FF & regAddress32) >> 0),
			(unsigned int) GIGE_EEPROM_PERSISTENT_SUB_MASK_REG);

	//---------------------------------------------------------------
	//
	// Reads the Persistent default gateway to the eeprom on the I2C bus via GigE IP Core
	//
	regAddress32 = eeprom_read_dword(GIGE_EEPROM_PERSISTENT_DEF_GATEWAY_REG);
	DEBUG_PRINT("%s Persistent gateway: %d.%d.%d.%d at 0x%08X\r\n",	GIGE_EEPROM_OUT,
			(unsigned int) ((0xFF000000 & regAddress32) >> 24),
			(unsigned int) ((0x00FF0000 & regAddress32) >> 16),
			(unsigned int) ((0x0000FF00 & regAddress32) >> 8),
			(unsigned int) ((0x000000FF & regAddress32) >> 0),
			(unsigned int) GIGE_EEPROM_PERSISTENT_DEF_GATEWAY_REG);

	//---------------------------------------------------------------
	//
	// Reads the DeviceUserID in the eeprom on the I2C bus via GigE IP Core
	//
	memset(str16, 0, 16);
	for (i = 0; i < GIGE_EEPROM_USER_DEF_NAME_SIZE; i++)
	{
		regAddress32 = eeprom_read_byte(GIGE_EEPROM_USER_ID_REG + i);
		str16[i] = regAddress32 & 0x000000FF;
	}
	str16[10] = '\0';
	DEBUG_PRINT("%s UserID: %s at 0x%08X\r\n", GIGE_EEPROM_OUT,
			(char*) str16, (unsigned int) GIGE_EEPROM_USER_ID_REG);

	//---------------------------------------------------------------
	//
	// Reads the serial number to the eeprom on the I2C bus via GigE IP Core
	//
	memset(str16, 0, 16);
	for (i = 0; i < GIGE_EEPROM_SERIAL_NUM_SIZE; i++)
	{
		regAddress32 = eeprom_read_byte(GIGE_EEPROM_SERIAL_NUMBER_REG + i);
		str16[i] = regAddress32 & 0x000000FF;
	}
	str16[10] = '\0';
	DEBUG_PRINT("%s Serial: %s at 0x%08X\r\n", GIGE_EEPROM_OUT,
			(char*) str16, (unsigned int) GIGE_EEPROM_SERIAL_NUMBER_REG);

	//---------------------------------------------------------------
	//
	// Reads the first XML URL to the eeprom on the I2C bus via GigE IP Core
	DEBUG_PRINT("%s XML First  URL: %s at 0x%X in %d /0x%X in byte at 0x%08X\r\n",
			GIGE_EEPROM_OUT, (char*) xmlURL,   // Full URL
			(unsigned int) xmlStartAddress, // eg., 61700000;
			(unsigned int) xmlSize, // eg., 1F11D;
			(unsigned int) xmlSize, // eg., 1F11D;
			(unsigned int) GIGE_EEPROM_XML_URL1_REG);

	//---------------------------------------------------------------
	//
	// Reads the second XML URL to the eeprom on the I2C bus via GigE IP Core
	DEBUG_PRINT("%s XML Second URL: %s at 0x%X in %d /0x%X in byte at 0x%08X\r\n",
			GIGE_EEPROM_OUT, (char*) xmlURLSecond,   // Full URL
			(unsigned int) xmlStartAddressSecond, // eg., 61700000;
			(unsigned int) xmlSizeSecond, // eg., 1F11D;
			(unsigned int) xmlSizeSecond, // eg., 1F11D;
			(unsigned int) GIGE_EEPROM_XML_URL2_REG);


#if 0 // For debug
//---------------------------------------------------------------
	//
	// Reads the GigE Core register on the EPC bus
	// FIXED:
	for( i=GEV_GCSR_ADDR; i <= GEV_VER_DATE_ADDR; i+=4)
	{
		regAddress32 = IN32(i);
		DEBUG_PRINT("%s %d at 0x%08X\r\n",GIGE_MAC_REG_OUT,regAddress32,(unsigned int)i);
	}
#endif

	DEBUG_PRINT("==================== Build Time: %s ========================\r\n",	__TIME__);

	return;
}
#endif  //#if defined (IF_GIGE)


//**********************************************************************************
// Get the error code of GenICam GenTL from GigE Vision error code
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		error				：error番号
//	[ OUTPUT ]
//		error				：error番号
//==================================================================================
int toG(int error)
{
	int errorOut = 0,errorStatus = 0,erroDevice = 0;
	errorStatus = MAKE_ERRNUM_STATUS(error);
	erroDevice = MAKE_DEVICE_STATUS(error);
	switch (errorStatus)
	{
		case AVAL_STATUS_SUCCESS:			errorOut = GEV_STATUS_SUCCESS;				break;
		case AVAL_STATUS_NOT_IMPLEMENTED:	errorOut = GEV_STATUS_NOT_IMPLEMENTED;		break;
		case AVAL_STATUS_NOT_INITIALIZED:	errorOut = GEV_STATUS_CAMERA_NOT_INIT;		break;
		case AVAL_STATUS_RESOURCE_IN_USE:	errorOut = GEV_STATUS_BUSY;					break;
		case AVAL_STATUS_ACCESS_DENIED:		errorOut = GEV_STATUS_ACCESS_DENIED;		break;
		case AVAL_STATUS_INVALID_ID:		errorOut = GEV_STATUS_MSG_MISMATCH;			break;
		case AVAL_STATUS_NO_DATA:			errorOut = GEV_STATUS_DATA_OVERRUN;			break;
		case AVAL_STATUS_INVALID_PARAMETER:	errorOut = GEV_STATUS_INVALID_PARAMETER;	break;
		case AVAL_STATUS_IO:				errorOut = GEV_STATUS_INVALID_PROTOCOL;		break;
		case AVAL_STATUS_TIMEOUT:			errorOut = GEV_STATUS_TIMEOUT;				break;
		case AVAL_STATUS_ERROR:				errorOut = GEV_STATUS_LOCAL_PROBLEM;		break;
		case AVAL_STATUS_ABORT:				errorOut = GEV_STATUS_LOCAL_PROBLEM;		break;
		case AVAL_STATUS_INVALID_BUFFER:	errorOut = GEV_STATUS_BAD_ALIGNMENT;		break;
		case AVAL_STATUS_INVALID_ADDRESS:	errorOut = GEV_STATUS_INVALID_ADDRESS;		break;
		case AVAL_STATUS_NOT_AVAILABLE:		errorOut = GEV_STATUS_BUSY;					break;
		// Ver.1.2.0.0
		case AVAL_STATUS_BUFFER_TOO_SMALL:	errorOut = GEV_STATUS_DATA_OVERRUN;			break;
		case AVAL_STATUS_INVALID_INDEX:		errorOut = GEV_STATUS_INVALID_HEADER;		break;
		case AVAL_STATUS_PARSING_CHUNK_DATA:errorOut = GEV_STATUS_DATA_OVERRUN;			break;
		case AVAL_STATUS_INVALID_VALUE:		errorOut = GEV_STATUS_SOCKET_ERROR;			break;
		case AVAL_STATUS_RESOURCE_EXHAUSTED:errorOut = GEV_STATUS_DATA_OVERRUN;			break;
		case AVAL_STATUS_OUT_OF_MEMORY:		errorOut = GEV_STATUS_BUSY;					break;
		case AVAL_STATUS_INVALID_ARGUMENT:	errorOut = GEV_STATUS_MSG_MISMATCH;			break;
		// Following status are custom:
		case AVAL_STATUS_CONNECT_LOST:		errorOut = GEV_STATUS_MSG_MISMATCH;			break;
		case AVAL_STATUS_SHORTAGE_RESOURCE:	errorOut = GEV_STATUS_DATA_OVERRUN;			break;
		case AVAL_STATUS_OUT_OF_RANGE:		errorOut = GEV_STATUS_DATA_OVERRUN;			break;
		case AVAL_STATUS_INVALID_XFER:		errorOut = GEV_STATUS_MSG_MISMATCH;			break;
		case AVAL_STATUS_UNABLE_READ:		errorOut = GEV_STATUS_INVALID_ADDRESS;		break;
		case AVAL_STATUS_UNABLE_WRITE:		errorOut = GEV_STATUS_WRITE_PROTECT;		break;
		case AVAL_STATUS_NOT_NOTIFIED:		errorOut = GEV_STATUS_NO_MSG;				break;
		case AVAL_STATUS_COMPARE:			errorOut = GEV_STATUS_MSG_MISMATCH;			break;
		case AVAL_STATUS_INVALID_FILE:		errorOut = GEV_STATUS_INVALID_PARAMETER;	break;
		case AVAL_STATUS_ADJUST_IMPOSSIBLE:	errorOut = GEV_STATUS_MSG_MISMATCH;			break;
		default:							errorOut = GEV_STATUS_BAD_ALIGNMENT;		break;
	}

#if DEBUG_MODE_MSG
	if(errorOut != GEV_STATUS_SUCCESS)
	{
		DEBUG_PRINT("%s The module %d has been failed with 0x%08X\r\n", GIGE_TAG_OUT,erroDevice, errorOut);
	}
#endif // DEBUG_MODE_MSG

	return errorOut;
}

	
//**********************************************************************************
// FileSelector DPC
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int FileSelectorDPC (void)
{
	int status = GEV_STATUS_SUCCESS;
	unsigned int adrs, size;
	void *valueBuffer;

	switch (fileSel[fileSelector])
	{
		case FileOperationeSelector_Open:
			// Zero-fill to the empty buffer
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE); // temporary DDR memories
			fileOffset[fileSelector][fileSel[fileSelector]] = 0; // Address
			fileLength[fileSelector][fileSel[fileSelector]] = 0; // 0 bytes
			fileStatus[fileSelector][fileSel[fileSelector]] = 0; // Success or Failure
			fileResult[fileSelector][fileSel[fileSelector]] = 0; // if Success, N bytes is written
			fileSize[fileSelector] = 0; // 0 bytes
			break;

		case FileOperationeSelector_Close:
			// Clear the temporary buffer
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE);
			break;

		case FileOperationeSelector_Read:
			adrs = fileOffset[fileSelector][fileSel[fileSelector]]; // Gets the size of written on DDR memories
			size = fileLength[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
			valueBuffer = fileBuffer[fileSelector]; //Gets the start pointer of temporary buffer

			//if ((status = executeCommand (FIRM_CMD_DPC_DOWNLOAD, CPU_CMD_SYNC_OFF, (u32*) &cmd, sizeof(CPU_CMD) / 4)) != 0)
				//break;

			// Cmd Initialze
			if ((status = cmdExecuteInit ()) != AVAL_STATUS_SUCCESS)
				break;

			// Download
			if ((status = dpcGetMemoryNormal (valueBuffer, adrs, size)) != AVAL_STATUS_SUCCESS)
				break;

			// Stores the total written size of bit-stream image int the flash device
			fileResult[fileSelector][fileSel[fileSelector]] = size; // in bytes
			break;

		case FileOperationeSelector_Write:
			adrs = fileOffset[fileSelector][fileSel[fileSelector]]; // Gets the size of written on DDR memories
			size = fileLength[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
			valueBuffer = fileBuffer[fileSelector]; //Gets the start pointer of temporary buffer

			//if ((status = executeCommand (FIRM_CMD_DPC_UPLOAD, CPU_CMD_SYNC_OFF, (u32*) &cmd, sizeof(CPU_CMD) / 4)) != 0)
				//break;
			
			// Cmd Initialze
			if ((status = cmdExecuteInit ()) != AVAL_STATUS_SUCCESS)
				break;

			// Upload
			if ((status = gigeCmdDpcUpload (valueBuffer, adrs, size)) != AVAL_STATUS_SUCCESS)
				break;

			// Stores the total written size of bit-stream image int the flash device
			fileResult[fileSelector][fileSel[fileSelector]] = size; // in bytes
			break;

		case FileOperationeSelector_Delete:
			// Clear the temporary buffer
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE);
			break;

		default:
			break;
	}

	return (status);
}


//**********************************************************************************
// FileSelector FFC
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int FileSelectorFFC (void)
{
	int status = GEV_STATUS_SUCCESS;
	unsigned int adrs, size;
	void *valueBuffer;

	switch (fileSel[fileSelector])
	{
		case FileOperationeSelector_Open:
			// Zero-fill to the empty buffer
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE); // temporary DDR memories
			fileOffset[fileSelector][fileSel[fileSelector]] = 0; // Address
			fileLength[fileSelector][fileSel[fileSelector]] = 0; // 0 bytes
			fileStatus[fileSelector][fileSel[fileSelector]] = 0; // Success or Failure
			fileResult[fileSelector][fileSel[fileSelector]] = 0; // if Success, N bytes is written
			fileSize[fileSelector] = 0; // 0 bytes
			break;

		case FileOperationeSelector_Close:
			// Clear the temporary buffer
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE);

		case FileOperationeSelector_Read:
			adrs = fileOffset[fileSelector][fileSel[fileSelector]]; // Gets the size of written on DDR memories
			size = fileLength[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
			valueBuffer = fileBuffer[fileSelector]; //Gets the start pointer of temporary buffer

			//if ((status = executeCommand (FIRM_CMD_FFC_DOWNLOAD, CPU_CMD_SYNC_OFF, (u32*) &cmd, sizeof(CPU_CMD) / 4)) != 0)
				//break;
			
			// Cmd Initialze
			if ((status = cmdExecuteInit ()) != AVAL_STATUS_SUCCESS)
				break;

			// FFC Download
			status = toG (ffcGetMemoryNormal (valueBuffer, adrs, size, FlatFieldCorrectionModeSelector));
			
			// Stores the total written size of bit-stream image int the flash device
			fileResult[fileSelector][fileSel[fileSelector]] = size; // in bytes
			break;

		case FileOperationeSelector_Write:
			adrs = fileOffset[fileSelector][fileSel[fileSelector]]; // Gets the size of written on DDR memories
			size = fileLength[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
			valueBuffer = fileBuffer[fileSelector]; //Gets the start pointer of temporary buffer

			//if ((status = executeCommand (FIRM_CMD_FFC_UPLOAD, CPU_CMD_SYNC_OFF, (u32*) &cmd, sizeof(CPU_CMD) / 4)) != 0)
				//break;

			// Cmd Initialze
			if ((status = cmdExecuteInit ()) != AVAL_STATUS_SUCCESS)
				break;

			// Upload
			status = toG (ffcSetMemoryNormal (valueBuffer, adrs, size, FlatFieldCorrectionModeSelector));

			// Stores the total written size of bit-stream image int the flash device
			fileResult[fileSelector][fileSel[fileSelector]] = size; // in bytes
			break;

		case FileOperationeSelector_Delete:
			// Clear the temporary buffer
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE);
			break;

		default:
			break;
	}

	return (status);
}


//**********************************************************************************
// FileSelector Spectrum Wave Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if defined (MODE_SPECTRUM)
int FileSelectorSpectrumWave (void)
{
	int status = GEV_STATUS_SUCCESS;
	unsigned size;
	void *valueBuffer;
	unsigned int address2nd, address3rd;
	int i;
	int isDone;

	switch (fileSel[fileSelector])
	{
		case FileOperationeSelector_Open:
			// Zero-fill to the empty buffer
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE); // temporary DDR memories
			fileOffset[fileSelector][fileSel[fileSelector]] = 0; // Address
			fileLength[fileSelector][fileSel[fileSelector]] = 0; // 0 bytes
			fileStatus[fileSelector][fileSel[fileSelector]] = 0; // Success or Failure
			fileResult[fileSelector][fileSel[fileSelector]] = 0; // if Success, N bytes is written
			fileSize[fileSelector] = 0; // 0 bytes
			break;

		case FileOperationeSelector_Close:
			// Clear the temporary buffer
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE);
			break;

		case FileOperationeSelector_Read:
			address2nd = fileOffset[fileSelector][fileSel[fileSelector]]; // Gets the size of written on DDR memories
			address3rd = fileLength[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
			valueBuffer = (u32*) fileBuffer[fileSelector]; //Gets the start pointer of temporary buffer

			// Binary => ASCII変換
			if ((status = spectrumDownloadBintoAscii ((void *)valueBuffer, (void *)gpSpectrumBandInfo, (unsigned int *)&size)) != AVAL_STATUS_SUCCESS)
				break;

			// Stores the total written size of bit-stream image int the flash device
			fileResult[fileSelector][fileSel[fileSelector]] = address3rd; // in bytes

			// Read Size
			fileLength[fileSelector][fileSel[fileSelector]] = size;

			break;

		case FileOperationeSelector_Write:
			//adrs = fileOffset[fileSelector][fileSel[fileSelector]];
			size = fileLength[fileSelector][fileSel[fileSelector]];
			valueBuffer = fileBuffer[fileSelector];

			//if ((status = executeCommand (FIRM_CMD_SPECTRUM_WAVE, CPU_CMD_SYNC_OFF, (u32*) &cmd, sizeof(CPU_CMD) / 4)) != 0)
				//goto _NEXT;
		
			// Cmd Initialze
			if ((status = cmdExecuteInit ()) != AVAL_STATUS_SUCCESS)
				break;

			// Upload
			if ((status = gigeCmdSpectrumWaveUpload (valueBuffer, size)) != AVAL_STATUS_SUCCESS)
				break;

#if 0
			if (gInterFaceID != INTERFACE_CXP)
			{
				for (i=0; i<GE_UPDATE_SPECTRUM_WAVE_TIMEOUT; i++)
				{
					// コマンドステータス取得
					//if ((status = isExecuteCommandDone(CPU_CMD_TIMEOUT*4, FIRM_CMD_SPECTRUM_WAVE, &isDone)) != 0)
					if ((status = toG (cmdExecuteStatus ((int *)&isDone))) != AVAL_STATUS_SUCCESS)
					{
						DEBUG_PRINT_FORCE ("Spectrum Wave Update Status Error = %d\r\n", status);
						goto _DONE;
					}

					// コマンド終了?
					if (isDone == command_done)
						break;

					usDelay (1000);
				}

				if (i >= GE_UPDATE_SPECTRUM_WAVE_TIMEOUT)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_TIMEOUT);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "Spectrum Wave Update Timeout Error\n");
					goto _DONE;
				}
			}

_DONE:
#endif
			// キャッシュFlash
			cacheFlush ();

			fileStatus[fileSelector][fileSel[fileSelector]] = status; // Success or Failure

			// Stores the total written size of bit-stream image int the flash device
			fileResult[fileSelector][fileSel[fileSelector]] = size; // in bytes
			break;

		case FileOperationeSelector_Delete:
			break;

		default:
			break;
	}

	return (status);
}
#endif // #if defined (MODE_SPECTRUM)


//**********************************************************************************
// FileSelector FPGA
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int FileSelectorFpga (void)
{
	int status = GEV_STATUS_SUCCESS;
	unsigned int adrs, size;
	unsigned int *valueBuffer;
	int isDone;
	int i;

	switch (fileSel[fileSelector])
	{
		case FileOperationeSelector_Open:
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE); // temporary DDR memories
			fileOffset[fileSelector][fileSel[fileSelector]] = 0; // Address
			fileLength[fileSelector][fileSel[fileSelector]] = 0; // 0 bytes
			fileStatus[fileSelector][fileSel[fileSelector]] = 0; // Success or Failure
			fileResult[fileSelector][fileSel[fileSelector]] = 0; // if Success, N bytes is written
			fileSize[fileSelector] = 0; // 0 bytes
			break;

		case FileOperationeSelector_Close:
			// Clear the temporary buffer of boot-rom image
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE);
			break;

		case FileOperationeSelector_Read:
			#if 0
			address2nd = fileOffset[fileSelector][fileSel[fileSelector]]; // Clip the high virtual address
			valueBuffer = (u32*)fileBuffer[fileSelector];//Gets the start pointer of temporary buffer
			valueBuffer += address2nd;// Seek to specified address
			value2nd = SWAP_L((u32)(*valueBuffer));// Convert to little-endian
			value = value2nd;//  Reads the value to the specified buffer address
			#endif
			break;

		case FileOperationeSelector_Write:
			adrs = fileOffset[fileSelector][fileSel[fileSelector]]; // Gets the size of written on DDR memories
			size = fileLength[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
			valueBuffer = fileBuffer[fileSelector]; //Gets the start pointer of temporary buffer

			// Cmd Initialze
			if ((status = cmdExecuteInit ()) != AVAL_STATUS_SUCCESS)
				break;

			// Flash Write
			if ((status = gigeCmdFirmUpload (adrs, (unsigned char *)valueBuffer, size)) != AVAL_STATUS_SUCCESS)
				break;

			// Success or Failure
			fileStatus[fileSelector][fileSel[fileSelector]] = status;

			// Stores the total written size of bit-stream image int the flash device
			fileResult[fileSelector][fileSel[fileSelector]] = size; // in bytes
			break;

		case FileOperationeSelector_Delete:
			adrs = fileOffset[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
			size = fileLength[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
			for (i =  adrs; i < (adrs  + size); i += QSPI_FLASH_SEC_SIZE)
			{
				// Erase
				status = qspiFlashSectorErase((unsigned int) i);
				fileResult[fileSelector][fileSel[fileSelector]] = i	+ QSPI_FLASH_SEC_SIZE;
				fileStatus[fileSelector][fileSel[fileSelector]] = status; // Success or Failure
			}

			// Clear the temporary buffer of boot-rom image
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE);
			break;

		default:
			// Unknown
			status = GEV_STATUS_INVALID_PARAMETER;
			break;
	}

	return (status);
}


//**********************************************************************************
// FileSelector XML
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int FileSelectorXML (void)
{
	int status = GEV_STATUS_SUCCESS;
	unsigned int adrs, size;
	unsigned int *valueBuffer;
	int isDone;
	int i;

	switch (fileSel[fileSelector])
	{
		case FileOperationeSelector_Open:
			memset((void*) fileBuffer[fileSelector], 0, XMLFILE_SIZE); // temporary DDR memories
			fileOffset[fileSelector][fileSel[fileSelector]] = 0; // Address
			fileLength[fileSelector][fileSel[fileSelector]] = 0; // 0 bytes
			fileStatus[fileSelector][fileSel[fileSelector]] = 0; // Success or Failure
			fileResult[fileSelector][fileSel[fileSelector]] = 0; // if Success, N bytes is written
			fileSize[fileSelector] = 0; // 0 bytes

			memset((void*) FIRM_UPDATE_XML_FILE_NAME_ADRS, 0, FIRM_UPDATE_XML_FILE_NAME_SIZE); // temporary DDR memories
			break;

		case FileOperationeSelector_Close:
			memset((void*) fileBuffer[fileSelector], 0, XMLFILE_SIZE);
			break;

		case FileOperationeSelector_Read:
			#if 0
			address2nd = fileOffset[fileSelector][fileSel[fileSelector]]; // Clip the high virtual address
			valueBuffer = (u32*)fileBuffer[fileSelector];//Gets the start pointer of temporary buffer
			valueBuffer += address2nd;// Seek to specified address
			value2nd = SWAP_L((u32)(*valueBuffer));// Convert to little-endian
			value = value2nd;//  Reads the value to the specified buffer address
			#endif
			break;

		case FileOperationeSelector_Write:
			adrs = fileOffset[fileSelector][fileSel[fileSelector]]; // Gets the size of written on DDR memories
			size = fileLength[fileSelector][fileSel[fileSelector]]; // Gets the Length in byte by starting point
			valueBuffer = fileBuffer[fileSelector]; //Gets the start pointer of temporary buffer

			// Cmd Initialze
			if ((status = cmdExecuteInit ()) != AVAL_STATUS_SUCCESS)
				break;

			// Flash Write
			if ((status = gigeCmdXmlUpload (adrs, (unsigned char *)valueBuffer, size)) != AVAL_STATUS_SUCCESS)
				break;

			// Success or Failure
			fileStatus[fileSelector][fileSel[fileSelector]] = status;

			// Stores the total written size of bit-stream image int the flash device
			fileResult[fileSelector][fileSel[fileSelector]] = size; // in bytes
			break;

		case FileOperationeSelector_Delete:
			adrs =fileOffset[fileSelector][fileSel[fileSelector]]; // Gets the address of offset by starting point
			size =fileLength[fileSelector][fileSel[fileSelector]]; // Gets the Length in byte by starting point
			for (i = adrs; i < (adrs + size); i +=QSPI_FLASH_SEC_SIZE)
			{
				// Erase
				status = qspiFlashSectorErase ((unsigned int)i);
				if (status == 0)
				{
					fileResult[fileSelector][fileSel[fileSelector]] = i + QSPI_FLASH_SEC_SIZE;
				}
				else
				{
					fileStatus[fileSelector][fileSel[fileSelector]] = status; // Success or Failure
				}
			}

			// Clear the temporary buffer of boot-rom image
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE);
			break;

		default:
			// Unknown
			status = GEV_STATUS_INVALID_PARAMETER;
			break;
	}

	return (status);
}


//**********************************************************************************
// FileSelector Sensor FPGA Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if defined (MODE_FPGA_PF)
#if 0 //@@@1
int FileSelectorIfFpgaWrite (void)
{
	int status = GEV_STATUS_SUCCESS;
	unsigned int size;
	unsigned int *valueBuffer;
	int isDone;
	int i;

	switch (fileSel[fileSelector])
	{
		case FileOperationeSelector_Open:
			// Zero-fill to the empty buffer
			memset((void*) fileBuffer[fileSelector], 0, FLASH_UPDATE_POLAFIRE_SIZE); // temporary DDR memories
			fileOffset[fileSelector][fileSel[fileSelector]] = 0; // Address
			fileLength[fileSelector][fileSel[fileSelector]] = 0; // 0 bytes
			fileStatus[fileSelector][fileSel[fileSelector]] = 0; // Success or Failure
			fileResult[fileSelector][fileSel[fileSelector]] = 0; // if Success, N bytes is written
			fileSize[fileSelector] = 0; // 0 bytes
			gIfFpgaReConfig = 0;

			// SPI Init
			if ((status = PolarFireSpiMode (MODE_ENABLE)) != AVAL_STATUS_SUCCESS)
				break;

			break;

		case FileOperationeSelector_Close:
			// Clear the temporary buffer
			memset((void*) fileBuffer[fileSelector], 0, FLASH_UPDATE_POLAFIRE_SIZE);

			// Success or Failure
			if (fileStatus[fileSelector][fileSel[fileSelector]] == 0)
			{
				// Reconfig
				if (gIfFpgaFlashAdrs == FLASH_UPDATE_POLAFIRE_ADRS)
				{
					// ReConfig Flag設定
					gIfFpgaReConfig = 1;
				}
			}

			//SPIファイナライズ
			//PolarFireSpiMode (MODE_DISABLE);
			break;

		case FileOperationeSelector_Read:
			break;

		case FileOperationeSelector_Write:
			//adrs = fileOffset[fileSelector][fileSel[fileSelector]];
			size = fileLength[fileSelector][fileSel[fileSelector]];
			valueBuffer = fileBuffer[fileSelector];

			// Flashアドレス設定
			if (gDeviceIfUpdateSelector == 1)
				gIfFpgaFlashAdrs = FLASH_UPDATE_POLAFIRE_GDN_ADRS;		// 管理者領域
			else
				gIfFpgaFlashAdrs = FLASH_UPDATE_POLAFIRE_ADRS;			// ユーザー領域

			// Cmd Initialze
			if ((status = cmdExecuteInit ()) != AVAL_STATUS_SUCCESS)
				break;

			//if ((status = executeCommand(FIRM_CMD_IF_FPGA_UPLOAD, CPU_CMD_SYNC_OFF, (u32*)&cmd, sizeof(CPU_CMD) / 4)) != 0)
				//goto _DONE;

			// Flash Write
			if ((status = gigeCmdIfFpgaUpload (gIfFpgaFlashAdrs, (unsigned char *)valueBuffer, size)) != AVAL_STATUS_SUCCESS)
				break;

#if 0
			if (gInterFaceID != INTERFACE_CXP)
			{
				for (i=0; i<GE_UPDATE_IF_FPGA_TIMEOUT; i++)
				{
					// コマンドステータス取得
					//if ((status = isExecuteCommandDone(CPU_CMD_TIMEOUT*4, FIRM_CMD_IF_FPGA_UPLOAD, &isDone)) != 0)
					if ((status = cmdExecuteStatus ((int *)&isDone)) != AVAL_STATUS_SUCCESS)
						goto _DONE;
			
					// コマンド終了?
					if (isDone == command_done)
						break;

					usDelay (1000);
				}

				if (i >= GE_UPDATE_IF_FPGA_TIMEOUT)
				{
					status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_TIMEOUT);
					cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "IF FPGA Update Timeout Error\n");
					DEBUG_PRINT();
					goto _DONE;
				}
			}
#endif
_DONE:
			// Stores the total written size of bit-stream image int the flash device
			fileResult[fileSelector][fileSel[fileSelector]] = size; // in bytes

			// Success or Failure
			fileStatus[fileSelector][fileSel[fileSelector]] = status;
			break;

		case FileOperationeSelector_Delete:
			break;

		default:
			break;
	}

	return (status);
}
#endif //@@1
#endif // #if defined (MODE_FPGA_PF)


//**********************************************************************************
// FileSelector Sensor FPGA Write
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
#if defined (MODE_GIGE_10G)
int FileSelectorPhyWrite (void)
{
	int status = GEV_STATUS_SUCCESS;
	unsigned int size;
	unsigned char *valueBuffer;
	int isDone;
	int i;
	CPU_CMD cmd = { 0 };

	switch (fileSel[fileSelector])
	{
		case FileOperationeSelector_Open:
			// Zero-fill to the empty buffer
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE); // temporary DDR memories
			fileOffset[fileSelector][fileSel[fileSelector]] = 0; // Address
			fileLength[fileSelector][fileSel[fileSelector]] = 0; // 0 bytes
			fileStatus[fileSelector][fileSel[fileSelector]] = 0; // Success or Failure
			fileResult[fileSelector][fileSel[fileSelector]] = 0; // if Success, N bytes is written
			fileSize[fileSelector] = 0; // 0 bytes
			break;

		case FileOperationeSelector_Close:
			memset((void*) fileBuffer[fileSelector], 0, UPDATE_DATA_SIZE);
			break;

		case FileOperationeSelector_Read:
			break;

		case FileOperationeSelector_Write:
			//adrs = fileOffset[fileSelector][fileSel[fileSelector]];
			size = fileLength[fileSelector][fileSel[fileSelector]];
			valueBuffer = fileBuffer[fileSelector];

			//if ((status = executeCommand(FIRM_CMD_PHY_UPLOAD, CPU_CMD_SYNC_OFF, (u32*)&cmd, sizeof(CPU_CMD) / 4)) != 0)
				//goto _NEXT;

			// Cmd Initialze
			if ((status = cmdExecuteInit ()) != AVAL_STATUS_SUCCESS)
				break;

			// Write
			if ((status = gigeCmdPhyUpload (FLASH_PHY_DATA_ADRS, valueBuffer, size)) != AVAL_STATUS_SUCCESS)
				break;

#if 0
			for (i=0; i<GE_PHY_FPGA_TIMEOUT; i++)
			{
				// コマンドステータス取得
				//if ((status = isExecuteCommandDone(CPU_CMD_TIMEOUT*4, FIRM_CMD_PHY_UPLOAD, &isDone)) != 0)
				if ((status = cmdExecuteStatus ((int *)&isDone)) != AVAL_STATUS_SUCCESS)
					goto _DONE;

				// コマンド終了?
				if (isDone == command_done)
					break;

				usDelay (1000);
			}

			if (i >= GE_PHY_FPGA_TIMEOUT)
			{
				status = MAKE_ERROR_STATUS (AVAL_STATUS_CAMERA, AVAL_STATUS_TIMEOUT);
				cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "PHY Update Timeout Error = \n");
				goto _DONE;
			}
#endif
			// Stores the total written size of bit-stream image int the flash device
			fileResult[fileSelector][fileSel[fileSelector]] = size; // in bytes

			// Success or Failure
			fileStatus[fileSelector][fileSel[fileSelector]] = status;
			break;

		case FileOperationeSelector_Delete:
			break;

		default:
			break;
	}

_DONE:
	return (status);
}
#endif // #if defined (MODE_GIGE_10G)


#if defined (IF_GIGE)
#if defined (MODE_GIGE_10G)
// ---- Generic libgige user callback ------------------------------------------
//
//          This function is called by the libgige library when some of the
//          predefined events occurs.
//
// id     = identifier of the event
// param  = 32b parameter of the event
// data   = various extended data can be passed to/from the function
//
// return value = event specific
//
u32 gige_event(u32 id, u32 param, __attribute__((unused)) void *data)
{
    u32 ret = 0;

    switch (id)
    {
        // No event
        case LIB_EVENT_NONE:
            break;

        // Write access into the GVCP configuration register
        //      param = value written by a GEV application
        //      data  = unused
        case LIB_EVENT_GVCP_CONFIG_WRITE:
            if ((param & 4) || (gige_get_gev_version() == 2))   // ES bit set or GEV 2.x with extended IDs
                framebuf_control |= FRAMEBUF_C_EXTSTAT;         // Enable extended GVSP status codes
            else
                framebuf_control &= ~FRAMEBUF_C_EXTSTAT;        // Disable extended GVSP status codes
            break;

        // Open or close stream channel
        //      param = bit mask of open channels (channel 0 = bit 0, etc.)
        //      data  = unused
        case LIB_EVENT_STREAM_OPEN_CLOSE:
            if (param == 0)                                     // No more open stream channels
            {
                framebuf_control |= FRAMEBUF_C_INIT;            // Clear framebuffer
                video_gcsr        = 0x00000000;                 // Reset video in module
                gige_set_acquisition_status(0, 0);              // Acquisition off
            }
            break;

        // Write access into the stream channel configuration register
        //      param = value written by a GEV application
        //      data  = pointer to the stream channel index (u32)
        case LIB_EVENT_SCCFG_WRITE:
            break;

        // Application closed the control channel
        //      param = index of the (X)GigE core
        //      data  = unused
        case LIB_EVENT_APP_DISCONNECT:
            if (param == 0)                                     // No more open stream channels
            {
                video_gcsr        = 0x00000000;                 // Reset video in module
                gige_set_acquisition_status(0, 0);              // Acquisition off
            }
            break;

        // Physical link disconnected
        //      param = index of the (X)GigE core
        //      data  = unused
        case LIB_EVENT_LINK_DOWN:
            video_gcsr = 0x00000000;                            // Reset video in module
            gige_set_acquisition_status(0, 0);                  // Acquisition off
            break;

        // Write access to the physical link configuration register
        //      param = value written by a GEV application
        //      data  = unused
        case LIB_EVENT_LINK_CONFIG_WRITE:
            break;

        // A new trigger has been scheduled after reception of a valid scheduled action command.
        //      param = index of the action signal(0..127)
        //      data  =  pointer to a 64b unsigned integer with trigger time of the action
        case LIB_EVENT_SCHEDULED_ACTION:
            //{
            //    u64 *timestamp = (u64*)data;
            //    printf("Scheduled ACTION_CMD: signal %ld, timestamp %" PRIu64 "\r\n", param, *timestamp);
            //}
            break;

        // Undefined event
        default:
            break;
    }

    return ret;
}

#endif // #if defined (MODE_GIGE_10G)


#if defined (MODE_GIGE_10G)
//**********************************************************************************
// GigE Update Buffer
//----------------------------------------------------------------------------------
//	[ INPUT ]
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeUpdateBuffer (void)
{
	int status = AVAL_STATUS_SUCCESS;
    u32 chunk_size;
    u32 chunk_layout_id = 0;    // Chunk layout id
	static u32 old_chunk_layout_id = 0;
	int update_leader = 1;
	int update_trailer = 1;
	
	// Offsetx
	if ((status = aoiGetWidthOffset((int*)&video_offs_x)) != AVAL_STATUS_SUCCESS)
		video_offs_x = 0;

	// Offsety
    if ((status = aoiGetHeightOffset((int*)&video_offs_y)) != AVAL_STATUS_SUCCESS)
		video_offs_y = 0;

	// Width
    if ((status = aoiGetWidth((int*)&video_width)) != AVAL_STATUS_SUCCESS)
		video_width = WidthMax ();
	
	// Height
    if ((status = aoiGetHeight((int*)&video_height)) != AVAL_STATUS_SUCCESS)
		video_height = HeightMax ();
	
	// PixelFormat
	if ((status = cameraGetBitWidthGigE ((int *)&video_pixfmt)) != AVAL_STATUS_SUCCESS)
		video_pixfmt = GVSP_PIX_MONO12;
	
	if ((video_chunk_ctrl & 0x80000000) == 0)       // Check if extended chunk mode is activated
    {
        chunk_size      = 0;
        chunk_layout_id = 0;
    }
    else
    {
        if (video_chunk_enable == 0)                // Check if framecounter chunk is activated
        {
            chunk_size      = 8;                    // Additional bytes to describe image chunk only (4 byte id, 4 byte size)
            chunk_layout_id = video_chunkid_img |
                              video_pixfmt      |
                              video_width       |
                              video_height;         // Set chunk layout id
        }
        else
        {
            chunk_size      = 20;                   // Additional bytes to describe image chunk (8 bytes) and frame counter chunk (4 bytes data, 4 bytes id, 4 bytes size)
            chunk_layout_id = video_chunkid_img |
                              video_chunkid_fc  |
                              video_pixfmt      |
                              video_width       |
                              video_height;         // Set chunk layout id
        }

        // Verify that chunk_layout_id gets changed when chunk layout changes
        if (update_trailer == 1)
        {
            if (chunk_layout_id == old_chunk_layout_id)
                chunk_layout_id++;
            old_chunk_layout_id = chunk_layout_id;
        }
    }

    // Adjust padding and total bytes per block
    framebuf_padding(video_pixfmt, video_width, video_height, chunk_size);
    gige_set_scmbs(0, framebuf_bpb);

    // Set Payload Type
    if ((video_chunk_ctrl & 0x80000000) == 0)
        framebuf_set_pld_type(PLD_IMAGE);
    else
        framebuf_set_pld_type(PLD_IMAGE | PLD_EXTCHUNK_MODE);

    // Update leader/trailer
    if (update_leader)
	{
        framebuf_img_leader(video_pixfmt, video_width, video_height, video_offs_x, video_offs_y);
	}

    if (update_trailer)
	{
        framebuf_img_trailer(video_height, chunk_layout_id);
	}
	
	return (status);
}

#endif // #if defined (MODE_GIGE_10G)
#endif // #if defined (IF_GIGE)

#if defined (MODE_GIGE_10G)
//**********************************************************************************
//	GigE Rate設定
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				：port番号
//		configData			：ConnectionConfigデータ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int gigeSetConnectionConfig (void)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int data3;
	double dbFrameRate, dbFrameRateMax;
	unsigned int uiExp;
	int dataI1;
	unsigned int intFrameRate;
#if defined(MODE_AUTO_EXPOSURE)
	int expMaxTime;
#endif

	// 取り込み停止
	acquisitionAbort ();

	//--------------------------------------------------------------------------------
	// 現在のフレームレート/露光時間取得
	//--------------------------------------------------------------------------------
	if ((status = acquisitionGetFrameRate (&dbFrameRate))!= AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = acquisitionGetExposure (&uiExp))!= AVAL_STATUS_SUCCESS)
		goto _DONE;

	//--------------------------------------------------------------------------------
	// Set H Interval
	//--------------------------------------------------------------------------------

	// センサ有効?
	//@@@1if ((status = sensorGetValidMode ((int *)&data3)) != AVAL_STATUS_SUCCESS)
		//@@@1goto _DONE;

	if (data3 == MODE_ENABLE)
	{
		// Get Bit
		if ((status = aoiGetBitWidth (&dataI1)) != AVAL_STATUS_SUCCESS)
			goto _DONE;
	
		// Set H Interval
		if ((status = sensorSetHInterval (dataI1))!= AVAL_STATUS_SUCCESS)
		goto _DONE;
	}

#if defined(MODE_AUTO_EXPOSURE)
	//--------------------------------------------------------------------------------
	// Exposure Max取得
	//--------------------------------------------------------------------------------
	if ((status = autoBrightGetExposureMax (&expMaxTime)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
#endif

	//--------------------------------------------------------------------------------
	// 現在のフレームレート/露光時間設定
	//--------------------------------------------------------------------------------
	// 最大レート取得
	if ((status = rateMax (&dbFrameRateMax)) != AVAL_STATUS_SUCCESS)
		goto _DONE;
	
	//if (dbFrameRate >= (dbFrameRateMax - 0.5))
	if (dbFrameRate >= dbFrameRateMax)
	{
		intFrameRate = (int)dbFrameRateMax;
		dbFrameRate = (double)intFrameRate;
	}

	if ((status = acquisitionSetFrameRate (dbFrameRate)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

	if ((status = acquisitionSetExposure (uiExp)) != AVAL_STATUS_SUCCESS)
		goto _DONE;

#if defined(MODE_AUTO_EXPOSURE)
	//--------------------------------------------------------------------------------
	// Exposure Max再設定
	//--------------------------------------------------------------------------------
	if (autoBrightCheckExposureMinMax (expMaxTime) == AVAL_STATUS_SUCCESS)
	{
		autoBrightSetExposureMax (expMaxTime);
	}
#endif

_DONE:
	return (status);
}
#endif // #if defined (MODE_GIGE_10G)


