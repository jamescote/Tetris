#include "model.h"

/* External ASM references and functions not needed outside this module */
void rot_R( UINT16* oldMap, UINT16* newMap );
void rot_L( UINT16* oldMap, UINT16* newMap );
UINT16 squishMap( struct Tetrimino* m_PieceToSquish );
void lock_piece( struct Game_Board* m_Board,
				 struct Tetrimino* m_cLockingPiece );
void Calc_Score( struct Game_Board* m_Board );
void fixPosition( struct Tetrimino* m_PieceToFix,
				  struct Game_Board* m_Board );
UINT8 pieceCollides( struct Tetrimino* m_PieceToCheck,
					 struct Game_Board* m_Board );
					 
/* Constant Definitions */
#define MAX_TETRIMINOS 7
#define PTS_TETRIS 50
#define PTS_LINE 10
#define TTRS_CNST 4
#define STARTING_X_POS 6
#define STARTING_Y_POS 22
const UINT16 iStartingMaps[ ][ 4 ] =
{
	/* O-Tetrimino */
	{0x0000, 0x0030, 0x0030, 0x0000},
	
	/* S-Tetrimino */
	{0x0000, 0x0000, 0x0030, 0x0060},
	
	/* Z-Tetrimino */
	{0x0000, 0x0000, 0x0030, 0x0018},
	
	/* I-Tetrimino */
	{0x0040, 0x0040, 0x0040, 0x0040},
	
	/* J-Tetrimino */
	{0x0000, 0x0010, 0x0010, 0x0030},
	
	/* L-Tetrimino */
	{0x0000, 0x0020, 0x0020, 0x0030},
	
	/* T-Tetrimino */
	{0x0070, 0x0020, 0x0000, 0x0000}
};

/*
	Move Left by 1
*/
void move_Left( struct Tetrimino* m_PieceToMove )
 {
	UINT8 i;
	UINT16 iSquishedMap = squishMap( m_PieceToMove );
	
	if( iSquishedMap < 0x0200 )	
	{
		for( i = 0; i < T_HEIGHT; i++ )
			m_PieceToMove->iMap[ i ] << 1;
		
		m_PieceToMove->bPos[ X_POS ]++;
	}
		
 }
 
 /*
	Move Right by 1
*/
 void move_Right( struct Tetrimino* m_PieceToMove )
 {
	UINT8 i;
	UINT16 iSquishedMap = squishMap( m_PieceToMove );
	
	if( !(iSquishedMap & 1) )
	{
		for( i = 0; i < T_HEIGHT; i++ )
			m_PieceToMove->iMap[ i ] >> 1;
	
		m_PieceToMove->bPos[ X_POS ]--;
	}
 }
 
 /*
	Move Down by 1
 */
 void move_Down( struct Tetrimino* m_PieceToMove,
				 struct Game_Board* m_Board )
{
	UINT8 iNextPos = m_PieceToMove->bPos[ Y_POS ];
	UINT8 i = 3;
	
	while( m_PieceToMove->iMap[ i ] != (UINT16)0 )
		i--;
	
	iNextPos += (i - 1);
	
	if( iNextPos >= 0 && ( m_PieceToMove->iMap[ i ] & m_Board->BoardMap[ iNextPos ] ) > 0 )
		lock_piece( m_Board, m_PieceToMove );
	else
		m_PieceToMove->bPos[ Y_POS ]--;
}

/*
	Rotates a Tetrimino and positions the piece to avoid collisions after the rotate.
*/
void Rotate( struct Tetrimino* m_PieceToRotate,
			  const struct Game_Board* m_Board, UINT8 cDirection )
{
	UINT16 xMap[4] = {0x0,0x0,0x0,0x0};
	UINT8 i, j = m_PieceToRotate->bPos[ X_POS ];
	
	while( j != 3 )
	{
		if( j > 3 )
		{
			for( i = 0; i < T_HEIGHT; i++ )
				m_PieceToRotate->iMap[ i ] >>= 1;
			
			j--;
		}
		else
		{
			for( i = 0; i < T_HEIGHT; i++ )
				m_PieceToRotate->iMap[ i ] <<= 1;
			
			j++;
		}
	}
	
	switch( cDirection )
	{
	case D_RIGHT:
		rot_R( xMap, m_PieceToRotate->iMap );
		break;
	case D_LEFT:
		rot_L( xMap, m_PieceToRotate->iMap );
		break;
	};
	
	for( i = 0; i < T_HEIGHT; i++ )
		m_PieceToRotate->iMap[ i ] = xMap[ i ];
	
	fixPosition( m_PieceToRotate, m_Board );
}

/*
	move a Tetrimino around to avoid collisions after a rotation.
*/
void fixPosition( struct Tetrimino* m_PieceToFix,
				  const struct Game_Board* m_Board )
{
	UINT16 wSquishedMap = squishMap( m_PieceToFix );
	UINT8 i, j;
	
	if( m_PieceToFix->bPos[ X_POS ] > 3 )
	{
		for( j = 3; j < m_PieceToFix->bPos[ X_POS ]; j++ )
			for( i = 0; i < T_HEIGHT; i++ )
				m_PieceToFix->iMap[ i ] <<= 1;
	}
	else 
		m_PieceToFix->bPos[ X_POS ] = 3;
		
	while( wSquishedMap > 0x0200 )
	{
		move_Right( m_PieceToFix );
		wSquishedMap >>= 1;
	}
	
	while( pieceCollides( m_PieceToFix, m_Board ) )
		m_PieceToFix->bPos[ Y_POS ]++;
}

