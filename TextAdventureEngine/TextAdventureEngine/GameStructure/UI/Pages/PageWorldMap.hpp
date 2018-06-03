#pragma once
#ifndef PAGEWORLDMAP_H
#define PAGEWORLDMAP_H

#include <Framework/UI/Page.hpp>
#include <Framework/UI/Views/ViewSlider.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewButton.hpp>
#include <Framework/Math/Curve/Bezier.hpp>

//=====================================================================================
struct Vector2;
struct Colour;

//=====================================================================================
class PageWorldMap final : public Page
						 , public View::IActionListener
{
public:
	PageWorldMap()
		: Page( "WORLD_MAP" )
	{}

protected:

	void OnButtonPress( ViewButton & a_ViewButton );

	void OnEnterPage() override;
	void OnExitPage() override;
	void OnTick( float a_DeltaTime ) override;
	void OnTickPost( float a_DeltaTime ) override;

	// Cleared on exit
	void SetPlayerPath( const Vector2 * a_Begin, const Vector2 * a_End );
	
private:

	void DrawPlayerPathSegment( const Vector2 & a_Pos1, const Vector2 & a_Pos2, const Colour & a_Colour, float a_Thickness = 25.0F );

	struct Segment
	{
		Vector2 Start;
		Vector2 End;
	};

	struct 
	{
		Bezier Curve;
		Array< Segment > SegmentList;
	} 
	m_PlayerPath;

	Vector2 m_Camera;

	ViewButton m_Button = ViewButton( "BackButton", this );
	ViewSlider m_TextField = ViewSlider( "TextField", this );

	static const float DASH_DASHLENGTH;
	static const float DASH_GAPLENGTH;
	static const float DASH_THICKNESS;
};

#endif//PAGEWORLDMAP_H