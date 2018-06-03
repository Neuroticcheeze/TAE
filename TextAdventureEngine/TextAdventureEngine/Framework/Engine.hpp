#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Time/Timer.hpp>

//=====================================================================================
class Engine final : public Manager< Engine >
{
	friend class GraphicsManager;

public:

	void Init();
	void InitPost();
	void StartGame( Consumer< float > a_GameLoopEvent );
	void Finalise();
	float GetTime() const;
	float GetDeltaTime() const;
	uint32_t GetFPS() const;
	void Quit();
	inline bool RequestedQuit() const
	{
		return m_RequestedQuit;
	}

private:

	Consumer< float > m_ClientCallback = nullptr;
	static void GameLoopCallback( float a_DT );
	bool m_RequestedQuit;
	bool m_Initialised = false;
	bool m_PostInitialised = false;
	void * m_Window = nullptr;
	void * m_Renderer = nullptr;
	Timer m_GameLoopTimer = Timer( true );
};

#endif//ENGINE_H