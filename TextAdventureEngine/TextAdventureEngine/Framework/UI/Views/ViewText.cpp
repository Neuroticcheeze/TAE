#include "ViewText.hpp"
#include <Framework/Graphics/Graphics.hpp>

//=====================================================================================
void ViewText::SetText( const char * a_String, bool a_Formatted )
{
	const bool changed = m_Text != a_String || m_Formatted != a_Formatted;

	if ( changed )
	{
		m_Formatted = a_Formatted;
		m_Text = a_String;
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

	Array< float > lineWidths;

	float firstLineHeight = !UsingBitmapFont() 
								? GraphicsManager::Instance().QueryTextSize( m_TextLines.First()->UnformattedString.Get(), m_Font, m_TextSize ).y
								: GraphicsManager::Instance().QueryTextSize( m_TextLines.First()->UnformattedString.Get(), m_BitmapFont ).y;
	float stringHeight = 0.0F;	
	{
		auto it = m_TextLines.First();
		while ( it < m_TextLines.End() )
		{
			const uint32_t p = it - m_TextLines.First();
			
			Vector2 size = !UsingBitmapFont() 
									? GraphicsManager::Instance().QueryTextSize( it->UnformattedString.Get(), m_Font, m_TextSize )
									: GraphicsManager::Instance().QueryTextSize( it->UnformattedString.Get(), m_BitmapFont );

			lineWidths.Append( size.x );

			stringHeight += ( size.y + m_LineSpacing );
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
		int32_t charBudget = m_CharacterLimit;

		float h = 0.0F;
		auto it = m_TextLines.First();
		while ( it < m_TextLines.End() )
		{
			if ( charBudget <= 0 )
			{
				break;
			}

			const uint32_t p = it - m_TextLines.First();
			const float height = !UsingBitmapFont() 
									? GraphicsManager::Instance().QueryTextSize( it->UnformattedString.Get(), m_Font, m_TextSize ).y
									: GraphicsManager::Instance().QueryTextSize( it->UnformattedString.Get(), m_BitmapFont ).y;

			float lineAlignment = 0.0F;

			if ( m_HAlign == GraphicsManager::TA_CENTER )
			{
				lineAlignment = -lineWidths[ static_cast< uint32_t >( it - m_TextLines.First() ) ] * 0.5F;
			}

			else if ( m_HAlign == GraphicsManager::TA_RIGHT )
			{
				lineAlignment = -lineWidths[ static_cast< uint32_t >( it - m_TextLines.First() ) ];
			}

			GraphicsManager::Instance().TfPush();
			GraphicsManager::Instance().TfTranslate( { x + xx + lineAlignment, y + hh - h } );
			
			if ( m_Skew )
			{
				GraphicsManager::Instance().TfShear( Vector2( 0.0F, 0.4F ) );
			}
			
			Vector2 textSize;

			if ( it->RawString.Length() > 0 )
			{
				StringEntry::Iterator seIter = it->GetIterator(
					[]( StringEntry::Symbol::FormatType a_FormatType, 
						StringEntry::Symbol::ExtendedFormatType a_ExtendedFormatType, 
						const StringEntry::Symbol::FormatParameter & a_FormatParameter )
					{
						switch ( a_FormatType )
						{
						case StringEntry::Symbol::FONTCOLOUR:
							GraphicsManager::Instance().SetColour( a_FormatParameter.Colour, GraphicsManager::COL_PRIMARY );
							break;
						}
					},
					[&]( StringEntry::Symbol::FormatType a_FormatType, 
							StringEntry::Symbol::ExtendedFormatType a_ExtendedFormatType )
					{
						switch ( a_FormatType )
						{
						case StringEntry::Symbol::FONTCOLOUR:
							GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_PRIMARY );
							break;
						}
					},
					[&]( const CString & a_String )
					{
						GraphicsManager::Instance().TfTranslate( Vector2( textSize.x, 0.0F ) );

						if ( UsingBitmapFont() )
						{
							textSize = GraphicsManager::Instance().GfxDrawText( a_String.Get(), m_BitmapFont, charBudget, false, GraphicsManager::TextAlignment::TA_LEFT, m_VAlign );
						}
						else
						{
							textSize = GraphicsManager::Instance().GfxDrawText( a_String.SubString( 0, charBudget ).Get(), m_Font, m_TextSize, GraphicsManager::TextAlignment::TA_LEFT, m_VAlign );
						}

						charBudget -= it->UnformattedString.Length();
					}, 
					[]( uint32_t a_TokenID )
					{
					} 
				);

				// Actually process the symbols per-line
				while ( seIter )
				{
					seIter.Handle();
					++seIter;
				}
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
				CString unformattedStr;
				
				if ( m_Formatted )
				{
					unformattedStr = StringTable::Process( segmentStr.Get() ).UnformattedString;
				}

				else
				{
					unformattedStr = segmentStr;
				}
				
				segmentSize = !UsingBitmapFont()
								? GraphicsManager::Instance().QueryTextSize( unformattedStr.Get(), m_Font, m_TextSize )
								: GraphicsManager::Instance().QueryTextSize( unformattedStr.Get(), m_BitmapFont );

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

	Array< CString > lines = str2.Split( "\n" );

	m_TextLines.Clear();

	auto it = lines.First();
	while ( it != lines.End() )
	{
		if ( m_Formatted )
		{
			m_TextLines.Append( StringTable::Process( it->Get() ) );
		}
		
		else
		{
			m_TextLines.Append( StringTable::ProcessUnformatted( it->Get() ) );
		}

		++it;
	}

#undef WORD_SEPARATORS
}