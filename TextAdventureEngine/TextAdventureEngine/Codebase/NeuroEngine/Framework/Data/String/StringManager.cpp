#include "StringManager.hpp"
#include <Framework/Data/Xml/XmlManager.hpp>
#include <Framework/Data/Xml/XmlObject.hpp>
#include <Framework/File/FileManager.hpp>
#include <Framework/File/FileStream.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
void StringManager::DeclareStringTable( uint32_t a_StringTableId, const char * a_Path )
{
	FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE, "res/strings" );
	Register( a_StringTableId, FileManager::Instance().GetWorkingDirectory( a_Path ) );
}

//=====================================================================================
void StringManager::LoadStringTable( uint32_t a_StringTableId )
{
	auto path = GetManifest()[ a_StringTableId ];
	if ( path )
	{
		Array< XmlObject > strings;
		FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE );
		FileStream xmlfile = FileManager::Instance().Open( path->Get(), FileManager::READ );
		if ( xmlfile.IsOpen() )
		{
			CString xmlfileText = xmlfile.ReadText();
			XmlObject assetxml = XmlManager::Instance().ParseXml( xmlfileText.Get() );
			FileManager::Instance().Close( xmlfile );
			assetxml.FindChildren( { 
				{ XmlObject::Filter::NAME, "string" }, 
				{ XmlObject::Filter::ATTRIBUTE, "id" },
				{ XmlObject::Filter::HAS_CHILDREN } },
				strings );
		}

		if ( strings.Count() > 0 )
		{
			StringTable tbl;
			for ( uint32_t k = 0; k < strings.Count(); ++k )
			{
				CString stringId;
				CString stringValue;

				auto & string = strings[ k ];

				XmlObject::AttributeType attrType;
				auto attr = string.GetAttribute( "id", &attrType );
				if ( attrType == XmlObject::AttributeType::STRING )
				{
					stringId = attr.String;
				}

				stringValue = string[ 0U ].GetValue();

				tbl.PutEntry( CRC32( stringId.Get() ), stringValue.Get() );
			}

			m_LoadedStringTables.Put( a_StringTableId, tbl );
		}
	}
}

//=====================================================================================
void StringManager::UnLoadStringTable( uint32_t a_StringTableId )
{
	if ( m_LoadedStringTables.Contains( a_StringTableId ) )
	{
		m_LoadedStringTables.Remove( a_StringTableId );
	}
}

//=====================================================================================
const StringEntry & StringManager::GetString( uint32_t a_StringId )
{	
	const StringEntry * dynamicStr = m_DynamicEntries[ a_StringId ];
	if ( dynamicStr )
	{
		return static_cast< const StringEntry & >( *dynamicStr );
	}

	auto it = m_LoadedStringTables.GetEntries().First();
	const auto end = m_LoadedStringTables.GetEntries().End();
	while ( it != end )
	{
		const StringTable & tbl = it->Value;

		const StringEntry * str = nullptr;
		if ( ( str = &tbl[ a_StringId ] ) != &StringTable::NULL_STRING_ENTRY )
		{
			return static_cast< const StringEntry & >( *str );
		}

		++it;
	}

	return StringTable::NULL_STRING_ENTRY;
}

//=====================================================================================
void StringManager::InsertOrUpdateDynamicEntry( uint32_t a_StringId, const char * a_String )
{
	m_DynamicEntries.Put( a_StringId, StringTable::Process( a_String ) );
}

//=====================================================================================
void StringManager::EraseDynamicEntry( uint32_t a_StringId )
{
	m_DynamicEntries.Remove( a_StringId );
}