#include "FileManager.hpp"
#include "FileStream.hpp"

#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
#include <sys/stat.h>
#include <direct.h>
#include <fstream>

//=====================================================================================
void FileManager::Init()
{
}

//=====================================================================================
void FileManager::Finalise()
{
}

//=====================================================================================
bool FileManager::SetWorkingDirectory( UserLocation a_UserLocation, const char * a_AppendPath )
{
	PROFILE;

	static char path[ MAX_PATH + 1 ];

	switch ( a_UserLocation )
	{
	case FileManager::UserLocation::APPDATA:
		
		if ( SHGetSpecialFolderPathA( HWND_DESKTOP, path, CSIDL_APPDATA, FALSE ) )
		{
			m_WorkingDirectory = path;
		}
		else return false;

		break;
	case FileManager::UserLocation::PROGRAM_FILES:
		
		if ( SHGetSpecialFolderPathA( HWND_DESKTOP, path, CSIDL_PROGRAM_FILES, FALSE ) )
		{
			m_WorkingDirectory = path;
		}
		else return false;

		break;
	case FileManager::UserLocation::DOCUMENTS:

		if ( SHGetSpecialFolderPathA( HWND_DESKTOP, path, CSIDL_MYDOCUMENTS, FALSE ) )
		{
			m_WorkingDirectory = path;
		}
		else return false;

		break;
	case FileManager::UserLocation::PICTURES:

		if ( SHGetSpecialFolderPathA( HWND_DESKTOP, path, CSIDL_MYPICTURES, FALSE ) )
		{
			m_WorkingDirectory = path;
		}
		else return false;

		break;
	case FileManager::UserLocation::MUSIC:

		if ( SHGetSpecialFolderPathA( HWND_DESKTOP, path, CSIDL_MYMUSIC, FALSE ) )
		{
			m_WorkingDirectory = path;
		}
		else return false;

		break;
	case FileManager::UserLocation::VIDEOS:

		if ( SHGetSpecialFolderPathA( HWND_DESKTOP, path, CSIDL_MYVIDEO, FALSE ) )
		{
			m_WorkingDirectory = path;
		}
		else return false;

		break;
	case FileManager::UserLocation::DESKTOP:

		if ( SHGetSpecialFolderPathA( HWND_DESKTOP, path, CSIDL_DESKTOPDIRECTORY, FALSE ) )
		{
			m_WorkingDirectory = path;
		}
		else return false;

		break;
	case FileManager::UserLocation::EXECUTABLE:
		m_WorkingDirectory = "$";
		break;
	default:
		break;
	}

	// do some formatting to make it a clean valid path

	m_WorkingDirectory += "/";

	if ( a_AppendPath )
	{
		m_WorkingDirectory += a_AppendPath;
		m_WorkingDirectory += "/";
	}

	m_WorkingDirectory = FormatPath( ( const char * )m_WorkingDirectory );
	m_WorkingDirectory = ExpandPath( ( const char * )m_WorkingDirectory );

	return true;
}

//=====================================================================================
void FileManager::SetWorkingDirectory( const CString & a_Path )
{
	m_WorkingDirectory = a_Path;

	// do some formatting to make it a clean valid path

	m_WorkingDirectory += "/";

	m_WorkingDirectory = FormatPath( ( const char * )m_WorkingDirectory );
	m_WorkingDirectory = ExpandPath( ( const char * )m_WorkingDirectory );
}

const CString & FileManager::GetWorkingDirectory() const
{
	return m_WorkingDirectory;
}

//=====================================================================================
bool FileManager::Exists( const char * a_Path ) const
{
	PROFILE;

	CString str = FormatPath( ( m_WorkingDirectory + a_Path ).Get() );

	if ( str.Get()[ str.Length() - 1 ] == '/' ) // treat as a directory
	{
		DWORD dwAttrib = GetFileAttributes( str.Get() );
		return ( dwAttrib != INVALID_FILE_ATTRIBUTES &&
			   ( dwAttrib & FILE_ATTRIBUTE_DIRECTORY ) );
	}

	else // treat as a file
	{
		struct stat buffer;
		return stat( str.Get(), &buffer ) == 0;
	}
}

