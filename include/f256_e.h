/*
 * f256_e.h
 *
 *  Created on: November 29, 2022
 *      Author: micahbly
 */

// this version is for the "e" or extended / flat memory map FPGA loads of the F256JR, F256K, and F256K2
// Adapted from F256jr/k version of FileManager starting June 2, 2024

#ifndef F256_E_H_
#define F256_E_H_



/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

#include <stdint.h>


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/


// adapted from vinz67
#define R8(x)						*((volatile __far uint8_t* const)(x))			// make sure we read an 8 bit byte; for VICKY registers, etc.
#define P8(x)						(volatile __far uint8_t* const)(x)			// make sure we read an 8 bit byte; for VICKY registers, etc.
#define R16(x)						*((volatile __far uint16_t* const)(x))		// make sure we read an 16 bit byte; for RNG etc.
#define R32(x)						*((volatile __far uint32_t* const)(x))		// make sure we read an 32 bit byte;
// and near variants. keeping far variants as regular "R8", etc, for backwards compatibility with other 6502 code of mine
#define NR8(x)						*((volatile uint8_t* const)(x))			// make sure we read an 8 bit byte; for VICKY registers, etc.
#define NP8(x)						(volatile uint8_t* const)(x)			// make sure we read an 8 bit byte; for VICKY registers, etc.
#define NR16(x)						*((volatile uint16_t* const)(x))		// make sure we read an 16 bit byte; for RNG etc.


// ** F256jr - Tiny VICKY

#define TEXT_COL_COUNT_FOR_PLOTTING		80	// regardless of visible cols (between borders), VRAM is fixed at 80 cols across.
#define TEXT_ROW_COUNT_FOR_PLOTTING		60	// regardless of visible rows (between borders), VRAM is fixed at 60 rows up/down.

#define TEXT_ROW_COUNT_60HZ			60
#define TEXT_ROW_COUNT_70HZ			50

#define TEXT_FONT_WIDTH				8	// for text mode, the width of the fixed-sized font chars
#define TEXT_FONT_HEIGHT			8	// for text mode, the height of the fixed-sized font chars.
#define TEXT_FONT_BYTE_SIZE			(8*256)

#define VIDEO_MODE_FREQ_BIT			0x01	//!> the bits in the 2nd byte of the system control register that define video mode (resolution). if this bit is set, resolution is 70hz 320x200 (text mode 80*25). if clar, is 60hz 630*240
#define VIDEO_MODE_DOUBLE_X_BIT		0x02	//!> the bits in the 2nd byte of the system control register control text mode doubling in horizontal. if set, text mode chars are doubled in size, producing 40 chars across
#define VIDEO_MODE_DOUBLE_Y_BIT		0x04	//!> the bits in the 3rd byte of the system control register control text mode doubling in vertical. if set, text mode chars are doubled in size, producing 25 or 30 chars down

#define GAMMA_MODE_DIPSWITCH_BIT	0x20	//!>the bits in the 2nd byte of the system control register reflect dip switch setting for control gamma correction on/off
#define GAMMA_MODE_ONOFF_BITS		0b01000000	//!>the bits in the 1st byte of the system control register control gamma correction on/off


// Address	R/W	7 6 5 4 3 2 1 0 
// 0xD000 	R/W X GAMMA SPRITE TILE BITMAP GRAPH OVRLY TEXT 
// 0xD001 	R/W — FON_SET FON_OVLY MON_SLP DBL_Y DBL_X CLK_70

// Address	R/W 7  6 5 4  3  2 1 0 
// 0xD002	R/W —  LAYER1 —  LAYER0 
// 0xD003	R/W —  - - -  -  LAYER2
//Table 4.2: Bitmap and Tile Map Layer Registers

// Tiny VICKY I/O page 0 addresses
#define VICKY_BASE_ADDRESS				0xf01000		// Tiny VICKY offset/first register
#define VICKY_MASTER_CTRL_REG_L			0xf01000		// Tiny VICKY Master Control Register - low - gamma, sprite, tiles, bitmap, graphics, text modes
#define VICKY_MASTER_CTRL_REG_H			0xf01001		// Tiny VICKY Master Control Register - high - font 1/2, screen res, etc.
#define VICKY_LAYER_CTRL_1				0xf01002		// VICKY Bitmap and Tile Map Layer Register 1
#define VICKY_LAYER_CTRL_2				0xf01003		// VICKY Bitmap and Tile Map Layer Register 2
#define VICKY_BORDER_CTRL_REG			0xf01004		// Tiny VICKY Border Control Register
#define VICKY_BORDER_COLOR_B			0xf01005		// Tiny VICKY Border Color Blue
#define VICKY_BORDER_COLOR_G			0xf01006		// Tiny VICKY Border Color Green
#define VICKY_BORDER_COLOR_R			0xf01007		// Tiny VICKY Border Color Red
#define VICKY_BORDER_X_SIZE				0xf01008		// Tiny VICKY Border X size in pixels
#define VICKY_BORDER_Y_SIZE				0xf01009		// Tiny VICKY Border Y size in pixels
#define VICKY_BACKGROUND_COLOR_B		0xf0100d		// Tiny VICKY background color Blue
#define VICKY_BACKGROUND_COLOR_G		0xf0100e		// Tiny VICKY background color Green
#define VICKY_BACKGROUND_COLOR_R		0xf0100f		// Tiny VICKY background color Red
#define VICKY_TEXT_CURSOR_ENABLE		0xf01010		// Bit 0=enable cursor; 1-2=rate; 3=flash enable; 4-7 unused
	#define CURSOR_ONOFF_BITS				0b00000001		//!> bit 0 controls whether cursor is displayed or not
	#define CURSOR_FLASH_RATE_BITS			0b00000110		//!> bits 1-2 control rate of cursor flashing (if visible)
	#define CURSOR_FLASH_RATE_1S			0b00000000		//!> bits 1&2 off = 1 blink per second
	#define CURSOR_FLASH_RATE_12S			0b00000010		//!> bits 1 on = 1 blink per 1/2 second
	#define CURSOR_FLASH_RATE_14S			0b00000100		//!> bits 2 on = 1 blink per 1/4 second
	#define CURSOR_FLASH_RATE_15S			0b00000110		//!> bits 1&2 on = 1 blink per 1/5 second
	#define CURSOR_FLASH_ON_BITS			0b00001000		//!> bit 3 controls whether cursor flashes (1) or remains solid (0)
