/******************************************************************************/
/*  GigE Vision Core Firmware                                                 */
/*----------------------------------------------------------------------------*/
/*    File :  framebuf.c                                                      */
/*    Date :  2011-10-17                                                      */
/*     Rev :  0.2                                                             */
/*  Author :  JP                                                              */
/*----------------------------------------------------------------------------*/
/*  GigE Vision reference design MPMC framebuffer functions                   */
/*----------------------------------------------------------------------------*/
/*  0.1  |  2011-06-20  |  JP  |  Initial release                             */
/*  0.2  |  2011-10-17  |  JP  |  Fixed framebuffer allocation                */
/******************************************************************************/

#if defined (IF_GIGE)

#include <xparameters.h>
#include <stdlib.h>
#include <stdio.h>
#include <xil_printf.h>
#include "gige.h"
#include "user.h"
#include "framebuf.h"
#include "../Common/common.h"


//================================================================================
// GigE 1Gbps
//================================================================================
#if !defined (MODE_GIGE_10G)

// ---- Initialization of the framebuffer --------------------------------------
//
//               Initialization of the framebuffer. The function returns pointer
//               to the memory allocated for the framebuffer.
//
// low_latency = enable the low-latency readout mode
// deinterlace = perform de-interlacing of the interlaced video
// size        = number of bytes to allocate
//
void *framebuf_init(int low_latency, int deinterlace, u32 size)
{
    void *fb;
    u32 align, fb_len;

    // Framebuffer core check
    if (framebuf_id != 0xA5A50001)
    {
        xil_printf("[UU] No AXI Framebuffer found!\r");
        printf("[UU] framebuf_id(0x0000) = 0x%X\r", (unsigned int)framebuf_id);
        return NULL;
    }

    // Allocate space for the framebuffer (aligned to burst length address)
    align  = ((framebuf_status & FRAMEBUF_S_BURST) >> 6) - 1;    // Burst length in bytes - 1
    fb_len = size & (~align);
    fb     = malloc(fb_len + align);
    if (!fb)
    {
        xil_printf("[UU] Error allocating memory for the framebuffer!\r");
        printf("[UU] fb = %p\r", fb);
        return NULL;
    }
    fb = (void *)(((u32)fb + align) & (~align));

    // Initialize framebuffer
    framebuf_bot     = (u32)fb;
    framebuf_top     = (u32)fb + fb_len;
    framebuf_control = FRAMEBUF_C_INIT | FRAMEBUF_C_CLRSTAT |
                       (low_latency ? FRAMEBUF_C_LOWLAT : 0) |
                       (deinterlace ? FRAMEBUF_C_DEINT  : 0);

    // Frame Buffer Address
    OUT32 (FIRM_DATA_GIGE_FRAME_BUFF_ADRS, (unsigned int )fb);

    // Print IP core information
    xil_printf("[UU] AXI Framebuffer, v%d.%d.%d (%04X-%02X-%02X)\r",
               framebuf_version >> 24, (framebuf_version >> 16) & 0xFF, framebuf_version & 0xFFFF,
               framebuf_date >> 16, (framebuf_date >> 8) & 0xFF, framebuf_date & 0xFF);
    xil_printf("     0x%08X bytes starting at 0x%08X, alignment %d bytes\r", fb_len, (u32)fb, align + 1);

    return fb;
}


// ---- Calculation of frame buffer reset -----------------------
//
// low_latency = enable the low-latency readout mode
// deinterlace = perform de-interlacing of the interlaced video
// size        = number of bytes to allocate
//

