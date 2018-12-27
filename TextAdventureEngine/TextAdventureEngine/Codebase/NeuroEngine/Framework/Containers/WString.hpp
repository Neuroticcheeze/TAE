#pragma once
#ifndef WSTRING_H
#define WSTRING_H

//=====================================================================================
class CString;

//=====================================================================================
class WString final
{
public:

	WString();
	WString( const WString & a_Other );
	WString( const wchar_t * a_WCharStr );
	~WString();

	WString & operator=( const WString & a_Other );
	WString & operator=( const wchar_t * a_WCharStr );
	WString & operator+=( const WString & a_Other );
	WString & operator+=( const wchar_t * a_WCharStr );

	bool operator==( const WString & a_Other ) const;
	bool operator==( const wchar_t * a_WCharStr) const;

	bool operator!=( const WString & a_Other ) const;
	bool operator!=( const wchar_t * a_WCharStr) const;

	friend
	WString operator+( const WString & a_Left, const WString & a_Right );

	friend
	WString operator+( const WString & a_Left, const wchar_t * a_Right );

	/* Get a const pointer to the underlying wide-character string. */
	const wchar_t * Get() const;

	/* Get the length of the wide-character string. */
	uint32_t Length() const;

	/* Get a substring of this. If the end of the substring exceeds the end of this string, the end of the substring gets truncated to fit. */
	WString SubString( uint32_t a_Start, uint32_t a_Length ) const;

	/* Return the index of the first occurrence of the substring, or -1 if not found. */
	int32_t Find( const WString & a_SubString ) const;

	/* Replaces the first occurence of "from" with "to" inside this string. */
	void Replace( const WString & a_From, const WString & a_To );

	/* Changes all letters to their uppercase equivalent. */
	void ToUpper() const;

	/* Changes all letters to their lowercase equivalent. */
	void ToLower() const;

	explicit operator CString() const;
	explicit operator const wchar_t *() const;

private:

	wchar_t * m_String;
	uint32_t m_Length;
	uint32_t m_Capacity;
};

#endif//WSTRING_H