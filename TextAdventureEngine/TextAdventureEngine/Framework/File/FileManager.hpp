#pragma once
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/CString.hpp>

//=====================================================================================
class FileStream;

//=====================================================================================
class FileManager final : public Manager< FileManager >
{
public:

	enum class UserLocation
	{
		APPDATA,
		PROGRAM_FILES,
		DOCUMENTS,
		PICTURES,
		MUSIC,
		VIDEOS,
		DESKTOP,
		EXECUTABLE,
	};

	ENUM( OpenFlags, uint8_t )
		READ = 1,
		WRITE = 2,
		BINARY = 4,
		APPEND = 8,
	END_ENUM_F( OpenFlags, uint8_t );

	ENUM( AttributeFlags, uint8_t )
		DEFAULT = 0,
		HIDDEN = 1,
		READONLY = 2,
		TEMP = 4,
	END_ENUM_F( AttributeFlags, uint8_t );

	ENUM( IncludeFlags, uint8_t )
		FILES = 1,
		FOLDERS = 2,
		FILES_AND_FOLDERS = 3,
	END_ENUM_F( IncludeFlags, uint8_t );

	void Init();
	void Finalise();

	bool SetWorkingDirectory( UserLocation a_UserLocation, const char * a_AppendPath = nullptr );
	void SetWorkingDirectory( const CString & a_Path );
	const CString & GetWorkingDirectory() const;
	bool Exists( const char * a_Path ) const;
	bool Create( const char * a_Path, uint8_t a_AttributeFlags = DEFAULT );
	FileStream Open( const char * a_Path, uint8_t a_OpenFlags, uint32_t a_WriteStreamCapacity = 1024 );
	bool Close( FileStream & a_FileStream );
	bool Delete( const char * a_Path );
	Array< CString > FindAll( const char * a_Root = "", const char * a_WildCard = "*", IncludeFlags a_IncludeFlags = FILES_AND_FOLDERS, bool a_SearchSubdirs = false );

private:

	struct OpenedFile
	{
		bool m_Write, m_Append;
		void * m_Handle;
		void * m_Ptr;
		uint32_t m_PtrSize;
	};

	CString FormatPath( const char * a_Input ) const;
	CString ExpandPath( const char * a_Input ) const;

	CString m_WorkingDirectory;
	Array< OpenedFile > m_OpenedFiles;
};

#endif//FILEMANAGER_H