void framebuf_reset(int low_latency, int deinterlace, u32 size,u8 is_msg)
{
    u32 align = 0, fb_len = 0;

    // Allocate space for the framebuffer (aligned to burst length address)
	align  = ((framebuf_status & FRAMEBUF_S_BURST) >> 6) - 1;    // Burst length in bytes - 1
	fb_len = size & (~align);

	// Clears the buffers
	memset((void *)framebuffer,10,fb_len);

	// Initialize framebuffer
    framebuf_bot     = (u32)framebuffer;
    framebuf_top     = (u32)framebuffer + fb_len;
    framebuf_control = FRAMEBUF_C_INIT | FRAMEBUF_C_CLRSTAT |
                       (low_latency ? FRAMEBUF_C_LOWLAT : 0) |
                       (deinterlace ? FRAMEBUF_C_DEINT  : 0);
    if(1 == is_msg)
    {
#if DEBUG_MODE_MSG
		// Print IP core information
		xil_printf("[UU] AXI Framebuffer, v%d.%d.%d (%04X-%02X-%02X)\r",
				   framebuf_version >> 24, (framebuf_version >> 16) & 0xFF, framebuf_version & 0xFFFF,
				   framebuf_date >> 16,    (framebuf_date >> 8) & 0xFF,     framebuf_date & 0xFF);
		xil_printf("     0x%08X bytes starting at 0x%08X, alignment %d bytes\r", fb_len, (u32)framebuffer, align + 1);
#endif //DEBUG_MODE_MSG
    }
    return;
}


// ---- Calculation of frame padding and bytes per frame -----------------------
//
//         This function updates current line/frame padding and total bytes
//         per frame according to the video frame width and height
//
// bpp   = bytes per pixel
// deint = framebuffer is performing deinterlacing
// burst = framebuffer burst length in bytes
//
void framebuf_padding(u8 bpp, u8 deint, u8 burst)
{
    u32 pad_frame, pad_line;

    // Deinterlacing interlaced video
    if (deint)
    {
        pad_frame = 0;
        pad_line  = (u32)burst - ((video_width * (u32)bpp) % (u32)burst);
    }
    // Passing any (i or p) video through
    else
    {
        pad_frame = (u32)burst - ((video_width * video_height * (u32)bpp) % (u32)burst);
        pad_line  = 0;
    }

    // Correction
    if (pad_frame == (u32)burst)
        pad_frame = 0;
    if (pad_line == (u32)burst)
        pad_line = 0;

    // Update registers
    video_padding   = (pad_line << 16) | (pad_frame & 0xFFFF);
    video_total_bpf = (video_width * video_height * (u32)bpp) + (video_height * pad_line) + pad_frame;

    return;
}
// ---- List contents of the registers -----------------------------------------
//
void framebuf_printregs(void)
{
    xil_printf("ID registers:\r");
    xil_printf("  Core ID               = 0x%08X\r", framebuf_id);
    xil_printf("  Version               = %d.%d.%d\r", framebuf_version >> 24, (framebuf_version >> 16) & 0xFF, framebuf_version & 0xFFFF);
    xil_printf("  Build date            = %04X-%02X-%02X\r", framebuf_date >> 16, (framebuf_date >> 8) & 0xFF, framebuf_date & 0xFF);
    xil_printf("Control registers:\r");
    xil_printf("  Deinterlacing         = %s\r", (framebuf_control & FRAMEBUF_C_DEINT  ? "ON" : "OFF"));
    xil_printf("  Low latency           = %s\r", (framebuf_control & FRAMEBUF_C_LOWLAT ? "ON" : "OFF"));
    xil_printf("  Framebuffer bottom    = 0x%08X\r", framebuf_bot);
    xil_printf("  Framebuffer top + 1   = 0x%08X\r", framebuf_top);
    xil_printf("Status registers:\r");
    xil_printf("  Resend FIFO overflow  = %s\r", (framebuf_status & FRAMEBUF_S_RFO ? "YES" : "NO"));
    xil_printf("  Descr. FIFO overflow  = %s\r", (framebuf_status & FRAMEBUF_S_DFO ? "YES" : "NO"));
    xil_printf("  NPI burst length      = %d dwords\r", (framebuf_status & FRAMEBUF_S_BURST) >> 8);
    xil_printf("  Blocks in descr. FIFO = %d\r", framebuf_status & FRAMEBUF_S_BLOCKS);
    xil_printf("  Read block            = %d dwords at 0x%08X\r", framebuf_ptr_rdlen, framebuf_ptr_read);
    xil_printf("  Resend block          = %d dwords at 0x%08X\r", framebuf_ptr_rslen, framebuf_ptr_rsnd);
    xil_printf("  Write block           = %d dwords at 0x%08X\r", framebuf_ptr_wrlen, framebuf_ptr_write);
    xil_printf("  Dropped blocks (WR)   = %d\r", framebuf_blkdrop);
    xil_printf("  Dropped blocks (FIFO) = %d\r", framebuf_fifodrop);
    xil_printf("  Resend requests       = %d\r", framebuf_rsnd_req);
    xil_printf("  OK resend replies     = %d\r", framebuf_rsnd_ok);
    xil_printf("  N/A resend replies    = %d\r", framebuf_rsnd_na);
    xil_printf("  Skipped blocks (RD)   = %d\r", framebuf_blkskip);
    xil_printf("Descriptor registers:\r");
    xil_printf("  Start address         = 0x%08X\r", framebuf_d_start);
    xil_printf("  Payload length        = %d dwords\r", framebuf_d_len & FRAMEBUF_D_LEN);
    xil_printf("  Valid header length   = %d dwords\r", ((framebuf_d_len & FRAMEBUF_D_HLEN) >> 24) & 0xF);
    xil_printf("  Payload type          = %s\r", (((framebuf_d_len & FRAMEBUF_D_TYPE) >> 28) == 1 ? "IMAGE" : "RAW DATA"));

    return;
}


