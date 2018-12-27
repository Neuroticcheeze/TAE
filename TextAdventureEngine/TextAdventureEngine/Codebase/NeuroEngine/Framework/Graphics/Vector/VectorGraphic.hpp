#pragma once
#ifndef VECTORGRAPHIC_H
#define VECTORGRAPHIC_H

#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Containers/Array.hpp>
#include "VectorShape.hpp"

//=====================================================================================
class GLMeshIndexed;

//=====================================================================================
class VectorGraphic final
{
public:

	VectorGraphic();
	VectorGraphic( VectorGraphic & a_Other );
	VectorGraphic & operator=( VectorGraphic & a_Other );
	~VectorGraphic();

	void Generate();
	void Destruct();
	void GfxDraw();

	Array< VectorShape* > Shapes;

private:

	GraphicsManager::Shader m_Shader;
	GLMeshIndexed * m_PtrBatchGLMeshIndexed;
	bool m_Owner;
};

#endif//VECTORGRAPHIC_H