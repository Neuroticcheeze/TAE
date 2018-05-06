#include "MemoryExt.hpp"

void GlobalPtrTable::RemWeak( void * a_UnderlyingPtr, void * a_Weak )
{
	PtrMeta * meta = m_Refs[ a_UnderlyingPtr ];

	if ( meta )
	{
		int32_t i = meta->Value0.IndexOf( a_Weak );
		
		if ( i >= 0 && i < static_cast< int32_t >( meta->Value0.Count() ) )
		{
			meta->Value0.RemoveAt( i );
		}
	}
}