//================================================================================
//// GigE 10Gbps
//================================================================================
#else // #if !defined (MODE_GIGE_10G)

// Fixed start address of the static buffer (not dynamically allocated)
// TODO: Uncomment and set appropriate address for static buffer!
//#define _FRAMEBUF_DEDICATED_RAM_ 0x0000000000000000


// ---- Global variables -------------------------------------------------------
//
volatile uint16_t framebuf_pad_x;   // Line padding
volatile uint16_t framebuf_pad_y;   // Block (frame) padding
volatile uint64_t framebuf_bpb;     // Total bytes per block


// ---- Initialization of the framebuffer --------------------------------------
//
//               Initialization of the framebuffer. The function returns pointer
//               to the memory allocated for the framebuffer.
//
//         NOTE: The return value is not correct for statically allocated buffer
//               on 32b platform starting at 64b address above 4GB!
//
// low_latency = enable the low-latency readout mode
// deinterlace = perform de-interlacing of the interlaced video
// gev1mode    = operate in GEV 1.x compatibility mode
//               0: GEV 2.x (64b block id, extended GVSP status codes)
//               1: GEV 1.x (16b block id, no extended GVSP status codes)
// size        = number of bytes to allocate
//
void *framebuf_init(int low_latency, int deinterlace, int gev1mode, uint64_t size)
{
    void *fb;
    uint64_t bl, dw, align, fb_len, fb_bot;

    // Framebuffer core check
    if (framebuf_id != 0xA5A50005)
    {
        printf("[UU] No framebuffer core found!\r\n");
        return NULL;
    }

    // Allocate space for the framebuffer (aligned to burst length address)
    bl     = ((framebuf_status & FRAMEBUF_S_BURST) >> 8) + 1;   // AXI burst length in words
    dw     = 1 << (framebuf_status & FRAMEBUF_S_BITS_AXI);      // AXI data width in bytes
#if 1
    // AVALの都合で変更。4kアラインに変更
    align  = (bl * dw * 4) - 1;                                 // AXI burst length in bytes - 1
#else
    align  = (bl * dw) - 1;                                     // AXI burst length in bytes - 1
#endif
    fb_len = size & (~align);
#ifndef _FRAMEBUF_DEDICATED_RAM_
    // Frame buffers in CPU shared RAM
    fb = malloc(fb_len + align);
    if (!fb)
    {
        printf("[UU] Error allocating memory for the framebuffer!\r\n");
        return NULL;
    }

    // Frame Buffer Address
    OUT32 (FIRM_DATA_GIGE_FRAME_BUFF_ADRS, (unsigned int )fb);

    fb     = (void *)(((uintptr_t)fb + (uintptr_t)align) & (uintptr_t)(~align));
    fb_bot = (uint64_t)((uintptr_t)fb);
#else
    // Frame buffers in dedicated separate RAM (not shared with CPU)
    fb_bot = _FRAMEBUF_DEDICATED_RAM_;
    fb     = (void *)(uintptr_t)fb_bot;
#endif

    // Initialize framebuffer
    framebuf_bot_h      = (uint32_t)(fb_bot >> 32);
    framebuf_bot_l      = (uint32_t)(fb_bot & 0xFFFFFFFF);
    framebuf_top_h      = (uint32_t)((fb_bot + fb_len) >> 32);
    framebuf_top_l      = (uint32_t)((fb_bot + fb_len) & 0xFFFFFFFF);
    framebuf_control    = FRAMEBUF_C_LEADTS | FRAMEBUF_C_INIT | FRAMEBUF_C_CLRSTAT |
                          (low_latency ? RDMODE_LOW_LAT     : 0) |
                          (deinterlace ? FRAMEBUF_C_DEINT   : 0) |
                          (gev1mode    ? FRAMEBUF_C_BID16B  : 0) |
                          (!gev1mode   ? FRAMEBUF_C_EXTSTAT : 0);
    framebuf_pld_type   = PLD_IMAGE;    // Default is PLD_IMAGE, switching to chunk mode is done from user firmware with function framebuf_set_pld_type
    framebuf_lead_offs  = 0x0000;
    framebuf_trail_offs = 0x0400;

    // Enable dynamic trailers
    //framebuf_control = framebuf_control | FRAMEBUF_C_DYNTRAIL;

    // Print IP core information
    sprintf (gLogMsgBuff, "[UU] Framebuffer version %" PRIu32 ".%" PRIu32 ".%" PRIu32 " (%04" PRIX32 "-%02" PRIX32 "-%02" PRIX32 ")\r\n",
           framebuf_version >> 24, (framebuf_version >> 16) & 0xFF, framebuf_version & 0xFFFF,
           framebuf_date >> 16, (framebuf_date >> 8) & 0xFF, framebuf_date & 0xFF);
	cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);

    sprintf (gLogMsgBuff, "     0x%016" PRIX64 " bytes at 0x%016" PRIX64 ", alignment %" PRIu64 " bytes\r\n",
           fb_len, fb_bot, align + 1);
	cameraLogMsg (MSG_LEVEL_ERROR, __FILE__, __func__, __LINE__, status, gLogMsgBuff);


	
    return fb;
}


