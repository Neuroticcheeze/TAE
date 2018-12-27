#include "EntityPlayer.hpp"
#include <TileSiege/WorldManager.hpp>
#include <NeuroEngine/Framework/Graphics/GraphicsManager.hpp>
#include <NeuroEngine/Framework/Utils/Hash.hpp>
#include "EntityBullet.hpp"

#include "../Actor/Actor.hpp"
#include "../Actor/Modules/ActorModuleGridMovable.hpp"

//=====================================================================================
EntityPlayer::EntityPlayer()
{
	GameInputManager & gipm = GameInputManager::Instance();
	gipm.RegisterListener( CRC32( "MOVE_X" ), this );
	gipm.RegisterListener( CRC32( "MOVE_Y" ), this );
	gipm.RegisterListener( CRC32( "JUMP" ), this );
}

//=====================================================================================
EntityPlayer::~EntityPlayer()
{
	GameInputManager & gipm = GameInputManager::Instance();
	gipm.DeregisterListener( CRC32( "MOVE_X" ), this );
	gipm.DeregisterListener( CRC32( "MOVE_Y" ), this );
	gipm.DeregisterListener( CRC32( "JUMP" ), this );
}

//=====================================================================================
void EntityPlayer::Tick( float a_DeltaTime )
{
	const PosInt32 & physPos = GetPhysicsProxy()->GetPosition();

	GraphicsManager & gfx = GraphicsManager::Instance();
	gfx.SetColour( Colour::GRAY.Lerp( Colour::WHITE, 0.3F ).WithSaturation( 0.4F ), GraphicsManager::COL_PRIMARY );
	gfx.SetColour( Colour::GRAY.Lerp( Colour::WHITE, 0.6F ).WithSaturation( 0.4F ) * 0.5F, GraphicsManager::COL_SECONDARY );
	gfx.GfxDrawQuad( Vector2( physPos.XPos, physPos.YPos ) + m_DeficitToProxy, Vector2::ONE, 0.05F );

	if ( m_DeficitToProxy.LengthSqr() > 0.0001F )
	{
		static const float SPEED = 5.0F;
		m_DeficitToProxy -= m_DeficitToProxy.Normalised() * Clamp( a_DeltaTime * SPEED );
	}
	else
	{
		m_DeficitToProxy = Vector2::ZERO;
	}
}

//=====================================================================================
void EntityPlayer::OnInputBegin( uint32_t a_InputEvent, float a_Value )
{
	if ( m_DeficitToProxy != Vector2::ZERO ) return;

	int valI = Round( a_Value );

	if ( a_InputEvent == CRC32( "MOVE_X" ) && valI == -1 ) { if ( WorldManager::Instance().GetPhysSystem()->MovePhysObject( GetPhysicsProxy(), RectilinearPhysicsSystem::DIR_LEFT ) ) m_DeficitToProxy = Vector2::RIGHT; }
	if ( a_InputEvent == CRC32( "MOVE_X" ) && valI == +1 ) { if ( WorldManager::Instance().GetPhysSystem()->MovePhysObject( GetPhysicsProxy(), RectilinearPhysicsSystem::DIR_RIGHT ) ) m_DeficitToProxy = -Vector2::RIGHT; }
	if ( a_InputEvent == CRC32( "MOVE_Y" ) && valI == +1 ) { if ( WorldManager::Instance().GetPhysSystem()->MovePhysObject( GetPhysicsProxy(), RectilinearPhysicsSystem::DIR_UP ) ) m_DeficitToProxy = -Vector2::UP; }
	if ( a_InputEvent == CRC32( "MOVE_Y" ) && valI == -1 ) { if ( WorldManager::Instance().GetPhysSystem()->MovePhysObject( GetPhysicsProxy(), RectilinearPhysicsSystem::DIR_DOWN ) ) m_DeficitToProxy = Vector2::UP; }
	if ( a_InputEvent == CRC32( "JUMP" ) )
	{
		Actor::Config bulletConfig;
		bulletConfig.m_Name = "Bullet";
		WorldManager::Instance().CreateActor( bulletConfig );
	}
}

//=====================================================================================
void EntityPlayer::OnInputUpdate( uint32_t a_InputEvent, float a_Value )
{
	OnInputBegin( a_InputEvent, a_Value );
}

//=====================================================================================
void EntityPlayer::OnInputEnd( uint32_t a_InputEvent, float a_Value )
{
}