/******************************************************************************/
/*  GigE Vision Core Firmware                                                 */
/*----------------------------------------------------------------------------*/
/*    File :  flash.h                                                         */
/*    Date :  2014-08-06                                                      */
/*     Rev :  0.1                                                             */
/*  Author :  JP                                                              */
/*----------------------------------------------------------------------------*/
/*  GigE Vision reference design SPI flash memory include file                */
/*----------------------------------------------------------------------------*/
/*  0.1  |  2014-08-06  |  JP  |  Initial release for Zynq PS QSPI controller */
/******************************************************************************/

#ifndef _FLASH_H_
#define _FLASH_H_


// ---- General-purpose macros -------------------------------------------------

// Get minimum of two values
#define minimum(a, b)   (a < b ? a : b)


// ---- Function prototypes ----------------------------------------------------

int  flash_init();
u32  flash_read_dword(u32 address);
void flash_write_block(u32 address, u32 *buffer, u32 length);

#endif // #ifndef _FLASH_H_

// eof
