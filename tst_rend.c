/* Includes */
#include "renderer.h"
#include "model.h"
#include "types.h"
#include "font.c"
#include "gm_cnsts.h"
#include <osbind.h>
#include <linea.h>


int main( )
{
	struct Tetrimino tst_piece = { {6,19}, {0x0050, 0x0028, 0x0050, 0x0028} };
	struct Game_Board tst_Board = 
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
		0, 0, 0, 0, 0, 0 
	};
	struct Game_Board tst_Board2 = 
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
		0, 0, 0, 0, 0, 0 
	};
	                                                 
	UINT16* fbBase16 = Physbase( );
	int i;
	
	render_string( (UINT8*)fbBase16, "Testing...\nJump.\tTwo.", SCORE_POS_X, SCORE_POS_Y );
	
	render_tetrimino( fbBase16, &tst_piece );
	
	for( i = 0; i < 5; i++ )
	{
		move_Down( &tst_piece, &tst_Board );
		render_tetrimino( fbBase16, &tst_piece );
	}
	render_board( fbBase16, &tst_Board );
	Vsync( );
	render_board( fbBase16, &tst_Board2 );
	return 0;
}