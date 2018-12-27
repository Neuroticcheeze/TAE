#include "ItemRegistry.hpp"
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
const Item ItemRegistry::NULL_ITEM( Item::UNDEFINED, 0, "", "", Item::NO_ICON );

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