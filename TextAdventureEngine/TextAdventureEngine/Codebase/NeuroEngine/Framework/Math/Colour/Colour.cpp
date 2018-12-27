#include "Colour.hpp"
#include <Framework/Math/Math.hpp>
#include <Framework/Containers/CString.hpp>

const Colour Colour::RED( 1.0F, 0.0F, 0.0F );
const Colour Colour::ORANGE( 1.0F, 0.5F, 0.0F );
const Colour Colour::YELLOW( 1.0F, 1.0F, 0.0F );
const Colour Colour::LIME( 0.5F, 1.0F, 0.0F );
const Colour Colour::GREEN( 0.0F, 1.0F, 0.0F );
const Colour Colour::CYAN( 0.0F, 1.0F, 1.0F );
const Colour Colour::COBALT( 0.0F, 0.0F, 1.0F );
const Colour Colour::VIOLET( 0.8F, 0.0F, 1.0F );
const Colour Colour::MAGENTA( 1.0F, 0.0F, 1.0F );
const Colour Colour::BLACK( 0.0F, 0.0F, 0.0F );
const Colour Colour::GRAY( 0.5F, 0.5F, 0.5F );
const Colour Colour::WHITE( 1.0F, 1.0F, 1.0F );
const Colour Colour::INVISIBLE( 1.0F, 1.0F, 1.0F, 0.0F );
const Colour Colour::FLAT_NORMALMAP( 0.5F, 0.5F, 1.0F );

//=====================================================================================
Colour::Colour()
	: r( 0.0F )
	, g( 0.0F )
	, b( 0.0F )
	, a( 1.0F )
{

}

//=====================================================================================
Colour::Colour( const Colour & a_Colour )
{
	r = a_Colour.r;
	g = a_Colour.g;
	b = a_Colour.b;
	a = a_Colour.a;
}

//=====================================================================================
Colour::Colour( float a_Red, float a_Green, float a_Blue, float a_Alpha )
{
	r = a_Red;
	g = a_Green;
	b = a_Blue;
	a = a_Alpha;
}

//=====================================================================================
Colour::Colour( const ColourI & a_ColourI )
{
	r = static_cast< float >( a_ColourI.GetRed() ) / 255.0F;
	g = static_cast< float >( a_ColourI.GetGreen() ) / 255.0F;
	b = static_cast< float >( a_ColourI.GetBlue() ) / 255.0F;
	a = static_cast< float >( a_ColourI.GetAlpha() ) / 255.0F;
}

//=====================================================================================
Colour::operator ColourI()
{
	return ColourI( *this );
}

//=====================================================================================
float Colour::Luminosity( bool a_UseAlpha ) const
{
	const float S = 0.2126F;
	const float T = 0.7152F;
	const float U = 0.0722F;

	return ( S * r + T * g + U * b ) * ( a_UseAlpha ? a : 1.0F );
}

//=====================================================================================
bool Colour::operator==( const Colour & a_Other ) const
{
	return Approximate( r, a_Other.r ) &&
		   Approximate( g, a_Other.g ) &&
		   Approximate( b, a_Other.b ) &&
		   Approximate( a, a_Other.a );
}

//=====================================================================================
bool Colour::operator!=( const Colour & a_Other ) const
{
	return !( *this == a_Other );
}

//=====================================================================================
bool Colour::operator>( const Colour & a_Other ) const
{
	return Luminosity() > a_Other.Luminosity();
}

//=====================================================================================
bool Colour::operator>=( const Colour & a_Other ) const
{
	return Luminosity() >= a_Other.Luminosity();
}

//=====================================================================================
bool Colour::operator<( const Colour & a_Other ) const
{
	return Luminosity() < a_Other.Luminosity();
}

//=====================================================================================
bool Colour::operator<=( const Colour & a_Other ) const
{
	return Luminosity() <= a_Other.Luminosity();
}

//=====================================================================================
Colour & Colour::operator=( const Colour & a_Other )
{
	r = a_Other.r;
	g = a_Other.g;
	b = a_Other.b;
	a = a_Other.a;

	return *this;
}

//=====================================================================================
Colour & Colour::operator+=( const Colour & a_Other )
{
	r += a_Other.r;
	g += a_Other.g;
	b += a_Other.b;
	a += a_Other.a;

	return *this;
}

//=====================================================================================
Colour & Colour::operator-=( const Colour & a_Other )
{
	r -= a_Other.r;
	g -= a_Other.g;
	b -= a_Other.b;
	a -= a_Other.a;

	return *this;
}

