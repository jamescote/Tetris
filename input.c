/* Includes */
#include "input.h"
#include <osbind.h>

/* Local Function Declarations */

/*
	Returns whether there is pending input or not.
*/
bool inputPending( )
{
	return Cconis();
}

/*
	Returns the next Character in the buffer.
*/
long getInput( )
{
	return Cnecin();
}