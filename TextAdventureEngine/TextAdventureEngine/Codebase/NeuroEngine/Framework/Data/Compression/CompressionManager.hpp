#pragma once
#ifndef COMPRESSIONMANAGER_H
#define COMPRESSIONMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Array.hpp>

//=====================================================================================
class CompressionManager final : public Manager< CompressionManager >
{
public:

	typedef uint32_t addrspace_t;
	typedef uint8_t mask_t;
#define MASK_VAL_TRUE 0x01
#define MASK_VAL_FALSE 0x00

	struct RectUInt32ValUInt8
	{
		addrspace_t XPos, YPos, Width, Height;
		uint8_t Value;
	};
	
	void Init() override;
	void InitPost() override;
	void Finalise() override;

	// Perform compression on grid data using the 'FastCompress2DMMUInt8' algorithm. Overlapping rectangles may be 
	// present in certain scenarios, however will optimally result in a fewer number of rectangles because of this.
	// To render overlaps with transparency, making use of the renderer's stencil buffer will solve this problem.
	Array< RectUInt32ValUInt8 > CompressGridToRectsUInt8( const uint8_t * a_Data, addrspace_t a_Width, addrspace_t a_Height, uint8_t a_Reserved = 0xFF );


	// Compress 2D uint8 data into series of min-max index pairs covering rectangular grouped data.
	// Overlaps may be present with two or more min-max pairs.
	// The size of o_MMOutput buffer should be = 3*a_Width*a_Height, where 
	//		B[x*3+0] is lookahead-to-next, 
	//		B[x*3+1] is min-idx, 
	//		B[x*3+2] is max-idx
	void FastCompress2DMMUInt8( const uint8_t * a_Data, addrspace_t a_Width, addrspace_t a_Height,
								mask_t * a_Mask,
								addrspace_t * o_MMOutput,
								uint8_t a_Reserved = 0xFF );

private:
};

#endif//COMPRESSIONMANAGER_H