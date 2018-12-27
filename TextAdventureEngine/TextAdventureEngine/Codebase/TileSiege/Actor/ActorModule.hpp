#pragma once
#ifndef ACTORMODULE_H
#define ACTORMODULE_H

#include <NeuroEngine/Framework/Containers/Dictionary.hpp>
#include <NeuroEngine/Framework/Utils/Hash.hpp>
#include <NeuroEngine/Framework/Utils/TypeTraits.hpp>
#include <NeuroEngine/Framework/Template/Serialisable.hpp>
#include <NeuroEngine/Framework/Utils/Tuple.hpp>
#include <NeuroEngine/Framework/Containers/DataStream.hpp>

//=====================================================================================
typedef uint32_t ActorModuleTypeId;
class Actor;

//=====================================================================================
#define ACTOR_MODULE( CLASS )\
	public:\
	ActorModuleTypeId GetTypeId() const override { return CLASS::TYPE_ID; }\
	static const ActorModuleTypeId TYPE_ID = CRC32( #CLASS );\
	using ConfigType = CLASS ## Config;\
	private:

//=====================================================================================
#define REGISTER_ACTOR_MODULE( CLASS )\
	IActorModule::s_Factory.Put( CLASS::TYPE_ID, Tuple2< Functor< IActorModule*() >, Functor< IActorModuleConfig*() > >( [](){ return new CLASS; }, [](){ return new CLASS ## Config; } ) );\
	STATIC_ASSERT( IS_BASE_OF( IActorModule, CLASS ), #CLASS " must be derived from IActorModule" );\
	STATIC_ASSERT( IS_BASE_OF( IActorModuleConfig, CLASS ## Config ), #CLASS "Config must be derived from IActorModuleConfig" );

//=====================================================================================
#define ACTOR_MODULE_CONFIG_DECL( CLASS )\
	struct CLASS ## Config : public IActorModuleConfig

//=====================================================================================
#define ACTOR_MODULE_CONFIG( CLASS )\
	ActorModuleTypeId GetTypeId() const override;\
	static const ActorModuleTypeId TYPE_ID;

//=====================================================================================
#define ACTOR_MODULE_CONFIG_POST_ACTOR_MODULE( CLASS )\
	ActorModuleTypeId CLASS ## Config::GetTypeId() const { return CLASS::TYPE_ID; }\
	const ActorModuleTypeId CLASS ## Config::TYPE_ID = CLASS::TYPE_ID;

//=====================================================================================
#define CONFIG\
	( reinterpret_cast< const ConfigType* >( GetConfig() ) )

//=====================================================================================
struct IActorModuleConfig : Serialisable< IActorModuleConfig >
{
	virtual ActorModuleTypeId GetTypeId() const { return ActorModuleTypeId(); }
};

//=====================================================================================
class IActorModule : public Serialisable< IActorModule >
{
	friend class Actor;

public:

	IActorModule() {}
	IActorModule( const IActorModule & a_Other ) = delete;

	IActorModule & operator=( const IActorModule & a_Other ) = delete;

	bool operator==( const IActorModule & a_Other ) const { return this == &a_Other; }
	bool operator!=( const IActorModule & a_Other ) const { return !( *this == a_Other ); }

	virtual void Write( DataStream & a_Writer ) const override {}
	virtual void Read( DataStream & a_Reader ) override {}

	virtual void Tick( float a_DeltaTime ) {}
	virtual bool IsRequestingTicks() const { return false; }
	
	virtual ActorModuleTypeId GetTypeId() const { return ActorModuleTypeId(); }

	// Type registration for the ActorModule factory
	static void Registry();
	static IActorModule * CreateActorModuleFromTypeId( ActorModuleTypeId a_TypeId );
	static IActorModuleConfig * CreateConfigFromTypeId( ActorModuleTypeId a_TypeId );
	static Dictionary< ActorModuleTypeId, Tuple2< Functor< IActorModule*() >, Functor< IActorModuleConfig*() > > > s_Factory;

	inline Actor * GetActor() { return m_Actor; }
	inline const Actor * GetActor() const { return m_Actor; }

	inline const IActorModuleConfig * GetConfig() const { return m_Config; }

private:

	IActorModuleConfig * m_Config;
	Actor * m_Actor = nullptr;
};

#endif//ACTORMODULE_H