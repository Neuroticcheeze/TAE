#include "GraphicsManager.hpp"
#include <Framework/Containers/Set.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Engine.hpp>
#include <Framework/Graphics/GL/GLTexture.hpp>
#include <Framework/Graphics/GL/GLShader.hpp>
#include <Framework/Graphics/GL/GLEnums.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_opengl.h>
#include <gl/glu.h>
#include <SDL/SDL_opengl_glext.h>
#include <glm/mat3x3.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/ext.hpp>
#undef DrawText
#include <Framework/File/FileManager.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Graphics/GL/GLDraw.hpp>
#include <Framework/Graphics/GL/GLMesh.hpp>
#include <Framework/Graphics/GL/GLRenderTexture.hpp>

#define WM reinterpret_cast< SDL_Window* >( Engine::Instance().m_Window )
#define R reinterpret_cast< SDL_Renderer* >( Engine::Instance().m_Renderer )

//=====================================================================================
using namespace glm;
typedef glm::mat3 Float3x3;

//=====================================================================================
class UShader final
{
	friend class GraphicsManager;

public:

	UShader()
		: m_PtrGLShader( nullptr )
		, m_Owner( false )
	{}

	UShader( GLShader * a_Shader, Array< Tuple2< GraphicsManager::Texture, uint32_t > > a_RuntimeTextures )
		: m_PtrGLShader( a_Shader )
		, m_Owner( true )
		, m_RuntimeTextures( a_RuntimeTextures )
	{

	}

	UShader( UShader & a_Other )
		: UShader()
	{
		*this = a_Other;
	}

	UShader & operator=( UShader & a_Other )
	{
		this->~UShader();

		m_Owner = true;
		m_PtrGLShader = a_Other.m_PtrGLShader;
		a_Other.m_Owner = false;
		m_RuntimeTextures = a_Other.m_RuntimeTextures;
		return *this;
	}

	~UShader()
	{
		if ( m_PtrGLShader && m_Owner )
		{
			delete m_PtrGLShader;
			m_PtrGLShader = nullptr;
		}
	}

private:

	Array< Tuple2< GraphicsManager::Texture, uint32_t > > m_RuntimeTextures;
	GLShader * m_PtrGLShader;
	bool m_Owner;
};

//=====================================================================================
void * GraphicsManager::GetGLShaderPtr( const Shader & a_Shader )
{
	return a_Shader->m_PtrGLShader;
}

//=====================================================================================
Vector2 GraphicsManager::QueryTextureSize( const Texture & a_Texture ) const
{
	PROFILE;

	if ( a_Texture )
	{
		Vector2 size;
		if ( a_Texture->Value0 )
		{
			const GLTexture * gltex = reinterpret_cast< GLTexture* >( a_Texture->Value0 );
			size.x = static_cast< float >( gltex->GetWidth() );
			size.y = static_cast< float >( gltex->GetHeight() );
		}

		else if ( a_Texture->Value1 )
		{
			const SDL_Surface * surf = reinterpret_cast< SDL_Surface* >( a_Texture->Value1 );
			size.x = static_cast< float >( surf->w );
			size.y = static_cast< float >( surf->h );
		}

		return size;
	}

	return Vector2::ZERO;
}

//=====================================================================================
Vector2 GraphicsManager::QueryRenderTextureSize( const RenderTexture & a_RenderTexture ) const
{
	PROFILE;

	float width = 0.0F;
	float height = 0.0F;

	if ( a_RenderTexture )
	{
		GLRenderTexture * glrt = reinterpret_cast< GLRenderTexture* >( *a_RenderTexture.Ptr() );
		if ( ASSERT( glrt, "%s: Corrupted RenderTexture", __FUNCTION__ ) )
		{
			width = static_cast< float >( glrt->Width() );
			height = static_cast< float >( glrt->Height() );
		}
	}

	return Vector2( width, height );
}


//=====================================================================================
GraphicsManager::RenderTexture GraphicsManager::ResizeRenderTexture( uint32_t a_HashName, const Vector2 & a_Dimensions )
{
	PROFILE2( "ByHashName" );

	PRenderTexture * rtex = m_RenderTextures[ a_HashName ];

	if ( rtex &&
		 InRange( a_Dimensions.x, 1.0F, 8192.0F ) && 
		 InRange( a_Dimensions.y, 1.0F, 8192.0F ) )
	{
		GLRenderTexture * glrt = reinterpret_cast< GLRenderTexture* >( *rtex->Ptr() );
		if ( ASSERT( glrt, "%s: Corrupted RenderTexture", __FUNCTION__ ) )
		{
			PixelFormat			pixelFormat = glrt->GetPixelFormat();
			TextureFilterMode	filter =	  glrt->GetFilterMode();
			TextureWrapMode		wrap =		  glrt->GetWrapMode();
			bool				emip =		  glrt->GetMipMapsEnabled();
			uint32_t			numCols =	  glrt->GetNumColourTextures();

			m_RenderTextures.Remove( a_HashName );
			return AllocateRenderTexture( a_HashName, a_Dimensions, numCols, pixelFormat, filter, wrap, emip );
		}
	}

	return RenderTexture();
}

//=====================================================================================
void GraphicsManager::ResizeRenderTexture( RenderTexture & a_RenderTexture, const Vector2 & a_Dimensions )
{
	PROFILE;

	if ( a_RenderTexture && 
		 InRange( a_Dimensions.x, 1.0F, 8192.0F ) && 
		 InRange( a_Dimensions.y, 1.0F, 8192.0F ) )
	{
		if ( QueryRenderTextureSize( a_RenderTexture ) != a_Dimensions )
		{
			auto pair = m_RenderTextures.GetEntries().Find< const RenderTexture & >( []( const RenderTexture & a_It, const Dictionary< uint32_t, PRenderTexture >::Pair & a_Pair ) {
				return a_Pair.Value == a_It;
			}, a_RenderTexture );
			
			if ( pair )
			{
				GLRenderTexture * glrt = reinterpret_cast< GLRenderTexture* >( *a_RenderTexture.Ptr() );
				if ( ASSERT( glrt, "%s: Corrupted RenderTexture", __FUNCTION__ ) )
				{
					PixelFormat			pixelFormat = glrt->GetPixelFormat();
					TextureFilterMode	filter =	  glrt->GetFilterMode();
					TextureWrapMode		wrap =		  glrt->GetWrapMode();
					bool				emip =		  glrt->GetMipMapsEnabled();
					uint32_t			numCols =	  glrt->GetNumColourTextures();

					uint32_t key = pair->Key;
					m_RenderTextures.Remove( key );
					a_RenderTexture = AllocateRenderTexture( pair->Key, a_Dimensions, numCols, pixelFormat, filter, wrap, emip );
				}
			}
		}
	}
}

//=====================================================================================
void GraphicsManager::ActivateRenderTexture( RenderTexture & a_RenderTexture, const InitialiserList< uint32_t > & a_DrawBuffers, bool a_Clear )
{
	PROFILE;

	if ( ASSERT( a_RenderTexture, "%s: rendertexture is null!", __FUNCTION__ ) )
	{
		GLRenderTexture * glrt = reinterpret_cast< GLRenderTexture* >( *a_RenderTexture.Ptr() );
		if ( ASSERT( glrt, "%s: Corrupted RenderTexture", __FUNCTION__ ) )
		{
			m_ActiveRenderTexture = a_RenderTexture;
			GLDraw::Instance().SetRenderTarget( glrt, a_DrawBuffers, a_Clear );
		}
	}
}

//=====================================================================================
void GraphicsManager::ActivateRTDrawBuffers( const InitialiserList< uint32_t > & a_DrawBuffers, bool a_Clear )
{
	GLDraw::Instance().SetRenderTargetDBC( a_DrawBuffers, a_Clear );
}

//=====================================================================================
void GraphicsManager::DeactivateRenderTexture() 
{
	m_ActiveRenderTexture = nullptr;
	GLDraw::Instance().SetRenderTarget( nullptr, {}, false );
}

//=====================================================================================
const GraphicsManager::RenderTexture & GraphicsManager::GetActiveRenderTexture() const
{
	return m_ActiveRenderTexture;
}

//=====================================================================================
static const char * const COMMENT_REGEX = R"(//.*$)";
static const char * const COMMENTBLOCK_REGEX = R"((?:\/\*(?:.|\n)*\*\/|\/\*(?:.|\n)*\Z))";
static const char * const BEGIN_REGEX = R"(^#BEGIN[ \t]*(V|F)[ \t]*(\w+)[ \t]*>>$)";
static const uint32_t	  BEGIN_TYPE = 1;
static const uint32_t	  BEGIN_NAME = 2;
static const char * const INCLUDE_REGEX	= R"(^#include[ \t]*\<((?:\w+[\/])*\w+\.glsl)\>)";
static const uint32_t	  INCLUDE_PATH = 1;
static const char * const LOADT_REGEX = R"(^#loadt[ \t]*\([ \t]*'(.+)'[ \t]*,[ \t]*(linear|nearest)[ \t]*,[ \t]*(clamp|repeat|mirror)[ \t]*(?:,[ \t]*mips[ \t]*=[ \t]*(true|false)|)[ \t]*\)[ \t]*((?:layout[ \t]*\(\s*binding\s*=([ \t]*[0-9]+[ \t]*)\)|)\s*uniform\s*(sampler(?:1|2)D)\s*(\w+);))";
static const uint32_t	  LOADT_PATH = 1;
static const uint32_t	  LOADT_FILTER = 2;
static const uint32_t	  LOADT_WRAP = 3;
static const uint32_t	  LOADT_MIPS = 4;
static const uint32_t	  LOADT_TEXUNIT = 6;
static const uint32_t	  LOADT_SAMPLERTYPE = 7;
static const uint32_t	  LOADT_VARNAME = 8;
static const uint32_t	  LOADT_GLSLREPLACE = 5;
static const char * const DEFINE_REGEX = R"(^#define\s+(\w+)(?:$|\(\s*\)\s*$|(?:$|\(\s*(\w+|(?:\w+\s*,\s*)*\w+)\s*\)\s*|\s+)(.*)$))";
static const uint32_t	  DEFINE_NAME = 1;
static const uint32_t	  DEFINE_PARAMETERS = 2;
static const uint32_t	  DEFINE_BODY = 3;

//^#undef\s+(\w+)

