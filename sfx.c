/*
	Implements functionality for the sound effects used
	in the game.
	Written by: James Cote
*/
/* Includes */
#include "sfx.h"
#include "psg.h"
#include "snd_cnst.h"

/* Defines */
#define MOVE_TETRIMINO_ENV_FREQ 0x0100
#define LOCK_TETRIMINO_ENV_FREQ 0x0200
#define TOP_OUT_ENV_FREQ 0x3a00
#define LINE_CLEAR_ENV_FREQ 0x1800
#define TETRIS_ENV_FREQ 0x2000

/* plays a soft little noise as the tetrimino moves */
void play_Move_Tetrimino_FX()
{
	set_noise( A );
	enable_channel( SFX_CHANNEL, false, true );
	enable_channel( EXTRA_CHANNEL, false, false );
	set_volume( SFX_CHANNEL, ENVELOPE_VOLUME );
	set_envelope( MOVE_TETRIMINO_ENV_FREQ, TETRI_MOVE_ENVELOPE );
}

/* plays a locking sound when a Tetrimino Locks into place */
void play_Lock_Tetrimino_FX( )
{
	set_noise( B );
	enable_channel( EXTRA_CHANNEL, false, true );
	set_volume( EXTRA_CHANNEL, ENVELOPE_VOLUME );
	set_envelope( LOCK_TETRIMINO_ENV_FREQ, TETRI_LOCK_ENVELOPE );
}

/* Plays a low chord that fades out when the player tops out */
void play_Top_Out_FX( )
{
	set_tone( SFX_CHANNEL, E << 1 );
	set_tone( EXTRA_CHANNEL, A << 1);
	enable_channel( SFX_CHANNEL, true, false );
	enable_channel( EXTRA_CHANNEL, true, false );
	set_volume( SFX_CHANNEL, ENVELOPE_VOLUME );
	set_volume( EXTRA_CHANNEL, ENVELOPE_VOLUME );
	set_envelope( TOP_OUT_ENV_FREQ, TOP_OUT_ENVELOPE );
}

/* plays a slightly high chord when a player clears a line */
void play_Line_Clear_FX( )
{
	set_tone( SFX_CHANNEL, G >> 1 );
	set_tone( EXTRA_CHANNEL, F );
	enable_channel( SFX_CHANNEL, true, false );
	enable_channel( EXTRA_CHANNEL, true, false );
	set_volume( SFX_CHANNEL, ENVELOPE_VOLUME );
	set_volume( EXTRA_CHANNEL, ENVELOPE_VOLUME );
	set_envelope( LINE_CLEAR_ENV_FREQ, CLEAR_LINE_ENVELOPE );
}

/* plays a fuller chord than an line clear for slightly longer if the player gets a tetris */
void play_Tetris_FX( )
{
	set_tone( SFX_CHANNEL, C >> 1 );
	set_tone( EXTRA_CHANNEL, E );
	enable_channel( SFX_CHANNEL, true, false );
	enable_channel( EXTRA_CHANNEL, true, false );
	set_volume( SFX_CHANNEL, ENVELOPE_VOLUME );
	set_volume( EXTRA_CHANNEL, ENVELOPE_VOLUME );
	set_envelope( TETRIS_ENV_FREQ, CLEAR_LINE_ENVELOPE );
}

