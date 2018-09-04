#pragma once
#ifndef VIEWTICKBOX_H
#define VIEWTICKBOX_H

#include <Framework/UI/View.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/Graphics/SpriteSheet.hpp>
#include <Framework/Graphics/NineSpriteSheet.hpp>

//=====================================================================================
class ViewTickBox final : public View
{
public:

	ViewTickBox( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );

	static const uint32_t SPRITE_BACKGROUND = 0;
	static const uint32_t SPRITE_TICKMARK = 1;

	inline ViewSprite & GetSprite( uint32_t a_Type )
	{
		return m_Sprites[ a_Type ];
	}

	void SetValue( bool a_Value );
	inline bool GetValue() const
	{
		return m_Flag;
	}

protected:

	void OnEnabled() override;
	void OnDisabled() override;
	void OnTick( float a_DeltaTime );
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton );
	void RequestInitialEvents( IActionListener * a_ActionListener );

private:

	bool	   m_Flag;
	ViewSprite m_Sprites[ 2 ] = 
	{
		ViewSprite( "Background", nullptr, this ),
		ViewSprite( "Tickmark", nullptr, this )
	};
};

#endif//VIEWTICKBOX_H