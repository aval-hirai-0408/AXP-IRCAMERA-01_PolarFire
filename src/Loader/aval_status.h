//**********************************************************************************
//
//                           Camera Header
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// aval_status.h - Firmware Error Header
//**********************************************************************************


#ifndef __AVAL_STATUS_H__
#define __AVAL_STATUS_H__

//============================================================================
// Common status
// Name                              Codes :   Level      :  Description
//============================================================================
#define AVAL_STATUS_SUCCESS                0  /**! Info   : Operation was successful; No error occurred. */
#define AVAL_STATUS_ERROR              -1001  /**! Error  : Unspecified runtime error. */
#define AVAL_STATUS_NOT_INITIALIZED    -1002  /**! Error  : Module not initialized; e.g. An initialize method was not called before handling; or cannot establish a connection between devices(a frame grabber and a camera). */
#define AVAL_STATUS_NOT_IMPLEMENTED    -1003  /**! Error  : Requested operation not implemented; e.g. No supported event or has no functions on a buffer module. */
#define AVAL_STATUS_RESOURCE_IN_USE    -1004  /**! Warn   : Requested resource is already in use by another operator. Requested operation is not allowed; e.g. a remote device is opened by another client. */
#define AVAL_STATUS_ACCESS_DENIED      -1005  /**! Warn   : Requested operation is not allowed; e.g. a remote device is opened by another client. */
#define AVAL_STATUS_INVALID_HANDLE     -1006  /**! Error  : Given handle does not support the operation; e.g. function call on wrong handle or NULL pointer. */
#define AVAL_STATUS_INVALID_ID         -1007  /**! Error  : ID could not be connected to a resource; e.g. a device with the given ID is currently not available. */
#define AVAL_STATUS_NO_DATA            -1008  /**! Error  : The function has no data to work on. */
#define AVAL_STATUS_INVALID_PARAMETER  -1009  /**! Error  : One of the parameter given was not valid or out of range and none of the error codes above fits. */
#define AVAL_STATUS_IO                 -1010  /**! Error  : Communication error has occurred; e.g.; A read and/or write operation to a remote device is failed.  Or To access to the physical module 'PCI'. */
#define AVAL_STATUS_TIMEOUT            -1011  /**! Warn   : An operation’s timeout time period expired before it could be completed. */
#define AVAL_STATUS_ABORT              -1012  /**! Info   : An operation has been aborted before it could be completed. For example a wait operation through a waiting any event. */
#define AVAL_STATUS_INVALID_BUFFER     -1013  /**! Error  : No buffer announced for an acquisition or one or more buffers of the acquisition with invalid buffer size. */
#define AVAL_STATUS_NOT_AVAILABLE      -1014  /**! Warn   : Resource or information is not available at a given time in a current state. */
#define AVAL_STATUS_INVALID_ADDRESS    -1015  /**! Error  : A given address is out of range or invalid for internal reasons. */

#define AVAL_STATUS_BUFFER_TOO_SMALL   -1016  /**! Error  : Ver.1.2.0.0 */
#define AVAL_STATUS_INVALID_INDEX      -1017  /**! Error  : Ver.1.2.0.0 */
#define AVAL_STATUS_PARSING_CHUNK_DATA -1018  /**! Error  : Ver.1.2.0.0 */
#define AVAL_STATUS_INVALID_VALUE      -1019  /**! Error  : Ver.1.2.0.0 */
#define AVAL_STATUS_RESOURCE_EXHAUSTED -1020  /**! Error  : Ver.1.2.0.0 */
#define AVAL_STATUS_OUT_OF_MEMORY      -1021  /**! Error  : Ver.1.2.0.0 */
#define AVAL_STATUS_INVALID_ARGUMENT   -1022  /**! Error  : The number of the argument isn't right. */

