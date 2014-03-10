#include <osbind.h>
#include <stdio.h>

int main( )
{
	long cInput = Cnecin( );
	
	while( cInput != 'q' )
	{
		printf( "int code: %lX :: %c = %d\n", (long)cInput, (char)cInput, (int)cInput );
		cInput = Cnecin( );
	}
}