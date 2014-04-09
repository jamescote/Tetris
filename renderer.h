/*
	Name: Renderer.h
	Purpose: Contains interface for using the renderer module.
	Written by: James Cote
*/
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
				 const Game_Model* m_GameModel,
				 int iFPS );
void render_String( UINT8* fbBase8, 
					const char* sText, 
					UINT16 iXPxlPos, UINT16 iYPxlPos );
void flag_FPS_Rend( );
void flag_FPS_Clear( );
void render_Static( UINT16* fbBase16 );
void clear_Screen( UINT16* fbBase16 );
void draw_Menu( UINT16* fbBase16, UINT8 Draw_Selection );

#endif