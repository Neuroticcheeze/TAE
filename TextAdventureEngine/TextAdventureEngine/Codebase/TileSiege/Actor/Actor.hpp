#pragma once
#ifndef ACTOR_H
#define ACTOR_H

#include <NeuroEngine/Framework/Utils/MemoryExt.hpp>
#include <NeuroEngine/Framework/Containers/Array.hpp>
#include <NeuroEngine/Framework/Containers/CString.hpp>
#include <TileSiege/Actor/ActorModule.hpp>

//=====================================================================================
class Actor final : public Serialisable< Actor >
{
	friend class WorldManager;

public:

	struct Config final
	{
		CString m_Name;
		uint32_t m_ActorTemplate;
	};

public:

	Actor() {}
	Actor( const Actor & a_Other ) = delete;
	Actor & operator=( const Actor & a_Other ) = delete;

	bool operator==( const Actor & a_Other ) const { return this == &a_Other; }
	bool operator!=( const Actor & a_Other ) const { return !( *this == a_Other ); }

	void Write( DataStream & a_Writer ) const override;
	void Read( DataStream & a_Reader ) override;

	void Tick( float a_DeltaTime );
	bool IsRequestingTicks() const { return m_TickedModules.Count() > 0; }

	void AddModule( ActorModuleTypeId a_TypeId, IActorModuleConfig * a_Config = nullptr );
	
	WeakPointer< IActorModule > GetModuleById( ActorModuleTypeId a_TypeId ) const;

	template< typename T >
	TWeakPointer< T, IActorModule > GetModule()
	{
		return TWeakPointer< T, IActorModule >( GetModuleById( T::TYPE_ID ) );
	}

private:

	CString m_Name;
	Array< Pointer< IActorModule > > m_Modules;
	Array< Pointer< IActorModule > > m_TickedModules;
};

#endif//ACTOR_H