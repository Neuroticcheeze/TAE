#pragma once
#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/StaticArray.hpp>
#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Containers/StaticDictionary.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Graphics/SpriteSheet.hpp>
#include <Framework/Utils/Memory.hpp>
#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Containers/Stack.hpp>
#include <Framework/Graphics/GL/GLEnums.hpp>
#include <Framework/Graphics/VertexBuffer.hpp>

//=====================================================================================
struct Vector2;
struct Colour;
class UShader;
class GLMeshIndexed;
class Bezier;
class VertexBuffer;

//=====================================================================================
class GraphicsManager final : public TickableManager< GraphicsManager >
{
public:

	ENUM( TextureResidency, uint8_t )
		TEXRES_HOST_MEMORY	= 1,
		TEXRES_VIDEO_MEMORY	= 2,
		TEXRES_BOTH		= TEXRES_HOST_MEMORY | TEXRES_VIDEO_MEMORY,
	END_ENUM_F( TextureResidency, uint8_t );

	ENUM( PixelFormat, uint8_t )
		PF_RGB8,
		PF_RGBA8,
		PF_RGBA32F,
	END_ENUM;

	typedef WeakPointer< Tuple3< void *, void *, uint8_t /*user-data*/ > > Texture;
	typedef WeakPointer< void * > RenderTexture;
	typedef WeakPointer< Tuple3< CString, void *, uint32_t > > Font;
	typedef WeakPointer< Tuple3< Texture, uint16_t, StaticArray< Tuple3< uint16_t, Vector2, Vector2 >, 256 > > > BitmapFont;
	typedef WeakPointer< SpriteSheet > TextureAtlas;
	typedef WeakPointer< UShader > Shader;

	void Init();
	void InitPost();
	void Tick( float a_Dt );
	void Finalise();

	Shader LoadShader( uint32_t a_HashName, const char * a_Path, uint32_t a_VertexShader = 0, uint32_t a_FragmentShader = 0 );
	Texture LoadTexture( uint32_t a_HashName, const char * a_Path, TextureResidency a_TextureResidency = TEXRES_VIDEO_MEMORY, TextureFilterMode a_Filter = TextureFilterMode::LINEAR, TextureWrapMode a_Wrap = TextureWrapMode::REPEAT, bool a_EnableMipmapping = false, TextureType a_TextureType = TextureType::TT_2D );
	Texture LoadTexture( uint32_t a_HashName, uint32_t a_Width, uint32_t a_Height, void * a_Pixels, PixelFormat a_PixelFormat, TextureFilterMode a_Filter = TextureFilterMode::LINEAR, TextureWrapMode a_Wrap = TextureWrapMode::REPEAT, bool a_EnableMipmapping = false, TextureType a_TextureType = TextureType::TT_2D );
	TextureAtlas LoadTextureAsAtlas( uint32_t a_HashName, const char * a_Path, const Vector2 & a_SpritesPerAxis, TextureFilterMode a_Filter = TextureFilterMode::LINEAR );
	TextureAtlas LoadAtlas( uint32_t a_HashName, const SpriteSheet & a_SpriteSheet );
	Font LoadFont( uint32_t a_HashName, const char * a_Path, uint32_t a_FontSize );
	BitmapFont LoadBitmapFont( uint32_t a_HashName, const char * a_CharacterSet, const Font & a_Font, uint32_t a_FontSize );
	RenderTexture AllocateRenderTexture( uint32_t a_HashName, const Vector2 & a_Size, uint32_t a_NumColourTextures, PixelFormat a_PixelFormat = PF_RGBA32F, TextureFilterMode a_Filter = TextureFilterMode::LINEAR, TextureWrapMode a_Wrap = TextureWrapMode::REPEAT, bool a_EnableMipmapping = false );
	bool DeallocateRenderTexture( uint32_t a_HashName );
	bool DeallocateRenderTexture( RenderTexture & a_RenderTexture );
	void SetFontSize( uint32_t a_HashName, uint32_t a_FontSize );
	bool UnloadFont( uint32_t a_HashName );
	bool UnloadBitmapFont( uint32_t a_HashName );
	bool UnloadTexture( uint32_t a_HashName );
	bool UnloadAtlas( uint32_t a_HashName );
	bool UnloadShader( uint32_t a_HashName );
	RenderTexture GetRenderTexture( uint32_t a_HashName ) const;
	Texture GetTexture( uint32_t a_HashName ) const;
	TextureAtlas GetAtlas( uint32_t a_HashName ) const;
	Font GetFont( uint32_t a_HashName ) const;
	BitmapFont GetBitmapFont( uint32_t a_HashName ) const;
	Shader GetShader( uint32_t a_HashName ) const;

