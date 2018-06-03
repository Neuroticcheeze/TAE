#pragma once
#ifndef PLAYERSAVEHANDLER_H
#define PLAYERSAVEHANDLER_H

#include <Framework/File/Save/ISaveLoadListener.hpp>
#include <Framework/Template/Singleton.hpp>

class PlayerSaveHandler final : public ISaveLoadListener, 
								public Singleton< PlayerSaveHandler >
{
protected:

	bool Save( FileStream & a_FsOut )
	{
		a_FsOut.Write( 1997 );
		return true;
	}

	bool Load( FileStream & a_FsIn )
	{
		uint32_t k = 0;
		a_FsIn.Read( k );
		return true;
	}

	uint32_t GetAllocSize() const
	{
		return 512;
	}
};

#endif//PLAYERSAVEHANDLER_H