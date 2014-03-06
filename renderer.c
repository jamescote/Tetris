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

/* Character Map */
/* TODO: Implement */

/* Local Function Declaration */
void render_line( UINT16* fbBase16, 
				  UINT16 iMap, int iYPxlPos );
void render_board_line( UINT16* fbBase16, 
						UINT16 iNewMap, 
						UINT16 iOldMap, 
						UINT16 iYPxlPos );
				  
/* Private Variables */
struct sRendState
{
	UINT16 BoardMap[ BOARD_HEIGHT ];
	UINT16 bTPos[ 2 ];
	UINT8 bBorderDrawn;
	UINT8 iNextPce;
	UINT16 iCurrScore;
	UINT16 iLnsCleared;
} sMainState = { {0x0}, {TETRI_START_POS_X,TETRI_START_POS_Y}, 0, 0, 0, 0 };

/* Function Implementation */

/*
	Renders a Tetrimino in Board Space.
*/
void render_tetrimino( UINT16* fbBase16, 
					   const struct Tetrimino* m_TetriModel )
{	
	int iYPos = m_TetriModel->bPos[ Y_POS ];
	UINT16 iYPxlPos, iXPxlPos;
	UINT8 i = ( iYPos < 0 ? 0 - iYPos : 0 );
	
	if( iYPos != sMainState.bTPos[ Y_POS ] || 
		m_TetriModel->bPos[ X_POS ] != sMainState.bTPos[ X_POS ] )
	{
		iYPxlPos = BOARD_BOTTOM_LINE_Y - (sMainState.bTPos[ Y_POS ] << MINO_SHIFT_SIZE);
		iXPxlPos = BOARD_DRAW_END_X - ((sMainState.bTPos[ X_POS ] + 1) << MINO_SHIFT_SIZE);
		clear_region( fbBase16, 
					  iXPxlPos,
					  iXPxlPos + (MINO_SIZE << TETRI_MAP_BASE),
					  iYPxlPos,
					  iYPxlPos + (MINO_SIZE << TETRI_MAP_BASE) );
	
		for( i; i < T_HEIGHT; i++ )
		{	
			render_line( fbBase16, m_TetriModel->iMap[ i ], ((BOARD_HEIGHT - iYPos) + i) << MINO_SHIFT_SIZE );
		}
		
		for( i = 0; i < 2; i++ )
			sMainState.bTPos[ i ] = m_TetriModel->bPos[ i ];
	}
}

/*
	Taking in a Map, renders a line on the game board.
*/
void render_line( UINT16* fbBase16, 
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
void render_board( UINT16* fbBase16, 
				   const struct Game_Board* m_Board )
{
	UINT16 iYPxlPos = BOARD_BOTTOM_LINE_Y - MINO_SIZE;
	UINT16 iSideBottomBorders = BOARD_BOTTOM_LINE_Y - 1;
	UINT8 i;
	
	/* Draw Borders */
	if( !sMainState.bBorderDrawn )
	{
		plot_v_line( (UINT8*)fbBase16, BOARD_DRAW_START_X, 0, iSideBottomBorders );
		plot_v_line( (UINT8*)fbBase16, BOARD_DRAW_END_X, 0, iSideBottomBorders );
		plot_h_line( (UINT32*)fbBase16, 
					 BOARD_DRAW_START_X, 
					 BOARD_DRAW_END_X, 
					 BOARD_BOTTOM_LINE_Y );
					 
		sMainState.bBorderDrawn = 1;
	}
	
	/* Fill Board */
	i = 0;
	while( i < BOARD_HEIGHT && 
		   ( m_Board->BoardMap[ i ] != 0 || sMainState.BoardMap[ i ] != 0 ) )
	{
		if( m_Board->BoardMap[ i ] != sMainState.BoardMap[ i ] )
		{
			render_board_line( fbBase16, 
							   m_Board->BoardMap[ i ], 
							   sMainState.BoardMap[ i ], 
							   iYPxlPos );
			sMainState.BoardMap[ i ] = m_Board->BoardMap[ i ];
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
void render_board_line( UINT16* fbBase16, UINT16 iNewMap, UINT16 iOldMap, UINT16 iYPxlPos )
{
	UINT16 iXPxlPos = BOARD_DRAW_END_X - MINO_SIZE,
		   iYPxlEndPos = iYPxlPos + (MINO_SIZE - 1);
	UINT8 j, iOldBit, iNewBit;
	
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
void render_all( UINT16* fbBase16,  
				 const struct Model* m_GameModel )
{
	render_tetrimino( fbBase16, (&m_GameModel->cCurrPiece) );
	render_board( fbBase16, (&m_GameModel->cMainBoard) );
}

/*
	Take in a String and Output each character as a bitmap from the fonts
	object.
*/
void render_string( UINT8* fbBase8, const char* sText, UINT16 iXPxlPos, UINT16 iYPxlPos )
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
			draw_bitmap_8( fbBase8, iCurrX, iCurrY, charMap[ ' ' ], BMP_HEIGHT );
		
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
