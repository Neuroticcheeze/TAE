#include "PageItemInventory.hpp"
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
void PageItemInventory::OnEnterPage()
{
	const float ITEM_START_TOP_LEFT = 0.1F;
	const float ITEM_START_HEIGHT = 0.5F;
	const float ITEM_SPACING = -0.045F;
	const float NAME_ICON_SPACING = -0.02F;
	const float ICON_SCALE = 0.165F;

	for ( uint32_t k = 0; k < NUM_ITEMS_PER_PAGE; ++k )
	{
		ViewSprite & icon = m_ItemIcons[ k ];
		icon.SetBorder( View::Alignment::LEFT, ITEM_START_TOP_LEFT);
		icon.SetBorder( View::Alignment::RIGHT, 1.0F - ( ITEM_START_TOP_LEFT + ICON_SCALE ) );
		icon.SetBorder( View::Alignment::TOP, ITEM_START_HEIGHT + k * ( ICON_SCALE + ITEM_SPACING ) );
		icon.SetBorder( View::Alignment::BOTTOM, 1.0F - ( ITEM_START_HEIGHT + k * ( ICON_SCALE + ITEM_SPACING ) + ICON_SCALE ) );
		icon.SetupFromSpriteSheet( _GetSpriteSheet( "ITEMS" ) );
		icon.SetZOrder( -1 );

		ViewText & name = m_ItemNames[ k ];
		name.SetBorder( View::Alignment::LEFT, ITEM_START_TOP_LEFT + ICON_SCALE + NAME_ICON_SPACING );
		name.SetBorder( View::Alignment::RIGHT, 0.0F );
		name.SetBorder( View::Alignment::TOP, ITEM_START_HEIGHT + k * ( ICON_SCALE + ITEM_SPACING ) );
		name.SetBorder( View::Alignment::BOTTOM, 1.0F - ( ITEM_START_HEIGHT + k * ( ICON_SCALE + ITEM_SPACING ) + ICON_SCALE ) );
		name.SetFont( _GetFont( "DEFAULT" ) );
		name.SetWordWrap( false );
		name.SetVAlign( ViewText::CENTER );
		name.SetZOrder( -1 );
	}

	m_BackgroundSprite.SetZOrder( 1 );
	m_BackgroundSprite.SetTint( Colour::WHITE );
	m_BackgroundSprite.SetBorder( View::Alignment::LEFT, 0.0F );
	m_BackgroundSprite.SetBorder( View::Alignment::RIGHT, 0.0F );
	m_BackgroundSprite.SetBorder( View::Alignment::TOP, ITEM_START_HEIGHT - ITEM_START_TOP_LEFT );
	m_BackgroundSprite.SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_BackgroundSprite.SetupFromNineSprite( _GetTexture( "BG_INVENTORY_0" ), 80.0F );

	m_ItemDescription.SetTint( Colour::WHITE );
	m_ItemDescription.SetBorder( View::Alignment::LEFT, 0.05F );
	m_ItemDescription.SetBorder( View::Alignment::RIGHT, 0.05F );
	m_ItemDescription.SetBorder( View::Alignment::TOP, 0.05F );
	m_ItemDescription.SetBorder( View::Alignment::BOTTOM, ITEM_START_HEIGHT );
	m_ItemDescription.SetFont( _GetFont( "DEFAULT" ) );
	m_ItemDescription.SetFontSize( 38.0F );
	m_ItemDescription.SetWordWrap( true );
	m_ItemDescription.SetVAlign( ViewText::TOP );

	m_SelectedItemIndex = 1; // To bypass sameval check inside 'SelectItem'
	SelectItem( 0 );
}

//=====================================================================================
void PageItemInventory::OnExitPage()
{
	m_ItemDescription.Reset();
}

//=====================================================================================
void PageItemInventory::SelectItem( int32_t a_Index )
{
	a_Index = Clamp( a_Index, 0, ( int32_t )m_InventoryReference.GetItems().Count() - 1 );
	if ( a_Index != m_SelectedItemIndex )
	{
		m_SelectedItemIndex = a_Index;
		UpdateItemsDisplay( ( a_Index / NUM_ITEMS_PER_PAGE ) * NUM_ITEMS_PER_PAGE );
	}
}

//=====================================================================================
void PageItemInventory::OnTick( float a_DeltaTime )
{
	if ( InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_UP ) == InputManager::ButtonState::PRESSED )
	{
		SelectItem( GetSelectedItem() - 1 );
	}
	
	if ( InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_DOWN ) == InputManager::ButtonState::PRESSED )
	{
		SelectItem( GetSelectedItem() + 1 );
	}

	if ( InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_RETURN ) == InputManager::ButtonState::PRESSED )
	{
		ItemGroup & ig = m_InventoryReference.GetItems()[ ( uint32_t )GetSelectedItem() ];
		PRINT( ig.Use( m_InventoryReference ).Get() );
		//TODO... add listeners for when the player presses enter. Listeners should receive the "ItemGroup &" the player selected
	}
}

//=====================================================================================
void PageItemInventory::UpdateItemsDisplay( uint32_t a_OffsetIntoInventory )
{
	const Array< ItemGroup > itemgroups = m_InventoryReference.GetItems();

	if ( itemgroups.Count() == 0 )
	{
		return;
	}

	if ( itemgroups.Count() > a_OffsetIntoInventory )
	{
		uint32_t countAfter = itemgroups.Count() - a_OffsetIntoInventory;

		for ( uint32_t k = 0; k < NUM_ITEMS_PER_PAGE; ++k )
		{
			const bool show = k < countAfter;
			const bool selected = ( k + a_OffsetIntoInventory ) == m_SelectedItemIndex;
			
			// tint
			m_ItemNames[ k ].SetTint( show ? ( selected ? Colour::WHITE : Colour::GRAY * 1.5F ) : Colour::INVISIBLE );
			m_ItemIcons[ k ].SetTint( show ? Colour::WHITE : Colour::INVISIBLE );

			// contents
			if ( show )
			{
				const ItemGroup & ig = itemgroups[ k + a_OffsetIntoInventory ];
				CString count = ig.GetCount() > 1 ? CString().Format( " ( x%u )", ig.GetCount() ) : "";
				m_ItemNames[ k ].SetText( ( ig.GetItem().GetName() + count ).Get() );
				m_ItemNames[ k ].SetFontSize( selected ? 62.0F : 48.0F );
				m_ItemIcons[ k ].SetSpriteSheetIndex( ig.GetItem().GetIcon() );
			}
		}
	}

	if ( InRange( m_SelectedItemIndex, 0, ( int32_t )itemgroups.Count() - 1 ) )
	{
		const ItemGroup & ig = itemgroups[ m_SelectedItemIndex ];
		m_ItemDescription.SetText( ig.GetItem().GetDescription().Get() );
	}
}