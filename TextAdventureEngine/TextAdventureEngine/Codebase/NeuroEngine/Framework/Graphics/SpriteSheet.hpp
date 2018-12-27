#pragma once
#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <Framework/Utils/MemoryExt.hpp>
#include <Framework/Utils/Tuple.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
class SpriteSheet
{
public:
	
	typedef WeakPointer< Tuple3< void*, void*, uint8_t > > Texture;

	SpriteSheet( Texture a_Texture = nullptr, const Vector2 & a_SpritesPerAxis = Vector2::ZERO );

	void ApplyUV( uint32_t a_SpriteIndex ) const;
	void ApplyUV( uint32_t a_SpriteIndexX, uint32_t a_SpriteIndexY ) const;
	void DrawSprite( uint32_t a_SpriteIndex, const Vector2 & a_Position, const Vector2 & a_Size ) const;
	void DrawSprite( uint32_t a_SpriteIndexX, uint32_t a_SpriteIndexY, const Vector2 & a_Position, const Vector2 & a_Size ) const;
	const Vector2 & SpritesPerAxis() const;

private:

	Texture m_Texture;
	Vector2 m_SpritesPerAxis;
};

#endif//SPRITESHEET_H