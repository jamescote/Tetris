#ifndef _RENDERER_H
#define _RENDERER_H

#include "model.h"
#include "types.h"

/*
	Public use interface for Renderer Object.
*/
void render_tetrimino( UINT16* fbBase16, 
					   const struct Tetrimino* m_TetriModel );
void render_board( UINT16* fbBase16,
				   const struct Game_Board* m_Board );
void render_all( UINT16* fbBase16,
				 const struct Model* m_GameModel );
void render_string( UINT8* fbBase8, 
					const char* sText, 
					UINT16 iXPxlPos, UINT16 iYPxlPos );

#endif