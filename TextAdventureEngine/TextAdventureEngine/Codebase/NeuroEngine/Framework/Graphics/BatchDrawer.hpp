#pragma once
#ifndef BATCHDRAWER_H
#define BATCHDRAWER_H

#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Containers/StaticArray.hpp>

//=====================================================================================
class BatchDrawer final
{
public:

	// Queue text to be rendered through this batchdrawer.
	Vector2 DrawText( const char * a_String, const GraphicsManager::BitmapFont & a_BitmapFont, 
					  uint32_t a_StringLength = UINT_MAX, bool a_StringLengthAffectsTextAlignment = false, 
					  GraphicsManager::TextAlignment a_Horizontal = GraphicsManager::TA_LEFT, 
					  GraphicsManager::TextAlignment a_Vertical = GraphicsManager::TA_BOTTOM, 
					  GraphicsManager::TextFlags a_Flags = GraphicsManager::TF_DEFAULT );



private:

	struct Packet
	{
		float m_Top[9];
	};

	struct DrawTextPacket : Packet
	{
		CString m_String;
		GraphicsManager::BitmapFont m_BitmapFont;
		uint32_t m_StringLength;
		bool m_StringLengthAffectsTextAlignment;
	};

	StaticArray< Packet*, 128 > m_QueuedDrawTextPackets;
};

#endif//BATCHDRAWER_H