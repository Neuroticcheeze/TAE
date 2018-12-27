#include "CompressionManager.hpp"
#include <Framework/Utils/MemoryOps.hpp>

//=====================================================================================
void CompressionManager::Init()
{	
	/*==================|
	|   |   |   |XXX|XXX|
	| XX|XXX|XX |X  |  X|
	| XX|XXX|XX |X  |  X|
	|===|===|===|===|===|
	| XX|XXX|XX |X  |  X|
	| XX|XXX|XX |X  |  X|
	| XX|XXX|XX |XXX|XXX|
	|===|===|===|===|===|
	| XX|XXX|XX |XXX|   |
	| XX|XXX|XX |XXX| # |
	|   |   |   |XXX|   |
	|===========|=======|
	| 
	*/
}

//=====================================================================================
void CompressionManager::InitPost()
{
}

//=====================================================================================
void CompressionManager::Finalise()
{
}

Array< CompressionManager::RectUInt32ValUInt8 > CompressionManager::CompressGridToRectsUInt8( const uint8_t * a_Data, addrspace_t a_Width, addrspace_t a_Height, uint8_t a_Reserved )
{
	PROFILE;

	Array< RectUInt32ValUInt8 > rects;

	uint32_t outputCount = ( a_Width * a_Height ) << 1;
	addrspace_t * output = CAllocate< addrspace_t >( outputCount );
	mask_t * mask = CAllocate< mask_t >( a_Width * a_Height );
	BSet( mask, MASK_VAL_TRUE, a_Width * a_Height * sizeof( mask_t ) );
	BSet( output, 0, a_Width * a_Height * sizeof( addrspace_t ) );

	uint32_t rectCount = 0;

	do
	{
		BSet( output, 0, sizeof( addrspace_t ) * outputCount );
		output[ 0 ] = UINT32_MAX;

		rectCount = 0;

		FastCompress2DMMUInt8( a_Data, a_Width, a_Height, mask, output, a_Reserved );

		uint32_t it = 0;
		do
		{
			if ( output[ 0 ] == UINT32_MAX )
			{
				break;
			}

			uint32_t min = it;
			uint32_t max = output[ it * 2 + 0 ];
			
			uint32_t minX = min % a_Width;
			uint32_t minY = min / a_Width;
			uint32_t maxX = max % a_Width;
			uint32_t maxY = max / a_Width;

			RectUInt32ValUInt8 rect;
			rect.XPos = minX;
			rect.YPos = minY;
			rect.Width = maxX - minX + 1;
			rect.Height = maxY - minY + 1;
			rect.Value = a_Data[ it ];
			rects.Append( rect );

			it = output[ it * 2 + 1 ];
			++rectCount;
		}
		while ( it != UINT32_MAX );
	}
	while ( rectCount > 0 );

	CFree( output );
	CFree( mask );

	return rects;
}

