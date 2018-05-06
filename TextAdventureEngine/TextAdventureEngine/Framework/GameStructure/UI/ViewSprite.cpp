#include "ViewSprite.hpp"
#include <SIGIL-0.9.0/sl.h>

//=====================================================================================
void ViewSprite::OnTick( float a_DeltaTime )
{
	switch ( m_SpriteType )
	{
	case ViewSprite::SINGLE:
		slSprite( m_SingleSpriteTexture, GetCenter().x, GetCenter().y, GetSize().x, GetSize().y );
		break;
	case ViewSprite::SPRITESHEET:
		m_SpriteSheetSource.DrawSprite( m_SpriteSheetIndex, GetPosition(), GetSize() );
		break;
	case ViewSprite::NINESPRITE:
		m_NineSpriteSheet.DrawNineSprite( GetPosition(), GetSize() );
		break;
	}
}

//=====================================================================================
void ViewSprite::SetupFromNineSprite( int32_t a_Texture, float a_EdgeSize )
{
	m_SpriteType = NINESPRITE;
	m_NineSpriteSheet = NineSpriteSheet( a_Texture, a_EdgeSize );
}

//=====================================================================================
void ViewSprite::SetupFromSingleSprite( int32_t a_Texture )
{
	m_SpriteType = SINGLE;
	m_SingleSpriteTexture = a_Texture;
}

//=====================================================================================
void ViewSprite::SetupFromSpriteSheet( const SpriteSheet & a_SpriteSheet )
{
	m_SpriteType = SPRITESHEET;
	m_SpriteSheetSource = a_SpriteSheet;
}