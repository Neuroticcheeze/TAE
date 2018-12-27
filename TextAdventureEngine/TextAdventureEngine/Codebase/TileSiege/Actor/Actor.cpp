#include "Actor.hpp"

//=====================================================================================
void Actor::Tick( float a_DeltaTime )
{
	auto it = m_TickedModules.First();
	while ( it != m_TickedModules.End() )
	{
		( *it )->Tick( a_DeltaTime );
		++it;
	}
}

//=====================================================================================
void Actor::AddModule( ActorModuleTypeId a_TypeId, IActorModuleConfig * a_Config )
{
	if ( m_TickedModules.Find< ActorModuleTypeId >( []( ActorModuleTypeId typeId, const Pointer< IActorModule > & a_Elem )
			{
				return a_Elem->GetTypeId() == typeId;
			}, a_TypeId ) 
		!= nullptr )
	{
		return;
	}

	if ( m_Modules.Find< ActorModuleTypeId >( []( ActorModuleTypeId typeId, const Pointer< IActorModule > & a_Elem )
			{
				return a_Elem->GetTypeId() == typeId;
			}, a_TypeId ) 
		!= nullptr )
	{
		return;
	}

	IActorModule * actorModule = IActorModule::CreateActorModuleFromTypeId( a_TypeId );
	actorModule->m_Actor = this;
	actorModule->m_Config = a_Config;

	if ( actorModule->IsRequestingTicks() )
	{
		m_TickedModules.Append( Pointer< IActorModule >( actorModule ) );
	}
	else
	{
		m_Modules.Append( Pointer< IActorModule >( actorModule ) );
	}
}

//=====================================================================================
WeakPointer< IActorModule > Actor::GetModuleById( ActorModuleTypeId a_TypeId ) const
{
	if ( Pointer< IActorModule > * module = m_TickedModules.Find< ActorModuleTypeId >( []( ActorModuleTypeId typeId, const Pointer< IActorModule > & a_Elem )
			{
				return a_Elem->GetTypeId() == typeId;
			}, a_TypeId ) )
	{
		return WeakPointer< IActorModule >( *module );
	}

	if ( Pointer< IActorModule > * module = m_Modules.Find< ActorModuleTypeId >( []( ActorModuleTypeId typeId, const Pointer< IActorModule > & a_Elem )
			{
				return a_Elem->GetTypeId() == typeId;
			}, a_TypeId ) )
	{
		return WeakPointer< IActorModule >( *module );
	}

	return nullptr;
}

//=====================================================================================
void Actor::Write( DataStream & a_Writer ) const
{
	a_Writer.WriteS( m_Name );

	a_Writer.Write( m_Modules.Count() );
	auto it = m_Modules.First();
	while ( it != m_Modules.End() )
	{
		a_Writer.Write( ( *it )->GetTypeId() );
		a_Writer.WriteS( *it->Ptr() );
		++it;
	}

	a_Writer.Write( m_TickedModules.Count() );
	it = m_TickedModules.First();
	while ( it != m_TickedModules.End() )
	{
		a_Writer.Write( ( *it )->GetTypeId() );
		a_Writer.WriteS( *it->Ptr() );
		++it;
	}
}

//=====================================================================================
void Actor::Read( DataStream & a_Reader )
{
	a_Reader.ReadS( m_Name );
}