//=====================================================================================
CString PreprocessComments( const CString & a_Source, const CString & a_Path )
{
	CString out = a_Source;

	{
		Array< CString::RegexMatch > commentblocks = out.Regex( COMMENTBLOCK_REGEX );
		auto it = commentblocks.First();
		const auto end = commentblocks.End();
		while ( it != end )
		{
			out.Replace( it->GroupCaptures[ 0U ], "" );
			++it;
		}
	}

	{
		Array< CString::RegexMatch > commentblocks = out.Regex( COMMENT_REGEX );
		auto it = commentblocks.First();
		const auto end = commentblocks.End();
		while ( it != end )
		{
			out.Replace( it->GroupCaptures[ 0U ], "" );
			++it;
		}
	}

	return out;
}

//=====================================================================================
CString PreprocessIncludes( const CString & a_Source, const CString & a_Path, uint32_t a_Level = 0 )
{
	CString result = a_Source;

	ASSERT( a_Level < 16, "GLSL Preprocessor: ERROR! \"%s\": Performing too many recursive #include injections, "
		   "may have found a cyclic dependency; these are always illegal!", a_Path.Get() );

	Array< CString::RegexMatch > includes = CString( result ).Regex( INCLUDE_REGEX );

	auto it = includes.First();
	const auto end = includes.End();
	while ( it != end )
	{
		const CString & path = it->GroupCaptures[ INCLUDE_PATH ];

		CString includeSrc = PreprocessIncludes( FileManager::Instance().ReadAll( path.Get() ), a_Path, a_Level + 1 ).Trim();
		
		if ( includeSrc.Length() > 0 )
		{
			PRINT( "GLSL Preprocessor: Found #include directive; injecting \"%s\" into \"%s\"", path.Get(), a_Path.Get() );
		}

		else
		{
			PRINT( "GLSL Preprocessor: Found #include directive; There is nothing worthwhile to inject, skipping \"#include <%s>\"", path.Get() );
		}

		result = result.SubString( 0, it->Start ) + includeSrc + result.SubString( it->Start + it->Length, INT_MAX );

		int32_t off = includeSrc.Length() - it->Length;
		for ( uint32_t p = ( uint32_t )( it - includes.First() ); p < includes.Count(); ++p )
		{
			includes[ p ].Start += off;
		}

		++it;
	}

	return result;
}

//=====================================================================================
struct ShaderTextureLoader final
{
	CString		Path;
	TextureFilterMode	Filter;
	TextureWrapMode		Wrap;
	bool		MipsEnabled;
	uint32_t	TexUnit;
	TextureType	Type;
	CString		VarName;
};

//=====================================================================================
CString PreprocessLoadDirectives( const CString & a_Source, const CString & a_Path, Array< ShaderTextureLoader > & a_Loaders )
{
	CString result = a_Source;

	Array< CString::RegexMatch > loaddirs = CString( result ).Regex( LOADT_REGEX );

	auto it = loaddirs.First();
	const auto end = loaddirs.End();
	while ( it != end )
	{
		const CString &		path = it->GroupCaptures[ LOADT_PATH ];
		TextureFilterMode	filter;
		TextureWrapMode		wrap;
		bool				mipsEnabled = false;
		uint32_t			texUnit = 0;
		TextureType			type;
		const CString &		varname = it->GroupCaptures[ LOADT_VARNAME ];
		const CString &		glslReplace = it->GroupCaptures[ LOADT_GLSLREPLACE ];

		{
			const CString & sFilter = it->GroupCaptures[ LOADT_FILTER ];
			if ( sFilter == "linear" )
			{
				filter = TextureFilterMode::LINEAR;
			}
		
			else
			{
				filter = TextureFilterMode::NEAREST;
			}
		}

		{
			const CString & sWrap = it->GroupCaptures[ LOADT_WRAP ];
			if ( sWrap == "repeat" )
			{
				wrap = TextureWrapMode::REPEAT;
			}
		
			else if ( sWrap == "clamp" )
			{
				wrap = TextureWrapMode::CLAMP;
			}

			else
			{
				wrap = TextureWrapMode::MIRROR;
			}
		}

		{
			const CString & hasmips = it->GroupCaptures[ LOADT_MIPS ];
			mipsEnabled = hasmips == "true";
		}

		{
			const CString & sTexUnit = it->GroupCaptures[ LOADT_TEXUNIT ];
			CString::Parse( sTexUnit.Get(), texUnit );
		}

		{
			const CString & sType = it->GroupCaptures[ LOADT_SAMPLERTYPE ];
			if ( sType == "sampler1D" )
			{
				type = TextureType::TT_1D;
			}
		
			else
			{
				type = TextureType::TT_2D;
			}
		}

		a_Loaders.Append( ShaderTextureLoader {  
			path,
			filter,
			wrap,
			mipsEnabled,
			texUnit,
			type,
			varname
		} );

		result = result.SubString( 0, it->Start ) + glslReplace + result.SubString( it->Start + it->Length, INT_MAX );

		int32_t off = glslReplace.Length() - it->Length;
		for ( uint32_t p = ( uint32_t )( it - loaddirs.First() ); p < loaddirs.Count(); ++p )
		{
			loaddirs[ p ].Start += off;
		}

		++it;
	}

	return result;
}

//=====================================================================================
void GraphicsManager::Init()
{
}

//=====================================================================================
void GraphicsManager::InitPost()
{
	PROFILE;

	m_TextShader = LoadShader( WSID( "_Internal_TextShaderPerfectEdges" ), "text.shader", WSID( "Text_VMain" ), WSID( "Text_FPerfectEdges" ) );
	m_TextShaderGradiented = LoadShader( WSID( "_Internal_TextShaderPerfectEdgesGradiented" ), "text.shader", WSID( "Text_VMain" ), WSID( "Text_FPerfectEdgesGradiented" ) );
	m_TextShaderGlyphs = LoadShader( WSID( "_Internal_TextShaderGlyphPerfectEdges" ), "text.shader", WSID( "Text_VGlyph" ), WSID( "Text_FPerfectEdges" ) );
	m_TextShaderGlyphsGradiented = LoadShader( WSID( "_Internal_TextShaderGlyphGradientedPerfectEdges" ), "text.shader", WSID( "Text_VGlyph" ), WSID( "Text_FPerfectEdgesGradiented" ) );
	m_SimpleColourShader = LoadShader( WSID( "_Internal_SimpleColourShader" ), "colour.shader", WSID( "Col_VMain" ), WSID( "Col_FMain" ) );
	m_TextureColourExplicitUVShader = LoadShader( WSID( "_Internal_TextureColourExplicitUVShader" ), "texture.shader", WSID( "Col_VExplicitUV" ), WSID( "Col_FTextureColour" ) );

	m_BadStringTexture = ResolveTextTexture( "[ERROR]", m_DefaultFont, false );

	m_01Quad = GLMesh::CreateQuad( "_Internal_01Quad", Vector2::ZERO, Vector2::ONE );
	m_01LineQuad = new GLMesh( "_Internal_01LineQuad", GLMesh::VertexFormat::VF_POS2_TEX2, {
						0.0F, 0.0F, 0.0F, 0.0F, 
						0.0F, 1.0F, 0.0F, 0.0F, 
						1.0F, 1.0F, 0.0F, 0.0F, 
						1.0F, 0.0F, 0.0F, 0.0F },
						{ 0, 1, 1, 2, 2, 3, 3, 0 }, 
						GLMesh::DrawMode::DM_LINES );

	SetColour( Colour::WHITE, ColourUsage::COL_PRIMARY );
	SetColour( Colour::GRAY, ColourUsage::COL_SECONDARY );
	SetColour( Colour::BLACK, ColourUsage::COL_BACKGROUND );
}

//=====================================================================================
void GraphicsManager::Tick( float a_Dt )
{
	if ( false )
	{
		const float SIZE_PX = 48.0F;
		const float PADDING_PX = 4.0F;

		GraphicsManager::Instance().TfPush();
		GraphicsManager::Instance().TfTranslate( Vector2( 0.0F ), false, true );

		auto tempArray = m_Textures.GetEntries();
		const auto begin = m_Textures.GetEntries().First();
		const auto end = m_Textures.GetEntries().End();
		auto it = begin;
		while ( it != end )
		{
			CString keyStr = CString().Format( "0x%u", it->Key );
			GraphicsManager::Instance().SetColour( Colour::WHITE, COL_PRIMARY );
			int32_t sampler = GraphicsManager::Instance().SetTexture( Texture( it->Value ), 0 );
			GraphicsManager::Instance().GfxDrawQuadTextured( Vector2( 0.0F, ( ( it - begin ) + 1 ) * -( SIZE_PX + PADDING_PX ) + PADDING_PX ), Vector2( SIZE_PX ), sampler );
			++it;
		}

		GraphicsManager::Instance().TfPop();
	}

	auto * values = m_TextCache.GetValues();	
	if ( m_TextCache.Count() > 0 )
	for ( int32_t k = m_TextCache.Count() - 1; k >= 0; --k )
	{
		auto & val = values[ k ];
		++val.m_IdleFrames;
	}
}

//=====================================================================================
void GraphicsManager::Finalise()
{
	UnloadShader( m_TextShader );
	UnloadShader( m_TextShaderGradiented );
	UnloadShader( m_TextShaderGlyphs );
	UnloadShader( m_TextShaderGlyphsGradiented );
	UnloadShader( m_SimpleColourShader );
	UnloadShader( m_TextureColourExplicitUVShader );

	delete m_01Quad;
	m_01Quad = nullptr;

	delete m_01LineQuad;
	m_01LineQuad = nullptr;
}

