#include "NavFlowField.hpp"
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Graphics/VertexBuffer.hpp>
#include <Framework/Engine.hpp>
#include <Framework/Utils/Hash.hpp>

#include <unordered_set>

//=====================================================================================
NavFlowField::NavFlowField()
	: m_Gfx( new VertexBuffer( GLMesh::DrawMode::DM_QUADS, VFD_POS2_COL4, UINT_MAX ) )
{
}

//=====================================================================================
NavFlowField::~NavFlowField()
{
	SetInputData( nullptr, 0, 0 );

	Free( m_Gfx );
}

//=====================================================================================
void NavFlowField::SetInputData( const uint8_t * a_CostField, uint32_t a_Width, uint32_t a_Height )
{
	const bool differentSize = m_Width * m_Height != a_Width * a_Height;

	// Ensure destination is inside flowfield
	if ( differentSize && !( InRange( m_DestX + 1U, 1U, a_Width ) && InRange( m_DestY + 1U, 1U, a_Height ) ) )
	{
		SetDestination( 0, 0 );
	}

	// We have allocated data with different dimensions, re-allocate all current memory
	if ( m_CostField && differentSize )
	{
		CFree( m_CostField );
		CFree( m_IntegrationField );
		CFree( m_FlowField );
	}

	// Was a call to clear flowfield data
	if ( !a_CostField || a_Width * a_Height == 0 )
	{
		m_Width = m_Height = 0;
		return; // No more processing
	}

	m_Width = a_Width;
	m_Height = a_Height;

	if ( differentSize )
	{
		m_CostField			= CAllocate< uint8_t >( m_Width * m_Height );
		m_IntegrationField	= CAllocate< uint8_t >( m_Width * m_Height );
		m_FlowField			= CAllocate< uint8_t >( m_Width * m_Height );
	}

	BCopy( a_CostField, m_CostField, sizeof( uint8_t ) * m_Width * m_Height );
}

//=====================================================================================
void NavFlowField::SetDestination( uint32_t a_XPos, uint32_t a_YPos )
{
	if ( ASSERT_WARN( ( InRange( a_XPos + 1U, 1U, m_Width ) && InRange( a_YPos + 1U, 1U, m_Height ) ),
					   __FUNCTION__ " [%u,%u] does not fit in a flow graph of size [%u,%u]", a_XPos, a_YPos, m_Width, m_Height ) )
	{
		m_DestX = a_XPos;
		m_DestY = a_YPos;
	}
	
	else
	{
		m_DestX = 0;
		m_DestY = 0;
	}
}

