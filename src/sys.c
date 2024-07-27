/*
 * lib_sys.c
 *
 *  Created on: Mar 22, 2022
 *      Author: micahbly
 */


// THIS IS A CUT-DOWN VERSION OF the OS/f lib_sys.c file, just enough to power Lich King
// adapted for Foenix F256 Jr starting November 29, 2022


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "sys.h"
#include "app.h"
#include "comm_buffer.h"
#include "debug.h"
#include "event.h"
#include "memory.h"
#include "text.h"

// C includes
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// F256 includes
#include "f256_e.h"


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/


/*****************************************************************************/
/*                           File-scope Variables                            */
/*****************************************************************************/


// F256JR/K colors, used for both fore- and background colors in Text mode
// in C256 & F256, these are 8 bit values; in A2560s, they are 32 bit values, and endianness matters
static uint8_t standard_text_color_lut[64] = 
{
	0x00, 0x00, 0x00, 0x00,
	0x66, 0x66, 0x66, 0x00,
	0xAA, 0x00, 0x00, 0x00,
	0x00, 0xAA, 0x00, 0x00,
	0xEA, 0x41, 0xC0, 0x00,
	0x00, 0x48, 0x87, 0x00,
	0x00, 0x9C, 0xFF, 0x00,
	0xFF, 0xDB, 0x57, 0x00,
	0x28, 0x3F, 0x3F, 0x00,
	0x8A, 0xAA, 0xAA, 0x00,
	0xFF, 0x55, 0x55, 0x00,
	0x55, 0xFF, 0x55, 0x00,
	0xED, 0x8D, 0xFF, 0x00,
	0x00, 0x00, 0xFF, 0x00,			
	0x55, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF, 0xFF, 0x00
};

static System		system_storage;


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

System*				global_system = &system_storage;

extern char*		global_string_buff1;

