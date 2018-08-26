#pragma once
#ifndef PAGEDIALOGUEEDITOR_H
#define PAGEDIALOGUEEDITOR_H

#include <Framework/UI/Page.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewButton.hpp>
#include <Framework/UI/Views/ViewDraggable.hpp>
#include <Framework/UI/FadeManager.hpp>

//=====================================================================================
class PageDialogueEditor final : public Page
							   , public View::IActionListener
{
	friend class DialogueEditor;

public:
	PageDialogueEditor()
		: Page( "DIALOGUE_EDITOR" )
		, m_TESTDraggable( new ViewDraggable( "Test!!", this, GetRootView() ) )
	{}

	~PageDialogueEditor()
	{
		GetRootView()->DetatchChild( m_TESTDraggable );
		Free( m_TESTDraggable );
	}

protected:

	void OnEnterPage() override;
	void OnExitPage() override;
	void OnTick( float a_DeltaTime ) override;
	void OnTickPost( float a_DeltaTime ) override;

private:

	void SetupUI();
	
	Array< ViewDraggable* > m_UIDraggableNodes;

	ViewDraggable * m_TESTDraggable;
};

#endif//PAGEDIALOGUEEDITOR_H