//=====================================================================================
void NavFlowField::Rebuild()
{
	PROFILE;

#define ___idx2xy( idx, x, y ) { x = idx % m_Width; y = idx / m_Width; }
#define ___xy2idx( x, y ) ( ( x ) + ( y ) * m_Width )

#define ___UNVISITED_NODE 0xFF

	std::unordered_set< uint32_t > openSet;
	uint32_t neighbourCache[ 8 ];
	uint32_t neighbourCacheSize = 0;

	auto fetchNeighbours = [&]( uint32_t a_Index )
	{
		neighbourCacheSize = 0;

		uint32_t x, y;
		___idx2xy( a_Index, x, y );

		uint32_t nIdx = 0;

		nIdx = ___xy2idx( x - 1, y );
		if ( x >= 1 && ( m_IntegrationField[ nIdx ] == ___UNVISITED_NODE ) && ( m_CostField[ nIdx ] != 255 ) )
			{ neighbourCache[ neighbourCacheSize++ ] = nIdx; }

		nIdx = ___xy2idx( x + 1, y );
		if ( x < m_Width - 1 && ( m_IntegrationField[ nIdx ] == ___UNVISITED_NODE ) && ( m_CostField[ nIdx ] != 255 ) )
			{ neighbourCache[ neighbourCacheSize++ ] = nIdx; }

		nIdx = ___xy2idx( x, y - 1 );
		if ( y >= 1 && ( m_IntegrationField[ nIdx ] == ___UNVISITED_NODE ) && ( m_CostField[ nIdx ] != 255 ) )
			{ neighbourCache[ neighbourCacheSize++ ] = nIdx; }

		nIdx = ___xy2idx( x, y + 1 );
		if ( y < m_Height - 1 && ( m_IntegrationField[ nIdx ] == ___UNVISITED_NODE ) && ( m_CostField[ nIdx ] != 255 ) ) 
			{ neighbourCache[ neighbourCacheSize++ ] = nIdx; }

		//
	};


	auto fetchNeighbours4 = [&]( uint32_t a_Index )
	{
		neighbourCacheSize = 8;

		uint32_t x, y;
		___idx2xy( a_Index, x, y );

		bool lFail = x == 0;
		bool rFail = x >= m_Width - 1;
		bool bFail = y == 0;
		bool tFail = y >= m_Height - 1;

		neighbourCache[ 0 ] = ( rFail ) ? UINT_MAX : ___xy2idx( x + 1, y + 0 );
		neighbourCache[ 1 ] = UINT_MAX;
		neighbourCache[ 2 ] = ( tFail ) ? UINT_MAX : ___xy2idx( x + 0, y + 1 );
		neighbourCache[ 3 ] = UINT_MAX;
		neighbourCache[ 4 ] = ( lFail ) ? UINT_MAX : ___xy2idx( x - 1, y + 0 );
		neighbourCache[ 5 ] = UINT_MAX;
		neighbourCache[ 6 ] = ( bFail ) ? UINT_MAX : ___xy2idx( x + 0, y - 1 );
		neighbourCache[ 7 ] = UINT_MAX;

	};

	auto fetchNeighbours8 = [&]( uint32_t a_Index )
	{
		neighbourCacheSize = 8;

		uint32_t x, y;
		___idx2xy( a_Index, x, y );

		bool lFail = x == 0;
		bool rFail = x >= m_Width - 1;
		bool bFail = y == 0;
		bool tFail = y >= m_Height - 1;

		neighbourCache[ 0 ] = ( rFail ) ? UINT_MAX : ___xy2idx( x + 1, y + 0 );
		neighbourCache[ 1 ] = ( rFail || tFail ) ? UINT_MAX : ___xy2idx( x + 1, y + 1 );
		neighbourCache[ 2 ] = ( tFail ) ? UINT_MAX : ___xy2idx( x + 0, y + 1 );
		neighbourCache[ 3 ] = ( lFail || tFail ) ? UINT_MAX : ___xy2idx( x - 1, y + 1 );
		neighbourCache[ 4 ] = ( lFail ) ? UINT_MAX : ___xy2idx( x - 1, y + 0 );
		neighbourCache[ 5 ] = ( lFail || bFail ) ? UINT_MAX : ___xy2idx( x - 1, y - 1 );
		neighbourCache[ 6 ] = ( bFail ) ? UINT_MAX : ___xy2idx( x + 0, y - 1 );
		neighbourCache[ 7 ] = ( rFail || bFail ) ? UINT_MAX : ___xy2idx( x + 1, y - 1 );

	};

	// Fill field with "unvisited" values
	BSet( m_IntegrationField, 0xFFFFFFFF, sizeof( uint8_t ) * m_Width * m_Height );

	// Begin with goal
	m_IntegrationField[ ___xy2idx( m_DestX, m_DestY ) ] = 0;
	openSet.insert( ___xy2idx( m_DestX, m_DestY ) );

	// Calculate the integration field
	while ( !openSet.empty() )
	{
		auto nodeIt = openSet.begin();
		uint32_t node = *nodeIt;
		openSet.erase( nodeIt );

		fetchNeighbours( node );

		auto neighbour = neighbourCache;
		while ( neighbour < ( neighbourCache + neighbourCacheSize ) )
		{
			m_IntegrationField[ *neighbour ] = 
												m_IntegrationField[ node ] +
												m_CostField[ *neighbour ] +
												1;
			openSet.insert( *neighbour );
			++neighbour;
		}
	}

	// Calculate the vector field
	for ( uint32_t i = 0; i < m_Width * m_Height; ++i )
	{
		if ( m_CostField[ i ] == 225 )
		{
			continue;
		}

#ifdef IMPL_8DIR_FLOWFIELD
		fetchNeighbours8( i );
#else
		fetchNeighbours4( i );
#endif//IMPL_8DIR_FLOWFIELD

		uint8_t dir = 0;
		uint8_t min = 0xFF;
		auto neighbour = neighbourCache;
		while ( neighbour < ( neighbourCache + neighbourCacheSize ) )
		{
			if ( *neighbour == UINT_MAX )
			{
				++neighbour;
				continue;
			}

			uint8_t f = ( uint8_t )m_IntegrationField[ *neighbour ];
			if ( f < min )
			{
				min = f;
				dir = neighbour - neighbourCache;
			}
			++neighbour;
		}

		m_FlowField[ i ] = dir;
	}

#undef ___idx2xy
#undef ___xy2idx

	// Build optimised rectangles
	m_OptimisedCostData = CompressionManager::Instance().CompressGridToRectsUInt8( m_CostField, m_Width, m_Height, 254 );

	// Build gfx
	m_Gfx->ClearBuffer();

//#define GFX_DRAW_ALL
#ifdef GFX_DRAW_ALL
	for ( uint32_t x = 0; x < m_Width; ++x )
	{
		for ( uint32_t y = 0; y < m_Height; ++y )
		{
			Colour colour;
			colour.r = (float)m_CostField[x + y * m_Width] / 255.0F;
			colour.g = (float)m_IntegrationField[x + y * m_Width] / 55.0F;
			//colour.b = (float)m_FlowField[x + y * m_Width] / 8.0F;
			colour.a = 1.0F;

			colour *= 0.1F;
			
			if ( x == m_DestX && y == m_DestY ) colour = Colour::GREEN;
			
			Vector2 minv = Vector2( x, y );
			Vector2 maxv = minv + Vector2::ONE;
	
			m_Gfx->VtxPosition( minv );
			m_Gfx->VtxColour( colour );
			m_Gfx->PushVertex();
			m_Gfx->VtxPosition( Vector2( minv.x, maxv.y ) );
			m_Gfx->VtxColour( colour );
			m_Gfx->PushVertex();
			m_Gfx->VtxPosition( maxv );
			m_Gfx->VtxColour( colour );
			m_Gfx->PushVertex();
			m_Gfx->VtxPosition( Vector2( maxv.x, minv.y ) );
			m_Gfx->VtxColour( colour );
			m_Gfx->PushVertex();
		}
	}
#else

	for ( uint32_t k = 0; k < m_OptimisedCostData.Count(); ++k )
	{
		const CompressionManager::RectUInt32ValUInt8 & rectAt = m_OptimisedCostData[ k ];
		

		const Colour colour = rectAt.Value == 255 ? Colour::RED : Colour::GRAY;

		//if ( x == m_DestX && y == m_DestY ) colour = Colour::GREEN;
		
		Vector2 minv = Vector2( ( float )rectAt.XPos, ( float )rectAt.YPos );
		Vector2 maxv = minv + Vector2( ( float )rectAt.Width, ( float )rectAt.Height );

		m_Gfx->VtxPosition( minv );
		m_Gfx->VtxColour( colour * 0.5F );
		m_Gfx->PushVertex();
		m_Gfx->VtxPosition( Vector2( minv.x, maxv.y ) );
		m_Gfx->VtxColour( colour * 0.75F );
		m_Gfx->PushVertex();
		m_Gfx->VtxPosition( maxv );
		m_Gfx->VtxColour( colour.Lerp( Colour::ORANGE, 0.15F ) );
		m_Gfx->PushVertex();
		m_Gfx->VtxPosition( Vector2( maxv.x, minv.y ) );
		m_Gfx->VtxColour( colour.Lerp( Colour::VIOLET, 0.15F ) * 0.5F );
		m_Gfx->PushVertex();
	}
#endif//GFX_DRAW_ALL
	m_Gfx->Build();
}

