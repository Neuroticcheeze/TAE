#include "CString.hpp"

#include <Framework/Utils/MemoryOps.hpp>
#include <Framework/Math/Math.hpp>
#include <Framework/Containers/WString.hpp>
#include <Framework/Containers/DataStream.hpp>
#include <stdarg.h>
#include <sstream>
#include <string>

//=====================================================================================
CString::CString()
	: m_String( nullptr )
	, m_Length( 0 )
	, m_Capacity( 0 )
{

}

//=====================================================================================
CString::CString( const CString & a_Other )
	: m_String( nullptr )
	, m_Length( 0 )
	, m_Capacity( 0 )
{
	*this = a_Other;
}

//=====================================================================================
CString::CString( const char * a_CharStr )
	: m_String( nullptr )
	, m_Length( 0 )
	, m_Capacity( 0 )
{
	*this = a_CharStr;
}

//=====================================================================================
CString::CString( const char * a_Begin, const char * a_End )
	: m_String( nullptr )
	, m_Length( 0 )
	, m_Capacity( 0 )
{
	if ( ASSERT( a_Begin < a_End, "Failed to construct CString with invalid start & end addresses: [%u/%u]", a_Begin, a_End ) )
	{
		m_Length = a_End - a_Begin;
		m_Capacity = NextPowerOf2( m_Length );
		m_String = BAllocate< char >( m_Length + 1 );
		BCopy( a_Begin, m_String, ( m_Length ) * sizeof( char ) );
		m_String[ m_Length ] = '\0';
	}
}

//=====================================================================================
CString::~CString()
{
	BFree( m_String );
	m_Capacity = 0;
	m_Length = 0;
}

//=====================================================================================
void CString::Write( DataStream & a_Writer ) const
{
	a_Writer.Write< uint16_t >( m_Length );
	a_Writer.WriteRaw( m_Length + 1, m_String );
}

//=====================================================================================
void CString::Read( DataStream & a_Reader )
{
	uint16_t len;
	a_Reader.Read( len );

	++len;

	char * buffer = BAllocate< char >( len );
	a_Reader.ReadRaw( len, buffer );

	*this = buffer;

	BFree( buffer );
}

//=====================================================================================
CString & CString::operator=( const CString & a_Other )
{
	if ( m_String )
	{
		BFree( m_String );
	}

	m_Length = a_Other.m_Length;
	m_Capacity = a_Other.m_Capacity;

	if ( a_Other.m_String )
	{
		m_String = BAllocate< char >( m_Capacity );
		BCopy( a_Other.m_String, m_String, m_Length + 1 ); // don't forget null terminator
	}

	return *this;
}

//=====================================================================================
CString & CString::operator=( const char * a_CharStr)
{
	if ( m_String )
	{
		BFree( m_String );
	}

	if ( a_CharStr )
	{
		m_Length = 0;

		const char * nextChar = a_CharStr;

		while ( *nextChar != '\0' )
		{
			++m_Length;
			++nextChar;
		}
		
		m_Capacity = NextPowerOf2( m_Length + 1 );
		m_String = BAllocate< char >( m_Capacity );
		BCopy( a_CharStr, m_String, m_Length + 1 ); // don't forget null terminator
	}

	else
	{
		m_Capacity = 0;
		m_Length = 0;
	}

	return *this;
}

//=====================================================================================
CString & CString::operator+=( const CString & a_Other )
{
	return *this += a_Other.m_String;
}

//=====================================================================================
CString & CString::operator+=( const char * a_CharStr )
{
	if ( a_CharStr )
	{
		uint32_t length = 0;

		const char * nextChar = a_CharStr;

		while ( *nextChar != '\0' )
		{
			++length;
			++nextChar;
		}

		uint32_t oldLength = m_Length;
		char * oldString = m_String;

		m_Length += length;
		m_Capacity = NextPowerOf2( m_Length + 1 );

		m_String = BAllocate< char >( m_Capacity );

		// copy part 1
		if ( oldString )
		{
			BCopy( oldString, m_String, oldLength );
		}

		// copy part 2
		BCopy( a_CharStr, m_String + oldLength, length + 1 ); // don't forget null terminator

		if ( oldString )
		{
			BFree( oldString );
		}
	}

	return *this;
}

//=====================================================================================
CString & CString::operator+=( char a_Char )
{
	uint32_t oldLength = m_Length;
	char * oldString = m_String;

	++m_Length;
	m_Capacity = NextPowerOf2( m_Length + 1 );

	m_String = BAllocate< char >( m_Capacity );

	// copy part 1
	if ( oldString )
	{
		BCopy( oldString, m_String, oldLength );
	}

	m_String[ oldLength ] = a_Char;
	m_String[ oldLength + 1 ] = '\0';

	if ( oldString )
	{
		BFree( oldString );
	}

	return *this;
}

