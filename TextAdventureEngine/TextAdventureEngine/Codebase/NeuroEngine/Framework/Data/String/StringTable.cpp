#include "StringTable.hpp"
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
const StringEntry StringTable::NULL_STRING_ENTRY = StringEntry( "[::BAD_STRING_ID::]" );

//=====================================================================================
StringTable::StringTable()
	: m_MemoryFootPrint( 0 )
{}

//=====================================================================================
void StringTable::PutEntry( uint32_t a_Hash, const char * a_String )
{
	StringEntry str = Process( a_String );

	const auto & entries = m_Entries.GetEntries();
	auto it = entries.First();
	const auto end = entries.End();
	while ( it != end )
	{
		if ( it->Key == a_Hash || it->Value == str )
		{
			return;
		}

		++it;
	}

	m_Entries.Put( a_Hash, str );
	m_MemoryFootPrint += sizeof( a_Hash ) + sizeof( StringEntry ) + str.RawString.Length() + 1;
}

//=====================================================================================
uint32_t StringTable::GetMemoryFootprint() const
{
	return m_MemoryFootPrint;
}

//=====================================================================================
const StringEntry & StringTable::operator[]( uint32_t a_Hash ) const
{
	auto str = m_Entries[ a_Hash ];
	return str ? *str : NULL_STRING_ENTRY;
}

//=====================================================================================
StringEntry StringTable::ProcessUnformatted( const char * a_String )
{
	StringEntry result;
	StringEntry::Symbol symbol;

	symbol.Type = StringEntry::Symbol::STRING_DATA;
	
	symbol.StringData_String = a_String;
	result.RawString = a_String;
	result.UnformattedString = a_String;

	result.Symbols.Append( symbol );

	return result;
}

