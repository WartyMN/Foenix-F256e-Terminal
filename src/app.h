/*
 * app.h
 *
 *  Created on: Jan 10, 2023
 *      Author: micahbly
 *
 *  A pseudo commander-style 2-column file manager
 *
 */
  
// Adapted from F256jr/k version of FileManager starting June 2, 2024

#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/


// project includes
#include "serial.h"

// C includes
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <string.h>



/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

// hide __fastcall_ from everything but CC65 (to squash some warnings in LSP/BBEdit)
#ifndef __CC65__
	#define __fastcall__ 
#endif

// hide "__far" keyboard from host mac language checker
#ifndef __CALYPSI__
#  define __far 
#endif

#ifndef NULL
#  define NULL 0
#endif

#define MAJOR_VERSION	0
#define MINOR_VERSION	1
#define UPDATE_VERSION	2

#define VERSION_NUM_X	0
#define VERSION_NUM_Y	24


// there are 2 panels which can be accessed with the same code
#define NUM_PANELS					2
#define PANEL_ID_LEFT				0
#define PANEL_ID_RIGHT				1


#define APP_DIALOG_WIDTH					42
#define APP_DIALOG_HEIGHT					7
#define APP_DIALOG_STARTING_NUM_BUTTONS		2
#define APP_DIALOG_BUFF_SIZE				((APP_DIALOG_WIDTH+2) * (APP_DIALOG_HEIGHT+2))	// for the temp char and color buffs when opening a window, this is how much mem we'll reserve for each

#define FILE_MAX_FILENAME_SIZE_CBM	(16+1)	// CBM DOS defined
#define FILE_MAX_FILENAME_SIZE		(40+1)	// in F256 kernel, total path can't be longer than 255 chars.
#define FILE_MAX_PATHNAME_SIZE		(255)	// in F256 kernel, total path can't be longer than 255 chars.
#define FILE_MAX_APPFILE_INFO_SIZE	255		// for info panel display about mime/app type, the max # of bytes to display
#define FILE_MAX_TEXT_PREVIEW_SIZE	255		// for info panel previews, the max # of bytes to read in and display
#define FILE_TYPE_MAX_SIZE_NAME		4	// mostly PRG/REL, but allowed for SUBD
#define FILE_SIZE_MAX_SIZE			16	// max size of human-readable file size. eg, "255 blocks", "1,200 MB"
#define FILE_BYTES_PER_BLOCK_IEC	254	// for CBM DOS, 1 block = 256b but really only 254
#define FILE_BYTES_PER_BLOCK		256	// for FAT32, 1 block = 256b
#define MAX_NUM_FILES_IEC			144 // The directory track should be contained totally on track 18. Sectors 1-18 contain the entries and sector 0 contains the BAM (Block Availability Map) and disk name/ID. Since the directory is only 18 sectors large (19 less one for the BAM), and each sector can contain only 8 entries (32 bytes per entry), the maximum number of directory entries is 18 * 8 = 144. http://justsolve.archiveteam.org/wiki/CBMFS
// BUT... 1581 supported 296 entries. hmm. 


#define DEVICE_LOWEST_FFS_DEVICE_NUM	0
#define DEVICE_HIGHEST_FFS_DEVICE_NUM	1
#define DEVICE_MAX_FFS_DEVICE_COUNT		(DEVICE_HIGHEST_FFS_DEVICE_NUM - DEVICE_LOWEST_FFS_DEVICE_NUM + 1)
#define DEVICE_LOWEST_IEC_DEVICE_NUM	(DEVICE_HIGHEST_FFS_DEVICE_NUM + 1)
#define DEVICE_HIGHEST_IEC_DEVICE_NUM	(DEVICE_LOWEST_IEC_DEVICE_NUM + 1)
#define DEVICE_LOWEST_DEVICE_NUM		DEVICE_LOWEST_FFS_DEVICE_NUM
#define DEVICE_HIGHEST_DEVICE_NUM		DEVICE_HIGHEST_IEC_DEVICE_NUM
#define DEVICE_MAX_DEVICE_COUNT			(DEVICE_HIGHEST_DEVICE_NUM - DEVICE_LOWEST_DEVICE_NUM + 1)

#define NO_DISK_PRESENT_FILE_NAME	31	// this is a char I see reported as a "islbl" by tool when scanning a floppy disk drive with no disk in it. 
#define NO_DISK_PRESENT_ANYMORE_FILE_NAME	28	// this is a char I see reported as a "islbl" by tool when scanning a floppy disk drive with no disk in it, when previously it was scanned with a disk in it.  

#define MEM_DUMP_BYTES_PER_ROW		16
#define MAX_MEM_DUMP_LEN			(24 * MEM_DUMP_BYTES_PER_ROW)	// 24*16 = 384
#define MEM_DUMP_START_X_FOR_HEX	7
#define MEM_DUMP_START_X_FOR_CHAR	(MEM_DUMP_START_X_FOR_HEX + MEM_DUMP_BYTES_PER_ROW * 3)