//=====================================================================================
bool CString::operator==( const CString & a_Other ) const
{
	return *this == a_Other.m_String;
}

//=====================================================================================
bool CString::operator==( const char * a_CharStr ) const
{
	if ( a_CharStr == m_String )
	{
		return true;
	}

	if ( a_CharStr == nullptr || m_String == nullptr )
	{
		return false;
	}

	return strcmp( a_CharStr, m_String ) == 0;
}

//=====================================================================================
bool CString::operator!=(const CString & a_Other) const
{
	return *this != a_Other.m_String;
}

//=====================================================================================
bool CString::operator!=(const char * a_CharStr) const
{
	return !( *this == a_CharStr );
}

//=====================================================================================
CString operator+( const CString & a_Left, const CString & a_Right )
{
	CString result = a_Left;
	return result += a_Right;
}

//=====================================================================================
CString operator+( const CString & a_Left, const char * a_Right )
{
	CString result = a_Left;
	return result += a_Right;
}

//=====================================================================================
CString operator+( const CString & a_Left, char a_Right )
{
	CString result = a_Left;
	return result += a_Right;
}

//=====================================================================================
const char * CString::Get() const
{
	return m_String == nullptr ? "" : m_String;
}

//=====================================================================================
uint32_t CString::Length() const
{
	return m_Length;
}

//=====================================================================================
CString CString::SubString( uint32_t a_Start, uint32_t a_Length ) const
{
	PROFILE;

	if ( a_Start >= m_Length || a_Length == 0 )
	{
		return CString();
	}
	
	uint32_t end = a_Start + a_Length;

	if ( end > m_Length )
	{
		end = m_Length;
	}

	char prevChar = m_String[ end ];
	m_String[ end ] = '\0';
	CString result( m_String + a_Start );
	m_String[ end ] = prevChar;

	return result;
}

//=====================================================================================
int32_t CString::Find( const CString & a_SubString, uint32_t a_StartIndex ) const
{
	PROFILE;

	if ( a_SubString.m_String == nullptr || m_String == nullptr )
	{
		return -1;
	}

	const char * ssFirstChar = &( ( const char * ) a_SubString )[ 0 ];
	const char * ssCurrentChar;
	uint32_t ssLen = a_SubString.Length();

	const char * nextChar = m_String + a_StartIndex;

	while (*nextChar != '\0')
	{
		if ( *nextChar == *ssFirstChar )// current char and first char of substring match, let's check the rest
		{
			bool match = true;

			ssCurrentChar = ssFirstChar; //reset the current char
			for ( uint32_t s = 1; s < ssLen; ++s ) // start at 1 because we already checked the first letter
			{
				if ( s + static_cast< int32_t >( nextChar - m_String ) >= m_Length )
				{
					match = false;
					break;
				}

				char ss = *( ssCurrentChar + s );
				char tc = *( nextChar + s );

				if ( ss != tc )
				{
					match = false;
					break;
				}
			}

			if ( match )
			{
				return static_cast< int32_t >( nextChar - m_String );
			}
		}

		++nextChar;
	}

	return -1;
}

//=====================================================================================
Array< int32_t > CString::FindAll( const CString & a_SubString ) const
{
	PROFILE;

	Array< int32_t > result;
	int32_t id = 0;
	do
	{
		id = Find( a_SubString, id );

		if ( id == -1 )
		{
			break;
		}

		result.Append( id );
		id += a_SubString.Length();
	}
	while ( true );

	return result;
}

//=====================================================================================
Array< int32_t > CString::FindAll( InitialiserList< const char * > a_SubStrings ) const
{
	PROFILE;

	Array< int32_t > result;

	uint32_t num = NSize( a_SubStrings );
	for ( uint32_t j = 0; j < num; ++j )
	{
		for ( uint32_t j2 = 0; j2 < num; ++j2 )
		{
			if ( j != j2 )
			{
				if ( strcmp( *( NBegin( a_SubStrings ) + j ), *( NBegin( a_SubStrings ) + j2 ) ) == 0 )
				{
					// duplicate entries.
				}
			}
		}
	}
	
	for ( uint32_t j = 0; j < num; ++j )
	{
		const Array< int32_t > & list = FindAll( CString( *( NBegin( a_SubStrings ) + j ) ) );
		const int32_t * it = list.First();
		const int32_t * const end = list.End();

		while ( it < end )
		{
			result.Append( *it );
			++it;
		}
	}

	result.Sort( []( const int32_t & a_Left, const int32_t & a_Right ){ return a_Left < a_Right; } );
	return result;
}

