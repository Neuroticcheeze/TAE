#include "ActorModuleRectilinearPhysicsAgent.hpp"
#include <TileSiege/WorldManager.hpp>
#include <TileSiege/Actor/Modules/ActorModulePositional.hpp>

//=====================================================================================
ACTOR_MODULE_CONFIG_POST_ACTOR_MODULE( ActorModuleRectilinearPhysicsAgent );

//=====================================================================================
void ActorModuleRectilinearPhysicsAgentConfig::Write( DataStream & a_Writer ) const
{
	a_Writer.Write( m_MovementSpeed );
}

//=====================================================================================
void ActorModuleRectilinearPhysicsAgentConfig::Read( DataStream & a_Reader )
{
	a_Reader.Read( m_MovementSpeed );
}

//=====================================================================================
ActorModuleRectilinearPhysicsAgent::ActorModuleRectilinearPhysicsAgent()
{
	PosInt32 pos;
	RectilinearPhysicsSystem::RectArray collShapes;
	collShapes.Append( RectInt32{ 0, 0, 0, 0 } );

	m_PhysicsProxy = WorldManager::Instance().GetPhysSystem()->CreatePhysObject( collShapes, pos );
	m_DeficitToProxy = Vector2::ZERO;
}

//=====================================================================================
ActorModuleRectilinearPhysicsAgent::~ActorModuleRectilinearPhysicsAgent()
{
	WorldManager::Instance().GetPhysSystem()->DestroyPhysObject( m_PhysicsProxy );
}

//=====================================================================================
void ActorModuleRectilinearPhysicsAgent::Tick( float a_DeltaTime )
{
	if ( m_DeficitToProxy.LengthSqr() > 0.01F )
	{
		m_DeficitToProxy -= m_DeficitToProxy.Normalised() * Clamp( a_DeltaTime * CONFIG->GetMovementSpeed() );

		// Apply to positional module
		Vector2 physPos = { static_cast< float >( m_PhysicsProxy->GetPosition().XPos ), static_cast< float >( m_PhysicsProxy->GetPosition().YPos ) };
		this->GetActor()->GetModule< ActorModulePositional >()->SetPosition( physPos + m_DeficitToProxy );
	}
	else if ( m_DeficitToProxy != Vector2::ZERO )
	{
		m_DeficitToProxy = Vector2::ZERO;

		// Apply to positional module
		Vector2 physPos = { static_cast< float >( m_PhysicsProxy->GetPosition().XPos ), static_cast< float >( m_PhysicsProxy->GetPosition().YPos ) };
		this->GetActor()->GetModule< ActorModulePositional >()->SetPosition( physPos + m_DeficitToProxy );
	}
}

//=====================================================================================
bool ActorModuleRectilinearPhysicsAgent::Move( RectilinearPhysicsSystem::Direction a_Direction )
{
	if ( m_DeficitToProxy != Vector2::ZERO ) return false;

	if ( a_Direction == RectilinearPhysicsSystem::DIR_LEFT ) { if ( WorldManager::Instance().GetPhysSystem()->MovePhysObject( m_PhysicsProxy, RectilinearPhysicsSystem::DIR_LEFT ) ) { m_DeficitToProxy = Vector2::RIGHT; return true; } }
	if ( a_Direction == RectilinearPhysicsSystem::DIR_RIGHT ) { if ( WorldManager::Instance().GetPhysSystem()->MovePhysObject( m_PhysicsProxy, RectilinearPhysicsSystem::DIR_RIGHT ) ) { m_DeficitToProxy = -Vector2::RIGHT; return true; } }
	if ( a_Direction == RectilinearPhysicsSystem::DIR_UP ) { if ( WorldManager::Instance().GetPhysSystem()->MovePhysObject( m_PhysicsProxy, RectilinearPhysicsSystem::DIR_UP ) ) { m_DeficitToProxy = -Vector2::UP; return true; } }
	if ( a_Direction == RectilinearPhysicsSystem::DIR_DOWN ) { if ( WorldManager::Instance().GetPhysSystem()->MovePhysObject( m_PhysicsProxy, RectilinearPhysicsSystem::DIR_DOWN ) ) { m_DeficitToProxy = Vector2::UP; return true; } }
	return false;
}