#ifndef _FONT_C
#define _FONT_C

/* Includes */
#include "types.h"
#include "gm_cnsts.h"
#include <stdio.h>

/* 
	Font Character Map with default font for signaling an error with reading in
	the font. 
*/
const static UINT8 defaultBMP[BMP_HEIGHT] =
{
	0xFF,
	0xFF,
	0xdb,
	0xe7,
	0xe7,
	0x81,
	0xa5,
	0xa5,
	0x81,
	0x81,
	0x99,
	0x81,
	0x81,
	0xd5,
	0xff,
	0xff
};
static UINT8 charMap[MAX_CHAR][BMP_HEIGHT];
   
 /* Defines */
 #define MAX_ROWS 8
 #define MAX_COLS 16

/*
	Function to pull the font from a 128x128 bitmap.
*/
static void pull_font( const char* file_name )
{
	FILE* fPtr = fopen( file_name, "r" );
	UINT8 cCurrChar, byteRow = 0, charRow = 0, i, j;
	UINT8 cNxtChar;
	
	if( 0 != fPtr )
	{
		cNxtChar = fgetc( fPtr );
		while( !feof( fPtr ) )
		{
			charMap[cCurrChar][byteRow] = cNxtChar;
			
			if( byteRow >= MAX_COLS )
			{
				charRow = cCurrChar;
				byteRow = 0;
			}
			else if( cCurrChar == (charRow + MAX_COLS) )
			{
				cCurrChar = charRow;
				byteRow++;
			}
			else
				cCurrChar++;
				
			cNxtChar = fgetc( fPtr );
		}
		
		fclose( fPtr );
	}
	else
		for( i = 0; i < MAX_CHAR; i++ )
			for( j = 0; j < BMP_HEIGHT; j++ )
				charMap[ i ][ j ] = defaultBMP[ j ];
}

#endif