// shared system bitmap color table
// any code using the system is allowed to overwrite this
// having a global clut makes up for not being able to read palette data out of the actual CLUTs, which are write-only inside the VICKY
// the default system CLUT here is a slightly modified apple 256 color CLUT. On Foenix machines, first color is always transparent. 
// in apple CLUT, first was white. Moved white to slot 256-12, replacing a very dark blue. that gives a run from white->gray->black at the end of the CLUT. 
// this CLUT has also been reversed from RGBA to VICKY's BGRA order.
uint8_t global_system_clut[] = 
{
	0x00,0x00,0x00,0x00, 0xCC,0xFF,0xFF,0x00, 0x99,0xFF,0xFF,0x00, 0x66,0xFF,0xFF,0x00, 
	0x33,0xFF,0xFF,0x00, 0x00,0xFF,0xFF,0x00, 0xFF,0xCC,0xFF,0x00, 0xCC,0xCC,0xFF,0x00, 
	0x99,0xCC,0xFF,0x00, 0x66,0xCC,0xFF,0x00, 0x33,0xCC,0xFF,0x00, 0x00,0xCC,0xFF,0x00, 
	0xFF,0x99,0xFF,0x00, 0xCC,0x99,0xFF,0x00, 0x99,0x99,0xFF,0x00, 0x66,0x99,0xFF,0x00, 
	0x33,0x99,0xFF,0x00, 0x00,0x99,0xFF,0x00, 0xFF,0x66,0xFF,0x00, 0xCC,0x66,0xFF,0x00, 
	0x99,0x66,0xFF,0x00, 0x66,0x66,0xFF,0x00, 0x33,0x66,0xFF,0x00, 0x00,0x66,0xFF,0x00, 
	0xFF,0x33,0xFF,0x00, 0xCC,0x33,0xFF,0x00, 0x99,0x33,0xFF,0x00, 0x66,0x33,0xFF,0x00, 
	0x33,0x33,0xFF,0x00, 0x00,0x33,0xFF,0x00, 0xFF,0x00,0xFF,0x00, 0xCC,0x00,0xFF,0x00, 
	0x99,0x00,0xFF,0x00, 0x66,0x00,0xFF,0x00, 0x33,0x00,0xFF,0x00, 0x00,0x00,0xFF,0x00, 
	0xFF,0xFF,0xCC,0x00, 0xCC,0xFF,0xCC,0x00, 0x99,0xFF,0xCC,0x00, 0x66,0xFF,0xCC,0x00, 
	0x33,0xFF,0xCC,0x00, 0x00,0xFF,0xCC,0x00, 0xFF,0xCC,0xCC,0x00, 0xCC,0xCC,0xCC,0x00, 
	0x99,0xCC,0xCC,0x00, 0x66,0xCC,0xCC,0x00, 0x33,0xCC,0xCC,0x00, 0x00,0xCC,0xCC,0x00, 
	0xFF,0x99,0xCC,0x00, 0xCC,0x99,0xCC,0x00, 0x99,0x99,0xCC,0x00, 0x66,0x99,0xCC,0x00, 
	0x33,0x99,0xCC,0x00, 0x00,0x99,0xCC,0x00, 0xFF,0x66,0xCC,0x00, 0xCC,0x66,0xCC,0x00, 
	0x99,0x66,0xCC,0x00, 0x66,0x66,0xCC,0x00, 0x33,0x66,0xCC,0x00, 0x00,0x66,0xCC,0x00, 
	0xFF,0x33,0xCC,0x00, 0xCC,0x33,0xCC,0x00, 0x99,0x33,0xCC,0x00, 0x66,0x33,0xCC,0x00, 
	0x33,0x33,0xCC,0x00, 0x00,0x33,0xCC,0x00, 0xFF,0x00,0xCC,0x00, 0xCC,0x00,0xCC,0x00, 
	0x99,0x00,0xCC,0x00, 0x66,0x00,0xCC,0x00, 0x33,0x00,0xCC,0x00, 0x00,0x00,0xCC,0x00, 
	0xFF,0xFF,0x99,0x00, 0xCC,0xFF,0x99,0x00, 0x99,0xFF,0x99,0x00, 0x66,0xFF,0x99,0x00, 
	0x33,0xFF,0x99,0x00, 0x00,0xFF,0x99,0x00, 0xFF,0xCC,0x99,0x00, 0xCC,0xCC,0x99,0x00, 
	0x99,0xCC,0x99,0x00, 0x66,0xCC,0x99,0x00, 0x33,0xCC,0x99,0x00, 0x00,0xCC,0x99,0x00, 
	0xFF,0x99,0x99,0x00, 0xCC,0x99,0x99,0x00, 0x99,0x99,0x99,0x00, 0x66,0x99,0x99,0x00, 
	0x33,0x99,0x99,0x00, 0x00,0x99,0x99,0x00, 0xFF,0x66,0x99,0x00, 0xCC,0x66,0x99,0x00, 
	0x99,0x66,0x99,0x00, 0x66,0x66,0x99,0x00, 0x33,0x66,0x99,0x00, 0x00,0x66,0x99,0x00, 
	0xFF,0x33,0x99,0x00, 0xCC,0x33,0x99,0x00, 0x99,0x33,0x99,0x00, 0x66,0x33,0x99,0x00, 
	0x33,0x33,0x99,0x00, 0x00,0x33,0x99,0x00, 0xFF,0x00,0x99,0x00, 0xCC,0x00,0x99,0x00, 
	0x99,0x00,0x99,0x00, 0x66,0x00,0x99,0x00, 0x33,0x00,0x99,0x00, 0x00,0x00,0x99,0x00, 
	0xFF,0xFF,0x66,0x00, 0xCC,0xFF,0x66,0x00, 0x99,0xFF,0x66,0x00, 0x66,0xFF,0x66,0x00, 
	0x33,0xFF,0x66,0x00, 0x00,0xFF,0x66,0x00, 0xFF,0xCC,0x66,0x00, 0xCC,0xCC,0x66,0x00, 
	0x99,0xCC,0x66,0x00, 0x66,0xCC,0x66,0x00, 0x33,0xCC,0x66,0x00, 0x00,0xCC,0x66,0x00, 
	0xFF,0x99,0x66,0x00, 0xCC,0x99,0x66,0x00, 0x99,0x99,0x66,0x00, 0x66,0x99,0x66,0x00, 
	0x33,0x99,0x66,0x00, 0x00,0x99,0x66,0x00, 0xFF,0x66,0x66,0x00, 0xCC,0x66,0x66,0x00, 
	0x99,0x66,0x66,0x00, 0x66,0x66,0x66,0x00, 0x33,0x66,0x66,0x00, 0x00,0x66,0x66,0x00, 
	0xFF,0x33,0x66,0x00, 0xCC,0x33,0x66,0x00, 0x99,0x33,0x66,0x00, 0x66,0x33,0x66,0x00, 
	0x33,0x33,0x66,0x00, 0x00,0x33,0x66,0x00, 0xFF,0x00,0x66,0x00, 0xCC,0x00,0x66,0x00, 
	0x99,0x00,0x66,0x00, 0x66,0x00,0x66,0x00, 0x33,0x00,0x66,0x00, 0x00,0x00,0x66,0x00, 
	0xFF,0xFF,0x33,0x00, 0xCC,0xFF,0x33,0x00, 0x99,0xFF,0x33,0x00, 0x66,0xFF,0x33,0x00, 
	0x33,0xFF,0x33,0x00, 0x00,0xFF,0x33,0x00, 0xFF,0xCC,0x33,0x00, 0xCC,0xCC,0x33,0x00, 
	0x99,0xCC,0x33,0x00, 0x66,0xCC,0x33,0x00, 0x33,0xCC,0x33,0x00, 0x00,0xCC,0x33,0x00, 
	0xFF,0x99,0x33,0x00, 0xCC,0x99,0x33,0x00, 0x99,0x99,0x33,0x00, 0x66,0x99,0x33,0x00, 
	0x33,0x99,0x33,0x00, 0x00,0x99,0x33,0x00, 0xFF,0x66,0x33,0x00, 0xCC,0x66,0x33,0x00, 
	0x99,0x66,0x33,0x00, 0x66,0x66,0x33,0x00, 0x33,0x66,0x33,0x00, 0x00,0x66,0x33,0x00, 
	0xFF,0x33,0x33,0x00, 0xCC,0x33,0x33,0x00, 0x99,0x33,0x33,0x00, 0x66,0x33,0x33,0x00, 
	0x33,0x33,0x33,0x00, 0x00,0x33,0x33,0x00, 0xFF,0x00,0x33,0x00, 0xCC,0x00,0x33,0x00, 
	0x99,0x00,0x33,0x00, 0x66,0x00,0x33,0x00, 0x33,0x00,0x33,0x00, 0x00,0x00,0x33,0x00, 
	0xFF,0xFF,0x00,0x00, 0xCC,0xFF,0x00,0x00, 0x99,0xFF,0x00,0x00, 0x66,0xFF,0x00,0x00, 
	0x33,0xFF,0x00,0x00, 0x00,0xFF,0x00,0x00, 0xFF,0xCC,0x00,0x00, 0xCC,0xCC,0x00,0x00, 
	0x99,0xCC,0x00,0x00, 0x66,0xCC,0x00,0x00, 0x33,0xCC,0x00,0x00, 0x00,0xCC,0x00,0x00, 
	0xFF,0x99,0x00,0x00, 0xCC,0x99,0x00,0x00, 0x99,0x99,0x00,0x00, 0x66,0x99,0x00,0x00, 
	0x33,0x99,0x00,0x00, 0x00,0x99,0x00,0x00, 0xFF,0x66,0x00,0x00, 0xCC,0x66,0x00,0x00, 
	0x99,0x66,0x00,0x00, 0x66,0x66,0x00,0x00, 0x33,0x66,0x00,0x00, 0x00,0x66,0x00,0x00, 
	0xFF,0x33,0x00,0x00, 0xCC,0x33,0x00,0x00, 0x99,0x33,0x00,0x00, 0x66,0x33,0x00,0x00, 
	0x33,0x33,0x00,0x00, 0x00,0x33,0x00,0x00, 0xFF,0x00,0x00,0x00, 0xCC,0x00,0x00,0x00, 
	0x99,0x00,0x00,0x00, 0x66,0x00,0x00,0x00, 0x33,0x00,0x00,0x00, 0x00,0x00,0xEE,0x00, 
	0x00,0x00,0xDD,0x00, 0x00,0x00,0xBB,0x00, 0x00,0x00,0xAA,0x00, 0x00,0x00,0x88,0x00, 
	0x00,0x00,0x77,0x00, 0x00,0x00,0x55,0x00, 0x00,0x00,0x44,0x00, 0x00,0x00,0x22,0x00, 
	0x00,0x00,0x11,0x00, 0x00,0xEE,0x00,0x00, 0x00,0xDD,0x00,0x00, 0x00,0xBB,0x00,0x00, 
	0x00,0xAA,0x00,0x00, 0x00,0x88,0x00,0x00, 0x00,0x77,0x00,0x00, 0x00,0x55,0x00,0x00, 
	0x00,0x44,0x00,0x00, 0x00,0x22,0x00,0x00, 0x00,0x11,0x00,0x00, 0xEE,0x00,0x00,0x00, 
	0xDD,0x00,0x00,0x00, 0xBB,0x00,0x00,0x00, 0xAA,0x00,0x00,0x00, 0x88,0x00,0x00,0x00, 
	0x77,0x00,0x00,0x00, 0x55,0x00,0x00,0x00, 0x44,0x00,0x00,0x00, 0x22,0x00,0x00,0x00, 
	0xFF,0xFF,0xFF,0x00, 0xEE,0xEE,0xEE,0x00, 0xDD,0xDD,0xDD,0x00, 0xBB,0xBB,0xBB,0x00, 
	0xAA,0xAA,0xAA,0x00, 0x88,0x88,0x88,0x00, 0x77,0x77,0x77,0x00, 0x55,0x55,0x55,0x00, 
	0x44,0x44,0x44,0x00, 0x22,0x22,0x22,0x00, 0x11,0x11,0x11,0x00, 0x00,0x00,0x00,0x00, 
};



