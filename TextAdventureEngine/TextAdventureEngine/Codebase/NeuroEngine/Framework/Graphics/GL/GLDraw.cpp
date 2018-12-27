#include "GLDraw.hpp"
#include "PrecompiledHeaderGL.hpp"
#include "GLMeshIndexed.hpp"
#include "GLMesh.hpp"
#include "GLShader.hpp"
#include "GLTexture.hpp"
#include "GLRenderTexture.hpp"
#include "GLShaderStorageBuffer.hpp"
#include "GLUniformBuffer.hpp"
#include <Framework/Engine.hpp>

//=====================================================================================
GLint GLDraw::m_CurrentMeshHandle = 0;

//=====================================================================================
void GLDraw::SetRenderTarget( const GLRenderTexture * a_RenderTexture, const Array< uint32_t > & a_DrawBuffers, bool a_Clear )
{
	if ( !a_RenderTexture )
	{
		GLRenderTexture::m_CurrentFramebufferHandle = 0;
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glViewport( 0, 0, ( GLsizei )Engine::Instance().GetDisplaySize().x, ( GLsizei )Engine::Instance().GetDisplaySize().y );
		glScissor( 0, 0, ( GLsizei )Engine::Instance().GetDisplaySize().x, ( GLsizei )Engine::Instance().GetDisplaySize().y );
	}

	else if ( ASSERT( a_RenderTexture->IsValid(), "GLDraw: Trying to target invalid rendertexture '%s'", a_RenderTexture ? a_RenderTexture->Name() : "null" ) )
	{
		const bool changingFBO = GLRenderTexture::m_CurrentFramebufferHandle != a_RenderTexture->GetHandle();
		if ( changingFBO )
		{
			GLRenderTexture::m_CurrentFramebufferHandle = a_RenderTexture->GetHandle();
			glBindFramebuffer( GL_FRAMEBUFFER, a_RenderTexture->GetHandle() );

			GLenum drawBuffers[ 8 ];
			const uint32_t * dbBegin = ( a_DrawBuffers.First() );
			const uint32_t * dbIt = ( a_DrawBuffers.First() );
			const uint32_t * dbEnd = ( a_DrawBuffers.End() );
			while ( dbIt != dbEnd )
			{
				if ( *dbIt < a_RenderTexture->GetNumColourTextures() )
				{
					drawBuffers[ dbIt - dbBegin ] = GL_COLOR_ATTACHMENT0 + *dbIt;
				}
				++dbIt;
			}
			glDrawBuffers( dbEnd - dbBegin, drawBuffers );
		}

		if ( changingFBO )
		{
			glViewport( 0, 0, a_RenderTexture->Width(), a_RenderTexture->Height() );
			glScissor( 0, 0, a_RenderTexture->Width(), a_RenderTexture->Height() );
		}

		if ( a_Clear )
		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
		}
	}
}

//=====================================================================================
void GLDraw::SetRenderTargetDBC( const Array< uint32_t > & a_DrawBuffers, bool a_Clear )
{
	GLenum drawBuffers[ 8 ];
	const uint32_t * dbBegin = ( a_DrawBuffers.First() );
	const uint32_t * dbIt = ( a_DrawBuffers.First() );
	const uint32_t * dbEnd = ( a_DrawBuffers.End() );
	while ( dbIt != dbEnd )
	{
		if ( *dbIt < 16 )
		{
			drawBuffers[ dbIt - dbBegin ] = GL_COLOR_ATTACHMENT0 + *dbIt;
		}
		++dbIt;
	}
	glDrawBuffers( dbEnd - dbBegin, drawBuffers );

	if ( a_Clear )
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	}
}

//=====================================================================================
void GLDraw::BindTexture( const GLRenderTexture * a_RenderTexture, uint32_t a_Unit, uint32_t a_ColourTextureIndex, bool a_RebuildMips )
{
	if ( !a_RenderTexture )
	{
		glActiveTexture( GL_TEXTURE0 + a_Unit );
		glBindTexture( GL_TEXTURE_2D, 0 );
		return;
	}

	if ( ASSERT( a_RenderTexture && a_RenderTexture->IsValid(), "GLDraw: Trying to use invalid rendertexture '%s'", a_RenderTexture ? a_RenderTexture->Name() : "null" ) )
	{
		GLint units;
		glGetIntegerv( GL_MAX_IMAGE_UNITS, &units );
		if ( ASSERT( static_cast< GLint >( a_Unit ) < units, "GLDraw: Trying to bind '%s' to an invalid unit index (max = %d)", a_RenderTexture->Name(), units ) )
		{
			glActiveTexture( GL_TEXTURE0 + a_Unit );
			glBindTexture( GL_TEXTURE_2D, a_RenderTexture->GetCAHandle( a_ColourTextureIndex ) );

			if ( a_RebuildMips )
			{
				if ( a_RenderTexture->GetMipMapsEnabled() )
				{
					glGenerateMipmap( GL_TEXTURE_2D );
				}

				else
				{
					PRINT( "GLDraw: [%s] Invalid operation - trying to generate mipmaps for a Render Texture that does not support them.", a_RenderTexture->Name() );
				}
			}
		}
	}
}

//=====================================================================================
void GLDraw::BindTexture( const GLTexture * a_Texture, uint32_t a_Unit )
{
	if ( !a_Texture )
	{
		glActiveTexture( GL_TEXTURE0 + a_Unit );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glBindTexture( GL_TEXTURE_1D, 0 );
	}

	else if ( ASSERT( a_Texture->IsValid(), "GLDraw: Trying to use invalid texture '%s'", a_Texture ? a_Texture->Name() : "null" ) )
	{
		GLenum ttype = a_Texture->GetTextureType() == TextureType::TT_1D 
							? GL_TEXTURE_1D 
							: GL_TEXTURE_2D;

		GLint units;
		glGetIntegerv( GL_MAX_IMAGE_UNITS, &units );
		if ( ASSERT( static_cast< GLint >( a_Unit ) <= units, "GLDraw: Trying to bind '%s' to an invalid unit index (max = %d)", a_Texture->Name(), units ) )
		{
			glActiveTexture( GL_TEXTURE0 + a_Unit );
			glBindTexture( ttype, a_Texture->GetHandle() );
		}
	}
}

