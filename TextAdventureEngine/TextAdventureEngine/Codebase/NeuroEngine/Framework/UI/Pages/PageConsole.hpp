#pragma once
#ifndef PAGECONSOLE_H
#define PAGECONSOLE_H

#include <Framework/UI/Page.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/UI/Views/ViewTextField.hpp>
#include <Framework/UI/Views/ViewRectangles.hpp>

//=====================================================================================
class PageConsole final : public Page
						, public View::IActionListener
{
	friend class ConsoleManager;

public:

	PageConsole();
	~PageConsole();

	bool IsBlocking() const override
	{
		return false;
	}

protected:

	void OnEnterPage() override;
	void OnExitPage() override;
	void OnTick( float a_DeltaTime ) override;
	void OnTickPost( float a_DeltaTime ) override;
	void OnTextFieldSubmit( ViewTextField & a_ViewTextField ) override;

private:

	void SetupUI();
	
	ViewRectangles  * m_ConsoleBackground;
	ViewText		* m_ConsoleLog;
	ViewTextField	* m_ConsoleInput;

	GraphicsManager::BitmapFont m_ConsoleFont; //< Font for the console text
};

#endif//PAGECONSOLE_H