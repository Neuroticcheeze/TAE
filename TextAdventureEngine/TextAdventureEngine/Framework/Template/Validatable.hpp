#pragma once
#ifndef VALIDATABLE_H
#define VALIDATABLE_H

//=====================================================================================
#define CHECK_VALID if ( ! IsValid() ) return;
#define CHECK_VALID_V( RETVAL ) if ( ! IsValid() ) return RETVAL;

//=====================================================================================
class Validatable abstract
{
public:

	virtual bool IsValid() const = 0;
};

#endif//VALIDATABLE_H