/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// enable or disable the gamma correction 
void Sys_SetGammaMode(bool enable_it);

// convert a decimal number to BCD format (for use with RTC)
uint8_t Sys_DecimalToBCD(uint8_t dec_number);



/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// enable or disable the gamma correction 
void Sys_SetGammaMode(bool enable_it)
{
	uint8_t		the_gamma_mode_bits = R8(VICKY_GAMMA_CTRL_REG);
	uint8_t		new_mode_flag;

	// LOGIC:
	//   both C256s and A2560s have a gamma correction mode
	//   It needs to be hardware enabled by turning DIP switch 7 on the motherboard to ON
	//     bit 6 (0x40) of the vicky master control turns Gamma correction on and off
	
	if (enable_it)
	{
		new_mode_flag = 0xFF;
	}
	else
	{
		new_mode_flag = 0x00;
	}

	//DEBUG_OUT(("%s %d: vicky byte 3 before gamma change = %x", __func__, __LINE__, the_gamma_mode_bits));
	//the_gamma_mode_bits |= (GAMMA_MODE_ONOFF_BITS & new_mode_flag);
	
	//the_gamma_mode_bits = (the_gamma_mode_bits & ~((uint8_t)1 << 6)) | ((uint8_t)enable_it << 6);
	the_gamma_mode_bits  = (the_gamma_mode_bits & ~GAMMA_MODE_ONOFF_BITS) | (new_mode_flag & GAMMA_MODE_ONOFF_BITS);
	
	R8(VICKY_GAMMA_CTRL_REG) = the_gamma_mode_bits;
		
	//DEBUG_OUT(("%s %d: vicky byte 3 after gamma change = %x, %x", __func__, __LINE__, the_gamma_mode_bits, R8(VICKY_GAMMA_CTRL_REG)));
	//DEBUG_OUT(("%s %d: wrote to %x to register at %p", __func__, __LINE__, the_gamma_mode_bits, P8(VICKY_GAMMA_CTRL_REG)));
}


// enable or disable double height/width pixels
void Sys_SetTextPixelHeight(bool double_x, bool double_y)
{
	uint8_t		text_mode_flags = R8(VICKY_MASTER_CTRL_REG_H) & VICKY_RES_FON_SET;
	
	if (double_x)
	{
		text_mode_flags |= VICKY_RES_X_DOUBLER_FLAG;
	}

	if (double_y)
	{
		text_mode_flags |= VICKY_RES_Y_DOUBLER_FLAG;
	}
	
	R8(VICKY_MASTER_CTRL_REG_H) = text_mode_flags;
}


