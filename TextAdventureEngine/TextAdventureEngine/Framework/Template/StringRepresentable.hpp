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
};

//=====================================================================================
template< typename T >
struct StringRepresentableStruct
{
public:

	virtual CString ToString( const char * a_Parameter = "" ) const abstract;
};

#endif//STRINGREPRESENTABLE_H