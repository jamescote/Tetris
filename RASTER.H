#ifndef RASTER_H
#define RASTER_H

#include "types.h"

void clear_region( UINT16* fbBase,
		   int x1, int x2,
		   int y1, int y2 );
void plot_h_line( UINT32* fbBase,
		  int x1, int x2, int y );
void plot_v_line( UINT8* fbBase,
		  int x, int y1, int y2 );
void draw_bitmap_8( UINT8* fbBase,
		   int x, int y,
		   const UINT8* pBitMap,
		   unsigned int iHeight );
void draw_bitmap_16( UINT16* fbBase16,
		   int x, int y,
		   const UINT16* pBitMap,
		   unsigned int iHeight );
UINT16* get_video_base( );
void set_video_base( UINT16* wVBase );

#endif
