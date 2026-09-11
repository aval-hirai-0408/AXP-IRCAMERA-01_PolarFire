//**********************************************************************************
//
//                              Camera Program
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
//
// i2cPs.c - I2C Ps Program
//**********************************************************************************

//----------------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------------
#include "../Common/common.h"


#if defined (MODE_I2C)
//----------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------
//#define MIV_I2C	//@@@1

// Page Size
#define I2C_PAGE_SIZE  						(254)

// Buffer Size
#define I2C_READ_BUFF_SIZE					(I2C_PAGE_SIZE+2)
#define I2C_WRITE_BUFF_SIZE					(I2C_PAGE_SIZE+2)

// I2C Timeout
#define I2C_TRANSMIT_TIMEOUT				(5000)
#define I2C_EXCLUSION_TIMEOUT				(5000)



#if !defined (MIV_I2C)

#define MASTER_SER_ADDR     0x21

#endif // #if defined (MIV_I2C)



//----------------------------------------------------------------------------------
// globals
//----------------------------------------------------------------------------------

// I2C Status
int gI2cPsStatus = 0;

// Buffer
unsigned char gpI2cReadBuffer[I2C_READ_BUFF_SIZE];
unsigned char gpI2cWriteBuffer[I2C_WRITE_BUFF_SIZE];

// Instance
#if defined (MIV_I2C)
miv_i2c_instance_t miv_i2c;
#else
i2c_instance_t g_core_i2c0;
i2c_instance_t g_core_i2c1;
#endif


//**********************************************************************************
//	初期化
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		-
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int i2cInitialize (void)
{
	int status = AVAL_STATUS_SUCCESS;

#if defined (MIV_I2C)

	// Initialize
	MIV_I2C_init (&miv_i2c, MIV_I2C_BASE_ADDR);

	// Config
    MIV_I2C_config (&miv_i2c, 0x63);

	// Interrupt Enable
	MRV_enable_local_irq (MRV32_MSYS_EIE2_IRQn);

#else // #if defined (MIV_I2C)

	// Initialize the I2C0 Driver
	I2C_init (&g_core_i2c0, COREI2C0_BASE_ADDR, MASTER_SER_ADDR, I2C_PCLK_DIV_256);

	// Initialize the system tick for 10mS operation or 1 tick every 100th of
	// a second and also make sure it is lower priority than the I2C IRQs.
	MRV_systick_config(SYS_CLK_FREQ / 2);

	// Interrupt Enable
    MRV_enable_local_irq (MRV32_MSYS_EIE0_IRQn);

#endif // #if defined (MIV_I2C)

_DONE:
	return (gI2cPsStatus);
}


#if defined (MIV_I2C)
//**********************************************************************************
//	割り込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				:ポート番号
//		slaveAdrs			:スレーブアドレス
//		adrs				:書き込みアドレス
//		pBuffer				:書き込みデータを格納したポインタ
//		size				:書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
void MSYS_EI2_IRQHandler(void)
{
    MIV_I2C_isr (&miv_i2c);
}
#else // #if defined (MIV_I2C)
uint8_t MSYS_EI0_IRQHandler(void)
{
    I2C_isr(&g_core_i2c0);
    return (EXT_IRQ_KEEP_ENABLED);
}
#endif // #if defined (MIV_I2C)



//**********************************************************************************
//	書き込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				:ポート番号
//		slaveAdrs			:スレーブアドレス
//		adrs				:書き込みアドレス
//		pBuffer				:書き込みデータを格納したポインタ
//		size				:書き込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int i2cWrite (int port, unsigned int slaveAdrs, unsigned int adrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int transed, transSize;
	unsigned int alignAdrs, alignSize;
	int i;

