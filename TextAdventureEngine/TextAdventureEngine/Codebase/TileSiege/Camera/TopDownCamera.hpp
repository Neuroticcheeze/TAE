#pragma once
#ifndef TOPDOWNCAMERA_H
#define TOPDOWNCAMERA_H

#include <NeuroEngine/Framework/Math/Spatial/Rect2.hpp>
#include <NeuroEngine/Framework/Input/InputManager.hpp>

//=====================================================================================
class TopDownCamera final
	: public InputManager::IKeyEventListener
	, public InputManager::IMouseEventListener
{
public:

	struct TransformDescriptor final
	{
		Rect2 m_SpacialData;
	};

public:

	TopDownCamera( const TransformDescriptor & a_InitialTransformDescriptor = TransformDescriptor{ Rect2::CreateFromMinMax( 0.0F, 5.0F ) } );

	TransformDescriptor & GetTransformDescriptor();
	const TransformDescriptor & GetTransformDescriptor() const;
	void ResetTransformDescriptor();
	void PushCameraTransformation();
	void PopCameraTransformation();
	void SetControllerEnabled( bool a_Flag );
	
	void OnKeyPressed( InputManager::Key a_Key ) override;
	void OnKeyReleased( InputManager::Key a_Key ) override;
	void OnMousePressed( InputManager::MouseButton a_MouseButton ) override;
	void OnMouseReleased( InputManager::MouseButton a_MouseButton ) override;

	void OnTick( float a_DeltaTime );

	Vector2 GetScreenToWorld( const Vector2 & a_ScreenCoord ) const;
	Vector2 GetWorldToScreen( const Vector2 & a_WorldCoord ) const;

private:

	const TransformDescriptor m_InitialTransformDescriptor;
	TransformDescriptor m_CurrentTransformDescriptor;
	bool m_ControllerEnabled = false;

	struct
	{
		Vector2 m_Move;
	} 
	m_ControllerState;
};

#endif//TOPDOWNCAMERA_H