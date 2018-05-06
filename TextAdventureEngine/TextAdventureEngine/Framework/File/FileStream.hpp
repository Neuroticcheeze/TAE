#pragma once
#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <Framework/Containers/DataStream.hpp>
#include <Framework/Containers/CString.hpp>

//=====================================================================================
class FileStream : public DataStream
{
	friend class FileManager;
	void * m_FileHandle;

public:
	FileStream() : DataStream() {}
	FileStream( void * a_Buffer, uint32_t a_BufferLength ) : DataStream( a_Buffer, a_BufferLength ) {}

	void WriteText( const char * a_Text )
	{
		CString str = a_Text;
		WriteRaw( str.Length(), str.Get() );
	}

	CString ReadText( uint32_t a_MaxLength = 4096 )
	{
		if ( a_MaxLength > 0 )
		{
			char * str = new char[ a_MaxLength ];

			uint32_t curr = Tell();
			uint32_t end = Seek( 0, SeekMode::END );
			Seek( curr, SeekMode::BEGIN );
			uint32_t len = Min( a_MaxLength, end - curr );

			ReadRaw( len, str );

			str[ len ] = '\0';

			CString result = str;

			delete[] str;

			return result;
		}

		return "";
	}
};

#endif//FILESTREAM_H