#include "ViewSprite.hpp"
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
void ViewSprite::OnTick( float a_DeltaTime )
{
	if ( m_Overrider )
	{
		if ( !m_Overrider( *this, GetPosition(), GetSize() ) )
		{
			return;
		}
	}

	GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
	//GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, GetTint().a < 1.0F );

	switch ( m_SpriteType )
	{
	case ViewSprite::SINGLE:
		if ( m_SingleSpriteTexture )
		{
			GraphicsManager::Instance().GfxSetUV();
			int32_t s = GraphicsManager::Instance().SetTexture( m_SingleSpriteTexture );
			GraphicsManager::Instance().GfxDrawQuadTextured( GetPosition(), GetSize(), s );
		}
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
void ViewSprite::SetDrawOverrider(  DrawOverrider a_Overrider )
{
	m_Overrider = a_Overrider;
}

//=====================================================================================
void ViewSprite::SetupFromNineSprite( SpriteSheet::Texture a_Texture, float a_EdgeSize )
{
	m_SpriteType = NINESPRITE;
	m_NineSpriteSheet = NineSpriteSheet( a_Texture, a_EdgeSize );
}

//=====================================================================================
void ViewSprite::SetupFromSingleSprite( SpriteSheet::Texture a_Texture )
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