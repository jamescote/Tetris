/* Includes */
#include "tvnt_hnd.h"
#include "gm_cnsts.h"
#include "model.h"
#include "renderer.h"
#include "music.h"
#include "mnu_cns.h"

/* Global Variables */
struct FPS_Status
{
	int iRenderCount;
	int iTimer;
	int iCurrentFPS;
	bool bEnableFPS;
}FPS_Status =
{
	0, 0, 0, false
};

const UINT8 bNumHotKeys = GAME_KEY_COUNT + MENU_KEY_COUNT;
const UINT8 bEventHotKeys[ GAME_KEY_COUNT + MENU_KEY_COUNT ] =
{
	MOVE_LEFT,
    MOVE_RIGHT, 		
	MOVE_DOWN,		
	ROTATE_RIGHT,	
	ROTATE_RIGHT_X,
	ROTATE_LEFT,		
	PAUSE_P,			
	PAUSE_ESC,		
	PAUSE_F1,		
	QUIT,			
	FPS,	
	ONE_PLAYER_KEY,	
    TWO_PLAYER_KEY, 	
    UP_ARROW,		
	ENTER			
};

void getHotKeys( UINT8 bKeys[], UINT8 *bCount )
{
	UINT8 i;
	
	for( i = 0; i < bNumHotKeys; i++ )
		bKeys[ i ] = bEventHotKeys[ i ];
	bKeys = bEventHotKeys;
	*bCount = bNumHotKeys;
}

/*
	Handles Asynchronous events required by the Game Menu
*/
UINT8 handleMenuAsync( const UINT8 cTrigger, char* bCurrentSelection )
{
	UINT8 bReturnState = RUN_MENU;
	
	switch( cTrigger )
	{
		case MOVE_LEFT:
		case UP_ARROW:
			/* Handle Moving to previous Selection */
			*bCurrentSelection -= 1;
			if( *bCurrentSelection < QUIT_GAME )
				*bCurrentSelection = RUN_MENU - 1;
			break;
		case MOVE_RIGHT:
		case MOVE_DOWN:
			/* Handle Moving to next selection */
			(*bCurrentSelection)++;
			if( *bCurrentSelection >= RUN_MENU )
				*bCurrentSelection = QUIT_GAME;
			break;
		case ENTER:
			/* Handle Selecting Selection */
			bReturnState = *bCurrentSelection;
			break;
		case ONE_PLAYER_KEY:
			bReturnState = ONE_PLAYER;
			break;
		case TWO_PLAYER_KEY:
			bReturnState = TWO_PLAYER;
			break;
		case QUIT:
			bReturnState = QUIT_GAME;
			break;
		default:
			break;
	};
	
	return bReturnState;
}

/*
	Handles Asynchronous events required by the tetris game.
*/
void handleAsync( const UINT8 cTrigger, Game_Model* m_MainGameModel, bool bPaused )
{		
	switch( cTrigger )
	{
		case MOVE_LEFT:
			if( !bPaused )
				move_Left( &(m_MainGameModel->cCurrPiece),
						   &(m_MainGameModel->cMainBoard) );
			break;
		case MOVE_RIGHT:
			if( !bPaused )
				move_Right( &(m_MainGameModel->cCurrPiece),
							&(m_MainGameModel->cMainBoard) );
			break;
		case MOVE_DOWN:
			if( !bPaused )
				move_Down( &(m_MainGameModel->cCurrPiece), 
						   &(m_MainGameModel->cMainBoard) );
			break;
		case ROTATE_RIGHT_X:
		case ROTATE_RIGHT:
			if( !bPaused )
				Rotate( &(m_MainGameModel->cCurrPiece),
						&(m_MainGameModel->cMainBoard), D_RIGHT );
			break;
		case ROTATE_LEFT:
			if( !bPaused )
				Rotate( &(m_MainGameModel->cCurrPiece),
						&(m_MainGameModel->cMainBoard), D_LEFT );
			break;
		case PAUSE_P:
		case PAUSE_ESC:
		case PAUSE_F1:
			m_MainGameModel->cMainBoard.state = BOARD_PAUSE_STATE;
			break;
		case QUIT:
			m_MainGameModel->cMainBoard.state = BOARD_GAME_OVER_STATE;
			break;
		case FPS:
			FPS_Status.bEnableFPS = true - FPS_Status.bEnableFPS;
			if( FPS_Status.bEnableFPS )
				flag_FPS_Rend( );
			else
				flag_FPS_Clear( );
			break;
		default:
			break;
	};
}

/*
	Synchronized game logic is executed here.
*/
void handleSync( Game_Model* m_MainGameModel, UINT32 lTimeElapsed )
{
	FPS_Status.iTimer += lTimeElapsed;
	m_MainGameModel->cMainBoard.iTimeElapsed += lTimeElapsed;
	FPS_Status.iRenderCount++;
	if( FPS_Status.iTimer >= TICKS_PER_SECOND )
	{
		FPS_Status.iTimer -= TICKS_PER_SECOND;
		if( FPS_Status.bEnableFPS )
			flag_FPS_Rend( );
		FPS_Status.iCurrentFPS = FPS_Status.iRenderCount;
		FPS_Status.iRenderCount = 0;
	}
	update_music( lTimeElapsed );
	if( gravityTriggered( &(m_MainGameModel->cMainBoard) ) )
		move_Down( &(m_MainGameModel->cCurrPiece),
				   &(m_MainGameModel->cMainBoard) );
}

void handleRenderSync( UINT16* fbBase16, Game_Model* m_MainGameModel )
{
	render_All( fbBase16, m_MainGameModel, FPS_Status.iCurrentFPS );
}