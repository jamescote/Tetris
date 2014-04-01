#ifndef _MUSIC_H
#define _MUSIC_H

/* includes */
#include "types.h"
#include "gm_cnsts.h"

/* Note Structure */
/* Contains information about each note to play. */
typedef struct Note
{
	UINT16 wNote;
	int iDuration;
}Note;

/* public function declarations */
void start_music( Note nNewSong[], int iSize, bool useEnvelope );
void switch_music( Note nNewSong[], int iSize, bool useEnvelope );
void play_music( );
void pause_music( );
void adjust_music_volume( bool bUp );
void update_music( UINT32 iTime_Elapsed );
void adjust_tempo( UINT8 bLevel );

#endif