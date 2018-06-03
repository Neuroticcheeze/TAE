#include "PageWorldMap.hpp"
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Graphics/Graphics.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Containers/StaticArray.hpp>
#include <Framework/Config/Config.hpp>
#include <GameStructure/World/WorldManager.hpp>
#include <GameStructure/World/Location/LocationInfo.hpp>

//=====================================================================================
const float PageWorldMap::DASH_DASHLENGTH = 20.0F;
const float PageWorldMap::DASH_GAPLENGTH = 10.0F;
const float PageWorldMap::DASH_THICKNESS = 8.0F;

//=====================================================================================
void PageWorldMap::OnEnterPage()
{
	Array< RouteInfo > routesToBlackCreek = WorldManager::Instance().GetAdjacentRoutes( WSID( "black_creek" ) );

	Array< Vector2 > spline;
	spline.Append(WorldManager::Instance().GetLocationInfoFromID(routesToBlackCreek.First()->HeadLocation).LatitudeLongitude);
	spline.Concat(routesToBlackCreek.First()->Spline);
	spline.Append(WorldManager::Instance().GetLocationInfoFromID(routesToBlackCreek.First()->TailLocation).LatitudeLongitude);

	SetPlayerPath(spline.First(), spline.End());

	
	m_Button.SetBorder( View::Alignment::LEFT, 0.8F );
	m_Button.SetBorder( View::Alignment::RIGHT, 0.05F );
	m_Button.SetBorder( View::Alignment::TOP, 0.75F );
	m_Button.SetBorder( View::Alignment::BOTTOM, 0.05F );
	m_Button.GetBackground().SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 80.0F );
	m_Button.GetLabel().SetText( "Button" );
	
	
	m_TextField.SetBorder( View::Alignment::LEFT, 0.05F );
	m_TextField.SetBorder( View::Alignment::RIGHT, 0.85F );
	m_TextField.SetBorder( View::Alignment::TOP, 0.05F );
	m_TextField.SetBorder( View::Alignment::BOTTOM, 0.05F );
	m_TextField.GetBackground().SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 35.0F );
	m_TextField.GetBackground().SetTint( Colour( 0.5F, 0.5F, 0.5F, 1.0F ) );
	m_TextField.GetSlider().SetupFromSingleSprite(_GetTexture("BG_INVENTORY_0") );
	GetRootView()->AddChild( &m_TextField );
	m_TextField.SetRange(Vector2(-1, 1));
	m_TextField.SetStep(0.1F, false);
	m_TextField.SetSliderSize( 0.1F );
	//m_TextField.GetTextView().SetText( "" );
	


	GetRootView().Ptr()->AddActionListener( this );
}

//=====================================================================================
void PageWorldMap::OnExitPage()
{
	GetRootView().Ptr()->RemoveActionListener( this );
	SetPlayerPath( nullptr, nullptr );
}

//=====================================================================================
void PageWorldMap::OnTick( float a_DeltaTime )
{
}

//=====================================================================================
void PageWorldMap::OnTickPost( float a_DeltaTime )
{
	static float k = 0;
	k += a_DeltaTime * 10.25F;
	float k2 = Floor( k );

	Vector2 focus;

	Graphics::Push();
	Graphics::Translate( Vector2( ( float )Config::WINDOW_X, ( float )Config::WINDOW_Y ) * 0.5F );
	Graphics::Translate( -m_Camera );
	
	for ( uint32_t s = 0; s < Min( m_PlayerPath.SegmentList.Count(), ( uint32_t )k2 ); ++s )
	{
		const auto & segment = m_PlayerPath.SegmentList[ s ];
		Vector2 p = segment.End;

		if ( s == ( ( uint32_t )k - 1U ) )
		{
			float t = Clamp( k - k2 );
			p = Lerp( segment.Start, segment.End, t );

			// Focus on the player path
			m_Camera = m_Camera.SmoothLerp( p, a_DeltaTime );
		}

		DrawPlayerPathSegment( segment.Start, p, Colour::RED, PageWorldMap::DASH_THICKNESS );
	}

	Graphics::Pop();
}

//=====================================================================================
void PageWorldMap::DrawPlayerPathSegment( const Vector2 & a_Pos1, const Vector2 & a_Pos2, const Colour & a_Colour, float a_Thickness )
{
	Vector2 diff = a_Pos2 - a_Pos1;
	float length = diff.Length();
	diff = diff.Normalised();
	float ang = 90.0F - Conv_RadiansToDegrees( ATan2F( diff.y, diff.x ) );
	
	Graphics::SetForeColor( a_Colour );	
	Graphics::Push();
	Graphics::Translate( a_Pos1 );
	Graphics::Rotate( ang );
	Graphics::DrawRectangleFill( Vector2( length * 0.5F, 0 ), Vector2( length, a_Thickness ) );
	Graphics::Pop();
}

//=====================================================================================
void PageWorldMap::SetPlayerPath( const Vector2 * a_Begin, const Vector2 * a_End )
{
	m_PlayerPath.SegmentList.Clear();
	m_PlayerPath.Curve = Bezier( Array< Vector2 >( a_Begin, a_End ) );

	const float dashDLength = PageWorldMap::DASH_DASHLENGTH;
	const float dashGLength = PageWorldMap::DASH_GAPLENGTH;

	const uint32_t numSegments = 512;
	ASSERT( numSegments > 1 && IsEven( numSegments ), "The number of segments (%u) must be greater than 1 and even.", numSegments );

	StaticArray< Vector2, numSegments > segs;
	for ( uint32_t s = 0; s < numSegments; ++s )
	{
		const float t = static_cast< float >( s ) / ( numSegments - 1 );
		Vector2 evPos = m_PlayerPath.Curve.Evaluate( t );
		segs.Append( evPos );
	}

	float kLength = dashDLength + dashGLength;
	Vector2 prevP3 = segs[ 0U ];
	float total = 0.0F;
	for ( uint32_t s = 0; s < numSegments - 1; ++s )
	{
		const Vector2 & p1 = segs[ s + 0 ];
		const Vector2 & p2 = segs[ s + 1 ];
		const float pDist = ( p2 - p1 ).Length();

		float t = 1.0F;
		if ( StridedOverInterval( kLength, total, total + pDist, &t ) )
		{
			Vector2 p3 = Lerp( p1, p2, t );
			Vector2 p3End = Lerp( prevP3, p3, dashDLength / kLength );
			m_PlayerPath.SegmentList.Append( { prevP3, p3End } );
			prevP3 = p3;
		}

		total += pDist;
	}
}

//=====================================================================================
void PageWorldMap::OnButtonPress( ViewButton & a_ViewButton )
{
	if ( a_ViewButton == m_Button )
	{

	}
}