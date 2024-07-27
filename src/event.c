/*
 * event.c
 *
 *  Created on: June 9, 2024
 *      Author: micahbly
 */
 
// This is a re-start from scratch of the OS/f event.c/event.h done in 2022, for use with f/manager F256K2
// Code from the OS/f version may eventually be copied into here, depending on how things go


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "event.h"
#include "app.h"
#include "comm_buffer.h"	// only needed if debugging// #include "memory.h"
#include "debug.h"
#include "keyboard.h"
#include "serial.h"
// #include "text.h"

// C includes
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>


// F256 includes
#include "f256_e.h"
#include "general.h"


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define NUM_ACTION_KEY_MAPS			6
#define NUM_ACTION_KEY_MAP_BYTES	(NUM_ACTION_KEY_MAPS * 2)
#define NUM_GRAPHIC_MAPPED_KEYS		99
#define FIRST_GRAPHIC_MAPPED_KEY	33		// the charset code point to start mapping to JIS. 
#define LAST_GRAPHIC_MAPPED_KEY		((FIRST_JIS_MAPPED_KEY + NUM_JIS_MAPPED_KEYS) - 1)
#define NUM_JIS_MAPPED_KEYS			93
#define FIRST_JIS_MAPPED_KEY		33		// the charset code point to start mapping to JIS. 
#define LAST_JIS_MAPPED_KEY			((FIRST_JIS_MAPPED_KEY + NUM_JIS_MAPPED_KEYS) - 1)
#define NUM_MAPPED_KEYS				137
#define NUM_EXTENDED_KEYS			12
#define NUM_EXTENDED_KEY_BYTES		(NUM_EXTENDED_KEYS * 2)
#define NUM_SHIFT_COMBOs			27
#define NUM_SHIFT_COMBO_BYTES		(NUM_SHIFT_COMBOs * 2)
#define NUM_MODIFIER_KEYS			9
#define NUM_MODIFIER_KEY_BYTES		(NUM_MODIFIER_KEYS * 2)
#define NUM_UNIFIED_MODIFIER_KEYS	5
#define DEBUG_SHIFT_VRAM_LOC		(VICKY_TEXT_CHAR_RAM + 135)	// place on screen to light up if any SHIFT is down
#define DEBUG_ALT_VRAM_LOC			(DEBUG_SHIFT_VRAM_LOC + 1)	// place on screen to light up if any ALT is down
#define DEBUG_CTRL_VRAM_LOC			(DEBUG_ALT_VRAM_LOC + 1)	// place on screen to light up if any CONTROL is down
#define DEBUG_GUI_VRAM_LOC			(DEBUG_CTRL_VRAM_LOC + 1)	// place on screen to light up if any GUI ("windows") is down

// related to keymap sets
#define KEY_SETMAP_STD				KEY_F1	// ISO standard key layout
#define KEY_SETMAP_F256K			KEY_F1	//KEY_F2	// F256K/Foenix key layout
#define KEY_SETMAP_FOENISCII_1		KEY_F3	// graphic characters mapped with Foenix-specific logic - primary set
#define KEY_SETMAP_FOENISCII_2		KEY_F4	// graphic characters mapped with Foenix-specific logic - secondary set
#define KEY_SETMAP_PETSCII			KEY_F5	// PETSCII/C-64 placement simulation (glyphs aren't 1:1, so only aproximate)
#define KEY_SETMAP_KANA				KEY_F7	// JIS approximation
#define KEYBOARD_MODE_VRAM_LOC		(VICKY_TEXT_CHAR_RAM + 158)	// place to place "kana" flag when in kana entry mode
#define KBD_MAP_STD_LED_R			0xFF	// standard keymap set will use white LED color, but only if capslock is on
#define KBD_MAP_STD_LED_G			0xFF	// standard keymap set will use white LED color, but only if capslock is on
#define KBD_MAP_STD_LED_B			0xFF	// standard keymap set will use white LED color, but only if capslock is on
#define KBD_MAP_FOENISCII_1_LED_R	0xFF	// FOENISCII keymap set 1 will use purple LED color
#define KBD_MAP_FOENISCII_1_LED_G	0x00	// FOENISCII keymap set 1 will use purple LED color
#define KBD_MAP_FOENISCII_1_LED_B	0xFF	// FOENISCII keymap set 1 will use purple LED color
#define KBD_MAP_FOENISCII_2_LED_R	0xB0	// FOENISCII keymap set 2 will use different purple LED color
#define KBD_MAP_FOENISCII_2_LED_G	0x00	// FOENISCII keymap set 2 will use different purple LED color
#define KBD_MAP_FOENISCII_2_LED_B	0x50	// FOENISCII keymap set 2 will use different purple LED color
#define KBD_MAP_PETSCII_LED_R		0x00	// PETSCII keymap set will use blue LED color in honor of Commodore
#define KBD_MAP_PETSCII_LED_G		0x00	// PETSCII keymap set will use blue LED color in honor of Commodore
#define KBD_MAP_PETSCII_LED_B		0xFF	// PETSCII keymap set will use blue LED color in honor of Commodore
#define KBD_MAP_KANA_LED_R			0xFF	// Japanese kana keymap set will use red LED color
#define KBD_MAP_KANA_LED_G			0x00	// Japanese kana keymap set will use red LED color
#define KBD_MAP_KANA_LED_B			0x00	// Japanese kana keymap set will use red LED color


// "prefix" codes
// if not one of the first 3, then anything 84 and higher is considered out of range, and should be ignored.
#define KBD_KEY_RELEASED			0xf0
#define KBD_EXT_PREFIX0				0xe0
#define KBD_EXT_PREFIX1				0xe1
#define KBD_OUT_OF_RANGE_START		(KEY_F16 + 1)

#define KBD_INT_QUEUE_SIZE			256		//! waste bytes on this, but this way a uint8_t will roll over without math in the IRQ handler. 


#ifdef _F256K_

	#define KBD_MATRIX_SIZE		9
	#define KBD_COLUMNS			9
	#define KBD_ROWS			8

#endif


/*****************************************************************************/
/*                           File-scope Variables                            */
/*****************************************************************************/

static uint8_t			serial_temp;	// misc uses within serial interrupt
static uint8_t			modifier_keys_pressed[NUM_UNIFIED_MODIFIER_KEYS] = {0,0,0,0,0};
static uint8_t			ps2_released = 0;	// key released prefix received
static uint8_t			ps2_e0 = 0;		// e0 prefix received
static uint8_t			ps2_e1 = 0;		// e1 prefix received
static uint8_t			kbd_mapped_char;
static uint8_t			kbd_temp_char;
static uint8_t			kbd_raw_code;
static uint8_t			kbd_i;
static uint8_t			kbd_j;
static uint8_t			pending_int_value;
static uint8_t			kbd_map_mode = KEY_SETMAP_STD;	// KEY_SETMAP_STD, KEY_SETMAP_KANA, etc.
static uint8_t			kbd_event_what;
static uint8_t			kbd_modifiers;
//static event_kind		kbd_event_what;
//static event_modifiers	kbd_modifiers;
static bool				kbd_is_modifier_key;
static EventRecord*		kbd_event;
//static uint8_t			kbd_kana_mode_label[2] = {CH_JIS_KA, CH_JIS_NA};

// map of F256 VICKY PS/2 codes to Foenix ASCII
//   this may be replaced if/when MCP is ported to F256s
// See f256_e.h for matching macro definitions
const static uint8_t keymap[NUM_MAPPED_KEYS] = 
{
	0, KEY_F9, 0, KEY_F5, KEY_F3, KEY_F1, KEY_F2, KEY_F12, 	//0x00
	0, KEY_F10, KEY_F8, KEY_F6, KEY_F4, CH_TAB, '`', 0,
	0, KEY_LALT, KEY_LSHIFT, 0, KEY_LCTRL, 'q', '1', 0, 	//0x10
	0, 0, 'z', 's', 'a', 'w', '2', 0,
	0, 'c', 'x', 'd', 'e', '4', '3', 0, 				//0x20
	0, ' ', 'v', 'f', 't', 'r', '5', 0,
	0, 'n', 'b', 'h', 'g', 'y', '6', 0, 				//0x30
	0, 0, 'm', 'j', 'u', '7', '8', 0,
	0, ',', 'k', 'i', 'o', '0', '9', 0, 				//0x40
	0, '.', '/', 'l', ';', 'p', '-', 0,	// note: not seeing difference between / on keypad vs main part of keyboard.
	0, 0, '\'', 0, '[', '=', 0, 0, 						//0x50
	KEY_CAPS, KEY_RSHIFT, CH_ENTER, ']', 0, '\\', 0, 0,
	0, 0, 0, 0, 0, 0, KEY_BKSP, 0, 						//0x60
	0, CH_K1, 0, CH_K4, CH_K7, 0, 0, 0,
	CH_K0, CH_KPOINT, CH_K2, CH_K5, CH_K6, CH_K8, KEY_ESC, KEY_NUM, //0x70
	KEY_F11, CH_KPLUS, CH_K3, CH_KMINUS, CH_KTIMES, CH_K9, KEY_SCROLL, 0,
	0, 0, 0, KEY_F7, KEY_SYSREQ, 0, 0, 0, 0, 				//0x80
};

// mapping from ASCII 33 ('!') through 122 ('z'), to the Japanese JIS characters
const static uint8_t ascii_2_jis[NUM_JIS_MAPPED_KEYS] = 
{
					CH_JA_HOUR, 	CH_JIS_BOU,		CH_JIS_L_A, 	CH_JIS_L_U,		CH_JIS_L_E,		CH_JIS_L_YA,  					// 0x21
	CH_JIS_KE,		CH_JIS_L_YO,	CH_JIS_WO,		CH_JIS_L_YU,	CH_JIS_P,		CH_JIS_NE,		CH_JIS_HO,		CH_JIS_RU,		// 0x28
	CH_JIS_ME,		CH_JIS_WA,		CH_JIS_NU,		CH_JIS_HU,		CH_JIS_A,		CH_JIS_U,		CH_JIS_E,		CH_JIS_O,		// 0x30
	CH_JIS_YA,		CH_JIS_YU,		CH_JIS_YO,		CH_JA_SEC,		CH_JIS_RE,		CH_JIS_DOKUTEN,	CH_JIS_HE,		CH_JIS_KUTEN,	// 0x38
	CH_JIS_MID,		CH_JA_MIN,		'A',			'B',			'C',			'D',			'E',			'F',			// 0x40
	'G',			'H',			'I',			'J',			'K',			'L',			'M',			'N',			// 0x48
	'O',			'P',			'Q',			'R',			'S',			'T',			'U',			'V',			// 0x50
	'W',			'X',			'Y',			'Z',			CH_JIS_RO,		'\\',			CH_JIS_MU,		CH_JIS_L_O,		// 0x58
	CH_JIS_B,		'\'',			CH_LINE_ES,		CH_JIS_KO,		CH_JIS_SO,		CH_JIS_SHI,		CH_JIS_I,		CH_JIS_HA,		// 0x60
	CH_JIS_KI,		CH_JIS_KU,		CH_JIS_NI,		CH_JIS_MA,		CH_JIS_NO,		CH_JIS_RI,		CH_JIS_MO,		CH_JIS_MI,		// 0x68
	CH_JIS_RA,		CH_JIS_SE,		CH_JIS_TA,		CH_JIS_SU,		CH_JIS_TO,		CH_JIS_KA,		CH_JIS_NA,		CH_JIS_HI,		// 0x70
	CH_JIS_TE,		CH_JIS_SA,		CH_JIS_N,		CH_JIS_TSU,		CH_JIS_OPEN,	'|',			CH_JIS_CLOSE					// 0x78
};

