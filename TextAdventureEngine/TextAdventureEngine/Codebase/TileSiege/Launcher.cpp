#include <NeuroEngine/Main.hpp>
#include <NeuroEngine/Framework/ApplicationContext.hpp>
#include <NeuroEngine/Framework/Utils/MemoryOps.hpp>
#include <TileSiege/WorldManager.hpp>
#include <TileSiege/Input/GameInputManager.hpp>

//=====================================================================================
class TileSiegeCtx : public IApplicationContext
{
	void Initialise() override;
	void Tick( float a_DeltaTime ) override;
	void Finalise() override;
	const char * GetName() const
	{
		return "Tile Siege";
	}
} TileSiegeCtx;

//=====================================================================================
void TileSiegeCtx::Initialise()
{

	INIT_MANAGER( GameInputManager, "res/input.xml" );
	INIT_MANAGER( WorldManager );

	INITPOST_MANAGER( GameInputManager );
	INITPOST_MANAGER( WorldManager );
}

//=====================================================================================
void TileSiegeCtx::Tick( float a_DeltaTime )
{
	TICK_MANAGER( GameInputManager, a_DeltaTime );
	TICK_MANAGER( WorldManager, a_DeltaTime );
}

//=====================================================================================
void TileSiegeCtx::Finalise()
{
	FINIT_MANAGER( WorldManager );
	FINIT_MANAGER( GameInputManager );
}

//=====================================================================================
// Entry point to entire program.
//=====================================================================================
int main( int a_Args, char *a_Argv[] )
{
	return Launch( &TileSiegeCtx, a_Args, a_Argv );
}