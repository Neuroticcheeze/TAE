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
						  , public InputManager::IKeyEventListener
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

	inline void SetEmptyTextString( const char * a_String )
	{
		m_EmptyTextString = a_String;
	}

	void SetCursorToStart()
	{
		m_CursorPosition = 0;
		RefreshCursor();
	}

	void MoveCursor( int32_t a_By )
	{
		m_CursorPosition += a_By;
		RefreshCursor();
	}

	void SetCursor( int32_t a_To )
	{
		m_CursorPosition = a_To;
		RefreshCursor();
	}

	void SetCursorToEnd()
	{
		m_CursorPosition = INT_MAX;
		RefreshCursor();
	}

	int32_t GetCursor() const
	{
		return m_CursorPosition;
	}

	inline void SetMultilineEnabled( bool a_Enabled = true ) { m_MultilineEnabled = a_Enabled; }
	inline bool GetMultilineEnabled() const { return m_MultilineEnabled; }

protected:

	void OnTick( float a_DeltaTime );
	void OnTickPost( float a_DeltaTime );
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton );
	void RequestInitialEvents( IActionListener * a_ActionListener );
	void OnCharTyped( char a_Char );
	void OnKeyPressed( InputManager::Key a_Key );
	void OnGainFocus();
	void OnLoseFocus();
	
private:

	int32_t m_CursorPosition;
	void RefreshCursor();
	void RefreshText();
	uint32_t m_CachedTextLength;

	bool	   m_MultilineEnabled;
	bool	   m_Enabled;
	ViewSprite m_Background;
	ViewText   m_Text;
	ViewText   m_EmptyTextView;
	struct { float xOffset, lineHeight, yOffset; }
			   m_TextBB;
	CString	   m_EmptyTextString;
};

#endif//VIEWTEXTFIELD_H