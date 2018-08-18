#include "PageMainMenu.hpp"
#include <Framework/UI/PageManager.hpp>
#include <Framework/Engine.hpp>
#include <GameStructure/UI/Pages/PageOptions.hpp>
#include <GameStructure/Game.hpp>
#include <GameStructure/Globals.hpp>

//=====================================================================================
void PageMainMenu::SetupUI()
{
	m_Title->SetBorder( View::Alignment::LEFT, 0.0F );
	m_Title->SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Title->SetBorder( View::Alignment::TOP, 0.0F );
	m_Title->SetBorder( View::Alignment::BOTTOM, 0.75F );
	m_Title->SetTint( Colour::WHITE );
	m_Title->SetWordWrap( false );
	m_Title->SetHAlign( GraphicsManager::TA_CENTER );
	m_Title->SetVAlign( GraphicsManager::TA_CENTER );
	m_Title->SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultHeading ) );
	m_Title->SetText( "TITLE"  );
	GetRootView()->AddChild( m_Title );

	m_Version->SetBorder( View::Alignment::LEFT, 0.0F );
	m_Version->SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Version->SetBorder( View::Alignment::TOP, 1.0F );
	m_Version->SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_Version->SetTint( Colour::WHITE );
	m_Version->SetWordWrap( false );
	m_Version->SetHAlign( GraphicsManager::TA_LEFT );
	m_Version->SetVAlign( GraphicsManager::TA_BOTTOM );
	m_Version->SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultTiny ) );
	m_Version->SetLineSpacing( 0.0F );
	m_Version->SetText( "[v1.0 Pre-Alpha] Copyright (c) 2018. All Rights Reserved" );
	GetRootView()->AddChild( m_Version );

	m_LoadGame->SetBorder( View::Alignment::LEFT, 0.275F );
	m_LoadGame->SetBorder( View::Alignment::RIGHT, 0.275F );
	m_LoadGame->SetBorder( View::Alignment::TOP, 0.3F );
	m_LoadGame->SetBorder( View::Alignment::BOTTOM, 0.55F );
	m_LoadGame->GetBackground().SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 40.0F );
	m_LoadGame->SetTint( Colour::WHITE );
	m_LoadGame->GetLabel().SetWordWrap( false );
	m_LoadGame->GetLabel().SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
	m_LoadGame->GetLabel().SetTint( Colour::WHITE );
	m_LoadGame->GetLabel().SetText( "Load" );
	GetRootView()->AddChild( m_LoadGame );

	m_Options->SetBorder( View::Alignment::LEFT, 0.275F );
	m_Options->SetBorder( View::Alignment::RIGHT, 0.275F );
	m_Options->SetBorder( View::Alignment::TOP, 0.5F );
	m_Options->SetBorder( View::Alignment::BOTTOM, 0.35F );
	m_Options->GetBackground().SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 40.0F );
	m_Options->SetTint( Colour::WHITE );
	m_Options->GetLabel().SetWordWrap( false );
	m_Options->GetLabel().SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
	m_Options->GetLabel().SetTint( Colour::WHITE );
	m_Options->GetLabel().SetText( "Options" );
	GetRootView()->AddChild( m_Options );
	
	m_Quit->SetBorder( View::Alignment::LEFT, 0.275F );
	m_Quit->SetBorder( View::Alignment::RIGHT, 0.275F );
	m_Quit->SetBorder( View::Alignment::TOP, 0.7F );
	m_Quit->SetBorder( View::Alignment::BOTTOM, 0.15F );
	m_Quit->GetBackground().SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 40.0F );
	m_Quit->SetTint( Colour::WHITE );
	m_Quit->GetLabel().SetWordWrap( false );
	m_Quit->GetLabel().SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
	m_Quit->GetLabel().SetTint( Colour::WHITE );
	m_Quit->GetLabel().SetText( "Quit" );
	GetRootView()->AddChild( m_Quit );
}

//=====================================================================================
void PageMainMenu::OnEnterPage()
{
	PROFILE;

	SetupUI();
	GetRootView()->AddActionListener( this );
}

//=====================================================================================
void PageMainMenu::OnExitPage()
{
	GetRootView()->RemoveActionListener( this );
}

//=====================================================================================
void PageMainMenu::OnTick( float a_DeltaTime )
{
}

//=====================================================================================
void PageMainMenu::OnTickPost( float a_DeltaTime )
{
	GraphicsManager::Instance().SetColour( Colour::COBALT.Lerp( Colour::CYAN, 0.4F ).Lerp( Colour::GRAY, 0.65F ), GraphicsManager::COL_BACKGROUND );
}

//=====================================================================================
void PageMainMenu::OnButtonPress( ViewButton & a_ViewButton )
{
	if ( &a_ViewButton == m_LoadGame )
	{

	}

	if ( &a_ViewButton == m_Options )
	{
		FadeManager::Instance().ListenForFadeBarrier( FadeManager::POST_UI, WSID( "PushOptionsUI" ), *this );

		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::INVISIBLE * Colour::BLACK );
		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::BLACK, 0.2F );
		FadeManager::Instance().PushFadeBarrier( FadeManager::Layer::POST_UI, WSID( "PushOptionsUI" ) );
		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::INVISIBLE * Colour::BLACK, 0.2F );
	}

	if ( &a_ViewButton == m_Quit )
	{
		FadeManager::Instance().ListenForFadeBarrier( FadeManager::POST_UI, WSID( "Quit" ), *this );

		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::INVISIBLE * Colour::BLACK, 0.0F );
		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::BLACK, 0.2F );
		FadeManager::Instance().PushFadeBarrier( FadeManager::Layer::POST_UI, WSID( "Quit" ) );

	}
}

//=====================================================================================
void PageMainMenu::OnFadeBarrier( uint32_t a_FadeBarrierID )
{
	if ( a_FadeBarrierID == WSID( "PushOptionsUI" ) )
	{
		PageManager::Instance().Push( Game::GetOptionsUI() );
	}

	else if ( a_FadeBarrierID == WSID( "Quit" ) )
	{
		Engine::Instance().Quit();
	}
}