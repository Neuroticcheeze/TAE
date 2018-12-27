#include "TopDownCamera.hpp"
#include <NeuroEngine/Framework/Graphics/GraphicsManager.hpp>
#include <NeuroEngine/Framework/Engine.hpp>

//=====================================================================================
TopDownCamera::TopDownCamera( const TransformDescriptor & a_InitialTransformDescriptor )
	: m_InitialTransformDescriptor( a_InitialTransformDescriptor )
	, m_CurrentTransformDescriptor( a_InitialTransformDescriptor )
{
}

//=====================================================================================
const TopDownCamera::TransformDescriptor & TopDownCamera::GetTransformDescriptor() const
{
	return m_CurrentTransformDescriptor;
}

//=====================================================================================
TopDownCamera::TransformDescriptor & TopDownCamera::GetTransformDescriptor()
{
	return m_CurrentTransformDescriptor;
}

//=====================================================================================
void TopDownCamera::ResetTransformDescriptor()
{
	m_CurrentTransformDescriptor = m_InitialTransformDescriptor;
}

//=====================================================================================
void TopDownCamera::PushCameraTransformation()
{
	auto & gfx = GraphicsManager::Instance();

	const TransformDescriptor & tfDesc = GetTransformDescriptor();

	gfx.TfPush();

	const Vector2 displaySize = Engine::Instance().GetDisplaySize();
	const float maxDim = Max( displaySize.x, displaySize.y );
	gfx.TfScale( Vector2::ONE / tfDesc.m_SpacialData.GetSize() * maxDim );
	
	gfx.TfTranslate( -tfDesc.m_SpacialData.GetMin() );
}

//=====================================================================================
void TopDownCamera::PopCameraTransformation()
{
	auto & gfx = GraphicsManager::Instance();

	gfx.TfPop();
}

//=====================================================================================
void TopDownCamera::SetControllerEnabled( bool a_Flag )
{
	InputManager & inpm = InputManager::Instance();

	if ( m_ControllerEnabled != a_Flag )
	{
		m_ControllerEnabled = a_Flag;

		// We're enabling controller input
		if ( m_ControllerEnabled )
		{
			inpm.AttachListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );
			inpm.AttachListener( InputManager::InputKeyEvent::ON_KEY_RELEASED, this );
			inpm.AttachListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
			inpm.AttachListener( InputManager::InputMouseEvent::ON_MOUSE_RELEASED, this );
		}

		// We're disabling controller input
		else
		{
			inpm.DetatchListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );
			inpm.DetatchListener( InputManager::InputKeyEvent::ON_KEY_RELEASED, this );
			inpm.DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
			inpm.DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_RELEASED, this );
			m_ControllerState = decltype( m_ControllerState )();// Reset controller state data
		}
	}
}

//=====================================================================================
void TopDownCamera::OnKeyPressed( InputManager::Key a_Key )
{
	switch ( a_Key )
	{
	case InputManager::KEYCODE_RIGHT:
		m_ControllerState.m_Move.x += 1.0F;
		break;
	case InputManager::KEYCODE_LEFT:
		m_ControllerState.m_Move.x -= 1.0F;
		break;
	case InputManager::KEYCODE_UP:
		m_ControllerState.m_Move.y += 1.0F;
		break;
	case InputManager::KEYCODE_DOWN:
		m_ControllerState.m_Move.y -= 1.0F;
		break;
	}

	m_ControllerState.m_Move.x = Clamp( m_ControllerState.m_Move.x, -1.0F, 1.0F );
	m_ControllerState.m_Move.y = Clamp( m_ControllerState.m_Move.y, -1.0F, 1.0F );
}

//=====================================================================================
void TopDownCamera::OnKeyReleased( InputManager::Key a_Key )
{
	switch ( a_Key )
	{
	case InputManager::KEYCODE_RIGHT:
		m_ControllerState.m_Move.x -= 1.0F;
		break;
	case InputManager::KEYCODE_LEFT:
		m_ControllerState.m_Move.x += 1.0F;
		break;
	case InputManager::KEYCODE_UP:
		m_ControllerState.m_Move.y -= 1.0F;
		break;
	case InputManager::KEYCODE_DOWN:
		m_ControllerState.m_Move.y += 1.0F;
		break;
	}

	m_ControllerState.m_Move.x = Clamp( m_ControllerState.m_Move.x, -1.0F, 1.0F );
	m_ControllerState.m_Move.y = Clamp( m_ControllerState.m_Move.y, -1.0F, 1.0F );
}

//=====================================================================================
void TopDownCamera::OnMousePressed( InputManager::MouseButton a_MouseButton )
{
}

//=====================================================================================
void TopDownCamera::OnMouseReleased( InputManager::MouseButton a_MouseButton )
{
}

//=====================================================================================
void TopDownCamera::OnTick( float a_DeltaTime )
{
	const float cMovementSpeed = 3.0F;
	GetTransformDescriptor().m_SpacialData.Move( m_ControllerState.m_Move * a_DeltaTime * cMovementSpeed );
}

//=====================================================================================
Vector2 TopDownCamera::GetScreenToWorld( const Vector2 & a_ScreenCoord ) const
{
	const Vector2 displaySize = Engine::Instance().GetDisplaySize();
	const float maxDim = Max( displaySize.x, displaySize.y );
	const Vector2 scale = ( Vector2::ONE / GetTransformDescriptor().m_SpacialData.GetSize() ) * maxDim;
	const Vector2 trans = -GetTransformDescriptor().m_SpacialData.GetMin();

	Vector2 result = a_ScreenCoord;
	result /= scale;
	result -= trans;
	return result;
}

//=====================================================================================
Vector2 TopDownCamera::GetWorldToScreen( const Vector2 & a_WorldCoord ) const
{
	const Vector2 displaySize = Engine::Instance().GetDisplaySize();
	const float maxDim = Max( displaySize.x, displaySize.y );
	const Vector2 scale = ( Vector2::ONE / GetTransformDescriptor().m_SpacialData.GetSize() ) * maxDim;
	const Vector2 trans = -GetTransformDescriptor().m_SpacialData.GetMin();

	Vector2 result = a_WorldCoord;
	result += trans;
	result *= scale;
	return result;
}