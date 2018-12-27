#include "PageMainMenu.hpp"
#include <Framework/UI/PageManager.hpp>
#include <Framework/Engine.hpp>
#include <GameStructure/UI/Pages/PageOptions.hpp>
#include <GameStructure/UI/Pages/PageDialogue.hpp>
#include <GameStructure/Game.hpp>
#include <GameStructure/Globals.hpp>

#include <Framework/UI/Views/ViewRectangles.hpp>
#include <Framework/Math/Curve/Bezier.hpp>

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



	m_TESTDraggable->SetBordersFromSizeAndOffset(Vector2(500.0F, 150.0F));
	ViewSprite * rects = new ViewSprite( "rects", this );
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
		GraphicsManager::Instance().SetColour(Colour::GRAY * 0.4F, GraphicsManager::COL_PRIMARY);
		GraphicsManager::Instance().SetColour(Colour::GRAY * 0.9F, GraphicsManager::COL_SECONDARY);
		GraphicsManager::Instance().GfxDrawQuad(a_Position, a_Size, 3.0F);
		GraphicsManager::Instance().SetColour(Colour::GRAY * 0.9F, GraphicsManager::COL_PRIMARY);
		GraphicsManager::Instance().GfxDrawQuad(a_Position + Vector2(0.0F, a_Size.y) + Vector2(0, -60), Vector2(a_Size.x, 60));
		GraphicsManager::Instance().SetColour(Colour::WHITE, GraphicsManager::COL_PRIMARY);
		GraphicsManager::Instance().SetColour(Colour::INVISIBLE, GraphicsManager::COL_SECONDARY);
		GraphicsManager::Instance().SetState(GraphicsManager::RS_TRANSPARENCY, true);
		GraphicsManager::Instance().TfPush();
		GraphicsManager::Instance().TfTranslate( a_Position + Vector2(0.0F, a_Size.y) + Vector2(10, -30 ) );
		GraphicsManager::Instance().GfxDrawText("\"ENTRY\" (Dialogue)", (GraphicsManager::Font)nullptr,32.0F, GraphicsManager::TA_LEFT, GraphicsManager::TA_CENTER, GraphicsManager::TF_SHADOW);
		GraphicsManager::Instance().TfPop();
		GraphicsManager::Instance().TfPush();
		GraphicsManager::Instance().TfTranslate( a_Position + Vector2(0.0F, a_Size.y) + Vector2(10, -70 ) );
		GraphicsManager::Instance().GfxDrawText("MetaType: VT_RESPONSE", (GraphicsManager::Font)nullptr,24.0F, GraphicsManager::TA_LEFT, GraphicsManager::TA_TOP, GraphicsManager::TF_SHADOW);
		GraphicsManager::Instance().TfPop();

		return false;
	});
	m_TESTDraggable->AddChild( rects );
	GetRootView()->AddChild( m_TESTDraggable );
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
	GraphicsManager::Instance().SetColour( Colour::GRAY * 0.2F, GraphicsManager::COL_BACKGROUND );
}

//=====================================================================================
void PageMainMenu::OnButtonPress( ViewButton & a_ViewButton )
{
	if ( &a_ViewButton == m_LoadGame )
	{
		FadeManager::Instance().ListenForFadeBarrier( FadeManager::POST_UI, WSID( "PushLoadGameUI" ), *this );

		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::INVISIBLE * Colour::BLACK );
		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::BLACK, 0.2F );
		FadeManager::Instance().PushFadeBarrier( FadeManager::Layer::POST_UI, WSID( "PushLoadGameUI" ) );
		FadeManager::Instance().PushFadeData( FadeManager::Layer::POST_UI, Colour::INVISIBLE * Colour::BLACK, 0.2F );
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
	if ( a_FadeBarrierID == WSID( "PushLoadGameUI" ) )
	{
		PageManager::Instance().Push( Game::GetDialogueUI() );
	}

	else if ( a_FadeBarrierID == WSID( "PushOptionsUI" ) )
	{
		PageManager::Instance().Push( Game::GetOptionsUI() );
	}

	else if ( a_FadeBarrierID == WSID( "Quit" ) )
	{
		Engine::Instance().Quit();
	}
}