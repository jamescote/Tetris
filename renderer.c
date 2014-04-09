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
#include "mnu_cns.h"
#include "mminos.c"

/* Local Function Declaration */
void render_Line( UINT16* fbBase16, 
				  UINT16 iMap, 
				  int iYPxlPos, 
				  UINT8 bClearFlag,
				  bool bAND );
bool redrawTetrimino( const Tetrimino* m_CurrTetrimino );
void clear_Tetrimino( UINT16* fbBase16 );
void WToStr( char* cReturnString, UINT16 iValue );
void render_Status( UINT8* fbBase8, const Game_Board* m_Board );
void render_Value( UINT8* fbBase8, UINT16 iValue, UINT16 iXPxlPos, UINT16 iYPxlPos );
void fill_Board( UINT16* fbBase16, const Game_Board* m_Board );
void render_Next_Tetrimino( UINT16* fbBase16, const Game_Board* m_Board );
void render_FPS( UINT8* fbBase8, int iValue );
void draw_Selection_Mino( UINT16* fbBase16, UINT8 bNewSelection );
void render_Menu_Static( UINT16* fbBase16 );
				  
/* Private Variables */
const stMenuMino m_SelMinoPositions[ RUN_MENU ][ 2 ] =
{
	{ 
		{ QUIT_MINO_X1, QUIT_MINO_Y }, 
		{ QUIT_MINO_X2, QUIT_MINO_Y }
	},
	{ 
		{ ONE_P_MINO_X1, ONE_P_MINO_Y }, 
		{ ONE_P_MINO_X2, ONE_P_MINO_Y }
	},
	{ 
		{ TWO_P_MINO_X1, TWO_P_MINO_Y }, 
		{ TWO_P_MINO_X2, TWO_P_MINO_Y }
	}
};

struct stMenuRendState
{
	bool bStaticDrawn;
	UINT8 bCurrSelection;
} stMenuRendState[ 2 ] =
{
	{ false, RUN_MENU },
	{ false, RUN_MENU }
};

struct sRendState
{
	Tetrimino m_TetriState;
	UINT16 BoardRendMap[ BOARD_HEIGHT ];
	UINT16 iRendScore, iRendLns_Clrd, iRendLvl;
	UINT8 iRendNxtPiece;
	bool bRenderFPS;
	bool bClearFPS;
} sMainState[ 2 ] = 
{ 
	{
		{{0}, {0}},
		{0},
		GARBAGE_INPUT_16,
		GARBAGE_INPUT_16,
		GARBAGE_INPUT_16,
		GARBAGE_INPUT_8
	},
	{
		{{0}, {0}},
		{0},
		GARBAGE_INPUT_16,
		GARBAGE_INPUT_16,
		GARBAGE_INPUT_16,
		GARBAGE_INPUT_8
	}
};
UINT8 iCurrState = FRONT_STATE;
UINT8 iCurrMenuState = FRONT_STATE;

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
			sMainState[ j ].BoardRendMap[ i ] = 0;
		sMainState[ j ].iRendScore 		= GARBAGE_INPUT_16;
		sMainState[ j ].iRendLns_Clrd 		= GARBAGE_INPUT_16;
		sMainState[ j ].iRendLvl			= GARBAGE_INPUT_16;
		sMainState[ j ].iRendNxtPiece		= GARBAGE_INPUT_8;
		
		stMenuRendState[ j ].bStaticDrawn = false;
		stMenuRendState[ j ].bCurrSelection = RUN_MENU;
	}
}

/*
	Sets a flag so that, on the next render, a new FPS will be rendered.
*/
void flag_FPS_Rend( )
{
	UINT8 i;
	
	for( i = 0; i <= BACK_STATE; i++ )
		sMainState[ i ].bRenderFPS = true;
}

/*
	Sets a flag to clear the FPS on the next render
*/
void flag_FPS_Clear( )
{
	UINT8 i;
	
	for( i = 0; i <= BACK_STATE; i++ )
		sMainState[ i ].bClearFPS = true;
}

