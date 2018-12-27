#pragma once
#ifndef GLDRAW_H
#define GLDRAW_H

#include <Framework/Template/Singleton.hpp>
#include <Framework/Containers/Array.hpp>

//=====================================================================================
class GLShaderStorageBuffer;
class GLUniformBuffer;
class GLRenderTexture;
class GLTexture;
class GLShader;
class GLMeshIndexed;
class GLMesh;

#define NO_TEXTURE ( GLTexture* )nullptr
#define NO_RENDERTEXTURE ( GLRenderTexture* )nullptr

//=====================================================================================
class GLDraw : public Singleton< GLDraw >
{
	friend class GLMesh;
	friend class GLMeshIndexed;

public:

	void SetRenderTarget( const GLRenderTexture * a_RenderTexture, const Array< uint32_t > & a_DrawBuffers, bool a_Clear = true );
	void SetRenderTargetDBC( const Array< uint32_t > & a_DrawBuffers, bool a_Clear = true );
	void BindTexture( const GLRenderTexture * a_RenderTexture, uint32_t a_Unit = 0, uint32_t a_ColourTextureIndex = 0, bool a_RebuildMips = true );
	void BindTexture( const GLTexture * a_Texture, uint32_t a_Unit = 0 );
	void BindShaderStorageBuffer( const GLShaderStorageBuffer * a_ShaderStorageBuffer, uint32_t a_BaseIndex );
	void BindUniformBuffer( const GLUniformBuffer * a_UniformBuffer, uint32_t a_BaseIndex );
	void UseShader( const GLShader * a_Shader );
	void DrawMesh( const GLMeshIndexed * a_Mesh, uint32_t a_Count = UINT_MAX );
	void DrawMesh( const GLMesh * a_Mesh, uint32_t a_Count = UINT_MAX );
	void DispatchCompute( uint32_t a_NumGroup0, uint32_t a_NumGroup1, uint32_t a_NumGroup2 );

private:

	static int32_t m_CurrentMeshHandle; // common handle slot shared by indexed & non-indexed meshes.
};

#endif//GLDRAW_H