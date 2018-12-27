#include "ActorModulePlayerController.hpp"
#include <TileSiege/WorldManager.hpp>
#include "ActorModuleRectilinearPhysicsAgent.hpp"

//=====================================================================================
ACTOR_MODULE_CONFIG_POST_ACTOR_MODULE( ActorModulePlayerController );

//=====================================================================================
void ActorModulePlayerControllerConfig::Write( DataStream & a_Writer ) const
{
	a_Writer.Write( f );
}

//=====================================================================================
void ActorModulePlayerControllerConfig::Read( DataStream & a_Reader )
{
	a_Reader.Read( f );
}

//=====================================================================================
ActorModulePlayerController::ActorModulePlayerController()
{
	GameInputManager & gipm = GameInputManager::Instance();
	gipm.RegisterListener( CRC32( "MOVE_X" ), this );
	gipm.RegisterListener( CRC32( "MOVE_Y" ), this );
	gipm.RegisterListener( CRC32( "JUMP" ), this );
}

//=====================================================================================
ActorModulePlayerController::~ActorModulePlayerController()
{
	GameInputManager & gipm = GameInputManager::Instance();
	gipm.DeregisterListener( CRC32( "MOVE_X" ), this );
	gipm.DeregisterListener( CRC32( "MOVE_Y" ), this );
	gipm.DeregisterListener( CRC32( "JUMP" ), this );
}


//=====================================================================================
void ActorModulePlayerController::OnInputBegin( uint32_t a_InputEvent, float a_Value )
{
	int valI = Round( a_Value );

	ActorModuleRectilinearPhysicsAgent * module = GetActor()->GetModule< ActorModuleRectilinearPhysicsAgent >().Ptr();

	if ( a_InputEvent == CRC32( "MOVE_X" ) && valI == -1 ) { module->Move( RectilinearPhysicsSystem::DIR_LEFT ); }
	if ( a_InputEvent == CRC32( "MOVE_X" ) && valI == +1 ) { module->Move( RectilinearPhysicsSystem::DIR_RIGHT ); }
	if ( a_InputEvent == CRC32( "MOVE_Y" ) && valI == +1 ) { module->Move( RectilinearPhysicsSystem::DIR_UP ); }
	if ( a_InputEvent == CRC32( "MOVE_Y" ) && valI == -1 ) { module->Move( RectilinearPhysicsSystem::DIR_DOWN ); }
	if ( a_InputEvent == CRC32( "JUMP" ) )
	{
		Actor::Config bulletConfig;
		bulletConfig.m_Name = "Bullet";
		WorldManager::Instance().CreateActor( bulletConfig );
	}
}

//=====================================================================================
void ActorModulePlayerController::OnInputUpdate( uint32_t a_InputEvent, float a_Value )
{
	OnInputBegin( a_InputEvent, a_Value );
}

//=====================================================================================
void ActorModulePlayerController::OnInputEnd( uint32_t a_InputEvent, float a_Value )
{
}