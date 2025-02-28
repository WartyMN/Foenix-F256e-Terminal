/*
 * strings.c
 *
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "strings.h"

// C includes
#include <stdint.h>
#include <string.h>

// F256 includes


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/


/*****************************************************************************/
/*                          File-scoped Variables                            */
/*****************************************************************************/

static uint8_t			string_merge_buff_192b[192];

static char* strings[NUM_STRINGS]= {
     (char*)"Are you sure you want to exit?",
     (char*)"Are you sure?",
     (char*)"Set Current Date & Time",
     (char*)"Enter date and time as 'YY-MM-DD HH:mm'",
     (char*)"Enter name for this file:",
     (char*)"Baud",
     (char*)"Select baud",
     (char*)"BBS",
     (char*)"Enter URL and port",
     (char*)"Hang up?",
     (char*)"Yes",
     (char*)"No",
     (char*)"OK",
     (char*)"Cancel",
     (char*)"MAIN MENU",
     (char*)"b Set Baud",
     (char*)"f Fone Home",
     (char*)"d Dial a BBS",
     (char*)"a About",
     (char*)"q Quit",
     (char*)"Error: Could not initialize serial comms",
     (char*)"Error: Serial overflow",
     (char*)"Unknown ANSI sequence: %s",
     (char*)"Generic Error!",
     (char*)"Fatal error %u",
     (char*)"We don't need no stinkin' error messages",
     (char*)"A disk error occurred",
     (char*)"Out of memory error",
     (char*)"Scanning for drives...",
     (char*)"%u drives found",
     (char*)"External SD card successfully mounted as Device 0",
     (char*)"Internal SD card successfully mounted as Device 1",
     (char*)"Device %d successfully mounted",
     (char*)"Hit any key",
     (char*)"Available memory: %zu bytes",
     (char*)"Serial buffer saved to disk",
     (char*)"300 baud was good enough for WOPR, it's good enough for you.",
     (char*)"1200 baud: Volksmodem unlocked!",
     (char*)"2400 baud and flying!",
     (char*)"Trueish fact: Nobody ever advertised a 3600 baud modem.",
     (char*)"4800 baud. That's 16 times faster than a VIC modem!",
     (char*)"9600 baud. Bit of a sweet spot between speed and reliability.",
     (char*)"19200 baud. All hands stand by to reduce sail.",
     (char*)"38400 baud? Hope that's a 68000 in your pocket.",
     (char*)"57600 baud. US Robotics for the win!",
     (char*)"115200 baud. I think you overestimate my skilz.",
     (char*)"   300",
     (char*)"   600",
     (char*)"  1200",
     (char*)"  2400",
     (char*)"  3600",
     (char*)"  4800",
     (char*)"  9600",
     (char*)" 19200",
     (char*)" 38400",
     (char*)" 57600",
     (char*)"115200",
     (char*)"ANSI charset with Foenix-style font selected.",
     (char*)"ANSI charset with IBM-style font selected.",
     (char*)"Japanese charset (JIS X 0201) with Foenix-style font selected.",
     (char*)"Foenix charset selected.",
     (char*)"https://github.com/WartyMN/f256-fterm/",
     (char*)"F256 f/term %c2024 Micah Bly. Version %u.%u.%u.",
     (char*)"Machine: F256K2 (optical keyboard)",
     (char*)"Machine: F256K upgraded to F256K2 (mechanical keyboard)",
     (char*)"Machine: F256K with 'Extended' memory FPGA load",
     (char*)"Machine: Unknown, ID=%d",
     (char*)"Machine: F256JR2",
     (char*)"F256JR",
     (char*)"F256JR2",
     (char*)"F256K",
     (char*)"F256K2",
     (char*)"<unknown hardware>",
};



/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/



/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/



/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// return the global string for the passed ID
// this is just a wrapper around the string, to make it easier to re-use and diff code in different overlays
char* Strings_GetString(uint8_t the_string_id)
{
	// LOGIC: 
	//   on 64K memory map machines, I typically store the strings in extended memory
	//     this routine, for those machines, generally maps in EM, copies the string to a non-EM buffer, and returns the local buffer.
	//   on non-memory constrained machines, there can still be a case for returning a copy of the string. 
	//     Lich King for example modifies some strings as displayed, to wrap in a certain display area. 
	//     slightly slower to copy of course. 
	//   fastest solution would be to simply not call this routine at all, and access via global_string[the_string_id]), but then 
	//     every string call will have to be re-written between memory-constrained and non-memory-constrained ports of any given app.
	
	strcpy((char*)string_merge_buff_192b, strings[the_string_id]);

	return (char*)string_merge_buff_192b;
}