#define VICKY_TEXT_CURSOR_CHAR			0xf01012		// 1-byte
#define VICKY_TEXT_X_POS				0xf01014		// 2-byte
#define VICKY_TEXT_Y_POS				0xf01016		// 2-byte

#define VICKY_LINE_INT_CTRL				0xf01018		// Vicky's line interrupt. 1 to enable, 0 to disable. other bit positions not used.
#define VICKY_LINE_INT_LINE_L			0xf01019
#define VICKY_LINE_INT_LINE_H			0xf0101a
#define VICKY_LINE_INT_RESERVED			0xf0101b
#define VICKY_LINE_INT_RAST_COL_L		0xf0101c
#define VICKY_LINE_INT_RAST_COL_H		0xf0101d
#define VICKY_LINE_INT_RAST_ROW_L		0xf0101e
#define VICKY_LINE_INT_RAST_ROW_H		0xf0101f

#define BITMAP_L0_CTRL					0xf01100		//!> bitmap control register for first bitmap
#define BITMAP_L0_VRAM_ADDR_L			0xf01101		//!> bitmap VRAM address pointer)		
#define BITMAP_L0_VRAM_ADDR_M			0xf01102		//!> bitmap VRAM address pointer)		
#define BITMAP_L0_VRAM_ADDR_H			0xf01103		//!> bitmap VRAM address pointer)
#define BITMAP_L1_CTRL					0xf01108		//!> bitmap control register for 2nd bitmap
#define BITMAP_L1_VRAM_ADDR_L			0xf01109		//!> bitmap VRAM address pointer)		
#define BITMAP_L1_VRAM_ADDR_M			0xf0110a		//!> bitmap VRAM address pointer)		
#define BITMAP_L1_VRAM_ADDR_H			0xf0110b		//!> bitmap VRAM address pointer)
#define BITMAP_L2_CTRL					0xf01110		//!> bitmap control register for 3rd bitmap
#define BITMAP_L2_VRAM_ADDR_L			0xf01111		//!> bitmap VRAM address pointer)		
#define BITMAP_L2_VRAM_ADDR_M			0xf01112		//!> bitmap VRAM address pointer)		
#define BITMAP_L2_VRAM_ADDR_H			0xf01113		//!> bitmap VRAM address pointer)



// ** tiles and tile maps -- see also VICKY_LAYER_CTRL_1 and VICKY_LAYER_CTRL_2

#define TILE0_CTRL						0xf01200		//!> tile control register for tilemap #1
#define TILE1_CTRL						0xf0120c		//!> tile control register for tilemap #2
#define TILE2_CTRL						0xf01218		//!> tile control register for tilemap #3

#define TILE_REG_LEN					0x0c		// number of bytes between start of one tilemap register and the next
#define TILE_CTRL_OFFSET_ADDR_LO		0x01		// offset from the tilemap control to the low address
#define TILE_CTRL_OFFSET_ADDR_MED		0x02		// offset from the tilemap control to the medium address
#define TILE_CTRL_OFFSET_ADDR_HI		0x03		// offset from the tilemap control to the high address
#define TILE_CTRL_OFFSET_MAP_SIZE_X		0x04		// offset from the tilemap control to num columns in map
#define TILE_CTRL_OFFSET_MAP_SIZE_Y		0x06		// offset from the tilemap control to num rows in map
#define TILE_CTRL_OFFSET_SCROLL_X_LO	0x08		// offset from the tilemap control horizontal scrollowing (lo)
#define TILE_CTRL_OFFSET_SCROLL_X_HI	0x09		// offset from the tilemap control horizontal scrollowing (hi)
#define TILE_CTRL_OFFSET_SCROLL_Y_LO	0x0a		// offset from the tilemap control vertical scrollowing (lo)
#define TILE_CTRL_OFFSET_SCROLL_Y_HI	0x0b		// offset from the tilemap control vertical scrollowing (hi)

#define TILESET0_ADDR_LO				0xf01280		// 20-bit address of tileset, lo
#define TILESET0_ADDR_MED				0xf01281		// 20-bit address of tileset, medium
#define TILESET0_ADDR_HI				0xf01282		// 20-bit address of tileset, hi
#define TILESET0_SHAPE					0xf01283		// 0 if tiles are arranged in 1 vertical column, or 8 if in a square
#define TILESET_REG_LEN					0x04		// number of bytes between start of one tileset register and the next


// ** serial comms related

#define UART_BASE						0xf01630		// starting point of serial-related registers
#define UART_RBR						(UART_BASE + 0)
#define UART_IER						(UART_BASE + 1)
	// flags for UART interrupt register
	#define FLAG_UART_IER_RXA			0b00000001		// data receive will trigger interrupt if set
	#define FLAG_UART_IER_TXA			0b00000010		// data send will trigger interrupt if set
	#define FLAG_UART_IER_ERR			0b00000100		// data error will trigger interrupt if set
	#define FLAG_UART_IER_STAT			0b00001000		// RS-232 line state change will trigger interrupt if set
	
#define UART_IIR						(UART_BASE + 2)
#define UART_LCR						(UART_BASE + 3)
#define UART_MCR						(UART_BASE + 4)	// Modem Control Register. "Before setting up any interrupts, you must set bit 3 of the MCR (UART register 4) to 1. This toggles the GPO2, which puts the UART out of tri-state, and allows it to service interrupts." -- https://www.activexperts.com/serial-port-component/tutorials/uart/
	// flags for UART interrupt register
	#define FLAG_UART_MCR_DTR			0b00000001		// Is reflected on RS-232 DTR (Data Terminal Ready) line.
	#define FLAG_UART_MCR_RTS			0b00000010		// Reflected on RS-232 RTS (Request to Send) line.
	#define FLAG_UART_MCR_OUT1			0b00000100		// GPO1 (General Purpose Output 1).
	#define FLAG_UART_MCR_OUT2			0b00001000		// GPO2 (General Purpose Output 2). Enables interrupts to be sent from the UART to the PIC.
	#define FLAG_UART_MCR_LOOP			0b00010000		// Echo (loop back) test.  All characters sent will be echoed if set.	
