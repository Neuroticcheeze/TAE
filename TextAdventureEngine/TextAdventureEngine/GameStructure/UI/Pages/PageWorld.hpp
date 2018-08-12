#pragma once
#ifndef PAGEWORLD_H
#define PAGEWORLD_H

#include <Framework/UI/Page.hpp>
#include <Framework/UI/Views/ViewText.hpp>
#include <Framework/UI/Views/ViewSprite.hpp>

//=====================================================================================
class PageWorld final : public Page
{
public:
	PageWorld()
		: Page( "WORLD" )
	{}

protected:

	void OnEnterPage() override;
	void OnExitPage() override;
	void OnTick( float a_DeltaTime ) override;

private:

};

#endif//PAGEWORLD_H