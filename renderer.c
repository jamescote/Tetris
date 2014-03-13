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
#include "tetri.c"
#include "gm_cnsts.h"

/* Local Function Declaration */
void render_Line( UINT16* fbBase16, 
				  UINT16 iMap, 
				  int iYPxlPos, 
				  UINT8 bClearFlag,
				  bool bAND );
bool redrawTetrimino( const Tetrimino* m_CurrTetrimino );
void clear_Tetrimino( UINT16* fbBase16 );
void WToStr( char* cReturnString, UINT16 iValue );
void render_Value( UINT8* fbBase8, UINT16 iValue, UINT16 iXPxlPos, UINT16 iYPxlPos );
void render_Static( UINT16* fbBase16 );
void render_Status( UINT8* fbBase8, const Game_Board* m_Board );
void fill_Board( UINT16* fbBase16, const Game_Board* m_Board );
void render_Next_Tetrimino( UINT16* fbBase16, const Game_Board* m_Board );
				  
/* Private Variables */
struct sRendState
{
	Tetrimino m_TetriState;
	Game_Board m_BoardState;
	bool bInitialDrawn;
} sMainState[ 2 ] = 
{ 
	{
		{{0}, {0}},
		{{0},GARBAGE_INPUT_16,
			 GARBAGE_INPUT_16,
			 GARBAGE_INPUT_16,
			 GARBAGE_INPUT_8,
			 GARBAGE_INPUT_8,
			 GARBAGE_INPUT_8,
			 GARBAGE_INPUT_8,
			 GARBAGE_INPUT_8 },
		false
	},
	{
		{{0}, {0}},
		{{0},GARBAGE_INPUT_16,
			 GARBAGE_INPUT_16,
			 GARBAGE_INPUT_16,
			 GARBAGE_INPUT_8,
			 GARBAGE_INPUT_8,
			 GARBAGE_INPUT_8,
			 GARBAGE_INPUT_8,
			 GARBAGE_INPUT_8 },
		false
	}
};
UINT8 iCurrState = FRONT_STATE;
	


/* Function Implementation */


/*
	Resets the Rend State to it's starting value
*/
void reset_Rend_State( )
{
	UINT8 i, j;
	
	for( j = 0; j <= BACK_STATE; j++ )
	{
		/* Reset Tetrimino */
		sMainState[ j ].m_TetriState.bPos[ X_POS ] = 0;
		sMainState[ j ].m_TetriState.bPos[ Y_POS ] = 0;
		for( i = 0; i < T_HEIGHT; i++ )
			sMainState[ j ].m_TetriState.iMap[ i ] = 0;
			
		/* Reset Game Board */
		for( i = 0; i < BOARD_HEIGHT; i++ )
			sMainState[ j ].m_BoardState.BoardMap[ i ] = 0;
		sMainState[ j ].m_BoardState.iScore 			= GARBAGE_INPUT_16;
		sMainState[ j ].m_BoardState.iLns_Clrd 		= GARBAGE_INPUT_16;
		sMainState[ j ].m_BoardState.iLvl			= GARBAGE_INPUT_16;
		sMainState[ j ].m_BoardState.nxtPiece		= GARBAGE_INPUT_8;
		
		/* Reset Inital Drawn Flag */
		sMainState[ j ].bInitialDrawn				= false;
	}
}

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
			render_Line( fbBase16, 
						 m_TetriModel->iMap[ i ], 
						 (((BOARD_HEIGHT - 1) - iYPos) + i) << MINO_SHIFT_SIZE, 
						 LINE_DRAW,
						 false );
		
		copyTetrimino( &(sMainState[ iCurrState ].m_TetriState), m_TetriModel );
	}
}

