#pragma once
#ifndef PAGEOPTIONS_H
#define PAGEOPTIONS_H

#include <Framework/UI/Page.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/UI/Views/ViewButton.hpp>
#include <Framework/UI/Views/ViewSlider.hpp>
#include <Framework/UI/Views/ViewTextField.hpp>
#include <Framework/UI/FadeManager.hpp>

//=====================================================================================
class PageOptions final : public Page
						, public View::IActionListener
						, private FadeManager::IListener
{
public:
	PageOptions()
		: Page( "OPTIONS" )
		, m_Back( new ViewButton( "Back", this ) )
		, m_AmbienceVolumeSlider( new ViewSlider( "AmbienceVolumeSlider", this ) )
		, m_AmbienceVolumeLabel( new ViewText( "AmbienceVolumeLabel", this ) )
		, m_MyTextField( new ViewTextField( "MyTextField", this ) )
	{}

	~PageOptions()
	{
		GetRootView()->DetatchChild( m_Back );
		GetRootView()->DetatchChild( m_AmbienceVolumeSlider );
		GetRootView()->DetatchChild( m_AmbienceVolumeLabel );
		GetRootView()->DetatchChild( m_MyTextField );
		delete m_Back;
		delete m_AmbienceVolumeSlider;
		delete m_AmbienceVolumeLabel;
		delete m_MyTextField;
	}

	bool IsBlocking() const override
	{
		return true;
	}

protected:

	void OnSliderValueChanged( ViewSlider & a_ViewSlider, float a_PreviousValue, float a_NewValue ) override;
	void OnButtonPress( ViewButton & a_ViewButton ) override;

	void OnEnterPage() override;
	void OnExitPage() override;
	void OnTick( float a_DeltaTime ) override;
	void OnTickPost( float a_DeltaTime ) override;

private:

	void SetupUI();
	void OnFadeBarrier( uint32_t a_FadeBarrierID );

	ViewButton	* m_Back;
	ViewSlider	* m_AmbienceVolumeSlider;
	ViewText	* m_AmbienceVolumeLabel;
	//ViewSlider	* m_GuiVolume;
	//ViewSlider	* m_MusicVolume;
	ViewTextField * m_MyTextField;
};

#endif//PAGEOPTIONS_H