#include "GL/PrecompiledHeaderGL.hpp"
#include "VertexFormat.hpp"
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
const VertexFormatDescriptor VFD_NULL( "", {});

//=====================================================================================
const VertexFormatDescriptor VFD_POS2( "VFD_POS2", {
	{ "Position", "iPos", VAType::FLOAT, VAVecCount::V2, false },
});

//=====================================================================================
const VertexFormatDescriptor VFD_POS2_TEX2( "VFD_POS2_TEX2", {
	{ "Position", "iPos", VAType::FLOAT, VAVecCount::V2, false },
	{ "TexCoord", "iTex", VAType::FLOAT, VAVecCount::V2, false },
});

//=====================================================================================
const VertexFormatDescriptor VFD_POS2_COL4( "VFD_POS2_COL4", {
	{ "Position", "iPos", VAType::FLOAT, VAVecCount::V2, false },
	{ "Colour",   "iCol", VAType::FLOAT, VAVecCount::V4, false },
});

//=====================================================================================
const VertexFormatDescriptor VFD_POS2_TEX2_COL4( "VFD_POS2_TEX2_COL4", {
	{ "Position", "iPos", VAType::FLOAT, VAVecCount::V2, false },
	{ "TexCoord", "iTex", VAType::FLOAT, VAVecCount::V2, false },
	{ "Colour",   "iCol", VAType::FLOAT, VAVecCount::V4, false },
});


//=====================================================================================
VertexFormatDescriptor::VertexFormatDescriptor( const char * a_Name, const InitialiserList< VFAttrib > & a_Attribs )
{
	static const GLenum VATypeToGLEnum[ VATypeCount ] = 
	{
		GL_BYTE, GL_SHORT, GL_INT,
		GL_FIXED, GL_FLOAT, GL_HALF_FLOAT, GL_DOUBLE,
		GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT,
		GL_INT_2_10_10_10_REV,
		GL_UNSIGNED_INT_2_10_10_10_REV,
		GL_UNSIGNED_INT_10F_11F_11F_REV,
	};
	static const size_t VATypeToSizeT[ VATypeCount ] = 
	{
		sizeof(GLbyte), sizeof(GLshort), sizeof(GLint),
		sizeof(GLfixed), sizeof(GLfloat), sizeof(GLhalf), sizeof(GLdouble),
		sizeof(GLubyte), sizeof(GLushort), sizeof(GLuint),
		sizeof(GLuint),
		sizeof(GLuint),
		sizeof(GLuint),
	};
	static const cstr_t VATypeVecCountToGLSLTypeCStr[ VATypeCount ][ VAVecCountCount ]  =
	{
		{ "int", "ivec2", "ivec3", "ivec4" }, 
		{ "int", "ivec2", "ivec3", "ivec4" }, 
		{ "int", "ivec2", "ivec3", "ivec4" }, 
		{ "float", "vec2", "vec3", "vec4" }, 
		{ "float", "vec2", "vec3", "vec4" }, 
		{ "float", "vec2", "vec3", "vec4" },
		{ "double", "dvec2", "dvec3", "dvec4" },
		{ "uint", "uvec2", "uvec3", "uvec4" }, 
		{ "uint", "uvec2", "uvec3", "uvec4" }, 
		{ "uint", "uvec2", "uvec3", "uvec4" },
		{ "uint", "uvec2", "uvec3", "uvec4" },
		{ "uint", "uvec2", "uvec3", "uvec4" },
		{ "uint", "uvec2", "uvec3", "uvec4" },
	};

	size_t binding = 0;
	size_t numFloats = 0;

	auto it = NBegin( a_Attribs );
	while ( it != NEnd( a_Attribs ) )
	{
		m_BuiltGLSLVertDeclHeader += CString::Format( "layout(location=%u)in %s %s;", 
													  binding, VATypeVecCountToGLSLTypeCStr[(int32_t)it->_type][(uint32_t)(it->_count)],
													  it->_shadername );

		if ( it->_type == VAType::FLOAT )
		{
			++numFloats;
		}

		++binding;
		m_PackedAttribs.Append( { ((uint32_t)it->_count+1), 
								  VATypeToGLEnum[(int32_t)it->_type], 
								  (bool)(it->_normalise?GL_TRUE:GL_FALSE), 
								  m_VertexSize 
								} );
		m_VertexSize += VATypeToSizeT[(int32_t)it->_type] * ( (uint32_t)(it->_count) + 1 );
		++it;
	}

	// if equal, every binding is a float
	if ( numFloats == binding )
	{
		m_FOS_NumFloats = numFloats;
	}

	m_HashCode = CRC32( CString::Format( "%s%s%u", a_Name, m_BuiltGLSLVertDeclHeader.Get(), m_VertexSize ).Get() );
	PRINT( "VertexFormatDescriptor generated: %s[...]", a_Name );
}