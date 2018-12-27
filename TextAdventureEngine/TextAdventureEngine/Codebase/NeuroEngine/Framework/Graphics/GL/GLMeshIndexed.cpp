#include "GLMeshIndexed.hpp"
#include "GLDraw.hpp"

//=====================================================================================
GLMeshIndexed::GLMeshIndexed()
	: GLObj( "<Unnamed>" )
	, m_DrawMode( static_cast< GLenum >( DrawMode::DM_POINTS ) )
	, m_IboHandle( 0 )
	, m_VboHandle( 0 )
	, m_VaoHandle( 0 )
	, m_IndexCount( 0 )
	, m_VertexAttribNum( 0 )
{
}

//=====================================================================================
GLMeshIndexed::GLMeshIndexed( const char * a_Name, 
				const VertexFormatDescriptor & a_Format,
				const Array< float > & a_VertexBuffer, 
				const Array< uint32_t > & a_IndexBuffer,
				DrawMode a_DrawMode )
	: GLObj( a_Name )
	, m_DrawMode( static_cast< GLenum >( a_DrawMode ) )
	, m_IboHandle( 0 )
	, m_VboHandle( 0 )
	, m_VaoHandle( 0 )
	, m_IndexCount( 0 )
	, m_VertexAttribNum( 0 )
{
	glGenVertexArrays( 1, &m_VaoHandle ); // Create our Vertex Array Object  
	glBindVertexArray( m_VaoHandle ); // Bind our Vertex Array Object so we can use it 

	glGenBuffers( 1, &m_VboHandle );
	glBindBuffer( GL_ARRAY_BUFFER, m_VboHandle ); // Bind our Vertex Buffer Object  
	glBufferData( GL_ARRAY_BUFFER, a_VertexBuffer.Count() * sizeof( float ), a_VertexBuffer.First(), GL_STATIC_DRAW ); // Set the size and data of our VBO and set it to STATIC_DRAW  
	
	if ( !ASSERT( glGetError() == GL_NO_ERROR, "Mesh Error: [VBO GEN/DATA] %s", Name() ) )
	{
		glDeleteVertexArrays( 1, &m_VaoHandle );
		glDeleteBuffers( 1, &m_VboHandle );
		m_VaoHandle = 0;
		m_VboHandle = 0;
		return;
	}

	size_t vertex = a_Format.GetVertexSize();
	m_VertexAttribNum = a_Format.GetPackedAttribs().Count();

	auto it = a_Format.GetPackedAttribs().First();
	while ( it != a_Format.GetPackedAttribs().End() )
	{
		int index = it - a_Format.GetPackedAttribs().First();
		glVertexAttribPointer( index, it->Count, it->Type, it->Normalised, vertex, ( void* )it->Offset );
		++it;
	}

	GLint maxAttribs = 0;
	glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &maxAttribs );
	for ( uint32_t k = 0; k < ( uint32_t )maxAttribs; ++k )
	{
		if ( k < m_VertexAttribNum )
		{
			glEnableVertexAttribArray( k );
		}
		else
		{
			glDisableVertexAttribArray( k );
		}
	}

	glGenBuffers( 1, &m_IboHandle );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IboHandle ); // Bind our Vertex Buffer Object  
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, a_IndexBuffer.Count() * sizeof( uint32_t ), a_IndexBuffer.First(), GL_STATIC_DRAW ); // Set the size and data of our VBO and set it to STATIC_DRAW  

	if ( !ASSERT( glGetError() == GL_NO_ERROR, "Mesh Error: [IBO GEN/DATA] %s", Name() ) )
	{
		glDeleteVertexArrays( 1, &m_VaoHandle );
		glDeleteBuffers( 1, &m_VboHandle );
		glDeleteBuffers( 1, &m_IboHandle );
		m_VaoHandle = 0;
		m_VboHandle = 0;
		m_IboHandle = 0;
		return;
	}

	glBindVertexArray( 0 );

	m_IndexCount = a_IndexBuffer.Count();
	Validate();
}

