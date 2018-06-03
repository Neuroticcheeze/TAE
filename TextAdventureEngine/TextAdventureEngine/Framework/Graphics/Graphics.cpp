#include "Graphics.hpp"
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Math/Colour/Colour.hpp>
#include <SIGIL-0.9.0/sl.h>

//=====================================================================================
void Graphics::Render()
{
	slRender();
}

//=====================================================================================
void Graphics::SetBackColor( const Colour & a_Colour )
{
	slSetBackColor( a_Colour.r, a_Colour.g, a_Colour.b );
}

//=====================================================================================
void Graphics::SetForeColor( const Colour & a_Colour )
{
	slSetForeColor( a_Colour.r, a_Colour.g, a_Colour.b, a_Colour.a );
}

//=====================================================================================
void Graphics::SetAdditiveBlend( bool a_Flag )
{
	slSetAdditiveBlend( a_Flag );
}

//=====================================================================================
void Graphics::Push()
{
	slPush();
}

//=====================================================================================
void Graphics::Pop()
{
	slPop();
}

//=====================================================================================
void Graphics::Translate( const Vector2 & a_Translation )
{
	slTranslate( a_Translation.x, a_Translation.y );
}

//=====================================================================================
void Graphics::Rotate( float a_Degrees )
{
	slRotate( a_Degrees );
}

//=====================================================================================
void Graphics::Scale( const Vector2 & a_Scale )
{
	slScale( a_Scale.x, a_Scale.y );
}

//=====================================================================================
int32_t Graphics::LoadTexture( const char * a_Path )
{
	return slLoadTexture( a_Path );
}

//=====================================================================================
void Graphics::DrawTriangleFill( const Vector2 & a_Offset, const Vector2 a_Size )
{
	slTriangleFill( a_Offset.x, a_Offset.y, a_Size.x, a_Size.y );
}

//=====================================================================================
void Graphics::DrawTriangleOutline( const Vector2 & a_Offset, const Vector2 a_Size )
{
	slTriangleOutline( a_Offset.x, a_Offset.y, a_Size.x, a_Size.y );
}

//=====================================================================================
void Graphics::DrawRectangleFill( const Vector2 & a_Offset, const Vector2 a_Size )
{
	slRectangleFill( a_Offset.x, a_Offset.y, a_Size.x, a_Size.y );
}

//=====================================================================================
void Graphics::DrawRectangleOutline( const Vector2 & a_Offset, const Vector2 a_Size )
{
	slRectangleOutline( a_Offset.x, a_Offset.y, a_Size.x, a_Size.y );
}

//=====================================================================================
void Graphics::DrawCircleFill( const Vector2 & a_Offset, float a_Radius, int32_t a_NumVertices )
{
	slCircleFill( a_Offset.x, a_Offset.y, a_Radius, a_NumVertices );
}

//=====================================================================================
void Graphics::DrawCircleOutline( const Vector2 & a_Offset, float a_Radius, int32_t a_NumVertices )
{
	slCircleOutline( a_Offset.x, a_Offset.y, a_Radius, a_NumVertices );
}

//=====================================================================================
void Graphics::DrawSemiCircleFill( const Vector2 & a_Offset, float a_Radius, float a_Degrees, int32_t a_NumVertices )
{
	slSemiCircleFill( a_Offset.x, a_Offset.y, a_Radius, a_NumVertices, a_Degrees );
}

//=====================================================================================
void Graphics::DrawSemiCircleOutline( const Vector2 & a_Offset, float a_Radius, float a_Degrees, int32_t a_NumVertices )
{
	slSemiCircleOutline( a_Offset.x, a_Offset.y, a_Radius, a_NumVertices, a_Degrees );
}

//=====================================================================================
void Graphics::DrawPoint( const Vector2 & a_Offset )
{
	slPoint( a_Offset.x, a_Offset.y );
}

//=====================================================================================
void Graphics::DrawLine( const Vector2 & a_P1, const Vector2 & a_P2 )
{
	slLine( a_P1.x, a_P1.y, a_P2.x, a_P2.y );
}

//=====================================================================================
void Graphics::SetSpriteTiling( const Vector2 & a_SizeNormalised )
{
	slSetSpriteTiling( a_SizeNormalised.x, a_SizeNormalised.y );
}

//=====================================================================================
void Graphics::SetSpriteScroll( const Vector2 & a_OffsetNormalised )
{
	slSetSpriteScroll( a_OffsetNormalised.x, a_OffsetNormalised.y );
}

//=====================================================================================
void Graphics::DrawSprite( int32_t a_Texture, const Vector2 & a_Offset, const Vector2 a_Size )
{
	slSprite( a_Texture, a_Offset.x, a_Offset.y, a_Size.x, a_Size.y );
}

//=====================================================================================
void Graphics::SetTextAlign( TextAlign a_TextAlign )
{
	slSetTextAlign( static_cast< int >( a_TextAlign ) );
}

//=====================================================================================
float Graphics::GetTextWidth( const char * a_String )
{
	return static_cast< float >( slGetTextWidth( a_String ) );
}

//=====================================================================================
float Graphics::GetTextHeight( const char * a_String )
{
	return static_cast< float >( slGetTextHeight( a_String ) );
}

//=====================================================================================
int32_t Graphics::LoadFont( const char * a_Path )
{
	return slLoadFont( a_Path );
}

//=====================================================================================
void Graphics::SetFont( int32_t a_Font, float a_FontSize )
{
	slSetFont( a_Font, static_cast< int >( Floor( a_FontSize ) ) );
}

//=====================================================================================
void Graphics::SetFontSize( float a_FontSize )
{
	slSetFontSize( static_cast< int >( Floor( a_FontSize ) ) );
}

//=====================================================================================
void Graphics::DrawText( const Vector2 & a_Offset, const char * a_String )
{
	slText( a_Offset.x, a_Offset.y, a_String );
}