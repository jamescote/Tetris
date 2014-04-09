/*
	Name: psg.c
	Purpose: Contains function implementations for dealing
			 with the atari psg sound chip.
	Written by: James Cote
*/

/* Includes */
#include "psg.h"
#include <osbind.h>

/* private Defines */
#define A_FINE_REG 		0x0
#define A_COURSE_REG 	0x1
#define B_FINE_REG 		0x2
#define B_COURSE_REG 	0x3
#define C_FINE_REG 		0x4
#define C_COURSE_REG 	0x5
#define NOISE_FREQ_REG 	0x6
#define PORT_MIXER_REG 	0x7
#define A_LEVEL_REG 	0x8
#define B_LEVEL_REG		0x9
#define C_LEVEL_REG		0xA
#define FINE_ENV_FREQ	0xB
#define ROUGH_ENV_FREQ	0xC
#define SHAPE_ENV		0xD
#define	PORT_A_DATA		0xE
#define	PORT_B_DATA		0xF

#define ENVELOPE_MASK 0x0F
#define SIZE_OF_BYTE 8
#define NOISE_TUNING_MASK 0x1F
#define MAX_REG			0x10
#define MIN_REG			0x0
#define MAX_CHANNELS	3
#define MIN_CHANNELS	0
#define COURSE_MASK		0xF
#define FINE_SIZE		8
#define VOLUME_MASK		0xe0
#define NOISE_OFFSET	3
#define STOP_SOUND_MASK 0x3f

/* Helper function declarations */
bool isValidChannel( int iChannel );
bool isValidRegister( int reg );
UINT8 combineMasks( UINT8 bLHS, UINT8 bRHS, bool bAND );
void write_psg( int reg, UINT8 val );
UINT8 read_psg( int reg );
int set_ipl( int mask );

/* Constant pointers to the PSG */
UINT8* const cpRegSelect = 0xFFFF8800;
UINT8* const cpWriteReg = 0xFFFF8802;
bool bSoundOff = true;

/* Defines */
#define VBL_MASK 6
						
/* write a value (0-255) into the specified register */						
void write_psg( int reg, UINT8 val )
{
	long old_ssp;
	int old_ipl;
	
	if( !bSoundOff && isValidRegister( reg ) )
	{
		old_ssp = Super(1);
		if( old_ssp == USER_MODE )
			old_ssp = Super( 0 );
			
		old_ipl = set_ipl( VBL_MASK );
			
		*cpRegSelect = reg;
		*cpWriteReg = val;
		
		set_ipl( old_ipl );
		
		if( old_ssp != SUPER_MODE )
			Super(old_ssp);
	}
}

/* read the value from a specified register */
UINT8 read_psg( int reg )
{
	long old_ssp;
	int old_ipl;
	UINT8 iReadValue = 0;
	UINT8* const cpReadReg = cpRegSelect;
	
	if( !bSoundOff && isValidRegister( reg ) )
	{
		old_ssp = Super(1);
		if( old_ssp == USER_MODE )
			old_ssp = Super( 0 );
			
		old_ipl = set_ipl( VBL_MASK );
		
		*cpRegSelect = reg;
		iReadValue = *cpReadReg;
		
		set_ipl( old_ipl );
		
		if( old_ssp != SUPER_MODE )
			Super(old_ssp);
	}
	
	return iReadValue;
}

/* Sets the fine and course tone for a given channel. */
void set_tone( int iChannel, int iTuning )
{
	int iRegisterSelect;
	
	if( isValidChannel( iChannel ) )
	{
		iRegisterSelect = (iChannel << 1);
		write_psg( iRegisterSelect, (char)iTuning );
		iRegisterSelect++;
		write_psg( iRegisterSelect, ((iTuning >> FINE_SIZE) & COURSE_MASK) );
	}
}

/* Sets the envelope to a given envelope value */
void set_envelope( UINT16 wEnvFrequency, UINT8 bEnvelope )
{
	bEnvelope &= ENVELOPE_MASK;
	
	write_psg( FINE_ENV_FREQ, (UINT8)wEnvFrequency );
	write_psg( ROUGH_ENV_FREQ, (UINT8)(wEnvFrequency >> SIZE_OF_BYTE) );
	
	write_psg( SHAPE_ENV, bEnvelope );
}

/* Sets the Noise frequency to a specified tuning */
void set_noise( int iTuning )
{
	iTuning &= NOISE_TUNING_MASK;
	
	write_psg( NOISE_FREQ_REG, (UINT8)iTuning );
}

/* Sets the volume of a given channel to the specified 
	volume (xxxM3210) */
void set_volume( int iChannel, int iVolume )
{
	UINT8 bVolumeRegister = A_LEVEL_REG;
	
	if( isValidChannel( iChannel ) && !(iVolume & VOLUME_MASK) )
	{
		bVolumeRegister += iChannel;
		write_psg( bVolumeRegister, iVolume );
	}
}

/* sets a channel with specified flags in the mixer */
void enable_channel( int iChannel, bool bTone_On, bool bNoise_On )
{
	UINT8 bMask = 1;
	UINT8 bRegisterValue;
	
	bTone_On = bTone_On ? true : false;
	bNoise_On = bNoise_On ? true : false;
	
	if( isValidChannel( iChannel ) )
	{
		bRegisterValue = read_psg( PORT_MIXER_REG );
		bRegisterValue = combineMasks( bRegisterValue, 
								       (bMask << iChannel), 
									   bTone_On );
		bRegisterValue = combineMasks( bRegisterValue, 
									   (bMask << (iChannel + NOISE_OFFSET)), 
									   bNoise_On );
		write_psg( PORT_MIXER_REG, bRegisterValue );
	}
}

/*
	Turn on the sound.
*/
void start_sound( )
{
	bSoundOff = false;
}

/* 
	Turns off all channels and sets their volumes to
	0.
*/
void stop_sound( )
{
	UINT8 bRegisterValue = read_psg( PORT_MIXER_REG );
	UINT8 i = CHANNEL_A;
	
	write_psg( PORT_MIXER_REG, (bRegisterValue | STOP_SOUND_MASK) );
	
	bSoundOff = true;
	
	for( i; i < MAX_CHANNELS; i++ )
		set_volume( i, 0 );
}

/* Helper functions */
bool isValidChannel( int iChannel )
{
	return iChannel < MAX_CHANNELS && iChannel >= MIN_CHANNELS;
}

bool isValidRegister( int reg )
{
	return reg < MAX_REG && reg >= MIN_REG;
}

/* Takes two values and either ors or ands them together
	based on the bAND flag.  Anding will also flip the right
	hand side. */
UINT8 combineMasks( UINT8 bLHS, UINT8 bRHS, bool bAND )
{	
	if( bAND )
		bLHS &= ~bRHS;
	else
		bLHS |= bRHS;
		
	return bLHS;
}