//=====================================================================================
GraphicsManager::Shader GraphicsManager::LoadShader( uint32_t a_HashName, const char * a_Path, uint32_t a_VertexShader, uint32_t a_FragmentShader )
{
	if ( a_FragmentShader == 0 )
	{
		a_FragmentShader = a_VertexShader;
	}

	if ( !m_Shaders.Contains( a_HashName ) )
	{
		FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE, "res/shaders" );
		CString shaderSrc = FileManager::Instance().ReadAll( a_Path );
		Array< CString > shaderSources = shaderSrc.Split( "#BEGIN " );

		Array< Tuple2< Texture, uint32_t > > runtimeTextures;
		GLShader::Source vSrc;
		GLShader::Source fSrc;
		bool foundVSrc = false;
		bool foundFSrc = false;

		auto it = shaderSources.First() + 1;
		const auto end = shaderSources.End();
		while ( it < end && !( foundFSrc && foundVSrc ) )
		{
			int32_t end = it->Find( " >>\n" );
			CString pre = it->SubString( 0, end );

			int32_t space = pre.Find( " ", 1 );
			char shaderTypeC = pre.SubString( 0, space )[ 0 ];
			GLShader::ShaderType shaderType = ( shaderTypeC == 'V' ) ? GLShader::VERTEX : GLShader::FRAGMENT;
			CString shaderNameS = pre.SubString( space + 1, INT_MAX ).Trim();
			uint32_t shaderName = WSID( shaderNameS.Get() );
			CString post = it->SubString( end + CString( " >>\n" ).Length(), INT_MAX );

			switch ( shaderType )
			{
			case GLShader::VERTEX:
				if ( ( shaderName == a_VertexShader ) || 
					 ( a_VertexShader == 0 && shaderNameS.Length() == 0 ) )
				{
					Array< ShaderTextureLoader > loaders;
					post = PreprocessComments( post, a_Path );
					post = PreprocessIncludes( post, a_Path );
					post = PreprocessLoadDirectives( post, a_Path, loaders );

					auto loaderIt = loaders.First();
					const auto loaderEnd = loaders.End();
					while ( loaderIt != loaderEnd )
					{
						runtimeTextures.Append( Tuple2< Texture, uint32_t >( 
							GraphicsManager::Instance().LoadTexture( WSID( loaderIt->VarName.Get() ), loaderIt->Path.Get(), 
							GraphicsManager::TextureResidency::TEXRES_VIDEO_MEMORY, loaderIt->Filter, loaderIt->Wrap, loaderIt->MipsEnabled, loaderIt->Type ), loaderIt->TexUnit ) );
						++loaderIt;
					}

					vSrc = GLShader::Source{ GLShader::VERTEX, post };
					foundVSrc = true;
				}
				break;
			case GLShader::FRAGMENT:
				if ( ( shaderName == a_FragmentShader ) || 
					 ( a_FragmentShader == 0 && shaderNameS.Length() == 0 ) )
				{
					Array< ShaderTextureLoader > loaders;
					post = PreprocessComments( post, a_Path );
					post = PreprocessIncludes( post, a_Path );
					post = PreprocessLoadDirectives( post, a_Path, loaders );

					auto loaderIt = loaders.First();
					const auto loaderEnd = loaders.End();
					while ( loaderIt != loaderEnd )
					{
						runtimeTextures.Append( Tuple2< Texture, uint32_t >( 
							GraphicsManager::Instance().LoadTexture( WSID( loaderIt->VarName.Get() ), loaderIt->Path.Get(), 
							GraphicsManager::TextureResidency::TEXRES_VIDEO_MEMORY, loaderIt->Filter, loaderIt->Wrap, loaderIt->MipsEnabled, loaderIt->Type ), loaderIt->TexUnit ) );
						++loaderIt;
					}

					fSrc = GLShader::Source{ GLShader::FRAGMENT, post };
					foundFSrc = true;
				}
				break;
			default:
				break;
			}

			++it;
		}

		if ( !ASSERT_WARN( foundVSrc, "GraphicsManager - There is no vertex shader by the CRC32 '%u' in shader file '%s'", a_VertexShader, a_Path ) )
		{
			return Shader();
		}

		if ( !ASSERT_WARN( foundFSrc, "GraphicsManager - There is no fragment shader by the CRC32 '%u' in shader file '%s'", a_FragmentShader, a_Path ) )
		{
			return Shader();
		}

		GLShader * shader = new GLShader( CString().Format( "GLShader_%u", a_HashName ).Get(), 
		{
			vSrc,
			fSrc,
		} );

		PShader::PtrType ushader( shader, runtimeTextures );
		m_Shaders.Put( a_HashName, PShader( ushader ) );
	}
	
	return Shader( *m_Shaders[ a_HashName ] );
}

//=====================================================================================
GraphicsManager::Texture GraphicsManager::LoadTexture( uint32_t a_HashName, const char * a_Path, TextureResidency a_TextureResidency, TextureFilterMode a_Filter, TextureWrapMode a_Wrap, bool a_EnableMipmapping, TextureType a_TextureType )
{
	if ( !m_Textures.Contains( a_HashName ) )
	{
		SDL_Surface * surface = IMG_Load( a_Path );

		if ( ASSERT( surface, "Failed to load texture [%s]: %s", a_Path, IMG_GetError() ) )
		{
			SDL_Surface * rgbSurface = SDL_CreateRGBSurface( 0, surface->w, surface->h, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 );
			SDL_BlitSurface( surface, nullptr, rgbSurface, nullptr );

			PTexture::PtrType tup3(
				( a_TextureResidency & TEXRES_VIDEO_MEMORY ) ? new GLTexture( CString().Format( "GLTexture_%u", a_HashName ).Get(), rgbSurface, a_Filter, a_Wrap, a_EnableMipmapping, a_TextureType, 0 ) : nullptr,
				( a_TextureResidency & TEXRES_HOST_MEMORY ) ? surface : nullptr,
				0 );
			m_Textures.Put( a_HashName, PTexture( tup3 ) );

			if ( ( a_TextureResidency & TEXRES_HOST_MEMORY ) == 0 )
			{
				SDL_FreeSurface( rgbSurface );
				SDL_FreeSurface( surface );
			}
		}

		else
		{
			return Texture();
		}
	}
	
	return Texture( *m_Textures[ a_HashName ] );
}

//=====================================================================================
GraphicsManager::Texture GraphicsManager::LoadTexture( uint32_t a_HashName, uint32_t a_Width, uint32_t a_Height, void * a_Pixels, PixelFormat a_PixelFormat, TextureFilterMode a_Filter, TextureWrapMode a_Wrap, bool a_EnableMipmapping, TextureType a_TextureType )
{
	if ( !m_Textures.Contains( a_HashName ) )
	{
		uint32_t depth;
		uint32_t format;

		switch ( a_PixelFormat )
		{
		case GraphicsManager::PF_RGBA8:
			depth = 32;
			format = SDL_PIXELFORMAT_RGBA32;
			break;
		case GraphicsManager::PF_RGB8:
		default:
			depth = 24;
			format = SDL_PIXELFORMAT_RGB24;
			break;
		}

		SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormatFrom( a_Pixels, a_Width, a_Height, depth, a_Width * depth, format );

		if ( ASSERT( surface, "Failed to construct texture: %s", IMG_GetError() ) )
		{
			PTexture::PtrType tup3( new GLTexture( CString().Format( "GLTexture_%u", a_HashName ).Get(), surface, a_Filter, a_Wrap, a_EnableMipmapping, a_TextureType, 0 ),
								    nullptr,
									0 );
			m_Textures.Put( a_HashName, PTexture( tup3 ) );
			SDL_FreeSurface( surface );
		}

		else
		{
			return Texture();
		}
	}
	
	return Texture( *m_Textures[ a_HashName ] );
}

//=====================================================================================
GraphicsManager::TextureAtlas GraphicsManager::LoadTextureAsAtlas( uint32_t a_HashName, const char * a_Path, const Vector2 & a_SpritesPerAxis )
{
	if ( !m_TextureAtlases.Contains( a_HashName ) )
	{
		m_TextureAtlases.Put( a_HashName, PTextureAtlas( SpriteSheet( LoadTexture( a_HashName, a_Path ), a_SpritesPerAxis ) ) );
	}

	if ( !m_Textures.Contains( a_HashName ) )
	{
		LoadTexture( a_HashName, a_Path );
	}

	return TextureAtlas( *m_TextureAtlases[ a_HashName ] );
}

//=====================================================================================
GraphicsManager::TextureAtlas GraphicsManager::LoadAtlas( uint32_t a_HashName, const SpriteSheet & a_SpriteSheet )
{
	if ( !m_TextureAtlases.Contains( a_HashName ) )
	{
		m_TextureAtlases.Put( a_HashName, PTextureAtlas( a_SpriteSheet ) );
	}
	
	return TextureAtlas( *m_TextureAtlases[ a_HashName ] );
}

//=====================================================================================
GraphicsManager::Font GraphicsManager::LoadFont( uint32_t a_HashName, const char * a_Path, uint32_t a_FontSize )
{
	if ( !m_Fonts.Contains( a_HashName ) )
	{
		TTF_Font * font = TTF_OpenFont( a_Path, a_FontSize );

		if ( ASSERT( font, "", a_Path, TTF_GetError() ) )
		{
			m_Fonts.Put( a_HashName, PFont( PFont::PtrType( a_Path, font, a_FontSize ) ) );

			if ( m_Fonts.Count() == 1 )
			{
				m_DefaultFont = Font( m_Fonts.GetEntries()[ 0U ].Value );
			}
		}

		else if ( m_Fonts.Count() > 0 )
		{
			return m_DefaultFont;
		}

		else
		{
			return Font();
		}
	}
	
	return Font( *m_Fonts[ a_HashName ] );
}

//=====================================================================================
GraphicsManager::BitmapFont GraphicsManager::LoadBitmapFont( uint32_t a_HashName, const char * a_CharacterSet, const Font & a_Font, uint32_t a_FontSize )
{
	if ( !a_CharacterSet || a_CharacterSet[ 0 ] == 0 )
	{
		return BitmapFont();
	}

	if ( !m_BitmapFonts.Contains( a_HashName ) )
	{
		Font font = a_Font ? a_Font : m_DefaultFont;

		if ( ASSERT( font, "Invalid font for BitmapFont [%u]", a_HashName ) )
		{
			uint32_t prevFontPt = font->Value2;
			SetFontSize( font, static_cast< uint32_t >( Floor( a_FontSize ) ) );

			CString charsetProper = a_CharacterSet;

			char ch[ 2 ] = { ' ', 0 };

			Texture bitmap = GfxGenTextStatic( charsetProper.Get(), font, a_FontSize );
			int32_t bitmapSizeX;
			int32_t bitmapSizeY;
			TTF_SizeText( reinterpret_cast< TTF_Font* >( font->Value1 ), charsetProper.Get(), &bitmapSizeX, &bitmapSizeY );
			Vector2 bitmapSize( bitmapSizeX, bitmapSizeY );

			StaticArray< Tuple3< uint16_t, Vector2, Vector2 >, 256 > glyphs( Tuple3< uint16_t, Vector2, Vector2 >(), 256 );
			
			uint16_t ySize = 0;
			int32_t width, height;
			int32_t prevOff = 0;

			for ( uint32_t i = 0; i < charsetProper.Length(); ++i )
			{				
				char c = charsetProper[ i ];

				if ( charsetProper[ i ] < 0 || charsetProper[ i ] > 255 )
				{
					continue;
				}

				ch[ 0 ] = c;

				TTF_SizeText( reinterpret_cast< TTF_Font* >( font->Value1 ), charsetProper.SubString( 0, i ).Get(), &prevOff, nullptr );
				TTF_SizeText( reinterpret_cast< TTF_Font* >( font->Value1 ), ch, &width, &height );
				ySize = Max( ySize, static_cast< uint16_t >( height ) );
				auto & glyph = glyphs[ static_cast< uint32_t >( charsetProper[ i ] ) ];
				glyph.Value0 = static_cast< uint16_t >( width );
				glyph.Value1 = Vector2( prevOff, 0.0F ) / bitmapSize;
				glyph.Value2 = Vector2( width, bitmapSize.y ) / bitmapSize;
			}
			
			SetFontSize( font, prevFontPt );
			m_BitmapFonts.Put( a_HashName, PBitmapFont( BitmapFont::PtrType( bitmap, ySize, glyphs ) ) );
		}
		else
		{
			return BitmapFont();
		}
	}
	
	return BitmapFont( *m_BitmapFonts[ a_HashName ] );
}

