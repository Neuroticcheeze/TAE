#include "WString.hpp"

#include <Framework/Utils/MemoryOps.hpp>
#include <Framework/Math/Math.hpp>
#include <Framework/Containers/CString.hpp>

#define WIDE( T ) T * sizeof( wchar_t )

//=====================================================================================
WString::WString()
	: m_String( nullptr )
	, m_Length( 0 )
	, m_Capacity( 0 )
{

}

//=====================================================================================
WString::WString( const WString & a_Other )
	: m_String( nullptr )
	, m_Length( 0 )
	, m_Capacity( 0 )
{
	*this = a_Other;
}

//=====================================================================================
WString::WString( const wchar_t * a_WCharStr )
	: m_String( nullptr )
	, m_Length( 0 )
	, m_Capacity( 0 )
{
	*this = a_WCharStr;
}

//=====================================================================================
WString::~WString()
{
	BFree( m_String );
	m_Capacity = 0;
	m_Length = 0;
}

//=====================================================================================
WString & WString::operator=( const WString & a_Other )
{
	if ( m_String )
	{
		BFree( m_String );
	}

	m_Length = a_Other.m_Length;
	m_Capacity = a_Other.m_Capacity;

	if ( a_Other.m_String )
	{
		m_String = BAllocate< wchar_t >( WIDE( m_Capacity ) );
		BCopy( a_Other.m_String, m_String, WIDE( m_Length ) );
		m_String[ m_Length ] = L'\0';
	}

	return *this;
}

//=====================================================================================
WString & WString::operator=( const wchar_t * a_WCharStr)
{
	if ( m_String )
	{
		BFree( m_String );
	}

	if ( a_WCharStr )
	{
		m_Length = 0;

		const wchar_t * nextChar = a_WCharStr;

		while ( *nextChar != L'\0' )
		{
			++m_Length;
			++nextChar;
		}
		
		m_Capacity = NextPowerOf2( m_Length + 1 );
		m_String = BAllocate< wchar_t >( WIDE( m_Capacity ) );
		BCopy( a_WCharStr, m_String, WIDE( m_Length ) );
		m_String[ m_Length ] = L'\0';
	}

	else
	{
		m_Capacity = 0;
		m_Length = 0;
	}

	return *this;
}

//=====================================================================================
WString & WString::operator+=( const WString & a_Other )
{
	return *this += a_Other.m_String;
}

//=====================================================================================
WString & WString::operator+=( const wchar_t * a_WCharStr )
{
	if ( a_WCharStr )
	{
		uint32_t length = 0;

		const wchar_t * nextChar = a_WCharStr;

		while ( *nextChar != L'\0' )
		{
			++length;
			++nextChar;
		}

		uint32_t oldLength = m_Length;
		wchar_t * oldString = m_String;

		m_Length += length;
		m_Capacity = NextPowerOf2( m_Length + 1 );

		m_String = BAllocate< wchar_t >( WIDE( m_Capacity ) );

		// copy part 1
		if ( oldString )
		{
			BCopy( oldString, m_String, WIDE( oldLength ) );
		}

		// copy part 2
		BCopy( a_WCharStr, m_String + oldLength, WIDE( length ) );
		m_String[ m_Length ] = L'\0';

		if ( oldString )
		{
			BFree( oldString );
		}
	}

	return *this;
}

//=====================================================================================
bool WString::operator==( const WString & a_Other ) const
{
	return *this == a_Other.m_String;
}

//=====================================================================================
bool WString::operator==( const wchar_t * a_WCharStr ) const
{
	if ( a_WCharStr == m_String )
	{
		return true;
	}

	if ( a_WCharStr == nullptr || m_String == nullptr )
	{
		return false;
	}

	return wcscmp( a_WCharStr, m_String ) == 0;
}

//=====================================================================================
bool WString::operator!=(const WString & a_Other) const
{
	return *this != a_Other.m_String;
}

//=====================================================================================
bool WString::operator!=(const wchar_t * a_WCharStr) const
{
	return !( *this == a_WCharStr);
}

//=====================================================================================
WString operator+( const WString & a_Left, const WString & a_Right )
{
	WString result = a_Left;
	return result += a_Right;
}

//=====================================================================================
WString operator+( const WString & a_Left, const wchar_t * a_Right )
{
	WString result = a_Left;
	return result += a_Right;
}

//=====================================================================================
const wchar_t * WString::Get() const
{
	return m_String == nullptr ? L"" : m_String;
}

//=====================================================================================
uint32_t WString::Length() const
{
	return m_Length;
}

//=====================================================================================
WString WString::SubString( uint32_t a_Start, uint32_t a_Length ) const
{
	if ( a_Start >= m_Length || a_Length == 0 )
	{
		return WString();
	}
	
	uint32_t end = a_Start + a_Length;

	if ( end > m_Length )
	{
		end = m_Length;
	}

	wchar_t prevChar = m_String[ end ];
	m_String[ end ] = L'\0';
	WString result( m_String + a_Start );
	m_String[ end ] = prevChar;

	return result;
}

//=====================================================================================
int32_t WString::Find( const WString & a_SubString ) const
{
	const wchar_t * ssFirstChar = &( ( const wchar_t * ) a_SubString )[ 0 ];
	const wchar_t * ssCurrentChar;
	uint32_t ssLen = a_SubString.Length();

	const wchar_t * nextChar = m_String;

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

				wchar_t ss = *( ssCurrentChar + s );
				wchar_t tc = *( nextChar + s );

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
void WString::Replace( const WString & a_From, const WString & a_To )
{
	int32_t start = Find( a_From );
	int32_t length = a_From.Length();

	if ( start != -1 )
	{
		WString pre = SubString( 0, start);
		WString post = SubString( start + length, m_Length - ( start + length ) );

		*this = pre + a_To + post;
	}
}

//=====================================================================================
void WString::ToUpper() const
{
	wchar_t * nextChar = m_String;
	
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
void WString::ToLower() const
{
	wchar_t * nextChar = m_String;
	
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
WString::operator CString() const
{
	char * tmp = BAllocate< char >( ( m_Length + 1 ) * sizeof( char ) );
	char * nextChar = tmp;

	const wchar_t * nextWChar = m_String;

	while ( *nextWChar != L'\0' )
	{
		*( nextChar ++ ) = static_cast< char >( *( nextWChar ++ ) );
	}

	*nextChar = '\0';

	CString result = tmp;

	BFree( tmp );

	return result;
}

//=====================================================================================
WString::operator const wchar_t *() const
{
	return m_String;
}

#undef WIDE