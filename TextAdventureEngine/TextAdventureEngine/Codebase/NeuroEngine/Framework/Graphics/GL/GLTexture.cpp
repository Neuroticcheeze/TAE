#include "GLTexture.hpp"

//=====================================================================================
GLTexture::GLTexture()
	: m_Handle( 0 )
	, m_TextureType( TextureType::TT_2D )
	, m_Width( 0 )
	, m_Height( 0 )
{
}

//=====================================================================================
GLTexture::GLTexture( const char * a_Name, SDL_Surface * a_FromSDLSurface, TextureFilterMode a_Filter, TextureWrapMode a_Wrap, bool a_EnableMips, TextureType a_TextureType, uint32_t a_1DScanLine )
	: GLObj( a_Name )
	, m_Handle( 0 )
	, m_TextureType( a_TextureType )
{
	if ( ASSERT( a_FromSDLSurface, "Texture Error [%s] (Supplied with a NULL pointer to SDL2 Surface)", Name() ) )
	{

		SDL_Surface * newSurface = a_FromSDLSurface;

		if ( !ASSERT( newSurface, "Texture Error [%s] %s", Name(), SDL_GetError() ) )
		{
			return;
		}

		int w = newSurface->w;
		int h = newSurface->h;

		if ( a_TextureType == TextureType::TT_2D )
		{
			m_Width = newSurface->w;
			m_Height = newSurface->h;
			glCreateTextures( GL_TEXTURE_2D, 1, &m_Handle );
			
			if ( !ASSERT( glGetError() == GL_NO_ERROR, "Texture Error: [CREATION] %s", Name() ) )
			{
				m_Handle = 0;
				return;
			}
			

			uint32_t levels = Ceil( Log2( Max( w, h ) ) + 1 );
			
			glTextureStorage2D( m_Handle, levels, GL_RGBA8, w, h );
			
			if ( !ASSERT( glGetError() == GL_NO_ERROR, "Texture Error: [VMEM ALLOC] %s", Name() ) )
			{
				glDeleteTextures( 1, &m_Handle );
				m_Handle = 0;
				return;
			}
			
			glTextureSubImage2D( m_Handle, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, newSurface->pixels );

			if ( !ASSERT( glGetError() == GL_NO_ERROR, "Texture Error: [DATA UPLOAD] %s", Name() ) )
			{
				glDeleteTextures( 1, &m_Handle );
				m_Handle = 0;
				return;
			}
			
			GLint prevHandle;
			glGetIntegerv( GL_TEXTURE_BINDING_2D, &prevHandle );
			glBindTexture( GL_TEXTURE_2D, m_Handle );
			
			
			GLint filter = ( uint32_t )a_Filter;
			
			if ( a_EnableMips )
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0 );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, levels );
			
				switch ( a_Filter )
				{
				case TextureFilterMode::LINEAR:
					filter = GL_LINEAR_MIPMAP_LINEAR;
					break;
				case TextureFilterMode::NEAREST:
					filter = GL_NEAREST_MIPMAP_NEAREST;
					break;
				default:
					break;
				}
			}

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ( GLenum )a_Wrap );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ( GLenum )a_Wrap );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, a_Filter == TextureFilterMode::LINEAR ? GL_LINEAR : GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );
			
			glGenerateMipmap( GL_TEXTURE_2D );
			
			glBindTexture( GL_TEXTURE_2D, prevHandle );
		}

		else
		{
			m_Width = newSurface->w;
			m_Height = 1;
			glCreateTextures( GL_TEXTURE_1D, 1, &m_Handle );
			
			if ( !ASSERT( glGetError() == GL_NO_ERROR, "Texture Error: [CREATION] %s", Name() ) )
			{
				m_Handle = 0;
				return;
			}

			

			uint32_t levels = Ceil( Log2( w ) );
			
			glTextureStorage1D( m_Handle, levels, GL_RGBA8, w );
			
			if ( !ASSERT( glGetError() == GL_NO_ERROR, "Texture Error: [VMEM ALLOC] %s", Name() ) )
			{
				glDeleteTextures( 1, &m_Handle );
				m_Handle = 0;
				return;
			}
			
			uint8_t * data = reinterpret_cast< uint8_t* >( newSurface->pixels );
			data += Min( ( int32_t )a_1DScanLine, h - 1 ) * newSurface->pitch;
			glTextureSubImage1D( m_Handle, 0, 0, w, GL_RGBA, GL_UNSIGNED_BYTE, data );
			
			if ( !ASSERT( glGetError() == GL_NO_ERROR, "Texture Error: [DATA UPLOAD] %s", Name() ) )
			{
				glDeleteTextures( 1, &m_Handle );
				m_Handle = 0;
				return;
			}
			
			GLint prevHandle;
			glGetIntegerv( GL_TEXTURE_BINDING_1D, &prevHandle );
			glBindTexture( GL_TEXTURE_1D, m_Handle );
			
			
			GLint filter = ( uint32_t )a_Filter;
			
			if ( a_EnableMips )
			{
				glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_LOD, 0 );
				glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAX_LOD, levels );
			
				switch ( a_Filter )
				{
				case TextureFilterMode::LINEAR:
					filter = GL_LINEAR_MIPMAP_LINEAR;
					break;
				case TextureFilterMode::NEAREST:
					filter = GL_NEAREST_MIPMAP_NEAREST;
					break;
				default:
					break;
				}
			}
			
			glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, ( GLenum )a_Wrap );
			glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, ( GLenum )a_Wrap );
			glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, a_Filter == TextureFilterMode::LINEAR ? GL_LINEAR : GL_NEAREST );
			glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, filter );
			
			glGenerateMipmap( GL_TEXTURE_1D );
			
			glBindTexture( GL_TEXTURE_1D, prevHandle );
		}
		
		//SDL_FreeSurface( newSurface );
		Validate();
	}
}

//=====================================================================================
GLTexture::~GLTexture()
{
	GLOBJ_RELEASE;
	glDeleteTextures( 1, &m_Handle );
	m_Handle = 0;
}