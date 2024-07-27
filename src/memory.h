/*
 * memory.h
 *
 *  Created on: December 4, 2022
 *      Author: micahbly
 */
 
// Adapted from F256jr/k version of FileManager starting June 2, 2024

#ifndef MEMORY_H_
#define MEMORY_H_




/* about this class
 *
 * this header represents a set of assembly functions in memory.asm
 * the functions in this header file are all related to moving memory between physical F256jr RAM and MMU-mapped 6502 RAM
 * these functions neeed to be in the MAIN segment so they are always available
 * all functions that modify the LUT will reset it to its original configurate before exiting
 *
 */

/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

#include "app.h"
#include <stdint.h>


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define PARAM_FOR_ATTR_MEM	true	// param for functions updating VICKY screen memory: make it affect color/attribute memory
#define PARAM_FOR_CHAR_MEM	false	// param for functions updating VICKY screen memory: make it affect character memory

#define ZP_UART_WRITE_IDX	0x00					// zero-page address holding 2-byte index to UART circular buffer's WRITE head
#define ZP_UART_READ_IDX	(ZP_UART_WRITE_IDX + 2)	// zero-page address holding 2-byte index to UART circular buffer's READ head
#define ZP_TEMP_1			(ZP_UART_READ_IDX + 2)	// zero-page address we will use for temp variable storage in assembly routines
#define ZP_OTHER_PARAM		(ZP_TEMP_1 + 1)			// zero-page address we will use for communicating 1 byte to/from assembly routines
#define ZP_X				(ZP_OTHER_PARAM + 1)	// zero-page address (2 bytes) we will use for passing X coordinate to assembly routines)
#define ZP_Y				(ZP_X + 2)				// zero-page address (2 bytes)we will use for passing Y coordinate to assembly routines


// starting point for all storage to extended memory. if larger than 8K, increment as necessary
#define VRAM_START_ADDR						0x40000		// when copying file data to EM, the starting physical address (20 bit)
#define VRAM_SIZE							(320*240)	// 76800 bytes = 75k
#define VRAM_END_ADDR						0x52c00		// preprocess math is apparently 16 bit not 24 bit (VRAM_START_ADDR + VRAM_SIZE)	// more than one bank is used!
#define VRAM_PREVIEW_IMG_START_ADDR			0x428bc		// (VRAM_START_ADDR + (320*32 + 188 = 10428)

#define UART_BUFFER_START_ADDR				0x7E000		// hard-coded address of UART buffer. See f256-term.scm. 
#define UART_BUFFER_END_ADDR				0x7FFFF		// 
#define UART_BUFFER_SIZE					((UART_BUFFER_END_ADDR - UART_BUFFER_START_ADDR) + 1)	// 8k
#define UART_BUFFER_MASK					(UART_BUFFER_SIZE - 1)


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

// // call to a routine in memory.asm that modifies the MMU LUT to bring the specified bank of physical memory into the CPU's RAM space
// // set zp_bank_num before calling.
// // returns the slot that had been mapped previously
// uint8_t __fastcall__ Memory_SwapInNewBank(uint8_t the_bank_slot);
// 
// // call to a routine in memory.asm that modifies the MMU LUT to bring the back the previously specified bank of physical memory into the CPU's RAM space
// // relies on a previous routine having set ZP_OLD_BANK_NUM. Should be called after Memory_SwapInNewBank(), when finished with the new bank
// // set zp_bank_num before calling.
// void __fastcall__ Memory_RestorePreviousBank(uint8_t the_bank_slot);
// 
// // call to a routine in memory.asm that returns whatever is currently mapped in the specified MMU slot
// // set zp_bank_num before calling.
// // returns the slot that had been mapped previously
// uint8_t __fastcall__ Memory_GetMappedBankNum(void);
// 
// call to a routine in memory.asm that writes an illegal opcode followed by address of debug buffer
// that is a simple to the f256jr emulator to write the string at the debug buffer out to the console
//void __fastcall__ Memory_DebugOut(void);

// call to a routine in memory.asm that copies specified number of bytes from src to dst
// set zp_to_addr, zp_from_addr, zp_copy_len before calling.
// credit: http://6502.org/source/general/memory_move.html
// void __fastcall__ Memory_Copy(void);

// call to a routine in memory.asm that copies specified number of bytes from src to dst
// set zp_to_addr, zp_from_addr, zp_copy_len before calling.
// this version uses the F256's DMA capabilities to copy, so addresses can be 24 bit (system memory, not CPU memory)
// in other words, no need to page either dst or src into CPU space
//void __fastcall__ Memory_CopyWithDMA(void);

// call to a routine in memory.asm that fills the specified number of bytes to the dst
// set zp_to_addr, zp_copy_len to num bytes to fill, and zp_other_byte to the fill value before calling.
// this version uses the F256's DMA capabilities to fill, so addresses can be 24 bit (system memory, not CPU memory)
// in other words, no need to page either dst into CPU space
//void __fastcall__ Memory_FillWithDMA(void);

#if defined _DMAA_
	// call to a routine in memory.asm that copies a specific "rectangle" of data from one buffer to another (think blit an image to a bitmap)
	// set zp_to_addr, zp_from_addr, zp_width, zp_height, zp_src_stride, zp_dst_stride before calling.
	// this version uses the F256's DMA capabilities to copy, so addresses can be 24 bit (system memory, not CPU memory)
	// in other words, no need to page either dst or src into CPU space
	void Memory_CopyRectWithDMA(void);
#endif

//void Memory_ReadFromUART(void);


#endif /* MEMORY_H_ */
