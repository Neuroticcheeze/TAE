#pragma once
#ifndef VIEWDRAGGABLE_H
#define VIEWDRAGGABLE_H

#include <Framework/UI/View.hpp>

//=====================================================================================
class ViewDraggable : public View
{
public:

	ViewDraggable( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );

	void SetExplicitOffset( bool a_Flag, const Vector2 & a_Offset );
	void SetEnabled( bool a_Enabled );
	inline bool GetEnabled() const { return m_Enabled; }
	inline bool IsBeingDragged() const { return m_Drag; }
	
	void OnTick( float a_DeltaTime ) override;

protected:

	void OnMouseEnter( const Vector2 & m_MousePosition ) override;
	void OnMouseLeave( const Vector2 & m_MousePosition ) override;
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) override;
	void OnMouseReleased( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) override;

private:

	bool m_Drag;
	bool m_Enabled;
	bool m_UseExplicitOffset;
	Vector2 m_ExplicitOffset;
	Vector2 m_TempGrabOffset;
};

#endif//VIEWDRAGGABLE_H