// mapping from ASCII 33 ('!') through 122 ('z'), to the FOENISCII graphic characters
const static uint8_t ascii_2_foenscii_set1[NUM_GRAPHIC_MAPPED_KEYS] = 
{
						CH_VFILL_DN_1,		CH_DIAG_L8,			CH_VFILL_DN_3,		CH_VFILL_DN_4,		CH_VFILL_DN_5,		CH_VFILL_DN_7,		CH_DIAG_L7,			// 0x20
	CH_HLINE_UP_3,		CH_HLINE_UP_5,		CH_RECT_2,			CH_DIAG_L1,			CH_VLINE_UP_2,		CH_HLINE_UP_6,		CH_VLINE_UP_4,		CH_VLINE_UP_6,
	CH_HLINE_UP_4,		CH_VFILL_UP_1,		CH_VFILL_UP_2,		CH_VFILL_UP_3,		CH_VFILL_UP_4,		CH_VFILL_UP_5,		CH_VFILL_UP_6,		CH_VFILL_UP_7,		// 0x30
	CH_VFILL_UP_8,		CH_HLINE_UP_2,		CH_DIAG_R4,			CH_DIAG_R3,			CH_VLINE_UP_3,		CH_DIAG_R1,			CH_VLINE_UP_5,		CH_VLINE_UP_7,
	CH_VFILL_DN_2,		CH_LINE_RND_NE,		CH_HFILL_DN_5,		CH_HFILL_DN_3,		CH_LINE_WNS,		CH_LINE_WNE,		CH_BLOCK_SWNE,		CH_CIRCLE_1,		// 0x40
	CH_CIRCLE_4,		CH_MISC_DIA,		CH_RECT_4,			CH_RECT_6,			CH_DIAG_L2,			CH_HFILL_DN_7,		CH_HFILL_DN_6,		CH_MISC_CLUB,
	CH_DIAG_L4,			CH_LINE_RND_ES,		CH_DIAG_X,			CH_LINE_RND_WN,		CH_BLOCK_NE,		CH_MISC_COPY,		CH_HFILL_DN_4,		CH_LINE_RND_WS,		// 0x50
	CH_HFILL_DN_2,		CH_BLOCK_SE,		CH_HFILL_DN_1,		CH_DIAG_R7,			CH_BSLASH,			CH_ARROW_RIGHT,		CH_VFILL_DN_6,		CH_HLINE_UP_7,
	CH_LSQUOTE,			CH_LINE_NE,			CH_HFILL_UP_5,		CH_HFILL_UP_3,		CH_LINE_NES,		CH_LINE_WES,		CH_BLOCK_NWSE,		CH_CIRCLE_3,		// 0x60
	CH_CIRCLE_2,		CH_MISC_HEART,		CH_RECT_3,			CH_RECT_5,			CH_DIAG_R2,			CH_HFILL_UP_7,		CH_HFILL_UP_6,		CH_MISC_SPADE,
	CH_DIAG_L3,			CH_LINE_ES,			CH_LINE_WNES,		CH_LINE_WN,			CH_BLOCK_NW,		CH_MISC_FOENIX,		CH_HFILL_UP_4,		CH_LINE_WS,			// 0x70
	CH_HFILL_UP_2,		CH_BLOCK_SW,		CH_HFILL_UP_1,		CH_DIAG_R8,			CH_PIPE,			CH_ARROW_LEFT,
};

// mapping from ASCII 33 ('!') through 122 ('z'), to the FOENISCII graphic characters - secondary set with alt characters
const static uint8_t ascii_2_foenscii_set2[NUM_GRAPHIC_MAPPED_KEYS] = 
{
						CH_VFILLC_DN_1,		CH_DIAG_L10,		CH_VFILLC_DN_3,		CH_VFILLC_DN_4,		CH_VFILLC_DN_5,		CH_VFILLC_DN_7,		CH_DIAG_L9,			// 0x20
	CH_VDITH_2,			CH_VDITH_4,			CH_MISC_GBP,		CH_DIAG_L1,			CH_VLINE_UP_2,		CH_VDITH_5,			CH_VLINE_UP_4,		CH_VLINE_UP_6,
	CH_VDITH_3,			CH_VFILLC_UP_1,		CH_VFILLC_UP_2,		CH_VFILLC_UP_3,		CH_VFILLC_UP_4,		CH_VFILLC_UP_5,		CH_VFILLC_UP_6,		CH_VFILLC_UP_7,		// 0x30
	CH_VFILLC_UP_8,		CH_VDITH_1,			CH_DIAG_R6,			CH_DIAG_R5,			CH_VLINE_UP_3,		CH_DIAG_R1,			CH_VLINE_UP_5,		CH_VLINE_UP_7,
	CH_VFILLC_DN_2,		CH_LINE_BLD_RND_NE,	CH_HFILLC_DN_5,		CH_HFILLC_DN_3,		CH_LINE_BLD_WNS,	CH_LINE_BLD_WNE,	CH_LINE_BLD_NS,		CH_CIRCLE_1,		// 0x40
	CH_CIRCLE_4,		CH_HDITH_8,			CH_RECT_4,			CH_RECT_6,			CH_DIAG_L2,			CH_HFILLC_DN_7,		CH_HFILLC_DN_6,		CH_HDITH_10,
	CH_DIAG_L6,			CH_LINE_BLD_RND_ES,	CH_MISC_CHECKMARK,	CH_LINE_BLD_RND_WN,	CH_HDITH_2,			CH_HDITH_6,			CH_HFILLC_DN_4,		CH_LINE_BLD_RND_WS,	// 0x50
	CH_HFILLC_DN_2,		CH_HDITH_4,			CH_HFILLC_DN_1,		CH_DIAG_R9,			CH_BSLASH,			CH_ARROW_UP,		CH_VFILLC_DN_6,		CH_HLINE_UP_7,
	CH_LSQUOTE,			CH_LINE_BLD_NE,		CH_HFILLC_UP_5,		CH_HFILLC_UP_3,		CH_LINE_BLD_NES,	CH_LINE_BLD_WES,	CH_LINE_BLD_WE,		CH_CIRCLE_3,		// 0x60
	CH_CIRCLE_2,		CH_HDITH_7,			CH_RECT_3,			CH_RECT_5,			CH_DIAG_R2,			CH_HFILLC_UP_7,		CH_HFILLC_UP_6,		CH_HDITH_9,
	CH_DIAG_L5,			CH_LINE_BLD_ES,		CH_LINE_BLD_WNES,	CH_LINE_BLD_WN,		CH_HDITH_1,			CH_HDITH_5,			CH_HFILLC_UP_4,		CH_LINE_BLD_WS,		// 0x70
	CH_HFILLC_UP_2,		CH_HDITH_3,			CH_HFILLC_UP_1,		CH_DIAG_R10,		CH_PIPE,			CH_ARROW_DN,
};

// mapping from ASCII 65 ('A') through 122 ('z'), to the FOENISCII graphic characters, trying to match a C-64 keyboard layout as best as possible
const static uint8_t ascii_2_petscii[NUM_GRAPHIC_MAPPED_KEYS] = 
{
						CH_BANG,			CH_VFILLC_UP_4,		CH_HASH,			CH_DOLLAR,			CH_PERCENT,			CH_AMP,				CH_DIAG_L7,			// 0x20
	CH_LPAREN,			CH_RPAREN,			CH_AST,				CH_VLINE_UP_4,		CH_COMMA,			CH_VFILLC_UP_8,		CH_PERIOD,			CH_FSLASH,
	CH_0,				CH_1,				CH_2,				CH_3,				CH_4,				CH_5,				CH_6,				CH_7,				// 0x30
	CH_8,				CH_9,				CH_COLON,			CH_SEMIC,			CH_LESS,			CH_HFILLC_UP_4,		CH_GREATER,			CH_QUESTION,
	CH_AT,				CH_MISC_SPADE,		CH_LINE_NS,			CH_LINE_WE,			CH_HLINE_UP_5,		CH_HLINE_UP_6,		CH_VLINE_UP_2,		CH_VLINE_UP_2,		// 0x40
	CH_VLINE_UP_5,		CH_LINE_RND_WS,		CH_LINE_RND_NE,		CH_LINE_RND_WN,		CH_LINE_BLD_NE,		CH_DIAG_R1,			CH_DIAG_R1,			CH_LINE_BLD_ES,
	CH_LINE_BLD_WS,		CH_CIRCLE_1,		CH_VLINE_UP_1,		CH_MISC_HEART,		CH_VLINE_UP_2,		CH_LINE_RND_ES,		CH_DIAG_X,			CH_CIRCLE_3,		// 0x50
	CH_MISC_CLUB,		CH_VLINE_DN_2,		CH_MISC_DIA,		CH_VFILL_UP_1,		CH_BSLASH,			CH_DIAG_R3,			CH_CARET,			CH_LINE_WNES,
	CH_LSQUOTE,			CH_LINE_ES,			CH_BLOCK_NWSE,		CH_BLOCK_NE,		CH_BLOCK_SE,		CH_LINE_WNE,		CH_BLOCK_SW,		CH_HFILL_UP_1,		// 0x60
	CH_HFILL_UP_2,		CH_BLOCK_S,			CH_HFILL_UP_3,		CH_HFILL_UP_4,		CH_BLOCK_E,			CH_HFILL_DN_1,		CH_HFILL_DN_2,		CH_VFILL_UP_3,
	CH_VFILL_UP_2,		CH_LINE_NES,		CH_LINE_WES,		CH_LINE_WS,			CH_HLINE_UP_8,		CH_BLOCK_N,			CH_BLOCK_NW,		CH_LINE_WNS,		// 0x70
	CH_LINE_WN,			CH_HFILL_DN_2,		CH_LINE_NE,			CH_LINE_BLD_WN,		CH_PIPE,			CH_HLINE_UP_4,
};

