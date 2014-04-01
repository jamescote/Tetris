#include "model.h"
#include "tetri.c"
#include "music.h"
#include "sfx.h"

/* External ASM references and functions not needed outside this module */
void rot_R( UINT16* oldMap, UINT16* newMap );
void rot_L( UINT16* oldMap, UINT16* newMap );
UINT16 squishMap( Tetrimino* m_PieceToSquish );
void lock_piece( Game_Board* m_Board,
				 Tetrimino* m_cLockingPiece );
void Calc_Score( Game_Board* m_Board );
void fixPosition( Tetrimino* m_PieceToFix,
				  Game_Board* m_Board );
bool pieceCollided( Tetrimino* m_PieceToCheck,
					 Game_Board* m_Board );
bool isTetriminoOutOfBounds( const Tetrimino* m_PieceToCheck );
					 
 /*
	Sets the game Model into a fresh state for starting a new game.
 */
void reset_Game( Game_Model* m_MainModel, UINT8 cFirstPiece, UINT8 cNxtPiece )
{
	/* Local Variables */
	UINT8 i;

	/* Reset Tetrimino */
	m_MainModel->cCurrPiece.bPos[ X_POS ] = TETRI_START_POS_X;
	m_MainModel->cCurrPiece.bPos[ Y_POS ] = TETRI_START_POS_Y;
	for( i = 0; i < T_HEIGHT; i++ )
		m_MainModel->cCurrPiece.iMap[ i ] = iStartingMaps[ cFirstPiece ][ i ];
	
	/* Reset Game Board */
	for( i = 0; i < BOARD_HEIGHT; i++ )
		m_MainModel->cMainBoard.BoardMap[ i ] = 0x0;
	m_MainModel->cMainBoard.iScore 			= 0;
	m_MainModel->cMainBoard.iLns_Clrd 		= 0;
	m_MainModel->cMainBoard.iLvl			= 0;
	m_MainModel->cMainBoard.tetrisCombo		= 1;
	m_MainModel->cMainBoard.chainCount		= 1;
	m_MainModel->cMainBoard.nxtPiece		= cNxtPiece;
	m_MainModel->cMainBoard.state			= BOARD_RUN_STATE;
	m_MainModel->cMainBoard.iGrvty			= DEFAULT_GRAVITY;
	m_MainModel->cMainBoard.iTimeElapsed	= 0;
}

/*
	Copiers for storing the object in the renderer state.
*/
void copyTetrimino( Tetrimino* m_Target, const Tetrimino* m_Source )
{
	UINT8 i;
	
	for( i = 0; i < MAX_POS; i++ )
		m_Target->bPos[ i ] = m_Source->bPos[ i ];
		
	for( i = 0; i < T_HEIGHT; i++ )
		m_Target->iMap[ i ] = m_Source->iMap[ i ];
}

/*
	Determines if the synchronous gravity trigger has occurred
	Resets the time elapsed variable if it has.
*/
bool gravityTriggered( Game_Board* m_Board )
{
	bool bTriggered = (m_Board->iTimeElapsed >= m_Board->iGrvty);
	
	if( bTriggered )
		m_Board->iTimeElapsed -= m_Board->iGrvty;
		
	return bTriggered;
}
					 
