#include "WorldManager.hpp"
#include <TileSiege/Actor/Actor.hpp>
#include "GameEntity/GameEntity.hpp"
#include "GameEntity/EntityPlayer.hpp"
#include <NeuroEngine/Framework/Graphics/GraphicsManager.hpp>
#include <NeuroEngine/Framework/Utils/Hash.hpp>
#include <NeuroEngine/Framework/Math/Spatial/Rect2.hpp>
#include <NeuroEngine/Framework/File/FileManager.hpp>
#include <NeuroEngine/Framework/File/FileStream.hpp>
#include <NeuroEngine/Framework/Engine.hpp>

#include "Actor/Modules/ActorModulePositional.hpp"
#include "Actor/Modules/ActorModuleRectilinearPhysicsAgent.hpp"
#include "Actor/Modules/ActorModulePlayerController.hpp"

//=====================================================================================
void WorldManager::Init()
{
	//Create the camera.
	TopDownCamera::TransformDescriptor initialCameraPlacement;
	initialCameraPlacement.m_SpacialData = Rect2::CreateFromPosSize( 0.0F, 12.0F );
	m_Camera = new TopDownCamera( initialCameraPlacement );
	m_Camera->SetControllerEnabled( true );

	// Create the physics system.
	m_PhysSystem = new RectilinearPhysicsSystem;
	
	IActorModule::Registry();
}

//=====================================================================================
void WorldManager::InitPost()
{
	// Create the world static collision data.
	RectilinearPhysicsSystem::RectArray walls;
	walls.Append( { { 0, 1 }, { 0, 1 } } );
	walls.Append( { { 2, 0 }, { 2, 5 } } );
	walls.Append( { { 1, 3 }, { 8, 4 } } );
	walls.Append( { { 10, 2 }, { 10, 3 } } );
	walls.Append( { { 14, 2 }, { 14, 5 } } );
	walls.Append( { { 11, 2 }, { 13, 2 } } );
	walls.Append( { { 11, 5 }, { 13, 5 } } );
	m_StaticCollisions = m_PhysSystem->CreatePhysObject( walls, PosInt32() );
	
	auto cfg1 = IActorModule::CreateConfigFromTypeId( ActorModulePositional::TYPE_ID );
	auto cfg2 = IActorModule::CreateConfigFromTypeId( ActorModuleRectilinearPhysicsAgent::TYPE_ID );
	auto cfg3 = IActorModule::CreateConfigFromTypeId( ActorModulePlayerController::TYPE_ID );
	((ActorModuleRectilinearPhysicsAgent::ConfigType*)cfg2)->m_MovementSpeed = 2.0F;
	((ActorModulePlayerController::ConfigType*)cfg3)->f = 1997.0F;
	RegisterActorTemplate( CRC32( "ActTpl_Player" ), 
		{
			cfg1,
			cfg2,
			cfg3,
		}
	);
	
	if ( FileStream stream0 = FileManager::Instance().Open( "res/player.atp", FileManager::BINARY | FileManager::WRITE ) )
	{
		WriteActorTemplateToStream( CRC32( "ActTpl_Player" ), stream0 );
		FileManager::Instance().Close( stream0 );
	}
	
	/*if ( FileStream stream0 = FileManager::Instance().Open( "res/player.atp", FileManager::BINARY | FileManager::READ ) )
	{
		RegisterActorTemplateFromStream( stream0 );
		FileManager::Instance().Close( stream0 );
	}
*/
	// Create the player entity
	Actor::Config pActorConfig;
	pActorConfig.m_Name = "Player";
	pActorConfig.m_ActorTemplate = CRC32( "ActTpl_Player" );
	WeakPointer< Actor > pActor = CreateActor( pActorConfig );
}

#include <NeuroEngine/Framework/Graphics/GraphicsManager.hpp>
#include <NeuroEngine/Framework/Input/InputManager.hpp>

