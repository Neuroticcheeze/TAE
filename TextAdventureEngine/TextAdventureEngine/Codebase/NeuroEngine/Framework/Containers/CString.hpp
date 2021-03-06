#pragma once
#ifndef CSTRING_H
#define CSTRING_H

#include <Framework/Containers/Array.hpp>
#include <Framework/Template/Serialisable.hpp>
#include <Framework/Template/StringRepresentable.hpp>
#include <Framework/Utils/InitialiserList.hpp>

//=====================================================================================
class WString;

//=====================================================================================
//!	\brief A string container object for narrow characters.
//!		   Supports Multibyte strings - Which can be converted from Unicode or UTF-8 strings.
class CString final : public Serialisable< CString >
{
public:

	struct RegexMatch;

	//!	\brief Default Constructor.
	//!		   Constructs an empty CString with no allocation footprint in memory.
	CString();

	//!	\brief Copy Constructor.
	//!		   Constructs this CString from another.
	//!	\param a_Other The CString to copy.
	//!	\sa CString::operator=
	CString( const CString & a_Other );

	//!	\brief Constructs a CString from a raw narrow char string.
	//!	\param a_CharStr a const char pointer that is a char string.
	//! \sa CString::operator=(const char* a_CharStr)
	CString( const char * a_CharStr );
	
	//!	\brief Constructs a CString from a raw narrow char string between two addresses.
	//!	\param a_Begin a const char pointer that starts the char string.
	//!	\param a_End a const char pointer that ends the char string.
	CString( const char * a_Begin, const char * a_End );

	//!	\brief Constructs a CString from a pattern string and the number to repeat it by.
	//!		   Equivalent to:
	//!		   for ( int i = 0; i < a_Repeat; ++i )
	//!			   *this += a_String;
	//!	\param a_String a const char pointer that is the char string pattern to be repeated.
	//!	\param a_Repeat the number of times to repeat a_String by.
	CString( const char * a_String, uint32_t a_Repeat );

	//! \brief Deallocates underlying memory footprint and gives us a capacity and length of 0
	~CString();

	//!	\brief Write this string to a DataStream.
	//! \param a_Writer The DataStream to write into.
	void Write( DataStream & a_Writer ) const;

	//!	\brief Read this string from a DataStream.
	//! \param a_Reader The DataStream to read from.
	void Read( DataStream & a_Reader );

	//!	\brief Clone another CString into this CString.
	//!	\param a_Other A CString to copy.
	//!	\return Returns a reference to this CString object.
	CString & operator=( const CString & a_Other );

	//!	\brief Copy in a raw char string from a const char pointer.
	//!	\param a_CharStr a const char pointer that is a char string.
	//!	\return Returns a reference to this CString object.
	CString & operator=( const char * a_CharStr );

	//!	\brief Append the contents of another CString onto this CString.
	//!	\param a_Other A CString to append.
	//!	\return Returns a reference to this CString object.
	CString & operator+=( const CString & a_Other );

	//!	\brief Append a raw char string onto this CString.
	//!	\param a_CharStr A raw char string to append.
	//!	\return Returns a reference to this CString object.
	CString & operator+=( const char * a_CharStr );

	
	//!	\brief Append a single character onto this CString.
	//!	\param a_CharStr A character to append.
	//!	\return Returns a reference to this CString object.
	CString & operator+=( char a_Char );

	//! \brief Equate this to another CString
	//!	\param a_Other A CString object to compare.
	//!	\return Returns whether or not this string contains 
	//			the same number of characters than the specified string.
	bool operator==( const CString & a_Other ) const;
	bool operator==( const char * a_CharStr ) const;

	bool operator!=( const CString & a_Other ) const;
	bool operator!=( const char * a_CharStr ) const;

	inline char & operator[]( uint32_t	a_Index ) const
	{
		return m_String[ a_Index ];
	}

	friend
	CString operator+( const CString & a_Left, const CString & a_Right );