	void SetFontSize( Font & a_Font, uint32_t a_FontSize );
	bool UnloadFont( Font & a_Font );
	bool UnloadBitmapFont( BitmapFont & a_BitmapFont );
	bool UnloadTexture( Texture & a_Texture );
	bool UnloadAtlas( TextureAtlas & a_TextureAtlas );
	bool UnloadShader( Shader & a_Shader );

	void SetShader( Shader & a_Shader );
	int32_t GetShaderGlobalLocation( Shader & a_Shader, const char * a_PropertyName );
	void SetShaderGlobalInt32( Shader & a_Shader, int32_t a_Loc, int32_t a_Value );
	void SetShaderGlobalFloat1( Shader & a_Shader, int32_t a_Loc, float a_Value );
	void SetShaderGlobalFloat2( Shader & a_Shader, int32_t a_Loc, const Vector2 & a_Value );
	void SetShaderGlobalFloat3( Shader & a_Shader, int32_t a_Loc, const float * a_ValueFloat3 );
	void SetShaderGlobalFloat3( Shader & a_Shader, int32_t a_Loc, float a_X, float a_Y, float a_Z ) { SetShaderGlobalFloat3( a_Shader, a_Loc, NBegin( { a_X, a_Y, a_Z } ) ); }
	void SetShaderGlobalFloat3x3( Shader & a_Shader, int32_t a_Loc, const float * a_ValueFloat3x3, bool a_Transpose = false );
	void SetShaderGlobalColour( Shader & a_Shader, int32_t a_Loc, const Colour & a_Value );
	void SetShaderGlobalFloat1Array( Shader & a_Shader, int32_t a_Loc, const Array< float > & a_Values );
	void SetShaderGlobalFloat2Array( Shader & a_Shader, int32_t a_Loc, const Array< Vector2 > & a_Values );
	void SetShaderGlobalColourArray( Shader & a_Shader, int32_t a_Loc, const Array< Colour > & a_Values );
	void SetShaderGlobalSampler( Shader & a_Shader, int32_t a_Loc, uint32_t a_TextureUnit );

	static void * GetGLShaderPtr( const Shader & a_Shader );

	Vector2 QueryTextureSize( const Texture & a_Texture ) const;
	Vector2 QueryRenderTextureSize( const RenderTexture & a_RenderTexture ) const;

	GraphicsManager::RenderTexture ResizeRenderTexture( uint32_t a_HashName, const Vector2 & a_Dimensions );
	void ResizeRenderTexture( RenderTexture & a_RenderTexture, const Vector2 & a_Dimensions );

	void ActivateRenderTexture( RenderTexture & a_RenderTexture, const InitialiserList< uint32_t > & a_DrawBuffers, bool a_Clear = true );
	void ActivateRTDrawBuffers( const InitialiserList< uint32_t > & a_DrawBuffers, bool a_Clear = true );
	void DeactivateRenderTexture();
	const RenderTexture & GetActiveRenderTexture() const;

	ENUM( TextureSetFlags, uint32_t )
		TS_NEXT_AVAILABLE = 1024,
		TS_FORCE_LAST_IF_FULL = 2048,
		TS_UNSET = 4096,
	END_ENUM_F( TextureSetFlags, uint32_t );

	int32_t SetTexture( const Texture & a_Texture, uint32_t a_UnitOrFlags = TS_NEXT_AVAILABLE | TS_FORCE_LAST_IF_FULL );
	int32_t SetTexture( const RenderTexture & a_RenderTexture, uint32_t a_ColourIndex = 0, uint32_t a_UnitOrFlags = TS_NEXT_AVAILABLE | TS_FORCE_LAST_IF_FULL, bool a_RebuildMips = true );

	void TfPush();
	void TfPushIdentity( bool a_PreserveViewport = true );
	void TfTranslate( const Vector2 & a_Translation, bool a_FlipX = false, bool a_FlipY = false );
	void TfRotate( float a_Degrees );
	void TfScale( const Vector2 & a_Scale );
	void TfShear( const Vector2 & a_Shear );
	void TfViewport( const Vector2 & a_Position, const Vector2 & a_Size );
	void TfPop();
	const float * TfGetTop( float * a_MultMatrix3x3 = nullptr ) const;
	void TfSetTop( float * a_SetMatrix3x3 ) const;
	void TfMul( const float * a_Left, const float * a_Right, float * o_Result ) const;
	void TfMakeTRS( float * o_Result, const Vector2 & a_Translation, float a_Rotation = 0.0F, const Vector2 & a_Scale = Vector2::ONE ) const;


	ENUM( TextAlignment, uint8_t )
		TA_LEFT = 0,
		TA_BOTTOM = 0,
		TA_CENTER = 1,
		TA_RIGHT = 2,
		TA_TOP = 2
	END_ENUM;

