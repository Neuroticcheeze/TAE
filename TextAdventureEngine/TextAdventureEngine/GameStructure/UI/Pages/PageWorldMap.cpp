#include "PageWorldMap.hpp"
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Graphics/Graphics.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Containers/StaticArray.hpp>
#include <Framework/Engine.hpp>
#include <GameStructure/World/WorldManager.hpp>
#include <GameStructure/World/Location/LocationInfo.hpp>
#include <Framework/UI/FadeManager.hpp>
#include <Framework/Graphics/Filter/ImageFilterManager.hpp>
#include <Framework/Graphics/Filter/GaussianBlurFilter.hpp>

//=====================================================================================
const float PageWorldMap::DASH_DASHLENGTH = 20.0F;
const float PageWorldMap::DASH_GAPLENGTH = 10.0F;
const float PageWorldMap::DASH_THICKNESS = 8.0F;

//=====================================================================================
void PageWorldMap::OnEnterPage()
{
	PROFILE;

	FadeManager::Instance().PushFadeData(FadeManager::Layer::POST_UI, Colour::BLACK, 0.0F, 0.125F );
	FadeManager::Instance().PushFadeData(FadeManager::Layer::POST_UI, Colour::INVISIBLE * Colour::BLACK, 0.5F );

	//Array< RouteInfo > routesToBlackCreek = WorldManager::Instance().GetAdjacentRoutes( WSID( "black_creek" ) );
	//
	//Array< Vector2 > spline;
	//spline.Append(WorldManager::Instance().GetLocationInfoFromID(routesToBlackCreek.First()->HeadLocation).LatitudeLongitude);
	//spline.Concat(routesToBlackCreek.First()->Spline);
	//spline.Append(WorldManager::Instance().GetLocationInfoFromID(routesToBlackCreek.First()->TailLocation).LatitudeLongitude);
	//
	//SetPlayerPath(spline.First(), spline.End());
	//
	//

	
	m_Button->SetBorder(View::Alignment::LEFT, 0.0F);
	m_Button->SetBorder(View::Alignment::RIGHT, 0.0F);
	m_Button->SetBorder(View::Alignment::TOP, 0.0F);
	m_Button->SetBorder(View::Alignment::BOTTOM, 0.0F);
	m_Button->GetBackground().SetupFromNineSprite(_GetTexture("BG_INVENTORY_0"), 25.0F);
	m_Button->GetLabel().SetText("Button");
	m_Button->GetBackground().SetDrawOverrider(this);
	GetRootView()->AddChild(m_Button);
	m_Button->SetZOrder(SHRT_MAX);


	m_Slider->SetBorder( View::Alignment::LEFT, 0.05F );
	m_Slider->SetBorder( View::Alignment::RIGHT, 0.85F );
	m_Slider->SetBorder( View::Alignment::TOP, 0.05F );
	m_Slider->SetBorder( View::Alignment::BOTTOM, 0.05F );
	m_Slider->GetBackground().SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 25.0F );
	m_Slider->GetBackground().SetTint( Colour( 0.5F, 0.5F, 0.5F, 1.0F ) );
	m_Slider->GetSlider().SetupFromNineSprite(_GetTexture("BG_INVENTORY_0"), 25.0F );
	GetRootView()->AddChild(m_Slider);
	m_Slider->SetRange(Vector2(0, 100));
	m_Slider->SetStep( 1, false );
	m_Slider->SetSliderSize( 0.15F );
	m_Slider->SetValue( 0.0F );
	m_Slider->SetZOrder(0);
	
	m_TextField->SetBorder( View::Alignment::LEFT, 0.2F );
	m_TextField->SetBorder( View::Alignment::RIGHT, 0.05F );
	m_TextField->SetBorder( View::Alignment::TOP, 0.05F );
	m_TextField->SetBorder( View::Alignment::BOTTOM, 0.85F );
	m_TextField->GetTextView().SetText( "abc" );
	m_TextField->SetCursorToEnd();
	m_TextField->GetBackground().SetupFromNineSprite(_GetTexture("BG_INVENTORY_0"), 25.0F);
	GetRootView()->AddChild( m_TextField );
	m_TextField->SetZOrder(0);


	m_TextField2->SetBorder( View::Alignment::LEFT, 0.2F );
	m_TextField2->SetBorder( View::Alignment::RIGHT, 0.05F );
	m_TextField2->SetBorder( View::Alignment::TOP, 0.2F );
	m_TextField2->SetBorder( View::Alignment::BOTTOM, 0.7F );
	m_TextField2->GetTextView().SetWordWrap( true );
	m_TextField2->GetTextView().SetText( "abcd" );
	m_TextField2->SetCursorToEnd();
	m_TextField2->GetBackground().SetupFromNineSprite(_GetTexture("BG_INVENTORY_0"), 25.0F);
	m_TextField2->SetEmptyTextString("Enter Name:");
	GetRootView()->AddChild( m_TextField2 );
	m_TextField2->SetZOrder(0);

	m_TickBox->GetSprite( ViewTickBox::SPRITE_BACKGROUND ).SetupFromSingleSprite( _GetTexture( "Button" ) );
	m_TickBox->GetSprite( ViewTickBox::SPRITE_TICKMARK ).SetupFromSingleSprite( _GetTexture( "RadioOn" ) );
	m_TickBox->SetBorder( View::Alignment::LEFT, 0.9F );
	m_TickBox->SetBorder( View::Alignment::RIGHT, 0.0F );
	m_TickBox->SetBorder( View::Alignment::TOP, 0.9F );
	m_TickBox->SetBorder( View::Alignment::BOTTOM, 0.0F );
	GetRootView()->AddChild( m_TickBox );
	m_TickBox->SetZOrder(0);

	GetRootView()->AddActionListener( this );
}

