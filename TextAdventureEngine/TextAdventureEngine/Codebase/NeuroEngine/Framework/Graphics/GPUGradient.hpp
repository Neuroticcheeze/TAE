#pragma once
#ifndef GPUGRADIENT_H
#define GPUGRADIENT_H

#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Math/Gradient.hpp>
#include <Framework/Math/Colour/Colour.hpp>

//=====================================================================================
class GPUGradient final
{
public:

	template< typename T >
	static GraphicsManager::Texture CreateFromGradient( const Gradient< T > & a_Gradient, uint32_t a_Samples ) 
	{
		ASSERT( false, __FUNCTION__ ": No support for this type of gradient!" );
		return GraphicsManager::Texture();
	}

#define Decl_CreateFromGradient_T( T )\
	template<>\
	static GraphicsManager::Texture CreateFromGradient< T >( const Gradient< T > & a_Gradient, uint32_t a_Samples ) {\
		return CreateFromGradient_##T( a_Gradient, a_Samples ); }\
	private:\
	static GraphicsManager::Texture CreateFromGradient_##T( const Gradient< T > & a_Gradient, uint32_t a_Samples );\
	public:\
	//////////
	


	Decl_CreateFromGradient_T( Colour );



	//////////
#undef Decl_CreateFromGradient_T
};

#endif//GPUGRADIENT_H