/*
	Move Left by 1
*/
void move_Left( Tetrimino* m_PieceToMove,
				const Game_Board* m_BoardRef )
 {
	UINT8 i;
	UINT16 iSquishedMap = squishMap( m_PieceToMove );
	
	if( iSquishedMap < LEFT_BOUND )	
	{
		for( i = 0; i < T_HEIGHT; i++ )
			m_PieceToMove->iMap[ i ] <<= 1;
			
		if( pieceCollided( m_PieceToMove, m_BoardRef ) )
			for( i = 0; i < T_HEIGHT; i++ )
				m_PieceToMove->iMap[ i ] >>= 1;
		else
		{
			m_PieceToMove->bPos[ X_POS ]++;
			play_Move_Tetrimino_FX( );
		}
	}
		
 }
 
 /*
	Move Right by 1
*/
 void move_Right( Tetrimino* m_PieceToMove, const Game_Board* m_BoardRef )
 {
	UINT8 i;
	UINT16 iSquishedMap = squishMap( m_PieceToMove );
	
	if( !(iSquishedMap & 1) )
	{
		for( i = 0; i < T_HEIGHT; i++ )
			m_PieceToMove->iMap[ i ] >>= 1;
	
		if( pieceCollided( m_PieceToMove, m_BoardRef ) )
			for( i = 0; i < T_HEIGHT; i++ )
				m_PieceToMove->iMap[ i ] <<= 1;
		else
		{
			m_PieceToMove->bPos[ X_POS ]--;
			play_Move_Tetrimino_FX( );
		}
	}
 }
 
 /*
	Move Down by 1
 */
 void move_Down( Tetrimino* m_PieceToMove,
				 Game_Board* m_Board )
{	
	m_PieceToMove->bPos[ Y_POS ]--;
	
	if( pieceCollided( m_PieceToMove, m_Board ) )
	{
		m_PieceToMove->bPos[ Y_POS ]++;
		lock_piece( m_Board, m_PieceToMove );
	}
	else
		play_Move_Tetrimino_FX( );
}

/*
	Rotates a Tetrimino and positions the piece to avoid collisions after the rotate.
*/
void Rotate( Tetrimino* m_PieceToRotate,
			 Game_Board* m_Board, UINT8 cDirection )
{
	UINT16 xMap[T_HEIGHT] = {0x0,0x0,0x0,0x0};
	UINT8 i, j = m_PieceToRotate->bPos[ X_POS ];
	
	while( j != RIGHT_ALIGNED_X_POS )
	{
		if( j > RIGHT_ALIGNED_X_POS )
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
void fixPosition( Tetrimino* m_PieceToFix,
				  const Game_Board* m_Board )
{
	UINT16 wSquishedMap;
	UINT8 i, j;
	
	if( m_PieceToFix->bPos[ X_POS ] > RIGHT_ALIGNED_X_POS )
	{
		for( j = RIGHT_ALIGNED_X_POS; j < m_PieceToFix->bPos[ X_POS ]; j++ )
			for( i = 0; i < T_HEIGHT; i++ )
				m_PieceToFix->iMap[ i ] <<= 1;
	}
	else 
		m_PieceToFix->bPos[ X_POS ] = RIGHT_ALIGNED_X_POS;
		
	wSquishedMap = squishMap( m_PieceToFix );
	
	while( wSquishedMap > LEFT_BOUND )
	{
		move_Right( m_PieceToFix, m_Board );
		wSquishedMap >>= 1;
	}
	
	while( pieceCollided( m_PieceToFix, m_Board ) )
		m_PieceToFix->bPos[ Y_POS ]++;
}

/*
	Function to check if Tetrimino has collided with another piece or 
	the bottom of the board.
*/
bool pieceCollided( const Tetrimino* m_PieceToCheck,
					 const Game_Board* m_Board )
{
	char iOffset = m_PieceToCheck->bPos[ Y_POS ], i = 0;
	bool bReturnValue = false;
	
	while( !bReturnValue && (i < T_HEIGHT) )
	{
		if( iOffset < BOARD_HEIGHT )
		{
			if( iOffset >= 0 )
				bReturnValue = (m_PieceToCheck->iMap[ i ] & m_Board->BoardMap[ iOffset ]) != 0;
			else
				bReturnValue = (m_PieceToCheck->iMap[ i ] != 0); 
		}
		
		iOffset -= 1;
		i++;
	}
	
	return bReturnValue;
}

/* 
	Reset a new piece to the top of the board.
	Debug Note: Spawns a CheckerBoard pattern if iPiece is not a valid piece.
*/
void spawnPiece( Tetrimino* m_cCurrPiece, UINT8 iPiece )
{
	UINT8 i;
	
	m_cCurrPiece->bPos[ Y_POS ] = TETRI_START_POS_Y;
	m_cCurrPiece->bPos[ X_POS ] = TETRI_START_POS_X;
	
	for( i = 0; i < T_HEIGHT; i++ )
		m_cCurrPiece->iMap[ i ] = iStartingMaps[ iPiece ][ i ];
}

/*
	Locks a Piece into the board and handles any result from the lock.
*/
void lock_piece( Game_Board* m_Board,
				 Tetrimino* m_cLockingPiece )
{
	int iPiecePos = m_cLockingPiece->bPos[ Y_POS ];
	UINT8 i;

	if( !isTetriminoOutOfBounds( m_cLockingPiece ) )
	{
		for( i = 0; i < T_HEIGHT; i++ )
			m_Board->BoardMap[ iPiecePos - i ] |= 
				m_cLockingPiece->iMap[ i ];
				
		play_Lock_Tetrimino_FX( );
				
		Calc_Score( m_Board );
		
		m_Board->state = BOARD_LOCK_STATE;
	}
	else
	{
		m_Board->state = BOARD_GAME_OVER_STATE;
		play_Top_Out_FX( );
	}
}

/* 
	Verify that the tetrimino is out of bounds and not just the position 
	of the tetrimino.
*/
bool isTetriminoOutOfBounds( const Tetrimino* m_PieceToCheck )
{
	bool bOutOfBounds = ( m_PieceToCheck->bPos[ Y_POS ] >= (BOARD_HEIGHT + T_HEIGHT) );
	UINT8 i = 0;
						
	if( !bOutOfBounds && m_PieceToCheck->bPos[ Y_POS ] >= BOARD_HEIGHT )
		while( m_PieceToCheck->bPos[ Y_POS ] - i >= BOARD_HEIGHT )
			bOutOfBounds |= m_PieceToCheck->iMap[ i++ ];
			
	return bOutOfBounds;
}

/*
	Contains all the game logic for calculating score.
*/
void Calc_Score( Game_Board* m_Board )
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
				
			play_Tetris_FX( );
		}
		else
		{
			m_Board->tetrisCombo = 1;
			play_Line_Clear_FX( );
		}
		
		m_Board->iLns_Clrd += iLinesCleared;
		
		if( m_Board->iLns_Clrd >= LEVEL_THRESHOLD )
		{
			m_Board->iLvl++;
			m_Board->iLns_Clrd -= LEVEL_THRESHOLD;
			m_Board->iGrvty -= GRAVITY_DELTA;
			m_Board->iTimeElapsed = 0;
			adjust_tempo( m_Board->iLvl );
		}
		
		m_Board->iScore += iPointsScored;
	}
	else
		m_Board->chainCount = m_Board->iLvl + 1;
}

