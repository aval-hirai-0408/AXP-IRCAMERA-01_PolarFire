//**********************************************************************************
//
//                              Camera Program Header
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// main.c - Main Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>
#include "hw_platform.h"
#include "miv_rv32_hal.h"

#include "core_uart_apb.h"
#include "core_spi.h"

#include "common.h"
#include "flash.h"


//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------
UART_instance_t g_uart;
uint8_t g_read_buf[FLASH_READ_SIZE];


//----------------------------------------------------------------------------------
// function defined
//----------------------------------------------------------------------------------
void read_program_from_flash_and_copy_to_ddr(void);
static void Bootloader_JumpToApplication(uint32_t stack_location, uint32_t reset_vector);
void delay1 (volatile uint32_t n);
//void debugFunc(void);


//----------------------------------------------------------------------------------
// function defined
//----------------------------------------------------------------------------------
#if 0
extern void FLASH_init (void);
extern void FLASH_global_unprotect (void);
extern void FLASH_read_device_id (uint8_t * manufacturer_id, uint8_t * device_id);
extern void FLASH_read (uint32_t address, uint8_t * rx_buffer, size_t size_in_bytes);
#endif

//**********************************************************************************
// Main
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		-
//==================================================================================
int loaderMain (void)
{
    int i, verSize;
    char *ptr8;
	//uint8_t rx_data[BUFFER_UART_SIZE];
	//size_t rx_size;

	// UART Initialize
	UART_init (&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY) );

	// BootStrap
	UART_polled_tx_string (&g_uart, (uint8_t*)"BootLoader Ver.");
	UART_polled_tx_string (&g_uart, (uint8_t*)FIRM_VERSION);
    UART_polled_tx_string (&g_uart, (uint8_t*)"\r\n");
	
    // Version Save
    verSize = 3;
    ptr8 = (char *)FIRM_VERSION;
    for (i=0; i<verSize; i++, ptr8++)
        (*(volatile unsigned char *)(FIRM_DATA_BOOT_VERSION + i)) = *ptr8;

    // 邨らｫｯ
    (*(volatile unsigned char *)(FIRM_DATA_BOOT_VERSION + i)) = '\0';

	delay1(1000);

	//debugFunc();

	// Load
	UART_polled_tx_string( &g_uart, ( uint8_t*)"Application copying to DDR from SPI flash is in progress\n\r");
    read_program_from_flash_and_copy_to_ddr();

	// Address
    //UART_polled_tx_string( &g_uart, ( uint8_t*)"Adrs = ");
    //sprintf (rx_data, "%x", DDR_BASE_ADDRESS);
    //UART_polled_tx_string( &g_uart, ( uint8_t*)rx_data);

    // Size
    //UART_polled_tx_string( &g_uart, ( uint8_t*)" / Size = ");
    //sprintf (rx_data, "%x", DDR_APP_MAX_SIZE);
    //UART_polled_tx_string( &g_uart, ( uint8_t*)rx_data);
    //UART_polled_tx_string( &g_uart, ( uint8_t*)"\r\n");

	// Start
	UART_polled_tx_string( &g_uart, (uint8_t*)"Application Execution control will be transferred to DDR\n\r");
	Bootloader_JumpToApplication((uint32_t)DDR_BASE_ADDRESS, (uint32_t)DDR_BASE_ADDRESS+4);

	// Loop
    while(1);

	return 0;
}


//**********************************************************************************
// SPI to DDR
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		-
//==================================================================================
#if 1
void read_program_from_flash_and_copy_to_ddr(void)
{
	volatile uint32_t i, j;

	// Initialize
	qspiFlashInitialize ();
	
	//ddr memory initialization with 0x55
	//for (i=0; i<DDR_APP_MAX_SIZE; i=i+4)
		//*(volatile uint32_t*)(DDR_BASE_ADDRESS+i) = 0x55555555;

	// Load
	for (i=0;i <DDR_APP_MAX_SIZE; i=i+FLASH_READ_SIZE)
	{
		#if 1

		qspiFlashRead ((FLASH_PRGRAM_START_ADRS+i), (unsigned char *)(DDR_BASE_ADDRESS+i), FLASH_READ_SIZE);

		#else
		
		qspiFlashRead ((FLASH_PRGRAM_START_ADRS+i), &g_read_buf[0], FLASH_READ_SIZE);

        for(j=0;j<FLASH_READ_SIZE;j++)
			*(volatile uint8_t*)(DDR_BASE_ADDRESS+i+j) = g_read_buf[j];
		
		#endif
	}
	
	return;
}
#else
void read_program_from_flash_and_copy_to_ddr(void)
{
	volatile uint32_t i, j;
	volatile uint32_t address;
	uint8_t manufacturer_id, device_id;

	address = FLASH_PRGRAM_START_ADRS;

	// Initialize
	FLASH_init();
	FLASH_global_unprotect();
	FLASH_read_device_id (&manufacturer_id, &device_id);

	//ddr memory initialization with 0x55
	for (i=0; i<DDR_APP_MAX_SIZE; i=i+4)
		*(volatile uint32_t*)(DDR_BASE_ADDRESS+i) = 0x55555555;

	// Load
	for (i=0;i <DDR_APP_MAX_SIZE; i=i+FLASH_READ_SIZE)
	{
		FLASH_read ((address+i), &g_read_buf[0], FLASH_READ_SIZE);

        for(j=0;j<FLASH_READ_SIZE;j++)
			*(volatile uint8_t*)(DDR_BASE_ADDRESS+i+j) = g_read_buf[j];
	}
	
	return;
}
#endif