// // template for remapping characters. do NOT modify, do not uncomment. Intention here is to make a copy, and replace-in-place the named chars.
// const static uint8_t dummy_full_charmap[256] = 
// {
// CH_MISC_GBP,		CH_VFILL_UP_2,		CH_VFILL_UP_3,		CH_BLOCK_S,			CH_VFILL_UP_5,		CH_VFILL_UP_6,		CH_VFILL_UP_7,		CH_VFILL_UP_8,		// 0x00
// CH_VFILL_DN_7,		CH_VFILL_DN_6,		CH_VFILL_DN_5,		CH_BLOCK_N,			CH_VFILL_DN_3,		CH_VFILL_DN_2,		CH_VLINE_UP_8,		CH_HDITH_1,
// CH_DITH_L1,			CH_HDITH_3,			CH_HDITH_4,			CH_HDITH_5,			CH_HDITH_6,			CH_DITH_L4,			CH_HDITH_8,			CH_DITH_L5,			// 0x10
// CH_HDITH_10,		CH_VDITH_1,			CH_VDITH_2,			CH_VDITH_3,			CH_VDITH_4,			CH_VDITH_5,			CH_DIAG_R2,			CH_DIAG_L2,
// CH_SPACE,			CH_BANG,			CH_DQUOTE,			CH_HASH,			CH_DOLLAR,			CH_PERCENT,			CH_AMP,				CH_SQUOTE,			// 0x20
// CH_LPAREN,			CH_RPAREN,			CH_AST,				CH_PLUS,			CH_COMMA,			CH_MINUS,			CH_PERIOD,			CH_FSLASH,
// CH_0,				CH_1,				CH_2,				CH_3,				CH_4,				CH_5,				CH_6,				CH_7,				// 0x30
// CH_8,				CH_9,				CH_COLON,			CH_SEMIC,			CH_LESS,			CH_EQUAL,			CH_GREATER,			CH_QUESTION,
// CH_AT,				CH_UC_A,			CH_UC_B,			CH_UC_C,			CH_UC_D,			CH_UC_E,			CH_UC_F,			CH_UC_G,			// 0x40
// CH_UC_H,			CH_UC_I,			CH_UC_J,			CH_UC_K,			CH_UC_L,			CH_UC_M,			CH_UC_N,			CH_UC_O,
// CH_UC_P,			CH_UC_Q,			CH_UC_R,			CH_UC_S,			CH_UC_T,			CH_UC_U,			CH_UC_V,			CH_UC_W,			// 0x50
// CH_UC_X,			CH_UC_Y,			CH_UC_Z,			CH_LBRACKET,		CH_BSLASH,			CH_RBRACKET,		CH_CARET,			CH_UNDER,
// CH_LSQUOTE,			CH_LC_A,			CH_LC_B,			CH_LC_C,			CH_LC_D,			CH_LC_E,			CH_LC_F,			CH_LC_G,			// 0x60
// CH_LC_H,			CH_LC_I,			CH_LC_J,			CH_LC_K,			CH_LC_L,			CH_LC_M,			CH_LC_N,			CH_LC_O,
// CH_LC_P,			CH_LC_Q,			CH_LC_R,			CH_LC_S,			CH_LC_T,			CH_LC_U,			CH_LC_V,			CH_LC_W,			// 0x70
// CH_LC_X,			CH_LC_Y,			CH_LC_Z,			CH_LCBRACKET,		CH_PIPE,			CH_RCBRACKET,		CH_TILDE,			CH_MISC_VTILDE,
// CH_HLINE_UP_7,		CH_HLINE_UP_6,		CH_LINE_NS,			CH_HLINE_UP_4,		CH_HLINE_UP_3,		CH_HLINE_UP_2,		CH_HFILL_UP_1,		CH_HFILL_UP_2,		// 0x80
// CH_HFILL_UP_3,		CH_BLOCK_W,			CH_HFILL_UP_5,		CH_HFILL_UP_6,		CH_HFILL_UP_7,		CH_HFILL_DN_7,		CH_HFILL_DN_6,		CH_HFILL_DN_5,
// CH_BLOCK_E,			CH_HFILL_DN_3,		CH_HFILL_DN_2,		CH_HLINE_UP_8,		CH_VLINE_UP_2,		CH_VLINE_UP_3,		CH_LINE_WE,			CH_VLINE_UP_5,		// 0x90
// CH_VLINE_UP_6,		CH_VLINE_UP_7,		CH_LINE_NES,		CH_LINE_WES,		CH_LINE_WNES,		CH_LINE_WNE,		CH_LINE_WNS,		CH_DIAG_X,
// CH_LINE_ES,			CH_LINE_WS,			CH_LINE_NE,			CH_LINE_WN,			CH_LINE_BLD_NES,	CH_LINE_BLD_WES,	CH_LINE_BLD_WNES,	CH_LINE_BLD_WNE,	// 0xA0
// CH_LINE_BLD_WNS,	CH_LINE_BLD_ES,		CH_LINE_BLD_WS,		CH_LINE_BLD_NE,		CH_LINE_BLD_WN,		CH_LINE_BLD_WE,		CH_LINE_BLD_NS,		CH_LINE_BLD_RND_ES,
// CH_LINE_BLD_RND_WS,	CH_LINE_BLD_RND_NE,	CH_LINE_BLD_RND_WN,	CH_CIRCLE_1,		CH_CIRCLE_3,		CH_RECT_3,			CH_CIRCLE_5,		CH_CIRCLE_6,		// 0xB0
// CH_DIAG_R3,			CH_DIAG_L3,			CH_DIAG_R1,			CH_DIAG_L1,			CH_LINE_RND_ES,		CH_LINE_RND_WS,		CH_LINE_RND_NE,		CH_LINE_RND_WN,
// CH_VFILLC_UP_1,		CH_VFILLC_UP_2,		CH_VFILLC_UP_3,		CH_VFILLC_UP_4,		CH_VFILLC_UP_5,		CH_VFILLC_UP_6,		CH_VFILLC_UP_7,		CH_VFILLC_UP_8,		// 0xC0
// CH_VFILLC_DN_7,		CH_VFILLC_DN_6,		CH_VFILLC_DN_5,		CH_VFILLC_DN_4,		CH_VFILLC_DN_3,		CH_VFILLC_DN_2,		CH_VFILLC_DN_1,		CH_HFILLC_UP_1,
// CH_HFILLC_UP_2,		CH_HFILLC_UP_3,		CH_HFILLC_UP_4,		CH_HFILLC_UP_5,		CH_HFILLC_UP_6,		CH_HFILLC_UP_7,		CH_HFILLC_DN_7,		CH_MISC_COPY,		// 0xD0
// CH_HFILLC_DN_6,		CH_HFILLC_DN_5,		CH_HFILLC_DN_4,		CH_HFILLC_DN_3,		CH_HFILLC_DN_2,		CH_HFILLC_DN_1,		CH_MISC_CHECKMARK,	CH_MISC_FOENIX,
// CH_RECT_4,			CH_CIRCLE_2,		CH_CIRCLE_4,		CH_RECT_2,			CH_DIAG_L4,			CH_DIAG_R4,			CH_DIAG_R8,			CH_DIAG_L8,			// 0xE0
// CH_DIAG_L5,			CH_DIAG_R5,			CH_DIAG_R9,			CH_DIAG_L9,			CH_DIAG_L6,			CH_DIAG_R6,			CH_DIAG_R10,		CH_DIAG_L10,
// CH_DIAG_L7,			CH_DIAG_R7,			CH_BLOCK_SE,		CH_BLOCK_SW,		CH_BLOCK_NE,		CH_BLOCK_NW,		CH_BLOCK_NWSE,		CH_BLOCK_SWNE,		// 0xF0
// CH_ARROW_DN,		CH_ARROW_LEFT,		CH_ARROW_RIGHT,		CH_ARROW_UP,		CH_MISC_HEART,		CH_MISC_DIA,		CH_MISC_SPADE,		CH_MISC_CLUB,
// };


// table of raw key codes (minus prefix) mapping to condensed modifier key
// this allows easier logic
const static uint8_t unified_modifier_keys[NUM_MODIFIER_KEY_BYTES] = 
{
	KEY_LSHIFT, KEY_UNIFIED_SHIFT,
	KEY_RSHIFT, KEY_UNIFIED_SHIFT,
	KEY_LCTRL, KEY_UNIFIED_CTRL,
	KEY_RCTRL, KEY_UNIFIED_CTRL,
	KEY_LALT, KEY_UNIFIED_ALT,
	KEY_RALT, KEY_UNIFIED_ALT,
	KEY_LMETA, KEY_UNIFIED_META,
	KEY_RMETA, KEY_UNIFIED_META,
	KEY_CAPS, KEY_UNIFIED_CAPS,
};

// table of extended 0 keys: code from PS2 and our key code. 
// does not include modifier keys, as those are already processed by unified_modifier_keys
const static uint8_t ps2_extended_keys[NUM_EXTENDED_KEY_BYTES] = 
{
	0x4a, KEY_KDIV,
	0x5a, KEY_KENTER,
	0x69, KEY_END,
	0x6b, KEY_CURS_LEFT,
	0x6c, KEY_HOME,
	0x70, KEY_INS,
	0x71, KEY_DEL,
	0x72, KEY_CURS_DOWN,
	0x74, KEY_CURS_RIGHT,
	0x75, KEY_CURS_UP,
	0x7a, KEY_PGDN,
	0x7d, KEY_PGUP,
};

// table of custom shifted chars (rather than a->A, b->B, etc.) 
const static uint8_t shift_combos[NUM_SHIFT_COMBO_BYTES] = 
{
	'1', '!',
	'2', '@',
	'3', '#',
	'4', '$',
	'5', '%',
	'6', '^',
	'7', '&',
	'8', '*',
	'9', '(',
	'0', ')',
	'-', '_',
	'=', '+',
	'[', '{',
	']', '}',
	0x5c, '|',
	';', ':',
	0x27, 0x22,
	',', '<',
	'.', '>',
	'/', '?',
	'`', '~',
	KEY_BKSP, KEY_DEL,
	KEY_F1, KEY_F2,
	KEY_F3, KEY_F4,
	KEY_F5, KEY_F6,
	KEY_F7, KEY_F8,
	KEY_BREAK, CH_TILDE,
};

// table of action keys mapped to character codes (so ESC produces 27 not 0x95, etc.)
const static uint8_t action_key_maps[NUM_ACTION_KEY_MAP_BYTES] = 
{
	KEY_BKSP, 	CH_BKSP,
	KEY_DEL, 	CH_DEL,
	KEY_ENTER,	CH_ENTER,
	KEY_TAB,	CH_TAB,
	KEY_ESC,	CH_ESC,
	KEY_BREAK,	CH_ESC,
};

// uint8_t		event_key_queue[KBD_INT_QUEUE_SIZE];
// uint8_t		event_key_queue_write_idx;
// uint8_t		event_key_queue_read_idx;

#ifdef _F256K_
	bool			kbd_initialized = false;
	bool			kbd_capslock_status;	// tracks status of capslock key for computers with built-in capslock keys. No relation to capslock on PS/2 keyboards.

	uint8_t			kbd_row_state[KBD_COLUMNS];		// for F256K internal keyboard, the tracked state of the 9 rows in keyboard matrix
	uint8_t			kbd_new_row_state[KBD_COLUMNS];		// for F256K internal keyboard, the current/new state of the 9 rows in keyboard matrix
	uint8_t			kbd_this_col_state;
	uint8_t			kbd_columns_eor;
	uint8_t			kbd_column;	

	const uint8_t	kbd_via_bit_table[8] = 
	{
		0b11111110,
		0b11111101,
		0b11111011,
		0b11110111,
		0b11101111,
		0b11011111,
		0b10111111,
		0b01111111,
	};

	const char kbd_256k_matrix[KBD_ROWS][KBD_COLUMNS] = {
		{KEY_DEL,CH_ENTER,KEY_CURS_LEFT,KEY_F7,KEY_F1,KEY_F3,KEY_F5,	KEY_CURS_UP,KEY_CURS_DOWN,},	// 0x00 - 0x08
		{'3',	'w',	'a',	'4',	'z',	's',	'e',	KEY_RSHIFT,0xff,},			// 0x09 - 0x11	// MB: this should be CH_LSHIFT but have conflict with 0. fix later.
		{'5',	'r',	'd',	'6',	'c',	'f',	't',	'x',	0xff,},				// 0x12 - 0x1a
		{'7',	'y',	'g',	'8',	'b',	'h',	'u',	'v',	0xff,},				// 0x1b - 0x23
		{'9',	'i',	'j',	'0',	'm',	'k',	'o',	'n',	0xff,},				// 0x24 - 0x2c
		{'-',	'p',	'l',	KEY_CAPS,'.',	';',	'[',	',',	0xff,},				// 0x2d - 0x35
		{'=',	']',	'\'',	KEY_BKSP,KEY_RSHIFT,KEY_LALT,CH_TAB,'/',	KEY_CURS_RIGHT,},	// 0x36 - 0x3e
		{'1',	CH_ESC,	KEY_LCTRL,'2',	' ',	KEY_LMETA,'q',	KEY_BREAK,0xff,},			// 0x3f - 0x47
	};


