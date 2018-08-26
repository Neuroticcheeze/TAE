#pragma once
#ifndef MATH_H
#define MATH_H

//=====================================================================================
#define PI		3.14159265359F
#define PI2		6.28318530718F
#define EULER	2.7182818284590452353602874713527F
#define DEG2RAD 0.01745329251F
#define RAD2DEG 57.2957795131F

//=====================================================================================
template< typename T >
inline T Min( const T & a_Left, const T & a_Right )
{
	return a_Left < a_Right ? a_Left : a_Right;
}

//=====================================================================================
template< typename T >
inline T Max( const T & a_Left, const T & a_Right )
{
	return a_Left > a_Right ? a_Left : a_Right;
}

//=====================================================================================
template< typename T >
inline T Abs( const T & a_Value )
{
	return a_Value < 0 ? -a_Value : a_Value;
}

//=====================================================================================
float Exp( float a_Value );

//=====================================================================================
float Mod( float a_Value, float a_Denom );

//=====================================================================================
double ExpD( double a_Value );

//=====================================================================================
bool ModInterval( float a_T, float a_Interval, float a_TFRatio = 0.5F );

//=====================================================================================
uint32_t NextPowerOf2( uint32_t a_Value );

//=====================================================================================
uint32_t Log2( uint32_t a_Value );

//=====================================================================================
bool Approximate( float a_Left, float a_Right );

//=====================================================================================
float Sqrt( float a_Value );

//=====================================================================================
float Sin( float a_Value );

//=====================================================================================
float Cos( float a_Value );

//=====================================================================================
float Tan( float a_Value );

//=====================================================================================
float ASin( float a_Value );

//=====================================================================================
float ACos( float a_Value );

//=====================================================================================
float ATan( float a_Value );

//=====================================================================================
float ATan2F( float a_X, float a_Y );

//=====================================================================================
template< typename T >
inline T Floor( const T & a_Value )
{
	return ( T )( ( uint32_t )a_Value );
}

//=====================================================================================
template< typename T >
inline T Ceil( const T & a_Value )
{
	return Floor( a_Value ) + (T)( ( a_Value - Floor( a_Value ) ) > 0.0F ? 1 : 0 );
}

//=====================================================================================
float Round( float a_Value );

//=====================================================================================
template< typename T >
inline T Clamp( const T & a_Value, const T & a_Min, const T & a_Max )
{
	return a_Value < a_Min ? a_Min : ( a_Value > a_Max ? a_Max : a_Value );
}

//=====================================================================================
template< typename T >
T Wrap( const T & a_Value, const T & a_Min, const T & a_Max )
{
	T t = a_Value;
	T diff = a_Max - a_Min + 1;
	while ( t < a_Min )
	{
		t += diff;
	}

	while ( t > a_Max )
	{
		t -= diff;
	}

	return Clamp( t, a_Min, a_Max );
}

//=====================================================================================
inline float Clamp( const float & a_Value )
{
	return Clamp< float >( a_Value, 0.0F, 1.0F );
}

//=====================================================================================
template< typename T >
inline bool InRange( T a_Value, T a_Min, T a_Max )
{
	return a_Value >= a_Min && a_Value <= a_Max;
}

//=====================================================================================
inline bool StridedOverInterval( float a_Interval, float a_Prev, float a_Next, float * o_T = nullptr )
{
	if ( o_T )
	{
		float p = ( Ceil( a_Prev / a_Interval ) * a_Interval );
		*o_T = ( p - a_Prev ) / ( a_Next - a_Prev );
		return p <= ( a_Next );
	}
	
	return ( Ceil( a_Prev / a_Interval ) * a_Interval ) <= ( a_Next );
}

//=====================================================================================
float Pow( float a_Base, float a_Power );

//=====================================================================================
long double PowL( long double a_Base, long double a_Power );

//=====================================================================================
bool IsMultipleOf( uint32_t a_Value, uint32_t a_MultipleOf );

//=====================================================================================
bool IsMultipleOf( uint64_t a_Value, uint64_t a_MultipleOf );

//=====================================================================================
template< typename T >
bool IsEven( const T & a_Value )
{
	return a_Value % 2 == 0;
}

//=====================================================================================
template< typename T >
T NextMultipleOf( const T & a_Value, const T & a_Multiple )
{
	return ( ( a_Value + a_Multiple - 1 ) / a_Multiple ) * a_Multiple;
}


//=====================================================================================
template< typename T >
T Align( const T & a_Value, const T & a_Pow2Alignment = 256 )
{
	return ( a_Value + a_Pow2Alignment - 1 ) & -a_Pow2Alignment;
}

//=====================================================================================
template< typename T, typename U >
inline T Lerp( const T & a_Left, const T & a_Right, const U & a_T )
{
	return a_Left * ( 1.0F - a_T ) + a_Right * ( a_T );
}

//=====================================================================================
template< typename T, typename U >
inline T Lerp( const T & a_Left, const T & a_Middle, const T & a_Right, const U & a_T )
{
	T a = Clamp( 1.0F - a_T );
	T b = Clamp( 1.0F - Abs( a_T - 1.0F ) );
	T c = Clamp( a_T - 1.0F );

	return a_Left * a + a_Middle * b + a_Right * c;
}

//=====================================================================================
template< typename T >
inline T ReMap( const T & a_Value, const T & a_InMin, const T & a_InMax, const T & a_OutMin, const T & a_OutMax )
{
	T c01 = ( a_Value - a_InMin ) / ( a_InMax - a_InMin );
	T c2 = c01 * ( a_OutMax - a_OutMin ) + a_OutMin;
	return c2;
}


#endif//MATH_H