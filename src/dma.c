/*
 * dma.c
 *
 *  Created on: July 3, 2024
 *      Author: micahbly
 */
 





/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "dma.h"

#include "general.h"
#include "memory.h"
#include "sys.h"
#include "text.h"

// C includes
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Platform includes
#include "f256_e.h"

// for debug only
// #include <stdio.h>
// #include "comm_buffer.h"
// extern char*		global_string_buff1;
// extern char*		global_string_buff2;

/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/



/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/



/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

//! \cond PRIVATE



//! \endcond



/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// **** NOTE: all functions in private section REQUIRE pre-validated parameters. 
// **** NEVER call these from your own functions. Always use the public interface. You have been warned!




//! \cond PRIVATE



// **** Debug functions *****


//! \endcond



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/


#if defined _DMAC_


//! Copy a linear span of memory from one buffer to another, using the DMA engine
//! @param dst: the location data will be copied to
//! @param src: the location data will be copied from
//! @param num_bytes: the number of bytes to be copied. 20-bit max on F256c, 24-bit max on F256e.
void DMA_Copy(uint8_t* dst, uint8_t* src, uint32_t num_bytes)
{
	uint8_t		the_bits;
	uint32_t	src_addr_int = (uint32_t)src;
	uint32_t	dst_addr_int = (uint32_t)dst;
	
	// DMA copy only works with even widths
	if (num_bytes < 2)
	{
		return;
	}
	
	// make sure DMA engine is off
	R8(DMA_CTRL) = 0;
	
	// enable for 2D operation, with fill and 2D operations flags cleared
	R8(DMA_CTRL) = (FLAG_DMA_CTRL_ENABLE);
	
	// set source address
	R8(DMA_SRC_ADDR_L) = (src_addr_int) & 0xff;
	R8(DMA_SRC_ADDR_M) = (src_addr_int >> 8) & 0xff;
	R8(DMA_SRC_ADDR_H) = (src_addr_int >> 16) & 0xff;
// sprintf(global_string_buff1, "src=%p, as written to vicky=%x, %x, %x", src, lo, med, hi);
// Buffer_NewMessage(global_string_buff1);

	// set destination address
	R8(DMA_DST_ADDR_L) = (dst_addr_int) & 0xff;
	R8(DMA_DST_ADDR_M) = (dst_addr_int >> 8) & 0xff;
	R8(DMA_DST_ADDR_H) = (dst_addr_int >> 16) & 0xff;

//sprintf(global_string_buff1, "src=%p, as written to vicky=%x, %x, %x", src, ((uint32_t)(src) >> (8*0)) & 0xff, ((uint32_t)(src) >> 8) & 0xff, ((uint32_t)(src) >> 16) & 0xff);
//sprintf(global_string_buff1, "src=%p, as written to vicky=%x, %x, %x", src, (src_addr_int >> (8*0)) & 0xff, (src_addr_int >> 8) & 0xff, (src_addr_int >> 16) & 0xff);
//sprintf(global_string_buff1, "src=%p, with pointer math=%x, %x, %x", src, src[0], src[1], src[2]);
// sprintf(global_string_buff1, "dst=%p, as written to vicky=%x, %x, %x", dst, lo, med, hi);
// Buffer_NewMessage(global_string_buff1);

	// set num bytes to copy (20-bit)
	R8(DMA_COUNT_L) = (num_bytes) & 0xff;
	R8(DMA_COUNT_M) = (num_bytes >> 8) & 0xff;
	R8(DMA_COUNT_H) = (num_bytes >> 16) & 0xff;
	
	// flip the start flag, and wait for it to complete
	the_bits = R8(DMA_CTRL) | FLAG_DMA_CTRL_START;
	R8(DMA_CTRL) = the_bits;
	
	do
	{
		the_bits = R8(DMA_STATUS);
	} while (the_bits >= FLAG_DMA_STATUS_BUSY);
	
	// turn the DMA engine off
	R8(DMA_CTRL) = 0;
}