#if defined (MIV_I2C)
	transSize = 0;
	for (transed=0; transed<size; transed+=transSize, adrs+=transSize)
	{
		//------------------------------------------------------------
		// Write転送サイズ設定
		//------------------------------------------------------------
		transSize = size - transed;
		if (transSize > I2C_PAGE_SIZE)
			transSize = I2C_PAGE_SIZE;

		//------------------------------------------------------------
		// スタートアドレスの確認
		//------------------------------------------------------------
		if (adrs & (I2C_PAGE_SIZE-1))
		{
			// I2C_PAGE_SIZEアライン分のサイズのみを転送する
			alignAdrs = adrs & (I2C_PAGE_SIZE-1);
			alignSize = I2C_PAGE_SIZE - alignAdrs;
			
			if (transSize > alignSize)
				transSize = alignSize;
		}

		//------------------------------------------------------------
		// Set Adrs
		//------------------------------------------------------------
		gpI2cWriteBuffer[0] = (unsigned char)((adrs>>8) & 0xff);
		gpI2cWriteBuffer[1] = (unsigned char)(adrs & 0xff);
		
		//------------------------------------------------------------
		// WriteデータCopy
		//------------------------------------------------------------
		for (i=0; i<I2C_PAGE_SIZE; i++)
			gpI2cWriteBuffer[(i+2)] = pBuffer[(transed+i)];
		
		//------------------------------------------------------------
		// Write
		//------------------------------------------------------------
		MIV_I2C_write (&miv_i2c, slaveAdrs, gpI2cWriteBuffer, transSize, MIV_I2C_RELEASE_BUS, MIV_I2C_ACK_POLLING_ENABLE);

		//------------------------------------------------------------
		// Check Status
		//------------------------------------------------------------
		for (i=0; i<I2C_TRANSMIT_TIMEOUT; i++)
		{
			if (MIV_I2C_IN_PROGRESS != miv_i2c.master_status)
				break;
			
			msDelay (1);
		}
		
		if (i >= I2C_TRANSMIT_TIMEOUT)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_I2C, AVAL_STATUS_TIMEOUT);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "I2C Write Timeout Error.\n");
			goto _DONE;
		}
	}

#else // #if defined (MIV_I2C)

    i2c_status_t i2cSatus;

	transSize = 0;
	for (transed=0; transed<size; transed+=transSize, adrs+=transSize)
	{
		//------------------------------------------------------------
		// Write転送サイズ設定
		//------------------------------------------------------------
		transSize = size - transed;
		if (transSize > I2C_PAGE_SIZE)
			transSize = I2C_PAGE_SIZE;

		//------------------------------------------------------------
		// スタートアドレスの確認
		//------------------------------------------------------------
		if (adrs & (I2C_PAGE_SIZE-1))
		{
			// I2C_PAGE_SIZEアライン分のサイズのみを転送する
			alignAdrs = adrs & (I2C_PAGE_SIZE-1);
			alignSize = I2C_PAGE_SIZE - alignAdrs;
			
			if (transSize > alignSize)
				transSize = alignSize;
		}

		//------------------------------------------------------------
		// Set Adrs
		//------------------------------------------------------------
		gpI2cWriteBuffer[0] = (unsigned char)((adrs>>8) & 0xff);
		gpI2cWriteBuffer[1] = (unsigned char)(adrs & 0xff);

		//------------------------------------------------------------
		// Write
		//------------------------------------------------------------
		I2C_write (&g_core_i2c0, slaveAdrs, gpI2cWriteBuffer, transSize, I2C_RELEASE_BUS);

		//------------------------------------------------------------
		// Check Status
		//------------------------------------------------------------
    	i2cSatus = I2C_wait_complete (&g_core_i2c0, I2C_TRANSMIT_TIMEOUT);

		if (i2cSatus != I2C_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_I2C, AVAL_STATUS_TIMEOUT);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "I2C Write Timeout Error.\n");
			goto _DONE;
		}
	}

#endif // #if defined (MIV_I2C)

_DONE:
	return (status);
}