//=====================================================================================
bool FileManager::Create( const char * a_Path, uint8_t a_AttributeFlags )
{
	PROFILE;

	CString down = FormatPath( a_Path );
	CString currPath = "";
	CString full = ExpandPath( FormatPath( ( m_WorkingDirectory + down ).Get() ).Get() );

	if ( down.Get()[ down.Length() - 1 ] == '/' ) // treat as a directory
	{
		Array< CString > nests = full.Split( "/" );

		for ( uint32_t t = 0; t < nests.Count(); ++t )
		{
			currPath += nests[ t ] + "/";

			if ( _mkdir( currPath.Get() ) != 0 && ( errno == ENOENT ) )
			{
				return false;
			}
		}

		DWORD attribs;

		if ( a_AttributeFlags == AttributeFlags::DEFAULT )
		{
			attribs = FILE_ATTRIBUTE_NORMAL;
		}

		else
		{
			if ( a_AttributeFlags & AttributeFlags::READONLY )
			{
				attribs |= FILE_ATTRIBUTE_READONLY;
			}

			if ( a_AttributeFlags & AttributeFlags::HIDDEN )
			{
				attribs |= FILE_ATTRIBUTE_HIDDEN;
			}

			if ( a_AttributeFlags & AttributeFlags::TEMP )
			{
				attribs |= FILE_ATTRIBUTE_TEMPORARY;
			}
		}

		SetFileAttributes( currPath.Get(), attribs );

		return true;
	}

	else // treat as a file
	{
		std::ofstream newfile( full.Get() );

		if ( !newfile.is_open() )
		{
			return false;
		}

		newfile.close();

		DWORD attribs;

		if ( a_AttributeFlags == AttributeFlags::DEFAULT )
		{
			attribs = FILE_ATTRIBUTE_NORMAL;
		}

		else
		{
			if ( a_AttributeFlags & AttributeFlags::READONLY )
			{
				attribs |= FILE_ATTRIBUTE_READONLY;
			}

			if ( a_AttributeFlags & AttributeFlags::HIDDEN )
			{
				attribs |= FILE_ATTRIBUTE_HIDDEN;
			}

			if ( a_AttributeFlags & AttributeFlags::TEMP )
			{
				attribs |= FILE_ATTRIBUTE_TEMPORARY;
			}
		}

		SetFileAttributes( full.Get(), attribs );

		return true;
	}
}

//=====================================================================================
FileStream FileManager::Open( const char * a_Path, uint8_t a_OpenFlags, uint32_t a_WriteStreamCapacity )
{
	PROFILE;

	CString full = ExpandPath( FormatPath( ( m_WorkingDirectory + "/" + a_Path ).Get() ).Get() );

	CString mode = "";

	bool isWrite = false;
	bool isAppend = false;

	if ( a_OpenFlags & OpenFlags::READ )
	{
		mode += "r";
	}

	if ( a_OpenFlags & OpenFlags::APPEND )
	{
		isWrite = true;
		isAppend = true;
		mode += "a";
	}

	else if ( a_OpenFlags & OpenFlags::WRITE )
	{
		isWrite = true;
		mode += "w";
	}

	if ( a_OpenFlags & OpenFlags::BINARY )
	{
		mode += "b";
	}

	FILE * stream = nullptr;
	fopen_s( &stream, full.Get(), mode.Get() );

	if ( !stream )
	{
		return FileStream( nullptr, 0 );
	}

	char* contents = nullptr;
	uint32_t size = 0;

	fseek( stream, 0, SEEK_END );

	if ( isWrite )
	{
		contents = new char[ a_WriteStreamCapacity ];
		size = a_WriteStreamCapacity;

		BSet( contents, 0, a_WriteStreamCapacity * sizeof( char ) );
	}

	else
	{
		size = ftell( stream );

		contents = new char[ size ];

		rewind( stream );

		size = static_cast< uint32_t >( fread( contents, sizeof( char ), size, stream ) );
	}

	FileStream result( contents, size );
	result.m_FileHandle = stream;

	OpenedFile of;
	of.m_Write = isWrite;
	of.m_Append = isAppend;
	of.m_Handle = stream;
	of.m_Ptr = contents;
	of.m_PtrSize = size;

	m_OpenedFiles.Append( of );

	return result;
}