//=====================================================================================
void NavFlowField::Draw()
{
	static const float SIZE = 50.0F;

	GraphicsManager::Instance().TfPush();
	GraphicsManager::Instance().TfScale( SIZE );
	GraphicsManager::Instance().GfxDrawVertexBuffer( *m_Gfx, 0 );
	GraphicsManager::Instance().TfPop();

#define DRAW_INFO_NONUMS
//#define DRAW_INFO
#if defined( DRAW_INFO ) || defined( DRAW_INFO_NONUMS )
	GraphicsManager::Instance().SetColour( Colour::WHITE, GraphicsManager::COL_PRIMARY );
	for ( uint32_t x = 0; x < m_Width; ++x )
	{
		for ( uint32_t y = 0; y < m_Height; ++y )
		{
			if ( m_CostField[x + y * m_Width] != 255 )
			{
#ifndef DRAW_INFO_NONUMS
				GraphicsManager::Instance().TfPush();
				GraphicsManager::Instance().TfTranslate(Vector2(x, y) * SIZE);
				GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
				GraphicsManager::Instance().GfxDrawText(CString::ToString(m_IntegrationField[x + y * m_Width]).Get(), GraphicsManager::Instance().GetBitmapFont(CRC32("BF_LETTERS_AND_DIGITS_DefaultBody")), 24);
				GraphicsManager::Instance().TfPop();
#endif//DRAW_INFO_NONUMS

				float f = DEG2RAD * ( m_FlowField[ x + y * m_Width ] * 45.0F );
				Vector2 dir2 = Vector2( Cos( f ), Sin( f ) );
				GraphicsManager::Instance().GfxDrawLine( ( Vector2( x, y ) + 0.5F ) * SIZE, ( Vector2( x, y ) + 0.5F + dir2 * 0.5F ) * SIZE, 3.0F );
			}
		}
	}
#endif
}

//=====================================================================================
bool NavFlowField::Move( uint32_t & io_XPos, uint32_t & io_YPos ) const
{
	switch ( m_FlowField[ io_XPos + io_YPos * m_Width ] )
	{
	case 0://Right
		io_XPos++;
		break;
	case 1://Top-Right
		io_XPos++;
		io_YPos++;
		break;
	case 2://Top
		io_YPos++;
		break;
	case 3://Top-Left
		io_XPos--;
		io_YPos++;
		break;
	case 4://Left
		io_XPos--;
		break;
	case 5://Bottom-Left
		io_XPos--;
		io_YPos--;
		break;
	case 6://Bottom
		io_YPos--;
		break;
	case 7://Bottom-Right
		io_XPos++;
		io_YPos--;
		break;
	}

	return true;
}

//=====================================================================================
Vector2 NavFlowField::MoveVelocity( const Vector2 & a_LocalPosition ) const
{
	uint32_t x = static_cast< uint32_t >( Floor( a_LocalPosition.x ) );
	uint32_t y = static_cast< uint32_t >( Floor( a_LocalPosition.y ) );
	uint32_t xi = x;
	uint32_t yi = y;

	if ( Move( xi, yi ) )
	{
		Vector2 diff = Vector2( static_cast< float >( ( int )xi - ( int )x ), static_cast< float >( ( int )yi - ( int )y ) );
		return diff.Normalised();
	}

	return Vector2::ZERO;
}