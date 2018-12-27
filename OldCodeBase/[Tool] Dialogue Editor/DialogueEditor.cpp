#include "DialogueEditor.hpp"

#include <Framework/Engine.hpp>
#include <Framework/Audio/AudioManager.hpp>
#include <Framework/File/Save/SaveManager.hpp>
#include <Framework/File/FileManager.hpp>
#include <Framework/Data/Xml/XmlManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Input/CommandLineManager.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/Thread/ThreadManager.hpp>
#include <Framework/Data/String/StringManager.hpp>
#include <Framework/UI/FadeManager.hpp>
#include <Framework/UI/PageManager.hpp>
#include <GameStructure/Items/ItemRegistry.hpp>
#include <GameStructure/World/Nature/TimeFlow.hpp>
#include <GameStructure/World/WorldManager.hpp>
#include <GameStructure/PlayerSaveHandler.hpp>
#include <Framework/Utils/Hash.hpp>
#include <[Tool] Dialogue Editor/UI/Pages/PageDialogueEditor.hpp>
#include <[Tool] Dialogue Editor/DialogueToolGlobals.hpp>

PageDialogueEditor * g_PageDialogueEditor = nullptr;

//=====================================================================================
#ifdef APP_IS_DIALOGUE_EDITOR
IApplicationContext * g_TheContext = new DialogueEditor;
#endif

//=====================================================================================
DialogueEditor * DialogueEditor::Get()
{
	return static_cast< DialogueEditor* >( g_TheContext );
}
#include <Framework/AI/Path/NavFlowField.hpp>
#include <Framework/Physics/RectilinearPhysicsSystem.hpp>
#include <nmmintrin.h>// for SSE4.2
#include <immintrin.h> // for AVX 

NavFlowField * navFlowField = nullptr;
RectilinearPhysicsSystem * physSys = nullptr;
RectilinearPhysicsSystem::PhysObject myPhysProxy;
Array< CompressionManager::RectUInt32ValUInt8 > worldCollisionShapesRaw;

