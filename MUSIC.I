# 1 "music.c" 1
# 1 "./music.h" 1
# 1 "./types.h" 1
# 9 "./types.h"
typedef unsigned char UINT8;
typedef unsigned int  UINT16;
typedef unsigned long UINT32;


typedef UINT8 bool;
# 5 "./music.h" 2
# 1 "./gm_cnsts.h" 1
# 6 "./music.h" 2




typedef struct Note
{
	UINT16 wNote;
	int iDuration;
}Note;


void start_music( Note nNewSong[], int iSize, bool useEnvelope );
void switch_music( Note nNewSong[], int iSize, bool useEnvelope );
void play_music( );
void pause_music( );
void adjust_music_volume( bool bUp );
void update_music( UINT32 iTime_Elapsed );
void adjust_tempo( UINT8 bLevel );
# 2 "music.c" 2
# 1 "./psg.h" 1
# 1 "./types.h" 1
# 5 "./psg.h" 2
# 1 "./gm_cnsts.h" 1
# 6 "./psg.h" 2
# 14 "./psg.h"
void set_tone( int iChannel, int iTuning );
void set_volume( int iChannel, int iVolume );
void enable_channel( int iChannel, bool bTone_On, bool bNoise_On );
void set_envelope( UINT16 wEnvFrequency, UINT8 bEnvelope );
void set_noise( int iTuning );
void stop_sound( );
# 3 "music.c" 2
# 1 "./snd_cnst.h" 1
# 4 "music.c" 2


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

struct mMainTheme m_MainTheme;





void set_note( const Note* m_NoteToSet );
void set_up_theme( Note nNewSong[], int iSize, bool useEnvelope, bool bStart );
void set_up_envelope( );


void set_up_theme( Note nNewSong[], int iSize, bool useEnvelope, bool bStart )
{
	m_MainTheme.pCurrentSong		= nNewSong;
	m_MainTheme.iCurrentSongSize 	= iSize;
	if( bStart || m_MainTheme.iCurrIndex >= iSize )
		m_MainTheme.iCurrIndex	= 0;
	if( bStart )
		m_MainTheme.bTempo 		= 0;
	m_MainTheme.iTickCount 			= m_MainTheme.pCurrentSong[m_MainTheme.iCurrIndex] .iDuration;
	m_MainTheme.bCurrentVolume 		= 0x0a ;
	m_MainTheme.bEnvelope			= useEnvelope;
	set_up_envelope( );
	set_note( &(m_MainTheme.pCurrentSong[m_MainTheme.iCurrIndex] ) );
	enable_channel( 0 , 1 , 0  );
	set_volume( 0 , m_MainTheme.bCurrentVolume );
}





void set_up_envelope( )
{
	if( m_MainTheme.bEnvelope )
	{
		m_MainTheme.bEnvelopeVolumeDelta = 0x0F  / m_MainTheme.iTickCount;
		m_MainTheme.bCurrentVolume = 0x0F ;

		set_volume( 0 , m_MainTheme.bCurrentVolume );
	}
}



void start_music( Note nNewSong[], int iSize, bool useEnvelope )
{
	set_up_theme( nNewSong, iSize, useEnvelope, 1  );
}


void switch_music( Note nNewSong[], int iSize, bool useEnvelope )
{
	set_up_theme( nNewSong, iSize, useEnvelope, 0  );
}


void play_music( )
{
	m_MainTheme.bPaused = 0 ;
	enable_channel( 0 , 1 , 0  );
	set_volume( 0 , m_MainTheme.bCurrentVolume );
}

void pause_music( )
{
	m_MainTheme.bPaused = 1 ;
	enable_channel( 0 , 0 , 0  );
	set_volume( 0 , 0 );
}


void adjust_music_volume( bool bUp )
{
	char bAdjustValue = 1;

	if( !bUp )
		bAdjustValue += ~bAdjustValue;

	if( (m_MainTheme.bCurrentVolume + bAdjustValue) >= 0 &&
		(m_MainTheme.bCurrentVolume + bAdjustValue) < 0x0F  )
		m_MainTheme.bCurrentVolume += bAdjustValue;

	set_volume( 0 , m_MainTheme.bCurrentVolume );
}


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

			m_MainTheme.iTickCount = m_MainTheme.pCurrentSong[m_MainTheme.iCurrIndex] .iDuration - m_MainTheme.bTempo;

			set_up_envelope( );

			set_note( &(m_MainTheme.pCurrentSong[m_MainTheme.iCurrIndex] ) );
		}
		else if( m_MainTheme.bEnvelope && m_MainTheme.bCurrentVolume != 0 )
		{
			m_MainTheme.bCurrentVolume -= m_MainTheme.bEnvelopeVolumeDelta;
			set_volume( 0 , m_MainTheme.bCurrentVolume );
		}
	}
}


void set_note( const Note* m_NoteToSet )
{
	set_tone( 0 , m_NoteToSet->wNote );
}



void adjust_tempo( UINT8 bLevel )
{
	m_MainTheme.bTempo = bLevel >> 2 ;
}
