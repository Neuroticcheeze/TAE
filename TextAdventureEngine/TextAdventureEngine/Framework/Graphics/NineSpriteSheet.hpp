#pragma once
#ifndef NINESPRITESHEET_H
#define NINESPRITESHEET_H

#include <Framework/Graphics/SpriteSheet.hpp>
#include <Framework/Math/Math.hpp>

//=====================================================================================
class NineSpriteSheet : protected SpriteSheet
{
public:

	NineSpriteSheet( int32_t a_Texture = -1, float a_EdgeSize = 50.0F );

	inline void SetEdgeSize( float a_EdgeSize )
	{
		m_EdgeSize = Max( 0.0F, a_EdgeSize);
	}

	inline float GetEdgeSize() const
	{
		return m_EdgeSize;
	}

	void DrawNineSprite( const Vector2 & a_Position, const Vector2 & a_Size ) const;

private:

	float m_EdgeSize;
};

#endif//NINESPRITESHEET_H