//=====================================================================================
GraphicsManager::RenderTexture GraphicsManager::AllocateRenderTexture( uint32_t a_HashName, const Vector2 & a_Size,
																	   uint32_t a_NumColourTextures,
																	   PixelFormat a_PixelFormat, TextureFilterMode a_Filter,
																	   TextureWrapMode a_Wrap, bool a_EnableMipmapping )
{
	if ( !m_RenderTextures.Contains( a_HashName ) )
	{
		m_RenderTextures.Put( a_HashName, PRenderTexture( new GLRenderTexture( CString().Format( "RenderTexture_%u", a_HashName ).Get(), 
																			   Clamp( static_cast< uint32_t >( a_Size.x ), 1U, 8192U ),
																			   Clamp( static_cast< uint32_t >( a_Size.y ), 1U, 8192U ),
																			   a_NumColourTextures,
																			   a_EnableMipmapping, a_Filter, a_Wrap ) ) );
	}
	
	return RenderTexture( *m_RenderTextures[ a_HashName ] );
}

//=====================================================================================
bool GraphicsManager::DeallocateRenderTexture( uint32_t a_HashName )
{
	if ( ASSERT_WARN( m_RenderTextures.Contains( a_HashName ), "(%s) The rendertexture by ID '%u' does not exist, ignoring.", __FUNCTION__, a_HashName ) )
	{
		PRenderTexture & rt = *m_RenderTextures[ a_HashName ];
		delete reinterpret_cast< GLRenderTexture* >( *rt.Ptr() );
		return m_RenderTextures.Remove( a_HashName );
	}

	return false;
}

//=====================================================================================
bool GraphicsManager::DeallocateRenderTexture( RenderTexture & a_RenderTexture )
{
	auto pair = m_RenderTextures.GetEntries().Find< const RenderTexture & >( []( const RenderTexture & a_It, const Dictionary< uint32_t, PRenderTexture >::Pair & a_Pair ) {
		return a_Pair.Value == a_It;
	}, a_RenderTexture );
	
	if ( pair )
	{
		return DeallocateRenderTexture( pair->Key );
	}

	return false;
}

//=====================================================================================
void GraphicsManager::SetFontSize( uint32_t a_HashName, uint32_t a_FontSize )
{
	PFont * font = m_Fonts[ a_HashName ];
	if ( font )
	{
		PFont & theFont = *font;
		
		if ( theFont->Value2 == a_FontSize )
		{
			return;
		}
		
		if ( ASSERT( theFont->Value1, "No font data associated with font [%u] at path \"\"", a_HashName, theFont->Value0.Get() ) )
		{
			TTF_CloseFont( reinterpret_cast< TTF_Font* >( theFont->Value1 ) );
		}

		theFont->Value1 = TTF_OpenFont( theFont->Value0.Get(), a_FontSize );

		if ( !ASSERT( theFont->Value1, "Failed to reload font [%u]: ERROR = %s", TTF_GetError() ) )
		{
			theFont->Value2 = 0;
			theFont->Value1 = nullptr;
			theFont->Value0 = "";

			m_Fonts.Remove( a_HashName );
		}

		else
		{
			theFont->Value2 = a_FontSize;
		}
	}
}

//=====================================================================================
bool GraphicsManager::UnloadFont( uint32_t a_HashName )
{
	if ( ASSERT_WARN( m_Fonts.Contains( a_HashName ), "(%s) The font by ID '%u' does not exist, ignoring.", __FUNCTION__, a_HashName ) )
	{
		PFont & font = *m_Fonts[ a_HashName ];
		TTF_CloseFont( reinterpret_cast< TTF_Font* >( font->Value1 ) );
		return m_Fonts.Remove( a_HashName );
	}

	return false;
}

//=====================================================================================
bool GraphicsManager::UnloadBitmapFont( uint32_t a_HashName )
{
	if ( ASSERT_WARN( m_BitmapFonts.Contains( a_HashName ), "(%s) The font by ID '%u' does not exist, ignoring.", __FUNCTION__, a_HashName ) )
	{
		PBitmapFont & bmfont = *m_BitmapFonts[ a_HashName ];
		bool r1 = UnloadTexture( bmfont->Value0 );
		bool r2 = m_BitmapFonts.Remove( a_HashName );
		return r1 && r2;
	}

	return false;
}

//=====================================================================================
bool GraphicsManager::UnloadTexture( uint32_t a_HashName )
{
	if ( ASSERT_WARN( m_Textures.Contains( a_HashName ), "(%s) The texture by ID '%u' does not exist, ignoring.", __FUNCTION__, a_HashName ) )
	{
		PTexture & texture = *m_Textures[ a_HashName ];
		PTexture::PtrType * textureTup = texture.Ptr();
		if ( textureTup->Value0 )
		{
			GLTexture * gltex = reinterpret_cast< GLTexture* >( textureTup->Value0 );
			delete gltex;
		}

		if ( textureTup->Value1 )
		{
			SDL_FreeSurface( reinterpret_cast< SDL_Surface* >( textureTup->Value1 ) );
		}
		
		return m_Textures.Remove( a_HashName );
	}

	return false;
}

//=====================================================================================
bool GraphicsManager::UnloadAtlas( uint32_t a_HashName )
{
	return m_TextureAtlases.Remove( a_HashName );
}

//=====================================================================================
bool GraphicsManager::UnloadShader( uint32_t a_HashName )
{
	if ( ASSERT_WARN( m_Shaders.Contains( a_HashName ), "(%s) The shader by ID '%u' does not exist, ignoring.", __FUNCTION__, a_HashName ) )
	{
		PShader & shader = *m_Shaders[ a_HashName ];
		ASSERT( shader->m_Owner, "(%s) The shader pointer storing object '%u' [UShader: '%s'] is not the owner! This is a bug!", __FUNCTION__, a_HashName, shader->m_PtrGLShader->Name() );

		delete shader->m_PtrGLShader;

		auto it = shader->m_RuntimeTextures.First();
		const auto end = shader->m_RuntimeTextures.End();
		while ( it != end )
		{
			typedef const Tuple2< Texture, uint32_t > & Iter;
			const uint32_t key = m_Textures.GetEntries().Find< Iter >( []( Iter a_It, const Dictionary< uint32_t, PTexture >::Pair & a_Pair ) {
				return *a_Pair.Value.Ptr() == *a_It.Value0.Ptr();
			}, *it )->Key;

			UnloadTexture( key );

			++it;
		}

		return m_Shaders.Remove( a_HashName );
	}

	return false;
}

//=====================================================================================
GraphicsManager::RenderTexture GraphicsManager::GetRenderTexture( uint32_t a_HashName ) const
{
	PRenderTexture * ptr = m_RenderTextures[ a_HashName ];
	return ptr ? RenderTexture( *ptr ) : RenderTexture();
}

//=====================================================================================
GraphicsManager::Texture GraphicsManager::GetTexture( uint32_t a_HashName ) const
{
	PTexture * ptr = m_Textures[ a_HashName ];
	return ptr ? Texture( *ptr ) : Texture();
}

//=====================================================================================
GraphicsManager::TextureAtlas GraphicsManager::GetAtlas( uint32_t a_HashName ) const
{
	PTextureAtlas * ptr = m_TextureAtlases[ a_HashName ];
	return ptr ? TextureAtlas( *ptr ) : TextureAtlas();
}

//=====================================================================================
GraphicsManager::Font GraphicsManager::GetFont( uint32_t a_HashName ) const
{
	PFont * ptr = m_Fonts[ a_HashName ];
	return ptr ? Font( *ptr ) : Font();
}

//=====================================================================================
GraphicsManager::BitmapFont GraphicsManager::GetBitmapFont( uint32_t a_HashName ) const
{
	PBitmapFont * ptr = m_BitmapFonts[ a_HashName ];
	return ptr ? BitmapFont( *ptr ) : BitmapFont();
}

//=====================================================================================
GraphicsManager::Shader GraphicsManager::GetShader( uint32_t a_HashName ) const
{
	PShader * ptr = m_Shaders[ a_HashName ];
	return ptr ? Shader( *ptr ) : Shader();
}

//=====================================================================================
void GraphicsManager::SetFontSize( Font & a_Font, uint32_t a_FontSize )
{
	auto pair = m_Fonts.GetEntries().Find< const Font & >( []( const Font & a_It, const Dictionary< uint32_t, PFont >::Pair & a_Pair ) {
		return a_Pair.Value == a_It;
	}, a_Font );
	
	if ( pair )
	{
		SetFontSize( pair->Key, a_FontSize );
	}
}

//=====================================================================================
bool GraphicsManager::UnloadFont( Font & a_Font )
{
	auto pair = m_Fonts.GetEntries().Find< const Font & >( []( const Font & a_It, const Dictionary< uint32_t, PFont >::Pair & a_Pair ) {
		return a_Pair.Value == a_It;
	}, a_Font );
	
	if ( pair )
	{
		return UnloadFont( pair->Key );
	}

	return false;
}

//=====================================================================================
bool GraphicsManager::UnloadBitmapFont( BitmapFont & a_BitmapFont )
{
	auto pair = m_BitmapFonts.GetEntries().Find< const BitmapFont & >( []( const BitmapFont & a_It, const Dictionary< uint32_t, PBitmapFont >::Pair & a_Pair ) {
		return a_Pair.Value == a_It;
	}, a_BitmapFont );
	
	if ( pair )
	{
		return UnloadFont( pair->Key );
	}

	return false;
}

