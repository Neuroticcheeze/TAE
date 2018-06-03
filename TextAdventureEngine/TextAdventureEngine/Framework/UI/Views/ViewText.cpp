#include "ViewText.hpp"
#include <Framework/Graphics/Graphics.hpp>

//=====================================================================================
void ViewText::SetText( const char * a_String )
{
	const bool changed = m_Text != a_String;
	m_Text = a_String;

	if ( changed )
	{
		RecalculateTextSegs();
	}
}

//=====================================================================================
void ViewText::OnTick( float a_DeltaTime )
{
	if ( m_TextLines.Count() == 0 )
	{
		return;
	}

	Graphics::SetFont( m_Font, m_TextSize );

	float x = GetPosition().x;
	float y = GetPosition().y;

	float firstLineHeight = Graphics::GetTextHeight( m_TextLines.First()->Get() );
	float stringHeight = 0.0F;	
	{
		auto it = m_TextLines.First();
		while ( it < m_TextLines.End() )
		{
			const uint32_t p = it - m_TextLines.First();
			const float height = Graphics::GetTextHeight( it->Get() );
			stringHeight += ( height + m_LineSpacing );
			++it;
		}
	}

	switch ( m_HAlign )
	{
	case LEFT:
		x = GetPosition().x;
		Graphics::SetTextAlign( Graphics::TEXT_ALIGN_LEFT );
		break;
	case MIDDLE:
		x = GetCenter().x;
		Graphics::SetTextAlign( Graphics::TEXT_ALIGN_CENTER );
		break;
	case RIGHT:
		x = GetPosition().x + GetSize().x;
		Graphics::SetTextAlign( Graphics::TEXT_ALIGN_RIGHT );
		break;
	}

	switch ( m_VAlign )
	{
	case TOP:
		y = GetPosition().y + GetSize().y - firstLineHeight;
		break;
	case CENTER:
		y = GetCenter().y - firstLineHeight + stringHeight * 0.5F;
		break;
	case BOTTOM:
		y = GetPosition().y - firstLineHeight + stringHeight;
		break;
	}

	{
		float h = 0.0F;
		auto it = m_TextLines.First();
		while ( it < m_TextLines.End() )
		{
			const uint32_t p = it - m_TextLines.First();
			const float height = Graphics::GetTextHeight( it->Get() );
			Graphics::DrawText( { x, y - h }, it->Get() );
			h += ( height + m_LineSpacing );
			++it;
		}
	}
}

//=====================================================================================
void ViewText::OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton )
{
}

//=====================================================================================
void ViewText::OnReset()
{
	m_Text = nullptr;
	m_TextLines.Clear();
}

//=====================================================================================
void ViewText::RecalculateTextSegs()
{
	PROFILE;

	Graphics::SetFont( m_Font, m_TextSize );

#define WORD_SEPARATORS " ", "-", ".", ",", "/", "\\", "~", "`"

	const bool wordwrap = m_WordWrap;

	CString str = m_Text;
	str.ReplaceAll( "\n ", "\n" );
	str.ReplaceAll( " \n", "\n" );

	if ( wordwrap )
	{
		const Array< int32_t > locs = str.FindAll( { WORD_SEPARATORS } );
		const int32_t * locIt = locs.First();
		const int32_t * locEnd = locs.End();

		float currentLineWidth = 0.0F;
		const float maxLineWidth = GetSize().x;
		int32_t off = 0;
		const char * start = str.Get();
		while ( locIt < locEnd )
		{
			const char * wordsep = str.Get() + ( *locIt ) + off;

			CString seg( start, wordsep );

			if (Graphics::GetTextWidth( ( seg[ 0 ] == '\n' ) ? ( seg.Get() + 1 ) : ( seg.Get() ) ) > maxLineWidth )
			{
				if ( locIt > locs.First() )
				{
					locIt--;
				}

				int32_t k = *locIt + 1 + off;
				str = str.SubString( 0, k ) + "\n" + str.SubString( k, str.Length() - k );
				++off;
				const char * end = str.Get() + ( *locIt ) + off;
				start = end;
			}

			++locIt;
		}
	}

	m_TextLines = str.Split( "\n" );

#undef WORD_SEPARATORS
}