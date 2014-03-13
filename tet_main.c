/* Includes */
#include "types.h"
#include "model.h"
#include "gm_cnsts.h"
#include "renderer.h"
#include "tvnt_hnd.h"
#include "input.h"
#include <stdlib.h>
#include <osbind.h>

/* Constants */
const long* cc_ClockPtr = (long*)0x462;
UINT8 gBackBuffer[ BACK_BUFFER_SIZE ];

/* Helper Function Declarations */
bool didClockTick( long* prevTick, UINT8* tickCount );
UINT32 get_time( );
UINT8 parseState( Game_Model* m_MainGameModel );
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
	UINT8 m_cGameState = GAME_START;
	long old_ssp;
	UINT16* fbBffrPtrs[ 2 ];
	fbBffrPtrs[ FRONT_BUFFER ] = Physbase( );
	fbBffrPtrs[ BACK_BUFFER ] = (((UINT16*)(gBackBuffer)+BACK_BUFFER_ALIGNMENT) & ~ 0xFF);
	UINT8 iCurrBackBffr = BACK_BUFFER;
	
	/* Set new Random Seed */
	srand( get_time( ) );
	
	reset_Game( &m_MainGameModel, get_Random_Tetrimino( ), get_Random_Tetrimino( ) );
	render_All( fbBffrPtrs[ FRONT_BUFFER ], &m_MainGameModel );						   
	m_cGameState = GAME_RUN;
	
	while( m_cGameState != GAME_EXIT )
	{
		if( GAME_PAUSED == m_cGameState )
		{
			if( inputPending( ) && getInput( ) == PAUSE )
				m_cGameState = GAME_RUN;
		}
		else
		{
			if( didClockTick( &prevTick, &(m_MainGameModel.cMainBoard.iTimeElapsed) ) )
			{
				handleSync( fbBffrPtrs[ iCurrBackBffr ], &m_MainGameModel );
				
				Setscreen( -1, fbBffrPtrs[ iCurrBackBffr ], -1 );
				iCurrBackBffr = !iCurrBackBffr;
			}
			
			if( inputPending( ) )
				handleAsync( getInput( ), &m_MainGameModel );
			
			m_cGameState = parseState( &(m_MainGameModel) );
		}
	}
	
	/* Set the frame buffer back to the front buffer. */
	Setscreen( -1, fbBffrPtrs[ 0 ], -1);
}

/* 
	Helper boolean function that checks if the clock did a tick since
	the last check.
*/
bool didClockTick( long* prevTick, UINT8* tickCount )
{
	UINT32 timeNow = get_time( );
	bool bClockTicked = *prevTick != timeNow;
	
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

UINT8 get_Random_Tetrimino( )
{
	return (rand( ) % MAX_TETRIMINOS);
}

/*
	Query the state of the Game Board and handle a state change appropriately
	for the Main Game Loop.
*/
UINT8 parseState( Game_Model* m_MainGameModel )
{
	UINT8 cReturnState = GAME_RUN;
	
	switch( m_MainGameModel->cMainBoard.state )
	{
		case BOARD_RESET_STATE:
			cReturnState = GAME_START;
			break;
		case BOARD_PAUSE_STATE:
			cReturnState = GAME_PAUSED;
			break;
		case BOARD_LOCK_STATE:
			spawnPiece( &(m_MainGameModel->cCurrPiece), 
						m_MainGameModel->cMainBoard.nxtPiece );
			m_MainGameModel->cMainBoard.nxtPiece = get_Random_Tetrimino( );
			m_MainGameModel->cMainBoard.state = BOARD_RUN_STATE;
			break;
		case BOARD_GAME_OVER_STATE:
			cReturnState = GAME_EXIT;
			break;
		default:
			break;
	};
	
	return cReturnState;
}