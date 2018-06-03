#pragma once
#ifndef ITEM_H
#define ITEM_H

#include <Framework/Containers/CString.hpp>

//=====================================================================================
class ItemGroup;
class Inventory;

//=====================================================================================
class Item
{
	friend class ItemGroup;

public:

	static const int32_t NO_ICON;

	ENUM( ItemType, uint8_t )
		UNDEFINED	= 0,
		GENERIC		= 1 << 0,
		WEAPON		= 1 << 1,
		POTION		= 1 << 2,
		FOOD		= 1 << 3,
		LITERATURE	= 1 << 4,
		CLOTHING	= 1 << 5,
		ALL			= GENERIC | WEAPON | POTION | FOOD | LITERATURE | CLOTHING
	END_ENUM_F( ItemType, uint8_t )

public:

	Item( ItemType a_ItemType = UNDEFINED, uint32_t a_Identifier = 0, CString a_Name = "", CString a_Description = "", int32_t a_Icon = NO_ICON );

	inline ItemType GetType() const
	{
		return m_ItemType;
	}

	inline bool HasIcon() const
	{
		return m_Icon != NO_ICON;
	}

	inline uint32_t GetIcon() const
	{
		return static_cast< uint32_t >( m_Icon );
	}

	inline const CString & GetName() const
	{
		return m_Name;
	}

	inline const CString & GetDescription() const
	{
		return m_Description;
	}

	inline uint32_t GetID() const
	{
		return m_Identifier;
	}

	bool operator==( const Item & a_Other ) const
	{
		return m_Identifier == a_Other.m_Identifier;
	}

	bool operator!=( const Item & a_Other ) const
	{
		return m_Identifier != a_Other.m_Identifier;
	}

protected:

	virtual CString OnUsed( ItemGroup & a_ItemGroup, Inventory & a_Inventory ) 
	{
		CString str = GetName();
		str.ToUpper();
		return CString().Format( "You can't use your %s at the moment.", str.Get() ); 
	};

private:

	CString m_Name;
	CString m_Description;
	int32_t	m_Icon;
	ItemType m_ItemType;
	uint32_t m_Identifier;
};

#endif//ITEM_H