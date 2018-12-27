#pragma once
#ifndef GLMESH_H
#define GLMESH_H

#include "PrecompiledHeaderGL.hpp"
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Graphics/VertexFormat.hpp>

//=====================================================================================
class GLMesh : public GLObj< GLMesh >
{
	friend class GLDraw;

public:

	//ENUMCLASS( VertexFormat, uint8_t )
	//	VF_POS2,
	//	VF_POS2_TEX2,
	//	VF_POS2_COL4,
	//	VF_POS2_TEX2_COL4,
	//END_ENUMCLASS( VertexFormat, uint8_t );
	
	ENUMCLASS( DrawMode, int32_t )
		DM_POINTS = GL_POINTS,
		DM_LINES = GL_LINES,
		DM_TRIANGLES = GL_TRIANGLES,
		DM_QUADS = GL_QUADS,
	END_ENUMCLASS( DrawMode, int32_t );

	GLMesh();
	GLMesh( const char * a_Name, const VertexFormatDescriptor & a_Format, const Array< float > & a_VertexBuffer, DrawMode a_DrawMode );
	GLMesh( const char * a_Name, const VertexFormatDescriptor & a_Format, uint32_t a_Capacity, DrawMode a_DrawMode ); // Dynamic buffer
	~GLMesh();
	
	void WriteBuffer( const Array< float > & a_VertexData, uint32_t a_VertexOffset = 0 );

	inline GLuint GetHandle() const 
	{ 
		return m_VaoHandle;
	}

	inline GLenum GetDrawMode() const
	{
		return m_DrawMode;
	}

	inline uint32_t GetVertexAttribNum() const
	{
		return m_VertexAttribNum;
	}

	inline uint32_t GetVertexNum() const
	{
		return m_VertexNum;
	}

	inline const VertexFormatDescriptor & GetFormat() const
	{
		return m_Format;
	}

	static GLMesh * CreateQuad( const char * a_Name, const Vector2 & a_Min = Vector2::ZERO, const Vector2 & a_Max = Vector2::ONE );
	static GLMesh * CreateGrid( const char * a_Name, uint32_t a_Width, uint32_t a_Height, const Vector2 & a_Min = Vector2::ZERO, const Vector2 & a_Max = Vector2::ONE );

private:

	uint32_t m_VertexAttribNum;
	GLuint m_VboHandle;
	GLuint m_VaoHandle;
	GLenum m_DrawMode;
	VertexFormatDescriptor m_Format;
	uint32_t m_VertexNum;
};

#endif//GLMESH_H