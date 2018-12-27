#include "VectorGraphic.hpp"
#include <Framework/Graphics/GL/GLDraw.hpp>
#include <Framework/Utils/Hash.hpp>

VectorGraphic::VectorGraphic()
	: m_PtrBatchGLMeshIndexed( nullptr )
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
	m_PtrBatchGLMeshIndexed = a_Other.m_PtrBatchGLMeshIndexed;
	a_Other.m_Owner = false;
	return *this;
}

VectorGraphic::~VectorGraphic()
{
	if ( m_Owner )
	{
		if ( m_PtrBatchGLMeshIndexed )
		{
			delete m_PtrBatchGLMeshIndexed;
			m_PtrBatchGLMeshIndexed = nullptr;
		}

		// Don't delete shader, have it stick around since the first vector graphic
	}
}

//=====================================================================================
void VectorGraphic::Generate()
{
	m_Shader = GraphicsManager::Instance().LoadShader( CRC32( "PositionColourShader" ), "colour.shader", CRC32( "Col_VColVertSrc" ), CRC32( "Col_FColVertSrc" ) );

	if ( !ASSERT( m_Owner, "VectorGraphic is not the owner to mesh data!" ) || 
		 !ASSERT_WARN( !m_PtrBatchGLMeshIndexed, "VectorGraphic failed to generate batch mesh, pre-existing mesh found, call Descruct first!" ) )
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
			shape.Generate( VFD_POS2_COL4, 32, vertices.First(), vout, 93, indices.First(), iout );
		}
		++shapeIt;
	}

	m_PtrBatchGLMeshIndexed = new GLMeshIndexed( CString::Format( "VectorGraphic_%u", size_t( this ) ).Get(), VFD_POS2_COL4, vertices, indices, GLMeshIndexed::DrawMode::DM_TRIANGLES );
}

//=====================================================================================
void VectorGraphic::Destruct()
{
	if ( !ASSERT( m_Owner, "VectorGraphic is not the owner to mesh data!" ) || 
		 !ASSERT_WARN( !m_PtrBatchGLMeshIndexed, "VectorGraphic failed to forget/destruct batch mesh, no pre-existing mesh was found, call Generate first!" ) )
	{
		return;
	}

	// Don't delete shader, have it stick around since the first vector graphic

	delete m_PtrBatchGLMeshIndexed;
	m_PtrBatchGLMeshIndexed = nullptr;
}

//=====================================================================================
void VectorGraphic::GfxDraw()
{
	if ( !ASSERT_WARN( m_PtrBatchGLMeshIndexed, "VectorGraphic failed to render, no drawable mesh data was found, call Generate first!" ) )
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
	GLDraw::Instance().DrawMesh( m_PtrBatchGLMeshIndexed );
}