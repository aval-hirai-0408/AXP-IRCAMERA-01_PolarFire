//**********************************************************************************
//
//                         Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// bootStrap.h - Boot Strap Header
//**********************************************************************************

#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

enum bootStrap
{
	Standard = 0x00000000,
	Revision = 0x00000004,
	XmlManifestSize = 0x00000008,
	XmlManifestSelector = 0x0000000C,
	XmlVersion = 0x00000010,
	XmlSchemaVersion = 0x00000014,
	XmlUrlAddress = 0x00000018,
	Iidc2Address = 0x0000001C,
	DeviceVendorName = 0x00002000,
	DeviceModelName = 0x00002020,
	DeviceManufacturerInfo = 0x00002040,
	DeviceVersionInfo = 0x00002070,
	DeviceSerialNumber = 0x000020B0,
	DeviceUserIdentification = 0x000020C0,
	WidthAddress = 0x00003000,
	HeightAddress = 0x00003004,
	AcquisitionModeAddress = 0x00003008,
	AcquistionStartAddress = 0x0000300C,
	AcquistionStopAddress = 0x00003010,
	PixelFormatAddress = 0x00003014,
	DeviceTapGeometryAddress = 0x00003018,
	Image1StreamIDAddress = 0x0000301C,
	Image2StreamIDAddress = 0x00003020,
	//ImagenStreamIDAddress = 0x00003020,
	ConnectionReset = 0x00004000,
	DeviceConnectionID = 0x00004004,
	MasterHostConnectionID = 0x00004008,
	ControlPacketSizeMax = 0x0000400C,
	StreamPacketSizeMax = 0x00004010,
	ConnectionConfig = 0x00004014,
	ConnectionConfigDefault = 0x00004018,
	TestMode = 0x0000401C,
	TestErrorCountSelector = 0x00004020,
	TestErrorCount = 0x00004024,
	TestPacketCountTx = 0x00004028,
	TestPacketCountRx = 0x00004030,
	ElectricalComplianceTest = 0x00004038,
#if defined (MODE_CXP_VERSION_20)
	CapabilityRegister = 0x0000403C,
#else
	HSupconnection = 0x0000403C,
#endif
	FeatureControlRegister = 0x00004040,
	VersionsSupported = 0x00004044,
	VersionUsed = 0x00004048,
	LinkSharingStatus = 0x0000404C,
	LinkSharingHorizontalStripeCount = 0x00004050,
	LinkSharingVerticalStripeCount = 0x00004054,
	LinkSharingHorizontalOverlap = 0x00004058,
	LinkSharingVerticalOverlap = 0x0000405C,
	LinkSharingDuplicateStripe = 0x00004060,
	Start_of_manufacturer_specific_register_space = 0x00006000,

	//manf specific boot strap registers
	Pattern = 0x00003148,
	Image2StreamID = 0x30E0,
	Image1StreamID = 0x30E4,
	HorzSwap = 0x0000304E,
	Gain = 0x000030E8,
	IfFpgaVersion = 0x000030F0,


	XML_ADD = 0x0000700C,
	XML_REQ = 0x00008100,

	//UpdateFirmAdrs = 0x1000000,
	//UpdateXmlAdrs = 02000000,

};

unsigned int XmlManifestSize_st           = 1;
unsigned int XmlManifestSelector_st       = 0;
unsigned int DeviceUserID_st			  = 0x11ff;
unsigned int ConnectionReset_st 		  = 0;
unsigned int ConnectionDeviceConnection_st[CXP_PORT_COUNT]  = {};
unsigned int ConnectionHostConnection_st  = 0;
unsigned int ControlPacketSizeMax_st 	  = 128;
unsigned int StreamPacketSizeMax_st 	  = 1024;
unsigned int ConnectionConfig_st          = DEFAULT_CONNECTION_CONFIG_DEFAULT;
unsigned int ConnectionConfigDefault_st   = DEFAULT_CONNECTION_CONFIG_DEFAULT;
unsigned int TestMode_st                  = 0;
unsigned int TestErrorCountSelector_st    = 0;
unsigned int TestErrorCount_st            = 0;
unsigned long long TestPacketCountTx_st   = 0;
unsigned long long TestPacketCountRx_st   = 0;
unsigned int ElectricalComplianceTest_st  = 0;
unsigned int FeatureControlRegister_st    = 0;
#if defined (MODE_CXP_VERSION_20)
unsigned int VersionUsed_st               = 0x00020000;
#else
unsigned int VersionUsed_st               = 0x00010001;
#endif

#endif // BOOTSTRAP_H


