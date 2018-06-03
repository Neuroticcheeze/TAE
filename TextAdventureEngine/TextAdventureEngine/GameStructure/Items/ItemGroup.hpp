#pragma once
#ifndef ITEMGROUP_H
#define ITEMGROUP_H

//=====================================================================================
class Item;
class Inventory;
class CString;

//=====================================================================================
class ItemGroup final
{
public:
	ItemGroup() : ItemGroup( 0, 0 ) {}
	ItemGroup( uint32_t a_ItemID, uint32_t a_Count = 1 );
	ItemGroup( const Item & a_Item, uint32_t a_Count = 1 );

	Item GetItem() const;
	uint32_t GetItemID() const;
	uint32_t GetCount() const;
	void SetCount( uint32_t a_Count );
	CString Use( Inventory & a_Inventory );
private:

	uint32_t m_ItemID;
	uint32_t m_ItemCount;
};

#endif//ITEMGROUP_H