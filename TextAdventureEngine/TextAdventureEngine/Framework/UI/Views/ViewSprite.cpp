#include "ViewSprite.hpp"
#include <Framework/Graphics/Graphics.hpp>

//=====================================================================================
void ViewSprite::OnTick( float a_DeltaTime )
{
	switch ( m_SpriteType )
	{
	case ViewSprite::SINGLE:
		Graphics::SetSpriteTiling( Vector2( 1.0F ) );
		Graphics::SetSpriteScroll( Vector2( 0.0F ) );
		Graphics::DrawSprite( m_SingleSpriteTexture, GetCenter(), GetSize() );
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