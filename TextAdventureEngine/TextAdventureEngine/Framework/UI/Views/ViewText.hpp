#pragma once
#ifndef VIEWTEXT_H
#define VIEWTEXT_H

#include <Framework/UI/View.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Data/String/StringTable.hpp>

//=====================================================================================
class ViewText final : public View
{
public:

	ViewText( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr )
		: View( a_Name, a_ContainerPage, a_Parent )
		, m_Font( nullptr )
		, m_BitmapFont( nullptr )
		, m_TextSize( 32.0F )
		, m_WordWrap( false )
		, m_LineSpacing( 0.0F )
		, m_Skew( false )
		, m_MultiVAlign( true )
		, m_HAlign( GraphicsManager::TA_LEFT )
		, m_VAlign( GraphicsManager::TA_TOP )
		, m_Formatted( false )
		, m_CharacterLimit( INT_MAX )
	{
		SetEnabled();
	}

	void SetText( const char * a_String, bool a_Formatted = false );

	CString GetText() const
	{
		return m_Text;
	}

	void SetFontSize( float a_FontSize )
	{
		ASSERT_WARN( m_Font != nullptr, "Setting fontsize for Font, but we're not using fonts; perhaps a call to SetBitmapFont was recently made?" );

		const bool changed = !Approximate( m_TextSize, a_FontSize );
		
		m_TextSize = Max( 0.0F, a_FontSize );
		
		if ( changed ) 
		{
			RecalculateTextSegs();
		}
	}
	
	float GetFontSize() const
	{
		return m_TextSize;
	}

	inline void SetLineSpacing( float a_LineSpacing )
	{
		m_LineSpacing = a_LineSpacing;
	}

	inline float GetLineSpacing() const
	{
		return m_LineSpacing;
	}

	void SetFont( GraphicsManager::Font a_Font )
	{
		const bool changed = a_Font != m_Font;

		m_Font = a_Font;
		
		if ( m_Font != nullptr )
		{
			m_BitmapFont = nullptr;
		}

		if ( changed )
		{
			RecalculateTextSegs();
		}
	}

	void SetBitmapFont( GraphicsManager::BitmapFont a_BitmapFont )
	{
		const bool changed = a_BitmapFont != m_BitmapFont;

		m_BitmapFont = a_BitmapFont;

		if ( m_BitmapFont != nullptr )
		{
			m_Font = nullptr;
		}

		if ( changed )
		{
			RecalculateTextSegs();
		}
	}

	GraphicsManager::Font GetFont() const
	{
		return m_Font;
	}

	GraphicsManager::BitmapFont GetBitmapFont() const
	{
		return m_BitmapFont;
	}

	inline void SetHAlign( GraphicsManager::TextAlignment a_TextAlignment )
	{
		m_HAlign = a_TextAlignment;
	}

	inline void SetVAlign( GraphicsManager::TextAlignment a_TextAlignment )
	{
		m_VAlign = a_TextAlignment;
	}

	inline GraphicsManager::TextAlignment GetHAlign() const
	{
		return m_HAlign;
	}

	inline GraphicsManager::TextAlignment GetVAlign() const
	{
		return m_VAlign;
	}

	void SetWordWrap( bool a_Flag )
	{
		const bool changed = m_WordWrap != a_Flag;
		m_WordWrap = a_Flag;

		if ( changed )
		{
			RecalculateTextSegs();
		}
	}

	inline bool GetWordWrap() const
	{
		return m_WordWrap;
	}

	inline void SetSkew( bool a_Flag )
	{
		m_Skew = a_Flag;
	}

	inline float GetSkew() const
	{
		return m_Skew;
	}

	void SetMultiVAlign( bool a_Flag )
	{
		m_MultiVAlign = a_Flag;
	}

	inline bool GetMultiVAlign() const
	{
		return m_MultiVAlign;
	}

	inline void SetCharacterLimit( int32_t a_CharacterLimit )
	{
		m_CharacterLimit = Clamp( a_CharacterLimit, 0, ( int32_t )m_Text.Length() );
	}

	inline int32_t GetCharacterLimit() const
	{
		return m_CharacterLimit;
	}

protected:

	void OnEnabled() override {}
	void OnDisabled() override {}
	void OnReset();
	void OnTick( float a_DeltaTime );
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) override;
	
private:

	bool UsingBitmapFont() const
	{
		return m_BitmapFont != nullptr;
	}

	void RecalculateTextSegs();

	CString m_Text;
	Array< StringEntry > m_TextLines;

	float m_TextSize;
	GraphicsManager::Font m_Font;
	GraphicsManager::BitmapFont m_BitmapFont;
	GraphicsManager::TextAlignment m_HAlign;
	GraphicsManager::TextAlignment m_VAlign;
	bool m_WordWrap;
	float m_LineSpacing;
	bool m_MultiVAlign;
	bool m_Skew;
	bool m_Formatted;
	int32_t m_CharacterLimit;
};

#endif//VIEWTEXT_H