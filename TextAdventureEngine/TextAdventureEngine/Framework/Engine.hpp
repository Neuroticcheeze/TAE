#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Time/Timer.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
class Engine final : public Manager< Engine, const char *, const Vector2 & >
{
	friend class Graphics;
	friend class GraphicsManager;

public:

	void Init( const char * a_Title, const Vector2 & a_DisplaySize );
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

	inline const Vector2 & GetDisplaySize() const
	{
		return m_DisplaySize;
	}

private:

	CString m_Title;
	Vector2 m_DisplaySize;
	Consumer< float > m_ClientCallback = nullptr;
	static void GameLoopCallback( float a_DT );
	bool m_RequestedQuit;
	bool m_Initialised = false;
	bool m_PostInitialised = false;
	void * m_Window = nullptr;
	void * m_Renderer = nullptr;
	void * m_GlCtx = nullptr;
	Timer m_GameLoopTimer = Timer( false );
};

#endif//ENGINE_H