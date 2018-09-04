#pragma once
#ifndef VIEWRECTANGLES_H
#define VIEWRECTANGLES_H

#include <Framework/UI/View.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
class ViewRectangles final : public View
{
public:

	struct Info final
	{
		Vector2 Position;
		Vector2 Size;
		float Thickness;
		Colour TintA;
		Colour TintB;
	};

public:

	ViewRectangles( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr )
		: View( a_Name, a_ContainerPage, a_Parent )
	{
		SetEnabled();
	}

	Array< Info > & GetRectangleList() { return m_RectInfos; }

protected:

	void OnEnabled() override {}
	void OnDisabled() override {}
	void OnTick( float a_DeltaTime );
	
private:

	Array< Info > m_RectInfos;
};

#endif//VIEWRECTANGLES_H