//! Switch machine into graphics mode, text mode, sprite mode, etc.
//! Use PARAM_SPRITES_ON/OFF, PARAM_BITMAP_ON/OFF, PARAM_TILES_ON/OFF, PARAM_TEXT_OVERLAY_ON/OFF, PARAM_TEXT_ON/OFF
void Sys_SetGraphicMode(bool enable_sprites, bool enable_bitmaps, bool enable_tiles, bool enable_text_overlay, bool enable_text)
{	
	uint8_t		the_bits;
	
	// LOGIC:
	//   clears everything but the gamma mode
	//   then re-enables only those modes specified

	the_bits = R8(VICKY_MASTER_CTRL_REG_L) & GAMMA_MODE_ONOFF_BITS;	

	if (enable_sprites | enable_bitmaps | enable_tiles | enable_text_overlay)
	{
		the_bits |= GRAPHICS_MODE_GRAPHICS;
	}
	
	if (enable_sprites)
	{
		the_bits |= GRAPHICS_MODE_EN_SPRITE;
	}

	if (enable_bitmaps)
	{
		the_bits |= GRAPHICS_MODE_EN_BITMAP;
		// enable bitmap layers 0, 1; disable layer 2
		R8(VICKY_LAYER_CTRL_1) = 0xFF;
		R8(VICKY_LAYER_CTRL_2) = 0x00;
	}

	if (enable_tiles)
	{
		the_bits |= GRAPHICS_MODE_EN_TILE;
	}

	if (enable_text)
	{
		the_bits |= GRAPHICS_MODE_TEXT;
		// disable bitmap layers 0, 1, 2
		R8(VICKY_LAYER_CTRL_1) = 0x00;
		R8(VICKY_LAYER_CTRL_2) = 0x00;
	}

	if (enable_text_overlay)
	{
		the_bits |= GRAPHICS_MODE_TEXT;
		the_bits |= GRAPHICS_MODE_TEXT_OVER;
		the_bits |= GRAPHICS_MODE_GRAPHICS;
	}

	// switch to graphics mode by setting graphics mode bit, and setting bitmap engine enable bit
	R8(VICKY_MASTER_CTRL_REG_L) = (the_bits);

	return;
}


//! Switch machine into text mode
//! @param as_overlay: If true, sets text overlay mode (text over graphics). If false, sets full text mode (no graphics);
void Sys_SetModeText(bool as_overlay)
{	
	// LOGIC:
	//   

	Sys_SetGraphicMode(as_overlay, as_overlay, as_overlay, as_overlay, !as_overlay);
	
// 	if (as_overlay)
// 	{
// 		
// 		// switch to text mode with overlay by setting graphics mode bit, setting bitmap engine enable bit, and setting graphics mode overlay		
// 		R8(VICKY_MASTER_CTRL_REG_L) = (GRAPHICS_MODE_TEXT | GRAPHICS_MODE_TEXT_OVER | GRAPHICS_MODE_GRAPHICS | GRAPHICS_MODE_EN_SPRITE | GRAPHICS_MODE_EN_BITMAP);
// 		// enable bitmap layers 0, 1; disable layer 2
// 		R8(VICKY_LAYER_CTRL_1) = 0xFF;
// 		R8(VICKY_LAYER_CTRL_2) = 0x00;
// 		
// 		// c256foenix, discord 2022/03/10
// 		// Normally, for example, if you setup everything to be in bitmap mode, and you download an image in VRAM and you can see it properly... If you turn on overlay, then you will see on top of that same image, your text that you had before.
// 		// Mstr_Ctrl_Text_Mode_En  = $01       ; Enable the Text Mode
// 		// Mstr_Ctrl_Text_Overlay  = $02       ; Enable the Overlay of the text mode on top of Graphic Mode (the Background Color is ignored)
// 		// Mstr_Ctrl_Graph_Mode_En = $04       ; Enable the Graphic Mode
// 		// Mstr_Ctrl_Bitmap_En     = $08       ; Enable the Bitmap Module In Vicky
// 		// all of these should be ON
// 	}
// 	else
// 	{
// 		R8(VICKY_MASTER_CTRL_REG_L) = (GRAPHICS_MODE_TEXT);
// 		// disable bitmap layers 0, 1, and 2
// 		R8(VICKY_LAYER_CTRL_1) = 0x00;
// 		R8(VICKY_LAYER_CTRL_2) = 0x00;
// 	}
	
	return;
}


// convert a decimal number to BCD format (for use with RTC)
uint8_t Sys_DecimalToBCD(uint8_t dec_number)
{
	uint8_t		bcd_number = 0;
	
	while (dec_number >= 10)
	{
		++bcd_number;
		dec_number -= 10;
	}
	
	bcd_number = bcd_number << 4;
	
	return (bcd_number | dec_number);
}




// // interrupt 1 is PS2 keyboard, interrupt 2 is A2560K keyboard
// void Sys_InterruptKeyboard(void)
// {
// 	kbd_handle_irq();
// }

// 
// // interrupt 4 is PS2 mouse
// void Sys_InterruptMouse(void);


// **** Debug functions *****

// void Sys_Print(System* the_system)
// {
// 	DEBUG_OUT(("System print out:"));
// 	DEBUG_OUT(("  address: %p", 			the_system));
// 	DEBUG_OUT(("  num_screens_: %i",		the_system->num_screens_));
// 	DEBUG_OUT(("  model_number_: %i",		the_system->model_number_));
// }


// void Sys_PrintScreen(Screen* the_screen)
// {
// 	DEBUG_OUT(("Screen print out:"));
// 	DEBUG_OUT(("  address: %p", 			the_screen));
// 	DEBUG_OUT(("  id_: %i", 				the_screen->id_));
// 	DEBUG_OUT(("  vicky_: %p", 				the_screen->vicky_));
// 	DEBUG_OUT(("  width_: %i", 				the_screen->width_));
// 	DEBUG_OUT(("  height_: %i", 			the_screen->height_));
// 	DEBUG_OUT(("  text_cols_vis_: %i", 		the_screen->text_cols_vis_));
// 	DEBUG_OUT(("  text_rows_vis_: %i", 		the_screen->text_rows_vis_));
// 	DEBUG_OUT(("  text_mem_cols_: %i", 		the_screen->text_mem_cols_));
// 	DEBUG_OUT(("  text_mem_rows_: %i", 		the_screen->text_mem_rows_));
// 	DEBUG_OUT(("  text_ram_: %p", 			the_screen->text_ram_));
// 	DEBUG_OUT(("  text_attr_ram_: %p", 		the_screen->text_attr_ram_));
// 	DEBUG_OUT(("  text_font_ram_: %p", 		the_screen->text_font_ram_));
// 	DEBUG_OUT(("  bitmap_[0]: %p", 			the_screen->bitmap_[0]));
// 	DEBUG_OUT(("  bitmap_[1]: %p", 			the_screen->bitmap_[1]));
// 	DEBUG_OUT(("  text_font_height_: %i",	the_screen->text_font_height_));
// 	DEBUG_OUT(("  text_font_width_: %i",	the_screen->text_font_width_));
// }



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// **** CONSTRUCTOR AND DESTRUCTOR *****





