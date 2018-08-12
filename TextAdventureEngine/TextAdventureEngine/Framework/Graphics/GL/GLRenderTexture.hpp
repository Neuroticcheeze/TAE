#pragma once
#ifndef GLRENDERTEXTURE_H
#define GLRENDERTEXTURE_H

#include "PrecompiledHeaderGL.hpp"
#include "GLEnums.hpp"
#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
class GLRenderTexture : public GLObj< GLRenderTexture >
{
	friend class GLDraw;

public:

	GLRenderTexture();
	GLRenderTexture( const char * a_Name, uint32_t a_Width, uint32_t a_Height, uint32_t a_NumColourTextures, bool a_EnableMipMapping = true, TextureFilterMode a_Filter = TextureFilterMode::LINEAR, TextureWrapMode a_Wrap = TextureWrapMode::CLAMP, GraphicsManager::PixelFormat a_Format = GraphicsManager::PF_RGBA32F );
	~GLRenderTexture();

	inline GLuint GetHandle() const 
	{ 
		return m_Handle;
	}

	inline uint32_t Width() const
	{
		return m_Width;
	}

	inline uint32_t Height() const
	{
		return m_Height;
	}

	inline GLuint GetCAHandle( uint32_t a_Index ) const
	{
		return m_RenderTextures[ a_Index ];
	}

	inline bool GetMipMapsEnabled() const
	{
		return m_EnableMipMapping;
	}

	inline GraphicsManager::PixelFormat GetPixelFormat() const
	{
		return m_PixelFormat;
	}
		
	inline TextureFilterMode GetFilterMode() const
	{
		return m_FilterMode;
	}

	inline TextureWrapMode GetWrapMode() const
	{
		return m_WrapMode;
	}

	inline uint32_t GetNumColourTextures() const
	{
		return m_RenderTextureCount;
	}

private:

	static GLint m_CurrentFramebufferHandle;
	GraphicsManager::PixelFormat m_PixelFormat;
	TextureFilterMode m_FilterMode;
	TextureWrapMode m_WrapMode;
	GLuint m_Handle;
	GLuint m_RenderTextures[ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	uint32_t m_RenderTextureCount;
	GLuint m_DepthBuffer;
	bool   m_EnableMipMapping;
	uint32_t m_Width;
	uint32_t m_Height;
};

#endif//GLRENDERTEXTURE_H