//=====================================================================================
void WorldManager::Tick( float a_DeltaTime )
{
	// Update the camera.
	m_Camera->OnTick( a_DeltaTime );

	// Push the camera transformation onto the stack,
	// we're about to start using it.
	m_Camera->PushCameraTransformation();

	// Draw simple shapes for the static collisions
	if ( m_StaticCollisions )
	{
		const auto & collisionShapes = m_StaticCollisions->GetCollisionShapes();
		auto it = collisionShapes.First();
		while ( it != collisionShapes.End() )
		{
			Vector2 min( it->Min.XPos, it->Min.YPos );
			Vector2 max( it->Max.XPos, it->Max.YPos );

			GraphicsManager::Instance().SetColour( Colour::GRAY, GraphicsManager::COL_SECONDARY );
			GraphicsManager::Instance().SetColour( Colour::GRAY * 0.4F, GraphicsManager::COL_PRIMARY );
			GraphicsManager::Instance().GfxDrawQuad( min, ( max - min + Vector2::ONE ), 0.05F );

			++it;
		}
	}

	// Tick through all ticking entities
	auto it = m_LoadedTickingActors.First();
	while ( it != m_LoadedTickingActors.End() )
	{
		( *it )->Tick( a_DeltaTime );
		++it;
	}

	static float f3 = 0.0F;

	if ( InputManager::Instance().GetKeyState(InputManager::KEYCODE_K) == InputManager::ButtonState::DOWN)
		f3 += a_DeltaTime * PI2 / 20.0F;
	if (InputManager::Instance().GetKeyState(InputManager::KEYCODE_L) == InputManager::ButtonState::DOWN)
		f3 -= a_DeltaTime * PI2 / 20.0F;

	Vector2 mouse = m_Camera->GetScreenToWorld( InputManager::Instance().GetMousePosition() );

	Vector2 dir = Vector2( Cos( f3 ), Sin( f3 ) );
	
	auto f = WorldManager::Instance().GetPhysSystem()->RayCast( mouse, dir );
	Vector2 end = mouse + dir * f.m_Distance;
	GraphicsManager::Instance().GfxDrawLine( mouse, end, 0.03F );
	Vector2 d2 = f.m_Normal.Reflect(dir);
	GraphicsManager::Instance().GfxDrawLine( end, end + d2 * 100.0F, 0.01F );


	// All rendering with the camera will be finished by this point, 
	// so pop the camera's transformation off the stack
	m_Camera->PopCameraTransformation();

	return;

	Vector2 size( 5, 4 );
	int arr[] = 
	{
		0,1,1,1,2,
		5,3,4,6,7,
		5,7,5,6,7,
		10,12,10,11,12,
	};
	
	GraphicsManager::TextureAtlas terrainAtlas = GraphicsManager::Instance().GetAtlas( CRC32( "TERRAIN" ) );
	for (int x = 0; x < (int)(size.x); ++x)
	for (int y = 0; y < (int)(size.y); ++y)
	{
		terrainAtlas->DrawSprite(arr[(int)(x+y*size.x)], Vector2(x, size.y - y) * 100.0F, Vector2::ONE * 100.0F);
	}
}

//=====================================================================================
void WorldManager::Finalise()
{
	Free( m_PhysSystem );
}

//=====================================================================================
void WorldManager::RegisterActorTemplate( uint32_t a_TemplateName, const Array< IActorModuleConfig* > & a_Configs )
{
	DeregisterActorTemplate( a_TemplateName );
	m_RegisteredActorTemplates.Put( a_TemplateName, a_Configs );
}

//=====================================================================================
void WorldManager::RegisterActorTemplateFromStream( DataStream & a_Reader )
{
	uint32_t actorTemplateName = 0;
	a_Reader.Read( actorTemplateName );

	ASSERT_WARN( !m_RegisteredActorTemplates.Contains( actorTemplateName ), "Registering actor template %u from stream, but it has already been registered! This will overwrite it.", actorTemplateName );

	uint32_t num = 0;
	a_Reader.Read( num );

	Array< IActorModuleConfig* > configs;

	for ( int k = 0; k < num; ++k )
	{
		ActorModuleTypeId typeId = 0;
		a_Reader.Read( typeId );

		IActorModuleConfig * newCfg = IActorModule::CreateConfigFromTypeId( typeId );
		ASSERT( newCfg, "Failed to read actor module config from bytestream! Corrupt data suspected..." );
		
		a_Reader.ReadS( *newCfg );
		
		configs.Append( newCfg );
	}

	RegisterActorTemplate( actorTemplateName, configs );
}

