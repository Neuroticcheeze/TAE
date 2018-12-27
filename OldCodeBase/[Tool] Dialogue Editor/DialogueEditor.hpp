#pragma once
#ifndef DIALOGUEEDITOR_H
#define DIALOGUEEDITOR_H

#include <Framework/ApplicationContext.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Containers/CString.hpp>
#include <[Tool] Dialogue Editor/Structure/Object.hpp>

//=====================================================================================
class DialogueEditor final
	: public IApplicationContext
{
public:

	struct EditorNode
	{
		Vector2 Position;
		Vector2 Size;
		Exposer * Exposer = nullptr;
	};

public:
	
	void Initialise() override;
	void Tick( float a_DeltaTime ) override;
	void Finalise() override;
	const char * GetName() const override { return "Dialogue Editor"; }

	void AddEditorNode( uint32_t a_Id, const EditorNode & a_EditorNode )
	{
		m_EditorNodes.Put( a_Id, a_EditorNode );
	}

	EditorNode * GetEditorNode( uint32_t a_Id )
	{
		return m_EditorNodes[ a_Id ];
	}

	const EditorNode * GetEditorNode( uint32_t a_Id ) const
	{
		return m_EditorNodes[ a_Id ];
	}

	static DialogueEditor * Get();

private:

	Dictionary< uint32_t, EditorNode > m_EditorNodes;
};

#endif//DIALOGUEEDITOR_H