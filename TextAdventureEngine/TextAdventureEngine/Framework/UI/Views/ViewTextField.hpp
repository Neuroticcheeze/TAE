#pragma once
#ifndef VIEWTEXTFIELD_H
#define VIEWTEXTFIELD_H

#include <Framework/UI/View.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/Graphics/SpriteSheet.hpp>
#include <Framework/Graphics/NineSpriteSheet.hpp>
#include <Framework/Input/InputManager.hpp>

//=====================================================================================
class ViewTextField final : public View
						  , public InputManager::ICharEventListener
{
public:

	ViewTextField( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );
	void Destruct() override;

	void SetEnabled( bool a_Enabled );

	inline bool GetEnabled() const
	{
		return m_Enabled;
	}

	inline ViewSprite & GetBackground()
	{
		return m_Background;
	}

	inline ViewText & GetTextView()
	{
		return m_Text;
	}

protected:

	void OnTick( float a_DeltaTime );
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton );
	void RequestInitialEvents( IActionListener * a_ActionListener );
	void OnCharTyped( char a_Char );
	void OnGainFocus();
	void OnLoseFocus();
	
private:

	bool	   m_Enabled;
	ViewSprite m_Background;
	ViewText   m_Text;
};

#endif//VIEWTEXTFIELD_H