#pragma once
#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include <NeuroEngine/Framework/Physics/RectilinearPhysicsSystem.hpp>
#include <NeuroEngine/Framework/Containers/CString.hpp>
#include <NeuroEngine/Framework/Math/Spatial/Rect2.hpp>

//=====================================================================================
typedef uint32_t GameEntityTypeId;

#define REGISTER_GAME_ENTITY_TYPE( CLASS ) GameEntity::s_GameEntityFactory.Put( CLASS::TYPE_ID, CLASS::Create )
#define GAME_ENTITY_TYPE( CLASS, UUID )\
			public:\
			GameEntityTypeId GetTypeId() const override { return CLASS::TYPE_ID; }\
			static const GameEntityTypeId TYPE_ID = UUID;\
			static GameEntity * Create() { return new CLASS; }\
			private:\

//=====================================================================================
struct GameEntityConfiguration
{
	CString m_Name;
	Vector2 m_Location;
	Array< Rect2 > m_Collider;
	GameEntityTypeId m_TypeId;
};

//=====================================================================================
class GameEntity
{
	friend class WorldManager;

public:

	GameEntity() {}
	GameEntity( const GameEntity & a_Other ) = delete;

	GameEntity & operator=( const GameEntity & a_Other ) = delete;

	bool operator==( const GameEntity & a_Other ) const { return this == &a_Other; }
	bool operator!=( const GameEntity & a_Other ) const { return !( *this == a_Other ); }

	virtual void Tick( float a_DeltaTime ) {}
	virtual bool IsRequestingTicks() const { return false; }
	virtual GameEntityTypeId GetTypeId() const { return GameEntityTypeId(); }

	// Type registration for the GameEntity factory
	static void Registry();
	static GameEntity * CreateFromTypeId( GameEntityTypeId a_TypeId );
	static Dictionary< GameEntityTypeId, Functor< GameEntity*() > > s_GameEntityFactory;

	inline const RectilinearPhysicsSystem::PhysObject & GetPhysicsProxy() const { return m_PhysicsProxy; }

private:

	RectilinearPhysicsSystem::PhysObject m_PhysicsProxy;
	uint32_t m_HashedName;
};

#endif//GAMEENTITY_H