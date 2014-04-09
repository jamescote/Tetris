#ifndef _INPUT_H
#define _INPUT_H

#include "types.h"

#define MAX_BUFFER_SIZE 	256
#define KYBD_VECTOR			70

/* Mouse Structure */
typedef struct mouse
{
	UINT16 xPos, yPos;
}sMouse;

/* Public Function Declarations */
bool inputPending( );
UINT8 getInput( );
void do_kybd_isr( );
void set_MIDI_Cntrl( bool bOn );
void update_input_buffer( UINT8 bTimeElapsed );
void initializeHotKeys( UINT8 bScanCodes[], UINT8 bCount );
void resetBuffer( );

#endif