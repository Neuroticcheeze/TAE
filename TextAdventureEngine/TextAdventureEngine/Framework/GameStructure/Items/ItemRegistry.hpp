#pragma once
#include <Framework/Template/Registry.hpp>
#include <Framework/Graphics/SpriteSheet.hpp>
#include "Item.hpp"

//=====================================================================================
struct Vector2;

//=====================================================================================
class ItemRegistry final : public Registry< ItemRegistry, Item, 1024 >
{
public:
	static const Item NULL_ITEM;

	void Init();

	void RegisterItem( const Item & a_Item );
	const Item & GetItemByID( uint32_t a_Id );
};