// **** System Initialization functions *****


//! Initialize the system (primary entry point for all system initialization activity)
//! Starts up the memory manager, creates the global system object, runs autoconfigure to check the system hardware, loads system and application fonts, allocates a bitmap for the screen.
bool Sys_InitSystem(void)
{	
	Bitmap*		the_bitmap;

	// open log file, if on real hardware, and built with calypsi, and debugging flags were passed
	#if defined LOG_LEVEL_1 || defined LOG_LEVEL_2 || defined LOG_LEVEL_3 || defined LOG_LEVEL_4 || defined LOG_LEVEL_5
		if (General_LogInitialize() == false)
		{
			printf("%s %d: failed to open log file for writing \n", __func__, __LINE__);
		}
	#endif
	
	//DEBUG_OUT(("%s %d: Initializing System...", __func__, __LINE__));
	
	// check what kind of hardware the system is running on
	// LOGIC: we need to know how many screens it has before allocating screen objects
	if (Sys_AutoDetectMachine() == false)
	{
		LOG_ERR(("%s %d: Detected machine hardware is incompatible with this software", __func__ , __LINE__));
		return false;
	}
	
	//DEBUG_OUT(("%s %d: Hardware detected. Running Autoconfigure...", __func__, __LINE__));
	
	if (Sys_AutoConfigure() == false)
	{
		LOG_ERR(("%s %d: Auto configure failed", __func__, __LINE__));
		return false;
	}

// 	// clear 0x0200, 0201, 0202, and 0203 to make next start after reset more accurate
// 	// (if started from flash, then from disk, then reset, the "- fm" would still be in memory otherwise)
// 	memset((char*)0x0200, 0, 4);

	// Enable mouse pointer -- no idea if this works, f68 emulator doesn't support mouse yet. 
	//R32(VICKYB_MOUSE_CTRL_A2560K) = 1;
	
	// allocate the foreground and background bitmaps, then assign them fixed locations in VRAM
	
	// LOGIC: 
	//   The only bitmaps we want pointing to VRAM locations are the system's layer0 and layer1 bitmaps for the screen
	//   We assign them fixed spaces in VRAM, 800*600 apart, so that the addresses are good even on a screen resolution change. 
	

// 	if ( (the_bitmap = Bitmap_New(global_system->screen_width_, global_system->screen_height_, PARAM_IN_VRAM)) == NULL)
	if ( (the_bitmap = Bitmap_New(VICKY_BITMAP_MAX_H_RES, VICKY_BITMAP_MAX_V_RES, PARAM_IN_VRAM)) == NULL)
	{
		LOG_ERR(("%s %d: Failed to create bitmap", __func__, __LINE__));
		return false;
	}

	//DEBUG_OUT(("%s %d: bitmap addr_int_=%lx, addr_=%p", __func__, __LINE__, the_bitmap->addr_int_, the_bitmap->addr_));
	
	//Sys_SetScreenBitmap(global_system, the_bitmap, i);
	global_system->bitmap_ = the_bitmap;
	R8(BITMAP_L0_VRAM_ADDR_L) = (the_bitmap->addr_int_) & 0xff;
	R8(BITMAP_L0_VRAM_ADDR_M) = (the_bitmap->addr_int_ >> (8*1)) & 0xff;
	R8(BITMAP_L0_VRAM_ADDR_H) = (the_bitmap->addr_int_ >> (8*2)) & 0xff;

	// copy clut into vicky space
	memcpy((uint8_t*)VICKY_CLUT0, global_system_clut, 0x400);
		
	// clear the bitmap
	Bitmap_FillMemory(the_bitmap, 0xFF);	// 00 is black, but there's the transparency. FF is also all-black in the default palette

	//DEBUG_OUT(("%s %d: System initialization complete.", __func__, __LINE__));

	return true;
}


//! Find out what kind of machine the software is running on, and determine # of screens available
//! @param	the_system: valid pointer to system object
//! @return	Returns false if the machine is known to be incompatible with this software. 
bool Sys_AutoDetectMachine(void)
{
	uint8_t	the_machine_id;
	
	the_machine_id = (R8(MACHINE_ID_REGISTER) & MACHINE_MODEL_MASK);	

	global_system->model_number_ = the_machine_id;
// 	DEBUG_OUT(("%s %d: global_system->model_number_=%u", __func__, __LINE__, global_system->model_number_));
	
	return true;
}


