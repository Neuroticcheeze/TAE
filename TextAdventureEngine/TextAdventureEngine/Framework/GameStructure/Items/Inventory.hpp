#pragma once
#ifndef INVENTORY_H
#define INVENTORY_H

#include <Framework/Containers/Array.hpp>
#include "Item.hpp"
#include "ItemGroup.hpp"

//=====================================================================================
class Inventory final
{
public:
	Inventory( uint32_t a_MaxItems, Item::ItemType a_ExcludedTypesFilter );

	void StoreItem( uint32_t a_ID );
	Item TakeItem( uint32_t a_ID );
	bool HoldsItem( uint32_t a_ID );

	inline Array< ItemGroup > & GetItems()
	{
		return m_ItemArray;
	}

	inline const Array< ItemGroup > & GetItems() const
	{
		return m_ItemArray;
	}

private:

	ItemGroup * GetItemGroup( uint32_t a_ID ) const;

	Array< ItemGroup > m_ItemArray;
	uint32_t m_MaxItems;
	Item::ItemType m_ExcludeTypes;
};

#endif//INVENTORY_H