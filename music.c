/* Includes */
#include "music.h"
#include "psg.h"
#include "snd_cnst.h"  

struct mMainTheme
{
	Note *pCurrentSong;
	int iCurrentSongSize;
	UINT8 bTempo;
	int iCurrIndex;
	int iTickCount;
	char bCurrentVolume;
	bool bPaused;
	bool bEnvelope;
	char bEnvelopeVolumeDelta;
};

struct mMainTheme m_MainTheme =
{
	0, 0, 0, 0, 0, 0, false, 0, 0
};

/* For simplicity */
#define currentNote m_MainTheme.pCurrentSong[m_MainTheme.iCurrIndex]

/* private function declarations */
void set_note( const Note* m_NoteToSet );
void set_up_envelope( );

/* Sets up information for software Envelope 
	- There is only one type of envelope that goes from high
		to low on a per note basis
*/
void set_up_envelope( )
{
	if( m_MainTheme.bEnvelope )
	{
		m_MainTheme.bEnvelopeVolumeDelta = MAX_VOLUME / m_MainTheme.iTickCount; 
		m_MainTheme.bCurrentVolume = MAX_VOLUME;
		
		set_volume( MUSIC_CHANNEL, m_MainTheme.bCurrentVolume );
	}
}

/* Initialization of the Music.  Also used to reset
	the music to default. */
void start_music( Note nNewSong[], int iSize, bool useEnvelope )
{
	m_MainTheme.pCurrentSong		= nNewSong;
	m_MainTheme.iCurrentSongSize 	= iSize;
	m_MainTheme.iCurrIndex			= 0;
	m_MainTheme.bTempo 				= 0;
	m_MainTheme.iTickCount 			= currentNote.iDuration;
	m_MainTheme.bCurrentVolume 		= DEFAULT_VOLUME;
	m_MainTheme.bEnvelope			= useEnvelope;
	start_sound( );
	set_up_envelope( );
	set_note( &(currentNote) );
	enable_channel( MUSIC_CHANNEL, true, false );
	set_volume( MUSIC_CHANNEL, m_MainTheme.bCurrentVolume );
	m_MainTheme.bPaused 			= false;
}

/* Music Control Functions */
void play_music( )
{
	m_MainTheme.bPaused = false;
	enable_channel( MUSIC_CHANNEL, true, false );
	set_volume( MUSIC_CHANNEL, m_MainTheme.bCurrentVolume );
}

void pause_music( )
{
	m_MainTheme.bPaused = true;
	enable_channel( MUSIC_CHANNEL, false, false );
	set_volume( MUSIC_CHANNEL, 0 );
}

/* Will manipulate music volume if volume isn't set to use the envelope */
void adjust_music_volume( bool bUp )
{
	char bAdjustValue = 1;
	
	if( !bUp )
		bAdjustValue += ~bAdjustValue;
	
	if( (m_MainTheme.bCurrentVolume + bAdjustValue) >= 0 && 
		(m_MainTheme.bCurrentVolume + bAdjustValue) < MAX_VOLUME )
		m_MainTheme.bCurrentVolume += bAdjustValue;
	
	set_volume( MUSIC_CHANNEL, m_MainTheme.bCurrentVolume );
}
/* End Music Control Functions */

void update_music( UINT32 iTime_Elapsed )
{
	if( !m_MainTheme.bPaused )
	{
		m_MainTheme.iTickCount -= iTime_Elapsed;
		if( m_MainTheme.iTickCount <= 0 )
		{		
			m_MainTheme.iCurrIndex++;
			if( m_MainTheme.iCurrIndex >= m_MainTheme.iCurrentSongSize )
				m_MainTheme.iCurrIndex = 0;
			
			m_MainTheme.iTickCount = currentNote.iDuration - m_MainTheme.bTempo;
			
			set_up_envelope( );
			
			set_note( &(currentNote) );
		}
		else if( m_MainTheme.bEnvelope && m_MainTheme.bCurrentVolume != 0 )
		{
			m_MainTheme.bCurrentVolume -= m_MainTheme.bEnvelopeVolumeDelta;
			set_volume( MUSIC_CHANNEL, m_MainTheme.bCurrentVolume );
		}
	}
}

/* Set a specified note to play on the music channel. */
void set_note( const Note* m_NoteToSet )
{
	set_tone( MUSIC_CHANNEL, m_NoteToSet->wNote );
}

/* Function for adjusting the tempo of the song based on the
	current level in the game. */
void adjust_tempo( UINT8 bLevel )
{
	m_MainTheme.bTempo = bLevel >> TEMPO_SHIFT_DIVISOR;
}