#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

//=====================================================================================
struct Colour;
struct Vector2;

//=====================================================================================
class Graphics final
{
public:

	ENUM( TextAlign, uint8_t )
		TEXT_ALIGN_CENTER,
		TEXT_ALIGN_RIGHT,
		TEXT_ALIGN_LEFT
	END_ENUM;

	static void SetBackColor( const Colour & a_Colour );
	static void SetForeColor( const Colour & a_Colour );
	static void SetAdditiveBlend( bool a_Flag );
	static void Push();
	static void Pop();
	static void Translate( const Vector2 & a_Translation );
	static void Rotate( float a_Degrees );
	static void Scale( const Vector2 & a_Scale );
	static int32_t LoadTexture( const char * a_Path );
	static void DrawTriangleFill( const Vector2 & a_Offset, const Vector2 a_Size );
	static void DrawTriangleOutline( const Vector2 & a_Offset, const Vector2 a_Size );
	static void DrawRectangleFill( const Vector2 & a_Offset, const Vector2 a_Size );
	static void DrawRectangleOutline( const Vector2 & a_Offset, const Vector2 a_Size );
	static void DrawCircleFill( const Vector2 & a_Offset, float a_Radius, int32_t a_NumVertices = 16 );
	static void DrawCircleOutline( const Vector2 & a_Offset, float a_Radius, int32_t a_NumVertices = 16 );
	static void DrawSemiCircleFill( const Vector2 & a_Offset, float a_Radius, float a_Degrees = 180.0F, int32_t a_NumVertices = 16 );
	static void DrawSemiCircleOutline( const Vector2 & a_Offset, float a_Radius, float a_Degrees = 180.0F, int32_t a_NumVertices = 16 );
	static void DrawPoint( const Vector2 & a_Offset );
	static void DrawLine( const Vector2 & a_P1, const Vector2 & a_P2 );
	static void SetSpriteTiling( const Vector2 & a_SizeNormalised );
	static void SetSpriteScroll( const Vector2 & a_OffsetNormalised );
	static void DrawSprite( int32_t a_Texture, const Vector2 & a_Offset, const Vector2 a_Size );
	static void SetTextAlign( TextAlign a_TextAlign );
	static float GetTextWidth( const char * a_String );
	static float GetTextHeight( const char * a_String );
	static int32_t LoadFont( const char * a_Path );
	static void SetFont( int32_t a_Font, float a_FontSize = 32.0F );
	static void SetFontSize( float a_FontSize );
	static void DrawText( const Vector2 & a_Offset, const char * a_String );

};

#endif//GRAPHICS_H