#endif


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

EventManager		global_event_manager_storage;
EventManager*		global_event_manager = &global_event_manager_storage;

extern uint8_t*			global_uart_in_buffer;
//extern uint8_t*			global_uart_in_buffer_cur_pos;
extern uint16_t			global_uart_write_idx;
extern uint16_t			global_uart_read_idx;

extern char* 			global_string_buff1;	// just need for debugging


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

//! Make the passed event a nullEvent, blanking out all fields
static void Event_SetNull(EventRecord* the_event);

// Process one key action from the interrupt, convert raw code, and turn it into an event
void Event_ProcessPS2KeyboardInterrupt(void);

#ifdef _F256K_

	// Process one key action from the F256K buil-in keyboard, convert raw code, and turn it into an event
	void Event_ProcessF256KKeyboardInterrupt(void);

#endif

/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

//! Make the passed event a nullEvent, blanking out all fields
static void Event_SetNull(EventRecord* the_event)
{
	the_event->what_ = EVENT_NULL;
	the_event->key_.modifiers_ = 0;
	the_event->key_.key_ = 0;
	the_event->key_.char_ = 0;
// 	the_event->code_ = 0L;
// 	the_event->when_ = 0L;
// 	the_event->window_ = NULL;
// 	the_event->control_ = NULL;
// 	the_event->x_ = -1;
// 	the_event->y_ = -1;
}


// Process one key from the interrupt, convert raw code, and turn it into an event
void Event_ProcessPS2KeyboardInterrupt(void)
{
	
	// PS/2 controller issues an 0xF0 key then a key code to tell you which key was released. eg, F01C means 'A' was released. 
	//   Some keys are always prefaced by an extra code, E0. (E114 is a one-off for PAUSE key). e.g, E075 is cursor up, and F0E075 means cursor up was released

	//sprintf(global_string_buff1, "%s %d: read idx=%x, write idx=%x", __func__, __LINE__, event_key_queue_read_idx, event_key_queue_write_idx);
	//Buffer_NewMessage(global_string_buff1);
	
	kbd_is_modifier_key = false;
	kbd_event_what = EVENT_KEYDOWN;
	kbd_modifiers = 0;
	
	if (kbd_raw_code != 0)
	{		
		if (kbd_raw_code == KBD_KEY_RELEASED)
		{
			//sprintf(global_string_buff1, "%s %d: kbd_raw_code=%x: prefix for a key release", __func__, __LINE__, kbd_raw_code);
			//Buffer_NewMessage(global_string_buff1);
			ps2_released = 1;
			return;
		}
		
		if (kbd_raw_code == KBD_EXT_PREFIX0)
		{
			//sprintf(global_string_buff1, "%s %d: kbd_raw_code=%x: prefix key 0", __func__, __LINE__, kbd_raw_code);
			//Buffer_NewMessage(global_string_buff1);
			ps2_e0 = 1;
			return;
		}
		
		if (kbd_raw_code == KBD_EXT_PREFIX1)
		{
			//sprintf(global_string_buff1, "%s %d: kbd_raw_code=%x: prefix key 1", __func__, __LINE__, kbd_raw_code);
			//Buffer_NewMessage(global_string_buff1);
			ps2_e1 = 1;
			return;
		}
		
		// if still here, it's either out of range or we need to map and return it
		if (kbd_raw_code >= KBD_OUT_OF_RANGE_START)
		{
			sprintf(global_string_buff1, "%s %d: kbd_raw_code=%x: out of range", __func__, __LINE__, kbd_raw_code);
			Buffer_NewMessage(global_string_buff1);
			kbd_mapped_char = 0;
		}
		else
		{
			// Handle keys based on prefixed code
			
			if (ps2_released)
			{
				// key kbd_raw_code was just released.
				// debug
				kbd_temp_char = keymap[kbd_raw_code];
				//sprintf(global_string_buff1, "%s %d: this key released: raw=%x, temp=%x", __func__, __LINE__, kbd_raw_code, kbd_temp_char);
				//Buffer_NewMessage(global_string_buff1);
				kbd_mapped_char = 0;
				kbd_event_what = EVENT_KEYUP;
			}
			
			if (ps2_e0 || ps2_e1)
			{
				if (ps2_e0)
				{
					//  Map keys prefixed with $e0
					
					for (kbd_j = 0; kbd_j < NUM_EXTENDED_KEY_BYTES; kbd_j=kbd_j+2)
					{
						if (ps2_extended_keys[kbd_j] == kbd_raw_code)
						{
							kbd_mapped_char = ps2_extended_keys[kbd_j+1];
							break;
						}
					}
					//sprintf(global_string_buff1, "%s %d: e0 active; kbd_raw_code=%x, mapped=%x", __func__, __LINE__, kbd_raw_code, kbd_mapped_char);
					//Buffer_NewMessage(global_string_buff1);
				}
				
				if (ps2_e1)
				{
					if (kbd_raw_code == 0x14)
					{
						kbd_mapped_char = KEY_PAUSE;
					}
					else
					{
						kbd_mapped_char = 0;
					}
					//sprintf(global_string_buff1, "%s %d: e1 active; kbd_raw_code=%x, mapped=%x", __func__, __LINE__, kbd_raw_code, kbd_mapped_char);
					//Buffer_NewMessage(global_string_buff1);
				}
			}
			else
			{
				// continue processing the key code
				
				// check if this was a modifier key by merging to simplified list of modifier keys (no left/right)
				for (kbd_j = 0; kbd_j < NUM_MODIFIER_KEY_BYTES; kbd_j=kbd_j+2)
				{
					if (unified_modifier_keys[kbd_j] == kbd_raw_code)
					{
						kbd_temp_char = unified_modifier_keys[kbd_j+1];
						
						// this was a modifier key being pressed or released: record that in the modifier table
						if (ps2_released > 0)
						{
							modifier_keys_pressed[kbd_temp_char] = 0;
						}
						else
						{
							modifier_keys_pressed[kbd_temp_char] = 1;
						}

						kbd_mapped_char = 0;
						kbd_is_modifier_key = true;
						//sprintf(global_string_buff1, "%s %d: modifier key detected, released=%u; kbd_raw_code=%x, modifier=%x", __func__, __LINE__, ps2_released, kbd_raw_code, kbd_temp_char);
						//Buffer_NewMessage(global_string_buff1);
						break;
					}
				}

				if (kbd_is_modifier_key == false)
				{
					// shifted vs special keys
					if (kbd_raw_code > 127)
					{
						// a few special keys up here (Function keys, etc.), use map to get right char code
						kbd_mapped_char = keymap[kbd_raw_code];
// 						sprintf(global_string_buff1, "%s %d: raw code > 127; kbd_raw_code=%x, mapped=%x", __func__, __LINE__, kbd_raw_code, kbd_mapped_char);
// 						Buffer_NewMessage(global_string_buff1);
					}
					else
					{
						// was not a special key
						kbd_temp_char = keymap[kbd_raw_code];
						
						// SHIFT?
						if (modifier_keys_pressed[KEY_UNIFIED_SHIFT])
						{
							if (kbd_temp_char >= 'a' && kbd_temp_char <= 'z')
							{
								kbd_temp_char -= 32;
							}
							else
							{
								for (kbd_j = 0; kbd_j < NUM_SHIFT_COMBO_BYTES; kbd_j=kbd_j+2)
								{
									if (shift_combos[kbd_j] == kbd_temp_char)
									{
										kbd_temp_char = shift_combos[kbd_j+1];
										break;
									}
								}
							}
// 							sprintf(global_string_buff1, "%s %d: shift was down; kbd_raw_code=%x, kbd_temp_char=%x", __func__, __LINE__, kbd_raw_code, kbd_temp_char);
// 							Buffer_NewMessage(global_string_buff1);
							
							kbd_modifiers |= KEY_FLAG_SHIFT;
						}
	
						// CONTROL?
						if (modifier_keys_pressed[KEY_UNIFIED_CTRL])
						{
							// CTRL for ASCII: project codes down to $00-$1F
							kbd_temp_char &= 0x1f;
// 							sprintf(global_string_buff1, "%s %d: control was down; kbd_raw_code=%x, kbd_temp_char=%x", __func__, __LINE__, kbd_raw_code, kbd_temp_char);
// 							Buffer_NewMessage(global_string_buff1);
							
							kbd_modifiers |= KEY_FLAG_CTRL;
						}
						
						// ALT?
						if (modifier_keys_pressed[KEY_UNIFIED_ALT])
						{
							// ALT for ASCII: set the high bit
							//kbd_raw_code *= 2;
							kbd_temp_char = kbd_temp_char | 0xF0;
// 							sprintf(global_string_buff1, "%s %d: alt was down; kbd_raw_code=%x, kbd_temp_char=%x", __func__, __LINE__, kbd_raw_code, kbd_temp_char);
// 							Buffer_NewMessage(global_string_buff1);
								
							kbd_modifiers |= KEY_FLAG_CTRL;
						}
						
						// Meta (Foenix)?
						if (modifier_keys_pressed[KEY_UNIFIED_META])
						{
							// Foenix/meta/windows key: do not modify mapped value, just set the modifier flag
							//  let calling program do whatever it needs to with that info
							kbd_modifiers |= KEY_FLAG_META;
						}
						
						kbd_mapped_char = kbd_temp_char;
// 						sprintf(global_string_buff1, "%s %d: done; kbd_raw_code=%x, kbd_temp_char=%x, mapped=%x", __func__, __LINE__, kbd_raw_code, kbd_temp_char, kbd_mapped_char);
// 						Buffer_NewMessage(global_string_buff1);
					}
				}
			}		
		}
		
		if (kbd_mapped_char)
		{
			//the_code = (kbd_raw_code << 8) | kbd_mapped_char;
// 			EventManager_AddEvent(kbd_event_what, kbd_raw_code, kbd_mapped_char, kbd_modifiers);
					
		// 	sprintf(global_string_buff1, "%s %d: reached; kbd_event_what=%i, raw=%x, mapped=%x", __func__, __LINE__, kbd_event_what, the_kbd_raw_code, the_mapped_code);
		// 	Buffer_NewMessage(global_string_buff1);
			
			kbd_event = &global_event_manager->queue_[global_event_manager->write_idx_++];
			global_event_manager->write_idx_ %= EVENT_QUEUE_SIZE;
			
			// LOGIC:
			//   Because an interrupt will (most likely) be the thing creating this event
			//   We accept only the minimum of info here and create the rest from that info
			kbd_event->what_ = kbd_event_what;
			kbd_event->key_.key_ = kbd_raw_code;
			kbd_event->key_.char_ = kbd_mapped_char;
			kbd_event->key_.modifiers_ = kbd_modifiers;			
		}

// 		// DEBUG: light up a spot on screen based on key being on or off
// 		for (i = 0; i < NUM_UNIFIED_MODIFIER_KEYS; i++)
// 		{
// 			R8(DEBUG_SHIFT_VRAM_LOC + i) = 48 + modifier_keys_pressed[i];
// 		}	
// 		
// 		R8(DEBUG_SHIFT_VRAM_LOC + i) = kbd_mapped_char;
		
		// clean up
		ps2_e0 = 0;
		ps2_e1 = 0;
		ps2_released = 0;
	}	
}


