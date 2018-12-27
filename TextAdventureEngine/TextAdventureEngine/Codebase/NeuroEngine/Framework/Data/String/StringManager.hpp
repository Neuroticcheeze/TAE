#pragma once
#ifndef STRINGMANAGER_H
#define STRINGMANAGER_H

#include <Framework/Template/Registry.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Containers/Dictionary.hpp>
#include "StringTable.hpp"

//=====================================================================================
class StringManager final : public Registry< StringManager, CString, 1024 >
{
public:

	void DeclareStringTable( uint32_t a_StringTableId, const char * a_Path );
	void LoadStringTable( uint32_t a_StringTableId );
	void UnLoadStringTable( uint32_t a_StringTableId );
	const StringEntry & GetString( uint32_t a_StringId );

	void InsertOrUpdateDynamicEntry( uint32_t a_StringId, const char * a_String );
	void EraseDynamicEntry( uint32_t a_StringId );

private:
	Dictionary< uint32_t, StringTable > m_LoadedStringTables;
	Dictionary< uint32_t, StringEntry > m_DynamicEntries;
};

#endif//STRINGMANAGER_H