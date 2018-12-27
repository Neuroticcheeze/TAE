#include "SaveManager.hpp"
#include <Framework/File/FileManager.hpp>

//=====================================================================================
void SaveManager::Init( const InitialiserList< Tuple3< uint32_t, const char *, ISaveLoadListener * > > & a_SaveFiles, SAVEVERSION_DTYPE a_SaveVersion )
{
	m_SaveVersion = a_SaveVersion;

	auto it = NBegin( a_SaveFiles );
	auto end = NEnd( a_SaveFiles );
	while ( it != end )
	{
		auto tup3 = Tuple3< const char *, uint32_t, ISaveLoadListener * >( it->Value1, NextPowerOf2( Clamp( it->Value2->GetAllocSize(), 8U, Conv_MegabyteToByte( 8U ) ) ), it->Value2 );
		
		if ( m_SaveFiles.Contains( it->Value0 ) )
		{
			( *m_SaveFiles[ it->Value0 ] ) = tup3;
		}

		else
		{
			m_SaveFiles.Put( it->Value0, tup3 );
		}
		
		++it;
	}
}

//=====================================================================================
void SaveManager::Finalise() 
{
	m_SaveFiles.GetEntries().Clear();
}

//=====================================================================================
SaveErrorCode SaveManager::SaveDirect( uint32_t a_SaveFile )
{
	auto tpl = m_SaveFiles[ a_SaveFile ];
	if ( tpl )
	{
		CString directory = FileManager::Instance().GetWorkingDirectory() + tpl->Value0;
		directory.ReplaceAll( "\\", "/" );
		Array< int32_t > slashes = directory.FindAll( "/" );
		directory = directory.SubString( 0, *slashes.Last() ) + "/";

		if ( !FileManager::Instance().Exists( directory.Get() ) )
		{
			FileManager::Instance().Create( directory.Get() );
		}

		FileStream stream = FileManager::Instance().Open( tpl->Value0, FileManager::WRITE | FileManager::BINARY, tpl->Value1 );
		stream.Write( m_SaveVersion );
		tpl->Value2->Save( stream );
		FileManager::Instance().Close( stream );
		return SaveErrorCode::SAVE_SUCCEEDED;
	}

	return SaveErrorCode::INVALID_SAVEFILE_ID;
}

//=====================================================================================
SaveErrorCode SaveManager::LoadDirect( uint32_t a_SaveFile )
{
	auto tpl = m_SaveFiles[ a_SaveFile ];
	if ( tpl )
	{
		FileStream stream = FileManager::Instance().Open( tpl->Value0, FileManager::READ | FileManager::BINARY, tpl->Value1 );
		
		SAVEVERSION_DTYPE version;
		stream.Read( version );

		if ( version != m_SaveVersion )
		{
			if ( version > m_SaveVersion )
			{
				return SaveErrorCode::LOAD_FAILED_NEWER_VERSION_FILE;
			}

			PRINT( "Save file is different version - branch load by switching listeners between versions" );
			// TODO
		}

		else
		{
			tpl->Value2->Load( stream );
		}

		FileManager::Instance().Close( stream );
	}

	return SaveErrorCode::INVALID_SAVEFILE_ID;
}