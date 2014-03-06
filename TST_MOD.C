#include "model.h"
#include "types.h"

int main( )
{
	struct Model m_TstModel = 
	{
		{ { 3, 0 },{0x0004, 0x0004, 0x0004, 0x0004} },
		{ { 0x0 }, 0, 0, 0 }
	};
	UINT16 newMap[ 4 ] = { 0x0, 0x0, 0x0, 0x0 };

	Rotate( &(m_TstModel.cCurrPiece), &(m_TstModel.cMainBoard), D_LEFT );
}
