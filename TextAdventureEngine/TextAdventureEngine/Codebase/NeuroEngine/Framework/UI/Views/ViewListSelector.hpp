#pragma once
#ifndef VIEWLISTSELECTOR_H
#define VIEWLISTSELECTOR_H

#include <Framework/UI/View.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewText.hpp>

//=====================================================================================
class ViewListSelector final 
	: public View
	, public InputManager::IKeyEventListener
{
private:

	class ViewSelection : public View
	{
	public:

		ViewSelection( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );
		~ViewSelection();
		ViewText * GetLabel() const { return m_Text; }

		void OnMouseEnter( const Vector2 & m_MousePosition );
		void OnMouseLeave( const Vector2 & m_MousePosition );
		void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton );

	protected:
		void OnEnabled() override {}
		void OnDisabled() override {}

	private:

		ViewText * m_Text;
	};

public:

	ViewListSelector( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );
	~ViewListSelector();

	const Array< CString > & GetList() const { return m_TextList; }
	void SetList( const Array< CString > & a_List );
	void SetSelectedIndex( int32_t a_Index );
	int32_t GetSelectedIndex() const;
	void SetSingleClickSelection( bool a_Flag ) { m_SingleClickSelection = a_Flag; }
	bool GetSingleClickSelection() const { return m_SingleClickSelection; }

protected:

	void OnEnabled() override {}
	void OnDisabled() override {}
	void OnKeyPressed( InputManager::Key a_Key ) override;
	void RequestInitialEvents( IActionListener * a_ActionListener );
	
private:

	void RefreshSelection();

	void OnSelection( ViewSelection * a_Selection );
	void OnSelectionHoverBegin( ViewSelection * a_Selection );
	void OnSelectionHoverEnd( ViewSelection * a_Selection );

	Array< ViewSelection* > m_List;
	Array< CString > m_TextList;
	int32_t m_SelectedIndex;
	bool m_SingleClickSelection;
};

#endif//VIEWLISTSELECTOR_H