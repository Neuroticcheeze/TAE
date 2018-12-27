#include "GPUGradient.hpp"
#include <Framework/Utils/Hash.hpp>
#include <Framework/Utils/Uuid.hpp>

//=====================================================================================
GraphicsManager::Texture GPUGradient::CreateFromGradient_Colour( const Gradient< Colour > & a_Gradient, uint32_t a_Samples )
{
	// Gradient is assumed to be between 0<-->1
	// Will produce RGBA32F texture

	float incr = 1.0F / Max( 1.0F, static_cast< float >( a_Samples - 1 ) );

	uint8_t * buffer = BAllocate< uint8_t >( a_Samples << 2 );
	uint8_t * end = buffer + ( a_Samples << 2 );
	uint8_t * iter = buffer;

	float f = 0.0F;
	for ( ; iter < end; f += incr )
	{
		ColourI col = a_Gradient.Evaluate( f );
		
		*( iter++ ) = col.GetRed();
		*( iter++ ) = col.GetGreen();
		*( iter++ ) = col.GetBlue();
		*( iter++ ) = col.GetAlpha();
	}

	CString name = CString::Format( "GPUGradient_%u", GenUniqueRND() );
	GraphicsManager::Texture myTex = GraphicsManager::Instance().LoadTexture( 
										CRC32( name.Get() ), 
										a_Samples, 1, buffer, 
										GraphicsManager::PF_RGBA8, 
										TextureFilterMode::LINEAR, 
										TextureWrapMode::CLAMP, false, TextureType::TT_2D );
	BFree( buffer );
	return myTex;
}