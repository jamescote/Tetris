/* Includes */
#include "types.h"
#include "model.h"
#include "gm_cnsts.h"
#include "renderer.h"
#include "tvnt_hnd.h"
#include "input.h"
#include "psg.h"
#include "music.h"
#include "sfx.h"
#include "songs.c"
#include <stdlib.h>
#include <osbind.h>
#include <stdio.h>

/* Constants */
const long* cc_ClockPtr = (long*)0x462;
UINT8 gBackBuffer[ BACK_BUFFER_SIZE ];

/* Helper Function Declarations */
bool didClockTick( long* prevTick, UINT32* tickCount );
UINT32 get_time( );
void parseState( Game_Model* m_MainGameModel, UINT8* gameState );
UINT8 get_Random_Tetrimino( );

/* Main Program */
int main( )
{
	/* Local Variables */
	Game_Model m_MainGameModel =
	{
		{ {0}, {0} },
		{ {0x0}, 0, 0, 0, 0, 0, 0, 0, 1 }
	};
	UINT8 tickCount = 0;
	long prevTick;
	UINT32 lTimeElapsed = 0;
	long setScreenTrigger;
	UINT8 m_cGameState = GAME_START;
	long old_ssp;
	UINT16* fbBffrPtrs[ 2 ];
	UINT8 iCurrBackBffr = BACK_BUFFER;
	
	/* Set up buffer pointers */
	fbBffrPtrs[ FRONT_BUFFER ] = Physbase( );
	fbBffrPtrs[ BACK_BUFFER ] = (UINT16*)(((UINT32)(gBackBuffer)+BACK_BUFFER_ALIGNMENT) & ~ (UINT32)0xFF);
	
	/* Set new Random Seed */
	srand( get_time( ) );
	
	/* Set up initial renders */
	reset_Game( &m_MainGameModel, get_Random_Tetrimino( ), get_Random_Tetrimino( ) );
	render_Static( fbBffrPtrs[ FRONT_BUFFER ] );
	render_Static( fbBffrPtrs[ BACK_BUFFER ] );
	render_All( fbBffrPtrs[ FRONT_BUFFER ], &m_MainGameModel, 0 );
	render_All( fbBffrPtrs[ BACK_BUFFER ], &m_MainGameModel, 0 );
	m_cGameState = GAME_RUN;
	
	prevTick = get_time( );
	
	/* Start game music */
	start_music( TetrisTheme_HighRes, iSongSizes[ TETRIS_HIGH_RES_SIZE_INDEX ], true );
	
	while( m_cGameState != GAME_EXIT )
	{
		if( m_cGameState != GAME_PAUSED && 
			didClockTick( &prevTick, &lTimeElapsed ) )
		{
			handleSync( fbBffrPtrs[ iCurrBackBffr ], &m_MainGameModel, &lTimeElapsed );
			
			/* flip buffers */
			Setscreen( -1, fbBffrPtrs[ iCurrBackBffr ], -1 );
			iCurrBackBffr = 1 - iCurrBackBffr;
			setScreenTrigger = get_time( );
			lTimeElapsed += (setScreenTrigger - prevTick);
			prevTick = setScreenTrigger;
		}
		
		if( inputPending( ) )
			handleAsync( getInput( ), &m_MainGameModel, m_cGameState == GAME_PAUSED );
			
		if( m_cGameState == GAME_PAUSED )
			prevTick = get_time( );
		
		parseState( &m_MainGameModel, &m_cGameState );
	}
	
	stop_sound( );
	
	/* Set the frame buffer back to the front buffer. */		
	Setscreen( -1, fbBffrPtrs[ FRONT_BUFFER ], -1);

	prevTick = get_time( );
	while (prevTick == get_time())
		;
		
	return 0;
}

/* 
	Helper boolean function that checks if the clock did a tick since
	the last check.
*/
bool didClockTick( long* prevTick, UINT32* tickCount )
{
	UINT32 timeNow = get_time( );
	bool bClockTicked = (*prevTick != timeNow);
	
	if( bClockTicked )
	{
		*tickCount += timeNow - *prevTick;
		*prevTick = timeNow;
	}
		
	return bClockTicked;
}

/*
	Helper function that return the current clock ticks.
*/
UINT32 get_time( )
{
	UINT32 lReturnTime;
	long old_ssp = Super( 0 );

	lReturnTime = *cc_ClockPtr;
	
	Super( old_ssp );
	
	return lReturnTime;
}

/*
	Returns a random index for a Tetrimino
*/
UINT8 get_Random_Tetrimino( )
{
	return (rand( ) % MAX_TETRIMINOS);
}

/*
	Query the state of the Game Board and handle a state change appropriately
	for the Main Game Loop.
*/
void parseState( Game_Model* m_MainGameModel, UINT8* gameState )
{	
	switch( m_MainGameModel->cMainBoard.state )
	{
		case BOARD_RESET_STATE:
			*gameState = GAME_START;
			break;
		case BOARD_PAUSE_STATE:
			if( *gameState == GAME_PAUSED )
			{
				*gameState = GAME_RUN;
				play_music( );
			}
			else
			{
				*gameState = GAME_PAUSED;
				pause_music( );
			}
			m_MainGameModel->cMainBoard.state = BOARD_RUN_STATE;
			break;
		case BOARD_LOCK_STATE:
			spawnPiece( &(m_MainGameModel->cCurrPiece), 
						m_MainGameModel->cMainBoard.nxtPiece );
			m_MainGameModel->cMainBoard.nxtPiece = get_Random_Tetrimino( );
			m_MainGameModel->cMainBoard.state = BOARD_RUN_STATE;
			break;
		case BOARD_GAME_OVER_STATE:
			*gameState = GAME_EXIT;
			break;
		default:
			break;
	};
}