#ifndef _MODEL_H
#define _MODEL_H

#include "types.h"

/* Defines */
#define BOARD_HEIGHT 20
#define BOARD_WIDTH	10
#define X_POS 0
#define Y_POS 1
#define T_HEIGHT 4
#define D_RIGHT	0
#define D_LEFT 1

/* Tetrimino Piece Structure */
struct Tetrimino
{
	UINT8 bPos[ 2 ];
	UINT16 iMap[ T_HEIGHT ];
};

/* Game Board Structure */
/* Board Layout:
				20
				|
				|
		10______0
 */
struct Game_Board
{
	UINT16 BoardMap[ BOARD_HEIGHT ];
	UINT16 iScore, iLns_Clrd, iLvl;
	UINT8 tetrisCombo, chainCount;
	UINT16 iGrvty;
};

struct Model
{
	struct Tetrimino cCurrPiece;
	struct Game_Board cMainBoard;
};

/* Function Declarations */
void move_Left( struct Tetrimino* m_PieceToMove );
void move_Right( struct Tetrimino* m_PieceToMove );
void move_Down( struct Tetrimino* m_PieceToMove,
				 struct Game_Board* m_Board );
void Rotate( struct Tetrimino* m_PieceToRotate,
			  struct Game_Board* m_Board, UINT8 cDirection );
void Game_Over( struct Game_Board* m_Board, 
				struct Tetrimino* m_cLockingPiece );
UINT8 clear_Lines( struct Game_Board* m_Board );


#endif