#define MEM_TEXT_VIEW_BYTES_PER_ROW	80
#define MAX_TEXT_VIEW_ROWS_PER_PAGE	(60-1)	// allow 1 line for instructions at top
#define MAX_TEXT_VIEW_LEN			(MAX_TEXT_VIEW_ROWS_PER_PAGE * MEM_TEXT_VIEW_BYTES_PER_ROW)	// 51*80 = 4080



/*****************************************************************************/
/*                             MEMORY LOCATIONS                              */
/*****************************************************************************/

#define STORAGE_FILE_BUFFER_LEN				256	// 1-page buffer for I/O operations.
#define STORAGE_STRING_BUFFER_1_LEN			204	// 204b buffer. 
#define STORAGE_STRING_BUFFER_2_LEN			204	// 204b buffer. 


#define STRING_STORAGE_SLOT                0x06
#define STRING_STORAGE_VALUE               0x12
#define STRING_STORAGE_PHYS_ADDR           0x24000



/*****************************************************************************/
/*                           App-wide color choices                          */
/*****************************************************************************/

#define APP_FOREGROUND_COLOR		ANSI_COLOR_BRIGHT_BLUE
#define APP_BACKGROUND_COLOR		COLOR_BLACK
#define APP_ACCENT_COLOR			COLOR_BLUE
#define APP_SELECTED_FILE_COLOR		COLOR_BRIGHT_WHITE

#define BUFFER_FOREGROUND_COLOR		COLOR_CYAN
#define BUFFER_BACKGROUND_COLOR		COLOR_BLACK
#define BUFFER_ACCENT_COLOR			COLOR_BRIGHT_BLUE

#define PANEL_FOREGROUND_COLOR		COLOR_GREEN
#define PANEL_BACKGROUND_COLOR		COLOR_BLACK
#define PANEL_ACCENT_COLOR			COLOR_BRIGHT_BLUE

#define LIST_ACTIVE_COLOR			COLOR_BRIGHT_GREEN
#define LIST_INACTIVE_COLOR			COLOR_GREEN

#define LIST_HEADER_COLOR			COLOR_BRIGHT_YELLOW

#define MENU_FOREGROUND_COLOR		COLOR_CYAN
#define MENU_INACTIVE_COLOR			COLOR_BRIGHT_BLUE
#define MENU_BACKGROUND_COLOR		COLOR_BLACK
#define MENU_ACCENT_COLOR			COLOR_BRIGHT_BLUE

#define DIALOG_FOREGROUND_COLOR		COLOR_BRIGHT_YELLOW
#define DIALOG_BACKGROUND_COLOR		COLOR_BLUE
#define DIALOG_ACCENT_COLOR			COLOR_BRIGHT_BLUE
#define DIALOG_AFFIRM_COLOR			COLOR_GREEN
#define DIALOG_CANCEL_COLOR			COLOR_RED

#define FILE_CONTENTS_FOREGROUND_COLOR	COLOR_BRIGHT_GREEN
#define FILE_CONTENTS_BACKGROUND_COLOR	COLOR_BLACK
#define FILE_CONTENTS_ACCENT_COLOR		COLOR_GREEN


/*****************************************************************************/
/*                                   Command Keys                            */
/*****************************************************************************/


// key codes for user input
#define ACTION_INVALID_INPUT		255	// this will represent illegal keyboard command by user

#define ACTION_CANCEL				CH_ESC
#define ACTION_CANCEL_ALT			KEY_BREAK
#define ACTION_CONFIRM				CH_ENTER

// navigation keys
#define MOVE_UP						KEY_CURS_UP
#define MOVE_RIGHT					KEY_CURS_RIGHT
#define MOVE_DOWN					KEY_CURS_DOWN
#define MOVE_LEFT					KEY_CURS_LEFT
#define ACTION_SWAP_ACTIVE_PANEL	CH_TAB

// file and memory bank actions
#define ACTION_SELECT				CH_ENTER // numpad key
#define ACTION_DELETE				CH_DEL

#define ACTION_SET_BAUD				'b'
#define ACTION_DIAL_FOENIX_BBS		'f'
#define ACTION_DIAL_BBS				'd'
#define ACTION_ABOUT				'a' 
#define ACTION_QUIT					'q'

#define ACTION_HELP					'?' // numpad key



/*****************************************************************************/
/*                                 Error Codes                               */
/*****************************************************************************/