/*
	Pushes all lines down while determining how many lines are cleared
	returns the number of lines cleared
*/
UINT8 clear_Lines( Game_Board* m_Board )
{
	UINT8 iReturnLines = 0, i, j;
	
	for( i = 0; i < BOARD_HEIGHT; i++ )
	{
		if( m_Board->BoardMap[ i ] == 0 )
			i = BOARD_HEIGHT;
		else if( m_Board->BoardMap[ i ] == FULL_LINE )
			iReturnLines++;
	}
	
	i = 0;
	j = 0;
	while( i < BOARD_HEIGHT )
	{
		if( j >= BOARD_HEIGHT )
			m_Board->BoardMap[ i++ ] = 0x0;
		else if( m_Board->BoardMap[ j ] == FULL_LINE )
			j++;
		else
			m_Board->BoardMap[ i++ ] = m_Board->BoardMap[ j++ ];
	}
	
	return iReturnLines;
}
 
 /* 
	Contains any and all Game Over Logic.
 */
void Game_Over( Game_Board* m_Board, 
				Tetrimino* m_cLockingPiece )
{
	/* TODO: code accordingly where required, or remove. */
}

/*
	Clears The entire Game Board of Minos.
*/
void Clear_All( Game_Board* m_Board )
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
 UINT16 squishMap( const Tetrimino* m_PieceToSquish )
 {
	UINT8 i;
	UINT16 iSquishedMap = m_PieceToSquish->iMap[ 0 ];
	
	for( i = 1; i < T_HEIGHT; i++ )
		iSquishedMap |= m_PieceToSquish->iMap[ i ];

	return iSquishedMap;
 }