//! Find out what kind of machine the software is running on, and configure the passed screen accordingly
//! Configures screen settings, RAM addresses, etc. based on known info about machine types
//! Configures screen width, height, total text rows and cols, and visible text rows and cols by checking hardware
//! @param	the_system: valid pointer to system object
//! @return	Returns false if the machine is known to be incompatible with this software. 
bool Sys_AutoConfigure(void)
{
	// always enable gamma correction, regardless of which machine it is
	Sys_SetGammaMode(true);

// 	sprintf(global_string_buff1, "global_system->model_number_=%u", global_system->model_number_);
// 	Buffer_NewMessage(global_string_buff1);
// 	General_DelayTicks(40000);

	// LOGIC: this code supports all the 65c02/65816 F256s, and no others. No FMX, No A2560K, etc.

	switch (global_system->model_number_)
	{
		case MACHINE_F256K:
		case MACHINE_F256K2:
		case MACHINE_F256KE:
		case MACHINE_F256K2E:
			// if machine with built-in keyboard, need to initialize it.
			#ifdef _F256K_
				// get the built-in keyboard on the F256K ready
				Event_InitalizeKeyboard256K();
			#endif
		case MACHINE_F256JR:
		case MACHINE_F256JRE:
			
// 			//DEBUG_OUT(("%s %d: Configuring screens for an F256jr/k/k2 (1 screen)", __func__, __LINE__));
// 			global_system->screen_[ID_CHANNEL_A]->vicky_ = P32(VICKY_C256);
// 			global_system->screen_[ID_CHANNEL_A]->text_ram_ = TEXT_RAM_C256;
// 			global_system->screen_[ID_CHANNEL_A]->text_attr_ram_ = TEXT_ATTR_C256;
// 			global_system->screen_[ID_CHANNEL_A]->text_font_ram_ = FONT_MEMORY_BANK_C256;
// 			global_system->screen_[ID_CHANNEL_A]->text_color_fore_ram_ = (char*)TEXT_FORE_LUT_C256;
// 			global_system->screen_[ID_CHANNEL_A]->text_color_back_ram_ = (char*)TEXT_BACK_LUT_C256;
		
			// use auto configure to set resolution, text cols, margins, etc
			if (Sys_DetectScreenSize() == false)
			{
				LOG_ERR(("%s %d: Unable to auto-configure screen resolution", __func__, __LINE__));
				return false;
			}
	
			// set standard color LUTs for text mode
			memcpy((uint8_t*)(TEXT_FORE_LUT), &standard_text_color_lut, 64);
			memcpy((uint8_t*)(TEXT_BACK_LUT), &standard_text_color_lut, 64);
			
// 			DEBUG_OUT(("%s %d: This screen has %i x %i text (%i x %i visible)", __func__, __LINE__, 
// 				global_system->text_mem_cols_, 
// 				global_system->text_mem_rows_, 
// 				global_system->text_cols_vis_, 
// 				global_system->text_rows_vis_
// 				));

			break;
			
		default:
			//DEBUG_OUT(("%s %d: this system %i not supported!", __func__, __LINE__, global_system->model_number_));
			return false;			
	}
	
	return true;
}






// **** Event-handling functions *****

// see MCP's ps2.c for real examples once real machine available

// // interrupt 1 is PS2 keyboard, interrupt 2 is A2560K keyboard
// void Sys_InterruptKeyboard(void)
// {
// 	printf("keyboard!\n");
// 	return;
// }
// 
// // interrupt 4 is PS2 mouse
// void Sys_InterruptMouse(void)
// {
// 	printf("mouse!\n");
// 	return;
// }




// **** Screen mode/resolution/size functions *****



// //! Switch machine into text mode
// //! @param	the_system: valid pointer to system object
// //! @param as_overlay: If true, sets text overlay mode (text over graphics). If false, sets full text mode (no graphics);
// void Sys_SetModeText(bool as_overlay)
// {	
// 	// LOGIC:
// 	//   On an A2560K or X, the only screen that has a text/graphics mode is the Channel B screen
// 	
// 	Sys_SwapIOPage(VICKY_IO_PAGE_REGISTERS);
// 	
// 	if (as_overlay)
// 	{
// 		// switch to text mode with overlay by setting graphics mode bit, setting bitmap engine enable bit, and setting graphics mode overlay		
// 		R8(VICKY_MASTER_CTRL_REG_L) = (GRAPHICS_MODE_TEXT | GRAPHICS_MODE_TEXT_OVER | GRAPHICS_MODE_GRAPHICS | GRAPHICS_MODE_EN_BITMAP);
// 		R8(BITMAP_CTRL) = 0x01;
// 		
// 		// c256foenix, discord 2022/03/10
// 		// Normally, for example, if you setup everything to be in bitmap mode, and you download an image in VRAM and you can see it properly... If you turn on overlay, then you will see on top of that same image, your text that you had before.
// 		// Mstr_Ctrl_Text_Mode_En  = $01       ; Enable the Text Mode
// 		// Mstr_Ctrl_Text_Overlay  = $02       ; Enable the Overlay of the text mode on top of Graphic Mode (the Background Color is ignored)
// 		// Mstr_Ctrl_Graph_Mode_En = $04       ; Enable the Graphic Mode
// 		// Mstr_Ctrl_Bitmap_En     = $08       ; Enable the Bitmap Module In Vicky
// 		// all of these should be ON
// 	}
// 	else
// 	{
// 		R8(VICKY_MASTER_CTRL_REG_L) = (GRAPHICS_MODE_TEXT);
// 		// disable bitmap
// 		R8(BITMAP_CTRL) = 0x00;
// 	}
// 	
// 	Sys_RestoreIOPage();
// }


