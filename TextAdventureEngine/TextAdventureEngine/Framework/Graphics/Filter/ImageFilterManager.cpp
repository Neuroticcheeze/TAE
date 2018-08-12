#include "ImageFilterManager.hpp"
#include <Framework/Utils/Hash.hpp>
#include "GaussianBlurFilter.hpp"

//=====================================================================================
#define MAKE_FILTER( NAME ) m_LoadedImageFilters.Put( WSID( #NAME ), Pointer< NImageFilter >( static_cast< NImageFilter* >( new NAME ) ) );

//=====================================================================================
void ImageFilterManager::InitPost()
{
	m_LoadedImageFilters.Clear();
	MAKE_FILTER( GaussianBlurFilter );
}

//=====================================================================================
ImageFilterManager::NImageFilter * ImageFilterManager::GetFilterImpl( uint32_t a_IDHash )
{
	auto filter = m_LoadedImageFilters[ a_IDHash ];
	return ( filter && *filter ) ? filter->Ptr() : nullptr;
}

#undef MAKE_FILTER