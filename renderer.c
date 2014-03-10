/*
	Name: renderer.c
	Purpose: Communicate with low-level rasterizing routines to draw Game objects
			 onto the screen.
	Written By: James Cote
*/

/* Includes */
#include "renderer.h"
#include "raster.h"
#include "font.c"
#include "gm_cnsts.h"

/* TODO: REMOVE AFTER TESTING */
#include <stdio.h>

/* Local Function Declaration */
void render_Line( UINT16* fbBase16, 
				  UINT16 iMap, int iYPxlPos );
void rend_Board_Line( UINT16* fbBase16, 
						UINT16 iNewMap, 
						UINT16 iOldMap, 
						UINT16 iYPxlPos );
bool redrawTetrimino( const Tetrimino* m_CurrTetrimino );
void clear_Tetrimino( UINT16* fbBase16 );
				  
/* Private Variables 
struct sRendState
{
	UINT16 BoardMap[ BOARD_HEIGHT ];
	UINT16 bTPos[ 2 ];
	UINT8 bBorderDrawn;
	UINT8 iNextPce;
	UINT16 iCurrScore;
	UINT16 iLnsCleared;
} sMainState = { {0x0}, {0,0}, 0, 0, 0, 0 };*/
struct sRendState
{
	Tetrimino m_TetriState;
	Game_Board m_BoardState;
	bool bBorderDrawn;
} sMainState = 
	{ 
		{{0}, {0}},
		{{0},0,0,0,0,0,0,0,0},
		false
	};

/* Function Implementation */

/*
	Renders a Tetrimino in Board Space.
*/
void render_Tetrimino( UINT16* fbBase16, 
					   const Tetrimino* m_TetriModel )
{	
	int iYPos = m_TetriModel->bPos[ Y_POS ];
	UINT8 i = ( iYPos < 0 ? 0 - iYPos : 0 );
	
	if( redrawTetrimino( m_TetriModel ) )
	{
		clear_Tetrimino( fbBase16 );
		
		for( i; i < T_HEIGHT; i++ )
			render_Line( fbBase16, m_TetriModel->iMap[ i ], (((BOARD_HEIGHT - 1) - iYPos) + i) << MINO_SHIFT_SIZE );
		
		copyTetrimino( &(sMainState.m_TetriState), m_TetriModel );
	}
}

/*
	Compares a passed in tetrimino object with the current render state to 
	determine if a tetrimino needs to be redrawn.
*/
bool redrawTetrimino( const Tetrimino* m_CurrTetrimino )
{
	bool bReturnValue;
	UINT8 i;

	bReturnValue = (sMainState.m_TetriState.bPos[ X_POS ] != m_CurrTetrimino->bPos[ X_POS ]);
	bReturnValue = bReturnValue || (sMainState.m_TetriState.bPos[ Y_POS ] != m_CurrTetrimino->bPos[ Y_POS ]);
	
	for( i = 0; i < T_HEIGHT; i++ )
		bReturnValue = bReturnValue || (sMainState.m_TetriState.iMap[ i ] != m_CurrTetrimino->iMap[ i ]);
		
	return bReturnValue;
}

/* 
	Function to only clear the Tetrimino Object. 
*/
void clear_Tetrimino( UINT16* fbBase16 )
{
	UINT16 iYPxlPos = BOARD_BOTTOM_LINE_Y - ((sMainState.m_TetriState.bPos[ Y_POS ] + 1) << MINO_SHIFT_SIZE);
	UINT16 iXPxlPos, iXPxlEndPos;
	UINT8 i, xPos = sMainState.m_TetriState.bPos[ X_POS ];
	UINT8 iEndBit;
	UINT8 iStartBit;
	UINT16 iMapCopy;
	
	for( i = 0; i < T_HEIGHT; i++ )
	{
		if( sMainState.m_TetriState.iMap[ i ] != 0 )
		{
			iMapCopy = sMainState.m_TetriState.iMap[ i ];
			iEndBit = 0;
			iStartBit = 1;
			
			while( iMapCopy != 0 )
			{
				iMapCopy >>= 1;
				
				if( !(iMapCopy & 1) )
					iEndBit++;
				
				iStartBit++;
			}
			
			iXPxlPos = BOARD_DRAW_END_X - ( iStartBit << MINO_SHIFT_SIZE );
			iXPxlEndPos = BOARD_DRAW_END_X - ( iEndBit << MINO_SHIFT_SIZE );
			
			clear_region( fbBase16,
						  iXPxlPos,
						  iXPxlEndPos,
						  iYPxlPos,
						  iYPxlPos + MINO_SIZE );
		}
		iYPxlPos += MINO_SIZE;
	}
}

/*
	Taking in a Map, renders a line on the game board.
*/
void render_Line( UINT16* fbBase16, 
				  UINT16 iMap, 
				  int iYPxlPos )
				  
{
	UINT16 i, iXPxlPos = (BOARD_DRAW_END_X - MINO_SIZE);
	
	if( iYPxlPos >= 0 && iYPxlPos < SCREEN_HEIGHT )
	{
		for( i = 0; i < BOARD_WIDTH; i++ )
		{
			if( iMap & (TETRI_MAP_BASE - 1) != 0 )
			{
				draw_square( fbBase16, 
							 iXPxlPos,
							 iYPxlPos );
			}
			iMap >>= 1;
			iXPxlPos -= MINO_SIZE;
		}	
	}
}

