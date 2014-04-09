/*
	Name: tet_main.c
	Purpose: Initializes game and runs main game loop.
	Written by: James Cote
*/
/* Includes */
#include "types.h"
#include "model.h"
#include "gm_cnsts.h"
#include "renderer.h"
#include "raster.h"
#include "tvnt_hnd.h"
#include "input.h"
#include "psg.h"
#include "music.h"
#include "sfx.h"
#include "songs.c"
#include "mnu_cns.h"
#include <stdlib.h>
#include <osbind.h>
#include <stdio.h>

/* Constants */
const long* cc_ClockPtr = (long*)0x462;
UINT8 gBackBuffer[ BACK_BUFFER_SIZE ];

/* Defines */
#define VBL_VECTOR	28

/* Helper Function Declarations */
bool didClockTick( UINT32* prevTick, UINT32* tickCount );
void parseState( Game_Model* m_MainGameModel, UINT8* gameState );
UINT8 get_Random_Tetrimino( );
char run_menu( UINT16* fbBuffers[] );
void run_game( UINT16* fbBuffers[] );
Vector install_vector( int num, Vector vector );
void vblank( );
void kybd_isr( );
void do_vbl( );

/* Global Variables used for VBL ISRs */
bool bDoRender 		= false;
UINT32 lClockTick 	= 0;
UINT8 m_cGameState 	= GAME_START;
Game_Model m_MainGameModel =
{
	{ {0}, {0} },
	{ {0x0}, 0, 0, 0, 0, 0, 0, 0, 1 }
};

/* Main Program */
int main( )
{
	UINT16* fbBuffers[ 2 ];
	char m_CurrentState = RUN_MENU;
	Vector pOld_VBL_Vector, pOld_KYBD_Vector;
	UINT8 bHotKeys[ MAX_BUFFER_SIZE ];
	UINT8 bHotKeyCount;
	
	getHotKeys( bHotKeys, &bHotKeyCount );
	initializeHotKeys( bHotKeys, bHotKeyCount );
	
	/* Initialize some hardware values */
	stop_sound( );
	set_MIDI_Cntrl( false );
	
	/* Install new Vectors */
	pOld_VBL_Vector = install_vector( VBL_VECTOR, vblank );
	pOld_KYBD_Vector = install_vector( KYBD_VECTOR, kybd_isr );
	
	/* Set up buffer pointers */
	fbBuffers[ FRONT_BUFFER ] = get_video_base( );
	fbBuffers[ BACK_BUFFER ] = (UINT16*)(((UINT32)(gBackBuffer)+BACK_BUFFER_ALIGNMENT) & ~ (UINT32)0xFF);
	
	while( QUIT_GAME != m_CurrentState )
	{
		if( RUN_MENU == m_CurrentState )
			m_CurrentState = run_menu( fbBuffers );
		else
		{
			run_game( fbBuffers );
			m_CurrentState = RUN_MENU;
		}
	}
	
	/* Uninstall new Vectors */
	install_vector( VBL_VECTOR, pOld_VBL_Vector );
	install_vector( KYBD_VECTOR, pOld_KYBD_Vector );
	
	set_MIDI_Cntrl( true );
		
	return 0;
}

char run_menu( UINT16* fbBuffers[] )
{
	char bReturnState 	= RUN_MENU;
	UINT8 iCurrBackBffr = BACK_BUFFER;
	char bCurrentSelection = ONE_PLAYER;
	
	/* Set up initial Renders */
	clear_Screen( fbBuffers[ FRONT_BUFFER ] );
	clear_Screen( fbBuffers[ BACK_BUFFER ] );
	draw_Menu( fbBuffers[ FRONT_BUFFER ], bCurrentSelection );
	draw_Menu( fbBuffers[ BACK_BUFFER ], bCurrentSelection );
	
	resetBuffer( );
	while( RUN_MENU == bReturnState )
	{
		if( bDoRender )
		{
			/* Re-render and flip buffers */
			draw_Menu( fbBuffers[ iCurrBackBffr ], 
					   bCurrentSelection );
			
			/* flip buffers */
			set_video_base( fbBuffers[ iCurrBackBffr ] );
			iCurrBackBffr = 1 - iCurrBackBffr;
			bDoRender = false;
		}
		
		if( inputPending( ) )
		{
			bReturnState = handleMenuAsync( getInput( ), &bCurrentSelection );
		}
	}
	
	/* Set the frame buffer back to the front buffer. */
	set_video_base( fbBuffers[ FRONT_BUFFER ] );

	bDoRender = false;
	while( !bDoRender )
		;
	
	return bReturnState;
}

void run_game( UINT16* fbBuffers[] )
{
	/* Local Variables */
	UINT8 iCurrBackBffr = BACK_BUFFER;
	
	m_cGameState = GAME_START;
	
	/* Set new Random Seed */
	srand( lClockTick );
	
	/* Set up initial renders */
	reset_Game( &m_MainGameModel, get_Random_Tetrimino( ), get_Random_Tetrimino( ) );
	render_Static( fbBuffers[ FRONT_BUFFER ] );
	render_Static( fbBuffers[ BACK_BUFFER ] );
	render_All( fbBuffers[ FRONT_BUFFER ], &m_MainGameModel, 0 );
	render_All( fbBuffers[ BACK_BUFFER ], &m_MainGameModel, 0 );
	m_cGameState = GAME_RUN;
	
	/* Start game music */
	start_music( TetrisTheme_HighRes, iSongSizes[ TETRIS_HIGH_RES_SIZE_INDEX ], true );
	
	resetBuffer( );
	while( m_cGameState != GAME_EXIT )
	{
		if( m_cGameState != GAME_PAUSED &&
			bDoRender )
		{
			handleRenderSync( fbBuffers[ iCurrBackBffr ], &m_MainGameModel );
			
			set_video_base( fbBuffers[ iCurrBackBffr ] );
			iCurrBackBffr = 1 - iCurrBackBffr;
			bDoRender = false;
		}
		
		if( inputPending( ) )
			handleAsync( getInput( ), &m_MainGameModel, m_cGameState == GAME_PAUSED );
		
		parseState( &m_MainGameModel, &m_cGameState );
	}
	
	/* Set the frame buffer back to the front buffer. */		
	set_video_base( fbBuffers[ FRONT_BUFFER ] );
	bDoRender = false;

	while( !bDoRender )
		;
	
	stop_sound( );
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

/* Subroutine to run VBL logic and functionality. */
void do_vbl( )

{
	/* Update Clock */
	lClockTick++;
	
	/* Trigger Render */
	bDoRender = true;

	/* Update Synchronous Events */
	if( GAME_PAUSED != m_cGameState )
	{
		handleSync( &m_MainGameModel, 1 );
		update_input_buffer( 1 );
	}
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
			resetBuffer( );
			break;
		case BOARD_GAME_OVER_STATE:
			*gameState = GAME_EXIT;
			break;
		default:
			break;
	};
}