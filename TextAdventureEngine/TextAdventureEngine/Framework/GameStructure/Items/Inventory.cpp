#include "Inventory.hpp"
#include "ItemRegistry.hpp"
#include <Framework/Utils/BitField.hpp>

//=====================================================================================
Inventory::Inventory( uint32_t a_MaxItems, Item::ItemType a_ExcludedTypesFilter )
	: m_MaxItems( a_MaxItems )
	, m_ExcludeTypes( a_ExcludedTypesFilter )
{
}

//=====================================================================================
void Inventory::StoreItem( uint32_t a_ID )
{
	const Item & item = ItemRegistry::Instance().GetItemByID( a_ID );
	
	if ( !FlagsHas( m_ExcludeTypes, item.GetType() ) )
	{
		if ( HoldsItem( a_ID ) )
		{
			auto ig = GetItemGroup( a_ID );
			ig->SetCount( ig->GetCount() + 1 );
		}

		else
		{
			m_ItemArray.Append( ItemGroup( a_ID ) );
		}
	}
}

//=====================================================================================
Item Inventory::TakeItem( uint32_t a_ID )
{
	ItemGroup * ig = GetItemGroup( a_ID );
	if ( ig )
	{
		ig->SetCount( ig->GetCount() - 1 );

		Item item = ig->GetItem();

		if ( ig->GetCount() == 0 )
		{
			m_ItemArray.Remove( ( const ItemGroup * )ig );
		}

		return item;
	}

	return ItemRegistry::NULL_ITEM;
}

//=====================================================================================
bool Inventory::HoldsItem( uint32_t a_ID )
{
	return GetItemGroup( a_ID ) != nullptr;
}

ItemGroup * Inventory::GetItemGroup( uint32_t a_ID ) const
{
	return ( ItemGroup * )m_ItemArray.Find< uint32_t >( []( uint32_t id, const ItemGroup & ig )
	{ 
		return id == ig.GetItemID() && ig.GetCount() > 0;
	}
	, a_ID );
}