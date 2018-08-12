#include "VectorEllipse.hpp"

//=====================================================================================
bool VectorEllipse::Generate( uint32_t a_NumVertices, float * o_Vertices, uint32_t & o_NumVerticesWritten,
							  uint32_t a_NumIndices, uint32_t * o_Indices, uint32_t & o_NumIndicesWritten ) const
{
	const float & mi = Abs( m_Properties.MinRadius );
	const float & ma = Abs( m_Properties.MaxRadius );

	if ( !ASSERT( !( a_NumVertices < ( 1 + 3 ) || 
		             a_NumIndices < ( 3 * 3 ) ), "VectorEllipse:: \"Cannot represent an ellipse with less than 4 vertices or 9 indices!\"" ) )
	{
		return false;
	}

	if ( !ASSERT( a_NumIndices >= ( a_NumVertices - 1 ) * 3, "VectorEllipse:: \"Incorrect number of indices, there must be at least ((a_NumVertices-1)*3)\"" ) )
	{
		return false;
	}
	
	const Colour colour = GetColour();
		
	o_Vertices[ 0 ] = 0.0F;
	o_Vertices[ 1 ] = 0.0F;
	BCopy( &colour.r, o_Vertices + 2, sizeof( float ) * 4 );

	uint32_t vertIndex = 1;
	const float sig = PI2 / static_cast< float >( a_NumVertices - 1 );
	for ( float phi = 0.0F; phi < PI2; phi += sig )
	{
		const float cos = Cos( phi );
		const float sin = Sin( phi );
		
		o_Vertices[ vertIndex * 6 + 0 ] = cos * mi;
		o_Vertices[ vertIndex * 6 + 1 ] = sin * ma;
		BCopy( &colour.r, o_Vertices + ( vertIndex * 6 + 2 ), sizeof( float ) * 4 );

		++vertIndex;
	}

	for ( uint32_t ind = 0; ind < ( a_NumIndices / 3 ); ++ind )
	{
		o_Indices[ ind * 3 + 0 ] = 0;
		o_Indices[ ind * 3 + 1 ] = ind + 1;
		o_Indices[ ind * 3 + 2 ] = ( ( ind + 2 ) == ( ( a_NumIndices / 3 ) + 1 ) ) ? 1 : ( ind + 2 );
	}

	o_NumVerticesWritten = vertIndex;
	o_NumIndicesWritten = ( vertIndex - 1 ) * 3;

	return true;
}

//=====================================================================================
VectorShape::ShapeType VectorEllipse::GetShapeType() const
{
	return VectorShape::ELLIPSE;
}

//=====================================================================================
const VectorEllipse::Properties & VectorEllipse::GetProperties() const
{
	return m_Properties;
}