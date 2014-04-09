/*
	Header declaration for the event handler system.
	Written by: James Cote
*/
#ifndef _TET_EVENT_HANDLER_H
#define _TET_EVENT_HANDLER_H

/* Includes */
#include "types.h"
#include "model.h"

/* Public Function Declarations */
UINT8 handleMenuAsync( const UINT8 cTrigger, char* bCurrentSelection );
void handleAsync( const UINT8 cTrigger, Game_Model* m_MainGameModel, bool bPaused );
void handleSync( Game_Model* m_MainGameModel, UINT32 lTimeElapsed );
void handleRenderSync( UINT16* fbBase16, Game_Model* m_MainGameModel );
void getHotKeys( UINT8 bKeys[], UINT8 *bCount );

#endif