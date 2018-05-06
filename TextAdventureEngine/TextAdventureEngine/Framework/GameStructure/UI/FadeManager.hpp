#pragma once
#ifndef FADEMANAGER_H
#define FADEMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Queue.hpp>
#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/Dictionary.hpp>

//=====================================================================================
class FadeManager final : public TickableManager< FadeManager >
{
public:

	class IListener abstract
	{
		friend class FadeManager;

	protected:

		virtual void OnFadeBarrier( uint32_t a_FadeBarrierID ) abstract;
	};

	ENUM( Layer, uint8_t )
		POST_ENV,
		POST_UI,
	END_ENUM_C( Layer, uint8_t );

	void Init();
	void Finalise();
	void Tick( float a_DeltaTime );

	void PushFadeData( Layer a_Layer, Colour a_Value, float a_FadeTime = 0.0F, float a_HoldTime = 0.0F );
	void PushFadeBarrier( Layer a_Layer, uint32_t a_FadeBarrierID );
	void ListenForFadeBarrier( Layer a_Layer, uint32_t a_FadeBarrierID, IListener & a_Listener );
	void DrawLayer( Layer a_Layer ) const;

private:

	struct FadeLayerData
	{
		FadeLayerData( const Colour & a_Source = Colour::INVISIBLE, const Colour & a_Target = Colour::BLACK, float a_FadeTime = 1.0F, float a_HoldTime = 0.0F );

		Colour m_Source;
		Colour m_Target;
		float m_FadeTime;
		float m_HoldTime;
		float m_CurrentTime;
		uint32_t m_FadeBarrierID;

		bool Tick( float a_DeltaTime, Colour & a_Result );
	};

	Queue< FadeLayerData > m_LayerDatas[ LayerCount ];
	Colour m_LayerResults[ LayerCount ];
	Dictionary< uint32_t, Array< IListener * > > m_FadeBarrierListeners[ LayerCount ];
};

#endif//FADEMANAGER_H