#include "GLDraw.hpp"
#include "PrecompiledHeaderGL.hpp"
#include "GLMesh.hpp"
#include "GLShader.hpp"
#include "GLTexture.hpp"
#include "GLRenderTexture.hpp"
#include <Framework/Engine.hpp>

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
void GLDraw::DrawMesh( const GLMesh * a_Mesh )
{
	if ( ASSERT( a_Mesh && a_Mesh->IsValid(), "GLDraw: Trying to draw invalid mesh '%s'", a_Mesh ? a_Mesh->Name() : "null" ) )
	{
		GLint mesh = GLMesh::m_CurrentMeshHandle;

		if ( mesh != a_Mesh->GetHandle() )
		{
			GLMesh::m_CurrentMeshHandle = a_Mesh->GetHandle();
			glBindVertexArray( a_Mesh->GetHandle() );

			for ( uint32_t k = 0; k < a_Mesh->GetVertexAttribNum(); ++k )
			{
				glEnableVertexAttribArray( k );
			}
		}

		glDrawElements( a_Mesh->GetDrawMode(), a_Mesh->GetIndexNum(), a_Mesh->GetIndexType(), ( const void* )0 );
	}
}