//=====================================================================================
void GLDraw::BindShaderStorageBuffer( const GLShaderStorageBuffer * a_ShaderStorageBuffer, uint32_t a_BaseIndex )
{
	GLint maxBindings; 
	glGetIntegerv( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxBindings ); 

	if ( a_BaseIndex > ( uint32_t )maxBindings )
	{
		ASSERT_WARN( false, "GLDraw: Trying to bind shader storage buffer to invalid index, the valid range is [%u <-> %u]", 0, maxBindings );
		return;
	}

	if ( ASSERT( a_ShaderStorageBuffer && a_ShaderStorageBuffer->IsValid(), "GLDraw: Trying to use invalid shader storage buffer '%s'", a_ShaderStorageBuffer ? a_ShaderStorageBuffer->Name() : "null" ) )
	{
		if ( GLShaderStorageBuffer::m_CurrentSSBOHandles[ a_BaseIndex ] != a_ShaderStorageBuffer->GetHandle() )
		{
			GLShaderStorageBuffer::m_CurrentSSBOHandles[ a_BaseIndex ] = a_ShaderStorageBuffer->GetHandle();
			glBindBufferBase( GL_SHADER_STORAGE_BUFFER, a_BaseIndex, a_ShaderStorageBuffer->GetHandle() );
		}
	}
}

//=====================================================================================
void GLDraw::BindUniformBuffer( const GLUniformBuffer * a_UniformBuffer, uint32_t a_BaseIndex )
{
	GLint maxBindings;
	glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings );

	if ( a_BaseIndex > ( uint32_t )maxBindings )
	{
		ASSERT_WARN( false, "GLDraw: Trying to bind uniform buffer to invalid index, the valid range is [%u <-> %u]", 0, maxBindings );
		return;
	}

	if ( ASSERT( a_UniformBuffer && a_UniformBuffer->IsValid(), "GLDraw: Trying to use invalid uniform buffer '%s'", a_UniformBuffer ? a_UniformBuffer->Name() : "null" ) )
	{
		if ( GLUniformBuffer::m_CurrentUBOHandles[ a_BaseIndex ] != a_UniformBuffer->GetHandle() )
		{
			GLUniformBuffer::m_CurrentUBOHandles[ a_BaseIndex ] = a_UniformBuffer->GetHandle();
			glBindBufferBase( GL_UNIFORM_BUFFER, a_BaseIndex, a_UniformBuffer->GetHandle() );
		}
	}
}

//=====================================================================================
void GLDraw::UseShader( const GLShader * a_Shader )
{
	if ( ASSERT( a_Shader && a_Shader->IsValid(), "GLDraw: Trying to use invalid shader '%s'", a_Shader ? a_Shader->Name() : "null" ) )
	{
		if ( GLShader::m_CurrentShaderHandle != a_Shader->GetHandle() )
		{
			GLShader::m_CurrentShaderHandle = a_Shader->GetHandle();
			glUseProgram( a_Shader->GetHandle() );
		}
	}
}

//=====================================================================================
void GLDraw::DrawMesh( const GLMeshIndexed * a_Mesh, uint32_t a_Count )
{
	if ( ASSERT( a_Mesh && a_Mesh->IsValid(), "GLDraw: Trying to draw invalid mesh '%s'", a_Mesh ? a_Mesh->Name() : "null" ) )
	{
		if ( m_CurrentMeshHandle != a_Mesh->GetHandle() )
		{
			m_CurrentMeshHandle = a_Mesh->GetHandle();
			glBindVertexArray( a_Mesh->GetHandle() );
		}

		glDrawElements( a_Mesh->GetDrawMode(), Min( a_Count, a_Mesh->GetIndexNum() ), a_Mesh->GetIndexType(), ( const void* )0 );
	}
}

//=====================================================================================
void GLDraw::DrawMesh( const GLMesh * a_Mesh, uint32_t a_Count )
{
	if ( ASSERT( a_Mesh && a_Mesh->IsValid(), "GLDraw: Trying to draw invalid mesh '%s'", a_Mesh ? a_Mesh->Name() : "null" ) )
	{
		if ( m_CurrentMeshHandle != a_Mesh->GetHandle() )
		{
			m_CurrentMeshHandle = a_Mesh->GetHandle();
			glBindVertexArray( a_Mesh->GetHandle() );
		}
		
		glDrawArrays( a_Mesh->GetDrawMode(), 0, Min( a_Count, a_Mesh->GetVertexNum() ) );
	}
}

//=====================================================================================
void GLDraw::DispatchCompute( uint32_t a_NumGroup0, uint32_t a_NumGroup1, uint32_t a_NumGroup2 )
{
	GLint counts[3];
	glGetIntegerv( GL_MAX_COMPUTE_WORK_GROUP_COUNT, counts );

	if ( ASSERT_WARN( a_NumGroup0 < counts[0] &&
					  a_NumGroup0 < counts[1] &&
					  a_NumGroup0 < counts[2], "Invalid dispatch sizes: {%d, %d, %d} must be less than {%d, %d, %d}", a_NumGroup0, a_NumGroup1, a_NumGroup2, counts[0], counts[1], counts[2] ) )
	{
		glDispatchCompute( a_NumGroup0, a_NumGroup1, a_NumGroup2 );
	}
}