// //! Change video mode to the one passed.
// //! @param	the_screen: valid pointer to the target screen to operate on
// //! @param	new_mode: One of the enumerated screen_resolution values. Must correspond to a valid VICKY video mode for the host machine. See VICKY_IIIA_RES_800X600_FLAGS, etc. defined in a2560_platform.h
// //! @return	returns false on any error/invalid input.
// bool Sys_SetVideoMode(uint8_t new_mode)
// {
// 	uint8_t		new_mode_flag;
// 	
// 	if (new_mode == RES_320X240)
// 	{
// 		new_mode_flag = VICKY_RES_320X240_FLAGS;
// 	}
// 	else if (new_mode == RES_320X200)
// 	{
// 		new_mode_flag = VICKY_RES_320X200_FLAGS;
// 	}
// 	else
// 	{
// 		LOG_ERR(("%s %d: specified video mode is not legal for this screen %u", __func__, __LINE__, new_mode));
// 		return false;
// 	}
// 	
//  	//DEBUG_OUT(("%s %d: specified video mode = %u, flag=%u", __func__, __LINE__, new_mode, new_mode_flag));
// 		
// 	Sys_SwapIOPage(VICKY_IO_PAGE_REGISTERS);
// 	
//  	//DEBUG_OUT(("%s %d: vicky before = %x", __func__, __LINE__, *the_screen->vicky_ ));
// 	R8(VICKY_MASTER_CTRL_REG_H) = R8(VICKY_MASTER_CTRL_REG_H) & new_mode_flag;
//  	//DEBUG_OUT(("%s %d: vicky after = %x", __func__, __LINE__, *the_screen->vicky_ ));
// 	
// 	Sys_RestoreIOPage();
// 	
// 	// teach screen about the new settings
// 	if (Sys_DetectScreenSize() == false)
// 	{
// 		LOG_ERR(("%s %d: Changed screen resolution, but the selected resolution could not be handled", __func__, __LINE__, new_mode));
// 		return false;
// 	}
// 
// 	// tell the MCP that we changed res so it can update it's internal col sizes, etc.  - this function is not exposed in MCP headers yet
// 	//sys_text_setsizes();
// 	
// 	return true;
// }


//! Detect the current screen mode/resolution, and set # of columns, rows, H pixels, V pixels, accordingly
bool Sys_DetectScreenSize(void)
{
	//uint8_t			new_mode;
	uint8_t			the_video_mode_bits;
	uint8_t			border_x_cols;
	uint8_t			border_y_cols;
	int16_t			border_x_pixels;
	int16_t			border_y_pixels;
	
	// detect the video mode and set resolution based on it
	
	the_video_mode_bits = R8(VICKY_MASTER_CTRL_REG_H);
	//DEBUG_OUT(("%s %d: 8bit vicky ptr 2nd byte=%p, video mode bits=%x", __func__, __LINE__, vicky_8bit_ptr, the_video_mode_bits));
	
	//   F256JR has 1 channel with 2 video modes, 70hz=640x400 (graphics doubled to 320x200) and 60hz=640x480

	if (the_video_mode_bits & VIDEO_MODE_FREQ_BIT)
	{
		//new_mode = RES_320X200;
		global_system->text_mem_rows_ = TEXT_ROW_COUNT_70HZ; // 2 options in JR. the_screen->height_ / TEXT_FONT_HEIGHT;
	}
	else
	{
		//new_mode = RES_320X240;
		global_system->text_mem_rows_ = TEXT_ROW_COUNT_60HZ; // 2 options in JR. the_screen->height_ / TEXT_FONT_HEIGHT;
	}

// we don't really care about pixels in this app... 
// 	switch (new_mode)
// 	{
// 		case RES_320X200:
// 			the_screen->width_ = 320;	
// 			the_screen->height_ = 200;
// 			DEBUG_OUT(("%s %d: set to RES_320X200", __func__, __LINE__));
// 			break;
// 			
// 		case RES_320X240:
// 			the_screen->width_ = 320;	
// 			the_screen->height_ = 240;
// 			DEBUG_OUT(("%s %d: set to RES_320X200", __func__, __LINE__));
// 			break;
// 	}
	
	// detect borders, and set text cols/rows based on resolution modified by borders (if any)
	border_x_pixels = R8(VICKY_BORDER_X_SIZE);
	border_y_pixels = R8(VICKY_BORDER_Y_SIZE);
	//DEBUG_OUT(("%s %d: border x,y=%i,%i", __func__, __LINE__, R8(VICKY_BORDER_X_SIZE), R8(VICKY_BORDER_Y_SIZE)));
	
	border_x_cols = (border_x_pixels * 2) / TEXT_FONT_WIDTH;
	border_y_cols = (border_y_pixels * 2) / TEXT_FONT_HEIGHT;
	global_system->text_mem_cols_ = TEXT_COL_COUNT_FOR_PLOTTING; // only 1 option in JR. the_screen->width_ / TEXT_FONT_WIDTH;
	global_system->text_cols_vis_ = global_system->text_mem_cols_ - border_x_cols;
	global_system->text_rows_vis_ = global_system->text_mem_rows_ - border_y_cols;
// 	global_system->rect_.MaxX = the_screen->width_;
// 	global_system->rect_.MaxY = the_screen->height_;	
	//Sys_PrintScreen(the_screen);
	
	return true;
}


//! Set the left/right and top/bottom borders
//! This will reset the visible text columns as a side effect
//! Grotesquely large values will be accepted as is: use at your own risk!
//! @param	border_width: width in pixels of the border on left and right side of the screen. Total border used with be the double of this.
//! @param	border_height: height in pixels of the border on top and bottom of the screen. Total border used with be the double of this.
//! @return	returns false on any error/invalid input.
void Sys_SetBorderSize(uint8_t border_width, uint8_t border_height)
{
	uint8_t		border_x_cols;
	uint8_t		border_y_cols;

	// LOGIC: 
	//   borders are set in pixels, from 0 to 31 max. 
	//   borders have no effect unless the border is enabled!
	
	// set borders
	R8(VICKY_BORDER_X_SIZE) = border_width;
	R8(VICKY_BORDER_Y_SIZE) = border_height;
	
	// enable borders or disable
	if (border_width > 0 || border_height > 0)
	{
		R8(VICKY_BORDER_CTRL_REG) = 1;
	}
	else
	{
		R8(VICKY_BORDER_CTRL_REG) = 0;
	}
	
	border_x_cols = (border_width * 2) / TEXT_FONT_WIDTH;
	border_y_cols = (border_height * 2) / TEXT_FONT_HEIGHT;
	//DEBUG_OUT(("%s %d: x and y borders set to %u, %u", __func__, __LINE__, border_width, border_height));
	//DEBUG_OUT(("%s %d: x and y borders cols/rows now %u, %u", __func__, __LINE__, border_x_cols, border_y_cols));
	
	// now we need to recalculate how many text cols/rows are visible, because it might have changed
	global_system->text_cols_vis_ = global_system->text_mem_cols_ - border_x_cols;
	global_system->text_rows_vis_ = global_system->text_mem_rows_ - border_y_cols;
	//DEBUG_OUT(("%s %d: visible cols,rows now %u, %u", __func__, __LINE__, global_system->text_cols_vis_, global_system->text_rows_vis_));
}