//=====================================================================================
StringEntry StringTable::Process( const char * a_String )
{
	PROFILE;
	StringEntry result;
	
	result.RawString = a_String;

	// Find all tokens
	Array< CString::RegexMatch > allTokensAndFormatters = result.RawString.Regex( R"(\[{1,2}[a-zA-Z0-9 #/]+\])" );

	// C++11 <regex> follows the (ECMA-262) regex syntax 
	// spec; therefore, neg lookbehind is not supported.
	// We do it ourselves, simply checking for any matches
	// that are preceded by a backslash (or at string's start).
	{
		Array< CString::RegexMatch > temp;
		
		auto it = allTokensAndFormatters.First();
		const auto end = allTokensAndFormatters.End();
		while ( it != end )
		{
			if ( it->Start == 0 || result.RawString[ it->Start - 1 ] != '\\' )
			{
				temp.Append( *it );
			}

			++it;
		}

		allTokensAndFormatters = temp;
	}

	typedef StringEntry::Symbol Symbol;

	uint32_t blockStart = 0;
	auto tfIt = allTokensAndFormatters.First();
	auto tfEnd = allTokensAndFormatters.End();
	while ( tfIt != tfEnd )
	{
		const uint32_t s = tfIt->Start;
		const uint32_t l = tfIt->Length;

		CString block1 = result.RawString.SubString( blockStart, ( s - blockStart ) );
		block1.ReplaceAll( "\\[", "[" );//Don't forget to turn all our escaped square-brackets into ordinary square brackets for string data
		CString block2 = result.RawString.SubString( s, l );

		//Block 1, if non-empty, is string data
		if ( block1.Length() > 0 )
		{
			Symbol symbol;
			symbol.Type = Symbol::STRING_DATA;
			symbol.StringData_String = block1;
			result.Symbols.Append( symbol );
			// String Data are the only symbols which get appended to the Unformatted String result.
			result.UnformattedString += block1;
		}

		
#define STRING_TO_ENUM_Push( ENUMVAL, PARAM )\
if ( a1 == #ENUMVAL ) {\
	symbol.Format_Type = StringEntry::Symbol::FormatType::ENUMVAL;\
	if ( symbol.Format_IsPush ) { PARAM }\
} else
#define STRING_TO_ENUM_Pop( ENUMVAL )\
if ( a1 == #ENUMVAL ) {\
	symbol.Format_Type = StringEntry::Symbol::FormatType::ENUMVAL;\
} else

		//Block 2 is a Formatter Symbol 
		if ( block2[ 1 ] == '[' )
		{
			Symbol symbol;
			symbol.Type = Symbol::FORMAT;

			CString a1, a2;

			//Block 2 is a Format Push Directive
			if ( block2[ 2 ] != '/' )
			{
				symbol.Format_IsPush = true;

				//Remove square brackets around symbol
				block2 = block2.SubString( 2, block2.Length() - 3 ).Trim();

				uint32_t spaceIdx = block2.Find( " " );
				if ( spaceIdx != -1 )
				{
					a1 = block2.SubString( 0, spaceIdx );
					a2 = block2.SubString( spaceIdx + 1, block2.Length() - ( spaceIdx + 1 ) );//block2 Str was trimmed so we can assume that the space is not at the start or end.
				}
				else
				{
					a1 = block2;
				}

				// At this point we have subtokens in a1 (and? a2)

				STRING_TO_ENUM_Push( FONTSIZE, 
					CString::Parse( a2.Get(), symbol.Format_Parameter.Number );
				)
				STRING_TO_ENUM_Push( FONTCOLOUR, 
					uint32_t hexval = 0xFFFFFFFF;
					a2.Replace( "#", "0x" );
					CString::Parse( a2.Get(), hexval, true );
					symbol.Format_Parameter.Colour = ColourI( 
						( hexval & 0xFF000000 ) >> 24,
						( hexval & 0xFF0000 ) >> 16, 
						( hexval & 0xFF00 ) >> 8, 
						( hexval & 0xFF )
					);
				)
				STRING_TO_ENUM_Push( BOLD, {} )
				STRING_TO_ENUM_Push( UNDERLINE, {} )
				STRING_TO_ENUM_Push( ITALICS, {} )
				STRING_TO_ENUM_Push( STRIKETHROUGH, {} )
				STRING_TO_ENUM_Push( SUPERSCRIPT, {} )
				STRING_TO_ENUM_Push( SUBSCRIPT, {} )

				// else
				{
					symbol.Format_Type = StringEntry::Symbol::FormatType::EXTENDED_FORMAT;
					symbol.Format_ExtendedType = CRC32( a1.Get() );
					symbol.Format_Parameter.String = a2;
				}
			}
			//Block 2 is a Format Pop Directive
			else
			{
				symbol.Format_IsPush = false;

				//Remove square brackets around symbol
				block2 = block2.SubString( 3, block2.Length() - 4 ).Trim();

				// Pop directives only allow 1 subtoken - the type of formatting to turn off.
				a1 = block2;
				
				STRING_TO_ENUM_Pop( FONTSIZE )
				STRING_TO_ENUM_Pop( FONTCOLOUR )
				STRING_TO_ENUM_Pop( BOLD )
				STRING_TO_ENUM_Pop( UNDERLINE )
				STRING_TO_ENUM_Pop( ITALICS )
				STRING_TO_ENUM_Pop( STRIKETHROUGH )
				STRING_TO_ENUM_Pop( SUPERSCRIPT )
				STRING_TO_ENUM_Pop( SUBSCRIPT )
				{}
			}

			result.Symbols.Append( symbol );
		}

#undef STRING_TO_ENUM_Push
#undef STRING_TO_ENUM_Pop

		//Block 2 is a Token Symbol
		else
		{
			//Remove square brackets around symbol
			block2 = block2.SubString( 1, block2.Length() - 3 ).Trim();

			Symbol symbol;
			symbol.Type = Symbol::TOKEN;
			symbol.Token_StringID = CRC32( block2.Get() );
			result.Symbols.Append( symbol );
		}

		blockStart = s + l;
		++tfIt;
	}

	// Last half-block is not captured above, process it explicitly
	CString::RegexMatch * last = allTokensAndFormatters.Last();
	blockStart = last ? ( last->Start + last->Length ) : 0;
	CString block1 = result.RawString.SubString( blockStart, ( result.RawString.Length() - blockStart ) );
	block1.ReplaceAll( "\\[", "[" );//Don't forget to turn all our escaped square-brackets into ordinary square brackets for string data
	if ( block1.Length() > 0 )
	{
		Symbol symbol;
		symbol.Type = Symbol::STRING_DATA;
		symbol.StringData_String = block1;
		result.Symbols.Append( symbol );
		// String Data are the only symbols which get appended to the Unformatted String result.
		result.UnformattedString += block1;
	}

	// We must have at least 1 symbol for the iterator, add an empty string.
	if ( result.Symbols.Count() == 0 )
	{
		Symbol symbol;
		symbol.Type = Symbol::STRING_DATA;
		result.Symbols.Append( symbol );
	}

	return result;

#ifdef OLD_FORMATTER

	PROFILE;

	StringEntry result;
	result.RawString = a_String;
	CString string = result.RawString;

	string.ReplaceAll( "[[", "@!");
	Array< int32_t > symbolsIdx = string.FindAll( { "@!", "[" } );
	string.ReplaceAll( "@!", "[[" );
	
	{	// add initial string 0->[first token or formatter]
		StringEntry::Symbol symbol;
		symbol.Type = StringEntry::Symbol::Type::STRING_DATA;
		symbol.StringData_String = string.SubString( 0, ( symbolsIdx.Count() > 0 ) ? symbolsIdx[ 0U ] : string.Length() );
		result.Symbols.Append( symbol );
	}

	for ( uint32_t k = 0; k < symbolsIdx.Count(); ++k )
	{
		int32_t symbolOpenIdx = symbolsIdx[ k ];
		int32_t symbolCloseIdx = string.Find( "]", symbolOpenIdx );
		int32_t nextIdx = ( symbolCloseIdx != -1 && ( ( k + 1 ) < symbolsIdx.Count() ) ) ? ( symbolsIdx[ k + 1 ] ) : ( string.Length() );
		
		const bool backslash = ( symbolOpenIdx > 0 && string[ symbolOpenIdx - 1 ] == '\\' );
		if ( symbolCloseIdx == -1 || backslash )
		{
			{	// add string
				StringEntry::Symbol symbol;
				symbol.Type = StringEntry::Symbol::Type::STRING_DATA;
				symbol.StringData_String = string.SubString( symbolOpenIdx, nextIdx - symbolOpenIdx );
				result.Symbols.Append( symbol );

				if ( symbol.StringData_String.Length() > 0 )
				{
					//PRINT( "StringTable::Process -> Found: STRING_DATA (@ %u): %s", symbolOpenIdx, symbol.StringData_String.Get() );
				}
			}
			continue;
		}
		
		// format directive
		if ( string[ symbolOpenIdx + 1 ] == '[' )
		{
			bool push = false;
			CString a1;	// type
			CString a2; // param

			const CString & inner = string.SubString( symbolOpenIdx + 2, symbolCloseIdx - ( symbolOpenIdx + 2 ) ).Trim();
			Array< CString > splits = inner.Split( " " );

			if ( inner.Length() > 0 && inner.Get()[ 0 ] == '/' )
			{
				a1 = inner.SubString( 1, inner.Length() - 1 );
				//PRINT( "StringTable::Process -> Found: FORMAT POP (@ %u): %s", symbolOpenIdx, a1.Get() );
			}

			else if ( splits.Count() == 1 || splits.Count() == 2 )
			{
				push = true;
				a1 = splits[ 0U ];
				a2 = splits.Count() == 1 ? "" : splits[ 1U ];

				//PRINT( "StringTable::Process -> Found: FORMAT (@ %u): %s [%s]", symbolOpenIdx, a1.Get(), a2.Get() );
			}

			{	// add format directive
				StringEntry::Symbol symbol;
				symbol.Type = StringEntry::Symbol::Type::FORMAT;
				symbol.Format_IsPush = push;

#define STRING_TO_ENUM( ENUMVAL, PARAM )\
if ( a1 == #ENUMVAL ) {\
	symbol.Format_Type = StringEntry::Symbol::FormatType::ENUMVAL;\
	if ( push ) { PARAM }\
} else
				STRING_TO_ENUM( FONTSIZE, 
					CString::Parse( a2.Get(), symbol.Format_Parameter.Number );
				)
				STRING_TO_ENUM( FONTCOLOUR, 
					uint32_t hexval = 0xFFFFFFFF;
					a2.Replace( "#", "0x" );
					CString::Parse( a2.Get(), hexval, true );
					symbol.Format_Parameter.Colour = ColourI( 
						( hexval & 0xFF000000 ) >> 24,
						( hexval & 0xFF0000 ) >> 16, 
						( hexval & 0xFF00 ) >> 8, 
						( hexval & 0xFF )
					);
				)
				STRING_TO_ENUM( BOLD, {} )
				STRING_TO_ENUM( UNDERLINE, {} )
				STRING_TO_ENUM( ITALICS, {} )
				STRING_TO_ENUM( STRIKETHROUGH, {} )
				STRING_TO_ENUM( SUPERSCRIPT, {} )
				STRING_TO_ENUM( SUBSCRIPT, {} )

				// else
				{
					symbol.Format_Type = StringEntry::Symbol::FormatType::EXTENDED_FORMAT;
					symbol.Format_ExtendedType = CRC32( a1.Get() );
					symbol.Format_Parameter.String = a2;
				}

#undef STRING_TO_ENUM

				result.Symbols.Append( symbol );
			}

			{	// add string
				StringEntry::Symbol symbol;
				symbol.Type = StringEntry::Symbol::Type::STRING_DATA;
				symbol.StringData_String = string.SubString( symbolCloseIdx + 1, nextIdx - ( symbolCloseIdx + 1 ) );
				result.Symbols.Append( symbol );

				if ( symbol.StringData_String.Length() > 0 )
				{
					//PRINT( "StringTable::Process -> Found: STRING_DATA (@ %u): %s", symbolCloseIdx + 1, symbol.StringData_String.Get() );
				}
			}
		}

		// generic token
		else
		{
			const CString & inner = string.SubString( symbolOpenIdx + 1, symbolCloseIdx - ( symbolOpenIdx + 1 ) );
			const uint32_t stringId = CRC32( inner.Get() );
			//PRINT( "StringTable::Process -> Found: TOKEN  (@ %u): '%s' [ID = %u]", symbolOpenIdx, inner.Get(), stringId );
			
			{	// add token
				StringEntry::Symbol symbol;
				symbol.Type = StringEntry::Symbol::Type::TOKEN;
				symbol.Token_StringID = stringId;
				result.Symbols.Append( symbol );
			}

			{	// add string
				StringEntry::Symbol symbol;
				symbol.Type = StringEntry::Symbol::Type::STRING_DATA;
				symbol.StringData_String = string.SubString( symbolCloseIdx + 1, nextIdx - ( symbolCloseIdx + 1 ) );
				result.Symbols.Append( symbol );

				if ( symbol.StringData_String.Length() > 0 )
				{
					//PRINT( "StringTable::Process -> Found: STRING_DATA (@ %u): %s", symbolCloseIdx + 1, symbol.StringData_String.Get() );
				}
			}
		}
	}

	StringEntry result2;
	result2.RawString = result.RawString;

	auto it = result.Symbols.First();
	auto end = result.Symbols.End();

	while ( it != end )
	{
		if ( it->Type != StringEntry::Symbol::Type::STRING_DATA ||
			 it->StringData_String.Length() > 0 )
		{
			result2.Symbols.Append( *it );
		}

		if ( it->Type == StringEntry::Symbol::Type::STRING_DATA &&
			 it->StringData_String.Length() > 0 )
		{
			result2.UnformattedString += it->StringData_String;
		}

		++it;
	}
	
	return result2;

#endif//OLD_FORMATTER
}