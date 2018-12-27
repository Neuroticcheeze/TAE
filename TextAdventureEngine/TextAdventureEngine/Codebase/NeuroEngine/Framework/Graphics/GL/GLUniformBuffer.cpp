#include "GLUniformBuffer.hpp"
#include "GLDraw.hpp"

//=====================================================================================
GLint GLUniformBuffer::m_CurrentUBOHandles[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

//=====================================================================================
GLUniformBuffer::GLUniformBuffer()
	: GLObj( "<Unnamed>" )
	, m_UboHandle( 0 )
{
}

//=====================================================================================
GLUniformBuffer::GLUniformBuffer( const char * a_Name, const Array< uint8_t > & a_BufferData )
	: GLObj( a_Name )
	, m_UboHandle( 0 )
{
	GLint maxSsboSize; 
	glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &maxSsboSize );


	if ( a_BufferData.Count() > ( uint32_t )maxSsboSize )
	{
		ASSERT_WARN( false, "Failed to construct UniformBuffer, max buffer size allowed is %u bytes!", maxSsboSize );
		return;
	}
	
	glGenBuffers( 1, &m_UboHandle );
	glBindBuffer( GL_UNIFORM_BUFFER, m_UboHandle );
	glBufferData( GL_UNIFORM_BUFFER, a_BufferData.Count(), a_BufferData.First(), GL_DYNAMIC_DRAW );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );
	Validate();

}

//=====================================================================================
GLUniformBuffer::GLUniformBuffer( const char * a_Name, uint32_t a_Capacity )
	: GLObj( a_Name )
	, m_UboHandle( 0 )
{
	GLint maxUboSize;
	glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &maxUboSize );


	if ( a_Capacity > ( uint32_t )maxUboSize )
	{
		ASSERT_WARN( false, "Failed to construct UniformBuffer, max buffer size allowed is %u bytes!", maxUboSize );
		return;
	}
	
	glGenBuffers( 1, &m_UboHandle );
	glBindBuffer( GL_UNIFORM_BUFFER, m_UboHandle );
	glBufferData( GL_UNIFORM_BUFFER, a_Capacity, nullptr, GL_STATIC_DRAW );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );
	Validate();
}

//=====================================================================================
GLUniformBuffer::~GLUniformBuffer()
{
	GLint binding = 0;
	glBindBuffer( GL_UNIFORM_BUFFER, m_UboHandle );
	glGetBufferParameteriv( GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING, &binding );
	m_CurrentUBOHandles[ binding ] = 0;

	glDeleteBuffers( 1, &m_UboHandle );
}

//=====================================================================================
void GLUniformBuffer::WriteBuffer( const Array< uint8_t > & a_BufferData, uint32_t a_ByteOffset )
{
	glBindBuffer( GL_UNIFORM_BUFFER, m_UboHandle );
	glBufferSubData( GL_UNIFORM_BUFFER, a_ByteOffset, a_BufferData.Count(), a_BufferData.First() );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );
}

//=====================================================================================
void GLUniformBuffer::WriteBuffer( const void * a_BufferData, uint32_t a_ByteCount, uint32_t a_ByteOffset )
{
	glBindBuffer( GL_UNIFORM_BUFFER, m_UboHandle );
	glBufferSubData( GL_UNIFORM_BUFFER, a_ByteOffset, a_ByteCount, a_BufferData );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );
}