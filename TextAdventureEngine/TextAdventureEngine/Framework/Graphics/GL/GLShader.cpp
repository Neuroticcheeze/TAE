#include "GLShader.hpp"
#include <Framework/Containers/Set.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Utils/Hash.hpp>
#include "GLTexture.hpp"

//=====================================================================================
GLint GLShader::m_CurrentShaderHandle = 0;

//=====================================================================================
GLShader::GLShader()
	: GLObj( "<Unnamed>" )
	, m_Handle( 0 )
{
}

//=====================================================================================
GLShader::GLShader( const char * a_Name, const InitialiserList< Source > & a_ShaderSources )
	: GLObj( a_Name )
	, m_Handle( 0 )
{
	Set< GLint > shaders;
	auto it = NBegin( a_ShaderSources );
	const auto end = NEnd( a_ShaderSources );

	bool failed = false;
	while ( it != end )
	{
		GLint shader = glCreateShader( it->Type );
		GLint length = static_cast< GLint >( it->SourceGLSL.Length() );
		const char * src = it->SourceGLSL.Get();
		glShaderSource( shader, 1, &src, &length );
		glCompileShader( shader );

		GLint success = 0;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
		if ( success == GL_FALSE )
		{
			Array< GLchar > errorLog;
			GLint maxLength = 0;
			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
			errorLog.Resize( maxLength, '\0' );
			glGetShaderInfoLog( shader, maxLength, &maxLength, errorLog.First() );
			glDeleteShader( shader );

			PRINT( "Shader Error: [COMPILING] %s", errorLog.First() );

			failed = true;
		}

		else
		{
			shaders.Insert( shader );
		}

		++it;
	}

	if ( failed )
	{
		auto it = shaders.First();
		const auto end = shaders.End();
		while ( it != end )
		{
			glDeleteShader( *it );
			++it;
		}
	}

	else
	{
		GLint program = glCreateProgram();

		const GLint * end = shaders.End();
		GLint * it;

		it = shaders.First();
		while ( it != end )
		{
			glAttachShader( program, *it );
			++it;
		}

		glLinkProgram( program );

		GLint success = 0;
		glGetProgramiv( program, GL_LINK_STATUS, &success );
		if ( success == GL_FALSE)
		{
			Array< GLchar > errorLog;
			GLint maxLength = 0;
			glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );
			errorLog.Resize( maxLength, '\0' );
			glGetProgramInfoLog( program, maxLength, &maxLength, errorLog.First() );
			glDeleteProgram( program );
			
			PRINT( "Shader Error: [LINKING] %s", errorLog.First() );

		}

		else
		{
			m_Handle = program;
			Validate();
			PRINT( "Compiled Shader \"%s\" Successfully.", Name() );
		}

		it = shaders.First();
		while ( it != end )
		{
			glDetachShader( program, *it );
			glDeleteShader( *it );
			++it;
		}
	}
}

//=====================================================================================
GLShader::~GLShader()
{
	GLOBJ_RELEASE;
	glDeleteProgram( m_Handle );
	m_Handle = 0;
}

//=====================================================================================
int32_t GLShader::GetUniform( const char * a_UniformName )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}

	GLint loc = 0;
	uint32_t wsid = WSID( a_UniformName );
	if ( m_Locations.Contains( wsid ) )
		 loc = *m_Locations[ wsid ];
	else 
	{
		loc = glGetUniformLocation( m_Handle, a_UniformName );
		if ( ASSERT_WARN( loc != -1, "Shader Error: [UNIFORM] %s (There is no uniform called \"%s\")", Name(), a_UniformName ) )
		{
			m_Locations.Put( wsid, loc );
		}
	}

	return static_cast< int32_t >( loc );
}

//=====================================================================================
void GLShader::SetUniformInt32( int32_t a_Loc, int32_t a_Value )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}

	glUniform1i( a_Loc, a_Value );
}

//=====================================================================================
void GLShader::SetUniformFloat1( int32_t a_Loc, float a_Value )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}

	glUniform1f( a_Loc, a_Value );
}

//=====================================================================================
void GLShader::SetUniformFloat2( int32_t a_Loc, const Vector2 & a_Value )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}

	glUniform2f( a_Loc, a_Value.x, a_Value.y );
}

//=====================================================================================
void GLShader::SetUniformFloat3( int32_t a_Loc, const float * a_ValueFloat3 )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}

	glUniform3fv( a_Loc, 1, a_ValueFloat3 );
}

//=====================================================================================
void GLShader::SetUniformFloat3x3( int32_t a_Loc, const float * a_ValueFloat3x3, bool a_Transpose )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}
	
	glUniformMatrix3fv( a_Loc, 1, a_Transpose, a_ValueFloat3x3 );
}

//=====================================================================================
void GLShader::SetUniformColour( int32_t a_Loc, const Colour & a_Value )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}

	glUniform4f( a_Loc, a_Value.r, a_Value.g, a_Value.b, a_Value.a );
}

//=====================================================================================
void GLShader::SetUniformFloat1Array( int32_t a_Loc, const Array< float > & a_Values )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}
	
	glUniform1fv( a_Loc, a_Values.Count(), a_Values.First() );
}

//=====================================================================================
void GLShader::SetUniformFloat2Array( int32_t a_Loc, const Array< Vector2 > & a_Values )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}
	
	glUniform2fv( a_Loc, a_Values.Count(), &a_Values.First()->x );
}

//=====================================================================================
void GLShader::SetUniformColourArray( int32_t a_Loc, const Array< Colour > & a_Values )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}
	
	glUniform1fv( a_Loc, a_Values.Count(), &a_Values.First()->r );
}

//=====================================================================================
void GLShader::SetUniformSampler( int32_t a_Loc, uint32_t a_TextureUnit )
{
	if ( m_CurrentShaderHandle != m_Handle )
	{
		m_CurrentShaderHandle = m_Handle;
		glUseProgram( m_Handle );
	}

	glUniform1i( a_Loc, a_TextureUnit );
}