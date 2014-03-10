#ifndef _MODEL_H
#define _MODEL_H

#include "types.h"
#include "gm_cnsts.h"

/* Tetrimino Piece Structure */
typedef struct
{
	UINT8 bPos[ 2 ];
	UINT16 iMap[ T_HEIGHT ];
} Tetrimino;

/* Game Board Structure */
/* Board Layout:
				20
				|
				|
		10______0
 */
typedef struct
{
	UINT16 BoardMap[ BOARD_HEIGHT ];
	UINT16 iScore, iLns_Clrd, iLvl;
	UINT8 tetrisCombo, chainCount, nxtPiece, state;
	UINT8 iGrvty;
} Game_Board;

typedef struct
{
	Tetrimino cCurrPiece;
	Game_Board cMainBoard;
} Game_Model;

/* Function Declarations */
void reset_Game( Game_Model* m_MainModel, UINT8 cFirstPiece, UINT8 cNxtPiece );
void move_Left( Tetrimino* m_PieceToMove );
void move_Right( Tetrimino* m_PieceToMove );
void move_Down( Tetrimino* m_PieceToMove,
				Game_Board* m_Board );
void Rotate( Tetrimino* m_PieceToRotate,
			 Game_Board* m_Board, UINT8 cDirection );
void Game_Over(Game_Board* m_Board, 
				Tetrimino* m_cLockingPiece );
UINT8 clear_Lines(Game_Board* m_Board );
void spawnPiece( Tetrimino* m_cCurrPiece, UINT8 iPiece );
void copyTetrimino( Tetrimino* m_Target, const Tetrimino* m_Source );
void copyGameBoard( Game_Board* m_Target, const Game_Board* m_Source );

void tetToString( char* cBuffer, const Tetrimino* m_Tetrimino );
void gbToString( char* cBuffer, const Game_Board* m_Board );


#endif
