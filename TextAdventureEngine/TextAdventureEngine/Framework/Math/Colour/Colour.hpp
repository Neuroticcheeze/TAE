#pragma once
#ifndef COLOUR_H
#define COLOUR_H

#include <Framework/Template/StringRepresentable.hpp>

//=====================================================================================
struct ColourI;

//=====================================================================================
struct Colour final : public StringRepresentable< Colour >
{
	Colour();
	Colour( const Colour & a_Colour );
	Colour( float a_Red, float a_Green, float a_Blue, float a_Alpha = 1.0F );
	Colour( const ColourI & a_ColourI );

	operator ColourI();

	float Luminosity( bool a_UseAlpha = false ) const;

	bool operator==( const Colour & a_Other ) const;
	bool operator!=( const Colour & a_Other ) const;
	bool operator>( const Colour & a_Other ) const;
	bool operator>=( const Colour & a_Other ) const;
	bool operator<( const Colour & a_Other ) const;
	bool operator<=( const Colour & a_Other ) const;
	Colour & operator=( const Colour & a_Other );
	Colour & operator+=( const Colour & a_Other );
	Colour & operator-=( const Colour & a_Other );
	Colour & operator*=( const Colour & a_Other );
	Colour & operator/=( const Colour & a_Other );
	Colour operator+( const Colour & a_Other ) const;
	Colour operator-( const Colour & a_Other ) const;
	Colour operator*( const Colour & a_Other ) const;
	Colour operator/( const Colour & a_Other ) const;
	Colour & operator+=( float a_Scalar );
	Colour & operator-=( float a_Scalar );
	Colour & operator*=( float a_Scalar );
	Colour & operator/=( float a_Scalar );
	Colour operator+( float a_Scalar ) const;
	Colour operator-( float a_Scalar ) const;
	Colour operator*( float a_Scalar ) const;
	Colour operator/( float a_Scalar ) const;

	CString ToString( const char * a_Parameter = "" ) const;

	static const Colour RED;
	static const Colour ORANGE;
	static const Colour YELLOW;
	static const Colour LIME;
	static const Colour GREEN;
	static const Colour CYAN;
	static const Colour COBALT;
	static const Colour VIOLET;
	static const Colour MAGENTA;
	static const Colour BLACK;
	static const Colour GRAY;
	static const Colour WHITE;
	static const Colour INVISIBLE;
	static const Colour FLAT_NORMALMAP;

	float r;
	float g;
	float b;
	float a;

	Colour Lerp( const Colour & a_Right, float a_T ) const;

	Colour & WithAlpha( float a_Alpha )
	{
		a = Max( a_Alpha, 0.0F );
		return *this;
	}

	Colour WithAlpha( float a_Alpha ) const
	{
		Colour newc = *this;
		newc.a = Max( a_Alpha, 0.0F );
		return newc;
	}

	Colour & WithSaturation( float a_Saturation )
	{
		const float f = Luminosity();
		const Colour gray( f, f, f, a );
		return *this = Lerp( gray, 1.0F - Clamp( a_Saturation ) );
	}

	const Colour WithSaturation( float a_Saturation ) const
	{
		const float f = Luminosity();
		const Colour gray( f, f, f, a );
		return Lerp( gray, 1.0F - Clamp( a_Saturation ) );
	}

	static friend Colour operator-( float a_LHS, const Colour & a_RHS )
	{
		return Colour( a_LHS - a_RHS.r, a_LHS - a_RHS.g, a_LHS - a_RHS.b, a_LHS - a_RHS.a );
	}
};

//=====================================================================================
struct ColourI final
{
	ColourI();
	ColourI( const ColourI & a_Colour );
	ColourI( uint8_t a_Red, uint8_t a_Green, uint8_t a_Blue, uint8_t a_Alpha = 255 );
	ColourI( const Colour & a_Colour );

	operator Colour();

	bool operator==( const ColourI & a_Other ) const;
	bool operator!=( const ColourI & a_Other ) const;
	ColourI & operator=( const ColourI & a_Other );
	ColourI & operator+=( const ColourI & a_Other );
	ColourI & operator-=( const ColourI & a_Other );
	ColourI operator+( const ColourI & a_Other ) const;
	ColourI operator-( const ColourI & a_Other ) const;
	ColourI & operator+=( uint8_t a_Scalar );
	ColourI & operator-=( uint8_t a_Scalar );
	ColourI operator+( uint8_t a_Scalar ) const;
	ColourI operator-( uint8_t a_Scalar ) const;

	void SetRed( uint8_t a_Value );
	void SetGreen( uint8_t a_Value );
	void SetBlue( uint8_t a_Value );
	void SetAlpha( uint8_t a_Value );

	uint8_t GetRed() const;
	uint8_t GetGreen() const;
	uint8_t GetBlue() const;
	uint8_t GetAlpha() const;

	union
	{
		uint8_t m_Channels[ 4 ];
		int32_t m_Colour;
	};
};

#endif//COLOUR_H