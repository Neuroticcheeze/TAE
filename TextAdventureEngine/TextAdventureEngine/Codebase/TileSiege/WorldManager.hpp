#pragma once
#ifndef WORLDMANAGER_H
#define WORLDMANAGER_H

#include <NeuroEngine/Framework/Containers/Set.hpp>
#include <NeuroEngine/Framework/Containers/Dictionary.hpp>
#include <NeuroEngine/Framework/Physics/RectilinearPhysicsSystem.hpp>
#include <NeuroEngine/Framework/Template/Manager.hpp>
#include <TileSiege/Camera/TopDownCamera.hpp>
#include <TileSiege/Actor/Actor.hpp>

//=====================================================================================
class WorldManager final
	: public TickableManager< WorldManager >
{
public:

	void Init() override;
	void InitPost() override;
	void Tick( float a_DeltaTime = 0.0F ) override;
	void Finalise() override;

	void RegisterActorTemplate( uint32_t a_TemplateName, const Array< IActorModuleConfig* > & a_Configs );
	void RegisterActorTemplateFromStream( DataStream & a_Reader );
	void DeregisterActorTemplate( uint32_t a_TemplateName );
	void WriteActorTemplateToStream( uint32_t a_TemplateName, DataStream & a_Writer ) const;

	WeakPointer< Actor > CreateActor( const Actor::Config & a_Config );
	WeakPointer< Actor > CreateActorFromStream( DataStream & a_Reader );
	void DestroyActor( const WeakPointer< Actor > & a_Actor );

	inline RectilinearPhysicsSystem * GetPhysSystem() const { return m_PhysSystem; }

private:

	TopDownCamera * m_Camera = nullptr;
	RectilinearPhysicsSystem * m_PhysSystem = nullptr;
	RectilinearPhysicsSystem::PhysObject m_StaticCollisions;

	Dictionary< uint32_t, Array< IActorModuleConfig* > > m_RegisteredActorTemplates;
	Set< Pointer< Actor > > m_LoadedActors, 
							m_LoadedTickingActors;
};

#endif//WORLDMANAGER_H