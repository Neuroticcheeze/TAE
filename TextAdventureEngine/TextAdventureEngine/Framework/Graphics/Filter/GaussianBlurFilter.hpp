#pragma once
#ifndef GAUSSIANBLURFITLER_H
#define GAUSSIANBLURFITLER_H

#include <Framework/Graphics/Filter/ImageFilterManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
class GLMesh;

//=====================================================================================
struct GaussianBlurParameters
{
	int32_t m_iSource;
	float m_pRadius;
};

//=====================================================================================
class GaussianBlurFilter : public ImageFilterManager::NImageFilter
{
	friend class ImageFilterManager;

public:

	bool Dispatch() override;
	inline void SetParameters( const GaussianBlurParameters & a_Parameters ) { m_Parameters = a_Parameters; }
	inline GaussianBlurParameters & GetParameters() { return m_Parameters; }

private:

	GaussianBlurParameters m_Parameters;

	GraphicsManager::Shader m_HVPass;
	int32_t m_HVPassProp_BlurRadius;
	int32_t m_HVPassProp_SrcTex;
	GLMesh * m_FullQuad;

private:

	GaussianBlurFilter();
	~GaussianBlurFilter();

	GaussianBlurFilter( const GaussianBlurFilter & ) = delete;
	GaussianBlurFilter & operator=( const GaussianBlurFilter & ) = delete;
};

#endif//GAUSSIANBLURFITLER_H