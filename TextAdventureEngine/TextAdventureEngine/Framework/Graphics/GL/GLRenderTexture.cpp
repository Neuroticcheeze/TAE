#include "GLRenderTexture.hpp"
#include <Framework/Math/Math.hpp>

//=====================================================================================
GLint GLRenderTexture::m_CurrentFramebufferHandle = 0;

//=====================================================================================
GLRenderTexture::GLRenderTexture()
	: GLObj( "<Unnamed>" )
	, m_Handle( 0 )
	, m_DepthBuffer( 0 )
	, m_Width( 0 )
	, m_Height( 0 )
	, m_EnableMipMapping( false )
	, m_FilterMode( TextureFilterMode::LINEAR )
	, m_WrapMode( TextureWrapMode::REPEAT )
	, m_PixelFormat( GraphicsManager::PF_RGBA32F )
{
}

//=====================================================================================
GLRenderTexture::GLRenderTexture( const char * a_Name, uint32_t a_Width, uint32_t a_Height, uint32_t a_NumColourTextures, bool a_EnableMipMapping, TextureFilterMode a_Filter, TextureWrapMode a_Wrap, GraphicsManager::PixelFormat a_Format )
	: GLObj( a_Name )
	, m_Handle( 0 )
	, m_DepthBuffer( 0 )
	, m_Width( Clamp( a_Width, 1U, 8192U ) )
	, m_Height( Clamp( a_Height, 1U, 8192U ) )
	, m_EnableMipMapping( a_EnableMipMapping )
	, m_FilterMode( a_Filter )
	, m_WrapMode( a_Wrap )
	, m_PixelFormat( a_Format )
{
	GLenum err;

	glGenFramebuffers( 1, &m_Handle );
	glBindFramebuffer( GL_FRAMEBUFFER, m_Handle );

	if ( !ASSERT( ( err = glGetError() ) == GL_NO_ERROR, "Render Texture Error: [CREATION] %s \"%s\"", Name(), GLErrorString( err ) ) )
	{
		glDeleteFramebuffers( 1, &m_Handle );
		m_Handle = 0;
		return;
	}

	const uint32_t numColours = Min( a_NumColourTextures, SDL_arraysize( m_RenderTextures ) );
	m_RenderTextureCount = numColours;
	glGenTextures( numColours, m_RenderTextures );
	for ( uint32_t i = 0; i < numColours; ++i )
	{
		glBindTexture( GL_TEXTURE_2D, m_RenderTextures[ i ] );

		switch ( a_Format )
		{
		case GraphicsManager::PF_RGB8:
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );
			break;
		case GraphicsManager::PF_RGBA8:
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
			break;
		case GraphicsManager::PF_RGBA32F:
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, 0 );
			break;
		default:
			break;
		}

		//glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );

		GLint filter = ( uint32_t )a_Filter;

		if ( m_EnableMipMapping )
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0 );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, Log2( Min( m_Width, m_Height ) ) );

			switch ( a_Filter )
			{
			case TextureFilterMode::LINEAR:
				filter = GL_LINEAR_MIPMAP_NEAREST;
				break;
			case TextureFilterMode::NEAREST:
				filter = GL_NEAREST_MIPMAP_NEAREST;
				break;
			default:
				break;
			}
		}

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ( uint32_t )a_Wrap );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ( uint32_t )a_Wrap );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, a_Filter == TextureFilterMode::LINEAR ? GL_LINEAR : GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );

		glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, m_RenderTextures[ i ], 0 );
	}

	glBindTexture( GL_TEXTURE_2D, 0 );

	if ( !ASSERT( ( err = glGetError() ) == GL_NO_ERROR, "Render Texture Error: [COLOUR ATTACHMENTS] %s \"%s\"", Name(), GLErrorString( err ) ) )
	{
		glDeleteTextures( numColours, m_RenderTextures );
		glDeleteFramebuffers( 1, &m_Handle );
		m_Handle = 0;
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		return;
	}

	// The depth buffer
	glGenRenderbuffers( 1, &m_DepthBuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, m_DepthBuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height );
	
	if ( !ASSERT( ( err = glGetError() ) == GL_NO_ERROR, "Render Texture Error: [DEPTH ATTACHMENT STORAGE] %s \"%s\"", Name(), GLErrorString( err ) ) )
	{
		glDeleteRenderbuffers( 1, &m_DepthBuffer );
		glDeleteTextures( numColours, m_RenderTextures );
		glDeleteFramebuffers( 1, &m_Handle );
		m_Handle = 0;
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		return;
	}
	
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer );
	
	if ( !ASSERT( ( err = glGetError() ) == GL_NO_ERROR, "Render Texture Error: [DEPTH ATTACHMENT LINKING] %s \"%s\"", Name(), GLErrorString( err ) ) )
	{
		glDeleteRenderbuffers( 1, &m_DepthBuffer );
		glDeleteTextures( numColours, m_RenderTextures );
		glDeleteFramebuffers( 1, &m_Handle );
		m_Handle = 0;
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		return;
	}

	// Always check that our framebuffer is ok
	if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE )
	{
		Validate();
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

//=====================================================================================
GLRenderTexture::~GLRenderTexture()
{
	GLOBJ_RELEASE;

	const uint32_t numColours = SDL_arraysize( m_RenderTextures );
	glDeleteTextures( numColours, m_RenderTextures );
	BSet( m_RenderTextures, 0, sizeof( m_RenderTextures ) );
	
	glDeleteRenderbuffers( 1, &m_DepthBuffer );
	m_DepthBuffer = 0;

	glDeleteFramebuffers( 1, &m_Handle );
	m_Handle = 0;
}