/*
	Function to check for Tetrimino Collisions.
*/
UINT8 pieceCollides( const struct Tetrimino* m_PieceToCheck,
					 const struct Game_Board* m_Board )
{
	UINT8 iOffset = m_PieceToCheck->bPos[ Y_POS ], i;
	UINT8 bReturnValue = 0;

	for( i = 0; i < T_HEIGHT; i++ )
	{
		if( (iOffset - i) < BOARD_HEIGHT )
			bReturnValue = bReturnValue || ( ( m_PieceToCheck->iMap[ i ] & m_Board->BoardMap[ iOffset - i ] ) > 0 );
	}
	
	return bReturnValue;
}

/* 
	Reset a new piece to the top of the board.
	Debug Note: Spawns a CheckerBoard pattern if iPiece is not a valid piece.
*/
void spawnPiece( struct Tetrimino* m_cCurrPiece, UINT8 iPiece )
{
	UINT8 i;
	
	m_cCurrPiece->bPos[ Y_POS ] = STARTING_Y_POS;
	m_cCurrPiece->bPos[ X_POS ] = STARTING_X_POS;
	
	if( (iPiece < 0) || (iPiece >= MAX_TETRIMINOS) )
	{
		m_cCurrPiece->iMap[0] = 0x0050;
		m_cCurrPiece->iMap[1] = 0x0028;
		m_cCurrPiece->iMap[2] = 0x0050;
		m_cCurrPiece->iMap[3] = 0x0028;
	}
	else
	{
		for( i = 0; i < T_HEIGHT; i++ )
			m_cCurrPiece->iMap[ i ] = iStartingMaps[ iPiece ][ i ];
	}
}

/*
	Locks a Piece into the board and handles any result from the lock.
*/
void lock_piece( struct Game_Board* m_Board,
				 struct Tetrimino* m_cLockingPiece )
{
	UINT8 iPiecePos = m_cLockingPiece->bPos[ Y_POS ];
	UINT8 i, iLinesCleared;

	if( iPiecePos >= 0 )
	{
		for( i = 0; i < T_HEIGHT; i++ )
			m_Board->BoardMap[ iPiecePos + i ] |= 
				m_cLockingPiece->iMap[ i ];
				
		Calc_Score( m_Board );
	}
	else
		Game_Over( m_Board, m_cLockingPiece );
}

/*
	Contains all the game logic for calculating score.
*/
void Calc_Score( struct Game_Board* m_Board )
{
	UINT8 iLinesCleared, i;
	UINT16 iPointsScored;

	iLinesCleared = clear_Lines( m_Board );
	
	if( iLinesCleared > 0 )
	{
		iPointsScored = ( PTS_LINE * iLinesCleared ) * m_Board->chainCount++;
		
		if( iLinesCleared >= T_HEIGHT )
		{
			iPointsScored += PTS_TETRIS * m_Board->tetrisCombo;
			if( m_Board->tetrisCombo < TTRS_CNST )
				m_Board->tetrisCombo++;
		}
		else
			m_Board->tetrisCombo = 0;
		
		m_Board->iLns_Clrd += iLinesCleared;
		
		if( ( m_Board->iLns_Clrd / 10 ) > m_Board->iLvl )
		{
			m_Board->iLvl++;
			m_Board->iGrvty++;
		}
	}
	else
		m_Board->chainCount = m_Board->iLvl;
}

/*
	Pushes all lines down while determining how many lines are cleared
	returns the number of lines cleared
*/
UINT8 clear_Lines( struct Game_Board* m_Board )
{
	UINT8 iReturnLines = 0, i;
	
	for( i = 0; i < BOARD_HEIGHT; i++ )
	{
		if( m_Board->BoardMap[ i ] == 0x0 )
			i = BOARD_HEIGHT;
		else
		{
			if( m_Board->BoardMap[ i ] == 0x03FF )
				iReturnLines++;
			
			if( ( iReturnLines > 0 ) && ( i != ( BOARD_HEIGHT - 1 ) ) )
				m_Board->BoardMap[ i ] = m_Board->BoardMap[ i + 1 ];
		}
	}

	return iReturnLines;
}
 
 /* 
	Contains any and all Game Over Logic.
 */
void Game_Over( struct Game_Board* m_Board, 
				struct Tetrimino* m_cLockingPiece )
{
	/* TODO: code accordingly where required, or remove. */
}

/*
	Clears The entire Game Board of Minos.
*/
void Clear_All( struct Game_Board* m_Board )
{
	UINT8 iIndex = 0;
	
	while( m_Board->BoardMap[ iIndex ] != 0x0 && iIndex < BOARD_HEIGHT )
	{
		m_Board->BoardMap[ iIndex++ ] = 0x0;
	}
}

 /*
	Purpose: Condense a map into 1 line to find the edges of
			 the map.
 */
 UINT16 squishMap( const struct Tetrimino* m_PieceToSquish )
 {
	UINT8 i;
	UINT16 iSquishedMap = m_PieceToSquish->iMap[ 0 ];
	
	for( i = 1; i < T_HEIGHT; i++ )
		iSquishedMap |= m_PieceToSquish->iMap[ i ];

	return iSquishedMap;
 }