// ---- Set payload type -------------------------------------------------------
//
void framebuf_set_pld_type(uint32_t pld_type)
{
    framebuf_pld_type = pld_type;
}


// ---- Calculation of frame padding and bytes per block -----------------------
//
//         This function updates current x/y padding and total bytes per block
//         global variables according to the pixel format, width, and height
//
// pixel_format = GenICam pixel format
// size_x       = video frame width
// size_y       = video frame height
// size_chunk   = size of chunk data
//
void framebuf_padding(uint32_t pixel_format, uint32_t size_x, uint32_t size_y, uint32_t size_chunk)
{
    uint32_t burst, bpp, temp;

    // Burst length in bytes and bytes per pixel
    burst = (((framebuf_status & FRAMEBUF_S_BURST) >> 8) + 1) * (1 << (framebuf_status & FRAMEBUF_S_BITS_AXI));
    bpp   = (((pixel_format & 0x00FF0000) >> 16) + 7) / 8;

    // Padding
    if (framebuf_control & FRAMEBUF_C_DEINT)    // Interlaced scan
    {
        temp = burst - ((size_x * bpp) % burst);
        if (temp >= burst)
            temp = 0;
        framebuf_pad_x = temp;
        framebuf_pad_y = 0;
    }
    else                                        // Progressive scan
    {
        if (size_chunk == 0)
        {
            temp = burst - ((size_x * size_y * bpp) % burst);
            if (temp >= burst)
                temp = 0;
            framebuf_pad_x = 0;
            framebuf_pad_y = temp;
        }
        else
        {
            // No image padding for progressive scan and chunk mode
            framebuf_pad_x = 0;
            framebuf_pad_y = 0;
        
        }
    }

    // Total bytes per block
    framebuf_bpb = ((((uint64_t)size_x * (uint64_t)bpp) + (uint64_t)framebuf_pad_x) * (uint64_t)size_y) + (uint64_t)framebuf_pad_y + (uint64_t)size_chunk;

    // Set safe maximum block length (total bytes per block + 1 burst)
    framebuf_blk_max_h = (uint32_t)((framebuf_bpb + (uint64_t)burst) >> 32);
    framebuf_blk_max_l = (uint32_t)((framebuf_bpb + (uint64_t)burst) & 0xFFFFFFFF);
    return;
}