#ifdef _F256K_

// Process one key action from the F256K buil-in keyboard, convert raw code, and turn it into an event
void Event_ProcessF256KKeyboardInterrupt(void)
{
//	if (kbd_initialized != true) return;
	
	kbd_modifiers = 0;
	kbd_is_modifier_key = false;
	kbd_mapped_char = 0;
	kbd_temp_char = 0;
	kbd_raw_code = 0;
	
	R8(VIA1_PORT_A_DIRECTION) = 0x00;	// CIA#1 port A = outputs. We need every pin.
	R8(VIA1_PORT_B_DIRECTION) = 0xff;	// CIA#1 port B = inputs. clear all bits of port B (inputs)

	R8(VIA0_PORT_A_DIRECTION) = 0xff;
	R8(VIA0_PORT_B_DIRECTION) = 0;

	// scan and save the 8 columns of data available from VIA1
	for (kbd_i = 0; kbd_i < 8; kbd_i++)
	{
		R8(VIA1_PORT_B_DATA) = kbd_via_bit_table[kbd_i];
		kbd_new_row_state[kbd_i] = R8(VIA1_PORT_A_DATA);
	}
	
	// get the 9th column, from VIA0 
	// set VIA0 port B as output
	R8(VIA1_PORT_B_DIRECTION) = 0;		// try blocking this
	R8(VIA0_PORT_B_DIRECTION) = 0xff;
	
	// read keyboard row 8 on VIA0
	R8(VIA0_PORT_B_DATA) = 0b01111111;
	kbd_new_row_state[8] = R8(VIA1_PORT_A_DATA);	// bit6=cursor right, bit0=cursor down
	
	// reset VIA0 Port b to input
	R8(VIA0_PORT_B_DIRECTION) = 0;
	
	
	for (kbd_i = 0; kbd_i < KBD_COLUMNS; kbd_i++)
	{
		kbd_this_col_state = kbd_new_row_state[kbd_i];
		kbd_columns_eor = kbd_this_col_state ^ kbd_row_state[kbd_i];

		// debug
// 		R8(VICKY_TEXT_CHAR_RAM + 80 + i) = kbd_this_col_state;
// 		R8(VICKY_TEXT_CHAR_RAM + i) = kbd_row_state[i];
		
		if (kbd_columns_eor != 0)
		{
			kbd_column = 0;
			
			kbd_row_state[kbd_i] = kbd_this_col_state;
			
			while (kbd_columns_eor != 0)
			{
				if (kbd_columns_eor & 0x01)
				{			
					// Current key changed
					//kbd_raw_code = kbd_column * KBD_COLUMNS + kbd_i;
					kbd_raw_code = kbd_256k_matrix[kbd_column][kbd_i];
					
					if (kbd_this_col_state & 0x01)
					{
						kbd_event_what = EVENT_KEYUP;
					}
					else
					{
						kbd_event_what = EVENT_KEYDOWN;
					}
					
					if (kbd_raw_code != 0xff)
					{		
						//R8(VICKY_TEXT_CHAR_RAM + 325) = 32;
						kbd_is_modifier_key = false;
						
						// check if this was a modifier key by merging to simplified list of modifier keys (no left/right)
						for (kbd_j = 0; kbd_j < NUM_MODIFIER_KEY_BYTES; kbd_j=kbd_j+2)
						{
							if (unified_modifier_keys[kbd_j] == kbd_raw_code)
							{
								kbd_temp_char = unified_modifier_keys[kbd_j+1];
								//R8(VICKY_TEXT_CHAR_RAM + 325) = kbd_temp_char;
								
								// this was a modifier key being pressed or released: record that in the modifier table
								if (kbd_event_what == EVENT_KEYUP)
								{
									modifier_keys_pressed[kbd_temp_char] = 0;
								}
								else
								{
									modifier_keys_pressed[kbd_temp_char] = 1;
								}
		
								kbd_mapped_char = 0;
								kbd_is_modifier_key = true;
								//sprintf(global_string_buff1, "%s %d: modifier key detected, released=%u; kbd_raw_code=%x, modifier=%x", __func__, __LINE__, ps2_released, kbd_raw_code, kbd_temp_char);
								
								if (kbd_temp_char == KEY_UNIFIED_CAPS)
								{
									// LOGIC:
									//   capslock-specific light only controlled by capslock key when in a standard keymap. not in kana or PETSCII, e.g. 
									
									// turn capslock light on and off on the key down action. ignore key up.
									if (kbd_event_what == EVENT_KEYDOWN)
									{
										if (kbd_capslock_status == false)
										{
											kbd_capslock_status = true;
											
											if (kbd_map_mode == KEY_SETMAP_STD)
											{
												R8(SYS0_REG) = R8(SYS0_REG) | FLAG_SYS0_REG_W_CAP_EN;
											}
										}
										else if (kbd_capslock_status == true)
										{
											kbd_capslock_status = false;
											
											if (kbd_map_mode == KEY_SETMAP_STD)
											{
												R8(SYS0_REG) = R8(SYS0_REG) & (0xff - FLAG_SYS0_REG_W_CAP_EN);
											}
										}
									}
								}
								
								//Buffer_NewMessage(global_string_buff1);
								break;
							}
						}
						
						//R8(VICKY_TEXT_CHAR_RAM + 320) = kbd_raw_code;

						//R8(VICKY_TEXT_CHAR_RAM + 322) = 48 + kbd_i;
						//R8(VICKY_TEXT_CHAR_RAM + 323) = 48 + kbd_column;
						//R8(VICKY_TEXT_CHAR_RAM + 324) = kbd_this_col_state & 0x01;

						//R8(VICKY_TEXT_CHAR_RAM + 326) = kbd_is_modifier_key;
		
						if (kbd_is_modifier_key == false)
						{
							// was not a special key
							kbd_temp_char = kbd_raw_code;
							
							// CAPSLOCK was on? If so make lowercase uppercase, but only touch a-z chars.
							if (kbd_capslock_status == true)
							{
								if (kbd_temp_char >= 'a' && kbd_temp_char <= 'z')
								{
									kbd_temp_char -= 32;
								}
							}
							
							// SHIFT?
							if (modifier_keys_pressed[KEY_UNIFIED_SHIFT])
							{
								if (kbd_temp_char >= 'a' && kbd_temp_char <= 'z')
								{
									kbd_temp_char -= 32;
								}
								else
								{
									for (kbd_j = 0; kbd_j < NUM_SHIFT_COMBO_BYTES; kbd_j=kbd_j+2)
									{
										if (shift_combos[kbd_j] == kbd_temp_char)
										{
											kbd_temp_char = shift_combos[kbd_j+1];
											break;
										}
									}
								}
	// 							sprintf(global_string_buff1, "%s %d: shift was down; kbd_raw_code=%x, kbd_temp_char=%x", __func__, __LINE__, kbd_raw_code, kbd_temp_char);
	// 							Buffer_NewMessage(global_string_buff1);
								
								kbd_modifiers |= KEY_FLAG_SHIFT;
							}
		
							// CONTROL?
							if (modifier_keys_pressed[KEY_UNIFIED_CTRL])
							{
								// CTRL for ASCII: project codes down to $00-$1F
								kbd_temp_char &= 0x1f;
	// 							sprintf(global_string_buff1, "%s %d: control was down; kbd_raw_code=%x, kbd_temp_char=%x", __func__, __LINE__, kbd_raw_code, kbd_temp_char);
	// 							Buffer_NewMessage(global_string_buff1);
								
								kbd_modifiers |= KEY_FLAG_CTRL;
							}
							
							// ALT?
							if (modifier_keys_pressed[KEY_UNIFIED_ALT])
							{
								// ALT for ASCII: add 96. gets you access to most chars.
								//kbd_raw_code *= 2;
								kbd_temp_char = kbd_temp_char + CH_ALT_OFFSET;
	// 							sprintf(global_string_buff1, "%s %d: alt was down; kbd_raw_code=%x, kbd_temp_char=%x", __func__, __LINE__, kbd_raw_code, kbd_temp_char);
	// 							Buffer_NewMessage(global_string_buff1);
									
								kbd_modifiers |= KEY_FLAG_ALT;
							}
						
							// Meta (Foenix)?
							if (modifier_keys_pressed[KEY_UNIFIED_META])
							{
								// Foenix/meta/windows key: do not modify mapped value, just set the modifier flag
								//  let calling program do whatever it needs to with that info
								kbd_modifiers |= KEY_FLAG_META;
							}
							
							// did we just switch key layouts?
							if (kbd_temp_char == KEY_SETMAP_KANA && modifier_keys_pressed[KEY_UNIFIED_META])
							{
								kbd_map_mode = KEY_SETMAP_KANA;
								R8(LED_CAPSLOCK_BLUE) = KBD_MAP_KANA_LED_B;
								R8(LED_CAPSLOCK_GREEN) = KBD_MAP_KANA_LED_G;
								R8(LED_CAPSLOCK_RED) = KBD_MAP_KANA_LED_R;
								R8(SYS0_REG) = R8(SYS0_REG) | FLAG_SYS0_REG_W_CAP_EN;
								// switch to font set #2, which has JA font loaded
								R8(VICKY_MASTER_CTRL_REG_H) = (VICKY_RES_FON_SET);

								//R8(KEYBOARD_MODE_VRAM_LOC + 0) = kbd_kana_mode_label[0];
								//R8(KEYBOARD_MODE_VRAM_LOC + 1) = kbd_kana_mode_label[1];
							}
							else if (kbd_temp_char == KEY_SETMAP_STD && modifier_keys_pressed[KEY_UNIFIED_META])
							{
								kbd_map_mode = KEY_SETMAP_STD;
								R8(LED_CAPSLOCK_BLUE) = KBD_MAP_STD_LED_B;
								R8(LED_CAPSLOCK_GREEN) = KBD_MAP_STD_LED_G;
								R8(LED_CAPSLOCK_RED) = KBD_MAP_STD_LED_R;
								// make sure we are using font #1, not #2, which has JA font loaded
								R8(VICKY_MASTER_CTRL_REG_H) = 0;
								
								// for STD mode, only turn on LED if capslock mode is actually active
								if (kbd_capslock_status == true)
								{
									R8(SYS0_REG) = R8(SYS0_REG) | FLAG_SYS0_REG_W_CAP_EN;
								}
								else
								{
									R8(SYS0_REG) = R8(SYS0_REG) & (0xff - FLAG_SYS0_REG_W_CAP_EN);
								}
								//R8(KEYBOARD_MODE_VRAM_LOC + 0) = 0x07;	// inverse space
								//R8(KEYBOARD_MODE_VRAM_LOC + 1) = 0x07;	// inverse space
							}
							else if (kbd_temp_char == KEY_SETMAP_FOENISCII_1 && modifier_keys_pressed[KEY_UNIFIED_META])
							{
								kbd_map_mode = KEY_SETMAP_FOENISCII_1;
								R8(LED_CAPSLOCK_BLUE) = KBD_MAP_FOENISCII_1_LED_B;
								R8(LED_CAPSLOCK_GREEN) = KBD_MAP_FOENISCII_1_LED_G;
								R8(LED_CAPSLOCK_RED) = KBD_MAP_FOENISCII_1_LED_R;
								R8(SYS0_REG) = R8(SYS0_REG) | FLAG_SYS0_REG_W_CAP_EN;
								// make sure we are using font #1, not #2, which has JA font loaded
								R8(VICKY_MASTER_CTRL_REG_H) = 0;
								//R8(KEYBOARD_MODE_VRAM_LOC + 0) = 0x07;	// inverse space
								//R8(KEYBOARD_MODE_VRAM_LOC + 1) = CH_MISC_FOENIX;
							}
							else if (kbd_temp_char == KEY_SETMAP_FOENISCII_2 && modifier_keys_pressed[KEY_UNIFIED_META])
							{
								kbd_map_mode = KEY_SETMAP_FOENISCII_2;
								R8(LED_CAPSLOCK_BLUE) = KBD_MAP_FOENISCII_2_LED_B;
								R8(LED_CAPSLOCK_GREEN) = KBD_MAP_FOENISCII_2_LED_G;
								R8(LED_CAPSLOCK_RED) = KBD_MAP_FOENISCII_2_LED_R;
								R8(SYS0_REG) = R8(SYS0_REG) | FLAG_SYS0_REG_W_CAP_EN;
								// make sure we are using font #1, not #2, which has JA font loaded
								R8(VICKY_MASTER_CTRL_REG_H) = 0;
								//R8(KEYBOARD_MODE_VRAM_LOC + 0) = 0x07;	// inverse space
								//R8(KEYBOARD_MODE_VRAM_LOC + 1) = CH_MISC_FOENIX;
							}
							else if (kbd_temp_char == KEY_SETMAP_PETSCII && modifier_keys_pressed[KEY_UNIFIED_META])
							{
								kbd_map_mode = KEY_SETMAP_PETSCII;
								R8(LED_CAPSLOCK_BLUE) = KBD_MAP_PETSCII_LED_B;
								R8(LED_CAPSLOCK_GREEN) = KBD_MAP_PETSCII_LED_G;
								R8(LED_CAPSLOCK_RED) = KBD_MAP_PETSCII_LED_R;
								R8(SYS0_REG) = R8(SYS0_REG) | FLAG_SYS0_REG_W_CAP_EN;
								// make sure we are using font #1, not #2, which has JA font loaded
								R8(VICKY_MASTER_CTRL_REG_H) = 0;
								//R8(KEYBOARD_MODE_VRAM_LOC + 0) = 'C';	// C=. Maybe [F] box better?
								//R8(KEYBOARD_MODE_VRAM_LOC + 1) = '=';	//
							}
							else
							{
								// was this an action key? if not, check if remapping is needed
								
								if ( (kbd_raw_code >= KEY_F1 && kbd_raw_code <= KEY_ESC) ||
									(kbd_raw_code >= KEY_KENTER && kbd_raw_code <= KEY_PAUSE) )
								{
									// IS an action key: set flag so consuming routine knows not to use as glyph
									kbd_modifiers |= KEY_FLAG_ACTION;
									
									// do remapping where available
									kbd_mapped_char = kbd_temp_char;
									
									for (kbd_j = 0; kbd_j < NUM_ACTION_KEY_MAP_BYTES; kbd_j=kbd_j+2)
									{
										if (action_key_maps[kbd_j] == kbd_temp_char)
										{
											kbd_mapped_char = action_key_maps[kbd_j+1];
											break;
										}
									}
									
								}
								else
								{
									// not an action key: remap or use as is.
									
									// do remapping for other character / key layouts?
									if (kbd_map_mode == KEY_SETMAP_KANA)
									{
										if (kbd_temp_char >= FIRST_JIS_MAPPED_KEY && kbd_temp_char <= LAST_JIS_MAPPED_KEY)
										{
											kbd_temp_char -= FIRST_JIS_MAPPED_KEY;
											kbd_temp_char = ascii_2_jis[kbd_temp_char];
										}
									}
									else if (kbd_map_mode == KEY_SETMAP_FOENISCII_1)
									{
										if (kbd_temp_char >= FIRST_GRAPHIC_MAPPED_KEY && kbd_temp_char <= LAST_GRAPHIC_MAPPED_KEY)
										{
											kbd_temp_char -= FIRST_GRAPHIC_MAPPED_KEY;
											kbd_temp_char = ascii_2_foenscii_set1[kbd_temp_char];
										}
									}
									else if (kbd_map_mode == KEY_SETMAP_FOENISCII_2)
									{
										if (kbd_temp_char >= FIRST_GRAPHIC_MAPPED_KEY && kbd_temp_char <= LAST_GRAPHIC_MAPPED_KEY)
										{
											kbd_temp_char -= FIRST_GRAPHIC_MAPPED_KEY;
											kbd_temp_char = ascii_2_foenscii_set2[kbd_temp_char];
										}
									}
									else if (kbd_map_mode == KEY_SETMAP_PETSCII)
									{
										if (kbd_temp_char >= FIRST_GRAPHIC_MAPPED_KEY && kbd_temp_char <= LAST_GRAPHIC_MAPPED_KEY)
										{
											kbd_temp_char -= FIRST_GRAPHIC_MAPPED_KEY;
											kbd_temp_char = ascii_2_petscii[kbd_temp_char];
										}
									}
									
									kbd_mapped_char = kbd_temp_char;
								}
	
		// 						sprintf(global_string_buff1, "%s %d: done; kbd_raw_code=%x, kbd_temp_char=%x, mapped=%x", __func__, __LINE__, kbd_raw_code, kbd_temp_char, kbd_mapped_char);
		// 						Buffer_NewMessage(global_string_buff1);
							}
	
						}


						//R8(VICKY_TEXT_CHAR_RAM + 328) = kbd_mapped_char;
						//R8(VICKY_TEXT_CHAR_RAM + 329) = 48 + kbd_mapped_char;

						
						if (kbd_mapped_char)
						{
							//the_code = (kbd_raw_code << 8) | kbd_mapped_char;
				// 			EventManager_AddEvent(kbd_event_what, kbd_raw_code, kbd_mapped_char, kbd_modifiers);
							
						// 	sprintf(global_string_buff1, "%s %d: reached; kbd_event_what=%i, raw=%x, mapped=%x", __func__, __LINE__, kbd_event_what, the_kbd_raw_code, the_mapped_code);
						// 	Buffer_NewMessage(global_string_buff1);
							
							kbd_event = &global_event_manager->queue_[global_event_manager->write_idx_++];
							global_event_manager->write_idx_ %= EVENT_QUEUE_SIZE;
							
							// LOGIC:
							//   Because an interrupt will (most likely) be the thing creating this event
							//   We accept only the minimum of info here and create the rest from that info
							kbd_event->what_ = kbd_event_what;
							kbd_event->key_.key_ = kbd_raw_code;
							kbd_event->key_.char_ = kbd_mapped_char;
							kbd_event->key_.modifiers_ = kbd_modifiers;			
						}
				
						// DEBUG: light up a spot on screen based on key being on or off
						//for (kbd_i = 0; kbd_i < NUM_UNIFIED_MODIFIER_KEYS; kbd_i++)
						//{
						//	R8(DEBUG_SHIFT_VRAM_LOC + kbd_i) = 48 + modifier_keys_pressed[kbd_i];
						//}	
					}	
				}
		
				kbd_this_col_state = kbd_this_col_state >> 1;
				kbd_columns_eor = kbd_columns_eor >> 1;
				kbd_column++;
			}
		}		
	}
	
	
return;

	
// 	// handle down and right cursors, which are on VIA0, not VIA1 like all other keys
// 	
// * handle extra column here
//                     lda       VIA0.Base+VIA_ORB_IRB load A with VIA #0's port B
//                     rola                            rotate A to the left (hi bit goes in carry)
//                     rol       DownRightStates,u          rotate the carry into bit 0 of down/right state byte
//                     lda       VIA_ORB_IRB,x get the column value for this row
// 	
// JRA  =  $dc01  ; CIA#0 (Port Register A)
// JDRA =  $dc03  ; CIA#0 (Data Direction Register A)
// 
// JRB  =  $dc00  ; CIA#0 (Port Register B)
// JDRB =  $dc02  ; CIA#0 (Data Direction Register B)
// 
// PRA  =  $db01  ; CIA#1 (Port Register A)
// DDRA =  $db03  ; CIA#1 (Data Direction Register A)
// 
// PRB  =  $db00  ; CIA#1 (Port Register B)
// DDRB =  $db02  ; CIA#1 (Data Direction Register B)
// 	
// 	uint8_t			kbd_row_state[9];		// for F256K internal keyboard, the state of the 9 rows in keyboard matrix
// 	uint8_t			kbd_down_right;		// for F256K internal keyboard, the state of the down and right arrow keys during polling
// 	uint8_t			kbd_mask;				// for F256K internal keyboard, copy of VIA1 Port A during processing
// 	uint8_t			kbd_hold;				// for F256K internal keyboard, copy of VIA1 Port B during processing
// 	uint8_t			kbd_bit_num;			// for F256K internal keyboard, # of the col bit being processed
// 	uint8_t			kbd_up_or_down;		// for F256K internal keyboard, 1 for PRESSED, 0 for RELEASED
// 	uint8_t			kbd_joy0;				// for F256K internal keyboard, not sure, probably state of Joy 0
// 	uint8_t			kbd_joy1;				// for F256K internal keyboard, not sure, probably state of Joy 0
// 
//                     ldy       #D.RowState point to the row state global area
//                     lda       #%01111111 initialize the accumulator with the row scan value
//                     bsr       loop@
// * Handle down and right arrow
//                     sta       VIA_ORA_IRA,x store A in VIA #1's port A
//                     lda       DownRightStates,u  get the down/right state byte
//                     tfr       a,b save into B
//                     eora      ,y XOR A with the row state at Y
//                     bne       HandleRow if non-zero, either down/right changed positions -- go handle it
//                     rts       else return from the ISR
// loop@               sta       VIA_ORA_IRA,x save the row scan value to the VIA1 output
//                     pshs      a         save it on the stack for now
// * handle extra column here
//                     lda       VIA0.Base+VIA_ORB_IRB load A with VIA #0's port B
//                     rola                            rotate A to the left (hi bit goes in carry)
//                     rol       DownRightStates,u          rotate the carry into bit 0 of down/right state byte
//                     lda       VIA_ORB_IRB,x get the column value for this row
//                     tfr       a,b       save a copy to B
//                     eora      ,y        XOR with the last row state value
//                     beq       next@     branch if there's no change
//                     bsr       HandleRow else one or more keys for this row have changed direction - handle it
// next@               leay      1,y       advance to the next row state value
//                     puls      a         restore the row scan value we read from VIA1
//                     orcc      #Carry    set the carry flag so it will rotate in the upper 8 bits
//                     rora                rotate A to the right
//                     bcs       loop@     branch if the carry is set to continue
// ex@                 rts                 return to the caller			
}

