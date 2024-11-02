//! @file strings.h

/*
 * strings.h
 *
 */



#ifndef STRINGS_H_
#define STRINGS_H_


/* about this class
 *
 *
 *
 *** things this class needs to be able to do
 * return a pointer to a temporary buffer containing the string value matching the ID passed
 *
 *** things objects of this class have
 *
 *
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes


// C includes
#include <stdint.h>

// f256 includes


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/


// Macro ID definitions for strings
#define ID_STR_DLG_QUIT_CONFIRM 0
#define ID_STR_DLG_ARE_YOU_SURE 1
#define ID_STR_DLG_SET_CLOCK_TITLE 2
#define ID_STR_DLG_SET_CLOCK_BODY 3
#define ID_STR_DLG_ENTER_FILE_NAME 4
#define ID_STR_DLG_BAUD_TITLE 5
#define ID_STR_DLG_BAUD_BODY 6
#define ID_STR_DLG_DIAL_BBS_TITLE 7
#define ID_STR_DLG_DIAL_BBS_BODY 8
#define ID_STR_DLG_HANGUP_TITLE 9
#define ID_STR_DLG_YES 10
#define ID_STR_DLG_NO 11
#define ID_STR_DLG_OK 12
#define ID_STR_DLG_CANCEL 13
#define ID_STR_MENU_APP 14
#define ID_STR_APP_SET_BAUD 15
#define ID_STR_APP_DIAL_FOENIX_BBS 16
#define ID_STR_APP_DIAL_BBS 17
#define ID_STR_APP_ABOUT 18
#define ID_STR_APP_QUIT 19
#define ID_STR_ERROR_INIT_FAILURE 20
#define ID_STR_ERROR_SERIAL_OVERFLOW 21
#define ID_STR_ERROR_UNKNOWN_ANSI_SEQUENCE 22
#define ID_STR_ERROR_GENERIC 23
#define ID_STR_MSG_FATAL_ERROR 24
#define ID_STR_MSG_FATAL_ERROR_BODY 25
#define ID_STR_ERROR_GENERIC_DISK 26
#define ID_STR_ERROR_ALLOC_FAIL 27
#define ID_STR_MSG_SCANNING 28
#define ID_STR_MSG_SHOW_DRIVE_COUNT 29
#define ID_STR_MSG_MOUNT_SUCCESS_SD0 30
#define ID_STR_MSG_MOUNT_SUCCESS_SD1_INTERNAL 31
#define ID_STR_MSG_MOUNT_SUCCESS_SD1_UNKNOWN 32
#define ID_STR_MSG_HIT_ANY_KEY 33
#define ID_STR_N_BYTES_FREE 34
#define ID_STR_MSG_DEBUG_DUMP 35
#define ID_STR_MSG_SET_BAUD_300 36
#define ID_STR_MSG_SET_BAUD_1200 37
#define ID_STR_MSG_SET_BAUD_2400 38
#define ID_STR_MSG_SET_BAUD_3600 39
#define ID_STR_MSG_SET_BAUD_4800 40
#define ID_STR_MSG_SET_BAUD_9600 41
#define ID_STR_MSG_SET_BAUD_19200 42
#define ID_STR_MSG_SET_BAUD_38400 43
#define ID_STR_MSG_SET_BAUD_57600 44
#define ID_STR_MSG_SET_BAUD_115200 45
#define ID_STR_BAUD_300 46
#define ID_STR_BAUD_600 47
#define ID_STR_BAUD_1200 48
#define ID_STR_BAUD_2400 49
#define ID_STR_BAUD_3600 50
#define ID_STR_BAUD_4800 51
#define ID_STR_BAUD_9600 52
#define ID_STR_BAUD_19200 53
#define ID_STR_BAUD_38400 54
#define ID_STR_BAUD_57600 55
#define ID_STR_BAUD_115200 56
#define ID_STR_MSG_SELECT_FONT_ANSI 57
#define ID_STR_MSG_SELECT_FONT_IBM 58
#define ID_STR_MSG_SELECT_FONT_JA 59
#define ID_STR_MSG_SELECT_FONT_FOENIX 60
#define ID_STR_ABOUT_GIT 61
#define ID_STR_ABOUT_THIS_APP 62
#define ID_STR_ABOUT_MACHINE_NAME_K2 63
#define ID_STR_ABOUT_MACHINE_NAME_K1_UPGRADE 64
#define ID_STR_ABOUT_MACHINE_NAME_K1_E_FPGA 65
#define ID_STR_ABOUT_MACHINE_NAME_UNKNOWN 66
#define ID_STR_ABOUT_MACHINE_NAME_JR2 67
#define ID_STR_MACHINE_JR 68
#define ID_STR_MACHINE_JR2 69
#define ID_STR_MACHINE_K 70
#define ID_STR_MACHINE_K2 71
#define ID_STR_MACHINE_UNKNOWN 72
#define NUM_STRINGS 73
#define TOTAL_STRING_BYTES 1809


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



// return the global string for the passed ID
// this is just a wrapper around the string, to make it easier to re-use and diff code in different overlays
char* Strings_GetString(uint8_t the_string_id);


#endif /* STRINGS_H_ */
