#pragma once
#ifndef ISAVELOADLISTENER_H
#define ISAVELOADLISTENER_H

#include <Framework/File/FileStream.hpp>

//=====================================================================================
class ISaveLoadListener abstract
{
	friend class SaveManager;

protected:

	virtual bool Save( FileStream & a_FileStream ) abstract;
	virtual bool Load( FileStream & a_FileStream ) abstract;
	virtual uint32_t GetAllocSize() const abstract;
};

#endif//ISAVELOADLISTENER_H