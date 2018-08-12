#include "Vector2.hpp"
#include <Framework/Math/Math.hpp>
#include <Framework/Containers/DataStream.hpp>
#include <Framework/Containers/CString.hpp>


#define dot( A, B ) ( A.x * B.x + A.y * B.y )
#define normalize( A ) Vector2( A ) * Vector2( 1.0F / Length() )
#define length( A ) Sqrt( x * x + y * y )
#define v( VALUE ) VALUE
#define vthis v( *this )
#define pvthis ( this )
#define make( GLM ) Vector2 result( GLM );

//=====================================================================================
const Vector2 Vector2::ZERO( 0.0F );
const Vector2 Vector2::ONE( 1.0F );
const Vector2 Vector2::ONE_NORM( 0.7071061F );
const Vector2 Vector2::RIGHT( 1.0F, 0.0F );
const Vector2 Vector2::UP( 0.0F, 1.0F );

//=====================================================================================
Vector2::Vector2()
	: x( 0.0F )
	, y( 0.0F )
{}

//=====================================================================================
Vector2::Vector2( float a_All )
	: x( a_All )
	, y( a_All )
{}

//=====================================================================================
Vector2::Vector2( float a_X, float a_Y )
	: x( a_X )
	, y( a_Y )
{}

//=====================================================================================
Vector2::Vector2( const Vector2 & a_Other )
	: x( a_Other.x )
	, y( a_Other.y )
{}

//=====================================================================================
void Vector2::Write( DataStream & a_Writer ) const
{
	a_Writer.Write( x );
	a_Writer.Write( y );
}

//=====================================================================================
void Vector2::Read( DataStream & a_Reader )
{
	a_Reader.Read( x );
	a_Reader.Read( y );
}

//=====================================================================================
CString Vector2::ToString( const char * a_Parameter ) const
{
	return CString( "FANCY" ) == a_Parameter ? 
		CString().Format( "{ %f, %f }", x, y ) :
		CString().Format( "%.2f,%.2f", x, y );
}

//=====================================================================================
bool Vector2::FromString( const char * a_String, Vector2 & o_Result ) const
{
	CString str = a_String;
	uint32_t ind = -1;
	if ( ( ind = str.Find( "," ) ) != -1 )
	{
		float x, y;
		if ( CString::Parse( str.SubString( 0, ind ).Get(), x ) &&
			 CString::Parse( str.SubString( ind + 1, str.Length() - ( ind + 1 ) ).Get(), y ) )
		{
			o_Result.x = x;
			o_Result.y = y;
			return true;
		}
	}

	return false;
}

//=====================================================================================
Vector2 Vector2::operator-() const
{
	return Vector2( -x, -y );
}

//=====================================================================================
Vector2 & Vector2::operator=( const Vector2 & a_Other )
{
	x = a_Other.x;
	y = a_Other.y;
	return *this;
}

//=====================================================================================
Vector2 Vector2::operator+( const Vector2 & a_Other ) const
{
	return Vector2( x + a_Other.x, y + a_Other.y );
}

//=====================================================================================
Vector2 Vector2::operator-( const Vector2 & a_Other ) const
{
	return Vector2( x - a_Other.x, y - a_Other.y );
}

//=====================================================================================
Vector2 Vector2::operator+( float a_Other ) const
{
	return Vector2( x + a_Other, y + a_Other );
}

//=====================================================================================
Vector2 Vector2::operator-( float a_Other ) const
{
	return Vector2( x - a_Other, y - a_Other );
}

//=====================================================================================
Vector2 & Vector2::operator+=( const Vector2 & a_Other )
{
	x += a_Other.x;
	y += a_Other.y;
	return *this;
}

//=====================================================================================
Vector2 & Vector2::operator-=( const Vector2 & a_Other )
{
	x -= a_Other.x;
	y -= a_Other.y;
	return *this;
}

//=====================================================================================
Vector2 & Vector2::operator+=( float a_Other )
{
	x += a_Other;
	y += a_Other;
	return *this;
}

//=====================================================================================
Vector2 & Vector2::operator-=( float a_Other )
{
	x -= a_Other;
	y -= a_Other;
	return *this;
}

//=====================================================================================
Vector2 Vector2::operator*( const Vector2 & a_Other ) const
{
	return Vector2( x * a_Other.x, y * a_Other.y );
}

//=====================================================================================
Vector2 Vector2::operator/( const Vector2 & a_Other ) const
{
	return Vector2( x / a_Other.x, y / a_Other.y );
}

//=====================================================================================
Vector2 Vector2::operator*( float a_Other ) const
{
	return Vector2( x * a_Other, y * a_Other );
}

//=====================================================================================
Vector2 Vector2::operator/( float a_Other ) const
{
	return Vector2( x / a_Other, y / a_Other );
}

//=====================================================================================
Vector2 & Vector2::operator*=( const Vector2 & a_Other )
{
	x *= a_Other.x;
	y *= a_Other.y;
	return *this;
}

//=====================================================================================
Vector2 & Vector2::operator/=( const Vector2 & a_Other )
{
	x /= a_Other.x;
	y /= a_Other.y;
	return *this;
}

//=====================================================================================
Vector2 & Vector2::operator*=( float a_Other )
{
	x *= a_Other;
	y *= a_Other;
	return *this;
}

//=====================================================================================
Vector2 & Vector2::operator/=( float a_Other )
{
	x /= a_Other;
	y /= a_Other;
	return *this;
}

//=====================================================================================
bool Vector2::operator==( const Vector2 & a_Other ) const
{
	return Approximate( x, a_Other.x ) &&
		   Approximate( y, a_Other.y );
}

//=====================================================================================
bool Vector2::operator!=( const Vector2 & a_Other ) const
{
	return ! ( *this == a_Other );
}

//=====================================================================================
float Vector2::Dot( const Vector2 & a_Other ) const
{
	return dot( Vector2( vthis ), v( a_Other ) );
}

//=====================================================================================
Vector2 Vector2::Normalised() const
{
	const auto & n = normalize( Vector2( vthis ) );
	return Vector2( n.x, n.y );
}

//=====================================================================================
float Vector2::Length() const
{
	return length( vthis );
}

//=====================================================================================
float Vector2::LengthSqr() const
{
	return dot( Vector2( vthis ), Vector2( vthis ) );
}

//=====================================================================================
Vector2 Vector2::SmoothLerp( const Vector2 & a_Target, float a_Frac ) const
{
	return Lerp( *this, a_Target, Clamp( a_Frac ) );
}

#undef dot
#undef normalize
#undef length
#undef v
#undef vthis
#undef pvthis