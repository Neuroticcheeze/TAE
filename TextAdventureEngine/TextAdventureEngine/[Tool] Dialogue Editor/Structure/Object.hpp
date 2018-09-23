#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Utils/Function.hpp>

//=====================================================================================
class Object;
class ViewNodePlug;

//=====================================================================================
class Ref
{
	friend class Exposer;

public:

	Ref( Object * a_Object = nullptr, uint8_t a_Idx = 0 )
		: m_Object( a_Object )
		, m_Idx( a_Idx )
	{
	}
	
	Object * GetObject() { return m_Object; }
	const Object * GetObject() const { return m_Object; }
	uint8_t GetIdx() const { return m_Idx; }

private:
	Object * m_Object;
	uint8_t m_Idx;
};

//=====================================================================================
class Exposer final
{
	friend class Object;
	friend class PageDialogueEditor;
	
public:

	Exposer * Declaration( const char * a_Name, const Colour & a_Tint );
	Exposer * Separator( const char * a_Name );
	Exposer * Field( const char * a_Name, float * a_Ref )	{ m_Fields.Put( WSID( a_Name ), { a_Ref, Pkt::OMTYPE_FLOAT, a_Name } ); return this; }
	Exposer * Field( const char * a_Name, CString * a_Ref ) { m_Fields.Put( WSID( a_Name ), { a_Ref, Pkt::OMTYPE_STRING, a_Name } ); return this; }
	Exposer * Field( const char * a_Name, Colour * a_Ref )	{ m_Fields.Put( WSID( a_Name ), { a_Ref, Pkt::OMTYPE_COLOUR, a_Name } ); return this; }
	Exposer * Output( const char * a_Name, Ref * a_RefRef )	{ m_Outputs.Put( WSID( a_Name ), { a_RefRef, Pkt::OMTYPE_REF, a_Name } ); return this; }
	Exposer * Input( const char * a_Name, Ref * a_RefRef )	{ m_Inputs.Put( WSID( a_Name ), { a_RefRef, Pkt::OMTYPE_REF, a_Name } ); return this; }

	Object * GetUnderlyingObject() const { return m_UnderlyingObject; }

private:

	struct Pkt
	{
		static const uint8_t OMTYPE_INVALID_  = 0xFF;
		static const uint8_t OMTYPE_FLOAT  = 0x00;
		static const uint8_t OMTYPE_STRING = 0x01;
		static const uint8_t OMTYPE_COLOUR = 0x02;
		static const uint8_t OMTYPE_REF = 0x04;

		void * m_OMAddress = nullptr;
		uint8_t m_OMType = OMTYPE_INVALID_;
		CString m_Name;
	};

	Dictionary< uint32_t, Pkt > m_Fields;
	Dictionary< uint32_t, Pkt > m_Outputs;
	Dictionary< uint32_t, Pkt > m_Inputs;

	CString m_Name = "";
	uint32_t m_NameHash = 0;
	Colour	m_Tint = Colour::GRAY;
	Object * m_UnderlyingObject = nullptr;
};

//=====================================================================================
class Object abstract
{
public:

	template< typename O >
	static Exposer* Construct()
	{
		O * o = new O;
		Exposer * exposer = new Exposer;
		o->Expose( exposer );
		exposer->m_UnderlyingObject = o;
		return exposer;
	}

protected:

	//static void Expose( ... );
	//void Save( ... );
	//void Load( ... );

private:


};

#endif//OBJECT_H