#define UART_LSR						(UART_BASE + 5)
#define UART_MSR						(UART_BASE + 6)
#define UART_SCR						(UART_BASE + 7)

#define UART_THR						(UART_BASE + 0)	// write register when DLAB=0
#define UART_FCR						(UART_BASE + 2)	// write register when DLAB=0
#define UART_DLL						(UART_BASE + 0)	// read/write register when DLAB=1
#define UART_DLM						(UART_BASE + 1)	// read/write register when DLAB=1

#define UART_BAUD_DIV_300		5244	// divisor for 300 baud
#define UART_BAUD_DIV_600		2622	// divisor for 600 baud
#define UART_BAUD_DIV_1200		1311	// divisor for 1200 baud
#define UART_BAUD_DIV_1800		874		// divisor for 1800 baud
#define UART_BAUD_DIV_2000		786		// divisor for 2000 baud
#define UART_BAUD_DIV_2400		655		// divisor for 2400 baud
#define UART_BAUD_DIV_3600		437		// divisor for 3600 baud
#define UART_BAUD_DIV_4800		327		// divisor for 4800 baud
#define UART_BAUD_DIV_9600		163		// divisor for 9600 baud
#define UART_BAUD_DIV_19200		81		// divisor for 19200 baud
#define UART_BAUD_DIV_38400		40		// divisor for 38400 baud
#define UART_BAUD_DIV_57600		27		// divisor for 57600 baud
#define UART_BAUD_DIV_115200	13		// divisor for 115200 baud

#define UART_DATA_BITS			0b00000011	// 8 bits
#define UART_STOP_BITS			0			// 1 stop bit
#define UART_PARITY				0			// no parity
#define UART_BRK_SIG			0b01000000
#define UART_NO_BRK_SIG			0b00000000
#define UART_DLAB_MASK			0b10000000
#define UART_THR_IS_EMPTY		0b00100000
#define UART_THR_EMPTY_IDLE		0b01000000
#define UART_DATA_AVAILABLE		0b00000001
#define UART_ERROR_MASK			0b10011110



#define VICKY_PS2_INTERFACE_BASE		0xf01640
#define VICKY_PS2_CTRL					(VICKY_PS2_INTERFACE_BASE)
#define VICKY_PS2_OUT					(VICKY_PS2_CTRL + 1)
#define VICKY_PS2_KDB_IN				(VICKY_PS2_OUT + 1)
#define VICKY_PS2_MOUSE_IN				(VICKY_PS2_KDB_IN + 1)
#define VICKY_PS2_STATUS				(VICKY_PS2_MOUSE_IN + 1)

#define VICKY_PS2_CTRL_FLAG_K_WR		0b00000010		// set to 1 then 0 to send a byte written on PS2_OUT to the keyboard
#define VICKY_PS2_CTRL_FLAG_M_WR		0b00001000		// set to 1 then 0 to send a byte written on PS2_OUT to the mouse
#define VICKY_PS2_CTRL_FLAG_KCLR		0b00010000		// set to 1 then 0 to clear the keyboard input FIFO queue
#define VICKY_PS2_CTRL_FLAG_MCLR		0b00100000		// set to 1 then 0 to clear the mouse input FIFO queue
#define VICKY_PS2_STATUS_FLAG_KEMP		0b00000001		// when 1, the keyboard input FIFO is empty
#define VICKY_PS2_STATUS_FLAG_MEMP		0b00000010		// when 1, the mouse input FIFO is empty
#define VICKY_PS2_STATUS_FLAG_M_NK		0b00010000		// when 1, the code sent to the mouse has resulted in an error
#define VICKY_PS2_STATUS_FLAG_M_AK		0b00100000		// when 1, the code sent to the mouse has been acknowledged
#define VICKY_PS2_STATUS_FLAG_K_NK		0b01000000		// when 1, the code sent to the keyboard has resulted in an error
#define VICKY_PS2_STATUS_FLAG_K_AK		0b10000000		// when 1, the code sent to the keyboard has been acknowledged

#define RTC_SECONDS						0xf01690		//  654: second digit, 3210: 1st digit
#define RTC_SECONDS_ALARM				0xf01691		//  654: second digit, 3210: 1st digit
#define RTC_MINUTES						0xf01692		//  654: second digit, 3210: 1st digit
#define RTC_MINUTES_ALARM				0xf01693		//  654: second digit, 3210: 1st digit
#define RTC_HOURS						0xf01694		//   54: second digit, 3210: 1st digit
#define RTC_HOURS_ALARM					0xf01695		//   54: second digit, 3210: 1st digit
#define RTC_DAY							0xf01696		//   54: second digit, 3210: 1st digit
#define RTC_DAY_ALARM					0xf01697		//   54: second digit, 3210: 1st digit
#define RTC_DAY_OF_WEEK					0xf01698		//  210: day of week digit
#define RTC_MONTH						0xf01699		//    4: second digit, 3210: 1st digit
#define RTC_YEAR						0xf0169a		// 7654: second digit, 3210: 1st digit
#define RTC_RATES						0xf0169b		//  654: WD (watchdog, not really relevant to F256); 3210: RS
	#define FLAG_RTC_RATE_NONE			0b00000000		// applies to bits 3210 of RTC_RATES
	#define FLAG_RTC_RATE_31NS			0b00000001		// applies to bits 3210 of RTC_RATES. See manual for values between 0001 and 1101
	#define FLAG_RTC_RATE_125MS			0b00001101		// applies to bits 3210 of RTC_RATES
	#define FLAG_RTC_RATE_63MS			0b00001100		// applies to bits 3210 of RTC_RATES - 62.5ms
	#define FLAG_RTC_RATE_250MS			0b00001110		// applies to bits 3210 of RTC_RATES
	#define FLAG_RTC_RATE_500MS			0b00001111		// applies to bits 3210 of RTC_RATES 