//=====================================================================================
void DialogueEditor::Initialise()
{
	//int a = rand() % 8;
	//int b = rand() % 8;
	//int min = Min( a, b );
	//b = Max( a, b );
	//a = min;
	//if ( ( b - a ) == 4 ) PRINT( "[%d <-> %d] OPPOSITES", a, b );

	__m128 A = _mm_setr_ps( 1.0F, 1.0F, 3.0F, 0.0F );
	__m128 B = _mm_setr_ps( 2.0F, 2.0F, 3.0F, 0.0F );
	__m128 C = _mm_mul_ps( A, B );
	__m128 s;
	s = _mm_hadd_ps(C, C);
	s = _mm_hadd_ps(s, s);
	float sum = s.m128_f32[0];

	const uint8_t W = 255;
	uint8_t costs[] = 
	{
		0,  W,  0,  0,  0,  0,  W,  0,  0,  0,
		0,  0,  0,  0,  0,	0,  0,  0,  0,  0,
		0,  W,  0,  0,  0,	0,  W,  0,  0,  0,
		0,  W,  0,  0,  0,	0,  W,  0,  0,  0,
		0,  0,  0,  0,  0,	0,  0,  0,  0,  0,
		0,  W,  0,  0,  W,	W,  W,  0,  0,  0,
		0,  W,  0,  0,  0,	0,  W,  0,  0,  0,
		0,  0,  0,  0,  0,	0,  0,  0,  0,  0,
		0,  W,  W,  W,  W,	0,  W,  0,  0,  0,
		0,  W,  0,  0,  0,	0,  W,  0,  0,  0,
		0,  0,  0,  0,  0,	0,  0,  0,  0,  0,
		0,  W,  0,  0,  0,	0,  W,  0,  W,  W,
		0,  W,  0,  0,  0,	0,  W,  0,  W,  W,
		0,  0,  0,  0,  0,	0,  0,  0,  W,  W,
		0,  W,  0,  0,  0,	0,  W,  0,  W,  W,
		0,  W,  0,  0,  0,	0,  W,  0,  W,  W,
		0,  0,  0,  0,  0,	0,  W,  W,  W,  W,
		0,  W,  W,  0,  W,	W,  W,  0,  0,  0,
		W,  W,  W,  0,  W,	W,  W,  0,  0,  0,
		W,  W,  0,  0,  0,	0,  W,  0,  0,  0,
		W,  W,  0,  0,  0,	0,  W,  W,  0,  0,
		W,  W,  0,  0,  0,	0,  W,  W,  0,  0,
		W,  W,  0,  0,  0,	0,  0,  0,  0,  0,
		W,  W,  0,  0,  0,	0,  W,  W,  W,  W,
	};
	worldCollisionShapesRaw = CompressionManager::Instance().CompressGridToRectsUInt8( costs, 10, 24, 254 );

	navFlowField = new NavFlowField;
	navFlowField->SetInputData( costs, 10, 24 );
	navFlowField->SetDestination( 2, 1 );
	navFlowField->Rebuild();

	physSys = new RectilinearPhysicsSystem;

	RectilinearPhysicsSystem::RectArray worldCollisionShapes;
	auto it = worldCollisionShapesRaw.First();
	auto end = worldCollisionShapesRaw.End();
	while ( it != end )
	{
		if ( it->Value == 255 )
		{
			RectInt32 rect;
			rect.Min.XPos = it->XPos;
			rect.Min.YPos = it->YPos;
			rect.Max.XPos = it->XPos + it->Width - 1;
			rect.Max.YPos = it->YPos + it->Height - 1;
			worldCollisionShapes.Append( rect );
		}
		++it;
	}
	physSys->CreatePhysObject( worldCollisionShapes, { 0, 0 } );
	myPhysProxy = physSys->CreatePhysObject( { { { 0, 0 }, { 0, 0 } } }, { 0, 0 } );

	DialogueToolGlobals::CreateBitmapFonts();
	g_PageDialogueEditor = new PageDialogueEditor();
	PageManager::Instance().Push( g_PageDialogueEditor );

}

//=====================================================================================
void DialogueEditor::Tick( float a_DeltaTime )
{
	navFlowField->Draw();

	if ( InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_UP ) == InputManager::ButtonState::PRESSED )
		physSys->MovePhysObject( myPhysProxy, RectilinearPhysicsSystem::DIR_UP );

	if ( InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_DOWN ) == InputManager::ButtonState::PRESSED )
		physSys->MovePhysObject( myPhysProxy, RectilinearPhysicsSystem::DIR_DOWN );

	if ( InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_LEFT ) == InputManager::ButtonState::PRESSED )
		physSys->MovePhysObject( myPhysProxy, RectilinearPhysicsSystem::DIR_LEFT );

	if ( InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_RIGHT ) == InputManager::ButtonState::PRESSED )
		physSys->MovePhysObject( myPhysProxy, RectilinearPhysicsSystem::DIR_RIGHT );

	auto & cshapes = myPhysProxy->GetCollisionShapes();
	auto it = cshapes.First();
	auto end = cshapes.End();
	while ( it != end )
	{
		auto rect = ( *it ) + myPhysProxy->GetPosition();

		Vector2 pos( rect.Min.XPos, rect.Min.YPos );
		Vector2 size = ( Vector2( rect.Max.XPos, rect.Max.YPos ) - pos + Vector2::ONE );

		GraphicsManager::Instance().TfPush();
		GraphicsManager::Instance().TfScale( 50 );
		GraphicsManager::Instance().GfxDrawQuad( pos, size );
		GraphicsManager::Instance().TfPop();

		++it;
	}
}

//=====================================================================================
void DialogueEditor::Finalise()
{
	PageManager::Instance().Pop();
	Free( g_PageDialogueEditor );
	DialogueToolGlobals::ReleaseBitmapFonts();
}