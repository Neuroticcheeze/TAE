#pragma once
#ifndef CLONEABLE_H
#define CLONEABLE_H

//=====================================================================================
class CString;

//=====================================================================================
template< typename T >
class Cloneable
{
public:

	virtual T Clone() const abstract;
};

#endif//CLONEABLE_H