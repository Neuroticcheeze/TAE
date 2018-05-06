#include "ItemRegistry.hpp"
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
const Item ItemRegistry::NULL_ITEM( Item::UNDEFINED, 0, "", "", Item::NO_ICON );

//=====================================================================================
void ItemRegistry::Init()
{
	RegisterItem( Item( Item::GENERIC, WSID( "broken_lantern" ), "Broken Kensington Lantern", "An older Kensington-made lantern. Once a lantern of exceptional brightness, used in the early lighthouses - it vapourised a special mixture of common and rare fuels to produce unrivaled luminosity. Alas, this one in particular has a broken wickplace and fuel-feeder... However, there is still some precious Kensington-grade fuel left in the resovoir - a valuable comodity still.", 6 ) );
	RegisterItem( Item( Item::FOOD, WSID( "apple" ), "Pink Lady Apple", "A delicious crisp apple of the 'Pink Lady' variety.", 7 ) );
	RegisterItem( Item( Item::FOOD, WSID( "item2" ), "Green Lady Apple", "A delicious crisp apple of the 'Green Lady' variety.", 6 ) );
	RegisterItem( Item( Item::FOOD, WSID( "item3" ), "Red Lady Apple", "A delicious crisp apple of the 'Red Lady' variety.", 1 ) );
	RegisterItem( Item( Item::FOOD, WSID( "item4" ), "Purple Lady Apple", "A delicious crisp apple of the 'Purple Lady' variety.", 7 ) );
}

//=====================================================================================
void ItemRegistry::RegisterItem( const Item & a_Item )
{
	this->Register( a_Item.GetID(), a_Item );
}

//=====================================================================================
const Item & ItemRegistry::GetItemByID( uint32_t a_Id )
{
	const Item * itemSet = this->GetManifest().GetValues();
	const uint32_t num = this->GetManifest().Count();

	for ( uint32_t k = 0; k < num; ++k )
	{
		const Item & item = *( itemSet + k );

		if ( item.GetID() == a_Id )
		{
			return item;
		}
	}

	return NULL_ITEM;
}