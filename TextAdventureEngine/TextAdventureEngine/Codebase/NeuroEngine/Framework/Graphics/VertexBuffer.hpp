#pragma once
#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <Framework/Graphics/GL/GLMesh.hpp>
#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Graphics/VertexFormat.hpp>

//=====================================================================================
class VertexBuffer final
{
public:

	friend class GraphicsManager;

	VertexBuffer( GLMesh::DrawMode a_DrawMode = GLMesh::DrawMode::DM_TRIANGLES, const VertexFormatDescriptor & a_Format = VFD_POS2, uint32_t a_MaxVertices = 4096 );
	VertexBuffer( VertexBuffer & a_Other );
	~VertexBuffer();

	static VertexBuffer * CreateFromGLMesh( GLMesh* a_GLMesh, uint32_t a_CustomCount = UINT_MAX )
	{
		VertexBuffer * vb = new VertexBuffer;

		vb->m_DrawMode = static_cast< GLMesh::DrawMode >( a_GLMesh->GetDrawMode() );
		vb->m_Format = a_GLMesh->GetFormat();
		vb->m_GPUBuffer = a_GLMesh;
		vb->m_Owner = false;
		vb->m_MaxVertices = Min( a_CustomCount, a_GLMesh->GetVertexNum() );

		return vb;
	}

	VertexBuffer & operator=( VertexBuffer & a_Other );

	// Builder
	VertexBuffer & PushVertex();
	VertexBuffer & VtxPosition( const float * a_Stream );
	VertexBuffer & VtxPosition( const Vector2 & a_Vector2 );
	VertexBuffer & VtxTexCoord( const float * a_Stream );
	VertexBuffer & VtxTexCoord( const Vector2 & a_Vector2 );
	VertexBuffer & VtxColour( const float * a_Stream );
	VertexBuffer & VtxColour( const Colour & a_Colour );
	
	void Build();
	void ClearBuffer();


private:

	struct { float x = 0.0F, y = 0.0F, u = 0.0F, v = 0.0F, r = 0.0F, g = 0.0F, b = 0.0F, a = 0.0F; } m_CachedVertex;
	Array< float > m_HostBuffer;
	GLMesh * m_GPUBuffer;
	bool m_Owner;
	GLMesh::DrawMode m_DrawMode;
	VertexFormatDescriptor m_Format;
	uint32_t m_MaxVertices;
};

#endif//VERTEXBUFFER_H