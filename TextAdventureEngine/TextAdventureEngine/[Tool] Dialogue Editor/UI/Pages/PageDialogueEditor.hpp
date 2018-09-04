#pragma once
#ifndef PAGEDIALOGUEEDITOR_H
#define PAGEDIALOGUEEDITOR_H

#include <Framework/UI/Page.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewButton.hpp>
#include <Framework/UI/Views/ViewDraggable.hpp>
#include <Framework/UI/Views/ViewFlyoutMenu.hpp>
#include <Framework/UI/FadeManager.hpp>
#include <[Tool] Dialogue Editor/DialogueEditor.hpp>

//=====================================================================================
class PageDialogueEditor final : public Page
							   , public View::IActionListener
							   , public InputManager::IMouseEventListener
{
	friend class DialogueEditor;

public:
	PageDialogueEditor()
		: Page( "DIALOGUE_EDITOR" )
	{
	}

	~PageDialogueEditor()
	{
	}

	void AddUINode( uint32_t a_Id, const DialogueEditor::EditorNode & a_NodeData );
	void RemoveUINode( uint32_t a_Id );

protected:

	void OnMousePressed( InputManager::MouseButton a_MouseButton ) override;

	void OnEnterPage() override;
	void OnExitPage() override;
	void OnTick( float a_DeltaTime ) override;
	void OnTickPost( float a_DeltaTime ) override;

private:

	void SetupUI();
	
	Dictionary< uint32_t, ViewDraggable* > m_UIDraggableNodes;

	ViewFlyoutMenu * m_ToolboxMenu;
};

#endif//PAGEDIALOGUEEDITOR_H