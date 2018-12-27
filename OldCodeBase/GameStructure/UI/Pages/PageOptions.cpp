#include "PageOptions.hpp"
#include <Framework/UI/FadeManager.hpp>
#include <Framework/Engine.hpp>
#include <GameStructure/Game.hpp>
#include <GameStructure/Globals.hpp>
#include <Framework/UI/PageManager.hpp>
#include <GameStructure/UI/Pages/PageMainMenu.hpp>
#include <Framework/UI/FadeManager.hpp>

//=====================================================================================
void PageOptions::SetupUI()
{
	m_Back->SetBorder( View::Alignment::LEFT, 0.275F - 0.2F );
	m_Back->SetBorder( View::Alignment::RIGHT, 0.275F + 0.2F );
	m_Back->SetBorder( View::Alignment::TOP, 0.7F + 0.1F );
	m_Back->SetBorder( View::Alignment::BOTTOM, 0.15F - 0.1F );
	m_Back->GetBackground().SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 40.0F );
	m_Back->SetTint( Colour::WHITE );
	m_Back->GetLabel().SetWordWrap( false );
	m_Back->GetLabel().SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
	m_Back->GetLabel().SetTint( Colour::WHITE );
	m_Back->GetLabel().SetText( "Back" );
	GetRootView()->AddChild( m_Back );

	m_AmbienceVolumeLabel->SetBorder( View::Alignment::LEFT, -1.0F );
	m_AmbienceVolumeLabel->SetBorder( View::Alignment::RIGHT, 1.0F );
	m_AmbienceVolumeLabel->SetBorder( View::Alignment::TOP, 0.0F );
	m_AmbienceVolumeLabel->SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_AmbienceVolumeLabel->SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
	m_AmbienceVolumeLabel->SetHAlign( GraphicsManager::TA_RIGHT );
	m_AmbienceVolumeLabel->SetVAlign( GraphicsManager::TA_CENTER );
	m_AmbienceVolumeLabel->SetTint( Colour::WHITE );

	m_AmbienceVolumeSlider->SetBorder( View::Alignment::LEFT, 0.875F );
	m_AmbienceVolumeSlider->SetBorder( View::Alignment::RIGHT, 0.05F );
	m_AmbienceVolumeSlider->SetBorder( View::Alignment::TOP, 0.1F );
	m_AmbienceVolumeSlider->SetBorder( View::Alignment::BOTTOM, 0.1F );
	m_AmbienceVolumeSlider->GetBackground().SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 40.0F );
	m_AmbienceVolumeSlider->GetBackground().SetTint( Colour( 0.5F, 0.5F, 0.5F, 1.0F ) );
	m_AmbienceVolumeSlider->GetSlider().SetupFromNineSprite( _GetTexture("BG_INVENTORY_0"), 40.0F );
	m_AmbienceVolumeSlider->SetRange( Vector2( 0.0F, 100.0F ) );
	m_AmbienceVolumeSlider->SetStep( 5.0F, true );
	m_AmbienceVolumeSlider->SetSliderSize( 0.125F );
	m_AmbienceVolumeSlider->SetValue( ( m_AmbienceVolumeSlider->GetRange().x + m_AmbienceVolumeSlider->GetRange().y ) * 0.5F );
	m_AmbienceVolumeSlider->GetSlider().AddChild( m_AmbienceVolumeLabel );
	GetRootView()->AddChild( m_AmbienceVolumeSlider );

	m_MyTextField->SetBorder( View::Alignment::LEFT, 0.2F );
	m_MyTextField->SetBorder( View::Alignment::RIGHT, 0.05F );
	m_MyTextField->SetBorder( View::Alignment::TOP, 0.05F );
	m_MyTextField->SetBorder( View::Alignment::BOTTOM, 0.85F );
	m_MyTextField->GetTextView().SetText( "Line 1\nLine 2" );
	m_MyTextField->SetEmptyTextString( "Enter text.." );
	m_MyTextField->SetMultilineEnabled( true );
	m_MyTextField->SetCursorToEnd();
	m_MyTextField->GetBackground().SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 40.0F );
	//m_MyTextField->GetTextView().SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
	GetRootView()->AddChild( m_MyTextField );
}

//=====================================================================================
void PageOptions::OnEnterPage()
{
	PROFILE;

	SetupUI();
	GetRootView()->AddActionListener( this );
}

//=====================================================================================
void PageOptions::OnExitPage()
{
	GetRootView()->RemoveActionListener( this );
}

//=====================================================================================
void PageOptions::OnTick( float a_DeltaTime )
{
}

//=====================================================================================
void PageOptions::OnTickPost( float a_DeltaTime )
{
	GraphicsManager::Instance().SetColour( Colour::COBALT.Lerp( Colour::CYAN, 0.4F ).Lerp( Colour::GRAY, 0.65F ), GraphicsManager::COL_BACKGROUND );
}

//=====================================================================================
void PageOptions::OnSliderValueChanged( ViewSlider & a_ViewSlider, float a_PreviousValue, float a_NewValue )
{
	if ( &a_ViewSlider == m_AmbienceVolumeLabel->GetParent()->GetParent() )
	{
		if ( a_NewValue == a_ViewSlider.GetRange().y )
		{
			m_AmbienceVolumeLabel->SetText( "MAX" );
		}
		else if ( a_NewValue == a_ViewSlider.GetRange().x )
		{
			m_AmbienceVolumeLabel->SetText( "OFF" );
		}
		else
		{
			auto fmt = CString::Format( "%.0f%", a_NewValue );
			m_AmbienceVolumeLabel->SetText( fmt.Get() );
		}
	}
}

//=====================================================================================
void PageOptions::OnButtonPress( ViewButton & a_ViewButton )
{
	if ( &a_ViewButton == m_Back )
	{
		FadeManager::Instance().ListenForFadeBarrier( FadeManager::POST_UI, WSID( "PopOptionsUI" ), *this );

		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::INVISIBLE * Colour::BLACK );
		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::BLACK, 0.2F );
		FadeManager::Instance().PushFadeBarrier( FadeManager::Layer::POST_UI, WSID( "PopOptionsUI" ) );
		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::INVISIBLE * Colour::BLACK, 0.2F );
	}
}

//=====================================================================================
void PageOptions::OnFadeBarrier( uint32_t a_FadeBarrierID )
{
	if ( a_FadeBarrierID == WSID( "PopOptionsUI" ) )
	{
		PageManager::Instance().Pop();
	}
}