#pragma once
#ifndef VECTORSHAPE_H
#define VECTORSHAPE_H

#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Graphics/GL/GLMesh.hpp>

//=====================================================================================
class VectorShape
{
public:

	VectorShape( const Colour & a_Colour = Colour::WHITE, const float * a_Transform = nullptr );
	
	ENUM( ShapeType, uint8_t )
		LINE,
		NGON,
		ELLIPSE,
		SEMICIRCLE,
		CRESCENT,
		ALMOND,
	END_ENUM;

	virtual ShapeType GetShapeType() const abstract;

	bool Generate( GLMesh::VertexFormat a_Format, 
				   uint32_t a_NumVertices, float * o_Vertices, uint32_t & o_NumVerticesWritten,
				   uint32_t a_NumIndices, uint32_t * o_Indices, uint32_t & o_NumIndicesWritten ) const
	{
		if ( ASSERT_WARN( a_Format == GLMesh::VertexFormat::VF_POS2_COL4, "%s only supports VF_POS2_COL4; skipping...", __FUNCTION__ ) )
		{
			return Generate( a_NumVertices, o_Vertices, o_NumVerticesWritten, a_NumIndices, o_Indices, o_NumIndicesWritten );
		}
	}

protected:

	const Colour & GetColour() const
	{
		return m_Colour;
	}

	virtual bool Generate( uint32_t a_NumVertices, float * o_Vertices, uint32_t & o_NumVerticesWritten,
						   uint32_t a_NumIndices, uint32_t * o_Indices, uint32_t & o_NumIndicesWritten ) const abstract;

private:

	float m_Transform[ 9 ] = 
	{
		1.0F, 0.0F, 0.0F,
		0.0F, 1.0F, 0.0F,
		0.0F, 0.0F, 1.0F,
	};

	Colour m_Colour;
};

#endif//VECTORSHAPE_H