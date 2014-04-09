/*
	Name: mnu_cns.h
	Purpose: contains Defines for drawing the menu on screen
	Written By: James Cote
*/
#ifndef _MENU_CONSTANTS_H
#define _MENU_CONSTANTS_H

/* Struct that contains the position of a Menu Mino */
typedef struct menu_Mino
{
	UINT16 wXPos, wYPos;
}stMenuMino;

/* Menu Render Defines */
#define TITLE_X_POS 	296
#define TITLE_Y_POS 	96
#define ONE_P_X_POS 	79
#define ONE_P_Y_POS		150
#define ONE_P_TXT_X		ONE_P_X_POS + ((BOX_WIDTH >> 1) - 33) + 1
#define ONE_P_TXT_Y		ONE_P_Y_POS + ((BOX_HEIGHT >> 1) - 4)
#define ONE_P_MINO_Y	ONE_P_Y_POS + 1
#define ONE_P_MINO_X1	ONE_P_X_POS + 1
#define ONE_P_MINO_X2	ONE_P_X_POS + BOX_WIDTH - 17
#define TWO_P_X_POS		399
#define TWO_P_Y_POS		ONE_P_Y_POS
#define TWO_P_TXT_X		TWO_P_X_POS + ((BOX_WIDTH >> 1) - 33) + 1
#define TWO_P_TXT_Y		TWO_P_Y_POS + ((BOX_HEIGHT >> 1) - 4)
#define TWO_P_MINO_Y	TWO_P_Y_POS + 1
#define TWO_P_MINO_X1	TWO_P_X_POS + 1
#define TWO_P_MINO_X2	TWO_P_X_POS + BOX_WIDTH - 17
#define QUIT_X_POS		239
#define QUIT_Y_POS		ONE_P_Y_POS + 50
#define QUIT_TXT_X		QUIT_X_POS + ((BOX_WIDTH >> 1) - 17) + 1
#define QUIT_TXT_Y		QUIT_Y_POS + ((BOX_HEIGHT >> 1) - 4)
#define QUIT_MINO_Y		QUIT_Y_POS + 1
#define QUIT_MINO_X1	QUIT_X_POS + 1
#define QUIT_MINO_X2	QUIT_X_POS + BOX_WIDTH - 17
#define BOX_WIDTH		162
#define BOX_HEIGHT		40
#define MAX_MENU_MINOS 	40

/* Menu State Defines */
#define QUIT_GAME	0
#define ONE_PLAYER 	1
#define TWO_PLAYER 	2
#define RUN_MENU	3

/* Menu Input Defines */
#define ONE_PLAYER_KEY 	0x02
#define TWO_PLAYER_KEY 	0x03
#define UP_ARROW		0x48
#define ENTER			0x1C
#define MENU_KEY_COUNT	4

#endif