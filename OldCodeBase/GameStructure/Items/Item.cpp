#include "Item.hpp"

//=====================================================================================
const int32_t Item::NO_ICON = -1;

//=====================================================================================
Item::Item( ItemType a_ItemType, uint32_t a_Identifier, CString a_Name, CString a_Description, int32_t a_Icon )
	: m_ItemType( a_ItemType )
	, m_Name( a_Name )
	, m_Description( a_Description )
	, m_Icon( a_Icon )
	, m_Identifier( a_Identifier )
{
}