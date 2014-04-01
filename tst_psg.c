/* Includes */
#include "input.h"
#include "psg.h"
#include "music.h"
#include "snd_cnst.h"
#include "sfx.h"
#include "songs.c"
#include "types.h"
#include <osbind.h>

#define VOLUME_UP		ROTATE_RIGHT
#define VOLUME_DOWN		MOVE_DOWN
#define TST_SFX_1		0x02
#define TST_SFX_2		0x03
#define TST_SFX_3		0x04
#define TST_SFX_4		0x05
#define TST_SFX_5		0x06
#define PAUSE			0x19 /*P*/
#define QUIT			0x10 /*Q*/

const long* cc_ClockPtr = (long*)0x462;

UINT32 get_time( );

/* main */
int main( )
{
	/* Order of writing PSG data:
		1. set Music Frequency
		2. set Noise Frequency
		3. set Mixer
		4. Level control
		5. Envelope Control
	*/
	bool bRunning 	= true;
	bool bPaused	= false;
	UINT32 iTime_Elapsed = 0;
	UINT32 prevTick = 0;
	UINT32 currTick = 0;
	
	/* PSG module test *
	set_tone( CHANNEL_A, 248 );
	enable_channel( CHANNEL_A, true, false );
	set_volume( CHANNEL_A, 10 );*/
	
	start_music( TetrisTheme_HighRes, iSongSizes[ TETRIS_HIGH_RES_SIZE_INDEX ], true );
	prevTick = get_time( );
	
	while( bRunning )
	{
		if( inputPending( ) )
		{
			switch( (char)(getInput( ) >> 16) )
			{
				case PAUSE:
					if( !bPaused )
					{
						pause_music( );
						bPaused = true;
					}
					else
					{
						play_music( );
						bPaused = false;
					}
					break;
				case QUIT:
					bRunning = false;
					break;
				case VOLUME_UP:
					adjust_music_volume( true );
					break;
				case VOLUME_DOWN:
					adjust_music_volume( false );
					break;
				case TST_SFX_1:
					play_Move_Tetrimino_FX( );
					break;
				case TST_SFX_2:
					play_Tetris_FX( );
					break;
				case TST_SFX_3:
					play_Line_Clear_FX( );
					break;
				case TST_SFX_4:
					play_Top_Out_FX( );
					break;
				case TST_SFX_5:
					play_Lock_Tetrimino_FX( );
					break;
				default:
					break;
			}
		}
		
		currTick = get_time( );
		iTime_Elapsed = currTick - prevTick;
		prevTick = currTick;
		
		update_music( iTime_Elapsed );
	}
		
	stop_sound( );
	
	return 0;
}

UINT32 get_time( )
{
	UINT32 lReturnTime;
	long old_ssp = Super( 0 );

	lReturnTime = *cc_ClockPtr;
	
	Super( old_ssp );
	
	return lReturnTime;
}