/*
	Compares a passed in tetrimino object with the current render state to 
	determine if a tetrimino needs to be redrawn.
*/
bool redrawTetrimino( const Tetrimino* m_CurrTetrimino )
{
	bool bDifference;
	UINT8 i;

	bDifference = (sMainState[ iCurrState ].m_TetriState.bPos[ X_POS ] != m_CurrTetrimino->bPos[ X_POS ]);
	bDifference = bDifference || (sMainState[ iCurrState ].m_TetriState.bPos[ Y_POS ] != m_CurrTetrimino->bPos[ Y_POS ]);
	
	for( i = 0; i < T_HEIGHT; i++ )
		bDifference = bDifference || (sMainState[ iCurrState ].m_TetriState.iMap[ i ] != m_CurrTetrimino->iMap[ i ]);
		
	return bDifference;
}

/* 
	Function to only clear the Tetrimino Object. 
*/
void clear_Tetrimino( UINT16* fbBase16 )
{
	UINT16 iYPxlPos = BOARD_BOTTOM_LINE_Y - ((sMainState[ iCurrState ].m_TetriState.bPos[ Y_POS ] + 1) << MINO_SHIFT_SIZE);
	UINT8 i;
	
	for( i = 0; i < T_HEIGHT; i++ )
	{
		if( sMainState[ iCurrState ].m_TetriState.iMap[ i ] != 0 )
		{
			render_Line( fbBase16, 
						 sMainState[ iCurrState ].m_TetriState.iMap[ i ], 
					     iYPxlPos, LINE_CLEAR, false );
		}
		iYPxlPos += MINO_SIZE;
	}
}

/*
	Taking in a Map, renders a line on the game board.
	Last two parameters act as bit flags for the rendering:
	bDrawFlag	| bAND	| Result
	0			  1		  - Clear the whole line
	0			  0		  - Clear active bits
	1			  1		  - Draw active and clear inactive
	1			  0		  - Draw only active
*/
void render_Line( UINT16* fbBase16, 
					 UINT16 iMap, 
					 int iYPxlPos,
					 UINT8 bDrawFlag,
					 bool bAND )
				  
{
	UINT16 i, iXPxlPos = (BOARD_DRAW_END_X - MINO_SIZE);
	UINT16 iYPxlEndPos;
	UINT8 bActiveBit;
	
	if( iYPxlPos >= 0 && iYPxlPos < SCREEN_HEIGHT )
	{
		iYPxlEndPos = iYPxlPos + MINO_SIZE;
		for( i = 0; i < BOARD_WIDTH; i++ )
		{		
			bActiveBit = (iMap & (TETRI_MAP_BASE - 1));
			
			if( bDrawFlag && bActiveBit ) 
				/*draw_square( fbBase16, iXPxlPos, iYPxlPos );*/
				draw_bitmap_16( fbBase16,
								iXPxlPos,
								iYPxlPos,
								iMinoBitmap,
								BMP_HEIGHT );
			else if( bAND || bActiveBit)
				clear_region( fbBase16,
							  iXPxlPos,
							  iXPxlPos + MINO_SIZE,
							  iYPxlPos,
							  iYPxlEndPos );
			iMap >>= 1;
			iXPxlPos -= MINO_SIZE;
		}	
	}
}

/*
	Renders the Game Board -- Anything that needs to be displayed to the
	player in terms of the state of the board.
*/
void render_Board( UINT16* fbBase16, 
				   const Game_Board* m_Board )
{	
	/* Draw Borders */
	if( !sMainState[ iCurrState ].bInitialDrawn )
	{
		render_Static( fbBase16 );
		sMainState[ iCurrState ].bInitialDrawn = true;
	}
	
	
	/* Fill Board */
	fill_Board( fbBase16, m_Board );
	
	render_Next_Tetrimino( fbBase16, m_Board );
	
	render_Status( (UINT8*)fbBase16, m_Board );
	
	iCurrState = !iCurrState;
}