//=====================================================================================
Colour & Colour::operator*=( const Colour & a_Other )
{
	r *= a_Other.r;
	g *= a_Other.g;
	b *= a_Other.b;
	a *= a_Other.a;

	return *this;
}

//=====================================================================================
Colour & Colour::operator/=( const Colour & a_Other )
{
	r /= a_Other.r;
	g /= a_Other.g;
	b /= a_Other.b;
	a /= a_Other.a;

	return *this;
}

//=====================================================================================
Colour Colour::operator+( const Colour & a_Other ) const
{
	return Colour(
		r + a_Other.r,
		g + a_Other.g,
		b + a_Other.b,
		a + a_Other.a
	);
}

//=====================================================================================
Colour Colour::operator-( const Colour & a_Other ) const
{
	return Colour(
		r - a_Other.r,
		g - a_Other.g,
		b - a_Other.b,
		a - a_Other.a
	);
}

//=====================================================================================
Colour Colour::operator*( const Colour & a_Other ) const
{
	return Colour(
		r * a_Other.r,
		g * a_Other.g,
		b * a_Other.b,
		a * a_Other.a
	);
}

//=====================================================================================
Colour Colour::operator/( const Colour & a_Other ) const
{
	return Colour(
		r / a_Other.r,
		g / a_Other.g,
		b / a_Other.b,
		a / a_Other.a
	);
}

//=====================================================================================
Colour & Colour::operator+=( float a_Scalar )
{
	r += a_Scalar;
	g += a_Scalar;
	b += a_Scalar;
	a += a_Scalar;

	return *this;
}

//=====================================================================================
Colour & Colour::operator-=( float a_Scalar )
{
	r -= a_Scalar;
	g -= a_Scalar;
	b -= a_Scalar;
	a -= a_Scalar;

	return *this;
}

//=====================================================================================
Colour & Colour::operator*=( float a_Scalar )
{
	r *= a_Scalar;
	g *= a_Scalar;
	b *= a_Scalar;
	a *= a_Scalar;

	return *this;
}

//=====================================================================================
Colour & Colour::operator/=( float a_Scalar )
{
	r /= a_Scalar;
	g /= a_Scalar;
	b /= a_Scalar;
	a /= a_Scalar;

	return *this;
}

//=====================================================================================
Colour Colour::operator+( float a_Scalar ) const
{
	return Colour(
		r + a_Scalar,
		g + a_Scalar,
		b + a_Scalar,
		a + a_Scalar
	);
}

//=====================================================================================
Colour Colour::operator-( float a_Scalar ) const
{
	return Colour(
		r - a_Scalar,
		g - a_Scalar,
		b - a_Scalar,
		a - a_Scalar
	);
}

//=====================================================================================
Colour Colour::operator*( float a_Scalar ) const
{
	return Colour(
		r * a_Scalar,
		g * a_Scalar,
		b * a_Scalar,
		a * a_Scalar
	);
}

//=====================================================================================
Colour Colour::operator/( float a_Scalar ) const
{
	return Colour(
		r / a_Scalar,
		g / a_Scalar,
		b / a_Scalar,
		a / a_Scalar
	);
}

//=====================================================================================
Colour Colour::Lerp( const Colour & a_Right, float a_T ) const
{
	return ( *this ) * ( 1.0F - a_T ) + a_Right * ( a_T );
}

CString Colour::ToString( const char * a_Parameter ) const
{
	return CString::Format( "[%.2f/%.2f/%.2f/%.2f]", r, g, b, a );
}

//=====================================================================================
ColourI::ColourI()
	: m_Colour( 255 ) // 255 = alpha only
{
}

//=====================================================================================
ColourI::ColourI( const ColourI & a_Colour )
	: m_Colour( a_Colour.m_Colour )
{
}

//=====================================================================================
ColourI::ColourI( uint8_t a_Red, uint8_t a_Green, uint8_t a_Blue, uint8_t a_Alpha )
{
	SetRed( a_Red );
	SetGreen( a_Green );
	SetBlue( a_Blue );
	SetAlpha( a_Alpha );
}

//=====================================================================================
ColourI::ColourI( const Colour & a_Colour )
{
	SetRed( static_cast< uint8_t >( Clamp( a_Colour.r ) * 255.0F ) );
	SetGreen( static_cast< uint8_t >( Clamp( a_Colour.g ) * 255.0F ) );
	SetBlue( static_cast< uint8_t >( Clamp( a_Colour.b ) * 255.0F ) );
	SetAlpha( static_cast< uint8_t >( Clamp( a_Colour.a ) * 255.0F ) );
}

