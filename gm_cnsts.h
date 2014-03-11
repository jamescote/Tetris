#ifndef _GAME_CNSTS_
#define _GAME_CNSTS_

/* Game Specific Values */
#define BOARD_DRAW_START_X 240
#define BOARD_DRAW_END_X 400
#define BOARD_BOTTOM_LINE_Y 320
#define RIGHT_ALIGNED_X_POS 3
#define TETRI_MAP_BASE 2
#define MINO_SHIFT_SIZE 4
#define MINO_SIZE 16
#define BMP_HEIGHT 16
#define BMP_WIDTH 8
#define SCREEN_HEIGHT 400
#define SCREEN_WIDTH 640
#define MAX_CHAR 128
#define TETRI_START_POS_X 5
#define TETRI_START_POS_Y 22
#define MAX_STR_LENGTH 512
#define TAB_WIDTH (BMP_WIDTH * 4)
#define BOARD_HEIGHT 20
#define BOARD_WIDTH	10
#define X_POS 0
#define Y_POS 1
#define MAX_POS 2
#define T_HEIGHT 4
#define D_RIGHT	0
#define D_LEFT 1
#define GRAVITY_DELTA 5

/* Renderer Defines */
#define LINE_CLEAR 1
#define LINE_DRAW 0
#define RIGHT_BORDER_X (BOARD_DRAW_END_X + 1)
#define LEFT_BORDER_X (BOARD_DRAW_START_X - 1)
#define BOTTOM_BORDER_Y (BOARD_BOTTOM_LINE_Y + 1)


/* Game Board States */
#define BOARD_RESET_STATE 0		
#define	BOARD_RUN_STATE 1		
#define BOARD_PAUSE_STATE 2
#define	BOARD_LOCK_STATE 3
#define BOARD_GAME_OVER_STATE 4

/* Main Game States */
#define GAME_EXIT 0
#define GAME_PAUSED 1
#define GAME_START 2
#define GAME_RUN 3

/* Input */
#define MOVE_LEFT 		0x4b
#define MOVE_RIGHT 		0x4d
#define MOVE_DOWN		0x50
#define ROTATE_RIGHT	0x48 /*|| 0x2d*/
#define ROTATE_LEFT		0x2c
#define PAUSE			0x01 /*|| 0x3b || 0x19*/
#define TEMP_QUIT		0x10

/* Text Positions */
#define SCORE_POS_X 416
#define SCORE_POS_Y 208

#define MAX_TETRIMINOS 7

/* Scoring Defines */
#define PTS_TETRIS 50
#define PTS_LINE 10
#define TTRS_CNST 4
#define FULL_LINE 0x3ff

/* Boolean Defines */
#define bool UINT8
#define true 1
#define false 0

/* Bitwise Defines */
#define WORD_LENGTH 16
#define	FULL_BYTE_MASK 0xff

/* Board Bounds */
#define LEFT_BOUND 0x0200

#endif