#pragma once
#ifndef VIEWSIZER_H
#define VIEWSIZER_H

#include <Framework/UI/Views/ViewDraggable.hpp>

//=====================================================================================
class ViewSizer : public ViewDraggable
{
public:

	ViewSizer( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );
	void OnTick( float a_DeltaTime ) override;

};

#endif//VIEWSIZER_H