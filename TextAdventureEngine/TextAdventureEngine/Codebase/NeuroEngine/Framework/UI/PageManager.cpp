#include "PageManager.hpp"
#include "Page.hpp"
#include <Framework/Input/InputManager.hpp>

//=====================================================================================
void PageManager::Init()
{
	while ( m_ActivePages.Count() > 0 )
	{
		m_ActivePages.Pop();
	}

	while ( m_PushPopEvents.Count() > 0 )
	{
		m_PushPopEvents.Pop();
	}

	m_QueuedPops = 0;

	for ( uint32_t k = 0; k < FadeManager::LayerCount; ++k )
	{
		SetFadeLayerPostOrder( ( FadeManager::Layer )k, INT_MAX >> 1 );
	}
}

//=====================================================================================
void PageManager::Finalise()
{
	//Just call Init() because it's only clearing stuff out
	Init();
}

//=====================================================================================
void PageManager::Tick( float a_DeltaTime )
{
	while ( m_PushPopEvents.Count() > 0 )
	{
		PushPop * ppEvent = m_PushPopEvents.Peek();
		
		if ( ppEvent->Value0 == POP )
		{
			--m_QueuedPops;
			( *m_ActivePages.Peek() )->OnExitPage();
			m_ActivePages.Pop();
		}
		else
		{
			m_ActivePages.Push( ppEvent->Value1 );
			( *m_ActivePages.Peek() )->OnEnterPage();
		}

		m_PushPopEvents.Pop();
	}

	Page ** it = m_ActivePages.Data();
	const uint32_t num = m_ActivePages.Count();
	uint32_t topBlocker = 0;

	if ( num > 0 && it )
	{
		// Work out which is the highest blocking page in the stack
		for ( int32_t i = num - 1; i >= 0; --i )
		{
			const Page & pageAt = **( it + i );
			if ( pageAt.IsBlocking() )
			{
				topBlocker = i;
				break;
			}
		}

		bool prevIgnoreInput = InputManager::Instance().IsIgnoringInput();
		InputManager::Instance().IgnoreInput( true );

		// Now, update
		for ( uint32_t i = topBlocker; i < num; ++i )
		{
			if ( i == m_ActivePages.Count() - 1 )
			{
				InputManager::Instance().IgnoreInput( false );
			}

			Page & pageAt = **( it + i );

			pageAt.m_IsBeingBlocked = false;
			pageAt.Tick( a_DeltaTime );

			for ( uint32_t k = 0; k < FadeManager::LayerCount; ++k )
			{
				int32_t i2 = m_FadeLayerPosts[ k ];

				if ( static_cast< uint32_t >( i2 ) >= i )
				{
					FadeManager::Instance().DrawLayer( ( FadeManager::Layer )k );
				}
			}
		}
		for ( uint32_t i = 0; i < topBlocker; ++i )
		{
			Page & pageAt = **( it + i );
			pageAt.m_IsBeingBlocked = true;
		}

		InputManager::Instance().IgnoreInput( prevIgnoreInput );
	}
}

//=====================================================================================
void PageManager::Push( Page * a_Page )
{
	m_PushPopEvents.Push( PushPop( PUSH, a_Page ) );
}

//=====================================================================================
void PageManager::Pop()
{
	if ( ( m_ActivePages.Count() - m_QueuedPops ) > 0 )
	{
		++m_QueuedPops;
		m_PushPopEvents.Push( PushPop( POP, nullptr ) );
	}
}

//=====================================================================================
uint32_t PageManager::NumActivePages() const
{
	return m_ActivePages.Count();
}

//=====================================================================================
Page * PageManager::Peek() const
{
	auto peek = m_ActivePages.Peek();
	if ( peek != nullptr )
	{
		return *peek;
	}

	return nullptr;
}

//=====================================================================================
void PageManager::SetFadeLayerPostOrder( FadeManager::Layer a_Layer, int32_t a_Order )
{
	m_FadeLayerPosts[ a_Layer ] = a_Order;
}