#define RTC_ENABLES						0xf0169c		// Controls various interrupt enables, only some of which apply to an F256
	#define FLAG_RTC_PERIODIC_INT_EN	0b00000100		// set PIE (bit 2) to raise interrupt based on RTC_RATES
	#define FLAG_RTC_ALARM_INT_EN		0b00001000		// Set AEI (bit 3) to raise interrupt based on RTC_SECONDS_ALARM, etc. 
#define RTC_FLAGS						0xf0169d		// check to see why an RTC interrupt was raised
	#define FLAG_RTC_PERIODIC_INT		0b00000100		// will be set if interrupt was raised based on RTC_RATES
	#define FLAG_RTC_ALARM_INT			0b00001000		// will be set if interrupt was raised based on alarm clock
#define RTC_CONTROL						0xf0169e		// set UTI (bit 3) to disable update of reg, to read secs. 
	#define MASK_RTC_CTRL_DSE			0b00000001		// if set (1), daylight savings is in effect.
	#define MASK_RTC_CTRL_12_24			0b00000010		// sets whether the RTC is using 12 or 24 hour accounting (1 = 24 Hr, 0 = 12 Hr)
	#define MASK_RTC_CTRL_STOP			0b00000100		// If it is clear (0) before the system is powered down, it will avoid draining the battery and may stop tracking the time. If it is set (1), it will keep using the battery as long as possible.
	#define MASK_RTC_CTRL_UTI			0b00001000		// if set (1), the update of the externally facing registers by the internal timers is inhibited. In order to read or write those registers, the program must first set UTI and then clear it when done.
	#define MASK_RTC_CTRL_UNUSED		0b11110000		// the upper 4 bits are not used.
#define RTC_CENTURY						0xf0169f		// 7654: century 10s digit, 3210: centurys 1s digit

// registers in write-only mode:
#define SYS0_REG						0xf016a0		// 
	// flags for write operations
	#define FLAG_SYS0_REG_W_PWR_L			0b00000001		// turns power LED on or off
	#define FLAG_SYS0_REG_W_SD_L			0b00000010		// turns disk (SD) LED on or off
	#define FLAG_SYS0_REG_W_L0				0b00000100		// set motherboard LED 0 to manual (1) or auto (0) mode
	#define FLAG_SYS0_REG_W_L1				0b00001000		// set motherboard LED 1 to manual (1) or auto (0) mode
	#define FLAG_SYS0_REG_W_BUZZER			0b00010000		// allows programmer to generate buzzer sounds by toggling on/off. F256JR only.
	#define FLAG_SYS0_REG_W_CAP_EN			0b00100000		// set to 1 to enable the LED for capslock. F256K only.
	#define FLAG_SYS0_REG_W_RESET			0b10000000		// set, then unset to trigger. See also SYS_RESET0 and SYS_RESET1 for more requirements.
	// flags for read operations
	#define FLAG_SYS0_REG_R_PWR_L			0b00000001		// power LED on or off
	#define FLAG_SYS0_REG_R_SD_L			0b00000010		// disk (SD) LED on or off
	#define FLAG_SYS0_REG_R_L0				0b00000100		// motherboard LED 0 manual (1) or auto (0) mode
	#define FLAG_SYS0_REG_R_L1				0b00001000		// motherboard LED 1 manual (1) or auto (0) mode
	#define FLAG_SYS0_REG_R_BUZZER			0b00010000		// allows programmer to generate buzzer sounds by toggling on/off. F256JR only.
	#define FLAG_SYS0_REG_R_SD_CD			0b00100000		// shows write-protect status of the SD card
	#define FLAG_SYS0_REG_R_SD_WP			0b01000000		// indicates if an SD card has been detected
#define SYS1_REG						0xf016a1		// 
	#define FLAG_SYS1_REG_L0_MAN			0b00000001		// controls MB LED 0 on/off when LED is in manual mode
	#define FLAG_SYS1_REG_L1_MAN			0b00000010		// controls MB LED 1 on/off when LED is in manual mode
	#define FLAG_SYS1_REG_PSG_STEREO		0b00000100		// set 1 for 4 left, 4 right, or 0 for all 8 channels to be sent to both L/R output
	#define FLAG_SYS1_REG_SID_STEREO		0b00001000		// set 1 for 3 left, 3 right, or 0 for all 6 channels to be sent to both L/R output
	#define FLAG_SYS1_REG_L0_RATE_L			0b00010000		// controls flash rate for MB LED 0 when in auto mode
	#define FLAG_SYS1_REG_L0_RATE_H			0b00100000		// controls flash rate for MB LED 0 when in auto mode
	#define FLAG_SYS1_REG_L1_RATE_L			0b01000000		// controls flash rate for MB LED 1 when in auto mode
	#define FLAG_SYS1_REG_L1_RATE_H			0b10000000		// controls flash rate for MB LED 1 when in auto mode
#define SYS_RESET0						0xf016a2		// to force a software reset, put 0xDE in SYS_RESET0, and 0xAD in SYS_RESET1, the set SYS0_REG's reset flag
#define SYS_RESET1						0xf016a3		// see SYS_RESET0

#define RANDOM_NUM_GEN_LOW				0xf016a4		// both the SEEDL and the RNDL (depends on bit 1 of RND_CTRL)
#define RANDOM_NUM_GEN_HI				0xf016a5		// both the SEEDH and the RNDH (depends on bit 1 of RND_CTRL)
#define RANDOM_NUM_GEN_ENABLE			0xf016a6		// bit 0: enable/disable. bit 1: seed mode on/off. "RND_CTRL"

