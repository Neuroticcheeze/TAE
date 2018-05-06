#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include <Framework/Template/Singleton.hpp>
#include <Framework/GameStructure/Items/Inventory.hpp>

//=====================================================================================
class Player final : public Singleton< Player >
{
public:

	ENUMCLASS( AttireSlot, uint8_t )
		HEAD,
		TORSO,
		LEGS,
		FEET,
		HANDS,
	END_ENUMCLASS( AttireSlot, uint8_t )

	static const uint32_t MAX_HEALTH;
	static const uint32_t MAX_NOURISH;
	static const uint32_t MAX_SLEEP;
	static const uint64_t MAX_MONEY;

	Player();

	void SetHealthRelative( int32_t a_Amount );
	void SetHealth( uint32_t a_Value );
	uint32_t GetHealth() const;

	void SetNourishmentRelative( int32_t a_Amount );
	void SetNourishment( uint32_t a_Value );
	uint32_t GetNourishment() const;

	void SetSleepRelative( int32_t a_Amount );
	void SetSleep( uint32_t a_Value );
	uint32_t GetSleep() const;

	void SetMoneyRelative( int32_t a_Amount );
	void SetMoney( uint64_t a_Value );
	uint64_t GetMoney() const;


	inline Inventory & GetBag()
	{
		return m_ItemBag;
	}

	inline const Inventory & GetBag() const
	{
		return m_ItemBag;
	}

	inline Item & GetAttire( AttireSlot a_Slot )
	{
		return m_Attire[ ( int32_t )a_Slot ];
	}

	inline const Item & GetAttire( AttireSlot a_Slot ) const
	{
		return m_Attire[ ( int32_t )a_Slot ];
	}

	bool SetAttire( AttireSlot a_Slot, const Item & a_AttirePiece );

private:

	uint32_t m_Health;
	uint32_t m_Nourishment;
	uint32_t m_Sleep;
	uint64_t m_Money;
	Inventory m_ItemBag;
	Item m_Attire[ AttireSlotCount ];
};

#endif//PLAYER_H