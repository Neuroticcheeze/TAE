#pragma once
#ifndef SERIALISABLE_H
#define SERIALISABLE_H

//=====================================================================================
class DataStream;

//=====================================================================================
template< typename T >
class Serialisable
{
public:

	virtual void Write( DataStream & a_Writer ) const = 0;
	virtual void Read( DataStream & a_Reader ) = 0;
};

#endif//SERIALISABLE_H