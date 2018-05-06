#include "GraphicsManager.hpp"
#include <SIGIL-0.9.0/sl.h>

//=====================================================================================
void GraphicsManager::Init()
{
}

void GraphicsManager::Finalise()
{
}

//=====================================================================================
const GraphicsManager::Texture & GraphicsManager::LoadTexture( uint32_t a_HashName, const char * a_Path )
{
	if ( !m_Textures.Contains( a_HashName ) )
	{
		m_Textures.Put( a_HashName, slLoadTexture( a_Path ) );
	}
	
	return *m_Textures[ a_HashName ];
}

//=====================================================================================
const SpriteSheet & GraphicsManager::LoadTextureAsSpriteSheet( uint32_t a_HashName, const char * a_Path, const Vector2 & a_SpritesPerAxis )
{
	if ( !m_SpriteSheets.Contains( a_HashName ) )
	{
		m_SpriteSheets.Put( a_HashName, SpriteSheet( LoadTexture( a_HashName, a_Path ), a_SpritesPerAxis ) );
	}

	if ( !m_Textures.Contains( a_HashName ) )
	{
		LoadTexture( a_HashName, a_Path );
	}

	return *m_SpriteSheets[ a_HashName ];
}

//=====================================================================================
const SpriteSheet & GraphicsManager::LoadSpriteSheet( uint32_t a_HashName, const SpriteSheet & a_SpriteSheet )
{
	if ( !m_SpriteSheets.Contains( a_HashName ) )
	{
		m_SpriteSheets.Put( a_HashName, a_SpriteSheet );
	}
	
	return *m_SpriteSheets[ a_HashName ];
}

//=====================================================================================
const GraphicsManager::Font & GraphicsManager::LoadFont( uint32_t a_HashName, const char * a_Path )
{
	if ( !m_Fonts.Contains( a_HashName ) )
	{
		m_Fonts.Put( a_HashName, slLoadFont( a_Path ) );
	}
	
	return *m_Fonts[ a_HashName ];
}

//=====================================================================================
const GraphicsManager::Texture & GraphicsManager::GetTexture( uint32_t a_HashName ) const
{
	return *m_Textures[ a_HashName ];
}

//=====================================================================================
const SpriteSheet & GraphicsManager::GetSpriteSheet( uint32_t a_HashName ) const
{
	return *m_SpriteSheets[ a_HashName ];
}

//=====================================================================================
const GraphicsManager::Font & GraphicsManager::GetFont( uint32_t a_HashName ) const
{
	return *m_Fonts[ a_HashName ];
}