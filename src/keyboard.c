/*
 * keyboard.c
 *
 *  Created on: Aug 23, 2022
 *      Author: micahbly
 *
 */

// adapted for (Lich King) Foenix F256 Jr starting November 30, 2022
// adapted for f/manager Foenix F256 starting March 10, 2024




/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "keyboard.h"
#include "comm_buffer.h"	// only needed if debugging
#include "debug.h"
#include "event.h"
#include "general.h"
#include "memory.h"

// C includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// F256 includes
#include "f256_e.h"


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define MINUTE_TIMER_COOKIE		127		// hard-coded. just don't want it to start with 0, as that's what the keyboard cookie will start with

#define KEYBOARD_QUEUE_SIZE		8




/*****************************************************************************/
/*                          File-scope Variables                             */
/*****************************************************************************/

static uint8_t			keyboard_queue_entries;
static uint8_t			keyboard_queue[KEYBOARD_QUEUE_SIZE];
static KeyRepeater		keyboard_repeater;



/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

extern char* 			global_string_buff1;	// just need for debugging


extern struct call_args args; // in gadget's version of f256 lib, this is allocated and initialized with &args in crt0. 
extern struct event_t event; // in gadget's version of f256 lib, this is allocated and initialized with &event in crt0. 
extern char error;


extern uint8_t	zp_temp_4;


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// Pop head of keyboard queue
uint8_t Keyboard_PopQueue(void);

// Calls kernel.nextEvent but also updates keyboard state events.
EventRecord* Keyboard_GetNextEvent(void);

// Process a key PRESSED/RELEASED, updating key status bit array
void Keyboard_ProcessKeyEvent(EventRecord* the_event);

// passed a keycode, it starts a timer tracking if that key is held down
void Keyboard_StartTimerForKey(uint8_t the_key);

// schedule a repeat event
// pass the frame count of requested next event
void Keyboard_ScheduleRepeatEvent(uint8_t next_frame_count);

// add a character to the key buffer, if space is available
void Keyboard_AddToQueue(uint8_t the_char);

// schedule a repeat event for the minute clock
void Keyboard_ScheduleMinuteHandRepeatEvent(void);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// Pop head of keyboard queue
uint8_t Keyboard_PopQueue(void)
{
	uint8_t		this_key;
	uint8_t		i;

	if (keyboard_queue_entries == 0)
	{
		return 0;
	}
	
	this_key = keyboard_queue[0];
	
	for (i = 0; i < (KEYBOARD_QUEUE_SIZE-1); i++)
	{
		keyboard_queue[i] = keyboard_queue[i+1];
	}
	
	--keyboard_queue_entries;
	
	return this_key;
}


// Calls kernel.nextEvent but also updates keyboard state events.
EventRecord* Keyboard_GetNextEvent(void)
{
	EventRecord*	the_event;
	
	//DEBUG_OUT(("%s %d: write_idx_=%i, read_idx_=%i, the_mask=%x", __func__, __LINE__, the_event_manager->write_idx_, the_event_manager->read_idx_, the_mask));

	if ( (the_event = EventManager_NextEvent()) != NULL)
	{
		DEBUG_OUT(("%s %d: Received Event: type=%i", __func__, __LINE__, the_event->what_));
		//Event_Print(the_event);
		
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
						
			case EVENT_KEYDOWN:
				DEBUG_OUT(("%s %d: key down event: '%c' (%x) mod (%x)", __func__, __LINE__, the_event->key_.raw_, the_event->key_.raw_, the_event->key_.modifiers_));

				return the_event;		

				break;
			
			case EVENT_KEYUP:
				DEBUG_OUT(("%s %d: key up event: '%c' (%x) mod (%x)", __func__, __LINE__, the_event->key_.raw_, the_event->key_.raw_, the_event->key_.modifiers_));

				return the_event;				

				break;

			default:
				DEBUG_OUT(("%s %d: other event: %i", __func__, __LINE__, the_event->what_));
				
				break;
		}
	}

	// We have a keyboard event. (which includes possibility of joystick event, on F256)
	//Keyboard_ProcessKeyEvent();
	//this_status = Keyboard_UpdateKeyboardJoystick();
	
	return NULL;
}


// Process a key PRESSED/RELEASED interrupt, updating key status bit array
// returns a mapped key code
void Keyboard_ProcessKeyEvent(EventRecord* the_event)
{
	uint8_t this_char;

	//sprintf(global_string_buff1, "Keyboard_ProcessKeyEvent: getnext returned what=%x, raw=%x, mapped=%x", the_event->what_, the_event->code_raw_, the_event->code_mapped_);
	//Buffer_NewMessage(global_string_buff1);
	
	if (the_event->what_ == EVENT_KEYDOWN)
	{
// 			if (event.key.flags)
// 			{
// 				// if a function key, return raw code.
// 				if (event.key.raw >= KEY_F1 && event.key.raw <= KEY_F12)
// 				{
// 					this_char = event.key.raw;
// 				}
// 				else
// 				{
// 					// if it wasn't an F key, it was a modifier key, we just ignore it.
// 					add_char_to_queue = false;
// 				}
// 			}
// 			else
// 			{
// 				this_char = event.key.ascii;
// 			}
// 			
// 			if (add_char_to_queue)
// 			{
// 				// Schedule repeats for keys from CBM/K keyboards
// 				if (event.key.keyboard == 0)
// 				{
// 					// for f/manager, we only want repeat on cursor keys, delete, backspace, etc.
// 					if (this_char == KEY_CURS_UP || this_char == KEY_CURS_DOWN || this_char == KEY_CURS_LEFT || this_char == KEY_CURS_RIGHT || this_char == KEY_DEL || this_char == KEY_BREAK)
// 					Keyboard_StartTimerForKey(this_char);
// 				}
// 
// 				// for any keyboard type, add this char to the key buffer
// 				Keyboard_AddToQueue(this_char);				
// 			}

			this_char = the_event->key_.char_;
			Keyboard_AddToQueue(this_char);	
	}
// 	else
// 	{
// 		// jmp     StopRepeat WHICH IS "inc     repeat.cookie -> rts"
// 		keyboard_repeater.cookie++;
// 
// 		// prevent collision with the permanent minute hand cookie		
// 		if (keyboard_repeater.cookie == MINUTE_TIMER_COOKIE)
// 		{
// 			keyboard_repeater.cookie++;
// 		}
// 	}

}