//=====================================================================================
void CompressionManager::FastCompress2DMMUInt8( const uint8_t * a_Data, 
												addrspace_t a_Width, addrspace_t a_Height, 
												mask_t * a_Mask,
												addrspace_t * o_MMOutput,
												uint8_t a_Reserved )
{
	bool failed = false;
	failed |= !ASSERT( a_Data, "No data supplied!" );
	failed |= !ASSERT( a_Width * a_Height, "Invalid dimensions!" );
	failed |= !ASSERT( o_MMOutput, "No output point to write the results into!" );
	if ( failed )
	{
		return;
	}

#define ___idx2xy( idx, x, y ) { x = idx % a_Width; y = idx / a_Width; }
#define ___xy2idx( x, y ) ( ( x ) + ( y ) * a_Width )
#define ___write_new_mml( IDX, MAXIDX, LOOKAHEAD )\
		o_MMOutput[ IDX * 2 + 0 ] = MAXIDX;\
		o_MMOutput[ IDX * 2 + 1 ] = LOOKAHEAD;\


	PROFILE;

	auto findNext = [&]( uint32_t a_Offset )
	{
		for ( uint32_t offIdx = a_Offset; offIdx < a_Width * a_Height; ++offIdx )
		{
			uint32_t x = 0, y = 0;
			___idx2xy( offIdx, x, y );

			if ( a_Mask[ offIdx ] == MASK_VAL_TRUE )
			{
				uint8_t it = a_Data[ offIdx ];
				uint8_t top = ( y == 0 || a_Mask[ ___xy2idx( x, y - 1 ) ] == MASK_VAL_FALSE ) ? a_Reserved : a_Data[ ___xy2idx( x, y - 1 ) ];
				uint8_t left = ( x == 0 || a_Mask[ ___xy2idx( x - 1, y ) ] == MASK_VAL_FALSE )  ? a_Reserved : a_Data[ ___xy2idx( x - 1, y ) ];

				// Did we fail the test? (top & left must be different to us)
				if ( top != it && left != it )
				{
					return offIdx;
				}
			}
		}

		return UINT32_MAX;
	};

	auto kernel = [&]( uint32_t a_XPos, uint32_t a_YPos )
	{
		uint32_t itIdx = ___xy2idx( a_XPos, a_YPos );

		if ( a_Mask[ itIdx ] == MASK_VAL_TRUE )
		{
			uint8_t it = a_Data[ itIdx ];
			uint8_t top = ( a_YPos == 0 || a_Mask[ ___xy2idx( a_XPos, a_YPos - 1 ) ] == MASK_VAL_FALSE ) ? a_Reserved : a_Data[ ___xy2idx( a_XPos, a_YPos - 1 ) ];
			uint8_t left = ( a_XPos == 0 || a_Mask[ ___xy2idx( a_XPos - 1, a_YPos ) ] == MASK_VAL_FALSE )  ? a_Reserved : a_Data[ ___xy2idx( a_XPos - 1, a_YPos ) ];

			// Did we fail the test? (top & left must be different to us)
			if ( top != it && left != it )
			{
				uint32_t maxX, maxY;
				___idx2xy( itIdx, maxX, maxY );

				// Calculate Max_X
				while ( ( maxX < a_Width ) && 
						( a_Mask[ ___xy2idx( maxX, maxY ) ] == MASK_VAL_TRUE ) && 
						( a_Data[ ___xy2idx( maxX, maxY ) ] == it ) )
				{
					a_Mask[ ___xy2idx( maxX, maxY ) ] = MASK_VAL_FALSE;
					++maxX;
				}
				--maxX;

				// Calculate Max_Y
				// We need to sweep between X and Max_X
				bool hitBedrock = false;
				do
				{
					/// Start at the next scanline down (a_PosY+1), the calculation of 
					/// Max_X above implicitly verified the scanline at a_PosY
					++maxY;

					for ( uint32_t sweep = a_XPos; sweep <= maxX; ++sweep )
					{
						if ( ( a_Mask[ ___xy2idx( sweep, maxY ) ] == MASK_VAL_FALSE ) || 
							 ( a_Data[ ___xy2idx( sweep, maxY ) ] != it ) ||
							 ( maxY >= a_Height ) )
						{
							hitBedrock = true;
							--maxY;// We failed the sweep on this scanline, but already set maxY to it, subtract by 1. 
							break;
						}
					}

					// We passed this sweep, set the mask
					if ( !hitBedrock )
					{
						for ( uint32_t sweep = a_XPos; sweep <= maxX; ++sweep )
						{
							a_Mask[ ___xy2idx( sweep, maxY ) ] = MASK_VAL_FALSE;
						}
					}
				}
				while ( !hitBedrock );

				uint32_t next = findNext( itIdx + 1 );
				___write_new_mml( itIdx, ___xy2idx( maxX, maxY ), next );
			}
		}
	};

	// 1. Compute all TL's
	for ( uint32_t y = 0; y < a_Height; ++y )
	{
		for ( uint32_t x = 0; x < a_Width; ++x )
		{
			kernel( x, y );
		}
	}

#undef ___idx2xy
#undef ___xy2idx
#undef ___write_new_mml
}