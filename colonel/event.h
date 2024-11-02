//! @file event.h

/*
 * event.h
 *
*  Created on: Apr 16, 2022
 *      Author: micahbly
 */

// F256K2 adaptation/re-start started June 9, 2024 
// This is a re-start from scratch of the OS/f event.c/event.h done in 2022, for use with f/manager F256K2
// Code from the OS/f version may eventually be copied into here, depending on how things go

#ifndef EVENT_H_
#define EVENT_H_


/* about this class: Event Manager
 *
 * Provides structures and functions for queueing events

 * NOTE: Event structures and style are largely based on Apple's old (pre-OS X) events.h
 *   I have adapted for Foenix realities, and for my style, and added a couple of conveniences
 *     A couple of conveniences added in style of Amiga Intuition: the window and control are available directly from the event record
 *   There is no expectation that somebody's old mac code would work
 *   however, it should be familiar in feel to anyone who programmed macs before OS X

 *
 *** things this class needs to be able to do
 * Provide interrupt handlers that turn mouse and keyboard actions into events
 * 
 *
 * 
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
//#include "general.h"
//#include "text.h"
//#include "sys.h"


// C includes
#include <stdbool.h>


// F256 includes
#include "f256_e.h"


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define EVENT_QUEUE_SIZE	64		//! number of event records in the circular buffer

// timing-related settings. In a bigger OS, these would probably be settable by individual user preference.

#define EVENT_KEYBOARD_REPEAT_RTC_RATE			FLAG_RTC_RATE_63MS	// the RTC rate value between key repeat actions
#define EVENT_KEYBOARD_REPEAT_START_SKIP_COUNT	8					// how many RTC interrupt events are skipped before registering a newly held-down key as going into repeat mode
#define EVENT_CLOCK_DISPLAY_SKIP_COUNT			16					// how many RTC interrupt events are skipped before bothering to update clock display.




/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/

typedef enum event_kind 
{
	nullEvent				= 0,
	mouseDown				= 1,
	mouseUp					= 2,
	keyDown					= 3,
	keyUp					= 4,
	autoKey					= 5,
	updateEvt				= 6,
	diskEvt					= 7,
	activateEvt				= 8,
	inactivateEvt			= 9,
	rMouseDown				= 10,
	rMouseUp				= 11,
	menuOpened				= 12,
	menuSelected			= 13,
	menuCanceled			= 14,
	controlClicked			= 15,
	mouseMoved				= 16,
	windowChanged			= 17,
	diskError				= 18,
} event_kind;

// Macro definitions of above, to save a byte
#define EVENT_NULL			0
#define EVENT_KEYDOWN		1
#define EVENT_KEYUP			2

typedef enum event_mask 
{
	mouseDownMask			= 1 << mouseDown,		// mouse button pressed
	mouseUpMask				= 1 << mouseUp,			// mouse button released
	keyDownMask				= 1 << keyDown,			// key pressed
	keyUpMask				= 1 << keyUp,			// key released
	autoKeyMask				= 1 << autoKey,			// key repeatedly held down
	updateMask				= 1 << updateEvt,		// window needs updating
	diskEvtMask				= 1 << diskEvt,			// disk inserted
	activateEvtMask			= 1 << activateEvt,		// activate window
	inactivateEvtMask		= 1 << inactivateEvt,	// deactivate window
	rMouseDownMask			= 1 << rMouseDown,		// right mouse button pressed
	rMouseUpMask			= 1 << rMouseUp,		// right mouse button released
	menuOpenedMask			= 1 << menuOpened,		// contextual menu opened
	menuSelectedMask		= 1 << menuSelected,	// item from the contextual menu selected
	menuCanceledMask		= 1 << menuCanceled,	// contextual menu closed without an item being selected
	controlClickedMask		= 1 << controlClicked,	// a clickable (2 state) control has been clicked
	mouseMovedMask			= 1 << mouseMoved,		// mouse has been moved
	windowChangedMask		= 1 << windowChanged,	// a window has changed size and/or position
	diskErrorMask			= 1 << diskError,		// a disk / file-system error has occurred
	everyEvent				= 0xFFFF				// all of the above
} event_mask;


typedef enum event_modifiers
{
	activeFlagBit			= 0,    // activate? (activateEvt and mouseDown)
	actionKeyBit			= 1,    // was this an action key, rather than glyph key? e.g., cursors, enter, backspace, del vs 'A', SPACE
	btnStateBit				= 7,    // state of button?
	foenixKeyBit			= 8,    // foenix key down?
	shiftKeyBit				= 9,    // shift key down?
	alphaLockBit			= 10,   // alpha lock down?
	optionKeyBit			= 11,   // option key down?
	controlKeyBit			= 12,   // control key down?
// 	rightShiftKeyBit		= 13,   // right shift key down?
// 	rightOptionKeyBit		= 14,   // right Option key down?
// 	rightControlKeyBit		= 15    // right Control key down?
} event_modifiers;

#define KEY_FLAG_ACTION		0b00000001	// set if key represents an action key, rather than glyph-generating key (DEL vs 'A', e.g.)
#define KEY_FLAG_SHIFT		0b00000010	// one or both SHIFT keys are down
#define KEY_FLAG_ALT		0b00000100
#define KEY_FLAG_CTRL		0b00001000
#define KEY_FLAG_META		0b00010000	// Foenix/CMD/Windows key
#define KEY_FLAG_CAPS		0b00100000

typedef enum event_modifier_flags
{
	activeFlag				= 1 << activeFlagBit,
	actionKey				= 1 << actionKeyBit,
	btnState				= 1 << btnStateBit,
	foenixKey				= 1 << foenixKeyBit,
	shiftKey				= 1 << shiftKeyBit,
	alphaLock				= 1 << alphaLockBit,
	optionKey				= 1 << optionKeyBit,
	controlKey				= 1 << controlKeyBit,
// 	rightShiftKey			= 1 << rightShiftKeyBit,
// 	rightOptionKey			= 1 << rightOptionKeyBit,
// 	rightControlKey			= 1 << rightControlKeyBit
} event_modifier_flags;

// keyboard source for event struct
#define EVENT_KEY_SOURCE_INTERNAL	0	// optical in K2, or mechanical in K1
#define EVENT_KEY_SOURCE_EXTERNAL	1	// ps/2


// TODO: localize this for A2560
enum
{
	kNullCharCode                 = 0,
	kHomeCharCode                 = 1,
	kEnterCharCode                = 3,
	kEndCharCode                  = 4,
	kHelpCharCode                 = 5,
	kBellCharCode                 = 7,
	kBackspaceCharCode            = 8,
	kTabCharCode                  = 9,
	kLineFeedCharCode             = 10,
	kVerticalTabCharCode          = 11,
	kPageUpCharCode               = 11,
	kFormFeedCharCode             = 12,
	kPageDownCharCode             = 12,
	kReturnCharCode               = 13,
	kFunctionKeyCharCode          = 16,
	kEscapeCharCode               = 27,
	kClearCharCode                = 27,
	kLeftArrowCharCode            = 28,
	kRightArrowCharCode           = 29,
	kUpArrowCharCode              = 30,
	kDownArrowCharCode            = 31,
	kDeleteCharCode               = 127,
	kNonBreakingSpaceCharCode     = 202
};



/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

struct EventKeyboard {
//	event_modifiers		modifiers_;	//! set for keyboard and mouse events
	uint8_t				key_;		//! the key code of the key pushed. eg, KEY_BKSP (0x92), not CH_BKSP (0x08). Most useful for handling action keys such as cursors, DEL, BS, ESC, etc.
	uint8_t				char_;		//! the character code resulting from the key, after mapping. e.g, 1 may return 49, ALT-1 may return 145, SHIFT-1 may return 33; backspace may return 8, etc.
	uint8_t				modifiers_;	//! bit flags for shift, ctrl, meta, etc. 
	uint8_t				source_;	//! 0=internal keyboard, 1=external ps/2 keyboard
//     char    ascii;
//     char    flags;  // negative for no associated ASCII.
//     struct event_timer_t event_timer;
};

#ifdef _OSF_
struct EventMouse {
	event_modifiers		modifiers_;	//! set for keyboard and mouse events
	int16_t				x_;			//! for mouse events: the global x position of mouse.
	int16_t				y_;			//! for mouse events: the global y position of mouse.
	Control*			control_;	//! for window events: the effected control. if not set on mouseDown/Up, pointer was not over a control
};
#endif

#ifdef _OSF_
struct EventWindow {
	event_modifiers		modifiers_;	//! set for keyboard and mouse events
	int16_t				x_;			//! for window events: the new global x position of the window.
	int16_t				y_;			//! for window events: the new global y position of the window.
	Control*			control_;	//! for window events: the effected control. if not set on mouseDown/Up, pointer was not over a control
	int16_t				width_;		//! for window events: the new width of the window.
	int16_t				height_;	//! for window events: the new height of the window.
	Window*				window_;	//! for window events: the affected window
};
#endif

// For 8-bits, EventRecord is pretty gutted. 
typedef struct EventRecord
{
	#ifdef _OSF_
		uint32_t		when_;		//! ticks - implement when timer code is in place
		event_kind		what_;
	#else
		uint8_t			what_;		//! what kind of event this is. Use EVENT_NULL, EVENY_KEYUP, etc. 
	#endif
	union {
		struct EventKeyboard	key_;
		#ifdef _OSF_
			struct EventMouse		mouse_;
			struct EventWindow		window_;
		#endif
	};
} EventRecord;

// MB: this is the older, pre-union version. for reference...
// typedef struct EventRecord
// {
// 	event_kind			what_;
// #ifdef _OSF_
// 	uint32_t			when_;		//! ticks - implement when timer code is in place
// #endif
// 	uint8_t				code_raw_;
// 	uint8_t				code_mapped_;
// // 	uint32_t			code_;		//! For keydown, keyup: the mapped key code in the low byte, raw key code in the 2nd-lowest byte. 
// // 									//! For windowChanged, the width in the high word, height in the low word.
// 	event_modifiers		modifiers_;	//! set for keyboard and mouse events
// #ifdef _OSF_
// 	uint32_t			when_;		//! ticks
// 	Window*				window_;	//! not set for a diskEvt
// 	Control*			control_;	//! not set for every event type. if not set on mouseDown/Up, pointer was not over a control
// #endif
// } EventRecord;

typedef struct EventManager
{
	EventRecord			queue_[EVENT_QUEUE_SIZE];	//! circular buffer for the event queue
// 	EventRecord*		queue_[EVENT_QUEUE_SIZE];	//! circular buffer for the event queue
	uint8_t				write_idx_;					//! index to queue_: where the next event record will be slotted
	uint8_t				read_idx_;					//! index to queue_: where the next event record will be read from
//	MouseTracker*		mouse_tracker_;				//! tracks whether mouse is in drag mode, etc.
} EventManager;





/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/


// **** events are pre-created in a fixed size array on system startup (circular buffer)
// **** as interrupts need to add more events, they take the next slot available in the array

// **** CONSTRUCTOR AND DESTRUCTOR *****





// **** Queue Management functions *****

//! Checks to see if there is an event in the queue
//! returns NULL if no event (not the same as returning an event of type nullEvent)
EventRecord* EventManager_NextEvent(void);

//! Add a new event to the event queue
//! NOTE: this does not actually insert a new record, as the event queue is a circular buffer
//! It overwrites whatever slot is next in line
//! @param	the_window: this may be set for non-mouse up/down events. For mouse up/down events, it will not be set, and X/Y will be used to find the window.
void EventManager_AddEvent(event_kind the_what, uint8_t the_raw_code,  uint8_t the_mapped_code, event_modifiers the_modifiers);

// Process one key action from the interrupt, convert raw code, and turn it into an event
void Event_ProcessPS2KeyboardInterrupt(void);

#if defined _F256K_ || defined _F256K2_

	// get the built-in keyboard on the F256K ready
	void Event_InitializeKeyboard256K(void);

	// set RTC 'rates' timer interrupt so we can update clock display
	void Event_InitializeRTCInterrupts(void);

	// Scan F256K built-in keyboard for an event
	void Event_ScanF256KMechKeyboard(void);
	void Event_ScanF256KOpticalKeyboard(void);

#endif


// **** Debug functions *****



#endif /* EVENT_H_ */


