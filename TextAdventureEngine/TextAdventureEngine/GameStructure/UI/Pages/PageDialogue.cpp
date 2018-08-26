#include "PageDialogue.hpp"
#include <Framework/UI/PageManager.hpp>
#include <Framework/Engine.hpp>
#include <Framework/Data/String/StringManager.hpp>
#include <GameStructure/UI/Pages/PageOptions.hpp>
#include <GameStructure/Game.hpp>
#include <GameStructure/Globals.hpp>


#include <Framework/Data/Xml/XmlManager.hpp>
#include <Framework/File/FileManager.hpp>


//=====================================================================================
void PageDialogue::SetupUI()
{
	m_Background->SetBorder( View::Alignment::LEFT, 0.0F );
	m_Background->SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Background->SetBorder( View::Alignment::TOP, 0.6F );
	m_Background->SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_Background->SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 40.0F );
	m_Background->SetTint( Colour::WHITE );
	GetRootView()->AddChild( m_Background );

	m_DialogueDisplay->SetBorder( View::Alignment::LEFT, 0.075F );
	m_DialogueDisplay->SetBorder( View::Alignment::RIGHT, 0.075F );
	m_DialogueDisplay->SetBorder( View::Alignment::TOP, 0.075F );
	m_DialogueDisplay->SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_DialogueDisplay->SetTint( Colour::WHITE );
	m_DialogueDisplay->SetHAlign( GraphicsManager::TA_LEFT );
	m_DialogueDisplay->SetVAlign( GraphicsManager::TA_TOP );
	m_DialogueDisplay->SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
	m_DialogueDisplay->SetWordWrap( true );
	m_Background->AddChild( m_DialogueDisplay );

	m_Responses->SetBorder( View::Alignment::LEFT, 0.1F );
	m_Responses->SetBorder( View::Alignment::RIGHT, 0.1F );
	m_Responses->SetBorder( View::Alignment::TOP, 0.4F );
	m_Responses->SetBorder( View::Alignment::BOTTOM, 0.075F );
	m_Responses->SetTint( Colour::WHITE );
	m_Background->AddChild( m_Responses );















	StringManager::Instance().InsertOrUpdateDynamicEntry(WSID("Welcome_1_Hello"), "Hello.");
	StringManager::Instance().InsertOrUpdateDynamicEntry(WSID("Welcome_1_Hello2"), "Can I help you?");
	StringManager::Instance().InsertOrUpdateDynamicEntry(WSID("Welcome_1_Introduction"), "I don't know");
	StringManager::Instance().InsertOrUpdateDynamicEntry(WSID("Welcome_1_WhoAreYou"), "Who are you?");
	StringManager::Instance().InsertOrUpdateDynamicEntry(WSID("Welcome_1_WhereAmI"), "Where am I?");
	StringManager::Instance().InsertOrUpdateDynamicEntry(WSID("Welcome_1_WhereIsMyDog"), "Where's my dog?");
	StringManager::Instance().InsertOrUpdateDynamicEntry(WSID("Welcome_1_Dog"), "My Dog");

	XmlObject xml = XmlManager::Instance().ParseXml(FileManager::Instance().ReadAll("res/world/conv_welcome.xml").Get());
	Conversation = Conversation::Create( xml );

}

//=====================================================================================
void PageDialogue::OnEnterPage()
{
	PROFILE;
	SetupUI();
	GetRootView()->AddActionListener( this, false );
}

//=====================================================================================
void PageDialogue::OnExitPage()
{
	GetRootView()->RemoveActionListener( this );
}

//=====================================================================================
void PageDialogue::OnTick( float a_DeltaTime )
{
	m_DialogueLetterTicker += a_DeltaTime;
	if ( m_DialogueLetterTicker > 0.025F )
	{
		m_DialogueLetterTicker = 0.0F;
		m_DialogueDisplay->SetCharacterLimit( m_DialogueDisplay->GetCharacterLimit() + 1 );
	}

	if ( m_DialogueDisplay->GetCharacterLimit() == m_DialogueDisplay->GetText().Length() )
	{
		m_Responses->SetTint( Colour::WHITE );
	}

	else
	{
		m_Responses->SetTint( Colour::INVISIBLE );
	}
}

//=====================================================================================
void PageDialogue::OnTickPost( float a_DeltaTime )
{
	GraphicsManager::Instance().SetColour( Colour::COBALT.Lerp( Colour::CYAN, 0.4F ).Lerp( Colour::GRAY, 0.65F ), GraphicsManager::COL_BACKGROUND );
}

//=====================================================================================
void PageDialogue::OnFadeBarrier( uint32_t a_FadeBarrierID )
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

//=====================================================================================
void PageDialogue::OnListSelectionConfirmed( ViewListSelector & a_ViewListSelector, int32_t a_SelectionIndex )
{
	int32_t responseIdx = m_Responses->GetSelectedIndex();
	Conversation.SelectResponseIndex( responseIdx );

	const CString & dialogue = StringManager::Instance().GetString( Conversation.GetCurrentDialogue().DialogueStringID ).RawString;

	Array< CString > responses;
	for ( uint32_t i = 0; i < Conversation.GetCurrentDialogue().Responses.Count(); ++i )
	{
		ResponseID rid = Conversation.GetCurrentDialogue().Responses[ i ];
		CString response = StringManager::Instance().GetString( Conversation.GetResponses()[ rid ]->ShorthandStringID ).RawString;

		response += " >> \"" + StringManager::Instance().GetString( Conversation.GetResponses()[ rid ]->ResponseStringID ).RawString + "\"";

		responses.Append( response );
	}
	
	m_DialogueDisplay->SetText( dialogue.Get(), true );
	m_Responses->SetList( responses );

	m_DialogueDisplay->SetCharacterLimit( 0 );
	m_DialogueLetterTicker = 0.0f;
}