//! Enable or disable the hardware cursor in text mode, for the specified screen
//! @param	the_system: valid pointer to system object
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param enable_it: If true, turns the hardware blinking cursor on. If false, hides the hardware cursor;
void Sys_EnableTextModeCursor(bool enable_it)
{
	uint8_t		the_cursor_mode_bits = R8(VICKY_TEXT_CURSOR_ENABLE);
	uint8_t		new_mode_flag;

	// LOGIC:
	//   bit 0 is enable/disable
	//   bit 1-2 are the speed of flashing
	//   bit 3 is solid (0) or flashing (1)
	
	if (enable_it)
	{
		new_mode_flag = 0xFF;
	}
	else
	{
		new_mode_flag = 0x00;
	}

	the_cursor_mode_bits  = (the_cursor_mode_bits & ~CURSOR_ONOFF_BITS) | (new_mode_flag & CURSOR_ONOFF_BITS);
	the_cursor_mode_bits  = (the_cursor_mode_bits & ~CURSOR_FLASH_RATE_BITS) | CURSOR_FLASH_RATE_1S;	// always use 1 blink per 1 sec rate
	
	R8(VICKY_TEXT_CURSOR_ENABLE) = the_cursor_mode_bits;
	
	R8(VICKY_TEXT_CURSOR_CHAR) = 7;	// 18=light 4x4 grid char, 181=medium box; 227=empty box; 7=filled space=CH_HFILL_UP_8
	
	//DEBUG_OUT(("%s %d: cursor enabled now=%u", __func__, __LINE__, enable_it));
	//DEBUG_OUT(("%s %d: vicky byte 3 after gamma change = %x, %x", __func__, __LINE__, the_gamma_mode_bits, R8(VICKY_GAMMA_CTRL_REG)));
	//DEBUG_OUT(("%s %d: wrote to %x to register at %p", __func__, __LINE__, the_gamma_mode_bits, P8(VICKY_GAMMA_CTRL_REG)));
}


// update the system clock with a date/time string in YY/MM/DD HH:MM format
// returns true if format was acceptable (and thus update of RTC has been performed).
bool Sys_UpdateRTC(char* datetime_from_user)
{
	static uint8_t		string_offsets[5] = {12,9,6,3,0};	// array is order by RTC order of min-hr-day-month-year
	static uint8_t		rtc_offsets[5] = {0,2,2,3,1};	// starting at min=d692
	static uint8_t		bounds[5] = {60,24,31,12,99};	// starting at min=d692
	volatile uint8_t	old_rtc_control;
	uint8_t				i;
	int8_t				this_digit;
	int8_t				tens_digit;
	uint8_t				rtc_array[5];
	uint8_t*			rtc_addr;
	
	for (i = 0; i < 5; i++)
	{
		this_digit = datetime_from_user[string_offsets[i]];
		
		if (this_digit < CH_ZERO || this_digit > CH_NINE)
		{
			return false;
		}
		
		tens_digit = this_digit - CH_ZERO;
		this_digit = datetime_from_user[string_offsets[i] + 1];
		
		if (this_digit < CH_ZERO || this_digit > CH_NINE)
		{
			return false;
		}
		
		rtc_array[i] = (this_digit - CH_ZERO) + (tens_digit * 10);	
	}

	//sprintf(global_string_buff1, "%02X %02X %02X %02X %02X", rtc_array[4], rtc_array[3], rtc_array[2], rtc_array[1], rtc_array[0]);
	//Text_DrawStringAtXY(0, 3, global_string_buff1, COLOR_BRIGHT_YELLOW, COLOR_BLACK);	

	// check if any of the numbers are too high
	for (i = 0; i < 5; i++)
	{
		if (rtc_array[i] > bounds[i])
		{
			return false;
		}
	}

	// numbers are all good, convert to BCD
	for (i = 0; i < 5; i++)
	{
		rtc_array[i] = Sys_DecimalToBCD(rtc_array[i]);
	}

	//sprintf(global_string_buff1, "20%02X-%02X-%02X %02X:%02X", rtc_array[4], rtc_array[3], rtc_array[2], rtc_array[1], rtc_array[0]);
	//Text_DrawStringAtXY(25, 3, global_string_buff1, COLOR_BRIGHT_YELLOW, COLOR_BLACK);	

	__asm("SEI"); // disable interrupts in case some other process has a role here
	
	// stop RTC from updating external registers. Required!
	old_rtc_control = R8(RTC_CONTROL);
	R8(RTC_CONTROL) = old_rtc_control | 0x08; // stop it from updating external registers

	rtc_addr = (uint8_t*)RTC_MINUTES;
	
	for (i = 0; i < 5; i++)
	{
		rtc_addr += rtc_offsets[i];		
		R8(rtc_addr) = rtc_array[i];
	}
	
	// restore timer control to what it had been
	R8(RTC_CONTROL) = old_rtc_control;

	__asm("CLI"); // restore interrupts

	return true;
}