// ---- Get currently supported SCPS increment ---------------------------------
//
uint32_t framebuf_scps_inc(void)
{
    uint32_t dw = 1 << ( framebuf_status & FRAMEBUF_S_BITS_AXI);
    uint32_t ow = 1 << ((framebuf_status & FRAMEBUF_S_BITS_OUT) >> 24);

    return (dw >= ow ? dw : ow);
}


// ---- Initialize image leader packet -----------------------------------------
//
void framebuf_img_leader(uint32_t pixel_format, uint32_t size_x, uint32_t size_y, uint32_t offset_x, uint32_t offset_y)
{
    // Calculate padding and bytes per block
    // ... use precalculated globals instead!
//  framebuf_padding(pixel_format, size_x, size_y);

    // Load leader packet DPRAM
    framebuf_pkt_dpram[(framebuf_lead_offs / 4) + 0] = pixel_format;
    framebuf_pkt_dpram[(framebuf_lead_offs / 4) + 1] = size_x;
    framebuf_pkt_dpram[(framebuf_lead_offs / 4) + 2] = size_y;
    framebuf_pkt_dpram[(framebuf_lead_offs / 4) + 3] = offset_x;
    framebuf_pkt_dpram[(framebuf_lead_offs / 4) + 4] = offset_y;
    framebuf_pkt_dpram[(framebuf_lead_offs / 4) + 5] = ((uint32_t)framebuf_pad_x << 16) | ((uint32_t)framebuf_pad_y & 0xFFFF);
    // Total payload length is needed for U3V core only!
//  framebuf_pkt_dpram[(framebuf_lead_offs / 4) + 6] = 0;
//  framebuf_pkt_dpram[(framebuf_lead_offs / 4) + 7] = framebuf_bpb;

    // Leader packet payload length
    framebuf_lead_len = 24;

    return;
}


// ---- Initialize image trailer packet ----------------------------------------
//
void framebuf_img_trailer(uint32_t size_y, uint32_t chunk_layout_id)
{
    // Load trailer packet DPRAM (not needed if dynamic trailers are activated)
    framebuf_pkt_dpram[(framebuf_trail_offs / 4) + 0] = size_y;

   // Trailer packet payload length (also needed if dynamic trailers are activated!)
    if (video_chunk_ctrl & 0x80000000)    //check if chunks are activated
    {
        framebuf_pkt_dpram[(framebuf_trail_offs / 4) + 1] = framebuf_bpb;
        framebuf_pkt_dpram[(framebuf_trail_offs / 4) + 2] = chunk_layout_id;
        framebuf_trail_len = 12;
    }
    else
    {
        framebuf_trail_len = 4;
    }

    return;
}


