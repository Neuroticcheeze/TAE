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
					symbol.Format_ExtendedType = WSID( a1.Get() );
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
			const uint32_t stringId = WSID( inner.Get() );
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
}