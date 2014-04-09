#include "input.h"
#include "types.h"
#include "gm_cnsts.h"
#include "tvnt_hnd.h"
#include <osbind.h>
#include <stdio.h>

const long* cc_ClockPtr = (long*)0x462;
void kybd_isr( );
bool did_clock_tick( );
long get_time( );
Vector install_vector( int num, Vector vector );

long lPrevTick;

int main( )
{
	Vector pOld_KYBD_Vector;
	UINT8 bInput;
	UINT8 bHotKeys[ MAX_BUFFER_SIZE ];
	UINT8 bHotKeyCount;
	
	getHotKeys( bHotKeys, &bHotKeyCount );
	initializeHotKeys( bHotKeys, bHotKeyCount );

	
	set_MIDI_Cntrl( false );
	
	pOld_KYBD_Vector = install_vector( KYBD_VECTOR, kybd_isr );

	while( bInput != QUIT )
	{
		if( did_clock_tick( ) )
		{
			update_input_buffer( get_time( ) - lPrevTick );
			lPrevTick = get_time( );
		}
		/*printf( inputPending() ? "input is pending\n" : "no input pending\n" );*/
		TEST_displayMakeTable( );
		if( inputPending( ) )
		{
			bInput = getInput( );
			printf( "0x%2X\n", bInput );
		}
	}
	
	install_vector( KYBD_VECTOR, pOld_KYBD_Vector );
	
	set_MIDI_Cntrl( true );
	
	return 0;
}

bool did_clock_tick( )
{
	long lClockTick = get_time( );
	bool bClockTicked = lClockTick > lPrevTick;
	
	return bClockTicked;
}

long get_time( )
{
	int old_ssp = Super( 0 );
	long lClockTick = *cc_ClockPtr;
	
	Super( old_ssp );
	
	return lClockTick;
}

/*
	Install a vector at a provided vector position.
*/
Vector install_vector( int num, Vector vector )
{
	Vector orig;
	Vector *vectp = (Vector *) ((long)num << 2 );
	long old_ssp = Super( 1 );
	
	if( old_ssp == USER_MODE )
		old_ssp = Super( 0 );

	orig = *vectp;
	*vectp = vector;

	if( old_ssp != SUPER_MODE )
		Super( old_ssp );
		
	return orig;
}