//=====================================================================================
bool GraphicsManager::UnloadTexture( Texture & a_Texture )
{
	auto pair = m_Textures.GetEntries().Find< const Texture & >( []( const Texture & a_It, const Dictionary< uint32_t, PTexture >::Pair & a_Pair ) {
		return a_Pair.Value == a_It;
	}, a_Texture );
	
	if ( pair )
	{
		return UnloadTexture( pair->Key );
	}

	return false;
}

//=====================================================================================
bool GraphicsManager::UnloadAtlas( TextureAtlas & a_TextureAtlas )
{
	auto pair = m_TextureAtlases.GetEntries().Find< const TextureAtlas & >( []( const TextureAtlas & a_It, const Dictionary< uint32_t, PTextureAtlas >::Pair & a_Pair ) {
		return a_Pair.Value == a_It;
	}, a_TextureAtlas );
	
	if ( pair )
	{
		return UnloadAtlas( pair->Key );
	}

	return false;
}

//=====================================================================================
bool GraphicsManager::UnloadShader( Shader & a_Shader )
{
	auto pair = m_Shaders.GetEntries().Find< const Shader & >( []( const Shader & a_It, const Dictionary< uint32_t, PShader >::Pair & a_Pair ) {
		return a_Pair.Value == a_It;
	}, a_Shader );
	
	if ( pair )
	{
		return UnloadShader( pair->Key );
	}

	return false;
}

//=====================================================================================
void GraphicsManager::SetShader( Shader & a_Shader )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__, __FUNCTION__ ) )
	{
		for ( uint32_t k = 0; k < a_Shader->m_RuntimeTextures.Count(); ++k )
		{
			void * gltptr			= a_Shader->m_RuntimeTextures[ k ].Value0->Value0;
			const uint32_t texunit  = a_Shader->m_RuntimeTextures[ k ].Value1;

			if ( !ASSERT_WARN( gltptr, "Shader '%s' trying to bind invalid runtime texture to unit %u! Appears to be a corrupted state. Skipping operation...", a_Shader->m_PtrGLShader->Name(), texunit ) )
			{
				continue;
			}

			GLDraw::Instance().BindTexture( reinterpret_cast< GLTexture * >( gltptr ), texunit );
		}
		GLDraw::Instance().UseShader( a_Shader->m_PtrGLShader );
	}
}

//=====================================================================================
int32_t GraphicsManager::GetShaderGlobalLocation( Shader & a_Shader, const char * a_PropertyName )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__ ) )
	{
		return a_Shader->m_PtrGLShader->GetUniform( a_PropertyName );
	}

	return -1;
}

//=====================================================================================
void GraphicsManager::SetShaderGlobalInt32( Shader & a_Shader, int32_t a_Loc, int32_t a_Value )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__ ) )
	{
		a_Shader->m_PtrGLShader->SetUniformInt32( a_Loc, a_Value );
	}
}

//=====================================================================================
void GraphicsManager::SetShaderGlobalFloat1( Shader & a_Shader, int32_t a_Loc, float a_Value )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__ ) )
	{
		a_Shader->m_PtrGLShader->SetUniformFloat1( a_Loc, a_Value );
	}
}

//=====================================================================================
void GraphicsManager::SetShaderGlobalFloat2( Shader & a_Shader, int32_t a_Loc, const Vector2 & a_Value )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__ ) )
	{
		a_Shader->m_PtrGLShader->SetUniformFloat2( a_Loc, a_Value );
	}
}

//=====================================================================================
void GraphicsManager::SetShaderGlobalFloat3( Shader & a_Shader, int32_t a_Loc, const float * a_ValueFloat3 )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__ ) )
	{
		a_Shader->m_PtrGLShader->SetUniformFloat3( a_Loc, a_ValueFloat3 );
	}
}

//=====================================================================================
void GraphicsManager::SetShaderGlobalFloat3x3( Shader & a_Shader, int32_t a_Loc, const float * a_ValueFloat3x3, bool a_Transpose )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__ ) )
	{
		a_Shader->m_PtrGLShader->SetUniformFloat3x3( a_Loc, a_ValueFloat3x3, a_Transpose );
	}
}

//=====================================================================================
void GraphicsManager::SetShaderGlobalColour( Shader & a_Shader, int32_t a_Loc, const Colour & a_Value )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__ ) )
	{
		a_Shader->m_PtrGLShader->SetUniformColour( a_Loc, a_Value );
	}
}

//=====================================================================================
void GraphicsManager::SetShaderGlobalFloat1Array( Shader & a_Shader, int32_t a_Loc, const Array< float > & a_Values )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__ ) )
	{
		a_Shader->m_PtrGLShader->SetUniformFloat1Array( a_Loc, a_Values );
	}
}

//=====================================================================================
void GraphicsManager::SetShaderGlobalFloat2Array( Shader & a_Shader, int32_t a_Loc, const Array< Vector2 > & a_Values )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__ ) )
	{
		a_Shader->m_PtrGLShader->SetUniformFloat2Array( a_Loc, a_Values );
	}
}

//=====================================================================================
void GraphicsManager::SetShaderGlobalColourArray( Shader & a_Shader, int32_t a_Loc, const Array< Colour > & a_Values )
{
	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__ ) )
	{
		a_Shader->m_PtrGLShader->SetUniformColourArray( a_Loc, a_Values );
	}
}

//=====================================================================================
void GraphicsManager::SetShaderGlobalSampler( Shader & a_Shader, int32_t a_Loc, uint32_t a_TextureUnit )
{
	if ( !ASSERT_WARN( a_TextureUnit >= 0, "%s -> Invalud texture unit [%u]", __FUNCTION__, a_TextureUnit ) )
	{
		return;
	}

	if ( ASSERT( a_Shader && a_Shader->m_PtrGLShader, "%s -> Invalid shader [null]", __FUNCTION__, __FUNCTION__ ) )
	{
		a_Shader->m_PtrGLShader->SetUniformSampler( a_Loc, a_TextureUnit );
	}
}

//=====================================================================================
int32_t GraphicsManager::SetTexture( const Texture & a_Texture, uint32_t a_UnitOrFlags )
{
	auto indexOf = [&]()
	{
		auto it = m_TextureBindings.Find< const Texture & >( []( const Texture & a_Texture, const TextureBinding & a_TextureBinding )
		{
			return a_TextureBinding._Texture == a_Texture;
		}, a_Texture );

		if ( it )
		{
			return static_cast< int32_t >( it - m_TextureBindings.First() );
		}

		return static_cast< int32_t  >( -1 );
	};

	int32_t ind;
	if ( ASSERT( a_Texture, "GraphicsManager::SetTexture: Texture is nil!" ) && 
		 ASSERT( a_Texture->Value0, "GraphicsManager::SetTexture: Texture is not stored in video memory!" ) )
	{
		if ( a_UnitOrFlags & TS_UNSET )
		{
			if ( ( ind = indexOf() ) > -1 )
			{
				GLDraw::Instance().BindTexture( ( GLTexture* )nullptr, ind );
				m_TextureBindings[ ( uint32_t )ind ]._Texture = nullptr;
				return -1;
			}
		}

		else
		{
			const bool nextAvailable = a_UnitOrFlags & TS_NEXT_AVAILABLE;
			
			if ( nextAvailable )
			{
				if ( ( ind = indexOf() ) != -1 )
				{
					return ind;
				}

				const bool forceIfFull = a_UnitOrFlags & TS_FORCE_LAST_IF_FULL;

				GLint units;
				glGetIntegerv( GL_MAX_IMAGE_UNITS, &units );

				for ( uint32_t k = 0; k < Min( 8U, ( uint32_t )units ); ++k )
				{
					if ( !m_TextureBindings[ k ] )
					{
						GLDraw::Instance().BindTexture( reinterpret_cast< GLTexture* >( a_Texture->Value0 ), k );
						m_TextureBindings[ k ]._Texture = a_Texture;

						return k;
					}
				}

				if ( forceIfFull )
				{
					GLDraw::Instance().BindTexture( reinterpret_cast< GLTexture* >( a_Texture->Value0 ), 0 );
					m_TextureBindings[ 0U ]._RenderTextureBind._RT = nullptr;
					m_TextureBindings[ 0U ]._Texture = a_Texture;
					return 0;
				}
			}

			// a_UnitOrFlags = just a unit
			else
			{
				// unset first
				if ( ( ind = indexOf() ) > -1 )
				{
					GLDraw::Instance().BindTexture( ( GLTexture* )nullptr, ind );
					m_TextureBindings[ ( uint32_t )ind ]._Texture = nullptr;
					m_TextureBindings[ ( uint32_t )ind ]._RenderTextureBind._RT = nullptr;
				}

				GLDraw::Instance().BindTexture( reinterpret_cast< GLTexture* >( a_Texture->Value0 ), a_UnitOrFlags );
				m_TextureBindings[ a_UnitOrFlags ]._Texture = a_Texture;
				return a_UnitOrFlags;
			}
		}
	}

	return -1;
}

