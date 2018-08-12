#pragma once
#ifndef VIEWSPRITE_H
#define VIEWSPRITE_H

#include <Framework/UI/View.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Graphics/SpriteSheet.hpp>
#include <Framework/Graphics/NineSpriteSheet.hpp>

//=====================================================================================
class ViewSprite final : public View
{
public:

	ENUM( SpriteType, uint8_t )
		SINGLE, 
		SPRITESHEET, 
		NINESPRITE,
	END_ENUM;

	ViewSprite( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr )
		: View( a_Name, a_ContainerPage, a_Parent )
		, m_SpriteType( SINGLE )
		, m_SpriteSheetIndex( 0 )
		, m_SingleSpriteTexture( 0 )
		, m_Overrider( nullptr )
	{
	}

	void SetupFromNineSprite( SpriteSheet::Texture a_Texture, float a_EdgeSize = 50.0F );
	void SetupFromSingleSprite( SpriteSheet::Texture a_Texture );
	void SetupFromSpriteSheet( const SpriteSheet & a_SpriteSheet );
	inline void SetSpriteSheetIndex( uint32_t a_SpriteSheetIndex )
	{
		m_SpriteSheetIndex = a_SpriteSheetIndex;
	}

	class IDrawOverrider abstract
	{
		friend class ViewSprite;

	protected:

		virtual bool OnDraw( const ViewSprite & a_Owner, const Vector2 & a_Position, const Vector2 & a_Size ) abstract;
	};

	void SetDrawOverrider( IDrawOverrider * a_Overrider );

protected:

	void OnTick( float a_DeltaTime );
	
private:

	IDrawOverrider * m_Overrider;

	SpriteType m_SpriteType;
	uint32_t m_SpriteSheetIndex;
	GraphicsManager::Texture m_SingleSpriteTexture;
	NineSpriteSheet m_NineSpriteSheet;
	SpriteSheet m_SpriteSheetSource;
};

#endif//VIEWSPRITE_H