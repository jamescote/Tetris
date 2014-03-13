/* Includes */
#include <osbind.h>
#include <unistd.h>
#include "types.h"
#include "font.c"
#include "raster.h"

/* Defines */

const UINT8 tempBitMap[ 16 ] = 
{
	0x00,
	0x00,
	0x3c,
	0x7c,
	0xfc,
	0x1c,
	0x1c,
	0x1c,
	0x1c,
	0x1c,
	0x1c,
	0x1c,
	0x1c,
	0x7e,
	0x7e,
	0x00
};
const UINT16 tempBitMap16[ 16 ] = 
{
	0x0000,
	0x7ffe,
	0x7ffe,
	0x7ffe,
	0x7ffe,
	0x7ffe,
	0x7ffe,
	0x7e7e,
	0x7e7e,
	0x7ffe,
	0x7ffe,
	0x7ffe,
	0x7ffe,
	0x7ffe,
	0x7ffe,
	0x0000
};


/* Main */
int main( )
{
	UINT8*  fbBase8	 = Physbase( );
	UINT16* fbBase16 = Physbase( );
	UINT32* fbBase32 = Physbase( );
	int x1, x2, y;
	float fX;
	int bDraw = 1;
	
	/* Clear full screen */
	clear_region( fbBase16, 0, 639, 0, 399 );
	plot_v_line( fbBase8, 320, 0, 399 );
	plot_h_line( fbBase32, 0, 639, 200 );
	
	/* Quad Section Test */
	/* H Triangle Test */
	plot_h_line( fbBase32, 320, 639, 0 );
	plot_h_line( fbBase32, 320, 639, 199 );
	fX = 321.0f;
	for( y = 1; y < 200; y++ )
	{
		x1 = (int)fX;
		if( (y & 7) != 0 )
		{
			plot_h_line( fbBase32, x1, x1 + 6, y );
			plot_h_line( fbBase32, 321 + (639 - x1), (321 + (639 - x1)) - 6, y );
		}
		else
			plot_h_line( fbBase32, x1, (321 + (639-x1)), y );
		fX += 1.6f;
	}
	
	/* V Triangle Test */
	plot_v_line( fbBase8, 0, 200, 399 );
	plot_v_line( fbBase8, 319, 200, 399 );
	fX = 201.0f;
	for( x1 = 0; x1 < 320; x1++ )
	{
		y = (int)fX;
		if( (x1 & 7) != 0 )
		{
			plot_v_line( fbBase8, x1, y, (y + 4) );
			plot_v_line( fbBase8, x1, 201 + (400 - y), (201 + (400 - y)) - 4 );
		}
		else
			plot_v_line( fbBase8, x1, y, (201 + (400 - y)) );
		fX += 0.625f;
	}
	
	/* Bitmap Test */
	for( y = 200; y <= 384; y += 16 )
		for( x1 = 320; x1 <= 624; x1 += 16 )
			draw_bitmap_16( fbBase16, x1, y, tempBitMap16, 16 );
			
	/* CheckerBoard Test */
	for( x2 = 0; x2 < 8; x2++ )
	{
		for( x1 = 0; x1 <= 304; x1 += 16 )
		{
			for( y = 0; y <= 184; y += 16 )
			{
				if( bDraw )
				{
					draw_square( fbBase16, x1, y );
					bDraw = !bDraw;
				}
				else
				{	
					clear_region( fbBase16, x1, x1 + 16, y, y + 16 );
					bDraw = !bDraw;
				}
			}
			bDraw = !bDraw;
		}
		
		bDraw = 1;
		for( x1 = 0; x1 <= 304; x1 += 16 )
		{
			for( y = 0; y <= 184; y += 16 )
			{
				if( bDraw )
				{
					clear_region( fbBase16, x1, x1 + 16, y, y + 16 );
					bDraw = !bDraw;
				}
				else
				{
					draw_square( fbBase16, x1, y );
					bDraw = !bDraw;
				}
			}
			bDraw = !bDraw;
		}
	}
	
	/* Character Map Test
	x1 = 0;
	y = 0;
	for( x2 = 0; x2 < 128; x2++ )
	{
		draw_bitmap_8( fbBase8, x1, y, charMap[x2], 16 );
		x1 += 8;
		
		if( x1 >= 128 )
		{
			x1 = 0;
			y += 16;
		}
	}*/
	
	/* Bitmap Clip Test 
	x1 = -8;
	x2 = 640;
	y = 400;
	while( x1 < 32 && x2 > 608 && y > 250 )
	{
		draw_bitmap_8( fbBase8, x1, y, tempBitMap, 16 );
		draw_bitmap_8( fbBase8, x2, y, tempBitMap, 16 );
		sleep( 1 );
		clear_region( fbBase16, -8, x1 + 8, y, 416 );
		clear_region( fbBase16, 640, x2, y, 416 );
		x1++;
		x2--;
		y--;
	}*/
	
	return 0;
}
