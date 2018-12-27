#pragma once
#ifndef VECTOR2_H
#define VECTOR2_H

#include <Framework/Template/Serialisable.hpp>
#include <Framework/Template/StringRepresentable.hpp>

//=====================================================================================
struct Vector2 final : public Serialisable< Vector2 >,
					   public StringRepresentable< Vector2 >
{
	Vector2();
	Vector2( float a_All );
	Vector2( float a_X, float a_Y );
	Vector2( const Vector2 & a_Other );

	void Write( DataStream & a_Writer ) const;
	void Read( DataStream & a_Reader );

	Vector2 & operator=( const Vector2 & a_Other );
	
	CString ToString( const char * a_Parameter = "" ) const;
	bool FromString( const char * a_String, Vector2 & o_Result ) const override;

	Vector2 operator-() const;

	Vector2 operator+( const Vector2 & a_Other ) const;
	Vector2 operator-( const Vector2 & a_Other ) const;
	Vector2 operator+( float a_Other ) const;
	Vector2 operator-( float a_Other ) const;

	Vector2 & operator+=( const Vector2 & a_Other );
	Vector2 & operator-=( const Vector2 & a_Other );
	Vector2 & operator+=( float a_Other );
	Vector2 & operator-=( float a_Other );

	Vector2 operator*( const Vector2 & a_Other ) const;
	Vector2 operator/( const Vector2 & a_Other ) const;
	Vector2 operator*( float a_Other ) const;
	Vector2 operator/( float a_Other ) const;

	Vector2 & operator*=( const Vector2 & a_Other );
	Vector2 & operator/=( const Vector2 & a_Other );
	Vector2 & operator*=( float a_Other );
	Vector2 & operator/=( float a_Other );

	bool operator==( const Vector2 & a_Other ) const;
	bool operator!=( const Vector2 & a_Other ) const;

	float Dot( const Vector2 & a_Other ) const;
	Vector2 Reflect( const Vector2 & a_In ) const;
	Vector2 Normalised() const;
	float Length() const;
	float LengthSqr() const;

	Vector2 SmoothLerp( const Vector2 & a_Target, float a_Frac ) const;

	float x;
	float y;

	static const Vector2 ZERO;
	static const Vector2 ONE;
	static const Vector2 ONE_NORM;
	static const Vector2 RIGHT;
	static const Vector2 UP;
};

#endif//VECTOR2_H