#define LED_POWER_BLUE					0xf016a7		// 
#define LED_POWER_GREEN					0xf016a8		// 
#define LED_POWER_RED					0xf016a9		// 
#define LED_DISK_BLUE					0xf016aa		// 
#define LED_DISK_GREEN					0xf016ab		// 
#define LED_DISK_RED					0xf016ac		// 
#define LED_CAPSLOCK_BLUE				0xf016ad		// 
#define LED_CAPSLOCK_GREEN				0xf016ae		// 
#define LED_CAPSLOCK_RED				0xf016af		// 

// registers in read-only mode:
#define MACHINE_ID_REGISTER				0xf016a7		// will be '2' for F256JR
#define MACHINE_PCB_ID_0				0xf016a8
#define MACHINE_PCB_ID_1				0xf016a9
#define MACHINE_PCB_MAJOR				0xf016eb		// error in manual? this and next 4 all show same addr. changing here to go up by 1.
#define MACHINE_PCB_MINOR				0xf016ec
#define MACHINE_PCB_DAY					0xf016ed
#define MACHINE_PCB_MONTH				0xf016ef
#define MACHINE_PCB_YEAR				0xf016f0
#define MACHINE_FPGA_SUBV_LOW			0xf016aa		// CHSV0 chip subversion in BCD (low)
#define MACHINE_FPGA_SUBV_HI			0xf016ab		// CHSV1 chip subversion in BCD (high)
#define MACHINE_FPGA_VER_LOW			0xf016ac		// CHV0 chip version in BCD (low)
#define MACHINE_FPGA_VER_HI				0xf016ad		// CHV1 chip version in BCD (high)
#define MACHINE_FPGA_NUM_LOW			0xf016ae		// CHN0 chip number in BCD (low)
#define MACHINE_FPGA_NUM_HI				0xf016af		// CHN1 chip number in BCD (high)

#define TEXT_FORE_LUT					0xf01800		// FG_CHAR_LUT_PTR	Text Foreground Look-Up Table
#define TEXT_BACK_LUT					0xf01840		// BG_CHAR_LUT_PTR	Text Background Look-Up Table

#define SPRITE0_CTRL					0xf01900		// Sprint #0 control register
#define SPRITE0_ADDR_LO					0xf01901		// Sprite #0 pixel data address register
#define SPRITE0_ADDR_MED				0xf01902		// Sprite #0 pixel data address register
#define SPRITE0_ADDR_HI					0xf01903		// Sprite #0 pixel data address register
#define SPRITE0_X_LO					0xf01904		// Sprite #0 X position
#define SPRITE0_X_HI					0xf01905		// Sprite #0 X position
#define SPRITE0_Y_LO					0xf01906		// Sprite #0 Y position
#define SPRITE0_Y_HI					0xf01907		// Sprite #0 Y position
#define SPRITE_REG_LEN					0x08		// number of bytes between start of one sprite register set and the next


#define VIA1_CTRL						0xf01b00		// 6522 VIA #2 Control registers (K only)
#define VIA1_PORT_B_DATA				0xf01b00		// VIA #2 Port B data
#define VIA1_PORT_A_DATA				0xf01b01		// VIA #2 Port A data
#define VIA1_PORT_B_DIRECTION			0xf01b02		// VIA #2 Port B Data Direction register
#define VIA1_PORT_A_DIRECTION			0xf01b03		// VIA #2 Port A Data Direction register
#define VIA1_TIMER1_CNT_L				0xf01b04		// VIA #2 Timer 1 Counter Low
#define VIA1_TIMER1_CNT_H				0xf01b05		// VIA #2 Timer 1 Counter High
#define VIA1_TIMER1_LATCH_L				0xf01b06		// VIA #2 Timer 1 Latch Low
#define VIA1_TIMER1_LATCH_H				0xf01b07		// VIA #2 Timer 1 Latch High
#define VIA1_TIMER2_CNT_L				0xf01b08		// VIA #2 Timer 2 Counter Low
#define VIA1_TIMER2_CNT_H				0xf01b09		// VIA #2 Timer 2 Counter High
#define VIA1_SERIAL_DATA				0xf01b0a		// VIA #2 Serial Data Register
#define VIA1_AUX_CONTROL				0xf01b0b		// VIA #2 Auxiliary Control Register
#define VIA1_PERI_CONTROL				0xf01b0c		// VIA #2 Peripheral Control Register
#define VIA1_INT_FLAG					0xf01b0d		// VIA #2 Interrupt Flag Register
#define VIA1_INT_ENABLE					0xf01b0e		// VIA #2 Interrupt Enable Register
#define VIA1_PORT_A_DATA_NO_HAND		0xf01b0f		// VIA #2 Port A data (no handshake)

#define VIA0_CTRL						0xf01c00		// 6522 VIA #1 Control registers
#define VIA0_PORT_B_DATA				0xf01c00		// VIA #1 Port B data
#define VIA0_PORT_A_DATA				0xf01c01		// VIA #1 Port A data
#define VIA0_PORT_B_DIRECTION			0xf01c02		// VIA #1 Port B Data Direction register
#define VIA0_PORT_A_DIRECTION			0xf01c03		// VIA #1 Port A Data Direction register
#define VIA0_TIMER1_CNT_L				0xf01c04		// VIA #1 Timer 1 Counter Low
#define VIA0_TIMER1_CNT_H				0xf01c05		// VIA #1 Timer 1 Counter High
#define VIA0_TIMER1_LATCH_L				0xf01c06		// VIA #1 Timer 1 Latch Low
#define VIA0_TIMER1_LATCH_H				0xf01c07		// VIA #1 Timer 1 Latch High
#define VIA0_TIMER2_CNT_L				0xf01c08		// VIA #1 Timer 2 Counter Low
#define VIA0_TIMER2_CNT_H				0xf01c09		// VIA #1 Timer 2 Counter High
#define VIA0_SERIAL_DATA				0xf01c0a		// VIA #1 Serial Data Register
#define VIA0_AUX_CONTROL				0xf01c0b		// VIA #1 Auxiliary Control Register
#define VIA0_PERI_CONTROL				0xf01c0c		// VIA #1 Peripheral Control Register
#define VIA0_INT_FLAG					0xf01c0d		// VIA #1 Interrupt Flag Register
#define VIA0_INT_ENABLE					0xf01c0e		// VIA #1 Interrupt Enable Register
#define VIA0_PORT_A_DATA_NO_HAND		0xf01c0f		// VIA #1 Port A data (no handshake)