int32_t GraphicsManager::SetTexture( const RenderTexture & a_RenderTexture, uint32_t a_ColourIndex, uint32_t a_UnitOrFlags, bool a_RebuildMips )
{
	auto indexOf = [&]()
	{
		struct dat
		{
			const RenderTexture & rt;
			uint32_t ci;
		};

		auto it = m_TextureBindings.Find< const dat & >([]( const dat & a_Dat, const TextureBinding & a_TextureBinding )
		{
			return a_TextureBinding._RenderTextureBind._RT == a_Dat.rt &&
				   a_TextureBinding._RenderTextureBind._ColourIndex == a_Dat.ci;
		}, dat{ a_RenderTexture, a_ColourIndex } );

		if ( it )
		{
			return static_cast< int32_t >( it - m_TextureBindings.First() );
		}

		return static_cast< int32_t  >( -1 );
	};

	int32_t ind;
	GLRenderTexture * glrt;
	if ( ASSERT( a_RenderTexture, "GraphicsManager::SetTexture: RenderTexture is nil!" ) && 
		 ASSERT( glrt = reinterpret_cast< GLRenderTexture* >( *a_RenderTexture.Ptr() ), "GraphicsManager::SetTexture: Corrupt rendertexture!" ) )
	{
		if ( a_UnitOrFlags & TS_UNSET )
		{
			if ( ( ind = indexOf() ) > -1 )
			{
				GLDraw::Instance().BindTexture( ( GLRenderTexture* )nullptr, ind, a_ColourIndex );
				m_TextureBindings[ ( uint32_t )ind ]._RenderTextureBind._RT = nullptr;
				return -1;
			}
		}

		else
		{
			const bool nextAvailable = a_UnitOrFlags & TS_NEXT_AVAILABLE;
			
			if ( nextAvailable )
			{
				if ( ( ind = indexOf() ) != -1 )
				{
					return ind;
				}

				const bool forceIfFull = a_UnitOrFlags & TS_FORCE_LAST_IF_FULL;

				GLint units;
				glGetIntegerv( GL_MAX_IMAGE_UNITS, &units );

				for ( uint32_t k = 0; k < Min( 8U, ( uint32_t )units ); ++k )
				{
					if ( !m_TextureBindings[ k ] )
					{
						GLDraw::Instance().BindTexture( glrt, k, a_ColourIndex, a_RebuildMips );
						m_TextureBindings[ k ]._RenderTextureBind._RT = a_RenderTexture;
						m_TextureBindings[ k ]._RenderTextureBind._ColourIndex = a_ColourIndex;

						return k;
					}
				}

				if ( forceIfFull )
				{
					GLDraw::Instance().BindTexture( glrt, 0, a_ColourIndex, a_RebuildMips );
					m_TextureBindings[ 0U ]._Texture = nullptr;
					m_TextureBindings[ 0U ]._RenderTextureBind._RT = a_RenderTexture;
					m_TextureBindings[ 0U ]._RenderTextureBind._ColourIndex = a_ColourIndex;
					return 0;
				}
			}

			// a_UnitOrFlags = just a unit
			else
			{
				// unset first
				if ( ( ind = indexOf() ) > -1 )
				{
					GLDraw::Instance().BindTexture( ( GLRenderTexture* )nullptr, ind, a_ColourIndex );
					m_TextureBindings[ ( uint32_t )ind ]._Texture = nullptr;
					m_TextureBindings[ ( uint32_t )ind ]._RenderTextureBind._RT = nullptr;
				}

				GLDraw::Instance().BindTexture( glrt, a_UnitOrFlags, a_ColourIndex, a_RebuildMips );
				m_TextureBindings[ ( uint32_t )a_UnitOrFlags ]._RenderTextureBind._RT = a_RenderTexture;
				m_TextureBindings[ ( uint32_t )a_UnitOrFlags ]._RenderTextureBind._ColourIndex = a_ColourIndex;
				return a_UnitOrFlags;
			}
		}
	}

	return -1;
}

//=====================================================================================
void GraphicsManager::TfPush()
{
	static const float identity[ 9 ] = {
		1.0F, 0.0F, 0.0F,
		0.0F, 1.0F, 0.0F,
		0.0F, 0.0F, 1.0F,
	};

	Float3x3 mat = Float3x3();
	Float3x3 vwp = Float3x3();
	Tuple2< Vector2, Vector2 > viewport = Tuple2< Vector2, Vector2 >( -Vector2::ONE, Vector2::ONE );
	if ( m_MatrixStack.Count() > 0 )
	{
		mat = make_mat3( m_MatrixStack.Peek()->Value0.Data );
		vwp = make_mat3( m_MatrixStack.Peek()->Value1.Data );
		viewport = m_MatrixStack.Peek()->Value2;
	}

	m_MatrixStack.Push( Tuple3< Mat3, Mat3, Tuple2< Vector2, Vector2 > >( Mat3( identity ), Mat3( identity ), 
																		  viewport ) );
	m_MatrixStack.Peek()->Value0 = Mat3( value_ptr( mat ) );
	m_MatrixStack.Peek()->Value1 = Mat3( value_ptr( vwp ) );
}

//=====================================================================================
void GraphicsManager::TfPushIdentity( bool a_PreserveViewport )	
{
	static const float identity[ 9 ] = {
		1.0F, 0.0F, 0.0F,
		0.0F, 1.0F, 0.0F,
		0.0F, 0.0F, 1.0F,
	};

	Float3x3 vwp = Float3x3();
	Tuple2< Vector2, Vector2 > viewport = Tuple2< Vector2, Vector2 >( -Vector2::ONE, Vector2::ONE );
	if ( m_MatrixStack.Count() > 0 && a_PreserveViewport )
	{
		vwp = make_mat3( m_MatrixStack.Peek()->Value1.Data );
		viewport = m_MatrixStack.Peek()->Value2;
	}

	m_MatrixStack.Push( Tuple3< Mat3, Mat3, Tuple2< Vector2, Vector2 > >( Mat3( identity ), Mat3( value_ptr( vwp ) ), 
																		  viewport ) );
}

//=====================================================================================
void GraphicsManager::TfTranslate( const Vector2 & a_Translation, bool a_FlipX, bool a_FlipY )
{
	if ( !ASSERT( m_MatrixStack.Count() > 0, "GraphicsManager: Failed - must have an existing matrix on the stack before making a call to '%s'", __FUNCTION__ ) )
	{
		return;
	}

	vec2 transl( a_Translation.x, a_Translation.y );

	if ( a_FlipX )
	{
		transl.x = m_MatrixStack.Peek()->Value2.Value1.x - transl.x;
	}

	if ( a_FlipY )
	{
		transl.y = m_MatrixStack.Peek()->Value2.Value1.y - transl.y;
	}

	Float3x3 mat = translate( make_mat3( m_MatrixStack.Peek()->Value0.Data ), transl );
	m_MatrixStack.Peek()->Value0 = Mat3( value_ptr( mat ) );
}

//=====================================================================================
void GraphicsManager::TfRotate( float a_Degrees )
{
	if ( !ASSERT( m_MatrixStack.Count() > 0, "GraphicsManager: Failed - must have an existing matrix on the stack before making a call to '%s'", __FUNCTION__ ) )
	{
		return;
	}

	Float3x3 mat = rotate( make_mat3( m_MatrixStack.Peek()->Value0.Data ), -radians( a_Degrees ) );
	m_MatrixStack.Peek()->Value0 = Mat3( value_ptr( mat ) );
}

//=====================================================================================
void GraphicsManager::TfScale( const Vector2 & a_Scale )
{
	if ( !ASSERT( m_MatrixStack.Count() > 0, "GraphicsManager: Failed - must have an existing matrix on the stack before making a call to '%s'", __FUNCTION__ ) )
	{
		return;
	}

	Float3x3 mat = scale( make_mat3( m_MatrixStack.Peek()->Value0.Data ), vec2( a_Scale.x, a_Scale.y ) );
	m_MatrixStack.Peek()->Value0 = Mat3( value_ptr( mat ) );
}

//=====================================================================================
void GraphicsManager::TfShear( const Vector2 & a_Shear )
{
	if ( !ASSERT( m_MatrixStack.Count() > 0, "GraphicsManager: Failed - must have an existing matrix on the stack before making a call to '%s'", __FUNCTION__ ) )
	{
		return;
	}

	Float3x3 mat = shearY( shearX( make_mat3( m_MatrixStack.Peek()->Value0.Data ), a_Shear.x ), a_Shear.y );
	m_MatrixStack.Peek()->Value0 = Mat3( value_ptr( mat ) );
}

//=====================================================================================
void GraphicsManager::TfViewport( const Vector2 & a_Position, const Vector2 & a_Size )
{
	if ( !ASSERT( m_MatrixStack.Count() > 0, "GraphicsManager: Failed - must have an existing matrix on the stack before making a call to '%s'", __FUNCTION__ ) )
	{
		return;
	}

	PROFILE;

	float left = a_Position.x;
	float bottom = a_Position.y;
	float right = left + a_Size.x;
	float top = bottom + a_Size.y;

	Float3x3 result;
	result[ 0 ][ 0 ] = 2.0F / ( right - left );
	result[ 1 ][ 1 ] = 2.0F / ( top - bottom );
	result[ 2 ][ 2 ] = 1.0F;
	result[ 2 ][ 0 ] = -( right + left ) / ( right - left );
	result[ 2 ][ 1 ] = -( top + bottom ) / ( top - bottom );

	m_MatrixStack.Peek()->Value1 = Mat3( value_ptr( result ) );
	m_MatrixStack.Peek()->Value2 = Tuple2< Vector2, Vector2 >( a_Position, a_Size );
}

//=====================================================================================
void GraphicsManager::TfPop()
{
	if ( !ASSERT( m_MatrixStack.Count() > 0, "GraphicsManager: Failed - must have an existing matrix on the stack before making a call to '%s'", __FUNCTION__ ) )
	{
		return;
	}

	m_MatrixStack.Pop();
}

//=====================================================================================
const float * GraphicsManager::TfGetTop( float * a_MultMatrix3x3 ) const
{
	PROFILE;

	static Float3x3 top;
	static const float * ptr = nullptr;
	static const float identity[ 9 ] = {
		1.0F, 0.0F, 0.0F,
		0.0F, 1.0F, 0.0F,
		0.0F, 0.0F, 1.0F,
	};

	if ( m_MatrixStack.Count() > 0 )
	{
		if ( a_MultMatrix3x3 )
		{
			top = make_mat3( m_MatrixStack.Peek()->Value1.Data ) * 
				  make_mat3( m_MatrixStack.Peek()->Value0.Data ) * 
				  make_mat3( a_MultMatrix3x3 );
		}

		else
		{
			
			top = make_mat3( m_MatrixStack.Peek()->Value1.Data ) * 
				  make_mat3( m_MatrixStack.Peek()->Value0.Data );
		}

		ptr = value_ptr( top );
	}

	else
	{
		ptr = a_MultMatrix3x3 ? a_MultMatrix3x3 : identity;
	}

	return ptr;
}

//=====================================================================================
void GraphicsManager::TfSetTop( float * a_SetMatrix3x3 ) const
{
	PROFILE;

	if ( ASSERT( a_SetMatrix3x3, "GraphicsManager::TfSetTop(..): Specified matrix ptr is pointing to location 0x00000000" ) &&
		 ASSERT( m_MatrixStack.Count() > 0, "GraphicsManager::TfSetTop(..): Matrix stack is empty." ) )
	{
		m_MatrixStack.Peek()->Value0 = Mat3( a_SetMatrix3x3 );
	}
}

//=====================================================================================
void GraphicsManager::TfMul( const float * a_Left, const float * a_Right, float * o_Result ) const
{
	BCopy( value_ptr( make_mat3( a_Left ) * make_mat3( a_Right ) ), o_Result, sizeof( float ) * 9 );
}

