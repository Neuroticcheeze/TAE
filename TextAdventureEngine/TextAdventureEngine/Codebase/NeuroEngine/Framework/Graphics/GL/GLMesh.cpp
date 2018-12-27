#include "GLMesh.hpp"
#include "GLDraw.hpp"

//=====================================================================================
GLMesh::GLMesh()
	: GLObj( "<Unnamed>" )
	, m_DrawMode( static_cast< GLenum >( DrawMode::DM_POINTS ) )
	, m_VboHandle( 0 )
	, m_VertexAttribNum( 0 )
	, m_VertexNum( 0 )
	, m_Format( VFD_NULL )
{
}

//=====================================================================================
GLMesh::GLMesh( const char * a_Name, 
				const VertexFormatDescriptor & a_Format,
				const Array< float > & a_VertexBuffer, 
				DrawMode a_DrawMode )
	: GLObj( a_Name )
	, m_Format( a_Format )
	, m_DrawMode( static_cast< GLenum >( a_DrawMode ) )
	, m_VboHandle( 0 )
	, m_VaoHandle( 0 )
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

	glBindVertexBuffer( 0, m_VboHandle, 0, vertex );

	auto it = a_Format.GetPackedAttribs().First();
	while ( it != a_Format.GetPackedAttribs().End() )
	{
		int index = it - a_Format.GetPackedAttribs().First();
		glVertexAttribFormat( index, it->Count, it->Type, it->Normalised, it->Offset );
	    glVertexAttribBinding( index, 0 );
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

	m_VertexNum = a_VertexBuffer.Count() * vertex;

	glBindVertexArray( 0 );

	Validate();
}