/*
	Populate the board with locked Tetriminos.
*/
void fill_Board( UINT16* fbBase16, const Game_Board* m_Board )
{
	UINT16 iYPxlPos = BOARD_BOTTOM_LINE_Y - MINO_SIZE;
	UINT8 i = 0;
	
	while( i < BOARD_HEIGHT && 
		   ( m_Board->BoardMap[ i ] != 0 || sMainState[ iCurrState ].m_BoardState.BoardMap[ i ] != 0 ) )
	{
		if( m_Board->BoardMap[ i ] != sMainState[ iCurrState ].m_BoardState.BoardMap[ i ] )
		{
			render_Line( fbBase16, 
						 m_Board->BoardMap[ i ], 
						 iYPxlPos,
						 LINE_DRAW,
						 true );
			sMainState[ iCurrState ].m_BoardState.BoardMap[ i ] = m_Board->BoardMap[ i ];
		}
		i++;
		iYPxlPos -= MINO_SIZE;
	}
}

/*
	Renders the Next Tetrimino that will be spawned.
*/
void render_Next_Tetrimino( UINT16* fbBase16, const Game_Board* m_Board )
{
	UINT8 iTetMap;
	UINT16 iXPxlPos = NEXT_TET_RIGHT_BORDER_X - MINO_SIZE;
	UINT16 iCurrXPxlPos = iXPxlPos;
	UINT16 iYPxlPos = NEXT_TET_TOP_BORDER_Y;
	UINT8 i, j;
	
	if( m_Board->nxtPiece != sMainState[ iCurrState ].m_BoardState.nxtPiece )
	{	
		clear_region( fbBase16,
					  NEXT_TET_LEFT_BORDER_X,
					  NEXT_TET_RIGHT_BORDER_X,
					  NEXT_TET_TOP_BORDER_Y,
					  NEXT_TET_BOTTOM_BORDER_Y );
					  
		for( i = 0; i < T_HEIGHT; i++ )
		{
			iTetMap = iNextMaps[ m_Board->nxtPiece ][ i ];
			for( j = 0; j < T_WIDTH; j++ )
			{
				if( (iTetMap & 1) != 0 )
					draw_bitmap_16( fbBase16,
									iCurrXPxlPos, iYPxlPos,
									iMinoBitmap,
									BMP_HEIGHT );
				iTetMap >>= 1;
				iCurrXPxlPos -= MINO_SIZE;
			}
			iCurrXPxlPos = iXPxlPos;
			iYPxlPos += MINO_SIZE;
		}
		
		sMainState[ iCurrState ].m_BoardState.nxtPiece = m_Board->nxtPiece;
	}
}

void render_Status( UINT8* fbBase8, const Game_Board* m_Board )
{
	/* Score Value */
	if( m_Board->iScore != sMainState[ iCurrState ].m_BoardState.iScore )
	{
		render_Value( fbBase8, m_Board->iScore, VALUE_X_POS, SCORE_Y_POS );
		sMainState[ iCurrState ].m_BoardState.iScore = m_Board->iScore;
	}
	
	/* Level Value */
	if( m_Board->iLvl != sMainState[ iCurrState ].m_BoardState.iLvl )
	{
		render_Value( fbBase8, m_Board->iLvl + 1, VALUE_X_POS, LEVEL_Y_POS );
		sMainState[ iCurrState ].m_BoardState.iLvl = m_Board->iLvl;
	}
	
	/* Lines Cleared Value */
	if( m_Board->iLns_Clrd != sMainState[ iCurrState ].m_BoardState.iLns_Clrd )
	{
		render_Value( fbBase8, m_Board->iLns_Clrd, VALUE_X_POS, LNS_CLRD_Y_POS );
		sMainState[ iCurrState ].m_BoardState.iLns_Clrd = m_Board->iLns_Clrd;
	}
}

void render_Value( UINT8* fbBase8, UINT16 iValue, UINT16 iXPxlPos, UINT16 iYPxlPos )
{
	char cValueBuffer[ MAX_STR_LENGTH ] = {0x0};
	
	WToStr( cValueBuffer, iValue );

	render_String( fbBase8, cValueBuffer,
				   iXPxlPos, iYPxlPos );
}