//=====================================================================================
void WorldManager::DeregisterActorTemplate( uint32_t a_TemplateName )
{
	auto * actp = m_RegisteredActorTemplates[ a_TemplateName ];
	if ( actp )
	{
		auto it = actp->First();
		while ( it != actp->End() )
		{
			delete *it;
			++it;
		}
		m_RegisteredActorTemplates.Remove( a_TemplateName );
	}
}

//=====================================================================================
void WorldManager::WriteActorTemplateToStream( uint32_t a_TemplateName, DataStream & a_Writer ) const
{
	auto * actp = m_RegisteredActorTemplates[ a_TemplateName ];
	if ( !actp )
	{
		ASSERT_WARN( false, "Failed to write actor template by identifier=%u to stream, it does not exist!", a_TemplateName );
		return;
	}

	a_Writer.Write( a_TemplateName );
	a_Writer.Write( actp->Count() );

	auto configIt = actp->First();
	while ( configIt != actp->End() )
	{
		a_Writer.Write( ( *configIt )->GetTypeId() );
		a_Writer.WriteS( **configIt );
		++configIt;
	}
}

//=====================================================================================
WeakPointer< Actor > WorldManager::CreateActor( const Actor::Config & a_Config )
{
	PROFILE;

	/*
	// Calculate location
	PosInt32 position = PosInt32{ static_cast< int32_t >( a_Config.m_Location.x ), static_cast< int32_t >( a_Config.m_Location.y ) };

	// Calculate collision shapes
	RectilinearPhysicsSystem::RectArray collisionShapes;
	auto it = a_Config.m_Collider.First();
	while ( it != a_Config.m_Collider.End() )
	{
		RectInt32 pRectInt32;
		pRectInt32.Min = PosInt32{ static_cast< int32_t >( it->GetMin().x ), static_cast< int32_t >( it->GetMin().y ) };
		pRectInt32.Max = PosInt32{ static_cast< int32_t >( it->GetMax().x ) - 1, static_cast< int32_t >( it->GetMax().y ) - 1 };
		collisionShapes.Append( pRectInt32 );
		++it;
	}
	*/

	auto * actp = m_RegisteredActorTemplates[ a_Config.m_ActorTemplate ];

	if ( !actp )
	{
		ASSERT_WARN( false, "Failed to create actor %s, actor template by identifier=%u does not exist!", a_Config.m_Name.Get(), a_Config.m_ActorTemplate );
		return WeakPointer< Actor >();
	}

	// Create the entity and register it with the physics system & world.
	Actor * actor = new Actor;
	actor->m_Name = a_Config.m_Name;
	
	auto configIt = actp->First();
	while ( configIt != actp->End() )
	{
		actor->AddModule( ( *configIt )->GetTypeId(), *configIt );
		++configIt;
	}

	auto ptr = Pointer< Actor >( actor );

	if ( actor->IsRequestingTicks() )
	{
		m_LoadedTickingActors.Insert( ptr );
	}
	else
	{
		m_LoadedActors.Insert( ptr );
	}
	
	return WeakPointer< Actor >( ptr );
}

//=====================================================================================
void WorldManager::DestroyActor( const WeakPointer< Actor > & a_Actor )
{
	if ( ASSERT_WARN( a_Actor, 
		 "Invalid actor! Cannot de-register with WorldManager..." ) )
	{
		PROFILE;

		auto ptrOf = Pointer< Actor >( a_Actor.Ptr() );

		if ( !m_LoadedActors.Remove( ptrOf ) )
		{
			// Assert if not even this list contains the entity, that would mean we have a pointer of the entity elsewhere (Not allowed)
			ASSERT_WARN( m_LoadedTickingActors.Remove( ptrOf ), "Entity appears to exist outside of WorldManager.. is this intentional? It will not be referenceable through WorldManager." );
		}
	}
}

//=====================================================================================
WeakPointer< Actor > WorldManager::CreateActorFromStream( DataStream & a_Reader )
{
	return WeakPointer< Actor >();
}