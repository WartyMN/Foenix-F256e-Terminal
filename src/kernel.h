//! @file kernel.h

/*
 * kernel.h
 *
 *  Created on: 2023
 *      Author: micahbly
 */

// Adapted from F256jr/k version of FileManager starting June 2, 2024
//   Originally this supported the F256jk/k microkernel by gadget, with maybe 50/50 my code.
//   For F256K2 (extended/flat memory map), f/manager isn't using the microkernel, so it is now scaled back and totally re-written

#ifndef KERNEL_H_
#define KERNEL_H_


/* about this class
 *
 *
 *
 *** things this class needs to be able to do
 * Provide medium-level file-system access to the low-level FatFS calls
 *
 *** things objects of this class have
 *
 *
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/


// C includes
#include <stdbool.h>
#include <stdint.h>


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/


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

 

// perform a MkDir on the specified device, with the specified path
// returns false on any error
bool Kernal_MkDir(char* the_folder_path, uint8_t drive_num);

// calls Pexec and tells it to run the specified path. 
// returns error on error, and never returns on success (because pexec took over)
bool Kernal_RunExe(char* the_path);

// runs a name program (a KUP, in other words)
// pass the KUP name and length
// returns error on error, and never returns on success (because SuperBASIC took over)
void Kernal_RunNamed(char* kup_name, uint8_t name_len);

// calls modojr and tells it to load the specified .mod file
// returns error on error, and never returns on success (because pexec took over)
bool Kernal_RunMod(char* the_path);

// deletes the file at the specified path
// returns false in all error conditions
bool Kernel_DeleteFile(const char* the_path);

// deletes the folder at the specified path
// returns false in all error conditions
bool Kernel_DeleteFolder(const char* the_path);

// renames the specified folder/file to the new name
// returns false in any error condition
bool Kernel_Rename(const char* old_path, const char *new_path);

// Format a FatFS volume
//   pass the name you want for the formatted disk/SD card in name, and the drive number (0-2) in the drive param.
//   do NOT prepend the path onto name. 
// returns negative number on any error
int Kernal_Mkfs(const char* name, const uint8_t drive_num);


#endif /* KERNEL_H_ */
