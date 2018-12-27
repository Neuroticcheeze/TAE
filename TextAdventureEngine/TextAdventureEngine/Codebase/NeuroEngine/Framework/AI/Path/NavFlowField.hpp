#pragma once
#ifndef NAVFLOWFIELD_H
#define	NAVFLOWFIELD_H

#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Data/Compression/CompressionManager.hpp>

//=====================================================================================
class VertexBuffer;

//=====================================================================================
//!	\brief Represents a uniform grid of weights and 8-directional navigation layer 
//!		   converging on a single point on the grid - to be used by a large 
//!		   number of agents.
class NavFlowField
{
public:

	NavFlowField();
	~NavFlowField();

	void SetInputData( const uint8_t * a_CostField, uint32_t a_Width, uint32_t a_Height );
	void SetDestination( uint32_t a_XPos, uint32_t a_YPos );
	void Rebuild();

	//!	\brief Navigate along the flow field.
	//!	\param io_XPos The x-coordinate that will be moved.
	//!	\param io_YPos The y-coordinate that will be moved.
	//!	\return Returns whether or not the resulting xy is at the destination.
	bool Move( uint32_t & io_XPos, uint32_t & io_YPos ) const;
	Vector2 MoveVelocity( const Vector2 & a_LocalPosition ) const;

	void Draw(); //const;

private:

	// Dimensions of the flowfield and the destination
	uint32_t m_Width = 0, m_Height = 0;
	uint32_t m_DestX = 0, m_DestY = 0;

	// Input data that the flow field gets built from.
	uint8_t * m_CostField = nullptr;

	// The integration field
	uint8_t * m_IntegrationField = nullptr;

	// The final 8-dir field
	uint8_t * m_FlowField = nullptr;

	// Array of costs represented as set of rectangles for optimised rendering.
	Array< CompressionManager::RectUInt32ValUInt8 > m_OptimisedCostData;

	// Vertex buffer for visualising the flow field.
	VertexBuffer * m_Gfx = nullptr;
};

#endif//NAVFLOWFIELD_H