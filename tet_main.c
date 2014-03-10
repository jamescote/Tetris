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

/* Helper Function Declarations */
bool didClockTick( long* prevTick, UINT8* tickCount );
ULONG32 get_time( );
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
	UINT16* fbBase16 = Physbase( ); /* TODO: Remove OSBinding */
	char cBuffer[ MAX_STR_LENGTH ];
	
	/* Set new Random Seed */
	srand( get_time( ) );
	
	while( m_cGameState != GAME_EXIT )
	{
		if( GAME_START == m_cGameState )
		{
			reset_Game( &m_MainGameModel, get_Random_Tetrimino( ), get_Random_Tetrimino( ) );
			render_All( fbBase16, &m_MainGameModel );						   
			m_cGameState = GAME_RUN;
		}
		else if( GAME_PAUSED == m_cGameState )
		{
			if( inputPending( ) && getInput( ) == PAUSE )
				m_cGameState = GAME_RUN;
		}
		else
		{
			if( didClockTick( &prevTick, &tickCount ) )
				handleSync( fbBase16, &tickCount, &m_MainGameModel );
			
			if( inputPending( ) )
				handleAsync( getInput( ), &m_MainGameModel );
			
			m_cGameState = parseState( &(m_MainGameModel) );
		}
	}
	
	/*return 0;*/
}

/* 
	Helper boolean function that checks if the clock did a tick since
	the last check.
*/
bool didClockTick( long* prevTick, UINT8* tickCount )
{
	ULONG32 timeNow = get_time( );
	bool bReturnVal = *prevTick != timeNow;
	
	if( bReturnVal )
	{
		*prevTick = timeNow;
		*tickCount++;
	}
		
	return bReturnVal;
}

/*
	Helper function that return the current clock ticks.
*/
ULONG32 get_time( )
{
	ULONG32 lReturnTime;
	long old_ssp = Super( 0 );

	lReturnTime = *cc_ClockPtr;
	
	Super( old_ssp );
	
	return lReturnTime;
}

UINT8 get_Random_Tetrimino( )
{
	return (rand( ) & (MAX_TETRIMINOS - 1));
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