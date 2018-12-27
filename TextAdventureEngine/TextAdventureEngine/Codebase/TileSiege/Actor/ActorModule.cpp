#include "ActorModule.hpp"

//=====================================================================================
// Registry of all ActorModule type classes, add to this to register a new ActorModule
#include "Modules/ActorModulePositional.hpp"
#include "Modules/ActorModuleRectilinearPhysicsAgent.hpp"
#include "Modules/ActorModulePlayerController.hpp"
//=====================================================================================
void IActorModule::Registry()
{
	REGISTER_ACTOR_MODULE( ActorModulePositional );
	REGISTER_ACTOR_MODULE( ActorModuleRectilinearPhysicsAgent );
	REGISTER_ACTOR_MODULE( ActorModulePlayerController );
}

//=====================================================================================
Dictionary< ActorModuleTypeId, Tuple2< Functor< IActorModule*() >, Functor< IActorModuleConfig*() > > > IActorModule::s_Factory;

//=====================================================================================
IActorModule * IActorModule::CreateActorModuleFromTypeId( ActorModuleTypeId a_TypeId )
{
	auto * factory = s_Factory[ a_TypeId ];
	if ( factory && factory->Value0 )
	{
		return factory->Value0();
	}

	return nullptr;
}

//=====================================================================================
IActorModuleConfig * IActorModule::CreateConfigFromTypeId( ActorModuleTypeId a_TypeId )
{
	auto * factory = s_Factory[ a_TypeId ];
	if ( factory && factory->Value1 )
	{
		return factory->Value1();
	}

	return nullptr;
}