/*
	Renders the Game Board -- Anything that needs to be displayed to the
	player in terms of the state of the board.
*/
void rend_Board( UINT16* fbBase16, 
				   const Game_Board* m_Board )
{
	UINT16 iYPxlPos = BOARD_BOTTOM_LINE_Y - MINO_SIZE;
	UINT16 iSideBottomBorders = BOARD_BOTTOM_LINE_Y - 1;
	UINT8 i;
	
	/* Draw Borders */
	if( !sMainState.bBorderDrawn )
	{
		plot_v_line( (UINT8*)fbBase16, BOARD_DRAW_START_X, 0, iSideBottomBorders );
		plot_v_line( (UINT8*)fbBase16, BOARD_DRAW_END_X, 0, iSideBottomBorders );
		plot_h_line( (ULONG32*)fbBase16, 
					 BOARD_DRAW_START_X, 
					 BOARD_DRAW_END_X, 
					 BOARD_BOTTOM_LINE_Y );
					 
		sMainState.bBorderDrawn = true;
	}
	
	/* Fill Board */
	i = 0;
	while( i < BOARD_HEIGHT && 
		   ( m_Board->BoardMap[ i ] != 0 || sMainState.m_BoardState.BoardMap[ i ] != 0 ) )
	{
		if( m_Board->BoardMap[ i ] != sMainState.m_BoardState.BoardMap[ i ] )
		{
			rend_Board_Line( fbBase16, 
							   m_Board->BoardMap[ i ], 
							   sMainState.m_BoardState.BoardMap[ i ], 
							   iYPxlPos );
			sMainState.m_BoardState.BoardMap[ i ] = m_Board->BoardMap[ i ];
		}
		i++;
		iYPxlPos -= MINO_SIZE;
	}
	
	/* 	TODO: 	Render next-tetrimino block
				Render Text Block for Score, lines cleared, etc.*/
}

/*
	Parses a line to render to the screen from the board map while comparing it
	to the original line that was there.  clears or adds a space as needed and
	leaves the space if unchanged.
*/
void rend_Board_Line( UINT16* fbBase16, UINT16 iNewMap, UINT16 iOldMap, UINT16 iYPxlPos )
{
	UINT16 iXPxlPos = BOARD_DRAW_END_X - MINO_SIZE,
		   iYPxlEndPos = iYPxlPos + (MINO_SIZE - 1);
	UINT8 j, iOldBit, iNewBit;
	
	printf( "REND_BOARD" );
	
	/* Check for Clearing Spaces */
	for( j = 0; j < BOARD_WIDTH; j++ )
	{
		iOldBit = (iOldMap & 1);
		iNewBit = (iNewMap & 1);
		if( iOldBit != iNewBit )
		{
			if( 0 == iNewBit )
				clear_region( fbBase16, 
							  iXPxlPos, 
							  iXPxlPos + (MINO_SIZE - 1), 
							  iYPxlPos, 
							  iYPxlEndPos );
			else
				draw_square( fbBase16, 
						 iXPxlPos,
						 iYPxlPos );
		}
			
		iOldMap >>= 1;
		iNewMap >>= 1;
		iXPxlPos -= MINO_SIZE;
	}
}
				   
/*
	Take in a Model object and render both the Tetrimino and the Game Board.
*/
void render_All( UINT16* fbBase16,  
				 const Game_Model* m_GameModel )
{
	render_Tetrimino( fbBase16, &(m_GameModel->cCurrPiece) );
	rend_Board( fbBase16, &(m_GameModel->cMainBoard) );
}

/*
	Take in a String and Output each character as a bitmap from the fonts
	object.
*/
void render_String( UINT8* fbBase8, const char* sText, UINT16 iXPxlPos, UINT16 iYPxlPos )
{
	UINT16 iCurrX = iXPxlPos, iCurrY = iYPxlPos;
	UINT8 iStrIndex = 0;
	bool bNewLine = false;
	
	while( iStrIndex < MAX_STR_LENGTH && sText[ iStrIndex ] != 0 )
	{
		if( sText[ iStrIndex ] == '\n' )
			bNewLine = true;
		else if( sText[ iStrIndex ] == '\t' )
		{
			while( (iCurrX += BMP_WIDTH) % 31 )
				;
			if( iCurrX >= SCREEN_WIDTH )
				bNewLine = true;
		}
		else if( sText[ iStrIndex ] < MAX_CHAR )
			draw_bitmap_8( fbBase8, iCurrX, iCurrY, charMap[ sText[ iStrIndex ] ], BMP_HEIGHT );
		else
			draw_bitmap_8( fbBase8, iCurrX, iCurrY, charMap[ 0 ], BMP_HEIGHT );
		
		if( bNewLine )
		{
			iCurrX = iXPxlPos;
			iCurrY += BMP_HEIGHT;
			bNewLine = false;
		}
		else
			iCurrX += BMP_WIDTH;
			
		iStrIndex++;
	}
}