//**********************************************************************************
// Jump Address
// Call this function if you want to switch to another program
// de-init any loaded drivers before calling this function
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		-
//==================================================================================
static void Bootloader_JumpToApplication (uint32_t stack_location, uint32_t reset_vector)
{
	//__disable_irq(); org
	MRV_disable_interrupts();

	// PLIC_init(); org
	MRV_disable_interrupts();

	// Disable all interrupts:
	write_csr(mie, 0);

	// Start executing from the top of DDR memory:
	__asm volatile("lui ra,0x80010");

	// Flush the cache.
	__asm volatile ("fence.i");

	// We need to explicitly execute a return instruction in case the compiler had
	// done some return address register manipulation in this function's veneer.
	__asm volatile("ret");

	// User application execution should now start and never return here....
}


//**********************************************************************************
// Delay
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		-
//==================================================================================
void delay1 (volatile uint32_t n)
{
	while(n)
		n--;
}


//**********************************************************************************
// Debug
//----------------------------------------------------------------------------------
//  [ INPUT ]
//      -
//  [ OUTPUT ]
//      -
//==================================================================================
#if 0
void debugFunc(void)
{
    uint8_t rx_data[BUFFER_UART_SIZE];
    unsigned int adrs;
    unsigned char data8;
    unsigned int data32;
    unsigned int totalCount, count;
    int disp = 1;


    while(1)
    {
        if (disp == 1)
        {
            UART_polled_tx_string( &g_uart, (uint8_t*)"0:Read\n\r");
            UART_polled_tx_string( &g_uart, (uint8_t*)"1:Write\n\r");
            disp = 0;
        }


       if (UART_get_rx ( &g_uart, rx_data, 1 ) == 1)
       {
           switch(rx_data[0])
           {
               // Read
               case '0':
                   UART_polled_tx_string( &g_uart, (uint8_t*)"Adrs = ");

                   totalCount = 0;
                   while (1)
                   {
                       count = UART_get_rx ( &g_uart, &rx_data[totalCount], 1);

                       if (count == 1)
                           UART_send( &g_uart, (uint8_t*)&rx_data[totalCount], 1);

                       totalCount += count;

                       if (totalCount >= 8)
                           break;
                   }
                   UART_polled_tx_string( &g_uart, (uint8_t*)"\n\r");

                   sscanf (rx_data, "%x", &adrs);

                   data8 = (*(volatile unsigned char *)adrs);
                   sprintf (rx_data, "%x", data8);
                   UART_polled_tx_string( &g_uart, (uint8_t*)"Data = ");
                   UART_polled_tx_string( &g_uart, (uint8_t*)rx_data);
                   UART_polled_tx_string( &g_uart, (uint8_t*)"\n\r");
                   disp = 1;
                   break;

               case '1': // transfer file to DDR
                   UART_polled_tx_string( &g_uart, (uint8_t*)"Adrs = ");
                   totalCount = 0;
                   while (1)
                   {
                       count = UART_get_rx ( &g_uart, &rx_data[totalCount], 1);

                       if (count == 1)
                           UART_send( &g_uart, (uint8_t*)&rx_data[totalCount], 1);

                       totalCount += count;

                       if (totalCount >= 8)
                           break;
                   }
                   UART_polled_tx_string( &g_uart, (uint8_t*)"\n\r");

                   sscanf (rx_data, "%x", &adrs);

                   UART_polled_tx_string( &g_uart, (uint8_t*)"Data = ");
                   totalCount = 0;
                   while (1)
                   {
                       count = UART_get_rx ( &g_uart, &rx_data[totalCount], 1);

                       if (count == 1)
                           UART_send( &g_uart, (uint8_t*)&rx_data[totalCount], 1);

                       totalCount += count;

                       if (totalCount >= 2)
                           break;
                   }
                   UART_polled_tx_string( &g_uart, (uint8_t*)"\n\r");

                   rx_data[2] = 0;
                   sscanf (rx_data, "%x", &data32);
                   data8 = (unsigned int)data32;
                   (*(volatile unsigned char *)adrs) = data8;
                   disp = 1;

                   break;

               default:
                    break;
           }
       }
    }
}
#endif // #if 0

// eof
