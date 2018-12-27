#pragma once
#ifndef GLSHADERSTORAGEBUFFER_H
#define GLSHADERSTORAGEBUFFER_H

#include "PrecompiledHeaderGL.hpp"

//=====================================================================================
class GLShaderStorageBuffer : public GLObj< GLShaderStorageBuffer >
{
	friend class GLDraw;

public:

	GLShaderStorageBuffer();
	GLShaderStorageBuffer( const char * a_Name, const Array< uint8_t > & a_BufferData );
	GLShaderStorageBuffer( const char * a_Name, uint32_t a_Capacity ); // Dynamic buffer
	~GLShaderStorageBuffer();
	
	void WriteBuffer( const Array< uint8_t > & a_BufferData, uint32_t a_ByteOffset = 0 );
	void WriteBuffer( const void * a_BufferData, uint32_t a_ByteCount, uint32_t a_ByteOffset = 0 );

	inline GLuint GetHandle() const 
	{ 
		return m_SsboHandle;
	}

private:

	GLuint m_SsboHandle;

	// Arbitrarily high count of 128, should be the max 
	// number of bindings across all supported implementations.
	static int32_t m_CurrentSSBOHandles[ 128 ];
};

#endif//GLSHADERSTORAGEBUFFER_H