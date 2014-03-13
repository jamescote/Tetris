/* Includes */
#include "renderer.h"
#include "model.h"
#include "types.h"
#include "font.c"
#include "gm_cnsts.h"
#include <osbind.h>
#include <linea.h>

/*void WToStr( char* cReturnString, UINT16 iValue );*/

int main( )
{

	Game_Model m_tst_Model =
	{
		{ {6,22}, {0x0050, 0x0028, 0x0050, 0x0028} },
		{ { 0x03ff,
		  0x02aa,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0 }, 
		0, 0, 0, 0, 0, 0, 0, 0 }
		
	};/*
	Tetrimino tst_piece = { {6,19}, {0x0050, 0x0028, 0x0050, 0x0028} };
	Game_Board tst_Board = 
	{ 
		{ 0x03ff,
		  0x02aa,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0 }, 
		0, 0, 0, 0, 0, 0, 0, 0 
	};
	Game_Board tst_Board2 = 
	{ 
		{ 0x02aa,
		  0x03ff,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0,
		  0x0 }, 
		0, 0, 0, 0, 0, 0, 0, 0 
	};*/
	                                                 
	UINT16* fbBase16 = Physbase( );
	char cBuffer[ MAX_STR_LENGTH ];
	int i;
	
	/*render_All( fbBase16, &m_tst_Model );
	
	/*render_Tetrimino( fbBase16, &tst_piece );
	
	for( i = 0; i < 5; i++ )
	{
		move_Down( &tst_piece, &tst_Board );
		render_Tetrimino( fbBase16, &tst_piece );
	}*/
	
	clear_Screen( fbBase16 );
	render_Board( fbBase16, &(m_tst_Model.cMainBoard) );
	
/*	for( i = 0; i < 20; i++ )
	{
		WToStr( cBuffer, i );
		render_String( (UINT8*)fbBase16, 
						cBuffer, 
						32, 16 + (i << 4) );
	}*/
	/*render_Board( fbBase16, &tst_Board );
	Vsync( );
	render_Board( fbBase16, &tst_Board2 );*/
}