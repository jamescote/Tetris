#ifndef _RENDERER_H
#define _RENDERER_H

#include "model.h"
#include "types.h"

/*
	Public use interface for Renderer Object.
*/
void render_Tetrimino( UINT16* fbBase16, 
					   const Tetrimino* m_TetriModel );
void render_Board( UINT16* fbBase16,
				   const Game_Board* m_Board );
void render_All( UINT16* fbBase16,
				 const Game_Model* m_GameModel );
void render_String( UINT8* fbBase8, 
					const char* sText, 
					UINT16 iXPxlPos, UINT16 iYPxlPos );
void clear_Screen( UINT16* fbBase16 );

#endif