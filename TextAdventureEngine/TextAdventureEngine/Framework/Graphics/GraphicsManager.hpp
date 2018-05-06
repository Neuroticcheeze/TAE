#pragma once
#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Graphics/SpriteSheet.hpp>

//=====================================================================================
struct Vector2;

//=====================================================================================
class GraphicsManager final : public Manager< GraphicsManager >
{
public:

	typedef int32_t Font;
	typedef int32_t Texture;

	void Init();
	void Finalise();

	const Texture & LoadTexture( uint32_t a_HashName, const char * a_Path );
	const SpriteSheet & LoadTextureAsSpriteSheet( uint32_t a_HashName, const char * a_Path, const Vector2 & a_SpritesPerAxis );
	const SpriteSheet & LoadSpriteSheet( uint32_t a_HashName, const SpriteSheet & a_SpriteSheet );
	const Font & LoadFont( uint32_t a_HashName, const char * a_Path );
	const Texture & GetTexture( uint32_t a_HashName ) const;
	const SpriteSheet & GetSpriteSheet( uint32_t a_HashName ) const;
	const Font & GetFont( uint32_t a_HashName ) const;

private:

	Dictionary< uint32_t, Texture > m_Textures;
	Dictionary< uint32_t, SpriteSheet > m_SpriteSheets;
	Dictionary< uint32_t, Font > m_Fonts;
};

//=====================================================================================
#define _GetTexture( HASHNAME ) GraphicsManager::Instance().GetTexture( WSID( HASHNAME ) )
#define _GetSpriteSheet( HASHNAME ) GraphicsManager::Instance().GetSpriteSheet( WSID( HASHNAME ) )
#define _GetFont( HASHNAME ) GraphicsManager::Instance().GetFont( WSID( HASHNAME ) )


#endif//GRAPHICSMANAGER_H