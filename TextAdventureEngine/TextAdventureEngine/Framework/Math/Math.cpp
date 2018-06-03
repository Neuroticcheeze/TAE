#include "Math.hpp"

#include <math.h>
#include <limits>

//=====================================================================================
float Exp( float a_Value )
{
	return expf( a_Value );
}

//=====================================================================================
double ExpD( double a_Value )
{
	return exp( a_Value );
}

//=====================================================================================
uint32_t NextPowerOf2( uint32_t a_Value )
{
	return static_cast< uint32_t >( pow( 2, ceil( log( a_Value ) / log( 2 ) ) ) );
}

//=====================================================================================
uint32_t Log2( uint32_t a_Value )
{
	return static_cast< uint32_t >( log( a_Value ) / log( 2 ) );
}

//=====================================================================================
bool Approximate( float a_Left, float a_Right )
{
	return fabsf( a_Left - a_Right ) <= std::numeric_limits< float >::epsilon();
}

//=====================================================================================
float Sqrt( float a_Value )
{
	return sqrtf( a_Value );
}

//=====================================================================================
float Sin( float a_Value )
{
	return sinf( a_Value );
}

//=====================================================================================
float Cos( float a_Value )
{
	return cosf( a_Value );
}

//=====================================================================================
float Tan( float a_Value )
{
	return tanf( a_Value );
}

//=====================================================================================
float ASin( float a_Value )
{
	return asinf( a_Value );
}

//=====================================================================================
float ACos( float a_Value )
{
	return acosf( a_Value );
}

//=====================================================================================
float ATan( float a_Value )
{
	return atanf( a_Value );
}

//=====================================================================================
float ATan2F( float a_X, float a_Y )
{
	return atan2f( a_Y, a_X );
}

//=====================================================================================
float Pow( float a_Base, float a_Power )
{
	return powf( a_Base, a_Power );
}

//=====================================================================================
long double PowL( long double a_Base, long double a_Power )
{
	return powl( a_Base, a_Power );
}

//=====================================================================================
bool IsMultipleOf( uint32_t a_Value, uint32_t a_MultipleOf )
{
	return a_Value % a_MultipleOf == 0;
}

//=====================================================================================
bool IsMultipleOf( uint64_t a_Value, uint64_t a_MultipleOf )
{
	return a_Value % a_MultipleOf == 0;
}

//=====================================================================================
float Round( float a_Value )
{
	return roundf( a_Value );
}