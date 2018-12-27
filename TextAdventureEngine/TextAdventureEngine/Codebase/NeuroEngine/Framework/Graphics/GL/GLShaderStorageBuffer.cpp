#include "GLShaderStorageBuffer.hpp"
#include "GLDraw.hpp"

//=====================================================================================
GLint GLShaderStorageBuffer::m_CurrentSSBOHandles[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

//=====================================================================================
GLShaderStorageBuffer::GLShaderStorageBuffer()
	: GLObj( "<Unnamed>" )
	, m_SsboHandle( 0 )
{
}

//=====================================================================================
GLShaderStorageBuffer::GLShaderStorageBuffer( const char * a_Name, const Array< uint8_t > & a_BufferData )
	: GLObj( a_Name )
	, m_SsboHandle( 0 )
{
	GLint maxSsboSize; 
	glGetIntegerv( GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSsboSize );


	if ( a_BufferData.Count() > ( uint32_t )maxSsboSize )
	{
		ASSERT_WARN( false, "Failed to construct ShaderStorageBuffer, max buffer size allowed is %u bytes!", maxSsboSize );
		return;
	}
	
	glGenBuffers( 1, &m_SsboHandle );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_SsboHandle );
	glBufferData( GL_SHADER_STORAGE_BUFFER, a_BufferData.Count(), a_BufferData.First(), GL_DYNAMIC_DRAW );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );
	Validate();

}

//=====================================================================================
GLShaderStorageBuffer::GLShaderStorageBuffer( const char * a_Name, uint32_t a_Capacity )
	: GLObj( a_Name )
	, m_SsboHandle( 0 )
{
	GLint maxSsboSize; 
	glGetIntegerv( GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSsboSize );


	if ( a_Capacity > ( uint32_t )maxSsboSize )
	{
		ASSERT_WARN( false, "Failed to construct ShaderStorageBuffer, max buffer size allowed is %u bytes!", maxSsboSize );
		return;
	}
	
	glGenBuffers( 1, &m_SsboHandle );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_SsboHandle );
	glBufferData( GL_SHADER_STORAGE_BUFFER, a_Capacity, nullptr, GL_STATIC_DRAW );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );
	Validate();
}

//=====================================================================================
GLShaderStorageBuffer::~GLShaderStorageBuffer()
{
	GLint binding = 0;
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_SsboHandle );
	glGetBufferParameteriv( GL_SHADER_STORAGE_BUFFER, GL_SHADER_STORAGE_BUFFER_BINDING, &binding );
	m_CurrentSSBOHandles[ binding ] = 0;

	glDeleteBuffers( 1, &m_SsboHandle );
}

//=====================================================================================
void GLShaderStorageBuffer::WriteBuffer( const Array< uint8_t > & a_BufferData, uint32_t a_ByteOffset )
{
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_SsboHandle );
	glBufferSubData( GL_SHADER_STORAGE_BUFFER, a_ByteOffset, a_BufferData.Count(), a_BufferData.First() );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );
}

//=====================================================================================
void GLShaderStorageBuffer::WriteBuffer( const void * a_BufferData, uint32_t a_ByteCount, uint32_t a_ByteOffset )
{
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, m_SsboHandle );
	glBufferSubData( GL_SHADER_STORAGE_BUFFER, a_ByteOffset, a_ByteCount, a_BufferData );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );
}