// passed a keycode, it starts a timer tracking if that key is held down
void Keyboard_StartTimerForKey(uint8_t the_key)
{
	uint8_t		current_timer_value = 0;
	
	keyboard_repeater.key = the_key;
	keyboard_repeater.cookie++;			// set a new ID
		
	// prevent collision with the permanent minute hand cookie
	if (keyboard_repeater.cookie == MINUTE_TIMER_COOKIE)
	{
		keyboard_repeater.cookie++;
	}
	
	// Get the current frame counter
	// including query makes the SetTimer call return the value of the current timer (in A)
// 	args.timer.units = (TIMER_FRAMES | TIMER_QUERY);
// 	current_timer_value = CALL(Clock.SetTimer);

	// Schedule a timer approx 0.75s in the future (repeat delay).
	Keyboard_ScheduleRepeatEvent(current_timer_value + 15);
}


// schedule a repeat event
// pass the frame count of requested next event
void Keyboard_ScheduleRepeatEvent(uint8_t next_frame_count)
{
// 	args.timer.absolute = next_frame_count;	
// 	args.timer.units = TIMER_FRAMES;
// 	args.timer.cookie = keyboard_repeater.cookie;
// 	
// 	CALL(Clock.SetTimer);
}


// schedule a repeat event for the minute clock
void Keyboard_ScheduleMinuteHandRepeatEvent(void)
{
// 	args.timer.absolute = 60;	
// 	args.timer.units = TIMER_SECONDS;
// 	args.timer.cookie = MINUTE_TIMER_COOKIE;
// 	
// 	CALL(Clock.SetTimer);
}


// initiate the minute hand timer
void Keyboard_InitiateMinuteHand(void)
{
	Keyboard_ScheduleMinuteHandRepeatEvent();
}


// returns 0 if it determined there was no repeat (yet). returns a key code if event resulted in a repeat.
uint8_t Keyboard_HandleRepeatTimerEvent(void)
{
// 	// before checking for keyboard repeats, check if this is our minute-timer cookie
// 	if (event.timer.cookie == MINUTE_TIMER_COOKIE)
// 	{
// 		App_DisplayTime();
// 		Keyboard_ScheduleMinuteHandRepeatEvent();	// schedule the next one
// 		return 0;
// 	}
// 
// 	// ignore retired timers
// 	if (event.timer.cookie != keyboard_repeater.cookie)
// 	{
// 		return 0;
// 	}
// 	
// 	// Schedule the next repeat for ~0.05s from now.
// 	Keyboard_ScheduleRepeatEvent(event.timer.value + 3);
// 	
// 	// Return the key being repeated.
// 	return keyboard_repeater.key;

	return 0;
}


// add a character to the key buffer, if space is available
void Keyboard_AddToQueue(uint8_t the_char)
{
	// check there is space in the keyboard buffer to put this char into
	if (keyboard_queue_entries == KEYBOARD_QUEUE_SIZE)
	{
		return;
	}
	
	// add to queue
	keyboard_queue[keyboard_queue_entries] = the_char;
	keyboard_queue_entries++;
}


// main event processor
void Keyboard_ProcessEvents(void)
{
	uint8_t			repeated_char;
	EventRecord*	the_event;
	
	while(1)
	{		
		if ((the_event = Keyboard_GetNextEvent()) == NULL)
		{
			return;
		}
		
// 		if (event.type == EVENT(timer.EXPIRED))
// 		{
// 			if ((repeated_char = Keyboard_HandleRepeatTimerEvent()) != 0)
// 			{
// 				Keyboard_AddToQueue(repeated_char);
// 			}		
// 		}
		if ((the_event->what_ == EVENT_KEYDOWN) || (the_event->what_ == EVENT_KEYUP))
		{
			Keyboard_ProcessKeyEvent(the_event);
		}
	}	
}



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// **** USER INPUT UTILITIES *****




// Wait for one character from the keyboard and return it
char Keyboard_GetChar(void)
{
	uint8_t		the_char = 0;
	
	//DEBUG_OUT(("%s %d: entered", __func__, __LINE__));

	Keyboard_ProcessEvents();

	do
	{
		the_char = Keyboard_GetKeyIfPressed();
	} while (the_char == 0);

	//DEBUG_OUT(("%s %d: key input=%x", __func__, __LINE__, the_char));
	
	return the_char;
}


// Check to see if keystroke events pending - does not wait for a key
uint8_t Keyboard_GetKeyIfPressed(void)
{
	// if there is anything in the queue, pop it and return it.
	if (keyboard_queue_entries > 0)
	{
		return Keyboard_PopQueue();
	}
 		
	// process any outstanding events
	Keyboard_ProcessEvents();
	
	return 0;
}
