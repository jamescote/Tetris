/* Includes */
#include "input.h"
#include <osbind.h>

/* Local Defines */
#define MFP_IN_SERVICE_B_FLAG	0xBF
#define MIDI_CNTRL_FLAG 		0xBF
#define KEYBOARD_INTERRUPT_MASK 6
#define DATA_READY_MASK			1
#define BREAK_MASK				0x80
#define MOUSE_HEADER			0xF8
#define LEFT_MOUSE_BTN_MASK		2
#define RIGHT_MOUSE_BTN_MASK	1
#define EMPTY					0
#define STARTING_DURATION		16
#define SUBSEQUENT_DURATION		8

/* States for KYBD FSM */
#define KYBD_OPEN_STATE			0
#define KYBD_X_POS_STATE		1
#define KYBD_Y_POS_STATE		2

/* Type Definitions */
#define buffer_head cKYBDBuffer[ cBffrHead ]
#define buffer_tail cKYBDBuffer[ cBffrTail ]
typedef UINT8 SCANCODE;

UINT8* const MFP_IN_SERVICE_B 	= 0xFFFFFA11;
UINT8* const MIDI_CNTRL_ADDR	= 0xFFFFFC04;

const UINT8* 	const IKBD_STATUS	= 0xFFFFFC00;
const SCANCODE* const IKBD_DATA		= 0xFFFFFC02;

/* local function declarations */
int set_ipl( int mask );
void handle_scancode( SCANCODE bInput );
UINT8 pop_head( );
void add_key( SCANCODE bCode );

/* Global Keyboard Data Declarations */
typedef struct sKey
{
	SCANCODE bScanCode;
	bool bValid;
}sKey;

typedef struct sKeyMakeState
{
	UINT16 wDuration;
	bool bMakeReceived;
}sKeyMakeState;

sKey cKYBDBuffer[ MAX_BUFFER_SIZE ];
sKeyMakeState cMakeTable[ MAX_BUFFER_SIZE ];
UINT8 bHotKeys[ MAX_BUFFER_SIZE ];
UINT8 cHotKeyCount	= 0;
UINT8 cBffrHead 	= 0;
UINT8 cBffrTail 	= 0;
UINT8 cFill 		= 0;
sMouse m_MouseModel = { 0,0 };
UINT8 bKYBD_STATE 	= KYBD_OPEN_STATE;

/*
	Returns whether there is pending input or not.
*/
bool inputPending( )
{
	int old_ssp = Super( 1 );
	int old_ipl;
	
	if( USER_MODE == old_ssp )
		old_ssp = Super( 0 );
		
	old_ipl = set_ipl( KEYBOARD_INTERRUPT_MASK );
	
	while( cBffrHead != cBffrTail && !cKYBDBuffer[ cBffrHead ].bValid )
		pop_head( );
		
	set_ipl( old_ipl );
	
	if( SUPER_MODE != old_ssp )
		Super( old_ssp );
	
	return EMPTY != cFill;
}

/*
	Set up a list of hotkeys for the update method to check.
*/
void initializeHotKeys( UINT8 bScanCodes[], UINT8 bCount )
{
	UINT8 i;
	
	cHotKeyCount = bCount;
	
	for( i = 0; i < bCount; i++ )
	{
		bHotKeys[ i ] = bScanCodes[ i ];
	}
}

/*
	Clears the Input Buffer.
*/
void resetBuffer( )
{
	cBffrHead = cBffrTail;
	cFill = 0;
}

/*
	Returns the next Character in the buffer.
	Returns 0x80 if an error occurs and there is nothing in the buffer to return.
*/
UINT8 getInput( )
{
	return pop_head( );
}

/* 
	Sets the 6th bit of the MIDI Control register to set MIDI interrupts to be on or off.
*/
void set_MIDI_Cntrl( bool bOn )
{
	int old_ssp = Super( 1 );
	
	if( USER_MODE == old_ssp )
		old_ssp = Super( 0 );
		
	if( bOn )
		*MIDI_CNTRL_ADDR |= ~MIDI_CNTRL_FLAG;
	else 
		*MIDI_CNTRL_ADDR &= MIDI_CNTRL_FLAG;
		
	if( SUPER_MODE != old_ssp )
		Super( old_ssp );
}