// ** WizNet comms related

#define WIZNET_BASE						0xf01d80		// starting point of WizNet-related registers
#define WIZNET_CTRL						(WIZNET_BASE + 0)	// RW - WizNet Control Register
#define WIZNET_DATA						(WIZNET_BASE + 1)	// RW - WizNet DATA Register
#define WIZNET_FIFO_CNT					(WIZNET_BASE + 2)	// RO - WizNet FIFO Rx Count (16bit access)
#define WIZNET_FIFO_CNT_LO				(WIZNET_BASE + 2)	// RO - WizNet FIFO Rx Count low byte (8bit access)
#define WIZNET_FIFO_CNT_HI				(WIZNET_BASE + 3)	// RO - WizNet FIFO Rx Count hi byte (8bit access)


#define OPT_KBD_BASE					0xf01dc0		// start of 4 byte run related to the optical keyboard on the F256K2
#define OPT_KBD_DATA					0xf01dc0		// FIFO queue for mechanical keyboard. Each event takes 2 bytes (2 reads)
#define OPT_KBD_STATUS					0xf01dc1		// read-only, indicates if buffer is empty, and whether the machine has an optical keyboard or not
	#define FLAG_OPT_KBD_STAT_EMPTY		0b00000001		// if set, keyboard queue is empty (optical keyboard only)
	#define FLAG_OPT_KBD_STAT_MECH		0b10000000		// if set, keyboard is mechanical, not optical (i.e., an F256K with upgraded mobo)
#define OPT_KBD_COUNT					0xf01dc2		// number of bytes in the optical keyboard FIFO queue - 2 byte value


#define DMA_CTRL						0xf01f00		// VICKY's DMA control register
	#define FLAG_DMA_CTRL_ENABLE			0b00000001		// enable / disable DMA
	#define FLAG_DMA_CTRL_2D_OP				0b00000010		// set to enable a 2D operation, clear to do a linear operations
	#define FLAG_DMA_CTRL_FILL				0b00000100		// set to enable fill operation, clear to enable copy operation
	#define FLAG_DMA_CTRL_INTERUPT_EN		0b00001000		// enables triggering an interrupt when DMA is complete
	#define FLAG_DMA_CTRL_START				0b10000000		// set to trigger the DMA operation
#define DMA_STATUS						0xf01f01		// DMA status register (Read Only)
	#define FLAG_DMA_STATUS_BUSY			0b10000000		// status bit set when DMA is busy copying data
#define DMA_FILL_VALUE					0xf01f01		// 8-bit value to fill with (COPY only)
#define DMA_SRC_ADDR					0xf01f04		// Source address (system bus - 3 byte)
#define DMA_SRC_ADDR_L					0xf01f04		// Source address (system bus - 3 byte) - LOW
#define DMA_SRC_ADDR_M					0xf01f05		// Source address (system bus - 3 byte) - MEDIUM
#define DMA_SRC_ADDR_H					0xf01f06		// Source address (system bus - 3 byte) - HIGH
#define DMA_DST_ADDR					0xf01f08		// Destination address (system bus - 3 byte)
#define DMA_DST_ADDR_L					0xf01f08		// Destination address (system bus - 3 byte) - LOW
#define DMA_DST_ADDR_M					0xf01f09		// Destination address (system bus - 3 byte) - MEDIUM
#define DMA_DST_ADDR_H					0xf01f0a		// Destination address (system bus - 3 byte) - HIGH
#define DMA_COUNT						0xf01f0c		// Number of bytes to fill or copy - must be EVEN - the number of bytes to copy (only available when 2D is clear) - 19 bit
#define DMA_COUNT_L						0xf01f0c		// Number of bytes to fill or copy - must be EVEN - the number of bytes to copy (only available when 2D is clear) - 19 bit
#define DMA_COUNT_M						0xf01f0d		// Number of bytes to fill or copy - must be EVEN - the number of bytes to copy (only available when 2D is clear) - 19 bit
#define DMA_COUNT_H						0xf01f0e		// Number of bytes to fill or copy - must be EVEN - the number of bytes to copy (only available when 2D is clear) - 19 bit
#define DMA_WIDTH						0xf01f0c		// Width of 2D operation - 16 bits - only available when 2D is set
#define DMA_WIDTH_L						0xf01f0c		// Width of 2D operation - 16 bits - only available when 2D is set
#define DMA_WIDTH_M						0xf01f0d		// Width of 2D operation - 16 bits - only available when 2D is set
#define DMA_HEIGHT						0xf01f0e		// Height of 2D operation - 16 bits - only available when 2D is set
#define DMA_HEIGHT_L					0xf01f0e		// Height of 2D operation - 16 bits - only available when 2D is set
#define DMA_HEIGHT_M					0xf01f0f		// Height of 2D operation - 16 bits - only available when 2D is set
#define DMA_SRC_STRIDE					0xf01f10		// Source stride for 2D operation - 16 bits - only available when 2D COPY is set
#define DMA_SRC_STRIDE_L				0xf01f10		// Source stride for 2D operation - 16 bits - only available when 2D COPY is set
#define DMA_SRC_STRIDE_M				0xf01f11		// Source stride for 2D operation - 16 bits - only available when 2D COPY is set
#define DMA_DST_STRIDE					0xf01f12		// Destination stride for 2D operation - 16 bits - only available when 2D is set
#define DMA_DST_STRIDE_L				0xf01f12		// Destination stride for 2D operation - 16 bits - only available when 2D is set
#define DMA_DST_STRIDE_M				0xf01f13		// Destination stride for 2D operation - 16 bits - only available when 2D is set