//! Copy a rectangular section of memory from one buffer to another, using the DMA engine
//! @param dst: the location data will be copied to
//! @param dst_stride: the "width" of the frame being copied into. The width of the screen, for example.
//! @param src: the location data will be copied from
//! @param width, height: the size of the rectangle to be copied, in bytes. 
//! @param src_stride: the "width" of the frame being copied from. If you are copying from one place in the screen to another, this would be the screen width. If you are copying from an image buffer to the screen, this would be the width of the image.
void DMA_CopyRect(uint8_t* dst, uint16_t dst_stride, uint8_t* src, uint16_t width, uint16_t height, uint16_t src_stride)
{
	uint8_t		the_bits;
	uint32_t	src_addr_int = (uint32_t)src;
	uint32_t	dst_addr_int = (uint32_t)dst;
	
	// DMA copy only works with even widths
	if (width % 2)
	{
		width--;
	}
	
	// make sure DMA engine is off
	R8(DMA_CTRL) = 0;
	
	// enable for 2D operation, with fill flag cleared
	R8(DMA_CTRL) = (FLAG_DMA_CTRL_ENABLE | FLAG_DMA_CTRL_2D_OP);
	
	// set source address
	R8(DMA_SRC_ADDR_L) = (src_addr_int) & 0xff;
	R8(DMA_SRC_ADDR_M) = (src_addr_int >> 8) & 0xff;
	R8(DMA_SRC_ADDR_H) = (src_addr_int >> 16) & 0xff;
// sprintf(global_string_buff1, "src=%p, as written to vicky=%x, %x, %x", src, lo, med, hi);
// Buffer_NewMessage(global_string_buff1);

	// set destination address
	R8(DMA_DST_ADDR_L) = (dst_addr_int) & 0xff;
	R8(DMA_DST_ADDR_M) = (dst_addr_int >> 8) & 0xff;
	R8(DMA_DST_ADDR_H) = (dst_addr_int >> 16) & 0xff;

//sprintf(global_string_buff1, "src=%p, as written to vicky=%x, %x, %x", src, ((uint32_t)(src) >> (8*0)) & 0xff, ((uint32_t)(src) >> 8) & 0xff, ((uint32_t)(src) >> 16) & 0xff);
//sprintf(global_string_buff1, "src=%p, as written to vicky=%x, %x, %x", src, (src_addr_int >> (8*0)) & 0xff, (src_addr_int >> 8) & 0xff, (src_addr_int >> 16) & 0xff);
//sprintf(global_string_buff1, "src=%p, with pointer math=%x, %x, %x", src, src[0], src[1], src[2]);
// sprintf(global_string_buff1, "dst=%p, as written to vicky=%x, %x, %x", dst, lo, med, hi);
// Buffer_NewMessage(global_string_buff1);

	// set height and width of rectangle to copy
	R8(DMA_WIDTH_L) = (width) & 0xff;
	R8(DMA_WIDTH_M) = (width >> 8) & 0xff;
	R8(DMA_HEIGHT_L) = (height) & 0xff;
	R8(DMA_HEIGHT_M) = (height >> 8) & 0xff;
// sprintf(global_string_buff1, "h=%x, as written to vicky=%x, %x", height, lo, med);
// Buffer_NewMessage(global_string_buff1);
	
	// source stride and destination stride are the width of each bitmap, if copying to/from bitmaps.
	R8(DMA_SRC_STRIDE_L) = (src_stride) & 0xff;
	R8(DMA_SRC_STRIDE_M) = (src_stride >> 8) & 0xff;
	R8(DMA_DST_STRIDE_L) = (dst_stride) & 0xff;
	R8(DMA_DST_STRIDE_M) = (dst_stride >> 8) & 0xff;
// sprintf(global_string_buff1, "src stride=%x, as written to vicky=%x, %x", src_stride, lo, med);
// Buffer_NewMessage(global_string_buff1);
// sprintf(global_string_buff1, "dst stride=%x, as written to vicky=%x, %x", dst_stride, lo, med);
// Buffer_NewMessage(global_string_buff1);
	
	// flip the start flag, and wait for it to complete
	the_bits = R8(DMA_CTRL) | FLAG_DMA_CTRL_START;
	R8(DMA_CTRL) = the_bits;
	
	do
	{
		the_bits = R8(DMA_STATUS);
	} while (the_bits >= FLAG_DMA_STATUS_BUSY);
	
	// turn the DMA engine off
	R8(DMA_CTRL) = 0;
}