//=====================================================================================
GLMesh::GLMesh( const char * a_Name, 
				const VertexFormatDescriptor & a_Format, 
				uint32_t a_Capacity, 
				DrawMode a_DrawMode )
	: GLObj( a_Name )
	, m_Format( a_Format )
	, m_DrawMode( static_cast< GLenum >( a_DrawMode ) )
	, m_VboHandle( 0 )
	, m_VaoHandle( 0 )
	, m_VertexAttribNum( 0 )
{
	size_t vertex = a_Format.GetVertexSize();
	m_VertexAttribNum = a_Format.GetPackedAttribs().Count();

	glGenVertexArrays( 1, &m_VaoHandle ); // Create our Vertex Array Object  
	glBindVertexArray( m_VaoHandle ); // Bind our Vertex Array Object so we can use it 

	glGenBuffers( 1, &m_VboHandle );
	glBindBuffer( GL_ARRAY_BUFFER, m_VboHandle ); // Bind our Vertex Buffer Object  
	glBufferData( GL_ARRAY_BUFFER, a_Capacity * vertex * sizeof( float ), nullptr, GL_DYNAMIC_DRAW ); // Set the size of our VBO and set it to DYNAMIC_DRAW  
	
	if ( !ASSERT( glGetError() == GL_NO_ERROR, "Mesh Error: [VBO GEN/DATA] %s", Name() ) )
	{
		glDeleteVertexArrays( 1, &m_VaoHandle );
		glDeleteBuffers( 1, &m_VboHandle );
		m_VaoHandle = 0;
		m_VboHandle = 0;
		return;
	}

	glBindVertexBuffer( 0, m_VboHandle, 0, vertex );

	auto it = a_Format.GetPackedAttribs().First();
	while ( it != a_Format.GetPackedAttribs().End() )
	{
		int index = it - a_Format.GetPackedAttribs().First();
		glVertexAttribFormat( index, it->Count, it->Type, it->Normalised, it->Offset );
	    glVertexAttribBinding( index, 0 );
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

	m_VertexNum = a_Capacity;

	glBindVertexArray( 0 );

	Validate();
}

//=====================================================================================
GLMesh::~GLMesh()
{
	GLOBJ_RELEASE;

	if ( GetHandle() == GLDraw::m_CurrentMeshHandle )
	{
		GLDraw::m_CurrentMeshHandle = 0;
	}

	glDeleteBuffers( 1, &m_VboHandle );
	glDeleteVertexArrays( 1, &m_VaoHandle );
	m_VboHandle = 0;
	m_VaoHandle = 0;
}

//=====================================================================================
void GLMesh::WriteBuffer( const Array< float > & a_VertexData, uint32_t a_VertexOffset )
{
	size_t vertex = m_Format.GetVertexSize();

	uint32_t offset = a_VertexOffset * vertex;
	uint32_t size = a_VertexData.Count() * sizeof(float);

	glBindBuffer( GL_ARRAY_BUFFER, m_VboHandle );
	glBufferSubData( GL_ARRAY_BUFFER, offset, size, a_VertexData.First() );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

//=====================================================================================
GLMesh * GLMesh::CreateQuad( const char * a_Name, const Vector2 & a_Min, const Vector2 & a_Max )
{
	return new GLMesh( a_Name, VFD_POS2_TEX2, 
		{	a_Min.x, a_Max.y, 0.0F, 0.0F,
			a_Min.x, a_Min.y, 0.0F, 1.0F, 
			a_Max.x, a_Min.y, 1.0F, 1.0F, 
			a_Max.x, a_Max.y, 1.0F, 0.0F 
		}, 
		DrawMode::DM_TRIANGLES );
}

//=====================================================================================
GLMesh * GLMesh::CreateGrid( const char * a_Name, uint32_t a_Width, uint32_t a_Height, const Vector2 & a_Min, const Vector2 & a_Max  )
{
#define SetVert( idxv, x, y, u, v )\
	vertices[ idx ] = x;\
	vertices[ idx ] = y;\
	vertices[ idx ] = u;\
	vertices[ idx ] = v

	Array< float > vertices;
	vertices.Resize( a_Width * a_Height * 4 * 4 );

	/*
	
	0    1/4   5/8    9
	*-----*-----*-----*
	|   / |   / |   / |
	|3/  2|7/  6|11/  |10
	*-----*-----*-----*
	|   / |   / |   / |
	| /   | /   | /   |
	*-----*-----*-----*
	|   / |   / |   / |
	| /   | /   | /   |
	*-----*-----*-----*
	
	*/

	for ( uint32_t vertY = 0; vertY < a_Height; ++vertY )
	{
		for ( uint32_t vertX = 0; vertX < a_Width; ++vertX )
		{
			uint32_t squareId = vertY * a_Width + vertX;
			
			float vertx = static_cast< float >( vertX );
			float verty = static_cast< float >( vertY );
			float width = static_cast< float >( a_Width );
			float height = static_cast< float >( a_Height );


			Vector2 sqrMin01 = Vector2( ReMap( vertx + 0.0F, 0.0F, width, 0.0F, 1.0F ), ReMap( verty + 0.0F, 0.0F, height, 0.0F, 1.0F ) );
			Vector2 sqrMax01 = Vector2( ReMap( vertx + 1.0F, 0.0F, width, 0.0F, 1.0F ), ReMap( verty + 1.0F, 0.0F, height, 0.0F, 1.0F ) );
			Vector2 sqrMin = Vector2( ReMap( vertx + 0.0F, 0.0F, width, a_Min.x, a_Max.y ), ReMap( verty + 0.0F, 0.0F, height, a_Min.x, a_Max.y ) );
			Vector2 sqrMax = Vector2( ReMap( vertx + 1.0F, 0.0F, width, a_Min.x, a_Max.y ), ReMap( verty + 1.0F, 0.0F, height, a_Min.x, a_Max.y ) );

			// Vertex 1
			vertices[ ( squareId * 4 + 0 ) * 4 + 0 ] = sqrMin.x;
			vertices[ ( squareId * 4 + 0 ) * 4 + 1 ] = sqrMin.y;
			vertices[ ( squareId * 4 + 0 ) * 4 + 2 ] = sqrMin01.x;
			vertices[ ( squareId * 4 + 0 ) * 4 + 3 ] = sqrMin01.y;

			// Vertex 2
			vertices[ ( squareId * 4 + 1 ) * 4 + 0 ] = sqrMax.x;
			vertices[ ( squareId * 4 + 1 ) * 4 + 1 ] = sqrMin.y;
			vertices[ ( squareId * 4 + 1 ) * 4 + 2 ] = sqrMax01.x;
			vertices[ ( squareId * 4 + 1 ) * 4 + 3 ] = sqrMin01.y;

			// Vertex 3
			vertices[ ( squareId * 4 + 2 ) * 4 + 0 ] = sqrMax.x;
			vertices[ ( squareId * 4 + 2 ) * 4 + 1 ] = sqrMax.y;
			vertices[ ( squareId * 4 + 2 ) * 4 + 2 ] = sqrMax01.x;
			vertices[ ( squareId * 4 + 2 ) * 4 + 3 ] = sqrMax01.y;

			// Vertex 4
			vertices[ ( squareId * 4 + 3 ) * 4 + 0 ] = sqrMin.x;
			vertices[ ( squareId * 4 + 3 ) * 4 + 1 ] = sqrMax.y;
			vertices[ ( squareId * 4 + 3 ) * 4 + 2 ] = sqrMin01.x;
			vertices[ ( squareId * 4 + 3 ) * 4 + 3 ] = sqrMax01.y;
		}
	}

	GLMesh * gridMesh = new GLMesh( a_Name, VFD_POS2_TEX2, vertices, DrawMode::DM_TRIANGLES );
	return gridMesh;

#undef SetVert
}