/*
	Draws the FPS in the top left corner of the screen
	or clears it based on the FPS render flags.
*/
void render_FPS( UINT8* fbBase8, int iValue )
{
	if( sMainState[ iCurrState ].bRenderFPS )
	{
		render_Value( fbBase8, iValue, 0, 0 );
		sMainState[ iCurrState ].bRenderFPS = false;
	}
	else if( sMainState[ iCurrState ].bClearFPS )
	{
		clear_region( (UINT16*)fbBase8, 0, TEXT_WIDTH << 1, 0, TEXT_HEIGHT );
		sMainState[ iCurrState ].bClearFPS = false;
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
	fill_Board( fbBase16, m_Board );
	
	render_Next_Tetrimino( fbBase16, m_Board );
	
	render_Status( (UINT8*)fbBase16, m_Board );
}

/*
	Populate the board with locked Tetriminos.
*/
void fill_Board( UINT16* fbBase16, const Game_Board* m_Board )
{
	UINT16 iYPxlPos = BOARD_BOTTOM_LINE_Y - MINO_SIZE;
	UINT8 i = 0;
	
	while( i < BOARD_HEIGHT && 
		   ( m_Board->BoardMap[ i ] != 0 || sMainState[ iCurrState ].BoardRendMap[ i ] != 0 ) )
	{
		if( m_Board->BoardMap[ i ] != sMainState[ iCurrState ].BoardRendMap[ i ] )
		{
			render_Line( fbBase16, 
						 m_Board->BoardMap[ i ], 
						 iYPxlPos,
						 LINE_DRAW,
						 true );
			sMainState[ iCurrState ].BoardRendMap[ i ] = m_Board->BoardMap[ i ];
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
	
	if( m_Board->nxtPiece != sMainState[ iCurrState ].iRendNxtPiece )
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
		
		sMainState[ iCurrState ].iRendNxtPiece = m_Board->nxtPiece;
	}
}

/*
	renders the values that are shown to the user:
		- Score
		- Level
		- Lines Cleared
*/
void render_Status( UINT8* fbBase8, const Game_Board* m_Board )
{
	/* Score Value */
	if( m_Board->iScore != sMainState[ iCurrState ].iRendScore )
	{
		render_Value( fbBase8, m_Board->iScore, VALUE_X_POS, SCORE_Y_POS );
		sMainState[ iCurrState ].iRendScore = m_Board->iScore;
	}
	
	/* Level Value */
	if( m_Board->iLvl != sMainState[ iCurrState ].iRendLvl )
	{
		render_Value( fbBase8, m_Board->iLvl + 1, VALUE_X_POS, LEVEL_Y_POS );
		sMainState[ iCurrState ].iRendLvl = m_Board->iLvl;
	}
	
	/* Lines Cleared Value */
	if( m_Board->iLns_Clrd != sMainState[ iCurrState ].iRendLns_Clrd )
	{
		render_Value( fbBase8, m_Board->iLns_Clrd, VALUE_X_POS, LNS_CLRD_Y_POS );
		sMainState[ iCurrState ].iRendLns_Clrd = m_Board->iLns_Clrd;
	}
}

/* 
	Takes in a 16-bit unsigned word value and draws it
	to a specified area on the screen.
*/
void render_Value( UINT8* fbBase8, UINT16 iValue, UINT16 iXPxlPos, UINT16 iYPxlPos )
{
	char cValueBuffer[ MAX_STR_LENGTH ] = {0x0};
	
	WToStr( cValueBuffer, iValue );

	render_String( fbBase8, cValueBuffer,
				   iXPxlPos, iYPxlPos );
}

/*
	Renders any static objects that are never meant to change
	during game play:  Borders & Labels
*/
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
}
				   
/*
	Take in a Model object and render both the Tetrimino and the Game Board.
*/
void render_All( UINT16* fbBase16,  
				 const Game_Model* m_GameModel,
				 int iFPS )
{
	iCurrState = 1 - iCurrState;
	render_Tetrimino( fbBase16, &(m_GameModel->cCurrPiece) );
	render_Board( fbBase16, &(m_GameModel->cMainBoard) );
	render_FPS( (UINT8*)fbBase16, iFPS );
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
			while( (iCurrX += TEXT_WIDTH) & TAB_WIDTH_MASK )
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
			iCurrY += TEXT_HEIGHT;
			bNewLine = false;
		}
		else
			iCurrX += TEXT_WIDTH;
			
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

/*
	Renders the Main Menu
*/
void draw_Menu( UINT16* fbBase16, UINT8 Draw_Selection )
{
	iCurrMenuState = 1 - iCurrMenuState;
	if( !stMenuRendState[ iCurrMenuState ].bStaticDrawn )
	{
		render_Menu_Static( fbBase16 );
		stMenuRendState[ iCurrMenuState ].bStaticDrawn = true;
	}
	
	if( stMenuRendState[ iCurrMenuState ].bCurrSelection != Draw_Selection &&
	    Draw_Selection >= QUIT_GAME && Draw_Selection < RUN_MENU )
	{
		draw_Selection_Mino( fbBase16, Draw_Selection );
		stMenuRendState[ iCurrMenuState ].bCurrSelection = Draw_Selection;
	}		
}

void draw_Selection_Mino( UINT16* fbBase16, UINT8 bNewSelection )
{
	UINT8 y;
	
	for( y = 0; y < 2; y++ )
	{
		if( stMenuRendState[ iCurrMenuState ].bCurrSelection != RUN_MENU )
			clear_region( fbBase16, 
						  m_SelMinoPositions[ stMenuRendState[ iCurrMenuState ].bCurrSelection ][ y ].wXPos,
						  m_SelMinoPositions[ stMenuRendState[ iCurrMenuState ].bCurrSelection ][ y ].wXPos + MINO_SIZE, 
						  m_SelMinoPositions[ stMenuRendState[ iCurrMenuState ].bCurrSelection ][ y ].wYPos, 
						  m_SelMinoPositions[ stMenuRendState[ iCurrMenuState ].bCurrSelection ][ y ].wYPos + BMP_HEIGHT );
						  
		draw_bitmap_16( fbBase16,
						m_SelMinoPositions[ bNewSelection ][ y ].wXPos, 
						m_SelMinoPositions[ bNewSelection ][ y ].wYPos,
						iMinoBitmap,
						BMP_HEIGHT );
	}			
}

void render_Menu_Static( UINT16* fbBase16 )
{
	UINT8 x;
	UINT16 y;
	
	/* Draw Mino BackGround */
	for( x = 0; x < MAX_MENU_MINOS; x++ )
		for( y = m_MenuMinos[ x ].wYPos; y <= (SCREEN_HEIGHT - MINO_SIZE); y += MINO_SIZE )
			draw_bitmap_16( fbBase16,
							m_MenuMinos[ x ].wXPos, y,
							iMinoBitmap,
							BMP_HEIGHT );
							
	render_String( (UINT8*)fbBase16, "TETRIS", 
				   TITLE_X_POS, TITLE_Y_POS );
	
	/* draw 1P box */
	plot_v_line( (UINT8*)fbBase16,	/* Left Border */
				 ONE_P_X_POS,
				 ONE_P_Y_POS,
				 ONE_P_Y_POS + BOX_HEIGHT );
	plot_h_line( (UINT32*)fbBase16,	/* Top Border */
				 ONE_P_X_POS,
				 ONE_P_X_POS + BOX_WIDTH,
				 ONE_P_Y_POS );
	plot_v_line( (UINT8*)fbBase16,	/* Right Border */
				 ONE_P_X_POS + BOX_WIDTH,
				 ONE_P_Y_POS,
				 ONE_P_Y_POS + BOX_HEIGHT );
	plot_h_line( (UINT32*)fbBase16,	/* Bottom Border */
				 ONE_P_X_POS,
				 ONE_P_X_POS + BOX_WIDTH,
				 ONE_P_Y_POS + BOX_HEIGHT );
				 
	render_String( (UINT8*)fbBase16, "1-Player", 
				   ONE_P_TXT_X, ONE_P_TXT_Y );
				 
	/* draw 2P box */
	plot_v_line( (UINT8*)fbBase16,	/* Left Border */
				 TWO_P_X_POS,
				 TWO_P_Y_POS,
				 TWO_P_Y_POS + BOX_HEIGHT );
	plot_h_line( (UINT32*)fbBase16,	/* Top Border */
				 TWO_P_X_POS,
				 TWO_P_X_POS + BOX_WIDTH,
				 TWO_P_Y_POS );
	plot_v_line( (UINT8*)fbBase16,	/* Right Border */
				 TWO_P_X_POS + BOX_WIDTH,
				 TWO_P_Y_POS,
				 TWO_P_Y_POS + BOX_HEIGHT );
	plot_h_line( (UINT32*)fbBase16,	/* Bottom Border */
				 TWO_P_X_POS,
				 TWO_P_X_POS + BOX_WIDTH,
				 TWO_P_Y_POS + BOX_HEIGHT );
				 
	render_String( (UINT8*)fbBase16,"2-Player", 
				   TWO_P_TXT_X, TWO_P_TXT_Y );
				 
	/* draw Quit box */
	plot_v_line( (UINT8*)fbBase16,	/* Left Border */
				 QUIT_X_POS,
				 QUIT_Y_POS,
				 QUIT_Y_POS + BOX_HEIGHT );
	plot_h_line( (UINT32*)fbBase16,	/* Top Border */
				 QUIT_X_POS,
				 QUIT_X_POS + BOX_WIDTH,
				 QUIT_Y_POS );
	plot_v_line( (UINT8*)fbBase16,	/* Right Border */
				 QUIT_X_POS + BOX_WIDTH,
				 QUIT_Y_POS,
				 QUIT_Y_POS + BOX_HEIGHT );
	plot_h_line( (UINT32*)fbBase16,	/* Bottom Border */
				 QUIT_X_POS,
				 QUIT_X_POS + BOX_WIDTH,
				 QUIT_Y_POS + BOX_HEIGHT );
	
	render_String( (UINT8*)fbBase16,"Quit", 
				   QUIT_TXT_X, QUIT_TXT_Y );
}