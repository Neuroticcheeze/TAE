#pragma once
#ifndef GLMESH_H
#define GLMESH_H

#include "PrecompiledHeaderGL.hpp"
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
class GLMesh : public GLObj< GLMesh >
{
	friend class GLDraw;

public:

	ENUMCLASS( VertexFormat, uint8_t )
		VF_POS2,
		VF_POS2_TEX2,
		VF_POS2_COL4,
	END_ENUMCLASS( VertexFormat, uint8_t );
	
	ENUMCLASS( DrawMode, int32_t )
		DM_POINTS = GL_POINTS,
		DM_LINES = GL_LINES,
		DM_TRIANGLES = GL_TRIANGLES,
	END_ENUMCLASS( DrawMode, int32_t );

	GLMesh();
	GLMesh( const char * a_Name, VertexFormat a_Format, const Array< float > & a_VertexBuffer, const Array< uint32_t > & a_IndexBuffer, DrawMode a_DrawMode );
	~GLMesh();
	
	inline GLuint GetHandle() const 
	{ 
		return m_VaoHandle;
	}

	inline uint32_t GetIndexNum() const
	{
		return m_IndexCount;
	}

	inline GLenum GetDrawMode() const
	{
		return m_DrawMode;
	}

	inline GLenum GetIndexType() const
	{
		return GL_UNSIGNED_INT;
	}

	inline uint32_t GetVertexAttribNum() const
	{
		return m_VertexAttribNum;
	}

	static GLMesh * CreateQuad( const char * a_Name, const Vector2 & a_Min = Vector2::ZERO, const Vector2 & a_Max = Vector2::ONE );
	static GLMesh * CreateGrid( const char * a_Name, uint32_t a_Width, uint32_t a_Height, const Vector2 & a_Min = Vector2::ZERO, const Vector2 & a_Max = Vector2::ONE );

private:

	static GLint m_CurrentMeshHandle;
	uint32_t m_VertexAttribNum;
	GLuint m_VboHandle;
	GLuint m_IboHandle;
	GLuint m_VaoHandle;
	uint32_t m_IndexCount;
	GLenum m_DrawMode;
};

#endif//GLMESH_H