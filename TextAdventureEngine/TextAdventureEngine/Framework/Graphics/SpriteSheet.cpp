#include "SpriteSheet.hpp"
#include <Framework/Math/Math.hpp>
#include <Framework/Graphics/Graphics.hpp>

SpriteSheet::SpriteSheet( int32_t a_Texture, const Vector2 & a_SpritesPerAxis )
{
	m_Texture = a_Texture;
	m_SpritesPerAxis.x = Round( a_SpritesPerAxis.x );
	m_SpritesPerAxis.y = Round( a_SpritesPerAxis.y );
}

void SpriteSheet::ApplyUV( uint32_t a_SpriteIndex ) const
{
	uint32_t w = static_cast< uint32_t >( m_SpritesPerAxis.x );
	uint32_t h = static_cast< uint32_t >( m_SpritesPerAxis.y );

	uint32_t u = a_SpriteIndex % w;
	uint32_t v = a_SpriteIndex / w;

	float x = ( float )u / w;
	float y = ( float )v / h;
	
	Graphics::SetSpriteScroll( { x, y } );
	Graphics::SetSpriteTiling( Vector2( 1.0F ) / m_SpritesPerAxis );
}

void SpriteSheet::ApplyUV( uint32_t a_SpriteIndexX, uint32_t a_SpriteIndexY ) const
{
	Graphics::SetSpriteScroll( Vector2( static_cast< float >( a_SpriteIndexX ), static_cast< float >( a_SpriteIndexY ) ) / m_SpritesPerAxis );
	Graphics::SetSpriteTiling( Vector2( 1.0F ) / m_SpritesPerAxis );
}

void SpriteSheet::DrawSprite( uint32_t a_SpriteIndex, const Vector2 & a_Position, const Vector2 & a_Size ) const
{
	ApplyUV( a_SpriteIndex );
	Graphics::DrawSprite( m_Texture, a_Position + a_Size * 0.5F, a_Size );
}

void SpriteSheet::DrawSprite( uint32_t a_SpriteIndexX, uint32_t a_SpriteIndexY, const Vector2 & a_Position, const Vector2 & a_Size ) const
{
	ApplyUV( a_SpriteIndexX, a_SpriteIndexY );
	Graphics::DrawSprite( m_Texture, a_Position, a_Size );
}

const Vector2 & SpriteSheet::SpritesPerAxis() const
{
	return m_SpritesPerAxis;
}