/*
 * startup.h
 *
 *  Created on: Apr 5, 2024
 *      Author: micahbly
 */
 
// Adapted from F256jr/k version of FileManager overlay_startup starting June 2, 2024

#ifndef OVERLAY_STARTUP_H_
#define OVERLAY_STARTUP_H_

/* about this class
 *
 *  Routines for starting up f/manager, including show splash screen(s)
 *
 */

/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

#include "app.h"
#include "text.h"
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
/*                       Public Function Prototypes                          */
/*****************************************************************************/


// clear screen and show app (foenix) logo, and machine logo if running from flash
void Startup_ShowLogo(void);

// enable the random number generator, and seed it
void Startup_InitializeRandomNumGen(void);


#endif /* OVERLAY_STARTUP_H_ */
