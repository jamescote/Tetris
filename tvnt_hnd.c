/* Includes */
#include "tvnt_hnd.h"
#include "gm_cnsts.h"
#include "model.h"
#include "renderer.h"

/* Private Function Declarations */

/*
	Handles Asynchronous events required by the tetris game.
*/
void handleAsync( const long lTrigger, Game_Model* m_MainGameModel )
{
	const char cTrigger = (char)(lTrigger >> WORD_LENGTH);
		
	switch( cTrigger )
	{
		case MOVE_LEFT:
			move_Left( &(m_MainGameModel->cCurrPiece),
					   &(m_MainGameModel->cMainBoard) );
			break;
		case MOVE_RIGHT:
			move_Right( &(m_MainGameModel->cCurrPiece),
						&(m_MainGameModel->cMainBoard) );
			break;
		case MOVE_DOWN:
			move_Down( &(m_MainGameModel->cCurrPiece), 
					   &(m_MainGameModel->cMainBoard) );
			break;
		case ROTATE_RIGHT:
			Rotate( &(m_MainGameModel->cCurrPiece),
					&(m_MainGameModel->cMainBoard), D_RIGHT );
			break;
		case ROTATE_LEFT:
			Rotate( &(m_MainGameModel->cCurrPiece),
					&(m_MainGameModel->cMainBoard), D_LEFT );
			break;
		case PAUSE:
			m_MainGameModel->cMainBoard.state = BOARD_PAUSE_STATE;
			break;
		default:
			break;
	};
}

/*
	Synchronized game logic is executed here.
*/
void handleSync( UINT16* fbBase16, Game_Model* m_MainGameModel )
{
	char cBuffer[ MAX_STR_LENGTH ] = {0x0};
	
	render_All( fbBase16, m_MainGameModel );
	
	if( gravityTriggered( &(m_MainGameModel->cMainBoard) ) )
		move_Down( &(m_MainGameModel->cCurrPiece),
				   &(m_MainGameModel->cMainBoard) );
	
	/*tetToString( cBuffer, &(m_MainGameModel->cCurrPiece) );
	render_String( (UINT8*)fbBase16, cBuffer, 496, 0 );
	gbToString( cBuffer, &(m_MainGameModel->cMainBoard) );
	render_String( (UINT8*)fbBase16, cBuffer, 0, 0 );*/
}