	ENUM( TextFlags, uint8_t )
		TF_DEFAULT = 0,
		TF_SHADOW = 1,
		TF_GRADIENT = 2,
	END_ENUM_F( TextFlags, uint8_t )

		
	ENUM( ColourUsage, uint8_t )
		COL_PRIMARY,
		COL_SECONDARY,
		COL_TERTIARY,
		COL_QUATERNARY,
		COL_BACKGROUND,
	END_ENUM_C( ColourUsage, uint8_t );

	ENUM( RenderState, uint8_t )
		RS_TRANSPARENCY,
		RS_STENCILING,
		RS_STENCILING_READ,
		RS_STENCILING_WRITE,
		RS_ADDITIVE_BLEND,
		RS_SCISSOR,
	END_ENUM;

	void SetState( RenderState a_RenderState, bool a_Value );
	void SetState( RenderState a_RenderState, int32_t a_Value );
	void SetState( RenderState a_RenderState, float a_Value );
	void SetState( RenderState a_RenderState, const Tuple2< Vector2, Vector2 > & a_Value );
	void SetColour( const Colour & a_Colour, ColourUsage a_ColourUsage );
	const Colour & GetColour( ColourUsage a_ColourUsage ) const;
	
	Vector2 QueryTextSize( const char * a_String, const Font & a_Font, float a_FontSize );
	Vector2 QueryTextSize( const char * a_String, const BitmapFont & a_BitmapFont, uint32_t a_StringLength = UINT_MAX );
	Texture GfxGenTextStatic( const char * a_String, const Font & a_Font, float a_FontSize );
	Texture GfxGenTextStatic( uint32_t a_HashName, const char * a_String, const Font & a_Font, float a_FontSize );
	Vector2 GfxDrawText( const char * a_String, const Font & a_Font, float a_FontSize, TextAlignment a_Horizontal = TA_LEFT, TextAlignment a_Vertical = TA_BOTTOM, TextFlags a_Flags = TF_DEFAULT );
	Vector2 GfxDrawText( const char * a_String, const BitmapFont & a_BitmapFont, TextAlignment a_Horizontal = TA_LEFT, TextAlignment a_Vertical = TA_BOTTOM, TextFlags a_Flags = TF_DEFAULT );
	Vector2 GfxDrawText( const char * a_String, const BitmapFont & a_BitmapFont, uint32_t a_StringLength = UINT_MAX, bool a_StringLengthAffectsTextAlignment = false, TextAlignment a_Horizontal = TA_LEFT, TextAlignment a_Vertical = TA_BOTTOM, TextFlags a_Flags = TF_DEFAULT );
	Vector2 GfxDrawTextStatic( const Texture & a_TextTexture, TextAlignment a_Horizontal = TA_LEFT, TextAlignment a_Vertical = TA_BOTTOM, TextFlags a_Flags = TF_DEFAULT );
	
	void	GfxSetUV( const Vector2 & a_Offset = Vector2::ZERO, const Vector2 & a_Tiling = Vector2::ONE );
	void	GfxDrawQuad( const Vector2 & a_Position, const Vector2 & a_Size, float a_Outline = 0.0F );
	void	GfxDrawQuadTextured(const Vector2 & a_Position, const Vector2 & a_Size, int32_t a_SamplerUnit, bool a_UseSetUV = true, float a_AlphaPowFactor = 1.0F, const InitialiserList< Vector2 > & a_UVCoordinates = { Vector2::ZERO, Vector2::UP, Vector2::ONE, Vector2::RIGHT } );
	void	GfxDrawLine( const Vector2 & a_PositionA, const Vector2 & a_PositionB, float a_Thickness, bool a_EnableGradient = false );
	void	GfxDrawBezier( const Bezier & a_Bezier, float a_Thickness );
	void	GfxDrawBezier( const Bezier & a_Bezier, float a_Thickness, float a_Dash, float a_DashRatio = 0.5F );
	void	GfxDrawTriangle( const Vector2 & a_PositionA, const Vector2 & a_PositionB, const Vector2 & a_PositionC );

	void GfxDrawVertexBuffer( const VertexBuffer & a_VertexBuffer, int32_t a_SamplerUnit );

private:

	struct
	{
		uint8_t StencilReadWriteValues[ 2 ] = { 1, 1 };
		bool Stencil = false;
		bool Additive = false;
	} m_StateInfo;

	Colour m_Colour[ ColourUsage::ColourUsage_NUM ];

	Vector2 m_UVOffset = Vector2::ZERO;
	Vector2 m_UVTiling = Vector2::ONE;

	struct CachedTextTexture final
	{
		Texture  m_Texture;
		uint32_t m_IdleFrames = 0;
	};

