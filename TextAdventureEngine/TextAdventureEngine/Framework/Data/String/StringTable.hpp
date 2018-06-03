#pragma once
#ifndef STRINGTABLE_H
#define STRINGTABLE_H

#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Containers/StaticArray.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Math/Colour/Colour.hpp>

//=====================================================================================
class StringEntry final
{
	friend class StringTable;
	friend class StringManager;
	friend class Iterator;

private:

	struct Symbol final
	{
		ENUM( Type, uint8_t )
			TOKEN,
			FORMAT,
			STRING_DATA,
		END_ENUM;

		Type Type;

		ENUM( FormatType, uint8_t )
			FONTSIZE,
			FONTCOLOUR,
			BOLD,
			UNDERLINE,
			ITALICS,
			STRIKETHROUGH,
			SUPERSCRIPT,
			SUBSCRIPT,
			EXTENDED_FORMAT,
		END_ENUM;

		typedef uint32_t ExtendedFormatType;

		struct FormatParameter
		{
			float	Number	= 0.0F;
			bool	Boolean = false;
			Colour	Colour	= Colour::BLACK;
			CString String	= "";
		};

		CString				StringData_String;
		uint32_t			Token_StringID;
		FormatType			Format_Type;
		ExtendedFormatType	Format_ExtendedType;
		bool				Format_IsPush;
		FormatParameter		Format_Parameter;

		Symbol()
		{

		}

		Symbol( const Symbol & a_Other )
		{
			*this = a_Other;
		}

		Symbol & operator=( const Symbol & a_Other )
		{
			Type = a_Other.Type;
			switch ( Type )
			{
			case StringEntry::Symbol::TOKEN:
				Token_StringID = a_Other.Token_StringID;
				break;
			case StringEntry::Symbol::FORMAT:
				Format_Type = a_Other.Format_Type;
				Format_Parameter = a_Other.Format_Parameter;
				Format_IsPush = a_Other.Format_IsPush;
				break;
			case StringEntry::Symbol::STRING_DATA:
				StringData_String = a_Other.StringData_String;
				break;
			}

			return *this;
		}
	};

	StaticArray< Symbol, 16 > Symbols;

public:

	CString RawString;

	StringEntry()
	{}

	StringEntry( const char * a_String )
		: RawString( a_String )
	{}

	bool operator==( const StringEntry & a_Other ) const
	{
		return RawString == a_Other.RawString;
	}

	typedef Action< void, StringEntry::Symbol::FormatType /*a_FormatType*/, StringEntry::Symbol::ExtendedFormatType /*a_ExtendedFormatType*/, const StringEntry::Symbol::FormatParameter & /*a_FormatParameter*/ > FormatPushHandler;
	typedef Action< void, StringEntry::Symbol::FormatType /*a_FormatType*/, StringEntry::Symbol::ExtendedFormatType /*a_ExtendedFormatType*/ > FormatPopHandler;

	//iterator to iterate over each token
	class Iterator final
	{
		friend class StringEntry;

	private:

		Iterator( const Symbol * a_Begin, const Symbol * a_End, FormatPushHandler a_FormatPushHandler, FormatPopHandler a_FormatPopHandler )
			: m_It( a_Begin )
			, m_End( a_End )
			, m_FormatPushHandler( a_FormatPushHandler )
			, m_FormatPopHandler( a_FormatPopHandler )
		{
			ASSERT( m_It && m_End && m_It < m_End, "StringEntry::Iterator invalid Array of StringEntry::Symbol" );
		}

		const Symbol *			m_It;
		const Symbol * const	m_End;
		FormatPushHandler		m_FormatPushHandler;
		FormatPopHandler		m_FormatPopHandler;

	public:
		
		operator bool() const
		{
			return m_It != m_End;
		}

		void Handle()
		{
			if ( *this )
			{
				if ( m_It->Type == Symbol::Type::FORMAT )
				{
					Symbol::FormatType ftype = m_It->Format_Type;
					Symbol::ExtendedFormatType eftype = 0;
					if ( ftype == Symbol::FormatType::EXTENDED_FORMAT )
					{
						eftype = m_It->Format_ExtendedType;
					}

					if ( m_It->Format_IsPush )
					{
						if ( m_FormatPushHandler )
						{
							m_FormatPushHandler( ftype, eftype, m_It->Format_Parameter );
						}
					}

					else
					{
						if ( m_FormatPopHandler )
						{
							m_FormatPopHandler( ftype, eftype );
						}
					}
				}
			}
		}

		Iterator & operator++()
		{
			if ( *this ) 
			{
				++m_It;
			}

			return *this;
		}
	};

	Iterator GetIterator( FormatPushHandler a_FormatPushHandler, FormatPopHandler a_FormatPopHandler ) const
	{
		return Iterator( Symbols.First(), Symbols.End(), a_FormatPushHandler, a_FormatPopHandler );
	}
};

//=====================================================================================
class StringTable final
{
	friend class StringManager;

public:

	StringTable();
	void PutEntry( uint32_t a_Hash, const char * a_String );
	uint32_t GetMemoryFootprint() const;
	const StringEntry & operator[]( uint32_t a_Hash ) const;
	static const StringEntry NULL_STRING_ENTRY;

private:

	static StringEntry Process( const char * a_String );
	uint32_t m_MemoryFootPrint;
	Dictionary< uint32_t, StringEntry > m_Entries;
};

#endif//STRINGTABLE_H