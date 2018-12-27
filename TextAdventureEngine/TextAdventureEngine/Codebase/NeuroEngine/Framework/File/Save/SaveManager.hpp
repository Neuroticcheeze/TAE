#pragma once
#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Utils/InitialiserList.hpp>
#include <Framework/Utils/Tuple.hpp>
#include "ISaveLoadListener.hpp"

//=====================================================================================
#define	SAVEVERSION_DTYPE uint32_t

//=====================================================================================
ENUMCLASS( SaveErrorCode, uint8_t )
	NO_ERROR,
	INVALID_SAVEFILE_ID,
	LOAD_FAILED_INVALID_OR_MISSING_FILE,
	LOAD_FAILED_NEWER_VERSION_FILE,
	LOAD_SUCCEEDED,
	SAVE_FAILED_NO_PERMISSIONS,
	SAVE_SUCCEEDED,
END_ENUMCLASS( SaveErrorCode, uint8_t )

//=====================================================================================
class SaveManager final : public Manager< SaveManager, const InitialiserList< Tuple3< uint32_t, const char *, ISaveLoadListener * > > &, SAVEVERSION_DTYPE >
{
public:

	void Init( const InitialiserList< Tuple3< uint32_t, const char *, ISaveLoadListener * > > & a_SaveFiles, SAVEVERSION_DTYPE a_SaveVersion );
	void Finalise();

	void RequestSave( uint32_t a_SaveFile ) { SaveDirect( a_SaveFile ); }
	void RequestLoad( uint32_t a_SaveFile ) { LoadDirect( a_SaveFile ); }

private:

	SaveErrorCode SaveDirect( uint32_t a_SaveFile );
	SaveErrorCode LoadDirect( uint32_t a_SaveFile );

	Dictionary< uint32_t, Tuple3< const char *, uint32_t, ISaveLoadListener * > > m_SaveFiles;
	SAVEVERSION_DTYPE m_SaveVersion;
};

#endif//SAVEMANAGER_H