//=====================================================================================
void CString::Replace( const CString & a_From, const CString & a_To )
{
	PROFILE;

	int32_t start = Find( a_From );
	int32_t length = a_From.Length();

	if ( start != -1 )
	{
		CString pre = SubString( 0, start);
		CString post = SubString( start + length, m_Length - ( start + length ) );

		*this = pre + a_To + post;
	}
}

//=====================================================================================
void CString::ReplaceAll( const CString & a_From, const CString & a_To )
{
	Array< int32_t > starts = FindAll( a_From );
	int32_t length = a_From.Length();

	int32_t offset = 0;

	for ( uint32_t t = 0; t < starts.Count(); ++t )
	{
		if ( starts[ t ] != -1 )
		{
			CString pre = SubString( 0, starts[ t ] );
			CString post = SubString( starts[ t ] + length, m_Length - ( starts[ t ] + length ) );

			offset += a_To.Length() - a_From.Length();
			*this = pre + a_To + post;
		}
	}
}

//=====================================================================================
Array< CString > CString::Split( const CString & a_Delimiter ) const
{
	PROFILE;

	Array< CString > cuts;

	CString rest = *this;

	int32_t length = a_Delimiter.Length();

	do
	{
		int32_t start = rest.Find( a_Delimiter );

		if ( start == -1 )
		{
			cuts.Append( rest );
			break;
		}

		cuts.Append( rest.SubString( 0, start ) );
		rest = rest.SubString( start + length, rest.Length() - ( start + length ) );
	}

	while ( true );

	return cuts;
}

//=====================================================================================
Array< CString > CString::Split( const char ** a_Delimiters, uint32_t a_NumDelimeters ) const
{
	PROFILE;

	Array< CString > cuts;

	CString rest = *this;

	do
	{
		int32_t start, length;
		
		for (uint32_t k = 0; k < a_NumDelimeters; ++k )
		{
			start = rest.Find( *( a_Delimiters + k ) );

			if ( start != -1 )
			{
				length = strnlen_s( *( a_Delimiters + k ), 8192 );
				break;
			}
		}

		if ( start == -1 )
		{
			cuts.Append( rest );
			break;
		}

		cuts.Append( rest.SubString( 0, start ) );
		rest = rest.SubString( start + length, rest.Length() - ( start + length ) );
	}

	while ( true );

	return cuts;
}

//=====================================================================================
void CString::ToUpper() const
{
	PROFILE;

	char * nextChar = m_String;
	
	while ( *nextChar != '\0' )
	{
		if ( *nextChar >= 'a' && *nextChar <= 'z' )
		{
			*nextChar -= 32;
		}

		++nextChar;
	}
}

//=====================================================================================
void CString::ToLower() const
{
	PROFILE;

	char * nextChar = m_String;
	
	while ( *nextChar != '\0' )
	{
		if ( *nextChar >= 'A' && *nextChar <= 'Z' )
		{
			*nextChar += 32;
		}

		++nextChar;
	}
}

//=====================================================================================
CString & CString::Format( const char * a_Format, ... )
{
	PROFILE;

	char buffer[ 4096 ];

	va_list argptr;
	va_start( argptr, a_Format );
	vsnprintf( buffer, sizeof( buffer ), a_Format, argptr );
	va_end( argptr );

	return *this = buffer;
}

//=====================================================================================
CString CString::TrimStart() const
{
	if ( m_Length < 1 )
	{
		return *this;
	}

	char * p = m_String;
	const char * end = m_String + m_Length;

	while ( p != end )
	{
		if ( *p != ' ' && *p != '\t' )
		{
			break;
		}

		++p;
	}

	return SubString( p - m_String, m_Length - ( p - m_String ) );
}

//=====================================================================================
CString CString::TrimEnd() const
{
	if ( m_Length < 1 )
	{
		return *this;
	}

	char * p = m_String + m_Length - 1;
	const char * end = m_String - 1;

	while ( p != end )
	{
		if ( *p != ' ' && *p != '\t' )
		{
			break;
		}

		--p;
	}

	return SubString( 0, p - m_String + 1 );

}

//=====================================================================================
CString CString::Trim() const
{
	CString res = TrimStart();
	res = res.TrimEnd();
	return res;
}

