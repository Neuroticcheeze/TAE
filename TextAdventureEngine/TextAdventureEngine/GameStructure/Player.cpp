#include "Player.hpp"

//=====================================================================================
const uint32_t Player::MAX_HEALTH = 10;
const uint32_t Player::MAX_NOURISH = 10;
const uint32_t Player::MAX_SLEEP = 10;
const uint64_t Player::MAX_MONEY = 9999999;

//=====================================================================================
Player::Player()
	: m_Health( MAX_HEALTH )
	, m_Nourishment( MAX_NOURISH )
	, m_Sleep( MAX_SLEEP )
	, m_Money( 0 )
	, m_ItemBag( 8, Item::ItemType::CLOTHING )
{}

//=====================================================================================
void Player::SetHealthRelative( int32_t a_Amount )
{
	a_Amount = Clamp( a_Amount, -( int32_t )( m_Health ), ( int32_t )( MAX_HEALTH - ( a_Amount + m_Health ) ) );
	m_Health += a_Amount;
}

//=====================================================================================
void Player::SetHealth( uint32_t a_Value )
{
	m_Health = Min( MAX_HEALTH, a_Value );
}

//=====================================================================================
uint32_t Player::GetHealth() const
{
	return m_Health;
}

//=====================================================================================
void Player::SetNourishmentRelative( int32_t a_Amount )
{
	a_Amount = Clamp( a_Amount, -( int32_t )( m_Nourishment ), ( int32_t )( MAX_NOURISH - ( a_Amount + m_Nourishment ) ) );
	m_Nourishment += a_Amount;
}

//=====================================================================================
void Player::SetNourishment( uint32_t a_Value )
{
	m_Nourishment = Min( MAX_NOURISH, a_Value );
}

//=====================================================================================
uint32_t Player::GetNourishment() const
{
	return m_Nourishment;
}

//=====================================================================================
void Player::SetSleepRelative( int32_t a_Amount )
{
	a_Amount = Clamp( a_Amount, -( int32_t )( m_Sleep ), ( int32_t )( MAX_SLEEP - ( a_Amount + m_Sleep ) ) );
	m_Sleep += a_Amount;
}

//=====================================================================================
void Player::SetSleep( uint32_t a_Value )
{
	m_Sleep = Min( MAX_SLEEP, a_Value );
}

//=====================================================================================
uint32_t Player::GetSleep() const
{
	return m_Sleep;
}

//=====================================================================================
void Player::SetMoneyRelative( int32_t a_Amount )
{
	a_Amount = Clamp( a_Amount, -( int32_t )( m_Money ), ( int32_t )( MAX_MONEY - ( a_Amount + m_Money ) ) );
	m_Money += a_Amount;
}

//=====================================================================================
void Player::SetMoney( uint64_t a_Value )
{
	m_Money = Min( MAX_MONEY, a_Value );
}

//=====================================================================================
uint64_t Player::GetMoney() const
{
	return m_Money;
}

//=====================================================================================
bool Player::SetAttire( AttireSlot a_Slot, const Item & a_AttirePiece )
{
	if ( ( a_AttirePiece.GetType() & Item::CLOTHING ) != 0 )
	{
		m_Attire[ ( int32_t )a_Slot ] = a_AttirePiece;
		return true;
	}

	return false;
}