#include "Dialogue.hpp"
#include <Framework/Data/Xml/XmlObject.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
Conversation Conversation::Create( const XmlObject & a_Xml )
{
	if ( !ASSERT_WARN( a_Xml.GetName() == "conversation", __FUNCTION__ ": Xml node must be called \"conversation\"" ) )
	{
		return Create( {}, {}, 0 );
	}

	Dictionary< DialogueID, Dialogue > dialogues;
	Dictionary< ResponseID, Response > responses;
	
	uint32_t conversationNameID = WSID( a_Xml.GetAttribute( "name" ).String.Get() );
	DialogueID entry = WSID( a_Xml.GetAttribute( "entry" ).String.Get() );

	Array< XmlObject > dialogueArrayXml;
	a_Xml.FindChildren( { { XmlObject::Filter::Type::NAME, "dialogue" } }, dialogueArrayXml );
	auto it1 = dialogueArrayXml.First();
	auto end1 = dialogueArrayXml.End();
	while ( it1 != end1 )
	{
		Dialogue dialogue;

		uint32_t name = WSID( it1->GetAttribute( "name" ).String.Get() );
		dialogue.DialogueStringID = WSID( ( *it1 )[ "string_id" ][ 0U ].GetValue().Get() );

		Array< XmlObject > responseListing;
		it1->FindChildren( { { XmlObject::Filter::Type::NAME, "listed_response" } }, responseListing );

		if ( responseListing.Count() > 0 )
		{
			for ( uint32_t i = 0; i < responseListing.Count(); ++i )
			{
				dialogue.Responses.Append( WSID( responseListing[ i ][ 0U ].GetValue().Get() ) );
			}
		}

		else
		{
			PRINT( __FUNCTION__ ": Found 0 response listings, looking for a direct dialogue link instead." );

			const XmlObject & dialogueLink = ( *it1 )[ "dialogue_link" ];
			ASSERT_WARN( dialogueLink.NumChildren() == 1, __FUNCTION__ ": Poorly formed 'dialogue_link'" );
			CString dialinkStr = dialogueLink.NumChildren() == 0 ? "" : dialogueLink[ 0U ].GetValue();
			dialogue.LinkToDialogueID = WSID( dialinkStr.Get() );
		}

		dialogues.Put( name, dialogue );

		++it1;
	}

	Array< XmlObject > responseArrayXml;
	a_Xml.FindChildren( { { XmlObject::Filter::Type::NAME, "response" } }, responseArrayXml );
	auto it2 = responseArrayXml.First();
	auto end2 = responseArrayXml.End();
	while ( it2 != end2 )
	{
		Response response;

		uint32_t name = WSID( it2->GetAttribute( "name" ).String.Get() );
		response.ResponseStringID = WSID( ( *it2 )[ "string_id" ][ 0U ].GetValue().Get() );
		response.ShorthandStringID = WSID( ( *it2 )[ "string_id_short" ][ 0U ].GetValue().Get() );
		response.LinkToDialogueID = WSID( ( *it2 )[ "dialogue_link" ][ 0U ].GetValue().Get() );

		responses.Put( name, response );

		++it2;
	}

	return Conversation::Create( dialogues, responses, entry );
}

//=====================================================================================
Conversation Conversation::Create( const Dictionary< DialogueID, Dialogue > & a_Dialogues, const Dictionary< ResponseID, Response > & a_Responses, DialogueID a_EntryDialogueID )
{
	// Validity is dictated by:
	// 1. Whether the entry dialogue ID is an existing dialogue within the dialogue pool
	// 2. Whether the dialogue pool contains existing links to other dialogues in the event that a dialogue is without responses
	// 3. Whether the dialogue pool only contains links to existing responses
	// 4. Whether the response pool only contains links to existing dialogues
	
	if ( !a_Dialogues.Contains( a_EntryDialogueID ) )
	{
		ASSERT_WARN( false, "Failed to create Conversation: 'a_EntryDialogueID' is not an existing DialogueID in 'a_Dialogues'" );
		return Conversation();
	}

	auto dialogueIt = a_Dialogues.GetEntries().First();
	auto dialogueEnd = a_Dialogues.GetEntries().End();
	while ( dialogueIt != dialogueEnd )
	{
		if ( dialogueIt->Value.Responses.Count() == 0 )
		{
			if ( !a_Dialogues.Contains( dialogueIt->Value.LinkToDialogueID ) )
			{
				ASSERT_WARN( false, "Failed to create Conversation: 'a_Dialogues' contains Dialogue which is attempting to link to a non-existing DialogueID in 'a_Dialogues'" );
				return Conversation();
			}
		}

		auto responsesIt = dialogueIt->Value.Responses.First();
		auto responsesEnd = dialogueIt->Value.Responses.End();
		while ( responsesIt != responsesEnd )
		{
			if ( !a_Responses.Contains( *responsesIt ) )
			{
				ASSERT_WARN( false, "Failed to create Conversation: 'a_Dialogues' contains Dialogue with a non-existing ResponseID in 'a_Responses'" );
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
			ASSERT_WARN( false, "Failed to create Conversation: 'a_Responses' contains Response with a non-existing DialogueID in 'a_Dialogues'" );
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

	if ( currentResponses.Count() == 0 )
	{
		SetDialogue( GetCurrentDialogue().LinkToDialogueID );
		return;
	}

	if ( ASSERT( a_Index < currentResponses.Count(), __FUNCTION__ " -> 'a_Index' was too high. There are %u current responses.", currentResponses.Count() ) )
	{
		SetDialogue( m_Responses[ currentResponses[ a_Index ] ]->LinkToDialogueID );
	}
}
