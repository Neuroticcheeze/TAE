#include "Dialogue.hpp"

//=====================================================================================
Conversation Conversation::Create( const Dictionary< DialogueID, Dialogue > & a_Dialogues, const Dictionary< ResponseID, Response > & a_Responses, DialogueID a_EntryDialogueID )
{
	// Validity is dictated by:
	// 1. Whether the entry dialogue ID is an existing dialogue within the dialogue pool
	// 2. Whether the dialogue pool only contains links to existing responses
	// 3. Whether the response pool only contains links to existing dialogues
	
	if ( !a_Dialogues.Contains( a_EntryDialogueID ) )
	{
		ASSERT( false, "Failed to create Conversation: 'a_EntryDialogueID' is not an existing DialogueID in 'a_Dialogues'" );
		return Conversation();
	}

	auto dialogueIt = a_Dialogues.GetEntries().First();
	auto dialogueEnd = a_Dialogues.GetEntries().End();
	while ( dialogueIt != dialogueEnd )
	{
		auto responsesIt = dialogueIt->Value.Responses.First();
		auto responsesEnd = dialogueIt->Value.Responses.End();
		while ( responsesIt != responsesEnd )
		{
			if ( !a_Responses.Contains( *responsesIt ) )
			{
				ASSERT( false, "Failed to create Conversation: 'a_Dialogues' contains Dialogue with a non-existing ResponseID in 'a_Responses'" );
				return Conversation();
			}
			++responsesIt;
		}

		++dialogueIt;
	}

	auto responsesIt = a_Responses.GetEntries().First();
	auto responsesEnd = a_Responses.GetEntries().End();
	while ( responsesIt != responsesEnd )
	{
		if ( !a_Dialogues.Contains( responsesIt->Value.LinkToDialogueID ) )
		{
			ASSERT( false, "Failed to create Conversation: 'a_Responses' contains Response with a non-existing DialogueID in 'a_Dialogues'" );
			return Conversation();
		}

		++responsesIt;
	}

	Conversation convo;
	convo.m_Dialogues = a_Dialogues;
	convo.m_Responses = a_Responses;
	convo.m_EntryDialogueID = a_EntryDialogueID;
	convo.RestartDialogue();

	return convo;
}

//=====================================================================================
const Dictionary< DialogueID, Dialogue > & Conversation::GetDialogues() const
{
	return m_Dialogues;
}

//=====================================================================================
const Dictionary< ResponseID, Response > & Conversation::GetResponses() const
{
	return m_Responses;
}

//=====================================================================================
void Conversation::RestartDialogue()
{
	m_CurrentDialogueID = m_EntryDialogueID;
}

//=====================================================================================
void Conversation::SetDialogue( DialogueID a_DialogueID )
{
	if ( ASSERT( m_Dialogues.Contains( a_DialogueID ), __FUNCTION__ " -> Trying to set current dialogue to non-existant dialogue in this conversation object!" ) )
	{
		m_CurrentDialogueID = a_DialogueID;
	}
}

//=====================================================================================
const Dialogue & Conversation::GetCurrentDialogue() const
{
	return *m_Dialogues[ m_CurrentDialogueID ];
}

//=====================================================================================
void Conversation::SelectResponseIndex( uint32_t a_Index )
{
	const auto & currentResponses = GetCurrentDialogue().Responses;

	if ( ASSERT( a_Index < currentResponses.Count(), __FUNCTION__ " -> 'a_Index' was too high. There are %u current responses.", currentResponses.Count() ) )
	{
		SetDialogue( m_Responses[ currentResponses[ a_Index ] ]->LinkToDialogueID );
	}
}
