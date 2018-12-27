#pragma once
#ifndef VIEWFLYOUTMENU_H
#define VIEWFLYOUTMENU_H

#include <Framework/UI/View.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/Utils/Function.hpp>

//=====================================================================================
struct KeyShortcut
{
	ENUM( KeyShortcutHeldTypes, int32_t )
		NULL_	= 0,
		CTRL	= 1 << 0,
		SHIFT	= 1 << 1,
		ALT		= 1 << 2,
	END_ENUM_F( KeyShortcutHeldTypes, int32_t );

	InputManager::KeyCode KeyCode;
	KeyShortcutHeldTypes HeldKeyCode = NULL_;

	bool IsNull() const { return HeldKeyCode == 0; }
};

//=====================================================================================
struct FlyoutContentTree;

//=====================================================================================
class ViewFlyoutMenu final 
	: public View
{
public:

	typedef Functor< void() > MenuAction;

private:

	class ViewFlyoutMenuItem 
		: public View
		, public InputManager::IKeyEventListener
	{
		friend class ViewFlyoutMenu;

	public:

		ViewFlyoutMenuItem( const char * a_Name, const MenuAction & a_MenuAction, const KeyShortcut & a_KeyShortcut, Page * a_ContainerPage, View * a_Parent = nullptr );
		~ViewFlyoutMenuItem();
		ViewText * GetLabel() const { return m_Text; }
		ViewText * GetLabelSmall() const { return m_TextSmall; }


		void OnMouseEnter( const Vector2 & m_MousePosition );
		void OnMouseLeave( const Vector2 & m_MousePosition );
		void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton );

		void OnKeyPressed( InputManager::Key a_Key ) override;

	protected:
		
		bool OnMessage( View * a_Child, uint32_t a_Message ) override;
		void OnTick( float a_DeltaTime ) override;

	private:

		ViewFlyoutMenu * m_Child;

		ViewText * m_Text;
		ViewText * m_TextSmall;
		MenuAction m_MenuAction;
		KeyShortcut m_KeyShortcut;

		uint32_t m_HideProhibitorCounter = 0;
	};

public:

	ViewFlyoutMenu( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );
	~ViewFlyoutMenu();
	
	void SetContentTree( const Array< FlyoutContentTree > & a_ContentTree );

protected:

	bool OnMessage( View * a_Child, uint32_t a_Message ) override;
	void OnTick( float a_DeltaTime ) override;

private:

	void SetList( const Array< FlyoutContentTree > & a_List );

	Array< ViewFlyoutMenuItem* > m_List;
	uint32_t m_HideProhibitorCounter = 0;
};

//=====================================================================================
struct FlyoutContentTree
{
	CString						Name;
	ViewFlyoutMenu::MenuAction	ExecuteAction;
	Array< FlyoutContentTree >	Members;
	KeyShortcut					KeyShortcut;
};

#endif//VIEWFLYOUTMENU_H