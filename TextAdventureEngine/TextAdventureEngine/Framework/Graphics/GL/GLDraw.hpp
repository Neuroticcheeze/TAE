#pragma once
#ifndef GLDRAW_H
#define GLDRAW_H

#include <Framework/Template/Singleton.hpp>
#include <Framework/Containers/Array.hpp>

//=====================================================================================
class GLRenderTexture;
class GLTexture;
class GLShader;
class GLMesh;

#define NO_TEXTURE ( GLTexture* )nullptr
#define NO_RENDERTEXTURE ( GLRenderTexture* )nullptr

//=====================================================================================
class GLDraw : public Singleton< GLDraw >
{
public:

	void SetRenderTarget( const GLRenderTexture * a_RenderTexture, const Array< uint32_t > & a_DrawBuffers, bool a_Clear = true );
	void SetRenderTargetDBC( const Array< uint32_t > & a_DrawBuffers, bool a_Clear = true );
	void BindTexture( const GLRenderTexture * a_RenderTexture, uint32_t a_Unit = 0, uint32_t a_ColourTextureIndex = 0, bool a_RebuildMips = true );
	void BindTexture( const GLTexture * a_Texture, uint32_t a_Unit = 0 );
	void UseShader( const GLShader * a_Shader );
	void DrawMesh( const GLMesh * a_Mesh );

private:


};

#endif//GLDRAW_H