#include "raster.h"

/* Constant Definitions */
#define XSCREENSIZE 640
#define YSCREENSIZE 400
#define SIZE_BYTE 8
#define SIZE_WORD 16
#define SIZE_LONG 32
#define LONG_SCREEN_WIDTH 20
#define WORD_SCREEN_WIDTH 40
#define BYTE_SCREEN_WIDTH 80


/*
	Purpose: Clear a rectangular region of the frame buffer.
	Note: Function truncates the region to be within bounds of
			the frame buffer.
*/
void clear_region( UINT16* fbBase,
		   int x1, int x2,
		   int y1, int y2 )
{
	UINT16 *fbStartX, *fbFinishX;
	int xStart			= x1 < x2 ? x1 : x2;
	int xFinish			= x1 < x2 ? x2 : x1;
	int yStart			= y1 < y2 ? y1 : y2;
	int yFinish			= y1 < y2 ? y2 : y1;
	int xStMask, xFinMask;		
	if( ( xStart < XSCREENSIZE && xFinish >= 0 ) &&	
		( yStart < YSCREENSIZE && yFinish >= 0 ) )
	{
		if( xStart < 0 )
			xStart = 0;
		if( xFinish >= XSCREENSIZE )
			xFinish = XSCREENSIZE - 1;
		if( yStart < 0 )
			yStart = 0;
		if( yFinish >= YSCREENSIZE )
			yFinish = YSCREENSIZE - 1;
		
		xStMask 	= ~((UINT16)-1 >> (xStart & (SIZE_WORD - 1)));
		xFinMask	= ~((UINT16)-1 << ((SIZE_WORD - 1) - (xFinish & (SIZE_WORD - 1))));
		fbStartX 	= fbBase + (yStart * WORD_SCREEN_WIDTH);
		fbFinishX 	= fbStartX + (xFinish >> 4);
		fbStartX	+= xStart >> 4;
		
		while( yStart <= yFinish )
		{
			if( fbStartX == fbFinishX )
				*fbStartX &= xStMask | xFinMask;
			else
			{
				*fbStartX 	&= xStMask;
				fbBase		= fbStartX + 1;
				while( fbBase != fbFinishX )
					*(fbBase++) = (UINT16)0;
				*fbFinishX &= xFinMask;
			}
			
			fbStartX 	+= WORD_SCREEN_WIDTH;
			fbFinishX	+= WORD_SCREEN_WIDTH;
			yStart++;
		}
	}
}

/*
	Purpose: Draw a 16x16 pixel square that's filled in.
	Note: Doesn't draw anything if x is not word aligned.
*/
void draw_square( UINT16* fbBase,
		  int x, int y )
{
	UINT8  i;
	
	if( ( x >= 0 && x <= XSCREENSIZE - SIZE_WORD ) &&
	    ( y >= 0 && y <= YSCREENSIZE - SIZE_WORD ) &&
		( (x & SIZE_WORD - 1) == 0 ) )
	{
		fbBase += (y * WORD_SCREEN_WIDTH) + (x >> 4);
		for( i = 0; i < SIZE_WORD; i++ )
		{
			*(fbBase) = (UINT16)-1;
			fbBase += WORD_SCREEN_WIDTH;
		}
	}
}

/*
	Purpose: Draw a horizontal line to the frame buffer.
*/
void plot_h_line( UINT32* fbBase,
		  int x1, int x2, int y )
{
	int xStart 			= x1 > x2 ? x2 : x1;
	int xFinish 		= xStart == x1 ? x2 : x1;
	UINT8 bLongMask		= SIZE_LONG - 1;
	UINT32* fbStart		= fbBase + (y * LONG_SCREEN_WIDTH);
	UINT32* fbFin 		= fbStart + (xFinish >> 5);
	fbStart 			+= (xStart >> 5);
	
	if( fbStart == fbFin )
		*fbStart |= ((UINT32)-1 >> (xStart & bLongMask) & 
					(UINT32)-1 << (bLongMask - (xFinish & bLongMask)));
	else
	{
		*(fbStart++) |= (UINT32)-1 >> (xStart & bLongMask);
		while( fbStart != fbFin )
			*(fbStart++) = (UINT32)-1;
		*fbStart |= (UINT32)-1 << (bLongMask - (xFinish & bLongMask));
	}
	
}

/*
	Purpose: Draw a vertical line to the frame buffer.
*/
void plot_v_line( UINT8* fbBase,
		  int x, int y1, int y2 )
{
	int yStart 	= y1 < y2 ? y1 : y2;
	int yFinish	= y1 < y2 ? y2 : y1;
	UINT8 cOut	= (UINT8)1;
	
	if( yStart < 0 )
		yStart = 0;
	if( yFinish >= YSCREENSIZE )
		yFinish = (YSCREENSIZE - 1);
	
	fbBase += (yStart * BYTE_SCREEN_WIDTH) + (x >> 3);
	cOut <<= (SIZE_BYTE - 1) - (x & (SIZE_BYTE - 1));
	
	while( yStart <= yFinish )
	{
		*(fbBase) |= cOut;
		fbBase += BYTE_SCREEN_WIDTH;
		yStart++;
	}
}

/*
	Purpose: Draw a byte aligned bitmap.
	TODO: Remove Clipping functionality.
*/
void draw_bitmap_8( UINT8* fbBase,
		     int x, int y,
		     const UINT8* pBitMap,
		     unsigned int iHeight )
{
	int iOffSet = (x & (SIZE_BYTE - 1));
	char iDrawSides = 0;
	UINT8* pEndOfBitmap = pBitMap;
	
	if( x > -SIZE_BYTE && x < XSCREENSIZE )
	{
		fbBase 	+= (y * BYTE_SCREEN_WIDTH) + (x >> 3);
		if( x < 0 )
			iDrawSides++;
		else if( XSCREENSIZE - x > (XSCREENSIZE - SIZE_BYTE) )
			iDrawSides--;
		
		pBitMap 	 -= y < 0 ? y : 0;
		y += iHeight;
		pEndOfBitmap += iHeight - (y >= YSCREENSIZE ? (y - YSCREENSIZE) : 0);
			
		while( pBitMap < pEndOfBitmap )
		{
			if( iDrawSides <= 0 )
				*fbBase |= (*pBitMap >> iOffSet);
			if( iDrawSides >= 0 )
				*(fbBase + 1) |= (*pBitMap << ((SIZE_BYTE - 1) - iOffSet));
			pBitMap++;
			fbBase += BYTE_SCREEN_WIDTH;
		}
	}
}
