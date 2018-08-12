#pragma once
#ifndef PAGEMAINMENU_H
#define PAGEMAINMENU_H

#include <Framework/UI/Page.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>
#include <Framework/UI/Views/ViewButton.hpp>
#include <Framework/UI/FadeManager.hpp>

//=====================================================================================
class PageMainMenu final : public Page
						 , public View::IActionListener
						 , private FadeManager::IListener
{
public:
	PageMainMenu()
		: Page( "MAIN_MENU" )
		, m_Title( new ViewText( "Title", this ) )
		, m_Version( new ViewText( "Version", this ) )
		, m_LoadGame( new ViewButton( "Play", this ) )
		, m_Options( new ViewButton( "Options", this ) )
		, m_Quit( new ViewButton( "Quit", this ) )
	{}

	~PageMainMenu()
	{
		GetRootView()->DetatchChild( m_Title );
		GetRootView()->DetatchChild( m_Version );
		GetRootView()->DetatchChild( m_LoadGame );
		GetRootView()->DetatchChild( m_Options );
		GetRootView()->DetatchChild( m_Quit );
		delete m_Title;
		delete m_Version;
		delete m_LoadGame;
		delete m_Options;
		delete m_Quit;
	}

protected:

	void OnButtonPress( ViewButton & a_ViewButton );

	void OnEnterPage() override;
	void OnExitPage() override;
	void OnTick( float a_DeltaTime ) override;
	void OnTickPost( float a_DeltaTime ) override;

private:

	void SetupUI();
	void OnFadeBarrier( uint32_t a_FadeBarrierID );

	ViewText	* m_Title;
	ViewText	* m_Version;
	ViewButton	* m_LoadGame;
	ViewButton	* m_Options;
	ViewButton	* m_Quit;
};

#endif//PAGEMAINMENU_H