	friend
	CString operator+( const CString & a_Left, const char * a_Right );

	friend
	CString operator+( const CString & a_Left, char a_Right );

	/* Get a const pointer to the underlying character string. */
	const char * Get() const;

	/* Get the length of the character string. */
	uint32_t Length() const;

	/* Get a substring of this. If the end of the substring exceeds the end of this string, the end of the substring gets truncated to fit. */
	CString SubString( uint32_t a_Start, uint32_t a_Length ) const;

	/* Return the index of the first occurrence of the substring, or -1 if not found. */
	int32_t Find( const CString & a_SubString, uint32_t a_StartIndex = 0 ) const;

	/* Return the index of the last occurrence of the substring, or -1 if not found. */
	int32_t FindLast( const CString & a_SubString, uint32_t a_StartBackIndex = UINT_MAX ) const;

	/* Return the index of all occurrences of the substring. */
	Array< int32_t > FindAll( const CString & a_SubString ) const;

	/* Return the index of all occurrences of any substring specified. */
	Array< int32_t > FindAll( InitialiserList< const char * > a_SubStrings ) const;

	/* Replaces the first occurence of "from" with "to" inside this string. */
	void Replace( const CString & a_From, const CString & a_To );

	/* Replaces all occurences of "from" with "to" inside this string. */
	void ReplaceAll( const CString & a_From, const CString & a_To );

	/* Splits this string up using the delimeter. */
	Array< CString > Split( const CString & a_Delimiter ) const;

	/* Splits this string up using any of the specified delimeters. */
	Array< CString > Split( const char ** a_Delimiters, uint32_t a_NumDelimeters ) const;
	
	/* Search for and return all string segments that satisfy the regular expression. */
	Array< RegexMatch > Regex( const char * a_Pattern ) const;

	/* Changes all letters to their uppercase equivalent. */
	void ToUpper() const;

	/* Changes all letters to their lowercase equivalent. */
	void ToLower() const;

	/* Build a string with a format and arguments. */
	static CString Format( const char * a_Format, ... );

	CString TrimStart() const;
	CString TrimEnd() const;
	CString Trim() const;

	static CString ToString( int8_t   a_Value );
	static CString ToString( int16_t  a_Value );
	static CString ToString( int32_t  a_Value );
	static CString ToString( int64_t  a_Value );
	static CString ToString( uint8_t  a_Value );
	static CString ToString( uint16_t a_Value );
	static CString ToString( uint32_t a_Value );
	static CString ToString( uint64_t a_Value );
	static CString ToString( float a_Value );
	static CString ToString( double a_Value );
	static CString ToString( bool	  a_Value );
	
	template< typename T >
	static CString ToString( StringRepresentable< T > a_Value )
	{
		return a_Value.ToString();
	}

	static bool Parse( const char * a_Value, int8_t & a_Out );
	static bool Parse( const char * a_Value, int16_t & a_Out );
	static bool Parse( const char * a_Value, int32_t & a_Out );
	static bool Parse( const char * a_Value, int64_t & a_Out );
	static bool Parse( const char * a_Value, uint8_t & a_Out );
	static bool Parse( const char * a_Value, uint16_t & a_Out );
	static bool Parse( const char * a_Value, uint32_t & a_Out, bool a_Base16 = false );
	static bool Parse( const char * a_Value, uint64_t & a_Out );
	static bool Parse( const char * a_Value, float & a_Out );
	static bool Parse( const char * a_Value, double & a_Out );
	static bool Parse( const char * a_Value, bool & a_Out );

	explicit operator WString() const;
	explicit operator const char *() const;

private:

	char * m_String;
	uint32_t m_Length;
	uint32_t m_Capacity;
};

//=====================================================================================
//!	\brief A struct used in regex matching. Stores the start of the match in 
//!		   our string, and the length of the match.
struct CString::RegexMatch final
{
	uint32_t Start;
	uint32_t Length;
	Array< CString > GroupCaptures;
};

#endif//CSTRING_H