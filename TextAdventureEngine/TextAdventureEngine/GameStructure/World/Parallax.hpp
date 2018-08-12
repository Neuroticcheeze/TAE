#pragma once
#ifndef PARALLAX_H
#define PARALLAX_H

#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/Set.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
class Parallax final
{
public:
	class Layer;
	
	Parallax( const Array< Layer > & a_Layers = {} );

	void DrawComposite( float a_DeltaTime );

	SETGET_V( ForeScale, float, m_ParallaxCamera.m_ForeScale );
	SETGET_V( BackScale, float, m_ParallaxCamera.m_BackScale );
	SETGET_V( CameraViewArea, Vector2, m_ParallaxCamera.m_CameraViewArea );
	SETGET_V_POSTOPONCHANGESET( CameraOffset, Vector2, m_ParallaxCamera.m_CameraOffset, RecalculateCameraOffset() );

	const Layer & operator[]( int32_t a_Index ) const;
	Layer & operator[]( int32_t a_Index );

private:

	void RecalculateCameraOffset();

	struct
	{
		Vector2 m_CameraViewArea;
		Vector2 m_CameraOffset;
		float m_ForeScale;
		float m_BackScale;

	} m_ParallaxCamera;

	Array< Layer > m_LayerArray;
};

//=====================================================================================
class Parallax::Layer final
{
	friend class Parallax;

public:

	class IListener;

	struct PerspectiveModifier final
	{
		float m_Upscale = 1.0F;
		float m_Movability = 1.0F;
	};

	inline void SetPerspective( const PerspectiveModifier & a_Perspective)  { m_PerspectiveModifier = { Max( 1.0F, a_Perspective.m_Upscale ), Min( 1.0F, a_Perspective.m_Movability ) }; }
	inline const PerspectiveModifier & GetPerspective() const { return m_PerspectiveModifier; }

	SETGETC( Distance, float );
	SETGETC( Texture, GraphicsManager::Texture );

	void RegisterListener( IListener & a_Listener );
	void DeregisterListener( IListener & a_Listener );

private:

	GraphicsManager::Texture m_Texture = nullptr;
	float m_Distance = 0.0F;
	PerspectiveModifier m_PerspectiveModifier;
	Set< IListener * > m_Listeners;
};

//=====================================================================================
class Parallax::Layer::IListener abstract
{
	friend class Parallax;

protected:

	virtual void OnPreDraw( Parallax::Layer & a_Layer ) abstract;
	virtual void OnPostDraw( Parallax::Layer & a_Layer ) abstract;
};

#endif//PARALLAX_H