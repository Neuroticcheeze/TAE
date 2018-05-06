#pragma once
#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
class SpriteSheet
{
public:

	SpriteSheet( int32_t a_Texture = -1, const Vector2 & a_SpritesPerAxis = Vector2::ZERO );

	void ApplyUV( uint32_t a_SpriteIndex ) const;
	void ApplyUV( uint32_t a_SpriteIndexX, uint32_t a_SpriteIndexY ) const;
	void DrawSprite( uint32_t a_SpriteIndex, const Vector2 & a_Position, const Vector2 & a_Size ) const;
	void DrawSprite( uint32_t a_SpriteIndexX, uint32_t a_SpriteIndexY, const Vector2 & a_Position, const Vector2 & a_Size ) const;
	const Vector2 & SpritesPerAxis() const;

private:

	int32_t m_Texture;
	Vector2 m_SpritesPerAxis;
};

#endif//SPRITESHEET_H