// Tiny VICKY I/O page 1 addresses
#define FONT_MEMORY_BANK0				0xf02000		// FONT_MEMORY_BANK0	FONT Character Graphic Mem (primary)
#define FONT_MEMORY_BANK1				0xf02800		// FONT_MEMORY_BANK1	FONT Character Graphic Mem (secondary)
#define VICKY_CLUT0						0xf03000		// each addition LUT is 400 offset from here
#define VICKY_CLUT1						(VICKY_CLUT0 + 0x400)	// each addition LUT is 400 offset from here
#define VICKY_CLUT2						(VICKY_CLUT1 + 0x400)	// each addition LUT is 400 offset from here
#define VICKY_CLUT3						(VICKY_CLUT2 + 0x400)	// each addition LUT is 400 offset from here

// Tiny VICKY I/O page 2 addresses
#define VICKY_TEXT_CHAR_RAM				0xf04000		// 
#define VICKY_TEXT_ATTR_RAM				0xf06000		// 
#define VICKY_TEXT_RAM_DELTA			(VICKY_TEXT_ATTR_RAM - VICKY_TEXT_CHAR_RAM)	// make text math easy

#define GRAPHICS_MODE_TEXT		0x01	// 0b00000001	Enable the Text Mode
#define GRAPHICS_MODE_TEXT_OVER	0x02	// 0b00000010	Enable the Overlay of the text mode on top of Graphic Mode (the Background Color is ignored)
#define GRAPHICS_MODE_GRAPHICS	0x04	// 0b00000100	Enable the Graphic Mode
#define GRAPHICS_MODE_EN_BITMAP	0x08	// 0b00001000	Enable the Bitmap Module In Vicky
#define GRAPHICS_MODE_EN_TILE	0x10	// 0b00010000	Enable the Tile Module in Vicky
#define GRAPHICS_MODE_EN_SPRITE	0x20	// 0b00100000	Enable the Sprite Module in Vicky
#define GRAPHICS_MODE_EN_GAMMA	0x40	// 0b01000000	Enable the GAMMA correction - The Analog and DVI have different color values, the GAMMA is great to correct the difference
#define GRAPHICS_MODE_DIS_VIDEO	0x80	// 0b10000000	This will disable the Scanning of the Video information in the 4Meg of VideoRAM hence giving 100% bandwidth to the CPU


// VICKY RESOLUTION FLAGS
#define VICKY_RES_320X240_FLAGS		0x00	// 0b00000000
#define VICKY_PIX_DOUBLER_FLAGS		0x02	// 0b00000001
#define VICKY_RES_320X200_FLAGS		0x03	// 0b00000011

// 0xD001 VICKY resolution control bits
#define VICKY_RES_CLK_70_FLAG		0x01	// 0b00000001 -- 70Hz = 640x480. 60hz if off = 640x400
#define VICKY_RES_X_DOUBLER_FLAG	0x02	// 0b00000010 -- 640 -> 320 pix if set
#define VICKY_RES_Y_DOUBLER_FLAG	0x04	// 0b00000100 -- 480 or 400 -> 240 or 200 pix if set
#define VICKY_RES_MON_SLP			0x08	// 0b00001000 -- if set, the monitor SYNC signal will be turned off, putting the monitor to sleep
#define VICKY_RES_FON_OVLY			0x10	// 0b00010000 -- fclear(0),onlythetextforegroundcolorwillbedisplayedwhentextoverlaysgraphics(allbackground colors will be completely transparent). If set (1), both foreground and background colors will be displayed, except that background color 0 will be transparent.
#define VICKY_RES_FON_SET			0x20	// 0b00100000 -- if set (1), the text font displayed will be font set 1. If clear (0), the text font displayed will be font set 0.
#define VICKY_RES_UNUSED7			0x40	// 0b01000000
#define VICKY_RES_UNUSED8			0x80	// 0b10000000

#define VICKY_BITMAP_MAX_H_RES		320		// VICKY in F256K and Jr supports a max resolution of 320x240, even if text engine displays at 640x480
#define VICKY_BITMAP_MAX_V_RES		240		// VICKY in F256K and Jr supports a max resolution of 320x240, even if text engine displays at 640x480

#define RES_320X200		0
#define RES_320X240		1
#define RES_640X480		3		// currently F256K2 and 68K machines only
#define RES_800X600		4		// currently not supported on F256 platform; 68K only
#define RES_1024X768	5		// currently not supported on F256 platform; 68K only

// machine model numbers - for decoding s_sys_info.model - value read from MACHINE_ID_REGISTER (see above)
#define MACHINE_C256FMX			0x00	///< for s_sys_info.model
#define MACHINE_C256U			0x01	///< for s_sys_info.model
#define MACHINE_F256JR			0x02	///< for s_sys_info.model
#define MACHINE_F256JRE			0x03	///< for s_sys_info.model
#define MACHINE_GENX			0x04	///< for s_sys_info.model
#define MACHINE_C256_UPLUS		0x05	///< for s_sys_info.model
#define MACHINE_UNDEFINED_1		0x06	///< for s_sys_info.model
#define MACHINE_UNDEFINED_2		0x07	///< for s_sys_info.model
#define MACHINE_A2560X			0x08	///< for s_sys_info.model. (GenX 32Bits Side)
#define MACHINE_A2560U_PLUS		0x09	///< for s_sys_info.model. there is no A2560U only in the field
#define MACHINE_A2560M			0x0a	///< for s_sys_info.model. 
#define MACHINE_A2560K			0x0b	///< for s_sys_info.model. "classic" A2560K
#define MACHINE_A2560K40		0x0c	///< for s_sys_info.model
#define MACHINE_A2560K60		0x0d	///< for s_sys_info.model
#define MACHINE_UNDEFINED_3		0x0e	///< for s_sys_info.model
#define MACHINE_UNDEFINED_4		0x0f	///< for s_sys_info.model
#define MACHINE_F256P			0x10	///< for s_sys_info.model
#define MACHINE_F256K2			0x11	///< for s_sys_info.model
#define MACHINE_F256K			0x12	///< for s_sys_info.model
#define MACHINE_F256KE			0x13	///< for s_sys_info.model
#define MACHINE_F256K2E			0x14	///< for s_sys_info.model