//=====================================================================================
CString::operator WString() const
{
	PROFILE;

	wchar_t * tmp = BAllocate< wchar_t >( ( m_Length + 1 ) * sizeof( wchar_t ) );
	wchar_t * nextWChar = tmp;

	const char * nextChar = m_String;

	while ( *nextChar != '\0' )
	{
		*( nextWChar ++ ) = static_cast< wchar_t >( *( nextChar ++ ) );
	}

	*nextWChar = L'\0';

	WString result = tmp;

	BFree( tmp );

	return result;
}

//=====================================================================================
CString::operator const char *() const
{
	return m_String;
}

//=====================================================================================
CString CString::ToString( int8_t a_Value )
{
	return CString().Format( "%hhd", a_Value );
}

//=====================================================================================
CString CString::ToString( int16_t a_Value )
{
	return CString().Format( "%hd", a_Value );
}

//=====================================================================================
CString CString::ToString( int32_t a_Value )
{
	return CString().Format( "%d", a_Value );
}

//=====================================================================================
CString CString::ToString( int64_t a_Value )
{
	return CString().Format( "%lld", a_Value );
}

//=====================================================================================
CString CString::ToString( uint8_t a_Value )
{
	return CString().Format( "%hhu", a_Value );
}

//=====================================================================================
CString CString::ToString( uint16_t a_Value )
{
	return CString().Format( "%hu", a_Value );
}

//=====================================================================================
CString CString::ToString( uint32_t a_Value )
{
	return CString().Format( "%u", a_Value );
}

//=====================================================================================
CString CString::ToString( uint64_t a_Value )
{
	return CString().Format( "%llu", a_Value );
}

//=====================================================================================
CString CString::ToString( float a_Value )
{
	return CString().Format( "%f", a_Value );
}

//=====================================================================================
CString CString::ToString( double a_Value )
{
	return CString().Format( "%f", a_Value );
}

//=====================================================================================
CString CString::ToString( bool	a_Value )
{
	return a_Value ? "true" : "false";
}

//=====================================================================================
bool CString::Parse( const char * a_Value, int8_t & a_Out )
{
	a_Out = atoi( a_Value );
	return true;
}

//=====================================================================================
bool CString::Parse( const char * a_Value, int16_t & a_Out )
{
	a_Out = atoi( a_Value );
	return true;
}

//=====================================================================================
bool CString::Parse( const char * a_Value, int32_t & a_Out )
{
	a_Out = atoi( a_Value );
	return true;
}

//=====================================================================================
bool CString::Parse( const char * a_Value, int64_t & a_Out )
{
	a_Out = atoll( a_Value );
	return true;
}

//=====================================================================================
bool CString::Parse( const char * a_Value, uint8_t & a_Out )
{
	a_Out = atoi( a_Value );
	return true;
}

//=====================================================================================
bool CString::Parse( const char * a_Value, uint16_t & a_Out )
{
	a_Out = atoi( a_Value );
	return true;
}

//=====================================================================================
bool CString::Parse( const char * a_Value, uint32_t & a_Out, bool a_Base16 )
{
	if ( a_Base16 )
	{
		try
		{
			std::string ss( a_Value );
			uint64_t ul = std::stoul( ss, nullptr, 16 );
			a_Out = static_cast< uint32_t >( ul );
			return true;
		}

		catch ( std::exception e )
		{
			return false;
		}
	}

	a_Out = atoi( a_Value );
	return true;
}

//=====================================================================================
bool CString::Parse( const char * a_Value, uint64_t & a_Out )
{
	a_Out = atoll( a_Value );
	return true;
}

//=====================================================================================
bool CString::Parse( const char * a_Value, bool & a_Out )
{
	PROFILE;

	CString r = a_Value;
	r.ToUpper();

	if ( r == "T" || 
		 r == "F" || 
		 r == "TRUE" || 
		 r == "FALSE" || 
		 r == "Y" || 
		 r == "N" || 
		 r == "YES" || 
		 r == "NO" )
	{
		a_Out = *r.Get() == 'T' || *r.Get() == 'Y';

		return true;
	}

// Allow 0/1 to count as a parseable boolean value
#ifdef CSTRING_PARSEBOOL_ALLOW_INTEGER
	int32_t i;
	bool result = Parse( a_Value, i );

	a_Out = i != 0;

	return result;
#else
	return false;
#endif
}

//=====================================================================================
bool CString::Parse( const char * a_Value, float & a_Out )
{
	try
	{
		a_Out = static_cast< float >( std::stof( a_Value ) );
		return true;
	}

	catch ( std::invalid_argument e )
	{
		return false;
	}
}

//=====================================================================================
bool CString::Parse( const char * a_Value, double & a_Out )
{
	a_Out = atof( a_Value );
	return true;
}