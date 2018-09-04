#pragma once
#ifndef VIEWSIZER_H
#define VIEWSIZER_H

#include <Framework/UI/Views/ViewDraggable.hpp>

//=====================================================================================
class ViewSizer : public View
{
public:

	ViewSizer( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );

	void SetAxors( View::Alignment a_XAxor, View::Alignment a_YAxor ) { m_XAxor = a_XAxor; m_YAxor = a_YAxor; }
	inline bool IsBeingDragged() const { return m_Drag; }
	
	void OnTick( float a_DeltaTime ) override;

	void SetMinXYSizeF( const Vector2 & a_Value ) { m_MinXYSizeF = a_Value; }

protected:

	void OnEnabled() override {}
	void OnDisabled() override {}
	void OnMouseEnter( const Vector2 & m_MousePosition ) override;
	void OnMouseLeave( const Vector2 & m_MousePosition ) override;
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) override;
	void OnMouseReleased( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) override;

private:

	View::Alignment m_XAxor = View::Alignment::RIGHT;
	View::Alignment m_YAxor = View::Alignment::TOP;

	bool m_Drag;

	Vector2 m_MinXYSizeF;
};


#endif//VIEWSIZER_H