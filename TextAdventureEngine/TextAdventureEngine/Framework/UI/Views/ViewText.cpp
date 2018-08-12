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

	// We can call this every frame because it will only do any work if the text size changes.
	if ( !UsingBitmapFont() )
	{
		GraphicsManager::Instance().SetFontSize( m_Font, m_TextSize );
	}

	float x = GetPosition().x;
	float y = GetPosition().y;

	float firstLineHeight = !UsingBitmapFont() 
								? GraphicsManager::Instance().QueryTextSize( m_TextLines.First()->Get(), m_Font, m_TextSize ).y
								: GraphicsManager::Instance().QueryTextSize( m_TextLines.First()->Get(), m_BitmapFont ).y;
	float stringHeight = 0.0F;	
	{
		auto it = m_TextLines.First();
		while ( it < m_TextLines.End() )
		{
			const uint32_t p = it - m_TextLines.First();
			
			const float height = !UsingBitmapFont() 
									? GraphicsManager::Instance().QueryTextSize( it->Get(), m_Font, m_TextSize ).y
									: GraphicsManager::Instance().QueryTextSize( it->Get(), m_BitmapFont ).y;

			stringHeight += ( height + m_LineSpacing );
			++it;
		}
	}
	stringHeight -= m_LineSpacing;

	float xx = 0.0F;

	switch ( m_HAlign )
	{
	case GraphicsManager::TA_CENTER:
		xx = GetSize().x / 2.0F;
		break;
	case GraphicsManager::TA_RIGHT:
		xx = GetSize().x;
		break;
	}

	float hh = 0.0F;

	switch ( m_VAlign )
	{
	case GraphicsManager::TA_TOP:
		hh = GetSize().y;
		break;
	case GraphicsManager::TA_CENTER:
		if ( m_MultiVAlign )
		{
			hh = ( GetSize().y + stringHeight - firstLineHeight ) / 2.0F;
		}
		else
		{
			hh = GetSize().y / 2.0F;
		}
		break;
	case GraphicsManager::TA_BOTTOM:
		hh = stringHeight - firstLineHeight;
		break;
	}

	GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );

	{
		float h = 0.0F;
		auto it = m_TextLines.First();
		while ( it < m_TextLines.End() )
		{
			const uint32_t p = it - m_TextLines.First();
			const float height = !UsingBitmapFont() 
									? GraphicsManager::Instance().QueryTextSize( it->Get(), m_Font, m_TextSize ).y
									: GraphicsManager::Instance().QueryTextSize( it->Get(), m_BitmapFont ).y;

			GraphicsManager::Instance().TfPush();
			GraphicsManager::Instance().TfTranslate( { x + xx, y + hh - h } );
			
			if ( m_Skew )
			{
				GraphicsManager::Instance().TfShear( Vector2( 0.0F, 0.4F ) );
			}
			
			if ( UsingBitmapFont() )
			{
				GraphicsManager::Instance().GfxDrawText( it->Get(), m_BitmapFont, m_HAlign, m_VAlign );
			}
			else
			{
				GraphicsManager::Instance().GfxDrawText( it->Get(), m_Font, m_TextSize, m_HAlign, m_VAlign );
			}

			GraphicsManager::Instance().TfPop();
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

#define WORD_SEPARATORS " ", ","

	const bool wordwrap = m_WordWrap;

	CString str = m_Text;
	CString str2 = "";

	if ( wordwrap )
	{
		str.ReplaceAll( "\n ", "\n" );
		str.ReplaceAll( " \n", "\n" );

		float lineWidthMax = GetSize().x;

		Array< int32_t > locs = str.FindAll( { WORD_SEPARATORS } );
		locs.Append( str.Length() );

		const int32_t * locIt = locs.First();
		const int32_t * locEnd = locs.End();

		CString segmentStr;
		Vector2 segmentSize;

		// The total constructed line & size
		CString lineStr;
		Vector2 lineSize;

		const char * locPtLeft = str.Get();
		while ( locIt < locEnd )
		{
			const char * locPtRight = str.Get() + *locIt;

			if ( locPtLeft == locPtRight )
			{
				++locIt;
				continue;
			}


			// Per segment/word
			{
				segmentStr = CString( locPtLeft, locPtRight ).Get();
				segmentSize = !UsingBitmapFont()
								? GraphicsManager::Instance().QueryTextSize( segmentStr.Get(), m_Font, m_TextSize )
								: GraphicsManager::Instance().QueryTextSize( segmentStr.Get(), m_BitmapFont );

				// Time to push onto new line?
				if ( ( lineSize.x + segmentSize.x ) >= lineWidthMax )
				{
					str2 += lineStr;
					str2 += "\n";

					lineStr = "";
					lineSize = Vector2::ZERO;
				}

				lineSize = Vector2( lineSize.x + segmentSize.x, Max( lineSize.y, segmentSize.y ) );
				lineStr += segmentStr;
			}

			locPtLeft = locPtRight;
			++locIt;
		}

		// Add the remainder (which will be less than the line width
		str2 += lineStr;
	}
	else
	{
		str2 = str;
	}

	if ( wordwrap )
	{
		str2.ReplaceAll( "\n ", "\n" );
		str2.ReplaceAll( " \n", "\n" );
	}

	m_TextLines = str2.Split( "\n" );

	auto it = m_TextLines.First();
	while ( it != m_TextLines.End() )
	{
		it++;
	}

#undef WORD_SEPARATORS
}