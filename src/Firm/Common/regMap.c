//**********************************************************************************
//
//                           Camera Header
//
//      Copyright (c) 2016 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// regMap.c - Register Mapping Display
//**********************************************************************************


//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"
#if defined (IF_GIGE)
#include "../GigE/framebuf.h"
#include "../GigE/gev.h"
#endif

//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern unsigned int *gpCameraCommonParameter;	// 共通領域
extern unsigned int *gpCameraUserParameter;		// UserSet領域


//**********************************************************************************
// FPGA Register Mapping
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int fpgaRegMap (void)
{
	int status = AVAL_STATUS_SUCCESS;

	DEBUG_PRINT_FORCE("\n");
	DEBUG_PRINT_FORCE("[Register]\n");
	DEBUG_PRINT_FORCE("  CoaxPress IP            : 0x%08x\n", CORECXP_BASE_ADDR);
	DEBUG_PRINT_FORCE("  CoaxPress FPGA          : 0x%08x\n", FPGA_CXP_BASE_ADDR);

	
#if 0//@@@1	
	
	DEBUG_PRINT_FORCE("  Version                 : 0x%08x\n", FPGA_VER_ADRS);

#if defined (MODE_SYSTEM_MANAGEMENT)
	DEBUG_PRINT_FORCE("  System Management       : 0x%08x\n", FPGA_SYSTEM_MANAGEMENT_ADRS);
#endif
	DEBUG_PRINT_FORCE("  GPIO                    : 0x%08x\n", FPGA_GPIO_ADRS);
	DEBUG_PRINT_FORCE("  LED                     : 0x%08x\n", FPGA_LED_ADRS);

	DEBUG_PRINT_FORCE("  Peltier                 : 0x%08x\n", FPGA_PELTIER_ADRS);
	DEBUG_PRINT_FORCE("  FFC                     : 0x%08x\n", FPGA_FFC_ADRS);
	DEBUG_PRINT_FORCE("  DPC                     : 0x%08x\n", FPGA_DPC_ADRS);
	DEBUG_PRINT_FORCE("  Digital Offset Gain     : 0x%08x\n", FPGA_DOG_ADRS);

	DEBUG_PRINT_FORCE("  AOI                     : 0x%08x\n", FPGA_AOI_ADRS);
	DEBUG_PRINT_FORCE("  DMA                     : 0x%08x\n", FPGA_DMA_ADRS);
	DEBUG_PRINT_FORCE("  LUT                     : 0x%08x\n", FPGA_LUT_ADRS);

#if defined (MODE_SENSOR_DRRS)
	DEBUG_PRINT_FORCE("  DRRS                    : 0x%08x\n", FPGA_DRRS_ADRS);
#endif

#if defined (MODE_SPECTRUM)
	DEBUG_PRINT_FORCE("  Spectrum                : 0x%08x\n", FPGA_SPECTRUM_ADRS);
#endif

	DEBUG_PRINT_FORCE("\n");
	DEBUG_PRINT_FORCE("  Device Ctrl             : 0x%08x\n", GENICAM_DEVICE_CONTROL_ADRS);
	DEBUG_PRINT_FORCE("  Acquisition Ctrl        : 0x%08x\n", GENICAM_ACQUISITION_CONTROL_ADRS);
	DEBUG_PRINT_FORCE("  Digital IO              : 0x%08x\n", GENICAM_DIGITAL_CONTROL_ADRS);
	DEBUG_PRINT_FORCE("  Counter                 : 0x%08x\n", GENICAM_COUNTER_CONTROL_ADRS);
	DEBUG_PRINT_FORCE("  Timer                   : 0x%08x\n", GENICAM_TIMER_CONTROL_ADRS);
#ifdef MODE_ENCODER
	DEBUG_PRINT_FORCE("  Encoder                 : 0x%08x\n", GENICAM_ENCODER_CONTROL_ADRS);
#endif

	DEBUG_PRINT_FORCE("\n");
	DEBUG_PRINT_FORCE("[DDR]\n");
	DEBUG_PRINT_FORCE("  UserSet Common          : 0x%08x\n", gpCameraCommonParameter);
	DEBUG_PRINT_FORCE("  UserSet Param           : 0x%08x\n", gpCameraUserParameter);
	DEBUG_PRINT_FORCE("  UserSet Spectrum        : 0x%08x\n", gpCameraUserParameter);

	DEBUG_PRINT_FORCE("  FFC                     : 0x%08x\n", FFC_MEMORY_ADRS);
	DEBUG_PRINT_FORCE("  DPC                     : 0x%08x\n", DPC_MEMORY_ADRS);

#if defined (MODE_CXP)
	if (gInterFaceID == INTERFACE_CXP)
	{
		DEBUG_PRINT_FORCE("  CXP Send All Buffer     : 0x%08x\n", FIRM_CXP_SEND_DATA_ALL_ADRS);
		DEBUG_PRINT_FORCE("  CXP Send Cmd Buffer     : 0x%08x\n", FIRM_CXP_SEND_DATA_CMD_ADRS);
		DEBUG_PRINT_FORCE("  CXP Recv All Buffer     : 0x%08x\n", FIRM_CXP_RECV_DATA_ALL_ADRS);
		DEBUG_PRINT_FORCE("  CXP Recv Cmd Buffer     : 0x%08x\n", FIRM_CXP_RECV_DATA_CMD_ADRS);
	}
#endif // #if defined (MODE_CXP)

#if defined (IF_GIGE)
	DEBUG_PRINT_FORCE("  FPGA DMA                : 0x%08x\n", FIRM_FPGA_DMA_ADRS);

	if ((gInterFaceID == INTERFACE_GIGE) || (gInterFaceID == INTERFACE_GIGE20))
	{
		DEBUG_PRINT_FORCE("  GigE Frame BUffer       : 0x%08x\n", IN32 (FIRM_DATA_GIGE_FRAME_BUFF_ADRS));
		#if defined (MODE_GIGE_10G)
		DEBUG_PRINT_FORCE("  GigE Frame BUffer IP    : 0x%08x\n", XPAR_M0_AXI_FB_BASEADDR);
		DEBUG_PRINT_FORCE("  GigE Vison IP           : 0x%08x\n", XPAR_M1_AXI_GEV_BASEADDR);
		#endif
	}
#endif
	
#if defined (MODE_SENSOR_DRRS)
	DEBUG_PRINT_FORCE("  DRRS Frame Buffer       : 0x%08x\n", DRRS_IMG_MEMORY_ADRS);
#endif

#endif //@@@1
	DEBUG_PRINT_FORCE("\n");

	return (status);
}

// eof
