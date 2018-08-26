#pragma once
#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <Framework/Containers/Dictionary.hpp>

//=====================================================================================
class XmlObject;

//=====================================================================================
typedef uint32_t DialogueID;
typedef uint32_t ResponseID;

//=====================================================================================
struct Response final
{
	uint32_t ResponseStringID = 0;
	uint32_t ShorthandStringID = 0;
	DialogueID LinkToDialogueID = 0;
};

//=====================================================================================
struct Dialogue final
{
	uint32_t DialogueStringID = 0;
	Array< ResponseID > Responses;
	DialogueID LinkToDialogueID; // Fall back to this link if no responses are present.
};

//=====================================================================================
class Conversation final
{
public:

	static Conversation Create( const Dictionary< DialogueID, Dialogue > & a_Dialogues, const Dictionary< ResponseID, Response > & a_Responses, DialogueID a_EntryDialogueID );
	static Conversation Create( const XmlObject & a_Xml );

	const Dictionary< DialogueID, Dialogue > & GetDialogues() const;
	const Dictionary< ResponseID, Response > & GetResponses() const;
	void RestartDialogue();
	void SetDialogue( DialogueID a_DialogueID );
	const Dialogue & GetCurrentDialogue() const;
	void SelectResponseIndex( uint32_t a_Index );

private:

	DialogueID m_EntryDialogueID;
	DialogueID m_CurrentDialogueID;

	Dictionary< DialogueID, Dialogue > m_Dialogues;
	Dictionary< ResponseID, Response > m_Responses;
};

#endif//DIALOGUE_H