#define ERROR_NO_ERROR													0
#define ERROR_NO_FILES_IN_FILE_LIST										101
#define ERROR_PANEL_WAS_NULL											102
#define ERROR_PANEL_ROOT_FOLDER_WAS_NULL								103
#define ERROR_PANEL_TARGET_FOLDER_WAS_NULL								104
#define ERROR_FOLDER_WAS_NULL											105
#define ERROR_FILE_WAS_NULL												106
#define ERROR_COULD_NOT_OPEN_DIR										107
#define ERROR_COULD_NOT_CREATE_NEW_FILE_OBJECT							108
#define ERROR_FOLDER_FILE_WAS_NULL										109
#define ERROR_NO_CONNECTED_DRIVES_FOUND									110
#define ERROR_FILE_TO_DESTROY_WAS_NULL									111
#define ERROR_DESTROY_ALL_FOLDER_WAS_NULL								112
#define ERROR_FILE_DUPLICATE_FAILED										113
#define ERROR_FOLDER_TO_DESTROY_WAS_NULL								114
#define ERROR_SET_CURR_ROW_FOLDER_WAS_NULL								115
#define ERROR_GET_CURR_ROW_FOLDER_WAS_NULL								116
#define ERROR_SET_FILE_SEL_BY_ROW_PANEL_WAS_NULL						117
#define ERROR_FILE_MARK_SELECTED_FILE_WAS_NULL							118
#define ERROR_FILE_MARK_UNSELECTED_FILE_WAS_NULL						119
#define ERROR_PANEL_INIT_FOLDER_WAS_NULL								120
#define ERROR_COPY_FILE_SOURCE_FOLDER_WAS_NULL							121
#define ERROR_COPY_FILE_TARGET_FOLDER_WAS_NULL							122
#define ERROR_POPULATE_FILES_FOLDER_WAS_NULL							123
#define ERROR_COULD_NOT_CREATE_ROOT_FOLDER_FILE							124
#define ERROR_COULD_NOT_CREATE_ROOT_FOLDER_OBJ							125
#define ERROR_MEMSYS_GET_CURR_ROW_FOLDER_WAS_NULL						126
#define ERROR_BANK_MARK_SELECTED_BANK_WAS_NULL							127
#define ERROR_BANK_MARK_UNSELECTED_BANK_WAS_NULL						128
#define ERROR_BANK_TO_DESTROY_WAS_NULL									129
#define ERROR_PANEL_INIT_MEMSYS_WAS_NULL								130
#define ERROR_DESTROY_ALL_MEMSYS_WAS_NULL								131
#define ERROR_COULD_NOT_CREATE_OR_RESET_MEMSYS_OBJ						132

#define ERROR_DEFINE_ME													255



/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/

// typedef enum file_list_scope
// {
// 	LIST_SCOPE_ALL = 0,
// 	LIST_SCOPE_SELECTED = 1,
// 	LIST_SCOPE_NOT_SELECTED = 2,
// } file_list_scope;

#define LIST_SCOPE_ALL				0
#define LIST_SCOPE_SELECTED			1
#define LIST_SCOPE_NOT_SELECTED		2

typedef enum device_number
{
	DEVICE_SD_CARD 				= 0,
	DEVICE_FLOPPY_1 			= 1,
	DEVICE_FLOPPY_2				= 2,
	DEVICE_MAX_DISK_DEVICE		= 3,	// use as upper bound, e.g, if x < DEVICE_MAX_DISK_DEVICE then this is a disk
	DEVICE_MIN_MEMORY_DEVICE	= 7,	// use as lower bound, e.g, if x > DEVICE_MIN_MEMORY_DEVICE then this is RAM or Flash
	DEVICE_RAM					= 8,
	DEVICE_FLASH				= 9,
} device_number;

typedef enum font_choice
{
	FONT_STD			= 0,	// The standard foenix font
	FONT_STD_KANA		,		// The standard look foenix font, but with Japanese kana glyphs (JIS)
	FONT_STD_ANSI		,		// The standard look foenix font, but with ANSI codepoints
	FONT_IBM_ANSI		,		// IBM 8x8 font with ANSI codepoints
	FONT_NOT_SET
} font_choice;

typedef enum ui_glyph_choice
{
	UI_MODE_FOENIX		= 0,	// draw UI with glyphs that match FOENIX "std" font code points
	UI_MODE_ANSI		= 1,	// draw UI with glyphs that match ANSI font code points
	UI_MODE_NOT_SET		= 2,
} ui_glyph_choice;


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct baud_config
{
	uint8_t		key_;
	uint16_t	divisor_;
	uint8_t		msg_string_id_;
	uint8_t		lbl_string_id_;
}  baud_config;


// also defined in f256.h

// typedef struct DateStamp {
//    int32_t	ds_Days;	      /* Number of days since Jan. 1, 1978 */
//    int32_t	ds_Minute;	      /* Number of minutes past midnight */
//    int32_t	ds_Tick;	      /* Number of ticks past minute */
// } DateStamp; /* DateStamp */


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// Draws the progress bar frame on the screen
void App_ShowProgressBar(void);

// Hides the progress bar frame on the screen
void App_HideProgressBar(void);

// draws the 'bar' part of the progress bar, according to a % complete passed (0-100 integer)
void App_UpdateProgressBar(uint8_t progress_bar_total);

// read the real time clock and display it
void App_DisplayTime(void);

// display error message, wait for user to confirm, and exit
void App_Exit(uint8_t the_error_number);


#endif /* FILE_MANAGER_H_ */
