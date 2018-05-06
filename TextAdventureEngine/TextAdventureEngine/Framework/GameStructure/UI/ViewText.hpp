#pragma once
#ifndef VIEWTEXT_H
#define VIEWTEXT_H

#include "View.hpp"
#include <Framework/Containers/Array.hpp>

//=====================================================================================
class ViewText final : public View
{
public:

	ENUM( TextHAlignment, uint8_t )
		LEFT, MIDDLE, RIGHT
	END_ENUM;

	ENUM( TextVAlignment, uint8_t )
		TOP, CENTER, BOTTOM
	END_ENUM;

	ViewText( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr )
		: View( a_Name, a_ContainerPage, a_Parent )
		, m_Font( 0 )
		, m_TextSize( 32.0F )
		, m_WordWrap( false )
		, m_LineSpacing( 10.0F )
	{
	}
	
	void SetText( const char * a_String );
	
	inline const CString & GetText() const
	{
		return m_Text;
	}

	void SetFontSize( float a_FontSize )
	{
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

	void SetFont( uint32_t a_Font )
	{
		const bool changed = a_Font != m_Font;

		m_Font = a_Font;
		
		if ( changed )
		{
			RecalculateTextSegs();
		}
	}
	
	uint32_t GetFont() const
	{
		return m_Font;
	}

	inline void SetHAlign( TextHAlignment a_TextAlignment )
	{
		m_HAlign = a_TextAlignment;
	}

	inline void SetVAlign( TextVAlignment a_TextAlignment )
	{
		m_VAlign = a_TextAlignment;
	}

	inline TextHAlignment GetHAlign() const
	{
		return m_HAlign;
	}

	inline TextVAlignment GetVAlign() const
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

protected:

	void OnReset();
	void OnTick( float a_DeltaTime );
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) override;
	
private:

	void RecalculateTextSegs();

	CString m_Text;
	Array< CString > m_TextLines;

	float m_TextSize;
	uint32_t m_Font;
	TextHAlignment m_HAlign;
	TextVAlignment m_VAlign;
	bool m_WordWrap;
	float m_LineSpacing;
};

#endif//VIEWTEXT_H