#endif


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

__attribute__((interrupt(0xffea))) void nmi_handler()
{
	R8(VICKY_TEXT_CHAR_RAM + 4799) = R8(VICKY_TEXT_CHAR_RAM + 4799) + 1;

	#ifdef _F256K_	
		// treat RESTORE key as '\' as it would be on a "normal" keyboard
		// Event_ProcessF256KKeyboardInterrupt();
		// no point in calling the F256K keyboard handler, as RESTORE isn't wired into the VIA
		// we can assume if we're here, that a KEYDOWN event happened. Holding RESTORE down doesn't fire multiple NMI interrupts, just the one.

		kbd_event = &global_event_manager->queue_[global_event_manager->write_idx_++];
		global_event_manager->write_idx_ %= EVENT_QUEUE_SIZE;
		
		if (modifier_keys_pressed[KEY_UNIFIED_SHIFT] == true)
		{
			kbd_event->key_.char_ = '|';
		}
		else
		{
			kbd_event->key_.char_ = '\\';
		}
		
		kbd_event->what_ = EVENT_KEYDOWN;
		kbd_event->key_.key_ = 0x38;	// spot RESTORE is mapped to in kbd_256k_matrix
		kbd_event->key_.modifiers_ = 0;			
		
	#endif
}


__attribute__((interrupt(0xffee))) void irq_handler()
{
	
	// DEBUG: increment first vis char everytime this handler is hit
	//R8(VICKY_TEXT_CHAR_RAM + 80) = R8(VICKY_TEXT_CHAR_RAM + 80) + 1; 

	pending_int_value = R8(INT_PENDING_REG0);

	// is this interrupt firing because of interrupt group 0?
	if ( pending_int_value )
	{
		// Check for P/S2 keyboard flag
		if ( (pending_int_value & JR0_INT02_KBD) != 0)
		{
			// this is a PS/2 keyboard interrupt
			
			// clear pending flag before doing any work
			R8(INT_PENDING_REG0) = pending_int_value;

			//R8(VICKY_TEXT_CHAR_RAM + 162) = R8(VICKY_TEXT_CHAR_RAM + 162) + 1; 
			
			// clear the pending flag so it doesn't show up again
			// think this works because manual says "When writing to the register, setting a flag will clear the pending status of the interrupt."
			// R8(INT_PENDING_REG0) = pending_int_value; // not sure it matters what we write, but this is what manual example does.

			 // check if there is a key in the queue
			 if ((R8(VICKY_PS2_STATUS) & VICKY_PS2_STATUS_FLAG_KEMP) == 0)
			 {
				//pending_int_value = R8(VICKY_PS2_KDB_IN);
				// debug: write char to screen in 2nd position
				//R8(VICKY_TEXT_CHAR_RAM + 1) = pending_int_value;
				kbd_raw_code = R8(VICKY_PS2_KDB_IN);
				Event_ProcessPS2KeyboardInterrupt();
				
				//event_key_queue[event_key_queue_write_idx++] = R8(VICKY_PS2_KDB_IN);
	
				//event_key_queue_idx %= EVENT_QUEUE_SIZE;  <-- does a long jump, seems to freeze up machine from interrupt. go simpler...
	// 		 	if (event_key_queue_idx > EVENT_QUEUE_SIZE)
	// 		 	{
	// 		 		event_key_queue_idx = 0;
	// 		 	}
				// above also caused freeze. switched queue size to 256 so that I can just let the idx roll over
			 }
		}
		else
		{
			// event was not PS/2. will assume it was SOF, and do an F256K keyboard check
	
			#ifdef _F256K_
			
				if ( (pending_int_value & JR0_INT00_SOF) != 0)
				{
					// clear pending flag before doing any work
					R8(INT_PENDING_REG0) = pending_int_value;
	
					if (kbd_initialized == true)
					{
						Event_ProcessF256KKeyboardInterrupt();
					}
				}
				else
				{
					// don't know what this is, but need to clear the pending flag
					R8(INT_PENDING_REG0) = pending_int_value;
				}

			#else
				// don't know what this is, but need to clear the pending flag
				R8(INT_PENDING_REG0) = pending_int_value;
			#endif
	
		}		
	}
	
	pending_int_value = R8(INT_PENDING_REG1);
	
	// is this interrupt firing because of interrupt group 2?
	if (pending_int_value)
	{
		//R8(VICKY_TEXT_CHAR_RAM + 80) = R8(VICKY_TEXT_CHAR_RAM + 80) + 1; 

		// clear pending flag before doing any work
		R8(INT_PENDING_REG1) = pending_int_value;
		
		// is this interrupt firing because of UART serial activity?
		if ( (pending_int_value & JR1_INT00_UART) != 0)
		{	
			serial_temp = (R8(UART_LSR) & UART_ERROR_MASK);
			
			if (serial_temp > 0)
			{
				sprintf(global_string_buff1, "serial error %x", serial_temp);
				Buffer_NewMessage(global_string_buff1);
				// clear error by reading the data register. (I think that's supposed to work to clear errors anyway)
				// Read and clear status registers
				serial_temp = R8(UART_LSR);
				serial_temp = R8(UART_MSR);
				serial_temp = R8(UART_BASE);
			}
			else
			{
				while ( (R8(UART_LSR) & UART_DATA_AVAILABLE) > 0)
				{
					global_uart_in_buffer[global_uart_write_idx++] = R8(UART_BASE);
					global_uart_write_idx %= UART_BUFFER_SIZE;
				}
			}
		}
	}
}




