#include "SpriteSheet.hpp"
#include <Framework/Math/Math.hpp>
#include <SIGIL-0.9.0/sl.h>

SpriteSheet::SpriteSheet( int32_t a_Texture, const Vector2 & a_SpritesPerAxis )
{
	m_Texture = a_Texture;
	m_SpritesPerAxis.x = Round( a_SpritesPerAxis.x );
	m_SpritesPerAxis.y = Round( a_SpritesPerAxis.y );
}

void SpriteSheet::ApplyUV( uint32_t a_SpriteIndex ) const
{
	uint32_t w = m_SpritesPerAxis.x;
	uint32_t h = m_SpritesPerAxis.y;

	uint32_t u = a_SpriteIndex % w;
	uint32_t v = a_SpriteIndex / w;

	float x = (float)u / w;
	float y = (float)v / h;
	float wid = 1.0F / m_SpritesPerAxis.x;
	float hei = 1.0F / m_SpritesPerAxis.y;

	slSetSpriteScroll( x, y );
	slSetSpriteTiling( wid, hei );
}

void SpriteSheet::ApplyUV( uint32_t a_SpriteIndexX, uint32_t a_SpriteIndexY ) const
{
	uint32_t w = m_SpritesPerAxis.x;
	uint32_t h = m_SpritesPerAxis.y;
	float x = (float)a_SpriteIndexX / w;
	float y = (float)a_SpriteIndexY / h;
	float wid = 1.0F / m_SpritesPerAxis.x;
	float hei = 1.0F / m_SpritesPerAxis.y;

	slSetSpriteScroll( x, y );
	slSetSpriteTiling( wid, hei );
}

void SpriteSheet::DrawSprite( uint32_t a_SpriteIndex, const Vector2 & a_Position, const Vector2 & a_Size ) const
{
	ApplyUV( a_SpriteIndex );
	slSprite( m_Texture, a_Position.x + a_Size.x * 0.5F, a_Position.y + a_Size.y * 0.5F, a_Size.x, a_Size.y );
}

void SpriteSheet::DrawSprite( uint32_t a_SpriteIndexX, uint32_t a_SpriteIndexY, const Vector2 & a_Position, const Vector2 & a_Size ) const
{
	ApplyUV( a_SpriteIndexX, a_SpriteIndexY );
	slSprite( m_Texture, a_Position.x, a_Position.y, a_Size.x, a_Size.y );
}

const Vector2 & SpriteSheet::SpritesPerAxis() const
{
	return m_SpritesPerAxis;
}