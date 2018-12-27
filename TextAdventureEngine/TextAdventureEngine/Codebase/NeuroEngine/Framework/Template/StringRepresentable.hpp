#pragma once
#ifndef STRINGREPRESENTABLE_H
#define STRINGREPRESENTABLE_H

//=====================================================================================
class CString;

//=====================================================================================
template< typename T >
class StringRepresentable
{
public:

	virtual CString ToString( const char * a_Parameter = "" ) const abstract;
	virtual bool FromString( const char * a_String, T & o_Result ) const 
	{ 
		return false; 
	}
	
	static bool Parse( const char * a_String, T & o_Result )
	{
		return o_Result.FromString( a_String, o_Result );
	}
};

#endif//STRINGREPRESENTABLE_H