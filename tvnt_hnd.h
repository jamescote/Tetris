#ifndef _TET_EVENT_HANDLER_H
#define _TET_EVENT_HANDLER_H

/* Includes */
#include "types.h"
#include "model.h"

/* Public Function Declarations */
void handleAsync( const long lTrigger, Game_Model* m_MainGameModel );
void handleSync( UINT16* fbBase16, Game_Model* m_MainGameModel );

#endif