// ---- List contents of the registers -----------------------------------------
//
void framebuf_printregs(void)
{
    uint32_t i;

    printf("ID registers:\r\n");
    printf("  Core ID               = 0x%08" PRIX32 "\r\n", framebuf_id);
    printf("  Version               = %" PRIu32 ".%" PRIu32 ".%" PRIu32 "\r\n", framebuf_version >> 24, (framebuf_version >> 16) & 0xFF, framebuf_version & 0xFFFF);
    printf("  Build date            = %04" PRIX32 "-%02" PRIX32 "-%02" PRIX32 "\r\n", framebuf_date >> 16, (framebuf_date >> 8) & 0xFF, framebuf_date & 0xFF);
    printf("Control registers:\r\n");
    printf("  Control               = 0x%08" PRIX32 "\r\n", framebuf_control);
    printf("  Framebuffer bottom    = 0x%08" PRIX32 "_%08" PRIX32 "\r\n", framebuf_bot_h, framebuf_bot_l);
    printf("  Framebuffer top + 1   = 0x%08" PRIX32 "_%08" PRIX32 "\r\n", framebuf_top_h, framebuf_top_l);
    printf("  Maximum block length  = 0x%08" PRIX32 "_%08" PRIX32 " = %" PRIu64 "\r\n", framebuf_blk_max_h, framebuf_blk_max_l,
                                                                                        ((uint64_t)framebuf_blk_max_h << 32) | (uint64_t)framebuf_blk_max_l);
    printf("  Leader DPRAM offset   = 0x%04" PRIX32 "\r\n", framebuf_lead_offs);
    printf("  Leader length         = %" PRIu32 "\r\n", framebuf_lead_len);
    printf("  Trailer DPRAM offset  = 0x%04" PRIX32 "\r\n", framebuf_trail_offs);
    printf("  Trailer length        = %" PRIu32 "\r\n", framebuf_trail_len);
    printf("Status registers:\r\n");
    printf("  Status                = 0x%08" PRIX32 "\r\n", framebuf_status);
    printf("                          %u bits input + AXI data width\r\n", 8 * (1 << (framebuf_status & FRAMEBUF_S_BITS_AXI)));
    printf("                          %u bits output data width\r\n", 8 * (1 << ((framebuf_status & FRAMEBUF_S_BITS_OUT) >> 24)));
    printf("                          %" PRIu32 " words bursts\r\n", 1 + ((framebuf_status & FRAMEBUF_S_BURST) >> 8));
    if (framebuf_status & FRAMEBUF_S_DYNTRAIL)
        printf("                          dynamic and external trailers supported\r\n");
    if (framebuf_status & FRAMEBUF_S_DF_OVFLW)
        printf("                          descriptor FIFO overflowed\r\n");
    if (framebuf_status & FRAMEBUF_S_RF_OVFLW)
        printf("                          resend FIFO overflowed\r\n");
    if (framebuf_status & FRAMEBUF_S_IF_OVFLW)
        printf("                          input FIFO overflowed\r\n");
    if (framebuf_status & FRAMEBUF_S_IF_EMPTY)
        printf("                          input FIFO empty\r\n");
    if (framebuf_status & FRAMEBUF_S_TF_OVFLW)
        printf("                          trailer FIFO overflowed\r\n");
    if (framebuf_status & FRAMEBUF_S_DF_FULL)
        printf("                          descriptor FIFO is full\r\n");
    if (framebuf_status & FRAMEBUF_S_WR_ACT)
        printf("                          memory write active\r\n");
    if (framebuf_status & FRAMEBUF_S_RD_ACT)
        printf("                          memory read active\r\n");
    printf("  Descr. FIFO writes    = %" PRIu32 "\r\n", framebuf_desc_wr);
    printf("  Descr. FIFO reads     = %" PRIu32 "\r\n", framebuf_desc_rd);
    printf("  Descr. FIFO drops     = %" PRIu32 "\r\n", framebuf_desc_drop);
    printf("  Write dropped blocks  = %" PRIu32 "\r\n", framebuf_wr_drop);
    printf("  Write no space in FB  = %" PRIu32 "\r\n", framebuf_wr_nosp);
    printf("  Write desc. FIFO full = %" PRIu32 "\r\n", framebuf_wr_fifo_f);
    printf("  Read skipped          = %" PRIu32 "\r\n", framebuf_rd_skip);
    printf("  Read canceled         = %" PRIu32 "\r\n", framebuf_rd_cancel);
    printf("  Read sent             = %" PRIu32 "\r\n", framebuf_rd_sent);
    printf("  Resend FIFO writes    = %" PRIu32 "\r\n", framebuf_rsnd_wr);
    printf("  Resend FIFO reads     = %" PRIu32 "\r\n", framebuf_rsnd_rd);
    printf("  Resend FIFO drops     = %" PRIu32 "\r\n", framebuf_rsnd_drop);
    printf("  Resend OK             = %" PRIu32 "\r\n", framebuf_rsnd_ok);
    printf("  Resend N/A            = %" PRIu32 "\r\n", framebuf_rsnd_na);
    printf("Pointers:\r\n");
    printf("  Write bottom          = 0x%08" PRIX32 "_%08" PRIX32 "\r\n", framebuf_p_wr_bot_h, framebuf_p_wr_bot_l);
    printf("  Write top             = 0x%08" PRIX32 "_%08" PRIX32 "\r\n", framebuf_p_wr_top_h, framebuf_p_wr_top_l);
    printf("  Read bottom           = 0x%08" PRIX32 "_%08" PRIX32 "\r\n", framebuf_p_rd_bot_h, framebuf_p_rd_bot_l);
    printf("  Resend bottom         = 0x%08" PRIX32 "_%08" PRIX32 "\r\n", framebuf_p_rs_bot_h, framebuf_p_rs_bot_l);
    printf("Descriptor registers:\r\n");
    printf("  Block start address   = 0x%08" PRIX32 "_%08" PRIX32 "\r\n", framebuf_d_start_h, framebuf_d_start_l);
    printf("  Block length          = 0x%08" PRIX32 "_%08" PRIX32 " = %" PRIu64 "\r\n", framebuf_d_len_h, framebuf_d_len_l,
                                                                                        ((uint64_t)framebuf_d_len_h << 32) | (uint64_t)framebuf_d_len_l);
    printf("  Block timestamp       = 0x%08" PRIX32 "_%08" PRIX32 "\r\n", framebuf_d_ts_h, framebuf_d_ts_l);
    printf("Interrupt registers:\r\n");
    printf("  Interrupt mask        = 0x%08" PRIX32 "\r\n", framebuf_int_mask);
    printf("  Interrupt request     = 0x%08" PRIX32 "\r\n", framebuf_int_req);
    printf("Leader DPRAM:");
    for (i = 0; i < (framebuf_lead_len + 3) / 4; i++)
    {
        if ((i % 8) == 0)
            printf("\r\n  ");
        printf("%08" PRIX32 " ", framebuf_pkt_dpram[(framebuf_lead_offs / 4) + i]);
    }
    printf("\r\nTrailer DPRAM:");
    for (i = 0; i < (framebuf_trail_len + 3) / 4; i++)
    {
        if ((i % 8) == 0)
            printf("\r\n  ");
        printf("%08" PRIX32 " ", framebuf_pkt_dpram[(framebuf_trail_offs / 4) + i]);
    }
    printf("\r\n");

    return;
}
#endif // #if !defined (MODE_GIGE_10G)

#endif // #if defined (IF_GIGE)

