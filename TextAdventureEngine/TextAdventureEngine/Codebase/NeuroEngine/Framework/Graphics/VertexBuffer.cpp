#include "VertexBuffer.hpp"
#include <Framework/Utils/Uuid.hpp>

//=====================================================================================
VertexBuffer::VertexBuffer( GLMesh::DrawMode a_DrawMode, const VertexFormatDescriptor & a_Format, uint32_t a_MaxVertices )
	: m_GPUBuffer( nullptr )
	, m_Owner( true )
	, m_DrawMode( a_DrawMode )
	, m_Format( a_Format )
	, m_MaxVertices( a_MaxVertices )
{
	ASSERT( a_Format.GetIsFloatsOnly(), "VertexFormatDescriptor must qualify as FOS-enabled." );
	ClearBuffer();
}

//=====================================================================================
VertexBuffer::VertexBuffer( VertexBuffer & a_Other )
	: VertexBuffer()
{
	*this = a_Other;
}

//=====================================================================================
VertexBuffer & VertexBuffer::operator=( VertexBuffer & a_Other )
{
	return *this;
	this->~VertexBuffer();

	m_Owner = true;
	m_CachedVertex = a_Other.m_CachedVertex;
	m_HostBuffer = a_Other.m_HostBuffer;
	m_GPUBuffer = a_Other.m_GPUBuffer;
	m_Format = a_Other.m_Format;
	m_DrawMode = a_Other.m_DrawMode;
	m_MaxVertices = a_Other.m_MaxVertices;
	a_Other.m_Owner = false;
}

//=====================================================================================
VertexBuffer::~VertexBuffer()
{
	if ( m_Owner )
	{
		if ( m_GPUBuffer )
		{
			Free( m_GPUBuffer );
		}
	}
}

//=====================================================================================
VertexBuffer & VertexBuffer::PushVertex()
{
	if ( m_HostBuffer.Count() >= ( m_MaxVertices * ( m_Format.GetNumFloats() ) ) ) 
	{ 
		ASSERT_WARN( false, "Failed to push another vertex! At limit of %u", m_MaxVertices ); return *this; 
	}

	if ( false ) {}
#define VFD_BRANCH( VFD ) else if ( m_Format == VFD )
	////////////////////
	VFD_BRANCH( VFD_POS2 ) {
		m_HostBuffer.Append( m_CachedVertex.x );
		m_HostBuffer.Append( m_CachedVertex.y );
	}
	VFD_BRANCH( VFD_POS2_TEX2 ) {
		m_HostBuffer.Append( m_CachedVertex.x );
		m_HostBuffer.Append( m_CachedVertex.y );
		m_HostBuffer.Append( m_CachedVertex.u );
		m_HostBuffer.Append( m_CachedVertex.v );
	}
	VFD_BRANCH( VFD_POS2_COL4 ) {
		m_HostBuffer.Append( m_CachedVertex.x );
		m_HostBuffer.Append( m_CachedVertex.y );
		m_HostBuffer.Append( m_CachedVertex.r );
		m_HostBuffer.Append( m_CachedVertex.g );
		m_HostBuffer.Append( m_CachedVertex.b );
		m_HostBuffer.Append( m_CachedVertex.a );
	}
	////////////////////
#undef VFD_BRANCH
	else ASSERT(false, "Unsupported Vertex-Format Descriptor!");

	return *this;
}

//=====================================================================================
VertexBuffer & VertexBuffer::VtxPosition( const float * a_Stream )
{
	m_CachedVertex.x = a_Stream[ 0 ];
	m_CachedVertex.y = a_Stream[ 1 ];
	return *this;
}

//=====================================================================================
VertexBuffer & VertexBuffer::VtxPosition( const Vector2 & a_Vector2 )
{
	m_CachedVertex.x = a_Vector2.x;
	m_CachedVertex.y = a_Vector2.y;
	return *this;
}

//=====================================================================================
VertexBuffer & VertexBuffer::VtxTexCoord( const float * a_Stream )
{
	m_CachedVertex.u = a_Stream[ 0 ];
	m_CachedVertex.v = a_Stream[ 1 ];
	return *this;
}

//=====================================================================================
VertexBuffer & VertexBuffer::VtxTexCoord( const Vector2 & a_Vector2 )
{
	m_CachedVertex.u = a_Vector2.x;
	m_CachedVertex.v = a_Vector2.y;
	return *this;
}

//=====================================================================================
VertexBuffer & VertexBuffer::VtxColour( const float * a_Stream )
{
	m_CachedVertex.r = a_Stream[ 0 ];
	m_CachedVertex.g = a_Stream[ 1 ];
	m_CachedVertex.b = a_Stream[ 2 ];
	m_CachedVertex.a = a_Stream[ 3 ];
	return *this;
}

//=====================================================================================
VertexBuffer & VertexBuffer::VtxColour( const Colour & a_Colour )
{
	m_CachedVertex.r = a_Colour.r;
	m_CachedVertex.g = a_Colour.g;
	m_CachedVertex.b = a_Colour.b;
	m_CachedVertex.a = a_Colour.a;
	return *this;
}

//=====================================================================================
void VertexBuffer::Build()
{
	if ( m_GPUBuffer )
	{
		Free( m_GPUBuffer );
	}

	if ( m_HostBuffer.Count() > 0 )
	{
		m_GPUBuffer = new GLMesh( CString::Format( "VertexBuffer_%u", GenUniqueRND() ).Get(), m_Format, m_HostBuffer, m_DrawMode );
	}
}

//=====================================================================================
void VertexBuffer::ClearBuffer()
{
	m_CachedVertex = decltype( m_CachedVertex )();
	m_HostBuffer.Clear();
}
