#pragma once
#ifndef GLUNIFORMBUFFER_H
#define GLUNIFORMBUFFER_H

#include "PrecompiledHeaderGL.hpp"

//=====================================================================================
class GLUniformBuffer : public GLObj< GLUniformBuffer >
{
	friend class GLDraw;

public:

	GLUniformBuffer();
	GLUniformBuffer( const char * a_Name, const Array< uint8_t > & a_BufferData );
	GLUniformBuffer( const char * a_Name, uint32_t a_Capacity ); // Dynamic buffer
	~GLUniformBuffer();
	
	void WriteBuffer( const Array< uint8_t > & a_BufferData, uint32_t a_ByteOffset = 0 );
	void WriteBuffer( const void * a_BufferData, uint32_t a_ByteCount, uint32_t a_ByteOffset = 0 );

	inline GLuint GetHandle() const 
	{ 
		return m_UboHandle;
	}

private:

	GLuint m_UboHandle;

	// Arbitrarily high count of 128, should be the max 
	// number of bindings across all supported implementations.
	static int32_t m_CurrentUBOHandles[ 128 ];
};

#endif//GLUNIFORMBUFFER_H