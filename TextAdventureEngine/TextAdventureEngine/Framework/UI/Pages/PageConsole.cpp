#include "PageConsole.hpp"
#include <Framework/Engine.hpp>
#include <Framework/UI/PageManager.hpp>
#include <Framework/Console/ConsoleManager.hpp>

//=====================================================================================
PageConsole::PageConsole()
	: Page( "CONSOLE" )
{
	m_ConsoleFont = GraphicsManager::Instance().LoadBitmapFont( 
		WSID( "_ConsoleFont" ), 
		R"( abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,./<>?;':"[]{}\|-=_+`~!@#$%^&*())", 
		nullptr/*Get the default loaded font*/, 
		26.0F );
}

//=====================================================================================
PageConsole::~PageConsole()
{
	GraphicsManager::Instance().UnloadBitmapFont( m_ConsoleFont );
}

//=====================================================================================
void PageConsole::SetupUI()
{
	const float FIELD_TO_LOG_RATIO = 0.1F;

	m_ConsoleBackground = new ViewRectangles( "ConsoleBackground", this );
	m_ConsoleBackground->SetBorder( View::Alignment::TOP, 0.0F );
	m_ConsoleBackground->SetBorder( View::Alignment::BOTTOM, 0.5F );
	m_ConsoleBackground->SetBorder( View::Alignment::LEFT, 0.0F );
	m_ConsoleBackground->SetBorder( View::Alignment::RIGHT, 0.25F );
	m_ConsoleBackground->SetTint( Colour::WHITE );
	m_ConsoleBackground->GetRectangleList().Clear();
	m_ConsoleBackground->GetRectangleList().Append( { Vector2::ZERO, Vector2::ONE, 3.0F, ( Colour::GRAY * 0.333F ).WithAlpha( 0.5F ), Colour::GRAY * 0.75F } );
	m_ConsoleBackground->GetRectangleList().Append( { Vector2::ZERO, Vector2( 1.0F, FIELD_TO_LOG_RATIO ), 3.0F, Colour::GRAY * 0.5F, Colour::GRAY } );

	m_ConsoleInput = new ViewTextField( "ConsoleInputField", this, m_ConsoleBackground );
	m_ConsoleInput->SetBorder( View::Alignment::TOP, 1.0F - FIELD_TO_LOG_RATIO );
	m_ConsoleInput->SetBorder( View::Alignment::BOTTOM, 0.01F );
	m_ConsoleInput->SetBorder( View::Alignment::LEFT, 0.01F );
	m_ConsoleInput->SetBorder( View::Alignment::RIGHT, 0.01F );
	m_ConsoleInput->GetTextView().SetBitmapFont( m_ConsoleFont );
	m_ConsoleInput->SetTint( Colour::WHITE );
	m_ConsoleInput->GetTextView().SetWordWrap( false );

	m_ConsoleLog = new ViewText( "ConsoleLogText", this, m_ConsoleBackground );
	m_ConsoleLog->SetBorder( View::Alignment::TOP, 0.01F );
	m_ConsoleLog->SetBorder( View::Alignment::BOTTOM, FIELD_TO_LOG_RATIO );
	m_ConsoleLog->SetBorder( View::Alignment::LEFT, 0.01F );
	m_ConsoleLog->SetBorder( View::Alignment::RIGHT, 0.01F );
	m_ConsoleLog->SetBitmapFont( m_ConsoleFont );
	m_ConsoleLog->SetTint( Colour::WHITE );
	m_ConsoleLog->SetLineSpacing( 5.0F );
	m_ConsoleLog->SetVAlign( GraphicsManager::TextAlignment::TA_BOTTOM );
	m_ConsoleLog->SetWordWrap( false );
}

//=====================================================================================
void PageConsole::OnEnterPage()
{
	PROFILE;

	SetupUI();
	GetRootView()->AddActionListener( this );
}

//=====================================================================================
void PageConsole::OnExitPage()
{
	m_ConsoleLog->Destruct();
	m_ConsoleLog->GetParent()->DetatchChild( m_ConsoleLog );
	Free( m_ConsoleLog );
	
	m_ConsoleInput->Destruct();
	m_ConsoleInput->GetParent()->DetatchChild( m_ConsoleInput );
	Free( m_ConsoleInput );
	
	m_ConsoleBackground->Destruct();
	m_ConsoleBackground->GetParent()->DetatchChild( m_ConsoleBackground );
	Free( m_ConsoleBackground );

	GetRootView()->RemoveActionListener( this );
}

//=====================================================================================
void PageConsole::OnTick( float a_DeltaTime )
{
	m_ConsoleLog->SetText( ConsoleManager::Instance().GetLog().Get(), true );
}

//=====================================================================================
void PageConsole::OnTickPost( float a_DeltaTime )
{
}