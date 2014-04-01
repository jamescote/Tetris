/* Includes */
#include "tvnt_hnd.h"
#include "gm_cnsts.h"
#include "model.h"
#include "renderer.h"
#include "music.h"

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

/*
	Handles Asynchronous events required by the tetris game.
*/
void handleAsync( const long lTrigger, Game_Model* m_MainGameModel, bool bPaused )
{
	const char cTrigger = (char)(lTrigger >> WORD_LENGTH);
		
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
void handleSync( UINT16* fbBase16, Game_Model* m_MainGameModel, UINT32* lTimeElapsed )
{
	FPS_Status.iTimer += *lTimeElapsed;
	m_MainGameModel->cMainBoard.iTimeElapsed += *lTimeElapsed;
	render_All( fbBase16, m_MainGameModel, FPS_Status.iCurrentFPS );
	FPS_Status.iRenderCount++;
	if( FPS_Status.iTimer >= TICKS_PER_SECOND )
	{
		FPS_Status.iTimer -= TICKS_PER_SECOND;
		if( FPS_Status.bEnableFPS )
			flag_FPS_Rend( );
		FPS_Status.iCurrentFPS = FPS_Status.iRenderCount;
		FPS_Status.iRenderCount = 0;
	}
	update_music( *lTimeElapsed );
	if( gravityTriggered( &(m_MainGameModel->cMainBoard) ) )
		move_Down( &(m_MainGameModel->cCurrPiece),
				   &(m_MainGameModel->cMainBoard) );
				   
	*lTimeElapsed = 0;
}