	Texture ResolveTextTexture( const CString & a_Text, const Font & a_Font, bool a_UseCache, const uint32_t * a_HashNameP = nullptr );
	void TextCacheApplyGC();
	StaticDictionary< Tuple2< uint64_t, uint32_t >, CachedTextTexture, 32 > m_TextCache;
	Shader m_TextShader;
	Shader m_TextShaderGradiented;
	Shader m_TextShaderGlyphs;
	Shader m_TextShaderGlyphsGradiented;
	Shader m_SimpleColourShader;
	Shader m_TextureColourExplicitUVShader;
	Shader m_VQuadShader;
	Shader m_BezierVQuadShader;
	Shader m_BezierVQuadDashedShader;
	Shader m_TriShader;
	Shader m_SimpleTextureColourShader;
	Shader m_SimpleColour2Shader;
	Shader m_SimpleTextureShader;
	GLMeshIndexed * m_Triangle = nullptr;
	GLMeshIndexed * m_01Quad = nullptr;
	GLMeshIndexed * m_01LineQuad = nullptr;
	GLMeshIndexed * m_BezierQuadStrip128 = nullptr;

	typedef Pointer< Tuple3< void *, void *, uint8_t /*user-data*/ > > PTexture;
	typedef Pointer< void * > PRenderTexture;
	typedef Pointer< Tuple3< CString, void *, uint32_t > > PFont;
	typedef Pointer< Tuple3< Texture, uint16_t, StaticArray< Tuple3< uint16_t, Vector2, Vector2 >, 256 > > > PBitmapFont;
	typedef Pointer< SpriteSheet > PTextureAtlas;
	typedef Pointer< UShader > PShader;

	Dictionary< uint32_t, PTexture > m_Textures;
	Dictionary< uint32_t, PRenderTexture > m_RenderTextures;
	Dictionary< uint32_t, PTextureAtlas > m_TextureAtlases;
	Dictionary< uint32_t, PFont > m_Fonts;
	Dictionary< uint32_t, PBitmapFont > m_BitmapFonts;
	Dictionary< uint32_t, PShader > m_Shaders;

	struct TextureBinding
	{
		TextureBinding()
		{
			_Texture = nullptr;
			_RenderTextureBind._ColourIndex = 0;
			_RenderTextureBind._RT = nullptr;
		}

		operator bool()
		{
			return _RenderTextureBind._RT || _Texture;
		}

		struct
		{
			uint8_t _ColourIndex = 0;
			RenderTexture _RT = nullptr;
		} _RenderTextureBind;
		Texture _Texture = nullptr;
	};

	StaticArray< TextureBinding, 8 > m_TextureBindings = StaticArray< TextureBinding, 8 >( TextureBinding(), 8 );

	RenderTexture m_ActiveRenderTexture;

	struct Mat3
	{
		Mat3()
			: Data( nullptr )
		{
			Data = new float[ 9 ];
			Data[ 0 ] = 1.0F;
			Data[ 1 ] = 0.0F;
			Data[ 2 ] = 0.0F;
			Data[ 3 ] = 0.0F;
			Data[ 4 ] = 1.0F;
			Data[ 5 ] = 0.0F;
			Data[ 6 ] = 0.0F;
			Data[ 7 ] = 0.0F;
			Data[ 8 ] = 1.0F;
		}

		Mat3( const float * a_InData )
			: Data( nullptr )
		{
			Data = new float[ 9 ];
			for ( uint32_t k = 0; k < 9; ++k )
			{
				Data[ k ] = a_InData[ k ];
			}
		}

		Mat3( const Mat3 & a_Other )
			: Data( nullptr )
		{
			*this = a_Other;
		}

		Mat3 & operator=( const Mat3 & a_Other )
		{
			if ( Data == nullptr )
			{
				Data = new float[ 9 ];
			}

			for ( uint32_t k = 0; k < 9; ++k )
			{
				Data[ k ] = a_Other.Data[ k ];
			}

			return *this;
		}

		~Mat3()
		{
			delete Data;
			Data = nullptr;
		}

		float * Data = nullptr;
	};
	Stack< Tuple3< Mat3, Mat3, Tuple2< Vector2, Vector2 > > > m_MatrixStack;

	Texture m_BadStringTexture;
	Font m_DefaultFont;

	ENUM( TextureSupportFlags, uint8_t )
		TEXFLG_NONE = 0,
		TEXFLG_TEXT = 1 << 0,
	END_ENUM_F( TextureSupportFlags, uint8_t )
};

//=====================================================================================
#define _GetTexture( HASHNAME ) GraphicsManager::Instance().GetTexture( CRC32( HASHNAME ) )
#define _GetSpriteSheet( HASHNAME ) GraphicsManager::Instance().GetAtlas( CRC32( HASHNAME ) )
#define _GetFont( HASHNAME ) GraphicsManager::Instance().GetFont( CRC32( HASHNAME ) )


#endif//GRAPHICSMANAGER_H