//=====================================================================================
void GraphicsManager::TfMakeTRS(float * o_Result, const Vector2 & a_Translation, float a_Rotation, const Vector2 & a_Scale) const
{
	 BCopy( value_ptr( translate( rotate( scale( mat3(), vec2( a_Scale.x, a_Scale.y ) ), a_Rotation * DEG2RAD ), vec2( a_Translation.x, a_Translation.y ) ) ), o_Result, sizeof( float ) * 9 );
}

//=====================================================================================
GraphicsManager::Texture GraphicsManager::GfxGenTextStatic( uint32_t a_HashName, const char * a_String, const Font & a_Font, float a_FontSize )
{
	CString text = a_String;
	int32_t nlInd = text.Find( "\n" );
	text = text.SubString( 0, nlInd == -1 ? INT_MAX : nlInd );

	Font font = a_Font ? a_Font : m_DefaultFont;
	uint32_t prevFontPt = font->Value2;
	SetFontSize( font, static_cast< uint32_t >( Floor( a_FontSize ) ) );
	Texture textTexture = ResolveTextTexture( text, font, false, &a_HashName );
	SetFontSize( font, prevFontPt );

	return textTexture;
}

//=====================================================================================
GraphicsManager::Texture GraphicsManager::GfxGenTextStatic( const char * a_String, const Font & a_Font, float a_FontSize )
{
	CString text = a_String;
	int32_t nlInd = text.Find( "\n" );
	text = text.SubString( 0, nlInd == -1 ? INT_MAX : nlInd );

	Font font = a_Font ? a_Font : m_DefaultFont;
	uint32_t prevFontPt = font->Value2;
	SetFontSize( font, static_cast< uint32_t >( Floor( a_FontSize ) ) );
	Texture textTexture = ResolveTextTexture( text, font, false );
	SetFontSize( font, prevFontPt );

	return textTexture;
}

//=====================================================================================
Vector2 GraphicsManager::QueryTextSize( const char * a_String, const Font & a_Font, float a_FontSize )
{
	PROFILE;

	CString text = a_String;
	int32_t nlInd = text.Find( "\n" );
	text = text.SubString( 0, nlInd == -1 ? INT_MAX : nlInd );
	Font font = a_Font ? a_Font : m_DefaultFont;
	
	int w, h;
	uint32_t prevFontPt = font->Value2;
	uint32_t fontPt = static_cast< uint32_t >( Floor( a_FontSize ) );
	
	if ( prevFontPt != fontPt )
	{
		SetFontSize( font, fontPt );
	}
	
	TTF_SizeText( reinterpret_cast< TTF_Font* >( font->Value1 ), a_String, &w, &h );

	if ( prevFontPt != fontPt )
	{
		SetFontSize( font, prevFontPt );
	}
	
	return Vector2( static_cast< float >( w ), static_cast< float >( h ) );
}

//=====================================================================================
Vector2 GraphicsManager::QueryTextSize( const char * a_String, const BitmapFont & a_BitmapFont, uint32_t a_StringLength )
{
	PROFILE2( "BitmapFont" );

	CString text = a_String;
	int32_t nlInd = text.Find( "\n" );
	text = text.SubString( 0, nlInd == -1 ? INT_MAX : nlInd );

	uint32_t length = Min( a_StringLength, text.Length() );

	float accum = 0.0F;
	for ( uint32_t index = 0; index < length; ++index )
	{
		char currentChar = text[ index ];
		const Tuple3< uint16_t, Vector2, Vector2 > & glyph = a_BitmapFont->Value2[ static_cast< uint32_t >( currentChar ) ];
		accum += glyph.Value0;
	}

	return Vector2( accum, a_BitmapFont->Value1 );
}

//=====================================================================================
Vector2 GraphicsManager::GfxDrawText( const char * a_String, const Font & a_Font, float a_FontSize, TextAlignment a_Horizontal, TextAlignment a_Vertical, TextFlags a_Flags )
{
	PROFILE;

	bool supportsSecondaryColour = a_Flags & TF_GRADIENT;
	Shader shader = ( a_Flags & TF_GRADIENT ) 
						? m_TextShaderGradiented 
						: m_TextShader;

	CString text = a_String;

	int32_t nlInd = text.Find( "\n" );
	text = text.SubString( 0, nlInd == -1 ? INT_MAX : nlInd );

	if ( text.Length() == 0 )
	{
		return Vector2::ZERO;
	}

	Font font = a_Font ? a_Font : m_DefaultFont;
	uint32_t prevFontPt = font->Value2;
	SetFontSize( font, static_cast< uint32_t >( Floor( a_FontSize ) ) );
	Texture textTexture = ResolveTextTexture( text, font, true );
	SetFontSize( font, prevFontPt );

	int32_t sampler = SetTexture( textTexture, TS_NEXT_AVAILABLE | TS_FORCE_LAST_IF_FULL );
	SetShaderGlobalSampler( shader, GetShaderGlobalLocation( shader, "uTex0" ), sampler );
	SetShaderGlobalColour( shader, GetShaderGlobalLocation( shader, "uColour" ), m_Colour[ ( uint32_t )COL_PRIMARY ] );
	
	if ( supportsSecondaryColour )
	{
		SetShaderGlobalColour( shader, GetShaderGlobalLocation( shader, "uColour2" ), m_Colour[ ( uint32_t )COL_SECONDARY ] );
	}

	Vector2 size = QueryTextureSize( textTexture );

	TfPush();
	TfScale( size );
	TfTranslate( Vector2( static_cast< float >( a_Horizontal ), static_cast< float >( a_Vertical ) ) * -0.5F );
	SetShaderGlobalFloat3x3( shader, GetShaderGlobalLocation( shader, "uTransform" ), TfGetTop() );
	TfPop();

	SetShader( shader );
	GLDraw::Instance().DrawMesh( m_01Quad );

	return size;
}

//=====================================================================================
Vector2 GraphicsManager::GfxDrawText( const char * a_String, const BitmapFont & a_BitmapFont, TextAlignment a_Horizontal, TextAlignment a_Vertical, TextFlags a_Flags )
{
	return GfxDrawText( a_String, a_BitmapFont, UINT_MAX, false, a_Horizontal, a_Vertical, a_Flags );
}

//=====================================================================================
Vector2 GraphicsManager::GfxDrawText( const char * a_String, const BitmapFont & a_BitmapFont, uint32_t a_StringLength, bool a_StringLengthAffectsTextAlignment, TextAlignment a_Horizontal, TextAlignment a_Vertical, TextFlags a_Flags )
{
	PROFILE2( "BitmapFont" );

	if ( !a_BitmapFont )
	{
		return Vector2::ZERO;
	}

	bool supportsSecondaryColour = a_Flags & TF_GRADIENT;
	Shader shader = ( a_Flags & TF_GRADIENT ) 
						? m_TextShaderGlyphsGradiented 
						: m_TextShaderGlyphs;

	CString text = a_String;
	int32_t nlInd = text.Find( "\n" );
	text = text.SubString( 0, nlInd == -1 ? INT_MAX : nlInd );

	uint32_t length = Min( a_StringLength, text.Length() );

	int32_t sampler = SetTexture( a_BitmapFont->Value0, TS_NEXT_AVAILABLE | TS_FORCE_LAST_IF_FULL );
	SetShaderGlobalSampler( shader, GetShaderGlobalLocation( shader, "uTex0" ), sampler );
	SetShaderGlobalColour( shader, GetShaderGlobalLocation( shader, "uColour" ), m_Colour[ ( uint32_t )COL_PRIMARY ] );
	
	if ( supportsSecondaryColour )
	{
		SetShaderGlobalColour( shader, GetShaderGlobalLocation( shader, "uColour2" ), m_Colour[ ( uint32_t )COL_SECONDARY ] );
	}

	float accum = 0.0F;
	for ( uint32_t index = 0; index < ( a_StringLengthAffectsTextAlignment ? length : text.Length() ); ++index )
	{
		char currentChar = text[ index ];
		const Tuple3< uint16_t, Vector2, Vector2 > & glyph = a_BitmapFont->Value2[ static_cast< uint32_t >( currentChar ) ];
		accum += glyph.Value0;
	}
	Vector2 size = Vector2( accum, a_BitmapFont->Value1 );

	TfPush();
	TfTranslate( size * Vector2( static_cast< float >( a_Horizontal ), static_cast< float >( a_Vertical ) ) * -0.5F );

	const float * left = TfGetTop();
	static float result[ 9 ];
	static float right[ 9 ] = 
	{
		1.0F, 0.0F, 0.0F,
		0.0F, 1.0F, 0.0F,
		0.0F, 0.0F, 1.0F,
	};

	int32_t sLocTransform = GetShaderGlobalLocation( shader, "uTransform" );
	int32_t sLocGlyphUV = GetShaderGlobalLocation( shader, "uGlyphUV" );

	accum = 0.0F;
	SetShader( shader );

	char * it = &text[ 0 ];
	const char * end = &text[ length ];

	while ( it != end )
	{
		//			  raw_ofst  uv_ofst  uv_glsz 
		const Tuple3< uint16_t, Vector2, Vector2 > & glyph = a_BitmapFont->Value2[ static_cast< uint32_t >( *it ) ];

		*( right + 0 ) = glyph.Value0;
		*( right + 4 ) = a_BitmapFont->Value1;
		*( right + 6 ) = accum;
		TfMul( left, right, result );

		SetShaderGlobalFloat3x3( shader, sLocTransform, result );
		SetShaderGlobalFloat2( shader, sLocGlyphUV, Vector2( glyph.Value1.x, glyph.Value2.x ) );
		GLDraw::Instance().DrawMesh( m_01Quad );
		
		accum += glyph.Value0;
		++it;
	}
	
	TfPop();

	return size;
}

//=====================================================================================
Vector2 GraphicsManager::GfxDrawTextStatic( const Texture & a_TextTexture, TextAlignment a_Horizontal, TextAlignment a_Vertical, TextFlags a_Flags )
{
	if ( !a_TextTexture || ( a_TextTexture->Value2 ^ TEXFLG_TEXT ) )
	{
		PRINT( "%s: Failed to draw static text texture - %s", 
				__FUNCTION__, 
				( a_TextTexture 
					? "Texture is null" 
					: "Texture does not support text-rendering" ) );
		return Vector2::ZERO;
	}

	int32_t sampler = SetTexture( a_TextTexture, TS_NEXT_AVAILABLE | TS_FORCE_LAST_IF_FULL );
	SetShaderGlobalSampler( m_TextShader, GetShaderGlobalLocation( m_TextShader, "uTex0" ), sampler );
	SetShaderGlobalColour( m_TextShader, GetShaderGlobalLocation( m_TextShader, "uColour" ), m_Colour[ ( uint32_t )COL_PRIMARY ] );

	Vector2 size = QueryTextureSize( a_TextTexture );

	TfPush();
	TfScale( size );
	TfTranslate( Vector2( static_cast< float >( a_Horizontal ), static_cast< float >( a_Vertical ) ) * -0.5F );
	SetShaderGlobalFloat3x3( m_TextShader, GetShaderGlobalLocation( m_TextShader, "uTransform" ), TfGetTop() );
	TfPop();

	SetShader( m_TextShader );
	GLDraw::Instance().DrawMesh( m_01Quad );

	return size;
}

