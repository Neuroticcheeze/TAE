#include "VectorGraphic.hpp"
#include <Framework/Graphics/GL/GLDraw.hpp>
#include <Framework/Utils/Hash.hpp>

VectorGraphic::VectorGraphic()
	: m_PtrBatchGLMesh( nullptr )
	, m_Owner( true )
	, m_Shader( nullptr )
{}

VectorGraphic::VectorGraphic( VectorGraphic & a_Other )
	: VectorGraphic()
{
	*this = a_Other;
}

VectorGraphic & VectorGraphic::operator=( VectorGraphic & a_Other )
{
	this->~VectorGraphic();

	m_Owner = true;
	Shapes = a_Other.Shapes;
	m_PtrBatchGLMesh = a_Other.m_PtrBatchGLMesh;
	a_Other.m_Owner = false;
	return *this;
}

VectorGraphic::~VectorGraphic()
{
	if ( m_Owner )
	{
		if ( m_PtrBatchGLMesh )
		{
			delete m_PtrBatchGLMesh;
			m_PtrBatchGLMesh = nullptr;
		}

		// Don't delete shader, have it stick around since the first vector graphic
	}
}

//=====================================================================================
void VectorGraphic::Generate()
{
	m_Shader = GraphicsManager::Instance().LoadShader( WSID( "PositionColourShader" ), "colour.shader", WSID( "Col_VColVertSrc" ), WSID( "Col_FColVertSrc" ) );

	if ( !ASSERT( m_Owner, "VectorGraphic is not the owner to mesh data!" ) || 
		 !ASSERT_WARN( !m_PtrBatchGLMesh, "VectorGraphic failed to generate batch mesh, pre-existing mesh found, call Descruct first!" ) )
	{
		return;
	}

	Array< float > vertices( 0.0F, 32 * 6 );
	Array< uint32_t > indices( 0, 93 );

	auto shapeIt = Shapes.First();
	const auto shapeEnd = Shapes.End();
	while ( shapeIt != shapeEnd )
	{
		if ( *shapeIt )
		{
			VectorShape & shape = **shapeIt;

			uint32_t vout, iout;
			shape.Generate( GLMesh::VertexFormat::VF_POS2_COL4, 32, vertices.First(), vout, 93, indices.First(), iout );
		}
		++shapeIt;
	}

	m_PtrBatchGLMesh = new GLMesh( CString().Format( "VectorGraphic_%u", size_t( this ) ).Get(), GLMesh::VertexFormat::VF_POS2_COL4, vertices, indices, GLMesh::DrawMode::DM_TRIANGLES );
}

//=====================================================================================
void VectorGraphic::Destruct()
{
	if ( !ASSERT( m_Owner, "VectorGraphic is not the owner to mesh data!" ) || 
		 !ASSERT_WARN( !m_PtrBatchGLMesh, "VectorGraphic failed to forget/destruct batch mesh, no pre-existing mesh was found, call Generate first!" ) )
	{
		return;
	}

	// Don't delete shader, have it stick around since the first vector graphic

	delete m_PtrBatchGLMesh;
	m_PtrBatchGLMesh = nullptr;
}

//=====================================================================================
void VectorGraphic::GfxDraw()
{
	if ( !ASSERT_WARN( m_PtrBatchGLMesh, "VectorGraphic failed to render, no drawable mesh data was found, call Generate first!" ) )
	{
		return;
	}

	GraphicsManager::Instance().SetShaderGlobalColour( m_Shader, 
													   GraphicsManager::Instance().GetShaderGlobalLocation( m_Shader, "uColour" ), 
													   GraphicsManager::Instance().GetColour( GraphicsManager::ColourUsage::COL_PRIMARY ) );
	
	GraphicsManager::Instance().SetShaderGlobalFloat3x3( m_Shader,
														 GraphicsManager::Instance().GetShaderGlobalLocation( m_Shader, "uTransform" ), 
														 GraphicsManager::Instance().TfGetTop() );

	GraphicsManager::Instance().SetShader( m_Shader );
	GLDraw::Instance().DrawMesh( m_PtrBatchGLMesh );
}