//**********************************************************************************
//	読み込み
//----------------------------------------------------------------------------------
//	[ INPUT ]
//		port				:ポート番号
//		slaveAdrs			:スレーブアドレス
//		adrs				:読み込みアドレス
//		pBuffer				:データを格納するポインタ
//		size				:読み込みサイズ
//	[ OUTPUT ]
//		AVAL_STATUS_SUCCESS	：正常終了
//		上記以外				：異常終了
//==================================================================================
int i2cRead (int port, unsigned int slaveAdrs, unsigned int adrs, unsigned char *pBuffer, unsigned int size)
{
	int status = AVAL_STATUS_SUCCESS;
	unsigned int transed, transSize;
	unsigned int alignAdrs, alignSize;
	uint8_t regAdrs[2];
	int i;

#if defined (MIV_I2C)
	transSize = 0;
	for (transed=0; transed<size; transed+=transSize, adrs+=transSize)
	{
		//------------------------------------------------------------
		// Read転送サイズ設定
		//------------------------------------------------------------
		transSize = size - transed;
		if (transSize > I2C_PAGE_SIZE)
			transSize = I2C_PAGE_SIZE;

		//------------------------------------------------------------
		// スタートアドレスの確認
		//------------------------------------------------------------
		if (adrs & (I2C_PAGE_SIZE-1))
		{
			// I2C_PAGE_SIZEアライン分のサイズのみを転送する
			alignAdrs = adrs & (I2C_PAGE_SIZE-1);
			alignSize = I2C_PAGE_SIZE - alignAdrs;
			
			if (transSize > alignSize)
				transSize = alignSize;
		}

		//------------------------------------------------------------
		// Set Adrs
		//------------------------------------------------------------
		regAdrs[0] = (unsigned char)((adrs>>8) & 0xff);
		regAdrs[1] = (unsigned char)(adrs & 0xff);

		//------------------------------------------------------------
		// Read
		//------------------------------------------------------------
		MIV_I2C_write_read (&miv_i2c, slaveAdrs, regAdrs, 2, &pBuffer[transed], transSize, MIV_I2C_RELEASE_BUS, MIV_I2C_ACK_POLLING_ENABLE);

		//------------------------------------------------------------
		// Check Status
		//------------------------------------------------------------
		for (i=0; i<I2C_TRANSMIT_TIMEOUT; i++)
		{
			if (MIV_I2C_IN_PROGRESS != miv_i2c.master_status)
				break;
			
			msDelay (1);
		}
		
		if (i >= I2C_TRANSMIT_TIMEOUT)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_I2C, AVAL_STATUS_TIMEOUT);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "I2C Read Timeout Error.\n");
			goto _DONE;
		}
	}

#else // #if defined (MIV_I2C)

    i2c_status_t i2cSatus;

	transSize = 0;
	for (transed=0; transed<size; transed+=transSize, adrs+=transSize)
	{
		//------------------------------------------------------------
		// Read転送サイズ設定
		//------------------------------------------------------------
		transSize = size - transed;
		if (transSize > I2C_PAGE_SIZE)
			transSize = I2C_PAGE_SIZE;

		//------------------------------------------------------------
		// スタートアドレスの確認
		//------------------------------------------------------------
		if (adrs & (I2C_PAGE_SIZE-1))
		{
			// I2C_PAGE_SIZEアライン分のサイズのみを転送する
			alignAdrs = adrs & (I2C_PAGE_SIZE-1);
			alignSize = I2C_PAGE_SIZE - alignAdrs;
			
			if (transSize > alignSize)
				transSize = alignSize;
		}

		//------------------------------------------------------------
		// Set Adrs
		//------------------------------------------------------------
		gpI2cWriteBuffer[0] = (unsigned char)((adrs>>8) & 0xff);
		gpI2cWriteBuffer[1] = (unsigned char)(adrs & 0xff);
		
		for (i=0; i<I2C_PAGE_SIZE; i++)
			gpI2cWriteBuffer[(i+2)] = pBuffer[(transed+i)];

		//------------------------------------------------------------
		// Read
		//------------------------------------------------------------
		I2C_write_read (&g_core_i2c0, slaveAdrs, gpI2cWriteBuffer, 2, &gpI2cReadBuffer[transed], transSize, I2C_RELEASE_BUS);

		//------------------------------------------------------------
		// Check Status
		//------------------------------------------------------------
    	i2cSatus = I2C_wait_complete (&g_core_i2c0, I2C_TRANSMIT_TIMEOUT);

		if (i2cSatus != I2C_SUCCESS)
		{
			status = MAKE_ERROR_STATUS (AVAL_STATUS_I2C, AVAL_STATUS_TIMEOUT);
			cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, "I2C Read Timeout Error.\n");
			goto _DONE;
		}
	}

#endif // #if defined (MIV_I2C)

_DONE:
	return (status);
}

#endif // #if definde (MODE_I2C)

// eof
