#pragma once
#ifndef DATASTREAM_H
#define DATASTREAM_H

#include <Framework/Template/Serialisable.hpp>

#include <type_traits>

//=====================================================================================
class DataStream
{
public:

	enum class SeekMode
	{
		BEGIN,
		CURRENT,
		END
	};

	DataStream();
	DataStream( void * a_Buffer, uint32_t a_BufferLength );
	DataStream( const DataStream & a_Other );

	DataStream & operator=( const DataStream & a_Other );
	bool operator==( const DataStream & a_Other ) const;
	bool operator==( const void * a_Buffer ) const;
	bool operator!=( const DataStream & a_Other ) const;
	bool operator!=( const void * a_Buffer ) const;

	uint32_t Tell() const;
	uint32_t Seek( int32_t a_Offset, SeekMode a_SeekMode = SeekMode::BEGIN );

	uint32_t ReadRaw( uint32_t a_Size, void * a_Out );
	uint32_t WriteRaw( uint32_t a_Size, const void * a_In );

	template< typename T >
	uint32_t Read( T & a_Out )
	{
		return ReadRaw( sizeof( T ), &a_Out );
	}
	
	template< typename T >
	uint32_t Write( const T & a_In )
	{
		return WriteRaw( sizeof( T ), &a_In );
	}

	template< typename T >
	uint32_t ReadS( Serialisable< T > & a_Out )
	{
		uint32_t t = Tell();
		
		a_Out.Read( *this );
		
		return Tell() - t;
	}
	
	template< typename T >
	uint32_t WriteS( const Serialisable< T > & a_In )
	{
		uint32_t t = Tell();
		
		a_In.Write( *this );
		
		return Tell() - t;
	}

private:

	uint8_t * m_Begin;
	uint8_t * m_End;
	uint8_t * m_Pointer;
};

#endif//DATASTREAM_H