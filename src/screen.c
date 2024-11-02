/*
 * screen.c
 *
 *  Created on: Jan 11, 2023
 *      Author: micahbly
 *
 *  Routines for drawing and updating the UI elements
 *
 */



/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "screen.h"
#include "app.h"
#include "comm_buffer.h"
#include "debug.h"
// #include "file.h"
// #include "folder.h"
#include "general.h"
#include "kernel.h"
#include "keyboard.h"
#include "memory.h"
#include "sys.h"
#include "text.h"
#include "strings.h"

// C includes
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// F256 includes
#include "f256_e.h"




/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/


/*****************************************************************************/
/*                           File-scope Variables                            */
/*****************************************************************************/


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

extern bool					global_launcher_mode;	// if true, only 1 panel shown; only exe type files shown; preview shown at right.
extern int8_t				global_connected_device[DEVICE_MAX_DEVICE_COUNT];	// will be 8, 9, etc, if connected, or -1 if not..
extern bool					global_started_from_flash;		// tracks whether app started from flash or from disk
extern bool					global_find_next_enabled;

extern char*				global_string_buff1;
extern char*				global_string_buff2;

extern TextDialogTemplate	global_dlg;	// dialog we'll configure and re-use for different purposes
extern char					global_dlg_title[36];	// arbitrary
extern char					global_dlg_body_msg[70];	// arbitrary
extern char					global_dlg_button[3][10];	// arbitrary
extern uint8_t				temp_screen_buffer_char[APP_DIALOG_BUFF_SIZE];	// WARNING HBD: don't make dialog box bigger than will fit!
extern uint8_t				temp_screen_buffer_attr[APP_DIALOG_BUFF_SIZE];	// WARNING HBD: don't make dialog box bigger than will fit!


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/



/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/




/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/


// display information about f/manager
void Screen_ShowAppAboutInfo(void)
{
	// show app name, version, and credit
	sprintf(global_string_buff1, Strings_GetString(ID_STR_ABOUT_FTERM), CH_COPYRIGHT, MAJOR_VERSION, MINOR_VERSION, UPDATE_VERSION);
	Buffer_NewMessage(global_string_buff1);
	
// 	// also show current bytes free
// 	sprintf(global_string_buff1, Strings_GetString(ID_STR_N_BYTES_FREE), _heapmemavail());
// 	Buffer_NewMessage(global_string_buff1);
}


// show user a dialog and have them enter a string
// if a prefilled string is not needed, set starter_string to an empty string
// set max_len to the maximum number of bytes/characters that should be collected from user
// returns NULL if user cancels out of dialog, or returns a path to the string the user provided
char* Screen_GetStringFromUser(char* dialog_title, char* dialog_body, char* starter_string, uint8_t max_len)
{
	bool				success;
	uint8_t				orig_dialog_width;
	uint8_t				temp_dialog_width;
	
	// copy title and body text
	General_Strlcpy((char*)&global_dlg_title, dialog_title, 36);
	General_Strlcpy((char*)&global_dlg_body_msg, dialog_body, 70);

	// copy the starter string into the edit buffer so user can edit
	General_Strlcpy(global_string_buff2, starter_string, max_len + 1);
	
	// adjust dialog width temporarily, if necessary and possible
	orig_dialog_width = global_dlg.width_;
	temp_dialog_width = General_Strnlen(starter_string,  max_len) + 2; // +2 is for box draw chars
	
	DEBUG_OUT(("%s %d: orig_dialog_width=%u, temp width=%u, max_len=%u, starter='%s'", __func__ , __LINE__, orig_dialog_width, temp_dialog_width, max_len, starter_string));
	
	if (temp_dialog_width < orig_dialog_width)
	{
		temp_dialog_width = orig_dialog_width - 2;
	}
	else
	{
		global_dlg.width_ = temp_dialog_width;
		temp_dialog_width -= 2;
	}
	
	success = Text_DisplayTextEntryDialog(&global_dlg, (char*)&temp_screen_buffer_char, (char*)&temp_screen_buffer_attr, global_string_buff2, temp_dialog_width);

	// restore normal dialog width
	global_dlg.width_ = orig_dialog_width;

	// did user enter a name?
	if (success == false)
	{
		return NULL;
	}
	
	return global_string_buff2;
}


// show user a 2 button confirmation dialog and have them click a button
// returns true if user selected the "positive" button, or false if they selected the "negative" button
bool Screen_ShowUserTwoButtonDialog(char* dialog_title, uint8_t dialog_body_string_id, uint8_t positive_btn_label_string_id, uint8_t negative_btn_label_string_id)
{
	// copy title, body text, and buttons
	General_Strlcpy((char*)&global_dlg_title, dialog_title, 36);
	General_Strlcpy((char*)&global_dlg_body_msg, Strings_GetString(dialog_body_string_id), 70);
	General_Strlcpy((char*)&global_dlg_button[0], Strings_GetString(negative_btn_label_string_id), 10);
	General_Strlcpy((char*)&global_dlg_button[1], Strings_GetString(positive_btn_label_string_id), 10);
					
	global_dlg.num_buttons_ = 2;

	return Text_DisplayDialog(&global_dlg, (char*)&temp_screen_buffer_char, (char*)&temp_screen_buffer_attr);
}