#define AVAL_STATUS_CUSTOM_ID          	-10000 /**! Error  : Any error smaller or equal than -10000 is implementation specific. If a Consumer application receives such an error number it should react as if it would be a generic runtime error. */
#define AVAL_STATUS_CONNECT_LOST       	-20001 /**! Warn   : A connection is lost.  e.g.; a physical cable plug-off; or stopped supply a power with a camera device*/
#define AVAL_STATUS_SHORTAGE_RESOURCE  	-20002 /**! Warn   : A resources is shortage.  e.g.; a physical continuas ; remapping memry; an image buffer; a FIFO memory or a storage.*/
#define AVAL_STATUS_OUT_OF_RANGE       	-20003 /**! Error  : One; or some parameter(s) was out of range . the parameter(s) exceeded by max or less than min. */
#define AVAL_STATUS_INVALID_XFER       	-20004 /**! Error  : The transfer parameters(DMA script; buffer address/list and the size of an image) for an acquisition is invalid. */
#define AVAL_STATUS_UNABLE_READ        	-20005 /**! Error  : Reading error has occurred; e.g.; A read operation to a remote(e.g.; Camera) or a local device(e.g.; PCI) is failed.  */
#define AVAL_STATUS_UNABLE_WRITE       	-20006 /**! Error  : Writing error has occurred; e.g.; A write operation to a remote(e.g., Camera) or a local device(e.g.; PCI) is failed.  */
#define AVAL_STATUS_NOT_NOTIFIED       	-20007 /**! Error  : Announce or received event's error has occurred; e.g.; Notification (e.g.; the end of frame, interrupt GPIN or received message ) to a user is failed.  */
#define AVAL_STATUS_COMPARE            	-20008 /**! Error  : Compare Error.  */
#define AVAL_STATUS_INVALID_FILE       	-20009 /**! Error  : Invalid File Name.  */
#define AVAL_STATUS_ADJUST_IMPOSSIBLE  	-20010 /**! Error  : Adjust Error.  */
#define AVAL_STATUS_CMD_ERROR          	-20011 /**! Error  : Command Error.  */
#define AVAL_STATUS_COMMUNICATION		-20012 /**! Error  : Communication Error.  */
#define AVAL_STATUS_CHECKSUM			-20013 /**! Error  : CheckSum Error.  */
#define AVAL_STATUS_UNABLE_ERASE       	-20014 /**! Error  : Erasing error has occurred; e.g.; A write operation to a remote(e.g., Camera) or a local device(e.g.; PCI) is failed.  */
#define AVAL_STATUS_RECIVE_RETRY       	-20015
#define AVAL_STATUS_CHLSUM_RETRY       	-20016

#define AVAL_STATUS_NO_ANSWER       	-30000


//============================================================================
// Device status
//============================================================================
#define AVAL_STATUS_INTC				(-1)  // Interrupt Controller
#define AVAL_STATUS_TIMER				(-2)  // Timer Controller
#define AVAL_STATUS_FLASH				(-3)  // Flash Controller
#define AVAL_STATUS_DMA					(-7)  // DMA Controller
#define AVAL_STATUS_PELTIER				(-8)  // Pelter Controller
#define AVAL_STATUS_BOARD				(-11) // Board Function
#define AVAL_STATUS_UART				(-12) // UART Controller
#define AVAL_STATUS_CAMERA				(-15) // Camera Function
#define AVAL_STATUS_FFC					(-16) // FFC Function
#define AVAL_STATUS_DPC					(-17) // DPC Function
#define AVAL_STATUS_CXP					(-18) // CoaxPress
#define AVAL_STATUS_LUT					(-21) // LUT Function
#define AVAL_STATUS_SPECTRUM			(-22) // Spectrum Function
#define AVAL_STATUS_I2C					(-25) // I2C
#define AVAL_STATUS_GIGE				(-29) // GigE Function
#define AVAL_STATUS_SENSOR				(-31) // Sensor Function


//============================================================================
// Make Code
//============================================================================
#define MAKE_ERROR_STATUS(dev,error)	(((dev&0xffff)<<16)|(error&0xffff))
#define MAKE_DEVICE_STATUS(dev)			((dev==0)?   (0) : (0-(0x10000-((dev>>16)&0xffff))))
#define MAKE_ERRNUM_STATUS(error)		((error==0)? (0) : (0-(0x10000-(error&0xffff))))

#endif // __AVAL_STATUS_H__

// eof