//! Fill a linear span of memory with a specified byte value
//! @param dst: the location the fill operation will start from
//! @param num_bytes: the number of bytes to be filled. 20-bit max on F256c, 24-bit max on F256e.
//! @param fill_value: The 8-bit value that will be used for the fill
void DMA_Fill(uint8_t* dst, uint32_t num_bytes, uint8_t fill_value)
{
	uint8_t		the_bits;
	uint32_t	dst_addr_int = (uint32_t)dst;
	
	// DMA copy only works with even widths
	if (num_bytes < 2)
	{
		return;
	}

	// make sure DMA engine is off
	R8(DMA_CTRL) = 0;
	
	// enable for 2D operation, with fill flag set
	R8(DMA_CTRL) = (FLAG_DMA_CTRL_ENABLE | FLAG_DMA_CTRL_FILL);
	
	// set the fill value
	R8(DMA_FILL_VALUE) = fill_value;

	// set destination address
	R8(DMA_DST_ADDR_L) = (dst_addr_int) & 0xff;
	R8(DMA_DST_ADDR_M) = (dst_addr_int >> 8) & 0xff;
	R8(DMA_DST_ADDR_H) = (dst_addr_int >> 16) & 0xff;

	// set num bytes to fill (20-bit)
	R8(DMA_COUNT_L) = (num_bytes) & 0xff;
	R8(DMA_COUNT_M) = (num_bytes >> 8) & 0xff;
	R8(DMA_COUNT_H) = (num_bytes >> 16) & 0xff;
		
	// flip the start flag, and wait for it to complete
	the_bits = R8(DMA_CTRL) | FLAG_DMA_CTRL_START;
	R8(DMA_CTRL) = the_bits;
	
	do
	{
		the_bits = R8(DMA_STATUS);
	} while (the_bits >= FLAG_DMA_STATUS_BUSY);
	
	// turn the DMA engine off
	R8(DMA_CTRL) = 0;
}


//! Fill a rectangular section of memory with a specified byte value
//! @param dst: the location the fill operation will start from
//! @param dst_stride: the "width" of the frame being filled. The width of the screen, for example.
//! @param width, height: the size of the rectangle to be filled, in bytes. 
//! @param fill_value: The 8-bit value that will be used for the fill
void DMA_FillRect(uint8_t* dst, uint16_t dst_stride, uint16_t width, uint16_t height, uint8_t fill_value)
{
	uint8_t		the_bits;
	uint32_t	dst_addr_int = (uint32_t)dst;
	
	// DMA copy only works with even widths
	if (width % 2)
	{
		width--;
	}
	
	// make sure DMA engine is off
	R8(DMA_CTRL) = 0;
	
	// enable for 2D operation, with fill flag set
	R8(DMA_CTRL) = (FLAG_DMA_CTRL_ENABLE | FLAG_DMA_CTRL_2D_OP | FLAG_DMA_CTRL_FILL);
	
	// set the fill value
	R8(DMA_FILL_VALUE) = fill_value;
	
	// set destination address
	R8(DMA_DST_ADDR_L) = (dst_addr_int) & 0xff;
	R8(DMA_DST_ADDR_M) = (dst_addr_int >> 8) & 0xff;
	R8(DMA_DST_ADDR_H) = (dst_addr_int >> 16) & 0xff;

	// set height and width of rectangle to copy
	R8(DMA_WIDTH_L) = (width) & 0xff;
	R8(DMA_WIDTH_M) = (width >> 8) & 0xff;
	R8(DMA_HEIGHT_L) = (height) & 0xff;
	R8(DMA_HEIGHT_M) = (height >> 8) & 0xff;
	
	// Destination stride is the width of the bitmap
	R8(DMA_DST_STRIDE_L) = (dst_stride) & 0xff;
	R8(DMA_DST_STRIDE_M) = (dst_stride >> 8) & 0xff;
	
	// flip the start flag, and wait for it to complete
	the_bits = R8(DMA_CTRL) | FLAG_DMA_CTRL_START;
	R8(DMA_CTRL) = the_bits;
	
	do
	{
		the_bits = R8(DMA_STATUS);
	} while (the_bits >= FLAG_DMA_STATUS_BUSY);
	
	// turn the DMA engine off
	R8(DMA_CTRL) = 0;
}



#endif

