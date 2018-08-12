#pragma once
#ifndef VECTORELLIPSE_H
#define VECTORELLIPSE_H

#include "VectorShape.hpp"

//=====================================================================================
class VectorEllipse : public VectorShape
{
public:

	struct Properties final
	{
		float MinRadius;
		float MaxRadius;
	};

public:

	VectorEllipse( const Properties & a_Properties = {}, const Colour & a_Colour = Colour::WHITE, const float * a_Transform = nullptr )
		: VectorShape( a_Colour, a_Transform )
		, m_Properties( a_Properties )
	{}

	ShapeType GetShapeType() const override;
	const Properties & GetProperties() const;

	bool Generate( uint32_t a_NumVertices, float * o_Vertices, uint32_t & o_NumVerticesWritten,
				   uint32_t a_NumIndices, uint32_t * o_Indices, uint32_t & o_NumIndicesWritten ) const;

private:

	Properties m_Properties;
};

#endif//VECTORELLIPSE_H