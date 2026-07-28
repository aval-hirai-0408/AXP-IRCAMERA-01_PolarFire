//**********************************************************************************
//
//                           Camera Header
//
//      Copyright (c) 2014 - 2026 AVAL DATA Corporation All Right Reserved.
//
// The distribution policy is described in the file "COPYING"
// furnished with this package.
// 
// common.h - Common Header
//**********************************************************************************

#ifndef _COMMON_H_
#define _COMMON_H_

//----------------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "aval_status.h"
#include "uart.h"
#include "camera.h"
#include "firm.h"
#include "fpga.h"
#include "cameraSave.h"
#include "gigeCmd.h"
#include "zlib.h"

#include "../Cmd/cmd.h"
#include "../Firm/correction.h"
#include "../Diag/diag.h"

#include "miv_rv32_hal.h"
#include "hw_platform.h"
#include "hw_reg_access.h"
#include "CoreUARTapb/core_uart_apb.h"
#include "CoreTimer/core_timer.h"
#include "CoreSPI/core_spi.h"
#include "CoreI2C/core_i2c.h"
#include "miv_i2c/miv_i2c.h"
#include "miv_timer/miv_timer.h"
#include "miv_plic/miv_plic.h"

#endif  // _COMMON_H_

// eof
