#pragma once
#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Stack.hpp>
#include <Framework/Containers/Queue.hpp>
#include <Framework/Utils/MemoryExt.hpp>
#include <Framework/Utils/Tuple.hpp>
#include <Framework/UI/FadeManager.hpp>

//=====================================================================================
class Page;

//=====================================================================================
class PageManager final : public TickableManager< PageManager >
{
public:

	void Init();
	void Finalise();
	void Tick( float a_DeltaTime = 0.0F );
	void Push( Page * a_Page );
	void Pop();
	uint32_t NumActivePages() const;
	Page * Peek() const;

	void SetFadeLayerPostOrder( FadeManager::Layer a_Layer, int32_t a_Order );
	
private:

	Stack< Page * > m_ActivePages;

	// safe page push/popping
	ENUM( PP, uint8_t ) PUSH, POP END_ENUM;
	typedef Tuple2< PP, Page * > PushPop;
	Queue< PushPop > m_PushPopEvents;
	uint32_t m_QueuedPops;

	int32_t m_FadeLayerPosts[ FadeManager::LayerCount ];
};

#endif//PAGEMANAGER_H