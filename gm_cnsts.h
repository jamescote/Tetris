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
#define MAX_STR_LENGTH 255
#define TAB_WIDTH (BMP_WIDTH * 4)
#define BOARD_HEIGHT 20
#define BOARD_WIDTH	10
#define X_POS 0
#define Y_POS 1
#define MAX_POS 2
#define T_HEIGHT 4
#define T_WIDTH 4
#define D_RIGHT	0
#define D_LEFT 1

/* Double Buffering Defines */
#define BACK_BUFFER_SIZE 32256
#define BACK_BUFFER_ALIGNMENT 256
#define BACK_BUFFER 1
#define FRONT_BUFFER 0

/* Score Calculation Defines */
#define LEVEL_THRESHOLD 10
#define GRAVITY_DELTA 3

/* Renderer Defines */
#define LINE_CLEAR 0
#define LINE_DRAW 1
#define BACK_STATE 1
#define FRONT_STATE 0
#define RIGHT_BORDER_X (BOARD_DRAW_END_X + 1)
#define LEFT_BORDER_X (BOARD_DRAW_START_X - 1)
#define BOTTOM_BORDER_Y (BOARD_BOTTOM_LINE_Y + 1)

/* Next Tetrimino Area Boundaries */
#define NEXT_TET_LEFT_BORDER_X (BOARD_DRAW_END_X + NEXT_TET_BORDER_WIDTH)
#define NEXT_TET_RIGHT_BORDER_X (NEXT_TET_LEFT_BORDER_X + (MINO_SIZE << TETRI_MAP_BASE))
#define NEXT_TET_TOP_BORDER_Y 32
#define NEXT_TET_BOTTOM_BORDER_Y (NEXT_TET_TOP_BORDER_Y + (MINO_SIZE << TETRI_MAP_BASE))
#define NEXT_TET_BORDER_WIDTH 16 /* Needs to be word aligned. */

/* Status Output */
#define STATUS_SPACING 5
#define LABEL_NEXT_X_POS (NEXT_TET_LEFT_BORDER_X + MINO_SIZE)
#define LABEL_NEXT_Y_POS (NEXT_TET_TOP_BORDER_Y - BMP_HEIGHT)
#define LABEL_X_POS (BOARD_DRAW_END_X + BMP_WIDTH)
#define VALUE_X_POS (LABEL_X_POS + 120)
#define SCORE_Y_POS (NEXT_TET_BOTTOM_BORDER_Y + NEXT_TET_BORDER_WIDTH) + 1
#define LEVEL_Y_POS (SCORE_Y_POS + (BMP_HEIGHT + STATUS_SPACING))
#define LNS_CLRD_Y_POS (LEVEL_Y_POS + (BMP_HEIGHT + STATUS_SPACING))

/* Game Board States */
#define BOARD_RESET_STATE 0		
#define	BOARD_RUN_STATE 1		
#define BOARD_PAUSE_STATE 2
#define	BOARD_LOCK_STATE 3
#define BOARD_GAME_OVER_STATE 4
#define BOARD_LEVEL_UP 5

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
#define GARBAGE_INPUT_8 (unsigned char)-1
#define GARBAGE_INPUT_16 (unsigned int)-1
#define GARBAGE_INPUT_32 (unsigned long)-1

/* Board Bounds */
#define LEFT_BOUND 0x0200

#endif