#define MACHINE_MODEL_MASK		0x1F		

typedef uint8_t	ColorIdx;



/*****************************************************************************/
/*                                 Interrupts                                */
/*****************************************************************************/

// F256 Interrupt-related

// Pending Interrupt (Read and Write Back to Clear)
#define INT_PENDING_REG0		0xF01660	// 
#define INT_PENDING_REG1		0xF01661	// 
#define INT_PENDING_REG2		0xF01662	// IEC Signals Interrupt
#define INT_PENDING_REG3		0xF01663	// NOT USED
// Polarity Set
#define INT_POL_REG0			0xF01664	// 
#define INT_POL_REG1			0xF01665	// 
#define INT_POL_REG2			0xF01666	// IEC Signals Interrupt
#define INT_POL_REG3			0xF01667	// NOT USED
// Edge Detection Enable
#define INT_EDGE_REG0			0xF01668	// 
#define INT_EDGE_REG1			0xF01669	// 
#define INT_EDGE_REG2			0xF0166A	// IEC Signals Interrupt
#define INT_EDGE_REG3			0xF0166B	// NOT USED
// Mask
#define INT_MASK_REG0			0xF0166C	// 
#define INT_MASK_REG1			0xF0166D	// 
#define INT_MASK_REG2			0xF0166E	// IEC Signals Interrupt
#define INT_MASK_REG3			0xF0166F	// NOT USED

// Interrupt Bit Definition
// Register Block 0
#define JR0_INT00_SOF			0x01 	// Start of Frame @ 60FPS or 70hz (depending on the Video Mode)
#define JR0_INT01_SOL			0x02 	// Start of Line (Programmable)
#define JR0_INT02_KBD			0x04 	// PS2 Keyboard
#define JR0_INT03_MOUSE			0x08 	// PS2 Mouse 
#define JR0_INT04_TMR0			0x10 	// Timer0
#define JR0_INT05_TMR1			0x20 	// Timer1
#define JR0_INT06_DMA0			0x40 	// DMA0 
#define JR0_INT07_CRT			0x80 	// Cartridge
// Register Block 1
#define JR1_INT00_UART			0x01 	// UART
#define JR1_INT01_TVKY2			0x02 	// COLLISION INT2 (VICKY)
#define JR1_INT02_TVKY3			0x04 	// COLLISION INT3 (VICKY)
#define JR1_INT03_TVKY4			0x08 	// COLLISION INT4 (VICKY)
#define JR1_INT04_RTC			0x10 	// Real Time Clock
#define JR1_INT05_VIA0			0x20 	// VIA0 (Jr & K)
#define JR1_INT06_VIA1			0x40 	// VIA1 (K Only) - Local Keyboard
#define JR1_INT07_SDCARD		0x80 	// SDCard Insert Int
// Register Block 2
#define JR2_INT00_IEC_DAT		0x01 	// IEC_DATA_i (K2 Only)
#define JR2_INT01_IEC_CLK		0x02 	// IEC_CLK_i (K2 Only)
#define JR2_INT02_IEC_ATN		0x04 	// IEC_ATN_i (K2 Only)
#define JR2_INT03_IEC_SREQ		0x08 	// IEC_SREQ_i (K2 Only)
#define JR2_INT04_RSVD1			0x10 	// Reserved
#define JR2_INT05_WIFI			0x20 	// WIFI IRQ (Module Generated)
#define JR2_INT06_HDMI			0x40 	// HDMI IRQ (From Sii9022) (K2 Only)
#define JR2_INT07_RSVD4			0x80 	// Reserved
// Register Block 3
#define JR2_INT00_WIFI_FIFO_E	0x01 	// WIFI FIFO Empty Interrupt (Triggers when Data in FIFO (after being emptied))
#define JR2_INT01_MIDI_FIFO_E	0x02 	// MIDI FIFO Empty Interrupt (Triggers when Data in FIFO (after being emptied))
#define JR2_INT02_KYBD_FIFO_E	0x04 	// Optical Keyboard FIFO Empty Interrupt (Triggers when Data in FIFO (after being emptied)) (K2 Only)
#define JR2_INT03_RSVD1			0x08 	// Reserved
#define JR2_INT04_RSVD2			0x10 	// Reserved
#define JR2_INT05_RSVD3			0x20 	// Reserved
#define JR2_INT06_RSVD4			0x40 	// Reserved
#define JR2_INT07_RSVD5			0x80 	// Reserved


/*****************************************************************************/
/*                             Named Characters                              */
/*****************************************************************************/

#define SC_HLINE        150
#define SC_VLINE        130
#define SC_ULCORNER     160
#define SC_URCORNER     161
#define SC_LLCORNER     162
#define SC_LRCORNER     163
#define SC_ULCORNER_RND 188
#define SC_URCORNER_RND 189
#define SC_LLCORNER_RND 190
#define SC_LRCORNER_RND 191
#define SC_CHECKERED	199
#define SC_T_DOWN		155 // T-shape pointing down
#define SC_T_UP			157 // T-shape pointing up
#define SC_T_LEFT		158 // T-shape pointing left
#define SC_T_RIGHT		154 // T-shape pointing right
#define SC_T_JUNCTION	156 // + shape meeting of 4 lines

/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct DateTime {
    uint8_t			year;
    uint8_t			month;
    uint8_t			day;
    uint8_t			hour;
    uint8_t			min;
    uint8_t			sec;
} DateTime;

typedef struct VICKY256Header
{
    uint8_t		magic[8];		// must read "VICKY256"
    uint8_t		version;		// first version is 1
    uint8_t		reserved;		// future use
    uint16_t	width;			// in pixels
    uint16_t	height;			// in pixels
    uint32_t	num_pixels;		// equivalent of width*height; if not, there's a problem. this must equal the bytes that follow the palette in the header.
	uint8_t		palette[1024];	// VICKY compatible BGRA format: 4 bytes per color, 256 colors.
} VICKY256Header;


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/





#endif /* F256_E_H_ */
