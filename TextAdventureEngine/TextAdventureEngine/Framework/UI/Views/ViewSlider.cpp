#include "ViewSlider.hpp"
#include <Framework/Graphics/Graphics.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewSlider::ViewSlider( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Enabled( false )
	, m_Value( 0.0F )
	, m_Range( 0.0F, 1.0F )
	, m_Background( "Background", a_ContainerPage, this )
	, m_Slider( "Slider", a_ContainerPage, this )
	, m_InitialTick( true )
{
	SetIsInteractible( true );
	SetEnabled( true );
	SetStep( 0.0F ); // will choose smallest possible incremental float value as our step
	SetSliderSize( 0.1F );

	GetBackground().SetBorder( View::Alignment::LEFT, 0.0F );
	GetBackground().SetBorder( View::Alignment::RIGHT, 0.0F );
	GetBackground().SetBorder( View::Alignment::TOP, 0.0F );
	GetBackground().SetBorder( View::Alignment::BOTTOM, 0.0F );
	GetBackground().SetZOrder( 1 );
	GetBackground().SetTint( Colour::WHITE );
	AddChild( &GetBackground() );

	GetSlider().SetTint( Colour::WHITE );
	AddChild( &GetSlider() );

	UpdateSlider();
}

//=====================================================================================
void ViewSlider::OnTick( float a_DeltaTime )
{
	if ( m_InitialTick )
	{
		SetValue( GetValue() );
		m_InitialTick = false;
	}

	if ( GetEnabled() && IsBeingGrabbed() )
	{
		Vector2 mpos = InputManager::Instance().GetMousePosition();
		float value = m_Value;

		if ( GetSize().x >= GetSize().y )
		{
			value = ReMap( mpos.x, GetPosition().x + GetSize().x * ( m_SliderSize * 0.5F ), GetPosition().x + GetSize().x * ( 1.0F - ( m_SliderSize * 0.5F ) ), m_Range.x, m_Range.y );
		}

		else
		{
			value = ReMap( mpos.y, GetPosition().y + GetSize().y * ( m_SliderSize * 0.5F ), GetPosition().y + GetSize().y * ( 1.0F - ( m_SliderSize * 0.5F ) ), m_Range.x, m_Range.y );
		}

		SetValue( value );
	}
}

//=====================================================================================
void ViewSlider::OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton )
{
}

//=====================================================================================
void ViewSlider::RequestInitialEvents( IActionListener * a_ActionListener )
{
	a_ActionListener->OnSliderValueChanged( *this, GetValue(), GetValue() );
}

//=====================================================================================
void ViewSlider::SetEnabled( bool a_Enabled )
{
	if ( a_Enabled != m_Enabled )
	{
		if ( m_Enabled = a_Enabled ) // when enabled
		{
			SetTint( Colour::WHITE );
		}

		else // when disabled
		{
			SetTint( Colour::GRAY.Lerp( Colour::RED, 0.5F ) );
		}
	}
}

//=====================================================================================
void ViewSlider::SetValue( float a_Value )
{
	// step the specified value
	if ( m_SnapSlider )
	{
		float step = Min( m_Range.y - m_Range.x, GetStep() );
		const float f1 = 1.0F / step;
		a_Value = Round( a_Value * f1 );
		a_Value /= f1;
	}

	// final clamp
	a_Value = Clamp( a_Value, m_Range.x, m_Range.y );
	
	if ( !Approximate( m_Value, a_Value ) )
	{
		float prev = m_Value;
		m_Value = a_Value;
		UpdateSlider();
		GetParent()->OnSliderValueChanged( *this, prev, a_Value );
	}
}

//=====================================================================================
float ViewSlider::GetValueNormalised() const
{
	return ReMap( GetValue(), m_Range.x, m_Range.y, 0.0F, 1.0F );
}

//=====================================================================================
float ViewSlider::GetValue() const
{
	float value = m_Value;

	// step the specified value
	if ( !m_SnapSlider )
	{
		float step = Min( m_Range.y - m_Range.x, GetStep() );
		const float f1 = 1.0F / step;
		value = Round( value * f1 );
		value /= f1;
	}

	return value;
}

//=====================================================================================
void ViewSlider::SetRange( const Vector2 & a_Range )
{
	ASSERT( m_Range.x < m_Range.y, "WARNING: ViewSlider[%s] has an invalid value range: %s", GetName().Get(), a_Range.ToString().Get() );

	m_Range.x = Min( a_Range.x, a_Range.y );
	m_Range.y = Max( a_Range.x, a_Range.y );
}

//=====================================================================================
void ViewSlider::UpdateSlider()
{
	if ( GetSize().x >= GetSize().y )
	{
		const float f = ReMap( m_Value, m_Range.x, m_Range.y, 0.0F, 1.0F );
		GetSlider().SetBorder( View::Alignment::LEFT, ( m_SliderSize * 0.5F ) + f * ( 1.0F - m_SliderSize ) - ( m_SliderSize * 0.5F ) );
		GetSlider().SetBorder( View::Alignment::RIGHT, ( 1.0F - ( f * ( 1.0F - m_SliderSize ) + ( m_SliderSize * 0.5F ) ) ) - ( m_SliderSize * 0.5F ) );
		GetSlider().SetBorder( View::Alignment::TOP, 0.0F );
		GetSlider().SetBorder( View::Alignment::BOTTOM, 0.0F );
	}

	else
	{
		const float f = ReMap( m_Value, m_Range.x, m_Range.y, 0.0F, 1.0F );
		GetSlider().SetBorder( View::Alignment::BOTTOM, ( m_SliderSize * 0.5F ) + f * ( 1.0F - m_SliderSize ) - ( m_SliderSize * 0.5F ) );
		GetSlider().SetBorder( View::Alignment::TOP, ( 1.0F - ( f * ( 1.0F - m_SliderSize ) + ( m_SliderSize * 0.5F ) ) ) - ( m_SliderSize * 0.5F ) );
		GetSlider().SetBorder( View::Alignment::LEFT, 0.0F );
		GetSlider().SetBorder( View::Alignment::RIGHT, 0.0F );
	}
}

//=====================================================================================
void ViewSlider::OnRectangleChanged( const Vector2 & a_TrueOffset, const Vector2 & a_TrueSize )
{
	UpdateSlider();
}

//=====================================================================================
void ViewSlider::SetStep( float a_Step, bool a_SnapSliderToStep )
{
	m_Step = Max( a_Step, FLT_MIN );
	m_SnapSlider = a_SnapSliderToStep;
}