//=====================================================================================
void PageWorldMap::OnExitPage()
{
	m_Button->GetBackground().SetDrawOverrider( nullptr );
	GetRootView()->RemoveActionListener( this );
	SetPlayerPath( nullptr, nullptr );
}

//=====================================================================================
void PageWorldMap::OnTick( float a_DeltaTime )
{
}

//=====================================================================================
void PageWorldMap::OnTickPost( float a_DeltaTime )
{
	
	GraphicsManager::Instance().SetColour( Colour::WHITE, GraphicsManager::COL_PRIMARY );
	//GraphicsManager::Instance().GfxDrawText( CString().Format( "%d", (int)this->m_Slider->GetValue() ).Get(), _GetFont( "DEFUALT" ), 32.0F );

	GaussianBlurParameters gaussianParams;
	gaussianParams.m_pRadius = 32.0F;

	auto mainRT = GraphicsManager::Instance().GetActiveRenderTexture();

	// source image comes from the main rt using the first colour attachment
	uint32_t sourceSMP = GraphicsManager::Instance().SetTexture( mainRT, 0, GraphicsManager::TS_NEXT_AVAILABLE | GraphicsManager::TS_FORCE_LAST_IF_FULL, false );
	gaussianParams.m_iSource = sourceSMP;

	// Run the gaussian blur filter
	ImageFilterManager::FilterRef< GaussianBlurFilter > gaussianBlur = ImageFilterManager::Instance().GetFilter< GaussianBlurFilter >( WSID( "GaussianBlurFilter" ) );
	gaussianBlur->SetParameters( gaussianParams );
	bool success = gaussianBlur->Dispatch();
	
	// restore CA_0 on Main RT ready for textured quad (& returning to previous state)
	GraphicsManager::Instance().ActivateRTDrawBuffers( { 0 }, true );

	if ( success )
	{
		// bind CA_1 from main RT to a sampler unit, then draw a textured quad using the sampler
		uint32_t resultSMP = GraphicsManager::Instance().SetTexture( mainRT, 1, GraphicsManager::TS_NEXT_AVAILABLE | GraphicsManager::TS_FORCE_LAST_IF_FULL, false );
		GraphicsManager::Instance().GfxDrawQuadTextured( Vector2::ZERO, Engine::Instance().GetDisplaySize(), resultSMP, false );
	}






















	static float k = 0;
	k += a_DeltaTime * 10.25F;
	float k2 = Floor( k );

	Vector2 focus;

	Graphics::Push();
	Graphics::Translate(Engine::Instance().GetDisplaySize() * 0.5F );
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
	if ( &a_ViewButton == m_Button )
	{
		PRINT("PRESS");
	}
}

//=====================================================================================
bool PageWorldMap::OnDraw( const ViewSprite & a_Owner, const Vector2 & a_Position, const Vector2 & a_Size )
{
	return true;

	GaussianBlurParameters gaussianParams;
	gaussianParams.m_pRadius = 32.0F;

	auto mainRT = GraphicsManager::Instance().GetActiveRenderTexture();

	// source image comes from the main rt using the first colour attachment
	uint32_t sourceSMP = GraphicsManager::Instance().SetTexture( mainRT, 0, GraphicsManager::TS_NEXT_AVAILABLE | GraphicsManager::TS_FORCE_LAST_IF_FULL, false );

	
	// Run the gaussian blur filter
	ImageFilterManager::FilterRef< GaussianBlurFilter > gaussianBlur = ImageFilterManager::Instance().GetFilter< GaussianBlurFilter >( WSID( "GaussianBlurFilter" ) );
	gaussianBlur->SetParameters( gaussianParams );
	bool success = gaussianBlur->Dispatch();
	
	// restore CA_0 on Main RT ready for textured quad (& returning to previous state)
	GraphicsManager::Instance().ActivateRTDrawBuffers( { 0 }, false );

	if ( success )
	{
		// bind CA_1 from main RT to a sampler unit, then draw a textured quad using the sampler
		uint32_t resultSMP = GraphicsManager::Instance().SetTexture( mainRT, 1, GraphicsManager::TS_NEXT_AVAILABLE | GraphicsManager::TS_FORCE_LAST_IF_FULL, false );
		GraphicsManager::Instance().GfxDrawQuadTextured( a_Position, a_Size, resultSMP, false );
	
		// We successfully rendered our effect, no need for fallback
		return false;
	}

	// If we hit this, then we failed on our effect, render fallback
	return true;
}