//=====================================================================================
bool FileManager::Close( FileStream & a_FileStream )
{
	PROFILE;

	OpenedFile * of = m_OpenedFiles.Find< void* >( []( void * a_Handle, const OpenedFile & a_File )
		{
			return a_File.m_Handle == a_Handle;
		},
		a_FileStream.m_FileHandle
	);

	if ( !of )
	{
		return false;
	}

	FILE * stream = static_cast< FILE* >( a_FileStream.m_FileHandle );

	if ( of->m_Write )
	{
		uint32_t size = Min( of->m_PtrSize, a_FileStream.Tell() );
		
		if ( of->m_Append )
		{
			fseek( stream, 0, SEEK_END );
		}

		fwrite( of->m_Ptr, sizeof( char ), size, stream );
	}

	bool result = fclose( stream ) == 0;

	if ( result )
	{
		delete[] of->m_Ptr;

		a_FileStream.m_FileHandle = nullptr;

		m_OpenedFiles.Remove( ( const OpenedFile * )of );

		return true;
	}

	return false;
}

//=====================================================================================
bool FileManager::Delete( const char * a_Path )
{
	PROFILE;

	CString path = ExpandPath( FormatPath( ( const char * )( m_WorkingDirectory + a_Path ) ).Get() );

	while ( Exists( path.Get() ) ) 
	{
		continue;
	}

	if ( path.Get()[ path.Length() - 1 ] == '/' ) // treat as a directory
	{
		if ( _rmdir( path.Get() ) != 0 )
		{
			return false;
		}

		return true;
	}

	else // treat as a file
	{
		return remove( path.Get() ) == 0;
	}
}

//=====================================================================================
Array< CString > FileManager::FindAll( const char * a_Root, const char * a_WildCard, IncludeFlags a_IncludeFlags, bool a_SearchSubdirs )
{
	PROFILE;

	Array< CString > result;

	{
		CString path = ExpandPath( FormatPath( ( const char * )( m_WorkingDirectory + a_Root + "/" + a_WildCard ) ).Get() );
		WIN32_FIND_DATA ffd;
		HANDLE hFind;

		hFind = FindFirstFile( path.Get(), &ffd );

		if ( hFind != INVALID_HANDLE_VALUE )
		{
			do
			{
				if ( ffd.dwFileAttributes != INVALID_FILE_ATTRIBUTES )
				{
					CString name = ffd.cFileName;
					if ( name == "." || name == ".." )
					{
						continue;
					}
					
					if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
					{
						if ( a_IncludeFlags & FOLDERS )
						{
							result.Append( name + "/" );
						}
					}

					else if ( a_IncludeFlags & FILES )
					{	
						result.Append( name );
					}
				}
			}

			while ( FindNextFile( hFind, &ffd ) != 0 );

			FindClose( hFind );
		}
	}

	if ( a_SearchSubdirs )
	{
		CString path = ExpandPath( FormatPath( ( const char * )( m_WorkingDirectory + a_Root + "/*" ) ).Get() );
		WIN32_FIND_DATA ffd;
		HANDLE hFind;

		hFind = FindFirstFile( path.Get(), &ffd );

		if ( hFind != INVALID_HANDLE_VALUE )
		{
			do
			{
				if ( ffd.dwFileAttributes != INVALID_FILE_ATTRIBUTES )
				{
					CString name = ffd.cFileName;
					if ( name == "." || name == ".." )
					{
						continue;
					}
					
					if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
					{
						CString subpath = FormatPath( ( const char * )( CString( a_Root ) + "/" + name ) ).Get();

						auto children = FindAll( subpath.Get(), a_WildCard, a_IncludeFlags, a_SearchSubdirs );

						for ( uint32_t j = 0; j < children.Count(); ++j )
						{
							const CString & child = children[ j ];

							result.Append( name + "/" + child );
						}
					}
				}
			}

			while ( FindNextFile( hFind, &ffd ) != 0 );

			FindClose( hFind );
		}
	}

	return result;
}

//=====================================================================================
CString FileManager::FormatPath( const char * a_Input ) const
{
	PROFILE;

	CString out = a_Input;

	out.ReplaceAll( "\\", "/" );

	while ( out.Find( "//" ) != -1 )
	{
		out.Replace( "//", "/" );
	}

	return out;
}

//=====================================================================================
CString FileManager::ExpandPath( const char * a_Input ) const
{
	PROFILE;

	char full[ _MAX_PATH ];
	char * tmp_ = NULL;
	if ( ( tmp_ = _fullpath( full, a_Input, _MAX_PATH ) ) != NULL )
	{
		CString tmp( tmp_ );
		if ( tmp.Length() > 1 && 
			 tmp_[ tmp.Length() - 1 ] == '\\' && 
			 tmp_[ tmp.Length() - 2 ] == '$' )
		{
			tmp_[ tmp.Length() - 2 ] = '\0';
		}
		return FormatPath( tmp_ );
	}
	
	return CString( a_Input );
}