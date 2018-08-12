#include "SpriteSheet.hpp"
#include <Framework/Math/Math.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

SpriteSheet::SpriteSheet( Texture a_Texture, const Vector2 & a_SpritesPerAxis )
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
	
	GraphicsManager::Instance().GfxSetUV( { x, y },
										  Vector2( 1.0F ) / m_SpritesPerAxis );
}

void SpriteSheet::ApplyUV( uint32_t a_SpriteIndexX, uint32_t a_SpriteIndexY ) const
{
	GraphicsManager::Instance().GfxSetUV( Vector2( static_cast< float >( a_SpriteIndexX ), static_cast< float >( a_SpriteIndexY ) ) / m_SpritesPerAxis,
										  Vector2( 1.0F ) / m_SpritesPerAxis );
}

void SpriteSheet::DrawSprite( uint32_t a_SpriteIndex, const Vector2 & a_Position, const Vector2 & a_Size ) const
{
	ApplyUV( a_SpriteIndex );
	
	int32_t s = GraphicsManager::Instance().SetTexture( m_Texture );
	GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
	GraphicsManager::Instance().GfxDrawQuadTextured( a_Position, a_Size, s, true, 1.0F, { Vector2::UP, Vector2::ZERO, Vector2::RIGHT, Vector2::ONE } );
}

void SpriteSheet::DrawSprite( uint32_t a_SpriteIndexX, uint32_t a_SpriteIndexY, const Vector2 & a_Position, const Vector2 & a_Size ) const
{
	ApplyUV( a_SpriteIndexX, a_SpriteIndexY );

	int32_t s = GraphicsManager::Instance().SetTexture( m_Texture );
	GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
	GraphicsManager::Instance().GfxDrawQuadTextured( a_Position, a_Size, s, true, 1.0F, { Vector2::UP, Vector2::ZERO, Vector2::RIGHT, Vector2::ONE } );
}

const Vector2 & SpriteSheet::SpritesPerAxis() const
{
	return m_SpritesPerAxis;
}