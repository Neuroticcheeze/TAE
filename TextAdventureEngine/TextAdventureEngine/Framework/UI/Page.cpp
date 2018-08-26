#include "Page.hpp"
#include <Framework/Engine.hpp>

//=====================================================================================
Page::ViewPage::ViewPage( const char * a_Name, Page * a_ContainerPage ) 
	: View( a_Name, a_ContainerPage, nullptr, true )
{
	SetTint( Colour::WHITE );
	SetBordersFromSizeAndOffset( Engine::Instance().GetDisplaySize() );
}

//=====================================================================================
Page::Page( const char * a_Name )
	: m_Name( a_Name )
	, m_FocusedView( 0 )
{
	m_RootView = new ViewPage( a_Name, this );
	m_RootView->SetTint( Colour::WHITE );
}

//=====================================================================================
Page::~Page()
{
	delete m_RootView;
	m_RootView = nullptr;
}

//=====================================================================================
void Page::Tick( float a_DeltaTime )
{
	OnTick( a_DeltaTime );

	m_AllViews.Clear();
	
	if ( m_RootView )
	{
		m_RootView->AppendToFrameCacheList();
	}

	bool block = false;
	for ( int32_t i = m_AllViews.Count() - 1; i >= 0; --i )
	{
		if ( !block )
		{
			block = m_AllViews[ ( uint32_t )i ]->CheckInput( false );
		}

		else
		{
			m_AllViews[ ( uint32_t )i ]->CheckInput( true );
		}
	}
	
	if ( ASSERT( m_RootView, "Page \"%s\" has no root view!", m_Name.Get() ) )
	{
		m_RootView->Tick( a_DeltaTime );
	}

	OnTickPost( a_DeltaTime );
}