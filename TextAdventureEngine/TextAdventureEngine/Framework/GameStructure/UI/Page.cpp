#include "Page.hpp"
#include <Framework/Config/Config.hpp>

//=====================================================================================
Page::ViewPage::ViewPage( const char * a_Name, Page * a_ContainerPage ) : View( a_Name, a_ContainerPage )
{
	UseExplicitSize( false );
	SetSize( Vector2( Config::WINDOW_X, Config::WINDOW_Y ) );
}

//=====================================================================================
Page::Page( const char * a_Name, const Pointer< View > & a_RootView  )
	: m_Name( a_Name )
	, m_RootView( a_RootView )
{
}

//=====================================================================================
Page::Page( const char * a_Name )
	: m_Name( a_Name )
{
	m_RootView = Pointer< View >( ViewPage( a_Name, this ) );
	m_RootView->SetTint( Colour::WHITE );
}

//=====================================================================================
void Page::Tick( float a_DeltaTime )
{
	OnTick( a_DeltaTime );

	if ( ASSERT( m_RootView, "Page \"%s\" has no root view!", m_Name.Get() ) )
	{
		m_RootView->Tick( a_DeltaTime );
	}
}