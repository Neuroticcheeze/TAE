#include "PageDialogueEditor.hpp"
#include <Framework/UI/PageManager.hpp>
#include <Framework/Engine.hpp>
#include <[Tool] Dialogue Editor/Globals.hpp>

#include <Framework/UI/Views/ViewTextField.hpp>
#include <Framework/UI/Views/ViewSizer.hpp>
#include <Framework/UI/Views/ViewRectangles.hpp>
#include <Framework/Math/Curve/Bezier.hpp>

//=====================================================================================
void PageDialogueEditor::SetupUI()
{
	m_TESTDraggable->SetBordersFromSizeAndOffset(Vector2(600.0F, 350.0F));
	
	ViewSprite * rects = new ViewSprite( "rects", this, m_TESTDraggable );
	rects->SetBorder( View::Alignment::TOP, 0.0F );
	rects->SetBorder( View::Alignment::LEFT, 0.0F );
	rects->SetBorder( View::Alignment::RIGHT, 0.0F );
	rects->SetBorder( View::Alignment::BOTTOM, 0.0F );
	rects->SetDrawOverrider([]( const ViewSprite & a_Owner, const Vector2 & a_Position, const Vector2 & a_Size )
	{
		GraphicsManager::Instance().SetState(GraphicsManager::RS_TRANSPARENCY, false);
		
		Bezier bezier;
		bezier.AddControlPoint(Vector2(0.0F, 50.0F));
		bezier.AddControlPoint(Vector2(100.0F, 50.0F));
		bezier.AddControlPoint(a_Position + Vector2(0.0F, a_Size.y - 20) - Vector2(100,0));
		bezier.AddControlPoint(a_Position + Vector2(0.0F, a_Size.y - 20));
		GraphicsManager::Instance().SetColour(Colour::ORANGE, GraphicsManager::COL_PRIMARY);
		GraphicsManager::Instance().GfxDrawBezier( bezier, 3.0F );
		return false;
	});

	// Add elements
	{
		ViewRectangles * background = new ViewRectangles( "bg", this, m_TESTDraggable );
		background->GetRectangleList().Append( { Vector2::ZERO, Vector2::ONE, 3.0F, ( Colour::GRAY * 0.4F ).WithAlpha( 1.0F ), ( Colour::GRAY * 0.9F ).WithAlpha( 1.0F ) } );
		background->SetBorder( View::Alignment::TOP, 0.0F );
		background->SetBorder( View::Alignment::LEFT, 0.0F );
		background->SetBorder( View::Alignment::RIGHT, 0.0F );
		background->SetBorder( View::Alignment::BOTTOM, 0.0F );
		
		ViewRectangles * tag = new ViewRectangles( "top", this, m_TESTDraggable );
		tag->GetRectangleList().Append( { Vector2::ZERO, Vector2::ONE, 3.0F, ( Colour::COBALT.Lerp( Colour::GRAY, 0.6F ) * 0.9F ).WithAlpha( 1.0F ), ( Colour::COBALT.Lerp( Colour::GRAY, 0.6F ) * 0.9F ).WithAlpha( 1.0F ) } );
		tag->SetBorder( View::Alignment::TOP, 0.0F );
		tag->SetBorder( View::Alignment::LEFT, 0.0F );
		tag->SetBorder( View::Alignment::RIGHT, 0.0F );
		tag->SetBorderPx( View::Alignment::BOTTOM, 70.0F );
		
		ViewTextField * name = new ViewTextField( "name", this, tag );
		name->SetEmptyTextString( "" );
		name->GetTextView().SetText( "Unnamed Node" );
		name->SetCursorToEnd();
		name->GetTextView().SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
		name->GetTextView().SetVAlign( GraphicsManager::TextAlignment::TA_CENTER );
		name->SetBorder( View::Alignment::TOP, 0.0F );
		name->SetBorder( View::Alignment::LEFT, 0.0F );
		name->SetBorder( View::Alignment::RIGHT, 0.0F );
		name->SetBorder( View::Alignment::BOTTOM, 0.0F );

		ViewTextField * data = new ViewTextField( "data", this, tag );
		data->SetEmptyTextString( "" );
		data->SetMultilineEnabled( true );
		data->GetTextView().SetText( FONTCOLOUR_TAG(22AAFFAA) "<string_id name=" FONTCOLOUR_TAG(22FFAAFF) "\"SID_L04313445_WELCOME_01\">" FONTCOLOUR_POP "\n    \"Hello, World!\"\n" FONTCOLOUR_TAG(22AAFFAA) "</string_id>", true );
		data->GetTextView().SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultTiny ) );
		data->SetBorderPx( View::Alignment::TOP, 0.0F );
		data->SetBorder( View::Alignment::LEFT, 0.0F );
		data->SetBorder( View::Alignment::RIGHT, 0.0F );
		data->SetBorder( View::Alignment::BOTTOM, -1.0F );

		ViewSizer * sizer = new ViewSizer( "sizer", this, m_TESTDraggable );
		sizer->SetTint( Colour::WHITE );
		sizer->SetBorderPx( View::Alignment::TOP, 32.0F );
		sizer->SetBorderPx( View::Alignment::LEFT, 32.0F );
		sizer->SetBorder( View::Alignment::RIGHT, 0.0F );
		sizer->SetBorder( View::Alignment::BOTTOM, 0.0F );

	}
}

//=====================================================================================
void PageDialogueEditor::OnEnterPage()
{
	PROFILE;

	SetupUI();
	GetRootView()->AddActionListener( this );
}

//=====================================================================================
void PageDialogueEditor::OnExitPage()
{
	GetRootView()->RemoveActionListener( this );
}

//=====================================================================================
void PageDialogueEditor::OnTick( float a_DeltaTime )
{
}

//=====================================================================================
void PageDialogueEditor::OnTickPost( float a_DeltaTime )
{
	GraphicsManager::Instance().SetColour( Colour::GRAY * 0.2F, GraphicsManager::COL_BACKGROUND );
}