// **** Queue Management functions *****

//! Checks to see if there is an event in the queue
//! returns NULL if no event (not the same as returning an event of type nullEvent)
EventRecord* EventManager_NextEvent(void)
{
	EventRecord*	the_event;
	
	// LOGIC:
	//   the event buffer is circular. nullEvents are allowed and present.
	//   so the way to know if there is a waiting event is to compare the read and write indices
	//   if read=write, then there are no pending events
	
	if (global_event_manager->read_idx_ == global_event_manager->write_idx_)
	{
		DEBUG_OUT(("%s %d: read_idx_=%i SAME AS write_idx_=%i", __func__, __LINE__, global_event_manager->read_idx_, global_event_manager->write_idx_));
		//sprintf(global_string_buff1, "%s %d: read_idx_=%i SAME AS write_idx_=%i", __func__, __LINE__, global_event_manager->read_idx_, global_event_manager->write_idx_);
		//Buffer_NewMessage(global_string_buff1);
		return NULL;
	}
	
	the_event = &global_event_manager->queue_[global_event_manager->read_idx_];

	//DEBUG_OUT(("%s %d: Next Event: type=%i", __func__, __LINE__, the_event->what_));
// 	sprintf(global_string_buff1, "%s %d: Next Event: type=%i", __func__, __LINE__, the_event->what_);
// 	Buffer_NewMessage(global_string_buff1);
	//EventManager_Print(global_event_manager);
	//Event_Print(the_event);
	
	global_event_manager->read_idx_++;
	global_event_manager->read_idx_ %= EVENT_QUEUE_SIZE;
	
	if (the_event->what_ == EVENT_NULL)
	{
		DEBUG_OUT(("%s %d: event was null. read_idx_=%i, write_idx_=%i", __func__, __LINE__, global_event_manager->read_idx_, global_event_manager->write_idx_));
// 		sprintf(global_string_buff1, "%s %d: event was null. read_idx_=%i, write_idx_=%i", __func__, __LINE__, global_event_manager->read_idx_, global_event_manager->write_idx_);
// 		Buffer_NewMessage(global_string_buff1);
		return NULL;
	}

	//DEBUG_OUT(("%s %d: read_idx_=%i, read_idx_ mod EVENT_QUEUE_SIZE=%i", __func__, __LINE__, global_event_manager->read_idx_, global_event_manager->read_idx_ % EVENT_QUEUE_SIZE));
	//DEBUG_OUT(("%s %d: exiting; event what=%i (%p), read_idx_=%i, write_idx_=%i", __func__, __LINE__, the_event->what_, the_event, global_event_manager->read_idx_, global_event_manager->write_idx_));
	
	return the_event;
}


//! Add a new event to the event queue
//! NOTE: this does not actually insert a new record, as the event queue is a circular buffer
//! It overwrites whatever slot is next in line
//! @param	the_window: this may be set for non-mouse up/down events. For mouse up/down events, it will not be set, and X/Y will be used to find the window.
// NOTE: , Window* the_window, Control* the_control removed for F256K2 implementation as we aren't doing windows (yet?)
void EventManager_AddEvent(event_kind the_what, uint8_t the_kbd_raw_code,  uint8_t the_mapped_code, event_modifiers the_modifiers)
{
	EventRecord*	the_event;

	//DEBUG_OUT(("%s %d: reached; the_what=%i, the_code=%i, x=%i, y=%i, the_window=%p", __func__, __LINE__, the_what, the_code, x, y, the_window));
	DEBUG_OUT(("%s %d: reached; the_what=%i, raw=%x, mapped=%x", __func__, __LINE__, the_what, the_kbd_raw_code, the_mapped_code));
// 	sprintf(global_string_buff1, "%s %d: reached; the_what=%i, raw=%x, mapped=%x", __func__, __LINE__, the_what, the_kbd_raw_code, the_mapped_code);
// 	Buffer_NewMessage(global_string_buff1);
	
	the_event = &global_event_manager->queue_[global_event_manager->write_idx_++];
	global_event_manager->write_idx_ %= EVENT_QUEUE_SIZE;
	
	if (the_what == EVENT_NULL)
	{
		DEBUG_OUT(("%s %d: null event added", __func__, __LINE__));
		Event_SetNull(the_event);
		return;
	}
	
	// LOGIC:
	//   Because an interrupt will (most likely) be the thing creating this event
	//   We accept only the minimum of info here and create the rest from that info
	the_event->what_ = the_what;
	the_event->key_.modifiers_ = the_modifiers;
	the_event->key_.key_ = the_kbd_raw_code;
	the_event->key_.char_ = the_mapped_code;
//	the_event->code_ = the_code;
// 	the_event->when_ = sys_time_jiffies();
// 	the_event->x_ = x;
// 	the_event->y_ = y;
// 	the_event->window_ = the_window;
// 	the_event->control_ = the_control;
	
	// check for a window and a control, using x and y
	// LOGIC:
	//   do not try to find window for a disk event
	//   do not try to find window/control for a control clicked event, it will already have been set
	//   for mouseup/down find window based on cursor pos (if window not already set, which it might have been)
	//   for all other events, if window hadn't been set, set it to active window.
	
// 	if (the_what == diskEvt || the_what == controlClicked)
// 	{
// 		// twiddle thumbs
// 	}
// 	else if (the_what == mouseDown || the_what == mouseUp)
// 	{
// 		// might already have been set, if interrupt isn't what generated the event
// 		if (the_event->window_ == NULL)
// 		{
// 			the_event->window_ = Sys_GetWindowAtXY(global_system, x, y);
// 		}
// 	}
// 	else if (the_event->window_ == NULL)
// 	{
// 		the_event->window_ = Sys_GetActiveWindow(global_system);
// 	}	
}


