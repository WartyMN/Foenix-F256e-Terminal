/*
 * kernel.c
 *
 *  Created on: 2023
 *      Author: micahbly
 */

// Adapted from F256jr/k version of FileManager starting June 2, 2024
//   Originally this supported the F256jk/k microkernel by gadget, with maybe 50/50 my code.
//   For F256K2 (extended/flat memory map), f/manager isn't using the microkernel, so it is now scaled back and totally re-written



/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "app.h"	// need for FILE_MAX_PATHNAME_SIZE
#include "event.h"
#include "general.h" // need for strnlen
#include "text.h"

// C includes
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

// F256 includes
#include "f256_e.h"
#include "ff.h"


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/


/*****************************************************************************/
/*                          File-scope Variables                             */
/*****************************************************************************/


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

extern FATFS		global_ffs_device[DEVICE_MAX_FFS_DEVICE_COUNT];		// FFS objects for SD cards


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// sets the passed drive byte to the first char of the path, and returns pointer to the path part after the drive num/colon
static const char* path_without_drive(const char *path, char *drive_num);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// sets the passed drive byte to the first char of the path, and returns pointer to the path part after the drive num/colon
static const char* path_without_drive(const char *path, char *drive_num)
{
    *drive_num = 0;
    
    if (strlen(path) < 2) {
        return path;
    }
    
    if (path[1] != ':') {
        return path;
    }
    
    if ((*path >= '0') && (*path <= '7')) {
        *drive_num = *path - '0';
    }
        
    return (path + 2);
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/


// deletes the file at the specified path
// returns false in all error conditions
bool Kernel_DeleteFile(const char* the_path)
{
	char		drive_num;
	FRESULT		the_result;
    
    // TODO: split handling here once IEC device support is ready. 
    //   use drive_num to find out if it should use FFS or IEC calls 
    
    the_path = path_without_drive(the_path, &drive_num);

	the_result = f_unlink(the_path);
	
	if (the_result != FR_OK)
	{
		EventManager_AddEvent(diskError, the_result, drive_num, 0);
		return false;
	}
    
    return true;
}


// deletes the folder at the specified path
// returns false in all error conditions
bool Kernel_DeleteFolder(const char* the_path)
{
	char		drive_num;
	FRESULT		the_result;
    
    // TODO: split handling here once IEC device support is ready. 
    //   use drive_num to find out if it should use FFS or IEC calls
    //   FFS uses same command to delete a file as it does folder, but IEC may need separate functions (microkernel did)
    
    return Kernel_DeleteFile(the_path);
}


// renames the specified folder/file to the new name
// returns false in any error condition
bool Kernel_Rename(const char* old_path, const char *new_path)
{
 	char		src_drive_num;
    char		dst_drive_num;
	FRESULT		the_result;
    
    // TODO: split handling here once IEC device support is ready. 
    //   use drive_num to find out if it should use FFS or IEC calls 
    
    old_path = path_without_drive(old_path, &src_drive_num);
    new_path = path_without_drive(new_path, &dst_drive_num);
    
    if (dst_drive_num != src_drive_num)
    {    
        // rename across drives is not supported.
        return false;
    }
 
	the_result = f_rename(old_path, new_path);
	
	if (the_result != FR_OK)
	{
		EventManager_AddEvent(diskError, the_result, src_drive_num, 0);
		return false;
	}
    
    return true;
}


// Format a FatFS volume
//   pass the name you want for the formatted disk/SD card in name, and the drive number (0-2) in the drive param.
//   do NOT prepend the path onto name. 
// returns negative number on any error
int Kernal_Mkfs(const char* name, const uint8_t drive_num)
{
    FRESULT			the_result;
	uint8_t			work_buffer[FF_MAX_SS];
	char			drive_path[2];
	const char*		the_drive_path = &drive_path[0];

	drive_path[0] = drive_num - 48;
	drive_path[1] = ':';
	
	the_result = f_mkfs(the_drive_path, 0, (void*)work_buffer, FF_MAX_SS);
	//FRESULT f_mkfs (const TCHAR* path, const MKFS_PARM* opt, void* work, UINT len);	/* Create a FAT volume */
	
	if (the_result != FR_OK)
	{
		EventManager_AddEvent(diskError, the_result, drive_num, 0);
		return -1;
	}
	
	// Give a work area to the specified drive
	the_result = f_mount(&global_ffs_device[drive_num], the_drive_path, PARAM_MOUNT_IMMEDIATELY);
    
	if (the_result != FR_OK)
	{
		EventManager_AddEvent(diskError, the_result, drive_num, 0);
		return -2;
	}
	
	return 0;
}


// perform a MkDir on the specified device, with the specified path
// returns false on any error
bool Kernal_MkDir(char* the_path, uint8_t drive_num)
{
    FRESULT				the_result;
    
    //the_path += 2;	// get past 0:, 1:, 2:, etc. 
    
    // TODO: split handling here once IEC device support is ready. 
    //   use drive_num to find out if it should use FFS or IEC calls 

	the_result = f_mkdir(the_path);
	
	if (the_result != FR_OK)
	{
		EventManager_AddEvent(diskError, the_result, drive_num, 0);
		return false;
	}

    return true;
}


// runs a name program (a KUP, in other words)
// pass the KUP name and length
// returns error on error, and never returns on success (because SuperBASIC took over)
void Kernal_RunNamed(char* kup_name, uint8_t name_len)
{
//     char			stream;
// 
// 	args.common.buf = kup_name;
// 	args.common.buflen = name_len;
// 
// 	stream = CALL(RunNamed);
    
    return; // just so cc65 is happy; but will not be hit in event of success as SuperBASIC will already be running.
}


// calls modojr and tells it to load the specified .mod file
// returns error on error, and never returns on success (because pexec took over)
bool Kernal_RunMod(char* the_path)
{
//     char		stream;
//     uint8_t		path_len;
// 	
// 	// kernel.args.buf needs to have name of named app to run, which in this case is '-' (pexec's real name)
// 	// we also need to prep a different buffer with a series of pointers (2), one of which points to a string for '-', one for 'SuperBASIC', and one for the basic program SuperBASIC should load
// 	// per dwsjason, these should be located at $200, with the pointers starting at $280. 
// 	//  'arg0- to pexec should be "-", then arg1 should be the name of the pgz you want to run, includign the whole path. 
// 	args.common.buf = (char*)0x0200; //"-";
// 	args.common.buflen = 2;
// 
// 	*(uint8_t*)(0x0200) = '-';
// 	*(uint8_t*)(0x0201) = 0;
// 	General_Strlcpy((char*)0x0202, "modo.pgz", 9);
// 	
//     // as of 2024-02-15, pexec doesn't support device nums, it always loads from 0:
//     the_path += 2;	// get past 0:, 1:, 2:, etc.     
// 	path_len = General_Strnlen(the_path, FILE_MAX_PATHNAME_SIZE)+1;
// 
// 	General_Strlcpy((char*)0x020b, the_path, path_len);
// 	
// 	args.common.ext = (char*)0x0280;
// 	args.common.extlen = 8;
// 
// 	// leave pointers for pexec so it knows where to find args
// 	*(uint8_t*)0x0280 = 0x00;
// 	*(uint8_t*)0x0281 = 0x02;	// first arg is at $0200
// 	*(uint8_t*)0x0282 = 0x02;
// 	*(uint8_t*)0x0283 = 0x02;	// second arg is at $0202
// 	*(uint8_t*)0x0284 = 0x0b;
// 	*(uint8_t*)0x0285 = 0x02;	// third arg (path to song file) is at $020b
// 	*(uint8_t*)0x0286 = 0x00;	// terminator
// 	
// 	stream = CALL(RunNamed);
//     
//     if (error) 
//     {
//         return false;
//     }
    
    return true; // just so cc65 is happy; but will not be hit in event of success as pexec will already be running.
}


// calls Pexec and tells it to run the specified path. 
// returns error on error, and never returns on success (because pexec took over)
bool Kernal_RunExe(char* the_path)
{
//     uint8_t		stream;
//     uint8_t		path_len;
//     int8_t		the_device_num;
// 	
// 	// kernel.args.buf needs to have name of named app to run, which in this case is '-' (pexec's real name)
// 	// we also need to prep a different buffer with a series of pointers (2), one of which points to a string for '-', one for '- filetorun.pgz'
// 	// per dwsjason, these should be located at $200, with the pointers starting at $280. 
// 	//  'arg0- to pexec should be "-", then arg1 should be the name of the pgz you want to run, includign the whole path. 
// 	args.common.buf = (char*)0x0200; //"-";
// 	args.common.buflen = 2;
// 	
//     // as of 2024-02-24, pexec now supports device nums, and if not found, will default to "0:"
//     //  this means that for SD card (0), we can leave it off, which keeps compatibility for people who haven't
//     //  updated to pexec v0.64 or newer. 
//     the_device_num = the_path[0] - CH_ZERO; // get to 0, 1, 2
//     
//     if (the_device_num < 1)
//     {
// 		the_path += 2;	// skip past 0: to keep compatibility with pre-0.64 versions of pexec     
//     }
// 	
// 	path_len = General_Strnlen(the_path, FILE_MAX_PATHNAME_SIZE)+1;
// 
// 	// set first arg: the named app (pexec in this case)
// 	*(uint8_t*)(0x0200) = '-';
// 	*(uint8_t*)(0x0201) = 0;
// 	// set second arg: the path
// 	General_Strlcpy((char*)0x0202, the_path, path_len);
// 	// leave pointers for pexec so it knows where to find args
// 	*(uint8_t*)0x0280 = 0x00;
// 	*(uint8_t*)0x0281 = 0x02;	// first arg is at $0200
// 	*(uint8_t*)0x0282 = 0x02;
// 	*(uint8_t*)0x0283 = 0x02;	// second arg is at $0202
// 	*(uint8_t*)0x0284 = 0x00;	// terminator
// 	
// 	args.common.ext = (char*)0x0280;
// 	args.common.extlen = 4;
// 	
// 	stream = CALL(RunNamed);
//     
//     if (error) 
//     {
//         return false;
//     }
    
    return true; // just so cc65 is happy; but will not be hit in event of success as pexec will already be running.
}

// Input
// • kernel.args.buf points to a buffer containing the name of the program to run. 
// • kernel.args.buflen contains the length of the name.
// Output
// • On success, the call doesn’t return.
// • Carry set on error (a program with the provided name was not found).
// Notes
// • The name match is case-insensitive.


//https://github.com/FoenixRetro/Documentation/blob/main/f256/programming-developing.md

// Parameter Passing
// 
// Although not part of the kernel specification, a standardized method of passing commandline arguments to programs exists.
// 
// Both DOS and SuperBASIC are able to pass arguments to the program to run, and pexec is also able to pass any further arguments after the filename on to the program. As an example, /- program.pgz hello in SuperBASIC would start pexec with the parameters -, program.pgz, and hello. pexec would then load program.pgz, and start it with the parameters program.pgz and hello.
// 
// Arguments are passed in the ext and extlen kernel arguments. This approach is suitable for passing arguments through the RunNamed and RunBlock kernel functions, and is also used by pexec when starting a PGX or PGZ program.
// 
// ext will contain an array of pointers, one for each argument given on the commandline. The first pointer is the program name itself. The list is terminated with a null pointer. extlen contains the length in bytes of the array, less the null pointer. For instance, if two parameters are passed, extlen will be 4.
// 
// pexec reserves $200-$2FF for parameters - programs distributed in the PGX and PGZ formats should therefore load themselves no lower than $0300, if they want to access commandline parameters. If they do not use the commandline parameters, they may load themselves as low as $0200.