/******************************************************************************/
/*  GigE Vision Core Firmware                                                 */
/*----------------------------------------------------------------------------*/
/*    File :  flash.c                                                         */
/*    Date :  2014-08-06                                                      */
/*     Rev :  0.1                                                             */
/*  Author :  JP                                                              */
/*----------------------------------------------------------------------------*/
/*  GigE Vision reference design SPI flash memory functions                   */
/*----------------------------------------------------------------------------*/
/*  0.1  |  2014-08-06  |  JP  |  Initial release for Zynq PS QSPI controller */
/******************************************************************************/
#if defined (IF_GIGE)

#include "gige.h"
#include "flash.h"
#include "../Common/common.h"



// ---- Global variables -------------------------------------------------------
//
volatile u32 flash_buffer[16384];

/*#################### Modefied AVALDATA Start ####################*/
#if 0
static XQspiPs spi_inst;


// ---- Initialize SPI flash controller ----------------------------------------
//
int flash_init()
{
    XQspiPs_Config *spi_cfg;

    // Initialize SPI driver
    spi_cfg = XQspiPs_LookupConfig(XPAR_XQSPIPS_0_DEVICE_ID);
    if (spi_cfg == NULL)
        return -1;
    if (XQspiPs_CfgInitialize(&spi_inst, spi_cfg, spi_cfg->BaseAddress) != XST_SUCCESS)
        return -1;

    // Perform SPI self-test
    if (XQspiPs_SelfTest(&spi_inst) != XST_SUCCESS)
        return -1;

    // Set manual start and manual chip select options and drive hold_b pin high
    XQspiPs_SetOptions(&spi_inst, XQSPIPS_MANUAL_START_OPTION | XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION);
    // Set the prescaler for QSPI clock
    XQspiPs_SetClkPrescaler(&spi_inst, XQSPIPS_CLK_PRESCALE_8);
    // Assert the FLASH chip select
    XQspiPs_SetSlaveSelect(&spi_inst);

    return 0;
}


// ---- Read double-word from SPI flash memory ---------------------------------
//
// address      = address within the SPI flash memory address space
//
// return value = contents of four consecutive bytes starting at 'address'
//
u32 flash_read_dword(u32 address)
{
    u8 wbuf[9];
    u8 rbuf[9];

    // Fast read
    wbuf[0] = 0x0B;                                     // Prepare read command
    wbuf[1] = (u8)((address >> 16) & 0xFF);
    wbuf[2] = (u8)((address >>  8) & 0xFF);
    wbuf[3] = (u8)( address        & 0xFF);
    XQspiPs_PolledTransfer(&spi_inst, wbuf, rbuf, 9);   // Read

    // Return data
    return ((u32)rbuf[5] << 24) | ((u32)rbuf[6] << 16) | ((u32)rbuf[7] << 8) | ((u32)rbuf[8]);
}


// ---- Write data block into SPI flash memory ---------------------------------
//
//           The function erases 64 kB memory block and writes new data into it
//           Maximum block length is 65536 bytes
//           The address is adjusted to start at 64 kB block boundary
//
// address = start address within the SPI flash memory
// *buffer = pointer to a dword data buffer
// length  = number of bytes to write
//
void flash_write_block(u32 address, u32 *buffer, u32 length)
{
    u8 cmd_wren     = {0x06};       // Write enable SPI command
    u8 cmd_rdstat[] = {0x05, 0x00}; // Read status register SPI command
    u8 wbuf[4 + 65536];             // Write buffer
    u8 rbuf[2];                     // Read buffer
    u32 i;

    // Adjust address to 64 kB boundaries and limit length to 64 kB
    address &= 0xFFFF0000;
    length   = minimum(length, 65536);

    // Erase block
    XQspiPs_PolledTransfer(&spi_inst, &cmd_wren, NULL, sizeof(cmd_wren));   // Write enable
    wbuf[0] = 0xD8;                                                         // Prepare sector erase command
    wbuf[1] = (u8)((address >> 16) & 0xFF);
    wbuf[2] = (u8)((address >>  8) & 0xFF);
    wbuf[3] = (u8)( address        & 0xFF);
    XQspiPs_PolledTransfer(&spi_inst, wbuf, NULL, 4);                       // Sector erase
    do {                                                                    // Wait for end of transaction
        XQspiPs_PolledTransfer(&spi_inst, cmd_rdstat, rbuf, sizeof(cmd_rdstat));
    } while ((rbuf[1] & 0x01) != 0);

    // Write block
    //XQspiPs_PolledTransfer(&spi_inst, &cmd_wren, NULL, sizeof(cmd_wren));   // Write enable
    wbuf[0] = 0x02;                                                         // Prepare write command
    wbuf[1] = (u8)((address >> 16) & 0xFF);
    wbuf[2] = (u8)((address >>  8) & 0xFF);
    wbuf[3] = (u8)( address        & 0xFF);
    for (i = 0; i < length; i++)                                            // Fill in the data
        wbuf[4 + i] = (buffer[i/4] >> (8 * (3 - (i % 4)))) & 0xFF;
    //XQspiPs_PolledTransfer(&spi_inst, wbuf, NULL, 4 + length);              // Write
    //do {                                                                    // Wait for end of transaction
    //    XQspiPs_PolledTransfer(&spi_inst, cmd_rdstat, rbuf, sizeof(cmd_rdstat));
    //} while ((rbuf[1] & 0x01) != 0);

    return;
}

#else

//----------------------------------------------------------------------------------
// externs
//----------------------------------------------------------------------------------
extern int qspiFlashWrite (unsigned int adrs, unsigned char *pBuffer, unsigned int size);
extern int qspiFlashRead (unsigned int adrs, unsigned char *pBuffer, unsigned int size);

//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
#define GIGE_FLASH_READ_SIZE	(4)


// ---- Read double-word from SPI flash memory ---------------------------------
//
// address      = address within the SPI flash memory address space
//
// return value = contents of four consecutive bytes starting at 'address'
//
u32 flash_read_dword(u32 address)
{
	unsigned int data;;

	if (qspiFlashRead (address, (unsigned char *)&data, GIGE_FLASH_READ_SIZE) != 0)
		return (-1);

	return (data);
}


// ---- Write data block into SPI flash memory ---------------------------------
//
//           The function erases 64 kB memory block and writes new data into it
//           Maximum block length is 65536 bytes
//           The address is adjusted to start at 64 kB block boundary
//
// address = start address within the SPI flash memory
// *buffer = pointer to a dword data buffer
// length  = number of bytes to write
//
void flash_write_block(u32 address, u32 *buffer, u32 length)
{
	qspiFlashWrite (address, (unsigned char *)buffer, length);
}

#endif
/*##################### Modefied AVALDATA End #####################*/
//#endif

#endif // #if defined (IF_GIGE)

// eof
