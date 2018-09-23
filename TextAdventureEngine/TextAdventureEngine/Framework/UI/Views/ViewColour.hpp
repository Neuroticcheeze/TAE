#pragma once
#ifndef VIEWCOLOUR_H
#define VIEWCOLOUR_H

#include <Framework/UI/View.hpp>

//=====================================================================================
class ViewColour : public View
{
public:

	ViewColour( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );
	void OnTick( float a_DeltaTime ) override;

protected:

	void OnEnabled() override {}
	void OnDisabled() override {}
	void OnMouseEnter( const Vector2 & m_MousePosition ) override;
	void OnMouseLeave( const Vector2 & m_MousePosition ) override;
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) override;
	
private:

	float m_Brightness = 1.0F;
};


#endif//VIEWCOLOUR_H