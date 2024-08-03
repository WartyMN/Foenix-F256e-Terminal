//! @file keyboard.h

/*
 * keyboard.h
 *
 *  Created on: Aug 23, 2022
 *      Author: micahbly
 */

// adapted for (Lich King) Foenix F256 Jr starting November 30, 2022
// adapted for f/manager Foenix F256 starting March 10, 2024
// Adapted from F256jr/k version of FileManager starting June 2, 2024


#ifndef KEYBOARD_H_
#define KEYBOARD_H_


/* about this class
 *
 *
 *
 *** things this class needs to be able to do
 * get a char code back from the keyboard when requested
 * interpret MCP scan codes, use modifiers to map scan codes to "char codes" defined here
 *
 *** things objects of this class have
 *
 *
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/


// C includes
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

// key definitions, which are not necessarily the same as the character that should be displayed if key is processed

#define KEY_UNIFIED_SHIFT     	0x00	
#define KEY_UNIFIED_ALT     	0x01	
#define KEY_UNIFIED_CTRL     	0x02	
#define KEY_UNIFIED_META     	0x03	
#define KEY_UNIFIED_CAPS     	0x04	

#define KEY_LSHIFT     	0x00	
#define KEY_RSHIFT     	0x01	
#define KEY_LCTRL     	0x02	
#define KEY_RCTRL     	0x03	
#define KEY_LALT     	0x04	
#define KEY_RALT     	0x05	
#define KEY_LMETA     	0x06	
#define KEY_RMETA     	0x07	
#define KEY_CAPS     	0x08	

#define KEY_POWER  		0x80	
#define KEY_F1      	0x81	
#define KEY_F2      	0x82	
#define KEY_F3      	0x83	
#define KEY_F4      	0x84	
#define KEY_F5      	0x85	
#define KEY_F6      	0x86	
#define KEY_F7      	0x87	
#define KEY_F8     		0x88	
#define KEY_F9      	0x89	
#define KEY_F10     	0x8a	
#define KEY_F11     	0x8b	
#define KEY_F12    		0x8c	
#define KEY_F13    		0x8d	
#define KEY_F14    		0x8e	
#define KEY_F15    		0x8f	
#define KEY_F16    		0x90
#define KEY_DEL    		0x91
#define KEY_BKSP   		0x92
#define KEY_TAB    		0x93
#define KEY_ENTER  		0x94
#define KEY_ESC    		0x95

#define KEY_K0      	0xa0	
#define KEY_K1      	0xa1	
#define KEY_K2      	0xa2		
#define KEY_K3      	0xa3		
#define KEY_K4      	0xa4		
#define KEY_K5      	0xa5		
#define KEY_K6     		0xa6		
#define KEY_K7  		0xa7	
#define KEY_K8  		0xa8	
#define KEY_K9    		0xa9	
#define KEY_KPLUS    	0xaa
#define KEY_KMINUS    	0xab
#define KEY_KTIMES   	0xac
#define KEY_KDIV   		0xad
#define KEY_KPOINT  	0xae
#define KEY_KENTER   	0xaf
#define KEY_NUM		   	0xb0

#define KEY_PGUP	   	0xb1
#define KEY_PGDN	   	0xb2
#define KEY_HOME	   	0xb3
#define KEY_END		   	0xb4
#define KEY_INS		   	0xb5
#define KEY_CURS_UP	   	0xb6
#define KEY_CURS_DOWN  	0xb7
#define KEY_CURS_LEFT  	0xb8
#define KEY_CURS_RIGHT 	0xb9
#define KEY_SCROLL	   	0xba
#define KEY_SYSREQ	   	0xbb
#define KEY_BREAK	   	0xbc
#define KEY_SLEEP	   	0xbd
#define KEY_WAKE	   	0xbe
#define KEY_PRTSCR	   	0xbf
#define KEY_MENU	   	0xc0
#define KEY_PAUSE	   	0xc1

// "character" definitions for each key. Might be how it gets rendered, but can also just be an ID#

#define CH_ALT_OFFSET		96	// this is amount that will be added to key press if ALT is held down.
// #define CH_LSHIFT     	0x00	
// #define CH_RSHIFT     	0x01	
// #define CH_LCTRL     	0x02	
// #define CH_RCTRL     	0x03	
// #define CH_LALT     	0x04	
// #define CH_RALT     	0x05	
// #define CH_LMETA     	0x06	
// #define CH_RMETA     	0x07	
// #define CH_CAPS     	0x08	
// 
// #define CH_CURS_UP      0x10
// #define CH_CURS_DOWN    0x0e
// #define CH_CURS_LEFT    0x02
// #define CH_CURS_RIGHT   0x06
// #define CH_DEL          0x04
#define CH_LF    	    10
#define CH_FF			12
#define CH_ENTER        13
#define CH_ESC          27
#define CH_TAB          9
#define CH_COPYRIGHT	215
#define CH_BKSP   		0x08	
#define CH_DEL   		0x7F	// 127
#define CH_MENU			0xFF	// no particular reason for selecting FF.

#define CH_K0      		'0'	
#define CH_K1      		'1'	
#define CH_K2      		'2'		
#define CH_K3      		'3'		
#define CH_K4      		'4'		
#define CH_K5      		'5'		
#define CH_K6     		'6'		
#define CH_K7  			'7'		
#define CH_K8  			'8'		
#define CH_K9    		'9'		
#define CH_KENTER   	(CH_ENTER)	
#define CH_KPLUS    	'+'	
#define CH_KMINUS    	'-'	
#define CH_KTIMES   	'*'	
#define CH_KDIV   		'/'	
#define CH_KPOINT  		'.'	
// #define CH_NUM    		0x81	
// #define CH_SCROLL    	0x81	
// #define CH_SYSREQ    	0x81	
// #define CH_HOME    		0x01	
// #define CH_END    		0x05
// #define CH_INS			KEY_INS
// #define CH_PGUP    		0x1A	
// #define CH_PGDN    		0x16	

// Standard alpha-numeric characters
#define CH_SPACE			32
#define CH_BANG				33
#define CH_DQUOTE			34
#define CH_HASH				35
#define CH_DOLLAR			36
#define CH_PERCENT			37
#define CH_AMP				38
#define CH_SQUOTE			39
#define CH_LPAREN			40
#define CH_RPAREN			41
#define CH_AST				42
#define CH_PLUS				43
#define CH_COMMA			44
#define CH_MINUS			45
#define CH_PERIOD			46
#define CH_FSLASH			47
#define CH_0				48
#define CH_1				49
#define CH_2				50
#define CH_3				51
#define CH_4				52
#define CH_5				53
#define CH_6				54
#define CH_7				55
#define CH_8				56
#define CH_9				57
#define CH_COLON			58
#define CH_SEMIC			59
#define CH_LESS				60
#define CH_EQUAL			61
#define CH_GREATER			62
#define CH_QUESTION			63
#define CH_AT				64
#define CH_UC_A				65
#define CH_UC_B				(CH_UC_A + 1)
#define CH_UC_C				(CH_UC_B + 1)
#define CH_UC_D				(CH_UC_C + 1)
#define CH_UC_E				(CH_UC_D + 1)
#define CH_UC_F				(CH_UC_E + 1)
#define CH_UC_G				(CH_UC_F + 1)
#define CH_UC_H				(CH_UC_G + 1)
#define CH_UC_I				(CH_UC_H + 1)
#define CH_UC_J				(CH_UC_I + 1)
#define CH_UC_K				(CH_UC_J + 1)
#define CH_UC_L				(CH_UC_K + 1)
#define CH_UC_M				(CH_UC_L + 1)
#define CH_UC_N				(CH_UC_M + 1)
#define CH_UC_O				(CH_UC_N + 1)
#define CH_UC_P				(CH_UC_O + 1)
#define CH_UC_Q				(CH_UC_P + 1)
#define CH_UC_R				(CH_UC_Q + 1)
#define CH_UC_S				(CH_UC_R + 1)
#define CH_UC_T				(CH_UC_S + 1)
#define CH_UC_U				(CH_UC_T + 1)
#define CH_UC_V				(CH_UC_U + 1)
#define CH_UC_W				(CH_UC_V + 1)
#define CH_UC_X				(CH_UC_W + 1)
#define CH_UC_Y				(CH_UC_X + 1)
#define CH_UC_Z				(CH_UC_Y + 1)
#define CH_LBRACKET			91
#define CH_BSLASH			92
#define CH_RBRACKET			93
#define CH_CARET			94
#define CH_UNDER			95
#define CH_LSQUOTE			96
#define CH_LC_A				(CH_UC_A + 32)
#define CH_LC_B				(CH_LC_A + 1)
#define CH_LC_C				(CH_LC_B + 1)
#define CH_LC_D				(CH_LC_C + 1)
#define CH_LC_E				(CH_LC_D + 1)
#define CH_LC_F				(CH_LC_E + 1)
#define CH_LC_G				(CH_LC_F + 1)
#define CH_LC_H				(CH_LC_G + 1)
#define CH_LC_I				(CH_LC_H + 1)
#define CH_LC_J				(CH_LC_I + 1)
#define CH_LC_K				(CH_LC_J + 1)
#define CH_LC_L				(CH_LC_K + 1)
#define CH_LC_M				(CH_LC_L + 1)
#define CH_LC_N				(CH_LC_M + 1)
#define CH_LC_O				(CH_LC_N + 1)
#define CH_LC_P				(CH_LC_O + 1)
#define CH_LC_Q				(CH_LC_P + 1)
#define CH_LC_R				(CH_LC_Q + 1)
#define CH_LC_S				(CH_LC_R + 1)
#define CH_LC_T				(CH_LC_S + 1)
#define CH_LC_U				(CH_LC_T + 1)
#define CH_LC_V				(CH_LC_U + 1)
#define CH_LC_W				(CH_LC_V + 1)
#define CH_LC_X				(CH_LC_W + 1)
#define CH_LC_Y				(CH_LC_X + 1)
#define CH_LC_Z				(CH_LC_Y + 1)
#define CH_LCBRACKET		123
#define CH_PIPE				124
#define CH_RCBRACKET		125
#define CH_TILDE			126


// FOENSCII graphic characters
#define CH_VFILL_UP_1		95	// underscore
#define CH_VFILL_UP_2		1
#define CH_VFILL_UP_3		2
#define CH_VFILL_UP_4		3
#define CH_VFILL_UP_5		4
#define CH_VFILL_UP_6		5
#define CH_VFILL_UP_7		6
#define CH_VFILL_UP_8		7	// solid inverse space
#define CH_VFILL_DN_8		7	// solid inverse space
#define CH_VFILL_DN_7		8
#define CH_VFILL_DN_6		9
#define CH_VFILL_DN_5		10
#define CH_VFILL_DN_4		11
#define CH_VFILL_DN_3		12
#define CH_VFILL_DN_2		13
#define CH_VFILL_DN_1		14

#define CH_VFILLC_UP_1		192	// same as solid fill up/down, but checkered
#define CH_VFILLC_UP_2		193
#define CH_VFILLC_UP_3		194
#define CH_VFILLC_UP_4		195
#define CH_VFILLC_UP_5		196
#define CH_VFILLC_UP_6		197
#define CH_VFILLC_UP_7		198
#define CH_VFILLC_UP_8		199	// full checkered block
#define CH_VFILLC_DN_8		199	// full checkered block
#define CH_VFILLC_DN_7		200
#define CH_VFILLC_DN_6		201
#define CH_VFILLC_DN_5		202
#define CH_VFILLC_DN_4		203
#define CH_VFILLC_DN_3		204
#define CH_VFILLC_DN_2		205
#define CH_VFILLC_DN_1		206

#define CH_HFILL_UP_1		134
#define CH_HFILL_UP_2		135
#define CH_HFILL_UP_3		136
#define CH_HFILL_UP_4		137
#define CH_HFILL_UP_5		138
#define CH_HFILL_UP_6		139
#define CH_HFILL_UP_7		140
#define CH_HFILL_UP_8		7	// solid inverse space
#define CH_HFILL_DN_8		7	// solid inverse space
#define CH_HFILL_DN_7		141
#define CH_HFILL_DN_6		142
#define CH_HFILL_DN_5		143
#define CH_HFILL_DN_4		144
#define CH_HFILL_DN_3		145
#define CH_HFILL_DN_2		146
#define CH_HFILL_DN_1		147

#define CH_HFILLC_UP_1		207
#define CH_HFILLC_UP_2		208
#define CH_HFILLC_UP_3		209
#define CH_HFILLC_UP_4		210
#define CH_HFILLC_UP_5		211
#define CH_HFILLC_UP_6		212
#define CH_HFILLC_UP_7		213
#define CH_HFILLC_UP_8		199	// full checkered block
#define CH_HFILLC_DN_8		199	// full checkered block
#define CH_HFILLC_DN_7		214
#define CH_HFILLC_DN_6		216
#define CH_HFILLC_DN_5		217
#define CH_HFILLC_DN_4		218
#define CH_HFILLC_DN_3		219
#define CH_HFILLC_DN_2		220
#define CH_HFILLC_DN_1		221

#define CH_HDITH_1			15	// horizontal dither patterns...
#define CH_HDITH_2			16
#define CH_HDITH_3			17
#define CH_HDITH_4			18
#define CH_HDITH_5			19
#define CH_HDITH_6			20
#define CH_HDITH_7			21
#define CH_HDITH_8			22
#define CH_HDITH_9			23
#define CH_HDITH_10			24

#define CH_DITH_L1			16	// full-block dither patterns
#define CH_DITH_L2			18
#define CH_DITH_L3			199
#define CH_DITH_L4			21
#define CH_DITH_L5			23

#define CH_VDITH_1			25	// vertical dither patterns...
#define CH_VDITH_2			26
#define CH_VDITH_3			27
#define CH_VDITH_4			28
#define CH_VDITH_5			29

#define CH_DIAG_R1			186	// diagonal patterns and lines...
#define CH_DIAG_R2			30

#define CH_DIAG_R3			184
#define CH_DIAG_R4			230
#define CH_DIAG_R5			234
#define CH_DIAG_R6			238

#define CH_DIAG_R7			229
#define CH_DIAG_R8			233
#define CH_DIAG_R9			237
#define CH_DIAG_R10			241

#define CH_DIAG_L1			187
#define CH_DIAG_L2			31

#define CH_DIAG_L3			228
#define CH_DIAG_L4			232
#define CH_DIAG_L5			236
#define CH_DIAG_L6			240

#define CH_DIAG_L7			185
#define CH_DIAG_L8			231
#define CH_DIAG_L9			235
#define CH_DIAG_L10			239

#define CH_DIAG_X			159

#define CH_HLINE_UP_1		95	// underscore
#define CH_HLINE_UP_2		148
#define CH_HLINE_UP_3		149
#define CH_HLINE_UP_4		150
#define CH_HLINE_UP_5		151
#define CH_HLINE_UP_6		152
#define CH_HLINE_UP_7		153
#define CH_HLINE_UP_8		14
#define CH_HLINE_DN_8		CH_HLINE_UP_8
#define CH_HLINE_DN_7		CH_HLINE_UP_7
#define CH_HLINE_DN_6		CH_HLINE_UP_6
#define CH_HLINE_DN_5		CH_HLINE_UP_5
#define CH_HLINE_DN_4		CH_HLINE_UP_4
#define CH_HLINE_DN_3		CH_HLINE_UP_3
#define CH_HLINE_DN_2		CH_HLINE_UP_2
#define CH_HLINE_DN_1		CH_HLINE_UP_1

#define CH_VLINE_UP_1		134
#define CH_VLINE_UP_2		133
#define CH_VLINE_UP_3		132
#define CH_VLINE_UP_4		131
#define CH_VLINE_UP_5		130
#define CH_VLINE_UP_6		129
#define CH_VLINE_UP_7		128
#define CH_VLINE_UP_8		147
#define CH_VLINE_DN_8		CH_VLINE_UP_8
#define CH_VLINE_DN_7		CH_VLINE_UP_7
#define CH_VLINE_DN_6		CH_VLINE_UP_6
#define CH_VLINE_DN_5		CH_VLINE_UP_5
#define CH_VLINE_DN_4		CH_VLINE_UP_4
#define CH_VLINE_DN_3		CH_VLINE_UP_3
#define CH_VLINE_DN_2		CH_VLINE_UP_2
#define CH_VLINE_DN_1		CH_VLINE_UP_1

#define CH_LINE_WE			150	// box-drawing lines. read clockwise from west. N=up-facing line, E=right-facing line, etc.
#define CH_LINE_NS			130
#define CH_LINE_NES			154
#define CH_LINE_WES			155
#define CH_LINE_WNES		156
#define CH_LINE_WNE			157
#define CH_LINE_WNS			158
#define CH_LINE_ES			160	// square edge corners
#define CH_LINE_WS			161
#define CH_LINE_NE			162
#define CH_LINE_WN			163
#define CH_LINE_RND_ES		188	// rounded edge corners
#define CH_LINE_RND_WS		189
#define CH_LINE_RND_NE		190
#define CH_LINE_RND_WN		191

#define CH_LINE_BLD_WE		173	// thick box-drawing lines. read clockwise from west. N=up-facing line, E=right-facing line, etc.
#define CH_LINE_BLD_NS		174
#define CH_LINE_BLD_NES		164
#define CH_LINE_BLD_WES		165
#define CH_LINE_BLD_WNES	166
#define CH_LINE_BLD_WNE		167
#define CH_LINE_BLD_WNS		168
#define CH_LINE_BLD_ES		169	// square edge corners
#define CH_LINE_BLD_WS		170
#define CH_LINE_BLD_NE		171
#define CH_LINE_BLD_WN		172
#define CH_LINE_BLD_RND_ES	175	// rounded edge corners
#define CH_LINE_BLD_RND_WS	176
#define CH_LINE_BLD_RND_NE	177
#define CH_LINE_BLD_RND_WN	178

#define CH_BLOCK_N			11	// half-width/height block characters. read clockwise from west. N=upper, E=right-side, etc.
#define CH_BLOCK_S			3
#define CH_BLOCK_W			137
#define CH_BLOCK_E			144
#define CH_BLOCK_SE			242
#define CH_BLOCK_SW			243
#define CH_BLOCK_NE			244
#define CH_BLOCK_NW			245
#define CH_BLOCK_NWSE		246
#define CH_BLOCK_SWNE		247

#define CH_MISC_GBP			0
#define CH_MISC_VTILDE		127
#define CH_MISC_COPY		215
#define CH_MISC_FOENIX		223
#define CH_MISC_CHECKMARK	222
#define CH_MISC_HEART		252
#define CH_MISC_DIA			253
#define CH_MISC_SPADE		254
#define CH_MISC_CLUB		255

#define CH_ARROW_DN			248
#define CH_ARROW_LEFT		249
#define CH_ARROW_RIGHT		250
#define CH_ARROW_UP			251
// circular shapes, from larger to smaller
#define CH_CIRCLE_1			180	// full-size filled circle
#define CH_CIRCLE_2			225	// full-size selected radio button circle
#define CH_CIRCLE_3			179	// full-size empty circle
#define CH_CIRCLE_4			226 // medium-size filled circle
#define CH_CIRCLE_5			182 // small circle (square tho)
#define CH_CIRCLE_6			183 // tiny circle (1 pixel)
// square shapes, from larger to smaller
#define CH_RECT_1			7	// full-size filled square
#define CH_RECT_2			227	// full-size empty square
#define CH_RECT_3			181 // almost-full size filled square
#define CH_RECT_4			224 // medium-size filled square
#define CH_RECT_5			182 // small square
#define CH_RECT_6			183 // tiny square (1 pixel)


// Japanese JIS characters plus 6 custom from PC-8001
#define CH_JA_HOUR			24
#define CH_JA_MIN			25
#define CH_JA_SEC			26
#define CH_JA_YEAR			28
#define CH_JA_MONTH			29
#define CH_JA_DAY			192

#define CH_JIS_FIRST		193
#define CH_JIS_KUTEN		193
#define CH_JIS_OPEN			194
#define CH_JIS_CLOSE		195
#define CH_JIS_DOKUTEN		196
#define CH_JIS_MID			197
#define CH_JIS_WO			198
#define CH_JIS_L_A			199
#define CH_JIS_L_I			200
#define CH_JIS_L_U			201
#define CH_JIS_L_E			202
#define CH_JIS_L_O			203
#define CH_JIS_L_YA			204
#define CH_JIS_L_YU			205
#define CH_JIS_L_YO			206
#define CH_JIS_L_TU			207
#define CH_JIS_BOU			208
#define CH_JIS_A			209
#define CH_JIS_I			210
#define CH_JIS_U			211
#define CH_JIS_E			212
#define CH_JIS_O			213
#define CH_JIS_KA			214
#define CH_JIS_KI			215
#define CH_JIS_KU			216
#define CH_JIS_KE			217
#define CH_JIS_KO			218
#define CH_JIS_SA			219
#define CH_JIS_SHI			220
#define CH_JIS_SU			221
#define CH_JIS_SE			222
#define CH_JIS_SO			223
#define CH_JIS_TA			224
#define CH_JIS_TI			225
#define CH_JIS_TSU			226
#define CH_JIS_TE			227
#define CH_JIS_TO			228
#define CH_JIS_NA			229
#define CH_JIS_NI			230
#define CH_JIS_NU			231
#define CH_JIS_NE			232
#define CH_JIS_NO			233
#define CH_JIS_HA			234
#define CH_JIS_HI			235
#define CH_JIS_HU			236
#define CH_JIS_HE			237
#define CH_JIS_HO			238
#define CH_JIS_MA			239
#define CH_JIS_MI			240
#define CH_JIS_MU			241
#define CH_JIS_ME			242
#define CH_JIS_MO			243
#define CH_JIS_YA			244
#define CH_JIS_YU			245
#define CH_JIS_YO			246
#define CH_JIS_RA			247
#define CH_JIS_RI			248
#define CH_JIS_RU			249
#define CH_JIS_RE			250
#define CH_JIS_RO			251
#define CH_JIS_WA			252
#define CH_JIS_N			253
#define CH_JIS_B			254
#define CH_JIS_P			255
#define CH_JIS_LAST			255

/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/




/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct KeyRepeater
{
	uint8_t		key;		// Key-code to repeat
	uint8_t		cookie;
} KeyRepeater;

	
/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// **** USER INPUT UTILITIES *****

// Check to see if keystroke events pending - does not wait for a key
uint8_t Keyboard_GetKeyIfPressed(void);

// Wait for one character from the keyboard and return it
char Keyboard_GetChar(void);

// main event processor
void Keyboard_ProcessEvents(void);

// initiate the minute hand timer
void Keyboard_InitiateMinuteHand(void);


#endif /* KEYBOARD_H_ */
