#include "NineSpriteSheet.hpp"

//=====================================================================================
NineSpriteSheet::NineSpriteSheet( int32_t a_Texture, float a_EdgeSize )
	: SpriteSheet( a_Texture, Vector2( 3 ) )
	, m_EdgeSize( a_EdgeSize )
{
}

//=====================================================================================
void NineSpriteSheet::DrawNineSprite( const Vector2 & p, const Vector2 & s_ ) const
{
	Vector2 e( m_EdgeSize );

	Vector2 s = s_;
	s.x = Max( e.x * 2.0F, s.x );
	s.y = Max( e.y * 2.0F, s.y );

#define v2 Vector2

	DrawSprite( 8, v2( p.x + s.x - e.x, p.y + s.y - e.y ), e );
	DrawSprite( 7, v2( p.x + e.x, p.y + s.y - e.y ), v2( s.x - e.x * 2.0F, e.y ) );
	DrawSprite( 6, v2( p.x, p.y + s.y - e.y ), e );
	DrawSprite( 4, p + e, s - e * 2.0F );
	DrawSprite( 0, p, e );
	DrawSprite( 1, v2( p.x + e.x, p.y ), v2( s.x - e.x * 2.0F, e.y ) );
	DrawSprite( 2, v2( p.x + s.x - e.x, p.y ), e );

	DrawSprite( 3, v2( p.x, p.y + e.y ), v2( e.x, s.y - e.y * 2.0F ) );
	DrawSprite( 5, v2( p.x + s.x - e.x, p.y + e.y ), v2( e.x, s.y - e.y * 2.0F ) );

#undef v2
}