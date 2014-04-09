/*
	Name: songs.c
	Purpose: Contains Song information for playing with the 
			 music module.
	Written By: James Cote
*/
#ifndef _SONGS_C
#define _SONGS_C

#include "music.h"
#include "snd_cnst.h"

#define TETRIS_HIGH_RES_SIZE_INDEX 0

const int iSongSizes[ TOTAL_NUMBER_OF_SONGS ] =
{
	TETRIS_HIGH_RES_NOTE_COUNT
};

const Note TetrisTheme_HighRes[] =
{
	{ E, 		QUARTER },
	{ B, 		EIGHTH },
	{ C, 		EIGHTH },
	{ D, 		QUARTER },
	{ C, 		EIGHTH },
	{ B, 		EIGHTH },
	{ A_LOW, 	QUARTER },
	{ A_LOW, 	EIGHTH },
	{ C, 		EIGHTH },
	{ E, 		QUARTER },
	{ D, 		EIGHTH },
	{ C, 		EIGHTH },
	{ B, 		ONEFIVE },
	{ C, 		EIGHTH },
	{ D, 		QUARTER },
	{ E, 		QUARTER },
	{ C, 		QUARTER },
	{ A_LOW, 	QUARTER },
	{ A_LOW, 	EIGHTH },
	{ A_LOW, 	EIGHTH },
	{ B, 		EIGHTH },
	{ C, 		EIGHTH },
	{ D, 		ONEFIVE },
	{ F, 		EIGHTH },
	{ A, 		QUARTER },
	{ G, 		EIGHTH },
	{ F, 		EIGHTH },
	{ E, 		ONEFIVE },
	{ C, 		EIGHTH },
	{ E, 		QUARTER },
	{ D, 		EIGHTH },
	{ C, 		EIGHTH },
	{ B, 		QUARTER },
	{ B, 		EIGHTH },
	{ C, 		EIGHTH },
	{ D, 		QUARTER },
	{ E, 		QUARTER },
	{ C, 		QUARTER },
	{ A_LOW, 	QUARTER },
	{ A_LOW, 	QUARTER },
	{ REST, 	QUARTER },
	/* Repeat */
	{ E, 		QUARTER },
	{ B, 		EIGHTH },
	{ C, 		EIGHTH },
	{ D, 		QUARTER },
	{ C, 		EIGHTH },
	{ B, 		EIGHTH },
	{ A_LOW, 	QUARTER },
	{ A_LOW, 	EIGHTH },
	{ C, 		EIGHTH },
	{ E, 		QUARTER },
	{ D, 		EIGHTH },
	{ C, 		EIGHTH },
	{ B, 		ONEFIVE },
	{ C, 		EIGHTH },
	{ D, 		QUARTER },
	{ E, 		QUARTER },
	{ C, 		QUARTER },
	{ A_LOW, 	QUARTER },
	{ A_LOW, 	EIGHTH },
	{ A_LOW, 	EIGHTH },
	{ B, 		EIGHTH },
	{ C, 		EIGHTH },
	{ D, 		ONEFIVE },
	{ F, 		EIGHTH },
	{ A, 		QUARTER },
	{ G, 		EIGHTH },
	{ F, 		EIGHTH },
	{ E, 		ONEFIVE },
	{ C, 		EIGHTH },
	{ E, 		QUARTER },
	{ D, 		EIGHTH },
	{ C, 		EIGHTH },
	{ B, 		QUARTER },
	{ B, 		EIGHTH },
	{ C, 		EIGHTH },
	{ D, 		QUARTER },
	{ E, 		QUARTER },
	{ C, 		QUARTER },
	{ A_LOW, 	QUARTER },
	{ A_LOW, 	QUARTER },
	{ REST, 	QUARTER },
	/* Break */
	{ E, 		DOUBLE_QUARTER },
	{ C, 		DOUBLE_QUARTER },
	{ D, 		DOUBLE_QUARTER },
	{ B, 		DOUBLE_QUARTER },
	{ C, 		DOUBLE_QUARTER },
	{ A, 		DOUBLE_QUARTER },
	{ G_SHARP, 	DOUBLE_QUARTER },
	{ B, 		QUARTER },
	{ REST, 	QUARTER },
	{ E, 		DOUBLE_QUARTER },
	{ C, 		DOUBLE_QUARTER },
	{ D, 		DOUBLE_QUARTER },
	{ B, 		DOUBLE_QUARTER },
	{ C, 		QUARTER },
	{ E, 		QUARTER },
	{ D_HIGH, 	DOUBLE_QUARTER },
	{ G_SHARP, 	DOUBLE_QUARTER },
	{ REST, 	DOUBLE_QUARTER }
};


#endif