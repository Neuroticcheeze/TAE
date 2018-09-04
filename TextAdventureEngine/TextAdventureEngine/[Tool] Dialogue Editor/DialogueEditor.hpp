#pragma once
#ifndef DIALOGUEEDITOR_H
#define DIALOGUEEDITOR_H

#include <Framework/ApplicationContext.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Containers/CString.hpp>

//=====================================================================================
class DialogueEditor final
	: public IApplicationContext
{
public:

	struct EditorNode
	{
		ENUM( Type, uint8_t )
			DIALOGUE,
			RESPONSE,
			SELECTOR,
		END_ENUM;

		Type Type;
		CString Name;
		Vector2 Position;
		Vector2 Size;
	};

public:
	
	void Initialise() override;
	void Tick( float a_DeltaTime ) override;
	void Finalise() override;
	const char * GetName() const override { return "Dialogue Editor"; }

	void AddEditorNode( const EditorNode & a_EditorNode )
	{
		m_EditorNodes.Append( a_EditorNode );
	}

	static DialogueEditor * Get();

private:

	Array< EditorNode > m_EditorNodes;
};

#endif//DIALOGUEEDITOR_H