//=====================================================================================
void GraphicsManager::GfxDrawQuad( const Vector2 & a_Position, const Vector2 & a_Size, float a_Outline )
{
	PROFILE;

	TfPush();
	TfTranslate( a_Position );
	TfScale( a_Size );
	SetShaderGlobalFloat3x3( m_SimpleColourShader, GetShaderGlobalLocation( m_SimpleColourShader, "uTransform" ), TfGetTop() );
	TfPop();

	SetShader( m_SimpleColourShader );

	SetShaderGlobalColour( m_SimpleColourShader, GetShaderGlobalLocation( m_SimpleColourShader, "uColour" ), m_Colour[ ( uint32_t )COL_PRIMARY ] );
	GLDraw::Instance().DrawMesh( m_01Quad );

	if ( a_Outline > 0.0F )
	{
		glLineWidth( a_Outline );
		SetShaderGlobalColour( m_SimpleColourShader, GetShaderGlobalLocation( m_SimpleColourShader, "uColour" ), m_Colour[ ( uint32_t )COL_SECONDARY ] );
		GLDraw::Instance().DrawMesh( m_01LineQuad );
		glLineWidth( 1.0F );
	}
}

//=====================================================================================
void GraphicsManager::GfxDrawQuadTextured( const Vector2 & a_Position, const Vector2 & a_Size, int32_t a_SamplerUnit, bool a_UseSetUV, float a_AlphaPowFactor, const InitialiserList< Vector2 > & a_UVCoordinates )
{
	TfPush();
	TfTranslate( a_Position );
	TfScale( a_Size );
	SetShaderGlobalFloat3x3( m_TextureColourExplicitUVShader, GetShaderGlobalLocation( m_TextureColourExplicitUVShader, "uTransform" ), TfGetTop() );
	TfPop();

	SetShaderGlobalFloat1( m_TextureColourExplicitUVShader, GetShaderGlobalLocation( m_TextureColourExplicitUVShader, "uAlphaPow" ), a_AlphaPowFactor );
	SetShaderGlobalColour( m_TextureColourExplicitUVShader, GetShaderGlobalLocation( m_TextureColourExplicitUVShader, "uColour" ), m_Colour[ ( uint32_t )COL_PRIMARY ] );
	SetShaderGlobalSampler( m_TextureColourExplicitUVShader, GetShaderGlobalLocation( m_TextureColourExplicitUVShader, "uTexture" ), a_SamplerUnit );

	auto it = NBegin( a_UVCoordinates );
	const auto end = NEnd( a_UVCoordinates );
	CString loc = "uTextureCoords[0]";
	for ( uint32_t p = 0; p < 4, it != end; ++p, ++it )
	{
		Vector2 uv = *it;
		loc[ 15 ] = '0' + p;

		if ( a_UseSetUV )
		{
			uv = uv * m_UVTiling + m_UVOffset;
		}

		SetShaderGlobalFloat2( m_TextureColourExplicitUVShader, GetShaderGlobalLocation( m_TextureColourExplicitUVShader, loc.Get() ), uv );
	}
	
	SetShader( m_TextureColourExplicitUVShader );
	GLDraw::Instance().DrawMesh( m_01Quad );
}

//=====================================================================================
void GraphicsManager::GfxSetUV( const Vector2 & a_Offset, const Vector2 & a_Tiling )
{
	m_UVOffset = a_Offset;
	m_UVTiling = a_Tiling;
}

//=====================================================================================
GraphicsManager::Texture GraphicsManager::ResolveTextTexture( const CString & a_Text, const Font & a_Font, bool a_UseCache, const uint32_t * a_HashNameP )
{
	if ( !a_UseCache && m_TextCache.Count() == m_TextCache.Capacity() )
	{
		return m_BadStringTexture;
	}

	Tuple2< uint64_t, uint32_t > identifier;

	identifier.Value0 = WSID( a_Text.Get() );

	const Dictionary< uint32_t, PFont >::Pair * ppair = m_Fonts.GetEntries().Find< const Font & >( []( const Font & a_UserData, const Dictionary< uint32_t, PFont >::Pair & a_PtrInList )
	{ 
		return a_UserData->Value1 == a_PtrInList.Value->Value1;
	}, a_Font );

	if ( ppair )
	{
		
		identifier.Value0 |= ( ppair->Key ) << 31;
		identifier.Value1 = ppair->Value->Value2;
	}

	if ( a_UseCache )
	{
		TextCacheApplyGC();
	}

	CachedTextTexture cttd; // filled out if we currently don't have a text-texture cached.
	CachedTextTexture * cttdPtrQuery = a_UseCache ? m_TextCache[ identifier ] : nullptr;

	// doesn't exist, we need to create it then return it
	if ( !cttdPtrQuery )
	{
		SDL_Surface* textSurface = TTF_RenderText_Blended( reinterpret_cast< TTF_Font* >( a_Font->Value1 ), 
														   a_Text.Get(), SDL_Color{ 255, 255, 255, 255 } );
		
		Texture texture = LoadTexture( a_HashNameP 
											? ( *a_HashNameP )
											: WSID( CString().Format( "TextTexture_%llu%u", identifier.Value0, identifier.Value1 ).Get() ), 
									  ( uint32_t )textSurface->w, ( uint32_t )textSurface->h, textSurface->pixels, 
									  PixelFormat::PF_RGBA8, TextureFilterMode::LINEAR, TextureWrapMode::CLAMP );
		
		SDL_FreeSurface( textSurface );
		
		const Dictionary< uint32_t, PTexture >::Pair * ppair = m_Textures.GetEntries().Find< const Texture & >( []( const Texture & a_UserData, const Dictionary< uint32_t, PTexture >::Pair & a_PtrInList )
		{ 
			return a_PtrInList.Value.Ptr() == a_UserData.Ptr();
		}, texture );

		ASSERT( ppair, "Error setting flag for text texture" );

		ppair->Value->Value2 = TEXFLG_TEXT;

		if ( !a_UseCache )
		{
			return texture;
		}

		cttd.m_Texture = texture;
		m_TextCache.Put( identifier, cttd );
		cttdPtrQuery = m_TextCache[ identifier ];
	}
	
	if ( !cttdPtrQuery || !cttdPtrQuery->m_Texture )
	{
		return m_BadStringTexture;
	}

	//reset the idle frames
	cttdPtrQuery->m_IdleFrames = 0;
	return cttdPtrQuery->m_Texture;
}

//=====================================================================================
void GraphicsManager::TextCacheApplyGC()
{
	const static uint32_t TEXT_CACHE_IDLEFRAME_TIMEOUT = 1;

	auto * keys = m_TextCache.GetKeys();
	auto * values = m_TextCache.GetValues();
	
	if ( m_TextCache.Count() > 0 )
	for ( int32_t k = m_TextCache.Count() - 1; k >= 0; --k )
	{
		const auto & key = keys[ k ];
		auto & val = values[ k ];

		if ( val.m_IdleFrames >= TEXT_CACHE_IDLEFRAME_TIMEOUT )
		{
			//if ( val.m_Texture )
			{
				auto pair = m_Textures.GetEntries().Find< const Texture & >( []( const Texture & a_It, const Dictionary< uint32_t, PTexture >::Pair & a_Pair ) {
						return a_Pair.Value.Ptr() == a_It.Ptr();
				}, val.m_Texture );

				if ( pair )
				{
					UnloadTexture( pair->Key );
				}
			}

			m_TextCache.Remove( key );
		}
	}
}

//====================================================================================
void GraphicsManager::SetState( RenderState a_RenderState, bool a_Value )
{
	switch ( a_RenderState )
	{
	case RS_TRANSPARENCY:
		if ( a_Value )
		{
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, m_StateInfo.Additive ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA );
		}
		else
		{
			glDisable( GL_BLEND );
		}
		break;
	case RS_STENCILING:
		if ( a_Value )
		{
			glEnable( GL_STENCIL_TEST );
		}
		else
		{
			glDisable( GL_STENCIL_TEST );
		}
		break;
	case RS_STENCILING_WRITE:
		if ( a_Value )
		{
			glStencilFunc( GL_ALWAYS, 1, 0xFF );
			glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
			glStencilMask( 0xFF );
			glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
			glDepthMask( GL_FALSE );
		}
		else
		{
			glStencilFunc( GL_EQUAL, 0, 0xFF );
			glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
			glDepthMask( GL_FALSE );
		}
		break;
	case RS_ADDITIVE_BLEND:
		m_StateInfo.Additive = a_Value;
		break;
	default:
		ASSERT_WARN( false, "GraphicsManager::SetState(..): Invalid dataset [bool] for renderstate (%u)", a_RenderState );
	}
}

//====================================================================================
void GraphicsManager::SetState( RenderState a_RenderState, int32_t a_Value )
{
	switch ( a_RenderState )
	{
	default:
		ASSERT_WARN( false, "GraphicsManager::SetState(..): Invalid dataset [int32_t] for renderstate (%u)", a_RenderState );
	}
}

//====================================================================================
void GraphicsManager::SetState( RenderState a_RenderState, float a_Value )
{
	switch ( a_RenderState )
	{
	default:
		ASSERT_WARN( false, "GraphicsManager::SetState(..): Invalid dataset [float] for renderstate (%u)", a_RenderState );
	}
}

//====================================================================================
void GraphicsManager::SetColour( const Colour & a_Colour, ColourUsage a_ColourUsage )
{ 
	m_Colour[ ( uint32_t )a_ColourUsage ] = a_Colour; 

	if ( a_ColourUsage == ColourUsage::COL_BACKGROUND )
	{
		glClearColor( a_Colour.r, a_Colour.g, a_Colour.b, a_Colour.a );
	}
}

//====================================================================================
const Colour & GraphicsManager::GetColour( ColourUsage a_ColourUsage ) const
{ 
	return m_Colour[ ( uint32_t )a_ColourUsage ]; 
}
	

#undef WM
#undef R