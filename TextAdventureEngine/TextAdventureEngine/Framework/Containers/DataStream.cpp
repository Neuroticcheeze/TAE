#include "DataStream.hpp"

#include <Framework/Utils/MemoryOps.hpp>

//=====================================================================================
DataStream::DataStream()
	: m_Begin( nullptr )
	, m_End( nullptr )
	, m_Pointer( nullptr )
{
}

//=====================================================================================
DataStream::DataStream( void * a_Buffer, uint32_t a_BufferLength ) : DataStream()
{
	if ( a_Buffer != nullptr )
	{
		m_Begin = reinterpret_cast< uint8_t * >( a_Buffer );
		m_End = m_Begin + a_BufferLength;
		m_Pointer = m_Begin;
	}
}

//=====================================================================================
DataStream::DataStream( const DataStream & a_Other )
{
	*this = a_Other;
}

//=====================================================================================
DataStream & DataStream::operator=( const DataStream & a_Other )
{
	m_Begin = a_Other.m_Begin;
	m_End = a_Other.m_End;
	m_Pointer = a_Other.m_Pointer;

	return *this;
}

//=====================================================================================
bool DataStream::operator==( const DataStream & a_Other ) const
{
	return m_Begin == a_Other.m_Begin && m_End == a_Other.m_End;
}

//=====================================================================================
bool DataStream::operator==( const void * a_Buffer ) const
{
	return m_Begin == a_Buffer;
}

//=====================================================================================
bool DataStream::operator!=( const DataStream & a_Other ) const
{
	return ! ( *this == a_Other );
}

//=====================================================================================
bool DataStream::operator!=( const void * a_Buffer ) const
{
	return ! ( *this == a_Buffer );
}

//=====================================================================================
uint32_t DataStream::Tell() const
{
	return static_cast< uint32_t >( m_Pointer - m_Begin );
}

//=====================================================================================
uint32_t DataStream::Seek( int32_t a_Offset, SeekMode a_SeekMode )
{
	switch( a_SeekMode )
	{
	case SeekMode::BEGIN:
		m_Pointer = m_Begin + a_Offset;
		break;

	case SeekMode::CURRENT:
		m_Pointer += a_Offset;
		break;

	case SeekMode::END:
		m_Pointer = m_End + a_Offset;
		break;
	}

	return Tell();
}

//=====================================================================================
uint32_t DataStream::ReadRaw( uint32_t a_Size, void * a_Out )
{
	if ( !m_Begin )
	{
		return 0;
	}

	BCopy( m_Pointer, a_Out, a_Size );
	m_Pointer += a_Size;
	return a_Size;
}

//=====================================================================================
uint32_t DataStream::WriteRaw( uint32_t a_Size, const void * a_In )
{
	if ( !m_Begin )
	{
		return 0;
	}

	BCopy( a_In, reinterpret_cast< void* >( m_Pointer ), a_Size );
	m_Pointer += a_Size;
	return a_Size;
}