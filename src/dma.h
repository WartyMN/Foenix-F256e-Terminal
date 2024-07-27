//! @file dma.h

/*
 * dma.h
 *
*  Created on: July 3, 2024
 *      Author: micahbly
 */
 
#ifndef DMA_H_
#define DMA_H_


/* about this class: DMA
 *
 * This provides functionality for accessing the VICKY's hardware DMA controller
 *
 *** things this class needs to be able to do
 * Fill a linear area of memory using DMA
 * Fill a 2D (rectangular) area of memory using DMA
 * Copy a linear area of memory using DMA
 * Copy a 2D area of memory using DMA
 *
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes

// C includes
#include <stdbool.h>

// Platform includes
#include "f256_e.h"


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/


/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/



/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/



/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

//! Copy a linear span of memory from one buffer to another, using the DMA engine
//! @param dst: the location data will be copied to
//! @param src: the location data will be copied from
//! @param num_bytes: the number of bytes to be copied. 20-bit max on F256c, 24-bit max on F256e.
void DMA_Copy(uint8_t* dst, uint8_t* src, uint32_t num_bytes);

//! Copy a rectangular section of memory from one buffer to another, using the DMA engine
//! @param dst: the location data will be copied to
//! @param dst_stride: the "width" of the frame being copied into. The width of the screen, for example.
//! @param src: the location data will be copied from
//! @param width, height: the size of the rectangle to be copied, in bytes. 
//! @param src_stride: the "width" of the frame being copied from. If you are copying from one place in the screen to another, this would be the screen width. If you are copying from an image buffer to the screen, this would be the width of the image.
void DMA_CopyRect(uint8_t* dst, uint16_t dst_stride, uint8_t* src, uint16_t width, uint16_t height, uint16_t src_stride);

//! Fill a linear span of memory with a specified byte value
//! @param dst: the location the fill operation will start from
//! @param num_bytes: the number of bytes to be filled. 20-bit max on F256c, 24-bit max on F256e.
//! @param fill_value: The 8-bit value that will be used for the fill
void DMA_Fill(uint8_t* dst, uint32_t num_bytes, uint8_t fill_value);

//! Fill a rectangular section of memory with a specified byte value
//! @param dst: the location the fill operation will start from
//! @param dst_stride: the "width" of the frame being filled. The width of the screen, for example.
//! @param width, height: the size of the rectangle to be filled, in bytes. 
//! @param fill_value: The 8-bit value that will be used for the fill
void DMA_FillRect(uint8_t* dst, uint16_t dst_stride, uint16_t width, uint16_t height, uint8_t fill_value);





#endif /* DMA_H_ */