//=====================================================================================
GLMeshIndexed::~GLMeshIndexed()
{
	GLOBJ_RELEASE;

	if ( GetHandle() == GLDraw::m_CurrentMeshHandle )
	{
		GLDraw::m_CurrentMeshHandle = 0;
	}

	glDeleteBuffers( 1, &m_VboHandle );
	glDeleteBuffers( 1, &m_IboHandle );
	glDeleteVertexArrays( 1, &m_VaoHandle );
	m_VboHandle = 0;
	m_IboHandle = 0;
}

//=====================================================================================
GLMeshIndexed * GLMeshIndexed::CreateQuad( const char * a_Name, const Vector2 & a_Min, const Vector2 & a_Max )
{
	return new GLMeshIndexed( a_Name, VFD_POS2_TEX2, 
		{	a_Min.x, a_Max.y, 0.0F, 0.0F,
			a_Min.x, a_Min.y, 0.0F, 1.0F, 
			a_Max.x, a_Min.y, 1.0F, 1.0F, 
			a_Max.x, a_Max.y, 1.0F, 0.0F 
		}, 
		{ 0, 1, 2, 0, 2, 3 }, 
		DrawMode::DM_TRIANGLES );
}

//=====================================================================================
GLMeshIndexed * GLMeshIndexed::CreateGrid( const char * a_Name, uint32_t a_Width, uint32_t a_Height, const Vector2 & a_Min, const Vector2 & a_Max  )
{
#define SetVert( idx, x, y, u, v )\
	vertices[ idx * 4 + 0 ] = x;\
	vertices[ idx * 4 + 1 ] = y;\
	vertices[ idx * 4 + 2 ] = u;\
	vertices[ idx * 4 + 3 ] = v

	Array< float > vertices;
	vertices.Resize( ( a_Width + 1 ) * ( a_Height + 1 ) * 4 );

	Array< uint32_t > indices;
	indices.Resize( a_Width * a_Height * 6 );

	/*
	
	0     1     2      3
	*-----*-----*-----*
	|   / |   / |   / |
	|4/   |5/   |6/  7|
	*-----*-----*-----*
	|   / |   / |   / |
	|8/  9| / 10| /   |11
	*-----*-----*-----*
	|   / |   / |   / |
	| /   | /   | /   |
	*-----*-----*-----*
	
	*/

	for ( uint32_t vertY = 0; vertY <= a_Height; ++vertY )
	{
		for ( uint32_t vertX = 0; vertX <= a_Width; ++vertX )
		{
			uint32_t vertI = vertX + vertY * ( a_Width + 1 );
			float a = static_cast< float >( vertX ) / static_cast< float >( a_Width );
			float b = static_cast< float >( vertY ) / static_cast< float >( a_Height );

			float a2 = ReMap( a, 0.0F, 1.0F, a_Min.x, a_Max.x );
			float b2 = ReMap( b, 0.0F, 1.0F, a_Min.y, a_Max.y );

			SetVert( vertI, a2, b2, a, 1.0F - b );
		}
	}

	for ( uint32_t squareY = 0; squareY < a_Height; ++squareY )
	{
		for ( uint32_t squareX = 0; squareX < a_Width; ++squareX )
		{
			uint32_t squareI = squareX + squareY * a_Width;

			uint32_t v0 = squareX + ( a_Width + 1 ) * ( squareY );
			uint32_t v1 = squareX + ( a_Width + 1 ) * ( squareY ) + 1;
			uint32_t v2 = squareX + ( a_Width + 1 ) * ( squareY + 1 );
			uint32_t v3 = squareX + ( a_Width + 1 ) * ( squareY + 1 ) + 1;

			indices[ squareI * 6 + 0 ] = v0;
			indices[ squareI * 6 + 1 ] = v1;
			indices[ squareI * 6 + 2 ] = v2;
			indices[ squareI * 6 + 3 ] = v1;
			indices[ squareI * 6 + 4 ] = v3;
			indices[ squareI * 6 + 5 ] = v2;
		}
	}

	GLMeshIndexed * gridMesh = new GLMeshIndexed( a_Name, VFD_POS2_TEX2, vertices, indices, DrawMode::DM_TRIANGLES );
	return gridMesh;

#undef SetVert
}