//! Wait for an event to happen, do system-processing of it, then if appropriate, give the window responsible for the event a chance to do something with it
void EventManager_WaitForEvent(void)
{
	EventRecord*	the_event;
	
	//DEBUG_OUT(("%s %d: write_idx_=%i, read_idx_=%i, the_mask=%x", __func__, __LINE__, the_event_manager->write_idx_, the_event_manager->read_idx_, the_mask));

	// TESTING: if no events in queue, add one to prime pump
// 	if (the_event_manager->write_idx_ == the_event_manager->read_idx_)
// 	{
// 		EventManager_AddEvent(EVENT_KEYDOWN, 65, -1, -1, 0L, NULL, NULL);
// 		EventManager_AddEvent(EVENT_KEYUP, 65, -1, -1, 0L, NULL, NULL);
// 		EventManager_GenerateRandomEvent();
// 	}
	
	// now process the queue as if it were happening in real time
// 	
// 	the_event = EventManager_NextEvent();
// 	DEBUG_OUT(("%s %d: first event: %i", __func__, __LINE__, the_event->what_));
// 	

	while ( (the_event = EventManager_NextEvent()) != NULL)
	{
// 		MouseMode		starting_mode;
// 		Window*			the_active_window;
		
		DEBUG_OUT(("%s %d: Received Event Event: type=%i", __func__, __LINE__, the_event->what_));
		//Event_Print(the_event);
		
// 		starting_mode = Mouse_GetMode(the_event_manager->mouse_tracker_);
		
		// LOGIC:
		//   event could be for:
		//   1. a mouse event. Will sort out non-app window click vs main window click vs about window click in the specific handler
		//   2. an update event. need to detect and route for main window vs about window
		//   3. an activate event. need to detect and route for main window vs about window
		//   4. a keyboard event. need to check for menu shortcuts and activate menus. no other keyboard input needed.

		switch (the_event->what_)
		{
			case EVENT_NULL:
				DEBUG_OUT(("%s %d: null event", __func__, __LINE__));
				break;
		
// 			case mouseMoved:				
// 				DEBUG_OUT(("%s %d: mouse move event (%i, %i)", __func__, __LINE__, the_event->x_, the_event->y_));
// 
// 				EventManager_HandleMouseMoved(the_event_manager, the_event);
// 	
// 				break;
// 				
// 			case mouseDown:				
// 				DEBUG_OUT(("%s %d: mouse down event (%i, %i)", __func__, __LINE__, the_event->x_, the_event->y_));
// 
// 				EventManager_HandleMouseDown(the_event_manager, the_event);
// 				
// 				break;
// 
// 			case mouseUp:
// 				DEBUG_OUT(("%s %d: mouse up event (%i, %i)", __func__, __LINE__, the_event->x_, the_event->y_));
// 
// 				EventManager_HandleMouseUp(the_event_manager, the_event);
// 				
// 				break;
// 
// 			case rMouseDown:				
// 				DEBUG_OUT(("%s %d: right mouse down event (%i, %i)", __func__, __LINE__, the_event->x_, the_event->y_));
// 
// 				EventManager_HandleRightMouseDown(the_event_manager, the_event);
// 				
// 				break;
// 
// 			case rMouseUp:
// 				DEBUG_OUT(("%s %d: right mouse up event (%i, %i)", __func__, __LINE__, the_event->x_, the_event->y_));
// 
// 				// eat this event: we don't care about right mouse up: menu events are designed to fire on right mouse DOWN
// 				
// 				break;
// 
// 			case menuOpened:
// 				DEBUG_OUT(("%s %d: menu opened event: %c", __func__, __LINE__, the_event->code_));
// 				// give window an event
// 				(*the_event->window_->event_handler_)(the_event);
// 		
// 				break;
// 				
// 			case menuSelected:
// 				DEBUG_OUT(("%s %d: menu item selected event: %c", __func__, __LINE__, the_event->code_));
// 				// give window an event
// 				(*the_event->window_->event_handler_)(the_event);
// 		
// 				break;
// 				
// 			case controlClicked:
// 				DEBUG_OUT(("%s %d: control clicked event: %c", __func__, __LINE__, the_event->code_));
// 				// give window an event
// 				(*the_event->window_->event_handler_)(the_event);
// 		
// 				break;
				
// 			case EVENT_KEYDOWN:
// 				DEBUG_OUT(("%s %d: key down event: '%c' (%x) mod (%x)", __func__, __LINE__, the_event->code_, the_event->code_, the_event->modifiers_));
// 
// 				// give active window an event
// 				the_active_window = Sys_GetActiveWindow(global_system);
// 
// 				(*the_active_window->event_handler_)(the_event);				
// 
// 				break;
// 			
// 			case EVENT_KEYUP:
// 				DEBUG_OUT(("%s %d: key up event: '%c' (%x) mod (%x)", __func__, __LINE__, the_event->code_, the_event->code_, the_event->modifiers_));
// 
// 				// give active window an event
// 				the_active_window = Sys_GetActiveWindow(global_system);
// 
// 				(*the_active_window->event_handler_)(the_event);				
// 
// 				break;

// 			case updateEvt:
// 				DEBUG_OUT(("%s %d: updateEvt event", __func__, __LINE__));
// 
// 				// give window an event
// 				(*the_event->window_->event_handler_)(the_event);				
// 
// // 				// Get a RSS surface pointer from the message
// // 				//  LOGIC: the_window will be 0 in some cases
// // 				//    for activateEvt and updateEvt, message will be WindowPtr
// // 				//    (see EventRecord)
// // 			
// // 				affected_app_window = (RSSWindow*)GetWRefCon((WindowPtr)the_event->message);
// // 
// // 				if (affected_app_window == global_app->main_window_)
// // 				{
// // 					the_surface = (RSSWindow*)affected_app_window;
// // 					Window_HandleUpdateEvent(the_surface, the_event);
// // 				}
// // 				else
// // 				{
// // 					// maybe it was an About window that got the event? 	
// // 					if (affected_app_window == global_app->about_window_)
// // 					{
// // 						the_about_window = (RSSAboutWindow*)affected_app_window;
// // 						AboutWindow_HandleUpdateEvent(the_about_window, the_event);
// // 					}
// // 				}
// // 			
// // 				return kUIMsgTypeNonRelevant;
// 				break;
			
// 			case activateEvt:
// 				DEBUG_OUT(("%s %d: activateEvt event", __func__, __LINE__));
// 
// 				// give window an event
// 				(*the_event->window_->event_handler_)(the_event);				
// 
// 				
// 				// tell window to make its active_ state
// 				//Window_SetActive(the_event->window_, true); // now this is done as part of Sys_SetActiveWindow
// 
// 				// do what, exactly? 
// 				//Sys_SetActiveWindow(global_system, the_event->window_); // don't set here, set in the mouse down thing that caused it
// 				//Sys_Render(global_system);
// 
// 				//Sys_SetActiveWindow(global_system, the_event->window_);
// 				//DEBUG_OUT(("%s %d: **** changed active window to = '%s'; redrawing all windows", __func__, __LINE__, the_event->window_));
// 				
// 				
// 				// need to make system move the window to the front
// 				// but also need to give app a chance to get this activate event before system does that. 
// 				//Window_Activate(the_event->window_);
// 				
// 				break;
// 			
// 			case inactivateEvt:
// 				DEBUG_OUT(("%s %d: inactivateEvt event", __func__, __LINE__));
// 				// do what, exactly? 
// 				//Sys_SetActiveWindow(global_system, the_event->window_);
// 				
// 				// tell window to make its active_ state
// 				Window_SetActive(the_event->window_, false); // now this is done as part of Sys_SetActiveWindow
// 				//Sys_Render(global_system);
// 
// 				// give window an event
// 				(*the_event->window_->event_handler_)(the_event);				
// 
// 				
// 				// need to make system move the window to the front
// 				// but also need to give app a chance to get this activate event before system does that. 
// 				//Window_Inactivate(the_event->window_);
// 				
// 				break;
// 
// 			case windowChanged:
// 				DEBUG_OUT(("%s %d: windowChanged event", __func__, __LINE__));
// 
// 				// give window an event
// 				(*the_event->window_->event_handler_)(the_event);				
// 
// 				break;
			
			default:
				DEBUG_OUT(("%s %d: other event: %i", __func__, __LINE__, the_event->what_));
				
				break;
		}
		
		//DEBUG_OUT(("%s %d: r idx=%i, w idx=%i, meets_mask will be=%x", __func__, __LINE__, the_event_manager->write_idx_, the_event_manager->read_idx_, the_event->what_ & the_mask));
		
		//getchar();	
		//General_DelayTicks(5);
	}
	
	return;
}


#ifdef _F256K_

	// get the built-in keyboard on the F256K ready
	void Event_InitalizeKeyboard256K(void)
	{
		uint8_t		i;

		kbd_initialized = false;
		kbd_map_mode = KEY_SETMAP_STD;
		
		// set Capslock LED light color
		R8(LED_CAPSLOCK_BLUE) = KBD_MAP_STD_LED_B;
		R8(LED_CAPSLOCK_GREEN) = KBD_MAP_STD_LED_G;
		R8(LED_CAPSLOCK_RED) = KBD_MAP_STD_LED_R;
		
		// turn the capslock light off
		R8(SYS0_REG) = R8(SYS0_REG) & (0xff - FLAG_SYS0_REG_W_CAP_EN);		
		kbd_capslock_status = false;		

		// Initialize the keyboard status to nothing pressed
		for (i = 0; i < KBD_MATRIX_SIZE; i++)
		{
			kbd_row_state[i] = 0xff;
		}
		
		// set up VIA1
		R8(VIA1_PORT_A_DIRECTION) = 0xff;	// CIA#1 port A = outputs. We need every pin.
		R8(VIA1_PORT_A_DATA) = 0xff;		// and set the corresponding values to 1		

		R8(VIA1_PORT_B_DIRECTION) = 0;		// CIA#1 port B = inputs. clear all bits of port B (inputs)
		R8(VIA1_PORT_B_DATA) = 0;			// set the corresponding values to 0

		R8(VIA1_AUX_CONTROL) = 0;
		R8(VIA1_PERI_CONTROL) = 0;
		R8(VIA1_INT_ENABLE) = 0;			// clear interrupts

		// set up VIA0 - only need 1 keyboard column from here
		R8(VIA0_PORT_A_DIRECTION) = 0b01111111;	// output: pull-ups to bits 0-6, 7 hi-z (not used).
		R8(VIA0_PORT_A_DATA) = 0b01111111;	// set the corresponding values to 1. pull-ups for 0-6.

		R8(VIA0_PORT_B_DIRECTION) = 0;		// clear all bits of port B (inputs)
		R8(VIA0_PORT_B_DATA) = 0;			// set the corresponding values to 0

		R8(VIA0_AUX_CONTROL) = 0;
		R8(VIA0_PERI_CONTROL) = 0;
		R8(VIA0_INT_ENABLE) = 0;			// clear interrupts

		
		// LOGIC:
		//   F256K keyboard does not fire an IRQ event for key actions
		//   you need to set up a timer to fire, and then check the VIA for what keys might be down or up during the interrupt
		
		// configure a VICKY interrupt based on raster line
		R8(VICKY_LINE_INT_LINE_L) = 0;	// will fire on line 0
		R8(VICKY_LINE_INT_LINE_H) = 0;
// 		R8(VICKY_LINE_INT_CTRL) = 1;	// enable it
		
		// turn on the SOF interrupt (PS/2 keyboard already on, so don't turn it off)
// 		__asm(	" lda #0xff\n"
// 				" sec\n"
// 				" sbc #0x04\n"
// 				" sbc #0x01\n"
// 				" sta long: 0xF0166C\n"
// 				);				

// 		__asm(	" lda #0xfa\n"
// 				" sta long: 0xF0166C\n"
// 				);				
				
		// JR0_INT02_KBD = 0x04
		// JR0_INT00_SOF = 0x01
		// INT_MASK_REG0 = 0xF0166C

		kbd_initialized = true;
		
		R8(VICKY_LINE_INT_CTRL) = 1;	// enable it
		
	}


#endif