/*
	Function for updating the keyboard buffer and pushing back codes that haven't
	received a break code yet.
*/
void update_input_buffer( UINT8 bTimeElapsed )
{
	UINT8 bIndex = 0;
	int old_ssp = Super( 1 );
	int old_ipl;
	
	if( USER_MODE == old_ssp )
		old_ssp = Super( 0 );
	
	old_ipl = set_ipl( KEYBOARD_INTERRUPT_MASK );
	
	for( bIndex; bIndex < cHotKeyCount; bIndex++ )
	{
		if( cMakeTable[ bHotKeys[ bIndex ] ].bMakeReceived )
		{
			cMakeTable[ bHotKeys[ bIndex ] ].wDuration -= bTimeElapsed;
			
			if( cMakeTable[ bHotKeys[ bIndex ] ].wDuration == 0 || 
				cMakeTable[ bHotKeys[ bIndex ] ].wDuration > STARTING_DURATION )
			{
				add_key( bHotKeys[ bIndex ] );
				cMakeTable[ bHotKeys[ bIndex ] ].wDuration = SUBSEQUENT_DURATION;
			}
		}
	}
	
	set_ipl( old_ipl );
	
	if( SUPER_MODE != old_ssp )
		Super( old_ssp );
}

/*
	Reads in data from the Keyboard and handles it accordingly.
*/
void do_kybd_isr( )
{
	SCANCODE bInput;
	
	/* Handle Key Board ISR Stuff */
	if( *IKBD_STATUS & DATA_READY_MASK )
	{
		bInput = *IKBD_DATA;
		
		switch( bKYBD_STATE )
		{
		case KYBD_OPEN_STATE:
			if( MOUSE_HEADER == (bInput & ~(LEFT_MOUSE_BTN_MASK + RIGHT_MOUSE_BTN_MASK)) )
				bKYBD_STATE = KYBD_X_POS_STATE;
			else
				handle_scancode( bInput );
			break;
		case KYBD_X_POS_STATE: /* Not Implemented Yet */
			bKYBD_STATE = KYBD_Y_POS_STATE;
			break;
		case KYBD_Y_POS_STATE: /* Not Implemented Yet */
			bKYBD_STATE = KYBD_OPEN_STATE;
			break;
		};
	}	
	
	/* Clear bit #6 on the MFP68901 Interrupt In-Service B register */
	*MFP_IN_SERVICE_B &= MFP_IN_SERVICE_B_FLAG;
}

/*
	Handles the scancode received in accordingly depending on make and break codes.
*/
void handle_scancode( SCANCODE bInput )
{
	UINT8 bIndex = cBffrHead;
	
	if( bInput & BREAK_MASK )	/* Break Code */
	{
		bInput &= ~BREAK_MASK;
		
		cMakeTable[ bInput ].bMakeReceived = false;
	}
	else						/* Make Code */
	{
		add_key( bInput );
	}
}

/* 
	Adds a key to the buffer based on the passed in scancode.
*/
void add_key( SCANCODE bCode )
{
	int old_ssp = Super( 1 );
	int old_ipl;
	
	if( USER_MODE == old_ssp )
		old_ssp = Super( 0 );
	
	old_ipl = set_ipl( KEYBOARD_INTERRUPT_MASK );
	
	if( cFill < MAX_BUFFER_SIZE )
	{
		cKYBDBuffer[ cBffrTail ].bScanCode 		= bCode;
		cKYBDBuffer[ cBffrTail ].bValid  		= true;
		cFill++;
		cBffrTail++;
		
		cMakeTable[ bCode ].bMakeReceived 	= true;
		cMakeTable[ bCode ].wDuration 		= STARTING_DURATION;
	}
	
	set_ipl( old_ipl );
	
	if( SUPER_MODE != old_ssp )
		Super( old_ssp );
}

/*
	Removes the head of the buffer and returns the associated ScanCode.
*/
UINT8 pop_head( )
{
	UINT8 bReturnCode = BREAK_MASK;
	int old_ssp = Super( 1 );
	int old_ipl;
	
	if( USER_MODE == old_ssp )
		old_ssp = Super( 0 );
	
	old_ipl = set_ipl( KEYBOARD_INTERRUPT_MASK );
	
	if( cBffrHead != cBffrTail )
	{
		cFill -= 1;
		bReturnCode = cKYBDBuffer[ cBffrHead ].bScanCode;
		cBffrHead++;
	}
	
	set_ipl( old_ipl );
	
	if( SUPER_MODE != old_ssp )
		Super( old_ssp );
		
	return bReturnCode;
}