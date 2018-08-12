#pragma once
#ifndef GLSHADER_H
#define GLSHADER_H

#include <Framework/Utils/InitialiserList.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Containers/Dictionary.hpp>
#include "PrecompiledHeaderGL.hpp"

//=====================================================================================
class GLTexture;

//=====================================================================================
class GLShader : public GLObj< GLShader >
{
	friend class GLDraw;
public:

	struct Source;

	ENUM( ShaderType, int32_t )
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
	END_ENUM;

	GLShader();
	GLShader( const char * a_Name, const InitialiserList< Source > & a_ShaderSources );
	~GLShader();
	inline GLint GetHandle() const 
	{ 
		return m_Handle; 
	}

	int32_t GetUniform( const char * a_UniformName );

	void SetUniformInt32( int32_t a_Loc, int32_t a_Value );
	void SetUniformFloat1( int32_t a_Loc, float a_Value );
	void SetUniformFloat2( int32_t a_Loc, const Vector2 & a_Value );
	void SetUniformFloat3( int32_t a_Loc, const float * a_ValueFloat3 );
	void SetUniformFloat3x3( int32_t a_Loc, const float * a_ValueFloat3x3, bool a_Transpose = false );
	void SetUniformColour( int32_t a_Loc, const Colour & a_Value );
	void SetUniformFloat1Array( int32_t a_Loc, const Array< float > & a_Values );
	void SetUniformFloat2Array( int32_t a_Loc, const Array< Vector2 > & a_Values );
	void SetUniformColourArray( int32_t a_Loc, const Array< Colour > & a_Values );
	void SetUniformSampler( int32_t a_Loc, uint32_t a_TextureUnit );

	
	inline void SetUniformFloat3( int32_t a_Loc, float a_X, float a_Y, float a_Z )
	{
		SetUniformFloat3( a_Loc, NBegin( { a_X, a_Y, a_Z } ) );
	}

private:

	static GLint m_CurrentShaderHandle;
	GLint m_Handle;
	Dictionary< uint32_t, GLint > m_Locations;
};

//=====================================================================================
struct GLShader::Source
{
	GLShader::ShaderType Type;
	CString SourceGLSL;
};

#endif//GLSHADER_H