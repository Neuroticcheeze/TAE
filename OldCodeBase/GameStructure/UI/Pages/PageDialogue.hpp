#pragma once
#ifndef PAGEDIALOGUE_H
#define PAGEDIALOGUE_H

#include <Framework/UI/Page.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewButton.hpp>
#include <Framework/UI/Views/ViewListSelector.hpp>
#include <Framework/UI/FadeManager.hpp>
#include <GameStructure/Dialogue/Dialogue.hpp>

//=====================================================================================
class PageDialogue final : public Page
						 , private FadeManager::IListener
						 , private View::IActionListener
{
public:
	PageDialogue()
		: Page( "DIALOGUE" )
		, m_DialogueDisplay( new ViewText( "DialogueDisplay", this ) )
		, m_Responses( new ViewListSelector( "ResponseListing", this ) )
		, m_Background( new ViewSprite( "Background", this ) )
		, m_DialogueLetterTicker( 0.0F )
	{}

	~PageDialogue()
	{
		GetRootView()->DetatchChild( m_DialogueDisplay );
		GetRootView()->DetatchChild( m_Responses );
		GetRootView()->DetatchChild( m_Background );
		Free( m_DialogueDisplay );
		Free( m_Responses );
		Free( m_Background );
	}

protected:

	void OnEnterPage() override;
	void OnExitPage() override;
	void OnTick( float a_DeltaTime ) override;
	void OnTickPost( float a_DeltaTime ) override;

	Conversation Conversation;

private:

	void SetupUI();
	void OnFadeBarrier( uint32_t a_FadeBarrierID ) override;
	void OnListSelectionConfirmed( ViewListSelector & a_ViewListSelector, int32_t a_SelectionIndex ) override;

	ViewText		 * m_DialogueDisplay;
	ViewListSelector * m_Responses;
	ViewSprite		 * m_Background;

	float m_DialogueLetterTicker;
};

#endif//PAGEDIALOGUE_H