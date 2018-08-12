#pragma once
#ifndef VIEWSLIDER_H
#define VIEWSLIDER_H

#include <Framework/UI/View.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/Graphics/SpriteSheet.hpp>
#include <Framework/Graphics/NineSpriteSheet.hpp>

//=====================================================================================
class ViewSlider final : public View
{
public:

	ViewSlider( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );

	void SetEnabled( bool a_Enabled );

	inline bool GetEnabled() const
	{
		return m_Enabled;
	}

	inline ViewSprite & GetBackground()
	{
		return m_Background;
	}

	inline ViewSprite & GetSlider()
	{
		return m_Slider;
	}

	void SetRange( const Vector2 & a_Range );

	inline const Vector2 & GetRange() const
	{
		return m_Range;
	}

	void SetStep( float a_Step, bool a_SnapSliderToStep = true );

	inline float GetStep() const
	{
		return m_Step;
	}

	void SetValue( float a_Value );
	float GetValue() const;
	float GetValueNormalised() const;

	void SetSliderSize( float a_RelativeScale )
	{
		m_SliderSize = Clamp( a_RelativeScale, 0.01F, 0.99F );
	}

protected:

	void OnTick( float a_DeltaTime );
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton );
	void RequestInitialEvents( IActionListener * a_ActionListener );
	void OnRectangleChanged( const Vector2 & a_TrueOffset, const Vector2 & a_TrueSize );

private:

	void UpdateSlider();

	float	   m_SliderSize;
	float	   m_Step;
	bool	   m_SnapSlider; // to step
	bool	   m_Enabled;
	Vector2	   m_Range;
	float	   m_Value;
	ViewSprite m_Background;
	ViewSprite m_Slider;

	bool m_InitialTick;
};

#endif//VIEWSLIDER_H