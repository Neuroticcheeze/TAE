#pragma once
#ifndef VERTEXFORMAT_H
#define VERTEXFORMAT_H

#include <Framework/Utils/InitialiserList.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/CString.hpp>

//=====================================================================================
ENUMCLASS( VAType, uint8_t )
	BYTE, SHORT, INT, 
	FIXED, FLOAT, HALF_FLOAT, DOUBLE,
	UNSIGNED_BYTE, UNSIGNED_SHORT, UNSIGNED_INT,
	INT_2_10_10_10_REV,
	UNSIGNED_INT_2_10_10_10_REV,
	UNSIGNED_INT_10F_11F_11F_REV,
END_ENUMCLASS( VAType, uint8_t );

//=====================================================================================
ENUMCLASS( VAVecCount, uint8_t )
	V1, V2, V3, V4,
END_ENUMCLASS( VAVecCount, uint8_t );

//=====================================================================================
class VertexFormatDescriptor final
{
public:

	using cstr_t = const char *;
	struct VFAttrib final { cstr_t _name; cstr_t _shadername; VAType _type; VAVecCount _count; bool _normalise; };
	struct PackedAttrib { uint8_t Count; uint32_t Type; bool Normalised; size_t Offset; };

	VertexFormatDescriptor( const char * a_Name, const InitialiserList< VFAttrib > & a_Attribs );

	const Array< PackedAttrib > & GetPackedAttribs() const { return m_PackedAttribs; }
	size_t GetVertexSize() const { return m_VertexSize; }
	const char * GetGLSLHeader() const { return m_BuiltGLSLVertDeclHeader.Get(); }

	bool GetIsFloatsOnly() const { return m_FOS_NumFloats > 0; }
	int32_t GetNumFloats() const { return GetIsFloatsOnly() ? m_FOS_NumFloats : 0; }

	bool operator!=( const VertexFormatDescriptor & a_RHS ) const { return !( *this == a_RHS ); }
	bool operator==( const VertexFormatDescriptor & a_RHS ) const
	{
		return m_HashCode == a_RHS.m_HashCode;
	}

private:

	uint32_t m_HashCode;

	Array< PackedAttrib > m_PackedAttribs;
	size_t m_VertexSize;
	CString m_BuiltGLSLVertDeclHeader;
	// Float optimisation support
	int32_t m_FOS_NumFloats = -1;
};

//=====================================================================================
extern const VertexFormatDescriptor VFD_NULL;
//
extern const VertexFormatDescriptor VFD_POS2;
extern const VertexFormatDescriptor VFD_POS2_TEX2;
extern const VertexFormatDescriptor VFD_POS2_COL4;
extern const VertexFormatDescriptor VFD_POS2_TEX2_COL4;


#endif//VERTEXFORMAT_H