void render_Static( UINT16* fbBase16 )
{
	clear_Screen( fbBase16 );
	
	/* Main Game Board Borders */
	plot_v_line( (UINT8*)fbBase16, LEFT_BORDER_X, 0, BOTTOM_BORDER_Y );
	plot_v_line( (UINT8*)fbBase16, RIGHT_BORDER_X, 0, BOTTOM_BORDER_Y );
	plot_h_line( (UINT32*)fbBase16, 
				 LEFT_BORDER_X, 
				 RIGHT_BORDER_X, 
				 BOTTOM_BORDER_Y );
				 
	/* Draw Labels */
	render_String( (UINT8*)fbBase16,
				   "NEXT:",
				   LABEL_NEXT_X_POS,
				   LABEL_NEXT_Y_POS );
	render_String( (UINT8*)fbBase16,
				   "SCORE: ",
				   LABEL_X_POS,
				   SCORE_Y_POS );
	render_String( (UINT8*)fbBase16,
				   "LEVEL: ",
				   LABEL_X_POS,
				   LEVEL_Y_POS );
	render_String( (UINT8*)fbBase16,
				   "LINES CLEARED: ",
				   LABEL_X_POS,
				   LNS_CLRD_Y_POS );
				 
	/* Next Tetrimino area */
	plot_h_line( (UINT32*)fbBase16,	/* Top Line - Outside */
				 RIGHT_BORDER_X,
				 NEXT_TET_RIGHT_BORDER_X + NEXT_TET_BORDER_WIDTH,
				 NEXT_TET_TOP_BORDER_Y - NEXT_TET_BORDER_WIDTH );
	plot_h_line( (UINT32*)fbBase16,	/* Top Line - Inside */
				 NEXT_TET_LEFT_BORDER_X,
				 NEXT_TET_RIGHT_BORDER_X,
				 NEXT_TET_TOP_BORDER_Y - 1 );
	plot_v_line( (UINT8*)fbBase16,	/* Left Line - Inside */
				 NEXT_TET_LEFT_BORDER_X - 1,
				 NEXT_TET_TOP_BORDER_Y,
				 NEXT_TET_BOTTOM_BORDER_Y );
	plot_v_line( (UINT8*)fbBase16,	/* Right Line - Inside */
				 NEXT_TET_RIGHT_BORDER_X + 1,
				 NEXT_TET_TOP_BORDER_Y,
				 NEXT_TET_BOTTOM_BORDER_Y );
	plot_v_line( (UINT8*)fbBase16,	/* Right Line - Outside */
				 NEXT_TET_RIGHT_BORDER_X + NEXT_TET_BORDER_WIDTH,
				 NEXT_TET_TOP_BORDER_Y - NEXT_TET_BORDER_WIDTH,
				 NEXT_TET_BOTTOM_BORDER_Y + NEXT_TET_BORDER_WIDTH );
	plot_h_line( (UINT32*)fbBase16,	/* Bottom Line - Outside */
				 RIGHT_BORDER_X,
				 NEXT_TET_RIGHT_BORDER_X + NEXT_TET_BORDER_WIDTH,
				 NEXT_TET_BOTTOM_BORDER_Y + NEXT_TET_BORDER_WIDTH );
	plot_h_line( (UINT32*)fbBase16, /* Bottom Line - Inside */
				 NEXT_TET_LEFT_BORDER_X,
				 NEXT_TET_RIGHT_BORDER_X,
				 NEXT_TET_BOTTOM_BORDER_Y + 1 );
				 
	sMainState[ iCurrState ].bInitialDrawn = true;
}
				   
/*
	Take in a Model object and render both the Tetrimino and the Game Board.
*/
void render_All( UINT16* fbBase16,  
				 const Game_Model* m_GameModel )
{
	render_Tetrimino( fbBase16, &(m_GameModel->cCurrPiece) );
	render_Board( fbBase16, &(m_GameModel->cMainBoard) );
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

/*
	Externally accessible function to clear the screen
*/
void clear_Screen( UINT16* fbBase16 )
{
	clear_region( fbBase16, 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT );
	reset_Rend_State( );
}
