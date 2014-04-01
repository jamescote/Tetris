#ifndef _PSG_H
#define _PSG_H

/* Includes */
#include "types.h"
#include "gm_cnsts.h"

/* Sound Defines */
#define CHANNEL_A		0
#define CHANNEL_B		1
#define CHANNEL_C		2

/* Public Function Declarations */
void set_tone( int iChannel, int iTuning );
void set_volume( int iChannel, int iVolume );
void enable_channel( int iChannel, bool bTone_On, bool bNoise_On );
void set_envelope( UINT16 wEnvFrequency, UINT8 bEnvelope );
void set_noise( int iTuning );
void stop_sound( );

#endif