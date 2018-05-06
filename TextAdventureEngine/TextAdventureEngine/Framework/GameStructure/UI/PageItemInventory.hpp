#pragma once
#ifndef PAGEITEMINVENTORY_H
#define PAGEITEMINVENTORY_H

#include "Page.hpp"
#include "ViewText.hpp"
#include "ViewSprite.hpp"
#include <Framework/GameStructure/Items/Inventory.hpp>

//=====================================================================================
class PageItemInventory final : public Page
{
public:
	PageItemInventory( Inventory & a_Inventory, const char * a_InventoryName ) 
		: Page( a_InventoryName )
		, m_InventoryReference( a_Inventory )
	{}

protected:

	void OnEnterPage() override;
	void OnExitPage() override;
	void OnTick( float a_DeltaTime ) override;
	void SelectItem( int32_t a_Index );
	inline int32_t GetSelectedItem() const
	{
		return m_SelectedItemIndex;
	}

private:

	void UpdateItemsDisplay( uint32_t a_OffsetIntoInventory );
	Inventory & m_InventoryReference;
	int32_t m_SelectedItemIndex;

	const static uint32_t NUM_ITEMS_PER_PAGE = 3;

	ViewSprite m_BackgroundSprite = ViewSprite( "Background", this );
	ViewText m_ItemDescription = ViewText( "ItemDescription", this );

	ViewText m_ItemNames[ NUM_ITEMS_PER_PAGE ] = { 
		ViewText( "ItemName1", this ), 
		ViewText( "ItemName2", this ), 
		ViewText( "ItemName3", this ), 
	};

	ViewSprite m_ItemIcons[ NUM_ITEMS_PER_PAGE ] = {
		ViewSprite( "ItemIcon1", this ), 
		ViewSprite( "ItemIcon2", this ), 
		ViewSprite( "ItemIcon3", this ), 
	};
};

#endif//PAGEITEMINVENTORY_H