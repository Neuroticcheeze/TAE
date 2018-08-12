#pragma once
#ifndef VIEWBUTTON_H
#define VIEWBUTTON_H

#include <Framework/UI/View.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/Graphics/SpriteSheet.hpp>
#include <Framework/Graphics/NineSpriteSheet.hpp>

//=====================================================================================
class ViewButton final : public View
{
public:

	ViewButton( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );

	void SetEnabled( bool a_Enabled );

	inline bool GetEnabled() const
	{
		return m_Enabled;
	}

	inline ViewSprite & GetBackground()
	{
		return m_Background;
	}

	inline ViewText & GetLabel()
	{
		return m_Label;
	}

protected:

	void OnTick( float a_DeltaTime );
	void OnMouseEnter( const Vector2 & m_MousePosition );
	void OnMouseLeave( const Vector2 & m_MousePosition );
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton );
	void OnMouseReleased( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton );
	void RequestInitialEvents( IActionListener * a_ActionListener );
	
private:

	bool	   m_Enabled;
	ViewSprite m_Background;
	ViewText   m_Label;
};

#endif//VIEWBUTTON_H