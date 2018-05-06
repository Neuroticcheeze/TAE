#include "ItemGroup.hpp"
#include "Item.hpp"
#include "ItemRegistry.hpp"

//=====================================================================================
ItemGroup::ItemGroup( uint32_t a_ItemID, uint32_t a_Count )
	: m_ItemCount( a_Count )
	, m_ItemID( a_ItemID )
{
}

//=====================================================================================
ItemGroup::ItemGroup( const Item & a_Item, uint32_t a_Count )
	: m_ItemCount( a_Count )
	, m_ItemID( a_Item.GetID() )
{
}

//=====================================================================================
Item ItemGroup::GetItem() const
{
	return ItemRegistry::Instance().GetItemByID( m_ItemID );
}

//=====================================================================================
uint32_t ItemGroup::GetItemID() const
{
	return m_ItemID;
}

//=====================================================================================
uint32_t ItemGroup::GetCount() const
{
	return m_ItemCount;
}

//=====================================================================================
void ItemGroup::SetCount( uint32_t a_Count )
{
	m_ItemCount = a_Count;
}

//=====================================================================================
CString ItemGroup::Use( Inventory & a_Inventory )
{
	Item item = GetItem();
	if ( item != ItemRegistry::NULL_ITEM )
	{
		return item.OnUsed( *this, a_Inventory );
	}
}