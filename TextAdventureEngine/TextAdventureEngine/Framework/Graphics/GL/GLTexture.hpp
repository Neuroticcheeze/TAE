#pragma once
#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "PrecompiledHeaderGL.hpp"
#include "GLEnums.hpp"

//=====================================================================================
class GLTexture : public GLObj< GLTexture >
{
public:

	GLTexture();
	GLTexture( const char * a_Name, SDL_Surface * a_FromSDLSurface, TextureFilterMode a_Filter = TextureFilterMode::LINEAR, TextureWrapMode a_Wrap = TextureWrapMode::REPEAT, bool a_EnableMips = false, TextureType a_TextureType = TextureType::TT_2D, uint32_t a_1DScanLine = 0 );
	//GLTexture( const char * a_Name, SDL_Texture * a_FromSDLTexture, bool a_Force1D = false );
	//GLTexture( const char * a_Name ); /* Render Target overload */
	~GLTexture();

	inline GLuint GetHandle() const 
	{ 
		return m_Handle; 
	}

	inline TextureType GetTextureType() const
	{
		return m_TextureType;
	}

	inline uint32_t GetWidth() const
	{
		return IsValid() ? m_Width : 0;
	}

	inline uint32_t GetHeight() const
	{
		return IsValid() ? m_Height : 0;
	}

private:

	GLuint m_Handle;
	TextureType m_TextureType;
	uint32_t m_Width;
	uint32_t m_Height;
};

#endif//GLTEXTURE_H