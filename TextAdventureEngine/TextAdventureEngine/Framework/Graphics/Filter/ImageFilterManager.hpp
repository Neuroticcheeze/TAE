#pragma once
#ifndef IMAGEFILTER_MANAGER
#define IMAGEFILTER_MANAGER

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/StaticDictionary.hpp>
#include <Framework/Utils/MemoryExt.hpp>

//=====================================================================================
class ImageFilterManager final : public Manager< ImageFilterManager >
{
public:

	template< class T >
	using FilterRef = WeakPointer< T >;

	template< class T >
	FilterRef< T > GetFilter( uint32_t a_IDHash )
	{
		return WeakPointer< T >::Grab( dynamic_cast< T* >( GetFilterImpl( a_IDHash ) ) );
	}

	class NImageFilter
	{
		friend class ImageFilterManager;

	public:
		virtual bool Dispatch() { return false; }

	protected:

	private:
	};

	union NBindPoint
	{
		int32_t m_iSource;
		int32_t m_oRTColIndex;
	};

public:

	void Init() {}
	void InitPost();
	void Finalise() {}

private:

	NImageFilter * GetFilterImpl( uint32_t a_IDHash );
	StaticDictionary< uint32_t, Pointer< NImageFilter >, 16 > m_LoadedImageFilters;
};

#endif//IMAGEFILTER_MANAGER