//=====================================================================================
ColourI::operator Colour()
{
	return Colour( 
		static_cast< float >( GetRed() ) / 255.0F,
		static_cast< float >( GetGreen() ) / 255.0F,
		static_cast< float >( GetBlue() ) / 255.0F,
		static_cast< float >( GetAlpha() ) / 255.0F
	);
}

//=====================================================================================
bool ColourI::operator==( const ColourI & a_Other ) const
{
	return m_Colour == a_Other.m_Colour;
}

//=====================================================================================
bool ColourI::operator!=( const ColourI & a_Other ) const
{
	return m_Colour != a_Other.m_Colour;
}

//=====================================================================================
ColourI & ColourI::operator=( const ColourI & a_Other )
{
	m_Colour = a_Other.m_Colour;
	return *this;
}

//=====================================================================================
ColourI & ColourI::operator+=( const ColourI & a_Other )
{
	SetRed( GetRed() + a_Other.GetRed() );
	SetGreen( GetGreen() + a_Other.GetGreen() );
	SetBlue( GetBlue() + a_Other.GetBlue() );
	SetAlpha( GetAlpha() + a_Other.GetAlpha() );
	return *this;
}

//=====================================================================================
ColourI & ColourI::operator-=( const ColourI & a_Other )
{
	SetRed( GetRed() - a_Other.GetRed() );
	SetGreen( GetGreen() - a_Other.GetGreen() );
	SetBlue( GetBlue() - a_Other.GetBlue() );
	SetAlpha( GetAlpha() - a_Other.GetAlpha() );
	return *this;
}

//=====================================================================================
ColourI ColourI::operator+( const ColourI & a_Other ) const
{
	return ColourI( GetRed() + a_Other.GetRed(),
					GetGreen() + a_Other.GetGreen(),
					GetBlue() + a_Other.GetBlue(),
					GetAlpha() + a_Other.GetAlpha()
	);
}

//=====================================================================================
ColourI ColourI::operator-( const ColourI & a_Other ) const
{
	return ColourI( GetRed() - a_Other.GetRed(),
					GetGreen() - a_Other.GetGreen(),
					GetBlue() - a_Other.GetBlue(),
					GetAlpha() - a_Other.GetAlpha()
	);
}

//=====================================================================================
ColourI & ColourI::operator+=( uint8_t a_Scalar )
{
	SetRed( GetRed() + a_Scalar );
	SetGreen( GetGreen() + a_Scalar );
	SetBlue( GetBlue() + a_Scalar );
	SetAlpha( GetAlpha() + a_Scalar );
	return *this;
}

//=====================================================================================
ColourI & ColourI::operator-=( uint8_t a_Scalar )
{
	SetRed( GetRed() - a_Scalar );
	SetGreen( GetGreen() - a_Scalar );
	SetBlue( GetBlue() - a_Scalar );
	SetAlpha( GetAlpha() - a_Scalar );
	return *this;
}

//=====================================================================================
ColourI ColourI::operator+( uint8_t a_Scalar ) const
{
	return ColourI( GetRed() + a_Scalar,
					GetGreen() + a_Scalar,
					GetBlue() + a_Scalar,
					GetAlpha() + a_Scalar
	);
}

//=====================================================================================
ColourI ColourI::operator-( uint8_t a_Scalar ) const
{
	return ColourI( GetRed() - a_Scalar,
					GetGreen() - a_Scalar,
					GetBlue() - a_Scalar,
					GetAlpha() - a_Scalar
	);
}

//=====================================================================================
void ColourI::SetRed( uint8_t a_Value )
{
	m_Channels[ 0 ] = a_Value;
}

//=====================================================================================
void ColourI::SetGreen( uint8_t a_Value )
{
	m_Channels[ 1 ] = a_Value;
}

//=====================================================================================
void ColourI::SetBlue( uint8_t a_Value )
{
	m_Channels[ 2 ] = a_Value;
}

//=====================================================================================
void ColourI::SetAlpha( uint8_t a_Value )
{
	m_Channels[ 3 ] = a_Value;
}

//=====================================================================================
uint8_t ColourI::GetRed() const
{
	return m_Channels[ 0 ];
}

//=====================================================================================
uint8_t ColourI::GetGreen() const
{
	return m_Channels[ 1 ];
}

//=====================================================================================
uint8_t ColourI::GetBlue() const
{
	return m_Channels[ 2 ];
}

//=====================================================================================
uint8_t ColourI::GetAlpha() const
{
	return m_Channels[ 3 ];
}